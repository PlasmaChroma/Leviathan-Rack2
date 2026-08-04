#include "PuffyWidget.hpp"

#include "PanelSvgUtils.hpp"
#include "PuffyFishWidget.hpp"
#include "PuffyTransferPreviewWidget.hpp"
#include "PuffyVisualPalette.hpp"
#include "visual/ApertureLight.hpp"
#include "visual/FractalGlassOverlay.hpp"
#include "visual/PreviewSurface.hpp"
#include "visual/VisualAssets.hpp"

namespace {

struct PuffyViewportGradient final : TransparentWidget {
	void draw(const DrawArgs& args) override {
		const float inset = mm2px(0.16f);
		const float width = std::max(0.f, box.size.x - 2.f * inset);
		const float height = std::max(0.f, box.size.y - 2.f * inset);
		nvgBeginPath(args.vg);
		nvgRect(args.vg, inset, inset, width, height);
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
	static constexpr float FONT_SIZE = 11.f;
	Puffy* module = nullptr;
	bool negativePart = true;

	PuffyCharacterReadout(Puffy* module, bool negativePart)
		: module(module), negativePart(negativePart) {
	}

	void draw(const DrawArgs& args) override {
		if (!APP || !APP->window || !APP->window->uiFont) {
			return;
		}
		const int character = module
			? clamp(int(std::lround(
				module->params[negativePart
					? Puffy::CHARACTER_PARAM
					: Puffy::POSITIVE_CHARACTER_PARAM].getValue())),
				0,
				puffy::kCharacterCount - 1)
			: int(puffy::Character::Bloom);
		static const char* const labels[] = {
			"BLOOM",
			"SPINE",
			"FRENZY",
			"RIPTIDE",
			"VOID",
			"SWARM"
		};
		const bool charactersLinked = module
			&& module->params[Puffy::CHARACTER_LINK_PARAM].getValue() > 0.5f;
		std::string text;
		NVGcolor textColor;
		if (charactersLinked) {
			text = negativePart ? labels[character] : "LINKED";
			textColor = negativePart
				? puffy_visual::characterTint(character)
				: nvgRGB(255, 255, 255);
		}
		else {
			text = std::string(negativePart ? "− " : "+ ") + labels[character];
			textColor = puffy_visual::characterTint(character);
		}
		nvgFontSize(args.vg, FONT_SIZE);
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgTextAlign(
			args.vg,
			NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		nvgFillColor(args.vg, textColor);
		nvgText(
			args.vg,
			0.5f * box.size.x,
			0.5f * box.size.y,
			text.c_str(),
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
		const bool advancingLinkedPositive = module
			&& !negativeIsSource
			&& module->params[Puffy::CHARACTER_LINK_PARAM].getValue() > 0.5f;
		int linkedNextCharacter = int(puffy::Character::Bloom);
		if (advancingLinkedPositive) {
			// The audio thread keeps the positive parameter synchronized from the
			// authoritative negative parameter while linked. Derive the intended
			// click result before the base switch changes the positive parameter,
			// so that synchronization cannot erase the click between those steps.
			const int linkedCharacter = clamp(
				int(std::lround(
					module->params[Puffy::CHARACTER_PARAM].getValue())),
				int(puffy::Character::Bloom),
				puffy::kCharacterCount - 1);
			linkedNextCharacter = linkedCharacter >= puffy::kCharacterCount - 1
				? int(puffy::Character::Bloom)
				: linkedCharacter + 1;
		}
		SmallGoldButton::onDragStart(e);
		if (advancingLinkedPositive) {
			module->params[Puffy::CHARACTER_PARAM].setValue(linkedNextCharacter);
			module->params[Puffy::POSITIVE_CHARACTER_PARAM].setValue(
				linkedNextCharacter);
		}
		else if (module) {
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
	visual_assets::SplitPanelRenderer splitPanel(
		this, "res/Puffy.panel.svg");
	const std::string& panelPath = splitPanel.panelPath();
	splitPanel.addLabels("res/Puffy.labels.svg");
	visual_assets::addFractalGlassOverlay(
		this, panelPath, splitPanel.panelSurfaceEffectWidget());
	math::Rect leviathanLogoRectMm(
		Vec(14.120335f, 118.43102f),
		Vec(32.71933f, 12.24054f));
	panel_svg::loadRectFromSvgMm(
		panelPath,
		"BRANDING_LEVIATHAN_LOGO_RASTER",
		&leviathanLogoRectMm);
	addChild(visual_assets::createAspectFitRasterImageWidget(
		"res/icon/Leviathan_Logo_S2.png",
		leviathanLogoRectMm));
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
	addChild(visual_assets::createPreviewFrameEnhancementWidget(fishRectMm));
	const float viewportInsetMm = 0.2f;
	math::Rect fishContentRectMm = fishRectMm;
	fishContentRectMm.pos = fishContentRectMm.pos.plus(
		Vec(viewportInsetMm));
	fishContentRectMm.size = fishContentRectMm.size.minus(
		Vec(2.f * viewportInsetMm));
	auto* viewportFramebuffer = new widget::FramebufferWidget();
	viewportFramebuffer->box.pos = mm2px(fishContentRectMm.pos);
	viewportFramebuffer->box.size = mm2px(fishContentRectMm.size);
	viewportFramebuffer->dirtyOnSubpixelChange = false;
	auto* viewportGradient = new PuffyViewportGradient();
	viewportGradient->box.size = viewportFramebuffer->box.size;
	viewportFramebuffer->addChild(viewportGradient);
	addChild(viewportFramebuffer);

	auto* fish = new PuffyFishWidget(module);
	fish->box.pos = mm2px(fishContentRectMm.pos);
	fish->box.size = mm2px(fishContentRectMm.size);
	addChild(fish);

	math::Rect transferPreviewRectMm;
	if (!panel_svg::loadRectFromSvgMm(
		panelPath, "transfer_preview_rect", &transferPreviewRectMm)) {
		transferPreviewRectMm.pos = Vec(4.5f, 67.f);
		transferPreviewRectMm.size = Vec(51.96f, 10.5f);
	}
	addChild(visual_assets::createPreviewFrameEnhancementWidget(
		transferPreviewRectMm));
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
	auto* negativeCharacterReadout = new PuffyCharacterReadout(module, true);
	negativeCharacterReadout->box.size = mm2px(Vec(12.5f, 4.f));
	negativeCharacterReadout->box.pos =
		anchor("negative_character_readout", Vec(9.91f, 95.5f))
			.minus(negativeCharacterReadout->box.size.mult(0.5f));
	addChild(negativeCharacterReadout);
	auto* positiveCharacterReadout = new PuffyCharacterReadout(module, false);
	positiveCharacterReadout->box.size = mm2px(Vec(12.5f, 4.f));
	positiveCharacterReadout->box.pos =
		anchor("positive_character_readout", Vec(22.41f, 95.5f))
			.minus(positiveCharacterReadout->box.size.mult(0.5f));
	addChild(positiveCharacterReadout);
	addParam(createParamCentered<LeviathanHaloKnob2>(
		anchor("puff_param", Vec(18.f, 83.5f)),
		module, Puffy::PUFF_PARAM));
	auto* sensitivityKnob = createParamCentered<Eclipse2Knob>(
		anchor("sensitivity_param", Vec(44.f, 83.5f)),
		module, Puffy::SENSITIVITY_PARAM);
	sensitivityKnob->setProgressRingBipolar(true);
	addParam(sensitivityKnob);
	addParam(createParamCentered<Eclipse2Knob>(
		anchor("mix_param", Vec(51.96f, 97.5f)),
		module, Puffy::MIX_PARAM));
	auto* puffCvAmountKnob = createParamCentered<Eclipse2Knob>(
		anchor("puff_cv_amount_param", Vec(30.48f, 96.f)),
		module, Puffy::PUFF_CV_AMOUNT_PARAM);
	puffCvAmountKnob->setProgressRingBipolar(true);
	addParam(puffCvAmountKnob);

	addInput(createInputCentered<Magitek2InputJack>(
		anchor("input_l", Vec(7.62f, 111.93589f)),
		module, Puffy::INPUT_L));
	addInput(createInputCentered<Magitek2InputJack>(
		anchor("input_r", Vec(22.86f, 111.93589f)),
		module, Puffy::INPUT_R));
	addInput(createInputCentered<Magitek2InputJack>(
		anchor("puff_cv_input", Vec(43.f, 108.f)),
		module, Puffy::PUFF_CV_INPUT));
	addOutput(createOutputCentered<Magitek2OutputJack>(
		anchor("output_l", Vec(38.10f, 111.93589f)),
		module, Puffy::OUTPUT_L));
	addOutput(createOutputCentered<Magitek2OutputJack>(
		anchor("output_r", Vec(53.34f, 111.93589f)),
		module, Puffy::OUTPUT_R));

	addChild(createLightCentered<SmallAperture<RedApertureLight>>(
		anchor("limit_light", Vec(45.72f, 111.93589f)),
		module, Puffy::LIMIT_LIGHT));

	addChild(createWidget<CyanOrbScrew>(Vec(RACK_GRID_WIDTH, 0.f)));
	addChild(createWidget<CyanOrbScrew>(
		Vec(box.size.x - 2.f * RACK_GRID_WIDTH, 0.f)));
	addChild(createWidget<CyanOrbScrew>(
		Vec(RACK_GRID_WIDTH, box.size.y - RACK_GRID_WIDTH)));
	addChild(createWidget<CyanOrbScrew>(
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
