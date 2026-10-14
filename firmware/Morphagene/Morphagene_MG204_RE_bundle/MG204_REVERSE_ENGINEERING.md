# Make Noise Morphagene MG204 reverse-engineering notes

## Result in one sentence

MG204's ordinary Gene Size is an output-time duration derived from a 12-bit,
splice-relative exponential curve. The full-counter-clockwise region is a
separate whole-splice mode. Morph is independently converted into a rational
Gene-launch interval and overlap-density schedule.

This report describes the `mg204.wav` distributed in Make Noise's MG204 update
archive. Addresses below use a flash base of `0x08020000`.

## Provenance and image recovery

| Item | Value |
|---|---|
| Update ZIP SHA-256 | `a653d8c67f8061fe6b2ffaf6a978584c02a8219eda2a0b690ca0f1272d380a36` |
| `mg204.wav` SHA-256 | `dc6cb0731ef21725c83c6d24c7cd2e98ad7c6ffccc55071ec3bf0c8e318894a8` |
| Deframed flash SHA-256 | `44037eb2cf24b5fc411135e487a6fa226498a8478db981659bb53341cb967609` |
| Flash range | `0x08020000..0x080483ff` (164,864 bytes) |
| Initial stack pointer | `0x20030000` |
| Reset handler | `0x08021415` (Thumb) |

The updater audio is mono, signed 16-bit PCM at 48 kHz. It contains one second
of silence followed by 732,636 symbols. Each symbol is exactly eight samples,
or one cycle of a 6 kHz carrier. The four carrier phases encode dibits:

| Phase | Dibit |
|---:|:---:|
| 45° | `10` |
| 135° | `00` |
| 225° | `01` |
| 315° | `11` |

Dibits are packed most-significant-bit first. The resulting 183,159-byte stream
has 1,508 leading zero bytes, then the sync word `99 99 99 99 CC CC CC CC`.
Its transport payload consists of 644 records:

- 256 data bytes;
- a four-byte, big-endian IEEE CRC-32 (all 644 verify);
- eight zero bytes, or 23 after every fourth record;
- the eight-byte sync word, except after the final record.

The final stream has 1,515 trailing zero bytes. Removing transport framing
yields the flat Cortex-M firmware image described above. `decode_mg204.py`
performs this complete recovery and validation.

## Gene Size control input

The Gene Size ADC state is at RAM `0x20021c7c`. The control scanner at
`0x080263e5` maintains a 64-conversion moving sum for this channel and exposes
the average by shifting right six bits. The usable value is therefore a
filtered 12-bit integer, here called `A`, in the range 0..4095.

The main DSP function begins at `0x08027519`. At `0x080278d2..0x08027956`, it
recomputes Gene Size when either:

- `abs(A - previous_A) > 32`, or
- the selected splice length has changed.

This creates roughly a 33-code control deadband. The curve itself has four-code
plateaus because it indexes with `A >> 2`.

The firmware sees one already-combined ADC channel for Gene Size. No separate
Gene Size pot and CV operands occur in this path, so their summing/attenuation
is upstream of the DSP calculation (most plausibly in the analog control
front-end and ADC path).

## Exact ordinary-mode curve

Let:

- `S` be the current splice length in 48 kHz samples;
- `A` be the filtered Gene Size ADC code;
- `B` be a folded splice span;
- `L` be the ordinary-mode Gene duration in output samples.

First, the firmware folds long splices into the interval at or below 12 seconds:

```text
B = float(S)
while B > 576000:
    B *= 0.5
```

Equivalently, `B = S / 2^h`, where
`h = max(0, ceil(log2(S / 576000)))`.

For `A > 199`:

```text
q = A >> 2
i = 1073 - q
x = LUT[i]
L = max(8, B * x * x * x)
```

The 1,024-entry float table begins at `0x08044d10`. Every entry matches the
single-precision rounding of:

```text
LUT[i] = 2 ** (i / 341 - 3),  i = 0..1023
```

Ignoring the firmware's intermediate float32 rounding, the full curve reduces
to:

```text
L = max(8, B * 2 ** ((150 - 3*floor(A/4)) / 341))
```

The actual implementation should use the stored float32 table and two float32
multiplications when bit-for-bit agreement matters.

Representative points, before the eight-sample floor and clock quantization:

| ADC `A` | LUT index | `L / B` | Duration for a 10 s splice |
|---:|---:|---:|---:|
| 200 | 1023 | 1.000000000 | 10.000 s |
| 256 | 1009 | 0.918169740 | 9.182 s |
| 512 | 945 | 0.621481286 | 6.215 s |
| 1024 | 817 | 0.284733323 | 2.847 s |
| 1536 | 689 | 0.130451319 | 1.305 s |
| 2048 | 561 | 0.059766618 | 597.7 ms |
| 2560 | 433 | 0.027382238 | 273.8 ms |
| 3072 | 305 | 0.012545244 | 125.5 ms |
| 3584 | 177 | 0.005747638 | 57.48 ms |
| 4000 | 73 | 0.003048317 | 30.48 ms |
| 4092–4095 | 50 | 0.002649402 | 26.49 ms |

The span folding is literal. For example, a 12.0-second splice uses a
12.0-second base, while a 12.01-second splice uses approximately a 6.005-second
base. Further discontinuities occur at successive powers of two above that
boundary. Whole-splice mode bypasses this behavior.

## Full-counter-clockwise endpoint

`A <= 199` branches at `0x08028824` and sets a dedicated whole-splice flag at
RAM `0x20021e14`. This is not merely the first point on the exponential curve.
Downstream traversal and wrapping use the selected splice boundaries while the
flag is set, producing the documented full-splice behavior.

The ordinary Gene-duration variable at `0x20021f90` is reused as an internal
step/chunk quantity in this mode. It is initialized from the splice length and,
when necessary, repeatedly halved until it is at most 5,000 samples
(`0x0802878c..0x080287b4`). That internal value must not be mistaken for the
audible Gene duration: the dedicated flag makes the full splice the boundary.

This creates a hard mode boundary between ADC codes 199 and 200:

- 0..199: whole selected splice;
- 200..4095: ordinary exponential Gene curve.

## Vari-Speed independence

The ordinary result `L` is stored at RAM `0x20021f90` as an output-time
quantity. Vari-Speed's signed playback increment is calculated separately and
drives the read heads. The scheduler's Gene-age/boundary state uses `L`, which
is why changing playback rate changes the source material traversed without
changing elapsed Gene time.

An earlier analysis mislabeled RAM `0x20021f48` as the playback increment. It
is actually the Morph launch-interval factor described below. Implementations
should therefore keep an explicit output-sample Gene timer rather than deriving
Gene lifetime from source distance.

## Morph schedule

The filtered Morph ADC is RAM `0x20021c6c`. Let `M = ADC / 4096`. The DSP uses:

```text
index = min(21, floor(M * 33.99))
```

It loads three parallel tables copied from flash into RAM:

- `0x200003e0[index]` → launch-interval factor, stored at `0x20021f48`;
- `0x20000358[index]` → overlap density, stored at `0x2002209c`;
- `0x200004f0[index]` → denominator/cycle length used by the rational scheduler.

The exact active entries are documented in `MORPH_PARAMETER_NOTES.md`. The
essential structure is:

- Morph 0 starts a new Gene every two Gene durations, leaving one Gene duration
  of silence;
- the first four stages reduce the launch interval through `2`, `3/2`, `4/3`,
  and `1`, reaching seamless 1/1 looping at ADC 362;
- subsequent stages use rational launch intervals below one Gene duration and
  reciprocal overlap densities, progressing through 5/4, 4/3, 3/2, ... up to
  four simultaneous Genes;
- the scheduler reaches its four-Gene plateau at ADC 2531 (about 61.8% of the
  ADC range), leaving the upper control region for spatial and pitch behavior.

The direct Morph value is smoothed per audio sample approximately as
`state += 0.01 * (target - state)`. Randomized panning begins in the code above
Morph `0.5`; upper-regime pitch variation is calculated from `Morph - 0.6`.
The three additional playback increments are the primary Vari-Speed increment
multiplied by live ratios at `0x20022124`. Defaults are `2.0`, `1.5`, and
`1.333333`, and options `mcr1..3` may replace them with magnitudes from 0.0625
through 16; negative values reverse the corresponding voice.

## Clock-present quantization

RAM `0x20021cb0` is set while a valid external clock is being tracked. When it
is one and the preliminary Gene duration exceeds eight samples, the code at
`0x0802ab6c..0x0802acd6` quantizes the result to musically regular fractions of
the *unfolded* splice length `S`.

The decision sequence is exact:

```text
if L >= (2/3) * S:
    L = S
else:
    candidates = S/2, S/3, S/4, S/6, S/8, S/12, S/16, ...
    L = first candidate <= preliminary_L
```

Candidate ratios are generated by alternately multiplying by `3/4` and `2/3`.
The stored `2/3` constant is `0x3f2aaa9f` (approximately
0.6666659713), not the correctly rounded binary32 value of exact 2/3, so an
emulator seeking exact threshold behavior should use the firmware constant.

## Envelope-related quantities

For ordinary Genes, the firmware also derives a half-length and reciprocal from
`L`. The half-length used by the overlap/envelope path is capped at 24,000
samples (0.5 seconds), while the ordinary Gene length itself is not thereby
capped. Very small computed lengths are floored to eight samples. A separate
diagnostic/special operating path caps a test span at 384,000 samples; it is not
the normal user Gene Size limit.

## Options structure recovered

The parsed options structure begins at RAM `0x200012a8`:

| Offset | Key |
|---:|:---|
| `+0x00` | `vsop` |
| `+0x08` | `pmod` |
| `+0x0c` | `omod` |
| `+0x14` | `cvop` |
| `+0x18` | `ckop` |
| `+0x1c` | `rsop` |
| `+0x20` | `pmin` |
| `+0x24` | `inop` |
| `+0x28` | `gnsm` |
| `+0x2c` | `mcr1` |
| `+0x30` | `mcr2` |
| `+0x34` | `mcr3` |

`0x20022124`, which might superficially resemble an options address in some
decompiler output, is instead the live Morph chord-ratio array.

The `cvop` field is tested at `0x08028f1e/0x08028f3a` and in the mirrored path
at `0x0802a1ec/0x0802a208`. Its enabled branch applies a ramp/smoothing update
of the approximate form `0.9*prior + 0.06*input*scaleA*scaleB`, clamped to
0..1. This is downstream modulation behavior and does not replace the Gene Size
ADC-to-duration equation above.

## Confidence and remaining uncertainties

High-confidence, directly verified facts:

- audio symbol timing, phase mapping, packing, framing, padding, and every CRC;
- flash base, image size, vector table, and reset handler;
- Gene Size ADC state, 64-conversion boxcar, threshold, deadband, table address,
  table contents, cube, long-splice folding, and eight-sample floor;
- whole-splice endpoint flag and its distinct downstream control flow;
- separation of output-time Gene scheduling from the Vari-Speed read heads;
- Morph's 22-stage rational launch/overlap schedule and upper-regime thresholds;
- clock-present quantization sequence;
- options structure offsets listed above.

Still requiring hardware measurement or a fuller peripheral reconstruction:

- the exact voltage-transfer function of the panel pot, CV input, and
  attenuverter before the combined ADC code;
- the ADC conversion cadence, needed to turn the 64-conversion boxcar into an
  exact smoothing time in milliseconds;
- user-visible consequences of the long-splice folding discontinuities under
  every clock/shift/record state;
- names and semantics for the remaining large playback state structure.

## Included files

- `decode_mg204.py`: complete WAV-to-flash decoder with CRC validation;
- `extract_gene_size.py`: verifies the LUT formula and exports all 4,096 ADC
  codes for a chosen splice duration;
- `gene_size_curve_10s.csv`: full curve for a 10-second splice;
- `mg204_flash_08020000.bin`: recovered flat flash image;
- `decompile_main_dsp.c`: automated decompiler output for the large DSP
  function (use as a navigation aid; its inferred types are often wrong).
