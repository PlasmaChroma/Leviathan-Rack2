# Nautiloid Improvement Plan

## Phase 1: Stabilize Current MVP

Goal: make the current CPU/threaded renderer predictable.

- Fix tiny movement propagation fully.
- Add debug counters for:
  - render request serial
  - display preview generation
  - Iris preview generation
  - stale render drops
  - display cache hits/misses
  - Iris-compatible render completions
- Add optional debug overlay or log mode for those counters.
- Keep SIMD disabled until visual behavior is stable.
- Confirm main display and mini Iris preview always update on release.

## Phase 2: Interaction Responsiveness

Goal: make pan/zoom feel immediate even when exact render lags.

- Reproject/crop the last display frame instantly during pan.
- Continue exact render in the foreground worker.
- Keep background cache warming independent.
- Tune pan request throttling separately for:
  - active drag
  - drag release
  - zoom-speed hold
- Consider a lower-resolution immediate display render while moving, then full resolution on settle.

## Phase 3: Better Display / Iris Split

Goal: treat Nautiloid display and Iris-compatible output as separate products.

- Main display stays display-aspect, currently `768x512`.
- Iris output stays `1024x256`.
- Add clear internal names:
  - `displaySource`
  - `displayCacheSource`
  - `irisSource`
- Add a future export path from `irisSource`.
- Decide whether the bottom Iris mini preview should show:
  - latest completed Iris source
  - pending/stale state indicator
  - generation mismatch indicator

## Phase 4: Cache Scheduling

Goal: replace one oversized cache with smarter background work.

- Keep current `1.5x` cache short term.
- Add pan-direction prediction.
- Recenter cache ahead of movement, not only at current center.
- Track cache coverage explicitly.
- If current view is inside cache, crop immediately.
- If near cache edge, queue a cache refresh before it becomes critical.

## Phase 5: Progressive Rendering

Goal: avoid all-or-nothing full frame renders.

- Split display render into tiles.
- Render center tiles first.
- Render visible edges next.
- Render overscan/predicted tiles last.
- Add quality tiers:
  - fast preview iteration count
  - normal display iteration count
  - settled/high-quality iteration count
- Replace stale tiles progressively instead of swapping whole frames only.

## Phase 6: Precision and Deep Zoom

Goal: support deeper zoom without flat/unstable output.

- Keep scalar double as baseline.
- Re-enable SIMD only after output parity is validated.
- Add zoom/formula-specific precision thresholds.
- Increase iteration count with zoom.
- Add anchor-relative coordinate math before considering perturbation.
- Treat perturbation/BLA as later research, not MVP work.

## Phase 7: GPU Preview Backend

Goal: make the display feel fluid without replacing the CPU source pipeline.

- Add optional GPU display-only renderer for Mandelbrot-family formulas.
- Keep CPU renderer authoritative for:
  - Iris-compatible output
  - export
  - deterministic fallback
- Avoid GPU readback during interaction.
- Use GPU preview as a visual layer, not as the source-of-truth initially.

## Recommended Next Step

Do Phase 1 first. Specifically: add render/debug counters and make tiny movement propagation observable. Right now we are guessing from visible behavior; a small telemetry pass will make the next fixes much less speculative.
