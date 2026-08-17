# Wyrm Renderer Implementation Plan

## Status and scope

Last reviewed: 2026-08-17, after removal of the animated sand-field system.

## Resume state

Updated: 2026-08-17

Implemented and build-validated:

- Removed the disabled body render-target/FBO implementation.
- GL Slither no longer dirties the cached NanoVG editor framebuffer.
- `DisplayGeometryCache` supports points-only GL requests and upgrades cached
  points with `nearRock` metadata only for NanoVG.
- Envelope progress is now a plain translucent overlay rather than a PNG-backed
  texture.
- NanoVG, OpenGL, and SHDR now read one canonical body material.
- SHDR now builds and submits one outer strip; its fragment shader composites the
  outer, middle, and highlight layers analytically.
- Body joins are calculated at each actual fallback width, and at the actual
  outer width for SHDR, rather than scaling a width-constrained unit join.
- Rock hover emphasis, drag arrows, and the drag-mode label now render in the
  live overlay. Hover-only pointer movement no longer dirties the cached editor;
  actual rock dragging still does because it changes persistent geometry.
- The GL waveform area now has an analytical shader path backed by the shared
  display curve in a one-row `RGBA32F` texture. One editor-sized quad evaluates
  oscillator/envelope coverage, polarity gradients, alternating columns, curve
  AA, and the oscillator midpoint. The former CPU raster and per-column quads are
  retained temporarily as automatic shader-initialization fallback.
- Envelope fill in the analytical GL path uses a continuous height gradient from
  purple at the editor floor to cyan at the ceiling. Oscillator mode retains its
  bipolar cyan/purple material split.
- NanoVG envelope fill uses the same purple-floor to cyan-ceiling material,
  including matching alternating-column shading and its non-texture fallback.

Immediate validation checkpoint:

- Visually compare the new SHDR body against NanoVG and OpenGL in collapsed and
  expanded views, especially sharp rock bends, outer-edge softness, and internal
  layer transitions. `visual.png` exposed severe strip pinching at acute bends;
  the previous bevel/miter clamps were replaced on 2026-08-17. Fully preserving
  width produced visibly sharp miter spikes, so the current compromise caps the
  miter at 1.6x half-width. That correction builds but still needs visual approval.

Known limitation: a single triangle strip cannot provide both perfectly constant
width and truly rounded joins at arbitrary acute turns. If the bounded-miter
result is still unacceptable, do not continue tuning the scalar cap; use explicit
join geometry or the deferred screen-space body renderer.

Checkpoint result: the bounded-miter body was provisionally accepted after a
brief Rack test. It is not considered perfect, but further join tuning is deferred
until after the curve-texture and analytical-fill work, since that data path may
support replacing strip-based body rendering altogether.

Next implementation slice after approval:

1. Visually validate analytical fill parity in oscillator/envelope and collapsed/
   expanded views. In particular check polarity, midpoint thickness, column
   alignment, curve-edge AA, and behavior around rock-constrained segments.
2. After approval, remove the full-resolution CPU waveform raster; decide whether
   the immediate-mode fallback remains worthwhile or should become NanoVG fallback.
3. Add reusable buffers, then asynchronous GPU timing.

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
    base rocks in every mode
  live NanoVG overlay
    rock hover/drag decoration, tracer, and envelope progress
```

The GL and NanoVG editor paths share `DisplayGeometryCache`. GL Slither animation
invalidates only the GL framebuffer; NanoVG Slither invalidates the NanoVG editor.
Hover-only interaction stays in the live overlay. The static background
framebuffer is independent and is only dirtied on size changes.

The remaining significant costs and divergences are:

- The analytical GL waveform fill is active, but its former full-resolution CPU
  texture and immediate-mode column renderer remain compiled as fallback pending
  final visual approval.
- The accepted strip body still has bounded-miter compromises at acute corners.
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

- [x] Remove dead body-render-target code (baseline capture remains manual).
- [x] Phase 1: isolate cached NanoVG invalidation from GL animation and hover.
- [x] Phase 2: add lean geometry requests and one canonical body material.
- [x] Phase 3: consolidate SHDR body to one analytical strip (provisionally
  accepted with the documented bounded-miter limitation).
- [ ] Phase 4: replace the waveform raster/column quads with one shader pass
  (analytical path implemented and build-validated; visual approval and old-raster
  removal pending).
- [ ] Phase 5: add reusable buffers and asynchronous GPU timing.
- [ ] Decide from measurements whether screen-space body rendering is justified.

## Likely files

- `src/WyrmRenderGeometry.hpp`
- `src/WyrmWaveEditor.cpp`
- `src/WyrmRendererGL.cpp`
- `src/WyrmWidget.cpp`
- `src/Wyrm.hpp`
- `src/DebugTerminalTransport.hpp/.cpp`
