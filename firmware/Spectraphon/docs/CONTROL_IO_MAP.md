# Controls, modes, I/O and confidence map

**Continuation:** [RACK_RECONSTRUCTION.md](RACK_RECONSTRUCTION.md), sections 3–10, supplies checked slow-slot routing, all three raw interaction modes, exact digital lane roles, Array readers and capture/clock facts. Analog pin/voltage calibration and complete gesture/persistence behavior remain separate gaps.

Section 4 also defines the complete indicator routine's register outputs:
positive override, A selection, B selection and engine-display priority;
four PWM compare words; mode/capture blinking; and auxiliary polarity based on
linear mismatch, Shift/LF state and pulse countdown. These definitions have
3612 additional original-routine checks. Physical color/brightness and other
direct display writers remain outside that contract.

The positive override field is also the calibration stage. Section 4 now
defines its boot-button priority, stable-input retention, Shift-release
cancellation/advance and post-save caller reset. Clearing it resets raw UI
mode/interaction bytes and button snapshots. Actual calibration measurements
and flash writes remain outside these lifecycle tests.

Section 3 additionally defines calibration's twelve 64-entry histories and
shared index, channel polarity/order, stage 1 upper-endpoint capture and stage 2
offset/reciprocal-gain updates. It distinguishes strict update thresholds from
inclusive readiness thresholds and preserves zero-span infinity. Stages 3..11
now have checked pitch measurement windows, reciprocal slopes and tail
extrapolation, including rejection behavior and normal pitch conversion using
the generated tables. Measured volts-to-ADC behavior remains unresolved.

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

The recovered pitch math uses an octave table, integer/fractional splitting and a 48 kHz denominator. The independent internal coordinate can be expressed approximately as `1.021974921 * 2^(q/2048+4)` Hz. B's linked-pitch equation and all three interaction modes are instruction-checked. The complete LED routine and manufacturer descriptions [S7, S8] now establish **raw 0 = independent/off, 1 = Follow, 2 = Sync**. Physical pitch calibration remains unresolved. Section 4 of the continuation gives the exact indicator pattern and tuning-beacon windows, including retained pin state.

The fourth-power FM-index mapping at `0x0802e87e–0x0802e8e8` is instruction-checked: `60*u^4`, with separately rounded squarings. Slow ADC slots 4 and 5 feed A and B. Reconstruction section 4 defines the digital internal bus: each side receives the other side's old clean-sine value, while clean phases remain outside that modulation. `phase_step()` and complete callbacks check its phase/rate integration, including Follow/Sync. Physical external-CV summing, voltage limits and attenuverter polarity remain unverified; do not infer them from the normalized curve.

The LF branch divides a rate by 256. That is a directly observed rate change, while exact mode-entry, state persistence and every alternate path remain separate questions.

## 3. Fast-CV acquisition

Fast samples use four halfwords per frame around `0x30000040`. Tracing the current paths gives:

| Slot | Calibration-index association | Destination interpretation | Confidence |
|---:|---:|---|---|
| 0 | 9 | A Partials, smoothed field around `0x20000880` | High path-level interpretation |
| 1 | 8 | A pitch-related path | High path-level interpretation |
| 2 | 11 | B Partials, smoothed field around `0x200023fc` | High path-level interpretation |
| 3 | 10 | B pitch-related path | High path-level interpretation |

The table does not identify physical ADC pins or convert volts into these readings. The continuation now defines fallback offsets/gains, all eight pitch regions, smoothing and saved calibration selection/layout. The actual saved block at `0x080e0000` lies outside this image. Coarse/fine/CV analog summation, device gain errors and volts-to-code behavior still require additional evidence for a voltage-accurate implementation.

Slow ADC samples are based around `0x30000020`. The checked slot order is A Slide, A Focus, B Focus, B Slide, A FM, B FM. Section 3 of the continuation gives exact addresses, normalization and smoothing. This identifies logical routing, not physical ADC pins or jack voltage gains.

## 4. Digital input and output buffers

Input audio is read as interleaved 32-bit words around `0x30000440`, with a conversion constant `0x2ffffff6` (approximately `4.656610098e-10`, near `1/2^31`). This is not a measured volts-per-code specification.

Output writes reach four paired regions:

| Pair base | Observed stores in the sample path | Interpretation status |
|---|---|---|
| `0x30000840` | B even, B odd | Digital role and conversion instruction-checked |
| `0x30000c40` | A odd, A even | Digital role and conversion instruction-checked |
| `0x30001040` | A clean sine, A Sub/CV | Digital role and conversion instruction-checked |
| `0x38000000` | B Sub/CV, B clean sine | Digital role and conversion instruction-checked |

The principal store block is around `0x080302b8–0x0803038c`, with pointer setup earlier around `0x0802ec94–0x0802ed12`. The eight-lane structure agrees with the public output count [S1], but agreement in count does not prove ordering.

Reconstruction section 8 gives the checked digital scales: spectral/clean sine lanes use 2^30 and Sub/CV uses 858993472, with truncation toward zero and distinct A/B clip rounding. The ordinary-range tests do not cover every saturated conversion. A voltage-faithful model still needs physical jack correspondence, analog scaling, DAC/codec identification and reconstruction filtering. Preserve independent output paths rather than collapsing them into one stereo sum.

## 5. Buttons, clock and mode fields

The GPIO callback at `0x08032688` distinguishes arguments 64 and 128. The two branches update clock counters/flags around:

```text
A (argument 64): counter 0x20002ee8; flag 0x20002ee4
B (argument 128): counter 0x20002ee0; flag 0x20002edc
```

The initial report reversed these A/B labels. The continuation now checks each ISR through the complete UI handler and verifies the receiving Array/capture fields. Application UI/clock processing is concentrated around `0x0802d900`; LED/output handling around `0x0802d464`; persistent-state interactions around `0x0802d280`. Selected transitions are checked, but the complete state machine has not been reconstructed.

Mode-related fields include A `0x20002e8c` combined with `0x20002e94`, and B `0x20002e88` combined with `0x20002e90`. Their branch results select standard, Noise and Chaos paths. Initialization, the unshifted SAM-to-SAO-to-Noise-to-Chaos cycle, LF toggles and selected concurrent actions are checked in reconstruction sections 2 and 9. Retain separate raw mode, engine and cached state where the handler observes them; a single enum updated early can change simultaneous-button behavior. Remaining composed gestures and successful persistence are still separate gaps.

Current Array selection is associated with A `0x20002430` and B `0x20000b20`. Reader clock offsets are associated with A `0x20002438` and B `0x20002434`. The distinction between selection and offset matters: changing a slot and stepping a frame are separate operations.

The manufacturer's cheat sheet documents button combinations and mode-dependent Clock/Shift behavior [S3]. This bundle does not claim to recover every debounce threshold, long-press edge, menu priority, timeout, persistence flag or startup-button gesture. For implementation, keep a documented behavior state machine distinct from raw firmware-equivalence claims.

## 6. Missing I/O facts worth measuring

A pin-accurate or voltage-accurate port should wait for a schematic, board inspection or controlled measurements. In particular, the current evidence does not establish every jack's usable voltage range, output amplitude at each setting, impedance, calibration tolerance, negative-FM behavior, clock trigger threshold, or sample alignment between all output pairs.

For a Rack prototype, expose normalized internal parameters and explicit output lanes first. Then implement a replaceable panel/CV calibration layer. This avoids baking uncertain analog assumptions into an otherwise well-grounded DSP core.
