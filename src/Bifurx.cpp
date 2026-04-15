#include "plugin.hpp"
#include "PanelSvgUtils.hpp"

#include <algorithm>
#include <atomic>
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
constexpr int kFftHopSize = kFftSize / 4;
constexpr int kGuideCount = 4;
const float kGuideFreqs[kGuideCount] = {20.f, 100.f, 1000.f, 10000.f};
constexpr int kBifurxModeCount = 10;
constexpr int kBifurxModeParamIndex = 0;
const char* const kBifurxModeLabels[kBifurxModeCount] = {
	"Low + Low",
	"Low + Band",
	"Notch + Low",
	"Notch + Notch",
	"Low + High",
	"Band + Band",
	"High + High",
	"High + Notch",
	"Band + High",
	"High + Low"
};
constexpr float kResponseMinDb = -48.f;
constexpr float kResponseMaxDb = 48.f;
constexpr float kOverlayDbfsFloor = -96.f;
constexpr float kOverlayDbfsCeiling = 6.f;
constexpr float kDisplayDbfsSpan = 30.f;
constexpr float kDisplayTopDbfsFloor = -36.f;
constexpr float kDisplayTopDbfsCeiling = 0.f;
constexpr float kDisplayTopDynamicCeilingDbfs = kOverlayDbfsCeiling;
constexpr float kDisplayPeakHeadroomDb = 0.6f;

float clamp01(float v) {
	return clamp(v, 0.f, 1.f);
}

float fastExp2(float x) {
	return rack::dsp::exp2_taylor5(clamp(x, -24.f, 24.f));
}

float fastExp(float x) {
	return fastExp2(x * kLog2e);
}

std::string bifurxUserRootPath() {
	return system::join(asset::user(), "Leviathan/Bifurx");
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
	return 2.f - 1.98f * std::pow(r, 1.35f);
}

float signedWeight(float balance, bool upperPeak) {
	const float sign = upperPeak ? 1.f : -1.f;
	return fastExp(0.7f * sign * clamp(balance, -1.f, 1.f));
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
	float wB
) {
	switch (mode) {
		case 0: return cascadeLp;
		case 1: return T(0.95f) * T(wA) * lpA + T(1.05f) * T(wB) * bpB - T(0.12f) * (bpA + bpB);
		case 2: return T(1.05f) * T(wB) * lpB - T(0.55f) * T(wA) * bpA;
		case 3: return cascadeNotch;
		case 4: return T(0.95f) * T(wA) * lpA + T(0.95f) * T(wB) * hpB;
		case 5: return T(1.15f) * (T(wA) * bpA + T(wB) * bpB);
		case 6: return cascadeHpToLp;
		case 7: return T(1.05f) * T(wA) * hpA - T(0.55f) * T(wB) * bpB;
		case 8: return T(1.12f) * T(wA) * bpA + T(0.92f) * T(wB) * hpB - T(0.10f) * (hpA + bpB);
		case 9: return cascadeHpToHp;
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
	float freqParamNorm = 0.5f;
	float voctCv = 0.f;
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
	const std::complex<float> cascadeLp = lpB * lpA;
	const std::complex<float> cascadeNotch = ntB * ntA;
	const std::complex<float> cascadeHpToLp = lpB * hpA;
	const std::complex<float> cascadeHpToHp = hpB * hpA;
	return combineModeResponse<std::complex<float>>(
		model.mode,
		lpA, bpA, hpA, ntA,
		lpB, bpB, hpB, ntB,
		cascadeLp, cascadeNotch, cascadeHpToLp, cascadeHpToHp,
		model.wA, model.wB
	);
}

struct BifurxSpectrumWidget final : Widget {
	struct CurveDebugRecorder {
		bool active = false;
		std::ofstream file;
		std::string path;
		double startTimeSec = 0.0;
		uint64_t sequence = 0;
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
	float overlayDb[kCurvePointCount];
	float overlayTargetDb[kCurvePointCount];
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
	int curveDebugLogDecimator = 0;
	float cachedAxisSampleRate = 0.f;
	BifurxPreviewState previewState;
	bool hasPreview = false;
	bool hasOverlay = false;
	bool hasCurveTarget = false;
	bool hasOverlayTarget = false;
	uint32_t lastPreviewSeq = 0;
	uint32_t lastAnalysisSeq = 0;
	CurveDebugRecorder curveDebugRecorder;

	BifurxSpectrumWidget();
	~BifurxSpectrumWidget() override;
	void step() override;
	void syncCurveDebugCaptureState();
	void startCurveDebugCapture();
	void stopCurveDebugCapture();
	void logCurveDebugSample(const BifurxPreviewState& state, float peakAX, float peakAY, float peakBX, float peakBY);
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
		std::snprintf(label, sizeof(label), "Mode: %s", kBifurxModeLabels[mode]);
		nvgFontSize(args.vg, std::max(8.f, box.size.y * 0.62f));
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgFillColor(args.vg, nvgRGBA(214, 222, 232, 238));
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
		LIGHTS_LEN
	};

	TptSvf coreA;
	TptSvf coreB;
	dsp::ClockDivider previewPublishDivider;
	dsp::ClockDivider previewPublishSlowDivider;
	dsp::ClockDivider controlUpdateDivider;
	BifurxPreviewState lastPreviewState;
	bool hasLastPreviewState = false;
	BifurxPreviewState previewStates[2];
	std::atomic<int> previewPublishedIndex{0};
	std::atomic<uint32_t> previewPublishSeq{0};
	float previewFreqAFiltered = 440.f;
	float previewFreqBFiltered = 440.f;
	float previewQAFiltered = 1.f;
	float previewQBFiltered = 1.f;
	float previewBalanceFiltered = 0.f;
	bool previewFilterInitialized = false;
	float previewFilterAlpha = 0.f;
	float previewFilterAlphaSlow = 0.f;
	float previewFilterAlphaSampleRate = 0.f;
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
	float analysisInputHistory[kFftSize] = {};
	float analysisOutputHistory[kFftSize] = {};
	int analysisWritePos = 0;
	int analysisFilled = 0;
	int analysisHopCounter = 0;
	bool analysisPublishedOnce = false;
	dsp::SchmittTrigger modeLeftTrigger;
	dsp::SchmittTrigger modeRightTrigger;
	BifurxAnalysisFrame analysisFrames[2];
	std::atomic<int> analysisPublishedIndex{0};
	std::atomic<uint32_t> analysisPublishSeq{0};
	bool fftScaleDynamic = true;
	bool curveDebugLogging = false;

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
		previewPublishDivider.setDivision(128);
		previewPublishSlowDivider.setDivision(1024);
		controlUpdateDivider.setDivision(16);
	}

	json_t* dataToJson() override {
		json_t* root = Module::dataToJson();
		json_object_set_new(root, "fftScaleDynamic", json_boolean(fftScaleDynamic));
		json_object_set_new(root, "curveDebugLogging", json_boolean(curveDebugLogging));
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
		const int writeIndex = 1 - analysisPublishedIndex.load(std::memory_order_relaxed);
		const int start = analysisWritePos;
		const int firstCount = kFftSize - start;
		const int secondCount = start;

		std::memcpy(
			analysisFrames[writeIndex].input,
			analysisInputHistory + start,
			size_t(firstCount) * sizeof(float)
		);
		std::memcpy(
			analysisFrames[writeIndex].input + firstCount,
			analysisInputHistory,
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

	void pushAnalysisSample(float inputSample, float outputSample) {
		analysisInputHistory[analysisWritePos] = sanitizeFinite(inputSample);
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
			sanitizeCoreState(coreA);
			sanitizeCoreState(coreB);

			if (modeLeftTrigger.process(params[MODE_LEFT_PARAM].getValue())) {
				const int currentMode = clamp(int(std::round(params[MODE_PARAM].getValue())), 0, 9);
				params[MODE_PARAM].setValue(float((currentMode + 9) % 10));
			}
			if (modeRightTrigger.process(params[MODE_RIGHT_PARAM].getValue())) {
				const int currentMode = clamp(int(std::round(params[MODE_PARAM].getValue())), 0, 9);
				params[MODE_PARAM].setValue(float((currentMode + 1) % 10));
			}

			const float in = sanitizeFinite(inputs[IN_INPUT].getVoltage());
			const float level = params[LEVEL_PARAM].getValue();
			const float drive = levelDriveGain(level);
		const int mode = int(std::round(params[MODE_PARAM].getValue()));
		const int tito = int(std::round(params[TITO_PARAM].getValue()));
		const bool fastPathEligible = (tito == 1)
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
		const float resoNorm = clamp(
			params[RESO_PARAM].getValue() + clamp(inputs[RESO_CV_INPUT].getVoltage(), 0.f, 8.f) / 8.f,
			0.f, 1.f
		);

		if (updateFastControls) {
			const float voct = inputs[VOCT_INPUT].getVoltage();
			const float fmAmt = params[FM_AMT_PARAM].getValue();
			const float fm = clamp(inputs[FM_INPUT].getVoltage(), -10.f, 10.f) * fmAmt;
			const float resoCv = clamp(inputs[RESO_CV_INPUT].getVoltage(), 0.f, 8.f) / 8.f;
			const float balanceCv = clamp(inputs[BALANCE_CV_INPUT].getVoltage(), -5.f, 5.f) / 5.f;
			const float spanCv = clamp(inputs[SPAN_CV_INPUT].getVoltage(), -10.f, 10.f) / 5.f;
			const float spanNorm = clamp(
				params[SPAN_PARAM].getValue() + 0.5f * params[SPAN_CV_ATTEN_PARAM].getValue() * spanCv,
				0.f, 1.f);
			const float spanOct = 8.f * shapedSpan(spanNorm);
			balance = clamp(params[BALANCE_PARAM].getValue() + balanceCv, -1.f, 1.f);
			const float resoNorm = clamp(params[RESO_PARAM].getValue() + resoCv, 0.f, 1.f);
			const float centerHz = kFreqMinHz * fastExp2(kFreqLog2Span * clamp01(params[FREQ_PARAM].getValue()))
				* fastExp2(voct + fm);
			freqA0 = centerHz * fastExp2(-0.5f * spanOct);
			freqB0 = centerHz * fastExp2(0.5f * spanOct);
			const float baseDamping = resoToDamping(resoNorm);
			dampingA = clamp(baseDamping * fastExp(0.55f * balance), 0.02f, 2.2f);
			dampingB = clamp(baseDamping * fastExp(-0.55f * balance), 0.02f, 2.2f);
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

		const float couplingDepth = (0.02f + 0.25f * resoNorm * resoNorm) * (tito == 1 ? 0.f : 1.f);
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
		if (!fastPathEligible) {
			if (tito == 2) {
				modA = couplingDepth * coreA.ic1eq / 5.f;
				modB = couplingDepth * coreB.ic1eq / 5.f;
			}
			else if (tito == 0) {
				modA = couplingDepth * coreB.ic1eq / 5.f;
				modB = couplingDepth * coreA.ic1eq / 5.f;
			}

			const std::pair<float, float> cutoffs = modulatedCutoffs(modA, modB);
			cutoffA = cutoffs.first;
			cutoffB = cutoffs.second;
		}
		float modeOut = 0.f;

		auto processA = [&](float sample) -> SvfOutputs {
			if (fastPathEligible) {
				return coreA.processWithCoeffs(sample, cachedCoeffsA);
			}
			return coreA.process(sample, args.sampleRate, cutoffA, dampingA);
		};
		auto processB = [&](float sample) -> SvfOutputs {
			if (fastPathEligible) {
				return coreB.processWithCoeffs(sample, cachedCoeffsB);
			}
			return coreB.process(sample, args.sampleRate, cutoffB, dampingB);
		};

		switch (mode) {
			case 0: {
				const SvfOutputs a = processA(excitation);
				const SvfOutputs b = processB(a.lp);
				modeOut = combineModeResponse<float>(
					mode,
					a.lp, a.bp, a.hp, a.notch,
					b.lp, b.bp, b.hp, b.notch,
					b.lp, 0.f, 0.f, 0.f,
					wA, wB
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
					wA, wB
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
					wA, wB
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
					wA, wB
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
					wA, wB
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
					wA, wB
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
					wA, wB
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
					wA, wB
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
					wA, wB
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
					wA, wB
				);
			} break;
		}

		const float safeModeOut = sanitizeFinite(modeOut);
		const float out = sanitizeFinite(5.5f * softClip(safeModeOut / 5.5f));
		outputs[OUT_OUTPUT].setChannels(1);
		outputs[OUT_OUTPUT].setVoltage(out);

		const float previewTargetFreqA = clamp(freqA0, 4.f, 0.46f * args.sampleRate);
		const float previewTargetFreqB = clamp(freqB0, 4.f, 0.46f * args.sampleRate);
		const float previewTargetQA = 1.f / std::max(dampingA, 0.05f);
		const float previewTargetQB = 1.f / std::max(dampingB, 0.05f);
		const float previewTargetBalance = balance;
		const bool previewPitchCvConnected = inputs[VOCT_INPUT].isConnected() || inputs[FM_INPUT].isConnected();
		const float previewSmoothingAlpha = previewPitchCvConnected ? previewFilterAlphaSlow : previewFilterAlpha;
		if (!previewFilterInitialized) {
			previewFreqAFiltered = previewTargetFreqA;
			previewFreqBFiltered = previewTargetFreqB;
			previewQAFiltered = previewTargetQA;
			previewQBFiltered = previewTargetQB;
			previewBalanceFiltered = previewTargetBalance;
			previewFilterInitialized = true;
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
		previewState.balance = previewBalanceFiltered;
		previewState.freqParamNorm = clamp(params[FREQ_PARAM].getValue(), 0.f, 1.f);
		previewState.voctCv = inputs[VOCT_INPUT].isConnected() ? clamp(inputs[VOCT_INPUT].getVoltage(), -10.f, 10.f) : 0.f;
		const bool previewPublishTick = previewPitchCvConnected ? previewPublishSlowDivider.process() : previewPublishDivider.process();
		if (!hasLastPreviewState || (previewPublishTick && previewStatesDiffer(previewState, lastPreviewState))) {
			publishPreviewState(previewState);
		}

		pushAnalysisSample(drivenIn, out);

			const float fmAmt = clamp(params[FM_AMT_PARAM].getValue(), -1.f, 1.f);
			lights[FM_AMT_POS_LIGHT].setBrightness(std::max(fmAmt, 0.f));
			lights[FM_AMT_NEG_LIGHT].setBrightness(std::max(-fmAmt, 0.f));

			const float spanAtten = clamp(params[SPAN_CV_ATTEN_PARAM].getValue(), -1.f, 1.f);
			lights[SPAN_CV_ATTEN_POS_LIGHT].setBrightness(std::max(spanAtten, 0.f));
			lights[SPAN_CV_ATTEN_NEG_LIGHT].setBrightness(std::max(-spanAtten, 0.f));
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
		overlayDb[i] = 0.f;
		overlayTargetDb[i] = 0.f;
		overlayOutputDbfs[i] = kOverlayDbfsFloor;
		overlayTargetOutputDbfs[i] = kOverlayDbfsFloor;
	}
}

BifurxSpectrumWidget::~BifurxSpectrumWidget() {
	stopCurveDebugCapture();
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
	curveDebugRecorder.file << "# Bifurx curve debug trace v1\n";
	curveDebugRecorder.file << "# Start when curve debug logging is enabled, stop when it is disabled\n";
	curveDebugRecorder.file << "seq,t_sec,mode,freq_param,voct_cv,freq_a_hz,freq_b_hz,peak_a_x,peak_a_y,peak_b_x,peak_b_y\n";
	curveDebugRecorder.startTimeSec = system::getTime();
	curveDebugRecorder.sequence = 0;
	curveDebugRecorder.active = true;
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
}

void BifurxSpectrumWidget::logCurveDebugSample(
	const BifurxPreviewState& state,
	float peakAX,
	float peakAY,
	float peakBX,
	float peakBY
) {
	if (!module || !curveDebugRecorder.active || !curveDebugRecorder.file.good()) {
		return;
	}

	const double tSec = std::max(0.0, system::getTime() - curveDebugRecorder.startTimeSec);
	curveDebugRecorder.file
		<< curveDebugRecorder.sequence++ << ","
		<< tSec << ","
		<< state.mode << ","
		<< state.freqParamNorm << ","
		<< state.voctCv << ","
		<< state.freqA << ","
		<< state.freqB << ","
		<< peakAX << ","
		<< peakAY << ","
		<< peakBX << ","
		<< peakBY << "\n";
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
	Widget::step();
	syncCurveDebugCaptureState();
	if (!module) {
		return;
	}

	bool dirty = false;
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

	if (hasCurveTarget) {
		bool curveAnimating = false;
		const float curveSmoothing = 0.24f;
		for (int i = 0; i < kCurvePointCount; ++i) {
			const float prev = curveDb[i];
			const float next = mixf(prev, curveTargetDb[i], curveSmoothing);
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
			const float prevDb = overlayDb[i];
			const float nextDb = mixf(prevDb, overlayTargetDb[i], overlayDbSmoothing);
			overlayDb[i] = nextDb;
			if (std::fabs(nextDb - prevDb) > 0.02f) {
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

	if (dirty && framebuffer) {
		framebuffer->setDirty();
	}
}

void BifurxSpectrumWidget::updateCurveCache() {
	if (!hasPreview) {
		return;
	}

	const BifurxPreviewModel model = makePreviewModel(previewState);
	updateAxisCache();

	for (int i = 0; i < kCurvePointCount; ++i) {
		const float mag = std::abs(previewModelResponse(model, curveHz[i]));
		const float db = 20.f * std::log10(std::max(mag, 1e-5f));
		curveTargetDb[i] = clamp(db, kResponseMinDb, kResponseMaxDb);
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

	float binDeltaDb[kFftBinCount];
	float binOutputDbfs[kFftBinCount];
	for (int bin = 0; bin < kFftBinCount; ++bin) {
		const float inputAmp = amplitudeScale * orderedSpectrumMagnitude(fftInputFreq, bin);
		const float outputAmp = amplitudeScale * orderedSpectrumMagnitude(fftOutputFreq, bin);
		binDeltaDb[bin] = clamp(20.f * std::log10((outputAmp + 1e-6f) / (inputAmp + 1e-6f)), -24.f, 24.f);
		binOutputDbfs[bin] = clamp(20.f * std::log10(outputAmp / 5.f + 1e-6f), kOverlayDbfsFloor, kOverlayDbfsCeiling);
	}

	float sampledDeltaDb[kCurvePointCount];
	float sampledOutputDbfs[kCurvePointCount];
	for (int i = 0; i < kCurvePointCount; ++i) {
		const float binPosition = curveBinPos[i];
		const int binA = int(std::floor(binPosition));
		const int binB = std::min(binA + 1, kFftSize / 2);
		const float frac = binPosition - float(binA);
		sampledDeltaDb[i] = mixf(binDeltaDb[binA], binDeltaDb[binB], frac);
		sampledOutputDbfs[i] = mixf(binOutputDbfs[binA], binOutputDbfs[binB], frac);
	}

	float framePeakDbfs = kOverlayDbfsFloor;
	float frameSmoothedOutputDbfs[kCurvePointCount];
	const float targetSmoothing = hasOverlayTarget ? 0.45f : 1.f;
	for (int i = 0; i < kCurvePointCount; ++i) {
		const int left = std::max(0, i - 1);
		const int right = std::min(kCurvePointCount - 1, i + 1);
		const float smoothDeltaDb = 0.12f * sampledDeltaDb[left] + 0.76f * sampledDeltaDb[i] + 0.12f * sampledDeltaDb[right];
		const float smoothOutputDbfs = 0.12f * sampledOutputDbfs[left] + 0.76f * sampledOutputDbfs[i] + 0.12f * sampledOutputDbfs[right];
		frameSmoothedOutputDbfs[i] = smoothOutputDbfs;
		overlayTargetDb[i] = mixf(overlayTargetDb[i], smoothDeltaDb, targetSmoothing);
		overlayTargetOutputDbfs[i] = mixf(overlayTargetOutputDbfs[i], smoothOutputDbfs, targetSmoothing);
		framePeakDbfs = std::max(framePeakDbfs, overlayTargetOutputDbfs[i]);
	}

	if (!hasOverlayTarget) {
		for (int i = 0; i < kCurvePointCount; ++i) {
			overlayDb[i] = overlayTargetDb[i];
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

	for (int i = 0; i < kCurvePointCount; ++i) {
		const float x01 = float(i) / float(kCurvePointCount - 1);
		curveX[i] = plotX + usableW * x01;
		curveY[i] = responseYForDb(curveDb[i]);
	}

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

	char curveMinLabel[12];
	char curveMaxLabel[12];
	compactSignedLabel(responseMinDb, curveMinLabel, sizeof(curveMinLabel));
	compactSignedLabel(responseMaxDb, curveMaxLabel, sizeof(curveMaxLabel));
	char curveRangeLabel[40];
	std::snprintf(curveRangeLabel, sizeof(curveRangeLabel), "Curve %s/%s dB", curveMinLabel, curveMaxLabel);
	nvgTextAlign(args.vg, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP);
	nvgText(args.vg, w - 1.5f, 1.f, curveRangeLabel, nullptr);

	nvgBeginPath(args.vg);
	nvgMoveTo(args.vg, plotX, responseYForDb(0.f));
	nvgLineTo(args.vg, plotX + usableW, responseYForDb(0.f));
	nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 24));
	nvgStrokeWidth(args.vg, 1.2f);
	nvgStroke(args.vg);

	if (hasOverlay) {
		const NVGcolor purple = nvgRGB(122, 92, 255);
		const NVGcolor cyan = nvgRGB(28, 204, 217);
		const NVGcolor white = nvgRGB(206, 210, 216);
		nvgShapeAntiAlias(args.vg, 1);

		for (int i = 0; i < kCurvePointCount - 1; ++i) {
			const float avgDeltaDb = 0.5f * (overlayDb[i] + overlayDb[i + 1]);
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
	nvgLineJoin(args.vg, NVG_ROUND);
	nvgLineCap(args.vg, NVG_ROUND);
	nvgStrokeWidth(args.vg, 3.2f);
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
	nvgLineJoin(args.vg, NVG_ROUND);
	nvgLineCap(args.vg, NVG_ROUND);
	nvgStrokeWidth(args.vg, 1.35f);
	nvgStroke(args.vg);

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
		const float clampedHz = clamp(targetHz, minHz, maxHz);
		const float targetX01 = logPosition(clampedHz, minHz, maxHz);
		const float markerRadius = 2.3f;
		const float curveIndex = targetX01 * float(kCurvePointCount - 1);
		const int i0 = clamp(int(std::floor(curveIndex)), 0, kCurvePointCount - 1);
		const int i1 = std::min(i0 + 1, kCurvePointCount - 1);
		const float t = curveIndex - float(i0);
		marker.x = plotX + usableW * targetX01;
		marker.yCurve = mixf(curveY[i0], curveY[i1], t);
		marker.yMarker = clamp(
			marker.yCurve,
			spectrumTopY + markerRadius + 0.4f,
			spectrumBottomY - markerRadius - 0.4f
		);
		marker.hz = clampedHz;
		marker.visible = marker.yCurve <= (spectrumBottomY - 0.35f);
		formatFrequencyLabel(marker.hz, marker.label, sizeof(marker.label));
		return marker;
	};

	PeakMarker peaks[2];
	peaks[0] = buildMarkerAtFrequency(previewState.freqA);
	peaks[1] = buildMarkerAtFrequency(previewState.freqB);
	if (module && module->curveDebugLogging) {
		curveDebugLogDecimator++;
		// Throttle debug output to keep logs readable during modulation tests.
		if (curveDebugLogDecimator >= 30) {
			curveDebugLogDecimator = 0;
			logCurveDebugSample(
				previewState,
				peaks[0].x, peaks[0].yMarker,
				peaks[1].x, peaks[1].yMarker
			);
		}
	}
	else {
		curveDebugLogDecimator = 0;
	}

	float labelX[2] = {peaks[0].x, peaks[1].x};
	const int leftIndex = (labelX[0] <= labelX[1]) ? 0 : 1;
	const int rightIndex = 1 - leftIndex;
	const float labelMargin = std::max(18.f, w * 0.08f);
	const float minLabelSeparation = std::max(30.f, w * 0.18f);
	const float minX = plotX + labelMargin;
	const float maxX = plotX + usableW - labelMargin;
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

	const float freqLabelFontSize = std::max(7.f, h * 0.055f);
	const float labelTextY = labelBandTop + 0.5f * labelBandHeight;
	const float guideNominalY = labelBandTop + std::min(2.1f, 0.18f * labelBandHeight);
	const float guideMaxY = labelTextY - 0.5f * freqLabelFontSize - 0.6f;
	const float guideYBottom = clamp(guideNominalY, labelBandTop + 0.2f, guideMaxY);
	for (int i = 0; i < 2; ++i) {
		if (!peaks[i].visible) {
			continue;
		}
		const float markerRadius = 2.3f;
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, peaks[i].x, peaks[i].yMarker + markerRadius + 0.45f);
		nvgLineTo(args.vg, peaks[i].x, guideYBottom);
		nvgStrokeColor(args.vg, nvgRGBA(246, 250, 255, 170));
		nvgStrokeWidth(args.vg, 1.1f);
		nvgStroke(args.vg);

		nvgBeginPath(args.vg);
		nvgCircle(args.vg, peaks[i].x, peaks[i].yMarker, markerRadius);
		nvgFillColor(args.vg, nvgRGBA(252, 255, 255, 244));
		nvgFill(args.vg);
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, peaks[i].x, peaks[i].yMarker, markerRadius + 0.95f);
		nvgStrokeColor(args.vg, nvgRGBA(8, 10, 14, 220));
		nvgStrokeWidth(args.vg, 1.f);
		nvgStroke(args.vg);
	}

	nvgFontSize(args.vg, freqLabelFontSize);
	nvgFontFaceId(args.vg, APP->window->uiFont->handle);
	nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
	for (int i = 0; i < 2; ++i) {
		nvgFillColor(args.vg, nvgRGBA(4, 6, 9, 240));
		nvgText(args.vg, labelX[i], labelTextY + 0.75f, peaks[i].label, nullptr);
		nvgFillColor(args.vg, nvgRGBA(241, 246, 252, 250));
		nvgText(args.vg, labelX[i], labelTextY, peaks[i].label, nullptr);
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

			Vec modeLeftPosMm = modePosMm.plus(Vec(-2.5f, 0.f));
			Vec modeRightPosMm = modePosMm.plus(Vec(2.5f, 0.f));
			addParam(createParamCentered<BifurxModeLeftButton>(mm2px(modeLeftPosMm), module, Bifurx::MODE_LEFT_PARAM));
			addParam(createParamCentered<BifurxModeRightButton>(mm2px(modeRightPosMm), module, Bifurx::MODE_RIGHT_PARAM));
			addParam(createParamCentered<RoundBlackKnob>(mm2px(levelPosMm), module, Bifurx::LEVEL_PARAM));
		addParam(createParamCentered<Davies1900hWhiteKnob>(mm2px(freqPosMm), module, Bifurx::FREQ_PARAM));
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

			addChild(createLightCentered<SmallLight<GreenLight>>(mm2px(fmLightPosMm), module, Bifurx::FM_AMT_POS_LIGHT));
			addChild(createLightCentered<SmallLight<RedLight>>(mm2px(fmLightPosMm), module, Bifurx::FM_AMT_NEG_LIGHT));
			addChild(createLightCentered<SmallLight<GreenLight>>(mm2px(spanLightPosMm), module, Bifurx::SPAN_CV_ATTEN_POS_LIGHT));
			addChild(createLightCentered<SmallLight<RedLight>>(mm2px(spanLightPosMm), module, Bifurx::SPAN_CV_ATTEN_NEG_LIGHT));
		}

	void appendContextMenu(Menu* menu) override {
		ModuleWidget::appendContextMenu(menu);

		Bifurx* bifurx = dynamic_cast<Bifurx*>(module);
		if (!bifurx) {
			return;
		}

		menu->addChild(new MenuSeparator());
		menu->addChild(createBoolPtrMenuItem("Dynamic FFT Scale", "", &bifurx->fftScaleDynamic));
		menu->addChild(createBoolPtrMenuItem("Log Curve Debug", "", &bifurx->curveDebugLogging));
	}
};

Model* modelBifurx = createModel<Bifurx, BifurxWidget>("Bifurx");
