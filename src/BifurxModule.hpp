#pragma once

#include "BifurxDsp.hpp"
#include "BifurxInputStage.hpp"
#include "BifurxOutputStage.hpp"
#include "BifurxOversampling.hpp"
#include "BifurxTransitionSmoother.hpp"
#include "DebugTerminalTransport.hpp"

#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <limits>

namespace bifurx {

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

struct Bifurx : Module {
	ModuleTeardownTimer teardownTimer {"Bifurx"};
	enum ColorScheme {
		SCHEME_DEFAULT = 0,
		SCHEME_CLASSIC,
		SCHEME_MONOCHROME,
		SCHEME_FIRE,
		SCHEME_RETRO_AMBER,
		SCHEME_RETRO_GREEN,
		SCHEME_LEN
	};
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
		MODE_MENU_PARAM,
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
	enum VisualWorkerMode {
		VISUAL_WORKER_INHERIT = -1,
		VISUAL_WORKER_OFF = 0,
		VISUAL_WORKER_AUTO = 1,
		VISUAL_WORKER_ON = 2
	};

	TptSvf coreA;
	TptSvf coreB;
	BifurxNonlinearOversampling2x nonlinearOversampling;
	BifurxLegacyOversampling2x legacyOversampling;
	BifurxIirOversampling2x iirOversampling;
	// 1 = legacy dark FIR, 2 = current FIR, 3 = low-latency IIR4.
	std::atomic<int> boundaryResampling{3};
	int audioBoundary = -1;
	BifurxTransitionSmoother transitionSmoother;
	RenderMode renderMode = RENDER_OPENGL;
	// Production context-owned fixed GL surface. The debug menu may disable it
	// temporarily for diagnostics, but enabled is the supported default path.
	std::atomic<bool> fixedGlSurfaceEnabled {true};
	dsp::ClockDivider previewPublishDivider;
	dsp::ClockDivider previewPublishSlowDivider;
	dsp::ClockDivider controlUpdateDivider;
	dsp::ClockDivider perfMeasureDivider;
	BifurxPreviewState lastPreviewState;
	bool hasLastPreviewState = false;
	BifurxPreviewState previewStates[kSnapshotSlotCount];
	double previewStatePublishTimes[kSnapshotSlotCount] = {};
	uint32_t previewStateSeqs[kSnapshotSlotCount] = {};
	std::atomic<uint32_t> previewStateReaders[kSnapshotSlotCount] {};
	std::atomic<uint64_t> previewPublishedToken{0};
	uint64_t previewPublishGeneration = 0;
	std::atomic<uint32_t> previewPublishSeq{0};
	BifurxLlTelemetryState llTelemetryStates[kSnapshotSlotCount];
	uint32_t llTelemetryStateSeqs[kSnapshotSlotCount] = {};
	std::atomic<uint32_t> llTelemetryStateReaders[kSnapshotSlotCount] {};
	std::atomic<uint64_t> llTelemetryPublishedToken{0};
	uint64_t llTelemetryPublishGeneration = 0;
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
	float llTelemetryAlpha = 0.f;
	float llTelemetryAlphaSampleRate = 0.f;
	float previewPrevTargetFreqA = 440.f;
	float previewPrevTargetFreqB = 440.f;
	bool previewTargetMotionInitialized = false;
	int previewTargetStillSamples = 0;
	int previewSampleAccum = 0;
	bool controlFastCacheValid = false;
	bool previewDemandWasActive = false;
	int audioQualityMode = -1;
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
	float cachedFrequencyRangeSampleRate = 0.f;
	float cachedFrequencyRangeOctaves = 0.f;
	float cachedFreqParamNorm = -1.f;
	float cachedVoctCv = 0.f;
	float cachedFm = 0.f;
	float cachedPitchSampleRate = 0.f;
	bool cachedLowLatencyVisual = false;
	bool cachedHighResonanceSelfOscEnabled = false;
	bool cachedSoftLimitingEnabled = true;
	bool cachedNonlinearOversamplingEnabled = true;
	CharacterStageState cachedCharacterState;
	float cachedCharacterDrive = 0.f;
	float cachedCharacterResoNorm = 0.f;
	bool cachedCharacterHighResEnabled = false;
	bool cachedCharacterStateValid = false;
	SvfCoeffs cachedCoeffsA;
	SvfCoeffs cachedCoeffsB;
	SvfCoeffs selfOscCoeffsA;
	SvfCoeffs selfOscCoeffsB;
	float selfOscCoeffFreqA = 0.f;
	float selfOscCoeffFreqB = 0.f;
	float selfOscCoeffDampingA = 0.f;
	float selfOscCoeffDampingB = 0.f;
	float selfOscCoeffSampleRateA = 0.f;
	float selfOscCoeffSampleRateB = 0.f;
	SvfCoeffs titoCoeffsA;
	SvfCoeffs titoCoeffsB;
	float titoCoeffFreqA = 0.f;
	float titoCoeffFreqB = 0.f;
	float titoCoeffDampingA = 0.f;
	float titoCoeffDampingB = 0.f;
	float titoCoeffSampleRateA = 0.f;
	float titoCoeffSampleRateB = 0.f;
	float titoSmDcCorrection = 0.f;
	BifurxAnalysisFrame analysisFrames[kAnalysisFrameSlotCount];
	uint32_t analysisFrameSeqs[kAnalysisFrameSlotCount] = {};
	std::atomic<uint32_t> analysisFrameReaders[kAnalysisFrameSlotCount] {};
	std::atomic<uint64_t> analysisPublishedToken{0};
	uint64_t analysisPublishGeneration = 0;
	int analysisCaptureSlots[2] = {-1, -1};
	int analysisCapturePositions[2] = {};
	int analysisCaptureCountdown = 0;
	float llTelemetryExcitationSq = 0.f;
	float llTelemetryStageALpSq = 0.f;
	float llTelemetryStageBLpSq = 0.f;
	float llTelemetryOutputSq = 0.f;
	dsp::SchmittTrigger modeLeftTrigger;
	dsp::SchmittTrigger modeRightTrigger;
	std::atomic<uint32_t> analysisPublishSeq{0};
	std::atomic<uint32_t> analysisVisualSubscribers{0};
	std::atomic<uint32_t> analysisGeneration{0};
	std::atomic<bool> visualWatchdogEnabled{false};
	std::atomic<uint32_t> visualHeartbeat{0};
	uint32_t audioVisualHeartbeat = 0;
	int visualLeaseSamples = 0;
	std::atomic<bool> fftScaleDynamic {true};
	std::atomic<bool> showModuleResponseOverlay {false};
	ColorScheme colorScheme = SCHEME_DEFAULT;
	std::atomic<bool> threeColorFftGradient {true};
	std::atomic<bool> legacyVisuals {false};
	std::atomic<bool> useGlShaderRenderer {true};
	std::atomic<bool> lowLatencyVisual {false};
	std::atomic<int> visualWorkerMode {VISUAL_WORKER_INHERIT};
	std::atomic<bool> highResonanceSelfOscEnabled {false};
	std::atomic<bool> softLimitingEnabled {true};
	// Debug-only A/B control. Production processing forces this on whenever
	// Dragon King debug functionality is unavailable.
	std::atomic<bool> nonlinearOversamplingEnabled {true};
	std::atomic<int> modulationQualityMode {MOD_QUALITY_BALANCED};
	int controlUpdateDivision = 16;
	int previewPublishFastDivision = kPreviewPublishFastDivision;
	int previewPublishSlowDivision = kPreviewPublishSlowDivision;
	std::atomic<bool> curveDebugLogging {false};
	std::atomic<bool> perfDebugLogging {false};
	std::atomic<uint64_t> perfAudioProcessRangeMinNs{std::numeric_limits<uint64_t>::max()};
	debug_terminal::AtomicTimingAverage perfAudioProcessRangeAverage;
	std::atomic<uint64_t> perfAudioProcessRangeMaxNs{0};
	std::atomic<float> perfSampleRate{0.f};
	std::atomic<float> perfUiRenderMs{0.f};
	std::atomic<int> perfMode{0};
	std::atomic<bool> perfFastPathEligible{false};
	std::atomic<bool> perfPreviewPitchCvConnected{false};
	uint32_t debugInstanceId = 0;
	double createdUnixTimeSec = 0.0;

	Bifurx();
	~Bifurx() override;
	void resetCircuitStates();
	json_t* dataToJson() override;
	void dataFromJson(json_t* root) override;
	void publishPreviewState(const BifurxPreviewState& state);
	void publishLlTelemetryState(const BifurxLlTelemetryState& state);
	bool readPreviewState(uint32_t lastSeq, BifurxPreviewState* state, double* publishTimeSec, uint32_t* seq);
	bool readLlTelemetryState(uint32_t lastSeq, BifurxLlTelemetryState* state, uint32_t* seq);
	bool copyAnalysisFrame(
		uint32_t lastSeq,
		float* rawInput,
		float* output,
		uint32_t* seq
	);
	void pushAnalysisSample(float rawInputSample, float outputSample);
	void resetAnalysisCapture();
	void subscribeAnalysisVisual();
	void unsubscribeAnalysisVisual();
	void onSampleRateChange(const SampleRateChangeEvent& e) override;
	// Rack's ResetEvent base implementation resets parameters, then dispatches
	// this deprecated hook for module-specific runtime state.
	void onReset() override;
	void process(const ProcessArgs& args) override;
	void processBypass(const ProcessArgs& args) override;
};

} // namespace bifurx
