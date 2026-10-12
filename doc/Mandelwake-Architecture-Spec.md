# Mandelwake Architecture Specification

Status: **Version-one architecture semi-final; MVP-prototype ready**  
Target: **Unreleased 18 HP Leviathan module**  
Last revised: **2026-08-01**

This document is the authoritative version-one specification for Mandelwake. It
consolidates the useful decisions from `Mandelwake-DR.md` and
`mw-refinements.md`, resolves their known inconsistencies, and removes
research-era assumptions that should not constrain implementation.

The source documents remain useful design history. If they disagree with this
specification, this document takes precedence for version one.

“Semi-final” means the product and deterministic behavior are accepted as the
implementation baseline. A decision may still be refined before release, but a
change must be intentional, recorded here, and followed through into tests and
golden vectors where applicable. Physical panel validation and generated
reference artifacts remain open implementation gates rather than unresolved
product design.

## 1. Product Definition

Mandelwake is a clocked, deterministic, polyphonic complex-orbit modulation
generator. It turns a seed, a navigable fractal viewport, clocks, and control
voltages into correlated continuous CV and repeatable musical events.

As with the strongest Leviathan modules, the visualization is a prime feature,
not decoration added after DSP. Mandelwake must make its deterministic orbit
legible as an iconic instrument state: the sound-control behavior and the
display are two expressions of the same evolving wake.

It is a musical state machine, not a conventional fractal browser and not a
band-limited oscillator. An accepted clock step advances a bounded number of
complex-map micro-iterations. The resulting orbit state becomes four
continuous modulation targets and three event streams.

### 1.1 Core identity

| Decision | Resolution |
|---|---|
| Width | 18 HP; validate the measured layout before freezing IDs |
| Polyphony | 1–16 independent orbit channels |
| State core | Signed Q4.28 fixed point with explicitly bounded `int64_t` intermediates |
| Maps | Mandelbrot, Julia, Burning Ship |
| Clocking | External polyphonic clock or internal sample-rate-independent clock |
| Continuous outputs | X, Y, Radius, Phase |
| Event outputs | Gate, Escape, Step |
| Signature visualization | Abyssal Wake: bioluminescent orbit trail, trident head, bathymetric contours, and escape rupture |
| DSP allocation | None after construction |
| DSP locks | None |
| DSP worker | None |
| Visual backend | NanoVG with cached static layers |
| Visual cadence | At most 30 Hz in Normal/High quality |
| External dependencies | None beyond Rack and the existing repository |

### 1.2 Musical model

CENTER X and CENTER Y choose the center of a fractal viewport. The seed chooses
a stable point within that viewport for each channel. ZOOM changes the size of
the viewport and therefore changes the actual recurrence content. X and Y CV
move the seeded point inside the viewport. MUTATION introduces a deterministic
step-indexed perturbation at the current viewport scale.

This gives each control one clear role:

- CENTER chooses the region.
- SEED chooses a repeatable location and trajectory within the region.
- ZOOM changes the scale of exploration.
- X/Y CV navigate locally.
- MUTATION prevents static interior points from becoming musically inert.
- ITERATIONS controls the amount of orbit evolution per accepted step.
- SMOOTH controls voltage motion without changing event decisions.
- DENSITY controls orbit-correlated gate probability.

### 1.3 Explicit non-goals for version one

- arbitrary formulas or user scripting;
- a live high-resolution fractal heatmap;
- a generic oscilloscope-style XY trace as the finished visual identity;
- GPU compute;
- a band-limited audio-rate oscillator;
- MIDI clock synchronization;
- polyphonic visual overlays;
- unbounded particle effects;
- a DSP worker thread;
- DENSITY CV;
- adding ports after the panel and ID layout are frozen.

High internal or external clock rates are supported, but X/Y/Radius/Phase remain
zero-order-held or slewed control signals. They are not anti-aliased and are not
advertised as audio oscillator outputs.

## 2. Maps and Orbit State

Each of the sixteen persistent channel slots owns independent orbit state, step
count, clock state, pulse counters, smoothing state, and visual history. Active
polyphony controls which slots advance and appear at outputs; it does not
allocate or destroy state.

| Map | Recurrence | Reset state |
|---|---|---|
| Mandelbrot | `z' = z² + c + m` | `z = 0` |
| Julia | `z' = z² + c + m` | Seed-derived bounded `z` |
| Burning Ship | `z' = (abs(x) + i*abs(y))² + c + m` | `z = 0` |

Changing MAP resets all sixteen channel slots on the next engine sample. It clears
orbit history and event pulses, reconstructs deterministic state from the same
base seed, and suppresses an orbit step on that sample. MAP changes do not
generate a new base seed.

RESET reconstructs the current map from the existing base seed. Reset scope is
explicit:

- a monophonic RESET input edge broadcasts to all sixteen slots;
- a polyphonic RESET input resets only the corresponding connected channels;
- Rack `onReset()` is global and resets all sixteen slots;
- MAP change and RESEED are global and reset all sixteen slots.

Every affected reset clears that channel’s event pulses and history, restores
its map-specific initial state, sets its orbit step index to zero, and inserts
the initial bounded point as the first history entry. RESET does not change the
base seed. RESEED changes the base seed and then performs a global reset.
Consequently:

- RESET repeats the same sequence;
- RESEED produces a different sequence;
- duplicating or saving a patch preserves its sequence;
- Rack randomize changes the seed only when SEED LOCK is off.

### 2.1 Step index, feature, and seed order

After reset, the first accepted step uses `stepIndex == 0`. All mutation,
re-entry, and GATE hashes for that accepted step use the same index. A
micro-iteration that escapes uses its zero-based micro-iteration index for
re-entry. The engine then extracts Radius and Phase from the final stored
bounded state, performs the GATE trial, appends one history point, and increments
`stepIndex` modulo `2^64`. STEP fires for every accepted step; ESCAPE fires if
any micro-iteration escaped. An escaped candidate itself is never published as
continuous output or history.

A new module obtains its initial 64-bit base seed from two Rack random words.
Once assigned, the base seed is ordinary persisted state and the pure engine
uses no entropy source. Each RESEED edge advances the seed by the exact rule:

```text
candidate = mix64(baseSeed ^ 0x4D575F5245534544)  // MW_RESED
baseSeed = candidate != baseSeed
    ? candidate
    : candidate ^ 0x9E3779B97F4A7C15
```

Holding the momentary RESEED parameter produces only one edge. SEED LOCK does
not block an explicit RESEED, pasted seed, or seed derivation command; it only
prevents Rack’s general randomize action from replacing the seed. When unlocked,
Rack randomize obtains a fresh 64-bit seed from two Rack random words before the
global reset. Pure-engine tests always inject an explicit seed.

Construction and patch load synchronize the RESEED edge detector to the current
parameter level. A loaded or automated high value cannot reseed until it first
returns low and rises again.

RESEED and SEED LOCK set Rack parameter randomization disabled, so general
randomize neither presses the button nor changes the lock decision it is about
to consult. `onRandomize()` performs one global reset after other parameters
have randomized and synchronizes the stored MAP selector so the next process
sample does not repeat that reset. It retains the base seed when locked and
replaces it when unlocked.

## 3. Viewport and ZOOM Semantics

ZOOM changes the recurrence input, not merely display magnification or output
gain.

Let the effective zoom be `Z` octaves in `[0, 12]` and:

```text
zoomScale = 2^(-Z)
```

For deterministic cross-platform behavior, effective ZOOM is rounded to the
nearest 1/256 octave, with halfway cases away from zero, before entering the
orbit engine. `zoomScale` is obtained from a checked-in 3,073-entry Q4.28 table:

```text
zoomScaleQ28[i] = roundHalfAwayFromZero(2^(-i / 256) * 2^28)
```

The committed integer table and its checksum are part of algorithm version
one. No `pow()` or `exp2()` is called from the audio path.

For each channel, the base seed deterministically supplies normalized viewport
coordinates `seedX` and `seedY` in `[-1, +1)`. These values remain constant
until RESEED.

At an accepted step:

```text
xCvNorm = clamp(X_INPUT / 5 V, -1, +1) * X_AMOUNT
yCvNorm = clamp(Y_INPUT / 5 V, -1, +1) * Y_AMOUNT

viewportX = clamp(seedX + xCvNorm, -2, +2)
viewportY = clamp(seedY + yCvNorm, -2, +2)

cX = CENTER_X + zoomScale * 1.625 * viewportX
cY = CENTER_Y + zoomScale * 1.500 * viewportY
```

The 1.625 and 1.500 constants are half the full CENTER X and CENTER Y panel
ranges. Effective `cX` and `cY` are finally clamped to the representable and
documented control domains:

```text
cX in [-2.25, +1.00]
cY in [-1.50, +1.50]
```

This definition ensures that:

- ZOOM affects an unpatched monophonic channel because the seed supplies a
  stable viewport location;
- deeper ZOOM values produce finer movement around CENTER;
- X/Y CV remain musically useful without redefining output scaling;
- identical seed, channel, parameters, and sampled CV produce identical `c`.

### 3.1 Mutation scaling

MUTATION is a normalized viewport-relative amount from 0–100%, not an absolute
coordinate-unit parameter.

```text
mutationDepth = 0.25 * MUTATION * zoomScale
mX = mutationDepth * signedHashX(seed, channel, step, microIteration)
mY = mutationDepth * signedHashY(seed, channel, step, microIteration)
```

The signed hash values lie in `[-1, +1)` and are converted directly into fixed
point. The version-one default is 24%, which yields a maximum mutation depth of
0.015 coordinate units at the default two-octave ZOOM:

```text
0.25 * 0.24 * 2^(-2) = 0.015
```

MUTATE CV is bipolar and additive before the normalized MUTATION value is
clamped to `[0, 1]`.

## 4. Fixed-Point Arithmetic Contract

### 4.1 Representation

```cpp
using OrbitQ28 = int32_t;

constexpr int kFractionBits = 28;
constexpr int64_t kScaleQ28 = int64_t{1} << kFractionBits;
constexpr int64_t kComponentSafetyQ28 = int64_t{4} << kFractionBits;
constexpr int64_t kEscapeRadiusSquaredQ56 = int64_t{1} << 58;
```

The mathematical escape radius is 2, so escape occurs when `|z|² > 4`. The
component safety bound is ±4. It is a wider arithmetic guard and must not be
confused with the escape radius.

### 4.2 Defined recurrence order

For each micro-iteration:

1. Widen stored X and Y to `int64_t`.
2. Clamp each input component to `[-kComponentSafetyQ28,
   +kComponentSafetyQ28]`.
3. For Burning Ship, compute a safe absolute value in `int64_t`; never apply
   32-bit `abs()` to `INT32_MIN`.
4. Compute `x*x`, `y*y`, and `2*x*y` in `int64_t`.
5. Divide products by `kScaleQ28` with an explicitly tested rounding rule.
6. Add fixed-point `c` and mutation in `int64_t`.
7. Clamp each candidate component to the ±4 safety bound before squaring it.
8. Compute candidate radius squared in `int64_t`.
9. Compare with `kEscapeRadiusSquaredQ56`.
10. On escape, emit the event and apply deterministic re-entry without
    first narrowing the escaped candidate.
11. Otherwise narrow the bounded candidates to `OrbitQ28`.

This order guarantees that recurrence products, the doubled XY product, and
the escape-square sum remain inside signed 64-bit range.

Signed shifts, negative division rounding, saturation, fixed-point conversion,
integer absolute value, and narrowing must use named helpers with golden tests.
Implementation-defined signed right-shift behavior is not part of the contract.

All signed fixed-point division uses **round to nearest, halfway away from
zero**. The helper determines the sign, performs quotient and remainder work on
unsigned magnitudes, increments the magnitude when twice the remainder is at
least the positive divisor, and reapplies the sign. This rule applies equally
to positive and negative values and does not depend on compiler signed-shift or
signed-remainder behavior.

### 4.3 Deterministic hashing

Mutation, seed-derived viewport coordinates, re-entry, and GATE decisions use
this exact unsigned 64-bit SplitMix64 finalizer. Unsigned overflow is intentional.

```cpp
uint64_t mix64(uint64_t x) {
    x += UINT64_C(0x9E3779B97F4A7C15);
    x = (x ^ (x >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
    x = (x ^ (x >> 27)) * UINT64_C(0x94D049BB133111EB);
    return x ^ (x >> 31);
}
```

Every random word is constructed in this exact order:

```cpp
uint64_t orbitHash(uint64_t baseSeed,
                   uint64_t domainTag,
                   uint32_t channel,
                   uint32_t map,
                   uint64_t step,
                   uint32_t microIteration) {
    uint64_t h = mix64(baseSeed ^ domainTag);
    h = mix64(h ^ uint64_t(channel));
    h = mix64(h ^ uint64_t(map));
    h = mix64(h ^ step);
    return mix64(h ^ uint64_t(microIteration));
}
```

Fields irrelevant to a particular purpose are passed as zero; they are never
omitted or packed differently. Version one freezes these ASCII-derived domain
tags:

| Purpose | Tag |
|---|---:|
| Viewport X | `0x4D575F5649455758` (`MW_VIEWX`) |
| Viewport Y | `0x4D575F5649455759` (`MW_VIEWY`) |
| Julia initial X | `0x4D575F4A554C4958` (`MW_JULIX`) |
| Julia initial Y | `0x4D575F4A554C4959` (`MW_JULIY`) |
| Mutation X | `0x4D55544154455F58` (`MUTATE_X`) |
| Mutation Y | `0x4D55544154455F59` (`MUTATE_Y`) |
| Re-entry X | `0x5245454E54525F58` (`REENTR_X`) |
| Re-entry Y | `0x5245454E54525F59` (`REENTR_Y`) |
| Gate trial | `0x4D575F474154455F` (`MW_GATE_`) |
| Base-seed advance | `0x4D575F5245534544` (`MW_RESED`) |

Map integer values are frozen as Mandelbrot `0`, Julia `1`, and Burning Ship
`2`. Channel indices are zero-based in `[0, 15]`. Each orbit-hash call uses
fields as follows:

| Purpose | Channel | Map | Step | Micro-iteration |
|---|---|---|---|---|
| Viewport X/Y | Actual | `0` | `0` | `0` |
| Julia initial X/Y | Actual | `1` | `0` | `0` |
| Mutation X/Y | Actual | Actual | Current | Current |
| Re-entry X/Y | Actual | Actual | Current | Escaping index |
| Gate trial | Actual | Actual | Current | `0` |

Viewport coordinates deliberately omit MAP so CENTER/ZOOM navigation does not
jump merely because MAP changes. The Julia initial state uses its frozen map
value to remain a separate hash space.

The high bits of a hash word are consumed before the low bits. Conversions from
hash words use these exact bit extractions:

```text
signedUnitQ30(h) = int64(h >> 33) - 2^30  // [-1, +1) in Q1.30
trialQ16(h)      = uint16(h >> 48)         // [0, 65535]
reentryQ28(h)    = int64(h >> 38) - 2^25  // [-0.125, +0.125) in Q4.28
```

Viewport seed coordinates convert `signedUnitQ30()` to Q4.28 using the frozen
signed division rule with divisor 4. Mutation multiplies Q4.28 depth by
`signedUnitQ30()` in signed 64-bit and divides by `2^30` with that same rule.
The base-seed advance uses `mix64()` directly, applies the equality fallback in
Section 2.1, and does not call `orbitHash()`.

The conversions avoid out-of-range unsigned-to-signed casts and are named
helpers with golden tests. No standard-library random engine or
implementation-defined hash is part of the algorithm.

## 5. Escape and Feature Extraction

ESCAPE fires once for an accepted step if any micro-iteration escapes. After
escape, remaining micro-iterations for that accepted step are skipped.

Version one ships one escape policy: deterministic re-entry. Two separately
tagged hash values are mapped with `reentryQ28()` to components in
`[-0.125, +0.125)`. This square is
strictly contained within the radius-0.25 disk around the origin. The escaped
candidate is never used as input to re-entry, so identical seed, channel, map,
step, and micro-iteration always produce identical bounded re-entry state.
Fold and hold-last policies are deferred until a later algorithm version.

Radius uses the classic unsigned restoring integer-square-root algorithm over
the Q8.56 squared-radius value. It processes exactly 32 two-bit groups from
most-significant to least-significant and returns `floor(sqrt(value))` in Q4.28.
No floating conversion participates in event decisions.

Phase uses a checked-in 4,097-entry first-octant arctangent table. Given
non-zero `absX` and `absY`, the smaller magnitude divided by the larger is
quantized by floor to an index in `[0, 4096]`. Each table entry is the Q2.30
encoding of `atan(index / 4096) / pi`, generated offline with round-to-nearest,
halfway away from zero. Let `a` be the non-negative table result and define
`base = a` when `absX >= absY`, otherwise `base = 0.5 - a`, all in Q1.30.
Quadrants are reconstructed exactly as:

```text
x >= 0, y >= 0: phase = base
x <  0, y >= 0: phase = 1 - base
x <  0, y <  0: phase = -1 + base
x >= 0, y <  0: phase = -base
```

Thus the negative real axis with `y == 0` maps to `+1`; negative `y` approaches
it from `-1`. The zero vector alone has phase zero. The committed integer table
and its checksum are part of algorithm version one; runtime interpolation is
not used.

`atan2()` and `sqrt()` are not called in the audio path.

Feature extraction may be skipped when its output is disconnected and it is
not needed for GATE or the selected display channel. Event-affecting features
must never depend on floating point.

## 6. Exact Output Mapping

Unslewed targets use these mappings:

```text
xNorm      = clamp(x / 2, -1, +1)
yNorm      = clamp(y / 2, -1, +1)
radiusNorm = clamp(radius / 2, 0, 1)
phaseNorm  = clamp(angle / pi, -1, +1)

X_OUTPUT      = 5 V  * xNorm
Y_OUTPUT      = 5 V  * yNorm
RADIUS_OUTPUT = 10 V * radiusNorm
PHASE_OUTPUT  = 5 V  * phaseNorm
```

Equivalently, X and Y use 2.5 V per mathematical coordinate unit before
clamping, and Radius uses 5 V per unit.

When the serialized Phase polarity option is Unipolar, only the final voltage
mapping changes:

```text
PHASE_OUTPUT = 5 V * (phaseNorm + 1)  // 0–10 V
```

Phase smoothing operates in normalized circular phase before either voltage
mapping, so changing polarity cannot alter the chosen shortest path.

Linear Radius is the only version-one response. A future Expanded response may
be added as a serialized option while retaining Linear as the compatibility
default.

GATE, ESCAPE, and STEP are exactly 0 V or 10 V. Pulse duration uses an integer
sample counter calculated from the selected duration and sample rate with a
documented rounding rule.

### 6.1 Gate probability

DENSITY and normalized Radius are quantized to closed unsigned Q0.16 values in
`[0, 65536]`, stored in `uint32_t`, before entering the event decision. The
endpoint 65536 is retained so 100% is exactly representable.

```text
radiusQ16 = roundHalfAwayFromZero(clamp(radiusQ28 / 2, 0, 1) * 65536)
edgeQ16 = 16384 + roundHalfAwayFromZero(3 * radiusQ16 / 4)
probabilityQ16 = roundHalfAwayFromZero(densityQ16 * edgeQ16 / 65536)
trialQ16 = hash >> 48
```

Intermediates are unsigned 64-bit. The event fires when the 16-bit trial in
`[0, 65535]` is strictly less than `probabilityQ16`. This makes zero probability
never fire and 65536 probability always fire. The multiplication, rounding,
and comparison are integer-only.

DENSITY is knob-only in version one. There is no DENSITY input or DENSITY
attenuator.

## 7. Clocking and Event Precedence

| Condition | Required behavior |
|---|---|
| CLOCK unpatched and free-run enabled | Internal accumulator requests steps at effective RATE |
| CLOCK patched | Internal accumulator pauses; rising edges request steps |
| Polyphonic CLOCK | Each channel advances independently |
| Monophonic CLOCK with higher CV polyphony | Clock channel zero broadcasts |
| RESEED and any reset/step on the same sample | RESEED wins, applies the current MAP globally, and suppresses steps |
| MAP change and RESET on the same sample | MAP reset wins globally and suppresses steps |
| RESET and step on the same sample | RESET wins and suppresses the step |
| MAP change and step on the same sample | MAP reset wins and suppresses the step |
| Multiple transitions in one sample | At most one accepted step per channel |
| Cable inserted while high | Synchronize detector; do not create a phantom step |
| Cable removed | Suppress a step on the removal sample, then resume retained internal phase |
| Sample-rate change | Preserve normalized internal phase and rebuild timing coefficients |
| STEP | One pulse per accepted internal or external step |
| ESCAPE | One pulse when any micro-iteration in the accepted step escapes |

The precedence order is global Rack reset, RESEED, MAP change, RESET input,
then clock-requested step. Only the highest applicable state-changing action is
performed for a channel on a sample. A simultaneous RESEED and MAP change uses
the new seed with the newly selected map in one reconstruction. State-changing
actions clear affected event pulses and do not emit STEP, ESCAPE, or GATE.

Edge thresholds are explicitly 0.1 V low and 2.0 V high.

Internal RATE spans 0.05–200 Hz, defaults to 4 Hz, and accepts one volt per
octave CV. Effective pitch is quantized and uses the checked-in RATE table
defined in Section 9.2, not per-sample `pow()`.

Each channel uses a `uint64_t` normalized phase accumulator. When its quantized
RATE index or sample rate changes, the adapter computes one cached increment by
rounding `(rateMicroHz * 1e-6 / sampleRate) * 2^64` to the nearest unsigned
integer. Implementation uses a scaled decomposition or `ldexp()` so the
mathematical `2^64` is never converted to an overflowing integer. Per sample,
unsigned addition is the only phase math; wraparound requests one step. RATE is
always below supported audio sample rates, so one channel cannot request more
than one internal step per sample. Construction and patch load begin at phase
zero; sample-rate changes retain the accumulator bits and rebuild only the
increment.

The internal phase accumulator pauses, without advancing or clearing, while an
external CLOCK cable is connected. After cable removal, the removal sample
cannot step; accumulation resumes on the following sample from the retained
phase. A naturally imminent rollover may therefore occur one or more samples
later, but cable removal itself never creates an edge.

`freeRunWhenUnclocked` defaults to true.
`restartInternalPhaseOnReset` defaults to true. When enabled, RESET sets the
internal phase of every affected channel to zero; when disabled, RESET preserves
it. MAP changes preserve internal phase because MAP reset concerns orbit state
rather than clock origin.

The default pulse width is 1 ms. Pulse length is:

```text
samples = max(1, floor(sampleRate * pulseWidthMs / 1000 + 0.5))
```

An event sets its counter to `samples` before output generation, so the event
sample is the first high sample. The counter decrements after output generation.
A retrigger while high restarts the full duration; it does not extend from the
old endpoint or ignore the new event.

## 8. Smoothing

X, Y, Radius, and Phase are target-and-slew outputs. At zero smoothing, the
target is assigned immediately. Otherwise a one-pole response is used:

```text
v += coefficient * (target - v)
```

SMOOTH spans 0–2000 ms and defaults to 80 ms. Its coefficient is cached per
channel and recomputed only when a quantized effective SMOOTH value or sample
rate changes. Recalculation uses the Section 9.2 formula; a standard exponential
is permitted at that cache-update boundary but must never run unconditionally
per sample.

Phase uses shortest-path circular interpolation so crossing +5 V/−5 V does not
slew around the long side of the circle.

Smoothing affects only output voltage. It never affects recurrence, Radius used
by GATE, event timing, history, or escape decisions.

RESET, RESEED, MAP change, and Rack global reset assign both the unslewed target
and current smoothed value of every affected channel to its reconstructed
initial feature values. Old voltage motion therefore cannot bleed across a
sequence restart. Changing Phase polarity remaps the existing normalized phase
state and does not reset or reinterpret the orbit.

## 9. Controls and Ports

### 9.1 Parameters

| Parameter | Range | Default | CV behavior |
|---|---:|---:|---|
| MAP | Mandelbrot, Julia, Burning Ship | Mandelbrot | None |
| CENTER X | −2.25 to +1.00 | −0.75 | X through bipolar X AMOUNT |
| CENTER Y | −1.50 to +1.50 | 0.00 | Y through bipolar Y AMOUNT |
| ZOOM | 0–12 octaves | 2 | ZOOM CV, 1 V/oct at full amount |
| ITERATIONS | Integer 1–32 | 4 | None in v1 |
| MUTATION | 0–100% viewport-relative | 24% | MUTATE through bipolar amount |
| SMOOTH | 0–2000 ms, cubic perceptual taper | 80 ms | 0–10 V additive |
| RATE | 0.05–200 Hz, stored as octave pitch around 4 Hz | 4 Hz | 1 V/oct |
| DENSITY | 0–100% | 50% | None in v1 |
| X AMOUNT | −1 to +1 | 0 | Scales X input |
| Y AMOUNT | −1 to +1 | 0 | Scales Y input |
| ZOOM AMOUNT | −1 to +1 | 0 | Scales ZOOM input |
| MUTATE AMOUNT | −1 to +1 | 0 | Scales MUTATE input |
| RESEED | Momentary | Off | Generates a new base seed and resets |
| SEED LOCK | Latching | On | Controls Rack-randomize seed behavior |

RESEED and SEED LOCK are ordinary parameters so automation, MIDI mapping,
history, and patch persistence follow Rack conventions. Base seed remains
non-parameter state.

### 9.2 Effective-control adaptation

The Rack adapter replaces every non-finite input voltage with 0 V before any
other operation. Parameters are clamped to their configured domains. Inputs
with one channel broadcast to the active channels; a polyphonic input uses its
corresponding channel and supplies 0 V above its channel count unless a
specific broadcast rule says otherwise.

Orbit controls are sampled only when that channel accepts a step. Their exact
normalized forms are:

```text
xNorm = clamp(X_V / 5, -1, +1) * X_AMOUNT
yNorm = clamp(Y_V / 5, -1, +1) * Y_AMOUNT
zoomOct = clamp(ZOOM + clamp(ZOOM_V, -5, +5) * ZOOM_AMOUNT, 0, 12)
mutation = clamp(MUTATION
    + clamp(MUTATE_V / 5, -1, +1) * MUTATE_AMOUNT, 0, 1)
densityQ16 = roundHalfAwayFromZero(clamp(DENSITY, 0, 1) * 65536)
iterations = clamp(roundHalfAwayFromZero(ITERATIONS), 1, 32)
map = clamp(roundHalfAwayFromZero(MAP), 0, 2)
```

`zoomOct` is then quantized to its 1/256-octave table index. CENTER, normalized
viewport coordinates, mutation depth, and CV terms are converted to their
documented fixed-point domains with round-to-nearest-half-away-from-zero before
the pure engine step. The Q4.28 evaluation order is CENTER plus ZOOM scale times
the corresponding viewport coefficient times viewport coordinate, with each
multiplication using the named rounded fixed-point helper. Fast-math compiler
flags may not alter this adapter.

SMOOTH uses a stored normalized parameter `s` in `[0, 1]`; its default is
`0.3419951893`. After quantization it maps to approximately 79.9 ms and is
displayed as 80 ms. Effective smoothing is:

```text
sEffective = clamp(s + clamp(SMOOTH_V / 10, 0, 1), 0, 1)
sQuantized = roundHalfAwayFromZero(sEffective * 1024) / 1024
smoothMs = 2000 * sQuantized^3
```

Zero selects immediate assignment. Otherwise the cached one-pole coefficient
is `1 - exp(-1 / (0.001 * smoothMs * sampleRate))`. The exponential is allowed
only when the quantized value or sample rate changes, never unconditionally per
sample.

RATE stores octave pitch relative to 4 Hz. Effective pitch is the parameter
plus finite RATE input voltage, clamped to the range needed for 0.05–200 Hz and
rounded to the nearest 1/256 octave. A checked-in table covering indices
`[-1619, 1445]` supplies integer microhertz:

```text
rateMicroHz[i] = roundHalfAwayFromZero(
    clamp(4 * 2^(i / 256), 0.05, 200) * 1,000,000)
```

The table is generated with the other ZOOM and phase tables and is part of the v1
reference artifact. The audio path performs lookup and cached phase-increment
updates, not `pow()` or `exp2()`.

### 9.3 Inputs

The port budget is exactly eight inputs.

| Input | Range and behavior |
|---|---|
| CLOCK | Gate/trigger; unpatched selects internal clock |
| RESET | Gate/trigger; mono broadcasts, poly is channel-specific |
| X | Nominal ±5 V through X AMOUNT |
| Y | Nominal ±5 V through Y AMOUNT |
| ZOOM | Nominal ±5 V, 1 V/oct at full amount |
| MUTATE | Nominal ±5 V through MUTATE AMOUNT |
| SMOOTH | 0–10 V additive over the full smoothing range |
| RATE | 1 V/oct for the internal clock |

Active polyphony is the maximum connected channel count across all eight
inputs, clamped to `[1, 16]`; an entirely unpatched module is monophonic. A
monophonic input broadcasts where the corresponding polyphonic behavior
requires it. A selected display channel may show a retained inactive slot and
is not required to follow active output polyphony.

### 9.4 Outputs

The port budget is exactly seven outputs. Every output uses the active channel
count.

| Output | Range |
|---|---:|
| X | −5 to +5 V |
| Y | −5 to +5 V |
| RADIUS | 0 to 10 V |
| PHASE | −5 to +5 V |
| GATE | 0 or 10 V pulse |
| ESCAPE | 0 or 10 V pulse |
| STEP | 0 or 10 V pulse |

### 9.5 Lights

Version one has one logical Light ID: the gold SEED LOCK indicator. It is fully
lit when locked and dark when unlocked, following the parameter without an
audio envelope. STEP, GATE, ESCAPE, map, and compatibility state are already
communicated by the display, selector, or output behavior and do not receive
redundant dedicated panel lights in v1.

## 10. Persistence and Compatibility

Rack automatically persists parameters. JSON stores only non-parameter state:

| Field | Default | Meaning |
|---|---:|---|
| `schemaVersion` | `1` | Storage representation |
| `algorithmVersion` | `1` | Deterministic musical behavior |
| `baseSeedHi`, `baseSeedLo` | New random words | Exact unsigned 64-bit seed as two 32-bit words |
| `freeRunWhenUnclocked` | `true` | Internal clock policy |
| `restartInternalPhaseOnReset` | `true` | Reset policy |
| `phasePolarity` | `Bipolar` | Bipolar −5–5 V or Unipolar 0–10 V |
| `pulseWidthMs` | `1` | 0.1, 1, 5, or 10 ms |
| `displayQuality` | `Normal` | Low, Normal, High, or Frozen |
| `selectedDisplayChannel` | `0` | Zero-based channel shown in the display |

Malformed, absent, and future-version fields fall back safely without
overwriting valid Rack parameter values. All loaded integers and enums are
clamped before reaching the engine.

Orbit coordinates, step indices, pulse counters, smoothing memory, visual
history, and internal clock phase are intentionally not serialized. Loading or
duplicating reconstructs every channel at step index zero from the saved seed,
map, and parameters, with internal phase zero. “Preserves its sequence” means
the same future sequence can be replayed; it does not mean transport resumes
mid-orbit at the exact saved sample.

`schemaVersion` describes the JSON representation. `algorithmVersion`
describes recurrence, hashing, quantization, event, and reset behavior. They are
independent compatibility axes. A version-one patch missing
`algorithmVersion` is interpreted as algorithm version 1.

Future builds must retain dispatch for every published algorithm version used
by saved patches; loading an old patch must not silently substitute the newest
algorithm. An unknown future algorithm version falls back safely to version 1,
sets a non-persisted compatibility-warning flag for the UI, and preserves the
unknown loaded number for a subsequent save rather than pretending migration
succeeded. Explicit migration, if ever offered, must be a user action with Rack
history support.

Mandelwake is unreleased, so IDs may change during prototyping. Once the module
is published, Param/Input/Output/Light IDs become append-only and the default
algorithm version becomes a compatibility contract.

Golden trace changes require an explicit schema or algorithm-version decision;
goldens must never be casually regenerated to hide a behavioral change.

## 11. Engine and Thread Architecture

Recommended file boundaries:

| File | Responsibility |
|---|---|
| `MandelwakeFixedPoint.hpp` | Defined conversions, saturation, multiplication, integer sqrt/phase helpers |
| `MandelwakeTables.hpp` | Generated ZOOM, RATE, and phase integer reference tables plus checksums |
| `MandelwakeEngine.hpp/.cpp` | Rack-independent deterministic maps, hashes, escape, events |
| `MandelwakeVisualData.hpp` | POD history and snapshot exchange |
| `Mandelwake.hpp/.cpp` | Rack params, voltage adaptation, clocking, smoothing, JSON |
| `MandelwakeWidget.hpp/.cpp` | Anchored panel, controls, context menu, drawing |
| `tools/generate_mandelwake_tables.py` | Offline deterministic table generator and checksum verifier |
| `tests/mandelwake_engine_spec.cpp` | Pure v1 arithmetic, hash, map, lifecycle, and trace contract |

The pure engine accepts integer-domain step inputs. Event-affecting values must
not enter it as floating point. The version-one step frame contains at least
these normative fields:

```cpp
struct MandelwakeStepInputs {
    int32_t cXQ28 = 0;
    int32_t cYQ28 = 0;
    int32_t mutationDepthQ28 = 0;
    uint32_t densityQ16 = 32768;
    uint8_t iterations = 4;
};

struct MandelwakeStepOutputs {
    int32_t xQ28 = 0;
    int32_t yQ28 = 0;
    uint32_t radiusQ28 = 0;
    int32_t phaseQ30 = 0;
    bool gate = false;
    bool escaped = false;
};
```

The Rack module sanitizes voltage, quantizes sampled controls, and converts
targets to voltage. The engine owns recurrence and event determinism.

All sixteen channel-state slots always exist. An inactive channel retains its
orbit coordinates, orbit step counter, internal clock phase, smoothing state,
and history but does not advance. On deactivation, event pulse counters are
cleared. On activation or reactivation, clock/reset edge detectors synchronize
to current input levels and the activation sample cannot produce a step or
reset edge. The retained orbit resumes on later accepted steps; changing
between mono and poly cabling does not reconstruct a channel from its seed.

Global Rack reset, monophonic RESET broadcast, MAP change, and RESEED apply to
all sixteen slots, including inactive ones. Polyphonic RESET applies only to
the addressed channel slots as defined in Section 2. RESEED reconstructs all
sixteen channel states immediately. These rules make a 16→1→16 repatch
performatively continuous without permitting stale high pulses or phantom
cable edges.

### 11.1 Visual history and publication

Maintain a fixed 256-point history ring for every channel. This costs roughly
32 KiB for two 32-bit coordinates across sixteen channels and avoids a data race
or history discontinuity when the selected display channel changes.

The UI writes the selected channel through an atomic integer. At 15–30 Hz the
engine publishes only that channel into a fixed POD snapshot using a proven
lock-free multi-slot exchange. The audio thread never allocates, waits, mutates
reference counts, or reads UI-owned memory without atomic synchronization.

The selected-channel snapshot includes:

- the ordered Q4.28 history points and current ring position;
- the current point and most recent pre-escape and re-entry points;
- monotonically wrapping `uint32_t` escape, reset, reseed, and map-change
  serials;
- current map, channel, mutation amount, seed-lock state, and compatibility
  warning state.

Serials allow the UI to notice events that occur between display refreshes
without lengthening DSP pulses or polling DSP-owned booleans. If several events
occur between frames, the UI renders one bounded reaction for the latest event
and may scale its intensity by the saturated serial delta. It never queues an
unbounded number of visual events.

The display may interpolate visual points, but visual interpolation is never fed
back into DSP.

## 12. UI and Visual Requirements

The panel should read as a navigable abyssal chart rather than a generic XY
plot. The display is the prime spatial feature and receives panel area before
decorative labeling or redundant status furniture. Its target clear aperture
is at least 68 mm × 34 mm; any reduction requires explicit review during the
measured panel gate. Proposed vertical hierarchy:

1. title, sigil, and MAP;
2. large orbit display;
3. CENTER X/Y, ZOOM, and ITERATIONS;
4. MUTATION, SMOOTH, RATE, and DENSITY;
5. four bipolar amount controls;
6. RESEED and illuminated SEED LOCK;
7. eight inputs left and seven outputs right.

Use the repository’s split panel renderer, label layer, component library, and
SVG component anchors. Required anchors must be present in both the combined
source SVG and the runtime `.panel.svg`, and the generated panel anchor atlas
must be refreshed after layout changes.

### 12.1 Signature visualization: the Abyssal Wake

The Abyssal Wake is a required version-one feature and Mandelwake’s primary
visual identity. It has four inseparable layers:

1. **Bathymetric field.** Dim fractal boundary contours resemble an abyssal
   navigation chart beneath smoked fractal glass. They establish the current
   map, CENTER, and ZOOM without becoming a live heatmap.
2. **Bioluminescent wake.** The selected orbit is drawn as a tapered trail whose
   oldest points recede into deep violet and whose newest points rise through
   electric cyan toward a pale core. The primary centerline uses the exact
   published history coordinates.
3. **Trident head.** The current point is a compact gold trident/crown glyph,
   oriented along the most recent non-degenerate trail direction. It remains
   recognizable at Rack 50–200% zoom and falls back to upright when direction
   is undefined.
4. **Wake rupture.** ESCAPE visibly breaks the trail at the last bounded point,
   emits a sharp expanding ring, and ignites the re-entry point. The break and
   ring make escape legible without relying on color.

The wake uses at most sixteen age bands. Each band may use a bounded outer haze,
colored body, and pale core stroke, keeping draw-call count fixed rather than
proportional to every point times an arbitrary effect count. A subtle secondary
filament may separate from the primary path as MUTATION rises, but it is derived
from adjacent trail tangents, never represented as additional DSP state, and
never obscures the exact centerline. There are no free particles.

The palette follows the established Leviathan hierarchy:

- near-black blue/teal glass and desaturated purple bathymetry for depth;
- violet-to-cyan wake energy;
- pale cyan/white for the newest wake core;
- restrained Leviathan gold for the trident, reseed ignition, and critical
  accents;
- text and status colors that remain readable without the glow layers.

### 12.2 Event choreography

Visual reactions are short, bounded UI envelopes triggered by snapshot serials:

| Event | Required reaction | Nominal settling time |
|---|---|---:|
| STEP | Advance and relight the wake head; no independent flash | Next display refresh |
| ESCAPE | Trail discontinuity, breach ring, re-entry ignition | 220 ms |
| RESET | Existing wake contracts toward the origin and clears | 180 ms |
| RESEED | RESET contraction followed by a gold-to-cyan constellation ignition | 300 ms |
| MAP change | Contours crossfade while the wake resets | 250 ms |

These envelopes use UI time only and never feed the engine. They dirty only the
display framebuffer while active and settle to a static frame. Multiple events
between frames collapse into the bounded latest-event response. Mandelwake does
not use a full-panel flash, unbounded afterimages, or cable-obscuring overflow.

Frozen quality retains the last fully composed abyssal chart, including its
trident and contours, then stops all animation and framebuffer invalidation.
RESET, RESEED, and MAP still affect DSP while frozen; unfreezing publishes the
current state without replaying accumulated visual events.

### 12.3 Contours, status, and preview

Contour generation is visual-only, preallocated, incremental, and cached. Low
quality uses only a static abyssal vignette. Normal quality evaluates at most a
48 × 32 coarse grid; High evaluates at most an 80 × 48 grid. Work is divided
across UI frames, the previous cache remains visible until replacement is
complete, and contour generation never runs on the audio thread. Cache keys use
the selected map and quantized visual CENTER/ZOOM state. Approximate visual
contours do not participate in algorithm-version goldens.

The display communicates selected channel, map, seed-lock, Frozen state, and
compatibility warnings in a restrained chart-legend treatment. The orbit remains
the dominant content; status text must not become a dashboard over the wake.

The module-browser preview uses a fixed canonical seed and checked-in static
hero state showing all four signature layers. It must be null-safe and should
read as Mandelwake even when Rack does not advance module DSP during capture.

Before IDs are frozen, a measured 18 HP panel artifact must demonstrate all
fifteen ports, every version-one control, the display, status indicators, and
labels at their actual dimensions. Controls and cables must not overlap,
adjacent jack clearance must be usable, labels must remain readable at Rack
100% zoom, and the display must retain enough area for a 256-point trace. Every
planned version-one control and port must have a final anchor. Param, Input,
Output, and Light enum order is frozen only after it matches this accepted
physical layout.

| Quality | Points | Background | Refresh ceiling |
|---|---:|---|---:|
| Low | 64 | Static vignette | 15 Hz |
| Normal | 256 | Cached contour, grid up to 48 × 32 | 30 Hz |
| High | 512 visual samples interpolated from 256 history points | Cached contour, grid up to 80 × 48 | 30 Hz |
| Frozen | Last cached frame | Unchanged | 0 Hz after settling |

Use the standard Leviathan split-panel surface, fractal-glass overlay, Perfect
Wave branding, cyan/purple Magitek jack language, gold action treatment, and
shared preview-frame enhancement. These establish family resemblance; the
Abyssal Wake establishes Mandelwake’s individual silhouette.

No image, SVG, font, string, widget, or framebuffer is created repeatedly in
`draw()`. NanoVG image handles follow `NvgGraphicsLifecycle.hpp`; any future GL
resources follow `GlLifecycleUtils.hpp`.

Display coordinate editing is explicitly deferred from version one. The display
does not consume pointer gestures beyond ordinary context-menu interaction.
CENTER X/Y knobs and their CV inputs are the only viewport-position editors.
Any later direct-manipulation mode must update parameters through Rack history
gestures and requires an intentional public-interface review; it may never
write orbit state directly.

## 13. Context Menu

| Group | Options |
|---|---|
| Clock | Free-run unpatched; restart internal phase on RESET |
| Output | Phase polarity; pulse width |
| Seed | Copy seed; paste seed; derive a new seed |
| Display | Low, Normal, High, Frozen; selected channel |
| Diagnostics | Debug-gated DSP and snapshot counters |

Diagnostics and developer-only controls are gated by
`isDragonKingDebugEnabled()`.

Copy Seed writes `MW1-` followed by exactly sixteen uppercase hexadecimal
digits. Paste Seed accepts that format case-insensitively with surrounding
whitespace, changes nothing on parse failure, and performs a global reset on
success. Derive New Seed applies the Section 2.1 base-seed advance and global
reset, exactly like one RESEED edge. Seed-changing context actions create Rack
history entries. Display-channel labels are one-based for people while stored
indices remain zero-based.

Linear Radius is the only v1 response. If an Expanded response is prototyped,
it must be explicitly serialized and Linear must remain the default.

## 14. Performance Contract

The expensive recurrence runs only on accepted orbit steps. Per-sample work is
limited to edge detection, internal clock phase, pulse counters, cached
smoothing, voltage output, and lightweight routing.

Required properties:

- zero audio-thread allocations after construction;
- zero audio-thread mutex acquisitions;
- no per-sample `sin`, `cos`, `atan2`, `sqrt`, `pow`, or `exp`;
- no file I/O, logging, wall-clock dependence, or UI ownership in `process()`;
- fixed arrays for all sixteen channels;
- bounded iterations and bounded snapshot copies;
- disconnected feature extraction skipped when it cannot affect GATE or UI;
- display quality and visibility produce bit-identical DSP;
- at most sixteen wake age bands and a fixed number of stroke passes;
- contour grids never exceed the selected quality budget and rebuild
  incrementally outside the audio thread;
- visual event handling coalesces serial deltas and never grows a queue;
- static and Frozen displays stop invalidating their framebuffers after event
  envelopes and contour work settle.

Performance numbers in the research document are targets to measure, not an
architecture promise. Release thresholds should be recorded only after native
profiling on the supported toolchains.

## 15. Test Contract

### 15.1 Pure engine tests

- fixed-point conversion and rounding at positive and negative boundaries;
- generated ZOOM, RATE, and phase table checksums and endpoint values;
- recurrence product limits and doubled-XY limit;
- post-recurrence candidate clamp before escape squaring;
- exact `1 << 58` escape constant;
- Burning Ship handling of minimum signed values;
- integer radius and phase reference vectors, including zero and all axes;
- stable, escaping, boundary-adjacent, Julia, and Burning Ship goldens;
- exact hash construction, constants, field order, and every domain tag;
- restoring integer-square-root floor behavior and phase-table checksum;
- deterministic mutation and domain-separated hashes;
- exact orbit-hash call fields for viewport, Julia, mutation, re-entry, and
  GATE;
- first accepted step uses index zero and increments only after feature/event
  extraction;
- base-seed advance and its equality fallback;
- sixteen-channel seed separation;
- repeated RESET reconstruction;
- counter-wrap behavior;
- maximum iterations and randomized property tests;
- no period of 64 steps or shorter across 4,096 steps for representative
  factory/default seeds;
- intentional mutation-zero periodic cases remain reproducible.

### 15.2 Rack runtime tests

- RESET/MAP/clock same-sample precedence;
- external clock insertion and removal without phantom edges;
- exact integer pulse lengths at supported sample rates;
- pulse retrigger restarts full duration;
- internal RATE accumulation error below one sample over ten minutes;
- mono broadcast and per-channel reset;
- active channel count changes;
- 16→1→16 channel retention, global inactive-slot reset, targeted polyphonic
  RESET, MAP reset, and pulse clearing;
- all output bounds and non-finite input sanitization;
- parameter display strings and snapping;
- effective X/Y/ZOOM/MUTATE/DENSITY/SMOOTH/RATE adapter boundaries and
  non-finite sanitization;
- malformed JSON, schema/algorithm-version dispatch, and exact seed round trip;
- SEED LOCK randomize behavior;
- offline and real-time event equivalence;
- display enabled/frozen/hidden DSP equivalence;
- no audio-thread allocation or lock acquisition.

### 15.3 UI and lifecycle tests

- required anchors resolve and remain inside the panel;
- combined source SVG and `.panel.svg` carry matching anchor geometry;
- module-browser preview is null-safe and shows the canonical Abyssal Wake hero
  state;
- the trident head remains recognizable from Rack 50–200% and has a stable
  upright fallback for degenerate motion;
- escape is identifiable from trail discontinuity and ring geometry without
  color;
- escape/reset/reseed/map serial jumps produce one bounded latest-event
  response rather than queued animations;
- wake age bands, stroke passes, contour grids, and incremental work remain
  within their specified bounds;
- Frozen mode settles, does not replay accumulated events, and resumes at the
  current state;
- Rack zoom matrix from 25–400%;
- no clipped labels at longest values;
- framebuffers settle when static;
- graphics-context reopen rebuilds resources lazily;
- selected-channel changes are race-free;
- multi-instance rendering remains bounded;
- module deletion and Rack shutdown leave no graphics or worker lifetime fault.

## 16. Repository and Build Integration

New module integration must update together:

- model declaration in `plugin.hpp`;
- `addModel()` registration in `plugin.cpp`;
- model definition in the Mandelwake module source;
- stable manifest slug and metadata in `plugin.json`;
- runtime and combined panel assets;
- generated panel anchor atlas;
- `build/tests/mandelwake_engine_spec` build/run recipes and
  `TEST_BINS_NON_RACK`/`test-fast` membership.

The repository already discovers ordinary `src/*.cpp` files through wildcards.
Do not add redundant source-list entries unless a specialized target needs them.

`tools/generate_mandelwake_tables.py` uses only the Python standard library and
writes integer literals; generated tables are never built at plugin runtime.
For checksum purposes, each table’s values are encoded in declared order as
four-byte little-endian integers (`int32_t` for ZOOM and Phase, `uint32_t` for
RATE) and hashed separately with SHA-256. The header records each lowercase hex
digest. A `--check` mode regenerates in memory and fails without rewriting when
the committed literals or digests differ. The committed literals, rather than
host transcendental-library results at runtime, are authoritative after lock.

Rack SDK 2.6.6 is the current architecture target. Revalidate the exact SDK and
minimum Rack version during release hardening rather than treating the research
document’s date as permanent.

In WSL/WSL-like environments:

- edit code and run pure/focused tests;
- expect `test-fast` to pass;
- treat `test-rack` as work in progress;
- do not diagnose final `plugin.so` linking as authoritative.

Final plugin linking, Rack interaction, and release validation occur in the
Windows/MSYS2 toolchain unless a real Linux environment has a matching Rack SDK
and runtime.

Do not stage or commit files; repository commits remain user-owned.

## 17. Version-One Lock Record

Accepted design decisions are checked below. Unchecked items are concrete
implementation artifacts that must exist before IDs or deterministic behavior
become release-final.

### Public interface

- [x] The version-one width is 18 HP.
- [x] The version-one control set is final, with display editing deferred.
- [x] Eight inputs and seven outputs are final; DENSITY remains knob-only.
- [x] SEED LOCK is the sole logical panel light; event communication remains in
  the display and outputs.

### Deterministic engine

- [x] Viewport ZOOM equations and 1/256-octave quantization are accepted.
- [x] Mutation is viewport-relative with a 24% default.
- [x] Q4.28 division rounds to nearest with halfway cases away from zero.
- [x] Candidate clamp, escape comparison, and narrowing order are accepted.
- [x] SplitMix64 constants, field order, and domain tags are frozen.
- [x] Hash call fields, zero-based first-step order, and base-seed advancement
  are frozen.
- [x] Restoring floor square root and the 4,097-entry phase lookup are frozen.
- [x] Exact X/Y/Radius/Phase voltage mappings are accepted.
- [x] Bipolar and Unipolar Phase mappings share normalized circular smoothing.
- [x] Integer-only Radius, Phase, and GATE feature extraction are accepted.
- [x] Linear Radius is the sole version-one mapping.
- [x] Deterministic bounded re-entry is the sole version-one escape policy.

### Timing and state

- [x] Internal RATE remains 0.05–200 Hz with a non-bandlimited warning.
- [x] External clock pauses internal phase; removal resumes without an
  immediate step.
- [x] Free-run-unpatched and restart-phase-on-RESET both default on.
- [x] Pulse width defaults to 1 ms, rounds to nearest with a one-sample
  minimum, and restarts its full duration on retrigger.
- [x] Persistent sixteen-slot channel lifecycle and activation-edge suppression
  are accepted.
- [x] Monophonic/global RESET scope, targeted polyphonic RESET scope, and reset
  smoothing behavior are accepted.
- [x] MAP changes reset every orbit without generating a new seed.
- [x] RESET, RESEED, SEED LOCK, and Rack-randomize semantics are accepted.
- [x] Output smoothing and shortest-path phase wrapping are accepted as
  voltage-only behavior.
- [x] Effective-control sanitization, quantization, SMOOTH taper, and RATE-table
  adaptation are accepted.

### Compatibility

- [x] `schemaVersion` and persisted `algorithmVersion` both begin at 1.
- [x] Old algorithms remain dispatchable; unknown future versions preserve
  their identifier and expose a compatibility warning.

### UI publication

- [x] Per-channel 256-point histories and selected-channel snapshot publication
  are accepted.
- [x] The Abyssal Wake, trident head, bathymetric field, and wake rupture are
  required version-one identity features.
- [x] RESET, RESEED, MAP, and ESCAPE use bounded serial-driven event
  choreography with no particle queue or full-panel flash.
- [x] Low, Normal, High, and Frozen quality behavior and cadence are accepted.
- [x] Display coordinate editing remains deferred from version one.

### Remaining implementation gates

- [ ] Generate and commit the version-one ZOOM, RATE, and phase lookup tables with
  documented checksums.
- [ ] Produce a measured 18 HP panel proving control, port, cable, label, and
  display feasibility, including the target 68 mm × 34 mm clear aperture.
- [ ] Produce and approve the canonical module-browser hero state and confirm
  that Mandelwake remains visually identifiable at browser-preview scale.
- [ ] Place every version-one control and port, then freeze Param/Input/Output/
  Light ID order against the accepted panel.
- [ ] Generate golden vectors only after the deterministic implementation
  matches every accepted decision above.

## 18. MVP Prototype Contract

The MVP is a working vertical slice used to audition Mandelwake’s musical core
and iconic visualization inside Rack. It is not a throwaway floating-point
sketch: deterministic primitives, seed behavior, ports, and recurrence enter
through their version-one interfaces from the first prototype. Panel polish,
render refinements, factory presets, and release compatibility hardening may
follow after the vertical slice proves the instrument.

### 18.1 Required MVP artifacts

The prototype is complete only when it contains:

- generated ZOOM, RATE, and phase tables with repeatable checksums;
- the Rack-independent fixed-point engine with all three maps, mutation,
  escape/re-entry, Radius, Phase, and integer GATE decisions;
- a pure `mandelwake_engine_spec` target included in `test-fast`;
- the Rack module wrapper with all accepted parameters, eight inputs, seven
  outputs, 1–16-channel routing, clock/reset precedence, smoothing, pulses,
  persistence, and seed commands;
- plugin declaration, registration, and manifest entry under the permanent
  slug `Mandelwake`;
- provisional `res/mandelwake.svg`, `res/mandelwake.panel.svg`, and
  `res/mandelwake.labels.svg` assets with component anchors;
- a visible Normal-quality Abyssal Wake with exact centerline, age color,
  trident head, cached coarse bathymetry, wake rupture, and null-safe browser
  hero state;
- Frozen behavior sufficient to verify framebuffer settling and DSP/UI
  independence;
- context-menu access to clock policy, Phase polarity, pulse width, selected
  channel, display quality, and seed operations.

### 18.2 Permitted MVP simplifications

The following may remain visibly provisional without changing architecture:

- panel illustration, typography, decorative glass tuning, and final spacing;
- Low and High quality may initially share Normal’s geometry budget in internal
  MVP builds, provided neither exceeds that budget; their specified distinct
  behavior is required before release;
- RESET, RESEED, and MAP choreography may use their specified timing with
  simplified easing, while ESCAPE rupture and the trident remain mandatory;
- contour evaluation may begin with the Normal 48 × 32 grid only;
- performance thresholds may be measurements rather than release gates;
- factory presets, manual prose, screenshots, and store assets may be absent.

The MVP may not replace fixed-point recurrence with floating point, omit
algorithm-version persistence, change the port budget, collapse RESET and
RESEED into one action, feed visual approximations back into DSP, allocate from
`process()`, or substitute a generic XY line for the Abyssal Wake.

### 18.3 Prototype compatibility boundary

The slug, JSON field names, and algorithm version are treated as v1 candidates
from the first MVP. Param/Input/Output/Light enum order remains provisional
until the measured panel gate is accepted. Patches made with an internal MVP
build are therefore test material and are not promised compatibility across
prototype revisions. Once ID order is frozen, all later changes follow the
append-only release rule.

The first prototype uses the exact top-to-bottom order listed in Sections 9.1,
9.3, 9.4, and 9.5 for Param, Input, Output, and Light enums respectively. This
gives implementation one unambiguous starting order without falsely declaring
release compatibility before the physical-layout review.

Any deterministic-engine change after provisional golden traces exist must
update the specification and explain the trace change. “Still a prototype” is
not permission to regenerate goldens without review.

### 18.4 MVP exit criteria

The MVP is ready for musical audition when all of the following are true:

- table generation is reproducible and the committed checksums verify;
- `build/tests/mandelwake_engine_spec` passes and aggregate `test-fast` remains
  green in the supported local environment;
- Mandelbrot, Julia, and Burning Ship produce bounded, visibly distinct,
  repeatable default traces from a fixed seed;
- RESET repeats the selected-channel trace, RESEED changes it, and save/load
  preserves the exact base seed and algorithm version;
- internal and external clocks drive correct STEP, ESCAPE, and GATE pulses at
  one and sixteen channels without phantom insertion/removal edges;
- all continuous and event outputs stay inside their documented voltage bounds;
- the browser and live module show a recognizable Abyssal Wake, gold trident,
  bathymetric field, and non-color-only escape rupture;
- hiding, freezing, or changing display quality leaves DSP traces bit-identical;
- steady-state `process()` performs no allocation or lock acquisition;
- the provisional 18 HP panel is usable enough for hands-on audition, with no
  overlapping controls or inaccessible jacks.

In WSL, focused engine tests and source-object compilation are authoritative for
the prototype pass. Final plugin linking and Rack audition remain authoritative
in the Windows/MSYS2 environment described in the repository instructions.

### 18.5 Recommended implementation order

1. Generate and verify the three reference tables.
2. Implement fixed-point helpers, hashing, channel reset, and pure engine tests.
3. Add all three maps, event extraction, and provisional deterministic traces.
4. Add the Rack adapter, clocks, polyphony, outputs, parameters, and JSON.
5. Build the provisional anchored 18 HP panel and register the module.
6. Add snapshot publication and the Normal/Frozen Abyssal Wake vertical slice.
7. Run focused tests and object compilation in WSL, then hand the complete
   plugin to Windows/MSYS2 for authoritative linking and Rack audition.
8. Refine musical defaults and presentation without changing locked behavior
   accidentally; promote traces to final goldens only after review.
