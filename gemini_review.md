# DSP Review: TemporalDeck Scratching vs. Transport

This document outlines the findings from an architectural review of the `TemporalDeck` plugin, specifically focusing on the discrepancy between the "clean" sound of the Reverse button and the "grindy" or inconsistent sound of manual scratching.

## 1. The Core Architectural Discrepancy

The primary issue is that the engine uses two completely different mathematical models for playback:

### Transport Path (Clean)
Used for Play, Reverse, and Freeze.
- **Model:** Velocity-based integration.
- **Math:** `readHead = prevReadHead + speed`.
- **Result:** Perfectly smooth phase increments. No jitter, no dependence on the write head's position.

### Scratch Path (Grind)
Used for Mouse and Wheel scratching.
- **Model:** Delay-based relative positioning.
- **Math:** `readHead = newestWritePos - lagSamples`.
- **The Problem:** Because `newestWritePos` increments every sample, `readHead` *also* increments every sample by default. To "freeze" audio during a scratch, the logic must perfectly increment `lagSamples` by 1.0 every sample. Any jitter in the `lagSamples` calculation (from UI events or smoothing filters) manifests as immediate frequency modulation (jitter/grind).

## 2. Key Issues Identified

### A. The "Stationary Scratch" Discontinuity
In the current code, if you hold the platter still during a drag, the engine defaults to Speed 1.0 (following the write head) because `lagSamples` stops changing. 
- There is a `stationaryManualHold` hack that forces `readHead = prevReadHead` (Speed 0), but it only kicks in when the UI stops sending events.
- This creates a "fighting" sensation where slow movements or pauses during a scratch jump between Speed 1.0 and Speed 0.0.

### B. Oversimplified Prediction & Inertia
The inertia model (`kScratchInertiaFollowHz`, etc.) and the prediction scale (`kManualVelocityPredictScale = 0.95f`) are attempting to smooth out sparse UI events (usually 60Hz or less) into audio-rate (44.1kHz+) movement.
- Using a 0.95 scale means the engine intentionally "under-predicts" by 5%, causing a constant drift back towards Speed 1.0 that the user has to "fight" during slow reverse scratches.
- The inertia model adds a "rubbery" phase lag that makes the audio feel disconnected from the mouse movement.

### C. Heuristic "Band-Aids"
The scratch path is burdened with several non-linear processes that the Reverse button bypasses:
- **`scratchFlipTransientEnv`**: Manually injects "clicks" on direction changes.
- **`scratchDcOut`**: A high-pass filter that only activates during slow reverse glides.
- **`deClickAmt`**: Constant crossfading with previous samples.
- **`grindAmt`**: An adaptive smoothing layer that tries to hide the "needle-grind" caused by the jittery lag tracking.

These heuristics are likely fighting each other, making the DSP difficult to tune.

## 3. Recommended Improvements

### I. Unify the Path (Velocity-Based Scratching)
Instead of scratching controlling a **Position Lag**, it should control the **Playback Speed**.
- Mouse deltas should be converted into a target `scratchSpeed`.
- The engine should smoothly interpolate the `speed` variable towards the `targetSpeed`.
- Use the same `readHead = prevReadHead + speed` logic for EVERYTHING. This ensures that a scratch at speed -1.0 sounds identical to the Reverse button.

### II. Improve UI-to-Audio Bridge
- **Timestamps:** Use the time elapsed between UI events to calculate a more accurate velocity.
- **Linear Ramp:** Instead of an inertia/spring model, use simple linear interpolation of the velocity over the expected UI frame time.

### III. Remove Synthetic Transients
A turntable model shouldn't need a "transient generator." If the `readHead` movement is physically modeled correctly (with proper interpolation), the natural "bite" and "grind" of the audio will emerge from the sample-rate conversion itself.

### IV. Buffer-Relative Positioning (Optional)
If absolute positioning is required (e.g., for "Slip" or "Position CV"), the `readHead` should still be updated via velocity, but with a corrective "spring" term that gently pulls it toward the target position, rather than snapping to it via `newestPos - lag`.

## Conclusion
The engine is currently "over-processed" because it's trying to fix a fundamental math error: tracking a static point in a moving reference frame. By switching to a unified velocity-based model, most of the complex de-clicking and smoothing logic can be removed, resulting in a much cleaner and more "analog" sound.
