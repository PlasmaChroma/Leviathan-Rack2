# MP86 Clock Synchronization, Tempo Ratios, and Acquisition State Machine

Date: 2026-10-08.
Audited Binary: MP86 (`31323684dacda3d6313565a1a47596005870d1defed55e0336fd419ca82040b9`).
Continues [analysis/HOLD_AND_INPUTS.md](HOLD_AND_INPUTS.md) and [analysis/EVENT_TRANSITIONS.md](EVENT_TRANSITIONS.md).

---

## 1. Overview and State Addresses

When an external clock pulse arrives at the TEMPO input, or when the front panel TEMPO button is pressed, the firmware transitions into clocked delay synchronization (`0x20004d78 == 1`).

In clocked mode, instead of continuous exponential delay time tracking, delay targets are quantized to exact musical ratios selected by the Rate knob from a 13-entry delay ratio table.

| State Address | Type | Meaning |
|---|---|---|
| `0x20005878` | `u32` | DSP block counter (increments once per 4-frame callback; nominal 12 kHz) |
| `0x20004c5c` | `i32` | Clock qualification counter (saturated at 5) |
| `0x200056c4` | `i32` | Clock high-count counter |
| `0x20002b48` | `u32` | Timestamp of last accepted clock pulse (block counter units) |
| `0x2000383c` | `u32` | Last accepted clock period in block counter units |
| `0x20003864` | `u32` | Staged raw measured period |
| `0x20004d78` | `u32` | Clock-mode active flag ($1 = \text{clocked sync}$, $0 = \text{free unclocked}$) |
| `0x20003898` | `u32` | Clock timeout / overdue flag ($1 = \text{overdue > 4 seconds}$) |
| `0x20002b60` | `f32` | Scaled base clock delay interval (samples) |
| `0x20003804` | `[13]f32` | Reconstructed 13-entry tempo delay ratio table |
| `0x20003890` | `u32` | Current integer control token ($0\dots 12$) |
| `0x20001b28` | `u32` | Last consumed token for Head Pair A |
| `0x20001b44` | `u32` | Last consumed token for Head Pair B |
| `0x20004dd4` | `u32` | Clock octave prescaler state |
| `0x200000a8` | `[5]f32` | Startup copy of `rate_aux_coefficients` from flash `0x0802beb0` |

---

## 2. Clock Pulse Qualification

The clock input pin is `GPIOG[10]` (bit mask `1024` at `0x40021810`).
Routine: `0x08023af8..0x08023b20` in `disassembly/dsp_core.asm`:

```text
raw_bit_high = (GPIOG_IDR & 1024) != 0

if raw_bit_high and qualification == 5:
    high_count += 1
    if high_count == 1:
        elapsed = now - last_accepted_timestamp  // uint32 modular subtraction
        if signed(elapsed) > 240:
            enter accepted_clock_handler (0x080264e8)
        else:
            enter short_interval_continuation (0x08025fb6)
else:
    qualification += 1
    if signed(qualification) > 5:
        qualification = 5
    high_count = 0
```

### Physical Parameters
* **Sample / Block Timebase**: One count = 4 stereo audio frames. At nominal 48,000 Hz audio rate, the clock block rate is **12,000 Hz** ($83.333\ \mu\text{s}$ per count).
* **Minimum Pulse Separation**: 240 counts = **$20.0\text{ ms}$** (maximum trackable clock rate of $50\text{ Hz}$ / 3,000 BPM).
* **Clock Loss Timeout**: Checked at `0x08023b20..0x08023b46`. If $\text{now} - \text{last\_accepted\_timestamp} > 48,000$ counts (**$4.0\text{ seconds}$** / 15 BPM minimum), `0x20003898` is set to 1. If manual Rate changes occur while overdue, clock mode `0x20004d78` is cleared back to 0.

---

## 3. The Accepted Clock Handler (`0x080264e8`)

When a valid rising edge exceeds 240 counts, execution branches to `0x080264e8`:

### 3.1 Jitter Tolerance & Immediate Re-alignment
Let `new_period = now - last_accepted_timestamp`:

```text
stored_period = [0x2000383c]
error = new_period - stored_period
tolerance = stored_period >> 7  // +/- 0.78125% window

if abs(error) > tolerance:
    // Period changed: update stored period to new_period
    [0x2000383c] = new_period
    current_token = [0x20003890]
    [0x20001b28] = current_token
    [0x20001b44] = current_token

    // If neither head pair is currently in mid-crossfade:
    if status_A != 2 and status_B != 2:
        if hold_flag == 0 and flip_flag == 0:
            // Force phase re-alignment:
            delay_counter_A = 0
            delay_counter_B = 0
            status_A = 1
            status_B = 1
```

If the tempo changes by more than $\sim 0.78\%$, both heads are immediately signaled to crossfade to the newly aligned tempo boundaries.

### 3.2 Base Clock Period Normalization
At `0x08026562..0x08026614`, the raw period (in 12 kHz blocks) is converted into float audio samples:

$$s_{14} = 2.0 + 4.0 \times \operatorname{float}(\text{new\_period})$$

The firmware then matches $s_{14}$ against the 5 octave coefficients from `rate_aux_coefficients` (`0x200000a8`):

| Index | Value | Approximate Divisor |
|---|---|---|
| 0 | `0x3b85cd15` ($0.0040832856$) | $\approx 244.9$ |
| 1 | `0x3a85cd15` ($0.0010208214$) | $\approx 979.6$ |
| 2 | `0x3985cd15` ($0.00025520535$) | $\approx 3918.4$ |
| 3 | `0x3885cd15` ($6.3801337 \times 10^{-5}$) | $\approx 15673.7$ |
| 4 | `0x3785cd18` ($1.595034 \times 10^{-5}$) | $\approx 62694.6$ |

The loop evaluates:
$$\text{candidate} = s_{14} \times \text{rate\_aux}[i]$$

The first index $i \in [0, 4]$ satisfying:
$$2.0 \le \text{candidate} \le 8.0$$
is selected.

The resulting normalized value is written to `0x20002b60` (optionally prescaled by $0.25$ if `0x20004dd4 == 0`).
Finally, clock mode `0x20004d78` is set to **1** (`0x0802663a`).

---

## 4. The 13 Musical Delay Ratios and Token Mapping

In clocked mode, the manual Rate knob and Rate CV do not continuously change the delay pitch; they select among **13 discrete delay ratios**.

### 4.1 The Exact 13-Ratio Table (`0x20003804`)
Extracted from startup initialization stores (`0x08023428..0x0802348e`):

| Token | Ratio Constant | Float32 | Musical Meaning (Repeats) |
|---|---|---|---|
| 0 | $1 / 2$ | $0.5$ | Double tempo (half delay time) |
| 1 | $4 / 7$ | $0.5714286$ | Septuplet ratio |
| 2 | $3 / 5$ | $0.6$ | Quintuplet ratio |
| 3 | $2 / 3$ | $0.6666667$ | Dotted eighth / Triplet feel |
| 4 | $3 / 4$ | $0.75$ | Dotted sixteenth / $3:4$ polyrhythm |
| 5 | $4 / 5$ | $0.8$ | Quintuplet ratio |
| **6** | **$1 / 1$** | **$1.0$** | **Unity (Exact 1:1 clock sync)** |
| 7 | $5 / 4$ | $1.25$ | Quintuplet ratio |
| 8 | $4 / 3$ | $1.3333334$ | $4:3$ polyrhythm |
| 9 | $3 / 2$ | $1.5$ | Dotted quarter ratio |
| 10 | $5 / 3$ | $1.6666666$ | Quintuplet ratio |
| 11 | $7 / 4$ | $1.75$ | Septuplet ratio |
| 12 | $2 / 1$ | $2.0$ | Half tempo (double delay time) |

*Note on Nomenclature*: These are **delay time ratios** ($t_{\text{delay}} = t_{\text{clock}} \times \text{ratio}$). The audible repetition frequency is the reciprocal ($f_{\text{repeat}} = f_{\text{clock}} / \text{ratio}$).

### 4.2 Token Quantization Law
At `0x080261a2..0x080261c6`:
$$\text{token} = \operatorname{int}\left(\text{Rate}_{\text{ADC}} \times \frac{13}{4096}\right)$$

Where $\frac{13}{4096} = 0.003173828125$ (bits `0x3b500000`).
* For $\text{Rate} \in [0, 4095]$, this yields integers $0 \dots 12$.
* When the Rate knob is centered ($\text{Rate} \approx 2048$), $\operatorname{int}(2048 \times 13 / 4096) = 6$, selecting **$1.0$ (Unity)**.

---

## 5. Delay Target Evaluation in Audio Render

During each block, for head pair A and head pair B:

```text
// Pair A target calculation (0x08025d94..0x08025db6):
token_A = [0x20001b28]
delay_target_A = [0x20002b60] * tempo_delay_ratios[token_A]

// Pair B target calculation (0x08025e40..0x08025e6c):
token_B = [0x20001b44]
if ping_pong_or_skew:
    // Mirror token around center (token 6):
    token_B_effective = clamp(2 * 6 - token_B, 0, 12)
else:
    token_B_effective = token_B

delay_target_B = [0x20002b60] * tempo_delay_ratios[token_B_effective]
```

### Ping-Pong & Skew Polyrhythms
When Ping-Pong or Skew is active, Pair B mirrors around unity (Token 6):
* If Pair A is set to Token 4 ($3/4$), Pair B is set to Token 8 ($4/3$).
* This creates complementary metric counterpoints between Left and Right repeats locked to the master clock.

---

## 6. Rack Specification Implication

1. **TEMPO / CLOCK Input Jack**:
   * Accepts standard Rack gate/trigger pulses ($0\dots 10\text{V}$, threshold $+2.0\text{V}$).
   * Measures interval in host sample cycles, scaled to the internal 48 kHz / 12 kHz block timebase.
2. **Tempo Loss Timeout**:
   * Disengages clock sync automatically if no pulse is received within 4.0 seconds.
3. **Discrete Ratio Snapping**:
   * In clocked mode, the Rate knob and Rate CV (attenuverted) are quantized into the 13 integer bins.
4. **Mirroring Law**:
   * Dual delay heads automatically generate complementary rhythmic divisions when Ping Pong or Skew is engaged.
