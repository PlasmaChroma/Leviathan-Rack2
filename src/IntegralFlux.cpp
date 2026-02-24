#include "plugin.hpp"
#include <dsp/minblep.hpp>


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
		OUTER_IDLE,
		OUTER_RISE,
		OUTER_FALL
	};

	struct OuterChannelState {
		dsp::SchmittTrigger trigEdge;
		dsp::SchmittTrigger cycleButtonEdge;
		dsp::MinBlepGenerator<16, 16> gateBlep;

		OuterPhase phase = OUTER_IDLE;
		float phasePos = 0.f;
		float out = 0.f;
		bool cycleLatched = false;
		bool gateState = false;
		bool warpScaleValid = false;
		float cachedShapeSigned = 0.f;
		float cachedWarpScale = 1.f;
		bool stageTimeValid = false;
		float cachedRiseKnob = 0.f;
		float cachedFallKnob = 0.f;
		float cachedShape = 0.f;
		float cachedRiseCv = 0.f;
		float cachedFallCv = 0.f;
		float cachedBothCv = 0.f;
		float cachedRiseTime = 0.01f;
		float cachedFallTime = 0.01f;
		float activeRiseTime = 0.01f;
		float activeFallTime = 0.01f;
		float riseTimeStep = 0.f;
		float fallTimeStep = 0.f;
		int timeInterpSamplesLeft = 0;
	};

	struct OuterChannelConfig {
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
		float logShapeTimeScale;
		float expShapeTimeScale;
		OuterPhase gateHighPhase;
	};

	struct OuterChannelResult {
		bool cycleOn = false;
	};

	struct MixNonIdealCal {
		bool enabled = true;

		// SUM
		float sumSatV = 10.f;
		float sumDrive = 1.15f;

		// OR
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
	MixNonIdealCal mixCal;
	bool bandlimitedGateOutputs = false;
	int timingUpdateDiv = 1;
	int timingUpdateCounter = 0;
	bool timingInterpolate = true;
	float lightUpdateTimer = 0.f;
	static constexpr float LINEAR_SHAPE = 0.33f;
	static constexpr float OUTER_V_MIN = 0.f;
	static constexpr float OUTER_V_MAX = 10.2f;
	static constexpr float WARP_K_MAX = 40.f;
	static constexpr int WARP_SCALE_SAMPLES = 16;
	static constexpr float PARAM_CACHE_EPS = 1e-4f;
	static constexpr float CV_CACHE_EPS = 1e-3f;
	static constexpr float LIGHT_UPDATE_INTERVAL = 1.f / 120.f;

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

	static float processUnifiedShapedSlew(
		float out,
		float in,
		float riseTime,
		float fallTime,
		float shapeSigned,
		float warpScale,
		float dt
	) {
		float delta = in - out;
		if (delta == 0.f) {
			return out;
		}

		float stageTime = (delta > 0.f) ? riseTime : fallTime;
		stageTime = std::max(stageTime, 1e-6f);
		float range = OUTER_V_MAX - OUTER_V_MIN;
		// Slew-limiting mode must handle bipolar signals.
		// Use normalized magnitude so negative voltages don't clamp to x=0.
		float x = clamp(std::fabs(out) / std::max(OUTER_V_MAX, 1e-6f), 0.f, 1.f);
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
		float p = f - 1.f;
		float step = newState ? 10.f : -10.f;
		ch.gateBlep.insertDiscontinuity(p, step);
		ch.gateState = newState;
	}

	static void setGateStateImmediate(OuterChannelState& ch, bool newState) {
		ch.gateState = newState;
	}

	void setTimingUpdateDiv(int div) {
		timingUpdateDiv = std::max(1, div);
		timingUpdateCounter = 0;
		ch1.stageTimeValid = false;
		ch4.stageTimeValid = false;
	}

	void updateActiveStageTimes(OuterChannelState& ch) {
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

		float computeShapeTimeScale(float shape, float knob, float logScale, float expScale) const {
		shape = clamp(shape, 0.f, 1.f);
		(void) knob;
		if (shape < LINEAR_SHAPE) {
			float t = shape / LINEAR_SHAPE;
			return std::pow(logScale, 1.f - t);
		}
		if (shape > LINEAR_SHAPE) {
			float t = (shape - LINEAR_SHAPE) / (1.f - LINEAR_SHAPE);
			return std::pow(expScale, t);
		}
		return 1.f;
	}

		float computeStageTime(
			float knob,
			float stageCv,
			float bothCv,
			float shape,
			bool applyShapeTimeScale,
			float logShapeTimeScale,
			float expShapeTimeScale
		) const {
		// Baseline at knob minimum (linear shape) calibrated near ~666Hz cycle.
		const float minTime = 0.00075075f;
		// Absolute floor allows EXP/positive CV to run faster than the linear baseline.
		const float absoluteMinTime = 0.0001f;
		const float maxTime = 1500.f;
		// Use a curved knob law so noon timing tracks measured hardware behavior.
		// With this exponent, knob=0.5 is ~23x slower than knob=0 (not ~1400x).
		float knobShaped = std::pow(clamp(knob, 0.f, 1.f), 2.2f);
		float t = minTime * std::pow(maxTime / minTime, knobShaped);

		// Rise/Fall CV is linear over +/-8V.
		float linearScale = 1.f + clamp(stageCv, -8.f, 8.f) / 8.f;
		linearScale = std::max(linearScale, 0.05f);
		t *= linearScale;

		// Both CV is bipolar exponential, positive = faster, negative = slower.
		float bothScale = std::pow(2.f, -clamp(bothCv, -8.f, 8.f) / 2.f);
		t *= bothScale;
		if (applyShapeTimeScale) {
			t *= computeShapeTimeScale(shape, knob, logShapeTimeScale, expShapeTimeScale);
		}

		return clamp(t, absoluteMinTime, maxTime);
	}

	void triggerOuterFunction(OuterChannelState& ch) {
		ch.phase = OUTER_RISE;
		ch.phasePos = 0.f;
	}

	OuterChannelResult processOuterChannel(const ProcessArgs& args, OuterChannelState& ch, const OuterChannelConfig& cfg, bool timingTick) {
		float dt = args.sampleTime;

		if (ch.cycleButtonEdge.process(params[cfg.cycleParam].getValue())) {
			ch.cycleLatched = !ch.cycleLatched;
		}

		bool cycleCvHigh = inputs[cfg.cycleCvInput].getVoltage() >= 2.5f;
		bool cycleOn = ch.cycleLatched || cycleCvHigh;
		bool gateWasHigh = (ch.phase == cfg.gateHighPhase);

		bool trigRise = ch.trigEdge.process(inputs[cfg.trigInput].getVoltage());
		if (trigRise && ch.phase != OUTER_RISE) {
			triggerOuterFunction(ch);
		}

		float riseKnob = params[cfg.riseParam].getValue();
		float fallKnob = params[cfg.fallParam].getValue();
		float shape = params[cfg.shapeParam].getValue();
		float riseCv = inputs[cfg.riseCvInput].getVoltage();
		float fallCv = inputs[cfg.fallCvInput].getVoltage();
		float bothCv = inputs[cfg.bothCvInput].getVoltage();
		if (!ch.stageTimeValid || timingTick) {
			bool stageTimeDirty = !ch.stageTimeValid
				|| std::fabs(riseKnob - ch.cachedRiseKnob) > PARAM_CACHE_EPS
				|| std::fabs(fallKnob - ch.cachedFallKnob) > PARAM_CACHE_EPS
				|| std::fabs(shape - ch.cachedShape) > PARAM_CACHE_EPS
				|| std::fabs(riseCv - ch.cachedRiseCv) > CV_CACHE_EPS
				|| std::fabs(fallCv - ch.cachedFallCv) > CV_CACHE_EPS
				|| std::fabs(bothCv - ch.cachedBothCv) > CV_CACHE_EPS;
			if (stageTimeDirty) {
				ch.cachedRiseTime = computeStageTime(
					riseKnob,
					riseCv,
					bothCv,
					shape,
					true,
					cfg.logShapeTimeScale,
					cfg.expShapeTimeScale
				);
				ch.cachedFallTime = computeStageTime(
					fallKnob,
					fallCv,
					bothCv,
					shape,
					true,
					cfg.logShapeTimeScale,
					cfg.expShapeTimeScale
				);
				ch.cachedRiseKnob = riseKnob;
				ch.cachedFallKnob = fallKnob;
				ch.cachedShape = shape;
				ch.cachedRiseCv = riseCv;
				ch.cachedFallCv = fallCv;
				ch.cachedBothCv = bothCv;
				if (!ch.stageTimeValid) {
					ch.activeRiseTime = ch.cachedRiseTime;
					ch.activeFallTime = ch.cachedFallTime;
					ch.riseTimeStep = 0.f;
					ch.fallTimeStep = 0.f;
					ch.timeInterpSamplesLeft = 0;
				}
				else if (timingInterpolate && timingUpdateDiv > 1) {
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
		float shapeSigned = shapeSignedFromKnob(shape);
		if (!ch.warpScaleValid || std::fabs(shapeSigned - ch.cachedShapeSigned) > 1e-4f) {
			ch.cachedShapeSigned = shapeSigned;
			ch.cachedWarpScale = slopeWarpScale(shapeSigned);
			ch.warpScaleValid = true;
		}
		float scale = ch.cachedWarpScale;

		bool signalPatched = inputs[cfg.signalInput].isConnected();
		if (ch.phase == OUTER_IDLE && cycleOn) {
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
			float s = shapeSigned;
			float range = OUTER_V_MAX - OUTER_V_MIN;

			if (ch.phase == OUTER_RISE) {
				float dpPhase = dt / riseTime;
				ch.phasePos += dpPhase;
				float x = clamp((ch.out - OUTER_V_MIN) / range, 0.f, 1.f);
				float dp = clamp(dt / riseTime, 0.f, 0.5f);
				x += dp * slopeWarp(x, s) * scale;
				x = clamp(x, 0.f, 1.f);
				ch.out = OUTER_V_MIN + x * range;
				if (ch.phasePos >= 1.f || x >= 1.f) {
					float f = phaseCrossingFraction(ch.phasePos, dpPhase);
					float overshoot = std::max(ch.phasePos - 1.f, 0.f);
					ch.phasePos = overshoot * (riseTime / std::max(fallTime, 1e-6f));
					ch.phase = OUTER_FALL;
					ch.out = OUTER_V_MAX;
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
				x = clamp(x, 0.f, 1.f);
				ch.out = OUTER_V_MIN + x * range;
				if (ch.phasePos >= 1.f || x <= 0.f) {
					float f = phaseCrossingFraction(ch.phasePos, dpPhase);
					ch.phasePos = 0.f;
					ch.phase = OUTER_IDLE;
					ch.out = OUTER_V_MIN;
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
				ch.out,
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
			8.102198f,  // From doc/Measurements.md, CH1 shape min at rise/fall=0.
			0.732835f,  // From doc/Measurements.md, CH1 shape max at rise/fall=0.
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
			7.672819f,  // From doc/Measurements.md, CH4 shape min at rise/fall=0.
			0.690657f,  // From doc/Measurements.md, CH4 shape max at rise/fall=0.
			OUTER_RISE
		};

		bool timingTick = true;
		if (timingUpdateDiv > 1) {
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
		OuterChannelResult ch1Result = processOuterChannel(args, ch1, ch1Cfg, timingTick);
		OuterChannelResult ch4Result = processOuterChannel(args, ch4, ch4Cfg, timingTick);
		float ch1Var = clamp(ch1.out * attenuverterGain(params[ATTENUATE_1_PARAM].getValue()), -10.f, 10.f);
		float ch2In = inputs[INPUT_2_INPUT].isConnected() ? inputs[INPUT_2_INPUT].getVoltage() : 10.f;
		float ch2Var = clamp(ch2In * attenuverterGain(params[ATTENUATE_2_PARAM].getValue()), -10.f, 10.f);
		float ch3In = inputs[INPUT_3_INPUT].isConnected() ? inputs[INPUT_3_INPUT].getVoltage() : 5.f;
		float ch3Var = clamp(ch3In * attenuverterGain(params[ATTENUATE_3_PARAM].getValue()), -10.f, 10.f);
		float ch4Var = clamp(ch4.out * attenuverterGain(params[ATTENUATE_4_PARAM].getValue()), -10.f, 10.f);
		float eorOut = (ch1.gateState ? 10.f : 0.f) + (bandlimitedGateOutputs ? ch1.gateBlep.process() : 0.f);
		float eocOut = (ch4.gateState ? 10.f : 0.f) + (bandlimitedGateOutputs ? ch4.gateBlep.process() : 0.f);
		bool eorHigh = ch1.gateState;
		bool eocHigh = ch4.gateState;
		float busV1 = outputs[OUT_1_OUTPUT].isConnected() ? 0.f : ch1Var;
		float busV2 = outputs[OUT_2_OUTPUT].isConnected() ? 0.f : ch2Var;
		float busV3 = outputs[OUT_3_OUTPUT].isConnected() ? 0.f : ch3Var;
		float busV4 = outputs[OUT_4_OUTPUT].isConnected() ? 0.f : ch4Var;
		float sumRaw = busV1 + busV2 + busV3 + busV4;
		float orRaw = std::fmax(0.f, std::fmax(std::fmax(busV1 - mixCal.orVDrop, busV2 - mixCal.orVDrop), std::fmax(busV3 - mixCal.orVDrop, busV4 - mixCal.orVDrop)));
		float sumOut = 0.f;
		float invOut = 0.f;
		float orOut = 0.f;
		if (mixCal.enabled) {
			sumOut = softSatSymFast(sumRaw, mixCal.sumSatV, mixCal.sumDrive);
			invOut = -sumOut;
			if (mixCal.invUseExtraSat) {
				invOut = softSatSymFast(invOut, mixCal.invSatV, mixCal.invDrive);
			}
			orOut = softSatPosFast(orRaw, mixCal.orSatV, mixCal.orDrive);
		}
		else {
			sumOut = clamp(sumRaw, -10.f, 10.f);
			invOut = clamp(-sumOut, -10.f, 10.f);
			orOut = clamp(orRaw, 0.f, 10.f);
		}

		outputs[EOR_1_OUTPUT].setVoltage(eorOut);
		outputs[EOC_4_OUTPUT].setVoltage(eocOut);
		outputs[OR_OUT_OUTPUT].setVoltage(orOut);
		outputs[SUM_OUT_OUTPUT].setVoltage(sumOut);
		outputs[INV_OUT_OUTPUT].setVoltage(invOut);

		outputs[CH_1_UNITY_OUTPUT].setVoltage(ch1.out);
		outputs[OUT_1_OUTPUT].setVoltage(ch1Var);
		outputs[OUT_2_OUTPUT].setVoltage(ch2Var);
		outputs[OUT_3_OUTPUT].setVoltage(ch3Var);
		outputs[OUT_4_OUTPUT].setVoltage(ch4Var);
		outputs[CH_4_UNITY_OUTPUT].setVoltage(ch4.out);

		if (lightTick) {
			lights[CYCLE_1_LED_LIGHT].setBrightness(ch1Result.cycleOn ? 1.f : 0.f);
			lights[CYCLE_4_LED_LIGHT].setBrightness(ch4Result.cycleOn ? 1.f : 0.f);
			lights[EOR_CH_1_LIGHT].setBrightness(eorHigh ? 1.f : 0.f);
			lights[EOC_CH_4_LIGHT].setBrightness(eocHigh ? 1.f : 0.f);
			lights[LIGHT_UNITY_1_LIGHT].setBrightness(clamp(std::fabs(ch1.out) / OUTER_V_MAX, 0.f, 1.f));
			lights[LIGHT_UNITY_4_LIGHT].setBrightness(clamp(std::fabs(ch4.out) / OUTER_V_MAX, 0.f, 1.f));
			lights[OR_LED_LIGHT].setBrightness(clamp(orOut / 10.f, 0.f, 1.f));
			lights[INV_LED_LIGHT].setBrightness(clamp(std::fabs(invOut) / 10.f, 0.f, 1.f));
		}
	}
};

struct MyImageWidget : Widget {
	int imageHandle = -1;

    void draw(const DrawArgs& args) override {
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

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(31.875, 14.855)), module, IntegralFlux::CYCLE_1_LED_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(69.353, 14.855)), module, IntegralFlux::CYCLE_4_LED_LIGHT));
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
			menu->addChild(createMenuLabel("Gate Outputs"));
			menu->addChild(createBoolPtrMenuItem("Bandlimited EOR/EOC", "", &maths->bandlimitedGateOutputs));
			menu->addChild(createMenuLabel("Timing"));
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
