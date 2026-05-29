# Sil Optimization Pass 1: Behavior-Preserving Safe Wins

Target file: `src/Sil.cpp`

## Intent

This pass only targets optimizations and lifecycle fixes that should not change Sil's audible behavior or user-facing bypass behavior.

Current behavior to preserve:

- When `MASTERING_ENABLED_PARAM` is off, Sil still runs the mastering/adaptive chain internally.
- The audible output is dry input while mastering is off.
- Adaptive state stays warm for A/B testing.
- Histogram/waveform and spectrum views continue updating while mastering is off.
- Stage LEDs may remain gated by `masteringEnabled` exactly as they are today unless a separate UI decision changes that later.

Do **not** introduce `WarmBypass` / `ColdBypass` modes in this pass.
Do **not** skip mastering-stage processing in this pass.
Do **not** move stage decision math to lower control rates in this pass.
Do **not** change DSP constants, thresholds, gains, chain order, or limiter ceiling.

The goal is a small, reviewable patch set with minimal sonic risk.

---

## Priority Summary

| Priority | Change | Expected Risk | Behavior Impact |
|---:|---|---|---|
| 1 | Stop micropeak worker in destructor | Very low | None, lifecycle safety only |
| 2 | Replace rolling RMS scan with O(1) rolling sum | Low | Intended equivalent program RMS estimate, likely smoother |
| 3 | Remove redundant limiter linear interpolation peak loop | Low | No sample-peak behavior change expected |
| 4 | Replace initializer-list `std::max({ ... })` in hot paths | Very low | None |
| 5 | Divide light smoothing updates | Very low | Same target brightness, lower update rate |
| 6 | Precompute spectrum bin map | Low | Same spectrum bins, less repeated math |
| 7 | Avoid modulo in FFT input copy | Low/Medium implementation risk | None if ordering is preserved |

Recommended first commit: priorities 1-5 only.
Recommended second commit: priorities 6-7 after verifying visuals.

---

## 1. Micropeak Worker Destructor Safety

Current destructor deletes the FFT resource but does not stop the micropeak worker thread.

Implement:

```cpp
~Sil() {
	stopMicropeakWorker();
	delete spec.fft;
}
```

Acceptance:

- Module destruction cannot leave a worker thread running.
- No audio behavior changes.
- No UI behavior changes.

Notes:

- `stopMicropeakWorker()` is already written to be safe when the thread is not joinable.
- This should be done even if repair is not fully wired into the current process path.

---

## 2. O(1) Rolling Program RMS

Current `estimateRollingProgramDbFs()` scans up to 2048 samples from the rolling buffer when the glue adaptive threshold updates.

Replace this with a maintained rolling mono-square sum.

Add member:

```cpp
double rollingMonoSqSum = 0.0;
```

Update `configureRollingBuffer()`:

```cpp
void configureRollingBuffer(float sampleRate) {
	const int requestedLength = std::max(1, int(std::round(sampleRate * kRollingBufferSeconds)));
	if (requestedLength == rollingBufferLength) {
		return;
	}
	rollingBufferLength = requestedLength;
	rollingBufferL.assign(size_t(rollingBufferLength), 0.f);
	rollingBufferR.assign(size_t(rollingBufferLength), 0.f);
	rollingWriteIndex = 0;
	rollingFilled = 0;
	rollingMonoSqSum = 0.0;
}
```

Replace `pushRollingSample()`:

```cpp
void pushRollingSample(float sampleL, float sampleR) {
	if (rollingBufferLength <= 0) {
		return;
	}

	const int idx = rollingWriteIndex;

	if (rollingFilled >= rollingBufferLength) {
		const float oldMono = 0.5f * (rollingBufferL[size_t(idx)] + rollingBufferR[size_t(idx)]);
		rollingMonoSqSum -= double(oldMono) * double(oldMono);
	}

	rollingBufferL[size_t(idx)] = sampleL;
	rollingBufferR[size_t(idx)] = sampleR;

	const float newMono = 0.5f * (sampleL + sampleR);
	rollingMonoSqSum += double(newMono) * double(newMono);

	rollingWriteIndex++;
	if (rollingWriteIndex >= rollingBufferLength) {
		rollingWriteIndex = 0;
	}
	if (rollingFilled < rollingBufferLength) {
		rollingFilled++;
	}
}
```

Replace `estimateRollingProgramDbFs()`:

```cpp
float estimateRollingProgramDbFs() const {
	if (rollingFilled <= 0 || rollingBufferLength <= 0) {
		return -100.f;
	}
	const double meanSq = std::max(0.0, rollingMonoSqSum) / double(rollingFilled);
	const float rmsVolts = std::sqrt(float(meanSq));
	return toDbFsSafe(rmsVolts);
}
```

Acceptance:

- Glue adaptive threshold continues updating.
- No chain order changes.
- No bypass behavior changes.
- CPU spikes from the periodic RMS scan are removed.

Caution:

- This changes the RMS estimate from a sparse scan of up to 2048 samples to the full rolling window. It should be more stable, but it is not bit-identical.
- If exact behavior preservation is required, skip this item. If practical behavior preservation is acceptable, this is a safe optimization.

---

## 3. Remove Redundant Limiter Linear Interpolation Loop

Current limiter peak detection includes this loop:

```cpp
if (limiterPrevValid) {
	for (int i = 1; i <= kLimiterOversampleFactor; ++i) {
		const float a = float(i) / float(kLimiterOversampleFactor);
		const float interpL = limiterPrevL + (saturatedL - limiterPrevL) * a;
		const float interpR = limiterPrevR + (saturatedR - limiterPrevR) * a;
		peak = std::max(peak, std::max(std::fabs(interpL), std::fabs(interpR)));
	}
}
```

A linear interpolation between two scalar sample values cannot exceed the larger absolute endpoint unless it crosses zero, which only lowers the absolute value. Therefore this loop does not provide real true-peak detection.

Replace with endpoint sample-peak detection only:

```cpp
float peak = std::max(std::fabs(saturatedL), std::fabs(saturatedR));
```

Keep the existing limiter state updates:

```cpp
limiterPrevL = saturatedL;
limiterPrevR = saturatedR;
limiterPrevValid = true;
```

Optional comment:

```cpp
// Linear interpolation cannot exceed endpoint sample peaks; real true-peak
// detection would require reconstruction filtering or proper oversampling.
```

Acceptance:

- Limiter ceiling remains sample-peak safe.
- `limiterGain`, `limiterTriggerEma`, `limiterRecentGrDb`, and LED behavior remain effectively unchanged.
- No bypass behavior changes.

Caution:

- Do not remove `limiterPrevL`, `limiterPrevR`, or `limiterPrevValid` in this pass. Keeping them avoids broad cleanup and preserves future true-peak implementation hooks.
- Do not change `kLimiterOversampleFactor` in this pass unless removing the now-unused constant causes a compile warning policy issue.

---

## 4. Replace Initializer-List `std::max` in Hot Paths

Avoid `std::max({ ... })` in process-path code because it can be less direct than nested max calls.

Current histogram code:

```cpp
float instantPeak = std::max({std::abs(hist.currentMinL), std::abs(hist.currentMaxL), std::abs(hist.currentMinR), std::abs(hist.currentMaxR)});
```

Replace:

```cpp
const float instantPeak = std::max(
	std::max(std::fabs(hist.currentMinL), std::fabs(hist.currentMaxL)),
	std::max(std::fabs(hist.currentMinR), std::fabs(hist.currentMaxR))
);
```

Current spectrum update:

```cpp
maxPow = std::max({maxPow, spec.magnitudesL[i], spec.magnitudesR[i]});
```

Replace:

```cpp
maxPow = std::max(maxPow, std::max(spec.magnitudesL[i], spec.magnitudesR[i]));
```

Acceptance:

- No behavior changes.
- No visual changes.
- Compiles cleanly.

---

## 5. Divide Light Smoothing Updates

Current lights are smoothed every sample. This is unnecessary UI work.

Add member:

```cpp
dsp::ClockDivider lightDivider;
```

Add constant:

```cpp
static constexpr int kLightDivision = 32;
```

Initialize in constructor:

```cpp
lightDivider.setDivision(kLightDivision);
```

Replace per-sample light updates with divided updates:

```cpp
if (lightDivider.process()) {
	const float lightDt = args.sampleTime * float(kLightDivision);
	lights[LIMITER_ACTIVE_LIGHT].setSmoothBrightness(masteringEnabled ? limiterLed : 0.f, lightDt);
	lights[LOW_RECOVERY_LIGHT].setSmoothBrightness(masteringEnabled ? lowRecoveryAmount : 0.f, lightDt);
	lights[IMPACT_AIR_LIGHT].setSmoothBrightness(masteringEnabled ? impactAirLed : 0.f, lightDt);
	lights[REMOVE_MUD_LIGHT].setSmoothBrightness(masteringEnabled ? removeMudLed : 0.f, lightDt);
	lights[GLUE_COMP_LIGHT].setSmoothBrightness(masteringEnabled ? glueLed : 0.f, lightDt);
	lights[STEREO_ENHANCE_LIGHT].setSmoothBrightness(masteringEnabled ? stereoEnhanceLed : 0.f, lightDt);
	lights[SATURATOR_LIGHT].setSmoothBrightness(masteringEnabled ? saturatorLed : 0.f, lightDt);
	lights[MICROPEAK_LIGHT].setSmoothBrightness(0.f, lightDt);
	lights[MASTERING_ENABLED_LIGHT].setSmoothBrightness(masteringEnabled ? 0.5f : 0.f, lightDt);
	lights[REPAIR_ENABLED_LIGHT].setSmoothBrightness(repairEnabled ? 0.5f : 0.f, lightDt);
}
```

Acceptance:

- Light targets remain exactly the same as current code.
- Stage LEDs remain off while mastering is disabled, matching current behavior.
- Graph, spectrum, and waveform updates are unaffected.
- No audio behavior changes.

Caution:

- Do not introduce dimmed bypass LEDs in this pass. That is a UI behavior change and belongs in a later opt-in patch.

---

## 6. Precompute Spectrum Bin Map

Current FFT display update recomputes log-spaced frequency mapping for every spectrum update:

```cpp
float f01 = (float)i / (SPEC_FREQ_BINS - 1);
float hz = 20.f * std::pow(1000.f, f01);
float bin = hz / (sampleRate / FFT_SIZE);
```

Precompute this on construction and sample-rate change.

Add nested/member struct:

```cpp
struct SpectrumBinMapEntry {
	int idx = 0;
	float frac = 0.f;
};

SpectrumBinMapEntry specBinMap[SPEC_FREQ_BINS];
```

Add helper:

```cpp
void updateSpectrumBinMap(float sampleRate) {
	const float sr = std::max(sampleRate, 1.f);
	const float binHz = sr / float(FFT_SIZE);
	for (int i = 0; i < SPEC_FREQ_BINS; ++i) {
		const float f01 = float(i) / float(SPEC_FREQ_BINS - 1);
		const float hz = 20.f * std::pow(1000.f, f01);
		const float bin = hz / binHz;
		const int idx = clamp(int(bin), 0, FFT_SIZE / 2);
		specBinMap[i].idx = idx;
		specBinMap[i].frac = clamp(bin - float(idx), 0.f, 1.f);
	}
}
```

Call from constructor after FFT/window setup:

```cpp
updateSpectrumBinMap(APP->engine->getSampleRate());
```

Call from `onSampleRateChange()`:

```cpp
updateSpectrumBinMap(e.sampleRate);
```

Use in spectrum loop:

```cpp
const int binIdx = specBinMap[i].idx;
const float frac = specBinMap[i].frac;
float powL = 0.f;
float powR = 0.f;
if (binIdx < FFT_SIZE / 2) {
	powL = (1.f - frac) * getMagnitudePow(spec.fftOutL, binIdx) + frac * getMagnitudePow(spec.fftOutL, binIdx + 1);
	powR = (1.f - frac) * getMagnitudePow(spec.fftOutR, binIdx) + frac * getMagnitudePow(spec.fftOutR, binIdx + 1);
}
else {
	powL = getMagnitudePow(spec.fftOutL, FFT_SIZE / 2);
	powR = getMagnitudePow(spec.fftOutR, FFT_SIZE / 2);
}
```

Acceptance:

- Spectrum display remains visually equivalent.
- Spectrum continues updating whether mastering is enabled or disabled.
- No audio behavior changes.

Caution:

- Preserve current frequency range behavior. The current formula maps 20 Hz to 20 kHz because `20 * pow(1000, f01)` is used.
- Do not change `SPEC_FREQ_BINS`, `FFT_SIZE`, smoothing factors, normalization constants, or `specDivider` in this pass.

---

## 7. Avoid Modulo in FFT Input Copy

Current FFT input copy uses modulo per sample:

```cpp
for (int i = 0; i < FFT_SIZE; i++) {
	int idx = (spec.writePtr + i) % FFT_SIZE;
	spec.fftInL[i] = spec.bufferL[idx] * spec.window[i];
	spec.fftInR[i] = spec.bufferR[idx] * spec.window[i];
}
```

Replace with two linear loops that preserve the exact same order.

```cpp
int out = 0;
for (int idx = spec.writePtr; idx < FFT_SIZE; ++idx, ++out) {
	spec.fftInL[out] = spec.bufferL[idx] * spec.window[out];
	spec.fftInR[out] = spec.bufferR[idx] * spec.window[out];
}
for (int idx = 0; idx < spec.writePtr; ++idx, ++out) {
	spec.fftInL[out] = spec.bufferL[idx] * spec.window[out];
	spec.fftInR[out] = spec.bufferR[idx] * spec.window[out];
}
```

Acceptance:

- FFT input order is identical to the modulo version.
- Spectrum display remains visually equivalent.
- Spectrum continues updating whether mastering is enabled or disabled.
- No audio behavior changes.

Caution:

- This is easy to get subtly wrong. Keep it as a separate commit from the spectrum bin-map change if possible.

---

## Explicit Non-Goals For Opt1

Do not implement any of these in this pass:

- `SilProcessingMode`
- `WarmBypass` / `ColdBypass` mode enum
- Any early return from `process()` when mastering is disabled
- Any skip path for Impact Air, Remove Mud, Glue, Stereo Enhance, Saturator, or Limiter
- Any change to which signal feeds histogram or spectrum
- Any change that stops graph/spectrum/waveform updates while mastering is disabled
- Any dimmed bypass LED policy
- Any neutral EQ bypass
- Any coefficient epsilon skip
- Any control-rate movement of compressor/EQ/saturation decisions
- Moving FFT work to another thread
- Reducing `specDivider`
- Reducing histogram draw density
- Reordering the mastering chain

These may be valid future optimizations, but they can change behavior or require more subjective verification.

---

## Definition of Done

This pass is complete when:

- Sil compiles cleanly.
- Mastering enabled output is audibly unchanged.
- Mastering disabled output remains dry input.
- Mastering disabled still runs internal adaptive processing for warm A/B state.
- Histogram/waveform continues updating while mastering is disabled.
- Spectrum continues updating while mastering is disabled.
- Stage LED target behavior remains the same as current code.
- No audio-thread blocking is introduced.
- The micropeak worker cannot survive module destruction.
- CPU is modestly reduced without architectural changes.

Suggested verification:

1. Build the plugin.
2. Feed stereo audio into Sil.
3. Confirm mastering on output behaves normally.
4. Turn mastering off and confirm output is dry input.
5. While mastering is off, confirm histogram/waveform and both spectrum panels continue moving.
6. Re-enable mastering after 10-30 seconds and confirm adaptive state did not wake up cold.
7. Remove/reload module or close Rack under a thread sanitizer/debugger if available to verify worker shutdown safety.
