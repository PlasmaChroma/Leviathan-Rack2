# Codex Rework Plan: TemporalDeck Scratch Core

This document describes the large revision I would make to `TemporalDeck` if we decided to do a real scratch-architecture cleanup now instead of continuing to tune the current system incrementally.

## Goal

Make scratching feel and sound more physically coherent while reducing the amount of special-case corrective DSP.

The practical target is:

- slow reverse mouse glide should approach the smoothness of the `Reverse` button
- pause during drag should hold naturally without fighting the user
- wheel scratch should sound less synthetic
- code complexity should go down, not up

## Non-Goals

This rework would **not** try to be a perfect turntable simulator.

It would not include:

- full motor torque modeling
- detailed stylus/groove contact physics
- slip-mat clutch simulation
- expensive oversampling or heavy DSP blocks

The target is a lightweight, believable interaction model that fits the module and CPU budget.

## Current Problem

The current engine mixes two different ideas:

- transport is speed/integration based
- scratch is lag-relative to the moving write head

That mismatch forces the scratch path to use multiple repair layers:

- gesture smoothing
- prediction between UI events
- inertia blending
- transients
- de-clicking
- DC/rumble trim
- manual anti-grind smoothing
- wheel-specific chase logic

Those layers were added for good reasons, but together they indicate the scratch core is still compensating for the wrong primitive.

## Proposed Core Model

### Keep lag as the authoritative playback state

I would keep:

- `scratchLagSamples`
- `scratchLagTargetSamples`
- buffer-relative semantics for:
  - slip
  - position CV
  - scratch gate + position
  - edge clamping

This is important because `TemporalDeck` is fundamentally a delay/buffer-position device, not just a playback transport.

### Replace direct lag-chasing with integrated platter motion

The core change would be:

- user interaction produces a target platter velocity
- platter velocity is integrated over time with damping
- lag is advanced from that integrated velocity
- absolute targets use a correction term, not a hard repositioning loop

In other words:

- lag remains the state the module cares about
- but lag is moved by a small motion model rather than a stack of heuristic lag fixes

## Reworked State

I would introduce a cleaner motion state block:

- `platterLag`
- `platterVelocity`
- `platterTargetVelocity`
- `platterCorrectionForce`
- `platterHeld`

And reduce the meaning of the old scratch variables:

- `scratchLagTargetSamples` becomes a correction target, not the main movement driver
- `platterGestureVelocity` becomes a UI input signal only

## Reworked Signal Flow

### 1. Normal transport

- base transport speed from knob/CV/reverse
- convert to platter velocity in lag space
- integrate lag from velocity

### 2. Manual scratch

- mouse gesture maps to `targetPlatterVelocity`
- velocity follows target with damping and acceleration limits
- lag integrates from velocity
- when user stops moving but still holds:
  - velocity decays toward zero
  - held state can pin to zero if needed

### 3. Wheel scratch

- wheel impulse adds a temporary target velocity burst
- burst decays over a short window
- no separate wheel-specific lag chase logic

### 4. Position CV / external scratch

- position CV generates a desired lag target
- engine adds a spring/correction force toward that lag
- no direct `readHead = newestPos - target` stepping except where explicitly required

### 5. Slip return

- slip becomes a correction-to-zero process
- use correction force + final snap threshold
- avoid separate multi-phase logic unless still needed after simplification

## DSP Simplification

### Keep

- high-quality interpolation path
- cartridge voicing
- minimal de-click protection
- edge clamping

### Likely remove or greatly reduce

- synthetic flip transient layer
- special slow-reverse DC hack as a first-line fix
- multiple overlapping smoothing passes
- wheel-specific special cases that only exist to hide target-chase artifacts

### Replace with one minimal safety layer

After the motion model is cleaner, I would keep only one small artifact guard:

- a tiny crossfade/de-click blend when read delta is below a threshold and direction changes suddenly

That should be a safety net, not the main sound-shaping mechanism.

## UI-to-Audio Bridge

The UI bridge should also be revised.

### Current issue

Mouse updates are sparse and the engine infers audio-rate behavior from:

- gesture revisions
- a fixed "motion fresh" window
- heuristic velocity values

### Rework

Use:

- explicit timestamps or delta-time between UI events
- computed angular velocity from actual event spacing
- optional short linear ramp to the new target velocity

That gives the DSP a cleaner input signal and reduces the need for predictive hacks.

## Migration Plan

I would do this in phases, not as one huge blind rewrite.

### Phase 1: Build a new scratch core behind a local switch

Add a local implementation branch in `TemporalDeck.cpp`:

- old scratch core
- new scratch core

No UI changes yet.

### Phase 2: Implement manual scratch on the new core

Only move manual mouse scratch first.

Acceptance test:

- slow reverse glide
- hold still during drag
- fast direction flips

### Phase 3: Move wheel scratch to the same core

Wheel should become:

- velocity impulse input
- shared motion engine

Acceptance test:

- no sci-fi laser character
- easier return to NOW
- less directional asymmetry

### Phase 4: Reattach slip and position CV

Use correction/spring logic rather than direct target stepping where possible.

Acceptance test:

- slip still returns musically
- position CV still behaves predictably
- reverse edge logic still works

### Phase 5: Remove old heuristics

After the new path is stable, remove:

- dead code
- now-unnecessary de-click/transient branches
- duplicated smoothing logic

This is where the real payoff happens.

## Acceptance Criteria

I would consider the rework successful if:

1. Slow manual reverse sounds materially closer to `Reverse` than it does today.
2. Scratch feel is more direct with fewer edge artifacts.
3. Wheel scratch no longer requires a large number of bespoke fixes.
4. The scratch section of `TemporalDeck.cpp` is smaller and easier to reason about.
5. CPU cost stays roughly in the same range or improves.

## Risks

### 1. Regressing slip and position CV behavior

Those features currently rely on lag semantics. The rework must preserve that.

### 2. Losing the "good" parts of current scratch feel

Some current heuristics may be masking real problems but may also be contributing a character the user likes.

### 3. Scope creep

This can turn into a fake "physics simulator" project if not controlled.

The bar should remain:

- better sound
- better feel
- less code

## What I Would Actually Do First

If I were starting this rework now, my first practical step would be:

- isolate the current scratch block into one function
- implement a second scratch core beside it
- route only manual mouse scratch through the new core first

That gives an A/B point without destabilizing the entire module at once.

## Bottom Line

The right big revision is **not** "keep adding more cleanup layers."

The right revision is:

- keep lag-based module semantics
- move scratch to an integrated motion model
- unify manual and wheel behavior under one motion core
- aggressively delete heuristic repair code once the new path proves itself

