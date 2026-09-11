# Research brief: a low-overhead custom shader architecture for VCV Rack

## Request

Please conduct deep technical research into the best execution architecture for custom shader rendering inside a third-party VCV Rack 2 plugin. We are developing a reusable rendering layer called **Lumin**. We have a working analytic curve shader, but entering and leaving our custom GL rendering path costs substantially more CPU time than submitting the shader itself.

**The central question: can we avoid the expensive host/custom-render handoff, or must we amortize it?**

Do not assume our current offscreen-surface design is the right foundation. Investigate alternative integration points in Rack and NanoVG before recommending further optimization within that design. Deliver a source-backed recommendation and a small experiment plan that can discriminate between the leading options. Implementation and live tests of our current candidate will continue independently while you research.

## Product goal and constraints

- A full shader-based replacement is the eventual goal, with approximately the same visual quality or better and lower total cost during animation/modulation. Switching back to NanoVG whenever parameters move is not the intended solution.
- The first test bed is the two function-curve preview widgets in Integral Flux. Proc uses related function curves. Longer term, the execution infrastructure should benefit other modules and widgets across the plugin.
- Native Windows VCV Rack 2 is the primary build/runtime environment. The installed application is Rack 2 Pro. Existing tests use real GL2/NanoVG in a hidden GLFW window with limited Rack host fixtures. Do not assume a particular Rack minor version or available modern GL extension without checking.
- The code uses GLSL 1.20. Recommendations requiring newer GL should identify availability, portability, and fallback implications explicitly. Desktop driver and DAW editor lifecycle behavior matter.
- This must work as a distributable third-party plugin. Replacing Rack's bundled NanoVG, patching the Rack executable, depending on private host ABI, or modifying every other plugin is not an acceptable shipping dependency. Such options may be discussed separately for context.
- Preserve drawing order, clipping, alpha, Rack zoom, fractional pixel placement, multiple module instances, and graphics context recovery after editor close/reopen.
- Performance includes CPU geometry/preparation, render submission, driver waits, GPU work, image composition, and cache behavior. A faster isolated shader is insufficient if total module/frame cost rises.
- Existing released module parameters and patch serialization must remain compatible.

## Current implementation

### Curve rendering

The analytic shader draws a quad for each preview. Fragment work includes conservative rejection and a closest-point approximation to the rising/falling function curves. It supports Flux's Shark Fin mode and independent edge highlighting. It uses prepared shape constants and two solver iterations.

There is one generic vertex attribute (location 0), a static quad VBO, shader uniforms, premultiplied-alpha blending, and no sampled texture in the curve shader itself. The current shader is not the primary target of this research.

### Execution and presentation

The newly built candidate works approximately as follows:

```text
IntegralFluxWidget::draw
  start total module Draw timing
  inspect both visible preview widgets and their cache keys
  prepare requests for changed contours
  if any changed:
    capture host GL state once
    for each dirty preview:
      establish shader baseline
      ensure/validate its offscreen target
      bind and clear target
      submit analytic curve shader
      swap front/back target
    restore host GL state once
  normal Rack ModuleWidget::draw
    each preview draws its existing history
    presents its cached contour image through NanoVG
    draws existing tracer ball/overlays
  finish total module Draw timing
```

The two previews share the shader program/VBO but retain independent cached targets and dirty decisions. This is **one state boundary, two target updates, and two shader draws** when both change. It is not a single atlas or one draw call. Cached frames skip the custom GL boundary entirely.

Offscreen targets use NanoVG's framebuffer helpers and context-owned image handles. The current surface implementation double-buffers and retains peak capacity. It validates resources, handles context epochs, and prevents rewriting an image already queued for presentation during the same host frame. Presentation preserves subpixel alignment.

The normal pilot admits the main Rack NanoVG context, current expected GLFW context, uniform positive scale without rotation, and no nested framebuffer draw. Unsupported destinations/transforms retain a fallback. Research should determine whether these restrictions can be safely relaxed.

### State guard

The earlier general guard used `glPushAttrib(GL_ALL_ATTRIB_BITS)`, `glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS)`, projection/modelview stack operations, explicit binding queries/restores, and generic vertex attribute queries/restores for locations 0 through 3.

The new opt-in shader-only guard:

- Preserves enable, color-buffer, viewport, polygon, and scissor attribute groups.
- Preserves pixel-store client state.
- Explicitly saves/restores draw/read framebuffer, renderbuffer, program, array-buffer, active texture unit, texture bindings for the incoming unit and unit 0, pixel-unpack buffer, and generic attributes 0 through 3.
- Does not touch matrix stacks or legacy client arrays.
- Establishes a neutral shader baseline before each callback: bindings/unpack settings, disabled generic attributes, disabled depth/stencil/cull/scissor/alpha tests, premultiplied blending, and filled polygons.

Other surface users retain the general guard. A mixed batch uses the general guard. This is still a substantial state boundary; it has not established the minimum state contract Rack actually requires.

**Important unknown:** CPU time attributed to capture/restore can include deferred driver work or synchronization. We have not proved that the apparent cost is simply copying state. Please investigate this distinction explicitly.

## Evidence to interpret carefully

### First live Rack capture: earlier per-preview full guards

For 520 frames in the latest capture where both shader surfaces updated:

| CPU scope | Median |
|---|---:|
| Full module widget Draw | 788.65 microseconds |
| Both previews, including existing histories/overlays | 550.6 microseconds |
| Current contour path | 482.65 microseconds |
| Sum of both capture/restore scopes, calculated per frame | 427.5 microseconds |
| Sum of both shader callback scopes, calculated per frame | 20.5 microseconds |

The median per-frame state-scope share of contour CPU time was 90.85%. An earlier capture showed a similar concentration. These are nested CPU scopes, not GPU execution timings; medians are not additive.

There were zero fallback presentations in the captured pilot-on sections. The user initially saw no quality drop. This is encouraging visual feedback, not exhaustive visual acceptance.

The pilot-off and pilot-on sections had unequal update workloads. **Do not calculate a NanoVG-versus-shader speedup or slowdown ratio from these numbers.**

### Latest offline matched comparison: shared boundary candidate

Two 106-by-48 previews at 1x density; 100 warmup frames followed by 360 measured frames; three repetitions with rotated mode order. Total CPU includes preparation, target updates, NanoVG image submission, and NanoVG end-frame submission.

| Mode | Range of median total CPU across repetitions |
|---|---:|
| Separate full guards, independent programs | 104.3–105.2 microseconds |
| Shared full guard, shared program | 73.6–91.0 microseconds |
| Shared restricted guard, shared program | 61.8–75.5 microseconds |

The last mode still spent 44.0–53.6 microseconds median in capture/restore scopes. This is an improvement within our architecture, **not evidence that the architecture beats native NanoVG or has negligible overhead**. It has not yet been measured live in Rack. Shared program ownership also changes between the separate and grouped modes; this is not a perfectly isolated state-boundary microbenchmark.

Focused real-GL tests pass for grouped output equivalence, full/restricted/mixed state restoration, cached and independently dirty targets, fractional image alignment, context recovery, and avoiding a second GL boundary during presentation. Native Windows plugin linking and packaging pass. This is not a full host compatibility certification.

## Prior work: avoid rediscovering these without new evidence

- Extra point/segment pruning is not the desired route. The user is happy with the established visual level; original pruning remains intact.
- Optimizing NanoVG stroke/join computation looked promising offline, but a plugin cannot ship a replacement for the host's NanoVG implementation. A private NanoVG engine rendered into per-preview surfaces did not deliver the hoped-for live result.
- Retained polyline and texture-search approaches have not demonstrated a reliable total-cost win in the tested forms.
- Analytic function rendering avoids some polyline construction conceptually, but **the current live pilot still builds the old geometry for existing histories/overlays**. Geometry-elimination savings must not be credited to the current live measurements.
- Prepared math, approximate atan, and fewer solver iterations have been explored. Shader math improvements have not removed execution-boundary costs.
- Saving only the declared generic vertex attribute instead of all four was previously tried and did not improve timing. Recommending fewer attribute queries alone needs new evidence.
- Shader history variants have not consistently beaten the production cached snapshot-history path. Do not assume moving every element into one shader automatically wins.
- The user reports that an earlier HaloKnob2 shader-hybrid conversion delivered substantial gains. Wyrm also has existing GL rendering infrastructure. These are useful local implementation references, not proof that their execution paths suit these tiny previews.
- Legacy contour caching waits roughly 100 ms for a stable source before settling into a framebuffer cache. The shader pilot immediately renders a changed key and reuses unchanged output. Equal-work comparisons must account for this difference.

## Research questions

1. **Map the actual host render sequence.** From Rack source, identify NanoVG recording versus GL submission, widget traversal, layer ordering, framebuffer rendering, `OpenGlWidget`, and the points where custom GL is expected. What guarantees exist at those points? Cite exact files/functions and revision/version.
2. **Find the minimum correct host-state contract.** Which state is genuinely live across a plugin callback? Does Rack or NanoVG re-establish enough state to avoid our broad capture/restore? Distinguish documented guarantees from implementation observations. Consider initial allocation, steady updates, cache hits, and resource recreation separately.
3. **Explain the observed state-scope cost.** Could attribute-stack operations, `glGet*`, preceding GPU work, driver threading, or implicit synchronization account for it? What targeted measurements distinguish these explanations without merely moving the wait into another scope? Do not recommend `glFinish` as a production solution.
4. **Compare integration alternatives.** Assess the current offscreen path, Rack-native GL widget execution, direct rendering at a safe host boundary, module-level atlases, deferred submission/batching, and shared execution across modules. Explain feasibility without host modification, ordering/clipping consequences, update coupling, bandwidth/overdraw, and context ownership. Include a better route if these omit one.
5. **Find concrete working examples.** Examine maintained third-party Rack plugins with custom shader rendering and relevant Rack/NanoVG source. Explain their actual execution contract and why it applies—or does not apply—to small frequently updated widgets. Popularity alone is not performance evidence.
6. **Identify the scaling limits.** Can batching across module instances be achieved safely with public plugin APIs? Can cached-image submission remain in traversal order while updates occur earlier? How do multiple plugin instances, editor windows, offscreen captures, and DAW context recreation affect the design?
7. **Decide whether an atlas is worth testing.** Separate reducing host boundaries from reducing FBO switches/draw calls. Quantify what an atlas could plausibly save, what it cannot remove, and its dirty-region/filtering/lifetime costs.

## Required deliverable

Please return:

1. A direct recommendation: keep and amortize this execution method, replace its integration point, or gather specific missing evidence before choosing. State confidence and unresolved assumptions.
2. A source-backed render-sequence explanation and comparison table for the viable architectures. Link directly to primary source files/functions, official docs, or reproducible implementation examples. Identify applicable versions; do not treat all OpenGL implementations as equivalent.
3. A prioritized set of **small discriminating experiments**, each with a hypothesis, controlled variable, measurements, correctness checks, and a decision rule. Start with a no-op/clear-only boundary test under representative host load, if appropriate, before building another curve renderer.
4. A proposed reusable Lumin execution interface and ownership model at the conceptual level: where requests are collected/executed, where images are presented, who owns GL state/resources, and how contexts and caches recover. Keep this grounded in available Rack APIs.
5. A clear separation of facts supported by source, measurements supplied here, hypotheses, and claims requiring live validation.

Benchmark recommendations should compare equal workloads, include both CPU and delayed GPU timing where available, report median and tail behavior, and cover static/cache-hit, one-dirty, both-dirty, modulation, zoom/density changes, and multiple instances. Preserve total module Process/Step/Draw meanings while adding component timings. Evaluate whole-frame cost so that moving a stall does not count as eliminating it.

Avoid a generic OpenGL optimization checklist. The useful outcome is discovering whether we have chosen the right boundary into Rack's rendering system and designing the smallest test that can prove it.

## Optional local attachments

This brief is self-contained. The repository is not assumed to be accessible to you. If source inspection would resolve a question, request the relevant file rather than inventing its contents. These are repository-relative paths the developer can attach:

- `src/visual/AdaptiveGlSurface.hpp` and `.cpp`: surface lifecycle, batching, state guard.
- `src/render/FunctionContourPilot.hpp`: admission, caching, prepare/complete/present.
- `src/render/FunctionCurve.hpp`: analytic shader and exact GL footprint.
- `src/IntegralFluxWidget.cpp`: module-level batch and widget presentation integration.
- `src/GlResourceRetirement.hpp` and `.cpp`: context/resource ownership.
- `tests/adaptive_gl_batch_spec.cpp` and `tests/lumin_function_pilot_spec.cpp`: correctness tests and matched probes.
- `doc/lumin-evidence/function-first-live-results.json` and `function-shared-pass-results.json`: captured evidence and source/package hashes.
- `src/visual/HaloKnob2.cpp` and `src/WyrmRendererGL.cpp`: other local render implementations.

Snapshot: 10 September 2026. The developer is continuing live testing in parallel; treat this as a dated baseline, not an assumption about subsequent results.
