# Nautiloid Improvement Plan

## Current Status

Completed work:

- Added the Nautiloid MVP as a visual-only module with:
  - large display-aspect fractal preview
  - bottom Iris-compatible `1024x256` mini preview
  - fractal selector
  - reset view button
  - spring-loaded bipolar zoom-speed slider
  - click-drag panning on fractal views
- Split display and Iris-compatible rendering:
  - main display render currently targets `768x512`
  - Iris-compatible render targets `1024x256`
  - both are generated off the audio thread
- Added a separate background cache worker:
  - renders the Iris-compatible preview as deferred work
  - fills the display cache independently from the immediate display render
- Added debug visibility:
  - on-panel counters for requests, display completions, cache hits/misses, cache completions, Iris completions, and stale drops
  - optional CSV logging to the Rack user folder at `Leviathan/Nautiloid/fractal-pipeline.csv`
  - file logging is hidden behind Dragon King debug mode
- Kept SIMD disabled while visual stability is being debugged.
- Added predictive cache centering during panning.
- Replaced the single expanded display cache bitmap with a first-pass tile cache:
  - expanded cache still covers `1.5x` the visible display area
  - cache tiles are currently `128x128`
  - cache worker fills visible-near tiles first, then outward
  - foreground display uses cached tiles only when the required crop is complete
  - foreground display falls back to exact full-frame rendering when tiles are missing
  - tile cache applies to all built-in Nautiloid fractal modes, not only Mandelbrot

Deferred or changed:

- Instant frame reprojection during pan was tried and removed because edge flicker felt worse than waiting for real data.
- The current tile cache is display-cache tiling, not yet a persistent infinite/world tile grid.
- Partial/progressive visible tile composition is not enabled yet; the user still sees complete frames only.
- SIMD remains off pending scalar visual validation.

## Phase 1: Stabilize Current MVP

Goal: make the current CPU/threaded renderer predictable.

- Status: mostly complete.
- Fixed tiny movement propagation enough that small adjustments now reach the deferred Iris preview in observed testing.
- Added debug counters for:
  - render request serial
  - display preview generation
  - Iris preview generation
  - stale render drops
  - display cache hits/misses
  - Iris-compatible render completions
- Added optional debug overlay and file log mode for those counters.
- Kept SIMD disabled until visual behavior is stable.
- Confirmed main display and mini Iris preview are updating in current testing.

## Phase 2: Interaction Responsiveness

Goal: make pan/zoom feel immediate even when exact render lags.

- Status: revised.
- Reproject/crop the last display frame instantly during pan. Tried and removed due to edge flicker.
- Continue exact render in the foreground worker.
- Keep background cache warming independent.
- Tune pan request throttling separately for:
  - active drag
  - drag release
  - zoom-speed hold
- Consider a lower-resolution immediate display render while moving, then full resolution on settle.
- Current direction: prefer real rendered frames plus better background cache coverage over fake shifted-frame preview.

## Phase 3: Better Display / Iris Split

Goal: treat Nautiloid display and Iris-compatible output as separate products.

- Status: partially complete.
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
- Current implementation has separate display and Iris-compatible buffers, but naming can still be cleaned up.

## Phase 4: Cache Scheduling

Goal: replace one oversized cache with smarter background work.

- Status: first implementation complete.
- Kept current `1.5x` cache coverage short term.
- Added pan-direction prediction.
- Cache can aim slightly ahead of movement.
- Cache coverage is tracked by center/zoom/mode and visible crop coverage.
- If current view is inside complete cached tiles, crop immediately.
- If required tiles are missing, foreground display falls back to exact rendering.
- Background cache now fills tiles instead of rendering one monolithic expanded cache image.

## Phase 5: Progressive Rendering

Goal: avoid all-or-nothing full frame renders.

- Status: partially started.
- Split display cache render into tiles.
- Render visible-near tiles first.
- Render remaining cache tiles outward.
- Add quality tiers:
  - fast preview iteration count
  - normal display iteration count
  - settled/high-quality iteration count
- Replace stale tiles progressively instead of swapping whole frames only.
- Not yet done: visible progressive tile composition. The display still swaps complete frames.

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

Move from cache plumbing to measurement and tuning:

- Add tile-specific counters to the overlay/log:
  - current tile count
  - full tile count
  - tile cache resets
  - tile render completions
  - tile render aborts
- Separate foreground stale display drops from background cache aborts.
- Tune tile size after observing behavior, likely comparing `128x128` against `192x192`.
- Consider visible progressive tile composition only if full-frame fallback still feels too slow.
