# IntegralFlux.cpp – Codex Patch Spec: CV Soft Saturation + Slew Warp Phase Normalization

Target file: `src/IntegralFlux.cpp`
Applies to: `IntegralFlux::computeStageTime()`, `IntegralFlux::processOuterChannel()`, `IntegralFlux::processUnifiedShapedSlew()`, `OuterChannelState`
Intent: Remove hard CV clamp artifacts; make slew warp curvature depend on segment progress (phase), not absolute output magnitude.

---

## PART A — Soft-Saturate Rise/Fall/BOTH CV Before Exponential Mapping

### A1) Problem in Current Code
Two places hard-clamp CV to ±`CV_CLAMP_V` before converting to octaves:

1) `IntegralFlux::computeStageTime()`
```cpp
float stageOct = clamp(clamp(stageCv, -CV_CLAMP_V, CV_CLAMP_V) * STAGE_CV_OCT_PER_V, -CV_OCT_CLAMP, CV_OCT_CLAMP);
```

2. `IntegralFlux::processOuterChannel()` (BOTH)

```cpp
float bothOct = clamp(-clamp(bothCv, -CV_CLAMP_V, CV_CLAMP_V) * BOTH_CV_OCT_PER_V, -CV_OCT_CLAMP, CV_OCT_CLAMP);
```

Hard clamp introduces a “brick wall” in response near ±8V.

### A2) Required Behavior

Replace voltage hard clamp with **soft saturation** that:

* is ~linear around 0V,
* smoothly compresses toward ±8V,
* avoids derivative discontinuities,
* remains safe for `exp2_taylor5()`.

### A3) Implementation Requirements (Exact)

#### A3.1 Add this helper (place near other statics, e.g. near `shapeSignedFromKnob()`):

```cpp
static float softClamp8(float v) {
    // Smoothly approaches ±8V; linear near 0V
    return 8.0f * tanhf(v / 8.0f);
}
```

#### A3.2 Update `computeStageTime()` stage CV octave computation:

Replace:

```cpp
float stageOct = clamp(clamp(stageCv, -CV_CLAMP_V, CV_CLAMP_V) * STAGE_CV_OCT_PER_V, -CV_OCT_CLAMP, CV_OCT_CLAMP);
```

With:

```cpp
float stageCvSoft = softClamp8(stageCv);
float stageOct = clamp(stageCvSoft * STAGE_CV_OCT_PER_V, -CV_OCT_CLAMP, CV_OCT_CLAMP);
```

#### A3.3 Update `processOuterChannel()` BOTH octave computation:

Replace:

```cpp
float bothOct = clamp(-clamp(bothCv, -CV_CLAMP_V, CV_CLAMP_V) * BOTH_CV_OCT_PER_V, -CV_OCT_CLAMP, CV_OCT_CLAMP);
```

With:

```cpp
float bothCvSoft = softClamp8(bothCv);
float bothOct = clamp(-bothCvSoft * BOTH_CV_OCT_PER_V, -CV_OCT_CLAMP, CV_OCT_CLAMP);
```

### A4) Keep Existing Safety Limits

Do NOT change:

* `CV_OCT_CLAMP`
* the time clamps in `computeStageTime()` (`absoluteMinTime`, `maxTime`)
* caching / interpolation logic

Soft saturation replaces only the voltage limiting step.

---

## PART B — Slew Warp Must Use True Segment Phase (Not |out|)

### B1) Problem in Current Code

In `processUnifiedShapedSlew()`, warp uses:

```cpp
float x = clamp(std::fabs(out) / std::max(OUTER_V_MAX, 1e-6f), 0.f, 1.f);
```

This makes curvature depend on output magnitude (and DC offset), causing:

* asymmetry for bipolar signals,
* curvature changes when signal is offset,
* divergence from “function generator” feel (which is phase-based).

### B2) Final Decision: Use Integrator-Derived Segment Phase with Persistent State

We will compute x from segment progress:

`x = (out - segStartOut) / (segTargetOut - segStartOut)` clamped to [0..1]

This requires persistent per-channel state because `processUnifiedShapedSlew()` is currently stateless.

### B3) Required State Variables (Add to `OuterChannelState`)

Add the following to `struct OuterChannelState` (near `float out` is fine):

```cpp
// Slew warp phase tracking (for processUnifiedShapedSlew)
int slewDir = 0;            // +1 rising toward target, -1 falling toward target, 0 uninitialized
float slewStartOut = 0.f;   // output at start of current slew segment
float slewTargetOut = 0.f;  // current target voltage for the segment
float slewInvSpan = 0.f;    // cached 1 / (slewTargetOut - slewStartOut)
```

### B4) Add Helper: Compute Segment Phase

Add near other statics:

```cpp
static float computeSegPhase(float out, float startOut, float invSpan) {
    if (fabsf(invSpan) < 1e-9f) return 1.f;
    float phase = (out - startOut) * invSpan;
    return clamp(phase, 0.f, 1.f);
}
```

### B5) Change `processUnifiedShapedSlew()` Signature to Accept Channel State

Current signature:

```cpp
float processUnifiedShapedSlew(float out, float in, float riseTime, float fallTime, float shapeSigned, float warpScale, float dt)
```

Change to:

```cpp
float processUnifiedShapedSlew(
    OuterChannelState& ch,
    float in,
    float riseTime,
    float fallTime,
    float shapeSigned,
    float warpScale,
    float dt
)
```

And update call site in `processOuterChannel()` (slew path) from:

```cpp
ch.out = processUnifiedShapedSlew(ch.out, in, riseTime, fallTime, shapeSigned, scale, dt);
```

to:

```cpp
ch.out = processUnifiedShapedSlew(ch, in, riseTime, fallTime, shapeSigned, scale, dt);
```

### B6) Implement Segment Tracking Inside `processUnifiedShapedSlew()`

#### B6.1 Determine current slew direction and segment target

At function start:

* Compute `delta = in - ch.out`
* Determine `dir = (delta > 0) ? +1 : -1` (if delta == 0 return)

The target for the segment is always the instantaneous input `in` (the slewer chases input).

#### B6.2 Detect segment resets

Reset the segment start/target when either:

* direction changes (rising → falling or vice versa), OR
* target changes “enough” while direction stays the same (to avoid stale normalization)

Define:

```cpp
const float TARGET_EPS = 1e-4f;
```

Reset condition:

```cpp
bool dirChanged = (ch.slewDir != dir);
bool targetChanged = (fabsf(in - ch.slewTargetOut) > TARGET_EPS);
if (ch.slewDir == 0 || dirChanged || targetChanged) {
    ch.slewDir = dir;
    ch.slewStartOut = ch.out;
    ch.slewTargetOut = in;
    float span = ch.slewTargetOut - ch.slewStartOut;
    ch.slewInvSpan = (fabsf(span) < 1e-6f) ? 0.f : (1.f / span);
}
```

#### B6.3 Use phase as warp driver x

Replace:

```cpp
float x = clamp(std::fabs(out) / std::max(OUTER_V_MAX, 1e-6f), 0.f, 1.f);
```

With:

```cpp
float x = computeSegPhase(ch.out, ch.slewStartOut, ch.slewInvSpan);
```

This precomputes the reciprocal once per segment boundary and replaces a per-sample divide with a multiply in the hot path.

#### B6.4 Keep existing step math

Keep:

* `stageTime` selection by delta sign,
* `dp = clamp(dt / stageTime, 0..0.5)`,
* `step = dp * slopeWarp(x, shapeSigned) * warpScale * range`,
* overshoot clamp to `in`

BUT: `range` should remain `OUTER_V_MAX - OUTER_V_MIN` as currently, to preserve overall slew scaling.

### B7) Expected Behavioral Outcomes

* Bipolar slew curvature becomes symmetric when Rise/Fall settings match.
* Adding DC offset to input no longer changes warp curvature (normalized).
* Slew warp “feels” closer to the cycle/function generator phase shaping.

---

## PART C — Numeric Acceptance Criteria

### C1) CV Soft Saturation (Timing Continuity)

Setup:

* Fix Rise/Fall knobs.
* Fix shape at LINEAR (`shape ≈ 0.33`).
* Evaluate computed stage time (Rise or Fall) as a function of CV.

Measure:

* `T(v)` from `computeStageTime()` for `v ∈ {+7.9, +8.0, +8.1}` and similarly around -8.

Pass/Fail:

1. Edge continuity at +8V:

   * `abs(T(8.0) - T(7.9)) / T(8.0) <= 0.02` (<= 2%)
2. Edge continuity at -8V:

   * `abs(T(-8.0) - T(-7.9)) / T(-8.0) <= 0.02` (<= 2%)
3. Compression beyond rails:

   * `abs(T(12.0) - T(8.0)) / T(8.0) <= 0.10` (<= 10%)
   * `abs(T(-12.0) - T(-8.0)) / T(-8.0) <= 0.10` (<= 10%)

### C2) Slew Warp Symmetry (Bipolar Triangle)

Setup:

* Input: triangle -5V..+5V @ 33Hz
* Rise knob == Fall knob
* shapeSigned set non-zero (test both log and exp sides)
* No cycle; signal patched (slew mode)

Metric:

* Normalize each half-cycle segment output into phase domain:

  * rising: `nr(p)` sampled at phase p in {0.25, 0.5, 0.75}
  * falling: `nf(p)` sampled at phase p in {0.25, 0.5, 0.75}

Symmetry error:

* `e(p) = abs(nr(p) - (1 - nf(1-p)))`

Pass/Fail:

* `max_p e(p) <= 0.02` (<= 2% normalized curvature mismatch)

### C3) DC Offset Invariance

Repeat C2 with input offset +2V (triangle -3V..+7V)

Pass/Fail:

* `max_p e(p) <= 0.02` still holds.

---

## PART D — Notes / Non-Goals

* Do not change cycle-mode warp; it already uses `x = (out - OUTER_V_MIN) / range`.
* Do not alter `slopeWarp()` / `slopeWarpScale()` math.
* Do not change stage-time calibration constants.
* This patch is intended to be behaviorally “more analog” without altering overall timing range.

---

End of Spec
