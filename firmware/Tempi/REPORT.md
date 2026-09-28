# Tempi 71: recovered firmware and internal architecture

**Investigation date:** 27 September 2026  
**Input:** user-supplied `tempi71(1).wav`  
**Result:** exact transport recovery; PIC18 disassembly; executable models of selected routines; substantial state, GPIO, timing, and Select Bus reconstruction.

## 1. What was actually recovered

The ARM hypothesis does not fit this file. The payload contains legacy **PIC18 instructions**, PIC18 reset and interrupt-vector conventions, banked data-memory accesses, and peripheral-register operations consistent with the **PIC18(L)F46K22 family**. The instruction architecture is established much more firmly than the exact chip suffix: a device-ID read or a legible PCB photograph would still be needed to distinguish physical variants.

This is not an opaque compressed blob. The audio transmits readable **ASCII Intel HEX** using variable-duration, alternating-polarity half-cycles. Every record's length and checksum passes. A separate reconstruction step rebuilds the PCM using the decoded bits and recorded pause positions, and the result matches the original PCM exactly.

| Recovery result | Value |
|---|---:|
| WAV channels / sample format | Mono, signed 16-bit PCM |
| Sample rate | 40,000 Hz |
| Samples | 6,627,265 |
| Duration | 165.681625 seconds |
| Decoded ASCII payload | 102,104 bytes |
| Intel HEX records | 2,323 |
| Data records | 2,320 |
| Address-extension records | 2 |
| End-of-file records | 1 |
| Actual recovered addressed bytes | 37,110 |
| Recovered program-memory bytes | 37,088 |
| Decoded reachable instructions | 16,422 |
| Reachable instruction bytes | 36,546 |
| Entry-point candidates | 80 |
| Direct CALL sites | 270 |

The 80 entries are call targets and important vectors/startup entries, not a claim that the original source contained exactly 80 C functions. Recursive decoding found no out-of-image direct targets, unsupported reachable instruction words, or unresolved writes to the program-counter low register. That is strong internal consistency, not an independently certified decompilation.

### Recovered regions

| Program/configuration address range, inclusive | Bytes | Interpretation |
|---|---:|---|
| `0x000800–0x0097DF` | 36,832 | Application, runtime initialization data, helpers, and padding/reset words |
| `0x00FD40–0x00FE3F` | 256 | Factory states and other constant/mask tables |
| `0x200000–0x200007` | 8 | User-ID area, all `FF` |
| `0x300000–0x30000D` | 14 | Configuration-byte records |

**Not recovered:** flash `0x000000–0x0007FF`, untransmitted flash gaps, physical EEPROM contents, silicon device ID, per-unit calibration, source-level names, debug symbols, or schematics.

The address origin matters. The main raw binary begins at **program byte address `0x800`**, not zero. The separate high constant block must also be loaded at its correct address. The Intel HEX file is the least ambiguous container because it retains all addresses.

## 2. Evidence levels

This dossier distinguishes four kinds of result:

**Recovered bytes** are exact observations from the supplied file. **Executed-routine results** come from the included instruction-level harness and are checked against readable mathematical models. **Strong inferences** connect software behavior to a musical or physical role. **Open questions** require more tracing, independent emulation, or measurements on a real module.

All function names in the disassembly and tables are analyst names. Program addresses are always **byte addresses**; RAM addresses refer to a different address space. A label such as `ratio_to_halfperiod` is a useful reverse lookup, not an original symbol extracted from the developer's build.

The primary reference set is deliberately small: Make Noise's product page and manual; Microchip's PIC18(L)F2X/4XK22 data sheet; and the gputils PIC18F46K22 register/configuration definitions. The public product page identifies firmware 71, but the inspected English PDF says it documents firmware 60. Firmware observations in this dossier therefore take precedence over assumptions derived from that older manual. See `references/SOURCES.md`.

## 3. Execution architecture

The application has a conventional embedded foreground/interrupt split. It is not processing audio samples or running an ARM DSP framework.

At `0x0800`, the application reset vector jumps to `0x11BC`, which jumps to C-runtime startup at `0x779A`. Startup initializes RAM from several flash tables, clears working areas, and transfers to `main` at `0x8998`.

`main` calls the hardware initializer at `0x81F8`, prepares six channel-source pointers and state variables, and repeatedly services this sequence:

```text
0x7A5C  state selection, including Select Bus parser and state ADC
0x4B16  leading-tempo service
0x7336  fast six-channel button edge/debounce service
0x340A  human-programming service
0x11C0  eight-button UI state machine
0x65DE  ratio and phase recomputation
0x234A  timing commit / synchronization service
0x4F66  MOD routing and run/stop service
0x3F32  LED / UI display service
repeat
```

A high-priority interrupt entry at `0x0808` checks the Timer2 flag. On each timer event, it drives outputs, captures inputs, advances master/channel counters, polls UART receive, and services timing-sensitive button capture. The foreground loop does the more expensive arithmetic and editing operations.

### The six-channel engine

Each channel has a 12-byte timer record at RAM `0x498 + 12*c`, with current duration, next duration, and signed countdown fields. Separate arrays hold its square-wave state and pending output level. The source for a channel is selected through a RAM pointer, which allows the MOD machinery to alter routing/gating without replacing the timer engine.

The interrupt begins by copying the **previously prepared** six output bits to `LATB`. Later in the same interrupt it computes new pending bits. Consequently there is a one-interrupt-event pipeline between updating a pending value and that value being driven at the GPIO.

The six GPIOs are written sequentially, not with one atomic six-bit port assignment. The pipeline does not imply simultaneous physical edges; any instruction-level skew and the analog output-buffer behavior would need hardware timing work.

Output preparation combines the channel's waveform level, source level, enable/mute state, and run gate. Muting or stopping is not simply deletion of the stored ratio. The engine preserves separate timing, routing, and output-enable state.

## 4. Most useful reconstructed algorithms

### Ratio encoding is signed and quarter-stepped

The EEPROM stores one signed byte per channel. Loading a state sign-extends it to a 16-bit working value. Routine `0x7E54` turns that code into a halfperiod using integer arithmetic.

For master halfperiod `H` and code `r`, before bounds and 32-bit overflow behavior:

```text
r > 0:  channel_halfperiod = trunc(4*H / (r+4))
r = 0:  channel_halfperiod = H
r < 0:  channel_halfperiod = trunc(H*(4-r) / 4)
```

Thus `+4` means ×2, `+8` means ×3, `-4` means ÷2, `-8` means ÷3, and `0` means unity. Fine steps change the multiplier or divisor by **0.25**, not by a fixed percentage or an equal-tempered frequency step.

The routine clamps its result to **200 through `0xFFFFFF` timer ticks**. The included model preserves the actual signed-32-bit intermediate behavior and truncation, rather than silently replacing the calculation with ideal floating-point arithmetic. The tested musical code span is `-124..124`.

### Phase uses different reference periods on opposite sides of unity

Routine `0x65DE` computes phase offsets at RAM `0x520 + 4*c`. With phase byte `p`:

```text
division, r < 0:        offset = trunc(2*p*master_halfperiod / 4)
unity/multiplication:   offset = trunc(2*p*channel_halfperiod / 4)
```

In ordinary non-overflowing cases, each phase-code step is therefore a quarter of a **master cycle** for divisions, but a quarter of a **channel cycle** for unity or multiplication. Treating phase as a universal normalized knob over each channel's period would not reproduce this calculation.

This establishes the computed offset. It does not by itself fully specify when the synchronization service applies a changed offset to an already-running channel; that larger transition state machine remains only partially reconstructed.

### Alignment uses reduced rational denominators and least common multiples

Routine `0x8BCC` derives an integer alignment factor from each ratio. For negative codes it reduces `(4-r)/4`; for nonnegative codes it derives the denominator of `(r+4)/4`. Routine `0x93FC` combines channel factors through the LCM helper at `0x7F8E`.

This explains why the design maintains master-cycle counters and history snapshots in addition to six independent countdowns. It is retaining rational synchronization information, not merely running six unrelated oscillators at floating-point frequencies. Exact overflow policy for a six-way alignment length and every resynchronization transition are not claimed as finished here.

### State selection has calibrated hysteresis

The state selector uses 16 ADC thresholds and remembers the preceding state. Its hysteresis margin is **one third of an adjacent threshold gap, rounded down**. This was established by tracing the division helper and checked with 4,096 synthetic ADC/previous-state fixtures. It is not a simple `floor(adc*16/1024)` mapper.

### Mutation randomness is reproducible

Routine `0x8446` maintains a 64-bit linear-congruential generator:

```text
state = (state * 6364136223846793005 + 1) modulo 2^64
returned_value = (state >> 49) & 0xFF
```

The return is noteworthy: the code shifts by 49 and copies a nonadjacent high byte into the 16-bit return location. The result observed in all tested cases is an **8-bit value**, not the 15-bit value one might expect from the shift alone. This dossier records the behavior without assuming whether it was intended.

The mutation routine consumes random values using modulo-based perturbations, including `rand % 7 - 3` and `rand % 5 - 2`. A compatible implementation should retain this generator and byte extraction if deterministic behavior matters. Full seed provenance and every clipping/normalization path in mutation remain open.

## 5. States, persistence, and presets

The firmware's state structure can be recovered much more precisely than “four banks of sixteen.” Each state consumes **14 logical bytes**: six signed ratio codes, six phase bytes, an output-enable mask, and a MOD mask.

In EEPROM those bytes are **column-major**, not contiguous 14-byte records. With state `s` in `0..63` and channel `c` in `0..5`:

```text
ratio[c]       EEPROM[0x000 + 64*c + s]
phase[c]       EEPROM[0x180 + 64*c + s]
enable_mask    EEPROM[0x300 + s]
mod_mask       EEPROM[0x340 + s]
```

All 64 addressing cases were executed and checked. The 896-byte state area fits in the 1 KB EEPROM model used by the code; the remaining area holds globals and calibration.

RAM caches use a different layout: channel-interleaved ratio bytes at `0xC2C + 6*s`, phase bytes at `0xDAC + 6*s`, and two 64-byte mask arrays. A 64-bit dirty bitmap supports editing, storing, and reverting. “Edited in RAM” and “stored to EEPROM” are genuinely different operations in the implementation.

The factory-reset routine at `0x6E56` was executed with its interactive-calibration branch disabled. Its 1,101 writes touched 908 distinct EEPROM bytes. This generated the two explicitly named **SYNTHETIC** EEPROM files. They reconstruct code-defined defaults, not the contents of anyone's physical Tempi.

The preset tables were also extracted directly from flash. Bank A contains recognizable power-of-two, prime, consecutive, even, odd, Fibonacci, mixed-ratio, phase, and noninteger patterns. The remaining 48 states are initialized to unity and zero phase. Exact ratio/phase bytes for all 64 states are supplied in CSV and JSON.

## 6. I/O and Select Bus

The primary digital I/O mapping is substantially resolved. The six clock outputs map, in channel order, to **RB5, RB3, RB1, RB2, RB4, RB0**. Six channel buttons map to **RC2, RC5, RD7, RC1, RC6, RD6**; the two program buttons use **RC0 and RE2**.

The leading clock is read on RA4. State selection uses RA5 for its gate and AN3/RA3 for the analog control. RA0 supplies the MOD level. UART1 receive on RC7 is the strong Select Bus candidate. The firmware explicitly disables UART transmission; RC6 is used as a channel-button input, not as an active serial transmitter.

The Select Bus parser at `0x71AA` was recovered down to message bytes. It recognizes exact status values `C0`, `F0`, `F4`, and `F7`; the system-exclusive header it checks is **`00 02 2D`**. It implements program selection, copy/store dispatch, a 64-state Mesh bitmap, current-state initialization, and reversion through higher-level routines.

The details are in `docs/SELECT_BUS.md`, including accepted and ignored status behavior, the 460-byte receive ring, and recorded byte-by-byte test traces. This is not a claim that arbitrary MIDI traffic or MIDI Clock drives the module. In particular, the examined parser does not schedule clocks from `F8` bytes.

## 7. What is still not established

The recovered initializer sets the Timer2 prescaler and postscaler but does not establish a recovered write to PR2. The oscillator's physical frequency is also unknown. Absolute tick duration must therefore remain conditional. For example, **64 MHz and PR2=255 would imply 48 µs per Timer2 interrupt**, but neither assumption is a measured property of this module.

Likewise, SPBRG1 is set to 32 with the low-speed 8-bit baud generator. Its baud is `Fosc/(64*33)`, not automatically 31,250. Do not hard-code either an absolute clock rate or a standard MIDI baud based solely on this analysis.

Other limits include input comparator thresholds and inversion outside the MCU, output-voltage levels and impedances, actual analog CV scaling, exact LED colors/driver part, the bootloader's waveform tolerance and flash policy, and complete gesture-to-timing behavior for every UI state.

The supplied image contains configuration bytes indicating code-protection and other policies under the candidate MCU interpretation. Those are update-file contents, not live fuse measurements, and they do not reveal which records the missing bootloader accepts.

## 8. How to use the artifacts

For further reverse engineering, begin with the addressed disassembly and `functions.csv`, then use `calls.csv`, `ram_symbols.csv`, and register cross-references to find the code behind a behavior. For a behavioral reimplementation, begin with the executable ratio, phase, ADC, state, and random models and their test vectors.

The next highest-value technical work is the complete timing-commit state machine at `0x234A`, followed by human-programming normalization at `0x340A`, MOD transition details at `0x4F66`, and precise UI gesture timing. A logic-analyzer capture and a chip/oscillator photograph would resolve several hardware unknowns much faster than further guessing from constants.

**Bottom line:** this recovery provides the real application bytes and a substantial engineering model, including several exact implementation details absent from a user-facing manual. It is a sound starting point for a faithful implementation, but it is not presented as an already validated, cycle-accurate Tempi clone.
