# Bifurx premium release review

Review date: 2026-09-15  
Reviewed revision: `ebeaca7fb8ba4b9ecde11e0cbff0890764804e17`  
Reviewer: Codex / Nexora Lumineth  
Disposition: **Do not ship this revision as the finished Premium release.**

This is a fresh review of the current implementation, not a restatement of the resolved findings in `doc/bifurx-sol-h.md`. Several earlier fixes are sound and remain valuable. New production-code probes nevertheless reproduce substantial clean-path high-frequency loss, stale polyphonic output after bypass, a long-session integer overflow, incorrect low-frequency response display, and a graphics-state-dependent blank spectrum. There are also concrete concurrency defects, analyzer inaccuracies, and avoidable performance costs.

The review is complete within the scope below. Release qualification is not complete: successful paid activation, real DAW editor lifecycle testing, listening approval, and other supported operating systems require further evidence. Those are explicitly identified as release gates rather than represented as tests that passed.

Implementation follow-up (2026-09-16): see [the implementation and audition record](bifurx-premium-implementation.md) for current fixes, candidate A/B behavior, Linux evidence, and remaining gates. The findings below describe the reviewed revision and are retained as historical evidence. DRM is explicitly deferred until a test key is available; the mono contract now includes bypass.

## 1. Scope, evidence, and severity

Reviewed the complete Bifurx source family: `Bifurx.cpp/.hpp`, input/output stages, oversampling, transition smoothing, UI, OpenGL renderer, render data/preparation, worker service, and licensing wrapper. Followed its directly relevant dependencies into the Rack SDK resamplers/ports/math, shared GL surface and resource retirement, NanoVG ownership, raster loading, panel anchors, premium staging/synchronization, Pro bootstrap/manifest, and vendored DRM integration. Reviewed both Bifurx DSP test suites, the license tests, relevant staging/lifecycle tests, the earlier engineering review, development documentation, and the local public-manual source and panel screenshot in `../Leviathan-Pages/manuals/`.

The shared visual library is much larger than Bifurx. This review follows the Bifurx usage and its resource/lifecycle boundaries; it is not a claim that every unrelated shared widget or other module has been exhaustively audited.

Evidence classifications:

- **Reproduced:** exercised against current production code, native Windows code, or actual OpenGL as described.
- **Source-confirmed:** the conflicting operations or incorrect formula are directly present; no customer-visible failure is claimed unless reproduced.
- **Qualification gap / product decision:** evidence or a defined behavior is still needed; this is not a claim that the untested configuration fails.

Priority definitions:

- **P1:** resolve before premium release. Significant audio/display correctness, undefined behavior, thread safety, or reliable rendering is affected.
- **P2:** fix or explicitly resolve before release. Includes analyzer accuracy, verification gaps, and meaningful performance/UX shortcomings.
- **P3:** measured optimization or polish; can be scheduled after the correctness gates if the documented performance budget passes.

Source references use the reviewed revision's line numbers. Names and reproductions are also given because line numbers will move during implementation.

## 2. Executive findings

| ID | Priority | Finding | Evidence |
|---|---|---|---|
| PREM-01 | P1 | Always-on boundary resampling removes substantial clean treble and adds undocumented delay | Native measurement, including complete module |
| PREM-02 | P1 | Leaving polyphonic bypass leaves extra output channels carrying stale voltages | Native reproduction |
| PREM-03 | P1 | Quality menu writes an audio-owned non-atomic cache flag | Source-confirmed data race |
| PREM-04 | P1 | Renderer inherits GL state that can suppress its entire spectrum | Actual native OpenGL reproduction |
| PREM-05 | P1 | Audio reads unsynchronized mutable DRM state during background license refresh | Source-confirmed integration hazard |
| PREM-06 | P1 | Stationary-preview sample counter overflows in long sessions | Native accelerated reproduction |
| PREM-07 | P1 | Nominal response calculation loses tens of dB at low frequencies/high rates | Native production audio/display comparison |
| PREM-08 | P2 | Analyzer extrapolates below FFT bin 2 instead of interpolating valid bins | Reproduced arithmetic |
| PREM-09 | P2 | Spectrum labeled dBFS lacks an accurate, defined amplitude calibration | Native coherent-tone measurement |
| PREM-10 | P2 | Renderer color laws and setting invalidation disagree | Source-confirmed |
| PREM-11 | P2 | Marker placement can misrepresent frequency and alter the nominal curve | Source-confirmed; product accuracy decision |
| PREM-12 | P2 | Standalone test model has drifted; critical production contracts are untested | Source-confirmed |
| PREM-13 | P2 | Non-finite CV is interpreted as maximum modulation | Native reproduction |
| PREM-14 | P2 | Performance recording has competing consumers and incomplete rendering totals | Source-confirmed |
| PREM-15 | P2 | Audio performs unused debug/visual work, including in Display Only | Source-confirmed and timed |
| PREM-16 | P2 | Offscreen modules can continue analysis, worker, and GL rendering work | Source-confirmed scheduling gap; cost needs Rack profiling |
| PREM-17 | P3 | Worker and renderer rebuild reusable data and some unused geometry | Source-confirmed optimization opportunities |
| PREM-18 | P2 | Worker/animation state does not consistently identify the data being displayed | Source-confirmed |
| PREM-19 | P2 | Sound-affecting menu actions lack coherent undo/redo behavior | Source-confirmed |

There are no claims here that mono processing itself, the defined divided-rate quality choices, or the established 3 ms topology transition are defects. Those are legitimate product decisions and the manual largely explains them.

## 3. Validation performed

All Windows commands used the documented MINGW64 bridge, with the Rack application directory first in the execution path for Rack-linked tests. No plugin was installed, no license was changed, no source was synchronized into Pro, and no files were staged or committed. Production source was not changed.

| Validation | Result | Meaning / limit |
|---|---|---|
| Native `make -j10 plugin.dll` | Passed; existing Windows DLL up to date | Authoritative incremental Windows build check; not a clean release rebuild |
| Native complete `test-fast` | Passed, exit 0 | Includes the filter-model suite; Bifurx's production runtime suite is assigned to `test-rack`, not this target |
| Separately rebuilt native Bifurx suites | 31 filter-model tests; 39 runtime tests passed | Fresh isolated `.exe` outputs; runtime includes current `Bifurx.cpp` and uses shipping optimization flags; standalone suite partly models different code |
| Native `premium-dist test-premium` | Passed | Real DRM enabled; isolated Premium DLL linked and archive packaged |
| Real DRM negative-path tests | All 5 checks passed | Missing context/license silence, stale channel clearing, expected license path, and unlicensed bypass gating |
| Native `test-gl-lifecycle` | 549 checks passed | Real GL/NanoVG shared-helper lifecycle test, not the complete Bifurx renderer |
| Added review GL diagnostic | Blank spectrum reproduced with inherited stencil test | Exercises actual `BifurxSpectrumGLWidget::renderGlContent()` through `AdaptiveGlSurface` |
| Added native audio stress diagnostic | 2,880,000 samples; zero non-finites; zero output samples above 5 V | 10 audio modes × 3 TITO positions × 4 sample rates; limiter on and self-osc enabled, changing controls and ±10 V input |
| Added native transfer, bypass, CV, counter, and analyzer probes | Findings reproduced below | Production code; audio probes built with `-O3 -funsafe-math-optimizations -march=nehalem` |
| Panel anchor atlas regenerated to a temporary file | Byte-identical to checked-in atlas | Current SVG asset hashes and generated anchor table agree |
| Master SVG split regeneration in a temporary directory | Not completed | Both MSYS Inkscape and installed Windows Inkscape text outlining timed out after 30 seconds; no runtime asset was overwritten |

Premium package produced: `build/premium-validation/dist/Leviathan-Pro-2.1.0-win-x64.vcvplugin`. It is a build artifact, **not an approved release candidate**.

Review diagnostic sources and logs are retained under `build/bifurx-review/`; Premium validation output is in `build/bifurx-review-premium.log`. These are ignored development artifacts, not permanent regression tests. The important setup, results, and required regression contracts are recorded in this document so its conclusions do not depend on retaining those artifacts.

The stress test demonstrates useful finite/bounded behavior for its inputs. It does not establish sound quality, all possible parameter combinations, limiter-disabled maximum levels, sustained extreme CV behavior, or successful activation.

## 4. Release blockers

### PREM-01 — The oversampling boundaries impose a large hidden low-pass response

**Locations:** `src/BifurxOversampling.hpp:12–54`; `src/Bifurx.cpp:1333–1335,1423–1426`; installed Rack SDK `include/dsp/resampler.hpp:112–179`.

Both the LEVEL boundary and output boundary always run a `dsp::Upsampler<2,8>` followed by `dsp::Decimator<2,8>` in production. They run even when LEVEL is exactly clean unity and Soft Limiting is disabled or never reaches its knee. The SDK's default cutoff is 0.9; each helper uses a short, 16-tap windowed-sinc kernel. Consequently, a clean path passes through four reconstruction/anti-alias filters. The wrapper's description of these as polyphase helpers overstates the implementation: the inspected SDK describes its current convolution as naive and contains a TODO for a polyphase hierarchy.

Measured at 48 kHz with a 1 V peak sine through `processOutput(processInput(x, 0.5), 0.5, false)` after settling:

| Frequency | Boundary-pair gain |
|---|---:|
| 100 Hz | -0.0148 dB |
| 1 kHz | -0.0201 dB |
| 10 kHz | -1.6500 dB |
| 18 kHz | -12.2181 dB |
| 20 kHz | -16.8050 dB |

A separate complete-module probe used High + High, a 20 Hz center, zero SPAN/RES/BAL/TITO, LEVEL 0.5, and a 0.05 V peak sine. It measured -1.6499, -12.2174, and -16.8040 dB at 10, 18, and 20 kHz respectively. This confirms the loss is in the audible production path, not merely an isolated test helper.

The clean boundary pair's impulse peak occurs at sample 14: approximately 0.292 ms at 48 kHz. Display Only is immediate pass-through after its transition. Parallel dry/processed mixing therefore has an additional timing/phase difference beyond the intended filter phase. The nominal gold curve includes neither this rolloff nor the boundary delay.

**Customer consequence:** clean high-pass and notch configurations unexpectedly darken material; disabling limiting does not remove the rolloff. The spectrum can show attenuation far beyond what the nominal curve implies. Calling the memoryless input helper “unity” does not establish unity for the whole resampled path.

**Required work:** retain anti-aliasing benefits, but redesign and specify the boundary resampling. Measure candidate FIR/IIR or combined-stage designs for passband, alias rejection, latency, phase, and cost. Do not simply remove oversampling: existing evidence demonstrates worthwhile alias reduction. Do not introduce signal-dependent bypass without delay matching, filter-state continuity, hysteresis, and transition tests. A static clean-path optimization is only acceptable if changes of LEVEL/limiter state cannot create timing discontinuities.

**Acceptance:** add full-module low-level sweeps at 44.1/48/96/192 kHz, not just individual nonlinear-helper tests. Proposed release target: unintended boundary coloration within ±0.25 dB through 16 kHz at 44.1 kHz and through 18 kHz at 48 kHz; document the remaining transition band. Record impulse/group delay and matched alias tests at multiple fundamentals and drive levels. Approve any different passband target explicitly as voicing. Update the nominal-response contract/manual to account for the final fixed processing chain. Keep the ±5 V physical-output guarantee with limiting enabled.

### PREM-02 — Polyphonic bypass leaves stale extra channels on return to processing

**Locations:** `src/Bifurx.cpp:718–721` constructor/configuration, `1157–1165` bypass, `1428` normal output write; Rack SDK `engine/Port.hpp:156–173`.

The module configures a bypass route that can copy a polyphonic input. Its normal `process()` writes only channel 0 and never restores mono output channel count. The constructor's `setChannels(1)` cannot repair a later bypass transition; it also does nothing while the port is disconnected. The unlicensed path correctly clears channels, but the ordinary licensed path does not.

**Reproduction:** connect four input channels with constant voltages `[1,2,3,4]`, mark the output connected, call `processBypass()`, then `process()`. The native probe reports `channels=4`, with extra output voltages still `2,3,4`. They remain stale because normal DSP only writes the first channel.

**Customer consequence:** downstream polyphonic modules receive unintended constant voltages or stale audio samples, contrary to the documented mono contract.

**Required work:** enforce the output channel contract in normal processing and Display Only. For the existing mono design, set one channel before publishing the sample so Rack clears higher channels. Decide whether bypass intentionally preserves input polyphony or is mono too; document that separately.

**Acceptance:** exercise connected outputs with 1/2/4/16 channels, enter and leave bypass, disconnect/reconnect cables, and enter/leave Display Only. After normal processing the output must expose exactly one connected channel and every previously active higher voltage must be zero. Repeat on an activated Premium build as well as the DRM-free runtime harness.

### PREM-03 — Quality selection races with the audio thread

**Locations:** `src/Bifurx.hpp:664`; `src/BifurxUI.cpp:1171–1188`; `src/Bifurx.cpp:1226–1228,1329`.

All three quality-menu callbacks store the atomic `modulationQualityMode`, then directly write `controlFastCacheValid = false`. The engine concurrently reads and writes this ordinary `bool`. Making the menu's other field atomic does not synchronize access to this field. This is a C++ data race and undefined behavior, regardless of whether current x64 hardware usually performs byte stores atomically.

**Required work:** keep the cache flag and derived DSP state exclusively audio-owned. Have the engine detect a change in the atomic requested quality, or consume an atomic invalidation generation/flag, and invalidate locally. Do not put a mutex in `process()`. Review direct cache invalidation in persistence loading against Rack's engine-lock contract separately; the menu callback is the independently demonstrated unsynchronized path.

**Acceptance:** continuously switch quality on the UI/control thread while processing modulated audio. Check race freedom with a suitable sanitizer harness where available, and assert finite output and correct first-update behavior after every switch. No menu callback should write an audio-owned cache member.

### PREM-04 — The actual renderer lacks a complete incoming GL-state baseline

**Locations:** `src/BifurxGL.cpp:1231–1280,1330–1364,1437–1459`; `src/visual/AdaptiveGlSurface.cpp:255–323` and `beginShaderPass()`.

The fixed-surface helper saves and restores incoming state, but its standalone `renderIfNeeded()` path does not call the neutral-state setup used by `renderBatch()`. Bifurx's callback disables depth/culling/scissor, yet does not establish all state it depends on: for example stencil and alpha tests, blend equation, and the fixed-function fallback's program/array-buffer baseline. Saving state protects the host on return; it does not make the callback independent of that state while rendering.

**Reproduction:** an isolated hidden native Windows GL window instantiated the actual Bifurx GL widget, with a valid full-width Display Only spectrum. Through the production surface callback, normal state produced total framebuffer alpha `6,161,200`. Enabling `GL_STENCIL_TEST` with `glStencilFunc(GL_NEVER, ...)` before the next render produced total alpha `0`. Both runs reported zero GL errors. The shared lifecycle suite still passed, because it does not render this callback under that condition.

**Required work:** establish a complete known baseline before Bifurx renders, while retaining the existing host-state guard. Include the shader and fixed-function fallback paths. Explicitly normalize stencil/alpha/depth/culling, blend equation/factors, active texture and texture enables as needed, current program, array-buffer/client-array interpretation, pixel unpack state, and relevant color/scissor state. The fallback passes CPU pointers to `glVertexPointer`; ensure a host VBO cannot make those pointers be interpreted as offsets.

Also review compositing: `glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)` multiplies source alpha into the alpha channel as well as RGB. An offscreen surface later composited by NanoVG needs a consistent premultiplied-alpha contract. Use appropriate separate RGB/alpha blend factors, or premultiply shader output, and test overlap/edge pixels.

**Acceptance:** actual Bifurx renderer pixel tests under deliberately hostile but valid incoming GL state must match clean-state rendering, and all incoming state must be restored. Cover shader, plain OpenGL, allocation/shader failure fallback, transparent edges, and repeated renders. Retain the 549-check shared lifecycle suite. Then qualify editor close/reopen and context replacement in Rack Pro/DAW hosts.

### PREM-05 — License refresh and audio verification share unsynchronized mutable state

**Locations:** `src/BifurxLicense.hpp:14–15`; `src/Bifurx.cpp:1158,1169`; `../Leviathan-Pro/DRM/drm.hpp:318–366,438–443,516–554`.

The audio path calls the vendored context's `isModelVerified("Bifurx")` every sample. In the inspected vendor header, the context launches a download thread, `state` is an ordinary enum, and `moduleSlugs` is an ordinary `std::set`. Refresh eventually calls `load()`, which writes state and clears/repopulates the set. `isModelVerified()` reads those objects without synchronization. A refresh of an already verified context can retain verified status while updating the whitelist, making the container access particularly concerning.

This is an integration/thread-safety finding, not a licensing bypass claim. No real account token or license was used. The existing negative-path test explicitly joins the query thread before inspecting the context, so it cannot detect this race.

**Required work:** resolve the supported synchronization contract with VCV and the precise vendored revision. Prefer a vendor-supported implementation that publishes immutable/atomic verification results safely to the realtime path. An application-side cached boolean is insufficient if its producer still calls into the same mutable context unsafely. Do not add a lock, network request, file read, or thread join to audio processing; do not disable DRM or invent an independent activation mechanism.

**Acceptance:** exercise a valid local license, an in-flight refresh, refreshed entitlements, failed refresh with an existing valid license, initial unlock, offline restart, and teardown while a request is active. Verification must remain race-free and the audio path bounded. Measure successful verification cost, not only the early-return unlicensed case. Record the vendor revision and the approved integration mechanism.

### PREM-06 — Long stationary sessions overflow a signed sample counter

**Location:** `src/Bifurx.cpp:1471`; `src/Bifurx.hpp` member `previewTargetStillSamples`.

Every preview tick with little frequency motion adds elapsed samples to a signed `int`. Only the comparison against the 96-sample settle threshold is needed, but the counter continues indefinitely. Approximate overflow times for a stationary patch are 13.53 hours at 44.1 kHz, 12.43 hours at 48 kHz, 6.21 hours at 96 kHz, and 3.11 hours at 192 kHz.

**Reproduction:** warm the real module for 512 samples, set the counter to `INT_MAX - 64`, and process another 256 samples. The native production-optimization probe finishes with `-2147483457`. Signed overflow is undefined behavior; this observed wrapping is not a portable contract. At minimum the stillness classification becomes wrong.

**Required work:** saturate at the required hold threshold. Cap before addition so the saturation calculation itself cannot overflow. Audit neighboring lifetime counters for the same issue; sequence-wrap comparisons in visual state also deserve explicit tests.

**Acceptance:** seed near `INT_MAX`, process stationary input, and prove the counter stays bounded and the settled preview behaves normally. Test motion restarting the counter and a long-session/synthetic-wrap scenario at each supported sample rate. Use UBSan on a supported harness/toolchain where practical.

### PREM-07 — The nominal curve is numerically inaccurate at low frequencies

**Locations:** `src/Bifurx.hpp:255–266`; `src/Bifurx.cpp:463–516,583–605` (`DisplayBiquad`, coefficient conversion, response evaluation).

The nominal model uses mathematically correct TPT-derived transfer functions, but stores and evaluates them in single precision. At low normalized frequencies the denominator evaluates differences of nearly equal terms. High Q magnifies coefficient rounding and cancellation. This is separate from PREM-01: the low-frequency audio is healthy while the nominal display is wrong.

**Reproduction:** Band + Band, zero SPAN/BAL/TITO, RES 1.0, self-oscillation off, LEVEL 0.5, a 0.0001 V peak sine at the configured center. Run eight seconds, measure the second half, and compare production output RMS gain with `previewModelResponseDb(makePreviewModel(lastPreviewState), hz)`.

| Sample rate | Frequency | Nominal gain | Measured audio gain | Display error |
|---|---:|---:|---:|---:|
| 48 kHz | 10 Hz | 33.48 dB | 37.10 dB | -3.62 dB |
| 96 kHz | 10 Hz | 23.62 dB | 37.11 dB | -13.48 dB |
| 192 kHz | 10 Hz | 5.75 dB | 37.16 dB | -31.41 dB |
| 192 kHz | 20 Hz | 23.62 dB | 37.16 dB | -13.53 dB |
| 192 kHz | 100 Hz | 36.57 dB | 37.14 dB | -0.56 dB |

**Required work:** use a numerically stable evaluation of the production TPT response. Options include double-precision coefficient construction/evaluation on the visual worker, or an algebraic form that avoids subtracting nearly equal float terms. Merely converting already-rounded float biquad coefficients to double is insufficient. Preserve the production approximation/coefficients when defining the reference. This is visual preparation, not a reason to put expensive double/complex operations into per-sample DSP.

**Acceptance:** compare display response against an independently stable reference and settled production small-signal measurements across every mode, 4 Hz through the useful Nyquist range, high Q, and all supported rates. Establish a tighter passband/peak tolerance, for example ≤0.25 dB where reference gain is above -40 dB; assess deep notch locations and floors separately. Keep the calculation cached and benchmark worker cost.

## 5. Analyzer, visual, verification, and UX findings

### PREM-08 — Low-frequency FFT lookup performs negative-fraction extrapolation

**Locations:** `src/Bifurx.cpp:1621–1627`; duplicate implementation in `src/BifurxRenderPrep.cpp:132–138`.

The code computes `binA = max(2, floor(binPos))` and then `frac = binPos - binA`. When the requested frequency is below bin 2, `frac` is negative. It extrapolates away from bin 3 instead of interpolating neighboring valid samples or clamping the lookup position. At 48 kHz/4096 samples, 10 Hz gives `binPos=0.853333`, `binA=2`, and `frac=-1.146667`. The whole displayed region below 23.4375 Hz is affected; at 192 kHz that extends to 93.75 Hz.

A synthetic spectrum with energy in bin 2 produced -21.9923 dB at bin position 2 and -23.8350 dB at position 3, but -19.8793 dB at the displayed 10 Hz position. The low end rises beyond both sampled values. This also works against the explicitly authored subsonic fade.

**Required work:** define low-bin behavior and clamp the *sampling coordinate*, or include the appropriate DC/bin-1 handling and interpolate real adjacent bins. Keep indices within the FFT extent and interpolation weights in [0,1]. Consolidate the duplicated preparation code so the correction reaches synchronous and worker paths together.

**Acceptance:** deterministic rising/falling/impulse spectra at positions below, on, and above bins 0/1/2, plus tones at 10/20/40/80 Hz at every sample rate. Interpolation must not overshoot its samples. Verify the intended subsonic attenuation separately from FFT resolution limits.

### PREM-09 — Define and correct analyzer amplitude calibration

**Locations:** `src/Bifurx.cpp:1575–1638`; `src/BifurxRenderPrep.cpp:85–156`; top-left dBFS labels in both renderers.

The analyzer applies a Hann window, amplitude scale `4/N`, a five-bin *weighted average of power*, log conversion, log-axis interpolation, and further smoothing. The `0.04` factor implies a 5 V peak reference, but averaging power across the window's spectral lobe lowers a coherent sine's displayed peak. A 5 V peak coherent sine at FFT bin 46 (539.0625 Hz at 48 kHz), in Display Only with fixed scale, produced a maximum target of **-3.6677 dBFS**. Off-bin placement and log-grid position add further dependence.

This is not a physical output-level defect. It is an analyzer contract defect: “dBFS” suggests a calibrated level reference, while the graph is currently a smoothed visual energy estimate. The manual explains that the label is the window ceiling but does not define a calibration that accounts for this result.

**Required work:** choose whether the display represents peak amplitude, RMS amplitude, spectral power density, or a deliberately smoothed visual estimate. For a tone-amplitude analyzer, normalize window/spectral aggregation appropriately and use power-domain interpolation or peak-preserving aggregation where suitable. For an energy-density display, label and document the units/reference honestly. Do not fix only one frequency with an arbitrary gain offset.

The measured input/output ratio also needs an excitation/confidence rule. With virtually no input energy, generated harmonics or self-oscillation can produce very large “gain” values. That may be useful coloration, but is not an LTI transfer measurement. The manual already cautions that source excitation matters; reflect that distinction in rendering, such as fading unsupported bins or identifying generated energy.

**Acceptance:** coherent and noncoherent tones at known 0/-6/-20/-60 dB levels, noise, silence, and impulse cases. Define tolerances for the chosen units, sample rates, and smoothing. A displayed level should not depend unexpectedly on whether a tone falls exactly on an FFT bin or a plotted log-axis sample.

### PREM-10 — Display Only colors and scale changes depend on renderer

**Locations:** CPU `displayOnlyColorTone()` in `src/Bifurx.cpp:255–267`; GLSL function in `src/BifurxGL.cpp:629–633`; NanoVG change tracking at `src/BifurxUI.cpp:347–356`; GL `step()` at `src/BifurxGL.cpp:1158–1228`; request early-return at `src/Bifurx.cpp:1807–1810`.

The CPU color law shapes energy from linear toward squared or a fast-rising polynomial. The shader instead uses `clamp(0.5 + 0.5*shape - 0.25*(1-energy), 0, 1)`. At energy 0.8 and neutral shape, the CPU returns 0.8 and the default shader returns 0.45. At maximum negative shape, the shader returns zero for every energy, whereas the CPU varies with energy squared. The default renderer does not implement the manual's cooler/slower versus hotter/faster rise description.

Dirty tracking is inconsistent too. NanoVG detects Dynamic FFT Scale changes and resets the fixed ceiling immediately; GL has no corresponding scale-change member/check. Worker requests are suppressed solely by preview/analysis sequences, not changed visual settings. With processing paused or bypassed and the spectrum settled, toggling Dynamic FFT Scale can therefore leave the GL ceiling unchanged until fresh audio data arrives. The Display Only FM color control similarly lacks an explicit cached-value invalidation path in either renderer; ordinary live FFT updates mask the omission.

**Required work:** establish one documented color function with matching CPU/GLSL implementations, and add visual-setting versions/dirty keys independent of engine publication. Update fixed/dynamic ceiling state locally when possible; submit a worker refresh when a setting affects preparation. A bypassed or paused module must still respond to visual controls.

**Acceptance:** deterministic CPU/shader color comparisons over energy/shape grids, screenshots for all palettes and gradient modes, and setting changes with live audio, paused processing, bypass, and silence. No renderer should require unrelated parameter motion to refresh its visual settings.

### PREM-11 — Marker styling can falsify the plotted response and reported frequency

**Locations:** `src/Bifurx.hpp:448–476` (`displayAnchorForMarker()`); `src/Bifurx.cpp:2153–2171,2330–2338`.

Notch-related markers search for a nearby response extremum and use that location's frequency for the displayed label. The manual calls the markers the lower/upper core frequencies with exact values. Those are different quantities. Separately, `calculateRefinedCurvePoints()` forcibly moves a priority-2 nominal-curve point to the marker's bottom lane for pinned notch modes. A cosmetic marker position therefore changes the gold response line rather than drawing a separate marker/guide.

**Consequence:** a customer can see an apparently exact frequency or a deep notch that is partly the result of layout policy. This is especially questionable with broad/shallow notches, overlapping responses, low-Q settings, and limited curve resolution.

**Required work:** keep response geometry derived only from response data. Render marker lanes independently. Decide whether labels mean core cutoff or detected response extremum and make that meaning consistent in the manual and UI. If both matter, distinguish them rather than silently substituting one for the other.

**Acceptance:** for modes Notch + Low, Notch + Notch, and High + Notch, compare plotted curve vertices with the response evaluator at low/high Q, narrow/wide spans, and frequency rails. Moving the marker to a lane must not change curve dB. Verify labels against the chosen frequency definition.

### PREM-12 — Existing tests can pass while materially different code ships

**Locations:** `tests/bifurx_filter_test_model.hpp:63–113,119–156,281–298`; `tests/bifurx_runtime_spec.cpp:763–909,1341–1416`; `Makefile:166–181,364–429,556–557,898–899`.

The standalone model's Band + High combiner is different from production. It uses `0.94*wB*hpB - 0.14*(hpA+bpB)` where production uses `0.92*wB*hpB - 0.16*(bpA+bpB)`. Its SVF coefficient helper uses `std::tan`, and its display helper is an RBJ-style calculation rather than the current production TPT conversion. Its runtime model does not include the always-on boundary resampling. Those tests can validate a reference or intended shape, but not the actual complete Bifurx path.

The production runtime suite is valuable, but the alias test exercises one boundary at about 4.184 kHz. It does not establish two-boundary passband flatness. The marker-gain comparison permits up to 6 dB difference and tests a small set of midband conditions. Most behavior helpers are fixed at 48 kHz. The routine Bifurx Rack-linked test uses `-O1`, whereas the plugin uses `-O3 -funsafe-math-optimizations`; the review probes used the latter.

Build/test selection also leaves a coverage hole: `test-fast` does not run `bifurx_runtime_spec`; that suite remains in `test-rack`, which repository guidance describes as work in progress. Its explicit Makefile prerequisites omit `src/Bifurx.hpp`, despite the runtime translation unit including it, so a header-only DSP change need not rebuild the test. The shared test output directory also contains both extensionless Linux ELF binaries and Windows `.exe` binaries, and the runner checks the extensionless name first. This review does not claim that a wrong-platform binary actually ran; it independently rebuilt and invoked both Bifurx suites at explicit isolated `.exe` paths. All 70 checks passed, including the current runtime suite at shipping optimization flags.

**Required work:** explicitly separate independent reference-model tests from production-contract tests. For shared topology/constants, use production pure helpers or assert equivalence against the intentional independent reference. Remove stale “current production” assumptions. Add regressions for PREM-01 through PREM-10 and PREM-18 using actual module/renderer paths. Run relevant numerical/finite tests with shipping optimization flags as well as sanitizer-friendly flags. Make Bifurx's production runtime suite part of the routine release checks, track included headers through generated dependencies or a complete prerequisite list, and separate test outputs/selection by toolchain so Linux and Windows runs have unambiguous provenance.

**Acceptance:** intentionally change the production Band + High coefficient or resampling passband and demonstrate that the appropriate production test fails. Change only `Bifurx.hpp` and verify recompilation, then alternate WSL/native runs and confirm the correct executable is built and run. Enforce tighter audio/display agreement over a defined multi-rate grid. Do not replace an independent oracle with an exact copy of implementation arithmetic and call that new verification.

### PREM-13 — Invalid CV produces maximum modulation rather than neutral behavior

**Locations:** `src/Bifurx.cpp:1183–1200,1259–1267`; Rack `math::clamp()` uses `fmin/fmax`.

The audio input is sanitized with `sanitizeFinite()`, but CV inputs are directly clamped. In the inspected SDK, a NaN clamp resolves to the high rail. The native probe fed NaN into each CV input separately, with FM/SPAN attenuverters at +1:

| Input | Result |
|---|---|
| V/OCT or FM | Cutoff pair moves to upper range; lower cutoff about 7.269 kHz at default SPAN |
| RES | Resonance becomes 1.0 |
| BAL | Balance becomes +1.0 |
| SPAN | Span becomes 1.0 |

Output remained finite in this test. Therefore this is **not** a reproduced NaN crash or LUT out-of-bounds claim. The defect is a bad upstream value causing an unintended maximum-strength musical change, including possible self-oscillation if enabled.

**Required work:** sanitize every external CV before range mapping. Define the fallback as zero CV or last valid CV, consistently. Preserve ordinary finite rail handling. Review parameter text/automation boundaries too, but do not assume valid Rack parameters are normally NaN.

**Acceptance:** NaN and both infinities on every input, followed by recovery to ordinary signals, must produce the documented fallback and no persistent state contamination or sudden maximum-modulation interpretation. Test with self-oscillation and TITO active.

### PREM-14 — Performance reporting is unreliable for some paths

**Locations:** `src/BifurxUI.cpp:225–251,1078–1086,1094–1101,1104–1140`; `src/BifurxGL.cpp:1219–1224,1485–1519`.

`logPerfDebugSample()` exchanges/reset the audio timing counters to collect its interval. The module-level Debug Terminal submitter independently exchanges or clears the same counters. These are competing consumers: a CSV interval may lose samples or receive numerator/count values that no longer describe the same window. Atomic fields prevent memory races, but do not make this statistics protocol coherent.

The CSV recorders exist in `BifurxSpectrumWidget`, the NanoVG renderer. The default OpenGL path hides that renderer and prevents its step from running. The common menu still offers Log Curve/Performance Debug, so selecting them under the default renderer can produce no capture. Curve/overlay timing counters in the CSV are reset and printed, but the current step path does not increment those counters around `runRenderTick()`.

The module wrappers correctly try to aggregate Step/Draw, which should be preserved. However, fixed-surface rendering happens inside GL `step()` and is reported separately as surface work; `moduleDrawUsRange` only encloses `ModuleWidget::draw()` and the live overlay. Under the repository's definition of Draw as total visible rendering work, it does not include the substantial offscreen render that creates the visible spectrum.

**Required work:** centralize metric collection and publish an interval snapshot to every recorder instead of having multiple consumers reset the same accumulators. Make recording renderer-independent. Preserve the first three macro fields exactly as `Process`, `Step`, and `Draw`, with explicit module-level aggregation for rendering moved to step-time surfaces. Keep component timings after them; define overlap clearly so sums are not misinterpreted. Distinguish CPU submission from GPU completion time.

**Acceptance:** simultaneous CSV/Debug Terminal recording must produce coherent matching intervals, including on the default renderer. Force a heavy surface update and show that the reported module rendering cost reflects it. Record no file/clock/transport work outside the Dragon King debug gate.

### PREM-18 — Worker snapshots, animation state, and settings need separate identities

**Locations:** `src/Bifurx.cpp:1799–1914,2056–2135`; `src/BifurxRenderData.hpp`; `src/BifurxWorker.cpp:97–108`; `src/BifurxRenderPrep.cpp:232–243`.

Several state meanings are conflated:

1. `hasOverlayTarget` means “animation is still converging,” but is also used to decide whether target preparation has previous data and should smooth against it. Once animation settles and clears the flag, the next FFT target is treated like the first frame and may be copied immediately. Likewise `hasCurveTarget` is both convergence and initial-data state. That makes transition behavior depend on whether the prior frame happened to finish settling.
2. Overlay smoothing uses fixed per-frame coefficients (0.20/0.22/0.10), despite `updateAnimation()` receiving elapsed time. A 30 FPS window and a 120 FPS window have different time constants.
3. Worker curve adoption accepts an older usable snapshot, which is a sensible anti-starvation policy, but only copies its arrays; `state.previewState` remains the newest submitted state. Markers/model lookup can therefore use newer cutoff/mode information with an older curve under worker load.
4. Each worker snapshot initializes `displayTopTargetDbfs` to zero; previous spectrum arrays are carried in the payload but the previous display-top target is not. The worker and synchronous paths therefore do not supply the same state to the display-top hysteresis function.
5. A preview-only request can supersede a pending request carrying a new FFT payload. The display has already marked that analysis sequence submitted, and the replacement may have no payload. The next fresh analysis frame usually repairs this, but the coalescing protocol can discard an analysis update without retaining its prepared result or explicitly reporting the skip.

**Required work:** separate data-valid flags from animation-active flags. Use elapsed-time-based visual smoothing with defined limits. Adopt the preview metadata associated with a rendered snapshot, or retain distinct newest-engine and displayed-model state. Carry all state needed for deterministic scale behavior. Coalesce curve and analysis generations independently so a newer curve cannot silently discard the only pending FFT frame while claiming its sequence.

**Acceptance:** compare worker-on/off results for the same captured sequence; test a deliberately delayed worker, alternating preview-only/analysis requests, settings changes without new samples, first frame, fully settled silence, and 30/60/120 FPS. Markers must correspond to the curve actually on screen. Skipped work must be intentional and bounded, and display response times should remain approximately constant in seconds.

### PREM-19 — Menu changes do not provide coherent undo/redo

**Locations:** `src/BifurxUI.cpp:696–703,1151–1190,1239–1282`; mode-arrow processing in `src/Bifurx.cpp:1231–1232`.

Renderer switching explicitly records `history::ModuleChange`. Mode-menu actions directly set the mode parameter; quality, self-oscillation, limiting, and most other settings directly store values with no history action. The arrow buttons are separate momentary parameters that cause the engine to modify MODE; their ordinary parameter history does not necessarily restore the resulting mode. This is inconsistent for sound-design actions that materially change the patch.

**Required work:** route user actions through a coherent undoable edit mechanism. Parameter changes should record the resulting MODE change, not just a button press; serialized settings should record the relevant module change. Keep audio-thread state publication separate from UI history mutation. Group each user action into one meaningful undo step.

**Acceptance:** for every mode-selection method and sound-affecting setting, Ctrl-Z restores the prior audible configuration and redo restores the new one. Save/reload and module duplication must preserve the same settings. Add checks for history restoring settings while audio runs without reintroducing PREM-03.

## 6. Performance review

The realtime architecture has useful strengths: no routine mutex or heap allocation was found in the DRM-free audio path; coefficients and character state are cached; SPAN uses a LUT; FFT capture is incremental; the audio/visual handoff uses generation-tagged claimed slots; FFT computation occurs outside audio; worker requests carry small ownership handles instead of bulk arrays; and the visual service coalesces work per display. Preserve those properties.

The measurements below are local microbenchmarks, not customer CPU claims. The native `-O3` diagnostic called two million samples per case, with debug disabled and no actual UI worker consuming data. In its first run:

| Path | No analysis subscriber | With analysis subscriber |
|---|---:|---:|
| Low + Low, static controls | 114.4 ns/sample | 120.7 ns/sample |
| Display Only, static controls | 68.9 ns/sample | 72.8 ns/sample |

These numbers exclude Rack scheduling, connected-module overhead, successful DRM verification, UI drawing, and real worker consumption. Wall-clock timing on this workstation is affected by system load. They establish that even Display Only/headless use does nontrivial work; they are not a complete performance budget. Do not interpret the difference between paths as an exact isolated cost of one function.

### PREM-15 — Unused diagnostic and preview work remains in audio

**Locations:** `src/Bifurx.cpp:1240–1257,1333–1385,1440–1484`; `src/BifurxUI.cpp:399–407`.

Four LL telemetry envelope accumulators run every sample, and every preview tick computes four square roots, two amplitude-ratio logarithms, and publishes a telemetry slot. This happens with Dragon King debugging disabled, no live visual subscriber, and the default OpenGL renderer. The only Bifurx UI reader found is in the NanoVG spectrum widget; its resulting LL state is used for debug capture. In the two-million-sample no-subscriber probe, telemetry still published **15,625** times.

The preview path also computes `pow(1-alpha, elapsedSamples)` every preview tick even when it immediately selects the already-settled branch. Most steady-state intervals have a predictable sample count. Adaptive preview calculations cannot accelerate publication in their current arrangement: `adpTick` is only set inside a `perTick` condition, so `(perTick || adpTick)` does not add another opportunity to publish.

Display Only decides to skip the SVF only after pitch/slow-control derivation, character preparation, optional TITO/self-osc coefficient updates, and the input oversampler have already run. Some of that work may be justified to keep transitions warm, but the input oversampler output is discarded, and the expensive-path policy is not explicit.

**Required work:** gate LL diagnostics behind an actual diagnostic consumer and the debug policy. Gate purely visual production when no visual consumer exists, while ensuring a new subscriber receives an immediate coherent current snapshot. Cache ordinary preview smoothing factors or skip their evaluation on settled ticks. Remove ineffective adaptive work or make its intended cadence real. Design a Display Only fast path that preserves the established transition behavior and clearly chooses whether unused state is kept warm, reset, or primed on exit.

**Acceptance:** prove identical steady audio for existing processing modes and identical Display Only pass-through. Verify display/debug wake-up, subscriber removal, and transitions back to processing. Benchmark debug off/on, no widget, visible widget, and Display Only with actively changing controls/TITO. Headless non-debug processing should not publish LL diagnostic frames.

### PREM-16 — Visibility checks do not establish viewport visibility

**Locations:** `src/BifurxUI.cpp:765–768,860–865,1032–1037`; `src/BifurxGL.cpp:1158–1224`.

An analysis subscription lives for the entire module-widget lifetime. The helper and GL widget check widget visibility, but Bifurx does not establish whether its bounds intersect the viewport before `runRenderTick()` and step-time fixed-surface rendering. A module scrolled out of view normally remains a visible widget in the hierarchy. Therefore the existing no-subscriber optimization does not by itself stop work for an offscreen live Rack module.

**Required work:** integrate a real viewport/editor-visibility activity policy for analysis demand, worker submissions, and GL updates. Define hidden, minimized, and DAW-editor-closed behavior. Rate-limit or pause visual work without affecting audio. Preserve a latest-state snapshot and explicitly refresh on reentry; do not accumulate a backlog. If Rack guarantees a particular parent traversal is already culled, verify and document that boundary with a counter instead of assuming `isVisible()` proves it.

**Acceptance:** patch 1/8/32 instances, move all but one offscreen, close/minimize the editor, then return. Measure FFT frame production, worker jobs, GL surface generations, CPU/GPU time, and memory. Offscreen work should drop substantially and reentry should produce a current display within a defined bound, suggested ≤150 ms at 44.1/48 kHz. A fixed 4096-sample analysis window already costs about 93/85 ms to fill at those rates.

### PREM-17 — Reuse static curve data and avoid unused renderer work

**Locations:** `src/BifurxRenderPrep.cpp:24–36,177–200`; `src/BifurxWorker.cpp:97–108`; `src/BifurxGL.cpp:905–965,1286–1290,1420–1433,1460–1471`; `src/Bifurx.cpp:603–605`.

**Required work:** evaluate these concrete opportunities in order of likely implementation safety, and implement those justified by the measured workload:

1. **Axis cache:** the worker recomputes 513 logarithmic frequencies using `pow()` whenever the preview sequence changes and curve preparation is required, although the axis depends on sample rate. Cache the axis and reusable evaluation terms by sample rate/axis specification. Keep precision changes from PREM-07 in this design.
2. **Unchanged expected-curve mesh:** an FFT-only worker result reuses curve targets, but adoption unconditionally invalidates marker/template caches, and each GL surface update rebuilds three stroke layers for the expected curve. Separate nominal-curve geometry from live spectrum updates; cache the mesh until curve/size/style actually changes.
3. **Unused fallback geometry:** plain OpenGL fills `fillCrestStrokeVertices` with segment quads but renders `fillCrestLineVertices` as lines instead. The computed quad geometry is not submitted by that fallback. Remove the unused construction or intentionally render it. Hoist fallback palette/atomic loads out of the 512-segment loop.
4. **Per-job allocation:** the worker allocates a new roughly 10 KiB snapshot on every job. This is off audio and not automatically a defect. If many-instance traces show allocator cost, use bounded reusable result storage with clear reader ownership. Do not trade safe immutable snapshots for unsafe buffer reuse.
5. **Graphics boundary overhead:** each dirty module surface saves substantial GL state independently. The shared helper now has a batch API. Consider batching only after PREM-04 establishes the exact state contract and a workload shows driver-call cost. Preserve context ownership and host-state restoration.
6. **Browser construction:** the browser creates a full private Bifurx module, both render frontends, the authored audio probe, and FFT display state even though the thumbnail is static. Shared/lazy static preview data may reduce repeated browser construction cost. Measure thumbnail build time and retained memory before changing it.

**Acceptance:** compare CPU worker time, frame-time percentiles, draw upload bytes, allocation counts, and surface generations for static controls with live FFT, active CV, and 1/8/32 instances. Keep image/curve output equivalent within deliberate visual tolerances. Optimize repeated `sqrt`/normalization and transcendental work only where measured; an appropriate stable approximation is preferable to repeated expensive work, but accuracy-critical visual coefficient construction is a cold/worker task.

### Required performance matrix

Before release, record one repeatable report with hardware, GPU/driver, Rack version, block size, sample rate, build flags, debug state, and exact patch:

| Dimension | Required cases |
|---|---|
| Instance count | 1, 8, 32 |
| Engine rate | 44.1, 48, 96, 192 kHz |
| Signal | Silence, clean sine, broadband material, driven input, self-oscillation |
| Controls | Static, V/OCT/FM audio-rate, slow CV, RES/BAL/SPAN audio-rate in Exact, maximum SM/XM |
| Visuals | Shader default, plain OpenGL, NanoVG; worker on/off; modern/legacy panels |
| Host view | Visible, offscreen, zoom animation, minimized/closed editor, reopened editor |
| Metrics | Process/Step/Draw macro totals, separate worker/surface/GPU measures, p50/p95/p99/max, allocations, resident/GL memory |

Set the budget against an actual supported minimum machine. A proposed starting criterion is no missed audio deadlines in a ten-minute 32-instance test at the supported block size, no unbounded queue/memory growth, and no sustained visual stalls above the chosen UI frame budget. This is an acceptance proposal, not a claim that the current revision passes it. Fix the correctness issues before making aggressive speed tradeoffs.

## 7. Product, persistence, assets, and packaging assessment

### Controls and DSP behavior that are already coherent

- Eleven modes are configured: ten processing topologies and Display Only. Mono channel-1 input processing is explicitly described in the public manual. PREM-02 concerns failure to restore that contract after bypass.
- V/OCT and exponential FM update pitch without an implicit glide. RES/BAL/SPAN use defined /16, /8, or /1 behavior; High uses /8 when those CV inputs are connected, otherwise /16.
- RES CV is intentionally unipolar/additive over 0–8 V; BAL uses ±5 V; SPAN CV and FM have bipolar attenuverters. Do not redesign these mappings simply because bipolar RES CV might be convenient; document current semantics.
- SPAN uses a shaped 0–96-semitone range. At frequency limits the pair shifts inward to preserve separation. Frequency text entry and its inverse use accurate non-audio-rate math.
- The 3 ms fade-out/switch/fade-in transition for topology and limiting is tested at multiple sample rates. It produces a brief intentional dip rather than a simultaneous two-topology crossfade. Existing manual language explains this.
- The physical limiter has an explicit 4 V knee/5 V ceiling. The oversampled result is clamped against reconstruction overshoot. The distinction between its static transfer and the frequency-dependent resampling path must be corrected through PREM-01.
- Self-oscillation is opt-in and LEVEL is an input control, not an output VCA. Prior voicing/alias/DC measurements in `doc/bifurx-sol-h.md` remain relevant evidence; this review does not undo those decisions. The nonlinear core/TITO remains host-rate, so boundary oversampling must not be marketed as antialiasing the entire nonlinear system.
- The rational tangent approximation is not the cause of PREM-01. A coefficient-derived check at 48 kHz showed a 20 kHz target corresponding to about 19.952 kHz (-4.19 cents); at 10 kHz the deviation was about -0.081 cents. This is a separate upper-band tuning tolerance, not a 16.8 dB loss. Set the supported self-oscillator tracking range and verify it musically rather than making an unlimited precision promise.

### Persistence and lifecycle

Custom settings generally serialize explicitly, including visual preferences, quality, limiting, self-oscillation, and worker mode. Legacy renderer and quality encodings are handled. Runtime reset clears core/oversampler/transition/analysis-capture state. Generation-tagged snapshot claims and immutable worker payload leases are substantial improvements over unsafe shared mutable arrays. Worker shutdown is explicitly called from plugin destruction, and unregistering an in-flight display does not leave the worker holding a raw module pointer.

Remaining release tests should cover save/reopen, duplication, factory reset, randomization, undo/redo, missing/unknown JSON fields, wrong JSON types, and switching render/worker modes while data is in flight. Boolean deserialization currently treats any present non-true value as false; define whether malformed types should instead preserve the default. This is a robustness polish item rather than a demonstrated crash. Do not demand persistent oscillator phase unless that is a product requirement; current serialization stores settings, not the live circuit.

Analysis frames contain samples but not their acquisition sample rate/reset generation. A rate change resets in-progress capture but leaves the last published frame available. Qualify the transient display after sample-rate changes/reset so a stale frame is not interpreted indefinitely using a new frequency axis. If necessary, tag or invalidate published analysis by generation/rate. Likewise define whether bypassed/locked displays intentionally freeze or show a clear inactive state.

The GL widget uses shared context leases and deferred retirement, and avoids destructor-time driver deletion. Those are correct foundations. Same-context resource-size/validity checks on the fixed surface are partly debug-gated; the public draw path does not call the available `imageValid()` helper. Exercise invalid-handle/allocation recovery in production configuration, not only with extra validation enabled. This is a qualification requirement; the concrete reproduced graphics failure is PREM-04.

### UI and documentation

The inspected light-panel screenshot and current module construction provide a consistent 14 HP layout, distinct input/output jacks, a large analyzer, mode readout/buttons, and visual indications of modulation polarity. The manual is substantial: port scaling, mono behavior, eleven modes, nonlinear options, divided-rate quality, visual settings, patch recipes, and troubleshooting are already present. There is no basis for claiming that Bifurx has no user manual.

Before release:

- Correct the nominal-curve/marker/frequency claims through PREM-07/PREM-11, define the dBFS reference, and describe the final oversampling latency/passband behavior.
- Resolve the default shader's Display Only color law so the manual's recipe matches actual behavior.
- State bypass polyphony and whether Display Only preserves all channels or only the documented first channel.
- Expose useful units for LEVEL/RES/BAL/FM/TITO where appropriate. Most currently use raw normalized parameter display, while the manual describes percentages and musical roles. This is P3 clarity, not a functional blocker.
- Explain that “Low Latency Offload” raises preview publication cadence; it does not reduce audio latency and does not itself shorten the fixed FFT window. Its audio-side work also increases, not only worker cost.
- Verify light/dark and legacy layouts, label contrast, tiny controls, menu placement, and tooltip/readout legibility at practical Rack zoom and OS scaling. The saved screenshot does not qualify live rendering on every display.
- Consider shipping a small set of musical presets corresponding to the manual's six recipes. The current Premium distributable list contains resources and EULA, not a preset pack. Presets are polish, not an automatic requirement to add scope.

### Master assets and release closure

The runtime anchor atlas regenerated byte-for-byte. That verifies correspondence to the current assets, but it does not prove the runtime split SVGs can be reproduced from the editable master. That separate test hit Inkscape timeouts in a temporary directory with both installed executables. Treat master-to-runtime reproducibility as an open build-tool verification item, not as proof that the checked-in assets are corrupt or stale. Repair/qualify the outline invocation and compare outputs before editing artwork. Follow the repository's master-SVG → splitter → anchor-atlas workflow; do not edit generated files directly.

The Premium staging path successfully paired current main-repo shared sources with Pro's bootstrap/identity and external DRM header. It retained incremental objects, linked a Windows DLL, and produced a stripped package. The Pro manifest is `Leviathan-Pro` version `2.1.0`, with Bifurx's manual URL. Staging tests cover missing dependencies, wrong identity, isolation, timestamp retention, and path traversal prevention.

**Distribution decision required:** the ordinary Leviathan manifest still lists Bifurx with `hidden: false`, `src/plugin.cpp` registers it, and the ordinary build intentionally has no DRM dependency. That is useful for development. If the same ordinary package is publicly shipped, it provides a second unrestricted Bifurx under the Leviathan identity. Decide whether that is intentional. If Bifurx is Premium-exclusive, use an explicit release build/manifest boundary that excludes it from the free public artifact; merely hiding it in the browser is not a complete packaging boundary. Keep the normal development workflow available.

Validate the final package in a clean Rack profile with only its declared resources. Do not rely on another Leviathan installation, developer files, or cached assets masking a missing resource. Verify module browser identity, license overlay, manual link, panel variants, and patch save/reopen. Packaging success alone proves none of those interactive behaviors.

## 8. Implementation order and release acceptance

### Work order

1. **Realtime correctness:** fix PREM-02, PREM-03, and PREM-06 with focused regressions. Resolve the vendor concurrency contract in PREM-05 in parallel with local work; it depends on the approved DRM integration, not a guessed locking patch.
2. **Audio transfer:** redesign/qualify PREM-01. Establish the passband/latency/alias specification before tuning the display to the final chain.
3. **Renderer correctness:** fix PREM-04 and preserve shared lifecycle/state-restoration behavior. Add actual Bifurx rendering tests, not only helper tests.
4. **Display fidelity:** fix PREM-07/08/09/10/11/18 together around a shared preparation model, well-defined units, and consistent snapshot metadata.
5. **Verification/UX:** repair PREM-12 and PREM-19; define invalid-CV behavior and fix PREM-13.
6. **Performance:** repair instrumentation first (PREM-14), then remove unused audio work and qualify offscreen behavior (PREM-15/16). Apply PREM-17 changes according to measured cost.
7. **Release closure:** resolve free/Premium packaging policy, synchronize only the validated source set when intended, qualify master asset regeneration, and run the release matrix below.

### Mandatory final matrix

| Area | Exit condition |
|---|---|
| P1 findings | Each fixed with a failing-before/passing-after reproduction or vendor-supported resolution |
| P2 findings | Fixed, or an explicit documented product decision with quantified impact and corresponding manual/tests |
| DSP fidelity | Full-band/latency/alias evidence for final oversampling; multi-rate finite/bounded behavior; independent low-frequency curve agreement |
| Musical behavior | Level-matched listening on drums, sustained tones, bass, broadband/noisy material, resonance sweeps, SM/XM, and transitions; approved self-oscillator tuning/character |
| Host integration | Mono/bypass/poly transitions, reset, undo/redo, save/reopen, duplicate/delete, randomize, sample-rate changes, cable hot-plug |
| Licensing | Valid unlock and restart, offline valid-license operation, absent/invalid license, refresh, bypass, both panel treatments, teardown; no license material packaged |
| Graphics | Native GPU configurations including integrated graphics; every renderer; forced failure; zoom/DPI; editor close/reopen/context replacement; no blank spectrum or leaked host GL state |
| Performance | Agreed minimum-machine budget, truthful Process/Step/Draw totals, 1/8/32 instances, no offscreen waste/backlog or unbounded memory growth |
| Platforms | Native build, package, and host checks for every platform/architecture promised at release; this review establishes Windows evidence only |
| Assets/manual | Reproducible master split/atlas, clean-profile resource load, working release manual URL, documentation matching final behavior |
| Package identity | Premium artifact contains correct slug/version; intended free artifact boundary tested; final artifact hash and validation record saved |

A passing `test-fast`, successful Windows link, and missing-license silence are necessary evidence, but the reproduced failures demonstrate why they are not sufficient release criteria.

## 9. Reproduction notes

### Authoritative commands used

Run within the documented bridged MINGW64 shell (`/mnt/c/msys64/usr/bin/bash.exe -lc`, `MSYSTEM=MINGW64`, `PATH=/mingw64/bin:/usr/bin`, repository working directory):

```sh
make -j10 plugin.dll test-fast RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"
make -j10 premium-dist test-premium RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"
make test-gl-lifecycle RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"
```

No `make clean` or `make install` was used. Premium packaging cleans only its isolated staging distribution through the normal SDK target.

The review's audio diagnostic sources reuse the runtime test's existing globals/stubs and production source by renaming its main:

```cpp
#define main bifurx_existing_test_main
#include "../../tests/bifurx_runtime_spec.cpp"
#undef main
// Add the diagnostic main here. This includes the real Bifurx.cpp.
```

From `build/bifurx-review/`, the include above resolves correctly. Compile from the repository root with the Rack SDK headers and these sources:

```sh
g++ -std=c++17 -O3 -funsafe-math-optimizations -march=nehalem \
  -Wno-subobject-linkage -I../Rack-SDK/include -I../Rack-SDK/dep/include \
  build/bifurx-review/probe.cpp src/BifurxWorker.cpp src/BifurxRenderPrep.cpp \
  src/MathHelpers.cpp src/PanelSvgUtils.cpp src/PanelAnchorAtlas.cpp \
  -L../Rack-SDK -lRack -o build/bifurx-review/probe.exe
PATH="/c/Program Files/VCV/Rack2Pro:/mingw64/bin:/usr/bin" \
  build/bifurx-review/probe.exe
```

`probe.cpp` contains transfer, bypass, FFT, CV, impulse, and timing probes, with `overflow` and `stress` command-line cases. `curveprobe.cpp` contains the multi-rate small-signal comparison. The GL diagnostic additionally links `BifurxGL.cpp` via inclusion, `AdaptiveGlSurface.cpp`, resource retirement/lifecycle helpers, and `-lopengl32`, with `_USE_MATH_DEFINES`; it borrows the existing lifecycle test's isolated Scene/Window fixture.

For the fresh 39-test runtime run, `current_suite.cpp` uses the same include/rename technique and calls the renamed existing test main. It was compiled with the audio diagnostic command above, substituting `current_suite.cpp` and `current_suite.exe`. The 31-test model suite was compiled directly with `g++ -std=c++17 -O2 tests/bifurx_filter_spec.cpp src/MathHelpers.cpp -o build/bifurx-review/filter_suite.exe`. Both explicit `.exe` paths were executed; their output is retained as `current-suite.log` and `filter-suite.log` in the review artifact directory.

### Minimal bypass and counter setups

```cpp
Module::ProcessArgs args{};
args.sampleRate = 48000.f;
args.sampleTime = 1.f / args.sampleRate;
Bifurx m;
m.inputs[Bifurx::IN_INPUT].channels = 4;
m.outputs[Bifurx::OUT_OUTPUT].channels = 1;
for (int c = 0; c < 4; ++c)
    m.inputs[Bifurx::IN_INPUT].setVoltage(float(c + 1), c);
m.processBypass(args);
m.process(args);
// Current result: 4 output channels; channels 1..3 retain 2,3,4 V.

Bifurx stationary;
for (int n = 0; n < 512; ++n) stationary.process(args);
stationary.previewTargetStillSamples = INT_MAX - 64;
for (int n = 0; n < 256; ++n) stationary.process(args);
// Current native result: negative counter. Corrected behavior must saturate.
```

These fixtures intentionally seed internal state to make an hours-long fault fast to reproduce. They do not imply that a normal user can set the private counter.

### Review completion statement

All Bifurx implementation areas identified in the scope were reviewed, findings were traced to current code, the main numerical/audio/graphics failures were reproduced, available native build/test/package validation was completed, and this document records actionable fixes and release gates. No production fixes are included in this review deliverable. Untested activation, host, platform, listening, and SVG-outline steps remain explicitly unqualified; they are not silently assumed to pass.
