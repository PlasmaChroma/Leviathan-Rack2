# Crownstep VCV Rack Module — Code Review

## Overview

Crownstep is a well-conceived module that turns board game moves (Checkers, Chess, Reversi) into
musical sequences — each move maps to a pitch step derived from the board position, with mod and
accent outputs driven by move energy. The architecture is sound: game logic is separated into a
`crownstep` namespace, an `IGameRules` interface abstracts the three game modes cleanly, AI search
runs on a dedicated worker thread, and playback happens on the audio thread with sequence data
protected by a `std::recursive_mutex`. The code is generally readable and defensively written.

The findings below are grouped by severity.

---

## Implementation Brief for Follow-Up Agent

Treat this review as an implementation queue, not just commentary. The highest-value pass is:

1. Fix JSON restoration for `stepCounterStyle`.
2. Fix duplicate diagonal board-value layouts in both checkers/shared code and chess-specific code.
3. Remove recursive re-locking from `emitStepAtClockEdge()` and nearby locked playback paths where practical.
4. Add focused regression tests for the first two fixes.
5. Apply small low-risk cleanups only after the behavioral fixes are covered.

Do not add or reorder module params/inputs/outputs/lights casually. Crownstep already has compatibility
reservations (`RUN_PARAM`, `RUN_LIGHT`), and changing enum order can break existing patches. If adding a
`GATE_OUTPUT`, treat it as a product/API decision, not a drive-by cleanup.

Useful validation commands in this repo:

```sh
make test-fast
make -j4 build/src/CrownstepModule.cpp.o build/src/CrownstepPlayback.cpp.o build/src/CrownstepSerialization.cpp.o build/src/CrownstepUI.cpp.o
```

If full plugin linking fails in this environment, do not assume it is caused by Crownstep. Object-level
builds plus `make test-fast` are the useful local checks here.

Lifecycle caution for DAW hosts:

- In some DAW integrations, the UI/GL layer can be destroyed and relaunched while the audio module
  instance continues processing.
- Treat all widget-side GL/NanoVG resources (buffers, textures, caches, framebuffers) as disposable:
  validate handles on draw and rebuild lazily after context/UI restart.
- Do not store long-lived assumptions that widget lifetime == module/audio lifetime.
- Keep audio state ownership independent from UI resource lifecycle so a UI restart cannot perturb
  DSP behavior.

---

## Bugs

### 1. `stepCounterStyle` Is Never Restored from JSON

**File:** `CrownstepSerialization.cpp`

```cpp
json_t* stepCounterStyleJ = json_object_get(rootJ, "stepCounterStyle");
if (stepCounterStyleJ) {
    stepCounterStyle = STEP_COUNTER_RIBBON;  // always RIBBON; json value is ignored
}
```

The condition fires when the key exists in the save, but the integer value is discarded and the
field is hardcoded to `STEP_COUNTER_RIBBON`. This means `STEP_COUNTER_BASIC` can never survive a
save/load round-trip. The correct line:

```cpp
stepCounterStyle = clamp(int(json_integer_value(stepCounterStyleJ)), 0, STEP_COUNTER_STYLE_COUNT - 1);
```

Implementation notes:

- The broken read is in `Crownstep::dataFromJson()` near the existing `highlightMode` restore.
- `dataToJson()` already writes `"stepCounterStyle"` correctly, so this is a read-side-only fix.
- Add coverage to `tests/crownstep_persistence_spec.cpp` by setting:

```cpp
source.stepCounterStyle = Crownstep::STEP_COUNTER_BASIC;
```

Then assert:

```cpp
bool stepCounterStyleOk = loaded.stepCounterStyle == source.stepCounterStyle;
```

Include that boolean in `scalarOk` and in the failure detail string. Without the test change, this bug can
silently regress again because the current round-trip test does not exercise the field.

---

### 2. Duplicate Layout Cases in `boardValueIndexForMove`

**File:** `CrownstepModule.cpp`, lines ~778–792

```cpp
case 3:
    return crownstep::serpentineDiagonalRank(row, col, 8, 8);  // "Linear (Diagonal)"
// ...
case 6:
    return crownstep::serpentineDiagonalRank(row, col, 8, 8);  // "Serpentine (Diagonal)"
```

Cases `3` (Linear Diagonal) and `6` (Serpentine Diagonal) call the exact same function with the
same arguments. One of them is likely calling the wrong function — either `linearDiagonalRank` was
intended for case 3, or serpentine is correct for one but not both.

Important correction: this is not chess-only. The same duplicate exists in the shared checkers layout
path in `CrownstepCore.hpp`:

```cpp
case 3:
    return serpentineDiagonalRank(row, posInRow, 8, 4);  // "Linear (Diagonal)"
// ...
case 6:
    return serpentineDiagonalRank(row, posInRow, 8, 4);  // "Serpentine (Diagonal)"
```

The expected behavior is:

- `Linear (Diagonal)` should traverse diagonals in a stable one-direction order.
- `Serpentine (Diagonal)` should alternate direction per diagonal.

Add a helper alongside `serpentineDiagonalRank()` in `CrownstepCore.hpp`, for example:

```cpp
inline int linearDiagonalRank(int row, int col, int rowCount, int colCount) {
    int diagonal = row + col;
    int rank = 0;
    for (int d = 0; d < diagonal; ++d) {
        int rowMin = std::max(0, d - (colCount - 1));
        int rowMax = std::min(rowCount - 1, d);
        rank += rowMax - rowMin + 1;
    }
    int rowMin = std::max(0, diagonal - (colCount - 1));
    return rank + (row - rowMin);
}
```

Then use it for case `3` in both places:

- `CrownstepCore.hpp::boardValueForIndex()` for checkers/dark-square board indexing (`8 x 4`)
- `CrownstepModule.cpp::boardValueIndexForMove()` chess lambda (`8 x 8`)

Do not change case `6`; it should continue to use `serpentineDiagonalRank()`.

Suggested tests:

- Add a focused test near the Crownstep persistence/spec tests or a small Crownstep core spec.
- Assert that layout `3` and layout `6` differ for at least one cell on both board sizes.
- Assert a few exact values for early diagonals so the intended ordering is pinned down:

```cpp
// 8x8 example
linearDiagonalRank(0, 0, 8, 8) == 0
linearDiagonalRank(0, 1, 8, 8) == 1
linearDiagonalRank(1, 0, 8, 8) == 2
serpentineDiagonalRank(0, 1, 8, 8) == 2
serpentineDiagonalRank(1, 0, 8, 8) == 1
```

---

### 3. Self-Deadlock Hazard in `emitStepAtClockEdge`

**File:** `CrownstepPlayback.cpp`

```cpp
void Crownstep::emitStepAtClockEdge() {
    std::lock_guard<std::recursive_mutex> lock(sequenceMutex);   // lock acquired
    // ...
    int length = activeLength();        // activeLength() also acquires sequenceMutex
    // ...
    int sequenceIndex = activeStartIndex() + playhead;  // same
```

This works today because `sequenceMutex` is a `std::recursive_mutex`. However, it is fragile: if
`sequenceMutex` is ever changed to a plain `std::mutex` (for performance or tooling reasons), this
will deadlock immediately. The fix is to call the private `computeActiveRange` helper directly from
within `emitStepAtClockEdge`, which is already factored out in the anonymous namespace of the same
file. The public `activeLength()` / `activeStartIndex()` wrappers are then reserved for callers
that don't already hold the lock.

Implementation notes:

- `computeActiveRange()` is already in the anonymous namespace in `CrownstepPlayback.cpp`.
- Inside `emitStepAtClockEdge()`, replace calls to `activeLength()` and `activeStartIndex()` with one
  direct call:

```cpp
const ActiveRange range = computeActiveRange(this, int(history.size()), currentSequenceCap());
int length = range.length;
```

Then compute:

```cpp
int sequenceIndex = range.start + playhead;
```

- Be careful: `pitchForSequenceIndex()` also locks `sequenceMutex`. Because `emitStepAtClockEdge()`
  already holds the lock, this is another recursive-lock dependency. A complete cleanup should avoid
  calling the public locking wrapper from this locked section.
- The clean shape is to extract a small internal helper for the unlocked pitch calculation, or inline
  the existing logic while the lock is already held. Keep behavior identical:
  - prefer `moveHistory` when present so pitch interpretation and quantization update live
  - fall back to stored `history[sequenceIndex].pitch` for older saves
  - return `0.f` for invalid indices
- Do not add locks in the audio path beyond the existing sequence lock. The goal is fewer nested locks,
  not broader locking.

---

## Design Issues

### 4. No `GATE_OUTPUT`

**Files:** `CrownstepShared.hpp`, `CrownstepCore.hpp`

`Step::gate` is populated (`step.gate = true` universally; accent/mod vary by move type), and the
field is serialized and deserialized faithfully. However, there is no `GATE_OUTPUT` in `OutputId`
and no gate voltage is ever set on any output. This appears to be an incomplete feature — users
likely expect a gate output on a sequencer module. Even a simple "always high while clocked" gate
would make the module more directly patchable.

---

### 5. `CrownstepCore.hpp` Is a 1600-Line Header

Every translation unit that includes `CrownstepShared.hpp` rebuilds the full AI search helpers,
all three sets of game rules, chess/othello board logic, and the `SCALES` array. This is a
significant compile-time hit. The `static const` `SCALES` array in a header will also create a
copy per TU (only `constexpr` or `inline` avoids that in C++17). Moving game-mode implementations
to separate `.cpp` files and exposing only declarations in the header would meaningfully speed up
incremental builds.

---

### 6. `isChessMode` / `isOthelloMode` Use `strcmp` on Every Call

**File:** `CrownstepModule.cpp`

```cpp
bool Crownstep::isChessMode() const {
    return gameRules && std::strcmp(gameRules->gameId(), "chess") == 0;
}
```

`gameId()` returns a string literal only to be compared character-by-character on each call. Since
`gameMode` is already an `int` enum, the simpler and faster check is:

```cpp
bool Crownstep::isChessMode() const { return gameMode == GAME_MODE_CHESS; }
bool Crownstep::isOthelloMode() const { return gameMode == GAME_MODE_OTHELLO; }
```

The string indirection buys nothing here and is more fragile (a typo in a `gameId()` override
would silently mis-dispatch).

This is a low-risk cleanup. After changing these helpers, scan call sites that might depend on
`gameRules` being non-null. Current construction calls `setGameMode()` during module setup, so `gameMode`
is the authoritative state.

---

### 7. `aiSide` Sign Normalisation in `chooseAiMoveForSnapshot` Is Confusing

**File:** `CrownstepModule.cpp`

```cpp
int requestAiSide = (request.aiSide >= 0) ? HUMAN_SIDE : AI_SIDE;
```

`AI_SIDE` is `-1` and `HUMAN_SIDE` is `+1`. The condition `>= 0` maps `HUMAN_SIDE (1)` → `HUMAN_SIDE`
and `AI_SIDE (-1)` → `AI_SIDE`, so the transformation is an identity. The round-trip through an
inequality test is misleading to readers; `requestAiSide = request.aiSide` would be equivalent and
clear. If there is a defensive intent (guarding against zero), a comment would help.

Recommended implementation:

```cpp
int requestAiSide = (request.aiSide < 0) ? AI_SIDE : HUMAN_SIDE;
```

This preserves the existing zero-handling behavior while making the normalization intent explicit.

---

### 8. `moveAnimationQueue` Uses Linear-Time Front Erasure

**File:** `CrownstepModule.cpp`

```cpp
moveAnimation = moveAnimationQueue.front();
moveAnimationQueue.erase(moveAnimationQueue.begin());
```

`std::vector::erase` at the front is O(n) — it shifts every remaining element. The queue is
bounded in practice (one entry per multi-jump hop), but this is the wrong container for the
pattern. `std::deque` would make both front-access and front-erasure O(1) with no other changes
required.

Implementation notes:

- Change `moveAnimationQueue` in `CrownstepShared.hpp` from `std::vector<MoveVisualAnimation>` to
  `std::deque<MoveVisualAnimation>`.
- Add `#include <deque>` if it is not already available through existing includes.
- Replace `erase(moveAnimationQueue.begin())` with `pop_front()` in `advanceUiAnimationClock()`.
- Existing range-for rendering code should continue to work with `std::deque`.

---

## Performance

### 9. `collectCapturesRecursive` Copies Vectors on Every Recursive Call

**File:** `CrownstepCore.hpp`, line ~252

```cpp
inline void collectCapturesRecursive(
    const BoardState& sourceBoard,
    int originIndex, int currentIndex, int currentPiece,
    std::vector<int> path,      // copy
    std::vector<int> captured,  // copy
    std::vector<Move>* outMoves
)
```

Both `path` and `captured` are taken by value, so every recursive hop allocates and copies two
vectors. For checkers multi-jump chains this can be 2–4 levels deep; for AI search at difficulty 3
("Hurt me plenty") this compounds considerably. The standard approach is to pass by reference and
use push/pop backtracking:

```cpp
void collectCapturesRecursive(
    const BoardState& board, int origin, int current, int piece,
    std::vector<int>& path, std::vector<int>& captured,
    std::vector<Move>* out
) {
    // recurse, then path.pop_back() / captured.pop_back() after the call
}
```

---

### 10. `othelloSearchForSide` Generates Opponent Moves at Every Node

**File:** `CrownstepModule.cpp`

```cpp
std::vector<Move> moves = crownstep::othelloGenerateLegalMovesForSide(board, sideToMove);
std::vector<Move> opponentMoves = crownstep::othelloGenerateLegalMovesForSide(board, -sideToMove);
if (depth <= 0 || (moves.empty() && opponentMoves.empty())) {
```

At every node, the opponent's moves are generated unconditionally, even though they are only needed
when `moves` is empty (forced pass). Generating legal moves for Othello is non-trivial (it scans
the board for all flip-chains). Deferring the opponent generation:

```cpp
std::vector<Move> moves = crownstep::othelloGenerateLegalMovesForSide(board, sideToMove);
if (depth <= 0) { return evaluate...; }
if (moves.empty()) {
    std::vector<Move> opponentMoves = crownstep::othelloGenerateLegalMovesForSide(board, -sideToMove);
    if (opponentMoves.empty()) { return evaluate...; }
    return -othelloSearchForSide(...);
}
```

…would halve opponent-move generation work at every non-pass interior node.

Implementation notes:

- Preserve negamax sign behavior exactly.
- Evaluate immediately when `depth <= 0`; no legal move generation is needed at depth zero.
- Only generate opponent moves if `moves.empty()`.
- If both sides have no moves, evaluate the terminal board from `maximizingSide`.

---

## Minor / Style

### 11. `boardValueRandomSeed = 0` Silently Corrected in Deserialization

**File:** `CrownstepSerialization.cpp`

```cpp
if (boardValueRandomSeed == 0u) {
    boardValueRandomSeed = 1u;
}
```

The correction is correct (seed 0 must not be used with `mt19937`), but a zero seed in a save file
is almost certainly data corruption. A `DEBUG_ONLY` warning or `assert` would help catch this
during development.

---

### 12. `boardCellCount()` Uses `board.size()` As a Cap

**File:** `CrownstepModule.cpp`

```cpp
int Crownstep::boardCellCount() const {
    int localCount = int(board.size());  // always MAX_BOARD_SIZE = 64
    int rulesCount = gameRules ? gameRules->boardCellCount() : localCount;
    return std::max(0, std::min(localCount, rulesCount));
}
```

`board` is `std::array<int, MAX_BOARD_SIZE>`, so `board.size()` is always 64. For checkers, this
means `localCount` is 64 and `rulesCount` is 32, so the min correctly returns 32. The cap is
logically correct but only by coincidence; `MAX_BOARD_SIZE` is the ceiling for any future game,
not the active game's cell count. If a future game mode had `boardCellCount() > 64` this would
silently truncate. The cap could just be removed since `gameRules->boardCellCount()` already
returns the correct value and the `board` array is always large enough.

Revised assessment: this is not a release blocker and may be defensively useful. Since the backing board
storage is fixed at `MAX_BOARD_SIZE`, the cap prevents a future game rule from accidentally exposing more
cells than storage supports. Leave this alone unless the board storage model changes.

---

### 13. `CHESS_ATLAS_ENABLED` Dead-Code Branch

**File:** `CrownstepUI.cpp`

```cpp
constexpr bool CHESS_ATLAS_ENABLED = true;
```

The `false` path (if it exists in the body) is permanently dead. If the non-atlas code path still
exists in the file, it should either be removed or gated with `#if` to avoid maintenance drift.

Do not remove the fallback contour/piece drawing unless atlas rendering has a separate runtime failure
fallback. The current code still falls back when atlas drawing returns false, so the issue is specifically
the compile-time `CHESS_ATLAS_ENABLED` branch, not all fallback drawing.

---

### 14. `CrownstepRangeMenuQuantity` Ownership

**File:** `CrownstepUI.cpp`

```cpp
rangeSlider->quantity = new CrownstepRangeMenuQuantity(module);
```

VCV Rack's `ui::Slider` owns and deletes its `quantity` pointer, so this is safe. However, the
pattern is easy to get wrong — a comment or `std::unique_ptr` handoff would document the intent.

Lowest-risk fix is a one-line comment at the assignment site. Do not convert this to `unique_ptr` unless
Rack's `Slider` API is checked first; it expects a raw `Quantity*`.

---

## Suggested Work Order

### Pass 1: Behavioral Bugs

1. Fix `stepCounterStyle` deserialization.
2. Add persistence coverage for `stepCounterStyle`.
3. Add `linearDiagonalRank()` and route layout case `3` to it in both checkers/shared and chess paths.
4. Add a layout regression test.
5. Run `make test-fast` and Crownstep object builds.

### Pass 2: Playback Lock Hygiene

1. Update `emitStepAtClockEdge()` to compute active range directly while holding the sequence lock.
2. Remove nested calls to public locking helpers from inside that locked section.
3. Keep pitch fallback behavior identical for old saves.
4. Run `make test-fast` and `build/src/CrownstepPlayback.cpp.o`.

### Pass 3: Small Cleanups

1. Replace string-mode checks with `gameMode` comparisons.
2. Clarify `requestAiSide` normalization.
3. Optimize Othello pass handling.
4. Optionally switch `moveAnimationQueue` to `std::deque`.

Do not combine Pass 1 with product/API changes such as adding a gate output.

---

## Summary Table

| # | Severity | File | Description |
|---|----------|------|-------------|
| 1 | **Bug** | Serialization.cpp | `stepCounterStyle` always written as `RIBBON`, never restored |
| 2 | **Bug** | Module.cpp/Core.hpp | Diagonal layout cases 3 and 6 call the same function |
| 3 | **Bug/Fragile** | Playback.cpp | Re-locking `recursive_mutex` inside `emitStepAtClockEdge` |
| 4 | Design | Shared.hpp | `Step::gate` populated and serialized but never output |
| 5 | Design | Core.hpp | 1600-line header; `static const SCALES` duplicated per TU |
| 6 | Design | Module.cpp | `isChessMode()` / `isOthelloMode()` use `strcmp` unnecessarily |
| 7 | Design | Module.cpp | `aiSide` sign normalisation is an identity through inequality |
| 8 | Design | Module.cpp | `moveAnimationQueue` front-erase is O(n); use `std::deque` |
| 9 | Perf | Core.hpp | `collectCapturesRecursive` copies vectors on every recursive call |
| 10 | Perf | Module.cpp | Othello search always generates both sides' moves at every node |
| 11 | Minor | Serialization.cpp | Seed-0 correction is silent; should at least assert in debug |
| 12 | Minor | Module.cpp | `boardCellCount()` cap on `board.size()` is coincidental |
| 13 | Minor | UI.cpp | `CHESS_ATLAS_ENABLED = true` — false branch is dead code |
| 14 | Minor | UI.cpp | `CrownstepRangeMenuQuantity` ownership relies on implicit convention |
