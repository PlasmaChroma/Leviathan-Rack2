#include "plugin.hpp"
#include "PanelSvgUtils.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <complex>
#include <cstdint>
#include <cstring>
#include <exception>
#include <fstream>
#include <iomanip>

struct Bifurx;

namespace {

constexpr float kDefaultPanelWidthMm = 71.12f;
constexpr float kDefaultPanelHeightMm = 128.5f;
constexpr float kPi = 3.14159265358979323846f;
constexpr float kLog2e = 1.4426950408889634f;
constexpr float kFreqMinHz = 4.f;
constexpr float kFreqMaxHz = 28000.f;
constexpr float kFreqLog2Span = 12.7731392f; // log2(28000 / 4)
constexpr int kCurvePointCount = 513;
constexpr int kFftSize = 4096;
constexpr int kFftBinCount = kFftSize / 2 + 1;
constexpr int kFftHopSize = kFftSize / 2;
constexpr int kPreviewPublishFastDivision = 128;
constexpr int kPreviewPublishSlowDivision = 256;
constexpr int kPreviewAdaptiveCooldownSamples = 64;
constexpr float kPreviewAdaptiveOctaveThreshold = 0.015f;
constexpr float kPreviewAdaptiveSpanOctThreshold = 0.04f;
constexpr float kPreviewAdaptiveQThreshold = 0.05f;
constexpr float kPreviewAdaptiveBalanceThreshold = 0.015f;
constexpr float kLlTelemetryTauSeconds = 0.05f;
constexpr float kPreviewInstantSettleMotionOctThreshold = 2e-5f;
constexpr int kPreviewInstantSettleHoldSamples = 96;
constexpr int kGuideCount = 4;
const float kGuideFreqs[kGuideCount] = {20.f, 100.f, 1000.f, 10000.f};
constexpr int kBifurxModeCount = 10;
constexpr int kBifurxCircuitModeCount = 4;
// Circuit selection stays active while we normalize each core's exported
// semantic response into the shared mode algebra.
constexpr bool kBifurxTuneSvfOnly = false;
const char* const kBifurxCircuitLabels[kBifurxCircuitModeCount] = {"SVF", "DFM", "MS2", "PRD"};
constexpr int kBifurxModeParamIndex = 0;
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
	"High + High"
};
constexpr float kResponseMinDb = -48.f;
constexpr float kResponseMaxDb = 48.f;
constexpr float kOverlayDbfsFloor = -96.f;
constexpr float kOverlayDbfsCeiling = 6.f;
constexpr float kOverlaySubsonicCutHz = 10.f;
constexpr float kOverlaySubsonicFadeHz = 30.f;
constexpr float kVoctSmoothingTauSeconds = 0.0025f;
constexpr float kVoctDeadbandVolts = 0.001f;
constexpr float kDisplayDbfsSpan = 30.f;
constexpr float kDisplayTopDbfsFloor = -36.f;
constexpr float kDisplayTopDbfsCeiling = 0.f;
constexpr float kDisplayTopDynamicCeilingDbfs = kOverlayDbfsCeiling;
constexpr float kDisplayPeakHeadroomDb = 0.6f;
constexpr float kCurveVisualSlewDbPerSec = 170.f;
constexpr float kPeakMarkerFillRadius = 2.2f;
constexpr float kPeakMarkerOutlineExtraRadius = 0.4f;
constexpr float kPeakMarkerOutlineStrokeWidth = 0.8f;
constexpr float kPeakMarkerEdgePadding = 0.4f;
constexpr float kPeakMarkerBottomLanePadding = 0.f;

float clamp01(float v) {
	return clamp(v, 0.f, 1.f);
}

float fastExp2(float x) {
	return rack::dsp::exp2_taylor5(clamp(x, -24.f, 24.f));
}

float fastExp(float x) {
	return fastExp2(x * kLog2e);
}

float amplitudeRatioDb(float numerator, float denominator) {
	return 20.f * std::log10((std::fabs(numerator) + 1e-6f) / (std::fabs(denominator) + 1e-6f));
}

std::string bifurxUserRootPath() {
	return system::join(asset::user(), "Leviathan/Bifurx");
}

float shapedSpan(float value) {
	return std::pow(clamp01(value), 1.45f);
}

float levelDriveGain(float knob) {
	const float x = clamp01(knob);
	return 0.06f + 0.95f * x + 3.6f * x * x * x;
}

float softClip(float x) {
	return std::tanh(x);
}

float sanitizeFinite(float x, float fallback = 0.f) {
	return std::isfinite(x) ? x : fallback;
}

float mixf(float a, float b, float t) {
	return a + (b - a) * t;
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
	return minHz * std::pow(maxHz / minHz, clamp01(x01));
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

float resoToDamping(float resoNorm) {
	const float r = clamp01(resoNorm);
	return 2.f - 1.97f * std::pow(r, 1.18f);
}

int clampCircuitMode(int mode) {
	return clamp(mode, 0, kBifurxCircuitModeCount - 1);
}

float signedWeight(float balance, bool upperPeak) {
	const float sign = upperPeak ? 1.f : -1.f;
	return fastExp(0.82f * sign * clamp(balance, -1.f, 1.f));
}

float cascadeWideMorph(float spanNorm) {
	const float x = clamp01((clamp01(spanNorm) - 0.03f) / 0.97f);
	return std::pow(x, 0.58f);
}

float highHighSpanCompGain(float wideMorph) {
	const float x = clamp01((wideMorph - 0.75f) / 0.25f);
	return 1.f + 0.685f * std::pow(x, 1.1f);
}

struct SvfOutputs {
	float lp = 0.f;
	float bp = 0.f;
	float hp = 0.f;
	float notch = 0.f;
};

float circuitCutoffScale(int circuitMode) {
	switch (clampCircuitMode(circuitMode)) {
		case 1: // DFM
			return 0.95f;
		case 2: // MS2
			return 0.88f;
		case 3: // PRD
			return 1.02f;
		default:
			return 1.f;
	}
}

float circuitQScale(float resoNorm, int circuitMode) {
	switch (clampCircuitMode(circuitMode)) {
		case 1: // DFM
			return 1.28f + 0.95f * resoNorm;
		case 2: // MS2
			return 0.74f + 0.36f * resoNorm;
		case 3: // PRD
			return 1.42f + 1.15f * resoNorm;
		default:
			return 1.f;
	}
}

struct SemanticExportProfile {
	float lpScale = 0.f;
	float bpScale = 0.f;
	float hpScale = 0.f;
};

SemanticExportProfile semanticExportProfile(int circuitMode, int stageIndex) {
	const int clampedCircuitMode = clampCircuitMode(circuitMode);
	const int clampedStageIndex = clamp(stageIndex, 0, 1);
	SemanticExportProfile profile;
	switch (clampedCircuitMode) {
		case 1: // DFM
			if (clampedStageIndex == 0) {
				profile.lpScale = 4.5f;
				profile.bpScale = 1.05f;
				profile.hpScale = 1.05f;
			}
			else {
				profile.lpScale = 2.4f;
				profile.bpScale = 0.90f;
				profile.hpScale = 0.90f;
			}
			return profile;
		case 2: // MS2
			if (clampedStageIndex == 0) {
				profile.lpScale = 2.0f;
				profile.bpScale = 0.92f;
				profile.hpScale = 0.92f;
			}
			else {
				profile.lpScale = 1.1f;
				profile.bpScale = 0.72f;
				profile.hpScale = 0.72f;
			}
			return profile;
		case 3: // PRD
			if (clampedStageIndex == 0) {
				profile.lpScale = 2.5f;
				profile.bpScale = 0.98f;
				profile.hpScale = 0.98f;
			}
			else {
				profile.lpScale = 1.6f;
				profile.bpScale = 0.78f;
				profile.hpScale = 0.78f;
			}
			return profile;
		default:
			return profile;
	}
}

template <typename T>
T normalizeSemanticComponent(const T& value, float exportScale) {
	if (!(exportScale > 0.f)) {
		return value;
	}
	const float magnitude = std::abs(value);
	if (!(magnitude > 0.f) || !std::isfinite(magnitude)) {
		return value;
	}
	const float compressed = exportScale * std::tanh(magnitude / exportScale);
	if (!(compressed > 0.f) || !std::isfinite(compressed)) {
		return value;
	}
	return value * T(compressed / magnitude);
}

SvfOutputs normalizeSemanticOutputs(const SvfOutputs& raw, int circuitMode, int stageIndex) {
	const SemanticExportProfile profile = semanticExportProfile(circuitMode, stageIndex);
	SvfOutputs out;
	out.lp = normalizeSemanticComponent(raw.lp, profile.lpScale);
	out.bp = normalizeSemanticComponent(raw.bp, profile.bpScale);
	out.hp = normalizeSemanticComponent(raw.hp, profile.hpScale);
	out.notch = out.lp + out.hp;
	return out;
}

float modeCircuitSyncCompGain(int mode, int circuitMode, float wideMorph) {
	const int clampedCircuitMode = clampCircuitMode(circuitMode);
	if (clampedCircuitMode == 0) {
		return 1.f;
	}
	switch (mode) {
		case 0: {
			// Low+Low: keep low shelf/body aligned across alternate circuit models.
			static const float kGainByCircuit[kBifurxCircuitModeCount] = {1.f, 0.97f, 1.04f, 0.95f};
			return kGainByCircuit[clampedCircuitMode];
		}
		case 1:
		case 8: {
			// Keep LB/BH mirrored while matching perceived level/slope across models.
			static const float kGainByCircuit[kBifurxCircuitModeCount] = {1.f, 0.94f, 1.07f, 0.91f};
			return kGainByCircuit[clampedCircuitMode];
		}
		case 2:
		case 7: {
			// Keep NL/HN mirrored while matching notch-depth energy across models.
			static const float kGainByCircuit[kBifurxCircuitModeCount] = {1.f, 0.95f, 1.06f, 0.92f};
			return kGainByCircuit[clampedCircuitMode];
		}
		case 9: {
			// High+High: compensate stronger model divergence at wide span settings.
			static const float kBaseByCircuit[kBifurxCircuitModeCount] = {1.f, 0.93f, 1.08f, 0.89f};
			float gain = kBaseByCircuit[clampedCircuitMode];
			const float hiSpan = clamp01((wideMorph - 0.72f) / 0.28f);
			if (clampedCircuitMode == 1 || clampedCircuitMode == 3) {
				gain -= 0.04f * hiSpan;
			}
			else if (clampedCircuitMode == 2) {
				gain += 0.03f * hiSpan;
			}
			return gain;
		}
		default:
			return 1.f;
	}
}

NVGcolor mixColor(const NVGcolor& a, const NVGcolor& b, float t) {
	const float clampedT = clamp01(t);
	NVGcolor out;
	out.r = mixf(a.r, b.r, clampedT);
	out.g = mixf(a.g, b.g, clampedT);
	out.b = mixf(a.b, b.b, clampedT);
	out.a = mixf(a.a, b.a, clampedT);
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

struct SvfCoeffs {
	float g = 0.f;
	float k = 0.f;
	float a1 = 1.f;
};

SvfCoeffs makeSvfCoeffs(float sampleRate, float cutoff, float damping) {
	const float sr = std::max(sampleRate, 1.f);
	const float limitedCutoff = clamp(cutoff, 4.f, 0.46f * sr);
	const float g = std::tan(kPi * limitedCutoff / sr);
	const float k = clamp(damping, 0.02f, 2.2f);
	const float a1 = 1.f / (1.f + g * (g + k));
	SvfCoeffs coeffs;
	coeffs.g = g;
	coeffs.k = k;
	coeffs.a1 = a1;
	return coeffs;
}

struct TptSvf {
	float ic1eq = 0.f;
	float ic2eq = 0.f;

	SvfOutputs processWithCoeffs(float input, const SvfCoeffs& coeffs) {
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

	SvfOutputs process(float input, float sampleRate, float cutoff, float damping) {
		return processWithCoeffs(input, makeSvfCoeffs(sampleRate, cutoff, damping));
	}
};

void sanitizeCoreState(TptSvf& core) {
	if (!std::isfinite(core.ic1eq) || !std::isfinite(core.ic2eq)) {
		core.ic1eq = 0.f;
		core.ic2eq = 0.f;
	}
}

struct DfmCore {
	float s1 = 0.f;
	float s2 = 0.f;

	SvfOutputs process(float input, float sampleRate, float cutoff, float damping, float drive, float resoNorm) {
		const float sr = std::max(sampleRate, 1.f);
		const float limitedCutoff = clamp(cutoff, 4.f, 0.46f * sr);
		const float g = std::tan(kPi * limitedCutoff / sr);
		const float q = 1.f / std::max(damping, 0.05f);
		const float fb = clamp(0.14f + 0.42f * q + 0.24f * resoNorm, 0.f, 1.60f);
		// `input` is already level-shaped upstream; keep a strong unity floor so
		// alternate circuit models do not collapse low-frequency energy.
		const float driveScaled = 0.88f + 0.16f * clamp(drive, 0.f, 10.f);
		const float u = driveScaled * input - fb * s2;
		const float x = std::tanh(u) + 0.08f * std::tanh(2.f * (driveScaled * input - 0.6f * fb * s2));
		s1 += g * (x - s1);
		s2 += g * (s1 - s2);
		SvfOutputs out;
		out.lp = 4.8f * s2;
		out.bp = 5.0f * (s1 - s2);
		out.hp = 4.8f * (x - s1);
		out.notch = out.lp + out.hp;
		return out;
	}
};

void sanitizeCoreState(DfmCore& core) {
	if (!std::isfinite(core.s1) || !std::isfinite(core.s2)) {
		core.s1 = 0.f;
		core.s2 = 0.f;
	}
}

struct Ms2Core {
	float z1 = 0.f;
	float z2 = 0.f;
	float z3 = 0.f;
	float z4 = 0.f;

	SvfOutputs process(float input, float sampleRate, float cutoff, float damping, float drive, float resoNorm) {
		const float sr = std::max(sampleRate, 1.f);
		const float limitedCutoff = clamp(cutoff, 4.f, 0.46f * sr);
		const float g = std::tan(kPi * limitedCutoff / sr);
		const float q = 1.f / std::max(damping, 0.05f);
		const float resonance = clamp(0.14f + 0.18f * q + 0.16f * resoNorm, 0.f, 0.98f);
		// `input` is already level-shaped upstream; keep a strong unity floor so
		// alternate circuit models do not collapse low-frequency energy.
		const float driveScaled = 0.86f + 0.15f * clamp(drive, 0.f, 10.f);
		const float x = std::tanh(0.92f * (driveScaled * input - resonance * z4));
		z1 += g * (std::tanh(x) - z1);
		z2 += g * (std::tanh(z1) - z2);
		z3 += g * (std::tanh(z2) - z3);
		z4 += g * (std::tanh(z3) - z4);
		SvfOutputs out;
		out.lp = 4.8f * z4;
		out.bp = 4.6f * (z2 - z3);
		out.hp = 4.8f * (x - z4);
		out.notch = out.lp + out.hp;
		return out;
	}
};

void sanitizeCoreState(Ms2Core& core) {
	if (!std::isfinite(core.z1) || !std::isfinite(core.z2) || !std::isfinite(core.z3) || !std::isfinite(core.z4)) {
		core.z1 = 0.f;
		core.z2 = 0.f;
		core.z3 = 0.f;
		core.z4 = 0.f;
	}
}

struct PrdCore {
	float z1 = 0.f;
	float z2 = 0.f;
	float z3 = 0.f;
	float z4 = 0.f;

	SvfOutputs process(float input, float sampleRate, float cutoff, float damping, float drive, float resoNorm) {
		const float sr = std::max(sampleRate, 1.f);
		const float limitedCutoff = clamp(cutoff, 4.f, 0.46f * sr);
		const float g = std::tan(kPi * limitedCutoff / sr);
		const float q = 1.f / std::max(damping, 0.05f);
		const float resonance = clamp(0.14f + 0.30f * q + 0.36f * resoNorm, 0.f, 1.38f);
		// `input` is already level-shaped upstream; keep a strong unity floor so
		// alternate circuit models do not collapse low-frequency energy.
		const float driveScaled = 0.90f + 0.18f * clamp(drive, 0.f, 10.f);
		const float u = driveScaled * input - resonance * z4;
		const float x = std::tanh(u) + 0.24f * std::tanh(2.2f * u);
		z1 += g * (std::tanh(x - 0.05f * z1) - z1);
		z2 += g * (std::tanh(z1) - z2);
		z3 += g * (std::tanh(z2) - z3);
		z4 += g * (std::tanh(z3 + 0.12f * z2) - z4);
		SvfOutputs out;
		out.lp = 5.1f * z4;
		out.bp = 5.3f * (z1 - z3);
		out.hp = 5.1f * (x - 0.56f * z1 - z4);
		out.notch = out.lp + out.hp;
		return out;
	}
};

void sanitizeCoreState(PrdCore& core) {
	if (!std::isfinite(core.z1) || !std::isfinite(core.z2) || !std::isfinite(core.z3) || !std::isfinite(core.z4)) {
		core.z1 = 0.f;
		core.z2 = 0.f;
		core.z3 = 0.f;
		core.z4 = 0.f;
	}
}

struct DisplayBiquad {
	float b0 = 0.f;
	float b1 = 0.f;
	float b2 = 0.f;
	float a1 = 0.f;
	float a2 = 0.f;

	std::complex<float> response(float omega) const {
		const std::complex<float> z1 = std::exp(std::complex<float>(0.f, -omega));
		const std::complex<float> z2 = z1 * z1;
		const std::complex<float> numerator = b0 + b1 * z1 + b2 * z2;
		const std::complex<float> denominator = 1.f + a1 * z1 + a2 * z2;
		return numerator / denominator;
	}
};

DisplayBiquad makeDisplayBiquad(float sampleRate, float cutoff, float q, int type) {
	const float sr = std::max(sampleRate, 1.f);
	const float freq = clamp(cutoff, 4.f, 0.46f * sr);
	const float omega = 2.f * kPi * freq / sr;
	const float cosW = std::cos(omega);
	const float sinW = std::sin(omega);
	const float alpha = sinW / (2.f * std::max(q, 0.05f));

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
			b0 = alpha;
			b1 = 0.f;
			b2 = -alpha;
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
	const T& cascadeHpToLp,
	const T& cascadeHpToHp,
	float wA,
	float wB,
	float wideMorph,
	int circuitMode
) {
	const T circuitComp = T(modeCircuitSyncCompGain(mode, circuitMode, wideMorph));
	switch (mode) {
		case 0:
			return circuitComp * cascadeLp;
		case 1: return circuitComp * (T(0.92f) * T(wA) * lpA + T(1.18f) * T(wB) * bpB - T(0.16f) * (bpA + bpB));
		case 2: return circuitComp * (T(1.08f) * T(wB) * lpB - T(0.61f) * T(wA) * bpA);
		case 3: return T(1.03f) * cascadeNotch;
		case 4: return T(0.98f) * T(wA) * lpA + T(0.98f) * T(wB) * hpB - T(0.06f) * (bpA + bpB);
		case 5: return T(1.08f) * (T(wA) * bpA + T(wB) * bpB);
		case 6: return T(1.04f) * cascadeHpToLp;
		case 7: return circuitComp * (T(1.08f) * T(wA) * hpA - T(0.61f) * T(wB) * bpB);
		case 8: return circuitComp * (T(1.18f) * T(wA) * bpA + T(0.92f) * T(wB) * hpB - T(0.16f) * (bpA + bpB));
		case 9: return circuitComp * (T(1.06f * highHighSpanCompGain(wideMorph)) * cascadeHpToHp);
		default: return T(1.f);
	}
}

struct BifurxPreviewState {
	float sampleRate = 44100.f;
	float freqA = 440.f;
	float freqB = 440.f;
	float qA = 1.f;
	float qB = 1.f;
	float balance = 0.f;
	float balanceTarget = 0.f;
	float resoNorm = 0.f;
	float spanParamNorm = 0.5f;
	float spanCvNorm = 0.f;
	float spanAtten = 0.f;
	float spanNorm = 0.5f;
	float spanOct = 0.f;
	float freqParamNorm = 0.5f;
	float voctCv = 0.f;
	int mode = 0;
	int circuitMode = 0;
};

struct BifurxLlTelemetryState {
	bool active = false;
	int circuitMode = 0;
	float excitationRms = 0.f;
	float stageALpRms = 0.f;
	float stageBLpRms = 0.f;
	float outputRms = 0.f;
	float stageBLpOverALpDb = 0.f;
	float outputOverInputDb = 0.f;
};

struct BifurxPreviewModel {
	DisplayBiquad lowA;
	DisplayBiquad bandA;
	DisplayBiquad highA;
	DisplayBiquad notchA;
	DisplayBiquad lowB;
	DisplayBiquad bandB;
	DisplayBiquad highB;
	DisplayBiquad notchB;
	float markerFreqA = 440.f;
	float markerFreqB = 440.f;
	float sampleRate = 44100.f;
	float wA = 1.f;
	float wB = 1.f;
	float wideMorph = 0.f;
	int mode = 0;
	int circuitMode = 0;
};

struct BifurxAnalysisFrame {
	alignas(16) float rawInput[kFftSize];
	alignas(16) float drivenInput[kFftSize];
	alignas(16) float input[kFftSize];
	alignas(16) float output[kFftSize];
};

bool previewStatesDiffer(const BifurxPreviewState& a, const BifurxPreviewState& b) {
	if (a.mode != b.mode) {
		return true;
	}
	if (a.circuitMode != b.circuitMode) {
		return true;
	}
	if (std::fabs(a.sampleRate - b.sampleRate) > 0.5f) {
		return true;
	}
	if (std::fabs(a.balance - b.balance) > 1e-3f) {
		return true;
	}
	if (std::fabs(std::log2(std::max(a.freqA, 1.f) / std::max(b.freqA, 1.f))) > 1e-3f) {
		return true;
	}
	if (std::fabs(std::log2(std::max(a.freqB, 1.f) / std::max(b.freqB, 1.f))) > 1e-3f) {
		return true;
	}
	if (std::fabs(a.qA - b.qA) > 1e-3f) {
		return true;
	}
	if (std::fabs(a.qB - b.qB) > 1e-3f) {
		return true;
	}
	return false;
}

BifurxPreviewModel makePreviewModel(const BifurxPreviewState& state) {
	BifurxPreviewModel model;
	// previewState already carries circuit-adjusted target frequencies/Q from
	// the audio path, so do not apply circuit scaling a second time here.
	const float freqA = clamp(state.freqA, 4.f, 0.46f * std::max(state.sampleRate, 1.f));
	const float freqB = clamp(state.freqB, 4.f, 0.46f * std::max(state.sampleRate, 1.f));
	const float qA = clamp(state.qA, 0.2f, 18.f);
	const float qB = clamp(state.qB, 0.2f, 18.f);
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
	model.mode = state.mode;
	model.circuitMode = clampCircuitMode(state.circuitMode);

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
	const SemanticExportProfile profileA = semanticExportProfile(model.circuitMode, 0);
	const SemanticExportProfile profileB = semanticExportProfile(model.circuitMode, 1);
	const std::complex<float> lpA = normalizeSemanticComponent(model.lowA.response(omega), profileA.lpScale);
	const std::complex<float> bpA = normalizeSemanticComponent(model.bandA.response(omega), profileA.bpScale);
	const std::complex<float> hpA = normalizeSemanticComponent(model.highA.response(omega), profileA.hpScale);
	const std::complex<float> ntA = lpA + hpA;
	const std::complex<float> lpB = normalizeSemanticComponent(model.lowB.response(omega), profileB.lpScale);
	const std::complex<float> bpB = normalizeSemanticComponent(model.bandB.response(omega), profileB.bpScale);
	const std::complex<float> hpB = normalizeSemanticComponent(model.highB.response(omega), profileB.hpScale);
	const std::complex<float> ntB = lpB + hpB;
	const std::complex<float> cascadeLp = lpB * lpA;
	const std::complex<float> cascadeNotch = ntB * ntA;
	const std::complex<float> cascadeHpToLp = lpB * hpA;
	const std::complex<float> cascadeHpToHp = hpB * hpA;
	return combineModeResponse<std::complex<float>>(
		model.mode,
		lpA, bpA, hpA, ntA,
		lpB, bpB, hpB, ntB,
		cascadeLp, cascadeNotch, cascadeHpToLp, cascadeHpToHp,
		model.wA, model.wB, model.wideMorph, model.circuitMode
	);
}

float previewModelResponseDb(const BifurxPreviewModel& model, float hz) {
	const float mag = std::abs(previewModelResponse(model, hz));
	return 20.f * std::log10(std::max(mag, 1e-5f));
}

constexpr float kPreviewProbeLevelKnob = 0.5f;
constexpr float kPreviewProbeImpulseAmplitude = 0.01f;

struct BifurxProbeEngineState {
	TptSvf svfA;
	TptSvf svfB;
	DfmCore dfmA;
	DfmCore dfmB;
	Ms2Core ms2A;
	Ms2Core ms2B;
	PrdCore prdA;
	PrdCore prdB;
};

SvfOutputs processProbeStage(
	BifurxProbeEngineState& state,
	int stageIndex,
	int circuitMode,
	float input,
	float sampleRate,
	float cutoff,
	float damping,
	float drive,
	float resoNorm
) {
	switch (clampCircuitMode(circuitMode)) {
		case 1: {
			DfmCore& core = (stageIndex == 0) ? state.dfmA : state.dfmB;
			return normalizeSemanticOutputs(core.process(input, sampleRate, cutoff, damping, drive, resoNorm), circuitMode, stageIndex);
		}
		case 2: {
			Ms2Core& core = (stageIndex == 0) ? state.ms2A : state.ms2B;
			return normalizeSemanticOutputs(core.process(input, sampleRate, cutoff, damping, drive, resoNorm), circuitMode, stageIndex);
		}
		case 3: {
			PrdCore& core = (stageIndex == 0) ? state.prdA : state.prdB;
			return normalizeSemanticOutputs(core.process(input, sampleRate, cutoff, damping, drive, resoNorm), circuitMode, stageIndex);
		}
		default: {
			TptSvf& core = (stageIndex == 0) ? state.svfA : state.svfB;
			return core.process(input, sampleRate, cutoff, damping);
		}
	}
}

void simulatePreviewProbeImpulseResponse(
	const BifurxPreviewState& state,
	float* inputBuffer,
	float* outputBuffer,
	int sampleCount
) {
	if (!inputBuffer || !outputBuffer || sampleCount <= 0) {
		return;
	}

	BifurxProbeEngineState engine;
	const float sampleRate = std::max(state.sampleRate, 1.f);
	const float freqA = clamp(state.freqA, kFreqMinHz, 0.46f * sampleRate);
	const float freqB = clamp(state.freqB, kFreqMinHz, 0.46f * sampleRate);
	const float dampingA = clamp(1.f / std::max(state.qA, 0.05f), 0.02f, 2.2f);
	const float dampingB = clamp(1.f / std::max(state.qB, 0.05f), 0.02f, 2.2f);
	const float lowW = signedWeight(state.balance, false);
	const float highW = signedWeight(state.balance, true);
	const float norm = 2.f / (lowW + highW);
	const float wA = lowW * norm;
	const float wB = highW * norm;
	const float wideMorph = cascadeWideMorph(state.spanNorm);
	const float drive = levelDriveGain(kPreviewProbeLevelKnob);
	const int mode = clamp(state.mode, 0, kBifurxModeCount - 1);
	const int circuitMode = clampCircuitMode(state.circuitMode);

	for (int i = 0; i < sampleCount; ++i) {
		const float rawIn = (i == 0) ? kPreviewProbeImpulseAmplitude : 0.f;
		const float excitation = 5.f * softClip(0.2f * rawIn * drive);
		const SvfOutputs a = processProbeStage(
			engine, 0, circuitMode, excitation, sampleRate, freqA, dampingA, drive, state.resoNorm
		);

		SvfOutputs b;
		float modeOut = 0.f;
		switch (mode) {
			case 0: {
				b = processProbeStage(engine, 1, circuitMode, a.lp, sampleRate, freqB, dampingB, drive, state.resoNorm);
				modeOut = combineModeResponse<float>(
					mode,
					a.lp, a.bp, a.hp, a.notch,
					b.lp, b.bp, b.hp, b.notch,
					b.lp, 0.f, 0.f, 0.f,
					wA, wB, wideMorph, circuitMode
				);
			} break;
			case 1:
			case 2:
			case 4:
			case 5:
			case 7:
			case 8: {
				b = processProbeStage(engine, 1, circuitMode, excitation, sampleRate, freqB, dampingB, drive, state.resoNorm);
				modeOut = combineModeResponse<float>(
					mode,
					a.lp, a.bp, a.hp, a.notch,
					b.lp, b.bp, b.hp, b.notch,
					0.f, 0.f, 0.f, 0.f,
					wA, wB, wideMorph, circuitMode
				);
			} break;
			case 3: {
				b = processProbeStage(engine, 1, circuitMode, a.notch, sampleRate, freqB, dampingB, drive, state.resoNorm);
				modeOut = combineModeResponse<float>(
					mode,
					a.lp, a.bp, a.hp, a.notch,
					b.lp, b.bp, b.hp, b.notch,
					0.f, b.notch, 0.f, 0.f,
					wA, wB, wideMorph, circuitMode
				);
			} break;
			case 6: {
				b = processProbeStage(engine, 1, circuitMode, a.hp, sampleRate, freqB, dampingB, drive, state.resoNorm);
				modeOut = combineModeResponse<float>(
					mode,
					a.lp, a.bp, a.hp, a.notch,
					b.lp, b.bp, b.hp, b.notch,
					0.f, 0.f, b.lp, 0.f,
					wA, wB, wideMorph, circuitMode
				);
			} break;
			case 9:
			default: {
				b = processProbeStage(engine, 1, circuitMode, a.hp, sampleRate, freqB, dampingB, drive, state.resoNorm);
				modeOut = combineModeResponse<float>(
					mode,
					a.lp, a.bp, a.hp, a.notch,
					b.lp, b.bp, b.hp, b.notch,
					0.f, 0.f, 0.f, b.hp,
					wA, wB, wideMorph, circuitMode
				);
			} break;
		}

		inputBuffer[i] = excitation;
		outputBuffer[i] = sanitizeFinite(5.5f * softClip(sanitizeFinite(modeOut) / 5.5f));
	}
}

struct BifurxSpectrumWidget final : Widget {
	struct CurveDebugRecorder {
		bool active = false;
		std::ofstream file;
		std::string path;
		double startTimeSec = 0.0;
		uint64_t sequence = 0;
	};
	struct PerfDebugRecorder {
		bool active = false;
		std::ofstream file;
		std::string path;
		double startTimeSec = 0.0;
		uint64_t sequence = 0;
		double lastLogTimeSec = -1.0;
		uint64_t lastAudioSampledCount = 0;
		uint64_t lastAudioProcessNs = 0;
		uint64_t lastAudioControlsNs = 0;
		uint64_t lastAudioCoreNs = 0;
		uint64_t lastAudioPreviewNs = 0;
		uint64_t lastAudioAnalysisNs = 0;
		uint64_t lastUiStepCount = 0;
		uint64_t lastUiStepNs = 0;
		uint64_t lastUiDrawCount = 0;
		uint64_t lastUiDrawNs = 0;
		uint64_t lastUiCurveUpdateCount = 0;
		uint64_t lastUiCurveUpdateNs = 0;
		uint64_t lastUiOverlayUpdateCount = 0;
		uint64_t lastUiOverlayUpdateNs = 0;
		uint64_t lastUiDrawSetupCount = 0;
		uint64_t lastUiDrawSetupNs = 0;
		uint64_t lastUiDrawBackgroundCount = 0;
		uint64_t lastUiDrawBackgroundNs = 0;
		uint64_t lastUiDrawExpectedCount = 0;
		uint64_t lastUiDrawExpectedNs = 0;
		uint64_t lastUiDrawOverlayCount = 0;
		uint64_t lastUiDrawOverlayNs = 0;
		uint64_t lastUiDrawCurveCount = 0;
		uint64_t lastUiDrawCurveNs = 0;
		uint64_t lastUiDrawMarkersCount = 0;
		uint64_t lastUiDrawMarkersNs = 0;
	};

	Bifurx* module = nullptr;
	widget::FramebufferWidget* framebuffer = nullptr;
	dsp::RealFFT fft;
	alignas(16) float window[kFftSize];
	alignas(16) float fftInputTime[kFftSize];
	alignas(16) float fftOutputTime[kFftSize];
	alignas(16) float fftInputFreq[2 * kFftSize];
	alignas(16) float fftOutputFreq[2 * kFftSize];
	float curveDb[kCurvePointCount];
	float curveTargetDb[kCurvePointCount];
	float overlayFilterDb[kCurvePointCount];
	float overlayTargetFilterDb[kCurvePointCount];
	float overlayModuleDb[kCurvePointCount];
	float overlayTargetModuleDb[kCurvePointCount];
	float overlayOutputDbfs[kCurvePointCount];
	float overlayTargetOutputDbfs[kCurvePointCount];
	float curveX[kCurvePointCount];
	float curveY[kCurvePointCount];
	float curveHz[kCurvePointCount];
	float curveBinPos[kCurvePointCount];
	float bottomY = 0.f;
	float displayTopDbfs = kDisplayTopDbfsCeiling;
	float displayTopTargetDbfs = kDisplayTopDbfsCeiling;
	bool lastFftScaleDynamic = true;
	double lastCurveDebugLogTimeSec = -1.0;
	float cachedAxisSampleRate = 0.f;
	BifurxPreviewState previewState;
	BifurxLlTelemetryState llTelemetryState;
	bool hasPreview = false;
	bool hasLlTelemetry = false;
	bool hasOverlay = false;
	bool hasCurveTarget = false;
	bool hasOverlayTarget = false;
	uint32_t lastPreviewSeq = 0;
	uint32_t lastLlTelemetrySeq = 0;
	uint32_t lastAnalysisSeq = 0;
	CurveDebugRecorder curveDebugRecorder;
	PerfDebugRecorder perfDebugRecorder;
	uint64_t uiStepCount = 0;
	uint64_t uiStepNs = 0;
	uint64_t uiStepMaxNs = 0;
	uint64_t uiDrawCount = 0;
	uint64_t uiDrawNs = 0;
	uint64_t uiDrawMaxNs = 0;
	uint64_t uiCurveUpdateCount = 0;
	uint64_t uiCurveUpdateNs = 0;
	uint64_t uiOverlayUpdateCount = 0;
	uint64_t uiOverlayUpdateNs = 0;
	uint64_t uiDrawSetupCount = 0;
	uint64_t uiDrawSetupNs = 0;
	uint64_t uiDrawBackgroundCount = 0;
	uint64_t uiDrawBackgroundNs = 0;
	uint64_t uiDrawExpectedCount = 0;
	uint64_t uiDrawExpectedNs = 0;
	uint64_t uiDrawOverlayCount = 0;
	uint64_t uiDrawOverlayNs = 0;
	uint64_t uiDrawCurveCount = 0;
	uint64_t uiDrawCurveNs = 0;
	uint64_t uiDrawMarkersCount = 0;
	uint64_t uiDrawMarkersNs = 0;

	BifurxSpectrumWidget();
	~BifurxSpectrumWidget() override;
	void step() override;
	void syncCurveDebugCaptureState();
	void syncPerfDebugCaptureState();
	void startCurveDebugCapture();
	void stopCurveDebugCapture();
	void startPerfDebugCapture();
	void stopPerfDebugCapture();
	void logCurveDebugSample(
		const BifurxPreviewState& state,
		const BifurxLlTelemetryState& llTelemetry,
		float peakAX,
		float peakAYCurve,
		float peakAYMarker,
		float peakBX,
		float peakBYCurve,
		float peakBYMarker,
		float uiFrameMs,
		uint32_t previewSeq,
		bool previewUpdated,
		uint32_t analysisSeq,
		bool analysisUpdated
	);
	void logPerfDebugSample();
	void updateAxisCache();
	void updateCurveCache();
	void updateOverlayCache(const BifurxAnalysisFrame& frame);

	void draw(const DrawArgs& args) override;
};

struct BananutBlack : app::SvgPort {
	BananutBlack() {
		setSvg(Svg::load(asset::plugin(pluginInstance, "res/BananutBlack.svg")));
	}
};

void drawModeStepCaption(const Widget::DrawArgs& args, const Vec& size, const char* caption) {
	if (!APP || !APP->window || !APP->window->uiFont) {
		return;
	}
	nvgFontSize(args.vg, 8.f);
	nvgFontFaceId(args.vg, APP->window->uiFont->handle);
	nvgFillColor(args.vg, nvgRGBA(225, 232, 240, 244));
	nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
	nvgText(args.vg, 0.5f * size.x, 0.5f * size.y + 0.2f, caption, nullptr);
}

struct BifurxModeLeftButton final : TL1105 {
	void draw(const DrawArgs& args) override {
		TL1105::draw(args);
		drawModeStepCaption(args, box.size, "<");
	}
};

struct BifurxModeRightButton final : TL1105 {
	void draw(const DrawArgs& args) override {
		TL1105::draw(args);
		drawModeStepCaption(args, box.size, ">");
	}
};

struct BifurxModeReadoutWidget final : Widget {
	Module* module = nullptr;

	void draw(const DrawArgs& args) override {
		if (!APP || !APP->window || !APP->window->uiFont) {
			return;
		}

		int mode = 0;
		if (module) {
			mode = clamp(int(std::round(module->params[kBifurxModeParamIndex].getValue())), 0, kBifurxModeCount - 1);
		}

		char label[24];
		std::snprintf(label, sizeof(label), "Mode (%d): %s", mode + 1, kBifurxModeLabels[mode]);
		nvgFontSize(args.vg, std::max(9.5f, box.size.y * 0.72f));
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 255));
		nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		nvgText(args.vg, 0.5f * box.size.x, 0.5f * box.size.y, label, nullptr);
	}
};

} // namespace

struct Bifurx final : Module {
	enum ParamId {
		MODE_PARAM,
		LEVEL_PARAM,
		FREQ_PARAM,
		RESO_PARAM,
		BALANCE_PARAM,
		SPAN_PARAM,
		FM_AMT_PARAM,
		SPAN_CV_ATTEN_PARAM,
		TITO_PARAM,
		MODE_LEFT_PARAM,
		MODE_RIGHT_PARAM,
		FILTER_CIRCUIT_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		IN_INPUT,
		VOCT_INPUT,
		FM_INPUT,
		RESO_CV_INPUT,
		BALANCE_CV_INPUT,
		SPAN_CV_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		FM_AMT_POS_LIGHT,
		FM_AMT_NEG_LIGHT,
		SPAN_CV_ATTEN_POS_LIGHT,
		SPAN_CV_ATTEN_NEG_LIGHT,
		FILTER_CIRCUIT_TL_LIGHT,
		FILTER_CIRCUIT_TR_LIGHT,
		FILTER_CIRCUIT_BR_LIGHT,
		FILTER_CIRCUIT_BL_LIGHT,
		LIGHTS_LEN
	};

	TptSvf coreA;
	TptSvf coreB;
	DfmCore dfmA;
	DfmCore dfmB;
	Ms2Core ms2A;
	Ms2Core ms2B;
	PrdCore prdA;
	PrdCore prdB;
	int filterCircuitMode = 0; // 0: SVF, 1: DFM, 2: MS2, 3: PRD
	int activeCircuitMode = 0;
	dsp::ClockDivider previewPublishDivider;
	dsp::ClockDivider previewPublishSlowDivider;
	dsp::ClockDivider controlUpdateDivider;
	dsp::ClockDivider perfMeasureDivider;
	BifurxPreviewState lastPreviewState;
	bool hasLastPreviewState = false;
	BifurxPreviewState previewStates[2];
	std::atomic<int> previewPublishedIndex{0};
	std::atomic<uint32_t> previewPublishSeq{0};
	BifurxLlTelemetryState llTelemetryStates[2];
	std::atomic<int> llTelemetryPublishedIndex{0};
	std::atomic<uint32_t> llTelemetryPublishSeq{0};
	float previewFreqAFiltered = 440.f;
	float previewFreqBFiltered = 440.f;
	float previewQAFiltered = 1.f;
	float previewQBFiltered = 1.f;
	float previewBalanceFiltered = 0.f;
	bool previewFilterInitialized = false;
	float previewFilterAlpha = 0.f;
	float previewFilterAlphaSlow = 0.f;
	float previewFilterAlphaSampleRate = 0.f;
	float voctCvFiltered = 0.f;
	bool voctCvFilterInitialized = false;
	float voctCvFilterAlpha = 0.f;
	float voctCvFilterSampleRate = 0.f;
	float previewPrevTargetFreqA = 440.f;
	float previewPrevTargetFreqB = 440.f;
	bool previewTargetMotionInitialized = false;
	int previewTargetStillSamples = 0;
	int previewAdaptiveCooldown = 0;
	bool controlFastCacheValid = false;
	float cachedDampingA = 0.7f;
	float cachedDampingB = 0.7f;
	float cachedWA = 1.f;
	float cachedWB = 1.f;
	float cachedFreqA0 = 440.f;
	float cachedFreqB0 = 440.f;
	float cachedBalance = 0.f;
	SvfCoeffs cachedCoeffsA;
	SvfCoeffs cachedCoeffsB;
	float analysisRawInputHistory[kFftSize] = {};
	float analysisDrivenInputHistory[kFftSize] = {};
	float analysisOutputHistory[kFftSize] = {};
	float llTelemetryExcitationSq = 0.f;
	float llTelemetryStageALpSq = 0.f;
	float llTelemetryStageBLpSq = 0.f;
	float llTelemetryOutputSq = 0.f;
	int analysisWritePos = 0;
	int analysisFilled = 0;
	int analysisHopCounter = 0;
	bool analysisPublishedOnce = false;
	dsp::SchmittTrigger modeLeftTrigger;
	dsp::SchmittTrigger modeRightTrigger;
	dsp::SchmittTrigger filterCircuitTrigger;
	BifurxAnalysisFrame analysisFrames[2];
	std::atomic<int> analysisPublishedIndex{0};
	std::atomic<uint32_t> analysisPublishSeq{0};
	bool fftScaleDynamic = true;
	bool curveDebugLogging = false;
	bool perfDebugLogging = false;
	std::atomic<uint64_t> perfAudioSampledCount{0};
	std::atomic<uint64_t> perfAudioProcessNs{0};
	std::atomic<uint64_t> perfAudioControlsNs{0};
	std::atomic<uint64_t> perfAudioCoreNs{0};
	std::atomic<uint64_t> perfAudioPreviewNs{0};
	std::atomic<uint64_t> perfAudioAnalysisNs{0};
	std::atomic<uint64_t> perfAudioProcessMaxNs{0};

	Bifurx() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

			configSwitch(MODE_PARAM, 0.f, 9.f, 0.f,
				"Mode",
				{kBifurxModeLabels[0], kBifurxModeLabels[1], kBifurxModeLabels[2], kBifurxModeLabels[3], kBifurxModeLabels[4],
					kBifurxModeLabels[5], kBifurxModeLabels[6], kBifurxModeLabels[7], kBifurxModeLabels[8], kBifurxModeLabels[9]});
		configParam(LEVEL_PARAM, 0.f, 1.f, 0.5f, "Level");
		configParam(FREQ_PARAM, 0.f, 1.f, 0.5f, "Frequency");
			configParam(RESO_PARAM, 0.f, 1.f, 0.35f, "Resonance");
			configParam(BALANCE_PARAM, -1.f, 1.f, 0.f, "Balance");
			configParam(SPAN_PARAM, 0.f, 1.f, 0.5f, "Span");
			configParam(FM_AMT_PARAM, -1.f, 1.f, 0.f, "FM amount");
			configParam(SPAN_CV_ATTEN_PARAM, -1.f, 1.f, 0.f, "Span CV attenuator");
			configSwitch(TITO_PARAM, 0.f, 2.f, 1.f, "TITO", {"XM", "Clean", "SM"});
			configButton(MODE_LEFT_PARAM, "Mode previous");
			configButton(MODE_RIGHT_PARAM, "Mode next");
			configButton(FILTER_CIRCUIT_PARAM, "Filter circuit next");

		configInput(IN_INPUT, "Signal In");
		configInput(VOCT_INPUT, "V/Oct");
		configInput(FM_INPUT, "FM");
		configInput(RESO_CV_INPUT, "Resonance CV");
		configInput(BALANCE_CV_INPUT, "Balance CV");
		configInput(SPAN_CV_INPUT, "Span CV");
		configOutput(OUT_OUTPUT, "Signal Out");
		configBypass(IN_INPUT, OUT_OUTPUT);

		paramQuantities[MODE_PARAM]->snapEnabled = true;
		paramQuantities[TITO_PARAM]->snapEnabled = true;
		previewPublishDivider.setDivision(kPreviewPublishFastDivision);
		previewPublishSlowDivider.setDivision(kPreviewPublishSlowDivision);
		controlUpdateDivider.setDivision(16);
		perfMeasureDivider.setDivision(64);
	}

	void resetCircuitStates() {
		coreA.ic1eq = 0.f;
		coreA.ic2eq = 0.f;
		coreB.ic1eq = 0.f;
		coreB.ic2eq = 0.f;
		dfmA.s1 = 0.f;
		dfmA.s2 = 0.f;
		dfmB.s1 = 0.f;
		dfmB.s2 = 0.f;
		ms2A.z1 = 0.f;
		ms2A.z2 = 0.f;
		ms2A.z3 = 0.f;
		ms2A.z4 = 0.f;
		ms2B.z1 = 0.f;
		ms2B.z2 = 0.f;
		ms2B.z3 = 0.f;
		ms2B.z4 = 0.f;
		prdA.z1 = 0.f;
		prdA.z2 = 0.f;
		prdA.z3 = 0.f;
		prdA.z4 = 0.f;
		prdB.z1 = 0.f;
		prdB.z2 = 0.f;
		prdB.z3 = 0.f;
		prdB.z4 = 0.f;
		llTelemetryExcitationSq = 0.f;
		llTelemetryStageALpSq = 0.f;
		llTelemetryStageBLpSq = 0.f;
		llTelemetryOutputSq = 0.f;
		voctCvFiltered = 0.f;
		voctCvFilterInitialized = false;
	}

	void setFilterCircuitMode(int newMode) {
		const int clampedMode = clampCircuitMode(newMode);
		const bool changed = (filterCircuitMode != clampedMode) || (activeCircuitMode != clampedMode);
		filterCircuitMode = clampedMode;
		activeCircuitMode = clampedMode;
		if (changed) {
			resetCircuitStates();
		}
	}

	json_t* dataToJson() override {
		json_t* root = Module::dataToJson();
		json_object_set_new(root, "fftScaleDynamic", json_boolean(fftScaleDynamic));
		json_object_set_new(root, "curveDebugLogging", json_boolean(curveDebugLogging));
		json_object_set_new(root, "perfDebugLogging", json_boolean(perfDebugLogging));
		json_object_set_new(root, "filterCircuitMode", json_integer(clampCircuitMode(filterCircuitMode)));
		return root;
	}

	void dataFromJson(json_t* root) override {
		Module::dataFromJson(root);
		json_t* fftScaleDynamicJ = json_object_get(root, "fftScaleDynamic");
		if (fftScaleDynamicJ) {
			fftScaleDynamic = json_is_true(fftScaleDynamicJ);
		}
		json_t* curveDebugLoggingJ = json_object_get(root, "curveDebugLogging");
		if (curveDebugLoggingJ) {
			curveDebugLogging = json_is_true(curveDebugLoggingJ);
		}
		json_t* perfDebugLoggingJ = json_object_get(root, "perfDebugLogging");
		if (perfDebugLoggingJ) {
			perfDebugLogging = json_is_true(perfDebugLoggingJ);
		}
		json_t* filterCircuitModeJ = json_object_get(root, "filterCircuitMode");
		if (filterCircuitModeJ) {
			setFilterCircuitMode(int(json_integer_value(filterCircuitModeJ)));
		}
		else {
			setFilterCircuitMode(filterCircuitMode);
		}
	}

	void resetPerfStats() {
		perfAudioSampledCount.store(0, std::memory_order_release);
		perfAudioProcessNs.store(0, std::memory_order_release);
		perfAudioControlsNs.store(0, std::memory_order_release);
		perfAudioCoreNs.store(0, std::memory_order_release);
		perfAudioPreviewNs.store(0, std::memory_order_release);
		perfAudioAnalysisNs.store(0, std::memory_order_release);
		perfAudioProcessMaxNs.store(0, std::memory_order_release);
	}

	void publishPreviewState(const BifurxPreviewState& state) {
		int writeIndex = 1 - previewPublishedIndex.load(std::memory_order_relaxed);
		previewStates[writeIndex] = state;
		previewPublishedIndex.store(writeIndex, std::memory_order_release);
		previewPublishSeq.fetch_add(1, std::memory_order_release);
		lastPreviewState = state;
		hasLastPreviewState = true;
	}

	void publishLlTelemetryState(const BifurxLlTelemetryState& state) {
		const int writeIndex = 1 - llTelemetryPublishedIndex.load(std::memory_order_relaxed);
		llTelemetryStates[writeIndex] = state;
		llTelemetryPublishedIndex.store(writeIndex, std::memory_order_release);
		llTelemetryPublishSeq.fetch_add(1, std::memory_order_release);
	}

	void publishAnalysisFrame() {
		const int writeIndex = 1 - analysisPublishedIndex.load(std::memory_order_relaxed);
		const int start = analysisWritePos;
		const int firstCount = kFftSize - start;
		const int secondCount = start;

			std::memcpy(
				analysisFrames[writeIndex].rawInput,
				analysisRawInputHistory + start,
				size_t(firstCount) * sizeof(float)
			);
			std::memcpy(
				analysisFrames[writeIndex].rawInput + firstCount,
				analysisRawInputHistory,
				size_t(secondCount) * sizeof(float)
			);
			std::memcpy(
				analysisFrames[writeIndex].drivenInput,
				analysisDrivenInputHistory + start,
				size_t(firstCount) * sizeof(float)
			);
			std::memcpy(
				analysisFrames[writeIndex].drivenInput + firstCount,
				analysisDrivenInputHistory,
				size_t(secondCount) * sizeof(float)
			);
			std::memcpy(
				analysisFrames[writeIndex].input,
				analysisDrivenInputHistory + start,
				size_t(firstCount) * sizeof(float)
			);
			std::memcpy(
				analysisFrames[writeIndex].input + firstCount,
				analysisDrivenInputHistory,
				size_t(secondCount) * sizeof(float)
			);
		std::memcpy(
			analysisFrames[writeIndex].output,
			analysisOutputHistory + start,
			size_t(firstCount) * sizeof(float)
		);
		std::memcpy(
			analysisFrames[writeIndex].output + firstCount,
			analysisOutputHistory,
			size_t(secondCount) * sizeof(float)
		);

		analysisPublishedIndex.store(writeIndex, std::memory_order_release);
		analysisPublishSeq.fetch_add(1, std::memory_order_release);
	}

		void pushAnalysisSample(float rawInputSample, float drivenInputSample, float outputSample) {
			analysisRawInputHistory[analysisWritePos] = sanitizeFinite(rawInputSample);
			analysisDrivenInputHistory[analysisWritePos] = sanitizeFinite(drivenInputSample);
			analysisOutputHistory[analysisWritePos] = sanitizeFinite(outputSample);
			analysisWritePos = (analysisWritePos + 1) % kFftSize;
		if (analysisFilled < kFftSize) {
			analysisFilled++;
		}
		if (analysisFilled == kFftSize) {
			analysisHopCounter++;
			if (!analysisPublishedOnce || analysisHopCounter >= kFftHopSize) {
				analysisHopCounter = 0;
				publishAnalysisFrame();
				analysisPublishedOnce = true;
			}
		}
	}

	void process(const ProcessArgs& args) override {
			using PerfClock = std::chrono::steady_clock;
			const bool measurePerf = perfDebugLogging && perfMeasureDivider.process();
			const PerfClock::time_point perfStart = measurePerf ? PerfClock::now() : PerfClock::time_point();
			PerfClock::time_point perfCoreStart;
			PerfClock::time_point perfPreviewStart;
			PerfClock::time_point perfAnalysisStart;

					if (kBifurxTuneSvfOnly) {
						activeCircuitMode = 0;
					}
				int effectiveCircuitMode = kBifurxTuneSvfOnly ? 0 : activeCircuitMode;
			sanitizeCoreState(coreA);
			sanitizeCoreState(coreB);
			sanitizeCoreState(dfmA);
			sanitizeCoreState(dfmB);
			sanitizeCoreState(ms2A);
			sanitizeCoreState(ms2B);
			sanitizeCoreState(prdA);
			sanitizeCoreState(prdB);

			if (modeLeftTrigger.process(params[MODE_LEFT_PARAM].getValue())) {
				const int currentMode = clamp(int(std::round(params[MODE_PARAM].getValue())), 0, 9);
				params[MODE_PARAM].setValue(float((currentMode + 9) % 10));
			}
			if (modeRightTrigger.process(params[MODE_RIGHT_PARAM].getValue())) {
				const int currentMode = clamp(int(std::round(params[MODE_PARAM].getValue())), 0, 9);
				params[MODE_PARAM].setValue(float((currentMode + 1) % 10));
			}
				if (filterCircuitTrigger.process(params[FILTER_CIRCUIT_PARAM].getValue())) {
					setFilterCircuitMode((clampCircuitMode(filterCircuitMode) + 1) % kBifurxCircuitModeCount);
				}

		const float in = sanitizeFinite(inputs[IN_INPUT].getVoltage());
		const float level = params[LEVEL_PARAM].getValue();
		const float drive = levelDriveGain(level);
		const int mode = int(std::round(params[MODE_PARAM].getValue()));
		const int tito = int(std::round(params[TITO_PARAM].getValue()));
		const float freqParamNorm = clamp(params[FREQ_PARAM].getValue(), 0.f, 1.f);
		const bool voctConnected = inputs[VOCT_INPUT].isConnected();
		const float voctCvRaw = voctConnected ? clamp(inputs[VOCT_INPUT].getVoltage(), -10.f, 10.f) : 0.f;
		if (std::fabs(voctCvFilterSampleRate - args.sampleRate) > 0.5f) {
			voctCvFilterAlpha = onePoleAlpha(1.f / std::max(args.sampleRate, 1.f), kVoctSmoothingTauSeconds);
			voctCvFilterSampleRate = args.sampleRate;
		}
		float voctCv = 0.f;
		if (voctConnected) {
			if (!voctCvFilterInitialized) {
				voctCvFiltered = voctCvRaw;
				voctCvFilterInitialized = true;
			}
			else {
				voctCvFiltered += voctCvFilterAlpha * (voctCvRaw - voctCvFiltered);
			}
			// Prevent tiny DC/noise wobble from a connected 0V source from
			// audibly ratcheting cutoff while turning the frequency knob.
			voctCv = (std::fabs(voctCvFiltered) < kVoctDeadbandVolts) ? 0.f : voctCvFiltered;
		}
		else {
			voctCvFiltered = 0.f;
			voctCvFilterInitialized = false;
		}
		const float fmAmt = clamp(params[FM_AMT_PARAM].getValue(), -1.f, 1.f);
		const float fmCv = inputs[FM_INPUT].isConnected() ? clamp(inputs[FM_INPUT].getVoltage(), -10.f, 10.f) : 0.f;
		const float fm = fmCv * fmAmt;
		const float resoCvNorm = clamp(inputs[RESO_CV_INPUT].getVoltage(), 0.f, 8.f) / 8.f;
		const float resoNorm = clamp(params[RESO_PARAM].getValue() + resoCvNorm, 0.f, 1.f);
		const float balanceCvNorm = clamp(inputs[BALANCE_CV_INPUT].getVoltage(), -5.f, 5.f) / 5.f;
		const float balanceNorm = clamp(params[BALANCE_PARAM].getValue() + balanceCvNorm, -1.f, 1.f);
		const float spanParamNorm = clamp(params[SPAN_PARAM].getValue(), 0.f, 1.f);
		const float spanAtten = clamp(params[SPAN_CV_ATTEN_PARAM].getValue(), -1.f, 1.f);
		const float spanCvNorm = clamp(inputs[SPAN_CV_INPUT].getVoltage(), -10.f, 10.f) / 5.f;
		const float spanNorm = clamp(spanParamNorm + 0.5f * spanAtten * spanCvNorm, 0.f, 1.f);
		const float spanOct = 8.f * shapedSpan(spanNorm);
		const float spanWideMorph = cascadeWideMorph(spanNorm);
		const bool fastPathEligible = (effectiveCircuitMode == 0)
			&& (tito == 1)
			&& !inputs[VOCT_INPUT].isConnected()
			&& !inputs[FM_INPUT].isConnected()
			&& !inputs[RESO_CV_INPUT].isConnected()
			&& !inputs[BALANCE_CV_INPUT].isConnected()
			&& !inputs[SPAN_CV_INPUT].isConnected();
		const bool updateFastControls = !controlFastCacheValid || !fastPathEligible || controlUpdateDivider.process();
		if (std::fabs(previewFilterAlphaSampleRate - args.sampleRate) > 0.5f) {
			previewFilterAlpha = onePoleAlpha(1.f / std::max(args.sampleRate, 1.f), 0.05f);
			previewFilterAlphaSlow = onePoleAlpha(1.f / std::max(args.sampleRate, 1.f), 0.20f);
			previewFilterAlphaSampleRate = args.sampleRate;
		}

		float freqA0 = cachedFreqA0;
		float freqB0 = cachedFreqB0;
		float dampingA = cachedDampingA;
		float dampingB = cachedDampingB;
		float wA = cachedWA;
		float wB = cachedWB;
		float balance = cachedBalance;

			if (updateFastControls) {
				balance = balanceNorm;
				const float centerHz = kFreqMinHz * fastExp2(kFreqLog2Span * freqParamNorm) * fastExp2(voctCv + fm);
				const float sr = std::max(args.sampleRate, 1.f);
					auto computeCircuitFreqs = [&](int circuitMode, float *freqAOut, float *freqBOut) {
						const float cutoffScale = circuitCutoffScale(circuitMode);
						// Keep span symmetric around center by limiting octave shift before
						// cutoff clamping in the core.
						const float safeCenterHz = clamp(centerHz * cutoffScale, kFreqMinHz, 0.46f * sr);
						const float maxShiftUp = std::max(0.f, std::log2((0.46f * sr) / safeCenterHz));
						const float maxShiftDown = std::max(0.f, std::log2(safeCenterHz / kFreqMinHz));
						const float maxSymShift = std::min(maxShiftUp, maxShiftDown);
						const float halfSpanOct = std::min(0.5f * spanOct, maxSymShift);
						float baseA = safeCenterHz * fastExp2(-halfSpanOct);
						float baseB = safeCenterHz * fastExp2(halfSpanOct);
						if (freqAOut) {
							*freqAOut = clamp(baseA, kFreqMinHz, 0.46f * sr);
						}
						if (freqBOut) {
							*freqBOut = clamp(baseB, kFreqMinHz, 0.46f * sr);
						}
					};
					const int safeEffectiveMode = clampCircuitMode(effectiveCircuitMode);
					const float qScale = circuitQScale(resoNorm, safeEffectiveMode);
					computeCircuitFreqs(safeEffectiveMode, &freqA0, &freqB0);
					const float baseDamping = resoToDamping(resoNorm) / std::max(qScale, 1e-4f);
					dampingA = clamp(baseDamping * fastExp(0.48f * balance), 0.02f, 2.2f);
					dampingB = clamp(baseDamping * fastExp(-0.48f * balance), 0.02f, 2.2f);
			const float lowW = signedWeight(balance, false);
			const float highW = signedWeight(balance, true);
			const float norm = 2.f / (lowW + highW);
			wA = lowW * norm;
			wB = highW * norm;

			cachedDampingA = dampingA;
			cachedDampingB = dampingB;
			cachedWA = wA;
			cachedWB = wB;
			cachedFreqA0 = freqA0;
			cachedFreqB0 = freqB0;
			cachedBalance = balance;
			cachedCoeffsA = makeSvfCoeffs(args.sampleRate, freqA0, dampingA);
			cachedCoeffsB = makeSvfCoeffs(args.sampleRate, freqB0, dampingB);
			controlFastCacheValid = true;
		}

		const float couplingDepth = (0.018f + 0.20f * resoNorm * resoNorm) * (tito == 1 ? 0.f : 1.f);
		const float drivenIn = 5.f * softClip(0.2f * in * drive);
		const float excitation = drivenIn + (resoNorm > 0.985f ? 1e-6f : 0.f);

		auto modulatedCutoffs = [&](float modA, float modB) {
			const float cutA = freqA0 * fastExp2(clamp(modA, -2.5f, 2.5f));
			const float cutB = freqB0 * fastExp2(clamp(modB, -2.5f, 2.5f));
			return std::pair<float, float>(cutA, cutB);
		};

			float cutoffA = freqA0;
			float cutoffB = freqB0;
			float modA = 0.f;
			float modB = 0.f;
			auto circuitModStateA = [&]() {
				switch (effectiveCircuitMode) {
					case 1: return dfmA.s1;
					case 2: return ms2A.z2;
					case 3: return prdA.z2;
					default: return coreA.ic1eq;
				}
			};
			auto circuitModStateB = [&]() {
				switch (effectiveCircuitMode) {
					case 1: return dfmB.s1;
					case 2: return ms2B.z2;
					case 3: return prdB.z2;
					default: return coreB.ic1eq;
				}
			};
			if (!fastPathEligible) {
				if (tito == 2) {
					modA = couplingDepth * circuitModStateA() / 5.f;
					modB = couplingDepth * circuitModStateB() / 5.f;
				}
				else if (tito == 0) {
					modA = couplingDepth * circuitModStateB() / 5.f;
					modB = couplingDepth * circuitModStateA() / 5.f;
				}

			const std::pair<float, float> cutoffs = modulatedCutoffs(modA, modB);
			cutoffA = cutoffs.first;
			cutoffB = cutoffs.second;
		}
		if (measurePerf) {
			perfCoreStart = PerfClock::now();
		}
			float modeOut = 0.f;
			float llExcitationSample = 0.f;
			float llStageALpSample = 0.f;
			float llStageBLpSample = 0.f;

			auto processA = [&](float sample) -> SvfOutputs {
				SvfOutputs raw;
				switch (effectiveCircuitMode) {
					case 1:
						raw = dfmA.process(sample, args.sampleRate, cutoffA, dampingA, drive, resoNorm);
						break;
					case 2:
						raw = ms2A.process(sample, args.sampleRate, cutoffA, dampingA, drive, resoNorm);
						break;
					case 3:
						raw = prdA.process(sample, args.sampleRate, cutoffA, dampingA, drive, resoNorm);
						break;
					default:
						break;
				}
				if (effectiveCircuitMode == 0) {
					if (fastPathEligible) {
						raw = coreA.processWithCoeffs(sample, cachedCoeffsA);
					}
					else {
						raw = coreA.process(sample, args.sampleRate, cutoffA, dampingA);
					}
				}
				return normalizeSemanticOutputs(raw, effectiveCircuitMode, 0);
			};
			auto processB = [&](float sample) -> SvfOutputs {
				SvfOutputs raw;
				switch (effectiveCircuitMode) {
					case 1:
						raw = dfmB.process(sample, args.sampleRate, cutoffB, dampingB, drive, resoNorm);
						break;
					case 2:
						raw = ms2B.process(sample, args.sampleRate, cutoffB, dampingB, drive, resoNorm);
						break;
					case 3:
						raw = prdB.process(sample, args.sampleRate, cutoffB, dampingB, drive, resoNorm);
						break;
					default:
						break;
				}
				if (effectiveCircuitMode == 0) {
					if (fastPathEligible) {
						raw = coreB.processWithCoeffs(sample, cachedCoeffsB);
					}
					else {
						raw = coreB.process(sample, args.sampleRate, cutoffB, dampingB);
					}
				}
				return normalizeSemanticOutputs(raw, effectiveCircuitMode, 1);
			};

		switch (mode) {
			case 0: {
				const SvfOutputs a = processA(excitation);
				const SvfOutputs b = processB(a.lp);
				llExcitationSample = excitation;
				llStageALpSample = a.lp;
				llStageBLpSample = b.lp;
				modeOut = combineModeResponse<float>(
					mode,
					a.lp, a.bp, a.hp, a.notch,
					b.lp, b.bp, b.hp, b.notch,
					b.lp, 0.f, 0.f, 0.f,
						wA, wB, spanWideMorph, effectiveCircuitMode
					);
			} break;
			case 1: {
				const SvfOutputs a = processA(excitation);
				const SvfOutputs b = processB(excitation);
				modeOut = combineModeResponse<float>(
					mode,
					a.lp, a.bp, a.hp, a.notch,
					b.lp, b.bp, b.hp, b.notch,
					0.f, 0.f, 0.f, 0.f,
						wA, wB, spanWideMorph, effectiveCircuitMode
					);
			} break;
			case 2: {
				const SvfOutputs a = processA(excitation);
				const SvfOutputs b = processB(excitation);
				modeOut = combineModeResponse<float>(
					mode,
					a.lp, a.bp, a.hp, a.notch,
					b.lp, b.bp, b.hp, b.notch,
					0.f, 0.f, 0.f, 0.f,
						wA, wB, spanWideMorph, effectiveCircuitMode
					);
			} break;
			case 3: {
				const SvfOutputs a = processA(excitation);
				const SvfOutputs b = processB(a.notch);
				modeOut = combineModeResponse<float>(
					mode,
					a.lp, a.bp, a.hp, a.notch,
					b.lp, b.bp, b.hp, b.notch,
					0.f, b.notch, 0.f, 0.f,
						wA, wB, spanWideMorph, effectiveCircuitMode
					);
			} break;
			case 4: {
				const SvfOutputs a = processA(excitation);
				const SvfOutputs b = processB(excitation);
				modeOut = combineModeResponse<float>(
					mode,
					a.lp, a.bp, a.hp, a.notch,
					b.lp, b.bp, b.hp, b.notch,
					0.f, 0.f, 0.f, 0.f,
						wA, wB, spanWideMorph, effectiveCircuitMode
					);
			} break;
			case 5: {
				const SvfOutputs a = processA(excitation);
				const SvfOutputs b = processB(excitation);
				modeOut = combineModeResponse<float>(
					mode,
					a.lp, a.bp, a.hp, a.notch,
					b.lp, b.bp, b.hp, b.notch,
					0.f, 0.f, 0.f, 0.f,
						wA, wB, spanWideMorph, effectiveCircuitMode
					);
			} break;
			case 6: {
				const SvfOutputs a = processA(excitation);
				const SvfOutputs b = processB(a.hp);
				modeOut = combineModeResponse<float>(
					mode,
					a.lp, a.bp, a.hp, a.notch,
					b.lp, b.bp, b.hp, b.notch,
					0.f, 0.f, b.lp, 0.f,
						wA, wB, spanWideMorph, effectiveCircuitMode
					);
			} break;
			case 7: {
				const SvfOutputs a = processA(excitation);
				const SvfOutputs b = processB(excitation);
				modeOut = combineModeResponse<float>(
					mode,
					a.lp, a.bp, a.hp, a.notch,
					b.lp, b.bp, b.hp, b.notch,
					0.f, 0.f, 0.f, 0.f,
						wA, wB, spanWideMorph, effectiveCircuitMode
					);
			} break;
			case 8: {
				const SvfOutputs a = processA(excitation);
				const SvfOutputs b = processB(excitation);
				modeOut = combineModeResponse<float>(
					mode,
					a.lp, a.bp, a.hp, a.notch,
					b.lp, b.bp, b.hp, b.notch,
					0.f, 0.f, 0.f, 0.f,
						wA, wB, spanWideMorph, effectiveCircuitMode
					);
			} break;
			case 9:
			default: {
				const SvfOutputs a = processA(excitation);
				const SvfOutputs b = processB(a.hp);
				modeOut = combineModeResponse<float>(
					mode,
					a.lp, a.bp, a.hp, a.notch,
					b.lp, b.bp, b.hp, b.notch,
					0.f, 0.f, 0.f, b.hp,
						wA, wB, spanWideMorph, effectiveCircuitMode
					);
			} break;
		}

		const float safeModeOut = sanitizeFinite(modeOut);
		const float out = sanitizeFinite(5.5f * softClip(safeModeOut / 5.5f));
		outputs[OUT_OUTPUT].setChannels(1);
		outputs[OUT_OUTPUT].setVoltage(out);
		const float llTelemetryAlpha = onePoleAlpha(args.sampleTime, kLlTelemetryTauSeconds);
		if (mode == 0) {
			llTelemetryExcitationSq += llTelemetryAlpha * (llExcitationSample * llExcitationSample - llTelemetryExcitationSq);
			llTelemetryStageALpSq += llTelemetryAlpha * (llStageALpSample * llStageALpSample - llTelemetryStageALpSq);
			llTelemetryStageBLpSq += llTelemetryAlpha * (llStageBLpSample * llStageBLpSample - llTelemetryStageBLpSq);
			llTelemetryOutputSq += llTelemetryAlpha * (out * out - llTelemetryOutputSq);
		}
		else {
			llTelemetryExcitationSq += llTelemetryAlpha * (0.f - llTelemetryExcitationSq);
			llTelemetryStageALpSq += llTelemetryAlpha * (0.f - llTelemetryStageALpSq);
			llTelemetryStageBLpSq += llTelemetryAlpha * (0.f - llTelemetryStageBLpSq);
			llTelemetryOutputSq += llTelemetryAlpha * (0.f - llTelemetryOutputSq);
		}
		if (measurePerf) {
			perfPreviewStart = PerfClock::now();
		}

		const float previewTargetFreqA = clamp(freqA0, 4.f, 0.46f * args.sampleRate);
		const float previewTargetFreqB = clamp(freqB0, 4.f, 0.46f * args.sampleRate);
		const float previewTargetQA = 1.f / std::max(dampingA, 0.05f);
		const float previewTargetQB = 1.f / std::max(dampingB, 0.05f);
		const float previewTargetBalance = balance;
		const bool previewPitchCvConnected = inputs[VOCT_INPUT].isConnected() || inputs[FM_INPUT].isConnected();
		const float previewSmoothingAlpha = previewPitchCvConnected ? previewFilterAlphaSlow : previewFilterAlpha;
		if (!previewTargetMotionInitialized) {
			previewPrevTargetFreqA = previewTargetFreqA;
			previewPrevTargetFreqB = previewTargetFreqB;
			previewTargetStillSamples = 0;
			previewTargetMotionInitialized = true;
		}
		const float targetMotionAOct =
			std::fabs(std::log2(std::max(previewTargetFreqA, 1.f) / std::max(previewPrevTargetFreqA, 1.f)));
		const float targetMotionBOct =
			std::fabs(std::log2(std::max(previewTargetFreqB, 1.f) / std::max(previewPrevTargetFreqB, 1.f)));
		const float targetMotionOct = std::max(targetMotionAOct, targetMotionBOct);
		if (targetMotionOct <= kPreviewInstantSettleMotionOctThreshold) {
			previewTargetStillSamples++;
		}
		else {
			previewTargetStillSamples = 0;
		}
		const bool previewInstantSettleNow = (previewTargetStillSamples >= kPreviewInstantSettleHoldSamples);
		previewPrevTargetFreqA = previewTargetFreqA;
		previewPrevTargetFreqB = previewTargetFreqB;

		if (!previewFilterInitialized) {
			previewFreqAFiltered = previewTargetFreqA;
			previewFreqBFiltered = previewTargetFreqB;
			previewQAFiltered = previewTargetQA;
			previewQBFiltered = previewTargetQB;
			previewBalanceFiltered = previewTargetBalance;
			previewFilterInitialized = true;
		}
		else if (previewInstantSettleNow) {
			// Once control motion is effectively still, snap preview to current
			// target so the notch settles immediately at the held position.
			previewFreqAFiltered = previewTargetFreqA;
			previewFreqBFiltered = previewTargetFreqB;
			previewQAFiltered = previewTargetQA;
			previewQBFiltered = previewTargetQB;
			previewBalanceFiltered = previewTargetBalance;
		}
		else {
			// Keep the preview stable by tracking nominal control state, not
			// instantaneous audio-rate modulation/coupling in the DSP core.
				const float a = previewSmoothingAlpha;
				previewFreqAFiltered += a * (previewTargetFreqA - previewFreqAFiltered);
				previewFreqBFiltered += a * (previewTargetFreqB - previewFreqBFiltered);
				previewQAFiltered += a * (previewTargetQA - previewQAFiltered);
			previewQBFiltered += a * (previewTargetQB - previewQBFiltered);
			previewBalanceFiltered += a * (previewTargetBalance - previewBalanceFiltered);
		}

		BifurxPreviewState previewState;
		previewState.sampleRate = args.sampleRate;
		previewState.freqA = previewFreqAFiltered;
		previewState.freqB = previewFreqBFiltered;
		previewState.qA = previewQAFiltered;
		previewState.qB = previewQBFiltered;
			previewState.mode = mode;
			previewState.circuitMode = effectiveCircuitMode;
			previewState.balance = previewBalanceFiltered;
		previewState.balanceTarget = balanceNorm;
		previewState.resoNorm = resoNorm;
		previewState.spanParamNorm = spanParamNorm;
		previewState.spanCvNorm = spanCvNorm;
		previewState.spanAtten = spanAtten;
		previewState.spanNorm = spanNorm;
		previewState.spanOct = spanOct;
		previewState.freqParamNorm = freqParamNorm;
		previewState.voctCv = voctCv;
		if (previewAdaptiveCooldown > 0) {
			previewAdaptiveCooldown--;
		}

		bool adaptivePreviewTick = false;
		if (hasLastPreviewState && previewAdaptiveCooldown <= 0) {
			const float freqMoveAOct =
				std::fabs(std::log2(std::max(previewState.freqA, 1.f) / std::max(lastPreviewState.freqA, 1.f)));
			const float freqMoveBOct =
				std::fabs(std::log2(std::max(previewState.freqB, 1.f) / std::max(lastPreviewState.freqB, 1.f)));
			const float spanMoveOct = std::fabs(previewState.spanOct - lastPreviewState.spanOct);
			const float qMoveA = std::fabs(previewState.qA - lastPreviewState.qA);
			const float qMoveB = std::fabs(previewState.qB - lastPreviewState.qB);
			const float balanceMove = std::fabs(previewState.balance - lastPreviewState.balance);
			const bool rapidPreviewMove =
				(freqMoveAOct > kPreviewAdaptiveOctaveThreshold) ||
				(freqMoveBOct > kPreviewAdaptiveOctaveThreshold) ||
				(spanMoveOct > kPreviewAdaptiveSpanOctThreshold) ||
				(qMoveA > kPreviewAdaptiveQThreshold) ||
				(qMoveB > kPreviewAdaptiveQThreshold) ||
				(balanceMove > kPreviewAdaptiveBalanceThreshold);
			if (rapidPreviewMove) {
				adaptivePreviewTick = true;
				previewAdaptiveCooldown = kPreviewAdaptiveCooldownSamples;
			}
		}

		const bool periodicPreviewTick =
			previewPitchCvConnected ? previewPublishSlowDivider.process() : previewPublishDivider.process();
		const bool previewPublishTick = periodicPreviewTick || adaptivePreviewTick;
		if (!hasLastPreviewState || (previewPublishTick && previewStatesDiffer(previewState, lastPreviewState))) {
			publishPreviewState(previewState);
		}
		if (previewPublishTick) {
			BifurxLlTelemetryState llTelemetryState;
			llTelemetryState.active = (mode == 0);
			llTelemetryState.circuitMode = effectiveCircuitMode;
			llTelemetryState.excitationRms = std::sqrt(std::max(llTelemetryExcitationSq, 0.f));
			llTelemetryState.stageALpRms = std::sqrt(std::max(llTelemetryStageALpSq, 0.f));
			llTelemetryState.stageBLpRms = std::sqrt(std::max(llTelemetryStageBLpSq, 0.f));
			llTelemetryState.outputRms = std::sqrt(std::max(llTelemetryOutputSq, 0.f));
			llTelemetryState.stageBLpOverALpDb =
				amplitudeRatioDb(llTelemetryState.stageBLpRms, llTelemetryState.stageALpRms);
			llTelemetryState.outputOverInputDb =
				amplitudeRatioDb(llTelemetryState.outputRms, llTelemetryState.excitationRms);
			publishLlTelemetryState(llTelemetryState);
		}
		if (measurePerf) {
			perfAnalysisStart = PerfClock::now();
		}

		pushAnalysisSample(in, drivenIn, out);

			lights[FM_AMT_POS_LIGHT].setBrightness(std::max(fmAmt, 0.f));
			lights[FM_AMT_NEG_LIGHT].setBrightness(std::max(-fmAmt, 0.f));

			lights[SPAN_CV_ATTEN_POS_LIGHT].setBrightness(std::max(spanAtten, 0.f));
			lights[SPAN_CV_ATTEN_NEG_LIGHT].setBrightness(std::max(-spanAtten, 0.f));

			const int circuitModeLight = clampCircuitMode(filterCircuitMode);
			lights[FILTER_CIRCUIT_TL_LIGHT].setBrightness(circuitModeLight == 0 ? 1.f : 0.f); // SVF
			lights[FILTER_CIRCUIT_TR_LIGHT].setBrightness(circuitModeLight == 1 ? 1.f : 0.f); // DFM
			lights[FILTER_CIRCUIT_BR_LIGHT].setBrightness(circuitModeLight == 2 ? 1.f : 0.f); // MS2
			lights[FILTER_CIRCUIT_BL_LIGHT].setBrightness(circuitModeLight == 3 ? 1.f : 0.f); // PRD
			if (measurePerf) {
				const PerfClock::time_point perfEnd = PerfClock::now();
				const uint64_t controlsNs = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(perfCoreStart - perfStart).count();
				const uint64_t coreNs = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(perfPreviewStart - perfCoreStart).count();
				const uint64_t previewNs = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(perfAnalysisStart - perfPreviewStart).count();
				const uint64_t analysisNs = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(perfEnd - perfAnalysisStart).count();
				const uint64_t processNs = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(perfEnd - perfStart).count();
				perfAudioSampledCount.fetch_add(1, std::memory_order_relaxed);
				perfAudioProcessNs.fetch_add(processNs, std::memory_order_relaxed);
				perfAudioControlsNs.fetch_add(controlsNs, std::memory_order_relaxed);
				perfAudioCoreNs.fetch_add(coreNs, std::memory_order_relaxed);
				perfAudioPreviewNs.fetch_add(previewNs, std::memory_order_relaxed);
				perfAudioAnalysisNs.fetch_add(analysisNs, std::memory_order_relaxed);
				uint64_t prevMax = perfAudioProcessMaxNs.load(std::memory_order_relaxed);
				while (processNs > prevMax &&
					!perfAudioProcessMaxNs.compare_exchange_weak(prevMax, processNs, std::memory_order_relaxed)) {
				}
			}
		}
	};

namespace {

float orderedSpectrumMagnitude(const float* fftData, int bin) {
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

BifurxSpectrumWidget::BifurxSpectrumWidget()
	: fft(kFftSize) {
	for (int i = 0; i < kFftSize; ++i) {
		window[i] = 0.5f - 0.5f * std::cos(2.f * kPi * float(i) / float(kFftSize - 1));
	}
		for (int i = 0; i < kCurvePointCount; ++i) {
			curveDb[i] = kResponseMinDb;
			curveTargetDb[i] = kResponseMinDb;
			overlayFilterDb[i] = 0.f;
			overlayTargetFilterDb[i] = 0.f;
			overlayModuleDb[i] = 0.f;
			overlayTargetModuleDb[i] = 0.f;
			overlayOutputDbfs[i] = kOverlayDbfsFloor;
			overlayTargetOutputDbfs[i] = kOverlayDbfsFloor;
		}
}

BifurxSpectrumWidget::~BifurxSpectrumWidget() {
	stopCurveDebugCapture();
	stopPerfDebugCapture();
}

void BifurxSpectrumWidget::syncCurveDebugCaptureState() {
	if (!module) {
		stopCurveDebugCapture();
		return;
	}
	if (!module->curveDebugLogging) {
		stopCurveDebugCapture();
		return;
	}
	startCurveDebugCapture();
}

void BifurxSpectrumWidget::syncPerfDebugCaptureState() {
	if (!module) {
		stopPerfDebugCapture();
		return;
	}
	if (!module->perfDebugLogging) {
		stopPerfDebugCapture();
		return;
	}
	startPerfDebugCapture();
}

void BifurxSpectrumWidget::startCurveDebugCapture() {
	if (!module || curveDebugRecorder.active) {
		return;
	}

	std::string traceDir = system::join(bifurxUserRootPath(), "curve_debug");
	system::createDirectories(traceDir);
	const long long stampMs = (long long) std::llround(system::getUnixTime() * 1000.0);
	const std::string filename = "curve_debug_" + std::to_string(stampMs) + ".csv";
	curveDebugRecorder.path = system::join(traceDir, filename);
	curveDebugRecorder.file.open(curveDebugRecorder.path.c_str(), std::ios::out | std::ios::trunc);
	if (!curveDebugRecorder.file.good()) {
		WARN("Bifurx: failed to open curve debug file: %s", curveDebugRecorder.path.c_str());
		curveDebugRecorder.path.clear();
		return;
	}

	curveDebugRecorder.file.setf(std::ios::fixed);
	curveDebugRecorder.file << std::setprecision(6);
	curveDebugRecorder.file << "# Bifurx curve debug trace v3\n";
	curveDebugRecorder.file << "# Start when curve debug logging is enabled, stop when it is disabled\n";
	curveDebugRecorder.file
		<< "seq,t_sec,mode,circuit_mode,freq_param,voct_cv,freq_a_hz,freq_b_hz,"
		   "reso_norm,balance_target,balance_filtered,"
		   "span_param,span_cv,span_atten,span_norm,span_oct,"
		   "ll_active,ll_input_rms,ll_stage_a_lp_rms,ll_stage_b_lp_rms,ll_output_rms,"
		   "ll_stage_b_over_a_db,ll_output_over_input_db,"
		   "preview_seq,preview_updated,analysis_seq,analysis_updated,"
		   "peak_a_x,peak_a_y_curve,peak_a_y_marker,"
		   "peak_b_x,peak_b_y_curve,peak_b_y_marker,ui_frame_ms\n";
	curveDebugRecorder.startTimeSec = system::getTime();
	curveDebugRecorder.sequence = 0;
	curveDebugRecorder.active = true;
	lastCurveDebugLogTimeSec = -1.0;
	INFO("Bifurx: curve debug capture started: %s", curveDebugRecorder.path.c_str());
}

void BifurxSpectrumWidget::stopCurveDebugCapture() {
	if (!curveDebugRecorder.active) {
		return;
	}

	if (curveDebugRecorder.file.good()) {
		curveDebugRecorder.file.flush();
		curveDebugRecorder.file.close();
	}
	INFO("Bifurx: curve debug capture saved: %s", curveDebugRecorder.path.c_str());
	curveDebugRecorder.active = false;
	curveDebugRecorder.startTimeSec = 0.0;
	curveDebugRecorder.sequence = 0;
	curveDebugRecorder.path.clear();
	lastCurveDebugLogTimeSec = -1.0;
}

void BifurxSpectrumWidget::startPerfDebugCapture() {
	if (!module || perfDebugRecorder.active) {
		return;
	}

	perfDebugRecorder = PerfDebugRecorder{};
	std::string traceDir = system::join(bifurxUserRootPath(), "perf_debug");
	system::createDirectories(traceDir);
	const long long stampMs = (long long) std::llround(system::getUnixTime() * 1000.0);
	const std::string filename = "perf_debug_" + std::to_string(stampMs) + ".csv";
	perfDebugRecorder.path = system::join(traceDir, filename);
	perfDebugRecorder.file.open(perfDebugRecorder.path.c_str(), std::ios::out | std::ios::trunc);
	if (!perfDebugRecorder.file.good()) {
		WARN("Bifurx: failed to open perf debug file: %s", perfDebugRecorder.path.c_str());
		perfDebugRecorder.path.clear();
		return;
	}

	module->resetPerfStats();
	uiStepCount = 0;
	uiStepNs = 0;
	uiStepMaxNs = 0;
	uiDrawCount = 0;
	uiDrawNs = 0;
	uiDrawMaxNs = 0;
	uiCurveUpdateCount = 0;
	uiCurveUpdateNs = 0;
	uiOverlayUpdateCount = 0;
	uiOverlayUpdateNs = 0;
	uiDrawSetupCount = 0;
	uiDrawSetupNs = 0;
	uiDrawBackgroundCount = 0;
	uiDrawBackgroundNs = 0;
	uiDrawExpectedCount = 0;
	uiDrawExpectedNs = 0;
	uiDrawOverlayCount = 0;
	uiDrawOverlayNs = 0;
	uiDrawCurveCount = 0;
	uiDrawCurveNs = 0;
	uiDrawMarkersCount = 0;
	uiDrawMarkersNs = 0;
	perfDebugRecorder.file.setf(std::ios::fixed);
	perfDebugRecorder.file << std::setprecision(3);
	perfDebugRecorder.file << "# Bifurx performance debug trace v1\n";
	perfDebugRecorder.file << "# Sampled audio timings plus UI timings aggregated per interval\n";
	perfDebugRecorder.file
		<< "seq,t_sec,"
		   "audio_sampled_calls,audio_avg_us,audio_max_us,audio_controls_avg_us,audio_core_avg_us,audio_preview_avg_us,audio_analysis_avg_us,"
		   "ui_step_calls,ui_step_avg_us,ui_step_max_us,"
		   "ui_draw_calls,ui_draw_avg_us,ui_draw_max_us,"
		   "ui_curve_updates,ui_curve_avg_us,"
		   "ui_overlay_updates,ui_overlay_avg_us,"
		   "ui_draw_setup_avg_us,ui_draw_background_avg_us,ui_draw_expected_avg_us,ui_draw_overlay_avg_us,ui_draw_curve_avg_us,ui_draw_markers_avg_us\n";
	perfDebugRecorder.startTimeSec = system::getTime();
	perfDebugRecorder.active = true;
	perfDebugRecorder.lastLogTimeSec = -1.0;
	INFO("Bifurx: perf debug capture started: %s", perfDebugRecorder.path.c_str());
}

void BifurxSpectrumWidget::stopPerfDebugCapture() {
	if (!perfDebugRecorder.active) {
		return;
	}
	if (perfDebugRecorder.file.good()) {
		perfDebugRecorder.file.flush();
		perfDebugRecorder.file.close();
	}
	INFO("Bifurx: perf debug capture saved: %s", perfDebugRecorder.path.c_str());
	perfDebugRecorder = PerfDebugRecorder{};
}

void BifurxSpectrumWidget::logCurveDebugSample(
	const BifurxPreviewState& state,
	const BifurxLlTelemetryState& llTelemetry,
	float peakAX,
	float peakAYCurve,
	float peakAYMarker,
	float peakBX,
	float peakBYCurve,
	float peakBYMarker,
	float uiFrameMs,
	uint32_t previewSeq,
	bool previewUpdated,
	uint32_t analysisSeq,
	bool analysisUpdated
) {
	if (!module || !curveDebugRecorder.active || !curveDebugRecorder.file.good()) {
		return;
	}

	const double tSec = std::max(0.0, system::getTime() - curveDebugRecorder.startTimeSec);
	curveDebugRecorder.file
		<< curveDebugRecorder.sequence++ << ","
		<< tSec << ","
		<< state.mode << ","
		<< state.circuitMode << ","
		<< state.freqParamNorm << ","
		<< state.voctCv << ","
		<< state.freqA << ","
		<< state.freqB << ","
		<< state.resoNorm << ","
		<< state.balanceTarget << ","
		<< state.balance << ","
		<< state.spanParamNorm << ","
		<< state.spanCvNorm << ","
		<< state.spanAtten << ","
		<< state.spanNorm << ","
		<< state.spanOct << ","
		<< (llTelemetry.active ? 1 : 0) << ","
		<< llTelemetry.excitationRms << ","
		<< llTelemetry.stageALpRms << ","
		<< llTelemetry.stageBLpRms << ","
		<< llTelemetry.outputRms << ","
		<< llTelemetry.stageBLpOverALpDb << ","
		<< llTelemetry.outputOverInputDb << ","
		<< previewSeq << ","
		<< (previewUpdated ? 1 : 0) << ","
		<< analysisSeq << ","
		<< (analysisUpdated ? 1 : 0) << ","
		<< peakAX << ","
		<< peakAYCurve << ","
		<< peakAYMarker << ","
		<< peakBX << ","
		<< peakBYCurve << ","
		<< peakBYMarker << ","
		<< uiFrameMs << "\n";

	if ((curveDebugRecorder.sequence % 120u) == 0u) {
		curveDebugRecorder.file.flush();
	}
}

void BifurxSpectrumWidget::logPerfDebugSample() {
	if (!module || !perfDebugRecorder.active || !perfDebugRecorder.file.good()) {
		return;
	}

	const double tSec = std::max(0.0, system::getTime() - perfDebugRecorder.startTimeSec);
	const uint64_t audioSampledCount = module->perfAudioSampledCount.load(std::memory_order_acquire);
	const uint64_t audioProcessNs = module->perfAudioProcessNs.load(std::memory_order_acquire);
	const uint64_t audioControlsNs = module->perfAudioControlsNs.load(std::memory_order_acquire);
	const uint64_t audioCoreNs = module->perfAudioCoreNs.load(std::memory_order_acquire);
	const uint64_t audioPreviewNs = module->perfAudioPreviewNs.load(std::memory_order_acquire);
	const uint64_t audioAnalysisNs = module->perfAudioAnalysisNs.load(std::memory_order_acquire);
	const uint64_t audioProcessMaxNs = module->perfAudioProcessMaxNs.load(std::memory_order_acquire);

	const uint64_t audioSampledDelta = audioSampledCount - perfDebugRecorder.lastAudioSampledCount;
	const uint64_t audioProcessDeltaNs = audioProcessNs - perfDebugRecorder.lastAudioProcessNs;
	const uint64_t audioControlsDeltaNs = audioControlsNs - perfDebugRecorder.lastAudioControlsNs;
	const uint64_t audioCoreDeltaNs = audioCoreNs - perfDebugRecorder.lastAudioCoreNs;
	const uint64_t audioPreviewDeltaNs = audioPreviewNs - perfDebugRecorder.lastAudioPreviewNs;
	const uint64_t audioAnalysisDeltaNs = audioAnalysisNs - perfDebugRecorder.lastAudioAnalysisNs;

	const uint64_t uiStepDeltaCount = uiStepCount - perfDebugRecorder.lastUiStepCount;
	const uint64_t uiStepDeltaNs = uiStepNs - perfDebugRecorder.lastUiStepNs;
	const uint64_t uiDrawDeltaCount = uiDrawCount - perfDebugRecorder.lastUiDrawCount;
	const uint64_t uiDrawDeltaNs = uiDrawNs - perfDebugRecorder.lastUiDrawNs;
	const uint64_t uiCurveUpdateDeltaCount = uiCurveUpdateCount - perfDebugRecorder.lastUiCurveUpdateCount;
	const uint64_t uiCurveUpdateDeltaNs = uiCurveUpdateNs - perfDebugRecorder.lastUiCurveUpdateNs;
	const uint64_t uiOverlayUpdateDeltaCount = uiOverlayUpdateCount - perfDebugRecorder.lastUiOverlayUpdateCount;
	const uint64_t uiOverlayUpdateDeltaNs = uiOverlayUpdateNs - perfDebugRecorder.lastUiOverlayUpdateNs;
	const uint64_t uiDrawSetupDeltaCount = uiDrawSetupCount - perfDebugRecorder.lastUiDrawSetupCount;
	const uint64_t uiDrawSetupDeltaNs = uiDrawSetupNs - perfDebugRecorder.lastUiDrawSetupNs;
	const uint64_t uiDrawBackgroundDeltaCount = uiDrawBackgroundCount - perfDebugRecorder.lastUiDrawBackgroundCount;
	const uint64_t uiDrawBackgroundDeltaNs = uiDrawBackgroundNs - perfDebugRecorder.lastUiDrawBackgroundNs;
	const uint64_t uiDrawExpectedDeltaCount = uiDrawExpectedCount - perfDebugRecorder.lastUiDrawExpectedCount;
	const uint64_t uiDrawExpectedDeltaNs = uiDrawExpectedNs - perfDebugRecorder.lastUiDrawExpectedNs;
	const uint64_t uiDrawOverlayDeltaCount = uiDrawOverlayCount - perfDebugRecorder.lastUiDrawOverlayCount;
	const uint64_t uiDrawOverlayDeltaNs = uiDrawOverlayNs - perfDebugRecorder.lastUiDrawOverlayNs;
	const uint64_t uiDrawCurveDeltaCount = uiDrawCurveCount - perfDebugRecorder.lastUiDrawCurveCount;
	const uint64_t uiDrawCurveDeltaNs = uiDrawCurveNs - perfDebugRecorder.lastUiDrawCurveNs;
	const uint64_t uiDrawMarkersDeltaCount = uiDrawMarkersCount - perfDebugRecorder.lastUiDrawMarkersCount;
	const uint64_t uiDrawMarkersDeltaNs = uiDrawMarkersNs - perfDebugRecorder.lastUiDrawMarkersNs;

	auto avgUs = [](uint64_t totalNs, uint64_t count) {
		return count > 0 ? (double(totalNs) / double(count)) / 1000.0 : 0.0;
	};

	perfDebugRecorder.file
		<< perfDebugRecorder.sequence++ << ","
		<< tSec << ","
		<< audioSampledDelta << ","
		<< avgUs(audioProcessDeltaNs, audioSampledDelta) << ","
		<< (double(audioProcessMaxNs) / 1000.0) << ","
		<< avgUs(audioControlsDeltaNs, audioSampledDelta) << ","
		<< avgUs(audioCoreDeltaNs, audioSampledDelta) << ","
		<< avgUs(audioPreviewDeltaNs, audioSampledDelta) << ","
		<< avgUs(audioAnalysisDeltaNs, audioSampledDelta) << ","
		<< uiStepDeltaCount << ","
		<< avgUs(uiStepDeltaNs, uiStepDeltaCount) << ","
		<< (double(uiStepMaxNs) / 1000.0) << ","
		<< uiDrawDeltaCount << ","
		<< avgUs(uiDrawDeltaNs, uiDrawDeltaCount) << ","
		<< (double(uiDrawMaxNs) / 1000.0) << ","
		<< uiCurveUpdateDeltaCount << ","
		<< avgUs(uiCurveUpdateDeltaNs, uiCurveUpdateDeltaCount) << ","
		<< uiOverlayUpdateDeltaCount << ","
		<< avgUs(uiOverlayUpdateDeltaNs, uiOverlayUpdateDeltaCount) << ","
		<< avgUs(uiDrawSetupDeltaNs, uiDrawSetupDeltaCount) << ","
		<< avgUs(uiDrawBackgroundDeltaNs, uiDrawBackgroundDeltaCount) << ","
		<< avgUs(uiDrawExpectedDeltaNs, uiDrawExpectedDeltaCount) << ","
		<< avgUs(uiDrawOverlayDeltaNs, uiDrawOverlayDeltaCount) << ","
		<< avgUs(uiDrawCurveDeltaNs, uiDrawCurveDeltaCount) << ","
		<< avgUs(uiDrawMarkersDeltaNs, uiDrawMarkersDeltaCount) << "\n";

	perfDebugRecorder.lastAudioSampledCount = audioSampledCount;
	perfDebugRecorder.lastAudioProcessNs = audioProcessNs;
	perfDebugRecorder.lastAudioControlsNs = audioControlsNs;
	perfDebugRecorder.lastAudioCoreNs = audioCoreNs;
	perfDebugRecorder.lastAudioPreviewNs = audioPreviewNs;
	perfDebugRecorder.lastAudioAnalysisNs = audioAnalysisNs;
	perfDebugRecorder.lastUiStepCount = uiStepCount;
	perfDebugRecorder.lastUiStepNs = uiStepNs;
	perfDebugRecorder.lastUiDrawCount = uiDrawCount;
	perfDebugRecorder.lastUiDrawNs = uiDrawNs;
	perfDebugRecorder.lastUiCurveUpdateCount = uiCurveUpdateCount;
	perfDebugRecorder.lastUiCurveUpdateNs = uiCurveUpdateNs;
	perfDebugRecorder.lastUiOverlayUpdateCount = uiOverlayUpdateCount;
	perfDebugRecorder.lastUiOverlayUpdateNs = uiOverlayUpdateNs;
	perfDebugRecorder.lastUiDrawSetupCount = uiDrawSetupCount;
	perfDebugRecorder.lastUiDrawSetupNs = uiDrawSetupNs;
	perfDebugRecorder.lastUiDrawBackgroundCount = uiDrawBackgroundCount;
	perfDebugRecorder.lastUiDrawBackgroundNs = uiDrawBackgroundNs;
	perfDebugRecorder.lastUiDrawExpectedCount = uiDrawExpectedCount;
	perfDebugRecorder.lastUiDrawExpectedNs = uiDrawExpectedNs;
	perfDebugRecorder.lastUiDrawOverlayCount = uiDrawOverlayCount;
	perfDebugRecorder.lastUiDrawOverlayNs = uiDrawOverlayNs;
	perfDebugRecorder.lastUiDrawCurveCount = uiDrawCurveCount;
	perfDebugRecorder.lastUiDrawCurveNs = uiDrawCurveNs;
	perfDebugRecorder.lastUiDrawMarkersCount = uiDrawMarkersCount;
	perfDebugRecorder.lastUiDrawMarkersNs = uiDrawMarkersNs;

	if ((perfDebugRecorder.sequence % 20u) == 0u) {
		perfDebugRecorder.file.flush();
	}
}

void BifurxSpectrumWidget::updateAxisCache() {
	if (!hasPreview) {
		return;
	}

	const float sampleRate = std::max(previewState.sampleRate, 1.f);
	if (cachedAxisSampleRate > 0.f && std::fabs(cachedAxisSampleRate - sampleRate) <= 0.5f) {
		return;
	}

	const float minHz = 10.f;
	const float maxHz = std::min(20000.f, 0.46f * sampleRate);
	for (int i = 0; i < kCurvePointCount; ++i) {
		const float x01 = float(i) / float(kCurvePointCount - 1);
		const float hz = logFrequencyAt(x01, minHz, maxHz);
		curveHz[i] = hz;
		curveBinPos[i] = clamp(hz * float(kFftSize) / sampleRate, 0.f, float(kFftSize / 2));
	}
	cachedAxisSampleRate = sampleRate;
}

void BifurxSpectrumWidget::step() {
	using PerfClock = std::chrono::steady_clock;
	const bool perfLoggingActive = module && module->perfDebugLogging;
	const PerfClock::time_point perfStepStart = perfLoggingActive ? PerfClock::now() : PerfClock::time_point();
	Widget::step();
	syncCurveDebugCaptureState();
	syncPerfDebugCaptureState();
	if (!module) {
		return;
	}

	bool dirty = false;
	bool previewUpdatedThisStep = false;
	bool analysisUpdatedThisStep = false;
	const bool fftScaleDynamicNow = module->fftScaleDynamic;
	if (fftScaleDynamicNow != lastFftScaleDynamic) {
		lastFftScaleDynamic = fftScaleDynamicNow;
		if (!fftScaleDynamicNow) {
			displayTopDbfs = kDisplayTopDbfsCeiling;
			displayTopTargetDbfs = kDisplayTopDbfsCeiling;
		}
		dirty = true;
	}

	const uint32_t previewSeq = module->previewPublishSeq.load(std::memory_order_acquire);
	if (previewSeq != lastPreviewSeq) {
		const int index = module->previewPublishedIndex.load(std::memory_order_acquire);
		previewState = module->previewStates[index];
		hasPreview = true;
		updateAxisCache();
		lastPreviewSeq = previewSeq;
		previewUpdatedThisStep = true;
		const PerfClock::time_point perfCurveStart = perfLoggingActive ? PerfClock::now() : PerfClock::time_point();
		updateCurveCache();
		if (perfLoggingActive) {
			const uint64_t curveUpdateNs = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(PerfClock::now() - perfCurveStart).count();
			uiCurveUpdateCount++;
			uiCurveUpdateNs += curveUpdateNs;
		}
		dirty = true;
	}

	const uint32_t llTelemetrySeq = module->llTelemetryPublishSeq.load(std::memory_order_acquire);
	if (llTelemetrySeq != lastLlTelemetrySeq) {
		const int index = module->llTelemetryPublishedIndex.load(std::memory_order_acquire);
		llTelemetryState = module->llTelemetryStates[index];
		hasLlTelemetry = true;
		lastLlTelemetrySeq = llTelemetrySeq;
	}

	const uint32_t analysisSeq = module->analysisPublishSeq.load(std::memory_order_acquire);
	if (analysisSeq != lastAnalysisSeq) {
		const int index = module->analysisPublishedIndex.load(std::memory_order_acquire);
		const PerfClock::time_point perfOverlayStart = perfLoggingActive ? PerfClock::now() : PerfClock::time_point();
		updateOverlayCache(module->analysisFrames[index]);
		if (perfLoggingActive) {
			const uint64_t overlayUpdateNs = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(PerfClock::now() - perfOverlayStart).count();
			uiOverlayUpdateCount++;
			uiOverlayUpdateNs += overlayUpdateNs;
		}
		hasOverlay = true;
		lastAnalysisSeq = analysisSeq;
		analysisUpdatedThisStep = true;
		dirty = true;
	}

	if (hasCurveTarget) {
		bool curveAnimating = false;
		float uiFrameSec = 1.f / 60.f;
		if (APP && APP->window) {
			const float frameSec = float(APP->window->getLastFrameDuration());
			if (std::isfinite(frameSec) && frameSec > 0.f) {
				uiFrameSec = clamp(frameSec, 1.f / 240.f, 1.f / 20.f);
			}
		}
		const float curveMaxStepDb = std::max(0.25f, kCurveVisualSlewDbPerSec * uiFrameSec);
		for (int i = 0; i < kCurvePointCount; ++i) {
			const float prev = curveDb[i];
			float delta = curveTargetDb[i] - prev;
			delta = clamp(delta, -curveMaxStepDb, curveMaxStepDb);
			const float next = prev + delta;
			curveDb[i] = next;
			if (std::fabs(next - prev) > 0.01f) {
				curveAnimating = true;
			}
		}
		dirty = dirty || curveAnimating;
	}

		if (hasOverlayTarget) {
			bool overlayAnimating = false;
			const float overlayDbSmoothing = 0.22f;
			const float overlayLevelSmoothing = 0.20f;
			for (int i = 0; i < kCurvePointCount; ++i) {
				const float prevFilterDb = overlayFilterDb[i];
				const float nextFilterDb = mixf(prevFilterDb, overlayTargetFilterDb[i], overlayDbSmoothing);
				overlayFilterDb[i] = nextFilterDb;
				if (std::fabs(nextFilterDb - prevFilterDb) > 0.02f) {
					overlayAnimating = true;
				}

				const float prevModuleDb = overlayModuleDb[i];
				const float nextModuleDb = mixf(prevModuleDb, overlayTargetModuleDb[i], overlayDbSmoothing);
				overlayModuleDb[i] = nextModuleDb;
				if (std::fabs(nextModuleDb - prevModuleDb) > 0.02f) {
					overlayAnimating = true;
				}

			const float prevLevel = overlayOutputDbfs[i];
			const float nextLevel = mixf(prevLevel, overlayTargetOutputDbfs[i], overlayLevelSmoothing);
			overlayOutputDbfs[i] = nextLevel;
			if (std::fabs(nextLevel - prevLevel) > 0.02f) {
				overlayAnimating = true;
			}
		}

		const float prevTop = displayTopDbfs;
		float topSmoothing = (displayTopTargetDbfs > prevTop) ? 0.22f : 0.10f;
		if (module && module->fftScaleDynamic && displayTopTargetDbfs > prevTop) {
			// Fast attack in dynamic mode so short peaks stay in-frame.
			topSmoothing = 0.70f;
		}
		displayTopDbfs = mixf(prevTop, displayTopTargetDbfs, topSmoothing);
		if (std::fabs(displayTopDbfs - prevTop) > 0.02f) {
			overlayAnimating = true;
		}

		dirty = dirty || overlayAnimating;
	}

	if (module->curveDebugLogging && hasPreview) {
		const double nowSec = system::getTime();
		const double minIntervalSec = 1.0 / 60.0;
		if (lastCurveDebugLogTimeSec < 0.0 || (nowSec - lastCurveDebugLogTimeSec) >= minIntervalSec) {
			lastCurveDebugLogTimeSec = nowSec;

			float uiFrameMs = NAN;
			if (APP && APP->window) {
				const double frameSec = APP->window->getLastFrameDuration();
				if (std::isfinite(frameSec) && frameSec > 0.0) {
					uiFrameMs = float(frameSec * 1000.0);
				}
			}

			float peakAX = NAN;
			float peakAYCurve = NAN;
			float peakAYMarker = NAN;
			float peakBX = NAN;
			float peakBYCurve = NAN;
			float peakBYMarker = NAN;
			const float w = box.size.x;
			const float h = box.size.y;
			if (w > 0.f && h > 0.f) {
				const BifurxPreviewModel model = makePreviewModel(previewState);
				const float padX = 0.f;
				const float padY = std::max(4.f, h * 0.035f);
				const float plotX = padX;
				const float usableW = std::max(1.f, w - plotX - padX);
				const float minHz = 10.f;
				const float maxHz = std::min(20000.f, 0.46f * previewState.sampleRate);
				const float labelBandHeight = std::max(5.2f, h * 0.072f);
				const float labelBandTop = h - labelBandHeight;
				const float spectrumTopY = padY * 0.35f;
				const float spectrumBottomY = std::max(spectrumTopY + 1.f, labelBandTop - std::max(0.05f, h * 0.0008f));
				auto responseYForDb = [&](float db) {
					return responseYForDbDisplay(db, kResponseMinDb, kResponseMaxDb, spectrumBottomY, spectrumTopY);
				};
				const bool anchorMarkerToBottomLane = (previewState.mode == 3);
				auto evalPeak = [&](float targetHz, float* outX, float* outYCurve, float* outYMarker) {
					const float clampedHz = clamp(targetHz, minHz, maxHz);
					const float targetX01 = logPosition(clampedHz, minHz, maxHz);
					const float markerRadius = kPeakMarkerFillRadius + kPeakMarkerOutlineExtraRadius + 0.5f * kPeakMarkerOutlineStrokeWidth;
					const float curveIndex = targetX01 * float(kCurvePointCount - 1);
					const int i0 = clamp(int(std::floor(curveIndex)), 0, kCurvePointCount - 1);
					const int i1 = std::min(i0 + 1, kCurvePointCount - 1);
					const float t = curveIndex - float(i0);
					const float curveDbAtHz = mixf(curveDb[i0], curveDb[i1], t);
					const float yCurve = responseYForDb(curveDbAtHz);
					const float markerX = plotX + usableW * targetX01;
					const float markerMinY = spectrumTopY + markerRadius + kPeakMarkerEdgePadding;
					const float markerMaxY = spectrumBottomY - markerRadius - kPeakMarkerEdgePadding;
					const float bottomLaneY = spectrumBottomY - markerRadius - kPeakMarkerBottomLanePadding;
					const float yMarker = anchorMarkerToBottomLane ? bottomLaneY : clamp(yCurve, markerMinY, markerMaxY);
					*outX = clamp(
						markerX,
						plotX + markerRadius + kPeakMarkerEdgePadding,
						plotX + usableW - markerRadius - kPeakMarkerEdgePadding
					);
					*outYCurve = yCurve;
					*outYMarker = yMarker;
				};
				evalPeak(model.markerFreqA, &peakAX, &peakAYCurve, &peakAYMarker);
				evalPeak(model.markerFreqB, &peakBX, &peakBYCurve, &peakBYMarker);
			}

			logCurveDebugSample(
				previewState,
				hasLlTelemetry ? llTelemetryState : BifurxLlTelemetryState{},
				peakAX, peakAYCurve, peakAYMarker,
				peakBX, peakBYCurve, peakBYMarker,
				uiFrameMs,
				previewSeq, previewUpdatedThisStep,
				analysisSeq, analysisUpdatedThisStep
			);
		}
	}
	else if (lastCurveDebugLogTimeSec >= 0.0) {
		lastCurveDebugLogTimeSec = -1.0;
	}

	if (perfDebugRecorder.active) {
		const double nowSec = system::getTime();
		const double minIntervalSec = 0.5;
		if (perfDebugRecorder.lastLogTimeSec < 0.0 || (nowSec - perfDebugRecorder.lastLogTimeSec) >= minIntervalSec) {
			perfDebugRecorder.lastLogTimeSec = nowSec;
			logPerfDebugSample();
		}
	}

	if (dirty && framebuffer) {
		framebuffer->setDirty();
	}
	if (perfLoggingActive) {
		const uint64_t stepNs = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(PerfClock::now() - perfStepStart).count();
		uiStepCount++;
		uiStepNs += stepNs;
		uiStepMaxNs = std::max(uiStepMaxNs, stepNs);
	}
}

void BifurxSpectrumWidget::updateCurveCache() {
	if (!hasPreview) {
		return;
	}

	updateAxisCache();
	if (previewState.circuitMode == 0) {
		const BifurxPreviewModel model = makePreviewModel(previewState);
		for (int i = 0; i < kCurvePointCount; ++i) {
			const float db = previewModelResponseDb(model, curveHz[i]);
			curveTargetDb[i] = clamp(db, kResponseMinDb, kResponseMaxDb);
		}
	}
	else {
		simulatePreviewProbeImpulseResponse(previewState, fftInputTime, fftOutputTime, kFftSize);
		fft.rfft(fftInputTime, fftInputFreq);
		fft.rfft(fftOutputTime, fftOutputFreq);

		float binResponseDb[kFftBinCount];
		for (int bin = 0; bin < kFftBinCount; ++bin) {
			const float inputAmp = orderedSpectrumMagnitude(fftInputFreq, bin);
			const float outputAmp = orderedSpectrumMagnitude(fftOutputFreq, bin);
			binResponseDb[bin] = clamp(
				20.f * std::log10((outputAmp + 1e-9f) / (inputAmp + 1e-9f)),
				kResponseMinDb,
				kResponseMaxDb
			);
		}

		for (int i = 0; i < kCurvePointCount; ++i) {
			const float binPosition = curveBinPos[i];
			const int binA = clamp(int(std::floor(binPosition)), 0, kFftBinCount - 1);
			const int binB = std::min(binA + 1, kFftBinCount - 1);
			const float frac = binPosition - float(binA);
			const int left = std::max(0, binA - 1);
			const int right = std::min(kFftBinCount - 1, binB + 1);
			const float smoothA = 0.18f * binResponseDb[left] + 0.64f * binResponseDb[binA] + 0.18f * binResponseDb[right];
			const float smoothB = 0.18f * binResponseDb[binA] + 0.64f * binResponseDb[binB] + 0.18f * binResponseDb[right];
			const float db = mixf(smoothA, smoothB, frac);
			curveTargetDb[i] = clamp(db, kResponseMinDb, kResponseMaxDb);
		}
	}

	if (!hasCurveTarget) {
		for (int i = 0; i < kCurvePointCount; ++i) {
			curveDb[i] = curveTargetDb[i];
		}
		hasCurveTarget = true;
	}
}

void BifurxSpectrumWidget::updateOverlayCache(const BifurxAnalysisFrame& frame) {
	if (!hasPreview) {
		return;
	}

	updateAxisCache();
	const float amplitudeScale = 4.f / float(kFftSize);

	for (int i = 0; i < kFftSize; ++i) {
		fftInputTime[i] = frame.input[i] * window[i];
		fftOutputTime[i] = frame.output[i] * window[i];
	}

	fft.rfft(fftInputTime, fftInputFreq);
	fft.rfft(fftOutputTime, fftOutputFreq);

	for (int i = 0; i < kFftSize; ++i) {
		fftInputTime[i] = frame.rawInput[i] * window[i];
	}
	float fftRawInputFreq[2 * kFftSize];
	fft.rfft(fftInputTime, fftRawInputFreq);

	float binFilterDeltaDb[kFftBinCount];
	float binModuleDeltaDb[kFftBinCount];
	float binOutputDbfs[kFftBinCount];
	float binRawInputAmp[kFftBinCount];
	float binInputAmp[kFftBinCount];
	float binOutputAmp[kFftBinCount];
	for (int bin = 0; bin < kFftBinCount; ++bin) {
		const float binHz = (float(bin) * previewState.sampleRate) / float(kFftSize);
		const float subsonicWeight = clamp01((binHz - kOverlaySubsonicCutHz) / (kOverlaySubsonicFadeHz - kOverlaySubsonicCutHz));
		const float weightedRawInputAmp = subsonicWeight * amplitudeScale * orderedSpectrumMagnitude(fftRawInputFreq, bin);
		const float weightedInputAmp = subsonicWeight * amplitudeScale * orderedSpectrumMagnitude(fftInputFreq, bin);
		const float weightedOutputAmp = subsonicWeight * amplitudeScale * orderedSpectrumMagnitude(fftOutputFreq, bin);
		binRawInputAmp[bin] = weightedRawInputAmp;
		binInputAmp[bin] = weightedInputAmp;
		binOutputAmp[bin] = weightedOutputAmp;
	}

	// Use short band-energy windows (not single bins) so nonlinear circuit
	// coloration still reads as local lift/cut instead of jittery per-bin flips.
	constexpr int kOverlayBandRadius = 2;
	constexpr float kOverlayBandKernel[2 * kOverlayBandRadius + 1] = {0.08f, 0.24f, 0.36f, 0.24f, 0.08f};
	for (int bin = 0; bin < kFftBinCount; ++bin) {
		float rawInputEnergy = 0.f;
		float inputEnergy = 0.f;
		float outputEnergy = 0.f;
		for (int k = -kOverlayBandRadius; k <= kOverlayBandRadius; ++k) {
			const int sampleBin = clamp(bin + k, 0, kFftBinCount - 1);
			const float w = kOverlayBandKernel[k + kOverlayBandRadius];
			const float rawInAmp = binRawInputAmp[sampleBin];
			const float inAmp = binInputAmp[sampleBin];
			const float outAmp = binOutputAmp[sampleBin];
			rawInputEnergy += w * rawInAmp * rawInAmp;
			inputEnergy += w * inAmp * inAmp;
			outputEnergy += w * outAmp * outAmp;
		}
		const float rawInputAmp = std::sqrt(std::max(0.f, rawInputEnergy));
		const float inputAmp = std::sqrt(std::max(0.f, inputEnergy));
		const float outputAmp = std::sqrt(std::max(0.f, outputEnergy));
		binFilterDeltaDb[bin] = clamp(20.f * std::log10((outputAmp + 1e-6f) / (inputAmp + 1e-6f)), -24.f, 24.f);
		binModuleDeltaDb[bin] = clamp(20.f * std::log10((outputAmp + 1e-6f) / (rawInputAmp + 1e-6f)), -24.f, 24.f);
		binOutputDbfs[bin] = clamp(20.f * std::log10(outputAmp / 5.f + 1e-6f), kOverlayDbfsFloor, kOverlayDbfsCeiling);
	}

	float sampledFilterDeltaDb[kCurvePointCount];
	float sampledModuleDeltaDb[kCurvePointCount];
	float sampledOutputDbfs[kCurvePointCount];
	for (int i = 0; i < kCurvePointCount; ++i) {
		const float binPosition = curveBinPos[i];
		const int binA = std::max(2, int(std::floor(binPosition)));
		const int binB = std::min(binA + 1, kFftSize / 2);
		const float frac = binPosition - float(binA);
		sampledFilterDeltaDb[i] = mixf(binFilterDeltaDb[binA], binFilterDeltaDb[binB], frac);
		sampledModuleDeltaDb[i] = mixf(binModuleDeltaDb[binA], binModuleDeltaDb[binB], frac);
		sampledOutputDbfs[i] = mixf(binOutputDbfs[binA], binOutputDbfs[binB], frac);
	}

	float framePeakDbfs = kOverlayDbfsFloor;
	float frameSmoothedOutputDbfs[kCurvePointCount];
	const float targetSmoothing = hasOverlayTarget ? 0.45f : 1.f;
	for (int i = 0; i < kCurvePointCount; ++i) {
		const int left = std::max(0, i - 1);
		const int right = std::min(kCurvePointCount - 1, i + 1);
		const float smoothFilterDeltaDb = 0.12f * sampledFilterDeltaDb[left] + 0.76f * sampledFilterDeltaDb[i] + 0.12f * sampledFilterDeltaDb[right];
		const float smoothModuleDeltaDb = 0.12f * sampledModuleDeltaDb[left] + 0.76f * sampledModuleDeltaDb[i] + 0.12f * sampledModuleDeltaDb[right];
		const float smoothOutputDbfs = 0.12f * sampledOutputDbfs[left] + 0.76f * sampledOutputDbfs[i] + 0.12f * sampledOutputDbfs[right];
		frameSmoothedOutputDbfs[i] = smoothOutputDbfs;
		overlayTargetFilterDb[i] = mixf(overlayTargetFilterDb[i], smoothFilterDeltaDb, targetSmoothing);
		overlayTargetModuleDb[i] = mixf(overlayTargetModuleDb[i], smoothModuleDeltaDb, targetSmoothing);
		overlayTargetOutputDbfs[i] = mixf(overlayTargetOutputDbfs[i], smoothOutputDbfs, targetSmoothing);
		framePeakDbfs = std::max(framePeakDbfs, overlayTargetOutputDbfs[i]);
	}

	if (!hasOverlayTarget) {
		for (int i = 0; i < kCurvePointCount; ++i) {
			overlayFilterDb[i] = overlayTargetFilterDb[i];
			overlayModuleDb[i] = overlayTargetModuleDb[i];
			overlayOutputDbfs[i] = overlayTargetOutputDbfs[i];
		}
		hasOverlayTarget = true;
	}

	if (module && !module->fftScaleDynamic) {
		displayTopTargetDbfs = kDisplayTopDbfsCeiling;
	}
	else {
		// Use a robust upper reference so isolated spikes don't collapse the full display range.
		float sortedOutputDbfs[kCurvePointCount];
		for (int i = 0; i < kCurvePointCount; ++i) {
			sortedOutputDbfs[i] = frameSmoothedOutputDbfs[i];
		}
		const int p95Index = int(0.95f * float(kCurvePointCount - 1));
		std::nth_element(sortedOutputDbfs, sortedOutputDbfs + p95Index, sortedOutputDbfs + kCurvePointCount);
		const float p95Dbfs = sortedOutputDbfs[p95Index];
		const float robustTopRefDbfs = std::max(p95Dbfs, framePeakDbfs - 18.f);
		const float desiredTopDbfs = std::max(robustTopRefDbfs + 6.f, framePeakDbfs + kDisplayPeakHeadroomDb);
		displayTopTargetDbfs = clamp(desiredTopDbfs, kDisplayTopDbfsFloor, kDisplayTopDynamicCeilingDbfs);
	}
}

void BifurxSpectrumWidget::draw(const DrawArgs& args) {
	if (!hasPreview) {
		return;
	}

	const float w = box.size.x;
	const float h = box.size.y;
	if (!(w > 0.f && h > 0.f)) {
		return;
	}

	using PerfClock = std::chrono::steady_clock;
	const bool perfLoggingActive = module && module->perfDebugLogging;
	const PerfClock::time_point perfDrawStart = perfLoggingActive ? PerfClock::now() : PerfClock::time_point();
	PerfClock::time_point perfSectionStart = perfDrawStart;
	auto recordDrawSection = [&](uint64_t& count, uint64_t& totalNs) {
		if (!perfLoggingActive) {
			return;
		}
		const PerfClock::time_point now = PerfClock::now();
		const uint64_t ns = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(now - perfSectionStart).count();
		count++;
		totalNs += ns;
		perfSectionStart = now;
	};

	const float padX = 0.f;
	const float padY = std::max(4.f, h * 0.035f);
	const float plotX = padX;
	const float usableW = std::max(1.f, w - plotX - padX);
	const float minHz = 10.f;
	const float maxHz = std::min(20000.f, 0.46f * previewState.sampleRate);
	const float labelBandHeight = std::max(5.2f, h * 0.072f);
	const float labelBandTop = h - labelBandHeight;
	const float spectrumTopY = padY * 0.35f;
	const float spectrumBottomY = std::max(spectrumTopY + 1.f, labelBandTop - std::max(0.05f, h * 0.0008f));
	bottomY = spectrumBottomY;
	const float displayMaxDbfs = displayTopDbfs;
	const float displayMinDbfs = displayMaxDbfs - kDisplayDbfsSpan;
	const float responseMinDb = kResponseMinDb;
	const float responseMaxDb = kResponseMaxDb;
	auto responseYForDb = [&](float db) {
		return responseYForDbDisplay(db, responseMinDb, responseMaxDb, spectrumBottomY, spectrumTopY);
	};
	const bool anchorMarkerToBottomLane = (previewState.mode == 3);
	const float markerOuterRadius = kPeakMarkerFillRadius + kPeakMarkerOutlineExtraRadius + 0.5f * kPeakMarkerOutlineStrokeWidth;
	const float markerBottomLaneY = spectrumBottomY - markerOuterRadius - kPeakMarkerBottomLanePadding;
	const BifurxPreviewModel model = makePreviewModel(previewState);

	for (int i = 0; i < kCurvePointCount; ++i) {
		const float x01 = float(i) / float(kCurvePointCount - 1);
		curveX[i] = plotX + usableW * x01;
		curveY[i] = responseYForDb(curveDb[i]);
	}

	struct CurveDrawPoint {
		float x01 = 0.f;
		float x = 0.f;
		float y = 0.f;
		int priority = 0;
	};
	CurveDrawPoint dedupedCurveDrawPoints[kCurvePointCount + 6];
	int dedupedCurveDrawPointCount = 0;
	for (int i = 0; i < kCurvePointCount; ++i) {
		CurveDrawPoint point;
		point.x01 = float(i) / float(kCurvePointCount - 1);
		point.x = curveX[i];
		point.y = curveY[i];
		dedupedCurveDrawPoints[dedupedCurveDrawPointCount++] = point;
	}
	auto insertCurveDrawPoint = [&](const CurveDrawPoint& point) {
		constexpr float kCurveDrawPointEpsilon = 1e-6f;
		int insertIndex = dedupedCurveDrawPointCount;
		for (int i = 0; i < dedupedCurveDrawPointCount; ++i) {
			const float dx = point.x01 - dedupedCurveDrawPoints[i].x01;
			if (std::fabs(dx) <= kCurveDrawPointEpsilon) {
				if (point.priority >= dedupedCurveDrawPoints[i].priority) {
					dedupedCurveDrawPoints[i] = point;
				}
				return;
			}
			if (dx < 0.f) {
				insertIndex = i;
				break;
			}
		}
		if (dedupedCurveDrawPointCount >= (kCurvePointCount + 6)) {
			return;
		}
		for (int i = dedupedCurveDrawPointCount; i > insertIndex; --i) {
			dedupedCurveDrawPoints[i] = dedupedCurveDrawPoints[i - 1];
		}
		dedupedCurveDrawPoints[insertIndex] = point;
		dedupedCurveDrawPointCount++;
	};
	auto addCurveRefinementAround = [&](float targetHz) {
		const float clampedHz = clamp(targetHz, minHz, maxHz);
		const float targetX01 = logPosition(clampedHz, minHz, maxHz);
		const float refineDx = 0.35f / float(kCurvePointCount - 1);
		const float refineX01[3] = {
			clamp(targetX01 - refineDx, 0.f, 1.f),
			targetX01,
			clamp(targetX01 + refineDx, 0.f, 1.f)
		};
		for (int i = 0; i < 3; ++i) {
			const float sampleX01 = refineX01[i];
			const float sampleHz = (i == 1) ? clampedHz : logFrequencyAt(sampleX01, minHz, maxHz);
			const float curveIndex = logPosition(sampleHz, minHz, maxHz) * float(kCurvePointCount - 1);
			const int i0 = clamp(int(std::floor(curveIndex)), 0, kCurvePointCount - 1);
			const int i1 = std::min(i0 + 1, kCurvePointCount - 1);
			const float t = curveIndex - float(i0);
			// Keep refinement points tied to the same smoothed curve cache as
			// the vertical bars and peak markers so cutoff moves stay coherent.
			const float sampleDb = mixf(curveDb[i0], curveDb[i1], t);
			CurveDrawPoint point;
			point.x01 = sampleX01;
			point.x = plotX + usableW * sampleX01;
			point.y = responseYForDb(sampleDb);
			if (anchorMarkerToBottomLane && i == 1) {
				// In NN mode, present notch centers as pinned cut points at the marker lane.
				point.y = markerBottomLaneY;
				point.priority = 2;
			}
			else {
				point.priority = 1;
			}
			insertCurveDrawPoint(point);
		}
	};
	addCurveRefinementAround(model.markerFreqA);
	addCurveRefinementAround(model.markerFreqB);
	recordDrawSection(uiDrawSetupCount, uiDrawSetupNs);

	nvgSave(args.vg);
	const float clipInset = 0.8f;
	nvgScissor(args.vg, clipInset, clipInset, std::max(0.f, w - 2.f * clipInset), std::max(0.f, h - 2.f * clipInset));

	nvgBeginPath(args.vg);
	nvgRect(args.vg, 0.f, 0.f, w, h);
	nvgFillColor(args.vg, nvgRGBA(7, 10, 14, 26));
	nvgFill(args.vg);

	nvgBeginPath(args.vg);
	nvgRect(args.vg, 0.f, labelBandTop, w, h - labelBandTop);
	nvgFillColor(args.vg, nvgRGBA(4, 7, 11, 208));
	nvgFill(args.vg);
	nvgBeginPath(args.vg);
	nvgMoveTo(args.vg, 0.f, labelBandTop);
	nvgLineTo(args.vg, w, labelBandTop);
	nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 20));
	nvgStrokeWidth(args.vg, 1.f);
	nvgStroke(args.vg);

	// Clip plot rendering above the bottom label strip so the curve/overlay
	// never dives into the label area.
	nvgSave(args.vg);
	nvgScissor(args.vg, plotX, 0.f, usableW, std::max(1.f, spectrumBottomY));

	for (int i = 0; i < kGuideCount; ++i) {
		if (kGuideFreqs[i] >= maxHz) {
			continue;
		}
		const float guideX = plotX + usableW * logPosition(kGuideFreqs[i], minHz, maxHz);
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, guideX, padY * 0.35f);
		nvgLineTo(args.vg, guideX, bottomY);
		nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 18));
		nvgStrokeWidth(args.vg, 1.f);
		nvgStroke(args.vg);
	}

	auto spectrumYForDbfs = [&](float dbfs) {
		return rescale(clamp(dbfs, displayMinDbfs, displayMaxDbfs), displayMinDbfs, displayMaxDbfs, spectrumBottomY, spectrumTopY);
	};

	const int tickStartDb = int(std::floor(displayMinDbfs / 3.f)) * 3;
	const int tickEndDb = int(std::ceil(displayMaxDbfs / 3.f)) * 3;
	for (int tickDb = tickStartDb; tickDb <= tickEndDb; tickDb += 3) {
		const float y = spectrumYForDbfs(float(tickDb));
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, plotX, y);
		nvgLineTo(args.vg, plotX + usableW, y);
		nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, (tickDb == 0) ? 34 : 12));
		nvgStrokeWidth(args.vg, (tickDb == 0) ? 1.f : 0.65f);
		nvgStroke(args.vg);
	}

	nvgFontSize(args.vg, std::max(7.f, h * 0.05f));
	nvgFontFaceId(args.vg, APP->window->uiFont->handle);
	nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 255));
	auto compactSignedLabel = [](float value, char* out, size_t outSize) {
		std::snprintf(out, outSize, "%+.1f", value);
	};

	char valueLabel[12];
	compactSignedLabel(displayMaxDbfs, valueLabel, sizeof(valueLabel));
	char topLabel[24];
	std::snprintf(topLabel, sizeof(topLabel), "%5s dBFS", valueLabel);
	auto measureTopLabelWidthForValue = [&](float db) {
		char sampleValue[12];
		compactSignedLabel(db, sampleValue, sizeof(sampleValue));
		char sampleLabel[24];
		std::snprintf(sampleLabel, sizeof(sampleLabel), "%5s dBFS", sampleValue);
		return nvgTextBounds(args.vg, 0.f, 0.f, sampleLabel, nullptr, nullptr);
	};
	float topLabelReservedWidth = 0.f;
	topLabelReservedWidth = std::max(topLabelReservedWidth, measureTopLabelWidthForValue(kDisplayTopDbfsFloor));
	topLabelReservedWidth = std::max(topLabelReservedWidth, measureTopLabelWidthForValue(-10.f));
	topLabelReservedWidth = std::max(topLabelReservedWidth, measureTopLabelWidthForValue(-1.f));
	topLabelReservedWidth = std::max(topLabelReservedWidth, measureTopLabelWidthForValue(kDisplayTopDbfsCeiling));
	topLabelReservedWidth = std::max(topLabelReservedWidth, measureTopLabelWidthForValue(kDisplayTopDynamicCeilingDbfs));
	const float topLabelRightX = 1.5f + topLabelReservedWidth;
	nvgTextAlign(args.vg, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP);
	nvgText(args.vg, topLabelRightX, 1.f, topLabel, nullptr);

	nvgBeginPath(args.vg);
	nvgMoveTo(args.vg, plotX, responseYForDb(0.f));
	nvgLineTo(args.vg, plotX + usableW, responseYForDb(0.f));
	nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 24));
	nvgStrokeWidth(args.vg, 1.2f);
	nvgStroke(args.vg);
	recordDrawSection(uiDrawBackgroundCount, uiDrawBackgroundNs);

	const NVGcolor expectedPurple = nvgRGB(122, 92, 255);
	const NVGcolor expectedCyan = nvgRGB(28, 204, 217);
	const NVGcolor expectedWhite = nvgRGB(206, 210, 216);
	const float expectedLineWidth = 1.6f;
	nvgShapeAntiAlias(args.vg, 1);
	for (int i = 0; i < kCurvePointCount; i += 2) {
		const float curveDbValue = curveDb[i];
		const float posAmount = clamp01(curveDbValue / 18.f);
		const float negAmount = clamp01(-curveDbValue / 18.f);
		const float emphasis = std::max(posAmount, negAmount);

		NVGcolor tint = expectedWhite;
		if (posAmount > 0.f) {
			tint = mixColor(tint, expectedCyan, clamp01(posAmount * 1.35f));
		}
		if (negAmount > 0.f) {
			tint = mixColor(tint, expectedPurple, clamp01(negAmount * 1.25f));
		}
		tint.a = 0.06f + 0.16f * emphasis;

		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, curveX[i], spectrumBottomY);
		nvgLineTo(args.vg, curveX[i], curveY[i]);
		nvgStrokeColor(args.vg, tint);
		nvgStrokeWidth(args.vg, expectedLineWidth);
		nvgStroke(args.vg);
	}
	recordDrawSection(uiDrawExpectedCount, uiDrawExpectedNs);

		if (hasOverlay) {
			const NVGcolor purple = expectedPurple;
			const NVGcolor cyan = expectedCyan;
			const NVGcolor white = expectedWhite;
			nvgShapeAntiAlias(args.vg, 1);

			for (int i = 0; i < kCurvePointCount - 1; ++i) {
				const float avgDeltaDb = 0.5f * (overlayModuleDb[i] + overlayModuleDb[i + 1]);
				const float avgOutputDbfs = 0.5f * (overlayOutputDbfs[i] + overlayOutputDbfs[i + 1]);
				const float energyAmount = clamp01(rescale(avgOutputDbfs, displayMinDbfs, displayMaxDbfs, 0.f, 1.f));
				if (energyAmount <= 0.005f) {
					continue;
				}

			const float posAmount = clamp01(avgDeltaDb / 18.f);
			const float negAmount = clamp01(-avgDeltaDb / 18.f);
			NVGcolor tint = white;
			if (posAmount > 0.f) {
				tint = mixColor(tint, cyan, clamp01(posAmount * 1.40f));
			}
			if (negAmount > 0.f) {
				tint = mixColor(tint, purple, clamp01(negAmount * 1.25f));
			}
			NVGcolor fill = mixColor(white, tint, 0.55f + 0.45f * energyAmount);
			fill.a = 1.f;
			const float spectrumY0 = spectrumYForDbfs(overlayOutputDbfs[i]);
			const float spectrumY1 = spectrumYForDbfs(overlayOutputDbfs[i + 1]);
			const float seamPad = 0.45f;
			float x0 = curveX[i];
			float x1 = curveX[i + 1];
			if (i > 0) {
				x0 -= seamPad;
			}
			else {
				x0 -= seamPad;
			}
			if (i < kCurvePointCount - 2) {
				x1 += seamPad;
			}
			else {
				x1 += seamPad;
			}

			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, x0, spectrumY0);
			nvgLineTo(args.vg, x1, spectrumY1);
			nvgLineTo(args.vg, x1, spectrumBottomY);
			nvgLineTo(args.vg, x0, spectrumBottomY);
			nvgClosePath(args.vg);
				nvgFillColor(args.vg, fill);
				nvgFill(args.vg);
			}

			NVGcolor moduleLine = mixColor(white, cyan, 0.35f);
			moduleLine.a = 0.95f;
			nvgBeginPath(args.vg);
			for (int i = 0; i < kCurvePointCount; ++i) {
				const float y = responseYForDb(overlayModuleDb[i]);
				if (i == 0) {
					nvgMoveTo(args.vg, curveX[i], y);
				}
				else {
					nvgLineTo(args.vg, curveX[i], y);
				}
			}
			nvgStrokeWidth(args.vg, 1.4f);
			nvgStrokeColor(args.vg, moduleLine);
			nvgStroke(args.vg);

			NVGcolor filterLine = mixColor(cyan, purple, 0.18f);
			filterLine.a = 0.85f;
			nvgBeginPath(args.vg);
			for (int i = 0; i < kCurvePointCount; ++i) {
				const float y = responseYForDb(overlayFilterDb[i]);
				if (i == 0) {
					nvgMoveTo(args.vg, curveX[i], y);
				}
				else {
					nvgLineTo(args.vg, curveX[i], y);
				}
			}
			nvgStrokeWidth(args.vg, 1.0f);
			nvgStrokeColor(args.vg, filterLine);
			nvgStroke(args.vg);
			recordDrawSection(uiDrawOverlayCount, uiDrawOverlayNs);
		}
	else if (perfLoggingActive) {
		perfSectionStart = PerfClock::now();
	}

	nvgBeginPath(args.vg);
	for (int i = 0; i < dedupedCurveDrawPointCount; ++i) {
		const CurveDrawPoint& point = dedupedCurveDrawPoints[i];
		if (i == 0) {
			nvgMoveTo(args.vg, point.x, point.y);
		}
		else {
			nvgLineTo(args.vg, point.x, point.y);
		}
	}
	nvgStrokeColor(args.vg, nvgRGBA(255, 248, 208, 244));
	nvgLineJoin(args.vg, NVG_ROUND);
	nvgLineCap(args.vg, NVG_ROUND);
	nvgStrokeWidth(args.vg, 1.35f);
	nvgStroke(args.vg);
	recordDrawSection(uiDrawCurveCount, uiDrawCurveNs);

	nvgRestore(args.vg);

	struct PeakMarker {
		float x = 0.f;
		float yCurve = 0.f;
		float yMarker = 0.f;
		float hz = 0.f;
		bool visible = false;
		char label[16] = {};
	};
	auto buildMarkerAtFrequency = [&](float targetHz) {
		PeakMarker marker;
		const float safeHz = std::max(targetHz, 1e-6f);
		const float targetX01 = std::log(safeHz / minHz) / std::log(maxHz / minHz);
		const float markerRadius = markerOuterRadius;
		const float markerX = plotX + usableW * targetX01;
		const float markerXMin = plotX + markerRadius + kPeakMarkerEdgePadding;
		const float markerXMax = plotX + usableW - markerRadius - kPeakMarkerEdgePadding;
		if (markerX < markerXMin || markerX > markerXMax) {
			marker.visible = false;
			return marker;
		}
		marker.x = markerX;
		const float curveIndex = targetX01 * float(kCurvePointCount - 1);
		const int i0 = clamp(int(std::floor(curveIndex)), 0, kCurvePointCount - 1);
		const int i1 = std::min(i0 + 1, kCurvePointCount - 1);
		const float t = curveIndex - float(i0);
		marker.yCurve = mixf(curveY[i0], curveY[i1], t);
		const float markerMinY = spectrumTopY + markerRadius + kPeakMarkerEdgePadding;
		const float markerMaxY = spectrumBottomY - markerRadius - kPeakMarkerEdgePadding;
		const float bottomLaneY = markerBottomLaneY;
		marker.yMarker = anchorMarkerToBottomLane ? bottomLaneY : clamp(marker.yCurve, markerMinY, markerMaxY);
		marker.hz = safeHz;
		marker.visible = true;
		formatFrequencyLabel(marker.hz, marker.label, sizeof(marker.label));
		return marker;
	};

	PeakMarker peaks[2];
	peaks[0] = buildMarkerAtFrequency(model.markerFreqA);
	peaks[1] = buildMarkerAtFrequency(model.markerFreqB);

	float labelX[2] = {peaks[0].x, peaks[1].x};
	const float labelMargin = std::max(18.f, w * 0.08f);
	const float minLabelSeparation = std::max(30.f, w * 0.18f);
	const float minX = plotX + labelMargin;
	const float maxX = plotX + usableW - labelMargin;
	if (peaks[0].visible && peaks[1].visible) {
		const int leftIndex = (labelX[0] <= labelX[1]) ? 0 : 1;
		const int rightIndex = 1 - leftIndex;
		float leftX = clamp(labelX[leftIndex], minX, maxX);
		float rightX = clamp(labelX[rightIndex], minX, maxX);

		const float availableSpan = std::max(0.f, maxX - minX);
		const float targetSeparation = std::min(minLabelSeparation, availableSpan);
		float needed = targetSeparation - (rightX - leftX);
		if (needed > 0.f) {
			float moveLeft = std::min(0.5f * needed, leftX - minX);
			float moveRight = std::min(0.5f * needed, maxX - rightX);
			leftX -= moveLeft;
			rightX += moveRight;
			needed -= (moveLeft + moveRight);

			if (needed > 0.f) {
				float extraLeft = std::min(needed, leftX - minX);
				leftX -= extraLeft;
				needed -= extraLeft;
			}
			if (needed > 0.f) {
				float extraRight = std::min(needed, maxX - rightX);
				rightX += extraRight;
				needed -= extraRight;
			}
		}

		labelX[leftIndex] = leftX;
		labelX[rightIndex] = rightX;
	}
	else {
		for (int i = 0; i < 2; ++i) {
			if (peaks[i].visible) {
				labelX[i] = clamp(labelX[i], minX, maxX);
			}
		}
	}

	const float freqLabelFontSize = std::max(7.f, h * 0.055f);
	const float labelTextY = labelBandTop + 0.5f * labelBandHeight;
	const float guideNominalY = labelBandTop + std::min(2.1f, 0.18f * labelBandHeight);
	const float guideMaxY = labelTextY - 0.5f * freqLabelFontSize - 0.6f;
	const float guideYBottom = clamp(guideNominalY, labelBandTop + 0.2f, guideMaxY);
	for (int i = 0; i < 2; ++i) {
		if (!peaks[i].visible) {
			continue;
		}
		const float markerRadius = kPeakMarkerFillRadius;
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, peaks[i].x, peaks[i].yMarker + markerRadius + 0.45f);
		nvgLineTo(args.vg, peaks[i].x, guideYBottom);
		nvgStrokeColor(args.vg, nvgRGBA(252, 236, 176, 170));
		nvgStrokeWidth(args.vg, 1.1f);
		nvgStroke(args.vg);

		nvgBeginPath(args.vg);
		nvgCircle(args.vg, peaks[i].x, peaks[i].yMarker, markerRadius);
		nvgFillColor(args.vg, nvgRGBA(252, 255, 255, 244));
		nvgFill(args.vg);
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, peaks[i].x, peaks[i].yMarker, markerRadius + kPeakMarkerOutlineExtraRadius);
		nvgStrokeColor(args.vg, nvgRGBA(8, 10, 14, 220));
		nvgStrokeWidth(args.vg, kPeakMarkerOutlineStrokeWidth);
		nvgStroke(args.vg);
	}

	nvgFontSize(args.vg, freqLabelFontSize);
	nvgFontFaceId(args.vg, APP->window->uiFont->handle);
	nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
	for (int i = 0; i < 2; ++i) {
		if (!peaks[i].visible) {
			continue;
		}
		nvgFillColor(args.vg, nvgRGBA(4, 6, 9, 240));
		nvgText(args.vg, labelX[i], labelTextY + 0.75f, peaks[i].label, nullptr);
		nvgFillColor(args.vg, nvgRGBA(241, 246, 252, 250));
		nvgText(args.vg, labelX[i], labelTextY, peaks[i].label, nullptr);
	}

	nvgResetScissor(args.vg);
	nvgRestore(args.vg);
	recordDrawSection(uiDrawMarkersCount, uiDrawMarkersNs);
	if (perfLoggingActive) {
		const uint64_t drawNs = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(PerfClock::now() - perfDrawStart).count();
		uiDrawCount++;
		uiDrawNs += drawNs;
		uiDrawMaxNs = std::max(uiDrawMaxNs, drawNs);
	}
}

} // namespace

struct BifurxWidget final : ModuleWidget {
	explicit BifurxWidget(Bifurx* module) {
		setModule(module);
		const std::string panelPath = asset::plugin(pluginInstance, "res/bifurx.svg");
		try {
			setPanel(createPanel(panelPath));
		}
		catch (const std::exception& e) {
			WARN("Bifurx panel load failed (%s), using fallback: %s", panelPath.c_str(), e.what());
			setPanel(createPanel(asset::plugin(pluginInstance, "res/proc.svg")));
			box.size = mm2px(Vec(kDefaultPanelWidthMm, kDefaultPanelHeightMm));
		}

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		auto applyPointOverride = [&](const char* elementId, Vec* outPosMm) {
			Vec pointMm;
			if (panel_svg::loadPointFromSvgMm(panelPath, elementId, &pointMm)) {
				*outPosMm = pointMm;
			}
		};

		math::Rect spectrumRectMm(Vec(1.32f, 75.43f), Vec(68.45f, 21.41f));
		panel_svg::loadRectFromSvgMm(panelPath, "SPECTRUM", &spectrumRectMm);
		widget::FramebufferWidget* spectrumFb = new widget::FramebufferWidget();
		spectrumFb->box.pos = mm2px(spectrumRectMm.pos);
		spectrumFb->box.size = mm2px(spectrumRectMm.size);
		spectrumFb->dirtyOnSubpixelChange = false;
		BifurxSpectrumWidget* spectrum = new BifurxSpectrumWidget();
		spectrum->module = module;
		spectrum->framebuffer = spectrumFb;
		spectrum->box.size = spectrumFb->box.size;
		spectrumFb->addChild(spectrum);
		addChild(spectrumFb);

		math::Rect modeReadoutRectMm(Vec(spectrumRectMm.pos.x, spectrumRectMm.pos.y + spectrumRectMm.size.y + 0.9f), Vec(spectrumRectMm.size.x, 4.2f));
		BifurxModeReadoutWidget* modeReadout = new BifurxModeReadoutWidget();
		modeReadout->module = module;
		modeReadout->box.pos = mm2px(modeReadoutRectMm.pos);
		modeReadout->box.size = mm2px(modeReadoutRectMm.size);
		addChild(modeReadout);

		Vec modePosMm(13.4f, 22.0f);
		Vec levelPosMm(13.4f, 41.0f);
		Vec resoPosMm(13.4f, 60.0f);
		Vec freqPosMm(35.56f, 46.5f);
		Vec titoPosMm(57.7f, 22.0f);
		Vec spanPosMm(57.7f, 41.0f);
		Vec balancePosMm(57.7f, 60.0f);
		Vec fmAmtPosMm(25.3f, 45.0f);
		Vec spanCvAttenPosMm(45.82f, 45.0f);

		Vec inPosMm(7.6f, 112.2f);
		Vec filterCircuitPosMm(7.6f, 102.3f);
		Vec voctPosMm(17.15f, 112.2f);
		Vec fmPosMm(26.7f, 112.2f);
		Vec resoCvPosMm(36.25f, 112.2f);
		Vec balanceCvPosMm(45.8f, 112.2f);
		Vec spanCvPosMm(55.35f, 112.2f);
		Vec outPosMm(64.9f, 112.2f);

		applyPointOverride("MODE_PARAM", &modePosMm);
		applyPointOverride("LEVEL_PARAM", &levelPosMm);
		applyPointOverride("RESO_PARAM", &resoPosMm);
		applyPointOverride("FREQ_PARAM", &freqPosMm);
		applyPointOverride("TITO_PARAM", &titoPosMm);
		applyPointOverride("SPAN_PARAM", &spanPosMm);
		applyPointOverride("BALANCE_PARAM", &balancePosMm);
		applyPointOverride("FM_AMT_PARAM", &fmAmtPosMm);
		applyPointOverride("SPAN_CV_ATTEN_PARAM", &spanCvAttenPosMm);

		applyPointOverride("IN_INPUT", &inPosMm);
		applyPointOverride("FILTER_CIRCUIT_PARAM", &filterCircuitPosMm);
		applyPointOverride("VOCT_INPUT", &voctPosMm);
		applyPointOverride("FM_INPUT", &fmPosMm);
		applyPointOverride("RESO_CV_INPUT", &resoCvPosMm);
		applyPointOverride("BALANCE_CV_INPUT", &balanceCvPosMm);
		applyPointOverride("SPAN_CV_INPUT", &spanCvPosMm);
		applyPointOverride("OUT_OUTPUT", &outPosMm);

			Vec modeLeftPosMm = modePosMm.plus(Vec(-2.5f, 0.f));
			Vec modeRightPosMm = modePosMm.plus(Vec(2.5f, 0.f));
			addParam(createParamCentered<BifurxModeLeftButton>(mm2px(modeLeftPosMm), module, Bifurx::MODE_LEFT_PARAM));
			addParam(createParamCentered<BifurxModeRightButton>(mm2px(modeRightPosMm), module, Bifurx::MODE_RIGHT_PARAM));
			addParam(createParamCentered<RoundBlackKnob>(mm2px(levelPosMm), module, Bifurx::LEVEL_PARAM));
		addParam(createParamCentered<Davies1900hWhiteKnob>(mm2px(freqPosMm), module, Bifurx::FREQ_PARAM));
			addParam(createParamCentered<RoundBlackKnob>(mm2px(resoPosMm), module, Bifurx::RESO_PARAM));
			addParam(createParamCentered<RoundBlackKnob>(mm2px(balancePosMm), module, Bifurx::BALANCE_PARAM));
			addParam(createParamCentered<RoundBlackKnob>(mm2px(spanPosMm), module, Bifurx::SPAN_PARAM));
			addParam(createLightParamCentered<VCVLightSlider<GreenRedLight>>(mm2px(fmAmtPosMm), module, Bifurx::FM_AMT_PARAM, Bifurx::FM_AMT_POS_LIGHT));
			addParam(createLightParamCentered<VCVLightSlider<GreenRedLight>>(mm2px(spanCvAttenPosMm), module, Bifurx::SPAN_CV_ATTEN_PARAM, Bifurx::SPAN_CV_ATTEN_POS_LIGHT));
			addParam(createParamCentered<CKSSThreeHorizontal>(mm2px(titoPosMm), module, Bifurx::TITO_PARAM));

		addParam(createParamCentered<TL1105>(mm2px(filterCircuitPosMm), module, Bifurx::FILTER_CIRCUIT_PARAM));
		const float circuitLedOffsetMm = 2.7f;
		addChild(createLightCentered<SmallLight<YellowLight>>(
			mm2px(filterCircuitPosMm.plus(Vec(-circuitLedOffsetMm, -circuitLedOffsetMm))), module, Bifurx::FILTER_CIRCUIT_TL_LIGHT));
		addChild(createLightCentered<SmallLight<YellowLight>>(
			mm2px(filterCircuitPosMm.plus(Vec(circuitLedOffsetMm, -circuitLedOffsetMm))), module, Bifurx::FILTER_CIRCUIT_TR_LIGHT));
		addChild(createLightCentered<SmallLight<YellowLight>>(
			mm2px(filterCircuitPosMm.plus(Vec(circuitLedOffsetMm, circuitLedOffsetMm))), module, Bifurx::FILTER_CIRCUIT_BR_LIGHT));
		addChild(createLightCentered<SmallLight<YellowLight>>(
			mm2px(filterCircuitPosMm.plus(Vec(-circuitLedOffsetMm, circuitLedOffsetMm))), module, Bifurx::FILTER_CIRCUIT_BL_LIGHT));
		addInput(createInputCentered<PJ301MPort>(mm2px(inPosMm), module, Bifurx::IN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(voctPosMm), module, Bifurx::VOCT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(fmPosMm), module, Bifurx::FM_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(resoCvPosMm), module, Bifurx::RESO_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(balanceCvPosMm), module, Bifurx::BALANCE_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(spanCvPosMm), module, Bifurx::SPAN_CV_INPUT));
			addOutput(createOutputCentered<BananutBlack>(mm2px(outPosMm), module, Bifurx::OUT_OUTPUT));

			}

	void appendContextMenu(Menu* menu) override {
		ModuleWidget::appendContextMenu(menu);

		Bifurx* bifurx = dynamic_cast<Bifurx*>(module);
		if (!bifurx) {
			return;
		}

			menu->addChild(new MenuSeparator());
			menu->addChild(createSubmenuItem("Filter Circuit", "", [=](Menu* submenu) {
				for (int i = 0; i < kBifurxCircuitModeCount; ++i) {
					submenu->addChild(createCheckMenuItem(
						kBifurxCircuitLabels[i], "",
						[=]() { return bifurx->filterCircuitMode == i; },
						[=]() { bifurx->setFilterCircuitMode(i); }
					));
				}
			}));
			menu->addChild(createBoolPtrMenuItem("Dynamic FFT Scale", "", &bifurx->fftScaleDynamic));
			menu->addChild(createBoolPtrMenuItem("Log Curve Debug", "", &bifurx->curveDebugLogging));
			menu->addChild(createBoolPtrMenuItem("Log Performance Debug", "", &bifurx->perfDebugLogging));
		}
	};

Model* modelBifurx = createModel<Bifurx, BifurxWidget>("Bifurx");
