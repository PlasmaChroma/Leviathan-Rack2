# Bifurx Character Mode Port Requirements

## Purpose

Port Bifurx’s current four `Filter Circuit` personalities from:

```text
SVF / DFM / MS2 / PRD
```

to:

```text
SVF / Acid / Vowel / Corrode
```

The goal is to preserve Bifurx’s strongest existing design: a dual-peak resonant filter whose ten routing combinations operate over semantic outputs:

```text
Low, Band, High, Notch
```

The new modes should behave as **character modes built around the existing SVF-derived semantic output model**, not as unrelated circuit emulations forced to impersonate an SVF.

---

## Current Architecture Summary

The current `Bifurx.cpp` structure already supports the desired port cleanly.

### Existing mode layers

Bifurx currently has two separate mode systems:

1. **Routing/output combination mode**
   - Controlled by `MODE_PARAM`.
   - Uses `kBifurxModeCount = 10`.
   - Labels:
     - `Low + Low`
     - `Low + Band`
     - `Notch + Low`
     - `Notch + Notch`
     - `Low + High`
     - `Band + Band`
     - `High + Low`
     - `High + Notch`
     - `Band + High`
     - `High + High`

2. **Circuit/character mode**
   - Controlled by `FILTER_CIRCUIT_PARAM`.
   - Uses `kBifurxCircuitModeCount = 4`.
   - Current labels:

```cpp
const char* const kBifurxCircuitLabels[kBifurxCircuitModeCount] = {"SVF", "DFM", "MS2", "PRD"};
```

The port should keep the ten routing modes intact and replace only the four circuit/character personalities.

### Important current processing symbols

The implementation should preserve the following existing concepts:

```cpp
struct SvfOutputs {
    float lp = 0.f;
    float bp = 0.f;
    float hp = 0.f;
    float notch = 0.f;
};
```

```cpp
template <typename T>
T combineModeResponse(...);
```

```cpp
struct TptSvf { ... };
```

```cpp
auto processA = [&](float sample) -> SvfOutputs { ... };
auto processB = [&](float sample) -> SvfOutputs { ... };
```

```cpp
struct BifurxPreviewState { ... int circuitMode = 0; };
```

```cpp
BifurxPreviewModel makePreviewModel(const BifurxPreviewState& state);
```

```cpp
float previewModelResponseDb(const BifurxPreviewModel& model, float hz);
```

```cpp
void simulatePreviewProbeImpulseResponse(...);
```

```cpp
void BifurxSpectrumWidget::updateCurveCache();
void BifurxSpectrumWidget::updateOverlayCache(...);
```

---

## Core Design Decision

### Requirement

The new four modes must be implemented as **SVF-compatible character modes**.

Do not implement `Acid`, `Vowel`, or `Corrode` as unrelated filter topologies that only approximate `lp`, `bp`, `hp`, and `notch` after the fact.

### Rationale

The current DFM/MS2/PRD approach creates a mismatch:

```text
Bifurx routing algebra expects true-ish LP/BP/HP/Notch outputs.
DFM/MS2/PRD are nonlinear cascade/lowpass-inspired cores.
Their BP/HP/Notch exports are derived approximations.
```

This makes the alternate modes difficult to balance across all ten routing combinations.

The replacement modes should therefore share one invariant:

```text
Every character mode must return coherent SvfOutputs.
```

---

## New Mode Definitions

### Mode index mapping

Keep `kBifurxCircuitModeCount = 4`.

Replace the labels with:

```cpp
const char* const kBifurxCircuitLabels[kBifurxCircuitModeCount] = {
    "SVF",
    "Acid",
    "Vowel",
    "Corrode"
};
```

Recommended enum:

```cpp
enum BifurxCharacterMode {
    BIFURX_CHARACTER_SVF = 0,
    BIFURX_CHARACTER_ACID = 1,
    BIFURX_CHARACTER_VOWEL = 2,
    BIFURX_CHARACTER_CORRODE = 3
};
```

`filterCircuitMode` may remain named as-is for minimal patch churn, but new code should treat it as `characterMode` conceptually.

### Backward compatibility

Existing patch JSON stores:

```cpp
"filterCircuitMode"
```

Do not change the JSON key in the first pass. Existing saved patches with values `0..3` should load safely.

After the port, saved values map as:

```text
0 -> SVF
1 -> Acid      // formerly DFM
2 -> Vowel     // formerly MS2
3 -> Corrode   // formerly PRD
```

This is acceptable for the first port because the old modes are being replaced. Do not attempt to preserve old DFM/MS2/PRD behavior unless a separate legacy compatibility requirement is added later.

---

## Preview Semantics

The requested preview behavior is:

```text
SVF      preview curve accurate
Acid     preview curve approximate, overlay reveals behavior
Vowel    preview can be reasonably modeled
Corrode  preview approximate, overlay reveals behavior
```

### Existing visual layers

Bifurx already has two relevant preview layers:

1. **Expected curve**
   - Stored in `curveDb` / `curveTargetDb`.
   - Generated in `BifurxSpectrumWidget::updateCurveCache()`.
   - Currently uses:
     - analytic model for `circuitMode == 0`
     - impulse-probe FFT for non-SVF modes

2. **Live overlay**
   - Generated in `BifurxSpectrumWidget::updateOverlayCache()`.
   - Uses real module input/output FFT history.
   - This should be treated as the truth layer for nonlinear modes.

### Required behavior by mode

#### SVF

`SVF` must continue to use the analytic preview curve.

```cpp
if (previewState.circuitMode == BIFURX_CHARACTER_SVF) {
    const BifurxPreviewModel model = makePreviewModel(previewState);
    ... previewModelResponseDb(model, curveHz[i]) ...
}
```

Acceptance criteria:

- Curve tracks cutoff, span, balance, resonance, and routing mode smoothly.
- Curve remains visually stable under no CV.
- Curve remains the most accurate expected response for the clean mode.

#### Acid

`Acid` is nonlinear and level-dependent. The expected curve must be approximate only.

Requirements:

- Use a small-signal impulse/probe preview or an analytic SVF approximation with mild acid modifiers.
- Do not claim or try to render an exact static transfer curve.
- The live overlay should visibly reveal drive-dependent harmonics, resonance bloom, clipping, and spectral lift/cut.
- The overlay must remain smooth enough not to look like random glitter.

Preferred implementation:

```text
Acid expected curve = probe FFT response from processCharacterStage().
Acid truth layer = live overlay from real audio history.
```

#### Vowel

`Vowel` should be mostly modelable. It may use controlled coloration, but should avoid heavy nonlinear behavior.

Requirements:

- Vowel should have a reasonably stable expected curve.
- The analytic model may be extended to include character-specific Q scaling and semantic output weighting.
- The preview curve should be close enough that turning `FREQ`, `SPAN`, `RESO`, `BALANCE`, and `MODE` feels visually intentional.
- Live overlay may still differ due to input material, output saturation, and TITO, but should not be the only useful indicator.

Preferred implementation:

```text
Vowel expected curve = analytic or modelable SVF response with character-specific Q/output shaping.
```

A probe FFT curve is also acceptable if it is stable and smooth, but the Vowel DSP should be designed so it has a coherent quasi-linear response.

#### Corrode

`Corrode` is intentionally nonlinear and destructive.

Requirements:

- Expected curve may be approximate.
- The live overlay is the primary indicator of actual spectral behavior.
- The curve should still provide a useful location/shape hint for the two peaks.
- Corrode must not cause unstable preview spikes, NaNs, or unusable display scale pumping.

Preferred implementation:

```text
Corrode expected curve = probe FFT response from processCharacterStage().
Corrode truth layer = live overlay from real audio history.
```

---

## DSP Architecture Requirements

### New shared processing entry point

Introduce a single function for character-stage processing.

Recommended shape:

```cpp
SvfOutputs processCharacterStage(
    TptSvf& core,
    int characterMode,
    int stageIndex,
    float input,
    float sampleRate,
    float cutoff,
    float damping,
    float drive,
    float resoNorm,
    const SvfCoeffs* cachedCoeffsOrNull = nullptr
);
```

The function must return a complete `SvfOutputs` object.

### Existing stage processors should be simplified

Current `processA` and `processB` branch across `dfmA`, `ms2A`, and `prdA`.

Replace that structure with:

```cpp
auto processA = [&](float sample) -> SvfOutputs {
    return processCharacterStage(
        coreA,
        effectiveCircuitMode,
        0,
        sample,
        args.sampleRate,
        cutoffA,
        dampingA,
        drive,
        resoNorm,
        fastPathEligible ? &cachedCoeffsA : nullptr
    );
};

auto processB = [&](float sample) -> SvfOutputs {
    return processCharacterStage(
        coreB,
        effectiveCircuitMode,
        1,
        sample,
        args.sampleRate,
        cutoffB,
        dampingB,
        drive,
        resoNorm,
        fastPathEligible ? &cachedCoeffsB : nullptr
    );
};
```

The exact function signature may differ, but the code should converge on one shared processing path.

### Legacy cores

The existing structs may be removed or quarantined:

```cpp
DfmCore
Ms2Core
PrdCore
```

Minimum acceptable approach:

- Stop using them in audio processing.
- Stop using them in preview/probe processing.
- Remove their state from `Bifurx` if not needed.
- Remove them from `resetCircuitStates()` and `sanitizeCoreState()` if unused.

Optional preservation approach:

```cpp
#if BIFURX_ENABLE_LEGACY_CIRCUITS
    DfmCore dfmA;
    DfmCore dfmB;
    Ms2Core ms2A;
    Ms2Core ms2B;
    PrdCore prdA;
    PrdCore prdB;
#endif
```

Do not keep unused legacy code interleaved with the new character path unless it materially helps comparison during tuning.

---

## Character DSP Requirements

### 1. SVF mode

SVF mode must remain as close as possible to the current clean behavior.

Requirements:

- Use current `TptSvf::processWithCoeffs()` and `TptSvf::process()` behavior.
- No added saturation beyond the existing upstream/downstream module saturation.
- Must remain eligible for fast path when no CV/TITO modulation is active.
- Preview curve remains analytic and accurate.

Acceptance criteria:

- Existing patches using SVF sound unchanged or nearly unchanged.
- CPU performance in SVF mode does not regress meaningfully.
- Fast path still works.

### 2. Acid mode

Acid mode should sound like nonlinear resonant squelch, not merely “more resonance.”

Required sonic behavior:

- More rubbery resonance at medium/high `RESO`.
- Stronger response to `LEVEL` as input drive.
- Nonlinear feedback or state saturation inside the SVF path.
- Mild output compression/saturation.
- Stable under high resonance, wide span, and fast cutoff modulation.

Recommended DSP ingredients:

```text
input pre-drive
+ nonlinear feedback/state saturation
+ resonance-dependent damping adjustment
+ mild asymmetry or post-core saturation
```

Implementation guidance:

- Start from clean SVF coefficients.
- Apply input shaping before the core.
- Saturate one or both internal states softly.
- Reduce effective damping modestly at higher acid/resonance settings, but clamp hard.
- Do not let acid mode create self-oscillation runaway unless deliberately designed and bounded.

Suggested safety constraints:

```cpp
effectiveDamping >= 0.02f
effectiveDamping <= 2.2f
state values remain finite
output remains finite before final soft clip
```

TITO interaction:

- `TITO` modes `XM` and `SM` should work in Acid.
- Consider reducing coupling depth in Acid mode if instability appears.
- Suggested first-pass guard:

```cpp
const float characterCouplingScale =
    effectiveCircuitMode == BIFURX_CHARACTER_ACID ? 0.75f : 1.f;
```

### 3. Vowel mode

Vowel mode should make the dual peaks feel like a formant/vocal system.

Required sonic behavior:

- `SPAN` should feel like opening/shifting a mouth cavity.
- `BALANCE` should feel like emphasizing lower vs upper formant.
- `Band + Band` should be especially vocal/formant-like.
- `Low + Band`, `Notch + Low`, and `Band + High` should become expressive without collapsing level.
- Should remain less destructive than Acid or Corrode.

Recommended DSP ingredients:

```text
mostly-linear SVF core
+ character-specific Q emphasis
+ bandpass/formant emphasis
+ mild low/high spectral tilt
+ very mild saturation only if needed
```

Preview requirement:

Vowel should be reasonably modeled. Therefore, avoid heavy input-dependent nonlinear behavior.

Implementation guidance:

- Prefer modifying target Q and output semantic weights rather than adding heavy saturation.
- Keep `lp`, `bp`, `hp`, and `notch` coherent.
- Make Vowel mode the “best behaved” alternate character.

Possible first-pass output shaping:

```text
lp: slightly warmed / de-emphasized at high resonance
bp: emphasized, especially at medium/high resonance
hp: slightly softened to avoid thinness
notch: recomputed as lp + hp
```

Example conceptual transform:

```cpp
out.lp = 0.88f * raw.lp + 0.10f * raw.bp;
out.bp = (1.15f + 0.65f * resoNorm) * raw.bp;
out.hp = 0.82f * raw.hp - 0.06f * raw.bp;
out.notch = out.lp + out.hp;
```

Actual tuning should be by ear and by overlay/curve inspection.

### 4. Corrode mode

Corrode mode should be the aggressive industrial/noise personality.

Required sonic behavior:

- More abrasive than Acid.
- Strong response to `LEVEL` and `RESO`.
- Resonant peaks can become scorched, folded, or metallic.
- `High + High`, `Band + Band`, and `Notch + Notch` should become especially useful for industrial textures.
- Must still be bounded and playable.

Recommended DSP ingredients:

```text
SVF core
+ stronger resonant-component saturation
+ optional bandpass fold/clip
+ output soft compression
+ optional odd-harmonic grit
```

Implementation guidance:

- Use the SVF core to preserve semantic outputs.
- Apply destructive shaping to `bp` and/or internal state, not just the final output.
- Recompute `notch = lp + hp` after shaping.
- Keep final output bounded before module output soft clip.

Safety requirements:

- No NaNs.
- No infinities.
- No unbounded state growth.
- No persistent DC offset greater than roughly 100 mV in normal use unless intentionally driven asymmetrically.
- No audio-rate explosions when `RESO=1`, `LEVEL=1`, `SPAN=1`, and TITO is set to `SM` or `XM`.

---

## Character-Specific Scaling Requirements

The current code uses these functions for per-circuit compensation:

```cpp
float circuitCutoffScale(int circuitMode);
float circuitQScale(float resoNorm, int circuitMode);
SemanticExportProfile semanticExportProfile(int circuitMode, int stageIndex);
float modeCircuitSyncCompGain(int mode, int circuitMode, float wideMorph);
```

These should be retained but retuned for the new character modes.

### `circuitCutoffScale()`

Rename later if desired, but first pass may keep the function name.

Suggested first-pass values:

```cpp
float circuitCutoffScale(int characterMode) {
    switch (clampCircuitMode(characterMode)) {
        case BIFURX_CHARACTER_ACID:
            return 1.00f;
        case BIFURX_CHARACTER_VOWEL:
            return 0.92f;
        case BIFURX_CHARACTER_CORRODE:
            return 1.03f;
        case BIFURX_CHARACTER_SVF:
        default:
            return 1.f;
    }
}
```

Rationale:

- SVF remains exact.
- Acid should stay close to SVF cutoff expectations.
- Vowel can sit slightly lower/darker for vocal body.
- Corrode can lean slightly brighter.

### `circuitQScale()`

Suggested first-pass behavior:

```text
SVF:     1.00
Acid:   grows with resonance
Vowel:  moderate, predictable Q emphasis
Corrode: strong, but not runaway
```

Example starting values:

```cpp
float circuitQScale(float resoNorm, int characterMode) {
    const float r = clamp01(resoNorm);
    switch (clampCircuitMode(characterMode)) {
        case BIFURX_CHARACTER_ACID:
            return 1.10f + 0.75f * r;
        case BIFURX_CHARACTER_VOWEL:
            return 1.20f + 0.45f * r;
        case BIFURX_CHARACTER_CORRODE:
            return 1.05f + 1.05f * r;
        case BIFURX_CHARACTER_SVF:
        default:
            return 1.f;
    }
}
```

### `semanticExportProfile()`

Because the new modes should be SVF-compatible, this should become much lighter than the current DFM/MS2/PRD compensation.

First-pass recommendation:

```text
SVF:     no compression profile
Acid:   mild BP/HP limiting only if necessary
Vowel:  mild BP limiting to avoid mode 5 overreach
Corrode: stronger BP/HP limiting for sanity
```

The old values are very large because they were compensating non-SVF cores. Avoid carrying those values forward blindly.

Suggested first-pass profile:

```cpp
SemanticExportProfile semanticExportProfile(int characterMode, int stageIndex) {
    SemanticExportProfile profile;
    switch (clampCircuitMode(characterMode)) {
        case BIFURX_CHARACTER_ACID:
            profile.lpScale = 0.f;
            profile.bpScale = 5.5f;
            profile.hpScale = 5.5f;
            return profile;
        case BIFURX_CHARACTER_VOWEL:
            profile.lpScale = 0.f;
            profile.bpScale = 6.5f;
            profile.hpScale = 0.f;
            return profile;
        case BIFURX_CHARACTER_CORRODE:
            profile.lpScale = 5.5f;
            profile.bpScale = 3.8f;
            profile.hpScale = 4.5f;
            return profile;
        case BIFURX_CHARACTER_SVF:
        default:
            return profile;
    }
}
```

These values are tuning seeds, not final sound-design law.

### `modeCircuitSyncCompGain()`

Retain this function because it is useful for keeping the ten routing modes level-consistent across characters.

Retune its arrays for the new personalities.

Recommended first pass:

```text
SVF:     1.00 baseline
Acid:   slightly lower in aggressive modes if needed
Vowel:  slightly lower in Band+Band if BP is emphasized
Corrode: lower in High+High / Band+Band / Notch+Notch if destructive shaping gets loud
```

Acceptance target:

- Switching character modes should not produce surprising 12 dB jumps at moderate `LEVEL` and `RESO`.
- Character differences may be obvious, but not dangerously loud.

---

## Fast Path Requirements

Current fast path eligibility:

```cpp
const bool fastPathEligible = (effectiveCircuitMode == 0)
    && (tito == 1)
    && !inputs[VOCT_INPUT].isConnected()
    && !inputs[FM_INPUT].isConnected()
    && !inputs[RESO_CV_INPUT].isConnected()
    && !inputs[BALANCE_CV_INPUT].isConnected()
    && !inputs[SPAN_CV_INPUT].isConnected();
```

### Requirement

Keep fast path for SVF first.

Do not require Acid/Vowel/Corrode to use fast path in the first implementation.

Recommended first-pass:

```cpp
const bool fastPathEligible = (effectiveCircuitMode == BIFURX_CHARACTER_SVF)
    && (tito == 1)
    && !inputs[VOCT_INPUT].isConnected()
    && !inputs[FM_INPUT].isConnected()
    && !inputs[RESO_CV_INPUT].isConnected()
    && !inputs[BALANCE_CV_INPUT].isConnected()
    && !inputs[SPAN_CV_INPUT].isConnected();
```

Optional later optimization:

- Enable Vowel fast path if it remains mostly linear and coefficient-driven.
- Enable Acid/Corrode cached-coeff paths only after stability and performance are verified.

---

## Preview Implementation Requirements

### Preview state

Current `BifurxPreviewState` already contains:

```cpp
int circuitMode = 0;
```

No new field is required for first pass.

Optional future fields:

```cpp
bool nonlinearPreview = false;
float characterAmount = 1.f;
```

Do not add these unless needed for display differentiation.

### `makePreviewModel()`

SVF and Vowel may use `makePreviewModel()`.

Requirements:

- `SVF` must use current analytic model.
- `Vowel` may use analytic model if character-specific Q/export shaping is represented.
- `Acid` and `Corrode` should not rely solely on `makePreviewModel()` unless intentionally approximated.

### `updateCurveCache()` routing

Replace the current binary distinction:

```cpp
if (previewState.circuitMode == 0) {
    analytic
}
else {
    probe FFT
}
```

with character-specific preview policy:

```cpp
enum BifurxPreviewCurvePolicy {
    BIFURX_PREVIEW_ANALYTIC,
    BIFURX_PREVIEW_PROBE_FFT
};

BifurxPreviewCurvePolicy previewCurvePolicyForCharacter(int characterMode) {
    switch (clampCircuitMode(characterMode)) {
        case BIFURX_CHARACTER_SVF:
            return BIFURX_PREVIEW_ANALYTIC;
        case BIFURX_CHARACTER_VOWEL:
            return BIFURX_PREVIEW_ANALYTIC; // or PROBE_FFT if easier initially
        case BIFURX_CHARACTER_ACID:
        case BIFURX_CHARACTER_CORRODE:
        default:
            return BIFURX_PREVIEW_PROBE_FFT;
    }
}
```

Minimum acceptable policy:

```text
SVF analytic
Acid probe FFT
Vowel probe FFT initially, analytic later
Corrode probe FFT
```

Preferred policy:

```text
SVF analytic
Acid probe FFT
Vowel analytic/modelable
Corrode probe FFT
```

### Probe engine

`BifurxProbeEngineState` should be simplified to use SVF cores only:

```cpp
struct BifurxProbeEngineState {
    TptSvf svfA;
    TptSvf svfB;
};
```

`processProbeStage()` should call the same character-stage logic as the audio path.

Requirement:

The probe preview must not diverge structurally from the actual audio path. It may use a fixed low probe level, but it should exercise the same character mode code.

---

## UI Requirements

### Existing selector

The current four-LED selector can remain exactly as-is.

LED mapping after port:

```text
Top-left     SVF
Top-right    Acid
Bottom-right Vowel
Bottom-left  Corrode
```

Update comments and any labels accordingly.

Existing light code:

```cpp
lights[FILTER_CIRCUIT_TL_LIGHT].setBrightness(circuitModeLight == 0 ? 1.f : 0.f);
lights[FILTER_CIRCUIT_TR_LIGHT].setBrightness(circuitModeLight == 1 ? 1.f : 0.f);
lights[FILTER_CIRCUIT_BR_LIGHT].setBrightness(circuitModeLight == 2 ? 1.f : 0.f);
lights[FILTER_CIRCUIT_BL_LIGHT].setBrightness(circuitModeLight == 3 ? 1.f : 0.f);
```

Update comments:

```cpp
// SVF
// Acid
// Vowel
// Corrode
```

### Context menu

The existing context menu should automatically show updated labels if `kBifurxCircuitLabels` is changed.

Requirement:

Context menu path should remain:

```text
Filter Circuit -> SVF / Acid / Vowel / Corrode
```

Optional naming improvement:

```text
Character -> SVF / Acid / Vowel / Corrode
```

Do not rename the menu unless panel/docs also adopt the new term.

---

## State Reset and Sanitization Requirements

### Reset

Switching character modes should reset relevant filter state to prevent cross-mode pops/runaway.

Current mode switch calls:

```cpp
setFilterCircuitMode(...)
resetCircuitStates();
```

Requirement:

Keep this behavior.

If only `TptSvf coreA/coreB` remain, reset should at least clear:

```cpp
coreA.ic1eq = 0.f;
coreA.ic2eq = 0.f;
coreB.ic1eq = 0.f;
coreB.ic2eq = 0.f;
```

Also reset telemetry and preview-related filters as currently done.

### Sanitize

Every audio process call should sanitize active core state.

Requirement:

Keep or update:

```cpp
sanitizeCoreState(coreA);
sanitizeCoreState(coreB);
```

If Acid/Corrode add extra character state, that state must be sanitized too.

---

## TITO Requirements

Existing TITO values:

```text
0 -> XM
1 -> Clean
2 -> SM
```

Existing coupling depth:

```cpp
const float couplingDepth = (0.018f + 0.20f * resoNorm * resoNorm) * (tito == 1 ? 0.f : 1.f);
```

Existing modulation states are currently selected by circuit mode:

```cpp
case 1: return dfmA.s1;
case 2: return ms2A.z2;
case 3: return prdA.z2;
default: return coreA.ic1eq;
```

### Requirement

After the port, TITO modulation should use SVF state for all character modes unless a character has dedicated state.

Recommended:

```cpp
auto circuitModStateA = [&]() {
    return coreA.ic1eq;
};

auto circuitModStateB = [&]() {
    return coreB.ic1eq;
};
```

Optional character scaling:

```cpp
float titoCharacterScale(int characterMode) {
    switch (clampCircuitMode(characterMode)) {
        case BIFURX_CHARACTER_ACID:
            return 0.75f;
        case BIFURX_CHARACTER_CORRODE:
            return 0.55f;
        case BIFURX_CHARACTER_VOWEL:
            return 0.90f;
        case BIFURX_CHARACTER_SVF:
        default:
            return 1.f;
    }
}
```

Then:

```cpp
const float couplingDepth = titoCharacterScale(effectiveCircuitMode)
    * (0.018f + 0.20f * resoNorm * resoNorm)
    * (tito == 1 ? 0.f : 1.f);
```

Acceptance criteria:

- TITO remains expressive in all character modes.
- Acid and Corrode do not become uncontrollable at high `RESO`.
- Switching TITO does not cause NaNs or large DC shifts.

---

## Testing Requirements

### Compile/build tests

The module must compile cleanly under the existing Rack plugin build constraints.

Requirements:

- C++11 compatible.
- No new library dependencies.
- No dynamic allocation in the audio-rate hot path.
- No exceptions from audio code.

### Smoke tests

For each character mode:

```text
SVF
Acid
Vowel
Corrode
```

Test each routing mode:

```text
Low + Low
Low + Band
Notch + Low
Notch + Notch
Low + High
Band + Band
High + Low
High + Notch
Band + High
High + High
```

At minimum, run these control positions:

```text
LEVEL:   0.25, 0.50, 1.00
FREQ:    0.10, 0.50, 0.90
RESO:    0.00, 0.50, 0.95, 1.00
SPAN:    0.00, 0.50, 1.00
BALANCE: -1.00, 0.00, 1.00
TITO:    XM, Clean, SM
```

Acceptance criteria:

- No NaNs.
- No infinities.
- Output remains bounded by final soft clip.
- No persistent silence except when the selected filter combination and input make that expected.
- No mode produces dangerous gain jumps compared to neighboring character modes at moderate settings.

### Preview tests

For each character mode:

- Turn `FREQ`: peak markers and curve should move smoothly.
- Turn `SPAN`: markers should spread/contract smoothly.
- Turn `RESO`: curve should visibly sharpen or character should become apparent via overlay.
- Turn `BALANCE`: lower/upper peak weighting should be visually and audibly apparent.
- Switch `MODE`: curve should update to the correct routing combination.
- Switch character modes: preview state should publish and redraw.

Mode-specific preview acceptance:

```text
SVF:
  Expected curve is accurate and stable.

Acid:
  Expected curve is approximate; live overlay reveals extra nonlinear energy and resonance behavior.

Vowel:
  Expected curve is reasonably representative of the tonal/formant shape.

Corrode:
  Expected curve is approximate; live overlay reveals destructive nonlinear coloration.
```

### Performance tests

Use existing `perfDebugLogging` support.

Acceptance targets:

- SVF performance must remain essentially unchanged.
- Vowel should be close to SVF if implemented mostly linearly.
- Acid and Corrode may cost more but should not significantly threaten audio stability.
- UI preview FFT/probe updates must not cause zoom-change freezes or frame spikes beyond current behavior.

### Saved patch tests

- Save patch with each character mode.
- Reload patch.
- Confirm selected character mode persists.
- Confirm no crash when loading older patches with `filterCircuitMode = 0..3`.

---

## Suggested Implementation Order

### Step 1 — Rename mode labels and add enum

- Add `BifurxCharacterMode` enum.
- Replace `kBifurxCircuitLabels` values.
- Update comments for LEDs and context menu if needed.

### Step 2 — Implement shared character stage

- Add `processCharacterStage()`.
- Route SVF mode through existing clean `TptSvf` behavior.
- Temporarily make Acid/Vowel/Corrode call clean SVF until each is implemented.
- Confirm build and behavior unchanged except labels.

### Step 3 — Remove old DFM/MS2/PRD processing path

- Stop branching to `dfmA`, `ms2A`, `prdA` in `processA/processB`.
- Stop using old cores in `processProbeStage()`.
- Simplify reset/sanitize if desired.

### Step 4 — Add Acid

- Add nonlinear SVF processing path.
- Use probe FFT preview.
- Tune for stability at high resonance.
- Tune level compensation.

### Step 5 — Add Vowel

- Add mostly-linear formant-weighted SVF path.
- Prefer analytic preview if feasible.
- Tune `SPAN`, `BALANCE`, and `Band + Band` behavior.

### Step 6 — Add Corrode

- Add destructive resonant shaping.
- Use probe FFT preview.
- Tune anti-runaway clamps and output level.

### Step 7 — Retune compensation

- Retune `circuitCutoffScale()`.
- Retune `circuitQScale()`.
- Retune `semanticExportProfile()`.
- Retune `modeCircuitSyncCompGain()`.

### Step 8 — Full regression pass

- Run smoke matrix.
- Check preview behavior.
- Check saved patch behavior.
- Check performance logs.

---

## Non-Goals

Do not implement these in the first port unless explicitly requested later:

- A fifth mode.
- New front-panel controls.
- A literal TB-303 clone.
- A literal MS-20 clone.
- A literal diode ladder clone.
- Oversampling.
- New SVG panel layout.
- Exact nonlinear frequency-response visualization.
- Per-mode custom labels on the panel.

---

## Final Target Behavior

After the port, Bifurx should feel like one coherent dual-resonant instrument with four personalities:

```text
SVF
  Clean, precise, visually accurate.

Acid
  Rubbery nonlinear squelch. Curve gives a hint; overlay shows the beast.

Vowel
  Vocal/formant dual-peak behavior. Curve remains meaningfully predictive.

Corrode
  Industrial resonant damage. Curve gives position; overlay reveals the burn.
```

The ten existing routing modes remain the same. The filter personality changes, but the dual-peak algebra remains intact.

The design principle is:

```text
Do not replace the Bifurx machine.
Give the same machine four masks.
```
