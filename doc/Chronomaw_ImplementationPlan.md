# Chronomaw Implementation Plan

## Implementation Readiness Decision

Chronomaw v1 is approved for implementation as a clean-room, Rack-native timing and modulation engine with documented PPW-inspired behavior.

Chronomaw v1 does not claim exact Pamela's Pro Workout firmware parity. Exact parity remains a later golden-test target. Any behavior not explicitly frozen in this document must be implemented as a Chronomaw v1 approximation and marked in the relevant registry entry.

## Module Identity

Module display name: `Chronomaw`

Recommended files:

```text
src/
  Chronomaw.cpp
  Chronomaw.hpp
  ChronomawWidget.cpp
  ChronomawWidget.hpp
  ChronomawEngine.hpp
  ChronomawEngine.cpp
  ChronomawState.hpp
  ChronomawParamRegistry.hpp
  ChronomawSourceRegistry.hpp
  ChronomawSerialization.hpp
  ChronomawQuantizer.hpp
  ChronomawClock.hpp
  ChronomawTimeline.hpp
  ChronomawUiWidgets.hpp

tests/
  chronomaw_serialization_spec.cpp
  chronomaw_clock_spec.cpp
  chronomaw_bank_spec.cpp
  chronomaw_output_seed_spec.cpp
  chronomaw_cross_quant_spec.cpp

res/
  chronomaw.svg
  chronomaw-dark.svg
```

## Constants

```cpp
static constexpr int kNumOutputs = 8;
static constexpr int kNumBanks = 64;
static constexpr float kDefaultBpm = 120.0f;
static constexpr float kMinBpm = 10.0f;
static constexpr float kMaxBpm = 330.0f;
static constexpr float kOutputMinV = 0.0f;
static constexpr float kOutputMaxV = 5.0f;
static constexpr int kDefaultExternalPpqn = 24;
```

## Bank And BPM Ownership

All musical state is patch-local in v1.

`LiveState` is the currently sounding state. `BankState` is saved material. Saving a bank copies live bank-owned state into the selected bank. Loading a bank copies selected bank state into live state.

BPM is both live-owned and bank-owned:

```text
Save Bank:
  bank.bpm = live.bpm
  bank.outputs = live.outputs

Load Bank:
  live.bpm = bank.bpm
  live.outputs = bank.outputs
  live.activeBank = selectedBank
```

`running` is live patch transport state only. It may be serialized with the patch, but it is not saved into banks.

Patch save serializes `LiveState`, all 64 `BankState` entries, and `UiState`. Patch load restores those same structures with schema defaults and clamping.

## Phase 1 Exclusions

Phase 1 excludes:

- Exact PPW firmware parity.
- Hardware backup import/export.
- Official ALM/VCV patch compatibility.
- AXON-style expander implementation.
- PPEXP1/PPEXP2-style companion modules.
- Full final PPW divisor, waveform, and quantizer parity.
- Plugin-global musical memory.
- Any musical state stored outside Rack patch JSON.

## DSP Order

Chronomaw v1 freezes this DSP order:

1. Read raw CV, clock, run, reset, and expander sources.
2. Detect rising/falling edges.
3. Apply run, reset, rotate, and bank-change events.
4. Resolve external clock phase and tempo updates.
5. Advance internal transport phase.
6. Generate per-output phase positions.
7. Apply per-output timing modifiers: divisor/multiplier, phase, flex, and loop.
8. Generate base shape, gate, or envelope.
9. Apply probability and Euclidean decisions at deterministic event boundaries only.
10. Apply cross operations.
11. Apply quantization.
12. Apply level, offset, invert, mute, and output clamp.
13. Write outputs, lights, meters, and history buffers.

Later parity tests may override this, but Phase 1 code and tests target this order.

## Parity Status

```cpp
enum class ParityStatus {
    FrozenChronomawV1,
    ManualDerived,
    Approximation,
    NeedsGoldenTest,
    Deferred
};
```

Every registry entry must declare a `ParityStatus`.

## Minimal Modifier Registry V1

```cpp
enum class ModifierFamily {
    Divider,
    Multiplier,
    Utility,
    Triggered
};

enum class ModifierId {
    Div_64,
    Div_32,
    Div_16,
    Div_8,
    Div_4,
    Div_3,
    Div_2,
    Div_1,
    Mul_2,
    Mul_3,
    Mul_4,
    Mul_8,
    Mul_16,
    Utility_Off,
    Utility_Gate,
    Utility_Start,
    Utility_Stop,
    Trigger_Envelope,
    Trigger_OneShot,
    Trigger_Ratchet,
    Trigger_Hold
};
```

## Minimal Shape Registry V1

```cpp
enum class ShapeId {
    Gate,
    Triangle,
    SawUp,
    SawDown,
    Sine,
    Square,
    RandomStepped,
    RandomSmooth,
    EnvelopeAR
};
```

Parameter visibility:

- `Gate`: width, level, offset, phase, probability, Euclidean, loop, flex.
- `Triangle`, `SawUp`, `SawDown`, `Sine`, `Square`: width where meaningful, slew, level, offset, phase, probability, loop, flex.
- `RandomStepped`: slew visible, width disabled.
- `RandomSmooth`: slew visible, width disabled.
- `EnvelopeAR`: width treated as attack/decay balance or disabled until triggered semantics are finalized.

## Minimal Cross Registry V1

```cpp
enum class CrossOp {
    None,
    Add,
    Subtract,
    Multiply,
    Min,
    Max,
    SampleAndHold
};
```

## Minimal Quantizer Registry V1

```cpp
enum class ScaleId {
    Off,
    Chromatic,
    Major,
    Minor,
    MajorPentatonic,
    MinorPentatonic,
    Dorian,
    Phrygian,
    Mixolydian,
    User1,
    User2,
    User3
};
```

User scale masks are patch-local in v1.

## JSON Schema V1

Top-level shape:

```json
{
  "schemaVersion": 1,
  "live": {},
  "banks": [],
  "ui": {}
}
```

Loading rules:

- Missing fields load defaults.
- Unknown fields are ignored.
- Enum values clamp to valid registry entries.
- Bank count clamps or pads to 64.
- Output count clamps or pads to 8.
- BPM clamps to `10-330`.
- User scale masks clamp to 12-bit values.
- Schema v1 does not use patch storage.

## Baseline Fixtures

Required Phase 1 fixtures:

```text
tests/fixtures/chronomaw/
  clock_24ppqn_run_reset.json
  euclid_loop_repeatability.json
  probability_seed_repeatability.json
  cross_before_quant.json
  level_zero_offset_constant_voltage.json
  triggered_modifier_reduced_params.json
```

Each fixture must define:

```json
{
  "name": "",
  "sampleRate": 48000,
  "durationBeats": 8,
  "initialState": {},
  "inputEvents": [],
  "expectedEvents": [],
  "expectedVoltageRanges": []
}
```

Baseline expectations:

- `level_zero_offset_constant_voltage.json`: with `shape = Gate`, `levelPct = 0`, `offsetPct = 50`, and `mute = false`, output remains approximately `2.5 V` for the full test duration.
- `cross_before_quant.json`: `Out2` applies `CrossOp.Add` with an `Out1` stepped source before `Chromatic` quantization; expected output reflects post-cross quantization.
- `probability_seed_repeatability.json`: identical initial state, fixed `randomSeed`, reset events, and transport events produce identical probability decisions across repeated renders.
- `clock_24ppqn_run_reset.json`: external `24 PPQN` clock with run gate high locks transport to external PPQN phase and realigns outputs after a reset edge at a beat boundary.
- `euclid_loop_repeatability.json`: Euclidean state plus `loop.beats > 0` and fixed seed repeats identically on each loop.
- `triggered_modifier_reduced_params.json`: triggered modifiers expose only the reduced triggered parameter subset through the registry; ordinary divider/multiplier-only fields are disabled with visible reasons.

## Phase 1 Definition Of Done

Phase 1 is complete when:

- Chronomaw builds as a Rack module.
- Eight outputs generate stable clock/modulation signals.
- Internal BPM clock works from `10-330 BPM`.
- External `24 PPQN` clock plus run sync is tested.
- `LiveState` and 64 `BankState` entries serialize and deserialize.
- Bank save/load copies BPM and output states according to this plan.
- Output reset and seed reset work.
- Probability decisions are seed-repeatable.
- Cross processing occurs before quantization.
- `Level = 0` and `Offset > 0` produces constant voltage.
- Utility and triggered modifiers expose reduced parameter sets through the registry.
- UI shell supports global bar, overview, selected-output inspector, and basic timeline.
- Monitor/Edit density modes do not alter engine state.
