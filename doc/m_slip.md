# TemporalDeck — Musical Slip Return Redesign Spec

## Goal

Replace the current slip return behavior with a more musical approach that:

* avoids bright HF chatter during timed slip returns
* avoids hard positional snap behavior near NOW
* feels like a platter or transport motor catching back up
* reuses the module’s existing variable-rate read and interpolation architecture
* keeps current Slip UI/menu behavior intact

This change should affect **live-mode slip return only**.
Do **not** change manual scratch behavior, external CV scratch behavior, sample mode release behavior, or freeze behavior unless explicitly required by the integration points below.

---

## Problem Summary

The current slip return is effectively a **position error shrinker**:

* it computes lag-to-NOW
* reduces that lag over a configured return time
* enters a short final-catch phase
* then snaps/pins to live

That can create audible HF grit because:

1. the read-head velocity implied by direct lag collapse is not especially smooth
2. slip return does not fully behave like the higher-quality variable-rate scratch read path
3. final rejoin is still fundamentally a positional correction event rather than a masked audio handoff

---

## High-Level Design

### New behavior

Slip return should no longer be implemented as:

* “move lag directly toward zero over time”

It should instead be implemented as:

* “allow the read head to run forward faster than transport”
* “control that extra forward speed with an acceleration-limited servo”
* “reduce catch-up speed naturally as lag approaches zero”
* “perform final rejoin with a short equal-power blend into live”

### Mental model

Think of slip return as:

* transport base motion = platter motor nominal forward motion
* slip catch-up motion = temporary extra motor torque
* final arrival at NOW = gentle takeover, not hard positional pinning

---

## Required Behavioral Outcome

### Audible target

Compared to the current implementation, the new slip return should sound:

* smoother during medium/large returns
* less “sprayed” or “spitty” in the highs
* more like a deck catching up than a resampler being shoved
* less obviously synthetic during the last 10–20 ms before rejoin

### Functional target

The user-visible Slip modes should still behave conceptually as:

* **Slow** = slower musical catch-up
* **Normal** = default musical catch-up
* **Instant** = effectively immediate return, but still avoid a raw sample-discontinuous snap when practical

The existing slip mode enum and menu structure should remain unchanged. 

---

# Implementation Plan

## 1. Add dedicated servo-based slip return state

### Add these members to `TemporalDeckEngine`

Remove dependence on the current `slipFinalCatchActive`, `slipReturnRemaining`, and `slipReturnStartLag` behavior for the new return model.

Add:

```cpp
float slipCatchVelocity = 0.f;          // samples/sec, extra forward velocity above transport
bool slipBlendActive = false;           // final masked rejoin phase
float slipBlendRemaining = 0.f;         // seconds remaining in blend
double slipBlendStartReadHead = 0.0;    // read position at blend start
double slipBlendStartLag = 0.0;         // lag at blend start
```

### Keep these existing members

Keep and reuse:

```cpp
bool slipReturning = false;
float slipReturnOverrideTime = -1.f;
int slipReturnMode = TemporalDeck::SLIP_RETURN_NORMAL;
```

These are already part of the current architecture and should remain the public-facing control layer.  

---

## 2. Add new constants for servo return

### New constants

Add these near the current slip constants:

```cpp
static constexpr float kSlipBlendTime = 0.010f;              // 10 ms final blend
static constexpr float kSlipNearNowBlendThresholdMs = 18.f;  // begin blend window
static constexpr float kSlipCatchLagReferenceSec = 0.12f;    // lag normalization for speed curve
static constexpr float kSlipCatchMaxExtraRatioSlow = 1.10f;  // extra speed over transport
static constexpr float kSlipCatchMaxExtraRatioNormal = 1.65f;
static constexpr float kSlipCatchMaxExtraRatioInstant = 3.50f;
static constexpr float kSlipCatchAccelSlow = 7.5f;           // in units of sampleRate/sec
static constexpr float kSlipCatchAccelNormal = 11.0f;
static constexpr float kSlipCatchAccelInstant = 22.0f;
static constexpr float kSlipCatchBrakeMultiplier = 1.6f;     // stronger decel near arrival
static constexpr float kSlipCatchDoneVelocityRatio = 0.20f;  // relative to sampleRate
```

### Notes

* These are intentionally stated in perceptual terms, not “physics-pure” terms.
* Exact values may be tuned later, but use these initial defaults.
* Existing `kSlipReturnTime` may still be kept for compatibility, but the new system should primarily be governed by servo ratios/accel, not direct lag interpolation.

---

## 3. Add helper functions

Add the following helper methods to `TemporalDeckEngine`.

---

### 3.1 `float slipCatchMaxExtraRatio() const`

Returns mode-dependent maximum extra forward speed ratio.

```cpp
float slipCatchMaxExtraRatio() const {
  if (slipReturnOverrideTime >= 0.f) {
    return kSlipCatchMaxExtraRatioInstant;
  }
  switch (slipReturnMode) {
    case TemporalDeck::SLIP_RETURN_SLOW:   return kSlipCatchMaxExtraRatioSlow;
    case TemporalDeck::SLIP_RETURN_INSTANT:return kSlipCatchMaxExtraRatioInstant;
    case TemporalDeck::SLIP_RETURN_NORMAL:
    default:                               return kSlipCatchMaxExtraRatioNormal;
  }
}
```

---

### 3.2 `float slipCatchAccelRatio() const`

Returns mode-dependent acceleration scale.

```cpp
float slipCatchAccelRatio() const {
  if (slipReturnOverrideTime >= 0.f) {
    return kSlipCatchAccelInstant;
  }
  switch (slipReturnMode) {
    case TemporalDeck::SLIP_RETURN_SLOW:   return kSlipCatchAccelSlow;
    case TemporalDeck::SLIP_RETURN_INSTANT:return kSlipCatchAccelInstant;
    case TemporalDeck::SLIP_RETURN_NORMAL:
    default:                               return kSlipCatchAccelNormal;
  }
}
```

---

### 3.3 `void cancelSlipReturnState()`

Reusable cleanup helper.

```cpp
void cancelSlipReturnState() {
  slipReturning = false;
  slipBlendActive = false;
  slipCatchVelocity = 0.f;
  slipBlendRemaining = 0.f;
  slipBlendStartReadHead = readHead;
  slipBlendStartLag = 0.0;
  slipReturnOverrideTime = -1.f;
}
```

Use this wherever the old code currently clears slip-return state due to scratch takeover, sample mode, etc.

---

### 3.4 `void startSlipBlend(double lagNow)`

```cpp
void startSlipBlend(double lagNow) {
  slipBlendActive = true;
  slipBlendRemaining = kSlipBlendTime;
  slipBlendStartReadHead = readHead;
  slipBlendStartLag = lagNow;
}
```

---

## 4. Replace direct lag-collapse return with servo catch-up

## Current behavior to replace

The old implementation directly computes a `targetLag`, rewrites `readHead = newestPos - targetLag`, then uses a final catch phase and hard pin. That logic should be removed and replaced. 

---

## New core rule

During `slipReturning`, the read head should advance according to:

```cpp
read velocity = transport velocity + slipCatchVelocity
```

Where:

* `transport velocity` is the normal forward live transport speed
* `slipCatchVelocity` is an additional positive forward velocity generated by the servo
* the servo should reduce catch velocity as lag approaches zero

---

## 5. New slip-return integrator block

Insert a new helper:

```cpp
void integrateSlipCatchup(double newestPos, double maxLag, double minLag, float baseSpeed, float dt)
```

This function should only run when:

* `slipReturning == true`
* `!anyScratch`
* `!sampleModeActive`
* `!freezeForScratchModel`

### Pseudocode

```cpp
void integrateSlipCatchup(double newestPos, double maxLag, double minLag, float baseSpeed, float dt) {
  double lagNow = currentLagFromNewest(newestPos);
  float sr = std::max(sampleRate, 1.f);

  // Abort if already effectively at NOW
  if (lagNow <= 0.5) {
    readHead = newestPos;
    cancelSlipReturnState();
    return;
  }

  float lagSec = float(lagNow / sr);

  // Convert lag into desired extra speed.
  // Use a soft sqrt curve so large lag does not explode into harsh pitch motion.
  float lagNorm = clamp(lagSec / kSlipCatchLagReferenceSec, 0.f, 1.f);
  float desiredExtraRatio = slipCatchMaxExtraRatio() * std::sqrt(lagNorm);

  // Fade catch drive down as we approach NOW.
  float blendThresholdSec = kSlipNearNowBlendThresholdMs * 0.001f;
  float nearNowGain = clamp(lagSec / blendThresholdSec, 0.f, 1.f);
  desiredExtraRatio *= nearNowGain;

  float desiredExtraVel = desiredExtraRatio * sr;

  // Acceleration-limited slew
  float accelBase = slipCatchAccelRatio() * sr;
  float brakeBase = accelBase * kSlipCatchBrakeMultiplier;
  float dv = desiredExtraVel - slipCatchVelocity;
  float maxDv = (dv >= 0.f ? accelBase : brakeBase) * dt;
  slipCatchVelocity += clamp(dv, -maxDv, maxDv);

  // Never let catch velocity go negative
  slipCatchVelocity = std::max(0.f, slipCatchVelocity);

  // Move read head as variable-rate transport
  float transportVel = baseSpeed * sr;
  double unwrappedRead = unwrapReadNearWrite(readHead, newestPos);
  double candidate = unwrappedRead + double(transportVel + slipCatchVelocity) * double(dt);

  // During slip return we may touch NOW, but must not overshoot beyond newestPos
  candidate = std::max(newestPos - maxLag, std::min(candidate, newestPos));
  readHead = buffer.wrapPosition(candidate);

  // Recompute lag after motion
  lagNow = currentLagFromNewest(newestPos);
  lagSec = float(lagNow / sr);

  // Start masked blend when close enough and no longer approaching aggressively
  if (!slipBlendActive &&
      lagSec <= (kSlipNearNowBlendThresholdMs * 0.001f) &&
      slipCatchVelocity <= (kSlipCatchDoneVelocityRatio * sr)) {
    startSlipBlend(lagNow);
  }
}
```

---

## 6. Add final masked rejoin blend

## Goal

Do not finish slip return by directly pinning `readHead = newestPos` while the moving read stream is still audibly different.

Instead:

* continue generating a catch-up read stream
* simultaneously generate a live read stream from `newestPos`
* equal-power blend from catch stream to live stream over `kSlipBlendTime`

---

## 7. Promote slip return into variable-rate read path

Currently, scratch interpolation logic is gated by `anyScratch`. Slip return should use the same higher-quality read strategy because it is also a variable-rate read condition.  

### Replace

```cpp
bool scratchReadPath = anyScratch;
```

### With

```cpp
bool variableRateReadPath = anyScratch || slipReturning || slipBlendActive;
bool scratchReadPath = variableRateReadPath;
```

This preserves downstream cartridge-motion treatment if it currently expects `scratchReadPath`, but broadens the meaning to include slip return.

### Interpolation rule

When `variableRateReadPath` is true:

* use current scratch interpolation mode machinery
* default recommendation remains `LAGRANGE6`
* user-selected `SINC` should also work
* do not force plain cubic during slip return

---

## 8. Audio render logic during final blend

Where the code currently reads the wet signal, add a special case for `slipBlendActive`.

### Required behavior

Render:

* `catchRead = read from current moving readHead`
* `liveRead = read from newestPos`
* blend them with equal-power law

### Pseudocode

```cpp
std::pair<float, float> readInterpolatedAt(double pos, int interpMode) {
  switch (interpMode) {
    case TemporalDeck::SCRATCH_INTERP_SINC:
      return buffer.readSinc(pos);
    case TemporalDeck::SCRATCH_INTERP_LAGRANGE6:
      return buffer.readHighQuality(pos);
    case TemporalDeck::SCRATCH_INTERP_CUBIC:
    default:
      return buffer.readCubic(pos);
  }
}
```

Then in wet-read section:

```cpp
if (slipBlendActive) {
  slipBlendRemaining = std::max(0.f, slipBlendRemaining - dt);
  float t = 1.f - clamp(slipBlendRemaining / std::max(kSlipBlendTime, 1e-6f), 0.f, 1.f);

  auto catchWet = readInterpolatedAt(readHead, effectiveScratchInterpolation);
  auto liveWet = readInterpolatedAt(newestPos, effectiveScratchInterpolation);

  float a = std::cos(0.5f * float(M_PI) * t);
  float b = std::sin(0.5f * float(M_PI) * t);

  wetL = catchWet.first * a + liveWet.first * b;
  wetR = catchWet.second * a + liveWet.second * b;

  if (slipBlendRemaining <= 0.f) {
    readHead = newestPos;
    cancelSlipReturnState();
  }
}
```

### Important

During `slipBlendActive`, do **not** also run any old now-catch or final-catch positional rewrite logic.

---

## 9. Integration points in process flow

## 9.1 Start conditions

Keep the current start conditions conceptually intact:

* release from scratch with slip enabled
* slip enabled while behind NOW
* slip mode changed while behind NOW
* quick slip trigger while behind NOW

But when those conditions fire:

### Do this

```cpp
slipReturning = true;
slipBlendActive = false;
slipCatchVelocity = 0.f;
slipBlendRemaining = 0.f;
```

### Do not do this

Do not initialize:

* `slipFinalCatchActive`
* `slipReturnRemaining`
* `slipReturnStartLag`

Those belong to the removed model.

---

## 9.2 Cancel conditions

Whenever scratch takes over, or sample mode takes over, or another state supersedes slip return:

Call:

```cpp
cancelSlipReturnState();
```

Examples include:

* `anyScratch == true`
* `sampleModeActive == true`
* manual platter re-engagement
* explicit state reset paths

---

## 9.3 Now-catch interaction

Current code has a separate `nowCatchActive` path for tiny residual lag. For the new slip model:

### Required rule

If `slipReturning` or `slipBlendActive` is active:

* do **not** enter `nowCatchActive`
* do **not** run the old now-catch positional rewrite on the same frame

### Simpler policy

Slip blend now replaces the old “tiny-lag cleanup” for slip returns.

Keep `nowCatchActive` available for other non-slip situations if still needed.

---

## 10. Instant mode behavior

## Requirement

`SLIP_RETURN_INSTANT` should still feel effectively immediate, but should avoid the ugliest possible rejoin artifact.

### Behavior

Use the same servo/blend system, but with:

* very large max extra ratio
* very large accel ratio
* same short final blend

This makes it *functionally instant* without forcing a raw discontinuous hard pin unless lag is already negligible.

### Allowed exception

If lag is already below 0.5 samples, immediate pin is allowed.

---

## 11. Preserve sample mode separation

The current code clearly distinguishes sample mode from live buffer transport. Keep that separation intact. Do not use the new servo slip return in sample mode. 

### Requirement

If `sampleModeActive` is true:

* do not enter servo slip return
* do not run slip blend
* keep sample transport behavior unchanged

---

## 12. Preserve public API / UI behavior

Do not change:

* `SLIP_RETURN_SLOW`, `SLIP_RETURN_NORMAL`, `SLIP_RETURN_INSTANT`
* context menu labels and selection flow
* `triggerQuickSlipReturn()`
* slip-latched UI behavior

This is an internal DSP behavior change only. 

---

# Acceptance Criteria

## Audible acceptance

1. **Medium slip release**
   Releasing a manual scratch 100–300 ms behind NOW should produce a smooth catch-up without obvious HF spit or zipper-like brightness.

2. **Large slip release**
   Releasing a scratch far behind NOW should sound like accelerated transport catch-up, not like a chirping resampler.

3. **Arrival at NOW**
   The final 10–20 ms should not produce an obvious click, brittle edge, or abrupt timbral flip.

4. **Instant slip mode**
   Instant mode should still feel immediate to the user, but should be less harsh than a raw positional pin.

---

## Functional acceptance

1. Slip return still triggers from all the same user actions as before.
2. Scratch takeover still cancels slip return immediately.
3. Sample mode behavior remains unchanged.
4. Reverse/freeze logic remains unchanged unless explicitly interacting with slip takeover.
5. User scratch interpolation choice applies to slip return and final blend.
6. No new buffer overrun, wrap, or NOW overshoot behavior is introduced.

---

# Explicit Non-Goals

Do **not** do any of the following in this change:

* rewrite scratch gesture handling
* change sample-mode release semantics
* redesign platter UI or controls
* add new visible parameters or menu items
* change base transport speed mapping
* rewrite cartridge coloration
* rewrite position CV semantics

---

# Suggested Refactor Strategy

## Minimal-risk implementation order

1. Add new state members and constants.
2. Add helper functions:

   * `slipCatchMaxExtraRatio()`
   * `slipCatchAccelRatio()`
   * `cancelSlipReturnState()`
   * `startSlipBlend()`
   * `readInterpolatedAt(...)`
3. Replace old slip return block with `integrateSlipCatchup(...)`.
4. Expand scratch/variable-rate read gating so slip return uses HQ interpolation.
5. Add `slipBlendActive` render path.
6. Remove obsolete old final-catch behavior.
7. Verify all cancel/start conditions compile and behave correctly.

---

# Notes for Codex

## Important architectural constraint

This spec is intended to fit the current `TemporalDeckEngine` structure:

* `readHead` remains the authoritative playback position
* `newestPos` remains the live/NOW reference
* `currentLagFromNewest(newestPos)` remains the source of lag truth
* interpolation mode selection should reuse existing scratch interpolation modes
* slip mode enum should not change

## Preferred coding style

* keep helper functions small and local to `TemporalDeckEngine`
* avoid introducing heap allocations
* avoid creating duplicate interpolation logic paths
* keep the read/render path explicit and readable
* prefer reuse of existing `unwrapReadNearWrite()`, `buffer.wrapPosition()`, and interpolation methods

---

# One-line implementation summary

**Replace positional slip-lag collapse with an acceleration-limited forward catch-up servo, and replace final snap-to-live with a short equal-power live blend.**

