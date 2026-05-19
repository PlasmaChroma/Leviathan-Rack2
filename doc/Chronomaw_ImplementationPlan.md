# Chronomaw Implementation Plan

## Implementation Readiness Decision

Chronomaw v1 is approved for implementation as a clean-room, Rack-native timing and modulation engine with documented PPW-inspired behavior.

Chronomaw v1 does not claim exact Pamela's Pro Workout firmware parity. Exact parity remains a later golden-test target. Any behavior not explicitly frozen in this document must be implemented as a Chronomaw v1 approximation and marked in the relevant registry entry.

## Current Status Snapshot

Status note updated from current code scaffold. This section records implementation reality; the rest of the document remains target behavior.

Implemented:

- Module identity and integration are live: `Chronomaw` model exists and is registered.
- Core constants are present in `ChronomawState.hpp`: `8` outputs, `64` banks, BPM `10-330`, output clamp `0-5 V`, external PPQN default `24`.
- Rack surface v1 enums are present and wired (`ParamId`, `InputId`, `OutputId`, `LightId`).
- Panel widget scaffold exists and reads SVG component anchors through `PanelSvgUtils` with fallback coordinates.
- Dedicated panel `Reset All` action was removed; module reset is handled through Rack's standard module Initialize flow.
- Bank save/load copies `bpm` and `outputs` between `LiveState` and selected `BankState`.
- JSON schema root contains `schemaVersion`, `live`, `banks`, `ui`.
- JSON now serializes and restores:
  - `live`: bpm/running/activeBank/density plus full live `outputs`.
  - `banks`: bpm plus full per-bank `outputs`.
  - `ui`: selected output/tab.
- Basic engine loop exists with run/reset behavior and per-output mute/level/offset/invert clamp.
- Rack-linked serialization test exists and passes (`chronomaw_serialization_spec`).
- Custom Phase 1 UI scaffold is now active:
  - Click-to-select output rows.
  - Inspector tab switching.
  - Inspector editing for `phasePct`, `levelPct`, `offsetPct`, `probabilityPct`, `muted`, `invert`, and `randomSeed`.
  - Density-aware timeline/inspector layout.
- Timeline visualization is active with:
  - Fixed `now` cursor.
  - Multi-second history and deterministic future projection.
  - Matched seconds-per-pixel scale across history and future regions.
  - Smoothed interpolation to avoid jitter.

Partial:

- Engine timing is scaffold-level and does not yet implement the frozen Chronomaw DSP order.
- External clock support is minimal (clock input connectivity currently drives sync light; no PPQN lock model yet).
- Output generation is currently a shared gate-like placeholder, not per-output modifier/shape/probability/euclid/cross/quant behavior.
- Pattern inspector controls (`probabilityPct`, `randomSeed`) are state-editable but not yet musically active in DSP.
- Timeline future projection is currently accurate for the implemented placeholder gate/level/offset/invert/mute behavior, not final registry-driven behavior.

Missing:

- Modifier/shape/cross/quantizer registries beyond minimal enum stubs.
- Deterministic random event boundary semantics and seed reset handling.
- Full source registry and assignable CV plumbing into output parameters.
- Dedicated BPM text readout (`BPM xxx.x` + `INT/EXT` + lock indication) in the global bar.
- Overview badges for probability/Euclid/loop/cross/quant/CV status (currently row selection and base labels are implemented).
- Phase-1 fixture tests listed in this plan (`chronomaw_*_spec.cpp` set).

Immediate next steps:

1. Implement `chronomaw_bank_spec.cpp` to lock save/load copy semantics for BPM and outputs.
2. Add global BPM readout (`BPM xxx.x`) with `INT/EXT` source badge and sync lock indication.
3. Replace placeholder shared gate engine with per-output phase/divider scaffold so outputs are no longer identical.
4. Activate pattern semantics in DSP (probability decisions and seed-repeatable boundaries).
5. Introduce minimal quantizer/cross ordering harness to start aligning with frozen DSP order.

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

## Rack Integration Contract

Phase 1 uses these repo-facing identifiers:

```cpp
extern Model* modelChronomaw;
Model* modelChronomaw = createModel<Chronomaw, ChronomawWidget>("Chronomaw");
```

Add `modelChronomaw` to `src/plugin.hpp` and register it in `init()` in `src/plugin.cpp`.

Add this module entry to `plugin.json`:

```json
{
  "slug": "Chronomaw",
  "name": "Chronomaw",
  "description": "Rack-native eight-output clock and modulation engine with direct visual editing.",
  "tags": [
    "Clock generator",
    "Function Generator",
    "Sequencer",
    "Random"
  ],
  "hidden": false
}
```

## Rack Surface V1

Chronomaw is unreleased, so enum ordering can be chosen for implementation clarity in Phase 1. Once any public build ships, append-only compatibility rules apply to `ParamId`, `InputId`, `OutputId`, and `LightId`.

Initial module surface:

```cpp
enum ParamId {
    RUN_PARAM,
    BPM_PARAM,
    ACTIVE_BANK_PARAM,
    LOAD_BANK_PARAM,
    SAVE_BANK_PARAM,
    SELECTED_OUTPUT_PARAM,
    DENSITY_MODE_PARAM,
    PARAMS_LEN
};

enum InputId {
    CLK_INPUT,
    RUN_INPUT,
    RESET_INPUT,
    CV_1_INPUT,
    CV_2_INPUT,
    CV_3_INPUT,
    CV_4_INPUT,
    INPUTS_LEN
};

enum OutputId {
    OUT_1_OUTPUT,
    OUT_2_OUTPUT,
    OUT_3_OUTPUT,
    OUT_4_OUTPUT,
    OUT_5_OUTPUT,
    OUT_6_OUTPUT,
    OUT_7_OUTPUT,
    OUT_8_OUTPUT,
    OUTPUTS_LEN
};

enum LightId {
    RUN_LIGHT,
    SYNC_LIGHT,
    OUT_1_LIGHT,
    OUT_2_LIGHT,
    OUT_3_LIGHT,
    OUT_4_LIGHT,
    OUT_5_LIGHT,
    OUT_6_LIGHT,
    OUT_7_LIGHT,
    OUT_8_LIGHT,
    LIGHTS_LEN
};
```

The four `CV_*` inputs are built into the main module for Phase 1 assignable CV. AXON-style expansion remains deferred and adds more sources later through the source registry.

`LOAD_BANK_PARAM` and `SAVE_BANK_PARAM` are momentary action params. `ACTIVE_BANK_PARAM`, `SELECTED_OUTPUT_PARAM`, and `DENSITY_MODE_PARAM` are UI-facing state selectors. Primary per-output editing is owned by the custom editor/state model rather than by hundreds of Rack params in Phase 1.

## Panel Layout V1

Canonical Phase 1 layout targets `40 HP` and spends width to preserve vertical editor space.

Physical component placement:

- Left rail: vertically stack `CLK_INPUT`, `RUN_INPUT`, `RESET_INPUT`, then `CV_1_INPUT` through `CV_4_INPUT`.
- Right rail: vertically stack `OUT_1_OUTPUT` through `OUT_8_OUTPUT`.
- Output activity lights sit immediately inside the right output rail, one light per output.
- The top center global bar holds run, BPM, active bank, load/save, selected output, density mode, run light, and sync light.
- The center column below the global bar is reserved for custom UI widgets rather than jacks.

SVG anchor rectangles:

```text
GLOBAL_BAR_RECT: top-center transport and bank strip.
OVERVIEW_RECT: left-center eight-output state summary.
TIMELINE_RECT: upper-right-center shared timeline.
INSPECTOR_RECT: lower-right-center selected-output editor.
```

The `components` layer in `res/chronomaw.svg` is the source of truth for Phase 1 component anchors. It may be hidden visually; `ChronomawWidget` must continue to resolve positions by SVG element ID through `PanelSvgUtils` and use matching C++ fallback positions.

## Baseline Custom Interface V1

Chronomaw's primary interface is not a Pam-style one-knob menu. Phase 1 must establish a direct visual editor where the user can monitor all eight outputs and edit the selected output without long-presses, nested menus, or context-menu-only primary controls.

The baseline interface has four persistent surfaces:

- `ChronomawGlobalBarWidget`: transport, BPM, sync state, active bank, load/save actions, selected output, and density mode.
- `ChronomawOverviewWidget`: eight always-visible output rows for selection and status.
- `ChronomawTimelineWidget`: shared all-output timing view or compact event strip, depending on density mode.
- `ChronomawInspectorWidget`: selected-output editor with fixed tabs and exact value controls.

These widgets may initially live in `ChronomawUiWidgets.hpp` / `ChronomawWidget.cpp`; split them into separate files only when the implementation becomes large enough to justify it.

### Better-Than-Pam Contract

The interface is considered baseline-usable only when these tasks do not require a context menu:

- Select any of the eight outputs.
- See whether every output is muted, simple, probabilistic, looped, quantized, cross-modulated, or CV-controlled.
- Edit the selected output's mute, level, offset, phase, probability, invert, and seed/reset behavior.
- Change BPM, run state, active bank, selected output, and density mode.
- Save or load the active bank with clear visual feedback about the selected bank.
- Switch between Monitor and Edit density without losing selected output, selected tab, or timeline position.

Global tempo visibility requirement:

- The global bar must include a dedicated BPM readout text field, not just a BPM knob.
- Readout format for Phase 1: `BPM xxx.x` (one decimal place).
- Readout must update in real time when BPM changes from panel edits, bank load, or external sync state updates.
- Readout should include a compact tempo-source badge: `INT` for internal tempo and `EXT` for externally clocked tempo.
- When external clock is connected and driving transport, the BPM readout area must visibly indicate sync-locked vs unlocked state.

Primary editing target: common edits should be reachable in at most two user actions after the module is visible:

- Select output row, then edit visible inspector control.
- Select output row, then select visible tab.
- Select tab, then edit visible control.

No Phase 1 primary workflow may depend on long-press, hidden encoder state, or a Rack context-menu item.

### Output Overview V1

The overview is an eight-row dashboard. It must remain visible in Monitor and Edit modes.

Each row shows:

- Output number and selected-row highlight.
- Mute state.
- Current modifier label or placeholder (`/1` until modifier registry is active).
- Shape label or icon (`Gate` until shape registry is active).
- Badges for probability, Euclidean, loop, quantizer, cross, and CV assignment.

Badge rules:

- Enabled badge: visible and high-contrast.
- Disabled but available badge: dim.
- Unsupported in current scaffold: hidden or marked as `--`; do not fake active features.
- Conditional unavailable parameter: visible in the inspector but disabled with a short reason.

Clicking any row updates `UiState.selectedOutput` and the `SELECTED_OUTPUT_PARAM` facing selector. The selected output must be highlighted consistently in the overview, timeline, and inspector.

### Timeline V1

The timeline is a timing/event surface, not a generic oscilloscope. It shows all outputs in a shared transport frame.

Phase 1 minimum:

- Eight lanes matching outputs `1-8`.
- A clear `now` cursor.
- Recent gate/activity trace for each lane from engine history buffers (left of `now`).
- Deterministic short-horizon future projection for each lane (right of `now`) for currently implemented engine behavior.
- Selected-output emphasis without hiding other lanes.
- Mute overlay showing the output is forced to `0 V` while internal phase continues.
- Compact mode that remains visible in Edit density.
- One consistent time scale (seconds per pixel) across history and future regions.

Phase 1 may defer probability previews, Euclidean hit/rest rendering, and cross-dependency markers until the engine exposes stable registry-driven snapshot data.

The timeline reads copied/snapshotted state. It must not advance transport, RNG state, event counters, bank state, or audio-thread-owned engine state.

### Inspector V1

The inspector edits the selected output through fixed tabs:

- `Timing`
- `Shape`
- `Pattern`
- `Cross`
- `CV`
- `Quant`
- `Store`

Initial scaffold controls are allowed to be concentrated in the tabs where they make sense using the fields already present in `OutputState`:

- `Timing`: phase.
- `Shape`: level, offset, invert.
- `Pattern`: probability, random seed, output seed reset action.
- `Store`: copy/paste/reset selected output, save/load bank actions if not already provided in the global bar.

Tabs for unavailable registries still exist but render disabled controls with short reasons, for example `Cross registry not implemented yet` or `CV assignment not implemented yet`. Do not remove the tabs; visible disabled structure is better than hidden capability.

### Custom Value Controls

Every editable numeric custom control must support:

- Vertical drag using Rack-style fine adjustment behavior where practical.
- Mouse wheel or step adjustment.
- Double-click reset to the documented default.
- Exact value entry through a small inline editor or Rack-style right-click value action before public release.
- Tooltip or local label containing units and range.

Controls write through validated state-update functions. They must clamp ranges the same way serialization clamps ranges.

### Density Modes V1

`DENSITY_MODE_PARAM` selects layout density only. It must not alter engine state.

Monitor mode:

- Global bar visible.
- Overview visible.
- Timeline receives the largest center area.
- Inspector collapses to a compact selected-output summary.

Edit mode:

- Global bar visible.
- Overview visible.
- Timeline remains as a compact event strip.
- Inspector receives the largest center area.

Focus mode is optional in Phase 1. If implemented, it is for deep selected-output editing or debugging and must remain reversible without changing engine state.

### UI Implementation Order

Implement the custom interface in this order:

1. Passive draw: render global bar, overview rows, timeline lanes, inspector frame, and tabs from current state without editing behavior.
2. Selection and density: wire output row selection, selected-output highlighting, and Monitor/Edit layout changes.
3. Existing-state editing: add inspector controls for `mute`, `levelPct`, `offsetPct`, `phasePct`, `probabilityPct`, `invert`, and `randomSeed`.
4. Timeline history: draw current/recent output activity from engine history buffers or a lightweight UI snapshot.
5. Registry-driven editing: replace placeholders with modifier, shape, cross, CV, and quantizer controls as those registries become real.

The first implementation milestone is not visual polish. It is proving that the module can be operated from the custom surface without falling back to a hidden menu model.

## State Representation Contract

Phase 1 uses a hybrid state model:

- Rack params hold only global panel controls and action triggers listed in `Rack Surface V1`.
- Per-output musical settings live in `LiveState.outputs` and `BankState.outputs`.
- Per-output editor widgets write through validated state-update functions, not directly through scattered widget-local fields.
- `dataToJson()` serializes all custom musical state, all banks, random seeds, CV assignments, quantizer masks, and patch-local UI state.
- `dataFromJson()` validates then applies a complete state snapshot before audio processing observes it.
- Tests should target state/engine/serialization APIs directly before the full custom UI exists.

The consequence is explicit: Phase 1 does not get Rack's automatic undo/redo for every per-output field unless specific custom history actions are added. This is acceptable for the scaffold, but any high-risk destructive actions such as reset, bank load, or paste should receive explicit history support before public release.

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
- UI shell supports global bar, overview, selected-output inspector, and basic timeline/event strip.
- Global bar includes a dedicated BPM readout (`BPM xxx.x`) with source/status indication (`INT`/`EXT`, lock state).
- Overview shows all eight outputs and supports direct selected-output changes.
- Inspector edits the existing Phase 1 `OutputState` fields without context-menu-only workflows.
- Timeline/event strip remains visible in both Monitor and Edit density modes.
- Monitor/Edit density modes do not alter engine state.
