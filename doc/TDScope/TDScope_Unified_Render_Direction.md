# TDScope Unified Render Direction & Implementation Brief

## Purpose

TDScope currently has multiple render paths that began as performance experiments but have evolved into different visual instruments. The next step should not be “pick the fastest path and tolerate the look.” The next step should be: define one canonical visual language, make every renderer consume the same frame model and style constants, and keep alternate engines as implementation backends rather than separate aesthetics.

Recommendation: make the **tail raster look** the reference style and name the canonical style **Temporal Trace**. Keep the shader effect as an optional **Vintage Phosphor** treatment, but it should layer character on top of the same information model rather than becoming a separate interpretation of the waveform.

## Current-state diagnosis

### What is good

- The core row representation is strong: every visible row contains a left/right amplitude span, a visual intensity value, a transient/color-drive value, and a validity flag.
- The tail raster path has the most pleasing identity: dense luminous spans, integrated connectors, enough persistence to feel physical, and a good sense of motion.
- The read-head treatment is already communicative: a full-width amber/yellow horizontal band is easy to recognize while dragging and while observing the deck.
- The side-by-side stereo layout is clean and should remain the default stereo interpretation.
- The existing `scopeColorBrightness`, palette schemes, transient halo toggle, connector toggle, and render-main toggle provide useful controls; the problem is organization and drift, not that these concepts are wrong.

### What is drifting

1. **Renderer choice is user-facing as a debug concept.**
   - Current menu exposes `Standard`, `Tail raster`, `OpenGL`, `OpenGL SHDR`, and `SHDR Effect` under `Debug Render`.
   - These are currently both rendering engines and visual styles, which makes the module feel experimental rather than intentional.

2. **Palette logic is duplicated.**
   - `TDScope.cpp` has a richer low/mid/high palette ramp with midpoints and hold widths.
   - `TDScopeGL.cpp` has a simpler low/high ramp and then compensates with shader-side saturation, hot lift, hue shift, scanlines, and vignette.
   - This guarantees that the same color scheme cannot mean the same thing in all render paths.

3. **Row geometry is duplicated.**
   - `TDScopeDisplayWidget` and `TDScopeGlWidget` both own row arrays, live buckets, history caches, auto-scale logic, range changes, stereo layout decisions, and cached rebuild state.
   - Any behavior fix in one path can easily miss the other.

4. **The shader path is doing too much style authorship.**
   - The field shader has its own width, alpha, halo, continuity, saturation, jitter, hue-shift, scanline, chromatic, and vignette logic.
   - That can be beautiful, but it should be governed by a named effect amount, not silently replace the module’s base look.

5. **The hard-coded row density story is confused.**
   - `computeScopeDisplayVerticalSupersample()` currently returns `0.55f` before its adaptive zoom logic can execute.
   - Both major paths then use a hard-coded `rowCount = 333`.
   - That may be acceptable, but the code should say “333 is the canonical row count for this display” instead of pretending dynamic supersampling is active.

## Target visual direction: “Temporal Trace”

Temporal Trace should feel like a DJ waveform display that has been absorbed into the Temporal Deck/Leviathan visual language: clear, high-contrast, alive, and slightly arcane, but never noisy enough to obscure information.

### Core visual grammar

Use the same grammar in every renderer:

- **Vertical axis = time / lag window.**
  - Older audio sits toward one end of the vertical window, newer/now audio sits at the read-head reference.
  - Honor `scopeVerticalInverted`, but do not allow inversion to change color, thickness, or transient logic.

- **Horizontal span = min/max amplitude envelope.**
  - Each row draws from `x0` to `x1`.
  - The centerline represents zero amplitude.
  - Larger spans mean larger waveform excursions.

- **Brightness/intensity = local signal energy.**
  - `visualIntensity` should primarily convey local amplitude strength.
  - It should control alpha and slight stroke width, but not so much that low-level material vanishes.

- **Color-drive / halo = transient or discontinuity emphasis.**
  - `rowColorDrive` should push the hue upward and optionally bloom white.
  - High transient drive should mean “pay attention here,” not simply “make everything brighter.”

- **Connectors = shape continuity / motion.**
  - Connectors should make the waveform feel continuous when envelope edges move significantly from row to row.
  - Keep connectors subtle enough that they do not become a second waveform.

- **Read-head = command point.**
  - Keep the existing full-width amber/yellow multi-line band.
  - It should always be visible over every renderer and every color scheme.
  - It is an interaction anchor, not a decorative scanline.

- **Stereo = two equal lanes.**
  - Side-by-side is still the clearest mode.
  - The lane divider should be faint and stable.
  - The right-lane enable should move out of “debug” naming if it remains user-facing.

## Canonical appearance parameters

Use the current tail raster values as the initial canonical style because they already feel coherent.

### Canonical tone calculation

```cpp
float tone = clamp(0.78f * visualIntensity + 0.22f * transientDrive, 0.f, 1.f);
```

Connectors may use a slightly more energy-weighted tone:

```cpp
float connectorTone = clamp(0.82f * connectorVisual + 0.18f * connectorTransient, 0.f, 1.f);
```

Do not let OpenGL field shader, OpenGL geometry, and NanoVG each invent different tone formulas.

### Canonical main trace

Initial values should match the tail raster path:

```cpp
alpha = 122 + 120 * tone;
width = (0.78 + 0.62 * tone) * zoomThicknessMul;
color = sampleScopePalette(tone, alpha);
color = brightenColor(color, transientDrive * 0.90);
```

Implementation notes:

- In raster mode, the width becomes integer row fill height.
- In NanoVG mode, the width becomes `nvgStrokeWidth`.
- In OpenGL line mode, the width becomes `glLineWidth` or shader radius.
- In field shader mode, the formula should be directly mirrored in GLSL.

### Canonical transient halo

Initial values should match the tail raster path:

```cpp
float haloLinear = clamp((transientDrive - 0.080f) / 0.920f, 0.f, 1.f);
float haloT = haloLinear * haloLinear;
uint8_t haloAlpha = round((72 + 176 * max(visualIntensity, 0.24f)) * haloT);
float haloExtend = (1.35f + 5.20f * haloT) * zoomThicknessMul;
float haloWidth = mainWidth + (1.10f + 2.20f * haloT) * zoomThicknessMul;
```

Notes:

- White halo is acceptable because it reads as transient energy rather than a second palette.
- Do not use chromatic halo by default.
- Vintage mode may add color instability, but base mode should remain clean.

### Canonical connectors

Use connectors only when row-to-row edge motion is visually meaningful:

```cpp
float connectorMinDeltaPx = max(0.60f * zoomThicknessMul, 0.40f);
if (abs(x0 - prevX0) >= connectorMinDeltaPx || abs(x1 - prevX1) >= connectorMinDeltaPx) {
  // draw connector
}
```

Suggested connector style:

```cpp
alpha = 88 + 92 * connectorTone;
width = (0.58 + 0.40 * connectorTone) * zoomThicknessMul;
color = sampleScopePalette(connectorTone, alpha);
color = brightenColor(color, connectorTransient * 0.72);
```

Connectors should remain subordinate to the main trace.

## Palette direction

### Make palette sampling a shared utility

Create a small shared style utility, preferably in `TDScopeStyle.hpp` or `TDScopeRenderStyle.hpp`.

Proposed structures:

```cpp
namespace tdscope {

struct ScopeRgbStop {
  float r = 0.f;
  float g = 0.f;
  float b = 0.f;
};

struct ScopePaletteSpec {
  ScopeRgbStop low;
  ScopeRgbStop mid;
  ScopeRgbStop high;
  float midPoint = 0.5f;
  float midHoldHalfWidth = 0.f;
};

struct ScopeColor {
  float r = 0.f;
  float g = 0.f;
  float b = 0.f;
  float a = 1.f;
};

ScopePaletteSpec paletteSpecForScheme(int scheme);
ScopeColor sampleScopeColor(int scheme, float intensity, float alpha01, float brightness01);
void buildScopeColorLut(int scheme, float brightness01, ScopeColor* out256);
float computeScopeZoomThickness(float rackZoom);

} // namespace tdscope
```

Rules:

- Both NanoVG and OpenGL must use this palette spec.
- The GL color LUT texture should be generated from the same function used by NanoVG.
- Preserve existing named schemes, but make their low/mid/high stops match across engines.
- Keep the “hot lift” behavior shared too, not duplicated.

### Recommended default palette

For the default Temporal Deck palette, use:

- Low: cyan/teal, readable but not neon.
- Mid: violet/indigo, giving the Leviathan identity.
- High: magenta-pink with slight white lift at the hottest peaks.

This keeps the Serato-like readability but moves the personality toward Temporal Deck: less traffic-light utility, more luminous artifact.

### Brightness behavior

Keep the existing brightness concept:

- 0–50% scales color down toward dark.
- 50–100% lifts color toward white.

But move the brightness math into the shared style helper so every renderer interprets brightness identically.

## Architecture recommendation

### Separate “what to draw” from “how to draw it”

Right now `TDScopeDisplayWidget` and `TDScopeGlWidget` each build rows and render them. Refactor toward this shape:

```cpp
struct ScopeRow {
  float y = 0.f;
  float x0 = 0.f;
  float x1 = 0.f;
  float visual = 0.f;
  float transient = 0.f;
  bool valid = false;
};

struct ScopeLaneFrame {
  std::vector<ScopeRow> rows;
  float centerX = 0.f;
  float halfWidth = 0.f;
};

struct ScopeFrame {
  ScopeLaneFrame left;
  ScopeLaneFrame right;
  bool renderStereo = false;
  bool sampleMode = false;
  bool verticalInverted = false;
  float drawTop = 0.f;
  float drawBottom = 0.f;
  float readHeadY = 0.f;
  float windowTopLag = 0.f;
  float windowBottomLag = 0.f;
  uint64_t publishSeq = 0;
  float densityPct = 100.f;
  int rowCount = 0;
};
```

Then implement renderers as consumers:

```cpp
void renderScopeFrameNanoVg(NVGcontext* vg, const ScopeFrame& frame, const ScopeRenderStyle& style);
void renderScopeFrameTailRaster(NVGcontext* vg, const ScopeFrame& frame, TailRasterCache& cache, const ScopeRenderStyle& style);
void renderScopeFrameOpenGlGeometry(const ScopeFrame& frame, GlScopeState& gl, const ScopeRenderStyle& style);
void renderScopeFrameOpenGlField(const ScopeFrame& frame, GlScopeState& gl, const ScopeRenderStyle& style);
```

This allows four engines to exist without four meanings.

### Recommended staged refactor

#### Phase 1 — Style unification only

This phase is low-risk and should happen first.

1. Add `TDScopeStyle.hpp`.
2. Move palette specs, brightness adjustment, hot lift, tone mix constants, halo constants, connector constants, and zoom thickness into it.
3. Replace the duplicated `ensureColorLut()`, `gradientColorForIntensity()`, and `brightenColor()` logic in both `TDScope.cpp` and `TDScopeGL.cpp`.
4. Adjust GL field shader uniforms so the shader receives canonical constants or a canonical LUT.
5. Keep existing render modes, but make them look closer.

Acceptance criteria:

- Switching between Standard, Tail Raster, OpenGL, and OpenGL SHDR no longer changes palette identity.
- The same color scheme and brightness value produce visually equivalent low/mid/high colors in every path.
- With Vintage disabled, OpenGL SHDR should not introduce hue shift, jitter, scanline, or vignette.

#### Phase 2 — Rename and reorganize controls

This phase makes the module feel intentional.

Replace the current menu shape:

```text
Debug Render
  Scope Rate
  Framebuffer cache
  Render Mode
    Standard
    Tail raster
    OpenGL
    OpenGL SHDR
  SHDR Effect
  Main trace
  Connectors
  Stereo right lane
```

With:

```text
Scope View
  Engine
    Auto
    Tail Raster CPU
    OpenGL Geometry
    OpenGL Field Shader
    NanoVG Compatibility
  Style
    Temporal Trace
    Vintage Phosphor
  Detail Layers
    Main trace
    Transient halo
    Continuity connectors
    Stereo right lane
  Rate
    120 Hz
    60 Hz
    30 Hz
  Advanced Debug
    Framebuffer cache
    Show renderer label
```

Short-term compatibility option:

- Keep `debugRenderMode`, `debugUseGlShaderRenderer`, and `debugShdrEffectEnabled` internally for one release.
- Add new user-facing names but map them to the old fields.
- In `dataFromJson()`, read both old and new names.
- In `dataToJson()`, write the new names and optionally continue writing old names for one transition period.

Recommended defaults:

```cpp
scopeRenderEngine = SCOPE_RENDER_ENGINE_AUTO;
scopeVisualStyle = SCOPE_STYLE_TEMPORAL_TRACE;
scopeVintageEffectEnabled = false;
scopeTransientHaloEnabled = true;
debugRenderMainTraceEnabled = true;
debugRenderConnectorsEnabled = true;
debugRenderStereoRightLaneEnabled = true;
```

If stability is more important than acceleration for the next commit, set Auto to choose Tail Raster CPU until the GL field shader is visually matched.

#### Phase 3 — Shared row-frame builder

Once style is unified, extract row construction.

Suggested new files:

```text
TDScopeFrame.hpp
TDScopeFrame.cpp
TDScopeStyle.hpp
```

Move these responsibilities into the shared frame builder:

- `displayFullScaleVolts` and auto-range smoothing.
- Decode `ScopeBin` to normalized min/max.
- Compute stereo lane geometry.
- Compute `windowTopLag`, `windowBottomLag`, `readHeadY`, and `sampleMode`.
- Build visible `ScopeRow` arrays.
- Build `rowColorDrive` / transient drive.
- Maintain live bucket state via a reusable `ScopeLiveBucketState` object.
- Maintain optional history state via a reusable `ScopeHistoryState` object.

Important: the state object can remain owned by each widget/backend, but the algorithms should be shared.

Example:

```cpp
struct ScopeFrameBuildConfig {
  float boxWidth = 0.f;
  float boxHeight = 0.f;
  float rackZoom = 1.f;
  int rowCount = tdscope::kScopeCanonicalRowCount;
  int rangeMode = TDScope::SCOPE_RANGE_5V;
  bool verticalInverted = false;
  bool stereoRequested = false;
  bool stereoPayloadAvailable = false;
};

struct ScopeFrameBuildState {
  float autoDisplayFullScaleVolts = 5.f;
  bool autoDisplayScaleInitialized = false;
  bool autoLastSampleMode = true;
  float autoLivePeakHoldVolts = 0.f;
  int autoLivePeakHoldFrames = 0;
  ScopeLiveBucketState liveLeft;
  ScopeLiveBucketState liveRight;
  ScopeHistoryState history;
};

bool buildScopeFrame(
  const temporaldeck_expander::HostToDisplay& msg,
  const ScopeFrameBuildConfig& config,
  ScopeFrameBuildState& state,
  ScopeFrame* outFrame);
```

This keeps per-renderer caches possible while preventing algorithm drift.

#### Phase 4 — Make the shader a style modifier, not a separate visual system

Rename `debugShdrEffectEnabled` to something like:

```cpp
bool scopeVintageEffectEnabled = false;
float scopeVintageAmount = 1.f; // optional later
```

In GLSL:

- `uShdrEffect = 0.0` should mean “pure Temporal Trace, shader implementation.”
- `uShdrEffect = 1.0` should mean “Temporal Trace + vintage phosphor.”
- The effect may add:
  - faint scanline modulation,
  - very subtle row jitter,
  - mild vignette,
  - slight chroma separation on high transient rows,
  - mild phosphor bloom.
- The effect must not alter:
  - row-to-row amplitude interpretation,
  - read-head position,
  - palette identity,
  - brightness semantics,
  - transient meaning.

Suggested Vintage limits:

```cpp
maxJitterPx = 0.65f;       // current 3.0px transient jitter is too strong for a clarity-first view
scanlineDepth = 0.035f;    // subtle, not CRT cosplay
vignetteStrength = 0.10f;  // corners only
chromaShiftPx = 0.45f;     // high transient rows only
hueShiftAmount = 0.12f;    // current high hue shift can overpower palette identity
```

Vintage should feel like old glass remembering the waveform, not like a malfunctioning cable unless the user explicitly asks for chaos.

## Specific code-level recommendations

### 1. Replace dead adaptive supersample code

Current `computeScopeDisplayVerticalSupersample()` returns `0.55f` before its adaptive body can execute. Decide one of two directions:

Option A — honest fixed density:

```cpp
constexpr int kScopeCanonicalRowCount = 333;
float computeScopeDisplayVerticalSupersample(float rackZoom) {
  (void) rackZoom;
  return 0.55f;
}
```

Option B — real adaptive density:

```cpp
float computeScopeDisplayVerticalSupersample(float rackZoom) {
  rackZoom = std::max(rackZoom, 1e-4f);
  float zoomOutT = clamp((1.0f - rackZoom) / 0.35f, 0.f, 1.f);
  float zoomInT = clamp((rackZoom - 1.0f) / 1.0f, 0.f, 1.f);
  float supersample = 1.10f + (1.35f - 1.10f) * (1.f - zoomOutT);
  supersample += (kScopeDisplayVerticalSupersampleMax - 1.35f) * zoomInT;
  return clamp(supersample, 1.10f, kScopeDisplayVerticalSupersampleMax);
}
```

For the current design, Option A is safer. The row count is already hard-coded and visually stable. Turn dynamic density back on only after visual equivalence tests exist.

### 2. Make tail raster the reference, not a side mode

Current tail raster behavior should become the baseline style spec.

Rename concepts:

```cpp
DEBUG_RENDER_TAIL_RASTER -> SCOPE_ENGINE_TAIL_RASTER_CPU
DEBUG_RENDER_STANDARD    -> SCOPE_ENGINE_NANOVG_COMPAT
DEBUG_RENDER_OPENGL      -> SCOPE_ENGINE_OPENGL
```

Or keep old enum names internally but present user-facing labels as:

```text
Tail Raster CPU
NanoVG Compatibility
OpenGL Geometry
OpenGL Field Shader
```

### 3. Stop using “debug” names for normal features

These should be promoted or aliased:

```cpp
debugRenderMainTraceEnabled       -> scopeMainTraceEnabled
debugRenderConnectorsEnabled      -> scopeContinuityConnectorsEnabled
debugRenderStereoRightLaneEnabled -> scopeStereoRightLaneEnabled
debugUseGlShaderRenderer          -> scopeUseGlFieldShader
debugShdrEffectEnabled            -> scopeVintageEffectEnabled
```

Keep old JSON keys for backward compatibility.

### 4. Keep status/read-head overlays outside engine-specific style

The read-head and status messages should be drawn by a common overlay path after the backend renders the waveform.

Desired order:

1. Optional subtle display backdrop/grid.
2. Waveform backend.
3. Stereo divider.
4. Read-head.
5. Status text / attach prompts.
6. Debug renderer label only when debug mode is enabled.

Do not let GL field shader own the read-head.

### 5. Add a subtle display backdrop

Right now the waveform is very dependent on the panel art behind it. Add a shared, minimal backdrop to increase contrast without losing the panel identity.

Suggested NanoVG backdrop:

```cpp
nvgBeginPath(vg);
nvgRect(vg, 0.f, drawTop, box.size.x, drawBottom - drawTop);
nvgFillColor(vg, nvgRGBA(3, 5, 10, 34));
nvgFill(vg);
```

Add faint guides:

- center zero-amplitude line per lane: alpha 20–28,
- lane borders: alpha 10–16,
- stereo divider: existing alpha 22 is fine,
- no dense grid by default.

### 6. Make Vintage Phosphor additive and bounded

In `TDScopeGL.cpp`, the field shader currently uses `uShdrEffect` to alter saturation, hot lift, jitter, halo hue shift, scanline, chroma, and vignette. Keep the idea, but reduce its authority.

Implementation direction:

- Replace shader literals with uniforms derived from `ScopeRenderStyle`.
- Set all vintage-only modifications to zero when `scopeVintageEffectEnabled == false`.
- Clamp vintage jitter and hue shift to subtle ranges.
- Make Vintage unavailable or hidden for CPU/NanoVG engines unless a CPU approximation is intentionally added later.

### 7. Do not make “OpenGL SHDR” the identity of the module

The shader path should be a renderer choice plus optional style modifier:

```text
Engine: OpenGL Field Shader
Style: Temporal Trace
Vintage Phosphor: Off/On
```

This keeps accelerated rendering from implying a separate aesthetic.

### 8. Add screenshot parity tests/manual QA scenes

Build a simple manual QA checklist. For each scene, compare Tail Raster CPU, NanoVG Compatibility, OpenGL Geometry, and OpenGL Field Shader with Vintage off.

Test scenes:

1. Silence / low signal.
2. Steady sine at moderate amplitude.
3. Dense full-spectrum loop.
4. Transient percussion / clicks.
5. Fast scratch drag.
6. Slow downward live drag away from NOW.
7. Stereo material with obvious L/R differences.
8. Zoom levels: 50%, 100%, 200%.

Acceptance criteria:

- Same read-head position.
- Same stereo lane layout.
- Same amplitude envelope shape.
- Same color palette identity.
- Same relative transient emphasis.
- Vintage off: no scanline/chroma/vignette/jitter.
- Vintage on: added character without hiding small waveform details.

## Suggested implementation prompt for Codex

Use this as the first implementation instruction:

```markdown
Refactor TDScope rendering toward a single canonical visual style named Temporal Trace.

Primary goals:
1. Make Tail Raster CPU the visual reference.
2. Add a shared TDScopeStyle helper for palette, brightness, hot lift, tone mix, halo constants, connector constants, and zoom thickness.
3. Update NanoVG standard, Tail Raster, OpenGL geometry, and OpenGL field shader paths to use the shared constants and palette mapping.
4. Rename user-facing menu labels so render engines are not presented as debug aesthetics. Keep JSON/backward compatibility with existing debugRenderMode/debugUseGlShaderRenderer/debugShdrEffectEnabled fields.
5. Reframe SHDR Effect as Vintage Phosphor. Vintage should be optional, off by default, and must not change the underlying waveform geometry or palette meaning when disabled.
6. Do not attempt the full shared ScopeFrame builder in this first pass unless the style unification is already complete and stable.

Preserve:
- existing lag drag behavior,
- existing read-head interaction behavior,
- existing expander communication protocol,
- existing color scheme names,
- existing debug metrics.

Acceptance criteria:
- Tail Raster, NanoVG, OpenGL Geometry, and OpenGL Field Shader with Vintage off look like the same visual style.
- Color scheme and brightness changes match across engines.
- Vintage Phosphor only adds subtle scanline/jitter/chroma/vignette/bloom and can be disabled cleanly.
- Existing patches load without losing user preferences.
```

## Priority recommendation

Do **not** start by deleting render paths. Start by removing their artistic independence.

Best order:

1. Shared palette/style constants.
2. Menu rename and Vintage toggle.
3. Shader toned down to match Temporal Trace when Vintage is off.
4. Tail raster selected as default/reference.
5. Shared row-frame builder after the style is stable.
6. Then decide whether Standard/NanoVG should remain as compatibility fallback or be hidden under Advanced.

This keeps the work bounded, protects interaction behavior, and gives the module a single visual soul: crisp enough to read, luminous enough to love, and weird only where weirdness is invited.
