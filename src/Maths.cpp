#include "plugin.hpp"


struct Maths : Module {
	enum ParamId {
        RISE_1_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		INPUT_1_INPUT,
		INPUT_2_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	Maths() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configInput(INPUT_1_INPUT, "");
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

struct MathsWidget : ModuleWidget {
	MathsWidget(Maths* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Maths.svg")));

        MyImageWidget* img = new MyImageWidget();
        img->box.pos = Vec(0, 0);  // Position on the panel
        img->box.size = box.size; // Size of the image
        addChild(img);
/*
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
*/
        addParam(createParamCentered<Rogan1PSBlue>(mm2px(Vec(31.07, 34.723)), module, Maths::RISE_1_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.794, 15.678)), module, Maths::INPUT_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(19.157, 15.659)), module, Maths::INPUT_2_INPUT));


	}
};




Model* modelMaths = createModel<Maths, MathsWidget>("Maths");