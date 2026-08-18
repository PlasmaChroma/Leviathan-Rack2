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
- SHDR now renders body distance analytically in screen space from the shared
  curve texture. This removes strip joins, produces continuous acute peaks, and
  gives endpoints true round-cap distance semantics. The former analytical strip
  remains superseded; non-SHDR OpenGL retains its layered-strip fallback.
- Fallback body joins are calculated at each actual layer width rather than
  scaling a width-constrained unit join.
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
- NanoVG alternating-column shading is now emitted from exact authored-column
  boundaries rather than baked into its logical-resolution material bitmap. This
  matches the analytical GL cadence across renderer switches, zoom, and editor
  size; final visual approval remains pending.
- The temporary live GL CPU/GPU Debug Terminal probes and timer-query ring were
  removed after confirming that GL submission and execution were small relative
  to the whole-module draw. Reintroduce detailed renderer timing as CSV capture if
  later profiling makes it necessary.
- Debug Terminal `Process`, `Step`, and `Draw` retain their repository-wide macro
  meaning. Wyrm aggregates module-widget and detached expanded-overlay work for
  `Step`/`Draw`; editor-step, cached-editor, and live-overlay timings
  are reported only as subsequent component fields.
- Debug Terminal `Cache` measures the complete Rack editor-framebuffer redraw
  boundary (including framebuffer setup, NanoVG flush, and compositing) only when
  that cache is dirty. `CL.us` is the most recent such redraw; clean cached-image
  composites do not dilute either measurement.
- Detailed per-draw diagnosis now uses an opt-in CSV trace. Set
  `"WyrmDrawLogging": true` in the Rack user file
  `Leviathan/dragonking.txt` (with `"debug": true`) and restart Rack. Each Wyrm
  instance writes `Leviathan/Wyrm/wyrm_draw_<instance>_<timestamp>_<sequence>.csv`
  beneath the Rack user directory. Rows distinguish the normal module draw from
  the detached expanded editor and report total, child, decoration, editor
  surface, editor framebuffer, overlay, GL framebuffer, and residual timing.
  Dirty flags identify actual framebuffer rebuilds. CSV writes occur after the
  timed draw and flush periodically; disabling the flag on the next configuration
  refresh closes the file.
- In SHDR mode the same opt-in trace also uses asynchronous GPU timer queries to
  separate waveform-fill and analytical-body execution. A six-slot query ring
  never waits for results; supported drivers append valid, sequenced samples and
  their originating mode, envelope, Slither, and framebuffer-size metadata to
  later CSV rows. Unsupported timer-query contexts retain the CPU trace with
  `gpu_sample_valid=0`.
- The first Linux GPU capture found the analytical body responsible for roughly
  46--50% of collapsed SHDR pass time and 58--59% expanded. Its seven-segment
  neighborhood search now minimizes squared distance and takes one square root
  after the loop instead of one per candidate segment; the resulting distance
  and material coverage are otherwise unchanged. At the exact 489x445
  oscillator comparison point, the follow-up Linux capture reduced median body
  GPU time from 112.34 us to 94.79 us (15.6%) and combined fill-plus-body time
  from 167.73 us to 146.54 us (12.6%). The unchanged waveform control was 7.7%
  faster between sessions, so the conservative session-normalized body gain is
  about 8.6%. Other follow-up states were captured at different framebuffer
  sizes and are useful scaling data rather than direct before/after comparisons.

Immediate validation checkpoint:

- Visually compare the screen-space SHDR body against NanoVG in collapsed and
  expanded views, especially acute peaks, round endcaps, steep segments, sharp
  rock bends, outer-edge softness, and internal layer transitions.

Known limitation: screen-space distance checks a fixed local neighborhood of
curve segments for GLSL 1.20 compatibility and bounded fragment cost. Pathological
near-vertical folds should be included in visual validation; expand the fixed
neighborhood only if a real capture shows a missed nearest segment.

Checkpoint result: the bounded-miter body was provisionally accepted after a
brief Rack test, then `wyrm.png` exposed remaining peak continuity and square-cap
differences against NanoVG. A radial-over-strip experiment merely changed those
artifacts by double-compositing visible nodes, so it was removed. SHDR now uses
screen-space curve distance and awaits visual approval.

Next implementation slice after approval:

1. Visually validate analytical fill parity in oscillator/envelope and collapsed/
   expanded views. In particular check polarity, midpoint thickness, column
   alignment and cadence, curve-edge AA, and behavior around rock-constrained
   segments.
2. After approval, remove the full-resolution CPU waveform raster; decide whether
   the immediate-mode fallback remains worthwhile or should become NanoVG fallback.
3. Capture GPU timing for the screen-space body in collapsed, expanded, and
   Slither-active states before deciding whether to consolidate the two full-screen
   shader passes.

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
- The new screen-space SHDR body still needs visual and GPU-cost approval.
- GPU timing is available in Dragon King debug but still needs authoritative
  Windows/MSYS2 Rack captures.

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
- Keep detailed renderer timing out of the live terminal; use a dedicated CSV
  capture if another renderer investigation is needed.

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

## Phase 3: One-strip analytical SHDR body (superseded)

This phase landed and provided a working fallback, but visual comparison exposed
join and cap differences inherent to the strip topology. SHDR now uses the
screen-space body phase below; non-SHDR OpenGL retains layered strips.

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

- Add a static/shared buffer for editor-sized quads if it simplifies submission.
- Restore resources safely after Rack or DAW GL context recreation.
- If renderer profiling becomes necessary, capture geometry CPU time, GL CPU
  submission time, and asynchronous GPU timer-query results to CSV rather than
  expanding the live Debug Terminal schema.

### Acceptance

- SHDR body rendering does not submit pointers into CPU vectors.
- Static quad geometry is not rebuilt per draw.
- Context recreation does not retain stale GL handles.
- Profiling remains optional and introduces no work in normal rendering.
- Buffer changes are retained only if they improve submission or simplify code.

## Screen-space SHDR body rendering (implemented, pending approval)

SHDR derives body distance from the same curve texture used by the waveform fill.
This removes strip joins and naturally produces round endcaps. Its fixed local
segment neighborhood preserves GLSL 1.20 compatibility and bounded work, but steep
slopes, rock-constrained bends, expanded-editor GPU time, and continuous Slither
remain approval gates.

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
- [x] Phase 3: consolidate SHDR body to one analytical strip (subsequently
  superseded after visual evaluation).
- [ ] Approve the screen-space SHDR body visually and from GPU timing captures.
- [ ] Phase 4: replace the waveform raster/column quads with one shader pass
  (analytical path implemented and build-validated; visual approval and old-raster
  removal pending).
- [ ] Phase 5: asynchronous GPU timing implemented; capture authoritative results
  and decide whether reusable quad buffers are worthwhile.

## Likely files

- `src/WyrmRenderGeometry.hpp`
- `src/WyrmWaveEditor.cpp`
- `src/WyrmRendererGL.cpp`
- `src/WyrmWidget.cpp`
- `src/Wyrm.hpp`
- `src/DebugTerminalTransport.hpp/.cpp`
