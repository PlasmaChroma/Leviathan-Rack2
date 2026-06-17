# Bifurx Demo Notes (Behavioral Calibration Layer)

## Purpose

This document augments `Bifurx.md` and `bifurx-plan.md` with **behavioral observations derived from real-world usage (demo analysis)**.

It does NOT replace the spec.

It exists to:

* correct implicit assumptions
* clarify perceptual intent
* guide DSP tuning decisions
* prevent “technically correct but musically wrong” implementations

---

## Core Identity Correction

### Not:

* “two filters”
* “dual SVF module”

### Is:

> A **single system with two interacting resonant peaks**

This interaction is the instrument.

Implication:

* Treat the dual cores as a **coupled system**, not independent processors
* All parameter mappings should reinforce interaction, not isolation

---

## SPAN (Critical Parameter)

### Observed Behavior

* Drives the most audible transformation
* Creates:

  * dual peaks
  * spectral gaps
  * vocal/formant motion
* Frequently modulated in real usage
* Small movements near zero are subtle
* Large movements become dramatic

### Required Implementation Characteristics

* MUST be nonlinear
* MUST feel “alive” across full range

### Recommended Mapping

```cpp
// Example shaping (conceptual)
spanOct = pow(param, k) * maxSpan;
```

Where:

* `k > 1` for finer control near zero
* `maxSpan ≈ 8 octaves`

### Design Rule

> SPAN is not detune.
> SPAN is **timbre morphing**.

---

## BALANCE (Perceptual Correction)

### Observed Behavior

* Changes tone, not just level
* Interacts strongly with:

  * resonance
  * span
* Feels like shifting “center of gravity”

### Anti-Pattern (Do NOT do this)

```cpp
out = mix(coreA, coreB, balance);
```

### Required Behavior

BALANCE must influence:

* resonance distribution
* or nonlinear response weighting
* or energy emphasis between peaks

### Acceptable Approaches

* skew per-core Q
* skew output weighting nonlinearly
* combine both

### Design Rule

> BALANCE is **energy redistribution**, not mixing.

---

## LEVEL (Drive Behavior)

### Observed Behavior

* Adds:

  * “hair”
  * “growl”
* Not just amplitude

### Required Implementation

* pre-filter saturation stage
* interacts with resonance

### Suggested Model

```cpp
driven = saturate(input * gain);
```

Where:

* saturation is soft (tanh or similar)
* gain increases toward upper range

### Design Rule

> LEVEL is a **tone control**, not a volume knob.

---

## TITO (Critical Correction)

### Observed Modes

* CLEAN: neutral
* SM (Self Modulation):

  * internal feedback
  * textured / aggressive
* XM (Cross Modulation):

  * wobble / chirp
  * inter-core interaction

### Required Implementation Model

TITO must change **signal routing**, not just distortion.

#### CLEAN

```text
coreA → output
coreB → output
```

#### SM

```text
coreA → self feedback
coreB → self feedback
```

#### XM

```text
coreA → modulates coreB
coreB → modulates coreA
```

### Design Rule

> TITO is a **coupling topology switch**, not a flavor knob.

---

## RESONANCE Behavior

### Observed

* Supports self-oscillation
* Behavior varies by mode
* Interacts strongly with:

  * span
  * balance
  * drive

### Required Characteristics

* nonlinear mapping
* stable but expressive near high values
* must allow dual-peak oscillation

---

## Self-Oscillation (Important Feature)

### Observed

* produces **two tones** (one per peak)
* blendable via BALANCE
* spacing controlled by SPAN

### Implication

> This module is also a **dual sine oscillator**

### Design Requirement

* both cores must be able to oscillate independently
* interaction must remain stable under modulation

---

## Mode Interpretation (Confirmed)

From both spec and demo:

> Modes are **topological changes**, not output selections

This aligns with current plan 

### Reinforcement

Each mode must:

* feel structurally different
* not just sound like EQ variations

---

## Audio-Rate Modulation (Non-Optional)

### Observed

* FM used extensively
* modulation applied to:

  * frequency
  * span
  * balance

### Requirements

* parameters must support audio-rate modulation
* no zippering
* stable under rapid changes

### Architecture Validation

TPT SVF choice is correct 

---

## Formant Behavior

### Observed

* emerges naturally from:

  * dual peaks
  * span
  * balance
  * resonance

### Design Rule

> Do NOT explicitly model formants.

Let them emerge from correct interaction.

---

## Visualization Implications

The hardware provides:

* no visual feedback

The demo relies on:

* descriptive language (“swoosh”, “vocal”, “chirpy”)

### Bifurx Advantage

Your visualization will expose:

* peak positions
* spectral gaps
* modulation effects

### Design Guidance

* curve = structural truth (nominal)
* spectrum layer = perceptual truth (actual signal)

---

## Corrections to Existing Plan

### 1. BALANCE Needs Stronger Definition

Current plan:

> “affects per-core resonance or output weighting” 

Update:

* MUST be nonlinear
* MUST affect perceived tone, not just amplitude

---

### 2. TITO Needs Explicit Routing Model

Current plan:

> “voicing/coupling family” 

Update:

* define explicit signal routing paths (SM vs XM)

---

### 3. LEVEL Needs Saturation Guarantee

Current plan:

> “drive behavior” 

Update:

* explicitly include nonlinear stage pre-filter

---

### 4. SPAN Needs Curve Definition

Current plan:

> “nonlinear mapping” 

Update:

* must bias resolution toward low values
* must reach aggressive spread at high values

---

## Final Summary

Bifurx succeeds if:

* SPAN feels like morphing, not detune
* BALANCE shifts energy, not just mix
* LEVEL adds character, not just gain
* TITO changes topology, not just tone
* modes feel structurally distinct
* dual oscillation is stable and musical

Failure modes to avoid:

* linear parameter behavior
* static-sounding resonance
* weak interaction between cores
* treating features as independent instead of coupled

---

## Recommended Next Step

Before DSP implementation:

* lock SPAN curve function
* choose BALANCE weighting strategy
* define exact TITO routing diagram

These three define the instrument.

Everything else is implementation detail.
