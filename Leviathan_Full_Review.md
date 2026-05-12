# Leviathan Full Codebase Review

Review date: 2026-05-12

Scope: local workspace review of the VCV Rack modules Integral Flux, Proc, Temporal Deck, TD.Scope, Crownstep, Bifurx, Wyrm, and Sil. Reviewed source under `src/`, module metadata in `plugin.json`, and available tests under `tests/`.

## Verification Run

Command run:

```sh
make test
```

Result: all available tests passed.

Covered by tests: Temporal Deck platter/engine/expander/menu/frame/sample/virtual integration, Crownstep behavior and persistence, Bifurx filter/runtime behavior, Sil micropeak repair kernel, panel SVG utilities, and ODR/link uniqueness.

Not covered by tests: Rack UI/audio thread races, OpenGL/NanoVG runtime behavior in Rack, filesystem/network behavior from UI menus, realtime-safety under live audio, and manual interaction stress cases.

## Work Completed After Review

Status date: 2026-05-12

Quick fixes completed from this review:

- `TD.Scope`: fixed context-menu typo from `Inverted Verical` to `Inverted Vertical`.
- `Crownstep`: added the existing `sequenceMutex` guard around sequence-history reads in the sequence-length display quantity.
- `Integral Flux`: converted the simple BLEP and timing-interpolation menu flags to `std::atomic<bool>` and updated JSON/menu accessors to use atomic load/store.
- `Proc`: converted the simple BLEP and timing-interpolation menu flags to `std::atomic<bool>` and updated JSON/menu accessors to use atomic load/store.
- `Wyrm`: converted the simple `lfoMode`, `editorLocked`, and `sandViewEnabled` flags to `std::atomic<bool>` and updated JSON/menu/UI/audio accessors to use atomic load/store.
- `Sil`: replaced repeated constructor-time `APP->engine->getSampleRate()` calls with one guarded `initialSampleRate` value and clamped histogram bin size to at least one sample.

Verification after these changes:

```sh
make test
```

Result: all available tests passed after the quick-fix pass.

Still not addressed by this pass:

- Wyrm rock editing still needs a real snapshot or command-queue handoff before the P1 race is resolved.
- Bifurx, TD.Scope, and Temporal Deck still have additional plain UI/audio shared settings that should be classified and converted.
- Sil still needs a realtime-safety pass for repair-buffer reconfiguration/debug I/O behavior from or near the audio path.
- Temporal Deck file I/O should still move to an async UI-owned workflow where practical.
- `timingUpdateDiv` in Integral Flux/Proc was intentionally not converted as a simple atomic because its setter also resets timing counters and validity flags; that needs a small state-transition helper, not a raw atomic.

## Remediation Playbook

This section turns the remaining review findings into concrete implementation work. Use this as the preferred order of operations.

### 1. Fix Wyrm Rock State Handoff

Primary files:

- `src/Wyrm.hpp`
- `src/Wyrm.cpp`
- `src/WyrmWaveEditor.cpp`

Target design:

- Add a plain-data `WyrmRockSnapshot` containing `rockCount`, `rocks`, and any derived boundary/cache data that `process()` reads.
- Keep two snapshots in the module: one UI-writable pending snapshot and one audio-readable active snapshot.
- Publish pending-to-active with an atomic generation counter or an atomic index swap.
- In `process()`, read exactly one snapshot generation per process call and use that local copy for all rock DSP decisions.
- In `WyrmWaveEditor`, stop mutating `module->rocks[...]` directly while audio can read it. Edit the pending snapshot, rebuild pending cache, then publish.

Do not fix this with a blocking mutex in `Wyrm::process()`. If a lock is unavoidable, audio must use `try_lock()` and continue with the previous active snapshot on failure.

Acceptance checks:

- Dragging rocks cannot mutate the same `rocks` array that `process()` is reading.
- `process()` never observes half-updated rock position/cache state.
- Existing wave point atomics remain intact.
- Add a JSON round-trip test for rock state before or during the refactor.

### 2. Finish Cross-Thread Settings Cleanup

Primary files:

- `src/Bifurx.hpp`
- `src/Bifurx.cpp`
- `src/BifurxUI.cpp`
- `src/BifurxGL.cpp`
- `src/TDScope.hpp`
- `src/TDScope.cpp`
- `src/TDScopeWidget.cpp`
- `src/TemporalDeck.cpp`
- `src/TemporalDeckUI.cpp`
- `src/IntegralFlux.cpp`
- `src/Proc.cpp`

Target design:

- For single scalar settings read by audio and toggled by UI, use `std::atomic<bool>` or `std::atomic<int>` with relaxed load/store.
- For setting changes that also reset counters, clear caches, or update several related fields, use a small setter called from the audio thread or a versioned pending-settings struct.
- Replace `createBoolPtrMenuItem()` for audio-consumed settings with `createCheckMenuItem()` lambdas that load/store atomics or enqueue a settings command.
- Leave UI-only settings plain only if they are never read by `process()` or another non-UI thread.

Concrete remaining items:

- Bifurx: classify `highResonanceSelfOscEnabled`, `softLimitingEnabled`, `fftScaleDynamic`, `showModuleResponseOverlay`, `useGlShaderRenderer`, debug flags, modulation quality, and control update division.
- TD.Scope: classify range/channel/invert/color/debug settings and any values touched by both expander processing and widget rendering.
- Temporal Deck: move interpolation and trace/debug toggles to atomics or a pending settings snapshot.
- Integral Flux/Proc: replace `timingUpdateDiv` direct UI mutation with a setter that updates the divider, resets `timingUpdateCounter`, and invalidates timing caches together.

Acceptance checks:

- No audio-consumed setting is passed to `createBoolPtrMenuItem()` by pointer.
- Each audio-consumed setting has one clear ownership model: atomic scalar, audio-thread command, or versioned snapshot.
- JSON load/save uses the same accessors as the menu path.

### 3. Remove Sil Realtime Hazards

Primary file:

- `src/Sil.cpp`

Target design for repair latency:

- Preallocate repair and bypass delay buffers for the maximum required latency at the current sample rate.
- On Repair toggle, change only active latency/index state in `process()`.
- Move vector resize/assign operations to the constructor, `onSampleRateChange()`, or a non-audio reinitialization path.

Target design for debug capture:

- Replace direct mutex/file writes from `process()` with a bounded debug event ring buffer.
- A non-audio worker or UI-side drain writes events to disk.
- If the ring is full, drop the event and increment an atomic drop counter.

Acceptance checks:

- `Sil::process()` does not allocate, resize vectors, open files, close files, or write streams.
- Repair enable/disable can be toggled without vector assignment from the audio thread.
- Debug capture failure cannot block audio.

### 4. Move Temporal Deck Trace I/O Out Of `process()`

Primary files:

- `src/TemporalDeck.cpp`
- `src/TemporalDeckUI.cpp`

Target design:

- Audio thread emits fixed-size trace events into a bounded ring buffer.
- UI/background side owns directory creation, file opening, file writing, and file close.
- Trace enable/disable should publish an atomic state change, not create files directly from audio.

Acceptance checks:

- `process()` contains no directory creation, file open, file close, or stream write for trace logging.
- Ring overflow is handled by dropping trace events and tracking a counter.

### 5. Document Or Refactor TD.Scope Expander Request Ownership

Primary files:

- `src/TDScope.hpp`
- `src/TemporalDeck.cpp`

Target design:

- Decide whether TD.Scope is intentionally writing requests into the host module's `rightExpander.producerMessage`.
- If yes, document the direction and ownership at the write site and host read site.
- If no, refactor to TD.Scope-owned producer storage according to Rack expander semantics.

Acceptance checks:

- The request path has comments explaining which module owns each producer/consumer message buffer.
- Add a focused hostless test or mock if feasible.

## Executive Summary

The codebase is functionally broad and the included tests are useful, especially around Temporal Deck, Crownstep, Bifurx, and Sil repair. The largest remaining risk class is thread ownership: several modules expose ordinary `bool`, `int`, vectors, or structs to both Rack's UI thread and audio thread without atomics, locks, or a command queue. That is undefined behavior in C++ and can manifest as rare crashes, stale settings, denorm/random values, or audio glitches.

The second major risk is realtime safety. A few paths still perform filesystem work, vector allocation/reallocation, or potentially blocking debug writes from `process()`. Some of this is debug-gated, but debug mode should still not block Rack's audio thread.

## Priority Findings

### P1: Wyrm UI Mutates Oscillator Rock State While Audio Reads It

Files:

- `src/Wyrm.cpp:724`
- `src/Wyrm.cpp:749`
- `src/Wyrm.cpp:824`
- `src/WyrmWaveEditor.cpp:2008`
- `src/WyrmWaveEditor.cpp:2032`
- `src/WyrmWaveEditor.cpp:2036`

`Wyrm::process()` reads `rockCount`, `rocks`, and derived rock behavior in the audio thread while the wave editor directly mutates `module->rocks[rockIndex]`, calls `rebuildRockBoundaryCache()`, and may call `sculptWaveAroundRock()` from UI drag handlers. `wavePoints` are atomic, but the rock data and cache are not.

Impact: undefined behavior and possible audio-thread crash/corruption while dragging rocks. Even if crashes are rare, reads can observe partially updated rock phase/value/cache pairs, causing discontinuities or invalid geometry/DSP state.

Recommendation: move all rock and waveform edits through an audio-thread-safe handoff. Practical options:

- Store complete immutable rock snapshots in a double-buffer and publish with an atomic generation.
- Use a lock-free command queue from UI to audio, then audio applies rock changes at block/sample boundary.
- If locking is used, never take a blocking mutex in the audio thread; use try-lock with a safe previous snapshot fallback.

### P1: Cross-Thread Module Settings Are Plain Scalars Across Multiple Modules

Files:

- `src/IntegralFlux.cpp:176`
- `src/IntegralFlux.cpp:999`
- `src/IntegralFlux.cpp:1457`
- `src/Proc.cpp:153`
- `src/Proc.cpp:934`
- `src/Proc.cpp:1327`
- `src/Bifurx.hpp:646`
- `src/Bifurx.cpp:700`
- `src/BifurxUI.cpp:823`
- `src/Wyrm.hpp:216`
- `src/Wyrm.cpp:743`
- `src/WyrmWidget.cpp:428`
- `src/TDScope.hpp:84`
- `src/TDScope.hpp:361`
- `src/TDScopeWidget.cpp:234`
- `src/TemporalDeck.cpp:696`
- `src/TemporalDeck.cpp:1155`
- `src/TemporalDeck.cpp:1897`

Many module runtime options are plain `bool`/`int` values changed by context menus or UI widgets and read in `process()` or audio-adjacent code. Examples include Bifurx limiting/render/overlay flags, TD.Scope range/channel/invert/debug flags, Temporal Deck interpolation/trace settings, and remaining Integral Flux/Proc timing-rate state.

Update after quick-fix pass: the simplest Integral Flux, Proc, and Wyrm boolean flags listed above have been converted to atomics. This finding remains open for the larger setting set and for any setting whose change has side effects beyond storing one scalar.

Impact: undefined behavior by C++ memory model. These may work most of the time on x86, but they are still unsound and can fail under compiler optimization, other architectures, or timing stress.

Recommendation: classify each setting by thread ownership.

- Audio-thread settings should be `std::atomic<T>` or copied from a UI-owned pending value at safe points.
- UI-only settings should not be read from `process()`.
- Compound state changes should use a versioned snapshot so audio sees consistent sets of values.
- Avoid `createBoolPtrMenuItem()` for values consumed by audio unless the pointed value is not read by the audio thread.

### P1: Temporal Deck Trace File Creation/Writes Are Driven From `process()`

Files:

- `src/TemporalDeck.cpp:1173`
- `src/TemporalDeck.cpp:1178`
- `src/TemporalDeck.cpp:1183`
- `src/TemporalDeck.cpp:1362`
- `src/TemporalDeck.cpp:1889`

When scope drag trace logging is enabled, the audio thread creates directories, opens files, and later writes trace rows from `process()`. This is debug-gated, but it is still realtime-unsafe.

Impact: audio dropouts or Rack stalls when enabling trace logging or writing trace rows, especially on slow disks, network filesystems, or when the user directory is under sync software.

Recommendation: audio thread should only enqueue fixed-size trace events into a lock-free/ring buffer. A background/UI worker should own filesystem creation and file writes.

### P2: Sil Can Allocate/Reconfigure Delay Buffers In The Audio Path

Files:

- `src/Sil.cpp:708`
- `src/Sil.cpp:721`
- `src/Sil.cpp:723`
- `src/Sil.cpp:1179`
- `src/Sil.cpp:1186`

`Sil::process()` calls `configureLimiterFastPath()` and `configureRepairLatency()` every sample. The fast path usually returns, but when repair state or sample-rate-related size changes require reconfiguration, `configureRepairLatency()` assigns vectors and reconfigures buffers from the audio thread.

Impact: toggling Repair or reacting to a size mismatch can allocate/free memory in the DSP callback. This can produce audible dropouts and is a common Rack plugin rejection risk.

Recommendation: preallocate both bypass and repair delay buffers for max latency, then switch active latency with indices only. Handle sample-rate reconfiguration in `onSampleRateChange()` or through an audio-safe pending reinit flag.

### P2: Sil Debug Capture Writes Under Mutex From Audio Thread

Files:

- `src/Sil.cpp:967`
- `src/Sil.cpp:1009`
- `src/Sil.cpp:1091`
- `src/Sil.cpp:1198`

Micropeak debug capture uses a mutex and file stream writes while processing audio. It is behind DragonKing debug mode, but it is still blocking work in the audio callback.

Impact: debug capture can distort the very glitches it is intended to investigate and can block Rack if disk I/O stalls.

Recommendation: use a bounded ring buffer for debug events and write them from a non-audio worker. If the ring is full, drop events and increment an atomic drop counter.

### P2: Crownstep Has Some UI Reads Of Sequence State Without The Existing Mutex

Files:

- `src/Crownstep.cpp:12`
- `src/CrownstepPlayback.cpp:4`
- `src/CrownstepSerialization.cpp:42`
- `src/CrownstepUI.cpp:2722`

Most Crownstep sequence/history access is protected by `sequenceMutex`, but `CrownstepSeqLengthQuantity::getDisplayValueString()` reads `history.size()` without taking that mutex. Since history is modified elsewhere under lock, this unguarded read is still a data race.

Impact: low-frequency UI crash or incorrect display value while history changes.

Recommendation: either lock `sequenceMutex` for this read or expose a small atomic cached history length updated under the same lock when history changes.

### P2: Bifurx Context Menu Settings Mix UI, Render, And Audio Ownership

Files:

- `src/Bifurx.hpp:646`
- `src/Bifurx.hpp:648`
- `src/Bifurx.cpp:84`
- `src/Bifurx.cpp:700`
- `src/BifurxUI.cpp:823`
- `src/BifurxUI.cpp:824`
- `src/BifurxUI.cpp:826`

Bifurx has many settings toggled directly by menu items. Some are UI/render-only; others alter audio behavior, including high-resonance self-oscillation and soft limiting. They are plain fields.

Impact: audio-thread data races and inconsistent render/audio state while toggling options.

Recommendation: split fields into `AudioSettings` and `UiSettings`. Publish audio settings atomically/versioned; keep renderer-only settings UI-owned or atomic if shared between UI render components.

### P2: TD.Scope Request Writes Use Neighbor Expander Storage Directly

Files:

- `src/TDScope.hpp:119`
- `src/TDScope.hpp:401`
- `src/TDScope.hpp:429`
- `src/TDScope.hpp:435`
- `src/TemporalDeck.cpp:791`
- `src/TemporalDeck.cpp:1227`

TD.Scope initializes `leftExpander.producerMessage`, but its request path writes into `left->rightExpander.producerMessage` and flips `left->rightExpander.messageFlipRequested`. That may be intentional as a direct request-to-host path, but it bypasses the local producer pointer that the module owns.

Impact: this is fragile against Rack expander protocol expectations and future refactors. It makes ownership unclear: the right-side module writes into the left module's expander producer storage.

Recommendation: document this contract explicitly or change the request direction to use TD.Scope-owned `leftExpander.producerMessage` if Rack's left-expander producer semantics allow it. Add a focused test/mock for bidirectional expander messaging ownership.

### P3: Sil Constructor Relies Heavily On `APP->engine`

Files:

- `src/Sil.cpp:906`
- `src/Sil.cpp:915`
- `src/Sil.cpp:931`
- `src/Sil.cpp:950`

The Sil constructor repeatedly reads `APP->engine->getSampleRate()`. This is usually available in Rack runtime, but constructors are easier to test and reuse if they avoid global engine access and defer sample-rate-specific setup to `onSampleRateChange()` or a safe fallback sample rate.

Impact: brittle construction in non-Rack test harnesses or future initialization order changes.

Recommendation: use a local fallback such as `44100.f` in the constructor, then let `onSampleRateChange()` do authoritative setup.

### P3: TD.Scope Menu Has A User-Facing Typo

File:

- `src/TDScopeWidget.cpp:234`

The menu label says `Inverted Verical` instead of `Inverted Vertical`.

Impact: cosmetic polish issue.

Recommendation: rename the label.

## Module Notes

### Integral Flux

Strengths:

- Clear separation of channel state/config/result structures.
- Timing caches reduce hot-path work.
- Preview state uses atomics for audio-to-UI data handoff.
- Patch compatibility is considered in parameter ordering and JSON.

Risks:

- Context menu settings are plain fields read in audio processing.
- Performance options are persisted but not thread-safe while toggled live.
- There are no direct tests for Integral Flux DSP, JSON, or UI preview behavior.

Recommended tests:

- Cycle/trigger edge behavior at high rate.
- Slew response for rising/falling and curve extremes.
- JSON round-trip for cycle latch and performance settings.
- Mix bus behavior when individual outs are connected vs unconnected.

### Proc

Strengths:

- Shares a well-structured function/slew architecture with Integral Flux.
- JSON backward compatibility for earlier cycle latch naming is present.
- Output channels and lights are straightforward.

Risks:

- Same non-atomic performance settings issue as Integral Flux.
- No dedicated tests for Proc behavior or persistence.
- Signal/gate BLEP settings can change while MinBLEP state is in use.

Recommended tests:

- Halt input semantics.
- EOR/EOC gate timing across trigger/cycle/slew modes.
- Amplitude parameter behavior in function mode.
- JSON round-trip for performance settings.

### Temporal Deck

Strengths:

- Strongest test coverage in the repo.
- Engine, transport, sample prep, expander preview, and virtual integration are separated and covered by unit tests.
- Sample lifecycle uses a worker thread and atomic handoff instead of decoding in `process()`.

Risks:

- Trace logging performs filesystem work from `process()`.
- Several UI-set options are plain fields read/applied in the audio path.
- Expander request direction/ownership should be documented and tested more directly.

Recommended tests:

- Threaded sample load cancellation/destruction stress.
- Expander detach/reattach while dragging TD.Scope.
- Sample-loop edge cases with external freeze/reverse gates.

### TD.Scope

Strengths:

- Snapshot handoff to UI uses double buffering and atomics.
- Rendering code has fallback paths and debug metrics.
- Drag interaction is carefully bounded for velocity and zoom.

Risks:

- Many display/debug settings are plain fields shared between process/UI/render paths.
- Expander request writes into the neighbor's expander storage, making ownership unclear.
- No dedicated TD.Scope runtime tests beyond Temporal Deck expander preview.

Recommended tests:

- Expander message validity under stale/missing host.
- Range/channel/color JSON round-trip.
- Drag request publishing cadence and detach cleanup.

### Crownstep

Strengths:

- Game rules are isolated and thoroughly tested for checkers/chess/othello basics.
- AI work is offloaded to a worker thread.
- Persistence coverage is good.

Risks:

- Some UI display reads bypass `sequenceMutex`.
- Many non-audio state fields are shared between UI and process; the current recursive mutex covers sequence history but not all module state.
- AI worker lifecycle should continue to be stress-tested around rapid game-mode changes and module deletion.

Recommended tests:

- Rapid game mode/player mode changes while AI search is active.
- Thread sanitizer run against Crownstep UI/process simulations if feasible.
- Persistence of random board layouts across all game modes.

### Bifurx

Strengths:

- Good filter/runtime tests with concrete DSP contract coverage.
- Preview/runtime parity is tested.
- Render mode persistence includes legacy migration handling.

Risks:

- Audio-affecting options are plain fields toggled from UI.
- Render settings are shared by multiple render components without a clear ownership boundary.
- OpenGL path is too large to be meaningfully covered by current CLI tests.

Recommended tests:

- JSON round-trip for all settings including render mode and legacy keys.
- Live toggling of soft limiting/self-osc settings under audio stress.
- Manual GL/NanoVG fallback smoke test in Rack.

### Wyrm

Strengths:

- Wave points use atomics and a versioned wavetable rebuild path.
- Polyphony handling is straightforward.
- JSON captures waveform, rocks, point count, modes, and selected shape.

Risks:

- Rock state is not protected like wave points.
- `lfoMode` and editor flags are plain fields shared with UI.
- No tests cover JSON, waveform editing, rock collisions, or polyphonic output.

Recommended tests:

- JSON round-trip for wave points and rocks.
- Polyphonic VOCT/FM/sync/fold behavior.
- Stress test editing points and rocks while audio processing runs.

### Sil

Strengths:

- Repair kernel has focused tests.
- DSP chain is structured into named stages with clear state blocks.
- Debug counters use atomics.

Risks:

- Repair latency reconfiguration can allocate in `process()`.
- Debug file capture uses mutex/file I/O in `process()`.
- UI spectrum/histogram reads arrays written by audio without a snapshot protocol.
- Constructor relies heavily on `APP->engine`.

Recommended tests:

- Mastering/repair bypass latency and toggling behavior.
- Sample-rate changes and buffer sizing.
- Snapshot-safe spectrum/histogram rendering if refactored.

## Cross-Cutting Recommendations

1. Establish a thread-ownership rule for every module field. Audio-owned fields should be atomic, parameter-derived, or updated only on the audio thread. UI-owned fields should not be read from `process()`.

2. Replace direct `createBoolPtrMenuItem()` usage for audio-affecting settings with wrappers that write atomics or enqueue setting-change commands.

3. Move all file I/O, network, and potentially blocking debug output out of `process()`.

4. Add a small realtime-safety checklist to code review: no allocation, filesystem, locks, logging, or string construction in `process()` unless proven bounded and non-blocking.

5. Add ThreadSanitizer-compatible hostless tests where possible. Even if Rack itself is not in the harness, module-owned UI/audio simulations can catch many current races.

6. Expand module-specific tests for Integral Flux, Proc, Wyrm, TD.Scope persistence, and Sil bypass/latency. These are the current coverage gaps relative to Temporal Deck, Crownstep, and Bifurx.

## Final Assessment

The codebase is feature-rich and not in a broken state; the included tests pass. The highest-value hardening work is not more DSP tuning but thread ownership cleanup and realtime-safety cleanup. Addressing Wyrm rock-state handoff, replacing plain cross-thread settings, and removing file/allocation work from audio callbacks would materially improve Rack stability across all eight modules.
