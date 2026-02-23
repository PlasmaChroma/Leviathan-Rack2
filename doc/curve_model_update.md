# Spec: Maths Outer Channel Cycle/Envelope Curve Model Update (Measured HW Alignment)

## Summary

Update the CH1/CH4 “outer channels” envelope/cycle generator so that the **Curve** knob affects the **slew rate as a function of the current output voltage** (i.e., shape acts on **dV/dt**), rather than warping a linear time ramp.

This change is required to match measured Make Noise Maths behavior where, in self-cycle on CH4 OUT:

* Log mode produces a domed top and sharp bottom cusp (fall appears “inverted log”).
* Linear behavior occurs near curve knob ≈ **0.33** (not at 0.5).
* Exp mode produces very narrow peaks (“spikes”).

## Scope

**In scope**

* Modify the OUTER_RISE / OUTER_FALL generation path (cycle + triggered envelope) for CH1 and CH4.
* Ensure the Curve knob’s “true linear point” is **LINEAR_SHAPE = 0.33** (already defined) and used consistently.
* Fix self-cycle peak amplitude to match measured CH4 OUT Vpp (~10.2V) instead of the current cycle peak=8V.

**Out of scope (Phase 2)**

* Exact EOR/EOC pulse widths/timing (current implementation is phase-gate-like, not pulse-like).
* Full circuit emulation (diode drops, op-amp saturation).
* Mixer non-idealities (SUM/OR/INV) beyond existing code.

## Current Code References

* `shapeCurve(float x, float shape)` — currently warps a time ramp.
* `processOuterChannel()` — currently does:

  * Rise: `out = peak * shapeCurve(phasePos, shape)`
  * Fall: `out = peak * (1 - shapeCurve(phasePos, shape))`
* `processRampageSlew()` — uses `shapeSigned = shape01*2 - 1` which assumes linear at 0.5 (inconsistent with LINEAR_SHAPE=0.33).

## Required Behavioral Targets (Acceptance)

When CH4 is self-cycling with no external inputs (CH4 signal input unpatched), probing **CH_4_UNITY_OUTPUT**:

1. **Amplitude**

* Vpp should be approximately **10.2V** (measured hardware shows ~10.2–10.3Vpp).
* Implement as constants per outer channel:

  * `V_MIN = 0.0f`
  * `V_MAX = 10.2f` (tunable constant; default 10.2)

2. **Linear point**

* At curve knob ≈ **0.33**, waveform should be a near-triangle when Rise ≈ Fall.

3. **Log extreme**

* At curve knob minimum, waveform should show:

  * slow near top (rounded dome)
  * fast near bottom (sharp cusp)
  * fall appears “inverted log” relative to rise (slow start of fall near top, faster later)

4. **Exp extreme**

* At curve knob maximum, waveform should show:

  * very fast movement near the top (narrow peaks/spikes)
  * lingering near bottom (spends more time low)

## Design: Voltage-Dependent Slew Rate Shaping

### Core Idea

Replace “output warp of time ramp” with “shape the instantaneous slope as a function of normalized output voltage”.

Let:

* `V` be current output voltage
* `x = (V - V_MIN) / (V_MAX - V_MIN)` clamped to [0,1]
* `T` be the target segment time (riseTime or fallTime)
* `dp = dt / T` is normalized time step
* `g(x, s)` is a positive slope multiplier derived from curve parameter `s` (signed)

Then update `x` in normalized domain:

* **Rise**: `x += dp * g(x, s) * scale(s)`
* **Fall**: `x -= dp * g(x, s) * scale(s)`

Where `scale(s)` is a normalization constant so that the segment completes in the intended time `T` (otherwise shape would distort timing unpredictably and/or double-count your existing `computeShapeTimeScale()`).

Finally:

* `V = V_MIN + x * (V_MAX - V_MIN)`

### Curve Parameter Mapping (Use LINEAR_SHAPE = 0.33)

Define a helper to map knob `shape01 ∈ [0,1]` to signed `s ∈ [-1, +1]` with **s=0 at LINEAR_SHAPE**:

* For knob < LINEAR_SHAPE: negative (log side)
* For knob > LINEAR_SHAPE: positive (exp side)

**Required helper:**

```cpp
static float shapeSignedFromKnob(float shape01) {
    shape01 = clamp(shape01, 0.f, 1.f);
    if (shape01 < LINEAR_SHAPE) {
        // [-1..0]
        return (shape01 - LINEAR_SHAPE) / LINEAR_SHAPE;
    }
    if (shape01 > LINEAR_SHAPE) {
        // [0..+1]
        return (shape01 - LINEAR_SHAPE) / (1.f - LINEAR_SHAPE);
    }
    return 0.f;
}
```

### Slope Multiplier Function g(x, s)

We need:

* **Log side (s < 0):** `g` decreases with `x` (fast at bottom, slow at top)
* **Exp side (s > 0):** `g` increases with `x` (slow at bottom, fast at top)
* **Linear (s = 0):** `g = 1`

Use a tunable family:

```cpp
static float slopeWarp(float x, float s) {
    x = clamp(x, 0.f, 1.f);
    float u = fabsf(s);                 // 0..1 magnitude
    if (u < 1e-6f) return 1.f;

    // Tunables chosen to reproduce “spikes” at EXP extreme.
    // Codex should implement as constants for easy tuning.
    constexpr float K_MAX = 40.f;       // strength (tune)
    constexpr float P     = 2.0f;       // curvature power (tune)
    float k = K_MAX * u;

    if (s < 0.f) {
        // LOG: fast at x~0, slow at x~1
        return 1.f / (1.f + k * powf(x, P));
    } else {
        // EXP: slow at x~0, fast at x~1
        return 1.f + k * powf(x, P);
    }
}
```

### Normalization scale(s)

To keep segment duration approximately equal to `T` (from `computeStageTime()`), normalize the warp.

Given:

* `dx/dp = slopeWarp(x, s) * scale(s)`
  We want `p` to advance from 0→1 while `x` goes 0→1, i.e.:
* `1 = ∫_0^1 dp = ∫_0^1 dx / (slopeWarp(x,s) * scale(s))`
  So:
* `scale(s) = ∫_0^1 dx / slopeWarp(x, s)`

Implement `scale(s)` via low-cost numeric quadrature with fixed samples `N`:

```cpp
static float slopeWarpScale(float s) {
    float u = fabsf(s);
    if (u < 1e-6f) return 1.f;

    constexpr int N = 16;
    float sum = 0.f;
    for (int i = 0; i < N; ++i) {
        float xi = (i + 0.5f) / float(N);
        sum += 1.f / slopeWarp(xi, s);
    }
    // Approx integral of 1/g over [0,1]
    return sum / float(N);
}
```

This has three critical benefits:

* At linear, scale=1.
* In LOG, scale > 1 → bottom becomes even faster relative to top (sharper cusp) while keeping total time near T.
* In EXP, scale < 1 → bottom becomes slower (more dwell low) while keeping total time near T.

## Implementation Changes (Step-by-step)

### Step 1 — Add helpers (Maths class static funcs)

Add the following static helpers near existing `shapeCurve()`:

* `shapeSignedFromKnob(shape01)`
* `slopeWarp(x, s)`
* `slopeWarpScale(s)`

Do **not** delete `LINEAR_SHAPE`; continue using it as the empirical linear point (0.33).

### Step 2 — Replace OUTER_RISE / OUTER_FALL generation in `processOuterChannel()`

#### Remove these behaviors

* Do not compute `ch.out` from `shapeCurve(phasePos, shape)` anymore.
* Do not compute fall as `1 - shapeCurve(...)` mirror.

#### New behavior

Keep `phasePos` strictly as a **time progress** variable to preserve exact `riseTime` / `fallTime` handling.

Define constants:

* `V_MIN = 0.0f`
* `V_MAX = 10.2f` (tunable; start 10.2)
* `range = V_MAX - V_MIN`

Replace the `peak` logic:

* Remove `peak = cycleOn ? 8.f : 10.f;`
* Use `V_MAX` for both cycle and one-shot envelopes unless later measurements prove a different peak.

In OUTER_RISE:

1. `ch.phasePos += dt / riseTime`
2. `s = shapeSignedFromKnob(shape)`
3. `scale = slopeWarpScale(s)`
4. `x = clamp((ch.out - V_MIN) / range, 0, 1)`
5. `dp = dt / riseTime`
6. `x += dp * slopeWarp(x, s) * scale`
7. `ch.out = V_MIN + clamp(x, 0, 1) * range`
8. If `ch.phasePos >= 1` OR `x >= 1`:

   * `ch.phasePos = 0`
   * `ch.phase = OUTER_FALL`
   * `ch.out = V_MAX` (snap to exact top)

In OUTER_FALL:

1. `ch.phasePos += dt / fallTime`
2. compute `s`, `scale`, `x`, `dp` as above
3. `x -= dp * slopeWarp(x, s) * scale`
4. `ch.out = V_MIN + clamp(x, 0, 1) * range`
5. If `ch.phasePos >= 1` OR `x <= 0`:

   * `ch.phasePos = 0`
   * `ch.phase = OUTER_IDLE`
   * `ch.out = V_MIN`

#### Notes

* Using both `phasePos` and `x` thresholding makes it robust to numerical drift and ensures segment ends cleanly.
* The normalization `scale(s)` avoids “double-counting” shape effects on timing (you already have `computeShapeTimeScale()` in `computeStageTime()`).

### Step 3 — Make Curve mapping consistent in `processRampageSlew()`

Currently:

```cpp
float shapeSigned = clamp(shape01 * 2.f - 1.f, -1.f, 1.f);
```

This assumes linear at 0.5 and is inconsistent with the measured linear point (0.33).

Replace with:

```cpp
float shapeSigned = shapeSignedFromKnob(shape01);
```

Then update the blending logic accordingly (keep your current linear/log/exp blend approach for now; just correct the signed mapping so “linear-ish” is actually at ~0.33).

### Step 4 — Update lights normalization (minor)

Your unity lights divide by 10:

```cpp
lights[LIGHT_UNITY_4_LIGHT].setBrightness(clamp(fabs(ch4.out) / 10.f, 0.f, 1.f));
```

Since `V_MAX` is ~10.2, either:

* keep as-is (fine), or
* divide by `V_MAX` for more accurate full-scale brightness.

## Tuning Guidelines (Codex should expose constants)

Codex should implement these as named constants near the helpers:

* `V_MAX = 10.2f`
* `K_MAX` (start 40.0)
* `P` (start 2.0)
* `N` for scale integral (start 16)

Tuning targets:

* Increase `K_MAX` to make exp peaks narrower and log domes more pronounced.
* Increase `P` to push shaping closer to the endpoints (more “needle-like” exp peaks).
* Increase `N` only if you see instability/warp errors; 16 is usually enough.

## Regression / Test Plan

### Visual/behavioral tests (primary)

Patch: CH4 cycle ON, CH4 signal input unpatched, probe CH4 unity output.

* Test A (Log):

  * curve=0.0
  * rise≈11 o’clock, fall≈11 o’clock
  * Expect: domed top, sharp bottom cusp, fall “inverted-log-looking”
* Test B (Linear):

  * curve≈0.33
  * Expect: near-triangle
* Test C (Exp):

  * curve=1.0
  * Expect: very narrow top peaks/spikes, longer dwell near bottom

### Quantitative tests (optional but recommended)

For each shape setting, compute from sampled waveform:

* `dutyHigh = time(V > 0.9*V_MAX) / period`
* `dutyLow  = time(V < 0.1*V_MAX) / period`

Expect:

* Log: `dutyHigh` relatively large, `dutyLow` relatively small
* Exp: `dutyHigh` very small, `dutyLow` relatively large
* Linear: both moderate, roughly symmetric

### Timing tests (sanity)

Hold rise/fall fixed and verify:

* Shape changes do not explode timing unexpectedly (normalization prevents this).
* Any remaining timing differences should come primarily from your existing `computeShapeTimeScale()` calibration.

## Implementation Notes / Safety

* Clamp `riseTime`/`fallTime` to >0 as already done in `computeStageTime()`.
* Guard against `dt / T` being huge (extreme settings) by clamping `dp` to a max step (e.g. 0.5) to prevent numerical overshoot.
* Keep `V_MIN/V_MAX` only for the cycle/envelope path; do not clamp slew-following external input path unless later measurements demand it.

---

## Optional Phase 2 (separate spec)

After waveform is matched, do a second pass to implement proper EOR/EOC **pulses** (PulseGenerator) instead of current “phase high” gates.

---

If you want, I can also write a tiny **diff-style patch plan** (function-by-function edits) that Codex can apply directly, but the above is already structured to be “implement from spec” without needing extra interpretation.
