# DSP Review: TemporalDeck "Hybrid" Scratch Implementation

This document provides a deep-dive analysis into the current "Hybrid" scratching model. While more sophisticated than the legacy approach, it still contains the fundamental architectural "anchoring" error that causes it to sound inferior to the direct Transport/Reverse logic.

## 1. The Reference Frame Fallacy (The "Moving Zero")

The core issue remains: **The definition of "Zero Velocity" is inconsistent across the engine.**

### Normal Transport (Velocity-Anchored)
- **Math:** `readHead = prevReadHead + speed`.
- **Reference:** The stationary audio buffer.
- **Behavior:** If `speed = 0`, the playhead stops. If `speed = -1`, it reverses cleanly.

### Hybrid Scratch (Lag-Anchored)
- **Math:** `readHead = newestWritePos - integratedLag`.
- **Reference:** The *moving* write head.
- **The Problem:** In this model, if the integrated velocity is `0`, the playhead **moves at Speed 1.0** (following the write head). 
- To achieve a "Freeze" (Speed 0), the integration engine must perfectly maintain a velocity of exactly `1.0` (relative to the write head). 
- **The "Grind" Source:** Any tiny error in the integration, damping, or spring-correction (from sparse UI events) manifests as a frequency-modulated "jitter" because the engine is constantly trying to "subtract out" the write-head's movement to stay stationary.

## 2. Issues with the Hybrid Motion Model

### A. The "Spring-Mass-Damper" Conflict
The hybrid model (in `integrateHybridScratch`) implements a second-order physical system:
1.  **Velocity Integration:** `lag += velocity * dt`.
2.  **Spring Correction:** `correction = lagError * kHybridScratchCorrectionHz`.
3.  **Damping:** `kHybridScratchVelocityDampingHz`.

Because the UI sends "Lag Target" updates at a low frequency (~60Hz), the "Spring" is constantly being jerked by sparse updates. These jerks are then integrated into the velocity. The resulting oscillation is the "needle-grind" you hear.

### B. The 0.95x Under-Prediction
The variable `kManualVelocityPredictScale = 0.95f` is still present in the hybrid path. 
- By intentionally predicting only 95% of the gesture velocity, the engine creates a constant "lag drift."
- Every time a new UI event arrives (the remaining 5%), the "Spring" (CorrectionHz) has to snap the playhead back to the true position.
- This creates a 60Hz "sawtooth" velocity profile, which is highly audible as a tonal buzz during slow scratches.

### C. Reference Frame Complexity
The `integrateHybridScratch` function is trying to model physical inertia, but it's doing so in a coordinate system that is itself moving at Speed 1.0. This makes the constants (`followHz`, `correctionHz`, `damping`) extremely difficult to tune because their behavior changes relative to the buffer's write speed.

## 3. The Path to "Clean" Scratching (The "Real" Hybrid)

To get scratching to sound as clean as the Reverse button, you must **stop scratching the Lag and start scratching the Speed.**

### Proposed Architecture:
1.  **Discard the Lag-relative math during active scratching.**
2.  When a scratch starts, capture the current `initialLag`.
3.  Calculate a `targetSpeed` from the mouse/wheel delta (e.g., -2.0 for fast back, 0.0 for hold).
4.  **Use the Transport Math:** `readHead = prevReadHead + integratedSpeed`.
5.  If you need to ensure the scratch stays near the mouse cursor (Absolute Positioning), apply a tiny corrective force to the `speed` variable, not the position.

### Why this works:
- If you hold the mouse still, `targetSpeed` becomes `0`.
- The `integratedSpeed` ramps to `0`.
- The playhead stops moving relative to the buffer.
- **Result:** You get the same "dead silent" freeze as the Freeze button, with zero jitter from the write head.

## Conclusion
The current "Hybrid" model is over-complicated because it is a "Lag-fixup" engine. It spends most of its mathematical effort trying to negate the fact that its zero-point is moving. By switching to a **Velocity-First** model where the mouse deltas drive the transport's `speed` parameter directly, you will eliminate 90% of the DSP complexity and 100% of the "grind."
