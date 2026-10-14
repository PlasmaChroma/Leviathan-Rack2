# Crownstep AI: Fast Moves & Search Optimization

This document outlines technical recommendations for optimizing the move think timing and search efficiency of the Crownstep AI (Chess, Checkers, and Othello).

## 1. Search Algorithm Improvements

### Iterative Deepening & Time Management
Currently, the AI searches to a fixed depth based on difficulty. This leads to inconsistent thinking times (very fast in endgames, potentially slow in complex midgames).
- **Recommendation:** Implement Iterative Deepening. Search depth 1, then 2, then 3, etc., until a time budget is reached.
- **Benefit:** Allows the AI to always use its full allotted time and provides a "best move so far" if it needs to exit early. Difficulty can be mapped to milliseconds (e.g., Easy = 50ms, Hard = 1000ms) rather than static depth.

### Quiescence Search
The "horizon effect" causes the AI to make poor moves because it can't see a capture that happens just one ply beyond its fixed limit.
- **Recommendation:** At the end of the regular search, enter a "Quiescence Search" that only evaluates captures and promotions until the position is "quiet" (no more immediate tactical trades).
- **Benefit:** Prevents the AI from "blundering" into trades it thinks are safe because the search stopped mid-sequence.

### Transposition Tables (Zobrist Hashing)
The search often visits the same board positions via different move orders (transpositions).
- **Recommendation:** Implement a small Transposition Table (hash map) using Zobrist hashing. Store the evaluation and search depth for previously seen positions.
- **Benefit:** Massive speedup in mid-to-late game where transpositions are frequent.

---

## 2. Move Generation Bottlenecks

### Mobility Evaluation Overhead
In `chessEvaluatePosition`, the AI currently calls `chessGenerateLegalMovesForSide` for *both* players to calculate mobility scores.
- **Current Cost:** Every leaf node generates all legal moves twice.
- **Recommendation:** Remove full legal move generation from the evaluation function. Use pseudo-legal move counts as a proxy for mobility, or only calculate full mobility at the root node.

### In-place Board Updates (Make/Unmake)
The current search copies the entire `BoardState` array (64 ints) and `ChessState` struct at every node.
- **Recommendation:** Transition to a "MakeMove / UnmakeMove" pattern where the board is modified in-place and then reverted.
- **Benefit:** Reduces memory pressure and cache misses significantly.

### Bitboards (Long-term)
The current coordinate-based move generation is intuitive but slow.
- **Recommendation:** Use `uint64_t` bitboards to represent piece positions.
- **Benefit:** Move generation and king safety checks become bitwise operations, potentially speeding up the engine by 10x or more.

---

## 3. Alpha-Beta Pruning Efficiency

### Enhanced Move Ordering
Alpha-beta pruning is most effective when the best moves are searched first.
- **Recommendation:** Add the **Killer Heuristic** (store moves that caused a cutoff at the same depth in other branches) and the **History Heuristic** (score moves based on how often they cause cutoffs globally).
- **Benefit:** Causes more frequent cutoffs, allowing the search to reach significantly deeper plies in the same amount of time.

### Null Move Pruning
In many chess positions, "passing" your turn would be a disadvantage.
- **Recommendation:** If the current side can pass their turn and still have a score above beta (at a reduced depth search), prune the branch.
- **Benefit:** Quickly discards "dead" branches where one side is overwhelmingly winning.

---

## 4. UX & Perceived Timing

### Instant Move on Forced Positions
If there is only one legal move (e.g., forced evasion of check), the AI currently still "thinks" for the minimum delay.
- **Recommendation:** Detect single-move scenarios and bypass the search/delay entirely.

### Difficulty-Based Time Budgets
Instead of `AI_TURN_DELAY_SECONDS = 0.5f`, use a dynamic range:
- **Too young to die:** 100ms budget, max depth 2.
- **Ultra violence:** 2000ms budget, iterative deepening.

### Pondering (Background Thinking)
- **Recommendation:** Start the worker thread searching for the AI's best response as soon as the human's turn begins, based on the most likely human moves.
- **Benefit:** The AI will often have its move ready the instant the human finishes.
