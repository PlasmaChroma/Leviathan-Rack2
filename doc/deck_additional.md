# Temporal Deck — Addendum Specification v1.1

```markdown
MODULE: Temporal Deck
ADDENDUM VERSION: 1.1
APPLIES TO: Design Spec v1.0 + DSP Spec v1.0
PURPOSE: Capture finalized interaction behavior, CV semantics, display logic, and hidden features.
```

---

# 1. POSITION CV — Absolute Mode (Default)

POSITION CV operates as a **normalized absolute position control** within the accessible buffer window.

Voltage mapping:

```
0V  → NOW (writeHead)
-5V → maximum accessible past (BUFFER limit)
+V  → clamped to NOW
```

Position CV always maps across the **effective buffer window**, not the full physical buffer.

Let:

```
bufferMax = 8 seconds
bufferLimit = BUFFER * bufferMax
```

Mapping:

```
lag = (-CV / 5V) * bufferLimit
readHead = writeHead - lag
```

Constraints:

```
readHead ≤ writeHead
readHead ≥ writeHead - bufferLimit
```

Positive voltages are clamped to NOW.

---

# 2. Alternate POSITION CV Mode (Context Menu)

Context menu option:

```
POSITION CV MODE
• Absolute (default)
• Offset
```

### Absolute Mode

CV determines the **absolute playback position** in the accessible buffer window.

### Offset Mode

CV is interpreted as an **offset added to the current lag**:

```
targetLag = baseLag + CV_offset
```

Offset is clamped to buffer limits.

This mode is useful for subtle modulation effects.

---

# 3. POSITION CV Smoothing

To prevent discontinuities during stepped voltage changes:

```
positionSlew = 2–10 ms
```

This smoothing applies only to POSITION CV input, not platter gestures.

---

# 4. Arc Display — Final Semantics

The arc display represents **temporal lag relative to NOW**.

```
lag = writeHead - readHead
```

Arc orientation:

```
right endpoint = NOW
left endpoint  = playback position
```

Arc span:

```
0°–180° upper hemicircle
```

Arc thickness:

```
3.5–4 mm
```

Arc color:

```
yellow fill
purple outline
```

The arc endpoint itself represents the read head.

A separate read-head marker is **not used**.

---

# 5. Buffer Limit Indicator

A small dot indicates the maximum accessible buffer depth determined by the BUFFER knob.

```
maxLag = BUFFER * bufferMax
```

Dot angular position:

```
limitAngle = (maxLag / bufferMax) * 180°
```

Behavior:

* shows accessible history limit
* visualizes buffer fill during startup

---

# 6. Rotating Platter Artwork

The platter graphic rotates visually during scratching gestures.

Rotation reflects the virtual platter displacement.

Rotation does **not represent absolute playback position**; it only reflects gesture motion.

The platter returns to neutral rotation when idle.

---

# 7. Platter Gesture Scaling

Mouse motion is interpreted relative to platter center.

Key behaviors:

```
inner dead zone near center
radial weighting toward outer edge
scaled angular motion
```

Gesture movement is smoothed using platter inertia.

```
platterState += (targetState - platterState) * inertia
```

This produces a weighty physical feel.

---

# 8. Slip Mode — Clarification

Slip Mode allows scratching while the underlying timeline continues advancing.

During slip:

```
timelineHead advances normally
readHead temporarily follows platter
```

On release:

```
readHead smoothly returns to timelineHead
```

Return smoothing duration:

```
~50–200 ms recommended
```

---

# 9. Freeze Behavior — Clarification

Freeze halts writing to the buffer:

```
writeHead remains fixed
```

Read head continues moving within the frozen buffer.

Feedback may still circulate inside the frozen buffer.

---

# 10. Hidden Feature — Zero-Cross Catch

Context menu option:

```
Zero-Cross Catch
• On (default)
• Off
```

Purpose:

Reduce audible clicks when repositioning the read head abruptly.

When enabled, Temporal Deck searches for a nearby low-amplitude sample before committing a reposition event.

Applicable events:

* platter release
* slip return
* POSITION CV jumps
* transport resets

---

# 11. Zero-Cross Catch Algorithm

Given a requested reposition index:

```
targetIndex
```

Search window:

```
[targetIndex - N , targetIndex + N]
```

Recommended window:

```
N = 24 samples
```

Selection rule:

```
bestIndex = argmin(abs(L[i]) + abs(R[i]))
```

The read head is placed at the best candidate index.

This favors low-amplitude waveform points and reduces discontinuities.

---

# 12. Events Using Zero-Cross Catch

Zero-cross repositioning is applied only to **discrete reposition events**, not continuous playback.

Applied to:

```
platter release
slip return
absolute POSITION CV jumps
hard reposition commands
```

Not applied during:

```
normal transport playback
continuous platter drag
```

---

# 13. Visual Hierarchy

Final display elements:

| Element          | Meaning                    |
| ---------------- | -------------------------- |
| Yellow arc       | playback lag               |
| Right marker     | NOW                        |
| Buffer limit dot | maximum accessible history |
| Rotating platter | user gesture feedback      |

---

# 14. Final Control Philosophy

Temporal Deck exposes two orthogonal dimensions of time manipulation:

```
RATE      → velocity through time
POSITION  → location in time
```

This separation allows rich modulation and performance control.

---

# 15. Implementation Notes

Recommended small smoothing systems:

```
positionCV slew
platter inertia smoothing
release glide smoothing
```

Interpolation method:

```
cubic interpolation
```

Buffer length:

```
8 seconds fixed
```

Stereo channels share a single timeline.

---

# Result

With this addendum, the Temporal Deck specification now includes:

* finalized POSITION CV semantics
* arc visualization model
* platter interaction behavior
* buffer limit display logic
* rotating platter UI
* zero-cross catch audio refinement
* context-menu configuration modes

This specification now forms a **complete implementation target** for the module.

```

