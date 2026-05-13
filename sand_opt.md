Dragon King Leviathan — yes. The right architecture is **sand as an interchangeable visual backend**, not “more stuff inside `WyrmWaveEditor::draw()`.” The current code already has the right conceptual seam: sand is UI-only, driven by `visualSlitherPhase`, `displayWavePoint()`, `sandDepth`, `sandEnergy`, and `sandBaseNoise`, while the audio path remains separate.

The performance diagnosis is also clear: your telemetry says the normal editor/body path is roughly sub-millisecond, while sand adds about **0.4 ms update** plus **2.25 ms draw** per visible module. So the draw path, not the physics-ish update, is the first dragon to slay.  The current `drawSandBackground()` loops over every sand cell and emits NanoVG rectangles, plus occasional sparkle circles, which means thousands of tiny immediate-mode draw operations per frame.

## Codex target: Wyrm sand renderer optimization plan

### Core rule

Do **not** shader-render the whole editor first.

The plan should isolate the sand layer:

```text
NanoVG overlays stay:
- waveform columns
- wyrm body
- points
- hover guides
- rocks
- drag arrows
- editor controls

Sand backend becomes swappable:
- SandBackend::NanoVGCells
- SandBackend::NanoVGImage
- SandBackend::OpenGLTexture
- SandBackend::ShaderFeedback
```

This aligns with the existing design doc’s recommendation to keep Wyrm/slither/rock path generation CPU-side while moving only the sand memory/composite layer toward GPU acceleration.

---

# Phase 0 — Measurement and backend split

Before optimizing visuals, Codex should formalize the backend boundary.

## Add enums

In `Wyrm.hpp`:

```cpp
enum WyrmSandBackend {
    WYRMSAND_NANOVG_CELLS = 0,
    WYRMSAND_NANOVG_IMAGE = 1,
    WYRMSAND_OPENGL_TEXTURE = 2,
    WYRMSAND_SHADER_FEEDBACK = 3
};

enum WyrmSandDetail {
    WYRMSAND_DETAIL_LOW = 0,
    WYRMSAND_DETAIL_MEDIUM = 1,
    WYRMSAND_DETAIL_HIGH = 2,
    WYRMSAND_DETAIL_AUTO = 3
};
```

Add atomics or plain UI-side fields:

```cpp
std::atomic<int> sandBackend {WYRMSAND_NANOVG_IMAGE};
std::atomic<int> sandDetail {WYRMSAND_DETAIL_AUTO};
std::atomic<int> sandPersistence {1}; // 0 short, 1 medium, 2 long
```

`Wyrm` already persists `sandViewEnabled`, so add these beside the existing JSON fields rather than changing patch semantics. Existing patches may restore sand as enabled, so optimization matters even if new modules default it off.

## Keep existing metrics, extend slightly

The current editor already measures `sandUpdateUs`, `sandDrawUs`, total editor draw time, audio time, body sample count, point count, rock count, and sand enabled state.  Add:

```cpp
sandBackend
sandDetail
sandCellCount
sandActiveCellCount
sandImageUploadUs
sandGlDrawUs
```

Pass/fail should be based on:

```text
SUp us  = sand update
SDr us  = sand draw / composite
UI ms   = full editor draw
Aud us  = should remain unchanged
```

## Phase 0 acceptance

Target:

```text
No behavior change.
No visual change required.
Metrics identify backend/detail/cell count.
Sand off remains near current baseline.
```

---

# Phase 1 — Optimize existing NanoVG path

This is the quick survivability pass.

The design doc already suggests lower/adaptive grid resolution, skipping inactive cells, drawing fewer cells at low zoom, and adding a detail menu.  The current code clamps the sand field to roughly `64..128` by `32..72`, which can still mean thousands of cells, each drawn as a separate NanoVG path.

## 1.1 Add explicit quality tiers

Replace implicit sizing:

```cpp
targetW = clamp(int(box.size.x * 0.65f), 64, 128);
targetH = clamp(int(box.size.y * 0.65f), 32, 72);
```

with deterministic presets:

```cpp
struct SandGridSpec {
    int w;
    int h;
    int drawStride;
    float activeThreshold;
    bool drawSparkles;
};

SandGridSpec sandGridForDetail(int detail, float zoomOrScale, float recentUiMs) {
    switch (detail) {
        case WYRMSAND_DETAIL_LOW:
            return {48, 24, 2, 0.035f, false};
        case WYRMSAND_DETAIL_MEDIUM:
            return {64, 32, 1, 0.025f, true};
        case WYRMSAND_DETAIL_HIGH:
            return {96, 48, 1, 0.018f, true};
        case WYRMSAND_DETAIL_AUTO:
        default:
            if (recentUiMs > 2.0f) return {48, 24, 2, 0.04f, false};
            if (recentUiMs > 1.2f) return {64, 32, 1, 0.03f, false};
            return {96, 48, 1, 0.02f, true};
    }
}
```

I would avoid 128×72 except maybe as a hidden debug/highest mode. That resolution is visually seductive but not necessary inside the Rack module’s small editor rectangle.

## 1.2 Stop drawing static sand cell-by-cell

Current visual model:

```text
for every cell:
  compute shade
  nvgBeginPath()
  nvgRect()
  nvgFill()
  maybe nvgCircle()
```

That is the expensive part.

Instead:

```text
static base sand = one cached image
dynamic disturbances = only active cells
```

Add:

```cpp
std::vector<unsigned char> sandBaseRgba;
int sandBaseImage = 0;
bool sandBaseImageDirty = true;
```

When the grid changes, build `sandBaseRgba` once from `sandBaseNoise`, then:

```cpp
sandBaseImage = nvgCreateImageRGBA(vg, sandW, sandH, NVG_IMAGE_NEAREST, sandBaseRgba.data());
```

In draw:

```cpp
NVGpaint p = nvgImagePattern(vg, 0.f, 0.f, box.size.x, box.size.y, 0.f, sandBaseImage, 1.f);
nvgBeginPath(vg);
nvgRect(vg, 0.f, 0.f, box.size.x, box.size.y);
nvgFillPaint(vg, p);
nvgFill(vg);
```

That turns the static sand from thousands of NanoVG calls into one fill.

## 1.3 Draw only disturbed cells

For dynamic cells:

```cpp
const float activity = std::fabs(depth) + 0.65f * energy;
if (activity < activeThreshold)
    continue;
```

Then draw only those cells. At rest, sand becomes essentially one image draw. While slithering, only the trails/ridges draw.

## 1.4 Track active cells during update

In `disturbSandSegment()` and `stampSand()`, when a cell crosses the activity threshold, push its index into an active list:

```cpp
std::vector<int> sandActiveCells;
std::vector<uint8_t> sandActiveFlags;
```

Avoid scanning the full grid for drawing. Decay can still scan the whole field initially, but drawing should iterate active cells only:

```cpp
for (int idx : sandActiveCells) {
    float depth = sandDepth[idx];
    float energy = sandEnergy[idx];

    if (std::fabs(depth) + energy < retireThreshold) {
        sandActiveFlags[idx] = 0;
        continue;
    }

    drawDynamicCell(idx);
}
compactActiveListOccasionally();
```

Compaction can happen every 8–16 frames, not every draw.

## 1.5 Reduce update cost opportunistically

The update path is less scary than draw, but still worth trimming. Current update decays all cells every frame, then builds a current path and disturbs each segment when slither is active.

Replace global decay loop with lazy decay:

```cpp
float sandGlobalDepthDecayAccum;
float sandGlobalEnergyDecayAccum;
```

Store `depth`/`energy` in normalized accumulated space, or simpler: only decay active cells, because inactive cells are already visually irrelevant.

```cpp
for (int idx : sandActiveCells) {
    sandDepth[idx] *= depthDecay;
    sandEnergy[idx] *= energyDecay;
}
```

That changes exact decay slightly but should be invisible. The sand is memory-poetry, not a physics contract.

## 1.6 De-rate segment stamping

Current disturbance runs over `count - 1` waveform segments, so 128-point mode means 127 capsule stamps per frame. For sand, use a visual sample stride:

```cpp
int pathStride = 1;
if (detail == LOW) pathStride = 3;
if (detail == MEDIUM) pathStride = 2;
if (detail == HIGH) pathStride = 1;
```

Then stamp:

```cpp
for (int i = 0; i < count - pathStride; i += pathStride) {
    disturbSandSegment(currentPath[i], currentPath[i + pathStride], ...);
}
```

This should still read as continuous because the capsule radius is several pixels wide.

## Phase 1 acceptance

Target numbers:

```text
SDr: from ~2250 us down to < 600 us at Medium
SUp: from ~435 us down to < 250 us
Total UI with sand: under ~1.2 ms preferred, under ~1.6 ms acceptable
Sand off: unchanged
```

Visual acceptance:

```text
At rest: still granular.
Slither: still leaves trough/ridge memory.
Manual edit and rock landing stamps still visible.
No allocations during steady-state draw.
No audio changes.
```

---

# Phase 2 — NanoVG image backend

This is the “big CPU-side win” before going full OpenGL.

Instead of drawing disturbed cells as NanoVG geometry, generate one dynamic RGBA image buffer and draw it as one NanoVG image.

## Architecture

```text
CPU sand state:
  sandDepth[]
  sandEnergy[]
  sandBaseNoise[]

CPU image:
  sandRgba[]

NanoVG:
  nvgUpdateImage(vg, sandImage, sandRgba.data())
  nvgImagePattern(...)
```

This turns the entire sand background into:

```text
update CPU field
shade CPU pixels
upload/update one tiny image
draw one textured rectangle
```

For a 64×32 or 96×48 image, the upload is tiny.

## Key change

`drawSandBackground()` becomes:

```cpp
void drawSandBackground(NVGcontext* vg) {
    if (!sandEnabled()) {
        drawFlatBackground(vg);
        return;
    }

    ensureSandField();
    updateSandImagePixelsIfDirty();

    if (!sandImageValid)
        createSandImage(vg);
    else if (sandImageDirty)
        nvgUpdateImage(vg, sandImage, sandRgba.data());

    drawSandImage(vg);
}
```

This almost certainly beats drawing thousands of rectangles.

## Dirty policy

Do not regenerate image pixels unless:

```text
slither active
manual edit happened
rock moved / landed
active cell list non-empty
detail/backend changed
box size changed
persistence changed
```

At rest, no image update should happen. Just draw the cached texture.

## Visual improvement opportunity

Once using image shading, you can cheaply add better sand:

```cpp
// pseudo-shading
float hL = depthAt(x - 1, y);
float hR = depthAt(x + 1, y);
float hU = depthAt(x, y - 1);
float hD = depthAt(x, y + 1);

float normalLight = 0.5f + 0.35f * ((hL - hR) + 0.6f * (hU - hD));
float shade = base + depthBrightness + energyGlow + normalLight;
```

This gives the “raised ridge / dug trough” effect without additional draw calls.

## Phase 2 acceptance

Target:

```text
SDr: < 250 us Medium
SUp + image shading/upload: < 400 us Medium
Total UI with sand: near 0.8–1.1 ms in common case
At rest: almost same as sand off, plus one image draw
```

This phase may be enough. The desert becomes cheap enough to live.

---

# Phase 3 — Direct OpenGL texture renderer

This is the first GPU pass, but not yet a full feedback shader.

## Goal

Keep CPU sand simulation, but upload the shaded `sandRgba` buffer to an OpenGL texture and draw a textured quad directly.

This avoids NanoVG image overhead and gives more control over filtering, scaling, and future shader migration.

## Architecture

```text
WyrmWaveEditor
  SandField CPU state
  SandGLRenderer optional object

SandGLRenderer
  GLuint texture
  GLuint vao/vbo or immediate Rack-compatible quad path
  simple textured-quad shader
```

Pipeline:

```text
CPU updateSand()
CPU shadeSandRgba()
glTexSubImage2D()
draw full editor-sized quad
NanoVG overlays continue
```

## Important Rack safety rule

Do not make every editor a fragile standalone GL universe. Add the GL renderer with a defensive fallback:

```cpp
if (!sandGlRenderer || !sandGlRenderer->isReady()) {
    drawSandNanoVGImage(args.vg);
    return;
}
```

If context creation, shader compile, or texture upload fails, Wyrm should silently fall back to the NanoVG image backend.

## Direct GL renderer should not yet own simulation

This phase is not about ping-pong feedback. It only replaces the final draw/composite path.

That means:

```text
Same sandDepth/sandEnergy arrays.
Same visual behavior.
Different final blit.
```

## Phase 3 acceptance

Target:

```text
GL texture path visually matches NanoVG image path.
No zoom-change freeze/regression.
No crash on module delete.
No crash on patch close.
Fallback works.
SDr effectively becomes tiny; upload cost is measured separately.
```

---

# Phase 4 — Shader feedback sand renderer

This is the ultimate form.

The design doc’s shader direction is exactly right: upload a small Wyrm path representation, use two persistent sand textures, update one from the other each frame, then composite the sand in a final shader.

## GPU state

```cpp
struct SandShaderRenderer {
    GLuint sandTexA = 0;
    GLuint sandTexB = 0;
    GLuint noiseTex = 0;
    GLuint pathTex = 0;       // optional 1D texture
    GLuint updateProgram = 0;
    GLuint compositeProgram = 0;
    GLuint framebuffer = 0;
    bool swap = false;
};
```

Texture formats:

```text
sandTex:
  RG16F or RGBA16F preferred
  R = depth
  G = energy
  B/A optional future use

noiseTex:
  R8 or RGBA8 static grain/dune/noise
```

Resolution:

```text
Low:    64×32
Medium: 96×48
High:   128×64
```

Keep it low-res. The final composite shader can make it look richer than it is.

## CPU upload per frame

The CPU should upload only a compact displayed Wyrm path:

```cpp
struct SandPathPoint {
    float xNorm;
    float yNorm;
    float motion;
    float radius;
};
```

Use either:

```text
1D texture: 128–256 points
uniform array: okay for small point counts, less flexible
dynamic VBO: best if later drawing geometry
```

I’d choose **1D texture** because it maps naturally into shader sampling and avoids uniform limits.

## Feedback update shader

Each frame:

```text
read previous sand texture
apply decay
apply slight diffusion
inject trough/ridge/energy around Wyrm path
write next sand texture
swap
```

Fragment shader conceptual pseudo-code:

```glsl
vec2 sand = texture(prevSand, uv).rg;
float depth = sand.r;
float energy = sand.g;

// decay
depth *= exp(-uDepthDecay * dt);
energy *= exp(-uEnergyDecay * dt);

// optional tiny diffusion
depth = mix(depth, blurredDepth, uDiffusion);

// inject path disturbance
for each sampled path segment:
    float d = distanceToSegment(uv, a.xy, b.xy);
    float trough = smoothstep(bodyRadius, 0.0, d);
    float ridge = smoothstep(ridgeWidth, 0.0, abs(d - ridgeOffset));
    depth += -troughStrength * trough + ridgeStrength * ridge;
    energy += energyStrength * troughOrMotion;

outSand = vec2(clamp(depth, -1.0, 1.0), clamp(energy, 0.0, 1.0));
```

## Composite shader

Final shader:

```text
base sand color from noise texture
darken troughs
highlight ridges
sparkle from energy + noise threshold
fake normal from neighboring depth samples
vignette / editor edge darkening
```

This is where the visual can change. In shader mode, don’t try to preserve the cell-grid look. Make it smoother and more organic:

```text
NanoVG mode: sandy pixel/cell tray
Shader mode: continuous illuminated dune memory
```

That gives the user a quality upgrade, not merely a faster copy.

## Shader visual direction

Use:

```text
- soft continuous troughs
- narrow ridge highlights above/below the body
- very subtle golden flecks only where energy is high
- slight depth-based normal lighting
- no noisy chaos at high Slither Speed
```

The final image should feel like the wyrm is disturbing a **memory field**, not like a particle simulation. Tiny desert organism memory field, properly sovereign.

## Phase 4 acceptance

Target:

```text
CPU SUp: near zero for sand simulation
CPU SDr: near one GL composite cost
GPU visual stable during zoom changes
No interaction changes
No audio changes
Fallback to NanoVG image backend works
Shader compile failure does not break module
```

---

# Codex implementation order

Give Codex this sequence, not all phases at once:

```md
1. Add sand backend/detail/persistence enums and JSON persistence.
2. Refactor WyrmWaveEditor sand code behind drawSandByBackend().
3. Add SandMetrics fields and debug output values.
4. Implement NanoVGCells optimized mode:
   - smaller quality tiers
   - active-cell list
   - stride-based path stamping
   - sparse sparkle drawing
5. Implement NanoVGImage mode:
   - RGBA buffer
   - nvgCreateImageRGBA / nvgUpdateImage
   - one image-pattern fill
   - dynamic pixel shading
6. Make NanoVGImage the default backend.
7. Add OpenGLTexture backend only after NanoVGImage is stable.
8. Add ShaderFeedback backend after GL texture lifecycle is proven safe.
```

---

# Specific Codex instruction block

```md
You are modifying the Wyrm module sand visualization for performance.

Primary goal:
Reduce Wyrm sand rendering cost without changing audio behavior, wavetable behavior, point editing, rock behavior, slither behavior, patch compatibility, or existing editor overlays.

Context:
The current WyrmWaveEditor owns UI-only sand state: sandDepth, sandEnergy, sandBaseNoise, previousWyrmPath, visualSlitherPhase, and drawSandBackground(). The current expensive path draws every sand cell as NanoVG geometry. Existing debug metrics already measure sandUpdateUs, sandDrawUs, editorDrawUs, and audioUs.

Implement in stages.

Stage 1:
Add SandBackend and SandDetail enums. Add backend/detail/persistence fields to Wyrm, persist them to JSON, and expose them in the Wyrm context menu under a Sand submenu. Preserve existing sandViewEnabled behavior.

Stage 2:
Refactor sand rendering inside WyrmWaveEditor:
- drawSandBackground() should dispatch to backend-specific functions.
- keep current code available as NanoVGCells fallback.
- add quality presets Low, Medium, High, Auto.
- reduce default grid sizes.
- add active cell tracking so dynamic drawing does not scan/draw the full grid when mostly at rest.
- de-rate disturbance stamping by path stride in low/medium detail.
- avoid allocations during steady-state draw.

Stage 3:
Implement NanoVGImage backend:
- keep CPU sandDepth/sandEnergy simulation.
- build a persistent RGBA buffer for the sand field.
- create/update one NanoVG image from that buffer.
- draw the sand layer as one image-pattern rectangle.
- regenerate/upload image only when dirty.
- at rest, draw cached image without recomputing pixels.
- use cheap depth-gradient shading to make ridges/troughs look better than the cell renderer.

Stage 4:
Make NanoVGImage the default sand backend. Keep NanoVGCells as fallback/debug.

Do not implement shader feedback yet. Leave TODO seams for OpenGLTexture and ShaderFeedback backends, but do not destabilize the editor.
```

---

# Recommended defaults

```text
Sand View: off for new modules
Sand Backend: NanoVG Image
Sand Detail: Auto
Sand Persistence: Medium
Fallback Backend: NanoVG Cells
```

Because `sandViewEnabled` is already persisted, do not assume “default off” solves the problem for existing patches.

# Final performance target

For the next practical milestone, I’d aim for:

```text
Sand off:
  same as current

Sand on, NanoVGImage, Medium:
  SUp <= 250–350 us
  SDr <= 150–300 us
  total Wyrm UI <= ~1.0–1.3 ms

Sand on, NanoVGCells fallback:
  SDr <= 600–800 us
```

The deeper GPU/shader version is still worth doing, but **NanoVGImage is probably the best immediate move**: it preserves the visual model, collapses thousands of draw calls into one textured fill, keeps Rack lifecycle risk low, and creates a clean bridge to the eventual shader desert.
