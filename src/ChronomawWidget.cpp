#include "Chronomaw.hpp"
#include "PanelSvgUtils.hpp"
#include <array>
#include <string>

namespace {

struct ChronomawActionButton : TL1105 {};

struct ChronomawUiRects {
	math::Rect globalBar;
	math::Rect overview;
	math::Rect timeline;
	math::Rect inspector;
};

static NVGcolor chronomawRgb(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255) {
	return nvgRGBA(r, g, b, a);
}

static void drawRectFilled(const Widget::DrawArgs& args, const math::Rect& rect, NVGcolor fill, NVGcolor stroke) {
	nvgBeginPath(args.vg);
	nvgRoundedRect(args.vg, rect.pos.x, rect.pos.y, rect.size.x, rect.size.y, 4.0f);
	nvgFillColor(args.vg, fill);
	nvgFill(args.vg);
	nvgStrokeWidth(args.vg, 1.0f);
	nvgStrokeColor(args.vg, stroke);
	nvgStroke(args.vg);
}

static void drawLabel(const Widget::DrawArgs& args, float x, float y, int align, float size, NVGcolor color, const std::string& text) {
	if (!APP || !APP->window || !APP->window->uiFont) {
		return;
	}
	nvgFontSize(args.vg, size);
	nvgFontFaceId(args.vg, APP->window->uiFont->handle);
	nvgTextAlign(args.vg, align);
	nvgFillColor(args.vg, color);
	nvgText(args.vg, x, y, text.c_str(), nullptr);
}

struct ChronomawSurfaceWidget : Widget {
	Chronomaw* module = nullptr;
	ChronomawUiRects uiRects;

	static constexpr int kTabCount = 7;

	static const char* tabName(int index) {
		static const char* const kNames[kTabCount] = {"Timing", "Shape", "Pattern", "Cross", "CV", "Quant", "Store"};
		const int clamped = clamp(index, 0, kTabCount - 1);
		return kNames[clamped];
	}

	explicit ChronomawSurfaceWidget(Chronomaw* module, const ChronomawUiRects& uiRects) : module(module), uiRects(uiRects) {}

	int selectedOutput() const {
		if (!module) {
			return 0;
		}
		return clamp(module->state.ui.selectedOutput, 0, chronomaw::kNumOutputs - 1);
	}

	int selectedTab() const {
		if (!module) {
			return 0;
		}
		return std::max(0, module->state.ui.selectedTab);
	}

	chronomaw::DensityMode densityMode() const {
		if (!module) {
			return chronomaw::DensityMode::Monitor;
		}
		return module->state.live.density;
	}

	math::Rect timelineRectForDensity() const {
		const chronomaw::DensityMode density = densityMode();
		if (density == chronomaw::DensityMode::Edit) {
			return math::Rect(uiRects.timeline.pos, Vec(uiRects.timeline.size.x, std::max(24.f, uiRects.timeline.size.y * 0.38f)));
		}
		if (density == chronomaw::DensityMode::Focus) {
			return math::Rect(uiRects.timeline.pos, Vec(uiRects.timeline.size.x, std::max(18.f, uiRects.timeline.size.y * 0.25f)));
		}
		return uiRects.timeline;
	}

	math::Rect inspectorRectForDensity() const {
		const chronomaw::DensityMode density = densityMode();
		if (density == chronomaw::DensityMode::Monitor) {
			return math::Rect(uiRects.inspector.pos, Vec(uiRects.inspector.size.x, std::max(38.f, uiRects.inspector.size.y * 0.34f)));
		}
		if (density == chronomaw::DensityMode::Focus) {
			const float y = uiRects.timeline.pos.y + std::max(24.f, uiRects.timeline.size.y * 0.25f) + 3.f;
			const float h = std::max(44.f, uiRects.inspector.pos.y + uiRects.inspector.size.y - y);
			return math::Rect(Vec(uiRects.inspector.pos.x, y), Vec(uiRects.inspector.size.x, h));
		}
		return uiRects.inspector;
	}

	int outputRowAt(const Vec& p) const {
		if (!uiRects.overview.contains(p)) {
			return -1;
		}
		const float rowH = uiRects.overview.size.y / float(chronomaw::kNumOutputs);
		if (rowH <= 1.f) {
			return -1;
		}
		const int row = int((p.y - uiRects.overview.pos.y) / rowH);
		return clamp(row, 0, chronomaw::kNumOutputs - 1);
	}

	int tabAt(const Vec& p, const math::Rect& inspectorRect) const {
		const float tabStripH = 16.f;
		math::Rect tabRect(inspectorRect.pos, Vec(inspectorRect.size.x, tabStripH));
		if (!tabRect.contains(p)) {
			return -1;
		}
		const float tabW = inspectorRect.size.x / float(kTabCount);
		if (tabW <= 1.f) {
			return -1;
		}
		return clamp(int((p.x - inspectorRect.pos.x) / tabW), 0, kTabCount - 1);
	}

	void onButton(const event::Button& e) override {
		if (!module || e.button != GLFW_MOUSE_BUTTON_LEFT || e.action != GLFW_PRESS) {
			return;
		}

		const Vec local = e.pos;
		const int row = outputRowAt(local);
		if (row >= 0) {
			module->state.ui.selectedOutput = row;
			module->params[Chronomaw::SELECTED_OUTPUT_PARAM].setValue(float(row + 1));
			e.consume(this);
			return;
		}

		const math::Rect inspectorRect = inspectorRectForDensity();
		const int tab = tabAt(local, inspectorRect);
		if (tab >= 0) {
			module->state.ui.selectedTab = tab;
			e.consume(this);
		}
	}

	void drawOverview(const DrawArgs& args) {
		drawRectFilled(args, uiRects.overview, chronomawRgb(8, 16, 24, 148), chronomawRgb(116, 158, 190, 180));
		const float rowH = uiRects.overview.size.y / float(chronomaw::kNumOutputs);
		const int selected = selectedOutput();
		for (int i = 0; i < chronomaw::kNumOutputs; ++i) {
			math::Rect rowRect(
				Vec(uiRects.overview.pos.x + 1.5f, uiRects.overview.pos.y + rowH * float(i) + 1.f),
				Vec(uiRects.overview.size.x - 3.f, rowH - 1.8f)
			);
			const bool isSelected = (i == selected);
			drawRectFilled(
				args,
				rowRect,
				isSelected ? chronomawRgb(34, 68, 98, 210) : chronomawRgb(14, 24, 33, 168),
				isSelected ? chronomawRgb(154, 212, 255, 232) : chronomawRgb(84, 118, 143, 174)
			);
			drawLabel(
				args,
				rowRect.pos.x + 5.f,
				rowRect.pos.y + 0.5f * rowRect.size.y,
				NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE,
				9.5f,
				chronomawRgb(222, 238, 250, 240),
				"Out " + std::to_string(i + 1)
			);
			drawLabel(
				args,
				rowRect.pos.x + rowRect.size.x - 6.f,
				rowRect.pos.y + 0.5f * rowRect.size.y,
				NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE,
				8.4f,
				chronomawRgb(184, 214, 236, 220),
				(i == selected ? "SEL" : "--")
			);
		}
		drawLabel(
			args,
			uiRects.overview.pos.x + 4.f,
			uiRects.overview.pos.y - 2.5f,
			NVG_ALIGN_LEFT | NVG_ALIGN_BOTTOM,
			9.4f,
			chronomawRgb(180, 226, 251, 236),
			"Overview"
		);
	}

	void drawTimeline(const DrawArgs& args, const math::Rect& timelineRect) {
		drawRectFilled(args, timelineRect, chronomawRgb(9, 17, 28, 156), chronomawRgb(99, 139, 170, 186));
		const float laneH = timelineRect.size.y / float(chronomaw::kNumOutputs);
		for (int i = 1; i < chronomaw::kNumOutputs; ++i) {
			const float y = timelineRect.pos.y + laneH * float(i);
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, timelineRect.pos.x + 1.5f, y);
			nvgLineTo(args.vg, timelineRect.pos.x + timelineRect.size.x - 1.5f, y);
			nvgStrokeWidth(args.vg, 1.f);
			nvgStrokeColor(args.vg, chronomawRgb(74, 102, 128, 156));
			nvgStroke(args.vg);
		}
		const float nowX = timelineRect.pos.x + timelineRect.size.x * 0.28f;
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, nowX, timelineRect.pos.y + 1.2f);
		nvgLineTo(args.vg, nowX, timelineRect.pos.y + timelineRect.size.y - 1.2f);
		nvgStrokeWidth(args.vg, 1.7f);
		nvgStrokeColor(args.vg, chronomawRgb(236, 252, 255, 238));
		nvgStroke(args.vg);
		drawLabel(args, nowX + 2.6f, timelineRect.pos.y + 1.2f, NVG_ALIGN_LEFT | NVG_ALIGN_TOP, 8.0f, chronomawRgb(232, 247, 255, 220), "now");
		drawLabel(args, timelineRect.pos.x + 4.f, timelineRect.pos.y - 2.5f, NVG_ALIGN_LEFT | NVG_ALIGN_BOTTOM, 9.4f, chronomawRgb(180, 226, 251, 236), "Timeline");
	}

	void drawInspector(const DrawArgs& args, const math::Rect& inspectorRect) {
		drawRectFilled(args, inspectorRect, chronomawRgb(10, 16, 22, 170), chronomawRgb(96, 136, 160, 184));
		const float tabStripH = 16.f;
		const float tabW = inspectorRect.size.x / float(kTabCount);
		const int tabSel = clamp(selectedTab(), 0, kTabCount - 1);
		for (int i = 0; i < kTabCount; ++i) {
			math::Rect tabRect(
				Vec(inspectorRect.pos.x + tabW * float(i), inspectorRect.pos.y),
				Vec(tabW, tabStripH)
			);
			const bool active = (i == tabSel);
			drawRectFilled(
				args,
				tabRect,
				active ? chronomawRgb(30, 58, 80, 220) : chronomawRgb(14, 22, 30, 180),
				active ? chronomawRgb(170, 222, 250, 220) : chronomawRgb(82, 116, 136, 164)
			);
			drawLabel(
				args,
				tabRect.pos.x + 0.5f * tabRect.size.x,
				tabRect.pos.y + 0.5f * tabRect.size.y + 0.3f,
				NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE,
				8.0f,
				active ? chronomawRgb(238, 248, 255, 245) : chronomawRgb(170, 197, 215, 228),
				tabName(i)
			);
		}
		const int out = selectedOutput() + 1;
		drawLabel(
			args,
			inspectorRect.pos.x + 5.f,
			inspectorRect.pos.y + tabStripH + 10.f,
			NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE,
			10.2f,
			chronomawRgb(230, 242, 255, 240),
			"Output " + std::to_string(out)
		);
		drawLabel(
			args,
			inspectorRect.pos.x + 5.f,
			inspectorRect.pos.y + tabStripH + 24.f,
			NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE,
			8.6f,
			chronomawRgb(176, 205, 228, 230),
			"Phase 1 scaffold"
		);
		drawLabel(
			args,
			inspectorRect.pos.x + 5.f,
			inspectorRect.pos.y + tabStripH + 36.f,
			NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE,
			8.6f,
			chronomawRgb(154, 186, 209, 225),
			"Direct row+tab editing path"
		);
		drawLabel(args, inspectorRect.pos.x + 4.f, inspectorRect.pos.y - 2.5f, NVG_ALIGN_LEFT | NVG_ALIGN_BOTTOM, 9.4f, chronomawRgb(180, 226, 251, 236), "Inspector");
	}

	void draw(const DrawArgs& args) override {
		if (!module) {
			return;
		}
		drawRectFilled(args, uiRects.globalBar, chronomawRgb(8, 13, 20, 98), chronomawRgb(96, 136, 162, 140));
		drawOverview(args);
		const math::Rect timelineRect = timelineRectForDensity();
		const math::Rect inspectorRect = inspectorRectForDensity();
		drawTimeline(args, timelineRect);
		drawInspector(args, inspectorRect);

		const char* densityLabel = "Monitor";
		if (densityMode() == chronomaw::DensityMode::Edit) {
			densityLabel = "Edit";
		}
		else if (densityMode() == chronomaw::DensityMode::Focus) {
			densityLabel = "Focus";
		}
		drawLabel(args, uiRects.globalBar.pos.x + uiRects.globalBar.size.x - 3.f, uiRects.globalBar.pos.y + 2.f, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP, 8.5f, chronomawRgb(179, 220, 244, 236), std::string("Density: ") + densityLabel);
	}
};

} // namespace

ChronomawWidget::ChronomawWidget(Chronomaw* module) {
	setModule(module);
	const std::string panelPath = asset::plugin(pluginInstance, "res/chronomaw.svg");
	setPanel(createPanel(panelPath));

	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2.f * RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2.f * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

	Vec runPos(27.0f, 16.0f);
	Vec bpmPos(47.0f, 16.0f);
	Vec activeBankPos(67.0f, 16.0f);
	Vec loadBankPos(88.0f, 16.0f);
	Vec saveBankPos(98.0f, 16.0f);
	Vec selectedOutputPos(133.0f, 16.0f);
	Vec densityModePos(153.0f, 16.0f);
	Vec clkInPos(9.5f, 22.0f);
	Vec runInPos(9.5f, 35.5f);
	Vec resetInPos(9.5f, 49.0f);
	Vec cv1InPos(9.5f, 67.0f);
	Vec cv2InPos(9.5f, 80.5f);
	Vec cv3InPos(9.5f, 94.0f);
	Vec cv4InPos(9.5f, 107.5f);
	Vec runLightPos(32.5f, 16.0f);
	Vec syncLightPos(173.0f, 16.0f);
	std::array<Vec, chronomaw::kNumOutputs> outPos {};
	std::array<Vec, chronomaw::kNumOutputs> outLightPos {};
	ChronomawUiRects uiRects;
	uiRects.globalBar = math::Rect(Vec(19.f, 7.f), Vec(160.f, 18.f));
	uiRects.overview = math::Rect(Vec(22.f, 29.f), Vec(68.f, 84.f));
	uiRects.timeline = math::Rect(Vec(95.f, 29.f), Vec(86.f, 48.f));
	uiRects.inspector = math::Rect(Vec(95.f, 80.f), Vec(86.f, 33.f));
	for (int i = 0; i < chronomaw::kNumOutputs; ++i) {
		const float y = 18.5f + 13.0f * float(i);
		outPos[size_t(i)] = Vec(193.5f, y);
		outLightPos[size_t(i)] = Vec(184.8f, y);
	}

	auto applyPointOverride = [&](const char* elementId, Vec* outPosMm) {
		Vec pointMm;
		if (panel_svg::loadPointFromSvgMm(panelPath, elementId, &pointMm)) {
			*outPosMm = pointMm;
		}
	};
	auto applyRectOverride = [&](const char* elementId, math::Rect* outRectMm) {
		math::Rect rectMm;
		if (panel_svg::loadRectFromSvgMm(panelPath, elementId, &rectMm)) {
			*outRectMm = rectMm;
		}
	};

	applyPointOverride("RUN", &runPos);
	applyPointOverride("BPM", &bpmPos);
	applyPointOverride("ACTIVE_BANK", &activeBankPos);
	applyPointOverride("LOAD_BANK", &loadBankPos);
	applyPointOverride("SAVE_BANK", &saveBankPos);
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
	applyRectOverride("GLOBAL_BAR_RECT", &uiRects.globalBar);
	applyRectOverride("OVERVIEW_RECT", &uiRects.overview);
	applyRectOverride("TIMELINE_RECT", &uiRects.timeline);
	applyRectOverride("INSPECTOR_RECT", &uiRects.inspector);
	for (int i = 0; i < chronomaw::kNumOutputs; ++i) {
		const std::string outId = "OUT_" + std::to_string(i + 1) + "_OUTPUT";
		const std::string lightId = "OUT_" + std::to_string(i + 1) + "_LIGHT";
		applyPointOverride(outId.c_str(), &outPos[size_t(i)]);
		applyPointOverride(lightId.c_str(), &outLightPos[size_t(i)]);
	}

	auto* surface = new ChronomawSurfaceWidget(module, ChronomawUiRects{
		math::Rect(mm2px(uiRects.globalBar.pos), mm2px(uiRects.globalBar.size)),
		math::Rect(mm2px(uiRects.overview.pos), mm2px(uiRects.overview.size)),
		math::Rect(mm2px(uiRects.timeline.pos), mm2px(uiRects.timeline.size)),
		math::Rect(mm2px(uiRects.inspector.pos), mm2px(uiRects.inspector.size)),
	});
	surface->box.pos = Vec(0.f, 0.f);
	surface->box.size = box.size;
	addChild(surface);

	addParam(createParamCentered<CKD6>(mm2px(runPos), module, Chronomaw::RUN_PARAM));
	addParam(createParamCentered<BefacoTinyKnobWhite>(mm2px(bpmPos), module, Chronomaw::BPM_PARAM));
	addParam(createParamCentered<BefacoTinyKnobWhite>(mm2px(activeBankPos), module, Chronomaw::ACTIVE_BANK_PARAM));
	addParam(createParamCentered<ChronomawActionButton>(mm2px(loadBankPos), module, Chronomaw::LOAD_BANK_PARAM));
	addParam(createParamCentered<ChronomawActionButton>(mm2px(saveBankPos), module, Chronomaw::SAVE_BANK_PARAM));
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
