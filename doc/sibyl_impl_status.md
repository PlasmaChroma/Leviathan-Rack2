# Sibyl – Implementation Status & Roadmap

*Last updated: 2026-08-21*

This document tracks the implementation status of the **Sibyl** module against the specification in [`doc/sibyl.md`](./sibyl.md).

---

## 1. Executive Summary

Sibyl is currently an **integrated end-to-end prototype**. The module, panel, basic sequencer playback, complete-composition JSON path, and initial Octavia adapter are operational. Contract Core work is in progress: strict v1 composition validation is now covered by focused tests, while revision/adoption state, transport semantics, and several playback policies still need implementation before the semantic API can be considered v1-compliant.

| Component | Status | Notes |
|---|---|---|
| **Module Lifecycle & Panel** | ✅ Complete | 10 HP panel (`res/Sibyl.panel.svg` + `res/Sibyl.labels.svg`), 16 physical jacks, tooltips. |
| **Patch Persistence** | 🟡 Partial | Full composition serialization exists; authoritative revision assignment, failure status, and quantized adoption remain pending. |
| **Composition Compiler** | 🟡 Contract Core | Strict v1 types, enums, structural limits, pitch grammar, ranges, references, macro targets, and forward-compatible warnings are tested. Further cross-object semantic checks remain. |
| **Lock-Free DSP Engine** | 🟡 Partial | Atomic immutable-snapshot reading and polyphonic playback exist; pending adoption, bounded reclamation, and hot-path hardening remain. |
| **Hardware Clock & Sync** | 🟡 Partial | Physical I/O and basic clock/scene behavior exist; quantized requests, hysteresis, interpolation, phase policy, and non-loop termination remain. |
| **Pitch & Scale Compiler** | 🟡 Substantial | Scientific pitch, 12 scales, Euclidean degree wrapping, and compiled voltage bounds are implemented and tested; playback-level edge cases need broader coverage. |
| **Octavia Bridge (Core)** | 🟡 Routes Integrated | All routes exist. Full views and bulk replacement work; focused validation, complete transport, structured responses, and pending-state reporting remain. |
| **Granular `EDIT` Ops** | ⏳ Pending | Atomic incremental mutations (`upsert_pattern`, `set_scene_track`, `set_meta`, etc.). |
| **Edit Phase Policies** | ⏳ Pending | Boundary quantization (`nextBeat`, `nextScene`) and phase policies (`preserve`, `restartChanged`). |
| **OLED Display Widget** | ⏳ Pending | Real-time front-panel OLED / LED matrix display for title, scene, BPM, and track activity. |
| **Micro-timing & Swing** | ⏳ Pending | `swing` subdivision delay and `microshift` step offsets. |

---

## 2. Completed Features ("What We Have")

### 2.1 Core Threading & Memory Model
- **Atomic Snapshot Read:** `SibylModule::process()` acquires `m_activeCompositionPtr` via an atomic load (`memory_order_acquire`) and performs no JSON parsing or mutex locking. Allocation-free behavior and bounded off-thread reclamation still need explicit tests and hardening.
- **Off-Thread Compilation:** JSON payloads received via Octavia or Rack deserialization are parsed, validated, and compiled into an immutable `sibyl::Composition` struct on the UI/control thread before atomic publication.
- **Patch Persistence:** Implemented `dataToJson()` and `dataFromJson()` to serialize the full composition and revision into Rack patch files (`.vcv`).

### 2.2 Contract Core Compiler Milestone
- Added strict validation for v1 object types, enum values, documented structural limits, IDs, channels, event ranges, pitch representation exclusivity, scientific note/root grammar, resolutions, scene references, and macro targets.
- Unknown fields produce path-specific warnings rather than silently changing semantics.
- Macro definitions now compile into the immutable snapshot; previously they were serialized and consumed by DSP but not parsed from composition JSON.
- Added `tests/sibyl_json_spec.cpp` to `test-fast`, covering valid compilation, revision assignment, scale-degree compilation, macro compilation, warnings, and representative rejection paths.

### 2.3 Hardware I/O & Real-Time Sync
- **10 HP Physical Interface:**
  - `CLOCK IN`: Schmitt trigger with interval timing measurement, `externalPpqn` division, and timeout fallback (`hold`, `freeRun`, `internal`).
  - `RUN IN`: High = play, Low = pause (retains position, closes gates).
  - `RESET IN`: Schmitt trigger for resetting arrangement to scene 0, beat 0.
  - `SCENE TRIG IN`: Schmitt trigger for manual stepping to the next scene.
  - `SCENE CV IN`: 0–10 V continuous scene addressing with range clamping.
  - `MACRO 1–4 IN`: 0–10 V CV performance inputs routing unipolar/bipolar modulation to targets (`probability`, `velocity`, `gate`).
  - `V/OCT OUT`: Polyphonic pitch output ($max(\text{channel}) + 1$ channels, up to 16).
  - `GATE OUT`: Polyphonic gate output.
  - `VELOCITY OUT`: Polyphonic velocity (0–10 V).
  - `MOD OUT`: Polyphonic modulation (0–10 V unipolar or -5 to +5 V bipolar).
  - `CLOCK OUT`: 10 V, 1 ms pulse generator at configured `outputPpqn`.
  - `SCENE OUT`: 10 V, 1 ms pulse generator on every scene transition.
  - `EOC OUT`: 10 V, 1 ms pulse generator on arrangement loop wrap.

### 2.4 Pitch, Timing, & Playback Features
- **Pitch Representations:**
  - Direct V/Oct voltage (`pitchV`, C4 = 0 V).
  - Scientific pitch grammar (`note`, e.g. `"C4"`, `"Eb3"`, `"F#5"`).
  - Scale degree quantization (`degree` + `octave`) supporting all 12 scales (`major`, `natural_minor`, `harmonic_minor`, `melodic_minor`, `dorian`, `phrygian`, `lydian`, `mixolydian`, `locrian`, `major_pentatonic`, `minor_pentatonic`, `chromatic`) with Euclidean octave wrapping.
- **Step Timing & Modulations:**
  - Rational tick resolution parsing (`1/1` through `1/64`, including dotted `d` and triplet `t`).
  - Sample-rate accurate linear `glideMs` pitch interpolation.
  - Ratchet subdivisions (`ratchets` 1–16 evenly spaced attacks per step).
  - `tie: true` gate holding (suppresses re-attack and maintains gate high across steps).
  - Deterministic pseudo-randomness for `probability` checks seeded by `meta.seed` and event coordinates.

### 2.5 Semantic Interaction Protocol (Octavia Reference Adapter)
- **`vcv_sibyl_get_capabilities`**: Advertises API version 1, schema version 1, active revision, and supported operations.
- **`vcv_sibyl_get_composition`**:
  - `summary`: Token-efficient overview returning metadata, clock/transport settings, track declarations, scene outlines with track counts, pattern IDs with derived durations and event counts.
  - `full`: Complete serialized composition tree.
  - `pattern`: Single pattern inspection with derived stats.
  - `scene`: Single scene inspection with derived beat totals.
- **`vcv_sibyl_get_status`**: Real-time telemetry (`activeRevision`, `running`, `clockSource`, `estimatedBpm`, active `sceneId`, `sceneRepeat`, current `beat` playhead position).
- **`vcv_sibyl_validate`**: Dry-run candidate validation returning validation errors and paths without side effects.
- **`vcv_sibyl_transport`**: Route and request plumbing are present; only immediate `reset`/`restart` currently mutate runtime state.
- **`vcv_sibyl_edit`**: Bulk `replace_composition` transaction with optimistic concurrency protection (`expected_revision`).

---

## 3. Pending Features ("What Needs Done")

### 3.1 Granular Incremental Edit Operations
Expand `handleSibylRequest(Operation::EDIT, ...)` to support partial atomic mutations on a working copy:
- `set_meta`: Update individual metadata properties (`title`, `bpm`, `prompt`, `swing`, `scale`, `root`, `rootOctave`, `seed`).
- `set_clock`: Update clock properties (`externalPpqn`, `outputPpqn`, `externalTimeoutMs`, `onExternalStop`).
- `upsert_track` / `delete_track`: Add, update, or remove track declarations (with `object_in_use` checks).
- `upsert_pattern` / `delete_pattern`: Add, replace, or delete pattern definitions.
- `upsert_scene` / `delete_scene` / `reorder_scenes` / `set_scene_track`: Modify arrangement scenes and track-pattern assignments.
- `upsert_macro` / `delete_macro`: Configure performance macro targets and ranges.
- `object_in_use` validation: Reject deletion of patterns or tracks that are referenced in existing scenes within the same transaction.

### 3.2 Phase Policies & Quantized Adoption Boundaries
- **Quantized Adoption (`apply_at` / `applyAt`):**
  - `nextBeat`: Adopt pending snapshot at next quarter-note boundary (default).
  - `nextStep`: Adopt at next step-grid boundary.
  - `nextScene`: Adopt when the current scene completes its repeat cycle.
  - `immediate`: Adopt on next sample.
- **Phase Policies (`phase_policy`):**
  - `preserve`: Keep track playhead positions and pattern anchors (default).
  - `restartChanged`: Reset pattern phase only for tracks whose pattern content or assignment changed.
  - `restartAll`: Reset pattern phase for all tracks to step zero.

### 3.3 Front-Panel OLED Display Widget
- Implement a custom NanoVG `TransparentWidget` inside the OLED bezel (`res/Sibyl.panel.svg` display rect):
  - Track activity / gate matrix (16-channel LED indicators).
  - Song title (`meta.title`) and BPM.
  - Active scene ID and repeat counter (e.g. `VERSE A [1/2]`).
  - Active revision and sync source (`INT` / `EXT`).

### 3.4 Micro-timing & Swing
- **Swing:** Delay even-numbered subdivision steps by `meta.swing` (0.0 to 0.49 fraction of step).
- **Microshift:** Apply signed sub-step timing offset (`microshift` strictly between -0.5 and +0.5).

---

## 4. Codebase Architecture Summary

```text
src/
├── Sibyl.cpp           # SibylModule (DSP, I/O, SibylControl handler) & SibylWidget
├── SibylControl.hpp    # In-process C++ RTTI adapter interface for Octavia
├── SibylJSON.hpp       # Parser, validator, and serializer declarations
├── SibylJSON.cpp       # Jansson-based schema compiler, note parser, scale quantizer
└── SibylTypes.hpp      # C++ data structures for immutable Composition snapshots
res/
├── Sibyl.panel.svg     # 10 HP vector background panel & bezel art
└── Sibyl.labels.svg    # Vector label overlays for all 16 jacks
```
