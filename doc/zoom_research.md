# Research Prompt: Zoom-Stable OpenGL/SHDR Rendering in VCV Rack 2

You are conducting a source-level graphics architecture investigation for a VCV Rack 2 plugin named **Leviathan**. Do not jump directly to a patch. First establish exactly how Rack's rendering and framebuffer lifecycle work, then evaluate the plugin's implementation and propose an evidence-backed design.

Use current primary sources wherever possible: the VCV Rack source corresponding to the Rack 2.5-era SDK, official Rack API headers/documentation, NanoVG/NVGLU framebuffer implementation details, and OpenGL specifications or driver documentation where relevant. Clearly distinguish verified behavior from inference. Cite source files, functions, and stable links/commits. If Rack Pro implementation details are unavailable, identify precisely which conclusions depend on closed-source behavior.

## Environment

- Host: VCV Rack 2 Pro on Windows.
- Plugin API/SDK generation: Rack 2.5-era or newer Rack 2 SDK.
- Plugin: Leviathan, currently version 2.9.1.
- Authoritative build: native MSYS2 MINGW64, producing `plugin.dll`.
- Rendering technologies involved:
  - Rack `widget::FramebufferWidget`
  - Rack `widget::OpenGlWidget`
  - NanoVG overlays
  - custom OpenGL 2.1 / GLSL 1.20 rendering
  - NVGLU framebuffers owned by Rack

The principal modules are:

- `src/WyrmRendererGL.cpp`
- `src/WyrmRenderGeometry.hpp`
- `src/WyrmWaveEditor.cpp`
- `src/WyrmWidget.cpp`
- `src/BifurxGL.cpp`
- `src/BifurxUI.cpp`
- `src/GlLifecycleUtils.cpp/.hpp`
- `src/NvgGraphicsLifecycle.cpp/.hpp`

Assume you can inspect the repository and should read those files rather than reasoning only from this prompt.

## Original problem

When Rack zoom changes, custom SHDR/OpenGL displays in Wyrm and Bifurx can appear frozen for a very long time. NanoVG-only modules do not show the same severity.

The freeze is partial rather than global:

- In Wyrm envelope mode, the GL-rendered waveform can remain frozen while its white envelope trigger/progress fill continues animating.
- In Bifurx, the GL-rendered spectrum background can remain frozen while the NanoVG filter-response curve continues updating.

This layer separation indicates that the audio engine, Rack UI traversal, module telemetry, and lightweight NanoVG overlays can remain alive while the GL framebuffer content is stale or rebuilding.

## Experimental changes and observations

Several experimental changes have been made locally. Treat them as experiments, not established final architecture.

### Experiment A: explicit dirty caching

Bifurx previously called `OpenGlWidget::step()`, which dirties its framebuffer every frame. It was changed to call `widget::FramebufferWidget::step()` and rely on its existing explicit invalidation policy (`previewUpdated`, `analysisUpdated`, animation, configuration changes).

Wyrm already follows this cached-framebuffer pattern.

This is believed to be structurally safe, but determine whether it can miss any Rack-managed lifecycle behavior or necessary invalidation.

### Experiment B: remove Wyrm geometry dependence on absolute zoom

`wyrm_render::glBodySampleCount()` previously multiplied its sample budget by absolute zoom, capped at 2x. It was changed so authored GL geometry density no longer depends on Rack zoom. Wyrm also stopped explicitly marking its framebuffer dirty for every fractional change in `getAbsoluteZoom()`.

This prevents zoom changes from changing curve texture length and potentially changing `requiredBodySegmentRadius()`. That matters because `ensureBodyShader(segmentRadius)` currently deletes and synchronously compiles/links a GLSL program when the required radius changes.

Evaluate whether this is correct and whether Wyrm should additionally cache or precompile its finite shader-variant set.

### Experiment C: explicitly cancel Rack zoom in framebuffer render scale

The plugin currently experiments with calling the public API:

```cpp
if (dirty) {
    const float absoluteZoom = std::max(1e-4f, getAbsoluteZoom());
    const float fixedScale = logicalDensity / absoluteZoom;
    render(Vec(fixedScale, fixedScale));
}
widget::FramebufferWidget::draw(args);
```

This is done inside the widget's `draw()` override.

Current logical densities are:

- Bifurx spectrum: 2x.
- Wyrm compact editor: 2x.
- Wyrm expanded editor: 1.5x.

Observed result: zoom responsiveness improved dramatically. Instead of a prolonged freeze, GL content initially became blurred; increasing the fixed logical densities improved clarity. Wyrm slither animation at 1.5x appeared more jittery, plausibly due to raster quantization, so the compact editor was raised to 2x.

However, another observation appeared while loading a patch: some modules—including modules outside Leviathan—could remain visually uninitialized until some combination of zooming caused them to update. It is unknown whether this behavior predates the experiment, is a Rack/driver issue, or is caused by Leviathan disturbing shared framebuffer/GL/NanoVG state.

Do **not** assume causation in either direction. In particular, investigate whether calling `FramebufferWidget::render()` from within that same widget's `draw()` is supported, re-entrant, or capable of corrupting/restoring shared framebuffer, viewport, scissor, NanoVG, or OpenGL state incorrectly.

## Questions that must be answered

### 1. Rack framebuffer lifecycle

Trace the exact Rack implementation of:

- `FramebufferWidget::step()`
- `FramebufferWidget::draw()`
- `FramebufferWidget::render(scale, offset, clipBox)`
- `FramebufferWidget::getFramebufferSize()`
- `FramebufferWidget::deleteFramebuffer()`
- `OpenGlWidget::step()`
- context creation/destruction callbacks

Explain:

- Which method decides the backing texture dimensions?
- Which transforms contribute: widget size, absolute zoom, framebuffer scale, window pixel ratio, oversampling, and caller-provided `render(scale)`?
- What events mark a framebuffer dirty during animated zoom?
- Does Rack recreate the texture/FBO for every intermediate zoom value?
- Does it retain and scale the previous image at any point?
- Is `render()` intended to be called by plugin code, and specifically is calling it from `draw()` safe?
- What GL/NanoVG state does `render()` save and restore?
- Can nested framebuffer rendering affect subsequently drawn widgets or leave other Rack/plugin widgets visually uninitialized?

Provide a state/sequence diagram for a normal frame, a dirty-framebuffer frame, and a zoom-transition frame.

### 2. Explain the original long stalls

Separate and rank the plausible costs:

- allocation/deallocation of framebuffer textures and renderbuffers;
- driver synchronization caused by reallocating an in-use texture;
- repeated full-surface fragment workload as physical resolution grows;
- synchronous shader compilation/linking;
- Wyrm geometry reconstruction and float texture upload;
- Bifurx FFT/curve preparation and VBO/texture uploads;
- NanoVG framebuffer compositing;
- accidental repeated invalidation or feedback loops;
- GL state errors causing failed/incomplete framebuffer output;
- GPU memory pressure or Windows driver behavior.

For each hypothesis, identify observable evidence and a way to measure or falsify it.

### 3. Evaluate current experiments

For Experiments A, B, and C, report:

- correctness;
- expected performance effect;
- visual tradeoffs;
- lifecycle/context risks;
- whether the technique follows Rack's intended API contract;
- whether it can affect modules outside Leviathan;
- keep, revise, or revert recommendation.

Pay special attention to Experiment C. Dramatic responsiveness is valuable, so if its current placement is unsafe, look for a safe way to preserve fixed-resolution behavior rather than merely recommending a return to zoom-coupled framebuffers.

### 4. Design a genuinely zoom-independent surface

Develop at least three viable designs, including:

1. A Rack-native solution using `FramebufferWidget` APIs correctly, if possible.
2. A Leviathan-owned fixed-resolution GL framebuffer/texture solution with explicit lifecycle management.
3. A transition-aware solution that preserves the last completed image while zoom is moving and performs one rebuild after zoom settles.

You may include a fourth hybrid design using coarse resolution tiers.

For each design, specify:

- ownership of texture, framebuffer, and shader programs;
- how the result is composited into Rack/NanoVG;
- behavior during Rack zoom and window pixel-ratio changes;
- behavior during DAW editor close/reopen and GL context replacement;
- how stale handles are detected;
- how GL state is restored;
- handling of standard versus expanded Wyrm editor dimensions;
- expected memory and GPU cost;
- expected visual quality;
- risks to unrelated modules;
- implementation complexity.

The goal is not necessarily perfect sharpness during a zoom gesture. A slightly soft but continuously responsive cached image is acceptable. A multi-second UI stall is not.

### 5. Shader strategy

Investigate Wyrm's segment-radius-specialized body shader.

- Determine the actual finite range and commonly requested radius values.
- Decide whether one bounded shader, a small cached variant set, lazy compilation, or context-creation prewarming is best.
- Quantify the trade between fragment-loop cost and program compilation stalls.
- Ensure programs are context-owned and safely rebuilt after context replacement.
- Avoid compiling during animated zoom or the first critical visible frame if practical.

Also determine whether Bifurx has any zoom-driven shader compilation or only framebuffer/pixel-work scaling.

### 6. Reproduction and instrumentation plan

Design a deterministic test matrix covering:

- empty patch versus a large patch;
- one versus several Leviathan GL modules;
- Wyrm NanoVG, OpenGL, and OpenGL SHDR modes;
- Wyrm oscillator versus envelope mode;
- slither off/on at several rates;
- Wyrm compact versus expanded editor;
- Bifurx fixed GL versus SHDR path;
- active audio spectrum versus idle input;
- slow continuous zoom and rapid zoom-wheel changes;
- patch load at several initial zoom levels;
- Rack standalone versus DAW-hosted Rack Pro editor reopen;
- Leviathan installed versus temporarily removed;
- at least one known third-party framebuffer/OpenGL module.

Instrumentation must preserve Leviathan's Debug Terminal macro contract: the first fields remain `Process`, `Step`, and `Draw`. Add separate metrics after them rather than reinterpreting those fields.

Recommend measurements for:

- framebuffer requested and actual dimensions;
- GL object IDs and context identity/generation;
- dirty cause bitmask;
- framebuffer allocation/reallocation time;
- shader compile/link time and variant key;
- CPU geometry preparation time;
- texture/VBO upload time;
- GL draw submission time;
- asynchronous GPU timer results where supported;
- number of rebuilds during one zoom gesture;
- `glCheckFramebufferStatus()` and bounded `glGetError()` diagnostics gated by `isDragonKingDebugEnabled()`;
- time from patch load/context creation to first valid rendered frame.

Avoid `glFinish()` in normal operation. If used diagnostically to localize asynchronous work, gate it behind explicit developer functionality and explain the distortion it introduces.

### 7. Cross-plugin initialization symptom

Create an explicit fault tree for unrelated modules remaining visually uninitialized until zoom changes. Include:

- a Rack-level dirty/invalidation issue;
- graphics context initialization ordering;
- shared NanoVG or GL state leakage from Leviathan;
- nested/re-entrant framebuffer rendering;
- incomplete FBO or viewport/scissor restoration;
- plugin load ordering;
- GPU memory pressure;
- a driver issue;
- an unrelated pre-existing Rack behavior.

Propose A/B experiments that can attribute responsibility without ambiguity. The highest-value comparison should require no source changes if possible—for example, compare builds with Experiment C enabled versus disabled while holding the patch, Rack version, initial zoom, and launch sequence constant.

## Constraints

- Performance is a primary design goal.
- Do not perform expensive per-sample audio work; this problem belongs entirely to the UI/graphics side.
- NanoVG image handles and OpenGL objects are context-owned.
- Never delete a handle from a different context.
- Context-bound caches must be invalidated and lazily rebuilt after context replacement.
- Avoid destructor-time GL cleanup when the context may no longer be valid.
- Use Leviathan's shared lifecycle helpers where applicable, but do not force them onto resources they do not correctly model.
- Preserve module-level `Process`, `Step`, and `Draw` telemetry semantics.
- Any proposed solution must be safe for standalone Rack and DAW editor close/reopen.
- Do not edit code during the research phase unless explicitly asked. Produce the analysis and implementation plan first.

## Required deliverable

Produce a technical report with:

1. Executive conclusion and confidence level.
2. Verified Rack framebuffer lifecycle, with source citations.
3. Root-cause ranking for the zoom stalls.
4. Safety verdict for each current experiment.
5. Fault tree and A/B plan for the cross-plugin initialization symptom.
6. Comparison table of proposed architectures.
7. Recommended architecture and why it wins.
8. Staged implementation plan with rollback checkpoints.
9. Instrumentation and acceptance-test specification.
10. Any unresolved questions that require live Rack/driver evidence.

Be concrete. Include pseudocode for the recommended render lifecycle, but do not hand-wave over Rack/NanoVG/GL state ownership. The desired outcome is a design that preserves the demonstrated zoom responsiveness without risking shared rendering state or leaving other modules uninitialized.
