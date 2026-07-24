# UI Scaling Analysis & Solution

## Root Cause Diagnosis

After reading the complete Rack 2 source for `FramebufferWidget`, `SvgPanel`, and `SvgWidget`, the problem is now fully understood.

### How Rack's SvgPanel Works (The Reference)

[SvgPanel](file:///mnt/c/msys64/home/Plasm/Rack-SDK/include/app/SvgPanel.hpp) does this:

```cpp
SvgPanel::SvgPanel() {
    fb = new FramebufferWidget;         // Creates FB with default oversample=1.0
    sw = new SvgWidget;                  // SvgWidget draws at SVG's native size
    fb->addChild(sw);
}

void SvgPanel::step() {
    if (APP->window->pixelRatio < 2.0)
        fb->oversample = 2.0;           // Oversample on low-DPI
    else
        fb->oversample = 1.0;           // Native on high-DPI
    Widget::step();
}

void SvgPanel::setBackground(std::shared_ptr<window::Svg> svg) {
    sw->setSvg(svg);                     // setSvg calls wrap(), which sets box.size = svg->getSize()
    fb->box.size = sw->box.size...;      // FB sized to SVG native size
    box.size = fb->box.size;
}
```

**Critical detail**: `SvgWidget::draw()` does **no scaling** — it just calls `svgDraw(args.vg, svg->handle)` which renders the SVG at its native document size. The `SvgWidget::box.size` is set to the SVG's native size via `wrap()`. The `FramebufferWidget` box is also set to the SVG's native size. Everything matches — **no manual nvgScale is applied**.

### How Our Labels Overlay Works (The Problem)

```cpp
FittedPanelSvgWidget::draw() {
    nvgScale(args.vg, box.size.x / svgSize.x, box.size.y / svgSize.y);  // ⚠️ EXTRA SCALE
    svg->draw(args.vg);
}
```

Our `FittedPanelSvgWidget` applies an **explicit `nvgScale`** to map the SVG's native size to `panelSizePx`. This is because the labels SVG may have a different document size than the panel (it's authored as a separate SVG).

### The Double-Scale Problem at 200% UI Scale

Inside `FramebufferWidget::render()`, here's what happens:

```cpp
void FramebufferWidget::render(math::Vec scale, ...) {
    // scale = the world transform extracted from nvgCurrentTransform
    // At 200% UI scale, scale ≈ (2.0, 2.0)
    
    internal->fbScale = scale;
    
    // FB size calculation:
    float pixelRatio = std::fmax(1.f, std::floor(APP->window->pixelRatio));
    math::Vec newFbSize = internal->fbBox.size.mult(pixelRatio).ceil();
    // At 200%: fbBox is in world coords (already scaled by 2x), then multiplied by pixelRatio (2.0)
    // Result: FB is allocated at 4x the logical widget size
}

void FramebufferWidget::drawFramebuffer() {
    float pixelRatio = internal->fbSize.x * oversample / internal->fbBox.size.x;
    nvgBeginFrame(vg, internal->fbBox.size.x, internal->fbBox.size.y, pixelRatio);
    
    // Then applies the world scale:
    nvgScale(vg, internal->fbScale.x, internal->fbScale.y);
    
    // Then Widget::draw(args) is called, which calls children's draw()
    // For SvgWidget: just draws at native size → the fbScale handles screen scaling ✅
    // For FittedPanelSvgWidget: applies ANOTHER nvgScale on top → DOUBLE SCALE ❌
}
```

**The root cause**: `FramebufferWidget::drawFramebuffer()` already applies `internal->fbScale` (which includes the world transform / UI scale). When our `FittedPanelSvgWidget` then applies its **own** `nvgScale(box.size / svgSize)`, these scales **multiply**. At 100% UI scale this works because `fbScale ≈ 1.0`. At 200%, `fbScale ≈ 2.0`, so the SVG gets rendered at `2.0 × (box.size/svgSize)` — roughly double its intended size.

### Why the 200% Workaround Killed Performance

The workaround of reducing oversample at high pixel ratio (`return 1.f when pixelRatio >= 1.9`) was a band-aid. The actual problem is the double-scale, not the oversample value. The performance regression came from the initial attempts that likely triggered constant re-rendering or allocated massive framebuffers.

---

## The Fix: Match SvgPanel's Pattern

The solution is simple and elegant: **don't apply manual nvgScale**. Instead, match Rack's own `SvgPanel` pattern:

1. Size the `SvgWidget` (or equivalent) and the `FramebufferWidget` to the **SVG's native document size**
2. Let the `FramebufferWidget` handle all world-transform scaling automatically
3. Use `SvgPanel`'s exact same oversample strategy

### Why This Works

When the module calls `setPanel()`, the panel SVG's document size becomes the module's `box.size`. Since `SvgWidget::draw()` renders at native SVG size and the FB handles world scale, everything aligns automatically.

Our labels SVG has the **same document dimensions** as the panel SVG (they're authored as overlay pairs). So if we draw the labels SVG at its native size (no manual scale), it will already match the panel.

If the labels SVG has a **different** document size from the panel, we need a one-time size correction — but it should be applied to `box.size` and `fb->box.size`, NOT via `nvgScale` inside `draw()`.

---

## Recommended Implementation

### Option A: Direct SvgWidget Pattern (Recommended)

Replace `FittedPanelSvgWidget` with standard `SvgWidget` behavior:

```cpp
struct CachedPanelLabelsWidget final : Widget {
    widget::FramebufferWidget* fb = nullptr;

    CachedPanelLabelsWidget(const char* svgPath, Vec panelSizePx) {
        box.size = panelSizePx;

        fb = new widget::FramebufferWidget();
        fb->dirtyOnSubpixelChange = false;

        // Use a standard SvgWidget — NO manual nvgScale
        auto* sw = new widget::SvgWidget();
        sw->setSvg(loadPluginSvgCached(svgPath));
        // setSvg calls wrap() which sets sw->box.size = svg->getSize()
        
        fb->box.size = sw->box.size;  // FB sized to SVG native size
        fb->addChild(sw);
        addChild(fb);
    }

    void step() override {
        if (fb) {
            // Match Rack's SvgPanel strategy exactly
            const float pixelRatio = (APP && APP->window) ? APP->window->pixelRatio : 1.f;
            const float target = (pixelRatio < 2.f) ? 2.f : 1.f;
            fb->oversample = target;
        }
        Widget::step();
    }
};
```

> [!IMPORTANT]
> This assumes the labels SVG document size matches the panel SVG document size. If it doesn't, see Option B.

### Option B: If Labels SVG Has Different Document Size

If the labels SVGs are authored at a different scale than the panel SVGs, we need to set the widget's transform at the `CachedPanelLabelsWidget` level, **outside** the FramebufferWidget:

```cpp
struct CachedPanelLabelsWidget final : Widget {
    widget::FramebufferWidget* fb = nullptr;
    math::Vec svgNativeSize;

    CachedPanelLabelsWidget(const char* svgPath, Vec panelSizePx) {
        box.size = panelSizePx;

        fb = new widget::FramebufferWidget();
        fb->dirtyOnSubpixelChange = false;

        auto* sw = new widget::SvgWidget();
        sw->setSvg(loadPluginSvgCached(svgPath));
        svgNativeSize = sw->box.size;

        fb->box.size = svgNativeSize;  // FB at SVG native size
        fb->addChild(sw);
        addChild(fb);
    }

    void step() override {
        if (fb) {
            const float pixelRatio = (APP && APP->window) ? APP->window->pixelRatio : 1.f;
            fb->oversample = (pixelRatio < 2.f) ? 2.f : 1.f;
        }
        Widget::step();
    }

    void draw(const DrawArgs& args) override {
        // Scale the entire FB to fit panel if SVG size differs from panel size
        if (svgNativeSize.x > 0.f && svgNativeSize.y > 0.f) {
            nvgSave(args.vg);
            nvgScale(args.vg, box.size.x / svgNativeSize.x, box.size.y / svgNativeSize.y);
            Widget::draw(args);
            nvgRestore(args.vg);
        } else {
            Widget::draw(args);
        }
    }
};
```

> [!TIP]
> The key difference: the `nvgScale` is applied **outside** the FramebufferWidget's coordinate space, in the parent `draw()`. The FramebufferWidget will see this scale as part of the **world transform** (via `nvgCurrentTransform`) and factor it into `fbScale` correctly. This avoids the double-scale problem because the FB's internal child draws at native SVG size — the scale is only in the world transform, not duplicated inside.

### Option C: Fix the SVG Document Sizes (Best Long-Term)

Ensure each labels SVG has the same document/viewport size as its corresponding panel SVG. Then use Option A. This is the cleanest path since it matches exactly what Rack does.

---

## What Changes in the Oversample Strategy

| Scenario | Current Code | Rack's SvgPanel | Recommended |
|----------|-------------|-----------------|-------------|
| pixelRatio < 2.0 | clamp(req, 1.0, 1.5) | **2.0** | **2.0** |
| pixelRatio >= 2.0 | 1.0 | **1.0** | **1.0** |

> [!WARNING]
> The current code clamps oversample to 1.5 max even on normal DPI. Rack's own panels use **2.0** oversample on normal DPI. This is likely why labels appear slightly lower quality than panel text — they're rendered at 1.5× while the panel is at 2×.

---

## What To Remove

1. **`FittedPanelSvgWidget`** — Replace with standard `SvgWidget` usage (no manual `nvgScale`)
2. **`targetOversample()` adaptive logic** — Replace with Rack's simple `pixelRatio < 2.0 ? 2.0 : 1.0`
3. **`cachedOversample` tracking** — Rack doesn't track this; `FramebufferWidget::draw()` already re-renders when scale changes
4. **The `requestedOversample` parameter** — No longer needed; oversample is always 2.0 or 1.0

---

## Cache Invalidation Strategy

Rack's `FramebufferWidget::draw()` already handles all cache invalidation automatically:

1. **Scale change** (zoom/UI scale): `!scale.equals(internal->fbScale)` → dirty ✅
2. **Subpixel change** (scrolling): controlled by `dirtyOnSubpixelChange` ✅ (we set `false` for labels)
3. **Viewport change**: `!internal->fbClipBox.contains(args.clipBox)` → dirty ✅
4. **Context change** (DAW window reopen): `onContextDestroy`/`onContextCreate` → dirty ✅

**No manual dirty tracking needed** for oversample changes because:
- `step()` sets `fb->oversample` each frame, but this is just a float assignment
- The FB only re-renders when `dirty == true` (which it self-manages via scale/subpixel checks)
- If `pixelRatio` changes (e.g., window moves between monitors), the world transform scale changes too, which triggers the `!scale.equals(internal->fbScale)` check → automatic re-render

> [!NOTE]
> Rack's `SvgPanel::step()` also sets oversample every frame without calling `setDirty()`. It relies on the automatic scale-change detection. Our code can do the same.

---

## Performance Impact

This change should **improve** performance:
- Removes the per-`step()` float comparison and conditional `setDirty()` call
- Removes the extra `nvgScale` transform inside the hot FB render path
- Matches Rack's own battle-tested pattern (no surprises at any scale)
- Oversample 2.0 on normal DPI is slightly more expensive than 1.5, but this matches what Rack does for its own panels — it's the expected cost

---

## Test Matrix

| UI Scale | pixelRatio | Expected oversample | What to verify |
|----------|-----------|-------------------|----------------|
| 100% | 1.0 | 2.0 | Labels sharp, correctly sized, match panel |
| 150% | 1.5 | 2.0 | Labels sharp, correctly sized, match panel |
| 200% | 2.0 | 1.0 | **Previously broken** — labels must be correctly sized |
| 250% | 2.5 | 1.0 | Labels sharp, correctly sized |

For each scale, also test:
- Rack zoom: 25%, 50%, 100%, 200%
- Static view and scrolling/panning
- Module in isolation and in a busy patch

---

## Summary of Changes

```diff
 // VisualAssets.hpp
-Widget* createPanelLabelsWidget(const char* svgPath, Vec panelSizePx, float oversample = 2.0f);
+Widget* createPanelLabelsWidget(const char* svgPath, Vec panelSizePx);
```

```diff
 // VisualAssets.cpp
-struct FittedPanelSvgWidget final : TransparentWidget { ... };  // DELETE
 
 struct CachedPanelLabelsWidget final : Widget {
     widget::FramebufferWidget* fb = nullptr;
-    float requestedOversample = 1.f;
-    float cachedOversample = 0.f;
 
-    CachedPanelLabelsWidget(const char* svgPath, Vec panelSizePx, float requestedOversample) {
+    CachedPanelLabelsWidget(const char* svgPath, Vec panelSizePx) {
         box.size = panelSizePx;
-        this->requestedOversample = requestedOversample;
         fb = new widget::FramebufferWidget();
-        fb->box.size = panelSizePx;
-        fb->oversample = targetOversample();
         fb->dirtyOnSubpixelChange = false;
-        cachedOversample = fb->oversample;
 
-        FittedPanelSvgWidget* labels = new FittedPanelSvgWidget(loadPluginSvgCached(svgPath));
-        labels->box.size = panelSizePx;
+        auto* sw = new widget::SvgWidget();
+        sw->setSvg(loadPluginSvgCached(svgPath));
+        fb->box.size = sw->box.size;
         fb->addChild(labels);
         addChild(fb);
     }
 
-    float targetOversample() const { ... }  // DELETE
-
     void step() override {
         if (fb) {
-            const float nextOversample = targetOversample();
-            if (std::fabs(nextOversample - cachedOversample) > 0.01f) {
-                fb->oversample = nextOversample;
-                fb->setDirty();
-                cachedOversample = nextOversample;
-            }
+            const float pr = (APP && APP->window) ? APP->window->pixelRatio : 1.f;
+            fb->oversample = (pr < 2.f) ? 2.f : 1.f;
         }
         Widget::step();
     }
+
+    void draw(const DrawArgs& args) override {
+        // If SVG native size differs from panel, scale at the parent level
+        // (outside the FramebufferWidget's coordinate space)
+        // ... see Option B above if needed
+        Widget::draw(args);
+    }
 };
```

> [!IMPORTANT]
> **Before implementing**: Check whether the labels SVGs have the same document size as their corresponding panel SVGs. If they do → Option A (simplest). If not → Option B or Option C.
