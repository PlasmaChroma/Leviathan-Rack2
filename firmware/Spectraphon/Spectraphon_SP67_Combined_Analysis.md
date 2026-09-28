# Spectraphon SP67 — firmware and DSP reconstruction

**Analysis date:** 28 September 2026  
**Input:** the user's `sp67.dat`, 178,932 bytes  
**SHA-256:** `b50a87b9bd4f6ad90007918a16e8a4d033e2236c863faec55f5585abe34927d9`  
**Scope:** read-only static analysis, extracted data, instruction-derived mathematical models, and host-side tests. No firmware flashing, ARM execution, physical module measurement, or original source/debug symbols.

## Executive findings

The image is directly interpretable as a little-endian Thumb application linked at `0x08020000`. More importantly, substantial portions of its application behavior are now recognizable: a quadrature harmonic analyzer, two spectral Array readers, a polynomial resynthesis path controlled by Partials, mode-specific Noise/Chaos branches, and an ordinary RIFF/WAVE persistence layer.

The essential architectural result is **not “an FFT feeds a generic wavetable oscillator.”** The traced SAM path constructs harmonic sine/cosine references, demodulates the input against those references, filters the quadrature components, and estimates their magnitudes. The normal synthesis path evaluates paired polynomial recurrences; changing Partials changes those recurrences, not just a final volume multiplier. Arrays store spectral coefficients rather than recordings of the original input waveform.

The strongest recovered implementation facts are:

| Area | Result | Evidence |
|---|---|---|
| Executable | Raw application image; base `0x08020000`; reset `0x08036430` | Vector table and startup instructions |
| Processor | Cortex-M7/FP-capable STM32H7, H743/H753-class match | Thumb/FP instructions, vectors, peripheral addresses; sources S4–S5 |
| Audio timing | 64 stereo frames per callback; DSP math assumes 48 kHz | `0x08032678`, `0x08032680`, `0x0803038c`; pitch constants |
| Spectral storage | 64 float32 values per frame | Array readers and save loop |
| Normal active bank | At most 60 terms, processed in groups of four | Analyzer and standard synthesis loop bounds |
| SAM | Harmonic quadrature demodulation; two cascaded one-poles per component | `0x0802f4ac–0x0802f9bc` |
| SAO linear | Adjacent-frame interpolation plus nonlinear Focus shaping | `0x0802cd0c`, `0x0802ce5c` |
| SAO planar | Four-frame interpolation, with unusual edge arithmetic | `0x0802cfac`, `0x0802d0f8` |
| Partials | Polynomial carrier shaping with a cubic compensation curve | `0x08030cd4–0x08030fd2` and mirrored branch |
| Saved Arrays | Mono PCM16 WAV, 64 serialized coefficients per spectrum | `0x08033af8` |
| Compatibility caveat | Header finalization appears to retain 32-bit alignment/rate fields for PCM16 | `0x080338f4–0x080339fa` |
| Recoverable assets | Nine numerical tables, including a 64 × 64 factory Array | `tables/manifest.json` |

This is a useful implementation foundation, but **not a complete behavioral clone**. The full Noise/Chaos equations, UI state transitions, calibrated jack-to-ADC mapping, physical output calibration, capture scheduling, and all phase/FM interactions remain incompletely translated.

## 1. Evidence conventions and reproducibility

**B — byte evidence:** an address, byte sequence, table extent, constant, branch or pointer is directly recoverable from this exact upload.  
**T — translated behavior:** a mathematical or structural interpretation of instructions. This is stronger than a guess but remains subject to independent review and dynamic testing.  
**D — documented behavior:** stated in manufacturer documentation; not automatically proven by the binary.  
**H — hypothesis:** plausible interpretation that needs more tracing or measurement.  
**U — unresolved:** insufficient evidence for a defensible exact implementation.

Addresses throughout are runtime flash addresses unless described as RAM or file offsets. To convert flash to file offset, subtract `0x08020000`. Analyst-assigned names in the ELF, CSVs and prose are not recovered original symbol names.

The analysis uses LLVM's ARM disassembler through a small Python/ctypes wrapper. A conservative control-flow inventory and a separate linear sweep are included. The latter deliberately retains an **UNVERIFIED** label because a linear sweep interprets literal pools and other inline data as instructions. Even the control-flow inventory has heuristic function boundaries, unresolved switches and indirect branches. Its candidate count is not the number of original C/C++ functions, and its decoded-byte count is not a proof of complete code coverage.

Reproduce table extraction and the ELF with `scripts/export_assets.py`; reproduce disassembly with `scripts/inspect_firmware.py` and `scripts/build_inventory.py`. The disassembler currently expects the LLVM 19 shared-library location used by this environment. Reference math and the WAV tools do not depend on LLVM.

The input was never executed. The host tests validate consistency of the supplied tools and translations, not equality to hardware recordings.

## 2. Image identity and startup

### 2.1 Memory layout

| Item | Recovered value | Level |
|---|---|---|
| File length | `0x2baf4` / 178,932 bytes | B |
| Initial stack pointer | `0x20020000` | B |
| Reset vector word | `0x08036431` — Thumb entry at `0x08036430` | B |
| Application vector table | 166 words / `0x298` bytes | B/T |
| Main entry | `0x08034060` | B/T |
| Low-level system initialization | `0x08035b40` | B/T |
| Initialized-data source | `0x08049ac8` | B |
| Initialized-data destination | `[0x20000000, 0x2000001c)` | B |
| Startup zero range | `[0x20002030, 0x20014fa4)` | B |
| Last nonzero byte | File offset `0x29ae3` / address `0x08049ae3` | B |
| Trailing zero region | `0x2010` bytes | B |

The reset handler sets up the stack, copies the initialized-data image, zeroes the startup BSS range, and transfers through initialization into main. Many DSP locations below the startup BSS range are explicitly initialized elsewhere. Do not infer that every useful global must lie inside the linker-style BSS interval listed above.

The application offset is 128 KiB above the normal STM32 internal-flash origin. This strongly suggests space reserved for a resident updater/bootloader. **The supplied file does not contain that lower flash region.** Consequently, the exact bootloader file-selection logic, authenticity checks, update recovery behavior, and accepted image conventions cannot be recovered from this upload alone.

The terminal zeros are directly observed. Whether they represent linker padding, an unused section, or an updater convention is unresolved. Their presence does not prove a checksum scheme or a particular flash-write alignment.

### 2.2 Architecture and numerical environment

The instruction stream includes Thumb-2 and both single- and double-precision floating-point operations. The vector/peripheral footprint is consistent with an STM32H743/H753-class design. ST documents the corresponding Cortex-M7 family and its double-precision floating-point unit [S4]; the official device startup file provides the matching interrupt ordering [S5]. This does not identify a package marking, exact silicon revision, or populated flash capacity.

Main enables instruction/data caching and sets the floating-point flush-to-zero control bit. Thus subnormal arithmetic, fused multiply-add behavior and conversion instructions matter when seeking a close numerical match. A naive host double-precision rewrite is a useful conceptual model, but is not automatically bit-identical.

ARM's `VFNMS` operation is especially important here: its arithmetic is **product minus the old destination**, not old destination minus product [S6]. Misreading that instruction reverses recurrence signs and substantially changes the inferred synthesis algorithm.

## 3. Recovered application architecture

A defensible high-level organization is:

```text
Resident updater / bootloader       [not in uploaded image]
               |
     application reset and main
               |
  processor, RAM, peripheral setup
               |
  filesystem / Array restore / UI initialization
               |
     DMA / audio half-buffer callbacks
               |
       audio_block_process(offset)
               |
       +-- fast CV and input acquisition
       +-- pitch, FM and mode state
       +-- harmonic reference generation
       +-- SAM quadrature analysis
       +-- SAO spectral interpolation
       +-- standard / Noise / Chaos synthesis
       +-- envelope / sub / clocked-CV generation
       +-- fixed-point output buffers
```

Array I/O and button/clock processing are visible as separate routines, while much of the sample processing is in a large monolithic function. HAL-like peripheral code and FatFs-like filesystem code account for a significant portion of the image. These library-family identifications derive from register accesses, argument conventions, object layouts and call-site behavior; the original library revision and all compiler settings are not recovered.

A useful point for a Rack implementation is that the conceptual separation above is cleaner than the compiled layout. There is no reason to recreate the monolithic function structure in a new plugin. Preserve numerical ordering where it affects results, but use explicit analyzer, spectral-reader, oscillator, control-state and persistence components.

## 4. Interrupts, DMA and audio cadence

The non-default external interrupt footprint contains DMA1 Streams 0–7, ADC, EXTI9_5, SPI5, DMAMUX1 overrun and BDMA channel 0. Core exception handlers and SysTick are also present. The complete vector words are exported in `analysis/vector_table.csv`.

Two small callbacks are unusually helpful:

```text
0x08032678: pass offset 128; tail-branch to 0x0802e750
0x08032680: pass offset   0; tail-branch to 0x0802e750
```

Inside that destination, the sample loop advances its interleaved index by two and stops at 128. This is 64 stereo frames per callback. Pitch calculations use a reciprocal of 48,000, and WAV initialization also writes 48,000 as metadata. Together these support a nominal 48 kHz DSP timebase. **The recovered math assumes 48 kHz; this is not an independent physical measurement of the converter clock.** At that nominal rate, a block spans approximately 1.333 ms.

A DWT cycle-counter read at `0xe0001004` appears in processing/profiling logic. This is useful evidence that timing is monitored, but it is not itself a measurement of module CPU usage in this analysis.

SPI5 initialization is present. It must not automatically be labeled the SD interface. The actual filesystem `diskio` transport has not been conclusively traced, and the lack of a dedicated SDMMC interrupt handler does not rule out a polled SDMMC path.

## 5. Internal memory and Array organization

### 5.1 Working spectral buffers

| RAM address | Interpretation | Evidence level |
|---|---|---|
| `0x20002a40` | Side A SAO working coefficients, 64 floats | B/T |
| `0x20002b40` | Side A SAM amplitudes, adjacent 64-float bank | B/T |
| `0x20002c40` | Side B SAO working coefficients | B/T |
| `0x20002d40` | Side B SAM amplitudes | B/T |
| `0x20000d40` | Side A SAO per-sample coefficient increments | B/T |
| `0x20000e40` | Side A adjacent SAM increment bank, cleared by analyzer | B/T |
| `0x20000f40` | Side B SAO increments | B/T |
| `0x20001040` | Side B adjacent SAM increment bank | B/T |
| `0x20000a20` | Side A Array descriptors | B/T |
| `0x20000920` | Side B Array descriptors | B/T |

The normal reader produces a target spectrum, then stores `(target-current)/64` in the increment bank. The sample loop adds those increments to working coefficients. SAM amplitude production writes its adjacent bank and clears corresponding increment entries rather than using an identical interpolation path.

This memory layout explains why storage frame size and active oscillator count must be treated separately: vectors occupy 64 values even though the traced normal loops process at most 60 terms.

### 5.2 External-memory bases — keep the two sides separate

Main writes **two distinct external-memory base pointers**:

```text
0x20002f74 -> 0x60c01000   [A base]
0x20002f70 -> 0x60001000   [B base]
```

It clears `0x00c00000` bytes at each base: two 12 MiB regions, or 24 MiB explicitly initialized. That proves the initialized extent, not the board's total installed SDRAM.

A descriptor is 16 bytes. Its first word is an offset in **floats**, followed by two dimension words and a save-state marker. Side A slot offsets begin at `slot * 65536`; Side B offsets begin at `1048576 + slot * 65536`, relative to Side B's separate base. Thus the first B Array is around `0x60401000`, not immediately following the first A Array.

There are 16 descriptors per side. Each reserved slot accommodates 65,536 float32 coefficients, or 1,024 frames of 64 values. The factory initializer copies a 16 KiB coefficient block and uses dimensions 8 × 8. The remaining reserved memory includes working/capture space not fully mapped in this pass.

**Correction to the first reply:** the earlier “64 MB SDRAM” statement was not justified by the evidence presented there. This report relies on the actual initialized ranges and does not promote them into a claim about installed board capacity.

## 6. SAM: what the analyzer actually computes

### 6.1 Reference generation rather than a conventional FFT front end

The region around `0x0802f296–0x0802f4aa` generates harmonic quadrature references using an 8,192-entry sine table, interpolation, a quarter-cycle cosine offset and recurrence arithmetic. The analyzer then multiplies the input by those references and low-pass filters the results.

Conceptually, each detector asks: **how much of the current input is coherent with a particular harmonic of the analyzer's reference frequency?** That is a different implementation from taking fixed FFT bins, grouping them and feeding a generic oscillator bank. No FFT stage is needed to explain the traced SAM path. This is a path-specific conclusion, not a proof that no FFT-related routine exists anywhere in the whole image.

### 6.2 Input conditioning and quadrature smoothing

Each side includes a DC-removing recurrence equivalent to:

```text
u[n] = 12 * input[n]
y[n] = u[n] - u[n-1] + 0.995 * y[n-1]
```

For each active harmonic, in-phase and quadrature products each pass through two cascaded one-poles:

```text
v = y[n] * reference[n]
s1 = v  + p * (s1 - v)
s2 = s1 + p * (s2 - s1)
```

The two outputs are squared and summed. The result is converted to a magnitude-like value by a bit-level square-root approximation, not an ordinary library `sqrt()` call.

The A analyzer is visible around `0x0802f716–0x0802f9bc`; the mirrored B analyzer precedes it. State arrays contain two values per harmonic for each quadrature. The bank loops are unrolled in groups of four.

### 6.3 Slide and Focus mapping

For normalized analyzer Slide `s`, a table-based exponential gives approximately:

```text
q = trunc(8191 * s)
referenceIncrement = 2^(q / 2048) / 2400
```

This is implemented by a 2,048-entry fractional-octave lookup plus a power-of-two octave shift. At 48 kHz, normalized Slide 0–1 corresponds to approximately 20–319.892 Hz before harmonics are formed.

With normalized Focus `f`, the recovered pole mapping is approximately:

```text
w = 1 - 0.98*f
p = min(1 - 3.14*referenceIncrement*w*w, 0.9999799728393555)
```

The assembly mixes single and double precision, so the equation expresses its structure rather than promising identical rounding at every step. Increasing Focus moves the poles closer to one: narrower, slower detectors. Because two filters are cascaded, do not equate the total detector response to a single one-pole attack time.

The upper pole cap corresponds to roughly a one-second single-pole time constant at 48 kHz. This makes response time and spectral selectivity strongly related. Replacing the analyzer with an FFT and an arbitrary envelope follower would miss that coupling unless it were deliberately reconstructed.

### 6.4 Magnitude approximation and the zero case

The recovered unsigned integer transformation is:

```c
uint32_t u = bits_of_float(energy);
u = (u - 0x00800000u) >> 1;
u += 0x20000000u;
float magnitude = float_from_bits(u);
```

The approximation is coarse compared with a correctly rounded square root. It also has a non-obvious zero case: zero energy maps to bits `0x9fc00000`, a tiny negative value of approximately `-8.13e-20`, rather than exact positive zero. The supplied literal helper preserves this; it does not silently “repair” the firmware's arithmetic. Subnormal handling and the enabled flush-to-zero mode are further reasons not to claim host-level bit identity.

A cleaner `sqrt` implementation might be preferable for a new product, but that would be an intentional redesign. Keep a compatibility path while determining whether the approximation contributes audible level or transient differences.

## 7. Standard synthesis and the actual role of Partials

### 7.1 Do not model Partials as only a volume envelope

In the standard synthesis branches, Partials controls a radius-like parameter:

```text
r ≈ min(0.65*p + 0.35*p², 1)
```

Here `p` is the calibrated/smoothed internal Partials value, not a statement that panel voltage maps directly to a normalized floating-point number. The first product is single precision, followed by mixed-precision arithmetic.

The carrier basis itself then depends on `r`. For an odd-bank phase, define:

```text
x = r*cos(thetaOdd)
C0 = r²
C1 = x
Cn = 2*x*C(n-1) - C(n-2), for n >= 2
```

For an even-bank phase:

```text
y = r*sin(thetaEven)
z = r*cos(thetaEven)
E0 = 0
E1 = y
En = 2*z*E(n-1) - E(n-2), for n >= 2
```

The two accumulations use alternating signs over the odd/even coefficient positions. At `r=1`, the recurrences reduce to familiar trigonometric harmonic identities with a phase/sign convention. At smaller `r`, they do not simply equal `r^n` multiplied by the original nth harmonic. In particular, the odd recurrence uses **C0 = r²**, not the textbook Chebyshev seed of one.

This is why a superficially plausible “60 sine oscillators with a harmonic cutoff knob” would not necessarily sound like this implementation. The Partials gesture changes the polynomial waveshaping basis as well as the aggregate level.

The supplied `polynomial_synthesis()` is a mathematical probe of this branch. It intentionally takes independent odd/even phases and does not invent the unresolved surrounding phase/FM wiring.

### 7.2 Group admission and the 60-term ceiling

Both reference-bank processing and standard resynthesis are unrolled four terms at a time. The initial admission threshold is approximately `increment < 0.1125`; subsequent groups are admitted against approximately `groupBoundary * increment < 0.45`. The normal synthesis loop terminates at 60 processed terms.

Consequently, high-frequency term removal is groupwise. It is not necessarily equivalent to individually dropping every harmonic above Nyquist. The precise `0.45` margin, floating-point comparison, and four-term grouping are part of the recovered behavior. The model helper only covers nonnegative increments; behavior during extreme signed FM needs further tracing.

The last four values of the 64-value storage frame should not be deleted from imported files. They remain part of the serialized representation even though this normal branch does not accumulate them.

### 7.3 Compensation and clipping

A fitted gain-compensation polynomial follows the accumulation:

```text
g(r) = 9.508619308
       - 27.75853920*r
       + 32.67438126*r²
       - 13.46592999*r³

if r <= 0.0399999991:
    g(r) *= 25*r
```

The low-radius taper matters: it prevents the compensation fit from turning the bottom of the Partials control into an abrupt residual signal. At `r=1`, the fitted gain is approximately 0.95853, rather than exactly one.

Several output-related joins use a cubic soft clip with an explicit input clamp:

```text
x = clamp(x, -1.5, 1.5)
y = x * (1 - 0.1481481493*x²)
```

It maps the clamped endpoints close to ±1. This does not mean every jack passes through exactly one copy of this clipper. Individual output lanes, gains and clip sites need to be followed separately before a faithful final mixer is specified.

## 8. SAO: linear and planar spectral navigation

The two reader families are clearly separate. Their detailed addressing, interpolation and Focus equations are in `DSP_SPEC.md` and `ARRAY_FORMAT.md`.

The linear reader treats a bank as a sequence of spectra. For `N` frames and integer clock offset `k`, its position is approximately `k + (N-1)*Slide`. It interpolates neighboring 64-value frames, then applies a Focus-dependent amplitude transformation. A table supplies an exponent-like value; a float-bit approximation implements the power operation. There is an exact comparison that bypasses the transform when the internal `0.75*Focus` equals `0.25`. Do not interpret this as a proof of the panel knob's physical center position.

The planar reader constructs a grid extent from `ceil(sqrt(N))`, uses Focus and Slide as the two coordinates, and interpolates four spectra. Its wrap arithmetic is not a generic modulo torus. It subtracts the frame count only once at selected points and has a special row increment rule at the boundary. These details matter particularly for non-square Arrays and clock offsets. The supplied probe refuses an out-of-range address instead of reproducing an unsafe read.

Both modes drive 64-sample coefficient ramps. The nominal block duration and coefficient-update interval therefore belong in the model; merely smoothing controls at the host's block size would not necessarily reproduce the same modulation behavior.

Manufacturer documentation independently identifies linear and planar SAO and assigns Focus/Slide to their different roles [S3]. The exact equations here are instruction-derived rather than inferred from those descriptions.

## 9. Arrays are coefficient files, not sampled sound files

The firmware discovers `spect*.wav`, `speca*.wav` and `specb*.wav`. Its per-side save routine generates slot names such as `speca000.wav` and `specb015.wav`. The lower layer resembles FatFs, including directory enumeration and short-filename handling. A generic RIFF/WAVE parser is present.

The save path writes 64 coefficients per spectrum as 16-bit integer samples. The WAV's single channel is the serialization stream; it is not “audio side A” in a stereo recording, and its nominal duration is not the original capture duration. A full 1,024-frame bank occupies 131,072 payload bytes when saved as PCM16. The default 64-frame bank occupies 8,192 payload bytes.

RAM stores float32 coefficients. Save conversion uses a scale of 32,767 and truncation. The PCM16 reader uses division by 32,768. This introduces a small predictable round-trip scaling/quantization difference. A peak-like state variable conditionally scales data upward when it lies strictly between zero and one; the save routine writes the scaled coefficients back into Array RAM. Thus saving is not necessarily a numerically passive operation.

A notable static finding is a probable WAV header inconsistency: the PCM16 path appears to retain `byteRate=192000` and `blockAlign=4` while declaring one channel and 16 bits per sample at 48 kHz. The expected standard PCM16 values would be 96,000 and 2. The parser's bit-depth-driven sample conversion could explain why such files remain usable internally. **This has not yet been checked against a WAV physically saved by a Spectraphon.** It should be treated as a strong static finding awaiting a simple independent file check, not as a verified interoperability guarantee.

The included WAV reader bounds-checks chunks, enforces a complete 64-value frame structure, reports header inconsistencies, and rejects nonfinite values. The optional writer defaults to a standards-correct header. An explicit experimental option produces the observed inconsistent header fields. No produced file is claimed hardware-tested.

## 10. Noise, Chaos and auxiliary modulation

### 10.1 Noise: identifiable components, incomplete complete-equation translation

The SP67 application contains a dedicated Noise branch. One traced path begins around `0x08031038`. Its ingredients include:

- A 32-bit linear congruential generator with multiplier `0x0bb38435` and increment `0x3619636b`.
- Random-state conversion using a near-`1/2^32` scale and a `-0.5` offset.
- Smooth segment interpolation involving the sine table.
- A table of 256 geometrically spaced filter/rate coefficients, with 24 steps per octave.
- Multiple filter states, normalization logic, and modulation of sine-like carriers.

This is enough to rule out a faithful implementation consisting only of independent white noise at the outputs. It is not enough to supply a fully validated Noise engine. Filter update ordering, exact state limits, LF behavior and all mode-dependent input offsets remain to be translated.

### 10.2 Chaos: a structured multi-oscillator branch, not an unspecified random generator

A separate branch around `0x0802fa04` uses phase state, modulation and feedback paths. Focus participates in a `1 + 21*Focus` mapping with interpolation between neighboring integer relationships; Partials and Slide enter modulation/feedback scaling. This supports a structured coupled-oscillator interpretation rather than “play random noise.” The complete coupled equations and their state-update ordering are still unresolved.

It would be irresponsible to invent a Lorenz oscillator or generic cross-FM algorithm and label it a reconstruction of this branch. A creative approximation is possible, but should have a distinct compatibility status.

### 10.3 Sub/CV and envelope behavior

Four 1,024-sample periodic tables are extracted: ramp, sine, triangle-like and strongly shaped. Their numerical contents are known, but every table-to-output-mode association has not been traced. Sample-and-hold/smoothed-random logic reuses the pseudorandom generator family.

A traced input-level follower uses approximately 0.1 attack weight and 0.001 release weight per sample. Its comparison-dependent behavior is equivalent to fast attack and slow release. Clock-related GPIO dispatch and counters are visible, but a complete clocked-LFO state machine, including timeout and restart rules, is not yet available.

The manufacturer documents Sine/Sub behavior separately from the FM bus, and distinguishes sub-oscillator and clocked CV roles [S1]. The binary exposes separate phase and output paths consistent with that description, without yet resolving every physical jack assignment.

## 11. Controls, pitch, FM and output routing

The firmware contains separate slow-control and audio-rate CV buffers. The clearest fast-CV trace uses four halfwords per sample frame at `0x30000040`: the alternating channels lead to the A/B Partials and pitch paths. Slow ADC slots 4 and 5 feed a steep fourth-power mapping `60*u^4`, strongly associated with the two FM-index controls. **Those slow slots must not be mislabeled Partials.**

A pitch conversion uses a 2,048-step fractional-octave table, an octave shift, a constant near 1.021974921 and division by 48,000. A useful internal-coordinate interpretation is approximately:

```text
frequency ≈ 1.021974921 * 2^(q/2048 + 4)
```

The constant times 16 is about 16.3516 Hz. A low-frequency branch divides the relevant rate by 256, an eight-octave change. This does not by itself establish the complete FREQ knob calibration, V/oct offset, accepted CV range or physical low-frequency switch behavior.

The input stereo buffer is around `0x30000440`. Output stores reach four interleaved pairs around `0x30000840`, `0x30000c40`, `0x30001040` and `0x38000000`, establishing eight digital output lanes. Some lanes use Q30-like conversion; others use a different scale. The full lane-to-jack map is not sufficiently proven to print a falsely definitive output pin table.

`CONTROL_IO_MAP.md` therefore separates public panel roles, known buffer/field addresses, and unresolved calibration/routing. An implementation can expose the correct conceptual ports now, but should not claim voltage-accurate equivalence until the physical mapping and gains are checked.

## 12. Extracted tables and why they matter

Nine tables are exported as exact little-endian float32 binary and human-readable CSV, with addresses, element counts and SHA-256 hashes. The factory Array also has a frame-oriented CSV.

The 8,192-entry sine and 2,048-entry exponential tables closely match their expected analytic functions. The triangle-like and strongly shaped tables deserve preservation rather than replacement by an ideal waveform: the sampled details and any phase conventions may be intentional or may reflect the method used to create them. The Focus exponent table captures an otherwise easy-to-miss nonlinear mapping.

The factory data form 64 spectral frames of 64 values. Some coefficients exceed one. That alone disproves an importer rule that every internal float coefficient must already be clamped to `[0,1]`. The safe PCM16 writer deliberately rejects overflow instead of reproducing unchecked narrowing or silently changing the data. For analysis, retain the original floats.

The recovered factory bank is a direct asset extraction from the uploaded firmware, not an independently authored recreation. Technical accessibility is not a statement of permission to redistribute firmware-derived tables in a commercial plugin.

## 13. What the included tests do — and do not — establish

The supplied 31 host tests pass. They check firmware identity, exact ELF payload preservation, table integrity, analytic table fits, the square-root zero case, Focus bypasses, Array interpolation, frame limits, PCM conversions, malformed RIFF rejection, and polynomial identities at unity radius.

The unity-radius test is particularly useful: the translated recurrences reduce to their expected trigonometric identities. It catches several plausible sign and seed mistakes. It does **not** prove that the surrounding phase wiring, input normalization or hardware output path is correct.

The tests do not execute the ARM binary. They do not compare generated audio with the real module. They do not prove save files are accepted by the updater or firmware. A passing test suite is evidence about the supplied reconstruction tools, not a substitute for a hardware comparison.

## 14. Limits and corrections worth carrying forward

The first-pass response correctly identified the image and many startup addresses, but was too confident about the ultimate completeness of recovery. The present analysis narrows that confidence into specific, reviewable claims.

Important boundaries:

1. **MCU family is stronger than exact part identification.** H743/H753-class is supported; a precise package/revision is not.
2. **No claim of 64 MB installed SDRAM is carried forward.** Two 12 MiB initialization spans are directly established.
3. **“No opaque wrapper” is not “no security anywhere.”** The missing bootloader may have behavior unavailable in this file.
4. **No FFT in the traced SAM algorithm is not a whole-program absence proof.** It is enough to guide a model of this path.
5. **64 stored coefficients are not 64 actively synthesized terms.** The normal loops' ceiling is 60.
6. **Noise/Chaos and the full UI are partially mapped, not finished.** Their names and a few constants are not a complete model.
7. **48 kHz is strongly supported as the DSP's assumed timebase, not externally measured.**
8. **WAV header quirks and raw edge arithmetic need independent files or hardware traces.** The tools fail safely when the recovered indexing would be unsafe.

These qualifications are not cosmetic. They determine which parts Codex can implement as recovered behavior and which parts need separate experimental status.

## 15. Highest-value continuation and implementation order

For a Leviathan implementation, begin with the spectral representation, the two reader modes, and the standard Partials recurrence. Those have the strongest direct reconstruction and executable probes. Add the SAM detector bank with its actual Slide/Focus coupling, 64-frame cadence and numerical compatibility options. Keep exact lookup data behind an explicitly reviewable asset boundary.

Next, trace the complete phase/FM and even-output offset path. That determines whether a model which looks spectrally correct also responds correctly to FM, Sync and input controls. Only then should a hardware-output gain comparison be treated as decisive.

For further reverse engineering, the single most informative additional artifact is a hardware-saved Array WAV. It can immediately confirm or reject the header inconsistency, payload shape, file naming and save normalization assumptions. A small set of controlled captures can then expose spectrum timing and detector bandwidth. A second firmware revision would help isolate mode changes, but no genuine binary diff was performed here because only this snapshot was available.

See `CODEX_HANDOFF.md` for implementation boundaries, acceptance tests and a prioritized experiment matrix.


---

# Recovered DSP equations and implementation boundaries

This document translates selected SP67 paths into implementation-oriented notation. It is not the original source and not a complete module emulator. All addresses apply only to the hash in `REPORT.md`. The Python probes in `reference/sp67_reference.py` implement the stated subset.

## 1. Numeric conventions

Use `f32` for an IEEE754 single-precision rounding step, `bits()` for reinterpretation as uint32, `float_bits()` for the inverse, and `FMA32(a,b,c)` for a single-rounded `a*b+c` operation. `trunc` is toward zero. Bit shifts and additions in the magnitude helper are unsigned modulo 2^32.

The firmware uses both float32 and float64 operations and sets flush-to-zero. The mathematical oscillator/filter probes use host arithmetic in some places; only the explicitly bit-oriented helpers attempt to preserve the observed bit transforms. Native host `fmaf` supplies float32 FMA in those helpers. This is still not a full emulation of Cortex floating-point exception, default-NaN, signed-zero or flush-to-zero semantics.

`VFMA d,n,m` accumulates `d+n*m`; `VFMS d,n,m` gives `d-n*m`; `VFNMS d,n,m` gives `n*m-d`. Source S6 is the architecture-level check on these signs. The odd/even recurrence depends on reading them correctly.

## 2. Fundamental lookup tables

| Name | Address | Elements | Analytic relationship / use |
|---|---|---:|---|
| `sine_8192` | `0x0803a674` | 8192 | approximately `sin(2*pi*i/8192)` |
| `exp2_2048` | `0x08043a74` | 2048 | approximately `2^(i/2048)` |
| `focus_exponent_1024` | `0x08042a74` | 1024 | approximately `1.25 * 0.4^(i/1023)` |
| `noise_rate_256` | `0x08042674` | 256 | geometric, approximately 24 indices/octave |

A positive-cycle phase `phase` is converted into table position `P = phase*8192`. The lower integer bits wrap through an `&8191` index, while the fractional part interpolates neighboring entries. A 2048-index offset gives the cosine reference. Exact phase overflow handling must be taken from the particular call site, especially under negative FM.

Do not generate new tables merely because the intended analytic function is recognizable when comparing against the firmware. Exact float32 table samples are already available. Conversely, a distributable independent implementation may deliberately generate its own analytic tables rather than ship extracted assets.

## 3. Sample cadence and coefficient updates

Nominal sample rate `Fs = 48000` is embedded in DSP rate calculations. Audio callbacks supply 64 stereo sample frames. The relevant loop uses an interleaved word index 0,2,...,126.

For each coefficient `a_i`, the SAO reader writes:

```text
delta_i = (target_i - current_i) * 0.015625
```

The sample path advances the working coefficient by `delta_i`. In exact float32 arithmetic, 64 additions may not land bit-for-bit on the original target; do not silently snap to the target and call it exact. A redesigned Rack version may choose a final snap, but that is an implementation choice.

The host's processing block should not define the synthetic hardware cadence. For a compatibility implementation, keep an internal 64-sample clock at a defined DSP rate. At arbitrary Rack rates, the simplest comparison architecture is a 48 kHz core with explicit resampling. A native-rate implementation instead needs time-constant and cutoff adaptation and will no longer be trivially identical.

## 4. SAM reference frequency and detector pole

Evidence: setup around `0x0802e998–0x0802ea0c` and mirrored setup; quadrature bank around `0x0802f296`; detector blocks around `0x0802f4ac` and `0x0802f716`.

For normalized Slide `s`:

```text
q = trunc(f32(s*8191))
fractionalOctave = exp2Table[q & 2047]
octaveMultiplier = 1 << (q >> 11)
inc = fractionalOctave * octaveMultiplier / 2400
```

For normalized Focus `f`:

```text
w = f32(1 - 0.98*f)
p = min(f32(1 + double(inc)*(-3.14)*double(w)*double(w)),
        0.9999799728393555)
```

The exact compiler sequence can differ in intermediate rounding from the compact expression. `analysis_parameters()` intentionally describes the mapping, not every Cortex instruction.

Per side, use one input-conditioning history and a state vector for each quadrature/harmonic:

```text
u = 12 * input
conditioned = u - previous_u + .995 * previous_conditioned
previous_u = u
previous_conditioned = conditioned

for each active harmonic h:
    vi = conditioned * cosineReference[h]
    vq = conditioned * sineReference[h]
    i1[h] = vi + p*(i1[h] - vi)
    i2[h] = i1[h] + p*(i2[h] - i1[h])
    q1[h] = vq + p*(q1[h] - vq)
    q2[h] = q1[h] + p*(q2[h] - q1[h])
    energy = i2[h]^2 + q2[h]^2
    magnitude[h] = fast_sqrt_bits(energy)
```

Sine/cosine sign conventions may rotate the quadrature state without changing its ideal magnitude. To reproduce instruction-level transients, use the exact reference ordering and FMA sequence, not only the energy equivalence.

At most 60 normal terms are processed in groups of four. The reference increment is distinct from oscillator pitch: SAM can listen to one harmonic grid while synthesizing the resulting amplitudes at another frequency. The exact panel/CV calibration into normalized `s` and `f` remains a separate task.

## 5. Literal fast square root

```text
u = uint32(bits(energy))
u = uint32(u - 0x00800000)
u = u >> 1
u = uint32(u + 0x20000000)
magnitude = float_bits(u)
```

There is no visible Newton refinement in the traced detector loop. The transform creates a characteristic approximation error even for ordinary positive normalized inputs. At exactly zero it produces `0x9fc00000`. The helper rejects negative/nonfinite *input energy* as invalid caller input, but it deliberately retains this output quirk.

For a future exactness test, feed explicitly represented float32 energies spanning exponent boundaries and denormals. Compare raw output bits to instruction emulation with the same FPSCR, not simply to a square root tolerance.

## 6. Standard Partials recurrence

Evidence: side A normal branch `0x08030cd4` onward; the seed computations at `0x08030d60`, `0x08030dd8`; unrolled loop `0x08030e42–0x08030f44`; compensation `0x08030f94–0x08030fd2`. Side B has a mirrored branch around `0x0803092c`.

Let the internal smoothed Partials value be `p`. For nonnegative valid operation:

```text
r ≈ min(float32(float64(float32(0.65f*p)) + 0.35*float64(p)*float64(p)), 1)
```

Define separate phase arguments `thetaOdd` and `thetaEven`, because the exact phase-engine wiring is not fully reconstructed:

```text
x = r*cos(thetaOdd)
C0 = r*r
C1 = x
Cn = 2*x*C(n-1) - C(n-2)

z = r*cos(thetaEven)
E0 = 0
E1 = r*sin(thetaEven)
En = 2*z*E(n-1) - E(n-2)
```

For amplitudes `a[0..63]`, with at most 60 terms admitted:

```text
oddSum  = a[0]*C1 - a[2]*C3 + a[4]*C5 - a[6]*C7 + ...
evenSum = -a[1]*E2 + a[3]*E4 - a[5]*E6 + a[7]*E8 - ...
```

These signs come from the accumulation instructions, not from the public labels “Odd” and “Even.” At `r=1`, the expected trigonometric identities provide a useful independent algebraic check. Below unity radius, the recurrence generates polynomials of a scaled sinusoid and mixes lower harmonics of the corresponding parity. A simple per-harmonic amplitude weighting is not the same mapping.

The odd seed deserves emphasis. A textbook recurrence initialized with `C0=1` is not this code when `r != 1`. Nor is inserting a factor of `r²` on the second recurrence term justified: the observed recurrence subtracts the preceding state without that factor.

### Groupwise term selection

For a nonnegative increment `inc`, the recovered model is:

```text
if inc >= float32(.1125): active = 0
else:
    active = 4
    while active < 60 and float32((active+4)*inc) < float32(.45):
        active += 4
```

This is a translation of the ordinary positive-rate path, not a promise about all extreme negative-FM states. For example, `inc=.01` yields 44 terms in the supplied helper. A generic “keep n where n*f < Nyquist” implementation changes both the cutoff margin and grouping.

### Gain compensation

```text
g = 9.508619308 - 27.75853920*r + 32.67438126*r² - 13.46592999*r³
if r <= 0.03999999911: g *= 25*r
oddSum *= g
evenSum *= g
```

The assembly's FMA order is authoritative for exact rounding. The Python helper evaluates the mathematical polynomial; it is intended for unit tests and exploratory models.

## 7. Linear Array interpolation and Focus transform

Evidence: A `0x0802cd0c`; B `0x0802ce5c`.

Let `N=dim1*dim2`, `k` be the current integer clock offset, and `s` normalized Slide:

```text
position = FMA32(float32(N-1), s, float32(k))
i = trunc(position)
t = position - trunc(position)
j = i + 1
if i >= N: i -= N
if j >= N: j -= N
for coefficient c in 0..63:
    a = frame[i][c] + t*(frame[j][c] - frame[i][c])
```

Those are **one-subtraction wrap rules**, not arbitrary integer modulo. Upstream state normally constrains the indices. The reference helper throws an error if the translated addressing leaves the valid bank.

For normalized Focus `f`, prepare:

```text
kFocus = float32(.75f*f)
exponent = focusTable[min(trunc(f*1024),1023)]
gain = float32(1.25f - kFocus)
```

Each interpolated amplitude is then transformed as follows:

```text
if a <= 0 or kFocus == .25f:
    target = a
else:
    tBits = signed32(uint32(bits(a) + 0xc0876c0b))
    z = FMA32(exponent, float32(tBits), float32(1064866816))
    reconstructed = float_bits(uint32(trunc_to_signed32(z)))
    target = float32(gain*reconstructed)
```

This is a float-bit-domain approximation to a power family, not a standard `pow(a, exponent)`. The signed bit bias and the float32-rounded integer-like constant matter. A bypass is exact equality at a particular internal control value; it is not a broad neutral deadband. Nonpositive coefficients also bypass, and the gain multiplier is not applied on that bypass path.

Finally compute the 64-sample increment vector. This makes Focus a spectral amplitude-distribution transform in linear SAO, not the analyzer bandwidth control used in SAM.

## 8. Planar reader: preserve the actual indexing rules

Evidence: A `0x0802cfac`; B `0x0802d0f8`.

For a positive frame count `N`:

```text
g = ceil(sqrt(N))
h = g - 1
x = k + Focus*h
y = Slide*h
xi = trunc(x); tx = x - trunc(x)
yi = trunc(y); ty = y - trunc(y)

row0 = yi-h   if yi >= g else yi
row1 = yi+1-h if yi >= h else yi+1

indices = [row0*g+xi, row0*g+xi+1, row1*g+xi, row1*g+xi+1]
for each index:
    if index >= N: index -= N
```

Interpolate along the Focus coordinate within the two selected rows, then interpolate those results along Slide. Apply no linear-reader Focus power transform in this branch. Store increments over 64 samples.

The last-row rule can select row 1 rather than row 0. Fractional weights of zero hide some of this behavior at exact corners. Non-square banks and excessive clock offsets expose the edge arithmetic more clearly. The safe helper rejects out-of-bounds addresses instead of guessing an intended toroidal topology.

A complete clone should recover the upstream constraints that make these rules safe in ordinary use. Replacing them with `%N` may be a sensible independent design, but should be identified as a deviation.

## 9. Input follower, FM-index curve and output saturation

A traced level follower is:

```text
v = abs(input)
if v > envelope:
    envelope = .9*envelope + .1*v
else:
    envelope = .999*envelope + .001*v
```

Fast Partials paths include approximately .01 per-sample smoothing. The exact number and order of smoothing stages and panel calibrations should be taken from the corresponding path, not globally assumed identical for every control.

Slow ADC slots 4 and 5 participate in:

```text
fmIndex = 60 * normalizedControl^4
```

This yields a strongly compressed lower part of the knob range. It is an FM-index mapping, not the Partials-radius curve. The external/internal FM source combination and signed phase increments remain incomplete.

A repeatedly observed soft clip is:

```text
x = min(1.5, max(-1.5,x))
y = x * (1 - 0.14814814925193787*x*x)
```

Output conversion includes different fixed-point scales on different digital lanes. Do not apply a universal voltage scale or identical clipping path to all eight outputs without completing the lane trace.

## 10. Explicitly absent from the reference model

The executable probes do not model: full audio callback state ordering; calibrated ADC conversion; complete carrier/sub/even phase relationships; through-zero or extreme FM; Sync and Follow; button debounce and long-press timing; capture start/stop and clock timeout; full Noise or Chaos equations; all Sub/CV modes; exact save-state persistence; analog gain, clipping or noise; converter clocking; bootloader/update behavior.

These omissions should stay visible in any Codex handoff. The probes are useful building blocks and regression references, not an opaque package to be relabeled a finished emulator.


---

# Spectraphon Array persistence and WAV interchange

## 1. Three different representations

Keep these distinct:

| Layer | Representation |
|---|---|
| Live/working spectrum | 64 float32 values in internal RAM |
| Stored Array in external RAM | Multiple 64-float spectra, with a separate four-word descriptor |
| Saved Array file | RIFF/WAVE container; the observed save path emits 16-bit mono coefficient samples |

A spectrum is not an FFT complex vector and does not store a phase value alongside each magnitude. The observed normal Array reader consumes one real float per coefficient. File “samples” are serialized coefficient values, not the original incoming audio samples.

Evidence: readers at `0x0802cd0c`, `0x0802ce5c`, `0x0802cfac`, `0x0802d0f8`; converter at `0x08033434`; saver at `0x08033af8`.

## 2. Slot descriptors and addresses

A four-word descriptor has the observed layout:

```text
offset +0:  float index offset relative to the side's external-memory base
offset +4:  dimension 1
offset +8:  dimension 2
offset +12: save-state marker; zero enters the save path, nonzero skips it
```

The last field must not be simplistically called a “present” flag. In the save routine it behaves like dirty/unsaved state; the exact lifecycle across every UI transition is not fully reconstructed.

A-side descriptor base is `0x20000a20`; B-side descriptor base is `0x20000920`. Each contains 16 entries. Main's initializer uses 65,536-float spacing per slot. A offsets start at zero; B offsets include 1,048,576 floats, but relative to a separate B memory base.

```text
A absolute spectrum address = *(0x20002f74) + 4*descriptorA.offset
B absolute spectrum address = *(0x20002f70) + 4*descriptorB.offset

A base pointer value = 0x60c01000
B base pointer value = 0x60001000
```

At initialization, the default coefficient block is copied from `0x08045a98` and dimensions are set to 8 × 8. This source block contains 4,096 floats: 64 frames of 64 coefficients.

Capacity arithmetic:

```text
1 RAM frame       = 64 * 4 = 256 bytes
1 maximum slot    = 1024 * 256 = 262144 bytes
1 PCM16 frame     = 64 * 2 = 128 bytes
maximum WAV data  = 1024 * 128 = 131072 bytes
factory WAV data  = 64 * 128 = 8192 bytes
```

The file adds RIFF headers/chunks to those payload sizes. Storage capacity does not establish capture duration; frame cadence depends on capture/clock state, which is incompletely translated.

## 3. Names and discovery

Embedded wildcard strings are:

```text
spect*.wav
speca*.wav
specb*.wav
```

The observed per-side save names follow `speca000.wav` through `speca015.wav`, and the B equivalent. The enumeration path and a filename character walk fit a short-filename FatFs configuration. Exact mount options, all case handling and the meaning of every generic `spect*` path remain incompletely identified.

The inferred FatFs-like routines are labeled in the curated symbol CSV. Their names are analyst assignments, not symbols extracted from a linked library.

## 4. Saved WAV structure and the header anomaly

The save path selects internal sample-format code `0x73693136`, follows the signed-16 conversion path, and writes 128 payload bytes for every spectrum. The header builder/finalizer declares mono audio with nominal sample rate 48,000 and 16-bit samples.

The significant apparent inconsistency is:

| Header field | PCM16 save-path value inferred from instructions | Standard mono PCM16 value |
|---|---:|---:|
| Format tag | 1 | 1 |
| Channels | 1 | 1 |
| Sample rate | 48000 | 48000 |
| Byte rate | **192000** | **96000** |
| Block align | **4** | **2** |
| Bits per sample | 16 | 16 |

The common finalization sequence around `0x080338f4–0x0803390a` writes 192,000 and 4; the PCM16 branch around `0x08033998` selects 16 bits. The sequence then writes the header. The static translation therefore points to stale 32-bit byte-rate/alignment fields, not a 32-bit coefficient payload.

**Validation status:** no hardware-saved WAV was supplied. This is an instruction-derived finding awaiting file confirmation. A parser should report the inconsistency and use the actual format/bit-depth fields to validate payload structure, rather than blindly trust block alignment. A standards-correct writer should not reproduce the quirk by accident.

The optional `firmware_header_quirk=True` parameter in the reference encoder is deliberately explicit and experimental. It is not needed for ordinary coefficient inspection and is not a guarantee of hardware compatibility.

## 5. Quantization and save-time mutation

A per-side peak-like scalar controls conditional normalization:

```text
if 0 < trackedPeak < 1:
    gain = 1 / trackedPeak
else:
    gain = 1
```

Each coefficient is multiplied by this gain; the scaled float is written back to the Array RAM. The integer save conversion is approximately:

```text
integerSample = trunc(double(scaledCoefficient) * 32767.0)
```

The complete floating conversion/narrowing behavior for invalid or overflowing inputs is not asserted here. No explicit saturating clamp is visible in the traced store sequence. The reference writer **rejects** out-of-int16 values rather than reproducing unchecked narrowing.

The observed signed-16 read conversion uses a factor of `1/32768`, not `1/32767`. Thus, ignoring gain and float rounding, the round trip is approximately:

```text
loaded = trunc(savedFloat * 32767) / 32768
```

This is a real numerical distinction. The safe tool's encoder does not mutate the caller's data, while the firmware does update its stored floats. The API documentation makes that divergence explicit.

Some factory coefficients are larger than one. Do not destroy them during extraction merely to make a generic audio encoder happy. Raw float32 exports are the lossless representation of the extracted bank.

## 6. Input formats: parser recognition is not complete support

The generic WAV machinery recognizes RIFF/WAVE, `fmt `, `data`, and an additional `clm ` identifier, plus chunk walking/padding. It contains sample conversion branches for unsigned PCM8, signed PCM16/24/32 and IEEE float32. Other format tags seen in parsing are not enough to claim that compressed formats work as spectral Arrays.

The included tool supports only mono PCM8/16/24/32 and mono IEEE float32. It intentionally rejects ADPCM, mu-law, WAVE_FORMAT_EXTENSIBLE, multiple data chunks, nonfinite float data and oversized/incomplete spectral payloads. This is a bounded inspection/interchange tool, not a claim to emulate every permissive firmware parser behavior.

The tool normalizes integer PCM using standard divisors. The firmware's 24-/32-bit conversion uses a nearby float32 constant (`0x2ffffff6`) and intermediate integer-to-float rounding, so the tool is not an instruction-exact replacement for those conversion branches. PCM16's divisor agrees exactly with the observed 1/32768 scaling.

## 7. How file length becomes Array dimensions

Main's load path uses a special case for 4,096 scalar values:

```text
if sampleCount == 4096:
    dim1 = 8
    dim2 = 8
else:
    dim1 = sampleCount >> 6
    dim2 = 1
```

This is visible around `0x08034fd6–0x080350c6`. The special case matches the 64-frame factory bank. No extra custom dimensional header is necessary to explain normal files in this path. The generic parser's handling of other chunks does not establish that every possible metadata field is ignored elsewhere.

An importer should validate that the payload contains a whole number of 64-value frames and at most 1,024 frames. It should not infer stereo Arrays simply because a generic WAV library can open stereo files. The tool requires one channel because arbitrary multichannel Array semantics were not established.

## 8. Using the included tool

Run from the bundle root:

```bash
python reference/sp67_reference.py inspect-wav /path/to/speca000.wav
python reference/sp67_reference.py inspect-wav /path/to/speca000.wav --frames-json frames.json
python reference/sp67_reference.py extract-factory factory_frames.json
python tests/test_reference.py
```

The factory extraction command writes the coefficients already recovered from this firmware; it does not synthesize or guess their values. The inspect command reports format fields, inferred dimensions and header inconsistencies.

For programmatic use:

```python
from pathlib import Path
from reference.sp67_reference import decode_array_wav, encode_pcm16

info, frames = decode_array_wav(Path('speca000.wav').read_bytes())
print(info.frame_count, info.inferred_dimensions, info.warnings)
# Experimental output; not validated on a physical module.
encoded = encode_pcm16(frames)
Path('array_standard_header.wav').write_bytes(encoded)
```

Encoding can fail when coefficients exceed the representable int16 range. That failure is intentional: inspect or explicitly normalize the data rather than silently clipping it. Do not place experimental files on the only copy of a working SD card.

## 9. Independent checks that would most reduce uncertainty

The first comparison should use an untouched hardware-saved Array file, not a DAW-exported copy. Check the six format fields, actual data length, coefficient count and name. Compare the input file before and after a module load/save cycle to detect normalization or scaling.

For capture timing, use a controlled input tone and record a small Array at a known external clock interval. That can establish whether each capture tick stores one spectrum, how start/end events are handled, and whether the final frame count differs from the simple number of ticks. Those timing semantics should not be invented from the file format alone.


---

# Controls, modes, I/O and confidence map

## 1. Public module interface versus recovered internals

Make Noise describes two interacting spectral sides with eight outputs, two audio inputs, two gate inputs and ten CV inputs. SAM analyzes incoming sound; SAO uses stored spectra. Sine/Sub paths have a distinct relationship to the FM bus, and side B provides Follow/Sync interaction [S1]. Those statements establish intended user-facing behavior, not a recovered electrical schematic.

The compact mode map below follows the manufacturer's cheat sheet [S3]. The subsequent addresses and implementation notes come from the uploaded binary.

| Mode | Partials role | Focus role | Slide role |
|---|---|---|---|
| SAM | Harmonic activation/timbre | Analysis selectivity | Analysis reference frequency |
| Linear SAO | Harmonic activation/timbre | Coefficient compression/expansion | Position along the Array |
| Planar SAO | Harmonic activation/timbre | Fine grid coordinate | Coarse grid coordinate |
| Noise | Sideband width | Noise high-pass control | Noise low-pass control |
| Chaos | Cross-modulation depth | Oscillator ratio | Feedback |

This table is a semantic index. It is not a claim that the same ADC value has identical calibration or smoothing in every mode.

## 2. Pitch and FM controls

The recovered pitch math uses an octave table, integer/fractional splitting and a 48 kHz denominator. The internal coordinate can be expressed approximately as `1.021974921 * 2^(q/2048+4)` Hz. The path around `0x080305c4` interacts with Follow-like state but has not been translated enough to give a definitive Follow ratio or calibration equation.

The fourth-power FM-index mapping is visible in setup around `0x0802e87e–0x0802e8e8`: `60*u^4`. Two slow ADC slots, 4 and 5, feed the relevant paths. The complete mix between FM bus, external FM and pitch-rate terms needs further tracing. No numeric CV voltage limits, attenuverter polarity or through-zero specification should be inferred merely from the normalized curve.

The LF branch divides a rate by 256. That is a directly observed rate change, while exact mode-entry, state persistence and every alternate path remain separate questions.

## 3. Fast-CV acquisition

Fast samples use four halfwords per frame around `0x30000040`. Tracing the current paths gives:

| Slot | Calibration-index association | Destination interpretation | Confidence |
|---:|---:|---|---|
| 0 | 9 | A Partials, smoothed field around `0x20000880` | High path-level interpretation |
| 1 | 8 | A pitch-related path | High path-level interpretation |
| 2 | 11 | B Partials, smoothed field around `0x200023fc` | High path-level interpretation |
| 3 | 10 | B pitch-related path | High path-level interpretation |

The table does not identify physical ADC pins or convert volts into these readings. The calibration object around `0x200144d4` is not fully reconstructed. Coarse/fine frequency-control summation, offsets, gain errors, polarity and clipping remain to be mapped into a voltage-accurate implementation.

Slow ADC samples are based around `0x30000020`. The first four slots participate in Slide/Focus-related paths; slot order on side B should be rechecked before assigning physical pin labels. Slots 4–5 have the stronger FM-index interpretation described above.

## 4. Digital input and output buffers

Input audio is read as interleaved 32-bit words around `0x30000440`, with a conversion constant `0x2ffffff6` (approximately `4.656610098e-10`, near `1/2^31`). This is not a measured volts-per-code specification.

Output writes reach four paired regions:

| Pair base | Observed stores in the sample path | Interpretation status |
|---|---|---|
| `0x30000840` | Two soft-clipped signal lanes | Pair established; physical jack assignment not fully proven |
| `0x30000c40` | Two additional audio lanes | Pair established; trace final lane identity separately |
| `0x30001040` | Audio/auxiliary-related lane stores | Roles still require final mapping |
| `0x38000000` | Additional sine/CV-related stores | Roles still require final mapping |

The principal store block is around `0x080302b8–0x0803038c`, with pointer setup earlier around `0x0802ec94–0x0802ed12`. The eight-lane structure agrees with the public output count [S1], but agreement in count does not prove ordering.

A voltage-faithful model still needs: final per-lane gains, signed conversion limits, physical jack correspondence, analog scaling, DAC/codec selection, and any reconstruction filtering. Preserve independent output paths rather than collapsing them into one stereo sum.

## 5. Buttons, clock and mode fields

The GPIO callback at `0x08032688` distinguishes arguments 64 and 128. The two branches update clock counters/flags around:

```text
A: counter 0x20002ee0; flag 0x20002edc
B: counter 0x20002ee8; flag 0x20002ee4
```

Application UI/clock processing is concentrated around `0x0802d900`; control acquisition around `0x0802d464`; persistent-state interactions around `0x0802d280`. These are good entry points for continuing decompilation, but the complete state machine has not been reconstructed.

Mode-related fields include A `0x20002e8c` combined with `0x20002e94`, and B `0x20002e88` combined with `0x20002e90`. Their branch results select standard, Noise and Chaos paths. Do not encode those raw storage fields directly as a stable public API; their initialization and transitions are part of the unresolved state machine.

Current Array selection is associated with A `0x20002430` and B `0x20000b20`. Reader clock offsets are associated with A `0x20002438` and B `0x20002434`. The distinction between selection and offset matters: changing a slot and stepping a frame are separate operations.

The manufacturer's cheat sheet documents button combinations and mode-dependent Clock/Shift behavior [S3]. This bundle does not claim to recover every debounce threshold, long-press edge, menu priority, timeout, persistence flag or startup-button gesture. For implementation, keep a documented behavior state machine distinct from raw firmware-equivalence claims.

## 6. Missing I/O facts worth measuring

A pin-accurate or voltage-accurate port should wait for a schematic, board inspection or controlled measurements. In particular, the current evidence does not establish every jack's usable voltage range, output amplitude at each setting, impedance, calibration tolerance, negative-FM behavior, clock trigger threshold, or sample alignment between all output pairs.

For a Rack prototype, expose normalized internal parameters and explicit output lanes first. Then implement a replaceable panel/CV calibration layer. This avoids baking uncertain analog assumptions into an otherwise well-grounded DSP core.


---

# Codex handoff — a staged, evidence-led Spectraphon-inspired implementation

## Objective

Use the SP67 analysis to build a modular spectral instrument with clearly identified compatibility boundaries. This is not a request to execute or modify the uploaded firmware. The first deliverable should be a tested independent DSP core and data model, not an untestable monolithic Rack module.

Read `REPORT.md`, `DSP_SPEC.md`, `ARRAY_FORMAT.md`, `CONTROL_IO_MAP.md`, and the reference helper docstrings before implementation. Analyst names and equations are reconstructed, not original source. Where a behavior is unresolved, retain an explicit design decision rather than quietly pretending the gap is recovered.

## Non-negotiable facts to preserve in the baseline

The storage unit is 64 coefficients per spectrum. The normal active synthesis bank reaches at most 60 terms in four-term groups. The DSP math assumes 48 kHz and uses 64-frame updates. SAM is a harmonic quadrature detector bank with cascaded one-poles. Linear and planar SAO are different readers. Partials changes a polynomial recurrence, with the odd seed `C0=r²`, and has a fitted compensation curve. The linear Focus function is a bit-domain power approximation with exact bypass behavior. Save/read use different 32767/32768 scaling.

These are more important to a faithful core than copying panel cosmetics or using a fashionable generic FFT implementation.

## Proposed architecture

```text
SpectralFrame          std::array<float, 64>
SpectralArray          bounded vector<SpectralFrame>, dimensions, provenance
ArrayBank              16 slots per side; independent persistent metadata
QuadratureAnalyzer     reference generator, DC blocker, 60 detector states
ArrayReader            linear and planar modes, explicit edge policy
SpectralInterpolator   64-value current and delta vectors, internal cadence
PolynomialOscillator   odd/even basis, active-group guard, gain compensation
PhaseEngine            independent phases; unresolved links explicit
ModeEngine             standard / experimental Noise / experimental Chaos
ControlCalibration     normalized controls and CV interpretation
ClockUiState           documented transitions, independently testable
OutputStage            separate per-lane gains/converters, not a single stereo mix
ArrayFileCodec         bounded RIFF parsing and explicit normalization policy
RackAdapter            parameters, ports, serialization, UI; no filesystem in audio thread
```

Do not allocate, parse files, acquire blocking mutexes or regenerate tables inside the real-time sample path. Build Array snapshots outside that path and publish them through a bounded handoff. The exact concurrency machinery should fit the existing Leviathan engine conventions; this report did not inspect the current plugin source and does not invent filenames or claim integration work already exists.

## Phase A — data model and deterministic numerical probes

Implement the frame/bank model, table loader or independent table generator, safe WAV parser, linear reader and planar reader. Port the Python tests to C++ with explicit float32/FMA policy. Add a switch between recovered one-subtraction addressing and a redesigned safe wrap policy, and record that selection in patch state.

Acceptance: complete 64-value frames; at most 1024 frames per slot; exact imported float preservation where possible; malformed RIFF rejection; explicit warnings for the observed header inconsistency; no silent clipping of factory coefficients; reproducible interpolation at boundaries; no out-of-range reads under malformed input.

Treat the extracted factory spectra and lookup files as separately reviewable assets. Analysis possession does not settle redistribution rights. A production distribution may need independently generated waveforms and independently authored spectral presets. Do not ship the raw firmware merely because it is present in this research bundle.

## Phase B — standard oscillator

Port the recurrence and compensation curve before adding complex modulation. Expose independent odd/even phases for tests. Match the unity-radius trigonometric identities, zero-Partials muting, 60-term ceiling, four-term admission and soft-clip equation. Use the assembly addresses in `DSP_SPEC.md` to audit sign and seed choices.

Acceptance: deterministic reference vectors; no coefficient 60–63 contribution in the normal branch; correct polarity of odd/even sums; an explicit distinction between mathematical reference arithmetic and an instruction-close float32 implementation; stable output for zero input and all normalized parameters.

Do not replace the recurrence with `amplitude[h]*r^h*sin(h*phase)` and call it equivalent. Do not use a conventional Chebyshev seed of one below full Partials.

## Phase C — SAM analyzer

Implement the separate analysis reference frequency, input DC blocker and cascaded quadrature filters. Keep analyzer pitch independent of synthesis pitch. Preserve Slide/Focus coupling. Implement the literal magnitude approximation as a compatibility option and an ordinary square root as a clearly labeled alternative.

Acceptance: known-tone selectivity sweeps, amplitude-step response, phase-insensitive steady magnitudes, absence of unsafe denormal/nonfinite propagation, correct table bounds, and repeatable behavior at the pole cap. Compare both magnitude options before assuming their differences are inaudible.

For arbitrary Rack sample rates, choose and document either an internally resampled 48 kHz core or a native-rate adaptation. A native-rate adaptation needs pole/timebase conversion; merely replacing one sample-rate constant while retaining every smoothing coefficient changes behavior.

## Phase D — phase/FM, clocks, capture and output mapping

This phase needs more evidence than the current numerical core. Trace complete external/internal FM source combination, the `60*u^4` index path, even-output offsets, signed phase behavior, Follow/Sync and Sine/Sub independence. Keep a separate confidence label on inferred routing.

Capture should store spectral frames, not input waveform slices. Determine capture tick cadence, frame count, start/end semantics, wrap behavior and saturation from code or controlled hardware files. Until then, implement a declared independent capture policy instead of claiming a recovered one.

Acceptance: explicit clock state tests, simultaneous side independence, deterministic reset/sync behavior, no audio-thread disk work, stable output-lane mapping, and documented unverified analog gains.

## Phase E — Noise, Chaos and auxiliary CV

Only begin a faithful port after translating the corresponding branch completely. Current findings identify the LCG, coefficient table, filters and coupled-phase ingredients, but do not provide a verified full algorithm.

A creative mode can be implemented earlier, but must be described as an independent Noise/Chaos design. It should not be used as evidence that the firmware branch has been reconstructed. Preserve all user patch-state distinctions between faithful and experimental modes.

## Hardware/fixture experiment matrix

| Experiment | Input and controls | What it discriminates |
|---|---|---|
| Untouched saved Array | One module-saved file, before any DAW rewrite | Header anomaly, bit depth, payload shape, file naming |
| Load/save round trip | Known coefficient bank, unchanged controls | 32767/32768 scaling and save-time normalization |
| Single-tone SAM sweep | Fixed analyzer controls, slowly swept sine | Harmonic detector centers and bandwidth |
| Tone amplitude step | Fixed tone, two Focus settings | Cascaded filter response and magnitude approximation |
| Analysis/playback separation | Fixed input tone, sweep oscillator pitch separately | Independent analysis and synthesis reference paths |
| Sparse spectral bank | One nonzero coefficient at a time | Index order, active 60-term ceiling, odd/even signs |
| Partials sweep | Sparse coefficients 1, 3, 5, then mixed | Polynomial-basis behavior versus simple harmonic attenuation |
| Boundary Array | Distinct values in every frame | Linear/planar edge indexing and clock offsets |
| FM comparison | Internal bus then equivalent external signal | Source normalization, phase wiring, Sine/Sub independence |
| Capture at known clocks | Short sequence of distinct input spectra | Frame timing and capture termination |
| LF mode clocks | Known repeated clocks and missing clocks | Divider, timeout and CV-state behavior |
| Output level test | Low-level sine and near-clip spectra | Digital/analog gain, clip order and lane correspondence |

No fixture in this table is claimed to have been run. The 31 included tests are host-side mathematical/file checks, not these hardware experiments.

## Definition of done for a first useful port

The first useful port can legitimately include standard SAM and both SAO readers while leaving Noise/Chaos and physical calibration incomplete. It should expose that status directly in developer documentation, retain a reproducible reference-test suite, and avoid describing itself as bit-exact. A polished UI is not a substitute for the unresolved routing and timing work.


---

# Tools, artifact map and reproduction

## Quick start

From the extracted bundle root:

```bash
python scripts/export_assets.py
python tests/test_reference.py
python reference/sp67_reference.py extract-factory /tmp/factory_frames.json
```

These commands use Python 3.10+ and the standard library. Some numerical tests need a host C library providing `fmaf`, normally available through libm on Linux. They do not execute ARM instructions.

For disassembly in an environment with LLVM's ARM target support:

```bash
python scripts/inspect_firmware.py 0x08030cd4 0x08030fd2
python scripts/build_inventory.py
```

The script currently loads `/usr/lib/x86_64-linux-gnu/libLLVM-19.so`. Adjust that explicit path for another installation. No LLVM binary or font files are bundled. The renderer used for the HTML edition needs the separately installed Python `mistune` package; the Markdown reports remain directly readable without it.

## Artifact map

| Path | Contents / caveat |
|---|---|
| `README.md` | Starting point and scope |
| `docs/REPORT.md` | Main integrated analysis |
| `docs/DSP_SPEC.md` | Instruction-derived equations |
| `docs/ARRAY_FORMAT.md` | Memory descriptors, RIFF format, save/read behavior |
| `docs/CONTROL_IO_MAP.md` | Interface semantics and partially recovered routing |
| `docs/CODEX_HANDOFF.md` | Staged implementation and validation plan |
| `docs/SOURCES.md` | External sources, provenance and limits |
| `Spectraphon_SP67_Analysis.html` | Combined report, readable offline |
| `firmware/sp67.bin` | Exact copy of uploaded bytes |
| `firmware/sp67_analysis_mapped.elf` | Synthetic address-mapped analysis wrapper |
| `tables/*.bin` | Exact extracted little-endian float32 arrays |
| `tables/*.csv` | Addressed values and raw float bits |
| `tables/factory_spectra_frames.csv` | Factory bank in 64-coefficient rows |
| `analysis/identity.json` | Size/hash/startup identity |
| `analysis/symbols_curated.csv` | Analyst-assigned names with confidence |
| `analysis/dsp_regions.csv` | Important blocks inside the large DSP function |
| `analysis/memory_map_curated.csv` | Working RAM and external-memory map |
| `analysis/findings.json` | Structured findings and confidence |
| `analysis/open_questions.json` | Unresolved questions, evidence needed |
| `analysis/vector_table.csv` | All 166 vector words |
| `analysis/function_candidates.csv` | Heuristic entry/size/FP-count inventory |
| `analysis/call_graph.csv` | Direct and boundary edges; not a complete indirect call graph |
| `analysis/literal_xrefs.csv` | PC-relative literal references |
| `analysis/cfg_issues.csv` | Unresolved branches/switches/decode concerns |
| `analysis/strings.*` | Raw printable strings, including false positives from code/data |
| `analysis/entropy_4k.csv` | Byte entropy and zero fraction in 4 KiB windows |
| `analysis/numerical_metrics.json` | Helper-derived curve/error measurements |
| `analysis/test_results.json` | Host-test results and explicit scope |
| `disassembly/fn_*.asm` | Candidate-function disassembly, not original source |
| `disassembly/reachable_and_candidate_code.asm` | Combined candidate code |
| `disassembly/linear_sweep_UNVERIFIED.asm` | Includes inline data misread as code; not authoritative |
| `reference/sp67_reference.py` | Bounded WAV tool and DSP probes |
| `tests/test_reference.py` | 31 host-side consistency tests |
| `SHA256SUMS.txt` | Integrity manifest for delivered files |

## Importing into a reverse-engineering tool

The synthetic ELF is ELF32 little-endian ARM with a load segment at `0x08020000` and Thumb reset entry `0x08036431`. Its `.firmware` section contains every uploaded byte, including vector table, code, literal pools, numerical tables and terminal zeros. It is intentionally **not split into authoritative original linker sections**.

Let the tool read the ELF mapping. Confirm little-endian ARM/Cortex-M interpretation and Thumb context at the actual executable entries. Use the reset handler and vector targets as trusted entry points; do not disassemble the entire unsplit section as though it were all code. Apply the table extents from the manifest as data definitions. Add RAM blocks separately where required for analysis; the ELF load segment is not a dump of runtime RAM.

The ELF has analyst-created function/object symbols. They are useful hints, not recovered debug symbols. No Ghidra/IDA decompiler run was completed in this environment, so no original C output or validated Ghidra project is claimed. The ELF header and payload were checked with `readelf` and the host tests.

Never flash the analysis ELF. It is a research wrapper, not a vendor update file.

## Inventory limitations

The inventory combines vector roots, direct-call targets, pointer candidates and aligned prologue heuristics. Its control-flow walker follows common conditional branches and a subset of compiler switch-table patterns. Indirect branches, unusual entry points and function boundaries remain uncertain. The explicit issue CSV should guide subsequent decompiler work.

A manual correction removes a false boundary at `0x080336f4`: the WAV initializer actually begins at `0x080336f0`, before its push instruction. This illustrates why the inventory's candidate count must not be mistaken for the original function count.

## Reproduction scope

Table bytes, hashes, ELF payload mapping and script output can be reproduced directly from the upload. Mathematical translations can be reviewed against the listed addresses. Exact dynamic behavior, analog levels and physical file acceptance need evidence outside this bundle. Keep those two categories separate when evaluating test results.


---

# Sources and provenance

## Primary evidence: uploaded firmware

The user's `sp67.dat` is the primary source for the recovered addresses, constants, tables, memory organization and translated equations. Its SHA-256 is `b50a87b9bd4f6ad90007918a16e8a4d033e2236c863faec55f5585abe34927d9`. Every flash address in this bundle uses base `0x08020000`. The raw input is preserved unchanged as `firmware/sp67.bin`.

The byte-level evidence is not supplied by the external pages below. Public documentation is used to constrain semantics and check architecture, not to manufacture exact DSP equations that the manual does not state.

## External sources consulted

### S1 — Make Noise product page

https://www.makenoisemusic.com/modules/spectraphon/

Manufacturer source for module identity, dual SAM/SAO architecture, interface counts, and relationships between FM, Sine/Sub and Follow/Sync. Accessed 28 September 2026. No claim about current retail price is used.

### S2 — Make Noise firmware listing

https://www.makenoisemusic.com/firmware/

Manufacturer listing identifies SP67 as a Spectraphon firmware download at access time. This listing is not a hash verification of the uploaded image. The upload's identity is verified internally, not by claiming a downloaded vendor file was compared byte-for-byte.

### S3 — Make Noise Spectraphon cheat sheet

https://www.makenoise-manuals.com/spectraphon/spectraphon-cheat-sheet.pdf

Manufacturer's three-page reference for mode-dependent control roles and UI gestures. The mode matrix on page 3 was visually checked. This is a concise semantic cross-check, not an engineering specification. The full current manual PDF could not be reliably retrieved through the available web path; this bundle does not claim to have exhaustively checked it.

### S4 — STMicroelectronics STM32H743/753 family description

https://www.st.com/en/microcontrollers-microprocessors/stm32h743-753.html

Primary source for Cortex-M7 family features, including double-precision floating point. It supports the architecture-family interpretation, not the exact chip marking on the user's module.

### S5 — STMicroelectronics official Cortex-M7 device startup source

https://raw.githubusercontent.com/STMicroelectronics/cmsis-device-h7/master/Source/Templates/gcc/startup_stm32h743xx.s

Primary device source used to compare vector ordering and handler names. A current startup template need not have identical initialization ordering to this compiled firmware. No specific compiler/HAL version is inferred from superficial startup similarity.

### S6 — ARMv7-M Architecture Reference Manual, DDI 0403E.b

ARM-authored document, hosted by an academic mirror:

https://www.profdong.com/elc4438_spring2016/DDI0403E_B_armv7m_arm.pdf

Section A7.7.231 (printed pages A7-517 onward) supplies fused multiply-add/subtract sign conventions relevant to the recurrence. In particular, VFNMS computes product minus the old destination. The document is used for instruction semantics, not to assert that the STM32H7 only implements the older minimum feature set described in that manual.

## Negative provenance statements

No vendor source code, debug ELF, bootloader dump, board schematic, physical chip marking, hardware-generated Array file, hardware recording, second firmware revision or original compiler map was available for this pass. No successful Ghidra/IDA decompilation or ARM dynamic-emulation session is represented by the artifacts.

The factory spectra and lookup tables are directly extracted vendor data from the user-provided firmware. The analysis scripts, reports and reference probes are newly authored reconstruction work. Inclusion in a research bundle is not an assertion of redistribution permission for vendor assets.
