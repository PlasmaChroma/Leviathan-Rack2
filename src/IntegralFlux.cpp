#include "plugin.hpp"
#include "DebugTerminalTransport.hpp"
#include "MathHelpers.hpp"
#include "PanelSvgUtils.hpp"
#include "VisualAssets.hpp"
#include "WavePreviewTracer.hpp"
#include <dsp/minblep.hpp>
#include <array>
#include <cstdio>
#include <atomic>
#include <chrono>
#include <cmath>
#include <limits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {
std::atomic<uint32_t> gIntegralFluxDebugInstanceCounter {1u};
constexpr double kIntegralFluxDebugTerminalSubmitIntervalSec = debug_terminal::kTimingRangeSubmitIntervalSec;
std::unordered_map<uint32_t, double> gIntegralFluxDebugTerminalLastSubmitSec;
thread_local uint64_t gIntegralFluxGearDrawNsThisFrame = 0u;
thread_local uint64_t gIntegralFluxEclipseDrawNsThisFrame = 0u;
thread_local uint64_t gIntegralFluxApertureDrawNsThisFrame = 0u;

struct IntegralFluxFittedSvgWidget final : TransparentWidget {
	std::shared_ptr<window::Svg> svg;

	void setSvg(std::shared_ptr<window::Svg> svg) {
		this->svg = svg;
	}

	void draw(const DrawArgs& args) override {
		if (!svg || !svg->handle || box.size.x <= 0.f || box.size.y <= 0.f) {
			return;
		}
		const Vec svgSize = svg->getSize();
		if (svgSize.x <= 0.f || svgSize.y <= 0.f) {
			return;
		}

		nvgSave(args.vg);
		nvgScale(args.vg, box.size.x / svgSize.x, box.size.y / svgSize.y);
		svg->draw(args.vg);
		nvgRestore(args.vg);
	}
};
}

struct IntegralFlux : Module {
	ModuleTeardownTimer teardownTimer {"IntegralFlux"};
	// Panel/control IDs are intentionally ordered to match panel layout and existing patches.
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
		float cachedWarpScale = 1.f;
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

	void recordCurvePointReduction(int channel, size_t inputPointCount, size_t outputPointCount) {
		if (!isDragonKingDebugEnabled()) {
			return;
		}
		const size_t index = previewDebugChannelIndex(channel);
		debugCurvePointsReducedTotal[index].fetch_add(inputPointCount - std::min(inputPointCount, outputPointCount),
		                                              std::memory_order_relaxed);
		debugCurveReductionSamples[index].fetch_add(1u, std::memory_order_relaxed);
	}

	void recordTracerExtraPointReduction(int channel, const WavePreviewTracerCaptureStats& stats) {
		if (!isDragonKingDebugEnabled() || !stats.captured) {
			return;
		}
		const size_t index = previewDebugChannelIndex(channel);
		debugTracerExtraPointsReducedTotal[index].fetch_add(
			stats.simplifiedPointCount - std::min(stats.simplifiedPointCount, stats.compactedPointCount),
			std::memory_order_relaxed);
		debugTracerReductionSamples[index].fetch_add(1u, std::memory_order_relaxed);
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

	static float shapeSignedFromKnob(float shape01) {
		shape01 = clamp(shape01, 0.f, 1.f);
		if (shape01 < LINEAR_SHAPE) {
			return (shape01 - LINEAR_SHAPE) / LINEAR_SHAPE;
		}
		if (shape01 > LINEAR_SHAPE) {
			return (shape01 - LINEAR_SHAPE) / (1.f - LINEAR_SHAPE);
		}
		return 0.f;
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
		float warpScale,
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
		float range = OUTER_V_MAX - OUTER_V_MIN;
		float x = computeSegPhase(out, ch.slewStartOut, ch.slewInvSpan);
		float dp = clamp(dt / stageTime, 0.f, 0.5f);
		float step = dp * slopeWarp(x, shapeSigned) * warpScale * range;

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

	void requestTimingUpdateDiv(int div) {
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

	void publishPreviewState(PreviewSharedState& shared, float riseTime, float fallTime, float curveSigned, bool interactiveRecent) {
		// Batched atomic publish: UI only rebuilds when version increments.
		shared.riseTime.store(riseTime, std::memory_order_relaxed);
		shared.fallTime.store(fallTime, std::memory_order_relaxed);
		shared.curveSigned.store(curveSigned, std::memory_order_relaxed);
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
		float dt
	) {
		// Preview refresh runs slower than audio and only pushes updates when meaningful.
		bool knobChanged = std::fabs(riseKnob - state.lastRiseKnob) > PARAM_CACHE_EPS
			|| std::fabs(fallKnob - state.lastFallKnob) > PARAM_CACHE_EPS
			|| std::fabs(curveKnob - state.lastCurveKnob) > PARAM_CACHE_EPS;
		state.lastRiseKnob = riseKnob;
		state.lastFallKnob = fallKnob;
		state.lastCurveKnob = curveKnob;

		if (knobChanged) {
			state.interactiveHold = PREVIEW_INTERACTIVE_HOLD;
			// Push an immediate preview refresh on manual interaction.
			state.timer = PREVIEW_INTERACTIVE_INTERVAL;
		}
		if (state.interactiveHold > 0.f) {
			state.interactiveHold = std::max(0.f, state.interactiveHold - dt);
		}
		state.timer += dt;

		float interval = (state.interactiveHold > 0.f) ? PREVIEW_INTERACTIVE_INTERVAL : PREVIEW_CV_INTERVAL;
		bool changed = knobChanged || !state.sentOnce || previewChangedMeaningfully(
			riseTime, state.lastRiseSent,
			fallTime, state.lastFallSent,
			curveSigned, state.lastCurveSent
		);
		if (changed && state.timer >= interval) {
			publishPreviewState(shared, riseTime, fallTime, curveSigned, state.interactiveHold > 0.f);
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
		float& dotYNorm, bool& dotVisible, bool& interactiveRecent, uint32_t& version) const {
		const PreviewSharedState& shared = (channel == 4) ? previewCh4 : previewCh1;
		riseTime = shared.riseTime.load(std::memory_order_relaxed);
		fallTime = shared.fallTime.load(std::memory_order_relaxed);
		curveSigned = shared.curveSigned.load(std::memory_order_relaxed);
		dotXNorm = shared.dotXNorm.load(std::memory_order_relaxed);
		dotYNorm = shared.dotYNorm.load(std::memory_order_relaxed);
		dotVisible = shared.dotVisible.load(std::memory_order_relaxed) != 0;
		interactiveRecent = shared.interactiveRecent.load(std::memory_order_relaxed) != 0;
		version = shared.version.load(std::memory_order_relaxed);
	}

	float computeShapeTimeScale(float shape, float logScaleLog2, float expScaleLog2) const {
		// Shape knob (log/lin/exp) contributes a multiplicative time factor.
		// We interpolate in log2 domain so scaling stays perceptually smooth.
		shape = clamp(shape, 0.f, 1.f);
		if (shape < LINEAR_SHAPE) {
			float t = shape / LINEAR_SHAPE;
			return rack::dsp::exp2_taylor5((1.f - t) * logScaleLog2);
		}
		if (shape > LINEAR_SHAPE) {
			float t = (shape - LINEAR_SHAPE) / (1.f - LINEAR_SHAPE);
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
		float riseCv = inputs[cfg.riseCvInput].getVoltage();
		float fallCv = inputs[cfg.fallCvInput].getVoltage();
		float bothCv = inputs[cfg.bothCvInput].getVoltage();
		bool shapeKnobChanged = std::fabs(shape - ch.cachedShape) > PARAM_CACHE_EPS;
		if (!ch.stageTimeValid || timingTick) {
			// Recompute times only when a relevant source changed.
			bool stageTimeDirty = !ch.stageTimeValid
				|| std::fabs(riseKnob - ch.cachedRiseKnob) > PARAM_CACHE_EPS
				|| std::fabs(fallKnob - ch.cachedFallKnob) > PARAM_CACHE_EPS
				|| std::fabs(shape - ch.cachedShape) > PARAM_CACHE_EPS
				|| std::fabs(riseCv - ch.cachedRiseCv) > CV_CACHE_EPS
				|| std::fabs(fallCv - ch.cachedFallCv) > CV_CACHE_EPS
				|| std::fabs(bothCv - ch.cachedBothCv) > CV_CACHE_EPS;
			if (stageTimeDirty) {
				float bothScale = bothTimeScaleFromCv(bothCv);
				float shapeTimeScale = computeShapeTimeScale(shape, cfg.logShapeTimeScaleLog2, cfg.expShapeTimeScaleLog2);
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
		float shapeSigned = shapeSignedFromKnob(shape);
		bool previewStatePublished = updatePreviewChannel(
			previewShared,
			previewUpdateState,
			riseKnob,
			fallKnob,
			shape,
			riseTime,
			fallTime,
			shapeSigned,
			dt
		);
		if (!ch.warpScaleValid || std::fabs(shapeSigned - ch.cachedShapeSigned) > 1e-4f) {
			// Curve normalization changes only when shape changes.
			ch.cachedShapeSigned = shapeSigned;
			ch.cachedWarpScale = slopeWarpScale(shapeSigned);
			ch.warpScaleValid = true;
		}
		if (shapeKnobChanged && ch.phase != OUTER_IDLE) {
			// Re-anchor phase to current output whenever curve changes so the tracer
			// location is invalidated/recomputed against the updated curve shape.
			float range = std::max(OUTER_V_MAX - OUTER_V_MIN, 1e-6f);
			float x = clamp((ch.out - OUTER_V_MIN) / range, 0.f, 1.f);
			ch.phasePos = segmentPhaseFromOutputNorm(x, shapeSigned, ch.phase == OUTER_RISE);
		}
		float scale = ch.cachedWarpScale;

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
				x += dp * slopeWarp(x, s) * scale;
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
				x -= dp * slopeWarp(x, s) * scale;
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
				scale,
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

	IntegralFlux() {
		initKnobCurveLut();
		debugInstanceId = gIntegralFluxDebugInstanceCounter.fetch_add(1u, std::memory_order_relaxed);
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(ATTENUATE_1_PARAM, 0.f, 1.f, 0.5f, "CH1 attenuverter", "%", 0.f, 200.f, -100.f);
		configParam(CYCLE_1_PARAM, 0.f, 1.f, 0.f, "CH1 cycle");
		configParam(CYCLE_4_PARAM, 0.f, 1.f, 0.f, "CH4 cycle");
		configParam(RISE_1_PARAM, 0.f, 1.f, 0.f, "CH1 rise");
		configParam(RISE_4_PARAM, 0.f, 1.f, 0.f, "CH4 rise");
		configParam(ATTENUATE_2_PARAM, 0.f, 1.f, 0.5f, "CH2 attenuverter", "%", 0.f, 200.f, -100.f);
		configParam(FALL_1_PARAM, 0.f, 1.f, 0.f, "CH1 fall");
		configParam(FALL_4_PARAM, 0.f, 1.f, 0.f, "CH4 fall");
		configParam(ATTENUATE_3_PARAM, 0.f, 1.f, 0.5f, "CH3 attenuverter", "%", 0.f, 200.f, -100.f);
		configParam(LIN_LOG_1_PARAM, 0.f, 1.f, 0.f, "CH1 shape");
		configParam(LIN_LOG_4_PARAM, 0.f, 1.f, 0.f, "CH4 shape");
		configParam(ATTENUATE_4_PARAM, 0.f, 1.f, 0.5f, "CH4 attenuverter", "%", 0.f, 200.f, -100.f);
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
		configInput(CH1_CYCLE_CV_INPUT, "CH1 cycle");
		configInput(CH4_CYCLE_CV_INPUT, "CH4 cycle");
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

	~IntegralFlux() override {
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

// Create a bigger basic button
struct BigTL1105 : TL1105 {
    BigTL1105() {
        // Dialed back to ~85% of previous size for a tighter click area.
        box.size = mm2px(Vec(9.5, 9.5));
    }
};

struct WavePreviewWidget : widget::OpenGlWidget {
	// Preview boxes are small; this density materially lowers per-frame NanoVG work
	// while remaining visually smooth at current panel scale.
	static constexpr int POINT_COUNT = 128;
	static constexpr int PREVIEW_LUT_SIZE = 512;
	static constexpr float CENTER_LINE_WIDTH = 1.0f;
	static constexpr float WAVE_LINE_WIDTH = 1.4f;
	static constexpr float WAVE_EDGE_PAD = 1.0f;
	static constexpr float DOT_RADIUS = 2.1f;
	static constexpr float DOT_SHOW_MAX_HZ = 2.0f;
	static constexpr float DOT_HIDE_MIN_HZ = 2.4f;
	static constexpr float LABEL_FONT_SIZE = 11.5f;
	static constexpr int TRAIL_FRAME_COUNT = 6;
	static constexpr float TRAIL_FADE_SEC = 0.333f;
	static constexpr float TRAIL_MIN_CAPTURE_INTERVAL_SEC = 1.f / 24.f;
	static constexpr float TRAIL_LINE_WIDTH = 1.15f;
	static constexpr float GL_WAVE_LINE_WIDTH = 2.0f;
		static constexpr float GL_TRAIL_LINE_WIDTH = 1.6f;
		static constexpr float GL_HIGH_ZOOM_WIDTH_TAPER = 0.08f;
		static constexpr int TRAIL_DRAW_STRIDE = 2;
		static constexpr int TRAIL_CAPTURE_STRIDE = 1;
	int channel = 1;
	IntegralFlux* modulePtr = nullptr;
	std::array<Vec, POINT_COUNT> points {};
	WavePreviewTracer<POINT_COUNT, TRAIL_FRAME_COUNT> curveTracer;
		WavePreviewBufferedTracer<POINT_COUNT> frameTracer;
		std::array<float, PREVIEW_LUT_SIZE> cachedRiseLut {};
		std::array<float, PREVIEW_LUT_SIZE> cachedFallLut {};
		float cachedLutCurveSigned = 0.f;
		bool cachedLutsValid = false;
		uint32_t lastVersion = 0;
	bool pointsValid = false;
	float lastFreqHz = 100.f;
	float dotXNorm = 0.f;
	float dotYNorm = 0.f;
	bool dotVisible = false;

	WavePreviewWidget(IntegralFlux* module, int channel) {
		modulePtr = module;
		this->channel = channel;
	}

	bool useOpenGlRenderer() const {
		return modulePtr && modulePtr->previewRenderMode.load(std::memory_order_relaxed) == 1;
	}

	static NVGcolor tracerColorWithAlpha(float alpha) {
		return nvgRGBA(255, 190, 80, clamp(int(alpha), 0, 255));
	}

	static void glColorFromNvg(NVGcolor c) {
		glColor4f(c.r, c.g, c.b, c.a);
	}

	static Vec ribbonNormal(const Vec& prev, const Vec& current, const Vec& next) {
		Vec tangent = next - prev;
		float len2 = tangent.x * tangent.x + tangent.y * tangent.y;
		if (len2 < 1e-8f) {
			tangent = next - current;
			len2 = tangent.x * tangent.x + tangent.y * tangent.y;
		}
		if (len2 < 1e-8f) {
			tangent = current - prev;
			len2 = tangent.x * tangent.x + tangent.y * tangent.y;
		}
		if (len2 < 1e-8f) {
			return Vec(0.f, -1.f);
		}
		const float invLen = 1.f / std::sqrt(len2);
		return Vec(-tangent.y * invLen, tangent.x * invLen);
	}

	static void drawGlRibbonPoints(const Vec* linePoints, int pointCount, int stride, float lineWidth, NVGcolor color) {
		if (!linePoints || pointCount <= 0) {
			return;
		}
		stride = std::max(stride, 1);
		const float halfWidth = 0.5f * lineWidth;
		glColorFromNvg(color);
		glBegin(GL_TRIANGLE_STRIP);
		for (int i = 0; i < pointCount; i += stride) {
			const int prevIndex = std::max(0, i - stride);
			const int nextIndex = std::min(pointCount - 1, i + stride);
			const Vec n = ribbonNormal(linePoints[prevIndex], linePoints[i], linePoints[nextIndex]);
			glVertex2f(linePoints[i].x + n.x * halfWidth, linePoints[i].y + n.y * halfWidth);
			glVertex2f(linePoints[i].x - n.x * halfWidth, linePoints[i].y - n.y * halfWidth);
		}
		if ((pointCount - 1) % stride != 0) {
			const int i = pointCount - 1;
			const int prevIndex = std::max(0, i - stride);
			const Vec n = ribbonNormal(linePoints[prevIndex], linePoints[i], linePoints[i]);
			glVertex2f(linePoints[i].x + n.x * halfWidth, linePoints[i].y + n.y * halfWidth);
			glVertex2f(linePoints[i].x - n.x * halfWidth, linePoints[i].y - n.y * halfWidth);
		}
		glEnd();
	}

	static void drawGlRibbon(const std::array<Vec, POINT_COUNT>& linePoints, int stride, float lineWidth, NVGcolor color) {
		drawGlRibbonPoints(linePoints.data(), POINT_COUNT, stride, lineWidth, color);
	}

	static const std::array<Vec, 25>& glDotUnitCircle() {
		static const std::array<Vec, 25> unit = []() {
			std::array<Vec, 25> points {};
			for (int i = 0; i <= 24; ++i) {
				const float a = 6.28318530718f * float(i) / 24.f;
				points[size_t(i)] = Vec(std::cos(a), std::sin(a));
			}
			return points;
		}();
		return unit;
	}

	void drawGlDot() {
		if (!pointsValid || !dotVisible) {
			return;
		}
		float w = std::max(box.size.x, 1.f);
		float h = std::max(box.size.y, 1.f);
		float drawPad = 0.5f * WAVE_LINE_WIDTH + WAVE_EDGE_PAD;
		float left = drawPad;
		float top = drawPad;
		float right = std::max(left + 1.f, w - drawPad);
		float bottom = std::max(top + 1.f, h - drawPad);
		float drawW = right - left;
		float drawH = bottom - top;
		float targetX = left + clamp(dotXNorm, 0.f, 1.f) * drawW;
		float targetY = top + (1.f - clamp(dotYNorm, 0.f, 1.f)) * drawH;
		int i0 = 0;
		for (int i = 1; i < POINT_COUNT; ++i) {
			if (points[i].x >= targetX) {
				i0 = i - 1;
				break;
			}
			i0 = i - 1;
		}
		int i1 = std::min(i0 + 1, POINT_COUNT - 1);
		float x = targetX;
		float y = points[i0].y;
		if (i1 != i0 && points[i1].x > points[i0].x) {
			float t = clamp((targetX - points[i0].x) / (points[i1].x - points[i0].x), 0.f, 1.f);
			y = points[i0].y + (points[i1].y - points[i0].y) * t;
		}
		y = y * 0.9f + targetY * 0.1f;
		glColor4f(0.f, 0.f, 0.f, 0.86f);
		glBegin(GL_TRIANGLE_FAN);
		glVertex2f(x, y);
		const std::array<Vec, 25>& unitCircle = glDotUnitCircle();
		for (const Vec& p : unitCircle) {
			glVertex2f(x + p.x * (DOT_RADIUS + 0.55f), y + p.y * (DOT_RADIUS + 0.55f));
		}
		glEnd();
		glColor4f(1.f, 0.91f, 0.28f, 1.f);
		glBegin(GL_TRIANGLE_FAN);
		glVertex2f(x, y);
		for (const Vec& p : unitCircle) {
			glVertex2f(x + p.x * DOT_RADIUS, y + p.y * DOT_RADIUS);
		}
		glEnd();
	}

	void drawFramebuffer() override {
		math::Vec fbSize = getFramebufferSize();
		glViewport(0, 0, std::max(1, int(std::lround(fbSize.x))), std::max(1, int(std::lround(fbSize.y))));
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT);
		if (!pointsValid || !useOpenGlRenderer()) {
			return;
		}
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0, box.size.x, box.size.y, 0.0, -1.0, 1.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDisable(GL_SCISSOR_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_LINE_SMOOTH);

		const double nowSec = system::getTime();
		const float xScale = fbSize.x / std::max(box.size.x, 1.f);
		const float yScale = fbSize.y / std::max(box.size.y, 1.f);
		const float framebufferScale = std::max(0.1f, 0.5f * (xScale + yScale));
		const float lineScale = framebufferScale < 1.f
		                      ? std::sqrt(framebufferScale)
		                      : 1.f / (1.f + (framebufferScale - 1.f) * GL_HIGH_ZOOM_WIDTH_TAPER);
		const bool tracerEnabled = modulePtr && modulePtr->previewTracerEnabled.load(std::memory_order_relaxed);
		if (tracerEnabled) {
			for (const auto& frame : curveTracer.frames) {
				if (!frame.active) {
					continue;
				}
				const float age = float(nowSec - frame.birthSec);
				if (age < 0.f || age >= TRAIL_FADE_SEC) {
					continue;
				}
				const float fade = 1.f - age / TRAIL_FADE_SEC;
				drawGlRibbonPoints(frame.points.data(), int(frame.pointCount), 1, GL_TRAIL_LINE_WIDTH * lineScale, tracerColorWithAlpha(118.f * fade));
			}
		}
		drawGlRibbon(points, 1, GL_WAVE_LINE_WIDTH * lineScale, nvgRGBA(230, 230, 220, 255));
		if (modulePtr) {
			modulePtr->recordCurvePointReduction(channel, POINT_COUNT, POINT_COUNT);
		}
		drawGlDot();
	}

	void drawFrequencyLabel(const DrawArgs& args) {
		char freqText[32];
		if (lastFreqHz < 1.f) {
			std::snprintf(freqText, sizeof(freqText), "%4.0f mHz", lastFreqHz * 1000.f);
		}
		else if (lastFreqHz >= 1000.f) {
			std::snprintf(freqText, sizeof(freqText), "%4.2f kHz", lastFreqHz / 1000.f);
		}
		else {
			std::snprintf(freqText, sizeof(freqText), "%5.1f Hz", lastFreqHz);
		}
		nvgFontSize(args.vg, LABEL_FONT_SIZE);
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 255));
		nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_TOP);
		nvgText(args.vg, box.size.x * 0.5f, box.size.y + 1.5f, freqText, nullptr);
	}

	static void buildSegmentLut(std::array<float, PREVIEW_LUT_SIZE>& lut, float curveSigned, bool rising) {
		// Build once per preview update. Midpoint integration reduces visual artifacts at extreme curve asymmetry.
		float scale = IntegralFlux::slopeWarpScale(curveSigned);
		float dp = 1.f / float(PREVIEW_LUT_SIZE - 1);
		float x = rising ? 0.f : 1.f;
		lut[0] = x;
		for (int i = 1; i < PREVIEW_LUT_SIZE; ++i) {
			float k1 = IntegralFlux::slopeWarp(x, curveSigned) * scale;
			float xMid = rising ? (x + 0.5f * dp * k1) : (x - 0.5f * dp * k1);
			xMid = clamp(xMid, 0.f, 1.f);
			float k2 = IntegralFlux::slopeWarp(xMid, curveSigned) * scale;
			x += rising ? (dp * k2) : (-dp * k2);
			x = clamp(x, 0.f, 1.f);
			lut[i] = x;
		}
		lut.front() = rising ? 0.f : 1.f;
		lut.back() = rising ? 1.f : 0.f;
	}

	static float sampleSegmentLut(const std::array<float, PREVIEW_LUT_SIZE>& lut, float t) {
		t = clamp(t, 0.f, 1.f);
		float idx = t * float(PREVIEW_LUT_SIZE - 1);
		int i0 = int(idx);
		int i1 = std::min(i0 + 1, PREVIEW_LUT_SIZE - 1);
		float f = idx - float(i0);
		return lut[i0] + (lut[i1] - lut[i0]) * f;
	}

		void ensureSegmentLuts(float curveSigned) {
			if (cachedLutsValid && std::fabs(curveSigned - cachedLutCurveSigned) <= 1e-6f) {
				return;
			}
			buildSegmentLut(cachedRiseLut, curveSigned, true);
			buildSegmentLut(cachedFallLut, curveSigned, false);
			cachedLutCurveSigned = curveSigned;
			cachedLutsValid = true;
		}

		void rebuildPoints(float riseTime, float fallTime, float curveSigned, bool interactiveRecent) {
		float w = std::max(box.size.x, 1.f);
		float h = std::max(box.size.y, 1.f);
		float drawPad = 0.5f * WAVE_LINE_WIDTH + WAVE_EDGE_PAD;
		float left = drawPad;
		float top = drawPad;
		float right = std::max(left + 1.f, w - drawPad);
		float bottom = std::max(top + 1.f, h - drawPad);
		float drawW = right - left;
		float drawH = bottom - top;
		// The preview always shows exactly one full rise+fall cycle across widget width.
		float totalTime = std::max(riseTime + fallTime, 1e-6f);
		float riseRatio = riseTime / totalTime;
		float peakX = left + riseRatio * drawW;
		float riseWidth = std::max(peakX - left, 1e-4f);
		float fallWidth = std::max(right - peakX, 1e-4f);
		// Reserved hook if we later render interactive-state emphasis.
		(void) interactiveRecent;
			ensureSegmentLuts(curveSigned);

		for (int i = 0; i < POINT_COUNT; ++i) {
			float xNorm = float(i) / float(POINT_COUNT - 1);
			float x = left + xNorm * drawW;
			float y = -1.f;
			if (x <= peakX) {
				float t = (x - left) / riseWidth;
					float v = sampleSegmentLut(cachedRiseLut, t);
				y = -1.f + 2.f * v;
			}
			else {
				float t = (x - peakX) / fallWidth;
					float v = sampleSegmentLut(cachedFallLut, t);
				y = -1.f + 2.f * v;
			}
			float py = top + (0.5f - 0.5f * y) * drawH;
			py = clamp(py, top, bottom);
			points[i] = Vec(x, py);
		}

			// Preserve full crest height without flattening the apex into a
			// two-point plateau when the true peak falls between sample columns.
			float peakIndexF = riseRatio * float(POINT_COUNT - 1);
			int peakIndex = std::max(1, std::min(POINT_COUNT - 2, int(std::round(peakIndexF))));
			points[peakIndex] = Vec(peakX, top);
			points.front() = Vec(left, bottom);
			points.back() = Vec(right, bottom);
		pointsValid = true;
	}

	void step() override {
		const bool openGlRenderer = useOpenGlRenderer();
		if (!openGlRenderer) {
			Widget::step();
		}
		if (!modulePtr) {
			if (!pointsValid) {
				rebuildPoints(0.01f, 0.01f, 0.f, false);
			}
			return;
		}
		float riseTime = 0.01f;
		float fallTime = 0.01f;
		float curveSigned = 0.f;
		float previewDotXNorm = 0.f;
		float previewDotYNorm = 0.f;
		bool previewDotVisible = false;
		bool interactiveRecent = false;
		uint32_t version = 0;
		modulePtr->getPreviewState(channel, riseTime, fallTime, curveSigned, previewDotXNorm, previewDotYNorm,
			previewDotVisible, interactiveRecent, version);
		dotXNorm = previewDotXNorm;
		dotYNorm = previewDotYNorm;
		// Displayed frequency reflects the currently effective cycle period.
		lastFreqHz = 1.f / std::max(riseTime + fallTime, 1e-6f);
		// Always hide when FG is inactive; frequency hysteresis only applies while active.
		if (!previewDotVisible) {
			dotVisible = false;
		}
		else if (lastFreqHz >= DOT_HIDE_MIN_HZ) {
			dotVisible = false;
		}
		else if (lastFreqHz <= DOT_SHOW_MAX_HZ) {
			dotVisible = true;
		}
		const double nowSec = system::getTime();
		const bool tracerEnabled = modulePtr->previewTracerEnabled.load(std::memory_order_relaxed);
		const int tracerMode = openGlRenderer ? WAVE_PREVIEW_TRACER_CURVE_CACHE
		                                      : modulePtr->previewTracerCacheMode.load(std::memory_order_relaxed);
		if (!tracerEnabled) {
			curveTracer.clear();
			frameTracer.clear();
		}
		else if (tracerMode == WAVE_PREVIEW_TRACER_CURVE_CACHE) {
			curveTracer.expire(nowSec, TRAIL_FADE_SEC);
			frameTracer.clear();
		}
		else {
			curveTracer.clear();
		}
		if (!pointsValid || version != lastVersion) {
			if (tracerEnabled && pointsValid) {
				if (tracerMode == WAVE_PREVIEW_TRACER_CURVE_CACHE) {
					const WavePreviewTracerCaptureStats stats =
						curveTracer.capture(points, nowSec, TRAIL_MIN_CAPTURE_INTERVAL_SEC, TRAIL_CAPTURE_STRIDE);
					modulePtr->recordTracerExtraPointReduction(channel, stats);
				}
				else {
					WavePreviewBufferedTracerStyle style;
					style.color = nvgRGBA(255, 190, 80, 255);
					style.fadeSec = TRAIL_FADE_SEC;
					style.minCaptureIntervalSec = TRAIL_MIN_CAPTURE_INTERVAL_SEC;
					style.maxAlpha = 118.f;
					style.drawStride = TRAIL_CAPTURE_STRIDE;
					const WavePreviewTracerCaptureStats stats = frameTracer.capture(points, nowSec, box.size, style);
					modulePtr->recordTracerExtraPointReduction(channel, stats);
				}
			}
			rebuildPoints(riseTime, fallTime, curveSigned, interactiveRecent);
			lastVersion = version;
		}
		if (openGlRenderer) {
			setDirty();
			FramebufferWidget::step();
		}
	}

	void draw(const DrawArgs& args) override {
		if (useOpenGlRenderer()) {
			widget::OpenGlWidget::draw(args);
			drawFrequencyLabel(args);
			return;
		}
		nvgSave(args.vg);
		nvgScissor(args.vg, 0.f, 0.f, box.size.x, box.size.y);

		if (pointsValid) {
			const double nowSec = system::getTime();
			const bool tracerEnabled = modulePtr && modulePtr->previewTracerEnabled.load(std::memory_order_relaxed);
			if (tracerEnabled) {
				const int tracerMode = modulePtr->previewTracerCacheMode.load(std::memory_order_relaxed);
				if (tracerMode == WAVE_PREVIEW_TRACER_CURVE_CACHE) {
					WavePreviewTracerStyle style;
					style.color = nvgRGBA(255, 190, 80, 255);
					style.lineWidth = TRAIL_LINE_WIDTH;
					style.fadeSec = TRAIL_FADE_SEC;
					style.minCaptureIntervalSec = TRAIL_MIN_CAPTURE_INTERVAL_SEC;
					style.maxAlpha = 118.f;
					style.drawStride = TRAIL_DRAW_STRIDE;
					curveTracer.draw(args.vg, nowSec, style);
				}
				else {
					WavePreviewBufferedTracerStyle style;
					style.color = nvgRGBA(255, 190, 80, 255);
					style.fadeSec = TRAIL_FADE_SEC;
					style.minCaptureIntervalSec = TRAIL_MIN_CAPTURE_INTERVAL_SEC;
					style.maxAlpha = 118.f;
					style.drawStride = TRAIL_DRAW_STRIDE;
					frameTracer.draw(args.vg, nowSec, box.size, style);
				}
			}
			auto vg = args.vg;
			nvgBeginPath(vg);
			size_t reducedPointCount = 0;
			wave_preview::simplifyPath(points.data(), POINT_COUNT, 1, 0.02f, [vg, &reducedPointCount](const Vec& pt, bool isMove) {
				++reducedPointCount;
				if (isMove) {
					nvgMoveTo(vg, pt.x, pt.y);
				} else {
					nvgLineTo(vg, pt.x, pt.y);
				}
			});
			if (modulePtr) {
				modulePtr->recordCurvePointReduction(channel, POINT_COUNT, reducedPointCount);
			}
			nvgStrokeColor(args.vg, nvgRGBA(230, 230, 220, 255));
			nvgStrokeWidth(args.vg, WAVE_LINE_WIDTH);
			nvgLineCap(args.vg, NVG_BUTT);
			nvgLineJoin(args.vg, NVG_ROUND);
			nvgStroke(args.vg);
		}
		if (pointsValid && dotVisible) {
			float w = std::max(box.size.x, 1.f);
			float h = std::max(box.size.y, 1.f);
			float drawPad = 0.5f * WAVE_LINE_WIDTH + WAVE_EDGE_PAD;
			float left = drawPad;
			float top = drawPad;
			float right = std::max(left + 1.f, w - drawPad);
			float bottom = std::max(top + 1.f, h - drawPad);
			float drawW = right - left;
			float drawH = bottom - top;
			float targetX = left + clamp(dotXNorm, 0.f, 1.f) * drawW;
			float targetY = top + (1.f - clamp(dotYNorm, 0.f, 1.f)) * drawH;
			// Keep the marker on the drawn curve, but use continuous interpolation
			// across neighboring points to avoid visible stepping at slow rates.
			int i0 = 0;
			for (int i = 1; i < POINT_COUNT; ++i) {
				if (points[i].x >= targetX) {
					i0 = i - 1;
					break;
				}
				i0 = i - 1;
			}
			int i1 = std::min(i0 + 1, POINT_COUNT - 1);
			float x0 = points[i0].x;
			float x1 = points[i1].x;
			float x = targetX;
			float y = points[i0].y;
			if (i1 != i0 && x1 > x0) {
				float t = clamp((targetX - x0) / (x1 - x0), 0.f, 1.f);
				y = points[i0].y + (points[i1].y - points[i0].y) * t;
			}
			float blendToCurve = 0.9f;
			y = y * blendToCurve + targetY * (1.f - blendToCurve);
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, x, y, DOT_RADIUS);
			nvgFillColor(args.vg, nvgRGBA(255, 232, 72, 255));
			nvgFill(args.vg);
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, x, y, DOT_RADIUS + 0.55f);
			nvgStrokeWidth(args.vg, 0.9f);
			nvgStrokeColor(args.vg, nvgRGBA(0, 0, 0, 220));
			nvgStroke(args.vg);
		}

		nvgResetScissor(args.vg);
		nvgRestore(args.vg);

		// Keep label outside preview box to avoid occluding waveform.
		drawFrequencyLabel(args);
	}
};

static math::Rect insetRectMm(math::Rect rect, float insetMm) {
	rect.pos.x += insetMm;
	rect.pos.y += insetMm;
	rect.size.x = std::max(0.f, rect.size.x - 2.f * insetMm);
	rect.size.y = std::max(0.f, rect.size.y - 2.f * insetMm);
	return rect;
}

struct IntegralFluxHalo2Knob : LeviathanHaloKnob2 {
	IntegralFluxHalo2Knob() = default;
	explicit IntegralFluxHalo2Knob(Config config) : LeviathanHaloKnob2(config) {}

	void draw(const DrawArgs& args) override {
		if (!isDragonKingDebugEnabled()) {
			LeviathanHaloKnob2::draw(args);
			return;
		}
		using PerfClock = std::chrono::steady_clock;
		const PerfClock::time_point drawStart = PerfClock::now();
		LeviathanHaloKnob2::draw(args);
		gIntegralFluxGearDrawNsThisFrame += uint64_t(std::chrono::duration_cast<std::chrono::nanoseconds>(
			PerfClock::now() - drawStart).count());
	}
};

struct IntegralFluxCurveHalo2Knob : IntegralFluxHalo2Knob {
	IntegralFluxCurveHalo2Knob() : IntegralFluxHalo2Knob(LeviathanHaloKnob2::brightOrangeConfig()) {
	}
};

// Deprecated old EclipseKnob wrapper, replaced by Eclipse2Knob

struct IntegralFluxEclipse2Knob : Eclipse2Knob {
	void draw(const DrawArgs& args) override {
		if (!isDragonKingDebugEnabled()) {
			Eclipse2Knob::draw(args);
			return;
		}
		using PerfClock = std::chrono::steady_clock;
		const PerfClock::time_point drawStart = PerfClock::now();
		Eclipse2Knob::draw(args);
		gIntegralFluxEclipseDrawNsThisFrame += uint64_t(std::chrono::duration_cast<std::chrono::nanoseconds>(
			PerfClock::now() - drawStart).count());
	}
};

template <typename TBase>
struct IntegralFluxTimedApertureLight : TBase {
	void draw(const typename TBase::DrawArgs& args) override {
		if (!isDragonKingDebugEnabled()) {
			TBase::draw(args);
			return;
		}
		using PerfClock = std::chrono::steady_clock;
		const PerfClock::time_point drawStart = PerfClock::now();
		TBase::draw(args);
		gIntegralFluxApertureDrawNsThisFrame += uint64_t(std::chrono::duration_cast<std::chrono::nanoseconds>(
			PerfClock::now() - drawStart).count());
	}
};

struct IntegralFluxSplitLayerTestArt : TransparentWidget {
	struct GlassRectArt {
		math::Rect rectPx;
		float radiusPx = 0.f;
		NVGcolor baseColor = nvgRGB(87, 64, 191);
	};

	std::vector<GlassRectArt> glassRects;
	std::vector<math::Rect> screenRectsPx;

	void setGlassRects(std::vector<GlassRectArt> rects) {
		glassRects = std::move(rects);
	}

	void setScreenRects(std::vector<math::Rect> rectsPx) {
		screenRectsPx = std::move(rectsPx);
	}

	static bool rectsIntersect(const math::Rect& a, const math::Rect& b) {
		return a.pos.x < b.pos.x + b.size.x
			&& a.pos.x + a.size.x > b.pos.x
			&& a.pos.y < b.pos.y + b.size.y
			&& a.pos.y + a.size.y > b.pos.y;
	}

	static math::Rect rectIntersection(const math::Rect& a, const math::Rect& b) {
		const float x0 = std::max(a.pos.x, b.pos.x);
		const float y0 = std::max(a.pos.y, b.pos.y);
		const float x1 = std::min(a.pos.x + a.size.x, b.pos.x + b.size.x);
		const float y1 = std::min(a.pos.y + a.size.y, b.pos.y + b.size.y);
		return math::Rect(Vec(x0, y0), Vec(std::max(0.f, x1 - x0), std::max(0.f, y1 - y0)));
	}

	static void subtractRect(std::vector<math::Rect>& pieces, const math::Rect& cut) {
		std::vector<math::Rect> next;
		next.reserve(pieces.size() + 3u);
		for (const math::Rect& piece : pieces) {
			if (!rectsIntersect(piece, cut)) {
				next.push_back(piece);
				continue;
			}
			const math::Rect inter = rectIntersection(piece, cut);
			const float px0 = piece.pos.x;
			const float py0 = piece.pos.y;
			const float px1 = piece.pos.x + piece.size.x;
			const float py1 = piece.pos.y + piece.size.y;
			const float ix0 = inter.pos.x;
			const float iy0 = inter.pos.y;
			const float ix1 = inter.pos.x + inter.size.x;
			const float iy1 = inter.pos.y + inter.size.y;
			auto addPiece = [&next](float x0, float y0, float x1, float y1) {
				if (x1 - x0 > 0.5f && y1 - y0 > 0.5f) {
					next.push_back(math::Rect(Vec(x0, y0), Vec(x1 - x0, y1 - y0)));
				}
			};
			addPiece(px0, py0, px1, iy0);
			addPiece(px0, iy1, px1, py1);
			addPiece(px0, iy0, ix0, iy1);
			addPiece(ix1, iy0, px1, iy1);
		}
		pieces = std::move(next);
	}

	void drawGlassRectPiece(const DrawArgs& args, const GlassRectArt& glass, const math::Rect& piece) {
		const float x = glass.rectPx.pos.x;
		const float y = glass.rectPx.pos.y;
		const float w = glass.rectPx.size.x;
		const float h = glass.rectPx.size.y;
		if (!(w > 2.f && h > 2.f && piece.size.x > 0.5f && piece.size.y > 0.5f)) {
			return;
		}

		const float sourceRadius = glass.radiusPx > 0.f ? glass.radiusPx : std::min(std::min(w, h) * 0.085f, 8.0f);
		const float r = clamp(sourceRadius, 0.f, std::min(w, h) * 0.5f);
		const NVGcolor base = glass.baseColor;
		const NVGcolor cyan = nvgRGB(0x1c, 0xcc, 0xd9);
		const NVGcolor violet = nvgRGB(0x7a, 0x5c, 0xff);

		NVGpaint outerGlow = nvgBoxGradient(
			args.vg,
			x - 1.5f,
			y - 1.5f,
			w + 3.0f,
			h + 3.0f,
			r + 2.0f,
			7.0f,
			nvgRGBAf(base.r, base.g, base.b, 0.075f),
			nvgRGBA(0, 0, 0, 0));
		nvgSave(args.vg);
		nvgScissor(args.vg, piece.pos.x, piece.pos.y, piece.size.x, piece.size.y);
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, x - 1.5f, y - 1.5f, w + 3.0f, h + 3.0f, r + 2.0f);
		nvgFillPaint(args.vg, outerGlow);
		nvgFill(args.vg);

		NVGpaint glassFill = nvgLinearGradient(
			args.vg,
			x,
			y,
			x,
			y + h,
			nvgRGBA(255, 255, 255, 14),
			nvgRGBAf(base.r, base.g, base.b, 0.04f));
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, x + 0.6f, y + 0.6f, w - 1.2f, h - 1.2f, r);
		nvgFillPaint(args.vg, glassFill);
		nvgFill(args.vg);

		nvgSave(args.vg);
		nvgScissor(args.vg, x + 1.f, y + 1.f, w - 2.f, h - 2.f);
		NVGpaint sheen = nvgLinearGradient(
			args.vg,
			x + w * 0.12f,
			y + h * 0.05f,
			x + w * 0.55f,
			y + h * 0.62f,
			nvgRGBA(255, 255, 255, 10),
			nvgRGBA(255, 255, 255, 0));
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, x + w * 0.06f, y);
		nvgLineTo(args.vg, x + w * 0.23f, y);
		nvgLineTo(args.vg, x + w * 0.64f, y + h);
		nvgLineTo(args.vg, x + w * 0.46f, y + h);
		nvgClosePath(args.vg);
		nvgFillPaint(args.vg, sheen);
		nvgFill(args.vg);
		nvgRestore(args.vg);

		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, x + 0.75f, y + 0.75f, w - 1.5f, h - 1.5f, r);
		nvgStrokeWidth(args.vg, 0.85f);
		nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 17));
		nvgStroke(args.vg);

		NVGpaint edge = nvgLinearGradient(
			args.vg,
			x,
			y,
			x + w,
			y + h,
			nvgRGBAf(violet.r, violet.g, violet.b, 0.16f),
			nvgRGBAf(cyan.r, cyan.g, cyan.b, 0.12f));
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, x + 1.35f, y + 1.35f, w - 2.7f, h - 2.7f, std::max(1.f, r - 1.f));
		nvgStrokeWidth(args.vg, 0.55f);
		nvgStrokePaint(args.vg, edge);
		nvgStroke(args.vg);

		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, x + r + 2.f, y + 2.2f);
		nvgLineTo(args.vg, x + w - r - 2.f, y + 2.2f);
		nvgStrokeWidth(args.vg, 0.8f);
		nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 24));
		nvgStroke(args.vg);
		nvgRestore(args.vg);
	}

	void drawGlassRect(const DrawArgs& args, const GlassRectArt& glass) {
		std::vector<math::Rect> pieces;
		pieces.push_back(glass.rectPx);
		for (const math::Rect& screen : screenRectsPx) {
			subtractRect(pieces, screen);
			if (pieces.empty()) {
				break;
			}
		}
		for (const math::Rect& piece : pieces) {
			drawGlassRectPiece(args, glass, piece);
		}
	}

	void drawScreenGrid(const DrawArgs& args, const math::Rect& screen) {
		const float x = screen.pos.x;
		const float y = screen.pos.y;
		const float w = screen.size.x;
		const float h = screen.size.y;
		if (!(w > 4.f && h > 4.f)) {
			return;
		}

		nvgSave(args.vg);
		nvgScissor(args.vg, x, y, w, h);

		const int majorCols = std::max(3, int(std::round(w / 16.0f)));
		const int majorRows = std::max(3, int(std::round(h / 16.0f)));
		const int minorSubdivisions = 4;
		const float majorX = w / float(majorCols);
		const float majorY = h / float(majorRows);
		const NVGcolor minorColor = nvgRGBA(0x1c, 0xcc, 0xd9, 30);
		const NVGcolor majorColor = nvgRGBA(0x72, 0x8d, 0xff, 46);

		nvgBeginPath(args.vg);
		for (int col = 0; col < majorCols; ++col) {
			const float cellX = x + float(col) * majorX;
			for (int sub = 1; sub < minorSubdivisions; ++sub) {
				const float gx = cellX + majorX * (float(sub) / float(minorSubdivisions));
				nvgMoveTo(args.vg, gx, y);
				nvgLineTo(args.vg, gx, y + h);
			}
		}
		for (int row = 0; row < majorRows; ++row) {
			const float cellY = y + float(row) * majorY;
			for (int sub = 1; sub < minorSubdivisions; ++sub) {
				const float gy = cellY + majorY * (float(sub) / float(minorSubdivisions));
				nvgMoveTo(args.vg, x, gy);
				nvgLineTo(args.vg, x + w, gy);
			}
		}
		nvgStrokeWidth(args.vg, 0.38f);
		nvgStrokeColor(args.vg, minorColor);
		nvgStroke(args.vg);

		nvgBeginPath(args.vg);
		for (int col = 1; col < majorCols; ++col) {
			const float gx = x + float(col) * majorX;
			nvgMoveTo(args.vg, gx, y);
			nvgLineTo(args.vg, gx, y + h);
		}
		for (int row = 1; row < majorRows; ++row) {
			const float gy = y + float(row) * majorY;
			nvgMoveTo(args.vg, x, gy);
			nvgLineTo(args.vg, x + w, gy);
		}
		nvgStrokeWidth(args.vg, 0.55f);
		nvgStrokeColor(args.vg, majorColor);
		nvgStroke(args.vg);

		NVGpaint vignette = nvgBoxGradient(
			args.vg,
			x + 1.f,
			y + 1.f,
			w - 2.f,
			h - 2.f,
			1.5f,
			9.0f,
			nvgRGBA(0, 0, 0, 0),
			nvgRGBA(0, 0, 0, 132));
		nvgBeginPath(args.vg);
		nvgRect(args.vg, x, y, w, h);
		nvgFillPaint(args.vg, vignette);
		nvgFill(args.vg);

		NVGpaint edgeGlow = nvgBoxGradient(
			args.vg,
			x + 0.5f,
			y + 0.5f,
			w - 1.f,
			h - 1.f,
			1.5f,
			4.0f,
			nvgRGBA(0x1c, 0xcc, 0xd9, 78),
			nvgRGBA(0x1c, 0xcc, 0xd9, 0));
		nvgBeginPath(args.vg);
		nvgRect(args.vg, x, y, w, h);
		nvgStrokeWidth(args.vg, 1.1f);
		nvgStrokePaint(args.vg, edgeGlow);
		nvgStroke(args.vg);

		nvgRestore(args.vg);
	}

	void draw(const DrawArgs& args) override {
		const float w = box.size.x;
		const float h = box.size.y;
		if (!(w > 0.f && h > 0.f)) {
			return;
		}

		for (const GlassRectArt& glass : glassRects) {
			drawGlassRect(args, glass);
		}
		for (const math::Rect& screen : screenRectsPx) {
			drawScreenGrid(args, screen);
		}
	}
};

struct IntegralFluxWidget : ModuleWidget {
	float uiStepMsEma = 0.f;
	float uiDrawMsEma = 0.f;
	float gearDrawUsEma = 0.f;
	float eclipseDrawUsEma = 0.f;
	debug_terminal::UiTimingRangeAccumulator uiStepUsRange;
	debug_terminal::UiTimingRangeAccumulator uiDrawUsRange;
	debug_terminal::UiTimingRangeAccumulator apertureDrawUsRange;

	static float consumeReductionAverage(std::atomic<uint64_t>& total, std::atomic<uint64_t>& samples) {
		const uint64_t totalValue = total.exchange(0u, std::memory_order_acq_rel);
		const uint64_t sampleCount = samples.exchange(0u, std::memory_order_acq_rel);
		return sampleCount > 0u ? float(double(totalValue) / double(sampleCount)) : 0.f;
	}

	void step() override {
		using PerfClock = std::chrono::steady_clock;
		const bool measurePerf = isDragonKingDebugEnabled();
		const PerfClock::time_point stepStart = measurePerf ? PerfClock::now() : PerfClock::time_point();
		ModuleWidget::step();
		if (measurePerf) {
			const float stepMs = float(std::chrono::duration_cast<std::chrono::nanoseconds>(
				PerfClock::now() - stepStart).count()) * 1e-6f;
			uiStepMsEma = (uiStepMsEma > 0.f) ? (uiStepMsEma + (stepMs - uiStepMsEma) * 0.18f) : stepMs;
			uiStepUsRange.add(stepMs * 1000.f);
		}
	}

	IntegralFluxWidget(IntegralFlux* module) {
		setModule(module);
		PreviewBuildLogTimer previewBuildTimer("IntegralFlux", module);
		const std::string panelPath = asset::plugin(pluginInstance, "res/flux.svg");
		const std::string panelBasePath = asset::plugin(pluginInstance, "res/flux.panel.svg");
		setPanel(createPanel(panelBasePath));
		{
			widget::FramebufferWidget* overlayFb = new widget::FramebufferWidget();
			overlayFb->dirtyOnSubpixelChange = false;
			overlayFb->box.size = box.size;

			IntegralFluxSplitLayerTestArt* testArt = new IntegralFluxSplitLayerTestArt();
			testArt->box.size = box.size;
			{
				static bool cachedGlassRectsLoaded = false;
				static std::vector<IntegralFluxSplitLayerTestArt::GlassRectArt> cachedGlassRects;
				static std::vector<math::Rect> cachedScreenRectsPx;
				if (!cachedGlassRectsLoaded) {
					std::vector<panel_svg::SvgRectMatch> glassMatches;
					if (panel_svg::findRectsInGroupsWithIdSubstringMm(panelBasePath, "glass", &glassMatches)) {
						cachedGlassRects.reserve(glassMatches.size());
						for (const panel_svg::SvgRectMatch& match : glassMatches) {
							IntegralFluxSplitLayerTestArt::GlassRectArt art;
							art.rectPx = math::Rect(mm2px(match.rect.pos), mm2px(match.rect.size));
							if (match.hasCornerRadius) {
								const Vec radiusPx = mm2px(match.cornerRadius);
								art.radiusPx = std::min(radiusPx.x, radiusPx.y);
							}
							if (match.hasFillColor) {
								art.baseColor = match.fillColor;
							}
							cachedGlassRects.push_back(art);
						}
					}
					std::vector<panel_svg::SvgRectMatch> screenMatches;
					if (panel_svg::findRectsInGroupsWithIdSubstringMm(panelBasePath, "screen", &screenMatches)) {
						cachedScreenRectsPx.reserve(screenMatches.size());
						for (const panel_svg::SvgRectMatch& match : screenMatches) {
							math::Rect screenRectMm = insetRectMm(match.rect, 0.2f);
							cachedScreenRectsPx.push_back(math::Rect(mm2px(screenRectMm.pos), mm2px(screenRectMm.size)));
						}
					}
					cachedGlassRectsLoaded = true;
				}
				testArt->setGlassRects(cachedGlassRects);
				testArt->setScreenRects(cachedScreenRectsPx);
			}
			overlayFb->addChild(testArt);

			addChild(overlayFb);
		}
		{
			widget::SvgWidget* labels = new widget::SvgWidget();
			labels->setSvg(visual_assets::loadPluginSvgCached("res/flux.labels.svg"));
			labels->box.size = box.size;

			widget::FramebufferWidget* labelsFb = new widget::FramebufferWidget();
			labelsFb->box.size = box.size;
			labelsFb->oversample = 2.0f;
			labelsFb->dirtyOnSubpixelChange = true;
			labelsFb->addChild(labels);
			addChild(labelsFb);
		}
		{
			math::Rect dragonRectMm;
			if (!panel_svg::loadRectFromSvgMm(panelPath, "DRAGON_RENDER_AREA", &dragonRectMm)) {
				dragonRectMm.pos = Vec(42.1381f, 10.928f);
				dragonRectMm.size = Vec(17.4213f, 35.5849f);
			}
			IntegralFluxFittedSvgWidget* dragon = new IntegralFluxFittedSvgWidget();
			dragon->setSvg(visual_assets::loadPluginSvgCached("res/icon/Leviathan_Optimized.svg"));
			dragon->box.pos = mm2px(dragonRectMm.pos);
			dragon->box.size = mm2px(dragonRectMm.size);
			addChild(dragon);
		}
		previewBuildTimer.markPanelDone();

        // use LeviathanHaloKnob2 for surge/sink and curve shape knobs
        // use SmallAperture LEDs for the indicator lights
        // use EclipseKnob for the attenuverter knobs
        // use TL1105 for the cycle buttons

		addChild(createWidget<CyanOrbScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<CyanOrbScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<CyanOrbScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<CyanOrbScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		Vec cycle1ButtonPos(31.875f, 20.938f);
		Vec cycle4ButtonPos(69.552f, 20.938f);
		Vec rise1KnobPos(33.755f, 36.293f);
		Vec rise4KnobPos(67.638f, 36.293f);
		Vec fall1KnobPos(42.007f, 53.079f);
		Vec fall4KnobPos(59.185f, 53.079f);
		Vec linLog1KnobPos(13.975f, 50.526f);
		Vec linLog4KnobPos(91.716f, 50.526f);
		Vec attenuate1KnobPos(25.494f, 86.446f);
		Vec attenuate2KnobPos(42.542f, 86.446f);
		Vec attenuate3KnobPos(59.585f, 86.446f);
		Vec attenuate4KnobPos(75.931f, 86.446f);
		Vec input1Pos(9.947f, 15.354f);
		Vec input1TrigPos(20.911f, 15.354f);
		Vec input4TrigPos(80.217f, 15.354f);
		Vec input4Pos(91.181f, 15.354f);
		Vec ch1CycleCvPos(40.049f, 20.838f);
		Vec ch4CycleCvPos(61.179f, 20.838f);
		Vec ch1RiseCvPos(21.683f, 36.416f);
		Vec ch4RiseCvPos(79.81f, 36.216f);
		Vec ch1BothCvPos(26.633f, 50.27f);
		Vec ch4BothCvPos(74.56f, 50.07f);
		Vec ch1FallCvPos(32.704f, 63.263f);
		Vec ch4FallCvPos(69.189f, 63.263f);
		Vec input2Pos(42.543f, 76.377f);
		Vec input3Pos(59.585f, 76.377f);
		Vec eor1OutputPos(10.037f, 96.946f);
		Vec out1OutputPos(25.295f, 96.915f);
		Vec out2OutputPos(42.343f, 96.915f);
		Vec out3OutputPos(59.486f, 96.915f);
		Vec out4OutputPos(75.832f, 96.915f);
		Vec eoc4OutputPos(91.281f, 96.915f);
		Vec ch1UnityOutputPos(10.047f, 110.682f);
		Vec orOutputPos(33.652f, 110.882f);
		Vec sumOutputPos(50.714f, 110.882f);
		Vec invOutputPos(67.975f, 110.882f);
		Vec ch4UnityOutputPos(91.281f, 110.682f);
		Vec cycle1LightPos(31.875f, 14.855f);
		Vec cycle4LightPos(69.353f, 14.855f);
		Vec eor1LightPos(16.537f, 96.76f);
		Vec eoc4LightPos(84.603f, 96.716f);
		Vec unity1LightPos(16.547f, 110.499f);
		Vec unity4LightPos(84.731f, 110.599f);
		Vec orLightPos(42.374f, 110.758f);
		Vec invLightPos(59.554f, 110.758f);

		auto applyPointOverride = [&](const char* elementId, Vec* outPos) {
			Vec pointMm;
			if (panel_svg::loadPointFromSvgMm(panelPath, elementId, &pointMm)) {
				*outPos = pointMm;
			}
		};

		applyPointOverride("CYCLE_1", &cycle1ButtonPos);
		applyPointOverride("CYCLE_4", &cycle4ButtonPos);
		applyPointOverride("RISE_1", &rise1KnobPos);
		applyPointOverride("RISE_4", &rise4KnobPos);
		applyPointOverride("FALL_1", &fall1KnobPos);
		applyPointOverride("FALL_4", &fall4KnobPos);
		applyPointOverride("LIN_LOG_1", &linLog1KnobPos);
		applyPointOverride("LIN_LOG_4", &linLog4KnobPos);
		applyPointOverride("ATTENUATE_1", &attenuate1KnobPos);
		applyPointOverride("ATTENUATE_2", &attenuate2KnobPos);
		applyPointOverride("ATTENUATE_3", &attenuate3KnobPos);
		applyPointOverride("ATTENUATE_4", &attenuate4KnobPos);
		applyPointOverride("INPUT_1", &input1Pos);
		applyPointOverride("INPUT_1_TRIG", &input1TrigPos);
		applyPointOverride("INPUT_4_TRIG", &input4TrigPos);
		applyPointOverride("INPUT_4", &input4Pos);
		applyPointOverride("CH1_CYCLE_CV", &ch1CycleCvPos);
		applyPointOverride("CH4_CYCLE_CV", &ch4CycleCvPos);
		applyPointOverride("CH1_RISE_CV", &ch1RiseCvPos);
		applyPointOverride("CH4_RISE_CV", &ch4RiseCvPos);
		applyPointOverride("CH1_BOTH_CV", &ch1BothCvPos);
		applyPointOverride("CH4_BOTH_CV", &ch4BothCvPos);
		applyPointOverride("CH1_FALL_CV", &ch1FallCvPos);
		applyPointOverride("CH4_FALL_CV", &ch4FallCvPos);
		applyPointOverride("INPUT_2", &input2Pos);
		applyPointOverride("INPUT_3", &input3Pos);
			applyPointOverride("EOR_1", &eor1OutputPos);
			applyPointOverride("OUT_1", &out1OutputPos);
			applyPointOverride("OUT_2", &out2OutputPos);
			applyPointOverride("OUT_3", &out3OutputPos);
			applyPointOverride("OUT_4", &out4OutputPos);
			applyPointOverride("EOC_4", &eoc4OutputPos);
			applyPointOverride("CH_1_Unity", &ch1UnityOutputPos);
			applyPointOverride("OR_OUT", &orOutputPos);
			applyPointOverride("SUM_OUT", &sumOutputPos);
			applyPointOverride("INV_OUT", &invOutputPos);
			applyPointOverride("CH_4_Unity", &ch4UnityOutputPos);
			applyPointOverride("CYCLE_1_LED", &cycle1LightPos);
			applyPointOverride("CYCLE_4_LED", &cycle4LightPos);
			applyPointOverride("EoR_CH_1", &eor1LightPos);
			applyPointOverride("EoC_CH_4", &eoc4LightPos);
			applyPointOverride("Light_Unity_1", &unity1LightPos);
			applyPointOverride("Light_Unity_4", &unity4LightPos);
			applyPointOverride("OR_LED", &orLightPos);
			applyPointOverride("INV_LED", &invLightPos);
		previewBuildTimer.setAtlasStatus(panel_svg::getAtlasStatusLabelForSvg(panelPath));
		previewBuildTimer.markAnchorsDone();

		addParam(createParamCentered<GoldButton>(mm2px(cycle1ButtonPos), module, IntegralFlux::CYCLE_1_PARAM));
		addParam(createParamCentered<GoldButton>(mm2px(cycle4ButtonPos), module, IntegralFlux::CYCLE_4_PARAM));

        addParam(createParamCentered<IntegralFluxHalo2Knob>(mm2px(rise1KnobPos), module, IntegralFlux::RISE_1_PARAM));
		addParam(createParamCentered<IntegralFluxHalo2Knob>(mm2px(rise4KnobPos), module, IntegralFlux::RISE_4_PARAM));
		addParam(createParamCentered<IntegralFluxHalo2Knob>(mm2px(fall1KnobPos), module, IntegralFlux::FALL_1_PARAM));
		addParam(createParamCentered<IntegralFluxHalo2Knob>(mm2px(fall4KnobPos), module, IntegralFlux::FALL_4_PARAM));
		addParam(createParamCentered<IntegralFluxCurveHalo2Knob>(mm2px(linLog1KnobPos), module, IntegralFlux::LIN_LOG_1_PARAM));
		addParam(createParamCentered<IntegralFluxCurveHalo2Knob>(mm2px(linLog4KnobPos), module, IntegralFlux::LIN_LOG_4_PARAM));
		{
			WavePreviewWidget* ch1Preview = new WavePreviewWidget(module, 1);
			math::Rect previewRectMm;
			if (panel_svg::loadRectFromSvgMm(panelPath, "CH1_PREVIEW", &previewRectMm)) {
				addChild(visual_assets::createPreviewFrameEnhancementWidget(previewRectMm));
				previewRectMm = insetRectMm(previewRectMm, 0.2f);
				ch1Preview->box.pos = mm2px(previewRectMm.pos);
				ch1Preview->box.size = mm2px(previewRectMm.size);
			}
			else {
				math::Rect previewFallbackMm(Vec(3.75998355f, 68.96602539f), Vec(20.78393382f, 11.24561948f));
				addChild(visual_assets::createPreviewFrameEnhancementWidget(previewFallbackMm));
				previewFallbackMm = insetRectMm(previewFallbackMm, 0.2f);
				ch1Preview->box.pos = mm2px(previewFallbackMm.pos);
				ch1Preview->box.size = mm2px(previewFallbackMm.size);
			}
			addChild(ch1Preview);
		}
		{
			WavePreviewWidget* ch4Preview = new WavePreviewWidget(module, 4);
			math::Rect previewRectMm;
			if (panel_svg::loadRectFromSvgMm(panelPath, "CH4_PREVIEW", &previewRectMm)) {
				addChild(visual_assets::createPreviewFrameEnhancementWidget(previewRectMm));
				previewRectMm = insetRectMm(previewRectMm, 0.2f);
				ch4Preview->box.pos = mm2px(previewRectMm.pos);
				ch4Preview->box.size = mm2px(previewRectMm.size);
			}
			else {
				math::Rect previewFallbackMm(Vec(77.52500000f, 68.96600100f), Vec(20.78393300f, 11.24562000f));
				addChild(visual_assets::createPreviewFrameEnhancementWidget(previewFallbackMm));
				previewFallbackMm = insetRectMm(previewFallbackMm, 0.2f);
				ch4Preview->box.pos = mm2px(previewFallbackMm.pos);
				ch4Preview->box.size = mm2px(previewFallbackMm.size);
			}
			addChild(ch4Preview);
		}

		auto addBipolarEclipse2Knob = [&](Vec posMm, int paramId) {
			IntegralFluxEclipse2Knob* knob = createParamCentered<IntegralFluxEclipse2Knob>(mm2px(posMm), module, paramId);
			knob->setProgressRingBipolar(true);
			addParam(knob);
		};
		addBipolarEclipse2Knob(attenuate1KnobPos, IntegralFlux::ATTENUATE_1_PARAM);
		addBipolarEclipse2Knob(attenuate2KnobPos, IntegralFlux::ATTENUATE_2_PARAM);
		addBipolarEclipse2Knob(attenuate3KnobPos, IntegralFlux::ATTENUATE_3_PARAM);
		addBipolarEclipse2Knob(attenuate4KnobPos, IntegralFlux::ATTENUATE_4_PARAM);

		addInput(createInputCentered<Magitek2InputJack>(mm2px(input1Pos), module, IntegralFlux::INPUT_1_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(input1TrigPos), module, IntegralFlux::INPUT_1_TRIG_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(input4TrigPos), module, IntegralFlux::INPUT_4_TRIG_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(input4Pos), module, IntegralFlux::INPUT_4_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(ch1CycleCvPos), module, IntegralFlux::CH1_CYCLE_CV_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(ch4CycleCvPos), module, IntegralFlux::CH4_CYCLE_CV_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(ch1RiseCvPos), module, IntegralFlux::CH1_RISE_CV_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(ch4RiseCvPos), module, IntegralFlux::CH4_RISE_CV_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(ch1BothCvPos), module, IntegralFlux::CH1_BOTH_CV_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(ch4BothCvPos), module, IntegralFlux::CH4_BOTH_CV_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(ch1FallCvPos), module, IntegralFlux::CH1_FALL_CV_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(ch4FallCvPos), module, IntegralFlux::CH4_FALL_CV_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(input2Pos), module, IntegralFlux::INPUT_2_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(input3Pos), module, IntegralFlux::INPUT_3_INPUT));

		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(eor1OutputPos), module, IntegralFlux::EOR_1_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(out1OutputPos), module, IntegralFlux::OUT_1_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(out2OutputPos), module, IntegralFlux::OUT_2_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(out3OutputPos), module, IntegralFlux::OUT_3_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(out4OutputPos), module, IntegralFlux::OUT_4_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(eoc4OutputPos), module, IntegralFlux::EOC_4_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(ch1UnityOutputPos), module, IntegralFlux::CH_1_UNITY_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(orOutputPos), module, IntegralFlux::OR_OUT_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(sumOutputPos), module, IntegralFlux::SUM_OUT_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(invOutputPos), module, IntegralFlux::INV_OUT_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(ch4UnityOutputPos), module, IntegralFlux::CH_4_UNITY_OUTPUT));

		addChild(createLightCentered<IntegralFluxTimedApertureLight<SmallAperture<AmberApertureLight>>>(mm2px(cycle1LightPos), module, IntegralFlux::CYCLE_1_LED_LIGHT));
		addChild(createLightCentered<IntegralFluxTimedApertureLight<SmallAperture<AmberApertureLight>>>(mm2px(cycle4LightPos), module, IntegralFlux::CYCLE_4_LED_LIGHT));
		addChild(createLightCentered<IntegralFluxTimedApertureLight<SmallAperture<AmberApertureLight>>>(mm2px(eor1LightPos), module, IntegralFlux::EOR_CH_1_LIGHT));
		addChild(createLightCentered<IntegralFluxTimedApertureLight<SmallAperture<AmberApertureLight>>>(mm2px(eoc4LightPos), module, IntegralFlux::EOC_CH_4_LIGHT));
		addChild(createLightCentered<IntegralFluxTimedApertureLight<SmallAperture<GreenApertureLight>>>(mm2px(unity1LightPos), module, IntegralFlux::LIGHT_UNITY_1_LIGHT));
		addChild(createLightCentered<IntegralFluxTimedApertureLight<SmallAperture<GreenApertureLight>>>(mm2px(unity4LightPos), module, IntegralFlux::LIGHT_UNITY_4_LIGHT));
		addChild(createLightCentered<IntegralFluxTimedApertureLight<SmallAperture<MagentaApertureLight>>>(mm2px(orLightPos), module, IntegralFlux::OR_LED_LIGHT));
		addChild(createLightCentered<IntegralFluxTimedApertureLight<SmallAperture<GreenApertureLight>>>(mm2px(invLightPos), module, IntegralFlux::INV_LED_LIGHT));
	}

	void draw(const DrawArgs& args) override {
		using PerfClock = std::chrono::steady_clock;
		const bool measurePerf = isDragonKingDebugEnabled();
		if (measurePerf) {
			gIntegralFluxGearDrawNsThisFrame = 0u;
			gIntegralFluxEclipseDrawNsThisFrame = 0u;
			gIntegralFluxApertureDrawNsThisFrame = 0u;
			visual_assets::resetEclipseShadowDrawMetrics();
		}
		const PerfClock::time_point perfStart = measurePerf ? PerfClock::now() : PerfClock::time_point();
		ModuleWidget::draw(args);
		IntegralFlux* flux = static_cast<IntegralFlux*>(module);
		if (!flux) {
			return;
		}
		if (measurePerf) {
			const float drawMs = float(std::chrono::duration_cast<std::chrono::nanoseconds>(
				PerfClock::now() - perfStart).count()) * 1e-6f;
			uiDrawMsEma = (uiDrawMsEma > 0.f) ? (uiDrawMsEma + (drawMs - uiDrawMsEma) * 0.18f) : drawMs;
			uiDrawUsRange.add(drawMs * 1000.f);
			const float gearDrawUs = float(gIntegralFluxGearDrawNsThisFrame) * 1e-3f;
			gearDrawUsEma = (gearDrawUsEma > 0.f) ? (gearDrawUsEma + (gearDrawUs - gearDrawUsEma) * 0.18f) : gearDrawUs;
			const float eclipseDrawUs = float(gIntegralFluxEclipseDrawNsThisFrame) * 1e-3f;
			eclipseDrawUsEma = (eclipseDrawUsEma > 0.f) ? (eclipseDrawUsEma + (eclipseDrawUs - eclipseDrawUsEma) * 0.18f) : eclipseDrawUs;
			apertureDrawUsRange.add(float(gIntegralFluxApertureDrawNsThisFrame) * 1e-3f);
			const float uiMs = std::max(0.f, uiStepMsEma) + std::max(0.f, uiDrawMsEma);
			flux->perfUiRenderMs.store(std::max(0.f, uiMs), std::memory_order_relaxed);
		}

		if (measurePerf) {
			double nowSec = system::getTime();
			double& lastSubmitSec = gIntegralFluxDebugTerminalLastSubmitSec[flux->debugInstanceId];
			if (lastSubmitSec <= 0.0 || (nowSec - lastSubmitSec) >= kIntegralFluxDebugTerminalSubmitIntervalSec) {
				lastSubmitSec = nowSec;
				flux->perfAudioSampledCount.exchange(0, std::memory_order_acq_rel);
				flux->perfAudioProcessNs.exchange(0, std::memory_order_acq_rel);
				const float ch1CurvePointsReducedAvg = consumeReductionAverage(
					flux->debugCurvePointsReducedTotal[0], flux->debugCurveReductionSamples[0]);
				const float ch1TracerExtraPointsReducedAvg = consumeReductionAverage(
					flux->debugTracerExtraPointsReducedTotal[0], flux->debugTracerReductionSamples[0]);
				debug_terminal::submitIntegralFluxMetrics(
					flux->debugInstanceId,
					debug_terminal::consumeAudioProcessTiming(flux->perfAudioProcessMinNs, flux->perfAudioProcessMaxNs),
					uiStepUsRange.consume(),
					uiDrawUsRange.consume(),
					apertureDrawUsRange.consume(),
					gearDrawUsEma,
					eclipseDrawUsEma,
					ch1CurvePointsReducedAvg,
					ch1TracerExtraPointsReducedAvg);
			}
			if (APP && APP->window && APP->window->uiFont) {
				char debugIdLabel[32];
				std::snprintf(debugIdLabel, sizeof(debugIdLabel), "ID:%u", flux->debugInstanceId);
				const float x = box.size.x - mm2px(0.9f);
				const float y = mm2px(2.5f);
				nvgSave(args.vg);
				nvgFontFaceId(args.vg, APP->window->uiFont->handle);
				nvgFontSize(args.vg, 6.8f);
				nvgTextAlign(args.vg, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
				nvgFillColor(args.vg, nvgRGBA(8, 10, 14, 210));
				nvgText(args.vg, x + 0.45f, y + 0.45f, debugIdLabel, nullptr);
				nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 230));
				nvgText(args.vg, x, y, debugIdLabel, nullptr);
				nvgRestore(args.vg);
			}
		}
	}

	void appendContextMenu(Menu* menu) override {
		IntegralFlux* maths = static_cast<IntegralFlux*>(module);
		assert(menu);

		menu->addChild(new MenuSeparator());
		if (maths) {
			menu->addChild(createMenuLabel("Performance"));
			menu->addChild(createCheckMenuItem("Bandlimited EOR/EOC", "",
				[=]() { return maths->bandlimitedGateOutputs.load(std::memory_order_relaxed); },
				[=]() { maths->bandlimitedGateOutputs.store(!maths->bandlimitedGateOutputs.load(std::memory_order_relaxed), std::memory_order_relaxed); }
			));
			menu->addChild(createCheckMenuItem("Bandlimited CH1/CH4 Signal Outputs", "",
				[=]() { return maths->bandlimitedSignalOutputs.load(std::memory_order_relaxed); },
				[=]() { maths->bandlimitedSignalOutputs.store(!maths->bandlimitedSignalOutputs.load(std::memory_order_relaxed), std::memory_order_relaxed); }
			));
			menu->addChild(createMenuLabel("Preview Visual"));
			if (isDragonKingPreviewWidgetOptionsEnabled()) {
				menu->addChild(createSubmenuItem("Render", "",
					[=](Menu* submenu) {
						submenu->addChild(createCheckMenuItem("NanoVG", "",
							[=]() { return maths->previewRenderMode.load(std::memory_order_relaxed) == 0; },
							[=]() { maths->previewRenderMode.store(0, std::memory_order_relaxed); }
						));
						submenu->addChild(createCheckMenuItem("OpenGL", "",
							[=]() { return maths->previewRenderMode.load(std::memory_order_relaxed) == 1; },
							[=]() { maths->previewRenderMode.store(1, std::memory_order_relaxed); }
						));
					}
				));
			}
			menu->addChild(createCheckMenuItem("Preview Tracer", "",
				[=]() { return maths->previewTracerEnabled.load(std::memory_order_relaxed); },
				[=]() { maths->previewTracerEnabled.store(!maths->previewTracerEnabled.load(std::memory_order_relaxed), std::memory_order_relaxed); }
			));
			if (isDragonKingPreviewWidgetOptionsEnabled()) {
				menu->addChild(createSubmenuItem("Tracer Quality", "",
					[=](Menu* submenu) {
						submenu->addChild(createCheckMenuItem("Curve cache", "",
							[=]() { return maths->previewTracerCacheMode.load(std::memory_order_relaxed) == WAVE_PREVIEW_TRACER_CURVE_CACHE; },
							[=]() { maths->previewTracerCacheMode.store(WAVE_PREVIEW_TRACER_CURVE_CACHE, std::memory_order_relaxed); }
						));
						submenu->addChild(createCheckMenuItem("Frame cache", "",
							[=]() { return maths->previewTracerCacheMode.load(std::memory_order_relaxed) == WAVE_PREVIEW_TRACER_FRAME_CACHE; },
							[=]() { maths->previewTracerCacheMode.store(WAVE_PREVIEW_TRACER_FRAME_CACHE, std::memory_order_relaxed); }
						));
					}
				));
			}
			menu->addChild(createMenuLabel("Rate Control"));
			menu->addChild(createCheckMenuItem("Interpolate Timing Updates", "",
				[=]() { return maths->timingInterpolate.load(std::memory_order_relaxed); },
				[=]() { maths->timingInterpolate.store(!maths->timingInterpolate.load(std::memory_order_relaxed), std::memory_order_relaxed); }
			));
			menu->addChild(createSubmenuItem("Timing Update Rate", "",
				[=](Menu* submenu) {
					auto addDivItem = [=](int div, std::string label) {
						submenu->addChild(createCheckMenuItem(label, "",
							[=]() { return maths->requestedTimingUpdateDiv.load(std::memory_order_relaxed) == div; },
							[=]() { maths->requestTimingUpdateDiv(div); }
						));
					};
					addDivItem(1, "Audio rate (/1)");
					addDivItem(4, "Control rate (/4)");
					addDivItem(8, "Control rate (/8)");
					addDivItem(16, "Control rate (/16)");
					addDivItem(32, "Control rate (/32)");
				}
			));
		}
	}
};

} // namespace

Model* modelIntegralFlux = createModel<IntegralFlux, IntegralFluxWidget>("IntegralFlux");
