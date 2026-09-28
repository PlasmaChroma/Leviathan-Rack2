# Reimplementation guidance and next investigations

## A practical behavioral core

A software version should separate a musical state from its running timing state. Store each program as six signed ratio codes, six phase bytes, and two masks. Keep active timer records, phase offsets, dirty flags, source routing, and run gates outside that stored structure.

Reuse the recovered integer ratio and phase calculations before designing new continuous controls. The “quarter step” is a step in multiplier/divisor magnitude, not a universal percentage. Likewise, divider phase and multiplier phase have different reference periods. Test against the supplied vectors before connecting a front panel.

Keep a state cache and explicit save/revert semantics. A user editing a state should not implicitly overwrite persistent storage. The column-major hardware EEPROM layout need not be a software application's native layout, but an importer/exporter must preserve it exactly.

For deterministic mutation, retain the observed 64-bit recurrence and 8-bit extraction. Do not substitute a host PRNG while claiming identical results. Seed selection and mutation clipping still require further tracing.

## Time representation

The hardware's absolute interrupt tick is unresolved. A software implementation can use abstract ticks initially or a documented chosen timebase. It must not present that chosen rate as a recovered physical fact.

After the real tick is measured, use a fractional accumulator to map hardware ticks into the host's sample clock rather than rounding every hardware period to a fixed number of samples. Preserve integer countdown and output-pipeline semantics where they affect transitions. Whether to reproduce sequential GPIO skew is a separate fidelity choice.

## Highest-value remaining code

| Address | Investigation | Why it matters |
|---|---|---|
| `0x234A` | Complete timing commit / synchronization transitions | Defines when new ratios and phase values take effect |
| `0x340A` | Human-programming interval normalization and quantization | Converts taps into stored ratio and phase codes |
| `0x4B16` | Leading-clock tracking and tempo-control law | Establishes acquisition, loss-of-clock, and tempo interpolation behavior |
| `0x4F66` | Exact MOD state machine | Separates shift, toggle, momentary, stop, and restart cases |
| `0x11C0` | Gesture and edit-page transition table | Needed for front-panel compatibility rather than parameter-only emulation |
| `0x5B66` and `0x693A` | Calibration equations and interaction | Maps real ADC values to state thresholds |
| `0x6BFA` | Full mutation normalization and seed path | Completes deterministic state/bank mutation behavior |
| `0x466A` and other unnamed helpers | Service/diagnostic roles | Avoids misclassifying auxiliary code as the missing resident bootloader |

## Most decisive hardware evidence

A legible MCU and oscillator photograph would narrow the exact part and clock. A logic-analyzer capture of one steady channel, an external tempo edge, a ratio change, and a phase change would constrain timer rate and transition ordering. A known 1 KB EEPROM dump from a consenting owner would validate globals and calibration layout; the read-only decoder here is ready for such a file.

Clock tests should include transitions rather than just steady-state ratios: faster-to-slower external tempo, signal loss and return, mute/unmute, MOD run/stop, state recall during activity, and a phase change just before an expected edge. Those cases are where a plausible implementation is most likely to diverge from the original.

## Independent tooling check

Load the recovered program segments at their true byte addresses in an independent PIC18 disassembler. Start by checking the GOTO at `0x0800` and the timer handler at `0x0808`; confirm that your tool's address units are bytes rather than instruction words. Add the `0xFD40` data block separately when using raw binaries. The Intel HEX preserves address regions and is preferable when the importer supports it.

Do not force an ARM/Thumb language, relocate the application to zero, or interpret all uncovered constant bytes as instructions. The generated symbol and cross-reference CSVs can serve as navigation aids, but remain analyst annotations rather than authoritative compiler metadata.
