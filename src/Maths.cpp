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
		dsp::SchmittTrigger cycleCvGate;

		OuterPhase phase = OUTER_IDLE;
		float phasePos = 0.f;
		float out = 0.f;
		bool cycleLatched = false;
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
	};

	struct OuterChannelResult {
		bool cycleOn = false;
	};

	OuterChannelState ch1;
	OuterChannelState ch4;
	static constexpr float LINEAR_SHAPE = 0.33f;

	static float attenuverterGain(float knob01) {
		// Noon = 0, CCW = negative, CW = positive.
		return clamp(knob01, 0.f, 1.f) * 2.f - 1.f;
	}

		static float shapeCurve(float x, float shape) {
		x = clamp(x, 0.f, 1.f);
		shape = clamp(shape, 0.f, 1.f);
			if (shape < LINEAR_SHAPE) {
				// Log-ish: fast start, slow finish.
				float t = shape / LINEAR_SHAPE;
				float gamma = rescale(t, 0.f, 1.f, 0.35f, 1.f);
				return std::pow(x, gamma);
			}
			if (shape > LINEAR_SHAPE) {
				// Exp-ish: slow start, fast finish.
				float t = (shape - LINEAR_SHAPE) / (1.f - LINEAR_SHAPE);
				float gamma = rescale(t, 0.f, 1.f, 1.f, 3.5f);
				return std::pow(x, gamma);
			}
			return x;
		}

		static float processRampageSlew(
			float out,
			float in,
			float riseKnob01,
			float fallKnob01,
			float shape01,
			float riseCvV,
			float fallCvV,
			float bothCvV,
			float dt
		) {
			static constexpr float MIN_TIME = 0.01f;
			static constexpr float VREF = 10.0f;
			static constexpr float E = 2.718281828f;

			float delta = in - out;
			if (delta == 0.f) {
				return out;
			}

			float stageKnob = (delta > 0.f) ? riseKnob01 : fallKnob01;
			float stageCv = (delta > 0.f) ? riseCvV : fallCvV;

			float baseExp = 10.f * stageKnob;
			float stageExp = clamp(stageCv, 0.f, 10.f);
			float bothExp = -clamp(bothCvV, -8.f, 8.f) / 2.f;
			float rateExp = clamp(baseExp + stageExp + bothExp, 0.f, 10.f);
			float tau = MIN_TIME * std::pow(2.f, rateExp);

			float absDelta = std::fabs(delta);
			float sgn = (delta >= 0.f) ? 1.f : -1.f;
			float lin = sgn * (VREF / tau);
			float logv = sgn * (4.f * VREF) / tau / (absDelta + 1.f);
			float expv = (E * delta) / tau;

			float shapeSigned = clamp(shape01 * 2.f - 1.f, -1.f, 1.f);
			float dVdt = lin;
			if (shapeSigned < 0.f) {
				float mix = clamp((-shapeSigned) * 0.95f, 0.f, 1.f);
				dVdt = lin + (logv - lin) * mix;
			}
			else if (shapeSigned > 0.f) {
				float mix = clamp(shapeSigned * 0.90f, 0.f, 1.f);
				dVdt = lin + (expv - lin) * mix;
			}

			float prevOut = out;
			out += dVdt * dt;
			if ((in - prevOut) * (in - out) < 0.f) {
				out = in;
			}
			return out;
		}

		float computeShapeTimeScale(float shape, float knob) const {
		shape = clamp(shape, 0.f, 1.f);
		// Calibrated from measured FUNCTION sweeps:
		// Rise/Fall = 0.00 -> LOG ~180Hz, EXP ~1147Hz (linear baseline ~666Hz).
		// Rise/Fall = 0.10 -> LOG ~123Hz, EXP ~1083Hz.
		// Interpolate in log-domain across knob [0, 0.10], then hold.
		float knobBlend = clamp(knob / 0.10f, 0.f, 1.f);
		float logSlowScale = std::exp(
			std::log(3.7f) + (std::log(1.27f) - std::log(3.7f)) * knobBlend
		);
		float expFastScale = std::exp(
			std::log(0.5806f) + (std::log(0.144f) - std::log(0.5806f)) * knobBlend
		);
			if (shape < LINEAR_SHAPE) {
				float t = shape / LINEAR_SHAPE;
				return std::pow(logSlowScale, 1.f - t);
			}
			if (shape > LINEAR_SHAPE) {
				float t = (shape - LINEAR_SHAPE) / (1.f - LINEAR_SHAPE);
				return std::pow(expFastScale, t);
			}
			return 1.f;
		}

		float computeStageTime(float knob, float stageCv, float bothCv, float shape, bool applyShapeTimeScale) const {
		// Baseline at knob minimum (linear shape) calibrated near ~666Hz cycle.
		const float minTime = 0.00075075f;
		// Absolute floor allows EXP/positive CV to run faster than the linear baseline.
		const float absoluteMinTime = 0.0001f;
		const float maxTime = 1500.f;
		float t = minTime * std::pow(maxTime / minTime, clamp(knob, 0.f, 1.f));

		// Rise/Fall CV is linear over +/-8V.
		float linearScale = 1.f + clamp(stageCv, -8.f, 8.f) / 8.f;
		linearScale = std::max(linearScale, 0.05f);
		t *= linearScale;

		// Both CV is bipolar exponential, positive = faster, negative = slower.
		float bothScale = std::pow(2.f, -clamp(bothCv, -8.f, 8.f) / 2.f);
		t *= bothScale;
		if (applyShapeTimeScale) {
			t *= computeShapeTimeScale(shape, knob);
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

		bool cycleCvHigh = ch.cycleCvGate.process(rescale(inputs[cfg.cycleCvInput].getVoltage(), 0.1f, 2.5f, 0.f, 1.f));
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
			true
		);
		float fallTime = computeStageTime(
			params[cfg.fallParam].getValue(),
			inputs[cfg.fallCvInput].getVoltage(),
			inputs[cfg.bothCvInput].getVoltage(),
			shape,
			true
		);

		bool signalPatched = inputs[cfg.signalInput].isConnected();
		if (ch.phase == OUTER_IDLE && cycleOn) {
			triggerOuterFunction(ch);
		}

		if (ch.phase != OUTER_IDLE) {
			float peak = cycleOn ? 8.f : 10.f;
			if (ch.phase == OUTER_RISE) {
				ch.phasePos += dt / riseTime;
				if (ch.phasePos >= 1.f) {
					ch.phasePos = 0.f;
					ch.phase = OUTER_FALL;
					ch.out = peak;
				}
				else {
					ch.out = peak * shapeCurve(ch.phasePos, shape);
				}
			}

			if (ch.phase == OUTER_FALL) {
				ch.phasePos += dt / fallTime;
				if (ch.phasePos >= 1.f) {
					ch.phasePos = 0.f;
					ch.phase = OUTER_IDLE;
					ch.out = 0.f;
				}
				else {
					ch.out = peak * (1.f - shapeCurve(ch.phasePos, shape));
				}
			}
		}
		else if (signalPatched) {
			// Rampage-style shaped slew for external input.
			float in = inputs[cfg.signalInput].getVoltage();
			float riseKnob01 = params[cfg.riseParam].getValue();
			float fallKnob01 = params[cfg.fallParam].getValue();
			float shape01 = params[cfg.shapeParam].getValue();
			float riseCvV = inputs[cfg.riseCvInput].getVoltage();
			float fallCvV = inputs[cfg.fallCvInput].getVoltage();
			float bothCvV = inputs[cfg.bothCvInput].getVoltage();
			ch.out = processRampageSlew(
				ch.out,
				in,
				riseKnob01,
				fallKnob01,
				shape01,
				riseCvV,
				fallCvV,
				bothCvV,
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
			CH1_CYCLE_CV_INPUT
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
			CH4_CYCLE_CV_INPUT
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
		float sumOut = clamp(busV1 + busV2 + busV3 + busV4, -10.f, 10.f);
		float invOut = clamp(-sumOut, -10.f, 10.f);
		float orOut = clamp(std::fmax(0.f, std::fmax(std::fmax(busV1, busV2), std::fmax(busV3, busV4))), 0.f, 10.f);

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
		lights[LIGHT_UNITY_1_LIGHT].setBrightness(clamp(std::fabs(ch1.out) / 10.f, 0.f, 1.f));
		lights[LIGHT_UNITY_4_LIGHT].setBrightness(clamp(std::fabs(ch4.out) / 10.f, 0.f, 1.f));
		lights[OR_LED_LIGHT].setBrightness(clamp(orOut / 10.f, 0.f, 1.f));
		lights[INV_LED_LIGHT].setBrightness(clamp(std::fabs(invOut) / 10.f, 0.f, 1.f));
	}
};

int imageHandle = -1;
struct MyImageWidget : Widget {
    void draw(const DrawArgs& args) override {
        // 1. Get the path to your image in the plugin's "res" folder
        if (imageHandle < 0) {
            std::string path = asset::plugin(pluginInstance, "res/maths2.jpg");
            // 2. Create the image handle using NanoVG
            imageHandle = nvgCreateImage(args.vg, path.c_str(), 0);
        }

        if (imageHandle >= 0) {
            // 3. Define where and how big the image should be
            NVGpaint imgPaint = nvgImagePattern(args.vg, 0, 0, box.size.x, box.size.y, 0, imageHandle, 1.0f);
            
            // 4. Draw a rectangle and fill it with the image pattern
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
        box.size = mm2px(Vec(14.0, 14.0));  // default TL1105 is ~8mm
    }
};

struct MathsWidget : ModuleWidget {
	MathsWidget(Maths* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/maths.svg")));

        MyImageWidget* img = new MyImageWidget();
        img->box.pos = Vec(0, 0);  // Position on the panel
        img->box.size = box.size; // Size of the image
        
		addParam(createParamCentered<TL1105>(mm2px(Vec(10.349, 32.315)), module, Maths::CYCLE_1_PARAM));
		addParam(createParamCentered<TL1105>(mm2px(Vec(92.313, 32.315)), module, Maths::CYCLE_4_PARAM));

        addChild(img);
        
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
};

Model* modelMaths = createModel<Maths, MathsWidget>("Maths");
