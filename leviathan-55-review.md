# Leviathan 5.5 Repository Review

Review date: 2026-05-05
Repository: `/home/Levi.Kendall/dev/Leviathan-Rack2`

This review covers the full Leviathan VCV Rack plugin suite: Integral Flux, Proc, Temporal Deck, TD.Scope, Crownstep, Bifurx, Wyrm, Sil, shared helpers, assets, build/test wiring, and packaging metadata.

## Executive Summary

The suite is ambitious and substantially implemented. Temporal Deck, Crownstep, and Bifurx have meaningful unit/integration coverage and the fast test suite passed during this review. The strongest engineering work is in the extraction of Temporal Deck engine/input/sample-prep logic into testable units, Crownstep's rule coverage, Bifurx's DSP/preview model tests, and the use of async workers for expensive Temporal Deck sample prep and Crownstep AI.

The main risks are not broad compile failure in the plugin itself, but release-quality concerns around real-time safety, test target drift, UI/audio-thread data races, renderer portability, and incomplete metadata/manuals. The most urgent concrete problem found during the original review was that `make test-rack` failed to link `bifurx_runtime_spec`; that issue has since been fixed, and the document now treats it as a regression guard.

## Verification Performed

- `make test-build-fast`: passed.
- `make test-fast`: passed.
- `make -j4 plugin.so`: passed after follow-up fixes.
- `make test-rack`: initially failed at link time in `build/tests/bifurx_runtime_spec` with `undefined reference to isDragonKingDebugEnabled()`. After follow-up fixes, `make test-rack` passed.
- Static inspection of `plugin.json`, `Makefile`, `src`, `tests`, `doc`, and `res`.

## Top Priority Issues

### 1. Rack-linked test target was broken and should stay guarded

Evidence:

- [Makefile](Makefile):38 declares Rack-linked tests.
- [Makefile](Makefile):250 defines `build/tests/bifurx_runtime_spec` with `src/Bifurx.cpp` in dependencies, but the compile command only passes `tests/bifurx_runtime_spec.cpp src/PanelSvgUtils.cpp`.
- [src/Bifurx.cpp](src/Bifurx.cpp):887 calls `isDragonKingDebugEnabled()` from plugin code.
- [src/plugin.cpp](src/plugin.cpp):18 defines `isDragonKingDebugEnabled()`.

Original observed result:

```text
/usr/bin/ld: ... undefined reference to `isDragonKingDebugEnabled()'
collect2: error: ld returned 1 exit status
make: *** [Makefile:251: build/tests/bifurx_runtime_spec] Error 1
```

Current status: fixed after the original review by adding test-local debug stubs and correcting Bifurx Resample mode reachability. Keep `make test-rack` in CI so this does not regress.

### 2. Debug file I/O can run from Temporal Deck's audio callback

Evidence:

- [src/TemporalDeck.cpp](src/TemporalDeck.cpp):1177 creates a trace directory and opens a CSV file inside `TemporalDeck::process()`.
- [src/TemporalDeck.cpp](src/TemporalDeck.cpp):1397 writes CSV rows from `process()` while trace logging is active.

This is gated by `isDragonKingDebugEnabled()`, but the behavior is still in the real-time callback. File open, directory creation, stream formatting, and logging calls can block or allocate. Debug-only code still matters because it is easy to enable during exactly the sessions where timing diagnosis is needed.

Recommendation: move trace capture to a lock-free ring buffer plus UI/background drain, or make the audio thread only publish fixed-size events into a preallocated buffer. Opening/closing files should happen outside `process()`.

### 3. Sil performs expensive analysis and unsynchronized UI sharing directly in `process()`

Evidence:

- [src/Sil.cpp](src/Sil.cpp):189, [src/Sil.cpp](src/Sil.cpp):197, [src/Sil.cpp](src/Sil.cpp):208, and [src/Sil.cpp](src/Sil.cpp):221 compute `exp`, `pow`, and `log10`-related values in the audio path.
- [src/Sil.cpp](src/Sil.cpp):273 runs two 2048-point FFTs every 2048 samples from `process()`.
- [src/Sil.cpp](src/Sil.cpp):241 updates histogram arrays from `process()` while UI widgets read them directly later in the same file.
- [src/Sil.cpp](src/Sil.cpp):333 restores `colorScheme` without clamping, unlike the other modules.

This module looks more prototype-stage than the others. The FFT cadence is probably acceptable on many systems, but it creates periodic CPU spikes on the audio thread. More importantly, the UI reads `hist` and `spec` arrays without a double-buffer, atomics, or lock, so ThreadSanitizer would likely flag races.

Recommendation: precompute sample-rate-dependent coefficients in `onSampleRateChange()`, move spectrum analysis to a low-priority worker or double-buffered UI analysis path, and publish histogram/spectrum snapshots with a generation counter. Clamp `colorScheme` to `[0, SCHEME_LEN - 1]` in `dataFromJson()`.

### 4. Temporal Deck sample-build cancellation can still leave stale work in progress

Evidence:

- [src/TemporalDeck.cpp](src/TemporalDeck.cpp):1709 clears decoded/prepared state, then enqueues an `AsyncSampleBuildRequest::NONE` request.
- [src/TemporalDeckSampleLifecycle.cpp](src/TemporalDeckSampleLifecycle.cpp):166 handles only `LOAD_PATH` and `REBUILD_FROM_DECODED` explicitly; `NONE` falls through to `validDecoded == false` and clears only `sampleBuildInProgress`.
- [src/TemporalDeckSampleLifecycle.cpp](src/TemporalDeckSampleLifecycle.cpp):184 only discards a prepared result if its captured serial no longer equals the latest request serial.

The serial check is good for stale prepared output, but there is no true cancellation of long decode/prep work already running. Clearing a sample while a large decode/prep is active can still burn CPU/memory until the worker reaches the serial check.

Recommendation: add cooperative cancellation points to decode/prep, or add an atomic cancel generation checked before and after expensive allocations/resampling loops. Also expose load-failure state to the UI; `loadSampleFromPath()` currently returns true after queueing work regardless of decode outcome.

### 5. Plugin metadata is incomplete for a release

Evidence:

- [plugin.json](plugin.json):9 has empty `authorUrl`.
- [plugin.json](plugin.json):10 has empty `pluginUrl`.
- [plugin.json](plugin.json):11 has empty `manualUrl`.
- [plugin.json](plugin.json):61 still describes Crownstep as only `Checkers-driven sequencer`, despite chess and Othello support.

Recommendation: fill public URLs before distribution, add or link module manuals, and update descriptions to reflect actual module capabilities. For VCV Library release readiness, metadata quality matters nearly as much as code quality.

## Module Review

### Integral Flux

Strengths:

- Dual-channel slope behavior is consolidated through shared config structs rather than duplicated branches.
- Preview state uses atomics for small UI-facing fields.
- Output bus normalization is explicit and commented in [src/IntegralFlux.cpp](src/IntegralFlux.cpp):1043.

Risks and improvements:

- There is no dedicated test target for Integral Flux. Proc/Flux-style slope behavior is subtle enough to deserve deterministic tests for trigger behavior, cycle CV, EOR/EOC timing, bus normalization, and timing divider modes.
- The implementation is in one large file of roughly 1.5k lines. Consider extracting the pure slope core into a header/test harness as was done for Temporal Deck.
- The timing-divider options trade accuracy for CPU. Add tests that verify the menu state serializes and that interpolation remains bounded under high CV modulation.

### Proc

Strengths:

- Smaller single-channel form is useful and appears to share many hardened ideas with Integral Flux.
- JSON persistence covers cycle latch and timing/rendering options in [src/Proc.cpp](src/Proc.cpp):854.

Risks and improvements:

- Like Integral Flux, there are no direct automated tests.
- Proc and Integral Flux appear to duplicate a substantial amount of slope-generation logic. That raises maintenance cost and makes future fixes easy to apply to one module but not the other.
- Consider extracting a shared `SlopeCore` with module-specific wrappers.

### Temporal Deck

Strengths:

- Best-tested module in the suite. Fast tests cover platter input, frame input, engine behavior, expander preview, sample prep, virtual integration, and arc lights.
- Sample loading/prep is off the audio thread through `TemporalDeckSampleLifecycle`.
- Expander message protocol is documented in code and validated with tests.

Risks and improvements:

- Debug trace I/O occurs in `process()` as noted above.
- `applySampleRateChange()` can allocate/reset buffers from `process()` after sample-rate changes or pending state applies. That is safer than cross-thread allocation, but still real-time risky for large buffers.
- Sample decode failure is only logged from the worker. The UI caller cannot distinguish queued-success from decode-success.
- The built-in/expanded vinyl sync code downloads from GitHub and installs into user storage. It has reasonable path sanitization, but it should have tests for inventory parsing, traversal rejection, stale-file replacement, and partial-download cleanup.

### TD.Scope

Strengths:

- Uses double-buffered expander snapshots with atomic front-index/generation publishing in [src/TDScope.hpp](src/TDScope.hpp):326.
- Rendering has several performance paths and debug metrics.
- Drag contract is handled carefully and backed by Temporal Deck integration tests.

Risks and improvements:

- TD.Scope has large renderer files with OpenGL/NanoVG complexity but limited automated test coverage. Most regressions will be visual/performance regressions.
- Renderer mode defaults and legacy JSON migration are complex in [src/TDScope.hpp](src/TDScope.hpp):266. Add persistence tests for current and legacy renderer keys.
- The OpenGL renderer should have a clear fallback policy if shader compilation fails, GL context is unavailable, or Rack runs on weaker GPUs.

### Crownstep

Strengths:

- Strong domain tests: checkers, chess, Othello, AI depth sample, legal move filtering, castling, en passant, and persistence coverage.
- AI search is offloaded to a worker thread.
- Game-state serialization is broad and includes board/history/move-history.

Risks and improvements:

- The UI file is very large, and module state is broad. More code should move into pure helpers to keep the UI from becoming the domain model.
- AI worker stop waits for the current search to finish. If future difficulty/search depth increases, module deletion or Rack shutdown may block until search returns.
- `stepCounterStyle` is written, but any loaded value currently forces `STEP_COUNTER_RIBBON` in [src/CrownstepSerialization.cpp](src/CrownstepSerialization.cpp):190. If intentional, remove the serialized field; if not, restore the actual value.
- Randomized board value layout is seeded and persisted, which is good. Add tests for exact pitch reproducibility across JSON round trip.

### Bifurx

Strengths:

- Extensive DSP tests for frequency mapping, resonance, filter modes, resample mode, mirror behavior, and runtime telemetry.
- Audio-path preview publication is carefully rate-limited.
- Performance/debug capture is mostly UI-side in `BifurxUI.cpp` rather than audio-thread file I/O.

Risks and improvements:

- Rack-linked Bifurx runtime test previously failed to link; this is now fixed and should remain covered by CI.
- The module is mono-only: [src/Bifurx.cpp](src/Bifurx.cpp):1026 always sets one output channel. If mono is intentional, document it in the manual/description. If not, polyphony/stereo support is a major feature gap.
- Non-atomic UI flags such as `fftScaleDynamic`, `showModuleResponseOverlay`, `renderMode`, and debug booleans are read from UI/render code while potentially changed by menus or JSON. The risk is low for simple scalar fields, but C++ still considers this a data race.
- `BifurxSpectrumBase::updateOverlayCache()` reads `analysis*History` arrays written by the audio thread and uses only an atomic write-position publish. Without copying under a seqlock-style protocol, UI can read while audio writes newer samples.

### Wyrm

Strengths:

- Polyphony is supported through max-channel state arrays and per-channel sync triggers.
- Editable waveform points use atomics plus a version counter, which is a reasonable UI/audio bridge.
- JSON persistence covers custom wave points and rock state.

Risks and improvements:

- `Wyrm::process()` uses `syncTriggers[c]` for each poly channel; make sure `kWyrmMaxChannels` is at least Rack's current maximum polyphony and clamp channel count defensively.
- Rebuilding the wavetable happens on the audio thread when `waveVersion` changes in [src/Wyrm.cpp](src/Wyrm.cpp):423. The table is small enough that this may be acceptable, but aggressive editor gestures could still create audio spikes. Consider deferred/coalesced rebuilds or a double-buffered wavetable built on the UI thread.
- Add oscillator tests for pitch, sync reset, fold bounds, wavetable interpolation continuity, and JSON round trip.

### Sil

Strengths:

- Simple signal path and UI are easy to reason about.
- Includes low-band mono recovery, limiter, waveform history, and spectrum display in one compact module.

Risks and improvements:

- This is the least hardened module. It needs tests, real-time cleanup, snapshot-based UI data sharing, and clearer DSP claims.
- The description says `Automatic mastering module`, but the current code is closer to low-band recovery plus limiter plus visualization. Either expand the processing or narrow the public claim.
- No bypass is configured. For a mastering/dynamics module, `configBypass(INPUT_L_INPUT, OUTPUT_L_OUTPUT)` and right-channel bypass are expected.

## Shared Code and Architecture

### Panel SVG Utils

`PanelSvgUtils` is useful and tested, but it reparses SVG files with regex each time a widget constructor asks for a point/rect. This is fine at module construction scale, but the helper is fragile against SVG transforms, style-derived dimensions, single-quoted attributes, or non-100px/mm export assumptions.

Recommendation: cache loaded SVG text per path during widget construction, or add a small `PanelLayout` object that loads once and resolves all element IDs. Add explicit tests for missing IDs, malformed numeric attributes, single quotes if you want to support them, and unit assumptions.

### Codec

The codec path supports WAV/FLAC/MP3 and rejects unsupported channel counts. Good baseline. Missing hardening:

- Add tests for 24-bit WAV, float WAV, truncated chunks, unsupported >2 channel files, and huge-file limits.
- Consider streaming decode or early truncation for very large files rather than decode-full-then-truncate.

### Threading

Good patterns:

- Temporal Deck sample worker uses request serials.
- Crownstep AI worker keeps search off audio/UI.
- Wyrm and Temporal Deck use atomics for small UI/audio handoff.

Risk patterns:

- UI/render code reads arrays written by audio code in Bifurx and Sil without a full snapshot protocol.
- Some worker shutdown paths join threads; acceptable at module teardown, but future long-running network/search/decode work should be cancellable.
- Debug and telemetry code should not create hidden real-time hazards.

## Build, Tests, and CI

Current test coverage is uneven:

- Strong: Temporal Deck engine/input/sample/expander behavior, Crownstep rules/persistence, Bifurx DSP model.
- Weak/missing: Integral Flux, Proc, Wyrm, Sil, TDScope render behavior, Temporal Deck vinyl inventory/download, codec edge cases, panel SVG edge cases beyond existing tests.

Recommendations:

1. Fix `make test-rack` immediately.
2. Add `make test-fast` and `make test-rack` to CI.
3. Add a non-Rack pure DSP test target for Integral Flux/Proc shared slope core.
4. Add Wyrm oscillator and JSON tests.
5. Add Sil DSP bounds/persistence tests.
6. Add codec malformed-file tests.
7. Add at least one AddressSanitizer/UndefinedBehaviorSanitizer target for non-Rack tests.
8. Add a ThreadSanitizer-oriented harness for UI/audio snapshot structures where possible.

## Packaging and Assets

Strengths:

- `DISTRIBUTABLES += res` and license packaging are set up.
- Asset sizes are moderate; the largest bundled files are the vinyl PNGs and panel SVGs.

Risks and improvements:

- Repo root contains many planning/research markdown files. They are not distributed by `make dist`, but they can obscure the release source tree. Consider moving active design docs under `doc/` and archiving stale root notes.
- `plugin.json` lacks public/manual URLs.
- The apostrophe in `res/Vahdrim'Keth.svg` works in C++ string literals, but it can break careless shell scripts. The failed `xargs` attempt during review is an example. Consider renaming to a shell-safe asset filename.
- Add generated asset scripts under a documented `tools/` or `res-src/` convention if they are source assets; otherwise omit them from distribution.

## Documentation Gaps

The repository has many design docs, but user-facing documentation appears incomplete relative to the module count.

Recommended docs:

- One concise manual page per module.
- Explicit mono/poly/stereo support table.
- Expander placement instructions for TD.Scope.
- Temporal Deck sample format limits and where loaded/expanded vinyl assets are stored.
- Bifurx render mode troubleshooting and GPU fallback note.
- Crownstep explanation for checkers/chess/Othello modes, pitch mapping, and sequence window controls.
- Sil signal-flow diagram and exact claims.

## Suggested Fix Order

1. Keep `make test-rack` passing and add it to CI.
2. Remove file I/O from Temporal Deck `process()` debug trace path.
3. Add snapshot/double-buffer protocols for Sil histogram/spectrum and Bifurx analysis arrays.
4. Add tests for Sil, Wyrm, Integral Flux, and Proc.
5. Fill `plugin.json` URLs and update module descriptions.
6. Add codec and vinyl inventory tests.
7. Refactor shared slope logic out of Proc/Integral Flux.
8. Harden renderer fallback paths and document them.

## Implementation Notes for Delegation

This section is written for a smaller implementation model. Each task is scoped so it can be handled independently. Do not mix unrelated tasks in one patch. After each task, run the listed verification commands and include the important output in the final response.

General rules for all tasks:

- Preserve unrelated working-tree changes. Check `git status --short --untracked-files=all` before editing.
- Prefer small helper structs/functions over broad rewrites.
- Do not add allocations, file I/O, locks, or blocking joins to audio `process()` callbacks.
- For Rack UI/audio data sharing, prefer double-buffer snapshots with atomics over sharing mutable arrays directly.
- Use `rg` to find symbols and `make test-fast`, `make test-rack`, and `make -j4 plugin.so` for verification when relevant.

### Task A: Keep `make test-rack` passing

Current state after follow-up work: the original link issue has been fixed in [tests/bifurx_runtime_spec.cpp](tests/bifurx_runtime_spec.cpp) by adding test-local stubs for `isDragonKingDebugEnabled()` and `refreshDragonKingDebugEnabled()`. Bifurx Resample mode was also made reachable by setting `kBifurxUiModeCount = kBifurxModeCount` in [src/Bifurx.hpp](src/Bifurx.hpp), adding the 11th label in [src/Bifurx.cpp](src/Bifurx.cpp), and preventing the high-resonance self-osc seed from driving Resample mode.

If this regresses:

- Do not link `src/plugin.cpp` into `bifurx_runtime_spec` unless absolutely necessary. It pulls broad plugin globals and module model registration into a focused DSP/runtime test.
- Keep the test self-contained with stubs for plugin-level debug helpers.
- Verify mode count assumptions: `kBifurxModeLabels` has 11 labels, including `Resample`; UI/config mode range should include all labels intended to be selectable.
- Run `make test-rack`.
- Run `make test-fast` because Bifurx has non-Rack DSP tests that can catch mode-index drift.
- Run `make -j4 plugin.so`.

Expected passing summary:

```text
[SUMMARY] bifurx_runtime_spec passed 12 tests
Summary: 5/5 passed
Summary: 1/1 passed
```

### Task B: Remove Temporal Deck debug trace file I/O from audio `process()`

Problem files and symbols:

- [src/TemporalDeck.cpp](src/TemporalDeck.cpp), `TemporalDeck::process()`.
- Search for `trace`, `csv`, `isDragonKingDebugEnabled`, `open`, `ofstream`, and `mkdir`.
- The risky behavior is opening/creating directories and writing formatted CSV rows while the audio callback is running.

Target design:

- Audio thread should only write fixed-size POD trace records into a preallocated ring buffer.
- A non-audio thread should own file creation, stream lifetime, formatting, and disk writes.
- If adding a background worker is too large, the first acceptable step is to disable file writes from `process()` and keep only in-memory trace counters. Do not preserve blocking file I/O in audio.

Suggested implementation:

- Add a small struct near Temporal Deck debug/trace state:

```cpp
struct TemporalDeckTraceEvent {
	float sampleTime = 0.f;
	float readHead = 0.f;
	float writeHead = 0.f;
	float outputL = 0.f;
	float outputR = 0.f;
	uint32_t flags = 0;
};
```

- Add a fixed ring buffer to `TemporalDeck`, for example `std::array<TemporalDeckTraceEvent, 8192> traceEvents`.
- Add atomics `traceWriteIndex`, `traceReadIndex`, and `traceDroppedCount`.
- In `process()`, replace direct CSV writes with a non-blocking push:

```cpp
const uint32_t write = traceWriteIndex.load(std::memory_order_relaxed);
const uint32_t next = (write + 1u) & (traceEvents.size() - 1u);
const uint32_t read = traceReadIndex.load(std::memory_order_acquire);
if (next != read) {
	traceEvents[write] = event;
	traceWriteIndex.store(next, std::memory_order_release);
}
else {
	traceDroppedCount.fetch_add(1u, std::memory_order_relaxed);
}
```

- Drain from a worker thread or UI-side periodic callback. The drain may open the file and format CSV because it is not the real-time callback.
- Use a power-of-two ring size if using bitmask wrap. Otherwise use `% traceEvents.size()`.
- Do not use `std::mutex` in `process()`.
- Do not allocate strings in `process()`.

Testing strategy:

- Add a pure test if trace-push logic is extracted into a small helper header, for example `src/TemporalDeckTrace.hpp`.
- Test ring push/drain ordering, overflow drop count, wraparound, and disabled tracing.
- If not extracted, perform manual verification by running debug tracing in Rack later, but still run the automated suite.

Verification commands:

```sh
make test-fast
make -j4 plugin.so
```

### Task C: Add Sil snapshot protocol for histogram and spectrum data

Problem files and symbols:

- [src/Sil.cpp](src/Sil.cpp), module state arrays `hist`, `spec`, waveform/history arrays, FFT code, and display widgets later in the same file.
- Search for `hist`, `spec`, `fft`, `Spectrum`, `Wave`, `drawLayer`, and `dataFromJson`.

Target design:

- Audio thread writes into a back buffer only.
- UI reads only a published front snapshot.
- Publication is a single atomic index or sequence store after the copy is complete.
- UI should copy the snapshot once per draw or frame into local stack/member data before rendering.

Suggested implementation:

- Define a snapshot struct inside or near `Sil`:

```cpp
struct SilAnalyzerSnapshot {
	std::array<float, kHistBins> hist {};
	std::array<float, kSpecBins> spec {};
	std::array<float, kWaveSamples> waveL {};
	std::array<float, kWaveSamples> waveR {};
};
```

- If existing sizes are raw constants, convert them to named `constexpr int` values first. Keep this mechanical.
- Add two snapshots:

```cpp
SilAnalyzerSnapshot analyzerSnapshots[2];
std::atomic<int> analyzerPublishedIndex {0};
```

- Audio thread writes to `1 - analyzerPublishedIndex.load(std::memory_order_relaxed)` and publishes with `store(..., std::memory_order_release)` after all fields are copied.
- UI reads index with `load(std::memory_order_acquire)`, then copies that snapshot to a local variable before drawing.
- If copying full waveform every sample is too expensive, continue filling rolling arrays in audio but publish a decimated/copy snapshot at a lower cadence, such as every 512 or 2048 samples.
- Clamp `colorScheme` in `dataFromJson()`:

```cpp
colorScheme = clamp(int(json_integer_value(colorSchemeJ)), 0, SCHEME_LEN - 1);
```

- Add bypass configuration in the constructor if input/output IDs are stereo:

```cpp
configBypass(INPUT_L_INPUT, OUTPUT_L_OUTPUT);
configBypass(INPUT_R_INPUT, OUTPUT_R_OUTPUT);
```

Testing strategy:

- Add or extend a pure Sil test file, preferably separate from Rack. The current repository now has `tests/sil_micropeak_spec.cpp`; either add a new `tests/sil_dsp_spec.cpp` or add non-UI DSP tests to the existing Sil test target if no Rack symbols are required.
- Test that invalid JSON color scheme values clamp instead of producing out-of-range array access. If this requires Rack JSON/module construction, make it a Rack-linked test; otherwise factor the clamp helper into a pure function.
- Test any extracted snapshot helper for front/back publication ordering.

Verification commands:

```sh
make test-fast
make -j4 plugin.so
```

### Task D: Move Sil FFT or heavy analysis off the main audio thread

Problem files and symbols:

- [src/Sil.cpp](src/Sil.cpp), FFT path around the code that runs every 2048 samples.
- Search for `fft`, `spec`, `2048`, and `process`.

Target design:

- The audio thread should collect samples into a chunk buffer.
- When the chunk fills, it should attempt to hand the chunk to a worker without blocking.
- The worker performs FFT/analysis and publishes a snapshot for the UI.
- If the worker is busy, drop the chunk. Do not block audio waiting for analysis.

Suggested implementation:

- Follow the same pattern used by the Sil micropeak worker if present in [src/Sil.cpp](src/Sil.cpp): a fill buffer, pending buffer, `std::mutex`, `std::condition_variable`, worker thread, and an atomic published result.
- Important: the audio thread may attempt `std::unique_lock<std::mutex> lock(mutex, std::try_to_lock)`, but it must skip if the lock is unavailable.
- Worker lifecycle should start in the module constructor and stop in the destructor.
- Stop sequence should set a `stop` flag under mutex, `notify_one()`, then `join()` outside audio processing.
- Keep all `std::vector` growth out of `process()`. Use `std::array` for fixed 2048-sample chunks.

Testing strategy:

- Keep the FFT math testable in a pure helper if possible, for example `src/SilAnalysis.hpp`.
- Add tests for clean sine, silence, impulse, and broad transient behavior if the same analysis feeds indicators.
- Manual performance verification in Rack is still needed because this is a scheduling/CPU-spike change.

Verification commands:

```sh
make test-fast
make -j4 plugin.so
```

### Task E: Add cooperative cancellation to Temporal Deck sample builds

Problem files and symbols:

- [src/TemporalDeck.cpp](src/TemporalDeck.cpp), calls that enqueue sample build requests.
- [src/TemporalDeckSampleLifecycle.cpp](src/TemporalDeckSampleLifecycle.cpp), worker request/result handling.
- Search for `AsyncSampleBuildRequest`, `sampleBuildRequestSerial`, `sampleBuildInProgress`, `LOAD_PATH`, `REBUILD_FROM_DECODED`, `NONE`, `decode`, and `prepare`.

Target design:

- Clearing or replacing a sample should make existing expensive decode/prep work stop as soon as practical.
- Stale results should still be discarded by serial, but stale work should not continue through large allocations/resampling loops unnecessarily.
- UI/module state should expose whether load succeeded, failed, or is still pending.

Suggested implementation:

- Add `std::atomic<uint64_t> sampleBuildCancelSerial` or reuse/increment the request serial as a cancellation generation.
- Capture the generation at the start of worker handling.
- Check cancellation before decode, after decode, before large resampling/prep loops, periodically inside loops, and before publishing results.
- On cancel, clear `sampleBuildInProgress` and publish a canceled/idle status without touching current sample buffers.
- Add a status enum:

```cpp
enum class SampleBuildStatus {
	Idle,
	Queued,
	Loading,
	Ready,
	Failed,
	Canceled
};
```

- Publish the status through an atomic or under the existing worker-result handoff. Do not make UI poll worker-owned mutable state directly.
- If adding full status UI is too large, first add internal state and tests; UI display can be a follow-up.

Testing strategy:

- Add pure lifecycle tests if the worker logic can be isolated.
- Test that a `NONE` or clear request increments cancellation and prevents stale results from replacing buffers.
- Test that two queued paths only allow the latest serial to publish.
- Test failed decode status with an invalid path or malformed fixture.

Verification commands:

```sh
make test-fast
make -j4 plugin.so
```

### Task F: Fix Crownstep `stepCounterStyle` JSON behavior

Problem files and symbols:

- [src/CrownstepSerialization.cpp](src/CrownstepSerialization.cpp), search for `stepCounterStyle`.
- [src/Crownstep.hpp](src/Crownstep.hpp) or related headers for enum values.
- [tests/crownstep_persistence_spec.cpp](tests/crownstep_persistence_spec.cpp).

Current risk:

- The serializer writes `stepCounterStyle`, but loading appears to force `STEP_COUNTER_RIBBON`.

Decide one of two paths:

- If multiple styles are still supported, deserialize the stored integer with clamping to the valid enum range.
- If only ribbon is supported, remove the JSON write or keep reading legacy values but always write the current canonical value.

Suggested implementation for supported multiple styles:

```cpp
json_t* stepCounterStyleJ = json_object_get(root, "stepCounterStyle");
if (stepCounterStyleJ) {
	const int raw = int(json_integer_value(stepCounterStyleJ));
	stepCounterStyle = clamp(raw, 0, STEP_COUNTER_STYLE_LEN - 1);
}
```

- Use the actual enum/count names from the codebase. Do not invent `STEP_COUNTER_STYLE_LEN` if a different name already exists.
- Extend `tests/crownstep_persistence_spec.cpp` to set a non-default style, serialize, load into a new module/state, and assert equality.

Verification commands:

```sh
make test-fast
make -j4 plugin.so
```

### Task G: Add Wyrm oscillator and persistence tests

Problem files and symbols:

- [src/Wyrm.cpp](src/Wyrm.cpp), `Wyrm::process()`, wavetable rebuild code, sync triggers, JSON persistence.
- [src/WyrmWidget.cpp](src/WyrmWidget.cpp) and [src/WyrmWaveEditor.cpp](src/WyrmWaveEditor.cpp) only if UI persistence/editor behavior is involved.
- Search for `waveVersion`, `wavetable`, `syncTriggers`, `dataToJson`, and `dataFromJson`.

Suggested test target:

- Add `tests/wyrm_oscillator_spec.cpp`.
- Add `build/tests/wyrm_oscillator_spec` to `TEST_BINS_RACK` if constructing `Wyrm` requires Rack, or to `TEST_BINS_NON_RACK` only if the oscillator core is extracted into a Rack-free helper.
- If Rack-linked, use the existing `RACK_TEST_*` flags pattern in [Makefile](Makefile).

Tests to add:

- Pitch stability: at a known V/OCT and sample rate, count zero crossings or phase wraps over a fixed sample count.
- Sync reset: send a rising sync gate and assert phase/output discontinuity occurs deterministically.
- Fold bounds: high fold values should stay finite and bounded.
- Wavetable interpolation: adjacent samples around custom point changes should not produce NaN/Inf.
- JSON round trip: custom wave points and rock state survive `dataToJson()`/`dataFromJson()`.

Verification commands:

```sh
make test-fast
make -j4 plugin.so
```

### Task H: Add Integral Flux and Proc shared slope tests

Problem files and symbols:

- [src/IntegralFlux.cpp](src/IntegralFlux.cpp).
- [src/Proc.cpp](src/Proc.cpp).
- Search for `Slope`, `EOR`, `EOC`, `cycle`, `rise`, `fall`, `shape`, `timing`, and `divider`.

Target design:

- Extract pure slope math/state transition into a Rack-free helper if feasible, for example `src/SlopeCore.hpp`.
- Keep module-specific param/input/output wiring in the existing `.cpp` files.
- Avoid a giant refactor first. Start by extracting a minimal helper that can step a slope with normalized controls and produce phase/output/EOR/EOC.

Suggested helper shape:

```cpp
struct SlopeCoreParams {
	float riseSeconds = 0.1f;
	float fallSeconds = 0.1f;
	float shape = 0.f;
	bool cycle = false;
};

struct SlopeCoreState {
	float phase = 0.f;
	bool rising = false;
	bool falling = false;
};

struct SlopeCoreOutput {
	float voltage = 0.f;
	bool eor = false;
	bool eoc = false;
};
```

- The actual names can differ; the goal is a pure, deterministic stepping function.
- Add tests for trigger start, cycle mode, EOR/EOC timing, CV modulation bounds, and exact termination at slow and fast sample rates.
- Once tests exist, migrate Proc and Integral Flux to use the helper in small steps.

Verification commands:

```sh
make test-fast
make -j4 plugin.so
```

### Task I: Harden Bifurx analysis snapshot sharing

Problem files and symbols:

- [src/Bifurx.cpp](src/Bifurx.cpp), `pushAnalysisSample`.
- [src/BifurxUI.cpp](src/BifurxUI.cpp), `BifurxSpectrumBase::updateOverlayCache()`.
- [src/Bifurx.hpp](src/Bifurx.hpp), analysis arrays and published indices.
- Search for `analysisRawInputHistory`, `analysisOutputHistory`, `analysisResponseOutputHistory`, `analysisPublishedWritePos`, and `analysisPublishSeq`.

Target design:

- UI should not read arrays while audio is writing the same memory.
- Use a seqlock-style protocol or double-buffered FFT input snapshot.

Suggested seqlock implementation:

- Add `std::atomic<uint32_t> analysisSeq`.
- Audio increments to odd before writing a publishable block, copies/writes the block, then increments to even after completion.
- UI reads `seqA`, copies data, reads `seqB`, and accepts only if `seqA == seqB` and even.
- If copying 4096 samples per publish is acceptable, simpler double-buffering is clearer:

```cpp
struct BifurxAnalysisSnapshot {
	std::array<float, kFftSize> raw {};
	std::array<float, kFftSize> out {};
	std::array<float, kFftSize> response {};
	int writePos = 0;
};
BifurxAnalysisSnapshot analysisSnapshots[2];
std::atomic<int> analysisPublishedIndex {0};
```

- Audio fills a back snapshot at hop cadence and publishes the index with release ordering.
- UI reads the index with acquire ordering and copies from that immutable snapshot.

Testing strategy:

- Add a small pure helper test for snapshot publish/read if extracted.
- Run existing Bifurx filter and runtime tests.

Verification commands:

```sh
make test-fast
make test-rack
make -j4 plugin.so
```

### Task J: Fill plugin metadata and docs

Problem files:

- [plugin.json](plugin.json).
- `doc/` for module manuals.
- Root planning docs can be moved only if explicitly requested; do not reorganize broad documentation as a drive-by change.

Suggested metadata updates:

- Fill `authorUrl`, `pluginUrl`, and `manualUrl` with real public URLs.
- Update Crownstep description from checkers-only to checkers/chess/Othello sequencing.
- Add a support matrix to docs:

```text
Module | Mono input | Stereo input | Polyphony | Expander support | Notes
```

- Add concise manuals for Sil, Bifurx, Temporal Deck, TD.Scope, Crownstep, Wyrm, Proc, and Integral Flux.
- For Bifurx, explicitly state whether mono output is intentional. If stereo/poly support is desired, treat it as a separate feature task, not a documentation patch.

Verification commands:

```sh
make dist
```

If `make dist` is unavailable or requires environment setup, run `make -j4 plugin.so` and inspect the generated package target expected by the Rack SDK.

### Task K: Add codec and Panel SVG utility edge-case tests

Problem files and symbols:

- [src/codec.cpp](src/codec.cpp) and related headers.
- [src/PanelSvgUtils.cpp](src/PanelSvgUtils.cpp).
- Existing tests around panel SVG utilities are Rack-linked and already run under `make test-rack`.

Codec tests to add:

- 24-bit WAV fixture.
- Float WAV fixture.
- Truncated WAV/FLAC/MP3 file returns failure without crash.
- Unsupported channel count greater than 2 is rejected.
- Huge-file or excessive-frame count is capped or rejected.

Panel SVG tests to add:

- Single-quoted attributes if support is desired.
- Attributes in different order.
- Missing numeric attributes.
- Malformed numeric values.
- Explicit unit scale assumptions.
- Unsupported transforms should fail clearly or be documented as unsupported.

Implementation notes:

- Keep binary fixtures small. Use generated tiny fixtures checked into `tests/fixtures` only if acceptable for the repo.
- If generating fixtures in tests, avoid requiring network or external tools.
- Do not broaden regex behavior unless tests define the expected behavior.

Verification commands:

```sh
make test-fast
make test-rack
make -j4 plugin.so
```

## Final Assessment

Leviathan is functionally broad and has several mature subsystems, especially Temporal Deck, Crownstep, and Bifurx DSP. The next quality step is to make the suite uniformly release-hardened: keep the Rack-linked tests passing in CI, eliminate real-time debug I/O, turn UI/audio data sharing into explicit snapshot protocols, and bring the newer/less-tested modules up to the same test standard as the strongest modules.
