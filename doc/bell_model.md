# Doorstop Reference “Bell” Model

This document preserves the Doorstop reference model **at the start of the
low-mid body rework**, as implemented by `doorstop::ReferenceSpringEngine` at
that point. It is a code-oriented snapshot for reimplementing that sound in
another module. The constants below intentionally remain the pre-rework
values, even as the live engine evolves.

The name “bell model” is descriptive rather than literal: it is a nonlinear,
slow flex oscillator that controls the radiation of an inharmonic metallic
modal body. It is intended to produce the repeated metallic lobes of a spring
doorstop, not a freely ringing struck bell.

## 1. Signal topology

```text
strike velocity
  ├─ nonlinear strike shaping ────────┬─ macro flex impulse
  │                                   ├─ initial 8-mode excitation
  │                                   ├─ filtered-noise impact
  │                                   └─ mount oscillator impulse
  │
macro nonlinear flex x, v, a
  ├─ center-crossing detector ─────────── modest modal reinforcement
  ├─ flex energy ──────────────────────── modal pitch warping
  ├─ phase-normalized velocity ────────── radiation gate
  └─ velocity + acceleration ──────────── direct low body

metallic modal body × radiation gate ────┐
impact + mount + direct flex ────────────┼─ DC blocker ─ soft clip ─ output
                                         ┘
```

Per sample, before the DC blocker and final saturation:

```text
s = 0.45 * impact
  + 0.16 * mount
  + 0.50 * flexAudio
  + 1.90 * radiationGate * modalSum

y = 5 * tanhAudio(2.2 * dcBlock(s))
```

`tanhAudio()` is the project's audio-safe tanh approximation from
`MathHelpers.hpp`, not necessarily `std::tanh`. Reuse the project helper if
matching the existing character matters. The final output is bounded near
`±5 V`.

## 2. State and integration

The model is monophonic and sample-rate dependent. Store at least:

```cpp
// Macro flex
float x;                 // displacement
float v;                 // spring velocity
float a;                 // spring acceleration

// Eight independent modal oscillators
struct Mode { float position, velocity; } modes[8];

// Radiation / energy
float radiationEnvelope;
float smoothedFlexEnergy;
float lastDirection;     // +1 or -1

// Crossing state
enum ArmedSide { None, Positive, Negative } armedSide;
int crossingRefractorySamples;
int strikeRefractorySamples;

// Impact and mount
float impactEnvelope, impactBrightness, impactLowpass, impactLowReject;
float mountPosition, mountVelocity;

// housekeeping
float strikeLightEnvelope;
float dcPreviousInput, dcPreviousOutput;
float quietTime;
uint32_t noiseState;
bool sleeping;
```

All oscillators use semi-implicit Euler integration:

```cpp
velocity += acceleration * dt;
position += velocity * dt;
```

The engine owns its `dt = 1 / sampleRate` and ignores the `sampleTime`
argument passed to `process()`. A clean reimplementation should choose one
authoritative time step and use it consistently.

## 3. Strike semantics

Input velocity is clamped to `[-1, +1]`; sign is direction. Magnitude is
shaped before every excitation:

```cpp
u = clamp(abs(inputVelocity), 0, 1);
shaped = 0.20 * u + 0.80 * u * u;
direction = copysign(1, inputVelocity);
brightness = shaped * shaped;
```

Therefore weak strikes are deliberately much quieter/darker than full strikes.
A zero shaped magnitude does nothing.

On a valid strike:

```cpp
v = clamp(v + direction * shaped * 150, -maximumVelocity, maximumVelocity);
lastDirection = direction;
strikeRefractorySamples = roundDown(0.004 * sampleRate), at least 1;
```

The strike also injects each mode. Higher modes receive more of the squared
brightness component:

```cpp
available = clamp(1 - normalizedModalEnergy(), 0, 1);
for i = 0..7:
    t = i / 7
    modeBrightness = lerp(shaped, brightness, t)
    modes[i].velocity += direction * impactExcitation[i]
                         * modeBrightness * available
    modes[i].velocity = clamp(modes[i].velocity,
                              -modeVelocityLimit[i], +modeVelocityLimit[i])
```

The `available` term is important: it prevents rapid retriggers from adding
unbounded modal energy. Retriggers continue from all current macro, modal,
impact, and mount state; they do not reset the voice.

The remaining strike actions are:

```cpp
impactEnvelope = min(impactEnvelope + shaped, 2);
impactBrightness = max(impactBrightness, shaped);
mountVelocity = clamp(mountVelocity + direction * shaped * 42, -160, 160);
strikeLightEnvelope = max(strikeLightEnvelope, shaped);
sleeping = false;
quietTime = 0;
```

## 4. Macro flex oscillator

The visible and audible large-scale spring is a Duffing-like nonlinear
oscillator:

```text
restoring = ω0² x + k ω0² x³
a = -restoring - (2 ζ ω0) v
v += a dt
x += v dt
```

After integration, clamp `x` to `±maximumDisplacement`; if it is pressing
farther outward at a limit, set its velocity to zero. Clamp velocity to
`±maximumVelocity`.

At break-in `b`, define a smooth wear amount:

```cpp
w = b * b * (3 - 2 * b); // smoothstep(0, 1, b)
baseFrequencyHz    = 21.0 * lerp(1.00, 0.88, w);
dampingRatio       = 0.010 * lerp(1.00, 0.72, w);
nonlinearStiffness = 0.32  * lerp(1.00, 0.78, w);
maximumDisplacement= 2.00  * lerp(1.00, 1.15, w);
ω0 = 2π * baseFrequencyHz;
maximumVelocity = 4 * ω0;
```

Thus the fresh macro spring is approximately 21 Hz, while wear lowers its
resting pitch, damping, and stiffness and allows somewhat greater travel.
The cubic stiffness hardens the spring at high displacement.

Its normalized energy is:

```text
U(x) = 0.5 ω0²x² + 0.25 kω0²x⁴
Eflex = (0.5v² + U(x)) / U(maximumDisplacement)
```

`Eflex` may temporarily exceed one internally, but it is clamped to `[0, 1]`
for audio control and output telemetry.

## 5. Center crossings and the repeated “boing”

The macro oscillator does not re-create the entire metallic attack every half
cycle. Instead it continuously stores modal energy, exposes it through a
motion-dependent radiation gate, and applies a small modal reinforcement at
qualified center crossings.

Crossing qualification:

1. Arm only after `x` reaches either `±0.06 * maximumDisplacement`.
2. Detect a sign crossing from that armed side (`previousX > 0 && x <= 0`, or
   the mirrored negative condition).
3. Require `abs(v) / maximumVelocity > 0.025`.
4. Reject crossings during strike refractory or crossing refractory windows.
5. Both refractory windows are 4 ms, minimum one sample.
6. A geometric crossing always disarms the side; a qualified crossing sets
   `lastDirection` from the sign of current velocity and increments the
   diagnostic crossing counter.

On a qualified crossing:

```cpp
available = clamp(1 - normalizedModalEnergy(), 0, 1);
strength = 0.22 * normalizedSpeed * available;
for i = 0..7:
    directional = 1 + lastDirection * directionTilt[i];
    modes[i].velocity += crossingExcitation[i] * strength * directional;
    clamp mode velocity to its limit;
```

The crossing coefficients have alternating signs, so this is a restrained,
direction-sensitive transfer rather than a copy of the original impact.

## 6. Metallic modal body

There are eight independent, inharmonic damped oscillators. For each mode:

```text
modeAcceleration = -ωi² qi - 2γi q̇i
q̇i += modeAcceleration dt
qi += q̇i dt
modalSum += qi * outputGain[i] * (1 + lastDirection * directionTilt[i])
```

`γi = ln(1000) / decayT60Seconds[i]`: the declared decay time is T60.
Velocity, not position, is clamped after each integration step.

The current fresh, pre-specimen values are:

| i | frequency Hz | T60 s | impact excitation | crossing excitation | output gain | warp depth |
| -: | -: | -: | -: | -: | -: | -: |
| 0 | 375 | 8.40 | 235 | +18.0 | 1.07 | 0.008 |
| 1 | 586 | 8.40 | 270 | -14.0 | 0.95 | 0.020 |
| 2 | 773 | 7.90 | 285 | +11.0 | 0.95 | 0.038 |
| 3 | 1289 | 7.90 | 310 | -8.0 | 2.15 | 0.070 |
| 4 | 1580 | 6.80 | 275 | +6.0 | 2.35 | 0.060 |
| 5 | 2508 | 5.80 | 245 | -4.0 | 3.58 | 0.040 |
| 6 | 3380 | 4.20 | 155 | +2.5 | 4.30 | 0.025 |
| 7 | 4680 | 3.15 | 90 | -1.5 | 7.40 | 0.015 |

Current frequency control is deliberately modest and per-mode, not one strong
global pitch dive:

```cpp
smoothedEnergy = exp(-dt / 0.004) * smoothedEnergy
               + (1 - exp(-dt / 0.004)) * clamp(Eflex, 0, 1);

pitchWarp = 1 + frequencyWarpDepth[i] * smoothedEnergy;
frequency = min(restingFrequencyHz[i] * pitchWarp, 0.18 * sampleRate);
```

This gives higher pitch at high flex energy and a natural relaxation as the
spring settles. The `0.18 * sampleRate` ceiling protects integration and
alias-prone operation at low sample rates.

For diagnostics/saturation management, normalized modal energy is the mean:

```text
Emodal = mean_i((q̇i² + (ωrest,i qi)²) / modeVelocityLimit[i]²)
```

where `ωrest,i = 2π * restingFrequencyHz[i]`.

## 7. Radiation gate

Radiation is strongest near center crossing, inferred from phase rather than
using raw velocity alone so later lobes remain articulated:

```cpp
phaseVelocity = abs(v);
phaseDisplacement = ω0 * abs(x);
articulationVelocity = phaseVelocity / max(phaseVelocity + phaseDisplacement, 1e-6);
phasePulse = articulationVelocity * lerp(1, articulationVelocity, radiationCurvature);
targetPulse = phasePulse * (0.72 + 0.28 * clamp(Eflex, 0, 1));
```

Smooth it asymmetrically:

```cpp
attackCoeff  = exp(-dt / 0.0016);
releaseCoeff = exp(-dt / 0.0065);
coeff = targetPulse > radiationEnvelope ? attackCoeff : releaseCoeff;
radiationEnvelope = coeff * radiationEnvelope + (1 - coeff) * targetPulse;

radiationGate = (radiationFloor + (1 - radiationFloor) * radiationEnvelope)
              * (1 + lastDirection * radiationAsymmetry);
```

The nonzero floor preserves the metallic ridges between lobes; it is not a
hard mute. Direction only changes when struck/crossed, giving opposing swings
slightly different radiation.

## 8. Impact, mount, and direct flex paths

Impact is deterministic filtered xorshift noise. Convert the low 24 bits of
the xorshift state to approximately `[-1, 1)`, then use a brightness-dependent
one-pole low pass (`2400–6200 Hz`) and subtract a 180 Hz low-pass version:

```cpp
lowpass += (noise - lowpass) * lerp(alpha2400, alpha6200, brightness);
lowReject += (lowpass - lowReject) * alpha180;
impact = (lowpass - lowReject) * impactEnvelope
       * (0.16 + 0.24 * brightness);
impactEnvelope *= lerp(exp(-dt/0.003), exp(-dt/0.012), brightness);
impactBrightness *= exp(-dt/0.030);
```

The mount is a 82 Hz (specimen-adjusted) oscillator with a 0.11 s T60:

```text
mountAcceleration = -ωm² mountPosition - 2γm mountVelocity
mountVelocity += mountAcceleration dt
mountPosition += mountVelocity dt
mount = 0.36 * mountPosition
```

The direct flex audio is intentionally audible, unlike early design notes that
called for aggressive high-passing:

```cpp
flexAudio = 0.12 * (v / maximumVelocity)
          + 0.055 * clamp(a / 24000, -1, 1);
```

It supplies low physical weight and turn-around edge without becoming the main
metallic voice.

Finally, use a one-pole DC blocker at 5 Hz:

```cpp
y = x - previousInput + exp(-2π * 5 * dt) * previousOutput;
```

## 9. Stable specimen variation

A nonzero `specimenSeed` selects a stable individual spring. It is not live
randomness. Each property is derived independently from:

```cpp
hash32(v):
    v ^= v >> 16; v *= 0x7feb352d;
    v ^= v >> 15; v *= 0x846ca68b;
    v ^= v >> 16;

specimenUnit(tag) = 2 * ((hash32(seed ^ tag) & 0x00ffffff) / 0x00ffffff) - 1;
```

The present variations are:

| Property | Tags | Current range / formula |
| --- | --- | --- |
| Mount frequency | `0x100` | `82 * (1 + 0.06u)` Hz |
| Mode frequency | `0x200 + i` | base × `(1 + 0.03u)` |
| Mode T60 | `0x300 + i` | base × `(1 + 0.12u)` |
| Impact excitation | `0x400 + i` | base × `(1 + 0.08u)` |
| Crossing excitation | `0x500 + i` | base × `(1 + 0.10u)` |
| Output gain | `0x600 + i` | base × `(1 + 0.10u)` |
| Direction tilt | `0x700 + i` | `u * lerp(0.03, 0.10, i/7)` |
| Radiation curvature | `0x800` | clamp(`0.55 + 0.10u`, 0.4, 0.7) |
| Radiation floor | `0x801` | clamp(`0.24 + 0.025u`, 0.20, 0.28) |
| Radiation asymmetry | `0x802` | `0.07u` |
| Frequency-warp depth | `0x900 + i` | base × `(1 + 0.15u)` |

Noise uses a separate xorshift RNG seeded as `hash32(seed ^ 0xa341316c)` and
is reset to that state on motion reset. Consequently identical seed, sample
rate, initial condition, and strike sequence render bit-identically.

## 10. Break-in, reset, and sleeping

Unless break-in is locked, each strike increases it by:

```cpp
breakIn = clamp(breakIn + pow(shaped, 1.8) / 1000, 0, 1);
```

Changing break-in recomputes all wear/specimen coefficients but does not clear
motion itself. `restoreFactoryFresh()` sets break-in to zero, retains the
specimen seed, recomputes coefficients, and resets motion. `reset()` also
unlocks break-in. A new specimen should set a new seed and then restore fresh
state.

To sleep, all of the following must remain true for 50 ms:

```text
normalized flex energy  < 1e-8
normalized modal energy < 1e-8
impact envelope         < 1e-5
abs(mount position)     < 1e-5
abs(mount velocity)     < 1e-5
strike-light envelope   < 1e-5
abs(output volts)       < 1e-4
```

On sleep, clear all dynamic/filter state. The sleep-entry frame retains the
last computed output sample but has `sleeping = true` and `enteredSleep = true`.
If any state/output becomes non-finite, reset motion immediately and return a
zero frame.

## 11. Host/module contract

Doorstop's current host wrapper supplies the reference engine with normalized
strike values as follows:

* `TRIG` is a Schmitt rising edge, using 0.1 V low and 1.0 V high thresholds.
* At an external trigger, velocity is `VELOCITY / 10`, clamped to `[-1, 1]`.
  An unpatched velocity jack normals to `+5 V` (`+0.5`).
* A zero external velocity produces no strike.
* The manual strike parameter produces a positive strike. Default automation
  / MIDI strength is `0.5`; mouse height maps linearly from `1.0` at the top
  of the hit region to `0.10` at the bottom.
* Audio processing continues and visual state advances even when the output is
  disconnected.

Doorstop routes through `DoorstopEngineRouter`; `ReferenceV1` is the default.
The legacy engine is a separate, preserved implementation. Switching engines
crossfades the two processed outputs for 15 ms with equal-power gains
`cos(πt/2)` and `sin(πt/2)`, after resetting/preparing the destination engine.
That routing behavior is outside this model but is relevant if copying it into
a module that must switch live without clicks.

Persist reference-model identity as the seed, break-in amount, and lock state.
Doorstop additionally serializes engine choice and UI overflow preference.

## 12. Reimplementation checklist

For a faithful port, preserve these choices before retuning:

1. Use the nonlinear 21 Hz macro flex state for both visual motion and control.
2. Excite an 8-mode inharmonic body on strike, but gate its radiation by
   phase-normalized macro motion.
3. Keep a nonzero radiation floor and use only restrained crossing excitation.
4. Use higher-mode brightness weighting on the initial impact.
5. Preserve the separate filtered-noise snap, mount oscillator, and low direct
   flex contribution.
6. Make specimen differences deterministic and independent per property.
7. Bound mode and flex velocities, clamp modal frequency against sample rate,
   DC-block, softly limit, and reset on non-finite state.
8. Keep retriggering additive/stateful and energy-limited rather than resetting
   envelopes or normalizing every hit.

Useful verification is already encoded in
`tests/doorstop_reference_engine_spec.cpp`: deterministic same-seed renders,
distinct seeds, bounded output at 44.1–192 kHz, qualified crossings, eventual
sleep, and a controlled balance between macro low body and upper metallic
activity. `tools/doorstop_reference_render.cpp` is the standalone deterministic
renderer for listening and analysis.

## 13. Corpus and stochastic validation

A port should be evaluated as a *population of specimens*, not tuned until one
seed reproduces one recording. The stable specimen seed deliberately moves
frequencies, decay, excitation, radiation, and spectral balance. Individual
renders may therefore occupy different parts of the physical reference range.

`tools/audit_doorstop_corpus.py` automates the comparison:

```sh
# Measure the reviewed real recordings only.
make doorstop-corpus-audit

# Render the standard velocity/seed population and compare it with the corpus.
make doorstop-reference-evaluate
```

The audit uses the reviewed onsets in
`tools/doorstop_reference_manifest.json`. It measures exact 0–30, 30–100,
100–300, and 300–1000 ms regions in addition to whole-hit features, peak
density/Q, spectral flatness, pulse rate, and T20. Quality flags identify
recordings that merit listening review but do not silently exclude them.

To prevent recordings with many repeated strikes from dominating, the audit
first takes the median feature value within each recording. It then defines
the descriptive physical corridor as the 10th through 90th percentiles across
those per-recording medians. A model batch is judged along three independent
axes:

* **Containment:** fraction of seed renders inside the physical corridor.
* **Coverage:** fraction of the physical corridor spanned by the model's
  10th-to-90th percentile range.
* **Median bias:** displacement of the model median, measured in corridor
  widths.

Perfect containment is not the objective; some unusual specimens are useful.
The important failure modes are a population that is systematically displaced,
too narrow to express the recorded families, or broad only because it produces
implausible outliers. The standard evaluation grid currently uses 16 fixed
seeds at strike velocities 0.5, 0.75, and 1.0 (48 renders), which is large
enough for directional tuning. Increase `DOORSTOP_REFERENCE_SEEDS` when
estimating final distribution tails.

For architectural listening experiments:

```sh
make doorstop-variant-evaluate
```

This builds the standalone renderer with
`DOORSTOP_REFERENCE_ANALYSIS`; ordinary engine and test builds do not contain
the variant selector or its hot-path branch. It renders four analysis probes:

| Variant | Purpose |
| --- | --- |
| `current` | Bit-identical baseline from the production compilation path |
| `spring-only` | Impact, mount, flex, and dispersive texture without modal output or junction coupling |
| `modes-only` | The present radiated modal bank without the other audible components |
| `spring-forward` | No junction coupling, shorter/darker modal anchors, reduced modal mix, and a stronger dispersive path |

`tools/compare_doorstop_variants.py` scores all four populations and writes
same-seed, same-velocity listening groups under
`build/doorstop-variant-analysis/listening`. Files within a group are exactly
RMS-matched over 50 ms–2.5 s, with the common level reduced when necessary to
keep peaks at or below 0.95. The objective score is deliberately only a
screening order; a listening decision is still required before promoting any
candidate to a real engine version.
