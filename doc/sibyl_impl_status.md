# Sibyl – Implementation Status & Roadmap

*Last updated: 2026-08-22*

This document tracks the implementation status of the **Sibyl** module against the specification in [`doc/sibyl.md`](./sibyl.md).

---

## 1. Executive Summary

Sibyl is currently an **integrated end-to-end prototype**. The module, panel, sequencer playback, complete-composition JSON path, semantic Octavia adapter, revisioned adoption, granular atomic edits, and runtime transport core are operational. The transport/API milestone has passed an 18-scenario live Rack/Octavia acceptance run, including follow-up verification of RUN precedence and non-looping termination. Remaining v1 work is concentrated in physical-control semantics, real-time lifetime and telemetry hardening, complete phase-preserving adoption, external-clock quality, and musical micro-timing. The OLED follows the telemetry foundation rather than preceding core correctness.

| Component | Status | Notes |
|---|---|---|
| **Module Lifecycle & Panel** | ✅ Complete | 10 HP panel (`res/Sibyl.panel.svg` + `res/Sibyl.labels.svg`), 16 physical jacks, tooltips. |
| **Patch Persistence** | 🟡 Substantial | Full composition serialization, authoritative revision assignment, failure/warning status, and safe immediate load adoption exist; broader persistence/undo integration tests remain. |
| **Composition Compiler** | 🟡 Contract Core | Strict v1 types, enums, structural limits, pitch grammar, ranges, references, macro targets, and forward-compatible warnings are tested. Further cross-object semantic checks remain. |
| **Lock-Free DSP Engine** | 🟡 Hardening In Progress | Immutable publication now uses audio-thread hazard pointers and bounded control-thread reclamation; sustained stress and explicit allocation/lock instrumentation remain. |
| **Hardware Control & Sync** | 🟡 Substantial | RUN edge gate closure, quantized RESET/SCENE TRIG/Scene CV requests, Scene CV hysteresis, destination phase behavior, external timeout policies, and non-loop termination are implemented. Cable-level live acceptance and clock-estimator quality remain. |
| **Pitch & Scale Compiler** | 🟡 Substantial | Scientific pitch, 12 scales, Euclidean degree wrapping, and compiled voltage bounds are implemented and tested; playback-level edge cases need broader coverage. |
| **Octavia Bridge (Core)** | 🟡 Substantial | All routes exist. Full views, atomic edits, optimistic revision checks, adoption, transport commands, and pending-state reporting work; focused validation and broader integration coverage remain. |
| **Runtime Transport** | ✅ Core Accepted | Full v1 command vocabulary, quantized publication, scene/restart policies, runtime run state, panic, and probability epochs passed focused tests and an 18-scenario live Rack/Octavia acceptance run. |
| **Granular `EDIT` Ops** | ✅ Core Complete | All v1 operation names apply in order to a private copy, then pass through one strict full-composition validation/compile. |
| **Edit Adoption & Phase Policies** | ✅ Core Complete | `immediate`, `nextStep`, `nextBeat`, and `nextScene` adoption plus `preserve`, `restartChanged`, and `restartAll`; broader module-level behavior tests remain. |
| **OLED Display Widget** | ⏳ Sequenced After Telemetry | Real-time front-panel OLED / LED matrix display for title, scene, BPM, and track activity; must consume the planned race-free telemetry snapshot. |
| **Micro-timing & Swing** | ⏳ Pending | `swing` subdivision delay and `microshift` step offsets. |

---

## 2. Completed Features ("What We Have")

### 2.1 Core Threading & Memory Model
- **Atomic Snapshot Read:** `SibylModule::process()` acquires `m_activeCompositionPtr` via an atomic load (`memory_order_acquire`) and performs no JSON parsing or mutex locking. Allocation-free behavior and bounded off-thread reclamation still need explicit tests and hardening.
- **Off-Thread Compilation:** JSON payloads received via Octavia or Rack deserialization are parsed, validated, and compiled into an immutable `sibyl::Composition` struct on the UI/control thread before atomic publication.
- **Patch Persistence:** Implemented `dataToJson()` and `dataFromJson()` to serialize the full composition and revision into Rack patch files (`.vcv`).

### 2.2 Revisioned Adoption Contract
- Separates the latest **accepted** composition/revision from the **active** snapshot currently sounding and the newest **pending** adoption request.
- Bulk replacement is compiled transactionally against `expected_revision`; acceptance increments the accepted revision immediately, while DSP activation waits for the normalized `apply_at` boundary.
- Supports `immediate`, `nextStep`, `nextBeat`, and `nextScene` adoption. If several accepted revisions coalesce before activation, the newest snapshot wins and its changed-track mask is conservatively compared with the currently sounding snapshot.
- Supports `preserve`, `restartChanged`, and `restartAll`. Changed channels have stale gates closed at adoption; restart policies reset the selected pattern phases.
- Patch/preset loads use the saved revision only for initial restoration. Subsequent authoritative replacements receive a fresh local revision and enter through the same immutable-snapshot publication path.
- `get_status` now distinguishes `revision`, `activeRevision`, and pending revision/boundary/policy, and reports the latest validation error and warnings.
- Added `tests/sibyl_adoption_spec.cpp` to `test-fast`, covering strict option names, all four boundary decisions, and conservative changed-channel detection.

### 2.3 Contract Core Compiler Milestone
- Added strict validation for v1 object types, enum values, documented structural limits, IDs, channels, event ranges, pitch representation exclusivity, scientific note/root grammar, resolutions, scene references, and macro targets.
- Unknown fields produce path-specific warnings rather than silently changing semantics.
- Macro definitions now compile into the immutable snapshot; previously they were serialized and consumed by DSP but not parsed from composition JSON.
- Added `tests/sibyl_json_spec.cpp` to `test-fast`, covering valid compilation, revision assignment, scale-degree compilation, macro compilation, warnings, and representative rejection paths.

### 2.4 Atomic Granular Edit Transactions
- Added all v1 operations: `set_meta`, `set_clock`, track/pattern/scene/macro upsert and delete, `reorder_scenes`, `set_scene_track`, and ordered `replace_composition`.
- Operations mutate a private JSON composition in request order; the result is then validated and compiled exactly once. Rejection leaves the accepted and pending snapshots unchanged.
- Upserts normalize object identity from the operation `id`. Scalar setters accept only their documented semantic paths.
- Referenced track and pattern deletion reports `object_in_use`; reassignment/removal earlier in the same transaction permits deletion.
- Added `tests/sibyl_edit_spec.cpp` to `test-fast`, covering the complete mutation families, ordering, reference protection, missing/unknown objects, validator rejection, and base-snapshot immutability.

### 2.5 Hardware I/O & Real-Time Sync
- **10 HP Physical Interface:**
  - `CLOCK IN`: Schmitt trigger with interval timing measurement, `externalPpqn` division, and timeout fallback (`hold`, `freeRun`, `internal`).
  - `RUN IN`: High/low effective run precedence with falling-edge gate closure and position-preserving resume.
  - `RESET IN`: Schmitt-triggered arrangement reset at the next external clock edge or internal beat, applying destination phase behavior.
  - `SCENE TRIG IN`: Schmitt-triggered next-scene request using the composition's configured adoption boundary.
  - `SCENE CV IN`: 0–10 V scene addressing with clamping, a 5%-of-bin hysteresis margin, and quantized transition routing.
  - `MACRO 1–4 IN`: 0–10 V CV performance inputs routing unipolar/bipolar modulation to targets (`probability`, `velocity`, `gate`).
  - `V/OCT OUT`: Polyphonic pitch output ($max(\text{channel}) + 1$ channels, up to 16).
  - `GATE OUT`: Polyphonic gate output.
  - `VELOCITY OUT`: Polyphonic velocity (0–10 V).
  - `MOD OUT`: Polyphonic modulation (0–10 V unipolar or -5 to +5 V bipolar).
  - `CLOCK OUT`: 10 V, 1 ms pulse generator at configured `outputPpqn`.
  - `SCENE OUT`: 10 V, 1 ms pulse generator on every scene transition.
  - `EOC OUT`: 10 V, 1 ms pulse generator on arrangement loop wrap.

### 2.6 Pitch, Timing, & Playback Features
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

### 2.7 Semantic Interaction Protocol (Octavia Reference Adapter)
- **`vcv_sibyl_get_capabilities`**: Advertises API version 1, schema version 1, active revision, and supported operations.
- **`vcv_sibyl_get_composition`**:
  - `summary`: Token-efficient overview returning metadata, clock/transport settings, track declarations, scene outlines with track counts, pattern IDs with derived durations and event counts.
  - `full`: Complete serialized composition tree.
  - `pattern`: Single pattern inspection with derived stats.
  - `scene`: Single scene inspection with derived beat totals.
- **`vcv_sibyl_get_status`**: Real-time telemetry including accepted/active/pending revisions, pending adoption options, run/clock state, active scene/playhead, and latest errors/warnings.
- **`vcv_sibyl_validate`**: Dry-run candidate validation returning validation errors and paths without side effects.
- **`vcv_sibyl_transport`**: Publishes validated runtime-only commands for DSP-boundary application and reports normalized action, target, boundary, phase mode, and pending scene.
- **`vcv_sibyl_edit`**: Bulk `replace_composition` transaction with optimistic concurrency protection (`expected_revision`), `apply_at`, and `phase_policy`.

### 2.8 Runtime Transport Core
- Added `play`, `pause`, `stop`, `restart`, `panic`, `next_scene`, `previous_scene`, `select_scene`, and `reseed`; `reset` normalizes to an arrangement restart.
- Transport requests are immutable and atomically published for audio-thread application. Composition adoption occurs first when both requests share a boundary.
- Runtime run state is separate from serialized `transport.running`, so performance commands do not change composition revision, patch state, or undo history.
- Pending commands support the same four musical boundaries and remain visible through `get_status`. A separate clock-boundary phase permits quantized commands while the arrangement playhead is paused.
- Scene changes apply the destination scene phase policy or a one-request override. Restart targets independently address scene, arrangement, assigned patterns, or deterministic randomness.
- `panic` is always immediate, closes gates, and cancels generated pulses. `reseed` changes a runtime probability epoch without modifying `meta.seed`; arrangement restart restores the composition-seeded epoch.
- Added `tests/sibyl_transport_spec.cpp` to `test-fast`, covering normalization, aliases, strict field applicability, required targets/destinations, boundaries, phase modes, and stable response names.
- Live Rack/Octavia acceptance completed 18 scenarios covering immediate and quantized play/pause, stop/reset/restart targets, reseeding, scene navigation, phase overrides, panic, edit/transport ordering, pending-edit survival, revision immutability, RUN precedence, non-looping termination, and Octavia meter inspection. Follow-up fixes were retested for a final 18/18 pass.

---

## 3. Planned Milestones (Priority Order)

### 3.1 Hardware Control Semantics — Implemented; Live Acceptance Pending

Unify physical control requests with the same boundary and phase machinery used by semantic transport commands.

- Close all gates on a patched RUN input's falling edge while preserving clock, arrangement, scene, and pattern position; resume without rewinding on its rising edge.
- Quantize RESET to the next external clock edge or internal beat boundary and apply the first scene's phase policy.
- Route SCENE TRIG through the composition's transition quantization rather than changing scenes immediately.
- Add hysteresis to SCENE CV selection and route a changed selection through the same quantized scene-transition path.
- Apply destination scene and per-assignment phase behavior consistently for physical and API scene changes.
- Document the implementation's exact Schmitt-trigger thresholds.

Implementation result:

- Physical RESET and scene selection now use an allocation-free pending request on the DSP thread and share the semantic boundary helpers.
- API, physical, and natural scene entry now resolve destination scene defaults and per-track assignment phase overrides through one path.
- Focused tests cover RUN edge decisions, internal/external reset boundaries, all four scene adoption boundaries, bidirectional Scene CV hysteresis, clamping, and phase-mode resolution.
- `test-fast` passes and the native Windows/MSYS2 `plugin.dll` builds successfully.
- A live cable-level Rack pass remains before this milestone is marked fully accepted.

Acceptance criteria:

- Focused DSP tests cover RUN high/low edges, gate closure and preserved resume position, RESET in internal and external clock modes, all supported scene boundaries, noisy Scene CV near a boundary, and destination phase behavior.
- Existing Sibyl JSON, edit, adoption, transport, and Octavia contract tests remain green under `test-fast`.
- The native Windows/MSYS2 `plugin.dll` build succeeds.
- A short live Rack/Octavia pass verifies physical RUN, RESET, SCENE TRIG, and Scene CV behavior without regressing the accepted runtime transport suite.

### 3.2 Real-Time Lifetime and Telemetry Hardening

- Replace intentionally retained composition, adoption-request, and transport-request histories with bounded, non-audio-thread reclamation.
- Guarantee that the DSP thread never destroys the final reference to a published snapshot or request.
- Publish status, playhead, track activity, revision, and clock state through an explicit race-free immutable telemetry snapshot rather than reading mutable DSP counters from the control/UI thread.
- Add sustained edit/transport stress coverage and hot-path checks for allocation, locking, and lifetime regressions.

This milestone is a prerequisite for the OLED so the display and Octavia consume the same safe runtime state.

Implementation progress (2026-08-22):

- Replaced unbounded composition, adoption-request, and transport-request histories with hazard-protected owner pools reclaimed only on the control thread.
- The DSP thread publishes hazards before dereferencing raw immutable objects and never releases an owning smart pointer, so it cannot destroy the final reference.
- Added a sequence-guarded atomic telemetry publication for scene position, repeat, gate activity, and clock source. `get_status` no longer reads mutable DSP scene counters or Rack input state directly and now exposes `gateMask` for the future OLED/activity view.
- WSL `test-fast` passes and the authoritative native MINGW64 `plugin.dll` builds successfully.
- Remaining work: add sustained edit/transport lifetime stress coverage, explicit hot-path allocation/lock checks, and expand the telemetry payload for per-track playheads and clock estimation.

### 3.3 Complete Phase-Preserving Adoption

- Add module-level coverage for gate closure, phase reset, destination event generation, and coalesced edits at real DSP boundaries.
- Complete `preserve` semantics for changed pattern-length modulo mapping, newly inserted events behind the playhead, removed sounding events, unchanged sounding-event continuity, and changed-pitch glide behavior.
- Verify that `restartChanged` and `restartAll` generate destination step-zero events at adoption.

### 3.4 External Clock Hardening

- Add bounded external-tempo estimation, interpolation between detected edges, and estimator hysteresis.
- Verify stable `hold`, `freeRun`, and `internal` timeout transitions without moving detected edges away from their arrival samples.
- Broaden playback-level coverage for reconstructed CLOCK OUT phase and restart behavior.

### 3.5 Swing and Microshift

- **Swing:** Delay alternating eligible subdivisions by `meta.swing` (0.0 to 0.49 fraction of a step).
- **Microshift:** Apply signed sub-step event offsets (`microshift` strictly between -0.5 and +0.5).
- Define and test interactions among straight, dotted, and triplet resolutions, ratchets, ties, scene boundaries, swing, and positive/negative microshift.

### 3.6 Front-Panel OLED Display Widget

Implement a custom NanoVG `TransparentWidget` inside the OLED bezel (`res/Sibyl.panel.svg` display rect), reading only the race-free telemetry snapshot:

- Song title and shortened prompt.
- Active scene ID and repeat counter (for example `VERSE A [1/2]`).
- Track playheads and gate activity for up to 16 channels.
- Run state, clock source (`INT` / `EXT`), accepted/active/pending revision, and validation/error indication.

---

## 4. Codebase Architecture Summary

```text
src/
├── Sibyl.cpp           # SibylModule (DSP, I/O, SibylControl handler) & SibylWidget
├── SibylAdoption.hpp   # Adoption boundaries, phase policies, and request contract
├── SibylAdoption.cpp   # Changed-track detection and adoption helpers
├── SibylControl.hpp    # In-process C++ RTTI adapter interface for Octavia
├── SibylEdit.hpp       # Atomic semantic edit result and transaction interface
├── SibylEdit.cpp       # Ordered JSON-tree mutations and full compile handoff
├── SibylJSON.hpp       # Parser, validator, and serializer declarations
├── SibylJSON.cpp       # Jansson-based schema compiler, note parser, scale quantizer
├── SibylTransport.hpp  # Runtime transport command and validation contract
├── SibylTransport.cpp  # Strict command parser and normalization helpers
└── SibylTypes.hpp      # C++ data structures for immutable Composition snapshots
res/
├── Sibyl.panel.svg     # 10 HP vector background panel & bezel art
└── Sibyl.labels.svg    # Vector label overlays for all 16 jacks
```
