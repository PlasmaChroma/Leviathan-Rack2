# Leviathan Plugin Review

Date: 2026-04-16

Scope: static review of shipped Leviathan modules plus a full local `make test` run.

Test status:
- `make test` passed.
- Covered directly by tests: `TemporalDeck`, `Crownstep`, `PanelSvgUtils`, and some expander/protocol paths.
- Not directly covered by dedicated tests in this repo: `IntegralFlux`, `Proc`, `Bifurx`, most `TDScope` UI behavior.

## IntegralFlux

No material issues found in this pass.

Residual risk:
- `IntegralFlux` has no dedicated automated tests in this repo, so timing/cache regressions and UI-state edge cases remain under-covered.

## Proc

No material issues found in this pass.

Residual risk:
- `Proc` also lacks dedicated automated coverage, so any future changes to the timing-cache / BLEP / preview path will rely mostly on manual validation.

## TemporalDeck

No material issues found in this pass.

Residual risk:
- `TemporalDeck` is well covered by tests in this repo, but expander behavior still benefits from manual validation against real neighboring modules in Rack.

## TDScope

No material issues found in this pass.

Residual risk:
- Most `TDScope` behavior is still UI/manual-validation territory rather than dedicated automated coverage.

## Crownstep

### Medium

- The run control is effectively dead. `RUN_PARAM` is configured, but playback ignores it by hardcoding `bool running = true`, and the run LED is forced off every frame. That means patches cannot actually stop playback through the configured run parameter, and the indicator never reflects state. Relevant code: `src/CrownstepModule.cpp:183`, `src/CrownstepPlayback.cpp:111-118`, `src/CrownstepPlayback.cpp:173`.

Residual risk:
- The automated suite covers game logic and persistence well, but not the full panel wiring/UI behavior, which is exactly where this bug sits.

## Bifurx

No material issues found in this pass.

Residual risk:
- There is no dedicated automated test coverage for `Bifurx`, so DSP-edge and UI-preview regressions are still mostly dependent on manual validation.

## Cross-Module / Plugin-Wide

### Low

- The DragonKing debug flag path looks unreachable in the current tree/build. Runtime code only checks for `res/dragonking.txt`, but `res/` does not contain that file and the build only packages `res` as distributables. The repo does currently have a top-level `dragonking.txt`, but that path is never consulted by the code. As written, the debug-only branches guarded by `isDragonKingDebugEnabled()` appear impossible to enable from the current source layout/package recipe. Relevant code: `src/plugin.cpp:9-16`, `Makefile:16-18`.
