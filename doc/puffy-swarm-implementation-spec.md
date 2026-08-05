# Puffy — SWARM Character Implementation Specification

**Status:** Implementation handoff  
**Target branch:** `expander`  
**Module:** `Puffy`  
**Feature:** Sixth saturation character, provisionally named `SWARM`  
**Primary priorities:** audible identity, real-time safety, low audio-thread cost, low redraw cost, patch compatibility

---

## 1. Product Contract

`SWARM` adds a stochastic saturation character to Puffy.

The audible concept is not “saturation plus white noise.” Instead, each sample passes through a slightly unstable nonlinear trajectory whose distribution increasingly gathers near the positive or negative saturation rail as `PUFF` rises.

The essential relationship is:

```text
signal amplitude
+ bounded shared chaos
+ rail attraction
-> particulate saturation
```

At low `PUFF`, SWARM should sound like subtle microscopic instability around a mostly coherent transfer curve. At medium `PUFF`, the transfer becomes granular, rough, and electrically alive. At high `PUFF`, louder samples should form a dense probabilistic cloud near the rails without becoming an unrelated broadband noise generator.

The visual transfer preview should communicate this as a point cloud around a central transfer curve. The visual cloud is representative rather than sample-accurate.

### 1.1 Name

The implementation name is:

```cpp
Character::Swarm
```

The user-facing label is:

```text
SWARM
```

Do not rename existing characters.

### 1.2 Success Criteria

The feature is complete when:

- Existing patches retain their prior character selections.
- `SWARM` is available independently for negative and positive polarities.
- Zero input produces zero output.
- At `PUFF = 0`, SWARM is effectively identical to the existing clean path.
- Identical stereo inputs remain sample-identical at the outputs.
- SWARM creates audible stochastic texture without adding an independent noise floor.
- Increasing `PUFF` increases both nonlinear density and statistical rail attraction.
- Character changes remain click-free through Puffy’s existing transition system.
- The audio thread performs no allocation, locking, logging, file I/O, or UI work.
- Non-SWARM modes do not pay a meaningful steady-state CPU penalty.
- The transfer preview remains framebuffer-cached and does not redraw continuously in a stable state.
- The existing Puffy character animation remains within its current performance envelope.

---

## 2. Scope and Non-Goals

### 2.1 In Scope

- Append `SWARM` as character index `5`.
- Add a fast stochastic saturation kernel.
- Add deterministic, seedable PRNG state to `puffy::Engine`.
- Share one chaos frame across stereo channels and character transition paths.
- Add SWARM labels, clamping, palette color, and transfer-preview treatment.
- Extend engine tests and performance checks.
- Update relevant Puffy documentation.

### 2.2 Explicit Non-Goals

This change does not add:

- a user-facing noise amount control;
- a stereo-width or decorrelation control;
- independently randomized left and right channels;
- random values generated per oversampled lane;
- additive white, pink, or brown noise;
- sample-accurate audio data sent to the UI;
- an animated particle system around the fish;
- a new OpenGL renderer;
- a new persistence field for transient random state;
- a change to the limiter, DC blocker, oversampling factor, wet/dry architecture, or signal calibration.

A later visual pass may add a small particle ornament around Puffy, but it is not part of this implementation and must not block the DSP character.

---

## 3. Compatibility and Stable IDs

Append the new character. Do not insert it between existing values.

```cpp
enum class Character {
    Bloom = 0,
    Spine = 1,
    Frenzy = 2,
    Riptide = 3,
    Void = 4,
    Swarm = 5
};

constexpr int kCharacterCount = int(Character::Swarm) + 1;
```

This ordering is mandatory because Rack serializes snapped parameter values numerically.

Update both character parameters:

```cpp
configSwitch(
    CHARACTER_PARAM,
    0.f,
    float(puffy::kCharacterCount - 1),
    0.f,
    "Negative character",
    {"BLOOM", "SPINE", "FRENZY", "RIPTIDE", "VOID", "SWARM"});

configSwitch(
    POSITIVE_CHARACTER_PARAM,
    0.f,
    float(puffy::kCharacterCount - 1),
    0.f,
    "Positive character",
    {"BLOOM", "SPINE", "FRENZY", "RIPTIDE", "VOID", "SWARM"});
```

Replace every use of `Character::Void` as an upper-bound sentinel with:

```cpp
puffy::kCharacterCount - 1
```

or a helper derived from `kCharacterCount`.

Known areas include:

- `Puffy.cpp` parameter clamping;
- `PuffyWidget.cpp` readout clamping;
- linked-character button wraparound;
- `clampCharacter()` in `PuffyEngine.cpp`;
- any test that assumes `VOID` is the final character.

No new JSON schema field is required. Rack parameter persistence is sufficient. Existing patches using values `0..4` must load unchanged.

---

## 4. DSP Identity

### 4.1 Design Principle

SWARM is a **multiplicative, signal-dependent stochastic waveshaper**.

Randomness changes the instantaneous nonlinear drive and the strength with which a sample is pulled toward its polarity rail. Randomness must not be added directly to the output.

Required properties:

```text
f(0, chaos) = 0
f(x, chaos, amount=0) = x
f(-x, chaos) = -f(x, chaos)
finite input -> finite output
fully wet shaped target remains in [-1, +1]
```

The odd-symmetry property is evaluated with the same chaos value on both polarities. Selecting SWARM on only one polarity may intentionally create an asymmetric overall transfer.

### 4.2 Signal-Flow Position

Do not alter Puffy’s signal order.

SWARM occupies the same character stage as the other modes:

```text
input safety
-> sensitivity projection
-> base-rate dynamics and control updates
-> 4x oversampling
-> selected character, including SWARM
-> oversampled wet/dry blend
-> decimation
-> DC blocker
-> Auto Deflate
-> linked limiter
-> finite/output guard
```

### 4.3 Fast-Math Constraint

The SWARM hot path must use only:

- addition;
- subtraction;
- multiplication;
- comparisons;
- `fabs`;
- `min` / `max`;
- `copysign` or equivalent sign selection;
- the shared `tanhAudio()` lookup table with linear interpolation;
- bounded float-to-integer conversion for lookup indexing;
- integer XOR and bit shifts for PRNG generation.

The SWARM per-lane kernel must not call:

- `sin`;
- `cos`;
- `tan`;
- `tanh`;
- `exp`;
- `log`;
- `pow`;
- division;
- heap allocation;
- virtual dispatch.

A one-pole coefficient may use `exp()` in `setSampleRate()`, where Puffy already calculates control-rate coefficients. It must not be recalculated per sample.

---

## 5. Random Source and Chaos Frame

### 5.1 PRNG

Use a compact 32-bit xorshift generator.

Reference form:

```cpp
static uint32_t xorshift32(uint32_t& state) {
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return state;
}
```

The state must never be zero. Sanitize a zero seed to a fixed nonzero constant.

Convert the upper 24 bits to a signed float approximately in `[-1, +1)`:

```cpp
float signedRandom(uint32_t& state) {
    const uint32_t bits = xorshift32(state) >> 8;
    return float(bits) * (2.f / 16777216.f) - 1.f;
}
```

The exact conversion may be adjusted for code style, but it must be branch-light, finite, and deterministic.

### 5.2 Seed Contract

Add:

```cpp
void setSwarmSeed(uint32_t seed);
```

The engine owns:

```cpp
uint32_t swarmInitialSeed;
uint32_t swarmRngState;
```

`reset()` restores `swarmRngState` from `swarmInitialSeed`.

`setSwarmSeed()` must sanitize and store the seed, restore `swarmRngState`, and
clear the interpolation history:

```cpp
swarmPreviousFast = 0.f;
swarmCurrentFast = 0.f;
swarmSlow = 0.f;
```

`reset()` must clear those three history values as well. Reseeding must make the
very next generated frame deterministic; it must not retain chaos history from
the prior seed. Neither operation resets unrelated Puffy DSP state.

For tests, identical seeds and identical inputs must produce identical output.

For runtime modules, `Puffy` should provide a process-local unique nonzero seed during construction, derived from a static atomic counter and a simple integer hash. Seed creation occurs outside the audio thread.

Do not serialize the transient PRNG position. Patch reload may restart the instance’s stochastic sequence.

### 5.3 One Random Advance Per Base Sample

SWARM must advance the PRNG **once per base-rate sample**, not:

- once per channel;
- once per polarity;
- once per oversampled lane;
- once per transition path.

The generated state is expanded into a four-lane chaos frame and reused everywhere for that base sample.

```cpp
struct SwarmFrame {
    float lanes[kOversampleFactor] {};
};
```

### 5.4 Band-Limited Lane Expansion

Do not generate four independent random values for the four oversampled lanes.

Maintain:

```cpp
float swarmPreviousFast;
float swarmCurrentFast;
float swarmSlow;
float swarmSlowCoefficient;
```

For each required base-rate SWARM frame:

1. Move `swarmCurrentFast` to `swarmPreviousFast`.
2. Generate one new signed random target into `swarmCurrentFast`.
3. Copy `swarmSlow` into a local `previousSlow` before modifying it.
4. Update `swarmSlow` toward `swarmCurrentFast` using a one-pole filter.
5. Linearly interpolate both the fast and slow values across the four oversampled lanes.
6. Blend slow and fast chaos using an amount-dependent coefficient prepared outside the per-lane kernel.

Recommended slow time constant:

```text
3 ms
```

Recommended lane interpolation:

```cpp
static constexpr float laneT[kOversampleFactor] = {
    0.25f, 0.50f, 0.75f, 1.f
};

t = laneT[i];
fastLane = lerp(previousFast, currentFast, t);
slowLane = lerp(previousSlow, currentSlow, t);
chaosLane = lerp(slowLane, fastLane, fastMix);
```

Use compile-time lane fractions (or an equivalent unrolled form) so frame
preparation does not introduce a division into the audio hot path.

`fastMix` is prepared from the current amount:

```cpp
fastMix = amount * amount;
```

The implementation may store fast and slow lane arrays separately or directly store the final blended lanes. Prefer the representation with the fewest repeated operations during dual-path transitions.

### 5.5 Conditional Activation

Do not advance or prepare SWARM randomness when SWARM is irrelevant.

A SWARM frame is required when any character currently contributing to output is SWARM:

- current negative character;
- current positive character;
- transition-from negative or positive character;
- transition-to negative or positive character.

Pending characters that are not yet contributing do not require chaos generation.

When no contributing character is SWARM:

- do not advance the SWARM PRNG;
- do not update SWARM interpolation state;
- pass a zero/default frame or bypass the argument entirely.

This ensures existing characters retain effectively unchanged steady-state cost.

---

## 6. SWARM Coefficients

Extend `CharacterCoefficients` with:

```cpp
float swarmDrive = 1.f;
float swarmScatter = 0.f;
float swarmRailAttraction = 0.f;
float swarmFastMix = 0.f;
float swarmOutputGain = 1.f;
```

Prepare them once per base-rate path configuration:

```cpp
const float a2 = a * a;

swarmDrive = 1.f + 4.f * a2;
swarmScatter = 0.05f * a + 0.55f * a2;
swarmRailAttraction = 0.65f * a2;
swarmFastMix = a2;
swarmOutputGain = 1.f / max(tanhAudio(swarmDrive), 1e-6f);
```

These are initial tuning constants, not immutable doctrine. They may be adjusted during listening tests, but the following structural relationships must remain:

- drive rises nonlinearly with `PUFF`;
- scatter is small in the first half and substantial near maximum;
- rail attraction rises approximately quadratically;
- faster chaos dominates as `PUFF` approaches maximum.

Do not calculate these coefficients separately for every oversampled lane.

---

## 7. SWARM Transfer Function

### 7.1 Reference Kernel

Extend the character application function to accept a chaos value:

```cpp
static float applyCharacter(
    float input,
    const CharacterCoefficients& coefficients,
    float swarmChaos);
```

Non-SWARM characters ignore `swarmChaos`.

The public test/preview helper should accept an optional deterministic chaos argument:

```cpp
static float processCharacter(
    Character character,
    float input,
    float amount,
    const DynamicsState& dynamics,
    float swarmChaos = 0.f);
```

Reference SWARM kernel:

```cpp
case Character::Swarm: {
    const float chaos = std::max(-1.f, std::min(swarmChaos, 1.f));

    const float localDriveScale =
        std::max(0.35f, 1.f + coefficients.swarmScatter * chaos);
    const float z = coefficients.swarmDrive * localDriveScale * input;

    float saturated = tanhAudio(z) * coefficients.swarmOutputGain;
    saturated = std::max(-1.f, std::min(saturated, 1.f));
    const float railScatterGate = smoothstep01(
        (std::fabs(z) - 1.f) * 2.f);

    const float randomUnit = 0.5f + 0.5f * chaos;
    const float railScatter = railScatterGate
        * 0.24f * coefficients.swarmScatter * (1.f - randomUnit);
    saturated *= 1.f - railScatter;

    const float magnitude = std::min(std::fabs(saturated), 1.f);
    const float attraction =
        coefficients.swarmRailAttraction
        * magnitude * magnitude
        * randomUnit;

    const float rail = std::copysign(1.f, input);
    saturated += (rail - saturated) * attraction;
    saturated = std::max(-1.f, std::min(saturated, 1.f));

    return input + (saturated - input) * coefficients.amount;
}
```

### 7.2 Behavioral Interpretation

The underlying deterministic contour is BLOOM's normalized smooth tanh curve.
SWARM then layers two stochastic mechanisms around that centerline:

1. **Drive scatter** changes the instantaneous knee and how quickly a sample approaches clipping.
2. **Rail attraction** probabilistically gathers already-active samples toward the appropriate rail.

The `magnitude²` weighting prevents low-level material from being dragged aggressively toward the rail.

Once the local drive reaches the rail region, ease in a small inward-facing rail
scatter so all chaos realizations do not collapse onto the exact same `+/-1`
value. The transition spans `1 < |z| < 1.5`, avoiding a discontinuity in both
the audio transfer and the representative preview centerline:

```cpp
railScatterGate = smoothstep01((abs(z) - 1) * 2);
railScatter = railScatterGate
    * 0.24f * coefficients.swarmScatter * (1.f - randomUnit);
saturated *= 1.f - railScatter;
```

Apply rail attraction after this inset. This keeps the result bounded, odd for
matching chaos, and concentrated near its polarity rail while allowing the
particulate distribution and preview cloud to continue through the clipped
region.

The final amount interpolation preserves Puffy’s established control behavior and guarantees exact identity when `amount == 0`.

### 7.3 Silence and Noise-Floor Contract

SWARM must not produce an output when the input is exactly zero.

This is a strict test requirement:

```text
for all amount and chaos:
processCharacter(Swarm, 0, amount, ..., chaos) == 0
```

After filter histories settle, processing digital silence through the complete engine with SWARM selected must return to the same silence tolerance used by existing Puffy tests.

### 7.4 Stereo Contract

Use the same `SwarmFrame` lane for:

- left channel;
- right channel;
- negative polarity;
- positive polarity;
- old transition path;
- new transition path.

This preserves a stable stereo image and prevents a character transition from crossfading between unrelated noise realizations.

Do not add left/right decorrelation in this feature.

### 7.5 Transition Contract

Puffy’s existing 5 ms character transition remains authoritative.

During a transition involving SWARM:

- generate one shared frame per base-rate sample;
- process both old and new paths with that same frame;
- let non-SWARM paths ignore it;
- retain existing path-state and decimator behavior;
- do not reset the PRNG merely because a transition begins or is retargeted.

At `PUFF = 0`, switching to or from SWARM must remain transparent within the existing transition test tolerance.

---

## 8. Auto Deflate

Add SWARM to `autoDeflateDb()`.

Initial value:

```cpp
case Character::Swarm:
    return -3.f * amount;
```

The final constant may be tuned between approximately `-2.5 dB` and `-4.0 dB` at full amount, but only after RMS and listening comparisons.

Acceptance target with Auto Deflate enabled:

- SWARM should not create a gross loudness jump relative to FRENZY and RIPTIDE at matched input and amount.
- Do not tune solely from peak level because stochastic crest density changes perceived loudness.
- Do not add envelope-following gain compensation.

---

## 9. UI Integration

### 9.1 Character Labels and Cycling

Add `"SWARM"` to every character label array.

Linked-character cycling must wrap:

```text
SWARM -> BLOOM
```

Use `kCharacterCount` for wrap logic.

### 9.2 Palette

Append white as the SWARM identity color:

```cpp
nvgRGB(255, 255, 255)
```

The final art pass may move this slightly off-white, for example toward a very
subtle warm or cool neutral, but it must still read immediately as white rather
than gray, cyan, green, or another saturated character color.

Use the same SWARM color consistently for:

- Puffy's negative and positive body tint contributions;
- the corresponding transfer-preview center line and point cloud;
- the `SWARM` character readout text.

Do not introduce a separate cyan-green SWARM accent. White must remain visually distinct from:

- BLOOM green;
- SPINE gold;
- FRENZY coral;
- RIPTIDE blue;
- VOID violet.

`weightedCharacterTint()` should require no structural change beyond the expanded array size.

### 9.3 Fish Animation

Do not add a mode-specific particle system in this implementation.

The current character controller should continue to use one shared motion language across all characters. SWARM receives its identity through:

- body tint;
- transfer-preview cloud;
- audio behavior.

This avoids new per-frame geometry and preserves the existing controller invariants.

---

## 10. Transfer Preview

### 10.1 Visual Contract

When a polarity uses SWARM, that half of the transfer display should show:

- a faint central transfer curve;
- a bounded cloud of points around it;
- increasing vertical spread as `PUFF` rises;
- increasing clustering near the corresponding saturation rail;
- no implication that the dots are literal current audio samples.

When only one polarity uses SWARM, only that side becomes particulate.

### 10.2 Representative, Not Live

The preview must not consume the audio PRNG state and must not receive random values from the audio thread.

Use a deterministic integer hash based on:

- point index;
- cloud lane index;
- polarity;
- a fixed visual seed.

Convert the hash to `[-1, +1]` and pass it to `Engine::processCharacter(..., swarmChaos)`.

The cloud should remain stable for a given:

- widget size;
- character pair;
- quantized amount;
- any quantized FRENZY dynamics values relevant to the opposite polarity.

A stable display is acceptable and preferred over continuous shimmer.

### 10.3 Cached Geometry

Retain the existing `FramebufferWidget` for the transfer curve.

Recommended cached data:

```cpp
static constexpr int POINT_COUNT = 129;
static constexpr int SWARM_COLUMN_COUNT = 65;
static constexpr int SWARM_SAMPLES_PER_COLUMN = 3;
```

Maximum SWARM point count:

```text
65 columns × 3 points = 195 points
```

For each SWARM column:

1. Choose an input X position over the relevant polarity half.
2. Generate three deterministic chaos samples.
3. Evaluate the SWARM transfer helper.
4. Store the resulting point positions.

Draw all cloud circles in one NanoVG path and fill once per tint where practical. Do not add per-point shadows, gradients, strokes, blur, or glow.

Use the SWARM palette color for both its cloud and its central transfer line.
When only one polarity is SWARM, the opposite half retains its own character
color.

The existing 129-point central line may remain.

### 10.4 Redraw Policy

In a stable state, the cached curve framebuffer must not be dirtied.

Rebuild when:

- widget size changes materially;
- either character changes;
- the quantized amount changes;
- existing FRENZY activity thresholds require a rebuild.

For SWARM, quantize the visual amount to 64 bins:

```cpp
visualAmountBin = clamp(int(amount * 63.f + 0.5f), 0, 63);
representativeAmount = float(visualAmountBin) / 63.f;
```

Use `representativeAmount` for the SWARM cloud and for the cached central curve.
This keeps the cloud centered on the line that is actually displayed.

Throttle SWARM-related framebuffer rebuilds to a maximum of:

```text
30 Hz
```

This cap applies during rapid knob movement or audio-rate CV activity visible to the UI. It does not alter audio.

When either polarity is SWARM, the 30 Hz cap applies to the entire cached curve
framebuffer, not only to cloud generation. Coalesce amount changes and any
FRENZY dynamics changes on the opposite polarity into the next permitted
rebuild. This avoids an uncapped non-SWARM half continuously dirtying the same
framebuffer behind a nominally throttled SWARM cloud. When neither polarity is
SWARM, retain the existing redraw behavior.

The uncached activity fill, axes, and overrange flashes may continue to draw normally.

### 10.5 Visual Compromise Rule

If the point cloud causes measurable UI cost, simplify in this order:

1. Reduce SWARM samples per column from 3 to 2.
2. Reduce columns from 65 to 49.
3. Draw tiny axis-aligned rectangles instead of circles.
4. Remove the central SWARM line only if the cloud remains readable.
5. Reduce SWARM rebuild rate from 30 Hz to 20 Hz, then 12 Hz if necessary.

Do not respond to UI cost by moving visualization work onto the audio thread.

---

## 11. File-Level Implementation Plan

### `src/PuffyEngine.hpp`

- Append `Character::Swarm`.
- Update `kCharacterCount`.
- Add SWARM coefficient fields.
- Add PRNG and interpolation state.
- Add `SwarmFrame`.
- Add `setSwarmSeed()`.
- Add optional `swarmChaos` argument to `processCharacter()`.
- Extend `applyCharacter()` and `processPath()` signatures as needed.

### `src/PuffyEngine.cpp`

- Update `clampCharacter()`.
- Add xorshift helper.
- Add seed sanitization.
- Calculate SWARM slow coefficient in `setSampleRate()`.
- Reset SWARM state in `reset()`.
- Prepare SWARM coefficients in `prepareCharacter()`.
- Implement SWARM in `applyCharacter()`.
- Add SWARM Auto Deflate.
- Detect whether a SWARM frame is required.
- Generate at most one chaos frame per base-rate sample.
- Reuse it across channels and transition paths.

### `src/Puffy.cpp`

- Expand both `configSwitch()` ranges and labels.
- Replace upper clamps with `kCharacterCount - 1`.
- Seed the engine once during module construction.
- Do not publish audio random state to `PuffyVisualState`.

### `src/PuffyWidget.cpp`

- Add the readout label.
- Update clamps and wraparound.
- Use `kCharacterCount` rather than `Character::Void` as the upper bound.

### `src/PuffyVisualPalette.hpp`

- Append the white or near-white SWARM tint and use it as the shared source for
  body tinting, preview geometry, and readout text.

### `src/PuffyPose.hpp` and `src/PuffyCharacterController.hpp`

- Keep tint-weight storage sized by `puffy::kCharacterCount`.
- Replace the five-value tint-weight initializers with `{1.f}` so BLOOM remains
  the default and every appended character is zero-initialized without encoding
  the old character count.

### `src/PuffyTransferPreviewWidget.hpp`

- Add cached SWARM point storage.
- Add last amount-bin state.
- Add redraw-throttle accumulator/state.
- Keep storage fixed-size and allocation-free after construction.

### `src/PuffyTransferPreviewWidget.cpp`

- Add deterministic visual hash.
- Build the representative cloud only for SWARM polarities.
- Retain framebuffer caching.
- Add amount quantization and 30 Hz SWARM rebuild cap.
- Batch point drawing.

### `src/PuffyCharacterController.*`

No behavioral changes expected beyond the initializer cleanup above.

The existing tests iterate over `kCharacterCount`, so adding SWARM may expose assumptions that every character has identical motion. Preserve those invariants.

### `tests/puffy_engine_spec.cpp`

Extend existing tests and add SWARM-specific cases.

### Documentation

Treat `doc/puffy-swarm-implementation-spec.md` as the canonical SWARM handoff.

Update `doc/puffy.md` where it states that Puffy has exactly five characters or uses `0..4` character ranges.

### 11.1 Recommended Implementation Sequence

1. **Character plumbing and kernel:** append the enum value, replace upper-bound
   sentinels, add palette/readout support, implement the deterministic helper
   kernel, and land kernel-invariant tests.
2. **Engine integration and tuning:** add seeding and shared frame generation,
   route one frame through stereo and transition paths, add full-engine tests,
   then tune SWARM and Auto Deflate against the listening and performance gates.
3. **Cached preview and documentation:** add the deterministic point cloud,
   mixed-mode rebuild throttling, render-performance checks, and the six-character
   updates to `doc/puffy.md`.

Each stage should build and pass the existing Puffy suites before the next stage
begins. Sonic tuning belongs after deterministic engine integration, not in the
preview implementation.

---

## 12. Automated Test Requirements

### 12.1 Kernel Invariants

Add a `swarmKernelInvariants()` test that sweeps:

```text
amount: 0..1
input: -4..4
chaos: -1..1
```

Verify:

- all outputs are finite;
- `amount == 0` returns input;
- zero input returns zero;
- same-chaos odd symmetry holds;
- output remains between `min(input, -1)` and `max(input, +1)`;
- fully wet shaped output is bounded to `[-1, +1]` for normalized input inside `[-1, +1]`.

### 12.2 Chaos Has Audible Influence

At moderate and high `PUFF`, evaluate one active input across several chaos values.

Require:

- the maximum output minus minimum output exceeds a small threshold;
- the spread at `PUFF = 1` exceeds the spread at `PUFF = 0.25`;
- the spread at zero input remains exactly zero.

### 12.3 Statistical Rail Attraction

Using a deterministic sequence of at least 4096 chaos values:

- process a representative positive input such as `+0.45`;
- process the matching negative input with the same chaos values;
- compare mean absolute output at low, medium, and high `PUFF`.

Require the high-amount distribution to sit closer to its polarity rail than the low-amount distribution by a meaningful margin.

The test should verify the design tendency, not exact sample values.

### 12.4 Deterministic Seeding

Create two engines with the same seed and one with a different seed.

With identical input and controls:

- same-seed engines must produce sample-identical outputs;
- different-seed engines must diverge after SWARM becomes active;
- all three must remain finite and bounded.

After advancing an engine, call `setSwarmSeed()` again with the original seed
and verify that its next SWARM sequence matches a freshly seeded engine. Repeat
the check through `reset()`. These checks cover the fast and slow interpolation
history as well as the integer PRNG state.

### 12.5 Stereo Stability

With identical left and right inputs and SWARM active:

```text
max(abs(outL - outR)) <= 1e-6
```

Run this through steady state and through transitions into and out of SWARM.

### 12.6 Silence Recovery

Extend the existing rapid-character and silence test so `kCharacterCount` includes SWARM.

After settling:

```text
tail < 1e-6
```

No stochastic noise floor may remain.

### 12.7 Transition Safety

Exercise:

- BLOOM -> SWARM;
- SWARM -> VOID;
- SWARM -> FRENZY;
- rapid retargets while SWARM is one of the pending or active characters;
- linked and split-polarity configurations.

Verify:

- finite output;
- no sample exceeds Puffy’s limiter ceiling;
- no transient discontinuity beyond the established character-transition envelope;
- zero-PUFF transitions remain transparent.

### 12.8 No Allocation

The existing allocation tracker must continue to report:

```text
allocations == 0
```

Ensure the randomized character sweep reaches SWARM.

### 12.9 Existing Regression Suite

All existing Puffy engine and character-controller tests must pass without weakened tolerances, except where a hard-coded final character value is correctly updated from `VOID` to `SWARM`.

---

## 13. Performance Acceptance

### 13.1 Audio Benchmark Method

Add or use a standalone microbenchmark that:

- runs release-optimized code;
- disables debug terminal timing during the measured loop;
- warms the engine before measurement;
- measures at 48 kHz, 96 kHz, and 192 kHz;
- uses 4x oversampling;
- measures at least:
  - BLOOM steady state;
  - SPINE steady state;
  - FRENZY steady state;
  - SWARM steady state;
  - a transition involving SWARM;
  - a non-SWARM mode after the feature is compiled in.

Compare medians across multiple runs on the same machine.

### 13.2 Audio Performance Gates

Required:

- No dynamic allocation in the real-time path.
- No lock, atomic increment, logging, or file access added to `puffy::Engine::process()`.
- Non-SWARM steady-state performance regression must remain within benchmark noise, with a target of less than `2%`.
- SWARM should require no more than one PRNG advance per base-rate sample.
- SWARM steady-state target:
  - no slower than `1.25x` SPINE; and
  - no slower than `1.10x` FRENZY.
- A transition involving SWARM may retain the cost of Puffy’s existing dual-path transition, but must not generate separate random streams for each path.

If SWARM misses the target, optimize in this order:

1. Precompute all amount-dependent values.
2. Store one final chaos value per oversample lane.
3. Remove redundant clamps proven unnecessary by bounded construction.
4. Replace `copysign` with a branch or bit operation only if measurement shows a benefit.
5. Adjust the soft-clip polynomial only after an audio null/listening comparison.

Do not reduce oversampling specifically for SWARM unless a separate design decision is made for every character.

### 13.3 Render Performance Gates

Required:

- Stable SWARM preview causes zero framebuffer rebuilds after state settles.
- SWARM framebuffer rebuild rate never exceeds 30 Hz.
- Mixed SWARM/FRENZY preview rebuilds remain capped at 30 Hz for the shared
  framebuffer while SWARM is selected on either polarity.
- The cloud contains no more than 195 points in the initial implementation.
- Cloud points are batched into one or a small number of NanoVG fill calls.
- No blur, shadow, per-point gradient, texture upload, or new image load occurs during redraw.
- No full-panel framebuffer is dirtied by the SWARM preview.
- The fish controller remains capped at its existing 30 Hz update behavior.
- Hidden widgets do not perform SWARM-specific animation work.

A subjective visual reduction is acceptable if needed to meet these gates. Audio behavior is not to be simplified merely to make the preview exact.

---

## 14. Listening and Manual Acceptance

Test at minimum with:

- sine waves at low and high levels;
- bass;
- kick drum;
- drum bus;
- bright percussion;
- sustained pad;
- full mix;
- mono material duplicated to stereo;
- unrelated stereo material.

Listen at `PUFF` values around:

```text
0.10, 0.25, 0.50, 0.75, 1.00
```

Confirm:

- no audible noise during digital silence;
- low `PUFF` adds texture without obvious hiss;
- medium `PUFF` sounds particulate rather than merely clipped;
- high `PUFF` gathers peaks toward the rails;
- stereo center remains stable;
- no zipper or click occurs during character changes;
- Auto Deflate gives a useful comparison level;
- the sound remains distinct from SPINE, FRENZY, RIPTIDE, and VOID.

The mode should be rejected or retuned if it sounds primarily like:

- white noise mixed over the signal;
- conventional bit crushing;
- simple amplitude modulation;
- a duplicate of SPINE with hiss;
- uncontrolled stereo widening.

---

## 15. Completion Checklist

- [ ] `Character::Swarm = 5` appended.
- [ ] `kCharacterCount` updated.
- [ ] All character clamps use the new count.
- [ ] Both switches expose `SWARM`.
- [ ] Linked cycling wraps correctly.
- [ ] White or near-white palette color is used by Puffy, preview, and readout text.
- [ ] Tint-weight initializers no longer encode a five-character array.
- [ ] Seedable xorshift state added.
- [ ] Reseeding and reset clear all SWARM interpolation history.
- [ ] One chaos frame generated per required base-rate sample.
- [ ] Slow interpolation captures `previousSlow` before the one-pole update.
- [ ] Chaos frame shared across stereo and transition paths.
- [ ] No random work occurs when SWARM is not contributing.
- [ ] SWARM kernel contains no transcendental math or division.
- [ ] Zero input produces zero output.
- [ ] Auto Deflate case added.
- [ ] Representative cached cloud implemented.
- [ ] Stable preview does not continuously redraw.
- [ ] SWARM redraw capped at 30 Hz.
- [ ] Mixed SWARM/FRENZY redraws obey the shared-framebuffer cap.
- [ ] No fish particle system added.
- [ ] Engine tests extended.
- [ ] No-allocation test passes.
- [ ] Character-controller tests pass.
- [ ] Performance benchmark meets gates or deviations are documented.
- [ ] Existing patches load with unchanged character selections.
- [ ] `doc/puffy.md` updated for six characters.

---

## 16. Final Design Statement

SWARM is a bounded stochastic saturation field.

It should feel as though the transfer function is made from a population of microscopic nonlinear events: individually unstable, collectively coherent, and increasingly drawn toward the rails as Puffy inflates.

The implementation must preserve that identity without paying for chaos where it is not used and without asking the renderer to reenact every particle of the audio process.
