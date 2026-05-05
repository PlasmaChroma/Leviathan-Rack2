# Sil Micropeak Detection Refinement Plan

## Problem

Suno/AI-generated audio is expected to contain short digital burrs or micropeak artifacts, but Sil's `MICROPEAK_LIGHT` may not activate on real material even though synthetic large one-sample spikes trigger reliably.

## Current Implementation Snapshot

Relevant files:

- `src/Sil.cpp`
- `src/SilMicropeak.hpp`
- `tests/sil_micropeak_spec.cpp`

Current behavior:

- Sil fills a `2048` sample stereo chunk in `pushMicropeakSample()`.
- A background worker runs `sil_micropeak::analyzeChunk()` off the audio thread.
- On detection, `micropeak.holdSamples` is set to `sampleRate * 10s`.
- `MICROPEAK_LIGHT` and cleanup are active while that hold counter is nonzero.
- Current detector input is final output: `pushMicropeakSample(outL, outR, args.sampleRate);`
- Cleanup uses a 1-sample lookahead repair filter, but it only repairs while the detector hold is active.

Current detector thresholds in `src/SilMicropeak.hpp` are intentionally conservative:

- `minPeak = 0.58 * fullScaleVolts`, which is `2.9 V` at `5 V` full scale.
- `minNeighborDrop = 0.11 * fullScaleVolts`, which is `0.55 V`.
- `isolationRatio >= 6.0`.
- `peak >= localMax * 1.65`.
- Detection requires `eventCount >= 2` or `strongestSeverity >= 1.35`.

These thresholds catch obvious injected spikes but can miss lower-level AI artifacts, limiter-flattened artifacts, and repeated non-isolated waveform burrs.

## Main Diagnosis

The detector is probably looking at the wrong signal and using thresholds that are too absolute.

Scanning `outL/outR` means the limiter and cleanup path can reduce or reshape the very evidence the detector needs. Detection should happen on a pre-limiter signal, preferably after low-band recovery but before micropeak cleanup and limiter gain. That signal is `recoveredL/recoveredR` in `Sil::process()`.

Do not switch all the way to raw `inL/inR` as the default unless testing proves it is better. Raw input ignores Sil's own recovery stage and can trigger on content that the pipeline would naturally tame. A debug mode can compare raw/recovered/output later.

## Refined Required Changes

### 1. Feed the detector pre-cleanup, pre-limiter audio

In `src/Sil.cpp`, change the detector feed from final output to recovered signal.

Current line:

```cpp
pushMicropeakSample(outL, outR, args.sampleRate);
```

Preferred first change:

```cpp
pushMicropeakSample(recoveredL, recoveredR, args.sampleRate);
```

Placement:

- Keep it after `recoveredL/recoveredR` are computed.
- It can remain near the existing output/meters block, but pass recovered signal, not output.
- Do not feed `cleaned.l/cleaned.r`; cleanup can hide artifacts.
- Do not feed limiter output; limiter can hide artifacts.

Acceptance:

- Synthetic spikes before the limiter still trigger.
- Cleanup activation can start on future chunks once detection holds active.
- `make test-fast` and `make -j4 plugin.so` pass.

### 2. Split candidate telemetry from cleanup activation

Current LED only shows `micropeakActive`, which means only confirmed detection/hold. That makes debugging hard because candidate activity is invisible.

Add a candidate brightness path using existing atomics:

- `micropeak.lastEventCount`
- `micropeak.lastSeverity`

Recommended LED behavior:

```cpp
const bool micropeakActive = consumeMicropeakHoldSample();
const float candidateSeverity = micropeak.lastSeverity.load(std::memory_order_relaxed);
const int candidateEvents = micropeak.lastEventCount.load(std::memory_order_relaxed);
const float candidateLed = clamp(candidateSeverity * 0.6f + float(candidateEvents) * 0.08f, 0.f, 0.35f);
const float micropeakLed = micropeakActive ? 1.f : candidateLed;
lights[MICROPEAK_LIGHT].setSmoothBrightness(micropeakLed, args.sampleTime);
```

Important:

- Candidate brightness should not activate cleanup.
- Cleanup should remain gated by confirmed detection hold.
- Candidate brightness should be capped below full brightness, for example `0.35`, so the user can distinguish suspicious candidates from confirmed repair-active state.

### 3. Replace one hardcoded detector with a tunable profile

Do not jump straight to very high sensitivity globally. Add a profile struct in `src/SilMicropeak.hpp` so threshold tuning is explicit and testable.

Suggested shape:

```cpp
struct Profile {
	float minPeakFullScale = 0.30f;
	float minNeighborDropFullScale = 0.045f;
	float minIsolationRatio = 3.25f;
	float minLocalMaxRatio = 1.28f;
	float minSeverityForSingleEvent = 0.55f;
	int minEvents = 2;
	int localRadius = 24;
	int exclusionRadius = 2;
	int maxHalfPeakWidth = 4;
};
```

Then change:

```cpp
inline Result analyzeChunk(const float* left, const float* right, size_t count, float fullScaleVolts = 5.f)
```

To:

```cpp
inline Result analyzeChunk(const float* left, const float* right, size_t count, float fullScaleVolts = 5.f, Profile profile = {})
```

Use profile fields instead of hardcoded constants.

Recommended first-pass profile:

- `minPeakFullScale = 0.30f`, or `1.5 V` at `5 V` full scale.
- `minNeighborDropFullScale = 0.045f`, or `0.225 V`.
- `minIsolationRatio = 3.25f`.
- `minLocalMaxRatio = 1.28f`.
- `minEvents = 2`.
- `minSeverityForSingleEvent = 0.55f`.

Reasoning:

- This is materially more sensitive than the current detector.
- It is less reckless than the temporary `0.18f / 0.025f / 2.75f / 1.20f` profile, which is likely to flag normal bright percussion or hard-edited samples.
- It still requires either repeated candidates or a stronger single event.

### 4. Add a temporary debug profile, but keep it out of default behavior

A high-sensitivity profile is useful while validating against real Suno files, but it should not be the production default.

Debug profile:

- `minPeakFullScale = 0.18f`.
- `minNeighborDropFullScale = 0.025f`.
- `minIsolationRatio = 2.75f`.
- `minLocalMaxRatio = 1.20f`.
- `minEvents = 1`.
- `minSeverityForSingleEvent = 0.35f`.

Use one of these approaches:

- Compile-time test-only profile in `tests/sil_micropeak_spec.cpp`.
- Context-menu debug option later, if needed.
- Internal constant during manual calibration, then remove before release.

Do not ship this as default until false-positive testing is done.

### 5. Add a second detector feature for burr-like artifacts

The current detector mostly finds isolated one-sample spikes. Some AI artifacts may be short rough burrs over `2-8` samples and may not satisfy strict isolation.

Add an optional short-window roughness score inside `analyzeChunk()`:

For each candidate center `i`, compute local second-difference energy:

```cpp
float d2Sum = 0.f;
for (int o = -3; o <= 3; ++o) {
	const size_t j = size_t(int(i) + o);
	const float a = sampleAbsMax(left[j - 1], right[j - 1]);
	const float b = sampleAbsMax(left[j], right[j]);
	const float c = sampleAbsMax(left[j + 1], right[j + 1]);
	const float d2 = a - 2.f * b + c;
	d2Sum += std::fabs(d2);
}
const float roughness = d2Sum / std::max(localMean, 1e-4f);
```

Use it carefully:

- Count as candidate if normal isolation test passes.
- Or count as candidate if `peak >= profile.minPeak` and `roughness >= profile.minRoughness` and width is still short.
- Do not use roughness alone; it will false-trigger on noisy cymbals and distortion.

Suggested starting `minRoughness`: `4.5f`.

This can be a follow-up after recovered-signal feed and profile tuning.

## Test Plan

Extend `tests/sil_micropeak_spec.cpp` before loosening production thresholds too far.

Keep existing tests:

- Clean sine is not flagged.
- Repeated isolated micropeaks are flagged.
- Broad transient is not treated as a micropeak.
- Cleanup interpolates isolated micropeak.
- Cleanup leaves broad transient unchanged.
- Cleanup is gated by active hold.

Add tests:

1. Lower-level repeated micropeaks should trigger with the default refined profile.

Example:

- Background: sine or low noise at `0.2-0.4 V`.
- Spikes: `1.6-2.0 V`, one sample wide.
- Expected: detected.

2. Clean bright transient should not trigger.

Example:

- 8-20 sample triangular or exponential transient at `3-4 V`.
- Expected: not detected.

3. Single strong micropeak should trigger by severity.

Example:

- One isolated `4.5 V` one-sample spike.
- Expected: detected through `minSeverityForSingleEvent`.

4. Debug profile should see weak candidates that default profile ignores.

Example:

- One or two `0.9-1.2 V` spikes on quiet material.
- Expected: default may not detect; debug profile should report candidates/detected.

5. Limiter-hidden case.

Create a synthetic sequence where a pre-limiter spike would be reduced if scanned post-limiter. The pure helper cannot model Sil limiter directly, so test this by analyzing two buffers:

- `preLimiter`: contains spike.
- `postLimiter`: scaled/clipped down.
- Expected: preLimiter detects more events/severity than postLimiter.

## Manual Calibration Procedure

1. Implement recovered-signal feed first.
2. Add candidate LED brightness.
3. Run `make test-fast` and `make -j4 plugin.so`.
4. Feed known Suno/AI audio into Sil.
5. Watch three things:
   - Does candidate brightness flicker on suspicious regions?
   - Does full LED/cleanup activate on obvious artifacts?
   - Does clean non-AI material stay mostly dark?
6. If nothing appears, temporarily use the debug profile.
7. Tune upward until normal percussion, hard sync, and distorted synth patches stop false-triggering.

## Acceptance Criteria

- Known synthetic one-sample spikes trigger reliably.
- Lower-level repeated spikes around `1.5-2.0 V` trigger with the refined default profile.
- Clean sine, saw, pad, and broad transient tests do not trigger.
- Candidate LED brightness shows near misses without activating cleanup.
- Confirmed detection activates cleanup for the existing `10s` hold.
- `make test-fast` passes.
- `make test-rack` passes.
- `make -j4 plugin.so` passes.

## Implementation Order

1. Change detector feed to `recoveredL/recoveredR`.
2. Add candidate LED brightness from `lastEventCount` and `lastSeverity`.
3. Add `Profile` to `SilMicropeak.hpp` and convert hardcoded thresholds.
4. Add lower-level spike and false-positive tests.
5. Tune default profile.
6. Add roughness/burr detector only if real Suno material still misses after steps 1-5.
