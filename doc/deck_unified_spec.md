# Temporal Deck — Unified Implementation Specification (Codex Target)

```markdown
MODULE: Temporal Deck
TYPE: Stereo time-buffer / scratching instrument
TARGET: VCV Rack 2
PANEL: deck.svg
BUFFER: 8 seconds fixed
INTERPOLATION: Cubic
```

---

# 1. Module Overview

Temporal Deck is a stereo time-buffer instrument that continuously records incoming audio while allowing the user to navigate past audio using a virtual platter.

Two primary pointers define the engine:

```
writeHead → present moment (recording position)
readHead  → playback position
```

Constraint:

```
readHead ≤ writeHead
```

except during scratch manipulation.

The module behaves as:

* scratch instrument
* rhythmic delay
* temporal buffer
* time-shifting processor

The system exposes two orthogonal time axes:

```
RATE     → velocity through time
POSITION → location in time
```

---

# 2. Panel Geometry

Panel size:

```
20HP
101.6 mm × 128.5 mm
```

The SVG includes a custom anchor:

```
PLATTER_AREA
```

This anchor defines:

```
platter center
platter radius
```

It is not extracted by the helper script.

Custom widgets read this anchor directly.

---

# 3. Controls

## BUFFER

Defines maximum accessible lag.

```
maxLag = BUFFER * 8 seconds
```

Buffer memory is not resized.

---

## RATE

Controls transport speed.

Knob mapping:

```
0.0 → -2×
0.5 → 1×
1.0 → +2×
```

Formula:

```
speed = (knob - 0.5) * 4
```

---

## MIX

Dry/wet crossfade.

```
output = dry*(1-mix) + wet*mix
```

---

## FEEDBACK

Routes processed output back into the buffer.

```
feedbackSample = output * feedback
buffer[writeHead] += feedbackSample
```

---

# 4. CV Inputs

## POSITION CV

Absolute mode mapping:

```
0V → NOW
-5V → maximum lag
+V → clamped to NOW
```

Formula:

```
lag = (-CV / 5V) * bufferLimit
readHead = writeHead - lag
```

Offset mode available via context menu.

---

## RATE CV

Modifies playback speed.

```
-5V → -1× offset
0V  → no change
+5V → +1× offset
```

Final speed:

```
speed = clamp(baseSpeed + rateCVOffset, -3, +3)
```

---

# 5. Gate Inputs

## FREEZE GATE

Stops writing into the buffer.

```
writeHead frozen
```

---

## SCRATCH GATE

Allows external control of scratching via POSITION CV.

---

# 6. Transport Buttons

FREEZE
REVERSE
SLIP

Each toggles corresponding engine state.

---

# 7. Buffer System

Buffer length:

```
bufferSamples = sampleRate * 8 seconds
```

Stereo storage.

Circular indexing with safe wrapping.

---

# 8. Write Head

Each frame:

```
buffer[writeHead] = inputSample
writeHead++
```

Wrap modulo buffer size.

---

# 9. Read Head

Updated each frame:

```
readHead += speed
```

Clamped to:

```
writeHead
writeHead - maxLag
```

---

# 10. Interpolation

Playback uses cubic interpolation.

Four samples:

```
y0 y1 y2 y3
```

Fraction:

```
t = readHead - floor(readHead)
```

Cubic polynomial produces final sample.

---

# 11. Platter Interaction

Mouse motion relative to platter center.

```
dx = mouseX - centerX
dy = mouseY - centerY
angle = atan2(dy, dx)
```

Lag delta:

```
lagDelta = deltaAngle * 1 second / π
```

180° mouse motion ≈ 1 second timeline movement.

---

## Dead Zone

```
20% platter radius
```

Prevents jitter.

---

## Radial Weighting

Outer radius increases sensitivity.

```
weight = clamp(radius / platterRadius, 0.3, 1.0)
lagDelta *= weight
```

---

## Platter Inertia

```
platterVelocity += (gestureVelocity - platterVelocity) * 0.25
lag += platterVelocity * dt
```

Produces weighty feel.

---

# 12. Slip Mode

Timeline continues advancing during scratch.

```
timelineHead += dt
```

During slip:

```
readHead follows platter
```

On release:

```
readHead → timelineHead
```

Return time:

```
~120 ms
```

---

# 13. Freeze Mode

```
writeHead stops advancing
```

Feedback disabled while frozen.

---

# 14. Arc Visualization

Arc represents:

```
lag = writeHead - readHead
```

Angle mapping:

```
lagRatio = lag / bufferMax
arcAngle = lagRatio * 180°
```

Arc grows leftward from NOW.

---

# 15. Buffer Limit Dot

Dot indicates maximum accessible lag.

```
dotAngle = bufferLimit / bufferMax * 180°
```

Moves during startup until buffer fills.

---

# 16. Zero-Cross Catch

Optional click reduction.

Search window:

```
±24 samples
```

Choose sample minimizing:

```
abs(L) + abs(R)
```

Used for:

* platter release
* slip return
* POSITION CV jumps

---

# 17. Internal Class Architecture

Implementation occurs in **one module source file**.

Logical classes inside the file:

```
TemporalDeckBuffer
TemporalDeckEngine
TemporalDeckPlatterWidget
TemporalDeckDisplayWidget
TemporalDeck
TemporalDeckWidget
```

Responsibilities and boundaries are defined in the architecture document.

---

# 18. Implementation Order

1. buffer engine
2. transport logic
3. rack module integration
4. platter widget
5. arc display widget
6. polish / smoothing / zero-cross catch

---

