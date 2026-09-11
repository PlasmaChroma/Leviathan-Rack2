# Contour CPU attribution experiment

Follow-up completed: the [join preparation experiment](flux-join-profile-experiment.md)
compares round, bevel and miter joins and records the next optimization target.

Status: two native runs complete; stroke frontend work dominates this fixture.
No production rendering changes. This follows the
[bounded vertex-reduction experiment](flux-contour-reduction-experiment.md),
which did not justify a live renderer change.

## Question and method

Separate the existing contour fixture's CPU work into:

- Offscreen frame setup and clearing.
- NanoVG path commands (`nvgBeginPath`, `nvgMoveTo`, `nvgLineTo`).
- Stroke style setters.
- Inclusive `nvgStroke` time.
- Backend `renderStroke` callback time nested within `nvgStroke`.
- The remaining `nvgStroke` CPU time after subtracting that callback per frame.
- `nvgEndFrame` CPU time and residual placement/timer overhead.

The frontend remainder is not labeled tessellation-only: it includes whatever
frontend work NanoVG performs plus probe/timer overhead outside the backend
callback. `nvgEndFrame` measures CPU submission, not GPU completion. Delayed GPU
queries report separately; CPU and GPU times are not additive frame durations.

The existing `proc_preview_render_benchmark --stroke-profile` fixture uses
identical changing geometry, stroke color/width, butt caps, and round joins in
control and instrumented runs. The control invokes the existing geometry draw
method; the instrumented path splits the same sequence into timing brackets.
Source geometry generation stays outside the timed region in both cases.

Instrumentation wraps only the privately created benchmark context's backend
callback, forwards all arguments unchanged, counts expanded stroke vertices,
and restores the callback before context teardown. This internal NanoVG API is
an offline diagnostic boundary, not a proposed plugin/host integration mechanism.
No Rack-owned context is modified.

Run at 1x, 1.19x, and 4x, with 1 and 16 contours; use 100 warm-up and 260 measured
frames per scenario. Run control-first and instrumented-first comparisons to
expose timing perturbation and order effects. Report control and instrumented
total CPU times beside component timings. Require exact equality of the three
sampled images and one backend stroke path per submitted contour. All readback
and diagnostics formatting occur outside measured regions.

These are representative Proc stroke measurements on the laptop, not Flux
widget/host measurements. Do not infer a Flux speedup, UI frame rate, or source
latency. The purpose is to select the next bounded experiment. If no component
dominates consistently or instrumentation materially distorts the ranking,
report that ambiguity rather than prescribing a new backend from it.

```sh
make -j4 build/tools/proc_preview_render_benchmark
# Run with Rack's matching runtime DLL directory first in PATH.
./build/tools/proc_preview_render_benchmark.exe --stroke-profile
./build/tools/proc_preview_render_benchmark.exe --stroke-profile --reverse
```

## Results — 9 September 2026

Both runs used Intel Iris Xe Graphics, GL 4.6, driver build 32.0.101.7088.
[Raw logs and summaries](benchmarks/flux-stroke-profile-20260909/summary.json)
preserve hashes, checkpoint commit, benchmark source hash, and run order.
Every scenario passed exact image equality against the live geometry draw method
(36 paired image snapshots across both runs). Instrumented runs observed one
backend path per contour; all delayed GPU query samples were available.

| CPU median, 16 contours | Run 1, 1x | Run 2, 1x | Run 1, 4x | Run 2, 4x |
|---|---|---|---|---|
| Control total | 159.7 µs | 159.0 µs | 151.9 µs | 162.1 µs |
| Instrumented total | 165.9 µs | 166.3 µs | 161.2 µs | 161.0 µs |
| Path commands | 7.8 µs | 7.7 µs | 7.6 µs | 7.6 µs |
| Stroke frontend remainder | 146.1 µs | 145.9 µs | 141.4 µs | 140.8 µs |
| Backend stroke callback | 2.9 µs | 2.8 µs | 2.7 µs | 2.8 µs |
| `nvgEndFrame` | 6.7 µs | 6.3 µs | 6.5 µs | 6.7 µs |

These component medians are not additive. The frontend remainder is nested inside
inclusive stroke time, which is nested in the total. Per-frame frontend share of
inclusive stroke time is approximately 98%. Single-contour cases independently
show roughly 0.5 µs path commands, 9.4–9.5 µs frontend remainder, 0.2 µs callback,
and 1.2–1.5 µs `nvgEndFrame` medians. The 1.19x results show the same ranking.

Measured input averages about 55.8 retained points per contour, while the backend
receives about 439.9 expanded stroke vertices per contour. The control's logged
backend counts are zero placeholders because its callback is not instrumented;
they do not mean it rendered zero paths or vertices.

Instrumented/control total medians differ by approximately -0.8% to +6.1% across
the two runs and all scenarios. That variation plus timer overhead matters for
small comparisons, but it is much smaller than the separation between frontend
stroke work and path/backend work. This experiment supports the cost ranking;
it does not measure the exact cost of NanoVG tessellation independently or prove
that any one internal function is responsible.

## Decision

The next useful experiment targets stroke preparation inside `nvgStroke`, while
preserving the curve and appearance. Further path-command batching, upload tuning,
or resource sharing cannot remove the dominant cost observed in this fixture.
Separate round-join preparation from the rest of the frontend before selecting
a replacement: a diagnostic join-style comparison can identify that cost, but
must not silently become a visual change in Flux. A retained mesh only avoids
this work when the relevant geometry/style/scale is unchanged; continuous
modulation still needs an efficient update path.

The live Flux adapter, history, controls, and user configuration are unchanged.
No new Rack capture is required for this diagnostic result. Revalidate in the
actual Flux widget before promoting a future optimization; the fixture excludes
its cache wrapper, labels, controls, host traversal, and application frame pacing.
