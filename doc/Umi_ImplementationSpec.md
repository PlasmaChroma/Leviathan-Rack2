# Umi Implementation Specification

> Status: implementation handoff for an unreleased module.
>
> Module name and Rack slug: `Umi`.
>
> Source material: `doc/pachinko.md`, `doc/pachinko_visual_conversion_addendum.md`, and `res/Pachinko/Umi.png`.
>
> Authority: where this document conflicts with either source document, this document defines the Umi v1 contract.

## 1. Product contract

Umi is an 18 HP kinetic probability sequencer. A trigger releases one or more visible pearls into a deterministic, fixed-step pachinko board. The pearls collide with visible pegs and rails, then enter one of eight sinks. Each capture emits a corresponding 10 V trigger and updates derived event CV.

The essential relationship is:

```text
DROP -> visible physical path -> one visible sink -> matching output
```

Umi should feel like a luminous undersea arcade relic rather than a casino machine or a dark mechanical maw. The visual language is cyan water, pearl, chrome, gold, coral, and restrained magenta accents. The module name is **Umi**, but the panel omits a prominent title so the upper space can belong to the playfield and artwork. Do not use franchise logos, recognizable characters, branded typography, or copied commercial machine geometry.

### 1.1 V1 success case

At the end of v1, a user can:

- Drop pearls manually, from an external trigger, or from the internal rate generator.
- Change the path distribution with gravity, tilt, bounce, drag, and chaos.
- Spawn bursts of one to eight pearls.
- See every meaningful collider and every capture.
- Patch one eight-channel polyphonic sink-gate output plus aggregate and event-CV outputs.
- Save a seed and reproduce the same result with the same settings and input event stream.

### 1.2 Explicit non-goals

V1 does not include ball-to-ball collision, editable board geometry, user layout files, polyphonic outputs, gate-while-occupied behavior, a shader renderer, or persisted in-flight pearls. One balanced layout ships initially. Additional layouts are a later feature, not placeholder UI.

## 2. Decisions resolved from the source material

| Question | Umi v1 decision |
| --- | --- |
| Name | `Umi` for title, module name, C++ type prefix, and Rack slug |
| Width | 18 HP / 91.44 mm / 270 Rack px |
| Default layout | `Pearl`, a balanced 8-sink layout derived from the earlier Maw geometry |
| Live display | Central, tall, and visually dominant |
| Primary outputs | One polyphonic `GATES` output carries sinks 1-8 on channels 1-8 |
| Utility outputs | `ANY`, `LEFT`, `RIGHT`, `VEL`, `POS`, and `ACT` remain mandatory |
| CV inputs | `DROP`, `GRAV`, `BOUNCE`, `TILT`, `CHAOS`, and `CLEAR` |
| Omitted panel CV | Rate and density CV are omitted; upstream trigger patterns already control both behaviors. Drag CV is also omitted. |
| Layout selection | No layout parameter in v1; add it only when a second production layout exists |
| Renderer | NanoVG, with cached raster art and a continuously redrawn playfield |
| Physics owner | Audio thread, using fixed-capacity storage and no allocation or locks |
| UI handoff | Lock-free three-slot SPSC render snapshots; UI actions use a bounded SPSC command queue |
| Active pearl persistence | No; patch load begins with an empty board |
| Debug UI | Only when `isDragonKingDebugEnabled()` is true; never normal user state |

## 3. Assessment of `res/Pachinko/Umi.png`

The concept image is 971 x 1619 pixels, aspect ratio 0.59975, which naturally corresponds to approximately 15.2 HP at Rack panel proportions. It is an 8-bit, 256-color RGB palette PNG with no alpha channel. Pure black is baked into the outer background and the apparent holes; it is not transparency.

The image is a useful visual constitution, not a production panel:

- The shell crest, fish, coral, pearl, chrome/gold rails, sea-spirit figure, and lower shrine establish the identity.
- The open blue center establishes the playfield location.
- The apparent black sockets and four mounting slots are not reliable component positions.
- Painted bead lines inside the open center look like colliders. They must either become real colliders or be removed/subdued.
- The existing center is too narrow to be used literally while preserving readable runtime physics.
- The lower ornament is suitable for framing two ordered rows of output jacks after reconstruction.

### 3.1 Production conversion

Preserve the original concept image unchanged. Build a layered 4x production master at 1080 x 1520 pixels. Scaling the source to 1520 pixels tall produces an image approximately 912 pixels wide; extend the marine background and side rails by approximately 84 pixels per side rather than stretching the source.

The runtime package is:

```text
res/Umi.svg
res/Umi/panel_base_2x.png
res/Umi/panel_base@4x.png
res/Umi/panel_ornament_overlay@2x.png
res/Umi/panel_ornament_overlay@4x.png
res/Umi/playfield_frame@2x.png
res/Umi/playfield_frame@4x.png
res/Umi/playfield_mask@2x.png
res/Umi/playfield_mask@4x.png
```

Optional sprites such as a pearl or sink bloom may be added only if they are measurably cheaper or visually better than procedural drawing. The title and all functional labels belong in `res/Umi.svg`, not the raster base.

Before release, the original concept source should either live outside the distributable resource set or be explicitly excluded in `Makefile`; it is not a runtime dependency.

### 3.2 Cleanup requirements

- Remove all black holes and rounded rectangular mounting slots from production layers.
- Reconstruct water, coral, trim, and filigree beneath removed objects.
- Convert separated layers to 8-bit sRGB RGBA.
- Verify alpha edges on both light and dark backgrounds.
- Keep broad caustics and decorative sparkles static.
- Do not bake runtime pegs, sinks, balls, flashes, or output state into the art.

## 4. Panel and component contract

`res/Umi.svg` is a 91.44 x 128.5 mm structural panel. A hidden `components` layer is the source of truth for component placement. `UmiWidget` loads those anchors through `PanelSvgUtils` and retains matching C++ fallback coordinates.

### 4.1 Initial regions

These are layout-proof starting values and may move during art cleanup without changing DSP contracts:

| Region | x mm | y mm | w mm | h mm |
| --- | ---: | ---: | ---: | ---: |
| Upper utility band | 3.0 | 6.0 | 85.4 | 10.0 |
| Playfield aperture | 20.5 | 20.5 | 50.4 | 75.0 |
| Left control rail | 2.5 | 28.0 | 17.5 | 61.0 |
| Right control rail | 71.4 | 28.0 | 17.5 | 61.0 |
| Output row | 4.0 | 111.0 | 83.4 | 14.0 |

The 1000 x 1600 logical board is aspect-fitted and centered inside the aperture. The enlarged aperture uses the former title area and extends farther toward the lower output band. The resulting unused horizontal margin is part of the frame, not a stretched board.

### 4.2 Control grouping

- Upper band: `DROP` input, illuminated `DROP` button, `RATE`, and `DENSITY`. The upper-right space remains open for the artwork.
- Left rail: `GRAV` plus CV, `BOUNCE` plus CV, and `DRAG`.
- Right rail: `TILT` plus CV, `CHAOS` plus CV, and illuminated `CLEAR` plus input.
- Lower row: `GATES`, `ANY`, `LEFT`, `RIGHT`, `VEL`, `POS`, and `ACT`, strictly left to right.

Use compact Leviathan production widgets. Plugs and cables must not materially cover the playfield. Four normal Rack screw widgets replace the painted slots.

Provisional fallback centers, in panel millimeters, are:

| ID | Center | ID | Center |
| --- | ---: | --- | ---: |
| `drop_input` | (8.0, 14.5) | `drop_param` | (23.0, 14.5) |
| `rate_param` | (39.5, 14.5) | `density_param` | (57.0, 14.5) |
| `gravity_param` | (6.5, 37.5) | `gravity_cv_input` | (16.0, 37.5) |
| `bounce_param` | (6.5, 58.0) | `bounce_cv_input` | (16.0, 58.0) |
| `drag_param` | (11.0, 79.0) |  |  |
| `tilt_param` | (84.9, 37.5) | `tilt_cv_input` | (75.4, 37.5) |
| `chaos_param` | (84.9, 58.0) | `chaos_cv_input` | (75.4, 58.0) |
| `clear_param` | (84.4, 79.0) | `clear_input` | (75.4, 79.0) |
| `gates_output` .. `activity_output` | x = 8.0 + 12.57*i, y = 118.0 |  | i = 0..6 |
| `screw_tl` / `screw_tr` | (3.0, 3.0) / (88.4, 3.0) | `screw_bl` / `screw_br` | (3.0, 125.5) / (88.4, 125.5) |

These centers deliberately ignore the concept image's holes. The layout-proof phase may move them, after which both SVG anchors and C++ fallbacks must be updated together.

### 4.3 Required SVG anchor IDs

```text
playfield_rect

drop_param       rate_param       density_param
gravity_param    bounce_param     drag_param
tilt_param       chaos_param      clear_param

drop_input       gravity_cv_input bounce_cv_input
tilt_cv_input    chaos_cv_input   clear_input

gates_output     any_output       left_output      right_output
velocity_output  position_output  activity_output

screw_tl         screw_tr         screw_bl         screw_br
```

## 5. Rack API contract

Umi is unreleased, so these enums can be established cleanly. Once Umi is released, new IDs must only be appended.

```cpp
enum ParamId {
    DROP_PARAM,
    RATE_PARAM,
    DENSITY_PARAM,
    GRAVITY_PARAM,
    TILT_PARAM,
    BOUNCE_PARAM,
    DRAG_PARAM,
    CHAOS_PARAM,
    CLEAR_PARAM,
    PARAMS_LEN
};

enum InputId {
    DROP_INPUT,
    GRAVITY_CV_INPUT,
    TILT_CV_INPUT,
    BOUNCE_CV_INPUT,
    CHAOS_CV_INPUT,
    CLEAR_INPUT,
    INPUTS_LEN
};

enum OutputId {
    GATES_OUTPUT,
    ANY_OUTPUT,
    LEFT_OUTPUT,
    RIGHT_OUTPUT,
    VEL_OUTPUT,
    POS_OUTPUT,
    ACT_OUTPUT,
    OUTPUTS_LEN
};

enum LightId {
    DROP_LIGHT,
    CLEAR_LIGHT,
    SINK1_LIGHT,
    SINK2_LIGHT,
    SINK3_LIGHT,
    SINK4_LIGHT,
    SINK5_LIGHT,
    SINK6_LIGHT,
    SINK7_LIGHT,
    SINK8_LIGHT,
    ANY_LIGHT,
    LIGHTS_LEN
};
```

### 5.1 Parameter behavior

| Parameter | Rack range/default | Engine mapping |
| --- | --- | --- |
| `DROP` | momentary 0/1 | Rising edge creates one drop event |
| `RATE` | 0..1, default 0 | Below 0.02 is off; otherwise exponential 0.05..20 Hz |
| `DENSITY` | snapped 1..8, default 1 | Pearls spawned per drop event |
| `GRAV` | 0..1, default 0.45 | Linear 200..2200 board units/s² |
| `TILT` | -1..1, default 0 | Horizontal acceleration = tilt x gravity x 0.45 |
| `BOUNCE` | 0..1, default 0.55 | Restitution 0.15..0.92 |
| `DRAG` | 0..1, default 0.30 | Damping coefficient 0..4 s⁻¹ |
| `CHAOS` | 0..1, default 0.08 | Seeded micro-turbulence amount |
| `CLEAR` | momentary 0/1 | Remove all active pearls without emitting outputs |

Rate mapping may use `exp2`, but it is recalculated at control rate or only when the effective value changes. Physics-step drag uses a cached coefficient or the stable approximation `1 / (1 + drag * dt)`; it must not call `exp()` per pearl.

### 5.2 CV behavior

- The `DROP` button, `DROP` input, `CLEAR` button, and `CLEAR` input each use their own `dsp::SchmittTrigger`, with approximately 0.1 V low and 1.0 V high thresholds for voltage inputs.
- `TILT CV` is bipolar: `clamp(knob + voltage / 5, -1, 1)`.
- `GRAV`, `BOUNCE`, and `CHAOS` CV are additive bipolar modulation in normalized space: `clamp(knob + voltage / 5, 0, 1)`.
- Inputs are read monophonically from channel 0 in v1.
- Manual and external drop events are additive to the internal rate generator.

When rate is off, its phase resets to zero. When enabled, phase advances sample-accurately and emits a drop when it wraps. Density applies equally to manual, external, internal, and display-click drops.

### 5.3 Output behavior

- `GATES` emits 10 V pulses on eight polyphonic channels: channel 1 is sink 1 through channel 8 as sink 8. `ANY`, `LEFT`, and `RIGHT` emit monophonic 10 V pulses.
- Default pulse length is 10 ms. Context choices are 1, 5, 10, 20, and 50 ms.
- `LEFT` covers sinks 1-4; `RIGHT` covers sinks 5-8.
- Overlapping captures extend/retrigger the monophonic pulse; they do not create polyphony.
- `VEL` is sample-and-hold: `clamp(speed / 2400, 0, 1) * 10 V`.
- `POS` is sample-and-hold: `sinkIndex / 7 * 10 V`.
- `ACT` is 0..10 V. At each physics step its target includes active occupancy; captures add an impulse and the value decays with an approximately 350 ms time constant.
- If several captures occur in one physics step, all relevant trigger outputs fire. `VEL` and `POS` hold the final capture in deterministic ascending pearl-ID processing order.

## 6. Engine architecture

Separate Rack integration, the physics engine, layout data, and rendering:

```text
src/Umi.hpp
src/Umi.cpp
src/UmiEngine.hpp
src/UmiEngine.cpp
src/UmiLayout.hpp
src/UmiLayout.cpp
src/UmiWidget.cpp
tests/umi_engine_spec.cpp
```

`UmiEngine` and `UmiLayout` must remain Rack-independent so the main physics and determinism tests belong to `test-fast` rather than `test-rack`.

### 6.1 Fixed capacities

```cpp
constexpr int UMI_MAX_BALLS = 64;
constexpr int UMI_MAX_PEGS = 128;
constexpr int UMI_MAX_SEGMENTS = 96;
constexpr int UMI_SINK_COUNT = 8;
constexpr float UMI_BOARD_W = 1000.f;
constexpr float UMI_BOARD_H = 1600.f;
constexpr float UMI_PHYSICS_DT = 1.f / 240.f;
```

Use `std::array` plus active counts/free slots. Do not allocate, resize a vector, format strings, log, load assets, or take a mutex in `process()` or `stepPhysics()`.

Supported active-pearl limits are 16, 32, and 64; default 32. The default full-board policy is **ignore new pearls**. A context option may instead replace the oldest pearl.

### 6.2 State records

```cpp
struct UmiBall {
    Vec2 pos;
    Vec2 vel;
    float radius = 18.f;
    float age = 0.f;
    float lowSpeedTime = 0.f;
    uint32_t id = 0;
    bool active = false;
};

struct UmiPeg {
    Vec2 pos;
    float radius;
    uint8_t visualType;
};

struct UmiSegment {
    Vec2 a;
    Vec2 b;
    float radius;
    uint8_t material;
};

struct UmiSink {
    Vec2 pos;
    float radius;
    uint8_t outputIndex;
};
```

The layout uses fixed arrays and explicit counts. Rendering style fields never change collision behavior.

### 6.3 Simulation loop

`process()` accumulates `args.sampleTime`. While the accumulator is at least `UMI_PHYSICS_DT`, it runs one fixed step and subtracts the step. Limit catch-up to four steps per call and clamp discarded backlog so pathological host state cannot cause unbounded work.

Each fixed step performs:

1. Apply gravity, tilt acceleration, seeded chaos, and cached drag damping.
2. Integrate velocity and position.
3. Resolve board walls, capsule rails, and circular pegs.
4. Test swept travel against sink capture circles.
5. Apply lifetime, stuck, finite-value, and escape guards.
6. Publish a render snapshot after all pearls are stable for the step.

Broad collision rejection uses squared distance. Calculate a reciprocal square root only after an overlap is confirmed. Clamp speed to 2400 board units/s so a pearl travels no more than 10 units per physics step and cannot tunnel through normal 18-unit-radius geometry. No transcendental function is called per pearl per step.

### 6.4 Collision response

- Peg collision is circle against circle.
- Rail collision is circle against a capsule segment using closest-point projection.
- Push out penetration before applying velocity response.
- Apply restitution only when normal velocity points into the collider.
- Retain tangential velocity with a fixed material friction near 0.985.
- If centers coincide, use a deterministic fallback normal derived from pearl ID and collider index.
- Ball-to-ball collision is absent in v1.

Visible geometry and collision geometry must agree. Every runtime collider is drawn; collider-like decoration inside the aperture must be subdued.

### 6.5 Lifetime and safety

- Maximum age is 12 seconds; expiry emits no gate.
- Below 5 units/s, accumulate low-speed time.
- At 2 seconds, apply one small seeded nudge.
- At 5 seconds, remove the pearl without a gate.
- Remove any pearl with non-finite position or velocity.
- Remove pearls beyond x `[-300, 1300]` or y above 1900.
- Clear and seed changes never emit triggers.

## 7. Pearl layout

The one v1 layout is displayed as **Pearl** and built by `makePearlLayout(seed)`.

- Spawn center: `(500, 80)`.
- Spawn horizontal spread: +/-120 units.
- Spawned burst members receive small deterministic x and initial-velocity offsets so they do not perfectly overlap.
- The peg field is a regular staggered lattice with 11 rows. Odd-numbered rows contain 7 pegs except for the broad interior rows 3, 5, and 7, which contain 9; even-numbered rows contain 8 pegs.
- Seven-peg rows use x positions `170 + 110*column`; eight-peg rows use `115 + 110*column`; expanded nine-peg rows use `60 + 110*column`. Adjacent rows retain the 55-unit stagger, with the added outer pegs occupying the available sides of the raster shell.
- Row y positions are `190 + 105*row`, for rows 0-10; the final row is therefore at y 1240.
- Every ordinary peg has radius 22 and no seed-dependent position jitter.
- Sink centers span x 205 through 795 at y 1510 with even spacing and radius 38, for `i = 0..7`. The compact rings fit within the lower opening of the raster shell.
- Thin funnel-divider capsules run from y 1455 to 1585 with radius 4, leaving 193 logical units between the final peg edge and divider entrance.
- Mirrored capsule chains form the side walls. They sit slightly beyond the board edges through the upper field, then progressively sweep inward from roughly y 1000 to x 188/812 at the bottom, following the raster frame while preventing ambiguous exits.

The staggered grid is identical for every seed. Seed affects spawn and motion randomness, not visible board alignment. Do not add non-triggering spectacle until all eight sinks receive a useful share of captures at neutral settings.

### 7.1 Determinism

Use a local, explicitly specified 32-bit generator such as xorshift32. A zero JSON seed is remapped to a documented nonzero constant. The fixed layout consumes no random state. The runtime RNG is used only for spawn jitter, chaos, and cosmetic pearl variation derived from IDs.

With the same seed, sample rate, parameter/CV stream, and drop event stream, capture sink indices and fixed-step times must match exactly. Visual frame rate must not influence engine state.

Seed changes are available from the context menu. Randomize/copy/paste actions pass an explicit integer through the UI command queue, then reset RNG state, rebuild the fixed layout, clear pearls, and reset transient simulation timing.

## 8. Audio/UI handoff

The audio thread owns all physics, output, pulse, and light state. The UI never iterates the live engine arrays and never calls `spawnBall()` directly.

### 8.1 Render snapshots

Publish a bounded snapshot containing:

```cpp
struct UmiBallRenderState {
    Vec2 pos;
    Vec2 vel;
    float radius;
    float age;
    uint32_t id;
};

struct UmiRenderSnapshot {
    std::array<UmiBallRenderState, UMI_MAX_BALLS> balls;
    uint32_t ballCount;
    std::array<uint32_t, UMI_SINK_COUNT> captureSerial;
    uint32_t dropSerial;
    float activity;
    uint32_t seed;
    uint8_t layoutIndex;
};
```

Use a three-slot SPSC snapshot queue with atomic read/write indices. The audio thread writes only a free slot and skips publication when the queue is full; the UI drains the bounded queue to the newest snapshot each frame. The producer never overwrites an unread slot. A plain double buffer or seqlock over non-atomic payload fields is not sufficient because it can still create a C++ data race.

Capture and drop serials let the UI start frame-rate-local animations without requiring the audio thread to advance visual phases.

The renderer rebuilds and caches its read-only peg, rail, and sink drawing geometry through `makePearlLayout(snapshot.seed)` when the snapshot seed or layout index changes. It must not read the engine's live layout arrays. Layout generation is a pure deterministic operation shared by engine tests and renderer preparation.

### 8.2 UI commands

Display clicks and context-menu mutations use a fixed-capacity SPSC queue. Commands are POD records such as `DropAtX`, `SetSeed`, `Clear`, and `ResetBoard`. The audio thread drains a bounded number at the start of `process()`. If the queue is full, discard the newest UI gesture; never block the audio thread.

Any click inside the playfield queues `DropAtX`, clamped to the board's spawn range. Drag-to-tilt is deferred beyond v1.

## 9. Rendering architecture

Widget order is:

```text
ModuleWidget
  structural SVG panel and fallback fill
  cached raster background
  live playfield
    clipped water/background
    rails and pegs
    sinks
    pearls and optional short trails
    capture/conduit effects
  cached foreground ornament overlay
  controls, ports, lights, and screws
```

Static background and ornament layers live in `FramebufferWidget`s. The live playfield redraws continuously but uses simple precomputed paths/circles and the latest stable snapshot. Keep trails short, bounded, and disabled initially.

Pearls use a bright center, cyan rim, warm reflection, and small shadow. Ordinary pegs are restrained gold/pearl pins; larger bumpers may use purple jewel accents. Rails use a navy body, cyan edge, and fine gold highlight. The eight sinks remain visibly ordered left to right, matching channels 1 through 8 of `GATES`.

On capture, the UI animates a brief pearl bloom, sink flash, downward conduit pulse, and output-ring flash. Runtime lighting communicates state; fish, coral, broad caustics, the character, and crest remain static.

### 9.1 Board transform and clipping

Compute one aspect-preserving board transform from `playfield_rect` and use it for balls, radii, pegs, rails, sinks, debug geometry, and mouse conversion. Apply the production alpha mask or an equivalent NanoVG clip before drawing the live board. Never infer collision geometry from raster pixels.

### 9.2 Graphics lifecycle

All image operations occur on the UI thread. Persistent NanoVG handles must follow `src/NvgGraphicsLifecycle.hpp`:

- Record the owning `NVGcontext*` for every handle.
- Invalidate handles and cached framebuffers on context change.
- Validate persistent handles with `ownedNvgImageSizeMatches()` before reuse.
- Recreate lazily after DAW window close/reopen or context loss.
- Never delete a handle through a different context.
- Do not rely on destructor-time GL/NanoVG cleanup.

The full-resolution source is decoded only when needed. Prefer the 2x runtime asset at ordinary display scale and reserve 4x where pixel ratio justifies it.

### 9.3 Debug behavior

The collider overlay, counts, physics rate, snapshot status, and performance timing exist only when `isDragonKingDebugEnabled()` is true. Do not serialize the overlay setting or expose developer menu entries in normal builds. Any debug-terminal packet is similarly gated.

## 10. Persistence and menu

Use a schema version and clamp every loaded value:

```json
{
  "schema": 1,
  "seed": 1,
  "layout": 0,
  "maxBalls": 32,
  "pulseLengthMs": 10,
  "replaceOldest": false
}
```

Persist only the listed engine preferences. Parameters are already stored by Rack. Do not persist active pearls, pulse timers, clock phase, UI animation, debug state, or render snapshots. Unknown keys are ignored. Missing or malformed keys receive defaults.

The v1 context menu contains:

```text
Pulse length: 1 / 5 / 10 / 20 / 50 ms
Maximum pearls: 16 / 32 / 64
When full: Ignore new / Replace oldest
Randomize seed
Copy seed
Paste seed
Reset board
Clear pearls
```

`Reset board` rebuilds from the current seed and clears all transient state. `Clear pearls` only removes active pearls. Neither emits a trigger.

## 11. Repository integration

Implementation adds:

- `extern Model* modelUmi;` to `src/plugin.hpp`.
- `p->addModel(modelUmi);` to `src/plugin.cpp`.
- `Model* modelUmi = createModel<Umi, UmiWidget>("Umi");` in `src/UmiWidget.cpp`.
- A `plugin.json` entry initially marked `"hidden": true`, changed to false only at release readiness.
- Suggested tags: `Sequencer`, `Random`, `Clock generator`, and `Visual`.
- `tests/umi_engine_spec.cpp` and a `test-fast` Makefile target.

Suggested browser description:

> Visible pearl-physics probability sequencer with eight sink triggers and event CV.

## 12. Implementation phases

### Phase 1: layout proof

- Create `res/Umi.svg` at 18 HP with every required component anchor and fallback coordinate.
- Use a plain marine-blue placeholder panel.
- Place every real control, input, output, and screw.
- Verify labels and cable clearance at 75%, 100%, 125%, and 150% Rack zoom.

Exit condition: the complete hardware contract fits without covering the central aperture.

### Phase 2: headless engine

- Implement fixed storage, Pearl layout, spawning, integration, peg/capsule collision, sinks, guards, seed RNG, and capture events.
- Add the full focused test suite.
- Keep the engine free of Rack and graphics dependencies.

Exit condition: deterministic event sequences and bounded stress simulation pass under `test-fast`.

### Phase 3: Rack module

- Wire parameters, CV, Schmitt triggers, pulse generators, held CV, activity, persistence, menu settings, and the UI command queue.
- Register Umi as a hidden module.

Exit condition: all outputs and controls work with a placeholder renderer.

### Phase 4: live renderer

- Add three-slot SPSC snapshots, board transform, clipping, visible colliders, pearls, sink flashes, and output-ring feedback.
- Add the debug-only physics overlay.

Exit condition: visual cause and electrical effect remain unambiguous under dense operation.

### Phase 5: production art

- Clean and extend the source art into layered 18 HP assets.
- Integrate cached background/frame/overlay layers using shared NanoVG lifecycle helpers.
- Tune labels, glow, and contrast around real widget positions.

Exit condition: no false holes remain, the board dominates, and the UI retains the source image's undersea identity without sacrificing legibility.

### Phase 6: release validation

- Run `make test-fast` and the focused Umi test binary in the current environment.
- In WSL, treat full plugin linking as non-authoritative and hand final linking to the Windows/MSYS2 toolchain.
- Stress 64 pearls at 20 Hz and density 8.
- Exercise graphics-context close/reopen behavior.
- Verify save/load and same-seed replay.
- Set `hidden` false only after authoritative Windows/MSYS2 build and Rack smoke testing.

## 13. Focused test contract

`umi_engine_spec` covers at least:

1. One drop creates the requested number of active pearls.
2. Ignore-new and replace-oldest capacity policies are exact.
3. A pearl entering each sink reports the correct index and is removed once.
4. Aggregate left/right/any event classification is correct.
5. Same seed and event stream produce identical sink indices and fixed-step capture times.
6. Different seeds produce a changed path sequence without invalid layout geometry.
7. Every layout count is within capacity; colliders, spawn point, and sinks are in bounds.
8. Ten thousand or more physics steps produce no NaN, runaway count, or escaped live pearl.
9. Age expiry, stuck recovery/removal, clear, reset, and reseed emit no false capture.
10. Neutral settings exercise all eight sinks over a statistically useful deterministic run.
11. Positive and negative tilt materially shift capture distribution right and left.
12. Maximum control values remain bounded and complete within a test-time budget.

Manual Rack checks cover direct outputs 1-8, all aggregate triggers, VEL/POS hold behavior, ACT response, simultaneous captures, patch reload, context-menu settings, display click-to-drop, cable clearance, and graphics-context recreation.

## 14. Final acceptance criteria

Umi v1 is ready to unhide when:

- It builds and links in the authoritative Windows/MSYS2 Rack toolchain.
- `make test-fast` and `umi_engine_spec` pass.
- CPU work remains bounded at the 64-pearl stress case without audio-thread allocation or locks.
- Neutral controls produce a musically useful distribution across all eight sinks.
- Tilt, gravity, bounce, drag, and chaos have distinct, audible consequences.
- Same-seed replay is deterministic under the stated contract.
- Each capture visibly maps to exactly one numbered direct output.
- No painted collider lies about the physics and no empty fake control hole remains.
- Raster resources survive graphics-context recreation without stale-handle access.
- The playfield remains readable with typical patch cables attached.
- The result reads immediately as **Umi**: a bright, strange, premium undersea probability instrument.
