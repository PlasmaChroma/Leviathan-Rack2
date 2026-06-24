# Undertow Review

## 1. Executive Summary

Undertow is a compact analog-character oscillator with sine, threshold-fold morph, gated sub oscillator, exponential/linear FM, sync, and stepped coarse tuning. The implementation is coherent, sample-rate aware, bounded, and covered by shape tests. It is close to release, with mono-only architecture and shipped profiling as the main issues.

Release readiness: 8/10

## 2. Module Inventory

- Source/UI: `Undertow.cpp/.hpp`, `UndertowWidget.cpp`, `UndertowShape.hpp`; shared preview/debug/visual helpers. Panel: `res/undertow.svg` and shared icons.
- Params: Coarse, Fine, Linear FM amount, Morph, octave-step mode, edge hardness. Inputs: V/Oct, Expo FM, Linear FM, Morph CV, Sync, Sub gate. Outputs: Sine, Morph, Sub. Lights: Sync, Sub gate, stepped mode.
- Menu/state: coarse continuous/stepped, morph asymmetry/side, analog character, preview tracer/quality; JSON saves these and edge hardness. No expander.

## 3. DSP and Audio/CV Correctness

Phase uses `args.sampleTime`; frequency is bounded to 8 Hz-20 kHz; hard sync, wrap, and sub transitions receive MinBLEP correction (`Undertow.cpp:126-245`). Linear FM is AC-coupled. Shape output is clamped, but sine MinBLEP correction is not explicitly clamped. All inputs/outputs are mono and external NaN/Inf is not sanitized. Oscillator phase and sub state are not serialized, appropriately avoiding phase-dependent presets.

## 4. UI, Panel, and Interaction Review

The panel and live morph preview support the alien-instrument language and expose the unusual waveshape effectively. Context options are meaningful but the preview-performance controls should be treated as advanced. Tooltip names are clear.

## 5. Performance Review

- Severity: High — shipped debug mode times every sample (`Undertow.cpp:124-255`, `res/dragonking.txt`).
- Severity: Low — per-sample DSP uses approximations, cached state, fixed MinBLEP storage, and no heap allocation.
- Severity: Low — preview rendering should be stress-tested across many instances.

## 6. Stability and Rack Integration

JSON values are range-clamped where needed and the panel is plugin-relative. No `onReset()` or `onSampleRateChange()` is needed for the sample-time formulation, but a reset test should confirm phase/trigger state expectations. Mono behavior is explicit via `setChannels(1)`.

## 7. Code Quality and Maintainability

DSP is separated from widget code and the pure shaping helper is tested. Constants are named and comments explain nonlinear choices. A small reusable finite-voltage helper would close the main robustness gap.

## 8. Musical Usefulness

Undertow offers a focused bass/utility voice with useful 5 V outputs, a controllable sub gate, FM, sync, and a distinctive morph. Defaults are immediately playable. Polyphonic support would materially increase value but is not required if mono is documented.

## 9. Bugs and Risk Register

| ID | Severity | Area | Finding | Evidence | Suggested Fix |
| -- | -------- | ---- | ------- | -------- | ------------- |
| UNDERTOW-001 | High | Performance | Debug timing runs every audio sample in the package. | `Undertow.cpp:124-255`; `res/dragonking.txt` | Disable debug by default or sample the timer. |
| UNDERTOW-002 | Medium | DSP | All modulation is mono and non-finite voltages can enter exponentiation/state. | `Undertow.cpp:135-159,229-245` | Sanitize/clamp inputs; document mono or add voices. |
| UNDERTOW-003 | Low | DSP | Sine output lacks the explicit final rail clamp used by Morph/Sub. | `Undertow.cpp:240-245` | Bound final correction or test overshoot contract. |

## 10. Recommended Fix Plan

### Must Fix Before Release

Disable production profiling.

### Should Fix Soon

Sanitize pitch/FM/sync inputs and publish a mono/poly contract.

### Nice to Have

Add polyphony and hard-sync spectral tests.

## 11. Suggested Tests

Sweep pitch accuracy and amplitude at 44.1-192 kHz; test sync at sub-sample positions, extreme FM, sub gate edges, disconnected gate normalization, preset/duplicate/reset, non-finite CV, 100-instance CPU, and preview zoom. Extend `undertow_shape_spec` with full oscillator output bounds.

## 12. Final Verdict

Status: Near release-ready  
Primary blocker: production profiling configuration  
Best next action: disable debug and add oscillator-level SR/sync tests
