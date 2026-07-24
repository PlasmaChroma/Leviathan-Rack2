#include "IntegralFlux.hpp"
#include "DebugTerminalTransport.hpp"
#include "MathHelpers.hpp"
#include <dsp/minblep.hpp>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <limits>

namespace {
std::atomic<uint32_t> gIntegralFluxDebugInstanceCounter {1u};
}

float IntegralFlux::shapeSignedForMode(float shapeSigned, bool rising, FunctionShapeMode mode) {
	if (mode == FUNCTION_SHAPE_SHARK_FIN) {
		return rising ? -shapeSigned : shapeSigned;
	}
	return shapeSigned;
}

float IntegralFlux::slopeWarp(float x, float s) {
	x = clamp(x, 0.f, 1.f);
	float u = std::fabs(s);
	if (u < 1e-6f) {
		return 1.f;
	}
	float k = WARP_K_MAX * u;
	float x2 = x * x;
	if (s < 0.f) {
		return 1.f / (1.f + k * x2);
	}
	return 1.f + k * x2;
}

float IntegralFlux::slopeWarpScale(float s) {
	if (std::fabs(s) < 1e-6f) {
		return 1.f;
	}
	float sum = 0.f;
	for (int i = 0; i < WARP_SCALE_SAMPLES; ++i) {
		float xi = (i + 0.5f) / float(WARP_SCALE_SAMPLES);
		sum += 1.f / slopeWarp(xi, s);
	}
	return sum / float(WARP_SCALE_SAMPLES);
}

float IntegralFlux::slopeWarpForMode(float outputNorm, float shapeSigned, bool rising, FunctionShapeMode mode) {
	return slopeWarp(outputNorm, shapeSignedForMode(shapeSigned, rising, mode));
}

float IntegralFlux::slopeWarpScaleForMode(float shapeSigned, bool rising, FunctionShapeMode mode) {
	return slopeWarpScale(shapeSignedForMode(shapeSigned, rising, mode));
}

IntegralFlux::FunctionShapeMode IntegralFlux::functionShapeModeFromParam(float value) {
	return value >= 0.5f ? FUNCTION_SHAPE_MATHS : FUNCTION_SHAPE_SHARK_FIN;
}

IntegralFlux::FunctionShapeMode IntegralFlux::functionShapeModeFromStoredInt(int value) {
	return value == FUNCTION_SHAPE_SHARK_FIN ? FUNCTION_SHAPE_SHARK_FIN : FUNCTION_SHAPE_MATHS;
}

struct IntegralFluxImpl : IntegralFlux {
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
		std::atomic<uint32_t> version {1};
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
	std::atomic<uint64_t> perfAudioSampledCount {0};
	std::atomic<uint64_t> perfAudioProcessNs {0};
	std::atomic<uint64_t> perfAudioProcessMinNs {std::numeric_limits<uint64_t>::max()};
	std::atomic<uint64_t> perfAudioProcessMaxNs {0};
	std::atomic<float> perfUiRenderMs {0.f};
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
	std::atomic<int> previewTracerCacheMode {WAVE_PREVIEW_TRACER_CURVE_CACHE};
	std::atomic<int> previewRenderMode {0};
	// UI light updates are rate-limited to reduce engine overhead.
	float lightUpdateTimer = 0.f;
	float previewDotPublishTimer = 0.f;
	static constexpr float LINEAR_SHAPE = 0.33f;
	static constexpr float OUTER_V_MIN = 0.f;
	static constexpr float OUTER_V_MAX = 10.2f;
	static constexpr float WARP_K_MAX = 40.f;
	static constexpr int WARP_SCALE_SAMPLES = 16;
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

	static size_t previewDebugChannelIndex(int channel) {
		return channel == 4 ? 1u : 0u;
	}

	void recordCurvePointReduction(int channel, size_t inputPointCount, size_t outputPointCount) override {
		if (!isDragonKingDebugEnabled()) {
			return;
		}
		const size_t index = previewDebugChannelIndex(channel);
		debugCurvePointsReducedTotal[index].fetch_add(inputPointCount - std::min(inputPointCount, outputPointCount),
		                                              std::memory_order_relaxed);
		debugCurveReductionSamples[index].fetch_add(1u, std::memory_order_relaxed);
	}

	void recordTracerExtraPointReduction(int channel, const WavePreviewTracerCaptureStats& stats) override {
		if (!isDragonKingDebugEnabled() || !stats.captured) {
			return;
		}
		const size_t index = previewDebugChannelIndex(channel);
		debugTracerExtraPointsReducedTotal[index].fetch_add(
			stats.simplifiedPointCount - std::min(stats.simplifiedPointCount, stats.compactedPointCount),
			std::memory_order_relaxed);
		debugTracerReductionSamples[index].fetch_add(1u, std::memory_order_relaxed);
	}

	std::atomic<bool>& bandlimitedGateOutputsControl() override {
		return bandlimitedGateOutputs;
	}

	std::atomic<bool>& bandlimitedSignalOutputsControl() override {
		return bandlimitedSignalOutputs;
	}

	std::atomic<bool>& timingInterpolateControl() override {
		return timingInterpolate;
	}

	std::atomic<bool>& previewTracerEnabledControl() override {
		return previewTracerEnabled;
	}

	std::atomic<int>& requestedTimingUpdateDivControl() override {
		return requestedTimingUpdateDiv;
	}

	std::atomic<int>& previewTracerCacheModeControl() override {
		return previewTracerCacheMode;
	}

	std::atomic<int>& previewRenderModeControl() override {
		return previewRenderMode;
	}

	void setPerfUiRenderMs(float value) override {
		perfUiRenderMs.store(std::max(0.f, value), std::memory_order_relaxed);
	}

	uint32_t debugInstanceIdForUi() const override {
		return debugInstanceId;
	}

	void resetAudioPerfSumsForUi() override {
		perfAudioSampledCount.exchange(0, std::memory_order_acq_rel);
		perfAudioProcessNs.exchange(0, std::memory_order_acq_rel);
	}

	debug_terminal::TimingRangeUs consumeAudioProcessTimingForUi() override {
		return debug_terminal::consumeAudioProcessTiming(perfAudioProcessMinNs, perfAudioProcessMaxNs);
	}

	float consumeCurveReductionAverageForUi(int channel) override {
		const size_t index = previewDebugChannelIndex(channel);
		const uint64_t totalValue = debugCurvePointsReducedTotal[index].exchange(0u, std::memory_order_acq_rel);
		const uint64_t sampleCount = debugCurveReductionSamples[index].exchange(0u, std::memory_order_acq_rel);
		return sampleCount > 0u ? float(double(totalValue) / double(sampleCount)) : 0.f;
	}

	float consumeTracerReductionAverageForUi(int channel) override {
		const size_t index = previewDebugChannelIndex(channel);
		const uint64_t totalValue = debugTracerExtraPointsReducedTotal[index].exchange(0u, std::memory_order_acq_rel);
		const uint64_t sampleCount = debugTracerReductionSamples[index].exchange(0u, std::memory_order_acq_rel);
		return sampleCount > 0u ? float(double(totalValue) / double(sampleCount)) : 0.f;
	}
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
	static constexpr float SHARK_FIN_LINEAR_SHAPE = 0.5f;
	std::array<float, KNOB_CURVE_LUT_SIZE> knobCurveLut {};
	float cachedInjectSampleTime = -1.f;
	float cachedInjectAlphaBase = 0.f;

	static float attenuverterGain(float knob01) {
		// Noon = 0, CCW = negative, CW = positive.
		return clamp(knob01, 0.f, 1.f) * 2.f - 1.f;
	}

	float injectAlphaBaseForSampleTime(float sampleTime) {
		if (std::fabs(sampleTime - cachedInjectSampleTime) > 1e-12f) {
			cachedInjectSampleTime = sampleTime;
			cachedInjectAlphaBase = OUTER_INJECT_GAIN * clamp(1.f - std::exp(-sampleTime / OUTER_INJECT_TAU), 0.f, 1.f);
		}
		return cachedInjectAlphaBase;
	}

	static float bothHzFromCv(float v) {
		float x = BOTH_K_OCT_PER_V * (v - BOTH_V0_V);
		float r = rack::dsp::exp2_taylor5(x);
		return BOTH_F_OFF_HZ + BOTH_F_MAX_HZ * (r / (1.f + r));
	}

	static float bothTimeScaleFromCv(float v) {
		float vs = levi_math::softLimit(v, 8.f);
		float f = bothHzFromCv(vs);
		// Neutral reference is constant for the life of the module, compute once.
		static const float neutralHz = bothHzFromCv(BOTH_NEUTRAL_V);
		float scale = neutralHz / std::max(f, 1e-6f);
		return clamp(scale, 1.f / BOTH_TIME_SCALE_MAX, BOTH_TIME_SCALE_MAX);
	}

	static void enforceOuterSpeedLimit(float& riseTime, float& fallTime, float minPeriod) {
		riseTime = std::max(riseTime, 1e-6f);
		fallTime = std::max(fallTime, 1e-6f);
		float period = riseTime + fallTime;
		if (period < minPeriod) {
			float scale = minPeriod / std::max(period, 1e-9f);
			riseTime *= scale;
			fallTime *= scale;
		}
	}

	static float shapeSignedFromKnob(float shape01, float linearShape) {
		shape01 = clamp(shape01, 0.f, 1.f);
		linearShape = clamp(linearShape, 1e-4f, 1.f - 1e-4f);
		if (shape01 < linearShape) {
			return (shape01 - linearShape) / linearShape;
		}
		if (shape01 > linearShape) {
			return (shape01 - linearShape) / (1.f - linearShape);
		}
		return 0.f;
	}

	static float shapeSignedFromKnobForMode(float shape01, FunctionShapeMode mode) {
		return shapeSignedFromKnob(shape01, mode == FUNCTION_SHAPE_SHARK_FIN ? SHARK_FIN_LINEAR_SHAPE : LINEAR_SHAPE);
	}

	static FunctionShapeMode functionShapeModeFromParam(float value) {
		return value >= 0.5f ? FUNCTION_SHAPE_MATHS : FUNCTION_SHAPE_SHARK_FIN;
	}

	static FunctionShapeMode functionShapeModeFromStoredInt(int value) {
		return value == FUNCTION_SHAPE_SHARK_FIN ? FUNCTION_SHAPE_SHARK_FIN : FUNCTION_SHAPE_MATHS;
	}

	static float slopeWarp(float x, float s) {
		// Differential warp used by both function-generator and slew modes.
		// We shape local slope, then normalize total travel time with slopeWarpScale().
		x = clamp(x, 0.f, 1.f);
		float u = std::fabs(s);
		if (u < 1e-6f) {
			return 1.f;
		}
		float k = WARP_K_MAX * u;
		float x2 = x * x;
		if (s < 0.f) {
			// LOG: fast near 0V, slow near top.
			return 1.f / (1.f + k * x2);
		}
		// EXP: slow near 0V, fast near top.
		return 1.f + k * x2;
	}

	static float slopeWarpScale(float s) {
		// Numerically estimate scale so different curve settings keep similar segment duration.
		// Integrates reciprocal slope over [0..1] with a small fixed sample count.
		if (std::fabs(s) < 1e-6f) {
			return 1.f;
		}
		float sum = 0.f;
		for (int i = 0; i < WARP_SCALE_SAMPLES; ++i) {
			float xi = (i + 0.5f) / float(WARP_SCALE_SAMPLES);
			sum += 1.f / slopeWarp(xi, s);
		}
		return sum / float(WARP_SCALE_SAMPLES);
	}

	static float shapeSignedForMode(float shapeSigned, bool rising, FunctionShapeMode mode) {
		if (mode == FUNCTION_SHAPE_SHARK_FIN) {
			return rising ? -shapeSigned : shapeSigned;
		}
		return shapeSigned;
	}

	static float slopeWarpForMode(float outputNorm, float shapeSigned, bool rising, FunctionShapeMode mode) {
		return slopeWarp(outputNorm, shapeSignedForMode(shapeSigned, rising, mode));
	}

	static float slopeWarpScaleForMode(float shapeSigned, bool rising, FunctionShapeMode mode) {
		return slopeWarpScale(shapeSignedForMode(shapeSigned, rising, mode));
	}

	static float segmentPhaseFromOutputNorm(float outputNorm, float shapeSigned, bool rising) {
		outputNorm = clamp(outputNorm, 0.f, 1.f);
		if (std::fabs(shapeSigned) < 1e-6f) {
			return rising ? outputNorm : (1.f - outputNorm);
		}
		const float start = rising ? 0.f : outputNorm;
		const float end = rising ? outputNorm : 1.f;
		const float span = std::max(end - start, 0.f);
		float partialSum = 0.f;
		for (int i = 0; i < WARP_SCALE_SAMPLES; ++i) {
			float t = (float(i) + 0.5f) / float(WARP_SCALE_SAMPLES);
			partialSum += 1.f / slopeWarp(start + span * t, shapeSigned);
		}
		const float partialIntegral = span * partialSum / float(WARP_SCALE_SAMPLES);
		const float totalIntegral = std::max(slopeWarpScale(shapeSigned), 1e-6f);
		return clamp(partialIntegral / totalIntegral, 0.f, 1.f);
	}

	static float segmentPhaseFromOutputNormForMode(float outputNorm, float shapeSigned, bool rising, FunctionShapeMode mode) {
		outputNorm = clamp(outputNorm, 0.f, 1.f);
		if (mode == FUNCTION_SHAPE_MATHS) {
			return segmentPhaseFromOutputNorm(outputNorm, shapeSigned, rising);
		}
		return segmentPhaseFromOutputNorm(outputNorm, shapeSignedForMode(shapeSigned, rising, mode), rising);
	}

	static float computeSegPhase(float out, float startOut, float invSpan) {
		if (std::fabs(invSpan) < 1e-9f) {
			return 1.f;
		}
		float phase = (out - startOut) * invSpan;
		return clamp(phase, 0.f, 1.f);
	}

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
	) {
		// Shared "core limiter" path when the outer channel is acting as a slew on input signal.
		// This reuses the same curve family used by free-running function generation.
		SlewStepResult result;
		float out = ch.out;
		float prevTargetOut = ch.slewTargetOut;
		float delta = in - out;
		if (std::fabs(delta) <= TARGET_EPS) {
			float targetDelta = in - prevTargetOut;
			if (targetDelta > TARGET_EPS) {
				result.direction = 1;
			}
			else if (targetDelta < -TARGET_EPS) {
				result.direction = -1;
			}
			else {
				result.direction = ch.slewDir;
			}
			ch.slewDir = 0;
			result.out = out;
			return result;
		}
		int dir = (delta > 0.f) ? 1 : -1;
		result.direction = dir;
		bool dirChanged = (ch.slewDir != dir);
		bool targetChanged = (std::fabs(in - ch.slewTargetOut) > TARGET_EPS);
		if (ch.slewDir == 0 || dirChanged || targetChanged) {
			ch.slewDir = dir;
			ch.slewStartOut = out;
			ch.slewTargetOut = in;
			float span = ch.slewTargetOut - ch.slewStartOut;
			ch.slewInvSpan = (std::fabs(span) < 1e-6f) ? 0.f : (1.f / span);
		}

		float stageTime = (delta > 0.f) ? riseTime : fallTime;
		stageTime = std::max(stageTime, 1e-6f);
		const bool rising = delta > 0.f;
		float range = OUTER_V_MAX - OUTER_V_MIN;
		float outputNorm = clamp((out - OUTER_V_MIN) / std::max(range, 1e-6f), 0.f, 1.f);
		float scale = rising ? riseWarpScale : fallWarpScale;
		float warp = slopeWarpForMode(outputNorm, shapeSigned, rising, shapeMode);
		float dp = clamp(dt / stageTime, 0.f, 0.5f);
		float step = dp * warp * scale * range;

		float prevOut = out;
		out += (delta > 0.f) ? step : -step;
		if ((in - prevOut) * (in - out) < 0.f) {
			out = in;
			ch.slewDir = 0;
		}
		else {
			ch.slewDir = dir;
		}
		result.out = out;
		return result;
	}

	static float phaseCrossingFraction(float phasePos, float dp) {
		// Returns the within-sample crossing point for BLEP insertion.
		// 1.0 means transition near end-of-sample, 0.0 near beginning.
		if (dp <= 1e-9f) {
			return 1.f;
		}
		return clamp(1.f - ((phasePos - 1.f) / dp), 0.f, 1.f);
	}

	static void remapPhasePosForStageTimeChange(OuterChannelState& ch, float oldRise, float oldFall, float newRise, float newFall) {
		if (ch.phase == OUTER_IDLE) {
			return;
		}
		oldRise = std::max(oldRise, 1e-6f);
		oldFall = std::max(oldFall, 1e-6f);
		newRise = std::max(newRise, 1e-6f);
		newFall = std::max(newFall, 1e-6f);
		const float oldTotal = oldRise + oldFall;
		const float newTotal = newRise + newFall;
		if (ch.phase == OUTER_RISE) {
			const float dotX = clamp((ch.phasePos * oldRise) / oldTotal, 0.f, 1.f);
			ch.phasePos = clamp((dotX * newTotal) / newRise, 0.f, 2.f);
		}
		else if (ch.phase == OUTER_FALL) {
			const float dotX = clamp((oldRise + ch.phasePos * oldFall) / oldTotal, 0.f, 1.f);
			ch.phasePos = clamp(((dotX * newTotal) - newRise) / newFall, 0.f, 2.f);
		}
	}

	static void insertGateTransition(OuterChannelState& ch, bool newState, float fraction01) {
		if (newState == ch.gateState) {
			return;
		}
		float f = clamp(fraction01, 1e-6f, 1.f);
		// Rack MinBLEP expects discontinuity position in [-1, 0] samples from current sample.
		float p = f - 1.f;
		float step = newState ? 10.f : -10.f;
		ch.gateBlep.insertDiscontinuity(p, step);
		ch.gateState = newState;
	}

	static void setGateStateImmediate(OuterChannelState& ch, bool newState) {
		ch.gateState = newState;
	}

	static void insertSignalTransition(OuterChannelState& ch, float step, float fraction01) {
		if (std::fabs(step) < 1e-9f) {
			return;
		}
		float f = clamp(fraction01, 1e-6f, 1.f);
		float p = f - 1.f;
		ch.signalBlep.insertDiscontinuity(p, step);
	}

	void applyTimingUpdateDiv(int div) {
		// Changing update rate invalidates cached timing so channels resync immediately.
		timingUpdateDiv = std::max(1, div);
		timingUpdateCounter = 0;
		ch1.stageTimeValid = false;
		ch4.stageTimeValid = false;
	}

	void requestTimingUpdateDiv(int div) override {
		requestedTimingUpdateDiv.store(std::max(1, div), std::memory_order_relaxed);
	}

	void applyRequestedTimingUpdateDiv() {
		const int requested = requestedTimingUpdateDiv.load(std::memory_order_relaxed);
		if (requested != timingUpdateDiv) {
			applyTimingUpdateDiv(requested);
		}
	}

	void initKnobCurveLut() {
		// Precompute knob taper to trade tiny memory for lower per-sample CPU.
		for (int i = 0; i < KNOB_CURVE_LUT_SIZE; ++i) {
			float x = float(i) / float(KNOB_CURVE_LUT_SIZE - 1);
			knobCurveLut[i] = std::pow(x, KNOB_CURVE_EXP);
		}
	}

	float shapeKnobTimeCurve(float knob) const {
		// Linear interpolation in LUT avoids powf() in the hot path.
		knob = clamp(knob, 0.f, 1.f);
		float idx = knob * float(KNOB_CURVE_LUT_SIZE - 1);
		int i0 = int(idx);
		int i1 = std::min(i0 + 1, KNOB_CURVE_LUT_SIZE - 1);
		float t = idx - float(i0);
		float v0 = knobCurveLut[i0];
		float v1 = knobCurveLut[i1];
		return v0 + (v1 - v0) * t;
	}

	void updateActiveStageTimes(OuterChannelState& ch) {
		// Optional de-zipper when timing is updated at control rate (/4, /8, ...).
		if (ch.timeInterpSamplesLeft > 0) {
			ch.activeRiseTime += ch.riseTimeStep;
			ch.activeFallTime += ch.fallTimeStep;
			ch.timeInterpSamplesLeft--;
			if (ch.timeInterpSamplesLeft == 0) {
				ch.activeRiseTime = ch.cachedRiseTime;
				ch.activeFallTime = ch.cachedFallTime;
			}
		}
	}

	void publishPreviewState(PreviewSharedState& shared, float riseTime, float fallTime, float curveSigned,
		FunctionShapeMode shapeMode, bool interactiveRecent) {
		// Batched atomic publish: UI only rebuilds when version increments.
		shared.riseTime.store(riseTime, std::memory_order_relaxed);
		shared.fallTime.store(fallTime, std::memory_order_relaxed);
		shared.curveSigned.store(curveSigned, std::memory_order_relaxed);
		shared.shapeMode.store(int(shapeMode), std::memory_order_relaxed);
		shared.interactiveRecent.store(interactiveRecent ? uint8_t(1) : uint8_t(0), std::memory_order_relaxed);
		shared.version.fetch_add(1, std::memory_order_relaxed);
	}

	void publishPreviewDot(PreviewSharedState& shared, bool visible, float xNorm, float yNorm) {
		shared.dotXNorm.store(clamp(xNorm, 0.f, 1.f), std::memory_order_relaxed);
		shared.dotYNorm.store(clamp(yNorm, 0.f, 1.f), std::memory_order_relaxed);
		shared.dotVisible.store(visible ? uint8_t(1) : uint8_t(0), std::memory_order_relaxed);
	}

	static bool previewChangedMeaningfully(float riseNow, float risePrev, float fallNow, float fallPrev, float curveNow, float curvePrev) {
		float riseAbs = std::fabs(riseNow - risePrev);
		float fallAbs = std::fabs(fallNow - fallPrev);
		float riseRel = riseAbs / std::max(std::fabs(risePrev), 1e-6f);
		float fallRel = fallAbs / std::max(std::fabs(fallPrev), 1e-6f);
		return riseAbs > 1e-4f || fallAbs > 1e-4f || riseRel > 0.01f || fallRel > 0.01f || std::fabs(curveNow - curvePrev) > 0.005f;
	}

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
	) {
		// Preview refresh runs slower than audio and only pushes updates when meaningful.
		bool knobChanged = std::fabs(riseKnob - state.lastRiseKnob) > PARAM_CACHE_EPS
			|| std::fabs(fallKnob - state.lastFallKnob) > PARAM_CACHE_EPS
			|| std::fabs(curveKnob - state.lastCurveKnob) > PARAM_CACHE_EPS;
		bool modeChanged = int(shapeMode) != state.lastShapeMode;
		state.lastRiseKnob = riseKnob;
		state.lastFallKnob = fallKnob;
		state.lastCurveKnob = curveKnob;
		state.lastShapeMode = int(shapeMode);

		if (knobChanged || modeChanged) {
			state.interactiveHold = PREVIEW_INTERACTIVE_HOLD;
			// Push an immediate preview refresh on manual interaction.
			state.timer = PREVIEW_INTERACTIVE_INTERVAL;
		}
		if (state.interactiveHold > 0.f) {
			state.interactiveHold = std::max(0.f, state.interactiveHold - dt);
		}
		state.timer += dt;

		float interval = (state.interactiveHold > 0.f) ? PREVIEW_INTERACTIVE_INTERVAL : PREVIEW_CV_INTERVAL;
		bool changed = knobChanged || modeChanged || !state.sentOnce || previewChangedMeaningfully(
			riseTime, state.lastRiseSent,
			fallTime, state.lastFallSent,
			curveSigned, state.lastCurveSent
		);
		if (changed && state.timer >= interval) {
			publishPreviewState(shared, riseTime, fallTime, curveSigned, shapeMode, state.interactiveHold > 0.f);
			state.lastRiseSent = riseTime;
			state.lastFallSent = fallTime;
			state.lastCurveSent = curveSigned;
			state.sentOnce = true;
			state.timer = 0.f;
			return true;
		}
		return false;
	}

	void getPreviewState(int channel, float& riseTime, float& fallTime, float& curveSigned, float& dotXNorm,
		float& dotYNorm, bool& dotVisible, FunctionShapeMode& shapeMode, bool& interactiveRecent, uint32_t& version) const override {
		const PreviewSharedState& shared = (channel == 4) ? previewCh4 : previewCh1;
		riseTime = shared.riseTime.load(std::memory_order_relaxed);
		fallTime = shared.fallTime.load(std::memory_order_relaxed);
		curveSigned = shared.curveSigned.load(std::memory_order_relaxed);
		shapeMode = functionShapeModeFromStoredInt(shared.shapeMode.load(std::memory_order_relaxed));
		dotXNorm = shared.dotXNorm.load(std::memory_order_relaxed);
		dotYNorm = shared.dotYNorm.load(std::memory_order_relaxed);
		dotVisible = shared.dotVisible.load(std::memory_order_relaxed) != 0;
		interactiveRecent = shared.interactiveRecent.load(std::memory_order_relaxed) != 0;
		version = shared.version.load(std::memory_order_relaxed);
	}

	float computeShapeTimeScale(float shape, FunctionShapeMode mode, float logScaleLog2, float expScaleLog2) const {
		// Shape knob (log/lin/exp) contributes a multiplicative time factor.
		// We interpolate in log2 domain so scaling stays perceptually smooth.
		shape = clamp(shape, 0.f, 1.f);
		const float linearShape = mode == FUNCTION_SHAPE_SHARK_FIN ? SHARK_FIN_LINEAR_SHAPE : LINEAR_SHAPE;
		if (shape < linearShape) {
			float t = shape / linearShape;
			return rack::dsp::exp2_taylor5((1.f - t) * logScaleLog2);
		}
		if (shape > linearShape) {
			float t = (shape - linearShape) / (1.f - linearShape);
			return rack::dsp::exp2_taylor5(t * expScaleLog2);
		}
		return 1.f;
	}

	float computeStageTime(
		float knob,
		float stageCv,
		float bothScale,
		float shapeTimeScale
	) const {
		// Shared CH1/CH4 calibration:
		// - min dials at curve minimum ~80 Hz
		// - min dials at curve maximum ~1.0 kHz
		const float minTime = OUTER_MIN_TIME;
		// Absolute floor allows EXP/positive CV to run faster than the linear baseline.
		const float absoluteMinTime = 0.0001f;
		const float maxTime = 1500.f;
		// Use a curved knob law so noon timing tracks measured hardware behavior.
		// With this exponent, knob=0.5 is ~23x slower than knob=0 (not ~1400x).
		float knobShaped = shapeKnobTimeCurve(knob);
		// Knob controls a wide exponential span in seconds.
		float t = minTime * rack::dsp::exp2_taylor5(knobShaped * LOG2_TIME_RATIO);

		// Rise/Fall CV applies in log-time domain:
		// +V -> longer (slower), -V -> shorter (faster).
		float stageCvSoft = levi_math::softLimit(stageCv, 8.f);
		float stageOct = clamp(stageCvSoft * STAGE_CV_OCT_PER_V, -CV_OCT_CLAMP, CV_OCT_CLAMP);
		t *= rack::dsp::exp2_taylor5(stageOct);

		// BOTH and curve-shape scaling are already multiplicative factors.
		t *= bothScale;
		t *= shapeTimeScale;

		return clamp(t, absoluteMinTime, maxTime);
	}

	void triggerOuterFunction(OuterChannelState& ch) {
		// Trigger always starts a fresh rise phase.
		ch.phase = OUTER_RISE;
		ch.phasePos = 0.f;
	}

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
	) {
		// This routine handles both behaviors of an outer channel:
		// 1) function generator when cycling/triggered
		// 2) slew limiter when a signal is patched and phase is idle
		float dt = args.sampleTime;
		ch.trigRearmSec = std::max(0.f, ch.trigRearmSec - dt);

		if (ch.cycleButtonEdge.process(params[cfg.cycleParam].getValue())) {
			ch.cycleLatched = !ch.cycleLatched;
		}

		bool cycleCvHigh = inputs[cfg.cycleCvInput].getVoltage() >= 2.5f;
		bool cycleOn = ch.cycleLatched || cycleCvHigh;
		bool gateWasHigh = ch.gateState;

		bool trigRise = ch.trigEdge.process(inputs[cfg.trigInput].getVoltage());
		bool trigAccepted = false;
		bool retriggerFromFall = false;
		if (trigRise && ch.trigRearmSec <= 0.f && ch.phase != OUTER_RISE) {
			retriggerFromFall = (ch.phase == OUTER_FALL);
			triggerOuterFunction(ch);
			if (retriggerFromFall) {
				// Manual behavior: trigger can reset only during FALL, restarting from cycle start.
				float prevOut = ch.out;
				ch.out = OUTER_V_MIN;
				if (bandlimitedSignalEnabled) {
					insertSignalTransition(ch, ch.out - prevOut, 1e-6f);
				}
			}
			trigAccepted = true;
			ch.trigRearmSec = 1.f / std::max(OUTER_MAX_TRIGGER_HZ, 1.f);
		}

		float riseKnob = params[cfg.riseParam].getValue();
		float fallKnob = params[cfg.fallParam].getValue();
		float shape = params[cfg.shapeParam].getValue();
		FunctionShapeMode shapeMode = functionShapeModeFromParam(params[cfg.shapeModeParam].getValue());
		float riseCv = inputs[cfg.riseCvInput].getVoltage();
		float fallCv = inputs[cfg.fallCvInput].getVoltage();
		float bothCv = inputs[cfg.bothCvInput].getVoltage();
		bool shapeKnobChanged = std::fabs(shape - ch.cachedShape) > PARAM_CACHE_EPS;
		bool shapeModeChanged = int(shapeMode) != ch.cachedShapeMode;
		if (!ch.stageTimeValid || timingTick) {
			// Recompute times only when a relevant source changed.
			bool stageTimeDirty = !ch.stageTimeValid
				|| std::fabs(riseKnob - ch.cachedRiseKnob) > PARAM_CACHE_EPS
				|| std::fabs(fallKnob - ch.cachedFallKnob) > PARAM_CACHE_EPS
				|| std::fabs(shape - ch.cachedShape) > PARAM_CACHE_EPS
				|| shapeModeChanged
				|| std::fabs(riseCv - ch.cachedRiseCv) > CV_CACHE_EPS
				|| std::fabs(fallCv - ch.cachedFallCv) > CV_CACHE_EPS
				|| std::fabs(bothCv - ch.cachedBothCv) > CV_CACHE_EPS;
			if (stageTimeDirty) {
				float bothScale = bothTimeScaleFromCv(bothCv);
				float shapeTimeScale = computeShapeTimeScale(shape, shapeMode, cfg.logShapeTimeScaleLog2, cfg.expShapeTimeScaleLog2);
				ch.cachedRiseTime = computeStageTime(
					riseKnob,
					riseCv,
					bothScale,
					shapeTimeScale
				);
				ch.cachedFallTime = computeStageTime(
					fallKnob,
					fallCv,
					bothScale,
					shapeTimeScale
				);
				ch.cachedRiseKnob = riseKnob;
				ch.cachedFallKnob = fallKnob;
				ch.cachedShape = shape;
				ch.cachedRiseCv = riseCv;
				ch.cachedFallCv = fallCv;
				ch.cachedBothCv = bothCv;
				if (!ch.stageTimeValid) {
					// Cold start: avoid interpolation artifacts.
					ch.activeRiseTime = ch.cachedRiseTime;
					ch.activeFallTime = ch.cachedFallTime;
					ch.riseTimeStep = 0.f;
					ch.fallTimeStep = 0.f;
					ch.timeInterpSamplesLeft = 0;
				}
				else if (timingInterpolateEnabled && timingUpdateDiv > 1) {
					// Interpolate timing across N samples to avoid sample-and-hold zipper tone.
					ch.riseTimeStep = (ch.cachedRiseTime - ch.activeRiseTime) / float(timingUpdateDiv);
					ch.fallTimeStep = (ch.cachedFallTime - ch.activeFallTime) / float(timingUpdateDiv);
					ch.timeInterpSamplesLeft = timingUpdateDiv;
				}
				else {
					ch.activeRiseTime = ch.cachedRiseTime;
					ch.activeFallTime = ch.cachedFallTime;
					ch.riseTimeStep = 0.f;
					ch.fallTimeStep = 0.f;
					ch.timeInterpSamplesLeft = 0;
				}
				ch.stageTimeValid = true;
			}
		}
		float prevRiseTime = ch.activeRiseTime;
		float prevFallTime = ch.activeFallTime;
		updateActiveStageTimes(ch);
		float riseTime = ch.activeRiseTime;
		float fallTime = ch.activeFallTime;
		if (ch.phase != OUTER_IDLE
			&& (std::fabs(riseTime - prevRiseTime) > 1e-6f || std::fabs(fallTime - prevFallTime) > 1e-6f)) {
			remapPhasePosForStageTimeChange(ch, prevRiseTime, prevFallTime, riseTime, fallTime);
		}
		bool fgActive = (ch.phase != OUTER_IDLE);
		if (trigAccepted) {
			// External trigger may run faster than self-cycle, but with an explicit ceiling.
			enforceOuterSpeedLimit(riseTime, fallTime, 1.f / std::max(OUTER_MAX_TRIGGER_HZ, 1.f));
		}
		else if (cycleOn) {
			// Self-cycle path is held to the lower hardware-like ceiling.
			enforceOuterSpeedLimit(riseTime, fallTime, 1.f / std::max(OUTER_MAX_CYCLE_HZ, 1.f));
		}
		else if (fgActive) {
			// One-shot/triggered FG segments use trigger-domain ceiling when not cycling.
			enforceOuterSpeedLimit(riseTime, fallTime, 1.f / std::max(OUTER_MAX_TRIGGER_HZ, 1.f));
		}
		float shapeSigned = shapeSignedFromKnobForMode(shape, shapeMode);
		bool previewStatePublished = updatePreviewChannel(
			previewShared,
			previewUpdateState,
			riseKnob,
			fallKnob,
			shape,
			riseTime,
			fallTime,
			shapeSigned,
			shapeMode,
			dt
		);
		if (!ch.warpScaleValid
			|| std::fabs(shapeSigned - ch.cachedShapeSigned) > 1e-4f
			|| shapeModeChanged) {
			// Curve normalization changes only when shape changes.
			ch.cachedShapeSigned = shapeSigned;
			ch.cachedShapeMode = int(shapeMode);
			ch.cachedRiseWarpScale = slopeWarpScaleForMode(shapeSigned, true, shapeMode);
			ch.cachedFallWarpScale = slopeWarpScaleForMode(shapeSigned, false, shapeMode);
			ch.warpScaleValid = true;
		}
		if ((shapeKnobChanged || shapeModeChanged) && ch.phase != OUTER_IDLE) {
			// Re-anchor phase to current output whenever curve changes so the tracer
			// location is invalidated/recomputed against the updated curve shape.
			float range = std::max(OUTER_V_MAX - OUTER_V_MIN, 1e-6f);
			float x = clamp((ch.out - OUTER_V_MIN) / range, 0.f, 1.f);
			ch.phasePos = segmentPhaseFromOutputNormForMode(x, shapeSigned, ch.phase == OUTER_RISE, shapeMode);
		}
		float riseScale = ch.cachedRiseWarpScale;
		float fallScale = ch.cachedFallWarpScale;

		bool signalPatched = inputs[cfg.signalInput].isConnected();
		float signalIn = signalPatched ? inputs[cfg.signalInput].getVoltage() : 0.f;
		if (ch.phase == OUTER_IDLE && cycleOn) {
			// Cycle retriggers as soon as the channel reaches idle.
			triggerOuterFunction(ch);
		}
		if (ch.phase != OUTER_IDLE) {
			bool gateIsHigh = (ch.phase == cfg.gateHighPhase);
			if (gateIsHigh != gateWasHigh) {
				// Transition occurred at start-of-sample due to trigger/cycle state.
				if (bandlimitedGateEnabled) {
					insertGateTransition(ch, gateIsHigh, 1e-6f);
				}
				else {
					setGateStateImmediate(ch, gateIsHigh);
				}
			}
		}
		else if (!signalPatched && gateWasHigh) {
			if (bandlimitedGateEnabled) {
				insertGateTransition(ch, false, 1e-6f);
			}
			else {
				setGateStateImmediate(ch, false);
			}
		}

		if (ch.phase != OUTER_IDLE) {
			// Function-generator integration path.
			float s = shapeSigned;
			float range = OUTER_V_MAX - OUTER_V_MIN;
			float xIn = 0.f;
			float injectAlpha = 0.f;
			if (signalPatched) {
				// Map patched input into the same normalized domain as the internal integrator state.
				float inSoft = levi_math::softLimit(signalIn, 8.f);
				xIn = clamp((inSoft - OUTER_V_MIN) / range, 0.f, 1.f);
				injectAlpha = injectAlphaBase;
			}

			if (ch.phase == OUTER_RISE) {
				float dpPhase = dt / riseTime;
				ch.phasePos += dpPhase;
				float x = clamp((ch.out - OUTER_V_MIN) / range, 0.f, 1.f);
				float dp = clamp(dt / riseTime, 0.f, 0.5f);
				x += dp * slopeWarpForMode(x, s, true, shapeMode) * riseScale;
				if (injectAlpha > 0.f) {
					// Hardware-like perturbation: gently pull active FG state toward input.
					x += injectAlpha * (xIn - x);
				}
				x = clamp(x, 0.f, 1.f);
				ch.out = OUTER_V_MIN + x * range;
				if (ch.phasePos >= 1.f || x >= 1.f) {
					// Preserve fractional overshoot so rise->fall transition remains sample-rate robust.
					float f = phaseCrossingFraction(ch.phasePos, dpPhase);
					float overshoot = std::max(ch.phasePos - 1.f, 0.f);
					ch.phasePos = overshoot * (riseTime / std::max(fallTime, 1e-6f));
					ch.phase = OUTER_FALL;
					// Keep output continuous at rise->fall boundary (no hard snap to max).
					if (bandlimitedGateEnabled) {
						insertGateTransition(ch, ch.phase == cfg.gateHighPhase, f);
					}
					else {
						setGateStateImmediate(ch, ch.phase == cfg.gateHighPhase);
					}
				}
			}

			if (ch.phase == OUTER_FALL) {
				float dpPhase = dt / fallTime;
				ch.phasePos += dpPhase;
				float x = clamp((ch.out - OUTER_V_MIN) / range, 0.f, 1.f);
				float dp = clamp(dt / fallTime, 0.f, 0.5f);
				x -= dp * slopeWarpForMode(x, s, false, shapeMode) * fallScale;
				if (injectAlpha > 0.f) {
					x += injectAlpha * (xIn - x);
				}
				x = clamp(x, 0.f, 1.f);
				ch.out = OUTER_V_MIN + x * range;
				if (ch.phasePos >= 1.f || x <= 0.f) {
					float f = phaseCrossingFraction(ch.phasePos, dpPhase);
					ch.phasePos = 0.f;
					ch.phase = OUTER_IDLE;
					float prevOut = ch.out;
					ch.out = OUTER_V_MIN;
					if (bandlimitedSignalEnabled) {
						insertSignalTransition(ch, ch.out - prevOut, f);
					}
					if (bandlimitedGateEnabled) {
						insertGateTransition(ch, ch.phase == cfg.gateHighPhase, f);
					}
					else {
						setGateStateImmediate(ch, ch.phase == cfg.gateHighPhase);
					}
				}
			}
		}
		else if (signalPatched) {
			// Use the same curve-warp family as the function generator path.
			SlewStepResult slewStep = processUnifiedShapedSlew(
				ch,
				signalIn,
				riseTime,
				fallTime,
				shapeSigned,
				shapeMode,
				riseScale,
				fallScale,
				dt
			);
			ch.out = slewStep.out;
			bool gateIsHigh = (cfg.gateHighPhase == OUTER_RISE) ? (slewStep.direction > 0) : (slewStep.direction < 0);
			if (gateIsHigh != gateWasHigh) {
				if (bandlimitedGateEnabled) {
					insertGateTransition(ch, gateIsHigh, 1e-6f);
				}
				else {
					setGateStateImmediate(ch, gateIsHigh);
				}
			}
		}
		else {
			ch.slewDir = 0;
			ch.out = 0.f;
		}

		OuterChannelResult result;
		result.cycleOn = cycleOn;
		result.previewStatePublished = previewStatePublished;
		return result;
	}

	IntegralFluxImpl() {
		initKnobCurveLut();
		debugInstanceId = gIntegralFluxDebugInstanceCounter.fetch_add(1u, std::memory_order_relaxed);
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(ATTENUATE_1_PARAM, 0.f, 1.f, 0.5f, "CH1 attenuverter", "%", 0.f, 200.f, -100.f);
		configParam(CYCLE_1_PARAM, 0.f, 1.f, 0.f, "CH1 loop");
		configParam(CYCLE_4_PARAM, 0.f, 1.f, 0.f, "CH4 loop");
		configParam(RISE_1_PARAM, 0.f, 1.f, 0.f, "CH1 rise", "%", 0.f, 100.f, 0.f);
		configParam(RISE_4_PARAM, 0.f, 1.f, 0.f, "CH4 rise", "%", 0.f, 100.f, 0.f);
		configParam(ATTENUATE_2_PARAM, 0.f, 1.f, 0.5f, "CH2 attenuverter", "%", 0.f, 200.f, -100.f);
		configParam(FALL_1_PARAM, 0.f, 1.f, 0.f, "CH1 fall", "%", 0.f, 100.f, 0.f);
		configParam(FALL_4_PARAM, 0.f, 1.f, 0.f, "CH4 fall", "%", 0.f, 100.f, 0.f);
		configParam(ATTENUATE_3_PARAM, 0.f, 1.f, 0.5f, "CH3 attenuverter", "%", 0.f, 200.f, -100.f);
		configParam(LIN_LOG_1_PARAM, 0.f, 1.f, 0.f, "CH1 shape", "%", 0.f, 100.f, 0.f);
		configParam(LIN_LOG_4_PARAM, 0.f, 1.f, 0.f, "CH4 shape", "%", 0.f, 100.f, 0.f);
		configParam(ATTENUATE_4_PARAM, 0.f, 1.f, 0.5f, "CH4 attenuverter", "%", 0.f, 200.f, -100.f);
		configSwitch(SHAPE_MODE_1_PARAM, 0.f, 1.f, 1.f, "CH1 function shape mode", {"Shark Fin", "Mirror"});
		configSwitch(SHAPE_MODE_4_PARAM, 0.f, 1.f, 1.f, "CH4 function shape mode", {"Shark Fin", "Mirror"});
		configInput(INPUT_1_INPUT, "CH1 signal");
		configInput(INPUT_1_TRIG_INPUT, "CH1 trigger");
		configInput(INPUT_2_INPUT, "CH2 signal");
		configInput(INPUT_3_INPUT, "CH3 signal");
		configInput(INPUT_4_TRIG_INPUT, "CH4 trigger");
		configInput(INPUT_4_INPUT, "CH4 signal");
		configInput(CH1_RISE_CV_INPUT, "CH1 rise");
		configInput(CH4_RISE_CV_INPUT, "CH4 rise");
		configInput(CH1_BOTH_CV_INPUT, "CH1 both");
		configInput(CH4_BOTH_CV_INPUT, "CH4 both");
		configInput(CH1_FALL_CV_INPUT, "CH1 fall");
		configInput(CH4_FALL_CV_INPUT, "CH4 fall");
		configInput(CH1_CYCLE_CV_INPUT, "CH1 loop");
		configInput(CH4_CYCLE_CV_INPUT, "CH4 loop");
		configOutput(OUT_1_OUTPUT, "CH1 variable");
		configOutput(OUT_2_OUTPUT, "CH2 variable");
		configOutput(OUT_3_OUTPUT, "CH3 variable");
		configOutput(OUT_4_OUTPUT, "CH4 variable");
		configOutput(EOR_1_OUTPUT, "CH1 end of rise");
		configOutput(CH_1_UNITY_OUTPUT, "CH1 unity");
		configOutput(OR_OUT_OUTPUT, "OR");
		configOutput(SUM_OUT_OUTPUT, "SUM");
		configOutput(INV_OUT_OUTPUT, "INV");
		configOutput(CH_4_UNITY_OUTPUT, "CH4 unity");
		configOutput(EOC_4_OUTPUT, "CH4 end of cycle");
	}

	~IntegralFluxImpl() override {
		teardownTimer.begin(id);
	}

	json_t* dataToJson() override {
		json_t* rootJ = json_object();
		json_object_set_new(rootJ, "ch1CycleLatched", json_boolean(ch1.cycleLatched));
		json_object_set_new(rootJ, "ch4CycleLatched", json_boolean(ch4.cycleLatched));
		json_object_set_new(rootJ, "bandlimitedGateOutputs", json_boolean(bandlimitedGateOutputs.load(std::memory_order_relaxed)));
		json_object_set_new(rootJ, "bandlimitedSignalOutputs", json_boolean(bandlimitedSignalOutputs.load(std::memory_order_relaxed)));
		json_object_set_new(rootJ, "timingUpdateDiv", json_integer(requestedTimingUpdateDiv.load(std::memory_order_relaxed)));
		json_object_set_new(rootJ, "timingInterpolate", json_boolean(timingInterpolate.load(std::memory_order_relaxed)));
		json_object_set_new(rootJ, "previewTracerEnabled", json_boolean(previewTracerEnabled.load(std::memory_order_relaxed)));
		json_object_set_new(rootJ, "previewTracerCacheMode", json_integer(previewTracerCacheMode.load(std::memory_order_relaxed)));
		json_object_set_new(rootJ, "previewRenderMode", json_integer(previewRenderMode.load(std::memory_order_relaxed)));
		return rootJ;
	}

	void dataFromJson(json_t* rootJ) override {
		json_t* ch1CycleJ = json_object_get(rootJ, "ch1CycleLatched");
		if (ch1CycleJ) {
			ch1.cycleLatched = json_boolean_value(ch1CycleJ);
		}

		json_t* ch4CycleJ = json_object_get(rootJ, "ch4CycleLatched");
		if (ch4CycleJ) {
			ch4.cycleLatched = json_boolean_value(ch4CycleJ);
		}

		json_t* blepGatesJ = json_object_get(rootJ, "bandlimitedGateOutputs");
		if (blepGatesJ) {
			bandlimitedGateOutputs.store(json_boolean_value(blepGatesJ), std::memory_order_relaxed);
		}

		json_t* blepSignalJ = json_object_get(rootJ, "bandlimitedSignalOutputs");
		if (blepSignalJ) {
			bandlimitedSignalOutputs.store(json_boolean_value(blepSignalJ), std::memory_order_relaxed);
		}

		json_t* timingDivJ = json_object_get(rootJ, "timingUpdateDiv");
		if (timingDivJ) {
			requestTimingUpdateDiv(json_integer_value(timingDivJ));
		}

		json_t* timingInterpJ = json_object_get(rootJ, "timingInterpolate");
		if (timingInterpJ) {
			timingInterpolate.store(json_boolean_value(timingInterpJ), std::memory_order_relaxed);
		}

		json_t* previewTracerJ = json_object_get(rootJ, "previewTracerEnabled");
		if (previewTracerJ) {
			previewTracerEnabled.store(json_boolean_value(previewTracerJ), std::memory_order_relaxed);
		}

		json_t* previewTracerModeJ = json_object_get(rootJ, "previewTracerCacheMode");
		if (previewTracerModeJ) {
			const int mode = int(json_integer_value(previewTracerModeJ));
			previewTracerCacheMode.store(mode == WAVE_PREVIEW_TRACER_CURVE_CACHE ? WAVE_PREVIEW_TRACER_CURVE_CACHE : WAVE_PREVIEW_TRACER_FRAME_CACHE,
			                             std::memory_order_relaxed);
		}

		json_t* previewRenderModeJ = json_object_get(rootJ, "previewRenderMode");
		if (previewRenderModeJ) {
			previewRenderMode.store(json_integer_value(previewRenderModeJ) == 1 ? 1 : 0, std::memory_order_relaxed);
		}
		if (!isDragonKingPreviewWidgetOptionsEnabled()) {
			previewTracerCacheMode.store(WAVE_PREVIEW_TRACER_CURVE_CACHE, std::memory_order_relaxed);
			previewRenderMode.store(0, std::memory_order_relaxed);
		}
	}

	void process(const ProcessArgs& args) override {
		using PerfClock = std::chrono::steady_clock;
		const bool measurePerf = isDragonKingDebugEnabled();
		const PerfClock::time_point perfStart = measurePerf ? PerfClock::now() : PerfClock::time_point();
		const bool bandlimitedSignalEnabled = bandlimitedSignalOutputs.load(std::memory_order_relaxed);
		const bool bandlimitedGateEnabled = bandlimitedGateOutputs.load(std::memory_order_relaxed);
		const bool timingInterpolateEnabled = timingInterpolate.load(std::memory_order_relaxed);
		const float injectAlphaBase = injectAlphaBaseForSampleTime(args.sampleTime);
		applyRequestedTimingUpdateDiv();
		// Static config structs remove repeated branching and keep CH1/CH4 path unified.
		static const OuterChannelConfig ch1Cfg {
			CYCLE_1_PARAM,
			INPUT_1_TRIG_INPUT,
			INPUT_1_INPUT,
			RISE_1_PARAM,
			FALL_1_PARAM,
			LIN_LOG_1_PARAM,
			SHAPE_MODE_1_PARAM,
			CH1_RISE_CV_INPUT,
			CH1_FALL_CV_INPUT,
			CH1_BOTH_CV_INPUT,
			CH1_CYCLE_CV_INPUT,
			std::log2(OUTER_LOG_SHAPE_SCALE),  // Shared CH1/CH4 low-curve timing scale.
			std::log2(OUTER_EXP_SHAPE_SCALE),  // Shared CH1/CH4 high-curve timing scale.
			OUTER_FALL
		};
		static const OuterChannelConfig ch4Cfg {
			CYCLE_4_PARAM,
			INPUT_4_TRIG_INPUT,
			INPUT_4_INPUT,
			RISE_4_PARAM,
			FALL_4_PARAM,
			LIN_LOG_4_PARAM,
			SHAPE_MODE_4_PARAM,
			CH4_RISE_CV_INPUT,
			CH4_FALL_CV_INPUT,
			CH4_BOTH_CV_INPUT,
			CH4_CYCLE_CV_INPUT,
			std::log2(OUTER_LOG_SHAPE_SCALE),  // Shared CH1/CH4 low-curve timing scale.
			std::log2(OUTER_EXP_SHAPE_SCALE),  // Shared CH1/CH4 high-curve timing scale.
			OUTER_RISE
		};

		bool timingTick = true;
		if (timingUpdateDiv > 1) {
			// Control-rate timing update option reduces CPU when heavy CV modulation is present.
			timingUpdateCounter++;
			if (timingUpdateCounter >= timingUpdateDiv) {
				timingUpdateCounter = 0;
				timingTick = true;
			}
			else {
				timingTick = false;
			}
		}
		lightUpdateTimer += args.sampleTime;
		bool lightTick = false;
		if (lightUpdateTimer >= LIGHT_UPDATE_INTERVAL) {
			lightUpdateTimer -= LIGHT_UPDATE_INTERVAL;
			if (lightUpdateTimer >= LIGHT_UPDATE_INTERVAL) {
				lightUpdateTimer = 0.f;
			}
			lightTick = true;
		}
		previewDotPublishTimer += args.sampleTime;
		bool previewDotPublishTick = false;
		if (previewDotPublishTimer >= PREVIEW_DOT_PUBLISH_INTERVAL) {
			previewDotPublishTimer -= PREVIEW_DOT_PUBLISH_INTERVAL;
			if (previewDotPublishTimer >= PREVIEW_DOT_PUBLISH_INTERVAL) {
				previewDotPublishTimer = 0.f;
			}
			previewDotPublishTick = true;
		}
		OuterChannelResult ch1Result;
		OuterChannelResult ch4Result;
		ch1Result = processOuterChannel(args, ch1, ch1Cfg, previewCh1, previewUpdateCh1, timingTick, bandlimitedSignalEnabled, bandlimitedGateEnabled, timingInterpolateEnabled, injectAlphaBase);
		ch4Result = processOuterChannel(args, ch4, ch4Cfg, previewCh4, previewUpdateCh4, timingTick, bandlimitedSignalEnabled, bandlimitedGateEnabled, timingInterpolateEnabled, injectAlphaBase);
		float ch1OutRendered = ch1.out + (bandlimitedSignalEnabled ? ch1.signalBlep.process() : 0.f);
		float ch4OutRendered = ch4.out + (bandlimitedSignalEnabled ? ch4.signalBlep.process() : 0.f);
		if (previewDotPublishTick || ch1Result.previewStatePublished || ch4Result.previewStatePublished) {
			float outRangeInv = 1.f / std::max(OUTER_V_MAX - OUTER_V_MIN, 1e-6f);
			auto computeDotX = [](const OuterChannelState& ch) {
				if (ch.phase == OUTER_IDLE) {
					return 0.f;
				}
				float rise = std::max(ch.activeRiseTime, 1e-6f);
				float fall = std::max(ch.activeFallTime, 1e-6f);
				float total = rise + fall;
				if (ch.phase == OUTER_RISE) {
					return clamp((ch.phasePos * rise) / total, 0.f, 1.f);
				}
				if (ch.phase == OUTER_FALL) {
					return clamp((rise + ch.phasePos * fall) / total, 0.f, 1.f);
				}
				return 0.f;
			};
			publishPreviewDot(
				previewCh1,
				ch1.phase != OUTER_IDLE,
				computeDotX(ch1),
				(ch1OutRendered - OUTER_V_MIN) * outRangeInv
			);
			publishPreviewDot(
				previewCh4,
				ch4.phase != OUTER_IDLE,
				computeDotX(ch4),
				(ch4OutRendered - OUTER_V_MIN) * outRangeInv
			);
		}
		// Variable outputs are attenuverters; unity outputs bypass this scaling.
		float ch1Var = clamp(ch1OutRendered * attenuverterGain(params[ATTENUATE_1_PARAM].getValue()), -10.f, 10.f);
		float ch2In = inputs[INPUT_2_INPUT].isConnected() ? inputs[INPUT_2_INPUT].getVoltage() : 10.f;
		float ch2Var = clamp(ch2In * attenuverterGain(params[ATTENUATE_2_PARAM].getValue()), -10.f, 10.f);
		float ch3In = inputs[INPUT_3_INPUT].isConnected() ? inputs[INPUT_3_INPUT].getVoltage() : 5.f;
		float ch3Var = clamp(ch3In * attenuverterGain(params[ATTENUATE_3_PARAM].getValue()), -10.f, 10.f);
		float ch4Var = clamp(ch4OutRendered * attenuverterGain(params[ATTENUATE_4_PARAM].getValue()), -10.f, 10.f);
		float eorOut = (ch1.gateState ? 10.f : 0.f) + (bandlimitedGateEnabled ? ch1.gateBlep.process() : 0.f);
		float eocOut = (ch4.gateState ? 10.f : 0.f) + (bandlimitedGateEnabled ? ch4.gateBlep.process() : 0.f);
		bool eorHigh = ch1.gateState;
		bool eocHigh = ch4.gateState;
		float sumOut = 0.f;
		float invOut = 0.f;
		float orOut = 0.f;
		bool mixOutputsConnected = outputs[OR_OUT_OUTPUT].isConnected()
			|| outputs[SUM_OUT_OUTPUT].isConnected()
			|| outputs[INV_OUT_OUTPUT].isConnected();
		if (mixOutputsConnected || lightTick) {
			// Maths-style normalization:
			// once a variable output jack is patched, that channel is removed from SUM/OR/INV bus.
			float busV1 = outputs[OUT_1_OUTPUT].isConnected() ? 0.f : ch1Var;
			float busV2 = outputs[OUT_2_OUTPUT].isConnected() ? 0.f : ch2Var;
			float busV3 = outputs[OUT_3_OUTPUT].isConnected() ? 0.f : ch3Var;
			float busV4 = outputs[OUT_4_OUTPUT].isConnected() ? 0.f : ch4Var;
			float sumRaw = busV1 + busV2 + busV3 + busV4;
			float orRaw = std::fmax(0.f, std::fmax(std::fmax(busV1, busV2), std::fmax(busV3, busV4)));
			sumOut = clamp(sumRaw, -10.f, 10.f);
			invOut = clamp(-sumOut, -10.f, 10.f);
			orOut = clamp(orRaw, 0.f, 10.f);
		}

		outputs[EOR_1_OUTPUT].setVoltage(eorOut);
		outputs[EOC_4_OUTPUT].setVoltage(eocOut);
		outputs[OR_OUT_OUTPUT].setVoltage(orOut);
		outputs[SUM_OUT_OUTPUT].setVoltage(sumOut);
		outputs[INV_OUT_OUTPUT].setVoltage(invOut);

		outputs[CH_1_UNITY_OUTPUT].setVoltage(ch1OutRendered);
		outputs[OUT_1_OUTPUT].setVoltage(ch1Var);
		outputs[OUT_2_OUTPUT].setVoltage(ch2Var);
		outputs[OUT_3_OUTPUT].setVoltage(ch3Var);
		outputs[OUT_4_OUTPUT].setVoltage(ch4Var);
		outputs[CH_4_UNITY_OUTPUT].setVoltage(ch4OutRendered);

		if (lightTick) {
			// Light refresh is intentionally decoupled from audio rate.
			lights[CYCLE_1_LED_LIGHT].setBrightness(ch1Result.cycleOn ? 1.f : 0.f);
			lights[CYCLE_4_LED_LIGHT].setBrightness(ch4Result.cycleOn ? 1.f : 0.f);
			lights[EOR_CH_1_LIGHT].setBrightness(eorHigh ? 1.f : 0.f);
			lights[EOC_CH_4_LIGHT].setBrightness(eocHigh ? 1.f : 0.f);
			lights[LIGHT_UNITY_1_LIGHT].setBrightness(clamp(std::fabs(ch1OutRendered) / OUTER_V_MAX, 0.f, 1.f));
			lights[LIGHT_UNITY_4_LIGHT].setBrightness(clamp(std::fabs(ch4OutRendered) / OUTER_V_MAX, 0.f, 1.f));
			// Mixer LEDs indicate SUM bus polarity (INV is the same signal inverted):
			// red = negative SUM, green = positive SUM.
			lights[OR_LED_LIGHT].setBrightness(clamp((-sumOut) / 10.f, 0.f, 1.f));
			lights[INV_LED_LIGHT].setBrightness(clamp(sumOut / 10.f, 0.f, 1.f));
		}
		if (measurePerf) {
			const uint64_t elapsedNs = (uint64_t)std::chrono::duration_cast<std::chrono::nanoseconds>(
				PerfClock::now() - perfStart).count();
			perfAudioProcessNs.fetch_add(elapsedNs, std::memory_order_relaxed);
			perfAudioSampledCount.fetch_add(1u, std::memory_order_relaxed);
			debug_terminal::recordAudioProcessTiming(perfAudioProcessMinNs, perfAudioProcessMaxNs, elapsedNs);
		}
	}
};


namespace {
struct IntegralFluxModel : plugin::Model {
	engine::Module* createModule() override {
		engine::Module* m = new IntegralFluxImpl;
		m->model = this;
		return m;
	}

	app::ModuleWidget* createModuleWidget(engine::Module* m) override {
		IntegralFlux* flux = nullptr;
		if (m) {
			assert(m->model == this);
			flux = dynamic_cast<IntegralFlux*>(m);
		}
		app::ModuleWidget* mw = createIntegralFluxWidget(flux);
		assert(mw->module == m);
		mw->setModel(this);
		return mw;
	}
};
} // namespace

Model* modelIntegralFlux = []() {
	plugin::Model* model = new IntegralFluxModel;
	model->slug = "IntegralFlux";
	return model;
}();
