#include "plugin.hpp"


struct TemporalDeck : Module {
	enum ParamId {
		BUFFER_PARAM_PARAM,
		RATE_PARAM_PARAM,
		MIX_PARAM_PARAM,
		FEEDBACK_PARAM_PARAM,
		FREEZE_PARAM_PARAM,
		REVERSE_PARAM_PARAM,
		SLIP_PARAM_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		POSITION_CV_INPUT,
		RATE_CV_INPUT,
		INPUT_L_INPUT,
		INPUT_R_INPUT,
		SCRATCH_GATE_INPUT,
		FREEZE_GATE_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUTPUT_L_OUTPUT,
		OUTPUT_R_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		FREEZE_LIGHT_LIGHT,
		REVERSE_LIGHT_LIGHT,
		SLIP_LIGHT_LIGHT,
		LIGHTS_LEN
	};

	TemporalDeck() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(BUFFER_PARAM_PARAM, 0.f, 1.f, 0.f, "");
		configParam(RATE_PARAM_PARAM, 0.f, 1.f, 0.f, "");
		configParam(MIX_PARAM_PARAM, 0.f, 1.f, 0.f, "");
		configParam(FEEDBACK_PARAM_PARAM, 0.f, 1.f, 0.f, "");
		configParam(FREEZE_PARAM_PARAM, 0.f, 1.f, 0.f, "");
		configParam(REVERSE_PARAM_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SLIP_PARAM_PARAM, 0.f, 1.f, 0.f, "");
		configInput(POSITION_CV_INPUT, "");
		configInput(RATE_CV_INPUT, "");
		configInput(INPUT_L_INPUT, "");
		configInput(INPUT_R_INPUT, "");
		configInput(SCRATCH_GATE_INPUT, "");
		configInput(FREEZE_GATE_INPUT, "");
		configOutput(OUTPUT_L_OUTPUT, "");
		configOutput(OUTPUT_R_OUTPUT, "");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct TemporalDeckWidget : ModuleWidget {
	TemporalDeckWidget(TemporalDeck* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/deck.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(18.0, 18.0)), module, TemporalDeck::BUFFER_PARAM_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(40.0, 18.0)), module, TemporalDeck::RATE_PARAM_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(62.0, 18.0)), module, TemporalDeck::MIX_PARAM_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(84.0, 18.0)), module, TemporalDeck::FEEDBACK_PARAM_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(32.0, 100.0)), module, TemporalDeck::FREEZE_PARAM_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(50.8, 100.0)), module, TemporalDeck::REVERSE_PARAM_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(69.0, 100.0)), module, TemporalDeck::SLIP_PARAM_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.0, 72.0)), module, TemporalDeck::POSITION_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(89.0, 72.0)), module, TemporalDeck::RATE_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(14.0, 118.0)), module, TemporalDeck::INPUT_L_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(30.0, 118.0)), module, TemporalDeck::INPUT_R_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(46.0, 118.0)), module, TemporalDeck::SCRATCH_GATE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(62.0, 118.0)), module, TemporalDeck::FREEZE_GATE_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(78.0, 118.0)), module, TemporalDeck::OUTPUT_L_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(94.0, 118.0)), module, TemporalDeck::OUTPUT_R_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(32.0, 94.2)), module, TemporalDeck::FREEZE_LIGHT_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(50.8, 94.2)), module, TemporalDeck::REVERSE_LIGHT_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(69.0, 94.2)), module, TemporalDeck::SLIP_LIGHT_LIGHT));
	}
};


Model* modelTemporalDeck = createModel<TemporalDeck, TemporalDeckWidget>("TemporalDeck");