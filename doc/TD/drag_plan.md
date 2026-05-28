# TemporalDeck Drag Model Fix Plan

This plan is for the current codebase state as of 2026-05-26, not for an older snapshot. Some claims in `TemporalDeck_DragModel_Review.md` are directionally right but a few details are stale. Follow this plan instead of applying that review literally.

## Goal

Fix the remaining Live-mode manual drag feel problems without regressing the improvements already made to:

- TD.Scope stationary hold snap behavior
- TD.Scope stationary hold resuming playback while mouse is still held
- deep-buffer resistance reduction experiments already kept in the engine

The main target is:

- platter and TD.Scope forward/backward drag should feel more symmetric
- near-NOW Live drag should feel responsive
- deep-buffer Live drag should not get hidden forward pull
- platter and scope should converge toward the same engine-owned feel

## Non-goals

Do not do these in the first pass:

- do not rewrite the whole scratch model
- do not remove platter-side rebase logic wholesale
- do not remove TD.Scope anchor logic wholesale
- do not change Sample mode behavior unless a test proves it is necessary
- do not change serialization, params, or released-module IDs

## Current confirmed problems

These are the issues that are still true in the current tree and should drive the work.

### 1. `writeHeadCompensationActive` is effectively dead for touch drag

File:

- `src/TemporalDeckEngine.hpp`

Relevant code:

- `liveManualFreezeLikeBehavior = !sampleModeActive && manualTouchScratch`
- `freezeForScratchModel = freezeState || sampleManualFreezeBehavior || liveManualFreezeLikeBehavior`
- `shouldApplyLiveManualWriteHeadCompensation(...)`

Problem:

- touch drag sets `manualTouchScratch = true`
- that makes `liveManualFreezeLikeBehavior = true`
- that makes `freezeForScratchModel = true`
- `shouldApplyLiveManualWriteHeadCompensation()` currently passes `freezeForScratchModel` into `platter_interaction::shouldApplyWriteHeadCompensation(...)`
- that helper requires non-freeze
- result: the near-NOW write-head compensation path is effectively unreachable for manual touch drags

Consequence:

- comments in the engine suggest there is a near-NOW/manual-touch compensation split
- in reality, touch drags do not receive that split through this flag

### 2. `deepLiveManualMotion` is not actually "deep"

File:

- `src/TemporalDeckEngine.hpp`

Relevant code:

```cpp
bool deepLiveManualMotion =
  !sampleModeActive && manualTouchScratch && manualMotionActive && !writeHeadCompensationActive;
float correctionScale = deepLiveManualMotion ? 0.33f : 0.68f;
```

Problem:

- because `writeHeadCompensationActive` is effectively false for touch drags, `deepLiveManualMotion` becomes true for nearly all active Live manual touch motion
- therefore `correctionScale = 0.33f` is applied broadly, including near NOW

Consequence:

- near-NOW touch drag is under-corrected
- forward drag can feel floaty or disconnected
- the intended "high correction near NOW / lower correction deep in buffer" split does not really exist

### 3. Expander velocity derivation still has a fresh-packet timing problem

File:

- `src/TemporalDeck.cpp`

Current state:

- older claim that `framesSinceUpdate` is reset before read is stale
- current code reads `framesSinceUpdate` before reset

But there is still a real issue:

- on the first audio frame of a new expander request, `expanderLagDragFramesSinceUpdate` can be `0`
- code falls back to `frames = max(1, ...)`
- so `dtSec` can collapse to one audio sample
- that inflates `derivedVelocity`
- later clamp and advisory blending reduce the damage, but they do not remove the root cause

Consequence:

- TD.Scope packet boundaries can still create exaggerated velocity estimates
- this can bias perceived motion, especially during fast forward gestures

### 4. Touch drags do not get a near-NOW snap backstop

File:

- `src/TemporalDeckEngine.hpp`

Relevant code:

```cpp
bool allowNowSnap = !manualTouchScratch;
```

Problem:

- touch drags never enable the existing near-NOW snap behavior

Consequence:

- even when target and actual lag are both very close to NOW, touch can hover instead of settling cleanly

### 5. Freeze-only "feel direct" code exists, but non-freeze Live touch does not get an equivalent policy

File:

- `src/TemporalDeckEngine.hpp`

Relevant code:

- forward assist / snap-to-target blocks gated by `freezeState`

Problem:

- Freeze mode gets a very direct forward behavior
- non-freeze Live touch does not get a comparable near-NOW policy

Consequence:

- user can feel a strong difference between Freeze-enabled and normal Live drag

## Constraints from recent work

Keep these recent fixes intact unless there is strong evidence they are wrong.

### Keep

1. TD.Scope stationary hold handoff in `src/TemporalDeck.cpp`
   - current host-side direct-hold stepping removed the visible snap at drag stop
   - current host-side direct-hold stepping also prevents "starts playing again while still held"

2. Scope velocity is advisory, not fully authoritative
   - current blending in `src/TemporalDeck.cpp` should stay conceptually intact
   - it is part of keeping TD.Scope thin

3. Drag autoscale smoothing in TD.Scope
   - unrelated to the engine drag bug

### Reverted already, do not reintroduce casually

1. hard 1000 ms gating of platter UI rebase
   - this made backward platter drag worse

2. naive scope-side 1000 ms compensation gating
   - this also made feel worse

## Implementation strategy

Do this in small, testable steps. Do not batch everything into one huge change.

---

## Step 1: Make "deep vs near-NOW" use actual lag depth

### Objective

Restore the intended split:

- near NOW: higher correction
- deep in buffer: reduced correction

without depending on `writeHeadCompensationActive`.

### Files

- `src/TemporalDeckEngine.hpp`
- tests in `tests/temporaldeck_engine_spec.cpp`

### Required code change

Introduce an explicit helper for manual touch depth. Example shape:

```cpp
static bool isDeepLiveManualTouchMotion(bool sampleModeActive,
                                        bool manualTouchScratch,
                                        bool manualMotionActive,
                                        double scratchLagSamples,
                                        float sampleRate) {
  return !sampleModeActive &&
         manualTouchScratch &&
         manualMotionActive &&
         scratchLagSamples > double(std::max(sampleRate, 1.f) * kLiveManualWriteHeadCompensationWindowSec);
}
```

Then replace:

```cpp
bool deepLiveManualMotion =
  !sampleModeActive && manualTouchScratch && manualMotionActive && !writeHeadCompensationActive;
```

with the new depth-based helper.

### Important

Do not change `0.33f` and `0.68f` yet in this step. First restore the intended branching, then listen/test.

### Tests to add/update

In `tests/temporaldeck_engine_spec.cpp`:

1. add a policy-level test for the helper
   - near NOW returns false
   - at 1 second edge returns false or true depending on chosen strictness; be explicit and match helper semantics
   - deeper than 1 second returns true
   - sample mode returns false
   - idle/no motion returns false

2. add a regression-oriented touch test that verifies correction-scale branch selection indirectly
   - use near lag and deep lag
   - run a few manual touch frames
   - compare gap closure amount or lag error reduction
   - near-NOW should converge more aggressively than deep

### Expected result

- near-NOW manual touch should immediately feel more responsive
- deep-buffer manual touch should keep the lower-correction behavior

---

## Step 2: Fix expander derived-velocity timing on fresh requests

### Objective

Make TD.Scope packet-derived velocity use a realistic request interval instead of one-sample fallback on fresh packets.

### Files

- `src/TemporalDeck.cpp`
- maybe small tests in `tests/temporaldeck_virtual_integration_spec.cpp` or a new focused host/expander test if practical

### Preferred fix

Use real wall-time or accumulated audio-time between requests, not "frames since update after branch entry with min 1".

Two acceptable approaches:

#### Option A: store wall-clock request timestamp

Add to `TemporalDeck::Impl`:

- `double expanderLagDragLastRequestTimeSec = 0.0;`
- `bool expanderLagDragHasRequestTime = false;`

On each new request:

- compute `dtSec` from `system::getTime()` minus last request time
- clamp into a sane range similar to the platter UI drag path
- then update stored timestamp

This is the best option because it matches actual expander update cadence.

#### Option B: store previous inter-request frame count

Add to `TemporalDeck::Impl`:

- `int expanderLagDragLastIntervalFrames = 0;`

On each new request:

- use previous accumulated `expanderLagDragFramesSinceUpdate` as the interval
- only after capturing it, move it into `lastIntervalFrames` and reset the live counter

This is weaker than wall time but still better than one-sample fallback.

### Important

Do not remove our current "scope velocity is advisory" blending while doing this.

Keep:

```cpp
velocitySamples = derivedVelocity;
...
if (!derivedHasDirection || directionDisagrees) {
  velocitySamples = lagDragRequestVelocity;
} else {
  velocitySamples = 0.75f * derivedVelocity + 0.25f * lagDragRequestVelocity;
}
```

unless testing proves it is wrong.

### Tests to add/update

Add a focused test for packet-boundary velocity derivation if possible. At minimum:

1. simulate two TD.Scope-like updates separated by a realistic interval
2. verify derived velocity is not clamped from an obviously absurd one-sample `dt`
3. verify sign remains correct

If a direct host-level unit test is awkward, add a narrowly scoped helper function and test that helper instead.

### Expected result

- less spiky TD.Scope forward motion on fresh packet boundaries
- reduced mismatch between scope perceived motion and actual lag target convergence

---

## Step 3: Revisit near-NOW touch settle behavior

### Objective

Let forward touch drag settle cleanly at NOW without turning touch into a wheel-style snap path.

### Files

- `src/TemporalDeckEngine.hpp`
- tests in `tests/temporaldeck_engine_spec.cpp`

### Preferred change

Do not simply set:

```cpp
allowNowSnap = true;
```

for all touch drags.

Instead, make it conditional and conservative. Example shape:

```cpp
bool allowNowSnap =
  !manualTouchScratch ||
  (!sampleModeActive &&
   manualTouchScratch &&
   gestureDirection > 0.f &&
   scratchLagTargetSamples <= nowSnapThresholdSamples);
```

Possible acceptable variants:

- gate on `platterGestureVelocity > 0.f`
- gate on `gestureDirection > 0.f`
- require both target lag and current lag to be near NOW

### Important

Backward touch motion must not get a hidden snap or anti-motion clamp from this.

### Tests to add/update

1. near-NOW forward touch should settle to exact NOW
2. backward touch should not snap
3. deep-buffer touch should not snap

### Expected result

- near-NOW forward drag lands cleanly
- no regression to deep-buffer forward pull

---

## Step 4: Evaluate whether the freeze-only forward-assist policy should be generalized

### Objective

Decide whether non-freeze Live touch needs a lighter version of the direct forward assist currently limited to `freezeState`.

### Files

- `src/TemporalDeckEngine.hpp`

### Important

This is an evaluation step, not an automatic code change.

The current freeze-only blocks:

- are very aggressive
- zero velocity state
- directly set `readHead`
- may be too blunt to copy into normal Live touch

### Safer variant if needed

If Step 1 plus Step 3 do not solve the feel problem, implement a lighter near-NOW forward assist:

- only for manual touch
- only in Live mode
- only when moving toward NOW
- only inside the 1-second window
- do not teleport unless lag error exceeds a clear threshold

Possible shape:

```cpp
if (!sampleModeActive &&
    manualTouchScratch &&
    manualMotionActive &&
    gestureDirection > 0.f &&
    scratchLagSamples <= double(sampleRate) &&
    scratchLagTargetSamples < (scratchLagSamples - assistThresholdSamples)) {
  // blend or clamp toward target, not full teleport
}
```

### Warning

Do not do this before Steps 1-3. It is too easy to hide the real problem with more special-case force.

---

## Step 5: Clean up stale logic only after behavior is correct

### Candidates

1. dead/manual-touch-unreachable compensation commentary and branches
2. `filteredManualLagTargetSamples` if confirmed truly unused for active drag behavior

### Important

Do not do cleanup while behavior is still under investigation. Cleanup comes after the feel is correct and tests are in place.

## Testing checklist

Run these after each meaningful step:

1. focused platter harness

```bash
g++ -std=c++17 -O2 -Wall -Wextra tests/platter_spec_main.cpp tests/platter_spec_cases.cpp tests/platter_trace_replay.cpp -o build/tests/temporaldeck_platter_spec_harness
build/tests/temporaldeck_platter_spec_harness
```

2. engine spec

```bash
make -B build/tests/temporaldeck_engine_spec
build/tests/temporaldeck_engine_spec
```

3. full fast suite

```bash
make test-fast
```

## Manual validation checklist

After code/test validation, the implementer should manually evaluate these in Rack:

1. Platter, Live, non-freeze, near NOW
   - forward drag should feel responsive
   - backward drag should not be harder than before

2. Platter, Live, non-freeze, deep buffer
   - forward drag should not show obvious extra rotation relative to equal backward mouse motion
   - backward drag should still feel usable

3. TD.Scope, Live, non-freeze
   - no visible snap at end of drag while still holding mouse
   - does not resume playback while still holding
   - forward drag near NOW settles cleanly

4. Freeze mode
   - existing direct forward feel should remain

5. Sample mode
   - no unintended flow changes

## Recommended order of commits/patches

If working in small patches, the safest order is:

1. Step 1 only
2. test
3. Step 2 only
4. test
5. Step 3 only
6. test
7. only then decide whether Step 4 is needed

## Notes for Codex-5.3

Follow these rules strictly:

1. Do not blindly trust `TemporalDeck_DragModel_Review.md`; verify against current files first.
2. Do not reintroduce the reverted platter-side 1000 ms UI rebase gate.
3. Do not remove the current TD.Scope stationary-hold direct-hold stepping.
4. Do not replace the current scope advisory-velocity blend with full trust in scope velocity.
5. Prefer adding small helpers with clear names over embedding more compound boolean logic inline.
6. Keep tests close to the touched behavior. Do not rely on manual listening alone.
