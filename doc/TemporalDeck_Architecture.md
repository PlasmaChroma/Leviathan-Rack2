# TemporalDeck Architecture Notes

Date: 2026-04-02
Scope: Post-restructure runtime boundaries and test ownership.

## Runtime Boundaries

### `src/TemporalDeck.cpp`
- Rack module orchestration and glue.
- Owns param/input/output/light wiring and JSON patch state IO.
- Coordinates extracted components each audio frame.

### `src/TemporalDeckTransportControl.hpp/.cpp`
- Owns transport edge-state behavior:
  - freeze/reverse/slip button transitions
  - freeze-gate falling edge behavior
  - auto-freeze latch application
  - sample seek revision apply path
- No Rack UI dependencies; pure control transitions over engine state.

### `src/TemporalDeckSampleLifecycle.hpp/.cpp`
- Owns async sample lifecycle worker and state machine:
  - load-path decode
  - rebuild-from-decoded
  - prepared-sample handoff
  - allocation/decode fallback signaling
- Explicitly separates worker-thread sample state from DSP-thread process path.

### `src/TemporalDeckEngine.hpp`
- DSP runtime and transport math.
- Processes frame input into audio/output state and transport observables.

### `src/TemporalDeckPlatterInput.hpp/.cpp`
- Platter gesture aggregation and per-frame snapshot handoff.
- Keeps UI gesture timing/hold details out of engine internals.

### `src/TemporalDeckSamplePrep.hpp/.cpp`
- Sample preparation helpers (resample/trim/mode selection) used by lifecycle worker.

## Data Flow

1. UI + CV + gates update module fields and platter input state.
2. `TemporalDeckTransportControl` updates latches/seek state for current frame.
3. `TemporalDeckSampleLifecycle` applies pending prepared sample and rebuild requests.
4. `TemporalDeckEngine::process()` executes DSP for the frame.
5. `TemporalDeck.cpp` publishes outputs/lights/UI-facing atoms.

## Test Ownership

- `tests/temporaldeck_engine_spec.cpp`: engine-level DSP/transport invariants.
- `tests/temporaldeck_platter_input_spec.cpp`: platter snapshot/hold semantics.
- `tests/temporaldeck_sample_prep_spec.cpp`: sample prep correctness.
- `tests/temporaldeck_virtual_integration_spec.cpp`: cross-component behavior:
  platter -> transport control -> engine, including edge-regression cases.

## Current Intent

- Keep `TemporalDeck.cpp` orchestration-focused.
- Add new files only for full behavior/state-machine boundaries.
- Prefer extending the virtual integration suite before introducing more runtime split points.
