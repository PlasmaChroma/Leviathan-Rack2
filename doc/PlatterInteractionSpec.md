# TemporalDeck Platter Interaction Spec (Draft v1)

## 1) Purpose

Define a single, testable contract for platter interaction so scratch feel is stable across refactors.

This spec prioritizes:

- Physical consistency (`33.333 RPM` baseline).
- Predictable forward/backward behavior in both `freeze` and live-buffer modes.
- One authoritative control mapping from user gesture to lag target.

## 2) Core Physical Baseline

Use the platter speed baseline:

- `kNominalPlatterRpm = 33.333333`
- `secondsPerRevolution = 60 / kNominalPlatterRpm = 1.8 s`
- `samplesPerRevolution = sampleRate * 1.8`

At scratch sensitivity `1.0x`, one full revolution of hand motion corresponds to `1.8 s` of buffer-lag movement.

## 3) Definitions

- `lag`: distance from read head to newest sample (`NOW`) in samples.
- `lagTarget`: desired lag set by UI gesture mapping.
- `lagActual`: realized lag from engine integration.
- `toward NOW`: decreasing lag.
- `away from NOW`: increasing lag.
- `deltaAngle`: signed angular platter movement in radians.
- `motionFresh`: short window after gesture updates where motion velocity is considered active.

## 4) Authoritative Mapping

All drag gestures must map with:

`lagDeltaSamples = -deltaAngle / (2*pi) * samplesPerRevolution * scratchSensitivity`

Notes:

- Negative sign means positive platter rotation toward NOW (convention may invert with UI orientation; the implementation must stay internally consistent).
- Radius of cursor contact does not change time-per-revolution mapping.
- Cursor radius may influence filtering/deadzone only, not core angle-to-time conversion.

## 5) Mode Behavior

### 5.1 Drag (unlocked cursor)

- Primary source is cursor-angle delta around platter center.
- The control anchor (`contactAngle`) tracks actual cursor angle.
- Deadzone is angular, derived from a small pixel threshold.

### 5.2 Drag (cursor lock)

- Uses tangential mouse delta to derive `deltaAngle`.
- Must preserve same `1.8 s/rev @ 1.0x` calibration.

### 5.3 Wheel

- Wheel is a separate interaction mode.
- It may use shaped acceleration/burst behavior, but must preserve directional symmetry targets and explicit forward compensation rules.

## 6) Live vs Freeze Compensation

### 6.1 Freeze

- Write head is stationary.
- Forward drag should not include write-head baseline compensation.

### 6.2 Live buffer

- Write head advances at +1x.
- During active manual motion (`motionFresh` true), forward tracking must include write-head baseline so forward drag does not feel resistant.
- When motion is not fresh and user is effectively holding still, velocity must settle to zero (no drift).

## 7) Rebase and Targeting Rules

When applying new drag deltas:

- Rebase toward NOW using the most-forward of (`lagTarget`, live lag sample).
- Rebase away from NOW using the farthest-back of (`lagTarget`, live lag sample).
- Never collapse accumulated gesture progress solely because DSP smoothing is behind UI event rate.

## 8) Visual Contract

- Platter visual rotation amount must be derived from realized read-head motion, not separate gesture-only blending.
- Direction and magnitude displayed should match audio-relevant movement.
- Optional debug overlay/marker may be used to show control anchor (`contactAngle`) vs realized phase during validation.

## 9) Acceptance Tests (Must Pass)

### A. Freeze 1-rev calibration

- Setup: `freeze=on`, sensitivity `1.0x`, start at `lag=2.0 s`.
- Action: rotate platter exactly one full turn toward NOW.
- Expectation: lag moves by `1.8 s +/- 5%`.

### B. Freeze linearity

- Setup: as above.
- Action: rotate `0.5 rev`, then another `0.5 rev`.
- Expectation: each half-turn contributes equal lag movement within `5%`.

### C. Live forward non-resistance

- Setup: `freeze=off`, sensitivity `1.0x`, start at `lag=2.0 s`.
- Action: one steady forward turn over ~`0.4-0.7 s`.
- Expectation: forward movement should not require multiple turns to approach NOW; result lag should be materially closer to NOW than start (`<= 0.4 s` target, tolerance `+/- 0.2 s`).

### D. Stationary hold

- Setup: drag touch held with negligible movement.
- Expectation: no directional creep; lag drift remains under `20 ms/s`.

### E. Direction symmetry (drag)

- Equal-magnitude clockwise/counterclockwise angle deltas at same sensitivity should produce equal-and-opposite lag deltas in freeze mode within `5%`.

## 10) Tunable Parameters (Limited Set)

Only these are allowed to tune feel after this spec:

- `kMouseScratchTravelScale` (global drag calibration; default target `1.0`).
- Drag deadzone threshold.
- Motion freshness duration bounds.
- Velocity smoothing coefficients.

Do not add new hidden compensations without updating this spec and acceptance tests.

## 11) Instrumentation Requirements

During tuning/debug builds, expose:

- `deltaAngle`
- `lagTarget`
- `lagActual`
- `motionFresh active`
- `write-head compensation active`

Any future feel change must report before/after results for tests A-E.

## 12) Implementation Policy

- One control equation for drag mapping.
- One explicitly documented compensation rule for live forward motion.
- One visual source of truth tied to realized engine motion.

Changes that violate the above require a spec update first.
