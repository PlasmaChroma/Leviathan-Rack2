# TemporalDeck: Current Functionality (Codex Snapshot)

This document describes the **currently implemented** behavior in `src/TemporalDeck.cpp`.

## Scope
- Module: `TemporalDeck`
- Model slug: `TemporalDeck`
- Buffer model: stereo circular buffer, up to 8 seconds
- Interpolation: cubic read

## Controls and I/O

### Params
- `BUFFER`: sets accessible buffer span from `0..8s`
- `RATE`: transport rate knob (displayed as mapped speed, not raw knob value)
- `MIX`: dry/wet blend
- `FEEDBACK`: input/output feedback into the write path
- `FREEZE` (latched button)
- `REVERSE` (latched button)
- `SLIP` (latched button)

### Inputs
- `POSITION CV`
- `RATE CV`
- `INPUT L`
- `INPUT R` (normalled to L if unpatched)
- `SCRATCH GATE`
- `FREEZE GATE`

### Outputs
- `OUTPUT L`
- `OUTPUT R`

### Lights
- `FREEZE`
- `REVERSE`
- `SLIP`

## Transport and Buffer Semantics

- Internal buffer size is `sampleRate * 8` samples.
- `writeHead` is next-write index.
- "NOW" is treated as **newest readable sample** (`writeHead - 1` wrapped).
- Read path uses cubic interpolation (`readCubic`).
- Default transport speed is from a mapped rate law (`baseSpeedFromKnob`) plus rate CV.

## Rate Mapping

`RATE` knob mapping (before CV/reverse):
- `0.0` -> `-2.0x`
- `0.5` -> `1.0x` (neutral)
- `1.0` -> `2.0x`

Then:
- add `RATE CV / 5` (clamped contribution)
- final clamp to `[-3.0x, 3.0x]`
- apply reverse sign flip if reverse is enabled

Tooltip/display for `RATE` uses `DeckRateQuantity` and shows multiplier (e.g. `1.00x`) rather than raw `0..1` knob value.

## Scratch Paths

### Manual platter scratch (mouse drag)
- Drag updates platter lag target from angular motion.
- Travel scaling:
  - `kMouseScratchTravelScale = 4.0`
- Engine keeps a persistent `scratchLagTargetSamples` and follows it with shaped smoothing:
  - `kScratchFollowTime = 0.012s`
  - eased follow curve
  - soft lag-step limiting using tanh:
    - `kScratchSoftLagStepLimit = 10.0`
- Result: directional, smoothed scratch response with reduced extreme pitch spikes.

### Mouse wheel scratch
- Active only when hovering over platter area.
- Wheel updates lag target directly (no wheel velocity injection currently).
- Travel scaling:
  - `kWheelScratchTravelScale = 4.5`
- Short hold window keeps wheel gestures acting as brief scratch gestures:
  - slip off: ~`20ms`
  - slip on: ~`90ms`

### Scratch gate + position CV (external scratch)
- If `SCRATCH GATE` is high and `POSITION CV` is connected, lag follows position CV mapping directly.

## Slip Behavior

Slip is a latched mode (`SLIP` button/menu state). Current behavior:

- On entering scratch while slip is enabled:
  - timeline reference is captured (`timelineHead = readHead`).
- On scratch release with slip enabled:
  - slip return is armed.
- On enabling slip while not scratching:
  - if current lag is above `kSlipEnableReturnThreshold` (`64` samples), return is armed.
  - otherwise timeline is synced and no return is armed.

### Slip return shape
Two-stage return to NOW:
1. Exponential-like lag reduction (`kSlipReturnTime = 0.12s`)
2. Final shaped catch when lag enters `kSlipFinalCatchThresholdMs = 120ms`
   - final catch duration: `kSlipFinalCatchTime = 0.035s`
   - snaps to exact NOW when complete

## Freeze and Reverse

### Freeze
- `FREEZE` button or `FREEZE GATE` high holds transport speed to zero.
- While freeze is active, buffer writes are suppressed.

### Reverse
- Reverse flips effective transport direction.
- Current implementation has edge-hold behavior at oldest accessible point:
  - if reverse reaches oldest edge (`lag >= limit` approximately), reverse speed is forced to zero
  - writes are also held at that edge
- Net effect: reverse stops at oldest accessible edge rather than wrapping.

## Position CV Mode

- Position CV can operate in absolute or offset mode.
- Context menu option:
  - `Position CV offset mode`

## UI and Visuals

### Display
- Yellow lag arc with multi-stroke glow.
- Limit dot.
- Top-right lag readout in milliseconds (`N ms`).

### Platter visual
- Continuous phase accumulator (`platterPhase`) avoids seam-reset jumps.
- Groove rings are procedural with per-ring wobble/rotation offsets.
- Platter rotation UI updates are published every sample for responsiveness.

### UI publish throttling
- General UI fields are throttled to `120 Hz`:
  - `uiLagSamples`
  - `uiAccessibleLagSamples`
  - `uiSampleRate`
- `uiPlatterAngle` is intentionally published at audio rate for scratch feedback.

## Persistence

Persisted JSON fields:
- `freezeLatched`
- `reverseLatched`
- `slipLatched`
- `positionCvOffsetMode`

## Notable Current Caveats

1. Reverse edge behavior currently applies at whatever the accessible limit is (not only when full 8s is filled), so partial-buffer reverse can also stop at the oldest edge.
2. Scratch feel and wheel feel are currently tuned with high travel multipliers and smoothing; they are functional but likely still subject to further voicing.
3. Slip return is intentionally stylized (two-stage) rather than strictly physical.

## Constants Snapshot (current)

- `kSlipReturnTime = 0.12s`
- `kSlipEnableReturnThreshold = 64 samples`
- `kSlipFinalCatchThresholdMs = 120ms`
- `kSlipFinalCatchTime = 0.035s`
- `kScratchFollowTime = 0.012s`
- `kScratchSoftLagStepLimit = 10.0 samples/step (soft-limited)`
- `kMouseScratchTravelScale = 4.0`
- `kWheelScratchTravelScale = 4.5`
- `kUiPublishRateHz = 120`
- `kNominalPlatterRpm = 33.333333`

