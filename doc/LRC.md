Absolutely. I’d frame it as a **performance-first rendering architecture**, with development ergonomics as a secondary dividend rather than the reason for the project.

# Leviathan Shared Rendering System

## High-Level Architecture Specification

### Status

Proposed

### Working Name

**Leviathan Render Core (LRC)**

---

## 1. Purpose

Leviathan Render Core is a shared rendering subsystem for the Leviathan VCV Rack plugin suite.

Its primary purpose is to improve **runtime rendering performance, frame consistency, and perceived UI fluidity inside VCV Rack**, particularly for modules containing many dynamic, procedural, animated, or GPU-rendered visual elements.

The system should reduce redundant rendering work across Leviathan widgets and modules while remaining compatible with Rack's existing rendering model.

A secondary goal is to provide a common visual infrastructure that makes future Leviathan modules easier to implement, optimize, profile, and maintain.

The project is considered successful only if it provides measurable runtime benefit or enables materially richer visuals at equivalent runtime cost.

Architectural cleanliness by itself is not sufficient justification.

---

# 2. Primary Design Principle

> **Share expensive rendering resources globally, consolidate dynamic rendering locally, and leave static content cached for as long as possible.**

The intended structure is:

```text
VCV Rack Rendering System
        │
        │
        ├── Static Rack / NanoVG content
        │
        └── Leviathan modules
                │
                ├── Module A Dynamic Surface
                ├── Module B Dynamic Surface
                └── Module C Dynamic Surface
                         │
                         ▼
                 Leviathan Render Core
                         │
              ┌──────────┼──────────┐
              │          │          │
           Shaders    Geometry   Textures
              │          │          │
              └──────────┼──────────┘
                         │
                        GPU
```

The render core is shared plugin-wide.

Dynamic compositing remains primarily **per module**, respecting Rack's widget hierarchy and avoiding attempts to replace Rack's global renderer.

---

# 3. Goals

## 3.1 Runtime Performance

The highest priority is reducing the rendering cost of active Leviathan modules.

Target improvements include:

* lower CPU time spent preparing visual frames;
* fewer redundant GL state transitions;
* fewer redundant shader compilations and resource allocations;
* fewer independent framebuffer redraws;
* fewer expensive NanoVG operations on continuously changing visuals;
* reduced framebuffer invalidation;
* shared immutable geometry;
* shared shader programs;
* shared textures and atlases;
* efficient dynamic vertex and instance data updates;
* coherent batching of visually related objects;
* graceful handling of large numbers of similar controls.

Particular emphasis should be placed on the difference between:

```text
steady state
```

and:

```text
active interaction / animation
```

Leviathan already performs well when cached widgets remain unchanged.

The largest opportunity is therefore reducing the cost of **continuous redraw and state changes**.

---

## 3.2 Frame-Time Consistency

Average FPS is not sufficient.

The renderer should optimize for consistent visual frame delivery and avoid isolated expensive redraws that produce visible stutter.

Performance evaluation should therefore consider:

* average render time;
* 95th percentile frame time;
* 99th percentile frame time;
* worst-frame latency;
* redraw spikes during parameter movement;
* redraw spikes during module insertion;
* redraw spikes during zoom;
* redraw spikes during window resize;
* redraw behavior inside DAW-hosted Rack.

A renderer that produces 120 FPS on average but periodically generates 40 ms frames should be considered worse than one producing a stable 90 FPS.

Perceived smoothness is the primary metric.

---

## 3.3 Scalability

The rendering architecture should remain efficient as users add more Leviathan modules.

The preferred scaling behavior is:

```text
additional module
      ≈
additional visual workload
```

rather than:

```text
additional module
      =
duplicated renderer infrastructure
+ duplicated shaders
+ duplicated geometry
+ duplicated buffers
+ duplicated setup overhead
```

Repeated visual primitives should share GPU resources whenever technically appropriate.

---

## 3.4 Rich Visuals Without Proportional Cost

The renderer should make it possible to increase visual sophistication without requiring proportional increases in CPU usage.

Examples include:

* animated halos;
* procedural glow;
* gradients;
* particles;
* spectral displays;
* vector-like GPU geometry;
* shader-based glass;
* procedural highlights;
* animated panel ornament;
* 2D and limited 3D rendering.

The intended direction is:

> richer visuals through more efficient rendering rather than richer visuals through more CPU work.

---

## 3.5 Development Consistency

Although secondary to runtime performance, the system should reduce repeated rendering implementation work.

Future modules should be able to consume stable primitives such as:

```text
Halo
Arc
GlowLine
RoundedRect
KnobBody
Indicator
GlassSurface
ParticleField
Spectrum
Waveform
TexturedQuad
```

without reimplementing shaders, lifecycle handling, buffer management, and context restoration.

---

# 4. Non-Goals

The initial system will **not** attempt to:

* replace Rack's global renderer;
* replace GLFW;
* replace Rack's OpenGL context;
* introduce Vulkan or Metal as alternative Rack backends;
* render third-party modules;
* intercept Rack's global scene;
* modify Rack's cable renderer;
* replace Rack's event system;
* combine every Leviathan module into one Rack-wide framebuffer;
* require modern OpenGL features unavailable on supported legacy platforms.

The system should work *with* Rack rather than requiring unsupported host modifications.

---

# 5. Architectural Model

## 5.1 Plugin-Wide Render Core

A single logical rendering subsystem should exist per active GL context.

Conceptually:

```cpp
LeviathanRenderCore
{
    ContextState
    ShaderLibrary
    GeometryCache
    TextureCache
    MaterialLibrary
    DynamicBufferPool
    RenderStats
}
```

Resources that can safely be shared should belong here instead of individual widgets.

Examples:

```text
unit circle geometry
unit quad geometry
rounded rectangle geometry
halo geometry
common shaders
common textures
gradient ramps
noise textures
font atlases where appropriate
```

---

# 6. Per-Module Dynamic Rendering Surface

Complex Leviathan modules should progressively move toward one primary dynamic rendering surface.

Instead of:

```text
Module
 ├── framebuffer
 ├── framebuffer
 ├── framebuffer
 ├── framebuffer
 ├── OpenGL widget
 ├── NanoVG animation
 └── additional framebuffer
```

prefer:

```text
Module
 ├── static/cached panel content
 │
 ├── Leviathan Dynamic Surface
 │     ├── knobs
 │     ├── halos
 │     ├── displays
 │     ├── meters
 │     ├── particles
 │     └── procedural effects
 │
 └── Rack interaction widgets
```

Rack widgets may continue to provide:

* parameter behavior;
* mouse interaction;
* drag handling;
* tooltips;
* context menus;
* focus;
* accessibility-related behavior.

Their visible appearance need not necessarily be rendered by the same widget.

This explicitly separates:

```text
interaction model
```

from:

```text
visual model
```

---

# 7. Static vs Dynamic Rendering

The renderer should aggressively distinguish between static and dynamic content.

## Static Layer

Examples:

* panel artwork;
* labels;
* decorative geometry;
* fixed shadows;
* non-changing ornaments;
* background textures.

These should remain cached and should not participate in ordinary animation frames.

## Dynamic Layer

Examples:

* knob indicators;
* halo arcs;
* scopes;
* waveforms;
* meters;
* moving particles;
* Puffy animation;
* modulation indicators;
* changing procedural effects.

These should use the shared dynamic renderer.

## Principle

A change to one dynamic element should **not invalidate static content**.

Likewise, static art changes should not force reconstruction of unrelated GPU resources.

---

# 8. Shared Shader Library

Shader programs should generally be compiled once per GL context rather than once per widget.

Example conceptual API:

```cpp
auto* program =
    renderCore.shaders().get("halo");
```

Programs should be lazily created and cached.

Potential initial shaders include:

```text
FlatColor
TexturedQuad
SoftCircle
Halo
GlowStroke
Gradient
Spectrum
Particle
```

Shader variants should be minimized.

Prefer parameterized shaders over many slightly different programs when doing so does not significantly complicate rendering.

---

# 9. Shared Geometry

Common shapes should be uploaded once and reused.

Examples:

```text
unit quad
unit circle
ring
arc strip
rounded rectangle
knob shell
indicator wedge
simple line strip
```

Object-specific appearance should preferably be expressed through:

* transformation;
* uniform values;
* vertex attributes;
* textures;
* instance data.

For example:

```text
30 HaloKnobs

should ideally mean

1 knob mesh
1 halo mesh
30 transforms
30 parameter sets
```

rather than 30 independently generated copies of equivalent geometry.

---

# 10. Batching

The renderer should minimize redundant state changes.

Objects using the same:

* shader;
* texture;
* blend mode;
* geometry;
* material

should be rendered together when practical.

A typical module render might conceptually become:

```text
bind halo shader
draw all halos

bind knob shader
draw all knobs

bind indicator shader
draw all indicators

bind display shader
draw displays
```

rather than repeatedly switching renderer state for each widget.

Initial batching does not require true GPU instancing.

Simple coherent draw ordering may provide much of the benefit while retaining wide hardware compatibility.

---

# 11. Optional Instanced Rendering

Hardware-supported instanced rendering may be added as an optimization path.

It must not be required for correct operation.

The renderer should therefore support:

```text
Baseline Path
    conservative OpenGL
    ordinary VBO rendering

Enhanced Path
    instancing
    newer buffer techniques
    optional advanced features
```

Visual output should remain substantially equivalent.

---

# 12. Dynamic Data Updates

Frequently changing data should avoid unnecessary allocations and buffer recreation.

The renderer should investigate:

* persistent reusable CPU-side arrays;
* reusable dynamic VBOs;
* buffer orphaning where appropriate;
* ring-buffered dynamic upload regions;
* fixed-capacity particle buffers;
* dirty ranges;
* parameter/state snapshots.

The render thread should avoid allocating memory during normal frames wherever practical.

---

# 13. Dirty-State Model

Not every dynamic surface needs to redraw continuously.

The system should support at least:

### Cached Surface

Redraw only when visual state changes.

Suitable for:

* controls;
* settings indicators;
* mostly-static procedural visuals.

### Live Surface

Redraw each visual frame.

Suitable for:

* scopes;
* animated particle systems;
* continuously moving displays;
* Puffy;
* high-motion visualization.

### Rate-Limited Surface

Redraw at a configurable visual frequency independent of audio processing.

Example:

```text
audio thread: 48 kHz

display state: continuously updated

visual renderer:
30 / 60 / 90 Hz
```

This may provide major savings for displays where 144 Hz rendering provides no perceptible benefit.

---

# 14. Audio/Render Separation

The rendering architecture must preserve strict separation between DSP and graphics.

The audio thread must never:

* call OpenGL;
* allocate rendering resources;
* wait for the renderer;
* lock a renderer mutex;
* perform visualization geometry generation that can be moved elsewhere.

DSP modules should publish lightweight state snapshots.

Example:

```cpp
struct VisualState {
    float knobValues[8];
    float modulation[8];
    float level[8];
};
```

The renderer consumes the latest available snapshot asynchronously.

Dropped visual updates are preferable to affecting audio timing.

---

# 15. Context Loss and Recreation

DAW-hosted Rack and platform-specific window behavior may destroy and recreate OpenGL contexts.

The renderer must treat this as normal operation.

All GL objects must be associated with a context generation.

On context loss:

```text
GL handles become invalid
```

but logical renderer state should remain recoverable.

On context restoration:

```text
shared resources are rebuilt lazily
```

Modules should not individually implement their own context recovery systems unless absolutely necessary.

Context lifecycle handling should be centralized.

---

# 16. Failure and Compatibility Strategy

The rendering subsystem should degrade gracefully.

A failure in an advanced visual feature must not:

* crash Rack;
* destabilize the audio engine;
* corrupt unrelated modules;
* prevent patches from loading.

Where possible:

```text
advanced renderer unavailable
        ↓
simplified renderer
```

should be preferable to complete module failure.

---

# 17. Instrumentation

The renderer should include internal profiling from the beginning.

Metrics should include:

```text
render calls
draw calls
shader switches
texture binds
geometry uploads
dynamic bytes uploaded
surface redraw count
resource rebuild count
CPU render preparation time
GPU render time if reliably available
```

Optional developer overlays could expose metrics such as:

```text
LRC
CPU: 0.42 ms
Draws: 18
Surfaces: 2
Uploads: 14 KB
Shaders: 4
```

Without instrumentation, performance work will easily become speculative.

---

# 18. Performance Test Methodology

Changes to the shared renderer should be evaluated against repeatable test patches.

Suggested scenarios:

### Test A — Idle

Large Leviathan patch with no changing controls.

Purpose:

Verify that the new system does not regress excellent cached steady-state behavior.

### Test B — Heavy Interaction

Multiple continuously moving HaloKnobs.

Purpose:

Measure the redraw path that currently causes elevated rendering cost.

### Test C — Animated Modules

Puffy, scopes, spectra, meters, or equivalent continuously animated content.

Purpose:

Measure sustained dynamic rendering.

### Test D — Dense Rack

Many Leviathan modules visible simultaneously.

Purpose:

Measure scaling behavior.

### Test E — DAW Host

VCV Rack hosted inside Reaper.

Purpose:

Measure:

* frame pacing;
* context behavior;
* editor resize;
* editor reopen;
* visual smoothness.

This environment deserves explicit testing because perceived stutter may not correlate directly with Rack's reported renderer FPS.

---

# 19. Success Metrics

Exact thresholds should be established after baseline profiling.

Initial desired outcomes:

* no measurable idle-performance regression;
* lower active rendering CPU usage;
* improved 95th/99th percentile frame time;
* reduced redraw spikes;
* fewer GL resource duplicates;
* fewer framebuffer invalidations;
* smoother parameter animation;
* smoother hosted Rack UI;
* predictable scaling with multiple Leviathan modules.

For major migrations such as HaloKnob2, a target such as:

```text
>= 25% reduction in active render CPU cost
```

would constitute a meaningful success.

Larger gains should be pursued where batching opportunities allow them.

If a migration produces less than roughly 5–10% measurable improvement and adds substantial complexity, it should be reconsidered.

---

# 20. Development Ergonomics

The renderer should expose simple primitives rather than require individual modules to manipulate raw GL state.

Example:

```cpp
renderer.halo(...);
renderer.knob(...);
renderer.glowLine(...);
renderer.texturedQuad(...);
renderer.particleField(...);
```

Raw OpenGL should remain available for unusual modules but should not be the default development path.

This provides several secondary benefits:

* consistent visual quality;
* fewer GL lifecycle bugs;
* easier optimization;
* faster module development;
* easier cross-platform compatibility;
* centralized shader improvements;
* easier performance profiling.

Any optimization made to a shared primitive benefits every module using it.

---

# 21. Migration Strategy

The renderer should be introduced incrementally.

Existing working visual systems should not be rewritten wholesale.

## Phase 0 — Render Core Extraction

Use an existing GL-heavy module such as Bifurx.

Extract:

* shader management;
* buffer management;
* texture management;
* context lifecycle;
* common GL utilities.

Visual output should remain unchanged.

Purpose:

Validate shared infrastructure with minimal behavioral risk.

---

## Phase 1 — HaloKnob2 Experiment

HaloKnob2 should serve as the primary performance experiment.

Create a module-level renderer capable of drawing multiple HaloKnobs from shared geometry and shaders.

Compare against the existing widget/framebuffer implementation.

Measure:

* active CPU cost;
* frame pacing;
* draw count;
* framebuffer activity;
* GPU cost;
* visual fidelity.

If this produces meaningful improvement, the architecture is validated.

---

## Phase 2 — Shared Visual Primitives

Generalize successful renderer elements into reusable primitives.

Likely candidates:

```text
Halo
Knob
Arc
Glow
Gradient
Indicator
LED
Glass
Texture
```

---

## Phase 3 — Dynamic Modules

Migrate suitable animated modules.

Candidates include:

```text
Puffy
Bifurx
TD-Scope
Iris displays
future procedural visualizers
```

Each migration should remain independently benchmarkable.

---

## Phase 4 — Advanced Rendering

Only after the architecture proves itself should more ambitious features be explored:

* GPU particle simulation;
* instanced rendering;
* 3D knob geometry;
* shared texture atlases;
* advanced blur/glow;
* physically inspired materials;
* procedural panel animation.

Performance headroom should finance richer visuals rather than richer visuals consuming all newly gained performance.

---

# 22. Guiding Constraints

The renderer should remain:

**Performance-first**

Every abstraction must justify itself against real Rack runtime behavior.

**Incremental**

No flag-day rewrite.

**Observable**

Performance must be measurable.

**Conservative at the host boundary**

Avoid unsupported Rack internals where possible.

**Aggressive inside Leviathan-owned surfaces**

Once inside our rendering domain, optimize freely.

**Audio-safe**

Graphics can never compromise DSP timing.

**Cross-platform**

Linux, Windows, macOS Intel, and macOS ARM must remain first-class targets wherever supported by Rack.

---

# 23. Architectural North Star

The long-term objective is not merely to make individual widgets faster.

It is to move Leviathan away from:

```text
a collection of independent widgets that happen to draw things
```

toward:

```text
a coherent visual engine embedded inside VCV Rack
```

while retaining Rack as the owner of:

```text
windowing
scene composition
module placement
interaction
global UI
```

Leviathan Render Core becomes responsible for efficiently rendering the visual world **inside Leviathan modules**.

The desired result is that a dense Leviathan patch feels fluid even when many visual elements are simultaneously alive.

If the renderer also makes beautiful modules easier to create, that is a significant secondary benefit.

But the governing question for every architectural decision remains:

> **Does this make Rack feel faster, smoother, or visually richer for the same computational cost?**

I’d probably make the **HaloKnob2 benchmark the formal go/no-go gate** for the architecture. It gives us an unusually clean experiment: lots of repeated geometry, known expensive active redraw behavior, and a steady-state implementation that is already good enough that the new system can’t hide behind easy wins. If LRC materially improves *that* case, we’ve probably found something worth propagating across the suite.
