#include "PuffyWidget.hpp"

#include "PanelSvgUtils.hpp"
#include "PuffyFishWidget.hpp"
#include "PuffyTransferPreviewWidget.hpp"
#include "PuffyVisualPalette.hpp"
#include "visual/ApertureLight.hpp"
#include "visual/PreviewSurface.hpp"
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
	static constexpr float FONT_SIZE = 8.8f;
	Puffy* module = nullptr;

	explicit PuffyCharacterReadout(Puffy* module) : module(module) {
	}

	void draw(const DrawArgs& args) override {
		if (!APP || !APP->window || !APP->window->uiFont) {
			return;
		}
		const int negativeCharacter = module
			? clamp(int(std::lround(
				module->params[Puffy::CHARACTER_PARAM].getValue())),
				0,
				int(puffy::Character::Void))
			: int(puffy::Character::Bloom);
		const int positiveCharacter = module
			? clamp(int(std::lround(
				module->params[Puffy::POSITIVE_CHARACTER_PARAM].getValue())),
				0,
				int(puffy::Character::Void))
			: int(puffy::Character::Bloom);
		static const char* const labels[] = {
			"BLOOM",
			"SPINE",
			"FRENZY",
			"RIPTIDE",
			"VOID"
		};
		nvgFontSize(args.vg, FONT_SIZE);
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
		nvgFillColor(args.vg, puffy_visual::characterTint(negativeCharacter));
		nvgText(
			args.vg,
			0.f,
			0.5f * box.size.y,
			(std::string("− ") + labels[negativeCharacter]).c_str(),
			nullptr);
		nvgTextAlign(args.vg, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
		nvgFillColor(args.vg, puffy_visual::characterTint(positiveCharacter));
		nvgText(
			args.vg,
			box.size.x,
			0.5f * box.size.y,
			(std::string("+ ") + labels[positiveCharacter]).c_str(),
			nullptr);
	}
};

struct PuffyCharacterButton final : SmallGoldButton {
	Puffy* module = nullptr;
	bool negativeIsSource = true;

	PuffyCharacterButton() {
		momentary = false;
	}

	void onDragStart(const event::DragStart& e) override {
		SmallGoldButton::onDragStart(e);
		if (module) {
			module->synchronizeCharacterSelectionFromUi(negativeIsSource);
		}
	}
};

struct PuffyCharacterLinkButton final : SmallGoldApertureButton {
	Puffy* module = nullptr;

	void onDragStart(const event::DragStart& e) override {
		SmallGoldApertureButton::onDragStart(e);
		if (module
			&& module->params[Puffy::CHARACTER_LINK_PARAM].getValue() > 0.5f) {
			// Relinking always makes the left/negative character authoritative.
			module->synchronizeCharacterSelectionFromUi(true);
		}
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

	math::Rect transferPreviewRectMm;
	if (!panel_svg::loadRectFromSvgMm(
		panelPath, "transfer_preview_rect", &transferPreviewRectMm)) {
		transferPreviewRectMm.pos = Vec(4.5f, 67.f);
		transferPreviewRectMm.size = Vec(51.96f, 10.5f);
	}
	addChild(visual_assets::createPreviewFrameEnhancementWidget(
		transferPreviewRectMm, nvgRGBA(255, 190, 80, 118)));
	const float previewInsetMm = 0.2f;
	transferPreviewRectMm.pos = transferPreviewRectMm.pos.plus(
		Vec(previewInsetMm));
	transferPreviewRectMm.size = transferPreviewRectMm.size.minus(
		Vec(2.f * previewInsetMm));
	const Vec transferPreviewPos = mm2px(transferPreviewRectMm.pos);
	const Vec transferPreviewSize = mm2px(transferPreviewRectMm.size);
	auto* transferSurface = preview_surface::createCachedOpaqueGrid(
		transferPreviewSize);
	transferSurface->box.pos = transferPreviewPos;
	addChild(transferSurface);
	auto* transferPreview = new PuffyTransferPreviewWidget(module);
	transferPreview->box.pos = transferPreviewPos;
	transferPreview->box.size = transferPreviewSize;
	addChild(transferPreview);

	auto* characterButton = createParamCentered<PuffyCharacterButton>(
		anchor("character_param", Vec(30.48f, 14.f)),
		module, Puffy::CHARACTER_PARAM);
	characterButton->module = module;
	characterButton->negativeIsSource = true;
	addParam(characterButton);
	auto* positiveCharacterButton = createParamCentered<PuffyCharacterButton>(
		anchor("positive_character_param", Vec(23.32f, 99.63f)),
		module, Puffy::POSITIVE_CHARACTER_PARAM);
	positiveCharacterButton->module = module;
	positiveCharacterButton->negativeIsSource = false;
	addParam(positiveCharacterButton);
	auto* characterLinkButton =
		createLightParamCentered<PuffyCharacterLinkButton>(
			anchor("character_link_param", Vec(30.48f, 99.63f)),
			module,
			Puffy::CHARACTER_LINK_PARAM,
			Puffy::CHARACTER_LINK_LIGHT);
	characterLinkButton->module = module;
	addParam(characterLinkButton);
	auto* characterReadout = new PuffyCharacterReadout(module);
	characterReadout->box.size = mm2px(Vec(25.f, 4.f));
	characterReadout->box.pos =
		anchor("character_readout", Vec(16.16f, 95.5f))
			.minus(characterReadout->box.size.mult(0.5f));
	addChild(characterReadout);
	addParam(createParamCentered<LeviathanHaloKnob2>(
		anchor("puff_param", Vec(18.f, 83.5f)),
		module, Puffy::PUFF_PARAM));
	addParam(createParamCentered<Eclipse2Knob>(
		anchor("deflate_param", Vec(44.f, 83.5f)),
		module, Puffy::DEFLATE_PARAM));
	addParam(createParamCentered<Eclipse2Knob>(
		anchor("mix_param", Vec(51.96f, 97.5f)),
		module, Puffy::MIX_PARAM));
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
