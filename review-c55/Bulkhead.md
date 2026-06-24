# Bulkhead Review

## 1. Executive Summary

Bulkhead is a spatial room reverb with editable room/listener/speaker geometry, first reflections and a compact comb/allpass tail. The UI concept is strong, but wall CV accumulates directly into saved room bounds every sample and the late reverb delays are fixed sample counts, making behavior unstable and sample-rate dependent. Its hidden status is correct.

Release readiness: 3/10

## 2. Module Inventory

- Source/UI: `Bulkhead.cpp/.hpp`, `BulkheadGeometry.cpp/.hpp`, `BulkheadWidget.cpp`. Assets: `res/bulkhead.svg` and reset icons.
- Params: Decay, Diffuse, Mix, Absorb, Motion. Inputs: listener X/Y, four wall distances, stereo audio. Outputs: stereo. Lights: none.
- Menu/state: direct geometry dry toggle. JSON stores room/listener/speaker positions/yaws and dry mode. No expander. `plugin.json` marks it hidden.

## 3. DSP and Audio/CV Correctness

Input R normalizes from L, mix is equal-power, outputs are soft-clipped/bounded, and delay storage is rebuilt on sample-rate changes. Critical faults remain: wall CV is added to `room.*` each audio sample (`Bulkhead.cpp:238-246`), causing rapid drift/runaway and mutating serialized state; comb/allpass base delays are raw sample constants rather than scaled by sample rate (`Bulkhead.cpp:291-325`). Input/state non-finite values are not sanitized, and JSON geometry is not normalized after load.

## 4. UI, Panel, and Interaction Review

The interactive room canvas directly communicates the spatial concept and fits the artifact aesthetic. With no lights and many geometric handles/CV jacks, labels and tooltips must explain units and whether CV is absolute or relative. The reset control is important and should be unmistakable.

## 5. Performance Review

- Severity: Medium — first-order image geometry, square roots and trigonometric equal-power mix run every sample (`Bulkhead.cpp:250-373`).
- Severity: Medium — all delay lines allocate maximum buffers on init/sample-rate change (`Bulkhead.cpp:180-214`).
- Severity: Low — ordinary processing is fixed-size and allocation-free.

## 6. Stability and Rack Integration

`onSampleRateChange()` reallocates/reset all tails, producing discontinuity but avoiding stale sizes. `dataFromJson()` accepts arbitrary/non-finite/inverted geometry (`Bulkhead.cpp:396-411`). Reset restores scene geometry but does not explicitly clear DSP delay/filter states (`Bulkhead.cpp:221-223`). Presets can therefore load invalid rooms and reset can retain reverb tails unexpectedly.

## 7. Code Quality and Maintainability

Geometry is usefully separated and has three unit tests. The comment calling the geometry/DSP “still minimal” is accurate (`Bulkhead.cpp:229`). Define immutable base geometry plus per-frame CV-derived effective geometry; this fixes drift without a rewrite.

## 8. Musical Usefulness

Direct spatial editing and geometry-derived early/late balance are compelling. Current CV behavior is unplayable over sustained gates, and sample-rate-dependent tail tuning makes presets unreliable. Keep it experimental until those fundamentals are corrected.

## 9. Bugs and Risk Register

| ID | Severity | Area | Finding | Evidence | Suggested Fix |
| -- | -------- | ---- | ------- | -------- | ------------- |
| BULKHEAD-001 | Critical | DSP/State | Wall CV accumulates into persistent room bounds every sample. | `Bulkhead.cpp:238-246` | Compute effective bounds from base state + CV without mutation. |
| BULKHEAD-002 | High | DSP | Comb/allpass times change with sample rate. | `Bulkhead.cpp:291-325` | Express base delays in seconds and scale by `sampleRate`. |
| BULKHEAD-003 | High | Robustness | JSON accepts invalid/inverted/non-finite geometry. | `Bulkhead.cpp:396-411` | Validate finite values and normalize bounds/positions. |
| BULKHEAD-004 | Medium | Lifecycle | Reset leaves DSP tail/state intact. | `Bulkhead.cpp:221-223` | Specify soft vs hard reset; clear DSP for hard reset. |
| BULKHEAD-005 | Medium | Verification | Only pure geometry has tests. | `tests/bulkhead_geometry_spec.cpp` | Add reverb/CV/SR invariants. |

## 10. Recommended Fix Plan

### Must Fix Before Release

Stop wall-CV accumulation, scale delays by sample rate, and validate loaded geometry.

### Should Fix Soon

Define reset/tail behavior, smooth geometry modulation, and benchmark DSP.

### Nice to Have

Add distance units/readouts and modulation-depth controls.

## 11. Suggested Tests

Hold ±10 V wall CV for minutes and assert bounded immutable base state; compare impulse-response times at 44.1-192 kHz; malformed/inverted JSON; mono/stereo normalization; reset/preset/duplicate; rapid geometry moves; silence/NaN recovery; 50 instances; canvas zoom and edge dragging.

## 12. Final Verdict

Status: Needs major work  
Primary blocker: cumulative wall CV and sample-rate-dependent reverb timing  
Best next action: separate base/effective geometry and convert all delays to seconds
