# MG204 Morph parameter notes

## Core mapping

Morph's filtered 12-bit ADC value is stored at `0x20021c6c`.

```cpp
float morph = morphAdc / 4096.0f;
int stage = std::min(21, static_cast<int>(morph * 33.99f));
```

The stage lookup occurs at `0x08027a38..0x08027a70`. The tables originate in
the initialized-data image at flash `0x08047250` and are copied to RAM at boot.

| Stage | ADC range | Launch interval / Gene length | Overlap density | Cycle denominator |
|---:|---:|---:|---:|---:|
| 0 | 0–120 | 2/1 | 1 | 1 |
| 1 | 121–241 | 3/2 | 1 | 2 |
| 2 | 242–361 | 4/3 | 1 | 3 |
| 3 | 362–482 | 1/1 | 1 | 1 |
| 4 | 483–602 | 4/5 | 5/4 | 5 |
| 5 | 603–723 | 3/4 | 4/3 | 4 |
| 6 | 724–843 | 2/3 | 3/2 | 3 |
| 7 | 844–964 | 3/5 | 5/3 | 5 |
| 8 | 965–1084 | 4/7 | 7/4 | 7 |
| 9 | 1085–1205 | 1/2 | 2 | 2 |
| 10 | 1206–1325 | 4/9 | 9/4 | 9 |
| 11 | 1326–1446 | 3/7 | 7/3 | 7 |
| 12 | 1447–1566 | 2/5 | 5/2 | 5 |
| 13 | 1567–1687 | 3/8 | 8/3 | 8 |
| 14 | 1688–1807 | 4/11 | 11/4 | 11 |
| 15 | 1808–1928 | 1/3 | 3 | 3 |
| 16 | 1929–2048 | 4/13 | 13/4 | 13 |
| 17 | 2049–2169 | 3/10 | 10/3 | 10 |
| 18 | 2170–2289 | 2/7 | 7/2 | 7 |
| 19 | 2290–2410 | 3/11 | 11/3 | 11 |
| 20 | 2411–2530 | 4/15 | 15/4 | 15 |
| 21 | 2531–4095 | 1/4 | 4 | 4 |

For stages 3–21, overlap density is the reciprocal of the launch interval.
Stages 0–2 retain one voice and introduce silence between Genes. Stage 3 is the
firmware's seamless 1/1 point.

The cycle denominator is the reduced fraction's denominator and is used to
reset a scheduler counter, preventing rational overlap patterns from drifting.

## Regimes

1. **Gap regime, stages 0–2:** launch interval exceeds one Gene duration.
2. **Seamless point, stage 3:** interval equals one Gene duration.
3. **Overlap regime, stages 4–20:** progressively denser rational overlaps.
4. **Four-Gene plateau, stage 21:** interval remains 1/4 for the remainder of
   the knob range.
5. **Upper spatial/pitch regime:** additional behavior uses the continuous,
   smoothed Morph value even while the stage table is pinned at 21.

The smoothed control state at RAM `0x20022084` is updated in the audio loop:

```cpp
smoothedMorph += (morph - smoothedMorph) * 0.01f;
```

The code contains explicit boundaries at `0.5` and `0.6`:

- Above 0.5, a uniform pseudorandom value scaled by 0.5 is compared with
  `smoothedMorph - 0.5`. This progressively introduces randomized stereo
  placement rather than switching it on as a single hard mode.
- The upper pitch/randomization calculation is proportional to
  `smoothedMorph - 0.6`, giving it zero depth at 0.6 and increasing depth toward
  full clockwise.

## Morph chord voices

The base signed Vari-Speed increment is multiplied by three ratios at RAM
`0x20022124` to produce the additional read-head increments.

Firmware defaults:

```text
voice 1: 1.0 × base rate
voice 2: 2.0 × base rate
voice 3: 1.5 × base rate
voice 4: 1.333333 × base rate
```

The `mcr1`, `mcr2`, and `mcr3` options replace the final three ratios. Accepted
magnitudes are 0.0625 through 16.0. Negative values reverse only the associated
voice.

## External clock

Clock-present control flow tests the launch-interval factor against `0.5` and
then applies `ckop` policy. This couples the Morph stage to Gene Shift versus
time-stretch behavior. The exact state-machine branches remain under analysis;
the table and threshold themselves are verified.

## Implementation guidance

For a faithful clone, use the table exactly rather than replacing it with a
continuous overlap curve. Schedule a new Gene every:

```text
launchInterval = geneDuration * launchFactor[stage]
```

Maintain four independent read heads/envelopes. The first three stages create
silence; stage 3 is contiguous; later stages overlap. Continue using the
continuous smoothed Morph value for upper-regime pan and pitch depth after the
four-Gene scheduler has saturated.

The exact random distributions, pan law, and dynamic-envelope shapes still
need final instruction-level reconstruction before claiming bit-faithful
behavior.
