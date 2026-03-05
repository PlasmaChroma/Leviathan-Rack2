Dragon King Leviathan — excellent. **Temporal Deck** is a strong name and clearly communicates what the module is: a **time-manipulation playback deck** rather than a traditional delay. That clarity will matter when people browse modules in Rack.

Below is a **Codex-ready draft requirements specification**.
It focuses on:

* clear **DSP architecture**
* precise **UI widget behavior**
* **SVG layout integration**
* modular **CV mapping**
* mouse **scratching UX**

This is intentionally structured so Codex can implement it incrementally.

---

# Temporal Deck

### VCV Rack Module Specification (Draft v0.1)

Author: Leviathan / Integral Flux
Target Platform: **VCV Rack 2**
Implementation Language: **C++ (Rack SDK)**

---

# 1. Overview

**Temporal Deck** is a VCV Rack module that records incoming audio into a circular buffer and allows users to manipulate playback position and speed in real time.

Primary inspiration:

* DJ vinyl scratching
* tape transport manipulation
* delay-line time traversal

Unlike traditional delay modules, Temporal Deck exposes the **playback head position as a direct control surface**.

The module allows:

* manual **record-style scratching**
* CV controlled **time traversal**
* **reverse playback**
* **freeze / loop**
* **delay-style operation**
* optional **lookahead window** for forward scratching

---

# 2. Core Concept

Temporal Deck operates on a **circular audio buffer**.

Two logical read heads exist:

```
Write Head → always advancing (unless freeze)

Read Head A → normal playback transport
Read Head B → scratch head
```

The module output is a **crossfade between the two read heads** depending on scratch state.

```
output = mix(readA, readB, scratchMix)
```

Where:

```
scratchMix ∈ [0,1]
```

---

# 3. DSP Architecture

## 3.1 Circular Buffer

Buffer stores incoming audio samples.

Recommended default:

```
maxBufferTime = 10 seconds
```

Memory requirement:

```
sampleRate * seconds * channels
```

Example:

```
48000 * 10 * 2 ≈ 960k samples
```

Circular structure:

```
buffer[index]
writeHead = (writeHead + 1) % bufferSize
```

Stereo supported.

---

# 3.2 Read Head A (Transport Head)

Normal playback head.

Advances continuously:

```
readHeadA += playbackSpeed
```

Default:

```
playbackSpeed = 1.0
```

Used for:

* standard playback
* delay-style operation
* post-scratch recovery

---

# 3.3 Read Head B (Scratch Head)

Directly controlled by user interaction.

Possible control modes:

### Position Mode

```
readHeadB = platterPosition
```

### Velocity Mode

```
readHeadB += platterVelocity
```

Velocity mode feels more vinyl-like.

---

# 3.4 Interpolation

Reading from buffer must use interpolation.

Minimum acceptable:

```
linear interpolation
```

Preferred:

```
cubic interpolation
```

Example:

```
sample = lerp(buffer[i], buffer[i+1], frac)
```

---

# 3.5 Scratch Crossfade

To prevent clicks when switching heads:

```
output = (1 - scratchMix) * sampleA + scratchMix * sampleB
```

Where `scratchMix` transitions with smoothing.

Suggested smoothing:

```
5 ms attack
20 ms release
```

---

# 3.6 Slip Mode

While scratching, the free-run transport continues virtually.

When scratch releases:

```
readHeadA resumes as if playback continued underneath
```

This emulates DJ **slip mode**.

---

# 3.7 Freeze Mode

Freeze stops the write head:

```
writeHead paused
```

Buffer becomes static loop.

Useful for scratching loops.

---

# 3.8 Lookahead Window (Forward Scratch)

Optional latency buffer.

```
lookaheadTime = 0–500 ms
```

Normal playback is delayed by this amount, allowing:

```
forward scratching within lookahead window
```

---

# 4. User Interaction Model

## 4.1 Virtual Platter Control

Central UI element: **record platter**.

Large circular control.

Supports:

```
mouse drag
mouse rotation
scroll wheel
```

Mouse drag should emulate **record scratching**.

---

## 4.2 Platter Interaction Modes

### Touch

Mouse pressed inside platter.

Enables scratch mode.

```
scratchActive = true
```

### Drag

Mouse movement modifies platter velocity.

```
platterVelocity = deltaAngle / deltaTime
```

### Release

Scratch ends.

```
scratchActive = false
```

Crossfade returns to transport head.

---

# 5. Panel Layout

Proposed 20HP layout.

```
+-----------------------------------+
|           TEMPORAL DECK           |
|                                   |
|          ( PLATTER UI )           |
|                                   |
| Buffer  Speed  Feedback           |
|                                   |
| Freeze  Reverse  Slip             |
|                                   |
| In   Out   SpeedCV   PosCV        |
+-----------------------------------+
```

---

# 6. Parameters

## 6.1 Buffer Length

Range:

```
1 – 20 seconds
```

Defines circular buffer size.

---

## 6.2 Playback Speed

Range:

```
-4x → +4x
```

Negative speeds allow reverse playback.

---

## 6.3 Feedback

Range:

```
0–1
```

Feeds output back into buffer.

Creates delay behavior.

---

## 6.4 Wet/Dry Mix

Blend between input and processed output.

---

# 7. Inputs

## Audio

```
INPUT_L
INPUT_R
```

---

## Control

### Position CV

```
0–10V → buffer position
```

Maps to platter position.

---

### Speed CV

```
-5V → reverse
0V → stop
+5V → forward
```

Adds to playback speed.

---

### Freeze Gate

```
High → freeze write head
```

---

### Scratch Gate

Optional external scratch control.

---

# 8. Outputs

```
OUTPUT_L
OUTPUT_R
```

Optional future expansion:

```
BUFFER_POSITION_OUT
```

---

# 9. UI Rendering

The platter UI will be drawn inside the Rack widget.

Two approaches:

### Option A (Preferred)

SVG platter graphic with rotating transform.

Waveform overlay drawn via NanoVG.

### Option B

Full custom NanoVG render.

---

# 10. SVG Integration

Panel art must define placeholder regions.

Suggested element IDs:

```
PLATTER_AREA
BUFFER_KNOB
SPEED_KNOB
FEEDBACK_KNOB
FREEZE_BUTTON
REVERSE_BUTTON
```

These elements are read by helper.py for component placement.

---

# 11. Mouse Scratching Behavior

Mouse drag distance maps to platter angle.

```
angle += deltaX * sensitivity
```

Velocity estimated as:

```
velocity = (angle - prevAngle) / deltaTime
```

Velocity used to modify playback speed.

---

# 12. Performance Considerations

Important for smooth scratching.

Recommended:

* SIMD reads
* minimal allocations
* interpolation optimized

Expected CPU load: **moderate**.

---

# 13. Future Enhancements

Possible additional features:

### Quantized Buffer

Buffer synced to clock.

---

### Granular Scratch Mode

Playback head jumps across grains.

---

### Waveform Ring

Display waveform around platter.

---

### Needle Drop

Click to reposition playback.

---

# 14. Implementation Phases

Recommended development order.

### Phase 1

Core delay buffer.

### Phase 2

Manual playback head.

### Phase 3

Scratch head + crossfade.

### Phase 4

UI platter interaction.

### Phase 5

Lookahead forward scratch.

---

# 15. Target Feel

Temporal Deck should feel like:

```
"holding a vinyl record made of time itself"
```

The experience should be:

* tactile
* responsive
* musical

---

If you'd like, the next thing I would strongly recommend designing (before writing DSP code) is the **platter widget architecture**, because the UX success of this module will depend heavily on that.

There are a couple **clever Rack widget tricks** that make mouse scratching feel dramatically more natural than a normal knob, and they will affect how the SVG and widget hierarchy should be structured.
