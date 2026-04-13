# Bifurx Implementation Plan

## Purpose

This document translates [Bifurx.md](/mnt/c/msys64/home/Plasm/Leviathan/doc/Bifurx.md) into an implementation-oriented plan for the Leviathan plugin.

The goal is to move from concept/spec language into a build sequence that can be executed in phases without losing the core identity of the module.

## v1 Scope Freeze

### In Scope

- New module: `Bifurx`
- Mono audio path for v1
- Dual-core digital SVF architecture
- 10-mode topology switch modeled on the documented dual-peak multimode behavior
- Front-panel controls matching the intended hardware workflow:
  - `LEVEL`
  - `FREQ`
  - `RESO`
  - `SPAN`
  - `BALANCE`
  - `FM AMT` slider
  - `SPAN CV ATTEN` slider
  - `MODE` selector
  - `TITO` 3-state switch
- Front-panel ports:
  - `IN`
  - `OUT`
  - `V/OCT`
  - `FM`
  - `RESO CV`
  - `BALANCE CV`
  - `SPAN CV`
- Real-time preview area showing:
  - nominal filter magnitude response curve
  - spectrum modification overlay derived from live audio
- Original panel art and original explanation text
- JSON state only where needed beyond parameter values

### Explicitly Out of Scope For v1

- Stereo processing
- Expander sync implementation
- Claim of exact analog Belgrad-equivalent internal behavior
- Arbitrary nonlinear analog circuit modeling
- Oversized calibration/analysis UI beyond the main preview
- Preset system beyond standard Rack patch save/load

## Design Targets

### Module Identity

Bifurx should feel like:
- a performable dual-peak filter first
- a visually explanatory module second
- an original Leviathan module, not a literal visual clone
- a "color box" where saturation and distortion are core features, not secondary
- a high-sensitivity instrument where "everything you do matters"—no dead zones or flat parameter regions

Critical identity correction from demo analysis:
- Bifurx should not be treated as “two filters” in user-facing behavior
- it should behave like a single instrument built around two interacting resonant peaks
- parameter mappings should reinforce interaction, not isolation

### v1 Quality Bar

v1 does not need to be a component-level analog model.
It does need to satisfy:
- musically convincing dual-peak filter behavior
- clear mode differentiation
- stable modulation
- useful response preview
- no obvious CPU or UI pathology
- dual oscillator behavior with musical V/OCT tracking

## Engineering Assumptions

### DSP Strategy

Use two digital SVF cores as the baseline.

Recommended baseline for v1:
- TPT/state-variable style core (Zavalishin style) to support audio-rate modulation and nonlinearities
- per-core outputs available internally:
  - lowpass
  - bandpass
  - highpass
  - notch
- symmetric frequency detune around a shared center frequency
- treat the two cores as a coupled system, not as two independent processors that are merely mixed together
- ensure phase-accurate combinations in parallel/serial paths to preserve phaser-like movement in notch modes

Design implication:
- SPAN, BALANCE, RESO, and TITO should all strengthen the sense of interacting peaks
- TITO should be implemented as an explicit signal routing change:
  - CLEAN: standard topology
  - SM (Self-Mod): internal core feedback loops
  - XM (Cross-Mod): mutual inter-core modulation
- anti-pattern: independent-core design where interaction is weak and only the final mix stage creates apparent complexity

### Display Strategy

Use a nominal linear model for the response curve.

That means:
- the preview curve is based on the current mode/parameter state
- nonlinear/TITO coloration is not required to be fully represented in the line curve
- the live FFT overlay can communicate actual signal modification separately
- the preview should make the audible-band placement of the two peaks legible, especially when SPAN pushes one or both peaks toward subsonic or ultrasonic regions

This keeps the display tractable and honest.

### UI Strategy

The module should be built around three visual layers:
- panel art
- parameter / port controls
- preview display widget

The preview should be cached and updated at a throttled UI rate.

## Frozen v1 Parameter Spec

The following values are now frozen for v1 implementation.

### MODE

- Stored range: `0..9`
- Step count: `10`
- Default: `0`
- Default mode label: `LL`
- Behavior: hard-stepped selector, no CV in v1

### TITO

- Stored range: `0..2`
- States:
  - `0 = XM`
  - `1 = CLEAN`
  - `2 = SM`
- Default: `1`
- Default state label: `CLEAN`

### FREQ

- Stored parameter range: normalized `0..1`
- Default: `0.5`
- Mapping: exponential/logarithmic
- Effective frequency target: approximately `4 Hz .. 28 kHz`
- Notes:
  - `V/OCT` is summed into the frequency domain
  - `FM` is applied through `FM AMT`

### LEVEL

- Stored parameter range: `0..1`
- Default: `0.5`
- Intended behavior:
  - near `0.0`: near-muted / very low drive
  - around `0.5`: approximately unity for a typical Rack `±5 V` audio signal
  - near `1.0`: strong pre-filter drive / overdrive ("hair" and "growl")
- Notes:
  - this is a gain/drive/tone control, not a bipolar trim
  - perceptually, this should behave as a tone/drive control, not just an amplitude trim
  - pre-filter saturation is part of the v1 identity, not an optional afterthought

### RESO

- Stored parameter range: `0..1`
- Default: `0.35`
- Mapping: nonlinear internal mapping to resonance / feedback amount
- Target behavior:
  - lower and middle range should be musically usable
  - upper range should reach self-oscillation-like behavior in at least some modes
  - resonance threshold is allowed to vary by mode (not normalized)

### SPAN

- Stored parameter range: `0..1`
- Default: `0.0`
- Mapping: nonlinear power curve (finer control near zero)
- Internal domain: `0 .. 8 octaves`
- Notes:
  - symmetric detune around center frequency
  - zero default means the module opens in the collapsed / single-cutoff-like regime
  - SPAN should be treated as a timbre-morphing control, not a plain detune control
  - small motion near zero should remain subtle, while larger settings should become dramatically more vocal/formant-like

### BALANCE

- Stored parameter range: `-1..+1`
- Default: `0.0`
- Notes:
  - normalized symmetric skew domain
  - used internally to bias lower vs upper peak dominance
  - should be implemented as energy redistribution (affecting both gain and resonance/Q), not a simple final crossfade between core outputs

### FM AMT

- Stored parameter range: `-1..+1`
- Default: `0.0`
- Behavior: bipolar attenuverter
- Notes:
  - negative polarity is intentionally supported

### SPAN CV ATTEN

- Stored parameter range: `-1..+1`
- Default: `0.0`
- Behavior: bipolar attenuverter
- Notes:
  - this is intentionally bipolar because `SPAN CV` is bipolar in musical use

## Frozen v1 Jack/CV Handling

The following jack semantics are considered fixed for v1.

- `IN`: mono audio input
- `OUT`: mono audio output
- `V/OCT`: accept `-10 V .. +10 V`
- `FM`: accept `-10 V .. +10 V`
- `RESO CV`: accept `0 V .. +8 V`
- `BALANCE CV`: accept bipolar `-5 V .. +5 V`
- `SPAN CV`: accept `-10 V .. +10 V`

Interpretation note:
- although `SPAN CV` should accept `-10 V .. +10 V`, the musically useful “full range” can be concentrated closer to `-5 V .. +5 V`

## Behavioral Calibration Notes From Demo Analysis

The following observations should guide tuning decisions during implementation.

### Coupled Peaks First

- The user-facing behavior should read as one system with two interacting resonant peaks
- Avoid designs where the cores feel independent until the final output mix

### SPAN Priority

- SPAN is one of the main identity controls
- It should feel subtle near zero and increasingly dramatic at larger values
- It should be treated as timbre morphing, not just core detuning

### BALANCE Priority

- BALANCE should change spectral center-of-gravity and peak energy distribution
- A simple output crossfade is not sufficient

### LEVEL Priority

- LEVEL must audibly add hair / growl at higher settings
- It should interact with resonance and should be implemented as a pre-filter drive stage

### Audio-Rate Modulation Requirement

- Frequency modulation must tolerate audio-rate use
- The implementation should also remain open to internal audio-rate interaction between the two cores under TITO states
- Parameter smoothing and topology choice should be judged against this requirement

### Self-Oscillation Requirement

- High resonance should allow two-tone / dual-peak oscillatory behavior in at least some modes
- BALANCE and SPAN should still matter in that regime

### Additional Calibration From Second Demo Pass

- SPAN can easily move one or both peaks outside the audible range
- FREQ and SPAN must be treated as a coupled navigation system, not independent axes
- the preview should help the user understand where the two peaks sit relative to the audible band
- BALANCE should track low-vs-high prominence perceptually, not core identity mechanically
- resonance threshold is allowed to vary by mode and should not be normalized away
- double-notch / notch-family modes should retain phase-like movement character rather than sounding like sterile static cuts
- pinging / transient excitation is a legitimate use case and should remain stable
- layered FM plus internal coupling should remain musically usable, not collapse into zipper noise or obvious digital breakage

## Concrete v1 Module Surface

### Parameters

Proposed v1 parameter list:
- `LEVEL_PARAM`
- `FREQ_PARAM`
- `RESO_PARAM`
- `SPAN_PARAM`
- `BALANCE_PARAM`
- `FM_AMT_PARAM`
- `SPAN_CV_ATTEN_PARAM`
- `MODE_PARAM`
- `TITO_PARAM`

### Inputs

Proposed v1 input list:
- `IN_INPUT`
- `VOCT_INPUT`
- `FM_INPUT`
- `RESO_CV_INPUT`
- `BALANCE_CV_INPUT`
- `SPAN_CV_INPUT`

### Outputs

Proposed v1 output list:
- `OUT_OUTPUT`

### Lights

Keep v1 lights minimal.
Possible lights:
- small state illumination for sliders or TITO state if needed
- avoid large light surface until panel layout is fixed

## Parameter/CV Interpretation

### Frequency

`FREQ` is the shared center frequency.

v1 implementation target:
- exponential frequency mapping
- `V/OCT` summed into frequency domain
- `FM` treated as frequency modulation input scaled by `FM AMT`

### Span

`SPAN` controls symmetric detuning around the center frequency.

Recommended implementation:
- map the knob nonlinearly into an octave-distance domain
- apply symmetric split:
  - `f1 = f0 * 2^(-span/2)`
  - `f2 = f0 * 2^(+span/2)`

### Balance

`BALANCE` should skew the relative dominance of the lower and upper peaks.

Recommended v1 behavior:
- balance affects per-core resonance/Q AND per-core output weighting
- implement as energy redistribution across the spectrum
- keep the behavior symmetrical around center

### Resonance

`RESO` controls the overall resonant emphasis and should support self-oscillation-like behavior in at least some modes.

Implementation note:
- resonance threshold is allowed to vary by mode
- v1 should not artificially normalize self-oscillation onset across modes

### Level

`LEVEL` is both input trim and overdrive entrance.

Recommended v1 behavior:
- modest clean range at lower settings
- saturating pre-filter drive at higher settings ("color box" approach)
- saturation should interact with resonance peaks

### TITO

Three states:
- neutral (CLEAN)
- self-mod (SM)
- cross-mod (XM)

v1 goal:
- make each state audibly distinct through topology changes
- CLEAN: standard parallel/serial routing
- SM: internal resonance feedback modulation within each core
- XM: inter-core audio-rate modulation (Core A modulates B, B modulates A)
- treat TITO as a coupling topology switch, not just a flavor/distortion knob

## Mode Mapping Plan

The 10 modes should be implemented as explicit topology selections over the two SVF cores.

Planned v1 mode mapping:
- `LL`: LP into LP cascade
- `LB`: LP + BP parallel blend (with phase-accurate cancellation gaps)
- `NL`: LP with subtractive notch character (tuned below main cutoff)
- `NN`: notch into notch cascade (phaser-like movement)
- `LH`: LP + HP parallel blend (variable-width band-rejection)
- `BB`: BP + BP dual-formant blend
- `HH`: HP into LP bandpass window (flat-top at high SPAN)
- `HN`: HP with subtractive notch character (tuned above HP corner)
- `BH`: BP + HP parallel blend (lower formant + cancellation gap)
- `HL`: HP into HP cascade

This mapping should be coded clearly and independently from UI naming so it can be tuned later. Critical: ensure phase-accurate combinations to maintain characteristic "gap" and "phaser" movement.

## DSP Implementation Phases

### Phase 1: Linear Core

Deliverables:
- dual SVF cores
- parameter mapping for frequency/span/resonance/balance
- 10 mode topology switch
- stable mono audio output

Requirements:
- no nonlinear/TITO behavior yet beyond placeholders if necessary
- CPU should already be reasonable
- modes should produce clearly different responses
- baseline behavior should already read as two interacting peaks, not two detached filters

### Phase 2: Curve Preview

Deliverables:
- real-time magnitude response display
- log-frequency x-axis mapping
- stable redraw path

Requirements:
- based on nominal linear transfer behavior
- should respond immediately to parameter changes
- should not allocate or do heavy work in the audio thread
- should make the two peak locations readable relative to the audible band so SPAN/FREQ navigation stays intelligible

### Phase 3: Input Drive And Nonlinear Character

Deliverables:
- `LEVEL` drive behavior
- resonance/path saturation where appropriate
- initial TITO differentiation

Requirements:
- no obvious zipper/crackle under normal use
- neutral / self-mod / cross-mod should be audibly distinct
- TITO differentiation should come from signal routing/coupling behavior first, with distortion only as a support characteristic

### Phase 4: Spectrum Modification Overlay

Deliverables:
- rolling input/output analysis buffers
- FFT-based overlay data
- purple attenuation / cyan boost / near-white neutral color mapping

Requirements:
- compute off the audio thread or via lock-free handoff to UI-side analysis path
- throttled update rate
- visually stable, not noisy nonsense during silence

### Phase 5: Tuning Pass

Deliverables:
- gain trims by mode if needed
- self-oscillation threshold tuning
- better TITO voicing
- preview polish

## Preview Architecture

### Curve Layer

Nominal line curve only.

Implementation intent:
- evaluate a display transfer model across log-spaced frequencies
- draw a polyline in the preview window
- do not attempt sample-accurate nonlinear plotting in v1

### Spectrum Layer

Derived from live input/output comparison.

Implementation intent:
- capture short rolling windows of input and output
- FFT them periodically
- compute per-bin delta in dB
- map to color and draw under/behind the curve

### Performance Constraints

- preview updates should be throttled
- cached geometry/color buffers should be reused
- widget should be wrapped in a framebuffer when appropriate

## Rack Integration Plan

### Files To Add

Expected initial file set:
- `src/Bifurx.cpp`
- `res/Bifurx.svg`
- optional helper headers if the display/DSP split gets large

Possible supporting files if needed:
- `src/BifurxDSP.hpp`
- `src/BifurxDisplay.hpp`
- `src/BifurxDisplay.cpp`

### Scaffolding Sequence

1. create SVG with component anchors
2. scaffold module/widget source
3. wire params/ports/switches
4. add preview placeholder widget
5. add DSP baseline

## Serialization

v1 should rely mostly on parameter persistence.

Only add custom JSON if needed for:
- preview preferences
- future hidden/internal state not represented as params

Default recommendation:
- avoid custom serialization in the first implementation unless necessary

## Risks

### 1. DSP Fidelity Risk

The largest risk is not “can it be implemented,” but whether the first-pass digital topology feels musically close enough to justify the module identity.

Mitigation:
- prioritize clear dual-peak behavior and good modulation feel
- do not chase analog-perfect claims in v1

### 2. TITO Ambiguity

The least fully specified part is the nonlinear/cross-mod character.

Mitigation:
- treat TITO as a coupling topology switch in v1
- implement routing differences before chasing saturation differences
- tune by ear after the linear baseline is solid

### 3. Preview Truthfulness

Users may interpret the line curve as exact behavior.

Mitigation:
- keep the line curve nominal and stable
- let the FFT color layer communicate actual signal modification

### 4. CPU Risk

A complex preview plus two filters plus nonlinear behavior can become expensive.

Mitigation:
- stage the implementation
- cache display work
- throttle FFT refresh
- keep the first-pass DSP scalar and clear before optimizing

## Acceptance Criteria For v1

Bifurx v1 is acceptable when:
- all controls and ports are wired and behave coherently
- all 10 modes produce distinct and musically useful responses, including notch-family modes that retain convincing phase-like motion
- SPAN creates an intelligible dual-peak split and feels like a "timbre morph" rather than a simple detune
- BALANCE meaningfully redistributes energy (gain and Q) between the two peaks rather than behaving like a simple output mix
- RESO can reach strong resonant behavior and at least some self-oscillation-like states, with mode-dependent thresholds preserved
- TITO states are audibly different through explicit routing changes (Self-Mod vs Cross-Mod)
- module tracks V/OCT reasonably (approx 5 octaves) in self-oscillation, behaving as a dual sine oscillator
- system remains stable under "pinging" (fast transient excitation) through V/OCT or FM inputs
- response curve updates correctly with mode and control changes
- spectrum overlay reads as useful rather than decorative noise
- module performs reliably in Rack without UI hitching or DSP instability
- frequency modulation remains stable under fast modulation and does not zipper under normal audio-rate use
- the module conveys a high-sensitivity "instrument" feel where parameter interactions are continuously expressive

## Post-v1 Roadmap

Not part of the first implementation, but should remain architecture-compatible:
- expander-based stereo parameter sync
- more exact nonlinear/TITO modeling
- optional gain normalization modes
- polyphonic processing refinement
- alternate preview modes

## Progress Tracker

### Current Status

- [x] Research/spec doc exists in `doc/Bifurx.md`
- [x] Initial implementation plan created
- [ ] v1 control/default ranges frozen
- [ ] SVG/component layout created
- [ ] Module scaffolded
- [ ] Dual-core SVF baseline implemented
- [ ] Mode topology mapping implemented
- [ ] Response curve display implemented
- [ ] TITO/nonlinear character implemented
- [ ] FFT spectrum overlay implemented
- [ ] Tuning/validation pass completed

## Recommended Immediate Next Step

Before code generation:
- create `res/Bifurx.svg` using the frozen v1 parameter spec in this plan
- then scaffold the module from the SVG
- then implement the linear dual-core baseline before nonlinear/TITO work

That is the most efficient path from spec to implementation.
