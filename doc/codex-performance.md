# Codex Performance Analysis: Integral Flux (VCV Rack)

## Scope

This document analyzes runtime CPU cost for the current `src/IntegralFlux.cpp` implementation and proposes concrete optimization work.

Context from user report:
- Integral Flux instance: ~5% CPU
- Befaco Rampage instance: ~1.1% CPU

Goal:
- Identify likely hotspots
- Rank optimization opportunities by impact vs risk
- Provide a practical implementation roadmap

---

## Executive Summary

A ~4-5x CPU gap versus Rampage is plausible with the current feature set and math choices, but it is likely reducible.

Highest-cost contributors are likely:
1. Per-sample transcendental math (`std::pow`, `std::tanh`) in outer-channel processing and mix non-ideal mode.
2. Continuous recomputation of timing expressions (`computeStageTime`) for both channels every sample.
3. BLEP gate edge smoothing (`MinBlepGenerator`) on EOR/EOC every sample.
4. Extra branch/mixing logic always executed even when channels are effectively static.

Good news:
- Major previous hotspot (`slopeWarpScale`) was already addressed with per-channel caching.
- Cycle CV has been corrected to level-sensitive behavior without obvious performance penalty.

---

## Current Hotspot Inventory

## 1) Outer-channel envelope/slew core

In `processOuterChannel()` the following run every sample for each outer channel:
- `computeStageTime(...)` for rise and fall
- shape mapping and warp application
- phase transition handling including BLEP insertions
- slew-mode processing when signal patched

Relevant locations:
- `computeStageTime`: `src/IntegralFlux.cpp` (`std::pow` heavy)
- `processOuterChannel`: `src/IntegralFlux.cpp`
- `processUnifiedShapedSlew`: `src/IntegralFlux.cpp`

### Likely cost drivers
- `std::pow` in timing and shaping code
- frequent clamps and conditionals
- duplicated stage-time computations in cases where controls are static

## 2) Mix non-ideal path

When enabled (`mixCal.enabled`), mix outputs use soft saturation:
- `softSatSym` uses `std::tanh`
- `softSatPos` also uses `std::tanh`

Relevant locations:
- `softSatSym`, `softSatPos`
- mix section in `process()`

### Likely cost drivers
- 2-3 `tanh` evaluations per sample per module
- no fast-path for small-signal linear region

## 3) Gate BLEP smoothing

EOR/EOC output path uses:
- `dsp::MinBlepGenerator<16,16>` for each outer channel
- `gateBlep.process()` every sample

Relevant locations:
- `OuterChannelState` gate BLEP member
- `insertGateTransition`
- `eorOut`/`eocOut` output construction in `process()`

### Likely cost drivers
- BLEP processing per sample even when no visible need for anti-aliased gates in CV context

## 4) I/O and bus routing overhead

Not individually large, but cumulative:
- many `isConnected()`, `getVoltage()`, and output/light writes every sample

This is probably not the primary issue, but contributes.

## 5) UI path (likely secondary)

Current image widget is behind `#if 0` in widget ctor, so panel JPG draw cost should not currently matter.
There is still custom switch widget code (`IMBigPushButton`), but UI cost should be much lower than DSP cost during steady audio processing.

---

## Already-Implemented Optimization (Important)

`slopeWarpScale(s)` is cached per channel and recomputed only when shape changes, via:
- `warpScaleValid`
- `cachedShapeSigned`
- `cachedWarpScale`

This was a critical optimization and should be retained.

---

## Performance Risk Ranking

## High impact / low-to-medium risk

1. Cache/reduce `computeStageTime` recomputation
- Current: recomputed every sample, both rise and fall, both channels.
- Proposal: compute only when dependent inputs change beyond epsilon.
- Dependencies:
  - rise/fall knob
  - rise/fall/both CV
  - shape and shape-time scales
- Expected gain: high.

2. Add fast-path for mix non-ideal saturation
- For small `|x|` use linear approximation instead of `tanh`.
- Example threshold near `|x| < 0.5V` (tunable).
- Keep exact `tanh` for larger amplitudes.
- Expected gain: medium-high with minimal sound impact.

3. Optional disable of BLEP on EOR/EOC
- Add context menu toggle: `Band-limited EOR/EOC`.
- Default OFF for CV-focused performance mode.
- Expected gain: medium.

## Medium impact / medium risk

4. Replace some `pow` calls with cheaper forms
- When exponent is fixed small integer (e.g. `WARP_P=2`), use multiply (`x*x`) instead of `pow`.
- Use precomputed constants for repeated `pow(base, expr)` patterns where possible.
- Expected gain: medium.

5. Cache shape-sign conversion for steady controls
- shape mapping itself is cheap; cache only if bundled with stage-time cache to reduce branching.
- Expected gain: low-medium.

## High impact / higher risk

6. LUT-based approximations for `tanh` and/or nonlinear mapping
- Precompute LUT for `tanh` and linear interpolate.
- Keeps smooth behavior, reduces transcendental cost.
- Risk: slight transfer-shape deviation if LUT coarse.
- Expected gain: medium-high for large instance counts.

7. Control-rate evaluation split
- Evaluate slow-moving controls/CVs at control rate (e.g., every 8-16 samples).
- Interpolate where needed.
- Risk: can alter fast-modulation response.
- Expected gain: high, but must be measured carefully against behavior goals.

---

## Concrete Optimization Plan

## Phase A (safe, immediate)

1. Stage-time caching per outer channel
- Add cached values and "dirty" checks for riseTime/fallTime inputs.
- Recompute timing only when any dependency changes by epsilon.

2. Micro-optimize warp math
- If `WARP_P == 2.f`, replace `pow(x, WARP_P)` with `x * x`.

3. Add small-signal linear fast-path in `softSatSym`
- For `abs(x) < xLinearThresh`, return `x * driveLinear`.
- Keep existing tanh path otherwise.

4. Add runtime toggle for BLEP
- Context menu bool; if off, output hard gate without BLEP processing.

## Phase B (measurement-backed)

5. Profile-based tuning with VCV Rack meter
- Compare CPU before/after each change.
- Record at least:
  - idle module
  - CH1+CH4 cycling
  - audio-rate signal slew
  - non-ideal mix on/off

6. Optional LUT for tanh if still above target.

---

## Suggested Instrumentation

Add temporary lightweight counters/timers (compile-time guarded):
- time spent in `processOuterChannel` CH1
- time spent in `processOuterChannel` CH4
- time spent in mix/saturation section
- counts of `computeStageTime` recomputations

This will confirm whether optimization effort is going to the true bottleneck.

---

## Expected Outcome

With Phase A implemented, a meaningful reduction is realistic. A conservative expectation is substantial drop from current ~5%, though exact final value depends on:
- Rack sample rate
- whether both channels are active/cycling
- non-ideal mix mode usage
- EOR/EOC BLEP mode

Reaching near Rampage-level CPU may still require further simplification or control-rate partitioning, because current model intentionally includes extra non-ideal and curve-shaping complexity.

---

## Notes

- This analysis is static (code-based) and should be validated with profiling runs inside Rack.
- Keep sound/behavior parity checks after each optimization stage to avoid regressions.
