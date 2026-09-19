## Surgical patch to IntegralFlux.cpp

### 1) Add two small constants (near your other tunables)

Search near your outer constants (where `OUTER_V_MIN/MAX` live) and add:

```cpp
// How strongly Signal IN perturbs the running function core (Cycle/trigger mode).
// 0 = current behavior (no effect), 1 = strong hardware-like coupling.
static constexpr float OUTER_INJECT_GAIN = 0.55f;

// Time constant (seconds) for how fast the perturbation pulls the integrator state.
// Smaller = more audio-rate influence; larger = slower “bending”.
static constexpr float OUTER_INJECT_TAU = 0.0015f; // ~1.5 ms
```

These are intentionally conservative defaults. You can tune later.

---

### 2) In `processOuterChannel()`, compute `x_in` once when running

In your existing function-generator path:

```cpp
if (ch.phase != OUTER_IDLE) {
    // Function-generator integration path.
    float s = shapeSigned;
    float range = OUTER_V_MAX - OUTER_V_MIN;
    ...
```

Add this immediately after `range`:

```cpp
    // If Signal IN is patched, map it into the same 0..1 domain as x so it can perturb the core.
    float x_in = 0.f;
    float injectAlpha = 0.f;
    if (signalPatched) {
        float inV = inputs[cfg.signalInput].getVoltage();
        // Soft clamp to keep extreme patching from destabilizing the integrator.
        // (You already use softClamp8 elsewhere; it’s a good fit here too.)
        float inSoft = softClamp8(inV);
        x_in = clamp((inSoft - OUTER_V_MIN) / range, 0.f, 1.f);

        // One-pole “attraction” strength per-sample: alpha = 1 - exp(-dt/tau)
        float a = 1.f - std::exp(-dt / OUTER_INJECT_TAU);
        injectAlpha = OUTER_INJECT_GAIN * clamp(a, 0.f, 1.f);
    }
```

---

### 3) Apply the perturbation inside BOTH rise and fall updates

#### In the RISE section, you currently do:

```cpp
x += dp * slopeWarp(x, s) * scale;
x = clamp(x, 0.f, 1.f);
ch.out = OUTER_V_MIN + x * range;
```

Immediately after the `x += ...` line (and before clamping), inject:

```cpp
// Signal IN perturbation (hardware-ish): gently pull the integrator state toward input.
if (injectAlpha > 0.f) {
    x += injectAlpha * (x_in - x);
}
```

So it becomes:

```cpp
x += dp * slopeWarp(x, s) * scale;

if (injectAlpha > 0.f) {
    x += injectAlpha * (x_in - x);
}

x = clamp(x, 0.f, 1.f);
ch.out = OUTER_V_MIN + x * range;
```

#### Do the same in FALL, right after `x -= ...`:

```cpp
x -= dp * slopeWarp(x, s) * scale;

if (injectAlpha > 0.f) {
    x += injectAlpha * (x_in - x);
}

x = clamp(x, 0.f, 1.f);
ch.out = OUTER_V_MIN + x * range;
```

That’s it.

---

## What this will change in practice

### Before (current Integral Flux)

* **Cycle ON + IN patched:** input does basically nothing (sealed oscillator)
* You only “feel” Signal IN when `OUTER_IDLE` (slew mode)

### After (with the patch above)

* **Cycle ON + IN patched:** input now *bends* the running contour
* Slow CV in Signal IN → “warbly contour,” threshold biasing, asymmetry
* Audio in Signal IN → timbral grit / dirty FM-ish motion (in a controlled, bounded way)
* Still stable, still respects your Rise/Fall core (not replaced by “track input”)

This is much closer to the analog *experience* of Maths: the core is still the core, but you can push it while it runs.

---

## Two knobs you can tune (and what they do)

* `OUTER_INJECT_GAIN` (0..1-ish)

  * Higher = more dramatic bending / chaos
  * Too high = it starts to feel like “it’s tracking input” rather than being a generator you can perturb

* `OUTER_INJECT_TAU`

  * Smaller (e.g. 0.0007) = more audio-rate influence (edgier, dirtier)
  * Larger (e.g. 0.004) = more “slow bending,” less audio grit

If you want **closest-to-hardware**, I’d start around:

* gain: **0.45–0.65**
* tau: **1–3 ms**

---

## One extra hardware-ish refinement (optional, but very Maths)

On real Maths, the “injection” feels stronger near certain parts of the curve (because analog circuits are non-linear). If you want that flavor later, you can weight `injectAlpha` by a function of `x` (e.g., stronger near the midrange, weaker near rails). But don’t do that yet—first get the core perturbation feeling right.

---

If you want, I can also suggest a tiny “scope test suite” patch (three inputs: DC offset, slow sine, audio) that will let you *visually* compare:

1. Maths hardware
2. Integral Flux before
3. Integral Flux after this injection patch

…so you can tune gain/tau with confidence.
