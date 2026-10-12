# LRC Context Resource Core Plan

## Status

Proposed implementation plan. Begins only after the baseline can measure
resource creation, reuse, fallback, and context recreation.

Parent charter: [Leviathan Render Core](LRC.md)

Consumer experiment: [HaloKnob2 resource sharing](LRC_HaloKnob2_Sharing_Experiment.md)

## Objective

Build the smallest context-safe subsystem capable of sharing one shader
program, one immutable geometry buffer, and one immutable texture across
multiple render surfaces in the same active graphics context.

This milestone proves ownership and recovery. It is not a general renderer,
material framework, scene graph, or dynamic batching system.

---

## 1. Required properties

The core must:

- distinguish context generations without trusting reusable numeric GL names;
- abandon handles inherited from an earlier context before use;
- create resources lazily from a valid UI/draw context;
- avoid GL calls from arbitrary destructors;
- delete only resources known to belong to the current context;
- recover when a widget misses the preceding context-destroy event;
- allow several widgets to reuse an immutable resource;
- isolate initialization failure to one resource and context generation;
- expose creation, reuse, failure, abandonment, and rebuild counters;
- keep normal acquisition allocation-free after resources are warm;
- never involve the audio thread;
- preserve a consumer-owned fallback path.

---

## 2. Explicit non-goals

Do not add in this milestone:

- module-wide command buffers;
- cross-widget batching;
- dynamic VBO pooling;
- instancing;
- a generic material graph;
- shader hot reload;
- plugin-global animation clocks;
- framebuffer ownership;
- NanoVG image ownership migration;
- automatic conversion of existing renderers;
- background GL compilation;
- mutexes reachable from audio processing.

---

## 3. Discovery gate: context identity

Before implementation, document the exact Rack callbacks and identity values
available during:

- initial scene creation;
- normal widget insertion;
- context destruction;
- context recreation while widgets survive;
- standalone window recreation if applicable;
- DAW editor close/reopen;
- module-browser preview rendering.

The discovery must answer:

1. Which object or event can safely identify the active graphics context?
2. Can the same pointer value be reused for a later context?
3. Which component observes every create/destroy event?
4. Can a consumer be drawn before it observes an expected create event?
5. On which thread do callbacks and draws occur?
6. What cleanup is safe when destruction is missed?

The likely design is an explicit monotonically increasing context generation
combined with current-context event participation. The exact source of the
generation is deliberately not fixed in this plan.

Pointer identity may be part of a key but must not be the only defense against
allocator reuse.

### 3.1 Source-confirmed context facts

The Rack SDK and current Leviathan source establish the following:

- `Widget::ContextCreateEvent` is recursively delivered after the Window,
  OpenGL context, and NanoVG context are created.
- `Widget::ContextDestroyEvent` is recursively delivered before those Window
  contexts are destroyed.
- Both events expose an `NVGcontext*`; neither exposes a unique OpenGL context
  ID or monotonically increasing generation.
- `OpenGlWidget` inherits `FramebufferWidget` and dirties every frame by
  default. A cached GL surface must override `step()` and call
  `FramebufferWidget::step()`.
- `FramebufferWidget` owns its framebuffer internals, dirty scheduling,
  context callbacks, and redraw-on-next-draw behavior.
- The current UI thread can query `glfwGetCurrentContext()`. TD.Scope currently
  regards GL as current when that value equals `APP->window->win`.
- HaloKnob2 clears inherited GL names unconditionally in its explicit
  context-create callback before lazy rebuilding.
- Bifurx likewise has explicit create/destroy callbacks and abandons inherited
  names on create.
- A raw GL object name can be reused by a later context and therefore cannot
  prove ownership or validity.
- NanoVG image handles are context-owned; Puffy and shared NVG helpers treat a
  changed or invalid context as a reason to abandon rather than cross-delete.

These facts justify generation-based ownership but do not yet identify the
correct global coordinator.

### 3.2 Unresolved facts requiring diagnostic observation

The first diagnostic run must determine:

- callback order between parent framebuffer widgets, child OpenGL widgets, and
  independent module widgets;
- whether every surviving widget receives create after a missed destroy in the
  target DAW;
- whether the `NVGcontext*`, GLFW window pointer, or both can reuse the same
  address across editor reopen;
- whether module-browser previews use the primary Rack context, a distinct
  context, or framebuffer bypass without GL initialization;
- whether more than one relevant graphics context can be active during the
  plugin's lifetime;
- whether widgets inserted after initial context creation receive a create
  event before first draw;
- the actual UI thread identity for create, destroy, step, and draw callbacks;
- whether destroy callbacks run while `glfwGetCurrentContext()` still matches
  `APP->window->win` in standalone and each target plugin host;
- behavior during window resize, monitor/DPR movement, and editor replacement;
- whether Rack destroys the scene tree before or after the final useful shared
  registry retirement point.

No production resource-registry design may claim these answers from source
alone.

### 3.3 Context observation record

Each diagnostic event row should contain:

```text
sequence,wall_time_us,thread_id,event,
widget_family,widget_address,module_id,
nvg_context_address,glfw_current_context_address,rack_window_address,
resource_generation_if_any,notes
```

Address values are diagnostic correlation aids, not durable identifiers. Logs
must avoid audio-thread access and remain debug-gated.

### 3.4 C1 — Standalone context observation procedure

Run two variants with the same instrumented build.

#### C1A — Patch present at startup

1. Save a patch containing Integral Flux, Bifurx, Puffy, TD.Scope, and Wyrm with
   their relevant visuals visible.
2. Start standalone Rack directly into that patch.
3. Record create callbacks, first step/draw/acquire for each family, and first
   correct visual frame.
4. Pan and zoom without changing parameters.
5. Open the module browser and expose Halo and GL-backed previews.
6. Close the browser, remove one instance, add it again, then exit Rack.
7. Record destroy callbacks and whether GL is still current during them.

#### C1B — Widgets inserted after context creation

1. Start standalone Rack with an empty patch.
2. After the window is stable, insert the same target modules one at a time.
3. Record whether each new widget receives context create before first step and
   first draw.
4. Duplicate modules and remove them in a different order.
5. Exit and record destroy/teardown ordering.

Repeat both variants at least three times before treating ordering as stable.

### 3.5 C2 — Target-DAW editor lifecycle procedure

1. Load the plugin in the target DAW with the C1 target patch.
2. Open the editor and wait for all visuals to reach a correct frame.
3. Resize the editor, pan/zoom Rack, and open/close the module browser.
4. Close the editor without unloading the plugin instance.
5. Wait at least two seconds, reopen, and record callbacks, pointer identities,
   GL-current state, rebuilds, fallback, and time to first correct frame.
6. Repeat close/reopen at least ten times.
7. Repeat one cycle after moving the editor between monitors/DPRs if available.
8. Save/reload the DAW project and distinguish plugin-instance destruction from
   editor-only context replacement.
9. Finally unload the plugin instance and record the terminal destroy/teardown
   sequence.

Run once with extra GL validation disabled and once enabled. The validation run
is diagnostic only and is not used for performance comparison.

### 3.6 Observation exit criteria

The context discovery phase is complete only when:

- C1A, C1B, and C2 raw logs are retained;
- every unresolved item in Section 3.2 is answered or explicitly marked
  unobservable;
- event order differences between standalone and DAW are documented;
- the chosen generation coordinator and key are justified from observations;
- missed-destroy recovery behavior is demonstrated rather than assumed;
- preview behavior is classified;
- the proposed registry retirement point is named;
- the decision is recorded before D1 exposes a shared GL handle.

---

## 4. Minimal conceptual API

The implementation may differ after discovery, but consumer behavior should
remain approximately this small:

```cpp
struct LrcContextGeneration;

struct LrcResourceKey {
    const char* family;
    uint32_t variant;
};

template <typename Resource>
struct LrcResourceView {
    Resource* resource = nullptr;
    uint64_t generation = 0;
    bool valid() const;
};

LrcResourceView<HaloSharedResources> acquireHaloResources(
    const LrcContextGeneration& context);
```

Important constraints:

- returned views are UI-thread/context-generation objects, not audio-safe
  handles;
- a consumer does not cache a view across a context-create event without
  generation validation;
- acquisition failure is explicit;
- consumer fallback does not call back into a failed resource;
- variants exist only when binary resources truly differ. Uniform-only visual
  differences do not create unnecessary program variants.

Avoid an untyped string-only cache as the first public API. Typed resource
families make creation, teardown, and diagnostics easier to audit.

---

## 5. Ownership model requirements

### Logical descriptions

Shader source, CPU texture data, geometry descriptions, and resource keys may
outlive contexts. They contain no context-bound handles.

### Context-generation resources

Programs, shaders, buffers, textures, and any GL-backed cache belong to exactly
one generation.

### Consumers

Consumers own:

- framebuffer surfaces;
- visual state and dirty tracking;
- module-specific uniforms;
- fallback widgets;
- decisions about whether a resource failure disables an advanced path.

### Retirement

Resource retirement must not depend on the last arbitrary widget destructor
calling GL. Preferred behavior is generation-level retirement during a known
current context-destroy callback. If that event is missed, clear numeric names
without deletion on the next create and allow the old context owner/driver to
reclaim them.

Reference counts may describe logical use, but reference-count destruction must
not silently issue unsafe GL calls.

---

## 6. Resource state machine

Each shared resource family should have an inspectable state similar to:

```text
Uninitialized
    ├── acquire in valid context → Creating
    └── no valid context         → Unavailable

Creating
    ├── success → Ready
    └── failure → FailedForGeneration

Ready
    ├── reuse                → Ready
    ├── current destroy      → Retired
    └── later create/mismatch→ Abandoned

FailedForGeneration
    ├── ordinary acquire     → explicit failure
    └── new generation       → Uninitialized
```

Retry within the same generation should be opt-in and bounded. Recompiling a
known-failing shader on every draw is forbidden.

---

## 7. First resource set

The first complete resource family is defined by the Halo sharing plan and is
limited to:

- one linked Halo GLSL program and its uniform locations;
- one immutable unit-quad or canonical-quad VBO;
- one immutable Halo cap atlas texture;
- creation/reuse/failure counters.

Halo palette/configuration differences remain uniforms unless measurement
proves a binary resource variant is required.

No other module migrates until this family survives the acceptance matrix.

---

## 8. GL-state contract

Shared resources do not imply shared mutable GL state.

Consumers remain responsible for setting and restoring the state their draw
requires, including:

- framebuffer and viewport as governed by Rack's surface;
- program binding;
- texture unit and texture binding;
- array buffer and vertex attribute state;
- blending;
- depth, cull, scissor, and line state if modified.

The resource core may expose immutable handles and uniform locations. It does
not leave a program or buffer implicitly bound for a later widget.

State-restoration rules must be documented from current Rack/Leviathan
practice and verified in mixed-renderer racks.

---

## 9. Failure isolation

Resource creation failures must record:

- resource family and variant;
- context generation;
- compile/link stage;
- bounded driver log text;
- number of affected consumers;
- fallback activation count.

One failed family must not poison unrelated shared resources. A Halo shader
failure must still allow Bifurx, Wyrm, TD.Scope, NanoVG panels, and audio to
operate.

Warnings should be emitted once per failure/generation rather than once per
frame or consumer.

---

## 10. Instrumentation

Debug-gated counters:

- context generations created/destroyed/abandoned;
- acquire calls;
- cold creates;
- warm reuses;
- create failures;
- same-generation rejected retries;
- GL programs, buffers, and textures created/deleted/abandoned;
- logical consumers currently attached where observable;
- time spent in cold creation;
- time to first correct frame after context create.

Counters must distinguish logical resource families from raw GL object counts.

---

## 11. Implementation phases

### Pre-code documentation gate

Before even diagnostic code is added, the following must exist:

- the source ownership inventory in `LRC_Baseline_and_Benchmarks.md`;
- the benchmark environment and raw-result schema;
- this fact/unknown context ledger;
- the Halo visual-reference manifest;
- the first-code file/change boundary and rollback sequence;
- a named standalone and DAW observation procedure;
- an explicit list of questions that diagnostic logging must answer.

Once this gate is satisfied, the first code is diagnostic instrumentation only.
The shared registry does not begin until those observations are recorded.

### Phase A — Context event probe

Add temporary debug-gated event logging to the smallest relevant surface and
exercise standalone plus DAW reopen cycles. Document the observed ordering and
identity behavior.

Exit gate: the generation mechanism is evidence-backed.

### Phase B — Pure state-machine model

Implement resource-generation and failure-state logic without GL calls where
possible. Unit-test create/reuse/failure/new-generation/abandon transitions.

Exit gate: lifetime semantics can be tested independently of Rack rendering.

### Phase C — Minimal GL registry

Add typed registration/acquisition for the Halo resource family. Keep code in a
small dedicated module rather than expanding `GlLifecycleUtils` into a hidden
renderer. `GlLifecycleUtils` remains the shared validity-check helper unless a
clear naming/ownership refactor is warranted.

Exit gate: one context, multiple consumers, one resource set.

### Phase D — Context recovery and failure injection

Exercise clean destroy, missed destroy simulation, new create, compile failure,
texture failure where injectable, and automatic consumer fallback.

Exit gate: no stale handle reuse and no repeated failure storm.

### Phase E — Benchmark and decision

Run the paired baseline scenarios. Decide whether to retain, revise, or remove
the resource core before adding a second family.

---

## 12. Validation matrix

Required checks:

- one Halo consumer;
- many Halo consumers in one module;
- many modules using Halo;
- normal and bright-orange Halo configs;
- module browser preview;
- add/remove/re-add modules;
- rack pan and zoom;
- standalone context recreation where available;
- repeated target-DAW editor close/reopen;
- forced shader initialization failure;
- advanced-path recovery on a new context generation;
- mixed Bifurx, TD.Scope, Wyrm, Puffy, and Halo rack;
- debug validation enabled and disabled.

Object compilation and focused unit tests are expected in WSL. Final GL and DAW
validation is authoritative only in the Windows/MSYS2 toolchain.

---

## 13. Acceptance criteria

The resource core is accepted only if:

- one shared Halo resource set services all compatible consumers in one
  context generation;
- per-consumer framebuffer and dirty behavior is unchanged;
- context reopen never reuses an inherited GL name;
- clean destroy deletes resources only through the owning current context;
- missed destroy remains safe;
- failure falls back once without warning/log storms;
- no production draw adds unconditional `glIs*` checks;
- warm acquisition does not allocate or compile;
- instrumentation proves reduced duplicate program/buffer/texture creation;
- baseline scenarios show no idle or active regression;
- the implementation remains small enough to audit as lifecycle infrastructure.

If these criteria are not met, do not broaden the API. Preserve module-local
resources and record the rejected design in the results document.
