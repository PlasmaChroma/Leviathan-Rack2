#include "plugin.hpp"


struct Maths : Module {
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

		OuterPhase phase = OUTER_IDLE;
		float phasePos = 0.f;
		float out = 0.f;
		bool cycleLatched = false;
		bool warpScaleValid = false;
		float cachedShapeSigned = 0.f;
		float cachedWarpScale = 1.f;
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
	static constexpr float LINEAR_SHAPE = 0.33f;
	static constexpr float OUTER_V_MIN = 0.f;
	static constexpr float OUTER_V_MAX = 10.2f;
	static constexpr float WARP_K_MAX = 40.f;
	static constexpr float WARP_P = 2.f;
	static constexpr int WARP_SCALE_SAMPLES = 16;

	static float attenuverterGain(float knob01) {
		// Noon = 0, CCW = negative, CW = positive.
		return clamp(knob01, 0.f, 1.f) * 2.f - 1.f;
	}

	static float softSatSym(float x, float satV, float drive) {
		satV = std::max(satV, 1e-6f);
		return satV * std::tanh((drive / satV) * x);
	}

	static float softSatPos(float x, float satV, float drive) {
		float y = softSatSym(std::fmax(0.f, x), satV, drive);
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
		if (s < 0.f) {
			// LOG: fast near 0V, slow near top.
			return 1.f / (1.f + k * std::pow(x, WARP_P));
		}
		// EXP: slow near 0V, fast near top.
		return 1.f + k * std::pow(x, WARP_P);
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

	OuterChannelResult processOuterChannel(const ProcessArgs& args, OuterChannelState& ch, const OuterChannelConfig& cfg) {
		float dt = args.sampleTime;

		if (ch.cycleButtonEdge.process(params[cfg.cycleParam].getValue())) {
			ch.cycleLatched = !ch.cycleLatched;
		}

		bool cycleCvHigh = inputs[cfg.cycleCvInput].getVoltage() >= 2.5f;
		bool cycleOn = ch.cycleLatched || cycleCvHigh;

		bool trigRise = ch.trigEdge.process(inputs[cfg.trigInput].getVoltage());
		if (trigRise && ch.phase != OUTER_RISE) {
			triggerOuterFunction(ch);
		}

		float shape = params[cfg.shapeParam].getValue();
		float riseTime = computeStageTime(
			params[cfg.riseParam].getValue(),
			inputs[cfg.riseCvInput].getVoltage(),
			inputs[cfg.bothCvInput].getVoltage(),
			shape,
			true,
			cfg.logShapeTimeScale,
			cfg.expShapeTimeScale
		);
		float fallTime = computeStageTime(
			params[cfg.fallParam].getValue(),
			inputs[cfg.fallCvInput].getVoltage(),
			inputs[cfg.bothCvInput].getVoltage(),
			shape,
			true,
			cfg.logShapeTimeScale,
			cfg.expShapeTimeScale
		);
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

		if (ch.phase != OUTER_IDLE) {
			float s = shapeSigned;
			float range = OUTER_V_MAX - OUTER_V_MIN;

			if (ch.phase == OUTER_RISE) {
				ch.phasePos += dt / riseTime;
				float x = clamp((ch.out - OUTER_V_MIN) / range, 0.f, 1.f);
				float dp = clamp(dt / riseTime, 0.f, 0.5f);
				x += dp * slopeWarp(x, s) * scale;
				x = clamp(x, 0.f, 1.f);
				ch.out = OUTER_V_MIN + x * range;
				if (ch.phasePos >= 1.f || x >= 1.f) {
					ch.phasePos = 0.f;
					ch.phase = OUTER_FALL;
					ch.out = OUTER_V_MAX;
				}
			}

			if (ch.phase == OUTER_FALL) {
				ch.phasePos += dt / fallTime;
				float x = clamp((ch.out - OUTER_V_MIN) / range, 0.f, 1.f);
				float dp = clamp(dt / fallTime, 0.f, 0.5f);
				x -= dp * slopeWarp(x, s) * scale;
				x = clamp(x, 0.f, 1.f);
				ch.out = OUTER_V_MIN + x * range;
				if (ch.phasePos >= 1.f || x <= 0.f) {
					ch.phasePos = 0.f;
					ch.phase = OUTER_IDLE;
					ch.out = OUTER_V_MIN;
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

	Maths() {
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
			0.732835f   // From doc/Measurements.md, CH1 shape max at rise/fall=0.
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
			0.690657f   // From doc/Measurements.md, CH4 shape max at rise/fall=0.
		};

		OuterChannelResult ch1Result = processOuterChannel(args, ch1, ch1Cfg);
		OuterChannelResult ch4Result = processOuterChannel(args, ch4, ch4Cfg);
		float ch1Var = clamp(ch1.out * attenuverterGain(params[ATTENUATE_1_PARAM].getValue()), -10.f, 10.f);
		float ch2In = inputs[INPUT_2_INPUT].isConnected() ? inputs[INPUT_2_INPUT].getVoltage() : 10.f;
		float ch2Var = clamp(ch2In * attenuverterGain(params[ATTENUATE_2_PARAM].getValue()), -10.f, 10.f);
		float ch3In = inputs[INPUT_3_INPUT].isConnected() ? inputs[INPUT_3_INPUT].getVoltage() : 5.f;
		float ch3Var = clamp(ch3In * attenuverterGain(params[ATTENUATE_3_PARAM].getValue()), -10.f, 10.f);
		float ch4Var = clamp(ch4.out * attenuverterGain(params[ATTENUATE_4_PARAM].getValue()), -10.f, 10.f);
		bool eorHigh = (ch1.phase == OUTER_FALL);
		bool eocHigh = (ch4.phase == OUTER_RISE);
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
			sumOut = softSatSym(sumRaw, mixCal.sumSatV, mixCal.sumDrive);
			invOut = -sumOut;
			if (mixCal.invUseExtraSat) {
				invOut = softSatSym(invOut, mixCal.invSatV, mixCal.invDrive);
			}
			orOut = softSatPos(orRaw, mixCal.orSatV, mixCal.orDrive);
		}
		else {
			sumOut = clamp(sumRaw, -10.f, 10.f);
			invOut = clamp(-sumOut, -10.f, 10.f);
			orOut = clamp(orRaw, 0.f, 10.f);
		}

		outputs[EOR_1_OUTPUT].setVoltage(eorHigh ? 10.f : 0.f);
		outputs[EOC_4_OUTPUT].setVoltage(eocHigh ? 10.f : 0.f);
		outputs[OR_OUT_OUTPUT].setVoltage(orOut);
		outputs[SUM_OUT_OUTPUT].setVoltage(sumOut);
		outputs[INV_OUT_OUTPUT].setVoltage(invOut);

		outputs[CH_1_UNITY_OUTPUT].setVoltage(ch1.out);
		outputs[OUT_1_OUTPUT].setVoltage(ch1Var);
		outputs[OUT_2_OUTPUT].setVoltage(ch2Var);
		outputs[OUT_3_OUTPUT].setVoltage(ch3Var);
		outputs[OUT_4_OUTPUT].setVoltage(ch4Var);
		outputs[CH_4_UNITY_OUTPUT].setVoltage(ch4.out);

		lights[CYCLE_1_LED_LIGHT].setBrightness(ch1Result.cycleOn ? 1.f : 0.f);
		lights[CYCLE_4_LED_LIGHT].setBrightness(ch4Result.cycleOn ? 1.f : 0.f);
		lights[EOR_CH_1_LIGHT].setBrightness(eorHigh ? 1.f : 0.f);
		lights[EOC_CH_4_LIGHT].setBrightness(eocHigh ? 1.f : 0.f);
		lights[LIGHT_UNITY_1_LIGHT].setBrightness(clamp(std::fabs(ch1.out) / OUTER_V_MAX, 0.f, 1.f));
		lights[LIGHT_UNITY_4_LIGHT].setBrightness(clamp(std::fabs(ch4.out) / OUTER_V_MAX, 0.f, 1.f));
		lights[OR_LED_LIGHT].setBrightness(clamp(orOut / 10.f, 0.f, 1.f));
		lights[INV_LED_LIGHT].setBrightness(clamp(std::fabs(invOut) / 10.f, 0.f, 1.f));
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

// Create a bigger basic button
struct BigTL1105 : TL1105 {
    BigTL1105() {
        // Dialed back to ~85% of previous size for a tighter click area.
        box.size = mm2px(Vec(9.5, 9.5));
    }
};

struct MathsWidget : ModuleWidget {
	MathsWidget(Maths* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/flux.svg")));

        // these are deliberately under the image because the buttons are not big enough for the UI elements
		addParam(createParamCentered<BigTL1105>(mm2px(Vec(10.349, 32.315)), module, Maths::CYCLE_1_PARAM));
		addParam(createParamCentered<BigTL1105>(mm2px(Vec(92.313, 32.315)), module, Maths::CYCLE_4_PARAM));

        MyImageWidget* img = new MyImageWidget();
        img->box.pos = Vec(0, 0);
        img->box.size = box.size;
        //addChild(img);
        
        // use Rogan1PSBlue for the rise/fall knobs
        // use LargeLight<RedLight> for the cycle and EOR LEDs
        // use Rogan1PSWhite for the attenuverter knobs
        // use TL1105 for the cycle buttons
		
		addParam(createParamCentered<Rogan1PSWhite>(mm2px(Vec(50.805, 23.141)), module, Maths::ATTENUATE_1_PARAM));
		addParam(createParamCentered<Rogan1PSBlue>(mm2px(Vec(30.423, 34.13)), module, Maths::RISE_1_PARAM));
		addParam(createParamCentered<Rogan1PSBlue>(mm2px(Vec(71.969, 34.085)), module, Maths::RISE_4_PARAM));
		addParam(createParamCentered<Rogan1PSWhite>(mm2px(Vec(50.805, 43.288)), module, Maths::ATTENUATE_2_PARAM));
		addParam(createParamCentered<Rogan1PSBlue>(mm2px(Vec(30.423, 56.125)), module, Maths::FALL_1_PARAM));
		addParam(createParamCentered<Rogan1PSBlue>(mm2px(Vec(71.969, 56.079)), module, Maths::FALL_4_PARAM));
		addParam(createParamCentered<Rogan1PSWhite>(mm2px(Vec(50.805, 62.906)), module, Maths::ATTENUATE_3_PARAM));
		addParam(createParamCentered<Rogan1PSBlue>(mm2px(Vec(30.423, 80.585)), module, Maths::LIN_LOG_1_PARAM));
		addParam(createParamCentered<Rogan1PSBlue>(mm2px(Vec(71.969, 80.539)), module, Maths::LIN_LOG_4_PARAM));
		addParam(createParamCentered<Rogan1PSWhite>(mm2px(Vec(50.805, 82.663)), module, Maths::ATTENUATE_4_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.794, 15.678)), module, Maths::INPUT_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(19.157, 15.659)), module, Maths::INPUT_1_TRIG_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(35.819, 15.668)), module, Maths::INPUT_2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(65.739, 15.668)), module, Maths::INPUT_3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(82.815, 15.668)), module, Maths::INPUT_4_TRIG_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(94.304, 15.668)), module, Maths::INPUT_4_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.628, 50.821)), module, Maths::CH1_RISE_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(94.516, 50.812)), module, Maths::CH4_RISE_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.202, 62.311)), module, Maths::CH1_BOTH_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(91.721, 62.218)), module, Maths::CH4_BOTH_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.575, 74.408)), module, Maths::CH1_FALL_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(94.402, 74.449)), module, Maths::CH4_FALL_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.199, 86.4)), module, Maths::CH1_CYCLE_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(91.82, 86.263)), module, Maths::CH4_CYCLE_CV_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(33.268, 101.985)), module, Maths::OUT_1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(45.246, 101.977)), module, Maths::OUT_2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(56.768, 101.944)), module, Maths::OUT_3_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(68.799, 102.011)), module, Maths::OUT_4_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.722, 114.057)), module, Maths::EOR_1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(19.774, 114.067)), module, Maths::CH_1_UNITY_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(39.333, 114.007)), module, Maths::OR_OUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(51.018, 114.007)), module, Maths::SUM_OUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(62.309, 114.007)), module, Maths::INV_OUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(82.293, 114.007)), module, Maths::CH_4_UNITY_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(94.402, 114.007)), module, Maths::EOC_4_OUTPUT));

		addChild(createLightCentered<LargeLight<RedLight>>(mm2px(Vec(13.543, 23.613)), module, Maths::CYCLE_1_LED_LIGHT));
		addChild(createLightCentered<LargeLight<RedLight>>(mm2px(Vec(88.599, 23.613)), module, Maths::CYCLE_4_LED_LIGHT));
		addChild(createLightCentered<LargeLight<YellowLight>>(mm2px(Vec(13.818, 105.495)), module, Maths::EOR_CH_1_LIGHT));
		addChild(createLightCentered<LargeLight<GreenLight>>(mm2px(Vec(25.282, 105.495)), module, Maths::LIGHT_UNITY_1_LIGHT));
		addChild(createLightCentered<LargeLight<GreenLight>>(mm2px(Vec(77.122, 105.495)), module, Maths::LIGHT_UNITY_4_LIGHT));
		addChild(createLightCentered<LargeLight<YellowLight>>(mm2px(Vec(88.371, 105.495)), module, Maths::EOC_CH_4_LIGHT));
		addChild(createLightCentered<LargeLight<RedLight>>(mm2px(Vec(30.744, 114.103)), module, Maths::OR_LED_LIGHT));
		addChild(createLightCentered<LargeLight<GreenLight>>(mm2px(Vec(70.907, 114.109)), module, Maths::INV_LED_LIGHT));
	}

	void appendContextMenu(Menu* menu) override {
		Maths* maths = dynamic_cast<Maths*>(module);
		assert(menu);

		menu->addChild(new MenuSeparator());
		menu->addChild(createMenuLabel("Mix Modeling"));
		if (maths) {
			menu->addChild(createBoolPtrMenuItem("Analog Mix Non-Idealities", "", &maths->mixCal.enabled));
		}
	}
};

Model* modelMaths = createModel<Maths, MathsWidget>("Maths");
