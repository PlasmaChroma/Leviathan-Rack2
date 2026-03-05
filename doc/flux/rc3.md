# Integral Flux – CV Soft Saturation & Slew Phase Normalization Spec

Version: 1.0  
Applies to: CH1 / CH4 outer channels  
Scope: CV preprocessing + slew warp behavior  
Non-breaking: Yes (behavior refinement only)

---

# PART I — Replace Hard CV Clamp with Analog-Style Soft Saturation

## 1. Motivation

Current behavior:
- Stage CV and BOTH CV are hard-clamped to ±8V.
- This produces a digital “brick wall” response at extremes.

Real analog circuits:
- Compress near rails.
- Do not exhibit abrupt slope discontinuities.
- Feel more elastic under heavy modulation.

Goal:
- Preserve bounded input range.
- Avoid infinite exponential blowup.
- Introduce smooth headroom compression.

---

## 2. Required Behavior

Replace all instances of:

    clamp(v, -8.0f, 8.0f)

with a soft-saturation function that:

- Approaches ±8V asymptotically
- Is linear around 0V
- Has continuous first derivative

---

## 3. Soft Saturation Function

Preferred implementation:

    float softClamp8(float v) {
        return 8.0f * tanhf(v / 8.0f);
    }

Properties:
- Linear near zero
- Smooth asymptote at ±8
- Stable under SIMD exp2 mapping
- No discontinuities

Optional faster approximation:

    float softClamp8_fast(float v) {
        float x = v / 8.0f;
        return 8.0f * (x / (1.0f + fabsf(x)));
    }

Use fast version only if profiling demands it.

---

## 4. Where to Apply

Apply softClamp8() to:

- Rise CV
- Fall CV
- BOTH CV

Before converting to octave shift.

Example modification:

Old:

    float vR = clamp(riseCv, -CV_CLAMP_V, CV_CLAMP_V);

New:

    float vR = softClamp8(riseCv);

Repeat for vF and vB.

---

## 5. No Other Changes Required

- Octave clamp remains in place.
- exp2 mapping remains unchanged.
- Time hard-min clamp remains in place.

---

# PART II — Slew Warp Should Use Normalized Phase Instead of |Output|

## 1. Motivation

Current warp intensity is derived from:

    x = abs(out) / OUTER_V_MAX

This causes:
- Warp to depend on signal amplitude.
- Bipolar input to produce asymmetric curvature.
- Shape changes when offset shifts.
- Behavior to differ for slew vs cycle modes.

Analog function generators typically shape based on:
- Internal traversal position of the ramp,
- Not absolute output magnitude.

Goal:
- Make warp dependent on segment progress.
- Keep curvature consistent regardless of amplitude.

---

## 2. Define Normalized Segment Phase

For each active segment (Rise or Fall):

Define:

    phase = clamp( elapsedTime / segmentDuration, 0.0f, 1.0f );

Where:
- elapsedTime accumulates per segment
- segmentDuration is T_rise or T_fall

If segment time changes mid-segment:
- Recompute phase from integrator position rather than elapsed time.

Alternative robust method:

    phase = clamp((out - startLevel) / (targetLevel - startLevel), 0, 1);

This ensures stability under time modulation.

Preferred: Use integrator-derived normalization (not raw output magnitude).

---

## 3. Warp Mapping

Replace current warp driver:

Old:

    float x = clamp(abs(out) / OUTER_V_MAX, 0.f, 1.f);

New:

    float x = phase;

Warp mapping remains:

- Linear: x
- Log: pow(x, warpExponent)
- Exp: 1 - pow(1 - x, warpExponent)

WarpExponent remains derived from warp parameter.

---

## 4. Behavior Requirements

After modification:

- Warp should be independent of output offset.
- Bipolar slewing should preserve symmetry.
- Shape should remain consistent across amplitude changes.
- Slew mode should visually resemble cycle mode curvature.

---

## 5. Edge Case Handling

If segmentDuration <= absoluteMinTime:

- Force phase = 1.0f

Prevent divide-by-zero.

If targetLevel == startLevel:

- Force phase = 1.0f

---

## 6. Performance Considerations

- Phase computation is cheap (one divide).
- Warp mapping already uses powf; no additional heavy math introduced.
- No impact on SIMD CV path.

---

# VALIDATION CHECKS

After implementing:

1. Slam BOTH with ±10V:
   - No abrupt timing discontinuities.
   - Response compresses smoothly.

2. Slew a ±5V triangle:
   - Rise/Fall curvature symmetric.
   - Warp unaffected by DC offset.

3. Compare cycle vs slew:
   - Curvature visually similar on scope.

---

# RESULTING BEHAVIOR

The module will:

- Respond to CV with analog-style headroom compression.
- Avoid harsh exponential cliffs.
- Maintain curvature integrity during slew limiting.
- Feel closer to a discrete transistor expo core.

---

End of Spec