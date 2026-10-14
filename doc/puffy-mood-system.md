# Puffy Organic Mood & Unified Behavior System Specification

> Status: Detailed architectural recommendation and implementation specification for Puffy unreleased module.
> 
> Module name and Rack slug: `Puffy`.
> 
> Companion documents: [puffy.md](file:///mnt/c/msys64/home/Plasm/Leviathan/doc/puffy.md), [puffy-s5-notes.md](file:///mnt/c/msys64/home/Plasm/Leviathan/doc/puffy-s5-notes.md), [puffy-s5-confirmation.md](file:///mnt/c/msys64/home/Plasm/Leviathan/doc/puffy-s5-confirmation.md).

---

## 1. Executive Summary & Core Philosophy

Puffy operates across three distinct operational layers:
1. **Audio & DSP Engine**: Saturation waveshaping, sensitivity scaling, stereo linking, and peak limiters (`PuffyEngine.hpp`).
2. **Visual Pose & Animation**: Physical inflation spring, transient twitches, winking, blinking, and gain-reduction blushes (`PuffyCharacterController.hpp`).
3. **Spatial Roaming System**: VCV Rack canvas exploration, mouse repulsion, ID-seeded wander, and bungee tethering ([`PuffyWidget.cpp`](file:///mnt/c/msys64/home/Plasm/Leviathan/src/PuffyWidget.cpp#L600-L905)).

While the separation between DSP and UI guarantees audio determinism and stability, Puffy's visual and spatial behavior currently suffers from minor disconnects:
- **Gaze Disconnect**: Puffy's eyes gaze pseudo-randomly left and right even when swimming at high speed or fleeing a mouse cursor.
- **Expression Disconnect**: Spatial acceleration speeds up fin fluttering, but facial expressions (squint, smile, excitement) respond exclusively to audio energy and limiter reduction, leaving Puffy looking calm and unbothered while fleeing panic-inducing cursor movements.
- **Panel Feedback Gap**: The panel compass needle indicates Puffy's direction, but lacks distance or state feedback.

This document defines the **Unified Organic Mood System** for Puffy. It harmonizes DSP reactivity, facial/body expressions, and spatial roaming mechanics into a single, cohesive character model without breaking audio determinism or non-blocking UI contracts.

---

## 2. Architecture & Data Flow

The unified mood model strictly preserves the boundary between audio DSP and UI rendering:

```text
 ┌─────────────────────────────────────────────────────────────────────────────┐
 │                            Audio Engine (DSP)                               │
 │ • Audio processing & parameter calculation (Sample Rate)                   │
 │ • PuffyDynamicsDetector updates (1 ms attack / 45 ms release)               │
 │ • Limiter gain reduction computation                                        │
 └──────────────────────────────────────┬──────────────────────────────────────┘
                                        │ (Control-rate atomic snapshot ~240 Hz)
                                        ▼
 ┌─────────────────────────────────────────────────────────────────────────────┐
 │                         PuffyVisualState Snapshot                           │
 │ • effectiveAmount, inputActivity, transientActivity, gainReduction          │
 │ • positiveInputActivity, negativeInputActivity                              │
 │ • roamingMovementAcceleration, roamingDistance, spatialVelocity (New)       │
 └──────────────────────────────────────┬──────────────────────────────────────┘
                                        │ (Evaluated at 30 Hz GUI rate)
                                        ▼
 ┌─────────────────────────────────────────────────────────────────────────────┐
 │                      PuffyCharacterController                               │
 │ 1. Evaluates Mood State Weights (Serene, Energized, Bracing, Startled)       │
 │ 2. Computes Spatial Gaze (Velocities & Cursor Repulsion)                     │
 │ 3. Synthesizes Pose (Inflation, Squint, Fin Angles, Blush, Mouth, Spikes)    │
 └──────────────────────────────────────┬──────────────────────────────────────┘
                                        │ (Emits renderer-neutral PuffyPose)
                                        ▼
 ┌──────────────────────────────────────┴──────────────────────────────────────┐
 │                            Renderers (NanoVG)                               │
 │ • Static Panel Viewport (Compass needle + distance indicator when roaming)  │
 │ • Detached Roaming Overlay (Floating puffer avatar on VCV Rack canvas)     │
 └─────────────────────────────────────────────────────────────────────────────┘
```

---

## 3. The 4-State Organic Mood Matrix

Instead of treating audio features and spatial physics as isolated inputs into discrete animation functions, `PuffyCharacterController` continuously evaluates four continuous **Mood Weights** ($0.0 \dots 1.0$) that smoothly blend pose outputs.

### 3.1 Mood Weight Derivation

Let $A_{\text{eff}}$ be `effectiveAmount`, $I_{\text{act}}$ be `inputActivity`, $T_{\text{act}}$ be `transientActivity`, $G_{\text{red}}$ be `gainReduction`, $S_{\text{acc}}$ be `roamingMovementAcceleration`, and $D_{\text{ratio}}$ be `roamingDistanceRatio`.

$$\begin{aligned}
W_{\text{Serene}} &= \text{clamp01}\left(1.0 - 1.2 \cdot I_{\text{act}} - 0.8 \cdot A_{\text{eff}} - S_{\text{acc}}\right) \\
W_{\text{Energized}} &= \text{clamp01}\left(0.65 \cdot A_{\text{eff}} + 0.35 \cdot I_{\text{act}} + 0.4 \cdot T_{\text{act}}\right) \\
W_{\text{Bracing}} &= \text{clamp01}\left(1.5 \cdot G_{\text{red}}\right) \\
W_{\text{Startled}} &= \text{clamp01}\left(1.4 \cdot S_{\text{acc}} + 0.6 \cdot T_{\text{act}} - 0.2 \cdot W_{\text{Bracing}}\right)
\end{aligned}$$

Each mood smoothly approaches its target with distinct attack and decay rates to simulate physical body inertia and emotional transitions.

### 3.2 Mood Pose Matrix

| Feature | Serene / Idle | Energized / Puffed | Bracing / Protective | Startled / Fleeing |
| :--- | :--- | :--- | :--- | :--- |
| **Inflation** | Low baseline ($0.05 \dots 0.20$), micro-breathing | High inflation ($0.60 \dots 1.0$), spring overshoot | Moderately compressed, tight pressurized sphere | Rapid inflation pulse followed by jitter |
| **Eye Shape** | Soft open lids, calm glances | Wide open, bright gaze, occasional excitement squint | Heavy asymmetric squint, winking on polarity dominance | Wide startled eyes, pupils dilated |
| **Gaze Behavior** | Doom-style casual left/right sequence | Rapid glances towards active input channels | Locked forward / downward brace | Snaps forward along velocity vector or back at fleeing cursor |
| **Mouth** | Small relaxed smile, periodic closure sequence | Wide happy smile | Tightened mouth line (`mouthTension` high) | O-shaped startled mouth / small gasp |
| **Fins** | Gentle rhythm flutter | Active flutter, wide angle extension | Tucked slightly inward for protection | Frantic fast flapping (`movementFinActivity` high) |
| **Spines** | Relaxed, flat against body | Fully extended outward | Rigid, stiff extension | Jittery micro-extension |
| **Color & Tint** | Pure character base color | Brightened character tint + highlight | Warm coral/amber blush overlay | Pale transient flash / highlight pulse |

---

## 4. Detailed Technical Improvements

### 4.1 Spatial Gaze Integration

Currently, `gazeX` is updated via a timer-based doom-style sequence in `PuffyCharacterController.cpp`:

```cpp
// Existing logic: static glance sequence
gazeX = approach(gazeX, gazeTargetX, gazeGlancing ? 18.f : 11.f, dt);
```

#### Proposed Enhancement: Velocity & Cursor Spatial Gaze
When roaming is active (`roamingAvatarActive == true`), gaze becomes spatial-aware:

1. **Velocity-Aligned Gaze**: When moving steadily ($S_{\text{acc}} > 0.15$), `gazeTargetX` shifts towards the normalized horizontal velocity component $V_x$.
2. **Cursor Repulsion Glancing**: When fleeing the mouse ($S_{\text{acc}} > 0.45$), Puffy glances back over his shoulder toward the fleeing cursor (opposite to movement vector) for brief $0.3\text{ s}$ pulses to show awareness of the threat.
3. **Doom-Style Fallback**: When stationary ($S_{\text{acc}} \le 0.15$), gaze reverts to the default casual glance sequence.

```cpp
if (visual.roamingAvatarActive && spatialSpeed > 0.15f) {
    if (isFleeingMouse) {
        // Look back towards the cursor nervously
        gazeTargetX = clamp01(-normalizedVelocityX * 0.85f);
        gazeTargetY = clamp01(-normalizedVelocityY * 0.60f);
    } else {
        // Look ahead in direction of travel
        gazeTargetX = clamp01(normalizedVelocityX * 0.75f);
        gazeTargetY = clamp01(normalizedVelocityY * 0.50f);
    }
}
```

### 4.2 Startled Fleeing Physical Expressions

When the user moves their cursor near Puffy during Roaming Mode, `PuffyRoamingOverlay` applies repulsion force. We map this spatial force into visual pose parameters:

1. **Squash & Stretch Impulse**: When a strong flee force is applied, trigger a short directional squash along the movement axis ($\text{squashX} = +0.08 \cdot S_{\text{acc}}$, $\text{squashY} = -0.06 \cdot S_{\text{acc}}$).
2. **Startled Eye Pupil Shrink**: High $S_{\text{acc}}$ scales pupil size down slightly ($0.85\times$) while opening eyelids wider, creating a distinct "startled" look.
3. **Mouth Gasp**: When $S_{\text{acc}} > 0.5$, transition `mouthClosure` and `mouthTension` to create a small open "gasp" shape rather than a wide relaxed smile.

### 4.3 Panel Viewport Compass & Distance Indicator

When Puffy is roaming, his panel viewport displays a directional needle ([`PuffyFishWidget.cpp:495-504`](file:///mnt/c/msys64/home/Plasm/Leviathan/src/PuffyFishWidget.cpp#L495-L504)). We enhance this viewport indicator:

1. **Distance Proportional Needle**:
   - Scale needle size from $0.6\times$ (near home) to $1.2\times$ (max range).
   - Pulse needle opacity at a rate proportional to `roamingDistance`.
2. **State Color Coding**:
   - **Coral/Red** (`#FF6450`): Normal roaming.
   - **Bright Amber / Yellow** (`#FFB81C`): Puffy is excited / high audio drive.
   - **Cyan / Pulsing Blue**: Puffy is exploring new cells on the outer perimeter.
3. **Mini-Puffy Silhouette (Optional)**:
   - At distance $> 500\text{ px}$, draw a tiny 8px translucent puffer icon at the needle tip.

---

## 5. Implementation Roadmap

### Phase 1: Controller & State Struct Updates
- Update `PuffyVisualState` in `Puffy.hpp` to include `spatialVelocityX`, `spatialVelocityY`, `isFleeingMouse`, and `roamingDistanceRatio`.
- Add mood weight smoothing variables ($W_{\text{Serene}}$, $W_{\text{Energized}}$, $W_{\text{Bracing}}$, $W_{\text{Startled}}$) to `PuffyCharacterController.hpp`.

### Phase 2: Mood System Integration in `PuffyCharacterController.cpp`
- Implement mood weight equations and smooth step transitions.
- Integrate spatial gaze calculation into `update()` method.
- Update `pose->leftFinAngle`, `pose->rightFinAngle`, `pose->leftBlink`, `pose->rightBlink`, and mouth parameters to reflect combined mood weights.

### Phase 3: Roaming Overlay & Panel Viewport Polish
- Update `PuffyRoamingOverlay::step()` in `PuffyWidget.cpp` to calculate and store normalized spatial velocity and fleeing state into `Puffy`.
- Update `PuffyFishWidget::draw()` in `PuffyFishWidget.cpp` to render the distance-enhanced panel compass.

### Phase 4: Verification & Acceptance Testing
- **Audio Integrity**: Verify zero allocations, zero lock contention, and zero DSP behavior changes in `puffy_engine_spec`.
- **Visual Polish**: Verify smooth transitions between static panel display and roaming overlay, checking for zero visual pops or eye flickering.
- **Performance Gate**: Ensure frame rates remain $\ge 60\text{ FPS}$ with zero UI thread stalls.

---

## 6. Summary of Benefits

1. **Enhanced Character Expressiveness**: Puffy reacts authentically to both what he hears (audio dynamics) and where he is (spatial environment).
2. **Unified Codebase Structure**: Replaces isolated visual tweaks with a single mathematical mood matrix in `PuffyCharacterController`.
3. **Zero Audio Impact**: Maintains absolute realtime safety and patch determinism across all VCV Rack workflows.
