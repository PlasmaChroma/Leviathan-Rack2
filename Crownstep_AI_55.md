# Crownstep AI 55 Notes

Focus: Crownstep chess AI at the highest difficulty (`Ultra violence`, depth 4) is exceeding the 500 ms minimum move time. These notes are source-review based, centered on keeping playing strength while reducing wall time.

## Current Shape

- AI turn delay is `AI_TURN_DELAY_SECONDS = 0.5f` in `src/CrownstepShared.hpp`. The worker starts as soon as that delay window begins, so any move that still lands late is search time exceeding the hidden 500 ms budget, not a scheduling issue.
- Runtime move selection uses `chooseChessMoveForSide()` in `src/CrownstepModule.cpp`, not the older inline `chessChooseAiMove()` copy in `src/CrownstepCore.hpp`.
- Highest chess difficulty maps to depth 4 via `chessSearchDepthForDifficulty()` in `src/CrownstepCore.hpp`.
- Search is plain negamax alpha-beta. Each interior node calls `chessGenerateLegalMovesForSide()`, sorts moves, applies each child, and recurses.
- Static evaluation calls `chessGenerateLegalMovesForSide()` for both sides to score mobility, then checks mate/stalemate through those legal move lists.

## Main Cost Drivers

1. Leaf evaluation is much heavier than it looks.

At `depth <= 0`, `chessSearchForSide()` calls `chessEvaluateForSide()`, which calls `chessEvaluatePosition()`. That evaluation generates full legal move lists for both AI and human:

```cpp
std::vector<Move> aiMoves = chessGenerateLegalMovesForSide(board, AI_SIDE, state);
std::vector<Move> humanMoves = chessGenerateLegalMovesForSide(board, HUMAN_SIDE, state);
```

Full legal move generation is expensive because every pseudo move is applied to a copied board and then checked with `chessIsKingInCheck()`. At depth 4, most visited nodes are leaves, so this likely dominates total time.

Recommendation: split evaluation into `chessEvaluateStaticFast()` and terminal handling.

- At ordinary leaves, use material plus cheap pseudo-mobility or piece-square terms.
- Only generate legal moves for terminal detection in the search node where `moves.empty()` is already known.
- If mobility is important to strength, use pseudo-legal mobility counts that skip the apply/check loop, or count attacks with lightweight piece scans.

Expected impact: high. This preserves most strength because legal mobility is a small weighted term (`* 4`) but is currently paid with full legal generation twice per leaf.

2. Root search does not carry alpha between candidate moves.

`chooseChessMoveForSide()` evaluates each root move with a full open window:

```cpp
int score = -chessSearchForSide(nextBoard, nextState, -aiSide, aiSide, depth - 1, -INF, INF);
```

After the first good move, later root moves should search with the current best score as alpha. In negamax form, that means calling children with `-beta, -alpha`, where root `alpha` is updated after each completed move.

Recommendation: maintain `rootAlpha = bestScore` across the root loop:

- Initialize `alpha = -INF`, `beta = INF`.
- For each root move, call child with `-beta, -alpha`.
- If the score beats best, update `bestScore`, `bestIndex`, and `alpha`.

Expected impact: medium to high, especially when move ordering puts captures/promotions first. No strength loss.

3. Move allocation is expensive for chess.

`Move` contains `std::vector<int> path` and `std::vector<int> captured`. Chess moves only need one destination and usually zero or one captured square, but every generated move currently pushes into vectors. Legal generation also creates separate `pseudo` and `legal` vectors per node.

Recommendation options:

- Low-risk: reserve likely capacities inside chess move generation (`pseudo.reserve(64)`, `legal.reserve(64)`) and avoid repeated reallocations.
- Better: add a compact chess-only move struct for search, with fixed fields like `origin`, `destination`, `capturedIndex`, flags, and promotion type.
- Middle ground: keep public `Move`, but add internal `ChessSearchMove` and convert only the final selected move back to `Move`.

Expected impact: medium. This reduces allocator pressure and memory traffic without changing search semantics.

4. Sorting recomputes move ordering scores repeatedly.

`chessSortMovesForSearch()` uses `std::stable_sort()` with a comparator that calls `chessMoveOrderingScore()` for both sides of every comparison. For small move lists this is not catastrophic, but at depth 4 it becomes repeated work at every node.

Recommendation: precompute ordering scores once per move before sorting.

- Use a small vector of `{Move, score}` or add a local scored wrapper.
- Use non-stable `std::sort()` with deterministic tie-breakers already present.
- Later, feed a best move from a transposition table to the front before MVV/LVA.

Expected impact: low to medium by itself, useful when combined with better move ordering.

5. No transposition table.

Chess positions can be reached by different move orders, and alpha-beta benefits heavily from remembering searched positions. The current board is a compact 64-cell array plus small `ChessState`, so a Zobrist key is straightforward.

Recommendation: add a small fixed-size transposition table scoped to a single AI search.

- Key should include board pieces, side to move, castling rights, and en passant file/index.
- Store depth, score, bound type (`exact`, `lower`, `upper`), and best move.
- Use the TT best move first in ordering.
- Keep it worker-local or stack-owned in the search context, so there is no audio-thread or UI-thread sharing concern.

Expected impact: medium. The best-move ordering often matters as much as direct cache hits.

## Best First Implementation Sequence

1. Add a search context and node counters.

Create a lightweight `ChessSearchContext` used only by `chooseChessMoveForSide()` / `chessSearchForSide()`. Track nodes, leaf nodes, legal move generations, eval calls, and elapsed time. This gives before/after numbers without changing behavior.

2. Replace leaf eval legal mobility with cheap eval.

Keep the current mate/stalemate behavior inside search when `moves.empty()`. At `depth <= 0`, avoid full legal move generation. Start with material plus pseudo-mobility, then tune if strength feels lower.

3. Carry root alpha.

This is a small correctness-preserving alpha-beta improvement and should be done early.

4. Precompute move-order scores.

Cheap, localized cleanup. It also makes future TT best-move ordering easier.

5. Add a small search-local transposition table.

After the leaf eval and root alpha wins are measured, TT is the next meaningful step if depth 4 still misses the 500 ms target.

## Strength Notes

- Removing full legal mobility from leaf eval sounds risky, but its current weight is only `4` points per move, while material values are 100+ and mate is 100000. A pseudo-mobility approximation should keep the same positional flavor at far lower cost.
- Root alpha and precomputed ordering are pure efficiency wins.
- A transposition table usually improves both speed and strength because it improves ordering and avoids repeated work.
- If timing is still inconsistent after these changes, add iterative deepening with a soft time budget as a fallback. Let the engine complete depth 4 when possible, but keep the last completed depth-3 result ready if the search is clearly going past budget. That does trade some worst-case strength for responsiveness, so it should come after the no-strength-loss optimizations.

## Places To Touch

- `src/CrownstepModule.cpp`: active worker-side chess selector and search.
- `src/CrownstepCore.hpp`: chess move generation, static evaluation, move ordering, depth mapping, and older inline search helpers.
- `tests/crownstep_spec.cpp`: add regression tests for selected tactical positions after changing evaluation/search.

## Validation Plan

- Add a microbenchmark or debug counter path for depth-4 chess from the initial position and a few midgame positions.
- Compare chosen moves before/after for a small fixed suite. Exact identity is not required after eval changes, but obvious tactical wins should remain.
- Run `make test-fast` in this repo context. Per repo note, avoid treating full plugin link failures as authoritative in WSL-like environments.

## 2026-04-28 First Pass Results

Implemented:

- Search counters for active worker-side chess search: nodes, evals, legal move generations, and cutoffs are now captured with the AI result.
- Root alpha carry in active module search and header-only chess search.
- Fast leaf evaluation using material plus pseudo-mobility instead of full legal move generation for both sides.

Validation:

- Forced rebuild and run of `build/tests/crownstep_spec`: 20/20 passed.
- `make test-fast`: passed.
- `make build/src/CrownstepModule.cpp.o`: passed.

Temporary depth-4 timing harness results, comparing copied old search shape against updated header search:

| Position | Old | New | Speedup | Move Match |
| --- | ---: | ---: | ---: | --- |
| Initial board | 395.795 ms | 74.209 ms | 5.33x | yes |
| Italian-ish opening sample | 865.860 ms | 65.883 ms | 13.14x | no |
| Reduced tactical sample | 117.967 ms | 2.377 ms | 49.63x | yes |

The non-matching Italian-ish move is expected risk from changing the leaf evaluator: full legal mobility was replaced by pseudo-mobility. It is not automatically a regression, but it should be checked against a small tactical/positional suite before deciding whether to tune pseudo-mobility or add piece-square terms.

## 2026-04-28 Safe Second Pass

Implemented:

- `chessSortMovesForSearch()` now precomputes each move-order score once before sorting instead of recomputing scores inside every comparator call.
- Added a small depth-4 chess AI sample suite to `tests/crownstep_spec.cpp`.

Sample suite coverage:

- Initial chess position, expected current move `12->20`.
- Italian-ish opening position, validating selected move is legal and recording the current choice.
- Reduced tactical position, expected current move `27->24`.

Validation result:

- Forced rebuild and run of `build/tests/crownstep_spec`: 21/21 passed.
- Sample suite result on this machine: `initial=12->20/legal italian-ish=3->21/legal reduced tactical=27->24/legal elapsedMs=143.443`.

This pass is intended as a stability and measurement base before higher-risk changes like a transposition table or evaluator tuning.
