## Integral Flux — Generator Waveform Preview (CH1 / CH4)

### Purpose

Add a lightweight visualization widget to the module UI that previews the **generator waveform shape** for **Channel 1** and **Channel 4**, positioned **beneath the Curve knob** in each generator section.

The preview is intended to:

* Communicate the **shape** (curvature / response) rather than precise time-domain accuracy.
* Update responsively during **manual knob changes**.
* Update at a **rate-limited cadence** during **CV-driven modulation** to avoid UI/render overhead.

---

## Scope

### In scope

* Two independent waveform previews: **CH1 generator** and **CH4 generator**.
* Preview depicts the *current generator shape implied by Rise/Fall/Curve/Mode* (and any other generator parameters that affect shape).
* Rendered in the module panel UI using Rack widget drawing (NanoVG).

### Out of scope (explicitly not required for v1)

* Rendering incoming audio waveforms from inputs.
* Displaying slew-limited output shape from external input (this is generator-only preview).
* High-frequency oscilloscope fidelity, anti-alias perfection, persistence, graticule, or triggers.
* Multi-cycle scrolling.

---

## UI Placement & Layout

### Location

* Place the widget **directly beneath the Curve knob** in the generator portion for CH1 and CH4.
* Center-align under the Curve knob’s x-position.
* The widget should not overlap existing components; it should be sized to fit the available panel gap.

### Size

* Recommended bounding box: **~18–28 mm wide**, **~8–14 mm tall** (final in SVG/placement pass).
* The widget must accept a `Rect box` set by the panel layout code.

### Visual design

* Background: transparent (panel shows through) or a subtle dark rectangle if needed for contrast.
* Draw:

  * a center horizontal line at 0V (optional but recommended).
  * waveform polyline.
* Color: match module style (e.g., a dim off-white/gray line). Keep it consistent with existing LED/text palette.

---

## What the preview represents

### Domain mapping

The preview depicts a *normalized single-cycle representation* of the generator waveform, defined in normalized coordinates:

* **x ∈ [0, 1]** across widget width
* **y ∈ [-1, 1]** mapped to widget height (top = +1, bottom = -1)

### “Midpoint is the peak”

Interpretation requirement:

* The displayed waveform should have its **maximum magnitude at x = 0.5** (“midpoint peak”).
* This implies the preview is a stylized cycle where the apex occurs at the temporal midpoint, rather than necessarily matching a physical oscillator phase convention.

### Horizontal clipping when too slow

* The preview attempts to show **one cycle**.
* If the computed cycle length in pixels exceeds the widget width (i.e., time is “too slow”), the preview must **clip horizontally**:

  * Still render the center peak at x = 0.5.
  * Crop left/right portions that don’t fit.
* Concretely: compute waveform points over a virtual time window centered on the peak; draw only the portion that maps into widget x-range.

> Implementation note: treat the widget as a fixed window around the “peak moment” rather than scaling time to always fit.

---

## Waveform generation model (preview)

The preview is not required to be the *actual* output waveform sampled from DSP; it is acceptable (and preferred) to compute a **cheap deterministic curve** from current generator parameters.

### Inputs (per channel)

At minimum, the preview depends on:

* `riseTime` (effective)
* `fallTime` (effective)
* `curve` (effective)
* generator mode/shape toggles that alter response (e.g., lin/log/exp if applicable)
* whether channel is cycling / generator active (if that changes shape)

“Effective” means: include knob + CV + attenuverter + any clamping that the DSP uses, but do not require audio-rate sampling.

### Output

A vector of points `(x, y)` length `N` (default **64** points; adjustable). N must be small to keep CPU/GPU minimal.

### Suggested canonical preview function

Define a “cycle” as two segments meeting at x=0.5:

* Segment A: rising from y=-1 at x=0 to y=+1 at x=0.5
* Segment B: falling from y=+1 at x=0.5 to y=-1 at x=1

Each segment is shaped by `curve` using a monotonic warp:

* Let `t` be normalized segment time in [0,1].
* Use a curve mapping function `warp(t, curve)`:

  * `curve = 0` → linear: `warp(t)=t`
  * `curve > 0` → more exponential/log-like (choose a smooth mapping)
  * `curve < 0` → inverse curvature
* Then map:

  * Rising: `y = -1 + 2 * warp(t, curve)`
  * Falling: `y = +1 - 2 * warp(t, curveFall)`
* Optionally allow rise/fall to have different curvature if your DSP does.

#### Rise/Fall timing influence

Rise/Fall should affect **horizontal allocation**:

* Let `Tr` = effective rise time, `Tf` = effective fall time.
* Allocate fraction of the cycle:

  * `xr = Tr / (Tr + Tf)`
  * Peak location would normally be at `x = xr`
* But because the UI requirement says peak at midpoint (x=0.5), we do this instead:

  * Keep peak fixed at x=0.5 visually.
  * Use Tr/Tf to adjust **local slope density** (i.e., sample distribution or curvature intensity), OR to influence how much of the “cycle” is visible before clipping.
* Minimal viable approach:

  * Use Tr/Tf only to decide “too slow” clipping window size:

    * longer (Tr+Tf) → show less of the cycle (more clipping)
  * Keep the core curve shape stable.

This keeps the preview intuitive, stable, and cheap, while still reflecting “slow vs fast” via clipping.

---

## Rendering

### Drawing method

* Implement as a `Widget` subclass (e.g., `WavePreviewWidget`).
* Override `draw(const DrawArgs& args)` and use NanoVG:

  * Begin path
  * MoveTo first point
  * LineTo subsequent points
  * Stroke

### Coordinate mapping

* Convert normalized points to pixel coordinates inside `box`:

  * `px = x * box.size.x`
  * `py = (0.5 - 0.5*y) * box.size.y`  (y=+1 at top)

### Clipping

* Use NanoVG scissor:

  * `nvgScissor(vg, 0, 0, box.size.x, box.size.y)`
* Ensure the waveform does not draw outside its box.

---

## Update / Performance Policy

### Design goals

* **Knob interaction** feels immediate.
* **CV modulation** does not cause UI to redraw at audio/control rate.
* Avoid allocations in the hot path.

### Data flow

* The widget holds a cached polyline `std::array<Vec, N>` (or `std::vector<Vec>` pre-sized).
* The module updates a small “preview state” struct per channel when permitted.

### Rate limiting

Define two update pathways:

1. **Interactive pathway** (high priority)

* Triggered when the user drags:

  * rise knob
  * fall knob
  * curve knob
  * any generator mode toggle that affects shape
* Update preview at most **every 16 ms (~60 Hz)** while dragging.
* On drag end, force one final update.

2. **CV pathway** (low priority)

* Triggered by modulation changes (inputs, expander, automation, etc.)
* Update preview at most **every 66 ms (~15 Hz)** by default.
* Add a hard minimum of **33 ms (~30 Hz)** if you want it smoother; choose one and keep it constant.

### Detection strategy

Implementation options (choose one; either is acceptable):

* **A. UI-side time gate only**

  * In widget `step()`, check elapsed time and pull current effective params from module (thread-safe approach required).
* **B. Module-side dirty flag + time gate**

  * Module computes `effectiveParams` and sets `previewDirtyCH1/CH4`.
  * Widget polls these flags and only rebuilds polyline when dirty and time gate allows.

Recommended: **B**, because it prevents repeated recomputation when nothing has changed.

### Thread safety

* Rack UI and audio threads are separate.
* Do **not** read/write shared non-atomic data without protection.
* Use one of:

  * `dsp::RingBuffer`/`rack::dsp` messaging pattern,
  * atomics for small numeric state (floats via `std::atomic<float>` if supported/acceptable),
  * or copy a compact struct using atomics per field + version stamp.

#### Minimal safe pattern

Per channel maintain:

* `std::atomic<uint32_t> previewVersion`
* `std::atomic<float> effRise, effFall, effCurve`
* Widget caches last seen `previewVersion`; if changed and time gate allows, rebuild polyline.

Module increments `previewVersion` only when the effective preview params change meaningfully.

### Change threshold (to prevent churn)

When deciding “effective params changed”:

* `rise/fall`: update if relative change > ~1% OR absolute change > small epsilon (e.g., 1e-4 in normalized units).
* `curve`: update if abs delta > ~0.01 (tune based on curve range).

---

## Integration points

### Module

Add per channel:

* Effective preview params (atomics)
* Version counter (atomic)
* Optional: a `bool interactiveOverride` flag set by widget callbacks (or rely on widget time gate)

### Widget

* `WavePreviewWidget` constructed with:

  * pointer to module (may be null in browser)
  * channel index (1 or 4)
  * size box already set by layout code
* Implements:

  * `step()` to handle timing gates and rebuild polyline when needed
  * `draw()` to render cached polyline

### Module widget layout

* In `IntegralFluxWidget` (or equivalent):

  * `addChild(new WavePreviewWidget(...))` under Curve knob for CH1 and CH4.

---

## Testing & Acceptance Criteria

### Visual correctness

* With default parameters, preview shows a symmetric “up then down” cycle with peak at center.
* Turning Curve visibly changes shape (more convex/concave).
* Rise/Fall changes do not break continuity; preview stays stable and intuitive.
* When rise+fall become very slow, waveform visibly “zooms”/clips horizontally rather than shrinking to fit.

### Performance

* No heap allocations during continuous operation (verify by reviewing code paths).
* Under heavy CV modulation, preview updates capped to configured rate and does not spike CPU.
* No data races / TSAN-clean (or at least no obvious unsafe shared access).

### Edge cases

* Module absent (module browser): widget draws a default placeholder waveform.
* NaNs or invalid params: clamp to safe defaults before computing polyline.

---

## Notes / Future Extensions (non-blocking)

* Optional grid/centerline toggle.
* Indicate rise vs fall asymmetry via slightly different curvature per half.
* Add “cycle active” state (e.g., dim line when not cycling).
* If desired later: draw *actual* generator table sampled from DSP at low-rate.

---

If you want, I can also fold this into your existing “Integral Flux / Maths clone” codex spec style so the naming and parameter plumbing matches your current code conventions (e.g., how you compute effective rise/fall and curve right now).
