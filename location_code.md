# Nautiloid Fractal Location Code — Implementation Specification

## Target

Implement this feature directly in the Leviathan VCV Rack plugin using GPT-5.6 Terra.

The implementation must compile cleanly with the existing project toolchain, preserve the current Nautiloid rendering and expander behavior, and include automated tests for the location-code codec.

---

## Objective

Add a compact editable text field to **Nautiloid** that exposes the current fractal viewport as a portable 16-character string.

The string acts as a rudimentary save/load mechanism:

* Users can select and copy the current string.
* Users can paste or type another valid string.
* A valid entered string immediately restores the encoded:

  * Fractal type
  * Zoom
  * Center X
  * Center Y
* The field automatically updates whenever the viewport changes.
* A green aperture LED beside the field indicates whether the current manually entered text is valid.
* Automatically generated strings must always be valid.

No external files, preset browser, or additional save/load buttons are required.

---

# 1. User Experience

## 1.1 Normal state

When the user is not manually editing the field:

* The field displays the canonical 16-character location code for the current Nautiloid viewport.
* The green validity aperture is illuminated.
* Panning, zooming, resetting, or selecting another fractal updates the displayed code.
* The user can click the field and use standard selection and clipboard operations:

  * `Ctrl/Cmd+A`
  * `Ctrl/Cmd+C`
  * `Ctrl/Cmd+V`
  * Arrow keys
  * Backspace/Delete

The code should be presented without padding or separators:

```text
EYl3Q6v5M2Lk8xRa
```

Do not include a visible prefix such as `N1:` in this first format. The format version is embedded in the encoded payload.

## 1.2 Manual input

As soon as the user modifies the text:

1. Validate the complete field contents.
2. If the string is valid:

   * Keep the validity aperture green.
   * Decode the string.
   * Immediately apply the decoded viewport.
   * Request a complete Nautiloid render.
   * Request regeneration of the Iris-compatible source.
   * Normalize the field to the canonical encoded form.
3. If the string is invalid:

   * Turn off the green validity aperture.
   * Preserve the text while the user continues editing.
   * Do not alter the current viewport.
   * Do not submit any render or Iris requests.

A pasted valid code should therefore load immediately without requiring a separate button or pressing Enter.

## 1.3 Cancelling invalid input

Invalid text is a temporary UI editing state, not module state.

When the field contains invalid text:

* Pressing `Escape` restores the code for the current viewport.
* Losing keyboard focus restores the code for the current viewport.
* The validity aperture returns to green.
* Invalid text must not be serialized into the patch.

This ensures that the green aperture can only remain dark while the user is actively editing an invalid code.

## 1.4 Viewport changes during manual editing

While the text field has focus and contains invalid manually entered text:

* Do not overwrite the text with automatic viewport updates.
* Continue tracking the latest viewport internally.
* On cancel or focus loss, replace the invalid text with the latest viewport code.

When the field contains valid text, ordinary automatic synchronization may resume.

---

# 2. Location-Code Format

## 2.1 Encoded size

Use an exact 12-byte binary payload encoded as unpadded Base64URL:

```text
12 bytes = 96 bits = 16 Base64URL characters
```

The string must always be exactly 16 characters.

Allowed characters:

```text
A-Z
a-z
0-9
-
_
```

Do not use ordinary Base64 `+` or `/`.

Do not include `=` padding.

## 2.2 Payload layout

Use this byte layout:

| Byte | Contents                                                  |
| ---: | --------------------------------------------------------- |
|    0 | Format version in high nibble; fractal mode in low nibble |
|  1–2 | Quantized zoom, unsigned 16-bit                           |
|  3–6 | Quantized center X, unsigned 32-bit                       |
| 7–10 | Quantized center Y, unsigned 32-bit                       |
|   11 | CRC-8 over bytes 0–10                                     |

All multi-byte integers must use **big-endian/network byte order**.

## 2.3 Version

Initial format version:

```cpp
constexpr uint8_t kNautiloidLocationCodeVersion = 1;
```

Byte zero:

```cpp
byte0 = uint8_t((version << 4) | (fractalMode & 0x0f));
```

The decoder must reject:

* Version zero
* Unknown versions
* Fractal modes not accepted by `iris::isBuiltinFractalMode()`

The existing built-in fractal mode IDs fit within four bits. Add a compile-time or test-time guard so a future built-in mode greater than 15 cannot silently truncate.

## 2.4 Zoom encoding

Nautiloid currently uses a zoom range of:

```text
0.0 through 4.0
```

Encode it into an unsigned 16-bit integer:

```cpp
uint16_t encodeZoom(float zoom) {
    const double normalized =
        std::clamp(double(zoom), 0.0, 4.0) / 4.0;

    return uint16_t(std::llround(
        normalized * double(std::numeric_limits<uint16_t>::max())
    ));
}
```

Decode using:

```cpp
float decodeZoom(uint16_t encoded) {
    return float(
        double(encoded) /
        double(std::numeric_limits<uint16_t>::max()) *
        4.0
    );
}
```

Use shared constants rather than duplicating the numeric `4.0` throughout the implementation.

## 2.5 Coordinate encoding

Nautiloid currently clamps both center coordinates to:

```text
-2.0 through +2.0
```

Encode each coordinate into an unsigned 32-bit integer:

```cpp
uint32_t encodeCoordinate(double value) {
    const double normalized =
        (std::clamp(value, -2.0, 2.0) + 2.0) / 4.0;

    return uint32_t(std::llround(
        normalized * double(std::numeric_limits<uint32_t>::max())
    ));
}
```

Decode using:

```cpp
double decodeCoordinate(uint32_t encoded) {
    return -2.0 +
        double(encoded) /
        double(std::numeric_limits<uint32_t>::max()) *
        4.0;
}
```

## 2.6 CRC

Use a deterministic CRC-8 implementation with:

```text
CRC-8/ATM
Polynomial: 0x07
Initial value: 0x00
Final XOR: 0x00
Reflection: none
```

Calculate the CRC over payload bytes `0–10`.

Place the result in byte `11`.

The decoder must reject a code whose CRC does not match.

Do not use `std::hash`, platform-specific checksums, or implementation-dependent byte representations.

---

# 3. Canonical Viewport State

## 3.1 Requirement

Every viewport state stored by Nautiloid must be representable exactly by its location code.

The code must not merely approximate an existing full-precision state at export time.

Instead, the module should store the decoded canonical representation of the quantized state:

```text
Requested state
    ↓
Clamp and validate
    ↓
Quantize to location-code integers
    ↓
Decode the integers
    ↓
Store decoded canonical values
    ↓
Render Nautiloid and Iris from those values
```

This guarantees that:

```text
current viewport → copied code → pasted code
```

reconstructs the same canonical state used to generate the original Nautiloid and Iris source pixels.

## 3.2 Canonicalization function

Provide a pure function similar to:

```cpp
Nautiloid::FractalState canonicalizeNautiloidFractalState(
    const Nautiloid::FractalState& requested);
```

Canonicalization must:

1. Validate or replace the fractal mode.
2. Clamp zoom and coordinates.
3. Quantize zoom, X, and Y using the location-code rules.
4. Decode those integers back to the runtime types.
5. Return the canonical state.

`Nautiloid::setFractalState()` must canonicalize before storing values.

The operation must be idempotent:

```cpp
canonicalize(canonicalize(state)) == canonicalize(state)
```

within exact equality of the resulting stored primitive values.

## 3.3 Existing patches

Existing JSON patch fields remain supported:

```text
fractalMode
fractalZoom
fractalCenterX
fractalCenterY
```

Do not remove or rename them.

When an older patch loads, `dataFromJson()` should continue passing the restored state through `setFractalState()`. This will canonicalize the old values to the new representable grid.

The resulting change should be visually negligible.

Do not add the generated location string to patch JSON. It is derived state.

---

# 4. Codec API

Create a small isolated codec unit. Preferred files:

```text
src/NautiloidLocationCode.hpp
src/NautiloidLocationCode.cpp
```

A header-only implementation is acceptable only if that better matches the existing project organization.

Suggested API:

```cpp
namespace nautiloid_location {

constexpr size_t kPayloadSize = 12;
constexpr size_t kEncodedLength = 16;
constexpr uint8_t kFormatVersion = 1;

struct DecodeResult {
    bool valid = false;
    Nautiloid::FractalState state;
    std::string error;
};

std::string encode(const Nautiloid::FractalState& state);

DecodeResult decode(const std::string& text);

Nautiloid::FractalState canonicalize(
    const Nautiloid::FractalState& state);

bool isValid(const std::string& text);

} // namespace nautiloid_location
```

Avoid a circular dependency between `Nautiloid.hpp` and the codec.

Acceptable solutions include:

* Moving the small viewport state struct into the codec header.
* Defining a codec-specific POD state and converting at the module boundary.
* Forward declarations where legal and maintainable.

Prefer the cleanest low-coupling solution.

## 4.1 Decoder rules

Before decoding:

* Remove leading and trailing ASCII whitespace.
* Do not remove whitespace from the middle of the string.
* Require exactly 16 characters after trimming.
* Require Base64URL characters only.
* Decode to exactly 12 bytes.
* Validate the version.
* Validate the fractal mode.
* Validate the CRC.

The decode operation must not throw for malformed user input.

Return an invalid result with a concise diagnostic string.

The diagnostic does not need to be shown on the panel in this task, but it should be available for tests and future tooltip support.

## 4.2 Canonical output

`encode()` must always return:

* Exactly 16 characters
* Unpadded Base64URL
* Current supported format version
* Correct CRC
* A code accepted by `decode()`

The following must hold:

```cpp
decode(encode(state)).valid == true
```

And:

```cpp
encode(decode(encode(state)).state) == encode(state)
```

---

# 5. Module Changes

## 5.1 Light ID

Add a new light:

```cpp
enum LightId {
    IRIS_LINK_LIGHT,
    IRIS_READY_LIGHT,
    INTEGRAL_FLUX_LINK_LIGHT,
    LOCATION_CODE_VALID_LIGHT,
    LIGHTS_LEN
};
```

Preserve the ordering of existing lights by appending the new ID before `LIGHTS_LEN`.

## 5.2 Validity state

Add a module-side atomic flag:

```cpp
std::atomic<bool> locationCodeInputValid {true};
```

The widget owns the editing state and updates this atomic flag.

The audio/engine thread owns light output:

```cpp
lights[LOCATION_CODE_VALID_LIGHT].setBrightness(
    locationCodeInputValid.load(std::memory_order_relaxed)
        ? 1.f
        : 0.f
);
```

Do not perform text encoding, Base64 processing, allocation, or UI work in `process()`.

## 5.3 State updates

Modify `setFractalState()` so all state changes are canonicalized before storage.

This should automatically cover existing state-change paths, including:

* Mouse-wheel zoom
* Zoom slider
* Zoom CV
* Display panning
* Reset view
* Fractal selection
* Patch restoration
* Location-code loading

Avoid adding separate quantization logic to every interaction handler.

## 5.4 Loading a code

Add a module helper such as:

```cpp
bool Nautiloid::loadLocationCode(
    const std::string& code,
    std::string* error);
```

On success:

1. Decode the code.
2. Store the decoded state through `setFractalState()`.
3. Request a render with the cache centered on the decoded position.
4. Ensure the Iris source is regenerated from the new state.
5. Return `true`.

Recommended render call:

```cpp
requestRenderWithCenteredCache();
```

The existing render submission already propagates requests to the Iris worker, so do not create a competing rendering path.

On failure:

* Do not mutate module state.
* Do not request rendering.
* Return `false`.

Add a corresponding helper:

```cpp
std::string Nautiloid::locationCodeSnapshot() const;
```

This should encode a consistent `fractalStateSnapshot()`.

The helper runs on the GUI thread, not the audio thread.

---

# 6. Text Field Widget

## 6.1 Widget class

Add a custom widget in `NautiloidWidget.cpp`, for example:

```cpp
struct NautiloidLocationCodeField final : ui::TextField {
    Nautiloid* module = nullptr;

    bool manualEditActive = false;
    bool currentTextValid = true;
    bool internalTextUpdate = false;

    int lastMode = -1;
    float lastZoom = NAN;
    double lastCenterX = NAN;
    double lastCenterY = NAN;

    ...
};
```

Use the current VCV Rack text-field API and follow existing text-field implementations in the repository where API details differ.

Do not implement a custom keyboard-input system from scratch.

## 6.2 Programmatic versus user changes

The widget must distinguish:

* Programmatic text replacement caused by viewport synchronization
* Actual user input

Use a guard such as:

```cpp
internalTextUpdate = true;
text = nextCode;
internalTextUpdate = false;
```

Programmatic updates must not trigger loading or mark the field as manually edited.

## 6.3 Automatic synchronization

In `step()`:

1. Take `module->fractalStateSnapshot()`.
2. Compare it against the widget’s last observed canonical state.
3. When the state changes:

   * Cache the new state.
   * Generate the new code.
   * Update the field unless invalid manual editing is active.

Encoding once per changed state is sufficient.

Do not allocate and encode continuously on every frame when the state has not changed.

## 6.4 User editing flow

On actual text change:

```text
Mark manual edit active
        ↓
Trim only surrounding whitespace for validation
        ↓
Attempt strict decode
```

If decoding fails:

* Set `currentTextValid = false`.
* Set `module->locationCodeInputValid = false`.
* Retain the entered text.
* Do not alter the viewport.

If decoding succeeds:

* Set `currentTextValid = true`.
* Set `module->locationCodeInputValid = true`.
* Immediately load the decoded state.
* Replace the field with the canonical `encode(decodedState)` result.
* Update the cached last-observed state.
* End invalid manual-edit protection.

Prevent recursive change handling when replacing the text programmatically.

## 6.5 Focus behavior

When focus is lost:

* If the text is invalid, restore the current viewport code.
* Set validity to true.
* Clear manual-edit state.

If the Rack API does not provide a direct blur event in the expected class, use the appropriate focus-lost event or detect loss of selected widget state in `step()`.

## 6.6 Escape behavior

When the field receives `Escape`:

* Restore the current viewport code.
* Set validity to true.
* Clear manual-edit state.
* Release or preserve focus according to normal Rack text-field behavior.

Do not reset the viewport.

## 6.7 Enter behavior

A valid code will already have loaded automatically.

For Enter:

* If valid, normalize the field and optionally release focus.
* If invalid, do nothing beyond preserving the invalid text and dark LED.

Do not submit an invalid viewport.

## 6.8 Length handling

Permit temporary lengths other than 16 while the user edits.

Do not forcibly truncate at 16 characters because that makes replacement and paste correction awkward.

A field longer or shorter than 16 characters is simply invalid.

Optionally set a generous defensive maximum such as 64 characters to prevent pathological pasted content.

---

# 7. Text Field Appearance

The control should visually belong to Nautiloid and Leviathan rather than appearing as an unstyled Rack browser field.

Required presentation:

* Dark inset background
* Subtle cyan/purple or neutral metallic border consistent with the module
* Single-line text
* Centered or left-aligned monospaced-looking presentation
* Enough width to show all 16 characters simultaneously
* No horizontal scrolling in the canonical state
* Clear text selection highlight
* Visible caret while editing
* No label or decoration obscuring selection and copy operations

Suggested display text size:

```text
10–12 px, adjusted to fit all 16 characters
```

Use an existing bundled/UI font. Do not add a new font dependency solely for this control.

The field itself should remain readable whether valid or invalid. The LED is the primary validity signal; do not make invalid text disappear.

A subtle invalid border tint is optional, but the green aperture behavior is mandatory.

Do not place the text field inside a framebuffer that prevents normal live caret, selection, or focus updates.

---

# 8. Green Validity Aperture

Place a small green aperture immediately beside the location-code field.

Preferred construction:

```cpp
createLightCentered<SmallAperture<GreenLight>>(...)
```

Use an existing Leviathan green-only aperture/light class if one already exists in the project.

Before creating a new light class:

1. Search the existing plugin source for green aperture implementations.
2. Reuse the closest visually compatible component.
3. Only add a new green aperture type if no suitable component exists.

Behavior:

| State                                   | Aperture                     |
| --------------------------------------- | ---------------------------- |
| Automatically generated code            | Green/on                     |
| Valid pasted or typed code              | Green/on                     |
| Invalid text while actively editing     | Off                          |
| Invalid edit cancelled                  | Green/on                     |
| Invalid field loses focus               | Green/on after restoration   |
| Module browser preview without a module | Prefer green/on if practical |

The LED should not flash off during ordinary automatic viewport changes.

---

# 9. Panel Integration

Add SVG anchors to the Nautiloid panel asset:

```text
LOCATION_CODE_FIELD
LOCATION_CODE_VALID_LIGHT
```

Use:

* A rectangle for `LOCATION_CODE_FIELD`
* A point for `LOCATION_CODE_VALID_LIGHT`

Load them through the existing `rectMm()` and `pointMm()` helpers.

Suggested fallback placement:

```cpp
const math::Rect locationCodeRectMm =
    rectMm(
        "LOCATION_CODE_FIELD",
        math::Rect(Vec(12.f, 92.f), Vec(73.f, 7.f))
    );

const Vec locationValidLightMm =
    pointMm(
        "LOCATION_CODE_VALID_LIGHT",
        Vec(90.f, 95.5f)
    );
```

These are fallback coordinates only.

Adjust the actual SVG placement to:

* Avoid the fractal display
* Avoid the zoom slider
* Avoid existing source/reset controls
* Avoid the zoom CV jack
* Avoid expander indicators
* Preserve comfortable cursor access
* Keep the complete 16-character field readable

A small panel label such as:

```text
LOCUS
```

or:

```text
FRACTAL CODE
```

may be added to the labels SVG if it fits the established panel language.

Do not bake editable text into the SVG.

---

# 10. Rendering and Threading Constraints

## 10.1 Audio thread

The audio thread may:

* Read the atomic validity flag
* Set light brightness

The audio thread must not:

* Allocate strings
* Encode or decode Base64
* Calculate CRC over user text
* Access UI widget state
* Take UI-owned mutexes
* Trigger clipboard behavior

## 10.2 GUI thread

The GUI thread owns:

* Text contents
* Text selection
* Clipboard interaction
* Manual-edit state
* Codec validation
* Calling the module load helper
* Automatic field synchronization

## 10.3 Existing workers

Do not modify the fractal rendering algorithms or add another render worker.

Loading a valid code should use the existing:

* Display worker
* Cache worker
* Reprojection worker
* Iris worker

Do not directly write into preview or Iris source buffers from the text widget.

---

# 11. Serialization

Continue storing the existing viewport values in module JSON.

Do not serialize:

* The generated location code
* Invalid manually entered text
* Keyboard focus
* Caret position
* Selection state
* Temporary validity state

On patch restoration:

1. Load existing fractal state.
2. Canonicalize through `setFractalState()`.
3. Render normally.
4. Let the widget derive the valid location code.
5. Initialize the validity aperture to green.

---

# 12. Automated Tests

Add focused tests using the project’s existing test infrastructure.

If no suitable test target exists, create a small codec-specific test target that does not require launching the Rack GUI.

## 12.1 Fixed-vector tests

Create fixed expected vectors for at least:

* Mandelbrot at zoom `0`, center `0,0`
* Maximum zoom at center `-2,-2`
* Maximum zoom at center `2,2`
* One non-Mandelbrot fractal
* A representative arbitrary viewport

Hard-code the resulting expected 16-character codes after independently checking the byte layout and CRC.

These vectors establish cross-version compatibility.

Once committed, the version-1 expected strings must not change.

## 12.2 Round-trip tests

For a broad deterministic set of states:

```cpp
const std::string code = encode(state);
const DecodeResult result = decode(code);
```

Assert:

* `code.size() == 16`
* Every character belongs to Base64URL
* `result.valid`
* The decoded state equals `canonicalize(state)`
* Re-encoding the decoded state reproduces the identical string

Include thousands of deterministic pseudo-random cases.

## 12.3 Corruption tests

For each bit or representative bits in the payload:

* Mutate the encoded code.
* Confirm CRC, version, mode, length, or alphabet validation rejects it.

At minimum test:

* Empty string
* 15 characters
* 17 characters
* Embedded whitespace
* `+`
* `/`
* `=`
* Invalid CRC
* Unsupported version
* Invalid fractal mode
* All-zero payload
* Arbitrary text

## 12.4 Canonicalization tests

Assert:

```cpp
canonicalize(canonicalize(state)) == canonicalize(state)
```

Test:

* Values below and above all ranges
* `NaN`
* Positive and negative infinity
* Invalid fractal modes

Define deterministic fallback behavior for non-finite values:

* Invalid zoom → `0`
* Invalid center coordinate → `0`
* Invalid mode → Mandelbrot

Do not feed `NaN` into rounding or integer conversion.

## 12.5 Iris pixel-equivalence test

The most important behavioral test is canonical pixel reproduction.

For representative states across every built-in fractal:

1. Canonicalize the state.
2. Generate an Iris source with `iris::makeNautiloidIrisSource()`.
3. Encode the canonical state.
4. Decode the code.
5. Generate another Iris source from the decoded state.
6. Compare:

   * Width
   * Height
   * Channel count
   * Bit depth
   * Entire `rgb8` buffer

The buffers must be byte-for-byte identical.

This verifies the actual purpose of the feature rather than merely checking numeric closeness.

## 12.6 State mutation test

Test that `Nautiloid::setFractalState()` stores canonical values.

For a noncanonical input:

```cpp
module.setFractalState(requested);
const auto stored = module.fractalStateSnapshot();
```

Assert:

```cpp
stored == canonicalize(requested)
```

---

# 13. Manual Verification Matrix

Verify the following in Rack:

| Action                              | Expected result                                                |
| ----------------------------------- | -------------------------------------------------------------- |
| Open a new Nautiloid                | Valid 16-character code; green aperture on                     |
| Pan display                         | Code updates continuously or at each accepted state update     |
| Mouse-wheel zoom                    | Code updates                                                   |
| Use zoom slider                     | Code updates                                                   |
| Apply zoom CV                       | Code updates as viewport changes                               |
| Change fractal type                 | Code updates and remains valid                                 |
| Reset viewport                      | Code returns to canonical reset value                          |
| Copy code                           | Clipboard receives exactly 16 characters                       |
| Paste same code                     | View remains identical                                         |
| Paste another valid code            | Fractal and viewport load immediately                          |
| Delete one character                | LED turns off; viewport does not change                        |
| Finish typing a valid code          | View loads; LED turns on                                       |
| Press Escape during invalid edit    | Current valid code restored                                    |
| Click elsewhere during invalid edit | Current valid code restored                                    |
| Save patch during invalid edit      | Invalid text is not saved                                      |
| Reload patch                        | View restores; field shows valid derived code                  |
| Connect Iris                        | Iris receives the same canonical state represented by the code |
| Paste code into a second instance   | Iris-compatible RGB source matches the first instance          |

---

# 14. Files Expected to Change

Likely files:

```text
src/Nautiloid.hpp
src/Nautiloid.cpp
src/NautiloidWidget.cpp
src/NautiloidLocationCode.hpp
src/NautiloidLocationCode.cpp
res/nautiloid.panel.svg
res/nautiloid.labels.svg
tests/... Nautiloid location-code tests
```

Adapt paths to the actual repository layout.

Avoid unrelated refactoring.

---

# 15. Compatibility Requirements

The implementation must preserve:

* Existing Nautiloid patch loading
* Current fractal selection behavior
* Current pan and zoom interactions
* Zoom CV behavior
* Reset behavior
* GPU preview behavior
* CPU preview behavior
* Tile caching
* Zoom-ahead caching
* Iris expander synchronization
* Integral Flux expander indication
* Existing debug logging
* Existing panel rendering architecture

Do not alter existing fractal formulas, palettes, dimensions, or iteration counts as part of this task.

---

# 16. Completion Criteria

The task is complete when:

1. Nautiloid shows a readable 16-character location-code field.
2. The field updates whenever canonical viewport state changes.
3. Valid pasted or typed codes load immediately.
4. Invalid input never changes the viewport.
5. The green aperture is dark only during active invalid editing.
6. Invalid text is discarded on Escape or focus loss.
7. The codec is versioned, checksummed, deterministic, and platform-independent.
8. Every stored viewport state is canonical and encodable.
9. Code round trips reproduce byte-identical Iris source pixels.
10. Existing patches continue loading.
11. All automated tests pass.
12. The Leviathan plugin builds successfully.
13. The implementation contains no audio-thread string processing or new rendering race.
14. Terra provides a concise implementation summary and records the exact version-1 fixed test vectors in the final response.

---

# 17. Implementation Discipline for Terra

Before editing:

1. Inspect the existing Nautiloid module, widget, panel anchors, light types, and test conventions.
2. Search the repository for existing uses of `ui::TextField`.
3. Search for existing green aperture components.
4. Confirm the built-in fractal mode IDs fit within four bits.
5. Identify the normal build and test commands.

During implementation:

* Keep the codec pure and independently testable.
* Preserve existing thread ownership.
* Use the existing state and rendering entry points.
* Do not invent a parallel viewport model.
* Do not silently weaken validation.
* Do not change the 16-character format after fixed vectors are established.

After implementation:

1. Build the plugin.
2. Run codec tests.
3. Run existing Nautiloid/Iris tests.
4. Verify the manual interaction matrix where feasible.
5. Report:

   * Files changed
   * Test commands
   * Test results
   * Fixed version-1 code vectors
   * Any remaining visual placement caveats
