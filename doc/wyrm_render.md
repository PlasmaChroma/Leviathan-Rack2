# Wyrm Renderer Implementation Plan

## Status and scope

Last reviewed: 2026-08-17, after removal of the animated sand-field system.

This plan covers Wyrm's waveform fill, body, editor compositing, and renderer
instrumentation. The static background field is an independent cached layer and
is not part of this optimization. A future procedural/fractal background should
be designed separately and measured against that cached baseline.

The audio path, authored waveform data, envelope behavior, rock interaction,
patch serialization, and renderer selection must remain unchanged.

## Current baseline

```text
WyrmEditorSurface
  cached NanoVG background framebuffer
    static sand_color_96c field treatment
  OpenGL framebuffer
    waveform fill and body in OPENGL / OPENGL_SHDR
  cached NanoVG editor framebuffer
    waveform fill and body in NANOVG
    rocks and editor interaction graphics in every mode
  live NanoVG overlay
    tracer and envelope progress
```

The GL and NanoVG editor paths share `DisplayGeometryCache`. Slither changes the
display curve every frame, but the cached NanoVG editor is currently also dirtied
every frame in GL modes. The static background framebuffer is independent and is
only dirtied on size changes.

The remaining significant costs and divergences are:

- GL Slither needlessly redraws the NanoVG editor framebuffer.
- Hover changes and drag decoration also redraw the cached editor layer.
- `DisplayGeometryCache` always computes `nearRock`, though GL only uses points.
- SHDR builds three client-side strips and submits three body draws.
- GL waveform fill uses a full-editor CPU-generated texture plus immediate-mode
  quads for every authored point.
- NanoVG and GL body materials use different widths and alpha values.
- The source retains an unused body render-target allocation path.
- GL timing measures CPU submission, not completed GPU work.

## Design rules

1. Land independently measurable changes with a working fallback after each one.
2. Keep the static background, cached editor art, and live interaction layers
   separate.
3. Keep one canonical CPU display curve until profiling justifies moving curve
   generation to the GPU.
4. Define visual materials once and let each renderer implement the same design.
5. Remove redundant work before adding VBOs or more elaborate GPU architecture.
6. Preserve OpenGL compatibility and GLSL 1.20 support.

## Phase 0: Baseline and dead-code cleanup

### Work

- Capture collapsed and expanded views in NanoVG, OpenGL, and SHDR modes.
- Cover oscillator/envelope, Slither on/off, no rocks, and sharp rock bends.
- Record editor and GL redraw frequency, geometry-cache hits/misses, body sample
  count, editor draw time, and GL CPU submission time.
- Remove the unused body render-target fields, allocation helper, validation, and
  cleanup branches from `WyrmRendererGL.cpp`.
- Verify the renamed whole-renderer `perfWyrmGlUs` telemetry end to end.

### Acceptance

- Baseline captures and measurements exist before visual changes.
- No unused body FBO or texture is allocated.
- NanoVG, OpenGL, and SHDR selection and fallback still work.

## Phase 1: Stop unnecessary NanoVG redraws

### Work

- In `WyrmWaveEditor::step()`, dirty for Slither only in NanoVG mode.
- Continue dirtying for wave edits, rock-state changes, size and renderer changes,
  lock state, envelope mode, and active point/rock edits.
- Move hover strokes, drag arrows, and other transient interaction decoration to
  `WyrmEditorAnimationOverlay` (or a dedicated live overlay).
- Keep base rock shapes in the cached editor framebuffer.
- Add redraw counters if the existing timing cannot prove the result directly.

### Acceptance

- In GL modes, Slither redraws GL but not the cached NanoVG editor.
- Pointer movement across rocks does not redraw the cached editor.
- Hover, dragging, tracer, and envelope progress remain responsive.
- NanoVG mode continues to animate correctly.

## Phase 2: Lean shared geometry and canonical material

### Work

- Let `DisplayGeometryCache` satisfy either `PointsOnly` or
  `PointsAndNearRock` requests.
- Store near-rock validity separately so a NanoVG request can upgrade cached
  points without rebuilding them.
- Request points only from GL and near-rock metadata from NanoVG.
- Add separate counters/timing for point rebuilds and metadata upgrades.
- Introduce a renderer-owned body material containing the three widths, colors,
  alpha values, and SHDR edge softness.
- Use that material in NanoVG, OpenGL, and SHDR; renderer-specific AA remains an
  implementation detail.

### Acceptance

- GL performs no standalone near-rock scan.
- NanoVG retains its rounded-versus-hard bends around rocks.
- Renderer switching cannot expose stale metadata.
- Remaining visual differences are caused by path construction or AA, not
  different material constants.

## Phase 3: One-strip analytical SHDR body

### Design

Build only the widest strip, with a signed lateral coordinate at each vertex:

```cpp
struct BodyStripVertex {
    float x;
    float y;
    float side; // -1 at one edge, +1 at the other
};
```

The fragment shader uses `abs(side)` to generate the outer, middle, highlight,
and edge-AA coverage from the canonical material.

### Work

- Replace the three cached SHDR vertex vectors with one widest-strip vector.
- Compute joins at the actual outer width; remove the incorrect assumption that
  the current width-constrained join calculation is width-independent.
- Pass the lateral coordinate to the shader and composite all body layers there.
- Retain the non-SHDR OpenGL path as fallback.

### Acceptance

- SHDR stores two vertices per display sample and submits one body draw.
- Sharp bends and rock contacts have no cracks, inversions, or clipped layers.
- Layer boundaries share consistent AA.
- Shader failure still yields a working renderer.

## Phase 4: Analytical waveform-fill shader

### Design

Upload the canonical displayed Y values as a small one-dimensional texture when
the geometry revision changes, then render one editor-sized quad. The shader
computes:

- oscillator fill between curve and midpoint, or envelope fill to the bottom;
- positive/negative vertical gradients;
- authored-column alternating shade;
- oscillator midpoint treatment;
- AA at the curve boundary.

### Work

- Add a dedicated waveform shader and lifecycle-safe curve texture.
- Express authored-column parity analytically from fragment X and point count.
- Match NanoVG oscillator and envelope fill semantics.
- Remove the full-resolution `waveColumnTexture` pixel generation and per-point
  immediate-mode quads once visual parity is established.
- Keep an explicit fallback if shader initialization fails.

### Acceptance

- One draw renders the GL waveform area.
- No full-editor waveform material texture or CPU pixel raster remains.
- Curve texture uploads happen only when display geometry changes.
- Zero crossings, envelope boundaries, alternating columns, and midpoint align
  across normal and expanded views.

## Phase 5: Reusable buffers and accurate timing

### Work

- Add one reusable dynamic VBO for the consolidated body strip.
- Add a static/shared buffer for editor-sized quads if it simplifies submission.
- Upload body data only when the geometry revision changes.
- Restore resources safely after Rack or DAW GL context recreation.
- Add a rotating pool of GPU timer queries where supported; read results several
  frames later and never synchronously wait.
- Report geometry CPU time, GL CPU submission time, and GPU time separately.

### Acceptance

- SHDR does not submit pointers into CPU vectors.
- Static quad geometry is not rebuilt per draw.
- Context recreation does not retain stale GL handles.
- Unsupported contexts keep CPU-only telemetry without behavior changes.
- Buffer changes are retained only if they improve submission or simplify code.

## Deferred: screen-space body rendering

After the incremental phases, evaluate deriving body distance from the same curve
texture used by the waveform fill. This could remove strip joins entirely, but it
has higher risk around steep slopes, end caps, and rock-constrained bends. Proceed
only if measured geometry or body submission cost remains meaningful.

## Validation matrix

For every visual phase, check:

- collapsed and expanded editor;
- NanoVG, OpenGL, and SHDR;
- oscillator and envelope modes;
- Slither at zero and continuously active;
- no rocks, multiple rocks, and sharp constrained bends;
- normal and high Rack zoom;
- renderer switching and GL context recreation.

Use captures to compare body width/alpha, rock contacts, zero crossings, envelope
fill, alternating columns, and midpoint placement. Renderer work is complete only
when measurements show where CPU work was removed or transferred and no mode has
lost its fallback behavior.

## Checklist

- [ ] Phase 0: capture baseline and remove dead body-render-target code.
- [ ] Phase 1: isolate cached NanoVG invalidation from GL animation and hover.
- [ ] Phase 2: add lean geometry requests and one canonical body material.
- [ ] Phase 3: consolidate SHDR body to one analytical strip.
- [ ] Phase 4: replace the waveform raster/column quads with one shader pass.
- [ ] Phase 5: add reusable buffers and asynchronous GPU timing.
- [ ] Decide from measurements whether screen-space body rendering is justified.

## Likely files

- `src/WyrmRenderGeometry.hpp`
- `src/WyrmWaveEditor.cpp`
- `src/WyrmRendererGL.cpp`
- `src/WyrmWidget.cpp`
- `src/Wyrm.hpp`
- `src/DebugTerminalTransport.hpp/.cpp`
