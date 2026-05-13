# Wyrm Sand Optimization Design

## Completion Status

Last updated: 2026-05-13

Implementation checklist:

- [x] Keep the `WyrmSand` split as the implementation boundary.
- [x] Add sand backend/detail/persistence enums in `Wyrm.hpp`.
- [x] Persist sand backend/detail/persistence in `Wyrm::dataToJson()` / `Wyrm::dataFromJson()`.
- [x] Add `Sand` context submenu with `Sand View`, `Backend`, `Detail`, and `Persistence`.
- [ ] Add backend dispatch inside `WyrmSand::draw()`.
- [ ] Keep current cell renderer as explicit `NanoVGCells` backend implementation.
- [ ] Add detail presets and deterministic grid sizing to `WyrmSand::ensureField()`.
- [ ] Add `NanoVGImage` state, lifecycle, and one-image draw path to `WyrmSand`.
- [ ] Make `NanoVGImage` the default backend in runtime behavior.
- [ ] Add active cell tracking for dirty decisions and later active-only decay.
- [ ] Add path stride by detail.
- [ ] Extend debug metrics (`sandBackend`, `sandDetail`, `sandCellCount`, `sandActiveCellCount`, `sandImageUploadUs`).
- [ ] Evaluate `OpenGLTexture` backend only after `NanoVGImage` is stable.
- [ ] Evaluate `ShaderFeedback` backend only after GL texture lifecycle is proven safe.

## Current Baseline

Wyrm's sand view is UI-only and is now split between `WyrmWaveEditor` and `WyrmSand`.

`WyrmWaveEditor` owns editor-specific inputs:

- `visualSlitherPhase`
- displayed Wyrm path generation
- point/rock interaction stamps
- the `sandEnabled()` gate

`WyrmSand` owns sand state and rendering:

- `depth`
- `energy`
- `baseNoise`
- `previousPath`
- `ensureField()`
- `stamp()`
- `disturbSegment()`
- `update()`
- `draw()`

The audio path is separate and should stay untouched.

The current expensive behavior is the NanoVG cell renderer:

```text
for every sand cell:
  compute shade
  draw one NanoVG rect
  maybe draw one sparkle circle
```

The current grid is sized from the editor bounds:

```cpp
targetW = clamp(int(size.x * 0.65f), 64, 128);
targetH = clamp(int(size.y * 0.65f), 32, 72);
```

That can mean several thousand NanoVG draw operations per frame. The first optimization target should be sand rendering, not Wyrm body/waveform/rock drawing.

## Design Rule

`WyrmSand` should become the replaceable visual backend boundary. The rest of the editor remains NanoVG:

```text
Keep as-is:
- waveform columns
- Wyrm body
- point hover guides
- rocks
- drag arrows
- editor interaction overlays

Sand backend candidates:
- NanoVGCells
- NanoVGImage
- OpenGLTexture
- ShaderFeedback
```

Do not start by shader-rendering the whole editor. Keep the backend boundary around the sand layer only.

The current `WyrmWaveEditor::drawSandBackground()` wrapper should stay thin:

```cpp
void drawSandBackground(NVGcontext* vg) {
	sand.draw(vg, box.size, sandEnabled());
}
```

Backend dispatch should happen inside `WyrmSand`, not by re-growing sand implementation details inside `WyrmWaveEditor`.

## Recommended Backend Path

The next practical target is `NanoVGImage`.

Reasoning:

- It preserves the current CPU sand simulation model.
- It collapses thousands of NanoVG cell draws into one image-pattern fill.
- It has lower lifecycle risk than adding a new GL renderer.
- It creates a clean fallback path for future GL work.

`NanoVGCells` should remain as a fallback/debug backend, but it should not be the optimized default.

## Wyrm Settings

Add these enums near the existing Wyrm UI state in `Wyrm.hpp`:

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

enum WyrmSandPersistence {
	WYRMSAND_PERSISTENCE_SHORT = 0,
	WYRMSAND_PERSISTENCE_MEDIUM = 1,
	WYRMSAND_PERSISTENCE_LONG = 2
};
```

Add state:

```cpp
std::atomic<int> sandBackend {WYRMSAND_NANOVG_IMAGE};
std::atomic<int> sandDetail {WYRMSAND_DETAIL_AUTO};
std::atomic<int> sandPersistence {WYRMSAND_PERSISTENCE_MEDIUM};
```

Persist these beside `sandViewEnabled`. Do not change existing patch behavior for `sandViewEnabled`.

Expose them under a Wyrm context-menu `Sand` submenu:

```text
Sand View
Sand Backend
  NanoVG Image
  NanoVG Cells
Sand Detail
  Auto
  Low
  Medium
  High
Sand Persistence
  Short
  Medium
  Long
```

Do not expose `OpenGLTexture` or `ShaderFeedback` until those paths exist and have fallback behavior.

## Detail Presets

Replace implicit grid sizing with deterministic presets:

```cpp
struct SandGridSpec {
	int w;
	int h;
	int pathStride;
	float activeThreshold;
	bool drawSparkles;
};

SandGridSpec sandGridForDetail(int detail, float recentUiMs) {
	switch (detail) {
		case WYRMSAND_DETAIL_LOW:
			return {48, 24, 3, 0.040f, false};
		case WYRMSAND_DETAIL_MEDIUM:
			return {64, 32, 2, 0.030f, true};
		case WYRMSAND_DETAIL_HIGH:
			return {96, 48, 1, 0.020f, true};
		case WYRMSAND_DETAIL_AUTO:
		default:
			if (recentUiMs > 2.0f) return {48, 24, 3, 0.045f, false};
			if (recentUiMs > 1.2f) return {64, 32, 2, 0.035f, false};
			return {96, 48, 1, 0.025f, true};
	}
}
```

If a stable recent UI timing value is not available yet, `Auto` should initially behave like `Medium`. Adaptive behavior can be enabled after metrics are wired.

Avoid `128x72` as a normal mode. It is expensive for a small Rack editor rectangle and should only be considered for a hidden/debug mode.

## NanoVGImage Backend

Keep the existing CPU field:

```text
WyrmSand::depth[]
WyrmSand::energy[]
WyrmSand::baseNoise[]
```

Add image state to `WyrmSand`:

```cpp
std::vector<unsigned char> sandRgba;
int sandImage = -1;
int sandImageW = 0;
int sandImageH = 0;
bool sandImageDirty = true;
bool sandImageValid = false;
```

Release `sandImage` with `nvgDeleteImage()` when a valid NanoVG context is available.

If direct cleanup from `WyrmSand` is not practical because the NanoVG context is not available at destruction time, add an explicit `WyrmSand::destroyImage(NVGcontext* vg)` and call it from the widget lifecycle path that has access to `vg`.

Rendering flow:

```text
WyrmWaveEditor builds current displayed path
WyrmSand::ensureField(size)
WyrmSand::update(size, nowSec, currentPath, pathCount, slitherAmount)
shadeSandImageIfDirty()
create/update NanoVG image
draw one image-pattern rectangle
draw normal editor overlays
```

Representative structure:

```cpp
void WyrmSand::draw(NVGcontext* vg, Vec size, bool enabled) {
	if (!enabled) {
		drawFlatBackground(vg, size);
		return;
	}

	switch (currentSandBackend()) {
		case WYRMSAND_NANOVG_IMAGE:
			drawNanoVGImage(vg, size);
			break;
		case WYRMSAND_NANOVG_CELLS:
		default:
			drawCells(vg, size);
			break;
	}
}
```

If image creation or update fails, fall back to `drawCells(vg, size)` for that frame and mark the image backend invalid.

Pixel shading can be richer than the current cell fill without adding draw calls:

```cpp
float hL = depthAt(x - 1, y);
float hR = depthAt(x + 1, y);
float hU = depthAt(x, y - 1);
float hD = depthAt(x, y + 1);

float normalLight = 0.5f + 0.35f * ((hL - hR) + 0.6f * (hU - hD));
float shade = base + depthBrightness + energyGlow + normalLight;
```

Dirty image policy:

```text
Dirty when:
- sand grid changes
- sand backend/detail/persistence changes
- slither disturbance updates cells
- point edit stamps cells
- rock drag/landing stamps cells
- active cells are decaying

Not dirty when:
- sand is enabled but fully at rest
- only non-sand editor overlays change
```

At rest, the backend should draw the cached image without recomputing pixels.

Persistence should map only to decay constants, not to a different simulation model:

```text
Short:  faster depth/energy decay
Medium: current visual feel
Long:   slower depth/energy decay
```

Changing persistence should mark the sand image dirty but should not reset the field unless the implementation cannot preserve the existing state cleanly.

## Active Cell Tracking

Active cell tracking is still useful, but it should support both simulation and image-dirty decisions.

Add:

```cpp
std::vector<int> sandActiveCells;
std::vector<uint8_t> sandActiveFlags;
int sandActiveCompactCountdown = 0;
```

When `WyrmSand::stamp()` or `WyrmSand::disturbSegment()` changes a cell above threshold, mark it active:

```cpp
void markSandActive(int idx) {
	if (!sandActiveFlags[idx]) {
		sandActiveFlags[idx] = 1;
		sandActiveCells.push_back(idx);
	}
	sandImageDirty = true;
}
```

Decay can initially remain full-grid for simplicity. Once `NanoVGImage` is stable, move decay to the active list:

```cpp
for (int idx : sandActiveCells) {
	depth[idx] *= depthDecay;
	energy[idx] *= energyDecay;
	if (std::fabs(depth[idx]) + energy[idx] < retireThreshold) {
		sandActiveFlags[idx] = 0;
	}
}
```

Compact the active list every 8-16 frames, not every draw.

## Path Stamping

Sand disturbance does not need every displayed point segment in every detail mode.

Use the detail preset's `pathStride`:

```cpp
for (int i = 0; i < count - pathStride; i += pathStride) {
	disturbSegment(size, currentPath[i], currentPath[i + pathStride], troughStrength, ridgeStrength, energyStrength);
}
```

This is acceptable because the disturbance radius is wider than a single point segment at normal editor sizes.

## Metrics

Existing debug metrics include:

```text
sandUpdateUs
sandDrawUs
editorDrawUs
audioUs
sandEnabled
bodySampleCount
pointCount
rockCount
```

Extend debug output when implementing backend/detail support:

```text
sandBackend
sandDetail
sandCellCount
sandActiveCellCount
sandImageUploadUs
```

Keep audio metrics separate. Sand optimization should not change `process()`.

## Implementation Order

1. Keep the current `WyrmSand` split as the implementation boundary. Status: done.
2. Add sand backend/detail/persistence enums, JSON persistence, and menu entries. Status: done.
3. Add backend dispatch inside `WyrmSand::draw()`. Status: pending.
4. Keep current cell renderer as `NanoVGCells`.
5. Add detail presets and deterministic grid sizing to `WyrmSand::ensureField()`.
6. Add `NanoVGImage` state, image lifecycle, and one-image draw path to `WyrmSand`.
7. Make `NanoVGImage` the default backend.
8. Add active cell tracking for dirty decisions and later active-only decay.
9. Add path stride by detail.
10. Extend debug metrics.
11. Consider `OpenGLTexture` only after `NanoVGImage` is stable.
12. Consider `ShaderFeedback` only after GL texture lifecycle is proven safe.

## Acceptance Targets

Behavior:

```text
No audio behavior changes.
No wavetable behavior changes.
No point editing changes.
No rock interaction changes.
No slither behavior changes.
Existing patches restore sand view correctly.
Sand off remains near current baseline.
No steady-state draw allocations.
```

Visual:

```text
At rest: still granular.
Slither: still leaves trough/ridge memory.
Manual point edits still stamp visible sand.
Rock dragging and landing still stamp visible sand.
NanoVGCells remains available as fallback/debug.
```

Performance target for the next practical milestone:

```text
Sand on, NanoVGImage, Medium:
  SUp <= 250-350 us
  SDr <= 150-300 us
  total Wyrm UI <= ~1.0-1.3 ms

Sand on, NanoVGCells fallback:
  SDr <= 600-800 us
```

The immediate recommendation is to implement `NanoVGImage` before any direct OpenGL or shader-feedback work.
