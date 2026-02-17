#include "plugin.hpp"


struct Maths : Module {
	enum ParamId {
		ATTENUATE_1_PARAM,
		CYCLE_1_PARAM,
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
		CH1_RISE_CV_INPUT,
		CH1_BOTH_CV_INPUT,
		CH1_FALL_CV_INPUT,
		CH1_CYCLE_CV_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT_1_OUTPUT,
		OUT_2_OUTPUT,
		OUT_3_OUTPUT,
		OUT_4_OUTPUT,
		EOR_1_OUTPUT,
		CH_1_UNITY_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		CYCLE_1_LED_LIGHT,
		EOR_CH_1_LIGHT,
		LIGHT_UNITY_1_LIGHT,
		LIGHTS_LEN
	};

	enum Ch1Phase {
		CH1_IDLE,
		CH1_RISE,
		CH1_FALL
	};

	dsp::SchmittTrigger ch1TrigEdge;
	dsp::SchmittTrigger ch1CycleButtonEdge;
	dsp::SchmittTrigger ch1CycleCvGate;
	dsp::PulseGenerator ch1EorPulse;

	Ch1Phase ch1Phase = CH1_IDLE;
	float ch1PhasePos = 0.f;
	float ch1Out = 0.f;
	bool ch1CycleLatched = false;
	static constexpr float CH1_LINEAR_SHAPE = 0.33f;

	static float shapeCurve(float x, float shape) {
		x = clamp(x, 0.f, 1.f);
		shape = clamp(shape, 0.f, 1.f);
		if (shape < CH1_LINEAR_SHAPE) {
			// Log-ish: fast start, slow finish.
			float t = shape / CH1_LINEAR_SHAPE;
			float gamma = rescale(t, 0.f, 1.f, 0.35f, 1.f);
			return std::pow(x, gamma);
		}
		if (shape > CH1_LINEAR_SHAPE) {
			// Exp-ish: slow start, fast finish.
			float t = (shape - CH1_LINEAR_SHAPE) / (1.f - CH1_LINEAR_SHAPE);
			float gamma = rescale(t, 0.f, 1.f, 1.f, 3.5f);
			return std::pow(x, gamma);
		}
		return x;
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
		if (shape < CH1_LINEAR_SHAPE) {
			float t = shape / CH1_LINEAR_SHAPE;
			return std::pow(logSlowScale, 1.f - t);
		}
		if (shape > CH1_LINEAR_SHAPE) {
			float t = (shape - CH1_LINEAR_SHAPE) / (1.f - CH1_LINEAR_SHAPE);
			return std::pow(expFastScale, t);
		}
		return 1.f;
	}

	float computeStageTime(float knob, float stageCv, float bothCv, float shape) const {
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
		t *= computeShapeTimeScale(shape, knob);

		return clamp(t, absoluteMinTime, maxTime);
	}

	void triggerCh1Function() {
		ch1Phase = CH1_RISE;
		ch1PhasePos = 0.f;
	}

	Maths() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(ATTENUATE_1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(CYCLE_1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(RISE_1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(RISE_4_PARAM, 0.f, 1.f, 0.f, "");
		configParam(ATTENUATE_2_PARAM, 0.f, 1.f, 0.f, "");
		configParam(FALL_1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(FALL_4_PARAM, 0.f, 1.f, 0.f, "");
		configParam(ATTENUATE_3_PARAM, 0.f, 1.f, 0.f, "");
		configParam(LIN_LOG_1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(LIN_LOG_4_PARAM, 0.f, 1.f, 0.f, "");
		configParam(ATTENUATE_4_PARAM, 0.f, 1.f, 0.f, "");
		configInput(INPUT_1_INPUT, "");
		configInput(INPUT_1_TRIG_INPUT, "");
		configInput(CH1_RISE_CV_INPUT, "");
		configInput(CH1_BOTH_CV_INPUT, "");
		configInput(CH1_FALL_CV_INPUT, "");
		configInput(CH1_CYCLE_CV_INPUT, "");
		configOutput(OUT_1_OUTPUT, "");
		configOutput(OUT_2_OUTPUT, "");
		configOutput(OUT_3_OUTPUT, "");
		configOutput(OUT_4_OUTPUT, "");
		configOutput(EOR_1_OUTPUT, "");
		configOutput(CH_1_UNITY_OUTPUT, "");
	}

	void process(const ProcessArgs& args) override {
		float dt = args.sampleTime;

		if (ch1CycleButtonEdge.process(params[CYCLE_1_PARAM].getValue())) {
			ch1CycleLatched = !ch1CycleLatched;
		}

		bool cycleCvHigh = ch1CycleCvGate.process(rescale(inputs[CH1_CYCLE_CV_INPUT].getVoltage(), 0.1f, 2.5f, 0.f, 1.f));
		bool ch1CycleOn = ch1CycleLatched || cycleCvHigh;

		bool trigRise = ch1TrigEdge.process(inputs[INPUT_1_TRIG_INPUT].getVoltage());
		if (trigRise && ch1Phase != CH1_RISE) {
			triggerCh1Function();
		}

		float shape = params[LIN_LOG_1_PARAM].getValue();
		float riseTime = computeStageTime(
			params[RISE_1_PARAM].getValue(),
			inputs[CH1_RISE_CV_INPUT].getVoltage(),
			inputs[CH1_BOTH_CV_INPUT].getVoltage(),
			shape
		);
		float fallTime = computeStageTime(
			params[FALL_1_PARAM].getValue(),
			inputs[CH1_FALL_CV_INPUT].getVoltage(),
			inputs[CH1_BOTH_CV_INPUT].getVoltage(),
			shape
		);

		bool signalPatched = inputs[INPUT_1_INPUT].isConnected();
		if (ch1Phase == CH1_IDLE && ch1CycleOn) {
			triggerCh1Function();
		}

		if (ch1Phase != CH1_IDLE) {
			float peak = ch1CycleOn ? 8.f : 10.f;
			if (ch1Phase == CH1_RISE) {
				ch1PhasePos += dt / riseTime;
				if (ch1PhasePos >= 1.f) {
					ch1PhasePos = 0.f;
					ch1Phase = CH1_FALL;
					ch1EorPulse.trigger(1e-3f);
				}
				else {
						ch1Out = peak * shapeCurve(ch1PhasePos, shape);
				}
			}

			if (ch1Phase == CH1_FALL) {
				ch1PhasePos += dt / fallTime;
				if (ch1PhasePos >= 1.f) {
					ch1PhasePos = 0.f;
					ch1Phase = CH1_IDLE;
					ch1Out = 0.f;
				}
				else {
						ch1Out = peak * (1.f - shapeCurve(ch1PhasePos, shape));
				}
			}
		}
		else if (signalPatched) {
			// Slew mode: follow signal input with independent rise/fall timing.
			float target = clamp(inputs[INPUT_1_INPUT].getVoltage(), -10.f, 10.f);
			float delta = target - ch1Out;
			float stageTime = (delta >= 0.f) ? riseTime : fallTime;
			float alpha = clamp(dt / stageTime, 0.f, 1.f);
			float alphaGamma = 1.f;
			if (shape < CH1_LINEAR_SHAPE) {
				float t = shape / CH1_LINEAR_SHAPE;
				alphaGamma = rescale(t, 0.f, 1.f, 1.6f, 1.f);
			}
			else if (shape > CH1_LINEAR_SHAPE) {
				float t = (shape - CH1_LINEAR_SHAPE) / (1.f - CH1_LINEAR_SHAPE);
				alphaGamma = rescale(t, 0.f, 1.f, 1.f, 0.7f);
			}
			float shapedAlpha = std::pow(alpha, alphaGamma);
			ch1Out += delta * shapedAlpha;
		}
		else {
			ch1Out = 0.f;
		}

		bool eorHigh = ch1EorPulse.process(dt);
		outputs[EOR_1_OUTPUT].setVoltage(eorHigh ? 10.f : 0.f);

		outputs[CH_1_UNITY_OUTPUT].setVoltage(ch1Out);
		outputs[OUT_1_OUTPUT].setVoltage(ch1Out);
		outputs[OUT_2_OUTPUT].setVoltage(0.f);
		outputs[OUT_3_OUTPUT].setVoltage(0.f);
		outputs[OUT_4_OUTPUT].setVoltage(0.f);

		lights[CYCLE_1_LED_LIGHT].setBrightness(ch1CycleOn ? 1.f : 0.f);
		lights[EOR_CH_1_LIGHT].setBrightness(eorHigh ? 1.f : 0.f);
		lights[LIGHT_UNITY_1_LIGHT].setBrightness(clamp(std::fabs(ch1Out) / 10.f, 0.f, 1.f));
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
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Maths.svg")));

        MyImageWidget* img = new MyImageWidget();
        img->box.pos = Vec(0, 0);  // Position on the panel
        img->box.size = box.size; // Size of the image
        
        addParam(createParamCentered<TL1105>(mm2px(Vec(10.349, 32.315)), module, Maths::CYCLE_1_PARAM));

        addChild(img);
        
        // use Rogan1PSBlue for the rise/fall knobs
		
		addParam(createParamCentered<Rogan1PSBlue>(mm2px(Vec(30.873, 34.229)), module, Maths::RISE_1_PARAM));
		addParam(createParamCentered<Rogan1PSBlue>(mm2px(Vec(72.32, 34.184)), module, Maths::RISE_4_PARAM));
		addParam(createParamCentered<Rogan1PSBlue>(mm2px(Vec(30.769, 56.223)), module, Maths::FALL_1_PARAM));
		addParam(createParamCentered<Rogan1PSBlue>(mm2px(Vec(72.217, 56.178)), module, Maths::FALL_4_PARAM));
		addParam(createParamCentered<Rogan1PSBlue>(mm2px(Vec(30.719, 80.782)), module, Maths::LIN_LOG_1_PARAM));
		addParam(createParamCentered<Rogan1PSBlue>(mm2px(Vec(72.166, 80.737)), module, Maths::LIN_LOG_4_PARAM));

        addParam(createParamCentered<Rogan1PSWhite>(mm2px(Vec(51.219, 23.24)), module, Maths::ATTENUATE_1_PARAM));
        addParam(createParamCentered<Rogan1PSWhite>(mm2px(Vec(51.199, 43.388)), module, Maths::ATTENUATE_2_PARAM));
        addParam(createParamCentered<Rogan1PSWhite>(mm2px(Vec(50.846, 63.103)), module, Maths::ATTENUATE_3_PARAM));
        addParam(createParamCentered<Rogan1PSWhite>(mm2px(Vec(51.044, 82.96)), module, Maths::ATTENUATE_4_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.794, 15.678)), module, Maths::INPUT_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(19.157, 15.659)), module, Maths::INPUT_1_TRIG_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.628, 50.821)), module, Maths::CH1_RISE_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.202, 62.311)), module, Maths::CH1_BOTH_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.575, 74.408)), module, Maths::CH1_FALL_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.199, 86.4)), module, Maths::CH1_CYCLE_CV_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(33.268, 101.985)), module, Maths::OUT_1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(45.246, 101.977)), module, Maths::OUT_2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(56.768, 101.944)), module, Maths::OUT_3_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(68.799, 102.011)), module, Maths::OUT_4_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.722, 114.057)), module, Maths::EOR_1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(19.774, 114.067)), module, Maths::CH_1_UNITY_OUTPUT));

		addChild(createLightCentered<LargeLight<RedLight>>(mm2px(Vec(13.74, 23.811)), module, Maths::CYCLE_1_LED_LIGHT));
		addChild(createLightCentered<LargeLight<RedLight>>(mm2px(Vec(14.016, 105.991)), module, Maths::EOR_CH_1_LIGHT));
		addChild(createLightCentered<LargeLight<RedLight>>(mm2px(Vec(25.676, 105.976)), module, Maths::LIGHT_UNITY_1_LIGHT));
	}
};




Model* modelMaths = createModel<Maths, MathsWidget>("Maths");
