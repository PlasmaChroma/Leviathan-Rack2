Dragon King Leviathan, yes: **“Remove Mud” should be a gentle dynamic low-mid EQ / low-mid decongestor**, not a static cut and not a hard artifact gate.

The mastering idea is valid: mud is commonly associated with low-mid buildup around roughly **200–500 Hz**, while dynamic EQ is a good model because it can reduce that region only when the band-limited signal becomes excessive. A commercial dynamic EQ model typically triggers from a band-limited version of the input and applies a bounded dynamic gain change rather than permanently removing the band. ([masteringbox.com][1])

## Recommended v1 design

Add a new stage:

```cpp
input
→ low-band mono recovery
→ Remove Mud dynamic EQ
→ micropeak cleanup
→ limiter
→ output
```

That chain fits Sil’s current architecture nicely because Sil already has a mastering-wide enable switch, a low recovery stage, micropeak cleanup, limiter, and per-stage LEDs / visual feedback. The current light enum and process path already have the pattern you need: compute an amount, smooth it into a light, and keep the stage bypassed when mastering is disabled.

## Detection model

Do **not** trigger only on “there is energy at 300 Hz.” That will false-trigger on bass notes, toms, drones, warm pads, male vocals, and intentional body.

Instead compute a **mud index**:

```cpp
mudIndexDb = lowMidBandDb - referenceBandDb - allowedWarmthDb
```

Where:

```cpp
lowMidBand = 180–520 Hz
referenceBand = weighted blend of:
  80–160 Hz     // true bass/body anchor
  700–3000 Hz   // clarity/presence anchor
```

Suggested constants:

```cpp
kMudLowHz        = 180.f;
kMudHighHz       = 520.f;
kMudCenterHz     = 315.f;
kMudQ            = 0.75f;
kAllowedWarmthDb = 1.5f;
kMudThresholdDb  = 2.0f;
kMudKneeDb       = 4.0f;
kMudMaxCutDb     = 2.5f;   // v1 safe ceiling
```

The important trick: **compare low-mids to the rest of the mix**, not to an absolute threshold. Otherwise quiet muddy material may never trigger, and loud clean material may trigger constantly.

## Add a tonal / narrowband guard

This matters a lot in Rack-world.

A single sine, bass note, or pitched oscillator around 220–400 Hz should **not** be treated as mud. Mud is usually broad, congested, and sustained. So split the detector into three subbands:

```cpp
bandA = 180–260 Hz
bandB = 260–360 Hz
bandC = 360–520 Hz
```

Then compute:

```cpp
mudAvgDb = average(bandA, bandB, bandC)
mudSpreadDb = max(bandA, bandB, bandC) - min(bandA, bandB, bandC)
broadness = 1.0 - smoothstep(6 dB, 14 dB, mudSpreadDb)
```

Then:

```cpp
rawMud = softKnee(mudAvgDb - referenceDb - kAllowedWarmthDb,
                  kMudThresholdDb,
                  kMudKneeDb);

activation = rawMud * broadness;
```

That prevents a pure 300 Hz tone from turning the LED into the Eye of Sauron.

## Correction model

For v1, use a **single wide dynamic bell cut** centered around 300–350 Hz.

Target gain reduction:

```cpp
targetCutDb = -kMudMaxCutDb * activation;
```

Smoothing:

```cpp
attack  = 80–150 ms
release = 500–1200 ms
```

For mastering, I would start slower rather than faster:

```cpp
kMudAttackSec  = 0.120f;
kMudReleaseSec = 0.850f;
```

That keeps the filter from “talking” or pumping with kick/bass movement. Dynamic EQ attack/release behavior is exactly the right conceptual tool here; the low-mid band should breathe down when congested and return slowly when the mix opens. ([fabfilter.com][2])

## Best implementation path

Use two separate components:

### 1. Detector filters

Use cheap bandpass filters / biquads / SVFs for:

```cpp
mudA, mudB, mudC
bassReference
presenceReference
```

You only need RMS/envelope values from these.

### 2. Correction filter

Either of these is acceptable:

**Preferred v1:** RBJ peaking EQ biquad with dynamic gain.

```cpp
center = 315 Hz
Q      = 0.75
gain   = smoothedCutDb, 0 to -2.5 dB
```

Update coefficients at control rate, not every sample:

```cpp
dsp::ClockDivider mudCoeffDivider;
mudCoeffDivider.setDivision(32); // or 64
```

Because gain changes are already smoothed, coefficient updates at 32–64 samples should be transparent and cheap.

**Alternative:** subtractive band method:

```cpp
mudBand = bandpass(input);
output = input + (gainLinear - 1.f) * mudBand;
```

This is simpler and stable, but a proper peaking EQ will be more predictable.

## Mid/Side option

I would make v1 **stereo-linked L/R** first to avoid stereo image drift.

Then v2 can add a context-menu option:

```text
Remove Mud Mode:
- Stereo Linked
- Mid/Side Gentle
```

For Mid/Side Gentle:

```cpp
M = 0.5f * (L + R);
S = 0.5f * (L - R);

midCutMax  = 2.0 dB;
sideCutMax = 3.5 dB;
```

This is musically useful because low-mid mud in the **side channel** often reads as stereo haze / wool / fog, while center low-mids carry kick, bass body, vocal warmth, and musical gravity.

## LED behavior

The LED should represent **actual gain reduction**, not just detection.

```cpp
removeMudLed = clamp((-smoothedCutDb) / kMudMaxCutDb, 0.f, 1.f);
lights[REMOVE_MUD_LIGHT].setSmoothBrightness(
    masteringEnabled ? removeMudLed : 0.f,
    args.sampleTime
);
```

Suggested response:

```text
0.00–0.10  off / barely glowing
0.10–0.35  gentle cleanup
0.35–0.70  active decongestion
0.70–1.00  heavy mud removal, rare
```

I’d make it **amber/yellow**, since it belongs near “mastering cleanup” rather than “danger repair.” Micropeak can remain red because that is more artifact/repair-coded.

## C++ integration sketch

Add to `LightId`:

```cpp
enum LightId {
    LIMITER_ACTIVE_LIGHT,
    LOW_RECOVERY_LIGHT,
    REMOVE_MUD_LIGHT,
    MICROPEAK_LIGHT,
    MASTERING_ENABLED_LIGHT,
    LIGHTS_LEN
};
```

Add state:

```cpp
struct RemoveMudState {
    float mudEnvA = 1e-9f;
    float mudEnvB = 1e-9f;
    float mudEnvC = 1e-9f;
    float bassEnv = 1e-9f;
    float presenceEnv = 1e-9f;

    float targetCutDb = 0.f;
    float smoothedCutDb = 0.f;
    float ledAmount = 0.f;

    dsp::ClockDivider coeffDivider;

    // Detector filters:
    // Biquad/SVF mudA_L/R, mudB_L/R, mudC_L/R, bass_L/R, presence_L/R

    // Correction filters:
    // Biquad peaking_L/R
} removeMud;
```

In `process()` after low recovery:

```cpp
const float recoveredL = highL + recoveredLowL;
const float recoveredR = highR + recoveredLowR;

float mudCleanL = recoveredL;
float mudCleanR = recoveredR;
float removeMudLed = 0.f;

if (masteringEnabled) {
    const RemoveMudResult mudResult =
        removeMudFilter.process(recoveredL, recoveredR, args.sampleRate);

    mudCleanL = mudResult.l;
    mudCleanR = mudResult.r;
    removeMudLed = mudResult.ledAmount;
}

const float preMasterL = masteringEnabled ? mudCleanL : inL;
const float preMasterR = masteringEnabled ? mudCleanR : inR;
```

Then keep the rest of the chain as-is:

```cpp
micropeak cleanup
limiter
histogram/spectrum update
```

And add:

```cpp
lights[REMOVE_MUD_LIGHT].setSmoothBrightness(
    masteringEnabled ? removeMudLed : 0.f,
    args.sampleTime
);
```

## Acceptance criteria

Use these tests:

1. **Pink noise + broad 300 Hz boost**
   LED should light steadily. Output should sound clearer, not hollow.

2. **Clean full-range music**
   LED should stay mostly off or only gently flicker.

3. **220 Hz sine wave / sine sweep**
   LED should not fully latch. This validates the tonal guard.

4. **Kick + bass loop**
   LED may glow lightly but should not pump with every kick.

5. **Dense Suno / AI-generated low-mid buildup**
   LED should show medium activation, probably 20–60%, with audible clarity gain.

6. **Bypass mastering**
   Remove Mud stage and LED both go inactive, matching Sil’s current global mastering model.

## My strongest recommendation

Build this as:

```text
Remove Mud = adaptive broad low-mid dynamic EQ
Detector = spectral-ratio + broadness guard
Correction = wide peaking EQ, max -2.5 dB
LED = actual gain reduction amount
```

That gives Sil a mastering-minded “clarity intelligence” rather than a blunt subtractive EQ. It allows warmth, body, bass, and ritual gloom to remain sovereign — it only intervenes when the low-mid fog starts eating the stars.

[1]: https://www.masteringbox.com/learn/dynamic-eq-techniques?utm_source=chatgpt.com "Dynamic Equalization Techniques for Mastering and Mixing"
[2]: https://www.fabfilter.com/help/pro-q/using/dynamic-eq?utm_source=chatgpt.com "FabFilter Pro-Q 4 Help - Dynamic EQ"
