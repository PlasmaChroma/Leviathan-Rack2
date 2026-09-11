# Retained PolylineStroke shader checkpoint — 10 September 2026

Experimental implementation now exists in `src/render/PolylineStroke.hpp`. It is
header-only and is not included by any live module. No package was installed.
The proposed general segment-local backend is **not** implemented. This is an
explicit monotone-X specialization evolving the earlier distance-union probe.

## Implemented

- Owned source points and revisions, at most 1025 input points; exact duplicate
  normalization. Invalid/unsupported updates cannot reuse the previous contour.
- Separate geometry, coverage, and material invalidation. Unchanged draws and
  color, translation, or cap changes do not rebuild/upload source or geometry.
- Shared `PolylineDevice` program and per-stroke endpoint texture/VBO, with context
  leases, validity checks, and deferred retirement through existing helpers.
- Analytic round joins, butt/round caps, uniform logical width, premultiplied RGBA.
- Disjoint 4-device-pixel X slabs; conservative Y bounds clipped to each expanded
  slab. No independent overlapping capsule blending. Interior distance uses one
  square root after the minimum-squared-distance search.
- Explicit private-pass draw: no target creation, NanoVG stroke, host flush, or
  state guard inside the primitive. Executor baseline is required. It uses
  texture unit 0 and attributes 0/1, full-target clipping, positive uniform scale.

Limits: density 0.01–16, width at most 1024 logical units, target at most 16384
pixels per dimension, at most 4096 slabs and 64 candidate segments per slab.
Nonmonotone paths and excessive local density return unsupported. This is not
the complete v1 feature contract and is not a public adoption recommendation.

## Results

Same production-pruned changing Shark Fin fixtures as the prior probe, with
100 warmup frames, 360 timed frames and three rotated-order repetitions. Source
shape generation is outside timing; source copy, preparation, upload, GL work and
NanoVG image presentation are inside. The adapter deliberately advances source
revision every frame; these are changing-source measurements, not retained hits.

| Execution | Two strokes CPU median | 32 strokes CPU median |
|---|---:|---:|
| NanoVG, grouped-target run | 22.6–27.1 µs | 331.4–337.9 µs |
| Shader, grouped separate targets | 82.0–88.6 µs | 634.0–667.3 µs |
| NanoVG, shared-canvas run | 22.7–25.0 µs | 332.7–341.5 µs |
| Shader, one common target | 82.4–97.2 µs | 510.0–531.0 µs |

Neither arrangement wins. The common-target probe assumes a legal shared canvas;
it does not implement module admission, scheduling, or general clip composition.
GPU timings are also recorded; the data does not establish a single isolated
bottleneck. Texture-search coverage and per-stroke submission remain candidates
for replacement, rather than another surface-only optimization.

The historical NanoVG image screen still fails **154 of 1140 cases**: maximum
aggregate error 1.264579%, maximum channel difference 255. This is not visual
acceptance. A magnified representative crest still extends farther in the
analytic shader. Highlighted-run endpoint differences remain unresolved.

A separate CPU analytic reference checks the shader's translucent stroke coverage
within one alpha byte, including steep segments, a sharp join, fractional
translation, and slab boundaries. This verifies that specific union fixture; it
is not proof of general NanoVG appearance parity. Retention, style invalidation,
rejected-source behavior, context/fallback checks pass. Existing batch tests and
549 surface/lifecycle checks pass. Native `plugin.dll` is up to date; this opt-in
header is compiled in the probe, not the installed plugin. Full test-fast was not
run in this checkpoint.

## Reproduction and next experiment

Build `build/tests/lumin_polyline_spec` using native MINGW64. Run with Rack runtime
first on PATH. `--contracts` is expected to pass; the default 1140-image historical
screen is expected to fail. `--batch-benchmark` compares separate/grouped targets;
`--shared-benchmark` measures a common target. The archived fixtures are reused
through an adapter, leaving the old shader available as a reference.

Retain the source/device API and these checks. The next performance experiment
should replace endpoint-texture searching with segment-local shader inputs and
explicit overlap ownership, measured in the common private pass. Do not enable
this backend live merely because it now has a retained API.

Durable numbers: [retained-polyline-summary.json](lumin-evidence/retained-polyline-summary.json).
Raw logs are local under `doc/benchmarks/lumin-polyline-20260910`. Final defensive
input-validation changes postdate timings; contract checks were rerun afterward.
