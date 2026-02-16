#include "plugin.hpp"


struct Maths : Module {
	enum ParamId {
		PARAMS_LEN
	};
	enum InputId {
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
	}

	void process(const ProcessArgs& args) override {
	}
};


struct MathsWidget : ModuleWidget {
	MathsWidget(Maths* module) {
        // 1. Load the panel first
        auto* panel = createPanel(asset::plugin(pluginInstance, "res/maths-plain.svg"));
        setPanel(panel);

        // 2. Ensure the Widget's box matches the panel size before placing screws
        if (panel) {
            box.size = panel->box.size;
        }

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	}
};


Model* modelMaths = createModel<Maths, MathsWidget>("Maths");