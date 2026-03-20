# TemporalDeck.cpp — Code Review

**Reviewer:** Nexora Lumineth  
**Subject:** VCV Rack Plugin — `TemporalDeck.cpp`

---

## Executive Summary

TemporalDeck is a well-structured, thoughtfully designed VCV Rack delay/scratch module. The overall architecture is clean, the DSP intent is clear, and the audio-rate/UI separation via atomics is correctly handled. The codebase shows genuine domain knowledge — the cartridge character emulation, the slip/freeze/reverse state machine, and the interpolation tier selection all reflect careful design decisions.

The primary area of concern is the hybrid scratch model's UI platter interaction layer, which contains several compounding edge cases that can produce incorrect lag values, unreliable "snap to NOW" behavior, and directional asymmetry under specific gesture sequences. These are documented in detail in Section 3. The underlying engine logic is sound; the issues are largely confined to the widget-to-engine communication boundary.

---

## Architecture Overview

### Component Hierarchy

The module decomposes cleanly into three layers:

- **`TemporalDeckBuffer`** — ring buffer with cubic, linear, and 6-point Lagrange read modes
- **`TemporalDeckEngine`** — all DSP: state machine, scratch models, cartridge EQ/saturation, platter animation
- **`TemporalDeck` (Module)** — parameter management, atomic UI bridge, latch logic
- **Widget layer** — `TemporalDeckPlatterWidget`, `TemporalDeckDisplayWidget`, `TemporalDeckTonearmWidget`

This separation is architecturally sound. The engine exposes a single `process()` call that takes a flat set of inputs and returns a `FrameResult`, keeping DSP logic completely isolated from VCV framework concerns.

### Threading Model

The module correctly uses `std::atomic<>` for all cross-thread state (`platterTouched`, `platterGestureRevision`, `uiLagSamples`, etc.). The audio thread reads atomics and writes results back; the UI thread posts gesture data and reads display state. No mutexes are needed and none are used — appropriate for this pattern.

One subtlety worth noting: `platterWheelDelta` uses a load/add/store pattern in `addPlatterWheelDelta()` rather than `fetch_add()`. This is not a data race per se since VCV's UI events are single-threaded, but it is worth documenting as an assumption.

---

## Platter Interaction: Hybrid Model Analysis

This section covers the physical UI platter interactions in the hybrid scratch model. Findings are ordered from most to least severe.

---

### 🔴 BUG — Velocity Calculation Uses Pixel Ratio, Not Angular Velocity

**Location:** `TemporalDeckPlatterWidget::updateScratchFromLocal()`

**Issue:** The velocity passed to `setPlatterScratch()` is computed as:

```cpp
float velocity = tangentialPx * module->uiSampleRate.load() * 0.0007f * sensitivity * radiusRatio;
```

This is a pixel-space heuristic, not a true angular velocity derived from the physical platter model. The magic constant `0.0007f` has no documented relationship to `kMouseScratchTravelScale` or `kNominalPlatterRpm`. In the hybrid engine, `platterGestureVelocity` is used directly as `targetReadVelocity` (in samples/sec), so a unit mismatch here directly corrupts the follow dynamics.

**Action:** Derive velocity from the same physical model used for position:

```cpp
float velocity_sps = (tangentialPx / effectiveRadius) * samplesPerRadian / dt;
```

`dt` can be estimated from the UI frame rate or tracked via a timestamp at the widget level. This makes the velocity dimensionally consistent with what `integrateHybridScratch()` expects.

---

### 🔴 BUG — Click-Release / Drag-End Race on Fast Click

**Location:** `TemporalDeckPlatterWidget::onButton()`, `onDragEnd()`

**Issue:** If the user clicks and releases without triggering a `DragStart` (a fast click), the release is handled in `onButton()` with `action == GLFW_RELEASE`. However, `onButton()` fires before `onDragEnd()` in VCV's event pipeline when dragging is active. There is also an inverse case: a very fast drag start+stop can fire `DragStart → DragEnd` without an intervening `onDragMove`, leaving `dragging = true` but `platterTouched = false`, while a subsequent `onButton RELEASE` sees `dragging == false` and calls `setPlatterScratch(false)` again — doubling the release signal.

**Action:** Consolidate release logic into a single `releaseGesture()` helper and call it from both `onButton` (non-drag path) and `onDragEnd`. Add a guard: if `(!dragging && platterTouched not set)` return early from the `onButton` release handling.

---

### 🔴 BUG — Stale `contactAngle` After Sample Rate Change

**Location:** `TemporalDeckPlatterWidget` — `contactAngle`, `contactRadiusPx` fields

**Issue:** `contactAngle` and `contactRadiusPx` are set in `onDragStart()` and accumulated in `updateScratchFromLocal()`. If a sample rate change fires mid-drag (`onSampleRateChange` resets the engine), `contactAngle` retains the old drag state. The next `updateScratchFromLocal()` call computes a `deltaAngle` relative to a now-stale contact origin, producing a large spurious lag jump.

**Action:** When `onSampleRateChange()` fires (or when `setPlatterScratch` is called with `touched=false`), also reset `dragging = false`, `contactAngle = 0`, `contactRadiusPx = 0` on the widget. Since `onSampleRateChange()` runs on the audio thread and widget fields are UI-thread data, the safest approach is an `atomic<bool> resetPending` that the widget checks at the start of `onDragMove`.

---

### 🔴 BUG — `nowCatchActive` Blocks Lag Target Update for One Frame

**Location:** `TemporalDeckEngine::process()` — hybrid `manualTouchScratch` branch

**Issue:** When `nowCatchActive` is true and a new platter gesture arrives with `platterLagTarget > nowSnapThresholdSamples`, the code correctly cancels the now-catch. However, there is a window of one audio-thread `process()` cycle where both `nowCatchActive == true` AND the new lag target has been written to `scratchLagTargetSamples`. The now-catch block later in the function then overwrites `scratchLagTargetSamples = targetLag` before the flag has been cleared. The net effect is a one-frame ghost snap toward zero — manifesting as a small audible click when grabbing the platter quickly after releasing it near NOW.

**Action:** Reorder the now-catch cancellation to happen before the lag target assignment. The one-liner fix: move the cancellation check to the top of the `hasFreshPlatterGesture` branch, before writing `scratchLagTargetSamples`.

---

### 🟠 WARNING — Forward Scratch Baseline Offset Applied Asymmetrically

**Location:** `integrateHybridScratch()` — `targetReadVelocity` adjustment

**Issue:** The line `targetReadVelocity += sampleRate` (to outrun the write head on forward scratches) is only applied in the `manualTouchScratch` branch, not in the `wheelScratch` branch that calls the same `integrateHybridScratch()`. Wheel scratch passes `targetReadVelocity = 0.f` unconditionally, relying purely on the correction term. This creates a noticeable asymmetry: forward touch scratches feel snappy while forward wheel returns feel sluggish near NOW.

**Action:** Either apply the same baseline in the wheel path (pass `targetReadVelocity = sampleRate` when `lagError < 0`), or explicitly document that wheel scratch intentionally uses a different forward-return model.

---

### 🟠 WARNING — Wheel Delta Accumulation Not Atomic `fetch_add`

**Location:** `TemporalDeck::addPlatterWheelDelta()`

**Issue:** `platterWheelDelta` accumulates scroll events via load/add/store rather than a true atomic read-modify-write. Under rapid concurrent scroll events (fast trackpad, high-DPI), this can lose delta updates (last write wins). `std::atomic<float>` does not provide `fetch_add`, so this requires an explicit workaround.

**Action:** Use a compare-exchange loop for the accumulation, or switch to `std::atomic<int>` with fixed-point encoding (e.g. scaled by 1000). Also consider capping the stored delta at a maximum reasonable value to prevent extreme single-frame jumps.

---

### 🟠 WARNING — `platterMotionFreshSamples` Countdown Is Frame-Rate Coupled

**Location:** `TemporalDeck::process()` — `platterMotionFreshSamples` countdown

**Issue:** The "motion is fresh" window is set to `int(round(sampleRate * 0.02f))` samples by the UI thread using `uiSampleRate.load()`. If `uiSampleRate` lags behind a sample rate change, the countdown will be miscalibrated for that transition. Additionally, for high sample rates (192kHz), 20ms is 3840 samples — a long hold window for a fast staccato gesture.

**Action:** Store the desired duration in seconds rather than samples and convert on the audio thread using `args.sampleRate`. A helper `setPlatterMotionFreshSeconds(float sec)` that converts in `process()` handles sample rate transitions cleanly.

---

### 🟠 WARNING — Slip/`nowCatch` Mutual Exclusion Is Order-Dependent

**Location:** `TemporalDeckEngine::process()` — state machine

**Issue:** There are multiple places where `slipReturning` is cleared but `nowCatchActive` is not (and vice versa). The primary guard runs after the scratch branch, but the `nowCatchActive` block also fires in the same frame — both can assign to `readHead` in a single tick. The effective value is whichever executes last (consistently `nowCatch`, as it appears later in `process()`). This ordering dependency is fragile and undocumented.

**Action:** Add explicit mutual exclusion: `if (nowCatchActive && anyScratch) nowCatchActive = false;` early in `process()`. Document the intended priority order (scratch > nowCatch > slip > normal) as a comment block at the top of the readhead decision section.

---

### 🔵 NOTE — `microStepAmt` Threshold Undocumented

**Location:** Hybrid de-click section

**Issue:** The threshold `0.95f` in `clamp((0.95f - fabs(readDeltaForTone)) / 0.95f)` has an implicit relationship to normal 1x playback (where `readDelta ≈ 1.0`). This is sample-rate independent and the behavior is correct, but the intent is non-obvious to a future reader.

**Action:** Add a comment: `// Active only for sub-sample steps; 1x playback yields readDelta ~= 1.0, making this zero as intended.`

---

### 🔵 NOTE — SVG Parse Failure Is Silent

**Location:** `loadSvgCircleMm()` / `loadPlatterAnchor()`

**Issue:** If `res/deck.svg` is missing or the `PLATTER_AREA` circle element is malformed, `loadPlatterAnchor` returns false and the platter silently falls back to hardcoded defaults. Users will see a subtly wrong platter hitbox with no diagnostic output.

**Action:** Add a `WARN_LOG` or similar when `loadPlatterAnchor` returns false. Also consider declaring the regex as a `static const` local to avoid recompilation on repeated calls (though it is only called once at construction).

---

### 🔵 NOTE — Debug Overlay Left in Draw Code

**Location:** `TemporalDeckDisplayWidget::draw()`

**Issue:** A substantial debug overlay (`mouseText`, `motionText`, debug position rendering) is commented out in a block comment in the draw path.

**Action:** Either remove and rely on git history, or wrap in a `#ifdef TEMPORAL_DECK_DEBUG` guard so it can be re-enabled cleanly during development.

---

### ✅ PRAISE — Tangential-Only Drag Model

**Location:** `TemporalDeckPlatterWidget::updateScratchFromLocal()`

The physical screen-space model is well-conceived: contact radius and angle are locked at drag start, and only the tangential component of subsequent mouse movement drives the platter. Radial cursor drift is correctly ignored. The comment explaining the two regressions from the previous approach (click-and-hold drift, slow reverse creep) is exactly the kind of context future maintainers need. Consider extracting the radial/tangential decomposition into a named `PlatterContact` struct if a hardware controller input path is added later.

---

### ✅ PRAISE — Hybrid Velocity Deadband Zeroing

**Location:** `integrateHybridScratch()`

The four-condition deadband check (`scratchHandVelocity`, `scratchMotionVelocity`, `lagError`, and `desiredVelocity` all below threshold) before zeroing both velocities is correctly formulated. Using all four prevents premature zeroing during a slow drift toward a non-zero target, while still providing a clean rest state when genuinely parked. Many implementations only check one or two conditions and produce audible tails.

---

## Finding Summary

| Severity | Finding | Area |
|---|---|---|
| 🔴 BUG | Velocity units mismatch (pixel vs. samples/sec) | Hybrid scratch UX |
| 🔴 BUG | Click-release / drag-end race condition | Widget events |
| 🔴 BUG | Stale `contactAngle` after sample rate change | Widget state |
| 🔴 BUG | `nowCatch` blocks lag target update for one frame | Engine state machine |
| 🟠 WARNING | Forward scratch baseline offset asymmetry | Hybrid engine |
| 🟠 WARNING | Wheel delta accumulation not atomic `fetch_add` | Thread safety |
| 🟠 WARNING | `platterMotionFreshSamples` is frame-rate coupled | UI/audio bridge |
| 🟠 WARNING | Slip/nowCatch exclusion is order-dependent | Engine state machine |
| 🔵 NOTE | `microStepAmt` threshold undocumented | Hybrid de-click |
| 🔵 NOTE | SVG parse failure is silent | Initialization |
| 🔵 NOTE | Debug overlay in draw path | Code hygiene |
| ✅ PRAISE | Tangential-only drag model | Widget physics |
| ✅ PRAISE | Hybrid velocity deadband zeroing | Engine quality |

---

## Recommended Fix Priority

1. `nowCatch` blocks lag target update — one-line fix, highest impact-to-effort ratio
2. Click-release / drag-end race — consolidate release logic
3. Velocity units mismatch — requires deriving `dt` at widget level
4. Forward scratch baseline asymmetry — touch vs. wheel feel consistency
5. Stale `contactAngle` on sample rate change — edge case but audible when it fires
6. Wheel delta atomic consistency — correctness issue, not yet reported as a crash

---

## Additional DSP Notes

### Interpolation Tier Selection

The three-way dispatch (linear / cubic / 6-point Lagrange) based on `scratchReadPath` and `highQualityScratchInterpolation` is well-designed. The Lagrange implementation is correct; the node layout `{-2,-1,0,1,2,3}` with `t` in `[0,1)` maps naturally onto the buffer indexing. If users report subtle aliasing during fast rate sweeps in normal transport, consider gating the high-quality path on `|speed| > 1.5` as well.

### Cartridge Model

The cartridge EQ/saturation model is clean and well-parameterized. The motion-dependent LP corner (`lpHz` blended with `lpMotionHz` by `motionAmount`) is an elegant physical approximation of stylus dulling under speed. The lofi path's wow/flutter using deterministic sinusoids rather than a random walk is a reasonable CPU trade-off.

One consideration: the `motionAmount` scaling factor of `0.4` applied in the scratch path (to reduce cartridge darkening) interacts with the LOFI cartridge's `lofiBlend`, which also uses `motionAmount`. Under heavy scratching with LOFI selected, the effective `lofiBlend` range is compressed to `0.28..0.41` instead of `0.28..0.62`. This may feel inconsistent with non-scratch LOFI playback tone — worth being aware of subjectively.

### Arc LED Metering

The `lagRatio` / `limitRatio` arc light logic is correct and the dual-layer (yellow lag indicator + red limit marker) is a good UX pattern. The 120Hz publish rate is appropriate. Note that `uiLagSamples` is written at audio rate outside the publish timer — this is intentional and correct, since the widget's drag math depends on it being current.

---

## Closing

TemporalDeck is genuinely impressive work for a VCV module. The hybrid scratch model is architecturally the right direction — velocity-first integration with lag as the authoritative state is exactly how physical platter simulation should work. The bugs found are the kind that emerge from the difficulty of bridging sparse UI events to a continuous audio model, not from architectural mistakes. With the four BUG-level fixes in place, the hybrid platter should feel substantially more reliable under the edge cases that have been causing pain.

*The field is open. Good luck with the release.*
