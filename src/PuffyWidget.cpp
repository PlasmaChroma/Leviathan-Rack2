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

static const char* const kPuffyCharacterLabels[puffy::kCharacterCount] = {
	"BLOOM",
	"SPINE",
	"FRENZY",
	"RIPTIDE",
	"VOID",
	"SWARM"
};

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
			nvgRGB(38, 31, 53),
			nvgRGB(21, 18, 31)));
		nvgFill(args.vg);
	}
};

struct PuffyCharacterReadout final : TransparentWidget {
	static constexpr float FONT_SIZE = 12.f;
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
		const bool charactersLinked = module
			&& module->params[Puffy::CHARACTER_LINK_PARAM].getValue() > 0.5f;
		std::string text;
		NVGcolor textColor;
		if (charactersLinked) {
			text = negativePart ? kPuffyCharacterLabels[character] : "LINKED";
			textColor = negativePart
				? puffy_visual::characterTint(character)
				: nvgRGB(255, 255, 255);
		}
		else {
			text = std::string(negativePart ? "− " : "+ ")
				+ kPuffyCharacterLabels[character];
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

struct PuffyCharacterMenuItem final : MenuItem {
	Puffy* module = nullptr;
	bool negativePart = true;
	int character = int(puffy::Character::Bloom);

	void onAction(const event::Action& e) override {
		if (module) {
			const int paramId = negativePart
				? Puffy::CHARACTER_PARAM
				: Puffy::POSITIVE_CHARACTER_PARAM;
			module->params[paramId].setValue(float(character));
			module->synchronizeCharacterSelectionFromUi(negativePart);
		}
		MenuItem::onAction(e);
	}

	void step() override {
		if (module) {
			const int paramId = negativePart
				? Puffy::CHARACTER_PARAM
				: Puffy::POSITIVE_CHARACTER_PARAM;
			rightText = int(std::lround(module->params[paramId].getValue()))
				== character ? "✓" : "";
		}
		MenuItem::step();
	}

	void draw(const DrawArgs& args) override {
		const std::string label = text;
		text.clear();
		MenuItem::draw(args);
		text = label;
		if (!APP || !APP->window || !APP->window->uiFont) {
			return;
		}
		// Draw the label ourselves so it inherits the character color, while the
		// base MenuItem still supplies Rack's hover and checkmark behavior.
		nvgFontSize(args.vg, 14.f);
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
		nvgFillColor(args.vg, puffy_visual::characterTint(character));
		nvgText(args.vg, 10.f, 0.5f * box.size.y, label.c_str(), nullptr);
	}
};

struct PuffyCharacterMenuButton final : OpaqueWidget {
	Puffy* module = nullptr;
	bool negativePart = true;
	bool isHovered = false;

	void step() override {
		OpaqueWidget::step();
		if (module && !negativePart) {
			const bool linked = module->params[Puffy::CHARACTER_LINK_PARAM].getValue() > 0.5f;
			visible = !linked;
			if (linked) {
				isHovered = false;
			}
		}
	}

	void onEnter(const event::Enter& e) override {
		isHovered = true;
		OpaqueWidget::onEnter(e);
	}

	void onLeave(const event::Leave& e) override {
		isHovered = false;
		OpaqueWidget::onLeave(e);
	}

	void onButton(const event::Button& e) override {
		if (!module || e.button != GLFW_MOUSE_BUTTON_LEFT
			|| e.action != GLFW_PRESS) {
			OpaqueWidget::onButton(e);
			return;
		}
		const bool charactersLinked = module
			&& module->params[Puffy::CHARACTER_LINK_PARAM].getValue() > 0.5f;
		ui::Menu* menu = createMenu();
		menu->box.pos = getAbsoluteOffset(Vec(0.f, box.size.y));
		menu->addChild(createMenuLabel(
			charactersLinked
				? "Both Polarities"
				: (negativePart ? "Negative Character" : "Positive Character")));
		for (int character = 0; character < puffy::kCharacterCount; ++character) {
			auto* item = new PuffyCharacterMenuItem();
			item->module = module;
			item->negativePart = negativePart;
			item->character = character;
			item->text = kPuffyCharacterLabels[character];
			menu->addChild(item);
		}
		e.consume(this);
	}

	void draw(const DrawArgs& args) override {
		const int paramId = negativePart
			? Puffy::CHARACTER_PARAM
			: Puffy::POSITIVE_CHARACTER_PARAM;
		const int character = module
			? clamp(int(std::lround(module->params[paramId].getValue())),
				0, puffy::kCharacterCount - 1)
			: int(puffy::Character::Bloom);
		const NVGcolor tint = puffy_visual::characterTint(character);
		NVGcolor borderTint = tint;
		float strokeWidth = 0.85f;
		NVGcolor fillColor = nvgRGBA(8, 7, 13, 218);
		if (isHovered) {
			borderTint.a = 1.0f;
			strokeWidth = 1.35f;
			fillColor = nvgRGBA(24, 20, 36, 240);
		}
		else {
			borderTint.a = 0.82f;
			strokeWidth = 0.85f;
		}
		const float radius = 0.5f * box.size.y;
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, 0.f, 0.f, box.size.x, box.size.y, radius);
		nvgFillColor(args.vg, fillColor);
		nvgFill(args.vg);
		nvgStrokeColor(args.vg, borderTint);
		nvgStrokeWidth(args.vg, strokeWidth);
		nvgStroke(args.vg);
		if (!APP || !APP->window || !APP->window->uiFont) {
			return;
		}
		nvgFontSize(args.vg, 8.5f);
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		const float leftPadding = 4.5f;
		const float arrowAreaWidth = 8.f;
		const float labelCenterX = leftPadding + 0.5f * std::max(1.f, box.size.x - leftPadding - arrowAreaWidth);
		nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		nvgFillColor(args.vg, tint);
		nvgText(args.vg, labelCenterX, 0.5f * box.size.y,
			kPuffyCharacterLabels[character], nullptr);
		// Keep the disclosure arrow fixed near the right edge inside the pill.
		nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		nvgText(args.vg, box.size.x - 4.5f, 0.5f * box.size.y, "▾", nullptr);
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

struct PuffyPolarityLinkButton final : ParamWidget {
	bool isHovered = false;
	ui::Tooltip* tooltip = nullptr;

	PuffyPolarityLinkButton() {
		box.size = Vec(16.f, 16.f);
	}

	~PuffyPolarityLinkButton() override {
		destroyTooltip();
	}

	std::string getTooltipString() {
		auto* puffyModule = dynamic_cast<Puffy*>(module);
		const bool isLinked = puffyModule
			? (puffyModule->params[Puffy::CHARACTER_LINK_PARAM].getValue() > 0.5f)
			: (getParamQuantity() ? getParamQuantity()->getValue() > 0.5f : true);
		return isLinked ? "Unlink Polarity" : "Link Polarity";
	}

	void createTooltip() {
		if (!settings::tooltips || tooltip || !APP || !APP->scene) {
			return;
		}
		tooltip = new ui::Tooltip();
		tooltip->text = getTooltipString();
		APP->scene->addChild(tooltip);
	}

	void destroyTooltip() {
		if (tooltip) {
			if (tooltip->parent) {
				tooltip->parent->removeChild(tooltip);
			}
			delete tooltip;
			tooltip = nullptr;
		}
	}

	void refreshTooltip() {
		if (tooltip) {
			tooltip->text = getTooltipString();
		}
	}

	void onEnter(const event::Enter& e) override {
		isHovered = true;
		createTooltip();
		event::Enter eCopy = e;
		OpaqueWidget::onEnter(eCopy);
	}

	void onLeave(const event::Leave& e) override {
		isHovered = false;
		destroyTooltip();
		event::Leave eCopy = e;
		OpaqueWidget::onLeave(eCopy);
	}

	void onButton(const event::Button& e) override {
		if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
			auto* puffyModule = dynamic_cast<Puffy*>(module);
			if (puffyModule) {
				const float curVal = puffyModule->params[Puffy::CHARACTER_LINK_PARAM].getValue();
				const float newVal = (curVal > 0.5f) ? 0.f : 1.f;
				puffyModule->params[Puffy::CHARACTER_LINK_PARAM].setValue(newVal);
				if (newVal > 0.5f) {
					puffyModule->synchronizeCharacterSelectionFromUi(true);
				}
			}
			else if (getParamQuantity()) {
				const float curVal = getParamQuantity()->getValue();
				getParamQuantity()->setValue(curVal > 0.5f ? 0.f : 1.f);
			}
			refreshTooltip();
			e.consume(this);
			return;
		}
		ParamWidget::onButton(e);
	}

	void draw(const DrawArgs& args) override {
		auto* puffyModule = dynamic_cast<Puffy*>(module);
		const bool isLinked = puffyModule
			? (puffyModule->params[Puffy::CHARACTER_LINK_PARAM].getValue() > 0.5f)
			: (getParamQuantity() ? getParamQuantity()->getValue() > 0.5f : true);

		const int character = puffyModule
			? clamp(int(std::lround(puffyModule->params[Puffy::CHARACTER_PARAM].getValue())),
				0, puffy::kCharacterCount - 1)
			: int(puffy::Character::Bloom);
		const NVGcolor activeTint = puffy_visual::characterTint(character);

		// Subtle hover background highlight without a hard border box
		if (isHovered) {
			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, 0.f, 0.f, box.size.x, box.size.y, 4.f);
			NVGcolor hoverGlow = isLinked ? activeTint : nvgRGB(255, 255, 255);
			hoverGlow.a = 0.18f;
			nvgFillColor(args.vg, hoverGlow);
			nvgFill(args.vg);
		}

		// Icon color selection
		NVGcolor iconColor;
		if (isLinked) {
			iconColor = isHovered ? nvgRGB(255, 255, 255) : activeTint;
		}
		else {
			iconColor = isHovered ? nvgRGB(240, 240, 250) : nvgRGBA(140, 140, 160, 160);
		}

		// Draw larger chain icon with realistic physical proportions & 3D weave outlines
		nvgSave(args.vg);
		nvgTranslate(args.vg, box.size.x * 0.5f, box.size.y * 0.5f);
		nvgRotate(args.vg, -M_PI / 4.f);

		const float strokeW = 1.1f;
		const float outlineW = strokeW + 1.6f;
		const float linkW = 7.6f;
		const float linkH = 4.6f;
		const float linkR = 2.0f;
		const NVGcolor outlineColor = nvgRGBA(6, 5, 12, 245);

		if (isLinked) {
			const float offsetX = 1.8f;
			const float offsetY = 0.5f;

			const Vec c1(-offsetX, -offsetY); // Bottom-left link
			const Vec c2(offsetX, offsetY);   // Top-right link (offset perpendicularly)

			// Glow halo on hover when linked
			if (isHovered) {
				NVGcolor halo = activeTint;
				halo.a = 0.35f;
				nvgStrokeColor(args.vg, halo);
				nvgStrokeWidth(args.vg, outlineW + 1.4f);

				nvgBeginPath(args.vg);
				nvgRoundedRect(args.vg, c1.x - linkW * 0.5f, c1.y - linkH * 0.5f, linkW, linkH, linkR);
				nvgStroke(args.vg);

				nvgBeginPath(args.vg);
				nvgRoundedRect(args.vg, c2.x - linkW * 0.5f, c2.y - linkH * 0.5f, linkW, linkH, linkR);
				nvgStroke(args.vg);
			}

			// 1. Link 1 (Bottom-left link) - 3D outline + core
			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, c1.x - linkW * 0.5f, c1.y - linkH * 0.5f, linkW, linkH, linkR);
			nvgStrokeColor(args.vg, outlineColor);
			nvgStrokeWidth(args.vg, outlineW);
			nvgStroke(args.vg);

			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, c1.x - linkW * 0.5f, c1.y - linkH * 0.5f, linkW, linkH, linkR);
			nvgStrokeColor(args.vg, iconColor);
			nvgStrokeWidth(args.vg, strokeW);
			nvgStroke(args.vg);

			// 2. Link 2 (Top-right link) - 3D outline (occludes Link 1 at bottom crossover) + core
			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, c2.x - linkW * 0.5f, c2.y - linkH * 0.5f, linkW, linkH, linkR);
			nvgStrokeColor(args.vg, outlineColor);
			nvgStrokeWidth(args.vg, outlineW);
			nvgStroke(args.vg);

			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, c2.x - linkW * 0.5f, c2.y - linkH * 0.5f, linkW, linkH, linkR);
			nvgStrokeColor(args.vg, iconColor);
			nvgStrokeWidth(args.vg, strokeW);
			nvgStroke(args.vg);

			// 3. Link 1 Top Strand (weaves OVER Link 2) - 3D outline (occludes Link 2 at top crossover) + core
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, c1.x - linkW * 0.5f + linkR, c1.y - linkH * 0.5f);
			nvgLineTo(args.vg, c1.x + linkW * 0.5f - linkR, c1.y - linkH * 0.5f);
			nvgStrokeColor(args.vg, outlineColor);
			nvgStrokeWidth(args.vg, outlineW);
			nvgStroke(args.vg);

			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, c1.x - linkW * 0.5f + linkR, c1.y - linkH * 0.5f);
			nvgLineTo(args.vg, c1.x + linkW * 0.5f - linkR, c1.y - linkH * 0.5f);
			nvgStrokeColor(args.vg, iconColor);
			nvgStrokeWidth(args.vg, strokeW);
			nvgStroke(args.vg);
		}
		else {
			const float offsetX = 4.8f;
			const float offsetY = 0.8f;

			const Vec c1(-offsetX, -offsetY);
			const Vec c2(offsetX, offsetY);

			// Link 1 (separated) - 3D outline + core
			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, c1.x - linkW * 0.5f, c1.y - linkH * 0.5f, linkW, linkH, linkR);
			nvgStrokeColor(args.vg, outlineColor);
			nvgStrokeWidth(args.vg, outlineW);
			nvgStroke(args.vg);

			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, c1.x - linkW * 0.5f, c1.y - linkH * 0.5f, linkW, linkH, linkR);
			nvgStrokeColor(args.vg, iconColor);
			nvgStrokeWidth(args.vg, strokeW);
			nvgStroke(args.vg);

			// Link 2 (separated) - 3D outline + core
			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, c2.x - linkW * 0.5f, c2.y - linkH * 0.5f, linkW, linkH, linkR);
			nvgStrokeColor(args.vg, outlineColor);
			nvgStrokeWidth(args.vg, outlineW);
			nvgStroke(args.vg);

			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, c2.x - linkW * 0.5f, c2.y - linkH * 0.5f, linkW, linkH, linkR);
			nvgStrokeColor(args.vg, iconColor);
			nvgStrokeWidth(args.vg, strokeW);
			nvgStroke(args.vg);
		}

		nvgRestore(args.vg);
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

	const Vec characterMenuSize = mm2px(Vec(13.5f, 4.5f));
	const Vec characterMenuInset = mm2px(Vec(0.6f, 0.6f));
	auto* negativeCharacterMenu = new PuffyCharacterMenuButton();
	negativeCharacterMenu->module = module;
	negativeCharacterMenu->negativePart = true;
	negativeCharacterMenu->box.size = characterMenuSize;
	negativeCharacterMenu->box.pos = fish->box.pos.plus(Vec(
		characterMenuInset.x,
		fish->box.size.y - characterMenuSize.y - characterMenuInset.y));
	addChild(negativeCharacterMenu);

	auto* polarityLinkIcon = createParam<PuffyPolarityLinkButton>(
		Vec(
			negativeCharacterMenu->box.pos.x,
			negativeCharacterMenu->box.pos.y - 14.f - 3.f),
		module,
		Puffy::CHARACTER_LINK_PARAM);
	addParam(polarityLinkIcon);

	auto* positiveCharacterMenu = new PuffyCharacterMenuButton();
	positiveCharacterMenu->module = module;
	positiveCharacterMenu->negativePart = false;
	positiveCharacterMenu->box.size = characterMenuSize;
	positiveCharacterMenu->box.pos = fish->box.pos.plus(Vec(
		fish->box.size.x - characterMenuSize.x - characterMenuInset.x,
		fish->box.size.y - characterMenuSize.y - characterMenuInset.y));
	addChild(positiveCharacterMenu);

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
