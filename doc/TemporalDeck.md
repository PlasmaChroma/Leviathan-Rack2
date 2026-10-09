# TemporalDeck Canonical Design and Implementation Notes

Date: 2026-04-02
Status: Canonical working document for TemporalDeck.

## Purpose

TemporalDeck is a stereo, buffer-based performance deck for VCV Rack. It combines:
- live circular recording
- platter-style scratch interaction
- transport manipulation (freeze/reverse/slip)
- sample playback mode
- optional interpolation/coloration behavior for performance tone

This document is the primary technical reference. Older TemporalDeck markdowns should be treated as supporting archive notes unless explicitly updated.

## Current Runtime Architecture

### Module orchestration
`src/TemporalDeck.cpp` owns:
- Rack params/inputs/outputs/lights wiring
- JSON patch-state I/O
- per-frame orchestration of extracted components

### Extracted behavior units
- `src/TemporalDeckEngine.hpp`: DSP runtime, transport math, read/write behavior.
- `src/TemporalDeckTransportControl.hpp/.cpp`: freeze/reverse/slip transitions, gate-edge handling, seek revision application.
- `src/TemporalDeckSampleLifecycle.hpp/.cpp`: async sample lifecycle (decode/prep/install/fallback).
- `src/TemporalDeckPlatterInput.hpp/.cpp`: platter gesture aggregation and frame snapshot handoff.
- `src/TemporalDeckSamplePrep.hpp/.cpp`: sample preparation helpers.
- `src/TemporalDeckFrameInput.hpp/.cpp`: pure frame-input mapping abstraction (gate/pos/rate and related frame state).
- `src/TemporalDeckArcLights.hpp/.cpp`: light-bar compute/apply split.

## High-Level Audio/Transport Model

### Buffer model
- Stereo circular buffer with live write-head and controllable read-head.
- Read-head distance behind write-head is lag.
- Accessible lag depends on buffer setting and currently filled history.

### Write behavior
- Normal path writes live input each sample.
- Freeze disables writes.
- Edge conditions can constrain transport/scratch behavior.
- Live-edge behavior must keep filling while held at forward scratch edge (regression-covered).

### Transport states
- Freeze: hold playback motion, stop write.
- Reverse: run transport backward within allowed bounds.
- Slip: performance return toward NOW after release/manipulation.

### Scratch/platter behavior contract
- Physical baseline: 33.333 RPM equivalent mapping target.
- Authoritative mapping from gesture delta angle to lag delta.
- Freeze and live modes differ in forward compensation requirements.
- Rebase rules prevent losing accumulated gesture progress when engine smoothing lags UI updates.

## Sample Mode

Sample mode reuses the existing scratch buffer as playback source.

### First-pass intent
- Load sample from context menu.
- Decode and copy into deck buffer.
- Bounded playback region and seek behavior.
- Conservative memory policy and compatibility with existing engine flow.

### Constraints
- Treat loaded sample as primary source while in sample mode.
- Keep transport/seek behavior explicit and bounded to loaded content.
- Preserve live mode semantics when sample mode is not active.

## UI and CV Testability Boundaries

TemporalDeck now supports non-Rack test surfaces for key UI/CV-facing logic:
- frame input mapping is abstracted and unit-testable (`TemporalDeckFrameInput`).
- light-bar behavior is abstracted and unit-testable (`TemporalDeckArcLights`).
- platter interaction flows through testable state snapshots (`TemporalDeckPlatterInput`).

This allows gate/position/rate behaviors and light output semantics to be verified without Rack UI runtime.

## Test Ownership

- `tests/temporaldeck_engine_spec.cpp`: engine DSP and transport invariants.
- `tests/temporaldeck_platter_input_spec.cpp`: platter snapshot/hold semantics.
- `tests/temporaldeck_sample_prep_spec.cpp`: sample prep correctness.
- `tests/temporaldeck_frame_input_spec.cpp`: frame input mapping behavior.
- `tests/temporaldeck_arc_lights_spec.cpp`: arc light compute behavior.
- `tests/temporaldeck_virtual_integration_spec.cpp`: cross-component gesture/transport/sample regressions.

## File Management Policy

- Keep `TemporalDeck.cpp` orchestration-focused.
- Add new files only for full behavior/state-machine boundaries.
- Prefer extending tests over introducing thin utility files.
- Merge small, single-purpose helpers into existing boundaries unless they are reusable and ownership-clean.

## Known Future Work

### Slip return redesign
There is a draft servo-style slip redesign proposal that targets smoother, less synthetic rejoin behavior. Treat this as design exploration until explicitly adopted.

### SINC performance
Potential optimization path for high-quality scratch interpolation:
- Implemented: polyphase/precomputed kernel LUT (radius-8 windowed SINC,
  1024 phase table) in both live and sample-bounded SINC read paths.
- Implemented: regression tests that compare LUT SINC output to direct
  windowed-SINC reference math for wrapped/live and sample-bounded reads.
- Remaining ideas:
  - SIMD tap accumulation
  - optional quality guardrails under extreme motion

These should be pursued only with profile-backed validation and regression/perceptual checks.

## Consolidation Sources

This document consolidates the useful, non-Flux TemporalDeck content from:
- `doc/TemporalDeck - Functional Design Spec.md`
- `doc/TemporalDeck_Architecture.md`
- `doc/PlatterInteractionSpec.md`
- `doc/td_sample_mode.md`
- `doc/m_slip.md`
- `doc/SINC.md`
- `doc/plan.md` (file management/testing intent)

Docs that are mostly exploratory/reference (not canonical spec):
- `doc/TD_Deep_Research.md`
- `doc/Turntablist_Scratches_in_TemporalDeck.md`

## Scratch smoothing

The context-menu **Scratch smoothing** option defaults to On, including when
loading patches that predate the option. Its value is saved per module.

Smoothing follows internal scratch ownership and read-head motion: gesture entry,
release, hand/scope/wheel/CV handoffs, stops, restarts, reversals, and abrupt travel
changes can start a short 4 ms audio crossfade. It does not detect clicks from the
source waveform or change the requested playhead position. A stationary scratch
fades out after 8 ms of stillness over a further 8 ms, and resumes over 3 ms.

Turn it Off to bypass these transition crossfades and stationary-hold fades for
raw scratch articulation. Existing cartridge character and interpolation remain
independent. Source transients during steady motion are not detected or suppressed.

Arc clicks in the live buffer and RAM samples also use the optional transition
crossfade, including repeated clicks during an existing fade. Position changes
remain immediate. LongPlay retains a 5 ms residency-recovery fade, now using the same transition
owner. Stream recovery takes priority over scratch transition requests until it
finishes, including when optional scratch smoothing is Off.

The runtime continuity state, named scratch sources, and timing/motion thresholds
are owned by `TemporalDeckScratchSmoothing.hpp`. Source ownership handles release
as well as entry; the explicit CV gate trigger is only used when POS is connected.

Scratch DSP history is captured before the final hold/transition envelopes;
the last audible sample is retained separately for transition continuity.
Live Slip-to-NOW blending uses its actual selected duration, constant-sum gains
for the converging heads, and eases motion coloration and the Slip low-pass mix
to neutral before completing. Instant returns use the optional transition fade.
