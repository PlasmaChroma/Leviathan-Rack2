# Specification: Self-Oscillating SVF for Bifurx

## 1. Goal

Bifurx currently uses a linear TPT SVF core. It gives the expected clean SVF response, but it does not produce a stable analog-style self-oscillation when resonance is pushed to the top of the range.

The goal is to add musical self-oscillation without erasing the existing Bifurx character:

- The existing sound at low and middle resonance should remain effectively unchanged.
- Oscillation should begin softly around `RESO ~= 0.8`, with the exact threshold shaped by mode, balance, span, and level.
- The first audible self-oscillation should be sine-like and playable; pushing resonance further should reveal feedback-path saturation and more complex wave shapes.
- `LEVEL` and TITO behavior should still matter; self-oscillation should feel like it belongs to the current Bifurx voice instead of becoming a separate test oscillator.
- The implementation should be cheap enough for audio-rate use and compatible with the existing fast-path/control-cache design.

## 2. Current Code Shape

Relevant implementation points:

- `src/Bifurx.hpp`
  - `kSvfDampingMin = 0.02f`, so effective max Q is currently about 50.
  - `fastTanh()` and `softClip()` already exist.
  - `SvfCoeffs` stores `g`, `k`, and `a1`.
  - `TptSvf::processWithCoeffs()` is the linear core.
  - `processCharacterStage()` already receives `drive` and `resoNorm`, but currently ignores both.

- `src/Bifurx.cpp`
  - `resoToDamping()` maps `RESO` to `2.0 -> 0.03` before later clamps.
  - `Bifurx::process()` clamps stage damping with hard literals `0.02f` and `2.2f`.
  - At very high resonance, `excitation = drivenIn + 1e-6f` seeds the filter.
  - The preview model is linear and assumes `Q = 1 / damping`.
  - Runtime tests exist in `tests/bifurx_runtime_spec.cpp`.
  - Linear response tests/model exist in `tests/bifurx_filter_spec.cpp` and `tests/bifurx_filter_test_model.hpp`.

## 3. Recommended Implementation Path

### 3.1 Keep the Linear Core as the Default

Do not replace `TptSvf::processWithCoeffs()` outright in the first pass. That function is part of the current module voice, preview/probe behavior, and fast path.

Recommended first step:

- Add a second runtime-only method, for example:

```cpp
SvfOutputs TptSvf::processSelfOscWithCoeffs(
	float input,
	const SvfCoeffs& coeffs,
	float oscOnset,
	float oscHeat,
	float oscDrive
);
```

- Call it only from `processCharacterStage()` when resonance is in the self-oscillation zone.
- Leave normal processing routed through `processWithCoeffs()`.

This gives a narrow blast radius. If the nonlinear solver needs tuning, the established linear tone remains untouched for most parameter states.

### 3.2 Use a Soft Hardware-Style Resonance Onset

Avoid changing the whole `RESO` curve. Bifurx already has a defined response and tests around resonance sharpness.

Recommended mapping:

- Keep existing `resoToDamping()` for most of the knob.
- Start the self-oscillation zone around knob 8, but make the first part gentle:

```cpp
constexpr float kSelfOscResoStart = 0.80f;
constexpr float kSelfOscResoFull = 0.98f;
float oscNorm = smoothstep01((resoNorm - kSelfOscResoStart) / (kSelfOscResoFull - kSelfOscResoStart));
```

- Use separate curves for "starts to sing" and "gets nonlinear":

```cpp
float oscOnset = oscNorm * oscNorm;
float oscHeat = smoothstep01((resoNorm - 0.90f) / 0.10f);
```

- Let only this zone reduce damping below the current floor.
- Drive the damping toward zero gradually:

```cpp
constexpr float kSvfSelfOscDampingMin = 0.0005f;
float selfOscDamping = mixf(damping, kSvfSelfOscDampingMin, oscOnset);
```

This is closer to the Belgrad behavior: self-oscillation appears around knob 8, then the remaining resonance travel increases saturation and feedback complexity rather than acting like an on/off switch.

### 3.3 Use Controlled Negative Damping Only in the Oscillation Zone

Pure `k = 0` is not enough for a satisfying self-start. With no input and near-zero internal state, a marginal linear oscillator will only preserve whatever tiny seed it has. To grow from silence into a useful tone, the top of the resonance range needs a small amount of negative damping or an equivalent energy injection. The important constraint is to confine it to the self-oscillation zone and regulate amplitude inside the filter core.

Recommendation:

- Keep ordinary resonance strictly non-negative and linear.
- In the oscillation zone, subtract a small `oscPush` from the damping term using the soft onset curve.
- Add amplitude-dependent damping so the oscillator settles into a sine-like tone instead of growing until the output clip catches it.
- Increase the nonlinear damping/drive with `oscHeat` so the waveform becomes more saturated only as resonance is driven further.
- Avoid broad random-noise injection as the main start mechanism; a tiny deterministic seed is fine, but the core should be able to regulate itself.

Conceptually:

```cpp
float amp = v1 * oscDrive;
float nonlinearDamping = mixf(kSelfOscAmpDampingClean, kSelfOscAmpDampingHot, oscHeat) * amp * amp;
float kEff = c.k - kSelfOscPush * oscOnset + nonlinearDamping;
```

That is closer to a controlled resonator/Van der Pol style behavior than to simply clipping the output.

### 3.4 Put the Nonlinearity in the Resonance Feedback Path

For a TPT SVF, the linear equations are currently:

```cpp
const float v1 = coeffs.a1 * (ic1eq + coeffs.g * (input - ic2eq));
const float v2 = ic2eq + coeffs.g * v1;
out.hp = input - coeffs.k * v1 - v2;
```

The self-oscillating version should regulate the bandpass feedback/damping path, not simply clip the final output. Output clipping would hide instability but would not make the filter core behave like a stable nonlinear resonator.

Conceptual target:

```cpp
float kEff = regulatedDamping(v1, coeffs.k, oscOnset, oscHeat, oscDrive);
out.hp = input - kEff * v1 - v2;
```

Because this creates a nonlinear zero-delay loop, solve it approximately and cheaply.

### 3.5 Prefer an Amplitude-Regulated One-Step Solver

Use the linear solution as the initial estimate, then perform one correction step using an amplitude-dependent damping estimate. This preserves TPT behavior better than a unit-delay saturator and is cheap enough to run at audio rate.

Pseudo-code:

```cpp
SvfOutputs TptSvf::processSelfOscWithCoeffs(float input, const SvfCoeffs& c, float oscOnset, float oscHeat, float oscDrive) {
	const float m = ic1eq + c.g * (input - ic2eq);
	const float onePlusG2 = 1.f + c.g * c.g;
	float v1 = m / (onePlusG2 + c.g * c.k);

	const float drive = std::max(oscDrive, 1e-4f);
	const float amp = v1 * drive;
	const float amp2 = amp * amp;
	const float ampDamping = mixf(kSelfOscAmpDampingClean, kSelfOscAmpDampingHot, oscHeat);
	const float kEff = c.k - kSelfOscPush * oscOnset + ampDamping * amp2;
	v1 = m / std::max(onePlusG2 + c.g * kEff, 1e-5f);

	const float v2 = ic2eq + c.g * v1;
	ic1eq = 2.f * v1 - ic1eq;
	ic2eq = 2.f * v2 - ic2eq;

	SvfOutputs out;
	out.bp = v1;
	out.lp = v2;
	const float outAmp = v1 * drive;
	const float outKEff = c.k - kSelfOscPush * oscOnset + ampDamping * outAmp * outAmp;
	out.hp = input - outKEff * v1 - v2;
	out.notch = out.lp + out.hp;
	return out;
}
```

Candidate starting values:

```cpp
constexpr float kSelfOscPush = 0.006f;
constexpr float kSelfOscAmpDampingClean = 0.0010f;
constexpr float kSelfOscAmpDampingHot = 0.0022f;
```

These are tuning constants, not final truths. The first pass should prefer a slightly slow, polite self-start over a dramatic jump that changes the module's personality.

### 3.6 Let Existing Character Controls Influence the Oscillator

`processCharacterStage()` already receives `drive` and `resoNorm`. Use those rather than adding a new front-panel control.

Suggested first-pass mapping:

```cpp
float oscNorm = smoothstep01((resoNorm - kSelfOscResoStart) / (kSelfOscResoFull - kSelfOscResoStart));
float oscOnset = oscNorm * oscNorm;
float oscHeat = smoothstep01((resoNorm - kSelfOscHeatStart) / (1.f - kSelfOscHeatStart));
float oscDrive = mixf(0.75f, 2.6f, oscHeat) * mixf(0.85f, 1.35f, clamp01((drive - 1.f) / 2.f));
```

Reasoning:

- At ordinary resonance, `oscNorm` is 0 and the old path runs.
- Around knob 8, the oscillator starts softly because `oscOnset` rises quadratically.
- Near full resonance, `oscHeat` makes the feedback path more saturated and distorted.
- `LEVEL` affects how firm or harmonically rich the oscillation is, but does not become an uncontrolled loudness jump.

### 3.7 Seed Self-Oscillation Delicately

The existing `1e-6f` excitation at high resonance is a good starting point. Do not replace it with a large noise source.

Refinement options:

- Keep the existing seed in `Bifurx::process()`.
- If self-start is unreliable, add a tiny deterministic per-stage seed only when `oscNorm > 0` and input energy is near zero.
- Avoid audio-rate random noise for the first pass; it will change the noise floor and may pollute the module's quiet behavior.

### 3.8 Keep Preview Mostly Linear at First

The spectrum preview is a linear response model. A self-oscillating nonlinear filter does not have a simple small-signal response at the limit cycle amplitude.

Recommendation:

- Do not rewrite preview math in the first implementation.
- Continue showing the linear resonance curve up to the top range.
- Clamp displayed Q to a finite value, or annotate the top-end behavior visually later if needed.
- Add runtime/analysis tests for actual oscillator behavior instead of forcing preview to predict it immediately.

This avoids destabilizing a large amount of display code while the audio behavior is still being tuned.

### 3.9 Suggested Implementation Order

Build this in a narrow sequence:

- Add constants and the new `processSelfOscWithCoeffs()` entry point.
- Route only the self-oscillation zone through the new path from `processCharacterStage()`.
- Verify soft onset and bounded output with runtime tests before tuning distortion.
- Tune `oscHeat`, `oscDrive`, and amplitude damping only after the onset and tracking feel right.
- Leave preview/model updates for later unless runtime testing exposes a real mismatch that affects usability.

This keeps the first patch focused on audible behavior, not display parity.

## 4. Specific Code Changes to Plan

### 4.1 Constants

Add explicit constants instead of scattering literals:

```cpp
constexpr float kSvfLinearDampingMin = 0.02f;
constexpr float kSvfSelfOscDampingMin = 0.0005f;
constexpr float kSelfOscResoStart = 0.80f;
constexpr float kSelfOscResoFull = 0.98f;
constexpr float kSelfOscHeatStart = 0.90f;
constexpr float kSelfOscPush = 0.006f;
constexpr float kSelfOscAmpDampingClean = 0.0010f;
constexpr float kSelfOscAmpDampingHot = 0.0022f;
```

Keep `kSvfDampingMin` as the linear/display floor if possible, or rename only if the resulting patch stays small.

### 4.2 Coefficients

`makeSvfCoeffs()` currently clamps with `kSvfDampingMin`. For self-oscillation, either:

- Add `makeSvfCoeffsAllowSelfOsc(sampleRate, cutoff, damping)`, or
- Add an optional damping floor argument:

```cpp
SvfCoeffs makeSvfCoeffs(float sampleRate, float cutoff, float damping, float dampingMin = kSvfDampingMin);
```

The second option is compact, but it touches every caller signature. The first option is noisier but keeps existing call sites visually stable.

### 4.3 Runtime Stage

Update `processCharacterStage()` to choose the path:

```cpp
const float oscNorm = smoothstep01((resoNorm - kSelfOscResoStart) / (kSelfOscResoFull - kSelfOscResoStart));
const float oscOnset = oscNorm * oscNorm;
const float oscHeat = smoothstep01((resoNorm - kSelfOscHeatStart) / (1.f - kSelfOscHeatStart));
const float oscDrive = mixf(0.75f, 2.6f, oscHeat) * mixf(0.85f, 1.35f, clamp01((drive - 1.f) / 2.f));
if (oscNorm <= 0.f) {
	return cachedCoeffsOrNull ? core.processWithCoeffs(input, *cachedCoeffsOrNull)
	                          : core.process(input, sampleRate, cutoff, damping);
}

const float selfDamping = mixf(damping, kSvfSelfOscDampingMin, oscOnset);
const SvfCoeffs coeffs = makeSvfCoeffsAllowSelfOsc(sampleRate, cutoff, selfDamping);
return core.processSelfOscWithCoeffs(input, coeffs, oscOnset, oscHeat, oscDrive);
```

This intentionally bypasses cached linear coeffs only in the self-oscillation zone.

### 4.4 Top-Level Damping Clamp

In `Bifurx::process()`, these literals currently preserve the old floor:

```cpp
dampingA = clamp(baseDamping * fastExp(0.48f * balance), 0.02f, 2.2f);
dampingB = clamp(baseDamping * fastExp(-0.48f * balance), 0.02f, 2.2f);
```

Do not drop these to zero globally. Either keep them as the linear floor and lower damping inside `processCharacterStage()`, or replace `0.02f` with `kSvfDampingMin` for clarity only.

### 4.5 State Guards

Keep `sanitizeCoreState()` as a hard safety rail. Do not use it as the amplitude control.

Recommended additional runtime guard:

- After the nonlinear update, sanitize non-finite values immediately.
- Consider a soft internal state limiter around +/-18V only if testing shows rare overshoot.
- Avoid reducing the existing +/-20 clamp unless normal high-resonance behavior starts hitting it.

## 5. Expected Behavior

At `RESO < ~0.8`:

- Existing Bifurx response should remain effectively unchanged.
- Existing tests should continue to pass without relaxing broad tolerances.

At `RESO ~= 0.8`:

- With no input, the module should start to self-oscillate softly after a short settling time.
- The first tone should be mostly sine-like and should not jump abruptly in level.

At `RESO` near maximum:

- Oscillation pitch should follow `FREQ`, V/OCT, and span-derived A/B cutoffs.
- The filter should behave like one or two playable sine VCOs, depending on mode and span.
- Different modes should expose the oscillator through their existing low/band/high/notch combinations.
- `BALANCE` should still emphasize one stage over the other.
- `TITO` should still cross-modulate the stages and can make the oscillator more complex.
- Higher resonance should reveal more saturated/distorted feedback-path waveforms.
- Output should remain finite and musically bounded without relying on the final output clip alone.

## 6. Test Plan

Add focused tests before broad tuning.

Priority order:

- First: onset shape and bounded output.
- Second: V/OCT tracking and pitch stability.
- Third: hotter resonance waveform changes, TITO interaction, and mode-specific color.

Recommended runtime tests in `tests/bifurx_runtime_spec.cpp`:

- High `RESO`, zero input, neutral TITO: output RMS rises above a small threshold after settling.
- High `RESO`, zero input: output remains finite and bounded for at least 2 seconds at 48 kHz.
- Mid `RESO`, zero input: output remains near silent, protecting old behavior.
- Around `RESO = 0.80`, zero input: output begins below the max-resonance RMS and ramps in without a sudden full-scale jump.
- High `RESO`, two different `FREQ` settings: dominant output period/frequency changes monotonically.
- High `RESO`, V/OCT sweep: measured oscillator frequency tracks reasonably over about five octaves.
- High `RESO`, TITO positive/negative: output stays finite and produces measurably different RMS or spectral character.

Recommended model/test updates:

- Keep `tests/bifurx_filter_test_model.hpp` linear at first unless runtime tests need a duplicate nonlinear core.
- Avoid changing broad preview curve assertions until the preview is deliberately updated.
- Run `make test-fast` for the normal repo check.

## 7. Open Tuning Questions

- Where exactly should self-oscillation begin: `RESO` 0.78, 0.80, or 0.84?
- How wide should the soft onset be before the hotter nonlinear range: full by 0.95, 0.98, or only at max?
- Should `LEVEL` affect only harmonic saturation, or should it also affect oscillator amplitude?
- Should stage A and stage B share the same self-oscillation drive, or should `BALANCE` subtly affect drive as well as damping?
- Should the preview eventually display an "OSC" marker near the top of resonance, or should it remain a pure response analyzer?

## 8. References

- VCV Rack Community: Self-oscillating SVF discussions.
- Vadim Zavalishin, The Art of VA Filter Design.
- Existing `fastTanh()` and `softClip()` helpers in `src/Bifurx.hpp`.
