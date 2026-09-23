# Undertow — Initial Specification v0.2

## 1. Working Title

**Undertow** is a Leviathan-native compact oscillator concept: sub-forward, playable, and immediately musical, with a strong fundamental, curated harmonic shaping, and a dedicated sub voice.

Internal inspiration can be taken from the general category of compact sub-capable analog-style VCOs, but this module should remain original in naming, panel design, control choices, graphics, behavior, and language.

Recommended release name: **UNDERTOW**  
Subtitle: **Sub-Harmonic VCO**

## 2. Product Summary

Undertow is a compact oscillator focused on three voices:

- `SINE`: clean fundamental output
- `SHAPE`: sine-derived harmonic output
- `SUB`: dedicated sub-octave output

The module should feel simple and immediate, not like a large feature oscillator reduced onto a small panel. Its identity comes from:

- a strong, centered sine core
- a shaped output that stays fundamental-forward
- a powerful and musically useful sub voice
- a lightweight, informative display

## 3. v1 Scope

This document is now explicitly split around a realistic first release.

### 3.1 Required for v1

- `SINE`, `SHAPE`, `SUB` outputs
- `V/OCT`, `LIN FM`, `SHAPE CV`, `SYNC`, `S-GATE` inputs
- `COARSE`, `FINE`, `SHAPE`, `RIPPLE` controls
- hard sync
- one default sub mode
- one lightweight waveform display mode
- stable monophonic behavior
- polyphony only if implementation cost remains modest and behavior stays coherent

### 3.2 Optional for v1

- polyphony
- one additional sub mode
- octave-stepped coarse tune option
- context-menu display options

### 3.3 Deferred Beyond v1

- `EXP FM`
- `MIX` output
- multiple quality tiers exposed to the user
- analog character modes
- multiple display modes
- broad sub-mode families
- through-zero FM

## 4. Design Goals

### 4.1 Musical Goals

The module should sound:

- centered
- substantial
- melodic
- clean at simple settings
- animated under timbre CV
- strong in bass patches
- capable of more aggressive sync/FM tones at extreme settings

It should not become a generic wavetable or “many algorithms” oscillator.

### 4.2 UX Goals

A user should understand it quickly:

- top: waveform/display state
- middle: pitch and timbre
- bottom: patch points

The experience should feel simpler than a complex oscillator and faster to patch than a menu-heavy module.

### 4.3 Technical Goals

- accurate 1V/oct tracking
- low aliasing at normal musical use
- smooth control and CV response
- no expensive visualization work in the audio thread
- CPU cost low enough to allow many instances in practical Rack patches

## 5. Panel Direction

Target width: **similar to Proc-width**, using existing module sizing conventions rather than a hardcoded HP guess in this document.

The panel should use Leviathan’s own language and should not reference another manufacturer’s layout or visual identity.

Suggested v1 structure:

```text
┌───────────────────────────────┐
│ UNDERTOW                      │
│ [ waveform display ]          │
├───────────────┬───────────────┤
│ COARSE        │ FINE          │
│ [knob]        │ [knob]        │
├───────────────┼───────────────┤
│ SHAPE         │ RIPPLE        │
│ [knob]        │ [knob]        │
├───────────────┼───────────────┤
│ LIN FM        │ SHAPE CV      │
│ [atten+jack]  │ [atten+jack]  │
├───────────────┼───────────────┤
│ V/OCT         │ SYNC          │
│ jack          │ jack          │
├───────────────┼───────────────┤
│ S-GATE        │               │
│ jack          │               │
├───────────────┴───────────────┤
│ SINE   SHAPE   SUB            │
│ jack   jack    jack           │
└───────────────────────────────┘
```

This is directional only. Final placement should follow the repo’s existing panel/component placement patterns.

## 6. Controls and I/O

### 6.1 Coarse Tune

Name: `COARSE`

- musically useful oscillator range
- default around low-mid playable range
- optional context menu later for octave-stepped behavior

### 6.2 Fine Tune

Name: `FINE`

- modest range
- default centered

### 6.3 Shape

Name: `SHAPE`

Primary timbre control.

Behavior target:

- low: near-sine
- mid: stronger harmonic content, still pitch-centered
- high: brighter and more angular, but still recognizably derived from the sine core

The shaped output must not be a plain sine-to-triangle crossfade.

### 6.4 Ripple

Name: `RIPPLE`

Secondary timbre intensity control.

Purpose:

- control disturbance or agitation of the harmonic shape
- keep `SHAPE` as the family selector
- use `RIPPLE` as the intensity/animation control

At low settings, the module should stay melodic and restrained. At high settings, it may become brighter, rougher, and semi-folded.

### 6.5 Linear FM

Name: `LIN FM`

- input with attenuverter preferred
- ordinary linear FM for v1
- no through-zero requirement in v1

### 6.6 1V/Oct

Name: `V/OCT`

- Rack-standard pitch input
- monophonic minimum
- polyphonic if implemented cleanly

### 6.7 Sync

Name: `SYNC`

- hard sync on rising edge for v1
- anti-aliasing or discontinuity-softening should be part of the implementation plan

### 6.8 Shape CV

Name: `SHAPE CV`

- attenuverter preferred
- clamp to safe internal shaping range

### 6.9 Sub Gate

Name: `S-GATE`

v1 behavior:

- rising edge resets sub phase/state
- gate high enables sub output

Keep this simple in v1. More exotic sub gate behaviors can wait.

### 6.10 Outputs

Required:

- `SINE`
- `SHAPE`
- `SUB`

`MIX` is deferred.

## 7. Output Targets

### 7.1 Sine

- approximately `10 Vpp`
- centered around `0 V`
- clean enough for FM, filtering, bass, and general-purpose patching

### 7.2 Shape

- approximately `10 Vpp`
- centered around `0 V`
- main identity output

### 7.3 Sub

- approximately `10–12 Vpp`
- default: `-1 octave`
- strong and bass-forward

Default v1 sub mode: square `-1 octave`.

## 8. DSP Direction

### 8.1 Core Architecture

Use a phase-accumulator oscillator core with explicit per-channel state.

Example shape only:

```cpp
struct OscState {
    float phase = 0.f;
    bool subFlip = false;
    dsp::SchmittTrigger syncTrigger;
    dsp::SchmittTrigger sGateTrigger;
};
```

### 8.2 Performance Policy

This repo prioritizes hot-path efficiency.

For the audio path:

- prefer LUTs, SIMD helpers, cached coefficients, and fast approximations
- avoid endorsing scalar `std::sin`, `std::pow`, `std::tanh`, `std::exp`, etc. as the intended shipped path
- prototype math can start simple, but release implementation should be shaped by profiling and CPU discipline

### 8.3 Pitch Calculation

Pitch sources:

- base pitch from coarse/fine
- `V/OCT`
- linear FM contribution

Representative form:

```cpp
float pitchVolts = basePitchVolts + voct;
float freq = voltsToFreqFast(pitchVolts);
freq = applyLinearFm(freq, linFm, linFmDepth);
freq = clamp(freq, minFreq, maxFreq);
```

Use a fast project-appropriate pitch conversion path.

### 8.4 Phase Advance

```cpp
phase += freq * args.sampleTime;
phase -= std::floor(phase);
```

If oversampling or discontinuity handling is introduced, keep it tightly scoped and efficient.

### 8.5 Sine and Reference Waveforms

The implementation may use:

- SIMD sine helpers
- a lookup table
- a polynomial approximation

The important thing is that the shipped oscillator should not lean on slow scalar transcendentals in the inner loop unless profiling proves the cost is negligible.

### 8.6 Shape Output

The `SHAPE` voice should preserve the impression of a strong fundamental while adding animated harmonic structure.

Recommended design direction:

- start from the sine core
- add a small curated harmonic set
- add a bounded nonlinearity or angular endpoint behavior
- keep DC balance under control

Representative goal, not literal implementation:

```cpp
float sine = sineCore(phase);
float harmonicRipple = harmonicShape(phase, shape, ripple);
float shaped = shapeBlend(sine, harmonicRipple, shape, ripple);
shaped = dcSafeSoftLimit(shaped);
```

Do not treat the pseudocode in this document as the exact shipping formula.

### 8.7 Anti-Aliasing

Risk points:

- hard sync
- sub square transitions
- aggressive shape discontinuities
- high pitch
- audio-rate FM

v1 guidance:

- implement one efficient default anti-alias strategy
- do not expose quality tiers unless profiling shows a real, user-meaningful tradeoff

The shipped default should be the best practical compromise for many instances.

### 8.8 Sub Oscillator

Default design:

- divide-by-two square derived from phase wrap or a sync-aware sub state
- resettable from `S-GATE`

Representative form:

```cpp
if (phaseWrapped) {
    subFlip = !subFlip;
}
float sub = subFlip ? 1.f : -1.f;
```

### 8.9 Sync

v1:

- rising-edge hard sync
- reduce the worst discontinuity artifacts with a lightweight correction strategy

## 9. Visualization

### 9.1 v1 Display Goal

The display should communicate:

- the current shaped waveform
- sine reference relationship
- sub activity
- sync activity

It should feel alive, but it is secondary to oscillator stability and CPU efficiency.

### 9.2 v1 Display Scope

Implement one display mode only:

- `Waveform`

Suggested behavior:

- draw one or two cycles of the shaped output
- optionally overlay faint sine reference
- show sub state as a low rail or simple pulse indicator
- flash or tick on sync activity

Additional display modes are deferred.

### 9.3 Data Flow

For v1, prefer a lightweight preview model:

- no audio-thread allocations
- no expensive UI-thread recomputation from scratch
- use cached module state and/or a small decimated snapshot
- keep audio-thread communication lightweight and lock-free where possible

There should be no requirement for an OpenGL path in v1.

## 10. Polyphony

Polyphony is desirable, but not mandatory for first release if it meaningfully complicates the module or raises CPU cost.

If implemented:

- `V/OCT` channel count determines voice count
- mono CVs broadcast
- poly CVs modulate per voice
- outputs match voice count

Display can remain single-channel in v1.

## 11. Context Menu

Keep the v1 menu small.

Recommended v1 menu:

```text
Display
  Waveform

Coarse Tune Mode
  Continuous
  Octave Stepped
```

Possible later additions:

- sub mode
- analog character
- quality mode

Do not overexpose configuration until there is a proven need.

## 12. Default Behavior

With no CV patched:

- `SINE` is clean
- `SHAPE` is subtly richer than pure sine
- `SUB` is a strong `-1 octave` square

Suggested defaults:

- `COARSE`: musically useful bass-mid pitch
- `FINE`: centered
- `SHAPE`: modestly above pure sine
- `RIPPLE`: restrained, not extreme
- FM depths: zero
- shape CV depth: conservative

The default patch should immediately communicate identity without sounding harsh.

## 13. Acceptance Criteria

### 13.1 Functional

- 1V/oct tracks within a musically acceptable tolerance over at least `5 octaves`
- `SINE` and `SHAPE` are centered and roughly `10 Vpp`
- `SUB` is centered and roughly `10–12 Vpp`
- sync resets phase reliably
- `S-GATE` resets/enables sub as specified
- module state serializes/deserializes correctly

### 13.2 Musical

- low `SHAPE` remains clearly sine-like
- mid `SHAPE` adds harmonic motion without losing pitch center
- high `SHAPE` becomes brighter and more angular without collapsing into unusable harshness
- `SUB` is strong enough to anchor bass patches
- `LIN FM` is usable at moderate settings

### 13.3 Technical

- no audio-thread allocations in steady operation
- display path does not cause audio instability
- CPU cost remains reasonable for multi-instance use

### 13.4 Visual

- display updates smoothly at ordinary Rack UI cadence
- waveform visibly responds to `SHAPE` and `RIPPLE`
- sub activity is visible
- sync activity is visible

## 14. Implementation Plan

### Phase 1

- module skeleton
- `SINE`, `SHAPE`, `SUB`
- pitch path
- basic sub
- basic panel

### Phase 2

- `LIN FM`
- `SHAPE CV`
- `SYNC`
- `S-GATE`

### Phase 3

- waveform display
- sine reference
- sub and sync indicators

### Phase 4

- tuning and profiling
- anti-alias refinement
- optional polyphony
- limited context menu polish

### Phase 5

- panel art polish
- validation
- regression tests where practical

## 15. Avoided Risks

Do not:

- borrow another manufacturer’s visual identity, naming, or layout language
- turn this into a generic many-mode oscillator
- put expensive visualization work in the audio thread
- overbuild v1 with menu-heavy features
- ship obvious aliasing or unstable output behavior for the sake of feature count

## 16. Identity Statement

Undertow is a compact sub-harmonic oscillator for Rack: a clean sine voice pulled downward by a powerful sub and animated by a curated harmonic shaper. It should feel simple, immediate, bass-capable, and alive.
