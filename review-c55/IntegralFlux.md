# Integral Flux Review

## 1. Executive Summary

Integral Flux is a four-channel Maths-style function/slew/mixer: CH1/CH4 are shaped generators, CH2/CH3 attenuverters, and normalized SUM/OR/INV buses complete the instrument. The implementation broadly matches that intent and has careful timing caches and optional MinBLEP edges. Release risk is mainly production debug overhead, mono-only processing, and missing direct DSP tests.

Release readiness: 7/10

## 2. Module Inventory

- Source/UI: `src/IntegralFlux.cpp`; shared `MathHelpers`, `WavePreviewTracer`, `PanelSvgUtils`, `VisualAssets`, `ApertureLight`, and debug transport. Panel: `res/flux.svg` plus shared `res/icon/*` assets.
- Params (12): four attenuverters; CH1/CH4 cycle, rise, fall, and shape. Inputs (14): CH1/4 signal, trigger, rise/both/fall/cycle CV plus CH2/3 signal. Outputs (11): four variable, CH1/4 unity, EOR/EOC, OR/SUM/INV. Lights (8): cycle, EOR/EOC, unity and bus polarity.
- Menu/state: bandlimited gates/signals, preview renderer/tracer/quality, timing interpolation/rate; JSON also saves both cycle latches. No expander.

## 3. DSP and Audio/CV Correctness

Timing uses `args.sampleTime`, stage caches, Schmitt triggers, finite minimum times, output clamps, and optional MinBLEP (`processOuterChannel()`, `process()`, lines 697-1260). Normalization removes a channel from the mix bus when its variable output is patched. The entire module is monophonic and does not sanitize non-finite cable voltages; this should be documented or hardened. There is no explicit reset override, so reset relies on construction/default member state and Rack parameter reset.

## 4. UI, Panel, and Interaction Review

The panel, halo controls, aperture lights, and dual previews are visually coherent and communicate the four-channel hierarchy. Advanced performance/renderer choices are overexposed for a musical module, especially because production debug is enabled. Verify SVG anchor fallbacks at 50%-200% zoom and label the OR/SUM/INV normalization in a manual.

## 5. Performance Review

- Severity: High — shipped debug mode makes `process()` call `steady_clock::now()` every sample (`IntegralFlux.cpp:1094-1097`, `res/dragonking.txt`).
- Severity: Medium — two animated previews and instrumented custom controls increase UI cost; timing is recorded even when metrics are not user-relevant (`IntegralFlux.cpp:2060-2118`).
- Severity: Low — timing caches and rate-limited lights/previews are sound optimizations; no ordinary per-sample heap allocation was found.

## 6. Stability and Rack Integration

Patch compatibility is deliberately protected by enum ordering and legacy JSON keys. `dataFromJson()` assumes a non-null object (`IntegralFlux.cpp:1041`), and malformed numeric CV can contaminate state. Assets are plugin-relative. Duplicate/preset behavior should be tested because cycle latches and advanced performance options serialize.

## 7. Code Quality and Maintainability

CH1/CH4 share a configuration-driven DSP path, which is maintainable. The 2,198-line combined DSP/UI file and embedded profiling/debug infrastructure are the main liabilities. Extract the outer-channel engine and tests before changing behavior; do not rewrite the working UI.

## 8. Musical Usefulness

The normalization, default 10 V/5 V offsets on CH2/3, shaped slew, and end gates make this a strong standalone modulation hub. Defaults are playable. Polyphonic users will receive only channel 1 without an explicit warning.

## 9. Bugs and Risk Register

| ID | Severity | Area | Finding | Evidence | Suggested Fix |
| -- | -------- | ---- | ------- | -------- | ------------- |
| INTEGRALFLUX-001 | High | Performance | Production debug times every audio sample. | `res/dragonking.txt`; `IntegralFlux.cpp:1094-1097` | Ship debug disabled and compile/profile behind an opt-in gate. |
| INTEGRALFLUX-002 | Medium | DSP | All I/O is mono despite a multi-channel utility role. | `IntegralFlux.cpp:718-745,1203-1243` | Document mono behavior or add bounded 16-channel state. |
| INTEGRALFLUX-003 | Medium | Verification | No dedicated DSP regression target exists. | `Makefile` test list | Extract/test timing, normalization, reset and SR invariance. |
| INTEGRALFLUX-004 | Low | Robustness | JSON and cable floats are not type/finite guarded. | `IntegralFlux.cpp:1041-1087,718-745` | Validate JSON types and sanitize external voltages. |

## 10. Recommended Fix Plan

### Must Fix Before Release

Disable distributable debug profiling.

### Should Fix Soon

Add DSP tests; state/document mono behavior; harden non-finite inputs.

### Nice to Have

Split DSP/UI and simplify the public context menu.

## 11. Suggested Tests

At 44.1/48/96/192 kHz, measure rise/fall time and EOR/EOC width; exercise trigger/cycle/halt transitions, all cable-normalization combinations, preset/duplicate/reset, NaN injection, zoom levels, and 50-instance CPU. Add unit tests around `processOuterChannel()` and SUM/OR/INV removal.

## 12. Final Verdict

Status: Near release-ready  
Primary blocker: production debug overhead  
Best next action: disable debug-by-default and add timing/normalization tests
