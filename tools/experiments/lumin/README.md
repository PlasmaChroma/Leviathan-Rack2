# Archived Lumin rendering experiments

These are explicitly invoked research tools, not the PolylineStroke library.
Nothing in this directory is compiled into the normal plugin or selected by
`test-fast`. Build outputs retain their historical target names for convenience.

| Experiment | Status |
|---|---|
| Bounded extra decimation | Failed consistent performance gate; not adopted |
| Round-join endpoint shortcut | Offline numerical/full-stroke gains; reference retained |
| Private NanoVG surface | Image/lifecycle checks passed; live performance regressed; runtime integration removed |
| WYRM-derived shader surface | Prototype only; complete-path performance and image screening fail |

The supported runtime remains the legacy preview adapter and existing renderer.
The next feature is specified in `doc/lumin-polyline-stroke.md`.

## Native reproduction

Use `doc/windows_build_from_wsl.md` to select MINGW64, and put the installed Rack
runtime ahead of compiler runtime DLLs when executing Rack-linked binaries.
Run from the repository root. Create `work/` for diagnostic images if absent.

```sh
mkdir -p work
make -j6 build/tests/bounded_polyline_spec build/tests/round_join_candidate_spec
make -j6 build/tests/round_stroke_surface_spec build/tests/flux_shader_surface_spec
./build/tests/bounded_polyline_spec.exe
./build/tests/round_join_candidate_spec.exe
./build/tests/round_stroke_surface_spec.exe
./build/tests/flux_shader_surface_spec.exe
./build/tests/flux_shader_surface_spec.exe --benchmark
./build/tests/flux_shader_surface_spec.exe --grouped-images
./build/tests/flux_shader_surface_spec.exe --batch-benchmark
```

The shader image command intentionally exits nonzero: the preserved screening
result is 154 failures among 1,140 images. Its lifecycle checks pass before the
final image rejection. Benchmark mode is separate; successful execution does
not mean the performance gate passes. Do not change gates to hide these results.
`--grouped-images` is a separate parity test against the same shader, not NanoVG;
it passes all 1,140 images exactly. `--batch-benchmark` measures the shared-scope
executor with real Flux geometry; the shader still loses to host NanoVG.

The full-stroke historical comparison additionally needs pristine upstream files:

```sh
mkdir -p work/nanovg
for file in nanovg.c nanovg.h nanovg_gl.h fontstash.h stb_truetype.h stb_image.h; do
  curl -fL "https://raw.githubusercontent.com/VCVRack/nanovg/0bebdb314aff9cfa28fde4744bcb037a2b3fd756/src/$file" -o "work/nanovg/$file" || exit 1
done
make -j6 build/tools/round_stroke_benchmark
```

`prepare_round_stroke.py` checks the pristine NanoVG C source hash before applying
its transformation. `prepare_flux_shader_geometry.py` needs only repository
sources, not downloaded NanoVG. Neither generator edits production sources.
The raw-log analyzer additionally requires the historical forward/reverse/tail
logs, comparison PPM and Rack dependency provenance file named in its source;
those are local evidence inputs, not included in a fresh checkout.

## Evidence and historical integration

Durable summaries and their manifest are in `doc/lumin-evidence/`. Raw captures,
images and logs remain locally under ignored `doc/benchmarks` and `work`; source
hashes inside historical summaries refer to the original experiment layout.

`historical-live-integration.patch` records the removed runtime glue against
checkpoint `d9a524c`. It references original paths and is an archival record,
not a patch to apply to this cleaned layout. The native experiment engine and
its modified vendored NanoVG remain here for offline comparisons. See
`LICENSE-NanoVG.txt` and the vendor source notices. They are not shipped in the
normal plugin after cleanup.

### Function-defined curve probe

`function_curve_candidate.hpp` and `tests/lumin_function_curve_spec.cpp` test
the analytic Flux/Proc function family. See `doc/lumin-function-curve-experiment.md`
for commands, failed image screens, timings and integration limits. This remains
offline-only. `prepare_proc_shape.py` extracts Proc equations for reference tests.
