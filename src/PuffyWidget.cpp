#include "PuffyWidget.hpp"

#include "PanelSvgUtils.hpp"
#include "PuffyFishWidget.hpp"
#include "visual/ApertureLight.hpp"
#include "visual/VisualAssets.hpp"

namespace {

bool loadPoint(
	const std::string& panelPath,
	const char* id,
	Vec fallbackMm,
	Vec* pointMm) {
	if (panel_svg::loadPointFromSvgMm(panelPath, id, pointMm)) {
		return true;
	}
	*pointMm = fallbackMm;
	return false;
}

} // namespace

PuffyWidget::PuffyWidget(Puffy* module) {
	setModule(module);
	PreviewBuildLogTimer previewTimer("Puffy", module);
	const std::string panelPath =
		asset::plugin(pluginInstance, "res/Puffy.svg");
	setPanel(createPanel(panelPath));
	previewTimer.markPanelDone();
	previewTimer.setAtlasStatus(
		panel_svg::getAtlasStatusLabelForSvg(panelPath));

	auto anchor = [&](const char* id, Vec fallbackMm) {
		Vec point;
		loadPoint(panelPath, id, fallbackMm, &point);
		return mm2px(point);
	};

	math::Rect fishRectMm;
	if (!panel_svg::loadRectFromSvgMm(
			panelPath, "fish_rect", &fishRectMm)) {
		fishRectMm.pos = Vec(4.f, 20.f);
		fishRectMm.size = Vec(52.96f, 48.f);
	}
	auto* fish = new PuffyFishWidget(module);
	fish->box.pos = mm2px(fishRectMm.pos);
	fish->box.size = mm2px(fishRectMm.size);
	addChild(fish);

	addParam(createParamCentered<DarkTinyClockworkGearKnob>(
		anchor("character_param", Vec(30.48f, 14.f)),
		module, Puffy::CHARACTER_PARAM));
	addParam(createParamCentered<ClockworkGearKnob>(
		anchor("puff_param", Vec(18.f, 83.5f)),
		module, Puffy::PUFF_PARAM));
	addParam(createParamCentered<DarkTinyClockworkGearKnob>(
		anchor("deflate_param", Vec(44.f, 83.5f)),
		module, Puffy::DEFLATE_PARAM));
	addParam(createParamCentered<BipolarDarkTinyClockworkGearKnob>(
		anchor("puff_cv_amount_param", Vec(30.48f, 96.f)),
		module, Puffy::PUFF_CV_AMOUNT_PARAM));

	addInput(createInputCentered<Magitek2InputJack>(
		anchor("input_l", Vec(9.f, 108.f)),
		module, Puffy::INPUT_L));
	addInput(createInputCentered<Magitek2InputJack>(
		anchor("input_r", Vec(25.f, 108.f)),
		module, Puffy::INPUT_R));
	addInput(createInputCentered<Magitek2InputJack>(
		anchor("puff_cv_input", Vec(43.f, 108.f)),
		module, Puffy::PUFF_CV_INPUT));
	addOutput(createOutputCentered<Magitek2OutputJack>(
		anchor("output_l", Vec(18.f, 121.f)),
		module, Puffy::OUTPUT_L));
	addOutput(createOutputCentered<Magitek2OutputJack>(
		anchor("output_r", Vec(43.f, 121.f)),
		module, Puffy::OUTPUT_R));

	addChild(createLightCentered<SmallAperture<RedApertureLight>>(
		anchor("limit_light", Vec(54.f, 72.5f)),
		module, Puffy::LIMIT_LIGHT));

	addChild(createWidgetCentered<ScrewSilver>(
		anchor("screw_tl", Vec(3.f, 3.f))));
	addChild(createWidgetCentered<ScrewSilver>(
		anchor("screw_tr", Vec(57.96f, 3.f))));
	addChild(createWidgetCentered<ScrewSilver>(
		anchor("screw_bl", Vec(3.f, 125.5f))));
	addChild(createWidgetCentered<ScrewSilver>(
		anchor("screw_br", Vec(57.96f, 125.5f))));
	previewTimer.markAnchorsDone();
}

Model* modelPuffy = createModel<Puffy, PuffyWidget>("Puffy");
