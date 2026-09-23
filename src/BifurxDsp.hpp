#pragma once

#include "BifurxTypes.hpp"
#include "plugin.hpp"

#include <algorithm>
#include <cmath>
#include <complex>
#include <cstdint>
#include <string>

namespace bifurx {

inline float fastExp2(float x) {
	return rack::dsp::exp2_taylor5(clamp(x, -24.f, 24.f));
}

inline float fastExp(float x) {
	return fastExp2(x * kLog2e);
}

inline float fastLog2(float x) {
	union { float f; uint32_t i; } vx = {x};
	float y = (float)vx.i;
	y *= 1.1920928955078125e-7f;
	return y - 126.94269504f;
}

inline float fastTan(float x) {
	const float x2 = x * x;
	return x * (15.f - x2) / (15.f - 6.f * x2);
}

inline float amplitudeRatioDb(float numerator, float denominator) {
	return 20.f * std::log10((std::fabs(numerator) + 1e-6f) / (std::fabs(denominator) + 1e-6f));
}

std::string bifurxUserRootPath();

float shapedSpan(float value);

inline float sanitizeFinite(float x, float fallback = 0.f) {
	return std::isfinite(x) ? x : fallback;
}

inline float mixf(float a, float b, float t) {
	return a + (b - a) * t;
}

inline float orderedSpectrumMagnitude(const float* fftData, int bin) {
	if (bin <= 0) {
		return std::fabs(fftData[0]);
	}
	if (bin >= kFftSize / 2) {
		return std::fabs(fftData[1]);
	}
	const float re = fftData[2 * bin];
	const float im = fftData[2 * bin + 1];
	return std::sqrt(re * re + im * im);
}

inline float orderedSpectrumPower(const float* fftData, int bin) {
	if (bin <= 0) {
		return fftData[0] * fftData[0];
	}
	if (bin >= kFftSize / 2) {
		return fftData[1] * fftData[1];
	}
	const float re = fftData[2 * bin];
	const float im = fftData[2 * bin + 1];
	return re * re + im * im;
}

float onePoleAlpha(float dt, float tauSeconds);
float logPosition(float hz, float minHz, float maxHz);
float logFrequencyAt(float x01, float minHz, float maxHz);
float bifurxFrequencyHzFromParam(float paramValue);
float bifurxParamFromFrequencyHz(float hz);
float bifurxSpanSemitonesFromParam(float paramValue);
float bifurxParamFromSpanSemitones(float spanSemitones);
float responseYForDbDisplay(float db, float minDb, float maxDb, float bottomY, float topY);
float resoToDamping(float resoNorm);

float signedWeight(float balance, bool upperPeak);

struct SvfOutputs {
	float lp = 0.f;
	float bp = 0.f;
	float hp = 0.f;
	float notch = 0.f;
};

struct SvfCoeffs {
	float g = 0.f;
	float k = 0.f;
	float a1 = 1.f;
};

struct CharacterStageState {
	bool selfOscillating = false;
	float oscOnset = 0.f;
	float oscAmpDamping = 0.f;
};

CharacterStageState prepareCharacterStageState(float drive, float resoNorm, bool highResonanceSelfOscEnabled);

SvfCoeffs makeSvfCoeffs(float sampleRate, float cutoff, float damping, float dampingMin = kSvfDampingMin);

struct TptSvf {
	float ic1eq = 0.f;
	float ic2eq = 0.f;

	SvfOutputs processWithCoeffs(float input, const SvfCoeffs& coeffs);
	SvfOutputs processSelfOscWithCoeffs(const SvfCoeffs& coeffs, float input, float oscOnset, float oscAmpDamping);
	SvfOutputs process(float input, float sampleRate, float cutoff, float damping);
};

void sanitizeCoreState(TptSvf& core);

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
	const SvfCoeffs* cachedCoeffsOrNull = nullptr
);

SvfOutputs processCharacterStagePrepared(
	TptSvf& core,
	float input,
	const SvfCoeffs& normalCoeffs,
	const SvfCoeffs* selfOscCoeffsOrNull,
	const CharacterStageState& character
);

DisplayBiquad makeDisplayBiquad(float sampleRate, float cutoff, float q, int type);
template <typename T>
T combineModeResponse(
	int mode,
	const T& lpA,
	const T& bpA,
	const T& hpA,
	const T& ntA,
	const T& lpB,
	const T& bpB,
	const T& hpB,
	const T& ntB,
	const T& cascadeLp,
	const T& cascadeNotch,
	const T& cascadeNotchToLow,
	const T& cascadeHpToLp,
	const T& cascadeHighToNotch,
	const T& cascadeHpToHp,
	float wA,
	float wB
) {
	switch (mode) {
		case 0:
			return cascadeLp;
		case 1: return T(0.92f) * T(wA) * lpA + T(1.18f) * T(wB) * bpB - T(0.16f) * (bpA + bpB);
		case 2: return T(1.04f) * cascadeNotchToLow;
		case 3: return T(1.03f) * cascadeNotch;
		case 4: return T(0.98f) * T(wA) * lpA + T(0.98f) * T(wB) * hpB - T(0.06f) * (bpA + bpB);
		case 5: return T(1.08f) * (T(wA) * bpA + T(wB) * bpB);
		case 6: return T(1.04f) * cascadeHpToLp;
		case 7: return T(1.04f) * cascadeHighToNotch;
		case 8: return T(1.18f) * T(wA) * bpA + T(0.92f) * T(wB) * hpB - T(0.16f) * (bpA + bpB);
		case 9: return cascadeHpToHp;
		default: return T(1.f);
	}
}

} // namespace bifurx
