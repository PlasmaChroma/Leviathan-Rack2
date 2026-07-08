# VCV Rack UI Scaling Research Brief

Use this as a prompt/brief for ChatGPT or another research assistant. The goal is to understand the correct way to render dynamic/generated SVG label overlays in VCV Rack across UI scale, monitor DPI, and zoom levels without blurry text, incorrect size, or large performance regressions.

## Context

This repo is a VCV Rack plugin. Several modules use normal Rack SVG panels for their main panel art, and those panel SVGs behave correctly across tested UI scales. We recently split labels into separate generated SVG overlays, rendered through a shared helper:

- `src/visual/VisualAssets.hpp`
- `src/visual/VisualAssets.cpp`
- `visual_assets::createPanelLabelsWidget(...)`

Modules currently using the labels helper include:

- `src/IrisWidget.cpp`
- `src/BifurxUI.cpp`
- `src/WyrmWidget.cpp`
- `src/Proc.cpp`
- `src/IntegralFluxUI.cpp`
- `src/UndertowWidget.cpp`
- `src/TemporalDeckUI.cpp`

Current implementation shape:

- `FittedPanelSvgWidget` draws a separate labels SVG.
- It scales the labels SVG to the module `box.size`.
- It is wrapped in a `widget::FramebufferWidget`.
- `CachedPanelLabelsWidget::targetOversample()` currently changes framebuffer oversampling depending on `APP->window->pixelRatio`.
- At high pixel ratio, especially around 200% UI scale, oversampling had to be reduced to avoid scaling breakage.

## Symptoms Observed

1. At VCV Rack 200% UI scale, the generated labels SVG rendered much larger than the module panel.
   - Text appeared huge and offset.
   - Other Rack controls and panel graphics were correctly positioned.
   - The issue was isolated to the separate labels SVG overlay/framebuffer path.

2. 100% and 150% UI scale looked acceptable.
   - 200% and above showed the major scaling problem.

3. A workaround that avoided the 200% explosion caused a large performance regression.
   - Integral Flux render timing reportedly went from roughly `260us` to over `2300us`.
   - The likely culprit was rerendering or oversampling the labels/framebuffer too aggressively.

4. Disabling/reducing oversampling at high pixel ratio stabilized scale and performance, but zoomed-out text becomes blurry/mushy.
   - Current compromise appears visually acceptable at some scales, but not ideal when Rack zoom is reduced.

5. Text placed directly in the panel SVG behaves better.
   - Labels baked into the main Rack panel SVG get correct sizing and better scale/zoom behavior.
   - The separate generated labels overlay does not automatically inherit the same quality/scaling behavior.

6. There is concern about merging labels back into the main panel art.
   - If labels are merged into the main table/panel layer, they may be affected by module-specific glass/crystal/surface effects.
   - Separate label overlays avoid those effects but introduce framebuffer/DPI handling complexity.

## Current Hypotheses

Research these, do not assume they are true:

1. Rack’s normal panel SVG path may be rendered/cached using a specific framebuffer strategy that accounts for both UI scale and zoom differently than our custom `FramebufferWidget`.

2. Our labels overlay may be double-applying scale, pixel ratio, or oversample under 200% UI scale.

3. `FramebufferWidget::oversample`, `APP->window->pixelRatio`, and Rack zoom may interact nonlinearly.

4. The correct cache invalidation trigger may not be raw `pixelRatio` alone. It may need to consider the effective transform/zoom of the widget.

5. The separate SVG labels may need to be rendered at an effective backing resolution comparable to Rack’s panel SVG cache, but without causing per-frame redraws.

6. `dirtyOnSubpixelChange = false` may help performance/stability but could worsen text quality at certain zoom/subpixel positions.

## What We Need To Learn

Please research VCV Rack 2 widget rendering behavior, especially:

1. How does Rack render/caches module panel SVGs internally?
   - What classes are used?
   - How does Rack handle high-DPI displays?
   - How does Rack handle UI scale vs zoom?
   - Does Rack oversample panel SVGs, render them to a framebuffer, or redraw vector content directly?

2. What is the correct way for a plugin to render a static SVG overlay that:
   - matches module panel size exactly,
   - remains sharp when zoomed out,
   - behaves correctly at 100%, 150%, 200%, and higher UI scale,
   - does not rerender every frame,
   - does not cause a large CPU/GPU performance hit?

3. What exactly does `FramebufferWidget::oversample` mean in Rack 2?
   - Is it multiplied by `APP->window->pixelRatio`?
   - Is it multiplied by zoom or UI scale?
   - Does setting it dynamically require marking dirty?
   - Are there known bugs or edge cases at `pixelRatio >= 2`?

4. How can a plugin detect effective zoom/scale for a widget?
   - Is there a reliable transform API?
   - Is `APP->window->pixelRatio` enough?
   - Is there an equivalent to Rack’s panel cache scale selection?

5. Why would text baked into the main panel SVG look better than a separate labels SVG overlay?
   - Is it due to SVG rendering path?
   - Framebuffer filtering?
   - Mipmap/texture scaling?
   - Different cache resolution?
   - Different transform alignment?

6. Are there Rack examples or plugin examples that render separate high-quality static SVG label layers correctly?

## Current Code To Inspect

Start here:

```cpp
// src/visual/VisualAssets.cpp
struct FittedPanelSvgWidget final : TransparentWidget {
  std::shared_ptr<window::Svg> svg;

  void draw(const DrawArgs& args) override {
    const Vec svgSize = svg->getSize();
    nvgSave(args.vg);
    nvgScale(args.vg, box.size.x / svgSize.x, box.size.y / svgSize.y);
    svg->draw(args.vg);
    nvgRestore(args.vg);
  }
};

struct CachedPanelLabelsWidget final : Widget {
  widget::FramebufferWidget* fb = nullptr;
  float requestedOversample = 1.f;
  float cachedOversample = -1.f;

  float targetOversample() const {
    const float pixelRatio = (APP && APP->window) ? APP->window->pixelRatio : 1.f;
    if (pixelRatio >= 1.9f) {
      return 1.f;
    }
    return clamp(requestedOversample, 1.f, 1.5f);
  }
};
```

Also inspect Rack SDK headers/sources for:

- `widget::FramebufferWidget`
- `app::SvgPanel`
- `SvgWidget`
- panel rendering/caching code
- window pixel ratio handling
- zoom/scene transform handling

## Desired Output

Please produce:

1. A concise explanation of how Rack panel SVG rendering differs from a plugin-created `FramebufferWidget` SVG overlay.
2. A clear diagnosis of the likely cause of the 200% UI scale explosion.
3. A recommended implementation strategy for `createPanelLabelsWidget(...)`.
4. Any relevant code snippets or API references.
5. A performance-safe cache invalidation strategy.
6. A test matrix for validating:
   - UI scale: 100%, 150%, 200%, 250%
   - Rack zoom: common zoomed-in and zoomed-out values
   - normal DPI and high DPI displays
   - static module view and moving/scrolling rack

## Constraints

- Performance matters. The labels layer should be static/cached and not rerender every frame.
- Text quality matters, especially when zoomed out.
- Must not break 200% UI scale.
- Must work in Rack 2 plugin context.
- Avoid changing module parameter/control positioning.
- Avoid merging labels into the main panel if that causes labels to be affected by glass/crystal/surface effects.
