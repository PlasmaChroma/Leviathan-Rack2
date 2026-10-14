# TDLongPlay Hardening Implementation Plan

**Date:** 2026-08-06  
**Branch:** `TDLongPlay`  
**Status:** Required before merge/release  
**Scope:** Stabilize disk-backed sample playback without changing established Temporal Deck behavior

---

## 1. Purpose

TDLongPlay adds bounded-memory playback of samples longer than ten minutes. The core streaming architecture works, but review found several places where the new disk-backed representation is confused with existing Temporal Deck concepts. Those interactions can affect looping, sample replacement, scratching, TD.Scope, and real-time safety.

This document is the implementation checklist and merge gate for hardening the branch. It supersedes the optimistic completion verdict in `doc/TDLP-Review.md`; that file remains useful as historical context, but it does not cover the current branch state.

No work in this plan should casually redesign Temporal Deck. `Temporal Deck` and `TD.Scope` are released modules. Patch serialization, parameter/input/output/light ordering, transport behavior, and existing live/RAM sample behavior must remain compatible.

---

## 2. The Three Lengths Invariant

LongPlay introduces three different lengths. They must be named explicitly and never substituted for one another:

1. **Guard-buffer length**
   - Small in-memory compatibility buffer, currently approximately one second.
   - Used only where existing engine infrastructure requires allocated storage.
   - Must never define disk-sample positions, loop wrap, motion deltas, duration, or scope history.

2. **Physical file length**
   - Total decoded frames in the source file.
   - Defines the maximum possible LongPlay timeline.
   - Used for file bounds and non-looping cache edge behavior.

3. **Active sample-window length**
   - Physical file length reduced by Temporal Deck's Buffer control.
   - Defines the actual transport end, loop endpoint, accessible lag, scope wrapping, and sample-mode scratch topology.

Every position, lag, delta, cache request, and scope request added or changed by this work must document which length domain it uses.

---

## 3. Behavioral Invariants

The following behavior is non-negotiable:

- Existing live-buffer and RAM-sample behavior must remain unchanged.
- Existing parameter, input, output, and light IDs must not be reordered.
- Loading a LongPlay patch must preserve the saved Freeze intent.
- Audio must not wait for the graphics/UI thread.
- Startup audio, playhead, platter state, and TD.Scope must begin from the same resident source region.
- Looping must occur at the active Buffer-controlled endpoint, not necessarily at the physical end of the file.
- Changing sample files must never inherit the hidden guard-buffer mode as a user RAM-buffer selection.
- Audio-thread code must not allocate, lock, decode, perform file I/O, or execute unbounded work.
- A cold streamed read must have an explicit transition policy. Repeating an unrelated last sample is not an acceptable steady-state policy.
- Debug instrumentation introduced by this work must be gated by `isDragonKingDebugEnabled()`.

---

## 4. P0 Work — Required Correctness and Real-Time Safety

### 4.1 Make the cache aware of the active loop window

**Problem**

`LongPlayStreamEngine` currently receives a desired frame plus `bool loop`. When looping, its worker wraps block selection at the physical file length. Temporal Deck actually loops at the Buffer-controlled active sample-window endpoint. These differ whenever Buffer is below 100%.

**Implementation direction**

- Replace the boolean-only desired-window contract with an explicit POD request containing at least:
  - desired physical source frame;
  - loop enabled;
  - loop start frame;
  - exclusive or inclusive loop end frame, with the convention documented;
  - request generation/revision if needed for coherent publication.
- Publish the request lock-free from the audio thread.
- Convert the engine's output-rate active window into source-frame bounds in `LongPlayBridge`.
- Make worker block selection wrap inside those bounds.
- Near either loop boundary, prefetch both sides of the logical seam.
- Make startup scope-window residency use the same active bounds.
- Make reverse playback at loop start request blocks near the active loop end.

**Tests**

- Buffer at 25%, 50%, and 100% with looping enabled.
- Forward wrap from active end to frame zero.
- Reverse wrap from frame zero to active end.
- Startup at frame zero verifies that scope history comes from the active end.
- Active loop end intentionally placed far from the physical file end.
- Source rate different from Rack output rate.

**Acceptance**

No audio dropout, stale sample hold, wrong TD.Scope history, or cache request outside the active loop topology during these cases.

### 4.2 Separate persistent RAM-buffer choice from LongPlay storage state

**Problem**

`BUFFER_DURATION_LONGPLAY_DISK` is stored in `bufferDurationMode`. If a user replaces a LongPlay file with a file of ten minutes or less, RAM sample preparation reuses the hidden disk mode and truncates the new sample to approximately one second. Missing or changed restored files can produce the same invalid fallback.

**Implementation direction**

- Treat LongPlay as a runtime storage kind, not a valid RAM preparation choice.
- Retain the appended enum value for branch/patch compatibility if necessary, but introduce helpers such as:
  - `isRamBufferDurationMode(mode)`;
  - `sanitizeRamBufferDurationMode(mode, fallback)`;
  - `lastRamBufferDurationMode` or equivalent persistent state.
- A LongPlay install must not destroy the user's previous RAM/live buffer selection.
- Loading a short sample after LongPlay must use a valid RAM mode.
- Clearing LongPlay must return to a documented RAM/live mode, currently 10 seconds unless a preserved prior choice is intentionally selected.
- A missing LongPlay file must never leave Temporal Deck operating as a one-second live buffer because mode 6 leaked into ordinary preparation.
- `TemporalDeckSamplePrep` must reject or sanitize LongPlay mode rather than treating it as a one-second RAM sample mode.

**Tests**

- LongPlay file -> short stereo file.
- LongPlay file -> short mono file.
- LongPlay file -> live mode after Clear.
- Restored LongPlay patch with missing file.
- Restored LongPlay patch whose file was replaced by a short file.
- Sample-rate change while each state is active.

**Acceptance**

No ordinary sample is unexpectedly truncated to the guard-buffer length, and no hidden runtime mode appears as a user-selectable buffer range.

### 4.3 Repair the residency synchronization protocol

**Problem**

`isFrameResident()` reads non-atomic block metadata without joining the block's reader protocol while the worker writes that metadata. The sequence marker alone does not make concurrent non-atomic reads legal in C++.

**Implementation direction**

Choose one coherent protocol and use it everywhere:

- Preferred minimal change: make residency checks enter the same reader-count protocol as `readFrame()`, including sequence revalidation.
- Alternative: publish a fully atomic metadata snapshot with a proven seqlock pattern whose payload fields cannot participate in a C++ data race.
- Worker-side `present` checks may remain lightweight only where they are provably single-writer-thread reads.
- Document memory ordering beside the protocol.
- Avoid mutexes on audio-thread residency checks.

**Tests**

- Contention test must call `readFrame()` and `isFrameResident()` concurrently while the desired window thrashes.
- Repeated load/clear/load while readers are active.
- Run ThreadSanitizer on native Linux when available.
- Windows/MSYS2 should use any available supported race-analysis tooling, but normal Windows validation is still required even if no sanitizer is available.

**Acceptance**

No data races under a supported race detector, no invalid frame reads, and no deadlock or unbounded worker wait.

### 4.4 Bound TD.Scope work on the audio thread

**Problem**

Disk-backed scope cache reuse is currently disabled permanently. At the 90 Hz stereo publish ceiling, rebuilding the full scope can cause millions of atomic block reads per second on the audio thread.

**Implementation direction**

- Do not permanently disable scope-window reuse for disk samples.
- Add a cheap residency/cache generation published by the stream worker, or another bounded invalidation signal.
- During startup/cold movement:
  - build only from resident data;
  - refresh missing/new bins as residency changes;
  - avoid retaining fabricated bins.
- Once the visible window is resident:
  - reuse shifted bins;
  - evaluate only newly exposed bins;
  - invalidate on loop topology, channel mode, scale-relevant generation, seek, or source generation changes.
- Consider a worker-produced waveform envelope only if incremental audio-thread extraction cannot meet the budget. Do not introduce that larger design until measured evidence requires it.
- Add release-build timing instrumentation gated by Dragon King debug mode.

**Performance gate**

Define and measure a maximum per-publish scope extraction cost at 44.1, 48, 96, and 192 kHz, mono and stereo. The accepted threshold should be selected from real Rack measurements, but the test/instrumentation must make regressions visible.

**Acceptance**

No full streamed-window rebuild every publish once residency is stable, and no material audio-thread spike attributable to TD.Scope during ordinary playback.

---

## 5. P1 Work — Required Behavioral Hardening

### 5.1 Correct peak-voltage units

**Problem**

The resident file peak is correctly scaled to modular volts and then overwritten later in the same process call with the raw normalized file peak. TD.Scope receives the unscaled value.

**Implementation direction**

- Establish one API/unit contract: `absolutePeak()` returns normalized file amplitude or modular volts, never context-dependent values.
- Apply `kSampleFileVoltageScale` exactly once at the bridge/module boundary.
- Remove the duplicate conflicting assignment.
- Name variables with units (`normalizedPeak`, `peakVolts`).

**Tests**

- Known 0.5 and 0.75 normalized peaks publish 2.5 V and 3.75 V.
- Peak updates as the resident window moves.
- Mono and stereo scope modes.

### 5.2 Define cold scratch and cold scope-drag behavior

**Problem**

Explicit timeline seeks defer until resident, but platter movement, external Gate+Pos, and TD.Scope dragging can move the read head directly into cold blocks. Failed audio reads repeat the last successful sample.

**Implementation direction**

- Detect when the requested scratch interpolation neighborhood is not resident.
- Publish the desired cache anchor immediately.
- Use one explicit bounded transition policy, for example:
  - hold the last valid read position with a short fade;
  - then crossfade to the requested position when its interpolation neighborhood is resident.
- Do not allow scope-only probes to modify playback fallback state.
- Do not output indefinite held DC or advance a logical playhead through unavailable audio.
- Preserve scratch direction and gesture state across the handoff.

**Tests**

- Large platter jump outside the hot window.
- Large TD.Scope drag outside the hot window.
- External Gate+Pos jump outside the hot window.
- Cubic, Lagrange-6, and Sinc interpolation neighborhoods crossing block boundaries.
- Forward and reverse recovery, with loop on and off.

### 5.3 Use active sample-loop length for motion deltas

**Problem**

Read-head delta correction and platter motion still use the guard-buffer size in the general path, while the fast sample path uses an uncorrected raw delta. LongPlay loop wrap can therefore look like a huge reverse movement and can alter cartridge motion processing.

**Implementation direction**

- Add one helper for signed shortest-path sample read delta.
- In live mode it uses circular live-buffer size.
- In looping sample mode it uses active sample-loop length.
- In non-looping sample mode it uses direct bounded subtraction.
- Use it consistently for:
  - cartridge motion amount;
  - scratch transient direction;
  - platter phase/visual movement;
  - any velocity derived from `readHead - prevReadHead`.

**Tests**

- Forward and reverse loop seams produce approximately one-sample transport delta.
- No platter-angle jump at the seam.
- No one-frame cartridge-motion spike at the seam.
- RAM and disk-backed sample modes behave identically.

### 5.4 Make load replacement state deterministic

**Problem**

Loading a LongPlay file returns before applying the same reverse/slip reset behavior used by ordinary sample loading. Replacing one LongPlay file with another also leaves the old engine playing while its cache is invalidated.

**Implementation direction**

- Centralize common sample-load transition policy before choosing RAM or disk storage.
- Preserve Freeze exactly as existing released behavior requires.
- Apply reverse/slip behavior consistently for RAM and LongPlay.
- During LongPlay replacement, internally hold the old transport before invalidating its source.
- Do not emit a held last sample while the new file opens.
- Apply legacy `sampleAutoPlayOnLoad` compatibility to streamed installs as well as prepared RAM installs.

**Tests**

- RAM -> LongPlay and LongPlay -> RAM.
- LongPlay A -> LongPlay B while playing, frozen, reversed, and slipping.
- Patch restore with legacy `sampleAutoPlayOnLoad=false`.
- Freeze gate and reverse gate connected during replacement.

---

## 6. P2 Work — Decoder, Loading, and Duration Robustness

### 6.1 Fix WAV validation and 24-bit decoding

- Decode 24-bit PCM from exactly three bytes; never call a four-byte reader.
- Reject unsupported PCM and float bit depths explicitly.
- Bound WAV chunk allocation and validate chunk offsets against file size.
- Add mono/stereo fixtures for 8-, 16-, 24-, and 32-bit PCM plus 32-bit float.
- Run AddressSanitizer/UndefinedBehaviorSanitizer on native Linux where supported.

### 6.2 Move expensive probing off the UI thread

- Do not perform MP3 frame counting and seek-table construction synchronously in `loadSampleFromPath()`.
- Make the worker determine metadata and storage qualification.
- Avoid opening and analyzing the same file twice.
- Patch loading and the file dialog callback should return promptly.
- Publish load failure and metadata coherently back to the module/UI.

### 6.3 Define maximum supported duration

`sampleFrames` is currently an `int`, limiting output-rate logical length to `INT_MAX` frames. This is approximately:

- 13.5 hours at 44.1 kHz;
- 12.4 hours at 48 kHz;
- 6.2 hours at 96 kHz;
- 3.1 hours at 192 kHz.

Choose and document one policy:

- migrate LongPlay logical positions/counts to 64-bit while preserving existing RAM/live types; or
- reject/explicitly truncate files above a documented duration.

Silent clamping with `sampleTruncated=false` is not acceptable.

---

## 7. Test Matrix

### Automated focused tests

Extend `tests/temporaldeck_longplay_spec.cpp` or add narrowly scoped suites for:

- active loop endpoint prefetch;
- loop seam playback and platter delta;
- LongPlay-to-RAM replacement mode sanitization;
- missing/restored file fallback;
- concurrent `readFrame` and `isFrameResident`;
- cold platter/scope/CV movement;
- peak unit conversion;
- decoder bit-depth coverage;
- source/output sample-rate mismatch;
- maximum-duration policy.

Add a Rack-linked lifecycle test if required to exercise `TemporalDeck::loadSampleFromPath()`, serialization, and asynchronous install ordering. Engine-only tests are insufficient for those behaviors.

### Existing regression gates

The following must remain green after every work package:

- `make test-fast`
- `build/tests/temporaldeck_engine_spec`
- `build/tests/temporaldeck_virtual_integration_spec`
- `build/tests/temporaldeck_expander_preview_spec`
- `build/tests/temporaldeck_longplay_spec`

`test-rack` remains out of scope per repository guidance.

### Manual Windows/MSYS2 Rack matrix

The Windows/MSYS2 build is authoritative for the final plugin. Test at minimum:

- WAV, FLAC, and MP3 files longer than ten minutes;
- 44.1, 48, 96, and 192 kHz Rack engine rates where practical;
- Buffer at 25%, 50%, and 100%;
- loop on/off, freeze on/off, reverse, slip, wheel, platter drag, TD.Scope drag, and Gate+Pos;
- patch save/reload while playing and frozen;
- sample replacement in both directions between RAM and LongPlay;
- TD.Scope mono/stereo and fixed/automatic range;
- Deep Cache active while loading and playing;
- DAW/editor close and reopen to exercise graphics lifecycle independently of audio streaming.

Record audible dropouts, scope waiting time, maximum scope extraction cost, worker fill latency, and resident-block churn using Dragon King debug telemetry.

---

## 8. Recommended Implementation Order

1. Add failing tests for hidden mode leakage and reduced-Buffer loop topology.
2. Separate RAM buffer mode from LongPlay storage state.
3. Expand the desired-window contract to include active loop bounds.
4. Add the shared sample read-delta helper and loop-seam tests.
5. Repair `isFrameResident()` synchronization and expand contention tests.
6. Bound and cache TD.Scope extraction; measure audio-thread cost.
7. Define and implement cold scratch/scope-drag handoff.
8. Correct peak units and load-transition consistency.
9. Harden WAV decoding and asynchronous metadata loading.
10. Run the complete automated and Windows/MSYS2 manual matrices.

Do not combine all steps into one broad refactor. Each step should be independently testable, with Temporal Deck's existing behavior revalidated before proceeding.

---

## 9. Merge Gates

Implementation status as of 2026-08-06: the source changes and focused Linux/WSL
tests have completed the checked items below. Unchecked items remain required;
in particular, this is not yet a merge declaration.

TDLongPlay is ready to merge only when all of the following are true:

- [x] Active loop endpoint is used consistently by transport, cache, scope, and startup residency.
- [x] LongPlay mode cannot leak into RAM/live sample preparation.
- [x] Residency checks use the same reader-count/sequence protocol as audio reads; the contention test exercises both readers. TSan runtime validation remains unavailable in this WSL environment.
- [ ] Stable disk-backed scope playback reuses cached bins and meets the measured audio-thread budget.
- [x] Cold read-head movements hold the previous resident position and use a 5 ms resident handoff; manual gesture coverage remains in the Rack matrix.
- [x] Peak voltage units are correct.
- [x] Loop seams use the active loop length and focused forward/reverse tests produce one-sample deltas.
- [ ] RAM and LongPlay load transitions share the intended Freeze/reverse/slip policy.
- [x] 24-bit WAV decoding reads exactly three bytes and unsupported PCM/float depths are rejected.
- [ ] Maximum supported duration is explicit.
- [x] All focused tests and `make test-fast` pass in WSL.
- [ ] Authoritative Windows/MSYS2 plugin build succeeds.
- [ ] Manual Rack matrix passes without audible or visual regressions.

---

## 10. Out of Scope

- Redesigning Temporal Deck's public controls or panel.
- Reordering released parameter/input/output/light enums.
- Persisting the playhead position unless separately specified.
- Making audio startup wait for TD.Scope or the graphics thread.
- Reintroducing a separate standalone long-file player module.
- Adding full-file waveform analysis unless bounded scope extraction proves insufficient after measurement.
- General cleanup unrelated to the LongPlay integration.
