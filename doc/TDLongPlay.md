# TemporalDeck LongPlay (TDLongPlay) Implementation Specification

**Date:** 2026-08-06  
**Status:** Implemented on `TDLongPlay`; hardening required before merge
**Target Module:** `TemporalDeck`
**Target File:** `doc/TDLongPlay.md`

> Current correctness, compatibility, performance work, and merge gates are
> tracked in [`TDLongPlay-Hardening.md`](TDLongPlay-Hardening.md).

---

## 1. Executive Summary & Background

`TemporalDeck` is a stereo buffer performance deck for VCV Rack, combining live circular recording, platter scratching, freeze/reverse/slip transport, and sample playback. Currently, sample mode pre-allocates contiguous RAM arrays for fixed buffer duration modes (up to 10 minutes stereo/mono).

At 48 kHz / 32-bit floating point stereo:
- **10 Minutes:** $\sim 57.6 \text{M frames} \times 2 \text{ ch} \times 4 \text{ bytes} \approx 460 \text{ MB}$ of RAM.
- **60 Minutes (1 Hour):** $\sim 345.6 \text{M frames} \times 2 \text{ ch} \times 4 \text{ bytes} \approx 2.76 \text{ GB}$ of RAM per instance.

Keeping full hour-long audio files uncompressed in contiguous RAM is unviable in modular DAW environments where multiple deck instances may co-exist. However, scratching, bi-directional scrubbing, high-speed reverse playback, and instant CV/UI seeking are core requirements of `TemporalDeck`.

This specification details **TDLongPlay**: a hybrid, disk-backed streaming and hot-window RAM caching architecture for `TemporalDeck`. TDLongPlay expands sample support to **60+ minutes** while maintaining real-time lock-free scratching and high-fidelity interpolation, with a bounded RAM footprint of **$\le$ 32 MB**.

---

## 2. Architectural Design Goals

1. **Bounded Memory Footprint:** Keep total RAM usage for hour-long files below 32 MB per deck instance, regardless of audio file duration (10 min to 3+ hours).
2. **Real-Time Scratch Responsiveness:** Provide zero-latency, sub-sample interpolated scratching within an active bi-directional "Hot Window" around the playhead.
3. **Bi-Directional Pre-fetching:** Dynamically cache audio both **ahead** (forward playback) and **behind** (reverse playback / backspinning / scratch repeats) the active playhead.
4. **Graceful Large Seek Handling:** Allow instant jumps across the entire timeline. If a seek target falls outside the current Hot Window, tolerate a brief physical "record stop / spin-down" audio fade while disk reads catch up, minimizing dropouts and re-accelerating smoothly back into scratch/playback.
5. **Zero Audio-Thread Allocations or Locks:** The audio processing loop (`TemporalDeck::process`) must remain 100% lock-free, reading exclusively from pre-allocated atomic sequence-validated RAM blocks.
6. **Seamless Scope Visualization:** Render directly from the hot RAM blocks. Dynamic range auto-scaling uses the peaks of the resident blocks rather than a full-file calculation, providing an auto-adapting view of the current local timeline context without background file scanning overhead.

---

## 3. High-Level System Architecture

```
                       +-----------------------------------+
                       |         Audio File (Disk)         |
                       |  (WAV / FLAC / MP3 - 1+ Hours)    |
                       +-----------------------------------+
                                         |
                                         v  Background Worker Thread
                       +-----------------------------------+
                       |       LongPlayStreamEngine        |
                       | - Seek & decode on demand         |
                       | - Bi-directional block loader    |
                       +-----------------------------------+
                                         |
                                         v  Lock-Free Atomic Swap
                       +-----------------------------------+
                       |       HotWindowBuffer (RAM)       |
                       |  - Ring of N Blocks (~16-32 MB)   |
                       |  - Symmetric 50/50 RAM Window      |
                       +-----------------------------------+
                                         |
                                         v  Lock-Free Atomic Reader
+-------------------+  CV / Platter Gate  +-----------------------------------+
| Platter Touch /   | -----------------> |       TemporalDeckEngine          |
| Transport Control |                    | - Real-time scratch interpolation |
+-------------------+                    | - Vinyl stop/spin-up state machine|
                                         +-----------------------------------+
                                                         |
                                                         v
                                                 Stereo Audio Out
```

---

## 4. Hot Window Buffer & Memory Model

### 4.1 Block-Based Cache Structure
Instead of a single contiguous float array, the hot RAM buffer is divided into a ring of fixed-size blocks managed by atomic sequence markers (extending the lock-free reader model from `longplayer::Stream`):

- **Block Size ($K_{\text{block}}$):** $65,536 \text{ frames}$ ($\approx 1.36 \text{ seconds}$ at $48 \text{ kHz}$).
- **Block Count ($N_{\text{blocks}}$):** $32 \text{ blocks}$ ($\approx 43.7 \text{ seconds}$ total cached audio).
- **RAM Footprint:** $32 \text{ blocks} \times 65,536 \text{ frames} \times 2 \text{ ch} \times 4 \text{ bytes} \approx 16.7 \text{ MB}$.

Each block structure contains:
```cpp
struct LongPlayBlock {
    std::vector<float> stereoData;       // 65,536 * 2 floats
    std::atomic<uint64_t> sequence{0};   // Even = valid, Odd = write in progress
    mutable std::atomic<uint32_t> readers{0};
    uint64_t startFrame = 0;
    uint32_t validFrames = 0;
};
```

### 4.2 Symmetric 50/50 Bi-Directional Buffer Policy
To guarantee instant responsiveness whenever the user engages platter scratching, backspinning, or cue repeats, the Hot Window RAM cache maintains a **symmetric 50/50 split** centered on the active playhead/scratch anchor:

- **Forward Window (50%):** 16 blocks ($\approx 21.8 \text{ seconds}$) including the playhead block and extending ahead for linear playback and forward scrubbing.
- **Backward Window (50%):** 16 blocks ($\approx 21.8 \text{ seconds}$) behind the playhead for instant backspins, reverse motion, and phrase repeats.

```
       [  16 Blocks Backward (21.8s)  |  16 Blocks Forward (21.8s)  ]
   <----------------------------------*---------------------------------->
                                  PLAYHEAD
```

By enforcing a permanent, symmetric 50/50 window centered on the playhead, the streaming engine ensures that scratch gestures in either direction are immediately serviced from RAM with zero latency and zero buffer re-allocation overhead.

When platter touch or active scratch motion occurs, the background worker thread elevates to **High-Priority Scratch Fetch mode** (1 ms sleep polling) to aggressively shift the 50/50 window as the scratch anchor travels across the timeline.

---

## 5. Transport Dynamics & Seamless Seek Protocol

### 5.1 Hot Window vs. Cold Media Seek Latency
When a position change request occurs (via CV position input, scope click, or UI scrub slider):

1. **Intra-Window Seek (Distance $\le$ Hot Window boundary):**
   - Target frame is already resident in one of the 32 RAM blocks.
   - **Latency:** 0 ms. Instantaneous position snap with zero audio interruption.
2. **Extra-Window Seek (Distance > Hot Window boundary):**
   - Target frame is NOT yet loaded in RAM (cold disk fetch required, $\approx 2 - 10 \text{ ms}$).
   - **Behavior:** The deck **continues seamless audio playback as-is from the current hot RAM buffer** (forward or reverse, according to current transport direction and rate) while the background worker thread performs the disk seek.
   - **Transition:** As soon as the target block is populated and validated by the worker thread (within $\sim 2 - 10 \text{ ms}$), the playhead smoothly transitions / crossfades into the new target position.

### 5.2 Background Seek & Smooth Transition State Flow

```
   +-------------------------------------------------------+
   |             NORMAL PLAYBACK / SCRATCHING              |
   +-------------------------------------------------------+
                               |
                               | Seek Request Outside Hot Window
                               v
   +-------------------------------------------------------+
   |            CONTINUOUS BUFFER PLAYBACK (2-10ms)         |
   | - Audio thread keeps playing current RAM buffer as-is |
   | - Direction (Forward/Reverse) & rate maintained       |
   | - Worker thread seeks physical media in background    |
   +-------------------------------------------------------+
                               |
                               | Target Block Validated
                               v
   +-------------------------------------------------------+
   |                SEAMLESS POSITION SNAP                 |
   | - Playhead snaps to new target position               |
   | - Micro 5ms crossfade smooths target transition       |
   | - Center 50/50 RAM window on new position            |
   +-------------------------------------------------------+
                               |
                               v
   +-------------------------------------------------------+
   |             NORMAL PLAYBACK / SCRATCHING              |
   +-------------------------------------------------------+
```

### 5.3 Benefits of Continuous Buffer Playback
- **Zero Audio Dropouts:** Because a disk read takes only $\sim 2 - 10 \text{ ms}$, reading from the existing 21.8-second RAM buffer during those few milliseconds guarantees 100% gapless, drop-free playback.
- **Rhythmic Continuity:** Music or scratch loops continue advancing without sudden silences or pitch dips during rapid CV position sequencing or scrub sliding.
- **No Complex Mute Circuits:** Eliminates artificial spin-down/spin-up muting logic while preserving clean micro-crossfaded position snaps once the target block is ready.

---

## 6. Scope Integration (`TDScope`) & Dynamic Range

To support visualization on `TDScope` without full-file RAM allocation or continuous disk access during render cycles:

### 6.1 Resident Hot-Block Scaling
Instead of computing the absolute dynamic range across a 1+ hour file, the engine scans only the **resident 32 RAM blocks** ($\approx 43.7 \text{ seconds}$) to determine the current dynamic peak.
- As the playhead moves, the dynamic range scaling dynamically "breathes" to fit the loudest section within the current 43-second hot window.
- This provides the performer with optimal waveform resolution for the immediate scratch area, rather than squashing local details if a louder peak occurs an hour away.

### 6.2 Scope Rendering Pipeline
- **Data Source:** Rendered directly from the hot RAM blocks.
- **Disk Access during Scope Draw:** Strictly **0 bytes**.

---

## 7. API and Code Structure Modifications

### 7.1 Buffer Duration Mode Expansion
In `src/TemporalDeckEngine.hpp`:
```cpp
enum BufferDurationMode {
    BUFFER_DURATION_10S,
    BUFFER_DURATION_20S,
    BUFFER_DURATION_10MIN_STEREO,
    BUFFER_DURATION_10MIN_MONO,
    BUFFER_DURATION_1MIN_STEREO,
    BUFFER_DURATION_2MIN_STEREO,
    BUFFER_DURATION_LONGPLAY_DISK, // New: 1+ Hour Disk-Backed Mode
    BUFFER_DURATION_COUNT
};
```

### 7.2 Core Classes & File Responsibilities

1. **`src/LongPlayStreamEngine.hpp / .cpp`** (Extends `longplayer::Stream`):
   - Handles multi-format decoder instances (`dr_flac`, `dr_mp3`, PCM WAV).
   - Manages background thread requests, seek requests, and bi-directional block decoding into `LongPlayBlock` structures.
2. **`src/TemporalDeckEngine.hpp / .cpp`**:
   - Implements the continuous buffer playback and seamless micro-crossfade position snap transition.
   - Reads samples from `LongPlayBlock` using existing sub-sample interpolators (`SCRATCH_INTERP_CUBIC`, `SCRATCH_INTERP_LAGRANGE6`, `SCRATCH_INTERP_SINC`).
3. **`src/TemporalDeckSampleLifecycle.hpp / .cpp`**:
   - Integrates `LongPlayStreamEngine` into async loading pipelines.
   - Handles seamless transition between live recording buffer modes and disk-backed sample modes.

---

## 8. Verification & Testing Strategy

To ensure reliability, real-time safety, and regression prevention, test coverage will be implemented in `tests/temporaldeck_longplay_spec.cpp`:

1. **Block Ring & Reader Concurrency:** Test zero-lock atomic block validation under high-frequency reader/writer contention.
2. **Bi-Directional Pre-Fetch Correctness:** Verify proper block loading for forward, reverse, and rapid scratch reversal states.
3. **Seamless Seek Transition:** Validate smooth micro-crossfade position transitions during extra-window seeks.
4. **Memory Boundary Verification:** Ensure total allocation stays below 32 MB for long duration files (1 to 3 hours).

---

## 9. Implementation Roadmap

- [ ] **Phase 1: Stream Engine Adaptation** – Implement a bi-directional block pre-fetcher with a 50/50 symmetric RAM window.
- [ ] **Phase 2: Dynamic Range Tracking** – Track peak amplitude across resident blocks for localized scaling.
- [ ] **Phase 3: Engine State Machine & Interpolation Integration** – Add `BUFFER_DURATION_LONGPLAY_DISK` to `TemporalDeckEngine` and wire lock-free block reads to cubic/sinc interpolators.
- [ ] **Phase 4: Seamless Seek Protocol** – Implement continuous RAM buffer playback and micro-crossfade position snaps on cold seeks.
- [ ] **Phase 5: UI & Scope Integration** – Wire `TDScope` to render and scale directly from the hot RAM blocks.
- [ ] **Phase 6: Verification Suite** – Add comprehensive unit tests in `tests/temporaldeck_longplay_spec.cpp`.
