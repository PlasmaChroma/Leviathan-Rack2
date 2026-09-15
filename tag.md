# Lumin-Render: implementation plan and review

## Objective and current status

Reduce rendering cost while preserving host GL state, image quality, and graphics-context recovery. Performance claims need measurements on the native Windows toolchain and the actual GPU; fewer source-level calls alone do not establish a speedup.

This plan supersedes the earlier completed checklist. The first implementation pass repairs the confirmed state-restoration regression; the later optimization stages remain pending.

| Area | Current evidence | Next action |
| --- | --- | --- |
| `SurfaceStateGuard` repair | Strengthened GL tests, both Windows builds and routine suite pass; batching measured | Manual Rack/editor-recreation check remains |
| Scissored surface clear | Active region plus a one-pixel border is implemented | Verify retained-buffer pixels and measure GPU cost |
| Pressure-dependent density | Policy field and calculation exist; no producer sets this policy field | Add one explicit pilot after state tests pass |
| Bifurx log approximation | Implemented in both render-preparation paths | Share implementation; verify float error and benchmark |
| Dynamic ceiling shortcut | Compares peak against previous ceiling minus 6 dB | Replace with a policy that accounts for spectrum shape |
| Build and routine tests | Earlier Windows builds and `test-fast` passed | Use the new `test-render` target as well; routine tests do not cover GL state |

## 1. Repair GL state isolation before further optimization

Files: `src/visual/AdaptiveGlSurface.cpp`, `.hpp`, `src/visual/Eclipse2RuntimeBake.cpp`, and their focused GL tests.

### Implementation direction

1. Write a state-ownership table beside the guard contract. Audit mutations in `beginShaderPass`, framebuffer creation, `renderImpl`, and every callback. Include nested NanoVG rendering and readback in Eclipse2 baking. Each state must be restored by the shared boundary, restored locally by its owner, or explicitly forbidden by a restricted callback contract.
2. Restore the compatibility path's prior coverage first. Retaining the previous attribute-stack guard for general callbacks is an acceptable recovery step. Keep capture/restore once per batch and preserve the zero-GL-work cache-hit path. Removing `glPushAttrib` is not itself an acceptance criterion.
3. Keep a smaller explicit guard only for audited, opt-in shader callbacks. A mixed batch must use the general contract. Honor the selected contract consistently for single and batched rendering; do not silently weaken existing callers.
4. Cover the actual mutated state, including:
   - Draw/read framebuffer and renderbuffer bindings; restore framebuffer-dependent selections against their owning framebuffer.
   - Program, array buffers, vertex-attribute enables and pointer/buffer descriptors for the declared indices. Do not assume the next host operation is a NanoVG flush that repairs them.
   - Active texture unit and bindings on every permitted texture unit.
   - Pixel pack/unpack buffer bindings and modified pixel-store values: alignment, row length, skipped rows/pixels, byte swapping and bit order where applicable. Eclipse2 changes pack state and currently expects the outer guard to restore it.
   - Color write mask, clear color, blend factors/equations, scissor box and viewport.
   - Alpha/depth/stencil/cull enables, polygon mode, and callback-modified stencil functions/masks/operations. General NanoVG callbacks require more than the restricted shader subset.
   - Matrix mode and affected matrices on compatibility paths; callbacks must balance matrix stack depth.
5. Define the neutral state between callbacks separately from the host restoration contract. A batch callback that changes upload state must not contaminate the next callback, even when the outer host state is restored correctly at batch exit.

### Required verification

- Extend `tests/adaptive_gl_batch_spec.cpp` to seed non-default host state, invoke single, restricted, general, and mixed batches, and compare the promised state afterward.
- Keep the existing poisoned first callback test. Assert upload row length explicitly so its failure is distinguishable from scissor failure.
- Add meaningful coverage for color masks, pack/unpack settings, attribute descriptors, and a nested NanoVG/readback callback representative of Eclipse2.
- Exercise early returns and allocation failure, not just successful draws.
- Retain grouped-versus-independent pixel comparisons and cache-hit assertions.
- Use `GlLifecycleUtils.hpp`, `NvgGraphicsLifecycle.hpp`, and the existing deferred-retirement helpers. Keep context ownership checks and lazy recovery; do not replace them with a context-pointer-only assumption.

**Exit condition:** focused state and lifecycle tests pass with no GL errors, including the previously failing upload-state assertion. Benchmark only after this contract is restored.

## 2. Validate scissored clears independently

Files: `src/visual/AdaptiveGlSurface.cpp`, `tests/gl_surface_lifecycle_spec.cpp`, and the batch image test.

- Keep the current active-viewport-plus-border approach as a candidate. Derive the clear rectangle from the same actual backing capacity and viewport origin used by the draw callback.
- Test grow/shrink/grow cycles with retained capacity and both front/back buffers. Seed unused pixels with a conspicuous color, then verify they never enter the sampled image.
- Cover fractional zoom, pixel ratio, quantum rounding, alpha, non-square surfaces, and active sizes touching capacity boundaries.
- Compare visible output against a full-clear reference. The unused texture interior need not be transparent; the sampled boundary must be correct.
- Measure full and scissored clears at identical backing sizes, active sizes, and callback workloads. Partial clears can have driver-dependent costs; keep the faster verified option for the measured workload.

**Exit condition:** no old-pixel fringe or changed alpha at supported presentation scales, with recorded CPU/GPU timings. Avoid claiming a fill-rate improvement before this comparison.

## 3. Make worker optimizations correct and measurable

Files: `src/BifurxRenderPrep.cpp`, the matching helpers in `src/Bifurx.cpp`, and focused render-preparation tests.

### Shared logarithm approximation

- Factor the duplicate approximation into one small inline Bifurx helper so worker and fallback paths stay identical.
- Preserve both FFTs for ordinary module rendering: the measured response still colors the spectral fill when its response line is hidden. Preserve the existing display-only exception.
- Use a defined bit-copy operation such as `memcpy` for float representation access; confirm optimized native compilation removes copy overhead.
- Specify the supported input domain and the existing low-energy floor. Verify behavior at powers of two, near the floor, across actual energy/ratio ranges, and for invalid values according to an explicit policy.
- Add an actual float implementation sweep against `10.f * std::log10(x)`. A preliminary mathematical mantissa sweep found about **0.085253 dB** maximum error, so the old `<0.085 dB` claim is too strict. A provisional **0.09 dB** normal-domain budget is reasonable, subject to the float sweep; below-floor clamping is a separate intentional behavior.
- Benchmark complete preparation snapshots as well as the helper. Keep representative quiet, tonal, broadband and changing spectra, warm-up, repeated trials, and consumed outputs so the compiler cannot remove the work.

### Dynamic ceiling policy

- Remove the current early return as the correctness baseline. `previousTopTargetDbfs - 6.f` is not a saved previous peak: the ceiling also depends on the spectrum percentile, peak headroom and clamps.
- A previous-peak cache alone is insufficient. A tone and broadband signal can have the same peak but different percentiles; a peak-only shortcut can hold the wrong scale indefinitely.
- Measure percentile selection cost first. If it is material, use a bounded refresh interval with immediate refresh on first frame, configuration changes and significant peak growth. This bounds the stale percentile even when the peak stays constant.
- Apply peak headroom and range clamps on every snapshot. Document attack/release behavior and a maximum refresh delay. Start with an explicit provisional delay budget of 50 ms and validate it visually before adopting it.
- Test constant peak with changing spectral distribution, rising/falling levels, silence, scale-mode toggles and floor/ceiling transitions. Compare the optimized target trajectory with the unskipped baseline.

**Exit condition:** both preparation paths agree, approximation error meets its stated budget, ceiling response remains bounded, and end-to-end timing justifies the extra logic.

## 4. Connect adaptive resolution through one pilot

Files: `src/visual/AdaptiveGlSurface.hpp`, `.cpp`, one selected caller, and its existing pressure/update policy.

- Leave the default pressure at zero. Select one opt-in surface that presents through `draw()` for the first pilot. `drawAligned()` assumes the supplied density matches the raster; audit and update that contract before enabling pressure on aligned contours.
- Feed a stabilized level from the existing `VisualFramePressure` mechanism at preparation time. The similarly named field in `SettledContourFramebuffer` is a different policy and does not connect this new setting automatically.
- Use a four-entry density-factor table for levels 0..3: approximately `1.0`, `0.7071`, `0.5774`, `0.5`, avoiding a per-preparation square root. Retain the caller's minimum legible density.
- Change only the effective maximum. Below that cap, requested density should remain unchanged. The ideal pixel-area factors are `1`, `1/2`, `1/3`, `1/4` before minimum-density and extent rounding effects.
- Add hysteresis and slower recovery to the pressure producer. Do not rebuild or oscillate allocations on every frame. Reuse retained backing capacity while reducing active raster dimensions.
- Schedule redraw when the effective raster dimensions change; the current clean-cache early return only responds to growth. Avoid invalidating when a pressure-level change produces the same dimensions.
- Verify text/line legibility, transitions, zoom recovery, context recreation, and absence of stale pixels. Do not silently enable reduced quality on released modules before these checks.

**Exit condition:** the pilot actually receives nonzero pressure, reduces measured rendering cost under load, and returns to full quality without thrashing or misaligned presentation.

## 5. Validation and measurement protocol

### Commands available now

Run inside native MSYS2 MINGW64 with the installed Rack runtime first in the test process path:

```sh
make build/tests/adaptive_gl_batch_spec
PATH="/c/Program Files/VCV/Rack2Pro:$PATH" ./build/tests/adaptive_gl_batch_spec
make test-render RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"
make -j10 test-fast RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"
make -j10 plugin.dll
```

`test-gl-batch` now uses the existing `run_rack_test_bin` helper. The opt-in `test-render` target groups it with `test-gl-lifecycle`. Do not imply that `test-fast` exercises real GL state restoration.

### Benchmark and rollout requirements

- Use `adaptive_gl_batch_spec --benchmark` after correctness passes, comparing a correct baseline and each optimization separately. Record revision, compiler flags, GPU/driver, Rack version, surface count, active/backing sizes, zoom and pressure.
- Preserve module-level `Process`, `Step`, and `Draw` telemetry semantics. Report capture, setup, target, callback and restore timings as additional component metrics. Gate live debug reporting through `isDragonKingDebugEnabled`.
- Report CPU median/p95 and GPU timings separately. Use delayed GPU query collection; do not add synchronous waits to production rendering. Include static cache hits, one dirty surface, and larger dirty batches.
- Run a manual Rack check for Bifurx, Eclipse2 consumers and TD.Scope, including zoom, module removal, and editor/context recreation where supported. A successful build is not this visual check.
- Keep the implementation stages separately reviewable: state repair; clear optimization; worker changes; adaptive pilot. Sync shared changes into Pro only after the relevant validation, then build both Windows plugins.
- Do not retain a claimed speedup whose change is within run-to-run noise. Record actual results and unresolved limitations here when each stage completes.

## Review corrections to the previous narrative

- Current Bifurx uses a 4096-point FFT and **2049 bins**, not 513.
- The previous float expressions select the float `std::log10` overload; the document supplied no evidence of double-precision conversion overhead.
- The guard still queries four attribute enables in a batch. Attribute enables use `glGetVertexAttribiv`, not `glIsEnabled(GL_VERTEX_ATTRIB_ARRAY_ENABLED)`.
- The approximation contains a floor branch. The earlier **15x** speedup is unverified in this review.
- Broad claims about GL driver software emulation or guaranteed query stalls are hypotheses to measure on the target machine, not demonstrated causes.
- Earlier successful builds and routine tests did not detect the GL regression; the focused graphics tests are now a separate required validation step.

## Implementation record: state restoration

- Reproduced the original native test failure before changing the guard.
- Restored full compatibility attribute/client coverage for general callbacks and the narrower attribute groups for restricted shader callbacks. Explicitly preserve generic vertex descriptors and pack/unpack buffer bindings. Mixed batches retain the general contract; single calls now honor `shaderOnlyState` too.
- Preserved one host boundary per dirty batch and zero capture/render work for clean batches. The scissored clear remains in place.
- Expanded the batch regression test across independent general/restricted calls, general/restricted/mixed batches, poisoned callback state, and a private NanoVG recorder with pixel readback matching Eclipse2's usage. Verify distinct front/back stencil settings, pixel-store layout, color state and generic inputs, plus identical output pixels.
- Updated the lifecycle test to check the sampled clear border, including poisoned/reused buffers, rather than requiring unused texture interiors to be cleared. Existing allocation-failure, context-loss and retirement checks remain active.
- Native results so far: GL lifecycle **549 checks passed**; expanded GL batch test **passed**, including nested NanoVG/readback and no GL errors.
- This is a correctness repair with batching preserved. It does not establish that the restored guard is faster than the incomplete guard it replaces.
- Both normal Leviathan and synced Leviathan-Pro Windows DLL builds passed. Native `test-fast`: **109,950 checks, zero failures**. Diff whitespace checks passed in both repositories. Nothing installed or committed as part of this implementation pass.

### Corrected-renderer batching measurements

Native MINGW64 `-O2` test binary, NVIDIA GeForce RTX 3090, GL 4.6 driver 560.94. Current worktree based on `24d64a0` with the state repair above. Raw output: `build/render-state-benchmark.log` (local build artifact). Validation logs: `build/render-state-validation.log` and `build/render-state-pro-build.log`.

Each configuration uses 460 frames, discards 100 warm-up frames, and repeats three times with alternating execution order. Surfaces are 106x48 at density 1, all dirty, with a simple shader and NanoVG presentation. The table reports the median of the three per-trial medians in microseconds; it is not a whole-module frame benchmark.

| Surfaces | Separate CPU | Grouped CPU | Separate GPU | Grouped GPU |
| --- | ---: | ---: | ---: | ---: |
| 1 | 23.9 | 24.4 | 9.216 | 9.216 |
| 2 | 85.0 | 25.5 | 17.408 | 14.336 |
| 8 | 448.7 | 33.3 | 53.248 | 33.792 |
| 32 | 977.1 | 441.1 | 673.792 | 111.616 |
| 64 | 1994.0 | 901.6 | 1664.000 | 494.592 |

At 64 surfaces, median trial CPU p95 was 2311.5 us separate versus 1057.7 us grouped; GPU p95 was 2455.552 versus 974.848 us. The full output includes every trial's median and p95. Single-surface results show no useful improvement; larger batches benefit in this fixture. Timing is not smoothly proportional to count, so do not extrapolate these ratios to Rack workloads or claim a guard-specific speedup. Both sides use the repaired guard; no comparison against the state-leaking implementation is presented as a valid optimization win.

The next performance experiment should compare full versus scissored clears at fixed backing/active extents on this correct baseline, then assess whether any additional guard optimization is worth its complexity. Real Rack zoom and editor-recreation checks remain manual follow-up work.
