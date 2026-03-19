# Codex Direction: TemporalDeck Scratch Architecture

This note captures my assessment after reviewing `gemini_review.md` against the current `TemporalDeck.cpp` implementation.

## Summary

Gemini is directionally correct about the core problem:

- normal transport is fundamentally velocity-integrated
- scratch is fundamentally lag/position-relative against the moving write head
- that split is a real reason why `Reverse` sounds cleaner than a slow reverse-like manual scratch

However, the proposed fix in `gemini_review.md` is too absolute. I do **not** think TemporalDeck should switch to a pure velocity-only model for everything.

## What Gemini Got Right

### 1. There is a real architectural split

Normal transport updates playback from integrated speed:

- `readHead = prevReadHead + speed` style behavior in the normal transport branch

Scratch/path-follow updates playback from lag relative to `newestPos`:

- `readHead = newestPos - scratchLagSamples`

This means scratch has to continually fight a moving reference frame. That is the root cause of a lot of the "grind", "rubberiness", and "fighting the user" behavior.

### 2. The scratch path has accumulated many compensating heuristics

The current code now contains several corrective layers:

- manual target smoothing
- target prediction between UI events
- inertia blending
- flip transient injection
- de-click crossfade
- slow reverse DC/rumble trim
- manual anti-grind smoothing
- wheel-specific smoothing/snap logic

Each of these was introduced for a reason, but taken together they are a sign that the core scratch model is still not naturally producing the target behavior.

### 3. UI-to-audio bridging is still heuristic

Mouse and wheel gestures are still translated into audio-rate motion through estimates and hold windows, not a true timestamped kinematic model. That is likely still a major source of mismatch between hand motion and audible result.

## Where I Disagree With Gemini

### 1. "Use velocity for everything" is not the right direct fix

TemporalDeck is not just a platter toy. It has features whose semantics are naturally expressed in lag/position space:

- slip return
- position CV
- scratch gate + position behavior
- oldest-edge clamping
- accessible buffer-depth logic

A pure speed-only transport model would make those features harder to reason about and likely introduce new complexity elsewhere.

### 2. The current code is already beyond the exact failure mode Gemini described

The review describes stationary scratch as if the engine still simply falls back to speed `1.0`.

That is not the current state of the code. We already have:

- explicit stationary hold
- predicted lag drift between sparse drag events
- inertia in manual scratch lag movement
- transient/de-click/DC cleanup layers

So the diagnosis is still useful at a high level, but some of the specific observations are stale relative to the current implementation.

## Recommended Direction

### Keep lag as the authoritative state

The module's feature set is still best expressed in lag space.

I would keep:

- `scratchLagSamples`
- `scratchLagTargetSamples`
- buffer-relative semantics for slip / position / edge clamping

### Replace direct heuristic lag chasing with a cleaner integrated platter-motion model

Instead of saying:

- "here is the new lag target, chase it with several smoothing heuristics"

the engine should evolve something closer to:

- a platter velocity state
- a platter acceleration / drive term from user input
- damping / drag
- optional spring-to-target correction only where absolute positioning is required

That would be a hybrid model:

- lag remains the authoritative playback position state
- but lag is advanced by integrated motion rather than a stack of ad hoc lag-fixup layers

This is the direction I think is most technically coherent.

## Practical Plan

### Short term

Do not add more scratch heuristics until the core model is simplified.

Specifically:

- freeze current behavior
- stop piling on more one-off de-click/transient tweaks unless they are clearly temporary

### Medium term

Prototype a scratch core with:

- integrated platter velocity
- damping / friction
- manual gesture mapped to target velocity rather than directly to lag deltas
- lag correction spring only for:
  - slip return
  - absolute position CV
  - maybe wheel target settle

### Validation

Judge the prototype against these criteria:

- slow reverse mouse glide should approach the smoothness of `Reverse`
- pause during drag should hold naturally without mode-fighting
- wheel scratch should stop sounding synthetic
- code should remove, not add, special-case cleanup layers

## Bottom Line

Gemini correctly identified the main architectural fault line.

The correct response is **not** "switch everything to speed-only transport".

The better direction is:

- keep lag-based semantics for module features
- move scratch behavior toward an integrated motion model
- use fewer corrective heuristics
- make the code smaller and more physically coherent, not larger

