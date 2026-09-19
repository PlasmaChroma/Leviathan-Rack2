Dragon King Leviathan — looking at the STO scope shots, I would aim Undertow’s SHAPE output less toward “freeform asymmetric warped triangle” and more toward **one specific family**:

**sine → slightly sharpened sine → triangle-like contour → triangle with a lifted/folded lower trough and mild half-cycle lean.**

The important visual clue is that the STO shape does **not** look like a clean sine-to-triangle crossfade. It looks like the sine is being pulled into a triangle, but the lower half develops that little **W / inner-fold / pinched valley** character. The upper lobe stays comparatively smooth and rounded. That should be the signature.

Your current code is close in spirit, but I’d change the model. Right now the main SHAPE path uses `warpedPhase`, `triWarp`, `halfSign`, `edge2`, `skew`, and `kink`, then crossfades against the sine and applies DC blocking, trim, saturation, and balance.  The biggest issue is the `halfSign * edge2` style term: because `halfSign` flips while `edge2` is strongest at the triangle extrema, this can create actual step discontinuities at the wrap and trough. That will make vertical jumps / aliasy edges rather than the analog-looking bent triangle from the screenshots.

A second issue: the pre-sync “old state” shape calculation is not the same as the main SHAPE calculation. It uses an older `asymBefore / kinkBefore / creaseBefore` formula, while the main output uses the newer `warpedPhase / triWarp / skew / kink` formula.  That means sync MinBLEP correction is not correcting the actual waveform being emitted. I’d centralize the shape function and call the same function for both old and new phase states.

## Recommended behavior for the SHAPE knob

I’d define the knob like this:

| Shape range | Visual/audible behavior                                                                                                       |
| ----------: | ----------------------------------------------------------------------------------------------------------------------------- |
|      `0.00` | Exact sine, same phase and amplitude as SINE output.                                                                          |
| `0.10–0.30` | Sine becomes subtly more triangular; peaks sharpen but stay smooth.                                                           |
| `0.30–0.65` | Main STO-ish character appears: lower trough starts lifting/folding into a W-like bend.                                       |
| `0.65–1.00` | Strong distorted triangle: angular slopes, rounded/saturated top, folded/pinched lower valley, but **no hard discontinuity**. |

The core should be **continuous in phase**. If you want the shape to be bright, give it corners and curvature changes; do not give it un-BLEPed value jumps.

## Replace the current SHAPE block with a single evaluator

Something like this is the direction I’d take:

```cpp
inline float smooth01(float x) {
  x = clamp(x, 0.f, 1.f);
  return x * x * (3.f - 2.f * x);
}

inline float undertowStoShape(float phase, float shape) {
  const float p = phase - std::floor(phase);

  // Same phase convention as the current code:
  // +1 at phase 0/1, -1 at phase 0.5.
  const float tri = 4.f * std::fabs(p - 0.5f) - 1.f;
  const float sine = triToSine(tri);

  // Use multiple internal curves instead of one global shapeMix.
  const float s = smooth01(shape);

  // Triangle influence arrives fairly early.
  const float triAmt = 0.82f * smooth01((shape - 0.02f) / 0.72f);

  // Fold/lift arrives later. This is the STO-ish "pinched lower trough."
  const float foldAmt = smooth01((shape - 0.22f) / 0.78f);

  // Mild lean/asymmetry arrives gradually.
  const float leanAmt = smooth01((shape - 0.12f) / 0.88f);

  float y = crossfade(sine, tri, triAmt);

  // Lift the center of the negative trough to create the W / inner-fold look.
  // This is smooth and continuous; no branch-sign discontinuity.
  const float midDist = std::fabs(p - 0.5f);
  const float troughWindow = 1.f - smooth01(midDist / 0.145f);
  y += 0.42f * foldAmt * troughWindow;

  // Continuous half-cycle lean, strongest around zero crossings.
  // sin() is zero at phase 0, 0.5, and 1.0, so it does not step at extrema.
  const float zeroCrossWindow = 1.f - std::fabs(tri);
  y += 0.16f * leanAmt * std::sin(2.f * float(M_PI) * p) * zeroCrossWindow;

  // Gentle analog-ish saturation/rounding, not hard clipping.
  const float drive = 0.16f * s;
  y = y / (1.f + drive * std::fabs(y));

  // Static recentering/gain compensation for the trough lift.
  // Tune by eye/scope, but keep it static — no follower normalization.
  y -= 0.10f * foldAmt;
  y *= 1.f + 0.06f * s;

  return clamp(y, -1.f, 1.f);
}
```

Then the main output path becomes much simpler:

```cpp
const float tri = 4.f * std::fabs(voice.phase - 0.5f) - 1.f;
const float sine = triToSine(tri);

float shape = params[SHAPE_PARAM].getValue();
if (inputs[SHAPE_CV_INPUT].isConnected()) {
  // I would strongly consider additive CV here instead of multiplicative CV.
  shape += inputs[SHAPE_CV_INPUT].getVoltage() / 8.f;
}
shape = clamp(shape, 0.f, 1.f);

const float shaped = undertowStoShape(voice.phase, shape);

outputs[SINE_OUTPUT].setVoltage(5.f * sine + voice.sineBlep.process());
outputs[SHAPE_OUTPUT].setVoltage(clamp(5.f * shaped + voice.shapeBlep.process(), -5.f, 5.f));
```

And for sync correction:

```cpp
const float shapedBeforeEvents = undertowStoShape(phaseBeforeEvents, shapeBeforeEvents);

// after events / phase advance:
const float shaped = undertowStoShape(voice.phase, shape);

if (syncRising) {
  const float sineStep = sine - sineBeforeEvents;
  const float shapeStep = shaped - shapedBeforeEvents;
  insertBlepStep(&voice.sineBlep, sineStep * 5.f, syncDiscontinuityFrac);
  insertBlepStep(&voice.shapeBlep, shapeStep * 5.f, syncDiscontinuityFrac);
}
```

## I would remove or greatly weaken the SHAPE DC blocker

The current code runs the SHAPE signal through `dcBlockShape()` before output shaping.  For this particular oscillator shape, I’d avoid that unless there is a demonstrated DC problem. The screenshots look like a stable DC-coupled oscillator waveform, not a history-dependent high-passed signal.

Because your DC blocker coefficient is `0.9993`, at 48 kHz its cutoff is in the low-Hz region. That can still matter at the bottom of your oscillator range and can make low-frequency shape tests look like they “lean” or recover differently. For a VCO shape output, I’d prefer **static recentering** inside the transfer function.

So: remove this path for SHAPE unless needed:

```cpp
const float shapedDc = dcBlockShape(shapedRaw, &voice);
```

and instead rely on static bias/gain compensation inside `undertowStoShape()`.

## Shape CV recommendation

Right now the code treats Shape CV as a multiplier:

```cpp
shape = knob * clamp(cv / 8.f, 0.f, 1.f);
```

That makes CV act like an attenuator: with the knob at 50%, no CV can ever push shape beyond 50%. That might be musically useful, but it is probably not what people expect from an STO-like shape CV.

For STO-ish behavior, I’d use additive unipolar CV:

```cpp
shape = params[SHAPE_PARAM].getValue();
if (inputs[SHAPE_CV_INPUT].isConnected()) {
  shape += inputs[SHAPE_CV_INPUT].getVoltage() / 8.f; // or /10.f
}
shape = clamp(shape, 0.f, 1.f);
```

If you want both modes, make it a context menu option: **Shape CV: Additive / Multiply**.

## Tuning constants by eye

The three constants to tune against your screenshots are:

```cpp
triAmt max       // 0.75–0.90
trough lift     // 0.30–0.55
trough width    // 0.10–0.18 phase
```

I’d start with:

```cpp
triAmt max    = 0.82
trough lift   = 0.42
trough width  = 0.145
lean amount   = 0.16
drive          = 0.16
bias           = 0.10
gain           = 0.06
```

The result should be less chaotic than the current `halfSign`/`edge2` endpoint, but closer to the visual grammar of the STO shots: **same phase spine, increasingly triangular body, lower fold blooms in as the knob rises**. In other words: not a monster trying to escape the sine, but a sine learning to grow teeth.
