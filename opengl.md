# TDScope OpenGL Progress

## Current status

`TDScope` now has three surviving debug render modes:

- `Standard`
- `Tail raster`
- `OpenGL`

The old intermediate experiments were removed from the menu and remapped on load so old patches still land on valid surviving modes.

## What OpenGL currently is

The current `OpenGL` mode is the former `OpenGL geometry + history` path:

- geometry history is enabled
- history storage uses a ring head (`historyHeadRow`) instead of shifting arrays
- GL line submission is batched into width bins
- batch storage is persistent on the widget
- transient/color-drive updates are now limited to changed rows plus a small neighbor band in the history path
- halo brightness/compositing was adjusted toward the NanoVG/raster look
  - additive halo blending
  - modest gain increase on main trace
  - modest gain increase on connectors

## Current performance read

Observed behavior from benchmarking:

- `OpenGL` is stable during scratch/drag and avoids the larger interaction spikes seen in `Tail raster`
- `Tail raster` is still slightly faster in passive forward playback
- after recent fixes, `OpenGL` is around `0.50 ms` in the tested case, but still does not clearly beat `Tail raster` in the passive case

So the situation is:

- `OpenGL` is the better interaction path
- `Tail raster` is still the passive-play winner
- we are still trying to determine whether `OpenGL` can become the single primary renderer

## Important recent fixes

- Fixed a regression where the ring-head optimization was applied to the wrong path instead of the GL widget
- Reduced GL batch building from repeated per-bin rescans to single-pass bin fills
- Made GL batch vectors persistent widget-owned storage
- Limited transient/color-drive recompute to changed rows plus neighbors in the history-backed GL path
- Renamed the surviving GL mode in the menu to just `OpenGL`
- Pruned obsolete render modes from the menu and enum

## Remaining performance still on the table

These are the biggest known remaining costs in `OpenGL`:

1. Visible-slice copy from history ring

- We still copy the visible slice out of the history ring into `rowX0`, `rowX1`, `rowVisualIntensity`, etc. before drawing.
- This is pure overhead if the renderer can draw directly from the ring-backed history storage.

2. Per-frame batch content rebuild

- Batch storage is persistent now, but the line vertices are still rebuilt every frame.
- We are not yet updating only the changed band for halo/main/connectors.

3. Client arrays instead of VBOs

- The GL path still uses client-array submission.
- Persistent VBOs are still available as a next step if CPU-side cleanup is not enough.

4. Connector regeneration

- Connectors are still regenerated row-by-row every frame.
- They may still be an avoidable steady-state cost.

## Recommended next steps

### Next step 1: draw directly from history

Goal:

- remove the copy from the history ring into `row*` visible arrays before draw

Why:

- this is likely the next clean passive-play cost reduction
- it simplifies the relationship between cached geometry and drawn geometry

Approach:

- let the GL draw path address visible rows by logical row index
- resolve history slots on demand from `historyHeadRow + historyMarginRows + iy`
- keep `rowY` or derive visible y positions separately

### Next step 2: partial batch rebuild

Goal:

- update GL line batches only for changed rows instead of rebuilding all batches every frame

Why:

- the exposed-band update logic already exists on the geometry side
- we should carry that same incremental principle into the GL batch content

Approach:

- track the visible changed band after history shift/rebuild
- rebuild halo/main/connectors batch content only for that band
- if needed, use a small expansion around the band for connector continuity

### Next step 3: VBO submission

Goal:

- replace client arrays with persistent VBO-backed submission

Why:

- this is still a real remaining GL-side optimization
- it should only be attempted after the CPU-side history/batch work is reduced further

Approach:

- keep the current bin structure
- upload batched line vertices into persistent GL buffers
- issue draw calls from those buffers instead of client pointers

## Brightness matching status

OpenGL brightness was visibly dimmer than the raster/NanoVG path.

Current corrective actions already applied:

- additive halo blending
- boosted halo alpha
- boosted main trace alpha/lift
- boosted connector alpha/lift

Next brightness work, if still needed:

- tune only the gain constants first
- avoid changing renderer structure again just for visual matching

## Architectural note

`src/TDScope.cpp` has grown too large and should be split soon.

Recommended future split:

- `TDScopeModule.cpp`
- `TDScopeWidget.cpp`
- `TDScopeRenderShared.hpp/.cpp`
- `TDScopeRenderNanoVG.cpp`
- `TDScopeRenderGL.cpp`

Key rule:

- keep one shared data-prep/history layer
- keep renderer backends separate

## Decision point to watch

The main question is still:

- can `OpenGL` overtake `Tail raster` in passive forward playback while keeping its superior scratch consistency?

If yes:

- adopt `OpenGL` as the primary renderer

If not:

- keep both renderers in mind as a deliberate split between passive-play efficiency and interaction stability
