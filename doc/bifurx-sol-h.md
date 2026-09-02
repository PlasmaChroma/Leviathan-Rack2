# Bifurx engineering review

Date: 2026-08-30  
Scope: the complete current Bifurx implementation (`Bifurx.cpp/.hpp`, UI, NanoVG/OpenGL renderers, render-preparation worker, persistence, and both Bifurx test suites).  
Priority order: robustness/correctness, performance, then audio quality.

> Implementation follow-up (2026-08-30): BFX-01 through BFX-03 were addressed after this audit with generation-tagged claimed snapshot slots, incrementally built overlapping analysis frames, and comprehensive runtime reset handling. The detailed findings below preserve the original pre-fix evidence and rationale.
>
> A second follow-up addressed BFX-05 through BFX-07: frequency entry now uses accurate inverse mapping, rail handling preserves SPAN by shifting the cutoff pair, the nominal response uses the production TPT transfer and invalidates on all model dependencies, and V/Oct follows the patched voltage without an internal glide or deadband. The unambiguous portions of BFX-09 were also reclaimed by removing the historical High+High span-dependent boost, caching self-oscillation coefficients, preparing shared character state once per sample, and moving visual/debug atomics to the control cadence. High+High now exposes the raw unity-gain cascaded high-pass result; its remaining wide-SPAN level offset relative to Low+Low is the TPT/bilinear response rather than a hidden compensator. BFX-04 and the remaining voicing/renderer decisions stay open.
>
> A third follow-up completed the low-judgment cleanup pass. BFX-02's visual-subscriber gate now puts FFT capture fully to sleep when no live Bifurx widget consumes it. For BFX-08/BFX-09, a 1024-interval SPAN LUT replaces runtime `pow(x, 1.45)`, Exact mode separates source inspection from derived-state rebuilding, and static V/Oct/FM or unchanged slow CV no longer rebuild filter coefficients merely because a cable is connected. SPAN source metadata can update without recomputing its curve when the effective SPAN is unchanged. BFX-12 now explicitly releases the inactive renderer's worker registration. BFX-13's worker-default setter, debug-gating contract, and fixed-GL-surface naming are corrected. Focused tests cover the LUT error bound, subscriber sleep/wake transition, and worker setter.
>
> A fourth follow-up resolved BFX-04's output-stage contract. With Soft Limiting enabled, Bifurx is now exactly linear through +/-4 V and enters a unity-slope `tanhAudio()` knee that asymptotically approaches the nominal Eurorack +/-5 V audio boundary; disabling it remains an exact finite pass-through. The pure production transfer is shared with the standalone filter model, obsolete clip-wet/makeup and dB-domain limiter approximations were removed, and the measured response now uses the actual final audible output. Because that makes the former response-output capture a duplicate, the third analysis buffer and worker-request payload were removed. The gold curve remains the nominal linear TPT filter response, distinct from this measured complete-module response.
>
> A fifth follow-up resolved BFX-10's counterintuitive maximum-RES amplitude contour without changing the 0.80 onset. The former heat mapping increased effective amplitude-squared damping by roughly 24x across the upper RES range. Nonlinear damping is now proportional to oscillator onset, with only an 8% maximum heat trim, producing a controlled plateau: the Band+Band regression measures 2.146 V RMS at RES 0.90, a shallow 2.316 V crest, and 2.295 V at RES 1.00 (1.079x total plateau spread), while remaining below the +/-5 V safety boundary. The production LEVEL input stage also moved into a shared pure-DSP helper and now explicitly names `tanhLegacy()`; the standalone model uses the exact production input and output nonlinear stages instead of its former `std::tanh` duplicate. Selective oversampling remains an evaluation item rather than an automatic change.
>
> A sixth follow-up corrected a visualization regression introduced while optimizing the measured-response path. The raw-input FFT is required even when the optional response line is hidden because the normal spectrum fill uses input/output response to choose its low-versus-high endpoint tint. Gating that FFT on line visibility forced every bin to 0 dB response and collapsed the default two-color fill to its midpoint hue. Normal Bifurx views now always retain the response spectrum for color, while display-only/browser-preview mode still skips it because that gradient is energy-driven. A focused regression proves line visibility does not alter the color-driving response data.
>
> A seventh follow-up completed BFX-12's large-request transport cleanup without changing render semantics. Raw-input/output FFT samples and the prior overlay smoothing targets now live in a lazy three-slot pool owned by each active display. The UI performs the one required race-safe copy from the module's published analysis frame into an exclusively owned slot, then pending replacement and worker pickup transfer only a shared immutable lease. This preserves one-in-flight/one-pending/latest-replacement behavior while removing FFT copies under the service mutex. `BifurxUiRenderRequest` fell from roughly 36 KiB to 136 bytes; a compile-time size guard prevents arrays from being embedded again. Regression coverage verifies bounded slot reuse, retained payload lifetime, latest-request coalescing, and exact response content.
>
> An eighth follow-up reclaimed the low-risk portion of BFX-12's duplicated renderer storage. The FFT plan, Hann window, time buffers, and frequency buffers were transient and fully overwritten by each synchronous preparation call, so they no longer live in every NanoVG/OpenGL `BifurxSpectrumBase`. Synchronous rendering now uses one lazily allocated 114,704-byte arena per calling thread; the worker retains its independent thread-local arena and requires no cross-thread lock. Each renderer base fell to 17,048 bytes, removing roughly 229 KiB from Bifurx's always-present two-renderer widget. The default worker path need not allocate the synchronous arena at all. Focused first-use timing observations measured 69 microseconds on Linux and 247 microseconds in the native Windows test, always on the UI/test thread rather than the audio thread; these timings are informational rather than test thresholds.
>
> A ninth follow-up resolved the 44.1 kHz selective-oversampling decision in BFX-10. Bifurx now runs only the memoryless LEVEL input nonlinearity and output soft limiter at 2x, using Rack's polyphase reconstruction helpers; the stateful TPT filter, TITO coupling, and self-oscillation core remain at the host rate. A controlled live Octavia comparison used the same approximately 4.188 kHz sine on monitor A and Bifurx output on monitor B. At LEVEL 1.00 with Soft Limiting enabled, 2x processing reduced selected folded-product power by 16.91 dB. Representative products moved from -21.49 to -43.11 dBFS at 6.428 kHz, from -22.48 to -45.73 dBFS at 1.949 kHz, and from -28.94 to -53.58 dBFS at 10.314 kHz, while the fundamental changed by only -0.10 dB and DC remained negligible. A second comparison at LEVEL 0.50, where the input stage is exactly linear but the filter output still engages the limiter, isolated output-stage behavior: important limiter products improved by 11.51 to 26.90 dB with a 0.11 dB fundamental change. Some already-low bins were redistributed upward, but the dominant aliases were materially reduced. The result justifies keeping selective 2x enabled at 44.1/48 kHz. A serialized Dragon-King-debug-only `Selective 2x nonlinear oversampling` bypass now supports matched testing; normal non-debug processing forces 2x on. The native Bifurx runtime suite passes all 38 focused tests and the authoritative Windows `plugin.dll` builds successfully. Whether 2x should be bypassed automatically at 96/192 kHz remains an optional CPU optimization: earlier 96 kHz observations put the low folded component near -60 dBFS, but no matched 96 kHz on/off capture has yet established that the remaining benefit is negligible. The next sound-quality investigation is the TITO SM/XM DC behavior: earlier measurements found approximately -0.29/-0.30 V (SM) versus +0.095/+0.097 V (XM) at RES 0.75, increasing to -2.43/-2.48 V (SM) versus +0.85/+0.876 V (XM) at RES 0.85. This is a separate stateful-coupling issue that selective oversampling does not address.
>
> A tenth follow-up resolved the TITO SM DC behavior without altering XM or neutral TITO. High-level live testing exposed limiter-regenerated SM offsets of -0.825 V at RES 0.75 and -1.451 V at RES 0.85 even after simple pre-limiter mean subtraction, so SM now uses a slow output-referenced servo: it observes the final audible output but applies its correction before the limiter. At approximately 261.6 Hz with a 5 V sine, the same live cases measured +4.9 mV and +3.7 mV DC after settling. A raw Rack-voltage capture at RES 0.85 ranged from -4.9977 V to +4.9979 V, retaining 9.9956 Vpp. Full-scale regressions at approximately 261.6 Hz and 4.186 kHz, RES 0.75 and 0.85, bound settled SM DC below 0.10 V and require more than 9.5 Vpp; the measured fixture worst cases were 0.0738 V DC and 9.9556 Vpp. The focused runtime suite now passes all 39 tests, the complete fast suite passes 109,948 checks, and the native Linux plugin links successfully.
>
> An eleventh follow-up records BFX-11 as resolved. Bifurx already uses a sample-rate-independent 3 ms transition for discrete mode and Soft Limiting changes: it fades the active path to zero, adopts the latest requested path at the silent midpoint, and fades back in. Runtime tests cover continuity, transparent steady-state behavior, completion, and duration at 44.1, 48, 96, and 192 kHz. The High Resonance Self-Osc option is not routed through that transition, but explicit live on/off auditioning at high resonance did not reveal a material click or discontinuity; its staged onset and retained filter state make additional transition machinery unjustified. The next focused sound/performance decision is therefore the controlled 2x-oversampling comparison at 96/192 kHz, not further BFX-11 work.
>
> A twelfth follow-up resolves the high-sample-rate oversampling decision in favor of retaining the existing behavior unless profiling demonstrates a material Bifurx bottleneck. Matched live Octavia captures at a 96 kHz host rate used an approximately 4.186 kHz, 5 V sine through a near-transparent High+High configuration. With full LEVEL drive and Soft Limiting enabled, disabling selective 2x raised the combined first above-Nyquist folded products from -42.02 to -20.71 dBFS, a 21.31 dB regression, while changing the fundamental by only +0.051 dB. With LEVEL at its exactly linear midpoint to isolate the output limiter, disabling 2x raised those folds from -70.79 to -60.59 dBFS, a smaller 10.19 dB regression, with a +0.033 dB fundamental change. The full-drive result shows that the oversampling remains materially effective at 96 kHz. Because only the two memoryless nonlinear boundaries are processed at 2x while the stateful filters, TITO coupling, and self-oscillator remain at the host rate, no speculative 96/192 kHz bypass or signal-dependent switching will be added. A 192 kHz comparison is unnecessary for the present decision; revisit high-rate bypass only if measured performance data identifies this path as a significant cost.
>
> A thirteenth follow-up closes BFX-08's remaining user-facing clarity issue. Following Integral Flux's established rate-menu convention, Bifurx now exposes the actual divisors in its Modulation Quality choices: `Balanced — Control rate (/16)`, `High — Control rate (/8)`, and `Exact — Audio rate (/1)`. High uses /8 while RES, BAL, or SPAN CV is connected and otherwise falls back to /16; Exact is the explicit choice for audio-rate timbral CV. The earlier cache/LUT work already removed the avoidable recomputation inside these modes, so no further DSP redesign is required.
>
> A fourteenth follow-up retains the Dragon-King-debug-only `Context-owned fixed GL surface` toggle as a diagnostic escape hatch. It is enabled by default and remains the supported production renderer; ordinary users never see the choice. Disabling it explicitly selects Rack's legacy zoom-dependent `FramebufferWidget` path, which can help isolate context-owned image problems and may still render where allocating two fixed 3x surfaces fails under unusual GPU memory or maximum-texture constraints. With the option enabled, Rack's framebuffer draw also remains an automatic startup/context-recovery fallback until the context-owned surface has a valid front image. The fixed pair costs about 3.30 MiB per Bifurx with RGBA8 plus stencil-8 storage (up to about 5.28 MiB if the driver requires depth-24/stencil-8); this is acceptable for the production path but worth remembering during multi-instance or constrained-GPU diagnosis.

## Executive assessment

The filter core is fundamentally healthy: its TPT SVFs are bounded defensively, the mode routing is internally consistent, display-only mode is a true pass-through, the 0.80 self-oscillation onset is preserved, and the current native Windows tests pass. The graphics code also follows the repository's current context-loss policy much more carefully than an ordinary Rack display implementation.

I would not yet call the whole module release-robust, however. Two visualization publication mechanisms have C++ data races, the audio thread periodically performs a 48 KiB frame copy in one sample, and Rack's Reset action does not clear Bifurx's DSP state. Those three issues should be fixed before release. There are also meaningful specification gaps around the default soft limiter, the analytical response curve, V/Oct slew, and the duplicated test model.

Recommended disposition:

1. Fix findings BFX-01 through BFX-03 before release.
2. Resolve the intended soft-limiter contract and the preview/test drift in BFX-04 through BFX-06.
3. Reclaim the straightforward hot-path work in BFX-08 and BFX-09 before considering a visual frame-rate cap.
4. Treat the remaining audio-quality items as deliberate voicing decisions, not automatic rewrites.

## Findings

### BFX-01 — High: the audio/UI snapshot handoffs are not race-free

The preview and low-latency telemetry publishers write an ordinary struct into the nominally inactive half of a two-slot array, then release-store its index ([`Bifurx.cpp:801`](src/Bifurx.cpp#L801), [`Bifurx.hpp:631`](src/Bifurx.hpp#L631)). UI readers acquire the sequence and index and then copy the ordinary struct ([`Bifurx.cpp:1246`](src/Bifurx.cpp#L1246), [`BifurxUI.cpp:391`](src/BifurxUI.cpp#L391)). The analysis frames use the same scheme with much larger arrays ([`Bifurx.cpp:803`](src/Bifurx.cpp#L803), [`Bifurx.cpp:1375`](src/Bifurx.cpp#L1375), [`Bifurx.cpp:1581`](src/Bifurx.cpp#L1581)).

Acquire/release makes a newly published slot visible, but it does not reserve that slot while a reader copies it. If the audio thread publishes twice while the UI/worker is preempted, it can wrap around and overwrite the slot being read. That is a formal C++ data race and undefined behavior, not merely a possibility of accepting an old visual frame. The separate sequence and index loads can also pair sequence N with index N+1.

Impact ranges from a visually torn state to rare crashes or optimizer-dependent behavior. Analysis-frame copies have the largest exposure because the reader copies 12,288 floats and may then run FFT work against the selected frame.

Recommendation: solve all three publications with one proven SPSC snapshot primitive. A three-slot latest-value buffer with explicit reader ownership/acknowledgement is a good fit; a bounded SPSC queue with overwrite detection is also reasonable. The writer must never reuse a slot claimed by a reader. Publish the payload identity and sequence as one coherent record. Add a forced-preemption stress test and run it under ThreadSanitizer in a Linux test harness.

### BFX-02 — High: visualization creates a periodic audio-thread latency spike

Every sample is written into three 4096-sample histories. Every 2048 samples, [`pushAnalysisSample()`](src/Bifurx.cpp#L803) copies all three histories into a published frame in a single `process()` call: 3 × 4096 × 4 bytes = 48 KiB. This occurs about 23.4 times/second at 48 kHz and 93.8 times/second at 192 kHz. Average bandwidth is not alarming; concentrating the whole copy into one audio sample is.

The analysis path runs unconditionally, including when no module widget is present or visible ([`Bifurx.cpp:1100`](src/Bifurx.cpp#L1100)). Thus a headless patch still pays three history writes per sample and the periodic copy spike.

Recommendation: build 50%-overlapped FFT frames incrementally rather than rotating a ring into linear order in one sample. Two in-progress linear capture frames can absorb a bounded number of writes per sample and publish a completed immutable slot. Combine that with BFX-01's reader-owned triple buffer. Add an atomic visual-subscriber count so modules without a live display can skip capture and publication entirely; transition into capture cleanly when a widget subscribes.

This is the first performance change I would make. It targets worst-case audio latency rather than merely lowering an average.

### BFX-03 — High: Rack Reset does not reset Bifurx's circuit state

`resetCircuitStates()` exists ([`Bifurx.cpp:670`](src/Bifurx.cpp#L670)) but Bifurx does not override `onReset()`, and production code never calls the helper. The only call outside its definition is an explicit call in one test helper. Consequently Rack's Reset action resets parameters through the base class but leaves both integrator states, analysis history, telemetry, preview smoothing, TITO coefficient caches, and trigger/cache state alive.

This can produce a tail, discontinuity, stale preview, or self-oscillation state after a user expects a clean reset.

Recommendation: implement `onReset()` and centralize a comprehensive runtime reset. Besides the two SVF states, clear/invalidate TITO and normal coefficient caches, V/Oct and preview smoothers, analysis fill/publication state, telemetry, triggers/dividers as appropriate, and visual sequence state. Do not clear user-persistent menu choices unless Rack's reset contract for this module explicitly says to. Add a test that excites both cores, calls `onReset()`, and verifies silent/finite first output plus fresh analysis/preview state.

### BFX-04 — Medium-high: the default soft limiter, response overlay, and tests disagree

Production defaults `softLimitingEnabled` to true ([`Bifurx.hpp:715`](src/Bifurx.hpp#L715)) and applies `10 * tanhLegacy(out / 10)` to every non-display-only sample ([`Bifurx.cpp:84`](src/Bifurx.cpp#L84)). This is an always-active saturator, not a transparent limiter with a knee near ±10 V. Measured static gains are:

| Input | Output | Gain |
|---:|---:|---:|
| 1 V | 0.997 V | -0.026 dB |
| 5 V | 4.658 V | -0.615 dB |
| 8 V | 6.750 V | -1.476 dB |
| 10 V | 7.778 V | -2.183 dB |

Meanwhile the standalone filter test model returns `modeOut` unchanged and has tests named “output stage remains transparent” ([`bifurx_filter_test_model.hpp:315`](tests/bifurx_filter_test_model.hpp#L315), [`bifurx_filter_spec.cpp:503`](tests/bifurx_filter_spec.cpp#L503)). Its input saturation also uses `std::tanh`, while production reaches the legacy rational curve through the compatibility alias. Those tests pass while no longer specifying production behavior.

The “Show Module Response” analysis stores `modeOut`, not the final `out` ([`Bifurx.cpp:1101`](src/Bifurx.cpp#L1101)), so it excludes the default output saturation even though the normal spectrum shows the saturated output.

Recommendation: make an explicit product decision:

- If “Soft Limiting” is meant to protect only excursions near the rails, use a unity segment followed by a smooth knee and test its threshold, continuity, and maximum output.
- If the always-on compression is intentional coloration, call it saturation/soft clipping, keep it in the response overlay, and make the production and standalone tests share the exact implementation.

Whichever contract wins, remove the dead `levelOutputClipWet()`/makeup remnants and use `tanhAudio()` explicitly if mathematical tanh is desired. Bifurx is unreleased, so now is the inexpensive time to settle this behavior.

### BFX-05 — Medium: `fastLog2()` is unsuitable for inverse mapping and range-boundary decisions

The bit-level approximation in [`Bifurx.hpp:105`](src/Bifurx.hpp#L105) is fast but biased: `fastLog2(1)` is approximately 0.057305 octave, or 68.8 cents. Differences of two calls cancel much of that fixed bias, which makes it acceptable for coarse visual motion detection. Direct uses do not.

Two direct uses matter:

- Typing 4 Hz into the FREQ quantity maps to parameter 0.004486 and then displays/produces about 4.162 Hz rather than 4 Hz ([`Bifurx.cpp:110`](src/Bifurx.cpp#L110)).
- Span boundary handling uses it to calculate available upward/downward octave room ([`Bifurx.cpp:977`](src/Bifurx.cpp#L977)). At a frequency rail it reports nonzero room and contributes to asymmetric/clamped endpoint behavior.

Recommendation: use `std::log2` in parameter display/inverse conversion; this is not an audio hot path. For the span boundary, either use an accurate fast log or redesign the clamp to preserve the requested separation by shifting the pair inside the valid band. Add endpoint round-trip tests for 4 Hz and 28 kHz and range-edge SPAN tests.

The `fastTan()` approximation does not present the same concern: the diagnostic sweep found its worst cutoff realization error at the 0.46 × sample-rate ceiling to be about -7.1 cents, which is reasonable for this design.

### BFX-06 — Medium: expected-curve publication can go stale and is not the runtime transfer function

`previewStatesDiffer()` considers mode, sample rate, smoothed balance, frequency, and Q, but not `spanNorm` ([`Bifurx.cpp:475`](src/Bifurx.cpp#L475)). `makePreviewModel()` directly uses `spanNorm` to compute the High+High wide-span compensation ([`Bifurx.cpp:486`](src/Bifurx.cpp#L486)). If SPAN changes while the cutoff pair is held at the same clamped frequencies, the audio compensation changes but no preview state is published, leaving the expected curve stale. `resoNorm` is also omitted even though it is carried into the model, making future dependence easy to break silently.

More broadly, the gold curve uses RBJ biquads ([`Bifurx.cpp:427`](src/Bifurx.cpp#L427)) rather than the actual TPT SVF transfer and necessarily omits TITO's state-dependent modulation. It also omits input drive and output saturation. This can be acceptable as a nominal small-signal curve, but that contract should be explicit. Current tests emphasize curve-family trends and a small set of marker gains rather than broad production-model agreement.

Recommendation: include every field that affects `makePreviewModel()` in the publication predicate, beginning with `spanNorm`. Prefer deriving the analytical response from the same TPT coefficients and mode combiner used by production. Define the gold line as “nominal linear filter response,” and separately define the measured response overlay as the complete module transfer if that is the intended UI meaning.

### BFX-07 — Medium, audio quality: V/Oct is intentionally slewed by about 5.5 ms (10–90%)

The V/Oct input is low-pass filtered with a 2.5 ms time constant before pitch mapping ([`Bifurx.hpp:81`](src/Bifurx.hpp#L81), [`Bifurx.cpp:881`](src/Bifurx.cpp#L881)). A one-pole with that time constant takes roughly 5.5 ms to move from 10% to 90% after a pitch step. This adds a short glide to keyboard/sequencer pitch changes and blunts fast exponential modulation. FM bypasses this smoothing, which makes the two pitch paths behave differently. A 1 mV deadband adds a much smaller approximately 1.2-cent discontinuity around zero.

Recommendation: if the glide is part of the sound, document it or make it optional. If the goal was zipper suppression, smooth/interpolate coefficients or use a much shorter bounded slew rather than filtering the calibrated V/Oct signal. Add production-runtime tests for settled ±1 V octave ratios, pitch-step settling time, and fast V/Oct versus FM modulation.

### BFX-08 — Medium, performance/audio quality: control-rate modes can either alias CV or recompute unchanged nonlinear mappings

Balanced mode samples RES, BAL, and SPAN CV every 16 samples; High uses every 8 samples when those CVs are connected; Exact uses every sample ([`Bifurx.cpp:895`](src/Bifurx.cpp#L895), [`Bifurx.cpp:933`](src/Bifurx.cpp#L933)). At 48 kHz those are 3 kHz and 6 kHz zero-order-held control streams. They are appropriate for knobs and envelopes, but audio-rate RES/BAL/SPAN modulation can zipper or alias unless the user selects Exact.

Exact mode then recalculates `shapedSpan()` and `cascadeWideMorph()` with `std::pow` every sample even when the parameter/CV voltage is unchanged. V/Oct or FM being merely connected also forces pitch coefficient work every sample, even for static voltage. The `fastPathEligible` value is currently telemetry only; it does not select a separate process path ([`Bifurx.cpp:893`](src/Bifurx.cpp#L893)).

Recommendation: retain the user-facing quality choices, but separate “must inspect the source every sample” from “must recompute the derived value.” Cache the last sanitized CV/parameter values and only rebuild derived values when they change beyond an appropriate threshold; interpolate coefficients where necessary. Document that Balanced/High are envelope-rate modes and Exact is required for audio-rate timbral CV.

### BFX-09 — Medium, performance: several avoidable expensive operations remain in the hot DSP path

The clearest cases are:

- High+High calls `highHighSpanCompGain()` from the per-sample mode combiner, which contains `std::pow` ([`Bifurx.hpp:265`](src/Bifurx.hpp#L265), [`Bifurx.cpp:197`](src/Bifurx.cpp#L197)). The gain depends only on slow SPAN state and should be cached.
- In the self-oscillation region, each of the two stages recomputes smoothsteps, a square root, and fresh SVF coefficients every sample ([`Bifurx.cpp:374`](src/Bifurx.cpp#L374)). Much of this is identical between stages or unchanged until RES/LEVEL/cutoff moves.
- Several UI-control atomics are loaded, and `perfPreviewPitchCvConnected` is stored, on every sample even when Dragon King debug is disabled ([`Bifurx.cpp:992`](src/Bifurx.cpp#L992), [`Bifurx.cpp:1057`](src/Bifurx.cpp#L1057)).

Recommendation: cache HH compensation with the SPAN cache; compute self-oscillation onset/heat/drive once per sample or control update and pass a prepared character-state object into both stages; reuse cached self-osc coefficients when cutoff/damping are unchanged. Move visual/debug setting snapshots to the existing control divider where an immediate reaction is unnecessary.

### BFX-10 — Medium, audio-quality review: nonlinear paths are un-oversampled and the self-oscillator gets quieter at maximum RES

Both the upper LEVEL input drive and default output soft clip are memoryless nonlinearities at the host sample rate. They will generate aliases on bright material. Production also uses the legacy rational tanh compatibility alias even though the shared math layer asks new DSP to choose `tanhLegacy()` or `tanhAudio()` explicitly ([`MathHelpers.hpp:89`](src/MathHelpers.hpp#L89)).

The native runtime test measured self-oscillation RMS of 2.101 V at RES=0.90 and 0.561 V at RES=1.00 in its Band+Band case—an 11.5 dB drop as RES rises ([`bifurx_runtime_spec.cpp:658`](tests/bifurx_runtime_spec.cpp#L658)). The oscillator remains finite and bounded, which is good, but this amplitude contour is counterintuitive unless the intended “heat” behavior is strong compression.

Recommendation: audition `tanhAudio()` first because it is essentially a free accuracy improvement using the existing LUT. If aliasing is audible, selectively 2× oversample only when LEVEL drive is active, output limiting is materially engaged, or self-oscillation is enabled; do not oversample the whole clean filter by default. Retune self-oscillation amplitude damping so the RES=0.80 onset remains intact while RMS is monotonic or intentionally plateaus toward RES=1.00. Add harmonic/alias and amplitude-versus-RES sweeps before choosing.

### BFX-11 — Low-medium, audio quality: topology and nonlinear-option changes are instantaneous

Mode changes immediately select another routing topology while reusing the existing two core states ([`Bifurx.cpp:1029`](src/Bifurx.cpp#L1029)). Soft Limiting and High Resonance Self-Osc are also atomically toggled without a transition. These changes can click, especially with resonant material or when toggling the output saturator at high amplitude.

Recommendation: use a short equal-power or linear crossfade for mode changes and short ramps for menu-controlled nonlinear options. Because Bifurx is unreleased, a small fixed transition can become the defined behavior without compatibility cost.

### BFX-12 — Low-medium, UI performance/memory: both renderer stacks stay fully allocated and worker requests are copy-heavy

The module owns about 144 KiB of analysis float storage. Each `BifurxSpectrumBase` owns roughly another 176 KiB of FFT scratch before curve/state storage, and the widget constructs both NanoVG and OpenGL spectrum bases ([`Bifurx.hpp:380`](src/Bifurx.hpp#L380), [`BifurxUI.cpp:918`](src/BifurxUI.cpp#L918)). A normal live widget therefore sits around half a MiB of Bifurx-specific analysis/display storage before GL vectors/textures and worker snapshots.

A worker request embeds three 4096-sample arrays plus prior curve targets—about 52 KiB—and is copied into a mutex-protected pending slot and again into the worker's local request ([`BifurxRenderData.hpp:9`](src/BifurxRenderData.hpp#L9), [`BifurxWorker.cpp:53`](src/BifurxWorker.cpp#L53), [`BifurxWorker.cpp:186`](src/BifurxWorker.cpp#L186)). Switching renderers hides the old widget, but a previously registered hidden renderer does not actively release its worker slot because its step path no longer runs.

The render-preparation worker also calculates raw-input/response FFTs whenever the mode is not Display Only, even if “Show Module Response” is disabled ([`BifurxRenderPrep.cpp:193`](src/BifurxRenderPrep.cpp#L193)); the request does not carry that setting.

Recommendation: share one analysis/animation state between renderer front ends, lazily allocate renderer-specific scratch, and explicitly release the inactive renderer's worker registration on mode switch. Move large request payloads through owned buffers/pool slots rather than repeated struct assignment. Carry `showModuleResponseOverlay` into render preparation and skip the two unused FFTs when it is off.

### BFX-13 — Low: a few configuration/debug contracts are internally inconsistent

- `setBifurxVisualWorkerDefaultMode(int)` ignores its argument and always stores ON ([`BifurxWorker.cpp:229`](src/BifurxWorker.cpp#L229)). It is currently unused, but its API contract is false.
- Debug logging flags are serialized and loaded even outside Dragon King debug mode ([`Bifurx.cpp:671`](src/Bifurx.cpp#L671)). File recording is correctly stopped by the debug gate, but a loaded `perfDebugLogging=true` still enables NanoVG `chrono` instrumentation ([`BifurxUI.cpp:323`](src/BifurxUI.cpp#L323)).
- `fixedSurfaceExperiment` is described as a debug-only, non-serialized prototype but defaults true and is the normal OpenGL path ([`Bifurx.hpp:623`](src/Bifurx.hpp#L623)). Either graduate and rename it or make the production/debug distinction real.

These are not current crash risks, but cleaning them up will make later debugging and lifecycle work less ambiguous.

## What is already done well

- The TPT SVF update is compact and conventional, and cutoff is bounded below the Nyquist singularity ([`Bifurx.cpp:276`](src/Bifurx.cpp#L276), [`Bifurx.cpp:315`](src/Bifurx.cpp#L315)).
- Input non-finites are sanitized, output non-finites are contained, and core state is periodically checked/clamped. The self-oscillation path has an explicit finite fallback ([`Bifurx.cpp:365`](src/Bifurx.cpp#L365), [`Bifurx.cpp:404`](src/Bifurx.cpp#L404)).
- The new resonance mapping uses the whole knob, reaches Q≈33.33, and retains the desired 0.80 self-oscillation onset.
- Display Only bypasses the level, filter, and limiter paths exactly; the production runtime test verifies sample-level pass-through.
- The worker is latest-value/bounded per display rather than an unbounded job queue. Registration, unregistration, completion, and plugin shutdown are mutex/condition-variable coordinated, with explicit plugin-lifecycle shutdown ([`BifurxWorker.cpp:42`](src/BifurxWorker.cpp#L42), [`plugin.cpp:220`](src/plugin.cpp#L220)).
- The OpenGL widget avoids destructor-time GL deletion and resets resources on context recreation/destruction, consistent with the repository lifecycle standard ([`BifurxGL.cpp:197`](src/BifurxGL.cpp#L197)). The fixed surface is context-owned and rebuilt lazily.
- UI dirtying is data/animation driven; hidden NanoVG and OpenGL paths avoid ordinary stepping/drawing work.
- The process path performs no allocation or locking. The principal real-time problem is the bounded-but-bursty copy in BFX-02, not heap/mutex use.

## Test and validation assessment

Validation performed against the current working tree:

- Authoritative native MINGW64 `make -j10 test-fast` with the installed Rack 2 Pro runtime: passed (exit 0).
- Native Rack-linked `bifurx_runtime_spec`: 26/26 passed.
- Native `bifurx_filter_spec`: 31/31 passed.
- Incremental authoritative Windows `plugin.dll` compile and link: passed.
- Cutoff approximation diagnostic: maximum measured realized-cutoff error was approximately -7.1 cents at 0.46 × sample rate.

The existing test coverage is useful but has an important structural weakness: `bifurx_filter_spec` tests a duplicated model in `tests/bifurx_filter_test_model.hpp`. Production has already diverged from it in both saturation curves and the output stage, demonstrating that mirrored implementation tests can stay green while the actual module changes.

Highest-value additions:

1. Race/preemption tests for preview, telemetry, and analysis snapshot publication; ThreadSanitizer coverage for a Rack-independent snapshot primitive.
2. Reset-after-excitation and reset-during-self-oscillation tests.
3. Production output-stage golden tests with Soft Limiting both on and off, including the response-overlay definition.
4. Actual port-level RES/BAL/SPAN CV scaling and V/Oct/FM tests at 44.1, 48, 96, and 192 kHz.
5. SPAN behavior at both frequency rails and a regression test proving HH preview invalidation when only `spanNorm` changes.
6. A process-time spike benchmark that records maximum, not only average, with analysis publication enabled/disabled.
7. A context destroy/recreate Rack smoke test for both GL variants and renderer switching. Compilation alone cannot validate driver/context behavior.
8. Alias/harmonic sweeps for LEVEL drive, output limiting, TITO, and self-oscillation; amplitude-versus-RES sweeps across modes.

Where practical, move pure DSP helpers into one production header/library used by both the module and tests. Keep higher-level golden/reference tests independent so they can still catch shared implementation mistakes.

## Recommended implementation sequence

1. Introduce one reusable reader-owned triple-buffer/SPSC snapshot primitive and migrate preview, telemetry, and analysis publication.
2. Replace the audio-thread frame rotation with incremental overlapped capture and a visual subscriber gate.
3. Implement comprehensive `onReset()` and its tests.
4. Decide whether Soft Limiting is a transparent protector or an always-on saturator; align production, overlay, naming, preview assumptions, and tests.
5. Replace direct `fastLog2()` uses in inverse/range math and fix the preview dependency predicate.
6. Cache HH compensation and prepared self-oscillation state/coefficients; avoid recomputing unchanged Exact-mode controls.
7. Consolidate renderer state/scratch and remove unused FFT work when module response is hidden.
8. Audition V/Oct slew, self-oscillation amplitude, `tanhAudio()`, selective oversampling, and short topology crossfades as a focused audio-quality pass.

The original audit was report-only. The production follow-up changes completed afterward are summarized at the top of this document.
