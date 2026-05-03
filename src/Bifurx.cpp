#include "Bifurx.hpp"

namespace bifurx {

const char* const kBifurxModeLabels[kBifurxModeCount] = {
	"Low + Low",
	"Low + Band",
	"Notch + Low",
	"Notch + Notch",
	"Low + High",
	"Band + Band",
	"High + Low",
	"High + Notch",
	"Band + High",
	"High + High",
	"Resample"
};

std::string bifurxUserRootPath() {
	return system::join(asset::user(), "Leviathan/Bifurx");
}

constexpr float kSelfOscResoStart = 0.80f;
constexpr float kSelfOscResoFull = 0.98f;
constexpr float kSelfOscHeatStart = 0.90f;
constexpr float kSelfOscPush = 0.120f;
constexpr float kSelfOscAmpDampingClean = 0.060f;
constexpr float kSelfOscAmpDampingHot = 0.120f;
constexpr float kSvfSelfOscDampingMin = 0.0005f;

float levelDriveGain(float knob) {
	const float x = bifurx::clamp01(knob);
	// Midpoint should be exactly unity so the default LEVEL setting is neutral.
	return 0.075f + 0.95f * x + 3.6f * x * x * x;
}

float smoothstep01(float x) {
	const float t = bifurx::clamp01(x);
	return t * t * (3.f - 2.f * t);
}

float levelInputGain(float knob) {
	const float x = bifurx::clamp01(knob);
	if (x <= 0.5f) {
		return 2.f * x;
	}
	const float hot = 2.f * (x - 0.5f);
	return 1.f + 2.5f * hot * hot;
}

float levelDriveAmount(float knob) {
	const float x = bifurx::clamp01(knob);
	constexpr float kLevelDriveStart = 0.62f;
	if (x <= kLevelDriveStart) {
		return 0.f;
	}
	const float hot = bifurx::clamp01((x - kLevelDriveStart) / (1.f - kLevelDriveStart));
	return hot * hot;
}

float levelOutputClipWet(float knob) {
	(void) knob;
	return 0.f;
}

float levelOutputMakeupGain(float knob) {
	(void) knob;
	return 1.f;
}

float applyLevelInputStage(float in, float levelKnob) {
	constexpr float kLevelMaxDriveGain = 2.5f;
	const float clean = in * levelInputGain(levelKnob);
	const float driveAmount = levelDriveAmount(levelKnob);
	if (driveAmount <= 1e-5f) {
		return clean;
	}
	const float driveGain = 1.f + (kLevelMaxDriveGain - 1.f) * driveAmount;
	const float driven = 5.f * bifurx::softClip((clean * driveGain) / 5.f);
	return bifurx::mixf(clean, driven, driveAmount);
}

float applyLevelOutputStage(float modeOut, float levelKnob, bool softLimitingEnabled) {
	(void) levelKnob;
	const float out = bifurx::sanitizeFinite(modeOut);
	if (!softLimitingEnabled) {
		return out;
	}
	constexpr float kSoftLimitVolts = 10.f;
	return kSoftLimitVolts * bifurx::softClip(out / kSoftLimitVolts);
}

float onePoleAlpha(float dt, float tauSeconds) {
	if (tauSeconds <= 0.f) {
		return 1.f;
	}
	return 1.f - fastExp(-std::max(dt, 0.f) / tauSeconds);
}

float logPosition(float hz, float minHz, float maxHz) {
	const float safeHz = clamp(hz, minHz, maxHz);
	return std::log(safeHz / minHz) / std::log(maxHz / minHz);
}

float logFrequencyAt(float x01, float minHz, float maxHz) {
	return minHz * std::pow(maxHz / minHz, bifurx::clamp01(x01));
}

float responseYForDbDisplay(float db, float minDb, float maxDb, float bottomY, float topY) {
	const float clampedDb = clamp(db, minDb, maxDb);
	const float midY = 0.5f * (bottomY + topY);

	if (clampedDb >= 0.f) {
		if (maxDb <= 1e-6f) {
			return midY;
		}
		return rescale(clampedDb, 0.f, maxDb, midY, topY);
	}

	if (minDb >= -1e-6f) {
		return midY;
	}
	return rescale(clampedDb, minDb, 0.f, bottomY, midY);
}

float softLimitOverlayDeltaDb(float db) {
	constexpr float kneeDb = 18.f;
	constexpr float limitDb = 42.f;
	const float sign = (db < 0.f) ? -1.f : 1.f;
	const float absDb = std::fabs(db);
	if (absDb <= kneeDb) {
		return db;
	}
	const float over = absDb - kneeDb;
	const float compressed = kneeDb + (limitDb - kneeDb) * (1.f - fastExp(-over / (limitDb - kneeDb)));
	return sign * std::min(compressed, limitDb);
}

float softLimitExpectedCurveDb(float db) {
	constexpr float kneeDb = 26.f;
	constexpr float limitDb = 48.f;
	const float sign = (db < 0.f) ? -1.f : 1.f;
	const float absDb = std::fabs(db);
	if (absDb <= kneeDb) {
		return clamp(db, -limitDb, limitDb);
	}
	const float over = absDb - kneeDb;
	const float compressed = kneeDb + (limitDb - kneeDb) * (1.f - fastExp(-over / (limitDb - kneeDb)));
	return sign * std::min(compressed, limitDb);
}

float resoToDamping(float resoNorm) {
	const float r = bifurx::clamp01(resoNorm);
	return 2.f - 1.97f * std::pow(r, 1.18f);
}

float signedWeight(float balance, bool upperPeak) {
	const float b = clamp(balance, -1.f, 1.f);
	// Slight cubic emphasis: keep midpoint behavior close, push harder near extremes.
	const float shaped = clamp(b + 0.35f * b * b * b, -1.f, 1.f);
	const float sign = upperPeak ? 1.f : -1.f;
	return fastExp(0.82f * sign * shaped);
}

float cascadeWideMorph(float spanNorm) {
	const float x = bifurx::clamp01((bifurx::clamp01(spanNorm) - 0.03f) / 0.97f);
	return std::pow(x, 0.58f);
}

float highHighSpanCompGain(float wideMorph) {
	const float x = bifurx::clamp01((wideMorph - 0.75f) / 0.25f);
	return 1.f + 0.685f * std::pow(x, 1.1f);
}

float resampleFirstNotchHz(float freqAHz, float freqBHz, float sampleRate) {
	const float sr = std::max(sampleRate, 1.f);
	return clamp(std::min(freqAHz, freqBHz), 4.f, 0.42f * sr);
}

float resampleEndHz(float freqAHz, float freqBHz, float sampleRate) {
	const float sr = std::max(sampleRate, 1.f);
	const float first = resampleFirstNotchHz(freqAHz, freqBHz, sr);
	const float rawEnd = clamp(std::max(freqAHz, freqBHz), first * 1.12f, 0.46f * sr);
	return std::max(rawEnd, first * 1.12f);
}

float resamplingSecondNotchHz(float firstNotchHz, float endHz, float sampleRate) {
	const float sr = std::max(sampleRate, 1.f);
	const float first = clamp(firstNotchHz, 4.f, 0.42f * sr);
	const float end = clamp(endHz, first * 1.08f, 0.46f * sr);
	return std::exp(mixf(std::log(first), std::log(end), 0.43f));
}

float resamplingTailNotchHz(float firstNotchHz, float endHz, float sampleRate) {
	const float sr = std::max(sampleRate, 1.f);
	const float first = clamp(firstNotchHz, 4.f, 0.42f * sr);
	const float end = clamp(endHz, first * 1.08f, 0.46f * sr);
	return std::exp(mixf(std::log(first), std::log(end), 0.74f));
}

float resamplingRolloffHz(float firstNotchHz, float endHz, float sampleRate) {
	const float sr = std::max(sampleRate, 1.f);
	const float first = clamp(firstNotchHz, 4.f, 0.42f * sr);
	const float end = clamp(endHz, first * 1.08f, 0.46f * sr);
	return clamp(0.9f * end, 4.f, 0.46f * sr);
}

float resamplingDisplayDroop(float hz, float rolloffHz) {
	const float safeRolloffHz = std::max(rolloffHz, 1.f);
	const float x = std::max(hz, 0.f) / safeRolloffHz;
	return 1.f / std::sqrt(1.f + 1.45f * x * x + 1.15f * x * x * x * x);
}

int resampleCicLengthForFirstNull(float firstNotchHz, float sampleRate) {
	const float sr = std::max(sampleRate, 1.f);
	const float safeFirstNotchHz = clamp(firstNotchHz, kFreqMinHz, 0.46f * sr);
	return std::max(1, int(std::round(sr / safeFirstNotchHz)));
}

std::complex<float> resampleCicResponse(float hz, float sampleRate, int n, int stages) {
	const float sr = std::max(sampleRate, 1.f);
	const int taps = std::max(n, 1);
	const int stageCount = std::max(stages, 1);
	const float clampedHz = clamp(hz, 0.f, 0.49f * sr);
	const float omega = 2.f * kPi * clampedHz / sr;
	const std::complex<float> z1 = std::exp(std::complex<float>(0.f, -omega));
	std::complex<float> zN(1.f, 0.f);
	for (int i = 0; i < taps; ++i) {
		zN *= z1;
	}
	const std::complex<float> numerator = 1.f - zN;
	const std::complex<float> denominator = float(taps) * (1.f - z1);

	std::complex<float> base(1.f, 0.f);
	if (std::abs(denominator) > 1e-7f) {
		base = numerator / denominator;
	}

	std::complex<float> response(1.f, 0.f);
	for (int i = 0; i < stageCount; ++i) {
		response *= base;
	}
	return response;
}

float resampleCicMagnitude(float hz, float sampleRate, int n, int stages) {
	return std::abs(resampleCicResponse(hz, sampleRate, n, stages));
}

float resampleOnePoleAlpha(float sampleRate, float cutoff) {
	const float sr = std::max(sampleRate, 1.f);
	const float clampedCutoff = clamp(cutoff, kFreqMinHz, 0.46f * sr);
	return 1.f - fastExp(-2.f * kPi * clampedCutoff / sr);
}

std::complex<float> resampleOnePoleLowpassResponse(float hz, float sampleRate, float cutoff) {
	const float sr = std::max(sampleRate, 1.f);
	const float clampedHz = clamp(hz, 0.f, 0.49f * sr);
	const float alpha = resampleOnePoleAlpha(sr, cutoff);
	const std::complex<float> z1 = std::exp(std::complex<float>(0.f, -2.f * kPi * clampedHz / sr));
	return std::complex<float>(alpha, 0.f) / (std::complex<float>(1.f, 0.f) - std::complex<float>(1.f - alpha, 0.f) * z1);
}

float resampleStage1Mix(float resoNorm) {
	return std::pow(clamp01(resoNorm), 0.72f);
}

float resampleLobeShapeMix(float spanNorm, float resoNorm) {
	const float spanShape = clamp01(spanNorm);
	const float resoShape = resampleStage1Mix(resoNorm);
	return clamp(0.06f + 0.74f * spanShape + 0.20f * resoShape, 0.f, 0.96f);
}

float resamplePeak1HzFromFirstNull(float firstNullHz) {
	return std::max(firstNullHz, 0.f) * kResamplePeak1NullRatio;
}

float resamplePeak2HzFromFirstNull(float firstNullHz) {
	return std::max(firstNullHz, 0.f) * kResamplePeak2NullRatio;
}

float resampleFirstNullHzFromPeak2(float peak2Hz, float sampleRate) {
	const float sr = std::max(sampleRate, 1.f);
	const float safePeak2Hz = clamp(peak2Hz, kFreqMinHz, 0.46f * sr);
	return clamp(safePeak2Hz / kResamplePeak2NullRatio, kFreqMinHz, 0.42f * sr);
}

float resamplePeak2HzFromFreqPair(float freqAHz, float freqBHz, float sampleRate) {
	const float sr = std::max(sampleRate, 1.f);
	const float safeA = clamp(freqAHz, kFreqMinHz, 0.46f * sr);
	const float safeB = clamp(freqBHz, kFreqMinHz, 0.46f * sr);
	return clamp(std::sqrt(safeA * safeB), kFreqMinHz, 0.46f * sr);
}

float resampleRolloffHzFromPeakStandins(float peak1Hz, float peak2StandinHz, float sampleRate, float spanNorm) {
	const float sr = std::max(sampleRate, 1.f);
	const float safePeak1Hz = clamp(peak1Hz, kFreqMinHz, 0.46f * sr);
	const float safePeak2Hz = clamp(peak2StandinHz, safePeak1Hz * 1.08f, 0.46f * sr);
	const float spanShape = clamp01(spanNorm);
	return clamp(mixf(0.70f, 1.18f, spanShape) * safePeak2Hz, safePeak1Hz * 1.04f, 0.46f * sr);
}

NVGcolor mixColor(const NVGcolor& a, const NVGcolor& b, float t) {
	const float clampedT = bifurx::clamp01(t);
	NVGcolor out;
	out.r = bifurx::mixf(a.r, b.r, clampedT);
	out.g = bifurx::mixf(a.g, b.g, clampedT);
	out.b = bifurx::mixf(a.b, b.b, clampedT);
	out.a = bifurx::mixf(a.a, b.a, clampedT);
	return out;
}

void formatFrequencyLabel(float hz, char* out, size_t outSize) {
	const float safeHz = std::max(hz, 0.f);
	if (safeHz >= 1000.f) {
		if (safeHz >= 10000.f) {
			std::snprintf(out, outSize, "%.1fkHz", safeHz / 1000.f);
		}
		else {
			std::snprintf(out, outSize, "%.2fkHz", safeHz / 1000.f);
		}
		return;
	}
	if (safeHz >= 100.f) {
		std::snprintf(out, outSize, "%.0fHz", safeHz);
		return;
	}
	if (safeHz >= 10.f) {
		std::snprintf(out, outSize, "%.1fHz", safeHz);
		return;
	}
	std::snprintf(out, outSize, "%.2fHz", safeHz);
}

SvfCoeffs makeSvfCoeffs(float sampleRate, float cutoff, float damping, float dampingMin) {
	const float limitedCutoff = clamp(cutoff, 4.f, 0.46f * sampleRate);
	const float g = fastTan(kPi * limitedCutoff / sampleRate);
	const float k = clamp(damping, dampingMin, kSvfDampingMax);
	const float a1 = 1.f / (1.f + g * (g + k));
	SvfCoeffs coeffs;
	coeffs.g = g;
	coeffs.k = k;
	coeffs.a1 = a1;
	return coeffs;
}

SvfOutputs TptSvf::processWithCoeffs(float input, const SvfCoeffs& coeffs) {
	const float v1 = coeffs.a1 * (ic1eq + coeffs.g * (input - ic2eq));
	const float v2 = ic2eq + coeffs.g * v1;

	ic1eq = 2.f * v1 - ic1eq;
	ic2eq = 2.f * v2 - ic2eq;

	SvfOutputs out;
	out.bp = v1;
	out.lp = v2;
	out.hp = input - coeffs.k * v1 - v2;
	out.notch = out.lp + out.hp;
	return out;
}

SvfOutputs TptSvf::processSelfOscWithCoeffs(
	const SvfCoeffs& coeffs,
	float input,
	float oscOnset,
	float oscHeat,
	float oscDrive
) {
	const float m = ic1eq + coeffs.g * (input - ic2eq);
	const float onePlusG2 = 1.f + coeffs.g * coeffs.g;
	float v1 = m / std::max(onePlusG2 + coeffs.g * coeffs.k, 1e-5f);

	const float drive = std::max(oscDrive, 1e-4f);
	const float amp = v1 * drive;
	const float ampDamping = mixf(kSelfOscAmpDampingClean, kSelfOscAmpDampingHot, clamp01(oscHeat));
	const float kEff = coeffs.k - kSelfOscPush * clamp01(oscOnset) + ampDamping * amp * amp;
	v1 = m / std::max(onePlusG2 + coeffs.g * kEff, 1e-5f);

	const float v2 = ic2eq + coeffs.g * v1;
	ic1eq = 2.f * v1 - ic1eq;
	ic2eq = 2.f * v2 - ic2eq;

	SvfOutputs out;
	out.bp = v1;
	out.lp = v2;
	const float outAmp = v1 * drive;
	const float outKEff = coeffs.k - kSelfOscPush * clamp01(oscOnset) + ampDamping * outAmp * outAmp;
	out.hp = input - outKEff * v1 - v2;
	out.notch = out.lp + out.hp;
	return out;
}

SvfOutputs TptSvf::process(float input, float sampleRate, float cutoff, float damping) {
	return processWithCoeffs(input, makeSvfCoeffs(sampleRate, cutoff, damping));
}

void sanitizeCoreState(TptSvf& core) {
	if (!std::isfinite(core.ic1eq) || !std::isfinite(core.ic2eq)) {
		core.ic1eq = 0.f;
		core.ic2eq = 0.f;
	}
	core.ic1eq = clamp(core.ic1eq, -20.f, 20.f);
	core.ic2eq = clamp(core.ic2eq, -20.f, 20.f);
}

float processResampleLowpass(TptSvf& core, float input, float sampleRate, float cutoff) {
	const SvfCoeffs coeffs = makeSvfCoeffs(sampleRate, cutoff, 0.72f);
	return core.processWithCoeffs(input, coeffs).lp;
}

float processResampleContour(TptSvf& tailNotchCore, TptSvf& lowpassCore, float input, float sampleRate, float firstNotchHz, float endHz, float tailDamping) {
	const SvfCoeffs tailNotchCoeffs = makeSvfCoeffs(sampleRate, resamplingTailNotchHz(firstNotchHz, endHz, sampleRate), clamp(tailDamping, 0.02f, 2.2f));
	const float tailNotched = tailNotchCore.processWithCoeffs(input, tailNotchCoeffs).notch;
	const float rolled = processResampleLowpass(lowpassCore, tailNotched, sampleRate, resamplingRolloffHz(firstNotchHz, endHz, sampleRate));
	const float contourTilt = 1.f / (1.f + 0.32f * std::max(firstNotchHz, 1.f) / std::max(endHz, 1.f));
	return contourTilt * rolled;
}

void MovingAverageFilter::reset() {
	std::fill(delayLine.begin(), delayLine.end(), 0.f);
	writeIndex = 0;
	windowLength = 1;
	sum = 0.f;
}

float MovingAverageFilter::process(float input, int newWindowLength) {
	const int clampedWindowLength = std::max(newWindowLength, 1);
	if (delayLine.size() < size_t(clampedWindowLength)) {
		delayLine.resize(size_t(clampedWindowLength), 0.f);
	}

	if (clampedWindowLength != windowLength) {
		windowLength = clampedWindowLength;
		sum = 0.f;
		const size_t capacity = delayLine.size();
		for (int i = 1; i <= windowLength; ++i) {
			const size_t index = (writeIndex + capacity - size_t(i)) % capacity;
			sum += delayLine[index];
		}
	}

	const size_t capacity = delayLine.size();
	const size_t oldestIndex = (writeIndex + capacity - size_t(windowLength)) % capacity;
	sum += input - delayLine[oldestIndex];
	delayLine[writeIndex] = input;
	writeIndex = (writeIndex + 1) % capacity;
	return sum / float(windowLength);
}

void OnePoleLowpass::reset() {
	state = 0.f;
	alpha = 1.f;
	cachedSampleRate = 0.f;
	cachedCutoff = 0.f;
}

float OnePoleLowpass::process(float input, float sampleRate, float cutoff) {
	const float sr = std::max(sampleRate, 1.f);
	const float clampedCutoff = clamp(cutoff, kFreqMinHz, 0.46f * sr);
	if (std::fabs(cachedSampleRate - sr) > 0.5f || std::fabs(cachedCutoff - clampedCutoff) > 1e-3f) {
		alpha = resampleOnePoleAlpha(sr, clampedCutoff);
		cachedSampleRate = sr;
		cachedCutoff = clampedCutoff;
	}
	state += alpha * (input - state);
	return state;
}

void ResampleFilterChain::reset() {
	for (int i = 0; i < kResampleCicStages; ++i) {
		cic[i].reset();
	}
	postLowpass.reset();
}

float ResampleFilterChain::process(float input, float sampleRate, float firstNotchHz, float rolloffHz, float spanNorm, float resoNorm) {
	const int cicLength = resampleCicLengthForFirstNull(firstNotchHz, sampleRate);
	const float stage1 = cic[0].process(input, cicLength);
	const float stage2 = cic[1].process(stage1, cicLength);
	const float cicMixed = mixf(stage2, stage1, resampleLobeShapeMix(spanNorm, resoNorm));
	return postLowpass.process(cicMixed, sampleRate, rolloffHz);
}

SvfOutputs processCharacterStage(
	TptSvf& core,
	int stageIndex,
	float input,
	float sampleRate,
	float cutoff,
	float damping,
	float drive,
	float resoNorm,
	bool highResonanceSelfOscEnabled,
	const SvfCoeffs* cachedCoeffsOrNull
) {
	(void) stageIndex;
	if (!highResonanceSelfOscEnabled) {
		if (cachedCoeffsOrNull) {
			return core.processWithCoeffs(input, *cachedCoeffsOrNull);
		}
		return core.process(input, sampleRate, cutoff, damping);
	}
	const float oscNorm = smoothstep01((clamp01(resoNorm) - kSelfOscResoStart) / (kSelfOscResoFull - kSelfOscResoStart));
	if (oscNorm <= 0.f) {
		if (cachedCoeffsOrNull) {
			return core.processWithCoeffs(input, *cachedCoeffsOrNull);
		}
		return core.process(input, sampleRate, cutoff, damping);
	}
	const float oscOnset = std::sqrt(std::max(oscNorm, 0.f));
	const float oscHeat = smoothstep01((clamp01(resoNorm) - kSelfOscHeatStart) / (1.f - kSelfOscHeatStart));
	const float oscDrive = mixf(0.75f, 2.6f, oscHeat) * mixf(0.85f, 1.35f, clamp01((drive - 1.f) / 2.f));
	const float selfDamping = mixf(damping, kSvfSelfOscDampingMin, oscOnset);
	const SvfCoeffs coeffs = makeSvfCoeffs(sampleRate, cutoff, selfDamping, kSvfSelfOscDampingMin);
	SvfOutputs out = core.processSelfOscWithCoeffs(coeffs, input, oscOnset, oscHeat, oscDrive);
	if (!std::isfinite(out.lp) || !std::isfinite(out.bp) || !std::isfinite(out.hp) || !std::isfinite(out.notch)) {
		sanitizeCoreState(core);
		if (cachedCoeffsOrNull) {
			return core.processWithCoeffs(input, *cachedCoeffsOrNull);
		}
		return core.process(input, sampleRate, cutoff, damping);
	}
	return out;
}

std::complex<float> DisplayBiquad::response(float omega) const {
	const std::complex<float> z1 = std::exp(std::complex<float>(0.f, -omega));
	return response(z1, z1 * z1);
}

std::complex<float> DisplayBiquad::response(std::complex<float> z1, std::complex<float> z2) const {
	const std::complex<float> numerator = b0 + b1 * z1 + b2 * z2;
	const std::complex<float> denominator = 1.f + a1 * z1 + a2 * z2;
	return numerator / denominator;
}

DisplayBiquad makeDisplayBiquad(float sampleRate, float cutoff, float q, int type) {
	const float sr = std::max(sampleRate, 1.f);
	const float freq = clamp(cutoff, 4.f, 0.46f * sr);
	const float omega = 2.f * kPi * freq / sr;
	const float cosW = std::cos(omega);
	const float sinW = std::sin(omega);
	const float clampedQ = std::max(q, 1.f / kSvfDampingMax);
	const float alpha = sinW / (2.f * clampedQ);

	float b0 = 0.f;
	float b1 = 0.f;
	float b2 = 0.f;
	float a0 = 1.f + alpha;
	float a1 = -2.f * cosW;
	float a2 = 1.f - alpha;

	switch (type) {
		case 0: // lowpass
			b0 = 0.5f * (1.f - cosW);
			b1 = 1.f - cosW;
			b2 = 0.5f * (1.f - cosW);
			break;
		case 1: // bandpass
			b0 = alpha * clampedQ;
			b1 = 0.f;
			b2 = -b0;
			break;
		case 2: // highpass
			b0 = 0.5f * (1.f + cosW);
			b1 = -(1.f + cosW);
			b2 = 0.5f * (1.f + cosW);
			break;
		default: // notch
			b0 = 1.f;
			b1 = -2.f * cosW;
			b2 = 1.f;
			break;
	}

	DisplayBiquad biquad;
	biquad.b0 = b0 / a0;
	biquad.b1 = b1 / a0;
	biquad.b2 = b2 / a0;
	biquad.a1 = a1 / a0;
	biquad.a2 = a2 / a0;
	return biquad;
}

bool previewStatesDiffer(const BifurxPreviewState& a, const BifurxPreviewState& b) {
	if (a.mode != b.mode) return true;
	if (std::fabs(a.sampleRate - b.sampleRate) > 0.5f) return true;
	if (std::fabs(a.balance - b.balance) > 1e-3f) return true;
	if (std::fabs(fastLog2(std::max(a.freqA, 1.f)) - fastLog2(std::max(b.freqA, 1.f))) > 1e-3f) return true;
	if (std::fabs(fastLog2(std::max(a.freqB, 1.f)) - fastLog2(std::max(b.freqB, 1.f))) > 1e-3f) return true;
	if (std::fabs(a.qA - b.qA) > 1e-3f) return true;
	if (std::fabs(a.qB - b.qB) > 1e-3f) return true;
	return false;
}

BifurxPreviewModel makePreviewModel(const BifurxPreviewState& state) {
	BifurxPreviewModel model;
	const float freqA = clamp(state.freqA, 4.f, 0.46f * std::max(state.sampleRate, 1.f));
	const float freqB = clamp(state.freqB, 4.f, 0.46f * std::max(state.sampleRate, 1.f));
	const float qMin = 1.f / kSvfDampingMax;
	const float qMax = 1.f / kSvfDampingMin;
	const float qA = clamp(state.qA, qMin, qMax);
	const float qB = clamp(state.qB, qMin, qMax);
	model.lowA = makeDisplayBiquad(state.sampleRate, freqA, qA, 0);
	model.bandA = makeDisplayBiquad(state.sampleRate, freqA, qA, 1);
	model.highA = makeDisplayBiquad(state.sampleRate, freqA, qA, 2);
	model.notchA = makeDisplayBiquad(state.sampleRate, freqA, qA, 3);
	model.lowB = makeDisplayBiquad(state.sampleRate, freqB, qB, 0);
	model.bandB = makeDisplayBiquad(state.sampleRate, freqB, qB, 1);
	model.highB = makeDisplayBiquad(state.sampleRate, freqB, qB, 2);
	model.notchB = makeDisplayBiquad(state.sampleRate, freqB, qB, 3);
	model.markerFreqA = freqA;
	model.markerFreqB = freqB;
	model.sampleRate = state.sampleRate;
	model.qA = qA;
	model.qB = qB;
	model.resoNorm = state.resoNorm;
	model.mode = state.mode;
	if (model.mode == 10) {
		const float peak2Hz = resamplePeak2HzFromFreqPair(freqA, freqB, state.sampleRate);
		const float firstNullHz = resampleFirstNullHzFromPeak2(peak2Hz, state.sampleRate);
		const float maxHz = 0.46f * std::max(state.sampleRate, 1.f);
		model.markerFreqA = clamp(resamplePeak1HzFromFirstNull(firstNullHz), kFreqMinHz, maxHz);
		model.markerFreqB = clamp(resamplePeak2HzFromFirstNull(firstNullHz), kFreqMinHz, maxHz);
	}

	const float lowW = signedWeight(state.balance, false);
	const float highW = signedWeight(state.balance, true);
	const float norm = 2.f / (lowW + highW);
	model.wA = lowW * norm;
	model.wB = highW * norm;
	model.wideMorph = cascadeWideMorph(state.spanNorm);
	return model;
}

std::complex<float> previewModelResponse(const BifurxPreviewModel& model, float hz) {
	const float omega = 2.f * kPi * clamp(hz, 4.f, 0.49f * model.sampleRate) / std::max(model.sampleRate, 1.f);
	const std::complex<float> z1 = std::exp(std::complex<float>(0.f, -omega));
	const std::complex<float> z2 = z1 * z1;

	std::complex<float> lpA = model.lowA.response(z1, z2);
	std::complex<float> bpA = model.bandA.response(z1, z2);
	std::complex<float> hpA = model.highA.response(z1, z2);
	std::complex<float> lpB = model.lowB.response(z1, z2);
	std::complex<float> bpB = model.bandB.response(z1, z2);
	std::complex<float> hpB = model.highB.response(z1, z2);
	const std::complex<float> ntA = lpA + hpA, ntB = lpB + hpB, cascadeLp = lpB * lpA, cascadeNotch = ntB * ntA, cascadeNotchToLow = lpB * ntA, cascadeHpToLp = lpB * hpA, cascadeHighToNotch = ntB * hpA, cascadeHpToHp = hpB * hpA;
	if (model.mode == 10) {
		const float peak2Hz = std::max(model.markerFreqA, model.markerFreqB);
		const float firstNotchHz = resampleFirstNullHzFromPeak2(peak2Hz, model.sampleRate);
		const float peak1Hz = resamplePeak1HzFromFirstNull(firstNotchHz);
		const float rolloffHz = resampleRolloffHzFromPeakStandins(peak1Hz, resamplePeak2HzFromFirstNull(firstNotchHz), model.sampleRate, model.wideMorph);
		const int cicLength = resampleCicLengthForFirstNull(firstNotchHz, model.sampleRate);
		const std::complex<float> cicStage1 = resampleCicResponse(hz, model.sampleRate, cicLength, 1);
		const std::complex<float> cicStage2 = resampleCicResponse(hz, model.sampleRate, cicLength, 2);
		const float stage1Mix = resampleLobeShapeMix(model.wideMorph, model.resoNorm);
		const std::complex<float> cicResponse = cicStage2 + (cicStage1 - cicStage2) * stage1Mix;
		return cicResponse * resampleOnePoleLowpassResponse(hz, model.sampleRate, rolloffHz);
	}
	return combineModeResponse<std::complex<float>>(model.mode, lpA, bpA, hpA, ntA, lpB, bpB, hpB, ntB, cascadeLp, cascadeNotch, cascadeNotchToLow, cascadeHpToLp, cascadeHighToNotch, cascadeHpToHp, model.wA, model.wB, model.wideMorph);
}

float previewModelResponseDb(const BifurxPreviewModel& model, float hz) {
	const float mag = std::abs(previewModelResponse(model, hz));
	return 20.f * std::log10(std::max(mag, 1e-5f));
}

float previewProbeStimulusSample(const BifurxPreviewState& state, int sampleIndex) {
	(void) state;
	if (sampleIndex < 0) return 0.f;
	return (sampleIndex == 0) ? kPreviewProbeImpulseAmplitude : 0.f;
}

SvfOutputs processProbeStage(BifurxProbeEngineState& state, int stageIndex, float input, float sampleRate, float cutoff, float damping, float drive, float resoNorm, bool highResonanceSelfOscEnabled) {
	TptSvf& core = (stageIndex == 0) ? state.svfA : state.svfB;
	return processCharacterStage(core, stageIndex, input, sampleRate, cutoff, damping, drive, resoNorm, highResonanceSelfOscEnabled, nullptr);
}

void simulatePreviewProbeImpulseResponse(const BifurxPreviewState& state, float* inputBuffer, float* outputBuffer, int sampleCount) {
	if (!inputBuffer || !outputBuffer || sampleCount <= 0) return;
	BifurxProbeEngineState engine;
	const float sampleRate = std::max(state.sampleRate, 1.f), freqA = clamp(state.freqA, kFreqMinHz, 0.46f * sampleRate), freqB = clamp(state.freqB, kFreqMinHz, 0.46f * sampleRate), dampingA = clamp(1.f / std::max(state.qA, 0.05f), 0.02f, 2.2f), dampingB = clamp(1.f / std::max(state.qB, 0.05f), 0.02f, 2.2f), lowW = signedWeight(state.balance, false), highW = signedWeight(state.balance, true), norm = 2.f / (lowW + highW), wA = lowW * norm, wB = highW * norm, wideMorph = cascadeWideMorph(state.spanNorm), drive = levelDriveGain(kPreviewProbeLevelKnob);
	const int mode = clamp(state.mode, 0, kBifurxModeCount - 1);
	for (int i = 0; i < sampleCount; ++i) {
		const float rawIn = previewProbeStimulusSample(state, i), excitation = applyLevelInputStage(rawIn, kPreviewProbeLevelKnob);
		const SvfOutputs a = processProbeStage(engine, 0, excitation, sampleRate, freqA, dampingA, drive, state.resoNorm, true);
		SvfOutputs b; float modeOut = 0.f;
		switch (mode) {
			case 0: b = processProbeStage(engine, 1, a.lp, sampleRate, freqB, dampingB, drive, state.resoNorm, true); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, b.lp, 0.f, 0.f, 0.f, 0.f, 0.f, wA, wB, wideMorph); break;
			case 1:
			case 4:
			case 5:
			case 8: b = processProbeStage(engine, 1, excitation, sampleRate, freqB, dampingB, drive, state.resoNorm, true); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, wA, wB, wideMorph); break;
			case 2: b = processProbeStage(engine, 1, a.notch, sampleRate, freqB, dampingB, drive, state.resoNorm, true); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, b.lp, 0.f, 0.f, 0.f, wA, wB, wideMorph); break;
			case 3: b = processProbeStage(engine, 1, a.notch, sampleRate, freqB, dampingB, drive, state.resoNorm, true); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, b.notch, 0.f, 0.f, 0.f, 0.f, wA, wB, wideMorph); break;
			case 6: b = processProbeStage(engine, 1, a.hp, sampleRate, freqB, dampingB, drive, state.resoNorm, true); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, b.lp, 0.f, 0.f, wA, wB, wideMorph); break;
			case 7: b = processProbeStage(engine, 1, a.hp, sampleRate, freqB, dampingB, drive, state.resoNorm, true); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, 0.f, b.notch, 0.f, wA, wB, wideMorph); break;
			case 9: b = processProbeStage(engine, 1, a.hp, sampleRate, freqB, dampingB, drive, state.resoNorm, true); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, 0.f, 0.f, b.hp, wA, wB, wideMorph); break;
			default: {
				const float peak2Hz = resamplePeak2HzFromFreqPair(freqA, freqB, sampleRate);
				const float firstNotchHz = resampleFirstNullHzFromPeak2(peak2Hz, sampleRate);
				const float peak1Hz = resamplePeak1HzFromFirstNull(firstNotchHz);
				const float rolloffHz = resampleRolloffHzFromPeakStandins(peak1Hz, resamplePeak2HzFromFirstNull(firstNotchHz), sampleRate, wideMorph);
				modeOut = engine.resampleChain.process(excitation, sampleRate, firstNotchHz, rolloffHz, wideMorph, state.resoNorm);
			} break;
		}
		inputBuffer[i] = excitation; outputBuffer[i] = applyLevelOutputStage(modeOut, kPreviewProbeLevelKnob);
	}
}

static std::atomic<uint32_t> gBifurxDebugInstanceCounter{1u};

Bifurx::Bifurx() {
	debugInstanceId = gBifurxDebugInstanceCounter.fetch_add(1u, std::memory_order_relaxed);
	createdUnixTimeSec = system::getUnixTime();
	config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
	configSwitch(MODE_PARAM, 0.f, float(kBifurxModeCount - 1), 0.f, "Mode", {
		kBifurxModeLabels[0],
		kBifurxModeLabels[1],
		kBifurxModeLabels[2],
		kBifurxModeLabels[3],
		kBifurxModeLabels[4],
		kBifurxModeLabels[5],
		kBifurxModeLabels[6],
		kBifurxModeLabels[7],
		kBifurxModeLabels[8],
		kBifurxModeLabels[9],
		kBifurxModeLabels[10]
	});
	configParam(LEVEL_PARAM, 0.f, 1.f, 0.5f, "Level"); configParam(FREQ_PARAM, 0.f, 1.f, 0.5f, "Frequency"); configParam(RESO_PARAM, 0.f, 1.f, 0.35f, "Resonance"); configParam(BALANCE_PARAM, -1.f, 1.f, 0.f, "Balance"); configParam(SPAN_PARAM, 0.f, 1.f, 0.5f, "Span"); configParam(FM_AMT_PARAM, -1.f, 1.f, 0.f, "FM amount"); configParam(SPAN_CV_ATTEN_PARAM, -1.f, 1.f, 0.f, "Span CV attenuator"); configParam(TITO_PARAM, -1.f, 1.f, 0.f, "TITO strength"); configButton(MODE_LEFT_PARAM, "Mode previous"); configButton(MODE_RIGHT_PARAM, "Mode next");
	configInput(IN_INPUT, "Signal In"); configInput(VOCT_INPUT, "V/Oct"); configInput(FM_INPUT, "FM"); configInput(RESO_CV_INPUT, "Resonance CV"); configInput(BALANCE_CV_INPUT, "Balance CV"); configInput(SPAN_CV_INPUT, "Span CV"); configOutput(OUT_OUTPUT, "Signal Out"); configBypass(IN_INPUT, OUT_OUTPUT);
	paramQuantities[MODE_PARAM]->snapEnabled = true;
	previewPublishDivider.setDivision(kPreviewPublishFastDivision); previewPublishSlowDivider.setDivision(kPreviewPublishSlowDivision); controlUpdateDivider.setDivision(16); perfMeasureDivider.setDivision(64);
}

void Bifurx::resetCircuitStates() { coreA.ic1eq = 0.f; coreA.ic2eq = 0.f; coreB.ic1eq = 0.f; coreB.ic2eq = 0.f; resampleFilterCore.reset(); llTelemetryExcitationSq = 0.f; llTelemetryStageALpSq = 0.f; llTelemetryStageBLpSq = 0.f; llTelemetryOutputSq = 0.f; voctCvFiltered = 0.f; voctCvFilterInitialized = false; }
json_t* Bifurx::dataToJson() {
	json_t* root = json_object();
	json_object_set_new(root, "fftScaleDynamic", json_boolean(fftScaleDynamic));
	json_object_set_new(root, "showModuleResponseOverlay", json_boolean(showModuleResponseOverlay));
	json_object_set_new(root, "useGlShaderRenderer", json_boolean(useGlShaderRenderer));
	json_object_set_new(root, "controlUpdateMode", json_integer(controlUpdateMode));
	json_object_set_new(root, "curveDebugLogging", json_boolean(curveDebugLogging));
	json_object_set_new(root, "perfDebugLogging", json_boolean(perfDebugLogging));
	json_object_set_new(root, "highResonanceSelfOscEnabled", json_boolean(highResonanceSelfOscEnabled));
	json_object_set_new(root, "softLimitingEnabled", json_boolean(softLimitingEnabled));
	json_object_set_new(root, "renderMode", json_integer(renderMode));
	json_object_set_new(root, "createdUnixTimeSec", json_real(createdUnixTimeSec));
	return root;
}

void Bifurx::dataFromJson(json_t* root) {
	if (!root) {
		return;
	}
	Module::dataFromJson(root);
	json_t* fftScaleDynamicJ = json_object_get(root, "fftScaleDynamic");
	if (fftScaleDynamicJ) {
		fftScaleDynamic = json_is_true(fftScaleDynamicJ);
	}
	json_t* showModuleResponseOverlayJ = json_object_get(root, "showModuleResponseOverlay");
	if (showModuleResponseOverlayJ) {
		showModuleResponseOverlay = json_is_true(showModuleResponseOverlayJ);
	}
	json_t* useGlShaderRendererJ = json_object_get(root, "useGlShaderRenderer");
	if (useGlShaderRendererJ) {
		useGlShaderRenderer = json_is_true(useGlShaderRendererJ);
	}
	json_t* controlUpdateModeJ = json_object_get(root, "controlUpdateMode");
	if (controlUpdateModeJ) {
		controlUpdateMode = clamp(int(json_integer_value(controlUpdateModeJ)), CONTROL_UPDATE_TIERED, CONTROL_UPDATE_COUNT - 1);
		controlFastCacheValid = false;
	}
	json_t* curveDebugLoggingJ = json_object_get(root, "curveDebugLogging");
	if (curveDebugLoggingJ) {
		curveDebugLogging = json_is_true(curveDebugLoggingJ);
	}
	json_t* perfDebugLoggingJ = json_object_get(root, "perfDebugLogging");
	if (perfDebugLoggingJ) {
		perfDebugLogging = json_is_true(perfDebugLoggingJ);
	}
	json_t* highResonanceSelfOscEnabledJ = json_object_get(root, "highResonanceSelfOscEnabled");
	if (highResonanceSelfOscEnabledJ) {
		highResonanceSelfOscEnabled = json_is_true(highResonanceSelfOscEnabledJ);
	}
	json_t* softLimitingEnabledJ = json_object_get(root, "softLimitingEnabled");
	if (softLimitingEnabledJ) {
		softLimitingEnabled = json_is_true(softLimitingEnabledJ);
	}
	json_t* createdUnixTimeSecJ = json_object_get(root, "createdUnixTimeSec");
	if (createdUnixTimeSecJ && json_is_number(createdUnixTimeSecJ)) {
		const double loadedCreatedUnixTimeSec = json_number_value(createdUnixTimeSecJ);
		if (std::isfinite(loadedCreatedUnixTimeSec) && loadedCreatedUnixTimeSec > 0.0) {
			createdUnixTimeSec = loadedCreatedUnixTimeSec;
		}
	}

	auto decodeRenderMode = [](int rawRenderMode) {
		// Keep compatibility with earlier enum encodings where OpenGL could be 2 (or higher in migrated values).
		switch (rawRenderMode) {
			case RENDER_OPENGL:
			case 2:
			case 5:
			case 6:
				return RENDER_OPENGL;
			default:
				return RENDER_NANOVG;
		}
	};

	bool loadedRenderMode = false;
	json_t* renderModeJ = json_object_get(root, "renderMode");
	if (renderModeJ) {
		renderMode = (RenderMode) decodeRenderMode(int(json_integer_value(renderModeJ)));
		loadedRenderMode = true;
	}
	if (!loadedRenderMode) {
		// Legacy key used by older renderer debug menus.
		json_t* legacyRenderModeJ = json_object_get(root, "debugRenderMode");
		if (legacyRenderModeJ) {
			renderMode = (RenderMode) decodeRenderMode(int(json_integer_value(legacyRenderModeJ)));
		}
	}
	// Legacy key retained for backward patch compatibility.
	// Bifurx is SVF-only, so this key is intentionally ignored if present.
	json_t* legacyFilterCircuitModeJ = json_object_get(root, "filterCircuitMode");
	if (legacyFilterCircuitModeJ) {
		// Intentionally ignored.
	}
}
void Bifurx::resetPerfStats() { perfAudioSampledCount.store(0, std::memory_order_release); perfAudioProcessNs.store(0, std::memory_order_release); perfAudioControlsNs.store(0, std::memory_order_release); perfAudioCoreNs.store(0, std::memory_order_release); perfAudioPreviewNs.store(0, std::memory_order_release); perfAudioAnalysisNs.store(0, std::memory_order_release); perfAudioProcessMaxNs.store(0, std::memory_order_release); }
void Bifurx::publishPreviewState(const BifurxPreviewState& state) { int writeIndex = 1 - previewPublishedIndex.load(std::memory_order_relaxed); previewStates[writeIndex] = state; previewPublishedIndex.store(writeIndex, std::memory_order_release); previewPublishSeq.fetch_add(1, std::memory_order_release); lastPreviewState = state; hasLastPreviewState = true; }
void Bifurx::publishLlTelemetryState(const BifurxLlTelemetryState& state) { const int writeIndex = 1 - llTelemetryPublishedIndex.load(std::memory_order_relaxed); llTelemetryStates[writeIndex] = state; llTelemetryPublishedIndex.store(writeIndex, std::memory_order_release); llTelemetryPublishSeq.fetch_add(1, std::memory_order_release); }
void Bifurx::publishAnalysisFrame() { const int writeIndex = 1 - analysisPublishedIndex.load(std::memory_order_relaxed), start = analysisWritePos, firstCount = kFftSize - start, secondCount = start; std::memcpy(analysisFrames[writeIndex].rawInput, analysisRawInputHistory + start, size_t(firstCount) * sizeof(float)); std::memcpy(analysisFrames[writeIndex].rawInput + firstCount, analysisRawInputHistory, size_t(secondCount) * sizeof(float)); std::memcpy(analysisFrames[writeIndex].output, analysisOutputHistory + start, size_t(firstCount) * sizeof(float)); std::memcpy(analysisFrames[writeIndex].output + firstCount, analysisOutputHistory, size_t(secondCount) * sizeof(float)); std::memcpy(analysisFrames[writeIndex].responseOutput, analysisResponseOutputHistory + start, size_t(firstCount) * sizeof(float)); std::memcpy(analysisFrames[writeIndex].responseOutput + firstCount, analysisResponseOutputHistory, size_t(secondCount) * sizeof(float)); analysisPublishedIndex.store(writeIndex, std::memory_order_release); analysisPublishSeq.fetch_add(1, std::memory_order_release); }
void Bifurx::pushAnalysisSample(float rawInputSample, float outputSample, float responseOutputSample) { analysisRawInputHistory[analysisWritePos] = bifurx::sanitizeFinite(rawInputSample); analysisOutputHistory[analysisWritePos] = bifurx::sanitizeFinite(outputSample); analysisResponseOutputHistory[analysisWritePos] = bifurx::sanitizeFinite(responseOutputSample); analysisWritePos = (analysisWritePos + 1) % kFftSize; if (analysisFilled < kFftSize) analysisFilled++; if (analysisFilled == kFftSize) { analysisHopCounter++; if (!analysisPublishedOnce || analysisHopCounter >= kFftHopSize) { analysisHopCounter = 0; publishAnalysisFrame(); analysisPublishedOnce = true; } } }
void Bifurx::onSampleRateChange(const SampleRateChangeEvent& e) {
	controlFastCacheValid = false;
	voctCvFilterInitialized = false;
	previewFilterInitialized = false;
	const float sampleRate = std::max(e.sampleRate, 1.f);
	llTelemetryAlpha = onePoleAlpha(1.f / sampleRate, kLlTelemetryTauSeconds);
	llTelemetryAlphaSampleRate = sampleRate;
}

void Bifurx::process(const ProcessArgs& args) {
	using PerfClock = std::chrono::steady_clock;
	const bool measurePerf = perfDebugLogging && perfMeasureDivider.process();
	const PerfClock::time_point perfStart = measurePerf ? PerfClock::now() : PerfClock::time_point();
	PerfClock::time_point perfCoreStart, perfPreviewStart, perfAnalysisStart;

	sanitizeCoreState(coreA); sanitizeCoreState(coreB);

	if (modeLeftTrigger.process(params[MODE_LEFT_PARAM].getValue())) { const int currentMode = clamp(int(std::round(params[MODE_PARAM].getValue())), 0, kBifurxModeCount - 1); params[MODE_PARAM].setValue(float((currentMode + kBifurxModeCount - 1) % kBifurxModeCount)); }
	if (modeRightTrigger.process(params[MODE_RIGHT_PARAM].getValue())) { const int currentMode = clamp(int(std::round(params[MODE_PARAM].getValue())), 0, kBifurxModeCount - 1); params[MODE_PARAM].setValue(float((currentMode + 1) % kBifurxModeCount)); }

	const float in = bifurx::sanitizeFinite(inputs[IN_INPUT].getVoltage()), level = params[LEVEL_PARAM].getValue(), drive = levelDriveGain(level);
	const int mode = clamp(int(std::round(params[MODE_PARAM].getValue())), 0, kBifurxModeCount - 1);
	const float tito = clamp(params[TITO_PARAM].getValue(), -1.f, 1.f);
	const float titoAbs = std::fabs(tito);
	const bool titoNeutral = titoAbs < 0.02f;
	const float freqParamNorm = clamp(params[FREQ_PARAM].getValue(), 0.f, 1.f);
	const bool voctConnected = inputs[VOCT_INPUT].isConnected();
	const float voctCvRaw = voctConnected ? clamp(inputs[VOCT_INPUT].getVoltage(), -10.f, 10.f) : 0.f;
	if (std::fabs(voctCvFilterSampleRate - args.sampleRate) > 0.5f) { voctCvFilterAlpha = onePoleAlpha(1.f / std::max(args.sampleRate, 1.f), kVoctSmoothingTauSeconds); voctCvFilterSampleRate = args.sampleRate; }
	float voctCv = 0.f;
	if (voctConnected) { if (!voctCvFilterInitialized) { voctCvFiltered = voctCvRaw; voctCvFilterInitialized = true; } else voctCvFiltered += voctCvFilterAlpha * (voctCvRaw - voctCvFiltered); voctCv = (std::fabs(voctCvFiltered) < kVoctDeadbandVolts) ? 0.f : voctCvFiltered; }
	else { voctCvFiltered = 0.f; voctCvFilterInitialized = false; }
	const bool fmConnected = inputs[FM_INPUT].isConnected();
	const bool resoCvConnected = inputs[RESO_CV_INPUT].isConnected();
	const bool balanceCvConnected = inputs[BALANCE_CV_INPUT].isConnected();
	const bool spanCvConnected = inputs[SPAN_CV_INPUT].isConnected();
	const float fmAmt = clamp(params[FM_AMT_PARAM].getValue(), -1.f, 1.f), fmCv = fmConnected ? clamp(inputs[FM_INPUT].getVoltage(), -10.f, 10.f) : 0.f, fm = fmCv * fmAmt, resoCvNorm = clamp(inputs[RESO_CV_INPUT].getVoltage(), 0.f, 8.f) / 8.f, resoNorm = clamp(params[RESO_PARAM].getValue() + resoCvNorm, 0.f, 1.f), balanceCvNorm = clamp(inputs[BALANCE_CV_INPUT].getVoltage(), -5.f, 5.f) / 5.f, balanceNorm = clamp(params[BALANCE_PARAM].getValue() + balanceCvNorm, -1.f, 1.f), spanParamNorm = clamp(params[SPAN_PARAM].getValue(), 0.f, 1.f), spanAtten = clamp(params[SPAN_CV_ATTEN_PARAM].getValue(), -1.f, 1.f), spanCvNorm = clamp(inputs[SPAN_CV_INPUT].getVoltage(), -10.f, 10.f) / 5.f, spanNorm = clamp(spanParamNorm + 0.5f * spanAtten * spanCvNorm, 0.f, 1.f), spanOct = 8.f * bifurx::shapedSpan(spanNorm), spanWideMorph = cascadeWideMorph(spanNorm);
	const bool slowCvConnected = resoCvConnected || balanceCvConnected || spanCvConnected;
	const bool audioRateControlsActive = !titoNeutral || voctConnected || fmConnected;
	const bool fastPathEligible = titoNeutral && !voctConnected && !fmConnected && !slowCvConnected;
	perfSampleRate.store(args.sampleRate, std::memory_order_relaxed); perfMode.store(mode, std::memory_order_relaxed); perfFastPathEligible.store(fastPathEligible, std::memory_order_relaxed);
	const bool controlDividerTick = controlUpdateDivider.process();
	const bool forceAudioRateControls = controlUpdateMode == CONTROL_UPDATE_AUDIO_RATE;
	const bool updateFastControls =
		!controlFastCacheValid || audioRateControlsActive || (forceAudioRateControls && slowCvConnected) || controlDividerTick;
	if (std::fabs(previewFilterAlphaSampleRate - args.sampleRate) > 0.5f) { previewFilterAlpha = onePoleAlpha(1.f / std::max(args.sampleRate, 1.f), 0.05f); previewFilterAlphaSlow = onePoleAlpha(1.f / std::max(args.sampleRate, 1.f), 0.20f); previewFilterAlphaSampleRate = args.sampleRate; }
	if (std::fabs(llTelemetryAlphaSampleRate - args.sampleRate) > 0.5f) {
		llTelemetryAlpha = onePoleAlpha(1.f / std::max(args.sampleRate, 1.f), kLlTelemetryTauSeconds);
		llTelemetryAlphaSampleRate = args.sampleRate;
	}

	float freqA0 = cachedFreqA0, freqB0 = cachedFreqB0, dampingA = cachedDampingA, dampingB = cachedDampingB, wA = cachedWA, wB = cachedWB, balance = cachedBalance;
	if (updateFastControls) {
		balance = balanceNorm; const float centerHz = kFreqMinHz * fastExp2(kFreqLog2Span * freqParamNorm) * fastExp2(voctCv + fm), sr = std::max(args.sampleRate, 1.f);
		auto computeFreqs = [&](float* fAOut, float* fBOut) { const float safeCenterHz = clamp(centerHz, kFreqMinHz, 0.46f * sr), maxShiftUp = std::max(0.f, std::log2((0.46f * sr) / safeCenterHz)), maxShiftDown = std::max(0.f, std::log2(safeCenterHz / kFreqMinHz)), maxSymShift = std::min(maxShiftUp, maxShiftDown), halfSpanOct = std::min(0.5f * spanOct, maxSymShift); if (fAOut) *fAOut = clamp(safeCenterHz * fastExp2(-halfSpanOct), kFreqMinHz, 0.46f * sr); if (fBOut) *fBOut = clamp(safeCenterHz * fastExp2(halfSpanOct), kFreqMinHz, 0.46f * sr); };
		computeFreqs(&freqA0, &freqB0); const float baseDamping = resoToDamping(resoNorm);
		dampingA = clamp(baseDamping * fastExp(0.48f * balance), 0.02f, 2.2f); dampingB = clamp(baseDamping * fastExp(-0.48f * balance), 0.02f, 2.2f);
		const float lowW = signedWeight(balance, false), highW = signedWeight(balance, true), norm = 2.f / (lowW + highW); wA = lowW * norm; wB = highW * norm;
		cachedDampingA = dampingA; cachedDampingB = dampingB; cachedWA = wA; cachedWB = wB; cachedFreqA0 = freqA0; cachedFreqB0 = freqB0; cachedBalance = balance; cachedCoeffsA = makeSvfCoeffs(args.sampleRate, freqA0, dampingA); cachedCoeffsB = makeSvfCoeffs(args.sampleRate, freqB0, dampingB); controlFastCacheValid = true;
	}

	const float titoModeScale = 1.22f, titoStrength = 2.4f * titoAbs, couplingDepth = titoStrength * titoModeScale * (0.026f + 0.28f * resoNorm * resoNorm);
	const float drivenIn = applyLevelInputStage(in, level);
	const float oscNorm = highResonanceSelfOscEnabled
		? smoothstep01((clamp01(resoNorm) - kSelfOscResoStart) / (kSelfOscResoFull - kSelfOscResoStart))
		: 0.f;
	const float selfOscSeed = (oscNorm > 0.f) ? (2e-7f + 8e-7f * oscNorm) : 0.f;
	const float excitation = drivenIn + selfOscSeed;
	float cutoffA = freqA0, cutoffB = freqB0;
	if (!titoNeutral) {
		const float depthScaled = couplingDepth * 0.2f;
		float modA = 0.f, modB = 0.f;
		if (tito < 0.f) { modA = depthScaled * coreA.ic1eq; modB = depthScaled * coreB.ic1eq; }
		else { modA = depthScaled * coreB.ic1eq; modB = depthScaled * coreA.ic1eq; }
		cutoffA = freqA0 * fastExp2(clamp(modA, -2.5f, 2.5f)); cutoffB = freqB0 * fastExp2(clamp(modB, -2.5f, 2.5f));
	}
	if (measurePerf) perfCoreStart = PerfClock::now();
	float modeOut = 0.f, llExc = 0.f, llA = 0.f, llB = 0.f;
	auto pA = [&](float s) {
		const SvfCoeffs* coeffs = (fastPathEligible || (cutoffA == freqA0)) ? &cachedCoeffsA : nullptr;
		return processCharacterStage(
			coreA, 0, s, args.sampleRate, cutoffA, dampingA, drive, resoNorm, highResonanceSelfOscEnabled, coeffs
		);
	};
	auto pB = [&](float s) {
		const SvfCoeffs* coeffs = (fastPathEligible || (cutoffB == freqB0)) ? &cachedCoeffsB : nullptr;
		return processCharacterStage(
			coreB, 1, s, args.sampleRate, cutoffB, dampingB, drive, resoNorm, highResonanceSelfOscEnabled, coeffs
		);
	};

	switch (mode) {
		case 0: { const SvfOutputs a = pA(excitation), b = pB(a.lp); llExc = excitation; llA = a.lp; llB = b.lp; modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, b.lp, 0.f, 0.f, 0.f, 0.f, 0.f, wA, wB, spanWideMorph); } break;
		case 1: { const SvfOutputs a = pA(excitation), b = pB(excitation); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, wA, wB, spanWideMorph); } break;
		case 2: { const SvfOutputs a = pA(excitation), b = pB(a.notch); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, b.lp, 0.f, 0.f, 0.f, wA, wB, spanWideMorph); } break;
		case 3: { const SvfOutputs a = pA(excitation), b = pB(a.notch); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, b.notch, 0.f, 0.f, 0.f, 0.f, wA, wB, spanWideMorph); } break;
		case 4: { const SvfOutputs a = pA(excitation), b = pB(excitation); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, wA, wB, spanWideMorph); } break;
		case 5: { const SvfOutputs a = pA(excitation), b = pB(excitation); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, wA, wB, spanWideMorph); } break;
		case 6: { const SvfOutputs a = pA(excitation), b = pB(a.hp); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, b.lp, 0.f, 0.f, wA, wB, spanWideMorph); } break;
		case 7: { const SvfOutputs a = pA(excitation), b = pB(a.hp); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, 0.f, b.notch, 0.f, wA, wB, spanWideMorph); } break;
		case 8: { const SvfOutputs a = pA(excitation), b = pB(excitation); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, wA, wB, spanWideMorph); } break;
		case 9: { const SvfOutputs a = pA(excitation), b = pB(a.hp); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, 0.f, 0.f, b.hp, wA, wB, spanWideMorph); } break;
		default: {
			const float peak2Hz = resamplePeak2HzFromFreqPair(freqA0, freqB0, args.sampleRate);
			const float firstNotchHz = resampleFirstNullHzFromPeak2(peak2Hz, args.sampleRate);
			const float peak1Hz = resamplePeak1HzFromFirstNull(firstNotchHz);
			const float rolloffHz = resampleRolloffHzFromPeakStandins(peak1Hz, resamplePeak2HzFromFirstNull(firstNotchHz), args.sampleRate, spanWideMorph);
			modeOut = resampleFilterCore.process(excitation, args.sampleRate, firstNotchHz, rolloffHz, spanWideMorph, resoNorm);
		} break;
	}

	const float out = applyLevelOutputStage(modeOut, level, softLimitingEnabled);
	outputs[OUT_OUTPUT].setChannels(1); outputs[OUT_OUTPUT].setVoltage(out);
	const float llAlpha = llTelemetryAlpha;
	if (mode == 0) { llTelemetryExcitationSq += llAlpha * (llExc * llExc - llTelemetryExcitationSq); llTelemetryStageALpSq += llAlpha * (llA * llA - llTelemetryStageALpSq); llTelemetryStageBLpSq += llAlpha * (llB * llB - llTelemetryStageBLpSq); llTelemetryOutputSq += llAlpha * (out * out - llTelemetryOutputSq); }
	else { llTelemetryExcitationSq += llAlpha * (0.f - llTelemetryExcitationSq); llTelemetryStageALpSq += llAlpha * (0.f - llTelemetryStageALpSq); llTelemetryStageBLpSq += llAlpha * (0.f - llTelemetryStageBLpSq); llTelemetryOutputSq += llAlpha * (out * out - llTelemetryOutputSq); }
	if (measurePerf) perfPreviewStart = PerfClock::now();

	const float pTFqA = clamp(freqA0, 4.f, 0.46f * args.sampleRate), pTFqB = clamp(freqB0, 4.f, 0.46f * args.sampleRate), pTQA = 1.f / clamp(dampingA, kSvfDampingMin, kSvfDampingMax), pTQB = 1.f / clamp(dampingB, kSvfDampingMin, kSvfDampingMax), pTBal = balance;
	const bool pPitchCvConn = voctConnected || inputs[FM_INPUT].isConnected();
	perfPreviewPitchCvConnected.store(pPitchCvConn, std::memory_order_relaxed);
	const float pSmAlpha = pPitchCvConn ? previewFilterAlphaSlow : previewFilterAlpha;
	if (!previewTargetMotionInitialized) { previewPrevTargetFreqA = pTFqA; previewPrevTargetFreqB = pTFqB; previewTargetStillSamples = 0; previewTargetMotionInitialized = true; }
	const float tMAOct = std::fabs(std::log2(std::max(pTFqA, 1.f) / std::max(previewPrevTargetFreqA, 1.f))), tMBOct = std::fabs(std::log2(std::max(pTFqB, 1.f) / std::max(previewPrevTargetFreqB, 1.f))), tMOct = std::max(tMAOct, tMBOct);
	if (tMOct <= kPreviewInstantSettleMotionOctThreshold) previewTargetStillSamples++; else previewTargetStillSamples = 0;
	const bool pInstSettle = (previewTargetStillSamples >= kPreviewInstantSettleHoldSamples);
	previewPrevTargetFreqA = pTFqA; previewPrevTargetFreqB = pTFqB;
	if (!previewFilterInitialized || pInstSettle) { previewFreqAFiltered = pTFqA; previewFreqBFiltered = pTFqB; previewQAFiltered = pTQA; previewQBFiltered = pTQB; previewBalanceFiltered = pTBal; previewFilterInitialized = true; }
	else { const float a = pSmAlpha; previewFreqAFiltered += a * (pTFqA - previewFreqAFiltered); previewFreqBFiltered += a * (pTFqB - previewFreqBFiltered); previewQAFiltered += a * (pTQA - previewQAFiltered); previewQBFiltered += a * (pTQB - previewQBFiltered); previewBalanceFiltered += a * (pTBal - previewBalanceFiltered); }

	BifurxPreviewState pS; pS.sampleRate = args.sampleRate; pS.freqA = previewFreqAFiltered; pS.freqB = previewFreqBFiltered; pS.qA = previewQAFiltered; pS.qB = previewQBFiltered; pS.mode = mode; pS.balance = previewBalanceFiltered; pS.balanceTarget = balanceNorm; pS.resoNorm = resoNorm; pS.spanParamNorm = spanParamNorm; pS.spanCvNorm = spanCvNorm; pS.spanAtten = spanAtten; pS.spanNorm = spanNorm; pS.spanOct = spanOct; pS.freqParamNorm = freqParamNorm; pS.voctCv = voctCv;
	if (previewAdaptiveCooldown > 0) previewAdaptiveCooldown--;
	const bool perTick = pPitchCvConn ? previewPublishSlowDivider.process() : previewPublishDivider.process();
	bool adpTick = false;
	if (hasLastPreviewState && previewAdaptiveCooldown <= 0 && perTick) {
		const float fMA = std::fabs(fastLog2(std::max(pS.freqA, 1.f)) - fastLog2(std::max(lastPreviewState.freqA, 1.f))), fMB = std::fabs(fastLog2(std::max(pS.freqB, 1.f)) - fastLog2(std::max(lastPreviewState.freqB, 1.f))), sMO = std::fabs(pS.spanOct - lastPreviewState.spanOct), qMA = std::fabs(pS.qA - lastPreviewState.qA), qMB = std::fabs(pS.qB - lastPreviewState.qB), bM = std::fabs(pS.balance - lastPreviewState.balance);
		if (fMA > kPreviewAdaptiveOctaveThreshold || fMB > kPreviewAdaptiveOctaveThreshold || sMO > kPreviewAdaptiveSpanOctThreshold || qMA > kPreviewAdaptiveQThreshold || qMB > kPreviewAdaptiveQThreshold || bM > kPreviewAdaptiveBalanceThreshold) { adpTick = true; previewAdaptiveCooldown = kPreviewAdaptiveCooldownSamples; }
	}
	if (!hasLastPreviewState || ((perTick || adpTick) && previewStatesDiffer(pS, lastPreviewState))) publishPreviewState(pS);
	if (perTick || adpTick) { BifurxLlTelemetryState llTS; llTS.active = (mode == 0); llTS.excitationRms = std::sqrt(std::max(llTelemetryExcitationSq, 0.f)); llTS.stageALpRms = std::sqrt(std::max(llTelemetryStageALpSq, 0.f)); llTS.stageBLpRms = std::sqrt(std::max(llTelemetryStageBLpSq, 0.f)); llTS.outputRms = std::sqrt(std::max(llTelemetryOutputSq, 0.f)); llTS.stageBLpOverALpDb = amplitudeRatioDb(llTS.stageBLpRms, llTS.stageALpRms); llTS.outputOverInputDb = amplitudeRatioDb(llTS.outputRms, llTS.excitationRms); publishLlTelemetryState(llTS); }
	if (measurePerf) perfAnalysisStart = PerfClock::now();
	pushAnalysisSample(in, out, modeOut);

	lights[FM_AMT_POS_LIGHT].setBrightness(std::max(fmAmt, 0.f)); lights[FM_AMT_NEG_LIGHT].setBrightness(std::max(-fmAmt, 0.f));
	lights[SPAN_CV_ATTEN_POS_LIGHT].setBrightness(std::max(spanAtten, 0.f)); lights[SPAN_CV_ATTEN_NEG_LIGHT].setBrightness(std::max(-spanAtten, 0.f));
	lights[TITO_SM_LIGHT].setBrightness(std::max(-tito, 0.f));
	lights[TITO_XM_LIGHT].setBrightness(std::max(tito, 0.f));

	if (measurePerf) {
		const PerfClock::time_point pE = PerfClock::now();
		const uint64_t cNS = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(perfCoreStart - perfStart).count();
		const uint64_t crNS = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(perfPreviewStart - perfCoreStart).count();
		const uint64_t prNS = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(perfAnalysisStart - perfPreviewStart).count();
		const uint64_t aNS = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(pE - perfAnalysisStart).count(), pNS = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(pE - perfStart).count();
		perfAudioSampledCount.fetch_add(1, std::memory_order_relaxed); perfAudioProcessNs.fetch_add(pNS, std::memory_order_relaxed);
		perfAudioControlsNs.fetch_add(cNS, std::memory_order_relaxed); perfAudioCoreNs.fetch_add(crNS, std::memory_order_relaxed);
		perfAudioPreviewNs.fetch_add(prNS, std::memory_order_relaxed); perfAudioAnalysisNs.fetch_add(aNS, std::memory_order_relaxed);
		uint64_t pM = perfAudioProcessMaxNs.load(std::memory_order_relaxed);
		while (pNS > pM && !perfAudioProcessMaxNs.compare_exchange_weak(pM, pNS, std::memory_order_relaxed));
	}
}

namespace {

inline void prepareCurveTargets(const BifurxPreviewModel& model, const float* curveHz, float* curveTargetDb) {
	for (int i = 0; i < kCurvePointCount; ++i) {
		const float db = previewModelResponseDb(model, curveHz[i]);
		curveTargetDb[i] = clamp(db, kResponseMinDb, kResponseMaxDb);
	}
}

inline float computeDisplayTopTargetDbfs(
	const float* frameSmoothedOutputDbfs,
	const float* overlayTargetOutputDbfs,
	bool fftScaleDynamic
) {
	if (!fftScaleDynamic) {
		return kDisplayTopDbfsCeiling;
	}

	float framePeakDbfs = kOverlayDbfsFloor;
	for (int i = 0; i < kCurvePointCount; ++i) {
		framePeakDbfs = std::max(framePeakDbfs, overlayTargetOutputDbfs[i]);
	}

	float sortedOutputDbfs[kCurvePointCount];
	for (int i = 0; i < kCurvePointCount; i++) sortedOutputDbfs[i] = frameSmoothedOutputDbfs[i];
	const int p95Index = int(0.95f * float(kCurvePointCount - 1));
	std::nth_element(sortedOutputDbfs, sortedOutputDbfs + p95Index, sortedOutputDbfs + kCurvePointCount);
	const float robustTopRefDbfs = std::max(sortedOutputDbfs[p95Index], framePeakDbfs - 18.f);
	return clamp(
		std::max(robustTopRefDbfs + 6.f, framePeakDbfs + kDisplayPeakHeadroomDb),
		kDisplayTopDbfsFloor,
		kDisplayTopDynamicCeilingDbfs
	);
}

inline void prepareOverlayTargetsFromSpectra(
	float sampleRate,
	const float* curveBinPos,
	const float* fftOutputFreq,
	const float* fftResponseOutputFreq,
	const float* fftRawInputFreq,
	bool hasOverlayTarget,
	bool fftScaleDynamic,
	float* overlayTargetModuleDb,
	float* overlayTargetOutputDbfs,
	float* displayTopTargetDbfs
) {
	float binOutputDbfs[kFftBinCount];
	float binOutputPower[kFftBinCount];
	float binResponseOutputPower[kFftBinCount];
	float binRawInputPower[kFftBinCount];
	float binModuleDeltaDb[kFftBinCount];
	const float amplitudeScale = 4.f / float(kFftSize);
	const float amplitudeScaleSq = amplitudeScale * amplitudeScale;
	for (int bin = 0; bin < kFftBinCount; ++bin) {
		const float binHz = (float(bin) * sampleRate) / float(kFftSize);
		const float subsonicWeight = clamp01((binHz - kOverlaySubsonicCutHz) / (kOverlaySubsonicFadeHz - kOverlaySubsonicCutHz));
		const float weightedPowerScale = subsonicWeight * subsonicWeight * amplitudeScaleSq;
		binOutputPower[bin] = weightedPowerScale * orderedSpectrumPower(fftOutputFreq, bin);
		binResponseOutputPower[bin] = weightedPowerScale * orderedSpectrumPower(fftResponseOutputFreq, bin);
		binRawInputPower[bin] = weightedPowerScale * orderedSpectrumPower(fftRawInputFreq, bin);
	}

	constexpr int kOverlayBandRadius = 2;
	constexpr float kOverlayBandKernel[5] = {0.08f, 0.24f, 0.36f, 0.24f, 0.08f};
	for (int bin = 0; bin < kFftBinCount; ++bin) {
		float outputEnergy = 0.f;
		float responseOutputEnergy = 0.f;
		float rawInputEnergy = 0.f;
		for (int k = -kOverlayBandRadius; k <= kOverlayBandRadius; ++k) {
			const int sampleBin = clamp(bin + k, 0, kFftBinCount - 1);
			const float w = kOverlayBandKernel[k + kOverlayBandRadius];
			outputEnergy += w * binOutputPower[sampleBin];
			responseOutputEnergy += w * binResponseOutputPower[sampleBin];
			rawInputEnergy += w * binRawInputPower[sampleBin];
		}
		rawInputEnergy += 1e-12f;
		binModuleDeltaDb[bin] = softLimitOverlayDeltaDb(10.f * std::log10((responseOutputEnergy + 1e-12f) / rawInputEnergy));
		outputEnergy += 1e-12f;
		binOutputDbfs[bin] = clamp(10.f * std::log10(outputEnergy / 25.f + 1e-12f), kOverlayDbfsFloor, kOverlayDbfsCeiling);
	}

	float sampledOutputDbfs[kCurvePointCount];
	float sampledModuleDeltaDb[kCurvePointCount];
	for (int i = 0; i < kCurvePointCount; ++i) {
		const float binPos = curveBinPos[i];
		const int binA = std::max(2, int(std::floor(binPos)));
		const int binB = std::min(binA + 1, kFftSize / 2);
		const float frac = binPos - float(binA);
		sampledOutputDbfs[i] = mixf(binOutputDbfs[binA], binOutputDbfs[binB], frac);
		sampledModuleDeltaDb[i] = mixf(binModuleDeltaDb[binA], binModuleDeltaDb[binB], frac);
	}

	float frameSmoothedOutputDbfs[kCurvePointCount];
	const float targetSmoothing = hasOverlayTarget ? 0.45f : 1.f;
	for (int i = 0; i < kCurvePointCount; ++i) {
		const int left = std::max(0, i - 1), right = std::min(kCurvePointCount - 1, i + 1);
		const float smoothOutputDbfs = 0.12f * sampledOutputDbfs[left] + 0.76f * sampledOutputDbfs[i] + 0.12f * sampledOutputDbfs[right];
		frameSmoothedOutputDbfs[i] = smoothOutputDbfs;
		const float smoothModuleDeltaDb = 0.12f * sampledModuleDeltaDb[left] + 0.76f * sampledModuleDeltaDb[i] + 0.12f * sampledModuleDeltaDb[right];
		overlayTargetModuleDb[i] = mixf(overlayTargetModuleDb[i], smoothModuleDeltaDb, targetSmoothing);
		overlayTargetOutputDbfs[i] = mixf(overlayTargetOutputDbfs[i], smoothOutputDbfs, targetSmoothing);
	}

	*displayTopTargetDbfs = computeDisplayTopTargetDbfs(frameSmoothedOutputDbfs, overlayTargetOutputDbfs, fftScaleDynamic);
}

} // namespace

void BifurxSpectrumBase::syncBase() {
	if (!module) return;
	const uint32_t previewSeq = module->previewPublishSeq.load(std::memory_order_acquire);
	if (previewSeq != state.lastPreviewSeq) {
		const int index = module->previewPublishedIndex.load(std::memory_order_acquire);
		state.previewState = module->previewStates[index];
		state.hasPreview = true;
		state.lastPreviewSeq = previewSeq;
		updateAxisCache();
		const auto curvePrepStart = std::chrono::steady_clock::now();
		updateCurveCache();
		lastCurvePrepUs = float(std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::steady_clock::now() - curvePrepStart).count());
	}

	const uint32_t analysisSeq = module->analysisPublishSeq.load(std::memory_order_acquire);
	if (analysisSeq != state.lastAnalysisSeq) {
		const int index = module->analysisPublishedIndex.load(std::memory_order_acquire);
		const auto overlayPrepStart = std::chrono::steady_clock::now();
		updateOverlayCache(module->analysisFrames[index]);
		lastOverlayPrepUs = float(std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::steady_clock::now() - overlayPrepStart).count());
		state.hasOverlay = true;
		state.lastAnalysisSeq = analysisSeq;
	}
}

void BifurxSpectrumBase::updateAxisCache() {
	if (std::fabs(state.cachedAxisSampleRate - state.previewState.sampleRate) < 0.5f) return;
	state.cachedAxisSampleRate = state.previewState.sampleRate;
	const float minHz = 10.f;
	const float maxHz = std::min(20000.f, 0.46f * state.cachedAxisSampleRate);
	for (int i = 0; i < kCurvePointCount; i++) {
		const float x01 = float(i) / float(kCurvePointCount - 1);
		const float hz = logFrequencyAt(x01, minHz, maxHz);
		state.curveHz[i] = hz;
		state.curveBinPos[i] = (hz * float(kFftSize)) / state.cachedAxisSampleRate;
	}
}

void BifurxSpectrumBase::updateCurveCache() {
	if (!state.hasPreview) return;
	updateAxisCache();
	const BifurxPreviewModel& model = getOrUpdateModel();
	prepareCurveTargets(model, state.curveHz, state.curveTargetDb);
	if (!state.hasCurveTarget) {
		for (int i = 0; i < kCurvePointCount; i++) state.curveDb[i] = state.curveTargetDb[i];
		state.hasCurveTarget = true;
	}
}

const BifurxPreviewModel& BifurxSpectrumBase::getOrUpdateModel() const {
	if (state.lastPreviewSeq != lastModelUpdateSeq) {
		cachedModel = makePreviewModel(state.previewState);
		const_cast<BifurxSpectrumBase*>(this)->lastModelUpdateSeq = state.lastPreviewSeq;
	}
	return cachedModel;
}

void BifurxSpectrumBase::updateOverlayCache(const BifurxAnalysisFrame& frame) {
	if (!state.hasPreview) return;
	updateAxisCache();
	for (int i = 0; i < kFftSize; i++) fftOutputTime[i] = frame.output[i] * window[i];
	fft.rfft(fftOutputTime, fftOutputFreq);
	for (int i = 0; i < kFftSize; i++) fftOutputTime[i] = frame.responseOutput[i] * window[i];
	fft.rfft(fftOutputTime, fftResponseOutputFreq);
	for (int i = 0; i < kFftSize; i++) fftInputTime[i] = frame.rawInput[i] * window[i];
	fft.rfft(fftInputTime, fftRawInputFreq);
	const bool fftScaleDynamic = module ? module->fftScaleDynamic : true;
	prepareOverlayTargetsFromSpectra(
		state.previewState.sampleRate,
		state.curveBinPos,
		fftOutputFreq,
		fftResponseOutputFreq,
		fftRawInputFreq,
		state.hasOverlayTarget,
		fftScaleDynamic,
		state.overlayTargetModuleDb,
		state.overlayTargetOutputDbfs,
		&state.displayTopTargetDbfs
	);

	if (!state.hasOverlayTarget) {
		for (int i = 0; i < kCurvePointCount; i++) {
			state.overlayModuleDb[i] = state.overlayTargetModuleDb[i];
			state.overlayOutputDbfs[i] = state.overlayTargetOutputDbfs[i];
		}
		state.hasOverlayTarget = true;
	}
}

bool BifurxSpectrumBase::updateAnimation(float dt) {
	bool animationActive = false;
	constexpr float kCurveEpsilonDb = 0.01f;
	constexpr float kOverlayEpsilonDb = 0.02f;
	constexpr float kTopEpsilonDbfs = 0.02f;

	if (state.hasCurveTarget) {
		const float curveMaxStepDb = std::max(0.25f, kCurveVisualSlewDbPerSec * dt);
		float maxCurveResidualDb = 0.f;
		for (int i = 0; i < kCurvePointCount; ++i) {
			const float prev = state.curveDb[i];
			float delta = state.curveTargetDb[i] - prev;
			delta = clamp(delta, -curveMaxStepDb, curveMaxStepDb);
			state.curveDb[i] = prev + delta;
			maxCurveResidualDb = std::max(maxCurveResidualDb, std::fabs(state.curveTargetDb[i] - state.curveDb[i]));
		}
		if (maxCurveResidualDb <= kCurveEpsilonDb) {
			for (int i = 0; i < kCurvePointCount; ++i) {
				state.curveDb[i] = state.curveTargetDb[i];
			}
			state.hasCurveTarget = false;
		}
		else {
			animationActive = true;
		}
	}

	if (state.hasOverlayTarget) {
		const float overlayDbSmoothing = 0.22f;
		const float overlayLevelSmoothing = 0.20f;
		float maxOverlayResidualDb = 0.f;
		for (int i = 0; i < kCurvePointCount; ++i) {
			state.overlayModuleDb[i] = mixf(state.overlayModuleDb[i], state.overlayTargetModuleDb[i], overlayDbSmoothing);
			state.overlayOutputDbfs[i] = mixf(state.overlayOutputDbfs[i], state.overlayTargetOutputDbfs[i], overlayLevelSmoothing);
			const float moduleResidual = std::fabs(state.overlayTargetModuleDb[i] - state.overlayModuleDb[i]);
			const float outputResidual = std::fabs(state.overlayTargetOutputDbfs[i] - state.overlayOutputDbfs[i]);
			maxOverlayResidualDb = std::max(maxOverlayResidualDb, std::max(moduleResidual, outputResidual));
		}

		const float prevTop = state.displayTopDbfs;
		float topSmoothing = (state.displayTopTargetDbfs > prevTop) ? 0.22f : 0.10f;
		if (module && module->fftScaleDynamic && state.displayTopTargetDbfs > prevTop) {
			topSmoothing = 0.70f;
		}
		state.displayTopDbfs = mixf(prevTop, state.displayTopTargetDbfs, topSmoothing);
		const float topResidualDbfs = std::fabs(state.displayTopTargetDbfs - state.displayTopDbfs);

		if (maxOverlayResidualDb <= kOverlayEpsilonDb && topResidualDbfs <= kTopEpsilonDbfs) {
			for (int i = 0; i < kCurvePointCount; ++i) {
				state.overlayModuleDb[i] = state.overlayTargetModuleDb[i];
				state.overlayOutputDbfs[i] = state.overlayTargetOutputDbfs[i];
			}
			state.displayTopDbfs = state.displayTopTargetDbfs;
			state.hasOverlayTarget = false;
		}
		else {
			animationActive = true;
		}
	}

	return animationActive;
}

BifurxRenderTickResult BifurxSpectrumBase::runRenderTick(float dt) {
	BifurxRenderTickResult result;
	const uint32_t prevPreviewSeq = state.lastPreviewSeq;
	const uint32_t prevAnalysisSeq = state.lastAnalysisSeq;

	syncBase();
	result.previewUpdated = (state.lastPreviewSeq != prevPreviewSeq);
	result.analysisUpdated = (state.lastAnalysisSeq != prevAnalysisSeq);
	result.animationActive = updateAnimation(dt);
	result.curvePrepUs = result.previewUpdated ? lastCurvePrepUs : 0.f;
	result.overlayPrepUs = result.analysisUpdated ? lastOverlayPrepUs : 0.f;
	return result;
}

void BifurxSpectrumBase::calculateMarkerLayout(BifurxMarkerLayout* layout, float w, float h) const {
	if (!layout) return;
	const float padY = std::max(4.f, h * 0.035f);
	const float labelBandHeight = std::max(5.2f, h * 0.072f), labelBandTop = h - labelBandHeight;
	const float spectrumTopY = padY * 0.35f, spectrumBottomY = std::max(spectrumTopY + 1.f, labelBandTop - std::max(0.05f, h * 0.0008f));
	const float minHz = 10.f, maxHz = std::min(20000.f, 0.46f * state.previewState.sampleRate);
	const float markerOuterRadius = kPeakMarkerFillRadius + kPeakMarkerOutlineExtraRadius + 0.5f * kPeakMarkerOutlineStrokeWidth;
	const float markerBottomLaneY = spectrumBottomY - markerOuterRadius - kPeakMarkerBottomLanePadding;
	const BifurxPreviewModel& model = getOrUpdateModel();

	layout->anchorToBottomLane = markerPinnedToBottomLane(0) || markerPinnedToBottomLane(1);
	layout->markers[0].visible = false; layout->markers[1].visible = false;

	auto populateMarker = [&](int mIdx, float targetHz) {
		auto& m = layout->markers[mIdx];
		const auto anchor = displayAnchorForMarker(mIdx, targetHz, minHz, maxHz);
		const float mX = w * anchor.x01;
		if (mX < markerOuterRadius + kPeakMarkerEdgePadding || mX > w - markerOuterRadius - kPeakMarkerEdgePadding) {
			return;
		}
		m.x = mX;
		m.yCurve = curveYAtX01(anchor.x01, spectrumBottomY, spectrumTopY);
		const float mMinY = spectrumTopY + markerOuterRadius + kPeakMarkerEdgePadding, mMaxY = spectrumBottomY - markerOuterRadius - kPeakMarkerEdgePadding;
		m.yMarker = markerPinnedToBottomLane(mIdx) ? markerBottomLaneY : clamp(m.yCurve, mMinY, mMaxY);
		m.hz = std::max(anchor.hz, 1e-6f);
		m.visible = true;
		formatFrequencyLabel(m.hz, m.label, sizeof(m.label));
	};

	populateMarker(0, model.markerFreqA);
	populateMarker(1, model.markerFreqB);

	layout->labelX[0] = layout->markers[0].x;
	layout->labelX[1] = layout->markers[1].x;
	const float labelMargin = std::max(18.f, w * 0.08f), minLabelSeparation = std::max(30.f, w * 0.18f), minX = labelMargin, maxX = w - labelMargin;
	if (layout->markers[0].visible && layout->markers[1].visible) {
		const int leftIndex = (layout->labelX[0] <= layout->labelX[1]) ? 0 : 1, rightIndex = 1 - leftIndex;
		float leftX = clamp(layout->labelX[leftIndex], minX, maxX), rightX = clamp(layout->labelX[rightIndex], minX, maxX), needed = std::min(minLabelSeparation, std::max(0.f, maxX - minX)) - (rightX - leftX);
		if (needed > 0.f) {
			float moveLeft = std::min(0.5f * needed, leftX - minX), moveRight = std::min(0.5f * needed, maxX - rightX);
			leftX -= moveLeft; rightX += moveRight; needed -= (moveLeft + moveRight);
			if (needed > 0.f) { float extraLeft = std::min(needed, leftX - minX); leftX -= extraLeft; needed -= extraLeft; }
			if (needed > 0.f) rightX += std::min(needed, maxX - rightX);
		}
		layout->labelX[leftIndex] = leftX; layout->labelX[rightIndex] = rightX;
	} else {
		for (int i = 0; i < 2; ++i) if (layout->markers[i].visible) layout->labelX[i] = clamp(layout->labelX[i], minX, maxX);
	}

	layout->labelFontSize = std::max(7.f, h * 0.055f);
	layout->labelY = labelBandTop + 0.5f * labelBandHeight;
	layout->guideYBottom = clamp(labelBandTop + std::min(2.1f, 0.18f * labelBandHeight), labelBandTop + 0.2f, layout->labelY - 0.5f * layout->labelFontSize - 0.6f);
}

void BifurxSpectrumBase::calculateRefinedCurvePoints(std::vector<BifurxCurvePoint>* points, float w, float h) const {
	if (!points) return;
	points->clear();
	const size_t refinedPointReserve = size_t(kCurvePointCount) + 6;
	if (points->capacity() < refinedPointReserve) {
		points->reserve(refinedPointReserve);
	}
	
	const float padY = std::max(4.f, h * 0.035f);
	const float labelBandHeight = std::max(5.2f, h * 0.072f), labelBandTop = h - labelBandHeight;
	const float spectrumTopY = padY * 0.35f, spectrumBottomY = std::max(spectrumTopY + 1.f, labelBandTop - std::max(0.05f, h * 0.0008f));
	const float minHz = 10.f, maxHz = std::min(20000.f, 0.46f * state.previewState.sampleRate);
	const BifurxPreviewModel& model = getOrUpdateModel();
	const float markerOuterRadius = kPeakMarkerFillRadius + kPeakMarkerOutlineExtraRadius + 0.5f * kPeakMarkerOutlineStrokeWidth;
	const float markerBottomLaneY = spectrumBottomY - markerOuterRadius - kPeakMarkerBottomLanePadding;

	// Initial grid points
	for (int i = 0; i < kCurvePointCount; ++i) {
		points->push_back({float(i) / float(kCurvePointCount - 1), 0.f, 0});
	}

	auto addRefinement = [&](int mIdx, float targetHz) {
		const auto anchor = displayAnchorForMarker(mIdx, targetHz, minHz, maxHz);
		const float dx = 0.35f / float(kCurvePointCount - 1);
		points->push_back({clamp(anchor.x01 - dx, 0.f, 1.f), 0.f, 1});
		points->push_back({clamp(anchor.x01, 0.f, 1.f), 0.f, 2});
		points->push_back({clamp(anchor.x01 + dx, 0.f, 1.f), 0.f, 1});
	};

	addRefinement(0, model.markerFreqA);
	addRefinement(1, model.markerFreqB);
	if (state.previewState.mode == 10) {
		const float peak2Hz = std::max(model.markerFreqA, model.markerFreqB);
		const float firstNotchHz = resampleFirstNullHzFromPeak2(peak2Hz, state.previewState.sampleRate);
		const float peak1Hz = resamplePeak1HzFromFirstNull(firstNotchHz);
		const float rolloffHz = resampleRolloffHzFromPeakStandins(peak1Hz, resamplePeak2HzFromFirstNull(firstNotchHz), state.previewState.sampleRate, cascadeWideMorph(state.previewState.spanNorm));
		if (firstNotchHz >= minHz && firstNotchHz <= maxHz) {
			const float firstNullX01 = logPosition(firstNotchHz, minHz, maxHz);
			const float dx = 0.35f / float(kCurvePointCount - 1);
			points->push_back({clamp(firstNullX01 - dx, 0.f, 1.f), 0.f, 1});
			points->push_back({clamp(firstNullX01, 0.f, 1.f), 0.f, 2});
			points->push_back({clamp(firstNullX01 + dx, 0.f, 1.f), 0.f, 1});
		}
		if (rolloffHz >= minHz && rolloffHz <= maxHz) {
			const float rolloffX01 = logPosition(rolloffHz, minHz, maxHz);
			const float dx = 0.35f / float(kCurvePointCount - 1);
			points->push_back({clamp(rolloffX01 - dx, 0.f, 1.f), 0.f, 1});
			points->push_back({clamp(rolloffX01, 0.f, 1.f), 0.f, 2});
			points->push_back({clamp(rolloffX01 + dx, 0.f, 1.f), 0.f, 1});
		}
	}

	std::sort(points->begin(), points->end(), [](const BifurxCurvePoint& a, const BifurxCurvePoint& b) {
		if (std::fabs(a.x01 - b.x01) > 1e-7f) return a.x01 < b.x01;
		return a.priority > b.priority;
	});
	points->erase(std::unique(points->begin(), points->end(), [](const BifurxCurvePoint& a, const BifurxCurvePoint& b) {
		return std::fabs(a.x01 - b.x01) < 1e-7f;
	}), points->end());

	// Evaluate Y coordinates for all final points
	for (auto& p : *points) {
		p.y = curveYAtX01(p.x01, spectrumBottomY, spectrumTopY);
	}
	for (int markerIndex = 0; markerIndex < 2; ++markerIndex) {
		if (!markerPinnedToBottomLane(markerIndex)) continue;
		const auto anchor = displayAnchorForMarker(markerIndex, markerIndex == 0 ? model.markerFreqA : model.markerFreqB, minHz, maxHz);
		for (auto& p : *points) {
			if (p.priority == 2 && std::fabs(p.x01 - anchor.x01) < 1e-7f) {
				p.y = markerBottomLaneY;
			}
		}
	}
}

} // namespace bifurx
