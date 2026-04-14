#include "plugin.hpp"
#include "PanelSvgUtils.hpp"

#include <algorithm>
#include <atomic>
#include <cmath>
#include <complex>
#include <cstdint>
#include <cstring>
#include <exception>

struct Bifurx;

namespace {

constexpr float kDefaultPanelWidthMm = 71.12f;
constexpr float kDefaultPanelHeightMm = 128.5f;
constexpr float kPi = 3.14159265358979323846f;
constexpr int kCurvePointCount = 97;
constexpr int kFftSize = 2048;
constexpr int kFftBinCount = kFftSize / 2 + 1;
constexpr int kFftHopSize = kFftSize / 2;
constexpr int kGuideCount = 4;
const float kGuideFreqs[kGuideCount] = {10.f, 100.f, 1000.f, 10000.f};

float clamp01(float v) {
	return clamp(v, 0.f, 1.f);
}

float shapedSpan(float value) {
	return std::pow(clamp01(value), 1.65f);
}

float levelDriveGain(float knob) {
	const float x = clamp01(knob);
	return 0.05f + 0.9f * x + 5.f * x * x * x;
}

float softClip(float x) {
	return std::tanh(x);
}

float mixf(float a, float b, float t) {
	return a + (b - a) * t;
}

float clampDb(float db) {
	return clamp(db, -36.f, 18.f);
}

float responseDbToY(float db, float curveBaseY, float usableH) {
	return curveBaseY - rescale(clampDb(db), -36.f, 18.f, 0.f, usableH * 0.62f);
}

float logPosition(float hz, float minHz, float maxHz) {
	const float safeHz = clamp(hz, minHz, maxHz);
	return std::log(safeHz / minHz) / std::log(maxHz / minHz);
}

float logFrequencyAt(float x01, float minHz, float maxHz) {
	return minHz * std::pow(maxHz / minHz, clamp01(x01));
}

float expoMap(float norm, float minValue, float maxValue) {
	return minValue * std::pow(maxValue / minValue, clamp01(norm));
}

float resoToDamping(float resoNorm) {
	const float r = clamp01(resoNorm);
	return 2.f - 1.98f * std::pow(r, 1.35f);
}

float signedWeight(float balance, bool upperPeak) {
	const float sign = upperPeak ? 1.f : -1.f;
	return std::exp(0.7f * sign * clamp(balance, -1.f, 1.f));
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

struct SvfOutputs {
	float lp = 0.f;
	float bp = 0.f;
	float hp = 0.f;
	float notch = 0.f;
};

struct TptSvf {
	float ic1eq = 0.f;
	float ic2eq = 0.f;

	SvfOutputs process(float input, float sampleRate, float cutoff, float damping) {
		const float sr = std::max(sampleRate, 1.f);
		const float limitedCutoff = clamp(cutoff, 4.f, 0.46f * sr);
		const float g = std::tan(kPi * limitedCutoff / sr);
		const float k = clamp(damping, 0.02f, 2.2f);
		const float a1 = 1.f / (1.f + g * (g + k));
		const float v1 = a1 * (ic1eq + g * (input - ic2eq));
		const float v2 = ic2eq + g * v1;

		ic1eq = 2.f * v1 - ic1eq;
		ic2eq = 2.f * v2 - ic2eq;

		SvfOutputs out;
		out.bp = v1;
		out.lp = v2;
		out.hp = input - k * v1 - v2;
		out.notch = out.lp + out.hp;
		return out;
	}
};

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

struct BifurxPreviewState {
	float sampleRate = 44100.f;
	float freqA = 440.f;
	float freqB = 440.f;
	float qA = 1.f;
	float qB = 1.f;
	float balance = 0.f;
	int mode = 0;
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
	float sampleRate = 44100.f;
	float wA = 1.f;
	float wB = 1.f;
	int mode = 0;
};

struct BifurxAnalysisFrame {
	alignas(16) float input[kFftSize];
	alignas(16) float output[kFftSize];
};

bool previewStatesDiffer(const BifurxPreviewState& a, const BifurxPreviewState& b) {
	if (a.mode != b.mode) {
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
	model.lowA = makeDisplayBiquad(state.sampleRate, state.freqA, state.qA, 0);
	model.bandA = makeDisplayBiquad(state.sampleRate, state.freqA, state.qA, 1);
	model.highA = makeDisplayBiquad(state.sampleRate, state.freqA, state.qA, 2);
	model.notchA = makeDisplayBiquad(state.sampleRate, state.freqA, state.qA, 3);
	model.lowB = makeDisplayBiquad(state.sampleRate, state.freqB, state.qB, 0);
	model.bandB = makeDisplayBiquad(state.sampleRate, state.freqB, state.qB, 1);
	model.highB = makeDisplayBiquad(state.sampleRate, state.freqB, state.qB, 2);
	model.notchB = makeDisplayBiquad(state.sampleRate, state.freqB, state.qB, 3);
	model.sampleRate = state.sampleRate;
	model.mode = state.mode;

	const float lowW = signedWeight(state.balance, false);
	const float highW = signedWeight(state.balance, true);
	const float norm = 2.f / (lowW + highW);
	model.wA = lowW * norm;
	model.wB = highW * norm;
	return model;
}

std::complex<float> previewModelResponse(const BifurxPreviewModel& model, float hz) {
	const float omega = 2.f * kPi * clamp(hz, 4.f, 0.49f * model.sampleRate) / std::max(model.sampleRate, 1.f);
	const std::complex<float> lpA = model.lowA.response(omega);
	const std::complex<float> bpA = model.bandA.response(omega);
	const std::complex<float> hpA = model.highA.response(omega);
	const std::complex<float> ntA = model.notchA.response(omega);
	const std::complex<float> lpB = model.lowB.response(omega);
	const std::complex<float> bpB = model.bandB.response(omega);
	const std::complex<float> hpB = model.highB.response(omega);
	const std::complex<float> ntB = model.notchB.response(omega);

	switch (model.mode) {
		case 0: return lpB * lpA;
		case 1: return 0.95f * model.wA * lpA + 1.05f * model.wB * bpB - 0.12f * (bpA + bpB);
		case 2: return 1.05f * model.wB * lpB - 0.55f * model.wA * bpA;
		case 3: return ntB * ntA;
		case 4: return 0.95f * model.wA * lpA + 0.95f * model.wB * hpB;
		case 5: return 1.15f * (model.wA * bpA + model.wB * bpB);
		case 6: return lpB * hpA;
		case 7: return 1.05f * model.wA * hpA - 0.55f * model.wB * bpB;
		case 8: return 1.12f * model.wA * bpA + 0.92f * model.wB * hpB - 0.10f * (hpA + bpB);
		case 9: return hpB * hpA;
		default: return std::complex<float>(1.f, 0.f);
	}
}

struct BifurxSpectrumWidget final : Widget {
	Bifurx* module = nullptr;
	widget::FramebufferWidget* framebuffer = nullptr;
	dsp::RealFFT fft;
	alignas(16) float window[kFftSize];
	alignas(16) float fftInputTime[kFftSize];
	alignas(16) float fftOutputTime[kFftSize];
	alignas(16) float fftInputFreq[2 * kFftSize];
	alignas(16) float fftOutputFreq[2 * kFftSize];
	float curveDb[kCurvePointCount];
	float overlayDb[kCurvePointCount];
	float overlayInputDb[kCurvePointCount];
	float curveX[kCurvePointCount];
	float curveY[kCurvePointCount];
	float bottomY = 0.f;
	BifurxPreviewState previewState;
	bool hasPreview = false;
	bool hasOverlay = false;
	uint32_t lastPreviewSeq = 0;
	uint32_t lastAnalysisSeq = 0;

	BifurxSpectrumWidget();
	void step() override;
	void updateCurveCache();
	void updateOverlayCache(const BifurxAnalysisFrame& frame);

	void draw(const DrawArgs& args) override;
};

struct BananutBlack : app::SvgPort {
	BananutBlack() {
		setSvg(Svg::load(asset::plugin(pluginInstance, "res/BananutBlack.svg")));
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
		FM_AMT_LIGHT,
		SPAN_CV_ATTEN_LIGHT,
		LIGHTS_LEN
	};

	TptSvf coreA;
	TptSvf coreB;
	dsp::ClockDivider previewPublishDivider;
	BifurxPreviewState lastPreviewState;
	bool hasLastPreviewState = false;
	BifurxPreviewState previewStates[2];
	std::atomic<int> previewPublishedIndex{0};
	std::atomic<uint32_t> previewPublishSeq{0};
	float analysisInputHistory[kFftSize] = {};
	float analysisOutputHistory[kFftSize] = {};
	int analysisWritePos = 0;
	int analysisFilled = 0;
	int analysisHopCounter = 0;
	BifurxAnalysisFrame analysisFrames[2];
	std::atomic<int> analysisPublishedIndex{0};
	std::atomic<uint32_t> analysisPublishSeq{0};

	Bifurx() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		configSwitch(MODE_PARAM, 0.f, 9.f, 0.f,
			"Mode",
			{"LL", "LB", "NL", "NN", "LH", "BB", "HH", "HN", "BH", "HL"});
		configParam(LEVEL_PARAM, 0.f, 1.f, 0.5f, "Level");
		configParam(FREQ_PARAM, 0.f, 1.f, 0.5f, "Frequency");
		configParam(RESO_PARAM, 0.f, 1.f, 0.35f, "Resonance");
		configParam(BALANCE_PARAM, -1.f, 1.f, 0.f, "Balance");
		configParam(SPAN_PARAM, 0.f, 1.f, 0.f, "Span");
		configParam(FM_AMT_PARAM, -1.f, 1.f, 0.f, "FM amount");
		configParam(SPAN_CV_ATTEN_PARAM, -1.f, 1.f, 0.f, "Span CV attenuator");
		configSwitch(TITO_PARAM, 0.f, 2.f, 1.f, "TITO", {"XM", "Clean", "SM"});

		configInput(IN_INPUT, "Signal In");
		configInput(VOCT_INPUT, "V/Oct");
		configInput(FM_INPUT, "FM");
		configInput(RESO_CV_INPUT, "Resonance CV");
		configInput(BALANCE_CV_INPUT, "Balance CV");
		configInput(SPAN_CV_INPUT, "Span CV");
		configOutput(OUT_OUTPUT, "Signal Out");

		paramQuantities[MODE_PARAM]->snapEnabled = true;
		paramQuantities[TITO_PARAM]->snapEnabled = true;
		previewPublishDivider.setDivision(128);
	}

	void publishPreviewState(const BifurxPreviewState& state) {
		int writeIndex = 1 - previewPublishedIndex.load(std::memory_order_relaxed);
		previewStates[writeIndex] = state;
		previewPublishedIndex.store(writeIndex, std::memory_order_release);
		previewPublishSeq.fetch_add(1, std::memory_order_release);
		lastPreviewState = state;
		hasLastPreviewState = true;
	}

	void publishAnalysisFrame() {
		int writeIndex = 1 - analysisPublishedIndex.load(std::memory_order_relaxed);
		for (int i = 0; i < kFftSize; ++i) {
			int sourceIndex = (analysisWritePos + i) % kFftSize;
			analysisFrames[writeIndex].input[i] = analysisInputHistory[sourceIndex];
			analysisFrames[writeIndex].output[i] = analysisOutputHistory[sourceIndex];
		}
		analysisPublishedIndex.store(writeIndex, std::memory_order_release);
		analysisPublishSeq.fetch_add(1, std::memory_order_release);
	}

	void pushAnalysisSample(float inputSample, float outputSample) {
		analysisInputHistory[analysisWritePos] = inputSample;
		analysisOutputHistory[analysisWritePos] = outputSample;
		analysisWritePos = (analysisWritePos + 1) % kFftSize;
		if (analysisFilled < kFftSize) {
			analysisFilled++;
		}
		if (analysisFilled == kFftSize) {
			analysisHopCounter++;
			if (analysisPublishSeq.load(std::memory_order_relaxed) == 0 || analysisHopCounter >= kFftHopSize) {
				analysisHopCounter = 0;
				publishAnalysisFrame();
			}
		}
	}

	void process(const ProcessArgs& args) override {
		const float in = inputs[IN_INPUT].getVoltage();
		const float level = params[LEVEL_PARAM].getValue();
		const float drive = levelDriveGain(level);
		const float voct = inputs[VOCT_INPUT].getVoltage();
		const float fmAmt = params[FM_AMT_PARAM].getValue();
		const float fm = clamp(inputs[FM_INPUT].getVoltage(), -10.f, 10.f) * fmAmt;
		const float resoCv = clamp(inputs[RESO_CV_INPUT].getVoltage(), 0.f, 8.f) / 8.f;
		const float balanceCv = clamp(inputs[BALANCE_CV_INPUT].getVoltage(), -5.f, 5.f) / 5.f;
		const float spanCv = clamp(inputs[SPAN_CV_INPUT].getVoltage(), -10.f, 10.f) / 5.f;

		const int mode = int(std::round(params[MODE_PARAM].getValue()));
		const int tito = int(std::round(params[TITO_PARAM].getValue()));
		const float spanNorm = clamp(
			params[SPAN_PARAM].getValue() + 0.5f * params[SPAN_CV_ATTEN_PARAM].getValue() * spanCv,
			0.f, 1.f);
		const float spanOct = 8.f * shapedSpan(spanNorm);
		const float balance = clamp(params[BALANCE_PARAM].getValue() + balanceCv, -1.f, 1.f);
		const float resoNorm = clamp(params[RESO_PARAM].getValue() + resoCv, 0.f, 1.f);
		const float centerHz = expoMap(params[FREQ_PARAM].getValue(), 4.f, 28000.f) * std::pow(2.f, voct + fm);
		const float freqA0 = centerHz * std::pow(2.f, -0.5f * spanOct);
		const float freqB0 = centerHz * std::pow(2.f, 0.5f * spanOct);
		const float baseDamping = resoToDamping(resoNorm);
		const float dampingA = clamp(baseDamping * std::exp(0.55f * balance), 0.02f, 2.2f);
		const float dampingB = clamp(baseDamping * std::exp(-0.55f * balance), 0.02f, 2.2f);
		const float lowW = signedWeight(balance, false);
		const float highW = signedWeight(balance, true);
		const float norm = 2.f / (lowW + highW);
		const float wA = lowW * norm;
		const float wB = highW * norm;
		const float couplingDepth = (0.02f + 0.25f * resoNorm * resoNorm) * (tito == 1 ? 0.f : 1.f);
		const float drivenIn = 5.f * softClip(0.2f * in * drive);
		const float excitation = drivenIn + (resoNorm > 0.985f ? 1e-6f : 0.f);

		auto modulatedCutoffs = [&](float modA, float modB) {
			const float cutA = freqA0 * std::pow(2.f, clamp(modA, -2.5f, 2.5f));
			const float cutB = freqB0 * std::pow(2.f, clamp(modB, -2.5f, 2.5f));
			return std::pair<float, float>(cutA, cutB);
		};

		float modA = 0.f;
		float modB = 0.f;
		if (tito == 2) {
			modA = couplingDepth * coreA.ic1eq / 5.f;
			modB = couplingDepth * coreB.ic1eq / 5.f;
		}
		else if (tito == 0) {
			modA = couplingDepth * coreB.ic1eq / 5.f;
			modB = couplingDepth * coreA.ic1eq / 5.f;
		}

		const std::pair<float, float> cutoffs = modulatedCutoffs(modA, modB);
		const float cutoffA = cutoffs.first;
		const float cutoffB = cutoffs.second;
		float modeOut = 0.f;

		switch (mode) {
			case 0: {
				const SvfOutputs a = coreA.process(excitation, args.sampleRate, cutoffA, dampingA);
				const SvfOutputs b = coreB.process(a.lp, args.sampleRate, cutoffB, dampingB);
				modeOut = b.lp;
			} break;
			case 1: {
				const SvfOutputs a = coreA.process(excitation, args.sampleRate, cutoffA, dampingA);
				const SvfOutputs b = coreB.process(excitation, args.sampleRate, cutoffB, dampingB);
				modeOut = 0.95f * wA * a.lp + 1.05f * wB * b.bp - 0.12f * (a.bp + b.bp);
			} break;
			case 2: {
				const SvfOutputs a = coreA.process(excitation, args.sampleRate, cutoffA, dampingA);
				const SvfOutputs b = coreB.process(excitation, args.sampleRate, cutoffB, dampingB);
				modeOut = 1.05f * wB * b.lp - 0.55f * wA * a.bp;
			} break;
			case 3: {
				const SvfOutputs a = coreA.process(excitation, args.sampleRate, cutoffA, dampingA);
				const SvfOutputs b = coreB.process(a.notch, args.sampleRate, cutoffB, dampingB);
				modeOut = b.notch;
			} break;
			case 4: {
				const SvfOutputs a = coreA.process(excitation, args.sampleRate, cutoffA, dampingA);
				const SvfOutputs b = coreB.process(excitation, args.sampleRate, cutoffB, dampingB);
				modeOut = 0.95f * wA * a.lp + 0.95f * wB * b.hp;
			} break;
			case 5: {
				const SvfOutputs a = coreA.process(excitation, args.sampleRate, cutoffA, dampingA);
				const SvfOutputs b = coreB.process(excitation, args.sampleRate, cutoffB, dampingB);
				modeOut = 1.15f * (wA * a.bp + wB * b.bp);
			} break;
			case 6: {
				const SvfOutputs a = coreA.process(excitation, args.sampleRate, cutoffA, dampingA);
				const SvfOutputs b = coreB.process(a.hp, args.sampleRate, cutoffB, dampingB);
				modeOut = b.lp;
			} break;
			case 7: {
				const SvfOutputs a = coreA.process(excitation, args.sampleRate, cutoffA, dampingA);
				const SvfOutputs b = coreB.process(excitation, args.sampleRate, cutoffB, dampingB);
				modeOut = 1.05f * wA * a.hp - 0.55f * wB * b.bp;
			} break;
			case 8: {
				const SvfOutputs a = coreA.process(excitation, args.sampleRate, cutoffA, dampingA);
				const SvfOutputs b = coreB.process(excitation, args.sampleRate, cutoffB, dampingB);
				modeOut = 1.12f * wA * a.bp + 0.92f * wB * b.hp - 0.10f * (a.hp + b.bp);
			} break;
			case 9:
			default: {
				const SvfOutputs a = coreA.process(excitation, args.sampleRate, cutoffA, dampingA);
				const SvfOutputs b = coreB.process(a.hp, args.sampleRate, cutoffB, dampingB);
				modeOut = b.hp;
			} break;
		}

		const float out = 5.5f * softClip(modeOut / 5.5f);
		outputs[OUT_OUTPUT].setChannels(1);
		outputs[OUT_OUTPUT].setVoltage(out);

		BifurxPreviewState previewState;
		previewState.sampleRate = args.sampleRate;
		previewState.freqA = cutoffA;
		previewState.freqB = cutoffB;
		previewState.qA = 1.f / std::max(dampingA, 0.05f);
		previewState.qB = 1.f / std::max(dampingB, 0.05f);
		previewState.mode = mode;
		previewState.balance = balance;
		if (!hasLastPreviewState || (previewPublishDivider.process() && previewStatesDiffer(previewState, lastPreviewState))) {
			publishPreviewState(previewState);
		}

		pushAnalysisSample(drivenIn, out);

		lights[FM_AMT_LIGHT].setBrightness(std::fabs(params[FM_AMT_PARAM].getValue()));
		lights[SPAN_CV_ATTEN_LIGHT].setBrightness(std::fabs(params[SPAN_CV_ATTEN_PARAM].getValue()));
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
		curveDb[i] = -36.f;
		overlayDb[i] = 0.f;
		overlayInputDb[i] = -120.f;
	}
}

void BifurxSpectrumWidget::step() {
	Widget::step();
	if (!module) {
		return;
	}

	bool dirty = false;

	const uint32_t previewSeq = module->previewPublishSeq.load(std::memory_order_acquire);
	if (previewSeq != lastPreviewSeq) {
		const int index = module->previewPublishedIndex.load(std::memory_order_acquire);
		previewState = module->previewStates[index];
		hasPreview = true;
		lastPreviewSeq = previewSeq;
		updateCurveCache();
		dirty = true;
	}

	const uint32_t analysisSeq = module->analysisPublishSeq.load(std::memory_order_acquire);
	if (analysisSeq != lastAnalysisSeq) {
		const int index = module->analysisPublishedIndex.load(std::memory_order_acquire);
		updateOverlayCache(module->analysisFrames[index]);
		hasOverlay = true;
		lastAnalysisSeq = analysisSeq;
		dirty = true;
	}

	if (dirty && framebuffer) {
		framebuffer->setDirty();
	}
}

void BifurxSpectrumWidget::updateCurveCache() {
	if (!hasPreview) {
		return;
	}

	const BifurxPreviewModel model = makePreviewModel(previewState);
	const float minHz = 10.f;
	const float maxHz = std::min(20000.f, 0.46f * previewState.sampleRate);

	for (int i = 0; i < kCurvePointCount; ++i) {
		const float x01 = float(i) / float(kCurvePointCount - 1);
		const float hz = logFrequencyAt(x01, minHz, maxHz);
		const float mag = std::abs(previewModelResponse(model, hz));
		curveDb[i] = clampDb(20.f * std::log10(std::max(mag, 1e-5f)));
	}
}

void BifurxSpectrumWidget::updateOverlayCache(const BifurxAnalysisFrame& frame) {
	if (!hasPreview) {
		return;
	}

	const float sampleRate = std::max(previewState.sampleRate, 1.f);
	const float minHz = 10.f;
	const float maxHz = std::min(20000.f, 0.46f * sampleRate);
	const float amplitudeScale = 4.f / float(kFftSize);
	const float smoothing = hasOverlay ? 0.35f : 1.f;

	for (int i = 0; i < kFftSize; ++i) {
		fftInputTime[i] = frame.input[i] * window[i];
		fftOutputTime[i] = frame.output[i] * window[i];
	}

	fft.rfft(fftInputTime, fftInputFreq);
	fft.rfft(fftOutputTime, fftOutputFreq);

	float binDeltaDb[kFftBinCount];
	float binInputDb[kFftBinCount];
	for (int bin = 0; bin < kFftBinCount; ++bin) {
		const float inputAmp = amplitudeScale * orderedSpectrumMagnitude(fftInputFreq, bin);
		const float outputAmp = amplitudeScale * orderedSpectrumMagnitude(fftOutputFreq, bin);
		binDeltaDb[bin] = clamp(20.f * std::log10((outputAmp + 1e-6f) / (inputAmp + 1e-6f)), -24.f, 24.f);
		binInputDb[bin] = 20.f * std::log10(inputAmp + 1e-6f);
	}

	for (int i = 0; i < kCurvePointCount; ++i) {
		const float x01 = float(i) / float(kCurvePointCount - 1);
		const float hz = logFrequencyAt(x01, minHz, maxHz);
		const float binPosition = clamp(hz * float(kFftSize) / sampleRate, 0.f, float(kFftSize / 2));
		const int binA = int(std::floor(binPosition));
		const int binB = std::min(binA + 1, kFftSize / 2);
		const float frac = binPosition - float(binA);
		const float targetDeltaDb = mixf(binDeltaDb[binA], binDeltaDb[binB], frac);
		const float targetInputDb = mixf(binInputDb[binA], binInputDb[binB], frac);

		overlayDb[i] = mixf(overlayDb[i], targetDeltaDb, smoothing);
		overlayInputDb[i] = mixf(overlayInputDb[i], targetInputDb, smoothing);
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

	const float padX = std::max(8.f, w * 0.035f);
	const float padY = std::max(8.f, h * 0.10f);
	const float usableW = std::max(1.f, w - 2.f * padX);
	const float usableH = std::max(1.f, h - 2.f * padY);
	const float curveBaseY = padY + usableH * 0.70f;
	const float minHz = 10.f;
	const float maxHz = std::min(20000.f, 0.46f * previewState.sampleRate);
	bottomY = h - padY * 0.4f;

	for (int i = 0; i < kCurvePointCount; ++i) {
		const float x01 = float(i) / float(kCurvePointCount - 1);
		curveX[i] = padX + usableW * x01;
		curveY[i] = responseDbToY(curveDb[i], curveBaseY, usableH);
	}

	nvgSave(args.vg);
	nvgScissor(args.vg, 0.f, 0.f, w, h);

	nvgBeginPath(args.vg);
	nvgRect(args.vg, 0.f, 0.f, w, h);
	nvgFillColor(args.vg, nvgRGBA(7, 10, 14, 26));
	nvgFill(args.vg);

	for (int i = 0; i < kGuideCount; ++i) {
		if (kGuideFreqs[i] >= maxHz) {
			continue;
		}
		const float guideX = padX + usableW * logPosition(kGuideFreqs[i], minHz, maxHz);
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, guideX, padY * 0.85f);
		nvgLineTo(args.vg, guideX, bottomY);
		nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 18));
		nvgStrokeWidth(args.vg, 1.f);
		nvgStroke(args.vg);
	}

	nvgBeginPath(args.vg);
	nvgMoveTo(args.vg, padX, curveBaseY);
	nvgLineTo(args.vg, padX + usableW, curveBaseY);
	nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 24));
	nvgStrokeWidth(args.vg, 1.2f);
	nvgStroke(args.vg);

	if (hasOverlay) {
		const NVGcolor purple = nvgRGB(132, 72, 255);
		const NVGcolor cyan = nvgRGB(48, 222, 255);
		const NVGcolor white = nvgRGB(255, 255, 255);

		for (int i = 0; i < kCurvePointCount - 1; ++i) {
			const float avgDeltaDb = 0.5f * (overlayDb[i] + overlayDb[i + 1]);
			const float avgInputDb = 0.5f * (overlayInputDb[i] + overlayInputDb[i + 1]);
			const float effectAmount = clamp01(std::fabs(avgDeltaDb) / 18.f);
			const float energyAmount = clamp01(rescale(avgInputDb, -48.f, 6.f, 0.f, 1.f));
			const float alpha = effectAmount * effectAmount * (0.15f + 0.85f * energyAmount);
			if (alpha <= 0.005f) {
				continue;
			}

			const NVGcolor tone = (avgDeltaDb >= 0.f) ? cyan : purple;
			NVGcolor fill = mixColor(white, tone, 0.25f + 0.75f * effectAmount);
			fill.a = 0.30f + 0.50f * alpha;

			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, curveX[i], curveY[i]);
			nvgLineTo(args.vg, curveX[i + 1], curveY[i + 1]);
			nvgLineTo(args.vg, curveX[i + 1], bottomY);
			nvgLineTo(args.vg, curveX[i], bottomY);
			nvgClosePath(args.vg);
			nvgFillColor(args.vg, fill);
			nvgFill(args.vg);
		}
	}

	nvgBeginPath(args.vg);
	for (int i = 0; i < kCurvePointCount; ++i) {
		if (i == 0) {
			nvgMoveTo(args.vg, curveX[i], curveY[i]);
		}
		else {
			nvgLineTo(args.vg, curveX[i], curveY[i]);
		}
	}
	nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 34));
	nvgStrokeWidth(args.vg, 4.8f);
	nvgStroke(args.vg);

	nvgBeginPath(args.vg);
	for (int i = 0; i < kCurvePointCount; ++i) {
		if (i == 0) {
			nvgMoveTo(args.vg, curveX[i], curveY[i]);
		}
		else {
			nvgLineTo(args.vg, curveX[i], curveY[i]);
		}
	}
	nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 244));
	nvgStrokeWidth(args.vg, 2.2f);
	nvgStroke(args.vg);

	nvgFontSize(args.vg, std::max(10.f, h * 0.095f));
	nvgFontFaceId(args.vg, APP->window->uiFont->handle);
	nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 160));
	nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
	nvgText(args.vg, padX, padY * 0.35f, "SPECTRUM", nullptr);

	nvgFontSize(args.vg, std::max(8.f, h * 0.075f));
	nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 90));
	nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_BOTTOM);
	for (int i = 0; i < kGuideCount; ++i) {
		if (kGuideFreqs[i] >= maxHz) {
			continue;
		}
		const float guideX = padX + usableW * logPosition(kGuideFreqs[i], minHz, maxHz);
		const char* label = nullptr;
		switch (i) {
			case 0: label = "10"; break;
			case 1: label = "100"; break;
			case 2: label = "1k"; break;
			case 3: label = "10k"; break;
			default: break;
		}
		if (label) {
			nvgText(args.vg, guideX, bottomY - 2.f, label, nullptr);
		}
	}

	nvgResetScissor(args.vg);
	nvgRestore(args.vg);
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
		Vec voctPosMm(17.15f, 112.2f);
		Vec fmPosMm(26.7f, 112.2f);
		Vec resoCvPosMm(36.25f, 112.2f);
		Vec balanceCvPosMm(45.8f, 112.2f);
		Vec spanCvPosMm(55.35f, 112.2f);
		Vec outPosMm(64.9f, 112.2f);

		Vec fmLightPosMm(25.3f, 27.3f);
		Vec spanLightPosMm(45.82f, 27.3f);

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
		applyPointOverride("VOCT_INPUT", &voctPosMm);
		applyPointOverride("FM_INPUT", &fmPosMm);
		applyPointOverride("RESO_CV_INPUT", &resoCvPosMm);
		applyPointOverride("BALANCE_CV_INPUT", &balanceCvPosMm);
		applyPointOverride("SPAN_CV_INPUT", &spanCvPosMm);
		applyPointOverride("OUT_OUTPUT", &outPosMm);

		applyPointOverride("FM_AMT_LIGHT", &fmLightPosMm);
		applyPointOverride("SPAN_CV_ATTEN_LIGHT", &spanLightPosMm);

		addParam(createParamCentered<RoundBlackKnob>(mm2px(modePosMm), module, Bifurx::MODE_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(levelPosMm), module, Bifurx::LEVEL_PARAM));
		addParam(createParamCentered<RoundLargeBlackKnob>(mm2px(freqPosMm), module, Bifurx::FREQ_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(resoPosMm), module, Bifurx::RESO_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(balancePosMm), module, Bifurx::BALANCE_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(spanPosMm), module, Bifurx::SPAN_PARAM));
		addParam(createParamCentered<VCVSlider>(mm2px(fmAmtPosMm), module, Bifurx::FM_AMT_PARAM));
		addParam(createParamCentered<VCVSlider>(mm2px(spanCvAttenPosMm), module, Bifurx::SPAN_CV_ATTEN_PARAM));
		addParam(createParamCentered<CKSSThree>(mm2px(titoPosMm), module, Bifurx::TITO_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(inPosMm), module, Bifurx::IN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(voctPosMm), module, Bifurx::VOCT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(fmPosMm), module, Bifurx::FM_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(resoCvPosMm), module, Bifurx::RESO_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(balanceCvPosMm), module, Bifurx::BALANCE_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(spanCvPosMm), module, Bifurx::SPAN_CV_INPUT));
		addOutput(createOutputCentered<BananutBlack>(mm2px(outPosMm), module, Bifurx::OUT_OUTPUT));

		addChild(createLightCentered<SmallLight<BlueLight>>(mm2px(fmLightPosMm), module, Bifurx::FM_AMT_LIGHT));
		addChild(createLightCentered<SmallLight<BlueLight>>(mm2px(spanLightPosMm), module, Bifurx::SPAN_CV_ATTEN_LIGHT));
	}
};

Model* modelBifurx = createModel<Bifurx, BifurxWidget>("Bifurx");
