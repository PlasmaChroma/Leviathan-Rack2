# Leviathan Render — “Lumen”
## RFC 0.3: An additive, latency-conscious rendering runtime for Leviathan Rack modules

**Date:** 9 September 2026

**Status:** Proposed architecture, refined against current source and existing experiment records. This revision changes documentation only; it implements and benchmarks no Lumen runtime.

**Revision 0.3 decision:** Proceed with the host integration spike and bounded performance pilots. Broader migration depends on measured benefit over the current optimized renderer, acceptable framework overhead, and preserved visual/interaction behavior. Shared code and safer ownership are valuable outcomes, but do not establish a rendering speedup.

**Primary performance target:** The current development machine is the user's laptop with integrated graphics. Establish pilot gates on this machine first; a faster discrete GPU cannot substitute for passing those gates. The GPU model, driver, available memory, and measured bottlenecks remain to be recorded. Integrated graphics is a target constraint, not proof that a particular pass is bandwidth- or shader-bound.

**Source basis:** Local plugin checkout at `b18d37bb85632677dfff63b71c32498561d957cb`, including the retained-capacity edge clear; public Rack v2 and NanoVG source. Implementation must also record the exact Rack SDK, host, driver, and supported platform builds. Branch links in the references are navigation aids, not immutable evidence; resolve plugin sources at this full commit when reproducing the review.

**API status:** All `lr::` names and interfaces below are proposed. They are not existing plugin APIs.

---

## 1. Decision

Build a shared rendering runtime **inside** the plugin. Do not replace Rack’s widget tree, event delivery, controls, SVG/text infrastructure, or host graphics context.

The runtime should own the decisions that currently recur across displays: when geometry changes, when pixels need updating, where render targets live, how zoom affects quality, which work is urgent, how resources survive context changes, and how finished images enter Rack’s composition.

The unit of reuse is a **render contract**, not “everything must be a shader” and not “every module is one framebuffer.” A render contract describes a visual layer, its inputs, dependencies, fidelity policy, update policy, lifetime, and fallback. Its implementation may use NanoVG, plain compatibility OpenGL, a shader, or a combination.

This preserves three independent choices:

| Choice | Examples |
|---|---|
| How pixels are generated | NanoVG path, triangle ribbon, analytic fragment shader, texture sampling |
| How results are retained | Uncached live draw, immutable raster cache, retained geometry, history pair |
| When work executes | On change, while interacting, on capture, bounded animation cadence |

Do not collapse these into a single `renderMode` switch. A shader can be needlessly expensive; a NanoVG layer can be nearly free when cached; an uncached small primitive can cost less than another offscreen pass.

**Success means fewer necessary operations and more predictable operation sizes—not merely moving operations from C++ into GLSL.**

### Explicit non-goals for version 1

No replacement scene graph, new font engine, Vulkan/Metal/WebGPU backend, private GL rendering thread, general-purpose shader language, or giant module-wide cache. Do not require compute shaders, persistent mapping, GPU readback, or a modern desktop GL profile. Do not rewrite working visuals simply to make them conform.

---

## 2. Existing work to preserve

These observations identify starting points, not a claim that every path has been profiled in this review.

| Existing component | Valuable pattern | Proposed destination |
|---|---|---|
| `visual/AdaptiveGlSurface` | Retained front/back capacity with independently selected active raster dimensions | `Surface` allocation and resolution policies [S1] |
| `visual/SettledContourFramebuffer` | Direct current rendering while changing; cached rendering after settlement | A reusable live/settled curve recipe [S2] |
| `visual/SnapshotHistory` | Cache a captured contour, then vary presentation opacity | Immutable snapshot-trail recipe [S3] |
| `visual/PhosphorPreview` | GPU history pair; current contour remains separate | Accumulation-history recipe, with direct fading composition [S4] |
| `WavePreviewGeometryKey` | Normalized contour timing rather than absolute period in the geometry key | Semantic source revision generation [S5] |
| `WyrmRenderGeometry` | Zoom-independent sampling and backend-specific derived-data requirements | Canonical geometry plus optional derived representations [S6] |
| `BifurxRenderData` | Compact sequence-tagged requests and bounded immutable UI/worker payload leases | Shared CPU preparation service [S7] |
| `visual/HaloKnob2` | Independent knob surfaces and a fallback rendering path | Analytic-control recipe without module-wide invalidation [S8] |
| `visual/VisualAssets`, `PreviewSurface` | Split assets, thematic roles, shared preview backgrounds | Existing authoring interfaces retained above the runtime [S9, S10] |
| `gl_validation.md` | Repository-reported stalls from steady-state object validation | Context/lifetime-driven resource validity [S11] |
| `deepcache-review.md` | Bounded preparation, generation checks, and visibility-conscious work | Reusable queue/admission principles, not another preview archive [S12] |

A concrete inconsistency deserves an early audit: `PhosphorPreview::ensureResources()` calls the texture/framebuffer validation helper during rendering, and that helper invokes `glIs*`. This is a risk to investigate against the repository’s validation findings, not a new measured stall diagnosis. [S4, S11, S13]

### 2.1 Current baseline and evidence limits

The migration baseline is the current optimized renderer. Proc, Integral Flux, and Undertow already default to snapshot history. Their loaders force snapshots when `PreviewWidgetOptions` is disabled and migrate unversioned legacy vector defaults; versioned explicit developer choices remain supported when those options are enabled. Proc phosphor remains opt-in, and Integral Flux's optional raw GL preview is a separate path. Preserve these settings and semantics during migration. [S22]

Existing evidence establishes useful local improvements, not a speedup attributable to Lumen:

| Evidence | What it establishes | What it does not establish |
|---|---|---|
| Native hidden-context contour/history benchmarks | Actual backend CPU/GPU costs under specified workloads | Rack frame pacing, all drivers, or whole-module tail latency |
| Flux and Undertow vector/snapshot captures | Lower preview/history CPU submission cost in the recorded modulated workloads | A universal GPU or FPS gain; Proc still needs its own whole-module verification |
| Snapshot first-use capture | Undertow's mode switch incurred approximately 2.7 ms while initially caching six trails | A warm steady-state cost; cold admission needs separate budgeting |
| HaloKnob2 edge report at 119% and shared capacity clear | A source-level remedy for undefined/stale texels outside the active rectangle; Windows build passed | Live confirmation of the remedy at every zoom or every adaptive-surface user |

Use the [experiment tracker](preview-rendering-scratch.md), [Flux analysis](integral-flux-preview-baseline.md), and [Undertow analysis](undertow-preview-baseline.md) as the evidence index. Their dated entries are chronological; later policy entries supersede earlier defaults. The screenshot report motivates the regression in section 14, not a claim of completed GPU image validation.

### 2.2 Remaining performance hypotheses

Settled contours, snapshot trails, and retained surface capacity are part of the baseline. Their historical gains cannot be counted again as Lumen gains. The runtime must either remove additional work or make a specific remaining operation cheaper; moving the same work behind a recipe is an overhead experiment first.

| Cost to investigate | Candidate mechanism | Evidence required before adoption |
|---|---|---|
| Redundant geometry or raster updates | Separate semantic revisions and derived-data caches | Fewer builds/uploads/passes for the same visible changes; unchanged source age |
| Repeated host-state boundaries | Group eligible private-target updates within a module | Fewer guards and lower total submission time, including grouping overhead; correct state restoration |
| Repeated cold shader/resource creation | Share programs and immutable assets within a verified context epoch | Lower creation/reopen cost and resource counts across repeated controls; report warm cost separately |
| Continuously changing curves/displays | Bounded geometry, retained derived representations, smaller uploads | Lower preparation/submission or GPU time under continuous change at matched fidelity |
| Excess pixel work | Tighter effect bounds, fewer passes, admitted raster density | Lower shaded/cleared area and GPU cost; alpha, filtering, and glow comparisons pass |
| Extra presentation work | Direct history composition where semantics permit | Fewer presentation passes without changing fade, ordering, or recording lifetime |

These are hypotheses, not diagnosed bottlenecks. `AdaptiveGlSurface` already returns before its state guard on an unchanged cache hit; guard consolidation targets updates. Its full-capacity clear remains part of update cost even when active shading is smaller. HaloKnob2 owns shader programs per surface, making shared programs a cold-cost candidate; unchanged knobs already reuse cached pixels. [S1, S8]

Every adopted optimization gets a short result record: named workload, suspected cost, proposed mechanism, operation counts before/after, CPU/GPU/frame/source-age results, memory change, fidelity checks, and disposition. An inconclusive result stays inconclusive. Use section 14's comparison protocol to separate framework overhead from the optimization itself.

---

## 3. Required invariants

| ID | Requirement |
|---|---|
| LR-01 | Rack retains authority over widget layout, hit testing, events, draw order, clipping, and host context lifetime. |
| LR-02 | Current user feedback does not wait for a decorative history cadence, worker backlog, or cache-settlement timer. |
| LR-03 | A marker-only update cannot invalidate the source curve, its mesh, or its cached raster. |
| LR-04 | Ordinary zoom changes placement and possibly raster quality, but not semantic curve geometry. |
| LR-05 | Steady-state rendering performs no object-existence validation, synchronous readback, shader compilation caused by changing values, or unbounded application-side allocation. |
| LR-06 | Every mutable surface and every queued CPU job has bounded storage and an explicit overload policy. |
| LR-07 | Every GL resource belongs to a verified graphics-context epoch. A numeric GL name or pointer equality is not sufficient ownership proof. |
| LR-08 | A texture referenced by queued NanoVG presentation remains alive and immutable until the corresponding recording has safely flushed. |
| LR-09 | No pass reads the same texture subresource it is simultaneously rendering into. History feedback uses explicit previous-state resources. |
| LR-10 | A failed recipe falls back locally. It cannot disable unrelated module controls or change audio behavior. |
| LR-11 | A hidden display performs no optional GPU rendering. A dormant, unchanged layer does no geometry preparation, upload, or offscreen redraw. Host composition may still occur. |
| LR-12 | Default migrated visuals preserve appearance, layout, source semantics, and DSP behavior. Deliberate quality reductions are explicit policies. |
| LR-13 | Every offscreen update has a recorded reason and cost category. Measurements distinguish CPU submission from GPU execution. |
| LR-14 | No audio-thread code calls GL, NanoVG, UI methods, filesystem/logging operations, or a potentially blocking preparation service. |
| LR-15 | Existing `Process`, `Step`, and `Draw` telemetry retains module-level meaning. Moving work across widgets or execution phases cannot make that work disappear or charge it to another module. |
| LR-16 | A failed allocation/render attempt cannot publish a partially initialized result or destroy the last usable presentation. Every texel reachable by the declared filter is initialized. |

These are acceptance criteria, not promises that the operating system, host, driver, or GPU can provide hard real-time graphics deadlines.

LR-05 applies to resources whose ownership and lifetime the new runtime can prove. During incremental adoption, retain the checks required by `NvgGraphicsLifecycle` and `GlLifecycleUtils` at legacy boundaries. Replacing recurring driver existence checks with registry/epoch checks requires equivalent lifecycle protection and explicit failure handling; this RFC is not permission to delete validation globally. Extra validation remains a gated diagnostic mode.

---

## 4. Architecture

```text
Rack widgets, controls, SVG anchors, theme settings
                         |
             ModuleVisuals / recipe widgets
                         |
             Source snapshots and revisions
                         |
          Surface contracts + small retained plans
                         |
             RenderDevice admission/execution
                /             |             \
        CPU preparation    GPU resources    Diagnostics
                \             |             /
                    RackRenderBridge
                         |
          Rack-owned GL context and NanoVG composition
```

### 4.1 RackRenderBridge

The only subsystem allowed to interpret host drawing context, frame identity, actual transforms, capture modes, and context lifecycle. It supplies a `RenderScope`, effective pixel mapping, destination classification, and safe resource-retirement opportunities.

It must not replace `Window::step`, call private Rack internals, force host NanoVG flushes, or recursively invoke a Rack framebuffer’s `render()` from arbitrary code.

### 4.2 RenderDevice

One device instance per verified editor/native-context epoch. It owns capability records, program/material caches, resource pools, retirement queues, bounded CPU-preparation coordination, surface registration, and aggregate diagnostics.

Immutable resources may be shared inside that device. Mutable histories, transient state, interaction freshness, and content revisions remain surface-specific. A device must not become one global singleton blindly shared across different Rack instances or editors.

### 4.3 Surface

A surface is a persistent visual result with an explicit contract. It need not own a framebuffer. A live marker surface can be a direct NanoVG primitive; an analytic knob can own a raster target; a trail can reference immutable snapshots.

A surface owns its input revisions, dirty reasons, resolution policy, material reference, output resource leases, and scheduling policy. It does not own the audio module.

### 4.4 RenderPlan

A small retained list of passes with explicit dependencies, prepared when a recipe is constructed or structurally changed. No per-frame general graph reconstruction is required.

Each pass declares reads, writes, target format, active extent, load/clear behavior, and whether its output is persistent. The validator detects feedback hazards and missing writers. The executor can alias scratch storage only when descriptor compatibility and non-overlapping lifetimes are proven.

A “history edge” means previous stored state, not a cycle in the current execution graph. Most displays need only a handful of passes. Version 1 should remain a small scheduling/validation layer, not a game-engine render graph.

### 4.5 Recipes

Recipes encode recurring behavior: `CurvePreview`, `SnapshotTrail`, `PhosphorTrail`, `HaloControl`, `Scope`, `Spectrum`, `ShaderSurface`, `Conduit`, and cached panel decoration.

Most module code should use recipes. Shader authors use a lower-level material/pass interface. Raw GL remains an escape hatch inside a scoped pass.

---

## 5. Host integration and execution phases

Rack’s public source shows that ordinary scene stepping precedes main NanoVG frame recording, and the GL2 window creates a shared offscreen NanoVG context. This establishes useful integration opportunities, but does not by itself certify every Rack Pro or capture path. [S14]

The bridge must be validated against the exact supported SDK and host builds before a migration claims compatibility.

### 5.1 Normal editor path

**Step / prepare.** Recipe widgets collect coherent UI snapshots, compare semantic revisions, and publish bounded preparation requests. Preparation does not render hidden surfaces. Cheap current geometry can be prepared inline; expensive independent preparation can be queued.

**First participating draw (conditional batching path).** All normal scene stepping has completed. After phase 0 proves scope classification, the first participating ordinary editor draw may open a private-target render scope and admit prepared work for registered visible surfaces. This provides an ordinary-widget integration point without inventing a global pre-render hook. The device may sort eligible work by priority, freshness, and cost before restoring the host state. The pilot starts with surface-local execution; editor-wide admission is enabled only after its visibility, attribution, and recording-lifetime rules are verified.

**Present.** Individual widgets submit their resulting images and live overlays in Rack’s existing traversal order. GPU work can be batched independently of presentation ordering because it writes private targets. Nothing allows reordering host-visible alpha composition.

**Urgent local update.** A newly visible or newly changed surface that was absent from the earlier admission set can render locally before its first presentation. If it has already been presented in the same recording, retain that immutable presentation and take the change at the next legal opportunity.

**Maintenance.** A verified pre-recording widget step drains safe retirement queues and advances frame bookkeeping. An ordinary, maintenance-only lifecycle sentinel may be installed once per editor using supported widget child-management APIs to ensure cleanup still runs after the last rendering module is removed. It must not depend on draw-order tricks. Prove its registration, teardown, and plugin-lifetime behavior in the initial integration spike.

Rack exposes a frame-start timestamp; it can help identify normal frame epochs. Capture invocations need a separate scope identity, rather than assuming that every draw advances that timestamp. [S15]

Registration alone does not prove visibility. Use current layout/ancestor visibility and the destination clip where available; previous-frame visibility is only a hint. If another surface's current visibility or destination cannot be established, defer its work until its own draw. Scene stepping can continue while normal window drawing is skipped, so a `step()` callback is not itself permission for optional GPU work. [S14]

### 5.2 Nested and capture path

A draw into a parent framebuffer, a browser preview, and a screenshot may use a different destination context, transform, or call sequence. Do not trigger the entire editor’s pending rendering from one capture. Use a surface-local render scope or a known-safe fallback.

Never call `nvgBeginFrame()` on an already-recording host context. A cached NanoVG recipe that needs offscreen rendering must use a bridge-owned, non-active context with a tested image-sharing contract. If that contract cannot be established for a destination, use direct NanoVG fallback or a deliberately supported capture path.

Browser sources must tolerate `module == nullptr` and provide deterministic defaults. Capture mode must define its time, seed, resolution, and history behavior; repeated calls cannot accidentally deposit history twice.

### 5.3 Host-state boundary

Save and restore the actual destination framebuffer and every state category touched by the chosen backend: viewport, scissor, blend state, color mask, program, buffers, active texture/bindings, vertex attribute or compatibility client state, pixel-store settings, and compatibility matrices where applicable.

Start with a conservative guard. Then reduce its query set through measurement and explicit pass-state declarations. State caching is valid inside an owned scope, not across arbitrary Rack or third-party-plugin draws. Removing `glIs*` polling does not justify omitting necessary host-state restoration.

A pass must also establish the state its operations require. For example, a transparent full-target clear must enable all color write channels and disable inherited scissoring; depth/stencil clears require their own declared masks. Restoring host state afterward cannot repair an incomplete clear. A dedicated offscreen NanoVG context needs any stencil attachment required by its backend; a color-only target is sufficient only for recipes that do not require it.

Rack's default `OpenGlWidget` dirties itself each step. New recipe widgets must not inherit this behavior accidentally. Rack framebuffer nesting and oversampling also require deliberate handling rather than another wrapper layer. [S16, S17]

### 5.4 Host assumptions that block wider adoption

| Question to prove in phase 0 | Conservative behavior until proven |
|---|---|
| Can this callback distinguish ordinary editor recording from nested/browser/capture recording? | Execute only the current surface or use its direct fallback; no editor-wide admission |
| Which event establishes a new native-context epoch, including Pro editor reopen? | Invalidate on every delivered create event; do not infer continuity from equal pointers or successful `glIs*` calls |
| Which maintenance point proves each relevant NanoVG recording has flushed or been canceled? | Keep its presentation leases; bound retained versions and decline further cache updates when that allowance is full |
| Can a lifecycle sentinel safely outlive the last module and unregister before plugin/editor teardown? | Do not deploy a device pool whose cleanup depends on an unverified sentinel |

These are implementation gates. If the supported host cannot supply the evidence, narrow that destination to a proven local/fallback path. A context epoch counter cannot detect an undelivered context transition by itself.

---

## 6. Data model: separate the things that can change

### 6.1 Spaces and extents

Keep these distinct:

| Quantity | Meaning | Typical change |
|---|---|---|
| Semantic geometry | Normalized or module-local source curve | Shape, rise/fall ratio, actual signal content |
| Logical bounds | Rack layout rectangle and effect padding | Panel layout change |
| Presentation mapping | Placement, zoom, transform, clip, destination pixel mapping | Pan, zoom, DPI, capture |
| Active raster extent | Pixels currently used to render the result | Quality-tier change |
| Allocated capacity | Backing texture dimensions | Admission, growth tier, memory reclamation |

A fixed-capacity texture can still rerender constantly. A fixed-resolution surface can still rebuild geometry constantly. Neither is a complete zoom policy by itself.

### 6.2 Revision domains

Use typed monotonically increasing revisions, with an owner-generation identity to reject stale work after object reuse. Suggested domains:

`source`, `geometry`, `material`, `layout`, `overlay`, `history`, `resolution`, `resourceEpoch`.

A source adapter translates raw module values into semantic changes. It must document comparisons and tolerances. Compare toleranced values to the last accepted value so sub-threshold changes accumulate rather than disappear forever.

No blind hashing of large sample arrays every draw. No single `dirty=true` that rebuilds everything. A material change should not rebuild geometry unless the recipe specifically derives geometry from that material.

| Change | Geometry | Cached current curve | History | Live overlay |
|---|---:|---:|---:|---:|
| Moving phase dot | No | No | No | Yes |
| Curve shape changes | Yes | Yes | New capture if due | Yes |
| Color theme changes | No | Material-dependent | Recolor or reset by policy | Yes |
| Rack pan | No | Usually no | No | Placement only |
| Rack zoom within retained quality tier | No | No | No | Placement/pixel sizing |
| Higher-quality raster requested | No | Yes | Separate history policy | Placement/pixel sizing |
| New GL context | Keep CPU data | Recreate | Reset or explicit replay policy | Rebind |

### 6.3 Canonical geometry and derived data

Use one canonical CPU description for the visible curve, marker positioning, hit testing, NanoVG fallback, and shader-derived representations. It need not use the DSP’s internal resolution. A backend may request a simplified polyline, a ribbon mesh, a 1D lookup texture, or extra proximity data.

Cache those derived representations separately. Derive only what the chosen backend needs. When a zoom tier genuinely requires more detail, resample from the canonical curve—not from the previous simplified mesh. A source with unlimited spatial detail must declare an explicit refinement policy; the default should not assume such a source.

---

## 7. Author-facing API

### 7.1 Common case: a recipe and a source adapter

The following is the intended client-code shape, not code that compiles against the current repository:

```cpp
namespace lr = leviathan::render;

// Called once during ModuleWidget construction.
auto* preview = new lr::CurvePreviewWidget;
preview->box = previewBoundsFromExistingAnchors;
preview->setSource(makeProcPreviewSource(module));
preview->setStyle(lr::CurveStyle::leviathan(theme));
preview->setResolution(lr::ResolutionPolicy::interactiveAdaptive());
preview->setHistory(lr::HistoryStyle::snapshotDefault());
preview->setMarker(lr::MarkerStyle::live());
addChild(preview);
```

`makeProcPreviewSource()` is the small module-specific adapter to implement. It provides a stable contour description and revisions, a separately versioned marker, coherent source timing, and a deterministic browser state. Changing from Proc to Integral Flux should primarily change that adapter, not duplicate rendering code.

The illustrative `snapshotDefault()` resolves the existing module's trail style and capture policy; it introduces no new saved default. Phosphor is an explicit alternative. Keep snapshot lifetime, stroke width/color, capture cadence, outgoing-contour semantics, and composition order in the adapter/recipe contract.

The recipe owns stepping, dirty propagation, context events, history scheduling, fallback selection, and composition. Normal module authors do not handle GL names, framebuffer dimensions, texture uploads, or shader failure state.

### 7.2 Snapshot contract

A UI-consumed snapshot should contain these concepts:

```cpp
struct CurveFrame {
    CurveLease curve;          // Immutable, bounded UI-owned data/view.
    uint64_t geometryRevision;
    uint64_t overlayRevision;
    uint64_t ownerGeneration;
    double sampledAt;
    Vec marker;
    bool markerVisible;
    bool interacting;
};
```

`CurveLease` is a proposed ownership-aware view, not an audio-thread `shared_ptr`. Its data remains valid until the recipe relinquishes it. The source connection must detach cleanly when its module/widget owner disappears. Worker descriptors never contain raw widget pointers.

Style resolution, source binding, uniform lookup, and pass-list construction occur at setup or explicit configuration change—not every draw.

### 7.3 Custom shader surface

```cpp
surface.setMaterial(materials::spectrum());
surface.bindTexture(SpectrumSlots::Bins, binsTexture);
surface.setUniform(SpectrumUniforms::Range, range);
surface.setPriority(lr::Priority::Signal);
surface.invalidate(lr::DirtyReason::Source);
```

Slot identifiers are typed/cached bindings, not string lookups in the hot path. A material declares required capabilities, alpha/color semantics, uniforms, read resources, output format, and fallback.

### 7.4 Scoped raw GL

```cpp
void drawCustom(lr::GlPassContext& pass) {
    // The runtime has selected and bound this pass's private target.
    // Viewport, uniforms, and buffers come from owned/validated handles.
    drawExistingModuleGeometry(pass);
}
```

Legacy immediate-mode drawing can be wrapped here first. The callback cannot flush the host, retain the pass context, submit work to another thread, bind framebuffer zero as its destination, or leave undeclared GL mutations behind. It does not create resources during an ordinary warm draw.

### 7.5 Whole-module adaptation

A thin `ModuleVisuals` facade can coordinate shared theme roles, SVG anchors, panel decoration, conduits, and recipe creation. It must not replace Rack controls or introduce a second layout-authoring system.

Provide a `ModuleRenderGroup` for batching private-target work under fewer host-state boundaries. **Grouping execution does not merge surface invalidation or bake independent knobs into one module framebuffer.** Text, emissive layers, and controls retain their established stacking and Rack layer semantics.

Keep the core compatible with the project’s supported compiler/SDK baseline. The public examples intentionally do not require C++20 concepts, `std::span`, or designated initializers.

---

## 8. Zoom and fidelity policies

### 8.1 Three policies, not one universal behavior

Contour settlement and zoom settlement are independent. Preserve the current live-while-changing contour behavior: a geometry or highlight change immediately draws the latest contour directly, and only a quiet interval admits its cache. A zoom-only event can retain the old contour raster under the quality policy. Neither timer delays the marker, label, or direct response to a control change.

**Fixed raster.** Suitable for accumulation history, small effects with known acceptable resolution, and intentionally pixel-based visuals. Zoom only changes presentation until an explicit quality or layout change.

**Quantized adaptive.** Suitable for frequently changing displays. Choose active raster extents from a small set of density tiers, with hysteresis and a declared maximum. Reuse already admitted capacity.

**Interactive adaptive.** During a zoom gesture, preserve the existing raster and show live sharp overlays. After a short quiet interval, refresh at the selected density. The settlement interval delays a costly cache refresh, never the visible response to input.

Initial experimental values could use a 100 ms quiet interval and density tiers such as 0.5, 1, 1.5, 2, and 3. These are tuning parameters, not verified universal defaults. A continuous animated surface may adopt a new active tier immediately within existing capacity while keeping its geometry unchanged.

### 8.2 Capacity and admission

A recipe requests a preferred density and maximum useful density. The device admits a bounded capacity tier based on visibility, priority, and available memory. During warm zoom inside admitted capacity, no new texture allocation is required. Outside that range, preserve the last usable image while admitting a new tier, or use an explicit quality fallback.

Do not allocate every surface at the largest conceivable zoom. Keep a sharper existing raster on zoom-out when affordable; demote only after sustained lower demand or memory pressure. A fixed maximum density necessarily has a fidelity ceiling at extreme enlargement.

The bridge calculates effective physical sampling from the actual destination transform and pixel mapping. It must not multiply Rack zoom or DPI twice. Nonuniform transforms need an explicit policy; default to conservative axis coverage rather than silently treating every transform as a single scalar.

### 8.3 Filtering and bounds

Include effect padding for line fringes, bloom, and filtering. Keep active content bounds distinct from the drawable logical rectangle.

The reviewed adaptive-surface fix clears retained capacity to prevent old texels leaking into the edge through filtering. Preserve that correctness first. [S18]

This protection is needed on first allocation as well as reuse: unused texture storage cannot be assumed transparent. Count cleared pixels separately from shaded pixels. Clearing full retained capacity adds redraw bandwidth, especially for large animated displays, even though it adds no work to an unchanged cache hit. Its cost must remain in the before/after measurement.

An optimization may later clear only active content plus a sufficient initialized guard region, or use a supported active-rectangle sampling clamp. `CLAMP_TO_EDGE` on a whole backing texture is not a substitute for protecting a smaller active rectangle. Mipmapped subrects need additional care; disable mipmaps for such dynamic surfaces until their generation and borders are correct.

Pixel-perfect labels and interaction markers should generally remain live vector/text layers rather than being trapped in a temporarily lower-density curve raster.

---

## 9. Scheduling, source freshness, and audio isolation

### 9.1 Priority and freshness are separate

Suggested classes are `Interaction`, `Signal`, `History`, and `Decoration`. Every task also carries source time, semantic revision, estimated cost, and an optional freshness deadline.

Reserve admission capacity for current interaction. Prefer the newest relevant state rather than executing a FIFO backlog of obsolete visual states. Decorative work ages upward to avoid starvation, but cannot force a burst of catch-up rendering after being hidden.

A single long shader cannot be preempted by this scheduler after submission. Bound pass complexity, covered area, geometry size, and upload volume. Scheduling is not a cure for an unbounded fragment loop or driver compilation stall.

For continuously changing content, each pilot recipe must record its maximum geometry size, upload bytes per update, pass count, active and cleared pixel extents, and supported update cadence. Identify the policy used when demand exceeds those limits. First reduce the cost of necessary work; cadence reduction is a separate quality-policy experiment. Current feedback retains the direct or bounded fallback required by LR-02. A scheduler that merely postpones the same work must be assessed against source age as well as frame time.

As an initial experiment, an editor-wide target might reserve approximately 1 ms of CPU preparation/submission and 2 ms of GPU work for Leviathan at 60 Hz. These are admission-planning values, not a guarantee, per-module allowance, or measured budget. They must adapt to actual hardware and competing host work.

### 9.2 Preparation service

Use a shared, bounded, low-priority CPU pool, not one thread per widget. Keep small geometry operations inline; queuing a tiny curve can increase its latency more than it saves.

Requests contain copied descriptors and bounded immutable UI-side payload leases. Coalesce queued requests by display and semantic revision. Before upload, reject results whose owner generation or relevant source revision no longer matches. Context-independent CPU results can survive context recreation when their source/material requirements remain valid.

Cancellation must happen between bounded chunks. A worker must never dereference a destroyed module/widget or make a GL/NanoVG call. The renderer should not acquire all available CPU cores while Rack’s audio engine is active.

### 9.3 Audio publication

Use a preallocated, well-proven SPSC publication mechanism with explicit slot ownership, or a similarly verified bounded mailbox. No producer may overwrite a slot while the UI reads it. Do not implement a naïve seqlock around concurrently accessed non-atomic C++ payload memory.

Different sources need different overload semantics. A control/shape display can often keep the newest state. A scope or spectral history needs a bounded sample stream with timestamped decimation or min/max aggregation so dropping UI updates does not erase meaningful peaks. Do not apply “latest wins” blindly to all audio samples.

No reference-counted payload destruction, dynamic allocation, locking, waiting, or diagnostics emission may migrate onto the audio thread as a side effect of the publication design.

Do not replace existing audio publication solely to adopt a rendering recipe. First adapt its proven UI snapshot interface. If publication must change, treat that as a separately reviewed and tested concurrency change so renderer measurements do not hide an audio-side regression.

### 9.4 Latency accounting

Track source sampling time, preparation request, CPU completion, upload, and presentation submission. Report source age at submission separately from frame duration.

The normal current-curve/marker path must not intentionally introduce an extra frame queue. Rendering into a back surface is not a requirement to display it one frame later. Within an ordered GL command stream, the completed submission may be sampled by subsequent presentation without CPU waiting.

CPU timestamps do not measure display scanout or end-to-end photon latency. Label the metric accordingly. Never use `glFinish()` to make ordinary performance counters appear more precise.

---

## 10. Histories as first-class temporal recipes

### 10.1 Snapshot trails and phosphor are different effects

A snapshot trail preserves several discrete contours with separate ages. Accumulation phosphor preserves an energy field. Keep both recipes: replacing one with the other can change the visual meaning even when both are called “trails.”

A curve preview’s current contour, marker, grid, history, and labels remain independent. A history capture rate must not limit the current contour’s responsiveness.

### 10.2 Phosphor: store on deposit, fade in final composition

The current implementation already avoids rewriting stored history just to decay it, but still redraws its presentation framebuffer during passive fading. The proposed recipe exposes the stored history texture directly and applies decay in final composition. [S4]

For persistence T defined as time to 1% intensity, use:

```text
integratedAge += elapsed / T_previous
fade = exp(-ln(100) * integratedAge)
```

Use a monotonic elapsed-time source, validate finite positive persistence, and clamp negative elapsed intervals to zero. Audio sample timestamps must be converted into the same time domain before measuring source age.

Between deposits, no history-writing pass and no additional fading presentation target are necessary. The normal Rack image composition scales premultiplied RGB and alpha by the same fade.

On a deposit, materialize decayed previous energy into the other history target, add the new stamp, swap the logical front, and reset its presentation age. Changing persistence integrates the elapsed interval using the previous value before changing the future decay rate. It must not revive expired history or create a brightness jump.

Use timestamps and define deposit energy explicitly: fixed energy per event, or energy proportional to represented elapsed time. Frame-rate-independent decay alone does not make brightness independent of deposit rate. Late deposits must be aged to their intended sample time or deliberately discarded under a documented freshness limit.

For nonlinear tone mapping, spatially varying decay, or multi-component persistence, a single presentation scalar may not be sufficient. Such recipes must declare the additional channels/passes rather than silently changing the effect.

### 10.3 Sleeping and visibility

Maintain a conservative bound on remaining visible energy. Once it falls below the recipe’s threshold, unregister its animation demand without reading pixels back. Keep enough state to accept the next deposit correctly.

Each history declares `ClearOnHide`, `AdvanceAge`, or `KeepBoundedSamples`. Resume at current time. Do not replay a large backlog simply because the user scrolled the module into view.

Changing history resolution has a separate policy: retain existing storage, explicitly resample once, or clear. It must not silently reset every time Rack zoom changes.

---

## 11. GL resources, programs, and buffers

### 11.1 Required resource vocabulary

| Type | Purpose |
|---|---|
| Immutable mesh | Shared quads and stable ribbon/index geometry |
| Stream vertex/index buffer | Bounded changing geometry |
| Image texture | Panel/raster assets and cached pixels |
| Data texture | Curve lookup, spectral bins, packed custom data |
| Render target | Color plus optional declared depth/stencil attachments |
| Scratch target | Temporary pass output eligible for safe reuse |
| History pair | Explicit previous/next temporal storage |
| Presentation lease | Keeps a composed image and its storage valid through host flush |

A history pair and a presentation pair are not automatically both required. Avoid mechanically stacking double buffers around every layer. Reuse or omit presentation targets only when read/write hazards and NanoVG recording lifetime are satisfied.

### 11.2 Compatibility baseline

Keep compatibility OpenGL and GLSL 1.20 recipes as the baseline for this plugin’s existing approach. Optional acceleration is selected from capabilities actually verified on the running context.

Do not assume UBOs, SSBOs, instancing, persistent mapped storage, compute, timer queries, fences, half-float rendering/blending/filtering, multisample resolves, or framebuffer blits. Probe required extensions and formats at controlled resource-creation time. Prefer a simple fallback over an untested fast path.

A fallback material can use NanoVG curves, compatibility triangle ribbons, or RGBA8 storage. It must preserve source meaning, readability, interaction, and default layout.

### 11.3 Upload policy

Stable geometry remains resident. Share an immutable unit quad instead of rebuilding it for every logical size. Use dimensions and transforms as uniforms where appropriate.

For changing buffers, a compatibility implementation may use pre-sized streams with orphan/subdata techniques. Driver-managed storage replacement is not the same thing as promising the driver never allocates. Texture rings similarly reduce immediate reuse pressure but do not prove that the GPU has finished with an older slot.

Where synchronization objects are available, fence reuse and poll without waiting. Where they are not, retain an explicit conservative strategy and measure stalls. Do not introduce unsafe unsynchronized writes merely to satisfy a “no stalls” aspiration. PBO upload paths are optional, measured alternatives—not mandatory improvements.

No live renderer readback is permitted. A future explicit capture/export operation may use a separate, bounded readback path and state clearly when asynchronous support is unavailable.

### 11.4 Shader policy

Cache programs per context and finite material variant. Cache uniform/attribute locations after linking. Zoom, curve values, bloom levels, and texture dimensions must not become shader-source variants.

Compile minimal common programs at a controlled warm-up point and keep a known fallback. On the baseline GL path, compilation/linking may block the UI thread: a scheduler cannot make that work asynchronous by naming it a queued task. Optional parallel-compile support may be used only when present and tested.

An optional shader that fails compilation or allocation backs off rather than retrying every frame. Retain the last valid program during developer reload. Retry after an explicit request or relevant context/capability change.

Prefer bounded coverage geometry for glowing curves over a full-display fragment shader that searches every curve segment for every pixel. A data texture or ribbon representation should be chosen by the recipe’s cost/fidelity model, not by a universal “shader is faster” rule.

---

## 12. Ownership, context changes, and NanoVG interop

### 12.1 Identity model

Use separate identities for device/context epoch, resource generation, surface content revision, and presentation generation. They answer different questions.

A resource handle resolves through the device registry. Reusing a slot increments its generation. Context loss invalidates all old GPU handles while preserving useful CPU descriptions. A context pointer being numerically equal to an old pointer does not resurrect its resources.

### 12.2 Destruction

Normal widget removal retires owned resources and releases CPU payloads even while the editor remains open. Do not rely solely on eventual context destruction to reclaim surfaces.

If a compatible context is current and no unflushed host presentation references remain, delete retired GL objects in a safe maintenance scope. During a valid context-destroy callback, release resources while ownership is still known. If the context is already unavailable, discard names without issuing GL deletion against a replacement context, but still free CPU-side wrappers and registry entries.

The lifecycle sentinel/bridge must make same-editor add/remove churn testable. A resource manager that forgets raw FBO wrapper pointers is not complete lifecycle management.

Rack’s migration guidance specifically warns about editor context changes and context-dependent image/font references. [S19]

### 12.3 NanoVG image identity and leases

Generic NanoVG image IDs are renderer-side handles, not raw GLuint values. Upstream offers wrapping of an existing GL texture and a non-owning texture flag. [S20]

Rack's shared main/offscreen setup means different NanoVG context pointers do **not** automatically imply incompatible image registries. Encode the actual verified sharing relationship in the bridge. For a genuinely independent context, create the appropriate borrowed image wrapper only when the native GL sharing relationship is established; otherwise use fallback.

Rack's `nvgCreateSharedGL2` path is a host-specific sharing contract; generic upstream NanoVG wrapping support does not establish that contract. Check the bundled Rack NanoVG backend and public SDK for the supported build. Native GL texture sharing, NanoVG image-registry sharing, and framebuffer-object ownership are separate capabilities; never infer one from another. Existing ownership helpers remain conservative until the bridge explicitly models the verified relationship.

The wrapper must preserve premultiplied-alpha flags, texture orientation, dimensions, and ownership. A borrowed wrapper cannot delete a texture owned by the render device.

NanoVG records drawing commands for later flushing. [S21] A frame lease therefore protects both image registration and texture contents, not just allocation. Do not overwrite a surface after its earlier image presentation was queued in the same recording. Duplicate draw calls reuse a stable presentation or acquire a different admitted target; they cannot silently mutate an earlier queued image.

GPU completion fences and NanoVG recording leases solve different problems. A fence cannot protect commands that have not yet been emitted by NanoVG.

Use an explicit publication transition: `pending revisions -> admitted writable target -> successfully rendered output -> immutable presentation lease -> safe reuse/retirement`. Commit the output revision and clear its dirty reasons only after a successful render. Allocation failure, context replacement, or a deferred job leaves the latest requested revision pending and the previous valid front available. Coalesce further source changes into that pending revision. Temporary old/new targets and retired-but-leased outputs count against the memory budget; double buffering is not sufficient for an unbounded number of overlapping recordings.

---

## 13. Memory, quality, and failure behavior

### 13.1 Memory accounting

Count allocated capacity, not merely active viewport pixels. Include all persistent fronts, history pairs, scratch targets, depth/stencil, multisample storage, mip levels, upload rings, and CPU mirror payloads. Count shared immutable resources once while resident.

For scale: one 512 × 256 RGBA8 texture is 0.5 MiB; a pair is 1 MiB. An RGBA16F pair at the same dimensions is 2 MiB. Doubling both dimensions multiplies these sizes by four, before any additional targets.

An initial experimental per-editor budget might be 128 MiB, configurable over a range such as 64–256 MiB. This is a starting point for measurement, not an appropriate universal limit for all machines or patches.

On the primary laptop, derive the initial admitted capacity from measured patch behavior and memory pressure rather than adopting 128 MiB as a reservation. Account for retained capacity and transient peaks even when the active viewport is small. Test lower density/effect-cost policies explicitly; any appearance reduction remains a separately reported quality choice, not a matched-fidelity speedup.

Admit visible current controls/curves first. Reclaim unused scratch storage, hidden caches, and cold high-resolution capacity before degrading current interaction. History has its own bounded allowance; it cannot consume the entire device budget.

### 13.2 Quality model

Expose meaningful profiles such as `Full fidelity`, `Adaptive`, and `Compatibility`, with per-recipe detail overrides where useful. Keep fixed limits available for users who value deterministic resource use.

Automatic adaptation may reduce decoration cadence, bloom resolution, or obsolete history work. It must not secretly change DSP, hit-testing, control response, or the meaning of a waveform. Geometry simplification must remain inside an explicit visual-error policy.

Pin default color behavior to the existing appearance. Define premultiplied alpha and the color space of each pass. A future linear-light glow pipeline is an explicit visual change requiring calibration, not a silent global framebuffer setting.

### 13.3 Failure ladder

Within one surface, try the preferred material, then a lower-cost/capability-compatible material, then the existing readable NanoVG/GL fallback. A failed optional effect can disappear while the current curve and controls remain usable.

Reject zero, non-finite, overflowing, or unsupported dimensions before resource calls. Check framebuffer completeness at allocation/recreation. Distinguish diagnostics from ordinary production draws. Record a bounded error event and avoid per-frame repeated logging or retry loops.

---

## 14. Diagnostics and verification

### 14.1 Required counters

Detailed timings, diagnostic rings, A/B controls, and debug-terminal export follow `isDragonKingDebugEnabled()` and the relevant logging option. Keep only the bounded bookkeeping required for normal scheduling/accounting always active. Measure diagnostics-on and diagnostics-off runs so timer/query overhead is visible.

Per surface and aggregated per device, report geometry builds, mesh builds, offscreen passes, composites, upload bytes, capacity bytes, allocations/reallocations, compile/link attempts, cache hits, deferred/discarded work, source age, CPU preparation time, and CPU submission time.

Also count host-state boundaries, submitted geometry, active shaded-area estimates, and cleared pixels. Area counters describe submitted extents, not hardware fragment counts. Separate framework bookkeeping (revision checks, admission, lease/retirement maintenance) from recipe preparation, state setup/restoration, uploads, and pass submission. Include shared maintenance even on frames with no dirty surfaces. Measure aggregate cache-hit traversal so per-surface instrumentation does not dominate the operation being measured.

Record the reason for every update: source, geometry, material, layout, resolution promotion, history deposit, context recreation, or explicit reset. Explainable invalidation is a core debugging feature.

When supported, delayed GPU timer queries report execution cost without waiting for unavailable results. Unavailable results remain pending or are dropped. A CPU wall-clock bracket around GL calls is never labeled GPU time.

Store diagnostics in bounded in-memory counters/rings. Format text, export logs, and write files on explicit inspection, not inside ordinary draws or audio publication.

Every surface/task carries its owning module instance and generation. Preserve the first three Debug Terminal fields as `Process`, `Step`, and `Draw`; append component measurements afterward. If a render group executes work for modules A and B inside C's traversal, attribute A/B rendering to A/B rather than inflating C. Aggregate render work moved into preparation callbacks/layers into the owner's rendering total, with separate phase/component fields that explain where it executed. Avoid double counting inclusive timers; do not sum `Step` and `Draw` into a frame estimate when their documented scopes overlap. Report unassignable shared device work separately instead of inventing a per-module attribution. Compare each changed timing scope against an equally instrumented baseline.

### 14.2 Acceptance matrix

| Test | Required outcome |
|---|---|
| Static visible preview | No geometry rebuild, upload, or offscreen redraw after warm-up; normal composition remains |
| Marker-only animation | Current curve and history storage remain unchanged |
| Continuous zoom inside admitted capacity | No zoom-driven source-geometry rebuild, shader compile, or capacity allocation |
| Large zoom beyond capacity | Previous usable output persists; bounded promotion/fallback; no allocation oscillation |
| Repeated pan/subpixel movement | Correct clip/placement without semantic curve churn |
| Passive phosphor fade | Correct visible decay without history writes or a dedicated fading presentation target |
| History deposit at variable UI rates | Timestamp and energy semantics match documented policy |
| Persistence changed mid-fade | No brightness revival or discontinuity introduced by reinterpreting elapsed time |
| Hidden/minimized editor | No optional GPU rendering; stale jobs canceled/coalesced |
| Remove modules in a live editor | Allocated mutable surfaces return to the accounted baseline after safe retirement |
| Close/reopen editor repeatedly | No stale handles, wrong-context deletion, or unbounded CPU/GPU growth |
| Main, nested, browser, and screenshot rendering | Correct clipping, alpha, label/layer order, orientation, and null-module behavior |
| Forced shader/allocation failure | Local readable fallback; audio and controls continue |
| Worker completes after owner removal | Result safely discarded, no pointer dereference |
| Alpha/filter boundary tests | No stale edge texels, dark fringes, double premultiplication, or bloom clipping |
| Retained target shrinks and regrows | Poison unused texels, render a smaller active rectangle, and verify filtered right/bottom edges remain transparent; repeat on both front/back targets |
| Failed promotion or duplicate draw | Last valid output survives allocation failure; two presentations in one recording cannot observe a rewritten texture |
| Two modules batched inside a third's draw | Per-module totals/counters remain correctly attributed; shared work and overlapping timing scopes are explicit |
| Large patch | Bounded memory and queue sizes; no systematic starvation of late-traversal modules |

Exercise 25%, 50%, 100%, **119%**, 200%, and 400% zoom where supported; values immediately above/below active-size quantization boundaries; DPI/pixel scales 1 and 2 plus supported fractional scales; continuous up/down zoom; different UI frame rates; and both light/dark appearances. Revisit 119% after higher zoom and after a knob change that refreshes a previously sharper cache. Include mixed patches with 1, 8, 32, and 64 affected displays where practical.

Test CPU state machines and temporal math without a GL context. Add a real GL smoke harness for resource reuse, alpha, feedback hazards, and context recreation. Use image comparisons plus optical review for glow/fringe behavior; raw pixel equality alone can over-penalize harmless backend rounding.

Extend the existing [contour benchmark](../tools/proc_preview_render_benchmark.cpp), [snapshot benchmark](../tools/snapshot_history_benchmark.cpp), [phosphor benchmark](../tools/proc_phosphor_benchmark.cpp), and [shared benchmark utility](../tools/preview_benchmark_utils.hpp) before creating another harness. Keep offline readback/image comparisons outside timed regions; the runtime's no-readback rule does not prohibit explicit test verification. Offline runs can establish state-machine and GL-pass behavior; Rack captures still establish traversal, interaction, DPI, capture-mode, and editor-lifecycle behavior.

### 14.3 Performance decision gates

Record p50, p95, p99, maximum frame durations, source age, CPU preparation/submission, GPU execution where available, and allocation counts. Compare identical patches, viewports, quality settings, host versions, and driver environments. Include cold creation/first-use separately from warm behavior.

Before implementing the measured optimization, capture repeated baseline runs and record numerical gates in its experiment record: minimum useful improvement in the named bottleneck, maximum acceptable framework overhead, p95/p99 frame-time regression tolerance, source-age allowance, and memory ceiling. Specify absolute units as well as relative changes where useful, the run duration/repetition count, and observed run-to-run variation. Choose tolerances from the baseline and the intended interaction/frame budget; do not choose them afterward to fit the result. This RFC claims no percentage speedup.

Broader migration requires a repeatable improvement exceeding baseline variation in at least one named target workload, with no material p95/p99 frame-duration or source-age regression across the representative cases. Framework-only cache-hit and continuous-update overhead must remain within the predeclared limits at the largest tested population. A CPU submission improvement can qualify as that specific benefit when host pacing is unchanged, but must not be reported as an FPS or GPU gain. Code reduction alone does not pass the performance gate.

Windows/MSYS2, Linux, and supported Intel/Apple-silicon macOS builds need their own evidence. A fast result on one developer GPU does not validate the plugin-wide default.

Run the first complete pilot on the integrated-graphics laptop. Record GPU/driver identity, display resolution/scaling, power source, power mode, and thermal conditions with each comparison. Match those conditions between configurations and include a sustained run long enough to expose performance changes after warm-up. Use a representative audio-active patch and record audio settings and observed underruns/dropouts alongside rendering measurements; preparation workers must not buy UI throughput at the expense of audio stability. Keep comparisons with other hardware separate.

### 14.4 Controlled comparisons and pilot workloads

Use three comparison configurations at identical visual quality and instrumentation:

| Configuration | Purpose |
|---|---|
| A: Current optimized renderer | Baseline, including existing snapshot and settled-contour policies |
| B: Same rendering algorithm through the minimum Lumen adapter | Isolate framework/ownership overhead while keeping geometry, pass structure, resolution, and cadence equivalent |
| C: B plus one named optimization | Establish the incremental benefit of that mechanism; also compare net cost against A |

If a lifecycle or interop requirement forces B to change pass structure, document the difference and measure its cost separately; do not describe that result as pure bookkeeping overhead. Repeat matched runs, alternate comparison order, and separate cold/warm intervals. Record patch files, deterministic modulation inputs where possible, source/build identifiers, backend/settings, viewport, zoom/DPI, host frame-rate/vsync settings, driver/hardware, and diagnostics state with the results.

Run the following cases at 1, 8, 32, and 64 affected displays where practical. Record both module and surface/control counts: one module can own many surfaces. Include a representative mixed patch so isolated improvements are checked against competing host work.

| Workload | Main question |
|---|---|
| Warm static display | Does the framework preserve zero rebuild/upload/offscreen work with acceptable traversal and maintenance cost? |
| Marker-only animation | Is current feedback fresh while curve/history storage stays unchanged? |
| Continuous shape modulation with full history | Does the pilot help when settlement never occurs, including capture and upload costs? |
| Integral Flux's six HaloKnob2 controls alongside its previews | Does the framework preserve optimized knob caching and independent invalidation while previews update? |
| Zoom/pan and high DPI | Are allocations bounded and tail costs acceptable at large pixel extents and tier transitions? |
| Cold creation, first history capture, and editor reopen | Are initialization bursts, memory peaks, and time to first valid presentation improved or within the agreed limit? |
| Warm cached images with live overlays | How much cost remains in normal composition after offscreen updates disappear? |

Record actual host frame intervals with the measurement method stated, alongside module CPU timing and delayed GPU execution timing where supported. Include time outside instrumented module draws: deferred host NanoVG execution and presentation can remain expensive even when those draws appear cheap. Do not infer host frame time by summing overlapping module timers. If a required measurement is unavailable, record the gap and limit the conclusion; a hidden-context benchmark cannot substitute for host pacing evidence.

For a pixel-cost investigation, sweep admitted density and effect bounds while holding source workload fixed. For a geometry/upload investigation, vary bounded source complexity at fixed raster size. For a boundary/composition investigation, vary surface count with comparable total pixel area where feasible. Keep deliberate quality reductions separate from matched-fidelity results. These controlled changes help select the next optimization without treating CPU and GPU durations as a simple additive frame-cost equation.

### 14.5 First targeted test — Integral Flux

The [pilot capture and analysis guide](integral-flux-lumen-pilot.md) documents the existing logger, offline analyzer, and measurement gaps. Tool readiness does not establish a baseline or certify the host bridge.

Start with one Integral Flux instance on the primary laptop: its six HaloKnob2 controls plus the preview widgets, using the current default renderer and snapshot history. HaloKnob2 is already optimized; its role here is to check framework correctness and overhead in a mixed module. The previews supply the changing-content workload. Do not require a knob optimization to make this test useful.

Compare A and B from section 14.4 first, preserving current rendering algorithms, quality, and update policies. Use fixed zoom/DPI and the same audio-active patch, and repeat these intervals:

1. Observe cold creation separately, then hold contour shapes steady for 30 seconds with live markers active. Verify that marker motion causes no curve rebuilds or knob redraws after settlement.
2. Apply repeatable shape modulation to the preview channels for 30 seconds with snapshot history enabled. Measure geometry preparation, accepted captures, rasterizations, composition, and module-level frame/source-age behavior. Knob updates must follow their own actual input changes, not preview invalidation.
3. Stop modulation and allow histories to expire. Verify return to settled caches and no continuing offscreen work for unchanged content.

Follow with a short manual knob-interaction, zoom, and editor-reopen check for immediate response, independent invalidation, correct edges, and resource recreation. Keep these transition observations separate from fixed-condition timing intervals. Preserve module-level `Process`, `Step`, and `Draw`; append knob, preview, and framework components without double counting.

**Initial pass:** equivalent visuals and interaction, correct lifetime/invalidation behavior, and framework overhead within the predeclared limits. A speedup is not required for this correctness/overhead test. Broader migration still requires section 14.3's measured benefit. Use the results to select one preview optimization for configuration C; increase module count and run the full matrix only after this local test passes. The 30-second intervals are an initial check, not a substitute for sustained thermal or frame-tail validation.

---

## 15. Migration plan

### Phase 0 — Establish the host contract and baseline

Pin the source/SDK revisions. Build a small two-surface integration harness with one live overlay, a shader path, a forced fallback, and an independently owned offscreen target. Prove main/capture context handling, safe image leases, lifecycle sentinel cleanup, state restoration, and frame identity.

Capture baselines for Proc, Integral Flux, HaloKnob2, Bifurx, and Wyrm before changing their implementation. Audit production-path `glIs*` calls separately; do not attribute an improvement from removing them to the new architecture as a whole.

Start with the targeted Integral Flux test in section 14.5, then add Proc or Undertow as the second curve adapter. Expand to the section 14.4 workloads after the local correctness/overhead test passes; defer detailed heavy-display experiments until their migration is proposed. Create an experiment record with the section 14.3 numerical gates before optimizing. This keeps baseline collection useful without making migration of every renderer a prerequisite for learning anything.

Use snapshots and settled contours as the default preview comparison, with phosphor measured separately. Include Undertow when choosing it as the second adapter. Reuse the native Windows harnesses and [documented MINGW64 build](windows_build_from_wsl.md); Linux-only compilation does not validate `plugin.dll`. Do not require an editor-wide scheduler, worker pool, or resource aliasing to complete the first local integration spike.

**Exit:** the bridge is proven for the pilot destinations, measurements are reproducible, and numerical performance gates are recorded. Unproven Pro/capture destinations remain on the documented local/fallback path; listing an unresolved assumption does not certify it.

### Phase 1 — Extract ownership and surface policies

Adapt `AdaptiveGlSurface` behind the new resource/surface interface. Keep current visuals and conservative boundary clearing. Add capacity accounting, explicit epochs, retirement, failure handling, and diagnostics. Avoid new visual features in this phase.

Compare A and B from section 14.4 before adding optimizations. Include cache-hit traversal, continuous updates, retirement maintenance, and cold memory peaks. Keep the minimum adapter small enough that a failed overhead gate can be resolved by removing machinery.

**Exit:** same output, demonstrably bounded lifetimes, correct warm zoom behavior, and framework overhead within the predeclared limits.

### Phase 2 — Prove one portable curve recipe

Migrate Integral Flux's grid/current contour/history/marker separation using snapshot history first. Port that recipe to Proc or Undertow with a second source adapter before adding scheduling machinery. Preserve existing optional renderers through their legacy paths until their own visual/performance comparisons pass, including Integral Flux's raw GL preview and Proc's phosphor option if Proc is selected.

The second module is essential: it prevents a supposedly generic interface from becoming Proc-specific state hidden behind new names. Preserve legacy fallback and an A/B switch until visual and performance gates pass.

For released modules, preserve parameter/input/output/light ordering and existing JSON fields, mode IDs, defaults, and legacy migrations. Renderer caches, resource epochs, and quality-admission state are ephemeral; do not serialize them into patches. A new persisted setting needs an explicit versioned compatibility policy. Developer backend selection stays under the existing Dragon King/preview-options gates.

**Exit:** independent marker updates, shared geometry preparation, correct temporal behavior, and materially less duplicated client code. The curve pilot must also pass sections 14.3–14.4 against the existing optimized renderer, including continuous modulation, host frame tails, source age, memory, and cache-hit overhead. If no useful curve optimization remains, retain the experiment and narrow the extraction; do not use a control-only gain to justify broad curve migration.

### Optional control experiment — Shared Halo resources

After the Integral Flux correctness/overhead test, consider a bounded HaloKnob2 sharing experiment only if measurements justify it. It is not the first required optimization or a prerequisite for the curve pilot. Compare per-surface programs with context-owned shared programs while retaining independent knob caches, existing shader mathematics, and appearance. Measure cold creation/reopen, compile/link counts, resident resources, and warm static/changing controls separately. This experiment does not require the complete `HaloControl` recipe or editor-wide scheduling.

If state-boundary measurements identify material update overhead, test module-local private-target grouping as a separate change after the sharing comparison. Count guards and include grouping/bookkeeping cost; preserve presentation order and local invalidation. Adopt either mechanism only if its own comparison passes. Shared programs may improve cold behavior without improving steady-state rendering, and that is the scope of the claim.

### Phase 3 — Consolidate controls and heavy displays

Use any accepted control-experiment results when moving HaloKnob2 onto shared lifecycle/material resources while preserving independent per-knob caches. Migrate Bifurx’s preparation/upload mechanisms and Wyrm’s derived-geometry/material requirements without combining their distinct visual semantics. Each heavy display needs its own cost hypothesis, bounded continuous-update contract, and section 14 performance comparison before its default changes; curve/control results do not establish its benefit.

A renderer migration is not permission to alter DSP or remove a working backend before comparison.

### Phase 4 — Adopt the authoring facade selectively

Route new panel effects, raised conduits, and shader surfaces through `ModuleVisuals` and recipes. Keep simple existing NanoVG decorations unchanged unless their migration reduces code or fixes a measured issue.

Only then consider optional modern-GL acceleration, immutable control atlases, more advanced scratch aliasing, or a calibrated linear-light glow system.

### Stop conditions

Pause broader migration if the bridge requires undocumented host patching, the second source adapter needs substantial renderer duplication, or the pilot adds latency/memory without improving the measured problem. Simplify the abstraction rather than defending its existence.

Also pause expansion when framework overhead exceeds its gate, continuous-update or host-composition costs erase the targeted benefit, or apparent frame improvements come from older displayed data or unapproved quality reductions. Record a failed/inconclusive experiment and choose the next measured bottleneck. Do not add an editor-wide scheduler, worker pool, or resource aliasing to rescue an unproven local cost model.

---

## 16. Suggested code organization and first implementation backlog

```text
src/render/
    RenderTypes.hpp
    RackRenderBridge.hpp/.cpp
    RenderDevice.hpp/.cpp
    Surface.hpp/.cpp
    RenderPlan.hpp/.cpp
    Resources.hpp/.cpp
    Geometry.hpp/.cpp
    Telemetry.hpp/.cpp
    recipes/
        CurvePreview.hpp/.cpp
        SnapshotTrail.hpp/.cpp
        PhosphorTrail.hpp/.cpp
        HaloControl.hpp/.cpp
        ShaderSurface.hpp/.cpp
```

Treat this as a responsibility map, not a requirement to create every file before the first pilot. Existing `visual/` components can temporarily forward to these implementations. Delete replaced ownership/invalidation code rather than maintaining two competing systems indefinitely.

The first implementation backlog should be narrow: typed context/resource identities; a private-target GL scope; NanoVG image borrowing with presentation leases; one bounded RGBA8 surface pool; fixed and adaptive resolution policies; dirty-reason counters; and one curve recipe with a live marker. Add the global preparation pool and complex scheduling only when the pilot demonstrates the need.

The first reviewable deliverables are the host-contract result, baseline/threshold record, A/B adapter-overhead comparison, and individual curve/control optimization results. Create only the runtime pieces needed to obtain those results; the directory map is not an implementation checklist. Keep the existing renderer selectable through the established developer gates until the relevant performance and visual comparisons pass.

The guiding principle is: **shared infrastructure should remove repeated decisions from module code, while keeping the actual visual mathematics easy to inspect and change.**

---

## Source references

These references support observations about existing implementations and host behavior. The architecture, defaults, APIs, requirements, and migration gates are original proposals in this RFC.

- **S1 — Adaptive surface:** [header](https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/b18d37b/src/visual/AdaptiveGlSurface.hpp), [implementation](https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/b18d37b/src/visual/AdaptiveGlSurface.cpp).
- **S2 — Settled contour:** [SettledContourFramebuffer.hpp](https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/expander/src/visual/SettledContourFramebuffer.hpp).
- **S3 — Snapshot history:** [SnapshotHistory.hpp](https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/b18d37b/src/visual/SnapshotHistory.hpp).
- **S4 — Phosphor history:** [PhosphorPreview.hpp](https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/b18d37b/src/visual/PhosphorPreview.hpp).
- **S5 — Semantic geometry key:** [WavePreviewGeometryKey.hpp](https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/b18d37b/src/WavePreviewGeometryKey.hpp).
- **S6 — Wyrm geometry:** [WyrmRenderGeometry.hpp](https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/expander/src/WyrmRenderGeometry.hpp).
- **S7 — Bifurx preparation data:** [BifurxRenderData.hpp](https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/expander/src/BifurxRenderData.hpp).
- **S8 — Halo control:** [HaloKnob2.cpp](https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/expander/src/visual/HaloKnob2.cpp).
- **S9 — Visual authoring:** [VisualAssets.hpp](https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/expander/src/visual/VisualAssets.hpp).
- **S10 — Shared preview surfaces:** [PreviewSurface.hpp](https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/expander/src/visual/PreviewSurface.hpp).
- **S11 — Repository validation findings:** [gl_validation.md](https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/expander/gl_validation.md).
- **S12 — Preview-cache engineering review:** [deepcache-review.md](https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/expander/deepcache-review.md).
- **S13 — Validation helpers:** [GlLifecycleUtils.cpp](https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/expander/src/GlLifecycleUtils.cpp).
- **S14 — Host execution/context setup:** [Rack v2 Window.cpp](https://raw.githubusercontent.com/VCVRack/Rack/v2/src/window/Window.cpp).
- **S15 — Public window API:** [Rack v2 Window.hpp](https://raw.githubusercontent.com/VCVRack/Rack/v2/include/window/Window.hpp).
- **S16 — Default GL widget behavior:** [Rack v2 OpenGlWidget.cpp](https://raw.githubusercontent.com/VCVRack/Rack/v2/src/widget/OpenGlWidget.cpp).
- **S17 — Host framebuffer behavior:** [Rack v2 FramebufferWidget.cpp](https://raw.githubusercontent.com/VCVRack/Rack/v2/src/widget/FramebufferWidget.cpp).
- **S18 — Retained-capacity edge fix:** [commit b18d37b](https://github.com/PlasmaChroma/Leviathan-Rack2/commit/b18d37b).
- **S19 — Context migration guidance:** [Rack migration to v2](https://vcvrack.com/manual/Migrate2).
- **S20 — Image wrapping and ownership:** [NanoVG GL backend](https://raw.githubusercontent.com/memononen/nanovg/master/src/nanovg_gl.h).
- **S21 — NanoVG recording/flush model:** [NanoVG README](https://github.com/memononen/nanovg).
- **S22 — Current preview defaults and migration:** [Proc.cpp](../src/Proc.cpp), [IntegralFlux.cpp](../src/IntegralFlux.cpp), [Undertow.cpp](../src/Undertow.cpp), and the [dated experiment/policy record](preview-rendering-scratch.md). Local source paths were reviewed at the full plugin commit recorded above; they follow the checkout after subsequent edits.
