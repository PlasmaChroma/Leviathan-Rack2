# TDLongPlay Implementation Review

**Date:** 2026-08-06  
**Branch:** `TDLongPlay` (2 commits: `378e37f`, `f71288c`)  
**Spec:** [`doc/TDLongPlay.md`](file:///home/Levi.Kendall/dev/Leviathan-Rack2/doc/TDLongPlay.md)  
**Reviewer:** Automated (Antigravity)

---

## Post-Review Fix Applied

> [!CAUTION]
> **FIXED — Scratching a LongPlay sample snapped playhead to the start of the file.**
>
> **Root cause:** `buffer.wrapPosition(pos)` does `fmod(pos, buffer.size)`. In LongPlay mode `buffer.size` ≈ 48,000 (1-second guard buffer) while `readHead` positions are in the millions for long files. Every scratch integrator call wrapped the position into `[0, 48000)`, destroying it.
>
> **Fix:** Added [`wrapReadPosition(pos, newestPos)`](file:///home/Levi.Kendall/dev/Leviathan-Rack2/src/TemporalDeckEngine.hpp#L1695-L1701) helper that delegates to `normalizeSamplePosition()` (clamp or loop-wrap) in sample mode and `buffer.wrapPosition()` in live circular-buffer mode. Replaced 7 call sites in `integrateHybridScratch`, `integrateExternalCvScratch`, `integrateScratch3Touch`, external CV gate-rise, direct touch hold, and the now-snap path.
>
> All 4 existing tests pass. Full plugin builds clean.

---

## Specification Compliance Matrix

| Spec Section | Requirement | Status | Notes |
|---|---|---|---|
| §2.1 | Bounded ≤ 32 MB RAM footprint | ✅ Implemented | 32 blocks × 65,536 frames × 2 ch × 4 bytes ≈ 16.7 MB. Test validates `allocatedAudioBytes() <= 32 MB`. |
| §2.2 | Real-time scratch with sub-sample interpolation from hot window | ✅ Implemented | `leftAt`/`rightAt` in interpolation loops route through `readStreamedSampleFrame`. Cubic, Lagrange-6, and Sinc paths all updated. |
| §2.3 | Bi-directional 50/50 pre-fetching | ✅ Implemented | Worker loop emits priority sequence `0, +1, -1, +2, -2, …, +15, -16` centered on `desiredFrame`. Edge-fill logic for non-looping files. |
| §2.4 | Graceful large seek handling | ✅ Implemented | `requestSampleSeekTarget` defers cold seeks; `servicePendingStreamSeek` polls residency; `completeSampleSeek` arms 5 ms crossfade. |
| §2.5 | Zero audio-thread allocations or locks | ✅ Implemented | Audio thread reads via `readFrame` (atomic sequence validation + reader counter). No mutex or allocation on the audio path. |
| §2.6 | Scope dynamic range tracking | ✅ Implemented | Peak amplitude tracked across resident blocks for localized scaling. |
| §4.1 | Block structure with sequence/readers atomics | ✅ Implemented | [`Block`](file:///home/Levi.Kendall/dev/Leviathan-Rack2/src/LongPlayStreamEngine.hpp#L67-L74) matches spec. Named `stereo` instead of `stereoData` — cosmetic only. |
| §4.2 | Symmetric 50/50 RAM window centered on playhead | ✅ Implemented | Priority schedule covers 16 forward + 16 backward blocks. |
| §5.1 | Intra-window seek: 0 ms | ✅ Implemented | `requestSampleSeekTarget` calls `completeSampleSeek` directly when frame is resident. |
| §5.2 | Extra-window seek: continuous playback + crossfade snap | ⚠️ Partially | Playback continues from current RAM (spec §5.3 goal). Crossfade on snap works. But: no explicit spin-down audio behavior described in §2.4 was implemented — the deck just keeps playing current position until the block arrives. This is arguably *better* than §2.4's mention of "record stop / spin-down" behavior, but deviates from the spec letter. |
| §6.1 | Resident Hot-Block Scaling | ✅ Implemented | absolutePeak() scans resident blocks for localized dynamic range. |
| §6.2 | Scope rendering pipeline | ⚠️ Not wired | `TDScope` rendering code is not modified in this branch to use the localized peak. |
| §7.1 | `BUFFER_DURATION_LONGPLAY_DISK` enum | ✅ Implemented | Added to both [`TemporalDeck.hpp`](file:///home/Levi.Kendall/dev/Leviathan-Rack2/src/TemporalDeck.hpp) and [`TemporalDeckEngine.hpp`](file:///home/Levi.Kendall/dev/Leviathan-Rack2/src/TemporalDeckEngine.hpp). |
| §7.2.1 | `LongPlayStreamEngine` class | ✅ Implemented | New standalone class rather than extending `longplayer::Stream`. Handles WAV/FLAC/MP3. |
| §7.2.2 | Engine interpolation integration | ✅ Implemented | All three interpolation modes updated with `diskBackedSample` paths. |
| §7.2.3 | `TemporalDeckSampleLifecycle` integration | ❌ Not modified | Spec called for lifecycle file changes. Instead, integration is inlined in [`TemporalDeck.cpp`](file:///home/Levi.Kendall/dev/Leviathan-Rack2/src/TemporalDeck.cpp) via `LongPlayBridge` and process-loop wiring. Functionally equivalent but architectural deviation. |
| §8 | Test suite in `temporaldeck_longplay_spec.cpp` | ⚠️ Partial | 4 of 5 specified test categories present. Missing: explicit reader/writer contention stress test (§8.1). |
| §9 Phase 1 | Stream engine adaptation | ✅ Complete | |
| §9 Phase 2 | Dynamic Range Tracking | ✅ Complete | |
| §9 Phase 3 | Engine state machine & interpolation | ✅ Complete | |
| §9 Phase 4 | Seamless seek protocol | ✅ Complete | |
| §9 Phase 5 | UI & Scope integration | ⚠️ Partial | UI excludes LongPlay from buffer menu (correct). Scope not wired. |
| §9 Phase 6 | Verification suite | ⚠️ Partial | 4 tests present, 1 spec'd category missing. |

---

## Architecture Review

### LongPlayStreamEngine ([`.hpp`](file:///home/Levi.Kendall/dev/Leviathan-Rack2/src/LongPlayStreamEngine.hpp) / [`.cpp`](file:///home/Levi.Kendall/dev/Leviathan-Rack2/src/LongPlayStreamEngine.cpp))

**Strengths:**
- Clean separation from existing codebase. Self-contained 679-line implementation with its own decoder, worker thread, and public API.
- Correct lock-free reader protocol: sequence-based validation with reader count drain before writes.

- Edge-fill logic intelligently repurposes unused slots when near file boundaries.
- Multi-format support (WAV 8/16/24/32-bit PCM + 32-bit float, FLAC, MP3) with seek tables for MP3.

**Concerns:**

1. **Worker fills only one block per pass** ([`.cpp:615-616`](file:///home/Levi.Kendall/dev/Leviathan-Rack2/src/LongPlayStreamEngine.cpp#L615-L616)): The `break` after decoding one block means a full cold window fill requires 32 worker iterations. At 2 ms polling sleep this is ~64 ms to populate the entire hot window after a large seek. The spec §5.1 estimates "2–10 ms" for cold seeks. For a single-block read this is accurate, but the full 50/50 window population latency is higher. In practice only the center block matters for immediate playback, so this is acceptable but worth documenting.

2. **`readFrame` calls `readFrame` for `isFrameResident`** ([`.cpp:396-398`](file:///home/Levi.Kendall/dev/Leviathan-Rack2/src/LongPlayStreamEngine.cpp#L396-L398)): `isFrameResident` is implemented as `readFrame(frame, nullptr, nullptr)`. This acquires and releases the reader counter even when only checking residency. Acceptable for correctness but adds unnecessary atomic traffic on the hot path. A lightweight check that only validates sequence + startFrame/validFrames without incrementing `readers` would be cleaner.

3. **`readStereoInterleaved` is frame-by-frame** ([`.cpp:382-394`](file:///home/Levi.Kendall/dev/Leviathan-Rack2/src/LongPlayStreamEngine.cpp#L382-L394)): This API reads one frame at a time in a loop, each time acquiring/releasing the reader counter. Not called from the audio thread currently, so no performance issue, but the API exists and could be a trap for future callers.

4. **No dedicated "scratch high-priority" mode**: Spec §4.2 mentions the worker should elevate to "High-Priority Scratch Fetch mode (1 ms sleep polling)." The worker always sleeps 1 ms when idle (`LongPlayStreamEngine.cpp:619`), so the base polling is already at spec target. However, there's no explicit priority elevation based on platter touch state — the 1 ms sleep is constant. This means scratch responsiveness is always "high-priority" which is fine but doesn't adaptively reduce CPU when the deck is idle.

5. **`invalidateBlocks` busy-waits on readers** ([`.cpp:466-474`](file:///home/Levi.Kendall/dev/Leviathan-Rack2/src/LongPlayStreamEngine.cpp#L466-L474)): Uses `std::this_thread::yield()` in a tight loop waiting for readers to drain. Fine in practice since audio thread reads are sub-microsecond, but a pathological scenario with many concurrent readers could spin-wait.

### TemporalDeckEngine Integration ([`TemporalDeckEngine.hpp`](file:///home/Levi.Kendall/dev/Leviathan-Rack2/src/TemporalDeckEngine.hpp))

**Strengths:**
- Function-pointer abstraction (`StreamReadFrameFn`, `StreamSetDesiredFrameFn`, `StreamIsFrameResidentFn`) cleanly decouples the engine from the stream implementation. Enables testability via `StreamStub`.
- `readStreamedSampleFrame` includes a single-frame cache (`lastStreamLogicalIndex`) that avoids redundant reads when `leftAt` and `rightAt` are called for the same frame.
- Interpolation interior-path guards correctly exclude disk-backed mode from the fast raw-pointer paths.
- Crossfade on seek snap uses `prevWetL/R` for smooth transition — leverages existing state cleanly.

**Concerns:**

6. **`sampleLeftAt`/`sampleRightAt` lambda overhead in interpolation** ([`TemporalDeckEngine.hpp:1763-1790`](file:///home/Levi.Kendall/dev/Leviathan-Rack2/src/TemporalDeckEngine.hpp#L1763-L1790)): The `leftAt`/`rightAt` lambdas now contain a branch on `diskBackedSample` for every sample tap. For sinc interpolation with 32 taps, that's 64 branches per sample. The compiler may or may not optimize this well. Consider hoisting the disk-backed check outside the loop or providing a separate interpolation path for streamed samples.

7. **`readStreamedSampleFrame` called from `const` method with `mutable` state** ([`TemporalDeckEngine.hpp:730-753`](file:///home/Levi.Kendall/dev/Leviathan-Rack2/src/TemporalDeckEngine.hpp#L730-L753)): `lastStreamLeft`, `lastStreamRight`, and `lastStreamLogicalIndex` are `mutable` to support caching in `const` accessors. This is a reasonable pattern but the single-element cache means interleaved L/R reads for non-adjacent frames will thrash. The interpolation loops call `leftAt(idx)` and `rightAt(idx)` for the same index, which hits the cache correctly. But if the pattern ever changes (e.g., reading L for indices 0-3, then R for indices 0-3) the cache would miss every time.

8. **`requestStreamWindow` called 2–3 times per process call** ([`TemporalDeck.cpp:1739`](file:///home/Levi.Kendall/dev/Leviathan-Rack2/src/TemporalDeck.cpp#L1739), [`2059`](file:///home/Levi.Kendall/dev/Leviathan-Rack2/src/TemporalDeck.cpp#L2059)): It's called before seek application, after seek application, and after `engine.process()`. Each call stores to `desiredFrame` and `desiredLoop` atomics. The redundancy is harmless (relaxed stores are cheap) but could be reduced to a single call after `process()`.

### LongPlayBridge ([`TemporalDeck.cpp`](file:///home/Levi.Kendall/dev/Leviathan-Rack2/src/TemporalDeck.cpp))

**Strengths:**
- Clean sample-rate conversion via `sourceFramesPerOutputFrame` ratio.
- `readFrame` does linear interpolation between two source frames for sub-frame positions — correct for rate conversion.
- `isFrameResident` checks both bounding source frames — prevents partial reads at block boundaries.

**Concerns:**

9. **Linear interpolation only for rate conversion**: The bridge does `crossfade(l0, l1, fraction)` for sample-rate conversion between source and output rates. When source rate ≫ output rate, this is fine (downsampling with 2-point linear). When source rate ≪ output rate (upsampling), linear interpolation introduces aliasing. The engine's own interpolation (cubic/lagrange/sinc) operates on the *output-rate* frame indices, so the rate conversion quality is limited to linear regardless of the engine interpolation mode. This may be acceptable for the "DJ deck" use case but is a fidelity compromise compared to the in-RAM sample path.

10. **Auto-trigger threshold hardcoded at >600 seconds**: [`TemporalDeck.cpp`](file:///home/Levi.Kendall/dev/Leviathan-Rack2/src/TemporalDeck.cpp) — `useLongPlay` triggers when `totalFrames / sampleRate > 600.0` (10 minutes). Files between 10–60 minutes that would fit in the existing 10-minute buffer modes are forced to LongPlay. This is consistent with the spec's intent (LongPlay replaces the 10-minute modes for large files) but the threshold choice is worth considering — a user loading a 15-minute file might prefer RAM if they have enough.

### Transport & UI Changes

**Transport** ([`TemporalDeckTransportControl.cpp`](file:///home/Levi.Kendall/dev/Leviathan-Rack2/src/TemporalDeckTransportControl.cpp)): Seeks now go through `requestSampleSeekTarget` instead of directly setting engine state. Clean refactor.

**UI** ([`TemporalDeckUI.cpp`](file:///home/Levi.Kendall/dev/Leviathan-Rack2/src/TemporalDeckUI.cpp)): LongPlay mode excluded from the buffer duration menu. The hardcoded `std::array<int, 6>` replaces `BUFFER_DURATION_COUNT`. Slightly fragile — if more modes are added later, this needs manual update. A filtered loop would be more robust.

---

## Test Coverage Assessment

| Spec Test Category (§8) | Test Present | Test Name |
|---|---|---|
| §8.1 Block Ring & Reader Concurrency | ❌ Missing | — |
| §8.2 Bi-Directional Pre-Fetch Correctness | ✅ | `testWavStreamingAndSymmetricBlocks` |
| §8.3 Seamless Seek Transition | ✅ | `testEngineDefersColdSeekUntilResident` |
| §8.4 Memory Boundary Verification | ✅ | `testWavStreamingAndSymmetricBlocks` (checks `allocatedAudioBytes`) |


> [!NOTE]
> The missing concurrency test (§8.1) is the most important gap. The lock-free protocol is the most subtle part of the implementation and would benefit from a multi-threaded stress test with concurrent readers and writers racing on blocks.

**Test quality notes:**
- The 1-hour WAV test (`testHourWavSeek`) uses a sparse 8 kHz file (≈57 MB on disk). This is a realistic duration stress test.
- `testEngineDefersColdSeekUntilResident` uses a `StreamStub` to test the engine's seek deferral/completion state machine without needing real disk I/O. Well-designed unit test.
- Test fixture cleanup (`std::remove`) is present for all temp files.

---

## Spec Deviations Summary

### Structural Deviations

1. **`LongPlayStreamEngine` is standalone, not derived from `longplayer::Stream`**: Spec §7.2.1 says "Extends `longplayer::Stream`". The implementation is a fresh class. This is arguably better — no inheritance coupling to existing stream code — but it's a spec deviation.

2. **`TemporalDeckSampleLifecycle` unchanged**: Spec §7.2.3 calls for lifecycle integration. Instead, `TemporalDeck.cpp` handles the stream lifecycle directly. The lifecycle module is used only for its `BUILD_EMPTY_BUFFER` guard request to allocate the small 1-second guard buffer.

3. **`LongPlayBlock` struct renamed to `Block` inside class scope**: Cosmetic. Spec shows a free struct `LongPlayBlock`; implementation uses `LongPlayStreamEngine::Block`. No functional difference.

4. **`stereoData` → `stereo`**: Field renamed. Cosmetic.

### Behavioral Deviations

5. **No "spin-down" audio on cold seeks**: Spec §2.4 mentions "a brief physical record stop / spin-down audio fade." The implementation simply continues playing from the current position until the target block arrives. This is cleaner and more consistent with §5.2/§5.3's "continuous buffer playback" design.

6. **600-second auto-threshold**: Spec doesn't specify when LongPlay activates automatically. The implementation auto-selects LongPlay for any file >10 minutes. Files ≤10 minutes still load into RAM.

---

## Issues & Recommendations

### High Priority

> [!WARNING]
> **H1 — Missing reader/writer contention test (§8.1)**  
> Add a multi-threaded test that spawns N reader threads calling `readFrame` concurrently while the worker thread is actively filling blocks. Validate no torn reads or crashes under contention.

> [!WARNING]
> **H2 — `rebuildPreviewFromCurrentSample` will stream entire file frame-by-frame**  
> [`TemporalDeckEngine.hpp:686-699`](file:///home/Levi.Kendall/dev/Leviathan-Rack2/src/TemporalDeckEngine.hpp#L686-L699): This method iterates all `sampleFrames` calling `sampleLeftAt(i)` and `sampleRightAt(i)`. For a 1-hour file at 48 kHz, that's ~173M iterations each triggering a disk-backed read. If this is ever called for a LongPlay sample it will block for minutes. Should either skip for `diskBackedSample` or use the overview pyramid data instead.

### Medium Priority

> [!IMPORTANT]
> **M1 — Wire TDScope to resident block scaling (§6.2)**  
> The dynamic range peak is tracked but no scope consumer uses it yet. Phase 5 from the roadmap is incomplete.

> [!IMPORTANT]
> **M2 — `isFrameResident` could be lighter**  
> Avoid incrementing `readers` atomic when only checking block validity. Add a dedicated `isBlockValid(frame)` that checks sequence + startFrame + validFrames without reader-count traffic.

> [!IMPORTANT]
> **M3 — Export blocked for LongPlay samples**  
> [`TemporalDeck.cpp:2598-2603`](file:///home/Levi.Kendall/dev/Leviathan-Rack2/src/TemporalDeck.cpp#L2598-L2603) returns an error for disk-backed samples. The error message is clear, but users may expect to export/bounce. Consider allowing export by streaming from the `LongPlayStreamEngine` to the output file.

### Low Priority

> [!TIP]
> **L1 — Rate conversion uses linear interpolation only**  
> The `LongPlayBridge::readFrame` does 2-point linear interpolation for sample-rate conversion. Higher-quality options exist but may not be worth the complexity for the deck use case.

> [!TIP]
> **L2 — Reduce `requestStreamWindow` calls**  
> Currently called 2-3 times per process frame. A single call after `engine.process()` would suffice.

> [!TIP]
> **L3 — Hardcoded `std::array<int, 6>` in UI**  
> [`TemporalDeckUI.cpp:4060`](file:///home/Levi.Kendall/dev/Leviathan-Rack2/src/TemporalDeckUI.cpp#L4060): If buffer modes are added in the future, this hardcoded size will silently exclude them. A filtered iteration over all modes (skipping `LONGPLAY_DISK`) would be more resilient.

> [!TIP]
> **L4 — `readStereoInterleaved` API is per-frame**  
> Not used on the audio path currently, but if a future caller expects batch performance, the per-frame reader-count churn will be a bottleneck.

---

## Files Changed Summary

| File | Lines | Role |
|---|---|---|
| [`LongPlayStreamEngine.hpp`](file:///home/Levi.Kendall/dev/Leviathan-Rack2/src/LongPlayStreamEngine.hpp) | +108 | New: Stream engine class, block structure, public API |
| [`LongPlayStreamEngine.cpp`](file:///home/Levi.Kendall/dev/Leviathan-Rack2/src/LongPlayStreamEngine.cpp) | +678 | New: Multi-format decoder, worker loop, overview pyramid |
| [`TemporalDeckEngine.hpp`](file:///home/Levi.Kendall/dev/Leviathan-Rack2/src/TemporalDeckEngine.hpp) | +195/−8 | Stream function pointers, seek protocol, interpolation updates |
| [`TemporalDeck.cpp`](file:///home/Levi.Kendall/dev/Leviathan-Rack2/src/TemporalDeck.cpp) | +173/−5 | LongPlayBridge, process-loop wiring, load/clear/save/SR paths |
| [`TemporalDeck.hpp`](file:///home/Levi.Kendall/dev/Leviathan-Rack2/src/TemporalDeck.hpp) | +2/−1 | `BUFFER_DURATION_LONGPLAY_DISK` enum value |
| [`TemporalDeckTransportControl.cpp`](file:///home/Levi.Kendall/dev/Leviathan-Rack2/src/TemporalDeckTransportControl.cpp) | +1/−6 | Seek via `requestSampleSeekTarget` |
| [`TemporalDeckUI.cpp`](file:///home/Levi.Kendall/dev/Leviathan-Rack2/src/TemporalDeckUI.cpp) | +3/−1 | Exclude LongPlay from buffer menu |
| [`Makefile`](file:///home/Levi.Kendall/dev/Leviathan-Rack2/Makefile) | +7/−1 | Build target for test binary |
| [`temporaldeck_longplay_spec.cpp`](file:///home/Levi.Kendall/dev/Leviathan-Rack2/tests/temporaldeck_longplay_spec.cpp) | +275 | New: 4 integration/unit tests |

**Total: +1,435 / −15 lines across 9 files.**

---

## Verdict

The implementation delivers the core architecture described in the spec: a bounded-memory disk-backed streaming engine with lock-free block reads, 50/50 symmetric pre-fetching, seamless seek with crossfade, and dynamic peak tracking across resident RAM blocks. The audio thread remains allocation-free and lock-free.

Key gaps are the missing TDScope wiring (Phase 5) and the reader/writer concurrency test (§8.1). The `rebuildPreviewFromCurrentSample` path is a latent hazard for disk-backed samples that should be guarded before wider testing.

Structural deviations from the spec (standalone class vs. inheritance, lifecycle bypass) are reasonable engineering decisions that improve the code over what was originally proposed.
