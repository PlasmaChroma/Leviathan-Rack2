# Temporal Deck — Full Implementation Specification

Version 1.0
Target: VCV Rack 2
Width: 20HP (101.6 mm)
Height: 128.5 mm

Panel file: `deck.svg`

---

# 1. Conceptual Model

Temporal Deck is a **stereo time-buffer instrument** inspired by the physical behavior of a DJ turntable.

Unlike a traditional delay module, Temporal Deck exposes a **continuous recording of recent audio** as a playable surface.

The user interacts with time itself through a **virtual platter**, allowing them to:

* scratch the audio
* slow or accelerate playback
* freeze time
* temporarily slip away from the current timeline

The module continuously records audio into a circular buffer while a playback head moves through that recorded history.

Two primary pointers define the engine:

```
writeHead  → present moment (incoming audio location)
readHead   → playback location
```

At normal operation:

```
readHead follows writeHead
```

but it may move backward when the user interacts with the module.

The system enforces:

```
readHead ≤ writeHead
```

except during very brief interpolation states.

This ensures the module **never plays audio that has not yet been recorded**.

---

# 2. Buffer System

## 2.1 Fixed Memory Allocation

Temporal Deck allocates a fixed circular buffer during module initialization.

```
bufferLengthSeconds = 8
bufferLengthSamples = sampleRate * 8
```

This memory is **never resized**.

Reasons:

* avoids audio thread memory allocation
* ensures stable latency
* simplifies wrap logic

---

## 2.2 Circular Buffer Layout

The buffer stores stereo samples.

Conceptually:

```
bufferL[bufferSize]
bufferR[bufferSize]
```

Each index contains a single sample frame.

Indices wrap around using modulo arithmetic.

```
index = (index + 1) % bufferSize
```

---

## 2.3 Write Head

The write head always represents **NOW**.

Every audio frame:

```
bufferL[writeHead] = inputL
bufferR[writeHead] = inputR
writeHead++
```

Wrap occurs automatically when reaching the buffer end.

---

# 3. Accessible Buffer Range

Although the module stores 8 seconds of history, the user can choose to access only a portion of it.

This is controlled by the **BUFFER knob**.

```
BUFFER knob range: 0.0 → 1.0
```

Mapping:

```
maxLagSeconds = BUFFER * 8
```

Example:

| BUFFER | accessible history |
| ------ | ------------------ |
| 0.25   | 2 seconds          |
| 0.5    | 4 seconds          |
| 1.0    | 8 seconds          |

Important:

The buffer memory remains fully populated regardless of this setting.
The knob simply limits how far the read head may travel backward.

---

# 4. Read Head Transport

The read head determines what audio the user hears.

Every sample frame:

```
readHead += playbackSpeed
```

Where playbackSpeed is determined by:

```
RATE knob
RATE CV
scratch gestures
slip logic
```

---

# 5. Playback Speed

Playback speed represents **how quickly the read head advances through time**.

```
speed = 1.0 → normal playback
speed = 0.5 → half speed
speed = 2.0 → double speed
```

---

## 5.1 RATE Knob

The RATE knob controls base playback speed.

Suggested mapping:

```
knob = 0.5 → speed = 1.0
knob = 0.0 → speed = -2.0
knob = 1.0 → speed = +2.0
```

Negative speeds allow reverse playback.

---

## 5.2 RATE CV

RATE CV modulates playback speed.

Mapping:

```
0V → no change
+5V → +1.0 speed
-5V → -1.0 speed
```

Final speed:

```
speed = clamp(rateKnob + rateCVOffset, -3.0, +3.0)
```

---

# 6. Platter Interaction

The platter is a large circular interaction surface defined by:

```
PLATTER_AREA
```

in the SVG.

The platter behaves like a physical vinyl record.

---

## 6.1 Mouse Gesture Model

The system interprets mouse movement relative to platter center.

```
dx = mouseX - centerX
dy = mouseY - centerY
angle = atan2(dy, dx)
```

During drag:

```
deltaAngle = angle - previousAngle
```

This angular motion is translated into **timeline movement**.

---

## 6.2 Scratch Mapping

Rotation of the platter corresponds to movement through the audio timeline.

Recommended mapping:

```
180° rotation ≈ 1 second of timeline movement
```

Which means:

```
lagDelta = deltaAngle * (1 second / π)
```

---

## 6.3 Dead Zone

Near the center of the platter a dead zone prevents jitter.

```
deadRadius = 20% platter radius
```

Motion inside this zone produces no scratching.

---

## 6.4 Radial Weighting

Motion near the edge of the platter produces larger changes.

```
weight = clamp(radius / platterRadius, 0.3 → 1.0)
lagDelta *= weight
```

This makes the platter feel natural.

---

# 7. Platter Inertia

To simulate the mass of a physical record, platter movement is smoothed.

Instead of instantly applying motion:

```
platterVelocity += (gestureVelocity - platterVelocity) * inertiaFactor
```

Suggested inertia factor:

```
0.25
```

This produces a **weighty deck feel**.

---

# 8. Slip Mode

Slip mode mimics professional DJ decks.

During slip:

```
timelineHead continues advancing
```

But the user temporarily scratches the audio.

When released:

```
readHead smoothly returns to the timeline position
```

Return smoothing:

```
~120 ms
```

---

# 9. Freeze Mode

Freeze stops recording new audio.

```
writeHead does not advance
```

This allows the user to manipulate a static buffer.

During freeze:

```
feedback disabled
```

to prevent runaway loops.

---

# 10. Feedback Path

Feedback allows audio to be reinjected into the buffer.

Signal path:

```
output → feedbackGain → buffer input
```

Implementation:

```
buffer[writeHead] += output * feedback
```

---

# 11. Interpolation

Playback must interpolate between samples when speed ≠ 1.

Temporal Deck uses **cubic interpolation**.

Four samples are required:

```
y0 y1 y2 y3
```

Fractional position:

```
t = readHead - floor(readHead)
```

Cubic polynomial generates final sample.

Benefits:

* smooth pitch shifting
* minimal CPU cost
* widely used in samplers

---

# 12. Arc Visualization

Above the platter is a **yellow arc display**.

This arc represents **how far behind NOW the playback is**.

---

## 12.1 Orientation

```
RIGHT = NOW
LEFT  = older history
```

The arc only occupies the **upper half circle**.

---

## 12.2 Arc Meaning

The arc represents:

```
lag = writeHead - readHead
```

Converted to angle:

```
lagRatio = lag / maxBuffer
arcAngle = lagRatio * 180°
```

---

## 12.3 No Read Head Marker

The arc itself communicates the position.

The right side always represents NOW.

The left endpoint represents the playback position.

No additional indicator is required.

---

# 13. Buffer Limit Indicator

A small dot indicates the maximum reachable lag.

This is controlled by the BUFFER knob.

```
limitAngle = maxLag / 8s * 180°
```

This dot also indicates buffer fill during startup.

---

# 14. Startup Behavior

When the module begins:

```
buffer initially empty
```

The buffer gradually fills as audio arrives.

The limit dot moves left as the buffer becomes available.

---

# 15. Panel Color Language

Temporal Deck follows the **Integral Flux design language**.

Panel colors communicate function.

```
purple areas → modulatable controls
blue areas   → outputs
neutral area → platter instrument surface
```

This visual language helps users quickly identify patch points.

---

# 16. Inputs

```
IN_L
IN_R
RATE_CV
POSITION_CV
FREEZE_GATE
SCRATCH_GATE
```

---

# 17. Outputs

```
OUT_L
OUT_R
```

Audio outputs are located in the blue region of the panel.

---

# 18. Source File Architecture

The module is implemented in a **single source file**.

This maintains structural elegance and aligns with the user's existing module organization.

Logical classes exist within that file:

```
TemporalDeckBuffer
TemporalDeckEngine
TemporalDeckPlatterWidget
TemporalDeckDisplayWidget
TemporalDeck
TemporalDeckWidget
```

---

# 19. Implementation Order

Recommended coding sequence:

1. circular buffer
2. read/write head transport
3. playback interpolation
4. rate control
5. platter widget
6. arc display widget
7. slip + freeze behavior
8. feedback
9. polish and smoothing

---

