# Chromatide — Bitmap Painting Source for Iris

## Implementation Specification

**Working module name:** Chromatide  
**Plugin:** Leviathan  
**Target:** VCV Rack 2  
**Language:** C++17  
**Primary consumer:** Iris  
**Source category:** Editable raster image source  
**Canonical raster:** 1024 × 256 RGB8 (`iris::kCanonicalSourceWidth` × `iris::kCanonicalSourceHeight`)  
**Persistence:** QOI-compressed image embedded in patch JSON as Base64  

---

## 1. Purpose

Chromatide is a compact bitmap-painting module that allows users to create raster image data directly inside VCV Rack and transmit that data to Iris in real time.

Chromatide complements Nautiloid:

* **Nautiloid** is a reconstructible procedural fractal source. Iris can restore its image from fractal type, coordinates, zoom, and rendering parameters.
* **Chromatide** is an authoritative raster painting source. The painted pixels are the source data and are serialized losslessly.
* **Iris image loading** remains the final common path. Once Chromatide’s raster payload enters Iris, it is handled identically to decoded PNG or external image data (`iris::SOURCE_EXPANDER_IMAGE`).

Chromatide owns its canvas independently of Iris. Saving, duplicating, or removing modules does not create hidden ownership dependencies between them.

---

## 2. Core Design Decisions

### 2.1 Canonical backing raster

Chromatide stores one authoritative bitmap matching Iris’s canonical source dimensions:

```cpp
#include "IrisSourceField.hpp"

static constexpr int CHROMATIDE_WIDTH = iris::kCanonicalSourceWidth;    // 1024
static constexpr int CHROMATIDE_HEIGHT = iris::kCanonicalSourceHeight;  // 256
static constexpr int CHROMATIDE_CHANNELS = iris::kCanonicalSourceChannels; // 3
```

The runtime format is interleaved RGB8:

```text
R0 G0 B0  R1 G1 B1  R2 G2 B2 ...
```

The uncompressed image buffer requires exactly:

```text
1024 × 256 × 3 = 786,432 bytes
```

The backing raster remains at this exact resolution throughout editing, serialization, and transfer to Iris. Chromatide does not maintain a lower-resolution editable document or upscale on transmission.

### 2.2 Compact canvas geometry

The compact canvas UI uses the visible dimensions of Nautiloid’s fractal preview widget.

In the panel layout, this is loaded from the SVG panel anchor `"DISPLAY"` via `panel_svg::loadRectFromSvgMm` with standard fallback:

```cpp
// Nautiloid display geometry fallback (in mm)
const math::Rect displayRectMm(Vec(1.8f, 6.5f), Vec(98.0f, 65.27f));
```

The compact viewport has an aspect ratio of approximately 1.50:1 (3:2), whereas the canonical raster has a 4:1 aspect ratio (1024 × 256). The viewport is therefore a transformed display rather than a literal 1:1 pixel grid.

### 2.3 Fixed coordinate transform

All user interaction passes through a static bidirectional transform:

```text
Widget-local screen coordinates (x_local, y_local)
            ↓
Normalized canvas coordinates (u, v) ∈ [0.0, 1.0]
            ↓
Canonical raster coordinates (X, Y) ∈ [0, 1023] × [0, 255]
```

Normalized coordinates are defined as:

```cpp
u = clamp(localX / viewportWidth, 0.0f, 1.0f);
v = clamp(localY / viewportHeight, 0.0f, 1.0f);
```

Raster coordinates are:

```cpp
rasterX = u * static_cast<float>(CHROMATIDE_WIDTH - 1);
rasterY = v * static_cast<float>(CHROMATIDE_HEIGHT - 1);

int x = clamp(static_cast<int>(std::round(rasterX)), 0, CHROMATIDE_WIDTH - 1);
int y = clamp(static_cast<int>(std::round(rasterY)), 0, CHROMATIDE_HEIGHT - 1);
```

### 2.4 Canonical visual interpretation

The Nautiloid-sized viewport is Chromatide’s canonical visual presentation.

A brush stroke that appears circular in the viewport occupies an ellipse in the 1024 × 256 raster buffer. This is intentional. When rendered back through the viewport transform, the brush appears circular to the user. No attempt should be made to force isotropic circularity in the raw 4:1 raster storage.

### 2.5 Expanded editor overlay

Chromatide provides an expanded editor overlay matching the overlay architecture used by Wyrm (`WyrmExpandedEditorOverlay` in `WyrmWidget.cpp`):

* Uses the same perceptual aspect ratio (1.50:1) as the compact canvas.
* Displays a larger rendering of the backing raster.
* Shares the exact same `ChromatideCanvas` document object via reference or shared state.
* Does not copy, resample, encode, decode, or allocate new raster buffers when opened.
* Reparents the editor drawing surface (`ChromatideEditorSurface`) to `APP->scene` below `APP->scene->menuBar` when expanded, and returns it to the module widget container when closed.
* Closes safely without modifying the image beyond edits explicitly made by the user.

---

## 3. MVP Feature Set

Chromatide’s initial implementation includes:

1. Circular paint brush with viewport-isotropic mapping
2. Adjustable brush size (1 to 128 raster pixels)
3. Adjustable brush opacity (0.0 to 1.0)
4. Foreground color selection through a compact 8-color swatch palette
5. Eraser tool (paints using configured background color)
6. Eyedropper tool (samples raster RGB at pointer position)
7. Non-destructive Undo / Redo history stack
8. Clear canvas operation
9. Expanded editor overlay
10. Lossless patch persistence via QOI compression + Base64 encoding in patch JSON
11. Real-time raster transmission to Iris via `nautiloid_iris_expander::SourceSlot`
12. Automatic publication of committed edit transactions to Iris

---

## 4. Source Layout

Chromatide source files reside directly in `src/`, adhering to the project's flat layout:

```text
src/
    Chromatide.hpp                 // Module model definition, param/input/output/light IDs
    Chromatide.cpp                 // Engine logic, process(), serialization, publication
    ChromatideCanvas.hpp           // Document model, pixel access, dirty rect, undo history
    ChromatideCanvas.cpp           // Brush blending, stroke rasterization, QOI/Base64 codec
    ChromatideWidget.cpp           // Compact module UI, NanoVG renderer, controls, context menu
    ChromatideExpandedEditor.hpp   // Expanded overlay container & overlay link declarations
    ChromatideExpandedEditor.cpp   // Expanded editor overlay UI implementation
```

Existing infrastructure used directly:

```text
src/
    IrisSourceField.hpp            // iris::SourceField, canonical dimensions
    IrisWavetable.hpp              // iris::SourceKind (iris::SOURCE_EXPANDER_IMAGE)
    NautiloidIrisExpander.hpp      // nautiloid_iris_expander::SourceSlot ring-buffer
    third_party/qoi.h              // QOI encoder & decoder
    plugin.hpp                     // Plugin instance and model exports
```

Module registration in `src/plugin.hpp` and `src/plugin.cpp`:

```cpp
extern Model* modelChromatide;
```

Plugin slug:

```text
Chromatide
```

---

## 5. Canvas Document Model

### 5.1 Structure and ownership

`ChromatideCanvas` is owned directly by the `Chromatide` module instance (`src/Chromatide.hpp`):

```cpp
#pragma once

#include "IrisSourceField.hpp"
#include <array>
#include <vector>
#include <cstdint>
#include <cstddef>

class ChromatideCanvas {
public:
    static constexpr int WIDTH = iris::kCanonicalSourceWidth;   // 1024
    static constexpr int HEIGHT = iris::kCanonicalSourceHeight; // 256
    static constexpr int CHANNELS = iris::kCanonicalSourceChannels; // 3
    static constexpr size_t BUFFER_SIZE = static_cast<size_t>(WIDTH * HEIGHT * CHANNELS); // 786,432

    std::array<uint8_t, BUFFER_SIZE> pixels {};
    uint64_t revision = 0;

    void clear(uint8_t r, uint8_t g, uint8_t b);
    void sample(int x, int y, uint8_t& r, uint8_t& g, uint8_t& b) const;
    void setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);
    
    inline static size_t pixelOffset(int x, int y) {
        return (static_cast<size_t>(y) * static_cast<size_t>(WIDTH) + static_cast<size_t>(x)) * static_cast<size_t>(CHANNELS);
    }
};
```

The `Chromatide` module owns the canvas object. Widget and expanded-editor objects receive references to it.

The canvas must not be owned solely by the UI widget because:

* Patch serialization (`dataToJson` / `dataFromJson`) is executed on the module model.
* The document model must survive UI reconstruction (e.g. when zooming out or hiding module UI).
* Iris transmission occurs asynchronously from UI lifecycle.
* Headless and command-line execution modes remain fully valid.

### 5.2 Default Canvas State

New module instances initialize with an opaque black canvas:

```text
RGB = (0, 0, 0)
```

The background color defaults to black `(0, 0, 0)` and serves as the default target color for the eraser.

---

## 6. Brush Engine and Rasterization

### 6.1 Brush state

```cpp
struct ChromatideColor {
    uint8_t r = 255;
    uint8_t g = 255;
    uint8_t b = 255;
};

enum class ChromatideTool {
    Brush = 0,
    Eraser = 1,
    Eyedropper = 2
};

struct ChromatideBrushState {
    float size = 24.0f;       // Native vertical diameter in raster pixels (1.0 to 128.0)
    float opacity = 1.0f;    // Blend opacity (0.0 to 1.0)
    ChromatideColor foreground {255, 255, 255};
    ChromatideColor background {0, 0, 0};
    ChromatideTool tool = ChromatideTool::Brush;
};
```

### 6.2 Viewport-Isotropic Elliptical Raster Stamp Math

Let:

```text
W = viewportWidth (e.g. 98.0 mm)
H = viewportHeight (e.g. 65.27 mm)
A = W / H (Viewport aspect ratio ≈ 1.50)
```

To render a stamp that appears circular in viewport coordinates with vertical raster diameter $D$:

```text
Vertical raster radius:   Ry = D / 2.0f
Horizontal raster radius: Rx = (D / 2.0f) * (4.0f / A)
```

For a target point $(X, Y)$ in raster coordinates relative to stamp center $(X_c, Y_c)$:

```text
dx = (X - Xc) / Rx
dy = (Y - Yc) / Ry
normalizedDistanceSquared = dx * dx + dy * dy
```

If $\text{normalizedDistanceSquared} \le 1.0$, the point lies within the stamp boundary.

### 6.3 Antialiased Edge Falloff

To produce smooth antialiased stroke edges, calculate edge coverage across a 1.0-raster-pixel transition boundary:

```cpp
float distanceInPixels = std::sqrt(dx * dx * Rx * Rx + dy * dy * Ry * Ry);
float maxRadius = std::max(Rx, Ry);
float edgeStart = std::max(0.0f, maxRadius - 0.5f);
float edgeEnd = maxRadius + 0.5f;

float coverage = 1.0f - clamp((distanceInPixels - edgeStart) / (edgeEnd - edgeStart), 0.0f, 1.0f);
coverage = coverage * coverage * (3.0f - 2.0f * coverage); // Smoothstep
```

### 6.4 Alpha Compositing Semantics

Chromatide stores 24-bit RGB canvas data. Brush opacity is an inline compositing operation:

```cpp
float alpha = brushOpacity * coverage;

for (int c = 0; c < 3; ++c) {
    uint8_t srcColor = (tool == ChromatideTool::Eraser) ? bg[c] : fg[c];
    uint8_t dstColor = pixels[offset + c];
    float blended = static_cast<float>(dstColor) + alpha * (static_cast<float>(srcColor) - static_cast<float>(dstColor));
    pixels[offset + c] = static_cast<uint8_t>(clamp(std::round(blended), 0.0f, 255.0f));
}
```

### 6.5 Stroke Interpolation

Mouse pointer events from Rack UI do not arrive densely enough for continuous strokes during fast cursor moves.

Interpolate brush stamps along a straight line between previous position $(u_{prev}, v_{prev})$ and current position $(u_{curr}, v_{curr})$ in normalized canvas coordinates $[0, 1] \times [0, 1]$.

Stamp step distance:

```cpp
float viewportBrushDiameter = brushSize / static_cast<float>(CHROMATIDE_HEIGHT);
float stepSizeNorm = 0.20f * viewportBrushDiameter; // 20% of brush diameter
stepSizeNorm = std::max(stepSizeNorm, 0.001f);       // Minimum step threshold

float distanceNorm = std::hypot(uCurr - uPrev, (vCurr - vPrev) / A);
int numSteps = static_cast<int>(std::ceil(distanceNorm / stepSizeNorm));

for (int i = 1; i <= numSteps; ++i) {
    float t = static_cast<float>(i) / static_cast<float>(numSteps);
    float u = uPrev + t * (uCurr - uPrev);
    float v = vPrev + t * (vCurr - vPrev);
    stampAtNormalized(u, v, brushState);
}
```

---

## 7. Editing Transactions

An edit transaction represents one discrete undoable operation:

* A completed brush stroke (Pointer down $\rightarrow$ move $\rightarrow$ up)
* A completed eraser stroke
* Clear canvas
* Palette apply / modify (if affecting canvas)
* Undo / Redo execution

During an active stroke:

```text
Pointer down  → Begin transaction; snapshot canvas dirty bounds
Pointer move  → Apply stamp, expand dirty rectangle, update preview texture
Pointer up    → Commit transaction, push undo record, increment revision, publish to Iris
```

Increment `canvas.revision` on every committed edit:

```cpp
canvas.revision++;
```

---

## 8. Undo and Redo Architecture

Undo and redo use dirty-bounded image delta records to minimize memory consumption.

```cpp
struct RectI {
    int minX = 0;
    int minY = 0;
    int maxX = 0;
    int maxY = 0;

    int width() const { return maxX - minX + 1; }
    int height() const { return maxY - minY + 1; }
    bool valid() const { return maxX >= minX && maxY >= minY; }
};

struct ChromatideUndoRecord {
    RectI bounds;
    std::vector<uint8_t> beforeRgb;
    std::vector<uint8_t> afterRgb;
    
    size_t memoryUsage() const {
        return beforeRgb.size() + afterRgb.size() + sizeof(*this);
    }
};
```

### Undo Lifecycle

1. **Stroke Start:** Capture initial bounding box of active edits and clone the initial area.
2. **Stroke Update:** Update dirty bounding box `RectI dirtyBounds`.
3. **Stroke End:** Extract cropped `beforeRgb` and `afterRgb` regions corresponding to `dirtyBounds`.
4. **Push:** Push `ChromatideUndoRecord` onto `undoStack`. Clear `redoStack`.
5. **Memory Budget:** If total memory consumed by `undoStack` exceeds 32 MiB, pop the oldest records from the bottom of `undoStack`.

Undo history is runtime-only and excluded from patch JSON serialization.

---

## 9. Palette and Color Selection

### 9.1 Compact Palette

Chromatide includes an 8-color swatch palette with default sRGB values:

| Swatch | Name   | Hex | RGB |
| :--- | :--- | :--- | :--- |
| 0 | Black | `#000000` | (0, 0, 0) |
| 1 | White | `#FFFFFF` | (255, 255, 255) |
| 2 | Red | `#FF3B30` | (255, 59, 48) |
| 3 | Orange | `#FF9500` | (255, 149, 0) |
| 4 | Yellow | `#FFCC00` | (255, 204, 0) |
| 5 | Green | `#34C759` | (52, 199, 89) |
| 6 | Cyan | `#5AC8FA` | (90, 200, 250) |
| 7 | Purple | `#AF52DE` | (175, 82, 222) |

Clicking a swatch selects it as the active `foreground` color. The UI highlights the active swatch with a Leviathan cyan outline ring.

### 9.2 Eyedropper Tool

Clicking the Eyedropper tool enters momentary sample mode. Clicking anywhere on the canvas samples the exact RGB raster byte at that position, updates `foreground` color and active palette swatch (if matching), and automatically reverts tool selection to `Brush`.

---

## 10. Module UI and Widget Layout

The compact panel is structured as follows:

1. **Top Display Section:**
   * Custom `ChromatideDisplay` widget placed in the `DISPLAY` SVG anchor rect (`98.0mm × 65.27mm`).
   * Renders the 1024 × 256 raster preview via NanoVG texture.
   * Displays circular brush outline cursor on hover.
   * Handles pointer press, drag, release, and hover events.

2. **Tool Bar Controls:**
   * Tool selection buttons: `Brush`, `Eraser`, `Eyedropper`.
   * Action buttons: `Undo`, `Redo`, `Clear`.
   * `Expand Editor` glyph button (opens expanded editor overlay).

3. **Sliders / Knobs:**
   * `Size`: Brush size parameter (1 to 128 pixels, nonlinear response).
   * `Opacity`: Brush opacity parameter (0.0 to 1.0).

4. **Color Swatch Palette:**
   * 8 interactive color swatches arranged horizontally below the display.

5. **Clear Confirmation Protection:**
   * Accidental clicks on `Clear` require double-click confirmation or context-menu invocation to prevent loss of artwork. `Clear` produces one standard undoable record.

---

## 11. Expanded Editor Overlay

Following `WyrmWidget.cpp` (`WyrmExpandedEditorOverlay`):

```cpp
struct ChromatideOverlayLink {
    ChromatideWidget* owner = nullptr;
    ChromatideExpandedEditorOverlay* overlay = nullptr;
};
```

When the user clicks the `Expand Editor` button:

1. Create `ChromatideExpandedEditorOverlay` instance.
2. Reparent `ChromatideEditorSurface` from compact `editorDock` to `overlay->editorZoom`.
3. Add `overlay` to `APP->scene` below `APP->scene->menuBar`.
4. Layout overlay to cover expanded screen region while maintaining 1.50:1 aspect ratio.
5. On close (or module deletion), reparent `ChromatideEditorSurface` back to `editorDock` and delete `overlay`.

No pixel data copy or texture duplicate occurs during expand / collapse.

---

## 12. Preview Texture Management and Dirty Tracking

### 12.1 NanoVG Texture Staging

```cpp
struct ChromatidePreviewRenderer {
    int nvgImageHandle = -1;
    NVGcontext* lastNvgContext = nullptr;
    std::vector<uint8_t> rgbaBuffer; // 1024 * 256 * 4 = 1,048,576 bytes
    RectI dirtyRect;
    bool needsTextureUpload = false;
};
```

### 12.2 Dirty Bounds and Conversion

During painting, stamps expand `dirtyRect`. Prior to rendering each frame in `draw()`:

```cpp
void updateStagingBuffer(const ChromatideCanvas& canvas, const RectI& dirty) {
    if (!dirty.valid()) return;
    for (int y = dirty.minY; y <= dirty.maxY; ++y) {
        for (int x = dirty.minX; x <= dirty.maxX; ++x) {
            size_t srcOff = ChromatideCanvas::pixelOffset(x, y);
            size_t dstOff = (static_cast<size_t>(y) * CHROMATIDE_WIDTH + static_cast<size_t>(x)) * 4u;
            rgbaBuffer[dstOff + 0] = canvas.pixels[srcOff + 0];
            rgbaBuffer[dstOff + 1] = canvas.pixels[srcOff + 1];
            rgbaBuffer[dstOff + 2] = canvas.pixels[srcOff + 2];
            rgbaBuffer[dstOff + 3] = 255;
        }
    }
}
```

Upload texture to NanoVG at most once per UI frame using `nvgUpdateImage` or `nvgCreateImageRGBA`. Recreate `nvgImageHandle` automatically whenever `vg` context changes.

---

## 13. Patch Serialization (QOI + Base64)

### 13.1 Serialization Format

`dataToJson` encodes the 786,432-byte RGB8 canvas buffer using QOI (`src/third_party/qoi.h`) and standard Base64 encoding into patch JSON:

```json
{
  "chromatideVersion": 1,
  "canvas": {
    "encoding": "qoi-base64",
    "width": 1024,
    "height": 256,
    "channels": 3,
    "data": "cW9pZgAAA4AAAAEAAAMB..."
  },
  "brush": {
    "size": 24.0,
    "opacity": 1.0,
    "tool": 0,
    "foreground": [255, 255, 255],
    "background": [0, 0, 0]
  },
  "palette": [
    [0, 0, 0],
    [255, 255, 255],
    [255, 59, 48],
    [255, 149, 0],
    [255, 204, 0],
    [52, 199, 89],
    [90, 200, 250],
    [175, 82, 222]
  ],
  "selectedPaletteIndex": 1
}
```

### 13.2 Save / Load Implementation

```cpp
json_t* Chromatide::dataToJson() {
    json_t* root = json_object();
    json_object_set_new(root, "chromatideVersion", json_integer(1));

    // 1. QOI encode canvas RGB8 pixels
    qoi_desc desc {};
    desc.width = ChromatideCanvas::WIDTH;
    desc.height = ChromatideCanvas::HEIGHT;
    desc.channels = ChromatideCanvas::CHANNELS;
    desc.colorspace = QOI_SRGB;

    int outLen = 0;
    void* qoiData = qoi_encode(canvas.pixels.data(), &desc, &outLen);
    
    if (qoiData && outLen > 0) {
        // 2. Base64 encode QOI payload
        std::string b64Str = base64Encode(static_cast<const uint8_t*>(qoiData), size_t(outLen));
        std::free(qoiData);

        json_t* canvasJ = json_object();
        json_object_set_new(canvasJ, "encoding", json_string("qoi-base64"));
        json_object_set_new(canvasJ, "width", json_integer(ChromatideCanvas::WIDTH));
        json_object_set_new(canvasJ, "height", json_integer(ChromatideCanvas::HEIGHT));
        json_object_set_new(canvasJ, "channels", json_integer(ChromatideCanvas::CHANNELS));
        json_object_set_new(canvasJ, "data", json_string(b64Str.c_str()));
        json_object_set_new(root, "canvas", canvasJ);
    }
    
    // Serialize brush & palette settings...
    return root;
}
```

### 13.3 Deserialization Validation

During `dataFromJson`:

1. Decode Base64 string to byte vector.
2. Validate QOI header magic (`"qoif"`), `width == 1024`, `height == 256`, `channels == 3`.
3. Decode QOI bytes via `qoi_decode`.
4. Validate decoded length equals exactly 786,432 bytes.
5. If any validation fails, reset canvas to opaque black, log warning via `WARN("Chromatide: patch canvas decode failed")`, and maintain stable execution.

---

## 14. Iris Integration & Real-Time Publication

### 14.1 Inter-Module Expander Protocol

Chromatide connects to Iris as a left expander (`leftExpander.module`).

It publishes its canvas payload using Iris's existing multi-slot expander mechanism defined in `src/NautiloidIrisExpander.hpp` and `src/IrisSourceField.hpp`:

```cpp
#include "NautiloidIrisExpander.hpp"
#include "IrisSourceField.hpp"
#include "IrisWavetable.hpp"
```

Chromatide module manages an array of 3 expander source slots:

```cpp
std::array<nautiloid_iris_expander::SourceSlot, nautiloid_iris_expander::kSourceSlotCount> irisExpanderSlots;
std::atomic<int> irisExpanderWriteSlot {0};
std::atomic<int> irisExpanderPublishedSlot {-1};
std::atomic<uint64_t> irisPreviewGeneration {1u};
```

### 14.2 Publication Workflow

When a stroke ends, or clear / undo / redo occurs:

```cpp
void Chromatide::publishToIris() {
    uint64_t nextGen = ++irisPreviewGeneration;
    
    // 1. Find an idle slot in irisExpanderSlots
    int slotIdx = (irisExpanderWriteSlot.load() + 1) % nautiloid_iris_expander::kSourceSlotCount;
    nautiloid_iris_expander::SourceSlot* slot = &irisExpanderSlots[slotIdx];

    if (!nautiloid_iris_expander::claimSourceSlotForWrite(slot)) return;

    // 2. Populate iris::SourceField
    slot->source.width = iris::kCanonicalSourceWidth;   // 1024
    slot->source.height = iris::kCanonicalSourceHeight; // 256
    slot->source.channels = iris::kCanonicalSourceChannels; // 3
    slot->source.bitDepth = iris::kCanonicalSourceBitDepth; // 8
    slot->source.rgb8.assign(canvas.pixels.begin(), canvas.pixels.end());
    slot->source.sourceName = "Chromatide Canvas";
    slot->source.sourcePath = "";
    slot->source.originalWidth = iris::kCanonicalSourceWidth;
    slot->source.originalHeight = iris::kCanonicalSourceHeight;
    slot->source.originalChannels = iris::kCanonicalSourceChannels;
    slot->source.generatorKind = iris::SOURCE_GENERATOR_NONE;

    slot->generation.store(nextGen, std::memory_order_release);
    nautiloid_iris_expander::releaseSourceSlotWrite(slot);
    irisExpanderPublishedSlot.store(slotIdx, std::memory_order_release);

    // 3. Notify right-attached Iris module
    Module* right = rightExpander.module;
    if (right && (right->model == modelIris || right->model->slug == "Iris")) {
        if (right->leftExpander.module == this) {
            if (auto* iris = dynamic_cast<Iris*>(right)) {
                iris->requestExpanderSource(slot, nextGen);
            }
        }
    }
}
```

### 14.3 Source Kind Alignment

Iris receives the payload as `iris::SOURCE_EXPANDER_IMAGE` (defined in `src/IrisWavetable.hpp`), which routes directly into Iris's existing image-to-wavetable rendering pipeline without requiring code alterations inside Iris's core synthesis engine.

---

## 15. Module Duplication and Independence

Duplicating a Chromatide module creates a completely independent copy of:

* Canvas RGB pixels
* Brush parameters (size, opacity, tool)
* Color palette and background/foreground selection
* Revision counter

No global static canvas buffers or shared mutable pointers are used.

---

## 16. Context Menu

Chromatide provides standard right-click context menu options:

* **Reset Canvas to Black** (undoable)
* **Reset Palette to Defaults**
* **Re-publish Canvas to Iris** (forces immediate expander payload sync)

---

## 17. Performance Constraints

Chromatide is designed to run with zero audio-thread impact:

* **Zero allocations in `process()`:** Audio `process()` method contains only basic gate/CV checks or remains empty.
* **No per-stamp heap allocation:** Canvas editing reuses preallocated pixel arrays and scratch buffers.
* **Rate-limited UI uploads:** Preview texture updates occur at most once per UI frame (`draw()`).
* **No QOI encoding during painting:** Encoding is performed exclusively on patch save (`dataToJson()`).

---

## 18. Error Handling & Edge Cases

Chromatide remains robust against malformed input:

* **Patch corruption:** Bad Base64 or corrupted QOI data defaults gracefully to an opaque black canvas with a log error via `WARN()`.
* **Disconnection:** Disconnecting or deleting attached Iris modules does not crash or stall Chromatide.
* **Overlay safety:** Closing the patch or deleting the module widget while the expanded editor overlay is active safely closes the overlay link without memory leaks or dangling pointer accesses.

---

## 19. Non-Goals for MVP

* Layers or selection masks
* Shape tools (lines, rectangles, ellipses, text)
* Bucket fill, gradients, or blur filters
* Hardness / softness brush curves
* CV control over brush position or painting
* Audio-rate image streaming

---

## 20. Implementation Plan

### Phase 1 — Canvas & Codec
* Implement `ChromatideCanvas` with $1024 \times 256$ RGB8 storage in `src/ChromatideCanvas.hpp` / `.cpp`.
* Implement QOI + Base64 patch save/load functions in `src/ChromatideCanvas.cpp`.

### Phase 2 — Module & Iris Transport
* Implement `Chromatide` module model in `src/Chromatide.hpp` / `.cpp`.
* Implement `publishToIris()` using `nautiloid_iris_expander::SourceSlot` and `iris::SOURCE_EXPANDER_IMAGE`.

### Phase 3 — Compact UI & Painting Engine
* Implement `ChromatideWidget` in `src/ChromatideWidget.cpp`.
* Implement viewport coordinate transform, isotropic elliptical brush stamp math, antialiased edge falloff, and mouse event handling.

### Phase 4 — Undo/Redo & Palette Controls
* Implement dirty-bounded `ChromatideUndoRecord` stack with 32 MiB memory cap.
* Add compact 8-swatch palette, tool selection buttons, size/opacity controls, and clear confirmation.

### Phase 5 — Expanded Editor Overlay
* Implement `ChromatideExpandedEditorOverlay` in `src/ChromatideExpandedEditor.hpp` / `.cpp`.
* Implement non-allocating surface reparenting between compact dock and scene overlay.

### Phase 6 — Verification & Polish
* Verify bit-identical QOI round-trip serialization.
* Verify real-time transmission to Iris.
* Verify independent operation of duplicated Chromatide modules.

---

## 21. Required Verification Tests

1. **Transform Test:** Top-left viewport coordinate maps to raster $(0, 0)$; bottom-right maps to $(1023, 255)$.
2. **Isotropy Test:** A brush painted in the viewport appears circular on screen and generates an ellipse with axis ratio $4 / A$ in the raster buffer.
3. **Stroke Continuity Test:** Fast pointer movement generates smooth continuous strokes without gaps via normalized interpolation.
4. **Undo/Redo Test:** Undo restores exact pre-stroke raster bytes; new edits clear the redo stack; undo history memory remains bounded $\le 32$ MiB.
5. **Persistence Test:** Patch save and reload restores exact bit-identical RGB canvas data via QOI + Base64.
6. **Iris Integration Test:** Editing Chromatide canvas publishes updated `iris::SourceField` to attached Iris module without audio glitches or image resampling.
7. **Overlay Safety Test:** Closing or deleting Chromatide module while expanded editor is open closes overlay cleanly without dangling pointers.

---

## 22. Acceptance Criteria

Chromatide is complete for MVP when:

* Users can paint fluidly on the compact Nautiloid-sized viewport with circular brush rendering.
* The underlying backing raster is authoritative, lossless, and stored at $1024 \times 256$ RGB8 (`iris::kCanonicalSourceWidth` × `iris::kCanonicalSourceHeight`).
* Edits transmit automatically to Iris as `iris::SOURCE_EXPANDER_IMAGE` via lock-free `nautiloid_iris_expander::SourceSlot`.
* Patch save/load serializes the image losslessly via QOI + Base64 in patch JSON.
* Compact and expanded editors operate on the same document without memory duplication.
* Module duplication maintains 100% canvas independence between instances.
* Audio engine performance remains completely unaffected by canvas painting.
