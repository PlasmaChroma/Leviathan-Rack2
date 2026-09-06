#pragma once

#include "plugin.hpp"
#include "DebugTerminalTransport.hpp"
#include "WavePreviewTracer.hpp"
#include <dsp/minblep.hpp>
#include <array>
#include <atomic>
#include <cstdint>
#include <limits>

struct IntegralFlux : Module {
	// UI-thread only. Each module owns the work accumulated between its draws.
	struct UiStepDiagnostics {
		uint32_t previewDirtyRequests = 0;
		uint32_t previewPointRebuilds = 0;
		uint32_t previewTracerCaptures = 0; // Legacy CSV field: capture attempts.
		uint32_t previewTracerAcceptedCaptures = 0;
		uint32_t linearPointDirtyRequests = 0;
		uint32_t shapeGlyphDirtyRequests = 0;
		void recordCapture(bool accepted) {
			++previewTracerCaptures;
			if (accepted) ++previewTracerAcceptedCaptures;
		}
		UiStepDiagnostics consume() {
			const UiStepDiagnostics result = *this;
			*this = UiStepDiagnostics{};
			return result;
		}
	};
	UiStepDiagnostics uiStepDiagnostics;
	enum ParamId {
		ATTENUATE_1_PARAM,
		CYCLE_1_PARAM,
		CYCLE_4_PARAM,
		RISE_1_PARAM,
		RISE_4_PARAM,
		ATTENUATE_2_PARAM,
		FALL_1_PARAM,
		FALL_4_PARAM,
		ATTENUATE_3_PARAM,
		LIN_LOG_1_PARAM,
		LIN_LOG_4_PARAM,
		ATTENUATE_4_PARAM,
		SHAPE_MODE_1_PARAM,
		SHAPE_MODE_4_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		INPUT_1_INPUT,
		INPUT_1_TRIG_INPUT,
		INPUT_2_INPUT,
		INPUT_3_INPUT,
		INPUT_4_TRIG_INPUT,
		INPUT_4_INPUT,
		CH1_RISE_CV_INPUT,
		CH4_RISE_CV_INPUT,
		CH1_BOTH_CV_INPUT,
		CH4_BOTH_CV_INPUT,
		CH1_FALL_CV_INPUT,
		CH4_FALL_CV_INPUT,
		CH1_CYCLE_CV_INPUT,
		CH4_CYCLE_CV_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT_1_OUTPUT,
		OUT_2_OUTPUT,
		OUT_3_OUTPUT,
		OUT_4_OUTPUT,
		EOR_1_OUTPUT,
		CH_1_UNITY_OUTPUT,
		OR_OUT_OUTPUT,
		SUM_OUT_OUTPUT,
		INV_OUT_OUTPUT,
		CH_4_UNITY_OUTPUT,
		EOC_4_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		CYCLE_1_LED_LIGHT,
		CYCLE_4_LED_LIGHT,
		EOR_CH_1_LIGHT,
		LIGHT_UNITY_1_LIGHT,
		LIGHT_UNITY_4_LIGHT,
		EOC_CH_4_LIGHT,
		OR_LED_LIGHT,
		INV_LED_LIGHT,
		LIGHTS_LEN
	};
	enum FunctionShapeMode {
		FUNCTION_SHAPE_MATHS = 0,
		FUNCTION_SHAPE_SHARK_FIN = 1
	};

	static constexpr float LINEAR_SHAPE = 0.33f;
	static constexpr float SHARK_FIN_LINEAR_SHAPE = 0.5f;
	static constexpr float WARP_K_MAX = 40.f;
	static constexpr int WARP_SCALE_SAMPLES = 16;

	static float shapeSignedForMode(float shapeSigned, bool rising, FunctionShapeMode mode);
	static float slopeWarp(float x, float s);
	static float slopeWarpScale(float s);
	static float slopeWarpForMode(float outputNorm, float shapeSigned, bool rising, FunctionShapeMode mode);
	static float slopeWarpScaleForMode(float shapeSigned, bool rising, FunctionShapeMode mode);
	static FunctionShapeMode functionShapeModeFromParam(float value);
	static FunctionShapeMode functionShapeModeFromStoredInt(int value);

	ModuleTeardownTimer teardownTimer {"IntegralFlux"};
	enum OuterPhase {
		// IDLE: no active function cycle unless cycle mode is engaged.
		// RISE/FALL: function-generator mode integrates toward 10V then 0V.
		OUTER_IDLE,
		OUTER_RISE,
		OUTER_FALL
	};

	struct OuterChannelState {
		// Edge detectors for trigger input and momentary cycle button.
		dsp::SchmittTrigger trigEdge;
		dsp::SchmittTrigger cycleButtonEdge;
		// Optional anti-alias compensation for hard output steps.
		dsp::MinBlepGenerator<16, 16> gateBlep;
		dsp::MinBlepGenerator<16, 16> signalBlep;

		OuterPhase phase = OUTER_IDLE;
		// phasePos is a normalized [0..1+] phase accumulator for the active segment.
		float phasePos = 0.f;
		float out = 0.f;
		// Slew warp phase tracking for processUnifiedShapedSlew().
		int slewDir = 0;
		float slewStartOut = 0.f;
		float slewTargetOut = 0.f;
		float slewInvSpan = 0.f;
		bool cycleLatched = false;
		bool gateState = false;
		// Cached warp compensation for the current shape setting.
		bool warpScaleValid = false;
		float cachedShapeSigned = 0.f;
		int cachedShapeMode = FUNCTION_SHAPE_MATHS;
		float cachedRiseWarpScale = 1.f;
		float cachedFallWarpScale = 1.f;
		// Stage-time cache avoids recomputing expensive mapping every sample when unchanged.
		bool stageTimeValid = false;
		float cachedRiseKnob = 0.f;
		float cachedFallKnob = 0.f;
		float cachedShape = 0.f;
		float cachedRiseCv = 0.f;
		float cachedFallCv = 0.f;
		float cachedBothCv = 0.f;
		float cachedRiseTime = 0.01f;
		float cachedFallTime = 0.01f;
		// Active times may interpolate toward cached targets at reduced timing update rates.
		float activeRiseTime = 0.01f;
		float activeFallTime = 0.01f;
		// Trigger acceptance rearm timer for explicit max trigger rate behavior.
		float trigRearmSec = 0.f;
		float riseTimeStep = 0.f;
		float fallTimeStep = 0.f;
		int timeInterpSamplesLeft = 0;
	};

	struct OuterChannelConfig {
		// Per-channel wiring map so CH1/CH4 share one DSP implementation.
		int cycleParam;
		int trigInput;
		int signalInput;
		int riseParam;
		int fallParam;
		int shapeParam;
		int shapeModeParam;
		int riseCvInput;
		int fallCvInput;
		int bothCvInput;
		int cycleCvInput;
		float logShapeTimeScaleLog2;
		float expShapeTimeScaleLog2;
		OuterPhase gateHighPhase;
	};

	struct OuterChannelResult {
		bool cycleOn = false;
		bool previewStatePublished = false;
	};

	struct SlewStepResult {
		float out = 0.f;
		int direction = 0;
	};

	OuterChannelState ch1;
	OuterChannelState ch4;
	struct PreviewSharedState {
		// Lock-free handoff from engine thread -> UI thread.
		// Atomics keep preview independent from DSP timing.
		std::atomic<float> riseTime {0.01f};
		std::atomic<float> fallTime {0.01f};
		std::atomic<float> curveSigned {0.f};
		std::atomic<int> shapeMode {FUNCTION_SHAPE_MATHS};
		std::atomic<float> dotXNorm {0.f};
		std::atomic<float> dotYNorm {0.f};
		std::atomic<uint8_t> dotVisible {0};
		std::atomic<uint8_t> interactiveRecent {0};
		// Even sequence values are stable; odd values indicate an in-progress
		// engine-thread publication. UI readers retry rather than mixing frames.
		std::atomic<uint32_t> version {2};
		std::atomic<uint32_t> dotVersion {0};
	};
	struct PreviewUpdateState {
		float timer = 0.f;
		float interactiveHold = 0.f;
		float lastRiseKnob = 0.f;
		float lastFallKnob = 0.f;
		float lastCurveKnob = 0.33f;
		float lastRiseSent = 0.01f;
		float lastFallSent = 0.01f;
		float lastCurveSent = 0.f;
		int lastShapeMode = FUNCTION_SHAPE_MATHS;
		bool sentOnce = false;
	};
	PreviewSharedState previewCh1;
	PreviewSharedState previewCh4;
	PreviewUpdateState previewUpdateCh1;
	PreviewUpdateState previewUpdateCh4;
	std::atomic<bool> bandlimitedGateOutputs {false};
	std::atomic<bool> bandlimitedSignalOutputs {true};
	std::atomic<uint64_t> perfAudioProcessMinNs {std::numeric_limits<uint64_t>::max()};
	std::atomic<uint64_t> perfAudioProcessMaxNs {0};
	uint32_t perfAudioSampleCounter = 0u;
	std::array<std::atomic<uint64_t>, 2> debugCurvePointsReducedTotal {};
	std::array<std::atomic<uint64_t>, 2> debugCurveReductionSamples {};
	std::array<std::atomic<uint64_t>, 2> debugTracerExtraPointsReducedTotal {};
	std::array<std::atomic<uint64_t>, 2> debugTracerReductionSamples {};
	uint32_t debugInstanceId = 0u;
	int timingUpdateDiv = 1;
	int timingUpdateCounter = 0;
	std::atomic<int> requestedTimingUpdateDiv {1};
	std::atomic<bool> timingInterpolate {true};
	std::atomic<bool> previewTracerEnabled {true};
	std::atomic<int> previewTracerCacheMode {WAVE_PREVIEW_TRACER_SNAPSHOT_CACHE};
	std::atomic<int> previewRenderMode {0};
	// UI light updates are rate-limited to reduce engine overhead.
	float lightUpdateTimer = 0.f;
	float previewDotPublishTimer = 0.f;
	static constexpr float OUTER_V_MIN = 0.f;
	static constexpr float OUTER_V_MAX = 10.2f;
	static constexpr float PARAM_CACHE_EPS = 1e-4f;
	static constexpr float CV_CACHE_EPS = 1e-3f;
	static constexpr float TARGET_EPS = 1e-4f;
	static constexpr float LIGHT_UPDATE_INTERVAL = 1.f / 120.f;
	// Rise/Fall knob taper tuned against hardware low-end behavior.
	static constexpr float KNOB_CURVE_EXP = 1.5f;
	static constexpr float LOG2_TIME_RATIO = 20.930132f;
	// Timing calibration targets at rise=0, fall=0:
	// - Curve at linear point (0.33) ~= 500 Hz
	// - Curve full LOG ~= 80 Hz
	// - Curve full EXP ~= 1.0 kHz
	static constexpr float OUTER_MIN_TIME = 0.001f;
	static constexpr float OUTER_LOG_SHAPE_SCALE = 6.25f;
	static constexpr float OUTER_EXP_SHAPE_SCALE = 0.5f;
	// How strongly Signal IN perturbs the running FG core while cycling/triggered.
	static constexpr float OUTER_INJECT_GAIN = 0.55f;
	// One-pole attraction time constant for FG input perturbation.
	static constexpr float OUTER_INJECT_TAU = 0.0015f;
	// Empirical BOTH CV response fit (hardware-calibrated saturating model).
	static constexpr float BOTH_F_OFF_HZ = 1.93157058f;
	static constexpr float BOTH_F_MAX_HZ = 986.84629918f;
	static constexpr float BOTH_K_OCT_PER_V = 1.10815030f;
	static constexpr float BOTH_V0_V = 4.15514297f;
	static constexpr float BOTH_NEUTRAL_V = -0.05f;
	static constexpr float BOTH_TIME_SCALE_MAX = 64.f;

	static size_t previewDebugChannelIndex(int channel);

	void recordCurvePointReduction(int channel, size_t inputPointCount, size_t outputPointCount);

	void recordTracerExtraPointReduction(int channel, const WavePreviewTracerCaptureStats& stats);

	std::atomic<bool>& bandlimitedGateOutputsControl();

	std::atomic<bool>& bandlimitedSignalOutputsControl();

	std::atomic<bool>& timingInterpolateControl();

	std::atomic<bool>& previewTracerEnabledControl();

	std::atomic<int>& requestedTimingUpdateDivControl();

	std::atomic<int>& previewTracerCacheModeControl();

	std::atomic<int>& previewRenderModeControl();

	uint32_t debugInstanceIdForUi() const;

	debug_terminal::TimingRangeUs consumeAudioProcessTimingForUi();

	float consumeCurveReductionAverageForUi(int channel);

	float consumeTracerReductionAverageForUi(int channel);
	// Hardware-like FG ceilings.
	static constexpr float OUTER_MAX_CYCLE_HZ = 1000.f;
	static constexpr float OUTER_MAX_TRIGGER_HZ = 2000.f;
	static constexpr float CV_OCT_CLAMP = 12.f;
	static constexpr float STAGE_CV_OCT_PER_V = 0.5f;
	static constexpr float PREVIEW_INTERACTIVE_INTERVAL = 1.f / 60.f;
	static constexpr float PREVIEW_CV_INTERVAL = 1.f / 60.f;
	static constexpr float PREVIEW_INTERACTIVE_HOLD = 0.25f;
	static constexpr float PREVIEW_DOT_PUBLISH_INTERVAL = 1.f / 120.f;
	static constexpr int KNOB_CURVE_LUT_SIZE = 4096;
	std::array<float, KNOB_CURVE_LUT_SIZE> knobCurveLut {};
	float cachedInjectSampleTime = -1.f;
	float cachedInjectAlphaBase = 0.f;

	static float attenuverterGain(float knob01);

	float injectAlphaBaseForSampleTime(float sampleTime);

	static float bothHzFromCv(float v);

	static float bothTimeScaleFromCv(float v);

	static void enforceOuterSpeedLimit(float& riseTime, float& fallTime, float minPeriod);

	static float shapeSignedFromKnob(float shape01, float linearShape);

	static float shapeSignedFromKnobForMode(float shape01, FunctionShapeMode mode);

	static float segmentPhaseFromOutputNorm(float outputNorm, float shapeSigned, bool rising);

	static float segmentPhaseFromOutputNormForMode(float outputNorm, float shapeSigned, bool rising, FunctionShapeMode mode);

	static float computeSegPhase(float out, float startOut, float invSpan);

	SlewStepResult processUnifiedShapedSlew(
		OuterChannelState& ch,
		float in,
		float riseTime,
		float fallTime,
		float shapeSigned,
		FunctionShapeMode shapeMode,
		float riseWarpScale,
		float fallWarpScale,
		float dt
	);

	static float phaseCrossingFraction(float phasePos, float dp);

	static void remapPhasePosForStageTimeChange(OuterChannelState& ch, float oldRise, float oldFall, float newRise, float newFall);

	static void insertGateTransition(OuterChannelState& ch, bool newState, float fraction01);

	static void setGateStateImmediate(OuterChannelState& ch, bool newState);

	static void insertSignalTransition(OuterChannelState& ch, float step, float fraction01);

	void applyTimingUpdateDiv(int div);

	void requestTimingUpdateDiv(int div);

	void applyRequestedTimingUpdateDiv();

	void initKnobCurveLut();

	float shapeKnobTimeCurve(float knob) const;

	void updateActiveStageTimes(OuterChannelState& ch);

	void publishPreviewState(PreviewSharedState& shared, float riseTime, float fallTime, float curveSigned,
		FunctionShapeMode shapeMode, bool interactiveRecent);

	void publishPreviewDot(PreviewSharedState& shared, bool visible, float xNorm, float yNorm);

	static bool previewChangedMeaningfully(float riseNow, float risePrev, float fallNow, float fallPrev, float curveNow, float curvePrev);

	bool updatePreviewChannel(
		PreviewSharedState& shared,
		PreviewUpdateState& state,
		float riseKnob,
		float fallKnob,
		float curveKnob,
		float riseTime,
		float fallTime,
		float curveSigned,
		FunctionShapeMode shapeMode,
		float dt
	);

	void getPreviewState(int channel, float& riseTime, float& fallTime, float& curveSigned, float& dotXNorm,
		float& dotYNorm, bool& dotVisible, FunctionShapeMode& shapeMode, bool& interactiveRecent, uint32_t& version) const;

	float computeShapeTimeScale(float shape, FunctionShapeMode mode, float logScaleLog2, float expScaleLog2) const;

	float computeStageTime(
		float knob,
		float stageCv,
		float bothScale,
		float shapeTimeScale
	) const;

	void triggerOuterFunction(OuterChannelState& ch);

	OuterChannelResult processOuterChannel(
		const ProcessArgs& args,
		OuterChannelState& ch,
		const OuterChannelConfig& cfg,
		PreviewSharedState& previewShared,
		PreviewUpdateState& previewUpdateState,
		bool timingTick,
		bool bandlimitedSignalEnabled,
		bool bandlimitedGateEnabled,
		bool timingInterpolateEnabled,
		float injectAlphaBase
	);

	IntegralFlux();

	~IntegralFlux() override;

	json_t* dataToJson() override;

	void dataFromJson(json_t* rootJ) override;

	void process(const ProcessArgs& args) override;
};
