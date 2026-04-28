Yes — with that intent, this is a real mismatch.

At `LEVEL = 0.5`, the code does **not** currently produce a true unity baseline.

## What is happening

The midpoint level value is sent through:

```cpp
float levelDriveGain(float knob) {
    const float x = bifurx::clamp01(knob);
    return 0.06f + 0.95f * x + 3.6f * x * x * x;
}
```

At `x = 0.5`:

```text
0.06 + 0.475 + 0.45 = 0.985
```

So even in the small-signal case, the midpoint level is about:

```text
20 * log10(0.985) ≈ -0.13 dB
```

That alone is enough to create a faint purple tint if the overlay is sensitive. The default `LEVEL_PARAM` is configured at `0.5`, but the gain law’s midpoint is slightly below unity. ([GitHub][1])

The larger issue is that the signal is also always passing through the softclip stages:

```cpp
drivenIn = 5.f * softClip(0.2f * in * drive);
```

and later:

```cpp
out = 5.5f * softClip(modeOut / 5.5f);
```

So even if `drive` were changed from `0.985` to `1.0`, a normal modular-level signal can still lose apparent amplitude because the input and output shapers are active at the supposed neutral level. ([GitHub][1])

## Why the overlay reveals it

The analyzer is comparing raw input against final output:

```cpp
pushAnalysisSample(in, out);
```

Then it computes:

```cpp
10.f * log10(outputEnergy / rawInputEnergy)
```

So the purple region is accurately reporting: “the final output has less energy than the raw input here.” It is not only measuring the lowpass topology; it is measuring the full module chain, including level drive and output softclip. ([GitHub][1])

## The real bug

The bug is not merely the overlay. The module’s **LEVEL midpoint contract** is broken.

Your design intent is:

```text
LEVEL = 0.5  →  unity gain / neutral baseline
```

The current implementation is closer to:

```text
LEVEL = 0.5  →  slightly under-unity small-signal gain
             →  plus nonlinear compression for ordinary ±5V-ish material
```

That explains both symptoms:

```text
flat passband looks slightly purple
module response curve shows a dip
```

## Recommended fix

I would make `LEVEL = 0.5` an actual identity path, not just “roughly neutral drive.”

Something like this:

```cpp
float levelInputGain(float knob) {
    const float x = bifurx::clamp01(knob);

    // 0.0 -> silence/low gain
    // 0.5 -> unity
    // 1.0 -> hotter than unity
    if (x <= 0.5f) {
        return rack::math::rescale(x, 0.f, 0.5f, 0.f, 1.f);
    }

    const float hot = (x - 0.5f) / 0.5f;
    return 1.f + 2.5f * hot * hot;
}

float levelDriveAmount(float knob) {
    const float x = bifurx::clamp01(knob);

    // No saturation at neutral or below.
    if (x <= 0.5f) {
        return 0.f;
    }

    const float hot = (x - 0.5f) / 0.5f;
    return hot * hot;
}
```

Then in `process()`:

```cpp
const float level = params[LEVEL_PARAM].getValue();
const float inputGain = levelInputGain(level);
const float driveAmount = levelDriveAmount(level);

float excitation = in * inputGain;

if (driveAmount > 1e-5f) {
    const float drive = 1.f + 3.5f * driveAmount;
    excitation = 5.f * bifurx::softClip((excitation * drive) / 5.f);

    // Optional compensation so increased drive is character, not just loudness.
    // excitation *= postDriveTrim(driveAmount);
}
```

And I would also consider making the final output softclip conditional or much softer around neutral:

```cpp
float out = modeOut;

if (level > 0.5001f) {
    out = 5.5f * bifurx::softClip(modeOut / 5.5f);
}
```

or better:

```cpp
const float clipWet = smoothstep(0.5f, 1.f, level);
const float clipped = 5.5f * bifurx::softClip(modeOut / 5.5f);
const float out = mixf(modeOut, clipped, clipWet);
```

That keeps the midpoint honest while still letting the upper half of LEVEL become “drive/character.”

## Minimal quick fix

At minimum, change the gain curve so midpoint equals exactly unity:

```cpp
float levelDriveGain(float knob) {
    const float x = bifurx::clamp01(knob);

    // Same basic shape, but adjusted so x=0.5 returns 1.0.
    return 0.075f + 0.95f * x + 3.6f * x * x * x;
}
```

But that only fixes the tiny `-0.13 dB` small-signal error. It does **not** fix the deeper issue that the softclip stages are still compressing normal signal levels at the supposed neutral setting.

## My preferred final behavior

I’d define the knob contract this way:

```text
LEVEL 0.0–0.5:
    clean level trim
    0.5 = exact unity
    no intentional saturation

LEVEL 0.5–1.0:
    unity-or-hot gain
    increasing nonlinear drive
    optional output safety clipping
```

Then the flat lowpass passband should stay visually neutral at `LEVEL = 0.5`, and any purple before peak1 would become meaningful again instead of being a side-effect of the gain staging.

[1]: https://github.com/PlasmaChroma/Leviathan-Rack2/blob/expander/src/Bifurx.cpp "Leviathan-Rack2/src/Bifurx.cpp at expander · PlasmaChroma/Leviathan-Rack2 · GitHub"
