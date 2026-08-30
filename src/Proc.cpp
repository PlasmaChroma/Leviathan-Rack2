#include "plugin.hpp"
#include "DebugTerminalMetrics.hpp"
#include "MathHelpers.hpp"
#include "PanelSvgUtils.hpp"
#include "visual/VisualAssets.hpp"
#include "visual/FractalGlassOverlay.hpp"
#include "visual/PlasmaConduit.hpp"
#include "visual/PreviewSurface.hpp"
#include "WavePreviewTracer.hpp"
#include <dsp/minblep.hpp>
#include <array>
#include <cstdio>
#include <atomic>
#include <cmath>
#include <vector>

namespace {
std::atomic<uint32_t> gProcDebugInstanceCounter {1u};

struct ProcPercentQuantity final : ParamQuantity {
	float getDisplayValue() override {
		return getValue() * 100.f;
	}

	void setDisplayValue(float displayValue) override {
		setValue(displayValue / 100.f);
	}

	std::string getDisplayValueString() override {
		char buf[32];
		std::snprintf(buf, sizeof(buf), "%.1f", getDisplayValue());
		return buf;
	}
};
}

struct Proc : Module {
	ModuleTeardownTimer teardownTimer {"Proc"};
	// Panel/control IDs are intentionally ordered to match panel layout and existing patches.
	enum ParamId {
		CYCLE_PARAM,
		RISE_PARAM,
		FALL_PARAM,
		SHAPE_PARAM,
		AMP_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		SIGNAL_INPUT,
		TRIGGER_INPUT,
		HALT_INPUT,
		RISE_CV_INPUT,
		BOTH_CV_INPUT,
		FALL_CV_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		EOR_OUTPUT,
		EOC_OUTPUT,
		MAIN_OUTPUT,
		NEG_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		CYCLE_LIGHT,
		EOR_LIGHT,
		EOC_LIGHT,
		MAIN_LIGHT,
		NEG_LIGHT,
		LIGHTS_LEN
	};

	enum ChannelPhase {
		// IDLE: no active function cycle unless cycle mode is engaged.
		// RISE/FALL: function-generator mode integrates toward 8V then 0V.
		CHANNEL_IDLE,
		CHANNEL_RISE,
		CHANNEL_FALL
	};

	struct ChannelState {
		// Edge detectors for trigger input and momentary cycle button.
		dsp::SchmittTrigger trigEdge;
		dsp::SchmittTrigger cycleButtonEdge;
		// Optional anti-alias compensation for hard output steps.
		dsp::MinBlepGenerator<16, 16> eorGateBlep;
		dsp::MinBlepGenerator<16, 16> eocGateBlep;
		dsp::MinBlepGenerator<16, 16> signalBlep;

		ChannelPhase phase = CHANNEL_IDLE;
		// phasePos is a normalized [0..1+] phase accumulator for the active segment.
		float phasePos = 0.f;
		float out = 0.f;
		// Slew warp phase tracking for processUnifiedShapedSlew().
		int slewDir = 0;
		float slewStartOut = 0.f;
		float slewTargetOut = 0.f;
		float slewInvSpan = 0.f;
		bool cycleLatched = false;
		bool eorGateState = false;
		bool eocGateState = false;
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
		float signalOutputGain = 1.f;
	};

	struct ChannelConfig {
		// Wiring map keeps the DSP path decoupled from panel/control layout.
		int cycleParam;
		int trigInput;
		int signalInput;
		int haltInput;
		int riseParam;
		int fallParam;
		int shapeParam;
		int riseCvInput;
		int fallCvInput;
		int bothCvInput;
		float logShapeTimeScaleLog2;
		float expShapeTimeScaleLog2;
		ChannelPhase gateHighPhase;
	};

	struct ChannelResult {
		bool cycleOn = false;
		bool previewStatePublished = false;
	};

	struct SlewStepResult {
		float out = 0.f;
		int direction = 0;
		bool reachedTarget = false;
		float targetFraction = 1.f;
	};

	ChannelState channel;
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
	PreviewSharedState previewState;
	PreviewUpdateState previewUpdate;
	std::atomic<bool> bandlimitedGateOutputs {false};
	std::atomic<bool> bandlimitedSignalOutputs {true};
	int timingUpdateDiv = 1;
	int timingUpdateCounter = 0;
	std::atomic<int> requestedTimingUpdateDiv {1};
	std::atomic<bool> timingInterpolate {true};
	std::atomic<bool> previewTracerEnabled {true};
	std::atomic<int> previewTracerCacheMode {WAVE_PREVIEW_TRACER_CURVE_CACHE};
	debug_terminal::BaselineModuleMetrics debugMetrics;
	// UI light updates are rate-limited to reduce engine overhead.
	float lightUpdateTimer = 0.f;
	float previewDotPublishTimer = 0.f;
	bool previewDotWasVisible = false;
	static constexpr float LINEAR_SHAPE = 0.33f;
	static constexpr float FUNCTION_V_MIN = 0.f;
	// Proc's free-running FG mode spans 0-10 V, while slew mode keeps the wider reference range.
	static constexpr float FG_V_MAX = 10.f;
	static constexpr float SLEW_REF_V_MAX = 10.2f;
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
	static constexpr float MIN_STAGE_TIME = 0.001f;
	static constexpr float LOG_SHAPE_TIME_SCALE = 6.25f;
	static constexpr float EXP_SHAPE_TIME_SCALE = 0.5f;
	// How strongly Signal IN perturbs the running FG core while cycling/triggered.
	static constexpr float SIGNAL_INJECT_GAIN = 0.55f;
	// One-pole attraction time constant for FG input perturbation.
	static constexpr float SIGNAL_INJECT_TAU = 0.0015f;
	static constexpr float DEFAULT_FUNCTION_AMP = 8.f;
	// Empirical BOTH CV response fit (hardware-calibrated saturating model).
	static constexpr float BOTH_F_OFF_HZ = 1.93157058f;
	static constexpr float BOTH_F_MAX_HZ = 986.84629918f;
	static constexpr float BOTH_K_OCT_PER_V = 1.10815030f;
	static constexpr float BOTH_V0_V = 4.15514297f;
	static constexpr float BOTH_NEUTRAL_V = -0.05f;
	static constexpr float BOTH_TIME_SCALE_MAX = 64.f;
	// Hardware-like FG ceilings.
	static constexpr float MAX_CYCLE_HZ = 1000.f;
	static constexpr float MAX_TRIGGER_HZ = 2000.f;
	static constexpr float CV_OCT_CLAMP = 12.f;
	static constexpr float STAGE_CV_OCT_PER_V = 0.5f;
	static constexpr float PREVIEW_INTERACTIVE_INTERVAL = 1.f / 60.f;
	static constexpr float PREVIEW_CV_INTERVAL = 1.f / 60.f;
	static constexpr float PREVIEW_INTERACTIVE_HOLD = 0.25f;
	static constexpr float PREVIEW_DOT_PUBLISH_INTERVAL = 1.f / 120.f;
	static constexpr int KNOB_CURVE_LUT_SIZE = 4096;
	std::array<float, KNOB_CURVE_LUT_SIZE> knobCurveLut {};

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

	static void enforceSpeedLimit(float& riseTime, float& fallTime, float minPeriod) {
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
		ChannelState& ch,
		float in,
		float riseTime,
		float fallTime,
		float shapeSigned,
		float warpScale,
		float dt
	) {
		// Shared "core limiter" path when the channel is acting as a slew on input signal.
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
		float range = SLEW_REF_V_MAX - FUNCTION_V_MIN;
		float x = computeSegPhase(out, ch.slewStartOut, ch.slewInvSpan);
		float dp = clamp(dt / stageTime, 0.f, 0.5f);
		float step = dp * slopeWarp(x, shapeSigned) * warpScale * range;

		float prevOut = out;
		float nextOut = out + ((delta > 0.f) ? step : -step);
		if ((in - prevOut) * (in - nextOut) < 0.f) {
			float denom = nextOut - prevOut;
			if (std::fabs(denom) > 1e-9f) {
				result.targetFraction = clamp((in - prevOut) / denom, 1e-6f, 1.f);
			}
			result.reachedTarget = true;
			out = in;
			ch.slewDir = 0;
		}
		else {
			out = nextOut;
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

	static void remapPhasePosForStageTimeChange(ChannelState& ch, float oldRise, float oldFall, float newRise, float newFall) {
		if (ch.phase == CHANNEL_IDLE) {
			return;
		}
		oldRise = std::max(oldRise, 1e-6f);
		oldFall = std::max(oldFall, 1e-6f);
		newRise = std::max(newRise, 1e-6f);
		newFall = std::max(newFall, 1e-6f);
		const float oldTotal = oldRise + oldFall;
		const float newTotal = newRise + newFall;
		if (ch.phase == CHANNEL_RISE) {
			const float dotX = clamp((ch.phasePos * oldRise) / oldTotal, 0.f, 1.f);
			ch.phasePos = clamp((dotX * newTotal) / newRise, 0.f, 2.f);
		}
		else if (ch.phase == CHANNEL_FALL) {
			const float dotX = clamp((oldRise + ch.phasePos * oldFall) / oldTotal, 0.f, 1.f);
			ch.phasePos = clamp(((dotX * newTotal) - newRise) / newFall, 0.f, 2.f);
		}
	}

	static void insertGateTransition(dsp::MinBlepGenerator<16, 16>& blep, bool& state, bool newState, float fraction01) {
		if (newState == state) {
			return;
		}
		float f = clamp(fraction01, 1e-6f, 1.f);
		// Rack MinBLEP expects discontinuity position in [-1, 0] samples from current sample.
		float p = f - 1.f;
		float step = newState ? 10.f : -10.f;
		blep.insertDiscontinuity(p, step);
		state = newState;
	}

	static void setGateStateImmediate(bool& state, bool newState) {
		state = newState;
	}

	static void insertSignalTransition(ChannelState& ch, float step, float fraction01) {
		if (std::fabs(step) < 1e-9f) {
			return;
		}
		float f = clamp(fraction01, 1e-6f, 1.f);
		float p = f - 1.f;
		ch.signalBlep.insertDiscontinuity(p, step * ch.signalOutputGain);
	}

	void applyTimingUpdateDiv(int div) {
		// Changing update rate invalidates cached timing so the channel resyncs immediately.
		timingUpdateDiv = std::max(1, div);
		timingUpdateCounter = 0;
		channel.stageTimeValid = false;
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

	void updateActiveStageTimes(ChannelState& ch) {
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

	void getPreviewState(float& riseTime, float& fallTime, float& curveSigned, float& dotXNorm, float& dotYNorm,
		bool& dotVisible, bool& interactiveRecent, uint32_t& version) const {
		const PreviewSharedState& shared = previewState;
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
		// Timing calibration inherited from the original Flux channel behavior:
		// - min dials at curve minimum ~80 Hz
		// - min dials at curve maximum ~1.0 kHz
		const float minTime = MIN_STAGE_TIME;
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

	void triggerFunction(ChannelState& ch) {
		// Trigger always starts a fresh rise phase.
		ch.phase = CHANNEL_RISE;
		ch.phasePos = 0.f;
	}

	ChannelResult processChannel(
		const ProcessArgs& args,
		ChannelState& ch,
		const ChannelConfig& cfg,
		PreviewSharedState& previewShared,
		PreviewUpdateState& previewUpdateState,
		bool timingTick,
		bool bandlimitedSignal,
		bool bandlimitedGate,
		bool timingInterpEnabled,
		float injectAlphaBase
	) {
		auto updateGateOutputs = [&](bool eorHigh, bool eocHigh, float fraction01) {
			if (bandlimitedGate) {
				insertGateTransition(ch.eorGateBlep, ch.eorGateState, eorHigh, fraction01);
				insertGateTransition(ch.eocGateBlep, ch.eocGateState, eocHigh, fraction01);
			}
			else {
				setGateStateImmediate(ch.eorGateState, eorHigh);
				setGateStateImmediate(ch.eocGateState, eocHigh);
			}
		};

		// This routine handles both behaviors of Proc's single channel:
		// 1) function generator when cycling/triggered
		// 2) slew limiter when a signal is patched and phase is idle
		float dt = args.sampleTime;
		ch.trigRearmSec = std::max(0.f, ch.trigRearmSec - dt);

		if (ch.cycleButtonEdge.process(params[cfg.cycleParam].getValue())) {
			ch.cycleLatched = !ch.cycleLatched;
		}

		bool haltHigh = inputs[cfg.haltInput].getVoltage() >= 2.5f;
		bool cycleOn = ch.cycleLatched;

		bool trigRise = ch.trigEdge.process(inputs[cfg.trigInput].getVoltage());
		bool trigAccepted = false;
		bool retriggerFromFall = false;
		if (!haltHigh && trigRise && ch.trigRearmSec <= 0.f && ch.phase != CHANNEL_RISE) {
			retriggerFromFall = (ch.phase == CHANNEL_FALL);
			triggerFunction(ch);
			if (retriggerFromFall) {
				// Manual behavior: trigger can reset only during FALL, restarting from cycle start.
				float prevOut = ch.out;
				ch.out = FUNCTION_V_MIN;
				if (bandlimitedSignal) {
					insertSignalTransition(ch, ch.out - prevOut, 1e-6f);
				}
			}
			trigAccepted = true;
			ch.trigRearmSec = 1.f / std::max(MAX_TRIGGER_HZ, 1.f);
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
				else if (timingInterpEnabled && timingUpdateDiv > 1) {
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
		if (ch.phase != CHANNEL_IDLE
			&& (std::fabs(riseTime - prevRiseTime) > 1e-6f || std::fabs(fallTime - prevFallTime) > 1e-6f)) {
			remapPhasePosForStageTimeChange(ch, prevRiseTime, prevFallTime, riseTime, fallTime);
		}
		bool fgActive = (ch.phase != CHANNEL_IDLE);
		if (trigAccepted) {
			// External trigger may run faster than self-cycle, but with an explicit ceiling.
			enforceSpeedLimit(riseTime, fallTime, 1.f / std::max(MAX_TRIGGER_HZ, 1.f));
		}
		else if (cycleOn) {
			// Self-cycle path is held to the lower hardware-like ceiling.
			enforceSpeedLimit(riseTime, fallTime, 1.f / std::max(MAX_CYCLE_HZ, 1.f));
		}
		else if (fgActive) {
			// One-shot/triggered FG segments use trigger-domain ceiling when not cycling.
			enforceSpeedLimit(riseTime, fallTime, 1.f / std::max(MAX_TRIGGER_HZ, 1.f));
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
		if (shapeKnobChanged && ch.phase != CHANNEL_IDLE) {
			// Re-anchor phase to current output whenever curve changes so the tracer
			// location is invalidated/recomputed against the updated curve shape.
			float range = std::max(FG_V_MAX - FUNCTION_V_MIN, 1e-6f);
			float x = clamp((ch.out - FUNCTION_V_MIN) / range, 0.f, 1.f);
			ch.phasePos = segmentPhaseFromOutputNorm(x, shapeSigned, ch.phase == CHANNEL_RISE);
		}
		float scale = ch.cachedWarpScale;

		float functionAmp = params[AMP_PARAM].getValue();
		float functionAmpScale = functionAmp / FG_V_MAX;
		bool signalPatched = inputs[cfg.signalInput].isConnected();
		float signalIn = signalPatched ? inputs[cfg.signalInput].getVoltage() : 0.f;
		if (!haltHigh && ch.phase == CHANNEL_IDLE && cycleOn) {
			// Cycle retriggers as soon as the channel reaches idle.
			triggerFunction(ch);
		}
		ch.signalOutputGain = (ch.phase != CHANNEL_IDLE) ? functionAmpScale : 1.f;
		if (haltHigh) {
			ChannelResult result;
			result.cycleOn = cycleOn;
			result.previewStatePublished = previewStatePublished;
			return result;
		}

		if (ch.phase != CHANNEL_IDLE) {
			bool eorGateIsHigh = (ch.phase == cfg.gateHighPhase);
			bool eocGateIsHigh = (ch.phase == CHANNEL_RISE);
			updateGateOutputs(eorGateIsHigh, eocGateIsHigh, 1e-6f);
		}
		else if (!signalPatched) {
			updateGateOutputs(false, false, 1e-6f);
		}

		if (ch.phase != CHANNEL_IDLE) {
			// Function-generator integration path.
			ch.signalOutputGain = functionAmpScale;
			float s = shapeSigned;
			float range = FG_V_MAX - FUNCTION_V_MIN;
			float xIn = 0.f;
			float injectAlpha = 0.f;
			if (signalPatched) {
				// During active cycling, shape toward the patched signal's
				// fixed FG-domain amplitude so AMP does not alter Signal IN influence.
				float shapedTarget = clamp(signalIn, FUNCTION_V_MIN, FG_V_MAX);
				float targetNorm = (FG_V_MAX > FUNCTION_V_MIN)
					? clamp((shapedTarget - FUNCTION_V_MIN) / (FG_V_MAX - FUNCTION_V_MIN), 0.f, 1.f)
					: 0.f;
				xIn = targetNorm;
				injectAlpha = injectAlphaBase;
			}

			if (ch.phase == CHANNEL_RISE) {
				float dpPhase = dt / riseTime;
				ch.phasePos += dpPhase;
				float x = clamp((ch.out - FUNCTION_V_MIN) / range, 0.f, 1.f);
				float dp = clamp(dt / riseTime, 0.f, 0.5f);
				x += dp * slopeWarp(x, s) * scale;
				if (injectAlpha > 0.f) {
					// Hardware-like perturbation: gently pull active FG state toward input.
					x += injectAlpha * (xIn - x);
				}
				x = clamp(x, 0.f, 1.f);
				ch.out = FUNCTION_V_MIN + x * range;
				if (ch.phasePos >= 1.f || x >= 1.f) {
					// Preserve fractional overshoot so rise->fall transition remains sample-rate robust.
					float f = phaseCrossingFraction(ch.phasePos, dpPhase);
					float overshoot = std::max(ch.phasePos - 1.f, 0.f);
					ch.phasePos = overshoot * (riseTime / std::max(fallTime, 1e-6f));
					ch.phase = CHANNEL_FALL;
					// Keep output continuous at rise->fall boundary (no hard snap to max).
					updateGateOutputs(ch.phase == cfg.gateHighPhase, ch.phase == CHANNEL_RISE, f);
				}
			}

			if (ch.phase == CHANNEL_FALL) {
				float dpPhase = dt / fallTime;
				ch.phasePos += dpPhase;
				float x = clamp((ch.out - FUNCTION_V_MIN) / range, 0.f, 1.f);
				float dp = clamp(dt / fallTime, 0.f, 0.5f);
				x -= dp * slopeWarp(x, s) * scale;
				if (injectAlpha > 0.f) {
					x += injectAlpha * (xIn - x);
				}
				x = clamp(x, 0.f, 1.f);
				ch.out = FUNCTION_V_MIN + x * range;
				if (ch.phasePos >= 1.f || x <= 0.f) {
					float f = phaseCrossingFraction(ch.phasePos, dpPhase);
					ch.phasePos = 0.f;
					ch.phase = CHANNEL_IDLE;
					float prevOut = ch.out;
					ch.out = FUNCTION_V_MIN;
					if (bandlimitedSignal) {
						insertSignalTransition(ch, ch.out - prevOut, f);
					}
					updateGateOutputs(ch.phase == cfg.gateHighPhase, ch.phase == CHANNEL_RISE, f);
				}
			}
		}
		else if (signalPatched) {
			// Use the same curve-warp family as the function generator path.
			ch.signalOutputGain = 1.f;
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
			bool eorGateIsHigh = slewStep.direction < 0;
			bool eocGateIsHigh = slewStep.direction > 0;
			updateGateOutputs(eorGateIsHigh, eocGateIsHigh, 1e-6f);
		}
		else {
			ch.signalOutputGain = 1.f;
			ch.slewDir = 0;
			ch.out = 0.f;
		}

		ChannelResult result;
		result.cycleOn = cycleOn;
		result.previewStatePublished = previewStatePublished;
		return result;
	}

	Proc() {
		debugMetrics.assignInstanceId(gProcDebugInstanceCounter);
		initKnobCurveLut();
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(CYCLE_PARAM, 0.f, 1.f, 0.f, "Cycle");
		configParam<ProcPercentQuantity>(RISE_PARAM, 0.f, 1.f, 0.f, "Surge", "%");
		configParam<ProcPercentQuantity>(FALL_PARAM, 0.f, 1.f, 0.f, "Sink", "%");
		configParam<ProcPercentQuantity>(SHAPE_PARAM, 0.f, 1.f, 0.f, "Curve", "%");
		configParam(AMP_PARAM, 0.f, 10.f, DEFAULT_FUNCTION_AMP, "Function amplitude", " V");
		configInput(SIGNAL_INPUT, "Signal");
		configInput(TRIGGER_INPUT, "Trigger");
		configInput(HALT_INPUT, "Halt CV");
		configInput(RISE_CV_INPUT, "Rise CV");
		configInput(BOTH_CV_INPUT, "Both CV");
		configInput(FALL_CV_INPUT, "Fall CV");
		configOutput(EOR_OUTPUT, "End of rise");
		configOutput(EOC_OUTPUT, "End of cycle");
		configOutput(MAIN_OUTPUT, "Positive");
		configOutput(NEG_OUTPUT, "Negative");
	}

	~Proc() override {
		teardownTimer.begin(id);
	}

	json_t* dataToJson() override {
		json_t* rootJ = json_object();
		json_object_set_new(rootJ, "cycleLatched", json_boolean(channel.cycleLatched));
		json_object_set_new(rootJ, "bandlimitedGateOutputs", json_boolean(bandlimitedGateOutputs.load(std::memory_order_relaxed)));
		json_object_set_new(rootJ, "bandlimitedSignalOutputs", json_boolean(bandlimitedSignalOutputs.load(std::memory_order_relaxed)));
		json_object_set_new(rootJ, "timingUpdateDiv", json_integer(requestedTimingUpdateDiv.load(std::memory_order_relaxed)));
		json_object_set_new(rootJ, "timingInterpolate", json_boolean(timingInterpolate.load(std::memory_order_relaxed)));
		json_object_set_new(rootJ, "previewTracerEnabled", json_boolean(previewTracerEnabled.load(std::memory_order_relaxed)));
		json_object_set_new(rootJ, "previewTracerCacheMode", json_integer(previewTracerCacheMode.load(std::memory_order_relaxed)));
		return rootJ;
	}

	void dataFromJson(json_t* rootJ) override {
		json_t* cycleJ = json_object_get(rootJ, "cycleLatched");
		if (!cycleJ) {
			// Backward-compatibility for early Proc patches saved before the naming cleanup.
			cycleJ = json_object_get(rootJ, "ch1CycleLatched");
		}
		if (cycleJ) {
			channel.cycleLatched = json_boolean_value(cycleJ);
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
		if (!isDragonKingPreviewWidgetOptionsEnabled()) {
			previewTracerCacheMode.store(WAVE_PREVIEW_TRACER_CURVE_CACHE, std::memory_order_relaxed);
		}
	}

	void process(const ProcessArgs& args) override {
		const bool measurePerf = isDragonKingDebugEnabled();
		const auto processStart = debug_terminal::debugTimerStart(measurePerf);
		applyRequestedTimingUpdateDiv();
		static const ChannelConfig channelConfig {
			CYCLE_PARAM,
			TRIGGER_INPUT,
			SIGNAL_INPUT,
			HALT_INPUT,
			RISE_PARAM,
			FALL_PARAM,
			SHAPE_PARAM,
			RISE_CV_INPUT,
			FALL_CV_INPUT,
			BOTH_CV_INPUT,
			std::log2(LOG_SHAPE_TIME_SCALE),
			std::log2(EXP_SHAPE_TIME_SCALE),
			CHANNEL_FALL
		};

		bool timingTick = true;
		const bool bandlimitedSignal = bandlimitedSignalOutputs.load(std::memory_order_relaxed);
		const bool bandlimitedGate = bandlimitedGateOutputs.load(std::memory_order_relaxed);
		const bool timingInterpEnabled = timingInterpolate.load(std::memory_order_relaxed);
		const float injectAlphaBase = SIGNAL_INJECT_GAIN *
			clamp(1.f - std::exp(-args.sampleTime / SIGNAL_INJECT_TAU), 0.f, 1.f);
		if (timingUpdateDiv > 1) {
			timingUpdateCounter++;
			if (timingUpdateCounter >= timingUpdateDiv) {
				timingUpdateCounter = 0;
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
		bool previewDotTick = false;
		if (previewDotPublishTimer >= PREVIEW_DOT_PUBLISH_INTERVAL) {
			previewDotPublishTimer -= PREVIEW_DOT_PUBLISH_INTERVAL;
			if (previewDotPublishTimer >= PREVIEW_DOT_PUBLISH_INTERVAL) {
				previewDotPublishTimer = 0.f;
			}
			previewDotTick = true;
		}

		ChannelResult channelResult = processChannel(
			args,
			channel,
			channelConfig,
			previewState,
			previewUpdate,
			timingTick,
			bandlimitedSignal,
			bandlimitedGate,
			timingInterpEnabled,
			injectAlphaBase);
		float outRendered = channel.out * channel.signalOutputGain
			+ (bandlimitedSignal ? channel.signalBlep.process() : 0.f);
		auto computeDotX = [](const ChannelState& ch) {
			if (ch.phase == CHANNEL_IDLE) {
				return 0.f;
			}
			float rise = std::max(ch.activeRiseTime, 1e-6f);
			float fall = std::max(ch.activeFallTime, 1e-6f);
			float total = rise + fall;
			if (ch.phase == CHANNEL_RISE) {
				return clamp((ch.phasePos * rise) / total, 0.f, 1.f);
			}
			if (ch.phase == CHANNEL_FALL) {
				return clamp((rise + ch.phasePos * fall) / total, 0.f, 1.f);
			}
			return 0.f;
		};
		const bool dotVisible = channel.phase != CHANNEL_IDLE;
		if (previewDotTick || channelResult.previewStatePublished || dotVisible != previewDotWasVisible) {
			float outRangeInv = 1.f / std::max(FG_V_MAX - FUNCTION_V_MIN, 1e-6f);
			publishPreviewDot(
				previewState,
				dotVisible,
				computeDotX(channel),
				(channel.out - FUNCTION_V_MIN) * outRangeInv
			);
			previewDotWasVisible = dotVisible;
		}
		float eorOut = (channel.eorGateState ? 10.f : 0.f) + (bandlimitedGate ? channel.eorGateBlep.process() : 0.f);
		float eocOut = (channel.eocGateState ? 10.f : 0.f) + (bandlimitedGate ? channel.eocGateBlep.process() : 0.f);
		float negOut = -outRendered;

		outputs[EOR_OUTPUT].setVoltage(eorOut);
		outputs[EOC_OUTPUT].setVoltage(eocOut);
		outputs[MAIN_OUTPUT].setVoltage(outRendered);
		outputs[NEG_OUTPUT].setVoltage(negOut);

		if (lightTick) {
			lights[CYCLE_LIGHT].setBrightness(channelResult.cycleOn ? 1.f : 0.f);
			lights[EOR_LIGHT].setBrightness(channel.eorGateState ? 1.f : 0.f);
			lights[EOC_LIGHT].setBrightness(channel.eocGateState ? 1.f : 0.f);
			lights[MAIN_LIGHT].setBrightness(clamp(std::fabs(outRendered) / FG_V_MAX, 0.f, 1.f));
			lights[NEG_LIGHT].setBrightness(clamp(std::fabs(negOut) / FG_V_MAX, 0.f, 1.f));
		}
		if (measurePerf) {
			debugMetrics.recordProcess(debug_terminal::elapsedNsSince(processStart));
		}
	}
};

namespace {

struct ProcPreviewEdgeInteraction {
	bool riseHovered = false;
	bool fallHovered = false;
	bool curveHovered = false;
	bool riseDragging = false;
	bool fallDragging = false;
	bool curveDragging = false;
};

struct WavePreviewWidget : Widget {
	// Small preview box: lower geometry density reduces UI cost while staying smooth.
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
		static constexpr int TRAIL_DRAW_STRIDE = 2;
		static constexpr int TRAIL_CAPTURE_STRIDE = 1;
	std::array<Vec, POINT_COUNT> points {};
	WavePreviewTracer<POINT_COUNT, TRAIL_FRAME_COUNT> curveTracer;
	WavePreviewBufferedTracer<POINT_COUNT> frameTracer;
	Proc* modulePtr = nullptr;
	ProcPreviewEdgeInteraction* edgeInteraction = nullptr;
	uint32_t lastVersion = 0;
	bool pointsValid = false;
	int peakPointIndex = POINT_COUNT / 2;
	float lastFreqHz = 100.f;
	float dotXNorm = 0.f;
	float dotYNorm = 0.f;
	bool dotVisible = false;

	explicit WavePreviewWidget(Proc* module) : modulePtr(module) {
	}

	static void buildSegmentLut(std::array<float, PREVIEW_LUT_SIZE>& lut, float curveSigned, bool rising) {
		// Build once per preview update. Midpoint integration reduces visual artifacts at extreme curve asymmetry.
		float scale = Proc::slopeWarpScale(curveSigned);
		float dp = 1.f / float(PREVIEW_LUT_SIZE - 1);
		float x = rising ? 0.f : 1.f;
		lut[0] = x;
		for (int i = 1; i < PREVIEW_LUT_SIZE; ++i) {
			float k1 = Proc::slopeWarp(x, curveSigned) * scale;
			float xMid = rising ? (x + 0.5f * dp * k1) : (x - 0.5f * dp * k1);
			xMid = clamp(xMid, 0.f, 1.f);
			float k2 = Proc::slopeWarp(xMid, curveSigned) * scale;
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

	int highlightedEdge() const {
		if (!edgeInteraction) {
			return 0;
		}
		if (edgeInteraction->curveDragging) {
			return 3;
		}
		if (edgeInteraction->riseDragging) {
			return 1;
		}
		if (edgeInteraction->fallDragging) {
			return 2;
		}
		if (edgeInteraction->curveHovered) {
			return 3;
		}
		if (edgeInteraction->riseHovered) {
			return 1;
		}
		if (edgeInteraction->fallHovered) {
			return 2;
		}
		return 0;
	}

	static NVGcolor waveformColor() {
		return nvgRGBA(230, 230, 220, 255);
	}

	NVGcolor activeEdgeColor(int edge) const {
		const NVGcolor purple = nvgRGB(0x86, 0x5c, 0xff);
		const NVGcolor cyan = nvgRGB(0x00, 0xc6, 0xe4);
		if (!modulePtr) {
			return purple;
		}

		const int paramId = edge == 1 ? Proc::RISE_PARAM : Proc::FALL_PARAM;
		const float amount = clamp(modulePtr->params[paramId].getValue(), 0.f, 1.f);
		return nvgRGBAf(
			purple.r + (cyan.r - purple.r) * amount,
			purple.g + (cyan.g - purple.g) * amount,
			purple.b + (cyan.b - purple.b) * amount,
			1.f);
	}

	NVGcolor activeCurveColor() const {
		const NVGcolor orange = nvgRGB(0xdc, 0x5e, 0x1e);
		const NVGcolor yellow = nvgRGB(0xff, 0xb8, 0x00);
		if (!modulePtr) {
			return orange;
		}

		const float amount = clamp(modulePtr->params[Proc::SHAPE_PARAM].getValue(), 0.f, 1.f);
		return nvgRGBAf(
			orange.r + (yellow.r - orange.r) * amount,
			orange.g + (yellow.g - orange.g) * amount,
			orange.b + (yellow.b - orange.b) * amount,
			1.f);
	}

	void drawWaveSegment(const DrawArgs& args, int start, int end, NVGcolor color) {
		if (!pointsValid) {
			return;
		}
		start = clamp(start, 0, POINT_COUNT - 1);
		end = clamp(end, 0, POINT_COUNT - 1);
		const int count = end - start + 1;
		if (count < 2) {
			return;
		}
		NVGcontext* vg = args.vg;
		nvgBeginPath(vg);
		wave_preview::simplifyPath(points.data() + start, count, 1, 0.02f, [vg](const Vec& pt, bool isMove) {
			if (isMove) {
				nvgMoveTo(vg, pt.x, pt.y);
			}
			else {
				nvgLineTo(vg, pt.x, pt.y);
			}
		});
		nvgStrokeColor(vg, color);
		nvgStrokeWidth(vg, WAVE_LINE_WIDTH);
		nvgLineCap(vg, NVG_BUTT);
		nvgLineJoin(vg, NVG_ROUND);
		nvgStroke(vg);
	}

	void drawWaveform(const DrawArgs& args) {
		const int edge = highlightedEdge();
		if (edge == 0) {
			drawWaveSegment(args, 0, POINT_COUNT - 1, waveformColor());
			return;
		}
		if (edge == 3) {
			drawWaveSegment(args, 0, POINT_COUNT - 1, activeCurveColor());
			return;
		}
		const int peakIndex = clamp(peakPointIndex, 1, POINT_COUNT - 2);
		const NVGcolor highlightColor = activeEdgeColor(edge);
		drawWaveSegment(args, 0, peakIndex, edge == 1 ? highlightColor : waveformColor());
		drawWaveSegment(args, peakIndex, POINT_COUNT - 1, edge == 2 ? highlightColor : waveformColor());
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
		std::array<float, PREVIEW_LUT_SIZE> riseLut {};
		std::array<float, PREVIEW_LUT_SIZE> fallLut {};
		buildSegmentLut(riseLut, curveSigned, true);
		buildSegmentLut(fallLut, curveSigned, false);

		for (int i = 0; i < POINT_COUNT; ++i) {
			float xNorm = float(i) / float(POINT_COUNT - 1);
			float x = left + xNorm * drawW;
			float y = -1.f;
			if (x <= peakX) {
				float t = (x - left) / riseWidth;
				float v = sampleSegmentLut(riseLut, t);
				y = -1.f + 2.f * v;
			}
			else {
				float t = (x - peakX) / fallWidth;
				float v = sampleSegmentLut(fallLut, t);
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
			peakPointIndex = peakIndex;
			points[peakIndex] = Vec(peakX, top);
			points.front() = Vec(left, bottom);
			points.back() = Vec(right, bottom);
		pointsValid = true;
	}

	void step() override {
		Widget::step();
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
		modulePtr->getPreviewState(riseTime, fallTime, curveSigned, previewDotXNorm, previewDotYNorm, previewDotVisible,
			interactiveRecent, version);
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
		} else if (lastFreqHz <= DOT_SHOW_MAX_HZ) {
			dotVisible = true;
		}
		const double nowSec = system::getTime();
		const bool tracerEnabled = modulePtr->previewTracerEnabled.load(std::memory_order_relaxed);
		const int tracerMode = modulePtr->previewTracerCacheMode.load(std::memory_order_relaxed);
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
					curveTracer.capture(points, nowSec, TRAIL_MIN_CAPTURE_INTERVAL_SEC, TRAIL_CAPTURE_STRIDE);
				}
				else {
					WavePreviewBufferedTracerStyle style;
					style.color = nvgRGBA(255, 190, 80, 255);
					style.fadeSec = TRAIL_FADE_SEC;
					style.minCaptureIntervalSec = TRAIL_MIN_CAPTURE_INTERVAL_SEC;
					style.maxAlpha = 118.f;
					style.drawStride = TRAIL_CAPTURE_STRIDE;
					frameTracer.capture(points, nowSec, box.size, style);
				}
			}
			rebuildPoints(riseTime, fallTime, curveSigned, interactiveRecent);
			lastVersion = version;
		}
	}

	void draw(const DrawArgs& args) override {
		nvgSave(args.vg);
		nvgScissor(args.vg, 0.f, 0.f, box.size.x, box.size.y);
		if (pointsValid) {
			ModuleWidget* moduleWidget = getAncestorOfType<ModuleWidget>();
			Proc* modulePtr = moduleWidget ? moduleWidget->getModule<Proc>() : nullptr;
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
			drawWaveform(args);
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
		// Keep label outside preview box to avoid occluding waveform.
		nvgText(args.vg, box.size.x * 0.5f, box.size.y + 1.5f, freqText, nullptr);
	}
};

static math::Rect insetRectMm(math::Rect rect, float insetMm) {
	rect.pos.x += insetMm;
	rect.pos.y += insetMm;
	rect.size.x = std::max(0.f, rect.size.x - 2.f * insetMm);
	rect.size.y = std::max(0.f, rect.size.y - 2.f * insetMm);
	return rect;
}

struct AmpVoltageReadoutWidget : Widget {
	Proc* module = nullptr;
	int paramId = -1;

	void draw(const DrawArgs& args) override {
		Widget::draw(args);
		if (paramId < 0 || !APP || !APP->window || !APP->window->uiFont) {
			return;
		}
		float ampVolts = Proc::DEFAULT_FUNCTION_AMP;
		if (module && paramId < Proc::PARAMS_LEN) {
			ampVolts = module->params[paramId].getValue();
		}
		else if (paramId != Proc::AMP_PARAM) {
			return;
		}
		char ampText[16];
		std::snprintf(ampText, sizeof(ampText), "%.1fV", ampVolts);
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgFontSize(args.vg, 10.0f);
		nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 255));
		nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_TOP);
		nvgText(args.vg, box.size.x * 0.5f, 0.75f, ampText, nullptr);
	}
};

struct ProcCurveHalo2Knob : LeviathanHaloKnob2 {
	ProcPreviewEdgeInteraction* previewInteraction = nullptr;

	ProcCurveHalo2Knob() : LeviathanHaloKnob2(LeviathanHaloKnob2::brightOrangeConfig()) {
	}

	void setPreviewInteraction(ProcPreviewEdgeInteraction* interaction) {
		previewInteraction = interaction;
	}

	void onEnter(const event::Enter& e) override {
		if (previewInteraction) {
			previewInteraction->curveHovered = true;
		}
		LeviathanHaloKnob2::onEnter(e);
	}

	void onLeave(const event::Leave& e) override {
		if (previewInteraction) {
			previewInteraction->curveHovered = false;
		}
		LeviathanHaloKnob2::onLeave(e);
	}

	void onDragStart(const event::DragStart& e) override {
		if (previewInteraction) {
			previewInteraction->curveDragging = true;
		}
		LeviathanHaloKnob2::onDragStart(e);
	}

	void onDragEnd(const event::DragEnd& e) override {
		if (previewInteraction) {
			previewInteraction->curveDragging = false;
		}
		LeviathanHaloKnob2::onDragEnd(e);
	}
};

struct ProcLinearPointOverlay : TransparentWidget {
	Vec centerPx;

	static constexpr float LINEAR_SHAPE_VALUE = Proc::LINEAR_SHAPE;
	static constexpr float SHARK_FIN_LINEAR_SHAPE_VALUE = 0.5f;
	static constexpr float BASE_ANGLE_DEG = 120.f;
	static constexpr float SWEEP_ANGLE_DEG = 300.f;
	static constexpr float LINE_RADIUS_MM = 8.6f;
	static constexpr float LABEL_RADIUS_MM = 10.9f;
	static constexpr float LABEL_TANGENT_OFFSET_MM = 0.85f;
	static constexpr float LABEL_Y_OFFSET_MM = 0.32f;
	static constexpr float LABEL_TOP_Y_OFFSET_MM = 0.18f;
	static constexpr float LABEL_MIRROR_X_OFFSET_MM = 0.38f;
	static constexpr float LABEL_MIRROR_Y_OFFSET_MM = -0.28f;
	static constexpr float LINE_WIDTH_MM = 0.5f;
	static constexpr float TRIANGLE_GLYPH_WIDTH_MM = 4.1f;
	static constexpr float TRIANGLE_GLYPH_HEIGHT_MM = 2.35f;
	static constexpr float TRIANGLE_GLYPH_LINE_WIDTH_MM = 0.42f;

	explicit ProcLinearPointOverlay(Vec centerPx)
		: centerPx(centerPx) {
		box.pos = Vec(0.f, 0.f);
	}

	void draw(const DrawArgs& args) override {
		const float angle = (BASE_ANGLE_DEG + SWEEP_ANGLE_DEG * LINEAR_SHAPE_VALUE) * (float(M_PI) / 180.f);
		const Vec dir(std::cos(angle), std::sin(angle));
		const Vec tangent(-dir.y, dir.x);
		const float lineRadius = mm2px(Vec(LINE_RADIUS_MM, 0.f)).x;
		const float labelRadius = mm2px(Vec(LABEL_RADIUS_MM, 0.f)).x;
		const float tangentOffset = mm2px(Vec(LABEL_TANGENT_OFFSET_MM, 0.f)).x
			* clamp(std::fabs(LINEAR_SHAPE_VALUE - SHARK_FIN_LINEAR_SHAPE_VALUE)
				/ std::max(SHARK_FIN_LINEAR_SHAPE_VALUE - LINEAR_SHAPE_VALUE, 1e-4f), 0.f, 1.f);
		const float topBlend = clamp((LINEAR_SHAPE_VALUE - LINEAR_SHAPE_VALUE)
			/ std::max(SHARK_FIN_LINEAR_SHAPE_VALUE - LINEAR_SHAPE_VALUE, 1e-4f), 0.f, 1.f);
		const float labelYOffset = mm2px(Vec(0.f, LABEL_Y_OFFSET_MM + LABEL_TOP_Y_OFFSET_MM * topBlend)).y;
		const Vec mirrorOffset = mm2px(Vec(
			LABEL_MIRROR_X_OFFSET_MM * (1.f - topBlend),
			LABEL_MIRROR_Y_OFFSET_MM * (1.f - topBlend)));
		const float lineWidth = mm2px(Vec(LINE_WIDTH_MM, 0.f)).x;
		const float triangleWidth = mm2px(Vec(TRIANGLE_GLYPH_WIDTH_MM, 0.f)).x;
		const float triangleHeight = mm2px(Vec(0.f, TRIANGLE_GLYPH_HEIGHT_MM)).y;
		const float triangleLineWidth = mm2px(Vec(TRIANGLE_GLYPH_LINE_WIDTH_MM, 0.f)).x;
		const Vec lineEnd = centerPx.plus(dir.mult(lineRadius));
		const Vec labelPos = centerPx.plus(dir.mult(labelRadius)).plus(tangent.mult(tangentOffset)).plus(Vec(0.f, labelYOffset)).plus(mirrorOffset);
		const Vec triangleLeft(labelPos.x - 0.5f * triangleWidth, labelPos.y + 0.5f * triangleHeight);
		const Vec trianglePeak(labelPos.x, labelPos.y - 0.5f * triangleHeight);
		const Vec triangleRight(labelPos.x + 0.5f * triangleWidth, labelPos.y + 0.5f * triangleHeight);

		nvgSave(args.vg);
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, centerPx.x, centerPx.y);
		nvgLineTo(args.vg, lineEnd.x, lineEnd.y);
		nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 255));
		nvgStrokeWidth(args.vg, lineWidth);
		nvgLineCap(args.vg, NVG_ROUND);
		nvgStroke(args.vg);

		nvgLineCap(args.vg, NVG_ROUND);
		nvgLineJoin(args.vg, NVG_ROUND);
		nvgStrokeWidth(args.vg, triangleLineWidth);
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, triangleLeft.x, triangleLeft.y);
		nvgLineTo(args.vg, trianglePeak.x, trianglePeak.y);
		nvgLineTo(args.vg, triangleRight.x, triangleRight.y);
		NVGpaint triangleGradient = nvgLinearGradient(
			args.vg,
			triangleLeft.x,
			triangleLeft.y,
			triangleRight.x,
			triangleRight.y,
			nvgRGBA(255, 184, 0, 255),
			nvgRGBA(220, 94, 30, 255));
		nvgStrokePaint(args.vg, triangleGradient);
		nvgStroke(args.vg);
		nvgRestore(args.vg);
	}
};

struct ProcEdgeHalo2Knob : LeviathanHaloKnob2 {
	enum PreviewEdge {
		PREVIEW_EDGE_RISE,
		PREVIEW_EDGE_FALL
	};

	ProcPreviewEdgeInteraction* previewInteraction = nullptr;
	PreviewEdge previewEdge = PREVIEW_EDGE_RISE;

	void setPreviewInteraction(ProcPreviewEdgeInteraction* interaction, PreviewEdge edge) {
		previewInteraction = interaction;
		previewEdge = edge;
	}

	void setHovered(bool hovered) {
		if (!previewInteraction) {
			return;
		}
		if (previewEdge == PREVIEW_EDGE_RISE) {
			previewInteraction->riseHovered = hovered;
		}
		else {
			previewInteraction->fallHovered = hovered;
		}
	}

	void setDragging(bool dragging) {
		if (!previewInteraction) {
			return;
		}
		if (previewEdge == PREVIEW_EDGE_RISE) {
			previewInteraction->riseDragging = dragging;
		}
		else {
			previewInteraction->fallDragging = dragging;
		}
	}

	void onEnter(const event::Enter& e) override {
		setHovered(true);
		LeviathanHaloKnob2::onEnter(e);
	}

	void onLeave(const event::Leave& e) override {
		setHovered(false);
		LeviathanHaloKnob2::onLeave(e);
	}

	void onDragStart(const event::DragStart& e) override {
		setDragging(true);
		LeviathanHaloKnob2::onDragStart(e);
	}

	void onDragEnd(const event::DragEnd& e) override {
		setDragging(false);
		LeviathanHaloKnob2::onDragEnd(e);
	}
};

struct ProcWidget : ModuleWidget {
	debug_terminal::BaselineWidgetMetrics debugWidgetMetrics;
	ProcPreviewEdgeInteraction previewEdgeInteraction;

	void step() override {
		const bool measurePerf = isDragonKingDebugEnabled();
		const auto stepStart = debug_terminal::debugTimerStart(measurePerf);
		ModuleWidget::step();
		if (measurePerf) {
			debugWidgetMetrics.recordStep(debug_terminal::elapsedUsSince(stepStart));
		}
	}

	void draw(const DrawArgs& args) override {
		const bool measurePerf = isDragonKingDebugEnabled();
		const auto drawStart = debug_terminal::debugTimerStart(measurePerf);
		ModuleWidget::draw(args);
		Proc* proc = static_cast<Proc*>(module);
		if (!proc) {
			return;
		}

		if (isDragonKingDebugEnabled()) {
			debug_terminal::drawDebugInstanceId(args.vg, box.size, proc->debugMetrics.instanceId);
		}

		if (measurePerf) {
			debugWidgetMetrics.recordDraw(debug_terminal::elapsedUsSince(drawStart));
		}

		if (measurePerf) {
			const double nowSec = system::getTime();
			if (debug_terminal::baselineSubmitDue("Proc", proc->debugMetrics.instanceId, nowSec)) {
				debug_terminal::submitBaselineMetrics(
					"Proc",
					proc->debugMetrics.instanceId,
					proc->debugMetrics.consumeProcessRange(),
					debugWidgetMetrics.consumeStepRange(),
					debugWidgetMetrics.consumeDrawRange()
				);
			}
		}
	}

	ProcWidget(Proc* module) {
		setModule(module);
		PreviewBuildLogTimer previewBuildTimer("Proc", module);
		visual_assets::SplitPanelRenderer splitPanel(this, "res/proc.panel.svg");
		const std::string& panelBasePath = splitPanel.panelPath();
		splitPanel.addLabels("res/proc.labels.svg");
		splitPanel.addCompactLeviathanLogoBranding();
		visual_assets::addFractalGlassOverlay(
			this, panelBasePath, splitPanel.panelSurfaceEffectWidget());
		if (widget::FramebufferWidget* conduits =
			visual_assets::createPlasmaConduitLayer(panelBasePath, box.size)) {
			addChild(conduits);
		}
		previewBuildTimer.markPanelDone();

		addChild(createWidget<CyanOrbScrew>(Vec(0.f, 0)));
		//addChild(createWidget<CyanOrbScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<CyanOrbScrew>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		Vec cyclePos(33.075f, 20.138f);
		Vec risePos(32.907f, 36.293f);
		Vec fallPos(32.907f, 53.079f);
		Vec shapePos(11.775f, 57.926f);
		Vec ampPos(7.246f, 28.71f);
		Vec signalInPos(7.247f, 16.654f);
		Vec trigInPos(19.943f, 16.654f);
		Vec haltInPos(7.207f, 40.367f);
		Vec riseCvInPos(19.943f, 32.416f);
		Vec bothCvInPos(19.943f, 44.898f);
		Vec fallCvInPos(23.604f, 63.263f);
		Vec eorOutPos(9.437f, 96.946f);
		Vec eocOutPos(26.595f, 96.915f);
		Vec outPos(9.447f, 110.682f);
		Vec negOutPos(26.552f, 110.882f);
		Vec cycleLightPos(33.075f, 14.055f);
		Vec eorLightPos(15.937f, 96.76f);
		Vec eocLightPos(33.645f, 96.952f);
		Vec outLightPos(15.947f, 110.758f);
		Vec negLightPos(33.579f, 110.941f);

		auto applyPointOverride = [&](const char* elementId, Vec* outPosMm) {
			Vec pointMm;
			if (panel_svg::loadPointFromSvgMm(panelBasePath, elementId, &pointMm)) {
				*outPosMm = pointMm;
			}
		};

		applyPointOverride("CYCLE_1", &cyclePos);
		applyPointOverride("RISE_1", &risePos);
		applyPointOverride("FALL_1", &fallPos);
		applyPointOverride("LIN_LOG_1", &shapePos);
		applyPointOverride("AMP", &ampPos);
		applyPointOverride("SIGNAL_INPUT", &signalInPos);
		applyPointOverride("TRIGGER_INPUT", &trigInPos);
		applyPointOverride("HALT_INPUT", &haltInPos);
		applyPointOverride("RISE_CV_INPUT", &riseCvInPos);
		applyPointOverride("BOTH_CV_INPUT", &bothCvInPos);
		applyPointOverride("FALL_CV_INPUT", &fallCvInPos);
		applyPointOverride("EOR_OUTPUT", &eorOutPos);
		applyPointOverride("EOC_OUTPUT", &eocOutPos);
		applyPointOverride("MAIN_OUTPUT", &outPos);
		applyPointOverride("NEG_OUTPUT", &negOutPos);
		applyPointOverride("CYCLE_LIGHT", &cycleLightPos);
		applyPointOverride("EOR_LIGHT", &eorLightPos);
		applyPointOverride("EOC_LIGHT", &eocLightPos);
		applyPointOverride("MAIN_LIGHT", &outLightPos);
		applyPointOverride("NEG_LIGHT", &negLightPos);

		{
			widget::FramebufferWidget* linearPointFb = new widget::FramebufferWidget();
			const Vec centerPx = mm2px(shapePos);
			linearPointFb->box.size = mm2px(Vec(27.f, 27.f));
			linearPointFb->box.pos = centerPx.minus(linearPointFb->box.size.mult(0.5f));
			linearPointFb->dirtyOnSubpixelChange = false;
			ProcLinearPointOverlay* linearPoint = new ProcLinearPointOverlay(centerPx.minus(linearPointFb->box.pos));
			linearPoint->box.size = linearPointFb->box.size;
			linearPointFb->addChild(linearPoint);
			addChild(linearPointFb);
		}

		addParam(createParamCentered<LoopGoldButton>(mm2px(cyclePos), module, Proc::CYCLE_PARAM));
		{
			ProcEdgeHalo2Knob* riseKnob = createParamCentered<ProcEdgeHalo2Knob>(mm2px(risePos), module, Proc::RISE_PARAM);
			riseKnob->setPreviewInteraction(&previewEdgeInteraction, ProcEdgeHalo2Knob::PREVIEW_EDGE_RISE);
			addParam(riseKnob);
		}
		{
			ProcEdgeHalo2Knob* fallKnob = createParamCentered<ProcEdgeHalo2Knob>(mm2px(fallPos), module, Proc::FALL_PARAM);
			fallKnob->setPreviewInteraction(&previewEdgeInteraction, ProcEdgeHalo2Knob::PREVIEW_EDGE_FALL);
			addParam(fallKnob);
		}
		{
			ProcCurveHalo2Knob* curveKnob = createParamCentered<ProcCurveHalo2Knob>(mm2px(shapePos), module, Proc::SHAPE_PARAM);
			curveKnob->setPreviewInteraction(&previewEdgeInteraction);
			addParam(curveKnob);
		}
		addParam(createParamCentered<DarkTinyClockworkGearKnob>(mm2px(ampPos), module, Proc::AMP_PARAM));
		{
			AmpVoltageReadoutWidget* ampReadout = new AmpVoltageReadoutWidget();
			ampReadout->module = module;
			ampReadout->paramId = Proc::AMP_PARAM;
			ampReadout->box.pos = mm2px(Vec(ampPos.x - 4.746f, ampPos.y + 3.44f));
			ampReadout->box.size = mm2px(Vec(9.6, 2.6));
			addChild(ampReadout);
		}
		{
			WavePreviewWidget* previewWidget = new WavePreviewWidget(module);
			previewWidget->edgeInteraction = &previewEdgeInteraction;
			math::Rect previewRectMm;
			if (panel_svg::loadRectFromSvgMm(panelBasePath, "CH1_PREVIEW", &previewRectMm)) {
				// Keep the legacy SVG id until proc.svg is cleaned up as well.
				addChild(visual_assets::createPreviewFrameEnhancementWidget(previewRectMm));
				previewRectMm = insetRectMm(previewRectMm, 0.2f);
				previewWidget->box.pos = mm2px(previewRectMm.pos);
				previewWidget->box.size = mm2px(previewRectMm.size);
			}
			else {
				math::Rect previewFallbackMm(Vec(3.75998355f, 68.96602539f), Vec(20.78393382f, 11.24561948f));
				addChild(visual_assets::createPreviewFrameEnhancementWidget(previewFallbackMm));
				previewFallbackMm = insetRectMm(previewFallbackMm, 0.2f);
				previewWidget->box.pos = mm2px(previewFallbackMm.pos);
				previewWidget->box.size = mm2px(previewFallbackMm.size);
			}
			widget::FramebufferWidget* previewSurface = preview_surface::createCachedOpaqueGrid(previewWidget->box.size);
			previewSurface->box.pos = previewWidget->box.pos;
			addChild(previewSurface);
			addChild(previewWidget);
		}
		previewBuildTimer.setAtlasStatus(panel_svg::getAtlasStatusLabelForSvg(panelBasePath));
		previewBuildTimer.markAnchorsDone();

		addInput(createInputCentered<Magitek2InputJack>(mm2px(signalInPos), module, Proc::SIGNAL_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(trigInPos), module, Proc::TRIGGER_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(haltInPos), module, Proc::HALT_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(riseCvInPos), module, Proc::RISE_CV_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(bothCvInPos), module, Proc::BOTH_CV_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(fallCvInPos), module, Proc::FALL_CV_INPUT));

		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(eorOutPos), module, Proc::EOR_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(eocOutPos), module, Proc::EOC_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(outPos), module, Proc::MAIN_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(negOutPos), module, Proc::NEG_OUTPUT));

		addChild(createLightCentered<SmallAperture<AmberApertureLight>>(mm2px(cycleLightPos), module, Proc::CYCLE_LIGHT));

		addChild(createLightCentered<SmallAperture<GreenApertureLight>>(mm2px(eorLightPos), module, Proc::EOR_LIGHT));
		addChild(createLightCentered<SmallAperture<MagentaApertureLight>>(mm2px(eocLightPos), module, Proc::EOC_LIGHT));
		addChild(createLightCentered<SmallAperture<GreenApertureLight>>(mm2px(outLightPos), module, Proc::MAIN_LIGHT));
		addChild(createLightCentered<SmallAperture<MagentaApertureLight>>(mm2px(negLightPos), module, Proc::NEG_LIGHT));
	}

	void appendContextMenu(Menu* menu) override {
		Proc* proc = dynamic_cast<Proc*>(module);
		assert(menu);

		menu->addChild(new MenuSeparator());
		if (proc) {
			menu->addChild(createMenuLabel("Performance"));
			menu->addChild(createCheckMenuItem("Bandlimited EOR/EOC", "",
				[=]() { return proc->bandlimitedGateOutputs.load(std::memory_order_relaxed); },
				[=]() { proc->bandlimitedGateOutputs.store(!proc->bandlimitedGateOutputs.load(std::memory_order_relaxed), std::memory_order_relaxed); }
			));
			menu->addChild(createCheckMenuItem("Bandlimited Signal Outputs", "",
				[=]() { return proc->bandlimitedSignalOutputs.load(std::memory_order_relaxed); },
				[=]() { proc->bandlimitedSignalOutputs.store(!proc->bandlimitedSignalOutputs.load(std::memory_order_relaxed), std::memory_order_relaxed); }
			));
			menu->addChild(createMenuLabel("Preview Visual"));
			menu->addChild(createCheckMenuItem("Preview Tracer", "",
				[=]() { return proc->previewTracerEnabled.load(std::memory_order_relaxed); },
				[=]() { proc->previewTracerEnabled.store(!proc->previewTracerEnabled.load(std::memory_order_relaxed), std::memory_order_relaxed); }
			));
			if (isDragonKingPreviewWidgetOptionsEnabled()) {
				menu->addChild(createSubmenuItem("Tracer Quality", "",
					[=](Menu* submenu) {
						submenu->addChild(createCheckMenuItem("Curve cache", "",
							[=]() { return proc->previewTracerCacheMode.load(std::memory_order_relaxed) == WAVE_PREVIEW_TRACER_CURVE_CACHE; },
							[=]() { proc->previewTracerCacheMode.store(WAVE_PREVIEW_TRACER_CURVE_CACHE, std::memory_order_relaxed); }
						));
						submenu->addChild(createCheckMenuItem("Frame cache", "",
							[=]() { return proc->previewTracerCacheMode.load(std::memory_order_relaxed) == WAVE_PREVIEW_TRACER_FRAME_CACHE; },
							[=]() { proc->previewTracerCacheMode.store(WAVE_PREVIEW_TRACER_FRAME_CACHE, std::memory_order_relaxed); }
						));
					}
				));
			}
			menu->addChild(createMenuLabel("Rate Control"));
			menu->addChild(createCheckMenuItem("Interpolate Timing Updates", "",
				[=]() { return proc->timingInterpolate.load(std::memory_order_relaxed); },
				[=]() { proc->timingInterpolate.store(!proc->timingInterpolate.load(std::memory_order_relaxed), std::memory_order_relaxed); }
			));
			menu->addChild(createSubmenuItem("Timing Update Rate", "",
				[=](Menu* submenu) {
					auto addDivItem = [=](int div, std::string label) {
						submenu->addChild(createCheckMenuItem(label, "",
							[=]() { return proc->requestedTimingUpdateDiv.load(std::memory_order_relaxed) == div; },
							[=]() { proc->requestTimingUpdateDiv(div); }
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

Model* modelProc = createModel<Proc, ProcWidget>("Proc");
