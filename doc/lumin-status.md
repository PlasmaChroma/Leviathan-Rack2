# Lumin rendering: current status

Updated 11 September 2026, after the Flux native-framebuffer live comparison and user-authorized runtime consolidation.

## Git checkpoint versus working tree

Latest committed checkpoint: `7353d3b` (Checkpoint Lumin renderer experiments and host-queue bridge evidence), following `d9a524c` (Integral flux preview adapter). That checkpoint preserves the architecture, guarded shader experiments and offline callback probe. It predates the runtime host-stroke adapter, its live captures, and the Rack-native shader candidate and live results.

The retained implementation, tests and this status note remain working-tree changes, including untracked files. Detailed per-run reports and JSON results have been consolidated here and moved to ignored local `work/lumin-result-archive/`; they are not intended for the next commit. No staging or commit was performed. Keep generated builds/packages and raw captures out of Git. Preserve ignored raw inputs separately; the earlier checkpoint documents the `work/nanovg` reproduction dependency.

## Decisions

| Route | Evidence | Current disposition |
|---|---|---|
| Flux optimized stroke into host NanoVG queue | Live return-switch capture: contour median 18.05 us versus 29.05 / 38.9 us in flanking Standard intervals; zero fallbacks | Selected as the preferred Flux contour path, with automatic ordinary-NanoVG fallback. Rack 2.6.6 admission remains conservative; whole-module or FPS benefit remains unquantified. |
| Flux analytic shader through AdaptiveGlSurface | Visually encouraging; consistently expensive live | Removed from Flux runtime; retain standalone experimental reference. |
| Same Flux shader through Rack-native framebuffer | Repeated switches: contour about 187 us versus guarded 259-273 us; module Draw about 443-451 versus 523-541 us | Valuable execution-path finding, but still far more expensive than Standard NanoVG (19 us contour in the initial interval). Removed from Flux runtime; retain the standalone probe for other consumers. |
| Rack-native execution for other shader widgets | No cross-module live result yet | Carry forward as a hypothesis to test on a module already paying custom-GL overhead. Do not migrate AdaptiveGlSurface globally on Flux evidence alone. |
| General Lumin PolylineStroke | Retained prototype/specification; no accepted live implementation | Remains experimental. The function-specific shader and CPU bridge do not establish the general feature's acceptance. |

The native/guarded improvement repeats across switches with equal selected-row update counts and zero fallbacks. Standard appears only at the beginning of that capture, so its exact comparison is less controlled. All reported timings are CPU scopes, not GPU completion or FPS. The compact evidence record below preserves the comparison method and limits.

## What is preserved structurally

- Lumin source/primitive/execution separation and the existing preview adapter.
- Shared FunctionCurve shader and retained experimental primitives as reusable probes.
- Shared/restricted AdaptiveGlSurface batching and phase diagnostics; no global native-path port has been made.
- HostStrokeBridge runtime adapter, pinned CPU geometry source, conservative Rack 2.6.6 admission and ordinary-stroke fallback.
- NativeFunctionContour as a bounded Rack-owned framebuffer experiment with immediate changed-source updates, cache reuse, lifecycle handling and fallback.
- Actual bridge-use/fallback telemetry. The experimental contour selector and shader-specific runtime counters are removed. Historical capture parsing remains supported. Process/Step/Draw retain their module-level meanings.

Original segment pruning remains intact. Standard/bridge retain their existing 100ms settling behavior. The archived analytic candidates update changed contours immediately and cache unchanged images. History and the tracer marker were not replaced by these contour candidates. The host-stroke bridge is now preferred in the normal NanoVG contour path regardless of debug mode; failure uses the original stroke. Pre-existing legacy render settings remain compatible. The analytic backends are no longer selectable or instantiated by Flux.

## Next work

Preserve this result in the next user-owned checkpoint. For Flux, verify the consolidated build visually in Rack, including highlights, Shark Fin, zoom and window lifecycle. Its optimized path no longer needs a menu selection. Broader DAW/platform qualification remains outstanding; the CPU bridge owns no host GL resources. For broader rendering work, choose one existing shader consumer and compare its retained Rack-native path against AdaptiveGlSurface under matched workload, visual quality, cache cadence and context lifecycle. Target choice and migration are not yet decided.

Residual native render timing could be split into binding/clear versus shader submission if that helps the next consumer; it is no longer the required next step for Flux. The current native scope does not identify which of those dominates.

## Evidence and entry points

- [Historical checkpoint and research review](lumin-checkpoint-20260910.md)
- [Evidence index](lumin-evidence/README.md)
- [General PolylineStroke specification](lumin-polyline-stroke.md)

The latest native build passed focused Rack-linked tests, the analyzer suite (10 tests), and authoritative Windows plugin linking/packaging. No full test-fast pass or completed cross-platform/DAW lifecycle qualification is claimed. The installed native-test DLL hash differed from the packaged artifact; no byte-identity claim is made. Actual backend telemetry confirmed native execution.

## Consolidation validation

Removed Flux's analytic preparation/batching/presentation code, shader-owned widget state, contour-backend submenu and shader-only CSV fields. Retained the existing cache/source pruning/tracer ordering, normal fallback stroke, and bridge counters. `contour_backend=2` now denotes the preferred contour policy, while actual bridge/fallback counters determine work performed; pre-existing legacy OpenGL preview mode is still identified by `preview_render_mode`.

Native MINGW64 plugin link passed. The runtime bridge harness passed its image, inherited-state and replacement-host-context checks; the analyzer suite passed all 10 tests. Vendored unused-function warnings and the previously documented historical probe fontstash warning remain. No full test-fast result is claimed. No staging or commit was performed.

## Compact evidence record

Both decisive captures were taken in Rack 2.6.6 on Windows with Intel Iris Xe. Comparisons discard the first 60 and final 15 rows of each backend interval and retain rows with exactly two preview point rebuilds. Repeating with 30 or 120 leading rows discarded preserves the direction of the findings. These are observational CPU measurements; no deterministic replay, frame-rate win or GPU completion improvement is established.

Host-stroke return-switch capture: Standard / bridge / Standard, selected sample counts 454 / 394 / 254. Contour medians 29.05 / 18.05 / 38.9 us, p95 50.6 / 24.8 / 54.1 us. Module Draw medians 292.3 / 334.4 / 370.95 us rise through the sequence, as do history timings. Thus the bridge has a reversible local contour improvement (38-54%), but whole-module savings cannot be quantified from that capture. Zero bridge fallbacks. Twelve history trails; median submitted points 378 / 383 / 381.5.

Native execution capture: Standard / guarded / native / guarded / native. Selected sample counts 404 / 574 / 407 / 265 / 200. Contour medians 19.05 / 273.4 / 187.1 / 258.7 / 187.2 us; p95 55.65 / 397.27 / 359.73 / 387.98 / 328.07 us. Module Draw medians 214.2 / 540.7 / 442.5 / 523.4 / 451.3 us. Both previews actually update on every selected shader row, without fallback. Across all 1000 native rows, each preview is used 1000 times with zero fallbacks. Native improves on guarded contour cost by 28-32%; its residual native render scope is about 183.4-183.5 us for both previews. Standard appears only at the beginning, so its exact ratio is less controlled. These findings motivated preserving the execution experiment while removing Flux's shader candidates.

Reanalysis inputs are local, ignored captures under `doc/benchmarks/`. Source identities:

- Host-stroke return-switch: `integral_flux_draw_1_20260911_062138_0.csv`, SHA-256 `adff5886208c84181e36afb18075199110da859615a33525ed0e59100fc3e98d`.
- Native execution: `integral_flux_draw_1_20260911_064259_0.csv`, SHA-256 `1297fafb4715913aec2b917513c196ff6b23cfe6f28b3e48e5194a7662024f4b`.

The runtime bridge validation passed 1,500 exact image comparisons, 60 inherited-state comparisons, unchanged host callback-table checks and reuse after host NanoVG context replacement. Native-probe fractional-position images matched the direct shader byte-for-byte at .75, 1, 1.5 and 2 scale; this is parity with the shader, not proof of exact NanoVG parity. Native cache/context/fallback checks passed. The old explicit optimized bridge probe had a previously observed thin-stroke mismatch (max byte 27 at density 1.19); it is not the selected runtime adapter, and that historical failure was not resolved or waived.

The runtime adapter temporarily intercepts a synchronous host stroke callback to capture effective paint, tint, clipping, composite, AA and width, submits optimized CPU geometry, and restores the callback with scope-based cleanup. It owns no host GL resources and avoids reading opaque host-context layout. Admission remains restricted to the validated Rack release and open round-join/butt-cap preview strokes; failed admission uses ordinary NanoVG. This version gate is not a future ABI guarantee. Broader platform and DAW-window lifecycle qualification remains outstanding.

Per-run timing dumps, installation minutiae and historical test-menu procedures are intentionally excluded from the intended source checkpoint. The 11 detailed reports/JSON files remain locally under ignored `work/lumin-result-archive/` for optional inspection. Source, reusable tests, this guidance, and previously committed historical evidence are retained.
