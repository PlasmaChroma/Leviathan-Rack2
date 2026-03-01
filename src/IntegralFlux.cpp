#include "plugin.hpp"
#include <dsp/minblep.hpp>
#include <array>
#include <cstdio>
#include <atomic>


struct IntegralFlux : Module {
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
	};

	struct MixNonIdealCal {
		bool enabled = true;

		// SUM
		// Symmetric soft saturation models analog summing headroom.
		float sumSatV = 10.f;
		float sumDrive = 1.15f;

		// OR
		// Positive-only saturation models diode OR behavior at high levels.
		float orSatV = 10.f;
		float orDrive = 1.05f;
		float orVDrop = 0.f;  // Phase 1 keeps threshold behavior disabled.

		// INV
		bool invUseExtraSat = false;
		float invSatV = 10.f;
		float invDrive = 1.0f;
	};

	OuterChannelState ch1;
	OuterChannelState ch4;
	struct PreviewSharedState {
		// Lock-free handoff from engine thread -> UI thread.
		// Atomics keep preview independent from DSP timing.
		std::atomic<float> riseTime {0.01f};
		std::atomic<float> fallTime {0.01f};
		std::atomic<float> curveSigned {0.f};
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
	MixNonIdealCal mixCal;
	PreviewSharedState previewCh1;
	PreviewSharedState previewCh4;
	PreviewUpdateState previewUpdateCh1;
	PreviewUpdateState previewUpdateCh4;
	bool bandlimitedGateOutputs = false;
	bool bandlimitedSignalOutputs = true;
	int timingUpdateDiv = 1;
	int timingUpdateCounter = 0;
	bool timingInterpolate = true;
	// UI light updates are rate-limited to reduce engine overhead.
	float lightUpdateTimer = 0.f;
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
	// Hardware-like FG ceilings.
	static constexpr float OUTER_MAX_CYCLE_HZ = 1000.f;
	static constexpr float OUTER_MAX_TRIGGER_HZ = 2000.f;
	static constexpr float CV_OCT_CLAMP = 12.f;
	static constexpr float STAGE_CV_OCT_PER_V = 0.5f;
	static constexpr float PREVIEW_INTERACTIVE_INTERVAL = 1.f / 60.f;
	static constexpr float PREVIEW_CV_INTERVAL = 1.f / 30.f;
	static constexpr float PREVIEW_INTERACTIVE_HOLD = 0.25f;
	static constexpr int KNOB_CURVE_LUT_SIZE = 4096;
	std::array<float, KNOB_CURVE_LUT_SIZE> knobCurveLut {};

	static float attenuverterGain(float knob01) {
		// Noon = 0, CCW = negative, CW = positive.
		return clamp(knob01, 0.f, 1.f) * 2.f - 1.f;
	}

	static float fastTanh(float x) {
		// Low-cost tanh approximation.
		float x2 = x * x;
		return x * (27.f + x2) / (27.f + 9.f * x2);
	}

	static float softSatSymFast(float x, float satV, float drive) {
		satV = std::max(satV, 1e-6f);
		return satV * fastTanh((drive / satV) * x);
	}

	static float softSatPosFast(float x, float satV, float drive) {
		float y = softSatSymFast(std::fmax(0.f, x), satV, drive);
		return clamp(y, 0.f, satV);
	}

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

	static float computeSegPhase(float out, float startOut, float invSpan) {
		if (std::fabs(invSpan) < 1e-9f) {
			return 1.f;
		}
		float phase = (out - startOut) * invSpan;
		return clamp(phase, 0.f, 1.f);
	}

	float processUnifiedShapedSlew(
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
		float out = ch.out;
		float delta = in - out;
		if (delta == 0.f) {
			return out;
		}
		int dir = (delta > 0.f) ? 1 : -1;
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
		}
		return out;
	}

	static float phaseCrossingFraction(float phasePos, float dp) {
		// Returns the within-sample crossing point for BLEP insertion.
		// 1.0 means transition near end-of-sample, 0.0 near beginning.
		if (dp <= 1e-9f) {
			return 1.f;
		}
		return clamp(1.f - ((phasePos - 1.f) / dp), 0.f, 1.f);
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

	void setTimingUpdateDiv(int div) {
		// Changing update rate invalidates cached timing so channels resync immediately.
		timingUpdateDiv = std::max(1, div);
		timingUpdateCounter = 0;
		ch1.stageTimeValid = false;
		ch4.stageTimeValid = false;
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

	void getPreviewState(int channel, float& riseTime, float& fallTime, float& curveSigned, bool& interactiveRecent, uint32_t& version) const {
		const PreviewSharedState& shared = (channel == 4) ? previewCh4 : previewCh1;
		riseTime = shared.riseTime.load(std::memory_order_relaxed);
		fallTime = shared.fallTime.load(std::memory_order_relaxed);
		curveSigned = shared.curveSigned.load(std::memory_order_relaxed);
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
		float stageCvSoft = softClamp8(stageCv);
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
		bool timingTick
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
		bool gateWasHigh = (ch.phase == cfg.gateHighPhase);

		bool trigRise = ch.trigEdge.process(inputs[cfg.trigInput].getVoltage());
		bool trigAccepted = false;
		if (trigRise && ch.trigRearmSec <= 0.f && ch.phase != OUTER_RISE) {
			triggerOuterFunction(ch);
			trigAccepted = true;
			ch.trigRearmSec = 1.f / std::max(OUTER_MAX_TRIGGER_HZ, 1.f);
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

		bool signalPatched = inputs[cfg.signalInput].isConnected();
		if (ch.phase == OUTER_IDLE && cycleOn) {
			// Cycle retriggers as soon as the channel reaches idle.
			triggerOuterFunction(ch);
		}
		bool gateIsHigh = (ch.phase == cfg.gateHighPhase);
		if (gateIsHigh != gateWasHigh) {
			// Transition occurred at start-of-sample due to trigger/cycle state.
			if (bandlimitedGateOutputs) {
				insertGateTransition(ch, gateIsHigh, 1e-6f);
			}
			else {
				setGateStateImmediate(ch, gateIsHigh);
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
				float inV = inputs[cfg.signalInput].getVoltage();
				float inSoft = softClamp8(inV);
				xIn = clamp((inSoft - OUTER_V_MIN) / range, 0.f, 1.f);
				float a = 1.f - std::exp(-dt / OUTER_INJECT_TAU);
				injectAlpha = OUTER_INJECT_GAIN * clamp(a, 0.f, 1.f);
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
					float prevOut = ch.out;
					ch.out = OUTER_V_MAX;
					if (bandlimitedSignalOutputs) {
						insertSignalTransition(ch, ch.out - prevOut, f);
					}
					if (bandlimitedGateOutputs) {
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
					if (bandlimitedSignalOutputs) {
						insertSignalTransition(ch, ch.out - prevOut, f);
					}
					if (bandlimitedGateOutputs) {
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
			float in = inputs[cfg.signalInput].getVoltage();
			ch.out = processUnifiedShapedSlew(
				ch,
				in,
				riseTime,
				fallTime,
				shapeSigned,
				scale,
				dt
			);
		}
		else {
			ch.out = 0.f;
		}

		OuterChannelResult result;
		result.cycleOn = cycleOn;
		return result;
	}

	IntegralFlux() {
		initKnobCurveLut();
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(ATTENUATE_1_PARAM, 0.f, 1.f, 0.5f, "CH1 attenuverter");
		configParam(CYCLE_1_PARAM, 0.f, 1.f, 0.f, "CH1 cycle");
		configParam(CYCLE_4_PARAM, 0.f, 1.f, 0.f, "CH4 cycle");
		configParam(RISE_1_PARAM, 0.f, 1.f, 0.f, "CH1 rise");
		configParam(RISE_4_PARAM, 0.f, 1.f, 0.f, "CH4 rise");
		configParam(ATTENUATE_2_PARAM, 0.f, 1.f, 0.5f, "CH2 attenuverter");
		configParam(FALL_1_PARAM, 0.f, 1.f, 0.f, "CH1 fall");
		configParam(FALL_4_PARAM, 0.f, 1.f, 0.f, "CH4 fall");
		configParam(ATTENUATE_3_PARAM, 0.f, 1.f, 0.5f, "CH3 attenuverter");
		configParam(LIN_LOG_1_PARAM, 0.f, 1.f, 0.f, "CH1 shape");
		configParam(LIN_LOG_4_PARAM, 0.f, 1.f, 0.f, "CH4 shape");
		configParam(ATTENUATE_4_PARAM, 0.f, 1.f, 0.5f, "CH4 attenuverter");
		configInput(INPUT_1_INPUT, "CH1 signal");
		configInput(INPUT_1_TRIG_INPUT, "CH1 trigger");
		configInput(INPUT_2_INPUT, "CH2 signal");
		configInput(INPUT_3_INPUT, "CH3 signal");
		configInput(INPUT_4_TRIG_INPUT, "CH4 trigger");
		configInput(INPUT_4_INPUT, "CH4 signal");
		configInput(CH1_RISE_CV_INPUT, "CH1 rise CV");
		configInput(CH4_RISE_CV_INPUT, "CH4 rise CV");
		configInput(CH1_BOTH_CV_INPUT, "CH1 both CV");
		configInput(CH4_BOTH_CV_INPUT, "CH4 both CV");
		configInput(CH1_FALL_CV_INPUT, "CH1 fall CV");
		configInput(CH4_FALL_CV_INPUT, "CH4 fall CV");
		configInput(CH1_CYCLE_CV_INPUT, "CH1 cycle CV");
		configInput(CH4_CYCLE_CV_INPUT, "CH4 cycle CV");
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

	json_t* dataToJson() override {
		json_t* rootJ = json_object();
		json_object_set_new(rootJ, "ch1CycleLatched", json_boolean(ch1.cycleLatched));
		json_object_set_new(rootJ, "ch4CycleLatched", json_boolean(ch4.cycleLatched));
		json_object_set_new(rootJ, "mixNonIdealEnabled", json_boolean(mixCal.enabled));
		json_object_set_new(rootJ, "bandlimitedGateOutputs", json_boolean(bandlimitedGateOutputs));
		json_object_set_new(rootJ, "bandlimitedSignalOutputs", json_boolean(bandlimitedSignalOutputs));
		json_object_set_new(rootJ, "timingUpdateDiv", json_integer(timingUpdateDiv));
		json_object_set_new(rootJ, "timingInterpolate", json_boolean(timingInterpolate));
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

		json_t* mixEnabledJ = json_object_get(rootJ, "mixNonIdealEnabled");
		if (mixEnabledJ) {
			mixCal.enabled = json_boolean_value(mixEnabledJ);
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
		OuterChannelResult ch1Result;
		OuterChannelResult ch4Result;
		ch1Result = processOuterChannel(args, ch1, ch1Cfg, previewCh1, previewUpdateCh1, timingTick);
		ch4Result = processOuterChannel(args, ch4, ch4Cfg, previewCh4, previewUpdateCh4, timingTick);
		float ch1OutRendered = ch1.out + (bandlimitedSignalOutputs ? ch1.signalBlep.process() : 0.f);
		float ch4OutRendered = ch4.out + (bandlimitedSignalOutputs ? ch4.signalBlep.process() : 0.f);
		// Variable outputs are attenuverters; unity outputs bypass this scaling.
		float ch1Var = clamp(ch1OutRendered * attenuverterGain(params[ATTENUATE_1_PARAM].getValue()), -10.f, 10.f);
		float ch2In = inputs[INPUT_2_INPUT].isConnected() ? inputs[INPUT_2_INPUT].getVoltage() : 10.f;
		float ch2Var = clamp(ch2In * attenuverterGain(params[ATTENUATE_2_PARAM].getValue()), -10.f, 10.f);
		float ch3In = inputs[INPUT_3_INPUT].isConnected() ? inputs[INPUT_3_INPUT].getVoltage() : 5.f;
		float ch3Var = clamp(ch3In * attenuverterGain(params[ATTENUATE_3_PARAM].getValue()), -10.f, 10.f);
		float ch4Var = clamp(ch4OutRendered * attenuverterGain(params[ATTENUATE_4_PARAM].getValue()), -10.f, 10.f);
		float eorOut = (ch1.gateState ? 10.f : 0.f) + (bandlimitedGateOutputs ? ch1.gateBlep.process() : 0.f);
		float eocOut = (ch4.gateState ? 10.f : 0.f) + (bandlimitedGateOutputs ? ch4.gateBlep.process() : 0.f);
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
			float orRaw = std::fmax(0.f, std::fmax(std::fmax(busV1 - mixCal.orVDrop, busV2 - mixCal.orVDrop), std::fmax(busV3 - mixCal.orVDrop, busV4 - mixCal.orVDrop)));
			if (mixCal.enabled) {
				// Non-ideal mode: soft saturation and diode-ish OR response.
				sumOut = softSatSymFast(sumRaw, mixCal.sumSatV, mixCal.sumDrive);
				invOut = -sumOut;
				if (mixCal.invUseExtraSat) {
					invOut = softSatSymFast(invOut, mixCal.invSatV, mixCal.invDrive);
				}
				orOut = softSatPosFast(orRaw, mixCal.orSatV, mixCal.orDrive);
			}
			else {
				// Ideal digital fallback: hard clamps only.
				sumOut = clamp(sumRaw, -10.f, 10.f);
				invOut = clamp(-sumOut, -10.f, 10.f);
				orOut = clamp(orRaw, 0.f, 10.f);
			}
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
	}
};

struct MyImageWidget : Widget {
	int imageHandle = -1;

    void draw(const DrawArgs& args) override {
		// Lazy-load panel image on first draw to avoid startup overhead.
        if (imageHandle < 0) {
            std::string path = asset::plugin(pluginInstance, "res/maths2.jpg");
            imageHandle = nvgCreateImage(args.vg, path.c_str(), 0);
        }

        if (imageHandle >= 0) {
            NVGpaint imgPaint = nvgImagePattern(args.vg, 0, 0, box.size.x, box.size.y, 0, imageHandle, 1.0f);
            nvgBeginPath(args.vg);
            nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
            nvgFillPaint(args.vg, imgPaint);
            nvgFill(args.vg);
        }
    }
};

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
	static constexpr float LABEL_FONT_SIZE = 8.5f;
	int channel = 1;
	std::array<Vec, POINT_COUNT> points {};
	uint32_t lastVersion = 0;
	bool pointsValid = false;
	float lastFreqHz = 100.f;

	WavePreviewWidget(int channel) {
		this->channel = channel;
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
		IntegralFlux* modulePtr = nullptr;
		if (ModuleWidget* moduleWidget = getAncestorOfType<ModuleWidget>()) {
			modulePtr = moduleWidget->getModule<IntegralFlux>();
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
		bool interactiveRecent = false;
		uint32_t version = 0;
		modulePtr->getPreviewState(channel, riseTime, fallTime, curveSigned, interactiveRecent, version);
		// Displayed frequency reflects the currently effective cycle period.
		lastFreqHz = 1.f / std::max(riseTime + fallTime, 1e-6f);
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

		nvgResetScissor(args.vg);
		nvgRestore(args.vg);

		char freqText[32];
		if (lastFreqHz >= 1000.f) {
			std::snprintf(freqText, sizeof(freqText), "%4.2fkHz", lastFreqHz / 1000.f);
		}
		else {
			std::snprintf(freqText, sizeof(freqText), "%5.1fHz", lastFreqHz);
		}
		nvgFontSize(args.vg, LABEL_FONT_SIZE);
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 255));
		nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_TOP);
		// Keep label outside preview box to avoid occluding waveform.
		nvgText(args.vg, box.size.x * 0.5f, box.size.y + 1.5f, freqText, nullptr);
	}
};

struct IntegralFluxWidget : ModuleWidget {
	IntegralFluxWidget(IntegralFlux* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/flux.svg")));

        // use Rogan1PSBlue for the rise/fall knobs
        // use LargeLight<RedLight> for the cycle and EOR LEDs
        // use Rogan1PSWhite for the attenuverter knobs
        // use TL1105 for the cycle buttons
        // Davies1900hWhiteKnob

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<IMBigPushButton>(mm2px(Vec(31.875, 20.938)), module, IntegralFlux::CYCLE_1_PARAM));
		addParam(createParamCentered<IMBigPushButton>(mm2px(Vec(69.552, 20.938)), module, IntegralFlux::CYCLE_4_PARAM));

        addParam(createParamCentered<Davies1900hWhiteKnob>(mm2px(Vec(33.755, 36.293)), module, IntegralFlux::RISE_1_PARAM));
		addParam(createParamCentered<Davies1900hWhiteKnob>(mm2px(Vec(67.638, 36.293)), module, IntegralFlux::RISE_4_PARAM));
		addParam(createParamCentered<Davies1900hWhiteKnob>(mm2px(Vec(42.007, 53.079)), module, IntegralFlux::FALL_1_PARAM));
		addParam(createParamCentered<Davies1900hWhiteKnob>(mm2px(Vec(59.385, 53.079)), module, IntegralFlux::FALL_4_PARAM));
		addParam(createParamCentered<Davies1900hWhiteKnob>(mm2px(Vec(13.975, 57.178)), module, IntegralFlux::LIN_LOG_1_PARAM));
		addParam(createParamCentered<Davies1900hWhiteKnob>(mm2px(Vec(91.716, 57.178)), module, IntegralFlux::LIN_LOG_4_PARAM));
		{
			WavePreviewWidget* ch1Preview = new WavePreviewWidget(1);
			// From doc/preview_boxes.md (already includes 0.2 mm inset).
			ch1Preview->box.pos = mm2px(Vec(3.75998355f, 68.96602539f));
			ch1Preview->box.size = mm2px(Vec(20.78393382f, 11.24561948f));
			addChild(ch1Preview);
		}
		{
			WavePreviewWidget* ch4Preview = new WavePreviewWidget(4);
			// From doc/preview_boxes.md (already includes 0.2 mm inset).
			ch4Preview->box.pos = mm2px(Vec(77.52500000f, 68.96600100f));
			ch4Preview->box.size = mm2px(Vec(20.78393300f, 11.24562000f));
			addChild(ch4Preview);
		}

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(26.094, 86.446)), module, IntegralFlux::ATTENUATE_1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(42.042, 86.446)), module, IntegralFlux::ATTENUATE_2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(59.585, 86.446)), module, IntegralFlux::ATTENUATE_3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(75.931, 86.446)), module, IntegralFlux::ATTENUATE_4_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.947, 15.354)), module, IntegralFlux::INPUT_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(20.911, 15.354)), module, IntegralFlux::INPUT_1_TRIG_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(80.217, 15.354)), module, IntegralFlux::INPUT_4_TRIG_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(91.181, 15.354)), module, IntegralFlux::INPUT_4_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(40.049, 20.838)), module, IntegralFlux::CH1_CYCLE_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(61.179, 20.838)), module, IntegralFlux::CH4_CYCLE_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(21.683, 36.416)), module, IntegralFlux::CH1_RISE_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(79.81, 36.216)), module, IntegralFlux::CH4_RISE_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(26.633, 49.47)), module, IntegralFlux::CH1_BOTH_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(74.56, 49.27)), module, IntegralFlux::CH4_BOTH_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(32.704, 63.263)), module, IntegralFlux::CH1_FALL_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(69.389, 63.263)), module, IntegralFlux::CH4_FALL_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(42.143, 76.377)), module, IntegralFlux::INPUT_2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(59.585, 76.377)), module, IntegralFlux::INPUT_3_INPUT));

		addOutput(createOutputCentered<BananutBlack>(mm2px(Vec(10.037, 96.946)), module, IntegralFlux::EOR_1_OUTPUT));
		addOutput(createOutputCentered<BananutBlack>(mm2px(Vec(25.995, 96.915)), module, IntegralFlux::OUT_1_OUTPUT));
		addOutput(createOutputCentered<BananutBlack>(mm2px(Vec(41.943, 96.915)), module, IntegralFlux::OUT_2_OUTPUT));
		addOutput(createOutputCentered<BananutBlack>(mm2px(Vec(59.486, 96.915)), module, IntegralFlux::OUT_3_OUTPUT));
		addOutput(createOutputCentered<BananutBlack>(mm2px(Vec(75.832, 96.915)), module, IntegralFlux::OUT_4_OUTPUT));
		addOutput(createOutputCentered<BananutBlack>(mm2px(Vec(91.281, 96.915)), module, IntegralFlux::EOC_4_OUTPUT));
		addOutput(createOutputCentered<BananutBlack>(mm2px(Vec(10.047, 110.682)), module, IntegralFlux::CH_1_UNITY_OUTPUT));
		addOutput(createOutputCentered<BananutBlack>(mm2px(Vec(35.252, 110.882)), module, IntegralFlux::OR_OUT_OUTPUT));
		addOutput(createOutputCentered<BananutBlack>(mm2px(Vec(50.614, 110.882)), module, IntegralFlux::SUM_OUT_OUTPUT));
		addOutput(createOutputCentered<BananutBlack>(mm2px(Vec(65.975, 110.882)), module, IntegralFlux::INV_OUT_OUTPUT));
		addOutput(createOutputCentered<BananutBlack>(mm2px(Vec(91.281, 110.682)), module, IntegralFlux::CH_4_UNITY_OUTPUT));

		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(31.875, 14.855)), module, IntegralFlux::CYCLE_1_LED_LIGHT));
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(69.353, 14.855)), module, IntegralFlux::CYCLE_4_LED_LIGHT));
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(16.537, 96.76)), module, IntegralFlux::EOR_CH_1_LIGHT));
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(84.603, 96.716)), module, IntegralFlux::EOC_CH_4_LIGHT));
		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(16.547, 110.499)), module, IntegralFlux::LIGHT_UNITY_1_LIGHT));
		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(84.731, 110.599)), module, IntegralFlux::LIGHT_UNITY_4_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(28.274, 110.683)), module, IntegralFlux::OR_LED_LIGHT));
		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(72.954, 110.683)), module, IntegralFlux::INV_LED_LIGHT));
	}

	void appendContextMenu(Menu* menu) override {
		IntegralFlux* maths = dynamic_cast<IntegralFlux*>(module);
		assert(menu);

		menu->addChild(new MenuSeparator());
		menu->addChild(createMenuLabel("Mix Modeling"));
		if (maths) {
			menu->addChild(createBoolPtrMenuItem("Analog Mix Non-Idealities", "", &maths->mixCal.enabled));
			menu->addChild(createMenuLabel("Performance"));
			menu->addChild(createBoolPtrMenuItem("Bandlimited EOR/EOC", "", &maths->bandlimitedGateOutputs));
			menu->addChild(createBoolPtrMenuItem("Bandlimited CH1/CH4 Signal Outputs", "", &maths->bandlimitedSignalOutputs));
			menu->addChild(createMenuLabel("Rate Control"));
			menu->addChild(createBoolPtrMenuItem("Interpolate Timing Updates", "", &maths->timingInterpolate));
			menu->addChild(createSubmenuItem("Timing Update Rate", "",
				[=](Menu* submenu) {
					auto addDivItem = [=](int div, std::string label) {
						submenu->addChild(createCheckMenuItem(label, "",
							[=]() { return maths->timingUpdateDiv == div; },
							[=]() { maths->setTimingUpdateDiv(div); }
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

Model* modelIntegralFlux = createModel<IntegralFlux, IntegralFluxWidget>("IntegralFlux");
