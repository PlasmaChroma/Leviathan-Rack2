# Temporal Deck
## UI / Panel / Control Specification Draft v0.2

Author: Leviathan / Integral Flux  
Target: VCV Rack 2  
Status: Draft for Codex implementation planning

---

## 1. Design Intent

Temporal Deck should present itself as a **playable stereo time instrument**, not merely a utility delay.

The module should immediately communicate three things:

1. It records and replays audio from a time buffer.
2. The large center control is a **virtual record / platter** that can be manipulated directly by hand with the mouse.
3. Secondary controls shape how the deck behaves as an effect: buffer, speed, blend, feedback, freeze, and mode options.

The panel should balance:

- **performability** (easy live mouse interaction)
- **clarity** (readable signal path and control groupings)
- **modularity** (clear CV/audio I/O)
- **visual identity** (stylized deck / temporal vinyl motif)

---

## 2. High-Level Functional Requirements

Temporal Deck is a **stereo effect module**.

Minimum audio I/O:

- `INPUT_L`
- `INPUT_R`
- `OUTPUT_L`
- `OUTPUT_R`

At minimum, the first UI draft should support these core controls:

- Large central **platter widget** for mouse scratching
- `BUFFER` control
- `SPEED` control
- `MIX` control
- `FEEDBACK` control
- `FREEZE` button
- `REVERSE` button
- `SLIP` toggle/button

Recommended first-pass CV/gate I/O:

- `SPEED_CV`
- `POSITION_CV`
- `FREEZE_GATE`
- `SCRATCH_GATE` or `HOLD_GATE`

Optional but desirable additions:

- `LOOKAHEAD` control and/or CV
- `RESET / RETURN` trigger input
- `CLOCK` input for future buffer sync mode
- `POSITION_OUT` or `PHASE_OUT` output for future expansion

---

## 3. Panel Layout Philosophy

The module should be designed around a **dominant circular platter region** occupying the visual and interactive center of the panel.

The platter should be the largest single interactive element on the module.

All other controls should support the platter rather than compete with it.

### Layout principles

- Put **performance controls** closest to the platter.
- Put **audio I/O** at the bottom in a clear stereo grouping.
- Put **setup / shaping knobs** above or around the platter.
- Keep enough empty space around the platter so mouse interaction does not feel cramped.
- Avoid placing ports so close to the platter that patch cables visually block the main interaction zone too much.

---

## 4. Proposed First-Pass Control Set

### 4.1 Audio I/O

Bottom row, left-to-right:

- `IN L`
- `IN R`
- `OUT L`
- `OUT R`

This should read naturally as stereo in → stereo out.

### 4.2 Main Knobs

Recommended first-pass primary knobs:

- `BUFFER` — circular buffer length / history window
- `SPEED` — transport speed / base playback rate
- `MIX` — dry/wet blend
- `FEEDBACK` — recirculation amount

Recommended optional fifth knob if room allows:

- `LOOKAHEAD` — forward scratch window by intentional latency

### 4.3 Buttons / Toggles

Recommended buttons:

- `FREEZE`
- `REVERSE`
- `SLIP`

Optional future button:

- `MODE` (scratch mode / tape mode / position-vs-velocity behavior)

### 4.4 CV / Gate Inputs

Recommended first-pass CV inputs:

- `SPEED CV`
- `POSITION CV`
- `FREEZE`
- `SCRATCH`

Optional future CV inputs:

- `MIX CV`
- `FEEDBACK CV`
- `LOOKAHEAD CV`

---

## 5. Recommended Module Width

Because the platter must be large enough for **meaningful mouse interaction**, this should **not** be a narrow module.

Recommended working widths to explore:

- **16 HP** minimum if the platter is modest and controls are dense
- **18 HP** comfortable compromise
- **20 HP** preferred if the goal is strong visual identity and easy mouse use

Recommendation for current draft: **start at 18 HP or 20 HP**.

This is an effect/performance module. Giving the platter room is worth the panel space.

---

## 6. Platter Widget Requirements

The platter is not a normal knob.

It should be implemented as a **custom Rack widget** with bespoke mouse interaction behavior.

### 6.1 Functional behavior

The platter should support:

- click / hold to enter scratch interaction
- horizontal and/or angular drag to manipulate the virtual record
- release to hand control back to the transport system
- optional scroll-wheel fine adjustment
- optional modifier key for finer control

### 6.2 UX behavior

The platter should feel closer to a **touch surface** than a standard rotary knob.

Recommended interaction model for first implementation:

- Clicking inside platter sets `scratchActive = true`
- While held, mouse movement updates a scratch angle / scratch velocity accumulator
- Releasing exits scratch mode and triggers smooth return to transport playback

### 6.3 Visual layers

The platter region should eventually support these render layers:

1. Static platter base graphic
2. Rotating record texture / ring indicators
3. Read head / write head / position indicators
4. Optional waveform or fill-state visualization
5. Optional glow / active state while scratching

### 6.4 First-pass simplification

For the first implementation, the platter only needs:

- static circular record graphic
- basic rotational marker line or label
- visual indication when user is actively scratching

Waveform overlays and advanced indicators can come later.

---

## 7. SVG / Widget Division of Labor

The panel SVG should define the static panel art and the placement anchors.

The custom platter widget should be rendered in code on top of, or within, a clearly reserved platter area.

### 7.1 SVG should provide

- panel background art
- title / branding
- labels for controls and ports
- placeholder circle or ring for platter area
- alignment positions for knobs, buttons, and ports

### 7.2 Code-rendered widget should provide

- interactive platter behavior
- animated platter state
- active scratch visual feedback
- optional dynamic read/write indicators
- optional waveform / progress display

### 7.3 Recommended SVG placeholder IDs

Reserve explicit IDs for panel layout extraction:

- `PLATTER_AREA`
- `BUFFER_PARAM`
- `SPEED_PARAM`
- `MIX_PARAM`
- `FEEDBACK_PARAM`
- `LOOKAHEAD_PARAM` (optional)
- `FREEZE_PARAM`
- `REVERSE_PARAM`
- `SLIP_PARAM`
- `INPUT_L`
- `INPUT_R`
- `OUTPUT_L`
- `OUTPUT_R`
- `SPEED_CV`
- `POSITION_CV`
- `FREEZE_GATE`
- `SCRATCH_GATE`

If a helper script is used, these IDs should correspond directly to component placements.

---

## 8. Recommended Panel Organization Options

### Option A: Symmetric top-controls + center platter + bottom I/O

Top row:

- `BUFFER`
- `SPEED`
- `MIX`
- `FEEDBACK`

Center:

- large platter

Below / around platter:

- `FREEZE`
- `REVERSE`
- `SLIP`

Bottom row:

- stereo I/O and CV ports

**Pros:** very readable, traditional, stable  
**Cons:** may compress ports if too many CV jacks are added

### Option B: Side-column controls + center platter + bottom audio

Left side column:

- `BUFFER`
- `MIX`
- `FREEZE`
- `IN L`
- `OUT L`

Right side column:

- `SPEED`
- `FEEDBACK`
- `REVERSE`
- `IN R`
- `OUT R`

Bottom center / lower center:

- CV and gate inputs

Center:

- large platter

**Pros:** leaves platter visually dominant, good stereo symmetry  
**Cons:** can become visually busy if labels are not excellent

### Option C: Arc controls around platter

Knobs placed in a partial arc around the top half of the platter, buttons below, ports at bottom.

**Pros:** elegant and instrument-like  
**Cons:** harder to lay out cleanly in SVG, may be harder for helper scripts and alignment

### Recommendation

Start with **Option A** for implementation simplicity and clarity, unless visual mockups show that Option B gives much better platter space.

---

## 9. Suggested First-Pass Physical Component Styling

This section is intentionally about **visual category**, not exact C++ class names yet.

### 9.1 Knobs

Use **medium-to-large knobs** for the four main continuous parameters:

- `BUFFER`
- `SPEED`
- `MIX`
- `FEEDBACK`

Recommended visual style:

- clean, dark, performance-oriented
- readable indicator line
- enough diameter to distinguish from small trim controls

Do **not** make these trimpot-sized. This module should invite play.

If `LOOKAHEAD` is added in the first pass, it can be either:

- same size as main knobs, or
- slightly smaller secondary knob

### 9.2 Buttons

Use distinctly readable illuminated buttons or toggles for:

- `FREEZE`
- `REVERSE`
- `SLIP`

Buttons should have strong state visibility.

At least `FREEZE` should clearly indicate active/inactive state with an LED or internal light.

### 9.3 Ports

Standard 3.5mm jack style for all audio/CV/gate ports.

Stereo audio ports should be visually grouped as pairs.

### 9.4 LEDs / status lights

Recommended status lights:

- `FREEZE active`
- `REVERSE active`
- `SLIP active`
- optional `REC / buffer writing`
- optional `SCRATCH active`

---

## 10. Proposed First UI Draft Layout (Textual)

### 20 HP working concept

Top row:

- `BUFFER`
- `SPEED`
- `MIX`
- `FEEDBACK`

Center:

- large `PLATTER_AREA`

Lower center, just under platter:

- `FREEZE`
- `REVERSE`
- `SLIP`

Bottom two rows:

Audio row:

- `IN L`
- `IN R`
- `OUT L`
- `OUT R`

CV/gate row:

- `SPEED CV`
- `POSITION CV`
- `FREEZE`
- `SCRATCH`

This gives a balanced and understandable first-pass front panel.

---

## 11. Mouse Interaction Requirements for Codex

The platter widget must not inherit normal knob semantics without modification.

### Required behavior

- Capture mouse-down inside platter bounds.
- While held, compute deltas from mouse movement.
- Convert movement into a scratch transport signal.
- Allow fast reversals without losing state.
- On mouse-up, release gracefully back to transport playback.

### Strong recommendation

Support **horizontal drag emphasis** as an alternative to pure angular rotation.

Reason:

Many users will intuitively try to scratch left/right rather than spin in a perfect circle with the mouse.

Possible mapping:

- horizontal drag drives platter displacement / velocity most strongly
- vertical drag contributes less or is ignored in first version

This is likely more accessible than true circular mousing.

---

## 12. Accessibility / Playability Concerns

The module must remain usable even when patch cables overlap the lower section.

Therefore:

- do not place critical buttons too low
- keep platter high enough that cables do not hide the main interaction area
- use legible labels with sufficient contrast
- keep the platter large enough that accidental misses are rare

This is especially important because the platter is a mouse-performed surface, not just a set-and-forget control.

---

## 13. Phased UI Implementation Plan

### Phase UI-1

Establish panel width and component count.

Decide between:

- 18 HP
- 20 HP

### Phase UI-2

Create first SVG panel draft with:

- title
- platter placeholder circle
- all control labels
- all component anchors

### Phase UI-3

Implement custom platter widget in C++ with minimal visuals:

- circular hit area
- active state
- scratch drag behavior

### Phase UI-4

Wire platter widget into DSP engine.

### Phase UI-5

Add dynamic platter visuals:

- rotation marker
- read/write indicator
- optional fill ring

### Phase UI-6

Refine aesthetic details and component spacing after real in-Rack testing.

---

## 14. Decisions Needed Next

Before coding begins, confirm the following:

1. Final working width: **18 HP or 20 HP**
2. First-pass control count: whether `LOOKAHEAD` is included now or deferred
3. Preferred layout family: **Option A** or **Option B**
4. Main mouse gesture model:
   - mostly horizontal drag
   - angular drag
   - hybrid
5. Whether buttons should be:
   - latching buttons with LEDs
   - toggle switches
   - momentary where appropriate

---

## 15. Current Recommendation Summary

For the first real build of Temporal Deck:

- make it **stereo**
- use **20 HP** unless physical mockup strongly argues for 18 HP
- use a **large custom platter widget** as the center of the module
- use **4 main knobs**: `BUFFER`, `SPEED`, `MIX`, `FEEDBACK`
- use **3 state buttons**: `FREEZE`, `REVERSE`, `SLIP`
- provide **4 control inputs**: `SPEED CV`, `POSITION CV`, `FREEZE`, `SCRATCH`
- place audio I/O on the bottom in a clear stereo grouping
- keep the platter interaction model closer to **horizontal scratch dragging** than a standard rotary knob
- define all panel anchors in SVG early so helper-based code generation remains straightforward

---

## 16. Immediate Next Deliverables

The next concrete outputs should be:

1. A **component inventory decision** (exact count of knobs, buttons, ports, LEDs)
2. A **rough panel layout map with mm coordinates**
3. A **first SVG panel draft** with placeholder IDs
4. A **custom platter widget spec** describing mouse event handling in Rack

