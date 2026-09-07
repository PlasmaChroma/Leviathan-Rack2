# Leviathan combined source review

Date: 6 September 2026

Source baseline: `b18d37bb85632677dfff63b71c32498561d957cb`.

This is a broad, bounded source review, supported by native Windows tests and two focused native probes. It is not an exhaustive audit or an AST-analyzer report. The first-party `src/` C++ inventory contains 294 `.cpp`/`.hpp` files and approximately 127,322 lines, excluding `third_party` and `doom`. I traced selected execution and ownership paths rather than reading every line. No production code or existing tests were changed for this review.

The most consequential findings are unsafe reuse of UI snapshot buffers, blocking/allocation paths reachable from audio processing, and gaps in graphics resource retirement. Several rendering optimizations are useful and should be preserved, but current Flux instrumentation loses HaloKnob2 work after it moved into `step()`. Establish correct accounting before using those counters to judge another renderer migration.

## Follow-up repair status

**7 September 2026 — F03 fixed in the working tree.** Crownstep's New Game and debug-move parameter edges now publish atomic requests instead of mutating game/history/UI state from `process()`. The existing UI service performs the actions before servicing AI turns. New Game resets audio-owned outputs immediately and suppresses old-sequence clock/refresh work until UI completion; clock edges consumed during that wait are not replayed. Reset handling separates audio-owned playback state from AI cancellation and animation cleanup. Repeated developer-button presses coalesce while UI service is paused, so the pending action storage cannot grow. State restoration discards queued actions from the previous state and retains the saved playhead.

Native Crownstep module/persistence tests passed 10/10, including New Game with the sequence mutex held for 150 ms, immediate reset, deferred debug moves, first-clock behavior after UI completion, and restoration with pending actions. Core game tests passed 25/25, and the final Windows `plugin.dll` compile/link passed. The standalone module target now uses the plugin's x64 CPU baseline and links Winsock on MinGW, correcting existing standalone compilation/link failures encountered during validation. No live Crownstep smoke test, complete fast-suite rerun, or full Crownstep concurrency certification is claimed; the previously listed snapshot/hazard follow-up remains separate from these action branches.

**7 September 2026 — additional Rack smoke-test repair: Chromatide undo.** A stroke previously captured only its initial rectangle, then resized that saved data to the final dirty rectangle without recapturing originals or correcting row layout. Undo could consequently write zero-filled or misplaced pixels over existing artwork. Transactions now capture the full pre-stroke canvas once and crop both before/after images to the final dirty rectangle when committing. Retained history remains bounded by the existing 32 MiB policy. Native Chromatide tests passed all eight cases, including complete-pixel comparisons on patterned artwork after expanding strokes, edge-clipped erasing, repeated undo/redo, clear, and source publication. The Windows plugin build passed. Live rechecking of the reported symptom remains with the user.

**7 September 2026 — F02 fixed in the working tree.** Nautiloid and Chromatide now deliver owned sources through `serviceIrisSource()` on their widget's UI step. Nautiloid's process callback samples CV and sets lights from atomically published readiness; Chromatide's process callback no longer acquires `shared_ptr` ownership or submits requests. Source locking, request replacement/destruction, and copying Iris conversion settings occur on the UI side. Nautiloid checks consumer demand before acquiring its source; its attachment, explicit-sync, deduplication, and restore policies remain in the UI service. New source delivery follows UI cadence; if UI stepping pauses, Iris continues playing its current wavetable until the service resumes. No headless delivery worker was introduced.

[UI adjacency lookup](src/UiExpanderUtils.hpp) resolves module IDs through Rack's synchronized engine lookup and checks reciprocal IDs, avoiding engine-owned raw expander pointer reads in the moved UI paths. Contextless native fixtures retain a separate direct-pointer fallback. Iris's audio-side connection check and engine-locked serialization check remain on their original path. The lookup regression substitutes the `getModule` boundary because Rack's engine constructor is private plugin API.

[Native source integration tests](tests/nautiloid_iris_restore_spec.cpp) passed five checks: ID lookup/removal, Spider and Barnsley restore without request starvation, immutable cache behavior, and Chromatide UI-only submission/reconnection. The restore loops explicitly verify that audio processing does not increment source-delivery counters. Chromatide's existing suite, Iris Phase 4, Iris worker completion, and 17 existing graphics/cache contracts passed. The final native Windows `plugin.dll` compile/link passed. No live drag/reconnect, editor close/reopen, audio-allocation instrumentation, or complete fast-suite rerun was performed for this repair.

**7 September 2026 — F01 fully fixed in the working tree: Sil now joins TD.Scope.** Sil's [spectrum payload](src/SilSpectrumSnapshot.hpp) uses the shared three-slot transport. A producer-only `publishWith()` callback fills its owned slot directly, preserving the existing single chronological ring copy. The revision travels with the samples; the UI marks the revision it actually processed. The 24 Hz publication policy, visible-draw-only FFT work, spectrum smoothing, framebuffer dirty signal, and telemetry fields are retained. The single consumer is the left spectrum widget on Rack's UI thread. Storage grows by approximately one 16 KiB spectrum payload.

The expanded [snapshot suite](tests/spsc_latest_snapshot_spec.cpp) passed 6/6 on Linux and native MINGW64, including all 2,048 ring wrap positions, a held Sil reader, coherent samples/revisions over 10,000 captures, and zero tracked capture-path allocations/frees. ThreadSanitizer passed with no reported races using the documented `setarch x86_64 -R` launch. Native Sil repair tests passed 9/9, and the incremental Windows `plugin.dll` compile/link passed. A live spectrum visual check and a complete fast-suite rerun were not performed. Earlier partial-F01 notes below record the repair sequence.

**6 September 2026 — F11 fixed in the working tree.** [Cantor](src/Cantor.cpp) now includes each ungated voice's last accepted Intent, Coherence, and Field settings in its static cache key. Checks and recomputation retain the existing 32-sample control cadence, with immediate initialization for new voices. Keeping the key per voice also handles settings changed while a previously initialized voice is inactive. Interpret remains excluded because the static algorithm ignores it. [Native module tests](tests/cantor_module_spec.cpp) passed 8/8, including the original 0.011 V Field-change reproduction, control-tick timing, settings sweeps across 16 voices, returning voices, and preservation of gate latch/disconnect behavior. Native culture-engine tests passed 6/6, and the Windows `plugin.dll` compile/link passed. No live Rack check or complete fast-suite rerun was performed.

**6 September 2026 — F10 fixed in the working tree.** [The shared NanoVG helper](src/NvgGraphicsLifecycle.cpp) now treats nonpositive creation handles as failure, normalizes them to `-1`, and leaves cached dimensions invalid. Image reuse, size queries, and deletion likewise require a positive handle. The existing Iris/Nautiloid callers therefore skip drawing a failed image and retry on a later draw. [The focused lifecycle test](tests/nvg_graphics_lifecycle_spec.cpp) links the real helper to fake NanoVG boundary calls: 20 checks passed on Linux and native MINGW64, covering injected zero/negative failures, retry, resize failure, stable updates, and context ownership. The Iris/Nautiloid phase-5 contracts passed 4/4, and the native Windows `plugin.dll` compile/link passed. No live graphics-memory exhaustion test or complete fast-suite rerun was performed.

**6 September 2026 — TD.Scope portion of F01 fixed in the working tree; Sil remains open.** [TD.Scope](src/TDScope.hpp) now uses [a three-slot single-producer/single-consumer snapshot transport](src/SpscLatestSnapshot.hpp). Audio and UI each retain exclusive ownership of a payload slot and atomically exchange the spare. Audio publication uses one payload copy and one lock-free exchange, without allocation, waiting, or a retry loop. The UI finishes its copy before its next acquisition returns the old slot. This relies on TD.Scope's existing single Rack UI-thread reader contract; the helper is not a multiple-consumer transport. Initial empty snapshots, protocol validation, expander messages, parameter/light IDs, and serialization are preserved. Storage grows by one `HostToDisplay` payload (approximately 24 KiB).

[Snapshot tests](tests/spsc_latest_snapshot_spec.cpp), wired into `test-fast`, use the actual stereo `HostToDisplay` payload and passed 4/4 on Linux and native MINGW64. They cover initial/repeated reads, latest-value delivery, a reader retaining its original snapshot across 24,999 subsequent producer publications, concurrent coherent copies over 30,000 publications, and zero tracked heap operations. ThreadSanitizer passed with no reported races using `setarch x86_64 -R build/tests/spsc_latest_snapshot_tsan_spec`; the default launch had failed before execution with an unexpected-memory-mapping error. The native expander preview suite passed 7/7, and the incremental Windows `plugin.dll` compile/link passed. This validates the transport and protocol tests, not all module concurrency or live Rack behavior. No live scope visual check or complete fast-suite rerun was performed for this repair.

**6 September 2026 — F04 fixed in the working tree.** Sil now uses [a fixed-storage limiter peak queue](src/SilLimiterPeakWindow.hpp), with one extra slot for insertion before expiry. The queue replaces the deque in the actual limiter path; reset also resets its sample index. Detector math, gain behavior, and delay timing are unchanged. The original findings below describe the review baseline.

[The regression test](tests/sil_limiter_peak_window_spec.cpp), wired into `test-fast`, passed on Linux and native MINGW64: 12,582,912 exact comparisons against both the previous deque algorithm and an independent sliding maximum, covering every lookahead from 1 to 512, rising/falling/flat/impulse/alternating/random streams, wraparound, and resets. A separate 100,000-update check including construction/reset/destruction recorded zero allocations and zero frees. This allocation check covers the new queue, not the whole Sil callback. The native Sil repair suite passed 9/9, and an incremental native Windows `plugin.dll` compile/link passed. No live listening test was performed. The unrelated Octavia/Undertow baseline failures remain open; the complete fast suite was not rerun for this localized repair.

## Evidence and priority

- **P1:** prioritize for correctness or audio-thread behavior; a confirmed source path does not imply that an audible dropout or crash was observed.
- **P2:** actionable functional, resource, integration, or verification defect with a narrower trigger.
- **Source-confirmed:** caller and implementation establish the issue; no live Rack reproduction is claimed.
- **Probe-confirmed:** reproduced in a focused native Windows executable; the scope of that probe is stated.
- **Follow-up:** plausible concern that needs additional concurrency, GL, or host evidence before becoming a defect claim.

| ID | Priority | Area | Finding | Evidence |
|---|---|---|---|---|
| F01 | P1 | TD.Scope, Sil | Double-buffer publication does not protect a reader from slot reuse | Source-confirmed |
| F02 | P1 | Nautiloid / Chromatide / Iris | Audio processing reaches locks and ownership-changing worker submission | Source-confirmed |
| F03 | P1 | Crownstep | New-game/debug-move processing performs game/UI mutation and dynamic work on audio thread | Source-confirmed |
| F04 | P1 | Sil | Limiter deque still allocates/frees after its purported preallocation | Probe-confirmed container behavior |
| F05 | P1 before release | Chronomaw, Bulkhead | UI directly edits non-atomic state consumed by audio | Source-confirmed |
| F06 | P2 | Temporal Deck | Lifetime logging performs synchronous I/O from `process()` | Source-confirmed; logging enabled |
| F07 | P2 | Shared GL surfaces | Ordinary widget removal abandons resources and FBO wrappers | Source-confirmed lifetime paths |
| F08 | P2 | Flux / HaloKnob2 | Draw logging resets away the shader work performed during step | Source-confirmed |
| F09 | P2 | AdaptiveGlSurface | Allocation precedes state guard; full clear inherits color-write mask | Source-confirmed contract gaps |
| F10 | P2 | Shared NanoVG helper | Image creation failure handle `0` is reported as success | Source-confirmed against bundled backend |
| F11 | P2 | Cantor | Static-pitch cache ignores settings changes | Probe-confirmed with real module/engine |
| F12 | P2 | Chromatide persistence | QOI dimensions are checked after decoder allocation | Source-confirmed |
| F13 | P2 | Test baseline | Native `test-fast` is currently red in two recipe steps | Reproduced |

## Findings

### F01 — TD.Scope and Sil can overwrite a snapshot while the UI copies it

Locations: [TDScope.hpp:364](src/TDScope.hpp#L364), [TDScope.hpp:373](src/TDScope.hpp#L373), [TDScope.hpp:569](src/TDScope.hpp#L569), [Sil.cpp:782](src/Sil.cpp#L782), [Sil.cpp:2179](src/Sil.cpp#L2179).

Both producers alternate between two ordinary payload buffers. The UI loads the published index and reads that buffer without reserving it. A producer can publish the other slot and then return to the UI's slot before the copy finishes. TD.Scope checks the index/generation after the copy and retries; Sil simply copies the spectrum arrays. Neither protects the underlying non-atomic memory from concurrent access.

A concrete interleaving is: UI selects slot A; UI is descheduled; audio publishes B and starts overwriting A; UI resumes and copies A. TD.Scope may detect a changed generation afterward, but that cannot undo the C++ data race that occurred during the copy. Atomic index publication makes a completed initial write visible; it does not reserve the payload for the reader's lifetime. The Rack audio-to-audio expander message flip does not automatically protect the extra audio-to-UI copy.

Impact: undefined concurrent access, torn scope/spectrum data, and an invalid foundation for future rendering/thread changes. TD.Scope is released, so preserve its expander wire format and saved options when repairing the internal UI transport.

Recommended fix: use an explicit slot-ownership protocol or a bounded mailbox that prevents writer reuse while readers hold a slot. A reader retries or retains its last valid UI snapshot; audio never waits. Do not repair this by adding another post-copy generation check or an audio-side mutex.

Verification: pause a reader after slot acquisition, drive multiple producer publications, and prove the held slot is untouched. Add a concurrent sanitizer harness where the toolchain permits it. Existing passing tests are not evidence that this interleaving is safe.

### F02 — Image-expander delivery enters blocking paths from audio processing

Locations: [Nautiloid.cpp:788](src/Nautiloid.cpp#L788), [Nautiloid.cpp:1065](src/Nautiloid.cpp#L1065), [Chromatide.cpp:17](src/Chromatide.cpp#L17), [Iris.cpp:188](src/Iris.cpp#L188), [Iris.cpp:139](src/Iris.cpp#L139), [IrisWidget.cpp:765](src/IrisWidget.cpp#L765).

`Nautiloid::process()` obtains a source through `irisExpanderOwnedSourceSnapshot()`, which takes `snapshotMutex`. It then calls `Iris::requestOwnedExpanderSource()`, which copies conversion settings and submits through `workerMutex`. `Chromatide::process()` also delivers through this request path and uses atomic `shared_ptr` loading. The expensive image conversion is on a worker, but its submission is still reached from the audio callback.

The Nautiloid case is worse when Iris retains a manually selected image: the code obtains the locked source snapshot before checking `irisAcceptsAutoSync`. If a source generation remains unsent because auto-sync is refused, that branch can repeat on every process call. `Iris::sourceKind()` itself is atomic; it is not the blocking getter.

Replacing a pending `WorkerRequest` can also release `shared_ptr`/string storage on the caller. In addition, `requestOwnedExpanderSource()` reads ordinary `conversionSettings` while the Iris widget edits those fields directly. That creates a second cross-thread ownership issue during simultaneous source updates and menu changes.

Recommended fix: publish an immutable source token with explicit ownership into a bounded handoff consumed outside audio. Publish the conversion settings coherently as well. Keep large payload retirement on the UI/worker side. Checking consumer demand before acquiring the source reduces the repeated-lock case but does not by itself make source delivery audio-safe.

Verification: exercise Nautiloid→Iris and Chromatide→Iris while changing sources and settings; hold the UI/worker locks deliberately and confirm audio keeps advancing. Instrument audio-thread allocations/destructions around delivery. The native Iris restore/completion tests pass, but they do not establish a nonblocking audio callback.

### F03 — Crownstep's new-game button bypasses its playback publication separation

Locations: [CrownstepPlayback.cpp:252](src/CrownstepPlayback.cpp#L252), [CrownstepModule.cpp:993](src/CrownstepModule.cpp#L993), [CrownstepModule.cpp:1020](src/CrownstepModule.cpp#L1020), [CrownstepPlayback.cpp:99](src/CrownstepPlayback.cpp#L99).

`process()` directly calls `startNewGame()` on the NEW_GAME parameter edge and `appendDebugRandomMoves(10)` on the debug parameter edge. These operations mutate board/history/UI state, take `sequenceMutex`, clear/copy or append vector-backed structures, publish playback snapshots, and refresh legal moves. The normal clock playback snapshot path does not isolate these alternate process branches.

The debug-move branch is developer functionality, but NEW_GAME is ordinary interaction. It can contend with UI sequence editing and performs work whose cost depends on game/history state. UI-owned highlights and board state are also changed from the process thread.

Recommended fix: route game mutations to their existing non-audio owner and publish the resulting immutable playback state. Use a small audio-owned reset request for transport timing if the button must reset outputs immediately. Keep legal-move generation, vector ownership, and visual state out of `process()`.

Verification: trigger NEW_GAME under clocking while the UI edits/selects moves; trace locks and heap calls inside `process()`. Test the debug branch separately. Preserve the current first-step/EoC semantics.

### F04 — Sil's limiter pre-growth does not reserve deque storage

Locations: [Sil.cpp:354](src/Sil.cpp#L354), [Sil.cpp:950](src/Sil.cpp#L950), [Sil.cpp:2038](src/Sil.cpp#L2038).

The initialization comment says that resizing a `std::deque` to the maximum lookahead and clearing it avoids future growth allocations. A deque has no capacity-reservation contract equivalent to vector `reserve()`. Its block allocations can be released by clear/expiry and allocated again during the per-sample sliding window.

A native MINGW64 C++11/O3 probe used the same entry shape and push/back-pop/front-expiry algorithm, initialized with `resize(512); clear();`. After 5,000 warm-up updates, a decreasing-peak stream produced:

```text
100000 warm updates: allocations=3125 frees=3125 retained_entries=512
```

This measures the actual Windows standard-library container behavior under the limiter's operations. It is not a full Sil audio benchmark, and no audible glitch is claimed. It disproves the source comment's allocation-free premise even with bounded live entry count.

Recommended fix: a fixed-capacity monotone queue with head/tail indices. Accommodate the transient insertion-before-expiry entry, or expire first, so the maximum lookahead cannot overrun storage. Preserve detector/lookahead behavior bit-for-bit where practical.

Verification: count allocations around the entire Sil callback after warm-up, including repair-mode toggles; compare limiter peaks/output against the current implementation over rising, falling, flat, and bursty peaks. The existing `sil_repair_spec` covers repair helpers, not this full limiter container path.

### F05 — Chronomaw and Bulkhead still share mutable UI/audio state directly

Locations: [Chronomaw.hpp:64](src/Chronomaw.hpp#L64), [ChronomawWidget.cpp:255](src/ChronomawWidget.cpp#L255), [ChronomawWidget.cpp:530](src/ChronomawWidget.cpp#L530), [ChronomawWidget.cpp:600](src/ChronomawWidget.cpp#L600), [Chronomaw.cpp:179](src/Chronomaw.cpp#L179), [Bulkhead.hpp:44](src/Bulkhead.hpp#L44), [BulkheadWidget.cpp:189](src/BulkheadWidget.cpp#L189), [Bulkhead.cpp:225](src/Bulkhead.cpp#L225).

Chronomaw exposes `state.live.outputs` directly to widget editing: waveform/mute and inspector controls mutate ordinary fields that its engine consumes during processing. Its atomic timeline histories protect those histories, not the authored/live control structures.

Bulkhead's room bounds, listener/speaker positions, and yaw fields are ordinary objects. Canvas dragging writes them while `process()` reads them; some listener fields are also written by CV processing and read back by the canvas. Individually plausible float loads/stores do not establish C++ synchronization or a coherent room geometry snapshot.

These modules are prototypes/unreleased under the repository's stated compatibility policy. This is an opportunity to establish ownership before release, rather than perpetuating a second set of unsafe publication patterns.

Recommended fix: explicit UI commands or a versioned immutable configuration handoff, with audio-owned effective state and a separate UI telemetry snapshot. For Bulkhead, retain an authored base geometry and derive CV-modulated geometry in audio. Do not put a mutex around every sample.

Verification: concurrent drag/edit and process harnesses, including changing multiple room boundaries and Chronomaw bank load/save. This review did not execute ThreadSanitizer or observe a live crash.

### F06 — Temporal Deck's diagnostic mode can itself block audio

Locations: [TemporalDeck.cpp:1552](src/TemporalDeck.cpp#L1552), [TemporalDeck.cpp:1590](src/TemporalDeck.cpp#L1590), [TemporalDeck.cpp:1648](src/TemporalDeck.cpp#L1648), [TemporalDeck.cpp:271](src/TemporalDeck.cpp#L271).

With lifetime logging enabled, prepared-sample installation and deferred-state handling in `process()` obtain locked sample metadata and call `appendTemporalDeckLifetimeLoadingLog()`. That function creates directories, takes a global log mutex, opens files, formats CSV, and appends synchronously. The gate makes this conditional; it does not make those operations safe in an audio callback.

Impact: diagnostic runs can introduce the stalls they are intended to investigate, particularly during sample replacement or under slow filesystem conditions. The ordinary prepared-buffer retirement path is better: old ownership is passed back to the worker rather than deleted in audio.

Recommended fix: capture fixed-size event fields into a bounded audio-to-worker/UI log queue. Resolve paths, format strings, and write files outside `process()`. Account for dropped diagnostic events instead of blocking. Retain timing scopes around the actual install, excluding export.

Verification: enable lifetime logging and exercise repeated sample installs with an artificially slow log consumer. Audio must continue without filesystem calls, mutex acquisition, or string allocation.

### F07 — Context-loss safety currently trades away ordinary widget reclamation

Locations: [AdaptiveGlSurface.cpp:10](src/visual/AdaptiveGlSurface.cpp#L10), [AdaptiveGlSurface.cpp:15](src/visual/AdaptiveGlSurface.cpp#L15), [HaloKnob2.cpp:376](src/visual/HaloKnob2.cpp#L376), [WyrmRendererGL.cpp:1057](src/WyrmRendererGL.cpp#L1057), [BifurxGL.cpp:206](src/BifurxGL.cpp#L206), [NautiloidWidget.cpp:176](src/NautiloidWidget.cpp#L176).

`AdaptiveGlSurface::~AdaptiveGlSurface()` uses `reset(false)`, which drops `front` and `back` pointers without deleting GL resources or freeing the malloc-owned `NVGLUframebuffer` wrappers. Several owners also abandon their shader/buffer/texture names during ordinary widget destruction.

The inspected Rack removal path removes/deletes children without a graphics-context-destroy broadcast for each removed module. Consequently, adding/removing modules while the same editor remains open can strand FBO/renderbuffer/texture/program allocations until context destruction. Later context destruction cannot free an already-forgotten CPU wrapper pointer.

This is different from intentionally avoiding GL calls after the context is lost. That protection is valid; the missing piece is safe retirement while the editor remains alive. TD.Scope has a distinct current-context-aware destructor path, so do not describe every owner as identical.

Recommended fix: retire graphics objects through a verified context-owned maintenance path, protect queued NanoVG presentations until flush, and free CPU wrappers separately when GL deletion is no longer legal. Do not simply replace all `reset(false)` calls with immediate destructor-time GL deletion.

Verification: instrument object and wrapper counts during repeated add/remove cycles in one editor, then during editor close/reopen. GPU memory growth was not measured in this review. The bundled NVGLU implementation was inspected to confirm that wrappers are allocated with `malloc` and normally freed by `nvgluDeleteFramebuffer`.

### F08 — Integral Flux drops HaloKnob2 step-time rendering from its draw breakdown

Locations: [HaloKnob2.cpp:417](src/visual/HaloKnob2.cpp#L417), [HaloKnob2.cpp:911](src/visual/HaloKnob2.cpp#L911), [IntegralFluxWidget.cpp:2284](src/IntegralFluxWidget.cpp#L2284), [IntegralFluxWidget.cpp:2290](src/IntegralFluxWidget.cpp#L2290), [IntegralFluxWidget.cpp:2415](src/IntegralFluxWidget.cpp#L2415).

HaloKnob2 now renders its adaptive GL surface during `step()` and accumulates `gHaloKnob2DrawMetrics` there. Flux resets those metrics at the beginning of `draw()`, then reads them afterward. Normal adaptive shader work from that step has already been erased. The NanoVG fallback's draw-time work remains observable, so comparing the two breakdowns can favor the shader path for an accounting reason.

The module's draw timer also brackets `ModuleWidget::draw()` only; rendering moved into step appears in the step phase instead of preserving the intended aggregate visible-rendering accounting. It has not vanished from host CPU time.

Recommended fix: per-owner, per-frame accumulators spanning all render phases, consumed after collection. Keep `Process`, `Step`, and `Draw` meanings stable and expose phase/component timings separately. Avoid resetting one thread-local total at unrelated module traversal boundaries. Clarify any overlapping scopes before summing them.

Verification: change or zoom six HaloKnob2 controls and assert the reported adaptive renders are nonzero and correctly owned. Compare two Flux instances and forced NanoVG/shader modes with identical timing scopes. This matters before using new logs to validate the Lumen RFC or zoom gains.

### F09 — AdaptiveGlSurface's GL guard does not cover the complete operation

Locations: [AdaptiveGlSurface.cpp:94](src/visual/AdaptiveGlSurface.cpp#L94), [AdaptiveGlSurface.cpp:96](src/visual/AdaptiveGlSurface.cpp#L96), [AdaptiveGlSurface.cpp:116](src/visual/AdaptiveGlSurface.cpp#L116).

Two specific contract gaps remain:

1. `ensureBackSurface()` executes before the state snapshot. On allocation, NVGLU/NanoVG texture creation changes texture binding and unpack state. The bundled backend resets unpack values to its defaults and unbinds the texture, rather than preserving the caller's incoming values. The later guard therefore records state that allocation has already altered. Failure returns take that same unguarded path.
2. The full retained-target clear disables scissoring but does not enable all color-write channels. With an inherited restrictive `glColorMask`, the new clear is incomplete. Saving/restoring the mask protects the caller afterward; it does not initialize all the texels during the clear.

Impact depends on the state left at entry. This review does not claim these gaps caused the supplied 119% screenshot. The existing full-capacity clear addresses the observed stale-edge mechanism under normal write-mask state and should be retained.

Recommended fix: include creation/failure paths inside the owned state scope, establish the write masks required for each clear, and restore the actual incoming state. Audit generic vertex-attribute state and texture units with a harness rather than assuming compatibility attribute stacks cover every callback mutation.

Verification: start with a nondefault texture binding/unpack configuration and restrictive color mask; force first allocation, reuse, promotion, and failure. Check both restored state and initialized pixels. Include 119%, active-size boundary crossings, and shrinking/regrowing both retained targets. No new GL harness was run in this review.

### F10 — Shared NanoVG image upload reports handle zero as success

Location: [NvgGraphicsLifecycle.cpp:32](src/NvgGraphicsLifecycle.cpp#L32), especially [line 62](src/NvgGraphicsLifecycle.cpp#L62).

`updateOwnedNvgImageRgba()` tests the result of `nvgCreateImageRGBA()` with `handle < 0`. The bundled NanoVG GL backend returns `0` when it cannot allocate a texture record. The helper therefore fills cached dimensions and returns `true` for that failure handle.

Impact: callers can accept an invalid image as a successful upload, losing the opportunity for immediate fallback/backoff. Later size validation may reject it and retry, but the success contract is already wrong. This finding is specific to the demonstrated zero-return path; not every GL allocation failure necessarily returns zero through NanoVG.

Recommended fix: recognize nonpositive creation results, normalize the stored invalid sentinel, and keep cached dimensions invalid until creation succeeds. Audit creation checks in other shared asset helpers for the same convention.

Verification: inject a zero creation result and assert failure is returned, no valid cache is published, and the next retry is controlled. The backend's return convention was verified in the local Rack dependency source; no out-of-memory stress test was performed.

### F11 — Cantor's ungated static cache does not react to Field/other settings

Locations: [Cantor.cpp:124](src/Cantor.cpp#L124), [CantorCultureEngine.cpp:302](src/CantorCultureEngine.cpp#L302).

When no gate is connected, the recomputation condition watches initial state and pitch change. It does not watch settings even though `quantizeStatic()` uses Intent, Coherence, and Field. A held input can therefore keep an obsolete quantized result while those knobs change. Interpret is explicitly forced to zero by the static algorithm, so this finding does not require Interpret to affect that mode.

A native executable linked the real `Cantor.cpp` and `CantorCultureEngine.cpp` against the installed Rack runtime. With Intent=1, Coherence=0, no gate, and input 0.011 V:

```text
input=0.011 field 0->1: before=0 after_64_samples=0 fresh_quantization=0.0179219
```

The unchanged module output differs from a fresh engine evaluation of the same input and new settings. The 64 processed samples exceed the static control divider interval. The existing four module tests still pass because they set settings before initial quantization.

Recommended fix: include effective static settings in the cached key and invalidate all active ungated voices when they change. Retain control-rate throttling and gated note-latch semantics.

Verification: fixed pitch plus Field/Intent/Coherence sweeps, multiple polyphonic voices, gate connect/disconnect, and return to a previously inactive voice.

### F12 — Chromatide validates fixed canvas dimensions after allocating the image

Locations: [ChromatideCanvas.cpp:226](src/ChromatideCanvas.cpp#L226), [ChromatideCanvas.cpp:264](src/ChromatideCanvas.cpp#L264), [qoi.h:488](src/third_party/qoi.h#L488). Compare [DeepcacheArchive.cpp:214](src/DeepcacheArchive.cpp#L214).

The fixed-size canvas loader decodes arbitrary Base64 into a vector and calls `qoi_decode()` before checking that the header dimensions/channels match its canvas. QOI allocates the decoded pixel buffer from the header dimensions before the caller's rejection. A malformed or inappropriate embedded patch image can therefore request far more work/memory than this fixed canvas needs. Encoded input size and conversion to the decoder's `int` length are also not bounded here.

This is a local patch-loading robustness issue; it is not evidence of remote code execution. Deepcache already demonstrates the appropriate preflight: inspect dimensions and decoded byte limits before entering the decoder.

Recommended fix: bound encoded size, validate QOI header and expected canvas dimensions/channels before allocation, check length conversions, and decode into a temporary validated result before replacing the current canvas. Consider whether a rejected load should clear an existing canvas or preserve it.

Verification: tiny files claiming very large dimensions, wrong dimensions/channels, oversized Base64, truncated payloads, and valid round trips. Existing Chromatide tests pass but do not prove an allocation bound for hostile dimensions. No large allocation was deliberately attempted during review.

### F13 — The native fast-test baseline is not clean

Locations: [Makefile:428](Makefile#L428), [octavia_monitoring_panel_contract_spec.py:26](tests/octavia_monitoring_panel_contract_spec.py#L26), [octavia_monitoring_panel_contract_spec.py:81](tests/octavia_monitoring_panel_contract_spec.py#L81), [undertow_module_spec.cpp](tests/undertow_module_spec.cpp).

An ordinary native `make -j10 test-fast` stopped at the Octavia monitoring contract. A second diagnostic invocation with `make -i` completed the recipe while recording failures. Its zero exit status is **not** a passing-suite result: errors were explicitly ignored to collect coverage.

Observed failures:

- Octavia anchor contract: `TITLE_LABEL` and `LOUDNESS_METERS` are absent from `res/Octavia.svg`. The assertion ends in `StopIteration`. Runtime fallback rectangles exist, so this does not by itself prove a missing rendered meter.
- Octavia startup contract: the test requires literal `serverRunning.compare_exchange_strong` in `Octavia.cpp`, while startup is now delegated to `OctaviaServerLifecycle.hpp`. The separate native lifecycle behavior test passes. This is stale implementation-shape coupling rather than evidence that the old startup bug has returned.
- Undertow module baseline: monophonic reference summaries differ in the `free` case at field 4; the quantized waveform fingerprint changes from `17030102344340558454` to `8612068578004439136`. These two failures were previously recorded in the preview experiment tracker and are still present.

Recommended fix: reconcile the Octavia anchor contract with the intended master SVG, replace the obsolete startup string check with the established lifecycle behavior contract, and investigate/document the exact Undertow waveform change before updating golden expectations. Do not blanket-disable these checks or treat all baseline drift as harmless.

## Validation performed

Environment: native Windows MSYS2 MINGW64 invoked from WSL, using the installed Rack2Pro runtime ahead of compiler runtime directories, as documented in [windows_build_from_wsl.md](doc/windows_build_from_wsl.md).

Commands inside that environment:

```sh
make -j10 test-fast RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"
# Diagnostic collection only; intentionally continues after failed recipe lines:
make -i -j10 test-fast RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"
```

The continuation run reached the final Phonex ROM consistency check. Only recipe lines 457 and 479 reported ignored errors. Passing areas included the Octavia lifecycle/observation/control tests, Sibyl and Moirai suites, Temporal Deck engine/input/sample/LongPlay tests, Mandelwake, Puffy, Cantor's existing tests, Wyrm envelope tests, Doorstop engines, Bifurx filters, Sil repair helpers, Bulkhead geometry, Umi, aperture transfer, Iris and Nautiloid restoration/completion tests, Proc/Flux runtime tests, Deepcache, Chromatide, and the ROM/table generation checks.

Selected exact summaries: Bifurx filter 31 tests; Wyrm envelope 10/10; Cantor module 4/4; Cantor culture 6/6; Puffy engine 24/24; LongPlay 13/13; Phonex engine 109,749 checks with zero failures. Counts are local suite assertions/cases, not independently tested end-to-end patch scenarios.

Focused native probes:

- `build/reviews/ast_high_deque_probe.cpp`: same Sil deque algorithm and entry size, maximum window 512, 5,000 warm-up plus 100,000 measured updates. Global allocation/deallocation counters enabled only during the measured loop. Built with `g++ -std=c++11 -O3`; result embedded in F04.
- `build/reviews/ast_high_cantor_probe.cpp`: actual module and culture engine, 48 kHz process arguments, gate disconnected, fixed input across a settings change; compares with a fresh engine evaluation. Built with C++17/O2 and linked to `libRack`; result embedded in F11.

Probe sources/executables live in ignored build scratch, not the committed test suite. Full temporary logs are `/tmp/leviathan-ast-high-test-fast.log` and `/tmp/leviathan-ast-high-test-fast-all.log`; the substantive results are recorded here so the review does not depend on retaining those files.

No live patch mutation, listening test, GPU timing, GPU memory measurement, screenshot comparison, ThreadSanitizer run, complete new plugin link, Linux/macOS runtime validation, or `test-rack` run was performed for this review. Earlier Windows plugin builds are context, not new validation claimed here.

## Coverage and limits

| Area | Inspected during this pass | Limits |
|---|---|---|
| Shared graphics | AdaptiveGlSurface, graphics lifecycle helpers, HaloKnob2, snapshot/settled/phosphor helpers; selected Wyrm/Bifurx/Nautiloid/TD.Scope lifecycle callers | No exhaustive shader-math or host-context execution audit |
| Proc / Integral Flux / Undertow | Preview defaults/JSON migration, process-path scan, shared cache behavior, Flux instrumentation, native runtime/shape results | No full DSP transfer/aliasing characterization |
| Temporal Deck / TD.Scope | Sample install/retirement, runtime worker wakeups, LongPlay ownership excerpts, UI snapshot publication, draw integration | Transport/granular engine internals not exhaustively reread |
| Sil | Limiter fast-path storage, per-sample window, spectrum publication/reader, repair test scope | Full mastering response and latency calibration not remeasured |
| Iris / Nautiloid / Chromatide | Source delivery, worker submission, table adoption excerpts, UI settings, canvas persistence | GPU fractal precision and every image codec not audited |
| Crownstep | Clock playback snapshot path, new-game/debug mutation, locking/ownership | Game-rule correctness and search quality not evaluated |
| Wyrm | Atomic point publication, staged wavetable build, serialization excerpts, renderer lifetime | No blanket endorsement of all wave-editor concurrency |
| Chronomaw / Bulkhead | Process entry points, UI editing of shared state, geometry/control ownership | Prototypes; complete behavior/spec conformance not evaluated |
| Cantor / Puffy / Doorstop / Umi / Mandelwake | Process adapters and selected bounded-work/control/publication paths, native tests; focused Cantor repro | Engines mostly covered by targeted existing tests, not exhaustive mathematical review |
| Sibyl / Moirai | Selected process/adoption/publication paths and native suites | Hazard reclamation and all semantic-edit interleavings remain a separate deep review |
| Octavia | Lifecycle extraction, contract failures, selected routing/publication structure and native tests | No live bridge session or exhaustive HTTP/security audit |
| Deepcache / theme / debug transport | Archive preflight, UI manager/process separation, selected persistence and transport lifecycle paths | No broad filesystem race or multi-editor stress test |
| Assets / packaging | Build/test wiring and targeted Octavia anchors | No full SVG/anchor atlas, artwork, preset, or distribution inventory |

Vendored Doom, third-party codecs beyond the QOI allocation boundary, MCP server behavior, all external tools, and unrelated local files were outside substantive coverage. Existing review documents were treated as context, not as substitutes for current-source evidence.

## Patterns worth preserving

- Snapshot history separates immutable trail rendering from age-based opacity, and settled contours keep live changes responsive. Preserve this baseline when comparing a shared runtime; vector-history results alone are no longer the default comparison.
- The preview loaders explicitly version legacy defaults and keep developer choices separate from the production policy. Preserve released IDs and JSON migrations during ownership fixes.
- Temporal Deck's prepared-buffer handoff and worker retirement avoid deleting old large sample buffers directly in the ordinary install path. Extend that discipline to diagnostic event export.
- Wyrm spreads wavetable construction across bounded work units. Preserve that scheduling advantage while strengthening any publication coherence requirements.
- Bifurx coalesces worker requests by display and moves payload leases under the service mutex rather than copying FFT payloads there. Its writer/reader ownership approach deserves reuse after dedicated concurrency verification.
- Phonex uses explicit sequence-slot reservations; Umi bounds command and physics catch-up work. These are stronger foundations than assuming an atomic front index alone protects a payload.
- Deepcache preflights decoded image size before QOI allocation. Theme persistence distinguishes future schemas and avoids overwriting an unknown future document. These are useful local patterns for the other loaders.

## Follow-up investigations, not additional confirmed defects

1. **Worker wakeups:** Temporal Deck's runtime request/retirement paths publish atomic predicates and notify a condition variable without taking its wait mutex. Audit the check-to-sleep race in `TemporalDeckSampleLifecycle::workerLoop()` with a forced interleaving. A lost notification could strand work until another event. Do not fix this by introducing an audio-side blocking lock; consider a bounded polling/wakeup design.
2. **Wyrm multi-point coherence:** points are atomic, so their individual reads are not the Sil/TD.Scope data race. However, a trailing version increment alone does not mark a multi-point write as in progress. Test a writer paused before its final increment while a staged build finishes. Decide whether the intended contract allows intermediate sculpt states or requires a complete edit snapshot.
3. **Hazard protocols:** several independent implementations use reader counters, generations, or hazard pointers. Audit acquisition/reclamation ordering on weakly ordered platforms and bound audio-side retry loops. More atomics or extra buffers are not proof of a correct protocol.
4. **Invisible adaptive surfaces:** HaloKnob2 renders in `step()` without a local viewport/window-visibility admission check. Confirm how often Rack steps offscreen controls and minimized windows before claiming optional GPU work sleeps. The shared surface itself has no visibility policy.
5. **Phosphor validation cost:** `ensureResources()` still calls texture/FBO existence checks during rendering. Measure this separately from the deposition/presentation strategy; do not remove lifecycle protection wholesale.
6. **Adaptive failure publication:** the render callback returns `void`, and the helper swaps/clears dirty state after it returns. Review each shader/allocation fallback before adopting this helper as a general transactional surface. Also validate finite/overflowing dimensions at the shared boundary.
7. **Test orchestration:** `theme_service_spec` has a build rule but is absent from the routine fast-test list. Maintain an explicit inventory of helper tests, real module tests, source-string contracts, and graphics integration tests. A helper-only green result must not stand in for a complete module callback audit.

## Suggested repair order

1. Repair TD.Scope/Sil snapshot ownership, then the Iris source-delivery and Crownstep audio mutation paths. Add small forced-interleaving or allocation tests that exercise the actual failing boundary.
2. Replace Sil's deque with fixed storage and remove Temporal Deck's audio-side log I/O. Reconcile the existing red test baseline without blindly rewriting golden audio results.
3. Fix Cantor's settings invalidation and harden the shared NanoVG failure result and Chromatide decoder preflight.
4. Restore trustworthy Flux/Halo accounting; implement safe same-editor graphics retirement and complete GL state boundaries. Recheck the 119% artifact and add/remove churn live.
5. Continue the rendering RFC through a small local pilot only after these measurements and ownership contracts are reliable. Share proven mechanisms; avoid making a larger runtime a prerequisite for straightforward correctness fixes.

The review supports targeted repairs and better verification, not a wholesale rewrite or a claim that the plugin is broadly unsafe. The high-value work is concentrated in specific cross-thread, ownership, and measurement boundaries.
