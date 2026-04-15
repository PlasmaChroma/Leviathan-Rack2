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
- An adaptive publish path is active during rapid pitch/span movement.
- Preview frequencies, Q values, and balance are filtered before publication.
- Instant-settle logic snaps the filtered preview to the current target once motion is effectively still.
- Instant-settle is not limited to CV motion anymore; it also applies to manual sweeps and hard stops.

### UI-side stabilization already implemented
- The response curve is still sampled on a fixed log-frequency grid.
- The white response curve uses additional UI smoothing plus a per-frame slew limit.
- The FFT/energy overlay is smoothed independently from the white response curve.
- The bottom frequency label strip and clipping boundary above it are already in place.

### Marker behavior as implemented now
- Marker X positions are tied to `previewState.freqA` and `previewState.freqB`.
- Bottom labels also reflect those same preview frequencies.
- Marker Y is currently derived from the displayed curve, not from a fresh analytic evaluation at the exact frequency.
- Markers are clamped to the visible plot region and hidden when the evaluated point would fall below the visible plot floor.

### Debug instrumentation already implemented
- `Log Curve Debug` in the context menu writes dedicated CSV traces under:
  - `asset::user()/Leviathan/Bifurx/curve_debug/`
- Logging is performed from widget `step()`, not `draw()`.
- The CSV already includes:
  - preview and analysis sequence/update flags
  - frequency/span/resonance/balance context
  - marker and curve Y values
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
The response curve is still built from a fixed-resolution log-frequency grid. That means very steep peaks or notches can still look slightly offset, flattened, or late during motion, even though the underlying preview model is analytic.

### 2. Marker Y is display-derived instead of analytically evaluated
The most important mismatch in the current implementation is that marker Y is taken from the already sampled and smoothed display curve. That means:
- marker position can inherit fixed-grid sampling artifacts
- marker position can inherit UI smoothing lag
- debug CSV `peak_*_y_curve` and `peak_*_y_marker` are currently reporting display behavior, not true analytic marker-vs-model agreement

### 3. Two smoothing layers can still compound lag
The current system combines:
- engine-side filtered preview publication
- widget-side curve smoothing and slew limiting

That combination may still be heavier than necessary now that instant-settle exists.

## Proposed Next Phase

### Goal
Keep the current engine-side stabilization path, then reduce the remaining visual artifacting with the smallest targeted UI changes that improve correctness first.

### Phase 1: Make marker Y analytically correct
- Keep marker semantics tied to `previewState.freqA` and `previewState.freqB`.
- Keep marker X tied to log-frequency position.
- Change marker Y evaluation to:
  - build the current preview model from `previewState`
  - evaluate `previewModelResponse()` at the exact marker frequency
  - convert that dB value directly to plot Y
  - clamp or hide only at the final visibility stage

This should be treated as the first remaining fix because it improves both:
- visual correctness of the markers
- usefulness of debug CSV traces

### Phase 2: Reduce or retune white-curve smoothing if Phase 1 is not enough
- Re-check whether the current widget-side smoothing factor and slew limit are still justified now that instant-settle is active.
- If the curve still trails too much after control motion stops, reduce widget-side smoothing before attempting a larger reconstruction change.
- Overlay smoothing should remain independent.

### Phase 3: Add targeted curve refinement only if fixed-grid artifacts still matter
Do not jump straight to a full recursive adaptive curve rebuild unless the post-Phase-2 behavior still shows obvious steep-region sampling artifacts.

Preferred direction:
- keep the existing fixed curve pipeline as the base path
- add targeted extra sampling only where the response is visually steep or highly curved
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
- If analytic marker Y plus lighter curve smoothing makes the display feel correct, stop there.
- If steep-feature artifacts remain obvious after that, add targeted curve refinement.
- Only escalate to a full adaptive polyline rebuild if the narrower fixes still fail.
