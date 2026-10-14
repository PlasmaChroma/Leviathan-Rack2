# Recovered DSP equations and implementation boundaries

**Continuation:** [RACK_RECONSTRUCTION.md](RACK_RECONSTRUCTION.md) adds instruction-checked SAM/Noise/Chaos, calibration, interaction/phase/FM, Sub/CV, output and capture definitions. Section 3 gives the exact SAM control, reference and detector schedules; twelve complete SAM callbacks now check nonzero/silent inputs and all eight outputs. Its scope/status supersedes the unresolved-path list at the end of this initial specification.

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

This yields a strongly compressed lower part of the knob range. It is an FM-index mapping, not the Partials-radius curve. The current continuation's section 4 and `phase_step()` define the checked internal cross-FM sources, signed truncation-based wrap, even-phase pull and Follow/Sync integration. Physical external-CV summing remains outside the digital definition.

A repeatedly observed soft clip is:

```text
x = min(1.5, max(-1.5,x))
y = x * (1 - 0.14814814925193787*x*x)
```

Output conversion includes different fixed-point scales on different digital lanes. The completed digital lane trace is in reconstruction section 8: preserve per-side FMA rounding and separate Sub/CV scaling. This does not establish a universal jack-voltage scale.

## 10. Scope of the initial probes and current continuation

The initial `sp67_reference.py` probes remain intentionally narrower than the
continuation. `sp67_extended.py` and the original-instruction tests now define
Noise/Chaos, digital calibration/control paths, phase relationships, Follow/Sync,
all Sub/CV modes, capture/timeout, selected button/long-hold combinations,
settings codecs and file behavior. Complete SAM, SAO and Noise/Chaos callbacks
check integration on explicit finite trajectories. Consult
[CODEX_HANDOFF.md](CODEX_HANDOFF.md) for subsystem-to-test mappings rather than
treating this initial file's omissions as the current implementation boundary.

Remaining limits include the complete composed gesture/persistence state
machine, broader extreme and long-duration trajectories, physical panel/CV and
output gain, actual media/flash/converter timing and the missing bootloader.
The current models and fixtures are not a finished whole-board emulator.
