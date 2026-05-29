Dragon King Leviathan, agreed — keep the final limiter at **-1.0 dBTP** and tune the saturator to **kiss** that ceiling, not bulldoze it. That is the safer streaming-mastering posture: true-peak limiting helps catch inter-sample / codec-conversion overs, and -1.0 dBTP is widely used as a conservative streaming ceiling; some guidance even suggests more headroom for very loud masters. ([Music Guy Mixing][1])

Below is a Codex-ready markdown spec.

```md
# Sil Adaptive Saturator + -1.0 dBTP Limiter Spec

## Goal

Add a gentle adaptive saturator to Sil’s mastering chain before the final limiter.

The saturator should:
- Add subtle harmonic density and perceived loudness.
- Use recent audio history to auto-tune its makeup/drive behavior.
- Feed the final limiter enough that loud passages occasionally trigger limiting.
- Preserve the final limiter ceiling at **-1.0 dBTP / -1.0 dBFS equivalent in Rack voltage terms**.
- Avoid runaway feedback where the saturator keeps pushing harder because the limiter hides the true input level.

The limiter remains the final ceiling guard.

Current Sil already has:
- Global mastering enable.
- Low-band recovery.
- Micropeak cleanup.
- Final limiter stage.
- Stage activity LEDs.
- 10-second rolling output buffer.
- `kAudioFullScaleV = 5.f`.
- Current limiter ceiling based on `kAudioFullScaleV * pow(10, -1 / 20)`.

Use the existing architecture and LED style as the model.
```

```md
## v1 Scope Narrowing

For implementation v1, constrain complexity and CPU:

- Keep adaptive control primarily on **makeup**.
- Keep drive movement narrow and slow (`1.0` to `1.35`).
- Use a safer default pre-limiter target of **`-0.85 dBFS`**.
- Avoid per-update sorting of 1000 bins; use a fixed histogram estimator for percentile.
- Keep limiter interpolation based on the actual limiter input stream (post-saturator).
```

```md
## Target Mastering Chain

Recommended processing order:

Input
→ Low-band mono recovery
→ Remove Mud dynamic EQ
→ Micropeak cleanup
→ Glue compressor
→ Adaptive saturator
→ Final true-peak-ish limiter
→ Output

If Remove Mud and Glue are not implemented yet, add the saturator after micropeak cleanup and before the limiter for now:

Input
→ Low-band mono recovery
→ Micropeak cleanup
→ Adaptive saturator
→ Final limiter
→ Output
```

````md
## Limiter Constraint

The final limiter ceiling must remain:

- `kLimiterCeilingDb = -1.0f`
- `kLimiterCeilingV = kAudioFullScaleV * pow(10.f, kLimiterCeilingDb / 20.f)`

With `kAudioFullScaleV = 5.f`:

- `kLimiterCeilingV ≈ 4.45625 V`

Do not change the final limiter to -0.666 dBFS.

The saturator should instead target a pre-limiter level that occasionally exceeds or approaches the -1.0 dB ceiling by a small amount.

Recommended pre-limiter target:

- Conservative: `-0.85 dBFS`
- Medium/default: `-0.75 dBFS`
- Hot: `-0.60 dBFS`

For v1, use:

```cpp
static constexpr float kLimiterCeilingDb = -1.0f;
static constexpr float kSatTargetPreLimiterDb = -0.85f;
static constexpr float kSatLimiterTickleDb = kSatTargetPreLimiterDb - kLimiterCeilingDb; // +0.15 dB
````

This means loud passages are encouraged to enter the limiter by roughly 0.15 dB, reducing risk of constant limiter activity in dense material.

````

```md
## Important Design Rule

Do not tune the saturator from post-limiter output.

Bad feedback loop:

Saturator pushes harder
→ limiter clamps output
→ analyzer sees output is safe
→ saturator pushes harder
→ limiter overworks

Instead use one or both of these:

1. Primary analyzer: pre-saturator signal.
2. Secondary confirmation: post-saturator / pre-limiter signal.

Avoid using final output as the saturator control source.
````

````md
## New State

Add a new saturator state struct to `Sil`.

```cpp
struct SaturatorState {
    static constexpr int HISTORY_BINS = 1000;
    static constexpr int PERCENTILE_BINS = 96;

    float peakBins[HISTORY_BINS] = {};
    uint16_t percentileHist[PERCENTILE_BINS] = {};
    int binToHist[HISTORY_BINS] = {};
    int writeBin = 0;
    int samplesInBin = 0;
    int samplesPerBin = 441;
    float currentBinPeak = 0.f;

    float drive = 1.f;
    float makeupDb = 0.f;
    float targetDrive = 1.f;
    float targetMakeupDb = 0.f;

    float preLimiterPeakEstimate = 0.f;
    float ledAmount = 0.f;

    dsp::ClockDivider updateDivider;

    void reset(float sampleRate) {
        samplesPerBin = std::max(1, int(std::round(sampleRate * 10.f / HISTORY_BINS)));
        std::fill(std::begin(peakBins), std::end(peakBins), 0.f);
        std::fill(std::begin(percentileHist), std::end(percentileHist), 0);
        std::fill(std::begin(binToHist), std::end(binToHist), 0);
        writeBin = 0;
        samplesInBin = 0;
        currentBinPeak = 0.f;
        drive = 1.f;
        makeupDb = 0.f;
        targetDrive = 1.f;
        targetMakeupDb = 0.f;
        preLimiterPeakEstimate = 0.f;
        ledAmount = 0.f;
        updateDivider.setDivision(512);
    }
};
````

Add:

```cpp
SaturatorState saturator;
```

Call:

```cpp
saturator.reset(APP->engine->getSampleRate());
```

in the constructor.

Call:

```cpp
saturator.reset(e.sampleRate);
```

in `onSampleRateChange()`.

````

```md
## New Constants

Add near existing mastering constants:

```cpp
static constexpr float kLimiterCeilingDb = -1.0f;
static constexpr float kSaturatorTargetPreLimiterDb = -0.85f;

static constexpr float kSatMaxMakeupDb = 2.0f;
static constexpr float kSatMaxDrive = 1.35f;
static constexpr float kSatMinDrive = 1.0f;

static constexpr float kSatMakeupAttackSec = 1.50f;
static constexpr float kSatMakeupReleaseSec = 4.00f;
static constexpr float kSatDriveAttackSec = 2.00f;
static constexpr float kSatDriveReleaseSec = 5.00f;

static constexpr float kSatHistorySeconds = 10.0f;
static constexpr float kSatHistoryPercentile = 0.995f;
static constexpr int kSatUpdateDivision = 512;
````

Rationale:

* `kSatTargetPreLimiterDb = -0.85f` gives approximately 0.15 dB of intended limiter tickle.
* `kSatMaxMakeupDb = 2.0f` prevents runaway loudness chasing.
* `kSatMaxDrive = 1.35f` keeps saturation subtle and minimizes coloration swings.
* Slow attack/release prevents audible auto-level pumping.

````

```md
## Saturator Algorithm

Use a gentle arctangent saturator:

```cpp
y = atan(drive * x) / atan(drive)
````

This is smoother than hard clipping and subtler than aggressive tanh saturation.

Processing order inside the saturator:

1. Analyze pre-saturator peak history.
2. Compute recent loud peak percentile from 10-second history.
3. Estimate needed makeup to reach `kSaturatorTargetPreLimiterDb`.
4. Couple drive gently to makeup.
5. Smooth makeup and drive slowly.
6. Apply makeup into saturator.
7. Output saturated signal into final limiter.
8. LED shows saturation/makeup activity.

````

```md
## 10-Second History

Use 1000 bins, each representing roughly 10 ms.

For each bin, store:

```cpp
max(abs(L), abs(R))
````

This avoids storing every sample and gives a robust loud-portion estimate.

Do not use the single highest peak from the last 10 seconds. Use a percentile, default 99.5%, to avoid one stray spike causing the saturator to under-drive for the next 10 seconds.

For v1 CPU safety, do not sort all 1000 bins every control update. Maintain a fixed histogram (`PERCENTILE_BINS`), update counts incrementally when rolling bins are replaced, and recover percentile from the cumulative count.

Update the history from **pre-saturator input**, not post-limiter output.

````

```md
## Saturator Processing Sketch

Add a helper method to `Sil`:

```cpp
sil_micropeak::StereoSample processSaturator(
    float inL,
    float inR,
    float sampleRate
) {
    // 1. Update pre-saturator history.
    const float inputPeak = std::max(std::fabs(inL), std::fabs(inR));
    saturator.currentBinPeak = std::max(saturator.currentBinPeak, inputPeak);
    saturator.samplesInBin++;

    if (saturator.samplesInBin >= saturator.samplesPerBin) {
        saturator.peakBins[saturator.writeBin] = saturator.currentBinPeak;
        saturator.writeBin = (saturator.writeBin + 1) % SaturatorState::HISTORY_BINS;
        saturator.samplesInBin = 0;
        saturator.currentBinPeak = 0.f;
    }

    // 2. Control-rate update.
    if (saturator.updateDivider.process()) {
        // v1: estimate percentile from histogram counts (no 1000-element sort)
        const float recentLoudPeak = std::max(estimateRecentPeakPercentile(), 1e-6f);
        const float recentPeakNorm = clamp(recentLoudPeak / kAudioFullScaleV, 1e-6f, 4.f);
        const float recentPeakDb = 20.f * std::log10(recentPeakNorm);

        // Target pre-limiter peak slightly hotter than limiter ceiling.
        const float desiredMakeupDb =
            clamp(kSaturatorTargetPreLimiterDb - recentPeakDb, 0.f, kSatMaxMakeupDb);

        // Keep drive coupling mild and bounded in v1.
        const float desiredDrive =
            clamp(1.f + desiredMakeupDb * 0.22f, kSatMinDrive, kSatMaxDrive);

        const float updateSeconds = float(kSatUpdateDivision) / std::max(sampleRate, 1.f);

        const float makeupTau =
            (desiredMakeupDb > saturator.makeupDb) ? kSatMakeupAttackSec : kSatMakeupReleaseSec;
        const float makeupCoeff =
            std::exp(-updateSeconds / std::max(1e-3f, makeupTau));

        saturator.makeupDb =
            desiredMakeupDb + makeupCoeff * (saturator.makeupDb - desiredMakeupDb);

        const float driveTau =
            (desiredDrive > saturator.drive) ? kSatDriveAttackSec : kSatDriveReleaseSec;
        const float driveCoeff =
            std::exp(-updateSeconds / std::max(1e-3f, driveTau));

        saturator.drive =
            desiredDrive + driveCoeff * (saturator.drive - desiredDrive);
    }

    // 3. Apply makeup into soft saturation.
    const float makeup = std::pow(10.f, saturator.makeupDb / 20.f);
    const float drive = clamp(saturator.drive, kSatMinDrive, kSatMaxDrive);

    auto shape = [&](float x) {
        const float xNorm = clamp((x * makeup) / kAudioFullScaleV, -3.f, 3.f);
        const float shapedNorm = std::atan(drive * xNorm) / std::atan(drive);
        return shapedNorm * kAudioFullScaleV;
    };

    const float outL = shape(inL);
    const float outR = shape(inR);

    // 4. Activity LED.
    const float makeupActivity = clamp(saturator.makeupDb / kSatMaxMakeupDb, 0.f, 1.f);
    const float driveActivity = clamp((drive - kSatMinDrive) / (kSatMaxDrive - kSatMinDrive), 0.f, 1.f);
    saturator.ledAmount = clamp(0.55f * makeupActivity + 0.45f * driveActivity, 0.f, 1.f);

    return sil_micropeak::StereoSample(outL, outR);
}
````

````

```md
## Integration in `process()`

Currently the code computes `preMasterL` / `preMasterR`, then micropeak cleanup, then limiter.

After micropeak cleanup, insert saturator before limiter:

```cpp
const bool micropeakActive = consumeMicropeakHoldSample();

const sil_micropeak::StereoSample cleaned = micropeakCleanupFilter.process(
    sil_micropeak::StereoSample(preMasterL, preMasterR),
    micropeakActive,
    kAudioFullScaleV
);

sil_micropeak::StereoSample saturated = cleaned;

if (masteringEnabled) {
    saturated = processSaturator(cleaned.l, cleaned.r, args.sampleRate);
}
else {
    saturator.ledAmount = 0.f;
}

float outL = saturated.l;
float outR = saturated.r;
````

Then update the limiter to use `saturated` instead of `cleaned`:

```cpp
float peak = std::max(std::fabs(saturated.l), std::fabs(saturated.r));
```

And:

```cpp
outL = saturated.l * limiterGain;
outR = saturated.r * limiterGain;
limiterPrevL = saturated.l;
limiterPrevR = saturated.r;
```

When doing one-sample lookahead interpolation, interpolate between the previous saturated sample and current saturated sample, and evaluate forward interpolation from current saturated toward next limiter-input sample in the same domain.

Do not interpolate from saturated output toward pre-master input; after adding saturation, the limiter should observe the signal that actually enters it.

````

```md
## Limiter Update

Replace:

```cpp
const float limiterCeiling = kAudioFullScaleV * std::pow(10.f, -1.f / 20.f);
````

With:

```cpp
const float limiterCeiling = kAudioFullScaleV * std::pow(10.f, kLimiterCeilingDb / 20.f);
```

Keep:

```cpp
static constexpr int kLimiterOversampleFactor = 4;
```

But rename comments from “true-peak as much as possible” to “true-peak-ish oversampled limiter estimate.”

Optional quality improvement:

* Add context menu option for 4x / 8x limiter peak interpolation.
* Default to 4x for CPU safety.
* 8x is better for final export / high quality mode.

Do not lower the limiter ceiling below -1.0 dB unless a future “extra safe streaming” mode is added.

````

```md
## New Light

Add to `LightId`:

```cpp
SATURATOR_LIGHT,
````

Recommended order:

```cpp
enum LightId {
    LIMITER_ACTIVE_LIGHT,
    LOW_RECOVERY_LIGHT,
    REMOVE_MUD_LIGHT,
    MICROPEAK_LIGHT,
    GLUE_LIGHT,
    SATURATOR_LIGHT,
    MASTERING_ENABLED_LIGHT,
    LIGHTS_LEN
};
```

If Remove Mud / Glue are not implemented yet, use:

```cpp
enum LightId {
    LIMITER_ACTIVE_LIGHT,
    LOW_RECOVERY_LIGHT,
    MICROPEAK_LIGHT,
    SATURATOR_LIGHT,
    MASTERING_ENABLED_LIGHT,
    LIGHTS_LEN
};
```

In `process()`:

```cpp
lights[SATURATOR_LIGHT].setSmoothBrightness(
    masteringEnabled ? saturator.ledAmount : 0.f,
    args.sampleTime
);
```

Recommended LED color:

* Orange / amber.
* Saturator is warmth/density, not danger.
* Limiter remains yellow.
* Micropeak remains red.

````

```md
## Visual / SVG Integration

Add a new component point to `res/sil.svg`:

```text
SATURATOR_LIGHT
````

Place it near the other mastering-status LEDs.

Suggested vertical order:

LIMIT
LOW
MUD
PEAK
GLUE
SAT

Or if only current features exist:

LIMIT
LOW
PEAK
SAT

Update widget defaults:

```cpp
Vec saturatorLightPos(48.f, 51.5f);
```

Add point override:

```cpp
applyPointOverride("SATURATOR_LIGHT", &saturatorLightPos);
```

Add child:

```cpp
addChild(createLightCentered<SmallLight<OrangeLight>>(
    mm2px(saturatorLightPos), module, Sil::SATURATOR_LIGHT
));
```

If `OrangeLight` is unavailable, use `YellowLight` for v1.

````

```md
## Behavior Targets

### Quiet material

If recent loud peak is low:
- Saturator may add makeup.
- Drive rises slowly.
- Limiter may begin to tickle only during loudest passages.
- No sudden jumps.

### Already-hot material

If recent loud peak is already near -1 dBFS:
- Makeup should remain near 0 dB.
- Drive should remain near 1.0.
- Saturator LED should stay dim.
- Limiter should not be overworked.

### Dense AI-generated material

Expected behavior:
- Saturator adds mild density.
- Loud passages hit limiter occasionally.
- Limiter LED flickers or glows lightly.
- Output ceiling remains -1.0 dBFS / dBTP target.

### Single sine wave

Expected behavior:
- Saturator should not audibly fuzz at normal levels.
- At high levels, it should round smoothly, not hard clip.
- Limiter catches final overs.

### Kick-heavy loop

Expected behavior:
- Saturator should not chase every kick.
- 10-second history and slow smoothing should prevent pumping.
- Limiter should tickle, not clamp constantly.

### Bypass

When mastering is disabled:
- Saturator is bypassed.
- Saturator LED is off.
- Limiter gain resets as currently implemented.
````

```md
## Acceptance Criteria

1. Final output never exceeds the configured limiter ceiling under normal operation:
   - Target: `-1.0 dBFS equivalent = 4.456 V`
   - With current true-peak-ish limiter, verify using oversampled offline tests.

2. Saturator activity is slow and stable:
   - No audible gain chasing.
   - No rapid LED flicker except during legitimate signal changes.

3. Limiter activity occurs during loud passages:
   - Saturator should feed limiter enough to show occasional limiter LED movement.
   - Limiter gain reduction should usually stay under 1 dB.
   - Sustained >2 dB limiter reduction should be considered too hot for default behavior.

4. Saturator should not use post-limiter output for adaptive tuning.

5. The existing micropeak cleanup behavior should remain unchanged except that its output now feeds the saturator.

6. Existing histogram / spectrum should continue to display final output after limiter.

7. Mastering bypass should produce direct input-to-output behavior, consistent with existing Sil behavior.
```

````md
## Recommended Debug Readout

Optional but useful during tuning:

```text
SAT: +1.4 dB / Drive 1.31 / TP tickle 0.2 dB
````

Or compact:

```text
Sat: 47%
```

For production, LED may be enough.

````

```md
## Notes for Future Quality Mode

The current limiter approximates true peak using interpolation. This is acceptable for a Rack module v1, but for export-grade confidence, consider:

- 8x oversampled peak detection.
- FIR/polyphase true-peak estimation.
- Optional “Streaming Safe” mode:
  - Ceiling: -1.0 dBTP
  - Saturator pre-limiter target: -0.85 dBFS
- Optional “Hot” mode:
  - Ceiling: -1.0 dBTP
  - Saturator pre-limiter target: -0.60 dBFS
  - Warn internally by stronger limiter LED if sustained GR > 2 dB.

Default should remain conservative.
````

```md
## Summary

Implement:

LOW
→ MUD
→ PEAK
→ GLUE
→ SAT
→ LIMIT

For the saturator specifically:

- Analyze pre-saturator input over last 10 seconds.
- Use 99.5th percentile peak, not absolute max.
- Use histogram-based percentile estimation (no full sort in control updates).
- Auto makeup aims loud passages toward about -0.85 dBFS pre-limiter.
- Final limiter stays fixed at -1.0 dBFS / dBTP target.
- Saturator uses gentle atan shaping.
- Limiter catches the final 0.25 dB-ish excess during loud passages.
- LED shows saturation/makeup activity.

The result should feel like a mastering polish stage:
more density, more presence, slightly more loudness, but still streaming-safe.
```

The one adjustment I’d make to the prior ambition is exactly this: **the saturator aims above the limiter ceiling, but the exported/output truth remains -1.0 dBTP**. In Culture terms: the engine may flare, but the hull field does not breach.

[1]: https://www.musicguymixing.com/true-peak-limiting/?utm_source=chatgpt.com "True Peak Limiting - What It Is and Why You Should Use It"
