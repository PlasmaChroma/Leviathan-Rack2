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

### 1.1 Source-level ownership inventory

The following inventory is source-confirmed as of 2026-08-11. It is a starting
point for runtime verification, not a substitute for measured object counts.

| Surface | Current policy and owned resources | Current lifecycle behavior | Existing observability |
| --- | --- | --- | --- |
| HaloKnob2 `HaloGlSurface` | Independently cached per knob. Each surface owns one program, vertex shader, fragment shader, VBO, GPU cap-atlas texture, and Rack framebuffer. CPU cap pixels are shared through a weak cache while consumers retain them. | Explicit context destroy deletes local GL objects; context create abandons inherited names and lazily rebuilds; destructor abandons names without GL deletion. Extra `glIs*` validation is developer-config gated. | Thread-local GL/NanoVG framebuffer redraw time and count, debug-gated. |
| Bifurx spectrum GL surface | Module-level surface. Owns line, stroke, and texture shader pipelines, their VBOs, three curve textures, persistent CPU vertex arrays, and a legacy/local VBO field. | Explicit context destroy releases GL resources; context create abandons inherited names, resets renderer state, and dirties the surface; destructor abandons names. | Step/draw EMA, preparation timings, vertex count, fallback state, and debug-terminal ranges. |
| TD.Scope GL surface | Module-level display with line, segment, and field shader pipelines; one VBO per pipeline; three field-row textures; one color-LUT texture; persistent row/history/batch arrays. Dirty behavior is state-driven in product GL modes. | Context destroy releases/reset resources when Rack's GL context is current. Pipelines initialize lazily. No widget-local context-create override exists in the current source; ordinary framebuffer recursion and later lazy initialization carry restoration. Extra validation is developer-config gated. | Detailed CSV rows include total, setup, ingest, geometry, GL draw, row upload, field draw, state, validation, clear, viewport, fallback, and texture-upload fields. |
| Wyrm sand GL surface | Continuously live while a GL mode is visible. Owns a sand texture, wave-column texture, body shader program, body render-target texture/FBO pair, cached CPU body samples, and upload revision state. | Widget destructor abandons names. No widget-local context-create/destroy overrides exist in the current source. Optional developer-config draw-time validation resets invalid texture/program/FBO state. | Debug-gated total GL draw time published through module performance state. |
| Puffy fish/body cache | Live NanoVG composition rather than a custom GL surface. Shared cache owns context-bound final-body images, final-pointer images, and a transition atlas, plus context-free source pixels. With seven characters, the current arrays provide 49 body and 49 pointer slots. | Fish widget forwards context create/destroy. Shared cache tracks the active `NVGcontext*`, abandons handles on context change, and validates image dimensions before reuse. | Fine-grained body ensure/recolor/upload/draw, transition, fin, eye, cache, fallback, and context-reset metrics. |
| Puffy transfer preview | Cached NanoVG curve framebuffer with persistent CPU curve/swarm arrays. Rebuild is driven by quantized amount/character/dynamics/size/time criteria. | Rack framebuffer lifecycle; explicit redraw only after curve rebuild or relevant size/state change. | Curve rebuild count/time and curve/display draw time. |

### 1.2 Source-derived resource expectations to verify

Before LRC sharing, one successfully initialized Halo GL surface is expected to
create:

```text
1 linked program
2 shader objects retained by the surface
1 VBO
1 GPU cap-atlas texture
1 Rack-owned framebuffer image/FBO path
```

Six compatible Halo knobs are therefore expected to own six copies of the
program/VBO/texture set. Runtime counters must confirm this expectation before
it is used as the experiment's “before” value.

Bifurx and TD.Scope own dynamic textures and buffers whose contents are
instance-specific. The inventory does not classify those objects as shareable
merely because their formats match. Wyrm's render-target pair is also inherently
instance- and size-specific.

### 1.3 Inventory unknowns requiring observation

- actual framebuffer GL object counts owned internally by Rack;
- retained shader-object behavior and driver memory after successful links;
- actual texture byte allocation including mipmaps/driver format choices;
- whether module-browser previews initialize the same GL resource paths;
- whether any current surface performs an unlogged rebuild after DAW reopen;
- event ordering when several consumers acquire the same future shared family;
- which existing metrics materially perturb p95/p99 behavior when enabled.

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

Record two variants:

- `B0_empty`: empty visible rack, no browser or menus;
- `B0_flux_idle`: one default Integral Flux, no cables, both Cycle controls
  off, no automation, mouse outside the module.

Purpose: establish host noise and confirm instrumentation overhead.

### B1 — Halo idle scaling

One, five, and ten Integral Flux instances, all visible and untouched.

Use default module state, no cables, both Cycle controls off, no automation,
and a fixed rack viewport that keeps every measured instance visible. If ten
instances cannot be simultaneously visible at the target zoom, define and keep
a lower zoom for the entire B1 before/after pair rather than scrolling.

Purpose: measure cached compositing and resource duplication without active
redraw.

### B2 — One active Halo

One default Integral Flux instance. Automate `RISE_1_PARAM` with a normalized
triangle from `0.0` to `1.0` and back at `0.5 Hz` for 30 measured seconds. Keep
the other five Halo parameters fixed at their defaults and keep the pointer
outside the module.

Purpose: isolate one cached control's active redraw path.

### B3 — Halo simultaneous automation

Automate all six Halo parameters on one Integral Flux with normalized `0.5 Hz`
triangle waves. Phase-offset the six sources evenly by one-sixth of a cycle so
their value-boundary redraws do not all occur on exactly the same sample. Then
repeat with five visible instances using the same automation for corresponding
parameters.

Purpose: worst repeated analytic-surface workload and scaling.

The final comparison must use saved host automation or another repeatable
source. Manual multi-knob movement is exploratory only.

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

Begin from `B0_empty`. Insert ten default Integral Flux instances one at a time
through the same module-browser workflow, waiting two seconds between
insertions. Keep each new module visible until its first correct frame is
recorded. Record per-insertion time to first correct frame and cumulative
shader/program/buffer/texture creation counts.

Run a second variant that removes all ten in reverse order, then inserts one
more instance. This distinguishes warm shared-resource reuse from first-use
creation after all logical consumers have disappeared.

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

Use the C1 and C2 procedures in `LRC_Context_Resource_Core.md`. The Halo-focused
performance patch contains one Integral Flux for the minimal case and five for
the scaling case. The broader lifecycle audit patch additionally contains one
each of Bifurx, Puffy, TD.Scope, and Wyrm.

### B11 — Zoom, DPR, and rack pan

Use one Integral Flux with no automation and repeat at Rack zoom `75%`, `100%`,
`150%`, and `200%`. At each zoom, hold for five seconds, pan the module from an
integer-aligned resting position across at least one panel width over five
seconds, then hold for another five seconds. Repeat at DPR 1 and 2 where
hardware/host support is available.

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

### 5.1 Benchmark environment record

Every raw-data set and summary begins with this record. Unknown fields are
written as `unknown`; they are not silently omitted.

```yaml
run_id: YYYYMMDD-HHMMSS-scenario-build
scenario_id: B0..B11
scenario_revision: 1
code_revision: git hash or working-tree identifier
build_configuration: release/debug and relevant compiler flags
rack_version: exact version
plugin_format: standalone | VST3 | CLAP | other
host_name: standalone or DAW name
host_version: exact version
operating_system: edition and build
toolchain: MSYS2 compiler version and architecture
gpu: vendor and model
graphics_driver: exact version
display_resolution: width x height
os_display_scale_percent: integer percent
rack_zoom_percent: integer percent
framebuffer_pixel_ratio: measured value if available
monitor_refresh_hz: measured/configured value
audio_sample_rate_hz: integer
audio_buffer_frames: integer
module_counts: map of slug to visible instance count
renderer_modes: map of module to product/debug backend
dragon_king_debug: true | false
extra_gl_validation: true | false
logging_switches: explicit list
warmup_seconds: number
measurement_seconds: number
repetitions: integer
action_source: idle | manual drag | host automation | scripted UI
raw_log_paths: list
notes: free text
```

### 5.2 Summary result schema

One summary row represents one repetition, not an average of undocumented
runs.

```text
run_id,scenario_id,repetition,frames_observed,
frame_cpu_ms_avg,frame_cpu_ms_p95,frame_cpu_ms_p99,frame_cpu_ms_max,
surface_redraws,surface_redraw_cpu_us_avg,surface_redraw_cpu_us_p95,
program_creates,buffer_creates,texture_creates,resource_reuses,
resource_failures,context_rebuilds,fallback_frames,
first_correct_frame_ms,visual_failures,notes
```

Module-specific columns may follow this common prefix. Raw logs retain their
native high-resolution fields; the summary never discards the link to them.

### 5.3 Scenario artifact convention

Each repeatable patch or recipe receives:

```text
B<N>_<short-name>_r<revision>
```

The recipe records exact module slugs, counts, cable connections, parameter
values, automation source/rate, visible rack region, zoom, and the start/stop
action. If a Rack patch is stored outside the repository, the results document
records its stable location and checksum.

Manual mouse motion is acceptable for exploratory profiling but not for the
final paired Halo active-redraw comparison unless its path and duration can be
reproduced closely. Prefer host automation or a bounded debug-only UI driver
when manual variance obscures the result.

### 5.4 Readiness required for the first LRC experiment

Before D2 shared-program integration begins, the following artifacts must be
ready and dry-run once on the current local-resource renderer:

- `B0_empty` and `B0_flux_idle`;
- all B1 module-count variants;
- B2 saved automation;
- both B3 instance-count variants;
- B9 insertion recipe for Integral Flux;
- B10 C1/C2 context patches and procedures;
- B11 zoom/DPR viewport recipe;
- one environment record filled with real machine values;
- one raw log successfully reduced to the common summary schema.

B4–B8 may be completed before their respective dynamic-surface candidates are
evaluated. They do not block the initial Halo-only sharing experiment.

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
