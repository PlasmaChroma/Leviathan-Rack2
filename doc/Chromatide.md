# Chromatide — Bitmap Painting Source for Iris

## Implementation Specification

**Working module name:** Chromatide
**Plugin:** Leviathan
**Target:** VCV Rack 2
**Language:** C++17
**Primary consumer:** Iris
**Source category:** Editable raster image source
**Canonical raster:** 1024 × 256 RGB8
**Persistence:** QOI-compressed image embedded in patch JSON

---

## 1. Purpose

Chromatide is a compact bitmap-painting module that allows users to create image data directly inside VCV Rack and transmit that data to Iris.

Chromatide complements Nautiloid:

* **Nautiloid** is a reconstructible procedural source. Iris can restore its image from fractal type, coordinates, zoom, and rendering parameters.
* **Chromatide** is an authoritative raster source. The painted pixels are the source data and must be serialized losslessly.
* **Iris image loading** remains the final common path. Once Chromatide’s raster enters Iris, it should be handled like decoded PNG image data.

Chromatide must own its canvas independently of Iris. Saving, duplicating, or removing modules must not create hidden ownership dependencies between them.

---

## 2. Core Design Decisions

### 2.1 Canonical backing raster

Chromatide stores one authoritative bitmap:

```cpp
static constexpr int CHROMATIDE_WIDTH = 1024;
static constexpr int CHROMATIDE_HEIGHT = 256;
static constexpr int CHROMATIDE_CHANNELS = 3;
```

The runtime format is interleaved RGB8:

```text
RGBRGBRGB...
```

The uncompressed image requires:

```text
1024 × 256 × 3 = 786,432 bytes
```

The backing raster must remain at this exact resolution throughout editing, serialization, and transfer to Iris.

Chromatide must not maintain a lower-resolution editable document and upscale it before transmission.

### 2.2 Compact canvas geometry

The compact canvas must use the same visible dimensions as Nautiloid’s fractal preview widget.

Do not independently approximate or manually duplicate those dimensions. Reuse or centralize the relevant Nautiloid geometry constants where practical.

The compact view intentionally has a different aspect ratio from the underlying 4:1 raster. It is therefore a transformed viewport rather than a literal one-screen-pixel-per-image-pixel display.

### 2.3 Fixed transform

All interaction passes through a fixed transform:

```text
Widget-local screen coordinates
            ↓
Normalized canvas coordinates
            ↓
1024 × 256 raster coordinates
```

Normalized coordinates are defined as:

```cpp
u = localX / viewportWidth;
v = localY / viewportHeight;
```

Raster coordinates are:

```cpp
rasterX = u * 1024.0f;
rasterY = v * 256.0f;
```

Coordinates must be clamped before integer conversion:

```cpp
x = clamp(static_cast<int>(rasterX), 0, 1023);
y = clamp(static_cast<int>(rasterY), 0, 255);
```

The transform is static for a given view and should be calculated once when the view geometry is established.

### 2.4 Canonical visual interpretation

The compact Nautiloid-sized viewport is Chromatide’s canonical visual presentation.

A brush that appears circular in the viewport will generally occupy an ellipse in the 1024 × 256 raster. This is intentional. When the raster is rendered through the same viewport transform, the brush appears circular to the user.

No attempt should be made to preserve circular geometry when viewing the raw 4:1 raster independently.

### 2.5 Expanded editor

Chromatide must provide an expanded editor overlay similar in behavior to the expanded Wyrm editor.

The expanded editor:

* Uses the same perceptual aspect ratio as the compact Chromatide canvas.
* Displays a larger rendering of the same backing raster.
* Shares the exact same canvas object.
* Does not copy, resample, encode, decode, or transfer pixels when opened.
* Provides more physical drawing area and larger controls.
* Closes without modifying the underlying image beyond edits explicitly made by the user.

The compact and expanded views are two interfaces over one document.

---

## 3. MVP Feature Set

Chromatide’s initial implementation must include:

1. Circular paint brush
2. Adjustable brush size
3. Adjustable brush opacity
4. Foreground color selection through a compact palette
5. Eraser
6. Eyedropper
7. Undo
8. Redo
9. Clear canvas
10. Expanded editor
11. Lossless patch persistence
12. Raster transmission to Iris
13. Automatic publication after committed edits

The MVP does not require layers, selections, geometric shape tools, text, animation, pressure sensitivity, arbitrary resolution, or image-file export.

---

## 4. Suggested Source Layout

Use the repository’s existing conventions where they differ, but keep the document model separate from UI rendering.

Recommended files:

```text
src/
    Chromatide.cpp
    Chromatide.hpp
    ChromatideWidget.cpp
    ChromatideCanvas.cpp
    ChromatideCanvas.hpp
    ChromatideExpandedEditor.cpp
    ChromatideExpandedEditor.hpp
```

Shared image-source functionality may belong in existing Iris/Nautiloid infrastructure:

```text
src/image/
    ImageSourcePayload.hpp
    RasterImagePayload.hpp
```

Register the model using the normal Leviathan plugin structure:

```cpp
extern Model* modelChromatide;
```

Suggested plugin slug:

```text
Chromatide
```

---

## 5. Canvas Document Model

Create a document object owned by the module:

```cpp
class ChromatideCanvas {
public:
    static constexpr int WIDTH = 1024;
    static constexpr int HEIGHT = 256;
    static constexpr int CHANNELS = 3;

    std::array<uint8_t, WIDTH * HEIGHT * CHANNELS> pixels {};
    uint64_t revision = 0;

    void clear(const Rgb8& color);
    Rgb8 sample(int x, int y) const;
    void setPixel(int x, int y, const Rgb8& color);
};
```

The module owns this object. Widget and expanded-editor objects receive non-owning or appropriately lifetime-safe references to it.

The canvas must not be owned only by the widget because:

* Patch serialization belongs to the module.
* The document must survive UI reconstruction.
* Iris transmission should not depend on an active UI object.
* Headless or non-rendering Rack states must remain valid.

### 5.1 Pixel indexing

Use a single helper for indexing:

```cpp
inline size_t pixelOffset(int x, int y) {
    return static_cast<size_t>((y * WIDTH + x) * CHANNELS);
}
```

All coordinates must be validated or clamped before indexing.

### 5.2 Initial canvas

Default new modules to an opaque black canvas:

```text
RGB = 0, 0, 0
```

The background color should initially be black and should also serve as the eraser color.

---

## 6. Brush Model

### 6.1 Brush state

```cpp
struct ChromatideBrushState {
    float size = 24.0f;
    float opacity = 1.0f;
    Rgb8 foreground {255, 255, 255};
    Rgb8 background {0, 0, 0};

    enum class Tool {
        Brush,
        Eraser,
        Eyedropper
    };

    Tool tool = Tool::Brush;
};
```

Brush size is measured against the 256-pixel raster height.

A size of `24` means the brush’s native vertical diameter is approximately 24 raster pixels.

Suggested range:

```text
1–128 pixels
```

The control may use a nonlinear response so the smaller brush sizes receive greater precision.

### 6.2 View-circular brush geometry

Let:

```text
A = viewportWidth / viewportHeight
```

For a brush with vertical raster diameter `D`, the horizontal raster diameter required to appear circular in the viewport is:

```text
horizontalDiameter = D × 4 / A
verticalDiameter   = D
```

This relationship is static for each viewport aspect ratio.

The compact and expanded views must use the same aspect ratio, allowing the same native brush geometry to be used in both.

### 6.3 Alpha semantics

Chromatide stores RGB only. Brush opacity is a compositing operation and must not introduce a persistent alpha channel.

For each affected channel:

```cpp
float a = brushOpacity * coverage;
dst = round(dst + a * (src - dst));
```

Clamp the result to `0–255`.

The MVP should perform this blend directly in byte-oriented sRGB space. Do not introduce implicit gamma conversion unless the entire Iris image pipeline later standardizes on linear-light processing.

### 6.4 Eraser semantics

The eraser paints using `background`, applying the same brush size, edge coverage, and opacity behavior as the standard brush.

It does not erase into transparency.

### 6.5 Brush edge

The brush must have an antialiased perimeter.

Hardness may be fixed for the MVP. The interior should have full coverage and the outermost raster edge should transition smoothly over approximately one raster pixel.

A future hardness parameter may expand this behavior without changing the saved canvas format.

### 6.6 Stroke interpolation

Mouse events alone are not guaranteed to arrive densely enough to create continuous strokes.

Interpolate brush stamps between the previous and current pointer positions in normalized canvas space.

Suggested spacing:

```text
20% of the visible brush diameter
```

Apply a minimum spacing sufficient to avoid excessive repeated stamps for very small brushes.

Interpolation must occur before conversion to raster coordinates. This prevents horizontal movement from producing disproportionately dense stamping solely because the raster contains four times as many horizontal pixels.

---

## 7. Editing Transactions

A committed edit consists of one of:

* One complete brush stroke
* One complete eraser stroke
* Clear canvas
* Undo
* Redo
* A future whole-canvas operation

During a stroke:

```text
Pointer down  → begin transaction
Pointer move  → modify canvas and update preview
Pointer up    → commit transaction and publish revision
```

Do not publish a new Iris payload for every brush stamp.

### 7.1 Revision behavior

Increment `canvas.revision` after each committed image mutation:

```cpp
canvas.revision++;
```

This includes undo, redo, clear, and completed brush or eraser strokes.

Eyedropper actions do not modify pixels and must not increment the revision.

---

## 8. Undo and Redo

Undo and redo are required for the MVP.

Use bounded image-delta records rather than serializing undo history.

Recommended record:

```cpp
struct ChromatideUndoRecord {
    RectI bounds;
    std::vector<uint8_t> before;
    std::vector<uint8_t> after;
};
```

A straightforward implementation may:

1. Copy the complete 786,432-byte canvas at stroke start into a temporary scratch buffer.
2. Track the dirty bounds during the stroke.
3. At stroke completion, extract only the dirty rectangle from the before and after images.
4. Store those two cropped regions in the undo record.
5. Discard the complete temporary copy.

This keeps implementation complexity low while preventing every history entry from permanently consuming a full-canvas allocation.

Use either:

* A fixed maximum number of records, such as 64, or
* A memory budget, such as 32 MiB

A memory budget is preferred. Drop the oldest undo records when the budget is exceeded.

Starting a new committed edit after an undo must clear the redo stack.

Undo history is runtime-only and must not be written into patch JSON.

---

## 9. Palette and Color Selection

### 9.1 Compact palette

Provide a compact palette of eight foreground swatches.

Suggested defaults:

```text
Black
White
Red
Orange
Yellow
Green
Cyan
Purple
```

Clicking a swatch selects it as the foreground color.

Clearly indicate the active swatch with a border, glow, inset, or other Leviathan-consistent selection state.

### 9.2 Eyedropper

The eyedropper samples the backing raster at the pointer position and sets the foreground color.

Sampling must use the actual raster pixel, not a filtered preview color.

After one successful sample, the tool may either:

* Remain in eyedropper mode, or
* Return to the previous brush tool

For the MVP, return automatically to the brush tool after sampling. This makes the eyedropper behave as a momentary utility.

### 9.3 Palette persistence

Serialize the eight palette colors so future custom-palette editing can be added without changing the patch schema.

The MVP may ship with fixed palette editing behavior, but the serialized representation should already support arbitrary RGB values.

Also serialize:

* Selected palette index
* Foreground color
* Background color

---

## 10. Compact Module UI

The panel should include:

### Canvas area

* Exact visible dimensions of Nautiloid’s fractal preview widget
* Full-canvas transformed preview
* Brush cursor when hovered
* Direct mouse painting
* Expanded-editor button

### Tool controls

* Brush
* Eraser
* Eyedropper
* Undo
* Redo
* Clear

### Continuous controls

* Brush size
* Brush opacity

### Color controls

* Eight palette swatches
* Active foreground indication
* Background/eraser color indication where space permits

### Destructive action behavior

Clear should not happen accidentally.

Use one of:

* A context-menu confirmation
* Modifier-click
* A short two-stage armed state
* Existing Leviathan destructive-action conventions

Clear must create one undoable transaction.

---

## 11. Expanded Editor Overlay

Follow the existing expanded Wyrm editor architecture where applicable.

The overlay should:

* Render above neighboring modules.
* Remain associated with the owning Chromatide module.
* Close automatically if the module or module widget is destroyed.
* Avoid retaining dangling pointers.
* Share the same `ChromatideCanvas`.
* Share tool and brush state with the compact view.
* Use the same viewport aspect ratio as the compact canvas.
* Show the entire composition rather than changing to a raw 4:1 interpretation.

The expanded editor may provide larger versions of the compact controls.

Opening the editor must not:

* Copy the raster
* Convert it through QOI
* Resample it
* Create a second authoritative canvas
* Increment the canvas revision
* Publish a new image to Iris

Closing it must likewise be a UI-only operation.

Only committed edits made within the expanded editor affect revision state.

---

## 12. Preview Rendering

### 12.1 Persistent image texture

Maintain:

* The authoritative RGB8 canvas
* A persistent RGBA staging buffer
* A persistent NanoVG image handle or equivalent Rack-compatible texture

The texture should be scaled by the renderer into the compact or expanded viewport.

The authoritative raster must not be resized on the CPU for every frame.

### 12.2 Dirty tracking

Track modified raster bounds:

```cpp
struct DirtyRect {
    int minX;
    int minY;
    int maxX;
    int maxY;
    bool valid;
};
```

Every brush stamp expands the dirty rectangle.

Collapse multiple pointer events into at most one preview texture refresh per UI frame.

Even if the NanoVG implementation requires uploading the full texture, limit RGB-to-RGBA conversion to the dirty rectangle where practical.

A full RGBA texture is:

```text
1024 × 256 × 4 = 1 MiB
```

A full upload at UI frame frequency may be acceptable, but the code should not issue multiple full uploads within one frame.

### 12.3 Filtering

Use smooth filtering for the committed canvas so gradients and antialiased strokes survive downscaling.

Render the brush cursor separately in viewport space so it remains sharp and clearly circular.

Do not bake the cursor into the canvas texture.

### 12.4 Texture lifetime

Create and destroy graphics resources according to Rack/NanoVG context lifetime.

Do not serialize texture handles or hold them in the module’s JSON state.

The module’s image data must remain valid even when no widget or graphics context exists.

---

## 13. Patch Serialization

Chromatide must embed its authoritative canvas in the patch.

Use the same QOI and base64 utilities already used by Iris where possible.

### 13.1 Save path

```text
RGB8 canvas
    ↓
QOI encode
    ↓
Base64 encode
    ↓
Patch JSON string
```

### 13.2 Load path

```text
Patch JSON string
    ↓
Base64 decode
    ↓
QOI decode
    ↓
Validate
    ↓
RGB8 canvas
```

### 13.3 Suggested JSON structure

```json
{
  "chromatideVersion": 1,
  "canvas": {
    "encoding": "qoi-base64",
    "width": 1024,
    "height": 256,
    "channels": 3,
    "data": "<base64>"
  },
  "brush": {
    "size": 24.0,
    "opacity": 1.0,
    "tool": "brush",
    "foreground": [255, 255, 255],
    "background": [0, 0, 0]
  },
  "palette": [
    [0, 0, 0],
    [255, 255, 255],
    [255, 64, 64],
    [255, 144, 48],
    [255, 224, 64],
    [64, 220, 96],
    [48, 224, 255],
    [144, 72, 255]
  ],
  "selectedPaletteIndex": 1
}
```

QOI already carries some image metadata, but explicit JSON dimensions provide schema validation and clearer future migration.

### 13.4 Validation

Reject decoded data unless all of the following are true:

* Width is 1024
* Height is 256
* Channels are RGB-compatible
* Decoded byte count is exactly 786,432 after normalization
* QOI decoding succeeds without overrun or truncation

If decoding fails:

1. Reset to a black canvas.
2. Preserve stable module operation.
3. Log a concise warning.
4. Do not access partially decoded data.

### 13.5 Serialization exclusions

Do not serialize:

* Undo history
* Redo history
* Expanded-editor open state
* Texture handles
* RGBA preview buffers
* Dirty rectangles
* Temporary stroke snapshots
* Iris publication snapshots

---

## 14. Iris Integration

### 14.1 Source semantics

Chromatide must use the raster-image path inside Iris.

Do not represent Chromatide as a procedural or reconstructible source.

Once accepted by Iris, Chromatide data should enter the same canonical processing stage as decoded PNG data.

Conceptually:

```text
Chromatide RGB8 raster
         ↓
Existing Iris raster ingestion
         ↓
Existing image-to-wavetable conversion
```

### 14.2 No resampling at the boundary

Chromatide already produces the required 1024 × 256 raster.

Iris must not resample this payload merely because it came from another module.

Validate the dimensions and pass the pixel data directly into the existing canonical raster path.

### 14.3 Source type

Extend the existing image-source metadata only as much as necessary.

Suggested distinction:

```cpp
enum class IrisImageSourceKind {
    EmbeddedRaster,
    ExternalRaster,
    NautiloidFractal
};
```

Chromatide publishes:

```text
ExternalRaster
```

The internal raster processing should not require a Chromatide-specific conversion branch.

### 14.4 Payload

Adapt this shape to the existing Nautiloid/Iris transport:

```cpp
struct RasterImagePayload {
    uint32_t width = 1024;
    uint32_t height = 256;
    uint32_t channels = 3;

    uint64_t revision = 0;
    uint64_t contentHash = 0;

    std::shared_ptr<const std::vector<uint8_t>> pixels;
};
```

The payload should be immutable after publication.

A content hash is recommended for redundant-transfer detection and debugging but is not mandatory if the existing transport already provides source and revision identity.

### 14.5 Publication timing

Publish the complete raster:

* After a brush or eraser stroke ends
* After clear
* After undo
* After redo
* After successful patch restoration
* When a new Iris connection or source relationship is established
* When Iris explicitly requests the current source state, if supported

Do not publish once per brush stamp.

### 14.6 Thread safety

Do not allow Iris or an audio-thread callback to read from the mutable canvas while the UI is modifying it.

At commit time, create or update an immutable publication snapshot and atomically expose it through the existing transport.

Do not:

* Allocate large image buffers in `process()`
* QOI encode in `process()`
* Copy 786 KiB every audio sample or engine frame
* Pass mutable raw pointers across module lifetimes

The existing Nautiloid/Iris source mechanism should remain the transport authority. Extend it with a raster payload rather than creating an unrelated second communication system.

### 14.7 Iris persistence behavior

Iris may continue embedding its received raster through its existing image-embedding mechanism.

This deliberately permits both modules to retain their own compressed copy:

* Chromatide owns the editable original.
* Iris owns a self-contained snapshot of its active raster source.

This duplication is acceptable for the MVP because it gives both modules stable independent patch behavior.

If Chromatide is later removed, Iris should retain its embedded snapshot.

If Chromatide reconnects or republishes a newer revision, Iris should update from the authoritative Chromatide source.

---

## 15. Module Duplication and Ownership

Duplicating Chromatide must create an independent copy of:

* Canvas pixels
* Palette
* Brush settings
* Background and foreground colors

Editing one duplicate must not alter the other.

No static global image buffers may be used for module-instance document state.

Shared immutable publication snapshots are allowed only when their lifetime and copy-on-write behavior cannot couple edits between module instances.

---

## 16. Context Menu

Recommended context-menu actions:

```text
Reset canvas to black
Reset palette
Copy image data
Paste image data
Re-publish image to Iris
```

Only include copy/paste if a compatible existing Iris image encoding can be reused cleanly.

Context-menu reset actions must participate in undo where reasonable.

A future import action may allow PNG or QOI images to become editable Chromatide canvases, but file import is not required for the initial implementation.

---

## 17. Performance Requirements

Chromatide is primarily a UI module and must have negligible effect on Rack’s audio engine.

Required constraints:

* No per-pixel heap allocations during painting.
* No QOI encoding during painting.
* No full raster publication during every pointer movement.
* No large allocation or image conversion in `process()`.
* At most one preview texture refresh per UI frame.
* Preallocate RGB and RGBA canvas buffers.
* Reuse brush and undo scratch storage where practical.
* Use normalized-coordinate stroke interpolation.
* Avoid general-purpose resize operations for the compact preview.
* Let GPU/NanoVG scaling perform the static viewport transform.

Optional optimization after profiling:

* Cached brush masks by integer size
* Partial OpenGL sub-image uploads
* Compact preview cache
* SIMD RGB-to-RGBA conversion
* Background-thread QOI encoding during explicit patch save, provided Rack serialization requirements and object lifetimes permit it safely

Do not add those optimizations before establishing a correct baseline unless equivalent utilities already exist.

---

## 18. Error Handling

Chromatide must remain operational under malformed state.

Handle:

* Missing canvas JSON
* Invalid base64
* Invalid QOI
* Wrong image dimensions
* Unsupported channel count
* Truncated image data
* Overlay owner destruction
* Iris removal during publication
* Module deletion while expanded
* Graphics-context recreation

Failure should result in a safe black canvas or disconnected source state, never undefined memory access.

---

## 19. Non-Goals for MVP

The first implementation does not include:

* Layers
* Selection tools
* Move/transform tools
* Text
* Lines, rectangles, or shape primitives
* Fill bucket
* Gradients
* Smudge or blur
* Arbitrary brush images
* Softness/hardness control
* Arbitrary canvas dimensions
* Zoom and pan
* Animation
* Frame sequences
* CV-controlled painting
* Continuous audio-rate image streaming
* PNG export
* External file watching
* Alpha-channel storage
* Procedural reconstruction metadata

The architecture should not prevent these later, but none should delay the core module.

---

## 20. Implementation Phases

### Phase 1 — Canvas foundation

Implement:

* `ChromatideCanvas`
* 1024 × 256 RGB8 storage
* Pixel access
* Clear operation
* Revision counter
* Unit tests for indexing and bounds

### Phase 2 — Compact rendering

Implement:

* Nautiloid-sized viewport
* Fixed transform
* Persistent texture
* Full-canvas preview
* Dirty tracking
* Hover cursor

### Phase 3 — Painting

Implement:

* Brush
* Eraser
* Brush size
* Opacity
* Stroke interpolation
* Antialiased elliptical raster stamps
* Eyedropper

### Phase 4 — History and controls

Implement:

* Undo
* Redo
* Clear
* Palette
* Tool state
* Destructive-action protection

### Phase 5 — Persistence

Implement:

* QOI encode/decode
* Base64 patch storage
* Versioned JSON
* Validation and corruption fallback
* Bit-exact round-trip tests

### Phase 6 — Expanded editor

Implement:

* Overlay lifecycle
* Shared canvas
* Shared brush state
* Same viewport aspect ratio
* Safe closure on module deletion

### Phase 7 — Iris integration

Implement:

* External raster payload
* Revision publication
* Immutable snapshots
* Initial connection publication
* Iris ingestion through existing raster path
* No boundary resampling
* Snapshot retention when Chromatide is removed

### Phase 8 — Polish and profiling

Validate:

* Rendering responsiveness
* Patch size
* Memory use
* Undo budget
* UI consistency
* Multiple Chromatide/Iris instances
* Module duplication
* Overlay safety

---

## 21. Required Tests

### Canvas and transform

1. Top-left viewport coordinate maps to raster `(0, 0)`.
2. Bottom-right viewport coordinate clamps to `(1023, 255)`.
3. Center maps consistently in compact and expanded views.
4. A displayed circular brush remains circular in both views.
5. Raster brush geometry reflects the expected fixed ellipse.

### Painting

1. A 100% opaque stroke exactly replaces destination channels.
2. A 50% opaque stroke blends deterministically.
3. Fast strokes contain no visible gaps.
4. Eraser blends toward the configured background color.
5. Eyedropper samples exact raster values.
6. Brush painting cannot write outside the canvas buffer.

### Undo and redo

1. One stroke is undone exactly.
2. Redo restores exact bytes.
3. Clear is undoable.
4. New edits after undo clear redo history.
5. History correctly handles edge-crossing brush stamps.
6. History memory remains bounded.

### Persistence

1. Save/load produces a bit-identical RGB buffer.
2. Empty and flat-color canvases restore correctly.
3. High-entropy canvases restore correctly.
4. Invalid base64 falls back safely.
5. Invalid QOI falls back safely.
6. Wrong dimensions are rejected.
7. Palette and brush settings round-trip correctly.
8. Undo history is not restored.

### Expanded editor

1. Opening does not change canvas bytes or revision.
2. Closing does not change canvas bytes or revision.
3. Compact and expanded edits modify the same backing canvas.
4. Deleting the module closes the overlay safely.
5. Reopening displays all prior edits.

### Iris integration

1. Chromatide transfers exactly 1024 × 256 RGB8 data.
2. Iris receives byte-identical pixels.
3. No resize occurs during transfer.
4. Iris updates after stroke completion.
5. Iris does not update for every brush stamp.
6. Undo and redo publish new revisions.
7. Iris retains its embedded snapshot after Chromatide removal.
8. Reconnected Chromatide can republish its authoritative canvas.
9. Nautiloid’s fractal reconstruction behavior remains unchanged.
10. Multiple Iris consumers can receive the same committed image safely.

---

## 22. Acceptance Criteria

Chromatide is complete for MVP when:

* A user can paint fluidly in the Nautiloid-sized canvas.
* The brush appears circular despite the native 4:1 raster.
* The native backing image remains exactly 1024 × 256 RGB8.
* Compact and expanded editors operate on the same pixel buffer.
* Brush size and opacity behave consistently in both views.
* Palette, eraser, eyedropper, undo, redo, and clear work correctly.
* The canvas survives patch save/load losslessly through QOI.
* Chromatide can supply the image to Iris without resampling.
* Iris treats the received data through its existing raster-image pipeline.
* Nautiloid remains a distinct procedural source with reconstructible metadata.
* No image editing or serialization work disrupts Rack’s audio thread.
* Removing Chromatide does not destroy Iris’s embedded raster snapshot.
* Duplicated Chromatide modules remain fully independent.

---

## 23. Future Extension Points

The architecture should leave room for:

* Editable custom palette colors
* PNG/QOI import
* PNG/QOI export
* Fill tool
* Line and shape tools
* Softness and hardness
* Gradient tool
* Horizontal or vertical symmetry
* Tiling modes
* Noise and procedural brushes
* Selection and transform tools
* Image history browser
* CV-controlled color or brush parameters
* Throttled live-performance publication
* Alternate canonical view modes
* Direct visual comparison with Iris’s interpreted wavetable
* Shared image-source protocol for additional Leviathan visual generators

These should build on the authoritative raster and normalized-coordinate architecture rather than replacing it.

---

## 24. Final Architectural Summary

Chromatide is an editable, lossless raster source.

```text
Compact or expanded painting viewport
                ↓
Fixed normalized coordinate transform
                ↓
Authoritative 1024 × 256 RGB8 canvas
        ↙                       ↘
QOI patch serialization     Immutable raster publication
                                    ↓
                                  Iris
                                    ↓
                     Existing raster-to-wavetable path
```

The visible canvas is intentionally not a literal representation of the raster’s 4:1 geometry. Because the transform is fixed, it can be treated as a stable part of the instrument rather than as a recurring image-resize problem.

Chromatide owns the artwork. Iris consumes and may snapshot it. Nautiloid remains the special procedural source whose semantic parameters can reconstruct its image.
