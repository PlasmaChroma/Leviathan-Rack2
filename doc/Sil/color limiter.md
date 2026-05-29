Dragon King Leviathan — yes. I’d implement this as an **adaptive high-frequency spectral governor**, not a hard “filter always dulling the mix.” The purpose is: *let air exist, but prevent post-saturation fizz, brittle AI sheen, and excessive top-end hash from arriving at the true-peak limiter as the final color of the module.*

Current Sil already has the right architectural pattern: feature-specific state structs, envelope followers, coefficient dividers, adaptive LEDs, and a chain where the saturator currently feeds directly into the limiter. This new stage should insert exactly between `saturatedL/R` and the final limiter measurement/output path. 

````md
# Sil: Color Limiter / Adaptive Top-End Rolloff Spec

Target: Codex 5.3  
Module: Sil  
Feature Name: Color Limiter  
Suggested UI Label: COLOR LIMIT  
Signal Chain Position: after Saturator, before final True-Peak Limiter

## 1. Intent

Add an adaptive top-end rolloff stage that prevents excessive high-frequency color from accumulating after saturation and before the final true-peak limiter.

This is not a static lowpass and not a de-esser. It is a mastering-color limiter:

- It should leave balanced/dark material mostly untouched.
- It should gently reduce excessive “air hash,” saturation fizz, brittle AI sheen, and overly bright upper spectrum.
- It should preserve stereo image by applying the same tonal correction to L/R.
- It should be slow and smooth enough to feel like mastering coloration, not vocal de-essing.
- It should expose activity through a dedicated LED brightness value.

Preferred behavior: “soft velvet ceiling over the top octave,” not “blanket over the mix.”

---

## 2. Chain Placement

Current late-chain flow:

```cpp
enhancedL/R
  -> saturator
  -> final limiter
  -> outputs
````

New flow:

```cpp
enhancedL/R
  -> saturator
  -> color limiter
  -> final true-peak limiter
  -> outputs
```

Implementation target:

```cpp
float coloredL = saturatedL;
float coloredR = saturatedR;
float colorLimiterLed = 0.f;

if (masteringEnabled) {
    // adaptive color limiter processing here
}

float outL = coloredL;
float outR = coloredR;

// final limiter must now analyze coloredL / coloredR,
// not saturatedL / saturatedR.
```

The final limiter’s previous-sample state must also track `coloredL/R`.

Replace late limiter references:

```cpp
peak = max(abs(saturatedL), abs(saturatedR));
interpL = limiterPrevL + (saturatedL - limiterPrevL) * a;
interpR = limiterPrevR + (saturatedR - limiterPrevR) * a;
outL = saturatedL * limiterGain;
outR = saturatedR * limiterGain;
limiterPrevL = saturatedL;
limiterPrevR = saturatedR;
```

with:

```cpp
peak = max(abs(coloredL), abs(coloredR));
interpL = limiterPrevL + (coloredL - limiterPrevL) * a;
interpR = limiterPrevR + (coloredR - limiterPrevR) * a;
outL = coloredL * limiterGain;
outR = coloredR * limiterGain;
limiterPrevL = coloredL;
limiterPrevR = coloredR;
```

---

## 3. Processing Model

Use a dynamic high-shelf cut, driven by an adaptive high-frequency detector.

Do not implement this as a moving lowpass by default. A moving lowpass can sound obvious and can cause zipper-like spectral motion if the cutoff moves too much. A high shelf is safer, more mastering-like, and easier to blend into the current Sil design.

Core filter:

```cpp
high shelf
frequency: 9500 Hz
slope: 0.707 to 1.0
gain: 0 dB to -2.25 dB
```

Recommended default max cut:

```cpp
kColorMaxCutDb = 2.25f;
```

Optional later “strong mode” can raise this to `3.0f`, but the default should remain subtle.

---

## 4. Detection Philosophy

The detector should answer:

> “Is the top band too loud relative to the useful upper-mid / presence band?”

Measure two spectral regions:

```cpp
reference band: 2500 Hz – 8500 Hz
air/top band: 8500 Hz+
```

The color limiter should activate when the top band is excessive relative to the reference band, with an absolute gate so silence/noise-floor material does not cause false movement.

Use L/R detection, but shared gain:

```cpp
topEnv = 0.5 * (topEnvL + topEnvR);
refEnv = 0.5 * (refEnvL + refEnvR);
```

Then apply one shared shelf cut to both L and R.

This preserves stereo imaging and avoids independent left/right brightness pumping.

---

## 5. New Constants

Add near existing mastering constants:

```cpp
static constexpr float kColorTopLowHz = 8500.f;
static constexpr float kColorRefLowHz = 2500.f;
static constexpr float kColorRefHighHz = 8500.f;
static constexpr float kColorShelfHz = 9500.f;
static constexpr float kColorShelfSlope = 0.85f;

static constexpr float kColorMaxCutDb = 2.25f;

// Normalizes expected energy difference between ref band and top band.
// Tune by ear. Higher value makes the limiter more sensitive.
static constexpr float kColorTopNormDb = 7.0f;

// Relative brightness threshold.
static constexpr float kColorExcessThresholdDb = 0.0f;
static constexpr float kColorExcessKneeDb = 6.0f;

// Absolute gate: only operate when top band is actually audible.
static constexpr float kColorTopGateDbFs = -48.f;
static constexpr float kColorTopGateKneeDb = 12.f;

// Safety trigger for very bright full-scale material.
static constexpr float kColorHarshAbsDbFs = -30.f;
static constexpr float kColorHarshAbsKneeDb = 10.f;

static constexpr float kColorEnvAttackSec = 0.015f;
static constexpr float kColorEnvReleaseSec = 0.220f;

static constexpr float kColorGainAttackSec = 0.280f;
static constexpr float kColorGainReleaseSec = 1.400f;

static constexpr int kColorLimiterCoeffDivision = 32;
```

Rationale:

* `8500 Hz+` catches air/fizz/hash.
* `2500–8500 Hz` acts as the “musical brightness” reference.
* `9500 Hz` shelf keeps the correction smooth.
* `2.25 dB` max cut is enough to tame harshness without making the module sound dull.
* Slow release prevents flutter.
* Medium attack catches accumulating top-end without acting like a transient de-esser.

---

## 6. Add High-Shelf Support to Biquad

The existing `Biquad` supports peaking filters. Add RBJ high-shelf support:

```cpp
void setHighShelf(float sampleRate, float shelfHz, float slope, float gainDb) {
    if (sampleRate <= 1.f || shelfHz <= 1.f || slope <= 1e-4f) {
        b0 = 1.f;
        b1 = b2 = a1 = a2 = 0.f;
        return;
    }

    const float nyquistGuard = 0.45f * sampleRate;
    const float fc = clamp(shelfHz, 20.f, nyquistGuard);
    const float A = std::pow(10.f, gainDb / 40.f);
    const float w0 = 2.f * M_PI * fc / sampleRate;
    const float c = std::cos(w0);
    const float s = std::sin(w0);
    const float S = std::max(slope, 1e-4f);
    const float sqrtA = std::sqrt(A);

    const float alpha = s * 0.5f * std::sqrt(std::max(
        (A + 1.f / A) * (1.f / S - 1.f) + 2.f,
        1e-6f
    ));

    const float rawB0 = A * ((A + 1.f) + (A - 1.f) * c + 2.f * sqrtA * alpha);
    const float rawB1 = -2.f * A * ((A - 1.f) + (A + 1.f) * c);
    const float rawB2 = A * ((A + 1.f) + (A - 1.f) * c - 2.f * sqrtA * alpha);

    const float rawA0 = (A + 1.f) - (A - 1.f) * c + 2.f * sqrtA * alpha;
    const float rawA1 = 2.f * ((A - 1.f) - (A + 1.f) * c);
    const float rawA2 = (A + 1.f) - (A - 1.f) * c - 2.f * sqrtA * alpha;

    const float invA0 = (std::fabs(rawA0) > 1e-9f) ? (1.f / rawA0) : 1.f;

    b0 = rawB0 * invA0;
    b1 = rawB1 * invA0;
    b2 = rawB2 * invA0;
    a1 = rawA1 * invA0;
    a2 = rawA2 * invA0;
}
```

---

## 7. New State

Add:

```cpp
float colorEnvAttackCoeff = 0.f;
float colorEnvReleaseCoeff = 0.f;
float colorGainAttackCoeff = 0.f;
float colorGainReleaseCoeff = 0.f;
```

Add state struct:

```cpp
struct ColorLimiterState {
    dsp::RCFilter topHpL;
    dsp::RCFilter topHpR;

    dsp::RCFilter refHpL;
    dsp::RCFilter refLpL;
    dsp::RCFilter refHpR;
    dsp::RCFilter refLpR;

    float topEnvL = 1e-6f;
    float topEnvR = 1e-6f;
    float refEnvL = 1e-6f;
    float refEnvR = 1e-6f;

    float targetCutDb = 0.f;
    float smoothedCutDb = 0.f;
    float activation = 0.f;
    float ledAmount = 0.f;

    dsp::ClockDivider coeffDivider;
    Biquad shelfL;
    Biquad shelfR;

    bool coeffsNeutral = true;
} colorLimiter;
```

---

## 8. Cutoff Setup

Add:

```cpp
void updateColorLimiterCutoffs(float sampleRate) {
    const auto norm = [&](float hz) {
        return clamp(hz / std::max(sampleRate, 1.f), 1e-5f, 0.49f);
    };

    colorLimiter.topHpL.setCutoff(norm(kColorTopLowHz));
    colorLimiter.topHpR.setCutoff(norm(kColorTopLowHz));

    colorLimiter.refHpL.setCutoff(norm(kColorRefLowHz));
    colorLimiter.refHpR.setCutoff(norm(kColorRefLowHz));
    colorLimiter.refLpL.setCutoff(norm(kColorRefHighHz));
    colorLimiter.refLpR.setCutoff(norm(kColorRefHighHz));
}
```

Call this in:

```cpp
Sil()
onSampleRateChange()
```

---

## 9. Coefficient Setup

Extend `updateDynamicsCoefficients()`:

```cpp
colorEnvAttackCoeff = std::exp(-1.f / (kColorEnvAttackSec * sr));
colorEnvReleaseCoeff = std::exp(-1.f / (kColorEnvReleaseSec * sr));
colorGainAttackCoeff = std::exp(-1.f / (kColorGainAttackSec * sr));
colorGainReleaseCoeff = std::exp(-1.f / (kColorGainReleaseSec * sr));
```

Constructor:

```cpp
colorLimiter.coeffDivider.setDivision(kColorLimiterCoeffDivision);
colorLimiter.shelfL.setHighShelf(APP->engine->getSampleRate(), kColorShelfHz, kColorShelfSlope, 0.f);
colorLimiter.shelfR.setHighShelf(APP->engine->getSampleRate(), kColorShelfHz, kColorShelfSlope, 0.f);
```

Sample-rate change:

```cpp
resetColorLimiterState();
colorLimiter.shelfL.setHighShelf(e.sampleRate, kColorShelfHz, kColorShelfSlope, 0.f);
colorLimiter.shelfR.setHighShelf(e.sampleRate, kColorShelfHz, kColorShelfSlope, 0.f);
```

---

## 10. Reset Function

Add:

```cpp
void resetColorLimiterState() {
    colorLimiter.topEnvL = 1e-6f;
    colorLimiter.topEnvR = 1e-6f;
    colorLimiter.refEnvL = 1e-6f;
    colorLimiter.refEnvR = 1e-6f;

    colorLimiter.targetCutDb = 0.f;
    colorLimiter.smoothedCutDb = 0.f;
    colorLimiter.activation = 0.f;
    colorLimiter.ledAmount = 0.f;
    colorLimiter.coeffsNeutral = true;

    colorLimiter.topHpL.reset();
    colorLimiter.topHpR.reset();
    colorLimiter.refHpL.reset();
    colorLimiter.refLpL.reset();
    colorLimiter.refHpR.reset();
    colorLimiter.refLpR.reset();

    colorLimiter.shelfL.reset();
    colorLimiter.shelfR.reset();
}
```

---

## 11. Process Block

Insert immediately after saturator block and before limiter block:

```cpp
float coloredL = saturatedL;
float coloredR = saturatedR;
float colorLimiterLed = 0.f;

if (masteringEnabled) {
    colorLimiter.topHpL.process(saturatedL);
    colorLimiter.topHpR.process(saturatedR);
    const float topL = colorLimiter.topHpL.highpass();
    const float topR = colorLimiter.topHpR.highpass();

    colorLimiter.refHpL.process(saturatedL);
    const float refHighL = colorLimiter.refHpL.highpass();
    colorLimiter.refLpL.process(refHighL);
    const float refBandL = colorLimiter.refLpL.lowpass();

    colorLimiter.refHpR.process(saturatedR);
    const float refHighR = colorLimiter.refHpR.highpass();
    colorLimiter.refLpR.process(refHighR);
    const float refBandR = colorLimiter.refLpR.lowpass();

    auto updateColorEnv = [&](float& env, float x) {
        const float absX = std::fabs(x);
        const float c = (absX > env) ? colorEnvAttackCoeff : colorEnvReleaseCoeff;
        env = absX + c * (env - absX);
    };

    updateColorEnv(colorLimiter.topEnvL, topL);
    updateColorEnv(colorLimiter.topEnvR, topR);
    updateColorEnv(colorLimiter.refEnvL, refBandL);
    updateColorEnv(colorLimiter.refEnvR, refBandR);

    const float topEnv = 0.5f * (colorLimiter.topEnvL + colorLimiter.topEnvR);
    const float refEnv = 0.5f * (colorLimiter.refEnvL + colorLimiter.refEnvR);

    const float topDbFs = toDbFsSafe(topEnv);
    const float topVsRefDb = toDbSafe(topEnv) - toDbSafe(refEnv) + kColorTopNormDb;

    const float audibleTopGate = softKnee01(
        topDbFs,
        kColorTopGateDbFs,
        kColorTopGateKneeDb
    );

    const float relativeExcess = softKnee01(
        topVsRefDb,
        kColorExcessThresholdDb,
        kColorExcessKneeDb
    );

    const float absoluteHarshness = softKnee01(
        topDbFs,
        kColorHarshAbsDbFs,
        kColorHarshAbsKneeDb
    );

    // Saturator activity slightly increases sensitivity, because this stage is
    // specifically meant to catch post-saturation fizz and synthetic top-end hash.
    const float satBias = 1.f + 0.25f * clamp(saturator.ledAmount, 0.f, 1.f);

    const float targetActivation = clamp(
        audibleTopGate * std::max(relativeExcess, 0.65f * absoluteHarshness) * satBias,
        0.f,
        1.f
    );

    colorLimiter.targetCutDb = -kColorMaxCutDb * targetActivation;

    const float gainCoeff =
        (colorLimiter.targetCutDb < colorLimiter.smoothedCutDb)
            ? colorGainAttackCoeff
            : colorGainReleaseCoeff;

    colorLimiter.smoothedCutDb =
        colorLimiter.targetCutDb + gainCoeff * (colorLimiter.smoothedCutDb - colorLimiter.targetCutDb);

    colorLimiter.activation = clamp(
        -colorLimiter.smoothedCutDb / std::max(kColorMaxCutDb, 1e-6f),
        0.f,
        1.f
    );

    if (colorLimiter.coeffDivider.process()) {
        colorLimiter.shelfL.setHighShelf(
            args.sampleRate,
            kColorShelfHz,
            kColorShelfSlope,
            colorLimiter.smoothedCutDb
        );
        colorLimiter.shelfR.setHighShelf(
            args.sampleRate,
            kColorShelfHz,
            kColorShelfSlope,
            colorLimiter.smoothedCutDb
        );

        colorLimiter.coeffsNeutral =
            std::fabs(colorLimiter.smoothedCutDb) < 1e-5f;
    }

    coloredL = colorLimiter.shelfL.process(saturatedL);
    coloredR = colorLimiter.shelfR.process(saturatedR);

    // LED should show meaningful spectral limiting, not tiny coefficient drift.
    const float ledDeadbandDb = 0.20f;
    colorLimiter.ledAmount = clamp(
        std::max(0.f, -colorLimiter.smoothedCutDb - ledDeadbandDb) /
            std::max(kColorMaxCutDb - ledDeadbandDb, 1e-6f),
        0.f,
        1.f
    );

    colorLimiterLed = colorLimiter.ledAmount;
}
else {
    colorLimiter.targetCutDb = 0.f;
    colorLimiter.smoothedCutDb = 0.f;
    colorLimiter.activation = 0.f;
    colorLimiter.ledAmount = 0.f;

    if (!colorLimiter.coeffsNeutral) {
        colorLimiter.shelfL.setHighShelf(args.sampleRate, kColorShelfHz, kColorShelfSlope, 0.f);
        colorLimiter.shelfR.setHighShelf(args.sampleRate, kColorShelfHz, kColorShelfSlope, 0.f);
        colorLimiter.coeffsNeutral = true;
    }
}
```

---

## 12. UI / LED

Add a new light:

```cpp
COLOR_LIMITER_LIGHT,
```

Recommended enum order:

```cpp
LIMITER_ACTIVE_LIGHT,
LOW_RECOVERY_LIGHT,
REMOVE_MUD_LIGHT,
GLUE_COMP_LIGHT,
SATURATOR_LIGHT,
COLOR_LIMITER_LIGHT,
STEREO_ENHANCE_LIGHT,
MICROPEAK_LIGHT,
MASTERING_ENABLED_LIGHT,
REPAIR_ENABLED_LIGHT,
LIGHTS_LEN
```

However, if Codex wants the smallest patch-risk change, add `COLOR_LIMITER_LIGHT` immediately before `MASTERING_ENABLED_LIGHT` instead of reordering existing lights.

Add brightness update:

```cpp
lights[COLOR_LIMITER_LIGHT].setSmoothBrightness(
    masteringEnabled ? colorLimiterLed : 0.f,
    args.sampleTime
);
```

Panel placement recommendation:

```text
Low Recovery -> Remove Mud -> Glue -> Stereo Enhance -> Saturator -> Color Limit -> True-Peak Limit
```

If the existing panel has no room, place the Color Limit LED between the Saturator and Limiter LEDs visually, even if the enum is appended at the end.

---

## 13. Interaction With Existing Stages

### Stereo Enhance

Stereo Enhance may lift Side at 6 kHz. Color Limiter should not undo that by default.

That is why the Color Limiter shelf starts higher, around 9.5 kHz, and detects mostly above 8.5 kHz. The 6 kHz Side lift remains musical width/presence; Color Limiter catches excessive top-octave glare.

### Saturator

Saturation can generate upper harmonics and brittle edge. Color Limiter sits after Saturator specifically to catch that.

The `satBias` multiplier intentionally makes Color Limiter slightly more sensitive when saturation is doing more work.

### Final True-Peak Limiter

Final limiter must receive the color-limited signal. This prevents the limiter from preserving excessive top-end hash as part of the final clipped/limited spectral shape.

---

## 14. Optional Context Menu Mode

Not required for first implementation, but useful later:

```cpp
enum ColorLimiterMode {
    COLOR_LIMITER_OFF,
    COLOR_LIMITER_GENTLE,
    COLOR_LIMITER_NORMAL,
    COLOR_LIMITER_STRONG
};
```

Suggested values:

```text
Gentle: max cut 1.25 dB
Normal: max cut 2.25 dB
Strong: max cut 3.00 dB
```

Default: `Normal`.

If mode is implemented, persist in JSON:

```cpp
json_object_set_new(rootJ, "colorLimiterMode", json_integer(colorLimiterMode));
```

---

## 15. Acceptance Criteria

### Functional

* With `masteringEnabled == false`, Color Limiter must be neutral.
* With balanced pink-noise-like material, LED should remain mostly off or barely glow.
* With bright white-noise-heavy / brittle AI material, LED should visibly activate.
* With strong saturation generating high-frequency fizz, Color Limiter should apply roughly `0.5–2.25 dB` of high-shelf reduction.
* Final limiter must analyze `coloredL/R`, not `saturatedL/R`.

### Audio Quality

* No zippering during sweeps.
* No obvious pumping on hi-hats.
* No stereo image wobble.
* No more than subtle dulling on already-balanced mixes.
* Should feel like the top end is being “polished down,” not filtered away.

### Performance

* No allocations in `process()`.
* Coefficients update through `ClockDivider`, not every sample.
* CPU impact should be negligible compared to existing FFT / visualization work.

### Stability

* Must work at 44.1, 48, 96, and 192 kHz.
* Shelf frequency must clamp below Nyquist.
* State must reset cleanly on sample-rate change.

---

## 16. Tuning Notes

Initial constants should be treated as musical defaults, not sacred law.

Most likely tuning points:

```cpp
kColorTopNormDb
kColorExcessThresholdDb
kColorMaxCutDb
kColorShelfHz
kColorGainReleaseSec
```

If it activates too often:

```cpp
increase kColorExcessThresholdDb
decrease kColorTopNormDb
raise kColorTopGateDbFs less aggressively
```

If it barely activates on harsh AI material:

```cpp
increase kColorTopNormDb
lower kColorExcessThresholdDb
increase satBias from 0.25 to 0.35
```

If it sounds dull:

```cpp
reduce kColorMaxCutDb to 1.5
raise kColorShelfHz to 11000
increase release time
```

If it misses very high fizz:

```cpp
lower kColorTopLowHz to 7500
lower kColorShelfHz to 8500
```

---

## 17. Summary

Implement Color Limiter as:

```text
adaptive detector:
    compare 8.5k+ top band against 2.5k–8.5k reference band

adaptive gain:
    smooth target cut from 0 dB to -2.25 dB

audio filter:
    shared L/R high shelf around 9.5 kHz

placement:
    after Saturator, before final True-Peak Limiter

LED:
    brightness = normalized amount of high-shelf cut
```

This gives Sil a final spectral safeguard: saturation can add warmth and controlled density, while Color Limiter prevents the crown from turning into broken glass.

```
```
