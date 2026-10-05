# Validation plan and reverse-engineering backlog

## 1. Existing results

The archive manifest verifier matches all 184 extracted files. Parsed Pd graph endpoints are valid for all 5,727 connections. The restricted `calculateFollowSpeed` instruction-text probe passes all 12,288 integer-input/mode comparisons at tolerance `1e-5`. The native optimized and ASan/UBSan test runs each pass 107,421 checks, including those Follow fixture cases. These are not end-to-end audio or hardware results.

Files: `tables/follow_probe_results.json`, `tables/native_test_results.json`, `tables/native_sanitizer_test_results.json`, `tables/structural_validation.json`. Commands and limitations are in `reference/README.md`.

## 2. Oracle hierarchy

Use several independent types of reference, without conflating them:

| Oracle | Appropriate use | Limitation |
|---|---|---|
| Uploaded bytes and exact tables | Identity, constants, enums, graph structure | Presence does not prove an active normal path. |
| Inspected machine-code branch | Arithmetic and state transitions with proven inputs | Incomplete path coverage can misidentify a field or event order. |
| Restricted instruction-text execution | Small pure leaf routines, deterministic fixtures | Interpreter itself can be wrong; not a complete CPU. |
| Native invariants | Memory safety, finite outputs, expected algebra | Can pass even when the chosen model is unlike the original. |
| Original Pd process with appropriate environment | Scheduler, message ordering, block behavior | Requires dependencies and safely stubbed hardware/OS interactions. |
| Hardware recordings and control traces | User-visible behavior, analog transfer, actual timing | Measurement chain and device revision must be documented. |

This bundle has the first four, with restricted instruction-text execution for only one helper. It does not have the final two. A future emulation harness should start with leaf routines and substitute explicit Pd/OS/hardware stubs; never use the real privileged updater as a shortcut.

## 3. Minimum deterministic fixture format

Store sample rate, processing block size, firmware hash, preset/configuration, input signal hashes, event timestamp convention, initial buffer contents, random seed/generator identity, and actual output length. Log both requested and actual event times. For cross-process originals, record how clocks align.

Use a monotonic frame index and typed events rather than wall-clock sleeps. A JSON test case can reference small raw-float or WAV assets by hash. Capture source data and native outputs separately. Never overwrite golden fixtures automatically when a test fails.

## 4. Test matrix

| Area | Stimulus | Inspect |
|---|---|---|
| Grain interpolation | DC, ramp, single impulse, alternating sign; fractional read positions | DC preservation, exact neighborhood, forward/reverse boundary policy, non-finite reads. |
| Window shape | All 101 rows and endpoint textures; short/long grains | Endpoint zeros, shifted Gaussian behavior, square gain, phase alignment, reverse stepping. |
| Duration/Spray | Raw 0, 1, center-neighbors, 4094, 4095 plus MIDI additions | Square law, integer conversion, clamp order, physical versus internal minimum. |
| Scheduler | Fixed Length, swept Intensity, fixed random state | Rate curve, duration coupling, random timing distribution, missed/merged events. |
| Oversubscription | Bursts and sustained dense triggers | 82-slot indexing, count threshold, retirement duration, repeated reuse, MIDI interactions. |
| Capture | Impulses and ramps through starts/stops/retriggers | Two-writer handover, 256-sample fades, exact final write index, no assumed ring wrap. |
| Dub | Constant input/old-buffer values, steps in Dub | Write equation, slew, clipping placement, two-writer overlap. |
| Layer operations | Distinct tagged waveforms per layer | Record/play destination coupling, Omega boundaries, old-generation reader safety. |
| Follow | Every speed branch, loop changes, capture while following | Plateau positions, stop/reverse transitions, placement offset, record-head avoidance. |
| Quantisation | Every signed sequence entry and center departure | Duplicate weighting, pair order, detent fix, random draw sequence, pitch-CV latch time. |
| Onset | Isolated hits, close hit pairs, noise floor, stereo changes | Detector source, thresholds, holdoff, six profile actions, no unintended repeated capture. |
| Delay | Impulse + signed macro sweep + MIDI offsets | Actual delay milliseconds, dynamic multiplier, feedback sign/gain, event ordering. |
| Reverb | Impulse, Strike/onset during decay, routing changes | Primary graph response, duck/restore, no unproven tank clear, auxiliary activation. |
| Stereo/phase | Left-only/right-only/opposite-polarity signals | Mono summing, panning selection, phase policy, effect routing, overall gain. |
| WT mode | Pitch ramp, Length boundary crossings, empty/filled layers | 0.976642 factor, oscillator tables, entry hysteresis, transitions, clocked-mode inhibition. |
| Files | 624000 and 638924 frames, multiple rates, truncated/empty data | Conversion domain, tail handling, fallback behavior, atomic adoption. |
| Host stress | Multiple instances; 44.1/48/96/192 kHz; UI activity and imports | CPU spikes, no callback allocation or locks, deterministic timing, safe rate change. |

## 5. Metrics

Use frame-exact comparisons only when sample rate, random state, block ordering, and interpolation are equivalent. Otherwise report controlled metrics: launch/stop frame error, grain count, peak/RMS gain, spectral differences, reverb decay envelopes, delay-time estimates, and onset-trigger latency distributions. Clearly distinguish a musical tolerance from numerical equivalence.

For memory/race testing, use ASan/UBSan for scalar logic and a separate race-checking harness for worker/audio ownership. Instrument heap allocation and file/lock activity in the audio callback. No original hardware benchmark or Rack CPU percentage has been measured in this effort; do not invent one.

## 6. Prioritized backlog

`tables/open_questions.csv` and `.json` contain 14 identified issues with priority, entry points, and next actions. P0 means it materially blocks fidelity of the core instrument, not that a prototype cannot sound.

**First:** finish `play_next`/perform allocation and scheduler semantics. **Second:** finish recorder Dub/handover and capture boundaries. **Third:** resolve the delay macro's actual incoming domain and Pd message order. These are stronger next investments than expanding unrelated experimental features.

## 7. Reporting gate

Every new claim should include binary hash plus function/VA or patch/object path, its evidence grade, tested input range, and a known limitation. A small passing fixture cannot certify neighboring functions. A source symbol name cannot certify a signal path. A useful-sounding native approximation should be retained as a labelled option, not retroactively described as a decoded original algorithm.
