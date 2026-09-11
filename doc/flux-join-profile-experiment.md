# Contour join preparation experiment

Follow-up: an [isolated round-arc candidate](flux-round-arc-candidate.md) now
passes numerical tests; complete-stroke raster validation remains pending.

Status: two native runs complete, 10 September 2026. Round-join selection dominates
the stroke frontend cost in this fixture. Production rendering remains unchanged.

## Method

`proc_preview_render_benchmark --join-profile` extends the existing stroke probe.
Use the same changing Proc geometry, color, width, butt caps, NanoVG context and
timing brackets for round, bevel and miter joins (default NanoVG miter limit).
Only the join style changes. Each scenario has 100 warm-up and 260 measured frames,
at 1x, 1.19x and 4x, with 1 and 16 contours. Geometry generation is excluded.
The forward order is live control, round, bevel, miter; `--reverse` reverses it.
Both runs execute sequentially. CPU and delayed GPU durations remain separate.

Before execution, the diagnostic criterion was to compare frontend cost and
expanded vertex counts in both orders, inspect image differences, and use the
result to select an appearance-preserving optimization. Alternative joins are
diagnostic controls, with no production admission or visual equivalence claim.

The round instrumented images must exactly match the live geometry draw method.
All instrumented modes must deliver one backend path per contour. Three frames
(120, 240, 359) are read outside timing; alternative images report total absolute
RGBA error divided by four times reference alpha mass, maximum byte difference,
and changed pixel count. These sampled errors do not bound all possible curves.

## Results

Intel Iris Xe Graphics, GL 4.6, driver 32.0.101.7088. Raw logs, image sheets and
hashes are in [the local experiment record](benchmarks/flux-join-profile-20260910/summary.json).
Power/thermal conditions were not controlled or recorded; two short runs establish
a diagnostic ranking, not a sustained performance guarantee.

| 16 contours, CPU medians in microseconds | Forward 1x | Reverse 1x | Forward 4x | Reverse 4x |
|---|---:|---:|---:|---:|
| Round total | 169.5 | 179.3 | 159.2 | 187.4 |
| Bevel total | 38.2 | 37.4 | 33.6 | 37.8 |
| Miter total | 31.0 | 30.9 | 29.9 | 31.0 |
| Round frontend remainder | 149.0 | 156.1 | 139.9 | 162.4 |
| Bevel frontend remainder | 17.4 | 17.0 | 15.6 | 17.1 |
| Miter frontend remainder | 13.8 | 14.1 | 13.5 | 14.2 |

Frontend remainder is inclusive `nvgStroke` minus its backend callback per frame;
it is not an isolated internal-function timer. Component medians are not additive.
The round/bevel difference is about 88–89% of round frontend time in these cases.
At 1x the round and bevel paths expand to 1,826,048 and 1,822,208 vertices across
4,160 strokes: approximately 439 versus 438 vertices per contour. This near-equal
output size strongly points to join preparation rather than vertex submission
volume as the useful target. Miter output is about 123 vertices per contour.

All round/control sampled images match exactly, all backend-count assertions
pass, and all scenarios return 260 GPU samples with zero missing queries.
Both orders produce the same reported image errors. Bevel alpha-normalized error
is about 0.135–0.332%; miter about 0.356–0.415%. Maximum local differences reach
255/255. Low aggregate error therefore does not make either style equivalent.

Visual inspection of the [4x contact sheet](benchmarks/flux-join-profile-20260910/join-profile-4.00x.png)
shows the bevel flattening the sharp crest and the miter extending it; the
rounded crest is visibly different. Columns are round, bevel, miter; rows are
the three sampled frames. Readback is composited over dark gray for these sheets.

## Decision and next step

Prototype an offline round-join preparation optimization that preserves the
reference appearance. Inspect and instrument the relevant NanoVG join expansion
implementation before attributing the cost to specific trigonometric operations
or selecting a shortcut. The style comparison establishes a target, not that
internal diagnosis. Keep continuous geometry updates in the benchmark.

Compare any candidate against round joins with the same timing/image checks,
then expand corner cases and scales before testing actual Flux. Do not switch
Flux to bevel/miter or infer a live Flux speedup from this Proc-based fixture.

Native benchmark compilation passed without warnings. Both runs passed all
embedded checks. Native `make plugin.dll` reports up to date, as production
sources are unchanged. The known unrelated theme-test full-suite failure was
not rerun for this offline-only extension.

Reproduce in MINGW64, with Rack's runtime directory first on the executable PATH:

```sh
make -j4 build/tools/proc_preview_render_benchmark plugin.dll
./build/tools/proc_preview_render_benchmark.exe --join-profile
./build/tools/proc_preview_render_benchmark.exe --join-profile --reverse
```

Each invocation writes single-contour contact sheets to
`build/tools/join-profile-{scale}x.ppm`; preserve them before another invocation
if comparing image outputs between runs.
