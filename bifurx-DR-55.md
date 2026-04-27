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

### GL Backend Cleanup

The GL renderer is no longer a single path. It now has:

- a fixed-function fallback path using client-side vertex arrays
- a shader-backed path using `shaderVbo` uploads and `drawVertsShader()`

That changes the implementation risk profile. The main concern is no longer "add a VBO". The concern is "do not destabilize two rendering backends that already produce working output."

Current shape:

- `src/BifurxGL.cpp` builds the same CPU-side geometry for both backends:
  - `fillVertices`
  - `fillSoftCapVertices`
  - `cyanVertices`
  - `curveVertices`
  - shared `refinedPoints`
- backend selection happens late in `drawFramebuffer()`:
  - shader path when `useGlShaderRenderer` is enabled and `ensureShaderReady()` succeeds
  - fixed-function fallback otherwise
- the widget also exposes backend state visually through the NanoVG badge:
  - `GL SHDR`
  - `GL FIXED`
  - `GL FALLBACK`

Implications:

- Dirty-gating and animation convergence still matter more than GPU upload changes, because both backends pay the same CPU-side preparation cost before issuing draw calls.
- Any GL cleanup should preserve identical geometry generation and only narrow backend-specific differences.
- Do not combine "render behavior changes" and "render backend refactors" in the same patch unless a regression harness exists.

Recommended constraint:

- Treat the shader renderer as an optional acceleration path, not the new semantic source of truth.
- Keep the fixed-function fallback working until the shader path has equivalent visual behavior across the expected environments.
- When making GL edits, prefer changes that are backend-agnostic:
  - redraw gating
  - convergence stopping
  - persistent vector storage
  - explicit reserve policy

Validation:

- Check both `useGlShaderRenderer = false` and `useGlShaderRenderer = true`.
- Confirm the same patch still renders sensible output when shader compilation fails and the widget falls back automatically.
- Avoid claiming a perf win from the shader path alone unless draw frequency is held constant during measurement.

## Recommended Implementation Order

1. Header/API cleanup for `processCharacterStage`.
2. Preview bandpass/Q fix plus mirrored test-model update.
3. Add preview/runtime response tests for bandpass-heavy modes.
4. UI animation convergence and dirty-gating.
5. NanoVG/GL vector reserve and persistent refined-point storage, without changing GL backend semantics.
6. Cache LL telemetry alpha and throttle state sanitation.
7. Only then implement tiered control-rate updates behind tests.

## Detailed Implementation Plans

### Phase 1: Header/API Cleanup

Goal: remove small source-of-truth drift before touching behavior.

Status:

- Completed, but the actual cleanup went further than the original narrow API-fix plan.
- Bifurx is now materially simpler and explicitly SVF-only in code, not just in comments.

Primary files:

- `src/Bifurx.hpp`
- `src/Bifurx.cpp`
- `bifurx-DR-55.md`
- `tests/bifurx_filter_test_model.hpp`
- `tests/bifurx_filter_spec.cpp`
- `tests/bifurx_runtime_spec.cpp`

Completed work:

1. Removed the stale `processCharacterStage()` declaration mismatch, then simplified the function to an SVF-only signature instead of preserving dead character-mode plumbing.
2. Removed no-op circuit abstraction hooks from runtime and preview code:
   - `clampCircuitMode()`
   - circuit scaling helpers
   - semantic-export helpers
   - mode sync compensation hook
3. Removed dead circuit-mode fields from preview and telemetry structs where they no longer carried useful information.
4. Simplified the test model and test suite so they no longer imply active DFM / Acid / MS2 / PRD / related alternate circuit paths.
5. Kept patch compatibility behavior where it still mattered:
   - legacy `filterCircuitMode` JSON is still read and intentionally ignored
   - panel/order/serialization behavior was otherwise left alone

Validation:

- Run `make test-fast`
- Optionally build `build/tests/bifurx_runtime_spec` when Rack SDK include paths are available

Validation status:

- `make test-fast` passed after the stronger cleanup.
- Standalone `bifurx_runtime_spec` build was not treated as authoritative in this environment because Rack SDK headers were unavailable.

Outcome:

- Phase 1 should be treated as complete.
- The original Phase 1 text below this point is now historical context, not a remaining to-do.

### Phase 2: Preview Accuracy Fix

Goal: make the preview curve match the runtime SVF closely enough that the display is trustworthy in bandpass-heavy modes and at high resonance.

Status:

- Completed for the current scope.
- Core math changes are landed and validated in both fast and runtime-oriented test suites.

Primary files:

- `src/Bifurx.cpp`
- `src/Bifurx.hpp`
- `tests/bifurx_filter_test_model.hpp`
- `tests/bifurx_filter_spec.cpp`
- `tests/bifurx_runtime_spec.cpp`

Completed work:

1. Added shared SVF damping bounds in the runtime and mirrored test model so preview and runtime use the same clamping contract.
2. Changed preview bandpass construction from unity-peak RBJ gain to Q-scaled numerator gain so the preview center response rises with Q like the runtime TPT SVF.
3. Updated preview Q handling so published preview Q can reflect the true runtime damping range instead of the old legacy clamp.
4. Mirrored the exact preview math changes in `tests/bifurx_filter_test_model.hpp`.
5. Added a direct fast-test regression proving the preview bandpass peak now rises well above `0 dB` at high Q.

Suggested test additions:

- In `tests/bifurx_filter_spec.cpp`, add a case that builds a high-Q preview model and checks that the bandpass peak rises materially above `0 dB`.
- In `tests/bifurx_runtime_spec.cpp`, use `capturePreviewState()` plus `measureRuntimeGainDb()` to compare preview and runtime around each marker frequency for modes `1`, `5`, and `8`.
- Reuse the existing helpers instead of introducing a second measurement harness.

Additional current notes:

- `tests/bifurx_runtime_spec.cpp` now includes:
  - a high-Q preview publish check beyond the old legacy preview clamp
  - a band-heavy marker-gain tracking check for modes `1`, `5`, and `8`
- Runtime-oriented checks have been exercised in this environment through `make test`, including `bifurx_runtime_spec`.
- `make test-fast` is green after the Phase 2 math change.

Risks:

- A cosmetic Q fix without fixing bandpass transfer scaling will still leave the preview lying near resonance.
- If preview math and test-model math diverge, the tests will start validating the wrong model instead of the code the user sees.

Remaining work:

1. Optional future tuning only: decide whether preview/runtime deltas in modes `1`, `5`, and `8` warrant a more exact SVF-derived preview transfer function.
2. Keep current tests as regression guards while proceeding to later phases.

Exit criteria:

- Preview and runtime agree within a narrow tolerance in the targeted bandpass modes.
- High-resonance bandpass peaks are visibly and numerically higher than the current unity-peak preview.

### Phase 3: UI Idle And Redraw Gating

Goal: stop the display from continuing to animate and redraw after values have effectively converged.

Current status: complete for the planned scope. Core convergence fix and inactive-renderer tick guard are both landed.

Completed in code:

- `BifurxSpectrumBase::updateAnimation()` now tracks residuals for curve, overlay, and top readout.
- On convergence, curve state snaps to `curveTargetDb[]` and clears `hasCurveTarget`.
- On convergence, overlay arrays plus `displayTopDbfs` snap to targets and clear `hasOverlayTarget`.
- `animationActive` now naturally falls false after convergence, allowing existing dirty gating to stop redraw.
- Inactive renderer widgets now early-return before `runRenderTick()`, so only the active backend consumes preview/analysis work each frame.
- Validation runs: `make test-fast` and `make test` passed after this change.

Remaining for Phase 3:

- No blocking work remains in this phase for the current plan.

Primary files:

- `src/Bifurx.cpp`
- `src/Bifurx.hpp`
- `src/BifurxUI.cpp`
- `src/BifurxGL.cpp`

Execution guardrails:

- Prefer a first patch that edits only `BifurxSpectrumBase::updateAnimation()` and, if needed, the minimal redraw gating that consumes its result.
- Do not change DSP, preview transfer math, serialization, shader compilation, shader/fallback renderer selection, or draw geometry generation in this phase.
- Do not mix Phase 3 with Phase 4 allocation cleanup. If a patch touches `refinedPoints`, `reserve()`, VBO sizing, or vector lifetime, it is too broad.
- Do not refactor `syncBase()` aggressively in the first pass. Prove convergence and redraw-stop first, then consider any inactive-renderer guard as a second patch.

Exact intended behavior:

- Curve animation:
  - keep the existing smoothing while any point is materially different from its target
  - after smoothing all points, compute the maximum residual `abs(curveDb[i] - curveTargetDb[i])`
  - if the maximum residual is at or below `kCurveEpsilonDb`, snap every `curveDb[i]` directly to `curveTargetDb[i]`
  - only after the full snap, clear `state.hasCurveTarget = false`
- Overlay animation:
  - keep the existing smoothing for `overlayModuleDb`, `overlayOutputDbfs`, and `displayTopDbfs` while any residual is still above threshold
  - after smoothing, compute the maximum residual across both overlay arrays plus the top readout residual `abs(displayTopDbfs - displayTopTargetDbfs)`
  - if all overlay residuals are at or below `kOverlayEpsilonDb` and the top residual is at or below `kTopEpsilonDbfs`, snap:
    - `overlayModuleDb[i] = overlayTargetModuleDb[i]`
    - `overlayOutputDbfs[i] = overlayTargetOutputDbfs[i]`
    - `displayTopDbfs = displayTopTargetDbfs`
  - only after the full snap, clear `state.hasOverlayTarget = false`
- Redraw policy:
  - `animationActive` must become `false` after both curve and overlay state have converged and snapped
  - new preview publishes must re-arm curve animation through `updateCurveCache()`
  - new analysis publishes must re-arm overlay animation through `updateOverlayCache()`
  - do not add any always-dirty fallback in either renderer to mask a bad convergence fix

Important semantic note:

- `hasCurveTarget` and `hasOverlayTarget` are currently overloaded. They mean both:
  - the display arrays have been initialized at least once
  - the animation path has an active target to converge toward
- That means clearing them is only safe after a full snap-to-target, not merely because the remaining residual is small.
- The follow-up publish path must remain:
  - `updateCurveCache()` sees `hasCurveTarget == false`, copies new targets into the live arrays if needed, and marks the new target active
  - `updateOverlayCache()` does the same for overlay arrays and top readout

Recommended edit order:

1. Patch `BifurxSpectrumBase::updateAnimation()` only.
2. Verify that it snaps and clears target flags after convergence instead of slewing forever inside epsilon.
3. Re-run the existing dirty-gating path without changing renderer selection logic.
4. Only if redraws are still being consumed unnecessarily, add a second narrow patch for hidden/inactive renderer guards.

Implementation steps:

1. Finish the animation state machine in `BifurxSpectrumBase::updateAnimation()`:
   - keep the current smoothing constants and easing behavior while active
   - add residual tracking for curve, overlay, and top readout
   - when residuals are all within epsilon, snap to the exact targets and clear the corresponding target-active flag
2. Verify initialization semantics immediately after the convergence patch:
   - `updateCurveCache()` and `updateOverlayCache()` currently rely on `hasCurveTarget` / `hasOverlayTarget`
   - after introducing clearing-on-settle, verify those methods still repopulate immediately when a new preview or analysis frame arrives
3. Keep dirtying only for real work:
   - NanoVG path should continue using framebuffer dirtiness based on `previewUpdated`, `analysisUpdated`, and `animationActive`
   - GL path should continue using the same shared convergence result rather than inventing a backend-specific redraw rule
   - apply the same redraw-stop rules regardless of whether the GL widget ends up in shader or fixed-function mode
4. Treat inactive-renderer guards as optional second-pass work:
   - avoid `syncBase()` / FFT overlay work for widgets that are not actually the active renderer only if this can be done without changing backend semantics
   - if that guard is not obviously safe in one pass, defer it and keep Phase 3 focused on convergence
5. Preserve current visual smoothing while active; the fix is to stop once settled, not to make the UI jumpy.

Out of scope for this phase:

- any shader-path rewrite
- any fixed-function fallback rewrite
- any VBO upload optimization
- any persistent-vector or `reserve()` cleanup
- any preview math or filter-model changes
- any patch-format or JSON behavior changes

Suggested instrumentation pass:

- Use `perf_debug_*.csv` and compare `uiDrawCount`, `uiDrawAvgNs`, `uiCurveUpdateAvgNs`, and `uiOverlayUpdateAvgNs` before and after.
- Specifically confirm that an idle module with no parameter movement stops drawing continuously after the curve and overlay settle.
- Repeat the idle check in both GL modes:
  - shader renderer enabled and compiling successfully
  - shader renderer disabled or falling back to fixed-function

Risks:

- Clearing `hasCurveTarget` too early can cause one-frame pops if new targets arrive during the same UI frame.
- Clearing `hasOverlayTarget` without snapping all arrays can leave perpetual one-LSB animation churn.
- A redraw-policy fix that is only tested in one GL backend can silently break the other backend's perceived stability.
- A patch that adds hidden-renderer guards before proving base convergence can make the redraw problem harder to diagnose, because the wrong widget may simply stop updating instead of truly settling.

Model-safe acceptance checklist:

- `updateAnimation()` returns `true` while values are still materially moving and `false` after snap-to-target convergence.
- `state.hasCurveTarget` becomes `false` only after the full curve has been snapped to `curveTargetDb`.
- `state.hasOverlayTarget` becomes `false` only after both overlay arrays and `displayTopDbfs` have been snapped to their targets.
- A fresh preview publish makes curve animation active again on the next render tick.
- A fresh analysis publish makes overlay animation active again on the next render tick.
- No code in this phase changes renderer selection or rendering backend semantics.

Exit criteria:

- Idle Bifurx no longer redraws at frame rate after state convergence.
- New preview or analysis publishes still restart animation cleanly.
- Shader and fixed-function GL output both remain visually sane after the redraw changes.
- The implementation is narrow enough that a lower-capability model can be instructed to touch `updateAnimation()` first without needing to reinterpret the architectural intent.

### Phase 4: Small UI Allocation Cleanup

Goal: remove predictable per-draw allocation churn once redraw frequency is under control, without claiming or requiring "zero dynamic allocation ever."

Primary files:

- `src/BifurxUI.cpp`
- `src/BifurxGL.cpp`

Status intent:

- This phase is a container-lifetime and reserve-policy pass.
- It is not a geometry algorithm rewrite and not a renderer behavior change.
- Current status: implemented for the scoped goals (persistent NanoVG `refinedPoints`, one-time reserve policy, and reuse-oriented shared helper behavior).

Required behavior contract:

- Persistent container lifetime:
  - NanoVG refined curve storage must be a long-lived widget member, not a draw-local vector.
  - GL scratch vectors stay long-lived members (already true) and are reused frame-to-frame.
- Reserve-once policy:
  - Reserve expected capacities during construction or first-use init.
  - `reserve()` must not run every draw call.
- Grow-allowed policy:
  - Vectors are allowed to grow if future geometry exceeds reserved capacity.
  - Growth is treated as rare fallback behavior, not steady-state behavior.
- No-shrink policy:
  - Do not call `shrink_to_fit()` in draw paths.
  - Do not add per-frame clear+reallocate patterns that defeat reuse.
- Semantic freeze:
  - Same geometry points, same draw order, same shader/fallback selection semantics.
  - This phase must not alter visual output intent.

Concrete implementation requirements:

1. NanoVG path:
   - move `refinedPoints` from draw-local storage to a persistent member on `BifurxSpectrumWidget`.
   - ensure helper calls reuse that same vector each frame.
2. Reserve policy:
   - NanoVG `refinedPoints`: reserve a known bound (`kCurvePointCount + refinement margin`) once.
   - GL `refinedPoints`: reserve once.
   - GL fill/curve/cyan vertex vectors: reserve once from known worst-case geometry counts.
3. Keep helper logic stable:
   - do not change refinement math, sort/unique behavior, marker anchor behavior, or response sampling in this phase.
4. Keep renderer backend behavior stable:
   - do not change shader compile/init flow
   - do not remove fixed-function fallback
   - do not change backend selection logic in this phase

Out of scope:

- "No dynamic memory allocation ever" guarantees
- VBO upload strategy changes
- shader feature changes
- preview/runtime filter math changes
- redraw policy changes already covered by Phase 3

Validation:

- Build and run existing fast tests.
- Exercise both GL backends after any `src/BifurxGL.cpp` edit.
- Spot-check perf logging to confirm reduced draw-time churn, but treat this as secondary to Phase 3.
- Verify no visible rendering regressions (markers, curve continuity, overlays, top readout) in both GL shader and fixed-function modes.

Validation status:

- `make test-fast` passed after Phase 4 implementation.
- `make test` passed in this environment.
- Manual in-Rack visual/perf spot-check remains recommended as a follow-up verification step.

Exit criteria:

- No per-draw `std::vector` construction remains on the NanoVG curve-refinement path.
- Reserve policy exists and is one-time/initialization-oriented, not per-frame.
- Steady-state draw path reuses existing vector capacities in both NanoVG and GL paths.
- Occasional growth remains allowed when needed; shrinking is not introduced in draw paths.
- Shader and fallback rendering behavior are unchanged apart from reduced allocation churn.

### Phase 5: Audio-Thread Guardrails And Cheap Caches

Goal: trim obvious always-on work without changing modulation behavior.

Primary files:

- `src/Bifurx.cpp`
- `src/Bifurx.hpp`
- `tests/bifurx_runtime_spec.cpp`

Implementation steps:

1. Cache LL telemetry alpha on sample-rate change:
   - today `onePoleAlpha(args.sampleTime, kLlTelemetryTauSeconds)` runs every sample
   - mirror the existing cached pattern used for preview filter alpha and V/oct smoothing
2. Add a sanitation divider for core state:
   - run `sanitizeCoreState(coreA/coreB)` every `16` or `32` samples instead of every sample
   - keep final output finite-sanitized every sample
3. Only make analysis publishing conditional if the UI-side consumption contract is understood:
   - current `pushAnalysisSample(in, out)` is unconditional
   - a future `analysisConsumerActive` atomic is reasonable, but it should default safe and must not race hidden-but-soon-visible widgets into stale overlays
4. Add runtime stress tests before landing sanitation throttling:
   - max resonance
   - large input level
   - both TITO polarities
   - finite output is the invariant, not waveform identity

Validation:

- `make test-fast`
- `build/tests/bifurx_runtime_spec`
- Perf comparison with `perfAudioProcessAvgNs`, `perfAudioControlsAvgNs`, and `perfAudioAnalysisAvgNs`

Exit criteria:

- No NaN/Inf regressions under the stress cases.
- Audio-thread savings are measurable in perf logging.

### Phase 6: Tiered Control-Rate Updates

Goal: reduce worst-case control-path cost when CV is connected, while explicitly treating this as a modulation-behavior change.

Primary files:

- `src/Bifurx.cpp`
- `src/Bifurx.hpp`
- `tests/bifurx_runtime_spec.cpp`

Implementation steps:

1. Split the current monolithic `updateFastControls` decision into named buckets:
   - static control recalculation
   - slow CV recalculation
   - conservative pitch recalculation
   - mandatory audio-rate recalculation
2. Keep these paths audio-rate:
   - FM when connected with nontrivial amount
   - TITO whenever non-neutral
3. Start with only slow CV throttling:
   - resonance CV
   - balance CV
   - span CV
4. Treat V/oct separately:
   - default conservative behavior is to keep it effectively fast unless tests prove a slower path is acceptable
   - if a pitch divider is introduced, bypass it whenever delta-per-sample exceeds a threshold or FM is connected
5. Preserve smoothing semantics:
   - any downsampled control path should be paired with explicit smoothing so stepped control updates do not alias visibly or audibly
6. Land this phase only behind new tests and before/after perf evidence.

Suggested runtime coverage:

- Static no-CV patch remains unchanged.
- Slow resonance CV and slow span CV stay finite and qualitatively smooth.
- Audio-rate FM remains measurably different from a downsampled approximation.
- TITO still responds sample-accurately and does not collapse onto the cached-coefficient path.

Exit criteria:

- CPU drops materially for CV-connected cases.
- Fast modulation behavior that should stay audio-rate is preserved by tests.

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
