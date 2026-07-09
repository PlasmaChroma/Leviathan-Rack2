# Nautiloid Improvement Plan

## Current Status

Completed work:

- Removed the dedicated Eye of the World built-in mode; it is reachable as a Mandelbrot zoom target instead of a separate fractal selection
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
  - expanded cache currently covers `3x` the visible display area
  - cache tiles are currently `128x128`
  - cache worker fills visible-near tiles first, then outward
  - completed tiles are mirrored into a stitched cache presentation image for faster crop/downsample/reprojection sampling
  - a retained stitched presentation snapshot survives live cache resets so zoom-out can keep sampling the last useful wide image
  - foreground display uses full cached crops when the required crop is complete
  - foreground display can now publish partial cached crops over the previous completed frame while tiles are still filling
  - foreground display falls back to exact full-frame rendering when no useful cached coverage is available
  - tile cache applies to all built-in Nautiloid fractal modes, not only Mandelbrot
  - cache recentering can shift existing tiles when movement aligns to tile boundaries
  - the panel now includes a tile cache grid visualization showing current/stale tiles and visible crop position
  - tile lookup during display composition is now grid-indexed instead of a per-pixel linear tile scan
- Added display-only zoom reprojection:
  - keeps a separate authoritative display frame as the resampling base
  - publishes immediate bilinear-resampled zoom previews during zoom changes
  - has a three-level tiled zoom-ahead cache stack at approximately `zoom + 0.18`, `zoom + 0.36`, and `zoom + 0.54`
  - zoom-ahead layers are currently `768x512` at `1.35x` cache scale and display-only
  - zoom-ahead layers have stitched RGB buffers plus tile-valid masks, so partially completed layers can be sampled as soon as center/near tiles are available
  - samples the wider tile cache first when it can answer the requested world coordinate, then falls back to the display frame
  - tile-cache sampling now reads from the stitched presentation image while using tile validity as the mask
  - if the live tile cache has just reset, zoom reprojection can sample the retained stitched presentation snapshot before falling back to the display frame
  - display-frame fallback no longer edge-clamps outside the old display bounds, avoiding smeared borders when no wide cache can answer
  - avoids feeding reprojected frames into Iris/export/cache or back into the reprojection base
  - ignores pure pan requests to avoid reintroducing shifted-frame edge flicker
  - tile cache grid now draws the live view rectangle against the cache's own zoom, so zoom-out shows the view box growing
  - active zoom-out requests force a centered cache refresh once the live view consumes most of the cached span, and zoom release settles with a centered cache request
  - active zoom gestures split cheap preview/cache requests from full CPU display renders, so frequent zoom ticks can keep reprojection and zoom-ahead warming current without invalidating every in-flight CPU render
  - regular display tile-cache setup/reset/warming and Iris-compatible renders are deferred during active zoom because exact-zoom cache and Iris frames become stale immediately during continuous zoom
- Updated zoom UI:
  - active zoom-speed bar now uses center-out gradients, cyan for zoom-in and violet for zoom-out
- Added an experimental GPU preview path:
  - Dragon King-only context menu toggle
  - currently limited to the large Mandelbrot display preview
  - Mandelbrot CPU and GPU paths skip the main cardioid and period-2 bulb analytically before iterating
  - GPU Mandelbrot uses the same iteration budget as the CPU path, so preview geometry does not exceed the authoritative CPU/Iris classification
  - CPU rendering remains authoritative for Iris-compatible preview, expander handoff, cache, and export paths
  - Iris-compatible renders now run on a separate latest-only worker, so cache and zoom-ahead work cannot block Iris publication
  - CPU/NanoVG display remains the fallback if the GL shader is unavailable
  - full-quad GPU preview is allowed through the full zoom range for visual comparison
  - when GPU preview is active, available, and below roughly 68% of the zoom range, CPU display renders, display tile warming, and zoom-ahead display caches are skipped so Iris-compatible rendering can publish sooner
  - above roughly 68% zoom, GPU preview remains visible while CPU full-display rendering resumes for comparison/handoff; regular display tile-cache warming remains skipped because those exact-zoom tiles are not visible during GPU ownership
  - tiled double-single GPU precision mode was tried and removed because it was not an obvious improvement over the full-quad shader

Deferred or changed:

- Instant frame reprojection during pan was tried and removed because edge flicker felt worse than waiting for real data.
- The current tile cache is display-cache tiling, not yet a persistent infinite/world tile grid.
- Partial/progressive visible tile composition is enabled for display cache composites, but missing pixels currently reuse the previous display frame rather than rendering only the missing rectangles.
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
- Zoom is the exception: display-only resampling is now used as a temporary presentation layer until real tiles/full renders catch up.
- Zoom reprojection is handled by a latest-only presentation worker instead of being generated synchronously in the UI request path or waiting behind the main render worker.
- Zoom-in now has the first fixed-size multi-level predictive stack, but not yet a fully generic cache manager or GPU texture-backed compositor.

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
- Experimental GPU preview intentionally does not feed `irisSource` or the expander handoff.

## Phase 4: Cache Scheduling

Goal: replace one oversized cache with smarter background work.

- Status: first implementation complete.
- Kept current `1.5x` cache coverage short term.
- Added pan-direction prediction.
- Cache can aim slightly ahead of movement.
- Cache coverage is tracked by center/zoom/mode and visible crop coverage.
- If current view is inside complete cached tiles, crop immediately.
- If required tiles are partially available, foreground display can publish a partial cached composite over the previous display frame.
- If no useful cached coverage is available, foreground display falls back to exact rendering.
- Background cache now fills tiles instead of rendering one monolithic expanded cache image.
- Full caches can be recentred by tile-shifting existing storage instead of always discarding all cached tiles.

## Phase 5: Progressive Rendering

Goal: avoid all-or-nothing full frame renders.

- Status: partially complete.
- Split display cache render into tiles.
- Render visible-near tiles first.
- Render remaining cache tiles outward.
- Publish display cache composites progressively as background tiles complete.
- Add quality tiers:
  - fast preview iteration count
  - normal display iteration count
  - settled/high-quality iteration count
- Replace stale tiles progressively instead of swapping whole frames only.
- Current limitation: missing pixels in partial composites reuse the previous display frame; there is not yet a true missing-rectangle renderer or quality-tiered compositor.

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

- Status: first experiment added for Mandelbrot only.
- Add optional GPU display-only renderer for Mandelbrot-family formulas.
- Keep CPU renderer authoritative for:
  - Iris-compatible output
  - export
  - deterministic fallback
- Avoid GPU readback during interaction.
- Use GPU preview as a visual layer, not as the source-of-truth initially.

## Recommended Next Step

Move from cache plumbing to measurement and tuning:

- Use the new tile-specific overlay/log counters to tune behavior:
  - current tile count
  - full tile count
  - tile cache resets
  - tile cache shifts/reused tiles
  - tile render completions
  - tile render aborts
  - partial cache hits and composite publishes
- Split cache scheduling into visible tiles, Iris generation, then overscan tiles if Iris feels delayed by full cache sweeps.
- Tune tile size after observing behavior, likely comparing `128x128` against `192x192`.
- Consider a true missing-rectangle renderer if partial composites show too much stale-frame patching during fast movement.
