# Sil Mastering Engine Optimization Guide  
_Target: Codex 5.3 implementation pass_  
_Source: `Sil(4).cpp` uploaded in this conversation_

## 0. Intent

This guide consolidates the performance review for the `Sil` mastering engine while preserving the deliberate mastering-chain order and the current UX goal:

> Users should be able to A/B mastering on/off without the adaptive engine waking up cold.

Therefore, **do not implement a simple hard bypass as the default behavior**.  
Instead, introduce a **Warm A/B Shadow Mode** that keeps detector/adaptive state alive while avoiding expensive audio transformations whose outputs are discarded while mastering is bypassed.

The current chain order is treated as intentional:

```text
Input
→ Low-Band Mono Recovery
→ Impact Air
→ Remove Mud
→ Glue Compressor
→ Stereo Enhance
→ Saturator
→ Final Limiter
→ Output
```

The optimization goal is:

```text
Preserve sonic behavior when enabled.
Preserve adaptive state while bypassed.
Reduce redundant per-sample work.
Move UI/analysis work away from the audio hot path where practical.
```

---

## 1. Core Processing Modes

Add an internal processing mode concept. This lets the module distinguish between:

1. Full mastering output.
2. Warm bypass / shadow analysis.
3. Optional cold bypass for debugging or low-CPU use.

```cpp
enum class SilProcessingMode {
	Active,      // Full chain, output mastered signal.
	WarmBypass,  // Keep adaptive state warm, output dry input.
	ColdBypass   // Cheapest possible bypass, output dry input and skip adaptive updates.
};
```

Default mapping:

```cpp
SilProcessingMode getProcessingMode() const {
	if (masteringEnabled) {
		return SilProcessingMode::Active;
	}

	// Default behavior should remain warm A/B.
	// Optionally make this context-menu configurable later.
	return warmBypassEnabled ? SilProcessingMode::WarmBypass : SilProcessingMode::ColdBypass;
}
```

Add a module member:

```cpp
bool warmBypassEnabled = true;
```

Optional context menu later:

```text
Bypass Behavior
  ✓ Warm A/B State
    Cold CPU Bypass
```

---

## 2. Priority Summary

Implement in this order.

| Priority | Optimization | Expected Payoff | Sonic Risk |
|---:|---|---:|---:|
| 1 | Add WarmBypass mode instead of full hidden rendering | High | Low/Medium depending on shadow approximation |
| 2 | Remove redundant limiter linear oversample loop | Medium | Low if no real true-peak detector exists |
| 3 | Replace rolling RMS scan with O(1) rolling RMS | Medium | Low |
| 4 | Skip final EQ / waveshaper / limiter application in WarmBypass | High | Low if state updates remain |
| 5 | Move detector decision math to control-rate | Medium/High | Medium; tune divider carefully |
| 6 | Add neutral-bypass for inactive EQ stages | Medium | Low with hysteresis |
| 7 | Reduce light smoothing and UI work rate | Low/Medium | None |
| 8 | Move or reduce spectrum FFT work | Medium if module UI visible | None to audio |
| 9 | Cache biquad coefficient design constants | Low/Medium | Low |
| 10 | Optimize histogram/spectrum widget drawing | Medium UI payoff | None |

---

## 3. WarmBypass Architecture

### 3.1 Principle

When mastering is disabled, do not throw away all adaptive data.

Keep these alive:

- detector envelopes
- smoothed target gains
- rolling peak/RMS history
- saturator peak percentile history
- limiter engagement prediction
- LEDs/meters, if desired

Skip these if their output is not needed:

- final corrective EQ processing
- saturation waveshaping
- limiter gain application
- high-rate visual work
- unnecessary `pow`, `log10`, `sqrt`, `sin`, `cos` in per-sample paths

### 3.2 Stage pattern

Refactor stage code toward this conceptual shape:

```cpp
void analyzeStage(...);
void processStage(...);
void processStageShadowEstimate(...); // optional
```

Active mode:

```cpp
analyzeStage(...);
processStage(...);
```

WarmBypass mode:

```cpp
analyzeStage(...);
// Skip expensive output rendering unless later analysis depends on it.
// Output dry input at the end.
```

ColdBypass mode:

```cpp
outputs[OUTPUT_L_OUTPUT].setVoltage(inL);
outputs[OUTPUT_R_OUTPUT].setVoltage(inR);
return;
```

---

## 4. Stage-by-Stage Optimization Plan

## 4.1 Input and Bypass Mode Selection

At the top of `process()`:

```cpp
void process(const ProcessArgs& args) override {
	masteringEnabled = params[MASTERING_ENABLED_PARAM].getValue() > 0.5f;
	repairEnabled = params[REPAIR_ENABLED_PARAM].getValue() > 0.5f;

	const float inL = inputs[INPUT_L_INPUT].getVoltage();
	const float inR = inputs[INPUT_R_INPUT].getVoltage();

	const SilProcessingMode mode = getProcessingMode();

	if (mode == SilProcessingMode::ColdBypass) {
		outputs[OUTPUT_L_OUTPUT].setChannels(1);
		outputs[OUTPUT_R_OUTPUT].setChannels(1);
		outputs[OUTPUT_L_OUTPUT].setVoltage(inL);
		outputs[OUTPUT_R_OUTPUT].setVoltage(inR);

		updateBypassLightsOnly(args);
		return;
	}

	// Existing pipeline continues for Active and WarmBypass.
}
```

Acceptance:

- ColdBypass outputs dry signal.
- WarmBypass outputs dry signal but state continues updating.
- Active outputs mastered signal.
- Switching WarmBypass → Active should not cause obvious pumping, dead limiter state, or saturator wake-up jumps.

---

## 4.2 Low-Band Mono Recovery

Current behavior uses two lowpass stages per channel, then adaptive low-side collapse based on low-band L/R correlation.

State to keep warm:

```cpp
lowBandCorrLL
lowBandCorrRR
lowBandCorrLR
lowBandSideGain
```

Recommendation:

- Keep this stage running in Active and WarmBypass.
- It is early in the chain and feeds downstream analysis.
- Do not optimize this first unless profiling says it dominates.

Possible micro-optimizations:

- Avoid recomputing values not used in WarmBypass if later shadow stages are decoupled.
- Consider a lower-rate correlation update later, but keep the filters audio-rate.

Acceptance:

- Low recovery LED behavior remains similar.
- Sub-bass mono recovery engages identically or near-identically when Active.

---

## 4.3 Impact Air

Current work:

- Low transient detector from `lowMid`.
- Fast and slow envelope.
- dB ratio / soft knee.
- Adaptive high shelf coefficient update.
- Biquad high shelf process on L/R.

State to keep warm:

```cpp
impactAir.env
impactAir.slowEnv
impactAir.targetLiftDb
impactAir.smoothedLiftDb
impactAir.ledAmount
```

WarmBypass optimization:

```cpp
analyzeImpactAir(lowMid, args.sampleRate);

if (mode == SilProcessingMode::Active) {
	impactAirL = impactAir.shelfL.process(recoveredL);
	impactAirR = impactAir.shelfR.process(recoveredR);
}
else {
	// Shadow mode: keep the detector and smoothed gain warm,
	// but do not render the shelf output if downstream shadow analysis
	// can tolerate using recoveredL/R.
	impactAirL = recoveredL;
	impactAirR = recoveredR;
}
```

Recommended helper:

```cpp
void analyzeImpactAir(float lowMid, float sampleRate) {
	const float detector = std::fabs(lowMid);

	const float envCoeff = (detector > impactAir.env)
		? impactAirEnvAttackCoeff
		: impactAirEnvReleaseCoeff;
	impactAir.env = detector + envCoeff * (impactAir.env - detector);

	const float slowCoeff = (detector > impactAir.slowEnv)
		? impactAirSlowAttackCoeff
		: impactAirSlowReleaseCoeff;
	impactAir.slowEnv = detector + slowCoeff * (impactAir.slowEnv - detector);

	// This dB decision can later be moved to a control divider.
	const float transientDeltaDb =
		toDbSafe(impactAir.env / std::max(impactAir.slowEnv, kImpactAirSlowFloorVolts));

	const float transientGate =
		softKnee01(transientDeltaDb, kImpactAirTransientThresholdDb, kImpactAirTransientKneeDb);

	impactAir.targetLiftDb = kImpactAirMaxLiftDb * transientGate;

	const float gainCoeff = (impactAir.targetLiftDb > impactAir.smoothedLiftDb)
		? impactAirGainAttackCoeff
		: impactAirGainReleaseCoeff;

	impactAir.smoothedLiftDb =
		impactAir.targetLiftDb + gainCoeff * (impactAir.smoothedLiftDb - impactAir.targetLiftDb);

	impactAir.ledAmount =
		clamp(impactAir.smoothedLiftDb / std::max(kImpactAirMaxLiftDb, 1e-6f), 0.f, 1.f);

	if (impactAir.coeffDivider.process()) {
		impactAir.shelfL.setHighShelf(sampleRate, kImpactAirShelfHz, kImpactAirShelfQ, impactAir.smoothedLiftDb);
		impactAir.shelfR.setHighShelf(sampleRate, kImpactAirShelfHz, kImpactAirShelfQ, impactAir.smoothedLiftDb);
	}
}
```

Future optimization:

- Move `transientDeltaDb`, `softKnee01`, and shelf coefficient update to a 16–32 sample control divider.
- Keep envelopes sample-rate if transient response matters.

Acceptance:

- Impact Air LED behaves naturally in bypass.
- Re-enabling mastering does not cause shelf coefficient discontinuity.
- Active-mode sound remains unchanged.

---

## 4.4 Remove Mud

Current work:

- Mono extraction.
- Three band detectors: mud, bass, presence.
- Envelope followers.
- `toDbSafe()` comparisons.
- Adaptive peaking EQ coefficient update.
- L/R peaking EQ process.

State to keep warm:

```cpp
removeMud.mudEnv
removeMud.bassEnv
removeMud.presenceEnv
removeMud.targetCutDb
removeMud.smoothedCutDb
removeMud.ledAmount
```

WarmBypass optimization:

```cpp
analyzeRemoveMud(impactAirL, impactAirR, args.sampleRate);

if (mode == SilProcessingMode::Active) {
	mudCleanL = removeMud.peakingL.process(impactAirL);
	mudCleanR = removeMud.peakingR.process(impactAirR);
}
else {
	// Detector remains warm; output processing is skipped.
	mudCleanL = impactAirL;
	mudCleanR = impactAirR;
}
```

This is one of the safest savings because the peaking EQ output is discarded when bypassed.

Future optimization:

- Move the dB ratio and target-cut decision to a 32 or 64 sample divider.
- Keep bandpass-ish RC detector filters audio-rate.
- Only update biquad coefficients if gain changed by more than a small epsilon.

Suggested coefficient epsilon:

```cpp
static constexpr float kCoeffGainEpsilonDb = 0.005f;
```

Acceptance:

- Remove Mud LED continues to show activation during bypass.
- When mastering is re-enabled, the peaking EQ amount is already near the expected value.
- No large first-sample jump after enabling.

---

## 4.5 Rolling Program RMS

Current behavior estimates recent program level by scanning up to 2048 samples periodically.

Replace with O(1) rolling RMS accumulation.

Add member:

```cpp
double rollingMonoSqSum = 0.0;
```

Update reset/configuration:

```cpp
void configureRollingBuffer(float sampleRate) {
	const int requestedLength =
		std::max(1, int(std::round(sampleRate * kRollingBufferSeconds)));

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
		const float oldMono =
			0.5f * (rollingBufferL[size_t(idx)] + rollingBufferR[size_t(idx)]);
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
	if (rollingFilled <= 0) {
		return -100.f;
	}

	const float rmsVolts =
		std::sqrt(float(std::max(0.0, rollingMonoSqSum) / double(rollingFilled)));

	return toDbFsSafe(rmsVolts);
}
```

Acceptance:

- Glue adaptive threshold remains stable.
- CPU spikes from periodic rolling scans disappear.
- Threshold behavior should be at least as stable as before because it uses the full rolling window rather than a sparse scan.

---

## 4.6 Glue Compressor

Current work:

- Sidechain highpass.
- RMS envelope.
- Per-sample dB level calculation.
- Soft-knee compression decision.
- Per-sample `pow(10, totalGainDb / 20)`.
- L/R gain application.

State to keep warm:

```cpp
glue.rmsEnv
glue.gainReductionDb
glue.makeupDb
glue.ledAmount
glueAdaptiveThresholdDb
```

WarmBypass optimization:

```cpp
analyzeGlue(cleaned.l, cleaned.r, args.sampleRate);

if (mode == SilProcessingMode::Active) {
	gluedL = cleaned.l * glue.gainLinear;
	gluedR = cleaned.r * glue.gainLinear;
}
else {
	// State remains warm, but output gain application can be skipped.
	gluedL = cleaned.l;
	gluedR = cleaned.r;
}
```

Add cached linear gain:

```cpp
float glueGainLinear = 1.f;
```

Update it only when the gain decision updates, ideally at control rate.

Suggested control-rate divider:

```cpp
dsp::ClockDivider glueDecisionDivider;
static constexpr int kGlueDecisionDivision = 16; // or 32 after listening
```

Then:

```cpp
// Audio-rate:
glue.sidechainHp.process(glueMono);
const float sidechain = glue.sidechainHp.highpass();
const float rmsTarget = sidechain * sidechain;
glue.rmsEnv = glueRmsCoeff * glue.rmsEnv + (1.f - glueRmsCoeff) * rmsTarget;

// Control-rate:
if (glueDecisionDivider.process()) {
	updateGlueGainDecision();
}
```

Where `updateGlueGainDecision()` performs:

- `sqrt`
- `log10`
- soft-knee math
- gain-reduction smoothing
- `pow` to linear

Acceptance:

- Glue behavior remains musically equivalent.
- No audible stepping at `kGlueDecisionDivision = 16`.
- If stepping is audible, reduce division to 8 or smooth `glueGainLinear`.

---

## 4.7 Stereo Enhance

Current work:

- M/S conversion.
- Four detector bands.
- Four envelopes.
- Multiple dB/gate decisions.
- Adaptive Mid EQ and Side EQ coefficient updates.
- Always processes `midEq` and `sideEq`, even when neutral.

State to keep warm:

```cpp
stereoEnhance.mid350Env
stereoEnhance.midBroadEnv
stereoEnhance.side6kEnv
stereoEnhance.sideBroadEnv
stereoEnhance.targetMidCutDb
stereoEnhance.smoothedMidCutDb
stereoEnhance.targetSideLiftDb
stereoEnhance.smoothedSideLiftDb
stereoEnhance.midActivation
stereoEnhance.sideActivation
stereoEnhance.ledAmount
stereoEnhance.coeffsNeutral
```

WarmBypass optimization:

```cpp
analyzeStereoEnhance(gluedL, gluedR, args.sampleRate);

if (mode == SilProcessingMode::Active && !stereoEnhance.coeffsNeutral) {
	const float mid = 0.5f * (gluedL + gluedR);
	const float side = 0.5f * (gluedL - gluedR);

	const float enhancedMid = stereoEnhance.midEq.process(mid);
	const float enhancedSide = stereoEnhance.sideEq.process(side);

	enhancedL = enhancedMid + enhancedSide;
	enhancedR = enhancedMid - enhancedSide;
}
else {
	enhancedL = gluedL;
	enhancedR = gluedR;
}
```

Important:

- Keep detector filters warm in WarmBypass.
- Skip final Mid/Side EQ processing in WarmBypass.
- Skip final EQ processing in Active if both EQ amounts are neutral.

Use hysteresis for neutral state:

```cpp
bool updateNeutralBypass(bool currentActive, float midDb, float sideDb) {
	const float amount = std::max(std::fabs(midDb), std::fabs(sideDb));

	if (currentActive) {
		return amount > 0.0005f;
	}
	else {
		return amount > 0.002f;
	}
}
```

Consider replacing `coeffsNeutral` with:

```cpp
bool stereoEqActive = false;
```

Acceptance:

- Stereo Enhance LED works in bypass.
- Re-enabling mastering has the expected adaptive Mid cut / Side lift already prepared.
- Active sound is unchanged when EQ is active.
- CPU drops when stereo EQ amounts are neutral or mastering is bypassed.

---

## 4.8 Saturator

Current work:

- Peak history binning.
- Percentile histogram.
- Periodic drive/makeup adaptation.
- Per-sample waveshaping using `fastAtanApprox`.
- Near-neutral skip already exists.

State to keep warm:

```cpp
saturator.currentBinPeak
saturator.percentileHist
saturator.binToHist
saturator.drive
saturator.makeupDb
saturator.makeupLinear
saturator.driveNormInv
saturator.limiterEngagement
saturator.limiterRecentGrDb
saturator.ledAmount
```

WarmBypass optimization:

```cpp
updateSaturatorAnalysis(enhancedL, enhancedR, args.sampleRate);

if (mode == SilProcessingMode::Active) {
	processSaturatorAudio(enhancedL, enhancedR, saturatedL, saturatedR);
}
else {
	// Keep drive/makeup adaptation warm, but do not waveshape.
	saturatedL = enhancedL;
	saturatedR = enhancedR;
}
```

The current near-neutral skip should remain:

```cpp
const bool nearNeutralSat = (drive <= 1.01f && makeup <= 1.01f);
```

Suggested refinement:

- In WarmBypass, skip `shape()` always.
- Keep peak histogram and drive/makeup updates.
- Saturator adaptation should use the best available shadow signal. If earlier stages are approximated in bypass, document this as acceptable unless A/B tests reveal wake-up mismatch.

Acceptance:

- Saturator LED and drive/makeup continue adapting during bypass.
- No per-sample `fastAtanApprox` calls when bypassed.
- Re-enable state does not start from zero or stale drive.

---

## 4.9 Limiter

Current limiter includes a linear interpolation loop described as oversampling:

```cpp
for (int i = 1; i <= kLimiterOversampleFactor; ++i) {
	const float a = float(i) / float(kLimiterOversampleFactor);
	const float interpL = limiterPrevL + (saturatedL - limiterPrevL) * a;
	const float interpR = limiterPrevR + (saturatedR - limiterPrevR) * a;
	peak = std::max(peak, std::max(std::fabs(interpL), std::fabs(interpR)));
}
```

This does **not** provide true-peak detection. A linear interpolation between two scalar samples cannot exceed the greater absolute endpoint unless a real reconstruction filter is used. Therefore this loop is mostly redundant cost.

Replace with endpoint peak detection:

```cpp
float peak = std::max(std::fabs(saturatedL), std::fabs(saturatedR));
```

If true-peak behavior is desired later, implement an actual oversampled detector using a proper FIR/polyphase reconstruction approximation.

WarmBypass limiter prediction:

```cpp
updateLimiterPrediction(saturatedL, saturatedR, args.sampleRate);

if (mode == SilProcessingMode::Active) {
	outL = saturatedL * limiterGain;
	outR = saturatedR * limiterGain;
}
else {
	outL = inL;
	outR = inR;
}
```

State to keep warm:

```cpp
limiterGain
limiterTriggerEma
limiterRecentGrDb
limiterPrevL
limiterPrevR
limiterPrevValid
```

Possible design choice:

- In WarmBypass, update limiter state from the predicted shadow post-saturator signal.
- Output dry input.
- This keeps saturator adaptation informed by limiter engagement.

Acceptance:

- Limiter LED / GR state remains meaningful during bypass.
- Saturator sees limiter engagement history.
- Removing the linear oversample loop does not increase sample-peak overs above the current limiter ceiling.
- If true-peak accuracy is later required, implement it explicitly rather than relying on linear interpolation.

---

## 5. Control-Rate Decision Updates

Many adaptive decisions do not need to run every sample.

Keep audio-rate:

- filters used as detectors
- envelope followers when fast response matters
- final gain multiplication
- final EQ/waveshaping when active

Move to control-rate:

- dB conversions
- soft-knee decisions
- LED calculations
- coefficient design
- `pow(10, db / 20)` gain conversion
- spectrum normalization

Suggested dividers:

| Section | Initial Divider | Notes |
|---|---:|---|
| Impact Air decision | 16 or 32 | Fast transient stage; start conservative |
| Remove Mud decision | 32 or 64 | Slow enough for lower rate |
| Glue gain decision | 16 or 32 | Check for zippering |
| Stereo Enhance decision | 32 or 64 | Slow adaptive EQ |
| Saturator adaptation | Already 512 | Keep existing unless too sluggish |
| Limiter LED metrics | 16 or 32 | Limiter gain itself remains audio-rate if active |
| Lights | 32 | Use accumulated sample time |

Pattern:

```cpp
if (decisionDivider.process()) {
	updateDecisionHeavyMath();
}
```

If gain values are updated at control rate, smooth the resulting linear gain to avoid zippering:

```cpp
gainLinear = targetGainLinear + coeff * (gainLinear - targetGainLinear);
```

Acceptance:

- No audible stepping.
- LED/meter response remains readable.
- CPU use decreases measurably in profiler.

---

## 6. Neutral EQ Bypass

Add active flags to adaptive EQ stages.

Recommended flags:

```cpp
bool impactAirShelfActive = false;
bool removeMudEqActive = false;
bool stereoEqActive = false;
```

Use hysteresis:

```cpp
static bool updateDbActiveFlag(bool wasActive, float amountDb) {
	const float absDb = std::fabs(amountDb);
	const float offThreshold = 0.0005f;
	const float onThreshold = 0.0020f;
	return wasActive ? (absDb > offThreshold) : (absDb > onThreshold);
}
```

Usage:

```cpp
impactAirShelfActive =
	updateDbActiveFlag(impactAirShelfActive, impactAir.smoothedLiftDb);

if (mode == SilProcessingMode::Active && impactAirShelfActive) {
	impactAirL = impactAir.shelfL.process(recoveredL);
	impactAirR = impactAir.shelfR.process(recoveredR);
}
else {
	impactAirL = recoveredL;
	impactAirR = recoveredR;
}
```

Same idea for Remove Mud and Stereo Enhance.

Acceptance:

- No clicks when a stage activates/deactivates.
- Consider resetting inactive biquad states only after a short inactive hold, not instantly, to avoid re-entry discontinuities.
- Neutral sections consume less CPU.

---

## 7. Biquad Coefficient Caching

Current `setPeaking()` and `setHighShelf()` recompute:

- `pow`
- `sin`
- `cos`
- sometimes `sqrt`

The update is already divided, which is good. Further optimize by caching frequency/Q/sample-rate terms.

Add:

```cpp
struct BiquadDesignCache {
	float sampleRate = 0.f;
	float freq = 0.f;
	float q = 0.f;
	float c = 1.f;
	float s = 0.f;
	float alpha = 0.f;

	void update(float sr, float hz, float qIn) {
		if (sr == sampleRate && hz == freq && qIn == q) {
			return;
		}

		sampleRate = sr;
		freq = hz;
		q = qIn;

		const float nyquistGuard = 0.48f * sr;
		const float fc = clamp(hz, 10.f, nyquistGuard);
		const float w0 = 2.f * M_PI * fc / sr;

		c = std::cos(w0);
		s = std::sin(w0);
		alpha = s / (2.f * q);
	}
};
```

Then create cached coefficient setters for fixed-frequency adaptive EQs.

Also skip coefficient recalculation if gain has not changed enough:

```cpp
if (std::fabs(newGainDb - lastGainDb) > kCoeffGainEpsilonDb) {
	setPeakingCached(...);
	lastGainDb = newGainDb;
}
```

Acceptance:

- Coefficients match existing implementation within tiny floating-point tolerance.
- No audible change.
- Coefficient design work drops in profiling.

---

## 8. Spectrum FFT Optimization

Current spectrum system is UI-only but runs from the audio process path at a divider.

Recommendations:

### 8.1 Precompute spectrum bin map

Current display-bin mapping likely recomputes log-spaced frequency mapping each FFT update. Precompute on sample-rate change.

```cpp
struct SpecBinMap {
	int idx = 0;
	float frac = 0.f;
};

SpecBinMap specMap[SPEC_FREQ_BINS];

void updateSpecBinMap(float sampleRate) {
	for (int i = 0; i < SPEC_FREQ_BINS; ++i) {
		const float f01 = float(i) / float(SPEC_FREQ_BINS - 1);
		const float hz = 20.f * std::pow(1000.f, f01);
		const float bin = hz / (sampleRate / float(FFT_SIZE));

		const int idx = clamp(int(bin), 0, FFT_SIZE / 2);
		specMap[i].idx = idx;
		specMap[i].frac = bin - float(idx);
	}
}
```

Then FFT display loop uses `specMap[i]`.

### 8.2 Avoid modulo in FFT input copy

Instead of:

```cpp
const int idx = (spec.writePtr + i) % FFT_SIZE;
```

Use two linear copies:

```cpp
const int firstCount = FFT_SIZE - spec.writePtr;
const int secondCount = spec.writePtr;

// Copy writePtr..end
// Copy 0..writePtr
```

Apply window during copy.

### 8.3 Consider moving FFT out of audio thread

Best design:

- Audio thread writes ring buffer or decimated samples.
- UI/worker thread consumes snapshots.
- Audio thread never performs FFT.

If too large a refactor, at least:

- increase divider to 4096 or 8192
- precompute bin map
- avoid repeated expensive display math

Acceptance:

- No audible behavior change.
- UI spectrum remains responsive enough.
- Audio thread CPU spikes reduce.

---

## 9. Light and Meter Update Rate

Current lights appear to be smoothed every sample. This is unnecessary UI work.

Add:

```cpp
dsp::ClockDivider lightDivider;
static constexpr int kLightDivision = 32;
```

Initialize:

```cpp
lightDivider.setDivision(kLightDivision);
```

Use:

```cpp
if (lightDivider.process()) {
	const float dt = args.sampleTime * float(kLightDivision);

	lights[LIMITER_ACTIVE_LIGHT].setSmoothBrightness(masteringEnabled ? limiterLed : 0.f, dt);
	lights[LOW_RECOVERY_LIGHT].setSmoothBrightness(masteringEnabled ? lowRecoveryAmount : 0.f, dt);
	lights[IMPACT_AIR_LIGHT].setSmoothBrightness(masteringEnabled ? impactAirLed : 0.f, dt);
	lights[REMOVE_MUD_LIGHT].setSmoothBrightness(masteringEnabled ? removeMudLed : 0.f, dt);
	lights[GLUE_COMP_LIGHT].setSmoothBrightness(masteringEnabled ? glueLed : 0.f, dt);
	lights[SATURATOR_LIGHT].setSmoothBrightness(masteringEnabled ? saturatorLed : 0.f, dt);
	lights[STEREO_ENHANCE_LIGHT].setSmoothBrightness(masteringEnabled ? stereoEnhanceLed : 0.f, dt);
	lights[MICROPEAK_LIGHT].setSmoothBrightness(repairEnabled ? micropeakLed : 0.f, dt);
	lights[MASTERING_ENABLED_LIGHT].setSmoothBrightness(masteringEnabled ? 0.5f : 0.f, dt);
	lights[REPAIR_ENABLED_LIGHT].setSmoothBrightness(repairEnabled ? 0.5f : 0.f, dt);
}
```

Decision point:

- In WarmBypass, decide whether stage LEDs should show “what mastering would do” or dim because mastering is off.
- Recommended: keep adaptive LEDs visible at lower brightness in bypass so the user can see the dragon breathing.

Example:

```cpp
const float masterUiScale = masteringEnabled ? 1.f : 0.35f;
```

Acceptance:

- Lights still look smooth.
- CPU and UI overhead decrease slightly.
- Bypass state is visually clear.

---

## 10. Histogram Optimization

Current histogram keeps 1000 bins for audio history and the widget may draw many vertical paths.

Audio-side:

- Current bin accumulation is probably acceptable.
- Avoid expensive initializer-list `std::max({ ... })` in any hot path.

Replace:

```cpp
std::max({a, b, c, d})
```

with:

```cpp
std::max(std::max(a, b), std::max(c, d))
```

UI-side:

- Do not draw all 1000 bins if widget width is much smaller.
- Draw one column per screen pixel or per 0.5/1.0 px.
- Batch paths when possible.

Suggested draw density:

```cpp
const int columns = std::max(1, int(box.size.x));
for (int x = 0; x < columns; ++x) {
	// Aggregate one or more histogram bins into this screen column.
}
```

Acceptance:

- Histogram looks visually equivalent or cleaner.
- UI framerate improves when module is visible.

---

## 11. Micropeak Worker Review

The file contains a substantial micropeak worker implementation. If this is not currently wired into `process()`, it is not a direct hot-path cost, but it increases complexity.

Recommendations:

1. If repair is not active yet:
   - keep worker dormant
   - make sure no thread is started unnecessarily
   - make `repairEnabled == false` skip all micropeak push/analyze work

2. If repair is active:
   - keep chunk analysis off the audio thread
   - audio thread should only fill a chunk and try-lock publish it
   - avoid blocking or waiting from audio thread

3. Ensure destructor stops worker:

```cpp
~Sil() {
	stopMicropeakWorker();
	delete spec.fft;
}
```

If the worker can be started, the destructor should always stop/join it before module destruction.

Acceptance:

- No audio-thread blocking.
- No worker thread survives module destruction.
- Repair disabled means minimal repair overhead.

---

## 12. Suggested Refactor Skeleton

The goal is not to rewrite the module into many files. Keep the one-file architecture if preferred, but introduce stage helpers inside `Sil`.

```cpp
struct StageFrame {
	float inL = 0.f;
	float inR = 0.f;

	float lowRecoveredL = 0.f;
	float lowRecoveredR = 0.f;

	float airL = 0.f;
	float airR = 0.f;

	float mudL = 0.f;
	float mudR = 0.f;

	float glueL = 0.f;
	float glueR = 0.f;

	float enhancedL = 0.f;
	float enhancedR = 0.f;

	float saturatedL = 0.f;
	float saturatedR = 0.f;

	float outL = 0.f;
	float outR = 0.f;
};
```

Then process becomes more readable:

```cpp
void process(const ProcessArgs& args) override {
	readParams();

	StageFrame f;
	f.inL = inputs[INPUT_L_INPUT].getVoltage();
	f.inR = inputs[INPUT_R_INPUT].getVoltage();

	const SilProcessingMode mode = getProcessingMode();

	if (mode == SilProcessingMode::ColdBypass) {
		writeDryOutput(f);
		updateColdBypassUi(args);
		return;
	}

	processLowRecovery(f, args, mode);
	processImpactAir(f, args, mode);
	processRemoveMud(f, args, mode);
	processGlue(f, args, mode);
	processStereoEnhance(f, args, mode);
	processSaturator(f, args, mode);
	processLimiter(f, args, mode);

	if (mode == SilProcessingMode::Active) {
		writeMasteredOutput(f);
	}
	else {
		writeDryOutput(f);
	}

	updateMetersAndLights(f, args, mode);
}
```

This keeps the chain order explicit while making mode-specific optimization clean.

---

## 13. Testing and Acceptance Criteria

## 13.1 Functional Tests

### Active mode

- Output remains sonically equivalent to the current implementation.
- Chain order remains unchanged.
- No stage is skipped while active unless its gain is neutral.
- Limiter ceiling remains correct for sample peak.

### WarmBypass mode

- Output is dry input.
- Adaptive LEDs/meters continue responding.
- Saturator drive/makeup continues adapting.
- Glue threshold continues adapting.
- Limiter engagement prediction continues adapting.
- Re-enabling mastering after 10–30 seconds of bypass produces a stable, already-tuned chain.

### ColdBypass mode

- Output is dry input.
- CPU is lowest.
- Adaptive state may be stale by design.
- Clearly documented as optional behavior.

---

## 13.2 A/B Tests

Use these signals:

1. Full mix from Suno / AI-generated track.
2. Human-mastered reference track.
3. Dense bass-heavy material.
4. Bright stereo material.
5. Mono low-frequency sweep.
6. Fast transient drum loop.
7. Silence to loud transition.

Test sequence:

```text
Play material with mastering active for 15 seconds.
Bypass for 15 seconds in WarmBypass.
Re-enable mastering.
Listen for jump, pump, or stale-state behavior.
Repeat with ColdBypass and confirm difference is expected.
```

Acceptance:

- WarmBypass re-entry should feel substantially smoother than ColdBypass.
- No large limiter or saturator jump on re-enable.
- No obvious EQ coefficient discontinuity.

---

## 13.3 CPU Profiling

Measure CPU in Rack under four conditions:

1. Active, module visible.
2. Active, module hidden or offscreen.
3. WarmBypass, module visible.
4. ColdBypass, module visible.

Expected outcome:

```text
ColdBypass < WarmBypass < Active
```

WarmBypass should be meaningfully cheaper than Active.

Also compare before/after:

- limiter loop removal
- rolling RMS O(1)
- light divider
- neutral EQ bypass
- spectrum precompute/reduction

---

## 14. Specific Codex Tasks

Give Codex these as separate commits/tasks.

### Task 1 — Add processing mode

- Add `SilProcessingMode`.
- Add `warmBypassEnabled`.
- Implement ColdBypass early return.
- Preserve current default behavior as WarmBypass.

### Task 2 — Split stage analysis from stage rendering

- Refactor Impact Air, Remove Mud, Stereo Enhance, Saturator, Limiter into helper methods.
- Keep chain order exactly the same.
- Do not change constants or sonic parameters.

### Task 3 — Implement WarmBypass skip paths

In WarmBypass:

- Keep detector/adaptive state updates.
- Skip final Impact Air shelf rendering if acceptable.
- Skip Remove Mud peaking EQ rendering.
- Skip Stereo Enhance final M/S EQ rendering.
- Skip Saturator waveshaping.
- Skip Limiter output gain application.
- Output dry input.

### Task 4 — Remove limiter linear oversample loop

- Replace linear interpolation loop with endpoint peak.
- Leave a comment explaining that true-peak requires real reconstruction/oversampling.
- Preserve limiter state and LED behavior.

### Task 5 — O(1) rolling RMS

- Add `rollingMonoSqSum`.
- Update `pushRollingSample`.
- Replace scan-based `estimateRollingProgramDbFs`.

### Task 6 — Control-rate heavy math

- Add dividers for glue, mud, stereo enhance, and optionally impact air decisions.
- Move `log10`, `sqrt`, `pow`, soft-knee target decisions into divided update blocks.
- Smooth any control-rate gain values.

### Task 7 — Neutral EQ bypass

- Add active flags with hysteresis.
- Bypass neutral Impact Air shelf, Remove Mud peaking EQ, and Stereo Enhance EQ.
- Avoid clicks on activation/deactivation.

### Task 8 — UI/meters

- Update lights at divided rate.
- Consider lower brightness for stage LEDs while WarmBypass is active.
- Precompute spectrum bin map.
- Reduce histogram draw density.

### Task 9 — Micropeak lifecycle

- Ensure worker starts only when needed.
- Ensure destructor calls `stopMicropeakWorker()` before deleting resources.
- Ensure repair disabled path has minimal overhead.

---

## 15. Implementation Notes and Cautions

### Do not accidentally change chain order

The optimization pass should not reorder the mastering stages. It should only separate:

```text
state analysis
from
audible rendering
```

### Do not replace WarmBypass with ColdBypass

A cold bypass is useful as an option, but not as the default. The user-facing A/B behavior depends on adaptive memory.

### Be careful with control-rate gain updates

Moving a decision to control-rate is safe only if the resulting parameter is smoothed or inherently slow. If zippering appears, lower the divider or smooth the linear gain.

### Be careful with filter state resets

Resetting an inactive biquad every sample can cause a discontinuity when it becomes active again. Prefer either:

- leave state alone while inactive, or
- reset only after a hold time, or
- crossfade activation if needed.

### True peak should be explicit

The current linear interpolation limiter loop should not be treated as real true-peak detection. Either use sample-peak limiting honestly or implement a real true-peak approximation later.

---

## 16. Definition of Done

The optimization pass is complete when:

- Active-mode sound is equivalent to the current implementation.
- WarmBypass preserves adaptive state and gives smooth A/B re-entry.
- ColdBypass optionally provides a very low CPU path.
- CPU is measurably reduced in WarmBypass compared with current bypass behavior.
- CPU is measurably reduced in Active mode from:
  - limiter loop removal
  - O(1) rolling RMS
  - neutral EQ bypass
  - reduced UI/light work
  - reduced spectrum overhead
- No audio-thread blocking is introduced.
- No worker-thread lifecycle issues are introduced.
- Code remains readable enough for future mastering-stage additions.
