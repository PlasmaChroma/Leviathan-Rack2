# Pamela's Pro Workout Rack-Native Redesign

## Product Decision

Build a Rack-native reinterpretation of Pamela's Pro Workout rather than a visual clone of the hardware panel. Preserve the PPW timing engine, patch semantics, voltage behavior, bank behavior, per-output behavior, and expander concepts where relevant. Replace the encoder-and-menu workflow with a direct mouse-first editor built around overview, inspection, visual editing, and exact entry.

The central design rule is simple: preserve the PPW brain, discard the PPW body language.

The official ALM/VCV version already proves that the PPW behavior can exist in Rack with the same hardware-style UI. This project should not compete by reproducing that UI. Its value is a Rack-native interaction layer that makes PPW's hidden state visible and editable without long-presses, serial page navigation, or context-menu dependence for primary work.

## Compatibility Tier

This module targets behavioral compatibility where documented and musically observable. It does not target binary firmware compatibility, official ALM/VCV patch compatibility, or hardware-backup compatibility unless a later import/export layer explicitly implements those paths.

Compatibility priority order:

- Preserve musical semantics: timing, phase, reset, probability, loop, Euclidean generation, cross processing, quantization, and voltage behavior.
- Preserve user-facing ranges and names where legally and practically safe.
- Preserve patch-local behavior inside this module.
- Do not guarantee official ALM/VCV patch compatibility.
- Do not claim exact firmware parity unless backed by golden tests.

## Source Notes

Primary source: user-supplied *Pamela's Pro Workout* manual dated March 31, 2026, manual version 0.35 / firmware 130.

Cross-check sources used during the original research pass:

- ALM hosted PPW manual matching the March 31, 2026 version.
- ALM Pamela's Pro Workout product page.
- VCV Library listing for the official Pamela's Pro Workout module.
- VCV Rack 2 documentation covering panel design, custom widgets, serialization, expanders, context menus, dark panels, voltage standards, and parameter interaction.

Known source conflict: the March 2026 manual says the tempo range is `10-330 BPM` and storage is `64 banks`. ALM's product page still describes `10-303 BPM` and `7 banks / 56 slots`. Treat the March 2026 manual as the authority for current behavior unless direct firmware or official VCV behavior proves otherwise.

## Scope

### V1

- Main Pam-style timing module with eight outputs.
- Internal BPM clock and external clock/run sync.
- Per-output divisors/multipliers, waveform, width, slew, level, offset, phase, probability, Euclidean rhythm, loop, flex, cross operations, invert, quantization, scope/preview, save/load/reset, and assignable CV modulation.
- PPW output voltage behavior: `0-5 V` outputs by default.
- Configurable CV input range where relevant: `5 V` or `10 V`.
- 64-bank model if the March 2026 manual remains the target authority.
- Rack-native overview-plus-inspector UI.
- Patch save/load serialization for all module state.
- Automated tests for serialization, bank operations, clock/run sync, reset behavior, and deterministic seed behavior.

### V1 If Engine Access Exists

- Reuse an existing PPW-compatible timing/signal engine and build the new interaction shell around it.
- Add golden tests around known PPW behaviors before changing UI-facing state or serialization.

Assume no official PPW/VCV engine code is reusable unless it is explicitly available under a compatible license or provided by the rights holder. Until proven otherwise, implement a clean-room engine from public behavior, manual documentation, and user-measured or golden-test observations.

### Deferred

- AXON-2-style companion expander, unless the initial engine architecture already exposes the required CV/button source abstractions cleanly.
- Hardware-backup import/export.
- PPEXP1/PPEXP2-style MIDI/DIN companion modules.
- Exact hardware backup file compatibility.
- Deep compatibility shortcuts matching the official ALM/VCV module's keyboard workflow.

### Explicitly Out Of Scope

- A one-encoder clone of the hardware UI.
- Long-hold gestures for primary workflows.
- Hiding primary editing behind the context menu.
- Recreating ALM branding, trade dress, or protected panel identity.

## State Locality Rule

Default rule: all musical state is patch-local.

Patch-local musical state includes:

- Live output settings.
- Banks.
- Output stores.
- User scales.
- Random seeds.
- CV assignments.
- Quantizer selections.
- Selected output and selected tab, if useful for restoring the editing context.

Plugin-global state is allowed only for non-musical preferences:

- Default theme.
- Tooltip verbosity.
- Debug display defaults.

Hardware-style global memory and imported bank libraries may be added later as compatibility utilities, but they must be clearly labeled because global musical state can make Rack patches non-self-contained.

## Blocking Unknowns

Resolve these before implementation is treated as parity work:

- Exact divisor/multiplier enum list, including any firmware-130 additions.
- Exact waveform list and which parameters are available for each waveform.
- Exact triggered modifier names, ranges, and reduced parameter sets.
- Exact built-in quantizer scale list and user-scale persistence behavior.
- Full DSP ordering beyond the known requirement that cross operations occur before quantization.
- Whether existing PPW/VCV core code is legally and practically available for reuse.
- Whether 64 banks are patch-local, device/global, or implemented as a hybrid in the official behavior.
- How manual bank save/load maps onto Rack patch persistence and autosave expectations.

If exact parity matters, validate these against the official VCV module or hardware firmware with golden tests. If exact parity is not available, document each approximation explicitly in the implementation notes.

## Parity Requirements

Pamela's Pro Workout is an eight-channel BPM-synced modulation and clock engine. Feature parity means preserving behavior, not preserving encoder choreography.

Required behavior families:

- Master BPM clock.
- Eight independent outputs.
- Divisors/multipliers from the documented minimum through maximum range.
- Waveform selection.
- Width and slew behavior, including shape-dependent controls.
- Level and offset as percentages of the `0-5 V` output range.
- Phase.
- Probability.
- Euclidean generation.
- Looping with beats, nap, wake, and shift.
- Cross-output/source operations before quantization.
- Flex microtiming.
- Invert.
- Quantization and user scales.
- Per-output preview/scope.
- Per-output save, load, reset, and seed reset.
- Assignable CV modulation.
- Clock and run input modes.
- Expander-ready CV/button source model.

Important edge rules to preserve:

- Utility modifiers include `GATE`, `OFF`, `START`, and `STOP` and do not expose the normal extended parameter set.
- Four triggered modifier types behave more like envelopes and expose a reduced parameter subset.
- Random shapes expose an additional slew parameter.
- `Level = 0` with `Offset > 0` can be used as a constant-voltage output.
- Loops are defined in beats rather than steps.
- Cross operations occur before quantization.
- External sync should prefer high-resolution clocking such as `24 PPQN` plus run, rather than behaving like a one-trigger-per-step sequencer.

## Rack-Native UI Model

### Layout Decision

Use an overview-plus-inspector design. The recommended starting size is `32 HP`. If the editor becomes cramped, widen the module rather than reintroducing hidden pages.

The module should have three primary zones:

- Top global transport/clock bar.
- Eight-row output overview.
- Selected-output inspector.

Patch-heavy I/O should live on panel edges so cables do not cover the editor. The center of the module should be reserved for state visibility and editing.

### UI Density Rules

At `32 HP`:

- Overview rows must remain readable at 100% Rack zoom.
- Inspector tabs may use compact controls, but not hidden nested pages.
- If a tab requires vertical scrolling for primary parameters, widen the module before hiding those parameters.
- Numeric fields may be abbreviated in the overview but must be exact in the inspector.
- Contextual help should be terse and local, not a substitute for visible controls.

### Global Bar

Expose these directly on the panel:

- Run/Stop.
- BPM.
- Sync source/status.
- PPQN.
- `CLK` input mode.
- `RUN` input mode.
- Active bank.
- Load bank.
- Save bank.
- Reset all.

These are operating controls, not secondary settings. They should not require context-menu access.

### Output Overview

Each output row should show:

- Output number.
- Mute state.
- Modifier/division/multiplication.
- Waveform icon.
- Tiny live preview.
- Badges for probability, Euclidean, loop, quantizer, cross, and CV assignment.

The overview's job is to prevent hidden state. A user should be able to glance at all eight outputs and identify which channels are doing musically significant extra work.

### All-Channel Timeline

Provide a dedicated timeline view that shows what all eight outputs are doing now, what they recently did, and what they are expected to do over the next configurable number of cycles.

This is not a generic oscilloscope. It is a Pam-specific performance map tuned for clocks, gates, modulation shapes, Euclidean patterns, probability, loops, phase offsets, cross operations, and quantized CV.

Primary goals:

- Show all eight outputs in one synchronized time view.
- Put a clear `now` cursor at the current transport position.
- Prioritize future events over deep history.
- Make upcoming triggers, gates, envelopes, modulation peaks, loop boundaries, mutes, probability skips, and quantized voltage steps visible before they happen.
- Keep the selected output visually emphasized without hiding the other seven channels.

Recommended default horizon:

- History: `0.25-1` cycle behind `now`.
- Future: `2-8` cycles ahead of `now`.
- User control: compact horizon selector such as `1x`, `2x`, `4x`, `8x`.

Timeline row content:

- Recent output trace, drawn faintly behind `now`.
- Predicted future trace or event rail, drawn brighter ahead of `now`.
- Trigger/gate onset markers.
- Gate length or pulse width blocks.
- Loop boundary markers.
- Probability markers, with uncertain future events visually distinct from deterministic events.
- Euclidean hit/rest visualization where active.
- Quantized CV step labels or pitch/scale badges where useful.
- Cross-source indicator when an output depends on another output/source.
- Mute overlay that shows output is forced to `0 V` while internal timing continues.

Interaction:

- Clicking a row selects that output in the inspector.
- Hovering an event shows source details: channel, time offset, modifier, probability, Euclidean index, loop position, voltage estimate, and CV/cross dependencies.
- Dragging the horizon or zoom control changes timeline scale without changing engine state.
- A freeze/hold button may pause the visual timeline for inspection without pausing audio.

Prediction rules:

- Deterministic future events should be shown as committed predictions.
- Probabilistic future events should be shown as possible events until their musical decision point has passed.
- If a future value depends on live external CV, external clock instability, or expander input, mark that region as input-dependent rather than pretending the prediction is exact.
- The predictive timeline must be generated from a copied/snapshot state and must not advance the audio engine RNG, transport phase, or live event counters.

### Selected-Output Inspector

Use fixed tabs instead of nested menus:

- `Timing`
- `Shape`
- `Pattern`
- `Cross`
- `CV`
- `Quant`
- `Store`

All primary per-output tasks must follow this path: select output, select tab, edit visible control.

### Interaction Contract

- No long-presses anywhere in the primary workflow.
- No primary editing task should require a context-menu trip.
- Every editable numeric value must support drag, step/wheel adjustment, and exact entry.
- Custom widgets should follow Rack conventions: vertical drag, fine adjustment, double-click reset, and right-click exact-value editing where practical.
- Conditional parameters should remain visible but disabled, with a short reason such as `Available when Loop > 0` or `Available for random shapes only`.
- The selected output should always show a live preview/scope without leaving the current editing context.
- Edits represented as Rack parameters should use Rack's normal undo/redo behavior.
- Large custom state edits should either map to Rack history actions or be documented as serialized state changes without undo.
- Mute should force the output voltage to `0 V` without stopping internal phase or event generation, so unmuting resumes coherently.
- Provide a panic/reset-transport action that clears stuck run/sync state without destroying patch or bank data.

## Tab Specification

### Timing

Expose modifier selection, phase, and flex timing in one place.

Modifier selection should be grouped visually:

- Dividers.
- Multipliers.
- Utility.
- Triggered.

If a triggered modifier is selected, switch to the triggered-output subset and show assigned input plus beat length prominently. If `RUN mode = Rotate`, show the active rotation range in the global bar and make any temporary edit restrictions obvious in the selected-output view.

### Shape

Expose signal-shape controls:

- Shape.
- Width.
- Slew.
- Level.
- Offset.
- Invert.

Labels should adapt to selected waveform behavior. Show a helper note when `Level = 0` and `Offset > 0` creates a constant-voltage output. Show a warning if phase/width combinations enter a known double-trigger risk zone for pulse-like shapes.

### Pattern

Combine rhythm/probability controls:

- Probability.
- Euclidean steps.
- Euclidean triggers.
- Euclidean pad.
- Euclidean shift.
- Loop beats.
- Loop nap.
- Loop wake.
- Loop shift.

Euclidean editing should be visual and direct, with generated-grid preview plus exact numeric fields. Loop controls must be labeled as beat behavior, not step behavior. The preview should show loop boundaries because loops reset random/flex/Euclidean structures into repeatable musical cycles.

### Cross

Show three previews:

- Destination output before cross processing.
- Selected cross source.
- Result after cross processing.

Use a segmented list or searchable dropdown for operation selection. Use a source-picker matrix for `Out1..8`, input/CV sources, and future expander sources. Gray out expander sources when no expander is attached. Add a short operation explainer for each cross mode.

### CV

Replace the hardware assignment workflow with a modulation matrix.

Each assignable destination parameter gets one row:

- Source.
- Attenuation.
- Offset.
- Live applied-CV meter.

Permit one source per destination parameter. Permit many destination parameters to reference the same source. Gray out `CLK` and `RUN` as modulation sources unless their input modes are set to `CV`. Expand the matrix when AXON-style inputs are attached.

### Quant

Expose:

- Built-in scales.
- Three user scale slots.
- 12-note toggle grid for editing user scales.

Decision: selected scale per output is patch-local. User scale slot contents should be plugin-global only if the goal is to mimic device-wide persistent memory. If that choice is made, the UI must clearly label the persistence boundary so users understand that editing a user scale affects more than the current patch.

### Store

Unify storage actions:

- Save output to bank slot.
- Load output from bank slot.
- Reset output.
- Reset seed.
- Copy from other output.
- Paste to other output.

Global bank actions stay in the top bar. Output-level actions stay in the selected-output `Store` tab.

## Architecture

### Engine/Shell Separation

Keep timing and signal generation separate from UI. The UI should edit a data model; the engine should consume a validated runtime state.

Assume a clean-room engine unless compatible engine access is explicitly proven. If a PPW-compatible engine is legally available, preserve it and build a new shell around it. This is safer than rewriting timing/DSP behavior from manual prose. If no engine is available, implement the engine behind a narrow interface and keep parity tests close to it.

### State Ownership Rules

`LiveState` is the currently sounding state. `BankState` is saved material that can be loaded into `LiveState`. A bank load copies `BankState` into `LiveState`. A bank save copies `LiveState` into `BankState`.

Ownership rules:

- Changing `LiveState` does not mutate `BankState` until the user explicitly saves.
- Loading a bank replaces the live output state and any bank-owned global timing state.
- Saving a bank captures the live output state and any bank-owned global timing state.
- Patch serialization stores both `LiveState` and `BankState`.
- Top-level transport state such as `running` is patch-local but should not be saved into every bank unless hardware parity requires it.
- BPM ownership must be explicit: either bank-owned, live-only, or both with defined copy rules.

### Data-Driven UI

Do not hardcode page logic into widget code. Use a parameter registry that describes:

- Parameter id.
- Label.
- Range.
- Unit.
- Default.
- Exact-entry formatting.
- CV assignability.
- Visibility predicate.
- Disabled reason.
- Affected waveform/modifier families.

This avoids scattering conditions such as utility-modifier suppression, triggered-output subsets, random-shape slew, and expander-dependent CV sources through the UI.

Use a source registry alongside the parameter registry. CV assignment, cross-source selection, expander availability, and disabled-source UI must all consume the same source descriptors.

```cpp
enum class SourceKind {
    Output,
    CvInput,
    ClockInput,
    RunInput,
    ExpanderCv,
    ExpanderButton,
};

struct SourceDescriptor {
    int id = -1;
    std::string label;
    SourceKind kind = SourceKind::CvInput;
    bool audioRate = false;
    bool available = true;
    std::string unavailableReason;
    float nominalMinV = 0.0f;
    float nominalMaxV = 5.0f;
};
```

### Illustrative State Shape

This is not a final API. It is a guide to the state boundaries the implementation should preserve.

```cpp
enum class ClockInputMode { Clock, Cv, NextBank };
enum class RunInputMode { RunGate, Reset, Cv, PrevBank, Rotate };
enum class TabId { Timing, Shape, Pattern, Cross, Cv, Quant, Store };

struct CvAssignment {
    bool enabled = false;
    int sourceId = -1;
    float attenuation = 1.0f;
    float offset = 0.0f;
};

struct EuclidState {
    int steps = 0;
    int triggers = 0;
    int pad = 0;
    int shift = 0;
};

struct LoopState {
    int beats = 0;
    int nap = 0;
    int wake = 0;
    int shift = 0;
};

struct CrossState {
    int op = 0;
    int source = 0;
};

struct FlexState {
    int op = 0;
    float amountPct = 0.0f;
};

struct QuantState {
    int scaleId = 0;
    uint16_t userMask = 0;
};

struct OutputState {
    bool muted = false;
    int modifierId = 0;
    int shapeId = 0;
    float widthPct = 50.0f;
    float slewPct = 0.0f;
    float levelPct = 100.0f;
    float offsetPct = 0.0f;
    float phasePct = 0.0f;
    float probabilityPct = 100.0f;
    bool invert = false;
    EuclidState euclid;
    LoopState loop;
    CrossState cross;
    FlexState flex;
    QuantState quant;
    std::array<CvAssignment, kAssignableParamCount> cv;
    uint32_t randomSeed = 0;
};

struct BankState {
    float bpm = 120.0f;
    std::array<OutputState, 8> outputs;
};

struct LiveState {
    float bpm = 120.0f;
    bool running = false;
    ClockInputMode clkMode = ClockInputMode::Clock;
    RunInputMode runMode = RunInputMode::RunGate;
    int extPpqn = 24;
    int activeBank = 0;
    std::array<OutputState, 8> outputs;
};

struct UiState {
    int selectedOutput = 0;
    TabId selectedTab = TabId::Timing;
};

struct ModuleState {
    static constexpr int kSchemaVersion = 1;

    LiveState live;
    std::array<BankState, 64> banks;
    UiState ui;
};
```

## Timing Event Ordering

Within each `process()` frame, evaluate engine work in this order unless later parity tests prove a different order is required:

- Read raw CV, clock, run, reset, and expander sources.
- Detect rising/falling edges with Schmitt triggers.
- Apply run, reset, rotate, and bank-change events.
- Resolve external clock phase and tempo updates.
- Advance internal transport phase.
- Generate per-output phase positions.
- Apply per-output timing modifiers: divisor/multiplier, phase, flex, and loop.
- Generate base shape, gate, or envelope.
- Apply probability and Euclidean decisions at deterministic event boundaries only.
- Apply cross operations.
- Apply quantization.
- Apply level, offset, invert, mute, and output clamp.
- Write outputs, lights, meters, and history buffers.

Level, offset, invert, mute, and final clamp are post-quantization output-shaping operations unless direct parity evidence requires different ordering.

## Randomness Contract

Random behavior must be deterministic under identical patch state, BPM, transport position, reset events, and seed.

Randomness rules:

- Random, probability, and flex decisions consume random values only at documented musical decision points.
- Random values must not be consumed every audio sample.
- Seed reset must restore repeatable musical output for the same transport/reset conditions.
- UI previews must use separate preview RNG state or cached engine event data.
- UI redraws, tab changes, hover states, and scope refreshes must never advance the audio engine RNG.

## Preview Contract

Previews are diagnostic approximations unless explicitly marked as engine-derived.

Preview types:

- Live scope: sampled from actual output history.
- Predictive preview: generated from current state and transport assumptions.
- All-channel timeline: synchronized view of all eight output histories and lookahead predictions.
- Cross preview: may use reduced-resolution buffers but must preserve operation ordering.
- CV meters: derived from current source values after attenuation and offset.

Preview generation must not mutate engine state, consume engine RNG, or change transport phase.

All-channel timeline predictions should be explicitly labeled by confidence:

- `Committed`: deterministic events already implied by current transport and state.
- `Possible`: probabilistic/random events whose decision point has not arrived.
- `Input-dependent`: predictions that depend on external CV, external clock timing, run/rotate input, or expander state that cannot be known ahead of time.

The timeline may use lower-resolution rendering than the audio engine, but event positions must remain musically aligned to the same timing model used by output generation.

## Serialization Contract

Use Rack `dataToJson()` and `dataFromJson()` for module state that is not already represented by Rack params. This includes banks, output configs, random seeds, CV assignments, selected output, selected tab, and UI-specific patch-local state.

Serialization requirements:

- Include a schema version.
- Clamp all loaded enum and numeric values.
- Provide defaults for missing fields.
- Ignore unknown future fields.
- Keep migration functions small and explicit.
- Add tests for old/missing/partial JSON payloads once schema version `2` exists.

Patch storage should be reserved for optional large or file-backed data such as hardware-backup import/export. Do not use patch storage for normal Pam-style state unless JSON size becomes demonstrably problematic.

Plugin-global settings are appropriate for non-musical preferences such as default theme, advanced tooltip preference, and debug display defaults. User scale slots are musical state and should remain patch-local unless a later compatibility layer deliberately models hardware-style global memory with clear UI warnings.

## Rack Implementation Notes

Follow this repository's existing module structure rather than generic Rack SDK scaffolding. Add new files in the existing `src/`, `res/`, and `tests/` patterns used by this repo.

Recommended file grouping if this becomes a new module:

```text
src/
  PamsReimagined.cpp
  PamsReimagined.hpp
  PamsReimaginedWidget.cpp
  PamsReimaginedWidget.hpp
  PamsEngine.hpp
  PamsEngine.cpp
  PamsState.hpp
  PamsParamRegistry.hpp
  PamsSerialization.hpp
  PamsQuantizer.hpp
  PamsClock.hpp
  PamsUiWidgets.hpp

tests/
  pams_serialization_spec.cpp
  pams_clock_spec.cpp
  pams_bank_spec.cpp
  pams_output_seed_spec.cpp

res/
  pams_reimagined.svg
  pams_reimagined-dark.svg
```

Display implementation guidance:

- Use custom widgets for the editor surface.
- Cache static regions with a framebuffer where redraw cost is non-trivial.
- Redraw live scope, clock cursor, meters, and assignment activity separately from static chrome.
- Render the all-channel timeline as its own dirty-flagged widget so lookahead recomputation is decoupled from paint frequency.
- Provide dark-panel assets from the start.
- Keep text readable at 100% zoom on a normal display.

Performance budgets:

- Audio `process()` must avoid heap allocation.
- UI preview generation must not run on every audio frame.
- Live meters and scopes should update at a capped UI rate, typically `30-60 Hz`.
- Static panel/editor chrome should be framebuffer-cached.
- Expensive preview recomputation should be dirty-flagged by state changes.
- Timeline lookahead should recompute only when transport position crosses a meaningful display quantum or when relevant state changes.
- Timeline lookahead should have a bounded maximum horizon and bounded event count per channel.
- Expander message handling must not allocate on the audio thread.

Timing implementation guidance:

- Use Schmitt-trigger edge detection for clock/run/reset-like inputs.
- Use pulse generators for generated triggers.
- Avoid sequencing races around reset by briefly ignoring or ordering clock edges after reset-like events according to Rack sequencing best practice.
- Surface sync lock/state clearly in the top bar.

Golden fixture candidates:

```text
tests/fixtures/pams/
  clock_24ppqn_run_reset.json
  euclid_loop_repeatability.json
  probability_seed_repeatability.json
  cross_before_quant.json
  level_zero_offset_constant_voltage.json
  triggered_modifier_reduced_params.json
```

Each fixture should define:

- `name`
- `sampleRate`
- `durationBeats`
- `initialState`
- `inputEvents`
- `expectedEvents`
- `expectedVoltageRanges`

Example:

```json
{
  "name": "cross_before_quant",
  "sampleRate": 48000,
  "durationBeats": 8,
  "initialState": {},
  "inputEvents": [],
  "expectedEvents": [],
  "expectedVoltageRanges": []
}
```

## AXON-Style Expander Contract

AXON-2-style support should be implemented as a separate adjacent expander, not as hidden ports on the main module.

Expected expander features:

- Four extra CV inputs.
- Two assignable buttons.
- Source availability reflected in the main module's CV matrix and cross-source picker.
- Clean detached behavior: sources disappear or become disabled with clear UI reasons.

Use Rack's expander messaging model. If timing consistency matters between expander inputs and main-module processing, use a double-buffered message pattern and document which process frame owns each value.

## Context Menu

Keep the context menu narrow. It should contain secondary settings only:

- CV input range: `5 V` / `10 V`.
- Default theme.
- Show advanced tooltips.
- Import hardware backup, when implemented.
- Export hardware backup, when implemented.
- Legacy keyboard shortcuts, if intentionally supported.
- Debug overlay.

Do not place primary musical editing in the context menu.

## Phased Delivery

### Phase 1: Main Module Shell And Core Behavior

- Main module UI shell.
- Internal clock.
- External clock/run sync.
- Eight outputs.
- Per-output state model.
- Overview rows.
- Selected-output inspector.
- All-channel timeline with recent-history and future-lookahead modes.
- Timing, Shape, Pattern, Cross, CV, Quant, and Store tabs.
- Minimal Cross implementation with source selection, operation selection, and tested pre-quantization ordering.
- Serialization with schema version.
- Bank/output save/load/reset.
- Seed reset.
- Baseline tests.

### Phase 2: Cross And Visual Polish

- Cross tab visual polish.
- Result preview for cross processing.
- More complete live preview/scope behavior.
- Timeline hover details, confidence labeling, and freeze/hold inspection.
- Framebuffer/static redraw optimization.
- Dark panel.
- Tooltip and disabled-state polish.

### Phase 3: AXON-Style Expander

- Companion expander module.
- Four extra CV inputs.
- Two buttons.
- Main-module source matrix integration.
- Expander attach/detach tests.

### Phase 4: Compatibility Utilities

- Hardware-backup import/export if format access is available.
- Optional legacy keyboard shortcuts.
- Additional golden parity tests against official behavior.

## Acceptance Criteria

V1 is acceptable when:

- No primary function requires long-hold.
- No primary per-output function requires context-menu access.
- All primary per-output tasks are reachable via select output, select tab, edit visible control.
- Overview rows expose hidden musically significant state for all eight outputs.
- All-channel timeline shows all eight outputs against a shared `now` cursor.
- Timeline shows at least brief recent history and a configurable future horizon.
- Timeline predictions distinguish deterministic, probabilistic, and input-dependent future events.
- Selected-output editor always includes a live preview/scope.
- Output voltage behavior defaults to documented PPW `0-5 V` semantics.
- CV input range can be configured where relevant.
- Patch save/load restores all module state needed for musical behavior.
- Musical state is patch-local by default.
- Live state and bank state follow the documented copy-on-load/copy-on-save ownership rules.
- Minimal Cross processing is implemented in V1 and tested before quantization.
- Bank save/load, output save/load, reset, and seed reset are covered by automated tests.
- External clock plus run sync at `24 PPQN` is covered by automated tests.
- Reset/rotate behavior is covered by automated tests if rotate is implemented in V1.
- Random/probability/flex behavior is deterministic for identical seed, patch state, and reset/transport conditions.
- UI previews do not mutate engine state, transport phase, or engine RNG.
- Timeline lookahead does not allocate or run unbounded simulation from the audio thread.
- JSON loading is robust to missing fields and out-of-range enum values.
- Static display regions are cached or otherwise proven cheap enough not to matter.

Deferred acceptance criteria:

- AXON-style source expansion works only after Phase 3.
- Hardware-backup import/export works only after Phase 4.
- PPEXP1/PPEXP2-style modules are not required for this project unless scope is reopened.

## Strategic Summary

Do not build a bigger hardware panel. Build a Rack instrument that exposes PPW's timing engine as a readable, direct editor. The design succeeds if the user can understand all eight channels at a glance, focus one channel without losing context, edit exact values without ritual navigation, and trust that the saved patch restores the full musical state.
