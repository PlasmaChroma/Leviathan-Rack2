#include "Chronomaw.hpp"
#include "PanelSvgUtils.hpp"

namespace {

struct ChronomawActionButton : TL1105 {};

} // namespace

ChronomawWidget::ChronomawWidget(Chronomaw* module) {
	setModule(module);
	const std::string panelPath = asset::plugin(pluginInstance, "res/chronomaw.svg");
	setPanel(createPanel(panelPath));

	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2.f * RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2.f * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

	Vec runPos(9.0f, 14.0f);
	Vec bpmPos(21.0f, 14.0f);
	Vec activeBankPos(33.0f, 14.0f);
	Vec loadBankPos(9.0f, 24.0f);
	Vec saveBankPos(15.0f, 24.0f);
	Vec resetAllPos(21.0f, 24.0f);
	Vec selectedOutputPos(27.0f, 24.0f);
	Vec densityModePos(33.0f, 24.0f);
	Vec clkInPos(9.0f, 39.0f);
	Vec runInPos(15.0f, 39.0f);
	Vec resetInPos(21.0f, 39.0f);
	Vec cv1InPos(27.0f, 39.0f);
	Vec cv2InPos(33.0f, 39.0f);
	Vec cv3InPos(27.0f, 49.0f);
	Vec cv4InPos(33.0f, 49.0f);
	Vec runLightPos(21.0f, 39.0f);
	Vec syncLightPos(21.0f, 49.0f);
	std::array<Vec, chronomaw::kNumOutputs> outPos {};
	std::array<Vec, chronomaw::kNumOutputs> outLightPos {};
	for (int i = 0; i < chronomaw::kNumOutputs; ++i) {
		const float y = 56.0f + 8.0f * float(i);
		outPos[size_t(i)] = Vec(9.0f, y);
		outLightPos[size_t(i)] = Vec(15.0f, y);
	}

	auto applyPointOverride = [&](const char* elementId, Vec* outPosMm) {
		Vec pointMm;
		if (panel_svg::loadPointFromSvgMm(panelPath, elementId, &pointMm)) {
			*outPosMm = pointMm;
		}
	};

	applyPointOverride("RUN", &runPos);
	applyPointOverride("BPM", &bpmPos);
	applyPointOverride("ACTIVE_BANK", &activeBankPos);
	applyPointOverride("LOAD_BANK", &loadBankPos);
	applyPointOverride("SAVE_BANK", &saveBankPos);
	applyPointOverride("RESET_ALL", &resetAllPos);
	applyPointOverride("SELECTED_OUTPUT", &selectedOutputPos);
	applyPointOverride("DENSITY_MODE", &densityModePos);
	applyPointOverride("CLK_INPUT", &clkInPos);
	applyPointOverride("RUN_INPUT", &runInPos);
	applyPointOverride("RESET_INPUT", &resetInPos);
	applyPointOverride("CV_1_INPUT", &cv1InPos);
	applyPointOverride("CV_2_INPUT", &cv2InPos);
	applyPointOverride("CV_3_INPUT", &cv3InPos);
	applyPointOverride("CV_4_INPUT", &cv4InPos);
	applyPointOverride("RUN_LIGHT", &runLightPos);
	applyPointOverride("SYNC_LIGHT", &syncLightPos);
	for (int i = 0; i < chronomaw::kNumOutputs; ++i) {
		const std::string outId = "OUT_" + std::to_string(i + 1) + "_OUTPUT";
		const std::string lightId = "OUT_" + std::to_string(i + 1) + "_LIGHT";
		applyPointOverride(outId.c_str(), &outPos[size_t(i)]);
		applyPointOverride(lightId.c_str(), &outLightPos[size_t(i)]);
	}

	addParam(createParamCentered<CKD6>(mm2px(runPos), module, Chronomaw::RUN_PARAM));
	addParam(createParamCentered<BefacoTinyKnobWhite>(mm2px(bpmPos), module, Chronomaw::BPM_PARAM));
	addParam(createParamCentered<BefacoTinyKnobWhite>(mm2px(activeBankPos), module, Chronomaw::ACTIVE_BANK_PARAM));
	addParam(createParamCentered<ChronomawActionButton>(mm2px(loadBankPos), module, Chronomaw::LOAD_BANK_PARAM));
	addParam(createParamCentered<ChronomawActionButton>(mm2px(saveBankPos), module, Chronomaw::SAVE_BANK_PARAM));
	addParam(createParamCentered<ChronomawActionButton>(mm2px(resetAllPos), module, Chronomaw::RESET_ALL_PARAM));
	addParam(createParamCentered<BefacoTinyKnobWhite>(mm2px(selectedOutputPos), module, Chronomaw::SELECTED_OUTPUT_PARAM));
	addParam(createParamCentered<BefacoTinyKnobWhite>(mm2px(densityModePos), module, Chronomaw::DENSITY_MODE_PARAM));

	addInput(createInputCentered<PJ301MPort>(mm2px(clkInPos), module, Chronomaw::CLK_INPUT));
	addInput(createInputCentered<PJ301MPort>(mm2px(runInPos), module, Chronomaw::RUN_INPUT));
	addInput(createInputCentered<PJ301MPort>(mm2px(resetInPos), module, Chronomaw::RESET_INPUT));
	addInput(createInputCentered<PJ301MPort>(mm2px(cv1InPos), module, Chronomaw::CV_1_INPUT));
	addInput(createInputCentered<PJ301MPort>(mm2px(cv2InPos), module, Chronomaw::CV_2_INPUT));
	addInput(createInputCentered<PJ301MPort>(mm2px(cv3InPos), module, Chronomaw::CV_3_INPUT));
	addInput(createInputCentered<PJ301MPort>(mm2px(cv4InPos), module, Chronomaw::CV_4_INPUT));

	for (int i = 0; i < chronomaw::kNumOutputs; ++i) {
		addOutput(createOutputCentered<PJ301MPort>(mm2px(outPos[size_t(i)]), module, Chronomaw::OUT_1_OUTPUT + i));
		addChild(createLightCentered<SmallLight<BlueLight>>(mm2px(outLightPos[size_t(i)]), module, Chronomaw::OUT_1_LIGHT + i));
	}
	addChild(createLightCentered<SmallLight<GreenLight>>(mm2px(runLightPos), module, Chronomaw::RUN_LIGHT));
	addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(syncLightPos), module, Chronomaw::SYNC_LIGHT));
}

Model* modelChronomaw = createModel<Chronomaw, ChronomawWidget>("Chronomaw");
