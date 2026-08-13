#pragma once

#include "plugin.hpp"
#include "DebugTerminalTransport.hpp"
#include "WavePreviewTracer.hpp"
#include <atomic>
#include <cstdint>

struct IntegralFlux : Module {
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

	virtual void getPreviewState(int channel, float& riseTime, float& fallTime, float& curveSigned,
		float& dotXNorm, float& dotYNorm, bool& dotVisible, FunctionShapeMode& shapeMode,
		bool& interactiveRecent, uint32_t& version) const = 0;
	virtual void recordCurvePointReduction(int channel, size_t inputPointCount, size_t outputPointCount) = 0;
	virtual void recordTracerExtraPointReduction(int channel, const WavePreviewTracerCaptureStats& stats) = 0;
	virtual void requestTimingUpdateDiv(int div) = 0;

	virtual std::atomic<bool>& bandlimitedGateOutputsControl() = 0;
	virtual std::atomic<bool>& bandlimitedSignalOutputsControl() = 0;
	virtual std::atomic<bool>& timingInterpolateControl() = 0;
	virtual std::atomic<bool>& previewTracerEnabledControl() = 0;
	virtual std::atomic<int>& requestedTimingUpdateDivControl() = 0;
	virtual std::atomic<int>& previewTracerCacheModeControl() = 0;
	virtual std::atomic<int>& previewRenderModeControl() = 0;
	virtual void setPerfUiRenderMs(float value) = 0;
	virtual uint32_t debugInstanceIdForUi() const = 0;
	virtual void resetAudioPerfSumsForUi() = 0;
	virtual debug_terminal::TimingRangeUs consumeAudioProcessTimingForUi() = 0;
	virtual float consumeCurveReductionAverageForUi(int channel) = 0;
	virtual float consumeTracerReductionAverageForUi(int channel) = 0;
};
