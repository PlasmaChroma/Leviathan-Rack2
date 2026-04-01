# TemporalDeck — Functional Design Spec

## Overview

**TemporalDeck** is a stereo buffer-based performance deck for VCV Rack that behaves like a hybrid of:

* a live audio delay line
* a DJ turntable / platter interface
* a scratch instrument
* a transport manipulator
* a cartridge coloration simulator

At its core, the module continuously records incoming stereo audio into a circular buffer and allows the read head to move relative to the live write head. This creates a controllable temporal offset (“lag”) that can be manipulated by knob, CV, transport controls, or direct platter gestures.

The module is designed to support both **musical buffer playback** and **playable scratch interaction**.

---

## High-Level Behavior

### Core signal concept

The module maintains:

* a **stereo circular buffer**
* a **write head** that records incoming audio in real time
* a **read head** that reads from some point behind the write head

The distance between write head and read head is the module’s **lag**.

This lag can be controlled by:

* buffer amount
* playback rate
* reverse
* freeze
* slip return
* position CV
* scratch gate + position CV
* mouse platter dragging
* mouse wheel scratching

The read signal is then optionally colored by a cartridge model, mixed with the dry signal, and optionally fed back into the recording buffer.

---

# 1. Audio Buffer System

## 1.1 Buffer structure

The module records **stereo audio** into an internal circular buffer with separate left/right arrays.

### Supported buffer duration modes

Context menu selectable:

* **8 s**
* **16 s**
* **8 min**

Internal real durations are slightly larger than the user-facing values:

* 8 s mode → internally ~9 s
* 16 s mode → internally ~17 s
* 8 min mode → internally ~481 s

The implementation reserves roughly 1 second extra beyond the “usable” span.

## 1.2 Buffer write behavior

On every sample, unless prevented by state logic:

* input audio is written into the buffer
* feedback can be added into the write path
* write head advances by one sample
* filled length increases until the buffer is full

Write source:

```text
writeL = inputL + outputL * feedback
writeR = inputR + outputR * feedback
```

### Write suppression cases

Buffer writing is blocked when:

* **Freeze** is active
* the platter is being held at a constrained scratch edge
* reverse transport has hit the oldest readable edge

---

# 2. Playback / Transport Model

## 2.1 Read head model

The read head is normally constrained between:

* **newest readable position** (live edge / now)
* **oldest allowed lag** as set by the buffer control

So the module does not behave like an unconstrained looper; it behaves like a deck reading within a bounded history window.

## 2.2 Buffer control

### BUFFER knob

Controls the maximum accessible lag behind live input.

* 0.0 → effectively near live / minimal lag window
* 1.0 → full available lag for current buffer mode

Actual accessible lag is limited by:

* knob setting
* how much of the buffer has actually been filled so far

So immediately after loading/resetting, the full lag range is not yet available.

---

# 3. Rate / Playback Speed

## 3.1 RATE knob

Knob-only mapping:

* minimum: **0.5x**
* center: **1.0x**
* maximum: **2.0x**

This is a nonlinear piecewise mapping centered at 1x.

## 3.2 RATE CV input

RATE CV is added to the base speed as:

* `rateCv / 5`
* clamped to `[-1, +1]`

Then total speed is clamped to:

* **-3x to +3x**

## 3.3 Reverse transport

When **Reverse** is active:

* transport speed sign is inverted

In normal transport mode this causes the read head to move backward through the available lag window.

A safety clamp prevents transport from continuing past the oldest allowed edge.

---

# 4. Wet/Dry and Feedback

## 4.1 MIX knob

Controls dry/wet blend:

* 0 = dry input only
* 1 = fully buffered / processed playback

Output formula:

```text
out = dry * (1 - mix) + wet * mix
```

## 4.2 FEEDBACK knob

Feeds a portion of the module’s output back into the recording path.

This allows:

* regenerative delay-like behavior
* repeated scratching of prior output
* smeared / self-layering temporal textures
* more unstable behavior when combined with cartridge coloration and transport changes

---

# 5. Freeze / Reverse / Slip Modes

These are implemented as **latched transport states** with illuminated buttons.

## 5.1 Freeze

When active:

* playback speed becomes 0
* write head stops recording
* current buffer content is held
* read head remains stationary unless scratch logic overrides it

Freeze can also be activated by gate input.

## 5.2 Reverse

When active:

* normal transport runs backward through the accessible lag range
* mutually exclusive with Freeze and Slip when toggled from the panel

## 5.3 Slip

Slip acts as a **return-to-now performance mode**.

Behavior:

* while scratching or otherwise manipulating lag, audio can move away from live
* when scratch is released, the read head smoothly returns toward the live write point
* this includes:

  * an initial easing phase
  * a final catch-up phase
  * a near-zero “now catch” snap when close enough

Slip is also mutually exclusive with Freeze and Reverse at the button latch level.

### Slip intent

This is essentially a DJ-style “momentary deviation from the timeline, then return to live continuity” mode.

---

# 6. Position Addressing

## 6.1 POSITION CV input

When connected without scratch gate:

* acts as **absolute lag addressing**
* magnitude of CV maps to position within the accessible lag window

Mapping:

* `abs(cv) / 10`
* clamped to `[0, 1]`
* then multiplied by accessible lag

Notably, the sign of POSITION CV is ignored in this mode.

## 6.2 Scratch gate + Position CV

When both are present and scratch gate is high:

* the module enters **external scratch mode**
* POSITION CV sets scratch lag directly rather than ordinary position-follow mode

This creates a CV-driven scratch/read-head positioning behavior.

---

# 7. Scratch System

The scratch implementation is one of the module’s central features.

There are three scratch interaction families:

* **external CV scratch**
* **manual platter drag scratch**
* **mouse wheel scratch**

The module also supports two scratch models:

* **Legacy**
* **Hybrid**

Selectable from the context menu.

---

## 7.1 Scratch sensitivity

### SCRATCH SENSITIVITY knob

Mapped to a multiplier:

* lower half: **0.5x → 1.0x**
* upper half: **1.0x → 2.0x**

This affects how strongly platter gestures translate into lag movement.

---

## 7.2 Manual platter drag scratch

The platter UI supports direct left-mouse drag.

Behavioral characteristics:

* drag contact radius is captured on drag start
* only **tangential motion** is used for platter movement
* radial mouse drift is intentionally ignored
* live lag is used as the base so reverse drags don’t fight the advancing write head
* drag motion is translated into lag change using platter geometry and scratch sensitivity

This is meant to feel more like touching and rotating a physical platter, rather than simply scrubbing a position wheel.

---

## 7.3 Mouse wheel scratch

Hovering over the platter and scrolling triggers wheel scratch.

Behavior:

* wheel delta is converted into lag delta
* a short “scratch hold” duration keeps the wheel interaction active briefly after each scroll
* if Slip is active, this hold lasts longer
* wheel deltas drive either:

  * target lag gliding in Legacy mode
  * velocity bursts in Hybrid mode

This provides a flick / nudge style scratch interaction.

---

## 7.4 Legacy scratch model

Legacy mode is more direct and target-chasing.

Characteristics include:

* lag-target chasing with bounded step sizes
* smoothing for slow movement
* event-driven gesture following
* predictive lag drift between sparse drag events
* special handling for:

  * stationary hold
  * slow reverse movement
  * reverse bite / more aggressive backward feel
  * de-clicking and micro smoothing
  * transient emphasis on direction reversals

This model appears optimized for detailed manual control and explicit gesture interpretation.

---

## 7.5 Hybrid scratch model

Hybrid mode is more physically modeled.

Characteristics:

* velocity-first internal motion model
* hand-follow velocity
* correction velocity toward lag target
* acceleration limits
* damping / coast behavior
* wheel burst impulses
* deadband and snap-to-now logic near live edge

In short, Hybrid scratch tries to feel more inertial and platter-like, rather than directly position-chasing.

---

# 8. Interpolation / Read Quality

The module supports multiple read interpolation modes internally.

Available methods:

* **Linear**
* **Cubic**
* **High-quality 6-point Lagrange**

### Default behavior

* normal playback typically uses cubic
* scratch path can use high-quality interpolation if enabled
* a context menu toggle controls **High-quality scratch interpolation**

Purpose:

* reduce artifacts during non-integer read-head movement
* improve scratch sound fidelity
* preserve detail during motion-heavy buffer reading

---

# 9. Scratch Audio Cleanup / Tone Shaping During Motion

The module includes substantial special-case DSP for scratch playback.

## 9.1 Motion-aware treatment

Scratch playback is not just raw read-head motion. It includes:

* continuity blending
* micro-step de-clicking
* slow-motion smoothing
* transient enhancement on direction flips
* slow reverse rumble trimming
* DC/high-pass style suppression in specific cases
* motion-dependent cartridge dulling reduction during scratch paths

This is explicitly tuned so scratching sounds more intentional and less zippery, clicky, or rumble-damaged.

## 9.2 Different treatment for Legacy vs Hybrid

* **Hybrid** gets lighter cleanup so its motion model remains primary
* **Legacy** gets more pronounced de-click/transient shaping and slow-reverse cleanup

---

# 10. Cartridge Character System

TemporalDeck applies a stylized “cartridge / stylus” voicing stage to the wet signal.

## 10.1 Cartridge modes

Selectable by cycling the cartridge button on the tonearm pivot:

* **Clean**
* **M44-7**
* **C.MKII S**
* **680 HP**
* **Lo-Fi**

## 10.2 Cartridge processing features

Depending on mode, processing may include:

* high-pass / rumble removal behavior
* body resonance emphasis
* presence shaping
* low-pass motion dulling
* saturation / drive
* stereo tilt mismatch
* crossfeed
* makeup gain

## 10.3 Motion-aware dulling

Cartridge LP corner can shift with motion amount, simulating stylus behavior under movement.

## 10.4 Lo-Fi mode extras

Lo-Fi adds explicit degradation features:

* wow
* flutter
* mono blend / wear smear
* light hiss bed
* occasional crackle pops
* slight channel mismatch
* worn tonal response

This is the most “vinyl fiction” mode in the implementation.

---

# 11. UI / Visual Feedback

## 11.1 Main controls

### Parameters

* **BUFFER**
* **RATE**
* **SCRATCH SENSITIVITY**
* **MIX**
* **FEEDBACK**
* **FREEZE** button
* **REVERSE** button
* **SLIP** button
* **CARTRIDGE CYCLE** button

### Inputs

* **POSITION CV**
* **RATE CV**
* **INPUT L**
* **INPUT R**
* **SCRATCH GATE**
* **FREEZE GATE**

### Outputs

* **OUTPUT L**
* **OUTPUT R**

If the right input is unpatched, left input is normalled to right internally.

---

## 11.2 Platter visualization

The module includes a custom platter drawing with:

* rotating platter graphic
* center label
* simulated grooves
* platter rotation tied to transport / scratch motion
* gesture-biased visual response during scratching so direction changes remain readable

## 11.3 Tonearm / headshell visualization

A decorative tonearm widget is drawn with:

* pivot
* arm
* cartridge / headshell styling
* cartridge-specific visual appearance
* current cartridge label text

## 11.4 Lag display

The module displays current lag in **milliseconds**.

## 11.5 Arc lights

A semicircular arc of lights shows:

* current lag amount
* maximum accessible lag limit

Yellow lights show current position progression.
Red lights indicate the current maximum boundary marker.

## 11.6 State lights

Dedicated lights show latch state for:

* Freeze
* Reverse
* Slip

---

# 12. State Persistence / Serialization

The module saves and restores:

* Freeze latch state
* Reverse latch state
* Slip latch state
* High-quality scratch interpolation setting
* Cartridge character
* Scratch model
* Buffer duration mode

So its operational mode persists with the patch.

---

# 13. Context Menu Features

The context menu exposes:

## Scratch submenu

* **Scratch model**

  * Legacy
  * Hybrid

## Advanced submenu

* **Buffer range**

  * 8 s
  * 16 s
  * 8 min

## Direct toggle

* **High-quality scratch interpolation**

---

# 14. Operational Summary

## Conceptually, the module can function as:

### A live delay deck

Use BUFFER + RATE + MIX + FEEDBACK to create time-offset playback and regenerative echoes.

### A reverse deck

Engage Reverse and scan backward through recent audio.

### A frozen capture deck

Engage Freeze to hold the material and scratch or replay it.

### A slip performance deck

Manipulate the timeline, then smoothly recover to live audio.

### A CV-addressable temporal reader

Use POSITION CV and RATE CV for modulation-driven time scanning.

### An interactive scratch instrument

Use mouse drag or wheel gestures on the platter for tactile scratch performance.

### A vinyl coloration processor

Use cartridge modes to impose stylized playback coloration, from clean to degraded lo-fi.

---

# 15. Design Intent Summary

TemporalDeck is best understood not as a conventional delay, but as a **playable time-displacement instrument**.

Its major design pillars are:

1. **Live temporal capture**
2. **Physically suggestive platter interaction**
3. **Multiple transport paradigms**
4. **Scratch-specific DSP cleanup**
5. **Vinyl/cartridge character simulation**
6. **Strong visual feedback of lag and state**

---

# 16. Suggested One-Line Product Description

**TemporalDeck is a stereo live-buffer turntable that lets you freeze, reverse, slip, scratch, CV-address, and color recent audio like a playable digital platter.**

