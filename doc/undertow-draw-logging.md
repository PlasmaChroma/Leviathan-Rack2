# Undertow draw logging

Snapshot history is now the default for new instances and patches without a
saved tracer mode in Proc, Integral Flux, and Undertow. With Dragon King preview options enabled, legacy unversioned Curve cache
settings migrate to snapshots. New saves record `previewTracerCacheVersion=1`
so deliberate choices made with this build remain intact. Legacy Frame cache
and Snapshot cache choices remain intact. When
`PreviewWidgetOptions` is disabled, loading forces snapshot cache; Proc also
disables phosphor and Flux uses its existing NanoVG fallback. The menu
labels this choice **Snapshot cache (default)**. Earlier experiment notes below
record the rollout history.

Add `"UndertowDrawLogging": true` alongside `"debug": true` in the existing
Rack user configuration `Leviathan/dragonking.txt`, then restart Rack. On this
Windows setup that is `%LOCALAPPDATA%/Rack2/Leviathan/dragonking.txt`.
The repository-root example includes the flag disabled; it is not the live config.

Each Undertow instance writes a separate CSV under
`Leviathan/Undertow/undertow_draw_<instance>_<timestamp>_<sequence>.csv` in Rack's
user directory. Both flags are required. Disable the flag and restart to stop
logging. Rows flush every 32 draws; normal file closure flushes the remainder.

This is instrumentation only: no contour caching, simplification, capture
scheduling, DSP, parameter ID or saved-state behavior changed.

The CSV records monotonic time, full-width module ID, instance ID, latest module
step duration, module draw duration, ModuleWidget subtree duration, preview draw,
sample generation, point rebuilding, history drawing, contour simplification/path
submission, stroke submission and frequency-label formatting/drawing. It also
records rebuild counts, attempted and accepted captures, simplified point count,
tracer mode, shape/asymmetry settings, engine frequency and transform/display scale.
Undertow's current preview has no moving marker to time separately.

Timing interpretation:

- All durations are CPU microseconds, not GPU completion times or FPS.
- `step_us` and `draw_us` retain module-level scope. The existing Debug Terminal
  Process / Step / Draw metrics and process-stat consumption remain unchanged.
- Preview draw contains history, simplification/path submission, stroke and label
  work. Do not add those components to the enclosing preview or module totals.
- `simplify_path_submit_us` includes NanoVG move/line calls emitted by the existing
  simplifier callback. It is not a pure simplification benchmark.
- Sample/point rebuild timings and counters accumulate on that preview instance
  until its module draws, including any initial draw-time fallback rebuild.
- File opening/writing/flushing and Debug Terminal submission are outside recorded
  draw durations, but logging still adds overhead to the application. Keep the
  same flags enabled during before/after captures.

Suggested baseline: capture a stable contour, continuous shape modulation, and
a brief pause/resume of modulation at a fixed zoom. If using multiple modules,
each preview owns its counters; one module's draw cannot consume another's counts.

For the next capture, inspect median/p95 preview time, the simplification/stroke
split, accepted captures versus attempts, and the timed pause. Rendering changes
should follow this baseline rather than be mixed into the instrumentation pass.

Validation: native Windows plugin.dll linked; CSV header/row field counts match
(29). The four shape checks passed. The module suite passes 11 of 13 checks but
reports existing monophonic summary/fingerprint mismatches, reproduced both with
its previously built executable and after rebuilding. That harness includes the
unchanged Undertow.cpp DSP, not this widget/logger or plugin.cpp. No reference
expectations were changed. A live logging capture remains to be checked.

## Snapshot history comparison

With Dragon King preview options enabled, choose **Tracer Quality → Snapshot
cache (experimental)**. Mode 2 is snapshots; mode 0 remains stock vectors and
mode 1 remains the buffered frame cache. The default remains stock vectors.

Three appended CSV columns describe history workload:

- `history_trails`: number of visible stock-ring paths represented this draw.
- `history_source_points`: their total source polyline points. For snapshots,
  these describe image contents, not resubmitted vector geometry.
- `history_rasterizations`: actual snapshot slot framebuffer renders this draw.

The existing `history_draw_us` includes snapshot creation and compositing.
Compare rows with and without rasterizations, and include both in overall
median/p95. Buffered history has zero path/point counts because it is a raster
accumulation rather than a collection of discrete paths. Process/Step/Draw
retain their module-level meanings.

Capture Curve cache and Snapshot cache in the same patch at fixed zoom, with
morph modulation active, followed by a release and complete fade. Snapshot
rasterizations should stop during passive fade unless size, style, viewport or
context changes. Check the faint tail, zoom and DAW editor reopen visually.
