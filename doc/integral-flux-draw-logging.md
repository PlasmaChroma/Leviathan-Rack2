# Integral Flux preview history logging

Enable `"debug": true` and `"IntegralFluxDrawLogging": true` in the existing
`Leviathan/dragonking.txt` configuration and restart Rack with the rebuilt plugin. CSVs are written under
`%LOCALAPPDATA%/Rack2/Leviathan/IntegralFlux/`, one file per module instance.

The history investigation appends these fields to the existing CSV schema,
with combined totals and corresponding `ch1_` and `ch4_` columns:

| Field | Meaning |
| --- | --- |
| `history_draw_us` | CPU time submitting the stock history paths, or drawing/updating the buffered history backend. |
| `contour_draw_us` | CPU time in current-contour rendering: direct strokes, cache creation/composite, or GL waveform submission. |
| `history_trails` | Number of stock history paths submitted during this draw. |
| `history_submitted_points` | Sum of stored polyline points submitted for those history paths. |

These are nested measurements within existing preview/module totals, not extra
work to add to those totals. Contour timing excludes the marker, label, and
construction of the cache key. Framebuffer timings overlap contour timing when
building a NanoVG contour cache. GL component timings cover offscreen submission;
they exclude final framebuffer compositing and are zero on draws that reuse its
image. All timings are CPU measurements, not completed GPU execution times.

Trail counters follow each renderer's existing age/alpha visibility checks.
GL points count source polyline points, not the expanded ribbon vertices.
Buffered history is a raster image: its trail/point counts are zero (not
applicable), even when visible. Use the existing renderer/tracer mode fields to
separate backends. The default NanoVG + stock tracer is the intended comparison
with Undertow and Proc. Additional instrumentation runs only with Dragon King
and Flux logging enabled. Existing Process/Step/Draw semantics are preserved.

## Next capture

Keep zoom and patch workload fixed. Using NanoVG and stock tracing, capture an
idle section, sustained shape modulation, then stop modulation and let the trails
fade completely. The existing two-instance patch can also provide a simultaneous
stable reference. Keep the controls/settings the same as the previous capture.

Compare history time against submitted points and trail count, per channel,
and compare the settled contour with modulation active. This determines whether
Flux corroborates Undertow's history bottleneck before changing the shared
tracer. Flux can test the common mechanism on Proc's behalf; eventual gains still
need a short Proc check because curve complexity and capture activity differ.

## Experimental snapshot history

With Dragon King debugging and preview widget options enabled, choose
**Tracer Quality → Snapshot cache (experimental)** and keep the preview renderer
on **NanoVG**. `preview_tracer_mode=2` identifies this mode in CSVs; Curve cache
remains mode 0 and Frame cache mode 1. OpenGL continues using its vector tracer.
Existing patches retain their modes; Flux can round-trip the appended mode 2.

The prototype keeps six Rack-owned framebuffer slots per channel and uses the
same stock capture ring, point simplification, draw order, line style and linear
alpha quantization. Each visible slot rasterizes on replacement or invalidation;
subsequent frames composite its image with the age-dependent alpha. Resize,
zoom, pixel density, style and Rack context events invalidate caches. Nested
framebuffer and unsupported transform drawing fall back to direct paths.

`history_draw_us` includes snapshot creation and compositing. The appended
`history_rasterizations`, `ch1_history_rasterizations` and
`ch4_history_rasterizations` report actual offscreen slot renders. During a
fade-only interval these should be zero, unless a resource/viewport change
invalidates a slot. `history_submitted_points` in snapshot mode describes the
source polyline complexity represented by the composites; it does **not** mean
those points were resubmitted as vector strokes. Existing preview framebuffer
timings cover the current contour/GL surfaces, not the new history slots.

Capture stock and snapshot modes using the same modulation and zoom. Include
release/fade, mode switches and zoom checks; visually check line thickness,
overlap and faint tails. Also check a DAW editor close/reopen before treating the
experiment as ready for general use. Compare median and p95 history/preview
costs, including frames with nonzero rasterizations.
