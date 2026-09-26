# Morphagene Morph Refinement — Codex Handoff

## Objective

Refine the module's **Morph** behavior to match the recovered MG204 firmware model while preserving the existing UI, CV routing, recording/playback engine, Gene Size mapping, Vari-Speed behavior, and clock support.

This is a targeted DSP/scheduler change, not a rewrite.

## Critical correction to earlier Gene Size guidance

An earlier analysis incorrectly identified RAM `0x20021f48` as a playback increment. It is the **Morph launch-interval factor**.

Do **not** derive source-space Gene distance from Morph or from this address. Keep output-time Gene lifetime separate from source-head motion:

```cpp
geneAgeOutputSamples += 1;
sourcePosition += playbackIncrement;

if (geneAgeOutputSamples >= geneDurationSamples) {
    finishGene();
}
```

Vari-Speed changes how far the source head travels during a Gene. It must not change the Gene's output-time duration or Morph's output-time launch cadence.

## Recovered control mapping

The firmware normalizes the filtered 12-bit Morph ADC and selects one of 22 stages:

```cpp
const float morph = morphAdc / 4096.0f;
const int stage = std::min(21, static_cast<int>(morph * 33.99f));
```

If the implementation receives a normalized `[0, 1]` parameter rather than a 12-bit ADC value, preserve the same binning. A robust equivalent is to quantize to `0...4095` first and then apply the firmware formula.

```cpp
const int morphAdc = std::clamp(
    static_cast<int>(std::floor(normalizedMorph * 4096.0f)),
    0,
    4095);
const int stage = std::min(
    21,
    static_cast<int>((morphAdc / 4096.0f) * 33.99f));
```

Use these exact active stages:

| Stage | ADC range | Launch factor | Nominal density | Cycle denominator |
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

Suggested representation:

```cpp
struct MorphStage {
    int launchNumerator;
    int launchDenominator;
    int cycleDenominator;
};

static constexpr std::array<MorphStage, 22> kMorphStages {{
    {2, 1, 1}, {3, 2, 2}, {4, 3, 3}, {1, 1, 1},
    {4, 5, 5}, {3, 4, 4}, {2, 3, 3}, {3, 5, 5},
    {4, 7, 7}, {1, 2, 2}, {4, 9, 9}, {3, 7, 7},
    {2, 5, 5}, {3, 8, 8}, {4, 11, 11}, {1, 3, 3},
    {4, 13, 13}, {3, 10, 10}, {2, 7, 7}, {3, 11, 11},
    {4, 15, 15}, {1, 4, 4},
}};
```

Prefer rational accumulation or fixed-point phase over a repeatedly rounded floating-point interval. The cycle denominator exists to keep rational launch patterns from drifting.

## Scheduler behavior

Morph controls **when a new Gene launches**:

```text
launch interval = Gene duration in output samples × launch factor
```

Expected regimes:

- Stages 0–2: one active Gene followed by a gap.
- Stage 3: seamless one-Gene-in/one-Gene-out playback.
- Stages 4–20: progressively denser rational overlap.
- Stage 21: four-Gene density plateau for the entire remaining upper knob range.

Maintain four independent voice/read-head slots, each with its own:

- source position;
- signed playback increment;
- output-sample age;
- envelope state;
- active flag;
- per-launch pan and pitch state.

Rotate through the four slots at launch. Do not restart every head on a Morph-stage change. A stage transition should update the future launch schedule without discontinuously resetting active Genes.

The scheduler must be based on output samples. Changing Vari-Speed must change source traversal but must not alter Gene duration, gap duration, or overlap timing.

Preserve the existing Gene envelope if it is already dynamic and click-free. If the current envelope is unusable, isolate any replacement behind a helper and label it provisional; the exact firmware envelope law is not yet fully recovered.

## Continuous upper-Morph behavior

The stage table saturates at stage 21, but the firmware continues to use a smoothed continuous Morph value:

```cpp
smoothedMorph += (morph - smoothedMorph) * 0.01f;
```

This appears in the audio-rate path. Preserve this state separately from the discrete scheduler stage.

### Stereo randomization

The firmware contains an explicit boundary at Morph `0.5`. Above it, the probability/depth of randomized stereo placement grows with `smoothedMorph - 0.5`.

The exact pan distribution and pan law are not yet proven. Implement this in a small isolated helper so it can be replaced later. If the existing module already has convincing upper-Morph stereo behavior, preserve it and only ensure that:

- it begins progressively above `0.5`;
- it uses the continuous smoothed value;
- the per-Gene random choice is stable for that Gene rather than changing every sample;
- tests can use a deterministic random seed.

A provisional normalized depth, if needed, is:

```cpp
const float panDepth = std::clamp(
    (smoothedMorph - 0.5f) / 0.5f,
    0.0f,
    1.0f);
```

Do not describe that particular curve or pan law as bit-exact firmware behavior.

### Pitch randomization

The firmware contains a second explicit boundary at Morph `0.6`. Pitch variation depth is proportional to `smoothedMorph - 0.6` and increases toward full clockwise.

Again, isolate the distribution because its exact law is not yet fully recovered. Preserve a musically working existing implementation where possible, but enforce:

- zero upper-Morph pitch depth at and below `0.6`;
- progressive depth above `0.6`;
- a stable per-Gene pitch choice;
- deterministic seeding in tests.

A provisional normalized depth is:

```cpp
const float pitchDepth = std::clamp(
    (smoothedMorph - 0.6f) / 0.4f,
    0.0f,
    1.0f);
```

Do not invent a new global pitch mode or couple this depth to Gene Size.

## Four Morph voice ratios

The recovered default signed read-head rates are:

```text
voice 1: 1.0      × base Vari-Speed rate
voice 2: 2.0      × base Vari-Speed rate
voice 3: 1.5      × base Vari-Speed rate
voice 4: 1.333333 × base Vari-Speed rate
```

The firmware's `mcr1`, `mcr2`, and `mcr3` options replace the last three ratios. Valid magnitudes are `0.0625...16.0`; a negative value reverses only its associated voice.

If the clone has no options parser, keep the defaults as named constants and expose a narrow internal configuration point. Do not add UI merely for this refinement.

## External clock boundary

Do not rewrite a working clock implementation as part of this task.

Recovered firmware control flow tests the Morph launch factor against `0.5` in the clock-present path and applies the `ckop` policy. This is related to clocked Gene Shift versus time-stretch behavior, but the complete clock state machine is not yet proven.

Therefore:

- use the rational internal scheduler when unclocked;
- keep the existing clocked launch/time-stretch behavior intact where possible;
- structure the scheduler so the clock path can override launch timing cleanly;
- retain the `launchFactor <= 0.5` boundary as an explicit integration point;
- do not claim exact clocked behavior until that state machine is separately refined.

## Implementation order

1. Inspect the current Morph, Gene voice, Gene Size, Vari-Speed, clock, and envelope code before editing.
2. Separate source-head increment, Gene output-time lifetime, and Morph launch timing if they are currently conflated.
3. Add the exact 22-stage mapping and rational launch scheduler.
4. Ensure four independent rotating Gene voices work across gap, seamless, and overlap regimes.
5. Add or retain continuous Morph smoothing.
6. Gate existing stereo and pitch randomization at the recovered `0.5` and `0.6` boundaries, keeping uncertain distributions isolated.
7. Preserve the existing clocked path and document its integration seam.
8. Add focused unit or deterministic DSP tests.

## Required tests

### Mapping edges

Verify at least these transitions exactly:

```text
120 → stage 0     121 → stage 1
361 → stage 2     362 → stage 3
482 → stage 3     483 → stage 4
1084 → stage 8    1085 → stage 9
1807 → stage 14   1808 → stage 15
2530 → stage 20   2531 → stage 21
4095 → stage 21
```

### Scheduling

- Stage 0 launches every `2 × Gene duration`.
- Stage 3 launches every `1 × Gene duration` with no intended gap.
- Stage 9 sustains nominal density `2`.
- Stage 15 sustains nominal density `3`.
- Stage 21 sustains nominal density `4`.
- Long runs at rational stages do not accumulate an extra launch or lose one from floating-point drift.
- A live stage change does not reset all active voices or produce an avoidable discontinuity.

### Parameter independence

- Changing Vari-Speed changes source traversal but not Gene lifetime or launch interval in output time.
- Changing Gene Size scales both Gene lifetime and the Morph launch interval proportionally.
- Morph does not rewrite the base Vari-Speed parameter.

### Upper behavior

- Continuous Morph smoothing follows the recovered one-pole update.
- Pan randomization is absent at/below `0.5` and progressive above it.
- Pitch randomization is absent at/below `0.6` and progressive above it.
- Random choices are per Gene and reproducible under a fixed seed.
- Default voice ratios are `1`, `2`, `1.5`, and `4/3`.
- Negative configurable chord ratios reverse only the associated voice.

### Real-time safety

- No allocation, lock, logging, or file access occurs in the audio callback.
- All four voices remain bounds-safe at extreme Gene Size, Vari-Speed, reverse playback, and buffer wrap points.

## Acceptance criteria

The refinement is complete when:

- the exact 22-stage Morph table drives launch cadence;
- counterclockwise Morph produces real gaps, the 1/1 stage is contiguous, and clockwise motion builds to four overlapping Genes;
- the upper 38.2% of the physical Morph range remains at four-Gene scheduler density while continuous pan/pitch behavior continues to evolve;
- Gene duration and launch cadence are independent of Vari-Speed;
- active voices survive Morph-stage changes without wholesale resets;
- the clocked path has not regressed;
- all mapping, timing, ratio, transition, and real-time-safety tests pass.

## Evidence and confidence

High confidence / implement exactly:

- Morph normalization and stage calculation;
- all 22 active stage ranges;
- rational launch factors and scheduler cycle denominators;
- gap, seamless, overlap, and four-Gene plateau regimes;
- four independent voice concept and default ratios;
- continuous smoothing coefficient;
- upper thresholds at `0.5` and `0.6`;
- separation of output-time scheduling from playback increment.

Incomplete / preserve or isolate:

- exact envelope curve and dynamic-envelope details;
- exact stereo pan distribution and pan law;
- exact pitch-random distribution;
- complete clocked Gene Shift/time-stretch state machine and `ckop` branches.

When code comments distinguish these two categories, use **recovered** for the first and **provisional** for the second. Do not silently turn a provisional approximation into a claimed firmware fact.
