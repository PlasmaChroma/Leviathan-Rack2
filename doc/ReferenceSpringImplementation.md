# Doorstop Reference Spring Engine — Implementation Specification

## 1. Purpose

This specification defines a new reference-oriented physical model for Doorstop.
The engine models a slowly flexing doorstop spring whose motion articulates a
higher-frequency metallic body. It is a new engine, not a retuning of the
existing Classic, Coupled Body, Coil Contact, or Dispersive Spring models.

The existing engine remains available as a frozen legacy implementation. New
Doorstop modules use the reference engine by default. Patches created before
this engine existed continue to load the legacy engine and retain their saved
legacy sound model.

The implementation is considered successful when it produces the repeated,
decaying metallic lobes heard in the reference recordings without allowing the
16–21 Hz macroscopic flex frequency to dominate the audio output.

## 2. Scope

### 2.1 In scope

* A new `ReferenceSpringEngine`.
* A routing layer that owns the reference and legacy engines.
* A stable, serialized specimen identity.
* A nonlinear macroscopic flex oscillator.
* Hysteretic center-crossing detection.
* An eight-mode inharmonic metallic body.
* Continuous motion-dependent radiation.
* Bounded half-cycle modal excitation.
* Shared energy-dependent modal pitch descent.
* Direction-dependent radiation and excitation.
* Separate impact snap and mount thump paths.
* Existing break-in, manual strike, trigger, velocity, animation, and sleep
  behavior.
* Click-safe live engine switching.
* Schema-2 patch persistence with backward compatibility.
* A standalone render and analysis harness.
* Automated stability, behavior, and migration tests.

### 2.2 Out of scope for Reference V1

* Changes to panel controls, jacks, or module width.
* Polyphony.
* User editing of individual modal frequencies.
* Convolution or sample playback.
* Strict finite-element or coil-by-coil simulation.
* Replacing the existing visual spring renderer.
* Deleting or sonically changing a legacy model.
* Making the dispersive waveguide part of the first reference-engine
  acceptance milestone.
* Claiming numerical reference-match thresholds until the recordings,
  controlled renders, and analysis procedure are checked into a reproducible
  comparison fixture.

## 3. Product behavior

The context menu becomes:

```text
Sound engine
  Reference physical model
  Legacy models
    Probabilistic mix
    Classic modal
    Coupled body
    Coil contact
    Dispersive spring

Break-in: 37%
Lock break-in
Restore factory-fresh spring
Generate new specimen
Extend spring beyond panel
```

Rules:

1. A newly created module selects `ReferenceV1`.
2. An old patch without `engineMode` selects `Legacy` and restores its existing
   `soundModel`.
3. Selecting a legacy submenu item selects `Legacy` and that legacy model in
   one action.
4. `Restore factory-fresh spring` sets break-in to zero but preserves the
   specimen seed.
5. `Generate new specimen` creates a new seed, resets active motion, and sets
   break-in to zero. It does not change the selected engine.
6. Module reset selects `ReferenceV1`, clears active motion and break-in, and
   preserves the specimen seed. Specimen identity changes only through
   `Generate new specimen`.
7. Duplicating or saving and reloading a module preserves its specimen seed.

The reference engine is initially presented as a reference candidate during
development. It becomes the shipping default only after the acceptance tests
in this document pass.

## 4. Architecture

### 4.1 File layout

The target layout is:

```text
src/
  DoorstopEngine.hpp
  DoorstopEngine.cpp
  DoorstopEngineRouter.hpp
  DoorstopEngineRouter.cpp
  ReferenceSpringEngine.hpp
  ReferenceSpringEngine.cpp
tests/
  doorstop_engine_spec.cpp
  doorstop_reference_engine_spec.cpp
  doorstop_runtime_spec.cpp
tools/
  doorstop_reference_render.cpp
  analyze_doorstop_reference.py
```

The current `DoorstopEngine.hpp/.cpp`, its `doorstop::Engine` class, and its
`SoundModel` enum remain the legacy implementation in place. Do not rename,
extract, wrap internally, or refactor their equations, constants, RNG
sequence, strike selection, state, or output path.

`DoorstopEngineRouter` is the new public router used by `Doorstop`. It owns one
unchanged `doorstop::Engine` instance and one `ReferenceSpringEngine`. When
Legacy is selected, it forwards model selection, strikes, sample-rate changes,
condition changes, and processing directly to the existing engine methods.

The existing engine specification tests continue to instantiate and test
`doorstop::Engine` directly. Add deterministic controlled-render baselines as
an additional guard, but do not make their creation a reason to alter the
legacy class.

### 4.2 Public enums

```cpp
namespace doorstop {

enum class EngineMode : std::uint8_t {
    ReferenceV1 = 0,
    Legacy = 1,
    Count
};

} // namespace doorstop
```

Continue using the existing `SoundModel` type for legacy selection. Do not
rename or reorder its values. This preserves both the serialized numeric
mapping and the exact code path exercised by existing tests.

### 4.3 Common command and result types

```cpp
struct DoorstopFrame {
    float outputVolts = 0.f;
    float displacement = 0.f;
    float normalizedVelocity = 0.f;
    float normalizedEnergy = 0.f;
    float strikeLight = 0.f;
    float visualMaximumDisplacement = 1.f;
    bool sleeping = true;
    bool enteredSleep = false;
};

struct PersistentCondition {
    float breakIn = 0.f;
    bool breakInLocked = false;
    std::uint32_t specimenSeed = 1u;
};
```

The frame carries its visual displacement range so the module no longer needs
to inspect an engine-specific `EffectiveTuning` object when publishing
animation state.

### 4.4 Router responsibilities

`DoorstopEngineRouter` owns:

```cpp
ReferenceSpringEngine reference;
doorstop::Engine legacy;
EngineMode selectedMode;
SoundModel selectedLegacyModel;
PersistentCondition condition;
EngineTransition transition;
```

The router is responsible for:

* Sample-rate propagation.
* Engine and legacy-model selection.
* Canonical break-in and specimen state.
* Routing strikes to the selected engine.
* Click-safe engine transitions.
* Returning one common frame to the module.
* Reset, restore, and non-finite recovery semantics.

Its minimum public surface is:

```cpp
class DoorstopEngineRouter {
public:
    void setSampleRate(float sampleRate);
    void requestEngineMode(EngineMode mode);
    void setLegacySoundModel(SoundModel model);
    void setBreakIn(float amount);
    void setBreakInLocked(bool locked);
    void setSpecimenSeed(std::uint32_t seed);
    void strike(float normalizedVelocity);
    DoorstopFrame process();
    void resetMotion();
    void restoreFactoryFresh();

    EngineMode getEngineMode() const;
    SoundModel getLegacySoundModel() const;
    float getBreakIn() const;
    bool isBreakInLocked() const;
    std::uint32_t getSpecimenSeed() const;
    bool isSleeping() const;
};
```

`process()` uses the sample time derived from the most recently accepted sample
rate. It does not accept a second, potentially inconsistent time step.

The unchanged legacy engine remains responsible for its existing
model-selection RNG and all legacy DSP. A legacy selection must execute the
same `doorstop::Engine::setSoundModel()`, `strike()`, and `process()` methods
used today. The router synchronizes canonical break-in from the selected engine
after a strike; it must not apply the break-in dose twice.

Outside an active engine-transition fade, the router returns the legacy
engine's audio sample unchanged. It must not filter, saturate, normalize,
rescale, or otherwise post-process that sample. Adapting legacy telemetry into
the common visual frame is permitted and does not enter the audio computation.

For a legacy strike, the router first applies canonical condition state to the
legacy engine, calls the unchanged legacy `strike()`, then reads the resulting
break-in back. A reference strike follows the equivalent reference path. The
inactive engine receives canonical condition state when it becomes the
destination of a switch.

### 4.5 Real-time constraints

The audio thread must perform no allocation, file I/O, locking, logging, JSON
work, or container resizing. All mode arrays, transition state, delay storage,
and analysis taps are fixed-size.

Menu actions publish requests through atomics. The audio thread consumes those
requests at the beginning of `Doorstop::process()`.

## 5. Live engine switching

A mode change must not click or revive an old tail later.

When `selectedMode` changes:

1. Reset the destination engine's dynamic state.
2. Apply current sample rate, break-in, lock state, and specimen seed to it.
3. Retain the outgoing engine for a 15 ms equal-power fade.
4. Process both engines during the fade.
5. Send new strikes only to the newly selected engine.
6. During the fade, publish visual state from the newly selected engine if it
   has received a strike; otherwise publish the outgoing visual state.
7. Reset the outgoing engine when the fade completes.

The transition is bounded to two active engines for 15 ms. Switching between
legacy submenu models does not change engines and retains the current legacy
behavior: the newly selected model applies at the next strike while already
active legacy banks finish naturally.

If another engine-mode request arrives during the 15 ms transition, do not
start a third signal path or reset an engine whose gain is nonzero. If the
request names the current destination, treat it as a no-op. If it names the
outgoing engine, reverse the fade from its current gain values and make that
engine the destination. Reset only the engine whose gain reaches zero. Rapid
menu changes must remain bounded and click-safe.

## 6. Reference engine signal model

The reference signal is:

\[
y(t) =
y_{\mathrm{impact}}(t)
+ y_{\mathrm{mount}}(t)
+ y_{\mathrm{flex}}(t)
+ G(x,\dot{x},d)\sum_i g_iq_i(t)
\]

where:

* \(x\) and \(\dot{x}\) are the macroscopic flex state.
* \(q_i\) are inharmonic metallic-body modes.
* \(G\) is a direction-sensitive radiation gain.
* \(y_{\mathrm{flex}}\) is deliberately faint and high-passed.

At a qualified returning center crossing:

\[
\dot{q_i} \mathrel{+}=
C_i(d,s)\,
A_{\mathrm{cross}}\,
\left|\frac{\dot{x}}{\dot{x}_{\max}}\right|^p
E_{\mathrm{available}}
\]

The initial impact and each subsequent crossing are different events. A
crossing never replays the complete initial impact transient.

## 7. Macroscopic flex oscillator

### 7.1 State

```cpp
struct FlexState {
    float displacement = 0.f;
    float velocity = 0.f;
    float acceleration = 0.f;
};
```

Use the current nonlinear spring law as the starting point:

```cpp
restoring =
    omega0Sq * displacement
    + nonlinearStiffness * omega0Sq
        * displacement * displacement * displacement;

acceleration =
    -restoring
    - 2.f * dampingRatio * omega0 * velocity;
```

Integrate velocity before displacement, as the current engine does. Retain the
existing displacement, velocity, and energy limiting principles. The
reference implementation owns independent constants so legacy tuning remains
untouched.

### 7.2 Initial tuning region

These are starting regions for fitting, not acceptance values:

| Parameter | Initial value or range |
| --- | ---: |
| Resting flex frequency | 16 Hz |
| Worn flex frequency scale | 0.84 |
| Fresh damping ratio | 0.020 |
| Worn damping scale | 0.65 |
| Nonlinear stiffness | 1.4 |
| Maximum displacement | 2.0 internal units |

The flex oscillator drives animation, radiation timing, pitch warp, retrigger
interaction, and sleep behavior. Its direct audio path is high-passed at
120 Hz or higher and should be at least 18 dB below the modal body during an
ordinary medium strike.

## 8. Center-crossing detector

### 8.1 Required behavior

The detector must:

* Ignore the initial departure from rest.
* Produce at most one event per returning half-cycle.
* Avoid repeated events when a sample is exactly zero.
* Reject low-energy center dithering.
* Continue to behave correctly after a same- or opposite-direction retrigger.
* Report crossing direction from flex velocity.

### 8.2 State

```cpp
enum class ArmedSide : std::int8_t {
    None = 0,
    Positive = 1,
    Negative = -1
};

struct CrossingDetector {
    float previousDisplacement = 0.f;
    ArmedSide armedSide = ArmedSide::None;
    int refractorySamples = 0;
};
```

### 8.3 Algorithm

Define thresholds as fractions of current maximum displacement:

```cpp
armThreshold = 0.06f * maximumDisplacement;
minimumCrossingVelocity = 0.025f; // normalized
crossingRefractorySeconds = 0.004f;
```

Per sample:

1. Decrement `refractorySamples` if nonzero.
2. If unarmed and `displacement >= armThreshold`, arm `Positive`.
3. If unarmed and `displacement <= -armThreshold`, arm `Negative`.
4. If armed `Positive`, fire only when `previousDisplacement > 0` and
   `displacement <= 0`.
5. If armed `Negative`, fire only when `previousDisplacement < 0` and
   `displacement >= 0`.
6. Require normalized absolute velocity to exceed
   `minimumCrossingVelocity`.
7. Require the refractory counter to be zero.
8. On fire, clear the armed side and start the refractory counter.
9. Do not rearm until the oscillator reaches the opposite arm threshold.

The first strike begins with `ArmedSide::None`; therefore its initial departure
cannot fire a crossing. A retrigger does not reset the armed side. It starts the
4 ms refractory period so an impact and crossing cannot accidentally become
the same transient.

The initial thresholds are tunable. Automated tests define the semantics, not
the exact threshold values.

## 9. Motion articulation

Normalize velocity using the flex oscillator's current safe maximum:

```cpp
float v = clamp(abs(flex.velocity) / maximumVelocity, 0.f, 1.f);
float targetPulse = pow(v, pulseExponent);
```

Start with:

```text
pulseExponent:       2.4
radiationFloor:      0.08
radiationDepth:      0.92
pulse attack:        0.8 ms
pulse release:       3.0 ms
```

Smooth `targetPulse` with separate one-pole attack and release coefficients.
The smoothing suppresses sample-level gain roughness while preserving distinct
half-cycle lobes.

Direction is the sign of flex velocity at the most recent valid crossing. If
no crossing has occurred, use strike direction. Direction may influence:

* Overall radiation gain.
* A low/high spectral tilt across mode gains.
* Crossing-excitation magnitude.
* Mount coupling.

Reference V1 should begin with no more than ±10% overall directional gain and
no more than ±12% high-mode tilt. Specimen variation may move values within
those bounds but must not make one side inaudible.

## 10. Metallic modal body

### 10.1 Modes

Reference V1 uses exactly eight modes:

```cpp
constexpr int REFERENCE_MODE_COUNT = 8;

constexpr std::array<float, REFERENCE_MODE_COUNT>
REFERENCE_REST_FREQUENCIES_HZ {{
    245.f,
    318.f,
    405.f,
    545.f,
    730.f,
    980.f,
    1370.f,
    2050.f
}};
```

These are bootstrap values. They must ultimately be replaced or adjusted by
the reproducible fitting loop. Do not expose them as UI parameters.

Each mode stores position and velocity. Use a bounded semi-implicit update or
another fixed-cost resonator formulation demonstrated stable by the
sample-rate tests. Clamp effective frequency below `0.18 * sampleRate`.

### 10.2 Decay and gain

Initial decay regions:

```text
245–405 Hz:    0.9–1.5 s T60
545–980 Hz:    0.35–0.9 s T60
1370–2050 Hz:  0.12–0.45 s T60
```

Initial impact and output gains should broadly fall with frequency, but the
hard-strike brightness curve may increase upper-mode excitation.

All mode state persists between radiation lobes. Crossing articulation adds to
the existing state; it never clears or recreates a mode.

### 10.3 Shared pitch descent

```cpp
float pitchWarp =
    1.f
    + highEnergyPitchRise
        * pow(normalizedFlexEnergy, pitchWarpExponent);

float effectiveFrequency =
    restingFrequency
    * specimenFrequencyScale
    * pitchWarp
    * individualDynamicWarp;
```

Starting region:

```text
highEnergyPitchRise:    0.12–0.22
pitchWarpExponent:      0.45–0.80
individualDynamicWarp:  within ±1.5%
```

The shared component must dominate. All important modes should audibly descend
together as flex energy decays. Warp is computed from a smoothed energy value
and must not introduce discontinuities on retrigger.

### 10.4 Crossing excitation and energy bound

For a crossing with normalized speed \(v\):

```cpp
float rawStrength =
    crossingGain
    * pow(v, crossingExponent)
    * sqrt(clamp(normalizedFlexEnergy, 0.f, 1.f));
```

Compute current normalized modal energy and a soft remaining-capacity term:

```cpp
float available =
    clamp(1.f - totalModalEnergy / modalEnergyCeiling, 0.f, 1.f);

float crossingStrength = rawStrength * available;
```

Then apply fixed, specimen-adjusted, signed per-mode coupling coefficients.
Mode velocities and total modal energy must remain bounded after the update.

Starting values:

```text
crossingExponent:    1.5–2.5
modalEnergyCeiling:  calibrated so repeated full strikes remain below limiter
```

The crossing must not reset modal phase. Direction changes the coupling vector
slightly; it does not simply invert every mode.

## 11. Impact, mount, flex, and optional texture paths

### 11.1 Impact snap

Retain the legacy design principle but use independent state:

* Short filtered-noise excitation.
* Velocity-dependent brightness.
* Approximately 3–12 ms decay region.
* High-pass or low-reject filtering to prevent a DC impulse.

The impact excites the modal body once in addition to producing its own short
audio transient.

### 11.2 Mount thump

Use one low mount resonance initially, near 80–120 Hz, with a short decay.
Keep it subordinate to the metallic body and apply the existing anti-thump
lesson to hard retriggers.

### 11.3 Direct flex path

The flex path may contain normalized velocity and acceleration, but it must be
high-passed and mixed faintly. It exists to retain tactile motion and
retrigger weight, not to make 16 Hz the perceived pitch.

### 11.4 Dispersive texture

Do not include a waveguide in the first matching milestone. Once the modal,
crossing, and radiation behavior passes, an optional fixed small dispersive
texture may be added behind the modal body:

```cpp
body = modalBody + waveguideAmount * dispersiveTexture;
```

Its round-trip period must not appear as a dominant low-frequency output. Add
it only if level-matched comparison shows a repeatable improvement.

## 12. Specimen identity

### 12.1 Seed generation

`specimenSeed` is nonzero. A new module obtains a seed outside the audio
thread, using Rack's random facility or an equivalent host-safe source. The
audio thread receives it through the existing pending-state pattern.

The seed is serialized. Loading, saving, duplication, sample-rate changes,
engine switching, restoring factory freshness, and non-finite recovery do not
change it.

### 12.2 Deterministic derivation

Do not consume a mutable random stream while processing audio to obtain static
specimen properties. Derive each property independently:

```cpp
float specimenUnit(std::uint32_t seed, std::uint32_t propertyTag);
```

Hash `seed` with a unique compile-time tag, then map to `[-1, 1]`. Adding a new
property later must not change previously defined properties.

Reference V1 may vary:

| Property | Maximum initial variation |
| --- | ---: |
| Individual resting mode frequency | ±3% |
| Modal T60 | ±12% |
| Directional radiation | ±10% |
| Mount frequency | ±6% |
| Pulse exponent | ±8% |
| Upper-mode gain/brightness | ±12% |

Variation is applied when the seed or break-in changes, not per sample.

Dynamic noise may use a separate deterministic mutable RNG. Resetting motion
resets that RNG to a value derived from `specimenSeed`, making standalone test
renders reproducible.

## 13. Break-in behavior

Use the existing strike-dose law so condition progression remains familiar:

```cpp
breakIn += pow(shapedStrikeMagnitude, 1.8f) / 1000.f;
```

Clamp to `[0, 1]` and do nothing while locked. Apply the dose once per accepted
nonzero strike regardless of engine.

Reference-engine break-in may:

* Lower flex frequency.
* Lower damping and lengthen settle time.
* Lower modal frequencies slightly.
* Lengthen modal decays.
* Increase asymmetry modestly.
* Soften impact brightness.

Specimen identity and break-in are independent. Break-in modifies the same
specimen rather than selecting a new set of physics.

## 14. Output conditioning, safety, and sleep

The reference engine must:

* Reject non-finite strike and configuration inputs.
* Remain finite at 44.1, 48, 88.2, 96, and 192 kHz.
* Clamp effective modal frequencies to a stable sample-rate-relative ceiling.
* Bound flex velocity, modal velocity, modal energy, impact envelopes, and
  transition gain.
* Apply DC blocking before final output.
* Use a smooth saturator or limiter to remain within ±5 V.
* Recover from a non-finite dynamic state by clearing motion while preserving
  engine selection, break-in, lock state, and specimen seed.

Sleep requires all of the following to remain below threshold for a hold time:

* Flex energy.
* Every modal energy.
* Impact envelope.
* Mount state.
* Radiation envelope.
* Transition tail.
* Final output.

On sleep, clear dynamic state and return an exact zero frame until the next
accepted strike or engine-change command.

## 15. Persistence

### 15.1 Schema 2

Write:

```json
{
  "schema": 2,
  "allowVisualOverflow": true,
  "engineMode": "referenceV1",
  "legacySoundModel": "probabilisticMix",
  "soundModel": 4,
  "specimenSeed": 18374625,
  "breakIn": 0.37,
  "breakInLocked": false
}
```

`soundModel` remains as a numeric compatibility mirror of
`legacySoundModel`. An older plugin will ignore the unknown reference fields
and open the patch using the selected legacy fallback.

Canonical strings are:

```text
engineMode:
  referenceV1
  legacy

legacySoundModel:
  probabilisticMix
  classic
  coupledBody
  coilContact
  dispersiveSpring
```

### 15.2 Loading rules

1. Start with safe defaults.
2. If `engineMode` is absent, load `Legacy`.
3. For an old patch, restore the valid numeric `soundModel`; otherwise use
   `ProbabilisticMix`.
4. If `engineMode` is valid, use it.
5. Restore `legacySoundModel` from its valid string. Fall back to the numeric
   `soundModel`, then `ProbabilisticMix`.
6. Restore a valid nonzero `specimenSeed`. If absent, create one outside the
   audio thread and retain it when the loaded state is applied.
7. Clamp finite break-in to `[0, 1]`; invalid values become zero.
8. Invalid enum strings never select an out-of-range value.

`dataFromJson(nullptr)` follows old-patch compatibility rules and therefore
selects `Legacy / ProbabilisticMix`. The constructor without a load operation
selects `ReferenceV1`. This distinction is required for new-module defaults
and old-patch compatibility to coexist.

## 16. Standalone render and comparison loop

### 16.1 Render tool

`tools/doorstop_reference_render.cpp` links only engine and math sources. It
must render deterministic mono floating-point WAV files for:

* Sample rates: 44.1, 48, 96, and 192 kHz.
* Strike magnitudes: 0.1, 0.5, 0.75, and 1.0.
* Positive and negative strikes.
* Fresh and fully broken-in conditions.
* At least three fixed specimen seeds.
* Same-direction and opposite-direction retriggers at multiple flex phases.
* The reference engine and every legacy model.

Every output filename or adjacent manifest records engine version, sample
rate, strike sequence, condition, seed, and build revision.

### 16.2 Analysis tool

`tools/analyze_doorstop_reference.py` must record its complete method:

* Input crop and silence-trimming rules.
* Optional high-pass filtering.
* Window type and size.
* Spectral centroid definition.
* Band-energy calculation.
* Envelope extraction.
* Peak-picking thresholds and minimum spacing.
* Pulse-rate calculation.
* Level normalization used for comparison.

It produces machine-readable JSON/CSV plus spectrogram and envelope plots.
Reference recordings are described by a manifest with source name, checksum,
sample rate, channel handling, and crop range.

Do not encode the provisional centroid and pulse figures as hard acceptance
limits until the fixture is reproducible.

### 16.3 Hypothesis comparison

The harness must support three compile-time or command-line articulation
variants:

1. Continuous radiation gate only.
2. Crossing excitation only.
3. Continuous radiation plus bounded crossing excitation.

Compare them level-matched. Reference V1 selects the simplest variant that
consistently reproduces the audible lobe timing and metallic persistence. The
combined variant is the expected winner, not a foregone conclusion.

## 17. Automated tests

### 17.1 Legacy preservation

* Golden controlled renders match before and after routing integration.
* Legacy RNG selects the same model sequence from reset.
* Probabilistic distribution tests remain unchanged.
* Existing Coil Contact medium-strike equivalence remains unchanged.
* Existing settling, retrigger, and break-in tests remain unchanged.
* Selecting Legacy calls the unchanged `doorstop::Engine` computation path.

### 17.2 Crossing semantics

* Initial departure creates no crossing.
* Each clean flex cycle produces two events after the initial departure.
* Exact-zero samples do not create repeated events.
* Low-velocity center dithering creates no event.
* A valid event cannot repeat inside the refractory interval.
* Positive and negative initial strikes produce symmetric event counts.
* Same- and opposite-direction retriggers remain bounded.

Expose a test-only crossing count or diagnostics structure without adding
audio-thread logging.

### 17.3 Reference behavior

* Low, medium, and hard hits are finite and eventually sleep.
* Increasing strike magnitude increases early modal energy.
* The modal body has materially more 220 Hz–3 kHz energy than the direct flex
  path.
* A medium/hard render contains repeated envelope lobes after the impact.
* Lobe amplitude decays over time in the absence of retriggering.
* Modal energy never exceeds its configured ceiling.
* Radiation floor preserves modal continuity between lobes.
* Shared pitch warp decreases as flex energy decreases.
* Directional alternation remains inside configured bounds.
* Same seed and commands produce identical output.
* Different seeds produce different but bounded output.
* Sample-rate changes do not change seed, break-in, or engine selection.

### 17.4 Runtime and migration

* New constructor default is `ReferenceV1`.
* Schema-1 and schemaless patches load `Legacy`.
* Every old numeric `soundModel` value maps correctly.
* Schema-2 round-trip preserves engine, fallback model, seed, break-in, lock,
  and visual-overflow state.
* Invalid JSON restores safe defaults.
* Engine switching produces no discontinuity exceeding a defined transition
  threshold.
* A strike during transition reaches only the destination engine.
* Restore freshness preserves seed and lock state.
* Generate-new-specimen changes seed, clears motion, and clears break-in.
* Non-finite recovery preserves all persistent state.

## 18. Performance budget

Measure in the Rack process path with debug instrumentation disabled.

Reference V1 should:

* Allocate zero bytes per sample and per strike.
* Use eight modal states and fixed auxiliary state.
* Perform no trigonometric work per mode per sample unless profiling
  demonstrates that it fits the project budget.
* Sleep to the existing near-zero idle cost.
* Keep steady active CPU within 2× the current Coupled Body engine at the same
  sample rate.
* Permit the temporary 15 ms two-engine transition without missing Rack's
  audio deadline.

If frequency-varying resonators require expensive coefficient updates, update
coefficients at a bounded control interval and interpolate them. The update
rate must be high enough that the pitch descent remains smooth.

## 19. Implementation sequence

### Phase 0 — Reproducibility and legacy baseline

1. Add deterministic legacy render cases and golden hashes.
2. Record the current legacy model-selection sequence.
3. Add a manifest and documented analysis path for available reference audio.
4. Fix the math formatting in `ReferenceSpring.md`.

Exit condition: the unchanged legacy implementation has a recorded baseline
that detects any accidental change introduced around its call boundary.

### Phase 1 — Engine boundary

1. Leave the current `doorstop::Engine` and `SoundModel` implementation in
   place.
2. Introduce `DoorstopEngineRouter` and the common frame around it.
3. Keep the module behavior and schema at version 1 temporarily.
4. Run all existing Doorstop tests.

Exit condition: all legacy tests and golden renders pass, and a routed Legacy
strike reaches the unchanged engine methods.

### Phase 2 — Reference DSP prototype

1. Implement flex state and bounded integration.
2. Implement the hysteretic crossing detector and its unit tests.
3. Add the eight-mode body.
4. Add impact and mount paths.
5. Implement the three articulation variants.
6. Add shared pitch warp and specimen-derived coefficients.
7. Add sleep, recovery, and output conditioning.

Exit condition: all automated stability and behavioral tests pass in the
standalone harness.

### Phase 3 — Reference fitting

1. Render the complete comparison matrix.
2. Compare pulse timing, band energy, pitch trajectories, and decay envelopes.
3. Select the articulation variant.
4. Fit modal clusters and bounded parameter ranges.
5. Perform level-matched listening comparisons.

Exit condition: the reference engine consistently improves resemblance without
losing velocity response or retrigger stability.

### Phase 4 — Rack integration and migration

1. Add engine selection and transitions.
2. Add specimen generation and menu behavior.
3. Implement schema 2 and migration tests.
4. Adapt animation publication to the common frame.
5. Add transition and audio-thread performance tests.

Exit condition: new, old, malformed, and duplicated patches behave according
to this specification.

### Phase 5 — Default promotion

1. Run the full plugin test suite.
2. Profile at all supported sample rates.
3. Verify old-patch legacy renders.
4. Complete listening acceptance.
5. Promote `ReferenceV1` from candidate to the new-module default.

## 20. Acceptance criteria

Reference V1 is complete when:

1. Existing patches retain their selected legacy sound and deterministic
   legacy behavior.
2. Newly created modules use the reference engine.
3. The reference engine produces two qualified articulation opportunities per
   established flex cycle without duplicate center events.
4. Its primary audible energy is a decaying inharmonic metallic body rather
   than the sub-200 Hz flex motion.
5. Medium and hard strikes produce repeated, decaying, physically coherent
   lobes after the initial impact.
6. The body remains continuous between lobes and does not phase-reset.
7. Repeated crossings and retriggers cannot create unbounded modal energy.
8. Shared pitch descent, directional asymmetry, and specimen variation remain
   bounded and stable.
9. Every supported sample rate remains finite, within ±5 V, and eventually
   sleeps.
10. Engine switching is click-safe and does not resurrect discarded state.
11. Patch schema, reset, restore, duplication, and specimen-generation behavior
    match this document.
12. The comparison procedure is reproducible and the reference engine wins the
    agreed level-matched listening and measurement evaluation.

## 21. Deferred decisions

The following decisions require reference-fixture results rather than further
architectural speculation:

* Exact modal frequencies, decays, and gains.
* Whether eight modes are sufficient or a later version needs more.
* Exact radiation pulse exponent and floor.
* The amount of crossing re-excitation relative to pure radiation gating.
* The size and contour of shared pitch descent.
* Whether a dispersive texture provides a repeatable improvement.
* Quantitative centroid, band-energy, and envelope-correlation acceptance
  thresholds.

These values may change during fitting without changing the public schema.
Changes that materially alter the saved specimen interpretation require a new
engine identifier such as `ReferenceV2`; a saved `ReferenceV1` patch must not
silently acquire incompatible specimen physics.
