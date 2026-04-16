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

### High

- Unsafe expander protocol access with arbitrary right-side neighbors. `TemporalDeck` treats any module on the right that exposes `leftExpander.producerMessage` as a valid scope expander, then reads `rightExpander.consumerMessage` as `DisplayToHost` and writes `HostToDisplay` into the neighbor's message buffer without first confirming the neighbor is actually `TDScope` or otherwise protocol-compatible. That is an out-of-contract cast/write and can corrupt memory or misread foreign expander payloads when a different expander module is docked on the right. Relevant code: `src/TemporalDeck.cpp:1118-1125`, `src/TemporalDeck.cpp:1144-1249`.

## TDScope

### High

- Unsafe expander protocol read before neighbor validation. `TDScope::process()` dereferences `leftExpander.consumerMessage` as `HostToDisplay` before it checks whether the module on the left is actually `TemporalDeck`. If some unrelated expander-capable module sits to the left, this is still an unchecked reinterpret-cast read of a foreign message layout. Relevant code: `src/TDScope.cpp:160-163`, with the actual neighbor-type check only happening later at `src/TDScope.cpp:189`.

### Low

- Internal diagnostic text is always rendered on the shipped panel. The widget unconditionally draws `MISS <count>` in normal operation, which looks like leftover instrumentation rather than end-user UI. Relevant code: `src/TDScope.cpp:1235-1251`.

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
