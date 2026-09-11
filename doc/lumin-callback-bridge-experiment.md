# CPU stroke geometry bridge to Rack's NanoVG queue

10 September 2026. Offline proof of concept passes correctness checks and shows
a CPU benefit. No runtime integration, menu option, installation or settings change.

## Mechanism

`tools/experiments/lumin/callback_bridge.cpp` compiles the existing hash-pinned,
namespaced NanoVG geometry core with the earlier endpoint round-join optimization.
It does not compile a private GL backend. Its private context generates solid
stroke geometry and invokes the host's `NVGparams::renderStroke` callback with
the host backend user pointer. Rack's backend copies the resulting vertices and
uniforms into its normal queue, preserving insertion order with host commands.

The adapter copies only the stroke callback and anti-alias setting. It never
overwrites the host callback table, accesses the opaque host context layout,
creates an offscreen surface, captures/restores GL state, or invokes the host's
viewport/flush/cancel/delete callbacks. The private core's unused font atlas uses
dummy texture callbacks; it creates CPU data but no GL texture. The API is expressly
stroke-only, not a general text/image/fill backend.

The callback/user pointer are obtained anew on submission and cleared afterward.
Private context destruction owns only private CPU allocations. Tests reuse it
successfully after destroying and recreating the host NanoVG context. Native GL
context recreation and DAW lifecycle have not yet been tested for this adapter.

## Correctness

Native Windows test on installed Rack 2.6.6, Intel Iris Xe, driver 32.0.101.7088.
The geometry generator extracts the existing Flux sampling and simplification;
this experiment does not change point counts or legacy pruning.

3,000 image comparisons passed with **zero byte differences** for both the
unoptimized bridge and optimized bridge versus installed NanoVG. Coverage:

- Both Maths and Shark Fin source curves, 30 changing states per mode.
- Densities 1, 1.19, 2, 4 and 8.
- Fractional placement, rotation, nonuniform scale and skew.
- Explicit transformed scissor, global/color alpha, source-over and additive blend.
- Thin and wider strokes, butt and round caps, round joins.
- Two preview strokes interleaved with host fills before and after each stroke.
- Unchanged host callback table and framebuffer/program/array-buffer bindings.

The image gate retained the prior <=0.1% normalized error / <=16 byte tolerance;
the observed result is exact. This is a finite screen, not proof for every path,
clip stack, paint, cap/join configuration or host version. The new screen does
not replace the earlier sharp/duplicate/self-overlap stroke tests.

## Performance

Three modes: installed NanoVG; unoptimized private geometry through host queue;
optimized private geometry through host queue. Two shapes, two densities, three
rotated-order repetitions, 100 warmup plus 360 measured frames each. Two strokes
and interleaved fills per frame. CPU includes clear, frame setup, explicit state,
path commands, geometry expansion, submission and host end-frame. Source point
generation is outside the timed scope. GPU queries are retrieved asynchronously.

Final validation run, range of median CPU times across three repetitions:

| Shape | Density | Installed NanoVG | Optimized bridge |
|---|---:|---:|---:|
| Shark Fin | 1x | 32.9–52.5 us | 26.4–33.2 us |
| Shark Fin | 2x | 35.8–70.6 us | 14.1–29.3 us |
| Maths | 1x | 35.4–44.3 us | 17.0–28.0 us |
| Maths | 2x | 29.5–41.1 us | 15.4–33.4 us |

All 12 matched comparisons improve median CPU (8.8–61.6%) and p95 (5.2–73.0%).
The median of paired median reductions is 33.6%, not a pooled-frame statistic.
The initial run also improved all 12 median comparisons, but by a different range
(18.5–76.0%). Variation is material; no fixed expected live speedup is promised.
The unoptimized bridge alone is not consistently faster. This supports preserving
the earlier geometry optimization through host submission, rather than crediting
callback indirection itself. GPU timings vary; no consistent GPU win is claimed.

Neither run measures a complete module, existing cache-settling behavior, trail
cost, inherited-state extraction cost in a live widget, or a busy Rack host.

## Remaining integration gate

The harness **explicitly supplies** transform, scissor, alpha, density, paint and
composite state. The private context cannot automatically inherit those from the
host's opaque context. `nvgCurrentTransform` is available, but that does not solve
all inherited state. Determine a correct state contract for the live widget
without silently discarding clipping or alpha, reading version-specific host
memory, or globally intercepting callbacks.

`nvgInternalParams` is an internal backend interface exposed by the installed SDK,
not a demonstrated stable cross-version plugin extension contract. The probe uses
the exact installed revision's structures. Admission/version compatibility and
fallback policy need explicit treatment before shipping.

The next step is a restricted live adapter only after resolving that state
contract, then a separately identified backend choice and matched Rack capture.
Do not replace the current analytic shader option. Deep research may identify a
better integration route; this result adds concrete evidence in parallel.

## Reproduction and artifacts

Build with native MINGW64: `make -j6 build/tools/callback_bridge_spec`.
Run `build/tools/callback_bridge_spec.exe` with Rack2Pro's runtime first on PATH.
`make plugin.dll` is up to date; no production source changed. Full test-fast was
not run for this isolated experiment. The pinned upstream fontstash source emits
a GCC use-after-free warning in its cleanup routine; that source was not changed
by this experiment and must be reviewed before promoting the new core to runtime.

Source, binary and log hashes plus all CPU/GPU metrics:
`doc/lumin-evidence/callback-bridge-results.json`.
Local logs: `work/callback-bridge-results.txt` and
`work/callback-bridge-final-results.txt`.
