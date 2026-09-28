# Make Noise TEMPI 71 — Behavioral Reconstruction and Engineering Specification

**Purpose:** Standalone behavioral specification derived from the recovered TEMPI 71 firmware image, official Make Noise TEMPI documentation, and official René/Select Bus documentation.

**Prepared:** 28 September 2026  
**Primary firmware input:** `tempi71(1).wav` supplied by the user  
**Recovered architecture:** PIC18 application firmware, strongly consistent with PIC18F46K22 / PIC18LF46K22 family behavior  
**Intended use:** behavioral reimplementation, interoperability work, emulator/module design, further reverse engineering, and validation against physical hardware.

---

## 0. Executive summary

TEMPI is best understood not as six independent clock oscillators, but as a **six-channel rational polyclock engine referenced to a shared Leading Tempo**, with a second layer of stored musical state and a third layer of transient performance transformations.

The recovered firmware and public behavior agree on the following core architecture:

1. A **Leading Tempo** provides the master temporal reference.
2. Six Variable Clock channels derive rational multiples or divisions of that reference.
3. Each stored channel has an encoded **ratio** and **phase**.
4. Each State additionally stores per-channel **active/mute** and **MOD-enable** membership.
5. TEMPI stores **64 States**, arranged as four Banks of sixteen.
6. Runtime behaviors such as **Shift** and **Run/Stop** transform routing or phase without necessarily altering the stored ratio/phase data.
7. State selection combines an absolute base selection with relative stepping.
8. A receive-only **Select Bus** allows remote state, Mesh, Store, Default, and Revert operations.
9. Persistent EEPROM data is intentionally separated from the current editable RAM representation; edits are not implicitly saved.
10. Human Programming measures performed timing and converts it to the same ratio/phase representation used by Machine Programming.

The most important exact implementation discovery is the ratio code. Each channel stores a signed byte `r`, where quarter-step arithmetic is applied differently above and below unity:

```text
r > 0: multiplier = 1 + r/4
r = 0: unity
r < 0: divisor    = 1 - r/4
```

Examples:

```text
 r =   0  -> ×1
 r =  +1  -> ×1.25
 r =  +4  -> ×2
 r =  +8  -> ×3
 r =  -1  -> ÷1.25
 r =  -4  -> ÷2
 r =  -8  -> ÷3
 r = -124 -> ÷32
 r = +124 -> ×32
```

This is not a generic logarithmic clock-ratio parameter. A Fine increment/decrement changes the *multiplier or divisor number itself by 0.25*.

The firmware also keeps explicit rational alignment information using greatest-common-divisor and least-common-multiple arithmetic. This is strong evidence that synchronization is designed around a rational master grid rather than six unrelated free-running floating-point oscillators.

This document distinguishes exact recovered facts from semantic inferences and unresolved behavior.

---

# 1. Evidence model

Every claim in this specification should be read at one of four confidence levels.

| Level | Meaning |
|---|---|
| **A — Recovered/verified** | Directly visible in firmware bytes or validated by executing recovered PIC18 routines against an independent readable model. |
| **B — Corroborated** | Firmware behavior and official Make Noise documentation independently agree on the semantic interpretation. |
| **C — Strong inference** | Code architecture strongly supports the interpretation, usually with partial documentation support, but every transition has not been reduced to high-level behavior. |
| **D — Open** | Plausible or partially traced, but should not be treated as faithfully reconstructed. |

### Primary evidence

**Firmware evidence** comes from the TEMPI 71 update WAV supplied by the user. The transport was decoded exactly into Intel HEX; all Intel HEX checksums passed, and rebuilding the original PCM from the recovered bitstream reproduced the source PCM exactly.

Relevant recovered functions are referenced throughout this document by program byte address, for example:

```text
0x7E54  ratio_to_halfperiod
0x65DE  recompute_ratio_and_phase
0x7A5C  state_selection_service
0x4B16  leading_tempo_service
0x4F66  mod_routing_service
0x340A  human_programming_service
0x71AA  select_bus_parse_byte
```

All such names are analyst labels, not original symbols.

### Public documentation evidence

The current Make Noise TEMPI product page identifies `tempi71` as the current revision and describes TEMPI as a six-channel polyphonic time-shifting clock with Human/Machine programming, multiplier/divisor and phase programming, Shift, Run/Stop, 64 States, voltage-controlled State selection, and Select Bus support.

The publicly available English TEMPI manual contains older-version prose but includes the later behavioral additions relevant to TEMPI 71 in its firmware changelog. Where the manual and firmware differ, recovered v71 firmware takes precedence.

Official sources used:

- Make Noise TEMPI product page: https://www.makenoisemusic.com/modules/tempi/
- Make Noise TEMPI manual: https://www.makenoisemusic.com/wp-content/uploads/2024/03/tempimanual.pdf
- Make Noise René manual / Select Bus description: https://www.makenoisemusic.com/wp-content/uploads/2024/03/renemanual.pdf
- Microchip PIC18(L)F2X/4XK22 data sheet: https://ww1.microchip.com/downloads/en/DeviceDoc/40001412G.pdf

---

# 2. Recovered firmware platform

## 2.1 CPU family

**Confidence: A for PIC18 architecture; C for exact package/suffix.**

The update does **not** contain ARM code. The application uses classic PIC18 instructions, PIC18 vector conventions, banked data-memory addressing, PIC18-style table reads, EEPROM unlock sequences, UART registers, ADC registers, and Timer2 behavior.

The register map and memory organization are strongly consistent with the PIC18F46K22 / PIC18LF46K22 family.

The actual chip marking has not been read from a physical board, so this specification should not promote the candidate part number to a measured hardware fact.

## 2.2 Application layout

Recovered application flash begins at byte address `0x0800`, implying a resident region below it, almost certainly containing the firmware-update/bootloader machinery.

Important regions:

| Address range | Interpretation |
|---|---|
| `0x000800–0x0097DF` | Main application and initialized data |
| `0x00FD40–0x00FE3F` | Factory state tables and constants |
| `0x200000–0x200007` | User-ID region, transmitted as `FF` |
| `0x300000–0x30000D` | Configuration records |

The missing `0x000000–0x0007FF` region means the update transport and bootloader's flash-programming policy are outside the recovered application.

## 2.3 Foreground/interrupt split

**Confidence: A.**

TEMPI uses a classic real-time embedded split.

The foreground loop repeatedly services approximately:

```text
state_selection_service()
leading_tempo_service()
six_button_edge_service()
human_programming_service()
button_ui_service()
recompute_ratio_and_phase()
clock_timing_commit_service()
mod_routing_service()
led_ui_service()
```

Timer2 interrupt handling performs timing-critical work:

```text
1. Drive previously prepared output values.
2. Sample external timing inputs.
3. Detect/capture Leading Tempo edges.
4. Advance master/channel counters.
5. Toggle channel timing state when counters expire.
6. Poll UART RX and enqueue Select Bus bytes.
7. Sample fast channel-button timing information.
8. Prepare the next set of output values.
9. Return from interrupt.
```

A useful fidelity detail is that GPIO output values are **prepared during one interrupt event and physically driven at the start of the following event**. This creates a one-Timer2-event output pipeline.

---

# 3. Conceptual behavioral architecture

A faithful software implementation should keep **stored musical state**, **global configuration**, and **runtime timing state** separate.

Recommended conceptual model:

```cpp
struct TempiState {
    int8_t  ratio[6];       // recovered signed quarter-step representation
    uint8_t phase[6];       // recovered phase parameter bytes
    uint8_t enableMask;     // 1 = active/output-enabled
    uint8_t modMask;        // 1 = channel participates in MOD behavior
};

struct TempiGlobalSettings {
    uint8_t bank;            // 0..3
    HumanResolution humanResolution;
    ShiftMode shiftMode;     // OFF, CW, CCW, RANDOM
    RunStopMode runStopMode; // OFF, NORMAL, ALL, ALT
    ModGateMode modGateMode; // MOMENTARY or TOGGLED
    bool triggerMode[6];     // false=50% clock, true=nominal 10 ms trigger
    bool leadingTapTempo;
    bool selectBusFollow;
    uint32_t storedLeadingPeriod;
};

struct TempiRuntime {
    uint8_t currentState;       // 0..63
    uint8_t stateStepOffset;    // relative Gate stepping within bank
    uint8_t shiftSource[6];     // transient timing-source permutation
    bool    runGate[6];         // transient Run/Stop state
    RuntimeClock channel[6];
    LeadingClock master;
    bool dirtyState[64];
    uint64_t meshMask;
};
```

This is a **semantic model**, not the literal RAM layout. The original firmware uses multiple parallel arrays, pointer tables, bitmaps, and channel timer records.

---

# 4. Leading Tempo

## 4.1 Sources

**Confidence: B.**

The official manual describes five ways TEMPI can establish Leading Tempo:

1. Last Stored tempo when no external tempo source is present.
2. External pulses at the Leading Tempo input.
3. A bus-derived tempo source.
4. Tap Tempo using Channel 1 when enabled.
5. Voltage-controlled tempo using the State combo control/CV when Tap Tempo is disabled.

The firmware contains distinct external-edge capture, tap/UI timing, stored tempo, bus/follow configuration, and ADC control paths consistent with those modes.

## 4.2 External acquisition

**Confidence: B for two-edge acquisition; C for filtering details.**

The Leading Tempo input is read on **RA4**. Rising edges capture elapsed timing.

The manual explicitly states that TEMPI requires a minimum of **two incoming pulses** to measure and lock to a new external clock. That is consistent with direct period measurement: one edge establishes a reference point and the next provides an interval.

Behavioral model:

```cpp
onLeadingRisingEdge(now) {
    if (havePreviousEdge) {
        measuredPeriod = now - previousEdge;
        updateLeadingPeriod(measuredPeriod);
    }
    previousEdge = now;
}
```

Do **not** assume that `updateLeadingPeriod()` is merely `leadingPeriod = measuredPeriod`. The `leading_tempo_service` at `0x4B16` contains history/sanity logic that has not yet been completely reduced.

The manual's warning that fast-to-slow tempo changes take time is behaviorally important: absence of an expected edge cannot immediately distinguish a slower clock from a stopped clock.

## 4.3 Internal tempo control

**Confidence: C.**

When Tap Tempo is disabled, the State combo control and associated CV input change Leading Tempo rather than State.

The firmware contains a 16-entry tempo-control table:

```text
200000, 180000, 130000, 80000,
70000,  40000,  12000,  10000,
8500,   6000,   5000,   3500,
2500,   1750,   1000,   250
```

This table participates in the control path, apparently as a calibrated/interpolated period curve. The exact end-to-end `voltage -> tempo` law is not yet proven.

A reimplementation should therefore treat a simple linear BPM mapping as an approximation unless/until `0x627C` and surrounding ADC/calibration logic are fully reconstructed or a real module is measured.

---

# 5. Ratio system

## 5.1 Stored encoding

**Confidence: A.**

Each State stores one signed 8-bit ratio code per channel.

The implementation promotes it to a signed working value and converts it to channel half-period using routine `0x7E54`.

Let:

- `H` = Leading Tempo half-period in internal timer ticks
- `r` = signed ratio code
- `h` = resulting channel half-period

Then:

```text
r > 0:
    h = truncTowardZero((4 * H) / (r + 4))

r = 0:
    h = H

r < 0:
    h = truncTowardZero((H * (4 - r)) / 4)
```

After calculation, the firmware constrains the result to:

```text
200 <= h <= 0x00FFFFFF
```

The arithmetic uses signed 32-bit intermediate behavior.

## 5.2 Musical interpretation

For positive codes:

```text
frequencyRatio = (r + 4) / 4
               = 1 + r/4
```

For negative codes:

```text
divisor = (4 - r) / 4
        = 1 - r/4

frequencyRatio = 1 / divisor
```

Selected values:

| Code | Meaning | Half-period for `H=8000` |
|---:|---:|---:|
| `-124` | ÷32 | 256000 |
| `-12` | ÷4 | 32000 |
| `-8` | ÷3 | 24000 |
| `-5` | ÷2.25 | 18000 |
| `-4` | ÷2 | 16000 |
| `-2` | ÷1.5 | 12000 |
| `-1` | ÷1.25 | 10000 |
| `0` | ×1 | 8000 |
| `+1` | ×1.25 | 6400 |
| `+2` | ×1.5 | 5333 |
| `+4` | ×2 | 4000 |
| `+8` | ×3 | 2666 |
| `+12` | ×4 | 2000 |
| `+124` | ×32 | 250 |

This exactly explains the manual's Coarse and Fine programming language.

### Coarse programming

For integer multiplier `n`:

```text
r = +4 * (n - 1)
```

For integer divisor `n`:

```text
r = -4 * (n - 1)
```

Thus Machine Programming's 1..32 coarse range maps naturally onto the recovered signed-byte range.

### Fine programming

A Fine increment or decrement is naturally represented by adding or subtracting one code unit.

This produces:

```text
... ÷2.5, ÷2.25, ÷2, ÷1.75, ÷1.5, ÷1.25,
×1, ×1.25, ×1.5, ×1.75, ×2, ×2.25 ...
```

The discontinuity in representation at unity is intentional: positive and negative codes represent multiplier magnitude and divisor magnitude, not one globally linear frequency variable.

## 5.3 Factory-state validation

**Confidence: A/B.**

The recovered factory ratio tables independently reproduce the factory patterns described in the manual.

Examples:

```text
Recovered state 2:
0, -4, -12, -28, -60, -124
-> ÷1, ÷2, ÷4, ÷8, ÷16, ÷32

Recovered state 8:
0, +4, +12, +28, +60, +124
-> ×1, ×2, ×4, ×8, ×16, ×32
```

The final factory demonstration state contains:

```text
-4, -5, -6, -7, -8, -9
```

which corresponds to:

```text
÷2, ÷2.25, ÷2.5, ÷2.75, ÷3, ÷3.25
```

This matches the manual description of successive Fine Decrements producing clocks that slowly drift in and out of phase.

---

# 6. Rational synchronization model

## 6.1 Alignment factor

**Confidence: A.**

TEMPI computes a rational alignment factor for each channel.

For negative ratio codes:

```text
factor = (4-r) / gcd(4-r, 4)
```

For nonnegative ratio codes:

```text
factor = 4 / gcd(r+4, 4)
```

This is the reduced denominator needed for the channel/master relationship to return to a common master-cycle alignment.

Examples:

```text
×2       -> factor 1
×1.5     -> factor 2
×1.25    -> factor 4
÷2       -> factor 2
÷3       -> factor 3
÷2.25    -> factor 9
```

The firmware combines channel alignment factors with an LCM helper and stores a resulting master alignment length used by the timing engine.

## 6.2 Engineering implication

**Confidence: C semantic conclusion from A-level arithmetic.**

This is strong evidence that the intended behavior is not simply six independent oscillators whose frequencies happen to be mathematically related.

A high-fidelity implementation should derive all six clocks from a common rational temporal coordinate system. In a host environment such as VCV Rack, the implementation can remain sample-accurate while still using rational integer relationships internally.

Recommended conceptual strategy:

```cpp
master.phase += ...;

for each channel:
    derive rational cycle relationship from stored ratio code;
    schedule edges relative to shared master history/alignment;
```

rather than relying entirely on six unconstrained floating-point phase accumulators.

---

# 7. Phase programming

## 7.1 Recovered phase arithmetic

**Confidence: A for arithmetic; C for complete real-time transition semantics.**

Each channel stores one phase byte.

Routine `0x65DE` chooses its phase reference according to the sign of the ratio:

```cpp
base = (ratio < 0)
     ? masterHalfPeriod
     : channelHalfPeriod;

offset = truncTowardZero(base * phaseByte * 2 / 4);
```

Since `base` is a half-period, `phaseByte = 1` corresponds to one quarter of the relevant full cycle.

Therefore:

- **Division:** one phase-code unit = one quarter of a **Leading Tempo cycle**.
- **Unity/multiplication:** one phase-code unit = one quarter of the **channel's own cycle**.

This asymmetry is a real implementation detail.

A clone that treats phase as a universal normalized `0..1` fraction of each output channel's period will not reproduce this calculation for divided clocks.

## 7.2 Relationship to the manual

The manual states that Coarse Phase operates in Leading-Tempo-cycle terms and that Fine Phase for divisions moves in quarter cycles of Leading Tempo. It also notes that Coarse Phase on integral multiples may be inaudible because the multiplied output realigns at each master pulse.

The documentation's prose about multiplied-clock Fine Phase is less precise than the recovered arithmetic. For reimplementation, the recovered routine should be considered authoritative for the stored phase-to-offset conversion.

## 7.3 Coarse versus Fine phase

**Confidence: B behaviorally; D for exact UI-to-byte transformation in all cases.**

Manual behavior:

- Phase page entered with both PGM buttons.
- State changes and Human Programming are suspended while the page is active.
- Coarse phase is adjusted by holding PGM_A or PGM_B and tapping channel buttons.
- Fine phase is adjusted by holding a channel and pressing PGM_A or PGM_B.
- Coarse adjustment overrides prior Fine adjustment.

The precise transformation from every Coarse gesture to the stored `phaseByte`, particularly for noninteger multipliers, should still be traced through `button_ui_service()` before claiming bit-perfect front-panel emulation.

## 7.4 Runtime application

The calculated phase offset is not the entire phase model. `clock_timing_commit_service()` maintains histories and synchronization guards that determine *when* new timing takes effect relative to clocks already in flight.

Therefore two fidelity tiers should be distinguished:

**Parameter-compatible implementation:** reproduce ratio and phase arithmetic but apply changes using a reasonable sample-domain scheduler.

**Behaviorally faithful implementation:** also reproduce the timing commit/history state machine around transitions.

---

# 8. Output generation, mute, and pulse width

## 8.1 Six clock outputs

**Confidence: A MCU mapping.**

| Channel | MCU output |
|---|---|
| 1 | `RB5 / LATB5` |
| 2 | `RB3 / LATB3` |
| 3 | `RB1 / LATB1` |
| 4 | `RB2 / LATB2` |
| 5 | `RB4 / LATB4` |
| 6 | `RB0 / LATB0` |

The firmware updates these six bits individually.

Physical Eurorack voltage, output impedance, external buffering, and polarity at the jack have not been measured.

## 8.2 50% clocks versus 10 ms triggers

**Confidence: B semantic behavior.**

Each output defaults to a 50% duty-cycle clock. Clock Edit allows each channel to instead emit nominal **10 ms trigger pulses**.

Channel 1's Clock Edit state additionally controls whether Channel 1 is used for Leading Tap Tempo.

Recommended abstract behavior:

```cpp
if (outputMode == CLOCK_50_PERCENT) {
    gate = channelSquareWave;
} else {
    onChannelRisingEvent(): triggerPulse.start(0.010 seconds);
    gate = triggerPulse.active();
}
```

The exact original hardware tick conversion behind the 10 ms behavior should not be inferred from the recovered `200`-tick minimum-period clamp. Those are separate mechanisms.

## 8.3 Mute semantics

**Confidence: B.**

Mute suppresses physical output but does **not** destroy the channel's programmed clock.

The manual explicitly states that a muted channel LED continues to flash on programmed rising edges and that new timing may be programmed while muted.

The correct conceptual behavior is:

```cpp
internalClockContinues = true;
physicalOutput = channelEnabled ? internalClock : LOW;
ledTiming = internalClock;
```

not:

```cpp
if (muted)
    oscillator.stop();
```

This distinction matters when a channel is unmuted: its underlying timing relationship has continued to exist.

---

# 9. Stored States and persistence

## 9.1 State contents

**Confidence: A.**

Each of the 64 States consists of fourteen logical bytes:

```text
6 ratio bytes
6 phase bytes
1 output-enable mask
1 MOD-enable mask
```

The musical meaning is:

```cpp
struct StoredState {
    int8_t ratio[6];
    uint8_t phase[6];
    uint8_t enableMask;
    uint8_t modMask;
};
```

No evidence indicates that transient Shift permutations or Run/Stop offsets are serialized into each State.

## 9.2 EEPROM layout

**Confidence: A.**

The original EEPROM is column-major rather than an array of fourteen-byte State records.

For State `s=0..63` and channel `c=0..5`:

```text
ratio[c]       EEPROM[0x000 + 64*c + s]
phase[c]       EEPROM[0x180 + 64*c + s]
enable_mask    EEPROM[0x300 + s]
mod_mask       EEPROM[0x340 + s]
```

This occupies exactly:

```text
64 * 14 = 896 bytes
```

within a 1 KiB EEPROM address space used by the firmware.

## 9.3 RAM cache and dirty states

**Confidence: A.**

The running firmware caches all States in RAM and maintains a 64-bit dirty bitmap.

This implements a deliberate distinction between:

```text
saved State in EEPROM
        vs.
currently edited State in RAM
```

The manual repeatedly emphasizes the same user-facing rule: programming edits are **not persistent until Store is explicitly performed**.

This is an important architectural behavior, not merely a UI convention.

## 9.4 Store, Recall, Revert

A faithful implementation should support at least these semantics:

```text
Recall current State:
    discard current transient edits for selected state and restore saved content

Store current State:
    persist the selected editable state

Recall Bank:
    restore the selected bank's saved states

Store All Banks:
    persist all States plus global/persistent settings and current Leading Tempo

Revert via Select Bus:
    reload dirty material/settings from the persistent representation
```

Exact EEPROM write latency does not matter in a virtual implementation unless hardware timing compatibility is explicitly desired.

---

# 10. Factory States

**Confidence: A/B.**

The first sixteen factory States are stored as flash tables and reproduced by the factory initialization routine.

| Slot | Recovered channel relationships |
|---:|---|
| 1 | ×1, ×1, ×1, ×1, ×1, ×1 |
| 2 | ÷1, ÷2, ÷4, ÷8, ÷16, ÷32 |
| 3 | ÷2, ÷3, ÷5, ÷7, ÷11, ÷13 |
| 4 | ÷1, ÷2, ÷3, ÷4, ÷5, ÷6 |
| 5 | ÷2, ÷4, ÷6, ÷8, ÷10, ÷12 |
| 6 | ÷3, ÷5, ÷7, ÷9, ÷11, ÷13 |
| 7 | ÷2, ÷3, ÷5, ÷8, ÷13, ÷21 |
| 8 | ×1, ×2, ×4, ×8, ×16, ×32 |
| 9 | ×2, ×3, ×5, ×7, ×11, ×13 |
| 10 | ×1, ×2, ×3, ×4, ×5, ×6 |
| 11 | ×2, ×4, ×6, ×8, ×10, ×12 |
| 12 | ×3, ×5, ×7, ×9, ×11, ×13 |
| 13 | ×2, ×3, ×5, ×8, ×13, ×21 |
| 14 | ×2, ×3, ×4, ÷2, ÷3, ÷4 |
| 15 | all ÷1.5; phase bytes `0,1,2,3,4,5` |
| 16 | ÷2, ÷2.25, ÷2.5, ÷2.75, ÷3, ÷3.25 |

States 17–64 initialize to unity ratios, zero phase, all six outputs enabled, and no MOD-enabled channels.

Slot 15 is especially useful as a phase validation fixture because it proves that the factory image intentionally uses distinct phase codes on six equal-rate clocks.

---

# 11. State selection

## 11.1 Four Banks of sixteen

**Confidence: A/B.**

There are four Banks, each containing sixteen States.

Absolute State numbers are `0..63` internally; panel language is usually `1..64` conceptually and `1..16` within a Bank.

## 11.2 Absolute base plus relative Gate offset

**Confidence: A/C semantic naming.**

The firmware maintains separate quantities corresponding to:

- a Bank/base State selection
- a State Gate step counter
- a final State index formed by adding them modulo sixteen within the Bank

The behavior is well captured by:

```cpp
slot = (baseSlot + gateOffset) & 0x0F;
state = bankBase + slot;
```

When the absolute State CV/panel selection changes, the gate-step offset is reset.

On each State Select Gate rising edge:

```cpp
gateOffset = (gateOffset + 1) & 0x0F;
```

This directly explains the manual's rule that State CV is **absolute and higher priority**, while State Select Gate is **relative stepping**.

Example:

```text
base slot = 5, offset = 0 -> slot 5
Gate edge                  -> slot 6
Gate edge                  -> slot 7
CV moves to slot 12         -> base 12, offset reset -> slot 12
Gate edge                  -> slot 13
```

## 11.3 State CV quantization

**Confidence: A for digital algorithm; D for exact real-world calibration values.**

State CV is the only active ADC channel identified in the application (`AN3 / RA3`).

The manual defines the input range as **0–5 V**.

The firmware does not simply divide the ADC range into equal bins. It builds sixteen calibrated thresholds and applies hysteresis based on the previous selection.

Given adjacent thresholds `T[j-1]` and `T[j]`:

```text
margin = floor((T[j] - T[j-1]) / 3)
```

and the preceding selection determines which side of that margin is retained.

A virtual implementation that is not emulating calibration hardware can use sixteen ideal regions, but should preserve hysteresis if physical-style behavior is desired.

## 11.4 Programming-page freeze

**Confidence: B.**

While several programming pages are active, CV/Gate State changes are deliberately deferred/ignored until the programming operation finishes or the page is exited.

This applies to Human, Machine, Phase, Mute, and MOD editing behavior described in the manual.

This is musically useful: entering a programming mode can temporarily freeze externally modulated State selection while editing.

## 11.5 Interaction with voltage-controlled tempo

When Leading Tap Tempo is disabled, the State control/CV is reassigned to Leading Tempo. State selection then remains available through State Gate where routing permits it and through Select Bus.

This means the physical State control has two mutually exclusive functional domains:

```text
Tap Tempo enabled   -> State CV/pot selects State
Tap Tempo disabled  -> State CV/pot controls Leading Tempo
```

---

# 12. MOD membership

## 12.1 Stored MOD mask

**Confidence: A/B.**

Each State contains a six-bit MOD membership mask.

MOD membership is therefore part of the State's stored musical configuration.

The specific behavior applied to MOD-enabled channels is selected globally through the Program Edit page:

```text
Shift mode
Run/Stop mode
Momentary/Toggled handling
```

## 12.2 Visual behavior

Manual semantics:

```text
Active + MOD disabled = blue
Muted  + MOD disabled = red
Active + MOD enabled  = purple
Muted  + MOD enabled  = pink
```

The channel LEDs continue to reflect programmed timing even in muted/MOD states.

---

# 13. Shift

## 13.1 Core semantics

**Confidence: B.**

Shift moves **Variable Clock parameter values among eligible channels**. It does not shift physical mute state.

The manual describes it explicitly as a shift register for Variable Clock values.

Eligibility:

```text
channel must be MOD enabled
channel must not be muted
```

At least two eligible channels are necessary for an audible rotation.

A Shift happens immediately on a rising control edge.

## 13.2 Runtime representation

The firmware architecture strongly favors treating Shift as transient source/routing state rather than destructively rewriting the stored State on each edge.

Recommended model:

```cpp
// Canonical identity mapping after State load
for (int c = 0; c < 6; ++c)
    shiftSource[c] = c;
```

For eligible channel list:

```text
E = [c0, c1, ... cN]
```

CW Shift:

```cpp
rotate timing assignments one direction across E;
```

CCW Shift:

```cpp
rotate opposite direction across E;
```

Random / RODENT:

```cpp
reassign eligible timing-source mappings using the firmware PRNG;
```

Muted channels remain fixed.

## 13.3 State-change behavior

**Confidence: B.**

Leaving a State and later returning restores the original Variable Clock positions.

TEMPI 71's changelog explicitly clarifies that Shifted and Run/Stopped channel positions are reverted on State change.

Therefore a State load should reset transient Shift routing to canonical identity before applying the new State.

## 13.4 Simultaneous Shift + Run/Stop

**Confidence: B.**

If Shift and Run/Stop are both active:

```text
MOD input             -> Run/Stop
State Select Gate     -> Shift
```

The State Select Gate no longer performs State stepping in that configuration.

State selection must then use State CV/panel control or Select Bus, subject to the voltage-controlled-tempo mode caveat.

---

# 14. Run/Stop

Run/Stop is not equivalent to ordinary mute. It is a transient timing operation that can deliberately alter phase relationships.

## 14.1 Normal Run/Stop

**Confidence: B.**

MOD-enabled channels:

```text
MOD high -> Run
MOD low  -> Stop at 0 V
```

MOD-disabled channels are unaffected.

The key behavior is the Run transition:

**A channel starts immediately when commanded to Run, regardless of its previously scheduled phase relative to Leading Tempo.**

That start instant becomes a temporary phase displacement preserved until a State change or a subsequent Stop/reset condition.

Thus a faithful model needs runtime phase state such as:

```cpp
runtimePhaseOffset[channel]
```

rather than merely:

```cpp
output &= runGate;
```

## 14.2 Run/Stop All

**Confidence: B.**

In Run/Stop All:

```text
MOD high -> all channels Run
MOD low  -> all channels Stop
```

On Run:

- MOD-enabled channels start immediately and can acquire/preserve an induced phase displacement.
- MOD-disabled channels restart aligned to Leading Tempo.

This makes Run/Stop All useful as a composition-level transport/reset behavior.

## 14.3 Alternate Run/Stop

**Confidence: B.**

Alt Run/Stop divides channels by MOD membership:

```text
MOD high:
    MOD-enabled    -> Run
    MOD-disabled   -> Stop

MOD low:
    MOD-enabled    -> Stop
    MOD-disabled   -> Run
```

The manual states that channels start immediately regardless of the prior phase relationship and preserve the resulting offset until State change or Stop/reset behavior.

## 14.4 Momentary versus Toggled

**Confidence: B.**

Momentary mode uses actual gate level:

```text
high -> Run condition
low  -> Stop condition
```

Toggled mode changes the logical MOD state on each rising edge:

```cpp
onModRisingEdge()
    logicalModState = !logicalModState;
```

The firmware samples both raw MOD level and a separately latched/toggled representation, consistent with these two modes.

## 14.5 Shift after Run/Stop

TEMPI 71's changelog states that when Run/Stopped channels are subsequently Shifted, their Run/Stop-derived timing offset is preserved until the State or timing of that channel changes.

This implies that Shift should move timing assignments while preserving an orthogonal runtime phase displacement rather than recomputing everything from stored phase alone.

A useful abstract separation is:

```cpp
StoredTiming   timingSource[6];
RuntimeOffset  runStopOffset[6];
```

---

# 15. Human Programming

## 15.1 Known purpose

**Confidence: B.**

Human Programming converts performed button taps into the same two conceptual parameters used by Machine Programming:

```text
ratio
phase
```

At least two taps are required.

The second tap can cause the existing channel timing to be replaced as soon as the firmware can safely commit it.

Multiple channels may be Human-programmed concurrently.

State changes are ignored while Human Programming is active.

## 15.2 Resolution modes

The three documented Human Resolution modes are:

| Mode | Meaning |
|---|---|
| 100% | strict; equivalent in concept to Coarse Machine ratio/phase relationships |
| 50% | default intermediate resolution |
| 25% | freer; approaches Fine Machine ratio/phase resolution |

This gives a strong target for future decompilation of `human_programming_service` at `0x340A`.

Conceptually, the routine must perform something like:

```text
capture tap intervals and absolute tap timing
            |
            +--> estimate requested channel/master frequency relationship
            |
            +--> estimate phase relationship to Leading Tempo
            |
            +--> quantize ratio according to Human Resolution
            |
            +--> quantize phase according to Human Resolution
            |
            +--> update stored editable ratio/phase
```

## 15.3 What remains unresolved

**Confidence: D for exact quantizer.**

The precise laws still needing reduction are:

- tap rejection/debounce windows
- how many recent tap intervals are combined
- whether interval estimation is weighted or averaged
- exact ratio candidate search
- exact phase-error metric
- exact 50% resolution grid
- exact handling of ambiguous multiplier versus divisor candidates
- exact commit timing after a new Human value is inferred

For a faithful clone, this is one of the highest-value remaining reverse-engineering targets.

---

# 16. Machine Programming

## 16.1 Coarse ratio programming

**Confidence: A/B.**

The panel behavior maps directly to the recovered encoding.

Divisor:

```text
hold PGM_A
press channel n times (1..32)
-> divisor n
```

Multiplier:

```text
hold PGM_B
press channel n times (1..32)
-> multiplier n
```

The opposite PGM button can be used while holding the primary button to double the entered integer value repeatedly.

For example:

```text
÷2 -> ÷4 -> ÷8 -> ÷16 -> ÷32
```

maps directly onto ratio codes:

```text
-4 -> -12 -> -28 -> -60 -> -124
```

## 16.2 Fine ratio programming

Holding a channel while pressing:

```text
PGM_A -> slower / Fine decrement
PGM_B -> faster / Fine increment
```

is naturally represented as decrementing/incrementing the recovered ratio code by one.

## 16.3 Machine Phase

See Section 7. The front-panel interaction is documented, while the exact byte update for every coarse/fine case should be finished from the UI state machine before claiming bit-exactness.

---

# 17. Copy, Paste, and Mutate

## 17.1 Copy/Paste

**Confidence: A/B.**

State and Bank editing maintain a separate copied source that can later be pasted or mutated.

Paste copies the source material exactly into the destination editable RAM cache. It does not inherently imply persistence; explicit Store is still required.

## 17.2 Mutation PRNG

**Confidence: A.**

TEMPI maintains an eight-byte random state with recurrence:

```text
state = (state * 0x5851F42D4C957F2D + 1) mod 2^64
```

The observed returned random byte is:

```text
result = (state >> 49) & 0xFF
```

This byte extraction is unusual but repeatedly verified against the recovered routine.

## 17.3 Mutation perturbations

**Confidence: A/C.**

The mutation routine generates two signed perturbations using modulo reduction:

```text
ratioDelta = (random % 7) - 3   // nominal range -3..+3
phaseDelta = (random % 5) - 2   // nominal range -2..+2
```

These values are applied during the State/Bank mutation process.

Because the random source has 256 equiprobable byte values, modulo 7 and modulo 5 introduce slight statistical bias. A deterministic clone should reproduce this rather than replacing it with an ideal uniform distribution.

Exact clipping/normalization at every ratio/phase boundary and the initial seed provenance remain worthwhile final tracing tasks.

---

# 18. Select Bus

## 18.1 Direction and physical concept

**Confidence: A/B.**

TEMPI 71's application configures UART receive and does not implement Select Bus transmission.

The manual independently states that TEMPI receives Select Bus messages but does not transmit them.

The Select Bus uses the Eurorack bus board's otherwise commonly unused internal CV line and is **active low**.

Official protocol rate:

```text
31,250 bit/s
```

The manual provides a reference transmit/receive circuit. A virtual implementation does not need to emulate electrical inversion unless interoperating with physical bus hardware.

## 18.2 Recovered parser

**Confidence: A.**

Parser entry: `0x71AA`.

Accepted status bytes include:

```text
C0
F0
F4
F7
```

The proprietary message header is:

```text
00 02 2D
```

The receive ring is 460 bytes.

The Timer2 ISR polls UART RX and enqueues bytes; the foreground parser consumes queued data.

## 18.3 State Select

```text
C0 ss
```

where:

```text
00 <= ss <= 3F
```

requests State `ss`.

This is an exact `C0` parser path; do not assume generic MIDI channel-status behavior or running status.

## 18.4 State Save / Store dispatch

Official message form:

```text
F4 ss    // State Save, ss 0..63
F4 40    // Save All
```

The recovered `F4` handler dispatches values below 64 to the state copy/save path and values in the next range to the persistent-store path, matching the official semantics.

## 18.5 Proprietary framed commands

Frame:

```text
F0 00 02 2D [command...] F7
```

Recovered commands:

```text
F0 00 02 2D 00 ss F7   -> MESH OFF for State ss
F0 00 02 2D 01 ss F7   -> MESH ON for State ss
F0 00 02 2D 02 F7      -> Default current State
F0 00 02 2D 03 F7      -> Revert
```

These byte sequences match the official Select Bus technical page in the TEMPI manual.

## 18.6 MIDI Clock is not the scheduling mechanism here

The recovered parser does not interpret `F8` as a master scheduling clock. An interleaved `F8` is ignored by this parser path.

Select Bus protocol similarity to MIDI framing should therefore not be mistaken for ordinary MIDI Clock synchronization.

---

# 19. Mesh / Multi-Paste semantics

## 19.1 Mesh bitmap

**Confidence: A for bitmap and commands; B for musical meaning.**

TEMPI maintains an eight-byte / 64-bit Mesh bitmap.

The official René Select Bus documentation defines Mesh/Multi-Paste behavior at a system level:

```text
MESH enable/disable -> include/exclude a State from propagated changes
M-PASTE              -> copy current State material to selected Mesh destinations
STORE                -> persist programmable parameters
REVERT               -> restore the last Stored version
```

This maps naturally onto TEMPI's cached State architecture and dirty-state bitmap.

Conceptual model:

```text
persistent States
      |
      v
editable 64-State RAM cache
      |
      +--> local edits
      |
      +--> Mesh propagation to selected destinations
      |
      +--> dirty bitmap
      |
      +--> explicit Store -> persistence
      |
      +--> Revert -> restore persistent version
```

The recovered parser and bitmap implementation make Mesh a real first-class internal feature, not merely behavior external to TEMPI.

---

# 20. Input and control mapping

## 20.1 MCU-side signal mapping

**Confidence: A/C semantic labels.**

| Panel function | MCU resource |
|---|---|
| Leading Tempo input | `RA4` |
| MOD input | `RA0` |
| State Select Gate | `RA5` |
| State CV / combo control | `AN3 / RA3` |
| Select Bus RX | `RC7 / RX1` |
| Channel button 1 | `RC2` |
| Channel button 2 | `RC5` |
| Channel button 3 | `RD7` |
| Channel button 4 | `RC1` |
| Channel button 5 | `RC6` |
| Channel button 6 | `RD6` |
| PGM_A | `RC0` |
| PGM_B | `RE2` |

These are MCU-side assignments. They do not reveal jack conditioning circuitry, Schmitt thresholds, overvoltage protection, external inversion, or exact electrical levels.

## 20.2 ADC behavior

The ADC wrapper is nonblocking and returns the most recent completed result. `0xFFFF` is used as an invalid/no-result sentinel in the quantizer path.

Only AN3 is enabled as the examined analog control channel.

---

# 21. LED subsystem

**Confidence: A for serial interface; C for full color-channel naming.**

The firmware software-shifts a 16-bit LED word using:

```text
RD5 -> data
RD2 -> clock
RD4 -> latch pulse
```

This establishes a serial LED-driving interface but does not by itself identify the exact external driver chip.

Additional GPIO participate in indicator control, but the entire LED/color wiring matrix has not yet been semantically labeled.

A software reimplementation can reproduce user-facing LED colors from documented mode semantics without copying the original electrical multiplexing.

---

# 22. State/UI interaction matrix

The following is a behavioral implementation guide.

| Context | State CV/Gate changes | MOD processing | Channel timing edits |
|---|---|---|---|
| Normal performance | active | active | normal |
| Human Programming | deferred/ignored until completion | effectively frozen during programming | Human tap capture |
| Machine ratio entry | deferred/ignored while editing | frozen during programming | coarse/fine ratio edit |
| Phase page | ignored while page active | normal behavior should be treated cautiously during edits | phase edit |
| Mute page | ignored while page active | existing timing continues | mute toggles; timing remains programmable |
| MOD page | ignored while page active | MOD membership/settings edited | internal clock still visible via LEDs |
| Shift + Run/Stop simultaneously | State Gate reassigned to Shift | MOD jack handles Run/Stop | State CV/Select Bus remain selection paths where available |
| Tap Tempo disabled | State CV/pot controls Leading Tempo | normal | State selection constrained to other sources |

The exact timing of page entry/exit, double-press recognition, hold thresholds, and debounce should be recovered from `button_ui_service()` if exact hardware UI emulation is required.

---

# 23. Recommended software timing model

A faithful virtual implementation should *not* attempt to reproduce PIC18 instruction timing unless hardware-cycle emulation is explicitly the goal.

Instead, preserve musical invariants:

1. One shared Leading Tempo timeline.
2. Integer/rational ratio representation.
3. Recovered truncation behavior where ratios are converted to timing.
4. Rational realignment information.
5. Separate stored phase and runtime phase offsets.
6. Mute as output suppression, not clock destruction.
7. Shift as transient timing assignment/permutation.
8. Run/Stop as a runtime phase-producing operation.
9. Explicit state persistence rather than autosave.
10. Edge-driven State Gate/MOD behavior.

A useful VCV Rack architecture would be:

```cpp
class TempiCore {
public:
    LeadingClockTracker master;
    TempiChannel channels[6];
    StateManager states;
    StateSelector selector;
    ModRouter mod;
    HumanProgrammer human;
    SelectBusParser selectBus;
};
```

Each `TempiChannel` should separate:

```cpp
struct TempiChannel {
    // stored/editable musical parameters
    int8_t ratioCode;
    uint8_t phaseCode;
    bool enabled;
    bool modEnabled;

    // runtime-derived parameters
    int64_t nominalHalfPeriod;
    int64_t nominalPhaseOffset;
    int sourceChannel;
    int64_t runStopPhaseOffset;
    bool running;

    // edge scheduler
    int64_t countdown;
    bool level;
};
```

Do not collapse `nominalPhaseOffset`, `runStopPhaseOffset`, and Shift routing into one parameter; doing so makes TEMPI 71's documented transition semantics difficult to reproduce correctly.

---

# 24. Suggested event processing order for a virtual implementation

This is an implementation recommendation based on the recovered architecture, not a claim about exact PIC18 instruction ordering at every boundary.

At each host sample/block:

```text
A. Detect input edges
   - Leading Tempo
   - MOD
   - State Gate
   - button gestures

B. Process control-domain events
   - Select Bus bytes/messages
   - State CV quantizer / State base changes
   - State Gate stepping or Shift reassignment
   - Human/Machine edit operations
   - MOD Run/Stop mode changes

C. If stored timing parameters became dirty
   - recompute half-period
   - recompute nominal phase
   - recompute rational alignment length
   - schedule transition through synchronization policy

D. Advance master timeline

E. Advance six channel timing records

F. Apply transient routing/Run-Stop state

G. Apply mute/output-enable mask

H. Convert clock edges to either 50% output or 10 ms trigger output

I. Update UI/LED representation
```

For maximum fidelity, timing changes should eventually use a port of the history/commit behavior in `0x234A` rather than an immediate arbitrary phase reset.

---

# 25. Minimum faithful feature set

An implementation can reasonably claim high behavioral fidelity only if it includes all of the following:

- Six simultaneous channels.
- Leading Tempo source and external-period tracking.
- Exact quarter-step ratio encoding behavior.
- Integer Coarse and quarter-step Fine multiplier/divisor programming.
- Recovered divider-vs-multiplier phase arithmetic.
- Rational alignment / noninteger relationship handling.
- 50% clock and 10 ms trigger output modes.
- Mute that preserves internal timing.
- 64 editable States in four Banks.
- Explicit Store/Recall semantics.
- State CV absolute selection plus Gate-relative stepping.
- MOD membership stored per State.
- Shift CW/CCW/Random with muted-channel exclusion.
- Run/Stop, Run/Stop All, and Alternate Run/Stop.
- Momentary and Toggled MOD handling.
- Transient Run/Stop phase displacement.
- Shift/Run-Stop combined routing.
- Copy/Paste/Mutate.
- Select Bus receive behavior or a software-equivalent message interface.
- Mesh bitmap semantics.

Human Programming can initially be implemented as a documented approximation if clearly labeled, because its exact quantizer remains the largest high-value behavioral unknown.

---

# 26. Acceptance tests

## 26.1 Ratio fixtures

With `masterHalfPeriod = 8000`, verify:

```text
r=-124 -> 256000
r=-8   -> 24000
r=-4   -> 16000
r=-2   -> 12000
r=0    -> 8000
r=1    -> 6400
r=4    -> 4000
r=8    -> 2666
r=124  -> 250
```

Use integer truncation toward zero.

## 26.2 Phase fixture

With:

```text
H = 8000
r = -8   // ÷3, channel half-period 24000
p = 1
```

expected recovered offset is:

```text
4000 ticks
```

not 12000.

This is an excellent regression test against accidentally normalizing divider phase to the divided channel period.

## 26.3 Factory States

Factory reset should reproduce all sixteen Bank-A patterns listed in Section 10.

Particularly check:

```text
State 15 ratio codes = [-2,-2,-2,-2,-2,-2]
State 15 phase codes = [0,1,2,3,4,5]
State 16 ratio codes = [-4,-5,-6,-7,-8,-9]
```

## 26.4 Mute

1. Run a channel at ×1.
2. Mute it.
3. Internal edge timing/LED should continue.
4. Change its ratio while muted.
5. Unmute.
6. Output should reflect the newly programmed timing rather than restart an abandoned oscillator.

## 26.5 Shift

Given active, MOD-enabled channels 2, 3, 5:

```text
before: 2=A, 3=B, 5=C
CW shift -> assignments rotate among [2,3,5]
```

Muted eligible channels must be excluded.

Changing State and returning must restore original stored assignments.

## 26.6 Run/Stop

For a MOD-enabled channel whose next canonical edge is in the future:

1. Stop it.
2. Assert Run at an off-grid instant.
3. Channel starts immediately.
4. New temporary phase relationship persists.
5. Stop or State change clears/replaces the transient relationship according to mode.

A simple output gate without runtime phase state should fail this test.

## 26.7 State selector

Set base slot to 4:

```text
Gate -> 5
Gate -> 6
move CV/base to 10 -> 10
Gate -> 11
```

The absolute base change resets the relative stepping history.

## 26.8 Select Bus

Verify exact parsing of:

```text
C0 25
```

as State request 37 decimal.

Verify:

```text
F0 00 02 2D 01 07 01 3F 00 07 F7
```

leaves only Mesh State 63 enabled from those two target States.

Do not interpret an interleaved `F8` as timing input in this parser.

---

# 27. High-value remaining reverse engineering

The behavioral model is now strong enough to implement TEMPI's overall architecture, but the following areas remain the most valuable unfinished work.

## 27.1 Human Programming quantizer — `0x340A`

Goal: derive an executable function:

```cpp
TapHistory + LeadingHistory + Resolution
    -> ratioCode + phaseCode
```

This would remove the largest remaining approximation in a software clone.

## 27.2 Timing commit/synchronization — `0x234A`

Goal: reproduce exactly how a new ratio/phase is transitioned into a running clock without arbitrary discontinuity.

This is likely the key to making a clone *feel* indistinguishable during live programming rather than merely matching steady-state ratios.

## 27.3 Leading Tempo filtering — `0x4B16`

Goal: establish:

- edge-history depth
- smoothing
- clock-loss policy
- fast-to-slow transition behavior
- re-lock behavior

## 27.4 Voltage-controlled tempo law — `0x627C` and calibration helpers

Goal:

```text
ADC/calibration -> internal Leading period
```

including interpolation through the recovered tempo-control table.

## 27.5 Exact phase edit transforms — `0x11C0`

Goal: map every Coarse/Fine phase button gesture to `phaseByte`, including wrap/reset behavior around noninteger ratios.

## 27.6 Mutation normalization and seed path — `0x6BFA`, `0x8446`

Goal: finish deterministic behavior at extreme ratio/phase values and establish how the initial/random seed is produced.

## 27.7 Physical hardware measurements

A real unit could settle several remaining unknowns rapidly:

- exact MCU marking
- oscillator frequency
- Timer2 tick period
- actual gate thresholds
- output voltage and impedance
- 10 ms trigger tolerance
- channel-to-channel GPIO skew after output buffering
- State CV transfer curve

---

# 28. What should *not* be copied literally in a virtual implementation

The goal should be musical/behavioral fidelity, not accidental embedded constraints.

A VCV Rack or native software implementation does **not** need to reproduce:

- the physical 1 KiB EEPROM's column-major layout internally
- sequential LATB bit writes
- PIC18 banked RAM
- EEPROM interrupt blocking during writes
- UART ring-buffer limitations
- hardware LED shift-register signaling
- unresolved oscillator/timer assumptions

Those should only be recreated when necessary for import/export compatibility or hardware-coupled testing.

What **should** be preserved is the musical state machine:

```text
shared master timing
rational ratio representation
phase behavior
runtime transformation semantics
explicit persistence
state-selection priority
MOD routing
```

---

# 29. Recommended implementation priority

For a practical high-fidelity software TEMPI:

### Phase 1 — deterministic clock core

Implement:

- Leading master period
- exact ratio code
- six channel timers
- 50% / trigger mode
- mute
- State storage

### Phase 2 — phase and rational synchronization

Implement:

- recovered phase arithmetic
- alignment factors/LCM
- State transition timing policy

### Phase 3 — performance transformations

Implement:

- MOD membership
- Shift
- Run/Stop modes
- transient phase offsets
- combined Shift + Run/Stop routing

### Phase 4 — memory/workflow

Implement:

- four Banks / 64 States
- Store/Recall/Revert
- Copy/Paste
- deterministic Mutate
- State CV/Gate selection semantics

### Phase 5 — Human Programming and external sync refinement

Implement or finish reverse engineering:

- Human resolution quantizer
- Leading Tempo lock/filter behavior
- voltage-controlled tempo law

### Phase 6 — Select Bus

Implement:

- receive parser
- State Select
- Mesh
- Default
- Revert
- Store/Multi-Paste interoperability

---

# 30. Compact reference model

The following pseudocode captures the architecture without pretending to solve the remaining transition details.

```cpp
void Tempi::processSample(float dt) {
    // 1. Input/control edge detection
    detectLeadingEdge();
    detectModEdge();
    detectStateGateEdge();
    updateStateControlADC();
    serviceSelectBus();
    serviceButtons();

    // 2. Master timing
    master.service(dt);

    // 3. State selection
    if (!ui.blocksStateSelection()) {
        selector.service();
        if (selector.stateChanged())
            loadState(selector.state());
    }

    // 4. Programming
    if (human.active())
        human.service(master, channels);

    ui.serviceEdits(states.editableCurrent());

    // 5. Derived timing
    for (int c = 0; c < 6; ++c) {
        if (channels[c].timingDirty) {
            channels[c].nominalHalfPeriod =
                ratioToHalfPeriod(channels[c].ratioCode,
                                  master.halfPeriod);

            channels[c].nominalPhaseOffset =
                phaseToOffset(channels[c].ratioCode,
                              channels[c].phaseCode,
                              master.halfPeriod,
                              channels[c].nominalHalfPeriod);
        }
    }

    recomputeAlignmentIfDirty();
    commitTimingChangesWithSyncPolicy();

    // 6. MOD behavior
    mod.service(*this);

    // 7. Runtime clocks
    for (int c = 0; c < 6; ++c)
        channels[c].advance(master, dt);

    // 8. Output policy
    for (int c = 0; c < 6; ++c) {
        bool logical = channels[c].running
                    && states.current().isEnabled(c)
                    && channels[mod.sourceFor(c)].clockLevel;

        outputs[c] = pulseMode[c]
            ? triggerGenerators[c].renderFromEdges(logical, dt)
            : logical;
    }

    // 9. LEDs are based on internal timing and mode state,
    // not merely the physical output gate.
    updateIndicators();
}
```

The largest remaining gap in this model is `commitTimingChangesWithSyncPolicy()`: the original firmware's history-based commit behavior deserves its own dedicated reconstruction.

---

# 31. Confidence summary

| Area | Confidence | Status |
|---|---|---|
| WAV transport recovery | A | Exact |
| PIC18 architecture | A | Established |
| Exact MCU suffix/package | C | Candidate only |
| Six output GPIO mapping | A | Established |
| Panel input MCU mapping | A/C | Strong |
| Ratio encoding | A | Solved |
| Ratio arithmetic/clamps | A | Solved |
| Rational alignment | A | Solved |
| Stored phase arithmetic | A | Solved |
| Exact live phase commit behavior | C/D | Partial |
| State contents | A | Solved |
| EEPROM State layout | A | Solved |
| Dirty/save/revert architecture | A/B | Strong |
| Factory States | A/B | Solved |
| State Gate stepping | A/B | Strong |
| State ADC hysteresis | A | Solved digitally |
| Physical State CV calibration | D | Unknown |
| Shift semantics | B/C | Strong |
| Run/Stop semantics | B/C | Strong |
| Human Programming purpose | B | Strong |
| Human Programming exact quantizer | D | Open |
| Mutation PRNG | A | Solved |
| Mutation deltas | A/C | Strong |
| Select Bus parser/protocol | A/B | Solved |
| Mesh bitmap | A/B | Solved |
| Leading external period capture | A/B | Strong |
| Leading filtering/re-lock policy | C/D | Partial |
| Voltage-controlled tempo curve | C/D | Partial |
| Physical clock output voltage | D | Not recovered |

---

# 32. Final engineering interpretation

TEMPI 71 is a compact but sophisticated **rational timing computer**.

Its essential abstraction is:

```text
                    Leading Tempo
                         |
              rational master timeline
                         |
        +----------------+----------------+
        |                |                |
   ratio/phase      ratio/phase      ratio/phase ... x6
        |                |                |
        +-------- rational alignment -----+
                         |
                  transient routing
                 SHIFT / RUN-STOP
                         |
                  output enable/mute
                         |
               clock or 10 ms pulse
```

Around that timing core is a performance-memory system:

```text
64 editable States
    |
    +-- ratio[6]
    +-- phase[6]
    +-- active mask
    +-- MOD mask
    |
    +-- dirty tracking
    +-- Store / Recall / Revert
    +-- Copy / Paste / Mutate
    +-- Mesh / Multi-Paste
```

The strongest lesson from combining firmware and documentation is that many TEMPI behaviors are intentionally **non-destructive temporal transformations**. Mute does not kill the underlying timing. Shift does not need to rewrite the State. Run/Stop can generate a temporary phase relationship. State CV establishes an absolute anchor while State Gate walks relatively from it. Human and Machine Programming converge onto the same ratio/phase state representation.

That architecture is the part worth preserving. It is what makes TEMPI more than a six-output clock divider/multiplier, and it is the foundation on which a faithful software implementation should be built.

---

# Appendix A — recovered function index

| Address | Analyst label | Principal role |
|---|---|---|
| `0x0808` | `timer2_interrupt` | real-time output/input/counter service |
| `0x11C0` | `button_ui_service` | panel state machine and editing gestures |
| `0x234A` | `clock_timing_commit_service` | transition/synchronization machinery |
| `0x340A` | `human_programming_service` | tap-to-ratio/phase processing |
| `0x3F32` | `led_ui_service` | LED/UI output behavior |
| `0x4B16` | `leading_tempo_service` | Leading clock acquisition/control |
| `0x4F66` | `mod_routing_service` | Shift and Run/Stop runtime routing |
| `0x5B66` | `build_adc_calibration_thresholds` | calibrated State/tempo ADC thresholds |
| `0x627C` | unresolved tempo/control helper | likely control interpolation path |
| `0x65DE` | `recompute_ratio_and_phase` | derived timer and phase values |
| `0x6BFA` | `paste_or_mutate` | State/Bank paste and mutation |
| `0x6E56` | `factory_reset_states` | factory initialization |
| `0x71AA` | `select_bus_parse_byte` | Select Bus receive parser |
| `0x78FC` | `state_adc_quantizer` | 16-way hysteretic State quantizer |
| `0x7A5C` | `state_selection_service` | State selection priority/stepping |
| `0x7D08` | `mesh_bitmap` | Mesh set/clear/query operations |
| `0x7E54` | `ratio_to_halfperiod` | exact ratio arithmetic |
| `0x7F8E` | `alignment_lcm_u16` | rational alignment LCM |
| `0x8446` | `lcg64_next_byte` | deterministic mutation PRNG |
| `0x8540` | `load_active_state_from_cache` | activate editable State |
| `0x871A` | `store_dirty_states` | persist modified State data |
| `0x88D4` | `select_bus_copy_or_store` | Select Bus copy/store dispatcher |
| `0x8B16` | `state_dirty_bitmap` | dirty-State bookkeeping |
| `0x8BCC` | `ratio_alignment_factor` | reduced rational denominator |
| `0x8DC2` | `store_global_settings` | global persistence |
| `0x8EE6` | `initialize_current_state` | default current State |
| `0x93B2` | `write_phase_byte` | EEPROM phase write helper |
| `0x93FC` | `recompute_alignment_length` | six-channel alignment length |
| `0x9618` | `write_ratio_byte` | EEPROM ratio write helper |
| `0x964A` | `reload_dirty_states` | Revert/reload |
| `0x9678` | `store_leading_tempo` | persist Leading Tempo |
| `0x96A6` | `write_enable_mask` | persist enable mask |
| `0x972A` | `eeprom_read_byte` | EEPROM read |

---

# Appendix B — recovered factory ratio bytes

```text
State  1:    0,   0,   0,   0,   0,    0
State  2:    0,  -4, -12, -28, -60, -124
State  3:   -4,  -8, -16, -24, -40,  -48
State  4:    0,  -4,  -8, -12, -16,  -20
State  5:   -4, -12, -20, -28, -36,  -44
State  6:   -8, -16, -24, -32, -40,  -48
State  7:   -4,  -8, -16, -28, -48,  -80
State  8:    0,   4,  12,  28,  60,  124
State  9:    4,   8,  16,  24,  40,   48
State 10:    0,   4,   8,  12,  16,   20
State 11:    4,  12,  20,  28,  36,   44
State 12:    8,  16,  24,  32,  40,   48
State 13:    4,   8,  16,  28,  48,   80
State 14:    4,   8,  12,  -4,  -8,  -12
State 15:   -2,  -2,  -2,  -2,  -2,   -2
State 16:   -4,  -5,  -6,  -7,  -8,   -9
```

State 15 phase bytes:

```text
0, 1, 2, 3, 4, 5
```

All factory States use `enableMask = 0x3F` and `modMask = 0x00` after initialization.

---

# Appendix C — source/page cross-reference

Official TEMPI manual topics used in this reconstruction:

| Topic | Manual page |
|---|---:|
| Factory States / settings | 8–9 |
| Quick reference | 10–11 |
| Leading Tempo | 12–13 |
| Human Programming | 13 |
| Machine multiplier/divisor | 14–15 |
| Machine Phase | 15–16 |
| Mute | 17 |
| MOD enable | 18 |
| Shift | 19 |
| Run/Stop | 20–21 |
| Shift + Run/Stop | 22 |
| State selection | 23 |
| Copy/Paste/Mutate | 24 |
| Bank Edit | 25–26 |
| Select Bus / Free-Follow | 26 |
| Clock Edit | 27 |
| Tips and interaction notes | 28 |
| Select Bus overview | 32 |
| Select Bus technical messages/schematic | 33 |
| Firmware changelog | 34 |

---

# Appendix D — provenance and reproduction notes

This specification is derived from the prior firmware recovery dossier. The most important supporting artifacts are:

```text
firmware/tempi71_recovered.hex
analysis/disassembly_reachable.asm
analysis/functions.csv
analysis/factory_states.csv
analysis/validation.json
docs/ALGORITHMS.md
docs/IO_MAP.md
docs/MEMORY_AND_STATES.md
docs/SELECT_BUS.md
```

The recovery validation included:

```text
1,743 ratio fixtures
42 six-lane phase fixtures
249 alignment-factor cases
4,096 ADC hysteresis cases
64 GPIO output patterns
64 random-generator seeds
64 EEPROM State-addressing cases
64 factory-state cases
7 Select Bus scenarios
```

All packaged assertions passed. The custom decoder and harness share instruction definitions, so this remains a strong internal validation rather than independent third-party certification.
