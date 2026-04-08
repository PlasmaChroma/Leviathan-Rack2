# Crownstep Chess Refactor Tracker

## Objective
Implement a playable Chess mode inside Crownstep as an MVP while preserving the existing audio/output pipeline behavior and current Checkers functionality.

## MVP Definition (v1)
- 8x8 chess board rendered and interactable in Crownstep.
- Legal move generation for all pieces.
- Turn-based play: human vs simple AI.
- Game end detection: checkmate, stalemate.
- Basic promotion handling (auto-queen).
- Existing step/history/output pipeline remains active and stable.
- Mode selection between Checkers and Chess.

## Non-Goals (v1)
- Strong chess engine (search depth tuning, opening book, endgame tables).
- Advanced UI polish for chess notation/history display.
- Special draw rules beyond stalemate (50-move, repetition, insufficient material) unless trivial to add later.

## Current Baseline
- `IGameRules` abstraction exists and Checkers is already adapted.
- Crownstep module already routes gameplay through `gameRules`.
- Existing tests pass for Checkers behavior.

## Work Plan

### Phase 1: Core Chess Rules
- [x] Add chess board model and piece encoding in `CrownstepCore`.
- [x] Implement chess initial position constructor.
- [x] Implement pseudo-legal move generation for pawns, knights, bishops, rooks, queens, kings.
- [x] Implement legal move filtering (king safety / no self-check).
- [x] Implement captures and turn switching.
- [x] Implement promotion (auto-queen).
- [x] Implement check/checkmate/stalemate status.

### Phase 2: Engine Integration
- [x] Add `ChessRules : IGameRules`.
- [x] Add `chessRules()` accessor.
- [x] Add module game-mode selection (`Checkers` / `Chess`) in context menu.
- [x] Reset board/history/state cleanly on mode switch.
- [x] Keep sequence/history output path game-agnostic.

### Phase 3: UI + Interaction
- [x] Render chess pieces with readable minimal visuals.
- [x] Reuse selection + destination highlight flow for chess moves.
- [x] Keep existing move animation path working for chess moves.
- [x] Ensure complete-state overlay and hint behavior are consistent for chess.

### Phase 4: AI (MVP)
- [x] Implement simple legal-move picker (search/eval based, shallow depth by difficulty).
- [x] Lightweight tie-breaker prefers captures.
- [x] Ensure AI never outputs illegal move.

### Phase 5: Tests
- [x] Add chess initial position test.
- [x] Add legal move sanity tests from known board states.
- [x] Add checkmate and stalemate tests.
- [x] Add promotion test.
- [ ] Add integration test: module mode switch does not break output state machine.

## Architecture Notes
- Keep `Step` mapping and output CV generation downstream from game rules.
- Keep move representation generic enough for both Checkers and Chess.
- Avoid UI coupling to piece-type details outside board renderer.
- Keep game-specific rules in `CrownstepCore` adapters.

## Open Decisions
- [x] Castling in v1: deferred.
- [x] En passant in v1: deferred.
- [x] Draw rules beyond stalemate: deferred.
- [ ] AI personality presets vs single MVP policy.

## Progress Log
- 2026-04-07: Created Chess refactor tracker and phased implementation plan.
- 2026-04-07: Implemented ChessRules core (64-cell board, legal generation, king-safety filtering, auto-queen promotion, checkmate/stalemate outcome).
- 2026-04-07: Added module game mode switch (Checkers/Chess), persisted `gameMode`, and wired winner resolution through rule adapter.
- 2026-04-07: Added chess piece rendering branch (minimal readable glyph discs) and kept existing board interaction/animation path.
- 2026-04-07: Expanded `tests/crownstep_spec.cpp` with chess initialization, pin legality, stalemate, and checkmate tests (all pass).
- 2026-04-07: Added chess auto-queen promotion test.
- 2026-04-08: Re-verified build/tests after MVP pass (`make -B`, `build/tests/crownstep_spec` passing).

## Change Log (Implementation)
- Implemented `ChessRules` and `chessRules()` in `src/CrownstepCore.hpp`.
- Expanded board model capacity to support both checkers (32) and chess (64) through shared `BoardState`.
- Added chess move generation/evaluation helpers (legal filtering via king safety, checkmate/stalemate winner resolution, auto-queen promotion).
- Added module `gameMode` state in `src/Crownstep.cpp` with context menu mode switch and JSON persistence.
- Added chess rendering branch in `CrownstepBoardWidget` (minimal piece glyph discs) while reusing existing interaction and animation path.
- Added chess tests in `tests/crownstep_spec.cpp`: initial move count, pinned-piece legality, stalemate/checkmate outcomes, promotion.
- Remaining implementation tasks:
  - integration test for mode switch/output state machine
  - decide on AI personality presets vs single-policy AI
