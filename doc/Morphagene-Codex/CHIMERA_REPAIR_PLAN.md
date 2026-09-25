# Chimera repair plan

Review date: 2026-09-25. This document tracks fixes from the stability,
performance, correctness, and audio-quality review. Status is updated only
after the relevant validation completes.

## First repair batch: ownership and bypass

- [x] **P1 — Protect snapshot leases during reel replacement.** An automatic
  pre-record snapshot can start before the control dispatcher receives its
  notification. A completed import can then replace and retire the leased
  reel. Fence adoption on the audio side as well as control-side admission;
  validate snapshot commands against their store identity. Test both a
  snapshot already started when a load completes and a snapshot starting
  after adoption was queued. Preserve deferred reclamation and RT bounds.
- [x] **P1 — Discard bridged events across bypass.** At 96 kHz, a queued REC
  command or REC gate raised during bypass starts recording after resume.
  Reset delayed audio, control, event, and resampler state and seed gate
  hysteresis without synthetic edges. Test stale REC, held gates, fresh
  post-resume edges, audio/CV/EOSG, rate changes during bypass, and callback
  allocation/deallocation behavior.

## Remaining repairs and design decisions

- [x] **P2 — Bandlimit offline WAV conversion.** The reviewed 96-to-48 kHz
  conversion aliases a 30 kHz sine to 18 kHz at full strength. Replace linear
  interpolation with an offline bandlimited resampler. Preserve duration,
  truncation, cue mapping, stereo alignment, and nonfinite handling. Add
  passband and rejection tests for 44.1, 96, and 192 kHz sources.
- [ ] **P2 — Bound edit checkpoint retention (runtime fix complete).** Every edit/undo/redo wrote
  another WAV (about 66.8 MB at full capacity), while old history paths are
  discarded without deleting their files. Remove unreferenced files off
  audio after successful handoff, cover failed/stale jobs, and define a
  bounded abandoned-session cleanup policy. Never delete a leased checkpoint.
- [ ] **P2 — Choose playback anti-aliasing policy.** Four-tap cubic reading
  aliases a 15 kHz source at 2x into a full-strength 18 kHz tone. Compare a
  rate-dependent bandlimited reader with prefiltered levels, including live
  recording, reverse/chord playback, splice boundaries, and PM. Benchmark
  CPU/memory before changing the DSP profile or default sound.
- [x] **Robustness — Bound energy calculations.** A finite 1e30 impulse
  overflows energy accumulation and leaves CV near 8 V after ten seconds of
  silence. Bound power inputs and recover nonfinite detector state; test PM
  detection and normal-level behavior.
- [ ] **Integration — Audit closed-window/headless dispatch.** Rate bridge
  preparation is independent of widgets, but normal reel preparation,
  completion polling, and recovery rely on widget/control calls. Verify
  Rack Pro window close/reopen and hosts that stop widget stepping.
- [x] **Performance — Profile shortest grains and modulation.** Measure
  grain-launch trigonometry, repeated gene log/exp mapping, modulated rate
  pow/exp2, and cubic-tap remainder operations. Cache or approximate where
  measured benefit justifies it; correct the no-transcendentals comment.
- [x] **Design — Separate marker-only edits from full audio replacement.**
  Moving/removing a marker previously checkpointed and copied all audio and
  resets playback. Evaluate a bounded metadata handoff for seamless editing.

## Baseline evidence

The native optimized phase-6 callback benchmark (full reel, high Morph,
roughly 480-frame genes, PM, Current recording, COW snapshot) measured:

| Host rate | One core used | 10 ms block p99 |
| --- | ---: | ---: |
| 48 kHz | 2.098% | 0.2642 ms |
| 96 kHz | 5.186% | 0.7978 ms |
| 192 kHz | 9.321% | 1.2152 ms |

These single-machine callback figures exclude Rack scheduling, UI and disk
work. Full-reel commit/reload measured approximately 0.64/0.48 seconds.

Review validation: native `plugin.dll` was up to date; all invoked Chimera
phase 0–4, clock, bridge, WAV, edit, bundle, recovery, waveform, panel,
full-save, module and patch tests passed. Broad `test-fast` failed in the
Sibyl P5 companion and P6 combined-source compilation fixtures. Those are
separate from these repairs. The review did not run sanitizers, listening
tests or live DAW graphics/window lifecycle tests.

## Repair validation record

First batch completed 2026-09-25:

- `Chimera::coreCommands()` fences every reel adoption with the snapshot
  claim and rejects adoption while the old reel is capturing, leased, or
  reclaiming. The control dispatcher defers completed loads while a snapshot
  is claimed, even before notification arrives. Capture/release commands
  validate the reel handle, and release validates the snapshot request ID.
  A snapshot starting after an adoption was queued safely rejects that
  replacement with a retry message; it does not retire the leased store.
- Bridged bypass resume retires the old bridge through the existing worker
  path and adopts a fresh bridge. Its initial gate states preserve Schmitt
  hysteresis without generating edges. This deliberately includes a short
  silent preparation/priming interval rather than replaying delayed audio or
  REC commands. The 48 kHz direct path does not need a replacement bridge.
- An attempted in-place Speex reset failed the stale stereo input test
  against the installed Rack runtime. The final implementation uses fresh
  construction off audio and does not rely on that reset API.
- New module regressions cover unannounced snapshots, late snapshot claims
  in all capture/read/reclaim phases and all adoption kinds, safe adoption
  after release, queued REC, held REC, fresh REC edges, stale stereo audio,
  and a sample-rate change across bypass. Resume works without widget
  stepping. C++ allocation/deallocation traps remain clear in callbacks.
- Native `test-chimera-module`, `test-chimera-rate-bridge`, and
  `test-chimera-patch`: **PASS**. Bridge gate/priming coverage includes 8,
  44.1, 88.2, 96, 176.4, 192, and 768 kHz. Native `test-chimera-phase2`
  (ownership, concurrent snapshots, jobs, service lifecycle): **PASS**.
- Native MINGW64 `make -j4 plugin.dll`: **PASS**, rebuilt and linked the
  Windows plugin. No installation was performed. `git diff --check` passes.
- No sanitizer or live DAW/window smoke test was run in this batch. The
  known unrelated broad-suite Sibyl failure remains recorded above; the
  broad suite was not rerun for this focused repair.

Second batch completed 2026-09-25:

- Offline convenience import uses the Rack-provided Speex quality-10
  bandlimited resampler in bounded streaming blocks. Rational-period endpoint
  extension preserves clip alignment and tiny constant clips. Native 48 kHz
  input bypasses filtering. Cue mapping, duration rounding and truncation
  remain unchanged. Float64 values outside float range are sanitized before
  casting; resampler input is bounded to prevent internal overflow.
- Spectral regressions cover 1/18 kHz passband and 30 kHz rejection for
  applicable 44.1/96/192 kHz sources. Interior passband RMS/time-domain error
  is below 0.002 and rejected ultrasonic RMS below 0.0001. Additional tests
  cover zero/one/seven-frame clips, 8 kHz through 383999 Hz rational rates,
  stereo polarity, float64 overflow and the existing format/cue matrix.
- Energy/PM inputs and live conditioning are bounded before dangerous
  arithmetic. Detector state recovers if nonfinite. Impulses of positive and
  negative 1e30 and FLT_MAX followed by ten seconds of silence leave finite
  outputs, near-zero CV, and functioning PM detection. This test also passes
  with additional `-O3 -march=nehalem -funsafe-math-optimizations` coverage
  (the production Chimera module explicitly disables unsafe math).
- Edit checkpoints now have explicit per-instance ownership. Superseded,
  failed and rejected-edit files are removed on the control side, after
  restore workers release them. Undo/Redo and pending adoption files remain
  protected. Normal destruction cleans owned files after workers finish;
  failed deletion is retried and blocks further edits at three retained paths.
  Tests cover a file leased by Undo and deletion after rejected adoption.
- Marker move/remove commands now change only a bounded marker table under
  core ownership, with revision and recording checks. They preserve audio
  allocation, revisions, voices and cursor; splice-boundary changes take
  effect at the existing natural handoff. One-step Undo/Redo swaps metadata,
  preserving marker IDs. Intervening document changes invalidate that history.
  Successful metadata edits replace the previous edit history. Save waits for
  pending metadata acknowledgment. Tests cover snapshot immutability, selected
  marker removal, Undo/Redo, stale requests and a recording/enqueue race. The
  Rack patch fixture also verifies immediate stopped-engine Save drains a
  pending marker edit and persists the changed table without audio callbacks.
- DSP work caches gene length and base rate mapping, interpolates pan from a
  constructor-built table, and avoids modulo for cubic taps inside a splice.
  Exact rate mapping remains unchanged; continuously varying gene/pitch still
  computes the required mappings. Native core benchmark (full reel, PM,
  Current recording and COW) results:

  | Scenario | Before: one core | After: one core | Median 10 ms block, before → after |
  | --- | ---: | ---: | ---: |
  | 16-frame genes, modulated pitch | 2.193–2.274% | 1.471–1.502% | ~0.205 → ~0.141 ms |
  | 480-frame genes, fixed rate | 1.560–1.629% | 1.034–1.372% | ~0.150 → ~0.103 ms |

  These are single-machine core measurements, excluding bridge/UI/host work.
  One run had a 3.776 ms scheduling outlier; these are not worst-case deadlines.
  The benchmark accepts gene frames and an optional modulation argument.
- Native phase 1–4, WAV, edit, bundle, recovery, bridge, module and patch
  tests pass. This includes 224 profile vectors and callback C++ allocation/
  deallocation checks. The Windows `plugin.dll` build/link passes. No plugin
  installation, staging or commit was performed. No new live listening,
  DAW-window or sanitizer run was performed.

## Work still open

### Abandoned checkpoint policy

Runtime retention is repaired, but files left by crashes or older versions
are not swept automatically. Their legacy directories identify a module ID,
not a live process lease; another Rack instance can use the same namespace.
Deleting by age or module ID alone could remove another instance's Undo.
The safe next implementation is a versioned, uniquely named session directory
with a cross-process exclusive lease. A bounded startup sweep may delete only
recognized files in unlocked sessions, with an age grace period and a work
limit. Keep legacy files untouched unless explicitly selected for cleanup.
This remains unchecked until crash, overlapping-instance and failed-delete
tests pass. Runtime limits do not bound accumulation across repeated crashes.

### Playback filtering

Offline import filtering does not fix playback aliasing. The existing cubic
reader remains profile 1. A rate-dependent FIR must widen its source support
with playback increment; a nominal 16-tap unity filter grows toward 512 taps
at 32x, per voice/channel. Prefiltered octave levels instead approach one
additional source-sized payload (about 66.8 MB at full capacity), before COW
or prepared copies. Two active/prepared full-reserve Reels already account
for 267,264,000 raw bytes against the 256 MiB ledger. Adding levels without
redesigning that budget is not acceptable.

Next: benchmark a bounded multistage reader prototype, define how live writes
and splice-local filtering update derived data, and measure reverse/chord/PM
behavior before selecting a new profile or quality setting. Preserve the
current sound until that comparison and listening validation are complete.

### Closed-window control dispatch

Source audit confirms ordinary `serviceStep()` calls come from widget stepping
and synchronous non-audio operations (save/edit flows). Audio processing does
not poll normal reel jobs; the independent rate-preparation worker solves only
the bridge path. A host that ceases widget stepping can therefore delay normal
preparation, adoption, retirement and recovery until a control call resumes.
Actual Rack Pro close/reopen behavior remains unverified.

Do not call the existing dispatcher concurrently from a timer: its handles,
tickets, history and SPSC producer state assume a single control owner, and
persistence paths use host context. Next implementation must serialize all
control requests through a module-lifetime dispatcher, separate host-context
operations from worker-safe jobs, and drain on shutdown without audio locks.
Validate window-close/reopen, stopped-engine save, pending import/edit,
automatic recovery and removal while jobs are active in the actual host.
