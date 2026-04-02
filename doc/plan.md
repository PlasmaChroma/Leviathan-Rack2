# TemporalDeck Restructuring Plan

Date: 2026-04-02
Owner: ongoing Codex + human review

## Goal
Make platter/sample transport behavior easier to change and safer to test by isolating responsibilities and adding virtual (non-Rack) test coverage.

## Current Baseline
Completed slices already in place:
- `TemporalDeckEngine` extracted and unit-tested.
- Sample-prep helpers extracted (`TemporalDeckSamplePrep`).
- Platter input state extracted (`TemporalDeckPlatterInput`).
- Record trace menu item is re-enabled.
- Existing test harnesses are passing.

## Work Remaining

### 1) Extract sample-transport control layer
Scope:
- Move sample seek/apply-revision logic out of `TemporalDeck.cpp` process body.
- Isolate freeze/reverse/slip transport edge handling that is still module-glue heavy.

Deliverable:
- New small transport-control unit (header + cpp) used by `TemporalDeck.cpp`.

Acceptance criteria:
- No behavior change in live/sample playback.
- `TemporalDeck.cpp` process path is materially shorter and only orchestrates inputs/outputs.

Effort:
- ~0.5 to 1.0 human-day.

### 2) Extract sample lifecycle state machine
Scope:
- Isolate load/decode/prepare/install states and request serial handling.
- Keep worker thread logic separated from DSP runtime path.

Deliverable:
- Dedicated sample lifecycle component with explicit state transitions.

Acceptance criteria:
- Async load/rebuild paths behave identically.
- Failure fallback (allocation/decode) is preserved.
- Clear lock boundaries documented in code comments.

Effort:
- ~1.0 to 1.5 human-days.

### 3) Add virtual integration harness (platter -> input state -> engine)
Scope:
- Build a test harness that replays gesture sequences without Rack UI.
- Cover drag, wheel scratch, quick-slip trigger, freeze/reverse interaction, and sample mode transitions.

Deliverable:
- New integration-style test executable under `tests/`.

Acceptance criteria:
- Reproducible scripted gesture tests.
- Fails on known fragile regressions (lag drift, stale hold state, bad wrap/clamp transitions).

Effort:
- ~0.5 to 1.0 human-day.

### 4) Harden regression set around fragile edges
Scope:
- Add tests for edge transitions:
  - freeze gate falling edge behavior
  - sample loop on/off end handling
  - quick-slip one-shot semantics
  - sample seek while transport state changes

Deliverable:
- Expanded unit/integration specs wired into `make test`.

Acceptance criteria:
- All new tests pass locally and are deterministic.

Effort:
- ~0.5 human-day.

### 5) Final cleanup and documentation
Scope:
- Remove stale helper code and dead fields from module glue.
- Add short architecture notes (component boundaries + data flow).

Deliverable:
- Cleaned source layout and a short developer-facing note.
- Architecture note: `doc/TemporalDeck_Architecture.md`.

Acceptance criteria:
- No dead paths or duplicate logic left in `TemporalDeck.cpp`.
- `make test` and `make plugin.so` pass.

Effort:
- ~0.5 human-day.

## Suggested Execution Order
1. Sample-transport control extraction
2. Sample lifecycle extraction
3. Virtual integration harness
4. Edge-case regression expansion
5. Cleanup/docs pass

## File Management Notes
- Prefer a small number of cohesive units over many tiny helper files.
- For this restructuring pass, cap net-new module-side source files to two components:
  - `TemporalDeckTransportControl` (`.hpp` + `.cpp`)
  - `TemporalDeckSampleLifecycle` (`.hpp` + `.cpp`)
- Keep `TemporalDeck.cpp` as orchestration/glue:
  - Rack param/input/output/light wiring
  - state handoff between module/UI and extracted components
  - JSON serialization/deserialization and public module API forwarding
- Keep `TemporalDeckEngine`, `TemporalDeckPlatterInput`, and `TemporalDeckSamplePrep` as existing stable boundaries unless a behavioral bug requires change.
- Add a new file only when it owns a complete behavior/state-machine boundary; do not split out thin utility-only files unless reused by multiple components.
- If a proposed new file would be under ~150 lines, prefer merging into an existing component unless there is a clear compile-time or ownership reason.
- During extraction, remove moved logic from `TemporalDeck.cpp` immediately to avoid duplicate paths.
- Keep test ownership aligned with code ownership:
  - transport control behavior covered by integration-style gesture/transport specs
  - lifecycle behavior covered by deterministic async/load-state specs
- After each slice, ensure `make test` stays green before introducing another file boundary.

## Risk Notes
- Most behavior risk is around transport edge transitions and sample mode switching.
- Refactors should be done in small slices with test runs after each slice.
- Keep feature behavior stable first; defer tuning changes until after structure is stable.

## Definition of Done
- Core platter/sample logic is testable without Rack UI.
- Module file is primarily orchestration, not behavior-heavy logic.
- Regression suite catches previously fragile transition paths.
