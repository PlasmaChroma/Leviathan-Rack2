# V.Tune — frequency-driven body highlights

Implementation kit for Leviathan / VCV Rack. Prepared 2026-10-03 against the publicly retrievable `expander` branch interfaces, the supplied `Sound+Body.md`, and the supplied 788 × 1002 body outline.

**What is implemented:** frequency-to-layer evaluation, soft spatial masks registered to the exact uploaded image, a NanoVG/Rack widget, Vessel-to-V.Tune display telemetry, context-menu options and persistence, an integration patch, deterministic asset generation, standalone tests, and an offline interactive preview.

**What still needs local integration/validation:** bind the widget to your actual existing body-image rectangle, merge the patch with any newer local V.Tune changes, compile the plugin against your Rack SDK, and inspect it in Rack. The publicly fetched `VTuneWidget.cpp` had six controls and the expander light, but no body-image placement. This kit therefore does not invent panel coordinates or claim an in-host test.

Open `VTune_BodyMap_Preview.html` in a browser to try the actual baked textures, frequency bands, symbolic layer, transitions, and small-panel rendering. This preview is self-contained and makes no network requests. It is not a Rack screenshot or a substitute for in-Rack validation.

## 1. Meaning of the display

The supplied report is the requested content basis. Its “Synthesis: Frequency ↦ Body-Part Map” supplies the seven broad bands. It also mixes mechanical vibration, auditory responses, anecdotes, and traditional correspondences, without identifying research references or providing quantitative response curves. The implementation renders **the report's associations**; it does not calculate measured organ activity, anatomical resonance, exposure, treatment effectiveness, or safety.

The right-click mode names make that distinction visible:

| Mode | Behavior |
|---|---|
| Report regions (illustrative) | Default. Broad regions from the report's seven-band synthesis. |
| Solfeggio (symbolic) | Localized points near the report's explicitly named tones. |
| Combined (illustrative + symbolic) | Quieter broad regions with symbolic points on top. |
| Off (outline only) | No colored overlays. |

No complete Aparmita octave/chakra mapping is inferred: the supplied document gives only fragments. No gland-specific anatomical shape or special 174 Hz organ target is invented. The explicit list at source line 7 places 528 Hz at the solar plexus; the less consistent “heart/solar plexus” wording in the conclusion is not silently substituted. At 963 Hz, the upper-head motif is an artistic placement for the report's pineal/unity association, not an assertion that the document explicitly names a crown chakra.

### Report-region palette

The frequency bands and broad associations below follow source lines 11–23. Colors, relative strengths, spatial dimensions, and all interpolation parameters are authored UI choices rather than measured biological data.

| Frequency band | Screen regions used | Pastel |
|---|---|---|
| 20–30 Hz | Chest and abdomen, smaller eye/ear accents | Rose `#E7AEC5` |
| 30–50 Hz | Spine, core, legs | Peach `#EDC6A1` |
| 50–100 Hz | Larger muscle groups, arms and thighs | Butter `#E9DDA4` |
| 100–300 Hz | Throat, chest, abdomen and flanks | Mint `#A4DDC4` |
| 300–600 Hz | Head and upper neck | Periwinkle `#B1C0ED` |
| 600–1000 Hz | Upper/central head | Lavender `#D5B6E9` |
| 1000–2000 Hz | Ears with a softer head field | Ice blue `#A4DCEB` |

The first band is the report's “<30 Hz” category restricted to the requested 20–2000 Hz domain. Outside this domain, highlights fade out instead of assigning an unsupported region. At a shared boundary, neighboring report layers blend rather than competing for exclusive ownership.

The symbolic points are 396/root, 417/sacral, 528/solar plexus, 639/heart, 741/throat, 852/third eye, and 963/upper-head unity motif. A ±70-cent compact-support window is used only as a visual tuning tolerance. At an unrelated frequency such as 220 Hz, symbolic-only mode remains unlit; it does not force the nearest chakra onto every note.

## 2. Files and responsibilities

```text
src/
  VTuneBodyMapModule.cpp       Engine-side telemetry and JSON/event integration
  VTuneBodyMapWidget.cpp       NanoVG image compositing and context menu
  vtune/
    BodyMapData.hpp            Generated mapping constants and asset names
    BodyMapCore.hpp            SDK-free evaluator, animation, aspect-fit math
    BodyMapWidget.hpp          Rack widget interface
res/VTune/body-map/
  definition.json             Editable authoring definition + source provenance
  body_backing.png            Black body silhouette, transparent outside
  body_outline.png            Unmatted, transparent original outline
  band_0.png ... band_6.png   Seven registered pastel report masks
  tone_0.png ... tone_6.png   Seven registered symbolic-point masks
  opaque_reference.png        Audit/reference image; not drawn at runtime
  silhouette_debug.png        Audit image; not drawn at runtime
  asset_manifest.json         Asset hashes and dimensions
integration/
  VTune.integration.patch     Changes to the three existing V.Tune files
  Makefile.additions.mk        Strict floating-point flags and optional test target
reference/                    Original user-provided materials
tools/                       Offline authoring scripts; keep in this kit
tests/                       SDK-free C++ / asset / browser checks
docs/INTEGRATION.md           Exact integration steps and local acceptance checks
docs/VERIFICATION.md          Tests performed and explicit untested boundaries
docs/AGENT_HANDOFF.md         Local integration brief for a coding agent
```

The two new `.cpp` files deliberately live in `src/`, which the inspected plugin Makefile already includes using `$(wildcard src/*.cpp)`. The `src/vtune/` directory contains headers only. The existing resource packaging scans `res/`, so the new PNGs fall under the current resource inclusion pattern. Review this again if your local Makefile differs.

## 3. Frequency path: use the value Vessel already publishes

In the inspected code, `Vessel::visualFrequency` is an atomic display frequency. Vessel computes it from the applied pitch/fine/CV frequency, then constrains the binaural center so both separated frequencies stay within 20–2000 Hz. It is preferable to rebuilding the pitch from raw knob values or trying to detect a dominant partial from audio.

```text
Vessel controls + pitch CV
          |
          v
Vessel::visualFrequency       Existing published center frequency
          |
          v
VTune::updateBodyMapFrequency Engine-side neighbor check; about every 5 ms
          |
          v
VTune::bodyFrequencyHz       Own atomic, 0 means unavailable
          |
          v
BodyMapWidget::step          Evaluate region weights; smooth visual activations
          |
          v
BodyMapWidget::draw          Backing -> active pastel masks -> outline
```

The widget reads only its own V.Tune module's atomics. It never retains or dereferences a neighboring module from the UI thread. The audio-side bridge checks the neighbor's model and reciprocal expander connection before reading the existing atomic. There is no change to `TuneMessage`, its version, the tuning controls, Vessel's DSP, or the audio signal path.

This display represents the configured/displayed target, even when the bowl is silent or Vessel is output-muted. It is not energy-modulated, is not a spectrum analyzer, and does not reinterpret the binaural difference frequency as a body frequency. V.Tune bypass itself clears the map; a disconnected or invalid source fades to the outline.

## 4. Spatial rendering and scaling

The supplied PNG is **opaque**, including its black background. Drawing a glow below it would hide the glow. Drawing a glow above it would tint/dull the line work.

The kit instead draws these aligned layers:

1. A black body-shaped backing, transparent around the figure and in the gaps beneath the arms.
2. Pastel region textures with soft, precomputed alpha fields.
3. A transparent version of the original pale outline, with its black matte removed.

The original outline remains registered to its native 788 × 1002 source canvas. Each glow mask is 394 × 501, exactly half the width and height, with the same full-canvas registration. None is cropped or individually centered. All layers use one shared aspect-fit rectangle:

```cpp
const float scale = std::min(widgetWidth / 788.f, widgetHeight / 1002.f);
const float drawWidth = 788.f * scale;
const float drawHeight = 1002.f * scale;
const float drawX = (widgetWidth - drawWidth) * .5f;
const float drawY = (widgetHeight - drawHeight) * .5f;
```

This is the scaling invariant that keeps head, chest, hands, and legs lined up at Rack zoom and different widget sizes. Preserve the original image box when replacing the old widget. If your existing renderer deliberately stretches or crops the source rather than aspect-fitting it, either change that renderer to this shared transform or apply its exact transform to every layer—not just the outline.

The underarm holes are explicitly excluded from the silhouette. Inward feathering limits the highlight to the body; NanoVG rectangular scissoring is used only to restrict the widget bounds, not as a pretend body-shaped clip.

`BodyMapWidget` uses normal `draw()` compositing. These are soft panel colors, not additive light halos. Consequently Rack's normal panel dimming applies. A separate self-lit layer could be a later artistic change, but it should not be mixed into this default unintentionally.

The default backing is black. To show the panel surface through the interior instead, set `body->blackBodyBacking = false` when constructing the widget. The browser has a corresponding preview checkbox; this is not a saved Rack menu option in this implementation.

## 5. Smooth frequency behavior

### Band-edge crossfades

For each boundary `b`, define a smooth transition in log-frequency space:

```text
s(b, f) = smoothstep((log2(f / b) + 0.08) / 0.16)
w0 = 1 - s0
wi = s(i-1) - si
w6 = s5
```

Inside the supported frequency domain, report weights sum to one, are nonnegative, and at steady state use at most two neighboring bands. At exactly 300 Hz, for example, the 100–300 and 300–600 layers each receive 0.5. The full transition width is 0.16 octaves, about 1.92 semitones. This is a visual transition rule, not a measured tissue-response bandwidth.

### Animation

The evaluator's outputs are eased at the UI frame rate using a 90 ms time constant:

```cpp
const float amount = static_cast<float>(-std::expm1(-dt / 0.09));
current[i] += amount * (target[i] - current[i]);
```

A 90 ms time constant is not a hard 90 ms completion deadline; an exponential transition approaches its target over several time constants. Importantly, the **region weights** are smoothed, not the incoming frequency. A jump from 25 to 1500 Hz therefore fades between those endpoint regions without lighting the intervening muscles/throat/head bands as a spurious sweep. If the actual target frequency sweeps continuously, the map follows that sweep normally.

The default master opacity is 0.60, with menu choices 0.35 / 0.60 / 0.85. Combined mode applies report gain 0.55 and symbolic gain 0.90 before the master opacity. These values are aesthetic choices. Alpha-over compositing is not a calibrated brightness or physiological-strength scale.

## 6. Performance and context lifetime

No FFT, Markdown parser, image generation, mask rasterization, or Gaussian evaluation runs on the audio thread. The audio addition is a small periodic atomic telemetry copy. The UI evaluates at most six broad-band transitions and seven symbolic windows, then updates 14 weights.

All blur and silhouette clipping are baked into the PNGs. At settled report-mode frequencies the renderer issues one or two glow image quads, plus the backing and outline. More fading layers can remain active temporarily during jumps/mode changes. Layers below the draw epsilon are skipped.

Images are loaded lazily using the window image cache and Leviathan's `loadRasterMipmapHandle` helper. First use can allocate/upload a texture; subsequent frames reuse the shared per-context mipmapped textures. The widget forwards context-create/destroy events and does not retain or delete borrowed GL handles. This follows the fetched helper's ownership contract. Actual host/framebuffer behavior still needs local validation.

A full set of 15 half-resolution RGBA textures plus the native outline is about 14.3 MiB before mipmaps. The shared mipmapped copy would be roughly 19 MiB if all images are used in a given context, excluding driver alignment, the window's source-image cache, and any additional rendering contexts. Report-only use loads fewer masks. This is a deliberate small-image-memory tradeoff for avoiding per-frame blur/rasterization; these are storage estimates, not a measured GPU profile.

Keep the appended strict floating-point flags for the new module/widget code. A globally enabled fast-math mode can invalidate finite-value guards; the included Makefile additions follow the repository's existing target-specific strict-math convention.

## 7. Integration, at a glance

Read `docs/INTEGRATION.md` before applying. The essential sequence is:

1. Copy the new `src/` files and `res/VTune/body-map/` into your repository, preserving existing files.
2. Check and apply `integration/VTune.integration.patch`, or merge its small changes into newer local V.Tune code.
3. Replace the current static body draw with `BodyMapWidget` in **the same rectangle**. The patch looks for a newly proposed SVG rect anchor named `VTUNE_BODY_IMAGE`; it is not an existing anchor verified from your repository.
4. Append the strict-math Makefile rule, build, and perform the local acceptance checks.

If the body is already positioned by a `math::Rect` in your local widget, use that directly instead of adding an SVG anchor. If it is baked into the panel raster, remove that baked copy before adding the dynamic widget, or the original opaque art can cover the glows or leave doubled lines.

Missing anchor behavior is deliberate: it logs a warning and does not guess a body position that might overlap your controls. Your full local panel placement is the one required input that was not available from the fetched widget source.

## 8. Authoring and tests

The PNGs and generated header are already included; Python is **not** a runtime or normal plugin-build dependency. Keep this authoring kit separate from the plugin checkout to avoid collisions with generic tool filenames.

To edit colors, band definitions, or soft-region geometry, edit `res/VTune/body-map/definition.json`, then run from the kit root:

```sh
python3 -m pip install -r tools/requirements.txt
python3 tools/build_assets.py
python3 tools/build_preview.py
python3 tests/body_map_assets_test.py
```

`tools/create_definition.py` reconstructs the initial authored JSON and will overwrite edits; it is a reset/reference generator, not part of the normal edit cycle. Gaussian lobe coordinates are normalized to the original image canvas, not an anatomical coordinate standard. Changing the outline image requires checking/regenerating all masks, silhouette holes, and geometry.

Standalone core test, without Rack:

```sh
mkdir -p build
c++ -std=c++11 -O2 -Wall -Wextra -Werror -pedantic -Isrc \
    tests/body_map_core_test.cpp -o build/body_map_core_test
./build/body_map_core_test
```

C++11 and C++17 builds were both tested successfully. The test sweeps 100,001 frequencies and checks boundaries, invalid values, symbolic supports, modes, frame-rate independence, jump/disconnect fades, and aspect fitting. AddressSanitizer/UndefinedBehaviorSanitizer also passed for the standalone core. Asset tests and the interactive browser test passed; see `docs/VERIFICATION.md` for scope and limitations.

## 9. Inspected public interfaces

These URLs document the interface basis, not an assertion that your current checkout is identical. The branch is mutable and no exact repository commit was established.

- `https://github.com/PlasmaChroma/Leviathan-Rack2/tree/expander/src`
- `https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/refs/heads/expander/src/VTune.hpp`
- `https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/refs/heads/expander/src/VTune.cpp`
- `https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/refs/heads/expander/src/VTuneWidget.cpp`
- `https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/refs/heads/expander/src/Vessel.hpp`
- `https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/refs/heads/expander/src/Vessel.cpp`
- `https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/refs/heads/expander/src/PanelSvgUtils.hpp`
- `https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/refs/heads/expander/src/visual/VisualAssets.hpp`
- `https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/refs/heads/expander/Makefile`
- `https://raw.githubusercontent.com/VCVRack/Rack/v2/include/engine/Module.hpp`
- `https://raw.githubusercontent.com/VCVRack/Rack/v2/include/widget/Widget.hpp`
- `https://raw.githubusercontent.com/VCVRack/Rack/v2/include/window/Window.hpp`

The supplied source document and original image are preserved in `reference/`. Their inclusion is for traceability, not an independent verification of the report's claims.
