# Full-stroke round-join candidate

Status, 10 September 2026: private offline renderer implemented; complete-stroke
images and CPU submission tested against both an unmodified private reference
and installed Rack. No production renderer, installed plugin, or user option changed.

## Source and isolation

The download restriction from the isolated experiment is resolved. Rack reports
version 2.6.6. Its public release dependency points to VCVRack/nanovg commit
`0bebdb314aff9cfa28fde4744bcb037a2b3fd756`; the downloaded `nanovg.h` and
`nanovg_gl.h` match the installed SDK byte-for-byte. Dependency metadata and
original sources are retained locally under `work/nanovg`.

`tools/experiments/lumin/prepare_round_stroke.py` creates an explicitly altered private source copy
under `build/tools/round_stroke`, preserving the upstream license. It checks the
core source hash, prefixes NanoVG public symbols, and adds the endpoint fast path.
It does not replace any Rack function or mutate a Rack-owned application context.
Both private variants share one build, with the fast path selected outside each
draw. The reference still has a small selection-branch cost; installed Rack is
also measured to make that comparison visible.

The full implementation preserves the original inner-join handling, strip
topology, antialias coordinates, caps, and larger round arcs. It replaces only
the two-sample outer arc calculations when conservative direction, unit-length,
and angular-threshold checks pass. The threshold is calculated once per stroke
and its cost is included in timing. All stroke vertices are regenerated for each
changing contour; there is no cross-frame mesh reuse or extra segment pruning.

## Workload and gates

The generator extracts current Flux preview methods from `IntegralFluxWidget.cpp`
and slope functions from `IntegralFlux.cpp`, retaining existing 128-point sampling,
LUT integration, crest preservation, and legacy simplification. Its constants are
checked before extraction. Geometry source hashes are retained. This is the real
geometry code in a small offline model; the live widget, history and controls are
not instantiated.

The test covers both Shark Fin and Maths modes, changing rise ratio and signed
curve, at 1x, 1.19x, 2x, 4x and 8x effective scale. For each mode/scale it compares
all 260 measured geometry states, not only three screenshots. It additionally
checks six sharp, reversed, near-collinear, duplicate-point and overlapping open
paths in both directions at five widths (0.25, 1.15, 1.4, 4 and 12) and five scales.
That produces 2,900 image comparisons per run for each reference/candidate pair.

The image gate, set before the first full run, is <=0.1% alpha-normalized absolute
RGBA error and <=16 maximum byte difference; vertex topology and antialias
coordinates must match, with position error <0.0002 logical units. The unmodified
private baseline must also have pixel-identical output to installed Rack.
An initial exact-mesh diagnostic found a 1.19e-7 position difference with zero
pixel difference between private and installed builds. Baseline meshes therefore
use the same numerical tolerance while retaining exact pixel equality. The image
gate was not loosened.

CPU timing includes clear/frame setup, path commands, complete stroke preparation,
vertex packing, backend queueing and frame submission. Source geometry generation
is excluded equally. The 16-contour workload repeats each geometry 16 times,
matching the earlier population fixture. Each scenario has 100 warm-up and 260
measured frames, with delayed GPU queries reported separately. All GL work is in
a hidden offline window on Intel Iris Xe, GL 4.6, driver 32.0.101.7088.

The forward order is installed Rack, private reference, candidate; the reverse
order reverses it. The analyzer applies the earlier CPU screening rule of >=10%
median improvement and <=5% p95 regression. This is a CPU screen, not the RFC's
complete host-frame/source-age/memory admission gate. No power or thermal control
is asserted.

## Findings

The complete-stroke gain survives: early runs put 16-contour median CPU submission
roughly 70–80% below the private reference; the final pair spans 42.1–79.5% across
both modes and all five scales. All 20 final 16-contour comparisons pass the CPU
screen. Later batches show higher absolute times in some cases; uncontrolled run
conditions prevent assigning a cause. Both Flux modes benefit, but the earlier
70–80% range must not be treated as a guaranteed or universal gain.
The gain is smaller than the isolated math speedup because frame/path/backend
work remains, as expected. Raw installed/private timings are kept separately;
CPU and GPU durations must not be added or interpreted as host frame time.

Every candidate image in the completed full runs is pixel-identical to its
reference, and every private baseline image matches installed Rack. Mesh counts
and antialias coordinates are unchanged. Maximum observed candidate mesh position
difference is 7.68876816e-6 logical units. This validates the sampled workload,
not every possible width, transform, GPU or curve.

Initial short runs had isolated single-contour p95 regressions despite much lower
medians; different cases showed spikes on verification. Those results are retained
in `initial-*` and `verification-*` logs and summaries, not discarded. A targeted
longer check passed all twelve comparisons, then the check was expanded to every
single-contour mode/scale: six alternating repetitions with 2,000 measured frames
per variant in each repetition. All 60 of those longer comparisons pass both the
median and p95 CPU screens. The final short matrix still has two single-contour
p95 failures (38/40 comparisons pass overall); preserve that variation alongside
the stronger longer-run evidence rather than declaring every short run passed.
Final screening counts and numerical results are
recorded in the [machine-readable summary](benchmarks/flux-full-stroke-20260910/summary.json).

The [comparison sheet](benchmarks/flux-full-stroke-20260910/comparison.png) shows
reference, candidate, and a 16x amplified RGB difference at 8x scale. The difference
panel is black; numerical comparison also includes alpha. Images were inspected.

## Validation and disposition

Native compilation succeeds. The downloaded Fontstash emits a teardown warning
about passing a freed pointer to `fons__tt_done`; the selected STB implementation
of that function ignores its argument. No fonts are used in this fixture. This
upstream warning is recorded, not presented as a clean-warning build. It must be
resolved if a private NanoVG implementation is ever carried into production.
`make plugin.dll` reports up to date. The unrelated previously reported theme
full-suite compilation issue was not rerun for this tools-only experiment.

The candidate merits an experimental integration design. This benchmark does not
provide a safe way to replace Rack's internal renderer from a plugin. A live option
needs a private, context-owned stroke path with correct NanoVG composition and
lifecycle handling. Do not patch installed Rack, replace its callbacks, or change
released defaults based on this result. Preserve legacy pruning and rounded joins.
The remaining live checks are Flux modulation/history, zoom, settlement, reopen,
host frame tails and source freshness once that integration exists.

## Reproduce

Fetch `nanovg.c`, `nanovg.h`, `nanovg_gl.h`, `fontstash.h`, `stb_truetype.h`, and
`stb_image.h` from the pinned VCVRack/nanovg commit's `src/` directory into
`work/nanovg`. The Makefile does not download dependencies or modify the SDK.
Use native MINGW64 with Rack's runtime directory first when executing the binaries:

```sh
make -j4 build/tools/round_stroke_benchmark plugin.dll
./build/tools/round_stroke_benchmark.exe > doc/benchmarks/flux-full-stroke-20260910/forward.log
./build/tools/round_stroke_benchmark.exe --reverse > doc/benchmarks/flux-full-stroke-20260910/reverse.log
./build/tools/round_stroke_benchmark.exe --tail-check > doc/benchmarks/flux-full-stroke-20260910/tails.log
python3 tools/experiments/lumin/analyze_round_stroke.py
```

Create the capture directory first. `doc/benchmarks`, `build`, and the scratch
`work` directory are ignored; generated results are local evidence, not automatically included in
a future commit. No files have been staged or committed by this task.
