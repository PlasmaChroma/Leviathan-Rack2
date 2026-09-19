Dragon King Leviathan — yes. The clip does **not** read like a plain sine-to-triangle crossfade. The closer model is:

**triangle core → sine shaper → single-ended lower-lobe wavefolder → mild triangle imprint / edge tightening**

The current **Legacy/Geometric** path is closer because it already targets the lower trough, but it is still “drawing a bump by phase window.” The STO-like behavior looks more like a **threshold fold of the lower half**: once the sine/triangle core drops below a moving threshold, that part of the wave is reflected upward. That naturally creates the little bottom ripple, then the W/notched trough, then the sharper folded-triangle/shark-fin shape. Your current code crossfades toward triangle very aggressively with `triAmt = 0.99 * smooth01(shape / 0.48)` and then adds a local trough lift; the nonlinear path globally overdrives/biases the whole wave, which is why it feels less STO-like. 

## What I see in the video/audio

The sweep appears to pass through these states:

1. **Low SHAPE**: nearly pure sine, maybe slightly triangle-influenced.
2. **Early/mid SHAPE**: the upper lobe remains smooth, but the lower trough starts developing a tiny kink/ripple.
3. **Mid SHAPE**: the trough splits into a clear **W**: two side minima with a lifted inner valley.
4. **High SHAPE**: the lower fold becomes sharp; the waveform takes on a folded triangular geometry with steep shoulders.
5. **Not sub-octave mix**: I do not see reliable every-other-cycle alternation, so I would not mix the SUB output into SHAPE. The separate sub flip/output path in Undertow is already its own thing. 

## Recommended DSP model

Replace the “phase-window trough bump” with a **value-threshold lower fold**.

Conceptually:

```text
tri core phase
  ↓
triangle signal
  ↓
triangle-to-sine approximation
  ↓
small triangle imprint, mostly at higher Shape
  ↓
if signal falls below threshold:
    reflect the below-threshold portion upward
  ↓
soft edge / static gain compensation
```

The important move is this:

```cpp
under = max(threshold - core, 0)
y = core + foldGain * under
```

With `foldGain ≈ 2`, that behaves like a wavefolder reflecting the part below `threshold`. Use a soft knee so the fold does not alias harshly or look too digital.

## Drop-in candidate

I would test this as a replacement for `undertowStoShape()` while leaving the context-menu A/B in place for now. Your enum/menu structure already supports keeping both algorithms during tuning.  

```cpp
inline float softPositiveKnee(float x, float knee) {
  knee = std::max(knee, 1e-5f);
  if (x <= 0.f) {
    return 0.f;
  }
  if (x >= knee) {
    return x - 0.5f * knee;
  }
  return 0.5f * x * x / knee;
}

inline float undertowStoShape(float phase, float shape) {
  const float p = phase - std::floor(phase);

  // Triangle core, same phase convention as current Undertow:
  // p = 0 / 1 is high, p = 0.5 is low.
  const float tri = 4.f * std::fabs(p - 0.5f) - 1.f;
  const float sine = triToSine(tri);

  // Keep the external control taper, then smooth again for internal stages.
  const float u = smooth01(shape);

  // The STO-like motion is not primarily a global sine->triangle crossfade.
  // Keep triangle imprint modest so the upper lobe stays rounded through most
  // of the sweep.
  const float triAmt = 0.035f * u + 0.30f * u * u;
  float core = crossfade(sine, tri, triAmt);

  // Single-ended lower-lobe fold.
  // At low Shape the threshold sits almost at the sine minimum, producing only
  // a tiny bottom ripple. At high Shape it rises into the lower half, producing
  // the visible W / inner-fold geometry.
  const float fold = smooth01((u - 0.07f) / 0.86f);
  const float foldThreshold = -0.995f + 0.58f * fold; // about -1.00 -> -0.42
  const float knee = 0.115f - 0.075f * fold;          // softer early, sharper late

  const float under = softPositiveKnee(foldThreshold - core, knee);
  float y = core + (2.05f + 0.55f * fold) * fold * under;

  // Small analog-ish asymmetry / shoulder lean. This helps the high-shape trace
  // avoid looking like a mathematically perfect symmetric W.
  const float edge = smooth01((u - 0.46f) / 0.50f);
  const float zeroCrossWindow = std::max(0.f, 1.f - std::fabs(tri));
  y += 0.055f * edge * std::sin(2.f * float(M_PI) * p) * zeroCrossWindow;

  // Gentle one-sided diode-ish compression. Keep this subtle; clipping should
  // not be the main tone source.
  const float posDrive = 0.06f * u + 0.06f * edge;
  const float negDrive = 0.025f * u;
  if (y >= 0.f) {
    y = y / (1.f + posDrive * y);
  }
  else {
    y = y / (1.f + negDrive * -y);
  }

  // Static compensation for the DC lift introduced by lower folding.
  // Avoid an actual HP/DC blocker here; that would make the waveform history-
  // dependent during Shape CV sweeps.
  y -= 0.055f * fold;
  y *= 1.f + 0.10f * u;

  return clamp(y, -1.f, 1.f);
}
```

## Tuning targets

The three constants I would tune first:

```cpp
const float foldThreshold = -0.995f + 0.58f * fold;
const float triAmt = 0.035f * u + 0.30f * u * u;
float y = core + (2.05f + 0.55f * fold) * fold * under;
```

Use these rules:

| If Undertow sounds/looks like…         | Change                                                            |
| -------------------------------------- | ----------------------------------------------------------------- |
| Too triangular too early               | Lower `triAmt`, especially the `0.30f * u * u` term               |
| The W appears too late                 | Increase `0.58f` threshold travel or lower the `0.07f` fold start |
| The W is too smooth / not enough spike | Increase fold gain or reduce late `knee`                          |
| The bottom fold feels painted-on       | Reduce phase/asymmetry term; let threshold folding dominate       |
| Too much DC shift                      | Increase the `y -= 0.055f * fold` compensation slightly           |

My strongest recommendation: **do not make the primary morph a crossfade to triangle.** Let the triangle core inform the edges, but let the **lower threshold fold** create the actual STO-ish magic. That is the little dragon hiding under the sine wave.
