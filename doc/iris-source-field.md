# Iris Canonical Source Field Persistence Specification

## Objective

Refactor Iris image persistence so the module no longer embeds the generated wavetable as the primary saved artifact. Instead, Iris must preserve a compact, deterministic, high-fidelity **canonical RGB8 source field** inside Rack patch storage.

The canonical source field becomes the single source of truth for:

* wavetable generation
* color-channel switching
* channel preview display
* reload behavior when the original image file is missing
* patch portability across machines

The embedded wavetable cache should be removed for newly saved patches. Wavetables should be derived at load/rebuild time from the canonical source field.

## Current Problem

Iris currently treats the wavetable as the portable embedded artifact. This preserves sound, but it loses the source image information needed to change color channel or accurately redisplay the original image-derived source later.

Current behavior has several limitations:

1. If the user changes image color channel after the original source file is deleted or unavailable, Iris cannot rebuild from the embedded wavetable.
2. Embedded source preview is only a display preview, not a high-fidelity rebuild source.
3. Disk import and embedded load are different conceptual paths.
4. `requestRebuild()` depends on the original file path.
5. The module stores `iris-table.bin`, which contains derived audio data rather than the preserved image material.

## Desired Model

Iris should always convert source images through this deterministic pipeline:

```text
external image file
  -> decode
  -> resample to canonical RGB8 source field
  -> store source field in module state
  -> build wavetable from canonical source field
  -> display canonical source field in widget
```

When loading from an embedded patch asset:

```text
embedded canonical RGB8 source field
  -> decode
  -> build wavetable from canonical source field
  -> display canonical source field in widget
```

This means that, after the first import, Iris does not need the original source image in order to rebuild or display the module consistently.

## User-Facing Requirements

### Required

* Iris must support a patch-portable embedded image source.
* Embedded source must preserve enough fidelity for high-quality wavetable generation.
* Embedded source must preserve RGB channels independently so color mode can be changed later.
* Alpha should be discarded.
* Iris must not save the wavetable itself for newly saved patches.
* Iris must still save source path/name metadata for user clarity and reload attempts.
* If embedding is disabled, Iris should attempt to load from the original file path.
* If loading from embedded source fails and disk source fails, Iris should fall back to the default oscillator shape.
* Loading from embedded source and loading from the original disk file must result in the same wavetable, provided both paths use the same canonical source-field generation process.
* The displayed image in the widget must come from the same canonical source field used to generate the wavetable.

### Not Required

* Do not preserve the full original source image.
* Do not preserve alpha.
* Do not preserve arbitrary original image dimensions.
* Do not make old external source files mandatory for new source-field patches.
* Do not keep `iris-table.bin` as a newly written patch artifact.

## Core Design

Introduce a new preserved artifact:

```text
iris-source.qoi
```

This file stores the canonical RGB8 source field as a lossless QOI image.

The canonical source field is:

```text
width: fixed Iris conversion width
height: fixed Iris conversion height / row count
channels: RGB only
bit depth: 8-bit unsigned integer per channel
layout: interleaved RGBRGBRGB...
alpha: discarded
compression: QOI lossless RGB8
```

The source field is not a thumbnail. It is not a preview. It is the actual canonical source material for Iris conversion.

## Recommended Canonical Dimensions

Add explicit constants in the Iris image/conversion layer:

```cpp
namespace iris {
constexpr int kCanonicalSourceWidth = 1024;
constexpr int kCanonicalSourceHeight = 256;
constexpr int kCanonicalSourceChannels = 3;
constexpr int kCanonicalSourceBitDepth = 8;
}
```

If the existing wavetable frame size or row count constants already define equivalent values, wire these constants to those values rather than duplicating magic numbers.

The goal is that each canonical source row maps naturally to one scan row / wavetable frame, and each canonical source column maps naturally to one phase sample domain before seam handling.

If the current implementation uses a `frameSize + 1` stride for wrapped wavetable samples, the canonical source field should still store only the non-wrapped image width. The duplicate seam/wrap sample belongs to the derived wavetable, not the source field.

## Fidelity Policy

Use RGB8 as the canonical format.

Rationale:

* The existing Iris import path already decodes to 8-bit RGBA, so RGB8 matches current behavior closely.
* RGB8 preserves independent red, green, and blue channels for later channel switching.
* RGB8 raw payload size is modest: `1024 * 256 * 3 = 786,432 bytes`.
* QOI is a small single-header dependency already vendored at `src/third_party/qoi.h`.
* QOI supports RGB8 losslessly, which matches the canonical source field exactly.
* QOI avoids zstd/miniz link and deployment issues while still shrinking many image-derived fields.
* The implementation stays simpler than general-purpose compression: no endian conversion for sample values, no 16-bit image path, and no external library to link.
* For higher-bit-depth source formats, decode through stb's 8-bit path unless there is a later, explicit quality reason to preserve more precision before quantization.

## Storage Codec Policy

Use QOI for persisted canonical source fields.

QOI policy:

* Encode with `channels = 3`.
* Use `colorspace = QOI_SRGB`. QOI does not transform colors based on this flag; it is metadata.
* Decode with requested channel count `3` so the result is canonical interleaved RGB8.
* Validate decoded width, height, and channels against Iris canonical dimensions.
* Treat QOI as a storage codec only. It must not alter conversion settings or display behavior.

Do not use JPEG or any other lossy compression. Do not add miniz, zlib, or zstd for this pass.

Implementation note:

* `src/third_party/qoi.h` is vendored.
* Add `#define QOI_IMPLEMENTATION` in `src/codec.cpp` before including `third_party/qoi.h`, alongside the existing single-header codec implementation defines.
* Keep the `QOI_IMPLEMENTATION` include at file scope, not inside `namespace temporaldeck` or `namespace iris`.
* Add small wrapper functions in the Iris IO/source-field layer rather than exposing QOI details through module code.

## New Data Structures

Create a new header/source pair if cleanest:

```text
IrisSourceField.hpp
IrisSourceField.cpp
```

Suggested structure:

```cpp
namespace iris {

struct SourceField {
  int width = 0;
  int height = 0;
  int channels = 3;
  int bitDepth = 8;

  // Interleaved RGB8 samples.
  // Length must be width * height * 3.
  std::vector<uint8_t> rgb8;

  // Metadata only. These are not used as canonical rebuild data.
  std::string sourcePath;
  std::string sourceName;
  int originalWidth = 0;
  int originalHeight = 0;
  int originalChannels = 0;

  bool valid() const {
    return width > 0 &&
           height > 0 &&
           channels == 3 &&
           bitDepth == 8 &&
           rgb8.size() == size_t(width) * size_t(height) * 3u;
  }
};

}
```

Add this to `Iris` state, protected by the same snapshot mutex pattern used for the table/preview state:

```cpp
iris::SourceField snapshotSourceField;
```

The active audio oscillator can continue using `ImageWavetable`. The canonical source field does not need to be consulted on the audio thread.

## New IO Format

Use standard QOI file bytes for `iris-source.qoi`.

Do not wrap QOI in a second custom binary header for this pass. Iris metadata that is not part of the canonical pixel field remains in JSON:

* source path
* source name
* original source width
* original source height
* original source channels
* conversion settings

QOI header fields provide persisted canonical width, canonical height, channel count, and colorspace.

This intentionally means there is no Iris-specific source-file version or CRC in the source artifact. QOI gives basic format validation and exact lossless RGB8 round trips, but it is not a strong corruption-detection container. If stronger validation is needed later, add an Iris wrapper or JSON-side digest in a separate format revision.

Validation:

* Read enough of the QOI header to validate width, height, and channel count before decoding the full payload.
* QOI decode succeeds.
* decoded width equals `iris::kCanonicalSourceWidth`
* decoded height equals `iris::kCanonicalSourceHeight`
* `channels == 3`
* decoded payload size equals `width * height * 3`
* reject malformed or truncated QOI payloads.
* reject dimensions larger than Iris canonical dimensions before allocation; do not allow QOI's much larger generic maximum to drive allocation size.

## JSON Changes

Increment Iris JSON version.

Current JSON stores conversion settings and source metadata. Keep those settings. Replace the existing embedded table fields with embedded source fields.

Add:

```json
{
  "version": 2,
  "embedSource": true,
  "embeddedSourceFile": "iris-source.qoi",
  "sourceStorageFormat": "rgb8-qoi",
  "canonicalSourceWidth": 1024,
  "canonicalSourceHeight": 256
}
```

Keep legacy compatibility with:

```json
{
  "embedTable": true,
  "embeddedTableFile": "iris-table.bin"
}
```

But do not write `embedTable` for new saves except optionally as a deprecated compatibility marker if absolutely necessary.

Legacy `embedTable` should only be read as a settings hint. Do not use a legacy embedded wavetable as a portable source fallback.

Recommended new JSON fields:

```cpp
json_object_set_new(root, "version", json_integer(2));
json_object_set_new(root, "embedSource", json_boolean(embedSource));
json_object_set_new(root, "embeddedSourceFile", json_string(kEmbeddedSourceName));
json_object_set_new(root, "sourceStorageFormat", json_string("rgb8-qoi"));
json_object_set_new(root, "canonicalSourceWidth", json_integer(iris::kCanonicalSourceWidth));
json_object_set_new(root, "canonicalSourceHeight", json_integer(iris::kCanonicalSourceHeight));
```

Preserve:

* `sourcePath`
* `sourceName`
* `sourceWidth`
* `sourceHeight`
* `rowCount`
* `conversion`

When loading `iris-source.qoi`, restore canonical pixels from QOI and restore user-facing/source metadata from JSON. Specifically, copy JSON `sourcePath`, `sourceName`, `sourceWidth`, `sourceHeight`, and source channel count metadata into the in-memory `SourceField` after QOI decode. The QOI file is the canonical pixel field only; it is not the source of original-file metadata.

## Import Pipeline

Replace the old direct path:

```text
stbi_load(path) -> buildWavetableFromRgba()
```

with:

```text
stbi decode path
  -> buildCanonicalSourceFieldFromDecodedImage()
  -> buildWavetableFromSourceField()
```

### Decode

Use stb image's 8-bit decode path:

```cpp
stbi_load(path.c_str(), &width, &height, &channels, 4);
```

This intentionally quantizes source images to 8-bit RGBA before canonical resampling. That matches the current implementation and keeps the first source-field pass small.

Alpha is decoded only if convenient, but discarded during source-field generation.

### Canonical Resampling

Add functions:

```cpp
bool buildSourceFieldFromImageFile(
  const std::string& path,
  SourceField* out,
  std::string* error);

bool buildSourceFieldFromRgba8(
  const uint8_t* rgba,
  int width,
  int height,
  int originalChannels,
  SourceField* out,
  std::string* error);
```

The resampler must be deterministic.

Recommended resampler:

* bilinear sampling
* center-of-pixel mapping
* clamp edges
* no gamma correction in first pass unless current conversion already explicitly assumes it
* no alpha premultiplication
* discard alpha

Mapping:

```cpp
srcX = (dstX + 0.5f) * srcWidth / dstWidth - 0.5f;
srcY = (dstY + 0.5f) * srcHeight / dstHeight - 0.5f;
```

For 8-bit input:

```cpp
rgb8 = bilinear result rounded to nearest uint8_t
```

Do not apply contrast, brightness, gamma, inversion, normalization, trim, smoothing, row order, or channel selection during source-field generation.

Those remain conversion settings applied later.

## Wavetable Build From Source Field

Add:

```cpp
bool buildWavetableFromSourceField(
  const SourceField& source,
  const ConversionSettings& settings,
  ImageWavetable* out,
  std::string* error);
```

This function replaces the core role of `buildWavetableFromRgba()` for all new paths.

Behavior:

1. Validate source field.
2. For each source row and pixel, convert RGB8 to normalized float RGB.
3. Apply selected image channel:

   * all/luma
   * red
   * green
   * blue
4. Apply brightness/contrast/gamma/invert as current code intends.
5. Apply row order.
6. Apply trim mode.
7. Apply normalization mode.
8. Apply DC removal.
9. Apply seam smoothing / wave smoothing.
10. Generate final `ImageWavetable`.

The exact existing conversion behavior should be preserved as much as possible. Codex should move shared conversion math into reusable helpers rather than rewriting it semantically.

Important: Once this lands, `buildWavetableFromRgba()` should either:

* become a compatibility wrapper that builds a temporary `SourceField`, then calls `buildWavetableFromSourceField()`, or
* be removed if no longer needed.

There must not be two divergent conversion implementations.

## Load/Rebuild Priority

Implement deterministic source priority.

### On module add / patch load

```text
if embedSource is true and iris-source.qoi exists:
    load embedded source field
    build wavetable from embedded source field
else if sourcePath exists and file can be loaded:
    import disk file to canonical source field
    build wavetable from source field
else:
    load default oscillator shape
```

For this new implementation, default oscillator fallback is required.

Do not load legacy `iris-table.bin` as a fallback. If a legacy patch only has an embedded wavetable and the original disk image is unavailable, Iris should use the default oscillator shape. This is an intentional simplification for this implementation.

### On image load from UI or drag/drop

```text
decode selected disk image
build canonical source field
store source field in snapshot state
build wavetable from source field
update source metadata
update display generation
```

### On rebuild after settings change

```text
if canonical source field is valid:
    build wavetable from canonical source field
else if sourcePath exists:
    import disk file into canonical source field
    build wavetable
else:
    load default oscillator shape
```

`requestRebuild()` must not require `sourcePath()` if a valid canonical source field exists.

### On reload image

Reload should mean:

```text
attempt to reload from original sourcePath
if successful:
    replace canonical source field with new disk-derived source field
    rebuild wavetable
else if existing canonical source field is valid:
    rebuild from existing source field and report reload failure non-destructively
else:
    default oscillator shape
```

## Save Behavior

Replace current save behavior.

Current new behavior:

```text
if embedSource enabled and snapshotSourceField valid:
    write iris-source.qoi
else:
    do not write source binary
```

Do not save `iris-table.bin` for newly saved patches.

If embedding is disabled:

* save JSON source path/name/settings only
* do not write `iris-source.qoi`
* on load, attempt disk source path
* if disk source unavailable, default oscillator shape

Suggested constants:

```cpp
const char* kEmbeddedSourceName = "iris-source.qoi";
```

Retire or keep only for legacy read:

```cpp
const char* kEmbeddedTableName = "iris-table.bin";
```

## UI / Context Menu Changes

Rename the context menu option.

Current:

```text
Embed wavetable in patch
```

New:

```text
Embed image source in patch
```

Behavior:

* checked by default
* toggles `embedSource`
* no longer mentions wavetable
* tooltip/status text may mention that the embedded source allows channel changes and portable patch loading

Recommended menu text:

```text
Load image...
Reload image
Clear image

Embed image source in patch ✓
```

Optional later addition:

```text
Source storage quality
  RGB8 QOI ✓
```

Do not add alternate quality modes as part of this task.

## Widget Display Changes

The module display must render from the canonical source field, not from a separately stored source preview.

Add:

```cpp
void Iris::sourceFieldPreviewSnapshot(
  std::vector<uint8_t>* rgb8,
  int* width,
  int* height) const;
```

This should copy or resample the canonical RGB8 source field into an RGB8 display buffer for NanoVG upload.

The display path should use:

```text
canonical source field -> RGB8 display buffer -> optional channel filter -> NanoVG image
```

The source image display should show the canonical source field in canonical top-to-bottom row order. `rowOrder` is a wavetable conversion setting and should not flip the source image display. If a later UI wants to preview row-order effects, that should be a separate derived view.

The converted waveform preview should still be generated from the current wavetable.

Remove or de-emphasize `sourcePreviewRgb` as persistent data. It may remain as a derived UI cache if useful, but it must not be saved or treated as source truth.

Display consistency requirement:

* If the same source file is loaded from disk and then saved/reloaded from embedded source, the source image display must match pixel-for-pixel after conversion to display RGB8.
* The widget should not display a different image depending on whether the source came from disk or patch storage.
* Channel preview filtering should operate on the canonical RGB8 display buffer and should not mutate the stored source field.

## Worker Request Refactor

Update `WorkerRequest`.

Current request types are path-centric and table-centric. Replace or extend them with source-centric types.

Suggested enum:

```cpp
enum WorkerRequestType {
  REQUEST_DEFAULT,
  REQUEST_IMPORT_IMAGE_FILE,
  REQUEST_REBUILD_FROM_SOURCE,
  REQUEST_LOAD_EMBEDDED_SOURCE,
  REQUEST_RELOAD_IMAGE_FILE
};
```

Suggested request structure:

```cpp
struct WorkerRequest {
  WorkerRequestType type = REQUEST_DEFAULT;
  std::string path;
  iris::ConversionSettings settings;
  iris::SourceField source;
  uint64_t serial = 0;
};
```

For rebuilds, avoid copying large `SourceField` unnecessarily if possible. Either:

* snapshot the source field under lock into the worker request, or
* use `shared_ptr<const SourceField>` for worker handoff.

Do not read mutable source field state directly on the worker without a snapshot/copy ownership strategy.

## Publication Model

Current `publishBuiltTable()` publishes the table and preview. Replace this with a publication model that can publish both source and table when needed.

Suggested result:

```cpp
struct WorkerResult {
  iris::SourceField source;
  iris::ImageWavetable table;
  bool hasSource = false;
  bool preserveExistingSource = false;
  std::string error;
};
```

Publication rules:

* Import from disk publishes both source field and wavetable.
* Load embedded source publishes both source field and wavetable.
* Rebuild from existing source publishes only wavetable, preserving source field.
* Default publishes default table and clears/invalidates source field unless product decision says default source should exist.
* Legacy embedded-table-only patches publish the default table if no source image can be loaded.

## Default Fallback

If all source loading fails:

```cpp
*built = iris::makeDefaultTable();
```

Status should not permanently show stale image metadata as if the image is available.

Recommended status text:

* `Loading...`
* `Image source embedded`
* `Image source linked`
* `Source unavailable; using default`
* `Load failed; using default`

The error light may briefly show or remain lit depending on existing product behavior, but audio output should safely return to the default oscillator shape.

## Backward Compatibility

Existing patches may contain:

```text
iris-table.bin
```

Do not use this as a source or sound fallback in this pass. Legacy table persistence cannot support channel switching, source display, or rebuild behavior, and keeping it in the load path preserves the old conceptual split this change is meant to remove.

Rules:

1. If `iris-source.qoi` exists, prefer it.
2. If no source exists but disk source path is valid, rebuild from disk and create source field in memory.
3. If no source exists and disk path fails, load the default oscillator shape.
4. On next save, if no valid source field exists, do not write a fake `iris-source.qoi`.
5. `loadBinaryTable()` may remain in the codebase temporarily if other cleanup work depends on it, but Iris should not call it from the new patch-load path.

This means old patches whose only portable artifact is `iris-table.bin` require the original source image path to still be available if they should reconstruct the Iris image-derived wavetable.

## File Layout

Patch storage should contain:

```text
iris-source.qoi
```

Do not write:

```text
iris-table.bin
```

except if retaining a temporary debug option behind a compile-time flag.

## Implementation Tasks

### Task 1: Add Source Field Model

Create:

```text
IrisSourceField.hpp
IrisSourceField.cpp
```

Implement:

* `iris::SourceField`
* validation helpers
* QOI encode/decode helpers for canonical RGB8
* RGB8 display-buffer generation

### Task 2: Add Source Field QOI IO

Create or extend `IrisIO`.

Add:

```cpp
bool saveSourceField(
  const std::string& path,
  const SourceField& source,
  std::string* error);

bool loadSourceField(
  const std::string& path,
  SourceField* out,
  std::string* error);
```

Implement:

* QOI encode with `channels = 3`
* pre-decode QOI header/dimension validation
* QOI decode requesting 3 channels
* canonical dimension/channel validation
* malformed/truncated file rejection
* size limits

### Task 3: Add Canonical Import

Add:

```cpp
bool importImageFileToSourceField(
  const std::string& path,
  SourceField* out,
  std::string* error);
```

This replaces direct file-to-table import as the first step.

Use stb image:

* `stbi_load`

Discard alpha.

Record metadata:

* source path
* source filename
* original width
* original height
* original channels

### Task 4: Add Deterministic Resampler

Implement one shared deterministic resampler for 8-bit RGBA source images.

Requirements:

* bilinear
* center-of-pixel mapping
* edge clamping
* nearest rounding to uint8
* no conversion settings applied
* no alpha influence

### Task 5: Convert Wavetable Build to Source Field

Add:

```cpp
bool buildWavetableFromSourceField(
  const SourceField& source,
  const ConversionSettings& settings,
  ImageWavetable* out,
  std::string* error);
```

Refactor existing conversion math so file import and embedded import use the same function.

Set `ImageWavetable` metadata from `SourceField`:

```cpp
table.sourcePath = source.sourcePath;
table.sourceName = source.sourceName;
table.sourceWidth = source.originalWidth;
table.sourceHeight = source.originalHeight;
table.sourceChannels = source.originalChannels;
```

The table’s `rowCount` and `frameSize` should reflect the generated wavetable, not the original image dimensions.

### Task 6: Refactor Iris Module State

Add:

```cpp
bool embedSource = true;
iris::SourceField snapshotSourceField;
```

Replace:

```cpp
embedTable
```

with:

```cpp
embedSource
```

Keep accessors with legacy names only if needed for minimal UI disruption, but rename public-facing APIs:

```cpp
bool embedsSource() const;
void setEmbedSource(bool enabled);
```

### Task 7: Refactor Worker Requests

Update `requestImageLoad`, `requestReload`, `requestRebuild`, `clearToDefault`, and `workerLoop`.

Expected behavior:

* `requestImageLoad(path)` imports disk image to source field, then builds table.
* `requestRebuild()` rebuilds from valid `snapshotSourceField` first.
* `requestReload()` attempts disk source path first, but does not destroy a valid embedded/current source on failure.
* `clearToDefault()` loads default table and clears source.
* `onAdd()` loads `iris-source.qoi` when available.

### Task 8: Refactor Save/Load JSON

Update `dataToJson()` and `dataFromJson()`.

Write:

* version 2
* sourcePath
* sourceName
* sourceWidth
* sourceHeight
* rowCount
* conversion settings
* embedSource
* embeddedSourceFile
* sourceStorageFormat
* canonical source dimensions

Read:

* version 1 legacy table fields
* version 2 source fields
* old `embedTable` as fallback to initialize `embedSource` if needed
* JSON source metadata into embedded-source `SourceField` after QOI decode

### Task 9: Refactor Patch Storage Save

Update `onSave()`.

New save behavior:

```cpp
void Iris::onSave(const SaveEvent& e) {
  Module::onSave(e);
  if (!embedSource) return;

  iris::SourceField source;
  {
    std::lock_guard<std::mutex> lock(snapshotMutex);
    source = snapshotSourceField;
  }

  if (!source.valid()) return;

  const std::string directory = createPatchStorageDirectory();
  std::string error;
  if (!iris::saveSourceField(system::join(directory, kEmbeddedSourceName),
                             source,
                             &error)) {
    WARN("Iris: failed to save embedded source field: %s", error.c_str());
  }
}
```

Do not call `saveBinaryTable()` for new saves.

### Task 10: Refactor Widget Display

Change source display code to consume canonical source field preview.

Update `Iris::sourcePreviewSnapshot()` or replace with:

```cpp
void Iris::sourceFieldPreviewSnapshot(
  std::vector<uint8_t>* pixels,
  int* width,
  int* height) const;
```

This should generate an RGB8 display buffer deterministically.

The display code can continue filtering channel preview in RGB8 space, but the source RGB8 buffer must be derived from canonical source field.

### Task 11: Update Context Menu

Replace:

```text
Embed wavetable in patch
```

with:

```text
Embed image source in patch
```

Wire it to `embedSource`.

### Task 12: Remove New Wavetable Persistence

Keep `loadBinaryTable()` and `saveBinaryTable()` only if needed during transitional cleanup or for tests unrelated to the new path.

* `loadBinaryTable()` should not be called by Iris patch loading.
* `saveBinaryTable()` should not be called by Iris new save path.
* Remove or mark `kEmbeddedTableName` legacy-only.

## Testing Requirements

### Unit Tests

Add tests for:

1. RGB8 source import stores canonical RGB8 correctly.
2. RGB8 source import preserves 8-bit channel values where no resampling changes them.
3. Canonical resampling is deterministic.
4. Source field QOI round-trip is exact.
5. Truncated or malformed QOI payload rejects file.
6. Invalid decoded dimensions reject file.
7. Invalid decoded channel count rejects file.
8. Alpha channel does not influence canonical source field.
9. `buildWavetableFromSourceField()` produces finite samples.
10. Embedded QOI load restores source path/name/original dimensions from JSON metadata.

### Consistency Tests

Create a test image fixture with known RGB gradients and hard color-channel regions.

Test:

```text
load from disk -> build source field -> build table A
save source field -> load source field -> build table B
assert A == B within exact float equality if possible, otherwise tiny epsilon
```

Because both paths build from the same canonical RGB8 field, exact equality should be achievable after the source field exists.

Frame equality around the original external image should be phrased carefully:

* `external image -> canonical source field -> wavetable` and `saved QOI -> canonical source field -> wavetable` should match exactly when the same conversion settings are used.
* Re-importing the external source file later should also match if the same stb decode path and deterministic resampler are used, but tests should focus exact equality on canonical source fields rather than treating arbitrary future image decoder behavior as the source of truth.

Test every channel mode:

* all/luma
* red
* green
* blue

Test relevant conversion settings:

* brightness
* contrast
* gamma
* invert
* normalize mode
* row order
* trim mode
* seam smoothing
* wave smoothing
* DC removal

### Widget Display Tests

At minimum, add non-UI helper tests:

```text
source field -> display RGB8 buffer
save/load source field -> display RGB8 buffer
assert identical bytes
```

Manual UI test:

1. Load colorful image.
2. Save patch with embed enabled.
3. Move/delete original file.
4. Reopen patch.
5. Confirm source display still appears.
6. Cycle color channel.
7. Confirm waveform changes.
8. Confirm channel preview display matches expected channel filtering.

### Fallback Tests

1. Embed enabled, valid `iris-source.qoi`: loads embedded source.
2. Embed enabled, missing `iris-source.qoi`, valid disk path: loads disk source.
3. Embed disabled, valid disk path: loads disk source.
4. Embed disabled, missing disk path: defaults.
5. Embed enabled, corrupted source file, missing disk path: defaults.
6. Legacy patch with only `iris-table.bin` and no available disk source: defaults.

## Acceptance Criteria

The implementation is complete when:

* New patches save `iris-source.qoi`, not `iris-table.bin`.
* `iris-source.qoi` contains lossless QOI-encoded RGB8 canonical source data with no alpha.
* `iris-source.qoi` can be decoded to exactly the canonical RGB8 source field.
* QOI dimensions are validated before full decode/allocation.
* Embedded source loads restore original source metadata from JSON.
* Wavetable generation always happens from the canonical source field for both disk imports and embedded loads.
* Changing image color channel after deleting the original image still rebuilds correctly.
* The widget source display is derived from the canonical source field.
* The widget source display remains in canonical row order; `rowOrder` only affects wavetable conversion and waveform preview.
* Loading from disk and loading from embedded source produce the same wavetable after canonicalization.
* If embedding is disabled, Iris still tries to load from disk.
* If embedded and disk load both fail, Iris returns to default oscillator shape.
* The context menu says “Embed image source in patch.”
* Legacy embedded wavetable fallback is removed from Iris patch loading; old patches require a valid disk source or default.
* No newly saved patch relies on embedded wavetable data.

## Suggested Implementation Order

1. Add `SourceField` struct and validation.
2. Implement source field save/load through QOI.
3. Implement disk image to RGB8 canonical source field.
4. Implement wavetable build from source field.
5. Convert `importImageFile()` into source-field import + wavetable build.
6. Refactor worker requests.
7. Replace `embedTable` with `embedSource`.
8. Replace save/load patch storage from table to source field.
9. Update widget display to source-field-derived preview.
10. Update context menu.
11. Add tests for source-field round trip, fallback behavior, and channel rebuilds.
12. Remove new-save dependency on `iris-table.bin`.

## Notes for Codex

Preserve existing sonic behavior as much as possible. The goal is not to redesign the image-to-wavetable conversion algorithm; the goal is to move the canonical input to a stable RGB8 source field.

Avoid creating parallel conversion paths. Disk import and embedded import must converge as early as possible:

```text
disk image or embedded source
  -> canonical RGB8 source field
  -> one shared wavetable conversion function
```

This is the central invariant of the task.
