# Cartridge Target Template

Use this document to define cartridge modes in `TemporalDeck` with enough specificity to implement them consistently.

The goal is to capture:
- steady playback character
- active scratch/motion character
- gain and distortion intent
- constraints on what the cart should not become

This is intentionally compact. Add only details that affect DSP, feel, or UI.

## How To Use This

For each cartridge mode, fill out:
- identity: what real or fictional cart this mode represents
- playback target: how it should sound at steady transport
- scratch target: how it should behave under platter motion
- implementation hints: what likely needs to change in code
- guardrails: what we should avoid during tuning

Recommended workflow:
1. define the target in this document
2. tune the cart in `TemporalDeck.cpp`
3. compare playback and scratch behavior separately
4. update this doc if the implementation goal changes

---

## Cartridge Entry Template

### Name
- Short UI label:
- Full name:
- Real reference or inspiration:

### Role
- Intended use:
- Why this cart exists in the catalog:
- Closest neighboring modes it must still differ from:

### Playback Target
- Overall tonal balance:
- Low end:
- Midrange:
- High end:
- Stereo image:
- Output / loudness character:
- Distortion / saturation character:
- Noise / imperfections:

### Scratch / Motion Target
- Behavior during slow drag:
- Behavior during fast drag:
- Direction reversals:
- Top-end change under motion:
- Low-end change under motion:
- Transient bite:
- Tracking / stability feel:
- Any motion-dependent artifact that is desirable:

### Priority Order
- Trait 1:
- Trait 2:
- Trait 3:

### Do Not Do
- Avoid:
- Avoid:
- Avoid:

### DSP Notes
- Base EQ intent:
- Motion EQ intent:
- Saturation / limiting intent:
- Stereo / crossfeed intent:
- Additional stages needed:
- Gain normalization target:

### UI / Visual Notes
- Label to show on module:
- Optional color/material inspiration:
- Anything that affects tonearm/cart visual design:
- Should the on-module cartridge graphic change color/material with this mode:

### Validation Notes
- Best audio material for testing:
- What should be obvious on first listen:
- What would indicate the model is wrong:

---

## Current Candidate Set

### Clean
- Status: keep as neutral reference
- Notes:

### Lo-Fi
- Status: likely keep as degraded creative mode
- Notes:

### M44-7
- Status: candidate replacement
- Notes:

### Concorde Scratch
- Status: candidate replacement
- Notes:

### Stanton 680 HP
- Status: candidate addition
- Notes:

### Q.Bert
- Status: optional later candidate
- Notes:

---

## Global Questions

- Should named carts prioritize realism or musically useful approximation when the two conflict?
- Should gain normalization aim for matched perceived loudness, matched peak level, or preserve each cart's "hotness"?
- How much cart behavior should come from the cart stage itself versus the scratch engine?
- Do we want cartridge-specific scratch transient behavior, or keep transient behavior global?
- Do we want full real product names in the UI, or shortened labels?
- Should cartridge mode also drive the tonearm/cart graphic colors on the module UI?
