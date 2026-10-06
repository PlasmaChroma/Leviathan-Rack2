# Data Bender — 14 HP VCV Rack panel reconstruction

## Main files

**DataBender-14hp.svg** is the faceplate intended for use as a Rack panel. It retains the original title, labels, secondary button functions, printed trace routing, inverted stereo-output surround, Qu-Bit mark, and mounting-hole appearance. Knobs, buttons, LEDs and jacks are not baked into its visible artwork.

**DataBender-14hp-reference.svg** is a separate, fully assembled, all-vector reference. It includes static representations of the hardware, traced from the supplied photograph. Use this for visual comparison, not as the background behind live controls: doing so would duplicate the hardware.

**previews/DataBender-comparison.png** compares the supplied image, the assembled SVG, and the bare faceplate. The other preview PNGs are raster renders of the delivered SVG files.

## Dimensions and rendering

The panel is exactly **71.12 mm wide × 128.5 mm high**, with a matching `viewBox`. Its width is **14 × 5.08 mm = 14 HP**. The horizontal width converts to 210 Rack SVG pixels at 75 DPI. Use the millimetre coordinates rather than coordinates from a browser's 96-DPI rendering.

Visible artwork uses ordinary filled/stroked paths, rectangles, and ellipses. All lettering and the logo are outlined paths traced from the source; there are no font dependencies or font files. Neither panel embeds raster images. No masks, filters, clipping paths, external style sheets, or SVG `use` references are required.

The background is intentionally a nearly black panel, with slightly off-white artwork and a subtle perimeter edge. These are flat fills rather than complex gradients.

## Placement and editing

The faceplate includes an Inkscape layer named **components**, hidden by default. It contains **35 component-center guides**: 6 knobs, 7 buttons, 7 lights, 13 inputs and 2 outputs. Guide colors follow the Rack panel convention. Layer labels are supplied for Rack's `helper.py`; this reconstruction was not run through that helper or tested in a live Rack session.

`DataBender-layout.json` and `DataBender-layout.csv` contain every center in physical millimetres, source-image pixels, and Rack SVG pixels at 75 DPI. These are measured visible centers, not certified shaft, drill or mounting coordinates.

`DataBender-layout.hpp` supplies namespaced C++ position constants with a `.px()` conversion method. It is an optional convenience header, not a complete module implementation. All four mounting-hole centers are included separately.

To use the faceplate, copy it into the plugin's resource directory and load it normally:

```cpp
setPanel(createPanel(asset::plugin(pluginInstance, "res/DataBender-14hp.svg")));
```

For component placement, use the matching millimetre coordinates from the layout files, or include the optional header and use a position such as `data_bender_panel::TIME_PARAM.px()`. Component/widget classes and module parameter enumerations remain those of your implementation.

## Reusable visual components

The `assets` folder contains eight standalone, all-vector component references:

- `DB-TimeKnob.svg`, `DB-RepeatsKnob.svg`, and `DB-SmallKnob.svg`.
- `DB-Button.svg` and `DB-Jack.svg`.
- `DB-LED-Blue.svg`, `DB-LED-Green.svg`, and `DB-LED-Off.svg`.

Knob assets are rotated approximately 150 degrees from the photographed pose toward an upward-neutral pointer position. Check the exact neutral/minimum angle against your knob widget's rotation convention. The assembled reference retains the original photographed poses.

These assets are visual resources, not complete stateful Rack widgets. Button travel, knob rotation behavior, light brightness/color changes, shadows and hit areas need to be implemented in the widget code. LED references have fixed colors; they are not dynamic light definitions.

## Accuracy and scope

The source supplied for this reconstruction is a **280 × 509 pixel** photograph. Typography is traced from that photograph rather than replaced with a similar font. The printed traces and output surround were reconstructed as clean geometry; hardware is reduced to smooth, filled vector color regions. This keeps the reference appearance close to the photograph without embedding it.

This is **not the manufacturer's original vector artwork**, and an exact match to an unknown factory master cannot be verified from this source. Small tracing irregularities, photographic perspective, parallax, uncertain hidden artwork, and subpixel placement differences remain. One source pixel corresponds to about 0.254 mm horizontally and 0.2525 mm vertically; numerical precision in the coordinate files does not imply matching measurement accuracy.

The files were rendered and inspected outside Rack. A live Rack integration and the C++ header were not compiled or runtime-tested.

This artwork is provided for the requested personal-use reconstruction. The original manufacturer artwork, product name and marks remain associated with their respective owners; this is not an official or endorsed panel asset, nor a grant of redistribution rights.

## Technical references

- VCV Rack Module Panel Guide: https://vcvrack.com/manual/Panel
- VCV Rack window/SVG units API: https://vcvrack.com/docs-v2/namespacerack_1_1window

The user's supplied image, not another product photograph, is the visual source for this reconstruction.
