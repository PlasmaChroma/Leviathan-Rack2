# Spectraphon SP67 — firmware and DSP reconstruction

**Historical initial analysis:** [RACK_RECONSTRUCTION.md](RACK_RECONSTRUCTION.md) supplies the 2026-10-04 instruction-tested definitions and current gap status. All statements below about absent ARM execution or unresolved Noise/Chaos, phase, capture and output paths describe the initial pass, not current coverage. [CODEX_HANDOFF.md](CODEX_HANDOFF.md) reconciles all 15 original behavioral questions and maps current subsystems to executable evidence. The initial report is retained for provenance.

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
