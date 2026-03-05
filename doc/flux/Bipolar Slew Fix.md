# Codex-ready spec: Bipolar-safe shape normalization for signal slew path (Maths.cpp)

## Summary

When CH1/CH4 is used as a **slew limiter** (i.e., the *Signal In* jack is patched), the current shaping code normalizes `x` as if the output were always **unipolar 0 → 10.2V**. For bipolar audio/control signals (e.g., ±5V), the negative half-cycle gets pinned to `x=0`, which makes the LOG/EXP warping behave incorrectly and can present as **earlier amplitude loss** vs the physical module.

This change makes the signal-slew path’s warp-position `x` **bipolar-safe** by normalizing to the **magnitude** of the output voltage.

---

## Goals

* Fix the “negative half-cycle clamps to x=0” behavior in slew-limiting mode.
* Preserve existing rise/fall time scaling (still based on the 0→10.2V stage definition you’re already using).
* Keep the function-generator path (cycle/trigger envelope) untouched.

## Non-goals

* Re-calibrating the rise/fall knob time mapping to match hardware exactly (separate follow-up).
* Changing any output clamping/rails behavior.

---

## Files to modify

* `src/Maths.cpp` (or wherever your current `Maths.cpp` lives in the plugin)

---

## Current code (problem area)

Function:

* `static float processUnifiedShapedSlew(...)`

Current normalization (unipolar):

* `float range = OUTER_V_MAX - OUTER_V_MIN;`
* `float x = clamp((out - OUTER_V_MIN) / range, 0.f, 1.f);`

Because `OUTER_V_MIN = 0.f`, any negative `out` results in `x=0`, breaking shaping symmetry for bipolar signals.

---

## Surgical fix (implementation)

### Change: compute `x` from **|out|** instead of `(out - OUTER_V_MIN)`

In `processUnifiedShapedSlew(...)`, replace the existing `x` computation with magnitude-based normalization:

**New behavior**

* `x = clamp(abs(out) / OUTER_V_MAX, 0..1)`
* This treats the warp coordinate as “how far from 0V am I?” which is symmetric for ± voltages.
* Keep `range = OUTER_V_MAX - OUTER_V_MIN` exactly as-is for step scaling (so your rate/seconds mapping remains consistent with the 10.2V traverse assumption).

### Exact edit

Locate this block inside `processUnifiedShapedSlew(...)`:

```cpp
float stageTime = (delta > 0.f) ? riseTime : fallTime;
stageTime = std::max(stageTime, 1e-6f);
float range = OUTER_V_MAX - OUTER_V_MIN;
float x = clamp((out - OUTER_V_MIN) / range, 0.f, 1.f);
float dp = clamp(dt / stageTime, 0.f, 0.5f);
float step = dp * slopeWarp(x, shapeSigned) * warpScale * range;
```

Replace only the `x` line with:

```cpp
// Slew-limiting mode must handle bipolar signals.
// Use normalized magnitude so negative voltages don't clamp to x=0.
float x = clamp(std::fabs(out) / std::max(OUTER_V_MAX, 1e-6f), 0.f, 1.f);
```

Everything else stays the same.

**Notes**

* `std::fabs` is already used elsewhere in this file, so `<cmath>` support is already effectively assumed.
* We intentionally do **not** change `range` (still 10.2V) to avoid changing your “seconds per 10V” scaling.

---

## Acceptance criteria

1. **Symmetry restored**

* Patch a sine or triangle at ±5V into `INPUT_1_INPUT` (signal in for CH1).
* With the shape knob at EXP or LOG extremes, the output waveform should remain **visibly symmetric** about 0V (no “one side collapses earlier”).

2. **No behavior change for function-generator mode**

* With **no signal patched** into `INPUT_1_INPUT`, trigger/cycle behavior and 0→10.2V envelope behavior should be unchanged.

3. **No regressions in overshoot protection**

* Rapid steps (e.g., square wave) should still not overshoot the input target; your existing sign-crossing check remains intact.

---

## Manual test plan (quick)

### Test A: Bipolar triangle (the one that was biting you)

* Input: 33 Hz triangle, ±5V
* Mode: signal patched into CH1 signal input; view CH1 unity output
* Shape: test LOG, LIN, EXP
* Sweep rise/fall from fast → slower
* Expected:

  * Peaks reduce due to slew limiting **symmetrically**
  * EXP/LOG “character” affects curve feel, but does **not** pin the negative half-cycle into a fixed warp regime

### Test B: DC invariance sanity

* Input: +5V constant
* Expected: output eventually settles to +5V regardless of rise/fall settings (slew affects time-to-reach, not DC gain)

---

## Optional follow-up (out of scope, but likely still relevant)

If you still perceive “earlier amplitude loss” even after symmetry is fixed, the next lever is **re-fitting the rise/fall knob → time mapping** (your midrange effective slope may still be more conservative than hardware). That’s a separate calibration spec once you confirm this patch improves the bipolar behavior.

---

If you want, paste the post-change scope screenshots of ±5V triangle at the same knob positions you compared against hardware, and I’ll write the *second* Codex spec that re-anchors your knob/time curve to match the physical unit’s breakpoints.
