# Bifurx DR-55

## Purpose

This review evaluates `DR_bifurx.md` against the current Bifurx implementation and recommends the most sensible improvement path. The short version: make the preview truthful and stop avoidable UI churn first, then improve audio-thread control-rate behavior once guardrail tests exist.

Bifurx is unreleased, so compatibility constraints are lighter than the released modules. Still, avoid changing panel IDs, port order, or serialized keys unless there is a clear migration reason.

## Current Shape

The Bifurx core is already structured around two TPT SVF stages, cached coefficients, double-buffered preview state, and double-buffered FFT frames. The major problems are not architectural collapse. They are specific accuracy and workload leaks:

- The preview model is not fully aligned with the runtime SVF, especially bandpass gain and high-resonance Q.
- The UI can keep redrawing and animating even when state has effectively converged.
- Any connected CV currently forces the expensive control path to recompute every sample.
- A few stale or debug-oriented paths add friction and should be cleaned up before deeper work.

## Highest-Value Changes

### 1. Fix Preview Bandpass And Q Mapping

This is the most sensible first improvement because it changes no audio behavior and directly improves user trust in the module.

Evidence:

- Runtime SVF bandpass comes from `TptSvf::processWithCoeffs()` as `out.bp = v1` in `src/Bifurx.cpp`.
- Preview uses `makeDisplayBiquad()` and the bandpass case sets `b0 = alpha`, `b2 = -alpha`, which is the unity-peak RBJ-style bandpass.
- `makePreviewModel()` clamps `qA` and `qB` to `18.f`, while runtime damping can reach lower values through `resoToDamping()` and `makeSvfCoeffs()`.

Recommended change:

- Replace the preview bandpass with an SVF-consistent response, not just a cosmetic gain tweak.
- The pragmatic first patch is to scale the current bandpass numerator by Q. This makes center-frequency peak behavior match the TPT SVF much more closely.
- Raise the preview Q clamp to match runtime damping limits, or better, publish damping and derive preview Q from the same clamped damping used by the audio core.
- Update `tests/bifurx_filter_test_model.hpp` in lockstep with `src/Bifurx.cpp`, because the tests mirror preview math.

Validation:

- Add a focused preview/runtime response test for Band + Band and Low + Band at moderate and high resonance.
- Existing `test-fast` should remain the baseline. In WSL, do not treat final plugin linking as authoritative.

### 2. Stop Continuous UI Redraw Work

This should be second because the implementation risk is low and the payoff is visible. It also does not alter audio.

Evidence:

- `src/BifurxGL.cpp` calls `OpenGlWidget::step()` inside `BifurxSpectrumGLWidget::step()`. That path is not the same as a dirty-gated framebuffer step.
- Both renderers call `updateAnimation()` and then keep dirtying while `state.hasCurveTarget` or `state.hasOverlayTarget` is true.
- `updateAnimation()` slews values toward targets but never clears `hasCurveTarget` or `hasOverlayTarget` after convergence.
- NanoVG draw creates a local `std::vector<BifurxCurvePoint> refinedPoints` every draw. GL has a persistent vector, but it does not reserve capacity up front.

Recommended change:

- In GL, dirty-gate the framebuffer path instead of forcing redraw every UI frame.
- In `BifurxSpectrumBase::updateAnimation()`, snap values when max error is under a small epsilon and clear `hasCurveTarget` / `hasOverlayTarget`.
- Skip `syncBase()` / FFT consumption for renderer widgets that are not visible or whose render mode is inactive.
- Add persistent `refinedPoints` storage for the NanoVG widget and reserve known capacities for GL vectors.

Validation:

- Use existing perf debug CSV fields: `uiStepAvgNs`, `uiDrawCount`, `uiDrawAvgNs`, `uiOverlayUpdateAvgNs`.
- Confirm idle Bifurx does not continue drawing at display frame rate after preview and overlay settle.

### 3. Clean Up API Drift Before It Becomes A Real Bug

This is small but should be done early because it lowers confusion for every later Bifurx edit.

Evidence:

- `src/Bifurx.hpp` declares `processCharacterStage(TptSvf&, int stageIndex, ...)`.
- `src/Bifurx.cpp` defines `processCharacterStage(TptSvf&, int characterMode, int stageIndex, ...)`.
- Current internal calls use the CPP definition, but the header advertises a stale overload that has no definition.

Recommended change:

- Fix the header declaration to include `int characterMode`.
- If character modes remain intentionally collapsed to SVF, rename local variables/comments to make that explicit.
- Remove or quarantine legacy alternate-circuit references from tests/docs if those modes are no longer part of Bifurx.

Validation:

- Compile `build/tests/bifurx_runtime_spec`.
- Run `make test-fast`.

### 4. Cache Or Gate Low-Value Audio-Thread Work

This is useful, but it should follow the preview/UI fixes because the audio path is more sensitive.

Evidence:

- `sanitizeCoreState(coreA)` and `sanitizeCoreState(coreB)` run every sample.
- `onePoleAlpha(args.sampleTime, kLlTelemetryTauSeconds)` is recomputed every sample for LL telemetry.
- `pushAnalysisSample()` always maintains FFT history even when no visible consumer needs spectrum data.

Recommended change:

- Add a `sanitizeDivider` and run state sanitation every 16 or 32 samples, while keeping final output finite-sanitized.
- Cache the LL telemetry alpha on sample-rate change, as already done for preview and V/oct smoothing.
- Consider an audio-thread `analysisConsumerActive` atomic set by UI visibility/render mode. Keep default active if uncertain, but allow hidden modules to skip FFT history publishing.

Validation:

- Add a focused stress test for max resonance, TITO both polarities, and large input. The guardrail is finite output, not identical waveform.
- Compare `perfAudioProcessAvgNs`, `perfAudioControlsAvgNs`, and `perfAudioAnalysisAvgNs` before/after with perf logging enabled.

## Defer Until Tests Exist

### Multi-Tier Control Updates

`DR_bifurx.md` is right that the current path is expensive:

```cpp
const bool fastPathEligible = titoNeutral
	&& !voctConnected
	&& !inputs[FM_INPUT].isConnected()
	&& !inputs[RESO_CV_INPUT].isConnected()
	&& !inputs[BALANCE_CV_INPUT].isConnected()
	&& !inputs[SPAN_CV_INPUT].isConnected();
const bool updateFastControls = !controlFastCacheValid || !fastPathEligible || controlUpdateDivider.process();
```

That means any connected CV bypasses the divider and recomputes coefficients every sample.

However, the proposed tiered update system changes modulation behavior. It should not be the first patch unless Bifurx CPU is currently blocking use. The safe path is:

- First add instrumentation/tests that distinguish static, slow CV, V/oct, FM, and TITO cases.
- Keep FM and TITO audio-rate.
- Treat V/oct carefully. Pitch CV is often musically audio-rate even if many patches use it slowly.
- Only downsample resonance, balance, span, and maybe non-FM pitch after adding smoothing/aliasing guardrails.

Recommended design:

- `staticControlsDirty`: knob/sample-rate/mode changes only.
- `slowCvDivider`: resonance, balance, span CV at 64-256 sample intervals.
- `pitchDivider`: optional and conservative; bypass when FM input is connected or V/oct delta exceeds a per-sample threshold.
- `audioRateMod`: FM input connected with nonzero amount, or TITO non-neutral.

Do not call this a pure optimization. It is a behavior change for CV-rate modulation.

### VBO Rework

The GL widget creates a VBO but still draws from client-side vectors. That is untidy, but it is not the first performance target. Dirty-gating redraws should come before VBO work. If draw frequency drops correctly, the VBO change may not matter.

## Recommended Implementation Order

1. Header/API cleanup for `processCharacterStage`.
2. Preview bandpass/Q fix plus mirrored test-model update.
3. Add preview/runtime response tests for bandpass-heavy modes.
4. UI animation convergence and dirty-gating.
5. NanoVG/GL vector reserve and persistent NanoVG refined points.
6. Cache LL telemetry alpha and throttle state sanitation.
7. Only then implement tiered control-rate updates behind tests.

## Concrete Test Plan

Use WSL for source-level and fast tests only:

```sh
make test-fast
build/tests/bifurx_runtime_spec
```

Add or extend tests in these areas:

- `tests/bifurx_filter_test_model.hpp`: mirror preview changes exactly.
- `tests/bifurx_filter_spec.cpp`: assert expected bandpass peak behavior is not unity-clamped at high Q.
- `tests/bifurx_runtime_spec.cpp`: compare preview signatures against runtime measured gain for modes 1, 5, and 8.
- Add a finite-output stress case for max resonance, high level, and TITO.

## DR-55 Decision

The best improvement path is not a large rewrite. The sensible first tranche is preview accuracy plus UI idle behavior. Those are low-risk, user-visible, and testable. Audio-thread control-rate restructuring is worth doing, but only after guardrails exist, because it can change how Bifurx responds to fast modulation.

If only one change is made, fix the preview bandpass/Q mismatch. If two changes are made, also stop the renderer from redrawing after animation convergence.
