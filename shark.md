Befaco frames Rampage as a Serge/Buchla-style patch-programmable ramp/function generator built around voltage-controlled integrators, with continuous variable shape between log, linear, and exponential behavior, so I’d have Codex implement this as a **Rampage-inspired “Shark Fin” curve mode**, not a full Rampage behavior clone. ([Befaco][1])

# Codex Implementation Spec: Integral Flux CH1/CH4 Function Shape Mode Toggle

## Goal

Add a new per-channel toggle switch to `IntegralFlux` allowing outer channels 1 and 4 to switch between:

1. **Maths mode** — the existing Integral Flux / Maths-style function-generator curvature. This must remain the default and should preserve current behavior.
2. **Shark Fin mode** — a Befaco Rampage-inspired alternate curve family that keeps the same timing, CV, trigger, cycle, slew, output, gate, mixer, preview, and performance behavior, but changes the shape response of the function generator / slew core.

This should be implemented as a small, clean extension to the existing unified CH1/CH4 outer-channel DSP path, not as a duplicated DSP engine.

## Current Architecture Notes

`IntegralFlux.cpp` already has a shared outer-channel implementation for CH1 and CH4:

* `OuterChannelState ch1`
* `OuterChannelState ch4`
* `OuterChannelConfig`
* `processOuterChannel(...)`

Current curve behavior is concentrated around:

* `shapeSignedFromKnob(float shape01)`
* `slopeWarp(float x, float s)`
* `slopeWarpScale(float s)`
* `segmentPhaseFromOutputNorm(...)`
* `processUnifiedShapedSlew(...)`
* `WavePreviewWidget::buildSegmentLut(...)`

The new mode should hook into this existing curve layer.

## User-Facing Behavior

Each outer channel gets one toggle switch:

* CH1: toggles CH1 between Maths and Shark Fin shape modes.
* CH4: toggles CH4 between Maths and Shark Fin shape modes.

Default state must be **Maths** so old patches sound and behave the same.

Suggested UI labels:

* `Maths`
* `Shark Fin`

Suggested param names:

* `CH1 function shape mode`
* `CH4 function shape mode`

Avoid user-facing labels that imply a literal clone of Befaco Rampage. Prefer `Shark Fin` or `Rampage-style Shark Fin`.

## Param ID Changes

Append new params at the end of `ParamId`, immediately before `PARAMS_LEN`, so existing param IDs remain stable.

Do **not** insert them in the middle of the enum.

Example:

```cpp
enum ParamId {
	ATTENUATE_1_PARAM,
	CYCLE_1_PARAM,
	CYCLE_4_PARAM,
	RISE_1_PARAM,
	RISE_4_PARAM,
	ATTENUATE_2_PARAM,
	FALL_1_PARAM,
	FALL_4_PARAM,
	ATTENUATE_3_PARAM,
	LIN_LOG_1_PARAM,
	LIN_LOG_4_PARAM,
	ATTENUATE_4_PARAM,

	SHAPE_MODE_1_PARAM,
	SHAPE_MODE_4_PARAM,

	PARAMS_LEN
};
```

This preserves existing patch compatibility because all existing param indices stay unchanged.

## Mode Enum

Add a small enum inside `IntegralFlux`:

```cpp
enum FunctionShapeMode {
	FUNCTION_SHAPE_MATHS = 0,
	FUNCTION_SHAPE_SHARK_FIN = 1
};
```

Add a helper:

```cpp
static FunctionShapeMode functionShapeModeFromParam(float value) {
	return value >= 0.5f ? FUNCTION_SHAPE_SHARK_FIN : FUNCTION_SHAPE_MATHS;
}
```

## Config

In the constructor, add:

```cpp
configSwitch(
	SHAPE_MODE_1_PARAM,
	0.f,
	1.f,
	0.f,
	"CH1 function shape mode",
	{"Maths", "Shark Fin"}
);

configSwitch(
	SHAPE_MODE_4_PARAM,
	0.f,
	1.f,
	0.f,
	"CH4 function shape mode",
	{"Maths", "Shark Fin"}
);
```

Default must remain `0.f`.

## OuterChannelConfig

Extend `OuterChannelConfig` with a mode param:

```cpp
int shapeModeParam;
```

Update CH1 config:

```cpp
static const OuterChannelConfig ch1Cfg {
	CYCLE_1_PARAM,
	INPUT_1_TRIG_INPUT,
	INPUT_1_INPUT,
	RISE_1_PARAM,
	FALL_1_PARAM,
	LIN_LOG_1_PARAM,
	SHAPE_MODE_1_PARAM,
	CH1_RISE_CV_INPUT,
	CH1_FALL_CV_INPUT,
	CH1_BOTH_CV_INPUT,
	CH1_CYCLE_CV_INPUT,
	std::log2(OUTER_LOG_SHAPE_SCALE),
	std::log2(OUTER_EXP_SHAPE_SCALE),
	OUTER_FALL
};
```

Update CH4 config similarly with `SHAPE_MODE_4_PARAM`.

Keep ordering readable and update the struct field order consistently.

## DSP Shape Strategy

Do not fork `processOuterChannel()`.

Instead, introduce mode-aware shape helpers.

### Maths Mode

Maths mode must use the current behavior:

```cpp
slopeWarp(x, shapeSigned)
slopeWarpScale(shapeSigned)
```

This path should be bit-identical or extremely close to the current behavior when the new mode param is `Maths`.

### Shark Fin Mode

Implement Shark Fin as a second stage-aware curve family.

Core intent:

* Still use the same rise/fall times.
* Still use the same `shapeSignedFromKnob()`.
* Still keep the linear point near `LINEAR_SHAPE`.
* Still normalize curve travel time.
* Change curvature so rise and fall are shaped as complementary fin-like ramps rather than the current Maths-style voltage-domain response.

Recommended implementation:

```cpp
static float stageLocalX(float outputNorm, bool rising) {
	return rising ? outputNorm : (1.f - outputNorm);
}

static float shapeSignedForMode(float shapeSigned, bool rising, FunctionShapeMode mode) {
	if (mode == FUNCTION_SHAPE_SHARK_FIN) {
		// Complementary per-stage curvature produces the fin-like sweep.
		return rising ? shapeSigned : -shapeSigned;
	}
	return shapeSigned;
}

static float slopeWarpForMode(float outputNorm, float shapeSigned, bool rising, FunctionShapeMode mode) {
	if (mode == FUNCTION_SHAPE_SHARK_FIN) {
		float local = stageLocalX(outputNorm, rising);
		float stageShape = shapeSignedForMode(shapeSigned, rising, mode);
		return slopeWarp(local, stageShape);
	}

	return slopeWarp(outputNorm, shapeSigned);
}

static float slopeWarpScaleForMode(float shapeSigned, bool rising, FunctionShapeMode mode) {
	float stageShape = shapeSignedForMode(shapeSigned, rising, mode);
	return slopeWarpScale(stageShape);
}
```

This gives the Shark Fin mode a distinct curve while keeping the same basic slope-warp infrastructure.

If this feels visually reversed in Rack testing, swap the polarity rule for Shark Fin mode:

```cpp
return rising ? -shapeSigned : shapeSigned;
```

Use the preview and scope behavior to choose the polarity that produces the most convincing fin-like shape.

## Cache Changes

The current `OuterChannelState` has one cached warp scale:

```cpp
bool warpScaleValid = false;
float cachedShapeSigned = 0.f;
float cachedWarpScale = 1.f;
```

Because Shark Fin mode may use different rise/fall shape polarity, replace this with per-stage cache fields:

```cpp
bool warpScaleValid = false;
float cachedShapeSigned = 0.f;
int cachedShapeMode = FUNCTION_SHAPE_MATHS;
float cachedRiseWarpScale = 1.f;
float cachedFallWarpScale = 1.f;
```

Update the cache refresh logic:

```cpp
FunctionShapeMode shapeMode = functionShapeModeFromParam(params[cfg.shapeModeParam].getValue());

if (!ch.warpScaleValid
	|| std::fabs(shapeSigned - ch.cachedShapeSigned) > 1e-4f
	|| int(shapeMode) != ch.cachedShapeMode) {
	ch.cachedShapeSigned = shapeSigned;
	ch.cachedShapeMode = int(shapeMode);
	ch.cachedRiseWarpScale = slopeWarpScaleForMode(shapeSigned, true, shapeMode);
	ch.cachedFallWarpScale = slopeWarpScaleForMode(shapeSigned, false, shapeMode);
	ch.warpScaleValid = true;
}
```

Use:

```cpp
float riseScale = ch.cachedRiseWarpScale;
float fallScale = ch.cachedFallWarpScale;
```

## Function Generator Path

In `processOuterChannel()`, read the mode once per sample:

```cpp
FunctionShapeMode shapeMode = functionShapeModeFromParam(params[cfg.shapeModeParam].getValue());
```

In the rise path, replace:

```cpp
x += dp * slopeWarp(x, s) * scale;
```

with:

```cpp
x += dp * slopeWarpForMode(x, s, true, shapeMode) * riseScale;
```

In the fall path, replace:

```cpp
x -= dp * slopeWarp(x, s) * scale;
```

with:

```cpp
x -= dp * slopeWarpForMode(x, s, false, shapeMode) * fallScale;
```

Keep all existing trigger, cycle, gate, injection, BLEP, and output behavior unchanged.

## Slew Path

Update `processUnifiedShapedSlew(...)` to accept mode and per-stage scales.

Current parameters include:

```cpp
float shapeSigned,
float warpScale,
float dt
```

Change to:

```cpp
float shapeSigned,
FunctionShapeMode shapeMode,
float riseWarpScale,
float fallWarpScale,
float dt
```

Inside slew:

```cpp
bool rising = delta > 0.f;
float stageTime = rising ? riseTime : fallTime;
float scale = rising ? riseWarpScale : fallWarpScale;
float x = computeSegPhase(out, ch.slewStartOut, ch.slewInvSpan);
```

For Maths mode, preserve current behavior.

For Shark Fin mode, use a stage-local normalized coordinate. Since `computeSegPhase()` returns progress from the current segment start to target, the Shark Fin slew path should use that as the local coordinate rather than absolute voltage.

Recommended helper:

```cpp
static float slewWarpForMode(float segmentPhase, float outputNorm, float shapeSigned, bool rising, FunctionShapeMode mode) {
	if (mode == FUNCTION_SHAPE_SHARK_FIN) {
		float stageShape = shapeSignedForMode(shapeSigned, rising, mode);
		return slopeWarp(segmentPhase, stageShape);
	}

	return slopeWarp(outputNorm, shapeSigned);
}
```

Then:

```cpp
float outputNorm = clamp((out - OUTER_V_MIN) / std::max(OUTER_V_MAX - OUTER_V_MIN, 1e-6f), 0.f, 1.f);
float localPhase = computeSegPhase(out, ch.slewStartOut, ch.slewInvSpan);
float step = dp * slewWarpForMode(localPhase, outputNorm, shapeSigned, rising, shapeMode) * scale * range;
```

This keeps the Shark Fin slew response stage-relative, while Maths mode remains voltage-domain and backwards-compatible.

## Shape Change Re-Anchoring

Current code reanchors phase when shape changes:

```cpp
if (shapeKnobChanged && ch.phase != OUTER_IDLE) {
	...
	ch.phasePos = segmentPhaseFromOutputNorm(x, shapeSigned, ch.phase == OUTER_RISE);
}
```

Update this condition to include mode changes:

```cpp
bool shapeModeChanged = int(shapeMode) != ch.cachedShapeMode;
bool curveChanged = shapeKnobChanged || shapeModeChanged;
```

However, be careful: if `cachedShapeMode` is updated before this check, store the previous value first.

Add a mode-aware version of `segmentPhaseFromOutputNorm(...)`:

```cpp
static float segmentPhaseFromOutputNormForMode(
	float outputNorm,
	float shapeSigned,
	bool rising,
	FunctionShapeMode mode
);
```

For Maths mode, preserve the existing implementation.

For Shark Fin mode, integrate the reciprocal of `slopeWarpForMode(...)` across the stage-local coordinate and normalize by `slopeWarpScaleForMode(...)`.

Acceptance requirement: toggling mode while a function is running must not hard-reset the output. The current output voltage should stay continuous and the phase should be remapped as closely as practical.

## Preview State

The waveform preview must reflect the selected mode.

Extend `PreviewSharedState`:

```cpp
std::atomic<int> shapeMode {FUNCTION_SHAPE_MATHS};
```

Extend `publishPreviewState(...)` to include mode:

```cpp
void publishPreviewState(
	PreviewSharedState& shared,
	float riseTime,
	float fallTime,
	float curveSigned,
	FunctionShapeMode shapeMode,
	bool interactiveRecent
)
```

Store:

```cpp
shared.shapeMode.store(int(shapeMode), std::memory_order_relaxed);
```

Extend `getPreviewState(...)` to return mode.

Extend `PreviewUpdateState`:

```cpp
int lastShapeMode = FUNCTION_SHAPE_MATHS;
```

Update `updatePreviewChannel(...)` so mode changes count as meaningful preview changes.

## WavePreviewWidget

Preview LUTs must be keyed by both curve and mode.

Add:

```cpp
int cachedLutShapeMode = IntegralFlux::FUNCTION_SHAPE_MATHS;
```

Update:

```cpp
void ensureSegmentLuts(float curveSigned, IntegralFlux::FunctionShapeMode shapeMode)
```

Cache invalidation should compare both `curveSigned` and `shapeMode`.

Update `buildSegmentLut(...)` to accept mode:

```cpp
static void buildSegmentLut(
	std::array<float, PREVIEW_LUT_SIZE>& lut,
	float curveSigned,
	bool rising,
	IntegralFlux::FunctionShapeMode shapeMode
)
```

Use `slopeWarpForMode(...)` and `slopeWarpScaleForMode(...)` so the preview matches audio behavior.

In `rebuildPoints(...)`, pass mode through.

Acceptance requirement: switching modes immediately changes the preview curve for the correct channel only.

## UI Placement

Add two toggle switch positions:

```cpp
Vec shapeMode1SwitchPos(...);
Vec shapeMode4SwitchPos(...);
```

Use SVG point overrides:

```cpp
applyPointOverride("SHAPE_MODE_1", &shapeMode1SwitchPos);
applyPointOverride("SHAPE_MODE_4", &shapeMode4SwitchPos);
```

Fallback placement should be near each channel’s curve/preview area without colliding with jacks, knobs, lights, or labels.

Suggested strategy:

* Prefer adding anchor IDs to `res/flux.panel.svg` / label SVG if panel edits are available.
* If not, add conservative fallback coordinates in `IntegralFlux.cpp`.
* Use an existing toggle switch component if the Leviathan plugin already has one.
* If no custom switch exists, use a standard Rack toggle such as `CKSS`.

Example:

```cpp
addParam(createParamCentered<CKSS>(mm2px(shapeMode1SwitchPos), module, IntegralFlux::SHAPE_MODE_1_PARAM));
addParam(createParamCentered<CKSS>(mm2px(shapeMode4SwitchPos), module, IntegralFlux::SHAPE_MODE_4_PARAM));
```

If the plugin has a Leviathan-themed two-position switch, use that instead.

## Serialization

No custom JSON serialization is needed for the mode params if they are normal Rack params.

Do not add these to `dataToJson()` unless there is a specific reason. Rack should save param values automatically with the patch.

Existing custom JSON keys must remain unchanged.

## Backward Compatibility

Required:

* Existing patches load.
* Existing patches default to Maths mode.
* Existing param IDs are not shifted.
* Existing behavior should be unchanged when both mode switches are in Maths mode.
* Existing JSON data continues to load safely.

## Performance Requirements

The new mode must not add meaningful audio-thread cost.

Avoid:

* Per-sample heap allocations.
* Per-sample expensive `powf`.
* Per-sample JSON/string operations.
* Duplicated CH1/CH4 code.

Allowed:

* A small number of additional branch checks per sample.
* Cached rise/fall warp scales.
* Preview LUT rebuilds when the mode changes.

## Validation Checklist

### Build

Run the normal Rack plugin build.

Required result:

* No compile errors.
* No warnings from enum conversion if possible.
* No missing widget class.

### Patch Compatibility

Test loading an old patch that contains Integral Flux.

Expected:

* Patch loads without missing param problems.
* CH1 and CH4 default to Maths mode.
* Existing behavior sounds unchanged.

### Maths Mode Regression

With both switches in Maths mode:

* CH1 and CH4 outputs should match previous behavior.
* Shape knob behavior should be unchanged.
* Cycle rate should be unchanged.
* EOR/EOC gates should be unchanged.
* SUM/OR/INV normalization should be unchanged.
* Preview should match previous visuals.

### Shark Fin Mode Behavior

With a cycling CH1 or CH4:

* Toggle to Shark Fin.
* Shape knob should visibly and audibly alter the contour differently from Maths mode.
* Rise/fall time controls should still work.
* Rise/Fall/Both CV should still work.
* Cycle CV should still work.
* Trigger should still work.
* Output should stay in the expected voltage range.
* No NaN/inf behavior at extreme CV or knob settings.

### Slew Behavior

Patch signal input into CH1 or CH4 while the channel is not actively cycling/triggered.

Expected:

* Maths mode preserves current slew shape.
* Shark Fin mode produces the alternate stage-relative shape.
* Rise/fall direction gates still behave correctly.
* Output remains continuous and bounded.

### Active Toggle Behavior

While channel is cycling slowly:

* Switch Maths → Shark Fin.
* Switch Shark Fin → Maths.

Expected:

* No hard reset to 0V.
* No stuck phase.
* No stuck gate.
* No click-sized output discontinuity except the expected curve transition.
* Preview updates promptly.

### CH1/CH4 Independence

Set CH1 to Maths and CH4 to Shark Fin.

Expected:

* CH1 preview/audio uses Maths.
* CH4 preview/audio uses Shark Fin.

Then swap.

Expected:

* Modes remain independent.

### Preview

Expected:

* Preview curve changes when mode switch changes.
* Preview dot remains aligned with the active curve.
* Tracer trails do not explode or draw invalid paths.
* OpenGL preview mode and NanoVG preview mode both work.

## Implementation Preference

Keep this implementation small and surgical.

Preferred order:

1. Add enum + params + config switches.
2. Extend `OuterChannelConfig`.
3. Add mode-aware curve helper functions.
4. Convert warp scale cache to per-stage cache.
5. Update function generator path.
6. Update slew path.
7. Update phase remapping.
8. Update preview state and preview LUTs.
9. Add UI switch widgets and SVG point overrides.
10. Build and test.

## Non-Goals

Do not implement the full Befaco Rampage module.

Do not change:

* Mixer normalization.
* Output ranges.
* Attenuverter behavior.
* Cycle latch behavior.
* Trigger acceptance rules.
* EOR/EOC semantics.
* Existing timing calibration.
* Existing debug terminal metrics except if mode display/debugging is useful.

## Success Definition

Integral Flux now has two per-channel shape modes on channels 1 and 4.

Maths mode preserves existing behavior.

Shark Fin mode gives a clearly distinct Rampage-inspired fin-shaped response while preserving the same controls, CV behavior, trigger/cycle behavior, slew functionality, preview behavior, and output semantics.

The implementation remains unified, maintainable, patch-compatible, and performant.

[1]: https://www.befaco.org/rampage-2/ "Rampage - Befaco"
