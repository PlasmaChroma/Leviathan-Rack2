# Integrating the V.Tune body-map kit

## Scope and starting assumptions

This adds a target-frequency visualization to V.Tune. It preserves the existing parameter IDs, expander control message, Vessel DSP, and tuning behavior. It does not supply a complete replacement plugin or assume the full source tree is present in this kit.

The fetched V.Tune widget source does not contain the body-image placement visible in your supplied artwork/use case. You must bind the renderer to your actual local image rectangle. Do not replace the current module layout with guessed coordinates.

The two root-level implementation files and the header-only core use C++11-compatible constructs. The standalone core was tested in both C++11 and C++17 modes. No global language-standard change is needed for this feature.

## A. Stage the additions

Start from your intended `expander` working tree and preserve any uncommitted work. Inspect `git status` before applying changes. Set `KIT` to the extracted directory and run the following from your plugin checkout:

```sh
KIT=/absolute/path/to/VTune_BodyMap
mkdir -p src/vtune res/VTune/body-map
cp "$KIT"/src/VTuneBodyMapModule.cpp src/
cp "$KIT"/src/VTuneBodyMapWidget.cpp src/
cp "$KIT"/src/vtune/BodyMapData.hpp src/vtune/
cp "$KIT"/src/vtune/BodyMapCore.hpp src/vtune/
cp "$KIT"/src/vtune/BodyMapWidget.hpp src/vtune/
cp -R "$KIT"/res/VTune/body-map/. res/VTune/body-map/

git apply --check "$KIT/integration/VTune.integration.patch"
# Apply only when the check succeeds and the diff matches your local intent:
git apply "$KIT/integration/VTune.integration.patch"
```

The provided patch was checked against a local reconstruction of the fetched three V.Tune files, not a cloned full repository or your working tree. If a hunk fails, merge the changes deliberately. Do not force-replace a newer `VTune.hpp`, `VTune.cpp`, or `VTuneWidget.cpp` with older source.

The patch does not add the new source/asset files by itself; the copy step is required. Do not copy the kit's entire root over your plugin repository. Its generic authoring-script filenames are intended to remain in this external kit.

### Existing local event/JSON implementations

If your newer local `VTune` already overrides reset, bypass, expander-change, or JSON methods, merge the body-map behavior into those methods. Do not define two versions of an override. Keep existing saved fields and behavior; add `bodyMapSchema`, `bodyMapMode`, and `bodyMapOpacity` to the existing JSON object. Keep the runtime frequency out of serialization.

The baseline source had no overrides for these functions. That is why this kit implements them in `VTuneBodyMapModule.cpp`.

## B. Bind the actual body rectangle

### Preferred when local code already places the body

Replace the existing static image widget at the same point in the child ordering, using the exact same box. When that existing box is already in Rack pixels:

```cpp
#include "vtune/BodyMapWidget.hpp"

// existingBodyBoxPx is the rectangle already used by your local body renderer.
// Reuse its real value; this name is illustrative, not a known repository field.
auto* body = new vtune_body::BodyMapWidget(module);
body->box = existingBodyBoxPx;
addChild(body);
```

When the existing rectangle is in millimeters:

```cpp
auto* body = new vtune_body::BodyMapWidget(module);
body->box = math::Rect(mm2px(existingBodyRectMm.pos),
                       mm2px(existingBodyRectMm.size));
addChild(body);
```

These are alternative placements, not additions to the patch's anchor placement. Create **one** body widget. Remove the old static body draw/widget, including any second outline layer above it. If the image has been flattened into a panel raster or SVG, remove that static artwork from the relevant panel asset as part of the local integration.

Keep the dynamic widget outside a permanently cached/static framebuffer. A normal child of `ModuleWidget`, added in the panel-art layer before controls and before the split renderer finalizes its label overlays, is the intended arrangement. No dynamic framebuffer invalidation machinery is needed.

### Anchor-based placement supplied by the patch

The patch calls:

```cpp
math::Rect bodyRectMm;
if (panel_svg::loadRectFromSvgMm(panel.panelPath(),
                               "VTUNE_BODY_IMAGE", &bodyRectMm)) {
    auto* body = new vtune_body::BodyMapWidget(module);
    body->box = math::Rect(mm2px(bodyRectMm.pos), mm2px(bodyRectMm.size));
    addChild(body);
}
```

`VTUNE_BODY_IMAGE` is a **new proposed anchor**, not an anchor confirmed in the fetched panel. Add a `rect` of that ID to the panel source used by `panel.panelPath()`, matching the current body's actual image box. Use the source SVG's native units and the repository's established anchor visibility conventions. The helper converts the result to millimeters. Do not treat its inputs as arbitrary screen pixels or introduce a second conversion.

If your panel pipeline caches/generated-atlases SVG anchors, regenerate the panel-anchor atlas using the repository's normal target after adding the rectangle:

```sh
make generate-panel-anchor-atlas
```

Check whether your local panel workflow also requires regenerating the split `.panel.svg` or associated raster before this step. Do not add the anchor only to an unused design/source file and assume it will be read at runtime.

Without the anchor, the module logs:

```text
V.Tune body map: add VTUNE_BODY_IMAGE at the existing body bounds; no position was guessed.
```

The controls still construct; the body map is deliberately not placed at a fallback coordinate.

### Registration rule

The kit uses the complete supplied 788 × 1002 canvas. Every mask and outline uses one common aspect-fit transform. A translated, differently cropped, mirrored, or stretched local source image will not match those masks unless that transform is applied consistently. The artwork is almost symmetric, but do not rely on that to conceal a transform mismatch.

The rectangular widget scissor is not anatomical clipping. Anatomical clipping is in the PNG alpha channel, including the two underarm negative spaces. Do not replace those masks with unbounded radial gradients and expect the same result.

## C. Preserve strict floating-point handling

Append the target-specific rule from `integration/Makefile.additions.mk` after the Rack SDK include:

```make
build/src/VTune.cpp.o build/src/VTuneBodyMapModule.cpp.o build/src/VTuneBodyMapWidget.cpp.o: FLAGS += -fno-fast-math -fno-unsafe-math-optimizations
```

The existing Makefile already globs root `src/*.cpp`, so no additional `SOURCES` rule should be required for these two root-level implementation files. If your local build uses a fixed source list instead, add both files to it.

To add the optional standalone test target, copy `tests/body_map_core_test.cpp` into the plugin's tests directory and include the rest of `Makefile.additions.mk`. The compile itself has no Rack dependency; parsing the plugin Makefile still requires its normal Rack SDK include. Alternatively, run the test entirely from this kit as described in the README.

The existing `find res` distribution rule includes these resources. The audit-only `opaque_reference.png` and `silhouette_debug.png` are not used by the widget and may be omitted from a final distribution. The runtime does not parse `definition.json`; its data is compiled into `BodyMapData.hpp` and baked into the masks.

## D. Verify the telemetry interface

The bridge assumes the inspected interfaces:

```cpp
// Existing Vessel field:
std::atomic<float> visualFrequency;

// Added V.Tune fields:
std::atomic<float> bodyFrequencyHz {0.f};
std::atomic<int> bodyMapMode {0};
std::atomic<float> bodyMapOpacity {vtune_body::kDefaultOpacity};
```

`VTune::process()` calls `updateBodyMapFrequency(args.sampleTime)` before its existing early return for an unready expander. About every 5 ms, the bridge checks the left module and reciprocal neighbor relationship, then copies `visualFrequency` using relaxed atomic operations. Zero encodes no valid mapped frequency in the same value as the frequency, avoiding a split linked/value UI snapshot.

No reference to the neighbor is retained across callbacks. The widget reads only the added V.Tune atomics. If your current local code already publishes the needed target frequency in V.Tune, reusing that safe publication is preferable to adding a duplicate bridge. Keep the semantic source consistent: displayed/applied target-center frequency, not raw pitch knob position, strongest audio partial, or binaural difference.

### Lifecycle behavior

| Case | Implemented behavior |
|---|---|
| Connected, valid 20–2000 Hz target | Evaluate and ease selected overlays. |
| Vessel silent or output-muted | Continue showing the configured target map. |
| V.Tune bypassed | Clear publication; overlays fade to outline. |
| Neighbor removed/replaced | Clear publication and force next telemetry update. |
| Unsupported/invalid frequency | Publish zero; fade to outline, no clamped endpoint highlight. |
| Module browser (`module == nullptr`) | Outline only. |
| Reset V.Tune | Default mode/opacity and cleared transient publication. |
| Old patch with no body fields | Default report mode and 0.60 opacity. |
| Known saved body-map fields | Restore mode/opacity with range/type validation. |
| Unknown body-map schema | Disable highlights rather than guessing the schema. |
| Graphics context change | Reset failed-load flags/timing and forward cache lifecycle events. |

The module browser path intentionally uses no made-up frequency. Normal Rack app/engine lifecycle behavior, JSON merging, and bypass interaction still require local acceptance testing.

## E. Build and in-Rack acceptance checks

Build with your existing supported toolchain and Rack SDK. For example, in your normal MSYS2 or Linux build environment:

```sh
make -j4 RACK_DIR=/absolute/path/to/Rack-SDK
```

This command is an integration instruction; a full plugin build was **not** performed while preparing this kit.

Then test these cases in Rack:

1. Place V.Tune immediately to the right of Vessel. Confirm all six tuning controls and link lights retain their old behavior. Confirm the body aligns exactly with the previous body box and no static copy remains.
2. Set approximately 25, 45, 75, 174, 450, 800, and 1500 Hz. Check the intended broad regions and that the underarm holes remain empty. At 300 Hz, inspect the two-layer blend.
3. Jump from 25 to 1500 Hz. Only the endpoint regions should crossfade; there should be no extra animated pass through each intermediate region. Continuously sweep pitch separately to check smooth band-edge motion.
4. Select symbolic mode. Check 396, 417, 528, 639, 741, 852, and 963 Hz. Check an unrelated frequency such as 220 Hz stays unlit. The ±70-cent support is an aesthetic tolerance, not an evidence claim.
5. Test fine tune and V/oct modulation. Enable binaural separation near the low and high pitch limits and confirm the visualization follows Vessel's displayed center rather than the beat difference.
6. Disconnect/reconnect, replace the left neighbor with a different module, bypass/unbypass V.Tune, reset, save/load, duplicate, and open the browser. Ensure no stuck glows or broken control exchange.
7. Inspect at low/high Rack zoom, different device scales, dark/light panel surrounds, and low room brightness. Close/reopen the Rack/VST window to test image-context recreation. Watch the log for repeated missing-image or missing-anchor errors.
8. Inspect performance with multiple V.Tunes. Watch initial texture upload behavior separately from steady rendering; compare audio CPU and UI frame time against the original module. No measured performance improvement is claimed by the kit.

Record the local Rack version, OS/toolchain, the body rectangle used, build result, and any host-specific changes. These details are what close the remaining gap between the standalone-tested implementation and a validated module integration.
