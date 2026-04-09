#include "plugin.hpp"
#include "PanelSvgUtils.hpp"
#include <dsp/minblep.hpp>
#include <array>
#include <cstdio>
#include <atomic>


struct Proc : Module {
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
	bool bandlimitedGateOutputs = false;
	bool bandlimitedSignalOutputs = true;
	int timingUpdateDiv = 1;
	int timingUpdateCounter = 0;
	bool timingInterpolate = true;
	// UI light updates are rate-limited to reduce engine overhead.
	float lightUpdateTimer = 0.f;
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
	static constexpr int KNOB_CURVE_LUT_SIZE = 4096;
	std::array<float, KNOB_CURVE_LUT_SIZE> knobCurveLut {};

	static float softClamp8(float v) {
		// Smoothly approaches +/-8V while staying linear near zero.
		return 8.0f * std::tanh(v / 8.0f);
	}

	static float bothHzFromCv(float v) {
		float x = BOTH_K_OCT_PER_V * (v - BOTH_V0_V);
		float r = rack::dsp::exp2_taylor5(x);
		return BOTH_F_OFF_HZ + BOTH_F_MAX_HZ * (r / (1.f + r));
	}

	static float bothTimeScaleFromCv(float v) {
		float vs = softClamp8(v);
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

	void setTimingUpdateDiv(int div) {
		// Changing update rate invalidates cached timing so the channel resyncs immediately.
		timingUpdateDiv = std::max(1, div);
		timingUpdateCounter = 0;
		channel.stageTimeValid = false;
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

	void updatePreviewChannel(
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
		}
		if (state.interactiveHold > 0.f) {
			state.interactiveHold = std::max(0.f, state.interactiveHold - dt);
		}
		state.timer += dt;

		float interval = (state.interactiveHold > 0.f) ? PREVIEW_INTERACTIVE_INTERVAL : PREVIEW_CV_INTERVAL;
		bool changed = !state.sentOnce || previewChangedMeaningfully(
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
		}
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
		float stageCvSoft = softClamp8(stageCv);
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
		bool timingTick
	) {
		auto updateGateOutputs = [&](bool eorHigh, bool eocHigh, float fraction01) {
			if (bandlimitedGateOutputs) {
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
		if (!haltHigh && trigRise && ch.trigRearmSec <= 0.f && ch.phase != CHANNEL_RISE) {
			triggerFunction(ch);
			trigAccepted = true;
			ch.trigRearmSec = 1.f / std::max(MAX_TRIGGER_HZ, 1.f);
		}

		float riseKnob = params[cfg.riseParam].getValue();
		float fallKnob = params[cfg.fallParam].getValue();
		float shape = params[cfg.shapeParam].getValue();
		float riseCv = inputs[cfg.riseCvInput].getVoltage();
		float fallCv = inputs[cfg.fallCvInput].getVoltage();
		float bothCv = inputs[cfg.bothCvInput].getVoltage();
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
				else if (timingInterpolate && timingUpdateDiv > 1) {
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
		updateActiveStageTimes(ch);
		float riseTime = ch.activeRiseTime;
		float fallTime = ch.activeFallTime;
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
		updatePreviewChannel(
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
				float a = 1.f - std::exp(-dt / SIGNAL_INJECT_TAU);
				injectAlpha = SIGNAL_INJECT_GAIN * clamp(a, 0.f, 1.f);
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
					float prevOut = ch.out;
					ch.out = FG_V_MAX;
					if (bandlimitedSignalOutputs) {
						insertSignalTransition(ch, ch.out - prevOut, f);
					}
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
					if (bandlimitedSignalOutputs) {
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

		bool fgDotVisible = (ch.phase != CHANNEL_IDLE);
		float dotXNorm = 0.f;
		if (fgDotVisible) {
			float total = std::max(riseTime + fallTime, 1e-6f);
			if (ch.phase == CHANNEL_RISE) {
				dotXNorm = clamp((ch.phasePos * riseTime) / total, 0.f, 1.f);
			} else if (ch.phase == CHANNEL_FALL) {
				dotXNorm = clamp((riseTime + ch.phasePos * fallTime) / total, 0.f, 1.f);
			}
		}
		float dotYNorm = clamp((ch.out - FUNCTION_V_MIN) / std::max(FG_V_MAX - FUNCTION_V_MIN, 1e-6f), 0.f, 1.f);
		publishPreviewDot(previewShared, fgDotVisible, dotXNorm, dotYNorm);

		ChannelResult result;
		result.cycleOn = cycleOn;
		return result;
	}

	Proc() {
		initKnobCurveLut();
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(CYCLE_PARAM, 0.f, 1.f, 0.f, "Cycle");
		configParam(RISE_PARAM, 0.f, 1.f, 0.f, "Rise");
		configParam(FALL_PARAM, 0.f, 1.f, 0.f, "Fall");
		configParam(SHAPE_PARAM, 0.f, 1.f, 0.f, "Shape");
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

	json_t* dataToJson() override {
		json_t* rootJ = json_object();
		json_object_set_new(rootJ, "cycleLatched", json_boolean(channel.cycleLatched));
		json_object_set_new(rootJ, "bandlimitedGateOutputs", json_boolean(bandlimitedGateOutputs));
		json_object_set_new(rootJ, "bandlimitedSignalOutputs", json_boolean(bandlimitedSignalOutputs));
		json_object_set_new(rootJ, "timingUpdateDiv", json_integer(timingUpdateDiv));
		json_object_set_new(rootJ, "timingInterpolate", json_boolean(timingInterpolate));
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
			bandlimitedGateOutputs = json_boolean_value(blepGatesJ);
		}

		json_t* blepSignalJ = json_object_get(rootJ, "bandlimitedSignalOutputs");
		if (blepSignalJ) {
			bandlimitedSignalOutputs = json_boolean_value(blepSignalJ);
		}

		json_t* timingDivJ = json_object_get(rootJ, "timingUpdateDiv");
		if (timingDivJ) {
			setTimingUpdateDiv(json_integer_value(timingDivJ));
		}

		json_t* timingInterpJ = json_object_get(rootJ, "timingInterpolate");
		if (timingInterpJ) {
			timingInterpolate = json_boolean_value(timingInterpJ);
		}
	}

	void process(const ProcessArgs& args) override {
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

		ChannelResult channelResult = processChannel(args, channel, channelConfig, previewState, previewUpdate, timingTick);
		float outRendered = channel.out * channel.signalOutputGain
			+ (bandlimitedSignalOutputs ? channel.signalBlep.process() : 0.f);
		float eorOut = (channel.eorGateState ? 10.f : 0.f) + (bandlimitedGateOutputs ? channel.eorGateBlep.process() : 0.f);
		float eocOut = (channel.eocGateState ? 10.f : 0.f) + (bandlimitedGateOutputs ? channel.eocGateBlep.process() : 0.f);
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
	}
};

namespace {

struct IMBigPushButton : CKD6 {
	int* mode = NULL;
	TransformWidget *tw;
	IMBigPushButton() {
		setSizeRatio(0.9f);
	}
	void setSizeRatio(float ratio) {
		// Scale only the SVG child so hit area follows the visible button.
		sw->box.size = sw->box.size.mult(ratio);
		fb->removeChild(sw);
		tw = new TransformWidget();
		tw->addChild(sw);
		tw->scale(Vec(ratio, ratio));
		tw->box.size = sw->box.size;
		fb->addChild(tw);
		box.size = sw->box.size;
		shadow->box.size = sw->box.size;
	}
};

// Create a bigger basic button
struct BigTL1105 : TL1105 {
    BigTL1105() {
        // Dialed back to ~85% of previous size for a tighter click area.
        box.size = mm2px(Vec(9.5, 9.5));
    }
};

struct BananutBlack : app::SvgPort {
	BananutBlack() {
		setSvg(Svg::load(asset::plugin(pluginInstance, "res/BananutBlack.svg")));
	}
};

struct WavePreviewWidget : Widget {
	static constexpr int POINT_COUNT = 320;
	static constexpr int PREVIEW_LUT_SIZE = 1024;
	static constexpr float CENTER_LINE_WIDTH = 1.0f;
	static constexpr float WAVE_LINE_WIDTH = 1.4f;
	static constexpr float WAVE_EDGE_PAD = 1.0f;
	static constexpr float DOT_RADIUS = 2.1f;
	static constexpr float DOT_SHOW_MAX_HZ = 2.0f;
	static constexpr float DOT_HIDE_MIN_HZ = 2.4f;
	static constexpr float LABEL_FONT_SIZE = 11.5f;
	std::array<Vec, POINT_COUNT> points {};
	uint32_t lastVersion = 0;
	bool pointsValid = false;
	float lastFreqHz = 100.f;
	float dotXNorm = 0.f;
	float dotYNorm = 0.f;
	bool dotVisible = false;

	WavePreviewWidget() = default;

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

		int peakIndex = int(std::round(riseRatio * float(POINT_COUNT - 1)));
		peakIndex = std::max(0, std::min(POINT_COUNT - 1, peakIndex));
		float peakPx = left + (float(peakIndex) / float(POINT_COUNT - 1)) * drawW;
		points[peakIndex] = Vec(peakPx, top);
		points.front() = Vec(left, bottom);
		points.back() = Vec(right, bottom);
		pointsValid = true;
	}

	void step() override {
		Widget::step();
		ModuleWidget* moduleWidget = getAncestorOfType<ModuleWidget>();
		Proc* modulePtr = moduleWidget ? moduleWidget->getModule<Proc>() : nullptr;
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
		if (lastFreqHz >= DOT_HIDE_MIN_HZ) {
			dotVisible = false;
		} else if (lastFreqHz <= DOT_SHOW_MAX_HZ) {
			dotVisible = previewDotVisible;
		}
		if (!pointsValid || version != lastVersion) {
			rebuildPoints(riseTime, fallTime, curveSigned, interactiveRecent);
			lastVersion = version;
		}
	}

	void draw(const DrawArgs& args) override {
		nvgSave(args.vg);
		nvgScissor(args.vg, 0.f, 0.f, box.size.x, box.size.y);

		if (pointsValid) {
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, points[0].x, points[0].y);
			for (int i = 1; i < POINT_COUNT; ++i) {
				nvgLineTo(args.vg, points[i].x, points[i].y);
			}
			nvgStrokeColor(args.vg, nvgRGBA(230, 230, 220, 255));
			nvgStrokeWidth(args.vg, WAVE_LINE_WIDTH);
			nvgLineCap(args.vg, NVG_BUTT);
			nvgLineJoin(args.vg, NVG_ROUND);
			nvgStroke(args.vg);
		}
		if (pointsValid && dotVisible) {
			float w = std::max(box.size.x, 1.f);
			float drawPad = 0.5f * WAVE_LINE_WIDTH + WAVE_EDGE_PAD;
			float left = drawPad;
			float right = std::max(left + 1.f, w - drawPad);
			float drawW = right - left;
			float x = left + clamp(dotXNorm, 0.f, 1.f) * drawW;
			// Keep the marker visually glued to the rendered waveform by sampling
			// directly from the preview polyline instead of a separately published y.
			float idx = clamp(dotXNorm, 0.f, 1.f) * float(POINT_COUNT - 1);
			int i0 = clamp(int(std::floor(idx)), 0, POINT_COUNT - 1);
			int i1 = std::min(i0 + 1, POINT_COUNT - 1);
			float f = idx - float(i0);
			float y = points[i0].y + (points[i1].y - points[i0].y) * f;
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
		nvgText(args.vg, box.size.x * 0.5f, 0.f, ampText, nullptr);
	}
};

struct ProcWidget : ModuleWidget {
	ProcWidget(Proc* module) {
		setModule(module);
		const std::string panelPath = asset::plugin(pluginInstance, "res/proc.svg");
		setPanel(createPanel(panelPath));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		//addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		//addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<IMBigPushButton>(mm2px(Vec(33.075, 20.138)), module, Proc::CYCLE_PARAM));
		addParam(createParamCentered<Davies1900hWhiteKnob>(mm2px(Vec(32.907, 36.293)), module, Proc::RISE_PARAM));
		addParam(createParamCentered<Davies1900hWhiteKnob>(mm2px(Vec(32.907, 53.079)), module, Proc::FALL_PARAM));
		addParam(createParamCentered<Davies1900hWhiteKnob>(mm2px(Vec(11.775, 57.926)), module, Proc::SHAPE_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(7.246, 28.71)), module, Proc::AMP_PARAM));
		{
			AmpVoltageReadoutWidget* ampReadout = new AmpVoltageReadoutWidget();
			ampReadout->module = module;
			ampReadout->paramId = Proc::AMP_PARAM;
			ampReadout->box.pos = mm2px(Vec(2.5, 32.15));
			ampReadout->box.size = mm2px(Vec(9.6, 2.6));
			addChild(ampReadout);
		}
		{
			WavePreviewWidget* previewWidget = new WavePreviewWidget();
			math::Rect previewRectMm;
			if (panel_svg::loadRectFromSvgMm(panelPath, "CH1_PREVIEW", &previewRectMm)) {
				// Keep the legacy SVG id until proc.svg is cleaned up as well.
				previewRectMm = insetRectMm(previewRectMm, 0.2f);
				previewWidget->box.pos = mm2px(previewRectMm.pos);
				previewWidget->box.size = mm2px(previewRectMm.size);
			}
			else {
				previewWidget->box.pos = mm2px(Vec(3.75998355f, 68.96602539f));
				previewWidget->box.size = mm2px(Vec(20.78393382f, 11.24561948f));
			}
			addChild(previewWidget);
		}

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.247, 16.654)), module, Proc::SIGNAL_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(19.943, 16.654)), module, Proc::TRIGGER_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.207, 40.367)), module, Proc::HALT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(19.943, 32.416)), module, Proc::RISE_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(19.943, 44.898)), module, Proc::BOTH_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(23.604, 63.263)), module, Proc::FALL_CV_INPUT));

		addOutput(createOutputCentered<BananutBlack>(mm2px(Vec(9.437, 96.946)), module, Proc::EOR_OUTPUT));
		addOutput(createOutputCentered<BananutBlack>(mm2px(Vec(26.595, 96.915)), module, Proc::EOC_OUTPUT));
		addOutput(createOutputCentered<BananutBlack>(mm2px(Vec(9.447, 110.682)), module, Proc::MAIN_OUTPUT));
		addOutput(createOutputCentered<BananutBlack>(mm2px(Vec(26.552, 110.882)), module, Proc::NEG_OUTPUT));

		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(33.075, 14.055)), module, Proc::CYCLE_LIGHT));

		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(15.937, 96.76)), module, Proc::EOR_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(33.645, 96.952)), module, Proc::EOC_LIGHT));
		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(15.947, 110.758)), module, Proc::MAIN_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(33.579, 110.941)), module, Proc::NEG_LIGHT));
	}

	void appendContextMenu(Menu* menu) override {
		Proc* proc = dynamic_cast<Proc*>(module);
		assert(menu);

		menu->addChild(new MenuSeparator());
		if (proc) {
			menu->addChild(createMenuLabel("Performance"));
			menu->addChild(createBoolPtrMenuItem("Bandlimited EOR/EOC", "", &proc->bandlimitedGateOutputs));
			menu->addChild(createBoolPtrMenuItem("Bandlimited Signal Outputs", "", &proc->bandlimitedSignalOutputs));
			menu->addChild(createMenuLabel("Rate Control"));
			menu->addChild(createBoolPtrMenuItem("Interpolate Timing Updates", "", &proc->timingInterpolate));
			menu->addChild(createSubmenuItem("Timing Update Rate", "",
				[=](Menu* submenu) {
					auto addDivItem = [=](int div, std::string label) {
						submenu->addChild(createCheckMenuItem(label, "",
							[=]() { return proc->timingUpdateDiv == div; },
							[=]() { proc->setTimingUpdateDiv(div); }
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
