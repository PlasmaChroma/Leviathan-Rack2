# Shared surface execution screening

10 September 2026. Opt-in batch API implemented and native checks passed; no live
module uses it yet. The legacy surface entry point retains its original behavior.

Before the first benchmark run, set screening requirements: identical composed
pixels for identical callbacks; mixed-pass state isolation and host restoration;
zero guard/callback work for an unchanged batch; independent dirty updates.
For CPU performance, require at least 10% median improvement at 8/32/64 updates
in each of three rotated-order repetitions, with p95 no more than 5% worse.
Single-update overhead must remain below 5 us median. These are initial harness
screening limits, not a claim of host frame-time benefit or shader visual parity.

Compare the same shader callback, target sizes, clears, uploads and presentation
under separate versus shared boundaries. Warm 100 frames, then measure 360 frames
at 1/2/8/32/64 surfaces. Include host NanoVG end-frame submission and delayed GPU
query timing. Geometry/fixture construction stays outside the timed region.

## Results

`AdaptiveGlSurface::renderBatch` updates at most 64 distinct surfaces under one
lazy host-state guard, establishing a neutral shader state before each update.
Fully cached batches use no GL guard or callbacks. Targets and invalidation remain
independent. Allocation failure leaves per-update `rendered` false. Request
validation happens before rendering; callbacks follow the opt-in contract below.

Three rotated-order repetitions of the final simple-shader fixture yielded:

| Surfaces | Separate median range | Grouped median range | Paired median CPU reduction |
|---|---|---|---|
| 2 | 75.6–80.5 us | 45.9–58.6 us | 23.9–43.0% |
| 8 | 287.5–296.8 us | 79.3–85.2 us | 70.6–72.5% |
| 32 | 1095.9–1120.2 us | 227.5–232.9 us | 78.7–79.7% |
| 64 | 2230.5–2234.3 us | 418.2–440.2 us | 80.3–81.3% |

All 8/32/64 cases pass the initial median/p95 screening gate. The first run's
single-update case exceeded its 5 us overhead limit once (9.3 us); the final run,
with cached uniform lookup in both fixture paths, stayed within it (worst 4.3 us).
Timings at one surface vary substantially. Do not infer a one-surface benefit;
existing callers remain unchanged and no broad migration is justified yet.

The same grouped executor was also measured with the archived Flux shader and
production-generated pruned curves. Two contours took 78.9–97.9 us median grouped,
versus 111.6–138.3 us separately. The host NanoVG baseline was 22.5–23.4 us.
Grouping saves roughly 27–31% in this workload but does not make that shader
competitive. Its local distance search, per-update preparation/uploads and target
costs still need work; these data do not individually attribute those components.

The grouped path matches the separate shader path **exactly in all 1,140 Flux
image comparisons**, including split colors, opacity, clipping, densities and
fractional offsets. This establishes batching parity, not shader/NanoVG parity:
the shader's previous localized visual differences remain unresolved.

The focused batch test verifies mixed shader-state isolation, host bindings and
scissor/upload-state restoration, independent dirtiness, cache hits, and rejection
of invalid/duplicate requests. The existing 549 surface/lifecycle checks pass;
the archived corpus also exercises context retirement/recreation. Native plugin
linking passes. No module backend/setting was changed and no package was installed.

[Durable measurements and hashes](lumin-evidence/batch-execution-summary.json)
preserve exact results. Raw logs remain in `doc/benchmarks/lumin-batch-20260910`.
These are hidden-context CPU submission/GPU query measurements, not Rack frame
timings or demonstrated live module gains.

## Callback and adoption contract

Callbacks use texture unit 0 and generic attributes 0..3, keep matrix stacks
balanced, and draw into the selected private target. They establish their own
program/inputs and any state beyond the declared baseline. The baseline clears
program, texture/buffer bindings and generic-array enables; restores ordinary
pixel unpack state; disables depth/stencil/cull/scissor/alpha tests; enables
premultiplied source-over blending; and establishes fill mode and identity
projection/modelview matrices. The surface then binds/clears its own target and
supplies its active dimensions and vertical offset. It sets color writes enabled.

This is not permission to pass arbitrary legacy callbacks without auditing their
state requirements. Host state is saved/restored once; callbacks must not alter
matrix stack depth, other texture units or target attachment configuration.
The caller establishes current visibility/destination, owns callback data through
execution, and must not rewrite images already presented in the current recording.
The batch API is an execution primitive, not an editor-wide admission scheduler.

Next: use the measured executor for a retained PolylineStroke implementation with
bounded local coverage and cheaper uploads, then resolve perceptual differences.
Prove real module scheduling and mixed-consumer behavior before enabling it in Rack.

## Reproduce

Build `build/tests/adaptive_gl_batch_spec` and `build/tests/flux_shader_surface_spec`
in the documented MINGW64 environment, with Rack runtime DLLs first on PATH:

```sh
./build/tests/adaptive_gl_batch_spec.exe
./build/tests/adaptive_gl_batch_spec.exe --benchmark
./build/tests/flux_shader_surface_spec.exe --grouped-images
./build/tests/flux_shader_surface_spec.exe --batch-benchmark
```
