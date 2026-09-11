# Round-join endpoint candidate

Follow-up: [complete-stroke validation](flux-full-stroke-candidate.md) is now
implemented and measured. The source-download blocker below is historical and
resolved; this document preserves the earlier isolated-math experiment.

Status, 10 September 2026: isolated CPU candidate implemented and validated.
Complete-stroke raster validation and live Flux integration are not implemented.

## Candidate

The upstream [NanoVG round-join implementation](https://raw.githubusercontent.com/memononen/nanovg/master/src/nanovg.c)
converts outward normals to angles, computes a sample count, then converts the
sample angles back to normals. Many shallow joins need only two samples. Those
samples are the original normals, so most of that calculation can be avoided.

`tools/experiments/lumin/round_join_candidate.hpp` implements a conservative two-endpoint fast path.
For a counterclockwise turn in [0, pi], the dot product identifies turns safely
below the two-sample threshold. Unit-length, cross-product and threshold-margin
guards send uncertain cases to the reference calculation. Larger turns retain
the reference arc, including intermediate samples. This does not substitute a
bevel for a rounded crest. Policy construction computes the threshold once per
stroke policy, outside the per-join loop.

This is an independently written isolated math model of the upstream sampling
rule, not a patched copy of the installed Rack implementation. The installed SDK
has headers but no NanoVG C source. Downloading the source was denied by the
session network policy, and escalation was rejected. Public upstream code was
inspected through browsing; its exact correspondence to the installed Rack
binary has not been verified. Production source and host contexts are untouched.

## Validation and timing

Pre-run checks require identical sample counts and unit-vector position error
below 2e-6. The native test sweeps 2,104,893 cases: ncap 2–64, 129 orientations,
259 turn samples including both sides of the two-sample boundary and branch-cut
orientations. Maximum observed error against the float trigonometric reference
is 5.3312015e-7. This is observed numerical agreement, not an analytic bound for
all floating-point inputs. The helper's finite/unit/ordered input contract is
intentional; it is not a general production geometry API.

`--round-arc-candidate` extracts normals from the same 260 changing Proc shapes
used by the stroke fixture, normalizing directions and swapping clockwise inputs
outside timing. It tests 1x, 1.19x, 2x, 4x and 8x. Each timed observation processes
one shape's joins 16 times, representing 16 contours with the same geometry.
One 260-frame pass warms up; five passes produce 1,300 measured observations.
Reference-first and candidate-first runs are sequential, not concurrent.

| Arc-only CPU median, 16 contours | Forward reference | Forward candidate | Reverse reference | Reverse candidate |
|---|---:|---:|---:|---:|
| 1x | 133.6 us | 2.5 us | 128.4 us | 2.9 us |
| 1.19x | 133.0 us | 5.1 us | 127.3 us | 2.6 us |
| 2x | 133.3 us | 3.8 us | 127.4 us | 2.5 us |
| 4x | 149.6 us | 5.4 us | 127.3 us | 2.6 us |
| 8x | 148.8 us | 4.5 us | 127.6 us | 3.9 us |

All 13,976 modeled joins per scale preserve sample counts; maximum observed unit
error on these shapes is 1.686e-7. Timings exclude geometry, normalization, policy
creation, join classification outside arc sampling, vertex packing, path commands,
backend submission and all GPU work. They must not be reported as full contour or
Flux rendering speedups. Warm repeated input sets and uncontrolled thermal/power
conditions also limit interpretation. Numerical agreement does not establish
pixel equality: rasterization can amplify tiny changes near an edge.

Native compilation and focused tests passed. `make plugin.dll` reports up to
date. No full-suite rerun was needed for the offline helper; the previously
recorded unrelated theme-test failure is not resolved by this work.

Raw timings and hashes: [local record](benchmarks/flux-round-arc-20260910/summary.json).

```sh
make -j4 build/tools/proc_preview_render_benchmark build/tests/round_join_candidate_spec plugin.dll
./build/tests/round_join_candidate_spec.exe
./build/tools/proc_preview_render_benchmark.exe --round-arc-candidate
./build/tools/proc_preview_render_benchmark.exe --round-arc-candidate --reverse
```

## Next gate

Obtain the NanoVG source revision used by Rack and wire this fast path into a
private offline build or a verified standalone stroke expander. Compare complete
stroke meshes and rendered images against the installed reference across both
turn directions, sharp/degenerate corners, widths, scales and continuously
changing Flux geometry. Account for all preparation and submission costs.
Only after that should an experimental live Flux path be considered. This math
candidate is promising but is not yet ready for a Rack A/B test.
