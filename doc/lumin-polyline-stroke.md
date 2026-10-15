# Lumin PolylineStroke

> Current measured status and decisions: [Lumin status](lumin-status.md). This document preserves its original design or checkpoint context; later live results supersede earlier next-step recommendations.

Feature specification 0.1 — 10 September 2026

Status: proposed first-class library feature, ready to guide implementation.
An experimental retained `PolylineStroke` implementation now exists; see the
[implementation checkpoint](lumin-retained-polyline-experiment.md). No live
module uses it, and neither quality nor performance acceptance has passed. Lumin is the user-facing name in
this specification; the parent architecture is still titled Lumen in the RFC.
This feature does not rename existing code or promote the earlier prototype.

Implementation progress: the [shared-pass dependency](lumin-batch-experiment.md)
now exists as opt-in `AdaptiveGlSurface::renderBatch`, with identical output and
measured surface-submission savings in the harness. The first retained monotone-X specialization is implemented and measured; the
general segment-local backend and live module admission remain outstanding.

## Purpose

Draw frequently changing polylines with shader-defined width, antialiasing, joins
and caps, while minimizing complete rendering cost across Leviathan modules.
Initial consumers are waveform previews, waveform editors, scopes and spectrum
outlines. Preserve close perceptual parity at normal Rack sizes and during motion.

The feature is a retained drawing primitive, not a framebuffer-owning widget.
It draws into an executor-provided target alongside other compatible primitives.
A convenience preview recipe may provide a cached image, but surface allocation,
host-state entry and presentation are explicit costs outside the primitive.

"Maximum efficiency" is an optimization objective, not a promise that one shader
is fastest for every path, size, GPU or update rate. Adoption depends on complete
path measurements against the current optimized renderer. A tiny or settled
NanoVG path may remain the cheaper recipe while using the same source contract.

## Feature contract

| Area | Version 1 behavior |
|---|---|
| Source | Ordered finite 2D points, open path, stable handle and geometry revision |
| Geometry | Existing caller pruning preserved; no hidden resampling or extra decimation |
| Styling | Uniform width and RGBA per path; round joins; butt and round endpoint caps |
| Colored sections | Explicit ordered runs with independent caps; caller specifies shared endpoints and intended layering |
| Coverage | Stable antialiasing in target pixels, premultiplied-alpha output; no bright/dark seams from overlapping segments of one stroke |
| Transforms | Translation and positive uniform scale first; width expressed in logical or device pixels explicitly |
| Updates | Retain source/derived buffers; rebuild or upload only when relevant revisions change |
| Drawing | Submit to a declared private-target GL pass; no internal host flush, surface creation or state guard per line |
| Resources | Context-epoch ownership, cached program/uniform bindings, bounded retained capacity and deferred retirement |
| Failure | Explicit unsupported/invalid/resource-failure result; current-source legacy fallback at the recipe boundary |

V1 excludes arbitrary dashes, variable width, gradients along a stroke, polygon
fills, arbitrary rotation/nonuniform transforms, and closed paths. Add those only
with a concrete consumer and separate visual/performance checks. Glow can later
be a material layer; it is not required to make the basic line implementation
useful. Public limits are declared at creation; exceeding them must never silently
truncate a path or reduce its quality.

Repeated points are normalized deterministically on geometry update. Preserve
run boundaries. A run with fewer than two distinct points produces no line; it
does not implicitly become a marker. Tight turns, reversals and near-zero segments
must either have defined valid coverage or return unsupported for current-source
fallback. Do not solve them by silently dropping visible sections.

## Proposed module-facing API

Illustrative C++11-compatible API; all `lr::` names below are proposed:

```cpp
// Setup on the UI/render side. Storage bounds belong to this instance.
lr::PolylineOptions options;
options.maxPoints = 1024;
options.join = lr::Join::Round;
options.cap = lr::Cap::Butt;
options.widthSpace = lr::WidthSpace::Logical;
auto curve = device.createPolyline(options);

// Only when source geometry changes. update copies into owned bounded storage;
// the caller's points need not remain alive after this call.
auto update = curve.update(points.data(), pointCount, geometryRevision);
curve.setStyle(width, color); // Compared/cached separately from geometry.

// Inside an executor-owned pass that supplies target, mapping and clip.
auto result = pass.drawPolyline(curve, placement);
```

The recipe handles `update`/`result` failures using the current source through its
legacy path. It must not hide rejected updates behind an old frozen contour.
Geometry revisions belong to a stable handle; unchanged revisions promise
unchanged points. Debug builds check count/revision misuse where practical.
Preparation runs on the UI side; this API performs no rendering or allocation
on the audio thread.

Geometry, style and raster invalidation are separate. Color changes do not rebuild
point topology; width/scale changes invalidate derived coverage bounds when needed
but do not resample source points. A zoom change updates mapping and coverage or
the containing target's admitted density. A settled raster hit bypasses primitive
drawing entirely. Cached outputs still obey same-recording presentation leases.

## Rendering strategy

The preferred general-path candidate is segment-local geometry: bounded strips
and join/cap domains carrying adjacency, with shader-based extrusion and analytic
coverage. The CPU packs retained point/adjacency data when geometry changes; it
does not tessellate round arcs using per-join trigonometry. Fragment work is local
to the stroke support, rather than scanning every segment across a full rectangle.
The GL2 baseline may duplicate adjacency in a streamed vertex buffer; instancing,
geometry shaders and modern buffer APIs are optional accelerations, not requirements.

Before accepting that candidate, resolve overlap ownership. Independently blended
segment capsules would darken or brighten translucent joins. Use disjoint coverage
domains or a demonstrated union method. Any extra coverage target/pass belongs in
the measured cost. Colored runs intentionally preserve their separate compositing
semantics; do not union different colors or reorder translucent lines.

Keep WYRM's regular-X, bounded-neighbor distance shader as a specialized candidate
for sources that actually meet its assumptions. Irregular monotone points need
their own lookup; arbitrary polylines cannot inherit regular-X indexing. Any fast
path uses declared source properties plus validation and has a correctness-preserving
fallback. Compare candidates at matched quality before selecting the default.

The previous 64-segment tiled search prototype is a reference experiment, not the
default implementation. It failed complete-path performance and localized image
screening. Do not carry its per-preview forced surface refresh into this feature.

## Shared execution requirements

PolylineStroke consumes the [shared execution contract](lumen-plugin-wide-execution.md):

- One protected host-state boundary can contain multiple eligible target updates.
- The executor establishes the required state between different materials/passes.
- Compatible ordered lines may share program/buffer binding and a submission batch.
  Preserve target, clip, alpha and presentation ordering; compatible does not mean
  all polylines may be arbitrarily sorted.
- Programs and immutable material state are shared per verified graphics context.
  Source geometry, derived bounds and mutable outputs retain independent revisions.
- The containing recipe determines direct private-target drawing versus retained
  raster output. The primitive never creates an offscreen target per line.

Here "direct" means an already-owned private target. It does not authorize issuing
raw GL into Rack's destination during an outstanding NanoVG recording. Integration
must preserve host composition and public lifecycle boundaries.

Bound memory and queue capacity at setup. A warmed unchanged source must not
allocate, compile/link, look up uniforms, rebuild geometry or upload points.
Changing source work should remain O(N) preparation and bounded local coverage;
report actual vertices, uploaded bytes and fragment support area rather than
assuming that asymptotic complexity predicts driver performance.

## Validation and performance acceptance

Visual acceptance is close perceptual parity, not identical NanoVG tessellation.
Review representative stills at actual display sizes plus localized diagnostic
crops and animated shape/zoom transitions. Cover flat/steep sections, sharp crests,
short and repeated segments, reversals, translucent self-overlap, butt/round ends,
adjacent colored runs, clipping, theme changes, fractional positions, and densities
including 1, 1.19, 2 and 4. Missing coverage, obvious width/color shifts, join seams,
clipping defects and temporal shimmer fail. Record unresolved differences instead
of relying on a low whole-image average. Earlier failed image gates stay intact.

Use these comparisons independently:

1. Same drawing callbacks under separate versus shared execution, to isolate the
   framework change from the shader algorithm.
2. Current optimized NanoVG versus PolylineStroke with equivalent visible output,
   including preparation/uploads, target work, host-state protection, presentation
   and deferred host execution where measurable.
3. Real consumers: Flux plus WYRM or TD.Scope, then mixed patches with Halo controls
   and repeated displays. Keep unrelated idle caches unchanged.

Sweep 1, 8, 32 and 64 lines/displays, small and larger targets, uniform and irregular
point spacing, 16/128/1024 points where admitted, static/style-only/continuously
changing inputs, and high/low segment overlap. Include cold start and editor reopen
separately. Record CPU p50/p95/p99, delayed GPU time, allocations, uploads, state
boundaries, target switches, memory, displayed source age and actual host frame
intervals where available. Preserve Process/Step/Draw meanings and attribute shared
work without double counting.

Freeze numerical performance limits from repeated baseline measurements before
the optimization implementation. Require repeatable complete-path benefit beyond
baseline variation in declared target workloads, no material idle/frame-tail/source-age
regression, and bounded resource lifetime. No absolute "fastest" claim or universal
speedup is established by this specification.

## Delivery checkpoints

### First consumer: Integral Flux preview widgets

User-selected test bed: Integral Flux's two channel preview widgets. Use the
current NanoVG contour path as the reference. The old private-NanoVG experiment
is not the baseline, and the earlier shader prototype is not an accepted
PolylineStroke implementation.

The integration seam is `WavePreviewWidget::ContourLayer`: consume the existing
`simplifiedFullPath`, or `simplifiedRisePath`/`simplifiedFallPath` for highlighted
runs, with existing colors and 1.4 logical-pixel width. Each widget retains its
own source/style revisions and primitive handles. Storage is bounded by the
existing 128-point source. Avoid copying/uploading unchanged geometry per draw.

The module owns a render group for eligible updates from both previews. Prepare
both requests before group execution; do not render the second preview from
stale geometry merely because it appears later in Rack traversal. Establish each
preview's current visibility, target mapping and clip before admitting it. Keep
independent outputs and presentation ordering. If only one preview is eligible,
execute only that request. Prove the group in the harness before connecting it
to ordinary Rack draw traversal.

Keep background/grid, moving marker, labels, snapshot history, highlight
interaction and existing 100 ms contour settlement behavior unchanged. Initially
replace current-contour drawing only. Settled-cache rendering and unsupported
destinations may retain NanoVG, with explicit fallback counters. A later history
shader experiment must be measured separately.

| Configuration | Question |
|---|---|
| Current host NanoVG, old round-stroke experiment disabled | What is the optimized baseline? |
| PolylineStroke in independently protected passes | What does the shader plus full integration cost? |
| Same shader and targets under a shared module scope | What does grouping itself save? |

New developer selection must be distinct from `IntegralFluxRoundStroke`; do not
reinterpret old logs or silently change that flag's meaning. Log the selected
backend, actual shader draws, fallbacks, uploads, surface updates and host-state
boundaries alongside preparation/submission timings. Preserve Process/Step/Draw.

Start with one module, then repeated modules and a mixed patch. Include quiet,
independent rise/fall modulation, stopped-and-settled, highlight/drag, zoom and
editor-reopen cases. Test Shark Fin and Maths with asymmetric shapes. Extend the
existing extracted-geometry image corpus with UI sequence checks. Use repeatable
modulation and record cold creation separately. A second distinct consumer is
still required before calling the API broadly reusable.

### Implementation sequence

1. Validate the shared pass boundary and mixed-pass state isolation with existing
   renderers. This is the execution foundation, independently useful beyond lines.
2. Implement the bounded retained primitive and candidate coverage shaders against
   a host-rendered image oracle; resolve alpha/join ownership before live migration.
3. Measure complete-path costs and select the backend for admitted workloads.
4. Add Flux and a second distinct consumer behind developer selection; validate
   actual Rack capture, interaction, clipping, zoom and context reopen.
5. Promote supported workloads only after those gates pass. Retain efficient
   existing paths elsewhere; add optional features from measured consumer needs.

## Specialized function-curve alternative

The [analytic function shader probe](lumin-function-curve-experiment.md) supports
the shared Flux/Proc equation without point generation or segment uploads. It
shows a modest two-preview CPU median benefit when geometry generation can be
omitted, but visual acceptance, tail latency and live integration remain open.
