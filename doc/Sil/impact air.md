# Sil: Impact Air Spec

Target module: Sil  
Feature name: **Impact Air**  
Working description: bass-driven transient clarity lift

## Goal

Add a subtle mastering-chain stage that uses low-frequency mono impact to briefly open the top end.

When strong bass mono energy appears below `120 Hz` -- kick drum, low tom, bass transient -- an envelope follower drives a gentle high-shelf lift above `1 kHz`. The intended result is a short-lived clarity accent that follows low-end impact without permanently brightening the mix.

This is not a general transient enhancer. It is specifically a **bass-triggered air/presence lift**.

## Chain Position

Recommended starting position:

```text
Input
  -> Low Frequency Mono Recovery
  -> Impact Air
  -> Remove Mud
  -> Glue Compressor
  -> Stereo Enhance
  -> Saturator
  -> Color Limiter
  -> Final Limiter
  -> Output / analyzers
```

Rationale:

- The detector can reuse or mirror the same low-band mono region used by Low Frequency Mono Recovery.
- The air lift happens early enough that Glue/Saturator/Limiter can still integrate and control the added brightness.
- Remove Mud remains free to clean the low-mid body after the impact-driven lift has been decided.

If testing shows the shelf interacts better after Glue, that can be evaluated later, but the first implementation should start near the low-band stage.

## Detector

Listen to the mono bass range below `120 Hz`.

Suggested detector signal:

```cpp
const float lowMid = 0.5f * (lowL + lowR);
const float detector = std::fabs(lowMid);
```

Use a fast envelope follower:

```text
Detector band: mono low band, <120 Hz
Attack: 2-8 ms
Release: 80-180 ms
Gate floor: ignore very low bass envelope
Mapping: soft-knee, sqrt, or log-shaped 0..1 activation
```

The detector should react quickly to kick-like events but decay smoothly enough to avoid audible brightness flutter.

## Processing

Apply one shared stereo high-shelf lift to the current L/R signal.

Starting EQ target:

```text
Type: high shelf
Frequency: 1 kHz
Max lift: +1.0 dB
Slope/Q: gentle and broad
Gain smoothing: separate from detector envelope if needed
Coefficient update divider: similar to other dynamic EQ stages
```

The lift should be subtle. At full activation it should read as transient clarity, not as a tonal re-balance.

Pseudo-flow:

```cpp
const float activation = mapBassEnvelopeTo01(bassEnvelope);
targetShelfDb = activation * kImpactAirMaxLiftDb;
smoothedShelfDb = smooth(targetShelfDb);

processedL = highShelfL.process(inputL);
processedR = highShelfR.process(inputR);
```

## Suggested Constants

```cpp
static constexpr float kImpactAirDetectorHz = 120.f;
static constexpr float kImpactAirShelfHz = 1000.f;
static constexpr float kImpactAirMaxLiftDb = 1.0f;

static constexpr float kImpactAirEnvAttackSec = 0.004f;
static constexpr float kImpactAirEnvReleaseSec = 0.120f;
static constexpr float kImpactAirGainAttackSec = 0.010f;
static constexpr float kImpactAirGainReleaseSec = 0.160f;

static constexpr float kImpactAirGateDbFs = -42.f;
static constexpr float kImpactAirGateKneeDb = 10.f;
static constexpr int kImpactAirCoeffDivision = 32;
```

These are starting values only. The max lift should remain conservative unless listening tests show the stage is too shy.

## Metering

If the stage gets a chain LED, brightness should reflect actual shelf gain:

```cpp
ledAmount = clamp(smoothedShelfDb / kImpactAirMaxLiftDb, 0.f, 1.f);
```

## Naming Notes

Preferred label: **Impact Air**

Other names considered:

- Bass-Driven Air Lift
- Kick Air
- Transient Air Lift
- Low-End Spark
- Bass-Triggered Presence

Avoid **Transient Enhancer** as the primary label. It is too broad for a stage that specifically uses bass impact to trigger a high-frequency lift.
