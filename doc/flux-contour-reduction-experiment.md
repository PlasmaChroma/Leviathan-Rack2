# Flux current-contour reduction experiment

Status: offline experiment complete; neither variant meets the CPU admission
gate consistently, so no Flux renderer or user configuration change is made.
This is a separate experiment after the initial Lumen adapter checkpoint.
Benchmark labels B/C mean reference/candidate strokes within the offline fixture;
they do not indicate that the fixture executes the live adapter.

## Hypothesis and limits, recorded before running the benchmark

The active Flux captures place roughly 58–60 µs median CPU submission in current
contours across two previews. Removing redundant stroke vertices may reduce
NanoVG path submission and tessellation enough to pay for a bounded reduction
pass when geometry changes. The existing local-slope simplifier stays intact;
the experiment adds a second pass over its output with at most 128 vertices.

The additional centerline deviation is bounded to 0.02 logical pixels from the
existing simplified polyline. Endpoints and the topmost vertex are preserved.
At an effective scale of eight this permits up to 0.16 physical pixels of
centerline deviation; this is an explicit geometric approximation, not a claim
of pixel identity or a bound on antialiasing/join differences. History capture
geometry and marker placement must remain unchanged. No zoom-driven source
rebuild, new framebuffer pass, worker, or shader is required.

Initial offline admission gates: at least 10% median CPU improvement including
the added preparation for 16 continuously changing contours at 1x and 4x;
no more than 5% p95 CPU regression in those cases. Image comparisons require
summed absolute RGBA error divided by four times reference alpha mass <=3%, and
maximum channel difference <=64/255, at scales 1, 1.19, 2, 4, and 8. These are
screening limits, not proof of perceptual equivalence. Independent CPU tests
check centerline distance against a double-precision segment-distance oracle.

Extend `proc_preview_render_benchmark --bounded-reduction` as a representative
stroke fixture using the shared candidate helper. It includes reduction of all
three retained paths in CPU cost, normal NanoVG submission/flush, and separate
delayed GPU queries. Existing source geometry generation is unchanged and outside
the timed region for both configurations; only the added reduction is charged.
GPU queries exclude CPU preparation. Three sampled images per scenario are read
outside timed regions. The fixture uses Proc shapes, not the live Flux widget;
it cannot establish either Flux mode's visual parity or Rack performance.

If these gates pass, add a disabled-by-default Dragon King C option and live
counts/timing for the new preparation. Check both Flux shape modes, highlighted
edges/crest, continuous modulation, zoom, and return to settled caches. Live
admission requires a useful reduction in contour submission with no intentional
delay or capture change. Host frame/source-age and memory gates from the RFC
remain separate; no offline result alone changes a released default.

## First result and narrower follow-up

The first native run identifies Intel Iris Xe, GL 4.6, driver build
32.0.101.7088. All sampled image comparisons pass, with maximum channel difference
39/255 at 8x and alpha-normalized errors around 0.12%. Retained full-path vertices
fall from 231,936 to 192,336 over the 16-contour measurement interval (about 17%).
However, CPU medians including preparation are 165.7 -> 172.7 µs at 1x and
153.8 -> 154.8 µs at 4x. The first candidate fails its improvement gate and will
not be wired into Flux on these results.

Before the next run: test `--bounded-reduction-active-only`, which reduces only
the full path actually drawn by this fixture. Keep the same 0.02 tolerance,
image limits, and CPU gates. A live implementation would prepare split-highlight
paths lazily when needed; no result here certifies that interaction path. This
follow-up isolates the cost of preparing unused representations instead of
loosening the visual tolerance to achieve a favorable timing.

## Follow-up result and disposition

The [preserved native logs and computed gates](benchmarks/flux-contour-reduction-20260909/summary.json)
include SHA-256 hashes and the checkpoint commit. Active-only run 1 submits B then
C; run 2 reverses that order. Each scenario has 100 warm-up frames and 260 measured
frames. Numbers below include reduction of the selected paths, and do not sum CPU
and GPU durations.

| Active-only variant, 16 contours | B CPU median | C CPU median | Improvement | CPU gate |
|---|---|---|---|---|
| Run 1, 1x | 166.9 µs | 152.6 µs | 8.57% | Fail |
| Run 1, 4x | 159.8 µs | 143.7 µs | 10.08% | Pass |
| Run 2, 1x | 159.0 µs | 149.3 µs | 6.10% | Fail |
| Run 2, 4x | 155.2 µs | 153.6 µs | 1.03% | Fail |

Both variants pass the sampled image gates at every tested scale and population;
their images are identical to each other because they draw the same reduced full
path. GPU queries returned all 260 measured results per scenario. GPU results
vary with scenario/order and do not establish a universal gain. No host frame
rate or Flux-specific performance is measured here.

The initial three-path candidate loses its submission savings to extra CPU
preparation. Active-only preparation is cheaper, but its net improvement does
not consistently meet the predeclared threshold. Keep the helper and tests as
offline experimental code; no production source includes it. Do not add a live
toggle, change the visual tolerance, or claim a speedup from these results.

Validation: the independent CPU error-bound tests pass in WSL and native Windows;
the native GL experiments pass their image/error checks. Native `plugin.dll`
verification reports up to date because rendering sources are unchanged. The
known unrelated full-fast-suite theme-test compilation failure was not rerun.

Reproduce inside the documented MINGW64 environment, with the installed Rack
runtime ahead of compiler DLL directories when running the benchmark:

```sh
make -j4 build/tools/proc_preview_render_benchmark build/tests/bounded_polyline_spec
./build/tests/bounded_polyline_spec.exe
./build/tools/proc_preview_render_benchmark.exe --bounded-reduction
./build/tools/proc_preview_render_benchmark.exe --bounded-reduction-active-only
./build/tools/proc_preview_render_benchmark.exe --bounded-reduction-active-only --reverse
```

This narrows the next investigation: measure how much contour CPU cost is path
submission versus NanoVG stroke expansion before choosing a different backend.
The current-contour cost remains real in the live capture, but reducing this
fixture's vertex count by about 17% alone is insufficient to justify migration.

The follow-up [stroke CPU profile](flux-stroke-profile-experiment.md) separates
path commands, frontend stroke work, backend queuing, and frame submission.
