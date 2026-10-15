# Function-defined shader probe — 10 September 2026

An offline GLSL 1.20 function renderer now exists in
`tools/experiments/lumin/function_curve_candidate.hpp`. It is specific to the
function family used by Integral Flux and Proc. No live module was changed,
no package was installed, and no successful live performance claim is made.

## Shared mathematics

Both modules use `k = 40 * abs(shape)` and output-dependent slope:

- Negative shape: `dv/dt` proportional to `1 / (1 + k*v*v)`.
- Positive shape: `dv/dt` proportional to `1 + k*v*v`.

The integral of reciprocal slope gives horizontal phase as a function of output:
`v + k*v^3/3` for negative shape, and `atan(sqrt(k)*v)/sqrt(k)` for positive shape.
Normalize by the integral at 1. Zero shape is linear. Falling phase is reversed;
Flux Shark Fin negates the rising shape. Thus one shader supports both modules.

The test extracts Proc's actual slope functions from `src/Proc.cpp` and uses its
production `ProcPreviewGeometry`; Flux equations/geometry are extracted by the
existing generator. At seven shape settings and three rise ratios, Proc and
Flux Maths generated points match exactly. The analytic model differs from the
production midpoint-integrated LUT by at most 0.00095481 normalized phase in
those sampled checks (0.09796 logical pixels across 102.6 pixels). This is a
phase comparison, not a guarantee on final pixel or vertical distance error.

The shader uses one immutable quad and uniforms per curve. There are no shape
point uploads, segment textures, or segment searches. Four closest-point
Gauss-Newton iterations estimate Euclidean stroke distance on each side. The
continuous curve unions both sides; highlighted runs have independent butt caps
and preserve fall-over-rise composition. This is a bounded approximation, not
a proof of global closest-point convergence for every parameter/pixel.

## Measurements

Native Windows, GL2/NanoVG harness, 100 warmup and 360 measured frames, three
rotated-order repeats. Curvature changes every frame. All curves share one
private target and one host image presentation. The common-target arrangement
is an integration assumption, not implemented module scheduling.

| CPU median, two curves | NanoVG | Function shader |
|---|---:|---:|
| Geometry precomputed outside timing | 22.6–23.3 µs | 45.2–52.1 µs |
| Geometry generation plus rendering | 58.3–66.5 µs | 49.9–52.4 µs |

Paired median reduction in the second run is about 14–21%. Two-curve p95 is
mixed: shader 70.5–102.3 µs versus NanoVG 71.2–72.6 µs, so tail performance does
not pass an adoption gate. GPU timings are recorded separately, not claimed as
a universal GPU win. With eight curves and precomputed geometry, CPU medians
are 55.2–57.1 µs shader versus 86.7–91.6 µs NanoVG.

The geometry-inclusive run uses each preview's actual LUT generation, sampling,
and existing pruning. It assumes those points can be eliminated. Enabled history
trails can still require points, and real preview refresh/caching behavior may
change the benefit. The test does not include history, labels, markers, admission,
or whole-module Step/Draw timing. Settled cache hits are not measured.

## Appearance

The 288-case screen covers Maths/Shark Fin, rise fractions 0.05/0.5/0.95,
shape -0.85/0/0.85, densities 1/1.19/2/4, integer/fractional origin, and
single-color/highlighted runs (the falling highlight is translucent).
Against the existing pruned preview, 187 cases exceed the historical 5%
aggregate or 64-channel-byte screen: maximum aggregate 15.578365%, byte 255.
These thresholds are diagnostic and have not been relaxed to claim success.

Against a dense CPU-generated analytic curve rendered by NanoVG, maximum
aggregate error falls to 1.093709%, with 168 threshold failures and max byte 180.
This indicates that the existing sampled curve representation accounts for much
of the aggregate difference, especially on narrow sides. It does not absolve the
shader of localized cap/join/AA differences. A representative 1x positive-Maths
curve looks similar under magnification, but this is not full visual acceptance.
No animation, theme/global-alpha corpus, or live Rack comparison has passed.

## Reproduction / next work

`make build/tests/lumin_function_curve_spec` generates both production models.
Run with Rack's runtime directory first on PATH:

- Default: production appearance screen (currently returns failure).
- `--dense-reference`: diagnostic analytic reference (currently returns failure).
- `--benchmark`: geometry precomputed, render and presentation timed.
- `--end-to-end`: changing production geometry plus rendering timed.

The executable checks model agreement, shader compilation/linking, and GL errors.
Native `plugin.dll` is up to date; the candidate is compiled only in the probe.
The final image-screen exit-code adjustment does not change timed algorithms.

Next: review/resolve normal-size narrow-side and highlighted-peak differences,
then prove whether Flux/Proc can omit contour geometry when trails are disabled
and share the required private pass. Preserve legacy geometry when history needs
it. Measure real module Step/Draw before enabling any default. This experiment
supports pursuing a specialized FunctionCurve feature; it does not replace the
general PolylineStroke objective or establish a live win.

Durable [measurements](lumin-evidence/function-curve-summary.json); raw logs are
local under `doc/benchmarks/lumin-function-20260910`.

## Full history follow-up

The [parameter history experiment](lumin-function-history-experiment.md) now tests
Maths/Shark modulation against production cached history. Preparation/storage
shrink, but no consistent total CPU benefit survives the rendering costs. The
contour-only timing above is not a complete-preview result. Live code is unchanged.
