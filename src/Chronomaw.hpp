#pragma once

#include "ChronomawEngine.hpp"
#include "plugin.hpp"

struct Chronomaw : Module {
	enum ParamId {
		RUN_PARAM,
		BPM_PARAM,
		ACTIVE_BANK_PARAM,
		LOAD_BANK_PARAM,
		SAVE_BANK_PARAM,
		RESET_ALL_PARAM,
		SELECTED_OUTPUT_PARAM,
		DENSITY_MODE_PARAM,
		PARAMS_LEN
	};

	enum InputId {
		CLK_INPUT,
		RUN_INPUT,
		RESET_INPUT,
		CV_1_INPUT,
		CV_2_INPUT,
		CV_3_INPUT,
		CV_4_INPUT,
		INPUTS_LEN
	};

	enum OutputId {
		OUT_1_OUTPUT,
		OUT_2_OUTPUT,
		OUT_3_OUTPUT,
		OUT_4_OUTPUT,
		OUT_5_OUTPUT,
		OUT_6_OUTPUT,
		OUT_7_OUTPUT,
		OUT_8_OUTPUT,
		OUTPUTS_LEN
	};

	enum LightId {
		RUN_LIGHT,
		SYNC_LIGHT,
		OUT_1_LIGHT,
		OUT_2_LIGHT,
		OUT_3_LIGHT,
		OUT_4_LIGHT,
		OUT_5_LIGHT,
		OUT_6_LIGHT,
		OUT_7_LIGHT,
		OUT_8_LIGHT,
		LIGHTS_LEN
	};

	chronomaw::ModuleState state;
	chronomaw::Engine engine;
	chronomaw::FrameOutputs frameOut;
	dsp::SchmittTrigger runButtonEdge;
	dsp::SchmittTrigger loadBankEdge;
	dsp::SchmittTrigger saveBankEdge;
	dsp::SchmittTrigger resetAllEdge;

	Chronomaw();

	void process(const ProcessArgs& args) override;
	void onReset() override;
	json_t* dataToJson() override;
	void dataFromJson(json_t* rootJ) override;
};

struct ChronomawWidget : ModuleWidget {
	explicit ChronomawWidget(Chronomaw* module);
};

extern Model* modelChronomaw;

