# Morphagene S.O.S. Refinement — Codex Handoff

## Objective

Refine the module's **Sound On Sound (S.O.S.)** behavior to match the recovered MG204 signal flow. This is a small, targeted change. Preserve the existing reel, splice, playback, recording, input-gain, and UI architecture unless separation is required to obtain the routing below.

## Core correction

Do not implement S.O.S. as a conventional additive feedback control:

```cpp
// Incorrect for Morphagene
recordSignal = liveInput + feedback * reelPlayback;
```

The firmware uses S.O.S. as a **complementary linear crossfade** between the conditioned live input and rendered reel playback:

```cpp
mixed = (1.0f - sos) * live + sos * playback;
```

An equivalent form, matching the recovered instruction sequence, is:

```cpp
mixed = live + sos * (playback - live);
```

With the default `inop = 0`, the resulting mixture is both:

1. sent to the audio outputs; and
2. sent to the record write path while recording.

With `inop = 1`, monitoring remains the same S.O.S. crossfade, but the record
write source is overridden to the conditioned live input only. Preserve this
explicit option; it is recovered MG204 behavior, not a Chimera extension.

Apply S.O.S. once. Do not crossfade the monitor path and then apply S.O.S. again
in the recording path. `inop = 1` selects a different record source after the
monitor mix has been formed; it does not disable or alter the monitor crossfade.

## Signal flow

```text
conditioned live input ─┐
                       ├─ complementary S.O.S. crossfade ── audio output
rendered reel playback ─┘                      │
                                              └─ record source when inop = 0

conditioned live input ────────────────────────── record source when inop = 1
```

The playback side is the presently audible, manipulated reel signal.
Consequently, with `inop = 0`, Vari-Speed, direction, Gene Size, Slide, Morph,
Organize, and other audible playback transformations can be committed into a
recording during Time Lag Accumulation or Record Into New Splice. With
`inop = 1`, those transformations remain audible through monitoring but are
not fed to the record write path.

The record head itself remains forward-moving. Reverse Vari-Speed reverses the playback material being fed into the crossfade; it does not make the record head run backward.

## Exact recovered control mapping

The filtered 12-bit S.O.S. ADC value is at RAM `0x20021c68`.

The target coefficient is:

```cpp
float sosTarget = std::clamp(
    sosAdc * 0.0002635046258f - 0.04884000123f,
    0.0f,
    1.0f);
```

Equivalent calibration form:

```cpp
float sosTarget = std::clamp(
    (sosAdc - 185.3478f) / 3795.0f,
    0.0f,
    1.0f);
```

This produces these practical 12-bit regions:

| ADC | Result |
|---:|---:|
| 0–185 | exactly `0.0` |
| 186–3980 | linear transition |
| 3981–4095 | exactly `1.0` |
| 2048 | approximately `0.490817` |

The endpoint plateaus are control-calibration margins. Preserve them if firmware-level behavior is the goal.

If the implementation starts from a normalized effective S.O.S. control rather than a simulated ADC, quantize it to `0...4095` before applying the firmware calibration when exact boundary behavior matters.

## Required smoothing

At the native 48 kHz rate, the firmware smooths S.O.S. once per audio frame:

```cpp
smoothedSOS += (sosTarget - smoothedSOS) * 0.001f;
```

This is approximately a 20.8 ms one-pole time constant. Do not apply the raw target directly to the crossfade.

VCV Rack can run at arbitrary sample rates. Preserve the firmware's time-domain response rather than hard-coding a rate-dependent response:

```cpp
const float sosAlpha = 1.0f - std::pow(
    1.0f - 0.001f,
    48000.0f / sampleRate);

smoothedSOS += (sosTarget - smoothedSOS) * sosAlpha;
```

The coefficient can be recalculated when the sample rate changes; do not call `pow()` per sample.

## Knob and CV semantics

The hardware S.O.S. CV input is unipolar `0...8 V` and normalized to `+8 V`.

- With no CV cable, the knob is the full S.O.S. control.
- With a CV cable, the knob attenuates the incoming voltage.
- The CV is multiplicative, not an additive knob-plus-CV offset.

Faithful VCV behavior:

```cpp
float sosNormalized;

if (inputs[SOS_CV_INPUT].isConnected()) {
    const float cv = std::clamp(
        inputs[SOS_CV_INPUT].getVoltage() / 8.0f,
        0.0f,
        1.0f);
    sosNormalized = params[SOS_PARAM].getValue() * cv;
} else {
    sosNormalized = params[SOS_PARAM].getValue();
}
```

Then convert `sosNormalized` through the recovered ADC/calibration mapping and smoothing. Do not normalize against `10 V` merely because that is common in Rack; the Morphagene input range is `8 V`.

## Stereo behavior

Crossfade the two channels independently with the same smoothed S.O.S. coefficient:

```cpp
const float mixedL = liveL + smoothedSOS * (playbackL - liveL);
const float mixedR = liveR + smoothedSOS * (playbackR - liveR);
```

Preserve the module's existing mono-to-stereo input normalization and MG204 input-gain selection upstream of this operation. S.O.S. should receive already-conditioned live samples and should not duplicate input gain or clipping.

## Endpoint behavior

| S.O.S. | Monitor output and `inop = 0` record-write signal |
|---:|---|
| `0.0` | live input only; old material is replaced while recording |
| `0.5` | 50% live + 50% playback; each side is −6.02 dB in amplitude |
| `1.0` | playback only; no live input enters the recording |

With no live input connected, S.O.S. acts as a VCA for the playback loop.

With `inop = 0`, ordinary 1:1 playback, and repeated overdub passes, existing
material follows:

```text
old amplitude after N passes = original amplitude × sos^N
```

This feedback-like decay emerges from routing the crossfade into the record path. Do not add a separate feedback term or compensating gain.

## Recording modes

Honor `inop` in both relevant recording modes:

- **Time Lag Accumulation:** with `inop = 0`, overwrite the active destination
  with the live/playback mixture while the selected material loops; with
  `inop = 1`, write conditioned live input only.
- **Record Into New Splice:** with `inop = 0`, write the audible mixture into
  the new splice; with `inop = 1`, write conditioned live input only.

The normal monitor/output crossfade remains active when not recording. S.O.S. is not merely a record-only parameter.

The recovered options string defines the modes directly:

```text
inop 0 // record SOS mix
inop 1 // record input only
```

The parsed option is at offset `+0x24` from the options structure beginning at
RAM `0x200012a8`. The DSP forms the S.O.S. monitor mix first, then selects
either that mixture or the conditioned live samples for the buffer write.

## Suggested processing order

```cpp
// 1. Existing input stage
const StereoFrame live = conditionLiveInput(rawInput);

// 2. Existing reel/Gene engine
const StereoFrame playback = renderPlayback();

// 3. Recovered S.O.S. control path
const float sosTarget = computeSosTarget();
smoothedSOS += (sosTarget - smoothedSOS) * sosAlpha;

// 4. One complementary crossfade
const StereoFrame sosMix {
    live.left  + smoothedSOS * (playback.left  - live.left),
    live.right + smoothedSOS * (playback.right - live.right),
};

// 5. Monitor is always the S.O.S. mixture
sendToOutputs(sosMix);

if (recording) {
    // Recovered MG204 inop override
    const StereoFrame recordSource =
        (inop == 1) ? live : sosMix;
    writeRecordHeadForward(recordSource);
}
```

Adapt this to the existing architecture rather than duplicating the playback renderer or record machinery.

## Required tests

### Mapping

- ADC `0`, `185` → target `0`.
- ADC `186` → small positive target.
- ADC `2048` → approximately `0.490817`.
- ADC `3980` → approximately `0.999908`.
- ADC `3981`, `4095` → target `1`.

### Crossfade

Using constant `live = 1` and `playback = -1`, after settling:

- S.O.S. `0` → `1`.
- S.O.S. `0.5` → `0`.
- S.O.S. `1` → `-1`.

Test both stereo channels independently.

### CV behavior

- Unpatched CV: knob `0.75` produces an effective normalized control of `0.75` before calibration.
- Patched `8 V`, knob `0.75` → `0.75`.
- Patched `4 V`, knob `0.75` → `0.375`.
- Patched `0 V` → `0`, regardless of knob.
- Negative CV clamps to `0`; CV above `8 V` clamps to `1` before knob attenuation.

### Smoothing

- At 48 kHz, the smoothing coefficient is `0.001`.
- At other sample rates, the elapsed-time response matches the 48 kHz reference within tolerance.
- A control step does not produce an unsmoothed discontinuity.

### Recording

- At S.O.S. `0`, recording writes only live input.
- With `inop = 0`, S.O.S. `1` records only rendered playback.
- With `inop = 0`, S.O.S. `0.5` records the same equal linear mixture heard at the outputs.
- With `inop = 1`, recording writes conditioned live input only at every S.O.S.
  position while the outputs continue to monitor the S.O.S. crossfade.
- The mix is not applied twice.
- With `inop = 0` and 1:1 identity playback, repeated passes retain old material by `sos^N`.
- With `inop = 1`, playback transformations and reel feedback are not written.
- Reverse Vari-Speed reverses the recycled playback but the write index still advances forward.
- With `inop = 0`, playback transformations audible at the outputs are captured
  during Time Lag Accumulation and Record Into New Splice.

### Real-time safety

- No allocation, locks, logging, file access, or per-sample transcendental calculation occurs in the audio callback.
- Crossfade and record writes remain finite and bounds-safe at all parameter and CV extremes.

## Acceptance criteria

The refinement is complete when:

- S.O.S. is a smoothed complementary live/playback crossfade;
- its mixture always feeds monitoring and, by default, recording;
- `inop = 1` preserves the monitor mixture while overriding recording to
  conditioned live input only;
- the knob attenuates patched `0...8 V` CV rather than adding to it;
- the ADC endpoint calibration and 48 kHz smoothing response are preserved;
- `inop = 0` repeated overdubs exhibit the expected `sos^N` retention without
  an additional feedback path;
- recording remains forward-moving while manipulated or reversed playback can be re-recorded;
- all mapping, CV, smoothing, stereo, overdub, and real-time-safety tests pass.

## Confidence

High confidence / implement directly:

- S.O.S. RAM control source `0x20021c68`;
- calibrated linear target mapping and endpoint saturation;
- `0.001` smoothing at 48 kHz;
- complementary crossfade equation;
- default `inop = 0` shared output and record-write mixture;
- explicit `inop = 1` live-input-only record-source override after the monitor
  crossfade;
- multiplicative knob/CV behavior and `0...8 V` CV range;
- forward recording with independently reversible playback.

Preserve existing code unless separately investigated:

- MG204 four-step input-gain staging;
- mono-input normalization details;
- any codec-specific clipping or output filtering outside the S.O.S. crossfade.
