# LRC Dynamic Surface Experiment Plan

## Status

Candidate-evaluation plan. No module migration is implied until the Halo
resource-sharing go/no-go gate passes.

Parent charter: [Leviathan Render Core](LRC.md)

Prerequisite gate: [HaloKnob2 resource sharing](LRC_HaloKnob2_Sharing_Experiment.md)

## Objective

Determine, module by module, whether current dynamic visuals benefit from:

- shared immutable LRC resources;
- consolidation into a module subregion or module-wide surface;
- continued independently cached surfaces;
- rate-limited redraw;
- improved NanoVG caching;
- no architectural change.

The output is a set of evidence-backed decisions, not a requirement that every
candidate migrate.

---

## 1. Governing rule

> Consolidate visuals only when their update locality, surface area, draw
> ordering, and measured costs make consolidation superior to independent
> caching.

One module-wide dynamic surface is an available tool, not the LRC definition.

---

## 2. Prerequisites

Before any candidate implementation:

- its baseline scenario exists;
- dirty causes and current redraw policy are known;
- its graphics lifecycle has been exercised in the target DAW;
- fallback/product renderer behavior is documented;
- released/unreleased compatibility status is recorded;
- the candidate has one measurable target cost;
- the rollback path is clear.

---

## 3. Candidate evaluation worksheet

Every candidate plan fills out this worksheet.

### Current composition

- static panel and label layers;
- cached NanoVG framebuffers;
- OpenGL widgets/FBOs;
- live transparent widgets;
- interaction widgets;
- resource ownership;
- fallback paths.

### Update behavior

- elements that change together;
- elements that change independently;
- dirty frequency and cause;
- continuous versus event-driven motion;
- acceptable visual refresh rate;
- surface dimensions at representative zoom/DPR.

### Current cost

- idle framebuffer composites;
- active redraw count/time;
- CPU geometry or raster preparation;
- draw calls and uploads where observable;
- insertion/rebuild cost;
- p95/p99 frame impact;
- resource duplication.

### Proposed experiment

- exact surface boundary;
- content that remains static/cached;
- content that moves;
- interaction/visual separation;
- shared LRC resources used;
- dirty/rate-limit policy;
- fallback;
- expected benefit and primary risk.

### Decision

```text
retain current | optimize locally | share resources | consolidate | split | no-go
```

---

## 4. Priority framework

Score candidates qualitatively on:

- measured active cost;
- visible frame-time spikes;
- repeated immutable resources;
- co-located elements changing together;
- ability to isolate an experiment;
- visual-reference stability;
- lifecycle maturity;
- compatibility risk.

Prefer high-cost, well-isolated experiments in unreleased modules. Renderer
changes in released modules require stronger parity and rollback gates.

---

## 5. Initial candidate notes

These are starting hypotheses, not migration decisions.

### 5.1 Bifurx

Compatibility: unreleased.

Current characteristics:

- substantial module-level OpenGL spectrum/curve renderer;
- several shader programs, VBOs, and textures;
- established context recovery and debug measurements;
- module-specific render modes and fallback behavior.

High-value questions:

- Can common program/quad infrastructure use the shared context core without
  obscuring Bifurx's module-specific reset graph?
- Are its several programs truly reusable elsewhere or merely internally
  shareable across Bifurx instances?
- Does multi-instance resource sharing improve insertion/reopen behavior?
- Is current surface granularity already appropriate?

Likely first experiment: share one clearly immutable program/quad/texture family
across Bifurx instances, not rewrite the entire renderer.

### 5.2 Puffy

Compatibility: unreleased.

Current characteristics:

- animated fish composed through NanoVG and cached raster/image paths;
- transfer preview with a cached curve framebuffer;
- extensive debug-gated draw metrics;
- multiple visual components with different update frequencies;
- explicit context-owned NanoVG image handling.

High-value questions:

- Which components dominate sustained draw time after current caches?
- Can fish pose, body composition, and transfer preview be rate-limited
  independently?
- Would one Puffy dynamic surface eliminate useful independent caching?
- Are further image-atlas or state-snapshot improvements more valuable than GL?

Likely first experiment: rate/update-policy refinement using existing metrics,
not immediate conversion to a monolithic GL surface.

### 5.3 TD.Scope

Compatibility: released.

Current characteristics:

- high-density continuously changing display;
- multiple historical/debug renderer paths;
- substantial GL resource and style logic;
- existing documents governing unified render direction and release behavior;
- expander interaction and visual semantics that must remain stable.

High-value questions:

- Is the product renderer and canonical visual model now settled enough for an
  architectural migration?
- Which duplicate resources can be shared without reopening renderer-style
  decisions?
- Can row preparation and renderer consumption be separated further without
  changing drag/read-head behavior?
- Is its existing module-level surface already the right batching boundary?

Constraint: LRC must not use TD.Scope as an early framework playground. Any
migration follows its own product/render-direction documents and released
compatibility requirements.

### 5.4 Wyrm

Compatibility: unreleased.

Current characteristics:

- module-level OpenGL sand renderer;
- internal texture/FBO pairs and wave-column textures;
- editor framebuffer interactions;
- module-specific lifecycle/reset graph.

High-value questions:

- Which immutable programs or full-screen geometry can be safely shared across
  instances?
- Are internal render-target resources inherently per instance?
- Can upload frequency or texture changes be reduced before any surface change?

Likely first experiment: instance-shared immutable shader resources while
leaving render targets and sand state local.

### 5.5 Bifurx/Halo mixed module cases

Modules combining an OpenGL display with HaloKnob2 controls are useful for
testing shared-resource coexistence, but not evidence that all visuals should
be drawn in one surface. Compare independent cached controls against any
proposed module-wide composition explicitly.

### 5.6 Integral Flux and Proc previews

Compatibility: released.

Current characteristics:

- established OpenGL/cached preview systems;
- extensive module-specific profiling in Integral Flux;
- multiple independently interactive controls and displays.

Constraint: use these as regression and benchmark consumers first. Migrate only
after an unreleased candidate proves the same technique.

---

## 6. Experiment classes

### Class A — Shared immutable resources only

Keep every surface and dirty rule intact. Share programs, static geometry, or
immutable textures across compatible instances.

Use when resource duplication or context rebuild is the measured problem.

### Class B — Local surface optimization

Keep ownership local but reduce path generation, uploads, invalidation, or
allocation.

Use when one surface is expensive but does not share enough semantics with
other consumers.

### Class C — Rate-policy refinement

Split visual state publication from redraw frequency. Cache or rate-limit
subsystems independently.

Use when sustained animation is perceptually oversampled.

### Class D — Subregion consolidation

Combine nearby elements that update together into one bounded surface while
leaving unrelated controls independent.

Use when draw/state-switch overhead dominates and invalidation remains local.

### Class E — Module-wide dynamic surface

Draw most dynamic appearance in one surface and retain transparent Rack
interaction widgets.

Use only when most elements update together, the full surface is already live,
and batching/state coherence outweighs large-area redraw.

### Class F — No change

Document that the current architecture is already the best measured tradeoff.
This is a successful experiment result.

---

## 7. Static/dynamic separation requirements

Every experiment identifies and preserves:

### Static content

- panel artwork;
- labels;
- fixed ornaments;
- fixed shadows/backgrounds;
- unchanged texture assets.

### Dynamic content

- parameter indicators;
- scopes/waveforms;
- meters;
- animation and particles;
- transient effects;
- live procedural fields.

A dynamic update must not invalidate static content merely because both occupy
the same module. If consolidation would force that outcome, the proposal must
quantify why it is still beneficial.

---

## 8. Interaction separation

Visual consolidation may separate appearance from Rack widgets, but the module
must retain:

- exact hitboxes and z-order;
- parameter behavior and automation;
- drag semantics;
- tooltips;
- context menus;
- hover/pressed visual feedback;
- preview/module-browser behavior.

State passed from interaction widgets to a renderer must be bounded and
UI-thread appropriate. No new audio-thread renderer coupling is permitted.

---

## 9. Benchmark requirements per experiment

Each before/after comparison includes:

- idle with no visual changes;
- one active element where applicable;
- all relevant elements active;
- multiple module instances;
- insertion/first correct frame;
- zoom/DPR/rack pan;
- fallback or renderer-disabled path;
- context recreation;
- p95/p99 and worst-frame data;
- visual reference comparison.

Measure the cost the experiment claims to improve. Resource-sharing work must
show resource counts and creation timing; active batching work must show active
frame cost and draw/state reduction.

---

## 10. Compatibility tiers

### Tier U — Unreleased module

Internal renderer architecture and user-visible details may evolve, but tests,
fallback, context recovery, and patch behavior should still remain deliberate.

Initial high-risk experiments should prefer Bifurx, Puffy, Wyrm, Crownstep,
Chronomaw, Sil, or Bulkhead as appropriate to their actual visual workload.

### Tier R — Released module

Integral Flux, Proc, Temporal Deck, TD.Scope, and Undertow require:

- unchanged enum ordering and serialization;
- stronger visual parity;
- existing-patch smoke testing;
- explicit rollback/fallback;
- standalone and DAW validation;
- no unrelated behavior cleanup folded into the renderer migration.

---

## 11. Per-candidate implementation sequence

1. Complete the evaluation worksheet.
2. Capture reference visuals and baseline measurements.
3. Name one targeted bottleneck and one experiment class.
4. Implement behind a debug gate or isolated backend where practical.
5. Validate fallback and context lifecycle before performance judgment.
6. Run paired benchmarks.
7. Record retain/revise/no-go decision.
8. Remove failed experimental paths rather than accumulating permanent debug
   renderers without a product purpose.
9. Extract shared primitives only after another consumer demonstrates the same
   requirement.

---

## 12. Acceptance criteria

A dynamic-surface migration is accepted only if:

- the targeted cost improves materially in its named scenario;
- idle behavior does not regress;
- p95/p99 frame delivery does not regress;
- static content remains independently cached where appropriate;
- interaction behavior is unchanged;
- context recreation and fallback pass;
- resource ownership is easier to audit or no harder than before;
- no hot-path allocation or audio-thread coupling is introduced;
- released compatibility requirements pass where applicable;
- the implementation does not preserve redundant old/new renderers without a
  documented product or fallback reason.

As a general decision guide, less than roughly 5–10% measured benefit does not
justify substantial new complexity. A larger gain may still be rejected if it
damages frame consistency, lifecycle reliability, or maintainability.

---

## 13. Deliverables

For every evaluated candidate, add a short result section or companion result
document containing:

- baseline environment and raw-log location;
- current and proposed surface diagrams;
- measured result table;
- visual/fallback/context findings;
- compatibility findings;
- final decision;
- follow-up primitive extraction, if any.

Do not update `LRC.md` to claim a capability until at least one accepted
consumer implements it and the relevant benchmark result exists.
