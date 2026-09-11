# Lumen: plugin-wide execution direction

Decision recorded 10 September 2026: optimize rendering across the plugin, using
shared execution infrastructure where measurements support it. Flux is a probe,
not the limit of the design. The user accepts close perceptual parity; exact
NanoVG pixels are not the product requirement. This document scopes the next
architecture experiment. A [bounded shared-surface executor](lumin-batch-experiment.md)
is now implemented and tested offline; module scheduling is not integrated yet.

First named primitive: [Lumin PolylineStroke](lumin-polyline-stroke.md), a retained
shader line feature that consumes shared passes without owning one surface per
line. Its specification is proposed; the earlier shader prototype remains offline.

## Evidence and reach

The Flux private-NanoVG live capture regressed. The subsequent custom shader
prototype also regressed in its offline complete-path comparison. In the first
controlled 1x run, two empty surface updates/presentations cost 78.9–103.6 us
median versus 23.3–28.1 us for two host NanoVG contours. This implicates the
combined surface route, not specifically the state guard or any individual GL
call. Repeated results and limitations are in [the shader record](flux-shader-candidate.md).

Source inventory identifies these existing reuse points:

| Family | Existing code | Opportunity to measure |
|---|---|---|
| Shared controls | `visual/HaloKnob2.cpp`; references in Flux, Proc, Undertow, WYRM, Bifurx, Iris, Mandelwake and Puffy UI sources | Group dirty control updates; share immutable programs within a context; retain independent idle caches |
| Waveform editor | `WyrmRendererGL.cpp` | Existing revision-based geometry/texture reuse and tiled analytic stroke rendering; candidate executor consumer |
| Spectrum | `BifurxGL.cpp` | Multiple shader/material passes and optional adaptive surface; mixed-pass state isolation |
| Scope | `TDScopeGL.cpp` | Optional adaptive GL rendering with four generic attributes; test different pass-state requirements and continuous updates |
| Fractal preview | `NautiloidWidget.cpp` | Adaptive density and interaction/settled transitions; measure separately from small controls |
| Curve/history family | Flux, Proc and Undertow; `SettledContourFramebuffer`, `SnapshotHistory`, `PhosphorPreview` | Preserve current cache wins; adopt custom strokes only when complete-path comparisons justify them |

This inventory is source reach, not a claim that every path is enabled or shares
the same bottleneck. Existing optimized static caches are the baseline. Rendering
improvements do not imply audio DSP improvements or identical gains per module.

## Small shared execution contract

Retain AdaptiveGlSurface's allocation, capacity, image presentation and resource
retirement responsibilities. Separate its per-update host-state protection from
its target update so an executor can run multiple eligible updates inside one
protected scope. Keep the existing independently guarded entry point for local
updates and fallback.

Each pass declares its target, viewport, clear policy, input revisions, required
program/textures/buffers, blend/scissor/depth/stencil behavior and generic vertex
attributes. An outer guard restores Rack state once. Between passes, the executor
establishes the declared state, using known internal state where safe instead of
querying Rack repeatedly. A single outer guard alone is insufficient: a scope
shader can otherwise inherit a control's blend, pixel-store or attribute state.

Use an editor/context-epoch owner, not an unrestricted plugin-global GL singleton.
Share immutable shader programs/materials by their full configuration. Mutable
history, geometry revisions and outputs remain independently owned. Update only
changed inputs and preserve existing settled/cache-hit behavior.

Preparation publishes bounded update requests. Group only work whose current
visibility and destination are established. Start module-local; prove cross-module
registration, removal, hidden-window behavior and capture handling before extending
admission across an editor. Individual widgets still present in Rack traversal
order. Outputs already queued for presentation cannot be overwritten in the same
recording. Unsupported destinations use the existing local/fallback path.

Separate targets are sufficient for the first experiment. An atlas can reduce
target switches later, but adds filtering, clipping and lifetime constraints;
it is not a prerequisite. Likewise, no worker pool or general render graph is
required to measure boundary consolidation.

## Perceptual acceptance

Use normal Rack viewing sizes and actual DPI/zoom as the primary visual review,
including 119%, motion, highlights and both themes. Small antialias or round-join
coverage differences can be acceptable. Missing segments, broken joins, unstable
shimmer, conspicuous thickness/color changes, clipped effects, stale edges and
interaction lag are defects.

Retain quantitative comparisons as diagnostics: aggregate error, spatial extent
of changed pixels, localized feature crops and temporal behavior. A maximum-byte
threshold alone is not a perceptual verdict. Keep the first shader's failed
screening result intact; do not relabel it a pass. Establish a separate perceptual
review protocol before selecting the next candidate, with representative images
and motion review. Current localized highlighted-endpoint differences still need
attribution, even though exact pixel equality is no longer the intended goal.

## Experiment order and decision

1. Instrument the shared surface update to distinguish host-state protection,
   target preparation/clear, callback rendering, restore and presentation. Record
   update/guard/target-switch/upload counts and changed/unchanged populations.
2. Compare identical callbacks and independent targets under separate versus
   grouped scopes. Include mixed passes with deliberately different blend,
   scissor, pixel-store, texture and attribute state. Check output equivalence and
   host restoration, then measure 1, 8, 32 and 64 surfaces at comparable total
   pixel area as well as realistic sizes. This isolates the executor from shader
   quality changes.
3. Use at least two renderer families before promoting the interface: shared
   Halo controls and a waveform/display path, followed by a mixed patch containing
   WYRM, Flux and an available scope/spectrum path. Compare active, settled, cold,
   zoom and reopen cases with actual backend settings recorded.
4. Integrate the custom stroke shader independently, preserving existing pruning.
   Optimize segment lookup, uploads and perceptual coverage against that baseline.

Record numerical acceptance limits from repeated baseline variability before
implementing the executor optimization. Measure CPU submission, delayed GPU time,
whole-module Step/Draw and host pacing where available, plus memory and displayed
source age. Shared execution must retain module ownership for telemetry; moving
work into another module's draw or into step is not a speedup. Do not add nested
timer percentiles together.

The intended result is lower total rendering cost in representative mixed patches,
with no material regression in unchanged or independently cheap modules. Some
modules may gain little; they should remain on their efficient existing path.
If batching does not exceed measurement noise or pass-state setup consumes its
savings, stop expansion and use the stage measurements to choose the next cost.
