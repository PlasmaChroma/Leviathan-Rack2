#include "PuffyWidget.hpp"

#include "PanelSvgUtils.hpp"
#include "PuffyFishWidget.hpp"
#include "visual/ApertureLight.hpp"
#include "visual/VisualAssets.hpp"

namespace {

struct PuffyViewportGradient final : TransparentWidget {
	void draw(const DrawArgs& args) override {
		const float inset = mm2px(0.16f);
		const float width = std::max(0.f, box.size.x - 2.f * inset);
		const float height = std::max(0.f, box.size.y - 2.f * inset);
		const float radius = std::max(0.f, mm2px(3.f) - inset);
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, inset, inset, width, height, radius);
		nvgFillPaint(args.vg, nvgLinearGradient(
			args.vg,
			inset,
			inset,
			inset + width,
			inset + height,
			nvgRGB(31, 25, 44),
			nvgRGB(15, 13, 23)));
		nvgFill(args.vg);
	}
};

struct PuffyCharacterReadout final : TransparentWidget {
	static constexpr float FONT_SIZE = 11.5f;
	Puffy* module = nullptr;

	explicit PuffyCharacterReadout(Puffy* module) : module(module) {
	}

	void draw(const DrawArgs& args) override {
		if (!APP || !APP->window || !APP->window->uiFont) {
			return;
		}
		const int character = module
			? clamp(
				int(std::lround(
					module->params[Puffy::CHARACTER_PARAM].getValue())),
				0, 3)
			: int(puffy::Character::Bloom);
		static const char* const labels[] = {
			"BLOOM",
			"SPINE",
			"FRENZY",
			"RIPTIDE"
		};
		nvgFontSize(args.vg, FONT_SIZE);
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 255));
		nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		nvgText(
			args.vg,
			0.5f * box.size.x,
			0.5f * box.size.y,
			labels[character],
			nullptr);
	}
};

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

void PuffyWidget::step() {
	const bool measurePerf = isDragonKingDebugEnabled();
	const auto stepStart = debug_terminal::debugTimerStart(measurePerf);
	ModuleWidget::step();
	if (measurePerf) {
		debugWidgetMetrics.recordStep(
			debug_terminal::elapsedUsSince(stepStart));
	}
}

void PuffyWidget::draw(const DrawArgs& args) {
	const bool measurePerf = isDragonKingDebugEnabled();
	const auto drawStart = debug_terminal::debugTimerStart(measurePerf);
	ModuleWidget::draw(args);
	auto* puffyModule = static_cast<Puffy*>(module);
	if (!puffyModule) {
		return;
	}
	if (measurePerf) {
		debug_terminal::drawDebugInstanceId(
			args.vg, box.size, puffyModule->debugMetrics.instanceId);
		debugWidgetMetrics.recordDraw(
			debug_terminal::elapsedUsSince(drawStart));
		const double nowSec = system::getTime();
		if (debug_terminal::baselineSubmitDue(
				"Puffy", puffyModule->debugMetrics.instanceId, nowSec)) {
			debug_terminal::submitBaselineMetrics(
				"Puffy",
				puffyModule->debugMetrics.instanceId,
				puffyModule->debugMetrics.consumeProcessRange(),
				debugWidgetMetrics.consumeStepRange(),
				debugWidgetMetrics.consumeDrawRange());
		}
	}
}

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
	auto* viewportFramebuffer = new widget::FramebufferWidget();
	viewportFramebuffer->box.pos = mm2px(fishRectMm.pos);
	viewportFramebuffer->box.size = mm2px(fishRectMm.size);
	viewportFramebuffer->dirtyOnSubpixelChange = false;
	auto* viewportGradient = new PuffyViewportGradient();
	viewportGradient->box.size = viewportFramebuffer->box.size;
	viewportFramebuffer->addChild(viewportGradient);
	addChild(viewportFramebuffer);

	auto* fish = new PuffyFishWidget(module);
	fish->box.pos = mm2px(fishRectMm.pos);
	fish->box.size = mm2px(fishRectMm.size);
	addChild(fish);

	auto* characterButton = createParamCentered<SmallGoldButton>(
		anchor("character_param", Vec(30.48f, 14.f)),
		module, Puffy::CHARACTER_PARAM);
	characterButton->momentary = false;
	addParam(characterButton);
	auto* characterReadout = new PuffyCharacterReadout(module);
	characterReadout->box.size = mm2px(Vec(20.f, 5.f));
	characterReadout->box.pos =
		anchor("character_readout", Vec(44.5f, 14.f))
			.minus(characterReadout->box.size.mult(0.5f));
	addChild(characterReadout);
	addParam(createParamCentered<LeviathanHaloKnob2>(
		anchor("puff_param", Vec(18.f, 83.5f)),
		module, Puffy::PUFF_PARAM));
	addParam(createParamCentered<Eclipse2Knob>(
		anchor("deflate_param", Vec(44.f, 83.5f)),
		module, Puffy::DEFLATE_PARAM));
	auto* puffCvAmountKnob = createParamCentered<Eclipse2Knob>(
		anchor("puff_cv_amount_param", Vec(30.48f, 96.f)),
		module, Puffy::PUFF_CV_AMOUNT_PARAM);
	puffCvAmountKnob->setProgressRingBipolar(true);
	addParam(puffCvAmountKnob);

	addInput(createInputCentered<Magitek2InputJack>(
		anchor("input_l", Vec(9.f, 121.f)),
		module, Puffy::INPUT_L));
	addInput(createInputCentered<Magitek2InputJack>(
		anchor("input_r", Vec(23.32f, 121.f)),
		module, Puffy::INPUT_R));
	addInput(createInputCentered<Magitek2InputJack>(
		anchor("puff_cv_input", Vec(43.f, 108.f)),
		module, Puffy::PUFF_CV_INPUT));
	addOutput(createOutputCentered<Magitek2OutputJack>(
		anchor("output_l", Vec(37.64f, 121.f)),
		module, Puffy::OUTPUT_L));
	addOutput(createOutputCentered<Magitek2OutputJack>(
		anchor("output_r", Vec(51.96f, 121.f)),
		module, Puffy::OUTPUT_R));

	addChild(createLightCentered<SmallAperture<RedApertureLight>>(
		anchor("limit_light", Vec(54.f, 72.5f)),
		module, Puffy::LIMIT_LIGHT));

	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0.f)));
	addChild(createWidget<ScrewSilver>(
		Vec(box.size.x - 2.f * RACK_GRID_WIDTH, 0.f)));
	addChild(createWidget<ScrewSilver>(
		Vec(RACK_GRID_WIDTH, box.size.y - RACK_GRID_WIDTH)));
	addChild(createWidget<ScrewSilver>(
		Vec(
			box.size.x - 2.f * RACK_GRID_WIDTH,
			box.size.y - RACK_GRID_WIDTH)));
	previewTimer.markAnchorsDone();
}

void PuffyWidget::appendContextMenu(Menu* menu) {
	ModuleWidget::appendContextMenu(menu);
	auto* puffyModule = dynamic_cast<Puffy*>(module);
	if (!puffyModule) {
		return;
	}
	menu->addChild(new MenuSeparator());
	menu->addChild(createCheckMenuItem(
		"Auto Deflate",
		"",
		[puffyModule]() {
			return puffyModule->autoDeflateEnabled.load(
				std::memory_order_relaxed);
		},
		[puffyModule]() {
			const bool enabled = puffyModule->autoDeflateEnabled.load(
				std::memory_order_relaxed);
			puffyModule->autoDeflateEnabled.store(
				!enabled, std::memory_order_relaxed);
		}));
}

Model* modelPuffy = createModel<Puffy, PuffyWidget>("Puffy");
