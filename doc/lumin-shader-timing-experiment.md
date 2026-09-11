# Shader draw-path timing — 10 September 2026

This checkpoint separates analytic shader evaluation from the private-surface
execution/presentation path and tests fast math before another architecture change.
No live module, package, or default kernel was changed. The reference remains the
default; optimized kernels are explicit offline options.

## Implemented variants

`FunctionCurve::Kernel` now selects:

- `Reference`: original four-iteration GLSL atan-based calculation.
- `Prepared`: CPU-precomputed square roots, inverse normalization and inverse
  root-normalization. Positive phase uses built-in atan; negative phase remains
  polynomial. Shape-dependent reciprocal arithmetic moves out of fragments.
- `FastAtan`: Prepared plus a range-reduced polynomial atan. Reciprocal and pi/4
  transforms bound the Taylor input to +/-tan(pi/8), then an odd degree-11
  polynomial evaluates it. This is derived from the alternating atan series;
  it is not imported from a third-party fast-math implementation.
- `TwoStep`: Prepared with two closest-point Gauss-Newton iterations instead of
  four, retaining built-in atan. This is the useful GPU candidate from this run.

Each builds a distinct shader with compile-time definitions, so runtime variant
branches are not added to the fragment path. Existing context leases, validity
checks and deferred retirement remain intact. All kernels preserve point-free
parameter rendering, ring order, source-over blending and conservative rejection.

## Quality check

The new `lumin_shader_timing_spec` compares all three optimized variants with the
reference analytic shader in 300 fixtures. Coverage includes 1/1.19/2/4/8 density,
1%/5%/50%/95%/99% rise ratios, signed shape extremes and near-linear values,
Maths/Shark, highlighted colors, fractional position, and one/seven layers.
The predeclared tolerance is two channel bytes. **All variants pass at maximum
one byte difference**. This tests optimization equivalence; it does not erase
the existing analytic-versus-NanoVG failures or prove arbitrary closest-point
convergence outside the tested cases.

## Timing controls

Intel Iris Xe Graphics, native Windows GL2. Every mode renders two previews;
layer counts are 1 or 7 **per preview**. Test densities are 1x/2x, with 100 warmup
frames and 360 timed frames, three rotated-order repeats. No GPU samples were
skipped. Shape inputs change each frame; uniform packing/preparation is timed.

- **Private**: targets are already bound and cleared before timing; measure
  primitive preparation/submission and its GPU work without a host boundary or
  image presentation. This is a legal isolated private pass, not a live shortcut
  into a pending Rack NanoVG frame.
- **Complete**: include `AdaptiveGlSurface::renderBatch`, surface update/clear,
  NanoVG image presentation and end-frame flush.
- **No-draw control**: same reference program, resource validation, uniform and
  vertex setup, but skip glDrawArrays. The complete variant still updates and
  presents a transparent image. No uniform is compiled away to manufacture a
  cheaper shader. Driver deferred work/synchronization can nevertheless differ,
  so differences between medians are not a strict additive cost decomposition.

## Findings

The hand-written atan is **not consistently faster** on this GPU. Prepared
constants provide a small GPU improvement in several cases. The TwoStep kernel
has a clearer isolated result for two single-layer previews at 2x:

| Repeat | Reference GPU median | TwoStep GPU median | Reduction |
|---|---:|---:|---:|
| 0 | 17.628 us | 16.380 us | 7.1% |
| 1 | 19.344 us | 17.212 us | 11.0% |
| 2 | 19.760 us | 17.680 us | 10.5% |

These are GPU times, not total module CPU improvements. Seven-layer private
results are smaller/more variable, and complete-path CPU results vary heavily.
Typical steady private CPU submission is around 10–12 us for two previews,
while complete CPU samples frequently land around 60–100 us. The complete
no-draw control remains expensive (often 56–84 us, with outliers), demonstrating
substantial cost outside fragment evaluation. Do not attribute all of it to one
specific GL call: state queries, surface work, image submission and synchronization
have not yet been individually measured.

The production-snapshot-history comparison was rerun with TwoStep, preserving
six slots and actual capture/fade behavior. It still shows **no consistent
complete-preview CPU win**. At 2x, production CPU medians were 70.5–91.4 us;
layered analytic 79.7–97.3 us; cached analytic 95.2–100.2 us. The complete preview
still does not pass adoption, despite a validated isolated shader improvement.

## Reproduction and next focus

Build `build/tests/lumin_shader_timing_spec`. Default runs the 300 quality cases;
`--benchmark` runs all five timing variants and both pass scopes. The no-draw
control uses `setSubmitPixels(false)` and is diagnostic only.

`build/tests/lumin_function_history_spec --two-step` runs the production-history
comparison with the optimized kernel. Its default retains the reference kernel.
Native builds, quality checks, history capture/fade/cache checks and final GL
error checks passed. Native plugin.dll is up to date; the changed shader is only
compiled by explicit probes. Full test-fast was not run. Nothing installed,
staged or committed.

The next execution optimization should profile the host boundary/surface/
presentation phases individually and preserve lifecycle correctness. Replacing
atan alone is not supported as the route to a full win. Keep the faster math
variants available as controlled experiments rather than promoting them based on
isolated timing.

[Durable results](lumin-evidence/shader-timing-summary.json). Raw logs are local
under `doc/benchmarks/lumin-shader-timing-20260910/`. Earlier measurements and
failed image gates remain unchanged.

## Opt-in live candidate now prepared

The [current-contour live pilot](lumin-function-live-pilot.md) is built and
packaged, off by default. It preserves existing points/history/ball and adds
per-preview phase/candidate telemetry. No installation or live result yet.
