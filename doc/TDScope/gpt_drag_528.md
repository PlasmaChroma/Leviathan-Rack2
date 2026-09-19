Dragon King Leviathan, yes — I get the thrust. The current design is very close, but the “forward drag back to NOW” problem is being split across **two partially conflicting control models**:

1. **TDScope UI** sends a desired lag target and gesture velocity.
2. **Temporal Deck engine** interprets that as platter/manual scratch motion while live audio keeps advancing the write head.

That means the user’s hand is trying to move the read head forward, but live mode is continuously moving the finish line. The mechanism needs to feel like “pulling myself back to the present,” not “swimming upstream through fresh samples.”

## Main diagnosis

The biggest issue I see is this:

```cpp
static constexpr double kLagDragHoldDetectSec = 1.0 / 180.0;
```

in `TDScope.cpp`.

That is about **5.5 ms**. In Rack/UI terms, that is so short that ordinary gaps between drag events can be interpreted as “stationary hold.” TDScope then sends:

```cpp
module->setLagDragRequest(true, lagDragLocalLagSamples, 0.f, true);
```

with `stationaryHold = true`.

That matters because the engine has a direct touch-hold path:

```cpp
if (directTouchHoldActive) {
  ...
  readHead = platterTouchHoldReadHead;
  double heldLag = clampLag(currentLagFromNewest(newestPos), limit);
  scratchLagSamples = heldLag;
  scratchLagTargetSamples = heldLag;
}
```

The direct hold semantics are “hold this read head in place.” In LIVE mode, while input keeps filling the buffer, a stationary read head means **lag grows**. So if stationary hold is being triggered too aggressively, the user tries to move forward and the system keeps reintroducing lag.

That is probably the “why does it feel hard to compensate?” gremlin.

## The second major issue

In `TemporalDeckEngine.hpp`, live manual touch is treated as freeze-like:

```cpp
bool liveTouchUsesFrozenScratchModel = !sampleModeActive && manualTouchScratch;
bool scratchModelTreatsAsFreeze = freezeState || sampleTouchUsesFrozenScratchModel || liveTouchUsesFrozenScratchModel;
```

Then several live compensation helpers refuse to run when `scratchModelTreatsAsFreeze` is true:

```cpp
return !sampleModeActive &&
       platter_interaction::shouldApplyWriteHeadCompensation(scratchModelTreatsAsFreeze, ...)
```

and:

```cpp
if (sampleModeActive || scratchModelTreatsAsFreeze || !manualTouchScratch || !manualMotionActive ||
    gestureDirection <= 0.f) {
  return 0.f;
}
```

So TDScope is clearly trying to create a live forward-drag assist, but the engine classifies that drag as freeze-like and therefore suppresses the write-head compensation / toward-NOW assist that would actually make the drag feel sovereign.

The intent is good, but the model is crossed.

## The control equation should be simpler

For LIVE mode, think in terms of lag:

```text
lag = newestWritePosition - readHead
```

The write head advances at roughly:

```text
sampleRate samples/second
```

So to keep lag constant while live input continues, the read head must also move forward at:

```text
sampleRate samples/second
```

To move toward NOW, the read head must move faster than the write head.

So the engine-side target read velocity for a scope drag should effectively be:

```cpp
targetReadVelocity = sampleRate + lagDragVelocity;
```

where `lagDragVelocity` is already computed in TDScope as:

```cpp
(previousLag - currentLag) / dt
```

Positive means “moving toward NOW.”

Right now, the engine path often behaves more like:

```cpp
targetReadVelocity = lagDragVelocity;
```

because the `+ sampleRate` compensation is gated away for live manual touch. That is the river-fighting feeling.

## Recommended design direction

I would split the behavior into three explicit modes.

### 1. Active drag: co-moving lag control

While the user is actually dragging, TDScope should send a target lag and velocity. Temporal Deck should treat that as **live lag control**, not generic platter touch.

Engine behavior:

```cpp
if (scopeLagDragActive && liveMode && !freezeState) {
  targetReadVelocity = sampleRate + scopeLagDragVelocity;
}
```

This makes forward drag feel correct:

```text
velocity = 0      => hold current lag relative to NOW
velocity > 0      => move toward NOW
velocity < 0      => move deeper into the buffer
```

That is the core fix.

### 2. Soft hold: hold lag, not read head

When the mouse is down but not moving, the default should not be “pin the read head.” In LIVE mode, that causes lag growth.

Instead, stationary scope hold should mean:

```text
keep the same lag relative to live NOW
```

In implementation terms:

```cpp
readHead += 1 sample per audio frame
```

or simply:

```cpp
targetReadVelocity = sampleRate;
scratchLagTargetSamples = heldLag;
```

This keeps the user visually and mechanically in place.

### 3. Hard hold: optional “grab tape” mode

The current direct hold behavior is still useful. It feels like grabbing a piece of tape and letting live time move past it. But that should be a deliberate mode, not the default result of tiny mouse-event gaps.

Use something like:

```text
Shift + drag/hold = hard hold / tape grab
normal drag/hold = soft live lag hold
```

or expose it as an alternate gesture later.

## Immediate fixes I would make

### Fix 1: Increase or remove the 5.5 ms hold detector

Current:

```cpp
static constexpr double kLagDragHoldDetectSec = 1.0 / 180.0;
```

Recommended:

```cpp
static constexpr double kLagDragHoldDetectSec = 0.090; // 90 ms
```

or even:

```cpp
static constexpr double kLagDragHoldDetectSec = 0.120; // 120 ms
```

But my stronger recommendation: **do not send `stationaryHold = true` automatically during ordinary scope dragging.**

Use a separate semantic flag later for “hard hold.”

### Fix 2: Do not send stationary hold on initial mouse-down

Current:

```cpp
module->setLagDragRequest(true, lagDragLocalLagSamples, 0.f, true);
```

in `beginLagDragAt()`.

Recommended:

```cpp
module->setLagDragRequest(true, lagDragLocalLagSamples, 0.f, false);
```

A mouse press should enter active control, not immediately latch a direct hold.

### Fix 3: In LIVE mode, add the write-head baseline for scope drag

The engine already has this concept:

```cpp
targetReadVelocity += sampleRate;
```

but the conditions around `scratchModelTreatsAsFreeze` appear to prevent it from applying to live manual touch/scope drag.

I would not globally change platter behavior. Instead, distinguish **scope lag drag** from **local platter touch**.

Add something like this to `PlatterInputSnapshot`:

```cpp
bool scopeLagDragActive = false;
bool scopeLagDragSoftHold = false;
```

Then route TDScope’s expander drag request through a scope-specific setter:

```cpp
void setScopeLagDrag(bool active, float lagSamples, float velocitySamples, bool softHold);
```

Then in the engine:

```cpp
if (!sampleModeActive && scopeLagDragActive && !freezeState) {
  targetReadVelocity = sampleRate + platterGestureVelocity;
}
```

For soft hold:

```cpp
if (!sampleModeActive && scopeLagDragSoftHold && !freezeState) {
  scratchLagTargetSamples = clampLag(platterLagTarget, limit);
  targetReadVelocity = sampleRate;
}
```

That is the “I am holding my hand still relative to the living NOW” behavior.

## The 1000 ms next-to-NOW zone

I would make the lower/near-NOW region explicitly magnetic.

You already have:

```cpp
kLiveManualWriteHeadCompensationWindowSec = 1.0f;
```

That is the right conceptual boundary. I would formalize this as the **LIVE NOW assist zone**:

```cpp
static constexpr float kScopeLiveNowAssistWindowSec = 1.0f;
static constexpr float kScopeLiveNowSnapMs = 25.f;
static constexpr float kScopeLiveForwardAssistExtraRatio = 0.85f;
```

Behavior:

```cpp
float nowZone = sampleRate * kScopeLiveNowAssistWindowSec;
float depthT = clamp(lag / nowZone, 0.f, 1.f);
float nearNowT = 1.f - depthT;

float forwardAssist = sampleRate * nearNowT * nearNowT * kScopeLiveForwardAssistExtraRatio;
```

Then if the user is dragging toward NOW:

```cpp
targetReadVelocity = sampleRate + userTowardNowVelocity + forwardAssist;
```

This gives the last second of lag a “gravity well” back to the present. The user still controls it, but they do not need superhuman precision to cross the final 100–300 ms.

At very near NOW:

```cpp
if (lag < sampleRate * 0.025f && gestureDirectionTowardNow) {
  snapToNow();
}
```

That is a 25 ms capture zone. It should feel like docking, not like losing control.

## Release behavior

On mouse release, the current system sends:

```cpp
module->setLagDragRequest(false, 0.f, 0.f, false);
```

Then the deck decides what to do.

I would add release memory:

```cpp
lastScopeReleaseLag
lastScopeReleaseVelocity
lastScopeReleaseWasForward
```

If release happens inside the 1000 ms zone and the last motion was toward NOW, engage a short catch:

```cpp
if (releaseLag < sampleRate * 1.0f && releaseVelocity > 0.f) {
  beginScopeNowCatch(releaseLag, releaseVelocity);
}
```

Do not make this a hard teleport unless lag is tiny. Use tiers:

```text
0–25 ms: snap to NOW
25–250 ms: fast catch, 40–80 ms
250–1000 ms: eased catch, 100–220 ms
>1000 ms: no auto-catch unless user has strong forward release velocity
```

Your existing `kNowCatchTime = 0.004f` is too short to be a perceptual UX catch. That is essentially a micro-snap. For this interaction, I would use a separate scope catch time, probably around:

```cpp
static constexpr float kScopeNowCatchMinSec = 0.045f;
static constexpr float kScopeNowCatchMaxSec = 0.180f;
```

## TDScope rendering state looks mostly healthy

The live bucket design is sensible. LIVE mode ingests bins into lag-keyed buckets, which is the right idea for a moving write head. The same logic exists in both `TDScope.cpp` and `TDScopeGL.cpp`, so one practical warning: any change to the live window/bucket/near-NOW mapping needs to be kept in sync across both render paths. Right now there is enough duplicated logic that future drift is likely.

I would eventually move the live bucket ingest/rebuild code into a shared helper in `TDScopeShared.hpp` or a new `TDScopeLiveGeometry.hpp`.

## Codex-ready implementation plan

### Phase 1 — make current behavior less hostile

1. Change `kLagDragHoldDetectSec` from `1.0 / 180.0` to `0.090` or `0.120`.
2. In `beginLagDragAt()`, send `stationaryHold = false`.
3. In the tiny jitter branch, do not immediately send stationary hold. Send active drag with previous lag and zero velocity:

```cpp
module->setLagDragRequest(true, lagDragLocalLagSamples, 0.f, false);
```

4. Only send true stationary hold after a real pause, and only if you actually want hard-hold semantics.

This alone may remove a lot of the sticky forward-drag feeling.

### Phase 2 — add scope-specific live drag semantics

Add to `PlatterInputSnapshot`:

```cpp
bool scopeLagDragActive = false;
bool scopeLagDragSoftHold = false;
```

Add a new setter:

```cpp
void PlatterInputState::setScopeLagDrag(bool active, float lagSamples, float velocitySamples, bool softHold);
```

Then in the Temporal Deck module’s expander request decode path, route TD.Scope requests to `setScopeLagDrag()` instead of generic `setScratch()` / `setTouchHold()` behavior.

In the engine, when `scopeLagDragActive && live && !freeze`:

```cpp
targetReadVelocity = sampleRate + platterGestureVelocity;
```

When `scopeLagDragSoftHold && live && !freeze`:

```cpp
targetReadVelocity = sampleRate;
scratchLagTargetSamples = clampLag(platterLagTarget, limit);
```

### Phase 3 — implement the 1000 ms NOW assist zone

Add a live-specific helper:

```cpp
float scopeLiveTowardNowAssist(float lagSamples, float gestureVelocity, float sampleRate) {
  if (gestureVelocity <= 0.f) return 0.f;

  const float zone = sampleRate * 1.0f;
  const float depthT = clamp(lagSamples / zone, 0.f, 1.f);
  const float nearNowT = 1.f - depthT;

  return sampleRate * 0.85f * nearNowT * nearNowT;
}
```

Apply:

```cpp
targetReadVelocity = sampleRate + platterGestureVelocity
                   + scopeLiveTowardNowAssist(scratchLagSamples, platterGestureVelocity, sampleRate);
```

Then add a snap threshold:

```cpp
if (scratchLagSamples < sampleRate * 0.025f && platterGestureVelocity > 0.f) {
  readHead = newestPos;
  scratchLagSamples = 0.0;
  scratchLagTargetSamples = 0.0;
}
```

### Phase 4 — release catch

On drag release, if the last scope velocity was forward and lag is under 1 second, start a short catch-to-now glide. Keep this separate from normal slip return so it does not contaminate the musical behavior of the deck.

## My bottom-line recommendation

Do **not** keep trying to solve this mostly from TDScope by warping the requested lag target. The UI should send clean intent:

```text
target lag
gesture velocity
active / released / soft hold
```

Temporal Deck should own live causality:

```text
read velocity = write-head velocity + user lag velocity
```

The core shift is this:

```text
Current feeling:
“I drag forward, but NOW keeps escaping.”

Desired feeling:
“I drag inside a moving river, and the deck gives me the boat’s current velocity for free.”
```

That is the elegant fix. The user still performs the gesture; the system simply stops making them pay the write-head tax.
