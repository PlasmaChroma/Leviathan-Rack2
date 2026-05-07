# Sil Midrange Enhancer Implementation Spec

Target source: `src/Sil.cpp` current repo state  
Feature name: **Midrange Enhance** / **MID_ENHANCE_LIGHT**

## Intent

Add a subtle adaptive midrange enhancer to Sil’s mastering chain. The goal is not an obvious “smile correction” or vocal-forward effect. It should behave like a mastering-grade conditional dynamic EQ: it gently restores midrange presence only when the post-mud-removal signal appears under-supported in the useful midrange.

This is frequency midrange enhancement, **not** Mid/Side mid-channel processing. Apply the same dynamic peaking EQ to left and right channels to preserve stereo image and avoid fighting the existing `StereoEnhanceState` stage.

## Chain placement

Insert immediately after Remove Mud and before Glue Compressor:

```text
Input
  -> Low-band mono recovery
  -> Impact Air
  -> Remove Mud
  -> Midrange Enhance      // new stage
  -> Glue Compressor
  -> Stereo Enhance
  -> Saturator
  -> Final limiter
  -> Output
```

Rationale:

- Remove Mud may carve 180–520 Hz congestion. The midrange enhancer should evaluate the cleaned signal, then restore perceptual center/body only if the post-mud tone has become too recessed.
- Placing it before Glue lets the glue compressor integrate the restored midrange rather than allowing the enhancer to poke out as a separate EQ move.
- Placing it before Stereo Enhance prevents confusion with M/S spectral widening and keeps this feature mono-compatible.

## Warm A/B bypass policy

Sil's adaptive mastering stages should keep their detector and adaptive state warm while Mastering is disabled. This gives users a fair A/B comparison: when they turn Mastering back on, they hear what the mastering chain would be doing at that moment, not the startup transient of cold envelopes, empty rolling stats, or neutral adaptive EQ.

For Midrange Enhance:

- Run the detector bands, envelope followers, activation math, gain smoothing, coefficient updates, and L/R peaking biquads regardless of `masteringEnabled`.
- Keep final audible output dry while `masteringEnabled` is false, using the module's existing final output selection pattern.
- Gate `MID_ENHANCE_LIGHT` off while `masteringEnabled` is false, matching the other mastering-stage lights.
- Do not add a parameter, input, output, or serialized state for this stage. The feature only adds internal DSP state and one light.

## DSP model

Use **detector-band-driven dynamic peaking EQ**, not true multiband split/recombine dynamics.

Why:

- A full multiband crossover stage is heavier, adds phase/crossover interaction, and is excessive for a generic mastering chain.
- A detector-only multiband analysis feeding one broad peaking EQ is efficient, stable, easy to meter, and consistent with the existing Remove Mud / Impact Air / Stereo Enhance idioms in `Sil.cpp`.

### Tonal behavior

The stage should apply a broad lift centered around the intelligibility/body midrange:

```cpp
static constexpr float kMidEnhanceCenterHz = 1450.f;
static constexpr float kMidEnhanceQ = 0.72f;
static constexpr float kMidEnhanceMaxLiftDb = 0.85f;
```

This should produce a subtle “forwardness / articulation / living center” lift without becoming nasal, harsh, or obviously EQ’d. `0.85 dB` is intentionally conservative. Do not exceed `1.0 dB` unless a later tuning pass proves the chain is still too recessed.

## Detector bands

Analyze mono after Remove Mud:

```cpp
const float midEnhanceMono = 0.5f * (mudCleanL + mudCleanR);
```

Use simple `dsp::RCFilter` HP -> LP bandpass detectors, following the existing Remove Mud and Stereo Enhance style.

Recommended bands:

```cpp
// Low-mid/body reference after mud cleanup.
static constexpr float kMidEnhanceLowRefLowHz = 140.f;
static constexpr float kMidEnhanceLowRefHighHz = 560.f;

// Core target band: the range to restore when recessed.
static constexpr float kMidEnhanceCoreLowHz = 700.f;
static constexpr float kMidEnhanceCoreHighHz = 2400.f;

// Presence/harshness guard: prevents boosting when upper mids are already forward.
static constexpr float kMidEnhancePresenceLowHz = 2600.f;
static constexpr float kMidEnhancePresenceHighHz = 6500.f;
```

The low reference intentionally starts above sub/bass and overlaps the top of mud-removal territory. This lets the stage respond to “we cleaned the murk but lost perceived body” without becoming a low-mid restorer.

## State additions

Add coefficients near the existing dynamic coefficient fields:

```cpp
float midEnhanceEnvAttackCoeff = 0.f;
float midEnhanceEnvReleaseCoeff = 0.f;
float midEnhanceGainAttackCoeff = 0.f;
float midEnhanceGainReleaseCoeff = 0.f;
```

Add a new state struct after `RemoveMudState` or before `GlueCompressorState`:

```cpp
struct MidrangeEnhanceState {
    dsp::RCFilter lowRefHp;
    dsp::RCFilter lowRefLp;
    dsp::RCFilter coreHp;
    dsp::RCFilter coreLp;
    dsp::RCFilter presenceHp;
    dsp::RCFilter presenceLp;

    float lowRefEnv = 1e-6f;
    float coreEnv = 1e-6f;
    float presenceEnv = 1e-6f;

    float targetLiftDb = 0.f;
    float smoothedLiftDb = 0.f;
    float activation = 0.f;
    float ledAmount = 0.f;

    dsp::ClockDivider coeffDivider;
    Biquad liftL;
    Biquad liftR;
} midEnhance;
```

## Constants

Add near the other mastering constants:

```cpp
static constexpr float kMidEnhanceLowRefLowHz = 140.f;
static constexpr float kMidEnhanceLowRefHighHz = 560.f;
static constexpr float kMidEnhanceCoreLowHz = 700.f;
static constexpr float kMidEnhanceCoreHighHz = 2400.f;
static constexpr float kMidEnhancePresenceLowHz = 2600.f;
static constexpr float kMidEnhancePresenceHighHz = 6500.f;

static constexpr float kMidEnhanceCenterHz = 1450.f;
static constexpr float kMidEnhanceQ = 0.72f;
static constexpr float kMidEnhanceMaxLiftDb = 0.85f;

static constexpr float kMidEnhanceGateDbFs = -50.f;
static constexpr float kMidEnhanceGateKneeDb = 12.f;
static constexpr float kMidEnhanceDeficitThresholdDb = 1.15f;
static constexpr float kMidEnhanceDeficitKneeDb = 4.50f;
static constexpr float kMidEnhanceRefBiasDb = 0.75f;
static constexpr float kMidEnhanceRemoveMudAssistDb = 0.35f;

static constexpr float kMidEnhancePresenceNormDb = 1.75f;
static constexpr float kMidEnhancePresenceGuardThresholdDb = 2.50f;
static constexpr float kMidEnhancePresenceGuardKneeDb = 5.00f;

static constexpr float kMidEnhanceLimiterBackoffStartDb = 0.75f;
static constexpr float kMidEnhanceLimiterBackoffKneeDb = 1.25f;

static constexpr float kMidEnhanceEnvAttackSec = 0.050f;
static constexpr float kMidEnhanceEnvReleaseSec = 0.420f;
static constexpr float kMidEnhanceGainAttackSec = 0.350f;
static constexpr float kMidEnhanceGainReleaseSec = 1.250f;
static constexpr int kMidEnhanceCoeffDivision = 64;

static constexpr float kMidEnhanceLedDeadbandDb = 0.08f;
```

Tuning notes:

- `kMidEnhanceRefBiasDb` prevents the stage from treating ordinary spectral slope as a deficit.
- `kMidEnhanceRemoveMudAssistDb` makes the enhancer slightly more willing to act when Remove Mud is active, without hard-coupling the two stages.
- `kMidEnhanceLimiterBackoffStartDb` prevents the enhancer from adding energy when the final limiter has recently been working hard.
- The `refEnv` blend intentionally includes a small presence contribution. If presence-heavy material still triggers lift in practice, reduce or remove the presence contribution from `refEnv` before increasing the presence guard strength.
- Keep envelope floors at `1e-6f` and avoid allowing detector envelopes to decay to exact zero. This matches the existing denormal-safe style used by Remove Mud and Stereo Enhance.

## Coefficient/update functions

Add:

```cpp
void updateMidEnhanceCutoffs(float sampleRate) {
    const auto norm = [&](float hz) {
        return clamp(hz / std::max(sampleRate, 1.f), 1e-5f, 0.49f);
    };
    midEnhance.lowRefHp.setCutoff(norm(kMidEnhanceLowRefLowHz));
    midEnhance.lowRefLp.setCutoff(norm(kMidEnhanceLowRefHighHz));
    midEnhance.coreHp.setCutoff(norm(kMidEnhanceCoreLowHz));
    midEnhance.coreLp.setCutoff(norm(kMidEnhanceCoreHighHz));
    midEnhance.presenceHp.setCutoff(norm(kMidEnhancePresenceLowHz));
    midEnhance.presenceLp.setCutoff(norm(kMidEnhancePresenceHighHz));
}
```

Extend `updateDynamicsCoefficients(float sampleRate)`:

```cpp
midEnhanceEnvAttackCoeff = std::exp(-1.f / (kMidEnhanceEnvAttackSec * sr));
midEnhanceEnvReleaseCoeff = std::exp(-1.f / (kMidEnhanceEnvReleaseSec * sr));
midEnhanceGainAttackCoeff = std::exp(-1.f / (kMidEnhanceGainAttackSec * sr));
midEnhanceGainReleaseCoeff = std::exp(-1.f / (kMidEnhanceGainReleaseSec * sr));
```

Add reset helper:

```cpp
void resetMidEnhanceState() {
    midEnhance.lowRefEnv = 1e-6f;
    midEnhance.coreEnv = 1e-6f;
    midEnhance.presenceEnv = 1e-6f;
    midEnhance.targetLiftDb = 0.f;
    midEnhance.smoothedLiftDb = 0.f;
    midEnhance.activation = 0.f;
    midEnhance.ledAmount = 0.f;
    midEnhance.lowRefHp.reset();
    midEnhance.lowRefLp.reset();
    midEnhance.coreHp.reset();
    midEnhance.coreLp.reset();
    midEnhance.presenceHp.reset();
    midEnhance.presenceLp.reset();
    midEnhance.liftL.reset();
    midEnhance.liftR.reset();
}
```

Wire these in constructor and sample-rate handling:

```cpp
midEnhance.coeffDivider.setDivision(kMidEnhanceCoeffDivision);
updateMidEnhanceCutoffs(APP->engine->getSampleRate());
midEnhance.liftL.setPeaking(APP->engine->getSampleRate(), kMidEnhanceCenterHz, kMidEnhanceQ, 0.f);
midEnhance.liftR.setPeaking(APP->engine->getSampleRate(), kMidEnhanceCenterHz, kMidEnhanceQ, 0.f);
```

And in `onSampleRateChange()`:

```cpp
updateMidEnhanceCutoffs(e.sampleRate);
resetMidEnhanceState();
midEnhance.liftL.setPeaking(e.sampleRate, kMidEnhanceCenterHz, kMidEnhanceQ, 0.f);
midEnhance.liftR.setPeaking(e.sampleRate, kMidEnhanceCenterHz, kMidEnhanceQ, 0.f);
```

`midEnhance.coeffDivider` only needs to be configured in the constructor. Sample-rate changes should update detector cutoffs, reset state, then initialize `liftL` / `liftR` to 0 dB as shown above.

## Process block insertion

Replace the current transition:

```cpp
const float preMasterL = mudCleanL;
const float preMasterR = mudCleanR;
const sil_micropeak::StereoSample cleaned(preMasterL, preMasterR);
pushRollingSample(cleaned.l, cleaned.r);
```

with:

```cpp
float midEnhancedL = mudCleanL;
float midEnhancedR = mudCleanR;
float midEnhanceLed = 0.f;
{
    const float monoPostMud = 0.5f * (mudCleanL + mudCleanR);

    midEnhance.lowRefHp.process(monoPostMud);
    const float lowRefHigh = midEnhance.lowRefHp.highpass();
    midEnhance.lowRefLp.process(lowRefHigh);
    const float lowRefBand = midEnhance.lowRefLp.lowpass();

    midEnhance.coreHp.process(monoPostMud);
    const float coreHigh = midEnhance.coreHp.highpass();
    midEnhance.coreLp.process(coreHigh);
    const float coreBand = midEnhance.coreLp.lowpass();

    midEnhance.presenceHp.process(monoPostMud);
    const float presenceHigh = midEnhance.presenceHp.highpass();
    midEnhance.presenceLp.process(presenceHigh);
    const float presenceBand = midEnhance.presenceLp.lowpass();

    auto updateMidEnhanceEnv = [&](float& env, float x) {
        const float absX = std::fabs(x);
        const float c = (absX > env) ? midEnhanceEnvAttackCoeff : midEnhanceEnvReleaseCoeff;
        env = absX + c * (env - absX);
    };
    updateMidEnhanceEnv(midEnhance.lowRefEnv, lowRefBand);
    updateMidEnhanceEnv(midEnhance.coreEnv, coreBand);
    updateMidEnhanceEnv(midEnhance.presenceEnv, presenceBand);

    const float refEnv = 0.68f * midEnhance.lowRefEnv + 0.32f * midEnhance.presenceEnv;
    const float activityEnv = std::max(midEnhance.coreEnv, refEnv);
    const float levelGate = softKnee01(
        toDbFsSafe(activityEnv),
        kMidEnhanceGateDbFs,
        kMidEnhanceGateKneeDb
    );

    const float thresholdDb = kMidEnhanceDeficitThresholdDb -
        kMidEnhanceRemoveMudAssistDb * clamp(removeMud.ledAmount, 0.f, 1.f);
    const float deficitDb =
        toDbSafe(refEnv / std::max(midEnhance.coreEnv, 1e-7f)) - kMidEnhanceRefBiasDb;
    const float deficit = softKnee01(deficitDb, thresholdDb, kMidEnhanceDeficitKneeDb);

    const float presenceRatioDb =
        toDbSafe(midEnhance.presenceEnv / std::max(midEnhance.coreEnv, 1e-7f)) +
        kMidEnhancePresenceNormDb;
    const float presenceGuard = inverseSoftKnee01(
        presenceRatioDb,
        kMidEnhancePresenceGuardThresholdDb,
        kMidEnhancePresenceGuardKneeDb
    );

    const float limiterBackoff = inverseSoftKnee01(
        limiterRecentGrDb,
        kMidEnhanceLimiterBackoffStartDb,
        kMidEnhanceLimiterBackoffKneeDb
    );

    midEnhance.activation = clamp(levelGate * deficit * presenceGuard * limiterBackoff, 0.f, 1.f);
    midEnhance.targetLiftDb = kMidEnhanceMaxLiftDb * midEnhance.activation;

    const float liftCoeff =
        (midEnhance.targetLiftDb > midEnhance.smoothedLiftDb)
            ? midEnhanceGainAttackCoeff
            : midEnhanceGainReleaseCoeff;
    midEnhance.smoothedLiftDb =
        midEnhance.targetLiftDb + liftCoeff * (midEnhance.smoothedLiftDb - midEnhance.targetLiftDb);

    if (midEnhance.coeffDivider.process()) {
        midEnhance.liftL.setPeaking(
            args.sampleRate,
            kMidEnhanceCenterHz,
            kMidEnhanceQ,
            midEnhance.smoothedLiftDb
        );
        midEnhance.liftR.setPeaking(
            args.sampleRate,
            kMidEnhanceCenterHz,
            kMidEnhanceQ,
            midEnhance.smoothedLiftDb
        );
    }

    // Always process the peaking biquads so their internal state stays coherent
    // through neutral periods and does not re-enter stale when activation returns.
    midEnhancedL = midEnhance.liftL.process(mudCleanL);
    midEnhancedR = midEnhance.liftR.process(mudCleanR);

    const float activeDb = std::max(0.f, midEnhance.smoothedLiftDb - kMidEnhanceLedDeadbandDb);
    const float ledNorm = clamp(
        activeDb / std::max(kMidEnhanceMaxLiftDb - kMidEnhanceLedDeadbandDb, 1e-6f),
        0.f,
        1.f
    );
    midEnhance.ledAmount = std::sqrt(ledNorm);
    midEnhanceLed = midEnhance.ledAmount;
}

const float preMasterL = midEnhancedL;
const float preMasterR = midEnhancedR;
const sil_micropeak::StereoSample cleaned(preMasterL, preMasterR);
pushRollingSample(cleaned.l, cleaned.r);
```

Process note: this block should run even when `masteringEnabled` is false so A/B state remains warm. The final output selection later in `process()` should continue to decide whether the user hears the mastered signal or dry input.

## Light state and UI wiring

Add a new light ID after Remove Mud and before Glue:

```cpp
enum LightId {
    LIMITER_ACTIVE_LIGHT,
    LOW_RECOVERY_LIGHT,
    IMPACT_AIR_LIGHT,
    REMOVE_MUD_LIGHT,
    MID_ENHANCE_LIGHT,
    GLUE_COMP_LIGHT,
    STEREO_ENHANCE_LIGHT,
    SATURATOR_LIGHT,
    MICROPEAK_LIGHT,
    MASTERING_ENABLED_LIGHT,
    REPAIR_ENABLED_LIGHT,
    LIGHTS_LEN
};
```

Update the light smoothing block:

```cpp
lights[MID_ENHANCE_LIGHT].setSmoothBrightness(masteringEnabled ? midEnhanceLed : 0.f, lightDt);
```

`midEnhanceLed` should be initialized before the Midrange Enhance block and remain in scope for the existing `lightDivider` block.

Update `ChainLedDebugReadoutWidget`:

```cpp
static constexpr int kCount = 8;
std::array<int, kCount> lightIds = {
    Sil::LIMITER_ACTIVE_LIGHT,
    Sil::LOW_RECOVERY_LIGHT,
    Sil::IMPACT_AIR_LIGHT,
    Sil::REMOVE_MUD_LIGHT,
    Sil::MID_ENHANCE_LIGHT,
    Sil::GLUE_COMP_LIGHT,
    Sil::STEREO_ENHANCE_LIGHT,
    Sil::SATURATOR_LIGHT
};
```

Update widget placement. Add a fallback position:

```cpp
Vec midEnhanceLightPos(48.f, 48.4f);
Vec glueCompLightPos(48.f, 49.2f);
Vec stereoEnhanceLightPos(48.f, 50.0f);
Vec saturatorLightPos(48.f, 50.8f);
```

Add SVG override support:

```cpp
applyPointOverride("MID_ENHANCE_LIGHT", &midEnhanceLightPos);
```

Add the actual light:

```cpp
addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(midEnhanceLightPos), module, Sil::MID_ENHANCE_LIGHT));
```

And include it in debug text positions between Remove Mud and Glue.

Fallback spacing note: keep the visible order as Remove Mud -> MID+ -> Glue -> Stereo Enhance -> Saturator. If the current panel art has enough vertical room, prefer preserving the existing light spacing and shifting the lower chain lights down rather than crowding four lights into the old lower stack. The SVG component point should be treated as authoritative once added.

Panel note: add a `MID_ENHANCE_LIGHT` point to `res/sil.svg` in the same components-layer convention as the other chain lights. If the SVG is not updated immediately, the fallback above will still render the light.

## Efficiency constraints

The stage should add only:

- 6 one-pole RC filter processes on mono audio.
- 3 envelope followers.
- A small amount of scalar math.
- 2 peaking biquads on L/R every sample, including neutral periods, to keep filter state continuous.
- Coefficient recomputation once per `64` samples.

Do **not** add an FFT, lookahead buffer, crossover split, extra worker thread, or per-sample coefficient recomputation.

If profiling still flags the stage, the first optimization is to raise `kMidEnhanceCoeffDivision` from `64` to `128`. Do not remove the post-mud detector bands unless necessary; the whole point is that this stage evaluates the signal after Remove Mud.

## Expected behavior

### Silence / very quiet input

- No boost.
- LED off.
- No denormals or NaNs.

### Balanced full mix

- Usually 0–0.25 dB lift.
- LED mostly off or a faint glow.

### Mud-heavy material after Remove Mud cuts

- Remove Mud cuts 180–520 Hz congestion.
- Midrange Enhance may rise to ~0.25–0.65 dB if the cleaned result feels hollow/recessed.
- Glue then gently integrates the change.

### Already vocal/guitar-forward material

- No meaningful boost.
- LED low/off.

### Harsh/sibilant/upper-mid-heavy material

- Presence guard suppresses lift.
- LED off or minimal.

### Limiter-overworked material

- If `limiterRecentGrDb` is already elevated from previous samples, the stage backs off to avoid feeding more level into saturation/limiting.

## Acceptance tests

1. **Mastering off A/B behavior**  
   With Mastering disabled, the stage should continue updating detector state, smoothed lift, coefficients, and biquad state for coherent A/B recall. Output should remain dry input and `MID_ENHANCE_LIGHT` should be gated off like the other mastering lights.

2. **No click on activation**  
   Feed pink noise or a looped mix and ensure the dynamic lift ramps smoothly. No zippering should be audible when coefficients update every 64 samples. The peaking biquads should continue processing while neutral so stale filter state cannot reappear on reactivation.

3. **Subtle max effect**  
   Force activation with a synthetic recessed-mid test signal. Verify `smoothedLiftDb <= kMidEnhanceMaxLiftDb` and output does not sound like an obvious EQ preset.

4. **Presence guard**  
   Feed content dominated by 3–6 kHz. Verify `presenceGuard` suppresses activation.

5. **Remove Mud interaction**  
   On mud-heavy content, Remove Mud should activate first. Midrange Enhance may follow gently, but should not simply mirror Remove Mud 1:1.

6. **Source-level detector checks**  
   Add focused coverage or a debug-only harness for: silence produces no NaNs and no lift; a recessed-core synthetic signal drives `smoothedLiftDb > 0`; a 3–6 kHz dominated signal keeps activation near zero through the presence guard.

7. **Compatibility guard**  
   Confirm no params, inputs, outputs, or serialized JSON fields are added or reordered. The implementation should add only internal state and `MID_ENHANCE_LIGHT`.

8. **Debug readout**  
   DragonKing debug readout should show 8 chain lights in order and include the new MID_ENHANCE value between REMOVE_MUD and GLUE_COMP.

## Naming recommendation

In code, use `MidrangeEnhanceState` and `MID_ENHANCE_LIGHT`. On panel/art labels, prefer one of:

- `MID` — shortest, readable in the existing chain stack.
- `MID+` — communicates conditional enhancement.
- `CENTER` — more poetic, but could be confused with stereo center.

Recommended panel label: **MID+**.
