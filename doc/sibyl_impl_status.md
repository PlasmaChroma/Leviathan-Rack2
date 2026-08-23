# Sibyl – Implementation Status & Roadmap

*Last updated: 2026-08-22*

This document tracks the implementation status of the **Sibyl** module against the specification in [`doc/sibyl.md`](./sibyl.md).

---

## 1. Executive Summary

Sibyl is currently an **integrated end-to-end prototype**. The module, panel, sequencer playback, complete-composition JSON path, semantic Octavia adapter, revisioned adoption, granular atomic edits, and runtime transport core are operational. The transport/API milestone has passed an 18-scenario live Rack/Octavia acceptance run, including follow-up verification of RUN precedence and non-looping termination. Remaining v1 work is concentrated in physical-control semantics, real-time lifetime and telemetry hardening, complete phase-preserving adoption, external-clock quality, and musical micro-timing. The OLED follows the telemetry foundation rather than preceding core correctness.

| Component | Status | Notes |
|---|---|---|
| **Module Lifecycle & Panel** | ✅ Complete | 14 HP panel (`res/Sibyl.panel.svg` + `res/Sibyl.labels.svg`), 16 physical jacks, tooltips. |
| **Patch Persistence** | 🟡 Substantial | Full composition serialization, authoritative revision assignment, failure/warning status, and safe immediate load adoption exist; broader persistence/undo integration tests remain. |
| **Composition Compiler** | 🟡 Contract Core | Strict v1 types, enums, structural limits, pitch grammar, ranges, references, macro targets, and forward-compatible warnings are tested. Further cross-object semantic checks remain. |
| **Lock-Free DSP Engine** | ✅ Live Accepted | Immutable publication uses audio-thread hazard pointers and bounded control-thread reclamation; a 129-operation live edit/transport stress run passed without anomalies. |
| **Hardware Control & Sync** | ✅ Live Accepted | Physical controls and external-clock behavior passed consolidated live cable-level acceptance, including estimation, timeout policies, reconstructed output, RESET, RUN, and scene controls. |
| **Pitch & Scale Compiler** | 🟡 Substantial | Scientific pitch, 12 scales, Euclidean degree wrapping, and compiled voltage bounds are implemented and tested; playback-level edge cases need broader coverage. |
| **Octavia Bridge (Core)** | 🟡 Substantial | All routes exist. Full views, atomic edits, optimistic revision checks, adoption, transport commands, and pending-state reporting work; focused validation and broader integration coverage remain. |
| **Runtime Transport** | ✅ Core Accepted | Full v1 command vocabulary, quantized publication, scene/restart policies, runtime run state, panic, and probability epochs passed focused tests and an 18-scenario live Rack/Octavia acceptance run. |
| **Granular `EDIT` Ops** | ✅ Core Complete | All v1 operation names apply in order to a private copy, then pass through one strict full-composition validation/compile. |
| **Edit Adoption & Phase Policies** | ✅ Complete | All boundaries and phase policies, changed-length modulo mapping, stale-gate/glide handling, newest-wins coalescing, and destination step-zero generation are covered. |
| **OLED Display Widget** | 🟡 Implementation Complete | Custom oracle display for title, prompt, scene/repeat, 16 track playheads and gates, run/clock/revision state, pending adoption, warnings, and errors. Native Rack visual acceptance remains. |
| **Micro-timing & Swing** | ✅ Live Accepted | Straight-grid swing, signed microshift, macro swing, shifted ratchets/ties, loop wrapping, and clock/scene interaction passed live musical acceptance. |

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
- **14 HP Physical Interface:**
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

Live acceptance result:

- A 129-operation Rack/Octavia run exercised granular edits, all 12 adoption-boundary/phase-policy combinations, transport navigation, rejection stability, rapid immediate edits, and interleaved edit/transport publication.
- All 129 operations passed. Telemetry converged with no stale pending state, freeze, deadlock, reported audio interruption, or visible memory-growth pattern, and the original composition was restored.
- Structural review remains the authority for the no-audio-thread-destruction guarantee; the live run validates its observable behavior under pressure.

### 3.3 Complete Phase-Preserving Adoption

- Add module-level coverage for gate closure, phase reset, destination event generation, and coalesced edits at real DSP boundaries.
- Complete `preserve` semantics for changed pattern-length modulo mapping, newly inserted events behind the playhead, removed sounding events, unchanged sounding-event continuity, and changed-pitch glide behavior.
- Verify that `restartChanged` and `restartAll` generate destination step-zero events at adoption.

Implementation progress (2026-08-22):

- Centralized the per-channel adoption action for `preserve`, `restartChanged`, and `restartAll` and added focused coverage for unchanged continuity, changed-channel gate closure, and restart selection.
- Changed material now cancels a stale in-flight pitch glide while retaining its current pitch, rather than continuing toward a target from the replaced snapshot.
- Preserved phase is explicitly mapped modulo the replacement pattern duration; longer replacements retain their existing elapsed phase.
- Added a Rack-linked `sibyl_module_spec` exercising real `SibylModule::process()` boundaries, newest-wins coalescing, pending-state clearance, and destination step-zero pitch/gate output.
- The module harness exposed and fixed a stale-scene evaluation bug: after a natural scene crossing, the same sample previously generated events from the source scene. Playback now evaluates the destination scene immediately.
- WSL and native Windows `test-fast` pass, and the authoritative MINGW64 `plugin.dll` builds successfully. This milestone is accepted.

### 3.4 External Clock Hardening

- Add bounded external-tempo estimation, interpolation between detected edges, and estimator hysteresis.
- Verify stable `hold`, `freeRun`, and `internal` timeout transitions without moving detected edges away from their arrival samples.
- Broaden playback-level coverage for reconstructed CLOCK OUT phase and restart behavior.

Implementation progress (2026-08-22):

- Added an allocation-free bounded external-clock estimator. Measured tempo is constrained to Sibyl's 20–400 BPM contract, per-edge movement is limited, and small jitter uses a slower smoothing coefficient.
- Between-edge phase is interpolated from the learned interval but capped at the next expected quantum. A detected edge always completes the remaining quantum on its actual arrival sample, so prediction never relocates a physical edge.
- Implemented explicit `hold`, learned-tempo `freeRun`, and composition-tempo `internal` timeout paths without clearing the learned estimator.
- `estimatedBpm` telemetry now reports the learned external tempo rather than the composition BPM while CLOCK IN is connected.
- Reconstructed CLOCK OUT now triggers from output-grid crossings and honors `outputPpqn`; it no longer emits a pulse for every external input edge.
- Focused estimator and Rack-linked module tests pass at 120 BPM, including 4-PPQN input to 1-PPQN output reconstruction. WSL and native Windows `test-fast` pass, and the MINGW64 `plugin.dll` builds successfully.
- Alternating-jitter and isolated-outlier tests verify estimator stability and bounded response.
- CLOCK OUT now owns a separate phase domain. Scene, arrangement, pattern, hardware RESET, and stop/restart behavior can realign it without changing the global musical timeline or clearing the learned external interval; randomness-only restart leaves it untouched.
- The `internal` timeout policy free-runs at the learned rate only until the next quarter-note boundary, then changes to the composition BPM. A returning physical edge cancels fallback while preserving estimator history.
- External-clock implementation is feature-complete.

Live acceptance result:

- An 8-scenario Rack/Octavia cable-level run passed CLOCK IN convergence at 120 and 90 BPM, continuous jitter rejection, isolated-outlier containment, and clean external resynchronization.
- Reconstructed CLOCK OUT passed 4→1, 4→2, and 4→4 PPQN configurations without observed duplicate or missing pulses after convergence.
- `hold`, `freeRun`, and next-beat `internal` timeout policies behaved as specified; the measured internal fallback followed the configured 80 BPM composition tempo.
- RUN falling-edge gate closure and position-preserving resume, external-edge-quantized RESET, SCENE TRIG quantization, Scene CV clamping/hysteresis, and restart clock-domain behavior all passed.
- Final telemetry was coherent (`revision == activeRevision`, no pending adoption/transport, no error or warnings), with no observed freeze, deadlock, xrun, audio interruption, or abnormal memory-growth pattern.
- Temporary clock/control modules and cables were removed, and the original `31-EDO Microtonal Leviathan` composition and patch topology were restored.

### 3.5 Swing and Microshift

- **Swing:** Delay alternating eligible subdivisions by `meta.swing` (0.0 to 0.49 fraction of a step).
- **Microshift:** Apply signed sub-step event offsets (`microshift` strictly between -0.5 and +0.5).
- Define and test interactions among straight, dotted, and triplet resolutions, ratchets, ties, scene boundaries, swing, and positive/negative microshift.

Implementation result (2026-08-22):

- Added a compiled O(1) step-to-event index so the audio scheduler can inspect nearby shifted events without scanning sparse patterns per sample.
- Replaced integer-step firing with allocation-free scheduled-onset crossing. Positive and negative microshift work across pattern wraps, including negative step-zero offsets before the next loop boundary.
- Swing delays odd subdivisions only for straight resolutions; dotted and triplet grids retain their authored timing. Global and per-track swing macro contributions are additive and clamped to 0–0.49.
- Gate length and ratchet slices are measured from the shifted onset. Adjacent tied events hold the preceding gate continuously through a positive swing/microshift delay and suppress re-attack at the tied onset.
- Focused timing tests cover straight/dotted/triplet eligibility, additive offsets, Euclidean wrapping, exact-once crossings, and compiled lookup. Rack-linked module tests cover sample-level swing delay, negative microshift, shifted ratchets, and delayed ties.
- The complete WSL and native Windows `test-fast` suites pass, and the authoritative MINGW64 `plugin.dll` builds successfully.

Live acceptance result:

- An 8-scenario Rack/Octavia musical run passed straight `1/16` swing at 0.00, 0.20, and 0.48 while keeping even subdivisions anchored and producing no observed duplicate or missing events.
- Dotted and triplet grids remained immune to swing. Positive and negative microshift, including ±0.48 and negative step-zero loop wrapping, produced the expected measured onset fractions.
- Shifted two-slice ratchets were anchored to their shifted onset with no attack at the old grid position. A positively delayed tied event maintained a continuous 10 V gate and changed pitch at the shifted onset without a gate re-attack.
- Global swing macro modulation moved both tracks additively, clamped at 0.49, and per-track swing modulation affected only its destination track.
- Internal/external clock operation and `restart`, `continue`, and `alignGlobal` scene transitions completed without observed stale events, freeze, deadlock, or audio interruption.
- Octavia's roughly 5 ms HTTP observation cadence was sufficient at the deliberately slow test tempos to resolve the tested onset fractions, but it is not an audio-sample-accurate measurement instrument; focused DSP tests remain the authority for sample-level boundaries.
- All temporary modules and cables were removed, and the original 11-module/14-cable topology and Sibyl composition were restored.

### 3.6 Front-Panel OLED Display Widget

Implemented a custom NanoVG `TransparentWidget` inside the OLED bezel (`res/Sibyl.panel.svg` display rect), reading the race-free telemetry and a hazard-protected immutable composition snapshot:

- Song title and shortened prompt.
- Active scene ID and repeat counter (for example `VERSE A [1/2]`).
- Track playheads and gate activity for up to 16 channels.
- Run state, clock source (`INT` / `EXT`), accepted/active/pending revision, and validation/error indication.

Implementation details:

- The central “oracle constellation” presents all 16 channels as two banks of eight phase rails. Active pattern playheads move independently and gated channels bloom with a cyan phosphor flare.
- A scene-progress aura traverses the glass behind the constellation without feeding state back to the engine.
- The bottom message band prioritizes validation errors, then warnings, then the shortened composition prompt. The footer distinguishes internal/external clock and exposes BPM plus active, accepted, and pending revisions.
- Per-track phase was added to the existing sequence-guarded atomic telemetry publication. Display metadata is copied on the UI thread while holding a dedicated immutable-composition hazard, preventing reclamation races without introducing DSP allocation or locking.
- `res/Sibyl.svg` is the 14 HP editable master. The expanded panel, outlined labels, display bezel, hidden `SIBYL_DISPLAY` anchor, and all jack anchors regenerate from it; the panel anchor atlas was regenerated.
- The wider layout replaces five narrow I/O rows with separate three-column input and output matrices. This gives the oracle substantially more width and height while preserving clear signal grouping.
- `sibyl_module_spec` verifies display metadata, normalized active-track phase, coherent revision/repeat state, and release of the display hazard. WSL `test-fast`, native Windows `test-fast`, the focused native Windows Sibyl test, and the authoritative `plugin.dll` build pass.

Remaining acceptance is visual: load the native plugin in Rack and inspect typography, clipping, animation cadence, gate flares, pending/error states, and graphics-context recreation at practical zoom levels.

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
├── Sibyl.panel.svg     # 14 HP vector background panel & bezel art
└── Sibyl.labels.svg    # Vector label overlays for all 16 jacks
```
