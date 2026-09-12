# Lumin rendering: current status

Updated 11 September 2026, after the Flux native-framebuffer live comparison and user-authorized runtime consolidation.

## Git checkpoint versus working tree

Previous experimental checkpoint: `7353d3b` (Checkpoint Lumin renderer experiments and host-queue bridge evidence), following `d9a524c` (Integral flux preview adapter). That checkpoint preserves the architecture, guarded shader experiments and offline callback probe. It predates the runtime host-stroke adapter, its live captures, and the Rack-native shader candidate and live results.

The Flux consolidation is committed as `ce63fae` (Consolidate Flux rendering around optimized host strokes). The subsequent Proc/Undertow ports and this follow-up note remain working-tree changes. Detailed per-run reports and JSON results have been consolidated here and moved to ignored local `work/lumin-result-archive/`; they are not intended for the next commit. No staging or commit was performed. Keep generated builds/packages and raw captures out of Git. Preserve ignored raw inputs separately; the earlier checkpoint documents the `work/nanovg` reproduction dependency.

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

## Proc and Undertow follow-up

The user reported no observed problems during the consolidated Flux DAW lifecycle check on 11 September 2026. This is a successful practical check of that setup, not exhaustive qualification of all hosts/platforms.

Proc now applies the same HostStrokeBridge to its full/rising/falling preview contours, retaining the existing simplified paths, highlights, 100ms cache settlement, history and marker behavior. The shared geometry class remains unchanged so the ordinary NanoVG implementation remains the fallback/reference. No parameter IDs, state serialization or audio processing changed.

Undertow's main waveform is also an open round-join/butt-cap path. It now collects exactly the points emitted by its existing simplifier into a bounded stack array, then submits them through the bridge with the existing 1.25 width. Rejected strokes use ordinary NanoVG. Sampling, tolerance, history and label behavior are unchanged. Its existing draw CSV appends bridge_strokes/bridge_fallbacks. Simplify timing now covers collection; stroke timing includes bridge or fallback path submission, so those two component scopes should be considered together when comparing old/new captures. Module-level timing meanings are unchanged.

Validation: authoritative Windows plugin link passed; bridge harness passed 1,500 existing exact-image comparisons, 60 inherited-state cases, host-context replacement, and 180 added exact Undertow contour comparisons spanning shape/hardness/asymmetry and densities 1, 1.19, 2 and 4. Proc runtime tests passed including released schema/serialization and stress checks; Undertow shape tests passed 4/4. The historical explicit-probe fontstash warning remains unchanged. No full test-fast pass is claimed.

No new live performance improvement is claimed for Proc or Undertow yet. Reinstall and check both under modulation, Proc edge highlights, Undertow fold/asymmetry extremes, and DAW window reopen. Neither needs a debug switch for the optimization. The existing Rack 2.6.6 admission gate and automatic standard fallback apply to both. Raw validation output stays under ignored work/; no extra result-report files are added.

## ApertureLight investigation

The next shared-UI target is ApertureLight. Flux contains eight timed aperture lights. Its existing aperture_draw_us wraps the normal draw pass only, not the separate Rack light-layer bloom. The implementation has three framebuffer caches per light: static socket/lens, brightness-dependent normal core/crescent, and bloom. The normal cache dirties for brightness deltas above 0.0005 or summed RGB deltas above 0.001; this often requires a texture redraw for each changing LED. Specular remains direct. Brightness transfer already uses a LUT, so repeated powers are not the immediate issue.

An ignored offline Rack-linked probe in work/aperture_probe.cpp compares eight steady cached lights, eight changing cached lights, and the same changing lights with the normal cache bypassed. It uses real Rack framebuffers and explicitly renders dirty caches before presentation to avoid mock-window budget deferral; it is an equal-update cost probe, not a full Rack simulation. Bloom is excluded, matching the existing Flux normal-pass scope. Runs use three rotated orders, 100 warmup frames and 360 measured frames per condition. The final run's normal-pass medians were about 7 us steady, 31 us changing/cached, and 18 us changing/direct. Including host NanoVG flush: about 33 us changing/cached versus 23 us changing/direct. Cached p95 approached 0.9-1.0 ms versus 24-37 us direct in that fixture. An earlier run had overlapping median ranges but retained much larger cached tails, so absolute speedup is not established for live Rack.

A sampled opaque-panel image comparison at integer scale, Small size and default teal color had maximum component error 1 byte between direct and cached rendering. This is initial screening only; other sizes/colors, fractional transforms, clipping, inherited alpha/tint/compositing and context lifecycle remain to be checked. The shared production renderer has not changed. Local fixture setup crashes were resolved (mock window dirty-budget access and missing event state); they were not production ApertureLight failures.

Finding: frequently invalidated tiny normal-layer caches are a plausible shared cost source. Next candidate is direct normal-layer drawing while changing, retaining cache benefits when stable and preserving the static socket. Bloom is a separate investigation. Do not remove all caching or assume the narrow image check establishes general parity. Detailed probe outputs remain ignored in work/ rather than new per-run repository reports.

## Aperture settling implementation

User-authorized follow-up: the shared ApertureLight normal and bloom layers now draw directly until their source has been stable for 100ms, then build/reuse their respective framebuffer images. Existing brightness/color change thresholds remain intact. The static socket cache stays independent. Source changes resume direct drawing immediately, so an old cached brightness is not displayed while settling; cache rebuild on settlement is explicit rather than accepting Rack frame-budget deferral. Bloom-off resets its settlement, and context events invalidate both dynamic sources. Size changes restart the corresponding settlement. The renderer validates cached image ownership/size using the shared lifecycle helper before reuse.

Inherited tint or transparency retains the original cached composition route even during animation: the image screen blends differently from individually tinted/faded overlapping shapes. This exception avoids the larger differences found in the initial broader image screen. Nested-framebuffer and rotated drawing continue through Rack's direct bypass behavior. Both independent delays are local UI state and introduce no saved setting or module parameter changes.

Validation: aperture_light_settling_spec exercises initial direct rendering without allocating dynamic FBOs, continued direct rendering before the threshold, stable cache creation/reuse, resumed animation, bloom-only setting invalidation, off reset and context destruction/recreation. Image screening covers four sizes, four scales, three brightness values, clipping, fractional placement, transparency and RGB tint (144 comparisons); maximum observed component difference is 5/255, so exact byte parity is not claimed. Native Windows plugin link and the existing transfer LUT test pass. Live Rack/DAW appearance and performance still need confirmation after installation. No new standalone result-report files are added; this implementation is uncommitted.

## Aperture layer simplification

Removed the dynamic black crescent from the shared ApertureLight renderer. Its two circles plus hole fill used screen blending (ONE_MINUS_DST_COLOR, ONE), so black contributes no RGB darkening. The visible core, specular, bloom and static socket remain. This change complements the settling delay; it removes work whenever the dynamic normal layer is drawn, including cache rebuilds.

The retained aperture_light_layers_spec reconstructs the old crescent as a reference and compares against production rendering. All 960 final-image comparisons were byte-identical on an opaque panel background, across four sizes, four scales, five brightness values, three mixed-color configurations, full/half opacity, fractional placement and direct/cached routes. This is the scope of the exact-pixel claim; arbitrary destination alpha/compositing is not exhaustively covered.

Eight changing Small lights, 600 timed frames after 100 warmup frames, four alternating baseline/candidate orders: the final run measured normal dynamic submission medians of 35.7-37.1 us with the old layers versus 19.8-20.8 us after removal (about 44% lower). Including host flush, medians were 43.7-46.7 versus 23.8-24.7 us. Earlier runs had lower absolute timings but a similar relative submission reduction. The fixture excludes static-socket presentation and bloom, and these are CPU observations, not a live module/FPS speedup. Raw output remains in ignored work/.

The settling regression still passes all 144 comparisons (maximum component delta 5/255 between direct and cached routes) plus lifecycle/settlement checks. Native plugin linking and packaging were verified for the combined changes. Live appearance and timing remain the next check; nothing was staged or committed in this follow-up.
