# LRC Baseline and Benchmark Plan

## Status

Ready for execution. This plan defines evidence collection; it does not
authorize renderer migrations by itself.

Parent charter: [Leviathan Render Core](LRC.md)

## Objective

Create a repeatable baseline that separates idle cost, active redraw cost,
resource duplication, initialization spikes, and context-recreation behavior.
All later LRC claims must name the scenario and metric from this plan.

## Questions this plan must answer

1. Which current surfaces dominate CPU preparation and framebuffer redraw time?
2. Which resources are duplicated per control, per module, and per context?
3. Where do p95/p99 stalls occur: normal frames, insertion, zoom, automation,
   or context restoration?
4. Which modules are continuously live, explicitly cached, or accidentally
   redrawing?
5. Which measurements are reliable in standalone Rack and in the target DAW?
6. What is the smallest benchmark set that remains repeatable enough for
   architectural go/no-go decisions?

---

## 1. Scope

Initial audit targets:

- HaloKnob2 in Integral Flux and one unreleased module;
- Bifurx OpenGL rendering;
- Puffy animation and transfer preview;
- TD.Scope active renderer;
- Wyrm sand rendering;
- one mostly static Leviathan rack as an idle control.

The audit records existing behavior. It does not normalize every module's
instrumentation during the first pass.

---

## 2. Required metric vocabulary

Use these names consistently in logs and result tables.

### Frame metrics

- `frame_cpu_ms_avg`
- `frame_cpu_ms_p95`
- `frame_cpu_ms_p99`
- `frame_cpu_ms_max`
- `frames_observed`

If Rack exposes a more precise host frame measure, record its source. Do not
mix Rack-reported FPS, widget CPU timers, and wall-clock frame duration in one
column.

### Surface metrics

- `surface_redraws`
- `surface_redraw_cpu_us_avg`
- `surface_redraw_cpu_us_p95`
- `surface_draw_calls`
- `surface_upload_bytes`
- `surface_fallback_frames`
- `dirty_value`
- `dirty_hover`
- `dirty_drag`
- `dirty_animation`
- `dirty_size_or_zoom`
- `dirty_context`

Metrics that cannot be observed cheaply may be marked `not instrumented`; they
must not be estimated and presented as measurements.

### Resource metrics

- shader compile and link attempts/successes/failures;
- live linked program count;
- live immutable VBO count and bytes;
- live immutable texture count and bytes;
- context generation/rebuild count;
- resource reuse count;
- abandoned-name count after a missed destroy path, if observable.

### Event metrics

- module insertion to first correct frame;
- context create to first correct frame;
- editor reopen to first correct frame;
- worst resize frame;
- worst zoom/rack-pan frame.

---

## 3. Instrumentation rules

- Detailed instrumentation is gated by `isDragonKingDebugEnabled()`.
- File logging or debug-terminal publication uses an additional explicit
  logging switch where the module already has one.
- Production-disabled instrumentation performs no file I/O, allocation, or
  high-resolution timing in ordinary frames.
- Audio-thread state publication remains lock-free and bounded.
- Percentiles are computed offline or outside the hot draw path.
- GPU queries must be asynchronous. If retrieving a result can block, GPU
  timing is omitted.
- Every log includes renderer/backend, Rack zoom, pixel ratio/DPR when known,
  module count, scenario name, build identifier, and host name.
- Warm-up and measured windows are separate.

---

## 4. Benchmark rack scenarios

Each scenario should have a saved patch or a precise construction recipe.

### B0 — Idle control

One representative static or cached Leviathan module, no automation, no live
scope, and no mouse movement.

Purpose: establish host noise and confirm instrumentation overhead.

### B1 — Halo idle scaling

One, five, and ten Integral Flux instances, all visible and untouched.

Purpose: measure cached compositing and resource duplication without active
redraw.

### B2 — One active Halo

One Integral Flux instance. Drag one HaloKnob2 continuously through its range
for a fixed interval.

Purpose: isolate one cached control's active redraw path.

### B3 — Halo simultaneous automation

Automate every HaloKnob2 on one instance, then repeat with several instances.

Purpose: worst repeated analytic-surface workload and scaling.

### B4 — Animated Puffy

Run Puffy in a repeatable signal state with its animation and transfer preview
visible. Capture idle-with-signal and actively changing character/amount cases.

Purpose: sustained NanoVG/raster activity and module-local instrumentation.

### B5 — Bifurx renderer

Run the default product renderer with repeatable input, then exercise controls
that force its expensive rebuild paths.

Purpose: module-level GL surface, multiple shader resources, and DAW context
behavior.

### B6 — TD.Scope renderer

Use the current product-default renderer only for the primary baseline. Debug
backends may be recorded as secondary comparison rows.

Purpose: sustained high-density display rendering and frame pacing.

### B7 — Wyrm sand

Record a representative live state and a state that triggers texture or
render-target rebuild.

Purpose: complex module-specific GL resources and internal framebuffer use.

### B8 — Dense mixed rack

Combine representative instances of the targets above at realistic zoom.

Purpose: user-level scaling and contention between independent surfaces.

### B9 — Insertion burst

Insert the same GL-backed module repeatedly into a visible rack, recording time
to first correct frame and shader/resource create counts.

Purpose: expose duplicate initialization and compilation spikes.

### B10 — Context recreation

In standalone Rack where possible, and authoritatively in the target DAW:

1. open the patch;
2. allow all visuals to settle;
3. close the editor;
4. reopen it;
5. resize once;
6. repeat at least ten cycles.

Purpose: correctness, rebuild cost, stale handles, fallback, and first-frame
latency.

### B11 — Zoom, DPR, and rack pan

Exercise representative supported zoom levels and DPR 1/2 where hardware is
available. Pan the rack without changing parameters.

Purpose: accidental invalidation, surface-size changes, and raster quality.

---

## 5. Run protocol

For each scenario:

1. Record build identifier, OS, GPU/driver, Rack version, host, window mode,
   zoom, DPR, sample rate, and debug switches.
2. Load or construct the scenario from a clean Rack start.
3. Warm for at least five seconds after the last insertion or context event.
4. Measure a fixed interval, normally 30 seconds for steady scenarios and a
   fixed event count for insertion/reopen scenarios.
5. Repeat at least five times; use more repetitions for noisy DAW data.
6. Retain raw rows and summarize median-of-runs plus the worst observed run.
7. Record any visual corruption, fallback, missed redraw, or interaction issue
   even if timings look favorable.

Do not compare results from different GPUs or host configurations as though
they were paired before/after data.

---

## 6. Baseline audit tasks

### Task A — Surface inventory

For each target, record:

- widget/surface type;
- cached, live, or rate-limited policy;
- surface dimensions;
- dirty causes;
- owned programs, buffers, textures, and FBOs;
- fallback renderer;
- context create/destroy behavior;
- existing metrics and logging controls.

Deliverable: a table in the eventual baseline results document.

### Task B — Fill instrumentation gaps

Add only the counters/timers needed to answer the baseline questions. Prefer
small module-local instrumentation over introducing LRC runtime code before the
architecture gate.

### Task C — Capture current Halo duplication

Verify resource counts for:

- one Halo knob;
- six knobs in Integral Flux;
- multiple Integral Flux instances;
- mixed normal and bright-orange configurations.

Expected current behavior must be verified rather than assumed from source.

### Task D — Validate context cycle logging

Confirm each target reports context recreation, resource rebuild, first correct
frame, and fallback activation without causing GL calls from unsafe teardown.

### Task E — Establish comparison report

Create `doc/LRC_Baseline_Results.md` only when real measurements are available.
It should contain raw-data locations, environment metadata, summary tables, and
known limitations. Do not pre-populate it with target percentages.

---

## 7. Acceptance criteria

This milestone is complete when:

- every required scenario has a reproducible patch or recipe;
- current Halo resource duplication is directly measured;
- idle, active, insertion, and context-reopen costs are not conflated;
- the Windows/MSYS2 standalone baseline is recorded;
- the target DAW context-cycle baseline is recorded;
- raw logs can be traced to summarized values;
- instrumentation-off behavior is checked for negligible overhead;
- visual failures are reported alongside timing results;
- the result is sufficient to accept or reject the Halo sharing experiment.

---

## 8. Environment guidance

WSL may be used for source checks, focused unit tests, log parsers, and object
compilation. It is not authoritative for final Windows plugin linking, driver
behavior, Rack frame pacing, or DAW context recreation.

Final measurements must be collected in the user's Windows/MSYS2 Rack
toolchain. DAW results must identify the host and editor configuration.
