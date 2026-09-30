# Validation, reverse lookups, and limits

## Completed checks

| Test family | Coverage |
|---|---:|
| Intel HEX checksum and length | 2,323 records, all pass |
| PCM reconstruction | All 6,627,265 samples match |
| C-runtime initialization | 10,331 harness-executed instructions |
| EEPROM addressing | All 64 states, all 14 logical bytes each |
| Factory state reconstruction | All 64 states compared against tables/default writes |
| Factory write trace | 1,101 writes, 908 unique addresses |
| Ratio conversion | 1,743 input/code fixtures |
| Phase conversion | 42 fixtures, each checked on all six lanes |
| Alignment factors | All 249 tested ratio codes |
| LCM helper | Seven representative argument pairs |
| Output GPIO mapping | All 64 six-bit output patterns |
| Random generator | 64 seeds, full state and return value |
| ADC hysteresis | 4,096 synthetic fixtures |
| Select Bus parser | Seven focused message scenarios |

`analysis/validation.json` is the generated result, not a hand-written claim. Assertions fail the test process on mismatch. Test vectors and traces are retained where useful.

## Follow-up timing checks — 28 September 2026

`python3 tools/validate_timing.py` produces `analysis/timing_validation.json`:

| Test family | Coverage |
|---|---:|
| Leading capture consumer versus readable model | 728 cases |
| ADC tempo helper, two synthetic calibrations and six prior ADC values | 12,300 cases |
| Control/capture admission and priority | 18 cases |
| ISR rising-edge capture prefix | 4 cases |
| Master and all six channel reload fragments | 35 cases |
| Timing-commit numerical guards, six lanes | 324 cases |

All 13,409 cases passed in this follow-up run. The ADC wrapper is
hooked; arithmetic helpers execute recovered instructions. Leading model
fixtures use Tap enabled and unchanged Follow; separate caller fixtures cover
control-only, measurement-active, and capture-overrides-control cases. Commit
fixtures cover only the numerical guard fragment, not full history selection.
Details and exact model domains are in `TIMING_RECONSTRUCTION.md`.

## What the harness is

The harness executes selected legacy PIC18 instructions against recovered program bytes. It models arithmetic flags, banked RAM, indirect addressing, table reads, branches/calls/returns, and immediate-completion EEPROM access. It is enough to test isolated arithmetic, initialization, memory mapping, and parser paths.

It does **not** model oscillator timing, real peripheral clocks, ADC acquisition, asynchronous interrupts, analog circuitry, UART line sampling, hardware stack limits, all special-function-register side effects, or complete fast-interrupt context restoration. Its cycle counter is an internal approximation, not a measured performance result. The full firmware has not been booted into a faithful virtual board.

The instruction decoder and harness share definitions. Agreement between them is valuable but is not independent validation against a second PIC18 implementation. Independent disassembly/emulation and hardware traces would raise confidence further.

## Navigation and reverse lookups

`functions.csv` contains entry addresses, inferred names, reachable instruction counts, address extents, and call counts. `calls.csv` maps caller entry and call-site address to callee. `callgraph.dot` is Graphviz source for the call graph.

`ram_symbols.csv` gives named fields with evidence addresses. `direct_data_xrefs.csv` indexes direct RAM/SFR references. `sfr_xrefs.csv` is the peripheral-focused subset. These indexes are conservative: an unresolved bank operand remains unresolved, and indirect RAM-pointer accesses do not magically become named cross-references.

`instructions.json` preserves decoded instruction fields. `constant_tables.json` preserves exact selected flash data. `strings_candidates.txt` contains raw printable candidates, not authenticated messages, source names, or compiler identifiers. Instruction bytes often happen to be printable.

No original C source, symbol/debug table, compiler-version string, or independently verified exact physical part identification was recovered. Searches for public Tempi/PIC18 source or a confirmed MCU match did not supply a primary source establishing the exact chip; the family assignment rests on the binary and register/configuration fit.

## Architecture confidence

Legacy PIC18 decoding is supported by a coherent reset jump, interrupt service, C-runtime initialization, thousands of valid instructions, correctly resolved calls, and meaningful register operations. The K22 family is supported by the specific analog/peripheral map and configuration layout. A 64 KB, PORTD/PORTE-equipped variant is consistent with the high data block and observed GPIOs.

The F46K22 versus LF46K22 suffix, device revision, package, and physical oscillator remain unmeasured. A chip photograph or device-ID read would be stronger evidence than additional speculation from the same register map.

## Security/programming scope

This work decodes a file supplied by the user and analyzes a musical instrument's application. It does not bypass hardware read protection. The configuration records contain protection-related bits under the candidate-device interpretation, but those bytes do not show the live state of a unit.

The missing bootloader prevents conclusions about waveform acceptance thresholds, supported update records, fuse-writing policy, recovery behavior, or whether the recovered application can run after programming a blank chip. No hardware programming is attempted or recommended from the supplied artifacts.

## Evidence gaps that matter for a clone

The exact arithmetic models do not yet specify full UI gesture timing, tap normalization, end-to-end clock locking/relocking, complete phase-commit transitions, every MOD mode, save-time timing effects, or electrical thresholds. The follow-up resolves digital portions of Leading tracking and tempo interpolation, not these complete interactions. A module may sound and behave plausibly while differing on these boundaries. The package therefore separates exact models from partially named services and does not claim end-to-end behavioral equivalence.
