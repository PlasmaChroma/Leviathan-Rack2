# Module controls, ports, and digital I/O

## Panel-level interface

The documented interface has six clock outputs, six illuminated channel buttons, two program buttons, a leading-tempo input, a MOD gate input, state-select gate and CV inputs, and a combined manual/CV control. State selection can also arrive over the Select Bus. This panel inventory is checked against Make Noise's manual; the GPIO mappings below come from the recovered firmware.

The distinction matters: firmware reveals which MCU bits are read or written. It does not reveal the electrical transfer function of a jack buffer, its protection components, or the physical package-pin number of an unconfirmed chip variant.

## Clock outputs

| Channel | MCU output | Pending bit | Evidence |
|---|---|---|---|
| 1 | RB5 / LATB5 | RAM `0x43C`, bit 0 | Program `0x083C–0x0844` |
| 2 | RB3 / LATB3 | RAM `0x43D`, bit 0 | `0x0848–0x0850` |
| 3 | RB1 / LATB1 | RAM `0x43E`, bit 0 | `0x0854–0x085C` |
| 4 | RB2 / LATB2 | RAM `0x43F`, bit 0 | `0x0860–0x0868` |
| 5 | RB4 / LATB4 | RAM `0x440`, bit 0 | `0x086C–0x0874` |
| 6 | RB0 / LATB0 | RAM `0x441`, bit 0 | `0x0878–0x0880` |

All 64 logical output patterns were tested against this interrupt prefix. It preserves RB6/RB7 and updates the six mapped bits individually. Physical output voltage, active polarity after any external buffer, and channel-to-channel propagation skew were not measured.

The manufacturer describes default half-duty clocks and an optional nominal 10 ms trigger mode. The code has separate output-mode state and timer scheduling, but this dossier does not equate the `200`-tick arithmetic floor with a proven physical 10 ms duration. Those are different claims.

## Buttons

| Control | MCU input | Direct sample instruction |
|---|---|---|
| Channel 1 | RC2 | `0x11F2` |
| Channel 2 | RC5 | `0x11FA` |
| Channel 3 | RD7 | `0x1202` |
| Channel 4 | RC1 | `0x120A` |
| Channel 5 | RC6 | `0x1212` |
| Channel 6 | RD6 | `0x121A` |
| PGM_A | RC0 | `0x1222` |
| PGM_B | RE2 | `0x122A` |

The active-low six-button sample assembled by the interrupt is:

```text
button_mask = ~(PORTC | (PORTD << 8)) & 0xC066
```

The corresponding little-endian masks at flash `0xFE0D` are:

```text
CH1 0x0004   CH2 0x0020   CH3 0x8000
CH4 0x0002   CH5 0x0040   CH6 0x4000
```

These constants independently corroborate the direct sampling order. The fast channel-only debounce routine at `0x7336` is distinct from the larger eight-button UI service at `0x11C0`. Its duration counters at RAM `0x66C` should not be mistaken for the eight-key debounce area at `0x65C`.

The UI supports combinations of presses, holds, and repeated presses; the large service routine is included in full, but not every gesture threshold has been converted into a high-level transition table. A panel-compatible implementation needs that additional work rather than just attaching increment/decrement callbacks to eight buttons.

## Signal inputs and bus

| Function | MCU signal | Recovered behavior | Confidence |
|---|---|---|---|
| Leading tempo | RA4 | Rising-edge capture of elapsed master timing | High |
| MOD | RA0 | Sampled gate level and separately toggled state | High inference from routing consumers |
| State-select gate | RA5 | Edge-driven state stepping, also reflected in UI service | High inference |
| State CV / combo control | AN3 / RA3 | Only selected analog input; calibrated state quantizer | High inference |
| Select Bus receive | RX1 / RC7 | UART ring and proprietary message parser | High inference |

ANSELA is set to `0x08`, ADC channel selection to AN3, and other examined port analog enables are cleared. The ADC wrapper is nonblocking: it cycles through start, wait, and consume-result stages and otherwise returns the last result. Its initial invalid-result sentinel is `0xFFFF`.

The manual specifies a 0–5 V state-CV range and describes receive-only Select Bus operation on the power-bus CV line. Those are documented interface statements, not electrical measurements made during this study. The front-end scaling and jack thresholds cannot be reconstructed from a single ADC-channel selection.

## LED hardware interface

`0x953C` shifts a 16-bit word LSB-first using RD5 for data and RD2 for clock; `0x979E` pulses RD4 as a latch. The one/zero bit helpers are `0x9742` and `0x9756`. This is a software-driven serial LED interface, not evidence for a specific external shift-register part number.

RC3, RC4, RD0, RD1, and RD3 also appear in UI/indicator writes. Their exact lamp/color assignment is not fully named. RE0 and RE1 are configured as outputs, but no explicit LATE write was identified in the recovered application. They remain unresolved rather than being assigned invented roles.

## Hardware initialization and timing caveat

Key direction-register values are TRISA=`FF`, TRISB=`C0`, TRISC=`E7`, TRISD=`C0`, and TRISE=`04`. The serial peripheral is asynchronous, 8-bit, receive-enabled, transmit-disabled. RC6's channel-button use is consistent with that transmit disable.

Timer2 uses prescale 1 and postscale 3. With oscillator frequency `Fosc` and PR2 value `P`:

```text
interrupt_period = 4 * (P+1) * 3 / Fosc
```

No recovered application write establishes PR2. Assuming the reset value and assuming an oscillator frequency are both additional assumptions. The missing resident bootloader may leave peripheral state behind.

UART1's programmed divisor is:

```text
baud = Fosc / (64 * (32+1))
```

Actual bus baud and timer-event duration should be measured or confirmed from the bootloader/board before a cycle-accurate recreation is attempted.
