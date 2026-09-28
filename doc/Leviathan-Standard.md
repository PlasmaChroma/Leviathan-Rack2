# Leviathan development standard

Current reference: 2026-09-26. This document collects the conventions used by the Leviathan VCV Rack plugin: module architecture, panel authoring, theme integration, shared rendering and DSP helpers, diagnostics, compatibility, and validation.

The governing repository instructions are in [AGENTS.md](AGENTS.md). This reference describes those rules and the current implementation. Module-specific designs still apply; an available shared helper is not evidence that every module already uses it. Timing values and implementation details below should be updated when their owning code changes.

## 1. General priorities and module structure

- Performance is a primary requirement in both audio and UI paths. Prefer bounded work, cached results, appropriate approximations, and shared implementations.
- Keep DSP/state ownership separate from presentation. The module owns audio behavior; widgets express user intent and render published state. Heavy decoding, file operations, and bulk preparation belong off the audio callback.
- Avoid allocation, deallocation, blocking locks, file access, network operations, and unbounded loops on the audio thread. Preallocate storage and arrange reclamation outside the callback.
- Do not assume audio callbacks continue while Rack is stopped or bypassed, or that a widget exists. Browser previews can construct widgets with a null module; headless module operation must not depend on the widget.
- State transitions must update all related public state consistently. Sample-rate changes, bypass, reset, cancellation, and teardown need explicit ownership and transition behavior.

Module declarations and registration live in [src/plugin.hpp](src/plugin.hpp) and [src/plugin.cpp](src/plugin.cpp); public module metadata lives in [plugin.json](plugin.json). Follow nearby modules when introducing a model or splitting module, engine, and widget files. The [Makefile](Makefile) collects top-level `src/*.cpp`, `src/visual/*.cpp`, and `src/theme/*.cpp`; other subdirectories may require explicit source entries.

## 2. SVG masters are the panel source of truth

For split panels, edit **`res/<name>.svg`**. The asset basename is not always the displayed module name: Temporal Deck uses `deck`, Integral Flux uses `flux`, and TD.Scope uses `tdscope`. Preserve the existing case and spelling.

Generated assets include:

| Asset | Purpose |
| --- | --- |
| `<name>.panel.svg` | Runtime foreground panel artwork and required layout data |
| `<name>.background.svg` | Extracted base background |
| `<name>.labels.svg` | Static labels and branding remaining after theme text extraction |
| `<name>.theme-text-input.svg` | Input and control labels, outlined for runtime |
| `<name>.theme-text-output.svg` | Output labels, outlined for runtime |
| `<name>.theme-text.svg` | Older single-role text layer, where still present |

Do not edit generated SVGs or `src/PanelAnchorAtlas.cpp` directly. Some editable masters are deliberately excluded from distribution, so runtime code must use packaged assets rather than rely on the master being installed.

### Inkscape organization

Use real SVG groups with stable IDs and readable Inkscape labels. For ordinary split panels:

```xml
<!-- The SVG root declares xmlns:inkscape="http://www.inkscape.org/namespaces/inkscape". -->
<g id="theme_background">
  <rect x="0" y="0" width="..." height="..." fill="#000000"/>
</g>
<g id="labels">
  <!-- Static title, description, and other authored labels -->
  <g id="theme_text_input" inkscape:label="Input Text">
    <!-- Control labels and input-jack labels -->
  </g>
  <g id="theme_text_output" inkscape:label="Output Text">
    <!-- Output-jack labels -->
  </g>
</g>
```

`Input Text` and `Output Text` appear in Inkscape's Objects panel. New labels placed inside a group inherit its theme role. Use unique IDs; preserve transforms and inherited styles when moving labels. Nested layout groups can remain inside the role groups.

The splitter still accepts `data-theme-text="input"` and `data-theme-text="output"` for compatibility. These custom XML attributes are not the preferred authoring convention because they are hidden from ordinary Objects-panel editing. The migrated masters now use named groups.

Titles, logos, and status/readout text retain their authored treatment unless deliberately assigned a role. Input Text includes control labels as well as input jacks; Output Text identifies outputs. A panel without output labels need not have an empty output group.

Some older masters contain hidden editable text and a visible outlined copy. Identify the visible artwork before changing it. Sil is an example: grouping its existing visible paths does not turn those paths back into editable font text.

### Background and semantic glass

The master base in `theme_background` is pure black (`#000000`). Background theming recolors that extracted base when enabled; foreground shading, screens, and decorative artwork are separate. Do not flatten the entire panel into a background-colored rectangle.

Semantic field groups such as `glass_input`, `glass_output`, `glass_text_input`, and `glass_text_output` are interpreted by the shared panel helpers. The older `glass_text` convention remains supported. These are panel-field roles, distinct from the glyph groups named `theme_text_*`.

The splitter neutralizes recognized semantic field pigment for runtime rendering while preserving authored master colors and achromatic highlights. Use the shared semantic discovery functions instead of guessing roles from arbitrary colors or painting another opaque field over the same area.

### Regeneration

For standard split panels:

```sh
python3 tools/split_svg_labels.py res/deck.svg --overwrite
make generate-panel-anchor-atlas
```

For legacy panels that retain their static labels/artwork in the panel layer, use the existing background-only workflow. Despite the historical flag name, it also extracts named theme text groups:

```sh
python3 tools/split_svg_labels.py res/sil.svg --background-only --overwrite
python3 tools/split_svg_labels.py res/bulkhead.svg --background-only --overwrite
make generate-panel-anchor-atlas
```

Use the workflow already appropriate to the module; do not blindly run the standard splitter over every master. Inkscape is used to outline runtime text so installed panels do not depend on local fonts. `--keep-label-text` is useful for tooling/tests, not the normal production output. Regenerate the atlas whenever SVG contents or anchors change, including grouping-only changes that alter asset hashes.

## 3. Layout and panel assembly

[PanelSvgUtils.hpp](src/PanelSvgUtils.hpp) is the shared layout interface. Put stable, descriptive control/port/rectangle anchors in the master's components or other established anchor layers, then query them rather than duplicating panel coordinates throughout C++.

- `loadPointFromSvgMm()` locates point anchors.
- `loadRectFromSvgMm()` locates rectangles such as display areas and branding bounds.
- `loadCircleFromSvg()` exposes circle geometry; respect its explicit unit scale.
- `findThemeGlassRectsMm()` and `findThemeGlassPathsMm()` discover semantic panel fields.
- `getAtlasStatusLabelForSvg()` supports layout diagnostics.

The `Mm` helpers return millimeters. Convert once with `mm2px()` for Rack widget placement. Follow the existing module's fallback coordinates and anchor naming; keep anchors and runtime assets consistent. Do not repeatedly parse layout in `process()` or `draw()`.

Use [visual/VisualAssets.hpp](src/visual/VisualAssets.hpp) for shared panel assembly. A typical constructor uses a scoped `SplitPanelRenderer`:

```cpp
setModule(module);
visual_assets::SplitPanelRenderer panel(this, "res/example.panel.svg");
const std::string& panelPath = panel.panelPath();
panel.addPerfectWaveBranding();
panel.addThemedLabels("res/example.labels.svg",
    "res/example.theme-text-input.svg", "res/example.theme-text-output.svg");
// Add displays and controls, using panelPath for anchor lookup.
```

The renderer inserts deferred labels when it leaves scope, after the constructor's controls and dynamic layers have been added. Keep that ordering intentional. Pass `nullptr` for an absent text role.

Modules assembling their own layers can use `createThemedPanel()` and `createThemedPanelLabelsWidget()`. `createThemedPanel()` expects an **absolute** panel path; `SplitPanelRenderer` takes a plugin-relative asset path. Do not resolve the same path twice. Use the shared themed panel path rather than introducing a plain `createPanel()` that bypasses background support.

## 4. Theme service and persistence

The implementation is in [src/theme](src/theme): `ThemeTypes`, `ThemeService`, `ThemePresets`, `ThemePersistence`, `ThemeUiPoller`, and `ThemedTextWidget`.

| Theme role/state | Meaning |
| --- | --- |
| `Input`, `Output` | Semantic input/output panel-field colors |
| `TextInput`, `TextOutput` | Directional label colors |
| `Background` and `backgroundEnabled` | Optional replacement of the extracted black base |
| `surface.textureAmount` | Shared surface texture setting |

Leviathan modules share an **in-memory** theme service within the plugin. `ThemeService::read()` copies state under a mutex; generation accessors read atomic counters. These are UI/control facilities, not audio-rate APIs. Rendering consumers should cache values, check the relevant generation, and refresh only when needed rather than acquire theme state separately for every glyph or sample.

`ThemeUiPoller` staggers polling over a 240 ms period with 12 phase slots. Related layers should use the module/widget owner consistently when supported. This spreads invalidations; it does not eliminate redraw work.

For native C++ labels, [ThemedTextWidget.hpp](src/theme/ThemedTextWidget.hpp) caches `inputTextColor` and `outputTextColor` and polls for updates in `step()`. When its drawing is inside a label framebuffer, set `textFramebuffer` so theme changes mark that cache dirty. If overriding `step()`, call the themed base implementation. See Umi, Mandelwake, and Wyrm for current examples.

The Theme module currently publishes global drag changes at most once per second, then publishes the final release value and saves it. Local picker feedback can update sooner. Persistence uses the user-storage file `Leviathan/theme.json`; normal Leviathan drawing does not read that file for each module.

The current persistence schema is V3. Migration supports earlier documents: V1's single text color seeds both roles, explicit newer fields override those fallbacks, and older files preserve original backgrounds. A future schema is preserved instead of overwritten with an older interpretation. Use the service/persistence APIs rather than writing a competing theme file format.

Leviathan-Pro has a separate in-memory service because it is a separate plugin. The follower path checks saved theme-file metadata on a shared 250 ms cadence and reloads when changed. It is not the same global object as Leviathan's service.

### Theme invalidation and Deep Cache

[DeepcacheThemeIdentity.hpp](src/DeepcacheThemeIdentity.hpp) hashes rendered theme values rather than process-local generations or preset names. Persistent preview identities must remain meaningful after restart; do not replace them with generation counters.

Theme invalidation applies to the exact plugin slug **`Leviathan`**. Leviathan-Pro's raster previews are intentionally excluded. Other plugins do not need Leviathan theme metadata to use the cache.

Deep Cache now waits **three seconds after the last observed theme change** before applying the refresh. This exceeds the picker's one-second publication interval and prevents a rebuild between ordinary drag updates. Stale disk/capture/commit results still require identity validation. Deferral reduces repeated work; it does not make the eventual refresh free.

Some current rendering caches still invalidate on the aggregate color generation even when only an unrelated role changed. Treat narrower invalidation as an optimization to implement and measure, not as a guarantee already provided everywhere.

## 5. Shared visual components and rendering

Reuse the controls and visual language in [VisualAssets.hpp](src/visual/VisualAssets.hpp): Eclipse/Eclipse2 knobs, Halo/HaloKnob2 controls, Magitek2 input/output jacks, gold and aperture buttons, switches, sliders, and orb/Torx screws. Preserve existing parameter ranges, orientation, gestures, and light behavior when changing a component.

`SmallGoldButton` supports per-instance cap colors through `setColor(nvgRGB(r, g, b))` on the UI thread. It derives shaded face, rim, glint, and socket colors once and invalidates only the affected framebuffers. Assigned alpha is ignored; the material and press animation own opacity. Unconfigured buttons retain the original gold palette exactly; `resetColor()` restores it. This API is inherited by `LoopGoldButton` and `SmallGoldApertureButton`; the aperture light's `setBaseColor()` remains independent of the cap color. For example, after `createParamCentered<SmallGoldButton>(...)`, call `button->setColor(nvgRGB(220, 55, 65))` before `addParam(button)` to give that instance a red cap.

Branding helpers support mirrored Perfect Wave anchors (`BRANDING_WAVE_LEFT_RASTER` / `BRANDING_WAVE_RIGHT_RASTER`), a solo anchor, and compact Leviathan logos. Prefer those helpers and SVG rectangle anchors to new per-module raster placement logic. [doc/branding.md](doc/branding.md) records the historical Rubik/Ancient typography; its older background palette predates the current pure-black base/theme workflow.

| Shared facility | Intended use |
| --- | --- |
| [RasterImageAssets.cpp](src/visual/RasterImageAssets.cpp) and `VisualAssets.hpp` raster APIs | Image decoding, mipmapped raster handling, aspect-fit placement, indexed-PNG compatibility |
| [SharedSvgCache.hpp](src/visual/SharedSvgCache.hpp) | Context-owned raster reuse for repeated SVG drawing, with zoom/pixel-ratio handling and fallback |
| [PreviewSurface.hpp](src/visual/PreviewSurface.hpp) | Shared preview interiors/grids and opaque separation from animated panel layers |
| [FractalGlassOverlay.hpp](src/visual/FractalGlassOverlay.hpp) | Shared fractal panel-field overlay for adopting modules |
| [ApertureLight.hpp](src/visual/ApertureLight.hpp), [ApertureLightTransfer.hpp](src/visual/ApertureLightTransfer.hpp) | Shared aperture-light rendering and brightness transfer |
| [PlasmaSwitch.hpp](src/visual/PlasmaSwitch.hpp), [PlasmaConduit.hpp](src/visual/PlasmaConduit.hpp) | Shared switch/conduit visuals |
| [AdaptiveGlSurface.hpp](src/visual/AdaptiveGlSurface.hpp) | Managed offscreen GL surfaces and optional constrained batching |
| [AdaptiveVisualUpdate.hpp](src/render/AdaptiveVisualUpdate.hpp) | UI frame-pressure tracking and staggered update gates |
| [SettledContourFramebuffer.hpp](src/visual/SettledContourFramebuffer.hpp) | Caching contours separately from live markers/text/history |
| [SnapshotHistory.hpp](src/visual/SnapshotHistory.hpp) | Retained tracer history using Rack-owned slot framebuffers |
| [WavePreviewTracer.hpp](src/WavePreviewTracer.hpp), [WavePreviewSimplifier.hpp](src/WavePreviewSimplifier.hpp), [WavePreviewGeometryKey.hpp](src/WavePreviewGeometryKey.hpp) | Reusable waveform-preview geometry work |

Keep static or slowly changing artwork in cached layers and live markers, interaction, and status in appropriate overlays. Mark caches dirty for changes that affect their pixels, size, scale, or graphics context. Avoid repainting a full panel merely because a small live indicator moved.

UI `step()` and visible `draw()` are different workloads. Avoid expensive work on every frame when a parameter revision, geometry key, or slower update gate can drive it. Adaptive visual degradation is opt-in; it must not change DSP or lose user input. These helpers are available building blocks, not a mandate to convert every preview to GL or adaptive rendering.

[RackVisualFramePressure.hpp](src/render/RackVisualFramePressure.hpp) supplies a shared UI-thread pressure sampler for participating modules. Reuse that adapter rather than letting each widget independently infer a different host frame rate.

The `lumin` rendering code under `src/render` includes specialized stroke and analytic-contour paths. [HostStrokeBridge.hpp](src/render/HostStrokeBridge.hpp) has a specific host/version and stroke-state contract; [FunctionCurve.hpp](src/render/FunctionCurve.hpp) explicitly identifies its analytic family as experimental and the Flux pilot as opt-in. Read the implementation and associated experiment notes before adopting these paths. Their presence does not replace the standard NanoVG/framebuffer path or establish universal backend compatibility.

## 6. Graphics ownership and host lifecycle

Follow [NvgGraphicsLifecycle.hpp](src/NvgGraphicsLifecycle.hpp), [GlLifecycleUtils.hpp](src/GlLifecycleUtils.hpp), and [GlResourceRetirement.hpp](src/GlResourceRetirement.hpp).

- NanoVG image handles belong to their creating `NVGcontext*`. Never delete a handle through a different context.
- On context replacement, clear or invalidate context-bound state and rebuild lazily. A cached integer handle is not proof that the underlying resource remains valid.
- Validate retained images with the shared size/context helpers before reuse. Use `updateOwnedNvgImageRgba()` and `resetOwnedNvgImage()` where appropriate.
- For GL, reuse program/buffer, shader-set, texture-set, and texture/framebuffer validity helpers. Keep the module-specific reset graph local.
- Acquire and respect context leases when using the shared retirement service. Retire objects through the established mechanism rather than issuing arbitrary GL cleanup from a destructor after context teardown.
- Preserve the host's graphics state around custom passes. `AdaptiveGlSurface` callbacks and batches have specific state contracts; read them before adding a new rendering backend.
- Treat DAW editor close/reopen, Rack preview contexts, resize, zoom, and pixel-ratio changes as ordinary lifecycle cases.

Prefer Rack-owned framebuffer widgets where they meet the need. Rendering an image successfully once is not sufficient lifecycle validation.

## 7. Shared math and DSP conventions

Avoid expensive per-sample `sin`, `cos`, `sqrt`, `pow`, `exp`, and related operations when a cached coefficient, bounded approximation, LUT, or established Rack helper provides the needed result. Offline tools, serialization, one-time setup, and correctness-sensitive calculations can favor precision.

### `MathHelpers.hpp` / `MathHelpers.cpp`

The `levi_math` namespace provides:

| Helper | Contract and purpose |
| --- | --- |
| `tanhLegacy()` | Established rational saturation curve; preserve it where existing sound depends on it |
| `fastTanh()`, `softClip()`, `softLimit()` | Compatibility behavior based on that legacy curve |
| `tanhAudio()` | Interpolated LUT approximation of mathematical tanh; a different transfer curve from the legacy saturator |
| `sinCyclesAudioBounded()` | Sine LUT for caller-guaranteed cycle inputs in `[-2, 2]` |
| `triangleCyclesAudioBounded()` | Bounded cycle-domain triangle helper with the same input-range contract |
| `clamp01()`, `smoothstep01()`, `wrap01()` | Common scalar shaping/wrapping helpers |
| `wrap01Fast()` | Single-period correction only; not arbitrary-range modulo |

New DSP should choose `tanhLegacy()` or `tanhAudio()` explicitly. Do not silently substitute one for the other in released modules. LUT storage is shared through `MathHelpers.cpp`; include the implementation in focused standalone tests that use those tables.

### `FastAudioMath.hpp`

These scalar helpers do not require Rack/SIMD headers and avoid allocation, lazy initialization, and libm calls:

- `exp2Audio()` and `exp2Pitch()` require finite arguments in `[-1022, 1022]`.
- `log2Audio()` requires a positive, finite, normal double.
- `powUnitAudio()` handles nonpositive bases as zero; positive inputs and the derived exponent still need to satisfy the underlying helpers' domains.

They are not unrestricted substitutes for the standard library. Validate or constrain inputs before hot loops. Tests should cover meaningful domain limits, numerical error, musical timing/pitch behavior, and required NaN/Inf handling.

Rack's established DSP/SIMD approximations are also used in the plugin. Reuse the appropriate scalar or SIMD implementation rather than inventing a similar approximation per module. The Makefile intentionally disables unsafe fast-math for selected correctness-sensitive translation units, including Chimera and Mandelwake paths; preserve those exceptions.

## 8. Thread handoff, snapshots, and persistence

[SpscLatestSnapshot.hpp](src/SpscLatestSnapshot.hpp) is a reusable latest-value transport for **exactly one producer and one consumer**. Payloads must be trivially copyable. Intermediate publications may be replaced, so this is appropriate for display snapshots, not lossless trigger/command delivery. The returned reference remains valid only until that consumer's next `readLatest()` call.

Choose queues/mailboxes according to the owning module's event semantics. Document producer/consumer counts, admission rules, overflow behavior, cancellation, and destruction. A pointer is not an ownership lease; workers must retain valid storage until completion, and old results must not overwrite newer state.

Prepare expensive changes off audio and adopt them through a bounded ownership transition. Snapshot consumers should see immutable data. Do not reclaim buffers while audio, save, display, or worker readers still hold them. Chimera's leased Reels and scratch-page snapshots are a specialized implementation; see [CHIMERA_SNAPSHOT_OWNERSHIP.md](doc/Morphagene-Codex/CHIMERA_SNAPSHOT_OWNERSHIP.md) rather than assuming it is a generic plugin-wide allocator.

Use defensive, version-aware serialization and sensible defaults for older patches. Validate external dimensions, counts, ranges, and file metadata before allocating or indexing. File replacement and cross-process access need an explicit persistence protocol; an in-process mutex does not protect two Rack processes. Preserve module-specific staged-save, lease, and recovery contracts.

The [Temporal Deck / TD.Scope drag contract](doc/drag-contract.md) is a concrete ownership example: TD.Scope reports intent while Temporal Deck interprets playback behavior. Do not generalize its particular lag/gesture fields into an unrelated module's protocol.

## 9. Diagnostics and performance measurements

Gate developer/debug functionality with `isDragonKingDebugEnabled()` from `plugin.hpp`. Reuse [DebugTerminalTransport.hpp](src/DebugTerminalTransport.hpp), [DebugTerminalMetrics.hpp](src/DebugTerminalMetrics.hpp), and [tools/debug_terminal/server.py](tools/debug_terminal/server.py) for external telemetry. Avoid doing socket or formatting work in the audio callback; publish bounded timing/state data for consumption elsewhere.

The first three performance metrics have a stable meaning and order:

1. **Process** — total module-level audio processing.
2. **Step** — total module-level UI stepping.
3. **Draw** — total module-level visible rendering work.

Refactors must preserve that meaning. Aggregate work split across widgets, framebuffers, overlays, and backends; do not relabel a cache or GL component measurement as the module total. Add component timings after these three. Keep shared control timing aggregation consistent with the existing Halo/Eclipse instrumentation.

Use `PreviewBuildLogTimer` and atlas status reporting where the module already follows that pattern. Report benchmark conditions, sample rate, workload, build flags, and whether measurements include UI/disk/host work. Offline elapsed time is not a live audio-device deadline guarantee.

## 10. Compatibility and validation

The explicitly released modules in repository guidance are **Integral Flux, Proc, Temporal Deck, TD.Scope, and Undertow**. Append new parameter/input/output/light IDs; do not reorder existing enums. Preserve patch serialization, defaults, and behavior relied upon by existing patches, including audible transfer functions and parameter mappings.

Crownstep, Bifurx, Wyrm, Sil, Chronomaw, and Bulkhead are explicitly listed as unreleased in the current guidance. That allows more flexibility, not permission to assume arbitrary state loss is acceptable. Check release status before making compatibility-sensitive changes to other modules.

Use focused regression tests for the affected behavior, then the appropriate integration/build checks. For panel/theme changes, useful checks include:

```sh
python3 tests/split_svg_labels_spec.py
python3 tests/panel_background_contract_spec.py
make generate-panel-anchor-atlas
make -j10 plugin.dll
```

Also run the relevant module's panel/anchor tests when available. Inspect rendered panels for position, clipping, duplication, role assignment, and branding. Before/after image comparison is useful for organization-only SVG edits. Native label behavior and context recreation still need suitable runtime validation; source/render checks do not exercise every host path.

`test-fast` is the routine broad suite; `test-rack` is still a work in progress. Native Rack-linked tests require the installed Rack runtime and matching DLLs:

```sh
make -j10 test-fast RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"
```

Keep the Rack application directory ahead of compiler runtime directories when launching linked tests. Report unrelated failures explicitly instead of claiming the whole suite passed. Historical review logs are evidence for their particular runs, not the current health of the branch.

Concurrency changes warrant targeted stress/sanitizer checks where feasible. The documented WSL TSAN address-layout workaround is in `AGENTS.md`; apply it only to the instrumented process. A sanitizer startup failure before `main()` is not itself evidence of a module race, and passing exercised paths is not proof of all possible interleavings.

## 11. Build and delivery workflow

This is primarily a **Windows VCV Rack plugin** project. Native MSYS2 **MINGW64** with the matching Rack SDK is authoritative for `plugin.dll`. A Linux compiler inside WSL is useful for focused tests but its `plugin.so` does not validate the Windows binary. Real Linux with an appropriate SDK can perform authoritative Linux builds.

From this machine's PowerShell environment:

```powershell
& C:\msys64\usr\bin\bash.exe -lc 'export MSYSTEM=MINGW64; export PATH=/mingw64/bin:/usr/bin; cd /home/Plasm/Leviathan && make -j10 plugin.dll'
```

For the WSL-to-Windows invocation and distribution details, follow [windows_build_from_wsl.md](doc/windows_build_from_wsl.md). Use the native compiler, not the generic MSYS compiler environment. Sandbox process-boundary failures may require execution approval; they do not establish that the toolchain is unavailable.

Preserve incremental objects during ordinary development. Reserve clean builds for a concrete reason. A built DLL, a distributable archive, and an installed plugin are distinct outcomes; do not report one as another. Include required generated resources and `plugin.json` in packaging.

Do not stage or commit changes: repository guidance leaves those actions to the user. Preserve unrelated working-tree edits. Update this document when shared conventions change, and keep module-specific exceptions near their implementation and tests.

## 12. Practical change checklist

- Identify the owning module/state machine and any released-patch compatibility constraints.
- Look for an existing shared control, math helper, transport, or rendering facility before adding another implementation.
- Edit master SVGs, preserve anchor IDs, and place functional labels in the visible theme groups.
- Regenerate the appropriate split assets and atlas; verify the runtime resource paths are packaged.
- Keep audio work bounded and UI work driven by changes; preserve graphics context ownership and cache invalidation.
- Keep `Process`, `Step`, and `Draw` telemetry meaningful across refactors.
- Run focused tests, inspect affected visuals, and perform the appropriate native plugin build.
- Record what was actually validated, any remaining runtime checks, and whether the result was merely built or also packaged/installed.
