# Bifurx Preview Stabilization Proposal

## Summary
This document replaces the earlier "adaptive curve rebuild" proposal with a plan that matches the code as it exists now.

Bifurx no longer has a simple fixed-preview path. The current implementation already includes:
- engine-side preview-state filtering
- adaptive preview publication during rapid motion
- instant-settle behavior after motion stops
- UI-side curve smoothing and slew limiting
- dedicated curve debug CSV capture

The remaining issue is narrower than the original proposal assumed: the main concern is still visual stability of steep features, especially in `Notch + Notch`, during movement and immediately after hard stop turnarounds. The next work should be based on the current architecture rather than replacing it wholesale.

## Current Status
- Done: marker Y now evaluates the analytic preview model at the exact marker frequency instead of inheriting the displayed curve's sampled/smoothed Y position.
- Done: the white response curve no longer applies the previous extra `0.20` easing layer; it now tracks the target curve directly through the existing per-frame slew limit.
- Done: adaptive preview publication now reacts to rapid `Q` and filtered `balance` changes in addition to frequency/span movement.
- Done: the drawn white curve now gets targeted local refinement around `freqA` and `freqB` so steep notch/peak centers are less likely to visually miss the marker positions.
- Remaining: the white response curve still uses a fixed base log-frequency grid outside those locally refined regions, so broader steep-feature sampling artifacts can still appear under motion.
- Remaining: the current curve debug CSV still uses the older field names even though `peak_*_y_marker` now represents analytic marker positioning relative to the displayed curve.

## Current Baseline

### Preview state
- The display curve is still derived from the analytic preview model:
  - `makePreviewModel()`
  - `previewModelResponse()`
- The engine publishes a filtered `BifurxPreviewState` rather than exposing raw instantaneous DSP modulation state.
- Published preview values currently include:
  - `freqA`, `freqB`
  - `qA`, `qB`
  - filtered `balance`
  - mode and control-context fields used for debugging

### Engine-side stabilization already implemented
- Fast and slow preview publish dividers are active.
- An adaptive publish path is active during rapid frequency/span movement and now also reacts to rapid `Q` and filtered `balance` changes.
- Preview frequencies, Q values, and balance are filtered before publication.
- Instant-settle logic snaps the filtered preview to the current target once motion is effectively still.
- Instant-settle is not limited to CV motion anymore; it also applies to manual sweeps and hard stops.

### UI-side stabilization already implemented
- The response curve is still sampled on a fixed log-frequency grid.
- The drawn white curve now inserts local analytic refinement points around the two marker frequencies.
- The white response curve now follows its target through the per-frame slew limit without the previous extra easing layer.
- The FFT/energy overlay is smoothed independently from the white response curve.
- The bottom frequency label strip and clipping boundary above it are already in place.

### Marker behavior as implemented now
- Marker X positions are tied to `previewState.freqA` and `previewState.freqB`.
- Bottom labels also reflect those same preview frequencies.
- Marker Y is now derived from a fresh analytic evaluation of the preview model at the exact marker frequency.
- Markers are clamped to the visible plot region and hidden when the evaluated point would fall below the visible plot floor.

### Debug instrumentation already implemented
- `Log Curve Debug` in the context menu writes dedicated CSV traces under:
  - `asset::user()/Leviathan/Bifurx/curve_debug/`
- Logging is performed from widget `step()`, not `draw()`.
- The CSV already includes:
  - preview and analysis sequence/update flags
  - frequency/span/resonance/balance context
  - displayed-curve and marker Y values
  - UI frame duration

## What Has Already Been Solved
- The preview no longer relies only on a low-rate periodic publish path.
- Hard-stop settling is no longer expected to drift for a long time just because the control path was manual instead of CV-driven.
- The display now has enough instrumentation to compare:
  - publication timing
  - curve motion
  - marker-to-curve agreement
  - UI frame timing

That means the remaining work should be incremental and evidence-driven.

## Remaining Problems

### 1. Steep-feature rendering is still based on fixed grid sampling
The response curve still uses a fixed-resolution log-frequency base grid. Local refinement now helps around `freqA` and `freqB`, but very steep peaks or notches can still look slightly offset, flattened, or late away from those locally refined regions, even though the underlying preview model is analytic.

### 2. Debug schema still reflects the older marker behavior
This is now fixed for marker positioning. The remaining follow-up is mostly documentation/debug-schema cleanup:
- marker dots now reflect the analytic preview model at the exact marker frequency
- `peak_*_y_curve` versus `peak_*_y_marker` can now be interpreted as displayed-curve-versus-analytic-marker behavior
- if needed later, the CSV schema can be renamed to make that distinction explicit

### 3. White-curve motion may still need retuning
The current system combines:
- engine-side filtered preview publication
- widget-side slew limiting for the white curve

This is materially lighter than before, but further retuning may still be worthwhile if the slew limit alone feels too slow or too sharp in certain modes.

## Proposed Next Phase

### Goal
Keep the current engine-side stabilization path, then reduce the remaining visual artifacting with the smallest targeted UI changes that improve correctness first.

### Phase 1: Re-tune the remaining white-curve motion if needed
- Keep marker semantics tied to `previewState.freqA` and `previewState.freqB`.
- Keep marker X tied to log-frequency position.
- Keep marker Y analytically evaluated from the current preview model.
- Re-check whether the existing white-curve slew cap is still stronger than necessary now that the extra easing layer has been removed.

### Phase 2: If needed, further retune white-curve motion before larger reconstruction work
- If the curve still trails too much after control motion stops, loosen the slew cap before attempting a larger reconstruction change.
- Overlay smoothing should remain independent.

### Phase 3: Add targeted curve refinement only if fixed-grid artifacts still matter
Do not jump straight to a full recursive adaptive curve rebuild unless the post-Phase-2 behavior still shows obvious steep-region sampling artifacts.

Preferred direction:
- keep the existing fixed curve pipeline as the base path
- expand targeted extra sampling only where the response is visually steep or highly curved
- keep the refinement UI-only
- keep a hard point cap
- leave the FFT overlay path unchanged

This is a narrower and safer version of the earlier adaptive-curve proposal, and it fits the current codebase better.

## Explicit Non-Goals
- Do not move preview visualization onto the audio thread.
- Do not make marker semantics track local extrema or "visual peaks."
- Do not couple FFT overlay reconstruction to the white response curve changes in the next pass.
- Do not remove the current debug CSV tooling.

## Validation Plan

### Primary validation
- Repeat manual frequency sweeps with hard stops in all modes, with special attention to `Notch + Notch`.
- Confirm that after each hard stop:
  - the curve settles quickly
  - markers stop drifting immediately or near-immediately
  - the marker remains visually attached to the intended feature

### Debug-trace validation
- Capture new CSV traces before and after any further changes.
- Compare:
  - post-stop drift duration
  - `yJump > 8px`
  - high-percentile frame-to-frame Y movement
  - marker-vs-curve divergence

### Scope validation
- Confirm that any new work does not add audio-thread cost.
- Confirm that UI work remains bounded and does not create visible frame drops.

## Decision Rule
- If analytic marker Y plus the lighter white-curve motion makes the display feel correct, stop there.
- If steep-feature artifacts remain obvious after that, add targeted curve refinement.
- Only escalate to a full adaptive polyline rebuild if the narrower fixes still fail.
