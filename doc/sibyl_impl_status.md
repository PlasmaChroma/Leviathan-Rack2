# Sibyl – Implementation Status & Release-Hardening Roadmap

*Last updated: 2026-08-25*

This document tracks Sibyl against the current schema-v2 specification in
[`doc/sibyl.md`](./sibyl.md). It describes the state of the module now; completed
development history is summarized rather than left mixed into the remaining plan.

---

## 1. Completion Assessment

Sibyl's planned functional architecture is **implemented end to end**. It is no longer
best described as a prototype: it is a functional AI-first polyphonic sequencer and
arranger entering **playability refinement and release hardening**.

The major implementation milestones are complete:

- schema compilation, validation, serialization, and portable composition files;
- semantic inspection, atomic editing, optimistic revisions, and musical adoption;
- internal and external clocking, reconstructed clock output, transport, and scene control;
- polyphonic pitch, gate, velocity, and three independent modulation lanes;
- probability, swing, microshift, ratchets, glide, ties, and multi-step gates;
- race-free telemetry, bounded snapshot ownership, and the oracle display;
- physical inputs, manual performance buttons, and a persistent loop override.

The remaining work is not another large feature milestone. It is a bounded release tail:
native Rack acceptance of the latest panel and musical changes, explicit real-time
regression instrumentation, broader persistence/integration coverage, and a final
compatibility freeze.

| Area | Current state | Release significance |
|---|---|---|
| Module lifecycle and panel | ✅ Functionally complete | Latest layout needs final native visual acceptance |
| Schema-v2 compiler | ✅ Functionally complete | Add selected cross-object edge cases as they are discovered |
| Playback and timing | ✅ Functionally complete | Latest multi-step gates need live musical acceptance |
| Clock, transport, and hardware control | ✅ Live accepted | Recheck manual buttons and loop override with the final panel |
| Revisioned editing and adoption | ✅ Complete | Existing focused and live stress coverage is strong |
| Real-time ownership and telemetry | ✅ Implemented and stress-tested | Add explicit allocation/locking regression checks |
| Octavia semantic bridge | ✅ Functionally complete | Run final end-to-end contract acceptance after schema freeze |
| Persistence | ✅ Implemented | Broaden corrupt-file, patch reload, and undo integration coverage |
| Oracle display | ✅ Implemented | Final typography, clipping, animation, and context-recreation pass remains |

---

## 2. Current Product Contract

### 2.1 Composition and Editing

- Schema v2 uses `Meta → Tracks → Patterns → Arrangement` with optional scene
  descriptions for section-level musical intent.
- Composition updates are validated and compiled off the audio thread into immutable
  snapshots.
- `summary`, `full`, single-pattern, and single-scene inspection views are available.
- Atomic edit transactions include scalar metadata/clock edits; track, pattern, scene,
  and macro upsert/delete; scene-track assignment; scene reordering; and deliberate full
  replacement.
- Accepted, active, and pending revisions remain distinct. Changes can adopt at
  `immediate`, `nextStep`, `nextBeat`, or `nextScene` using `preserve`,
  `restartChanged`, or `restartAll` phase policy.
- Portable composition files and Rack patch state serialize schema v2. Legacy schema-v1
  portable files are deliberately rejected rather than silently reinterpreted.

### 2.2 Musical Event Model

- Pitch may be expressed as direct V/Oct, scientific pitch, or scale degree plus octave.
- Twelve scale modes, Euclidean degree wrapping, and compiled ±10 V pitch bounds are
  implemented.
- Sparse events support velocity, probability, swing, signed microshift, linear glide,
  ratchets, ties, and deterministic seeded choices.
- `gate` and track `defaultGate` are durations measured in pattern steps from 0 through
  1024. Values above 1 express sustained notes directly. Ties are reserved for legato
  continuation or pitch changes without retriggering.
- Ratcheted events use a 0–1 gate fraction within each slice; an explicit multi-step gate
  combined with ratchets is rejected.

### 2.3 Polyphonic Outputs

Sibyl publishes up to 16 channels according to the highest assigned track channel:

- `V/OCT`, `GATE`, and `VEL`;
- `MOD1`, `MOD2`, and `MOD3` as independent literal −10 V to +10 V lanes;
- monophonic `CLK`, `SCENE`, and `EOC` pulse outputs.

The stable schema field for MOD1 remains `mod`; MOD2 and MOD3 use `mod2` and `mod3`.
Modulation macro amounts and clamps are likewise expressed directly in volts.

### 2.4 Clock, Transport, and Performance Control

- Internal clocking follows composition BPM. External clocking includes bounded tempo
  estimation, jitter/outlier resistance, between-edge interpolation, and `hold`,
  `freeRun`, or `internal` timeout behavior.
- CLOCK OUT has its own reconstructed phase domain and honors `outputPpqn` independently
  of the input edge rate.
- Runtime commands include play, pause, stop, restart, panic, next/previous/select scene,
  and reseed. Runtime state does not rewrite the saved composition.
- RUN, RESET, SCENE TRIG, and SCENE CV share the same boundary and destination-phase
  machinery used by semantic transport.
- Gold panel buttons manually trigger TRIG, RUN, and RESET. The CLK-column button cycles
  `AUTO → LOOP → ONCE`; AUTO follows the composition while LOOP and ONCE are persistent
  overrides. Patched RUN retains precedence over the manual run state.

### 2.5 Oracle Display

- Displays `[current/total] scene name`, repeat state, per-track phase and gate activity,
  clock/run state, BPM, accepted/active/pending revision state, warnings, and errors.
- The lower message area shows the active scene description and falls back to the
  composition prompt when no description is present.
- The footer exposes `AUTO LOOP`/`AUTO ONCE`, `LOOP`, or `ONCE` so both the selected mode
  and AUTO's effective composition behavior are visible.
- Display metadata and phase telemetry use the same race-free publication model consumed
  by the semantic status route.

---

## 3. Completed Engineering Milestones

### 3.1 Real-Time Ownership and Telemetry

- Immutable composition, adoption, and transport objects use hazard-protected owner pools
  reclaimed on the control thread.
- The DSP thread does not parse JSON, take ownership locks, or destroy the final reference
  to a published object.
- Sequence-guarded telemetry publishes scene/repeat position, gates, clock source,
  estimated BPM, revisions, and per-track phase without exposing mutable DSP state.
- A 129-operation live Rack/Octavia stress run covered rapid and interleaved edit/transport
  publication without stale pending state, freeze, deadlock, reported interruption, or
  visible unbounded growth.

### 3.2 Adoption, Transport, and Hardware Semantics

- Boundary and phase-policy helpers are shared by semantic, physical, and natural scene
  transitions.
- Changed-length modulo mapping, stale-gate/glide cleanup, newest-wins coalescing, and
  destination step-zero generation are covered by focused module tests.
- An 18-scenario live transport acceptance run covered quantized commands, scene
  navigation, restart domains, reseeding, panic, revision stability, RUN precedence, and
  non-looping termination.
- An external-clock/hardware run covered 120/90 BPM convergence, jitter and outliers,
  reconstructed PPQN ratios, timeout modes, RESET, RUN, SCENE TRIG, Scene CV hysteresis,
  and clock-domain restart behavior.

### 3.3 Musical Timing

- Event lookup is compiled for allocation-free scheduled-onset crossing.
- Swing applies to eligible straight subdivisions while dotted and triplet grids remain
  authored as written.
- Positive and negative microshift work across pattern wrapping, including shifted
  step-zero events.
- Ratchet and tie timing is anchored to shifted event onset.
- A live musical acceptance run covered swing extremes, dotted/triplet immunity,
  microshift extremes, shifted ratchets, delayed ties, clock modes, and scene phase modes.

### 3.4 Automated Coverage

The routine suite contains focused Sibyl tests for:

- adoption and phase mapping;
- external-clock estimation;
- hardware-control decisions and hysteresis;
- JSON compilation and schema-v2 validation;
- ordered atomic edit transactions;
- Rack-linked module playback, display telemetry, persistence, manual buttons, three
  modulation lanes, and multi-step gates;
- timing and scheduled event crossing;
- runtime transport parsing and normalization;
- Octavia/Sibyl adapter contract behavior.

Both WSL-focused builds and authoritative native MINGW64 `plugin.dll` builds have passed
throughout the implementation and the current playability work.

### 3.5 ANT1 Native Acceptance

A 10/10 ANT1 live acceptance run passed on VCV Rack 2.10.0 with Leviathan 2.9.1. It
covered the final panel and oracle display, the `AUTO → LOOP → ONCE` button cycle and all
four footer presentations, patch-state persistence, manual and patched transport
precedence, non-looping termination, fractional and multi-step gates, interruption, ties,
ratchets, swing, three independent bipolar modulation lanes, revision adoption, rapid
newest-wins edits, graphics recovery, and final test-state restoration. No stuck gates,
transport freezes, stale pending revisions, loop-state ambiguity, or graphics errors were
observed. The Rack patch was left unsaved.

---

## 4. Release-Hardening Checklist

These are the remaining completion gates. They should be closed before declaring Sibyl
released and freezing compatibility-sensitive IDs and file semantics.

### 4.1 Final Native Rack Acceptance

- [x] Inspect the final input/output matrix, gold buttons, labels, and jack alignment at
  practical zoom levels.
- [x] Exercise TRIG, RUN, RESET, and loop buttons alongside patched inputs.
- [x] Confirm `[X/Y]` scene naming, descriptions, prompt fallback, larger status text, and
  `AUTO LOOP`/`AUTO ONCE`/`LOOP`/`ONCE` presentation without clipping.
- [x] Patch MOD1–3 polyphonically and verify independent −10 V to +10 V behavior.
- [x] Hear-test fractional, one-step, and multi-step gates; ties; ratchets; and interruption
  by later events across straight and swung internal-clock patterns.
- [ ] Repeat the gate/interruption hear-test under an external clock source.
- [x] Close and reopen the Rack/DAW window to verify NanoVG context recreation and display
  cache recovery.

### 4.2 Real-Time Regression Guardrails

- [x] Add an explicit hot-path allocation guard for steady-state `process()`; the hot path
  contains no locking primitives.
- [x] Add a repeatable sustained edit/transport reclamation stress test rather than
  relying only on the completed manual 129-operation run.
- [ ] Exercise rapid UI/display reads during snapshot reclamation under a sanitizer-capable
  test environment when practical.

### 4.3 Persistence and Contract Closure

- [x] Cover patch reload of pending accepted revisions, all loop-override states, and
  stopped/non-looping arrangements.
- [x] Define diagnostics as derived state and verify patch reload does not resurrect stale
  serialized warnings or errors.
- [ ] Exercise Rack undo/preset integration around authoritative composition replacement.
- [x] Add portable-file cases for malformed/foreign envelopes, missing composition data,
  truncated JSON, oversize rejection, and schema-version mismatch while retaining the
  accepted composition.
- [x] Run the complete Octavia/Sibyl contract suite against the final schema-v2 plugin.
- [x] Review examples and agent guidance once more for stale schema-v1, single-MOD, or
  normalized-modulation language.

### 4.4 Compatibility Freeze

- [ ] Decide that the schema-v2 event and macro vocabulary is sufficient for the first
  public release.
- [ ] Freeze parameter, input, output, and light enum ordering at release.
- [ ] Freeze portable composition envelope semantics and document the future migration
  policy before accepting schema v3 work.
- [ ] Perform the final native `test-fast`, `plugin.dll`, packaging, and fresh-install
  smoke test.

---

## 5. Playability Refinement Policy

The present tuning work is in scope. A useful late-stage change should make Sibyl easier
to compose for, patch, understand, or perform without undermining deterministic playback,
transactional editing, or real-time safety.

Examples already accepted under this policy include scene descriptions, `[X/Y]` scene
position, manual transport buttons, visible loop state, three voltage-native modulation
lanes, and multi-step gates. Similar changes should include focused schema/DSP tests and a
native build, then be added to the final Rack acceptance checklist when they affect sound
or panel presentation.

Future ideas are not release blockers unless deliberately promoted into the v2 contract.
Examples include modulation-only events, modulation glide, a panel step editor, additional
macro transforms, or a new schema version.

---

## 6. Codebase Map

```text
src/Sibyl.cpp                    module DSP, persistence, control handler, and widget
src/SibylTypes.hpp               immutable composition structures
src/SibylJSON.{hpp,cpp}          schema compiler, validator, and serializers
src/SibylEdit.{hpp,cpp}          ordered atomic semantic edits
src/SibylAdoption.{hpp,cpp}      adoption boundaries and phase policies
src/SibylTransport.{hpp,cpp}     runtime transport contract
src/SibylHardwareControl.{hpp,cpp} physical-control decisions
src/SibylClockEstimator.{hpp,cpp} external-clock estimation and fallback
src/SibylTiming.{hpp,cpp}        scheduled event timing
tests/sibyl_*                    focused and Rack-linked Sibyl coverage
tests/octavia_sibyl_contract_spec.py Octavia adapter contract coverage
res/Sibyl.svg                    editable split-panel master
doc/sibyl.md                     authoritative software specification
```
