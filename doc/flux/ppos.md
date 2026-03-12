Yes, Dragon King Leviathan. Here’s a Codex-ready spec to switch your wave preview placement to use the SVG rectangles `CH1_PREVIEW` and `CH4_PREVIEW`, and to inset the drawable area by **0.2 mm on all sides** so the waveform fits cleanly inside.

---

# Codex Spec — Position Wave Preview Using SVG Boxes `CH1_PREVIEW` / `CH4_PREVIEW`

## Goal

Replace hardcoded/manual positioning of the waveform preview widgets with bounds derived from two SVG “layout boxes”:

* `CH1_PREVIEW`
* `CH4_PREVIEW`

The waveform must be drawn **inside** these rectangles, inset by **0.2 mm** on each side.

---

## Assumptions / Preconditions

1. The panel SVG contains two rectangular elements with ids `CH1_PREVIEW` and `CH4_PREVIEW`.
2. These elements live in the same coordinate system as other COMPONENTS ids (i.e., **no transforms** on COMPONENTS layer/group).
3. Existing knob/jack/light placement either already has a helper or a known method to fetch SVG element bounds.
4. Units are consistent such that **0.2 mm** can be converted using the same mm→px conversion used elsewhere (`mm2px()`).

---

## Required Changes

### 1) Add a helper to fetch an SVG element’s bounding box by id

Implement (or reuse) a function that returns a `Rect` in **Rack pixels** for an element id in the panel SVG.

**Function signature (suggested):**

* `static bool getSvgElementRectPx(std::shared_ptr<Svg> svg, const std::string& id, Rect* outRectPx);`

**Behavior:**

* Find the SVG element by `id`
* Read its bounding box (`x,y,w,h`) in SVG units
* Convert to Rack pixels (consistent with your panel’s scaling)
* Return `true` if found, else `false`

**Failure behavior:**

* If not found, fall back to the current hardcoded rect so the module still loads.

> Codex: if your codebase already has a helper for this (often used to place screws or custom overlays), use it instead of adding a new one.

---

### 2) Apply a 0.2 mm inset margin

Define:

* `const float PREVIEW_INSET_MM = 0.2f;`

Compute:

* `insetPx = mm2px(PREVIEW_INSET_MM);`

Given SVG rect in px `r`:

* `r.pos.x += insetPx`
* `r.pos.y += insetPx`
* `r.size.x -= 2 * insetPx`
* `r.size.y -= 2 * insetPx`

Clamp:

* `r.size.x = max(r.size.x, 1.f)`
* `r.size.y = max(r.size.y, 1.f)`

This inset rectangle is used for:

* `waveWidget->box = r`
* AND for waveform scissor/drawing bounds (widget-local scissor still required)

---

### 3) Update ModuleWidget construction to place previews via SVG ids

In your `IntegralFluxWidget` (or module widget constructor):

* Create two `WavePreviewWidget` instances:

  * `ch1Preview = new WavePreviewWidget(module, /*ch=*/1);`
  * `ch4Preview = new WavePreviewWidget(module, /*ch=*/4);`

* Fetch rects:

  * `Rect r1 = getSvgElementRectPx(panelSvg, "CH1_PREVIEW");`
  * `Rect r4 = getSvgElementRectPx(panelSvg, "CH4_PREVIEW");`

* Apply inset 0.2 mm to each.

* Set each widget’s `box` to its rect.

* Add as children.

**Important:** ensure `addChild()` ordering matches desired z-order:

* Panel background is behind
* Preview widget on top
* If you have a “bezel overlay widget”, add it *after* the preview widget.

---

### 4) Ensure WavePreviewWidget draws in local coords

Inside `WavePreviewWidget::draw()`:

* Do **not** use absolute positions

* Draw assuming local `(0..box.size.x, 0..box.size.y)`

* Always scissor to widget bounds:

* `nvgSave(args.vg);`

* `nvgScissor(args.vg, 0, 0, box.size.x, box.size.y);`

* draw waveform

* `nvgRestore(args.vg);`

This guarantees the waveform stays inside the inset rectangle.

---

## Acceptance Criteria

1. The CH1 waveform preview is positioned exactly within SVG rect `CH1_PREVIEW` minus **0.2 mm** padding on all sides.
2. The CH4 waveform preview is positioned exactly within SVG rect `CH4_PREVIEW` minus **0.2 mm** padding on all sides.
3. Moving/resizing `CH1_PREVIEW` / `CH4_PREVIEW` in the SVG updates the preview placement without any C++ coordinate changes.
4. Waveform never draws outside its box (verified with scissor).
5. If the SVG ids are missing, the module still loads using the prior fallback placement.

---

## Notes for Codex (Implementation Guidance)

* Prefer reusing your existing SVG id lookup utilities if present.
* Avoid adding transforms in the COMPONENTS group; bounding boxes must match coordinate assumptions.
* Keep the inset value in **mm** and convert once using `mm2px()` for consistency.

---

If you want, I can also tailor this to the exact helper you’re already using (e.g., if you have a `helper.py`-generated header/struct with component bounds). If Codex currently doesn’t have a way to access SVG element bounds at runtime, the “extend helper.py to export preview rects” path is the most deterministic.
