#pragma once

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
#include <vector>

namespace bifurx {

// Forward declarations
struct Bifurx;
struct BifurxSpectrumGLWidget;
struct BifurxFreqQuantity final : ParamQuantity {
	float getDisplayValue() override;
	void setDisplayValue(float displayValue) override;
	std::string getDisplayValueString() override;
};
struct BifurxSpanQuantity final : ParamQuantity {
	float getDisplayValue() override;
	void setDisplayValue(float displayValue) override;
	std::string getDisplayValueString() override;
};

Widget* createGlSpectrumDisplay(Bifurx* module, math::Rect rectMm);

// Constants
constexpr float kDefaultPanelWidthMm = 71.12f;
constexpr float kDefaultPanelHeightMm = 128.5f;
constexpr float kPi = 3.14159265358979323846f;
constexpr float kLog2e = 1.4426950408889634f;
constexpr float kFreqMinHz = 4.f;
constexpr float kFreqMaxHz = 28000.f;
constexpr float kFreqLog2Span = 12.7731392f; // log2(28000 / 4)
constexpr float kSvfDampingMin = 0.02f;
constexpr float kSvfDampingMax = 2.2f;
constexpr int kCurvePointCount = 513;
constexpr int kFftSize = 4096;
constexpr int kFftBinCount = kFftSize / 2 + 1;
constexpr int kFftHopSize = kFftSize / 2;
constexpr int kPreviewPublishFastDivision = 128;
constexpr int kPreviewPublishSlowDivision = 256;
constexpr int kPerfMeasureDivision = 17;
constexpr int kPreviewAdaptiveCooldownSamples = 64;
constexpr float kPreviewAdaptiveOctaveThreshold = 0.015f;
constexpr float kPreviewAdaptiveSpanOctThreshold = 0.04f;
constexpr float kPreviewAdaptiveQThreshold = 0.05f;
constexpr float kPreviewAdaptiveBalanceThreshold = 0.015f;
constexpr float kLlTelemetryTauSeconds = 0.05f;
constexpr float kPreviewInstantSettleMotionOctThreshold = 2e-5f;
constexpr int kPreviewInstantSettleHoldSamples = 96;
constexpr int kBifurxModeCount = 10;
constexpr int kBifurxUiModeCount = kBifurxModeCount;
constexpr int kBifurxModeParamIndex = 0;
extern const char* const kBifurxModeLabels[kBifurxModeCount];

constexpr float kResponseMinDb = -48.f;
constexpr float kResponseMaxDb = 48.f;
constexpr float kOverlayDbfsFloor = -96.f;
constexpr float kOverlayDbfsCeiling = 6.f;
constexpr float kOverlaySubsonicCutHz = 10.f;
constexpr float kOverlaySubsonicFadeHz = 30.f;
constexpr float kVoctSmoothingTauSeconds = 0.0025f;
constexpr float kVoctDeadbandVolts = 0.001f;
constexpr float kTitoCoeffRelativeUpdateThreshold = 2.5e-4f;
constexpr float kTitoCoeffAbsoluteUpdateThresholdHz = 0.002f;
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

// Utility functions
inline float clamp01(float v) {
	return clamp(v, 0.f, 1.f);
}

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

inline float shapedSpan(float value) {
	return std::pow(clamp01(value), 1.45f);
}

float levelDriveGain(float knob);
float smoothstep01(float x);
float levelInputGain(float knob);
float levelDriveAmount(float knob);
float levelOutputClipWet(float knob);
float applyLevelInputStage(float in, float levelKnob);
float applyLevelOutputStage(float modeOut, float levelKnob, bool softLimitingEnabled = true);

inline float fastTanh(float x) {
	const float x2 = x * x;
	if (x2 < 9.f) {
		return x * (27.f + x2) / (27.f + 9.f * x2);
	}
	return (x > 0.f) ? 1.f : -1.f;
}

inline float softClip(float x) {
	return fastTanh(x);
}

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
float responseYForDbDisplay(float db, float minDb, float maxDb, float bottomY, float topY);
float softLimitOverlayDeltaDb(float db);
float softLimitExpectedCurveDb(float db);
float resoToDamping(float resoNorm);

float signedWeight(float balance, bool upperPeak);
float cascadeWideMorph(float spanNorm);
float highHighSpanCompGain(float wideMorph);

struct SvfOutputs {
	float lp = 0.f;
	float bp = 0.f;
	float hp = 0.f;
	float notch = 0.f;
};

NVGcolor mixColor(const NVGcolor& a, const NVGcolor& b, float t);
void formatFrequencyLabel(float hz, char* out, size_t outSize);

struct SvfCoeffs {
	float g = 0.f;
	float k = 0.f;
	float a1 = 1.f;
};

SvfCoeffs makeSvfCoeffs(float sampleRate, float cutoff, float damping, float dampingMin = kSvfDampingMin);

struct TptSvf {
	float ic1eq = 0.f;
	float ic2eq = 0.f;

	SvfOutputs processWithCoeffs(float input, const SvfCoeffs& coeffs);
	SvfOutputs processSelfOscWithCoeffs(const SvfCoeffs& coeffs, float input, float oscOnset, float oscHeat, float oscDrive);
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

struct DisplayBiquad {
	float b0 = 0.f;
	float b1 = 0.f;
	float b2 = 0.f;
	float a1 = 0.f;
	float a2 = 0.f;

	std::complex<float> response(float omega) const;
	std::complex<float> response(std::complex<float> z1, std::complex<float> z2) const;
};

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
	float wB,
	float wideMorph
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
		case 9: return T(1.06f * highHighSpanCompGain(wideMorph)) * cascadeHpToHp;
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
};

struct BifurxLlTelemetryState {
	bool active = false;
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
	float qA = 1.f;
	float qB = 1.f;
	float resoNorm = 0.f;
	float wA = 1.f;
	float wB = 1.f;
	float wideMorph = 0.f;
	int mode = 0;
};

struct BifurxAnalysisFrame {
	alignas(16) float rawInput[kFftSize];
	alignas(16) float output[kFftSize];
	alignas(16) float responseOutput[kFftSize];
};

struct BifurxSpectrumState {
	float curveHz[kCurvePointCount];
	float curveBinPos[kCurvePointCount];
	float curveDb[kCurvePointCount];
	float curveTargetDb[kCurvePointCount];
	float overlayModuleDb[kCurvePointCount];
	float overlayTargetModuleDb[kCurvePointCount];
	float overlayOutputDbfs[kCurvePointCount];
	float overlayTargetOutputDbfs[kCurvePointCount];
	float displayTopDbfs = kDisplayTopDbfsCeiling;
	float displayTopTargetDbfs = kDisplayTopDbfsCeiling;
	float cachedAxisSampleRate = 0.f;
	uint32_t lastPreviewSeq = 0;
	uint32_t lastAnalysisSeq = 0;
	bool hasPreview = false;
	bool hasOverlay = false;
	bool hasCurveTarget = false;
	bool hasOverlayTarget = false;
	BifurxPreviewState previewState;
};

struct BifurxCurvePoint {
	float x01;
	float y;
	int priority; // 0: regular, 1: refinement, 2: anchor pin
};

struct BifurxMarkerLayout {
	struct Marker {
		float x;
		float yCurve;
		float yMarker;
		float hz;
		bool visible = false;
		char label[16] = {};
	};
	Marker markers[2];
	float labelX[2];
	float labelY;
	float labelFontSize;
	float guideYBottom;
	bool anchorToBottomLane;
};

struct BifurxRenderTickResult {
	bool previewUpdated = false;
	bool analysisUpdated = false;
	bool animationActive = false;
	float curvePrepUs = 0.f;
	float overlayPrepUs = 0.f;
};

struct BifurxSpectrumBase {
	Bifurx* module = nullptr;
	BifurxSpectrumState state;

	// Common FF resources for analysis
	dsp::RealFFT fft;
	alignas(16) float window[kFftSize];
	alignas(16) float fftInputTime[kFftSize];
	alignas(16) float fftOutputTime[kFftSize];
	alignas(16) float fftInputFreq[2 * kFftSize];
	alignas(16) float fftOutputFreq[2 * kFftSize];
	alignas(16) float fftResponseOutputFreq[2 * kFftSize];
	alignas(16) float fftRawInputFreq[2 * kFftSize];

	uint32_t lastModelUpdateSeq = 0;
	mutable BifurxPreviewModel cachedModel;
	float lastCurvePrepUs = 0.f;
	float lastOverlayPrepUs = 0.f;

	BifurxSpectrumBase() : fft(kFftSize) {
		for (int i = 0; i < kFftSize; i++) {
			window[i] = 0.5f - 0.5f * std::cos(2.f * kPi * float(i) / float(kFftSize - 1));
		}
		for (int i = 0; i < kCurvePointCount; i++) {
			state.curveDb[i] = kResponseMinDb;
			state.curveTargetDb[i] = kResponseMinDb;
			state.overlayModuleDb[i] = 0.f;
			state.overlayTargetModuleDb[i] = 0.f;
			state.overlayOutputDbfs[i] = kOverlayDbfsFloor;
			state.overlayTargetOutputDbfs[i] = kOverlayDbfsFloor;
		}
	}

	virtual ~BifurxSpectrumBase() {}

	void syncBase();
	void updateAxisCache();
	void updateCurveCache();
	const BifurxPreviewModel& getOrUpdateModel() const;
	void updateOverlayCache(int writePos);
	bool updateAnimation(float dt);
	BifurxRenderTickResult runRenderTick(float dt);
	virtual void drawNanoVG(const rack::widget::Widget::DrawArgs& args) {}

	int markerAnchorKind(int markerIndex) const {
		switch (state.previewState.mode) {
			case 2: return (markerIndex == 0) ? -1 : 1;
			case 3: return -1;
			case 7: return (markerIndex == 1) ? -1 : 1;
			default: return 0;
		}
	}

	bool markerPinnedToBottomLane(int markerIndex) const {
		switch (state.previewState.mode) {
			case 2: return markerIndex == 0; // Notch + Low
			case 3: return true;            // Notch + Notch
			case 7: return markerIndex == 1; // High + Notch
			default: return false;
		}
	}

	struct DisplayAnchor { float x01 = 0.f; float hz = 0.f; };
	DisplayAnchor displayAnchorForMarker(int markerIndex, float targetHz, float minHz, float maxHz) const {
		const float clampedHz = clamp(targetHz, minHz, maxHz);
		DisplayAnchor anchor; anchor.x01 = logPosition(clampedHz, minHz, maxHz); anchor.hz = clampedHz;
		const int anchorKind = markerAnchorKind(markerIndex);
		if (anchorKind == 0) return anchor;
		if ((state.previewState.mode == 2 || state.previewState.mode == 7) &&
			std::fabs(std::log2(std::max(state.previewState.freqB, 1e-6f) / std::max(state.previewState.freqA, 1e-6f))) < 0.08f) {
			return anchor;
		}
		const int centerIndex = clamp(int(std::round(anchor.x01 * float(kCurvePointCount - 1))), 0, kCurvePointCount - 1);
		int bestIndex = centerIndex;
		float bestScore = (anchorKind < 0) ? state.curveDb[centerIndex] : -state.curveDb[centerIndex];
		for (int i = std::max(0, centerIndex - 18); i <= std::min(kCurvePointCount - 1, centerIndex + 18); ++i) {
			const float base = (anchorKind < 0) ? state.curveDb[i] : -state.curveDb[i];
			const float score = base + 0.22f * std::fabs(float(i - centerIndex));
			if (score < bestScore) { bestScore = score; bestIndex = i; }
		}
		anchor.x01 = float(bestIndex) / float(kCurvePointCount - 1);
		anchor.hz = logFrequencyAt(anchor.x01, minHz, maxHz);
		return anchor;
	}

	float curveYAtX01(float x01, float spectrumBottomY, float spectrumTopY) const {
		auto responseYForDb = [&](float db) { return responseYForDbDisplay(db, kResponseMinDb, kResponseMaxDb, spectrumBottomY, spectrumTopY); };
		const float curveIndex = clamp(x01, 0.f, 1.f) * float(kCurvePointCount - 1);
		const int i0 = clamp(int(std::floor(curveIndex)), 0, kCurvePointCount - 1), i1 = std::min(i0 + 1, kCurvePointCount - 1);
		return responseYForDb(mixf(state.curveDb[i0], state.curveDb[i1], curveIndex - float(i0)));
	}

	void calculateMarkerLayout(BifurxMarkerLayout* layout, float w, float h) const;
	void calculateRefinedCurvePoints(std::vector<BifurxCurvePoint>* points, float w, float h) const;
};

bool previewStatesDiffer(const BifurxPreviewState& a, const BifurxPreviewState& b);
BifurxPreviewModel makePreviewModel(const BifurxPreviewState& state);
std::complex<float> previewModelResponse(const BifurxPreviewModel& model, float hz);
float previewModelResponseDb(const BifurxPreviewModel& model, float hz);

constexpr float kPreviewProbeLevelKnob = 0.5f;
constexpr float kPreviewProbeImpulseAmplitude = 0.01f;
constexpr int kPreviewProbeBurstLength = 64;

float previewProbeStimulusSample(const BifurxPreviewState& state, int sampleIndex);

struct BifurxProbeEngineState {
	TptSvf svfA;
	TptSvf svfB;
};

SvfOutputs processProbeStage(
	BifurxProbeEngineState& state,
	int stageIndex,
	float input,
	float sampleRate,
	float cutoff,
	float damping,
	float drive,
	float resoNorm,
	bool highResonanceSelfOscEnabled
);

void simulatePreviewProbeImpulseResponse(
	const BifurxPreviewState& state,
	float* inputBuffer,
	float* outputBuffer,
	int sampleCount
);

struct Bifurx : Module {
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
		TITO_SM_LIGHT,
		TITO_XM_LIGHT,
		LIGHTS_LEN
	};

	enum RenderMode {
		RENDER_NANOVG,
		RENDER_OPENGL
	};
	enum ModulationQualityMode {
		MOD_QUALITY_BALANCED = 0,
		MOD_QUALITY_HIGH,
		MOD_QUALITY_EXACT,
		MOD_QUALITY_COUNT
	};

	TptSvf coreA;
	TptSvf coreB;
	RenderMode renderMode = RENDER_OPENGL;
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
	float llTelemetryAlpha = 0.f;
	float llTelemetryAlphaSampleRate = 0.f;
	float previewPrevTargetFreqA = 440.f;
	float previewPrevTargetFreqB = 440.f;
	bool previewTargetMotionInitialized = false;
	int previewTargetStillSamples = 0;
	int previewSampleAccum = 0;
	int previewAdaptiveCooldown = 0;
	bool controlFastCacheValid = false;
	float cachedDampingA = 0.7f;
	float cachedDampingB = 0.7f;
	float cachedWA = 1.f;
	float cachedWB = 1.f;
	float cachedFreqA0 = 440.f;
	float cachedFreqB0 = 440.f;
	float cachedBalance = 0.f;
	float cachedResoNorm = 0.35f;
	float cachedBalanceNorm = 0.f;
	float cachedSpanParamNorm = 0.33f;
	float cachedSpanCvNorm = 0.f;
	float cachedSpanAtten = 0.f;
	float cachedSpanNorm = 0.33f;
	float cachedSpanOct = 0.f;
	float cachedSpanWideMorph = 0.f;
	float cachedSpanFreqMulA = 1.f;
	float cachedSpanFreqMulB = 1.f;
	SvfCoeffs cachedCoeffsA;
	SvfCoeffs cachedCoeffsB;
	SvfCoeffs titoCoeffsA;
	SvfCoeffs titoCoeffsB;
	float titoCoeffFreqA = 0.f;
	float titoCoeffFreqB = 0.f;
	float titoCoeffDampingA = 0.f;
	float titoCoeffDampingB = 0.f;
	float titoCoeffSampleRateA = 0.f;
	float titoCoeffSampleRateB = 0.f;
	float analysisRawInputHistory[kFftSize] = {};
	float analysisOutputHistory[kFftSize] = {};
	float analysisResponseOutputHistory[kFftSize] = {};
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
	std::atomic<int> analysisPublishedWritePos{0};
	std::atomic<uint32_t> analysisPublishSeq{0};
	bool fftScaleDynamic = true;
	bool showModuleResponseOverlay = false;
	bool useGlShaderRenderer = true;
	bool highResonanceSelfOscEnabled = false;
	bool softLimitingEnabled = true;
	int modulationQualityMode = MOD_QUALITY_BALANCED;
	int controlUpdateDivision = 16;
	bool curveDebugLogging = false;
	bool perfDebugLogging = false;
	std::atomic<uint64_t> perfAudioSampledCount{0};
	std::atomic<uint64_t> perfAudioProcessNs{0};
	std::atomic<uint64_t> perfAudioControlsNs{0};
	std::atomic<uint64_t> perfAudioCoreNs{0};
	std::atomic<uint64_t> perfAudioPreviewNs{0};
	std::atomic<uint64_t> perfAudioAnalysisNs{0};
	std::atomic<uint64_t> perfAudioProcessMaxNs{0};
	std::atomic<float> perfSampleRate{0.f};
	std::atomic<int> perfMode{0};
	std::atomic<bool> perfFastPathEligible{false};
	std::atomic<bool> perfPreviewPitchCvConnected{false};
	uint32_t debugInstanceId = 0;
	double createdUnixTimeSec = 0.0;

	Bifurx();
	void resetCircuitStates();
	json_t* dataToJson() override;
	void dataFromJson(json_t* root) override;
	void resetPerfStats();
	void publishPreviewState(const BifurxPreviewState& state);
	void publishLlTelemetryState(const BifurxLlTelemetryState& state);
	void pushAnalysisSample(float rawInputSample, float outputSample, float responseOutputSample);
	void onSampleRateChange(const SampleRateChangeEvent& e) override;
	void process(const ProcessArgs& args) override;
};

} // namespace bifurx
