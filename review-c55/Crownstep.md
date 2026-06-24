# Crownstep Review

## 1. Executive Summary

Crownstep turns checkers, chess, or Othello moves into quantized pitch/accent/modulation sequences with background AI. The game logic and persistence are well tested and the concept is musically original. The former audio-thread recursive mutex blocker has been replaced by immutable triple-buffered playback snapshots; AI cancellation/teardown and broader Rack stress remain.

Release readiness: 7/10

## 2. Module Inventory

- Source/UI: `Crownstep.cpp`, `CrownstepCore.hpp`, module/playback/serialization/shared/UI files. Assets: `res/crownstep.svg`, board JPGs, `res/icon/chess.svg`; UI also references shared `res/proc.svg` anchors/icons.
- Params (7): sequence length, root, scale, reserved Run, New Game, debug add moves, range. Inputs: Clock, Reset, Transpose, Root. Outputs: Pitch, Accent, Mod, EOC. Lights: reserved Run, human turn, AI turn.
- Menu/state: game/player/difficulty/highlight/texture, quantizer/key/scale, pitch range/bipolar/bias/divider/layout/source and counter style. JSON persists board/game/history/sequence/UI and compatibility state. No expander.

## 3. DSP and Audio/CV Correctness

Clock/reset use Schmitt triggers and outputs are sample-and-hold between edges (`CrownstepPlayback.cpp:149-242`). Root CV is semitone quantized and persistence tests cover it. EOC intentionally stays high for a cycle except a special length-one pulse. Module is mono. Reset clears playhead but retains held pitch until the next edge, a behavior that should be specified/tested.

## 4. UI, Panel, and Interaction Review

The board, legal-move highlights, step counter and turn lights clearly expose the generative premise. Context-menu depth is high but logically grouped. Debug Add Moves is a visible parameter despite its name and should be development-only or renamed. Board textures require GPU/zoom checks.

## 5. Performance Review

- Severity: Low — sequence publication copies variable-length history on the UI/serialization side; audio playback uses stable snapshots without locking (`CrownstepPlayback.cpp`).
- Severity: Medium — AI is correctly moved to a worker and joined on destruction (`CrownstepModule.cpp:330-345`), but teardown can block until a search returns.
- Severity: Medium — large board UI and animation vectors need stress testing; game search itself is off-thread.

## 6. Stability and Rack Integration

Worker requests use snapshots and result mutexes. Persistence coverage is strong. Test deletion during maximum-depth AI, duplicate/preset while a request is active, and ensure stale results cannot apply after mode/reset changes. Reserved enum slots preserve patches.

## 7. Code Quality and Maintainability

Pure game rules and serialization separation are strong. `CrownstepCore.hpp` and UI are very large, and recursive locking hides ownership. Publish immutable sequence snapshots to the engine instead of sharing mutable vectors.

## 8. Musical Usefulness

Game-derived sequencing, multiple board interpretations and live quantization offer genuine value beyond a random sequencer. Defaults produce usable pitch. The many mapping modes need examples and a manual to expose sweet spots.

## 9. Bugs and Risk Register

| ID | Severity | Area | Finding | Evidence | Suggested Fix |
| -- | -------- | ---- | ------- | -------- | ------------- |
| CROWNSTEP-001 | Low | Audio RT | Resolved: audio playback formerly locked the UI-shared recursive mutex. | Triple-buffer snapshot handoff in `CrownstepPlayback.cpp`; nonblocking regression in `tests/crownstep_persistence_spec.cpp` | Retain concurrency regression coverage. |
| CROWNSTEP-002 | High | Lifecycle | Destructor may block on uncancellable AI search. | `CrownstepModule.cpp:330-345` | Add cooperative cancellation/deadline checks. |
| CROWNSTEP-003 | Medium | UX | Debug Add Moves is a production parameter. | `CrownstepShared.hpp:83`; `CrownstepModule.cpp:234` | Hide behind debug flag or rename as a musical randomize action. |
| CROWNSTEP-004 | Medium | DSP | Reset/held-output semantics are undocumented. | `CrownstepPlayback.cpp:169-177` | Define and test clear-vs-hold behavior. |

## 10. Recommended Fix Plan

### Must Fix Before Release

Bound AI teardown; audio-thread mutex acquisition is resolved.

### Should Fix Soon

Clarify reset/EOC semantics and remove debug-facing production controls.

### Nice to Have

Add patch examples and reduce menu depth.

## 11. Suggested Tests

Keep game/persistence tests; add Rack clock/reset bursts, length 0/1/max, simultaneous UI edits, ThreadSanitizer, delete/duplicate/preset during AI, 44.1-192 kHz EOC timing, disconnected CV, extreme transpose, 20 instances, board zoom and texture-missing fallback.

## 12. Final Verdict

Status: Near release-ready  
Primary blocker: uncancellable AI teardown and remaining real-Rack stress coverage  
Best next action: add cooperative AI cancellation and snapshot publication stress tests
