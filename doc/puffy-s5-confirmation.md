Dragon King Leviathan, this is **excellent review feedback**. It is not bikeshedding; it identifies several places where two competent implementers could follow the specification and produce different behavior. That is exactly what should be eliminated before handing the module to Codex. 

I would accept nearly all of it, but one point—the `FRENZY` curve—is potentially more serious than the reviewer suggests.

## 1. I would change `DEFLATE`, not merely clarify it

The reviewer correctly observes that the present placement makes `DEFLATE` a **pre-limiter headroom control**, despite being described as output attenuation. When the limiter is active, turning up `DEFLATE` mostly reduces gain reduction rather than producing the promised output-level change. 

Given Puffy’s intended role—an end-of-chain module where the user can dial in the final level—I think manual `DEFLATE` should move **after** the limiter:

```text
character
-> DC blocker
-> Auto Deflate
-> limiter
-> manual DEFLATE
-> finite/output guard
```

That gives us clean semantics:

* **Auto Deflate** remains before the limiter because it compensates for character loudness.
* **The limiter** controls peaks produced by the saturation chain.
* **Manual DEFLATE** becomes a literal final output trim of 0 to −12 dB.

Because `DEFLATE` only attenuates, placing it after the limiter cannot violate the limiter ceiling. It simply lowers the effective final ceiling proportionally.

This also means the limiter’s behavior and Puffy’s limiter-reduction expression remain stable while the user adjusts final output volume. That seems much more coherent than watching Puffy stop “straining” merely because the output trim was turned down.

If we deliberately retain the current pre-limiter location, I would rename the control to something like `HEADROOM`, because it would not really be a dependable output trim.

## 2. The independent MASTER oversampler is fine

The reviewer is right that `8x + MASTER` runs both:

* the character path’s 8× oversampling;
* the limiter detector’s separate 4× reconstruction stage.

That worst-case combination should be explicitly named in the performance test. 

I would **not** try to reuse the character oversampler. Those samples exist before or around the nonlinear character processing, while the true-peak detector must inspect the final post-character, post-filter signal. Reuse would complicate the architecture and risk making the detector dependent on the selected character oversampling setting.

Add this explicitly:

```text
The 8x realtime target is measured with:
- saturation oversampling = 8x;
- limiter mode = MASTER;
- stereo signal active;
- visual snapshot publication active.
```

That pins down the actual maximum-cost configuration.

## 3. There should be exactly one dynamics detector

The ambiguity the reviewer identifies is real. The specification currently allows someone to interpret the `FRENZY` detector and visual transient detector as two separate objects. 

I would define one always-running stereo-linked object:

```cpp
struct PuffyDynamicsDetector {
    float fast;
    float slowSq;
    float transient;
};
```

It runs regardless of the selected character:

```text
Input stereo pair
        |
        v
PuffyDynamicsDetector
        |
        +--> FRENZY DSP controls, when FRENZY is selected
        |
        +--> Puffy visual telemetry, in every character
```

This has several benefits:

* Switching into `FRENZY` does not begin with an empty detector.
* Visual motion and audible behavior cannot drift apart.
* There is one set of attack/release constants.
* Tests can observe the same authoritative detector state.

The spec should say unambiguously:

> Puffy owns exactly one continuously updated stereo-linked dynamics detector. FRENZY reads it when active, and the visual snapshot reads it in every character mode.

## 4. The `FRENZY` concern is larger than a missing test

The reviewer notes that only the positive side is normalized and that the negative excursion is unconstrained. 

I ran the current formula at:

```text
amount = 1
fast = 1
transient = 1
```

It produces approximately:

```text
x = -0.10 -> s = -4.12 before clamping
x = -0.05 -> s = -1.46 before clamping
x = +0.10 -> s = +0.86
```

Thus, at full activity, much of even the modest negative half-cycle gets slammed against the `−1.25` clamp. That is substantially more severe than a gently asymmetrical saturation curve.

There is an additional issue the notes do not mention: `fast` is not explicitly bounded. Since the input safety clamp allows ±20 V and `p` is normalized against 5 V, `fast` can theoretically approach `4`, not merely `1`. At that point:

* `drive` becomes very large;
* `bias` becomes large;
* `positiveNorm` collapses toward its `1e-4` safety floor;
* the curve becomes almost entirely governed by the hard clamp.

I would make two changes.

First:

```text
fastControl = clamp(fast, 0, 1)
```

Use `fastControl` in the drive and bias formulas. The raw detector can remain larger if useful for metering, but the waveshaper control should have a defined domain.

Second, use branch-specific normalization:

```cpp
zero = tanhAudio(drive * bias);

raw = tanhAudio(drive * (x + bias)) - zero;

positiveNorm =
    max(tanhAudio(drive * (1.f + bias)) - zero, 1e-4f);

negativeNorm =
    max(zero - tanhAudio(drive * (-1.f + bias)), 1e-4f);

s = raw >= 0.f
    ? raw / positiveNorm
    : raw / negativeNorm;
```

This still creates an asymmetrical transfer curve because the biased positive and negative branches have different curvature and slopes, but it prevents the negative side from becoming accidental near-hard-clipping over most of its range.

A deliberate negative-side multiplier could then restore some aggression:

```text
negativeScale = 1 + 0.10 * amount * fastControl
```

That would give us controlled asymmetry rather than asymmetry emerging from a nearly collapsed normalization denominator.

I would plot and listen-test the transfer function at:

```text
amount:    0.25, 0.50, 0.75, 1.00
fast:      0.00, 0.50, 1.00
transient: 0.00, 1.00
```

This should be resolved before the algorithm is treated as authoritative.

## 5. Split the fish widget now

I strongly agree with the architecture feedback. The appendix has already established the conceptual separation, so keeping the panel, menus, controls, animation controller, and renderer in one translation unit would create technical debt immediately. 

I would revise the v1 files to:

```text
src/Puffy.hpp
src/Puffy.cpp

src/PuffyEngine.hpp
src/PuffyEngine.cpp

src/PuffyWidget.hpp
src/PuffyWidget.cpp

src/PuffyFishWidget.hpp
src/PuffyFishWidget.cpp

src/PuffyCharacterController.hpp
src/PuffyCharacterController.cpp
```

Responsibilities:

```text
PuffyWidget
- panel construction
- knobs, switches, ports and lights
- context menu
- module/browser-preview ownership

PuffyFishWidget
- visual snapshot consumption
- NanoVG rendering
- visibility and update-rate handling
- cached fish assets

PuffyCharacterController
- inflation spring
- breathing
- blinking
- gaze
- fins
- expression state
- renderer-independent PuffyPose
```

That makes a future `PuffyFishOpenGL.cpp` an alternate renderer rather than a rewrite of the panel implementation.

On framebuffer caching, I agree conditionally with the reviewer: use the shared helper **if it is genuinely generic and already available**.  I would not make Puffy depend on something carrying Integral Flux assumptions merely to avoid ten lines of local code. The correct outcome is one neutral cache helper, not cross-module coupling.

## 6. Structural transitions need one coordinator

The concurrency problem is absolutely real. 

I would go slightly beyond the suggested “defer the character crossfade” rule. Use one transition coordinator with this precedence:

```text
Structural transition
    > character crossfade
```

Behavior:

1. A normal character change performs the existing 10 ms crossfade.
2. An oversampling or limiter change begins the 5 ms fade to zero.
3. If a character change arrives during that fade, record only the latest requested character.
4. At zero output:

   * commit the latest oversampling factor;
   * commit the latest limiter mode;
   * commit the latest character directly;
   * reset the relevant DSP state once.
5. Fade up over 5 ms.
6. Do not run a character crossfade afterward—the character was changed while output was silent.

If a structural request arrives during an existing character crossfade, the structural fade takes control. There is no value in finishing a parallel character crossfade behind a signal that is already fading to silence.

This gives Puffy a single transactional reconfiguration point rather than several overlapping mini-state-machines.

The test should rapidly alternate all three settings and verify:

* finite output;
* no discontinuity;
* no stale delayed samples;
* final state equals the most recently requested state;
* no allocation or lock.

## 7. The smaller notes are worth incorporating

The fixed 4096-sample limiter storage should receive a comment explaining that it is intentional over-provisioning, not a forgotten magic number. The panel-region math needs no modification. 

## My priority order

**Before implementation starts:**

1. Move manual `DEFLATE` after the limiter.
2. Bound and revise the `FRENZY` transfer function.
3. Declare exactly one dynamics-detector instance.
4. Define the unified transition coordinator.
5. Split the fish/controller files from `PuffyWidget.cpp`.

**Before release testing:**

6. Name `8x + MASTER` as the performance worst case.
7. Add transfer-curve tests for `FRENZY`.
8. Add concurrent-transition stress tests.
9. Document the deliberately oversized limiter storage.
10. Reuse a neutral framebuffer helper where appropriate.

The reviewer’s summary is accurate: the foundation is strong, and the remaining problems are concentrated around control semantics, shared state, and boundaries.  The one place I would push further is `FRENZY`: I would treat that as a DSP correction rather than merely adding a test around the existing behavior.
