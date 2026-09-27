# Mimeophon MP86 — firmware extraction and static engineering analysis

**Input:** the user-supplied `mp86.wav` · **Analysis date:** 27 September 2026  
**Result:** complete recovery of the 49,152-byte transported application image; 192/192 packet CRCs pass.  
**Scope:** offline decoding and static analysis. No module was connected, modified, flashed, or audio-tested.

## 1. Executive findings

The WAV is not a conventional two-frequency FSK update. It carries **absolute-phase QPSK** at a 6 kHz carrier, with 6,000 symbols per second and two bits per symbol. Its framing matches the public `stm-audio-bootloader` QPSK encoder family by Émilie Gillet [S1]. The payload is directly readable ARM Thumb firmware: no descrambling, decompression, or decryption is needed to obtain executable instructions.

Two independent demodulation methods agree on all **226,848 symbols**. All **192 payloads** pass their stored CRC-32. Their ordered concatenation is **48 KiB**. The recovered vector table, startup code, floating-point constants, diagnostic strings, and internal revision integer are mutually consistent. This is strong evidence that the transported bytes have been recovered correctly, rather than merely producing an attractive-looking disassembly.

The principal engineering findings are:

* An STM32F7/Cortex-M7-class, hardware-floating-point application at `0x08020000`, with a reset vector of `0x08020299` and initial stack pointer `0x2004c000`.
* A four-stereo-frame processing entry point at `0x080239b4`, called by two alternating DMA-buffer callbacks. Hardware initialization requests a nominal 48,000 Hz audio rate independently of the update WAV's sample rate.
* Two **2,097,152-float main delay buffers**, occupying **8 MiB each**, at `0xd0800000` and `0xd1000000`. At nominal 48 kHz, each holds approximately 43.69 seconds. These allocations are not a claim that the entire physical RAM device is only 16 MiB.
* A separate 32,768-float ring at `0xd0000000`, used by an **eight-tap feedback network with an explicitly recoverable 8×8 Hadamard matrix**. Four additional short allpass delay lines have lengths 969, 803, 1,236, and 1,511 samples.
* Exact control tables for exponential conversion, Halo matrix gain, Repeats gain, Color coefficients, and zone-dependent timing. There are 3,072 floats in the six large contiguous lookup tables alone.
* A four-stage allpass-average filter cascade in each audited Color channel, plus tap interpolation and amplitude-dependent polynomial gain. This is more specific than identifying “a filter” or “a reverb.”
* A codec-control fingerprint compatible with the WM8731/TLV320AIC23 register family, including 32-bit I²S interface settings. The register fingerprint does **not** establish the exact installed part number.

The remaining gap is not the firmware extraction. It is the full semantic reconstruction of the optimized DSP and its mode-dependent state transitions. The bundle preserves the evidence needed to continue that work.

## 2. Evidence levels and provenance

**Verified bytes:** values directly decoded from the WAV, CRCs, table entries, strings, instruction encodings, and literal addresses. These are reproducible with the bundled scripts.

**High-confidence interpretation:** Cortex-M7/F7-class platform, audio callbacks, large float rings, the Hadamard network, individual allpass equations, and the main lookup-table roles. These conclusions are supported by multiple code paths or by matching the register layout to primary documentation.

**Provisional interpretation:** exact correspondence of some ADC slots to panel controls, transition-window field names, several rate-related coefficients, and complete routing through Hold, Flip, Skew, and Ping Pong. The data is exact even where the label remains provisional.

**Not established:** original source code, original symbol names, a particular compiler version, exact MCU/codec package, the board's complete analog transfer function, hardware-measured audio timing, full state-machine equivalence, or the specific difference between MP86 and an earlier firmware image.

Make Noise identifies MP86 as the noise-floor-reduction revision [S2]. The code also writes the integer **86** into `0x200050fc` at approximately `0x080278a6`. This independently corroborates the revision label. The uploaded WAV was **not** byte-compared with the official ZIP: its link was located, but downloading that archive was unsuccessful. No assertion of signed authenticity follows from the CRCs.

### Cryptographic identities

| Artifact | Bytes | SHA-256 |
|---|---:|---|
| Uploaded `mp86.wav` | 3,725,612 | `d116d3b0d5d54443437934216377d178321e401e706321e1ad72d5ccc05a9652` |
| Recovered application BIN | 49,152 | `31323684dacda3d6313565a1a47596005870d1defed55e0336fd419ca82040b9` |

The original upload is preserved in `input/`. `MANIFEST.sha256` covers the deliverable files. The original firmware remains third-party material; analyst-written tools and labels do not change its ownership or licensing.

## 3. Audio transport: exact recovered format

### 3.1 WAV container and modulation

The input is uncompressed, mono, signed 16-bit little-endian PCM at 48,000 samples/s. It contains 1,862,784 frames and lasts **38.808 seconds**. The first 48,000 samples are zero. The signal after that begins on a usable carrier/symbol boundary.

Each symbol occupies eight samples: one complete cycle of the 6 kHz carrier. If a symbol has numeric value `s` in 0…3, its two quadrature signs are:

```text
I_sign = 2 * floor(s / 2) - 1
Q_sign = 2 * (s mod 2) - 1
x[n] ∝ I_sign*cos(2πn/8) + Q_sign*sin(2πn/8)
```

Four successive two-bit symbols are packed into each byte, most significant first. There is no differential decoding stage in the successful extraction. Carrier phase is referenced to the clean file's detected onset.

The bundled decoder uses a one-cycle complex DFT. For the same alignment, the signs of sample 0 and sample 2 independently classify the I and Q bits. The second method agrees with the DFT on every symbol. It is a useful independent check because it does not reuse the frequency-domain calculation.

This decoder intentionally targets the supplied clean digital file. It is not advertised as a general modem for recordings made through speakers, unknown phase shifts, resampling, clipping, or sample-clock drift. Such recordings need synchronization and error-handling machinery beyond this extraction.

### 3.2 Packet format

```text
Offset  Length  Meaning
0       8       00 00 00 00 00 00 00 00
8       4       99 99 99 99
12      4       CC CC CC CC
16      256     Payload bytes
272     4       CRC-32 of payload, big-endian stored word
Total   276
```

The CRC is the unsigned IEEE CRC-32 returned by `zlib.crc32(payload)`. There is no CRC over the preamble. For example, packet zero's stored/computed checksum is `9d57885f`.

The decoded post-silence byte stream is 56,712 bytes. It begins with 1,500 zero-symbol bytes, equivalent to one second of carrier training. The first packet starts at demodulated byte 1,500. Packet starts are normally separated by 276 bytes; after every fourth packet, an additional 15 zero-symbol bytes appear. These 15-byte pauses last 10 ms at the 1,500-byte/s raw transport rate.

The four-packet grouping is consistent with 1,024-byte update chunks. It does not by itself prove the target flash's erase-sector size or recover the resident bootloader's implementation.

### 3.3 Complete duration accounting

| Component | Duration |
|---|---:|
| Initial PCM silence | 1.000 s |
| Intro/training carrier | 1.000 s |
| 192 packets × 276 bytes / 1,500 bytes/s | 35.328 s |
| 48 group pauses × 0.010 s | 0.480 s |
| Outro carrier | 1.000 s |
| **Total** | **38.808 s** |

All non-packet demodulated bytes are zero-symbol framing/pause bytes. The decoder checks this rather than silently discarding unexplained data.

### 3.4 Relation to the public encoder

The quadrature mapping, preamble, byte packing, and CRC placement match [S1]. The pause schedule is specific to this file and differs from several current upstream target presets. Therefore the warranted statement is **protocol compatibility or derivation**, not proof that Make Noise shipped an unchanged current upstream bootloader.

The upstream encoder supports an optional XOR scrambler. It was not needed here: the CRC-valid packet contents themselves form the coherent firmware image. Applying that unrelated optional transform is not part of the successful decode. This analysis also does not prove that Morphagene uses the identical settings; its firmware WAV was not part of this task.

## 4. What the recovered binary contains

The raw image maps to `0x08020000…0x0802bfff` inclusive. The update does not contain the resident bootloader below that base. It also does not contain the module's live audio buffers or the settings region referenced at `0x080c0000…0x080fffff`.

| File offset | Flash address | Contents |
|---|---|---|
| `0x0000…0x01c7` | `0x08020000…0x080201c7` | 114-word vector table |
| `0x01c8…0x8c7b` | `0x080201c8…0x08028c7b` | Startup, support code, DSP, handlers, inline literal pools; not exclusively instructions |
| `0x8c7c…0x8dc3` | `0x08028c7c…0x08028dc3` | Diagnostic text region |
| `0x8dc4…0xbdc3` | `0x08028dc4…0x0802bdc3` | Six contiguous large float tables, 12,288 bytes |
| `0xbdc4…0xbe07` | `0x0802bdc4…0x0802be07` | Small constants/string material |
| `0xbe08…0xbf7b` | `0x0802be08…0x0802bf7b` | 372-byte RAM initializer |
| `0xbf7c…0xbfff` | `0x0802bf7c…0x0802bfff` | 132 preserved zero bytes |

These are analysis boundaries, not recovered original linker-section names. The entire transported image is retained. No trailing zeros were stripped and no imaginary bootloader bytes were prepended.

The bundle provides the image as raw BIN and address-bearing Intel HEX. The **ELF is a synthetic analysis wrapper** constructed from the recovered bytes, initialized RAM layout, and analyst labels. It is not the vendor's original ELF, and it should not be treated as a hardware update file.

## 5. Processor, startup, and memory organization

### 5.1 Architecture evidence

The first two vector words are:

```text
Initial SP     0x2004c000
Reset vector   0x08020299   (Thumb bit set; code at 0x08020298)
```

The reset trampoline transfers to runtime startup at `0x080201c8`. Startup copies 372 bytes from `0x0802be08` to `0x20000000` and clears `0x20000174…0x20005a7f`. The zeroed BSS span is 22,796 bytes. Main is at `0x08027648`.

Main explicitly programs the vector-table base to `0x08020000` and performs Cortex-M7-style instruction/data cache initialization. The instruction stream contains hardware single-precision operations, fused multiply-add, and min/max-number instructions. The peripheral and interrupt layout is consistent with the official STM32F746-class CMSIS definitions [S3].

The conservative identification is **Cortex-M7, STM32F7/F74x–F75x-compatible application**. A precise MCU SKU should not be asserted without an identifying constant, a board photograph, or hardware identification. The firmware is interrupt-driven in a bare-metal style; a named RTOS has not been identified.

### 5.2 Interrupts with nondefault handlers

The vector table contains 16 core entries and 98 peripheral positions. Most unused peripheral entries point to `0x08027c95`, a shared default handler. Identified active peripheral vectors include DMA1 streams 2 and 7, DMA2 streams 0, 1 and 5, and TIM2. Core fault handlers and SysTick have their own targets. The complete exact vector table is exported in `analysis/vectors.csv`.

### 5.3 Main delay storage

Initialization at `0x0802343e…0x080235de` establishes two float buffers:

| Buffer | Base | Bytes | Float samples | Nominal capacity at 48 kHz |
|---|---|---:|---:|---:|
| Main A | `0xd0800000` | 8,388,608 | 2,097,152 | 43.690667 s |
| Main B | `0xd1000000` | 8,388,608 | 2,097,152 | 43.690667 s |
| Auxiliary Halo ring | `0xd0000000` | 131,072 | 32,768 | Allocation capacity, not one audible 0.683 s echo |

The main-ring length is stored at `0x200037f8`; its mask, `0x001fffff`, at `0x20001b20`. The auxiliary mask is 32,767 at `0x20001b4c`.

Four 44-byte structures begin at `0x20001a14`. Their buffer pointers alternate between the two main arrays. This supports a dual-head-per-channel organization, consistent with changing read positions and transitions without allocating a separate recording for every zone. Exactly how each field participates in all Hold/Flip/clocked paths still needs a complete state-machine trace.

The clear routine at `0x08023988` provides a second, independent confirmation of the two main buffers and their sample count. Neither their contents nor any prerecorded material can be recovered from an update image: these arrays exist in volatile RAM after boot.

### 5.4 Persistent settings are outside this image

Main references a flash scan region beginning at `0x080c0000`, ending near `0x080fffff`. Those addresses are outside the transported application. The code for reading or updating settings can be analyzed, but this WAV does not disclose a particular module's saved settings, calibration, or input-gain selection.

## 6. Audio boundary and processing quantum

The main DSP entry is `0x080239b4`. Two wrappers at `0x0802697c` and `0x08026ab4` convert incoming signed 32-bit transfer words to float, invoke that entry with a count of **four stereo frames**, convert results back to signed integer words, and write the opposite half of the output DMA buffer.

| State | Address | Observed format |
|---|---|---|
| RX transfer buffer | `0x20001adc` | 16 × signed 32-bit words |
| TX transfer buffer | `0x20002ae8` | 16 × signed 32-bit words |
| Float input scratch | `0x20004d7c` | 8 floats / 4 stereo frames |
| Float output scratch | `0x200038a4` | 8 floats / 4 stereo frames |

The input conversion multiplier has bits `0x300a3d6d`, exactly approximately `5.0291398823e-10`. The output multiplier has bits `0x4e7ffc00`, exactly `1,073,676,288`. These are actual constants, not a generic assumption that every transfer uses precisely `1/2^31` and `2^31`. They are not enough to infer end-to-end module gain without the intervening DSP and analog circuitry.

Main stores **48,000** into the SAI audio-frequency configuration at `0x08027aac…0x08027ad0`; the SAI block addresses are also visible. This is independent evidence for the nominal processing rate. Actual LRCLK, oscillator tolerance, and PLL-derived rate have not been measured, so all seconds/milliseconds in this report are explicitly **nominal at 48 kHz**.

At that nominal rate, one block covers 83.333 µs and the two callbacks collectively process 12,000 blocks per second. This is the computation quantum, **not a measurement of complete analog-input-to-output latency**. DMA scheduling, converter filters, and hardware buffering add other terms.

## 7. Zone timing and exponential rate conversion

### 7.1 Exact base-delay table

Eight float values are copied from flash offset `0xbe10` into RAM at `0x20000008`. The following values come from the recovered bytes. Maximum ranges use the observed/documented per-zone factors; the firmware also contains an approximately 2,006,227.5-sample long-delay bound.

| Zone | Minimum samples | Minimum ms | Nominal maximum ms | Range factor |
|---:|---:|---:|---:|---:|
| 0 | 61.22520447 | 1.275525 | 20.408401 | 16 |
| 1 | 979.60327148 | 20.408401 | 81.633606 | 4 |
| 2 | 3918.41308594 | 81.633606 | 326.534424 | 4 |
| 3 | 7836.82617188 | 163.267212 | 653.068848 | 4 |
| 4 | 15673.65234375 | 326.534424 | 1306.137695 | 4 |
| 5 | 31347.30468750 | 653.068848 | 2612.275391 | 4 |
| 6 | 62694.60937500 | 1306.137695 | 5224.550781 | 4 |
| 7 | 125389.21875000 | 2612.275391 | 41796.406250 | 16 |

The values agree with the official manual's rounded ranges [S4]. They show why the longest playable nominal interval is about 41.796 s although the allocated ring can hold about 43.691 s. Allocation size and control range should not be conflated.

A mathematical observation, not a measured pitch claim: the smallest base delay closely matches `48000 / (440 * 2^(10/12))`, with the other bases related by powers of two. A feedback oscillator's audible fundamental also depends on filter phase and interpolation, so this is not proof that a particular patch will be exactly in tune at that nominal pitch.

### 7.2 Exponential lookup

Flash `0x08028dc4` contains 2,048 floats closely following:

```text
exp2_fraction[i] ≈ 2^(i / 2048),  0 ≤ i < 2048
```

The largest absolute deviation from a high-precision formula is about `6.42e-8`. A correctly rounded float32 re-evaluation matches **2,010/2,048** entries exactly; 38 entries differ. Consequently, a bit-conscious reconstruction should use the extracted values, not assume regenerating the apparent formula produces identical bytes.

One audited usage masks an index with 2,047 and doubles the result when the index is in the next octave. Other references occur in the rate/control paths around `0x080254b2`, `0x0802633e`, `0x08026694`, `0x0802675a`, and `0x08026834`. Recovering this table eliminates much of the guesswork in an exponential rate implementation, but does not on its own reconstruct every CV scaling and state branch.

### 7.3 Clocked delay ratios

Initialization builds a 13-entry float table at `0x20003804`:

```text
1/2, 4/7, 3/5, 2/3, 3/4, 4/5, 1,
5/4, 4/3, 3/2, 5/3, 7/4, 2
```

These are **delay-time ratios**. The corresponding repeat-frequency ratios are their reciprocals. This distinction matters: copying the manual's frequency multipliers into a delay-length calculation without inversion would reverse the mapping. These runtime values were reconstructed from immediate stores at `0x08023428…0x0802348e`; they are not falsely presented as one contiguous flash array.

### 7.4 Other zone-dependent values

The image also provides per-zone transition scales `[68, 132, 260, 1004, 1004, 1004, 1004, 1004]`, coefficients `[1/64, 1/128, 1/256, .001, .001, .001, .001, .001]`, and eight near-unity filter coefficients. The exact bytes and addresses are exported. The transition field's name is still provisional; it should not automatically become a fixed crossfade duration until all uses are traced.

## 8. Halo: an eight-way feedback/diffusion network

### 8.1 The recovered matrix

The code around `0x08024b8e…0x08025154` reads eight locations from the auxiliary float ring and computes an explicit sum/difference butterfly. The resulting eight outputs are the following unnormalized Sylvester Hadamard transform:

```text
+ + + + + + + +
+ - + - + - + -
+ + - - + + - -
+ - - + + - - +
+ + + + - - - -
+ - + - - + - +
+ + - - - - + +
+ - - + - + + -
```

Here each sign is ±1. The matrix satisfies `HᵀH = 8I`. The exact sign matrix is exported as CSV and as a tested C++ butterfly component. This algebraic structure is directly recoverable from the instructions; the interpretation as the Halo feedback network is additionally supported by its live coefficient coming from the Halo-associated lookup/smoothing path.

### 8.2 Ring taps and feedback destinations

The ring cursor decreases by one per processed sample and wraps with mask 32,767. Relative read offsets are:

```text
1838, 2241, 2662, 3128, 3688, 4251, 4861, 5539
```

Corresponding write offsets are:

```text
0, 1839, 2242, 2663, 3129, 3689, 4252, 4862
```

The CSV records both rather than calling all eight read offsets independent delay lengths. Their differences are 1,838, 402, 420, 465, 559, 562, 609, and 677 samples, before the extra processing on selected feedback branches. That distinction is important when reconstructing a partitioned single-ring implementation.

The matrix operates on the eight tapped signals. The feedback writes include gain, injection of the stereo signal, damping/nonlinear stages, and selected additional allpasses. The network is not accurately characterized as eight plain comb filters added in parallel.

### 8.3 Exact Halo feedback-gain law

The 128-entry table at `0x0802adc4` runs from approximately 0.176776707 to 0.348771602. Its live smoothed value at `0x20004d24` multiplies the unnormalized Hadamard outputs.

Because `H` has norm `sqrt(8)`, those endpoints correspond to approximately **0.50000003…0.98647506** when expressed as gain multiplying a normalized orthogonal matrix. This explains why the raw numbers look small: the implementation has not normalized the butterfly by `1/sqrt(8)`.

This is a practical reconstruction trap. Normalizing the matrix and then retaining the original raw table would attenuate the loop by an additional `sqrt(8)`. Conversely, using a normalized-gain table with an unnormalized matrix would increase loop gain. The table endpoint alone is not a complete decay-time or stability analysis: additional filters, nonlinearities, delay lengths, and injection paths affect the full system.

### 8.4 Four short allpass lines

| Base | Length | Nominal allocation time | Coefficient in audited path | Read behavior |
|---|---:|---:|---:|---|
| `0x20001b6c` | 969 samples | 20.1875 ms | 0.7 | Fractional, linearly interpolated |
| `0x20002b64` | 803 samples | 16.7292 ms | 0.7 | Fractional, linearly interpolated |
| `0x200038e8` | 1,236 samples | 25.7500 ms | 0.4 | Integer ring access |
| `0x20000278` | 1,511 samples | 31.4792 ms | 0.4 | Integer ring access |

The locally audited allpass update is:

```text
write_value = input + coefficient * delayed_value
output      = delayed_value - coefficient * write_value
```

The coefficients are represented by float32 constants, so 0.7 and 0.4 here are rounded human-readable forms. The exact constants are visible in the disassembly. The 969- and 803-sample arrays include interpolated read positions and slow modulation-related state; their allocation lengths are not a promise that the instantaneous delay is always the full array length.

### 8.5 Nonlinear feedback and modulation

One clear nonlinear stage clamps its input to ±2/3, then applies:

```text
y = x - x³/3
```

Additional filtering and scaling follow; this is not the whole Halo transfer function.

A 32-bit LCG uses multiplier `196314165` and increment `907633515`, with unsigned wrapping. Its centered random value is formed using `2^-32`, then scaled by approximately `2e-6`. Nearby state-dependent branches constrain tiny modulation increments around ±`3e-6`. References connect this state to the diffusion/read-position processing.

The warranted conclusion is that very small pseudorandom modulation is present. It is **not** justified to call it DAC dither, to attribute the module's audible noise floor to it, or to claim that MP86 introduced or removed it. Those claims require further tracing or a comparison image.

## 9. Color: filter stages, tap morphing, and nonlinear compensation

### 9.1 Four cascaded allpass-average stages

The two channel paths around `0x08024742` and `0x08024996` use a coefficient selected from the 256-entry table at `0x0802b5c4`. The coefficient is smoothed:

```text
a ← a + 0.01 * (target_a - a)
```

For one stage, the core recurrence is:

```text
ap = previous_input + a * (input - previous_ap)
output = 0.5 * (input + ap)
previous_input = input
previous_ap = ap
```

There are four cascaded stages, with intermediate taps retained. At fixed `a`, the allpass part has transfer function `(a + z^-1)/(1 + a*z^-1)`; averaging it with the input gives a first-order lowpass form. Four such stages and tap morphing are more specific than a generic “four-pole filter” description. They should not silently be replaced with an unrelated biquad or ladder filter in a fidelity-oriented port.

The stored coefficient table spans approximately `0.5697072…-0.9364711`. Because control indices are transformed and may be inverted, this statement does not by itself assign dark/bright endpoints to the physical knob.

### 9.2 Tap interpolation and level shaping

The code keeps multiple outputs from the cascade, applies several unequal scale factors, and interpolates between neighboring taps using a fractional control position. It also feeds other state back into the calculation. The standalone C++ component returns the five raw taps only; it does not pretend to reproduce the entire Color path.

Two additional 256-entry tables at `0x0802b1c4` and `0x0802b9c4` participate in Color-related control/filter/gain paths. Their ranges are approximately `0.0737930…0.9365152` and `0…48.14673`, respectively. Exact exports are preferable to speculative analytic replacements.

An unusual access around `0x08026054` uses an aligned **byte-offset** expression into the first of these tables, rather than a simple floating-point index multiplied by four. It is preserved in the disassembly. A port should trace the actual addressing before “fixing” it into a conventional table lookup.

### 9.3 Zone-dependent amplitude polynomial

Four coefficient groups copied into live state at `0x20002ad4` are used by a polynomial gain stage around `0x08024888…0x080248dc`:

```text
g(m) = c0 - c1*m + c2*m² - c3*m³
shaped_signal = g(m) * signal
```

The magnitude/control term `m` is derived from smoothed envelope-related state and clamped in the audited path. Two distinct coefficient sets are apparent: the first zone group uses approximately `[1.326183, .7918028, .2978016, .03785328]`; the remaining groups use approximately `[1.184146, .4349760, .1311650, .01429918]`.

This is an amplitude-dependent gain law, not four unexplained biquad coefficients. Its complete interaction with transient suppression, repeats, and Color needs further routing analysis. The exact coefficients and a narrowly scoped evaluator are supplied.

## 10. Repeats and control conditioning

The Repeats-associated lookup at `0x0802afc4` contains 128 floats. It starts with two zero entries, rises toward unity, and ends:

```text
… 0.9972241, 0.9993614, 1.06, 1.09, 1.12, 1.15,
  1.18, 1.21, 1.24, 1.27
```

That is an explicit above-unity control region, not merely “a feedback knob that probably goes past one.” It is nevertheless a **gain target inside a nonlinear, filtered system**; it does not imply every complete feedback loop has gain exactly 1.27 at the endpoint.

A seven-word control area begins at `0x20004f98`. Behavior-based assignments are:

| Byte offset | Likely control | Evidence |
|---:|---|---|
| +0 | Mix | Clamp to 0…1 and wet/dry-related smoothing |
| +4 | Zone | Eight-way selection and zone-table accesses |
| +8 | Repeats | 128-entry gain table with above-unity tail |
| +12 | Color | Filter/tap/gain table paths |
| +16 | Halo | Hadamard gain and auxiliary amount paths |
| +20 | Rate | Delay/exponential/time calculations |
| +24 | microRate | Fine rate modulation path |

These labels should be confirmed against board wiring for a hardware emulation. They are not a proven ADC pin map and they do not identify the analog CV summing/scaling circuit.

Several control paths perform an offset/scale correction resembling `int((raw - 200) * 1.08)` followed by a 0…4095 clamp. Mix contains a path resembling `clamp((raw - 200) * 0.000264, 0, 1)`. Rate uses a different correction. These are individual audited paths, not a claim that every operating mode applies one shared formula.

Several smoothed controls update once before the four-frame sample loop using `state += .001*(target-state)`. At nominal 48 kHz, an isolated first-order smoother at that block rate has a time constant of about 83.3 ms. Per-sample filter smoothing must not be confused with per-block control smoothing.

Zone selection includes a raw-difference threshold of 256 and eight-bin quantization behavior. That helps explain why continuously modulating the selection need not behave like an unfiltered `floor(knob*8)` at every sample. Complete hysteresis/state behavior remains a follow-up tracing task.

## 11. Codec initialization and input-gain clues

Function `0x08026bf0` sends eleven observed two-byte writes to an I²C API using address argument `0x34`. Under the usual shifted-address convention this corresponds to seven-bit address `0x1a`.

```text
0c 72   0e 0e   00 17   02 17   04 00   06 00
0a 00   08 12   10 00   12 01   0c 62
```

Splitting each control word into a seven-bit register address and nine-bit value produces a coherent WM8731/TLV320AIC23-compatible register sequence. TI's primary register documentation [S5] and the Linux WM8731 driver [S6] support this family-level interpretation.

In that compatible map, the sequence configures the digital interface as **32-bit I²S in slave mode**, selects line input and DAC playback, leaves analog bypass disabled, sets both input-volume fields initially to `0x17`, enables the active path, and changes the output power state near the end of initialization. Those are interpretations conditional on the register-family identification, not direct observations of the physical chip.

A nearby routine at `0x08026d3c` builds stereo input-volume writes from a value that includes an offset of 21. This is evidence that at least part of input-gain handling occurs in codec control, not only in float DSP. The full user-setting-to-decibel mapping and persistent-state selection were not exhaustively reconstructed.

A 32-bit serial transfer format does **not** mean a 32-bit analog converter or 32 bits of measured dynamic range. The precise converter part and effective resolution remain unverified.

## 12. Diagnostics and code organization

Readable diagnostic strings identify stack-frame and fault reporting: registers R0…PSR, fault-status/address registers, LR/EXC_RETURN, and HardFault/BusFault/UsageFault messages. They are exported separately from the indiscriminate printable-byte scan.

`strings_all.csv` deliberately flags strings outside the diagnostic region as unclassified. Printable instruction bytes can look like meaningful strings; they should not become invented feature names.

The recursive disassembly currently visits **11,185 candidate instructions**, covering 30,580 instruction bytes, with 144 candidate entry points and 307 direct call sites recorded. These counts describe the analysis traversal, not a proven original function inventory. Candidate roots include vectors, call targets and plausible code pointers. Four table-byte branches have explicit target enumeration; additional table-halfword and indirect paths remain unresolved.

There are two forms of disassembly:

**`reachable_thumb.asm`:** preferred starting point. It follows control flow and excludes many literal pools, but is still subject to candidate-root errors, indirect branches, and incompleteness.

**`linear_sweep_thumb.asm`:** broad byte coverage for manual checking. It intentionally includes constants decoded as apparent instructions. A line in this file alone is not proof that those bytes are executable.

Focused listings isolate startup, initialization, main, audio callbacks, interrupts, and the DSP core. Addresses are real; function names in companion symbols are analyst annotations. Original C++ classes, source filenames, and debug symbols were not recovered.

## 13. Deliverables and reproducibility

The bundle includes the original WAV, complete recovered BIN, Intel HEX, synthetic analysis ELF, vector/data/code-region extracts, the full demodulated stream, a 192-row packet manifest, exact float tables in CSV and little-endian binary, C++ exact-table constants, memory maps, symbol candidates, literal references, call edges, strings, disassembly, and selected mathematical components.

To reproduce the extraction from the bundle root:

```bash
python -m pip install -r tools/requirements.txt
python tools/decode_mp86.py input/mp86.wav --out reproduced
python tools/extract_artifacts.py --root reproduced
python tools/validate_bundle.py --root .
```

The decoder writes only local files. It neither connects to hardware nor installs firmware. It rejects a failed CRC, disagreement between demodulators, unsupported WAV format, unexpected nonzero framing, or the wrong packet count.

The table extractor checks the exact audited firmware hash before applying version-specific offsets. That prevents accidentally giving an earlier or later image convincing but incorrect MP86 labels.

For disassembly, `tools/build_analysis.py` uses a system LLVM shared library with ARM support. It was run with LLVM 19. The optional Ghidra annotation script was not executed in Ghidra in this environment and is marked accordingly. Importing the raw BIN with the correct address and Thumb context remains the clearest analysis path.

The selected C++ components compile under C++17 and pass sanity checks. Those checks validate their limited algebra and table syntax, **not full Mimeophon audio equivalence**. See `analysis/validation_results.json` for executed tests.

## 14. What is not recovered, and the next useful engineering step

A firmware update does not normally contain its own bootloader, the module's RAM history, or a live dump of persistent settings; this particular extraction does not supply those. It also does not magically turn optimized machine code back into the original source tree.

The highest-value continuation is to decompile and trace `0x080239b4`, using the recovered memory map and exact tables, and to isolate a deterministic sample-processing test harness. The first validation should cover the native four-frame quantum at nominal 48 kHz. Hold, Flip, clocked transitions, Skew and Ping Pong should then be added as explicit tested states rather than inferred from the front-panel description.

For a Rack implementation, exact timing tables, the Hadamard scaling convention, allpass lengths, and Color recurrences are immediately useful. The full control/event logic and interaction of the feedback paths still need careful reconstruction. A generic delay followed by a stock reverb is not what the recovered code describes.

To attribute the advertised MP86 noise reduction to a specific change, decode an older official image with the same workflow and compare constants, callback scaling, codec writes, filters, and DSP branches. With only MP86, any claim that a particular visible operation is “the noise fix” would be speculation.

## 15. Primary references

The conclusions above are primarily original analysis of the supplied bytes. External references are used for protocol matching, device-register interpretation, and limited corroboration—not as substitutes for extraction.

**[S1] Émilie Gillet, stm-audio-bootloader QPSK encoder.** Mapping, packet framing, CRC and optional scrambler. Accessed 2026-09-27.  
https://raw.githubusercontent.com/pichenettes/stm-audio-bootloader/master/qpsk/encoder.py

**[S2] Make Noise, Mimeophon product page.** Official MP86 release description and download link. Accessed 2026-09-27.  
https://www.makenoisemusic.com/modules/mimeophon/

**[S3] STMicroelectronics, STM32F746 CMSIS device header and startup template.** Interrupt layout and processor/peripheral definitions used for family-level identification. Accessed 2026-09-27.  
https://raw.githubusercontent.com/STMicroelectronics/cmsis-device-f7/master/Include/stm32f746xx.h  
https://raw.githubusercontent.com/STMicroelectronics/cmsis-device-f7/master/Source/Templates/gcc/startup_stm32f746xx.s

**[S4] Make Noise, Mimeophon manual.** Printed pages 10–13: rounded zone ranges, rate/tempo and feedback behavior. Relevant PDF pages were inspected visually as well as through extracted text. Accessed 2026-09-27.  
https://www.makenoise-manuals.com/mimeophon/mimeophon-manual.pdf

**[S5] Texas Instruments, TLV320AIC23B data manual, SLWS106H.** Sections 3.1.2–3.1.3, including register-map pages 3-3 and 3-4. Used as a compatible-map reference, not proof of the exact installed codec. Accessed 2026-09-27.  
https://www.ti.com/lit/ds/symlink/tlv320aic23b.pdf

**[S6] Linux upstream WM8731 driver.** Codec register use and serial sample-width settings. Accessed 2026-09-27.  
https://raw.githubusercontent.com/torvalds/linux/master/sound/soc/codecs/wm8731.c

**[S7] STMicroelectronics SAI HAL header.** Audio-frequency initialization structure. Accessed 2026-09-27.  
https://raw.githubusercontent.com/STMicroelectronics/stm32f7xx-hal-driver/master/Inc/stm32f7xx_hal_sai.h
