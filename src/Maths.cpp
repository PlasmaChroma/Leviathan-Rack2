#include "plugin.hpp"


struct Maths : Module {
	enum ParamId {
		CYCLE_1_PARAM,
		RISE_1_PARAM,
		RISE_4_PARAM,
		FALL_1_PARAM,
		FALL_4_PARAM,
		LIN_LOG_1_PARAM,
		LIN_LOG_4_PARAM,
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

	Maths() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(CYCLE_1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(RISE_1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(RISE_4_PARAM, 0.f, 1.f, 0.f, "");
		configParam(FALL_1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(FALL_4_PARAM, 0.f, 1.f, 0.f, "");
		configParam(LIN_LOG_1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(LIN_LOG_4_PARAM, 0.f, 1.f, 0.f, "");
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