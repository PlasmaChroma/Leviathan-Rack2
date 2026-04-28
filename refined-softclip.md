# Bifurx Refined Softclip Implementation Spec

## Purpose

This spec defines a conservative refinement of Bifurx gain staging so the default `LEVEL = 0.5` setting is a clean unity baseline while preserving the upper half of the knob as a drive and character range.

The immediate motivation is the lowpass flat-region dip and purple overlay tint seen before the first peak. The analyzer compares raw input against final output, so always-on input and output softclip stages make the overlay report a real level loss even when the filter topology itself should be neutral in the passband.

## Current Behavior

Bifurx currently derives a single `drive` value from `LEVEL` and always applies nonlinear shaping:

```cpp
const float drive = levelDriveGain(level);
const float drivenIn = 5.f * bifurx::softClip(0.2f * in * drive);
...
const float out = 5.5f * bifurx::softClip(modeOut / 5.5f);
```

The quick midpoint gain fix changed `levelDriveGain(0.5)` to exactly `1.0`, but that only fixes the small-signal gain law. It does not make the default path clean, because ordinary +/-5V modular signals still pass through `tanh`-style compression at both the input and output.

## Target Contract

`LEVEL` should behave as:

```text
0.0 - 0.5:
    clean gain trim
    no intentional saturation
    0.5 is exact unity

0.5 - 1.0:
    unity-or-hot gain
    increasing input drive
    output soft limiting fades in gradually
```

At `LEVEL = 0.5`, a low-amplitude signal in a unity passband should measure approximately `0 dB` through the module. For normal +/-5V material, the default should be clean unless the filter mode and resonance themselves create gain.

## Proposed Helpers

Replace the single-purpose `levelDriveGain()` contract with explicit helpers in `src/Bifurx.cpp` and declarations in `src/Bifurx.hpp`.

```cpp
float smoothstep01(float x) {
	const float t = bifurx::clamp01(x);
	return t * t * (3.f - 2.f * t);
}

float levelInputGain(float knob) {
	const float x = bifurx::clamp01(knob);
	if (x <= 0.5f) {
		return 2.f * x;
	}

	const float hot = (x - 0.5f) * 2.f;
	return 1.f + 2.5f * hot * hot;
}

float levelDriveAmount(float knob) {
	const float x = bifurx::clamp01(knob);
	if (x <= 0.5f) {
		return 0.f;
	}

	const float hot = (x - 0.5f) * 2.f;
	return hot * hot;
}

float levelOutputClipWet(float knob) {
	const float x = bifurx::clamp01(knob);
	return smoothstep01((x - 0.5f) * 2.f);
}
```

Use a named constant for maximum drive rather than burying it in the process path:

```cpp
constexpr float kLevelMaxDriveGain = 4.5f;
```

This preserves the old rough top-end drive range, because the previous curve reached about `4.625` at `LEVEL = 1.0`.

## Input Stage

Replace the always-on input softclip with a clean path below midpoint and a blended drive path above midpoint.

```cpp
const float level = params[LEVEL_PARAM].getValue();
const float inputGain = levelInputGain(level);
const float driveAmount = levelDriveAmount(level);

float excitation = in * inputGain;

if (driveAmount > 1e-5f) {
	const float driveGain = 1.f + (kLevelMaxDriveGain - 1.f) * driveAmount;
	const float driven = 5.f * bifurx::softClip((excitation * driveGain) / 5.f);
	excitation = bifurx::mixf(excitation, driven, driveAmount);
}

excitation += (resoNorm > 0.985f ? 1e-6f : 0.f);
```

Important details:

- The clean path is exactly `in` at `LEVEL = 0.5`.
- Below midpoint, the knob is a simple clean attenuator.
- Above midpoint, `driveAmount` controls both how hard the signal is pushed and how much of the saturated result is used.
- Keep passing a drive-related scalar into `processCharacterStage()` for now, but treat that as compatibility plumbing unless that function starts using the value again.

## Output Stage

Replace the always-on final softclip with a wet blend that is dry at midpoint and fully soft-limited at maximum level.

```cpp
const float cleanOut = bifurx::sanitizeFinite(modeOut);
const float clippedOut = 5.5f * bifurx::softClip(cleanOut / 5.5f);
const float clipWet = levelOutputClipWet(level);
const float out = bifurx::sanitizeFinite(bifurx::mixf(cleanOut, clippedOut, clipWet));
```

This avoids a hard discontinuity near `0.5` and preserves the existing softclip character in the upper half of the knob.

## Preview Probe Parity

Update `simulatePreviewProbeImpulseResponse()` to use the same helper functions and signal path as runtime processing.

Current preview probe code also always softclips:

```cpp
const float excitation = 5.f * bifurx::softClip(0.2f * rawIn * drive);
...
outputBuffer[i] = 5.5f * bifurx::softClip(modeOut / 5.5f);
```

The preview probe should instead compute excitation and output through shared helpers or identical local code. The implementation should avoid drift between:

- `Bifurx::process()`
- `simulatePreviewProbeImpulseResponse()`
- `tests/bifurx_filter_test_model.hpp`

Preferred structure:

```cpp
float applyLevelInputStage(float in, float level);
float applyLevelOutputStage(float modeOut, float level);
```

If these helpers are small and pure, both runtime and preview probe can call the same code directly.

## Tests

Update `tests/bifurx_filter_test_model.hpp` to mirror the new helper functions. The test model must not keep the old always-on softclip behavior.

Add focused tests:

1. `LEVEL midpoint is clean unity for small signal`
   - Use a low resonance lowpass case with a passband test tone.
   - Assert output/input is close to `0 dB` with a narrow tolerance, allowing only normal filter numerical error.

2. `LEVEL midpoint does not compress normal modular amplitude`
   - Use a representative +/-5V sine in a flat passband.
   - Assert it is not reduced by the old tanh compression amount.

3. `LEVEL upper half still adds saturation`
   - Compare harmonic content, RMS compression, or peak limiting at `LEVEL = 1.0` against `LEVEL = 0.5`.
   - This can be broad; the goal is to catch accidental removal of the drive behavior.

4. `Preview probe and runtime stay aligned`
   - Extend the existing runtime preview alignment tests if practical.
   - Cover at least `LEVEL = 0.5` and `LEVEL = 1.0` if the test harness exposes level.

Expected existing test movement:

- Tests that use `LEVEL = 0.5` may report higher output levels because hidden default compression is removed.
- Runtime LL floor/dropout tests may need updated numeric thresholds.
- The mixed-notch regression should continue to pass because its topology fix is independent of softclip behavior.

## Analyzer and Overlay Expectations

No analyzer code change is required for this pass.

The overlay currently measures raw input versus final output:

```cpp
pushAnalysisSample(in, out);
```

After this change, that remains the correct measurement. The difference is that a clean default passband should no longer show a level-loss tint caused by default gain staging. Any remaining purple should indicate actual filter response, resonance interaction, or output-level behavior above midpoint.

## Compatibility and Side Effects

This is a behavior change for existing patches:

- Default Bifurx will become cleaner and may sound louder on hot signals.
- Old patches that relied on always-on default rounding will lose some saturation at `LEVEL = 0.5`.
- High resonance modes may produce larger peaks at default because the final softclip is no longer always active.
- Above midpoint, the old character is preserved in spirit but not sample-for-sample.

The safest migration posture is to treat this as a bug fix to the default-level contract rather than a backward-compatible no-op.

## Feasibility

This is a good path and low-risk if implemented with shared pure helpers. The code has only a few signal-path touchpoints, and the test model already mirrors the runtime path, so drift can be controlled.

The main risk is not implementation complexity; it is retuning expectations after removing hidden compression. That risk is manageable with focused tests and a quick listening pass across:

- `Low + Low`
- `Low + Band`
- `Notch + Low`
- `High + Notch`
- `High + High`
- high resonance settings
- `LEVEL = 0.5`, `0.75`, and `1.0`

## Implementation Checklist

1. Add level helper declarations to `src/Bifurx.hpp`.
2. Implement helpers in `src/Bifurx.cpp`.
3. Replace runtime input softclip in `Bifurx::process()`.
4. Replace runtime output softclip in `Bifurx::process()`.
5. Update `simulatePreviewProbeImpulseResponse()` to match runtime staging.
6. Update `tests/bifurx_filter_test_model.hpp`.
7. Add focused level/softclip regression tests.
8. Run:

```sh
make test-build-fast
build/tests/bifurx_filter_spec
make test-build-rack
LD_LIBRARY_PATH=../Rack-SDK build/tests/bifurx_runtime_spec
```

## Recommended First Patch

Do this as one coherent patch, not as separate runtime and preview changes. Runtime, preview probe, and test model must move together or the response display and tests will describe a different module than the one users hear.

