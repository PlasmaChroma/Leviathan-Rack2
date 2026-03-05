# Maths (Revision 2013 / "Maths 2") - VCV Rack 2 Emulation

## Codex-Ready Implementation Specification (Known Behavior First)

## Purpose

Implement a VCV Rack 2 module that emulates documented and verified behavior of:
- CH1/CH4 dual slope generators
- CH2/CH3 attenuverting offset channels
- SUM/OR/INV bus behavior

This guide is designed to be implementation-ready while clearly tagging unknowns as TODOs.

## Non-goals for this pass

- Do not chase hardware-identical feel yet.
- Leave tunable/calibration constants explicit and isolated.
- Prefer deterministic placeholder behavior where hardware data is missing.

## 1) Versioning and Feature Set

### Primary target: Revision 2013

- Includes Cycle CV inputs (gate-qualified cycling) for CH1 and CH4.
- Includes INV output.
- Includes CH1/CH4 Unity outputs.
- CH2 input normalizes to +10V when unpatched.
- CH3 input normalizes to +5V when unpatched.

### Optional compatibility mode: Classic (Maths 1)

- No INV output.
- CH2 and CH3 both normalize to +5V.
- Bus normalization differs from 2013.

Implementation approach: keep a revision flag (`REV_2013` default, `REV_CLASSIC` optional).

## 2) Voltage Conventions and Rack Interop

### Verified signal ranges (Revision 2013 target)

- CH1/CH4 Signal inputs: +/-10V
- CH1/CH4 Rise/Fall/Both CV inputs: +/-8V
- Variable outputs 1-4: +/-10V
- SUM and INV outputs: +/-10V nominal range
- OR output: 0V to +10V, no negative response
- EOR/EOC: binary 0V or +10V

### Trigger and gate handling

Use Rack idioms unless the hardware behavior is explicitly specified.

- Trigger detection: `dsp::SchmittTrigger`
- Event pulses (if used): `dsp::PulseGenerator`

Known exception:
- Cycle CV gate threshold for HIGH should be treated as +2.5V.

## 3) Module Interface Contract

Names below are implementation IDs, not required panel labels.

### Params

CH1:
- Rise
- Fall
- Shape (Vari-Response)
- Cycle (latch/button)
- Attenuverter (drives Variable Out 1)

CH2:
- Attenuverter

CH3:
- Attenuverter

CH4:
- Rise
- Fall
- Shape
- Cycle (latch/button)
- Attenuverter (drives Variable Out 4)

### Inputs

CH1:
- Signal
- Trigger
- Rise CV
- Fall CV
- Both CV
- Cycle CV

CH2:
- Signal (normalized when unpatched)

CH3:
- Signal (normalized when unpatched)

CH4:
- Signal
- Trigger
- Rise CV
- Fall CV
- Both CV
- Cycle CV

### Outputs

- Variable Out 1
- Variable Out 2
- Variable Out 3
- Variable Out 4
- CH1 Unity Out
- CH4 Unity Out
- EOR (CH1)
- EOC (CH4)
- SUM Out
- INV Out (2013)
- OR Out

### Lights

At minimum:
- CH1 Cycle
- CH4 Cycle
- EOR
- EOC
- CH1/CH4 unity level indicators
- OR and INV indicators (if panel provides them)

## 4) DSP Architecture

### 4.1 Polyphony

- Monophonic only for this module.

### 4.2 Common helper conventions

- `dt = args.sampleTime`
- `clamp10(x) = clamp(x, -10.f, 10.f)`
- `clampPos10(x) = clamp(x, 0.f, 10.f)`

TODO/UNKNOWN:
- Whether analog clipping should be modeled as soft saturation instead of hard clamp.

### 4.3 CH2 and CH3 offset/attenuverting channels

Verified behavior:
- Input normalizations:
  - Rev 2013: CH2 = +10V if unpatched, CH3 = +5V if unpatched
  - Classic: CH2 = +5V, CH3 = +5V
- Attenuverter noon = 0 output, CW positive, CCW negative/inverted.

Codex-ready behavior:
- `in = inputConnected ? inputVoltage : normRef`
- `gain = mapAttenuverter(param)` with noon at 0, bipolar gain at extremes
- `var = clamp10(in * gain)`

TODO/UNKNOWN:
- Exact gain max (for "small amount of gain") requires measurement.

### 4.4 CH1 and CH4 slope channels

#### 4.4.1 Dual role

Each outer channel supports both:
- Function generation (triggered/cycling envelope)
- Slew/lag processing of Signal input

#### 4.4.2 Core rules

- Trigger transient shape is 0 -> peak -> 0, no sustain.
- Retrigger ignored during Rising, accepted during Falling.
- Rise/Fall CV are linear time controls.
- Both CV is bipolar exponential time control, opposite polarity vs Rise/Fall CV.
- Shape control spans LOG <-> LIN <-> EXPO-like behavior and affects timing behavior.

#### 4.4.3 Cycle behavior (Rev 2013)

- Cycle can be enabled by button latch or Cycle CV gate.
- Treat +2.5V and above as Cycle CV HIGH.

TODO/UNKNOWN:
- If Cycle gate drops mid-cycle, choose deterministic default and mark TODO.

## 5) Normalization and Mix Bus Logic

### 5.1 Revision 2013 bus rules

- Variable outs (1-4) are normalled to SUM/OR buses.
- If a Variable Out jack is patched, that channel is removed from bus injection (break-normal behavior).
- CH1/CH4 Unity outputs are not part of bus normalization.

### 5.2 Classic mode notes

- Keep separate TODO if classic bus behavior differs in your final implementation.

### 5.3 Bus math

Let `v1..v4` be variable channel values that are injected into the buses only when their corresponding variable out jack is unpatched.

- `SUM = clamp10(v1 + v2 + v3 + v4)`
- `INV = clamp10(-SUM)` (Rev 2013)
- `OR = clampPos10(max(0, v1, v2, v3, v4))`

TODO/UNKNOWN:
- Analog OR/SUM non-idealities (diode drop, soft saturation) are not locked.

## 6) Outer-Channel State Machine (CH1 and CH4)

Use one state machine per outer channel.

```text
enum Phase { IDLE, RISING, FALLING }

struct SlopeVoice {
  Phase phase;
  float y;            // unity-domain core output
  bool transientMode; // true for trigger/cycle envelope mode
  bool cycleLatched;
}
```

### Inputs read per process block

- Signal input voltage
- Trigger input edge
- Rise CV, Fall CV, Both CV
- Cycle button edge/latch
- Cycle CV gate state

### Cycle enable

`cycleEnabled = cycleLatched || cycleGateHigh`

Placeholder default if gate drops:
- Finish current segment, then return to IDLE.

### Trigger acceptance

On trigger event:
- If phase is not Rising: begin Rising transient.
- If phase is Rising: ignore trigger.

TODO/UNKNOWN:
- Falling retrigger restart semantics (restart from current vs hard reset) need measurement.

### Slew mode

When not in transient envelope behavior:
- Process Signal input with asymmetric slew (Rise and Fall rates).
- Preserve the rule that the circuit should not speed up external voltage transitions beyond pass-through behavior.

## 7) Output Routing and Attenuverters

### Unity outputs

- CH1/CH4 Unity outs are direct from outer-channel core `y`.
- Not affected by center attenuverters.

TODO/UNKNOWN:
- Exact peak behavior for triggered/cycle cases (8V vs 10V details) needs final measurement lock.

### Variable outputs

- CH1 Var = attenuverted CH1 unity
- CH2 Var = attenuverted CH2 input/offset path
- CH3 Var = attenuverted CH3 input/offset path
- CH4 Var = attenuverted CH4 unity

Codex-ready default:
- `var = clamp10(source * attenGain)`

## 8) EOR and EOC Behavior

### Verified event meaning

- EOR: CH1 end-of-rise event
- EOC: CH4 end-of-cycle (end-of-fall) event
- Binary voltage levels (0V / +10V)

### Codex-ready default model

If using phase gates (recommended placeholder):
- `EOR = 10V when phase == FALLING else 0V`
- `EOC = 10V when phase != FALLING else 0V`

Alternative acceptable placeholder:
- 1ms event pulses via `dsp::PulseGenerator`

TODO/UNKNOWN:
- Final choice between gate-style vs pulse-style behavior must be measurement-backed.

## 9) Explicit TODO/UNKNOWN List

Leave these clearly tagged in code:

1. Rise/Fall knob law constants and mapping.
2. Both CV exponential constant and unit interpretation.
3. Shape mapping and time-coupling constants.
4. Saturation model (hard clamp vs soft clip).
5. Trigger threshold/hysteresis exact hardware values.
6. Falling-phase retrigger restart semantics.
7. Cycle gate drop behavior mid-segment.
8. Unity output peak specifics in each mode.
9. OR/SUM analog non-idealities.

## 10) Implementation Notes for Rack

- Use `params[]`, `inputs[]`, `outputs[]`, and `lights[]` directly in `process()`.
- Use `dsp::SchmittTrigger` for edge detection.
- Keep calibration constants centralized so measurements can be dropped in later.
- Prefer deterministic placeholder behavior over ambiguous emulation guesses.

## References

- https://vcvrack.com/manual/VoltageStandards
- https://vcvrack.com/manual/PluginGuide
- https://vcvrack.com/manual/PluginDevelopmentTutorial
