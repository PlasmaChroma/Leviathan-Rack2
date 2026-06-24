# Proc Review

## 1. Executive Summary

Proc is a compact single-channel function generator, slew limiter, envelope follower, and bipolar output utility. Its shared design lineage with Integral Flux is technically solid and musically direct. The biggest release risks are the shipped debug timer, mono-only behavior, and lack of DSP regression tests.

Release readiness: 7/10

## 2. Module Inventory

- Source/UI: `src/Proc.cpp`; shared math, preview, SVG, visual and debug helpers. Panel: `res/proc.svg` and shared icons.
- Params: Cycle, Rise, Fall, Shape, Function amplitude. Inputs: Signal, Trigger, Halt, Rise CV, Both CV, Fall CV. Outputs: EOR, EOC, Positive, Negative. Lights: Cycle, EOR, EOC, Positive, Negative.
- Menu/state: gate/signal bandlimiting, tracer/quality, timing interpolation/rate; JSON saves cycle latch and these options. No expander.

## 3. DSP and Audio/CV Correctness

`processChannel()` uses sample-time-based segments, stage caching, Schmitt triggers, optional MinBLEP, and a smooth signal injection path (`Proc.cpp:634-918`). Main/negative outputs are complementary. Halt is level-sensitive. External voltages are not finite-sanitized and only channel 1 is processed. No explicit `onReset()` clears integrator/trigger state.

## 4. UI, Panel, and Interaction Review

The compact hierarchy, amplitude readout, preview, and aperture lights communicate function well. The advanced performance submenu is too technical for routine use. Validate the sparse screw layout and readout legibility at low zoom.

## 5. Performance Review

- Severity: High — debug mode causes two high-resolution clock reads per audio sample (`Proc.cpp:999-1111`, `res/dragonking.txt`).
- Severity: Medium — widget `step()` and `draw()` are timed unconditionally (`Proc.cpp:1475-1517`).
- Severity: Low — caches, light throttling, and preview publication are otherwise allocation-free and bounded.

## 6. Stability and Rack Integration

Enum order and a legacy `ch1CycleLatched` key preserve patches. `dataFromJson()` has no null/type guard (`Proc.cpp:953`). Assets are relative and preview construction has fallbacks. Preset/reset state-transition tests are absent.

## 7. Code Quality and Maintainability

The configuration-driven channel engine is clear, but 1,713 lines combine DSP, custom UI, metrics, and menus. Extracting a testable engine is justified; a broad rewrite is not.

## 8. Musical Usefulness

The amplitude control, halt input, dual end gates, and negative output distinguish Proc from a bare slew. Ranges and defaults are useful, though mono-only behavior and advanced anti-alias options need documentation.

## 9. Bugs and Risk Register

| ID | Severity | Area | Finding | Evidence | Suggested Fix |
| -- | -------- | ---- | ------- | -------- | ------------- |
| PROC-001 | High | Performance | Audio callback is timed every sample in shipped configuration. | `Proc.cpp:999-1111`; `res/dragonking.txt` | Disable debug in package; sample metrics sparsely. |
| PROC-002 | Medium | Performance | UI timing runs even outside debug mode. | `Proc.cpp:1475-1517` | Guard instrumentation. |
| PROC-003 | Medium | DSP | Mono-only, non-finite input propagation. | `Proc.cpp:666-692,1096-1099` | Document or implement polyphony; sanitize voltages. |
| PROC-004 | Medium | Verification | No direct engine tests. | `Makefile` | Add segment/gate/slew/SR tests. |

## 10. Recommended Fix Plan

### Must Fix Before Release

Remove production audio-thread profiling.

### Should Fix Soon

Guard UI profiling, add DSP tests, and define mono/poly policy.

### Nice to Have

Split the engine from UI and reduce context-menu complexity.

## 11. Suggested Tests

Measure envelopes and slew at four sample rates; patch audio-rate trigger, Halt, bipolar signal and rapid cycle toggles; verify EOR/EOC, amplitude, duplicate/preset/reset, disconnected signal, NaN rejection, 100 instances, and zoom/readout clarity.

## 12. Final Verdict

Status: Near release-ready  
Primary blocker: debug instrumentation in the audio callback  
Best next action: ship debug off and establish DSP regression tests
