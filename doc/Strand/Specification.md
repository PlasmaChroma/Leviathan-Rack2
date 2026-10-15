# Strand

Status: initial implementation, pending listening evaluation. Strand is intended to be an official Leviathan module. Target panel width: 3 HP.

## Purpose

Strand interleaves captured waveform pseudo-cycles from sources A and B into a single mono output C. The module plays one completed A pseudo-cycle, then one completed B pseudo-cycle, continuously selecting the most recent completed capture at each handoff. Both sources continue capturing throughout playback.

Strand is a pure mono module (2 inputs, 1 output). For stereo processing, two Strand modules can be used.

## Panel and routing

Three audio jacks are arranged vertically in 3 HP, with no controls required for the core operation:

| Source A | Source B | Output |
| --- | --- | --- |
| A | B | C |

- Interleave captured cycles from input A and input B into output C.
- When only one input is patched, repeat captures from that active source until the second source connects.
- Output connection state does not influence capture or playback.
- Inputs read channel 1 (monophonic); output produces a monophonic stream.

## Frequency statistics display

Below each port on the module panel, a real-time text readout displays frequency statistics:
- **Input A**: Average frequency of accepted pseudo-cycles over the past second.
- **Input B**: Average frequency of accepted pseudo-cycles over the past second.
- **Output C**: Combined average frequency over the past second. When both inputs are active, shows `(freqA + freqB) / 2`. When only one input is active, displays that active input's frequency.

Statistics are tracked over a rolling 1-second window partitioned into 20 sub-buckets (50 ms each) updated deterministically in the DSP loop. Disconnecting an input immediately resets its tracker, displaying `---`.

## Pseudo-cycle definition

A pseudo-cycle begins at a rising zero crossing, passes through a falling crossing, and ends at the next rising zero crossing. The end crossing is also the start of the next capture. The falling crossing arms completion; zero plateaus must not generate extra crossings.

This captures two crossing intervals rather than the half-cycle between consecutive crossings. It does not estimate the fundamental period: harmonically rich signals may produce multiple pseudo-cycles within one fundamental period.

Each of the four effective input signals has independent crossing and capture state. A completed capture becomes available atomically within the audio-processing state; unfinished captures are never played.

## Fractional zero-crossing boundaries

Estimate each crossing between adjacent samples with linear interpolation. For a rising crossing bracketed by samples x0 <= 0 and x1 > 0, with at least one strictly negative sample establishing the preceding negative excursion:

```text
alpha = -x0 / (x1 - x0)
crossingTime = previousSampleTime + alpha
```

Handle exact-zero samples and plateaus explicitly so each sign transition produces one event. Apply equivalent logic for falling crossings. Any detector hysteresis or excursion threshold must qualify the event without moving the stored boundary away from the actual interpolated zero crossing. Its value remains to be tuned.

Store fractional start and end times plus the enclosed samples. The reconstructed waveform has explicit zero-valued endpoints. Interpolate from the start endpoint to the first enclosed sample and from the last enclosed sample to the end endpoint, using their actual fractional time spacing. Do not simply replace the first and last original samples with zero: that would distort the captured timing.

Playback evaluates this piecewise-linear waveform at the output sample times. Preserve each segment's fractional duration and carry sub-sample timing across handoffs. If a handoff falls between output samples, evaluate the next output sample at its elapsed offset into the incoming segment. Do not insert a zero sample or round every segment to an integer duration; either would introduce timing drift or artificial dwell at zero.

The continuous reconstructed waveform therefore meets at zero at every A/B boundary. This removes an amplitude discontinuity at the seam, but does not guarantee matching slopes or an inaudible transition. A crossfade is not part of the initial core design; additional seam smoothing should be considered only after listening tests.

## Capture and playback

For each lane:

1. Continuously capture A and B independently.
2. Publish each valid completed pseudo-cycle as that source's newest available capture.
3. Play a selected capture once at its original recorded speed and duration.
4. At its end, select the newest completed capture from the opposite source and begin playback at that capture's zero boundary.
5. If the opposite source has no newer capture, reuse its last valid capture.

```text
A capture -> latest B capture -> latest A capture -> latest B capture -> ...
```

Once selected, a playback capture remains immutable until its segment finishes. New capture completion must not interrupt playback or overwrite its samples. Intermediate captures may be superseded before playback selects them; Strand selects the newest complete capture rather than consuming a queue.

Capture publication precedes playback selection when both occur in the same processing invocation. If a fractional boundary precedes a capture completion within that invocation, only captures completed by the boundary's timestamp are eligible; do not use future samples retroactively.

No resampling to a shared A/B period, pitch matching, amplitude normalization, or mixing is intended. Linear reconstruction at fractional boundaries preserves native playback speed. For steady sources, an A+B pattern lasts approximately TA + TB. Equal-frequency sources therefore yield an alternating pattern with twice the individual cycle duration, although perceived pitch depends on waveform content.

## Startup and exceptional input

The following are proposed defaults pending implementation and listening tests:

- Output silence until a valid capture exists for the lane.
- Prefer A when both first become available together; otherwise start with the available source.
- If only one source has a valid capture, repeat it. Begin alternation at a playback boundary once both become available.
- After its first rising crossing, each source continuously captures. If no subsequent rising crossing arrives within 100 ms, force completion and immediately start another capture. Silence or DC following a crossing therefore refreshes the buffer instead of indefinitely holding an old waveform. Before the first rising crossing, silence or DC alone produces no capture.
- Completely disconnecting a source pair invalidates its capture state. An already selected segment may finish, then playback uses the remaining valid source or silence.
- Changes to a pair's mono/stereo patching reset affected capture detectors and unpublished captures. Finish already selected playback segments, then use captures from the new routing.
- At the maximum capture duration, publish a bounded forced segment even without an intervening falling crossing. Distinguish these timeout segments from natural rising-to-rising pseudo-cycles. Natural crossings resume normal capture boundaries. Reject segments shorter than the minimum duration.
- Reset clears all captures and playback state. Sample-rate changes also clear transient state and recalculate duration limits.

Initial capture bounds are specified below. Bounds prevent unbounded storage and pathological per-sample work; captures longer than the maximum cannot refresh the held waveform. Noise qualification may be refined following listening tests.

## Implementation constraints

- Preallocate bounded capture and playback storage; no audio-callback allocation, blocking, or locks.
- Use explicit buffer ownership so ongoing capture cannot modify selected playback or the newest published capture.
- Keep work bounded per sample, including handling very short captures and fractional handoffs.
- Use inexpensive sign detection and linear interpolation. Division is needed at crossing events; expensive transcendental functions are unnecessary.
- Capture buffers, fractional timing, and playback position are transient and are not serialized into patches.
- Use existing Leviathan panel/component and graphics lifecycle helpers where applicable.
- Any developer diagnostics must be gated by isDragonKingDebugEnabled.

## Acceptance criteria

- A clean periodic input captures rising-to-rising intervals with an intervening falling crossing.
- Fractional boundary tests reconstruct exact zero endpoints without per-cycle duration rounding or added zero dwell.
- Unequal A/B periods play at their captured durations and alternate at completed playback boundaries.
- Captures refresh while their source is inactive, and each handoff selects the latest eligible complete capture.
- Replacing a published capture never modifies a segment already playing.
- Independent lane inputs can produce different capture lengths and handoff schedules without influencing each other.
- Mono-normalled sources produce identical left/right output when both source pairs are mono.
- Exact zeros, zero plateaus, silence, DC, noise, disconnection, forced timeout, reset, and sample-rate changes behave deterministically within bounded resources.
- Listening tests assess seam artifacts and determine whether optional smoothing is needed.

## Initial implementation decisions

- Each physical input reads channel 1; outputs are monophonic.
- Maximum capture duration is 100 ms (a 10 Hz natural-cycle threshold), bounded additionally by 65,534 samples at high host rates. Timeout completion publishes a forced segment and begins the next capture at that exact boundary. Playback uses the same bounded durations, so each A/B segment lasts at most 100 ms.
- Forced endpoints fade to/from zero over 0.5 ms, shortened to at most one quarter of the segment duration (and proportionally shortened when the storage cap shortens the maximum duration). This envelope only applies to forced boundaries; natural crossing boundaries retain fractional interpolation. It is not an overlapping crossfade.
- Minimum accepted pseudo-cycle duration is two samples. No amplitude hysteresis is applied initially; strict excursions and explicit plateau handling qualify crossings.
- The panel has three vertically arranged jacks in 3 HP, with A, B, and C labels.

## Implementation validation (2026-10-10)

- Native MINGW64 `plugin.dll` build/link passed.
- Native `test-fast` passed with the installed Rack2Pro runtime.
- Dedicated engine tests cover fractional boundaries and duration carry, capture refresh, immutable playback, mono determinism, zero plateaus, non-finite inputs, DC, and forced timeout.
- Rack-linked module tests cover three-port configuration, single-source playback, disconnection, reset, and sample-rate changes.
- Master panel artwork was rendered and inspected; split assets and the anchor atlas were regenerated. Runtime panel inspection and listening evaluation remain outstanding.

### Forced timeout validation

Native Strand engine and Rack-linked tests and the Windows plugin link passed after adding forced 100 ms boundaries. Slow-input tests verify bounded published durations and ongoing A/B switching; timeout endpoint tests verify zero termination and the fade.

## Panel styling and frequency readouts

All three jacks use the shared Magitek2 input/output components with a 34 mm vertical pitch. A and B use the standard purple input glass fields, and C uses the cyan output glass field, with the shared themed glass overlay. The panel has no screws. The standard solo wave branding sits at the bottom, using the same placement as Doorstop.

Directly below each jack, a compact frequency readout capsule displays real-time statistics:
- **Input A & B**: Average frequency of accepted natural pseudo-cycles accumulated over a 1.0-second interval, updated at 1 Hz. Forced timeout boundaries are excluded.
- **Output C**: Combined arithmetic mean of both inputs `(f_A + f_B) / 2` when both are active; when only one input is patched/active, it displays that active source's frequency; when inactive, it displays `---`.
- **Display Update Rate**: Data is accumulated over each 1.0-second window and published at 1 Hz. The UI widget caches formatted text and redraws without per-frame string formatting overhead. Cable disconnection immediately clears the port readout to `---`.

