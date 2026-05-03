# Wyrm Sandy Hysteresis Waveform View — Brief Spec

## Intent

Add a visual-only “sandy hysteresis” layer to the Wyrm waveform editor. The editor should feel like a small illuminated sand tray: the waveform/wyrm body slithers through it, dragging grains aside, leaving transient furrows, ridges, and settling trails. The feature should make the existing `Slither` and `Slither Speed` controls visibly alive without changing audio behavior.

## Existing anchors

The implementation should build on the current `WyrmWaveEditor` rather than introduce a new widget. The editor already owns a UI-only `visualSlitherPhase`, advances it from wall-clock time, computes per-point visual slither offsets, and draws the displayed waveform from `displayWavePoint()`. Wyrm also already exposes rock positions/radii, slither amount/speed, and rock clamp behavior. Therefore the sand layer should read existing editor/display state and stay entirely on the UI side.

## User-facing behavior

- The waveform area has a warm, granular sand background instead of a flat dark fill.
- At rest, sand has subtle static grain variation: darker low grains, brighter flecks, very slight dunes.
- When `Slither` is above zero, the moving displayed waveform disturbs the sand.
- Disturbance should behave like a hysteresis trace:
  - the current wyrm body cuts a darker trough through the sand;
  - displaced sand gathers as brighter ridges above/below the body;
  - previous positions remain faintly visible, then decay/settle over time;
  - faster `Slither Speed` increases apparent agitation, but should not become noisy chaos.
- Rocks should feel embedded in the sand. Around rocks, disturbance should visibly bend/clump, but rock dragging/lifting behavior remains unchanged.
- Manual waveform editing should also disturb the sand under the cursor, giving immediate tactile feedback.

## Non-goals

- Do not simulate real granular physics.
- Do not alter oscillator output, wavetable generation, slither math, rock collision, patch JSON, or DSP thread behavior.
- Do not allocate memory every frame.
- Do not require OpenGL/shader rendering for the first pass; NanoVG should be sufficient.

## Recommended implementation model

Use a low-resolution visual field, not thousands of free particles.

### Add UI-only sand state to `WyrmWaveEditor`

Suggested members:

```cpp
bool sandEnabled = true;
bool sandInitialized = false;
int sandW = 96;
int sandH = 48;
std::vector<float> sandDepth;      // persistent signed disturbance: -trough, +ridge
std::vector<float> sandEnergy;     // short-lived sparkle/agitation
std::vector<float> sandBaseNoise;  // deterministic static grain texture
std::vector<Vec> previousWyrmPath; // previous sampled path in editor pixels
```

Size can be derived from editor pixel size, clamped to a modest budget, for example:

```cpp
sandW = clamp(int(box.size.x * 0.65f), 64, 128);
sandH = clamp(int(box.size.y * 0.65f), 32, 72);
```

Initialize once, and reinitialize only if editor dimensions change meaningfully.

### Per-frame update

Call `updateSand(nowSec)` near the start of `WyrmWaveEditor::draw()` after `advanceVisualSlitherPhase()`.

Update sequence:

1. Apply decay/settling:
   - `sandDepth *= exp(-elapsed * depthDecay)`
   - `sandEnergy *= exp(-elapsed * energyDecay)`
2. Build current wyrm path in editor pixels using existing `displayWavePoint(i)` logic.
3. Compare current path to `previousWyrmPath`.
4. For each segment, disturb nearby sand cells using a capsule distance field:
   - center trough: subtract depth near the body centerline;
   - normal ridges: add depth slightly above and below the centerline;
   - energy: add short-lived sparkle where movement delta is high.
5. Store current path as `previousWyrmPath`.

Recommended first-pass constants:

```cpp
bodyRadiusPx = 3.0f;
ridgeOffsetPx = 3.5f;
troughStrength = 0.08f + 0.20f * slitherAmount;
ridgeStrength = 0.04f + 0.12f * slitherAmount;
depthDecay = 0.55f;   // slower, hysteresis memory
energyDecay = 3.5f;   // faster sparkle fade
```

When `Slither` is zero, still render sand, but only generate disturbances from manual editing/rock dragging. Avoid constant background motion in this state.

## Drawing order

Inside `WyrmWaveEditor::draw()`:

1. Draw sandy background instead of the current flat dark rectangle.
2. Draw sand disturbance cells/dots.
3. Draw midline and hover guides.
4. Draw waveform columns/points.
5. Draw wyrm body strokes.
6. Draw body plates/scales.
7. Draw rocks and drag arrows.

This keeps the sand visually behind the waveform while allowing trails to be read as history.

## Sand rendering style

For each sand cell:

- Convert grid cell center to editor pixels.
- Use `sandBaseNoise[cell]` to vary grain brightness.
- Use `sandDepth[cell]`:
  - negative depth = darker trough, slightly more transparent;
  - positive depth = brighter raised grain/ridge;
- Use `sandEnergy[cell]` for small bright flecks.

NanoVG approach:

```cpp
nvgBeginPath(vg);
nvgRect(vg, x, y, cellW + 0.5f, cellH + 0.5f);
nvgFillColor(vg, nvgRGBA(r, g, b, alpha));
nvgFill(vg);
```

Optional refinement: draw only every other cell when zoomed out or when editor is small; draw sparse flecks with tiny `nvgCircle()` calls for high-energy cells only.

## Interaction hooks

- `applyPointFromPos()` should add a local disturbance stamp at the mouse position.
- `moveRockFromMouse()` should add a larger, soft-edged displacement stamp around the rock’s old and new centers.
- `rockMouseMode == ROCK_MOUSE_LIFTS` should reduce disturbance while lifted, then add a small landing puff when released.

## Context menu

Add a submenu under Wyrm context menu:

- `Sand View` on/off, default on.
- `Sand Persistence`: Short / Medium / Long, default Medium.
- Optional later: `Sand Detail`: Low / Medium / High, default Medium.

These can remain UI-only for the first pass. Persist them to JSON only if the module already persists similar visual preferences elsewhere.

## Performance constraints

- No allocations in `draw()` after initial resize/init.
- No audio-thread writes or locks.
- No reads from polyphonic per-channel audio phase are required; use the existing visual slither phase.
- Target grid budget: <= 128 x 72 cells.
- Target additional UI cost: visually smooth at normal Rack zoom while dragging knobs and editing points.

## Acceptance criteria

- With `Sand View` on, the waveform editor has a clearly sandy background.
- Increasing `Slither` creates visible moving furrows/ridges tied to the displayed waveform path.
- Increasing `Slither Speed` increases visual travel rate consistently with the existing UI slither phase.
- Trails persist briefly and fade smoothly without flicker.
- Manual point drawing disturbs the sand under the cursor.
- Rock dragging disturbs nearby sand but does not change existing rock drag/lift semantics.
- Turning `Sand View` off restores a simple low-cost background.
- Audio output, patch state, point editing, rocks, sync, fold, and factory shapes behave exactly as before.

## Implementation notes for Codex

Start by landing the smallest viable version:

1. Add `SandField` state and initialization helpers inside `WyrmWaveEditor.cpp`.
2. Replace the flat background fill with `drawSandBackground()`.
3. Add `updateSand()` using the existing `displayWavePoint()` path.
4. Add local disturbance stamps for manual editing and rock movement.
5. Add context-menu toggles only after the visual is stable.

Prefer readable helpers over cleverness. The feature is a visual spell, not a DSP primitive: it should be beautiful, deterministic, cheap, and impossible to destabilize the oscillator.


Yes — **it would change the implementation path**, but not the core suggestion.

The original advice still holds conceptually: treat the sand as a **persistent memory field** disturbed by the animated Wyrm trace. But with OpenGL shaders, I would stop thinking “CPU grid drawn with NanoVG” and instead think:

> **GPU sand texture + ping-pong feedback pass + shader compositing.**

Your current editor is a `TransparentWidget` using NanoVG drawing, with UI-only animation state like `visualSlitherPhase`, `advanceVisualSlitherPhase()`, `slitherOffsetForIndex()`, and `displayWavePoint()` already separated from the audio DSP path. That’s a good seam: the shader version should still live in the visual/widget layer, not in `process()`. 

The shader version I’d spec as:

1. **Keep Wyrm/slither logic CPU-side**
   Use the existing phase/value mapping, rocks, `slitherOffset()`, `slitherSpeedFactor()`, and `applyRockClamp()` as the authoritative visual path generator. The oscillator already has `wavePoints`, `pointCount`, rocks, and slither state available in the module model. 

2. **Upload a small curve representation to GPU**
   Either upload the displayed Wyrm body as:

   * a 1D texture of sampled points, maybe 128–256 samples, or
   * a uniform array if small enough, or
   * a dynamic VBO polyline if drawing geometry.

3. **Use a persistent sand field texture**
   Instead of storing `sandHeight[]` or `sandEnergy[]` on CPU, create two low-res textures:

   * `sandA`
   * `sandB`

   Each frame:

   * read `sandA`
   * apply decay/diffusion/settling
   * inject disturbance along the current Wyrm trace
   * write into `sandB`
   * swap.

4. **Composite visually in a final shader**
   Final shader shades sand using:

   * base tan/noise grain
   * height/depression from the sand texture
   * ridge highlights
   * subtle sparkling/iridescent grains
   * darker trough where the Wyrm recently passed

5. **Leave interaction overlays in NanoVG**
   I would not immediately move everything to GL. Keep these as current NanoVG overlays:

   * editable points
   * hover column
   * rocks
   * drag arrows
   * waveform body stroke, unless you later want it shader-rendered too.

The big architectural change is this:

```md
NanoVG version:
CPU updates sand grid → NanoVG draws sand cells/noise → NanoVG draws Wyrm/rocks

OpenGL version:
CPU computes Wyrm path → GPU feedback shader updates sand texture → GPU composites sand → NanoVG overlays editor controls
```

For Rack specifically, I would still be cautious. A shader path is more powerful, but more fragile than a simple CPU/NanoVG layer. Given your recent OpenGLWidget scaling/freezing adventures, I’d only go GL here if you want the sand to look genuinely alive: smooth trails, normal-map-like ridges, shimmering grains, continuous erosion, soft diffusion. If the target is “nice atmospheric background that implies hysteresis,” CPU grid is safer. If the target is “tiny desert organism memory field,” shader path is the dragon gate.

My revised recommendation would be:

**Phase 1:** CPU/NanoVG sand field, prove the look and behavior.

**Phase 2:** Replace only the sand layer with an OpenGL ping-pong texture.

**Phase 3:** Optionally shader-render the Wyrm body itself, but keep rocks/edit handles in NanoVG for stability.

So: yes, OpenGL changes the mechanics substantially — especially persistence, decay, blur, and texture-based memory — but the spec’s central model remains correct: **the Wyrm disturbs a persistent sand memory layer; the sand slowly forgets.**
