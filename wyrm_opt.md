# Wyrm Rendering Optimization Plan

Last updated: 2026-05-14

## Purpose

Wyrm is still expensive compared with modules like Bifurx because the waveform editor is an always-redrawn, immediate-mode visual instrument. Sand optimization helped, especially with the OpenGL texture backend, but `Ed us` remains meaningful even when `Sand View` is off. This document covers the broader Wyrm render path: editor redraw policy, body rendering, waveform columns, rocks, hover overlays, sand update/draw, and telemetry.

The goal is to make Wyrm visually rich without making every visible module pay a full editor redraw cost every frame.

## Current State

Primary files:

- `src/WyrmWaveEditor.cpp`
- `src/WyrmSand.cpp`
- `src/WyrmSandGL.cpp`
- `src/WyrmWidget.cpp`
- `src/Wyrm.hpp`

Current debug terminal fields:

```text
UI ms     overall UI frame metric sent by Wyrm editor
Ed us     WyrmWaveEditor::draw() total editor work
SUp us    CPU sand simulation/update
SDr us    NanoVG sand draw path
SGL us    OpenGL sand texture upload/draw path
Aud us    sampled audio process time
Ch        channel count
Body      body sample count
```

Recent work already landed:

- `Sand -> Backend` exposes `NanoVG Image`, `NanoVG Cells`, `OpenGL Texture`, and `Shader Feedback`.
- `OpenGL Texture` backend moves sand draw into `WyrmSandGlWidget`.
- `SGL us` telemetry was added.
- `WT` and `Pts` were removed from debug terminal output.
- Sand CPU update now tracks active cells instead of always decaying the full grid.
- Sand disturbance uses detail-based stride, motion gating, and idle throttling.
- Low/medium sand sparkle was boosted so lower detail modes retain more of high-detail character.
- Non-sand waveform columns are batched into one normal path plus one hot-column path.
- Wyrm body sample points are cached once per editor draw and reused across the three NanoVG body strokes.

## Why Wyrm Is Still Expensive

### 1. Editor Draw Is Always Live

`WyrmWaveEditor::draw()` runs the editor path every visible frame:

- advance visual slither phase
- update sand
- draw sand or flat background
- draw midline
- compute hover state
- draw hover guides
- draw waveform columns when sand is off
- sample body path
- emit three NanoVG body strokes
- draw rocks
- draw drag arrows and mode label
- submit telemetry

Bifurx is cheaper partly because it uses `FramebufferWidget` and explicit dirtying: static visuals stop redrawing once their state has converged.

### 2. Body Rendering Is CPU Prep Plus Three Large NanoVG Strokes

The body path currently samples up to 768 centerline points. For 128 waveform points, the default body sample count is 512. That sampled path is emitted three times with different stroke widths/colors.

Even after caching sampled points inside one draw, NanoVG still receives three large paths every redraw.

### 3. Rock Avoidance Is Expensive Per Body Sample

Each body sample can do:

- Catmull interpolation over wave points
- rock boundary resolution for the base value
- slither offset
- rock boundary resolution for the slithered value
- rock-near checks for rounded-corner behavior

With rocks enabled and high body sample count, this math is a major part of non-sand `Ed us`.

### 4. Sand GL Only Moves One Layer

The OpenGL sand backend reduces `SDr us`, and overall `UI ms` improves, but the Wyrm body/editor still uses NanoVG. `Ed us` remains the cost of the non-sand editor drawing and math.

### 5. Sand Update Is Still CPU

`SUp us` is lower after active-cell and stride optimizations, especially at low detail. High detail still does CPU disturbance over many path/cell neighborhoods. GPU feedback simulation is possible later, but it is not the first broader Wyrm bottleneck to attack.

## Design Direction

Use Bifurx as the structural model:

```text
1. Separate state prep from drawing.
2. Cache expensive geometry/model state.
3. Dirty/redraw only when visible state changes.
4. Treat OpenGL as an optional acceleration backend.
5. Keep NanoVG fallback behavior stable.
```

Do not make the audio/DSP path part of this work. Wyrm render optimization should stay UI-only.

## Priority Work

### Phase 1: Better Telemetry

`Ed us` is too broad. Add temporary or permanent Wyrm sub-metrics:

```text
BodyPrep us
BodyDraw us
Cols us
Rocks us
Hover us
SandTotal us = SDr us + SGL us
```

Reasoning:

- `Ed us` currently hides whether cost is body math, NanoVG stroke emission, columns, rocks, or hover overlays.
- A short telemetry pass prevents optimizing the wrong layer.
- `SandTotal us` makes backend-neutral sand cost obvious.

Suggested debug terminal columns after cleanup:

```text
UI ms
Ed us
BodyP us
BodyD us
SUp us
Sand us
Aud us
Ch
Body
```

Keep column count reasonable. Extra columns can be temporary while profiling.

### Phase 2: Framebuffer Dirty Gating

Wrap the editor in a `FramebufferWidget`, similar to Bifurx spectrum widgets.

Dirty when:

- wave version changes
- point count changes
- rock state changes
- mouse hover enters/leaves or hover column/rock changes
- point editing or rock dragging is active
- `Sand View`, backend, detail, or persistence changes
- sand has active cells or visible decay
- slither amount is nonzero and visual phase advances
- editor size/zoom changes

Expected result:

- With sand off, slither off, and no interaction, editor redraw cost should mostly disappear.
- With sand on but settled, redraws should stop or become much less frequent.

Important implementation detail:

- Keep input handling on the editor widget.
- The framebuffer child should contain only the expensive visual layer, or the editor itself should own a framebuffer pointer and mark it dirty.
- Avoid masking bugs by always dirtying every frame.

### Phase 3: Persistent Body Geometry Cache

The current body cache only lasts for one `draw()` call. Promote it to member state.

Cache key should include:

```text
box size
point count
wave version
rock state/version
visual slither phase or slither inactive flag
slither amount
body sample count
rock clearance values
```

When slither is off and no editing/rock motion is happening, body prep should be near zero.

Consider adding explicit versions:

- `rockVersion`
- `editorVisualVersion`
- `sandVisualVersion`

These make dirty/cache invalidation much clearer than inferring from fields.

### Phase 4: Body Sample Count Policy

Current policy:

```cpp
bodySampleCount = max(pointCount, min(768, max(128, pointCount * 4)))
```

Options:

- Use `pointCount * 2` when rocks are off.
- Use `pointCount * 2` when slither is off and not editing.
- Use `pointCount * 4` only when rocks/slither/editing need smoother collision visuals.
- Reduce samples at low Rack zoom.
- Keep a high-quality mode for visual QA/debug if needed.

Risk:

- This can visibly change body smoothness. It needs manual visual comparison on sine, square, supersaw, heavy rocks, and high slither.

### Phase 5: OpenGL Body Renderer

The largest rendering win is likely moving the Wyrm body itself to OpenGL.

Do not use GL lines. Line width behavior is inconsistent across platforms and scaling. Use generated triangle geometry:

```text
sample centerline
compute normals
emit center +/- normal * halfWidth
draw GL_TRIANGLE_STRIP
repeat for the current three body layers
optionally add feather strips later
```

The OpenGL body renderer should consume the same CPU-generated body centerline as NanoVG at first. That avoids changing waveform, slither, and rock semantics while moving draw cost away from NanoVG.

Suggested implementation:

- Add a `WyrmBodyGlWidget` or extend the existing GL visual widget once sand/body ownership is clear.
- Keep NanoVG editor overlays: hover guides, rocks, drag arrows, editing feedback.
- Use the GL body only as a backend for the body strokes.
- Keep NanoVG fallback.

Open questions:

- Whether GL body should be part of `OpenGL Texture` sand mode or a separate `Body Backend` setting.
- Whether body GL should be enabled automatically when sand GL is active.

### Phase 6: Waveform Column Cache Or GL Batch

Waveform columns only draw when sand is off. They are now batched in NanoVG, but still CPU/NanoVG.

Options:

- Cache column path/image and rebuild only on wave/size/hover changes.
- Draw columns as a GL quad batch.
- Hide or simplify columns when body rendering already provides enough waveform readability.

This is lower priority than body rendering unless telemetry shows columns are still material.

### Phase 7: Sand Update GPU Feedback

GPU sand simulation is possible:

```text
sandA texture
sandB texture
feedback shader applies decay/diffusion/disturbance
swap textures
composite texture behind editor
```

This could reduce `SUp us`, but it adds GL lifecycle and fallback complexity. It should come after editor dirty gating and body render work unless `SUp us` remains the dominant measured cost.

## Concrete Near-Term Sequence

1. Add detailed Wyrm render telemetry.
2. Use that telemetry to confirm body prep/draw vs columns/rocks/sand.
3. Add framebuffer dirty gating for the editor.
4. Promote body geometry to a persistent cache.
5. Tune body sample count policy.
6. Prototype GL triangle-strip body renderer.
7. Revisit GPU sand feedback only if `SUp us` remains high after the above.

## Acceptance Criteria

- With `Sand View` off, idle Wyrm editor cost is much closer to Bifurx idle behavior.
- With `Sand View` on and OpenGL backend active, `SDr us` remains near zero and `SGL us` is visible in telemetry.
- `Ed us` drops significantly in static or low-motion cases.
- Body visual output remains recognizable and stable across sine, square, supersaw, rocks, and slither.
- Editing points and dragging/lifting rocks remain responsive.
- NanoVG fallback remains available.
- No audio/DSP behavior changes.

## Risks

- Framebuffer dirty gating can hide stale visual state if invalidation is incomplete.
- Body geometry caching can go stale unless versioning is explicit.
- Lower body sample counts can create visible faceting around rocks or sharp waveforms.
- GL triangle strips need careful normal joins to avoid spikes at sharp corners.
- GPU sand feedback can be fragile in Rack/OpenGL lifecycle paths.

## Notes

The current Wyrm sand work should remain in `sand_opt.md`. This document is broader: it covers the full editor renderer and should guide work once sand is no longer the only visible bottleneck.
