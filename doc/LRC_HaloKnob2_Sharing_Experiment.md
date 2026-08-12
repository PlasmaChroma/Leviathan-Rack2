# LRC HaloKnob2 Resource-Sharing Experiment

## Status

Formal LRC go/no-go experiment. Requires the benchmark baseline and the minimal
context-resource core.

Parent charter: [Leviathan Render Core](LRC.md)

Prerequisites: [Baseline and benchmarks](LRC_Baseline_and_Benchmarks.md) and
[context resource core](LRC_Context_Resource_Core.md)

## Objective

Share HaloKnob2's immutable GPU resources per graphics-context generation while
preserving its proven rendering architecture:

- one independently cached framebuffer per knob;
- explicit per-knob dirty state;
- per-knob value, hover, drag, and bloom state;
- analytic GLSL appearance;
- current NanoVG fallback;
- current interaction and parameter behavior.

This experiment tests resource sharing, not a visual redesign and not
module-wide Halo batching.

---

## 1. Current state

Each `LeviathanHaloKnob2::HaloGlSurface` currently owns:

- one linked shader program;
- vertex and fragment shader objects during the generation;
- one VBO;
- one GPU cap-atlas texture;
- uniform locations;
- per-surface initialization and failure state.

The CPU cap raster atlas is already shared opportunistically through a weak
pointer while consumers keep it alive. GPU resources remain duplicated per
surface.

Each surface also correctly owns:

- its Rack framebuffer;
- current normalized value;
- bloom amount;
- center-lit state;
- fallback widgets;
- force-fallback and failure behavior.

The experiment must preserve that ownership boundary.

---

## 2. Hypothesis

Sharing the linked program, immutable quad, and cap texture should reduce:

- shader compile/link duplication;
- texture upload duplication;
- immutable buffer duplication;
- module insertion spikes;
- editor reopen reconstruction work;
- resource growth in dense Halo racks.

It may not materially reduce steady active redraw cost because each knob still
draws independently and updates its own framebuffer. Active-frame improvement
is a welcome measured result, not a promised outcome.

---

## 3. Invariants

The following may not change during this experiment:

- knob placement or hitbox;
- parameter IDs, ranges, defaults, or serialization;
- current visual configurations and colors;
- normalized-value mapping and cap rotation;
- hover/drag center illumination behavior;
- cached idle policy;
- current shader output except for approved bug fixes;
- NanoVG fallback appearance and availability;
- automatic recovery after a new context generation;
- debug-only force-NanoVG control semantics.

HaloKnob2 is consumed by released modules. Any intentional visual change is a
separate task with explicit screenshot approval.

---

## 4. Target resource split

### Shared per context generation

```text
HaloSharedResources
├── linked program
├── uniform locations
├── immutable quad VBO
├── cap atlas texture
├── capability/failure state
└── debug counters
```

The shader source and CPU cap atlas description remain context-free logical
data.

### Per surface

```text
HaloGlSurface
├── Rack framebuffer
├── value and bloom state
├── center-lit/hover/drag state
├── dirty state
├── force-fallback state
├── fallback widget tree
└── current-generation resource view
```

No shared mutable uniform cache is needed initially. Each draw uploads its
surface state explicitly.

---

## 5. Geometry decision

The current surface uploads four vertices with `glBufferData(...,
GL_DYNAMIC_DRAW)` on each framebuffer redraw even though the topology is a
quad.

The experiment should prefer one immutable canonical quad, for example
normalized coordinates mapped by the shader through `uLogicalSize`. This
removes per-surface VBOs and per-redraw quad uploads.

If changing the vertex coordinate convention complicates visual parity, first
share a fixed 46-by-46 quad and retain the existing logical-size contract. The
chosen form must remain correct under Rack framebuffer scaling, zoom, and DPR.

Do not introduce a dynamic buffer pool for four immutable vertices.

---

## 6. Shader/program decisions

- One program should serve normal and bright-orange configs because their
  differences are already uniforms.
- Compile and link once per context generation.
- Shader objects may be deleted after successful link if supported by the
  established compatibility path; otherwise their ownership must be explicit.
- Uniform locations belong to the shared linked program.
- A compile/link failure is recorded once for the generation and returned to
  every consumer as an explicit unavailable result.
- A new context generation may retry from `Uninitialized`.

---

## 7. Texture decisions

- Upload the normal/lit cap atlas once per context generation.
- Preserve premultiplication, filtering, mipmap, and clamp behavior.
- Confirm every Halo consumer uses compatible cap artwork before declaring one
  texture universal.
- If future consumers use different cap artwork, key texture variants by a
  stable asset identity rather than by widget address.
- Texture creation failure must not prevent NanoVG fallback.

---

## 8. Dirty and fallback behavior

Resource acquisition does not dirty a warm surface by itself. A surface is
dirtied when:

- value or bloom changes;
- center-lit state changes;
- force-fallback mode changes;
- size/scale requires a framebuffer redraw;
- a new context generation requires the first correct image.

If the shared resource is unavailable:

1. mark the GL path failed for the current generation;
2. bypass to the existing NanoVG subtree;
3. avoid repeated acquisition/compile attempts every frame;
4. permit a retry only on a new generation or an explicit debug retry action.

One consumer must not mutate shared failure state in a way that corrupts an
already ready resource used by other consumers.

---

## 9. Instrumentation additions

Record, while debug is enabled:

- Halo surfaces created;
- shared resource cold creates;
- warm acquisitions/reuses;
- program links;
- VBO creations;
- cap texture creations and upload bytes;
- resource failures;
- fallback surfaces;
- context rebuilds;
- first correct frame time after insertion/create;
- existing GL and NanoVG framebuffer redraw counts/times.

The primary resource result table should compare expected object counts:

| Scenario | Before | After target |
| --- | ---: | ---: |
| One knob | one program/VBO/texture | one shared set |
| Six compatible knobs | six sets | one shared set |
| Multiple modules | one set per knob | one set per context generation |

Actual measured counts override these source-derived expectations.

---

## 10. Implementation phases

### Phase A — Freeze visual reference

Capture reference screenshots for normal and bright-orange configs at several
values, hover states, bloom levels, zooms, and DPRs. Record existing resource
counts and timing scenarios.

### Phase B — Share program only

Move compilation/linking and uniform locations behind the context-resource
core. Retain local VBO and texture temporarily.

Gate: visual parity, fallback, and context cycling pass.

### Phase C — Share immutable quad

Replace local dynamic quad upload with the shared immutable VBO. Confirm
logical-size mapping and framebuffer scaling.

Gate: zero placement/rotation/edge coverage regression.

### Phase D — Share GPU cap atlas

Move upload and texture ownership to the shared generation. Preserve CPU atlas
generation outside the GL hot path.

Gate: correct filtering, hover image selection, rotation, and context rebuild.

### Phase E — Remove obsolete local ownership

Delete local program/VBO/texture lifecycle paths only after all shared stages
pass. Keep surface-local framebuffer and fallback lifecycle intact.

### Phase F — Benchmark and decide

Run the paired benchmark matrix and record results. No second resource family is
added before this decision.

---

## 11. Released-module validation matrix

At minimum validate Halo consumers in:

- Integral Flux;
- Proc;
- Undertow;
- Bifurx;
- Iris;
- Mandelwake;
- Puffy;
- Wyrm.

For released modules, verify existing patch load, default visuals, automation,
hover/drag, context menus, and module duplication. The renderer change must not
alter IDs or serialization.

Test mixed normal/orange configurations in one rack so shared uniforms cannot
leak state between consecutive draws.

---

## 12. Required scenarios

- one idle knob;
- one active knob;
- six simultaneous active knobs;
- several Integral Flux instances idle;
- several Integral Flux instances automated;
- mixed Halo-consuming modules;
- module browser previews;
- add/remove/re-add modules;
- pan and zoom at representative DPR;
- force NanoVG before and after GL initialization;
- force shared shader failure;
- repeated standalone context recreation where available;
- at least ten target-DAW editor close/reopen cycles.

---

## 13. Acceptance criteria

The experiment passes only if:

- compatible Halo surfaces use one program, one immutable quad, and one cap
  texture per active context generation;
- idle framebuffer redraw behavior is unchanged;
- active visual output matches the approved references;
- normal and orange configs cannot contaminate each other;
- fallback remains correct and bounded;
- insertion and/or context-rebuild cost improves measurably, or resource
  duplication reduction is material enough to justify the core;
- active-frame cost does not regress beyond measurement noise;
- p95/p99 frame behavior does not regress;
- no stale GL names survive a context generation transition;
- no production per-frame allocation, compilation, texture upload, or
  unconditional `glIs*` validation is introduced;
- released-module smoke checks pass.

---

## 14. Go/no-go decision

### Go

Proceed to a second carefully chosen shared resource family only when the
acceptance criteria pass and the measured benefit justifies the lifecycle
complexity.

### Revise

Retain only the proven subset—for example shared program but local textures—if
one resource type creates disproportionate context risk.

### No-go

Remove the new shared path and preserve the current per-surface resources if
sharing causes unreliable context recovery, cross-widget state leakage,
negligible scaling benefit, or unacceptable debugging complexity.

A no-go is a valid architectural result and does not invalidate the existing
analytic Halo renderer.
