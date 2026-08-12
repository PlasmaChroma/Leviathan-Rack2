# Leviathan Render Core

## Architecture Charter

### Status

Active design direction. Implementation remains incremental and is governed by
the measurable gates in the companion plans.

### Working name

**Leviathan Render Core (LRC)**

---

## 1. Executive decision

LRC is not a greenfield renderer and it is not a mandate to put every visual in
one OpenGL surface.

Leviathan already has several mature rendering systems, including cached
NanoVG surfaces, analytic GLSL controls, module-level OpenGL displays,
context-recovery helpers, and debug-gated profiling. LRC exists to turn the
parts that are demonstrably reusable into a context-safe shared foundation
without discarding the update locality and fallback behavior that make the
current implementations reliable.

The governing principle is:

> Share expensive immutable resources per graphics context, choose rendering
> surface granularity from measured update behavior, and keep static content
> cached for as long as possible.

The first architectural proof is no longer whether a shader can outperform the
old HaloKnob2 renderer. That experiment has already succeeded. The next proof
is whether context-safe sharing of HaloKnob2's existing GPU resources improves
duplication, initialization, context recreation, and dense-patch scaling
without regressing its cached steady state or released-module reliability.

### Document map and authority

This file owns LRC's architectural principles, constraints, and milestone
gates. The companion plans own execution details:

- [Baseline and benchmarks](LRC_Baseline_and_Benchmarks.md)
- [Context resource core](LRC_Context_Resource_Core.md)
- [HaloKnob2 sharing experiment](LRC_HaloKnob2_Sharing_Experiment.md)
- [Dynamic surface experiments](LRC_Dynamic_Surface_Experiments.md)

Module-specific rendering documents remain authoritative for their product and
interaction contracts. `doc/haloknob_opt.md` is a valuable historical design
record for the analytic Halo renderer, but its future-tense implementation
steps are no longer the active LRC roadmap.

---

## 2. Purpose and success condition

LRC is a shared rendering foundation for the Leviathan VCV Rack plugin suite.
Its primary purposes are:

- lower active rendering CPU cost;
- improve 95th- and 99th-percentile frame times;
- eliminate avoidable duplicate shaders, buffers, textures, and setup work;
- make graphics-context destruction and recreation routine rather than
  exceptional;
- preserve excellent idle behavior;
- allow richer visuals at equal or lower runtime cost;
- make rendering performance observable and repeatable.

The project is successful only when it produces measured runtime benefit,
reduces a demonstrated reliability risk, or enables materially richer visuals
at equivalent cost. Architectural cleanliness alone is not sufficient.

---

## 3. Current implementation baseline

The repository already contains important pieces of the intended architecture.
Plans and implementation work must begin from this baseline rather than from
the older assumption that LRC is wholly unimplemented.

### 3.1 HaloKnob2

`src/visual/HaloKnob2.cpp` already provides:

- one independently cached `OpenGlWidget` surface per knob;
- an analytic GLSL 1.20 ring, bloom, reflection, and cap composition;
- a CPU-rasterized normal/lit cap atlas;
- explicit dirty-state updates instead of continuous redraw;
- NanoVG fallback and a fixed-GL failure frame;
- context-create/context-destroy recovery;
- optional extra GL validation;
- debug-gated redraw timing and counts.

This implementation delivered the earlier active-redraw performance proof.
The shader program, VBO, and GPU cap texture are still owned by each surface,
which makes HaloKnob2 the cleanest bounded experiment for shared per-context
resources.

The per-knob framebuffer is intentional. It preserves independent invalidation
when only one control moves. LRC must not replace it with a module-sized surface
without evidence that the larger redraw and compositing tradeoff is better.

### 3.2 Module-level OpenGL renderers

Leviathan already has substantial module-level OpenGL systems, including:

- Bifurx spectrum and transfer rendering;
- TD.Scope rendering backends;
- Wyrm's sand renderer;
- Integral Flux preview rendering.

These implementations contain valuable shader, buffer, texture, fallback, and
lifecycle knowledge. They also contain module-specific reset graphs and visual
contracts that should not be forced into a generic abstraction.

### 3.3 Shared lifecycle and cached-surface helpers

Existing shared foundations include:

- `src/NvgGraphicsLifecycle.hpp` for context-owned NanoVG image handles;
- `src/GlLifecycleUtils.hpp` for optional GL resource validity checks;
- `src/visual/PreviewSurface.hpp` for cached preview backgrounds;
- shared raster-image and panel rendering infrastructure;
- debug gating through `isDragonKingDebugEnabled()`;
- the debug terminal for small external diagnostic packets.

LRC extends these established patterns. It does not replace them with a second
competing lifecycle system.

### 3.4 Existing instrumentation

HaloKnob2, Puffy, Integral Flux, Bifurx, and TD.Scope already contain targeted
profiling. LRC should unify metric definitions and benchmark methodology before
attempting to replace every module-local counter.

---

## 4. Proven constraints

The following are treated as established constraints.

### 4.1 Rack owns the host renderer

Rack remains responsible for:

- the window and OpenGL context;
- scene composition;
- module placement;
- interaction and event dispatch;
- global NanoVG rendering;
- framebuffer scheduling;
- third-party modules and cables.

LRC renders only Leviathan-owned content through supported Rack widget and
context boundaries.

### 4.2 Graphics handles are context-owned

GL object names and NanoVG image handles are not portable between contexts.
DAW editor close/reopen can recreate the graphics context while module widgets
survive. A surviving widget may also miss an old context-destroy event.

Therefore:

- every context-create event invalidates inherited numeric GL names;
- resources are rebuilt lazily in a valid draw-time context;
- destructor-time GL cleanup is avoided unless a current owning context is
  guaranteed;
- a handle is never deleted through a different `NVGcontext*`;
- normal drawing does not perform unconditional `glIs*` validation.

### 4.3 Nested Rack framebuffer caches do not compose as independent caches

When Rack draws inside a framebuffer, nested `FramebufferWidget` caches may be
bypassed and their contents drawn directly. LRC must not assume that nesting
cached widgets produces reusable nested textures.

### 4.4 `OpenGlWidget` redraw policy must be explicit

Rack's ordinary `OpenGlWidget::step()` dirties the widget continuously. Cached
OpenGL surfaces that should remain idle must preserve framebuffer caching and
be invalidated only by visual-state changes.

### 4.5 Sharing and batching are different optimizations

Sharing one shader program or unit quad does not batch independent Rack widget
draws. Batching is practical only inside a render surface that owns the relevant
draw ordering and framebuffer. Cross-widget batching must not be promised
without a supported composition design.

### 4.6 Audio timing is inviolable

The audio thread must never:

- call OpenGL or NanoVG;
- allocate graphics resources;
- wait on the renderer;
- lock a renderer mutex;
- generate visualization geometry that can be built off the audio path.

DSP publishes bounded, lightweight snapshots. Rendering consumes the newest
complete snapshot and may drop intermediate visual updates.

---

## 5. Goals

### 5.1 Runtime performance

Target reductions include:

- CPU time preparing active visual frames;
- redundant GL state changes inside a surface;
- shader compilation and link duplication;
- immutable geometry and texture duplication;
- dynamic allocation and buffer recreation;
- unnecessary framebuffer invalidation;
- expensive NanoVG path construction during continuous redraw;
- resource rebuild spikes after context recreation.

Idle and active behavior must be measured separately.

### 5.2 Frame-time consistency

Average FPS is not an adequate result. Evaluation must include:

- average CPU render time;
- 95th- and 99th-percentile frame time;
- worst observed frame;
- redraw spikes during interaction and automation;
- module insertion cost;
- zoom, resize, and rack-pan behavior;
- DAW editor close/reopen and resize behavior.

Stable delivery is preferred over a higher average with visible stalls.

### 5.3 Scalability

Repeated modules and controls should add visual workload, not duplicate all
renderer infrastructure. Immutable resources should be shared when their
lifecycle and visual identity truly match.

### 5.4 Development consistency

Once proven, LRC may expose stable primitives such as textured quads, halos,
arcs, rounded rectangles, glow strokes, waveform spans, and particle fields.
Primitive APIs follow successful migrations; they are not designed in advance
of measured consumers.

---

## 6. Non-goals

The initial LRC will not:

- replace Rack's renderer, GLFW, OpenGL context, or event system;
- render third-party modules or cables;
- combine the entire rack into one Leviathan framebuffer;
- require Vulkan, Metal, compute shaders, or modern-only OpenGL;
- require instancing for correctness;
- force all visuals through OpenGL;
- force every module into one dynamic surface;
- centralize module-specific reset graphs that are clearer locally;
- redesign released module state, parameter IDs, or user behavior as a side
  effect of renderer work.

---

## 7. Architectural model

LRC is a logical subsystem per active graphics context, with four layers.

```text
Rack scene and graphics context
        │
        ├── LRC context/resource layer
        │     ├── shader programs
        │     ├── immutable geometry
        │     ├── immutable textures
        │     └── resource/rebuild statistics
        │
        ├── Leviathan render surfaces
        │     ├── independently cached controls
        │     ├── module dynamic surfaces
        │     ├── live/rate-limited displays
        │     └── NanoVG fallbacks
        │
        └── Rack interaction widgets
              ├── parameters and drag handling
              ├── tooltips and menus
              └── focus and hit testing
```

### 7.1 Context/resource layer

The first shared core is deliberately small. It should own only resources that
are immutable or logically shared for one context generation:

- linked shader programs;
- unit quad and similarly universal geometry;
- immutable texture atlases;
- capability results needed to select a safe path;
- creation, reuse, failure, and rebuild counters.

It must not initially own module state, widget framebuffers, animation clocks,
or interaction state.

The exact context identity and generation mechanism must be proven by the
resource-core plan before a public API is frozen. Pointer identity alone is not
assumed sufficient because host allocators may reuse addresses.

### 7.2 Render surfaces

A render surface owns a coherent visual composition and its dirty policy.
Supported shapes include:

- independently cached control surface;
- cached module subregion;
- module-wide dynamic surface;
- live surface;
- rate-limited surface;
- NanoVG-only cached surface.

Surfaces borrow or lease shared resources but keep their own state and
framebuffer ownership unless a migration proves another design superior.

### 7.3 Interaction widgets

Rack widgets remain the authority for parameters, pointer interaction,
tooltips, context menus, and hit testing. A module surface may draw a control
whose transparent interaction widget remains separate.

### 7.4 Module-specific rendering

Unusual systems may continue to own raw GL code. LRC is the preferred path for
repeated and proven patterns, not a prohibition on specialized rendering.

---

## 8. Surface-granularity decision rule

Surface granularity is selected from evidence, not aesthetic preference.

| Candidate | Prefer it when | Primary risk |
| --- | --- | --- |
| Independently cached control | Elements change independently and occupy small separated areas | More framebuffer composites and per-surface bookkeeping |
| Module subregion | Several nearby elements change together | Partial invalidation may still redraw excess pixels |
| Module-wide dynamic surface | Many visuals update together and can be coherently batched | One small change may redraw a large surface |
| Cached NanoVG surface | Visual changes are infrequent and vector construction is acceptable | Active redraw can become CPU-heavy |
| Live/rate-limited surface | Motion is continuous or visual sampling can be decoupled from UI FPS | Persistent frame cost |

Every migration records:

- changed elements per frame;
- dirty frequency and cause;
- surface pixel area at representative zoom/DPR;
- number of framebuffer composites at idle;
- CPU preparation and draw-submission cost;
- resource duplication;
- fallback behavior.

---

## 9. Dirty-state model

LRC surfaces support explicit policies:

### Cached

Redraw only when visual state changes. Appropriate for controls, settings
indicators, and mostly static procedural content.

### Live

Redraw on each visual frame. Appropriate for genuinely continuous animation,
scopes, and live particle fields.

### Rate-limited

Redraw at a bounded visual frequency independent of audio processing and,
where appropriate, independent of the host UI refresh rate.

Dirty causes should be classifiable during profiling, for example:

```text
value | hover | drag | animation | style | size | zoom | context | asset
```

Repeated publication of equivalent visual state must not dirty a cached
surface.

---

## 10. Lifecycle contract

All LRC work follows these rules:

1. Logical resource descriptions may outlive a context; GL/NVG handles may not.
2. Context creation starts a new generation and abandons inherited names.
3. Context destruction deletes only objects known to belong to the current
   context, then clears the generation's handles.
4. Missed destruction is survivable: the next context-create event clears old
   numeric names without issuing deletion calls through the new context.
5. Resources are lazily created from draw/step-time code with a valid context.
6. Failure is sticky only for the current generation unless retry policy says
   otherwise.
7. Fallback rendering never depends on the failed advanced resource.
8. Normal production draws avoid `glIs*`; extra validation remains debug-gated.
9. Shared resource destruction is explicit and context-aware, not an accidental
   consequence of arbitrary widget destructor order.

The companion resource-core plan owns the exact registry and lease mechanics.

---

## 11. Compatibility and fallback

An advanced rendering failure must not crash Rack, affect audio, corrupt other
modules, or prevent a patch from loading.

Where practical:

```text
shared/advanced path
        ↓ failure
module-local or NanoVG fallback
        ↓ failure
simple bounded visual
```

Visual migrations affecting released modules require:

- unchanged parameter/input/output/light ordering;
- unchanged patch serialization unless explicitly versioned;
- screenshot or manual visual-parity checks;
- standalone Rack testing;
- DAW editor close/reopen testing;
- fallback testing;
- multi-instance testing.

Integral Flux, Proc, Temporal Deck, TD.Scope, and Undertow are released and must
be treated accordingly. A shared HaloKnob2 change can affect several released
modules at once even though it does not alter their DSP.

---

## 12. Instrumentation contract

Instrumentation is part of the architecture, not a later embellishment.

Metrics should distinguish:

### Surface work

- redraw count by policy and dirty cause;
- CPU preparation time;
- framebuffer draw time;
- dynamic bytes uploaded;
- draw calls and state switches where cheaply observable.

### Shared resources

- create, reuse, failure, and rebuild counts;
- live program/buffer/texture counts;
- bytes of immutable and dynamic GPU data where knowable;
- context generation changes;
- fallback activations.

### Frame behavior

- average, p95, p99, and worst frame;
- insertion and first-render spikes;
- context reopen time to first correct frame;
- dropped or rate-limited visual updates.

Production instrumentation must be effectively dormant. Detailed timers,
logs, overlays, and debug-terminal packets remain gated by
`isDragonKingDebugEnabled()` and the relevant logging switch.

GPU timers may be used only if capability checks and non-blocking result
collection are reliable. The renderer must not stall merely to measure itself.

---

## 13. Performance methodology

The authoritative benchmark definition lives in the
[LRC baseline and benchmark plan](LRC_Baseline_and_Benchmarks.md).

Required scenario families are:

- idle cached rack;
- one actively manipulated control;
- simultaneous automation;
- continuously animated modules;
- dense multi-instance rack;
- rack pan and zoom/DPR changes;
- module insertion and removal;
- standalone context behavior;
- DAW editor resize and close/reopen.

WSL is suitable for source checks and focused tests but is not authoritative
for final Windows plugin linking or DAW graphics behavior. Final renderer gates
must be exercised in the Windows/MSYS2 Rack toolchain and the intended DAW host.

---

## 14. Milestone map

Implementation is split into bounded companion plans.

### Milestone 0 — Baseline and benchmark contract

Plan: [LRC baseline and benchmarks](LRC_Baseline_and_Benchmarks.md)

Inventory existing surfaces, establish repeatable scenarios, record current
metrics, and define the evidence format. No renderer architecture is considered
validated without this baseline.

### Milestone 1 — Minimal context/resource core

Plan: [LRC context resource core](LRC_Context_Resource_Core.md)

Implement the smallest context-aware sharing layer needed by one real consumer.
Do not add a generalized material system, dynamic buffer pool, or primitive
catalogue in this milestone.

### Milestone 2 — HaloKnob2 sharing experiment

Plan: [LRC HaloKnob2 sharing experiment](LRC_HaloKnob2_Sharing_Experiment.md)

Share HaloKnob2's program, immutable quad, and GPU cap texture while preserving
per-knob framebuffers, state, fallback, and dirty behavior.

This is the formal LRC go/no-go gate.

### Milestone 3 — Selective dynamic-surface experiments

Plan: [LRC dynamic surface experiments](LRC_Dynamic_Surface_Experiments.md)

Evaluate Bifurx, Puffy, TD.Scope, Wyrm, and other candidates individually.
Prefer unreleased modules for high-risk experiments. Released-module migration
requires a separate compatibility gate.

### Milestone 4 — Proven primitive extraction

Only after at least two consumers demonstrate the same stable need should LRC
extract reusable draw primitives, material descriptions, dynamic upload
helpers, or optional instancing.

---

## 15. Formal go/no-go criteria

LRC proceeds beyond the Halo sharing experiment only if all of the following
hold:

- no measurable idle-frame regression;
- no visual or interaction regression in Halo consumers;
- correct standalone and DAW context recreation;
- reliable fallback after forced initialization failure;
- demonstrably fewer duplicate GPU resources in multi-knob/multi-module cases;
- lower insertion or context-rebuild cost, or another measured scaling benefit;
- no new production per-frame validation or allocation;
- complexity remains bounded enough that module-local rendering is still easy
  to reason about.

If resource sharing produces negligible benefit, unreliable lifetime behavior,
or hard-to-debug cross-module coupling, the correct result is to retain the
proven per-surface implementation and stop broad LRC centralization.

Dynamic-surface experiments have their own gate: a migration producing less
than roughly 5–10% meaningful improvement while adding substantial complexity
should be reconsidered. A 25% or greater reduction in the targeted active cost
is a strong result, but thresholds must be tied to the baseline and scenario.

---

## 16. Open decisions

The following remain intentionally unresolved until the companion plans gather
evidence:

- the safest context identity and generation mechanism available through Rack;
- whether shared resources are held by explicit leases, generation-owned
  registries, or another non-blocking UI-thread mechanism;
- how shared resources are retired when no context-destroy event is observed;
- whether GPU timer queries are reliable on the supported host matrix;
- which renderer becomes the second shared-core consumer;
- whether any candidate actually benefits from module-wide dynamic batching;
- the minimum supported GL capability baseline beyond the existing GLSL 1.20
  paths;
- whether a user-facing renderer fallback is needed beyond automatic fallback
  and debug controls.

These are design questions, not permission to build a speculative framework.

---

## 17. Architectural north star

The long-term objective is a coherent Leviathan visual engine embedded inside
Rack, not a second global renderer and not a forced rewrite of successful
widgets.

The desired result is:

```text
shared context-safe foundations
        +
measured surface granularity
        +
strict static/dynamic separation
        +
audio-safe visual snapshots
        +
automatic bounded fallback
        =
a dense Leviathan rack that remains fluid and recoverable
```

Every architectural decision returns to one question:

> Does this make Rack faster, smoother, more reliable, or visually richer for
> the same computational cost—and can we demonstrate it?
