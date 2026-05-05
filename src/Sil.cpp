#include "plugin.hpp"
#include "PanelSvgUtils.hpp"

struct Sil : Module {
	enum ParamId {
		PARAMS_LEN
	};
	enum InputId {
		INPUT_L_INPUT,
		INPUT_R_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUTPUT_L_OUTPUT,
		OUTPUT_R_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	Sil() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configInput(INPUT_L_INPUT, "Left");
		configInput(INPUT_R_INPUT, "Right");
		configOutput(OUTPUT_L_OUTPUT, "Left");
		configOutput(OUTPUT_R_OUTPUT, "Right");
	}

	void process(const ProcessArgs& args) override {
		(void) args;
		const float inL = inputs[INPUT_L_INPUT].getVoltage();
		const float inR = inputs[INPUT_R_INPUT].getVoltage();
		outputs[OUTPUT_L_OUTPUT].setChannels(1);
		outputs[OUTPUT_R_OUTPUT].setChannels(1);
		outputs[OUTPUT_L_OUTPUT].setVoltage(inL);
		outputs[OUTPUT_R_OUTPUT].setVoltage(inR);
	}
};

struct BananutBlack : app::SvgPort {
	BananutBlack() {
		setSvg(Svg::load(asset::plugin(pluginInstance, "res/BananutBlack.svg")));
	}
};

struct SilWidget : ModuleWidget {
	SilWidget(Sil* module) {
		setModule(module);
		const std::string panelPath = asset::plugin(pluginInstance, "res/sil.svg");
		setPanel(createPanel(asset::plugin(pluginInstance, "res/sil.svg")));

		Vec inputLPos(26.f, 118.f);
		Vec inputRPos(42.f, 118.f);
		Vec outputLPos(58.f, 118.f);
		Vec outputRPos(74.f, 118.f);

		auto applyPointOverride = [&](const char* elementId, Vec* outPos) {
			Vec pointMm;
			if (panel_svg::loadPointFromSvgMm(panelPath, elementId, &pointMm)) {
				*outPos = pointMm;
			}
		};

		applyPointOverride("INPUT_L_INPUT", &inputLPos);
		applyPointOverride("INPUT_R_INPUT", &inputRPos);
		applyPointOverride("OUTPUT_L_OUTPUT", &outputLPos);
		applyPointOverride("OUTPUT_R_OUTPUT", &outputRPos);

		addInput(createInputCentered<PJ301MPort>(mm2px(inputLPos), module, Sil::INPUT_L_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(inputRPos), module, Sil::INPUT_R_INPUT));
		addOutput(createOutputCentered<BananutBlack>(mm2px(outputLPos), module, Sil::OUTPUT_L_OUTPUT));
		addOutput(createOutputCentered<BananutBlack>(mm2px(outputRPos), module, Sil::OUTPUT_R_OUTPUT));
	}
};

Model* modelSil = createModel<Sil, SilWidget>("Sil");
