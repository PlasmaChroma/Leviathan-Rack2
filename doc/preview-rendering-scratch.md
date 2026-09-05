# Preview rendering experiments

Working tracker for Proc first, then Integral Flux and Undertow where results
justify it. Preserve the current appearance, released state/IDs, and module-level
Process / Step / Draw telemetry. Phosphor remains a Dragon King experiment.

## Evidence and scope

- Live observation: the current phosphor tracer is slower than Proc's stock tracer.
- Source audit: preview publication versions can change without changing the
  effective rise fraction or shape; stock history currently captures on versions.
- Source audit: an allocated inactive buffered tracer repeatedly zeros its pixels.
- Hidden native Windows GL contexts work here. Offline CPU and GPU comparisons
  are possible, but Rack frame pacing and visual quality still need live checks.

## Experiments

1. Geometry-key invalidation and idempotent buffered clearing. Benchmark scheduler
   decisions and retained-buffer clearing offline; verify slow accumulated changes,
   timing clamps, resize, and pending upload state. No new renderer in this step.
2. Preview-specific measurements: rebuild/simplification, captures, contour
   submission, history updates, allocations, marker/text. Preserve macro timings.
3. Compare a separately retained contour against direct NanoVG rendering using
   stationary and continuously changing shapes. Keep marker and label independent.
4. Change phosphor to update history only on deposits and decay during composition.
   Treat persistence changes and premultiplied alpha explicitly; test faint tails.
5. Consider snapshot history or retained stroke meshes only if measurements support
   them. Keep scheduling, stroke rendering, and resource ownership experiments separate.
6. Follow-ups: Integral Flux layer invalidation; Undertow retained simplification.

## Measurement rules

Use warm-up, repeated batches, median and p95 durations, identical workloads and
build flags. Clearly distinguish scheduler microbenchmarks, actual geometry work,
CPU rendering submission, GPU completion, and whole-module/frame measurements.
Do not infer FPS gains from avoided rebuild counts. GPU queries must be retrieved
asynchronously; no glFinish in the normal profiling path.

Live matrix: idle, marker-only, common-rate modulation, genuine shape modulation,
fade-only; normal/high zoom; one/many instances; context close/reopen.

## Results

### First baseline pass — 2026-09-05

Implemented in Proc: normalized geometry-key acceptance independent of publication
version; marker/frequency remain live; history captures follow accepted geometry;
inactive tracers clear on backend transitions. Resize resets history instead of
capturing coordinates from the old dimensions. Ratio tolerance is only 1e-6 to
absorb floating-point noise, measured against the last accepted key; shape keeps
its exact existing LUT comparison. This is not a screen-space simplification policy.

Shared buffered clearing now skips already-transparent storage and preserves an
outstanding clear upload. The tool exercises actual capture followed by clearing,
initial/finished/pending uploads, proportional timing, slow movement, asymmetry,
shape changes, and resize.

Native MINGW64, repository O3/fast-math flags, 5 warm-up batches then 50 measured
batches of 2,000 operations. Numbers below are median / p95 **batch-average** ns
per operation, not individual-call tail latency:

| Operation | Median ns | p95 ns |
| --- | ---: | ---: |
| Old repeated clear, 106×48 | 274.35 | 281.95 |
| Old repeated clear, 212×96 | 1391.35 | 1419.25 |
| Old repeated clear, 424×192 | 6050.80 | 6320.85 |
| Idempotent clear, all three sizes | 1.30 | 1.30 |
| Geometry key, common-rate workload | 1.95 | 1.95 |
| Geometry key, changing ratio | 1.50 | 1.55 |

The scheduler fixture accepts zero additional keys for 12,000 proportional timing
updates after initialization. A version-per-update baseline would request 12,000
rebuilds/capture attempts. This counts decisions, not actual geometry generation,
successful captures, or GPU work. Ordinary instances that never allocated the
frame-cache buffer did not have the measured pixel-clearing cost.

Validation: native plugin.dll linked; all 10 Proc runtime tests and the benchmark's
contract checks passed. Live visuals and total draw-time improvement remain unmeasured.

Reproduce inside the documented native MINGW64 environment:

```sh
make -j10 build/tools/preview_invalidation_benchmark
export PATH="/c/Program Files/VCV/Rack2Pro":/mingw64/bin:/usr/bin
./build/tools/preview_invalidation_benchmark.exe
```

Next: measure actual Proc preview work separately, then compare direct and retained
contour rendering. No new contour cache or phosphor scheduling change in this pass.
