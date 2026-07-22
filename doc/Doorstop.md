# Leviathan Doorstop

## Nonlinear Physical-Modeling Percussion Module

### Codex Implementation Specification

**Status:** Approved for implementation
**Target:** VCV Rack 2.x / Leviathan plugin suite
**Working model slug:** `Doorstop`
**Proposed width:** 3 HP
**Primary implementation language:** C++
**DSP type:** Monophonic nonlinear physical modeling
**Visual type:** Procedural NanoVG animation with optional cross-panel rendering

---

# 1. Product Summary

Doorstop is a tiny impulse-excited physical-modeling voice that simulates striking a flexible metal spring doorstop sideways.

The module has no conventional sound-design controls. Its behavior is determined entirely by:

* When the object is struck.
* How hard it is struck.
* Its current physical state when another strike occurs.

A strike causes the virtual spring to bend, oscillate from side to side, radiate a metallic resonant sound, and gradually settle back to rest.

The visual spring and generated audio must derive from the same underlying simulation state. The animation is not an unrelated envelope visualization.

The intended experience is immediate:

> Patch a trigger, patch audio, and make the tiny metal object go *boioioioioing*.

---

# 2. Design Goals

## 2.1 Primary goals

The module must:

1. Produce a recognizable spring-doorstop sound without using samples.
2. Respond naturally to strike velocity.
3. Continue from its current physical state when retriggered.
4. Produce both visible and audible nonlinear motion.
5. Remain useful as a rhythmic and experimental synthesis voice.
6. Fit comfortably into a 3 HP panel.
7. visually exceed the panel boundaries during sufficiently energetic strikes.
8. Consume very little DSP CPU.
9. Have no mandatory user-facing knob controls.
10. feel like a physical object rather than an oscillator with an envelope.

## 2.2 Character goals

The sound should include:

* A sharp mechanical strike.
* A low flexing or body component.
* Several inharmonic metallic resonances.
* Pitch and spectral movement during large oscillations.
* A decay that emerges from energy loss rather than a conventional ADSR.
* Stronger brightness and longer audible settling after harder strikes.
* Complex behavior when repeatedly struck.

## 2.3 Non-goals

The MVP must not include:

* Sample loading.
* Polyphonic audio output.
* Exposed spring tuning controls.
* Exposed decay controls.
* Stereo output.
* Multiple selectable doorstop models.
* A conventional pitch input.
* An internal trigger sequencer.
* A large animated editor.
* A general-purpose modal synthesis interface.

The model should be internally parameterized so additional doorstop materials or models can be added later.

---

# 3. Module Interface

## 3.1 Panel width

The module shall be **3 HP** wide.

Use:

```cpp
box.size = Vec(3 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
```

A 2 HP implementation is not required. Three HP provides enough room for:

* A legible animated spring.
* Three vertically arranged ports.
* A clickable manual-strike region.
* Visible horizontal spring displacement.

## 3.2 Inputs

### TRIG

**Type:** Trigger/gate input
**Expected range:** Standard Rack trigger and gate voltages
**Behavior:** A rising edge applies an impulse to the simulated spring.

Requirements:

* A held-high gate causes only one strike.
* A new strike requires the signal to return below the low threshold.
* Retriggering must not reset the simulation.
* Trigger processing should use hysteresis, preferably `dsp::SchmittTrigger`.

Suggested thresholds:

* Low/reset threshold: approximately `0.1 V`
* Rising threshold: approximately `1.0 V`

Only input channel 0 is required for the MVP.

### VELOCITY

**Type:** Continuous CV input sampled at the instant of a trigger
**Expected range:** `0 V` to `10 V`

Mapping:

```cpp
float velocityNorm = clamp(voltage / 10.f, 0.f, 1.f);
```

Requirements:

* Negative voltages behave as `0 V`.
* Voltages above `10 V` behave as `10 V`.
* `0 V` produces no meaningful strike.
* `10 V` produces the maximum supported strike.
* The value is sampled only when a strike occurs.
* Changes in Velocity after a strike do not alter the energy already in the spring.

When the input is unpatched, use a normalled value of:

```text
5 V
```

This provides a useful medium strike from TRIG alone.

## 3.3 Output

### OUT

**Type:** Mono audio output

Requirements:

* Nominal peak range should remain around `±5 V`.
* The output must be DC-blocked.
* Maximum strikes may approach the output rails but must not produce numerical clipping.
* Use a smooth final saturator or soft limiter.
* Do not normalize every strike to the same level.
* Hard strikes must remain meaningfully louder and brighter than soft strikes.

## 3.4 Manual strike interaction

A separate visible pushbutton is not required.

Instead, the animated doorstop itself shall act as the manual strike control.

Implementation recommendation:

```cpp
enum ParamId {
    MANUAL_PARAM,
    PARAMS_LEN
};
```

Configure it as a momentary button:

```cpp
configButton(MANUAL_PARAM, "Manual strike");
```

Create a custom transparent `ParamWidget` covering the visible interior spring region.

Requirements:

* Clicking the spring produces a manual strike.
* The manual strike is equivalent to approximately `5 V` Velocity.
* The interactive region remains inside the module boundary.
* The external overflow graphic must never intercept mouse events.
* The control should remain MIDI-mappable through Rack because it is backed by a real parameter.
* The parameter widget draws no conventional button artwork.

A future enhancement may derive manual velocity from a horizontal mouse flick, but this is outside the MVP.

---

# 4. Module Identifiers

Use fixed identifier ordering once implementation begins.

```cpp
struct Doorstop : Module {
    enum ParamId {
        MANUAL_PARAM,
        PARAMS_LEN
    };

    enum InputId {
        TRIG_INPUT,
        VELOCITY_INPUT,
        INPUTS_LEN
    };

    enum OutputId {
        AUDIO_OUTPUT,
        OUTPUTS_LEN
    };

    enum LightId {
        STRIKE_LIGHT,
        LIGHTS_LEN
    };
};
```

`STRIKE_LIGHT` may drive a subtle impact glow in the mounting plate. It does not require a separate conventional LED component.

---

# 5. DSP Architecture

The sound engine shall combine four elements:

1. A nonlinear primary spring state.
2. A bank of inharmonic damped resonators.
3. A short impact transient.
4. A radiation and output stage.

The primary spring state also drives the animation.

---

# 6. Primary Spring Simulation

## 6.1 State

Maintain at least:

```cpp
float displacement = 0.f;
float springVelocity = 0.f;
float acceleration = 0.f;
```

Treat these as normalized physical quantities rather than literal SI units.

The resting state is:

```cpp
displacement = 0.f;
springVelocity = 0.f;
```

## 6.2 Nonlinear oscillator

Use a damped Duffing-style spring:

[
\ddot{x}
========

-\omega_0^2x
-\beta\omega_0^2x^3
-2\zeta\omega_0\dot{x}
]

Where:

* (x) is displacement.
* (\dot{x}) is velocity.
* (\omega_0) is the low-energy angular frequency.
* (\zeta) is damping.
* (\beta) is positive nonlinear stiffness.

Positive nonlinear stiffness causes larger oscillations to run at a different effective frequency than small oscillations. As the spring loses energy, its behavior relaxes toward the resting frequency.

Suggested starting range:

```text
Low-energy visible frequency: 14–18 Hz
Maximum large-signal frequency: approximately 22–28 Hz
```

The final values must be tuned by ear.

The lowest mode should remain slow enough to be visually readable at common display frame rates. Audible metallic content will primarily come from the modal resonators rather than relying on the fundamental spring frequency.

## 6.3 Numerical integration

Use semi-implicit Euler integration:

```cpp
springVelocity += acceleration * sampleTime;
displacement += springVelocity * sampleTime;
```

Calculate acceleration before the update:

```cpp
float x2 = displacement * displacement;
float restoring =
    baseOmegaSq * displacement
    + nonlinearAmount * baseOmegaSq * displacement * x2;

float dampingForce =
    2.f * dampingRatio * baseOmega * springVelocity;

acceleration = -restoring - dampingForce;
```

Semi-implicit integration is preferred over explicit Euler because it is better behaved for oscillator systems.

Do not allocate memory in `process()`.

## 6.4 Stability protection

The engine must protect itself from unstable state.

Required safeguards:

```cpp
displacement = clamp(displacement, -MAX_DISPLACEMENT, MAX_DISPLACEMENT);
springVelocity = clamp(springVelocity, -MAX_VELOCITY, MAX_VELOCITY);
```

Additionally:

* Check important state values with `std::isfinite()`.
* Reset the complete physical state if any state becomes non-finite.
* Prevent accumulated retriggers from creating unbounded energy.
* Apply a smooth energy-domain saturation rather than abruptly truncating ordinary strikes.
* Test all supported sample rates.

Suggested test sample rates:

```text
44.1 kHz
48 kHz
88.2 kHz
96 kHz
192 kHz
```

## 6.5 Sleeping state

When all physical and transient energy falls below a small threshold:

```cpp
displacement = 0.f;
springVelocity = 0.f;
acceleration = 0.f;
```

Enter a lightweight sleeping state.

Wake immediately when:

* TRIG receives a rising edge.
* The manual parameter receives a rising edge.

The sleep transition must not produce a discontinuity or output click.

---

# 7. Strike Processing

## 7.1 Velocity mapping

Velocity should use a curved response so soft strikes remain controllable while high voltages become dramatic.

Suggested mapping:

```cpp
float u = clamp(velocityVoltage / 10.f, 0.f, 1.f);
float shaped = 0.2f * u + 0.8f * u * u;
```

This exact curve is tunable.

The shaped strike value controls:

* Mechanical impulse.
* Modal excitation.
* Impact transient amplitude.
* Impact transient brightness.
* Visual strike flash.
* Maximum practical displacement.
* Audible settling duration.

## 7.2 Applying the impulse

Apply the strike directly to the current spring velocity:

```cpp
springVelocity += shaped * MAX_IMPULSE;
```

Do not reset:

* Displacement.
* Spring velocity.
* Modal phases.
* Modal amplitudes.
* Decay state.

The strike always pushes in the same nominal horizontal direction.

This creates phase-sensitive retriggering:

* A strike aligned with the current movement adds energy.
* A strike against the returning movement may reduce or reverse velocity.
* Rapid triggers may drive the object into sustained nonlinear motion.
* Triggers near resonant rates may reinforce repeating patterns.

This behavior is a central requirement.

## 7.3 Simultaneous trigger sources

TRIG and the manual parameter may both create strikes.

If they rise during the same audio sample:

* Process both impulses.
* Do not collapse them into a single strike.
* Apply the manual strike using the default manual strength.

## 7.4 Zero velocity

A trigger received while the patched Velocity input is at `0 V` should not produce a meaningful strike.

It may remain completely silent.

Do not substitute the normalled `5 V` value when a cable is connected.

---

# 8. Modal Resonator Bank

## 8.1 Purpose

The nonlinear spring state alone will sound too much like a low oscillator.

Add a compact bank of damped, inharmonic resonators representing:

* The metal coil.
* The weighted rubber tip.
* The mounting plate.
* Higher bending and torsional modes.

## 8.2 Mode count

Use four resonators for the MVP.

Each resonator maintains:

```cpp
struct ModeState {
    float position = 0.f;
    float velocity = 0.f;
};
```

Each mode can use a damped second-order state equation:

[
\ddot{q_i}
==========

-\omega_i^2q_i
-2\gamma_i\dot{q_i}
+F_i
]

Suggested starting frequencies:

```text
Mode 1: 170–220 Hz
Mode 2: 430–560 Hz
Mode 3: 900–1250 Hz
Mode 4: 1800–2800 Hz
```

Suggested approximate starting ratios from Mode 1:

```text
1.00
2.65
5.75
12.4
```

These are tuning seeds, not mandatory final constants.

The ratios must remain noticeably inharmonic.

## 8.3 Mode decay

Higher modes should decay faster.

Suggested decay ordering:

```text
Mode 1: longest
Mode 2: medium-long
Mode 3: short
Mode 4: very short
```

Approximate audible decay targets after a medium strike:

```text
Mode 1: 0.8–1.4 seconds
Mode 2: 0.4–0.9 seconds
Mode 3: 0.2–0.5 seconds
Mode 4: 0.08–0.25 seconds
```

The primary spring motion may remain audible after the brightest modes disappear.

## 8.4 Modal excitation

On every strike:

```cpp
mode[i].velocity += shapedStrike * modeGain[i];
```

Hard strikes must excite high modes disproportionately.

For example:

```cpp
float brightness = shaped * shaped;

mode[0].velocity += shaped * gain0;
mode[1].velocity += shaped * gain1;
mode[2].velocity += lerp(shaped, brightness, 0.6f) * gain2;
mode[3].velocity += brightness * gain3;
```

## 8.5 Dynamic modal frequency

The current large-scale spring displacement should slightly perturb modal frequencies.

Suggested formulation:

```cpp
float stiffnessWarp = 1.f + warpAmount[i] * displacement * displacement;
float asymmetryWarp = 1.f + asymmetry[i] * displacement;
float effectiveFrequency = baseFrequency[i] * stiffnessWarp * asymmetryWarp;
```

Requirements:

* Frequency movement must be subtle at low velocity.
* Maximum strikes should produce an obvious animated metallic pitch contour.
* Effective frequencies must be clamped to safe limits.
* The result must not sound like conventional wide vibrato.
* Modulation should diminish naturally as displacement decays.

## 8.6 Continuous coupling

A small amount of the primary spring acceleration may be fed into the lower modal modes:

```cpp
modeForce[i] += normalizedAcceleration * coupling[i];
```

This coupling must be subtle.

It should help the metallic body follow the visible flexing motion without continually replenishing enough energy to prevent decay.

---

# 9. Impact Transient

## 9.1 Components

Generate a short procedural impact containing:

* A filtered noise click.
* A low mounting thump.
* Optional rubber-tip coloration.

No samples shall be used.

## 9.2 Noise transient

On strike:

```cpp
strikeEnvelope += shapedStrike;
```

Use a fast exponential decay.

Suggested duration:

```text
Soft strike: 2–5 ms
Hard strike: 5–15 ms
```

Generate noise using a lightweight local PRNG.

Filter the noise so velocity affects brightness:

```text
Soft strike low-pass: approximately 2–4 kHz
Hard strike low-pass: approximately 8–14 kHz
```

Remove excessive low-frequency noise.

## 9.3 Mounting thump

Include a subtle low-frequency transient representing energy entering the wall or base plate.

Possible implementation:

* A short damped oscillator around `70–130 Hz`.
* Or a filtered impulse derived from primary spring acceleration.

The thump should be felt more than heard and must not dominate the metallic character.

---

# 10. Radiation and Output Model

## 10.1 Primary motion contribution

Do not simply output raw displacement.

Construct the body component from a normalized combination of:

* Velocity.
* Acceleration.
* Nonlinear acceleration harmonics.

Example:

```cpp
float normalizedVelocity = springVelocity / velocityScale;
float normalizedAcceleration = acceleration / accelerationScale;

float body =
    velocityGain * normalizedVelocity
    + accelerationGain * normalizedAcceleration;
```

A mild nonlinear function may be applied:

```cpp
body = std::tanh(body * bodyDrive);
```

This helps convert low flexing motion into audible repeated mechanical radiation.

## 10.2 Final mix

Suggested structure:

```cpp
float signal = 0.f;

signal += body * BODY_GAIN;

for (int i = 0; i < MODE_COUNT; ++i) {
    signal += modes[i].position * modeOutputGain[i];
}

signal += strikeNoise;
signal += mountingThump;
```

## 10.3 Output conditioning

The final stage must include:

1. A DC blocker.
2. Optional gentle high-frequency limiting.
3. Smooth saturation.
4. Scaling to Rack audio voltage.

Suggested output:

```cpp
float volts = softClip(filteredSignal * outputGain) * 5.f;
outputs[AUDIO_OUTPUT].setVoltage(volts);
```

Use a smooth soft-clipping function such as:

```cpp
float softClip(float x) {
    return std::tanh(x);
}
```

The saturator should activate mainly during extreme or repeatedly accumulated strikes.

## 10.4 Level targets

After tuning:

* A normalled `5 V` strike should peak around `3–4 V`.
* A `10 V` strike should approach but generally remain inside `±5 V`.
* Repeated resonant strikes may engage the soft limiter.
* Quiet strikes should remain clearly quieter rather than being auto-amplified.

---

# 11. Visual Design

## 11.1 General layout

Suggested vertical layout:

```text
Top:
    Small Leviathan title treatment

Upper and middle region:
    Animated vertical spring doorstop

Lower region:
    TRIG input
    VELOCITY input
    AUDIO output
```

Suggested physical arrangement:

* The spring mounting plate sits around the lower-middle portion of the panel.
* The spring rises vertically.
* The spring cap bends horizontally to the left and right.
* The ports are vertically stacked below the mounting plate.
* The output should be visually distinguished from the two inputs.

## 11.2 Animated geometry

The spring must bend as a flexible object.

Do not rotate a rigid sprite around its base.

Represent the spring using a curved centerline:

[
C(t), \quad 0 \le t \le 1
]

Where:

* `t = 0` is the fixed mounting point.
* `t = 1` is the rubber tip.
* Horizontal displacement increases toward the free end.

A suitable horizontal bend profile is:

```cpp
float bendProfile(float t) {
    return t * t * (3.f - 2.f * t);
}
```

Then:

```cpp
centerX = baseX + visualDisplacement * bendProfile(t);
centerY = baseY - springLength * t;
```

Wrap a procedural coil around the centerline:

```cpp
float phase = TWO_PI * coilTurns * t;
point = centerline + normal * coilRadius * std::sin(phase);
```

Suggested spring characteristics:

```text
Coil count: 14–22 visible turns
Coil radius: taper slightly near base and cap
Spring length: most of the upper panel
```

## 11.3 Rendering treatment

Draw the spring with multiple strokes:

1. Dark shadow or outline.
2. Main metallic body.
3. Narrow highlight.
4. Optional cyan/purple reflected-energy accent.

The cap should:

* Appear rubberized.
* Rotate according to the centerline tangent.
* Have a subtle highlight.
* Carry the strongest motion blur at extreme velocity.

The mounting plate should remain static and may be part of the panel asset or a cached visual widget.

## 11.4 Visual state transfer

The audio thread shall expose lightweight visual telemetry using atomics:

```cpp
std::atomic<float> visualDisplacement;
std::atomic<float> visualVelocity;
std::atomic<float> visualEnergy;
std::atomic<float> visualStrike;
```

Requirements:

* The UI thread only reads these values.
* The DSP thread never accesses NanoVG or widget state.
* The UI must not lock the audio thread.
* Values should be finite and clamped before publication.
* `visualStrike` may decay in the DSP or UI domain.

## 11.5 Motion trails

At high energy, render one or more faint historical spring-tip positions.

Requirements:

* Trails appear only during energetic movement.
* Trails must remain subtle.
* Trail opacity scales with velocity or energy.
* Trails help preserve the perception of fast motion near the display frame-rate limit.
* The effect must not make the spring look permanently blurred.

Maintain a small UI-side history buffer, for example:

```cpp
std::array<float, 3> displacementHistory;
```

---

# 12. Rendering Outside Module Bounds

## 12.1 Requirement

At ordinary strike levels, the complete doorstop should remain inside its 3 HP panel.

At high strike energies, the upper spring and rubber cap may extend beyond the panel’s left or right edge.

Suggested maximum overflow:

```text
Approximately 20–35 screen pixels beyond either side at default zoom
```

The extension should become noticeable only near the top portion of the velocity range.

Use a nonlinear screen mapping:

```cpp
float visualX =
    maxVisualTravel * std::tanh(simulationDisplacement * visualScale);
```

## 12.2 Rendering architecture

A normal widget draw receives a visible clip region, and Rack widgets provide coordinate conversion functions and layered drawing methods. Rack’s `RackWidget` exposes separate module, plug, and cable containers, which makes a dedicated rack-level visual widget a viable architecture.

Implement two coordinated visual widgets:

### `DoorstopSpringWidget`

Owned by `DoorstopWidget`.

Responsibilities:

* Draw all spring content that lies inside the panel.
* Handle the interior clickable region.
* Read visual atomics from the module.
* Remain functional when overflow rendering is unavailable.

### `DoorstopOverflowWidget`

Attached to an appropriate rack-level container rather than as a clipped child of the module panel.

Responsibilities:

* Draw only spring portions that extend outside the module bounds.
* Convert the doorstop anchor from module-local coordinates to rack coordinates.
* Follow rack scrolling and zooming.
* Follow module repositioning.
* Draw transparently.
* Ignore all mouse and hover interaction.
* Remove itself safely when the owning module widget is destroyed.

Preferred attachment target:

```cpp
APP->scene->rack->getModuleContainer()
```

The exact stacking location must be verified against:

* Adjacent modules.
* Cables.
* Plugs.
* Selection overlays.
* Module dragging.
* Browser previews.
* Rack zoom.

The overflow should ideally render above module panels but should not obscure cables and plugs more than necessary.

## 12.3 Shared overlay utility

If the expanded Wyrm editor introduces a reusable rack-space overlay system, Doorstop should reuse that infrastructure rather than creating a second incompatible overlay lifecycle.

The shared abstraction could provide:

* Owner registration.
* Rack-coordinate transformation.
* Safe destruction.
* Zoom tracking.
* Scroll tracking.
* Module drag tracking.
* Visibility rules.

Doorstop only needs a passive, noninteractive subset of that system.

## 12.4 Safety fallback

The internal spring animation is mandatory.

If rack-level rendering is temporarily unavailable, unsafe, or unsupported in a particular preview context:

* Render the spring clipped inside the panel.
* Never crash.
* Never leave an orphaned overlay.
* Never dereference a deleted module or module widget.

The released implementation must still attempt cross-panel rendering during normal rack operation.

## 12.5 Optional context-menu control

Add:

```text
Allow spring to extend over adjacent modules
```

Default:

```text
Enabled
```

When disabled:

* Clamp visual displacement to the panel boundary.
* Remove or hide the overflow widget.
* Preserve the same audio behavior.

Persist this option in module JSON.

Rack calls custom widget drawing every frame, while `FramebufferWidget` is intended for graphics that do not require continuous redraw. Therefore, cache static mounting and panel artwork where appropriate, but render the moving spring directly each frame.

---

# 13. Panel and Asset Strategy

Use the existing Leviathan visual conventions.

Suggested assets:

```text
res/Doorstop.svg
res/components/doorstop-base.svg
res/components/doorstop-cap.svg
```

However, the spring itself should be procedural.

Static panel content may include:

* Background.
* Leviathan branding.
* Port labels.
* Mounting screws.
* Mounting plate shadow.
* Subtle conduit elements.

Dynamic content should include:

* Spring coil.
* Rubber cap.
* Motion trails.
* Strike flash.
* Any over-panel rendering.

The panel must remain visually legible when the spring is at rest.

---

# 14. Threading Requirements

## 14.1 Audio thread

The audio thread owns:

* Trigger detectors.
* Manual trigger detector.
* Physical simulation.
* Modal states.
* Impact generators.
* Filters.
* Output voltage.
* Atomic visual telemetry writes.

The audio thread must not:

* Allocate memory.
* Read widget geometry.
* Call NanoVG.
* Modify Rack widget ownership.
* Use mutexes.
* Construct strings.
* Perform file I/O.

## 14.2 UI thread

The UI thread owns:

* Spring rendering.
* Overflow-widget lifecycle.
* Coordinate conversion.
* Motion history.
* Context-menu operations.
* Static asset loading.

The UI thread reads but does not modify DSP state.

---

# 15. Serialization

Do not serialize the active spring motion.

When a patch loads, the object begins at rest.

Persist only user-facing configuration such as:

```json
{
  "allowVisualOverflow": true
}
```

The exact JSON key should remain stable after release.

No random generator state needs to be serialized.

---

# 16. Sample-Rate Handling

Implement:

```cpp
void onSampleRateChange(const SampleRateChangeEvent& e) override;
```

Recalculate:

* Modal angular frequencies.
* Decay coefficients.
* Strike-envelope coefficients.
* Noise-filter coefficients.
* DC-blocker coefficients.
* Any oversampling or smoothing constants.

Changing sample rate must not create unstable values.

It is acceptable to preserve current displacement and velocity across a sample-rate change.

---

# 17. Suggested Class Structure

```cpp
struct DoorstopMode {
    float position = 0.f;
    float velocity = 0.f;

    void reset();
    float process(
        float sampleTime,
        float frequency,
        float damping,
        float force
    );
};

struct DoorstopImpact {
    float noiseEnvelope = 0.f;
    float thumpPosition = 0.f;
    float thumpVelocity = 0.f;
    uint32_t rngState = 0x12345678u;

    void reset();
    void strike(float strength);
    float process(float sampleTime, float strength);
};

struct Doorstop : Module {
    static constexpr int MODE_COUNT = 4;

    dsp::SchmittTrigger trigTrigger;
    dsp::SchmittTrigger manualTrigger;

    float displacement = 0.f;
    float springVelocity = 0.f;
    float acceleration = 0.f;

    std::array<DoorstopMode, MODE_COUNT> modes;
    DoorstopImpact impact;

    dsp::RCFilter dcBlocker;

    bool sleeping = true;
    bool allowVisualOverflow = true;

    std::atomic<float> visualDisplacement {0.f};
    std::atomic<float> visualVelocity {0.f};
    std::atomic<float> visualEnergy {0.f};
    std::atomic<float> visualStrike {0.f};

    Doorstop();

    void process(const ProcessArgs& args) override;
    void onSampleRateChange(
        const SampleRateChangeEvent& e
    ) override;

    void resetSimulation();
    void applyStrike(float normalizedVelocity);
    float processSpring(float sampleTime);
    float processModes(float sampleTime);
    float processOutput(float sampleTime);

    json_t* dataToJson() override;
    void dataFromJson(json_t* rootJ) override;
};

struct DoorstopSpringWidget : TransparentWidget {
    Doorstop* module = nullptr;

    void draw(const DrawArgs& args) override;
};

struct DoorstopHitWidget : ParamWidget {
    void draw(const DrawArgs& args) override;
};

struct DoorstopOverflowWidget : TransparentWidget {
    DoorstopWidget* owner = nullptr;

    void step() override;
    void draw(const DrawArgs& args) override;
};

struct DoorstopWidget : ModuleWidget {
    DoorstopSpringWidget* springWidget = nullptr;
    DoorstopOverflowWidget* overflowWidget = nullptr;

    DoorstopWidget(Doorstop* module);
    ~DoorstopWidget() override;

    void appendContextMenu(Menu* menu) override;
    void createOverflowWidget();
    void destroyOverflowWidget();
};
```

Exact class separation may be adjusted to match the existing Leviathan codebase.

---

# 18. Processing Pseudocode

```cpp
void Doorstop::process(const ProcessArgs& args) {
    float trigVoltage = inputs[TRIG_INPUT].getVoltage();

    bool externalStrike = trigTrigger.process(trigVoltage);
    bool manualStrike = manualTrigger.process(
        params[MANUAL_PARAM].getValue()
    );

    if (externalStrike) {
        float velocityVoltage =
            inputs[VELOCITY_INPUT].isConnected()
                ? inputs[VELOCITY_INPUT].getVoltage()
                : 5.f;

        float u = clamp(velocityVoltage / 10.f, 0.f, 1.f);
        applyStrike(u);
    }

    if (manualStrike) {
        applyStrike(0.5f);
    }

    if (sleeping) {
        outputs[AUDIO_OUTPUT].setVoltage(0.f);
        return;
    }

    float body = processSpring(args.sampleTime);
    float modal = processModes(args.sampleTime);
    float transient = impact.process(args.sampleTime);

    float signal =
        body * BODY_GAIN
        + modal * MODAL_GAIN
        + transient * IMPACT_GAIN;

    signal = processDcBlock(signal);
    signal = std::tanh(signal * OUTPUT_DRIVE);

    outputs[AUDIO_OUTPUT].setVoltage(signal * 5.f);

    publishVisualState();
    updateSleepState();
}
```

---

# 19. Tuning Constants

Collect audible tuning values into one clearly labeled structure or namespace.

Example:

```cpp
struct DoorstopTuning {
    float baseFrequency;
    float dampingRatio;
    float nonlinearStiffness;
    float maxImpulse;

    std::array<float, 4> modeFrequencies;
    std::array<float, 4> modeDamping;
    std::array<float, 4> modeExcitation;
    std::array<float, 4> modeOutputGain;
    std::array<float, 4> modeWarp;

    float bodyGain;
    float impactGain;
    float modalGain;
    float outputDrive;
};
```

Do not scatter unexplained magic numbers throughout `process()`.

Initial tuning should be considered provisional until evaluated through audio tests.

---

# 20. Context Menu

The MVP context menu should include only:

```text
Allow spring to extend over adjacent modules
```

Optional debugging entries may exist in development builds but should not appear in release builds.

Do not expose physical model tuning through the normal context menu in the MVP.

---

# 21. Plugin Registration

Add the model declaration:

```cpp
extern Model* modelDoorstop;
```

Register the model:

```cpp
Model* modelDoorstop =
    createModel<Doorstop, DoorstopWidget>("Doorstop");
```

Add it to plugin initialization:

```cpp
p->addModel(modelDoorstop);
```

Update `plugin.json` with:

* Module slug.
* Display name.
* Description.
* Tags.

Suggested description:

```text
Impulse-excited physical model of a vibrating spring doorstop.
```

Suggested tags:

```json
[
  "Physical modeling",
  "Percussion",
  "Sound effect"
]
```

---

# 22. Testing Requirements

## 22.1 DSP unit tests

Create tests for:

### Velocity mapping

* Negative voltage maps to zero.
* `0 V` maps to zero.
* `5 V` maps to a medium strike.
* `10 V` maps to maximum.
* Voltages above `10 V` clamp safely.

### Trigger behavior

* One rising edge produces one strike.
* A held gate does not repeatedly trigger.
* Returning low re-arms the trigger.
* Manual and external strikes both work.
* Simultaneous manual and external triggers accumulate.

### Retrigger behavior

* A retrigger does not reset displacement.
* A retrigger does not reset mode state.
* A retrigger can increase or oppose current spring velocity.
* Rapid retriggering remains finite.

### Decay behavior

* A single strike eventually reaches the sleep threshold.
* A hard strike remains active longer than a soft strike.
* Output approaches zero without a discontinuity.
* The sleeping state outputs exactly zero.

### Numerical stability

Run sustained randomized triggers at:

```text
44.1 kHz
48 kHz
96 kHz
192 kHz
```

Verify:

* No NaN.
* No infinity.
* No denormal-related runaway.
* Output remains bounded.
* Simulation eventually recovers after maximum accumulated energy.

### Output

* Output contains no significant DC after settling.
* Maximum output remains within the intended voltage range.
* Zero-velocity triggers remain silent or effectively silent.

## 22.2 Manual audio tests

Evaluate:

1. Single `1 V` Velocity strike.
2. Single `5 V` strike.
3. Single `10 V` strike.
4. Repeated 8th-note triggers.
5. Triggering near the spring’s visible resonance.
6. Rapid random triggers.
7. Triggering while the spring is returning.
8. Velocity sweeps from `0 V` to `10 V`.
9. Direct monitoring and monitoring through reverb.
10. Extreme accumulation followed by natural decay.

The sound should remain identifiable as a struck metal spring doorstop throughout this range.

## 22.3 Visual tests

Verify at multiple Rack zoom levels:

* Spring follows the DSP.
* Motion settles exactly to center.
* Cap orientation follows spring tangent.
* Trails appear only at high motion.
* External overflow aligns with the internal spring.
* Overflow follows scroll and zoom.
* Overflow follows module dragging.
* Overflow disappears when the module is deleted.
* Overflow does not intercept mouse input.
* Adjacent modules remain usable.
* Context-menu disable immediately removes overflow.
* Module browser preview does not create an orphan overlay.

---

# 23. Acceptance Criteria

The module is complete when all of the following are true:

1. The module occupies 3 HP.
2. It has TRIG and VELOCITY inputs.
3. It has one mono audio output.
4. An unpatched Velocity input behaves as `5 V`.
5. Clicking the visible doorstop manually strikes it.
6. Triggering applies energy rather than restarting an envelope.
7. Repeated strikes interact with existing motion.
8. Harder strikes create greater displacement.
9. Harder strikes create brighter audio.
10. Harder strikes remain audible for longer.
11. The audio contains a mechanical impact and inharmonic metallic tail.
12. The visible spring and audio derive from the same simulation.
13. High-energy motion extends outside the panel during normal rack operation.
14. External rendering does not interfere with adjacent-module interaction.
15. The module returns to a stable, silent resting state.
16. Output remains finite and bounded under abusive retriggering.
17. No audio-thread allocation or UI locking occurs.
18. The module compiles on all platforms supported by the Leviathan project.
19. The module has a valid manifest entry and is registered with the plugin.
20. DSP and lifecycle tests pass.

---

# 24. Implementation Order

## Phase 1 — Module skeleton

* Register the model.
* Create the 3 HP panel.
* Add the three ports.
* Add the invisible manual strike parameter.
* Produce temporary silence at the output.

## Phase 2 — Primary physical state

* Implement trigger handling.
* Implement velocity mapping.
* Implement nonlinear spring integration.
* Implement sleeping and stability protection.
* Publish visual displacement.

## Phase 3 — Audible model

* Implement the modal resonator bank.
* Implement modal frequency warping.
* Implement impact noise.
* Implement mounting thump.
* Implement DC blocking and output saturation.
* Tune medium and maximum strikes.

## Phase 4 — Interior animation

* Implement the curved spring centerline.
* Implement procedural coil rendering.
* Add rubber cap and mounting plate.
* Add strike glow and motion trails.
* Add manual click interaction.

## Phase 5 — Cross-panel animation

* Implement the passive overflow widget.
* Track rack coordinates, zoom, and scrolling.
* Confirm drawing order.
* Add safe lifecycle management.
* Add the overflow context-menu setting.

## Phase 6 — Validation and polish

* Add tests.
* Tune modal ratios and decay.
* Verify performance.
* Verify patch serialization.
* Test browser previews and module deletion.
* Remove development controls and diagnostic overlays.

---

# 25. Future Expansion Points

Do not implement these during the MVP, but avoid architectures that make them impossible:

* Multiple material models.
* Wall-mounted versus floor-mounted doorstops.
* Adjustable spring length.
* Rubber-tip hardness.
* Accent output generated at each direction reversal.
* Separate impact and resonant outputs.
* Stereo wall resonance.
* Audio input capable of physically exciting the spring.
* Mouse flick strength.
* Different visual doorstop designs.
* A hidden “unreasonably powerful strike” mode.
* Chaotic spring collisions with panel boundaries.
* Coupling between adjacent Doorstop modules.

---

# 26. Final Product Principle

Doorstop must not feel like a themed oscillator.

It is one persistent virtual object.

It receives force, carries momentum, exchanges energy among nonlinear modes, radiates sound, moves visibly, and eventually returns to silence.

Every implementation decision should preserve that illusion.
