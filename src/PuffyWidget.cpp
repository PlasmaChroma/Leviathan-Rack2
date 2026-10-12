#include "PuffyWidget.hpp"

#include "PanelSvgUtils.hpp"
#include "PuffyDrawDiagnostics.hpp"
#include "PuffyFishWidget.hpp"
#include "PuffyTransferPreviewWidget.hpp"
#include "PuffyVisualPalette.hpp"
#include "visual/ApertureLight.hpp"
#include "visual/FractalGlassOverlay.hpp"
#include "visual/PreviewSurface.hpp"
#include "visual/VisualAssets.hpp"

#include <algorithm>
#include <chrono>
#include <array>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <vector>

namespace {

static const char* const kPuffyCharacterLabels[puffy::kCharacterCount] = {
	"BLOOM",
	"SPINE",
	"FRENZY",
	"RIPTIDE",
	"VOID",
	"SWARM",
	"TEETH"
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

struct PuffyLimiterModeButton final : SmallGoldButton {
	ui::Tooltip* tooltip = nullptr;

	PuffyLimiterModeButton() : SmallGoldButton(15.f) {
	}

	~PuffyLimiterModeButton() override {
		destroyTooltip();
	}

	std::string getTooltipString() const {
		static const char* const names[] = {"Hard", "Soft", "Off"};
		const auto* puffyModule = dynamic_cast<const Puffy*>(module);
		const int mode = puffyModule
			? clamp(puffyModule->limiterMode.load(std::memory_order_relaxed), 0, 2)
			: 0;
		return std::string("Limiter: ") + names[mode];
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
		if (!tooltip) {
			return;
		}
		if (tooltip->parent) {
			tooltip->parent->removeChild(tooltip);
		}
		delete tooltip;
		tooltip = nullptr;
	}

	void step() override {
		SmallGoldButton::step();
		if (tooltip) {
			tooltip->text = getTooltipString();
		}
	}

	void onEnter(const event::Enter& e) override {
		createTooltip();
		event::Enter eCopy = e;
		OpaqueWidget::onEnter(eCopy);
	}

	void onLeave(const event::Leave& e) override {
		destroyTooltip();
		event::Leave eCopy = e;
		OpaqueWidget::onLeave(eCopy);
	}
};

struct PuffyRoamingButton final : SmallGoldButton {
	PuffyRoamingButton() : SmallGoldButton(15.f) {
		momentary = false;
	}
};

struct PuffyLimiterModeLabels final : TransparentWidget {
	void draw(const DrawArgs& args) override {
		if (!APP || !APP->window || !APP->window->uiFont) {
			return;
		}
		static const char* const labels[] = {"H", "S", "∅"};
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgFontSize(args.vg, 8.f);
		nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		nvgFillColor(args.vg, nvgRGB(255, 255, 255));
		const float rowSpacing = mm2px(3.f);
		for (int row = 0; row < 3; ++row) {
			nvgText(
				args.vg, 0.5f * box.size.x, rowSpacing * (float(row) + 0.5f),
				labels[row], nullptr);
		}
	}
};

struct PuffyRangeTooltip final : ui::Tooltip {
	WeakPtr<Widget> anchor;
	float selectorLeftFraction = 0.f;
	float selectorRightFraction = 1.f;

	void step() override {
		Widget* widget = anchor.get();
		auto* paramWidget = dynamic_cast<ParamWidget*>(widget);
		if (paramWidget && paramWidget->getParamQuantity()) {
			const float value = clamp(
				paramWidget->getParamQuantity()->getValue(), 0.f, 1.f);
			text = "Range " + std::to_string(int(std::lround(value * 100.f)))
				+ "%";
		}
		ui::Tooltip::step();
		if (!widget) {
			return;
		}
		auto* param = dynamic_cast<ParamWidget*>(widget);
		const float value = param && param->getParamQuantity()
			? clamp(param->getParamQuantity()->getValue(), 0.f, 1.f) : 0.f;
		const float selectorX = widget->box.size.x * crossfade(
			selectorLeftFraction, selectorRightFraction, value);
		const Vec selectorScene = widget->getAbsoluteOffset(Vec(selectorX, 0.f));
		box.pos = Vec(
			selectorScene.x - 0.5f * box.size.x,
			selectorScene.y - box.size.y - 3.f);
	}
};

struct PuffyRoamingRangeBar final : ParamWidget {
	static constexpr float kAssetAspect = 726.f / 76.f;
	static constexpr float kTrackTopFraction = 17.f / 76.f;
	static constexpr float kTrackBottomFraction = 59.f / 76.f;
	static constexpr float kFillLeftFraction = 97.f / 726.f;
	static constexpr float kFillRightFraction = 629.f / 726.f;
	static constexpr float kSelectorLeftFraction = 113.f / 726.f;
	static constexpr float kSelectorRightFraction = 613.f / 726.f;
	bool dragging = false;
	bool isHovered = false;
	PuffyRangeTooltip* rangeTooltip = nullptr;

	~PuffyRoamingRangeBar() override {
		destroyRangeTooltip();
	}

	void onContextCreate(const ContextCreateEvent& e) override {
		visual_assets::onRasterContextCreate(e.vg);
		ParamWidget::onContextCreate(e);
	}

	void onContextDestroy(const ContextDestroyEvent& e) override {
		visual_assets::onRasterContextDestroy(e.vg);
		ParamWidget::onContextDestroy(e);
	}

	Puffy* getPuffyModule() const {
		return dynamic_cast<Puffy*>(module);
	}

	Vec localMousePos() {
		if (!APP || !APP->scene) {
			return Vec();
		}
		return APP->scene->getMousePos().minus(getAbsoluteOffset(Vec()))
			.div(std::max(getAbsoluteZoom(), 1e-4f));
	}

	void setFromX(float x) {
		const float selectorLeft = box.size.x * kSelectorLeftFraction;
		const float selectorRight = box.size.x * kSelectorRightFraction;
		const float usable = std::max(selectorRight - selectorLeft, 1.f);
		const float value = clamp((x - selectorLeft) / usable, 0.f, 1.f);
		if (ParamQuantity* quantity = getParamQuantity()) {
			quantity->setValue(value);
		}
	}

	void step() override {
		ParamWidget::step();
		Puffy* puffyModule = getPuffyModule();
		visible = puffyModule && puffyModule->roamingEnabled.load(
			std::memory_order_relaxed);
	}

	void createRangeTooltip() {
		if (!settings::tooltips || rangeTooltip || !APP || !APP->scene) {
			return;
		}
		rangeTooltip = new PuffyRangeTooltip();
		rangeTooltip->anchor = this;
		rangeTooltip->selectorLeftFraction = kSelectorLeftFraction;
		rangeTooltip->selectorRightFraction = kSelectorRightFraction;
		APP->scene->addChild(rangeTooltip);
	}

	void destroyRangeTooltip() {
		if (!rangeTooltip) {
			return;
		}
		if (rangeTooltip->parent) {
			rangeTooltip->parent->removeChild(rangeTooltip);
		}
		delete rangeTooltip;
		rangeTooltip = nullptr;
	}

	void onEnter(const event::Enter& e) override {
		isHovered = true;
		createRangeTooltip();
		event::Enter copy = e;
		OpaqueWidget::onEnter(copy);
	}

	void onLeave(const event::Leave& e) override {
		isHovered = false;
		destroyRangeTooltip();
		event::Leave copy = e;
		OpaqueWidget::onLeave(copy);
	}

	void onButton(const event::Button& e) override {
		if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
			if (e.action == GLFW_PRESS) {
				dragging = true;
				setFromX(e.pos.x);
				e.consume(this);
				return;
			}
			if (e.action == GLFW_RELEASE && dragging) {
				dragging = false;
				e.consume(this);
				return;
			}
		}
		ParamWidget::onButton(e);
	}

	void onDragMove(const event::DragMove& e) override {
		if (dragging && e.button == GLFW_MOUSE_BUTTON_LEFT) {
			setFromX(localMousePos().x);
			e.consume(this);
			return;
		}
		ParamWidget::onDragMove(e);
	}

	void onDragEnd(const event::DragEnd& e) override {
		dragging = false;
		ParamWidget::onDragEnd(e);
	}

	void draw(const DrawArgs& args) override {
		Puffy* puffyModule = getPuffyModule();
		const float setting = getParamQuantity()
			? clamp(getParamQuantity()->getValue(), 0.f, 1.f) : 310.f / 810.f;
		const float actual = puffyModule
			? clamp((puffyModule->roamingDistance.load(std::memory_order_relaxed)
				- 90.f) / 810.f, 0.f, 1.f)
			: 0.f;
		const float assetWidth = box.size.x;
		const float assetHeight = assetWidth / kAssetAspect;
		const std::string fullPath = asset::plugin(
			pluginInstance, "res/icon/NeonBar.png");
		std::shared_ptr<window::Image> image = APP && APP->window
			? APP->window->loadImage(fullPath) : nullptr;
		if (image && image->handle >= 0) {
			int handle = visual_assets::loadRasterMipmapHandle(
				args.vg, image, fullPath);
			if (handle < 0) {
				handle = image->handle;
			}
			NVGpaint assetPaint = nvgImagePattern(
				args.vg, 0.f, 0.f, assetWidth, assetHeight,
				0.f, handle, 1.f);
			nvgBeginPath(args.vg);
			nvgRect(args.vg, 0.f, 0.f, assetWidth, assetHeight);
			nvgFillPaint(args.vg, assetPaint);
			nvgFill(args.vg);
		}

		const float trackY = assetHeight * kTrackTopFraction;
		const float trackHeight = assetHeight
			* (kTrackBottomFraction - kTrackTopFraction);
		const float fillLeft = assetWidth * kFillLeftFraction;
		const float fillRight = assetWidth * kFillRightFraction;
		const float fillWidth = std::max(fillRight - fillLeft, 1.f);
		const float selectorLeft = assetWidth * kSelectorLeftFraction;
		const float selectorRight = assetWidth * kSelectorRightFraction;
		const float selectorX = crossfade(
			selectorLeft, selectorRight, setting);
		const auto drawFillTo = [&](float endpoint, NVGcolor color) {
			const float width = clamp(endpoint - fillLeft, 0.f, fillWidth);
			if (width <= 0.01f) {
				return;
			}
			// Clip a complete trough-shaped capsule instead of rounding a short
			// fill rectangle. This keeps the source asset's left-cap curvature
			// constant even near the minimum setting.
			nvgSave(args.vg);
			nvgScissor(args.vg, fillLeft, trackY, width, trackHeight);
			nvgBeginPath(args.vg);
			nvgRoundedRect(
				args.vg, fillLeft, trackY, fillWidth, trackHeight,
				0.5f * trackHeight);
			nvgFillColor(args.vg, color);
			nvgFill(args.vg);
			nvgRestore(args.vg);
		};
		// The setting fill terminates at the sphere's midline. The sphere hides
		// the clipped vertical edge while remaining centered on the true value.
		drawFillTo(selectorX, nvgRGBA(155, 72, 224, 190));
		drawFillTo(
			fillLeft + fillWidth * actual,
			nvgRGBA(35, 222, 235, 145));

		const float selectorY = trackY + 0.5f * trackHeight;
		const float selectorRadius = 0.46f * trackHeight;
		// The setting handle is deliberately spherical rather than a flat
		// cutoff marker. Its center can reach both curved trough endpoints while
		// the surrounding raster supplies the ornamental min/max framing.
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, selectorX, selectorY, selectorRadius * 1.35f);
		nvgFillColor(args.vg, nvgRGBA(66, 224, 255, 45));
		nvgFill(args.vg);
		const NVGpaint selectorPaint = nvgRadialGradient(
			args.vg,
			selectorX - selectorRadius * 0.32f,
			selectorY - selectorRadius * 0.38f,
			selectorRadius * 0.08f,
			selectorRadius,
			isHovered
				? nvgRGBA(80, 247, 255, 255)
				: nvgRGBA(252, 240, 255, 255),
			isHovered
				? nvgRGBA(4, 104, 156, 255)
				: nvgRGBA(91, 26, 174, 255));
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, selectorX, selectorY, selectorRadius);
		nvgFillPaint(args.vg, selectorPaint);
		nvgFill(args.vg);
		nvgStrokeColor(args.vg, nvgRGBA(94, 231, 255, 235));
		nvgStrokeWidth(args.vg, 0.75f);
		nvgStroke(args.vg);

		if (APP && APP->window && APP->window->uiFont) {
			nvgFontFaceId(args.vg, APP->window->uiFont->handle);
			nvgFontSize(args.vg, 8.f);
			nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_TOP);
			nvgFillColor(args.vg, nvgRGB(255, 255, 255));
			nvgText(
				args.vg, box.size.x * 0.5f, assetHeight + 1.f,
				"RANGE", nullptr);
		}
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

struct PuffyCharacterMenuButton final : ParamWidget {
	bool negativePart = true;
	bool isHovered = false;

	Puffy* getPuffyModule() const {
		return dynamic_cast<Puffy*>(module);
	}

	void step() override {
		ParamWidget::step();
		Puffy* puffyModule = getPuffyModule();
		if (puffyModule && !negativePart) {
			const bool linked = puffyModule->params[Puffy::CHARACTER_LINK_PARAM].getValue() > 0.5f;
			visible = !linked;
			if (linked) {
				isHovered = false;
			}
		}
	}

	void onEnter(const event::Enter& e) override {
		isHovered = true;
		ParamWidget::onEnter(e);
	}

	void onLeave(const event::Leave& e) override {
		isHovered = false;
		ParamWidget::onLeave(e);
	}

	void onButton(const event::Button& e) override {
		Puffy* puffyModule = getPuffyModule();
		if (!puffyModule || e.button != GLFW_MOUSE_BUTTON_LEFT
			|| e.action != GLFW_PRESS) {
			ParamWidget::onButton(e);
			return;
		}
		const bool charactersLinked = puffyModule
			->params[Puffy::CHARACTER_LINK_PARAM].getValue() > 0.5f;
		ui::Menu* menu = createMenu();
		menu->box.pos = getAbsoluteOffset(Vec(0.f, box.size.y));
		menu->addChild(createMenuLabel(
			charactersLinked
				? "Both Polarities"
				: (negativePart ? "Negative Character" : "Positive Character")));
		for (int character = 0; character < puffy::kCharacterCount; ++character) {
			auto* item = new PuffyCharacterMenuItem();
			item->module = puffyModule;
			item->negativePart = negativePart;
			item->character = character;
			item->text = kPuffyCharacterLabels[character];
			menu->addChild(item);
		}
		e.consume(this);
	}

	void draw(const DrawArgs& args) override {
		Puffy* puffyModule = getPuffyModule();
		const int paramId = negativePart
			? Puffy::CHARACTER_PARAM
			: Puffy::POSITIVE_CHARACTER_PARAM;
		const int character = puffyModule
			? clamp(int(std::lround(puffyModule->params[paramId].getValue())),
				0, puffy::kCharacterCount - 1)
			: int(negativePart
				? puffy::Character::Bloom
				: puffy::Character::Spine);
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

		// Icon color selection
		NVGcolor iconColor;
		if (isLinked) {
			iconColor = isHovered ? nvgRGB(255, 255, 255) : nvgRGB(150, 150, 165);
		}
		else {
			iconColor = isHovered ? nvgRGB(245, 245, 255) : nvgRGBA(140, 140, 160, 160);
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
			const float offsetX1 = 2.2f; // Bottom-left link shifted slightly more left
			const float offsetX2 = 1.8f;
			const float offsetY = 0.5f;

			const Vec c1(-offsetX1, -offsetY); // Bottom-left link
			const Vec c2(offsetX2, offsetY);   // Top-right link (offset perpendicularly)

			// Draw Link 1 first, then Link 2 over it. The overlap outline is an
			// intentional depth cue at the lower crossover.
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

			// At the opposite crossover, locally bring Link 1 back over Link 2.
			// Dark outline uses NVG_BUTT line caps so it stays hidden under Link 1's
			// grey core without creating dark line cuts across the top strand.
			nvgLineCap(args.vg, NVG_BUTT);
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, c1.x - 0.2f, c1.y - linkH * 0.5f);
			nvgLineTo(args.vg, c1.x + linkW * 0.5f - linkR, c1.y - linkH * 0.5f);
			nvgArcTo(args.vg, c1.x + linkW * 0.5f, c1.y - linkH * 0.5f, c1.x + linkW * 0.5f, c1.y, linkR);
			nvgLineTo(args.vg, c1.x + linkW * 0.5f, c1.y + 0.2f);
			nvgStrokeColor(args.vg, outlineColor);
			nvgStrokeWidth(args.vg, outlineW);
			nvgStroke(args.vg);

			nvgLineCap(args.vg, NVG_ROUND);
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, c1.x - 0.6f, c1.y - linkH * 0.5f);
			nvgLineTo(args.vg, c1.x + linkW * 0.5f - linkR, c1.y - linkH * 0.5f);
			nvgArcTo(args.vg, c1.x + linkW * 0.5f, c1.y - linkH * 0.5f, c1.x + linkW * 0.5f, c1.y, linkR);
			nvgLineTo(args.vg, c1.x + linkW * 0.5f, c1.y + 0.5f);
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

std::string PuffyWidget::drawLogRootPath() {
	return system::join(asset::user(), "Leviathan/Puffy");
}

std::string PuffyWidget::drawLogDateTimeStamp() {
	std::time_t now = std::time(nullptr);
	std::tm tm {};
#if defined(_WIN32)
	localtime_s(&tm, &now);
#else
	localtime_r(&now, &tm);
#endif
	std::ostringstream out;
	out << std::put_time(&tm, "%Y%m%d_%H%M%S");
	return out.str();
}

void PuffyWidget::stopDrawLog() {
	if (drawLogFile.is_open()) {
		drawLogFile.close();
	}
	drawLogPath.clear();
	drawLogActive = false;
	drawLogRowCounter = 0u;
	consumePuffyDrawMetrics();
}

void PuffyWidget::syncDrawLog(bool enabled, std::uint32_t instanceId) {
	if (!enabled) {
		if (drawLogActive) {
			stopDrawLog();
		}
		return;
	}
	if (drawLogActive && drawLogFile.is_open()) {
		return;
	}
	const std::string root = drawLogRootPath();
	if (!system::createDirectories(root) && !system::isDirectory(root)) {
		WARN("Puffy failed to create draw log directory: %s", root.c_str());
		return;
	}
	static std::uint32_t openSequence = 0u;
	drawLogPath = system::join(
		root,
		"puffy_draw_" + std::to_string(instanceId) + "_"
			+ drawLogDateTimeStamp() + "_"
			+ std::to_string(openSequence++) + ".csv");
	drawLogFile.open(drawLogPath.c_str(), std::ios::out | std::ios::trunc);
	if (!drawLogFile.is_open()) {
		WARN("Puffy failed to open draw log CSV: %s", drawLogPath.c_str());
		drawLogPath.clear();
		return;
	}
	drawLogActive = true;
	drawLogRowCounter = 0u;
	consumePuffyDrawMetrics();
	drawLogFile << std::fixed << std::setprecision(3);
	drawLogFile
		<< "row,module_id,instance_id,time_sec,last_frame_us,total_draw_us,module_widget_draw_us,"
		<< "fish_draw_us,fish_other_us,body_ensure_us,body_recolor_us,body_upload_us,body_draw_us,"
		<< "body_transition_draw_us,body_transition_atlas_prewarm_us,"
		<< "fin_draw_us,eye_draw_us,transfer_draw_us,transfer_curve_draw_us,transfer_curve_rebuild_us,"
		<< "body_cache_hits,body_recolors,body_image_creates,body_image_updates,body_context_resets,"
		<< "body_fallback_draws,body_transition_draws,body_transition_atlas_creates,body_transition_atlas_resets,"
		<< "body_transition_atlas_prewarms,"
		<< "transfer_curve_rebuilds,negative_character,positive_character,"
		<< "effective_amount,input_activity,gain_reduction\n";
}

struct PuffyRoamingOverlay final : TransparentWidget {
	static constexpr float kBaseSize = 80.f;
	static constexpr float kBaseShadowPad = 16.f;
	static constexpr float kMaximumRackZoom = 8.f;
	static constexpr float kStableCanvasSize =
		(kBaseSize + kBaseShadowPad) * kMaximumRackZoom;
	Puffy* module = nullptr;
	PuffyFishWidget* fishWidget = nullptr;
	WeakPtr<PuffyFishWidget> compassWidget;
	Vec velocity{0.f, 0.f};
	Vec anchorPos{0.f, 0.f};
	std::array<double, 25> cellLastVisited {};
	Vec explorationTargetNormalized{0.f, 0.f};
	Vec explorationSteering{0.f, 0.f};
	float explorationTimeRemaining = 0.f;
	float plannerAccumulator = 0.f;
	float bestTargetDistance = INFINITY;
	float noTargetProgressTime = 0.f;
	float lastRangeSetting = -1.f;
	float rangeStableTimer = 0.f;
	bool explorationTargetActive = false;
	bool rangeResetPending = false;
	std::uint32_t rngState = 1u;

	static std::vector<PuffyRoamingOverlay*>& activeOverlays() {
		// Roaming overlays are created, stepped, and destroyed on Rack's UI
		// thread, so this registry needs no locking or audio-thread traffic.
		static std::vector<PuffyRoamingOverlay*> overlays;
		return overlays;
	}

	explicit PuffyRoamingOverlay(
		Puffy* module,
		PuffyFishWidget* compassWidget)
		: module(module), compassWidget(compassWidget) {
		const std::uint64_t moduleId = module && module->id >= 0
			? std::uint64_t(module->id) : 1u;
		rngState = std::uint32_t(moduleId) ^ std::uint32_t(moduleId >> 32)
			^ 0x9e3779b9u;
		if (rngState == 0u) {
			rngState = 1u;
		}
		box.size = Vec(kStableCanvasSize, kStableCanvasSize);
		fishWidget = new PuffyFishWidget(module, true);
		fishWidget->box.size = Vec(
			kBaseSize + kBaseShadowPad, kBaseSize + kBaseShadowPad);
		fishWidget->box.pos = box.size.minus(fishWidget->box.size).mult(0.5f);
		addChild(fishWidget);
		activeOverlays().push_back(this);
	}

	~PuffyRoamingOverlay() override {
		auto& overlays = activeOverlays();
		overlays.erase(
			std::remove(overlays.begin(), overlays.end(), this),
			overlays.end());
	}

	float rackZoom() const {
		return fishWidget
			? fishWidget->box.size.x / (kBaseSize + kBaseShadowPad)
			: 1.f;
	}

	float nextRandom01() {
		rngState ^= rngState << 13;
		rngState ^= rngState >> 17;
		rngState ^= rngState << 5;
		return float(rngState & 0x00ffffffu) / float(0x01000000u);
	}

	int explorationCell(Vec normalizedPosition) const {
		const float radius = normalizedPosition.norm();
		if (radius < 0.18f) {
			return 0;
		}
		const int ring = clamp(int((radius - 0.18f) / 0.22f), 0, 2);
		float angle = std::atan2(normalizedPosition.y, normalizedPosition.x);
		if (angle < 0.f) {
			angle += 2.f * float(M_PI);
		}
		const int sector = clamp(
			int(angle * (8.f / (2.f * float(M_PI)))), 0, 7);
		return 1 + ring * 8 + sector;
	}

	Vec explorationCellCenter(int cell) const {
		if (cell <= 0) {
			return Vec();
		}
		const int index = cell - 1;
		const int ring = index / 8;
		const int sector = index % 8;
		const float radius = 0.29f + 0.22f * float(ring);
		const float angle = (float(sector) + 0.5f)
			* (2.f * float(M_PI) / 8.f);
		return Vec(std::cos(angle), std::sin(angle)).mult(radius);
	}

	void selectExplorationTarget(
		Vec currentNormalized,
		Vec cursorNormalized,
		double now) {
		int bestCell = 0;
		float bestScore = -INFINITY;
		for (int cell = 0; cell < int(cellLastVisited.size()); ++cell) {
			const Vec candidate = explorationCellCenter(cell);
			const double visited = cellLastVisited[size_t(cell)];
			float score = visited <= 0.0
				? 125.f
				: std::min(float(now - visited), 90.f) * 1.45f;
			score -= candidate.minus(currentNormalized).norm() * 18.f;
			const float cursorDistance = candidate.minus(cursorNormalized).norm();
			if (cursorDistance < 0.38f) {
				score -= (0.38f - cursorDistance) * 85.f;
			}
			if (candidate.norm() > 0.72f) {
				score -= 4.f;
			}
			score += nextRandom01() * 9.f;
			if (score > bestScore) {
				bestScore = score;
				bestCell = cell;
			}
		}

		if (bestCell == 0) {
			const float angle = 2.f * float(M_PI) * nextRandom01();
			const float radius = 0.04f + 0.10f * nextRandom01();
			explorationTargetNormalized = Vec(
				std::cos(angle), std::sin(angle)).mult(radius);
		}
		else {
			const int index = bestCell - 1;
			const int ring = index / 8;
			const int sector = index % 8;
			const float radius = 0.19f + 0.22f * float(ring)
				+ 0.20f * nextRandom01();
			const float sectorWidth = 2.f * float(M_PI) / 8.f;
			const float angle = (float(sector) + 0.15f
				+ 0.70f * nextRandom01()) * sectorWidth;
			explorationTargetNormalized = Vec(
				std::cos(angle), std::sin(angle)).mult(
					std::min(radius, 0.84f));
		}
		explorationTargetActive = true;
		explorationTimeRemaining = 3.5f + 3.5f * nextRandom01();
		bestTargetDistance = INFINITY;
		noTargetProgressTime = 0.f;
	}

	Vec avatarCenter() const {
		return box.pos.plus(fishWidget->box.pos).plus(
			fishWidget->box.size.mult(0.5f));
	}

	void publishSpatialPosition(bool publishDirection = true) {
		if (!module) {
			return;
		}
		Vec center = avatarCenter();
		if (fishWidget) {
			center = box.pos.plus(fishWidget->box.pos).plus(
				fishWidget->visibleBodyCenter());
		}
		module->roamingTargetX.store(center.x, std::memory_order_relaxed);
		module->roamingTargetY.store(center.y, std::memory_order_relaxed);
		if (publishDirection) {
			Vec compassCenter = anchorPos;
			if (PuffyFishWidget* compass = compassWidget.get()) {
				compassCenter = APP && APP->scene
					? compass->getRelativeOffset(
						compass->compassCenter(), APP->scene)
					: compass->compassCenter();
			}
			module->roamingDirectionAngle.store(
				std::atan2(
					center.y - compassCenter.y,
					center.x - compassCenter.x),
				std::memory_order_release);
		}
	}

	void setRackZoom(float zoom) {
		if (!fishWidget) {
			return;
		}
		zoom = std::isfinite(zoom)
			? clamp(zoom, 0.05f, kMaximumRackZoom) : 1.f;
		const Vec nextSize(
			(kBaseSize + kBaseShadowPad) * zoom,
			(kBaseSize + kBaseShadowPad) * zoom);
		if (nextSize == fishWidget->box.size) {
			return;
		}
		const Vec center = avatarCenter();
		fishWidget->box.size = nextSize;
		fishWidget->box.pos = box.size.minus(nextSize).mult(0.5f);
		box.pos = center.minus(fishWidget->box.pos).minus(
			fishWidget->box.size.mult(0.5f));
	}

	void draw(const DrawArgs& args) override {
		if (fishWidget) {
			nvgSave(args.vg);
			nvgTranslate(
				args.vg, fishWidget->box.pos.x, fishWidget->box.pos.y);
			fishWidget->drawRoamingDropShadow(args.vg);
			nvgRestore(args.vg);
		}
		TransparentWidget::draw(args);
	}

	void step() override {
		TransparentWidget::step();
		if (!module || !APP || !APP->scene || !APP->scene->rack) return;
		
		Vec mousePos = APP->scene->getMousePos();
		Vec center = avatarCenter();
		float dt = APP->window ? APP->window->getLastFrameDuration() : 1.f/60.f;
		if (dt > 0.1f) dt = 0.1f;
		const float zoomScale = rackZoom();
		Vec toMouse = mousePos.minus(center);
		float distToMouse = toMouse.norm();

		Vec acceleration{0.f, 0.f};

		// 1. Flee mouse
		// Mouse and avatar positions are in scene pixels. Scale both the radius
		// and acceleration so avoidance has the same Rack-space behavior at every
		// zoom level.
		const float fleeRadius = 340.f * zoomScale;
		if (distToMouse < fleeRadius && distToMouse > 0.001f) {
			// Use a quadratic falloff: gentle push at the edges, strong darting when very close
			float normalizedDist = 1.f - (distToMouse / fleeRadius);
			float force = normalizedDist * normalizedDist * 1500.f * zoomScale;
			acceleration = acceleration.plus(toMouse.normalize().mult(-force));
		}

		// Nearby roaming Puffies gently separate. The influence scales with the
		// rendered bodies, fades smoothly to zero, and is capped so a crowd cannot
		// overpower cursor avoidance or launch an avatar across the rack.
		Vec separationAcceleration{0.f, 0.f};
		const float ownRadius = 0.5f * kBaseSize * rackZoom();
		for (PuffyRoamingOverlay* other : activeOverlays()) {
			if (!other || other == this || !other->module
				|| !other->module->roamingEnabled.load(std::memory_order_relaxed)) {
				continue;
			}
			Vec away = center.minus(other->avatarCenter());
			float distance = away.norm();
			const float otherRadius =
				0.5f * kBaseSize * other->rackZoom();
			const float influenceRadius =
				1.65f * (ownRadius + otherRadius);
			if (distance >= influenceRadius) {
				continue;
			}
			if (distance <= 0.001f) {
				// Give a perfectly overlapping pair stable opposite directions.
				const std::uint64_t ownId = module && module->id >= 0
					? std::uint64_t(module->id) : 0u;
				const std::uint64_t otherId = other->module->id >= 0
					? std::uint64_t(other->module->id) : 0u;
				const std::uint64_t low = std::min(ownId, otherId);
				const std::uint64_t high = std::max(ownId, otherId);
				const float angle = float((low * 1103515245u + high * 12345u)
					& 0xffffu) * (2.f * float(M_PI) / 65536.f);
				away = Vec(std::cos(angle), std::sin(angle));
				if (ownId > otherId) {
					away = away.mult(-1.f);
				}
				distance = 0.f;
			}
			else {
				away = away.div(distance);
			}
			const float proximity = clamp(
				1.f - distance / std::max(influenceRadius, 1.f), 0.f, 1.f);
			const float smoothProximity =
				proximity * proximity * (3.f - 2.f * proximity);
			separationAcceleration = separationAcceleration.plus(
				away.mult(440.f * smoothProximity));
		}
		const float separationMagnitude = separationAcceleration.norm();
		if (separationMagnitude > 520.f) {
			separationAcceleration = separationAcceleration.mult(
				520.f / separationMagnitude);
		}
		acceleration = acceleration.plus(separationAcceleration);

		// 2. Wander
		// Offset the time using the module's ID so multiple puffies don't swim in sync
		float time = system::getTime() + module->id * 12.34f;
		acceleration.x += std::sin(time * 2.1f) * 90.f;
		acceleration.y += std::cos(time * 1.7f) * 90.f;

		// 3. Bungee
		Vec toAnchor = anchorPos.minus(center);
		float distToAnchor = toAnchor.norm();
		const float rangeSetting = clamp(
			module->params[Puffy::ROAMING_RANGE_PARAM].getValue(), 0.f, 1.f);
		float maxDist = crossfade(90.f, 900.f, rangeSetting) * zoomScale;
		const float distanceRatio = clamp(
			distToAnchor / std::max(maxDist, 1.f), 0.f, 1.f);
		module->roamingDistance.store(
			distToAnchor / std::max(zoomScale, 0.05f),
			std::memory_order_relaxed);

		// Normalize navigation against the user-selected range. The visitation
		// map therefore survives zoom changes without remapping scene pixels.
		const Vec currentNormalized = center.minus(anchorPos).div(
			std::max(maxDist, 1.f));
		const Vec cursorNormalized = mousePos.minus(anchorPos).div(
			std::max(maxDist, 1.f));
		const double now = system::getTime();

		// Debounce range edits. The active target scales continuously during a
		// drag; once stable, history resets so newly exposed space is unexplored.
		if (lastRangeSetting < 0.f) {
			lastRangeSetting = rangeSetting;
		}
		else if (std::fabs(rangeSetting - lastRangeSetting) > 0.003f) {
			lastRangeSetting = rangeSetting;
			rangeStableTimer = 0.25f;
			rangeResetPending = true;
		}
		if (rangeResetPending) {
			rangeStableTimer -= dt;
			if (rangeStableTimer <= 0.f) {
				cellLastVisited.fill(0.0);
				explorationTargetActive = false;
				rangeResetPending = false;
			}
		}

		// Planner bookkeeping runs at only 10 Hz. Steady-frame steering below is
		// O(1); the 25-cell scan occurs only when choosing a new destination.
		plannerAccumulator += dt;
		if (plannerAccumulator >= 0.1f) {
			plannerAccumulator = std::fmod(plannerAccumulator, 0.1f);
			cellLastVisited[size_t(explorationCell(currentNormalized))] = now;
		}

		explorationTimeRemaining -= dt;
		if (!explorationTargetActive && !rangeResetPending
			&& distanceRatio < 0.96f) {
			selectExplorationTarget(currentNormalized, cursorNormalized, now);
		}
		Vec desiredExplorationSteering{0.f, 0.f};
		if (explorationTargetActive) {
			const Vec targetScene = anchorPos.plus(
				explorationTargetNormalized.mult(maxDist));
			const Vec toTarget = targetScene.minus(center);
			const float targetDistance = toTarget.norm();
			if (targetDistance + 5.f * zoomScale < bestTargetDistance) {
				bestTargetDistance = targetDistance;
				noTargetProgressTime = 0.f;
			}
			else {
				noTargetProgressTime += dt;
			}
			const float reachedDistance = std::max(
				22.f * zoomScale, 0.055f * maxDist);
			if (targetDistance < reachedDistance
				|| explorationTimeRemaining <= 0.f
				|| noTargetProgressTime > 2.4f) {
				explorationTargetActive = false;
			}
			else if (distanceRatio < 1.05f && targetDistance > 0.001f) {
				float steeringForce = 155.f
					+ 145.f * explorationTargetNormalized.norm();
				if (noTargetProgressTime > 1.4f) {
					steeringForce *= 1.18f;
				}
				desiredExplorationSteering =
					toTarget.normalize().mult(steeringForce);
			}
		}
		// Destination selection may rotate the desired force sharply. Smooth the
		// steering vector itself so Puffy curves toward a new region instead of
		// visibly changing course at the selection boundary.
		const float steeringApproach = 1.f - std::exp(-2.2f * dt);
		explorationSteering = explorationSteering.plus(
			desiredExplorationSteering.minus(explorationSteering)
				.mult(steeringApproach));
		acceleration = acceleration.plus(explorationSteering);

		if (distToAnchor > 0.001f) {
			const Vec inward = toAnchor.normalize();
			// Begin with an almost imperceptible bias in the middle of the
			// roaming area, then increasingly favor homeward travel toward the
			// boundary. This prevents wander/damping equilibria from camping at
			// maximum range while retaining free motion near the module.
			const float softBias = clamp(
				(distanceRatio - 0.35f) / 0.65f, 0.f, 1.f);
			const float softForce = 240.f * softBias * softBias;
			acceleration = acceleration.plus(inward.mult(softForce));
			if (distToAnchor > maxDist) {
				const float boundaryForce = (distToAnchor - maxDist) * 10.f;
				acceleration = acceleration.plus(inward.mult(boundaryForce));
			}
		}
		// Reuse the already-computed force magnitude as a cheap visual control.
		// Ignore gentle cruising forces, then ramp toward the fast flap rate.
		module->roamingMovementAcceleration.store(
			clamp((acceleration.norm() - 90.f) / 850.f, 0.f, 1.f),
			std::memory_order_relaxed);

		velocity = velocity.plus(acceleration.mult(dt));
		velocity = velocity.mult(std::pow(0.05f, dt));
		
		box.pos = box.pos.plus(velocity.mult(dt));
		// Position follows movement immediately. Direction is published once by
		// PuffyWidget after all Rack zoom geometry is coherent for the frame.
		publishSpatialPosition(false);
	}
};

PuffyWidget::~PuffyWidget() {
	stopDrawLog();
	retireRoamingOverlay();
}

void PuffyWidget::retireRoamingOverlay() {
	if (auto* puffyModule = dynamic_cast<Puffy*>(module)) {
		puffyModule->roamingAvatarActive.store(false, std::memory_order_release);
		puffyModule->roamingDistance.store(0.f, std::memory_order_relaxed);
	}
	if (roamingOverlay) {
		PuffyRoamingOverlay* overlay = roamingOverlay.get();
		// The overlay lives at scene level rather than below this ModuleWidget.
		// Quarantine it immediately so a retired DAW editor scene cannot keep
		// publishing state through a surviving module pointer.
		overlay->module = nullptr;
		overlay->compassWidget.set(nullptr);
		overlay->setVisible(false);
		if (overlay->parent) {
			overlay->requestDelete();
		}
		else {
			delete overlay;
		}
		roamingOverlay.set(nullptr);
	}
	roamingAttachStableFrames = 0u;
}

void PuffyWidget::onContextDestroy(const ContextDestroyEvent& e) {
	// Rack's DAW build may retain ModuleWidgets while replacing the editor
	// scene. A scene-level roaming overlay must never cross that boundary.
	retireRoamingOverlay();
	ModuleWidget::onContextDestroy(e);
}

void PuffyWidget::step() {
	const bool measurePerf = isDragonKingDebugEnabled();
	const auto stepStart = debug_terminal::debugTimerStart(measurePerf);
	ModuleWidget::step();
	
	auto* puffyModule = dynamic_cast<Puffy*>(module);
	if (puffyModule
		&& puffyModule->params[Puffy::CHARACTER_LINK_PARAM].getValue() > 0.5f
		&& puffyModule->params[Puffy::POSITIVE_CHARACTER_PARAM].getValue()
			!= puffyModule->params[Puffy::CHARACTER_PARAM].getValue()) {
		// Keep the cosmetic positive selector synchronized from the UI thread.
		// DSP already derives the positive character directly while linked.
		puffyModule->synchronizeCharacterSelectionFromUi(true);
	}
	
	auto* scene = APP ? APP->scene : nullptr;
	auto* rack = scene ? scene->rack : nullptr;
	// During patch restoration Rack can temporarily wrap/reparent module
	// widgets. Requiring the immediate parent to be moduleContainer made the
	// roaming overlay creation a one-shot failure on some load paths.
	bool validRackContext = puffyModule && scene && rack && isDescendantOf(rack);

	if (roamingOverlay) {
		PuffyRoamingOverlay* overlay = roamingOverlay.get();
		const bool belongsToCurrentScene = scene && overlay && overlay->parent
			&& overlay->isDescendantOf(scene);
		if (!belongsToCurrentScene) {
			retireRoamingOverlay();
		}
	}

	if (validRackContext) {
		bool wantsRoaming = puffyModule->roamingEnabled.load(std::memory_order_relaxed);
		if (wantsRoaming && !roamingOverlay) {
			puffyModule->roamingAvatarActive.store(
				false, std::memory_order_release);
			// Patch restoration can discard scene children installed during its
			// first traversal. Wait until this widget has remained attached for a
			// couple of complete UI frames, then install the avatar at scene level.
			if (++roamingAttachStableFrames >= 3u) {
				auto* overlay = new PuffyRoamingOverlay(
					puffyModule, panelFishWidget.get());
				overlay->setRackZoom(getRelativeZoom(scene));
				overlay->anchorPos = getRelativeOffset(
					box.size.mult(0.5f), scene);
				overlay->box.pos = overlay->anchorPos
					.minus(overlay->fishWidget->box.pos)
					.minus(overlay->fishWidget->box.size.mult(0.5f));
				// Scene-level placement keeps Puffy in front of module-local UI and
				// rack overlays, while still leaving Rack's menu bar unobstructed.
				if (scene->menuBar && scene->hasChild(scene->menuBar)) {
					scene->addChildBelow(overlay, scene->menuBar);
				}
				else {
					scene->addChild(overlay);
				}
				roamingOverlay.set(overlay);
				overlay->publishSpatialPosition();
				puffyModule->roamingAvatarActive.store(
					true, std::memory_order_release);
			}
		}
		else if (wantsRoaming && roamingOverlay) {
			roamingAttachStableFrames = 3u;
			PuffyRoamingOverlay* overlay = roamingOverlay.get();
			const float oldZoom = std::max(overlay->rackZoom(), 0.05f);
			const float nextZoom = clamp(
				getRelativeZoom(scene), 0.05f, 8.f);
			const Vec logicalOffset = overlay->avatarCenter()
				.minus(overlay->anchorPos).div(oldZoom);
			overlay->setRackZoom(nextZoom);
			const Vec nextAnchor = getRelativeOffset(
				box.size.mult(0.5f), scene);
			// Preserve roaming state in Rack-space units. Scaling only the range
			// boundary made zoom changes alter the normalized displacement and
			// briefly kick the bungee.
			const Vec nextCenter = nextAnchor.plus(
				logicalOffset.mult(nextZoom));
			overlay->box.pos = overlay->box.pos.plus(
				nextCenter.minus(overlay->avatarCenter()));
			overlay->velocity = overlay->velocity.mult(nextZoom / oldZoom);
			overlay->anchorPos = nextAnchor;
			overlay->publishSpatialPosition();
		}
		else if (!wantsRoaming && roamingOverlay) {
			retireRoamingOverlay();
		}
		else if (!wantsRoaming) {
			roamingAttachStableFrames = 0u;
			puffyModule->roamingAvatarActive.store(
				false, std::memory_order_release);
		}
	}
	
	if (measurePerf) {
		debugWidgetMetrics.recordStep(
			debug_terminal::elapsedUsSince(stepStart));
	}
}

void PuffyWidget::draw(const DrawArgs& args) {
	using PerfClock = std::chrono::steady_clock;
	auto* puffyModule = static_cast<Puffy*>(module);
	const bool logDraw = puffyModule
		&& isDragonKingDebugEnabled()
		&& isPuffyDrawLoggingEnabled();
	const std::uint32_t instanceId = puffyModule
		? puffyModule->debugMetrics.instanceId : 0u;
	syncDrawLog(logDraw, instanceId);
	const PerfClock::time_point totalStart = logDraw
		? PerfClock::now() : PerfClock::time_point();
	const bool measurePerf = isDragonKingDebugEnabled();
	const auto drawStart = debug_terminal::debugTimerStart(measurePerf);
	ModuleWidget::draw(args);
	const PerfClock::time_point afterModuleDraw = logDraw
		? PerfClock::now() : PerfClock::time_point();
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
	if (logDraw && drawLogActive && drawLogFile.is_open()) {
		const PuffyDrawMetrics metrics = consumePuffyDrawMetrics();
		PuffyVisualState visual;
		puffyModule->readVisualState(&visual);
		const float fishMeasuredUs = float(
			metrics.bodyEnsureNs
			+ metrics.bodyDrawNs
			+ metrics.bodyTransitionDrawNs
			+ metrics.bodyTransitionAtlasPrewarmNs
			+ metrics.finDrawNs
			+ metrics.eyeDrawNs) * 1e-3f;
		const float fishDrawUs = float(metrics.fishDrawNs) * 1e-3f;
		const double lastFrameSec = APP && APP->window
			? APP->window->getLastFrameDuration() : 0.0;
		drawLogFile
			<< drawLogRowCounter << ','
			<< puffyModule->id << ','
			<< instanceId << ','
			<< system::getTime() << ','
			<< lastFrameSec * 1e6 << ','
			<< float(std::chrono::duration_cast<std::chrono::nanoseconds>(
				PerfClock::now() - totalStart).count()) * 1e-3f << ','
			<< float(std::chrono::duration_cast<std::chrono::nanoseconds>(
				afterModuleDraw - totalStart).count()) * 1e-3f << ','
			<< fishDrawUs << ','
			<< std::max(0.f, fishDrawUs - fishMeasuredUs) << ','
			<< float(metrics.bodyEnsureNs) * 1e-3f << ','
			<< float(metrics.bodyRecolorNs) * 1e-3f << ','
			<< float(metrics.bodyUploadNs) * 1e-3f << ','
			<< float(metrics.bodyDrawNs) * 1e-3f << ','
			<< float(metrics.bodyTransitionDrawNs) * 1e-3f << ','
			<< float(metrics.bodyTransitionAtlasPrewarmNs) * 1e-3f << ','
			<< float(metrics.finDrawNs) * 1e-3f << ','
			<< float(metrics.eyeDrawNs) * 1e-3f << ','
			<< float(metrics.transferDrawNs) * 1e-3f << ','
			<< float(metrics.transferCurveDrawNs) * 1e-3f << ','
			<< float(metrics.transferCurveRebuildNs) * 1e-3f << ','
			<< metrics.bodyCacheHits << ','
			<< metrics.bodyRecolors << ','
			<< metrics.bodyImageCreates << ','
			<< metrics.bodyImageUpdates << ','
			<< metrics.bodyContextResets << ','
			<< metrics.bodyFallbackDraws << ','
			<< metrics.bodyTransitionDraws << ','
			<< metrics.bodyTransitionAtlasCreates << ','
			<< metrics.bodyTransitionAtlasResets << ','
			<< metrics.bodyTransitionAtlasPrewarms << ','
			<< metrics.transferCurveRebuilds << ','
			<< visual.negativeCharacter << ','
			<< visual.positiveCharacter << ','
			<< visual.effectiveAmount << ','
			<< visual.inputActivity << ','
			<< visual.gainReduction << '\n';
		if ((drawLogRowCounter++ & 31u) == 0u) {
			drawLogFile.flush();
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
	panelFishWidget.set(fish);
	fish->box.pos = mm2px(fishContentRectMm.pos);
	fish->box.size = mm2px(fishContentRectMm.size);
	addChild(fish);

	auto* roamingRange = createParam<PuffyRoamingRangeBar>(
		Vec(), module, Puffy::ROAMING_RANGE_PARAM);
	roamingRange->box.size = Vec(fish->box.size.x * 0.96f, 28.f);
	roamingRange->box.pos = fish->box.pos.plus(Vec(
		fish->box.size.x * 0.02f, 1.f));
	addParam(roamingRange);

	math::Rect negativeCharacterPillMm(
		Vec(1.5369994f, 58.077893f), Vec(13.5f, 4.5f));
	panel_svg::loadRectFromSvgMm(
		panelPath, "negative_character_pillbox", &negativeCharacterPillMm);
	auto* negativeCharacterMenu = createParam<PuffyCharacterMenuButton>(
		Vec(), module, Puffy::CHARACTER_PARAM);
	negativeCharacterMenu->negativePart = true;
	negativeCharacterMenu->box.pos = mm2px(negativeCharacterPillMm.pos);
	negativeCharacterMenu->box.size = mm2px(negativeCharacterPillMm.size);
	addParam(negativeCharacterMenu);

	auto* polarityLinkIcon = createParam<PuffyPolarityLinkButton>(
		anchor("polarity_link_button", Vec(4.2463326f, 55.029892f))
			.minus(Vec(8.f)),
		module,
		Puffy::CHARACTER_LINK_PARAM);
	addParam(polarityLinkIcon);

	math::Rect positiveCharacterPillMm(
		Vec(45.921999f, 58.077893f), Vec(13.5f, 4.5f));
	panel_svg::loadRectFromSvgMm(
		panelPath, "positive_character_pillbox", &positiveCharacterPillMm);
	auto* positiveCharacterMenu = createParam<PuffyCharacterMenuButton>(
		Vec(), module, Puffy::POSITIVE_CHARACTER_PARAM);
	positiveCharacterMenu->negativePart = false;
	positiveCharacterMenu->box.pos = mm2px(positiveCharacterPillMm.pos);
	positiveCharacterMenu->box.size = mm2px(positiveCharacterPillMm.size);
	addParam(positiveCharacterMenu);

	// Puffy's visible presence in the rack is the roaming-state indicator.
	// Keep this as a tactile latching button without a redundant LED.
	auto* roamingButton = createParamCentered<PuffyRoamingButton>(
		anchor("roaming_button", Vec(56.758f, 55.537893f)),
		module, Puffy::ROAMING_BUTTON_PARAM);
	addParam(roamingButton);

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
		anchor("input_l", Vec(6.5f, 111.93589f)),
		module, Puffy::INPUT_L));
	addInput(createInputCentered<Magitek2InputJack>(
		anchor("input_r", Vec(20.5f, 111.93589f)),
		module, Puffy::INPUT_R));
	addInput(createInputCentered<Magitek2InputJack>(
		anchor("puff_cv_input", Vec(43.f, 108.f)),
		module, Puffy::PUFF_CV_INPUT));
	addOutput(createOutputCentered<Magitek2OutputJack>(
		anchor("output_l", Vec(40.5f, 111.93589f)),
		module, Puffy::OUTPUT_L));
	addOutput(createOutputCentered<Magitek2OutputJack>(
		anchor("output_r", Vec(54.5f, 111.93589f)),
		module, Puffy::OUTPUT_R));

	addChild(createLightCentered<SmallAperture<RedApertureLight>>(
		anchor("limit_light", Vec(47.5f, 111.93589f)),
		module, Puffy::LIMIT_LIGHT));

	// The bottom-row black channel visually places the limiter between Puffy's
	// input and output groups. The master-SVG anchor matches the button's center
	// and radius; every part of the cluster is derived from that center.
	const Vec limiterGroupAnchor = anchor(
		"limiter_mode_group", Vec(30.61f, 118.34f));
	const float limiterButtonSize = 15.f;
	const float limiterRight = limiterGroupAnchor.x + 0.5f * limiterButtonSize;
	const float limiterLedX = limiterRight - mm2px(1.15f);
	const float limiterRowSpacing = mm2px(3.f);
	const float limiterFirstY = limiterGroupAnchor.y
		- 0.5f * limiterButtonSize - mm2px(7.8f);
	auto* limiterLabels = new PuffyLimiterModeLabels();
	limiterLabels->box.pos = Vec(
		limiterLedX - mm2px(4.f), limiterFirstY - 0.5f * limiterRowSpacing);
	limiterLabels->box.size = Vec(mm2px(2.9f), 3.f * limiterRowSpacing);
	addChild(limiterLabels);
	addChild(createLightCentered<TinyAperture<RedApertureLight>>(
		Vec(limiterLedX, limiterFirstY), module, Puffy::LIMITER_HARD_LIGHT));
	addChild(createLightCentered<TinyAperture<AmberApertureLight>>(
		Vec(limiterLedX, limiterFirstY + limiterRowSpacing),
		module, Puffy::LIMITER_SOFT_LIGHT));
	addChild(createLightCentered<TinyAperture<WhiteApertureLight>>(
		Vec(limiterLedX, limiterFirstY + 2.f * limiterRowSpacing),
		module, Puffy::LIMITER_OFF_LIGHT));
	addParam(createParam<PuffyLimiterModeButton>(
		limiterGroupAnchor.minus(Vec(0.5f * limiterButtonSize)),
		module, Puffy::LIMITER_BUTTON_PARAM));

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
		"Roaming Mode",
		"",
		[puffyModule]() {
			return puffyModule->roamingEnabled.load(
				std::memory_order_relaxed);
		},
		[puffyModule]() {
			const bool enabled = puffyModule->roamingEnabled.load(
				std::memory_order_relaxed);
			puffyModule->params[Puffy::ROAMING_BUTTON_PARAM].setValue(
				enabled ? 0.f : 1.f);
		}));

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
