# Flux custom shader candidate

Status, 10 September 2026: the user selected a custom shader as the intended
rendering direction. A first WYRM-derived prototype is implemented offline.
It does not pass image or complete-path performance screening and is not wired
into Integral Flux or installed in Rack. Cleanup moved its source and probes to
`tools/experiments/lumin`, removed the previous private-NanoVG runtime integration,
and disabled the old local flag. No production pruning or patch state changes.

## What was reused

`src/WyrmRendererGL.cpp` supplies the useful model: squared distance to nearby
segments in a fragment shader, plus conservative disjoint tiles that limit the
fragment domain. Its regular-X curve texture and bounded neighboring-index
lookup cannot directly address Flux's unevenly spaced pruned vertices.

`tools/experiments/lumin/flux_shader_candidate.hpp` instead uploads the actual retained segment
endpoints. Each 16-physical-pixel-wide tile carries a conservative contiguous
segment range. The shader computes minimum distance, round interior coverage,
butt endpoints and one-pixel antialias coverage, outputting premultiplied color.
Separate colored paths preserve draw order. It does not call NanoVG stroke
generation, expand joins on the CPU, or resample the curve.

This GL2/GLSL 1.20 prototype requires floating-point textures. It bounds input to
1,024 segments, monotone X and at most 64 candidate segments per tile, rejecting
unsupported input. These are admission limits, not permissions to discard points.
The small Flux fixture currently uses the existing 128-point source and existing
simplification. Geometry is extracted from production methods at build time.

`tools/experiments/lumin/experimental_shader_stroke.hpp` deliberately retains the previous
AdaptiveGlSurface integration for the first comparison. It has the same density,
fractional alignment, clipping/composition and context-lease lifecycle structure.
This is a diagnostic wrapper, not the intended final integration architecture.

## Image and lifecycle screening

`tools/experiments/lumin/flux_shader_surface_spec.cpp` compares 1,140 composed images: a synthetic
sharp join plus 18 production-generated Flux curves covering Shark Fin and Maths,
three rise ratios and three curve settings; full and split colors; densities
1, 1.19, 2, 4 and 8; three fractional offsets and two inherited alpha levels.

The initial shader screening gate was set before the first run: aggregate error
less than 5% of reference alpha mass and maximum channel error at most 64/255.
154 images fail the channel limit. Maximum aggregate error is 1.259119%, and
maximum channel error is 255/255. Aggregate similarity must not hide localized
join/highlight differences. The first sharp-join image shows added coverage at
the crest: an analytic round join does not exactly match NanoVG's coarse join
triangles. The broader highlight failures still need attribution.

Framebuffer restoration, repeated-frame/nested/rotation fallbacks, destruction
without a current GL context, retirement draining and reused-context recreation
checks pass before the final image rejection. The executable intentionally exits
nonzero on the image gate; it is an explicit experimental target outside test-fast.
The gate has not been relaxed to accept the prototype.

## Complete-path performance screening

The benchmark draws two current contours generated with production pruning,
including host `nvgEndFrame`, at densities 1 and 2. It compares Rack NanoVG,
private optimized NanoVG, the shader and a surface-only control. Each uses 100
warmup frames plus 360 measured frames, repeated three times with rotated order.
It also records asynchronous GPU query timings. Geometry generation is outside
the timed region. It does not emulate the entire Rack module, history or host
scheduling; these numbers must not be presented as live Rack measurements.

The first controlled run at 1x has these median CPU ranges across repetitions:

| Path, two contours | Median CPU range |
|---|---|
| Rack NanoVG | 23.3–28.1 us |
| Private NanoVG surfaces | 120.2–124.3 us |
| Shader surfaces | 152.9–201.3 us |
| Empty surfaces, clear/present only | 78.9–103.6 us |

The empty control is not equivalent visual work. It isolates the common
clear/state/surface/composition route sufficiently to show that it already
exceeds the host baseline without any line work. Do not subtract independent
medians to claim an exact shader or state-guard cost. The shader also has its own
texture-upload and per-fragment search cost; a shader alone does not fix this.

Raw images/timing logs, input hashes and exact results are preserved locally in
`doc/benchmarks/flux-shader-prototype-20260910`. Native MINGW64 compilation succeeds;
the production `plugin.dll` target remains current because no runtime code changed.

## Next engineering checkpoint

Keep the custom shader as the target. Evaluate a shared render submission and
tighter graphics-state handling instead of one private surface transition per
preview; use WYRM's integration as a reference as well as its shader. Measure
that submission cost before investing further in shader arithmetic. Batch and
cache uploads where justified by actual geometry revisions. Preserve NanoVG
ordering, host clipping, opacity, lifecycle retirement and module-level telemetry.

Resolve localized join and colored-endpoint coverage differences with the same
image corpus. Do not enable another live candidate until its visual gate passes
and complete-path performance beats the host baseline. A successful stroke-only
microbenchmark is insufficient.

## Reproduce

In the documented MINGW64 environment:

```sh
make -j6 build/tests/flux_shader_surface_spec
```

With Rack's installed runtime first on PATH, run
`build/tests/flux_shader_surface_spec.exe` for image/lifecycle screening, and add
`--benchmark` for the separate complete-path benchmark. The image run currently
returns failure as documented above. Diagnostic PPMs are written under `work/`.
