#include "Bifurx.hpp"
#include "BifurxSpectrumNanoVG.hpp"
#include "BifurxLicense.hpp"
#include "DebugTerminalTransport.hpp"
#include "BifurxWorker.hpp"
#include "visual/VisualAssets.hpp"
#include "visual/FractalGlassOverlay.hpp"
#include "visual/PlasmaConduit.hpp"

namespace bifurx {

struct BifurxSpectrumBackgroundWidget final : Widget {
	Bifurx* module = nullptr;
	widget::FramebufferWidget* framebuffer = nullptr;
	uint32_t lastPreviewSeq = 0;
	float sampleRate = 48000.f;
	int lastMode = -1;
	float lastDrawWidth = -1.f;
	float lastDrawHeight = -1.f;

	void step() override {
		Widget::step();
		bool dirty = false;
		if (module) {
			BifurxPreviewState previewState;
			double publishTimeSec = 0.0;
			uint32_t previewSeq = lastPreviewSeq;
			if (module->readPreviewState(lastPreviewSeq, &previewState, &publishTimeSec, &previewSeq)) {
				const float newSampleRate = std::max(1.f, previewState.sampleRate);
				if (std::fabs(newSampleRate - sampleRate) > 0.5f) { sampleRate = newSampleRate; dirty = true; }
				lastPreviewSeq = previewSeq;
			}
			const int mode = clamp(int(std::round(module->params[Bifurx::MODE_PARAM].getValue())), 0, kBifurxUiModeCount - 1);
			if (mode != lastMode) { lastMode = mode; dirty = true; }
		}
		if (std::fabs(box.size.x - lastDrawWidth) > 1e-4f || std::fabs(box.size.y - lastDrawHeight) > 1e-4f) { lastDrawWidth = box.size.x; lastDrawHeight = box.size.y; dirty = true; }
		if (dirty && framebuffer) framebuffer->setDirty();
	}

	void draw(const DrawArgs& args) override {
		const float w = box.size.x; const float h = box.size.y;
		if (!(w > 0.f && h > 0.f)) return;
		const float padX = 0.f, padY = std::max(4.f, h * 0.035f), plotX = padX, usableW = std::max(1.f, w - plotX - padX), minHz = 10.f, maxHz = std::min(20000.f, 0.46f * sampleRate), labelBandHeight = std::max(5.2f, h * 0.072f), labelBandTop = h - labelBandHeight, spectrumBottomY = std::max(padY * 0.35f + 1.f, labelBandTop - std::max(0.05f, h * 0.0008f));
		nvgSave(args.vg); nvgScissor(args.vg, 0.8f, 0.8f, std::max(0.f, w - 1.6f), std::max(0.f, h - 1.6f));
		nvgBeginPath(args.vg); nvgRect(args.vg, 0.f, 0.f, w, h); nvgFillColor(args.vg, nvgRGBA(7, 10, 14, 255)); nvgFill(args.vg);
		nvgBeginPath(args.vg); nvgRect(args.vg, 0.f, labelBandTop, w, h - labelBandTop); nvgFillColor(args.vg, nvgRGBA(4, 7, 11, 208)); nvgFill(args.vg);
		nvgBeginPath(args.vg); nvgMoveTo(args.vg, 0.f, labelBandTop); nvgLineTo(args.vg, w, labelBandTop); nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 20)); nvgStrokeWidth(args.vg, 1.f); nvgStroke(args.vg);
		nvgSave(args.vg); nvgScissor(args.vg, plotX, 0.f, usableW, std::max(1.f, spectrumBottomY));
		for (float dS = 10.f; dS < maxHz; dS *= 10.f) { for (int m = 1; m <= 9; ++m) { float gH = dS * float(m); if (gH >= maxHz) continue; const bool maj = (m == 1); float gX = plotX + usableW * logPosition(gH, minHz, maxHz); nvgBeginPath(args.vg); nvgMoveTo(args.vg, gX, padY * 0.35f); nvgLineTo(args.vg, gX, spectrumBottomY); nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, maj ? 34 : 16)); nvgStrokeWidth(args.vg, maj ? 1.f : 0.7f); nvgStroke(args.vg); } }
		nvgBeginPath(args.vg); float y0 = responseYForDbDisplay(0.f, kResponseMinDb, kResponseMaxDb, spectrumBottomY, padY * 0.35f); nvgMoveTo(args.vg, plotX, y0); nvgLineTo(args.vg, plotX + usableW, y0); nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 24)); nvgStrokeWidth(args.vg, 1.2f); nvgStroke(args.vg);
		nvgResetScissor(args.vg); nvgRestore(args.vg);
		if (isBifurxDisplayOnlyMode(lastMode)) {
			const struct { float hz; const char* label; } marks[] = {
				{100.f, "100Hz"},
				{1000.f, "1kHz"},
				{10000.f, "10kHz"}
			};
			nvgFontSize(args.vg, std::max(7.f, h * 0.055f));
			nvgFontFaceId(args.vg, APP->window->uiFont->handle);
			nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
			const float labelY = labelBandTop + 0.52f * labelBandHeight;
			for (const auto& mark : marks) {
				if (mark.hz <= minHz || mark.hz >= maxHz) continue;
				const float x = clamp(plotX + usableW * logPosition(mark.hz, minHz, maxHz), 14.f, w - 14.f);
				nvgFillColor(args.vg, nvgRGBA(4, 6, 9, 240));
				nvgText(args.vg, x, labelY + 0.75f, mark.label, nullptr);
				nvgFillColor(args.vg, nvgRGBA(241, 246, 252, 250));
				nvgText(args.vg, x, labelY, mark.label, nullptr);
			}
		}
		nvgRestore(args.vg);
	}
};

void drawModeStepTriangle(const Widget::DrawArgs& args, const Vec& size, bool pointRight) {
	const float cx = 0.5f * size.x, cy = 0.5f * size.y, hW = 2.8f, hH = 3.3f, off = pointRight ? (hW / 3.f) : (-hW / 3.f);
	nvgBeginPath(args.vg); if (pointRight) { nvgMoveTo(args.vg, cx - hW + off, cy - hH); nvgLineTo(args.vg, cx + hW + off, cy); nvgLineTo(args.vg, cx - hW + off, cy + hH); } else { nvgMoveTo(args.vg, cx + hW + off, cy - hH); nvgLineTo(args.vg, cx - hW + off, cy); nvgLineTo(args.vg, cx + hW + off, cy + hH); }
	nvgClosePath(args.vg); nvgFillColor(args.vg, nvgRGBA(225, 232, 240, 244)); nvgFill(args.vg);
}

static void setBifurxModeWithHistory(Bifurx* module, int mode) {
	if (!module) return;
	const float before = module->params[Bifurx::MODE_PARAM].getValue();
	if (before == float(mode)) return;
	module->params[Bifurx::MODE_PARAM].setValue(float(mode));
	if (APP && APP->history) {
		auto* h = new history::ParamChange;
		h->name = "change Bifurx mode";
		h->moduleId = module->id; h->paramId = Bifurx::MODE_PARAM;
		h->oldValue = before; h->newValue = float(mode);
		APP->history->push(h);
	}
}

template<int Direction> struct BifurxModeButton : TL1105 {
	void onDragStart(const DragStartEvent& e) override {
		if (e.button != GLFW_MOUSE_BUTTON_LEFT) return;
		auto* m = dynamic_cast<Bifurx*>(module);
		if (m) setBifurxModeWithHistory(m, (int(std::round(m->params[Bifurx::MODE_PARAM].getValue())) + kBifurxUiModeCount + Direction) % kBifurxUiModeCount);
	}
	void onDragEnd(const DragEndEvent&) override {}
	void draw(const DrawArgs& args) override { TL1105::draw(args); drawModeStepTriangle(args, box.size, Direction > 0); }
};
using BifurxModeLeftButton = BifurxModeButton<-1>;
using BifurxModeRightButton = BifurxModeButton<1>;

template<typename T> static void setBifurxSettingWithHistory(Bifurx* module, std::atomic<T>& setting, T value, const char* name) {
	const T before = setting.load(std::memory_order_relaxed);
	if (before == value) return;
	auto* h = (APP && APP->history) ? new history::ModuleChange : nullptr;
	if (h) { h->name = name; h->moduleId = module->id; h->oldModuleJ = module->toJson(); }
	setting.store(value, std::memory_order_relaxed);
	if (h) { h->newModuleJ = module->toJson(); APP->history->push(h); }
}

struct BifurxModeMenuButton final : TL1105 {
	Bifurx* module = nullptr;

	void onButton(const event::Button& e) override {
		if (!module || e.button != GLFW_MOUSE_BUTTON_LEFT || e.action != GLFW_PRESS) {
			TL1105::onButton(e);
			return;
		}
		ui::Menu* menu = createMenu();
		menu->box.pos = getAbsoluteOffset(Vec(0.f, box.size.y));
		menu->addChild(createMenuLabel("Filter Mode"));
		for (int mode = 0; mode < kBifurxUiModeCount; ++mode) {
			menu->addChild(createCheckMenuItem(
				kBifurxModeLabels[mode], "",
				[=]() { return int(std::round(module->params[Bifurx::MODE_PARAM].getValue())) == mode; },
				[=]() { setBifurxModeWithHistory(module, mode); }
			));
		}
		e.consume(this);
	}

	void draw(const DrawArgs& args) override {
		TL1105::draw(args);
		const float cx = 0.5f * box.size.x;
		const float cy = 0.5f * box.size.y;
		const float dy = std::max(1.6f, 0.16f * box.size.y);
		const float halfW = std::max(1.9f, 0.22f * box.size.x);
		const float y0 = cy - dy;
		for (int i = 0; i < 3; ++i) {
			const float y = y0 + dy * float(i);
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, cx - halfW, y);
			nvgLineTo(args.vg, cx + halfW, y);
			nvgStrokeWidth(args.vg, 1.2f);
			nvgStrokeColor(args.vg, nvgRGBA(225, 232, 240, 244));
			nvgStroke(args.vg);
		}
	}
};

struct BifurxModeReadoutWidget final : Widget {
	Module* module = nullptr;
	NVGcontext* cachedVg = nullptr;
	int cachedFontHandle = -1;
	float cachedFontSize = -1.f;
	int cachedMode = -1;
	float cachedLocalMinX = 0.f;
	float cachedLocalMinY = 0.f;
	float cachedLocalWidth = 0.f;
	float cachedLocalHeight = 0.f;

	void draw(const DrawArgs& args) override {
		if (!APP || !APP->window || !APP->window->uiFont) return;
		int m = module ? clamp(int(std::round(module->params[Bifurx::MODE_PARAM].getValue())), 0, kBifurxUiModeCount - 1) : 0;
		const float fontSize = std::max(9.5f, box.size.y * 0.72f);
		const int fontHandle = APP->window->uiFont->handle;

		static char sModeLabels[kBifurxUiModeCount][32];
		static bool sModeLabelsInit = false;
		if (!sModeLabelsInit) {
			for (int i = 0; i < kBifurxUiModeCount; ++i) {
				std::snprintf(sModeLabels[i], sizeof(sModeLabels[i]), "Mode (%d): %s", i + 1, kBifurxModeLabels[i]);
			}
			sModeLabelsInit = true;
		}
		const char* label = (m >= 0 && m < kBifurxUiModeCount) ? sModeLabels[m] : "";

		if (args.vg != cachedVg || fontHandle != cachedFontHandle || std::fabs(fontSize - cachedFontSize) > 1e-4f || m != cachedMode) {
			nvgFontSize(args.vg, fontSize);
			nvgFontFaceId(args.vg, fontHandle);
			nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
			float bounds[4];
			nvgTextBounds(args.vg, 0.f, 0.f, label, nullptr, bounds);
			cachedLocalMinX = bounds[0] - 4.f;
			cachedLocalMinY = bounds[1] - 1.5f;
			cachedLocalWidth = bounds[2] - bounds[0] + 8.f;
			cachedLocalHeight = bounds[3] - bounds[1] + 3.f;
			cachedVg = args.vg;
			cachedFontHandle = fontHandle;
			cachedFontSize = fontSize;
			cachedMode = m;
		}

		nvgFontSize(args.vg, fontSize);
		nvgFontFaceId(args.vg, fontHandle);
		nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		const float centerX = 0.5f * box.size.x;
		const float centerY = 0.5f * box.size.y;
		const float x = centerX + cachedLocalMinX;
		const float y = centerY + cachedLocalMinY;
		const float width = cachedLocalWidth;
		const float height = cachedLocalHeight;
		// One softly edged black capsule keeps the readout legible on light panels.
		const float radius = height * 0.5f;
		const NVGpaint halo = nvgBoxGradient(args.vg, x, y, width, height, radius, 1.5f,
			nvgRGBA(0, 0, 0, 230), nvgRGBA(0, 0, 0, 0));
		nvgBeginPath(args.vg);
		nvgRect(args.vg, x - 1.5f, y - 1.5f, width + 3.f, height + 3.f);
		nvgFillPaint(args.vg, halo);
		nvgFill(args.vg);
		nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 255));
		nvgText(args.vg, centerX, centerY, label, nullptr);
	}
};

struct BifurxModernPanelBackingWidget final : Widget {
	void draw(const DrawArgs& args) override {
		nvgBeginPath(args.vg);
		nvgRect(args.vg, 0.f, 0.f, box.size.x, box.size.y);
		nvgFillColor(args.vg, nvgRGB(0, 0, 0));
		nvgFill(args.vg);
	}
};

struct BifurxVisibleFramebufferWidget final : widget::FramebufferWidget {
	void step() override {
		if (!isVisible()) return;
		widget::FramebufferWidget::step();
	}
};

struct BifurxWidget final : ModuleWidget {
	std::unique_ptr<Bifurx> browserPreviewModule;
	Bifurx* analysisSubscriptionModule = nullptr;
	widget::FramebufferWidget* spectrumNanoVG = nullptr;
	Widget* spectrumOpenGL = nullptr;
	Widget* modernPanelBacking = nullptr;
	Widget* modernTopRaster = nullptr;
	Widget* modernTopRasterDark = nullptr;
	Widget* modernBottomRaster = nullptr;
	Widget* modernBottomRasterDark = nullptr;
	widget::FramebufferWidget* conduitFramebuffer = nullptr;
	Widget* legacySvgPanel = nullptr;
	Widget* legacyPanelSurface = nullptr;
	Widget* legacyGlassFramebuffer = nullptr;
	Widget* modernPanelBorder = nullptr;
	Widget* legacyTitleRaster = nullptr;
	Widget* legacyLabels = nullptr;
	Widget* modernSpectrumFrame = nullptr;
	Widget* legacySpectrumFrame = nullptr;
	widget::FramebufferWidget* spectrumBackgroundFramebuffer = nullptr;
	BifurxSpectrumBackgroundWidget* spectrumBackgroundContent = nullptr;
	BifurxSpectrumWidget* spectrumNanoVGContent = nullptr;
	BifurxSpectrumBase* spectrumOpenGLBase = nullptr;
	BifurxModeReadoutWidget* modeReadout = nullptr;
	CyanOrbScrew* legacyTopLeftScrew = nullptr;
	CyanOrbScrew* legacyTopRightScrew = nullptr;
	math::Rect modernSpectrumRectMm;
	math::Rect legacySpectrumRectMm;
	bool lastLegacyVisuals = false;
	bool lastPreferDarkPanels = false;
	int lastRenderMode = -1;
	debug_terminal::UiTimingRangeAccumulator moduleStepUsRange;
	debug_terminal::UiTimingRangeAccumulator moduleDrawUsRange;
	float lastConduitDrawUs = 0.f;

	void applySpectrumRect(const math::Rect& rectMm) {
		const Vec posPx = mm2px(rectMm.pos);
		const Vec sizePx = mm2px(rectMm.size);
		if (spectrumBackgroundFramebuffer && spectrumBackgroundContent) {
			spectrumBackgroundFramebuffer->box.pos = posPx;
			spectrumBackgroundFramebuffer->box.size = sizePx;
			spectrumBackgroundContent->box.size = sizePx;
			spectrumBackgroundFramebuffer->setDirty();
		}
		if (spectrumNanoVG && spectrumNanoVGContent) {
			spectrumNanoVG->box.pos = posPx;
			spectrumNanoVG->box.size = sizePx;
			spectrumNanoVGContent->box.size = sizePx;
			spectrumNanoVG->setDirty();
		}
		if (spectrumOpenGL) {
			spectrumOpenGL->box.pos = posPx;
			spectrumOpenGL->box.size = sizePx;
		}
		if (modeReadout) {
			modeReadout->box.pos = mm2px(Vec(rectMm.pos.x, rectMm.pos.y + rectMm.size.y + 0.6f));
			modeReadout->box.size = mm2px(Vec(rectMm.size.x, 4.2f));
		}
	}

	void applyLegacyVisuals(bool legacy) {
		const bool dark = settings::preferDarkPanels;
		if (legacySvgPanel) legacySvgPanel->setVisible(legacy);
		if (legacyPanelSurface) legacyPanelSurface->setVisible(legacy);
		if (legacyGlassFramebuffer) legacyGlassFramebuffer->setVisible(legacy);
		if (modernPanelBacking) modernPanelBacking->setVisible(!legacy);
		if (modernTopRaster) modernTopRaster->setVisible(!legacy && !dark);
		if (modernTopRasterDark) modernTopRasterDark->setVisible(!legacy && dark);
		if (modernBottomRaster) modernBottomRaster->setVisible(!legacy && !dark);
		if (modernBottomRasterDark) modernBottomRasterDark->setVisible(!legacy && dark);
		if (conduitFramebuffer) {
			// Modern raster assets contain the static conduits. Retain the cached
			// NanoVG layer only as a compatibility treatment for the legacy panel.
			conduitFramebuffer->setVisible(legacy);
			if (legacy) conduitFramebuffer->setDirty();
		}
		if (modernPanelBorder) modernPanelBorder->setVisible(!legacy);
		if (legacyTitleRaster) legacyTitleRaster->setVisible(legacy);
		if (legacyLabels) legacyLabels->setVisible(legacy);
		if (modernSpectrumFrame) modernSpectrumFrame->setVisible(!legacy);
		if (legacySpectrumFrame) legacySpectrumFrame->setVisible(legacy);
		if (legacyTopLeftScrew) legacyTopLeftScrew->setVisible(legacy);
		if (legacyTopRightScrew) legacyTopRightScrew->setVisible(legacy);
		applySpectrumRect(legacy ? legacySpectrumRectMm : modernSpectrumRectMm);
	}

	explicit BifurxWidget(Bifurx* module) {
		setModule(module);
		if (module) {
			analysisSubscriptionModule = module;
			analysisSubscriptionModule->subscribeAnalysisVisual();
			module->visualWatchdogEnabled.store(true, std::memory_order_relaxed);
		}
		Bifurx* displayModule = module;
		if (!displayModule) {
			browserPreviewModule.reset(new Bifurx());
			displayModule = browserPreviewModule.get();
			displayModule->params[Bifurx::MODE_PARAM].setValue(float(kBrowserPreviewMode));
			displayModule->params[Bifurx::LEVEL_PARAM].setValue(kBrowserPreviewLevel);
			displayModule->params[Bifurx::FREQ_PARAM].setValue(kBrowserPreviewFrequency);
			displayModule->params[Bifurx::RESO_PARAM].setValue(kBrowserPreviewResonance);
			displayModule->params[Bifurx::BALANCE_PARAM].setValue(kBrowserPreviewBalance);
			displayModule->params[Bifurx::SPAN_PARAM].setValue(kBrowserPreviewSpan);
			displayModule->params[Bifurx::FM_AMT_PARAM].setValue(kBrowserPreviewFmAmount);
			displayModule->params[Bifurx::SPAN_CV_ATTEN_PARAM].setValue(kBrowserPreviewSpanAttenuator);
			displayModule->params[Bifurx::TITO_PARAM].setValue(kBrowserPreviewTito);
			displayModule->lights[Bifurx::FM_AMT_POS_LIGHT].setBrightness(kBrowserPreviewFmAmount);
			displayModule->lights[Bifurx::TITO_XM_LIGHT].setBrightness(kBrowserPreviewTito);
		}
		PreviewBuildLogTimer previewBuildTimer("Bifurx", module);
		visual_assets::SplitPanelRenderer splitPanel(this, "res/bifurx.panel.svg", "res/bifurx.background.svg");
		legacySvgPanel = getPanel();
		const std::string& panelPath = splitPanel.panelPath();
		previewBuildTimer.markPanelDone();
		{
			widget::FramebufferWidget* backingFramebuffer = new BifurxVisibleFramebufferWidget();
			backingFramebuffer->box.size = box.size;
			backingFramebuffer->dirtyOnSubpixelChange = false;
			BifurxModernPanelBackingWidget* backing = new BifurxModernPanelBackingWidget();
			backing->box.size = box.size;
			backingFramebuffer->addChild(backing);
			modernPanelBacking = backingFramebuffer;
			addChildBottom(modernPanelBacking);
		}
		splitPanel.addPerfectWaveBranding();
		legacyPanelSurface = splitPanel.panelSurfaceEffectWidget();
		auto* legacyGlass = visual_assets::addFractalGlassOverlay(this, panelPath, legacyPanelSurface);
		legacyGlassFramebuffer = legacyGlass ? legacyGlass->parent : nullptr;
		math::Rect topRasterRectMm(
			Vec(0.f, 0.f), Vec(71.12f, 71.12f / (2141.f / 285.f)));
		panel_svg::loadRectFromSvgMm(panelPath, "BIFURX_TOP_RASTER", &topRasterRectMm);
		modernTopRaster = visual_assets::createAspectFitRasterImageWidget(
			"res/bifurx/Bifurx-LT.png", topRasterRectMm);
		addChild(modernTopRaster);
		modernTopRasterDark = visual_assets::createAspectFitRasterImageWidget(
			"res/bifurx/Bifurx-DT.png", topRasterRectMm);
		addChild(modernTopRasterDark);
		constexpr float kBottomRasterXmm = 0.4f;
		constexpr float kBottomRasterYmm = 77.5f;
		constexpr float kBottomRasterWidthMm = 70.32f;
		constexpr float kBottomRasterAspect = 1584.f / 993.f;
		const float bottomRasterHeightMm = kBottomRasterWidthMm / kBottomRasterAspect;
		const math::Rect bottomRasterRectMm(
			Vec(kBottomRasterXmm, kBottomRasterYmm),
			Vec(kBottomRasterWidthMm, bottomRasterHeightMm));
		modernBottomRaster = visual_assets::createAspectFitRasterImageWidget(
			"res/bifurx/Bifurx-LB.png", bottomRasterRectMm);
		addChild(modernBottomRaster);
		modernBottomRasterDark = visual_assets::createAspectFitRasterImageWidget(
			"res/bifurx/Bifurx-DB.png", bottomRasterRectMm);
		addChild(modernBottomRasterDark);
		modernPanelBorder = new app::PanelBorder();
		modernPanelBorder->box.size = box.size;
		addChild(modernPanelBorder);
		math::Rect leviathanLogoRectMm(
			Vec(19.200337f, 119.43102f),
			Vec(32.719331f, 12.24054f));
		panel_svg::loadRectFromSvgMm(
			panelPath, "BRANDING_LEVIATHAN_LOGO_RASTER", &leviathanLogoRectMm);
		addChild(visual_assets::createAspectFitRasterImageWidget(
			"res/icon/Leviathan_Logo_S2.png", leviathanLogoRectMm));
		math::Rect titleRasterRectMm(Vec(22.66f, 0.f), Vec(25.8f, 9.46667f));
		panel_svg::loadRectFromSvgMm(panelPath, "BIFURX_TITLE_RASTER", &titleRasterRectMm);
		legacyTitleRaster = visual_assets::createAspectFitRasterImageWidget(
			"res/icon/Bifurx-CS-96c.png", titleRasterRectMm);
		addChild(legacyTitleRaster);
		legacyTopLeftScrew = createWidget<CyanOrbScrew>(Vec(RACK_GRID_WIDTH, 0.f));
		legacyTopRightScrew = createWidget<CyanOrbScrew>(
			Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0.f));
		addChild(legacyTopLeftScrew);
		addChild(legacyTopRightScrew);
		addChild(createWidget<CyanOrbScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH))); addChild(createWidget<CyanOrbScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		auto applyPt = [&](const char* id, Vec* pos) { Vec p; if (panel_svg::loadPointFromSvgMm(panelPath, id, &p)) *pos = p; };
		modernSpectrumRectMm = math::Rect(Vec(0.789621f, 9.464366f), Vec(69.497729f, 63.360991f));
		panel_svg::loadRectFromSvgMm(panelPath, "SPECTRUM_MODERN", &modernSpectrumRectMm);
		legacySpectrumRectMm = math::Rect(Vec(0.789621f, 9.464366f), Vec(69.497729f, 63.360991f));
		panel_svg::loadRectFromSvgMm(panelPath, "SPECTRUM", &legacySpectrumRectMm);
		const math::Rect sRect = modernSpectrumRectMm;
		auto addFb = [&](math::Rect r, Widget* w) { widget::FramebufferWidget* fb = new BifurxVisibleFramebufferWidget(); fb->box.pos = mm2px(r.pos); fb->box.size = mm2px(r.size); fb->dirtyOnSubpixelChange = false; w->box.size = fb->box.size; fb->addChild(w); addChild(fb); return fb; };
		spectrumBackgroundContent = new BifurxSpectrumBackgroundWidget();
		spectrumBackgroundContent->module = displayModule;
		spectrumBackgroundFramebuffer = addFb(sRect, spectrumBackgroundContent);
		spectrumBackgroundContent->framebuffer = spectrumBackgroundFramebuffer;
		
		spectrumNanoVGContent = new BifurxSpectrumWidget();
		spectrumNanoVGContent->module = module;
		spectrumNanoVG = addFb(sRect, spectrumNanoVGContent);
		spectrumNanoVGContent->framebuffer = spectrumNanoVG;
		
		spectrumOpenGL = createGlSpectrumDisplay(module, sRect);
		spectrumOpenGLBase = dynamic_cast<BifurxSpectrumBase*>(spectrumOpenGL);
		addChild(spectrumOpenGL);
		modernSpectrumFrame = visual_assets::createPreviewFrameEnhancementWidget(
			modernSpectrumRectMm, nvgRGBA(28, 202, 216, 255));
		legacySpectrumFrame = visual_assets::createPreviewFrameEnhancementWidget(legacySpectrumRectMm);
		addChild(modernSpectrumFrame);
		addChild(legacySpectrumFrame);

		bool showGL = (module && module->renderMode == Bifurx::RENDER_OPENGL);
		lastRenderMode = showGL ? Bifurx::RENDER_OPENGL : Bifurx::RENDER_NANOVG;
		if (spectrumNanoVG) spectrumNanoVG->setVisible(!showGL);
		if (spectrumOpenGL) spectrumOpenGL->setVisible(showGL);

		modeReadout = new BifurxModeReadoutWidget();
		modeReadout->module = displayModule;
		modeReadout->box.pos = mm2px(Vec(sRect.pos.x, sRect.pos.y + sRect.size.y + 0.6f));
		modeReadout->box.size = mm2px(Vec(sRect.size.x, 4.2f));
		addChild(modeReadout);
		Vec mlP(10.9f, 22.f), mrP(15.9f, 22.f), mmP(8.9f, 22.f), lP(13.4f, 41.f), rP(13.4f, 60.f), fP(35.56f, 46.5f), tP(57.7f, 22.f), sP(57.7f, 41.f), bP(57.7f, 60.f), faP(25.3f, 45.f), saP(45.82f, 45.f);
		Vec iP(7.6f, 112.2f), vP(17.15f, 112.2f), fmP(26.7f, 112.2f), rcP(36.25f, 112.2f), bcP(45.8f, 112.2f), scP(55.35f, 112.2f), oP(64.9f, 112.2f);
		applyPt("MODE_LEFT_PARAM", &mlP); applyPt("MODE_RIGHT_PARAM", &mrP); applyPt("LEVEL_PARAM", &lP); applyPt("RESO_PARAM", &rP); applyPt("FREQ_PARAM", &fP); applyPt("TITO_PARAM", &tP); applyPt("SPAN_PARAM", &sP); applyPt("BALANCE_PARAM", &bP); applyPt("FM_AMT_PARAM", &faP); applyPt("SPAN_CV_ATTEN_PARAM", &saP);
		applyPt("MODE_MENU_BUTTON", &mmP);
		applyPt("IN_INPUT", &iP); applyPt("VOCT_INPUT", &vP); applyPt("FM_INPUT", &fmP); applyPt("RESO_CV_INPUT", &rcP); applyPt("BALANCE_CV_INPUT", &bcP); applyPt("SPAN_CV_INPUT", &scP); applyPt("OUT_OUTPUT", &oP);
		conduitFramebuffer = visual_assets::createPlasmaConduitLayer(
			panelPath, box.size, &lastConduitDrawUs);
		if (conduitFramebuffer) {
			addChild(conduitFramebuffer);
		}
		previewBuildTimer.setAtlasStatus(panel_svg::getAtlasStatusLabelForSvg(panelPath));
		previewBuildTimer.markAnchorsDone();
		auto addDisplayParam = [&](ParamWidget* param) {
			// The authored browser controls read from a private preview module, not
			// the ModuleWidget's null engine module, so bypass addParam() validation.
			if (module) addParam(param);
			else addChild(param);
		};
		auto* modeMenuButton = createParamCentered<BifurxModeMenuButton>(mm2px(mmP), displayModule, Bifurx::MODE_MENU_PARAM);
		modeMenuButton->module = displayModule;
		addDisplayParam(modeMenuButton);
		addDisplayParam(createParamCentered<BifurxModeLeftButton>(mm2px(mlP), displayModule, Bifurx::MODE_LEFT_PARAM)); addDisplayParam(createParamCentered<BifurxModeRightButton>(mm2px(mrP), displayModule, Bifurx::MODE_RIGHT_PARAM));
		const Vec freqCenterPx = mm2px(fP);
		addDisplayParam(createParamCentered<Eclipse2Knob>(mm2px(lP), displayModule, Bifurx::LEVEL_PARAM)); addDisplayParam(createParamCentered<LeviathanHaloKnob2>(freqCenterPx, displayModule, Bifurx::FREQ_PARAM)); addDisplayParam(createParamCentered<Eclipse2Knob>(mm2px(rP), displayModule, Bifurx::RESO_PARAM));
		{
			Eclipse2Knob* balanceKnob = createParamCentered<Eclipse2Knob>(mm2px(bP), displayModule, Bifurx::BALANCE_PARAM);
			balanceKnob->setProgressRingBipolar(true);
			addDisplayParam(balanceKnob);
		}
		addDisplayParam(createParamCentered<Eclipse2Knob>(mm2px(sP), displayModule, Bifurx::SPAN_PARAM)); addDisplayParam(createLightParamCentered<LuminSlider>(mm2px(faP), displayModule, Bifurx::FM_AMT_PARAM, Bifurx::FM_AMT_POS_LIGHT));
		addDisplayParam(createLightParamCentered<LuminSlider>(mm2px(saP), displayModule, Bifurx::SPAN_CV_ATTEN_PARAM, Bifurx::SPAN_CV_ATTEN_POS_LIGHT)); addDisplayParam(createParamCentered<BipolarDarkTinyClockworkGearKnob>(mm2px(tP), displayModule, Bifurx::TITO_PARAM));
		addChild(createLightCentered<SmallAperture<AmberApertureLight>>(mm2px(tP.plus(Vec(-7.0f, 0.f))), displayModule, Bifurx::TITO_SM_LIGHT));
		addChild(createLightCentered<SmallAperture<AmberApertureLight>>(mm2px(tP.plus(Vec(7.0f, 0.f))), displayModule, Bifurx::TITO_XM_LIGHT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(iP), module, Bifurx::IN_INPUT)); addInput(createInputCentered<Magitek2InputJack>(mm2px(vP), module, Bifurx::VOCT_INPUT)); addInput(createInputCentered<Magitek2InputJack>(mm2px(fmP), module, Bifurx::FM_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(rcP), module, Bifurx::RESO_CV_INPUT)); addInput(createInputCentered<Magitek2InputJack>(mm2px(bcP), module, Bifurx::BALANCE_CV_INPUT)); addInput(createInputCentered<Magitek2InputJack>(mm2px(scP), module, Bifurx::SPAN_CV_INPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(oP), module, Bifurx::OUT_OUTPUT));
		legacyLabels = visual_assets::createThemedPanelLabelsWidget(
			"res/bifurx.labels.svg", "res/bifurx.theme-text-input.svg", "res/bifurx.theme-text-output.svg",
			box.size, this);
		addChild(legacyLabels);
		lastLegacyVisuals = module
			? module->legacyVisuals.load(std::memory_order_relaxed)
			: false;
		lastPreferDarkPanels = settings::preferDarkPanels;
		applyLegacyVisuals(lastLegacyVisuals);
#if defined(LEVIATHAN_PRO_DRM) && LEVIATHAN_PRO_DRM
		// Added last so the official license overlay covers all interactive controls.
		auto* licenseOverlay = new drm::ModuleOverlay;
		licenseOverlay->setContext(leviathanDrmContext);
		addChild(licenseOverlay);
#endif
	}

	~BifurxWidget() override {
		if (analysisSubscriptionModule) {
			analysisSubscriptionModule->unsubscribeAnalysisVisual();
			analysisSubscriptionModule = nullptr;
		}
	}

	void step() override {
		using PerfClock = std::chrono::steady_clock;
		Bifurx* bifurx = dynamic_cast<Bifurx*>(module);
		const bool measurePerf = bifurx && isDragonKingDebugEnabled();
		const PerfClock::time_point perfStepStart = measurePerf ? PerfClock::now() : PerfClock::time_point();
		const bool preferDarkPanelsNow = settings::preferDarkPanels;
		if (preferDarkPanelsNow != lastPreferDarkPanels) {
			lastPreferDarkPanels = preferDarkPanelsNow;
			applyLegacyVisuals(lastLegacyVisuals);
		}
		const Rect viewport = getViewport(Rect(Vec(), box.size));
		const bool windowActive = !APP || !APP->window || !APP->window->win
			|| (glfwGetWindowAttrib(APP->window->win, GLFW_VISIBLE) && !glfwGetWindowAttrib(APP->window->win, GLFW_ICONIFIED));
		const bool visualActive = isVisible() && viewport.size.x > 0.f && viewport.size.y > 0.f && windowActive;
		if (bifurx) {
			if (visualActive) bifurx->visualHeartbeat.fetch_add(1, std::memory_order_relaxed);
			if (visualActive && !analysisSubscriptionModule) { analysisSubscriptionModule = bifurx; bifurx->subscribeAnalysisVisual(); }
			else if (!visualActive && analysisSubscriptionModule) { analysisSubscriptionModule->unsubscribeAnalysisVisual(); analysisSubscriptionModule = nullptr; }
		}
		bool showGL = false;
		if (bifurx) {
			const bool legacyVisualsNow = bifurx->legacyVisuals.load(std::memory_order_relaxed);
			if (legacyVisualsNow != lastLegacyVisuals) {
				lastLegacyVisuals = legacyVisualsNow;
				applyLegacyVisuals(lastLegacyVisuals);
			}
			showGL = bifurx->renderMode == Bifurx::RENDER_OPENGL;
			const int renderModeNow = showGL ? Bifurx::RENDER_OPENGL : Bifurx::RENDER_NANOVG;
			if (renderModeNow != lastRenderMode) {
				lastRenderMode = renderModeNow;
				if (spectrumNanoVG) spectrumNanoVG->setVisible(!showGL);
				if (spectrumOpenGL) spectrumOpenGL->setVisible(showGL);
				if (showGL && spectrumNanoVGContent) spectrumNanoVGContent->releaseWorkerRegistration();
				if (!showGL && spectrumOpenGLBase) spectrumOpenGLBase->releaseWorkerRegistration();
			}
		}
		if (bifurx) {
			if (spectrumNanoVG) spectrumNanoVG->setVisible(visualActive && !showGL);
			if (spectrumOpenGL) spectrumOpenGL->setVisible(visualActive && showGL);
			if (spectrumNanoVGContent) spectrumNanoVGContent->presentationActive = visualActive && !showGL;
			if (spectrumOpenGLBase) spectrumOpenGLBase->presentationActive = visualActive && showGL;
			if (!visualActive) {
				if (spectrumNanoVGContent) spectrumNanoVGContent->releaseWorkerRegistration();
				if (spectrumOpenGLBase) spectrumOpenGLBase->releaseWorkerRegistration();
			}
		}
		ModuleWidget::step();
		BifurxSpectrumBase* activeSpectrum = showGL ? spectrumOpenGLBase : static_cast<BifurxSpectrumBase*>(spectrumNanoVGContent);
		if (spectrumNanoVGContent) {
			spectrumNanoVGContent->syncCurveDebugCaptureState();
			spectrumNanoVGContent->syncPerfDebugCaptureState();
			if (measurePerf && activeSpectrum) spectrumNanoVGContent->recordCurveDebug(*activeSpectrum);
		}
		if (!measurePerf) return;

		const float stepUs = float(std::chrono::duration_cast<std::chrono::nanoseconds>(
			PerfClock::now() - perfStepStart).count()) * 1e-3f;
		moduleStepUsRange.add(stepUs);
		const double nowSec = system::getTime();
		if (!shouldSubmitBifurxDebugPacket(bifurx->debugInstanceId, nowSec)) return;
		const bool fixedSurfaceActive = showGL
			&& bifurx->fixedGlSurfaceEnabled.load(std::memory_order_relaxed);
		const auto processRange = debug_terminal::consumeAudioProcessTiming(
			bifurx->perfAudioProcessRangeMinNs, bifurx->perfAudioProcessRangeMaxNs, &bifurx->perfAudioProcessRangeAverage);
		const auto stepRange = moduleStepUsRange.consume();
		const auto drawRange = moduleDrawUsRange.consume();
		if (spectrumNanoVGContent) spectrumNanoVGContent->logPerfDebugSample(processRange, stepRange, drawRange, activeSpectrum, showGL);
		debug_terminal::submitBifurxUiMetrics(
			bifurx->debugInstanceId,
			processRange,
			stepRange,
			drawRange,
			showGL,
			activeSpectrum ? activeSpectrum->lastCurvePrepUs : 0.f,
			activeSpectrum ? activeSpectrum->lastOverlayPrepUs : 0.f,
			fixedSurfaceActive && activeSpectrum ? activeSpectrum->lastSurfaceRenderUs : 0.f,
			activeSpectrum ? activeSpectrum->renderClient.lastSubmitUs() : 0.f,
			lastConduitDrawUs);
	}

	void draw(const DrawArgs& args) override {
		using PerfClock = std::chrono::steady_clock;
		const bool measurePerf = isDragonKingDebugEnabled();
		const PerfClock::time_point perfDrawStart = measurePerf ? PerfClock::now() : PerfClock::time_point();
		ModuleWidget::draw(args);
		Bifurx* bifurx = dynamic_cast<Bifurx*>(module);
		if (bifurx && bifurx->renderMode == Bifurx::RENDER_OPENGL && spectrumOpenGL && spectrumOpenGL->visible) {
			nvgSave(args.vg);
			nvgTranslate(args.vg, spectrumOpenGL->box.pos.x, spectrumOpenGL->box.pos.y);
			if (spectrumOpenGLBase) {
				spectrumOpenGLBase->drawNanoVG(args);
			}
			nvgRestore(args.vg);
		}
		if (bifurx && isDragonKingDebugEnabled() && APP && APP->window && APP->window->uiFont) {
			char debugIdLabel[32];
			std::snprintf(debugIdLabel, sizeof(debugIdLabel), "ID:%u", bifurx->debugInstanceId);
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
		if (bifurx && measurePerf) {
			const float drawMs = float(std::chrono::duration_cast<std::chrono::nanoseconds>(
				PerfClock::now() - perfDrawStart).count()) * 1e-6f;
			moduleDrawUsRange.add(drawMs * 1000.f);
			const float prevMs = bifurx->perfUiRenderMs.load(std::memory_order_relaxed);
			const float emaMs = (prevMs > 0.f) ? (prevMs + (drawMs - prevMs) * 0.18f) : drawMs;
			bifurx->perfUiRenderMs.store(std::max(0.f, emaMs), std::memory_order_relaxed);
		}
	}

	void appendContextMenu(Menu* menu) override {
		ModuleWidget::appendContextMenu(menu); Bifurx* bifurx = dynamic_cast<Bifurx*>(module); if (!bifurx) return;
		auto rendererLabel = [=]() {
			if (bifurx->renderMode == Bifurx::RENDER_NANOVG) return "NanoVG";
			return bifurx->useGlShaderRenderer.load(std::memory_order_relaxed)
				? "OpenGL SHDR" : "OpenGL";
		};
		auto setRenderStateWithHistory = [=](Bifurx::RenderMode newMode, bool newUseShaderRenderer) {
			if (!bifurx || (bifurx->renderMode == newMode && bifurx->useGlShaderRenderer.load(std::memory_order_relaxed) == newUseShaderRenderer)) return;
			if (APP && APP->history) {
				history::ModuleChange* h = new history::ModuleChange();
				h->name = "change render engine";
				h->moduleId = bifurx->id;
				h->oldModuleJ = bifurx->toJson();
				bifurx->renderMode = newMode;
				bifurx->useGlShaderRenderer.store(newUseShaderRenderer, std::memory_order_relaxed);
				h->newModuleJ = bifurx->toJson();
				APP->history->push(h);
			}
			else {
				bifurx->renderMode = newMode;
				bifurx->useGlShaderRenderer.store(newUseShaderRenderer, std::memory_order_relaxed);
			}
		};
		menu->addChild(new MenuSeparator());
			menu->addChild(createSubmenuItem("Modulation Quality", "", [=](Menu* submenu) {
				submenu->addChild(createCheckMenuItem(
					"Balanced — Control rate (/16)", "",
					[=]() { return bifurx->modulationQualityMode.load(std::memory_order_relaxed) == Bifurx::MOD_QUALITY_BALANCED; },
					[=]() {
						setBifurxSettingWithHistory<int>(bifurx, bifurx->modulationQualityMode, Bifurx::MOD_QUALITY_BALANCED, "change modulation quality");
					}));
				submenu->addChild(createCheckMenuItem(
					"High — Control rate (/8)", "",
					[=]() { return bifurx->modulationQualityMode.load(std::memory_order_relaxed) == Bifurx::MOD_QUALITY_HIGH; },
					[=]() {
						setBifurxSettingWithHistory<int>(bifurx, bifurx->modulationQualityMode, Bifurx::MOD_QUALITY_HIGH, "change modulation quality");
					}));
				submenu->addChild(createCheckMenuItem(
					"Exact — Audio rate (/1)", "",
					[=]() { return bifurx->modulationQualityMode.load(std::memory_order_relaxed) == Bifurx::MOD_QUALITY_EXACT; },
					[=]() {
						setBifurxSettingWithHistory<int>(bifurx, bifurx->modulationQualityMode, Bifurx::MOD_QUALITY_EXACT, "change modulation quality");
					}));
			}));
			menu->addChild(createSubmenuItem("Color Scheme", "", [=](Menu* submenu) {
				auto addSchemeItem = [=](Bifurx::ColorScheme scheme, const std::string& label) {
					submenu->addChild(createCheckMenuItem(
						label, "",
						[=]() { return bifurx->colorScheme == scheme; },
						[=]() { bifurx->colorScheme = scheme; }
					));
				};
				addSchemeItem(Bifurx::SCHEME_DEFAULT, "Default (Purple/Cyan)");
				addSchemeItem(Bifurx::SCHEME_CLASSIC, "Classic (Green/Red)");
				addSchemeItem(Bifurx::SCHEME_MONOCHROME, "Monochrome (Gray/White)");
				addSchemeItem(Bifurx::SCHEME_FIRE, "Fire (Red/Yellow)");
				addSchemeItem(Bifurx::SCHEME_RETRO_AMBER, "Retro Amber");
				addSchemeItem(Bifurx::SCHEME_RETRO_GREEN, "Retro Green");
			}));
			menu->addChild(createCheckMenuItem(
				"Three-color FFT gradient", "",
				[=]() { return bifurx->threeColorFftGradient.load(std::memory_order_relaxed); },
				[=]() {
					const bool enabled = bifurx->threeColorFftGradient.load(std::memory_order_relaxed);
					bifurx->threeColorFftGradient.store(!enabled, std::memory_order_relaxed);
				}));
			menu->addChild(createCheckMenuItem(
				"Legacy visuals", "",
				[=]() { return bifurx->legacyVisuals.load(std::memory_order_relaxed); },
				[=]() {
					const bool enabled = bifurx->legacyVisuals.load(std::memory_order_relaxed);
					bifurx->legacyVisuals.store(!enabled, std::memory_order_relaxed);
				}));
			menu->addChild(createSubmenuItem("Renderer", rendererLabel(), [=](Menu* submenu) {
			submenu->addChild(createCheckMenuItem(
				"NanoVG", "",
				[=]() { return bifurx->renderMode == Bifurx::RENDER_NANOVG; },
				[=]() { setRenderStateWithHistory(Bifurx::RENDER_NANOVG, false); }));
			submenu->addChild(createCheckMenuItem(
				"OpenGL", "",
				[=]() { return bifurx->renderMode == Bifurx::RENDER_OPENGL && !bifurx->useGlShaderRenderer.load(std::memory_order_relaxed); },
				[=]() { setRenderStateWithHistory(Bifurx::RENDER_OPENGL, false); }));
			submenu->addChild(createCheckMenuItem(
				"OpenGL SHDR", "",
				[=]() { return bifurx->renderMode == Bifurx::RENDER_OPENGL && bifurx->useGlShaderRenderer.load(std::memory_order_relaxed); },
				[=]() { setRenderStateWithHistory(Bifurx::RENDER_OPENGL, true); }));
			}));
			menu->addChild(createCheckMenuItem("High Resonance Self-Osc", "",
				[=]() { return bifurx->highResonanceSelfOscEnabled.load(std::memory_order_relaxed); },
				[=]() { setBifurxSettingWithHistory(bifurx, bifurx->highResonanceSelfOscEnabled, !bifurx->highResonanceSelfOscEnabled.load(std::memory_order_relaxed), "change self-oscillation"); }));
			menu->addChild(createCheckMenuItem("Soft Limiting", "",
				[=]() { return bifurx->softLimitingEnabled.load(std::memory_order_relaxed); },
				[=]() { setBifurxSettingWithHistory(bifurx, bifurx->softLimitingEnabled, !bifurx->softLimitingEnabled.load(std::memory_order_relaxed), "change limiting"); }));
			menu->addChild(createCheckMenuItem("Dynamic FFT Scale", "",
				[=]() { return bifurx->fftScaleDynamic.load(std::memory_order_relaxed); },
				[=]() { bifurx->fftScaleDynamic.store(!bifurx->fftScaleDynamic.load(std::memory_order_relaxed), std::memory_order_relaxed); }));
			menu->addChild(createCheckMenuItem("Show Module Response", "",
				[=]() { return bifurx->showModuleResponseOverlay.load(std::memory_order_relaxed); },
				[=]() { bifurx->showModuleResponseOverlay.store(!bifurx->showModuleResponseOverlay.load(std::memory_order_relaxed), std::memory_order_relaxed); }));
			if (isDragonKingDebugEnabled()) {
				menu->addChild(new MenuSeparator());
				menu->addChild(createMenuLabel("Debug Audio"));
				menu->addChild(createCheckMenuItem(
					"Selective 2x nonlinear oversampling", "",
					[=]() { return bifurx->nonlinearOversamplingEnabled.load(std::memory_order_relaxed); },
					[=]() {
						bifurx->nonlinearOversamplingEnabled.store(
							!bifurx->nonlinearOversamplingEnabled.load(std::memory_order_relaxed),
							std::memory_order_relaxed);
					}));
				menu->addChild(createSubmenuItem("Oversampling boundary", "", [=](Menu* submenu) {
					const char* labels[] = {"Legacy dark boundary FIR", "Flat-passband FIR", "Low-latency IIR4"};
					for (int boundary = 1; boundary <= 3; ++boundary) {
						submenu->addChild(createCheckMenuItem(labels[boundary - 1], "",
							[=]() { return bifurx->boundaryResampling.load(std::memory_order_relaxed) == boundary; },
							[=]() { setBifurxSettingWithHistory(bifurx, bifurx->boundaryResampling, boundary, "change oversampling boundary"); }));
					}
				}));
				menu->addChild(new MenuSeparator());
				menu->addChild(createMenuLabel("Debug Rendering"));
				menu->addChild(createCheckMenuItem(
					"Context-owned fixed GL surface", "",
					[=]() { return bifurx->fixedGlSurfaceEnabled.load(std::memory_order_relaxed); },
					[=]() {
						bifurx->fixedGlSurfaceEnabled.store(
							!bifurx->fixedGlSurfaceEnabled.load(std::memory_order_relaxed),
							std::memory_order_relaxed);
					}));
			}
			menu->addChild(createCheckMenuItem("Low Latency Offload", "",
				[=]() { return bifurx->lowLatencyVisual.load(std::memory_order_relaxed); },
				[=]() { bifurx->lowLatencyVisual.store(!bifurx->lowLatencyVisual.load(std::memory_order_relaxed), std::memory_order_relaxed); }));
			menu->addChild(createCheckMenuItem(
				"Disable Visual Offload", "",
				[=]() { return bifurx->visualWorkerMode.load(std::memory_order_relaxed) == Bifurx::VISUAL_WORKER_OFF; },
				[=]() {
					const bool disabledNow = bifurx->visualWorkerMode.load(std::memory_order_relaxed) == Bifurx::VISUAL_WORKER_OFF;
					bifurx->visualWorkerMode.store(
						disabledNow ? Bifurx::VISUAL_WORKER_INHERIT : Bifurx::VISUAL_WORKER_OFF,
						std::memory_order_relaxed
					);
				}
			));
		if (isDragonKingDebugEnabled()) {
			menu->addChild(createCheckMenuItem("Log Curve Debug", "",
				[=]() { return bifurx->curveDebugLogging.load(std::memory_order_relaxed); },
				[=]() { bifurx->curveDebugLogging.store(!bifurx->curveDebugLogging.load(std::memory_order_relaxed), std::memory_order_relaxed); }));
			menu->addChild(createCheckMenuItem("Log Performance Debug", "",
				[=]() { return bifurx->perfDebugLogging.load(std::memory_order_relaxed); },
				[=]() { bifurx->perfDebugLogging.store(!bifurx->perfDebugLogging.load(std::memory_order_relaxed), std::memory_order_relaxed); }));
		}
	}
};

} // namespace bifurx

Model* modelBifurx = createModel<bifurx::Bifurx, bifurx::BifurxWidget>("Bifurx");
