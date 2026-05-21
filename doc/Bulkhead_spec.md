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
- One mono source or linked stereo source pair.
- Rectangular top-down room with four movable walls.
- Direct path and first-order image-source reflections.
- Optional second-order reflections in higher quality modes.
- Per-wall material presets and global material macro control.
- FDN late field with Eco, Studio, and HiFi quality modes.
- Early/late balance, decay, diffusion, motion, air/brightness, and mix controls.
- Cached custom room canvas with draggable source, listener, and wall handles.
- Parameter smoothing and state crossfades to avoid zipper noise.
- Focused tests for geometry timing, decay stability, and interpolation safety.

### Deferred

- Full convolution or imported impulse responses.
- Arbitrary polygon rooms.
- True 3D room editing on the main panel.
- Independent per-polyphonic-channel room simulation.
- Binaural/HRTF rendering.
- User-editable octave-band material curves.
- Full ambisonic or surround output.

## Signal Flow

```text
Input L/R
  |
  +--> Source model / mono-stereo placement
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
| `IN L` | Audio input | Left/mono input. If `IN R` is unpatched, duplicate to right source path. |
| `IN R` | Audio input | Optional right input. Enables linked stereo source pair. |
| `OUT L` | Audio output | Stereo left output. |
| `OUT R` | Audio output | Stereo right output. |

### Primary Controls

| Control | Range | CV | Description |
| --- | --- | --- | --- |
| `Size` | 0..1 | `SIZE CV` | Uniformly scales the editable room around its center. Maintains wall proportions unless walls are manually edited. |
| `Decay` | 0.1..30 s | `DECAY CV` | Target late-field RT60. Material absorption can shorten effective decay. |
| `Material` | stepped or continuous | `MAT CV` | Selects/morphs global material family. Per-wall overrides are available from the canvas/context menu. |
| `Absorb` | 0..1 | `ABSORB CV` | Scales wall absorption and late damping. Low is reflective, high is dry/damped. |
| `Diffuse` | 0..1 | `DIFF CV` | Controls early-to-late scattering and FDN density character. Low is specular/roomy, high is smooth/dense. |
| `Motion` | 0..1 | `MOTION CV` | Adds stable late-tail motion via feedback-matrix modulation and very slow delay modulation. |
| `Early/Late` | -1..+1 | `E/L CV` | Crossfades emphasis between geometric early field and diffuse late tail. |
| `Mix` | 0..1 | `MIX CV` | Dry/wet mix. |

### Geometry CV

| Port | Target | Range Mapping |
| --- | --- | --- |
| `SRC X` | Source x position | -5V..+5V maps to room left..right. |
| `SRC Y` | Source y position | -5V..+5V maps to room bottom..top. |
| `LST X` | Listener x position | -5V..+5V maps to room left..right. |
| `LST Y` | Listener y position | -5V..+5V maps to room bottom..top. |
| `YAW CV` | Listener yaw | -5V..+5V maps to -180..+180 degrees. |
| `WALL CV` | Assigned wall/size macro | Default maps to uniform room size; context menu can assign to left/right/top/bottom. |

V1 should avoid exposing eight wall CV ports unless panel space supports it cleanly. A single assignable `WALL CV` is enough for the first implementation and keeps the faceplate focused.

### Buttons / Modes

| Control | Behavior |
| --- | --- |
| `Quality` | Eco / Studio / HiFi. Context-menu item is acceptable for v1 if panel space is tight. |
| `Air` | Enables distance-dependent high-frequency loss. Off in Eco by default. |
| `Rays` | Toggles visual reflection rays. Display-only. |
| `Freeze` | Optional v1. Freezes late FDN input and feedback state while leaving dry/direct path live. If omitted, leave space in architecture. |

## Room Canvas

The room canvas is the primary interaction surface.

### Visual Elements

- Rectangular room boundary with four wall handles.
- Source node: one speaker for mono, linked L/R speaker pair for stereo input.
- Listener node: head/ear icon with yaw indicator.
- Optional first-order ray overlay.
- Material coloring or texture along walls.
- Active CV target highlight when a geometry CV is patched.
- Subtle late-field glow or density haze, driven from smoothed parameters, not audio-thread analysis.

### Interactions

- Drag source to change source position.
- Drag listener to change listener position.
- Drag listener yaw handle to rotate listening direction.
- Drag wall handles to resize room asymmetrically.
- Shift-drag or context menu for precise values if needed.
- Double-click source/listener/wall to reset that target.
- Right-click wall to assign material override.
- Right-click canvas to show quality, overlay, air, and advanced options.

### Rendering Policy

Use a custom Rack widget inside a cached framebuffer. Mark the framebuffer dirty only when room geometry, material, display size, or overlay state changes. Do not do expensive text layout or path recomputation every frame. Keep audio-thread data transfer to simple atomics or UI snapshots.

## Coordinate Model

Use normalized UI coordinates internally, then map to physical meters for DSP.

```text
room.left   <= source.x/listener.x <= room.right
room.bottom <= source.y/listener.y <= room.top
```

Recommended physical range:

- Minimum room dimension: 1.0 m.
- Maximum room dimension: 30.0 m.
- Default room: 8.0 m wide, 5.0 m deep.
- Listener default: center-right, facing source.
- Source default: center-left.
- Speed of sound: 343 m/s at default temperature.

For safety, keep source and listener at least 0.1 m away from walls after smoothing/clamping.

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
| Source/listener position | 10..20 ms |
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
3. Convert horizontal angle to equal-power stereo pan.
4. Optionally reduce high-frequency content for paths behind the listener using a subtle rear damping factor.

This is not HRTF. It should be a stable stereo room cue that reads in Rack patches.

## State Serialization

Persist:

- Geometry: room bounds, source position(s), listener position, listener yaw.
- Macro params: size, decay, material, absorb, diffuse, motion, early/late, mix, air enabled, quality mode.
- Per-wall material overrides.
- UI options: ray overlay, linked stereo source mode.
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
};

struct BulkheadPose {
    BulkheadVec2 position;
    float yawRadians = 0.f;
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

## Implementation Phases

### Phase 1: Geometry And Early Reflection Core

- Create geometry utilities for room bounds, source/listener pose, and first-order image sources.
- Implement direct path and first-order reflection delay computation.
- Implement dynamic delay line with 4-point interpolation.
- Implement simple material gain/low-pass stage.
- Produce stereo wet output without FDN.
- Add tests for expected delay samples from known geometry.

Exit criteria:

- Impulse produces direct path plus four first-order reflections at expected sample positions.
- Moving source/listener changes delay smoothly without clicks under smoothed modulation.
- Mono input produces stable stereo output.

### Phase 2: Late FDN Core

- Implement 8-channel FDN first.
- Add delay-length scaling from room dimensions.
- Add decay-derived feedback gains and damping filters.
- Add stereo output decorrelation.
- Add Eco/Studio/HiFi topology selection after 8-channel core is stable.

Exit criteria:

- FDN decays monotonically within expected tolerance for static impulse tests.
- Maximum decay remains finite and denormal-safe.
- Quality modes produce increasing density without changing basic wet level radically.

### Phase 3: Material And Air Model

- Add material presets.
- Add global Material and Absorb controls.
- Add per-wall material overrides.
- Add optional air absorption.

Exit criteria:

- Soft materials visibly and audibly reduce reflection energy and brightness.
- Air absorption has distance-dependent effect and can be bypassed.
- Material changes do not click.

### Phase 4: Rack Module And Panel

- Add module IDs, params, ports, lights if needed.
- Create initial `res/bulkhead.svg` panel.
- Implement room canvas widget with draggable nodes/walls.
- Add context menu for quality, air, ray overlay, linked stereo source, and material overrides.
- Persist state with schema version.

Exit criteria:

- Room canvas controls actual DSP state.
- CV and UI edits stay synchronized.
- Framebuffer caching avoids excessive UI redraw cost.

### Phase 5: Polish And Validation

- Tune defaults and gain staging.
- Add second-order reflections for Studio/HiFi.
- Add stable motion modulation.
- Add final tests and CPU profiling at 48/96/192 kHz.

Exit criteria:

- No runaway feedback at max decay/motion.
- No obvious zipper noise under normal CV movement.
- CPU is acceptable in Studio mode at 48 kHz.

## Testing Plan

### Unit Tests

- Geometry reflection positions for each wall.
- Direct-path delay for known source/listener distances.
- First-order reflection delay for known room geometry.
- CV mapping clamps source/listener inside room.
- Material reflectance maps absorption to gain correctly.
- FDN feedback gain decreases as decay decreases.
- Serialization round-trip preserves room state.

### DSP Regression Tests

- Static impulse response has finite output and expected early peaks.
- Moving source with smoothed CV produces finite output and no extreme sample jumps.
- Max decay with silence input decays or remains bounded.
- Material change crossfade avoids single-sample spikes.
- Quality mode switch does not produce NaN/Inf.

### Listening Checks

- Small concrete room: bright slap and short dense tail.
- Large soft room: delayed, dark reflections and controlled decay.
- Moving source: audible spatial change without zippering.
- High motion: lively tail without seasick direct sound.
- Percussive input: early reflections are clear but not ragged.

## Default Patch Behavior

On module load:

- Stereo linked source pair enabled if both inputs are patched, mono source otherwise.
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

- Exact panel width and port layout.
- Whether `Freeze` ships in v1 or waits for v1.1.
- Whether `WALL CV` is assignable only or whether individual wall CVs fit the panel.
- Whether Material is a stepped selector, continuous morph, or both.
- Whether source/listener positions also get knobs, or canvas + CV is enough.
- Final naming for `Early/Late`: alternatives include `Shape`, `Field`, or `Focus`.

## V1 Non-Negotiables

- Geometry must audibly affect the early response.
- Core DSP state must not depend on UI frame rate.
- No uncontrolled feedback, NaNs, or denormals.
- Movement must be smoothed or crossfaded.
- The room canvas must be functional, not decorative.
- Studio mode must be the default and must be CPU-reasonable.
