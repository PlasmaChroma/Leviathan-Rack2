# Moirai Implementation Plan

This document is the canonical, ready-to-execute implementation plan for
`Moirai`. It refines `doc/moirai.md` against the current Leviathan source tree.
The earlier document remains the product and architectural rationale; this file
resolves its open choices into v1 contracts, file ownership, build order, and
acceptance criteria.

Moirai is currently unreleased. Its initial enum and state formats may be chosen
cleanly, but they become compatibility contracts as soon as a public build is
made.

## 1. Product Contract

Moirai is a 12 HP, dual-lane, 16-channel polyphonic envelope bank.

- One polyphonic `GATE` input determines the active channel count.
- Lanes A and B independently evaluate one envelope per gate channel.
- `A` and `B` are polyphonic envelope outputs.
- `EOC A` and `EOC B` are polyphonic 10 V trigger outputs.
- `VEL`, `M1`, `M2`, and `M3` provide broadcast or per-channel modulation.
- `CLOCK` supports beat-relative durations; `RESET` returns all voices to idle.
- Each lane/channel assignment refers to a shared named program.
- The bank is stored in the patch and runs without Sibyl, Octavia, or a network
  connection.
- Sibyl is a natural signal source, but the only musical connection between the
  modules is ordinary Rack cables.
- Semantic edits are JSON-based, revision-guarded, atomic, validated, undoable,
  and compiled away from the audio thread.

The v1 implementation includes the compact contour/status display. A separate
expanded graphical editor is deferred.

## 2. Canonical v1 Scope

### Implement in v1

- Two lanes and 1–16 channels per lane.
- Up to 32 named programs and 8,192 contour points across the bank.
- Staged `gate`, staged `oneShot`, contour `oneShot`, and contour `cycle`
  programs.
- Millisecond and beat durations. Seconds are accepted by JSON and converted to
  milliseconds during parsing.
- Gate-low release branching and one sustain hold in staged `gate` programs.
- Linear, smoothstep, smootherstep/sigmoid, hold, step, exponential, and
  logarithmic segment curves.
- Clamped monotone-cubic or linear contour interpolation. No amplitude
  normalization.
- Restart, from-current, legato, and ignore-while-running retrigger policies.
- Forward counted loops and forward while-gate loops over staged segments.
- Per-trigger deterministic variation.
- Lane output modes `0_10`, `0_5`, and `bipolar_5`.
- Lane-configurable EOC source: program completion or loop completion.
- Per-program macro bindings for time scale, curve bias, level scale, level
  offset, and variant selection.
- Factory presets implemented through the same bank/edit representation as
  authored programs.
- `immediate`, `nextTrigger`, `allIdle`, and `nextClock` revision adoption.
- Existing voices finish with the compiled generation on which they started by
  default.
- Compact NanoVG display, fixed panel controls, context-menu preset access, and
  a manual trigger button.
- Generic Octavia semantic-control dispatch while preserving every existing
  Sibyl route and `vcv_sibyl_*` tool.
- Strongly named `vcv_moirai_*` MCP tools and an Octavia skill reference.
- Verification through Octavia's existing frame-aligned monitor/snapshot system;
  Moirai itself does not publish observation requests or bypass physical cables.

### Explicitly defer

- Expanded/freehand editor.
- Reverse and ping-pong staged loops.
- Multiple sustain regions.
- Contour gate/release branching. Use staged programs for gate envelopes.
- Tempo-aligned retrigger as a distinct policy. `nextClock` adoption is still
  included.
- Arbitrary user-authored expression graphs.
- Per-channel electrical output ranges.
- Continuous program/variant switching.
- Automatic closed-loop envelope refinement. Agent-driven capture, analysis,
  comparison, and iterative semantic edits use Octavia's existing observation
  system in v1.
- GL rendering, cached framebuffers, and Wyrm's sand/material renderer.

These exclusions are schema errors in v1 rather than silently ignored fields.
Capabilities must report the supported subset so later schema versions can add
them without ambiguity.

## 3. Fixed Hardware Contract

Use these enum orders from the first implementation:

```cpp
enum ParamId {
    TIME_PARAM,
    CURVE_PARAM,
    LEVEL_PARAM,
    LANE_PARAM,
    CHANNEL_PARAM,
    MANUAL_TRIGGER_PARAM,
    NUM_PARAMS
};

enum InputId {
    GATE_INPUT,
    VELOCITY_INPUT,
    M1_INPUT,
    M2_INPUT,
    M3_INPUT,
    CLOCK_INPUT,
    RESET_INPUT,
    NUM_INPUTS
};

enum OutputId {
    A_OUTPUT,
    EOC_A_OUTPUT,
    B_OUTPUT,
    EOC_B_OUTPUT,
    NUM_OUTPUTS
};

enum LightId {
    LANE_A_LIGHT,
    LANE_B_LIGHT,
    NUM_LIGHTS
};
```

Parameter definitions:

| Parameter | Range | Default | Contract |
| --- | ---: | ---: | --- |
| `TIME` | `-4..+4` | `0` | Log2 duration scale, `2^value`; 1/16x to 16x. |
| `CURVE` | `-1..+1` | `0` | Non-destructive curve/skew bias. |
| `LEVEL` | `0..1` | `1` | Final lane depth before electrical range mapping. |
| `LANE` | button | A | Toggles inspected lane; does not alter assignment. |
| `CHANNEL` | `0..15`, snapped | `0` | Selects inspected/manual-trigger channel. |
| `MANUAL TRIGGER` | button | off | Triggers the selected channel on both lanes. |

The factory preset chooser belongs in the module context menu in v1. Do not
make encoder press/double-click behavior a hidden requirement. Applying a
preset targets the selected lane and channel, creates/upserts an ordinary
program, assigns it, increments the bank revision, and produces one Rack undo
entry when initiated from the UI.

Port voltage rules:

- Gate thresholds use `dsp::SchmittTrigger` behavior suitable for standard Rack
  gates.
- `VELOCITY_INPUT` is interpreted as `0..10 V`; its disconnected neutral is
  `10 V` so an unpatched velocity input does not attenuate the envelope.
- `M1..M3` are interpreted as `-10..+10 V`; disconnected and missing-channel
  neutral is `0 V`.
- A monophonic modulation input broadcasts channel 0 to every gate channel.
- A polyphonic modulation input addresses matching channels. Channels beyond
  its cable channel count use the source's neutral value; they do not repeat
  the last channel.
- With `GATE` connected, output channel count is
  `clamp(GATE.channels, 1, 16)`. With it unpatched, the base count is one and a
  manual trigger temporarily raises it to at least `selectedChannel + 1` until
  those voices become idle.
- `EOC` pulses are 10 V for 1 ms and otherwise 0 V.
- `RESET` rising edge immediately idles every voice, clears EOC pulses and gate
  history, resets the clock estimator, and emits no EOC.

## 4. Panel and Assets

The editable master is `res/Moirai.svg`. Generate, but never directly edit:

- `res/Moirai.panel.svg`
- `res/Moirai.labels.svg`

The master SVG must contain hidden component anchors for all six parameters,
seven inputs, four outputs, two lane lights, and the display rectangle. Use
`PanelSvgUtils` lookups in `MoiraiWidget.cpp`; do not duplicate positions as
hardcoded pixels.

After changing the master:

```sh
python3 tools/split_svg_labels.py res/Moirai.svg --overwrite
make generate-panel-anchor-atlas
```

Suggested physical hierarchy:

```text
title / AI mark
display: lane, channel, program, revision, contour, playhead, activity
LANE    CHANNEL    MANUAL
TIME    CURVE      LEVEL
GATE VEL M1 M2 M3 CLOCK RESET
A EOC-A             B EOC-B
```

The display is a lightweight `TransparentWidget` using NanoVG only. It reads an
immutable UI snapshot, never mutable voice state. It shows both lane contours,
with the selected lane brighter, and publishes at display rate rather than
performing work per audio sample. No persistent NanoVG image is needed; if one
is introduced later it must use `NvgGraphicsLifecycle.hpp`.

## 5. Source Layout

Create these files:

- `src/MoiraiTypes.hpp`: authored and compiled value types, limits, enums.
- `src/MoiraiCurves.hpp`: allocation-free curve and interpolation helpers.
- `src/MoiraiCompiler.hpp/.cpp`: validation plus authored-to-compiled bank
  conversion.
- `src/MoiraiJSON.hpp/.cpp`: schema v1 parsing and all read serializers.
- `src/MoiraiEdit.hpp/.cpp`: ordered atomic semantic edit operations.
- `src/MoiraiPresets.hpp/.cpp`: factory program constructors.
- `src/MoiraiEngine.hpp/.cpp`: voice state machine, modulation resolution,
  clock handling, output evaluation, and generation adoption.
- `src/Moirai.hpp`: Rack module declaration, stable enums, publication state,
  and JSON/control entry points.
- `src/Moirai.cpp`: module configuration, process path, persistence, semantic
  request handling.
- `src/MoiraiWidget.cpp`: panel, controls, display, context menu, and
  `modelMoirai`.
- `src/OctaviaSemanticControl.hpp`: reusable in-process semantic adapter.
- `tests/moirai_curves_spec.cpp`
- `tests/moirai_compiler_spec.cpp`
- `tests/moirai_edit_spec.cpp`
- `tests/moirai_engine_spec.cpp`
- `tests/moirai_module_spec.cpp`
- `tests/octavia_semantic_contract_spec.py`
- `MCP/skill/octavia/references/moirai.md`

Registration changes:

- Declare `modelMoirai` in `src/plugin.hpp`.
- Add it in `src/plugin.cpp` after `modelSibyl` or beside the AI-first modules.
- Add a `plugin.json` entry with slug/name `Moirai`, a concise description, tags
  `Envelope generator`, `Function generator`, and `Polyphonic`, and
  `"hidden": true` until the module is release-ready.
- Add Moirai binaries to the current `TEST_BINS_NON_RACK`/`TEST_BINS_RACK`
  topology according to whether they belong in the routine fast set or the
  Rack-only set; choose Rack linkage and `run_test_bin` versus
  `run_rack_test_bin` independently, as the existing Sibyl recipes do. Add
  Python contract tests directly to `test-fast` beside the existing
  Octavia/Sibyl and monitoring-panel contracts.

## 6. Authored Bank Model

The authoritative patch document has `schemaVersion: 1` and this top-level
shape:

```json
{
  "schemaVersion": 1,
  "revision": 7,
  "seed": 12345,
  "clock": {
    "externalPpqn": 4,
    "fallbackBpm": 120.0,
    "lossTimeoutMs": 2000.0,
    "onClockLoss": "holdTempo"
  },
  "lanes": {
    "A": {
      "defaultProgram": "factory_adsr",
      "outputMode": "0_10",
      "eocSource": "program",
      "assignments": {"0": "bass_amp"},
      "channelLabels": {"0": "bass"}
    },
    "B": {
      "defaultProgram": "factory_adsr",
      "outputMode": "0_10",
      "eocSource": "program",
      "assignments": {},
      "channelLabels": {}
    }
  },
  "programs": {
    "bass_amp": {}
  }
}
```

IDs are object keys and are also stored in the compiled program. IDs must match
`[A-Za-z0-9_-]{1,64}`. Display names and channel labels are UTF-8 strings of at
most 64 bytes. Unknown fields are rejected in schema v1 so typos cannot appear
to succeed.

Hard validation limits:

| Item | Limit |
| --- | ---: |
| Programs | 32 |
| Contour points per program | 2–256 |
| Total contour points | 8,192 |
| Gate-path segments | 32 |
| Release-path segments | 32 |
| Macro bindings per program | 16 |
| Variants in one selection binding | 32 |
| Absolute stage duration | `0.01..600000 ms` |
| Beat stage duration | `1/4096..1024 beats` |
| Loop count | `1..1024` |

The full compact serialization must remain under Octavia's 1 MiB request/state
safety limit. The parser checks the byte limit before parsing as well as the
structural limits above.

### 6.1 Staged program

```json
{
  "name": "Bass Amp",
  "kind": "staged",
  "mode": "gate",
  "gatePath": [
    {"id": "attack", "to": 1.0, "duration": {"ms": 6.0},
     "curve": {"type": "exponential", "amount": 0.7}},
    {"id": "decay", "to": 0.62, "duration": {"ms": 95.0},
     "curve": {"type": "exponential", "amount": -0.2}}
  ],
  "sustain": {"mode": "hold"},
  "releasePath": [
    {"id": "release", "to": 0.0, "duration": {"ms": 180.0},
     "curve": {"type": "exponential", "amount": 0.35}}
  ],
  "retrigger": "fromCurrent",
  "variation": {"level": 0.0, "time": 0.0},
  "macroBindings": []
}
```

Rules:

- Stage `to` values are normalized authored levels in `0..1`.
- A rising gate starts `gatePath`; a falling gate immediately starts
  `releasePath` from the current value, including during attack or decay.
- A `gate` program requires a nonempty gate path and release path. It holds the
  final gate-path value while high after the path completes.
- A staged `oneShot` concatenates `gatePath` and `releasePath`, ignores gate-low
  after triggering, and forbids `sustain.mode=hold`.
- A stage may contain `loopStart: true` or `loopEnd` with either
  `{"count": N}` or `{"whileGate": true}`. There may be at most one loop,
  markers must be in `gatePath`, and nesting is rejected.
- A while-gate loop exits at its loop end when gate goes low, then enters the
  release path from the current value.

### 6.2 Contour program

```json
{
  "name": "Glass Double Bloom",
  "kind": "contour",
  "mode": "oneShot",
  "duration": {"beats": 0.75},
  "points": [
    {"t": 0.0, "v": 0.0},
    {"t": 0.04, "v": 0.94},
    {"t": 0.16, "v": 0.31},
    {"t": 1.0, "v": 0.0}
  ],
  "interpolation": "monotoneCubic",
  "retrigger": "restart",
  "variation": {"level": 0.0, "time": 0.0},
  "macroBindings": []
}
```

Rules:

- Point `t` is finite, strictly increasing, and covers exactly `0..1`.
- Point `v` is finite and in `0..1`.
- `oneShot` clamps at the final point and becomes idle.
- `cycle` wraps only after reaching `t=1`; its interpolation is periodic only
  across the final-to-first boundary. Ordinary contours never wrap during
  interpolation.
- Monotone tangents are computed during compilation and every interpolated
  sample is clamped to the range of its two authored endpoints. This preserves
  Wyrm's useful overshoot protection without its oscillator normalization.

### 6.3 Duration object

Exactly one key is allowed:

```json
{"ms": 80.0}
{"seconds": 0.08}
{"beats": 0.25}
```

Milliseconds and seconds compile to sample-rate-independent seconds. Beats
compile as beats and use the current estimated tempo. A stage may mix absolute
and beat units with other stages. TIME and trigger-sampled time modulation are
latched when a voice starts; a beat-relative running stage still follows
subsequent clock tempo changes.

### 6.4 Curves

The audio path must not call `std::pow`, `std::exp`, `std::sin`, or `std::sqrt`.
Use multiply-only polynomial shaping:

```cpp
float fourth(float x) { float x2 = x * x; return x2 * x2; }

// amount 0 is linear; positive is slow-start/fast-end; negative is its mirror.
float powerBias(float u, float amount) {
    if (amount >= 0.f) return lerp(u, fourth(u), amount);
    float x = 1.f - u;
    return 1.f - lerp(x, fourth(x), -amount);
}
```

- `linear`: `u`.
- `smoothstep`: `u*u*(3-2*u)`.
- `sigmoid`/`smootherstep`: `u^3*(u*(6*u-15)+10)`.
- `exponential`: `powerBias(u, clamp(amount, -1, 1))`; negative amounts are the
  mirrored/fast-start form used by the authored examples.
- `logarithmic`: `powerBias(u, -clamp(abs(amount), 0, 1))`.
- `hold`: retain the segment start until completion.
- `step`: use the target immediately at segment entry.

The panel `CURVE` value and continuous `curveBias` macro add to the authored
amount and clamp to `-1..1`; fixed smoothstep/sigmoid/hold/step types ignore
that bias. Curve tests must cover endpoints, monotonicity, finiteness, and
bounds.

## 7. Macro and Variation Contract

A binding has this normalized shape:

```json
{
  "source": "m1",
  "target": "timeScale",
  "inputRange": [-10.0, 10.0],
  "outputRange": [0.25, 4.0],
  "mapping": "exponential",
  "sampling": "onTrigger",
  "smoothingMs": 5.0
}
```

Allowed sources are `velocity`, `m1`, `m2`, and `m3`.

| Target | Sampling | Result |
| --- | --- | --- |
| `timeScale` | `onTrigger` only | Multiplies all durations. |
| `variantSelect` | `onTrigger` only | Selects one listed program ID by voltage bin. |
| `curveBias` | `onTrigger` or `continuous` | Adds to eligible curve amounts. |
| `levelScale` | `onTrigger` or `continuous` | Multiplies normalized output. |
| `levelOffset` | `onTrigger` or `continuous` | Adds normalized offset before clamp. |

Continuous bindings are one-pole smoothed. `smoothingMs` is required and
clamped to `1..1000 ms`; its coefficient is recalculated only when sample rate
or the binding changes. Structural targets with `continuous` sampling are
validation errors.

If no explicit velocity binding exists, the compiler adds the factory default
mapping `velocity 0..10 V -> levelScale 0..1`. A program can opt out with
`"velocityDefault": false`.

Variant selection is resolved once at trigger time before starting a voice.
Variant targets must exist, cannot point to another program containing a
variant binding, and cannot form a cycle. The selected program's other bindings
then resolve normally.

Variation fields are normalized maximum deviations in `0..1`. On trigger, use
a stable integer hash of bank seed, revision-independent program ID hash, lane,
channel, and per-channel trigger count. Convert it to deterministic bipolar
values for level and time. Never use a global RNG or allocate on trigger.

## 8. Runtime Compilation and Voice Engine

`compileBank()` performs all string lookup, graph validation, tangent
calculation, binding normalization, loop resolution, and summary calculation.
The resulting `CompiledBank` is immutable and contains fixed/index-based data
used directly by the audio thread.

Recommended runtime structures:

```cpp
struct EnvelopeVoice {
    const CompiledBank* bank = nullptr;
    const CompiledProgram* program = nullptr;
    uint32_t generation = 0;
    uint32_t triggerCount = 0;
    int segment = 0;
    int loopIteration = 0;
    float segmentPhase = 0.f;
    float segmentStart = 0.f;
    float value = 0.f;
    float latchedTimeScale = 1.f;
    float latchedCurveBias = 0.f;
    float latchedLevelScale = 1.f;
    float latchedLevelOffset = 0.f;
    bool gateHigh = false;
    bool running = false;
    bool releasing = false;
};
```

There are exactly 32 voices in `std::array<EnvelopeVoice, 32>`, indexed as
`lane * 16 + channel`. The module also owns 16 gate Schmitt triggers and two
arrays of 16 `dsp::PulseGenerator`s for EOC.

Per-sample order is fixed:

1. Resolve channel count and input voltages. Cache `TIME`'s scale when its
   parameter value changes, using Rack's fast exp2 helper rather than a standard
   transcendental in the voice loop.
2. Process reset; if it rises, clear all runtime state and skip triggering for
   that sample.
3. Process external clock and determine whether a `nextClock` boundary occurred.
4. Evaluate pending generation adoption before gate edges.
5. Detect gate edges once per channel.
6. On each rising edge, adopt a `nextTrigger` generation once globally before
   starting any voices from that sample; then apply each lane's retrigger rule.
7. On falling edge, branch staged gate voices into release.
8. Advance active voices, including multiple zero/very-short segment crossings
   with a bounded transition loop.
9. Apply continuous smoothed bindings, panel LEVEL, and lane voltage mapping.
10. Write envelope and EOC outputs, then publish telemetry at the divided UI
    rate.

The bounded transition loop is capped at 64 transitions per voice per sample.
Compilation rejects zero durations, so reaching the cap means corrupt runtime
state; force the voice idle and record a rate-limited debug diagnostic gated by
`isDragonKingDebugEnabled()`.

Retrigger behavior:

- `restart`: begin at authored level 0.
- `fromCurrent`: begin the first segment from current output value.
- `legato`: for a running staged gate voice, a new rise leaves its current stage
  and value untouched; from idle it starts normally. This mainly distinguishes
  manual/reconstructed retriggers from an actual new phrase.
- `ignoreWhileRunning`: ignore a rise for every program kind until the voice is
  idle; unlike legato it also ignores one-shot and cycle retriggers.

Output mapping occurs after normalized value is clamped to `0..1`:

- `0_10`: `10 * value`.
- `0_5`: `5 * value`.
- `bipolar_5`: `10 * value - 5`.

Panel LEVEL multiplies normalized value after program macros. It never rewrites
authored state. In bipolar mode LEVEL scales around 0 V, not around -5 V.

## 9. Clock Contract

Implement a Moirai-local edge-anchored estimator initially; do not refactor the
released Sibyl clock path as part of this module. It may reuse the tested
algorithmic shape of `SibylClockEstimator` without coupling the types.

- `externalPpqn`: integer `1..96`, default 4.
- `fallbackBpm`: finite `20..400`, default 120.
- `lossTimeoutMs`: finite `100..10000`, default 2000.
- `onClockLoss`: `holdTempo` or `fallback`.
- Before two valid clock edges, use fallback BPM.
- Reject implausible intervals and smooth accepted intervals without
  transcendental functions.
- `holdTempo` keeps the last estimate after timeout; `fallback` returns to the
  configured BPM.
- Beat-relative stages follow estimated BPM continuously. Clamp per-sample
  phase advance so a bad clock observation cannot skip unbounded stages.

`CLOCK` is not required for millisecond programs.

## 10. Revision Adoption and Reclamation

Maintain three public revision states:

- `acceptedRevision`: latest successfully validated edit/preset/load.
- `activeRevision`: generation used by new triggers.
- `pendingRevision`: accepted generation waiting for its boundary, or null.

Edit defaults are:

```json
{
  "apply_at": "nextTrigger",
  "active_voice_policy": "finishCurrent"
}
```

Adoption semantics:

| Boundary | Behavior |
| --- | --- |
| `immediate` | New triggers use the generation immediately. |
| `nextTrigger` | First gate rise on any channel adopts it before all rises in that sample. |
| `allIdle` | Adopt when no lane voice is running. |
| `nextClock` | Adopt on the next valid CLOCK rising edge. |

`finishCurrent` leaves running voices attached to their original compiled bank.
Also support `restartActive` for explicit immediate replacement: active voices
restart from their current values on corresponding programs in the new bank;
missing assignments use lane defaults. `restartActive` is valid only with
`apply_at: immediate`.

Compiled banks are owned on the UI/control side. Each bank has an atomic active
voice count, changed only when a voice starts using or releases that generation,
not on each sample. Retain owners for accepted, active, pending, hazard-read, or
voice-referenced generations. Reclaim only from UI/control callbacks. The audio
thread performs no allocation, deletion, lock acquisition, JSON work, string
lookup, or smart-pointer reference traffic.

Use the same acquire/publish/hazard pattern already proven in `Sibyl.cpp`, but
keep Moirai's ownership graph local and cover reclamation with a focused stress
test.

## 11. JSON Persistence

`dataToJson()` stores:

- `moiraiBank`: the complete accepted authored bank, including its revision.
- `selectedLane` and `selectedChannel`.
- Any future UI-only flags under a separate `ui` object.

Runtime phases, gate states, EOC pulse state, accepted/active pointer addresses,
and telemetry are never serialized.

`dataFromJson()`:

1. Applies size and schema checks.
2. Parses and compiles into a private candidate.
3. If valid, installs it as accepted and active immediately, clears pending
   state, resets all voices, and preserves its stored revision.
4. If invalid, installs the compiled factory bank, records a concise error for
   the display/status API, and never leaves null runtime pointers.

Undo/redo therefore uses ordinary Rack module snapshots and restores the entire
bank deterministically. Direct semantic edits return success only after the new
accepted state is installed, allowing Octavia to create exactly one undo entry.

## 12. Semantic Control Generalization

Add this local, optional RTTI adapter:

```cpp
struct OctaviaSemanticControl {
    enum class Operation {
        CAPABILITIES,
        GET_DOCUMENT,
        VALIDATE,
        EDIT,
        GET_STATUS,
        COMMAND
    };

    virtual ~OctaviaSemanticControl() = default;
    virtual const char* semanticCapabilityId() const noexcept = 0;
    virtual bool handleSemanticRequest(Operation operation,
                                       const std::string& requestJson,
                                       std::string& responseJson,
                                       std::string& error) = 0;
};
```

Capability IDs:

- Sibyl: `leviathan.sibyl.composition`
- Moirai: `leviathan.moirai.envelope-bank`

Make `SibylControl` inherit `OctaviaSemanticControl` and provide an inline
adapter from generic operations to its existing operations and
`handleSibylRequest()`. Sibyl itself should not need schema or DSP changes.
Map `GET_DOCUMENT` to `GET_COMPOSITION` and `COMMAND` to `TRANSPORT`.

Refactor Octavia's `SibylJob`, queue processor, and dispatcher into a semantic
job/queue/dispatcher. Preserve the following compatibility surfaces exactly:

- `/sibyl/{moduleId}/capabilities`
- `/sibyl/{moduleId}/composition`
- `/sibyl/{moduleId}/validate`
- `/sibyl/{moduleId}/edit`
- `/sibyl/{moduleId}/status`
- `/sibyl/{moduleId}/transport`
- all current `vcv_sibyl_*` tools and payloads

The refactor is deliberately limited to the semantic job path. It must not
alter Octavia's current observation history, snapshot pool, analysis engine,
measurement session, monitor input/light IDs, or `ObservationBus` cursor. Keep
`processObservationTriggers()` and the observation/analysis routes independent
of the semantic queue, and run the full Octavia observation regression set at
the Phase 0 gate.

Add generic routes:

```text
GET  /semantic/{moduleId}/capabilities
GET  /semantic/{moduleId}/document?view=summary&id=...
POST /semantic/{moduleId}/validate
POST /semantic/{moduleId}/edit
GET  /semantic/{moduleId}/status
POST /semantic/{moduleId}/command
```

The generic dispatcher must retain the existing 1 MiB request limit, five
second semantic timeout, Rack/UI-thread queue, object-response validation, and
cancellation handling. It captures old module state only for `EDIT`, invokes
the module, and pushes one undo entry only after successful commit. `COMMAND`
does not create undo.

The response capability ID must match the target's adapter. A module lacking
the interface returns a structured capability error rather than being described
as a missing Sibyl.

## 13. Moirai Semantic API

Moirai's capability response reports API version 1, schema version 1, limits,
supported program modes, curves, retrigger policies, adoption policies, edit
operations, and current revision.

Read views:

- `summary`: clock, lanes, assignments, program metadata, computed peak,
  sustain, gate/release duration, looping, output range, and point counts.
- `program`: one complete authored program selected by `id`.
- `channel`: label and both lane assignments selected by zero-based `channel`.
- `lane`: lane settings, default, and sparse overrides selected by `id=A|B`.
- `full`: authoritative complete bank.

Edit request:

```json
{
  "expected_revision": 7,
  "apply_at": "nextTrigger",
  "active_voice_policy": "finishCurrent",
  "operations": []
}
```

Supported ordered operations:

- `replace_bank`
- `upsert_program`
- `delete_program`
- `clone_program`
- `apply_preset`
- `assign_program`
- `set_channel_label`
- `set_lane_defaults`
- `set_macro_binding`
- `set_output_mode`
- `set_clock`

All operations apply to one private authored copy in request order. Then perform
one full validate-and-compile pass. Any error rejects the entire transaction,
leaves accepted/pending state untouched, and returns:

```json
{
  "ok": false,
  "error": {
    "code": "invalid_program",
    "path": "/operations/1/program/gatePath/0/duration",
    "message": "duration must contain exactly one of ms, seconds, or beats"
  }
}
```

Revision mismatch uses `code: revision_conflict` and includes
`currentRevision`. A successful edit increments the accepted revision exactly
once regardless of operation count and returns accepted, active, and pending
revision fields plus warnings.

Commands in v1:

- `trigger`: selected lane or both lanes, channel `0..15`.
- `reset`: same semantics as the RESET jack.
- `select`: set inspected lane/channel without changing bank revision.

Commands are performance/UI actions and do not create Rack undo history.

## 14. MCP and Octavia Skill Work

In `MCP/mcp_server/Octavia_MCP.py`, add typed inputs and wrappers:

- `vcv_moirai_get_capabilities`
- `vcv_moirai_get_bank`
- `vcv_moirai_get_program`
- `vcv_moirai_validate`
- `vcv_moirai_edit`
- `vcv_moirai_get_status`
- `vcv_moirai_command`

The current Rack-side Octavia implementation has named monitor, frozen
snapshot, triggered-snapshot, analysis, and comparison HTTP routes, but the
Python MCP server does not yet expose those routes as tools. Close that existing
bridge gap with reusable Octavia wrappers rather than embedding observation in
the Moirai API:

- `vcv_octavia_get_monitors`
- `vcv_octavia_create_snapshot`
- `vcv_octavia_get_snapshot`
- `vcv_octavia_analyze_snapshot`
- `vcv_octavia_compare_snapshot`
- `vcv_octavia_get_triggered_snapshots`

These wrappers are thin typed clients for the existing `/audio/monitors`,
`/audio/snapshot`, `/audio/snapshot/{id}`, `/audio/analyze`, `/audio/compare`,
and `/audio/triggered-snapshots` contracts. They are general Octavia tools, not
Moirai-specific tools, and must preserve `pending`, `snapshot_expired`,
`analysis_busy`, and other structured states so an agent can poll or recover
without recapturing.

The wrappers call the generic `/semantic` routes, preserve structured rejection
envelopes for agent recovery, and never ask the server to interpret Moirai's
musical schema. `vcv_moirai_edit` requires `expected_revision` in its typed
input. Mark read tools read-only and mutation/command tools non-read-only.

Update `MCP/tests/test_server_contract.py` for tool presence, route use,
revision forwarding, and rejection-envelope behavior. Add
`MCP/skill/octavia/references/moirai.md` with discovery, read-before-edit,
validate/edit/status, cable-patching, and revision-conflict recovery workflows;
route to it from the Octavia skill only when Moirai/envelope-bank work is
requested. Add Moirai to the module catalog in
`MCP/skill/octavia/references/leviathan.md`; preserve the current modular
reference routing rather than expanding the root skill with the full Moirai
schema.

## 15. Existing Octavia Observation Integration

The current repository already supplies the observation substrate anticipated
by the original Moirai concept:

- Octavia retains `masterL`, `masterR`, and monitor probes `A..D` on one
  Rack-frame timeline.
- Snapshots are immutable and support repeatable mono/stereo analysis and
  same-window comparison.
- Sibyl events can publish exact-onset requests through the allocation-free
  `OctaviaObservationBus`, targeted to a specific Octavia module ID.
- `/audio/triggered-snapshots` maps a Sibyl request to the resulting snapshot.

The retained history is currently 262,144 frames and the bounded snapshot pool
holds 12 entries. Callers must choose pre/post windows within the live history,
poll only while post-roll is pending, and tolerate eviction of old snapshots.

Moirai adds no private observation bus and does not take an Octavia module ID in
its bank schema. The integration remains physical and composable:

```text
Sibyl GATE/VEL/M1-M3  -> Moirai
Moirai A              -> Octavia monitor A       (envelope CV)
downstream audio      -> Octavia monitor B       (audible consequence)
Sibyl event marker    -> existing ObservationBus (capture timing only)
```

The agent resolves the live Octavia module ID, authors the Sibyl event's
`observation` object with monitor names and `preFrames`/`postFrames`, and uses
the resulting frozen snapshot for analysis or comparison. Octavia's v1 monitor
boundary records channel 0 of a poly cable, so per-channel Moirai verification
must either test channel 0 or route the desired channel through an ordinary
polyphonic breakout/select module before the monitor.

This workflow is a Phase 5 integration acceptance test, not a dependency of
Moirai playback. Missing Octavia, missing monitor cables, a full snapshot pool,
or an expired snapshot must never affect envelope processing or bank state.
Automatic judgment and iterative correction remain deferred; v1 only ensures
that an agent can explicitly capture, inspect, and then issue a normal
revisioned Moirai edit.

## 16. Telemetry and Display Snapshot

Publish a coherent sequence-guarded atomic snapshot at approximately 60 Hz:

```cpp
struct MoiraiTelemetrySnapshot {
    int acceptedRevision;
    int activeRevision;
    int pendingRevision; // -1 means none
    int channels;
    float estimatedBpm;
    bool externalClock;
    uint16_t activeMask[2];
    float selectedValue[2];
    float selectedPhase[2];
    int selectedStage[2];
};
```

Program names, stage names, and contour preview geometry come from an acquired
immutable compiled bank on the UI thread; they do not cross as mutable strings
from audio. The status API combines that bank metadata with one coherent
telemetry read.

Debug Terminal telemetry is optional. If added, gate it with
`isDragonKingDebugEnabled()` and preserve the macro timing fields `Process`,
`Step`, and `Draw` exactly. Moirai-specific voice or display timings follow
those fields rather than replacing them.

## 17. Factory Bank and Presets

The compiled-in preset catalog must always compile and expose these stable
template IDs:

- `factory_ad_percussive`
- `factory_ar`
- `factory_adsr`
- `factory_ahr`
- `factory_dadsr`
- `factory_trapezoid`
- `factory_pluck`
- `factory_pad`
- `factory_swell`
- `factory_duck`
- `factory_cycle_triangle`
- `factory_cycle_sine`
- `factory_wyrm_ar`
- `factory_wyrm_d1` through `factory_wyrm_d10`

The catalog is not serialized wholesale and does not count against the
32-program authored-bank limit. The initial authored bank contains only an
editable copy of `factory_adsr`, assigned as both lane defaults, so a new module
retains ample room for user programs.

Wyrm-derived shapes should copy the authored point values once into Moirai
preset constructors; do not call Wyrm runtime code or inherit its
normalization/wrapping behavior. Record the source shape name in a comment and
test endpoint/bounds behavior.

Applying a factory preset clones it to a deterministic editable ID such as
`preset_adsr_a_ch01`, replacing the prior clone at that target. This prevents a
local preset edit from mutating the shared immutable factory vocabulary used by
other channels.

## 18. Test Plan

### Pure curve tests

- Every curve returns exact endpoints, finite values, and stays in bounds.
- Monotone cubic never overshoots adjacent authored endpoints.
- Non-cycle contours clamp and never wrap.
- Cycle contours wrap only at completion.

### Compiler/JSON tests

- Valid staged and contour examples round-trip without semantic loss.
- Every hard limit has an at-limit pass and over-limit rejection.
- Unknown fields, duplicate/invalid IDs, missing assignments, invalid loops,
  variant cycles, non-finite numbers, unsorted points, and illegal sampling are
  rejected with stable paths.
- Summary/program/channel/lane/full views return the documented fields.
- Factory bank and every preset compile.
- Serialized maximum legal representative bank remains below 1 MiB.

### Engine tests

- Gate attack, early gate-low release, sustain, and release completion.
- One-shot ignores gate-low; cycle repeats.
- Counted and while-gate loops and EOC selection.
- All four retrigger policies.
- 16-channel independence across both lanes.
- Mono broadcast, matching poly input, and missing-channel neutral behavior.
- Velocity default, trigger-sampled time/variant, and smoothed continuous level
  or curve modulation.
- Deterministic variation repeats for the same seed/event identity.
- Output mode voltages and panel LEVEL behavior.
- Millisecond duration is tempo-independent; beat duration follows tempo.
- Reset produces idle zero-state and no EOC.
- EOC pulse width is correct across sample rates.
- No allocations occur in a sustained `process()` test after setup.

### Revision/control tests

- Revision conflict makes no state change.
- Multi-operation failure rolls back every operation.
- One successful transaction increments once.
- Each adoption boundary behaves exactly as specified.
- Existing voices finish on the old generation.
- Generation reclamation never frees accepted, active, pending, hazard, or
  voice-referenced banks.
- Rack undo restores the prior bank and redo restores the edit.
- Invalid patch state falls back safely and reports an error.

### Octavia/MCP contract tests

- Existing Sibyl routes/tools and undo rules remain intact.
- Existing observation history, snapshot, analysis, measurement, panel-contract,
  and Sibyl observation-bus tests remain intact after semantic generalization.
- Generic dispatch validates object responses and only records undo for EDIT.
- Moirai tools use generic routes and forward revision/adoption fields.
- Semantic size limits, timeout, missing-module, missing-capability, and handled
  rejection responses are covered.

### UI/integration smoke tests

- Anchors place every control/port correctly from `Moirai.svg`.
- Display follows selected lane/channel and shows accepted/active/pending state.
- Manual trigger works with GATE unpatched.
- Context-menu preset creates one undoable edit.
- A Sibyl poly GATE/VELOCITY/M1–M3 patch drives matching Moirai channels.
- A Sibyl observation marker captures Moirai channel-0 CV and its downstream
  consequence through physically cabled Octavia A/B monitors in one frame
  window; losing the observation never changes Moirai playback.
- Closing/reopening the Rack/DAW window rebuilds UI state without stale graphics
  handles.

## 19. Implementation Sequence and Gates

### Phase 0 — General semantic transport

1. Add `OctaviaSemanticControl.hpp` and the `SibylControl` adapter.
2. Generalize Octavia's job/queue/dispatcher.
3. Add generic routes while retaining old Sibyl routes.
4. Extend contract tests.

Gate: all existing Sibyl and Octavia tests pass unchanged in behavior; a mock
generic semantic module proves dispatch and edit-only undo. This includes the
observation history, analysis, measurement, observation-bus, monitoring-panel,
and Sibyl-triggered observation tests introduced by the current Octavia
monitoring implementation.

### Phase 1 — Pure model, JSON, compiler, presets

1. Add types, curve helpers, JSON parser/serializers, compiler, and preset bank.
2. Implement schema limits and structured issues.
3. Add pure focused tests.

Gate: all factory programs compile; invalid state is rejected deterministically;
no Rack module/UI code is required for core tests where avoidable.

### Phase 2 — Engine and generation adoption

1. Implement two-lane voice engine and clock estimator.
2. Implement macro resolution, output modes, EOC, reset, and telemetry.
3. Add immutable generation publication/reclamation.
4. Add engine and stress tests.

Gate: behavioral matrix passes at multiple sample rates and the steady-state
audio path allocates nothing.

### Phase 3 — Rack module and persistence

1. Add module class, stable IDs, ports, parameters, JSON state, and semantic
   handler.
2. Add atomic edit/adoption behavior and UI-originated undo action.
3. Register the hidden model and add module tests.

Gate: module state round-trips, semantic edits are one-entry undoable, and a
headless Rack-linked test runs 32 voices.

### Phase 4 — Panel and compact display

1. Create `res/Moirai.svg` with component anchors.
2. Generate panel/labels assets and anchor atlas.
3. Implement widget, display snapshot, preset menu, and manual trigger.
4. Perform Rack visual and lifecycle smoke tests.

Gate: no hardcoded component fallback is needed, display reads snapshots only,
and context loss/reopen is clean.

### Phase 5 — MCP wrappers and workflow documentation

1. Add Moirai tools and tests.
2. Add typed MCP wrappers for Octavia's already-implemented named observation
   routes and cover them in the MCP server contract test.
3. Add the Octavia skill reference and routing.
4. Exercise discover → read → validate → edit → status → patch workflow against
   a live module.
5. Exercise Sibyl-triggered, physically monitored envelope/consequence capture
   through Octavia's existing snapshot path.

Gate: an agent can configure both lanes, assign multiple channels, patch Sibyl,
verify active revision, resolve a triggered snapshot, and compare the monitored
envelope with its downstream consequence without using raw module-state
replacement or an unwrapped HTTP call.

### Phase 6 — Authoritative validation

From WSL, use quick local focused tests during iteration. Final Windows
validation uses the documented MINGW64 bridge:

```sh
/mnt/c/msys64/usr/bin/bash.exe -lc \
  'export MSYSTEM=MINGW64; \
   export PATH=/mingw64/bin:/usr/bin; \
   hash -r; \
   cd /home/Plasm/Leviathan && \
   make -j10 test-fast RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro" && \
   make -j10 plugin.dll'
```

Then run:

```sh
python3 tools/validate_plugin_json_tags.py
```

Final manual Rack checks cover model discovery, panel layout, poly cable channel
counts, preset undo/redo, patch save/reload, semantic edits during long releases,
external clock changes, and window close/reopen. `dist` is optional for a release
candidate; `install` is not part of ordinary validation unless explicitly
requested.

## 20. Definition of Done

Moirai v1 is complete when:

- The fixed 12 HP hardware contract and hidden model are registered.
- Both lanes correctly run 16 independent voices with the specified modulation,
  clock, retrigger, loop, output, reset, and EOC behavior.
- The authored bank is bounded, round-trippable, locally persistent, and safely
  compiled into immutable realtime data.
- Edits are revisioned, atomic, boundary-adopted, and exactly-one-entry undoable.
- Existing voices can finish old generations with proven safe reclamation.
- Existing Sibyl semantic APIs remain compatible after generic dispatch lands.
- Moirai's typed MCP tools and Octavia reference support a complete agent
  workflow.
- The audio path is allocation-free and avoids expensive transcendental calls.
- Focused tests, full `test-fast`, authoritative Windows `plugin.dll`, tag
  validation, and Rack smoke tests pass.

Do not stage or commit any files; repository history remains under user control.
