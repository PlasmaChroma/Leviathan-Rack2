# Scratch3 Design

## Goal

Use a two-phase path to converge toward one final scratch model for TemporalDeck:

1. Fix the shared input layer so the current models are being judged on good gesture data.
2. Build one experimental scratch model on top of that corrected input path if the current models still fall short.

This explicitly defers any `sinc` or more expensive resampling work until gesture mapping, stationary hold, reversal articulation, and frame-rate invariance are stable.

## Product Direction

The long-term goal is one scratch model in the final product.

Current role of each model:
- `Legacy`: regression baseline and safety net
- `Hybrid`: current principled architecture candidate
- `Scratch3` / `Experimental`: temporary proving ground for a cleaner final architecture

This document is not proposing a permanent third user-facing mode. It is proposing a controlled experiment to decide what survives.

## Why This Order

Both current models depend on the same manual platter gesture feed.

Right now, that feed has two likely structural weaknesses:
- manual scratch velocity is derived from per-frame mouse motion rather than explicit elapsed time
- gesture freshness uses a fixed 20 ms bridge window

That means both `Legacy` and `Hybrid` are being judged through an input layer that may already be distorting feel.

The correct order is:
- fix shared input behavior first
- re-evaluate the existing models
- only then introduce a new experimental engine model if needed

## Phase 1: Input Layer Corrections

### Objectives

- Make mouse scratch velocity frame-rate invariant
- Remove the fixed 20 ms freshness assumption
- Preserve hard stationary hold behavior
- Improve usability with optional cursor lock
- Add temporary observability so feel work can be judged with real data

### Scope

Phase 1 should avoid a major rewrite of the scratch engine.

It should only change:
- platter widget gesture timing and velocity derivation
- gesture freshness semantics
- light module plumbing needed to pass improved gesture data
- optional cursor lock
- temporary debugging instrumentation

It should not introduce:
- a new resampler
- a new interpolation tier
- a large rewrite of cartridge or transport behavior

### Phase 1.1: Explicit Drag Timing

Add timing state to `TemporalDeckPlatterWidget`.

Suggested fields:

```cpp
bool dragHasTiming = false;
double lastMoveTimeSec = 0.0;
float filteredGestureVelocity = 0.f;
float recentGestureDtSec = 1.f / 60.f;
```

Behavior:
- On `onDragStart`, initialize timing state.
- On each `onDragMove` or inside `updateScratchFromLocal`, sample `rack::system::getTime()`.
- Compute `dtSec` as the difference from the prior move timestamp.
- Clamp `dtSec` into a sane range to avoid first-event spikes and OS stall spikes.

Suggested clamp:

```cpp
dtSec = clamp(dtSec, 1.0 / 240.0, 1.0 / 20.0);
```

This does not change lag mapping. It only corrects time-dependent gesture estimation.

### Phase 1.2: Frame-Rate-Invariant Gesture Velocity

The platter drag should continue to derive lag from tangential mouse motion projected onto the platter radius. That part is conceptually right.

The change is only in velocity estimation.

Current conceptual issue:
- lag delta is derived from geometric motion
- gesture velocity is still derived from per-frame mouse delta

New rule:
- derive gesture velocity from actual lag delta divided by actual `dtSec`
- keep units explicit: `samples/sec`

Suggested shape:

```cpp
float lagDeltaSamples = deltaAngle * samplesPerRadian;
float measuredVelocity = lagDeltaSamples / dtSec;
```

Optional light smoothing:

```cpp
float alpha = 1.f - std::exp(-2.f * float(M_PI) * 30.f * dtSec);
filteredGestureVelocity += (measuredVelocity - filteredGestureVelocity) * alpha;
```

The smoothing should be light enough to reduce hand jitter without creating rubber-band feel.

### Phase 1.3: Replace Fixed 20 ms Freshness

Current behavior uses a fixed motion freshness window.

That should be replaced with one of two approaches.

#### Option A: Adaptive freshness window

Lower-risk first step.

Compute motion freshness from observed UI dt:

```cpp
int motionFreshSamples = std::round(sampleRate * dtSec * 1.25f);
motionFreshSamples = clamp(motionFreshSamples, 1, int(sampleRate * 0.03f));
```

Advantages:
- small code change
- preserves the current engine contract
- easy to compare against existing behavior

#### Option B: Age-aware engine decay

Preferred long-term shape.

Instead of treating motion as a fixed alive/dead window, pass either:
- gesture age
- or a timestamp/revision pair

Then let the engine decay motion toward zero when updates stop arriving.

Advantages:
- better physical feel
- avoids a hard constant-velocity continuation tail
- more flexible for later model design

Recommended path:
- implement Option A first in Phase 1
- evaluate
- move toward Option B if still needed

### Phase 1.4: Preserve Hard Stationary Hold

This is a non-negotiable acceptance criterion.

When the platter is touched and the hand stops moving:
- the deck must stop moving quickly and predictably
- the model must not continue drifting because the write head is still advancing

Any Phase 1 change must preserve:
- click-and-hold freeze
- very slow reverse drag without unwanted clockwise creep

This is more important than adding extra smoothness.

### Phase 1.5: Optional Cursor Lock

Add an optional platter-drag cursor lock.

Behavior:
- on drag start, if Rack allows cursor lock and the user enabled the module option, lock the cursor
- on drag end or abort, unlock the cursor

Requirements:
- respect Rack global cursor-lock policy
- expose as a context-menu toggle
- default to conservative behavior

This is a usability improvement and should remain independent of DSP changes.

### Phase 1.6: Temporary Observability

Add temporary debug visibility for scratch tuning.

Useful values:
- `dtSec`
- measured gesture velocity
- filtered gesture velocity
- motion freshness window or age
- lag target
- actual lag
- read delta

Possible forms:
- temporary debug text in the module UI
- logging behind a compile-time flag
- a small internal debug mode

Without this, feel tuning is too subjective and regressions are hard to pin down.

### Phase 1 Acceptance Criteria

Phase 1 is complete when the following are true:

1. Scratch feel is perceptually similar across different frame rates and monitor refresh conditions.
2. Click-and-hold does not drift.
3. Slow reverse drag does not creep forward.
4. Fast back-and-forth strokes are at least as good as before, preferably cleaner.
5. `Hybrid` can be judged fairly without obvious input-layer bias.

### Phase 1 Evaluation Matrix

After Phase 1, compare `Legacy` and `Hybrid` on:
- stationary hold
- slow reverse drag
- baby scratch
- scribble-like short reversals
- slow forward glide toward NOW
- behavior under reduced Rack frame rate / high UI load

Decision after evaluation:
- if `Hybrid` becomes clearly good enough, continue evolving `Hybrid`
- if both models still require too many compensations, proceed to Phase 2

## Phase 2: Scratch3 Experimental Model

### Purpose

`Scratch3` exists to test a cleaner final architecture using the corrected Phase 1 input feed.

It is not meant to become a third permanent user-facing mode.

It should be implemented as:
- internal experimental mode
- dev/test selectable
- easy to remove if it loses

### Design Goals

- one coherent motion model in lag-space
- explicit handling of measurement freshness
- strong stationary hold
- crisp reversal behavior
- less patch stacking than `Legacy`
- lower conceptual complexity than the current split between `Legacy` and `Hybrid`

### Core State

Suggested engine-side state:

```cpp
float lagEst = 0.f;
float lagVel = 0.f;           // samples/sec
float lagTargetMeas = 0.f;
float lagVelMeas = 0.f;       // samples/sec
float gestureAgeSec = 0.f;
uint32_t lastGestureRevision = 0;
```

Optional helper state:

```cpp
bool gestureFresh = false;
float reversalEnv = 0.f;
int lastVelSign = 0;
```

### Core Model

Operate in lag-space.

Inputs:
- measured lag target from UI
- measured gesture velocity from UI
- gesture freshness / age
- touch state
- deck lag limit

Outputs:
- stable lag estimate
- stable lag velocity
- read head position

Conceptual update per sample:

1. Predict

```cpp
lagEst += lagVel * dt;
```

2. If a fresh measurement arrived, correct toward it.

Alpha-beta style correction sketch:

```cpp
float residual = lagTargetMeas - lagEst;
lagEst += alpha * residual;
lagVel += (beta / dt) * residual;
```

3. If gesture is touched but stale, decay velocity quickly toward zero.

```cpp
lagVel *= decay;
```

4. If touched and velocity is under deadband and age exceeds about one frame, force hold.

```cpp
if (touched && gestureAgeSec > holdThresholdSec && std::fabs(lagVel) < holdVelDeadband) {
    lagVel = 0.f;
}
```

5. Clamp lag estimate into valid range.

```cpp
lagEst = clamp(lagEst, 0.f, lagLimit);
```

6. Derive `readHead = newestPos - lagEst`.

### Why This Is Different From Current Hybrid

`Hybrid` already has a velocity-first structure, but it is still shaped around the current gesture feed and existing corrective logic.

`Scratch3` should be more explicit about three things:
- gesture age
- the distinction between fresh measurement and stale continuation
- stationary hold as a first-class state rather than an afterthought

The goal is not “more smoothing.” The goal is a simpler estimator with fewer compensating exceptions.

### Hold Behavior

Hold behavior must be treated as a direct design feature.

Rules:
- touch + no fresh motion + low velocity = freeze
- no hidden write-head chase while the hand is stationary
- hold should happen quickly enough to feel intentional

This is a stronger requirement than “don’t drift too much.”

### Reversal Behavior

Reversal feel should be solved in this order:

1. Get a clean velocity zero crossing in the motion model.
2. Confirm that lag transitions remain controlled through reversal.
3. Only then decide whether extra audio articulation is needed.

If an extra reversal articulation layer is still needed:
- keep it short
- make it conditional
- do not use it to hide deeper motion-model problems

### Cleanup Layer

`Scratch3` should start with a minimal cleanup layer.

Allowed at first:
- very light de-click continuity blend
- very light ultra-slow smoothing
- optional DC trimming if needed in reverse motion

Not allowed initially:
- recreating the full `Legacy` patch stack by default

The experiment needs to prove it can get most of the way there with cleaner motion behavior, not by immediately accreting exceptions.

### Phase 2 Acceptance Criteria

`Scratch3` is worth keeping only if it clearly beats `Hybrid` on the things that matter most:

1. Stationary hold
2. Slow reverse control
3. Crisp short reversals
4. Consistency across frame-rate conditions
5. Reduced need for compensating cleanup logic

If it does not beat `Hybrid`, it should be removed.

## Comparison Plan

Once Phase 2 exists, compare:
- `Legacy`
- `Hybrid`
- `Scratch3`

Judge using the same gesture families:
- click-and-hold
- slow reverse drag
- baby scratch
- scribble-like micro-reversals
- forward push toward NOW
- fast stop-reverse-stop patterns

Also compare:
- sensitivity to reduced UI frame rate
- CPU cost
- quantity of cleanup logic needed to sound acceptable

## Decision Rules

After testing:

1. If Phase 1 makes `Hybrid` clearly good enough:
- keep `Hybrid`
- retire `Legacy` after confidence is high
- never promote `Scratch3`

2. If `Scratch3` clearly beats `Hybrid` with a cleaner internal design:
- promote `Scratch3`
- retire `Hybrid`
- keep `Legacy` only until transition confidence is high

3. Do not keep all three long term.

The final product should converge to one scratch model.

## Explicit Non-Goals For Now

These are intentionally deferred:
- `sinc` scratch resampling
- more interpolation tiers
- major cartridge-character redesign tied to scratch feel
- unrelated UI refactors

Those can be revisited later, but only after gesture mapping and motion feel are stable.

## Recommended Implementation Sequence

1. Add explicit drag timing in the platter widget.
2. Replace frame-dependent gesture velocity with lag-delta-over-dt velocity.
3. Replace fixed freshness with adaptive freshness.
4. Add optional cursor lock.
5. Add temporary debug observability.
6. Re-evaluate `Legacy` and `Hybrid`.
7. If still justified, add `Scratch3` experimental mode.
8. Tune hold, reversal, and short-stroke behavior.
9. Compare all models and delete losers.
10. Only then revisit higher-quality scratch resampling.

## Summary

This plan creates a disciplined path forward:
- correct the input layer first
- evaluate existing models fairly
- introduce one temporary experimental model only if justified
- converge back down to one final scratch model

That is the right way to reduce risk without getting trapped in indefinite model sprawl.
