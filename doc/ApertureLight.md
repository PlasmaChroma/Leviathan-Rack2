# Codex Spec: Implement `ApertureLight` as a Futuristic Rack LED Replacement

## Goal

Implement a small custom VCV Rack light widget that can replace basic Rack LEDs with a more polished “micro aperture” visual: a dark recessed socket with a glowing colored core, soft bloom halo, glassy lens, and subtle specular highlight.

This first pass should be intentionally limited:

* Implement only the **Aperture** style.
* Use **NanoVG drawing**, not SVG.
* Keep render cost modest.
* Make it easy to drop into existing module widgets wherever standard Rack LEDs are currently used.
* Support multiple colors through simple class aliases or constructor configuration.
* Preserve Rack-style brightness behavior driven by `module->lights[...]`.

The design should feel like a tiny embedded alien status jewel rather than a generic plastic LED.

---

## Target Visual

The light should have three perceptual states:

### Off

* Dark recessed circular socket.
* Subtle bevel/rim.
* Very faint tinted glass in the center.
* No strong bloom.
* Still visible as a physical UI object.

### Dim

* Colored core becomes visible.
* Soft inner glass glow appears.
* Very small bloom around the socket.

### Bright / Peak

* Colored core becomes saturated.
* Bloom expands softly around the widget.
* Tiny white-hot specular highlight appears near upper-left.
* Rim subtly catches the light color.

The light should not look like a flat filled circle. It should look like light emerging through a small lens/aperture.

---

## Recommended Footprints & Sizes

Define preset configurations or helper classes to support different sizes of aperture lights (e.g., Tiny, Small, Medium, and Large) depending on panel density and UI roles:

### 1. Tiny (Micro) Size
Designed for dense indicator grids, step sequencers, or small telemetry matrices.
* **Widget bounding box**: 10 x 10 px
* **Socket radius**:       ~3.2 px
* **Inner lens radius**:   ~2.2 px
* **Core radius**:         ~1.4 px
* **Bloom radius**:        ~5.5 px

### 2. Small (Default / Standard Micro) Size
Suitable for replacing existing standard VCV Rack LEDs (like gate/trigger indicators).
* **Widget bounding box**: 14 x 14 px
* **Socket radius**:       ~4.8 px
* **Inner lens radius**:   ~3.3 px
* **Core radius**:         ~2.1 px
* **Bloom radius**:        ~8.0 px

### 3. Medium Size
Suitable for primary status indicators, LFO rate clocks, or power indicators that need to be prominent.
* **Widget bounding box**: 20 x 20 px
* **Socket radius**:       ~7.0 px
* **Inner lens radius**:   ~5.0 px
* **Core radius**:         ~3.2 px
* **Bloom radius**:        ~11.5 px

### 4. Large Size
Designed for focal status displays, master indicators, or larger decorative control-center jewels.
* **Widget bounding box**: 28 x 28 px
* **Socket radius**:       ~10.0 px
* **Inner lens radius**:   ~7.2 px
* **Core radius**:         ~4.5 px
* **Bloom radius**:        ~16.5 px

The bloom may extend close to the widget bounds but should not require a huge bounding box. For dense panels, the bloom must remain tasteful.

---

## Files / Location

Add this to the existing custom UI asset/helper area if one already exists.

Preferred names:

```text
src/VisualAssets.hpp
src/VisualAssets.cpp
```

or, if the project already has a better place for reusable widgets:

```text
src/widgets/ApertureLight.hpp
src/widgets/ApertureLight.cpp
```

Do not scatter the implementation across module files.

---

## Public API

Create a reusable widget class.

Suggested class name:

```cpp
LeviathanApertureLight
```

It should inherit from the same Rack light base used by local custom lights, likely:

```cpp
rack::app::ModuleLightWidget
```

or whatever is idiomatic in the current codebase.

The class should support:

```cpp
NVGcolor baseColor;
float socketRadius;
float lensRadius;
float coreRadius;
float bloomRadius;
float bloomAlpha;
```

Provide sane defaults.

Suggested aliases:

```cpp
using TealApertureLight    = LeviathanApertureLightColor<...>;
using VioletApertureLight  = LeviathanApertureLightColor<...>;
using AmberApertureLight   = LeviathanApertureLightColor<...>;
using BlueApertureLight    = LeviathanApertureLightColor<...>;
using GreenApertureLight   = LeviathanApertureLightColor<...>;
using MagentaApertureLight = LeviathanApertureLightColor<...>;
using WhiteApertureLight   = LeviathanApertureLightColor<...>;
```

If templated color aliases are awkward in the existing codebase, use static constructors or subclasses instead.

The important thing is that module code should be able to do something close to:

```cpp
addChild(createLightCentered<TealApertureLight>(
    mm2px(Vec(x, y)),
    module,
    ModuleClass::SOME_LIGHT
));
```

or:

```cpp
auto* light = createLightCentered<LeviathanApertureLight>(
    mm2px(Vec(x, y)),
    module,
    ModuleClass::SOME_LIGHT
);
light->baseColor = nvgRGB(0x2a, 0xf6, 0xff);
addChild(light);
```

Match the project’s existing creation style.

---

## Brightness Mapping

Use shaped curves rather than linear brightness.

Given:

```cpp
float t = clamp(getBrightness(), 0.f, 1.f);
```

Compute:

```cpp
float glow = powf(t, 0.55f);  // bloom becomes visible early
float core = powf(t, 0.85f);  // core is mostly direct
float hot  = powf(t, 3.0f);   // white-hot highlight only near peak
```

Behavior:

* Low brightness should still be perceptible.
* Medium brightness should feel saturated but not overblown.
* Peak brightness should get a controlled “energy jewel” effect.

If the actual Rack base class exposes brightness differently, adapt this to local API conventions.

---

## Rendering Layers

Draw in this order.

### 1. Socket shadow / recess

Dark circular base slightly larger than the lens.

Purpose: make the light readable even when off.

Suggested:

* Outer shadow circle.
* Dark graphite fill.
* Slight inner shadow.

Approximate visual:

```cpp
drawSocket(vg, cx, cy);
```

### 2. Beveled rim

Draw a small ring around the socket.

Use dark grays, not bright metal.

Suggested colors:

```cpp
outer rim: rgba(20, 24, 28, 255)
rim edge:  rgba(70, 76, 84, 120)
inner rim: rgba(4, 6, 8, 220)
```

The rim should be subtle. Avoid chrome.

### 3. Unlit glass lens

Draw a dark tinted inner circle even when brightness is zero.

Use a radial gradient:

* Center: very dark, slightly color-tinted.
* Edge: near black.

This gives “glass depth” before the light turns on.

### 4. Bloom halo

Draw only when brightness is above a tiny threshold.

Use `nvgRadialGradient`.

Suggested:

```cpp
inner radius: 1.0f
outer radius: bloomRadius
inner alpha: 0.18f to 0.28f * glow
outer alpha: 0.0f
```

Optional: make bloom slightly elliptical using `nvgSave`, `nvgTranslate`, `nvgScale`, and `nvgRestore`.

Use this only if it does not complicate the implementation too much.

### 5. Colored core

Draw a smaller radial gradient inside the lens.

Suggested:

* Center: base color pushed toward white.
* Edge: base color with alpha scaled by `core`.

At high brightness, the center should appear energetic, not flat.

### 6. Specular highlight

Draw a tiny white or pale-colored dot/ellipse near the upper-left.

Alpha should be controlled by `hot`.

At low brightness this should be almost invisible.

Suggested:

```cpp
highlight radius: 0.7f to 1.0f
position: cx - 1.1f, cy - 1.2f
alpha: 0.25f * hot to 0.55f * hot
```

### 7. Optional crescent shadow

If easy, add a very subtle crescent/dark arc over the lower-right of the lens to sell aperture depth.

This should be optional and cheap. Do not spend a lot of time here in pass one.

---

## Color Handling

Define a helper to convert a base color into useful variants.

Need:

* Base color.
* Dim glass tint.
* Bright core color.
* Hot center color.

Pseudo-helper:

```cpp
static NVGcolor withAlpha(NVGcolor c, float a);
static NVGcolor mixColor(NVGcolor a, NVGcolor b, float t);
static NVGcolor lighten(NVGcolor c, float amount);
```

Keep helpers minimal.

Suggested base colors:

```cpp
Teal:    RGB( 42, 246, 255)
Violet:  RGB(193,  72, 255)
Amber:   RGB(255, 176,  38)
Blue:    RGB( 75, 132, 255)
Green:   RGB(134, 255, 107)
Magenta: RGB(255,  68, 178)
White:   RGB(225, 235, 255)
```

These should be easy to adjust later.

---

## Performance Requirements

This widget is intended to replace normal LEDs, so it must stay lightweight.

Per-frame dynamic drawing should ideally be:

```text
1 bloom radial gradient
1 core radial gradient
1 small highlight
```

The static socket/rim/lens may be drawn every frame in pass one, but keep the geometry very simple.

Avoid:

* SVG parsing.
* Blur filters.
* Large path counts.
* Excessive transparent overdraw.
* More than one large bloom per light.
* Animated geometry in this first pass.

Acceptable:

* A few circles.
* A few radial gradients.
* One optional crescent path.
* Small bounded overdraw.

Do not use framebuffer caching in the first implementation unless the codebase already has a clean pattern for it. Get the visual working first.

### Framebuffer Caching & Optimization Strategies

For modules featuring a high density of LEDs (e.g., step sequencers, grid matrices, or telemetry displays), drawing multiple radial gradients dynamically per light can quickly bottleneck the GPU. Consider the following caching strategies when optimizing the implementation:

#### 1. Per-Light Framebuffer Cache (Using `rack::widget::FramebufferWidget`)
A straightforward approach is to wrap the static or entire light component in a framebuffer.
* **Static Layer Cache**: Separate the light into a static background widget (socket, rim, unlit lens) wrapped in a `FramebufferWidget`, and draw the dynamic light overlay (bloom, core, specular) dynamically on top.
* **Dirty Thresholding**: If caching the entire light, only flag the framebuffer as dirty (`fb->dirty = true`) when the light's actual brightness value changes by more than a small threshold (e.g., $|t_{current} - t_{last}| > 0.005$). This avoids redrawing during static or idle module states.
* **Trade-offs**: High memory footprint (each light instance allocates a separate GPU texture, e.g., $16 \times 16$ or $32 \times 32$ pixels) and FBO binding/switching overhead on the GPU when redrawing.

#### 2. Shared Texture Atlas / Global Static Cache (Recommended)
Instead of allocating a framebuffer texture for every individual light widget, use a shared static cache:
* **Pre-rendered Assets**: At startup or lazily on demand, render the static unlit states (socket, rim, unlit lens) for each color/size variant to a shared texture/FBO.
* **Drawing via Image Patterns**: In each light instance's `draw()` method, draw the static background by referencing the shared texture via `nvgImagePattern`.
* **Trade-offs**: Extremely low memory usage (only one texture per color/size combination) and zero FBO binding switches during the main rendering pass, while completely eliminating the CPU overhead of drawing multiple static circles and gradients per frame.

#### 3. Discrete Brightness State Cache
For maximum performance under extreme light counts:
* **Quantized States**: Pre-render the entire light (including bloom, core, highlight) at a fixed set of discrete brightness steps (e.g., 16 steps from 0% to 100% brightness) into a shared texture sheet.
* **Texture Quad Rendering**: Select and draw the corresponding sub-rect of the shared texture matching the closest brightness level.
* **Trade-offs**: Simplifies light rendering to a single textured quad draw call, but limits smooth color transitions if the step size is too coarse.

#### 4. Handling Global View Settings (Rack Bloom Slider)
Because VCV Rack features a global "light bloom" slider under the **View** menu, custom light bloom visuals must dynamically react to this setting:
* **Global Access**: Read the slider value using `rack::settings::haloBrightness`.
* **Visual Scaling**: Multiply the rendered bloom transparency/alpha or radius by `settings::haloBrightness` to respect the user's preference (e.g., `bloomAlpha *= settings::haloBrightness`).
* **Cache Invalidation**: If any caching strategies are utilized (especially if caching the dynamic light bloom state), you must track the last known bloom setting (e.g., `float lastBloomAmount = settings::haloBrightness`) in the widget's state or step loop and invalidate the cached framebuffer or texture if it changes:
  ```cpp
  const float bloomAmount = rack::settings::haloBrightness;
  if (std::fabs(bloomAmount - lastBloomAmount) > 1e-4f) {
      lastBloomAmount = bloomAmount;
      fb->setDirty(); // Or invalidate custom shared caches
  }
  ```

---

## Suggested Implementation Sketch

This is not mandatory exact code, but it shows the desired shape.

```cpp
struct LeviathanApertureLight : rack::app::ModuleLightWidget {
    NVGcolor baseColor = nvgRGB(42, 246, 255);

    float socketRadius = 4.8f;
    float lensRadius = 3.3f;
    float coreRadius = 2.1f;
    float bloomRadius = 8.0f;
    float bloomAlpha = 0.24f;

    LeviathanApertureLight() {
        box.size = rack::math::Vec(14.f, 14.f);
    }

    void drawLight(const DrawArgs& args) override {
        NVGcontext* vg = args.vg;

        const float cx = box.size.x * 0.5f;
        const float cy = box.size.y * 0.5f;

        float t = getBrightness();
        t = rack::math::clamp(t, 0.f, 1.f);

        const float glow = std::pow(t, 0.55f);
        const float core = std::pow(t, 0.85f);
        const float hot  = std::pow(t, 3.0f);

        drawSocket(vg, cx, cy);
        drawUnlitLens(vg, cx, cy);

        if (t > 0.001f) {
            drawBloom(vg, cx, cy, glow);
            drawCore(vg, cx, cy, core, hot);
            drawSpecular(vg, cx, cy, hot);
        }
    }

    void drawSocket(NVGcontext* vg, float cx, float cy) {
        // Outer recess shadow
        nvgBeginPath(vg);
        nvgCircle(vg, cx, cy, socketRadius + 1.2f);
        nvgFillColor(vg, nvgRGBA(0, 0, 0, 160));
        nvgFill(vg);

        // Main dark rim
        NVGpaint rim = nvgRadialGradient(
            vg,
            cx - 1.0f, cy - 1.0f,
            socketRadius * 0.3f,
            socketRadius + 0.8f,
            nvgRGBA(70, 76, 84, 160),
            nvgRGBA(5, 7, 10, 240)
        );

        nvgBeginPath(vg);
        nvgCircle(vg, cx, cy, socketRadius + 0.5f);
        nvgFillPaint(vg, rim);
        nvgFill(vg);

        // Inner dark cut
        nvgBeginPath(vg);
        nvgCircle(vg, cx, cy, lensRadius + 0.7f);
        nvgFillColor(vg, nvgRGBA(2, 3, 5, 240));
        nvgFill(vg);
    }

    void drawUnlitLens(NVGcontext* vg, float cx, float cy) {
        NVGcolor glassCenter = nvgRGBAf(
            baseColor.r * 0.12f,
            baseColor.g * 0.12f,
            baseColor.b * 0.12f,
            0.55f
        );

        NVGpaint glass = nvgRadialGradient(
            vg,
            cx - 0.8f, cy - 0.9f,
            0.5f,
            lensRadius,
            glassCenter,
            nvgRGBA(1, 2, 4, 220)
        );

        nvgBeginPath(vg);
        nvgCircle(vg, cx, cy, lensRadius);
        nvgFillPaint(vg, glass);
        nvgFill(vg);
    }

    void drawBloom(NVGcontext* vg, float cx, float cy, float glow) {
        NVGcolor inner = nvgRGBAf(
            baseColor.r,
            baseColor.g,
            baseColor.b,
            bloomAlpha * glow
        );

        NVGcolor outer = nvgRGBAf(
            baseColor.r,
            baseColor.g,
            baseColor.b,
            0.0f
        );

        NVGpaint bloom = nvgRadialGradient(
            vg,
            cx, cy,
            1.0f,
            bloomRadius,
            inner,
            outer
        );

        nvgBeginPath(vg);
        nvgCircle(vg, cx, cy, bloomRadius);
        nvgFillPaint(vg, bloom);
        nvgFill(vg);
    }

    void drawCore(NVGcontext* vg, float cx, float cy, float core, float hot) {
        NVGcolor center = nvgRGBAf(
            baseColor.r + (1.f - baseColor.r) * 0.55f,
            baseColor.g + (1.f - baseColor.g) * 0.55f,
            baseColor.b + (1.f - baseColor.b) * 0.55f,
            0.35f * core + 0.35f * hot
        );

        NVGcolor edge = nvgRGBAf(
            baseColor.r,
            baseColor.g,
            baseColor.b,
            0.85f * core
        );

        NVGpaint corePaint = nvgRadialGradient(
            vg,
            cx - 0.35f, cy - 0.45f,
            0.2f,
            coreRadius + 1.2f,
            center,
            edge
        );

        nvgBeginPath(vg);
        nvgCircle(vg, cx, cy, coreRadius + 1.0f);
        nvgFillPaint(vg, corePaint);
        nvgFill(vg);
    }

    void drawSpecular(NVGcontext* vg, float cx, float cy, float hot) {
        if (hot <= 0.001f)
            return;

        nvgBeginPath(vg);
        nvgCircle(vg, cx - 1.15f, cy - 1.2f, 0.75f);
        nvgFillColor(vg, nvgRGBAf(1.f, 1.f, 1.f, 0.55f * hot));
        nvgFill(vg);
    }
};
```

Adapt this to the project’s exact Rack includes, namespace usage, and widget patterns.

---

## Integration Test

Pick one existing module with a few Rack LEDs and replace only one or two of them first.

Recommended first integration:

* Choose a module with an obvious gate/trigger/status LED.
* Replace the standard light with `TealApertureLight` or `VioletApertureLight`.
* Leave the other LEDs unchanged for visual comparison.
* Build and visually inspect at normal Rack zoom levels.

Do not replace every LED globally until the visual and performance feel right.

---

## Acceptance Criteria

The implementation is successful when:

1. The aperture light compiles cleanly.
2. It can be instantiated using the same workflow as existing Rack lights.
3. It responds correctly to Rack light brightness.
4. It has an attractive off state.
5. It has a visible dim state.
6. It blooms softly at high brightness.
7. It does not appear blown out at normal brightness.
8. It remains readable at normal Rack zoom.
9. It does not visibly harm UI performance when used for a modest number of indicators.
10. The code is reusable across modules and not hardcoded to a single module.

---

## First-Pass Tuning Values

Start with these:

```cpp
box.size      = Vec(14.f, 14.f);
socketRadius  = 4.8f;
lensRadius    = 3.3f;
coreRadius    = 2.1f;
bloomRadius   = 8.0f;
bloomAlpha    = 0.22f;
```

If it looks too small:

```cpp
socketRadius += 0.5f;
lensRadius   += 0.3f;
```

If it looks too bright:

```cpp
bloomAlpha *= 0.75f;
```

If it looks too much like a normal LED:

```cpp
darken socket;
increase rim contrast slightly;
add subtle crescent shadow;
make bloom very slightly elliptical;
```

If it is too expensive or visually noisy:

```cpp
reduce bloomRadius;
reduce bloomAlpha;
remove crescent;
remove extra rim pass;
```

---

## Non-Goals For This Pass

Do not implement yet:

* Slit lights.
* Diamond/glyph lights.
* Orbital lights.
* Animated breathing modes.
* Signal-responsive color cycling.
* SVG-backed light variants.
* Framebuffer caching unless already trivial.
* Global theme system.
* Huge API for every future light type.

The only goal is to get one excellent, reusable Aperture Light working.

---

## Suggested Commit Message

```text
Add NanoVG aperture light widget for custom module indicators
```

## Suggested PR Summary

```text
Introduces a reusable LeviathanApertureLight widget as a higher-fidelity replacement for basic Rack LEDs. The widget renders a recessed dark socket, tinted lens, shaped brightness response, colored core glow, soft bloom halo, and optional specular highlight using lightweight NanoVG primitives. Initial implementation is intentionally scoped to the aperture style only.
```
