# Integral Flux rendering pipeline review

Reviewed 23 September 2026. This is a source review and a reading of existing measurements, not a new live profiling result. No renderer or patch behavior was changed.

## Current path

`IntegralFluxWidget::step()` steps the widget tree and collects module-level UI timing. Six Halo knobs can update their adaptive GL surfaces during this phase. `IntegralFluxWidget::draw()` then draws the panel, controls, two previews, lights, and overlays, and records the module-level Draw metric. The existing Halo accounting carries step-time surface work into the draw breakdown; Step and Draw component scopes can overlap and must not be added together.

Each default preview uses the NanoVG route. Its point array and simplified full/rise/fall paths rebuild when the audio-published preview version changes. Snapshot history draws behind the contour, which uses a `SettledContourFramebuffer`: it draws directly while changing, then caches after 100 ms of stability. The normal contour stroke tries `HostStrokeBridge` and falls back to an ordinary NanoVG stroke when admission fails. The moving dot and frequency label remain live overlays. OpenGL preview mode is optional and off by default.

Relevant code: [widget and timings](../src/IntegralFluxWidget.cpp), [contour settlement](../src/visual/SettledContourFramebuffer.hpp), [host stroke bridge](../src/render/HostStrokeBridge.hpp), [snapshot history](../src/visual/SnapshotHistory.hpp).

## Ranked opportunities

### 1. Decide the Eclipse2 cap and ring pilots from live evidence

Flux has four Eclipse2 attenuverter knobs. The retained cap and cached ring are independent, session-only debug A/B switches, both off by default. The cap has the larger measured changing-knob opportunity: the native probe reported roughly 1.19 ms for four baseline knobs versus 0.14–0.17 ms with the retained cap. The ring cache showed a smaller offline improvement, roughly 136–172 µs baseline versus 112–121 µs cached. These are CPU measurements in probes, not a demonstrated module Draw or frame-rate improvement. The cap and combined cache have visible resampling differences in image checks.

Run a same-patch live Rack comparison in alternating intervals: baseline, cap, cap plus ring, baseline. Capture idle and four-knob motion, module Draw median and p95, Eclipse component time, framebuffer rebuild counts, and frame rate. Check the cap and LEDs at several zoom levels, fractional placement, and after DAW editor close/reopen. Promote each switch separately only if appearance is acceptable and the whole-module result improves. Preserve the ordinary renderer as fallback for unsupported geometry or failed images.

Sources: [Flux debug switches](../src/IntegralFluxWidget.cpp), [retained cap](../src/visual/Eclipse2RetainedCap.hpp), [ring cache](../src/visual/Eclipse2RingCache.hpp), [experiment record](lumin-status.md#eclipse2-live-ab-experiment).

### 2. Observe the existing aperture improvements live

Flux contains eight aperture lights. The shared renderer already draws changing normal and bloom layers directly during its 100 ms settlement window and caches them after stability. The dynamic black crescent was removed, off-state work was reduced, and shared bloom masks are enabled by default on supported draws. A new blanket cache bypass would duplicate work already done.

The remaining useful question is whether the combined path lowers **live** module cost when several LEDs change. Compare changing, steady, and off states with the shared bloom-mask baseline override. Record module Draw and separate normal/bloom component costs; check visual behavior and context recovery. The prior offline probes support the direction but do not establish a live frame-rate gain.

Sources: [aperture draw and settlement](../src/visual/ApertureLight.cpp), [aperture investigation and promotion](lumin-status.md#aperturelight-investigation).

### 3. Complete component timing before judging further renderer changes

Flux's `aperture_draw_us` timer wraps the light's normal `draw()` call. Rack's separate light-layer bloom work is outside that component scope. Add a distinct bloom component metric if another aperture A/B is pursued, while retaining `Process`, `Step`, and `Draw` as module-level totals. Keep the existing Halo step-surface accounting and state explicitly that component timings can overlap macro timings. This is an observability improvement, not an optimization by itself.

Sources: [aperture timer](../src/IntegralFluxWidget.cpp), [Halo step/draw accounting](Leviathan-Ast-High.md#f08--integral-flux-drops-haloknob2-step-time-rendering-from-its-draw-breakdown).

### 4. Avoid needless redraws in optional OpenGL preview mode

The optional OpenGL preview calls `setDirty()` and `FramebufferWidget::step()` on every UI step, even when its points and dot have not changed. An invalidation key could cover curve version, dot position/visibility, tracer fade or expiry, highlight state, size, scale, and context. This is lower priority because the default NanoVG path does not take it. Measure idle and active behavior first; an incomplete key could leave a stale dot or trail.

Source: [preview step](../src/IntegralFluxWidget.cpp).

## Routes already screened

The default contour should remain on the host stroke bridge for now. A live return-switch capture found contour medians of 18.05 µs with the bridge versus 29.05 and 38.9 µs in flanking ordinary-stroke intervals, with zero bridge fallbacks. Whole-module savings were not isolated because module Draw rose across the capture. Analytic shader candidates through guarded and Rack-native GL were substantially more expensive for these small previews and were removed from the Flux runtime. Existing point simplification and snapshot history are deliberate, measured choices. Revisit them only with a new equal-work workload or visual requirement.

Source: [Lumin status and compact evidence](lumin-status.md#compact-evidence-record).

## Validation boundary

Integral Flux is released. Any implemented rendering change must preserve parameter IDs, saved preview settings, patch appearance, and fallback behavior. Validate with the native MINGW64 `plugin.dll` build and `test-fast` when source changes are made, then use live Rack/DAW checks for appearance, zoom, multiple instances, and editor context recreation. Compare whole-module CPU and frame behavior as well as component timings; a faster isolated draw call is insufficient if framebuffer or driver work moves elsewhere.
