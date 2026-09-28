# Controls, modes, I/O and confidence map

## 1. Public module interface versus recovered internals

Make Noise describes two interacting spectral sides with eight outputs, two audio inputs, two gate inputs and ten CV inputs. SAM analyzes incoming sound; SAO uses stored spectra. Sine/Sub paths have a distinct relationship to the FM bus, and side B provides Follow/Sync interaction [S1]. Those statements establish intended user-facing behavior, not a recovered electrical schematic.

The compact mode map below follows the manufacturer's cheat sheet [S3]. The subsequent addresses and implementation notes come from the uploaded binary.

| Mode | Partials role | Focus role | Slide role |
|---|---|---|---|
| SAM | Harmonic activation/timbre | Analysis selectivity | Analysis reference frequency |
| Linear SAO | Harmonic activation/timbre | Coefficient compression/expansion | Position along the Array |
| Planar SAO | Harmonic activation/timbre | Fine grid coordinate | Coarse grid coordinate |
| Noise | Sideband width | Noise high-pass control | Noise low-pass control |
| Chaos | Cross-modulation depth | Oscillator ratio | Feedback |

This table is a semantic index. It is not a claim that the same ADC value has identical calibration or smoothing in every mode.

## 2. Pitch and FM controls

The recovered pitch math uses an octave table, integer/fractional splitting and a 48 kHz denominator. The internal coordinate can be expressed approximately as `1.021974921 * 2^(q/2048+4)` Hz. The path around `0x080305c4` interacts with Follow-like state but has not been translated enough to give a definitive Follow ratio or calibration equation.

The fourth-power FM-index mapping is visible in setup around `0x0802e87e–0x0802e8e8`: `60*u^4`. Two slow ADC slots, 4 and 5, feed the relevant paths. The complete mix between FM bus, external FM and pitch-rate terms needs further tracing. No numeric CV voltage limits, attenuverter polarity or through-zero specification should be inferred merely from the normalized curve.

The LF branch divides a rate by 256. That is a directly observed rate change, while exact mode-entry, state persistence and every alternate path remain separate questions.

## 3. Fast-CV acquisition

Fast samples use four halfwords per frame around `0x30000040`. Tracing the current paths gives:

| Slot | Calibration-index association | Destination interpretation | Confidence |
|---:|---:|---|---|
| 0 | 9 | A Partials, smoothed field around `0x20000880` | High path-level interpretation |
| 1 | 8 | A pitch-related path | High path-level interpretation |
| 2 | 11 | B Partials, smoothed field around `0x200023fc` | High path-level interpretation |
| 3 | 10 | B pitch-related path | High path-level interpretation |

The table does not identify physical ADC pins or convert volts into these readings. The calibration object around `0x200144d4` is not fully reconstructed. Coarse/fine frequency-control summation, offsets, gain errors, polarity and clipping remain to be mapped into a voltage-accurate implementation.

Slow ADC samples are based around `0x30000020`. The first four slots participate in Slide/Focus-related paths; slot order on side B should be rechecked before assigning physical pin labels. Slots 4–5 have the stronger FM-index interpretation described above.

## 4. Digital input and output buffers

Input audio is read as interleaved 32-bit words around `0x30000440`, with a conversion constant `0x2ffffff6` (approximately `4.656610098e-10`, near `1/2^31`). This is not a measured volts-per-code specification.

Output writes reach four paired regions:

| Pair base | Observed stores in the sample path | Interpretation status |
|---|---|---|
| `0x30000840` | Two soft-clipped signal lanes | Pair established; physical jack assignment not fully proven |
| `0x30000c40` | Two additional audio lanes | Pair established; trace final lane identity separately |
| `0x30001040` | Audio/auxiliary-related lane stores | Roles still require final mapping |
| `0x38000000` | Additional sine/CV-related stores | Roles still require final mapping |

The principal store block is around `0x080302b8–0x0803038c`, with pointer setup earlier around `0x0802ec94–0x0802ed12`. The eight-lane structure agrees with the public output count [S1], but agreement in count does not prove ordering.

A voltage-faithful model still needs: final per-lane gains, signed conversion limits, physical jack correspondence, analog scaling, DAC/codec selection, and any reconstruction filtering. Preserve independent output paths rather than collapsing them into one stereo sum.

## 5. Buttons, clock and mode fields

The GPIO callback at `0x08032688` distinguishes arguments 64 and 128. The two branches update clock counters/flags around:

```text
A: counter 0x20002ee0; flag 0x20002edc
B: counter 0x20002ee8; flag 0x20002ee4
```

Application UI/clock processing is concentrated around `0x0802d900`; control acquisition around `0x0802d464`; persistent-state interactions around `0x0802d280`. These are good entry points for continuing decompilation, but the complete state machine has not been reconstructed.

Mode-related fields include A `0x20002e8c` combined with `0x20002e94`, and B `0x20002e88` combined with `0x20002e90`. Their branch results select standard, Noise and Chaos paths. Do not encode those raw storage fields directly as a stable public API; their initialization and transitions are part of the unresolved state machine.

Current Array selection is associated with A `0x20002430` and B `0x20000b20`. Reader clock offsets are associated with A `0x20002438` and B `0x20002434`. The distinction between selection and offset matters: changing a slot and stepping a frame are separate operations.

The manufacturer's cheat sheet documents button combinations and mode-dependent Clock/Shift behavior [S3]. This bundle does not claim to recover every debounce threshold, long-press edge, menu priority, timeout, persistence flag or startup-button gesture. For implementation, keep a documented behavior state machine distinct from raw firmware-equivalence claims.

## 6. Missing I/O facts worth measuring

A pin-accurate or voltage-accurate port should wait for a schematic, board inspection or controlled measurements. In particular, the current evidence does not establish every jack's usable voltage range, output amplitude at each setting, impedance, calibration tolerance, negative-FM behavior, clock trigger threshold, or sample alignment between all output pairs.

For a Rack prototype, expose normalized internal parameters and explicit output lanes first. Then implement a replaceable panel/CV calibration layer. This avoids baking uncertain analog assumptions into an otherwise well-grounded DSP core.
