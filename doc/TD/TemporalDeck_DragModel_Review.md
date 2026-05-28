# TemporalDeck Drag Model Review: Forward Motion & Write-Head Compensation

_Full source review of drag model, write-head compensation, and expander protocol._

---

## Summary

Forward motion in manual drag (both direct UI and TD.Scope expander) feels wrong primarily because the write-head compensation architecture has silently decoupled from reality. The code structure implies compensation is actively managing near-NOW forward drag, but the condition chain that enables it is always false for any manual touch path. A cascading set of downstream decisions — correction scale, the `deepLiveManualMotion` flag, and the CONTAINMENT NOTE cancellation — are all gated on a variable that is never true for the paths they're supposed to serve. The result is that forward drag near NOW is simultaneously under-corrected and incorrectly categorized as "deep" drag, producing a sluggish, floaty feel. The expander path has a compounding problem in velocity derivation that amplifies this under-correction during fast forward gestures.

---

## Part 1: The `writeHeadCompensationActive` Dead Code Chain

### The Problem

`writeHeadCompensationActive` is defined in `TemporalDeckEngine.hpp` (line ~2461) inside the live manual scratch branch. The logic is:

```cpp
bool liveManualFreezeLikeBehavior = !sampleModeActive && manualTouchScratch;
bool freezeForScratchModel = freezeState || sampleManualFreezeBehavior || liveManualFreezeLikeBehavior;
// ...
writeHeadCompensationActive = shouldApplyLiveManualWriteHeadCompensation(
    sampleModeActive, freezeForScratchModel, ...);
```

And `shouldApplyLiveManualWriteHeadCompensation` calls `shouldApplyWriteHeadCompensation(freezeForScratchModel, ...)` which requires `!freezeState` — where `freezeState` here is `freezeForScratchModel`.

**The chain collapses:** `manualTouchScratch = true` → `liveManualFreezeLikeBehavior = true` → `freezeForScratchModel = true` → `shouldApplyWriteHeadCompensation(...) = false` → `writeHeadCompensationActive = false`.

For **every manual touch path** (both direct platter drag and expander/TD.Scope drag), `writeHeadCompensationActive` is permanently false. The entire block:

```cpp
if (writeHeadCompensationActive) {
    targetReadVelocity += sampleRate;  // NEVER EXECUTES
}
```

is dead. So is the CONTAINMENT NOTE cancellation:

```cpp
if (!sampleModeActive && !freezeForScratchModel && reverseGestureIntent && writeHeadCompensationActive) {
    targetReadVelocity -= sampleRate;  // ALSO NEVER EXECUTES
}
```

These paths only fire for **wheel scratch** (non-touch, `manualTouchScratch = false`), where `liveManualFreezeLikeBehavior = false`. The comments throughout this section describe near-NOW behavior that simply does not apply to the code that follows.

### Why This Hurts Forward Motion

The compensation exists to offset the write head advancing at `sampleRate` while the user holds or moves the record. Without it, the correction mechanism in `integrateHybridScratch` must shoulder the entire job of keeping the read head tracking the lag target while the buffer fills. This is fine architecturally — the correction can do it — but the rest of the parameter tuning assumes the compensation is either present (near NOW) or absent (deep in buffer), which leads to the next issue.

---

## Part 2: `deepLiveManualMotion` Is Always True for Touch

### The Problem

Directly below the compensation block:

```cpp
bool deepLiveManualMotion =
    !sampleModeActive && manualTouchScratch && manualMotionActive && !writeHeadCompensationActive;
float correctionScale = deepLiveManualMotion ? 0.33f : 0.68f;
```

Because `writeHeadCompensationActive` is always false for touch, this simplifies to:

```cpp
bool deepLiveManualMotion = !sampleModeActive && manualTouchScratch && manualMotionActive;
```

The "deep" qualifier is gone. For **all live manual drag** — including drag at lag = 0 (at NOW), near-NOW drag within a few milliseconds, and slow deliberate forward motion — the correction scale is capped at **0.33** instead of **0.68**. The intent was to reduce correction deep in the buffer (where full correction creates an unwanted forward pull toward stale targets). The effect is that near-NOW forward motion has only ~half the correction responsiveness that was designed.

The 0.33 scale is fed into `integrateHybridScratch` as `correctionScale`:

```cpp
float correctionVelocity = clamp(-lagError * (kHybridScratchCorrectionHz * correctionScale), ...);
// Effective Hz: 70 * 0.33 = 23 Hz (vs intended 70 * 0.68 = 47.6 Hz near NOW)
```

For a forward drag where the write head is advancing and `currentLag > targetLag`, the correction at 23 Hz closes the lag-target gap slowly enough that during a fast forward sweep, the read head visibly lags behind the gesture. This is the "floaty" or "disconnected" feel on forward motion.

### Differential Impact Between Forward and Backward

For **backward** motion, the gesture velocity directly encodes the direction and speed of lag increase. Backward drag makes `platterGestureVelocity < 0`, which contributes negative `targetReadVelocity`, and the correction velocity also works against the motion (since `lagError > 0` for backward). The result is that backward drag's feel is governed mainly by `scratchHandVelocity` following `platterGestureVelocity` at `kHybridScratchHandFollowHz = 220 Hz`, which is responsive.

For **forward** motion, `platterGestureVelocity > 0`, and the correction velocity is also positive (trying to close the lag gap). Both push in the same direction, but the combined effect depends on whether the lag-error term is large enough. When the write head is advancing, it continuously widens the lag error, so the correction must perpetually fight the drift. With correctionScale = 0.33, the correction bandwidth (23 Hz) is easily saturated by the drift, making forward motion feel heavier than backward.

---

## Part 3: The `gestureDirection` Fallback Uses Stale `targetReadVelocity`

### The Problem

The gesture direction logic falls through to `targetReadVelocity` when `lagDeltaSinceLastGesture` is below threshold:

```cpp
float gestureDirection = 0.f;
if (hasFreshPlatterGesture) {
    float lagDeltaSinceLastGesture = platterLagTarget - float(lastPlatterLagTarget);
    if (std::fabs(lagDeltaSinceLastGesture) > 1e-4f) {
        gestureDirection = (lagDeltaSinceLastGesture > 0.f) ? -1.f : 1.f;
    }
}
if (gestureDirection == 0.f && std::fabs(targetReadVelocity) > kHybridScratchVelocityDeadband) {
    gestureDirection = (targetReadVelocity > 0.f) ? 1.f : -1.f;
}
```

At this point, `targetReadVelocity` is `platterGestureVelocity` (compensation never fires, as established above). The `gestureDirection` derived from `targetReadVelocity` agrees with the lag delta direction in the normal case, so this is not a hard bug. However, there is a subtle issue in the **expander path**.

The expander calls `setScratch` with a `lagTarget` that decreases packet-by-packet during forward drag. `platterGestureRevision` is incremented per `setScratch` call, so `hasFreshPlatterGesture` is usually true. But `lagDeltaSinceLastGesture = platterLagTarget - lastPlatterLagTarget` uses the value from the _previous audio frame_, not the previous expander update. Between consecutive expander updates (every ~16ms = ~735 audio frames at 44.1 kHz), `lastPlatterLagTarget` stays at the previous expander update's value, so `lagDeltaSinceLastGesture` is computed correctly on the first audio frame of each expander update, then repeated for the next ~734 frames. That repetition is fine. But during the **first frame of a fresh expander update**, if the lag target jumped significantly, `lagDeltaSinceLastGesture` is amplified by the whole inter-update lag delta, which is consistent with the actual direction. Not a bug per se, but worth noting.

---

## Part 4: Expander Path — Velocity Derivation Biases Forward Drag

### The Problem

In `TemporalDeck.cpp` (lines ~1329–1349), the velocity sent to the platter input is blended from two sources:

```cpp
float derivedVelocity = (impl->expanderLagDragLastLagSamples - lagTarget) / dtSec;
// ...
velocitySamples = 0.75f * derivedVelocity + 0.25f * lagDragRequestVelocity;
```

`dtSec` is computed from `expanderLagDragFramesSinceUpdate`:

```cpp
int frames = std::max(1, impl->expanderLagDragFramesSinceUpdate);
float dtSec = std::max(args.sampleTime, float(frames) * args.sampleTime);
```

`expanderLagDragFramesSinceUpdate` is the number of **audio frames** elapsed since the last expander request (incremented per `process()` call when the request seq hasn't changed). For a normal 60fps expander, this is ~735 frames. Each time a new request arrives, `expanderLagDragFramesSinceUpdate` is reset to 0 and the next frame reads it as 0 (or 1 due to `std::max(1, ...)`), meaning `dtSec = args.sampleTime` (one sample time ≈ 22.7 µs at 44.1 kHz), **not** the true inter-update interval.

For the **first audio frame** after each expander update, `frames = 1` → `dtSec = sampleTime = 22.7 µs`. With a lag delta of, say, 200 samples:

```
derivedVelocity = 200 / 0.0000227 = 8,810,572 samples/sec
```

This is clamped by `maxAbsGestureVelocity = sampleRate * 3.0 = 132,300`, but even so the blend `0.75 * 132300 + 0.25 * V_scope` wildly overestimates the true gesture velocity for the first frame of each expander update during forward drag. The platter input receives this inflated velocity, which then drives `scratchHandVelocity` to follow at `kHybridScratchHandFollowHz = 220 Hz`, causing a visible velocity spike at each expander packet boundary.

For **backward** drag the effect is symmetric, but forward drag is more perceptually sensitive because:
- Forward drag ends at NOW (lag = 0), where there is no `allowNowSnap` overshoot protection (`allowNowSnap = !manualTouchScratch = false`).
- Overshooting forward leaves the read head briefly at lag < target, causing a micro-stutter as correction pulls backward.

### The Fix

`expanderLagDragFramesSinceUpdate` should be measured on the **previous** update cycle, not reset to 0 before reading. Alternatively, the `dtSec` for derivation should use the actual elapsed interval measured in wall time (similar to how the UI drag measures `dtSec = nowSec - lastMoveTimeSec`), or at minimum use the **previous** frame count rather than resetting first:

```cpp
// Before the reset, capture the interval
float dtSec = std::max(args.sampleTime, 
    float(std::max(1, impl->expanderLagDragFramesSinceUpdate)) * args.sampleTime);
impl->expanderLagDragFramesSinceUpdate = 0;  // reset after reading
```

The current code resets `expanderLagDragFramesSinceUpdate = 0` at line ~1350, then `dtSec` is computed from `frames = std::max(1, 0) = 1` on the same branch. Swap the read before the reset.

---

## Part 5: `motionFreshSamples` Keeps `deepLiveManualMotion` True Between Expander Packets

### The Problem

After each expander update that constitutes active drag, the host sets:

```cpp
int motionFreshSamples = clamp(..., minHoldSamples, maxHoldSamples);
// minHoldSamples ≈ 1103 samples at 44.1 kHz (25ms)
// maxHoldSamples ≈ 3969 samples (90ms)
impl->platterInput.setMotionFreshSamples(motionFreshSamples);
```

The fresh window covers the gap between expander packets (~16ms at 60fps ≈ 706 samples), so `platterMotionActive = true` is maintained continuously during sustained drag. This is correct and deliberate.

However, because `deepLiveManualMotion = !sampleModeActive && manualTouchScratch && manualMotionActive` (simplified from Part 2), `deepLiveManualMotion` is persistently true for the entire drag session. The `correctionScale` never steps up to 0.68 even when the user's lag target is near NOW. This means a forward drag that starts deep in the buffer and accelerates toward NOW progressively gets more sluggish as the read head approaches the near-NOW zone where the user most expects snappy response.

### Interaction with `nowSnapThresholdSamples`

The now-snap that would catch forward drift near NOW:

```cpp
if (allowNowSnap && scratchLagTargetSamples <= nowSnapThresholdSamples && 
    scratchLagSamples <= nowSnapThresholdSamples && scratchMotionVelocity >= 0.f) {
    // snap to lag = 0
}
```

is disabled for touch (`allowNowSnap = !manualTouchScratch = false`). So there is no snap backstop when correction undershoots. The user who expects forward drag to land cleanly at NOW may find the read head hovering within the 33ms nowSnap window but never settling.

---

## Part 6: Freeze-Mode Forward Assist Not Applied to Non-Freeze Live Touch

### The Problem

Two "feel direct" blocks in the engine's manual touch path instantly teleport the read head to the lag target when moving forward during explicit freeze:

```cpp
// Block A — edge-release assist
if (!sampleModeActive && freezeState && limit > 0.0 && 
    scratchLagSamples >= (limit - 0.5) &&
    scratchLagTargetSamples < (scratchLagSamples - 1.0)) {
    scratchLagSamples = scratchLagTargetSamples;
    scratchHandVelocity = 0.f; scratchMotionVelocity = 0.f; scratch3LagVelocity = 0.f;
    readHead = buffer.wrapPosition(newestPos - scratchLagSamples);
}

// Block B — general forward feel-direct
if (!sampleModeActive && freezeState && scratchLagTargetSamples < (scratchLagSamples - 1.0)) {
    scratchLagSamples = scratchLagTargetSamples;
    scratchHandVelocity = 0.f; scratchMotionVelocity = 0.f; scratch3LagVelocity = 0.f;
    readHead = buffer.wrapPosition(newestPos - scratchLagSamples);
}
```

Both are conditioned on `freezeState` (the actual freeze button/CV state). In non-freeze live mode, even though `freezeForScratchModel = true`, `freezeState = false`, so these blocks are skipped. The forward motion responsiveness that freeze mode provides — direct snap to the new target when moving forward — is unavailable during normal live drag. This asymmetry is visible: dragging forward with Freeze enabled feels crisply direct; without Freeze it feels sluggish, even though the user's intent is identical.

**Note:** Block B's threshold of 1.0 sample is very tight. Even a tiny forward gesture (lag target decreasing by more than 1 sample) will teleport the read head in freeze mode. This is probably too aggressive — it discards `scratchMotionVelocity` state that would otherwise provide inertial smoothing.

---

## Part 7: Anchor Stale for Expander Drag Duration

### The Problem

At scratch-start, the live anchor is set:

```cpp
if (!sampleModeActive && manualTouchScratch) {
    liveManualScratchAnchorNewestPos = newestPos;
    liveManualScratchAnchorLagSamples = scratchLagSamples;
}
```

The anchor is only re-synced at the buffer limit:

```cpp
if (!sampleModeActive && limit > 0.0 && scratchLagSamples >= (limit - 0.5) &&
    scratchLagTargetSamples >= (limit - 0.5)) {
    liveManualScratchAnchorNewestPos = newestPos;
}
```

The drift compensation that uses this anchor:

```cpp
if (!sampleModeActive && freezeState) {
    double driftLag = std::max(0.0, newestPos - liveManualScratchAnchorNewestPos);
    targetLag += driftLag * driftMix;
}
```

is correctly gated to `freezeState` only. In non-freeze live mode the stale anchor is never read for drift compensation. However, if a live drag session is active when the user engages Freeze mid-drag, the drift compensation will compute `driftLag` from the **drag start** anchor, not from the freeze-engage moment. For a long drag session, `driftLag` may be large enough to significantly distort the lag target when freeze engages during drag, making the transition into freeze feel like a lag jump.

---

## Part 8: `filteredManualLagTargetSamples` Is Set But Never Read During Drag

This is a minor vestigial field. It is set at scratch-start:

```cpp
filteredManualLagTargetSamples = scratchLagSamples;
```

And used in `applyPendingLiveSeekArc` in `TemporalDeckTransportControl.cpp`:

```cpp
engine.filteredManualLagTargetSamples = targetLag;
```

But it is never read in the live drag path in `process()`. The name suggests it was once a lag target smoother for the scratch model but has been superseded. Safe to remove to reduce confusion.

---

## Part 9: Expander Velocity Sign Preserved Correctly (Not a Bug)

The CONTAINMENT NOTE and surrounding code give the impression there may be a sign convention conflict between the expander velocity and the UI velocity. There is not — both paths agree:
- Positive `velocitySamples` / `filteredGestureVelocity` = toward NOW (decreasing lag)
- Negative = away from NOW (increasing lag)

The CONTAINMENT NOTE is correct in its handling of backward gestures (`platterGestureVelocity < 0` = backward = away from NOW). The issue is that the entire block is dead code for touch (Part 1), not that the signs are wrong.

---

## Summary of Issues by Severity

| # | Issue | Severity | Path |
|---|-------|----------|------|
| 1 | `writeHeadCompensationActive` always false for touch; CONTAINMENT NOTE is dead code | High | UI + Expander |
| 2 | `deepLiveManualMotion` always true for touch; correction permanently at 0.33 scale | High | UI + Expander |
| 4 | Expander `dtSec` computed after reset → velocity spike on each packet boundary | High | Expander only |
| 6 | Freeze-mode "feel-direct" forward assist not applied in non-freeze live touch | Medium | UI + Expander |
| 5 | `allowNowSnap = false` for touch; no backstop for forward undershoot near NOW | Medium | UI + Expander |
| 7 | Anchor goes stale mid-drag; drift compensation jumps if freeze engaged during drag | Low | UI + Expander |
| 3 | `gestureDirection` fallback from `targetReadVelocity` uses post-compensation value (inert) | Low | UI + Expander |
| 8 | `filteredManualLagTargetSamples` vestigial — set, never read in drag path | Cosmetic | Both |

---

## Recommended Path Forward

**Short-term (fix the feel without restructuring):**

1. **Fix `deepLiveManualMotion` to use actual lag depth**, not `writeHeadCompensationActive`. The intended split was: near-NOW drag gets high correction (0.68), deep drag gets reduced correction (0.33). Use `scratchLagSamples <= sampleRate * kLiveManualWriteHeadCompensationWindowSec` as the proximity test directly:

   ```cpp
   bool deepLiveManualMotion = !sampleModeActive && manualTouchScratch && manualMotionActive &&
       scratchLagSamples > double(sampleRate * kLiveManualWriteHeadCompensationWindowSec);
   ```

2. **Fix `expanderLagDragFramesSinceUpdate` read-before-reset** (Part 4) to get accurate `derivedVelocity` on each expander packet.

3. **Enable `allowNowSnap` for forward-only manual touch** (when `gestureDirection > 0.f` or when `platterGestureVelocity > 0`) so forward drag that reaches near-NOW settles cleanly.

**Medium-term (architectural cleanup):**

4. Remove the `writeHeadCompensationActive` add/subtract pair and the CONTAINMENT NOTE branch entirely since they're never reachable for touch. If wheel scratch needs this logic, scope it explicitly to `wheelScratch` rather than burying it in the dead manual touch branch.

5. Consider extending the freeze-mode "feel-direct" forward blocks to non-freeze live touch, with a higher threshold than 1.0 sample and without discarding velocity state (interpolate toward target instead of teleporting).

6. Anchor the live drag start position more precisely for freeze-transition safety: store a separate "freeze-engage anchor" at the moment `freezeState` rises during an active drag session.
