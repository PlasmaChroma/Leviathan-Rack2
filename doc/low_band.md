# Bifurx Low + Band Dip Notes

## Context

`shape1.png` showed Bifurx mode 2 in the UI, which maps to mode index `1`
in code: `Low + Band`.

The reported case was around:

- `SPAN`: about `16.5 st`
- `RESO`: about `0.4795`
- Marker frequencies in the screenshot: about `255 Hz` and `659 Hz`

At this setting the expected response developed a very sharp, narrow dip
between the markers. Neighboring spans produced less extreme dips, which made
the shape feel like an unlucky cancellation point rather than an intentional
mode character.

The relevant combiner in `src/Bifurx.hpp` is:

```cpp
case 1:
	return T(0.92f) * T(wA) * lpA
		+ T(1.18f) * T(wB) * bpB
		- T(0.16f) * (bpA + bpB);
```

The same shape is mirrored in `tests/bifurx_filter_test_model.hpp`.

## What The Sweep Showed

A quick preview-response sweep at the reported resonance showed the current
`0.16` subtractive band term producing roughly this pattern:

- `12 st`: about `-14 dB`
- `14 st`: about `-20 dB`
- `16 st`: about `-34 dB`
- `16.5 st`: about `-58 dB`
- `17 st`: about `-36 dB`
- `18 st`: about `-27 dB`
- `20 st`: about `-20 dB`
- `24 st`: about `-15 dB`

Reducing the subtractive term to `0.08` made the original `16.5 st` case less
extreme, but it mostly moved the deepest null to a wider span:

- `16.5 st`: about `-22 dB`
- `18 st`: about `-32 dB`
- `19 st`: about `-44 dB`
- `20 st`: about `-30 dB`

That means a coefficient-only tweak is fragile. It changes where the destructive
phase alignment happens instead of removing the underlying condition.

## Root Cause

`Low + Band` is a linear sum of phase-shifted filter outputs:

- lowpass from stage A
- bandpass from stage B
- a subtractive band-sculpt term from both stages

At certain frequency/span/resonance combinations, those complex response
vectors nearly cancel. Because this is vector cancellation, changing one gain
coefficient can move the zero/null to a nearby span rather than eliminating it.

This is why "just cap the dip" is not straightforward in the audio path. A true
audio cap would be nonlinear or signal-dependent unless the mode topology is
changed.

## Strategic Options

### 1. Visual-Only Cap

Apply a floor only to the expected response drawing, while leaving audio
unchanged.

Instead of plotting the raw complex response magnitude directly, compute a
display floor from component energy and blend/max against it. For example, the
plot could avoid drawing below some fraction of the energy in the contributing
parts:

```cpp
rawMag = abs(lowBandResponse);
energyFloor = floorAmount * sqrt(abs(lpA)^2 + abs(bpB)^2);
displayMag = sqrt(rawMag * rawMag + energyFloor * energyFloor);
```

Approximate sweep behavior from a model:

- `floorAmount = 0.03`: deepest dip around `-32 dB`
- `floorAmount = 0.05`: deepest dip around `-28 dB`
- `floorAmount = 0.08`: deepest dip around `-24 dB`
- `floorAmount = 0.10`: deepest dip around `-22 dB`

Pros:

- Does not change audio.
- Avoids endless coefficient tuning.
- Keeps the display from showing razor-thin near-zero nulls that may be more
  visually alarming than musically useful.
- Easy to test as a display/preview behavior.

Cons:

- The displayed curve becomes intentionally less literal.
- If the audio dip is actually objectionable, this hides the symptom rather
  than fixing the sound.
- Needs clear internal naming so future work understands it is a display floor,
  not the physical transfer function.

Recommended if the sound of `Low + Band` is acceptable and the main issue is
the expected response drawing.

### 2. Redesign The Low + Band Topology

Change the actual mode shape so it is less prone to near-perfect cancellation.
This means replacing or augmenting the current linear recipe:

```cpp
0.92 * lpA + 1.18 * bpB - 0.16 * (bpA + bpB)
```

Possible directions:

- Keep a stronger polarity-stable low foundation and use bandpass only as an
  additive accent.
- Replace the subtractive band-sculpt term with a smaller or differently phased
  cascade/parallel blend.
- Blend between the current parallel hybrid and a cascade-ish response at wider
  spans.
- Design the mode around a target response family instead of tuning component
  coefficients by eye.

Pros:

- Fixes the audio and display together.
- Avoids a mismatch between what the user hears and what the curve shows.
- Can make `Low + Band` more musically predictable across SPAN.

Cons:

- Changes the sound of the mode globally, not just at `16.5 st`.
- Needs broader listening/testing across resonance, balance, and span.
- More likely to affect existing patches if Bifurx is treated as release-stable
  later.

Recommended if the dip is audibly wrong or if `Low + Band` should feel smoother
and less cancellation-based by design.

### 3. Audio-Side Nonlinear Cancellation Guard

Detect when the output magnitude is very small relative to component energy and
inject or blend in a small floor.

Conceptually:

```cpp
if (abs(output) << componentEnergy) {
	output = guardedOutput;
}
```

This is not a simple linear filter response anymore. It would be signal
dependent and could create nonlinear artifacts around the cancellation zone.

Pros:

- Can directly prevent deep audio holes.
- Could preserve most of the current mode shape outside cancellation zones.

Cons:

- Adds nonlinear behavior to what is currently a linear mode mix.
- May produce distortion, motion, or level-dependent behavior around the null.
- Harder to reason about and test.

Not recommended unless a specifically nonlinear character is desired.

### 4. Coefficient Retuning Only

Lower the subtractive coefficient, for example from `0.16` to `0.08`.

Pros:

- Very small code change.
- Improves the exact reported `16.5 st` case.

Cons:

- Moves the sharpest dip to another span instead of solving the cause.
- Risks endless tuning around single screenshots/settings.
- Changes mode tone while still leaving a deep cancellation somewhere nearby.

Not recommended as the primary fix.

## Suggested Next Step

First decide whether the issue is visual, audible, or both:

- If visual only: revert any coefficient tweak and implement a display-only
  energy floor for `Low + Band` expected response.
- If audible: redesign the `Low + Band` topology and validate with focused
  sweeps plus listening tests.
- If both: topology redesign is cleaner than display masking.

For future testing, keep a regression sweep around the original neighborhood:

- mode index `1` / UI mode `2`
- `RESO ~= 0.4795`
- spans from roughly `14 st` through `24 st`
- inspect the deepest point between the A/B markers

That sweep catches the important failure mode: not just a dip, but a
needle-like null that appears only at a narrow span.
