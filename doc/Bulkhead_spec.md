# Bulkhead Module Specification

Bulkhead is a stereo room reverb for VCV Rack built around a visible, playable room model. It should not present itself as a generic reverb with a room graphic. The sound engine should derive the direct path and early reflections from a simple editable room, then feed a quality-scaled late reverb network that inherits the room's size, absorption, and motion.

The v1 target is a practical 2.5D shoebox room: the panel exposes a top-down 2D room editor, while height/floor/ceiling assumptions remain fixed or secondary. This keeps the module playable and implementable while leaving room for deeper acoustic modeling later.

## Product Identity

Bulkhead should feel like placing a source and listener inside a physical compartment:

- Moving the source changes direct-path timing, stereo direction, and early reflection timing.
- Moving the listener changes perceived position and reflection balance.
- Moving walls changes room size, reflection timing, and late-field character.
- Changing material changes reflection brightness, energy loss, and decay tone.
- Late reverb remains musical, dense, and stable rather than a raw physics demo.

The module's promise is spatial control first, reverb utility second, and physically informed behavior as the means rather than the UI burden.

## V1 Scope

### In Scope

- Stereo audio input and stereo audio output.
- Mono-compatible behavior when only left input is patched.
- One listener with position and yaw.
- One mono speaker or linked stereo speaker pair, with user-editable position and rotation.
- Rectangular top-down room with four movable walls.
- Direct path and first-order image-source reflections.
- Optional second-order reflections in higher quality modes.
- Per-wall material presets and global material macro control.
- FDN late field with Eco, Studio, and HiFi quality modes.
- Early/late balance, decay, diffusion, motion, air/brightness, and mix controls.
- Cached custom room canvas with draggable speaker, listener, and wall handles.
- Parameter smoothing and state crossfades to avoid zipper noise.
- Focused tests for geometry timing, decay stability, and interpolation safety.

## MVP Implementation Handoff

This section is the authoritative starting point for an initial implementation/scaffolding pass. Later sections describe the broader v1 direction, but the MVP should stay inside this smaller contract until it compiles, appears in Rack, and has testable geometry/DSP utilities.

### MVP Product Shape

- Panel width: 16 HP.
- Panel asset: `res/bulkhead.svg`.
- Main visible feature: a large custom room widget occupying the upper panel.
- Room widget shows a rectangular room, four movable walls, one listener with yaw, and either one mono speaker or a linked stereo speaker pair.
- Mouse interaction is the primary editor for room layout, speaker placement/rotation, and listener placement/rotation.
- CV modulation should initially target listener position and wall distance, not material selection.
- Material selection is menu/dropdown based.
- Floor and ceiling material slots exist in state from the start, even if they only affect late damping later.

### MVP Panel Anchors

Use the hidden `components` layer in `res/bulkhead.svg` through `PanelSvgUtils`. These IDs already exist in the panel mockup and should be treated as the initial UI contract:

| SVG ID | Purpose |
| --- | --- |
| `room_canvas` | Custom room editor widget bounds. |
| `decay_param` | Decay knob. |
| `diffuse_param` | Diffusion/density knob. |
| `mix_param` | Dry/wet mix knob. |
| `absorb_param` | Global absorption macro knob. |
| `early_late_param` | Early/late balance knob. |
| `motion_param` | Late-field motion amount knob. |
| `lst_x_input` | Listener X CV input. |
| `lst_y_input` | Listener Y CV input. |
| `wall_left_input` | Bipolar left-wall distance CV input. |
| `wall_right_input` | Bipolar right-wall distance CV input. |
| `wall_front_input` | Bipolar front-wall distance CV input. |
| `wall_back_input` | Bipolar back-wall distance CV input. |
| `in_l_input` | Left/mono audio input. |
| `in_r_input` | Optional right audio input. |
| `out_l_output` | Left audio output. |
| `out_r_output` | Right audio output. |

### MVP Params, Inputs, Outputs

Use these exact enum names for the first pass unless there is a compelling local naming conflict:

```cpp
enum ParamId {
    DECAY_PARAM,
    DIFFUSE_PARAM,
    MIX_PARAM,
    ABSORB_PARAM,
    EARLY_LATE_PARAM,
    MOTION_PARAM,
    PARAMS_LEN
};

enum InputId {
    LST_X_INPUT,
    LST_Y_INPUT,
    WALL_LEFT_INPUT,
    WALL_RIGHT_INPUT,
    WALL_FRONT_INPUT,
    WALL_BACK_INPUT,
    IN_L_INPUT,
    IN_R_INPUT,
    INPUTS_LEN
};

enum OutputId {
    OUT_L_OUTPUT,
    OUT_R_OUTPUT,
    OUTPUTS_LEN
};

enum LightId {
    LIGHTS_LEN
};
```

No `Material` knob, material CV, source CV, listener yaw CV, size CV, freeze button, or quality button is required for the initial panel scaffold. Those can be added later after the core module exists.

Suggested parameter ranges:

| Param | Range | Default | Notes |
| --- | ---: | ---: | --- |
| `DECAY_PARAM` | 0.1..30 s | 2.5 s | Configure logarithmic/display mapping if convenient. |
| `DIFFUSE_PARAM` | 0..1 | 0.55 | Low is specular/roomy, high is smooth/dense. |
| `MIX_PARAM` | 0..1 | 0.35 | Dry/wet. |
| `ABSORB_PARAM` | 0..1 | 0.35 | Global absorption macro. |
| `EARLY_LATE_PARAM` | -1..+1 | 0 | Negative favors early geometry, positive favors late tail. |
| `MOTION_PARAM` | 0..1 | 0.15 | No-op until late FDN motion exists. |

### MVP Geometry CV Mapping

- `LST_X_INPUT`: -5V..+5V maps listener x to room left..right.
- `LST_Y_INPUT`: -5V..+5V maps listener y to room bottom..top.
- Wall CV inputs are bipolar offsets around the current/manual wall placement.
- For wall CV, 0V means no modulation, +5V moves that wall farther from the listener, and -5V moves that wall nearer to the listener.
- Wall CV must clamp so the room never inverts and the listener always keeps a minimum wall distance.
- Use a conservative minimum listener-wall distance of 0.5 m for wall modulation.
- Use a 0.1 m minimum source/listener-to-wall placement margin for direct placement.

### MVP Code Setup Checklist

The initial coding pass should create only enough Rack integration and testable support code to make Bulkhead real in the codebase:

1. Add `extern Model* modelBulkhead;` to `src/plugin.hpp`.
2. Add `p->addModel(modelBulkhead);` to `src/plugin.cpp`.
3. Add a `Bulkhead` entry to `plugin.json` with slug/name `Bulkhead`.
4. Add `src/Bulkhead.hpp` and `src/Bulkhead.cpp` with the module class, enums, constructor config, default state, and pass-through/silent-safe audio process.
5. Add `src/BulkheadWidget.cpp` with `BulkheadWidget`, `createModel<Bulkhead, BulkheadWidget>("Bulkhead")`, panel loading from `res/bulkhead.svg`, anchor loading via `PanelSvgUtils`, and placeholder room widget placement.
6. Add `src/BulkheadGeometry.hpp/.cpp` with Rack-free geometry structs and first-order reflection path target generation.
7. Add a focused geometry unit test before implementing the full reverb tail.
8. Do not add module source files to the Makefile unless the build has changed; the repo already uses `SOURCES += $(wildcard src/*.cpp)`.
9. Update the Makefile only for a new focused geometry test target if needed.

For the first scaffold, `process()` may output dry input, silence, or a simple safe wet placeholder. Do not block module setup on the final FDN.

### MVP Scaffold Acceptance Criteria

The initial handoff is successful when:

- `plugin.json`, `src/plugin.hpp`, and `src/plugin.cpp` register `Bulkhead`.
- Rack can instantiate the module and display `res/bulkhead.svg`.
- The custom room widget occupies the `room_canvas` anchor and can draw placeholder walls/listener/speakers.
- All six knob params and all ten ports are placed from SVG anchors.
- `BulkheadGeometry` has Rack-free tests for direct distance and first-order image-source reflection positions.
- `test-fast` still passes, or any failure is clearly unrelated to Bulkhead.
- In WSL/WSL-like environments, full plugin link failures are not treated as authoritative regressions.

### MVP Non-Goals

- Do not implement full FDN before the module scaffold and geometry tests exist.
- Do not implement arbitrary polygon rooms.
- Do not implement source CVs, yaw CV, material CV, or freeze in the initial pass.
- Do not add polyphonic per-channel room simulation.
- Do not perform heap allocation or SVG parsing in `process()`.
- Do not depend on UI frame rate for DSP state.

### Deferred

- Full convolution or imported impulse responses.
- Arbitrary polygon rooms.
- True 3D room editing on the main panel.
- Independent per-polyphonic-channel room simulation.
- Binaural/HRTF rendering.
- User-editable octave-band material curves.
- Explicit floor/ceiling reflection paths.
- Full ambisonic or surround output.

## Signal Flow

```text
Input L/R
  |
  +--> Speaker model / mono-stereo placement
  |
  +--> Direct path delay + distance gain + stereo renderer
  |
  +--> First-order image-source reflection taps
  |      + optional second-order taps in Studio/HiFi
  |      + per-path material filters
  |      + distance/air loss
  |      + stereo directional renderer
  |
  +--> Late-field send derived from early energy
         + FDN delay network
         + damping / material coloration
         + motion modulation
         + stereo decorrelation
  |
Dry/Wet + Early/Late mix
  |
Output L/R
```

Direct and early paths are geometric and intentionally readable. The late field is statistical and optimized for density, but its delay scale, damping, and energy input are derived from the current room state.

## Front Panel Interface

### Audio I/O

| Port | Type | Behavior |
| --- | --- | --- |
| `IN L` | Audio input | Left/mono input. If `IN R` is unpatched, duplicate to right speaker path. |
| `IN R` | Audio input | Optional right input. Enables linked stereo speaker pair. |
| `OUT L` | Audio output | Stereo left output. |
| `OUT R` | Audio output | Stereo right output. |

### Primary Controls

| Control | Range | CV | Description |
| --- | --- | --- | --- |
| `Decay` | 0.1..30 s | none in MVP | Target late-field RT60. Material absorption can shorten effective decay. |
| `Absorb` | 0..1 | none in MVP | Scales wall absorption and late damping. Low is reflective, high is dry/damped. |
| `Diffuse` | 0..1 | none in MVP | Controls early-to-late scattering and FDN density character. Low is specular/roomy, high is smooth/dense. |
| `Motion` | 0..1 | none in MVP | Adds stable late-tail motion via feedback-matrix modulation and very slow delay modulation. |
| `Early/Late` | -1..+1 | none in MVP | Crossfades emphasis between geometric early field and diffuse late tail. |
| `Mix` | 0..1 | none in MVP | Dry/wet mix. |

The MVP panel uses the six controls above as one compact macro row. `Decay`, `Diffuse`, `Mix`, `Absorb`, `Early/Late`, and `Motion` are the only required v1 panel knobs at initial scaffold time. Material selection is not a knob in the current panel mockup; it is handled through menus/dropdowns.

### Geometry CV

| Port | Target | Range Mapping |
| --- | --- | --- |
| `LST X` | Listener x position | -5V..+5V maps to room left..right. |
| `LST Y` | Listener y position | -5V..+5V maps to room bottom..top. |
| `LEFT` | Left wall distance from listener | Bipolar offset around manual wall position; + moves farther, - moves nearer. |
| `RIGHT` | Right wall distance from listener | Bipolar offset around manual wall position; + moves farther, - moves nearer. |
| `FRONT` | Front wall distance from listener | Bipolar offset around manual wall position; + moves farther, - moves nearer. |
| `BACK` | Back wall distance from listener | Bipolar offset around manual wall position; + moves farther, - moves nearer. |

The MVP panel exposes listener X/Y and four wall-distance CVs. Speaker position, speaker rotation, and listener yaw are edited from the room canvas rather than dedicated CV ports in the initial panel.

### Buttons / Modes

| Control | Behavior |
| --- | --- |
| `Quality` | Eco / Studio / HiFi. Context-menu item for MVP. |
| `Air` | Enables distance-dependent high-frequency loss. Context-menu item for MVP. |
| `Rays` | Toggles visual reflection rays. Display-only context-menu item for MVP. |
| `Freeze` | Deferred. Leave room in architecture, but do not add it to the initial panel scaffold. |

## Room Canvas

The room canvas is the primary interaction surface.

### Visual Elements

- Rectangular room boundary with four wall handles.
- Speaker node: one speaker for mono input, linked L/R speaker pair for stereo input.
- Speaker direction indicator showing rotation within the room.
- Listener node: head/ear icon with yaw indicator.
- Optional first-order ray overlay.
- Material coloring or texture along walls.
- Active CV target highlight when a geometry CV is patched.
- Subtle late-field glow or density haze, driven from smoothed parameters, not audio-thread analysis.

### Interactions

- Drag speaker body to change speaker position.
- Drag speaker direction handle to rotate speaker aim.
- In stereo mode, drag either speaker to move the linked pair while preserving spacing and relative rotation.
- In stereo mode, rotate either speaker to rotate the linked pair around its midpoint unless unlinked.
- Drag listener to change listener position.
- Drag listener yaw handle to rotate listening direction.
- Drag wall handles to resize room asymmetrically.
- Shift-drag or context menu for precise values if needed.
- Double-click speaker/listener/wall to reset that target.
- Right-click wall to assign material override from a menu/dropdown.
- Right-click speaker to link/unlink stereo placement, reset rotation, or enter precise position/rotation values.
- Right-click canvas to show quality, overlay, air, and advanced options.

### Rendering Policy

Use a custom Rack widget inside a cached framebuffer. Mark the framebuffer dirty only when room geometry, material, display size, or overlay state changes. Do not do expensive text layout or path recomputation every frame. Keep audio-thread data transfer to simple atomics or UI snapshots.

## Coordinate Model

Use normalized UI coordinates internally, then map to physical meters for DSP.

```text
room.left   <= speaker.x/listener.x <= room.right
room.bottom <= speaker.y/listener.y <= room.top
```

Recommended physical range:

- Minimum room dimension: 1.0 m.
- Maximum room dimension: 30.0 m.
- Default room: 8.0 m wide, 5.0 m deep.
- Listener default: center-right, facing source.
- Mono speaker default: center-left, aimed toward listener.
- Stereo speaker pair default: left/front pair around the mono source default, aimed toward listener, linked.
- Speed of sound: 343 m/s at default temperature.

For safety, keep source/speaker and listener positions at least 0.1 m away from walls after smoothing/clamping.

### Speaker Placement And Rotation

Bulkhead should treat sources as visible speakers, not abstract dots. Position controls define where the speaker acoustic origin sits in the room. Rotation defines where the speaker faces. The speaker icon must remain fully inside the rectangular room bounds after placement, rotation, smoothing, and wall movement.

Mono input uses one speaker:

```text
speaker.position = user/source position
speaker.yaw = user/source rotation
```

Stereo input uses a linked L/R speaker pair by default:

```text
speakerPair.center = user/source position
speakerPair.yaw = user/source rotation
speakerL/R.position = center +/- rotatedPairOffset
speakerL/R.yaw = pair yaw
```

The linked stereo pair preserves speaker spacing and relative orientation while dragging or rotating. A context-menu unlink mode may allow independent L/R placement later, but v1 should keep linked placement as the default and primary behavior.

Bounding rules:

- Clamp speaker acoustic origins inside the room using the same margin as the source/listener minimum wall distance.
- Also account for a small icon footprint in the canvas so the drawn speaker body does not visually cross the wall.
- Rotation is continuous and wraps at +/-180 degrees; it is not clamped by wall angle.
- If a wall edit makes the current speaker placement invalid, move the speaker or linked pair minimally back inside the valid bounds.
- If the linked stereo pair no longer fits in a very small room, reduce displayed pair spacing down to a minimum visual separation before collapsing to a mono-like pair center.

DSP interpretation:

- Direct/reflection timing still uses the speaker acoustic origin position.
- Speaker yaw affects directional emission gain before the direct/early path sends.
- V1 can use a simple cardioid-like forward bias rather than a physically exact loudspeaker model.
- A rotation amount of 0 should face the default listener direction in the default patch.

## DSP Specification

### Direct Path

The direct path is a fractional delay from source to listener.

```text
distance = length(listener - source)
delaySamples = sampleRate * distance / speedOfSound
gain = directGain(distance)
pan = listenerRelativeStereoPan(source, listener, yaw)
```

Direct gain should be bounded and musical:

```text
gain = 1 / (1 + distance * directDistanceScale)
```

Avoid raw `1/r` singularities. Direct path should be disabled or heavily reduced when `Mix` is fully wet only if the dry signal is also removed; otherwise it remains part of the wet room model.

### Early Reflections

Use image-source reflection positions.

First-order reflections:

```text
left:   image.x = 2 * wall.left   - source.x
right:  image.x = 2 * wall.right  - source.x
bottom: image.y = 2 * wall.bottom - source.y
top:    image.y = 2 * wall.top    - source.y
```

Second-order reflections in Studio/HiFi should include adjacent wall pairs and opposite wall pairs, capped to a small fixed list. Do not generate arbitrary recursive image sources in v1.

Each reflection path computes:

```text
distance = length(listener - imageSource)
delaySamples = sampleRate * distance / speedOfSound
wallReflectance = sqrt(1 - absorption)
distanceGain = 1 / (1 + distance * reflectionDistanceScale)
pathGain = wallReflectanceProduct * distanceGain
```

Path output chain:

```text
fractionalDelay -> materialFilter -> airFilter -> gain -> stereoRenderer
```

### Fractional Delay

Use 4-point Lagrange or cubic Hermite interpolation for direct and early paths. These paths carry localization cues and should get the better interpolation.

Requirements:

- Delay time changes must be smoothed.
- Large discontinuities should crossfade between old and new geometry states over 10..30 ms.
- Delay buffers must cover maximum room diagonal plus safety margin.
- Denormal protection is required in feedback and filter paths.

### Material Model

Material presets should store octave-band-style absorption intent, even if v1 implements simplified filters.

Initial material families:

| Material | Character | Absorption Intent |
| --- | --- | --- |
| Concrete | hard, bright | very low broadband absorption |
| Metal | hard, glossy | low absorption, slight high emphasis possible |
| Wood | warm, controlled | moderate high-frequency absorption |
| Plaster | neutral room | moderate broadband absorption |
| Curtain | soft high damping | high HF absorption, moderate low-mid retention |
| Carpet/Foam | dry, close | high absorption and strong HF loss |

V1 implementation can use low-order IIR filters per reflection path:

- One low-pass or shelving stage for HF absorption.
- One gain stage for total reflectance.
- Optional low-mid damping for soft materials.

Future implementation can fit minimum-phase IIR filters from octave-band material curves without changing the panel model.

### Floor And Ceiling Materials

V1 should keep the editable room as a top-down 2D model, but the material system should include floor and ceiling material slots from the start. These surfaces may not receive explicit early-reflection modeling in v1, but users should be able to configure them so the room material model is not artificially limited to four walls.

MVP policy:

- Store floor and ceiling material slots internally with sensible defaults.
- Default floor: wood or neutral hard floor.
- Default ceiling: plaster or neutral ceiling.
- Do not expose floor/ceiling controls on the main panel in v1.
- Do not require explicit floor/ceiling image-source paths in v1 early reflections.
- Provide floor and ceiling material entries in an advanced/context menu.
- Let floor/ceiling materials influence the late field as a coarse damping contribution when feasible.

Post-MVP option:

- Optionally add first-order vertical reflections using fixed room height, listener height, and speaker height.
- Fold floor/ceiling absorption into late-tail damping and mixing time more explicitly.
- Keep this separate from wall material overrides so top-down room editing remains readable.

This keeps the architecture from assuming "four walls only" while avoiding a main-panel UI burden before the audible value of explicit vertical modeling is proven.

### Air Absorption

When enabled, air absorption applies distance-dependent high-frequency damping to direct/early paths and late-field damping. Keep it cheap:

```text
cutoff = mapDistanceToAirCutoff(distance, airAmount)
```

Air should be subtle at default room sizes and more obvious in large rooms.

### Late Reverb FDN

The late engine is a feedback delay network. It should receive a decorrelated sum of direct/early energy rather than raw input only.

Quality modes:

| Mode | FDN Order | Reflections | Intended Use |
| --- | ---: | --- | --- |
| Eco | 8 | first-order only | low CPU, live patching |
| Studio | 12 | first + selected second-order | default quality |
| HiFi | 16 | first + richer second-order | dense tails, final patches |

FDN requirements:

- Delay lengths are incommensurate and scaled by room size.
- Feedback matrix is unitary or energy-controlled.
- Feedback gain is derived from `Decay` and delay length.
- Damping filters implement frequency-dependent decay.
- Output taps are stereo-decorrelated.
- Tail remains stable for maximum decay and maximum motion.

Initial feedback matrix policy:

- Eco: Householder-style matrix for low CPU.
- Studio: optimized fixed 12x12 matrix or embedded Householder/Hadamard hybrid.
- HiFi: Hadamard-style or optimized fixed 16x16 matrix.

Do not expose matrix type on the panel. If needed, put it in an advanced context menu later.

### Motion

Motion should not be implemented by aggressively modulating every delay line. Primary motion source should be slow, stable modulation of late-field mixing/feedback behavior.

Motion sources:

- Very slow random/LFO modulation of unitary matrix interpolation.
- Small delay-length modulation inside late field only where artifact-safe.
- Optional tiny material/damping modulation.

Constraints:

- Motion amount 0 must be deterministic and static.
- Motion must preserve stability at high decay.
- Direct and early reflections should move only from actual room/source/listener changes, not fake chorus.

### Parameter Smoothing

Default smoothing targets:

| Parameter Class | Smoothing |
| --- | ---: |
| Speaker/listener position | 10..20 ms |
| Speaker rotation | 10..20 ms |
| Listener yaw | 10..20 ms |
| Wall position/size | 20..50 ms |
| Material/absorb/diffuse | 20..100 ms depending on filter redesign cost |
| Mix/Early-Late | 5..20 ms |
| Decay | 50..200 ms |
| Quality mode | crossfade/reinitialize safely, not audio-rate smooth |

Preset loads and major discontinuities should crossfade between old and new wet states rather than slewing through impossible room paths.

## Stereo Rendering

The renderer should be simple and robust for v1.

For each direct/reflection path:

1. Compute direction from listener to virtual source.
2. Rotate by listener yaw.
3. Compute speaker emission bias from speaker yaw and path direction.
4. Convert horizontal listener-relative angle to equal-power stereo pan.
5. Optionally reduce high-frequency content for paths behind the listener using a subtle rear damping factor.

This is not HRTF. It should be a stable stereo room cue that reads in Rack patches.

## State Serialization

Persist:

- Geometry: room bounds, speaker position(s), speaker rotation, listener position, listener yaw.
- Macro params/menu state: decay, global material default, absorb, diffuse, motion, early/late, mix, air enabled, quality mode.
- Per-wall material overrides.
- Floor and ceiling material slots.
- UI options: ray overlay, linked stereo speaker mode.
- Schema version.

Do not serialize transient DSP buffers, smoothing states, or FDN contents.

## Suggested Internal Files

```text
src/Bulkhead.hpp
src/Bulkhead.cpp
src/BulkheadDSP.hpp
src/BulkheadDSP.cpp
src/BulkheadGeometry.hpp
src/BulkheadGeometry.cpp
src/BulkheadFDN.hpp
src/BulkheadFDN.cpp
src/BulkheadMaterials.hpp
src/BulkheadMaterials.cpp
src/BulkheadWidget.cpp
res/bulkhead.svg
```

Keep geometry math and DSP kernels testable without Rack where possible.

## Data Structures

```cpp
struct BulkheadVec2 {
    float x = 0.f;
    float y = 0.f;
};

struct BulkheadRoom {
    float left = -4.f;
    float right = 4.f;
    float bottom = -2.5f;
    float top = 2.5f;
    float height = 3.f;
};

struct BulkheadSurfaceMaterials {
    int wallLeft = 0;
    int wallRight = 0;
    int wallBottom = 0;
    int wallTop = 0;
    int floor = 0;
    int ceiling = 0;
};

struct BulkheadPose {
    BulkheadVec2 position;
    float yawRadians = 0.f;
};

struct BulkheadSpeaker {
    BulkheadPose pose;
    float directivity = 0.35f;
};

struct BulkheadSpeakerPair {
    BulkheadSpeaker left;
    BulkheadSpeaker right;
    bool linked = true;
    float spacingMeters = 0.8f;
};

struct BulkheadReflectionPath {
    float delaySamples = 0.f;
    float gain = 0.f;
    float pan = 0.f;
    int materialIndex = 0;
    int order = 0;
};
```

The process path should consume a smoothed immutable snapshot of geometry/material state rather than reading UI state directly.

## Movement Performance Policy

Movement is central to Bulkhead, but movement must not turn the audio thread into a geometry engine. Treat this section as an implementation constraint, not a tuning suggestion.

### Required Runtime Split

Separate the system into three layers:

1. UI/CV target state: raw canvas edits, CV inputs, context-menu changes, and preset loads.
2. Smoothed control state: speaker/listener/wall/material values after clamping, wrapping, smoothing, and crossfade policy.
3. Audio path state: compact per-path targets consumed by the sample loop.

The audio sample loop should consume already-prepared path targets:

```cpp
struct BulkheadPathTarget {
    float delaySamples;
    float gain;
    float pan;
    int materialIndex;
    int order;
};
```

The sample loop may interpolate delay, gain, pan, and filter coefficients toward these targets. It must not search room geometry, rebuild image sources, allocate memory, parse UI state, or redesign filters per sample.

### Geometry Update Rate

Image-source geometry and path target generation should run at control rate or event rate, not audio rate.

Recommended policy:

- Recompute direct/early path targets once per audio block, or less often if no smoothed geometry value changed beyond epsilon.
- Use smaller epsilons for speaker/listener position and rotation; use larger epsilons for wall changes.
- Wall movement should be smoothed more slowly than speaker/listener movement because it invalidates more path geometry.
- Preset loads or large wall jumps should create a short crossfade between old and new path target sets instead of stepping all delays instantly.
- Quality mode determines a hard maximum path count; no mode may generate unbounded reflection paths.

Suggested hard caps:

| Mode | Direct Paths | First Order | Second Order | Total Early Paths |
| --- | ---: | ---: | ---: | ---: |
| Eco | 1 mono or 2 stereo | 4 per speaker | 0 | <= 10 |
| Studio | 1 mono or 2 stereo | 4 per speaker | selected 8 per speaker | <= 26 |
| HiFi | 1 mono or 2 stereo | 4 per speaker | selected 12 per speaker | <= 34 |

These caps are intentionally small. The late FDN is responsible for density; the geometric front end is responsible for readable spatial cues.

### Material And Filter Updates

Material changes should be target-based and rate-limited.

- Material preset lookup and filter target computation may happen at control rate.
- Filter coefficients should smooth or crossfade; do not redesign material filters every sample.
- Per-wall, floor, and ceiling material changes should mark dependent path targets dirty.
- If floor/ceiling materials only affect late damping in MVP, update that damping target at control rate.

### Allocation And Threading Constraints

- No heap allocation in `process()`.
- No `std::vector` resizing in the audio thread after initialization.
- Preallocate maximum path arrays for the active quality mode or for the maximum supported quality.
- Use double-buffered or versioned snapshots for UI-to-DSP handoff.
- Snapshot publication must be lock-free or use a non-blocking pattern already accepted elsewhere in the plugin.
- UI redraws are independent from DSP geometry updates; the room canvas dirty flag must not control audio recomputation.

### Movement Semantics

Speaker/listener motion and wall motion have different costs and should feel different:

- Speaker/listener position and speaker/listener rotation can feel responsive, with 10..20 ms smoothing.
- Wall edits can feel heavier, with 20..50 ms smoothing and path-set crossfades for large changes.
- Rotation changes should update directivity/pan targets without changing geometric delay unless speaker position also changes.
- Linked stereo speaker movement should update both speaker path sets together from one linked-pair transform.

### Performance Validation

Performance tests should include moving-state cases, not only static impulse cases:

- Continuous speaker drag for several seconds.
- Continuous speaker rotation for several seconds.
- Continuous listener yaw change for several seconds.
- Slow wall resize while audio is running.
- CV-rate modulation of `LST X`, `LST Y`, and the four wall-distance inputs.
- Quality mode switch during active audio.

No test should show NaN/Inf output, unbounded CPU growth, per-block allocation, or audible single-sample jumps.

## Implementation Phases

### Phase 1: Geometry And Early Reflection Core

- Create geometry utilities for room bounds, source/listener pose, and first-order image sources.
- Add speaker placement utilities for clamping position, linked stereo spacing, and rotation wrapping.
- Implement block/control-rate path target generation, not per-sample image-source rebuilding.
- Implement direct path and first-order reflection delay computation.
- Implement dynamic delay line with 4-point interpolation.
- Implement simple material gain/low-pass stage.
- Produce stereo wet output without FDN.
- Add tests for expected delay samples from known geometry.

Exit criteria:

- Impulse produces direct path plus four first-order reflections at expected sample positions.
- Moving source/listener changes delay smoothly without clicks under smoothed modulation.
- Rotating the speaker changes direct/early emphasis without changing geometric delay.
- Mono input produces stable stereo output.

### Phase 2: Late FDN Core

- Implement 8-channel FDN first.
- Add delay-length scaling from room dimensions.
- Add decay-derived feedback gains and damping filters.
- Add stereo output decorrelation.
- Add Eco/Studio/HiFi topology selection after 8-channel core is stable.

Exit criteria:

- FDN output remains finite and follows the expected decay envelope for static impulse tests.
- Maximum decay remains finite and denormal-safe.
- Quality modes produce increasing density without changing basic wet level radically.

### Phase 3: Material And Air Model

- Add material presets.
- Add global material default menu/state and the `Absorb` macro control.
- Add per-wall material overrides through wall context menus/dropdowns.
- Add floor and ceiling material defaults and advanced/context-menu selection.
- Add optional air absorption.

Exit criteria:

- Soft materials visibly and audibly reduce reflection energy and brightness.
- Air absorption has distance-dependent effect and can be bypassed.
- Material changes do not click.

### Phase 4: Rack Module And Panel

- Add module IDs, params, ports, lights if needed.
- Create initial `res/bulkhead.svg` panel.
- Implement room canvas widget with draggable nodes/walls.
- Add speaker body drag, speaker rotation handle, and linked stereo pair movement/rotation.
- Add context menu for quality, air, ray overlay, linked stereo source, and material overrides.
- Persist state with schema version.

Exit criteria:

- Room canvas controls actual DSP state.
- CV and UI edits stay synchronized.
- Framebuffer caching avoids excessive UI redraw cost.
- UI redraw throttling does not affect audio geometry update correctness.

### Phase 5: Polish And Validation

- Tune defaults and gain staging.
- Add second-order reflections for Studio/HiFi.
- Add stable motion modulation.
- Decide whether floor/ceiling materials affect only late damping in v1 or also receive explicit vertical reflection paths.
- Add final tests and CPU profiling at 48/96/192 kHz.

Exit criteria:

- No runaway feedback at max decay/motion.
- No obvious zipper noise under normal CV movement.
- Moving speaker/listener/walls do not allocate or rebuild unbounded state in `process()`.
- CPU is acceptable in Studio mode at 48 kHz.

## Testing Plan

### Unit Tests

- Geometry reflection positions for each wall.
- Direct-path delay for known source/listener distances.
- First-order reflection delay for known room geometry.
- CV mapping clamps source/listener inside room.
- Speaker placement clamps inside room after wall edits.
- Speaker rotation wraps cleanly across +/-180 degrees.
- Linked stereo pair preserves spacing while dragging and rotating when the room can contain it.
- Material reflectance maps absorption to gain correctly.
- Floor/ceiling material defaults serialize safely once included in state.
- FDN feedback gain decreases as decay decreases.
- Serialization round-trip preserves room state.

### DSP Regression Tests

- Static impulse response has finite output and expected early peaks.
- Moving source with smoothed CV produces finite output and no extreme sample jumps.
- Moving speaker/listener/wall scenarios do not allocate in the audio thread.
- Geometry target recomputation stays bounded under continuous CV movement.
- Max decay with silence input decays or remains bounded.
- Material change crossfade avoids single-sample spikes.
- Quality mode switch does not produce NaN/Inf.

### Listening Checks

- Small concrete room: bright slap and short dense tail.
- Large soft room: delayed, dark reflections and controlled decay.
- Moving source: audible spatial change without zippering.
- Rotating speaker: front-facing paths become stronger than rear-facing paths without obvious level jumps.
- High motion: lively tail without seasick direct sound.
- Percussive input: early reflections are clear but not ragged.

## Default Patch Behavior

On module load:

- Stereo linked source pair enabled if both inputs are patched, mono source otherwise.
- Speaker rotation: aimed toward listener.
- Room size: 8 m x 5 m.
- Source: left/front third.
- Listener: right/back third, facing source.
- Material: plaster or wood-neutral.
- Absorb: 0.35.
- Diffuse: 0.55.
- Decay: 2.5 s.
- Motion: 0.15.
- Early/Late: 0.0.
- Mix: 0.35.
- Quality: Studio.
- Air: enabled in Studio/HiFi, disabled in Eco.
- Ray overlay: off.

## Open Decisions Before Coding

- Exact final 16 HP artwork polish.
- Whether floor/ceiling materials affect only late damping in v1 or also receive explicit vertical reflection paths.
- Final naming for `Early/Late`: alternatives include `Shape`, `Field`, or `Focus`.

## V1 Non-Negotiables

- Geometry must audibly affect the early response.
- Core DSP state must not depend on UI frame rate.
- No uncontrolled feedback, NaNs, or denormals.
- Movement must be smoothed or crossfaded.
- The room canvas must be functional, not decorative.
- Studio mode must be the default and must be CPU-reasonable.
