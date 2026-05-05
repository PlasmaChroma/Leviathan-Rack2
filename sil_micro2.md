# Sil Micropeak Detector vNext Review

## Current Read

Observed behavior:

```text
Human reference music: no hits observed
AI/Suno material: suspicious reads, but latch still flickers
High-frequency sine sweep: false triggers in the upper range
```

This points to a detector that is close enough to see real AI-path evidence, but still too willing to treat coherent high-frequency curvature as an impulse defect. The main issue is not the hold timeout. The hold only exposes the instability; the detector evidence is still too sparse and too brittle.

The highest-impact fix is to make candidate acceptance more contextual before tuning latch behavior further.

## Current Code Shape

Relevant files:

```text
src/SilMicropeak.hpp
src/Sil.cpp
tests/sil_micropeak_spec.cpp
```

The mono detector currently evaluates each channel independently, then the worker merges the stereo result. That part is correct and should remain.

The weak point is inside `SilMicropeakDetector::analyzeChunkMono()`. The candidate confirmation currently behaves like:

```cpp
const bool isoPass = isolationRatio >= profile.minIsolationRatio;
const bool roughPass = roughness >= profile.minRoughness;

if (!(isoPass || roughPass))
    continue;
```

That `isoPass || roughPass` shape is the likely cause of the high-frequency sine false positive. A sine near Nyquist naturally has high second-difference energy. If roughness alone can confirm a candidate, coherent high-frequency tone can look like micropeak contamination.

## Target Behavior

The detector should identify abnormal impulses relative to local context, not merely narrow or high-curvature samples.

Desired rule hierarchy:

```text
1. Absolute amplitude floor
2. Neighbor drop floor
3. Local envelope / amplitude outlier gate
4. Isolation or contextual roughness confirmation
5. Width rejection
6. Coherence / tone rejection
7. Severity scoring
```

The important shift:

```text
Old: this sample is sharp
New: this sample is unusually sharp and unusually tall for its local context
```

## Implementation Plan

### 1. Add the Sine Regression First

Before changing thresholds, add a failing regression that reproduces the reported false positive.

Add to `tests/sil_micropeak_spec.cpp`:

```cpp
static std::vector<float> makeSineSweep(float sampleRate, float seconds, float f0, float f1, float amp) {
    const size_t n = size_t(sampleRate * seconds);
    std::vector<float> out(n);
    double phase = 0.0;

    for (size_t i = 0; i < n; ++i) {
        const float t = float(i) / float(std::max<size_t>(n - 1, 1));
        const float freq = f0 * std::pow(f1 / f0, t);
        phase += 2.0 * M_PI * double(freq) / double(sampleRate);
        out[i] = amp * std::sin(float(phase));
    }

    return out;
}

static bool detectAny(const std::vector<float>& x, const SilMicropeakProfile& profile, float sampleRate) {
    SilMicropeakDetector detector;
    constexpr size_t chunk = 2048;

    for (size_t offset = 0; offset + chunk <= x.size(); offset += chunk) {
        const SilMicropeakAnalysisResult result =
            detector.analyzeChunkMono(x.data() + offset, chunk, 5.f, sampleRate, profile);
        if (result.detected)
            return true;
    }

    return false;
}

TEST(SilMicropeak, HighFrequencySineSweepDoesNotTrigger) {
    SilMicropeakProfile profile;
    const auto sweep = makeSineSweep(48000.f, 2.f, 8000.f, 21000.f, 3.5f);
    EXPECT_FALSE(detectAny(sweep, profile, 48000.f));
}
```

If `M_PI` is unavailable, use a local constant.

This test should be added before the detector change so the patch proves it addresses the actual failure mode.

### 2. Stop Roughness From Independently Qualifying Events

Make the first production detector change conservative. Do not let roughness alone turn a candidate into an event.

Current rough shape:

```cpp
if (!(isoPass || roughPass))
    continue;
```

Replace with a stricter gate:

```cpp
const bool envelopeOutlier =
    peak >= localMax * profile.minLocalMaxRatio;

const bool isoPass =
    isolationRatio >= profile.minIsolationRatio;

if (!envelopeOutlier)
    continue;

if (!isoPass)
    continue;
```

This intentionally removes the roughness-only path from the default detector. It is the safest short-term fix because it should reduce sine/sweep false positives without making the worker latch more aggressive.

Expected tradeoff:

```text
False positives: should improve immediately
Suno flicker: may stay the same or get slightly worse until evidence accumulation improves
Human music baseline: should remain clean
```

### 3. Reintroduce Roughness Only as Contextual Confirmation

If the conservative change becomes too insensitive on AI material, reintroduce roughness as a contextual outlier, not absolute roughness.

Add profile fields:

```cpp
float minRoughnessOutlierRatio = 3.5f;
float minRoughnessNeighborDropRatio = 1.5f;
```

Calculate roughness from signed samples:

```cpp
const float centerD2 =
    std::fabs(channel[i - 1] - 2.f * channel[i] + channel[i + 1]);

float localD2Sum = 0.f;
int localD2Count = 0;

for (int o = -int(profile.localRadius); o <= int(profile.localRadius); ++o) {
    if (std::abs(o) <= int(profile.exclusionRadius) + 1)
        continue;

    const size_t j = size_t(int(i) + o);
    if (j == 0 || j + 1 >= count)
        continue;

    const float d2 =
        std::fabs(channel[j - 1] - 2.f * channel[j] + channel[j + 1]);
    localD2Sum += d2;
    localD2Count++;
}

const float localD2Mean =
    localD2Sum / float(std::max(localD2Count, 1));

const float roughnessOutlierRatio =
    centerD2 / std::max(localD2Mean, 1e-5f);

const bool roughPass =
    roughnessOutlierRatio >= profile.minRoughnessOutlierRatio &&
    centerD2 >= minNeighborDrop * profile.minRoughnessNeighborDropRatio;
```

Then candidate confirmation becomes:

```cpp
if (!envelopeOutlier)
    continue;

if (!(isoPass || roughPass))
    continue;
```

This keeps roughness useful for generated-audio burrs while rejecting coherent high-frequency tone where curvature is high everywhere.

### 4. Add Coherent-Tone Rejection

If the sine regression still fails after contextual roughness, add a chunk-level regular-spacing guard.

Track accepted event positions:

```cpp
std::array<int, 128> eventPositions {};
int eventPositionCount = 0;
```

When accepting an event:

```cpp
if (eventPositionCount < int(eventPositions.size()))
    eventPositions[eventPositionCount++] = int(i);
```

After the scan:

```cpp
int regularPairs = 0;
int comparedPairs = 0;

for (int k = 2; k < eventPositionCount; ++k) {
    const int d0 = eventPositions[k - 1] - eventPositions[k - 2];
    const int d1 = eventPositions[k] - eventPositions[k - 1];

    if (d0 <= 0 || d1 <= 0)
        continue;

    comparedPairs++;

    if (std::abs(d1 - d0) <= 2)
        regularPairs++;
}

const float spacingRegularity =
    comparedPairs > 0 ? float(regularPairs) / float(comparedPairs) : 0.f;

if (result.eventCount >= 6 && spacingRegularity > 0.75f) {
    result.strongestSeverity *= 0.25f;
    result.detected = false;
}
```

Do this only after candidate filtering. This guard should classify dense, evenly spaced candidates as coherent oscillator content rather than repairable micropeak contamination.

### 5. Replace Weak Streak With Leaky Evidence

The current worker has event count, severity, severity EMA, weak streak, score window, and adaptive hold floors. That is better than a raw latch, but the weak-streak path still favors consecutive chunks. AI material appears to produce intermittent suspicious chunks:

```text
hit -> miss -> weak hit -> miss -> hit
```

A leaky evidence accumulator is a better source classifier.

Add to `MicropeakWorkerState`:

```cpp
std::atomic<float> confidence {0.f};
float artifactEvidence = 0.f;
```

Worker update sketch:

```cpp
float evidenceAdd = 0.f;

if (directOnHit)
    evidenceAdd += 0.35f;
else if (keepHit)
    evidenceAdd += 0.14f;

evidenceAdd += clamp(strongestSeverity * 0.18f, 0.f, 0.25f);

const float decay = latched ? 0.965f : 0.925f;
artifactEvidence = artifactEvidence * decay + evidenceAdd;
artifactEvidence = clamp(artifactEvidence, 0.f, 1.5f);

const bool evidenceOn = artifactEvidence >= 0.70f;
const bool evidenceKeep = artifactEvidence >= 0.35f;
```

Use `evidenceOn` and `evidenceKeep` alongside the existing score-window logic. Do not immediately remove the score window; add evidence first, then simplify once behavior is known.

Suggested mapping:

```cpp
const float confidence =
    clamp(artifactEvidence / 0.70f, 0.f, 1.f);

micropeak.confidence.store(confidence, std::memory_order_relaxed);
```

### 6. Split LED Confidence From Repair Hold

The LED should show detector confidence. Repair should be gated by hold state. Those are related, but not the same.

Current behavior effectively couples the LED to `holdSamples`, so the light can flicker when repair hold drains even if the detector still has weak ongoing evidence.

Recommended audio-thread LED behavior:

```cpp
const float confidence =
    micropeak.confidence.load(std::memory_order_relaxed);

lights[MICROPEAK_LIGHT].setSmoothBrightness(
    clamp(confidence, 0.f, 1.f),
    args.sampleTime);
```

Recommended repair behavior:

```cpp
const bool micropeakActive =
    micropeak.consumeMicropeakHoldSample();
```

This makes the display answer "how convinced is the detector?" and the cleanup gate answer "should repair run on this sample?"

### 7. Align Repair Thresholds With Detection

The detector currently begins around `profile.minPeakFullScale = 0.30f`, which is roughly `1.5 V` at `5 V` full-scale.

The repair filter is stricter:

```cpp
const float minPeak = 0.52f * fullScaleVolts;
const float minNeighborDrop = 0.10f * fullScaleVolts;
```

That means detection can latch while repair ignores most of the events that caused the latch. This can be acceptable while tuning detection, but it should not remain implicit.

Recommended next step after detector stabilization:

```cpp
struct SilMicropeakRepairProfile {
    float minPeakFullScale = 0.30f;
    float minNeighborDropFullScale = 0.035f;
    float maxReplacementDeltaFullScale = 0.20f;
};
```

Then pass the same source-confidence state into cleanup:

```cpp
const float repairSensitivity =
    micropeakActive ? confidence : 0.f;
```

Keep repair conservative:

```text
Low confidence: no repair
Medium confidence: repair only strong isolated samples
High confidence: repair lower-level repeated samples
```

## Suggested Order Of Work

1. Add `HighFrequencySineSweepDoesNotTrigger`.
2. Remove roughness as an independent event qualifier.
3. Run existing micropeak tests and the sine regression.
4. If AI detection becomes too insensitive, add contextual roughness outlier logic.
5. Add `confidence` and leaky evidence in the worker.
6. Drive the LED from confidence while repair remains hold-gated.
7. Revisit repair thresholds after detector behavior is stable.

## Acceptance Criteria

The next detector patch should satisfy:

```text
Clean sine remains undetected.
High-frequency sine sweep remains undetected.
Known human reference music remains visually quiet.
Known AI/Suno sample accumulates confidence instead of rapid flicker.
One-channel artifacts still trigger when only L or only R contains evidence.
Micropeak repair still runs only while the detector hold is active.
```

## Practical Tuning Defaults

Start conservative:

```cpp
profile.minPeakFullScale = 0.30f;
profile.minNeighborDropFullScale = 0.035f;
profile.minIsolationRatio = 2.4f;
profile.minLocalMaxRatio = 1.18f;
profile.minSeverityForSingleEvent = 0.55f;
profile.minEvents = 2;
profile.localRadius = 32;
profile.exclusionRadius = 2;
profile.maxHalfPeakWidth = 4;
```

If contextual roughness is added:

```cpp
profile.minRoughnessOutlierRatio = 3.5f;
profile.minRoughnessNeighborDropRatio = 1.5f;
```

Avoid lowering thresholds until the sine regression is green. Lower thresholds may make Suno latch more often, but they will also make the high-frequency false-positive path worse unless roughness is fixed first.
