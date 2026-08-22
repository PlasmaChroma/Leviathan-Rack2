# Patch Design and Troubleshooting — VCV Rack Reference

Read this reference when selecting modules, diagnosing common patch problems, or reducing
the cost of a patch. Treat it as decision guidance, not a fixed shopping list.

## Selecting Modules

The installed Rack library is authoritative. Before recommending or adding a module, call
`vcv_list_library` with a focused `q` or `plugin` filter and use the returned plugin/model
slugs. Do not assume a module is installed, guess a slug, or replace a user's chosen module
without a reason tied to their request.

Choose by required behavior:

| Role | Selection questions |
|---|---|
| Agent-authored sequencer | Prefer `Leviathan:Sibyl`; it provides semantic tracks, patterns, scenes, atomic edits, and revision guards. Read `sibyl.md`. |
| Manual sequencer | Does the user want a visible step grid, direct panel editing, performance controls, melodic CV, gates, or both? Preserve an existing sequencer when present. |
| Oscillator | Required pitch range, polyphony, FM/sync behavior, waveform family, and modulation inputs. |
| Filter | Required response, slope, resonance behavior, drive, stereo/polyphony, and modulation inputs. |
| Envelope/function generator | Gate versus trigger behavior, stage count, retriggering, looping, voltage range, and polyphony. |
| VCA/mixer | Channel count, mono/stereo/polyphony, CV response, sends, metering, and whether a simple utility is sufficient. |
| Effect | Mono/stereo I/O, clocking, freeze/modulation features, latency, and whether it belongs per voice or after mixing. |
| Random/generative source | Clock/reset contract, deterministic seeding, probability controls, voltage range, and quantization needs. |
| Utility | Exact operation and voltage convention; prefer the smallest module that satisfies the routing need. |
| Analysis | Measurement required: waveform, spectrum, tuning, voltage, loudness, or stereo behavior. Octavia's own analysis tools may already suffice. |

When several installed modules qualify, explain the relevant tradeoff briefly instead of
declaring a universal winner. Use `vcv_get_module` after selection to confirm actual ports,
parameter ranges, and polyphonic behavior.

## Voltage Conventions

These are common Rack conventions, not guarantees. Inspect the selected module when the
exact contract matters.

- Audio and bipolar CV are commonly around ±5 V; unipolar CV is commonly 0–10 V.
- Gates are commonly 10 V, while trigger duration depends on the source.
- Pitch commonly uses 1 V/octave; in Octavia/Sibyl, C4 is 0 V.
- Octavia's analyzer normalizes ±5 V peak to 0 dBFS. Higher voltages are reported as
  overrange even though Rack cables can carry them.

## Troubleshooting Guidance

Treat each row as a hypothesis to test. Inspect current routing, signal levels, and the
actual module controls before changing values.

| Symptom | Inspect first | Possible correction |
|---|---|---|
| No sound | Trace backward from the audio sink; find the first silent output, then inspect that module and its gate/CV inputs. | Restore the missing source, open the VCA, correct routing, or start the stopped transport. |
| Intermittent missing notes | Gate duration, envelope retriggering, voice/channel counts, probability, and sequencer run state. | Align gate/envelope timing or polyphony with the intended articulation. |
| Wrong pitch | Pitch source, offsets, fine tune, quantizers, scale settings, and 1 V/oct routing. | Remove unintended offsets or configure the actual quantizer/oscillator contract. |
| Clicks at note boundaries | Waveform discontinuity, zero-time envelope stages, retriggers, and abrupt VCA changes. | Add a small attack/release appropriate to the module or correct retrigger behavior. |
| Clipping/overrange | Use Octavia signal and loudness analysis to locate the first excessive stage. | Reduce gain at that stage; add limiting only when it serves the musical intent. |
| Thin or disappearing stereo content | Correlation, polarity, duplicated paths, and low-frequency side energy. | Correct polarity/routing or narrow only the problematic frequency region. |
| Muddy effects | Send level, low-frequency content entering time-based effects, decay, feedback, and return EQ. | Reduce the contributing range or shorten/attenuate the effect rather than applying a fixed cutoff blindly. |
| Clock drift or wrong reset | Clock source, PPQN, reset polarity/timing, transport state, and competing clocks. | Use one clock authority and match the documented clock/reset contract of the modules involved. |
| Random source is static or chaotic | Probability, seed/lock state, clock, range, and downstream quantization. | Adjust only the control responsible for the undesired behavior; quantize only when pitched output is intended. |
| Feedback howl or runaway | Loop gain, DC, latency, nonlinear stages, and limiter state. | Break or mute the loop first, then rebuild from low gain with bounded stages. |
| High CPU/dropouts | Rack's CPU meter, polyphony, sample rate, oversampling, visual load, and repeated per-voice effects. | Measure one change at a time; reduce the demonstrated bottleneck while preserving the requested sound. |

## Performance Guidance

Do not quote fixed CPU percentages, safe voice counts, thread counts, or guaranteed savings.
They vary with processor, Rack and plugin versions, sample rate, block size, polyphony, and
patch topology. Octavia exposes whole-process timing through `vcv_get_perf`, but not reliable
per-module CPU attribution.

For an optimization request:

1. Inspect topology and current performance before changing anything.
2. Identify likely repeated costs such as high polyphony, per-voice effects, oversampling,
   expensive visualizers, or unnecessarily high sample rate.
3. Change one reversible factor at a time and compare the same musical passage.
4. Preserve sound and latency requirements stated by the user; do not impose a generic live,
   studio, or bounce configuration.
5. Report what was measured, what changed, and any audible or workflow tradeoff.
