#include "BifurxPreview.hpp"
#include "BifurxInputStage.hpp"
#include "BifurxOutputStage.hpp"
#include "BifurxOversampling.hpp"
#include "UndertowShape.hpp"

#include <array>
#include <algorithm>
#include <cmath>
#include <complex>

namespace bifurx {

bool previewStatesDiffer(const BifurxPreviewState& a, const BifurxPreviewState& b) {
	if (a.mode != b.mode || a.boundary != b.boundary) return true;
	if (std::fabs(a.sampleRate - b.sampleRate) > 0.5f) return true;
	if (std::fabs(a.balance - b.balance) > 1e-3f) return true;
	if (std::fabs(fastLog2(std::max(a.freqA, 1.f)) - fastLog2(std::max(b.freqA, 1.f))) > 1e-3f) return true;
	if (std::fabs(fastLog2(std::max(a.freqB, 1.f)) - fastLog2(std::max(b.freqB, 1.f))) > 1e-3f) return true;
	if (std::fabs(a.qA - b.qA) > 1e-3f) return true;
	if (std::fabs(a.qB - b.qB) > 1e-3f) return true;
	if (std::fabs(a.spanNorm - b.spanNorm) > 1e-4f) return true;
	if (std::fabs(a.resoNorm - b.resoNorm) > 1e-4f) return true;
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
	model.boundary = state.boundary;

	const float lowW = signedWeight(state.balance, false);
	const float highW = signedWeight(state.balance, true);
	const float norm = 2.f / (lowW + highW);
	model.wA = lowW * norm;
	model.wB = highW * norm;
	return model;
}

namespace {
template<typename Boundary> const std::array<float, 128>& boundaryImpulse() {
	static const auto impulse = [] {
		std::array<float, 128> samples{};
		Boundary filter;
		for (size_t i = 0; i < samples.size(); ++i)
			samples[i] = filter.processOutput(filter.processInput(i == 0 ? 1.f : 0.f, 0.5f), 0.5f, false);
		return samples;
	}();
	return impulse;
}
std::complex<float> linearBoundaryResponse(int boundary, double omega) {
	if (boundary == 0) return {1.f, 0.f};
	const auto& impulse = boundary == 3 ? boundaryImpulse<BifurxIirOversampling2x>()
		: boundary == 1 ? boundaryImpulse<BifurxLegacyOversampling2x>() : boundaryImpulse<BifurxNonlinearOversampling2x>();
	const std::complex<double> z = std::exp(std::complex<double>(0., -omega));
	std::complex<double> response(0., 0.), power(1., 0.);
	const int extent = boundary == 1 ? 32 : 128;
	for (int i = 0; i < extent; ++i) { response += double(impulse[i]) * power; power *= z; }
	return std::complex<float>(response);
}
}

std::complex<float> previewModelResponse(const BifurxPreviewModel& model, float hz) {
	if (isBifurxDisplayOnlyMode(model.mode)) return {1.f, 0.f};
	const double omega = 6.28318530717958647692 * clamp(hz, 4.f, 0.49f * model.sampleRate) / std::max(model.sampleRate, 1.f);
	const std::complex<double> z1 = std::exp(std::complex<double>(0.f, -omega));
	const std::complex<double> z2 = z1 * z1;

	std::complex<float> lpA(model.lowA.response(z1, z2));
	std::complex<float> bpA(model.bandA.response(z1, z2));
	std::complex<float> hpA(model.highA.response(z1, z2));
	std::complex<float> lpB(model.lowB.response(z1, z2));
	std::complex<float> bpB(model.bandB.response(z1, z2));
	std::complex<float> hpB(model.highB.response(z1, z2));
	const std::complex<float> ntA = lpA + hpA, ntB = lpB + hpB, cascadeLp = lpB * lpA, cascadeNotch = ntB * ntA, cascadeNotchToLow = lpB * ntA, cascadeHpToLp = lpB * hpA, cascadeHighToNotch = ntB * hpA, cascadeHpToHp = hpB * hpA;
	return linearBoundaryResponse(model.boundary, omega) * combineModeResponse<std::complex<float>>(model.mode, lpA, bpA, hpA, ntA, lpB, bpB, hpB, ntB, cascadeLp, cascadeNotch, cascadeNotchToLow, cascadeHpToLp, cascadeHighToNotch, cascadeHpToHp, model.wA, model.wB);
}

#if defined(BIFURX_DISPLAY_TEST_HOOKS)
std::complex<float> linearBoundaryResponseForTest(int boundary, double omega) {
	return linearBoundaryResponse(boundary, omega);
}
#endif

float previewModelResponseDb(const BifurxPreviewModel& model, float hz) {
	const float mag = std::abs(previewModelResponse(model, hz));
	return 20.f * std::log10(std::max(mag, 1e-5f));
}

float previewProbeStimulusSample(const BifurxPreviewState& state, int sampleIndex) {
	if (sampleIndex < 0) return 0.f;
	const float sampleRate = std::max(state.sampleRate, 1.f);
	const float phase = 261.63f * float(sampleIndex) / sampleRate;
	const float phase01 = phase - std::floor(phase);
	return 5.f * undertow_shape::thresholdFold(phase01, 0.5f, false, 0.5f, false);
}

namespace {

float undertowPreviewAtan(float x) {
	const float ax = std::fabs(x);
	if (ax <= 1.f) {
		return x * (0.78539816339f + 0.273f * (1.f - ax));
	}
	const float inv = 1.f / ax;
	const float t = inv * (0.78539816339f + 0.273f * (1.f - inv));
	return (x >= 0.f) ? (1.57079632679f - t) : (-1.57079632679f + t);
}

float undertowPreviewAnalogCharacter(float x, float env) {
	const float drive = 1.02f + 0.18f * clamp(env, 0.f, 1.f);
	const float signDrive = x >= 0.f ? (drive + 0.02f) : (drive - 0.02f);
	const float norm = std::max(undertowPreviewAtan(signDrive), 1e-6f);
	return undertowPreviewAtan(x * signDrive) / norm;
}

void generateUndertowBrowserPreview(float* buffer, int sampleCount, float sampleRate) {
	if (!buffer || sampleCount <= 0) return;
	const float sr = std::max(sampleRate, 1.f);
	const float sampleTime = 1.f / sr;
	constexpr float baseFrequencyHz = 261.63f;
	constexpr float linearFmAmount = 0.5f;
	constexpr float shape = 0.5f;
	constexpr float edgeHardness = 0.5f;
	float phase = 0.f;
	float linearFmHpState = 0.f;
	float characterEnv = 0.f;
	bool subHigh = false;
	const float hpCoeff = clamp(1.f - 2.f * kPi * 4.9f * sampleTime, 0.f, 1.f);
	const float attackCoeff = clamp(sampleTime / (0.002f + sampleTime), 0.f, 1.f);
	const float releaseCoeff = clamp(sampleTime / (0.050f + sampleTime), 0.f, 1.f);

	for (int i = 0; i < sampleCount; ++i) {
		// This mirrors the inspected patch: Undertow Sub self-patches its linear-FM
		// input at 50%, while Morph (also 50%) feeds Bifurx.
		const float subVoltage = subHigh ? 5.f : -5.f;
		const float linearFm = subVoltage - linearFmHpState;
		linearFmHpState = subVoltage - hpCoeff * linearFm;
		const float linearBus = linearFm * linearFmAmount * 0.10f;
		const float frequency = clamp(baseFrequencyHz + baseFrequencyHz * linearBus, 8.f, 20000.f);
		phase += frequency * sampleTime;
		if (phase >= 1.f) {
			phase -= std::floor(phase);
			subHigh = !subHigh;
		}

		const float triangle = 4.f * std::fabs(phase - 0.5f) - 1.f;
		const float sine = undertow_shape::triToSine(triangle);
		const float shaped = undertow_shape::thresholdFold(phase, shape, false, edgeHardness, false);
		const float envTarget = 0.5f * (std::fabs(sine) + std::fabs(shaped));
		const float envCoeff = envTarget > characterEnv ? attackCoeff : releaseCoeff;
		characterEnv += (envTarget - characterEnv) * envCoeff;
		buffer[i] = clamp(5.f * undertowPreviewAnalogCharacter(shaped, characterEnv), -5.f, 5.f);
	}
}

} // namespace

SvfOutputs processProbeStage(BifurxProbeEngineState& state, int stageIndex, float input, float sampleRate, float cutoff, float damping, float drive, float resoNorm, bool highResonanceSelfOscEnabled) {
	TptSvf& core = (stageIndex == 0) ? state.svfA : state.svfB;
	return processCharacterStage(core, stageIndex, input, sampleRate, cutoff, damping, drive, resoNorm, highResonanceSelfOscEnabled, nullptr);
}

void simulatePreviewProbeResponse(const BifurxPreviewState& state, float* inputBuffer, float* outputBuffer, int sampleCount) {
	if (!inputBuffer || !outputBuffer || sampleCount <= 0) return;
	BifurxProbeEngineState engine;
	const float sampleRate = std::max(state.sampleRate, 1.f), freqA = clamp(state.freqA, kFreqMinHz, 0.46f * sampleRate), freqB = clamp(state.freqB, kFreqMinHz, 0.46f * sampleRate), dampingA = clamp(1.f / std::max(state.qA, 0.05f), 0.02f, 2.2f), dampingB = clamp(1.f / std::max(state.qB, 0.05f), 0.02f, 2.2f), lowW = signedWeight(state.balance, false), highW = signedWeight(state.balance, true), norm = 2.f / (lowW + highW), wA = lowW * norm, wB = highW * norm, drive = levelDriveGain(kPreviewProbeLevelKnob);
	const int mode = clamp(state.mode, 0, kBifurxModeCount - 1);
	generateUndertowBrowserPreview(inputBuffer, sampleCount, sampleRate);
	for (int i = 0; i < sampleCount; ++i) {
		const float rawIn = inputBuffer[i], excitation = applyLevelInputStage(rawIn, kPreviewProbeLevelKnob);
		const SvfOutputs a = processProbeStage(engine, 0, excitation, sampleRate, freqA, dampingA, drive, state.resoNorm, true);
		SvfOutputs b; float modeOut = 0.f;
		switch (mode) {
			case 0: b = processProbeStage(engine, 1, a.lp, sampleRate, freqB, dampingB, drive, state.resoNorm, true); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, b.lp, 0.f, 0.f, 0.f, 0.f, 0.f, wA, wB); break;
			case 1:
			case 4:
			case 5:
			case 8: b = processProbeStage(engine, 1, excitation, sampleRate, freqB, dampingB, drive, state.resoNorm, true); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, wA, wB); break;
			case 2: b = processProbeStage(engine, 1, a.notch, sampleRate, freqB, dampingB, drive, state.resoNorm, true); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, b.lp, 0.f, 0.f, 0.f, wA, wB); break;
			case 3: b = processProbeStage(engine, 1, a.notch, sampleRate, freqB, dampingB, drive, state.resoNorm, true); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, b.notch, 0.f, 0.f, 0.f, 0.f, wA, wB); break;
			case 6: b = processProbeStage(engine, 1, a.hp, sampleRate, freqB, dampingB, drive, state.resoNorm, true); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, b.lp, 0.f, 0.f, wA, wB); break;
			case 7: b = processProbeStage(engine, 1, a.hp, sampleRate, freqB, dampingB, drive, state.resoNorm, true); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, 0.f, b.notch, 0.f, wA, wB); break;
			case 9: b = processProbeStage(engine, 1, a.hp, sampleRate, freqB, dampingB, drive, state.resoNorm, true); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, 0.f, 0.f, b.hp, wA, wB); break;
			default: b = processProbeStage(engine, 1, a.lp, sampleRate, freqB, dampingB, drive, state.resoNorm, true); modeOut = combineModeResponse<float>(0, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, b.lp, 0.f, 0.f, 0.f, 0.f, 0.f, wA, wB); break;
		}
		inputBuffer[i] = excitation; outputBuffer[i] = applyLevelOutputStage(modeOut, kPreviewProbeLevelKnob);
	}
}

} // namespace bifurx
