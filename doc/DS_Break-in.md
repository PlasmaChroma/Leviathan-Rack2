# Break-in Modeling for Doorstop Module

Dragon King Leviathan, this makes excellent sense, and the current architecture is almost ideally prepared for it. The engine already concentrates strike handling, spring coefficients, energy limiting, modal behavior, and dynamic reset in one place; the module already has custom JSON serialization; and the widget already consumes decimated physical telemetry.

## The materials-science interpretation

There is one useful distinction to preserve:

A properly operating steel spring does not generally become smoothly and pleasantly “looser” merely from ordinary elastic cycling. Permanent settling is more closely related to **presetting, plastic deformation, residual-stress stabilization, and stress relaxation**. Classical fatigue more often accumulates toward crack initiation and eventual failure rather than producing a tidy bounded softening curve. Spring manufacturers even preset springs specifically so later working loads do not change their dimensions through further plastic deformation. ([SAE Mobilus][1])

Repeated cyclic loading can nevertheless be represented as cumulative damage, and changes in structural compliance or resonant behavior are used to detect fatigue progression. ([NIST][2])

So I would frame this feature as:

> **Accelerated mechanical conditioning inspired by spring presetting and fatigue accumulation, deliberately capped before actual structural failure.**

Internally, call it `breakIn`. Reserve `damage` for a possible future system where the spring can become defective, rattle badly, or snap.

---

# Recommended state model

Each Doorstop instance gets one persistent value:

```cpp
float breakIn = 0.f;
```

Range:

```text
0.0 = factory fresh
1.0 = fully broken-in
```

The value only increases unless the user explicitly restores the spring.

For the first implementation, use a cumulative dose model:

```cpp
float severity = Engine::shapeMagnitude(std::fabs(normalizedVelocity));
float dose = std::pow(severity, 1.8f);

breakIn = std::min(
    1.f,
    breakIn + dose / fullBreakInDose
);
```

Where `fullBreakInDose` is expressed in equivalent maximum-strength strikes.

A sensible starting calibration would be:

```cpp
float fullBreakInDose = 1000.f;
```

For v1, conditioning is deposited once per accepted non-zero strike. Use the
already-computed `shaped` magnitude inside `Engine::strike()` rather than
calling `shapeMagnitude()` a second time:

```cpp
accumulateBreakIn(std::pow(shaped, 1.8f) / fullBreakInDose);
```

Positive and negative strikes condition the spring identically by magnitude.
Zero and non-finite strikes add no conditioning.

That means:

* Approximately 1,000 maximum-strength strikes reach full break-in.
* Medium strikes contribute much less per event.
* Tiny strikes barely alter the spring.
* A fast sequencer can condition the module during a session.
* Casual manual use changes it slowly enough to feel persistent.

The exponent should remain tunable. Around `1.7–2.2` gives accelerated, musically useful wear; real fatigue-inspired exponents could make low-amplitude strikes contribute almost nothing.

## Why cumulative dose rather than elapsed time

Wear should only advance when the spring is physically exercised.

It should not change because:

* Rack is left running.
* The module is sitting silently.
* The patch is saved repeatedly.
* The sample rate changes.

This makes the history meaningful: the spring sounds old because someone—or something clocked at audio-adjacent rates—has been abusing it.

---

# Better physical refinement: count the ringing too

The strike itself is not the only mechanical event. One strike creates many bending cycles.

A more physically evocative version can combine two sources:

```text
Impact conditioning:
    Added once when strike() is called.

Cyclic conditioning:
    Added at each meaningful spring turning point.
```

Inside `processSpring()`, track the maximum displacement during each half-cycle:

```cpp
float halfCyclePeak = 0.f;
float previousVelocity = 0.f;
```

Each sample:

```cpp
halfCyclePeak = std::max(halfCyclePeak, std::fabs(displacement));

const bool turned =
    (previousVelocity > 0.f && springVelocity <= 0.f)
    || (previousVelocity < 0.f && springVelocity >= 0.f);

if (turned) {
    float amplitude =
        halfCyclePeak / effectiveMaxDisplacement;

    if (amplitude > minimumWearAmplitude) {
        float cycleDose = std::pow(amplitude, cycleWearExponent);
        accumulateBreakIn(cycleDose * cycleWearScale);
    }

    halfCyclePeak = std::fabs(displacement);
}

previousVelocity = springVelocity;
```

Suggested values:

```text
minimumWearAmplitude: 0.03–0.06
cycleWearExponent:    3.0–4.5
```

This produces desirable behavior:

* Hard hits cause more wear because they create large cycles.
* Long, loose oscillations contribute some additional conditioning.
* Tiny end-of-decay movements do not accumulate phantom wear.
* Repeated retriggers into an already energetic spring can condition it faster.

This refinement is explicitly deferred until after the per-strike version has
been implemented and auditioned. The first implementation does not track
turning points or deposit cyclic conditioning.

---

# What “looser” should actually change

Break-in should not merely lengthen an envelope. It should morph the physical coefficients.

Calculate a smoothed condition value:

```cpp
float w = breakIn * breakIn * (3.f - 2.f * breakIn);
```

Then derive effective coefficients from the original tuning.

## Primary spring

Suggested fully broken-in endpoints:

```cpp
effectiveBaseFrequency =
    tuning.baseFrequencyHz * lerp(1.f, 0.84f, w);

effectiveDampingRatio =
    tuning.dampingRatio * lerp(1.f, 0.65f, w);

effectiveNonlinearStiffness =
    tuning.nonlinearStiffness * lerp(1.f, 0.72f, w);

effectiveMaxDisplacement =
    tuning.maxDisplacement * lerp(1.f, 1.15f, w);
```

With the present values, this would approximately mean:

| Property             | Fresh | Fully broken-in |
| -------------------- | ----: | --------------: |
| Base frequency       | 16 Hz |         13.4 Hz |
| Damping ratio        | 0.020 |           0.013 |
| Nonlinear stiffness  |  1.40 |            1.01 |
| Maximum displacement |  2.00 |            2.30 |

The audible result should be:

* Slightly lower fundamental motion.
* Longer settling.
* Wider swings.
* Less rigid pitch hardening.
* More obvious low-frequency wobble.
* Greater interaction between closely spaced retriggers.

That reads as an increasingly compliant spring rather than a simple decay-time increase.

## Modal body

The metallic modes should age more subtly:

```cpp
effectiveModeFrequency[i] =
    tuning.modeFrequenciesHz[i]
    * lerp(1.f, modeWearFrequencyScale[i], w);

effectiveModeDecay[i] =
    tuning.modeDecayT60Seconds[i]
    * lerp(1.f, modeWearDecayScale[i], w);
```

Possible endpoint scales:

```cpp
modeWearFrequencyScale = {
    0.96f,
    0.94f,
    0.92f,
    0.90f
};

modeWearDecayScale = {
    1.25f,
    1.22f,
    1.18f,
    1.12f
};
```

This makes an older Doorstop:

* Slightly deeper.
* Less taut.
* More resonant.
* Somewhat less brilliant in its highest mode.
* Still unmistakably the same object.

I would not radically alter the impact noise in the first pass. The evolving spring and resonator body will already provide a strong difference.

---

# Where this belongs in the existing engine

The cleanest implementation is inside `doorstop::Engine`.

Add:

```cpp
float breakIn = 0.f;
bool breakInLocked = false;

struct EffectiveTuning {
    float baseFrequencyHz = 16.f;
    float dampingRatio = 0.020f;
    float nonlinearStiffness = 1.4f;
    float maxDisplacement = 2.f;
    std::array<float, MODE_COUNT> modeFrequenciesHz {};
    std::array<float, MODE_COUNT> modeDecayT60Seconds {};
};

EffectiveTuning effectiveTuning;
```

And public methods:

```cpp
float getBreakIn() const;
void setBreakIn(float amount);
void restoreFactoryFresh();
void setBreakInLocked(bool locked);
bool isBreakInLocked() const;
const EffectiveTuning& getEffectiveTuning() const;
```

The engine’s current `strike()` method is the natural place to deposit the primary wear dose because it already calculates the shaped strike magnitude.

Do not mutate `Tuning` itself as the spring wears. Treat `Tuning` as the factory specification and calculate cached effective values from it. Otherwise repeated updates risk parameter drift and make restoring the spring less reliable.

`setBreakIn()` is an explicit state-restoration/control operation:

* Finite values clamp to `[0, 1]`.
* Non-finite values are ignored.
* It updates the effective coefficients immediately.
* It works even while break-in is locked. Locking prevents accumulated wear;
  it does not prevent patch restoration or explicit factory restoration.

The engine owns the authoritative break-in value and physical coefficients.
The module's atomic lock value is the requested persistent configuration; the
audio thread mirrors it into the engine before accepting strikes. Other
atomics on `Doorstop` are commands and telemetry for crossing the
UI/audio-thread boundary, not a second physical state model.

---

# Coefficient recalculation

The present `updateCoefficients()` calculates:

* Spring angular frequency.
* Maximum velocity.
* Energy ceiling.
* Noise filters.
* Decay coefficients.
* DC blocker coefficients.

Wear changes only part of that system. I would split it:

```cpp
void updateSampleRateCoefficients();
void updateWearCoefficients();
void updateEnergyCoefficients();
```

`updateWearCoefficients()` should recalculate:

* Effective base frequency.
* Effective damping.
* Effective nonlinear stiffness.
* Effective maximum displacement.
* Effective mode frequencies.
* Effective modal decay.
* `baseOmega`.
* `baseOmegaSq`.
* `maxVelocity`.
* Energy ceiling and knee.

Call it:

* After `setBreakIn()`.
* After every accepted strike that changes break-in in v1.
* After sample-rate changes.
* After factory restoration.

There is no need to recalculate expensive exponentials every audio sample. In
v1, wear only changes on accepted strikes or explicit state changes.

Because individual wear increments are tiny, retuning an active spring after an increment should remain smooth. Patch loading is even simpler because the engine begins at rest.

---

# Energy calculations must use the worn coefficients

This is important.

The current engine calculates spring potential, energy ceiling, velocity limiting, and normalized energy from the spring coefficients.

Once wear changes stiffness, these functions must use the **effective** values:

```cpp
float Engine::springPotential(float x) const {
    const float x2 = x * x;

    return 0.5f * effectiveBaseOmegaSq * x2
        + 0.25f
            * effectiveNonlinearStiffness
            * effectiveBaseOmegaSq
            * x2 * x2;
}
```

Likewise:

* `processSpring()` uses effective damping and stiffness.
* `processModes()` uses effective mode frequency and T60 values.
* `normalizedModeEnergy()` uses the effective mode frequency.
* `energyCeiling` uses the effective maximum displacement.
* Displacement clamping uses the effective maximum displacement.
* Coil-contact displacement normalization uses the effective maximum
  displacement.
* Visual telemetry clamps against
  `getEffectiveTuning().maxDisplacement`, not the factory value in `Tuning`.

Otherwise the limiter and sleep detector would still reason about a factory-fresh spring while the audio engine simulates a worn one.

---

# Reset semantics

The existing `Engine::reset()` only clears dynamic movement, which is currently sufficient because there is no persistent physical history.

With break-in, distinguish two operations:

```cpp
void resetMotion();
void restoreFactoryFresh();
```

## `resetMotion()`

Clears:

* Displacement.
* Velocity.
* Modal state.
* Impact envelopes.
* Filter history.
* Sleep state.

Preserves:

* Break-in.
* Break-in lock state.

Use this during:

* Patch loading.
* Recovery from non-finite state.
* Internal DSP reinitialization.

## `restoreFactoryFresh()`

Clears motion and sets:

```cpp
breakIn = 0.f;
```

Use this when:

* A brand-new module is constructed.
* Rack’s module reset/initialize action is invoked.
* The user selects a context-menu restoration command.

This distinction prevents a numerical recovery or patch load from mysteriously rejuvenating the hardware.

`restoreFactoryFresh()` resets break-in to `0.0` and clears motion. Rack’s
module reset additionally restores the break-in lock to its default, unlocked
state. A context-menu factory restoration clears the condition but preserves
the current lock choice, so a user can restore and leave the spring frozen
fresh.

---

# Patch serialization

`Doorstop::dataToJson()` currently stores `allowVisualOverflow` and
`soundModel`, and `dataFromJson()` resets the engine before restoring those
settings. Preserve both existing fields while adding break-in state.

Extend the JSON:

```cpp
json_object_set_new(
    rootJ,
    "breakIn",
    json_real(serializedBreakIn.load(std::memory_order_relaxed))
);

json_object_set_new(
    rootJ,
    "breakInLocked",
    json_boolean(breakInLocked.load(std::memory_order_relaxed))
);
```

Loading:

```cpp
float restoredBreakIn = 0.f;
bool restoredLocked = false;

json_t* breakInJ = json_object_get(rootJ, "breakIn");
if (json_is_number(breakInJ)) {
    const double value = json_number_value(breakInJ);

    if (std::isfinite(value)) {
        restoredBreakIn =
            clamp(static_cast<float>(value), 0.f, 1.f);
    }
}

json_t* lockedJ = json_object_get(rootJ, "breakInLocked");
if (json_is_boolean(lockedJ)) {
    restoredLocked = json_boolean_value(lockedJ);
}

pendingBreakIn.store(restoredBreakIn, std::memory_order_relaxed);
serializedBreakIn.store(restoredBreakIn, std::memory_order_relaxed);
breakInLocked.store(restoredLocked, std::memory_order_relaxed);
breakInStatePending.store(true, std::memory_order_release);
```

Backward compatibility is automatic:

* Old patches have no `breakIn` key.
* Missing values default to factory fresh.
* Invalid values clamp safely.
* New patches retain per-instance condition.
* A missing `breakInLocked` value defaults to unlocked.

Duplicating a module would duplicate its condition, which feels appropriate: the duplicate is effectively a copy of that exact virtual object.

---

# Thread-safe UI control

I strongly recommend two context-menu functions:

```text
Break-in: 37%
Lock break-in
Restore factory-fresh spring
```

The percentage item can be informational and disabled.

Because the audio engine is owned by the audio thread, the UI callback should not directly rewrite live engine state. Use atomic command fields in `Doorstop`:

```cpp
std::atomic<bool> breakInLocked {false};
std::atomic<bool> restoreSpringRequested {false};
std::atomic<float> serializedBreakIn {0.f};
std::atomic<float> pendingBreakIn {0.f};
std::atomic<bool> breakInStatePending {false};
```

The audio thread is the only writer to live engine state. It consumes patch
state and UI commands near the beginning of `process()`, before handling a
strike:

```cpp
if (breakInStatePending.exchange(false, std::memory_order_acquire)) {
    engine.resetMotion();
    engine.setBreakIn(pendingBreakIn.load(std::memory_order_relaxed));
}

engine.setBreakInLocked(
    breakInLocked.load(std::memory_order_relaxed)
);

if (restoreSpringRequested.exchange(false)) {
    engine.restoreFactoryFresh();
}
```

After consuming commands and after any strike, the audio thread publishes the
engine’s authoritative condition into `serializedBreakIn`. JSON serialization
and the context menu read the atomic mirrors. A context-menu lock toggle only
updates `breakInLocked`; the audio thread applies it to the engine on its next
process call.

Locking break-in is worth including because this is a genuinely persistent sonic mutation. It gives users three useful modes:

* Let the instrument naturally evolve.
* Freeze a particularly good mature state.
* Restore the original factory sound.

---

# Visual consequences

No major visual redesign is required. The current spring already derives its travel from physical displacement and renders overflow from the same snapshot.

The greater effective displacement and lower damping will naturally make a broken-in spring:

* Swing farther.
* Cross the panel boundary more often.
* Settle more slowly.
* Show longer motion trails.

Add this telemetry only if we want visible aging:

```cpp
std::atomic<float> visualBreakIn {0.f};
```

Possible subtle treatments:

* Slightly darker spring metal.
* Reduced cyan highlight and increased warm patina.
* A tiny increase in cap wobble.
* Slightly less geometrically perfect coil spacing.
* A faint reversal rattle at high break-in.

I would defer visual tarnish until the acoustic progression is proven. The changed motion itself may already communicate age beautifully.

No separate `visualBreakIn` value is required for v1. The existing displacement,
velocity, and energy telemetry is sufficient, provided displacement is clamped
against the effective worn limit rather than the factory tuning limit.

---

# Recommended first implementation boundary

For the initial feature, I would implement:

1. Persistent `breakIn` in the engine.
2. Per-strike cumulative conditioning.
3. A hard cap at `1.0`.
4. Break-in modulation of primary frequency, damping, nonlinear stiffness, maximum displacement, modal frequencies, and modal decay.
5. Patch serialization.
6. Context-menu percentage, lock, and factory restoration.
7. Tests proving deterministic progression and persistence.

Then add half-cycle wear only after listening to the per-strike version. It is physically richer, but the primary musical question is whether a fresh Doorstop and a fully broken-in Doorstop feel like two ages of the same object.

The v1 coefficient morph applies to the primary spring and to all modal banks
(`Classic`, `CoupledBody`, and `CoilContact`). Impact-noise, thump, coil-contact
decay, and dispersive-waveguide coefficients remain unchanged.

## Acceptance criteria

The implementation is complete when tests demonstrate:

1. A fresh engine reports `breakIn == 0.0` and is unlocked.
2. Exactly 1,000 accepted full-strength strikes advance an unlocked fresh
   engine to `breakIn == 1.0`, within floating-point tolerance.
3. Positive and negative strikes of equal magnitude add equal wear; zero and
   non-finite strikes add none.
4. Medium strikes advance more slowly according to
   `pow(shapeMagnitude(magnitude), 1.8)`.
5. Break-in never exceeds `1.0`, never decreases through ordinary processing,
   and does not change with elapsed silent time or sample-rate changes.
6. Locking freezes accumulated wear without changing the current sound;
   explicit `setBreakIn()` and factory restoration still work while locked.
7. At full break-in, effective primary and modal coefficients match the
   documented endpoint scales within floating-point tolerance.
8. Motion reset and non-finite recovery preserve condition and lock state.
   Rack module reset restores factory-fresh, unlocked defaults.
9. JSON round-trips condition and lock state. Old patches with neither key load
   factory-fresh and unlocked, and invalid numeric values are handled safely.
10. Processing remains finite and bounded at supported sample rates, with
    displacement bounded by the effective worn maximum.

The central design feels very coherent:

> Doorstop does not merely simulate the current movement of a spring. It remembers the history of forces that have passed through it.

That turns each module instance into a tiny persistent artifact—less like loading a synthesizer voice, more like owning a particular piece of hardware that has been struck a thousand times.

[1]: https://saemobilus.sae.org/papers/experimental-numerical-analysis-coil-springs-preset-plastic-deformation-2012-36-0450?utm_source=chatgpt.com "2012-36-0450: Experimental and Numerical Analysis on Coil Springs Preset Plastic Deformation - Technical Paper"
[2]: https://www.itl.nist.gov/div898//handbook/apr/section1/apr166.htm?utm_source=chatgpt.com "8.1.6.6. Fatigue life (Birnbaum-Saunders)"
