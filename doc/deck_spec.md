# Temporal Deck — Design Specification v1.0

```markdown
MODULE: Temporal Deck
TYPE: Stereo time-buffer / scratching instrument
PLATFORM: VCV Rack 2
WIDTH: 20HP (101.6 mm)
HEIGHT: 128.5 mm
```

---

# 1. Core Concept

Temporal Deck continuously records incoming audio into a circular buffer and allows the user to navigate recorded time using a virtual platter.

The module exposes the past as a playable surface where the user can:

* scratch audio
* warp playback speed
* freeze time
* slip-scratch like a DJ deck

The system maintains two main pointers:

```
writeHead = present moment (incoming audio)
readHead  = playback position within the buffer
```

Constraint:

```
readHead ≤ writeHead
```

except during temporary manipulation states.

---

# 2. Buffer Architecture

Maximum memory:

```
8 seconds circular buffer
```

This buffer is allocated once at module initialization.

The **BUFFER knob does not resize the buffer**.

Instead it defines the **maximum reachable lag**.

```
maxLag = BUFFER * 8 seconds
```

Older samples beyond this limit remain in memory but cannot be accessed by the read head.

Advantages:

* stable DSP
* no reallocation
* predictable arc behavior

---

# 3. Stereo Architecture

Left and Right channels share the same timeline.

```
single writeHead
single readHead
stereo audio stored per sample
```

This preserves stereo coherence during scratching.

---

# 4. Playback Transport

Playback rate is determined by:

```
RATE knob
RATE CV
```

Transport model:

```
readHead += speed
```

Where:

```
speed = 1.0 → normal playback
speed > 1.0 → faster playback
speed < 1.0 → slower playback
```

Suggested range:

```
0.25× → 3×
```

Center detent recommended.

---

# 5. RATE CV

CV is centered at 0V.

```
0V = normal playback speed
negative CV = slower playback
positive CV = faster playback
```

Suggested mapping:

```
±5V → 0.5× – 2× speed
```

CV offsets the knob value.

---

# 6. Platter Interaction

The platter is a custom widget defined by `PLATTER_AREA` in the SVG.

Interaction model:

* mouse position interpreted relative to platter center
* motion treated as finger dragging a record

At mouse-down:

```
store initial angle
```

During drag:

```
angleDelta = currentAngle − previousAngle
```

Motion is scaled and smoothed before affecting the read head.

Features:

* inner dead zone near platter center
* radial weighting (outer radius more sensitive)
* nonlinear scaling to prevent excessive jumps

---

# 7. Platter Feel

The platter has inertia to simulate mass.

Virtual platter position follows target gesture using smoothing.

Conceptually:

```
platterPos += (targetPos − platterPos) * inertia
```

This gives the control a “weighty deck” feel.

---

# 8. Release Behavior

When the platter is released:

```
readHead smoothly transitions back to RATE-driven motion
```

Context menu option:

```
Smooth Release (default)
Hard Release
```

Hard release resumes transport immediately.

---

# 9. Slip Mode

Slip allows temporary scratching while the timeline continues advancing.

During slip:

```
writeHead continues moving
readHead temporarily decoupled
```

On release:

```
readHead returns smoothly to NOW
```

---

# 10. Interpolation

Audio playback uses:

```
cubic interpolation
```

Benefits:

* smooth pitch shifting
* low CPU cost
* widely used in samplers

---

# 11. Feedback

Feedback routes processed audio back into the buffer.

Recommended path:

```
readHead output → buffer input
```

This creates repeating textures and time feedback.

---

# 12. Visualization System

The display uses an **upper hemicircle arc** above the platter.

Orientation:

```
right side = NOW
left side  = older past
```

Arc color:

```
yellow fill
purple outline
```

Arc thickness:

```
~3.5–4 mm
```

---

# 13. Arc Semantics

The arc represents:

```
lag = writeHead − readHead
```

Meaning the current playback distance from NOW.

Arc spans:

```
readHead → writeHead
```

Left endpoint = readHead
Right endpoint = NOW

No separate read-head marker is used.

The arc endpoint itself represents the read head.

---

# 14. Buffer Limit Indicator

A small dot indicates the maximum reachable lag.

This dot moves according to the BUFFER knob:

```
bufferAngle = maxLag / 8s * arcSpan
```

The dot also visualizes buffer priming at startup.

---

# 15. Startup Behavior

At module initialization:

* buffer begins empty
* limit dot moves left as history accumulates
* arc grows naturally as lag increases

---

# 16. Visual Elements

The arc system communicates three states:

| Element      | Meaning                   |
| ------------ | ------------------------- |
| yellow arc   | playback lag              |
| right marker | NOW                       |
| small dot    | maximum accessible buffer |

---

# 17. Platter Artwork

The platter artwork rotates visually according to platter motion.

Rotation reflects the virtual platter displacement during scratching.

This reinforces the deck metaphor.

---

# 18. Inputs / Outputs

### Inputs

```
IN_L
IN_R
RATE_CV
POSITION_CV
FREEZE_GATE
SCRATCH_GATE
```

### Outputs

```
OUT_L
OUT_R
```

---

# 19. Panel Design Language

Panel color semantics follow Integral Flux conventions.

```
purple regions = modulatable controls
blue regions   = outputs
neutral center = instrument surface
```

---

# 20. SVG Integration

SVG includes:

```
PLATTER_AREA
```

This anchor is read directly by code.

It is **not extracted by the helper script**.

Arc rendering and platter interaction are implemented in a custom widget.

---

# 21. Future Extensions

Possible later features:

* granular freeze textures
* alternate interpolation modes
* binaural buffer offsets
* segmented arc visualization mode

```

---

# What we achieved

This spec now fully defines:

- DSP architecture
- interaction model
- UI semantics
- buffer behavior
- visualization rules
- panel integration

It is **very close to Codex-ready**.

---

# Next logical step

The next thing that would help implementation is creating a **companion DSP architecture section** describing:

- exact buffer data structures
- pointer update logic
- interpolation implementation
- scratch smoothing math

That would make the spec essentially **drop-in ready for code generation**.
```
