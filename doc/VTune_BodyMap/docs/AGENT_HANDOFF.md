# Local coding-agent handoff: integrate V.Tune body highlights

Integrate the supplied kit into the existing Leviathan `expander` checkout. Read `README.md`, `docs/INTEGRATION.md`, and the current local `VTune.hpp`, `VTune.cpp`, `VTuneWidget.cpp`, `Vessel.hpp`, `Vessel.cpp`, panel assets, `PanelSvgUtils`, and `visual/VisualAssets` before editing. Preserve uncommitted work.

## Goal

Display soft pastel body-region highlights derived from Vessel's target/display frequency, using the supplied seated-body artwork. The seven broad source bands and optional solfeggio points are already encoded in `definition.json`, `BodyMapData.hpp`, and the registered PNG masks. Do not replace this with unsupported exact organ-resonance claims or a guessed complete chakra chart.

## Required local decision

Find the actual current body-image rendering location and its exact rectangle. The remote V.Tune widget version inspected while preparing this kit did not contain that placement. Reuse the current rectangle directly or add a real `VTUNE_BODY_IMAGE` rect anchor at those same bounds. Do not guess millimeter coordinates and do not move controls to make the kit fit.

Replace the old body draw with one `vtune_body::BodyMapWidget`. Remove a flattened/static body copy if it exists above or inside the panel assets. Preserve source aspect fit and draw order: black body backing, pastel masks, original transparent outline. Keep the dynamic widget outside a static framebuffer.

## Integration behavior to retain

Copy the kit's new source/header files and runtime resources. Apply or carefully merge `integration/VTune.integration.patch`; do not overwrite newer local whole files. Merge existing event/JSON overrides rather than creating duplicate definitions. Preserve parameter IDs and existing JSON fields.

Use `Vessel::visualFrequency` (or an already equivalent safe local target publication). Do not read neighboring modules from UI callbacks. Do not add FFT, audio capture, Markdown parsing, or Gaussian rasterization to the audio thread. Do not change the expander message schema or Vessel DSP for this display feature.

Keep the visual state smoothing on activation weights, not frequency. At 25 -> 1500 Hz, endpoint masks crossfade without an invented intermediate sweep. Out-of-range/invalid/disconnected values clear the map. Symbolic mode has finite ±70-cent support and must not choose a symbol for every frequency.

Use the existing raster-mipmap cache ownership contract and forward graphics context events. Do not hold stale or manually delete borrowed image handles. Preserve normal panel compositing by default, not additive/self-lit drawing.

Append strict-math flags for the three affected compilation targets from `Makefile.additions.mk`. The new `.cpp` files live in root `src/`, so inspect the existing source wildcard before adding redundant source rules. Ensure the masks and outline are included in distribution.

## Validation and report

Run the kit's SDK-free core/asset tests. Build the actual plugin against the local Rack SDK and fix genuine API/compiler mismatches. Then execute the in-Rack checks in `INTEGRATION.md`, especially exact image alignment, no duplicate static image, source frequency semantics near binaural limits, module removal/bypass/browser/save/load cases, graphics context recreation, and UI performance.

Conclude with the files changed, real body rectangle/anchor used, build/test outputs, any deviations from the kit, and any tests not performed. Do not claim the kit's standalone or browser tests were a full Rack integration test.
