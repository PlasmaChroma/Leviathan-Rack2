# Flux function history checkpoint — 10 September 2026

Parameter-based history now exists as an offline candidate. It retains each
captured curve's ratio, shape, Maths/Shark mode and birth time. Six slots occupy
208 bytes versus 6304 bytes for the stock point-history object in this native
build. This excludes raster textures and other renderer memory.

## Behavior and shader changes

The history matches production capture-before-update semantics, six-slot ring
order, minimum 1/24-second capture interval, 0.333-second lifetime, 118 maximum
alpha and integer alpha quantization. Mode belongs to each snapshot: switching
Maths/Shark does not reinterpret existing trails. A 120-frame contract comparison
against `WavePreviewTracer<128,6>` passes cadence, ring, timestamps, expiry and
fade checks. Clear/expiry checks pass. No live source path was modified.

The function shader now rejects pixels outside conservative support intervals
before its closest-point solve. The interval is derived from the maximum output
range that could intersect a stroke at that pixel; monotonicity bounds its X
extent. The 288-case appearance summary is unchanged after this optimization.
Trail width is 1.15, current contour width 1.4, on the same centerline/padding.

Three execution variants were measured:

1. Draw each analytic trail separately (initial screening).
2. Composite six trails and current contour within one shader submission per
   preview, retaining ring order and premultiplied source-over composition.
3. Cache each analytic trail image at capture, then fade/present the images;
   current curves remain shader-rendered in a common target. Current and dirty
   history targets enter GL under one `AdaptiveGlSurface::renderBatch` boundary.

The layered shader supports up to seven ordered layers. Each layer carries its
own shape/mode/color/width, so mixed-mode histories remain valid. No curve point
texture or per-shape vertex generation is used. Layered and cached variants remain
explicit probes, not promoted production recipes.

## Complete preview comparison

The baseline uses actual `FluxGeometry`, `WavePreviewTracer` capture/simplification
and **production `visual_assets::SnapshotHistory`**, including its framebuffers.
It is not an uncached redraw baseline. Cache telemetry verifies 306 rasterizations
for 5418 trail presentations per 460-frame benchmark run.

Two different curves modulate at a simulated 60 Hz, change Maths/Shark every
90 frames, and capture their previous state using production cadence. First 100
frames warm up; 360 frames are timed. Three rotated-order repetitions cover 1x
and 2x. Timings include preview stepping, point generation or parameter capture,
current curve rendering, history rendering/presentation, and host NanoVG flush.
GPU timing is separate. Metrics are named **CPU-preview-step/draw**, not the
module-level Process/Step/Draw contract.

Final three-way run, total CPU medians across repeats:

| Variant | 1x | 2x |
|---|---:|---:|
| Production cached history | 70.7–104.7 µs | 71.3–97.7 µs |
| Layered analytic history | 75.9–95.4 µs | 78.7–86.6 µs |
| Cached analytic history | 92.0–96.6 µs | 91.8–104.6 µs |

There is **no consistent total win**. Parameter preparation falls to roughly
0.6–0.7 µs in the cached variant, but drawing/presentation consumes the savings.
The previous contour-only 14–21% median benefit cannot be applied to this full
history case. Neither a GPU win nor better tail latency is established. These
runs are offline and timings vary; all raw repeats are retained.

The baseline current contour is redrawn during modulation, as intended; idle
settlement is not measured. The shader assumes legal admission to a shared
private target. No tracer ball, labels, full widget event work, audio, or total
module instrumentation is included. This is not a live Rack test.

## Quality and validation

History images were generated at frames 89, 95, 150 and 179 at 1x/2x, spanning
both modes and mixed histories. The four 1x baseline/cached-shader pairs were
inspected in a contact sheet. They show similar overall shapes and smooth
analytic trails; narrow peaks differ from sampled production geometry. This is
static inspection, not temporal or user visual acceptance.

The existing single-curve production screen still fails 187/288 cases under its
historical thresholds (max aggregate 15.578365%, max channel 255). Those failures
remain visible. No threshold was relaxed. The new history captures are diagnostic
images rather than a passing quantitative image gate.

Native probe builds and history contract/GL/cache assertions pass. Native
`plugin.dll` remains up to date; the candidates are not linked into live modules.
No package was installed, and no files were staged or committed. Full test-fast
was not run for this offline-only change.

## Reproduction / next decision

Build `build/tests/lumin_function_history_spec` using native MINGW64, then run
with Rack runtime first on PATH. Default runs all three final variants;
`--images` saves the diagnostic frames. The older separate-draw timing is retained
as evidence but is no longer a selectable variant of this final executable.

The next performance change should target measured draw/submission costs while
preserving cached history reuse. Do not enable the new recipe live on the basis
of reduced point storage or preparation alone. Normal-size highlighted peaks,
narrow sides and temporal stability still need acceptance before adoption.

[Durable results](lumin-evidence/function-history-summary.json). Raw logs and
images: local `doc/benchmarks/lumin-function-history-20260910/`.

## Shader timing follow-up

The [fast-math and draw-path probe](lumin-shader-timing-experiment.md) validates
precomputed constants, a fast atan and a two-iteration solver against the current
shader. Two iterations give a modest isolated GPU benefit; full-history CPU
adoption still fails. A draw-disabled control shows substantial execution and
presentation cost outside fragment evaluation. No live renderer change.
