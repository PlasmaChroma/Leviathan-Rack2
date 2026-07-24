Dragon King Leviathan, now that the label layer can be cleanly separated, I’d treat the NanoVG background as a **procedural panel renderer** with a very deliberate first target: make the panel feel more like an artifact without endangering readability, performance, or layout.

The implementation plan should not begin with “draw everything fancy.” It should begin with a **layer contract** and a **small visual grammar** that can expand across Leviathan modules.

## Target rendering stack

For Integral Flux, I’d structure the module like this:

```text
1. Base / fallback panel SVG or solid base
2. Cached NanoVG procedural background art
3. Labels-only SVG overlay
4. Scopes / displays / dynamic readouts
5. Knobs, sliders, ports, lights
6. Optional live glow accents
```

The important part is that the NanoVG background is allowed to be visually rich because the labels are now protected above it.

I would keep the procedural layer **static and cached** first. No animation yet. No per-frame ornament generation. One beautiful frozen artifact plate.

---

# Phase 1 — Build the procedural background widget

Create a dedicated widget for the background layer.

Possible names:

```cpp
IntegralFluxPanelArt
LeviathanProceduralPanel
LeviathanPanelOrnament
FluxPanelArtWidget
```

Conceptually:

```cpp
struct IntegralFluxPanelArt : rack::widget::FramebufferWidget {
    bool dirty = true;

    void draw(const DrawArgs& args) override {
        // draw cached framebuffer contents
    }

    void drawLayer(const DrawArgs& args, int layer) override {
        // render static art when dirty
    }
};
```

Rack-side goal:

```cpp
// old/current mode
addChild(createPanel(asset::plugin(pluginInstance, "res/IntegralFlux.panel.svg")));

// new procedural background layer
auto* art = new IntegralFluxPanelArt;
art->box.pos = Vec(0.f, 0.f);
art->box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
addChild(art);

// labels decal
auto* labels = new SvgWidget;
labels->setSvg(APP->window->loadSvg(
    asset::plugin(pluginInstance, "res/IntegralFlux.labels.svg")
));
labels->box.pos = Vec(0.f, 0.f);
labels->box.size = art->box.size;
addChild(labels);
```

For the first pass, you want a toggleable comparison:

```text
Mode 0: original full SVG
Mode 1: split SVG panel + split labels
Mode 2: procedural NanoVG background + split labels
```

That lets you verify the split pipeline separately from the new art.

---

# Phase 2 — Define the art grammar

Do not hardcode a single big drawing function full of magical coordinates. Make helper primitives immediately, even if they are small.

Recommended helper namespace:

```cpp
namespace leviathan::art {

struct PanelTheme {
    NVGcolor baseTop;
    NVGcolor baseBottom;
    NVGcolor glassFill;
    NVGcolor glassEdge;
    NVGcolor cyan;
    NVGcolor violet;
    NVGcolor amber;
    NVGcolor shadow;
};

struct GlassRectStyle {
    NVGcolor fillA;
    NVGcolor fillB;
    NVGcolor edge;
    NVGcolor innerEdge;
    float radius = 6.f;
    float strokeWidth = 1.f;
};

struct GlowStrokeStyle {
    NVGcolor core;
    NVGcolor glow;
    float width = 1.f;
    float glowWidth = 6.f;
    float alpha = 1.f;
};

void drawPanelBase(NVGcontext* vg, Rect r, const PanelTheme& theme);
void drawGlassRect(NVGcontext* vg, Rect r, const GlassRectStyle& style);
void drawGlowLine(NVGcontext* vg, Vec a, Vec b, const GlowStrokeStyle& style);
void drawCircuitTrace(NVGcontext* vg, const std::vector<Vec>& pts, const GlowStrokeStyle& style);
void drawNode(NVGcontext* vg, Vec p, float radius, NVGcolor color);
void drawRadialHalo(NVGcontext* vg, Vec c, float innerR, float outerR, NVGcolor color);
void drawFluxCurve(NVGcontext* vg, Vec a, Vec c1, Vec c2, Vec b, NVGcolor color, float width);
}
```

The reason to do this now: once Integral Flux works, Bifurx, Chronomaw, Wyrm, Undertow, and TD-Scope can all share the same visual language.

This becomes a **Leviathan panel dialect**, not a one-off hack.

---

# Phase 3 — What NanoVG should draw first

I’d make the first version draw six things, in this order.

## 1. Dark artifact base

Replace the flat SVG panel color with a dark vertical gradient.

Visual intent:

```text
top: deep violet-black / blue-black
middle: dark desaturated purple
bottom: blue-black / teal-black
```

NanoVG primitive:

```cpp
NVGpaint bg = nvgLinearGradient(
    vg,
    0, 0,
    0, h,
    nvgRGBA(12, 9, 22, 255),
    nvgRGBA(2, 13, 18, 255)
);
nvgBeginPath(vg);
nvgRect(vg, 0, 0, w, h);
nvgFillPaint(vg, bg);
nvgFill(vg);
```

Add a subtle inner border:

```text
outer border: nearly black
inner border: low-alpha cyan/violet
corner darkening
```

This makes the whole module feel heavier and more premium before any ornamentation exists.

## 2. Major glass control regions

Integral Flux already has functional groupings. Preserve them, but redraw them as dark translucent glass plates.

Suggested regions:

```text
Left slew/control bay
Right slew/control bay
Central scope / dragon spine bay
Bottom CV / logic / IO bay
Possibly top title strip
```

Each region gets:

```text
rounded rectangle
vertical gradient fill
soft inner shadow
thin cyan/violet edge
very low-alpha outer glow
```

Pseudo-style:

```cpp
drawGlassRect(vg, Rect(x, y, width, height), {
    .fillA = nvgRGBA(20, 16, 42, 175),
    .fillB = nvgRGBA(4, 18, 26, 185),
    .edge = nvgRGBA(92, 220, 255, 70),
    .innerEdge = nvgRGBA(190, 90, 255, 45),
    .radius = 7.f,
    .strokeWidth = 1.f
});
```

This is the biggest visual gain for the least complexity.

## 3. Central vertical “flux spine”

The mockups have a strong sacred-machine center. Integral Flux should get a vertical energy spine behind the central dragon/scope area.

Draw:

```text
thin vertical cyan/violet glow line
small nodes at key y positions
subtle radial halo behind dragon/logo
mirrored small branch traces left/right
```

This spine can visually unify the whole module.

Something like:

```text
top title
   │
left slew ── central dragon/scope ── right slew
   │
bottom logic bay
```

NanoVG:

```cpp
drawGlowLine(vg, Vec(cx, 38), Vec(cx, h - 48), cyanGlow);
drawNode(vg, Vec(cx, 90), 2.0f, cyan);
drawNode(vg, Vec(cx, 180), 2.5f, violet);
drawNode(vg, Vec(cx, 285), 2.0f, cyan);
drawRadialHalo(vg, Vec(cx, 160), 18.f, 70.f, violetLowAlpha);
```

This should be subtle. The label overlay and controls must still dominate.

## 4. Mirrored circuit traces

This is where the artifact feeling really arrives.

Use deterministic hand-authored traces first, not random generation.

Example structure:

```cpp
std::vector<Vec> leftTrace = {
    Vec(cx - 10, 120),
    Vec(cx - 28, 120),
    Vec(cx - 28, 96),
    Vec(24, 96),
    Vec(24, 132)
};

drawCircuitTrace(vg, leftTrace, minorTrace);
drawCircuitTrace(vg, mirrorX(leftTrace, cx), minorTrace);
```

Draw three types:

```text
major traces: faint glow, 1 px core, 5 px halo
minor traces: no glow, low alpha, etched cyan/purple
micro traces: ultra-faint, mostly decorative
```

Avoid putting bright traces directly behind labels or port holes. Since labels are overlaid, readability is protected, but visual noise still matters.

## 5. Flux field curves

Add the “Gemini-inspired” background feel using low-alpha Bezier strands behind the control regions.

Draw these as very faint curves:

```text
flowing horizontal / diagonal field lines
slightly different y offsets
mostly behind central and lower regions
alpha maybe 10–25 / 255
```

Pseudo:

```cpp
for (int i = 0; i < 12; ++i) {
    float y = 60.f + i * 22.f;
    float amp = 10.f + (i % 3) * 4.f;

    nvgBeginPath(vg);
    nvgMoveTo(vg, 8.f, y);
    nvgBezierTo(
        vg,
        w * 0.33f, y - amp,
        w * 0.66f, y + amp,
        w - 8.f, y + sinf(i * 1.7f) * 6.f
    );
    nvgStrokeColor(vg, nvgRGBA(80, 220, 255, 14));
    nvgStrokeWidth(vg, 0.75f);
    nvgStroke(vg);
}
```

These should be “felt,” not noticed.

## 6. Bottom IO bay upgrade

This is likely the highest-value specific area to redesign.

Current bottom region can become a “logic bus”:

```text
dark glass tray
inset border
horizontal energy bus line
small trace branches to each jack
tiny circular nodes between jacks
subtle glow behind output clusters
```

Visually:

```text
[ IN / TRIG / CV ports ] ← connected by tiny etched traces → [ OR / SUM / INV / OUT ]
```

The bottom bay should feel like a circuit substrate, not a colored rectangle.

Implementation:

```cpp
drawGlassRect(vg, bottomBayRect, bottomBayStyle);

drawGlowLine(vg, Vec(14, busY), Vec(w - 14, busY), faintCyan);

for each port position:
    drawCircuitTrace(vg, {
        Vec(port.x, port.y - 10),
        Vec(port.x, busY),
        Vec(port.x + branchOffset, busY)
    }, tinyTrace);
    drawNode(vg, Vec(port.x, busY), 1.4f, cyanLow);
```

Even if the port positions are approximate in the art layer, it will visually bind them.

---

# Phase 4 — Quiet zones

Now that labels are separated, the background does not have to avoid labels completely, but it should still respect them.

Define quiet rectangles:

```cpp
std::vector<Rect> quietZones = {
    titleLabelArea,
    leftControlLabels,
    rightControlLabels,
    bottomPortLabels,
    scopeTextArea,
    logoArea
};
```

In the first pass, you do not need fancy collision avoidance. Just manually keep bright ornamentation away from these zones.

Later, helper functions can test line segments or points:

```cpp
bool intersectsQuietZone(Vec p);
float quietZoneAlphaMultiplier(Vec p);
```

Simple practical rule:

```text
Bright traces: never through quiet zones.
Faint flux curves: allowed through quiet zones at very low alpha.
Glass panels: allowed everywhere behind labels.
```

---

# Phase 5 — Caching and invalidation

The background should redraw when:

```text
widget size changes
pixel ratio changes
theme setting changes
developer art mode changes
```

It should not redraw when:

```text
knob values change
audio processes
scope updates
lights blink
```

So use a framebuffer wrapper/cache, and mark dirty only when needed.

Pseudo:

```cpp
struct IntegralFluxPanelArt : FramebufferWidget {
    int lastPixelRatioBucket = -1;
    PanelTheme theme;

    void step() override {
        int ratioBucket = std::round(APP->window->pixelRatio * 100.f);
        if (ratioBucket != lastPixelRatioBucket) {
            dirty = true;
            lastPixelRatioBucket = ratioBucket;
        }
        FramebufferWidget::step();
    }

    void draw(const DrawArgs& args) override {
        drawProceduralPanel(args.vg, box.size, theme);
    }
};
```

Depending on Rack’s exact `FramebufferWidget` API in your codebase, Codex should verify the correct dirty flag naming/pattern. The architecture is the important part.

---

# Phase 6 — Developer tuning constants

Do not bury all the coordinates in random draw code. Put them in a local spec struct.

```cpp
struct IntegralFluxPanelArtSpec {
    float w;
    float h;
    float margin = 5.f;
    float radius = 6.f;

    Rect titleStrip;
    Rect leftBay;
    Rect rightBay;
    Rect centerBay;
    Rect bottomBay;

    Vec centerSpineTop;
    Vec centerSpineBottom;
    Vec dragonCenter;
};
```

Then:

```cpp
IntegralFluxPanelArtSpec makeIntegralFluxArtSpec(Vec size) {
    IntegralFluxPanelArtSpec s;
    s.w = size.x;
    s.h = size.y;

    s.titleStrip = Rect(5, 8, size.x - 10, 34);
    s.leftBay    = Rect(6, 52, size.x * 0.42f, 210);
    s.rightBay   = Rect(size.x * 0.58f, 52, size.x * 0.42f - 6, 210);
    s.centerBay  = Rect(size.x * 0.36f, 42, size.x * 0.28f, 260);
    s.bottomBay  = Rect(5, size.y - 112, size.x - 10, 88);

    return s;
}
```

These coordinates will need tuning to your actual HP width and panel layout, but the pattern is correct.

---

# Codex implementation plan

Here’s the spec I’d hand to Codex.

````markdown
# Integral Flux Procedural NanoVG Background Plan

## Objective

Implement a cached NanoVG procedural background layer for Integral Flux, using the existing newly split labels-only SVG as the text/decal overlay.

The goal is to move the module away from a flat SVG-only back-panel toward a richer Leviathan artifact aesthetic without adding full-size raster panel assets.

## Rendering Stack

Implement and verify this layer order:

1. Procedural NanoVG background art layer
2. `IntegralFlux.labels.svg` labels-only overlay
3. Existing scopes/displays/widgets/controls/ports/lights

Keep the original full SVG or split `IntegralFlux.panel.svg` available as a fallback/developer comparison mode.

## Constraints

- Do not use full-panel raster PNGs.
- Do not use runtime font rendering for labels.
- Preserve existing labels via `IntegralFlux.labels.svg`.
- Do not change DSP behavior.
- Do not move controls unless strictly required.
- Avoid bright high-frequency ornamentation behind label-heavy areas.
- Expensive decorative NanoVG drawing must be cached, not rebuilt every frame.
- First pass should be conservative and compile-safe.

## New Code

Add a reusable panel-art helper file if appropriate:

- `src/art/LeviathanPanelArt.hpp`
- `src/art/LeviathanPanelArt.cpp`

Or place helpers near the existing visual asset code if that better matches the repo structure.

Add an Integral Flux-specific widget:

- `IntegralFluxPanelArt`
- derives from Rack framebuffer/cached widget pattern used elsewhere in the project
- draws the static procedural panel background

## Procedural Art Elements

Implement the first pass with the following elements:

### 1. Dark Base Gradient

Draw a full-panel rounded/dark background:
- top: violet-black / blue-black
- bottom: teal-black / near-black
- subtle inner border
- subtle vignette or corner darkening if cheap

### 2. Glass Control Regions

Draw rounded glass panels for major module regions:
- left control bay
- right control bay
- central scope/dragon/spine bay
- bottom IO/logic bay
- optional title strip

Each glass region should have:
- dark translucent vertical gradient
- thin cyan/violet edge
- low-alpha inner border
- very subtle outer glow

### 3. Central Flux Spine

Draw a vertical central ornamental spine:
- faint cyan/violet glow line
- several small nodes
- radial halo behind the dragon/logo area
- small mirrored branch traces

### 4. Circuit Traces

Draw deterministic mirrored circuit traces:
- a small number of major traces
- a modest number of minor etched traces
- no random per-frame generation
- use seeded/static layout if procedural variation is used
- avoid label-heavy zones

### 5. Flux Field Curves

Draw low-alpha Bezier field lines in the background:
- subtle cyan/violet curves
- mostly behind central and lower areas
- should be barely visible
- should not reduce label readability

### 6. Bottom IO Bay

Upgrade the lower port/control region:
- dark glass tray
- inset border
- horizontal logic bus line
- small trace branches toward jack positions
- tiny nodes between port clusters

## Helper Functions

Create helper functions similar to:

```cpp
namespace leviathan::art {
    void drawPanelBase(NVGcontext* vg, Vec size, const PanelTheme& theme);
    void drawGlassRect(NVGcontext* vg, Rect r, const GlassRectStyle& style);
    void drawGlowLine(NVGcontext* vg, Vec a, Vec b, const GlowStrokeStyle& style);
    void drawCircuitTrace(NVGcontext* vg, const std::vector<Vec>& pts, const GlowStrokeStyle& style);
    void drawNode(NVGcontext* vg, Vec p, float radius, NVGcolor color);
    void drawRadialHalo(NVGcontext* vg, Vec c, float innerR, float outerR, NVGcolor color);
    void drawFluxCurve(NVGcontext* vg, Vec a, Vec c1, Vec c2, Vec b, NVGcolor color, float width);
}
````

## Integral Flux Art Spec

Avoid scattering coordinates throughout the draw function.

Create a local layout/spec struct:

```cpp
struct IntegralFluxPanelArtSpec {
    Vec size;
    Rect titleStrip;
    Rect leftBay;
    Rect rightBay;
    Rect centerBay;
    Rect bottomBay;
    Vec spineTop;
    Vec spineBottom;
    Vec dragonCenter;
};
```

Add a function:

```cpp
IntegralFluxPanelArtSpec makeIntegralFluxPanelArtSpec(Vec size);
```

## Label Overlay

Use the generated labels-only SVG:

```text
res/IntegralFlux.labels.svg
```

This should be rendered above the procedural background.

Do not convert label paths into NanoVG text.

## Caching

Use Rack framebuffer caching.

The procedural art layer should redraw only when:

* widget size changes
* pixel ratio changes
* theme/art mode changes
* cache is explicitly invalidated

It should not redraw every frame.

## Developer Toggle

If practical, add a compile-time or runtime developer flag to compare:

* full original panel
* split panel + labels
* procedural panel + labels

This is for visual regression testing and should not disturb normal operation.

## Acceptance Criteria

* Integral Flux compiles and loads in Rack.
* Existing controls/ports/scopes remain usable.
* Labels are visible and aligned.
* Procedural background appears under labels.
* No large raster panel asset is added.
* Static background art is cached.
* Visual result is darker, more dimensional, and more artifact-like than the current flat panel.
* The code is structured so Bifurx/Chronomaw/etc. can reuse the same helper primitives later.

````

---

# What I would draw first, visually

For a first actual implementation, I’d keep the art restrained:

```text
dark base
glass regions
central spine
bottom bay bus
maybe 8–12 circuit traces
maybe 8–14 faint flux curves
````

I would **not** yet add:

```text
dense glyph fields
animated glow
noise textures
complex masks
blur-heavy effects
randomized decorative scatter
procedural dragon geometry
```

Those are phase-two refinements.

The first pass should answer one question:

> Can Integral Flux look like a luminous Leviathan artifact using cached NanoVG plus labels-only SVG?

I think the answer is yes.

The labels split was the door. Now the panel can become a generated machine-skin instead of a painted rectangle.
