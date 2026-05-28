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
intended for case 3, or serpentine is correct for one but not both. This only affects Chess mode,
where the full 8×8 layout path is taken; for checkers, the path delegates to `CrownstepCore.hpp`'s
`sampledBoardValueForMove`. Worth verifying the intended distinction between the two.

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

---

### 13. `CHESS_ATLAS_ENABLED` Dead-Code Branch

**File:** `CrownstepUI.cpp`

```cpp
constexpr bool CHESS_ATLAS_ENABLED = true;
```

The `false` path (if it exists in the body) is permanently dead. If the non-atlas code path still
exists in the file, it should either be removed or gated with `#if` to avoid maintenance drift.

---

### 14. `CrownstepRangeMenuQuantity` Ownership

**File:** `CrownstepUI.cpp`

```cpp
rangeSlider->quantity = new CrownstepRangeMenuQuantity(module);
```

VCV Rack's `ui::Slider` owns and deletes its `quantity` pointer, so this is safe. However, the
pattern is easy to get wrong — a comment or `std::unique_ptr` handoff would document the intent.

---

## Summary Table

| # | Severity | File | Description |
|---|----------|------|-------------|
| 1 | **Bug** | Serialization.cpp | `stepCounterStyle` always written as `RIBBON`, never restored |
| 2 | **Bug** | Module.cpp | Chess layout cases 3 and 6 call the same function |
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
