# Morphagene MG204 — audited extension

This pass brings the Morphagene bundle closer to the Mimeophon reference in
reproducibility and instruction-level support, and extends the DSP breakdown.
The transported application is fully recovered. The DSP reconstruction remains
partial; selected equations and tables now have runnable arithmetic checks.

## 1. Findings that change the earlier breakdown

1. **Clocked Gene Size was described incorrectly.** The firmware retains the
   current upper bin once the preliminary duration reaches the next lower
   boundary. It does not store the first lower candidate. A preliminary length
   of 60% of a splice becomes approximately 2/3, not 1/2.
2. **Morph's rational labels are musical descriptions, not exact ROM values.**
   Eleven of the 22 active launch entries differ from correctly rounded rational
   fractions. The full arrays have 34 entries, with only 22 selected by the
   ordinary Morph stage calculation.
3. **Playback changes read kernels with overlap.** The lower-overlap path uses
   a four-tap quadratic expression with raw DC gain 2. The higher-overlap path
   uses two-point linear interpolation and doubles the envelope multiplier.
4. **Upper Morph uses a per-launch stereo crossmix and a discrete rate selector.**
   It selects among the base rate and three configured chord ratios; this traced
   operation is not a continuously distributed detuning multiplier.
5. **S.O.S. selects the record source, but a conditional filter follows it.**
   The old handoff's claim that the stored sample always equals the monitor mix
   is too strong. On a traced splice-index equality branch, the recording source
   passes through a 0.7-scaled DC-blocking recurrence while the monitor retains
   the unfiltered mixture.
6. **The two top-level legacy `.bin` files are still transport data.** Import
   the new canonical BIN or the original nested deframed BIN instead.

These findings qualify the old reports; the original files have been preserved
so the history and supporting material remain inspectable.

## 2. Evidence and confidence

“Exact bytes” means extraction from the hash-locked application, with address,
extent and hash recorded. “Instruction transcription” means a selected arithmetic
or branch sequence checked against the included listing. “Interpretation” assigns
names to state and behavior; those names are supplied by the analyst.

The selected-component tests compare finite arithmetic cases, including fused
operations, against independent offline calculations. They do not execute the
entire firmware, model its peripherals, or compare against hardware recordings.
No new Rack module implementation, firmware flashing, staging or commits were
performed as part of this analysis.

## 3. Image identity and recovery

| Item | Value |
|---|---|
| Original firmware ZIP SHA-256 | `a653d8c67f8061fe6b2ffaf6a978584c02a8219eda2a0b690ca0f1272d380a36` |
| WAV SHA-256 | `dc6cb0731ef21725c83c6d24c7cd2e98ad7c6ffccc55071ec3bf0c8e318894a8` |
| Application SHA-256 | `44037eb2cf24b5fc411135e487a6fa226498a8478db981659bb53341cb967609` |
| Application bytes | 164,864 / `0x28400` |
| Mapped flash | `0x08020000` through `0x080483ff` inclusive |
| WAV | Mono PCM16, 48 kHz, 5,909,088 samples / 123.106 seconds |
| Initial SP / reset vector | `0x20030000` / `0x08021415` |
| Reset code / main / DSP | `0x08021414` / `0x08022b68` / `0x08027518` |

The original demodulation findings are confirmed: one second of silence,
732,636 eight-sample QPSK symbols, 6 kHz carrier, phase-to-dibit mapping
45°→10, 135°→00, 225°→01, 315°→11, packed MSB first. The complete decoded
stream is 183,159 bytes. It begins with 1,508 zero bytes and
`99 99 99 99 cc cc cc cc`.

Each of 644 packets contains 256 application bytes followed by a big-endian
IEEE CRC-32. Between packets are eight zero bytes, or 23 after every fourth
packet, followed by the sync marker. The last packet has no subsequent sync;
1,515 zero bytes complete the stream. All 644 CRCs verify on a fresh decode.

`protocol/packets.csv` records every payload's stream offset, WAV sample/time,
flash destination, CRC and SHA-256. The new decoder walks the known packet count
and framing explicitly; it does not determine the final CRC boundary by removing
zero bytes. Negative tests reject changed payload, CRC, padding, sync and trailer.

### Legacy binary naming trap

| Existing file | Bytes | Actual contents |
|---|---:|---|
| `mg204_qpsk_decoded.bin` | 180,136 | Initial sync plus framed packet region; idle ends stripped |
| `mg204_firmware_image.bin` | 180,128 | Packet region with CRCs, padding and sync still present |
| `Morphagene_MG204_RE_bundle/mg204_flash_08020000.bin` | 164,864 | Correct application image |
| `binaries/morphagene_mg204.bin` | 164,864 | Same correct application, independently reproduced |

The first 256 bytes of a framed file can look plausible before addressing
diverges at its first CRC. The validator checks each file's relationship to the
fresh demodulated stream instead of relying on its name.

## 4. Startup, memory, and the audio boundary

At `0x08021414..0x08021442`, startup copies **4,080 bytes** from
`0x08047250` to `0x20000000`, then zeroes `0x20000ff0..0x200243c7`.
These limits are loaded from literals at `0x08021444..0x08021454`.
The copied region ends at flash `0x08048240`; the remaining **448 zero bytes**
are preserved. The supplied ELF maps these as synthetic flash/data/BSS sections;
it is not an original build ELF or a captured RAM image.

The DSP initializer at `0x08026730` sets two reel-plane pointers:

| Runtime pointer | Initialized value | Audited use |
|---|---|---|
| `0x200220fc` | `0xd0000000` | First signed-16-bit sample plane |
| `0x20022100` | `0xd1000000` | Second signed-16-bit sample plane |
| `0x20021ca4` | 8,388,608 | Per-plane sample capacity |
| `0x20021ca0` | `0x007fffff` | Wrapping mask |

Initialization evidence is `0x080268fc..0x0802692a`; recording at
`0x080291ee..0x08029212` uses separate halfword stores with the same index.
This describes two 16 MiB planes, rather than interleaved float storage. At
48 kHz, the capacity corresponds to about 174.76 seconds of stereo material.
That is a storage-capacity calculation, not a measurement of maximum usable
recording length in every operating state.

Main supplies **48,000** to audio initialization at `0x08022d18`.
It configures TX at `0x200213d0`, RX at `0x20021344`, and a count of **32
halfwords** at `0x08022d4c..0x08022d52`. The DMA interrupt at `0x08025b9c`
halves that count and calls the DSP with the corresponding buffer half.
The DSP reads adjacent signed halfwords and advances by two per iteration:
**eight stereo frames per half-buffer callback**, or 1/6 ms nominally.
This differs from Mimeophon's four-frame float-processing wrapper.

The input normalization literal is `0x38000100` (about 1/32767), followed by
1.5 scaling and clipping in the traced live-input path. Output conversion
multiplies by 32767, converts to signed integer, and stores halfwords
(`0x08027cbe..0x08027d78`, `0x08028f78..0x08028fce`). These are firmware
buffer formats, not proof of the codec's effective analog resolution or the
format of every supported SD WAV file.

Main reads per-device values at **`0x0800c000`**, outside this update image
(`0x08022d40` and following). A desktop reproduction cannot recover a particular
unit's voltage calibration from this update alone.

## 5. Control acquisition and Gene Size

The ADC interrupt `0x080263e4..0x08026524` confirms six independent moving sums:

| DMA halfword offset | Averaged state | Control interpretation | Window |
|---:|---|---|---:|
| +0 | `0x20021c68` | S.O.S. | 16 conversions |
| +2 | `0x20021c6c` | Morph | 16 conversions |
| +4 | `0x20021c70` | Organize | 64 conversions |
| +6 | `0x20021c74` | Vari-Speed | 64 conversions |
| +8 | `0x20021c78` | Slide | 4 conversions |
| +10 | `0x20021c7c` | Gene Size | 64 conversions |

The source DMA array begins at `0x20000ff0`. Window lengths come from the
right shifts by 4, 6 and 2 and index wrap comparisons against 15, 63 and 3.
Conversion cadence has not been fully reconstructed, so these are not asserted
as millisecond smoothing times.

For ordinary Gene Size (`A > 199`), the earlier curve is confirmed:

```text
B = float32(splice_length)
while B > 576000: B *= 0.5
x = exact_gene_table[1073 - (A >> 2)]
square = float32(x*x)
cube   = float32(square*x)
L      = float32(cube*B)
```

The later path floors ordinary duration to eight samples. `A <= 199` selects
the distinct whole-splice mode; its internal chunk quantity is not audible Gene
lifetime. Recalculation uses a greater-than-32 ADC deadband or splice-length
change. The 1,024 entries at `0x08044d10` all match rounded
`2**(i/341 - 3)` bit-for-bit.

The original CSV extractor used Python double arithmetic for the cube and final
product. For a ten-second splice, **1,264 of 3,896 ordinary ADC cases** differ
in final binary32 bits from the instruction-ordered calculation. This is a small
numerical difference, but the old CSV is not a bit-exact reference. The new
`analysis/control_curves.csv` preserves result bits. No interpolation across the
four-code plateaus or the long-splice folding discontinuity has been introduced.

## 6. Corrected clock quantization

Read `disassembly/clock_gene_quantizer.asm` together with the final store at
`0x0802acd0..0x0802acd6` in the full DSP listing. Let `L` be the preliminary
duration and `S` the **unfolded** splice length. The directly transcribed logic is:

```text
if L <= 8: return L
c = float32_from_bits(0x3f2aaa9f)
current = float32(S*c)
if L >= current: return S
multiplier = 0.75
repeat:
    next = float32(current*multiplier)
    if L >= next: return current
    current = next
    multiplier = c if multiplier == 0.75 else 0.75
```

Both comparison arms branch to a store of **s12, the current bin**. The next
candidate is in s9 or s10. This distinction corrects the old “first candidate
below preliminary L” description. Approximate output levels below S include
2S/3, S/2, S/3, S/4, S/6, and so on. Exact levels accumulate the stored
approximate two-thirds coefficient and binary32 rounding.

| Preliminary length, S = 480,000 | Stored quantized samples |
|---:|---:|
| 0.7 S | 480,000 |
| 0.6 S | 319,999.65625 |
| 0.4 S | 239,999.75 |
| 0.3 S | 159,999.671875 |
| 0.2 S | 119,999.75 |
| 0.1 S | 59,999.8125 |

These are calculated results from the audited branch sequence, not measured
hardware timings. The later duration floor and other event/state handling still
apply. Separately, the 6,000-entry clock reciprocal table at `0x0803ef50`
resembles `1/(32*(i+1))`, but **33 entries differ** from regenerating that formula
and rounding to binary32. Preserve the extracted values.

## 7. Morph stages, gain, and upper-regime choices

Morph still selects `min(21, trunc(float32((ADC/4096)*33.99f)))`, with the
stage-3 seamless point at ADC 362 and stage-21 plateau at ADC 2531. The
continuous Morph state is separately smoothed with coefficient 0.01 per frame.

| Table | Flash | Startup RAM | Physical entries |
|---|---|---|---:|
| Density | `0x080475a8` | `0x20000358` | 34 |
| Launch factor | `0x08047630` | `0x200003e0` | 34 |
| Playback gain shape | `0x080476b8` | `0x20000468` | 34 |
| Integer cycle denominator | `0x08047740` | `0x200004f0` | 34 |

The first, second and fourth use stage 0..21 in the audited normal path. The
gain table has a distinct index based on the sum of envelope-state values:
`clamp(trunc((sum-1)*6)+3, 0, 33)` (`0x08027ade..0x08027b16`). Its target
is smoothed as `0.01*table[index] + 0.99*previous`, then used in playback
scaling (`0x08028dca..0x08028df8`, mirrored at `0x0802a098..0x0802a0c6`).
Do not assume this is simply `1/number_of_genes`.

Some important exact launch entries:

| Stage | Nominal label | Stored value | Bits |
|---:|---|---:|---|
| 2 | 4/3 | 1.3333330154418945 | `0x3faaaaa8` |
| 6 | 2/3 | 0.6666600108146667 | `0x3f2aaa3b` |
| 8 | 4/7 | 0.5714300274848938 | `0x3f12493d` |
| 10 | 4/9 | 0.44444000720977783 | `0x3ee38da4` |
| 15 | 1/3 | 0.33333298563957214 | `0x3eaaaa9f` |

The nominal rational description remains useful. Replacing the stored table by
exact fractions is a deliberate approximation, and density is not an exact
floating-point reciprocal of launch factor. The cycle counter reset is visible
at `0x08028b06..0x08028b1e`; this does not prove an alternative rational
accumulator is identical in every transition.

### Random generator and stereo crossmix

The state at `0x200220f8` follows unsigned wraparound:

```text
state = (state * 0x0bb38435 + 0x3619636b) modulo 2^32
```

At a traced Gene launch (`0x0802842a..0x08028490`), the first draw, converted
to unsigned float and multiplied by 2^-33, is compared with `smoothedMorph-0.5`.
On success, a second draw times 2^-32 becomes the stored per-Gene crossmix
coefficient; otherwise the coefficient is zero. The success probability is
approximately `clamp(2*M-1, 0, 1)`, subject to finite RNG and float conversion.

For the two rendered channel values, the stored coefficient p produces:

```text
outA = (1-p)*A + p*B
outB = p*A + (1-p)*B
```

This is a linear interpolation between original and exchanged stereo channels.
At p=0.5 both channels become their average. Identical mono channels remain
unchanged. It is not evidence for an equal-power left/right pan law.
The arithmetic is visible at `0x08028c36..0x08028c5a`, with the same
crossmix structure in the dense path at `0x08029f5e..0x08029f82`.

### Discrete chord-rate selection

The launch path computes a nonnegative index, when M>0.6, approximately as:

```text
index = trunc((M-0.6f) * 9.999f * U)
```

Here U comes from an unsigned RNG draw times 2^-32; the supplied helper preserves
the actual multiplication order. The range under ordinary Morph values is 0..3.
The index selects the integer/fractional playback increment pair prepared at
`0x08027f3c..0x08027f9e` and consumed at `0x08028596..0x080285de`.
Pitch selection uses the second draw when the stereo test fails and a third
draw when it succeeds. Changing that draw count changes later choices.

The four rates are base, base×mcr1, base×mcr2 and base×mcr3. The initializer
loads 2.0, 1.5 and **`0x3faaaaaa` = 1.3333332538604736** for the three ratios
(`0x080268aa..0x080268e4`). This last constant is distinct from both the
stage-2 launch entry and correctly rounded exact 4/3. Options can replace the
ratios. The presence of additional per-frame RNG consumers means this isolated
launch helper does not reproduce an entire patch's random sequence.

## 8. Adaptive playback interpolation

At `0x08027c16..0x08027c2c`, the sum previously formed from six
envelope-state locations at `0x20021e64..0x20021e78` is compared with 2.0.
The branch is evaluated outside the per-frame loop. It selects two large DSP
paths. Initialization sets the active voice-count state to four, while several
arrays and helpers have capacity beyond four; physical storage capacity is not
proof of six user-audible Genes in the normal mode.

### Lower-overlap branch

`0x08028b5a..0x08028c32` reads a=x[n-1], b=x[n], c=x[n+1], d=x[n+2]
and the fractional position t. Its scalar mathematical form is:

```text
y = b + 0.5*(a+c) + t*((c-a) + 0.5*t*(a+d-b-c))
```

The supplied component retains the intermediate binary32 operations and three
explicit fused multiply-adds. This is quadratic in t, and a constant input v
returns **2v**. It is not a unity-gain cubic interpolator. The output is multiplied
by the voice envelope before the stereo crossmix and shared gain/clip stages.

### Higher-overlap branch

`0x08029efe..0x08029f6a` reads only b and c:

```text
y = (b + t*(c-b)) * (2*envelope)
```

The doubled envelope preserves the factor-of-two convention used by the sparse
kernel. This is a concrete performance tradeoff in the firmware and a reason not
to replace every read with one generic resampler while claiming fidelity.

The helper tests cover constant input, impulses at different tap positions,
ramps, signed-16-bit extremes and fractional positions. They do not include the
complete wrapping/splice-boundary state machine. Those address calculations,
voice lifecycle rules and all envelope branches remain outside the helper.

## 9. S.O.S. and the record-path qualification

The target mapping is confirmed at `0x080276be..0x080276e6` and
`0x08028850..0x0802886a`:

```text
scaled = float32(ADC * float32_from_bits(0x398a26fe))
target = 1 if scaled > float32_from_bits(0x3f864064)
         else max(0, scaled - float32_from_bits(0x3d480c74))
```

ADC 0..185 yields zero; 3981..4095 yields one. ADC 2048 yields
0.49081748723983765. S.O.S. is smoothed with 0.001 per audio frame and feeds a
complementary crossfade. The firmware uses explicit VFMA operations, so this is
more precisely `fma(playback-live, sos, live)` after the prior subtraction.

The instruction path `0x08028e7a..0x08028e9c` forms both monitor channels,
then checks `options+0x24` (`inop`). A zero value copies the mixture to the
record-source registers; nonzero leaves the conditioned live input there.
The mirrored higher-overlap path follows the same arrangement.

However, `0x08028ea0` compares the value read from `0x20022114` with the
selected splice at `0x20021de4`. On equality, `0x08028eac..0x08028ef0`
updates record-side state at `0x2002412c..0x20024138`:

```text
scaled_input = 0.7f * selected_record_source
record_value = scaled_input - previous_scaled_input + 0.997f*previous_record_value
```

The instruction order is a VFNMS for the feedback term, followed by a separate
addition of scaled input. The monitor samples are held in other registers and
bypass this recurrence. The selected record values are later quantized and
written to the two int16 reel planes.

Consequently, “record the S.O.S. mixture” is accurate as a **source-routing**
description. “Write exactly the same sample heard at the output” is not universal.
Likewise, the old `sos^N` overdub argument is an idealized routing model; filtering,
gain, clipping, resampling and int16 conversion affect actual successive passes.
The applicability of the splice-index equality condition across every record
transition still needs a complete event/state trace.

## 10. Vari-Speed, envelope and options boundaries

Two exact table blocks now support a calibrated-control model. This begins
after the upstream control normalization; it does not infer volts per ADC code.

For `vsop` 0/1, calculate `i=min(399,trunc(x*400))`, then clear one low bit
for mode 0 or two low bits for mode 1. If i>199, use magnitude table[i-200].
If i<=196, negate table[196-i]. Otherwise use zero. The magnitude table begins
at `0x080462b0`, and the audited access never exceeds index 198.
Evidence: `0x0802aa96..0x0802aae2`, `0x0802acf4..0x0802ad1a`, plus
the mirrored whole-splice path.

For `vsop>1`, use the positive table at `0x08045e10` with
`clamp(trunc(x*328)-16,0,295)` (`0x080287e0..0x08028816`). It has zero
entries at the bottom and a 4.0 plateau at the top. These are rate targets;
the smoothing, update holdoffs, clock multiplier, phase modulation and chord
selection remain distinct operations.

The half-duration envelope quantity and 24,000-sample cap are confirmed at
`0x08028ad4..0x08028ae8` and `0x08028924..0x08028964`. There are
separate 250-sample / 0.004-coefficient branches affected by `gnsm` and overlap
(`0x08027b7a..0x08027b94`, `0x08028966..0x08028990`). This is stronger
evidence than a generic guessed Hann window, but not yet a full envelope model.

The 2026-09-29 [Gene/splice envelope trace](analysis/gene_splice_envelope_trace.md)
now transcribes the configuration selector: half-duration/capped edges survive
only under the recovered Morph/gnsm conditions, while the seamless launch factor
forces 250. It also identifies linear per-frame envelope increments, separate
64-source-sample splice-edge ramps, and rotating voice launches for immediate
Organize. This does not establish a standalone universal 250-frame Organize
crossfade or a complete voice/event state machine. See the trace for addresses,
the component helper, tested cases, and remaining parity work.

`analysis/embedded_options.txt` preserves firmware-emitted option descriptions
and their addresses. The embedded “firmware version 155b” text also exists; its
presence is not evidence that the supplied update should be relabeled. Options
parser edge cases and exact codec/input-gain settings need further tracing.

## 11. Delivered evidence and checks

The extension includes 14 exact table blocks, binary and CSV exports, and C++17
hexadecimal constants for **8,056 words**. For blocks without an established
consumer, the manifest explicitly says the semantic role is unproven. A
rate-shaped neighboring block is not silently treated as the active rate law.

The recursive listing contains **23,200 instructions / 62,984 decoded bytes**,
204 entry candidates and 756 direct call edges. It starts from vectors and the
verified DSP entry, follows direct edges and five manually bounded switch
tables, and reports indirect transfers separately. Plausible code pointers are
exported as candidates without automatically treating all of them as code.
The broader linear sweep includes literal data decoded as instructions and is
provided only for discovery. Neither listing proves complete control-flow
coverage or original compiler function boundaries.

Artifact validation independently verifies the fresh WAV recovery, packet
records, table BIN/CSV/C++ round trips, Intel HEX reconstruction, ELF flash/RAM
loads, preserved tail and every recursive instruction's underlying bytes.

The selected-component test passed **28,646 checks** on Linux GCC 11.4 and native
Windows MINGW64 GCC 16.1 with `-mfma`. It covers every 12-bit control code,
fold/clock boundaries, 42 raw-kernel fixtures, 28 conditional RNG fixtures,
rate endpoints, stereo mixing, smoothing and record-filter DC rejection.
The default MinGW FMA library path differed by one ULP on a supplied kernel
case; hardware FMA passed the same fixture. This is recorded as a portability
limit rather than weakening the expected answer.

Fused instruction semantics were checked against the primary
[Arm instruction reference, section C4.12](https://documentation-service.arm.com/static/6245c734b059dc5ff9a8bdab).
VFMA adds the product without intermediate product rounding; VFNMS produces
product minus the prior destination. All device-specific findings otherwise
come from the supplied local firmware. No external behavior claims have been
substituted for a binary trace.

## 12. Remaining work

The next useful step is a typed model of the voice arrays and event state,
including every scheduler/envelope transition, splice change, record mode and
clock policy. The two large rendering branches should be compared field by field
before their common operations are combined into a complete reference renderer.

A faithful end-to-end model also needs the calibration/input front end, codec
writes, filesystem/reel transaction semantics and all RNG-consumption timing.
An instruction-accurate emulator or hardware output traces would provide the
next independent oracle. None are represented as having been produced here.

The new findings justify specific corrections and selected components. They do
not yet justify claiming complete hardware equivalence or automatically applying
the earlier implementation handoffs to Chimera.
