Absolutely, Dragon King Leviathan. I pulled up your current `WavePreviewWidget` in `IntegralFlux.cpp`, and the “bottomed out then restarted” behavior is coming from the **current approach intentionally tiling cycles** via:

```cpp
float p = std::fmod(local + risePx, periodPx);
...
if (p < risePx) { ... } else { ... }
```

That guarantees there will be **wrap seams** (a trough seam every `periodPx`), and when your “dynamic time window scaling” shifts `periodPx` / `risePx` / `fallPx` (plus smoothing), the seam can drift into awkward places. Even with the seam jitter epsilon, you’ll still *see* “hits bottom then restarts” because that’s literally the wrap boundary.

Below is a Codex spec to “get itself back in line” by changing the method:

* **Never `fmod` / wrap phase inside the preview.**
* Build **one canonical cycle** (peak at center) and then **crop a window** around the center when the times are slow.
* Keep your dynamic scaling, but it becomes a **zoom/crop width**, not “cycles across width”.

---

# Codex Spec — Fix Generator Wave Preview to Avoid Seam Restarts

## File / Target

* File: `IntegralFlux.cpp`
* Class: `WavePreviewWidget`
* Primary function to replace: `rebuildPoints(float riseTime, float fallTime, float curveSigned, bool interactiveRecent)`

## Problem Statement

The preview currently uses **periodic tiling** (`fmod`) across the widget width, producing visible **wrap seams** (trough-to-trough discontinuities). When period smoothing and dynamic window scaling change the seam position, the preview can appear to “bottom out early then restart”, especially at slow times or when CV/knobs move.

## Goals

1. Preview shows **a single-cycle shape** with **peak fixed at widget midpoint** (`x = 0.5 * width`).
2. When Rise/Fall are very slow, preview should **clip horizontally** (zoom in on a portion around the peak), not restart.
3. Preserve performance: small fixed point buffer, no allocations, controlled smoothing.

## Non-Goals

* No need to be DSP-accurate oscilloscope.
* No multi-cycle tiling.
* No phase accumulator.

---

## New Rendering Model: Canonical Cycle + Center Crop Window

### Definitions

* `w = box.size.x`, `h = box.size.y`
* Normalized cycle domain: `u ∈ [0, 1]`
* Peak is at `u = 0.5`
* Output wave is normalized `y ∈ [-1, 1]` where:

  * `y(0) = -1`
  * `y(0.5) = +1`
  * `y(1) = -1`

### Curve Warp (monotonic, no overshoot)

Keep curvature monotonic and endpoint-stable. Replace any integration-based slope stepping for preview with a monotonic warp.

Implement:

* `warp(t, c)` where `t ∈ [0,1]`, `cSigned ∈ [-1,1]`
* Let `u = clamp(abs(cSigned), 0, 1)`
* Let `p = 1 + CURVE_POWER * u` where `CURVE_POWER` is a constant (recommend `4.0f` to start)
* If `cSigned >= 0`: `warp = pow(t, p)`
* Else: `warp = 1 - pow(1 - t, p)`

This guarantees:

* `warp(0)=0`, `warp(1)=1`
* Monotonic increasing
* No early “bottoming out” due to overshoot

### Canonical Cycle Function

Define `yCycle(u)`:

* If `u < 0.5` (rising half):

  * `t = u / 0.5`
  * `v = warp(t, curveSigned)`
  * `y = -1 + 2*v`
* Else (falling half):

  * `t = (u - 0.5) / 0.5`
  * `v = warp(t, curveSigned)`  *(or allow separate fall curve later)*
  * `y = +1 - 2*v`

**Important:** This cycle is *not* time-scaled. It is purely a shape reference.

---

## Dynamic Time Window Scaling (Replace Period Tiling)

### Inputs

* `riseTime`, `fallTime` from module preview state
* `totalTime = max(riseTime + fallTime, 1e-6f)`
* Optionally use `freqHz = 1 / totalTime`

### Compute Visible Window Width (in cycle units)

Instead of `cyclesAcrossWidthForFrequency()` and `periodPx`, compute a *visible fraction of the cycle*:

* `visibleU ∈ (0, 1]`
* Fast times: `visibleU = 1.0` (full cycle visible)
* Slow times: `visibleU` shrinks (zoom in around peak)

Recommended mapping:

* Define a “full-cycle threshold” period:

  * `T_full = 0.05f` seconds (20 Hz) *(tune as desired)*
* Define a “max zoom” period:

  * `T_zoom = 2.0f` seconds *(tune)*
* Compute:

  * `slowNorm = clamp((totalTime - T_full) / (T_zoom - T_full), 0, 1)`
  * `visibleU = lerp(1.0f, MIN_VISIBLE_U, pow(slowNorm, 0.6f))`
* Where `MIN_VISIBLE_U` recommended `0.15f` (never show less than 15% of the cycle)

### Smooth Visible Window (keep your smoothing intent)

Reuse `smoothedPeriodPx` variable but repurpose it to smooth `visibleU`:

* Add member: `float smoothedVisibleU = 1.f;` (replace `smoothedPeriodPx`)
* On rebuild:

  * if first valid: `smoothedVisibleU = visibleU`
  * else: `smoothedVisibleU += (visibleU - smoothedVisibleU) * alpha`
* Choose `alpha` based on `interactiveRecent`:

  * interactive: `alpha = 0.35f` (snappy)
  * non-interactive/CV: `alpha = 0.15f` (stable)

---

## Point Generation (No fmod, No Multi-Cycle Anchors)

For each `i in [0..POINT_COUNT-1]`:

1. `x = (i / (POINT_COUNT - 1)) * w`
2. Map x to normalized window domain centered at peak:

   * `xNorm = x / w`
   * `u0 = 0.5f - 0.5f * smoothedVisibleU`
   * `u1 = 0.5f + 0.5f * smoothedVisibleU`
   * `u = u0 + xNorm * (u1 - u0)`
3. Clamp:

   * `u = clamp(u, 0.f, 1.f)`
4. Compute `y = yCycle(u)` using canonical function above.
5. Convert to pixel y:

   * `py = (0.5f - 0.5f * y) * h`
6. Store:

   * `points[i] = Vec(x, py)`

### Anchor Stabilization (Simplified)

Keep only the mid peak anchor stable (no repeating anchors across multiple cycles):

* Force the point nearest `x = 0.5*w` to `y = +1`
* Do **not** add anchors for “previous/next peaks” or repeated troughs (that logic belongs to the tiled model).

---

## Remove / Delete Old Logic

Codex should remove or disable:

* `cyclesAcrossWidthForFrequency()`
* All uses of `periodPx`, `risePx`, `fallPx`, `seamPxEps`
* All `fmod` usage in the preview
* The loops that stamp anchors at multiple `xPeak/xTrough` positions

---

## Expected Visual Behavior

* Peak is always centered.
* When times are slow, you see a **zoomed-in chunk around the peak**; the sides may not reach trough.
* No “restart” discontinuity inside the preview, ever.
* Curve changes only shape curvature, not phase seams.

---

## Performance & Update Strategy

Keep existing version-based rebuild behavior:

* `step()` rebuilds only when `version != lastVersion` or `!pointsValid`
* Point buffer stays as `std::array<Vec, POINT_COUNT>`
* No heap allocations in rebuild

Optional: reduce `POINT_COUNT` from 320 to 128–192; 320 is fine but more than necessary for a tiny widget.

---

## Acceptance Tests

1. Set very slow Rise/Fall: preview should *not* show a trough-to-rise restart inside the widget; it should just show a clipped shape around peak.
2. Sweep curve across range: preview remains continuous with peak fixed at center.
3. Rapid knob movement: preview stable, no seam jitter.
4. CV modulation: preview changes but does not flicker seams.

---

If you want, I can also write the *exact patch-style pseudocode* Codex can follow to modify `rebuildPoints()` line-for-line (keeping your existing helper names and member vars), but the spec above is already tight enough for Codex to implement correctly without reintroducing seams.
