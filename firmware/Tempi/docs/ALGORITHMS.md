# Executable algorithm notes

The mathematical models in `tools/models.py` are extracted behavioral models, not original source and not a complete module. Each verified model has corresponding machine-code execution tests in `tools/validate_and_extract.py`.

## 1. Ratio to halfperiod — program 0x7E54

**Inputs:** signed 16-bit ratio at RAM `0x060`, signed 32-bit master halfperiod at RAM `0x062`. **Output:** 32-bit halfperiod at RAM `0x060`.

```python
h = signed32(master_halfperiod)
if r > 0:
    h = trunc_div(signed32(h * 4), r + 4)
elif r < 0:
    h = trunc_div(signed32(h * (4 - r)), 4)
if h >= 0x01000000:
    h = 0x00FFFFFF
if h < 200:
    h = 200
return h
```

`trunc_div` rounds toward zero. A port should avoid depending on host-language overflow rules: explicitly constrain intermediate values to signed 32-bit behavior.

| Ratio code | Musical ratio | Halfperiod for H=8000 |
|---:|---|---:|
| -124 | ÷32 | 256000 |
| -8 | ÷3 | 24000 |
| -4 | ÷2 | 16000 |
| -2 | ÷1.5 | 12000 |
| 0 | Unity | 8000 |
| 1 | ×1.25 | 6400 |
| 4 | ×2 | 4000 |
| 8 | ×3 | 2666 |
| 124 | ×32 | 250 |

The ratio code is not a signed count of extra clock pulses. The two sides of zero encode multiplier magnitude and divisor magnitude using different reciprocal relationships. A smooth linear interpolation of the code is consequently not a linear interpolation of frequency across the full span.

**Validation:** 249 codes at seven input halfperiods, totaling 1,743 fixtures. Inputs cover zero, subminimum values, ordinary periods, and the maximum clamped-scale value. See `ratio_test_vectors.csv`.

## 2. Phase offset — program 0x65DE

The ratio calculation is called for each dirty channel. The result goes to RAM `0x5BE + 4*c`. The phase computation then uses either the master halfperiod or that result:

```python
base = H if r < 0 else ratio_halfperiod(r, H)
offset = trunc_div(signed32(base * (phase_byte & 255) * 2), 4)
```

The output is stored at `0x520 + 4*c`. For a divisor, `p=1` means one quarter of a master full cycle. For a multiplier or unity, it means one quarter of the channel full cycle.

For example, at `H=8000`, ratio code `-8` produces a 24,000-tick channel halfperiod, but phase code 1 produces only a 4,000-tick offset. A naïve “quarter of that divided clock” model would instead produce 12,000 ticks and would be wrong for this routine.

**Validation:** seven ratio codes and six phase bytes, applied to all six channels through the complete recomputation routine. These 42 six-lane fixtures agree with the model. They verify the computed offset, not the entire real-time phase-update scheduler.

## 3. Rational alignment — programs 0x8BCC, 0x7F8E, 0x93FC

The per-channel alignment factor is:

```text
r < 0:  (4-r) / gcd(4-r, 4)
r >= 0: 4 / gcd(r+4, 4)
```

It is the reduced denominator of the output/master frequency ratio. Thus ÷3 requires three master cycles to return to a common phase, while ×1.25 requires four. Integer multiples have factor one.

The LCM helper combines factors. Its observed calling convention uses 16-bit arguments at RAM `0x01F` and `0x021`, with result at `0x01F`. A 32-bit product and integer remainder/division helpers implement the arithmetic. The foreground code stores a resulting master alignment length at RAM `0x5F7`, which the interrupt uses in its counter wrap logic.

**Validation:** all 249 ratio-factor cases; seven representative LCM pairs. This is not an exhaustive proof of behavior when the full six-way alignment length overflows its stored width.

## 4. State ADC hysteresis — program 0x78FC

The calibration builder populates 16 ascending uint16 thresholds at RAM `0x500`. The previous state is at `0x4E6`. The ADC wrapper returns the latest reading or the sentinel `0xFFFF`.

```python
if adc == 0xFFFF:
    return previous
j = first index whose threshold[j] is greater than adc
if no such index:
    return 15
if j == 0:
    return 0
margin = (threshold[j] - threshold[j-1]) // 3
if previous == j and adc <= threshold[j-1] + margin:
    return j-1
if previous == j-1 and adc <= threshold[j] - margin:
    return j-1
return j
```

Equality matters. The upper threshold search uses a strict comparison; the two hysteresis decisions include equality. The margin is a divide-by-three calculation, not a three-bit shift.

**Validation:** 16 previous states × 256 ADC test values = 4,096 fixtures. The ADC-read wrapper is substituted with a controlled value; thresholds are synthetic. This does not verify real-unit calibration or analog noise behavior.

## 5. Random generator — program 0x8446

The generator has eight bytes of state at `0x5B6–0x5BD`. It multiplies by `0x5851F42D4C957F2D`, adds one, and retains the low 64 bits.

The extraction loop loads 50 into its counter but decrements before each iteration, resulting in **49 shifts**. The code copies the low shifted byte to the return low byte and copies the third shifted byte to the return high byte. The latter is zero after a 49-bit shift of a 64-bit word. Hence:

```text
next = (old * 0x5851F42D4C957F2D + 1) & 0xFFFFFFFFFFFFFFFF
result = (next >> 49) & 0xFF
```

For seed 1, the next state is `0x5851F42D4C957F2E`. The routine returns `0x28`, not `0x2C28`. That difference is covered by the tests. The multiplier's use alone is not evidence for a complete PCG algorithm; no such algorithm name is assigned here.

Modulo 7 and modulo 5 consumers produce small signed mutation perturbations. Because the source has 256 outcomes, such modulo reductions are not perfectly uniform. This is a mathematical consequence of the observed byte extraction, not a measured musical bias.

**Validation:** 64 seeds, including zero, one, all ones, and deterministic pseudorandom test seeds. Both the next state and returned value match.

## 6. Interrupt engine and synchronization — partially reconstructed

The Timer2 interrupt performs a fixed, unrolled six-channel service. A conceptual outline is:

```text
drive previously prepared outputs
sample RA4; on a rising edge capture external-tempo timing
advance master countdown and master alignment counter
if UART receive flag: enqueue one byte
sample MOD level and update its rising-edge toggle
update source-related master-history snapshots
for each channel:
    detect source transition / choose synchronization history
    decrement its signed countdown
    when elapsed: toggle channel level and rotate duration fields
    compute its next pending output from source, enable, run gate, and level
sample active-low channel buttons and capture timing events
advance human timing counters
return from interrupt
```

The foreground timing service at `0x234A` updates timer parameters, histories, phase relationships, and transition guards. It is over 4 KB of reachable instructions. Its full state-transition behavior has not yet been reduced to executable high-level code.

A ten-entry timing-history structure at RAM `0x700` stores 12-byte snapshots, with related phase/cycle arrays elsewhere. Those histories are important evidence against treating clock updates as immediate frequency assignments with an arbitrary oscillator phase reset.

## 7. Human programming and MOD — traced, not fully specified

Human programming uses six elapsed/captured timer arrays and combines measured taps with ratio and phase quantization. The large service at `0x340A`, the UI service at `0x11C0`, and their helper routines are the next places to trace. The mere availability of a captured interval does not establish the precise tap acceptance, normalization, or phase-quantization law.

MOD processing at `0x4F66` consumes both a sampled gate and a latched toggle, maintains per-channel MOD enables, and operates through channel-source pointers and run gates. This establishes the architecture needed for shift and run/stop behavior, but not every mode's complete transition order.

For implementation work, preserve the distinction between a verified arithmetic function and an unfinished event-state machine. Replacing the latter with a plausible musical behavior may produce a useful module, but it would be an explicit design approximation rather than a recovered fact.
