# Bifurx Concrete Filter Tuning Spec

## Purpose

This document turns the earlier `Bifurx` filter plan into a concrete tuning pass for [src/Bifurx.cpp](/mnt/c/msys64/home/Plasm/Leviathan/src/Bifurx.cpp).

The goal is not to redesign the module. The goal is to tune the existing implementation so that:

- controls feel intentional
- the 10 modes are clearly differentiated
- the 4 circuit types have obvious character
- resonance and self-oscillation behave predictably
- the preview remains useful as a nominal representation

## Scope

Tune only the current architecture:

- `shapedSpan()`
- `levelDriveGain()`
- `resoToDamping()`
- `signedWeight()`
- `DfmCore::process()`
- `Ms2Core::process()`
- `PrdCore::process()`
- `combineModeResponse()`
- `makePreviewModel()`

Do not replace the topology, add oversampling, or redesign the display in this pass.

## Baseline Assumptions

- WSL builds are non-authoritative for final Rack linking.
- `Bifurx` is unreleased, so behavior changes are allowed.
- The filter should continue matching the panel/control semantics in [doc/Bifurx.md](/mnt/c/msys64/home/Plasm/Leviathan/doc/Bifurx.md).

## Tuning Method

Every tuning decision should be checked in the same order:

1. Listen
2. Check the preview curve
3. Sweep the control that exposed the issue
4. Make one small coefficient change
5. Recheck level consistency across nearby modes and circuits

Do not tune multiple subsystems at once. Lock one layer, then move on.

## Fixed Test Setup

Use the same test gestures every time so comparisons mean something.

### Audio sources

- saw at 5 Vpp equivalent
- pulse at 5 Vpp equivalent
- white noise
- sine

### Control positions

Use these anchor positions repeatedly:

- `LEVEL`: `0.20`, `0.50`, `0.85`
- `FREQ`: `0.15`, `0.35`, `0.55`, `0.80`
- `RESO`: `0.10`, `0.45`, `0.75`, `0.92`
- `SPAN`: `0.00`, `0.12`, `0.30`, `0.60`, `1.00`
- `BALANCE`: `-0.75`, `0.00`, `0.75`
- `TITO`: center first, then self-mod, then cross-mod

### CV state

Start with no CV patched.

After the static pass is acceptable, add:

- slow bipolar FM
- slow SPAN CV
- V/OCT tracking check during self-oscillation

## Pass Order

## Pass 1: Clean SVF Baseline

Work only on the `SVF` circuit.

Touch only:

- `shapedSpan()`
- `resoToDamping()`
- `signedWeight()`
- `combineModeResponse()`

Do not tune `DFM`, `MS2`, `PRD`, or TITO yet.

### Pass 1 Targets

- `FREQ` sweep feels even enough across the full travel
- `SPAN` moves from collapsed to separated in a controlled way
- `RESO` rises smoothly and reaches obvious self-oscillation near the top of the knob
- `BALANCE` shifts peak emphasis without acting like a crude output fader
- all 10 modes are audibly distinct at `LEVEL=0.50`

### Pass 1 Concrete Checks

#### `shapedSpan()`

Current code:

- `pow(x, 1.65)`

Target behavior:

- `SPAN=0.12` should already produce a small but audible peak split
- `SPAN=0.30` should read as clearly dual-peak in `Band + Band`, `Low + High`, and `Notch + Notch`
- `SPAN=1.00` should be wide without making the middle of the control feel empty

Decision rule:

- If the first 20% of the knob does too little, reduce the exponent.
- If the first 20% does too much, increase the exponent.

#### `resoToDamping()`

Current code:

- `2.f - 1.98f * pow(r, 1.35f)`

Target behavior:

- `RESO=0.45` should be clearly resonant but still musical in all modes
- `RESO=0.75` should produce strong peak emphasis in most modes
- `RESO=0.92` should be on the edge of self-oscillation or already oscillating depending on mode

Decision rule:

- If resonance stays weak until the last 10% of the knob, make the curve more aggressive earlier.
- If self-oscillation arrives too early, flatten the upper end.

#### `signedWeight()`

Current code:

- exponential skew with factor `0.7`

Target behavior:

- `BALANCE=-0.75` strongly favors the lower peak
- `BALANCE=0.75` strongly favors the upper peak
- `BALANCE=0.00` is stable, centered, and not quieter than the skewed states

Decision rule:

- If balance feels too subtle, increase skew slightly.
- If one side collapses too easily, reduce skew.

#### `combineModeResponse()`

This is the main mode-tuning block.

Target behavior by mode:

- `mode 0` `Low + Low`: strongest lowpass family, steepest top-end rejection
- `mode 1` `Low + Band`: lowpass core with an obvious upper bump/formant
- `mode 2` `Notch + Low`: lowpass with an audible dip below the main corner
- `mode 3` `Notch + Notch`: dual-notch / phaser-like, especially obvious on noise
- `mode 4` `Low + High`: band-reject style, lows and highs survive, middle hollows out
- `mode 5` `Band + Band`: the most obviously dual-formant mode
- `mode 6` `High + High`: resonant passband bounded by highpass and lowpass edges
- `mode 7` `High + Notch`: highpass family with a notch above the corner
- `mode 8` `Band + High`: highpass family with an added lower formant
- `mode 9` `High + Low`: strongest highpass family, steepest low-end rejection

Level target:

- at `LEVEL=0.50`, `RESO=0.45`, `SPAN=0.30`, no mode should feel drastically quieter or louder than adjacent modes
- exact equal loudness is not required
- obvious level accidents are not acceptable

## Pass 2: Input Drive Behavior

Touch:

- `levelDriveGain()`
- only small trims in `combineModeResponse()` if needed

### Targets

- `LEVEL=0.20` is mostly clean
- `LEVEL=0.50` is audibly fuller, but mode identity is still clear
- `LEVEL=0.85` produces strong nonlinear color without turning every mode into the same clipped result

### Decision rule

- If the top quarter of the knob becomes unusably abrupt, soften the cubic growth in `levelDriveGain()`.
- If the first half of the knob does too little, raise the linear component slightly.

## Pass 3: Circuit Identity

Now tune the three nonlinear circuit models relative to the tuned `SVF`.

Touch:

- `DfmCore::process()`
- `Ms2Core::process()`
- `PrdCore::process()`
- `makePreviewModel()`

Do not rework the base mode coefficients unless a circuit-specific problem exposes a real issue there.

### Circuit Targets

#### `SVF`

- clean baseline
- strongest preview/audio agreement
- smoothest modulation behavior

#### `DFM`

- most aggressive and unstable-feeling
- strongest squelch and self-mod impression
- should sound dirtier than `SVF` at the same settings

Relevant code region:

- feedback amount in `DfmCore::process()`
- drive scaling in `DfmCore::process()`

#### `MS2`

- softer and warmer than `SVF`
- resonance should compress sooner under drive
- should not keep the sharpest peak at high input level

Relevant code region:

- resonance amount in `Ms2Core::process()`
- cascaded nonlinearity gain staging

#### `PRD`

- sharpest resonant peak
- strongest ladder-like bite
- should retain more pointed resonance under drive than `MS2`

Relevant code region:

- resonance amount in `PrdCore::process()`
- nonlinear blend and stage feedback terms

### Circuit Acceptance Checks

At `LEVEL=0.50`, `FREQ=0.35`, `SPAN=0.30`, `RESO=0.75`:

- switching circuit mode should produce immediate audible difference
- `MS2` should sound rounder than `PRD`
- `PRD` should sound peakier than `MS2`
- `DFM` should sound more unruly than both

At `LEVEL=0.85`, `RESO=0.75`:

- `MS2` should flatten/compress resonance more readily
- `PRD` should keep more edge
- `DFM` should feel the least polite

## Pass 4: Preview Alignment

Once the audio path feels right, align the preview model.

Touch only:

- `makePreviewModel()`

The preview is nominal, not exact. It only needs to remain directionally honest.

### Targets

- when a circuit sounds peakier, the preview should also read peakier
- when a circuit sounds softer or shifted, the preview should reflect that direction
- preview differences between circuits should not be so exaggerated that the UI overpromises

### Decision rule

- If a circuit sounds clearly different but the preview barely changes, increase the preview voicing difference
- If the preview diverges too far from what is heard, reduce the voicing offset rather than trying to emulate nonlinear detail

## Pass 5: TITO

Only after the previous passes are stable.

Touch:

- coupling depth logic around `couplingDepth`
- modulation routing only if clearly necessary

### Targets

- center position remains the reference behavior
- self-mod adds obvious extra animation and roughness
- cross-mod sounds different from self-mod, not just stronger
- TITO should expand the instrument, not destroy basic tuning

### Acceptance checks

At `RESO=0.75`, `SPAN=0.30`, `LEVEL=0.50`:

- self-mod should be more radical than center
- cross-mod should sound more warbly/interacting than center
- both positions should still preserve mode identity

## V/OCT and Self-Oscillation Check

This is a practical tuning gate, not a strict calibration pass.

Target:

- during self-oscillation, the module should track musically enough to use as a tone source over a few octaves

Check:

- set high resonance in several modes
- use low span, then moderate span
- test a simple stepped V/OCT sequence

Decision rule:

- if tracking is unusably inconsistent in the clean `SVF` case, revisit the resonance/cutoff relationship before touching nonlinear circuits

## Concrete Edit Sequence

Make edits in this order:

1. `shapedSpan()`
2. `resoToDamping()`
3. `signedWeight()`
4. `combineModeResponse()`
5. `levelDriveGain()`
6. `DfmCore::process()`
7. `Ms2Core::process()`
8. `PrdCore::process()`
9. `makePreviewModel()`
10. TITO coupling depth and routing if still needed

Do not jump ahead unless an earlier stage is already stable.

## What Counts As Done

This tuning pass is complete when all of the following are true:

- the 10 modes are easy to distinguish without staring at the display
- the 4 circuit modes are easy to distinguish with the same settings
- resonance rises predictably and self-oscillates near the top of the knob
- span and balance produce useful motion across their full ranges
- drive adds character instead of flattening all differences
- the preview stays directionally honest

## Follow-Up Deliverables

After the tuning pass lands:

- add a short summary of tuning behavior to [doc/Bifurx.md](/mnt/c/msys64/home/Plasm/Leviathan/doc/Bifurx.md)
- optionally add small helper tests for taper functions if we want to lock in control mapping behavior
