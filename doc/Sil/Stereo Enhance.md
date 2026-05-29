# Sil Mastering Engine — Stereo Enhance M/S Adaptive EQ Spec

Target implementer: Codex 5.3  
Target files: `src/Sil.cpp`, `res/sil.svg` if adding a panel LED anchor  
Feature name: **Stereo Enhance**

## 1. Goal

Add an adaptive Mid/Side EQ stage to Sil that gently improves stereo clarity without blindly widening or brightening every source.

The stage consists of two fixed EQ moves whose gains are dynamically scaled:

1. **Mid cut**
   - Routing: Mid only
   - Type: peaking EQ
   - Frequency: `350 Hz`
   - Max gain: `-2.0 dB`
   - Q: `7.3`
   - Purpose: remove narrow center congestion around 350 Hz, but only when that region is meaningfully present in the Mid signal.

2. **Side lift**
   - Routing: Side only
   - Type: peaking EQ
   - Frequency: `6000 Hz`
   - Max gain: `+2.0 dB`
   - Q: `0.71`
   - Purpose: add gentle stereo air/detail, but reduce or disable the boost when the Side signal already has strong 6 kHz / upper-presence energy.

The user-facing behavior should feel like a single mastering-stage enhancer, not two separate controls. There is no knob requirement for this pass. The existing **Mastering** enable switch gates this stage.

## 2. Recommended chain position

Insert **Stereo Enhance after Glue Compressor and before Saturator**:

```text
Input
  -> Low-band mono recovery
  -> Remove Mud
  -> Micropeak repair
  -> Glue Compressor
  -> Stereo Enhance          <-- new stage
  -> Saturator
  -> Final Limiter
  -> Output / analyzers
```

Rationale:

- Glue works on the already-cleaned stereo image first.
- Stereo Enhance then shapes the post-glue image.
- Saturator and limiter remain downstream, so any added side energy is still caught by the final mastering safety stages.
- Micropeak detection/repair is not confused by intentional high-side brightening.

## 3. Mid/Side math

Use standard equal-power-safe Rack voltage math already consistent with the rest of Sil:

```cpp
const float mid  = 0.5f * (inL + inR);
const float side = 0.5f * (inL - inR);

// process mid and side independently

outL = enhancedMid + enhancedSide;
outR = enhancedMid - enhancedSide;
```

Important properties:

- If `L == R`, side is zero and the side EQ has no audible effect.
- The Side lift should not affect the mono sum, because Side cancels in mono.
- The Mid cut intentionally affects the mono sum when activated.

## 4. New state

Add a new `StereoEnhanceState` struct near the existing mastering stage state structs.

```cpp
struct StereoEnhanceState {
    // Analysis filters: Mid 350 detector
    dsp::RCFilter mid350Hp;
    dsp::RCFilter mid350Lp;
    dsp::RCFilter midBroadHp;
    dsp::RCFilter midBroadLp;

    // Analysis filters: Side 6 kHz detector
    dsp::RCFilter side6kHp;
    dsp::RCFilter side6kLp;
    dsp::RCFilter sideBroadHp;
    dsp::RCFilter sideBroadLp;

    float mid350Env = 1e-6f;
    float midBroadEnv = 1e-6f;
    float side6kEnv = 1e-6f;
    float sideBroadEnv = 1e-6f;

    float targetMidCutDb = 0.f;       // negative or zero
    float smoothedMidCutDb = 0.f;     // negative or zero
    float targetSideLiftDb = 0.f;     // positive or zero
    float smoothedSideLiftDb = 0.f;   // positive or zero

    float midActivation = 0.f;        // 0..1
    float sideActivation = 0.f;       // 0..1
    float ledAmount = 0.f;            // 0..1

    dsp::ClockDivider coeffDivider;
    Biquad midEq;
    Biquad sideEq;
    bool coeffsNeutral = true;
};

StereoEnhanceState stereoEnhance;
```

Do not reuse `RemoveMudState`. The new stage needs independent Mid/Side-domain analysis and different thresholds.

## 5. Constants

Add these constants near the existing mastering constants.

```cpp
static constexpr float kStereoMidCenterHz = 350.f;
static constexpr float kStereoMidQ = 7.3f;
static constexpr float kStereoMidMaxCutDb = 2.0f;

static constexpr float kStereoSideCenterHz = 6000.f;
static constexpr float kStereoSideQ = 0.71f;
static constexpr float kStereoSideMaxLiftDb = 2.0f;

// Analysis bands. These are intentionally wider than the EQ bell so the detector
// is stable and musical rather than twitching on a tiny resonance.
static constexpr float kStereoMid350LowHz = 270.f;
static constexpr float kStereoMid350HighHz = 470.f;
static constexpr float kStereoMidBroadLowHz = 120.f;
static constexpr float kStereoMidBroadHighHz = 1400.f;

static constexpr float kStereoSide6kLowHz = 4200.f;
static constexpr float kStereoSide6kHighHz = 9500.f;
static constexpr float kStereoSideBroadLowHz = 1000.f;
static constexpr float kStereoSideBroadHighHz = 12000.f;

// Detector normalization compensates for narrow analysis bands reading lower
// than broad bands on normal full-spectrum material.
static constexpr float kStereoMidBandNormDb = 7.5f;
static constexpr float kStereoSideBandNormDb = 3.0f;

// Mid cut activation: only cut when the 350 Hz Mid band is present and excessive.
static constexpr float kStereoMidGateDbFs = -42.f;
static constexpr float kStereoMidGateKneeDb = 10.f;
static constexpr float kStereoMidExcessThresholdDb = 1.0f;
static constexpr float kStereoMidExcessKneeDb = 5.0f;

// Side lift activation: lift only when side top is not already bright.
static constexpr float kStereoSideGateDbFs = -56.f;
static constexpr float kStereoSideGateKneeDb = 10.f;
static constexpr float kStereoSideAlreadyBrightDb = 1.5f;
static constexpr float kStereoSideBrightKneeDb = 4.0f;

// Envelope/gain smoothing.
static constexpr float kStereoEnvAttackSec = 0.040f;
static constexpr float kStereoEnvReleaseSec = 0.300f;
static constexpr float kStereoMidGainAttackSec = 0.180f;
static constexpr float kStereoMidGainReleaseSec = 0.900f;
static constexpr float kStereoSideGainAttackSec = 0.250f;
static constexpr float kStereoSideGainReleaseSec = 1.200f;
static constexpr int kStereoEnhanceCoeffDivision = 32;
```

Also add coefficient fields if desired:

```cpp
float stereoEnvAttackCoeff = 0.f;
float stereoEnvReleaseCoeff = 0.f;
float stereoMidGainAttackCoeff = 0.f;
float stereoMidGainReleaseCoeff = 0.f;
float stereoSideGainAttackCoeff = 0.f;
float stereoSideGainReleaseCoeff = 0.f;
```

## 6. Helper functions

Add a dBFS helper instead of using raw `toDbSafe()` for absolute gates.

```cpp
static float toDbFsSafe(float volts) {
    return 20.f * std::log10(std::max(volts, 1e-7f) / kAudioFullScaleV);
}
```

Add an inverse soft-knee helper for the Side lift if desired:

```cpp
static float inverseSoftKnee01(float xDb, float thresholdDb, float kneeDb) {
    return 1.f - softKnee01(xDb, thresholdDb, kneeDb);
}
```

This means:

- `softKnee01()` rises as a condition becomes true.
- `inverseSoftKnee01()` falls as a condition becomes too strong.

## 7. Cutoff update function

Add:

```cpp
void updateStereoEnhanceCutoffs(float sampleRate) {
    const auto norm = [&](float hz) {
        return clamp(hz / std::max(sampleRate, 1.f), 1e-5f, 0.49f);
    };

    stereoEnhance.mid350Hp.setCutoff(norm(kStereoMid350LowHz));
    stereoEnhance.mid350Lp.setCutoff(norm(kStereoMid350HighHz));
    stereoEnhance.midBroadHp.setCutoff(norm(kStereoMidBroadLowHz));
    stereoEnhance.midBroadLp.setCutoff(norm(kStereoMidBroadHighHz));

    stereoEnhance.side6kHp.setCutoff(norm(kStereoSide6kLowHz));
    stereoEnhance.side6kLp.setCutoff(norm(kStereoSide6kHighHz));
    stereoEnhance.sideBroadHp.setCutoff(norm(kStereoSideBroadLowHz));
    stereoEnhance.sideBroadLp.setCutoff(norm(kStereoSideBroadHighHz));
}
```

Call it from:

- `Sil()` constructor
- `onSampleRateChange()`

Also reset the analysis filters on sample-rate changes if the Rack filter type exposes `reset()`. Stale detector history across rate changes is not useful here and can produce a short false LED/gain pulse.

## 8. Dynamics coefficient update

Extend `updateDynamicsCoefficients(float sampleRate)`:

```cpp
stereoEnvAttackCoeff = std::exp(-1.f / (kStereoEnvAttackSec * sr));
stereoEnvReleaseCoeff = std::exp(-1.f / (kStereoEnvReleaseSec * sr));
stereoMidGainAttackCoeff = std::exp(-1.f / (kStereoMidGainAttackSec * sr));
stereoMidGainReleaseCoeff = std::exp(-1.f / (kStereoMidGainReleaseSec * sr));
stereoSideGainAttackCoeff = std::exp(-1.f / (kStereoSideGainAttackSec * sr));
stereoSideGainReleaseCoeff = std::exp(-1.f / (kStereoSideGainReleaseSec * sr));
```

## 9. Reset function

Add:

```cpp
void resetStereoEnhanceState() {
    stereoEnhance.mid350Env = 1e-6f;
    stereoEnhance.midBroadEnv = 1e-6f;
    stereoEnhance.side6kEnv = 1e-6f;
    stereoEnhance.sideBroadEnv = 1e-6f;

    stereoEnhance.targetMidCutDb = 0.f;
    stereoEnhance.smoothedMidCutDb = 0.f;
    stereoEnhance.targetSideLiftDb = 0.f;
    stereoEnhance.smoothedSideLiftDb = 0.f;

    stereoEnhance.midActivation = 0.f;
    stereoEnhance.sideActivation = 0.f;
    stereoEnhance.ledAmount = 0.f;
    stereoEnhance.coeffsNeutral = true;

    stereoEnhance.midEq.reset();
    stereoEnhance.sideEq.reset();
}
```

Call it from `onSampleRateChange()`, then immediately set both EQs back to neutral coefficients for the new sample rate:

```cpp
stereoEnhance.midEq.setPeaking(e.sampleRate, kStereoMidCenterHz, kStereoMidQ, 0.f);
stereoEnhance.sideEq.setPeaking(e.sampleRate, kStereoSideCenterHz, kStereoSideQ, 0.f);
```

When `masteringEnabled == false`, set the target/smoothed gains and LED values to zero. Also force the EQ coefficients to neutral either immediately on the disable edge or by skipping the EQ path entirely while disabled. Do not leave stale non-zero biquad coefficients waiting for the next 32-sample coefficient tick, because the first samples after re-enable could otherwise use old gain.

A full state reset is acceptable on disable; preserving detector envelopes is not required.

## 10. Constructor initialization

In `Sil()` constructor:

```cpp
stereoEnhance.coeffDivider.setDivision(kStereoEnhanceCoeffDivision);
updateStereoEnhanceCutoffs(APP->engine->getSampleRate());
stereoEnhance.midEq.setPeaking(APP->engine->getSampleRate(), kStereoMidCenterHz, kStereoMidQ, 0.f);
stereoEnhance.sideEq.setPeaking(APP->engine->getSampleRate(), kStereoSideCenterHz, kStereoSideQ, 0.f);
```

If a bypass-edge flag is already used for other Sil stages, reuse that pattern to avoid writing neutral EQ coefficients every sample while bypassed.

## 11. Processing implementation

After Glue Compressor and before Saturator, replace direct use of `gluedL/gluedR` by a new enhanced pair.

Current region conceptually becomes:

```cpp
float enhancedL = gluedL;
float enhancedR = gluedR;
float stereoEnhanceLed = 0.f;

if (masteringEnabled) {
    // M/S encode
    const float mid = 0.5f * (gluedL + gluedR);
    const float side = 0.5f * (gluedL - gluedR);

    // --- Analysis bands: Mid ---
    stereoEnhance.mid350Hp.process(mid);
    const float mid350High = stereoEnhance.mid350Hp.highpass();
    stereoEnhance.mid350Lp.process(mid350High);
    const float mid350Band = stereoEnhance.mid350Lp.lowpass();

    stereoEnhance.midBroadHp.process(mid);
    const float midBroadHigh = stereoEnhance.midBroadHp.highpass();
    stereoEnhance.midBroadLp.process(midBroadHigh);
    const float midBroadBand = stereoEnhance.midBroadLp.lowpass();

    // --- Analysis bands: Side ---
    stereoEnhance.side6kHp.process(side);
    const float side6kHigh = stereoEnhance.side6kHp.highpass();
    stereoEnhance.side6kLp.process(side6kHigh);
    const float side6kBand = stereoEnhance.side6kLp.lowpass();

    stereoEnhance.sideBroadHp.process(side);
    const float sideBroadHigh = stereoEnhance.sideBroadHp.highpass();
    stereoEnhance.sideBroadLp.process(sideBroadHigh);
    const float sideBroadBand = stereoEnhance.sideBroadLp.lowpass();

    auto updateStereoEnv = [&](float& env, float x) {
        const float absX = std::fabs(x);
        const float c = (absX > env) ? stereoEnvAttackCoeff : stereoEnvReleaseCoeff;
        env = absX + c * (env - absX);
    };

    updateStereoEnv(stereoEnhance.mid350Env, mid350Band);
    updateStereoEnv(stereoEnhance.midBroadEnv, midBroadBand);
    updateStereoEnv(stereoEnhance.side6kEnv, side6kBand);
    updateStereoEnv(stereoEnhance.sideBroadEnv, sideBroadBand);

    // --- Mid cut activation ---
    const float mid350DbFs = toDbFsSafe(stereoEnhance.mid350Env);
    const float midBroadDbFs = toDbFsSafe(stereoEnhance.midBroadEnv);
    const float midPresenceGate = softKnee01(
        mid350DbFs,
        kStereoMidGateDbFs,
        kStereoMidGateKneeDb
    );
    const float midExcessDb =
        (mid350DbFs - midBroadDbFs) + kStereoMidBandNormDb;
    const float midExcess = softKnee01(
        midExcessDb,
        kStereoMidExcessThresholdDb,
        kStereoMidExcessKneeDb
    );
    const float targetMidActivation = clamp(midPresenceGate * midExcess, 0.f, 1.f);
    stereoEnhance.targetMidCutDb = -kStereoMidMaxCutDb * targetMidActivation;

    // --- Side lift activation ---
    const float side6kDbFs = toDbFsSafe(stereoEnhance.side6kEnv);
    const float sideBroadDbFs = toDbFsSafe(stereoEnhance.sideBroadEnv);
    const float sideContentGate = softKnee01(
        std::max(side6kDbFs, sideBroadDbFs),
        kStereoSideGateDbFs,
        kStereoSideGateKneeDb
    );
    const float sideBrightnessDb =
        (side6kDbFs - sideBroadDbFs) + kStereoSideBandNormDb;
    const float sideNotAlreadyBright = inverseSoftKnee01(
        sideBrightnessDb,
        kStereoSideAlreadyBrightDb,
        kStereoSideBrightKneeDb
    );
    const float targetSideActivation = clamp(sideContentGate * sideNotAlreadyBright, 0.f, 1.f);
    stereoEnhance.targetSideLiftDb = kStereoSideMaxLiftDb * targetSideActivation;

    // --- Smooth dynamic EQ gains ---
    const float midCoeff =
        (stereoEnhance.targetMidCutDb < stereoEnhance.smoothedMidCutDb)
            ? stereoMidGainAttackCoeff
            : stereoMidGainReleaseCoeff;
    stereoEnhance.smoothedMidCutDb =
        stereoEnhance.targetMidCutDb + midCoeff *
        (stereoEnhance.smoothedMidCutDb - stereoEnhance.targetMidCutDb);

    const float sideCoeff =
        (stereoEnhance.targetSideLiftDb > stereoEnhance.smoothedSideLiftDb)
            ? stereoSideGainAttackCoeff
            : stereoSideGainReleaseCoeff;
    stereoEnhance.smoothedSideLiftDb =
        stereoEnhance.targetSideLiftDb + sideCoeff *
        (stereoEnhance.smoothedSideLiftDb - stereoEnhance.targetSideLiftDb);

    stereoEnhance.midActivation = clamp(
        -stereoEnhance.smoothedMidCutDb / std::max(kStereoMidMaxCutDb, 1e-6f),
        0.f,
        1.f
    );
    stereoEnhance.sideActivation = clamp(
        stereoEnhance.smoothedSideLiftDb / std::max(kStereoSideMaxLiftDb, 1e-6f),
        0.f,
        1.f
    );

    if (stereoEnhance.coeffDivider.process()) {
        stereoEnhance.midEq.setPeaking(
            args.sampleRate,
            kStereoMidCenterHz,
            kStereoMidQ,
            stereoEnhance.smoothedMidCutDb
        );
        stereoEnhance.sideEq.setPeaking(
            args.sampleRate,
            kStereoSideCenterHz,
            kStereoSideQ,
            stereoEnhance.smoothedSideLiftDb
        );
        stereoEnhance.coeffsNeutral =
            std::fabs(stereoEnhance.smoothedMidCutDb) < 1e-5f &&
            std::fabs(stereoEnhance.smoothedSideLiftDb) < 1e-5f;
    }

    const float enhancedMid = stereoEnhance.midEq.process(mid);
    const float enhancedSide = stereoEnhance.sideEq.process(side);

    enhancedL = enhancedMid + enhancedSide;
    enhancedR = enhancedMid - enhancedSide;

    // LED is a summed activity meter over both adaptive components.
    stereoEnhance.ledAmount = clamp(
        0.65f * stereoEnhance.midActivation +
        0.65f * stereoEnhance.sideActivation,
        0.f,
        1.f
    );
    stereoEnhanceLed = stereoEnhance.ledAmount;
}
else {
    stereoEnhance.targetMidCutDb = 0.f;
    stereoEnhance.smoothedMidCutDb = 0.f;
    stereoEnhance.targetSideLiftDb = 0.f;
    stereoEnhance.smoothedSideLiftDb = 0.f;
    stereoEnhance.midActivation = 0.f;
    stereoEnhance.sideActivation = 0.f;
    stereoEnhance.ledAmount = 0.f;

    if (!stereoEnhance.coeffsNeutral) {
        stereoEnhance.midEq.setPeaking(args.sampleRate, kStereoMidCenterHz, kStereoMidQ, 0.f);
        stereoEnhance.sideEq.setPeaking(args.sampleRate, kStereoSideCenterHz, kStereoSideQ, 0.f);
        stereoEnhance.coeffsNeutral = true;
    }
}
```

Then feed the Saturator from `enhancedL/enhancedR`, not `gluedL/gluedR`:

```cpp
const float preSatPeak = std::max(std::fabs(enhancedL), std::fabs(enhancedR));
// ...
saturatedL = shape(enhancedL);
saturatedR = shape(enhancedR);
```

## 12. LED / UI integration

Add a new light enum entry:

```cpp
STEREO_ENHANCE_LIGHT,
```

Recommended visual/logical placement in `LightId`:

```cpp
LIMITER_ACTIVE_LIGHT,
LOW_RECOVERY_LIGHT,
REMOVE_MUD_LIGHT,
GLUE_COMP_LIGHT,
STEREO_ENHANCE_LIGHT,
SATURATOR_LIGHT,
MICROPEAK_LIGHT,
MASTERING_ENABLED_LIGHT,
REPAIR_ENABLED_LIGHT,
LIGHTS_LEN
```

If preserving existing light IDs is important for the current branch, append `STEREO_ENHANCE_LIGHT` immediately before `LIGHTS_LEN` instead and only place it visually between Glue and Saturator in `SilWidget`. Do not reorder params, inputs, outputs, or lights in released modules unless compatibility has been checked.

Add brightness update near the other mastering LEDs:

```cpp
lights[STEREO_ENHANCE_LIGHT].setSmoothBrightness(
    masteringEnabled ? stereoEnhanceLed : 0.f,
    args.sampleTime
);
```

In `SilWidget`, add an SVG point override:

```cpp
Vec stereoEnhanceLightPos(48.f, 49.6f);
applyPointOverride("STEREO_ENHANCE_LIGHT", &stereoEnhanceLightPos);
```

Add the child light:

```cpp
addChild(createLightCentered<SmallLight<YellowLight>>(
    mm2px(stereoEnhanceLightPos), module, Sil::STEREO_ENHANCE_LIGHT
));
```

If the panel SVG is not updated yet, the fallback position should keep the module compiling and visually place the light in the existing mastering LED cluster.

## 13. Important implementation notes

- Keep all allocation out of `process()`.
- Keep analysis IIR filters separate from EQ biquads.
- Do not use the existing visual FFT for this stage. The adaptive behavior must be audio-rate/local-state, not dependent on display refresh.
- Use smoothed gains, not stepped target gains, to prevent zippering.
- Coefficient updates every 32 samples are acceptable because the gains are already smoothed.
- Force a neutral coefficient update when bypassing or resetting; smoothed gain values alone do not change existing biquad coefficients.
- The `Biquad::setPeaking()` helper already exists and is suitable for both the Mid cut and Side lift.
- The Side lift is not a stereo generator. If there is no side signal, it should not create one.
- This stage should remain subtle. Max movement is only `-2 dB` Mid and `+2 dB` Side.

## 14. Tuning guidance

If the Mid cut over-triggers:

- Increase `kStereoMidExcessThresholdDb` from `1.0` to `2.0`.
- Lower `kStereoMidBandNormDb` from `7.5` to `6.0`.
- Raise `kStereoMidGateDbFs` from `-42` to `-38`.

If the Mid cut never triggers:

- Lower `kStereoMidExcessThresholdDb` from `1.0` to `0.0`.
- Raise `kStereoMidBandNormDb` from `7.5` to `9.0`.

If the Side lift makes bright stereo material too sharp:

- Lower `kStereoSideAlreadyBrightDb` from `1.5` to `0.0`.
- Increase `kStereoSideBrightKneeDb` from `4.0` to `6.0` for gentler fade-out.

If the Side lift almost never activates:

- Raise `kStereoSideAlreadyBrightDb` from `1.5` to `3.0`.
- Lower `kStereoSideGateDbFs` from `-56` to `-62`.

## 15. Acceptance tests

### A. Compile / integration

- `src/Sil.cpp` compiles cleanly under the existing Rack plugin build.
- No new files are required unless updating `res/sil.svg` for the panel anchor.
- No heap allocations occur inside `process()`.
- In WSL / WSL-like shells, treat source checks and `test-fast` as the expected validation path. Do not treat final `plugin.so` link failures as authoritative regressions there; final Rack plugin linking is expected in the Windows/MSYS2 toolchain.

### B. Bypass behavior

- With `MASTERING_ENABLED_PARAM` off, Stereo Enhance produces no EQ gain and LED is dark.
- With Repair enabled and Mastering disabled, existing repair behavior should remain unchanged.
- After toggling Mastering off and back on, the first processed samples should not reuse stale non-zero Stereo Enhance EQ coefficients from the previous enabled state.

### C. Mid cut behavior

Test input: in-phase stereo sine or narrow band around `350 Hz`.

Expected:

- Mid activation rises toward `1.0` after smoothing.
- `smoothedMidCutDb` approaches approximately `-2.0 dB`.
- Side activation remains near `0.0` for mono content.
- Output mono sum shows the 350 Hz cut.

Test input: normal material with weak 350 Hz Mid energy.

Expected:

- `smoothedMidCutDb` remains near `0 dB`, ideally no more than `-0.2 dB`.

### D. Side lift behavior

Test input: stereo/side-rich material with moderate upper presence.

Expected:

- `smoothedSideLiftDb` rises smoothly, commonly in the `+0.5 dB` to `+2.0 dB` range.
- Stereo Enhance LED brightens according to side activation.

Test input: side signal already bright around `6 kHz`.

Expected:

- Side lift is reduced or disabled.
- `smoothedSideLiftDb` should remain near `0 dB` to `+0.5 dB`, depending on threshold tuning.

Test input: mono signal.

Expected:

- Side signal is zero.
- Side lift may compute a target only if detector gates are badly tuned; audible output must remain unchanged by side EQ because side is zero.
- Preferably `sideContentGate` keeps side activation near zero.

### E. Mono compatibility

- Summing output to mono should cancel the Side lift contribution.
- Mono output should only reflect the adaptive Mid cut.

### F. LED behavior

- LED brightness is based on both components:

```cpp
0.65 * midActivation + 0.65 * sideActivation, clamped 0..1
```

- Either component alone should visibly light the LED.
- Both components together may saturate the LED near full brightness.

### G. Listening goal

On dense AI-generated or mastered stereo material, enabling the stage should usually feel like:

- slightly clearer center low-mids,
- slightly more open stereo detail,
- no obvious treble hype when the side channel is already bright,
- no obvious tonal damage when the 350 Hz Mid band is not excessive.
