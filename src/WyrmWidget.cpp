#include "Wyrm.hpp"
#include "WyrmSand.hpp"
#include "PanelSvgUtils.hpp"
#include "visual/VisualAssets.hpp"
#include "visual/FractalGlassOverlay.hpp"

#include <algorithm>
#include <cstdio>
#include <functional>

void drawWyrmStepTriangle(const Widget::DrawArgs& args, const Vec& size, bool pointRight) {
	const float cx = 0.5f * size.x;
	const float cy = 0.5f * size.y;
	const float halfW = 2.8f;
	const float halfH = 3.3f;
	const float offset = pointRight ? (halfW / 3.f) : (-halfW / 3.f);
	nvgBeginPath(args.vg);
	if (pointRight) {
		nvgMoveTo(args.vg, cx - halfW + offset, cy - halfH);
		nvgLineTo(args.vg, cx + halfW + offset, cy);
		nvgLineTo(args.vg, cx - halfW + offset, cy + halfH);
	}
	else {
		nvgMoveTo(args.vg, cx + halfW + offset, cy - halfH);
		nvgLineTo(args.vg, cx - halfW + offset, cy);
		nvgLineTo(args.vg, cx + halfW + offset, cy + halfH);
	}
	nvgClosePath(args.vg);
	nvgFillColor(args.vg, nvgRGBA(225, 232, 240, 244));
	nvgFill(args.vg);
}

struct WyrmWaveLeftButton final : TL1105 {
	Wyrm* module = nullptr;
	void onButton(const event::Button& e) override {
		TL1105::onButton(e);
		if (module && e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
			const int current = clamp(module->selectedShape, 0, SHAPE_COUNT - 1);
			const int next = (current + SHAPE_COUNT - 1) % SHAPE_COUNT;
			module->setFactoryShape(next);
		}
	}
	void draw(const DrawArgs& args) override {
		TL1105::draw(args);
		drawWyrmStepTriangle(args, box.size, false);
	}
};

struct WyrmWaveRightButton final : TL1105 {
	Wyrm* module = nullptr;
	void onButton(const event::Button& e) override {
		TL1105::onButton(e);
		if (module && e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
			const int current = clamp(module->selectedShape, 0, SHAPE_COUNT - 1);
			const int next = (current + 1) % SHAPE_COUNT;
			module->setFactoryShape(next);
		}
	}
	void draw(const DrawArgs& args) override {
		TL1105::draw(args);
		drawWyrmStepTriangle(args, box.size, true);
	}
};

struct WyrmShapeMenuItem : MenuItem {
	Wyrm* module = nullptr;
	int shape = SHAPE_SINE;

	void onAction(const event::Action& e) override {
		if (module) module->setFactoryShape(shape);
		MenuItem::onAction(e);
	}

	void step() override {
		rightText = (module && module->selectedShape == shape) ? "✓" : "";
		MenuItem::step();
	}
};

struct WyrmPointCountMenuItem : MenuItem {
	Wyrm* module = nullptr;
	int count = kWyrmPointCountDefault;

	void onAction(const event::Action& e) override {
		if (module) {
			module->setPointCount(count);
		}
		MenuItem::onAction(e);
	}

	void step() override {
		rightText = (module && module->pointCount == count) ? "✓" : "";
		MenuItem::step();
	}
};

struct WyrmFrequencyReadoutWidget final : Widget {
	Wyrm* module = nullptr;

	static std::string formatFrequencyText(float hz) {
		if (!std::isfinite(hz) || hz < 0.f) {
			hz = 0.f;
		}
		if (hz < 1.f) {
			return string::f("%.1f mHz", hz * 1000.f);
		}
		if (hz >= 1000.f) {
			return string::f("%.2f kHz", hz / 1000.f);
		}
		if (hz < 10.f) {
			return string::f("%.2f Hz", hz);
		}
		if (hz < 100.f) {
			return string::f("%.1f Hz", hz);
		}
		return string::f("%.0f Hz", hz);
	}

	static std::string formatEnvelopeTimeText(float ms) {
		if (!std::isfinite(ms) || ms < 0.f) {
			ms = 0.f;
		}
		if (ms < 10.f) {
			return string::f("%.2f ms", ms);
		}
		if (ms < 100.f) {
			return string::f("%.1f ms", ms);
		}
		return string::f("%.0f ms", ms);
	}

	void draw(const DrawArgs& args) override {
		if (!module || !APP || !APP->window || !APP->window->uiFont) {
			return;
		}
		nvgFontSize(args.vg, std::max(9.5f, box.size.y * 0.72f));
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 255));
		nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		std::string readoutText;
		if (module->envelopeMode.load(std::memory_order_relaxed)) {
			const float timeMs = module->displayEnvelopeTimeMs.load(std::memory_order_relaxed);
			readoutText = string::f("Time: %s", formatEnvelopeTimeText(timeMs).c_str());
		}
		else if (module->waveCustomized) {
			const float displayHz = module->displayFrequencyHz.load(std::memory_order_relaxed);
			const std::string freqText = formatFrequencyText(displayHz);
			readoutText = string::f("Custom: %s", freqText.c_str());
		}
		else {
			const float displayHz = module->displayFrequencyHz.load(std::memory_order_relaxed);
			const std::string freqText = formatFrequencyText(displayHz);
			const int shapeIndex = clamp(module->selectedShape, 0, SHAPE_COUNT - 1);
			readoutText = string::f("%s: %s", kWyrmShapeLabels[shapeIndex], freqText.c_str());
		}
		nvgText(args.vg, 0.5f * box.size.x, 0.5f * box.size.y, readoutText.c_str(), nullptr);
	}
};

struct WyrmVoctModeLabelWidget final : Widget {
	Wyrm* module = nullptr;

	void draw(const DrawArgs& args) override {
		if (!APP || !APP->window || !APP->window->uiFont) {
			return;
		}
		const bool envelopeMode = module && module->envelopeMode.load(std::memory_order_relaxed);
		nvgFontSize(args.vg, std::max(9.5f, box.size.y * 0.72f));
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 255));
		nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		nvgText(args.vg, 0.5f * box.size.x, 0.5f * box.size.y + mm2px(0.3f),
			envelopeMode ? "TRIG" : "V/OCT", nullptr);
	}
};

struct WyrmEditorSurface final : Widget {
	Wyrm* module = nullptr;
	std::shared_ptr<WyrmSand> sandState;
	Widget* sandGlWidget = nullptr;
	widget::FramebufferWidget* editorFramebuffer = nullptr;
	TransparentWidget* waveEditor = nullptr;

	explicit WyrmEditorSurface(Wyrm* m)
		: module(m)
		, sandState(std::make_shared<WyrmSand>()) {
		sandGlWidget = createWyrmSandGlWidget(module, sandState);
		addChild(sandGlWidget);
		waveEditor = createWyrmWaveEditor(module, sandState);
		editorFramebuffer = new widget::FramebufferWidget();
		editorFramebuffer->dirtyOnSubpixelChange = false;
		editorFramebuffer->addChild(waveEditor);
		addChild(editorFramebuffer);
	}

	void setEditorSize(Vec size) {
		size.x = std::max(1.f, size.x);
		size.y = std::max(1.f, size.y);
		setSize(size);
		sandGlWidget->setPosition(Vec());
		sandGlWidget->setSize(size);
		editorFramebuffer->setPosition(Vec());
		editorFramebuffer->setSize(size);
		waveEditor->setPosition(Vec());
		waveEditor->setSize(size);
		editorFramebuffer->setDirty();
	}

	void resetVisualTransitionState() {
		if (sandState) {
			sandState->resetHistory();
		}
		if (editorFramebuffer) {
			editorFramebuffer->setDirty();
		}
	}
};

struct WyrmEditorDock final : Widget {
	bool expanded = false;

	void draw(const DrawArgs& args) override {
		if (expanded) {
			const float radius = std::min(7.f, 0.08f * box.size.y);
			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, 0.f, 0.f, box.size.x, box.size.y, radius);
			nvgFillColor(args.vg, nvgRGBA(4, 8, 22, 205));
			nvgFill(args.vg);
			nvgStrokeColor(args.vg, nvgRGBA(108, 89, 205, 115));
			nvgStrokeWidth(args.vg, 1.f);
			nvgStroke(args.vg);
		}
		Widget::draw(args);
	}
};

struct WyrmAnchoredTooltip final : ui::Tooltip {
	WeakPtr<Widget> anchor;

	void step() override {
		ui::Tooltip::step();
		Widget* anchorWidget = anchor.get();
		if (!anchorWidget || !APP || !APP->scene) {
			if (parent) requestDelete();
			return;
		}

		const float anchorZoom = std::max(anchorWidget->getAbsoluteZoom(), 1e-6f);
		const Vec anchorOrigin = anchorWidget->getAbsoluteOffset(Vec());
		const Vec anchorSize = anchorWidget->box.size.mult(anchorZoom);
		const Vec sceneSize = APP->scene->box.size;
		const float margin = 4.f;
		float top = margin;
		if (APP->scene->menuBar && APP->scene->menuBar->isVisible()) {
			top = std::max(top,
				APP->scene->menuBar->box.pos.y + APP->scene->menuBar->box.size.y + margin);
		}

		const float desiredX = anchorOrigin.x + anchorSize.x + 2.f;
		const float desiredY = anchorOrigin.y + anchorSize.y + 2.f;
		const float maxX = std::max(margin, sceneSize.x - margin - box.size.x);
		const float maxY = std::max(top, sceneSize.y - margin - box.size.y);
		setPosition(Vec(
			clamp(desiredX, margin, maxX),
			clamp(desiredY, top, maxY)));
	}
};

template <typename BaseButton>
struct WyrmTooltipButton : BaseButton {
	std::function<std::string()> tooltipTextProvider;
	WeakPtr<ui::Tooltip> tooltip;

	~WyrmTooltipButton() override {
		destroyTooltip();
	}

	std::string tooltipText() const {
		return tooltipTextProvider ? tooltipTextProvider() : std::string();
	}

	void createTooltip() {
		if (!settings::tooltips || tooltip || !APP || !APP->scene) return;
		auto* nextTooltip = new WyrmAnchoredTooltip();
		nextTooltip->text = tooltipText();
		nextTooltip->anchor.set(this);
		tooltip.set(nextTooltip);
		APP->scene->addChild(nextTooltip);
	}

	void destroyTooltip() {
		ui::Tooltip* currentTooltip = tooltip.get();
		if (!currentTooltip) return;
		if (currentTooltip->parent) {
			currentTooltip->parent->removeChild(currentTooltip);
		}
		delete currentTooltip;
		tooltip.set(nullptr);
	}

	void refreshTooltip() {
		if (ui::Tooltip* currentTooltip = tooltip.get()) {
			currentTooltip->text = tooltipText();
		}
	}

	void onButton(const event::Button& e) override {
		// These are callback-only UI controls, not engine parameters. Do not let
		// TL1105's ParamWidget path try to build a parameter context menu.
		if (e.button != GLFW_MOUSE_BUTTON_LEFT) {
			e.consume(this);
			return;
		}
		BaseButton::onButton(e);
		if (e.action == GLFW_PRESS) {
			refreshTooltip();
		}
	}

	void onEnter(const event::Enter& e) override {
		BaseButton::onEnter(e);
		createTooltip();
	}

	void onLeave(const event::Leave& e) override {
		BaseButton::onLeave(e);
		destroyTooltip();
	}

	void step() override {
		BaseButton::step();
		refreshTooltip();
	}
};

struct WyrmEditorGlyphButton final : WyrmTooltipButton<LeviathanIconButton> {
	std::function<bool()> enabledAction;
	std::function<bool()> collapseGlyphAction;
	bool collapseGlyph = false;

	bool enabled() const {
		return !enabledAction || enabledAction();
	}

	void onButton(const event::Button& e) override {
		if (!enabled() && e.button == GLFW_MOUSE_BUTTON_LEFT) {
			e.consume(this);
			return;
		}
		WyrmTooltipButton<LeviathanIconButton>::onButton(e);
	}

	void draw(const DrawArgs& args) override {
		LeviathanIconButton::draw(args);
		const bool active = enabled();
		const bool collapse = collapseGlyphAction ? collapseGlyphAction() : collapseGlyph;
		const Vec center = box.size.mult(0.5f);
		const float direction = collapse ? -1.f : 1.f;
		const float innerX = 0.075f * box.size.x;
		const float outerX = 0.30f * box.size.x;
		const float head = std::max(2.f, 0.11f * box.size.y);
		nvgBeginPath(args.vg);
		for (float side : {-1.f, 1.f}) {
			const float startX = center.x + side * (collapse ? outerX : innerX);
			const float endX = center.x + side * (collapse ? innerX : outerX);
			nvgMoveTo(args.vg, startX, center.y);
			nvgLineTo(args.vg, endX, center.y);
			nvgMoveTo(args.vg, endX - side * direction * head, center.y - head);
			nvgLineTo(args.vg, endX, center.y);
			nvgLineTo(args.vg, endX - side * direction * head, center.y + head);
		}
		nvgStrokeColor(args.vg, active ? nvgRGBA(231, 247, 255, 245) : nvgRGBA(130, 137, 145, 150));
		nvgStrokeWidth(args.vg, 1.4f);
		nvgLineCap(args.vg, NVG_ROUND);
		nvgLineJoin(args.vg, NVG_ROUND);
		nvgStroke(args.vg);
	}
};

struct WyrmWidget;
struct WyrmExpandedEditorOverlay;

struct WyrmEditorOverlayLink {
	WyrmWidget* owner = nullptr;
	WyrmExpandedEditorOverlay* overlay = nullptr;
};

struct WyrmExpandedEditorOverlay final : widget::OpaqueWidget {
	WyrmEditorDock* anchorDock = nullptr;
	WyrmEditorSurface* editorSurface = nullptr;
	widget::ZoomWidget* editorZoom = nullptr;
	std::shared_ptr<WyrmEditorOverlayLink> link;
	std::function<void()> collapseAction;

	WyrmExpandedEditorOverlay() {
		editorZoom = new widget::ZoomWidget();
		addChild(editorZoom);
	}

	~WyrmExpandedEditorOverlay() override {
		// The scene can tear down before the module rack. Return the sole live
		// editor surface to its dock if normal collapse did not run first.
		if (editorSurface && anchorDock) {
			if (editorZoom && editorSurface->parent == editorZoom) {
				editorZoom->removeChild(editorSurface);
			}
			if (!editorSurface->parent) {
				anchorDock->addChild(editorSurface);
				editorSurface->setPosition(Vec());
				editorSurface->setEditorSize(anchorDock->box.size);
				editorSurface->resetVisualTransitionState();
			}
			anchorDock->expanded = false;
		}
		if (link && link->overlay == this) {
			link->overlay = nullptr;
		}
	}

	void layoutToScene() {
		if (!APP || !APP->scene || !anchorDock || !anchorDock->parent || !editorSurface) return;
		const Vec sceneSize = APP->scene->box.size;
		const float margin = 12.f;
		const float availableWidth = std::max(1.f, sceneSize.x - 2.f * margin);
		const float dockZoom = std::max(anchorDock->getAbsoluteZoom(), 1e-6f);
		const Vec moduleOrigin = anchorDock->parent->getAbsoluteOffset(Vec());
		const Vec dockOrigin = anchorDock->getAbsoluteOffset(Vec());
		const float dockBottom = dockOrigin.y + anchorDock->box.size.y * dockZoom;
		const float availableHeight = std::max(1.f, sceneSize.y - moduleOrigin.y - margin);
		const float desiredVerticalOverhang = mm2px(Vec(0.f, 0.45f)).y * dockZoom;
		const float panelWidth = std::min(2.f * anchorDock->box.size.x * dockZoom, availableWidth);
		const float desiredPanelHeight = std::max(1.f, dockBottom + desiredVerticalOverhang - moduleOrigin.y);
		const float panelHeight = std::min(desiredPanelHeight, availableHeight);
		const float verticalFrameOverhang = std::min(
			desiredVerticalOverhang,
			0.5f * std::max(0.f, panelHeight - 1.f));
		const float editorScreenHeight = std::max(1.f, panelHeight - 2.f * verticalFrameOverhang);
		const Vec requiredSize(panelWidth, panelHeight);
		if (!box.size.equals(requiredSize)) {
			setSize(requiredSize);
		}
		editorZoom->setPosition(Vec(0.f, verticalFrameOverhang));
		editorZoom->setZoom(dockZoom);
		const Vec logicalEditorSize(
			std::max(1.f, panelWidth / dockZoom),
			std::max(1.f, editorScreenHeight / dockZoom));
		if (!editorSurface->box.size.equals(logicalEditorSize)) {
			editorSurface->setPosition(Vec());
			editorSurface->setEditorSize(logicalEditorSize);
			editorSurface->resetVisualTransitionState();
		}

		const Vec dockCenter = dockOrigin.plus(anchorDock->box.size.mult(0.5f * dockZoom));
		setPosition(Vec(
			dockCenter.x - 0.5f * panelWidth,
			moduleOrigin.y));
	}

	void step() override {
		layoutToScene();
		widget::OpaqueWidget::step();
	}

	void draw(const DrawArgs& args) override {
		nvgBeginPath(args.vg);
		nvgRect(args.vg, 0.f, 0.f, box.size.x, box.size.y);
		nvgFillColor(args.vg, nvgRGBA(0, 0, 0, 255));
		nvgFill(args.vg);

		Widget::draw(args);

		const float borderWidth = 2.f;
		const float inset = 0.5f * borderWidth;
		nvgBeginPath(args.vg);
		nvgRect(args.vg, inset, inset,
			std::max(0.f, box.size.x - 2.f * inset),
			std::max(0.f, box.size.y - 2.f * inset));
		nvgStrokeColor(args.vg, nvgRGBA(112, 78, 224, 255));
		nvgStrokeWidth(args.vg, borderWidth);
		nvgStroke(args.vg);
	}

	void onHoverKey(const event::HoverKey& e) override {
		if (e.action == GLFW_PRESS && e.key == GLFW_KEY_ESCAPE && collapseAction) {
			collapseAction();
			e.consume(this);
			return;
		}
		widget::OpaqueWidget::onHoverKey(e);
	}
};

struct WyrmWidget : ModuleWidget {
	std::shared_ptr<window::Svg> ageSigilSvg;
	bool ageSigilUnlocked = false;
	WyrmEditorDock* editorDock = nullptr;
	WyrmEditorSurface* editorSurface = nullptr;
	WyrmEditorGlyphButton* expandEditorButton = nullptr;
	std::shared_ptr<WyrmEditorOverlayLink> editorOverlayLink;

	explicit WyrmWidget(Wyrm* module) {
		setModule(module);
		editorOverlayLink = std::make_shared<WyrmEditorOverlayLink>();
		editorOverlayLink->owner = this;
		PreviewBuildLogTimer previewBuildTimer("Wyrm", module);
		visual_assets::SplitPanelRenderer splitPanel(this, "res/wyrm.panel.svg");
		const std::string& panelPath = splitPanel.panelPath();
		splitPanel.addLabels("res/wyrm.labels.svg");
		splitPanel.addPerfectWaveBranding();
		visual_assets::addFractalGlassOverlay(
			this, panelPath, splitPanel.panelSurfaceEffectWidget());
		previewBuildTimer.markPanelDone();
		try {
			ageSigilSvg = Svg::load(asset::plugin(pluginInstance, "res/icon/Vahdrim'Keth.svg"));
		}
		catch (const std::exception& e) {
			WARN("Wyrm: failed to load age sigil SVG: %s", e.what());
			ageSigilSvg.reset();
		}

		addChild(createWidget<CyanOrbScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<CyanOrbScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<CyanOrbScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<CyanOrbScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		auto applyPt = [&](const char* id, Vec* pos) {
			Vec p;
			if (panel_svg::loadPointFromSvgMm(panelPath, id, &p)) {
				*pos = p;
			}
		};

		math::Rect editorRectMm(Vec(6.0f, 16.0f), Vec(59.12f, 52.0f));
		panel_svg::loadRectFromSvgMm(panelPath, "WYRM_WAVE_EDITOR", &editorRectMm);
		math::Rect freqReadoutRectMm(Vec(editorRectMm.pos.x, editorRectMm.pos.y + editorRectMm.size.y + 1.1f), Vec(editorRectMm.size.x, 3.8f));
		Vec freqPos(17.5f, 80.0f);
		Vec coarseStepModePos(8.1455664f, 99.54583f);
		Vec waveLeftPos(3.1315613f, 75.75f);
		Vec waveRightPos(8.1659473f, 75.75f);
		Vec finePos(20.5f, 86.225996f);
		Vec fmAttenPos(53.62f, 80.0f);
		Vec foldPos(35.56f, 98.0f);
		Vec expandEditorPos(58.151228f, 75.72f);
		Vec lockPos(63.185614f, 75.72f);
		Vec resetPos(68.22f, 75.72f);
		Vec slitherPos(17.50f, 112.80f);
		Vec slitherSpeedPos(26.50f, 112.80f);
		Vec slitherCvPos = slitherPos.plus(Vec(6.8f, 0.f));
		Vec slitherSpeedCvPos = slitherSpeedPos.plus(Vec(6.8f, 0.f));
		Vec voctPos(31.8f, 86.281416f);
		Vec voctModeLabelPos = voctPos.plus(Vec(0.f, 7.105625f));
		Vec envModePos(42.6f, 86.281416f);
		Vec fmPos(28.0f, 111.0f);
		Vec syncPos(43.0f, 111.0f);
		Vec syncModePos(50.0f, 111.0f);
		Vec lfoModePos(57.0f, 111.0f);
		Vec foldCvPos(64.0f, 111.0f);
		Vec rawOutPos(24.0f, 122.0f);
		Vec outPos(47.0f, 122.0f);
		applyPt("WYRM_FREQ_PARAM", &freqPos);
		applyPt("WYRM_COARSE_STEP_MODE_PARAM", &coarseStepModePos);
		applyPt("WYRM_WAVE_LEFT_PARAM", &waveLeftPos);
		applyPt("WYRM_WAVE_RIGHT_PARAM", &waveRightPos);
		applyPt("WYRM_FINE_PARAM", &finePos);
		applyPt("WYRM_FM_ATTEN_PARAM", &fmAttenPos);
		applyPt("WYRM_FOLD_PARAM", &foldPos);
		applyPt("WYRM_LOCK_BUTTON", &lockPos);
		applyPt("WYRM_RESET_BUTTON", &resetPos);
		applyPt("WYRM_SLITHER_PARAM", &slitherPos);
		applyPt("WYRM_SLITHER_SPEED_PARAM", &slitherSpeedPos);
		applyPt("WYRM_SLITHER_CV_INPUT", &slitherCvPos);
		applyPt("WYRM_SLITHER_SPEED_CV_INPUT", &slitherSpeedCvPos);
		applyPt("WYRM_VOCT_INPUT", &voctPos);
		voctModeLabelPos = voctPos.plus(Vec(0.f, 7.105625f));
		applyPt("WYRM_VOCT_MODE_LABEL", &voctModeLabelPos);
		applyPt("WYRM_ENV_MODE_PARAM", &envModePos);
		applyPt("WYRM_FM_INPUT", &fmPos);
		applyPt("WYRM_SYNC_INPUT", &syncPos);
		applyPt("WYRM_SYNC_MODE_PARAM", &syncModePos);
		applyPt("WYRM_LFO_MODE_PARAM", &lfoModePos);
		applyPt("WYRM_FOLD_CV_INPUT", &foldCvPos);
		applyPt("WYRM_RAW_OUTPUT", &rawOutPos);
		applyPt("WYRM_OUT_OUTPUT", &outPos);
		applyPt("WYRM_EDITOR_EXPAND_BUTTON", &expandEditorPos);
		previewBuildTimer.setAtlasStatus(panel_svg::getAtlasStatusLabelForSvg(panelPath));
		previewBuildTimer.markAnchorsDone();

		editorDock = new WyrmEditorDock();
		editorDock->setPosition(mm2px(editorRectMm.pos));
		editorDock->setSize(mm2px(editorRectMm.size));
		editorSurface = new WyrmEditorSurface(module);
		editorSurface->setPosition(Vec());
		editorSurface->setEditorSize(editorDock->box.size);
		editorDock->addChild(editorSurface);
		addChild(editorDock);
		addChild(visual_assets::createPreviewFrameEnhancementWidget(editorRectMm, visual_assets::PreviewFrameTint::Purple));
		expandEditorButton = new WyrmEditorGlyphButton();
		expandEditorButton->setPosition(mm2px(expandEditorPos).minus(expandEditorButton->box.size.mult(0.5f)));
		expandEditorButton->enabledAction = [this]() { return this->module != nullptr; };
		expandEditorButton->collapseGlyphAction = [this]() { return isEditorExpanded(); };
		expandEditorButton->tooltipTextProvider = [this]() {
			return isEditorExpanded() ? "Collapse waveform editor" : "Expand waveform editor";
		};
		expandEditorButton->buttonAction = [this]() {
			if (isEditorExpanded()) closeExpandedEditor();
			else openExpandedEditor();
		};
		addChild(expandEditorButton);
		auto* freqReadout = new WyrmFrequencyReadoutWidget();
		freqReadout->module = module;
		freqReadout->box.pos = mm2px(freqReadoutRectMm.pos);
		freqReadout->box.size = mm2px(freqReadoutRectMm.size);
		addChild(freqReadout);

		auto* lockButton = new WyrmTooltipButton<LeviathanIconButton>();
		const std::shared_ptr<window::Svg> lockClosedSvg = visual_assets::loadPluginSvgCached("res/icon/lock_closed-highlighted.svg");
		const std::shared_ptr<window::Svg> lockOpenSvg = visual_assets::loadPluginSvgCached("res/icon/lock_open-highlighted.svg");
		lockButton->iconProvider = [module, lockClosedSvg, lockOpenSvg]() {
			return (module && module->editorLocked.load(std::memory_order_relaxed)) ? lockClosedSvg : lockOpenSvg;
		};
		lockButton->tooltipTextProvider = [module]() {
			return (module && module->editorLocked.load(std::memory_order_relaxed))
				? "Unlock waveform editor"
				: "Lock waveform editor";
		};
		if (module) {
			lockButton->buttonAction = [module]() {
				module->editorLocked.store(!module->editorLocked.load(std::memory_order_relaxed), std::memory_order_relaxed);
			};
		}
		lockButton->box.pos = mm2px(lockPos).minus(lockButton->box.size.mult(0.5f));
		addChild(lockButton);
		auto* resetButton = new WyrmTooltipButton<LeviathanResetButton>();
		resetButton->box.pos = mm2px(resetPos).minus(resetButton->box.size.mult(0.5f));
		resetButton->tooltipTextProvider = []() { return "Reset waveform"; };
		if (module) {
			resetButton->buttonAction = [module]() {
				if (module->envelopeMode.load(std::memory_order_relaxed)) {
					module->setEnvelopeArShape();
				}
				else {
					module->setFactoryShape(module->selectedShape);
				}
			};
		}
		addChild(resetButton);
		auto* waveLeft = createParamCentered<WyrmWaveLeftButton>(mm2px(waveLeftPos), module, Wyrm::WAVE_LEFT_PARAM);
		waveLeft->module = module;
		addParam(waveLeft);
		auto* waveRight = createParamCentered<WyrmWaveRightButton>(mm2px(waveRightPos), module, Wyrm::WAVE_RIGHT_PARAM);
		waveRight->module = module;
		addParam(waveRight);

		addParam(createParamCentered<LeviathanHaloKnob2>(mm2px(freqPos), module, Wyrm::FREQ_PARAM));
		addParam(createParamCentered<BipolarDarkTinyClockworkGearKnob>(mm2px(finePos), module, Wyrm::FINE_PARAM));
		{
			Eclipse2Knob* fmAttenKnob = createParamCentered<Eclipse2Knob>(mm2px(fmAttenPos), module, Wyrm::FM_ATTEN_PARAM);
			fmAttenKnob->setProgressRingBipolar(true);
			addParam(fmAttenKnob);
		}
		addParam(createParamCentered<Eclipse2Knob>(mm2px(foldPos), module, Wyrm::FOLD_PARAM));
		addParam(createParamCentered<Eclipse2Knob>(mm2px(slitherPos), module, Wyrm::SLITHER_PARAM));
		addParam(createParamCentered<Eclipse2Knob>(mm2px(slitherSpeedPos), module, Wyrm::SLITHER_SPEED_PARAM));

		addInput(createInputCentered<Magitek2InputJack>(mm2px(voctPos), module, Wyrm::VOCT_INPUT));
		auto* voctModeLabel = new WyrmVoctModeLabelWidget();
		voctModeLabel->module = module;
		voctModeLabel->box.size = mm2px(Vec(12.f, 3.8f));
		voctModeLabel->box.pos = mm2px(voctModeLabelPos).minus(voctModeLabel->box.size.mult(0.5f));
		addChild(voctModeLabel);
		addInput(createInputCentered<Magitek2InputJack>(mm2px(fmPos), module, Wyrm::FM_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(syncPos), module, Wyrm::SYNC_INPUT));
		auto addModeToggle = [&](int paramId, int lightId, Vec posMm) {
			auto* button = createLightParamCentered<SmallGoldApertureButton>(mm2px(posMm), module, paramId, lightId);
			static_cast<SmallGoldApertureLight*>(button->getLight())->setBaseColor(nvgRGB(255, 118, 24));
			addParam(button);
		};
		addModeToggle(Wyrm::SYNC_MODE_PARAM, Wyrm::SYNC_MODE_LIGHT, syncModePos);
		addModeToggle(Wyrm::LFO_MODE_PARAM, Wyrm::LFO_MODE_LIGHT, lfoModePos);
		addModeToggle(Wyrm::ENV_MODE_PARAM, Wyrm::ENV_MODE_LIGHT, envModePos);
		addModeToggle(Wyrm::COARSE_STEP_MODE_PARAM, Wyrm::COARSE_STEP_MODE_LIGHT, coarseStepModePos);
		addInput(createInputCentered<Magitek2InputJack>(mm2px(foldCvPos), module, Wyrm::FOLD_CV_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(slitherCvPos), module, Wyrm::SLITHER_CV_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(slitherSpeedCvPos), module, Wyrm::SLITHER_SPEED_CV_INPUT));

		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(rawOutPos), module, Wyrm::RAW_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(outPos), module, Wyrm::OUT_OUTPUT));
	}

	~WyrmWidget() override {
		closeExpandedEditor();
		if (editorOverlayLink) {
			editorOverlayLink->owner = nullptr;
		}
	}

	bool isEditorExpanded() const {
		return editorOverlayLink && editorOverlayLink->overlay;
	}

	void openExpandedEditor() {
		if (isEditorExpanded() || !module || !APP || !APP->scene || !editorDock || !editorSurface) {
			return;
		}
		auto* overlay = new WyrmExpandedEditorOverlay();
		overlay->anchorDock = editorDock;
		overlay->editorSurface = editorSurface;
		overlay->link = editorOverlayLink;
		const std::shared_ptr<WyrmEditorOverlayLink> link = editorOverlayLink;
		overlay->collapseAction = [link]() {
			if (link && link->owner) {
				link->owner->closeExpandedEditor();
			}
		};
		editorOverlayLink->overlay = overlay;

		editorDock->removeChild(editorSurface);
		editorDock->expanded = true;
		overlay->editorZoom->addChild(editorSurface);
		overlay->layoutToScene();
		if (APP->scene->menuBar && APP->scene->hasChild(APP->scene->menuBar)) {
			APP->scene->addChildBelow(overlay, APP->scene->menuBar);
		}
		else {
			APP->scene->addChild(overlay);
		}
		editorSurface->resetVisualTransitionState();
	}

	void closeExpandedEditor() {
		WyrmExpandedEditorOverlay* overlay = editorOverlayLink ? editorOverlayLink->overlay : nullptr;
		if (!overlay) {
			return;
		}
		if (editorSurface && overlay->editorZoom && editorSurface->parent == overlay->editorZoom) {
			overlay->editorZoom->removeChild(editorSurface);
		}
		if (editorSurface && editorDock && !editorSurface->parent) {
			editorDock->addChild(editorSurface);
			editorSurface->setPosition(Vec());
			editorSurface->setEditorSize(editorDock->box.size);
			editorSurface->resetVisualTransitionState();
		}
		if (editorDock) {
			editorDock->expanded = false;
		}
		overlay->anchorDock = nullptr;
		overlay->editorSurface = nullptr;
		overlay->collapseAction = nullptr;
		editorOverlayLink->overlay = nullptr;
		if (overlay->parent) {
			overlay->requestDelete();
		}
		else {
			delete overlay;
		}
	}

	void step() override {
		ModuleWidget::step();
		Wyrm* wyrm = dynamic_cast<Wyrm*>(module);
		if (!wyrm || ageSigilUnlocked) return;
		const double createdUnixTimeSec = wyrm->createdUnixTimeSec;
		if (std::isfinite(createdUnixTimeSec) && createdUnixTimeSec > 0.0) {
			ageSigilUnlocked = (system::getUnixTime() - createdUnixTimeSec) >= 666.0;
		}
	}

	void draw(const DrawArgs& args) override {
		ModuleWidget::draw(args);
		Wyrm* wyrm = dynamic_cast<Wyrm*>(module);
		if (wyrm && isDragonKingDebugEnabled() && APP && APP->window && APP->window->uiFont) {
			char debugIdLabel[32];
			std::snprintf(debugIdLabel, sizeof(debugIdLabel), "ID:%u", wyrm->debugInstanceId);
			const float x = box.size.x - mm2px(0.9f);
			const float y = mm2px(2.5f);
			nvgSave(args.vg);
			nvgFontFaceId(args.vg, APP->window->uiFont->handle);
			nvgFontSize(args.vg, 6.8f);
			nvgTextAlign(args.vg, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
			nvgFillColor(args.vg, nvgRGBA(8, 10, 14, 210));
			nvgText(args.vg, x + 0.45f, y + 0.45f, debugIdLabel, nullptr);
			nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 230));
			nvgText(args.vg, x, y, debugIdLabel, nullptr);
			nvgRestore(args.vg);
		}
		if (!wyrm || !ageSigilSvg || !ageSigilUnlocked) {
			return;
		}
		const Vec sigilSize = mm2px(Vec(3.8f, 4.6f));
		const Vec rightSigilCenter = mm2px(Vec(54.8f, 4.47f));
		const Vec leftSigilCenter(box.size.x - rightSigilCenter.x, rightSigilCenter.y);
		const Vec svgSize = ageSigilSvg->getSize();
		if (svgSize.x <= 1.f || svgSize.y <= 1.f) {
			return;
		}
		const float scaleX = sigilSize.x / svgSize.x;
		const float scaleY = sigilSize.y / svgSize.y;
		auto drawSigilAt = [&](const Vec& center) {
			nvgSave(args.vg);
			nvgTranslate(args.vg, center.x, center.y);
			nvgScale(args.vg, scaleX, scaleY);
			nvgTranslate(args.vg, -svgSize.x * 0.5f, -svgSize.y * 0.5f);
			ageSigilSvg->draw(args.vg);
			nvgRestore(args.vg);
		};
		drawSigilAt(leftSigilCenter);
		drawSigilAt(rightSigilCenter);
	}

	void appendContextMenu(Menu* menu) override {
		ModuleWidget::appendContextMenu(menu);
		auto* module = dynamic_cast<Wyrm*>(this->module);
		if (!module) return;
		auto sandRendererLabel = [&](int backend) {
			switch (backend) {
				case WYRM_RENDER_OPENGL: return "OpenGL";
				case WYRM_RENDER_OPENGL_SHDR: return "OpenGL SHDR";
				case WYRM_RENDER_NANOVG:
				default: return "NanoVG";
			}
		};
		auto applyRenderMode = [=](int mode) {
			mode = clamp(mode, WYRM_RENDER_NANOVG, WYRM_RENDER_OPENGL_SHDR);
			module->renderMode.store(mode, std::memory_order_relaxed);
			switch (mode) {
				case WYRM_RENDER_OPENGL:
					module->sandBackend.store(WYRMSAND_OPENGL_TEXTURE, std::memory_order_relaxed);
					break;
				case WYRM_RENDER_OPENGL_SHDR:
					module->sandBackend.store(WYRMSAND_SHADER_FEEDBACK, std::memory_order_relaxed);
					break;
				case WYRM_RENDER_NANOVG:
				default:
					module->sandBackend.store(WYRMSAND_NANOVG_IMAGE, std::memory_order_relaxed);
					break;
			}
		};
		auto sandDetailLabel = [&](int detail) {
			switch (detail) {
				case WYRMSAND_DETAIL_LOW: return "Low";
				case WYRMSAND_DETAIL_MEDIUM: return "Medium";
				case WYRMSAND_DETAIL_HIGH: return "High";
				case WYRMSAND_DETAIL_AUTO: return "Auto";
				default: return "Unknown";
			}
		};
		auto sandPersistenceLabel = [&](int persistence) {
			switch (persistence) {
				case WYRMSAND_PERSISTENCE_SHORT: return "Short";
				case WYRMSAND_PERSISTENCE_MEDIUM: return "Medium";
				case WYRMSAND_PERSISTENCE_LONG: return "Long";
				default: return "Unknown";
			}
		};

		menu->addChild(new MenuSeparator());
		menu->addChild(createMenuItem(
			isEditorExpanded() ? "Collapse Waveform Editor" : "Expand Waveform Editor", "",
			[this]() {
				if (isEditorExpanded()) closeExpandedEditor();
				else openExpandedEditor();
			}));
		menu->addChild(createCheckMenuItem("Lock Wave Editor", "",
			[=]() { return module->editorLocked.load(std::memory_order_relaxed); },
			[=]() { module->editorLocked.store(!module->editorLocked.load(std::memory_order_relaxed), std::memory_order_relaxed); }
		));
		menu->addChild(createSubmenuItem("Renderer", sandRendererLabel(module->renderMode.load(std::memory_order_relaxed)), [=](Menu* rendererMenu) {
			rendererMenu->addChild(createCheckMenuItem("NanoVG", "",
				[=]() { return module->renderMode.load(std::memory_order_relaxed) == WYRM_RENDER_NANOVG; },
				[=]() { applyRenderMode(WYRM_RENDER_NANOVG); }
			));
			rendererMenu->addChild(createCheckMenuItem("OpenGL", "",
				[=]() { return module->renderMode.load(std::memory_order_relaxed) == WYRM_RENDER_OPENGL; },
				[=]() { applyRenderMode(WYRM_RENDER_OPENGL); }
			));
			rendererMenu->addChild(createCheckMenuItem("OpenGL SHDR", "",
				[=]() { return module->renderMode.load(std::memory_order_relaxed) == WYRM_RENDER_OPENGL_SHDR; },
				[=]() { applyRenderMode(WYRM_RENDER_OPENGL_SHDR); }
			));
		}));
		menu->addChild(createSubmenuItem("Sand", "", [=](Menu* submenu) {
			submenu->addChild(createCheckMenuItem("Sand View", "",
				[=]() { return module->sandViewEnabled.load(std::memory_order_relaxed); },
				[=]() { module->sandViewEnabled.store(!module->sandViewEnabled.load(std::memory_order_relaxed), std::memory_order_relaxed); }
			));
			submenu->addChild(new MenuSeparator());
			submenu->addChild(createSubmenuItem("Detail", sandDetailLabel(module->sandDetail.load(std::memory_order_relaxed)), [=](Menu* detailMenu) {
				detailMenu->addChild(createCheckMenuItem("Auto", "",
					[=]() { return module->sandDetail.load(std::memory_order_relaxed) == WYRMSAND_DETAIL_AUTO; },
					[=]() { module->sandDetail.store(WYRMSAND_DETAIL_AUTO, std::memory_order_relaxed); }
				));
				detailMenu->addChild(createCheckMenuItem("Low", "",
					[=]() { return module->sandDetail.load(std::memory_order_relaxed) == WYRMSAND_DETAIL_LOW; },
					[=]() { module->sandDetail.store(WYRMSAND_DETAIL_LOW, std::memory_order_relaxed); }
				));
				detailMenu->addChild(createCheckMenuItem("Medium", "",
					[=]() { return module->sandDetail.load(std::memory_order_relaxed) == WYRMSAND_DETAIL_MEDIUM; },
					[=]() { module->sandDetail.store(WYRMSAND_DETAIL_MEDIUM, std::memory_order_relaxed); }
				));
				detailMenu->addChild(createCheckMenuItem("High", "",
					[=]() { return module->sandDetail.load(std::memory_order_relaxed) == WYRMSAND_DETAIL_HIGH; },
					[=]() { module->sandDetail.store(WYRMSAND_DETAIL_HIGH, std::memory_order_relaxed); }
				));
			}));
			submenu->addChild(createSubmenuItem("Persistence", sandPersistenceLabel(module->sandPersistence.load(std::memory_order_relaxed)), [=](Menu* persistenceMenu) {
				persistenceMenu->addChild(createCheckMenuItem("Short", "",
					[=]() { return module->sandPersistence.load(std::memory_order_relaxed) == WYRMSAND_PERSISTENCE_SHORT; },
					[=]() { module->sandPersistence.store(WYRMSAND_PERSISTENCE_SHORT, std::memory_order_relaxed); }
				));
				persistenceMenu->addChild(createCheckMenuItem("Medium", "",
					[=]() { return module->sandPersistence.load(std::memory_order_relaxed) == WYRMSAND_PERSISTENCE_MEDIUM; },
					[=]() { module->sandPersistence.store(WYRMSAND_PERSISTENCE_MEDIUM, std::memory_order_relaxed); }
				));
				persistenceMenu->addChild(createCheckMenuItem("Long", "",
					[=]() { return module->sandPersistence.load(std::memory_order_relaxed) == WYRMSAND_PERSISTENCE_LONG; },
					[=]() { module->sandPersistence.store(WYRMSAND_PERSISTENCE_LONG, std::memory_order_relaxed); }
				));
			}));
		}));
		menu->addChild(createSubmenuItem("Rocks", string::f("%d", module->rockCount), [=](Menu* submenu) {
			const bool dragModeSelected = (module->rockMouseMode == ROCK_MOUSE_DRAGS);
			const std::string dragLabel = dragModeSelected ? "Mouse Drags Rocks" : "Mouse Drags Rocks (shift)";
			const std::string liftLabel = dragModeSelected ? "Mouse Lifts Rocks (shift)" : "Mouse Lifts Rocks";
			submenu->addChild(createCheckMenuItem(
				dragLabel, "",
				[=]() {
					return module->rockMouseMode == ROCK_MOUSE_DRAGS;
				},
				[=]() {
					module->rockMouseMode = ROCK_MOUSE_DRAGS;
					module->liftedRock = -1;
					module->publishRockState();
				}));
			submenu->addChild(createCheckMenuItem(
				liftLabel, "",
				[=]() {
					return module->rockMouseMode == ROCK_MOUSE_LIFTS;
				},
				[=]() {
					module->rockMouseMode = ROCK_MOUSE_LIFTS;
					module->publishRockState();
				}));
			submenu->addChild(new MenuSeparator());
			for (int count = 0; count <= kWyrmMaxRocks; ++count) {
				submenu->addChild(createCheckMenuItem(
					string::f("%d", count), "",
					[=]() {
						return module->rockCount == count;
					},
					[=]() {
						module->setRockCount(count);
					}));
			}
		}));
		menu->addChild(new MenuSeparator());
		menu->addChild(createSubmenuItem("Point Count", string::f("%d", module->pointCount), [=](Menu* submenu) {
			for (int count : {32, 48, 64, 128, 256}) {
				auto* item = new WyrmPointCountMenuItem();
				item->text = string::f("%d", count);
				item->module = module;
				item->count = count;
				submenu->addChild(item);
			}
		}));
		menu->addChild(new MenuSeparator());
		menu->addChild(createMenuLabel("Factory Shape"));
		for (int i = 0; i < SHAPE_COUNT; ++i) {
			auto* item = new WyrmShapeMenuItem();
			item->text = kWyrmShapeLabels[i];
			item->module = module;
			item->shape = i;
			menu->addChild(item);
		}
	}
};

Model* modelWyrm = createModel<Wyrm, WyrmWidget>("Wyrm");
