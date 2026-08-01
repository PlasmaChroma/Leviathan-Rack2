#include "Mandelwake.hpp"
#include "PanelSvgUtils.hpp"
#include "visual/FractalGlassOverlay.hpp"
#include "visual/VisualAssets.hpp"

#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>

namespace {

bool loadAnchorPointMm(const std::string& panelPath, const char* id, Vec* out, const Vec& fallback) {
	if (panel_svg::loadPointFromSvgMm(panelPath, id, out)) return true;
	*out = fallback;
	return false;
}

mandelwake::VisualSnapshot makeHeroSnapshot() {
	mandelwake::Engine engine(UINT64_C(0x4D414E44454C574B));
	mandelwake::StepInputs inputs;
	inputs.cXQ28 = static_cast<mandelwake::OrbitQ28>(-3 * mandelwake::kScaleQ28 / 4);
	inputs.cYQ28 = 0;
	inputs.mutationDepthQ28 = static_cast<mandelwake::OrbitQ28>(mandelwake::kScaleQ28 / 4);
	inputs.densityQ16 = 32768;
	inputs.iterations = 4;
	for (int i = 0; i < 192; ++i) engine.step(0, inputs);

	mandelwake::VisualSnapshot snapshot;
	const mandelwake::ChannelState& state = engine.channel(0);
	snapshot.pointCount = state.historyCount;
	for (int i = 0; i < state.historyCount; ++i) {
		snapshot.points[static_cast<std::size_t>(i)] = engine.historyPointOldestFirst(0, i);
	}
	snapshot.current = mandelwake::HistoryPoint(state.xQ28, state.yQ28);
	snapshot.lastPreEscape = state.lastPreEscape;
	snapshot.lastReentry = state.lastReentry;
	snapshot.escapeSerial = state.escapeSerial;
	snapshot.seedLocked = true;
	return snapshot;
}

struct MandelwakePanelLabels final : TransparentWidget {
	void label(const DrawArgs& args, const char* text, float xMm, float yMm,
		float sizeMm, NVGcolor color) {
		if (!APP || !APP->window || !APP->window->uiFont) return;
		const Vec position = mm2px(Vec(xMm, yMm));
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgFontSize(args.vg, mm2px(sizeMm));
		nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		nvgFillColor(args.vg, color);
		nvgText(args.vg, position.x, position.y, text, nullptr);
	}

	void draw(const DrawArgs& args) override {
		const NVGcolor control = nvgRGB(211, 211, 232);
		const NVGcolor input = nvgRGB(139, 221, 255);
		const NVGcolor output = nvgRGB(255, 221, 144);
		label(args, "MANDELWAKE", 45.72f, 5.3f, 2.15f, nvgRGB(244, 244, 255));
		const char* row1[] = {"MAP", "CENTER X", "CENTER Y", "ZOOM", "ITER"};
		const float row1x[] = {8.f, 24.f, 42.f, 60.f, 78.f};
		for (int i = 0; i < 5; ++i) label(args, row1[i], row1x[i], 44.f, 1.1f, control);
		const char* row2[] = {"MUTATION", "SMOOTH", "RATE", "DENSITY"};
		const char* row3[] = {"X AMT", "Y AMT", "ZOOM AMT", "MUTATE AMT"};
		const float rowX[] = {17.f, 36.f, 55.f, 74.f};
		for (int i = 0; i < 4; ++i) {
			label(args, row2[i], rowX[i], 56.3f, 0.95f, control);
			label(args, row3[i], rowX[i], 69.4f, 0.9f, control);
		}
		label(args, "RESEED", 39.f, 81.2f, 0.95f, control);
		label(args, "SEED LOCK", 52.5f, 81.2f, 0.95f, control);
		const char* inputs[] = {"CLOCK", "RESET", "X CV", "Y CV", "ZOOM", "MUTATE", "SMOOTH", "RATE"};
		const char* outputs[] = {"X", "Y", "RADIUS", "PHASE", "GATE", "ESCAPE", "STEP"};
		for (int i = 0; i < 8; ++i) label(args, inputs[i], i & 1 ? 20.f : 8.f, 91.5f + 9.3f * (i / 2), 0.82f, input);
		for (int i = 0; i < 7; ++i) label(args, outputs[i], i & 1 ? 83.4f : 71.4f, 91.5f + 9.3f * (i / 2), 0.82f, output);
		label(args, "LEVIATHAN", 45.72f, 126.2f, 0.78f, nvgRGB(116, 110, 157));
	}
};

struct MandelwakeDisplay final : Widget {
	Mandelwake* module = nullptr;
	widget::FramebufferWidget* framebuffer = nullptr;
	mandelwake::VisualSnapshot snapshot = makeHeroSnapshot();
	std::uint32_t observedEscapeSerial = 0;
	int observedQuality = Mandelwake::DISPLAY_NORMAL;
	double ruptureStartedAt = -1.0;

	Vec mapPoint(const mandelwake::HistoryPoint& point, double centerX, double centerY,
		double scale, float pad) const {
		const double x = static_cast<double>(point.xQ28) / static_cast<double>(mandelwake::kScaleQ28);
		const double y = static_cast<double>(point.yQ28) / static_cast<double>(mandelwake::kScaleQ28);
		const Vec mapped(
			0.5f * box.size.x + static_cast<float>((x - centerX) * scale),
			0.5f * box.size.y - static_cast<float>((y - centerY) * scale));
		return Vec(
			clamp(mapped.x, pad, box.size.x - pad),
			clamp(mapped.y, pad, box.size.y - pad));
	}

	void step() override {
		Widget::step();
		if (module) {
			const int quality = clamp(module->displayQuality.load(std::memory_order_relaxed), 0, 3);
			if (quality != observedQuality) {
				observedQuality = quality;
				if (framebuffer) framebuffer->dirty = true;
			}
			mandelwake::VisualSnapshot latest;
			if (module->consumeLatestVisualSnapshot(&latest)
				&& quality != Mandelwake::DISPLAY_FROZEN) {
				snapshot = latest;
				if (snapshot.escapeSerial != observedEscapeSerial) {
					observedEscapeSerial = snapshot.escapeSerial;
					ruptureStartedAt = system::getTime();
				}
				if (framebuffer) framebuffer->dirty = true;
			}
		}
		if (observedQuality != Mandelwake::DISPLAY_FROZEN
			&& ruptureStartedAt >= 0.0 && system::getTime() - ruptureStartedAt < 0.24) {
			if (framebuffer) framebuffer->dirty = true;
		}
	}

	void drawContours(const DrawArgs& args) {
		const NVGcolor colors[] = {
			nvgRGBA(73, 51, 145, 35), nvgRGBA(64, 63, 156, 32),
			nvgRGBA(41, 112, 154, 29), nvgRGBA(27, 163, 175, 24)
		};
		for (int line = 0; line < 8; ++line) {
			const float base = box.size.y * (0.16f + 0.105f * line);
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, -6.f, base);
			for (int segment = 0; segment < 6; ++segment) {
				const float x0 = box.size.x * segment / 6.f;
				const float x1 = box.size.x * (segment + 1) / 6.f;
				const int map = clamp(static_cast<int>(snapshot.map), 0, 2);
				const float sign = ((line + segment + map) & 1) ? -1.f : 1.f;
				const float depth = 3.f + line * 0.35f + map * (segment % 3);
				nvgQuadTo(args.vg, 0.5f * (x0 + x1), base + sign * depth, x1, base);
			}
			nvgStrokeColor(args.vg, colors[line & 3]);
			nvgStrokeWidth(args.vg, 0.7f);
			nvgStroke(args.vg);
		}
	}

	void drawTrident(const DrawArgs& args, const Vec& tip, const Vec& previous) {
		Vec direction = tip.minus(previous);
		float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
		if (length < 0.001f) direction = Vec(0.f, -1.f);
		else direction = direction.div(length);
		const Vec normal(-direction.y, direction.x);
		const Vec tail = tip.minus(direction.mult(12.f));
		const Vec crown = tip.minus(direction.mult(4.2f));
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, tail.x, tail.y);
		nvgLineTo(args.vg, tip.x, tip.y);
		nvgMoveTo(args.vg, crown.x, crown.y);
		nvgLineTo(args.vg, crown.x + normal.x * 5.f + direction.x * 2.f,
			crown.y + normal.y * 5.f + direction.y * 2.f);
		nvgMoveTo(args.vg, crown.x, crown.y);
		nvgLineTo(args.vg, crown.x - normal.x * 5.f + direction.x * 2.f,
			crown.y - normal.y * 5.f + direction.y * 2.f);
		nvgStrokeColor(args.vg, nvgRGBA(255, 205, 92, 245));
		nvgStrokeWidth(args.vg, 1.8f);
		nvgLineCap(args.vg, NVG_ROUND);
		nvgLineJoin(args.vg, NVG_ROUND);
		nvgStroke(args.vg);
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, tip.x, tip.y, 2.3f);
		nvgFillColor(args.vg, nvgRGBA(255, 225, 137, 240));
		nvgFill(args.vg);
	}

	void draw(const DrawArgs& args) override {
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, 0.f, 0.f, box.size.x, box.size.y, 4.f);
		nvgFillColor(args.vg, nvgRGBA(1, 4, 12, 255));
		nvgFill(args.vg);
		nvgSave(args.vg);
		nvgScissor(args.vg, 0.f, 0.f, box.size.x, box.size.y);
		drawContours(args);

		const int count = std::min<int>(snapshot.pointCount, mandelwake::kHistoryCapacity);
		double minX = -1.0, maxX = 1.0, minY = -1.0, maxY = 1.0;
		if (count > 0) {
			minX = maxX = static_cast<double>(snapshot.points[0].xQ28) / mandelwake::kScaleQ28;
			minY = maxY = static_cast<double>(snapshot.points[0].yQ28) / mandelwake::kScaleQ28;
			for (int i = 1; i < count; ++i) {
				const double x = static_cast<double>(snapshot.points[static_cast<std::size_t>(i)].xQ28) / mandelwake::kScaleQ28;
				const double y = static_cast<double>(snapshot.points[static_cast<std::size_t>(i)].yQ28) / mandelwake::kScaleQ28;
				minX = std::min(minX, x); maxX = std::max(maxX, x);
				minY = std::min(minY, y); maxY = std::max(maxY, y);
			}
		}
		const double centerX = 0.5 * (minX + maxX);
		const double centerY = 0.5 * (minY + maxY);
		const double spanX = std::max(0.38, maxX - minX);
		const double spanY = std::max(0.38, maxY - minY);
		const float pad = 9.f;
		const double scale = std::min((box.size.x - 2.f * pad) / spanX, (box.size.y - 2.f * pad) / spanY);
		const int stride = module && module->displayQuality.load(std::memory_order_relaxed) == Mandelwake::DISPLAY_LOW ? 2 : 1;

		if (count > 1) {
			for (int band = 0; band < 16; ++band) {
				const int first = band * (count - 1) / 16;
				const int last = std::max(first + 1, (band + 1) * (count - 1) / 16);
				const float age = static_cast<float>(band) / 15.f;
				nvgBeginPath(args.vg);
				Vec p = mapPoint(snapshot.points[static_cast<std::size_t>(first)], centerX, centerY, scale, pad);
				nvgMoveTo(args.vg, p.x, p.y);
				for (int i = first + stride; i <= last; i += stride) {
					const int current = std::min(i, last);
					p = mapPoint(snapshot.points[static_cast<std::size_t>(current)], centerX, centerY, scale, pad);
					const mandelwake::HistoryPoint& a = snapshot.points[static_cast<std::size_t>(std::max(first, current - stride))];
					const mandelwake::HistoryPoint& b = snapshot.points[static_cast<std::size_t>(current)];
					const bool rupture = a.xQ28 == snapshot.lastPreEscape.xQ28
						&& a.yQ28 == snapshot.lastPreEscape.yQ28
						&& b.xQ28 == snapshot.lastReentry.xQ28
						&& b.yQ28 == snapshot.lastReentry.yQ28;
					if (rupture) nvgMoveTo(args.vg, p.x, p.y);
					else nvgLineTo(args.vg, p.x, p.y);
				}
				const int r = static_cast<int>(112.f - 73.f * age);
				const int g = static_cast<int>(72.f + 132.f * age);
				const int b = static_cast<int>(214.f + 30.f * age);
				nvgStrokeColor(args.vg, nvgRGBA(r, g, b, static_cast<int>(55.f + 175.f * age)));
				nvgStrokeWidth(args.vg, 0.8f + 1.25f * age);
				nvgLineCap(args.vg, NVG_ROUND);
				nvgLineJoin(args.vg, NVG_ROUND);
				nvgStroke(args.vg);
			}
			const Vec tip = mapPoint(snapshot.points[static_cast<std::size_t>(count - 1)], centerX, centerY, scale, pad);
			int priorIndex = count - 2;
			while (priorIndex > 0
				&& snapshot.points[static_cast<std::size_t>(priorIndex)].xQ28 == snapshot.points[static_cast<std::size_t>(count - 1)].xQ28
				&& snapshot.points[static_cast<std::size_t>(priorIndex)].yQ28 == snapshot.points[static_cast<std::size_t>(count - 1)].yQ28) {
				--priorIndex;
			}
			const Vec prior = mapPoint(snapshot.points[static_cast<std::size_t>(priorIndex)], centerX, centerY, scale, pad);
			drawTrident(args, tip, prior);
		}

		const double ruptureAge = !module ? 0.07 : (ruptureStartedAt < 0.0 ? 1.0 : system::getTime() - ruptureStartedAt);
		if (ruptureAge < 0.24) {
			const float envelope = static_cast<float>(1.0 - ruptureAge / 0.24);
			const Vec before = mapPoint(snapshot.lastPreEscape, centerX, centerY, scale, pad);
			const Vec after = mapPoint(snapshot.lastReentry, centerX, centerY, scale, pad);
			nvgBeginPath(args.vg); nvgMoveTo(args.vg, before.x, before.y); nvgLineTo(args.vg, after.x, after.y);
			nvgStrokeColor(args.vg, nvgRGBA(255, 76, 139, static_cast<int>(220.f * envelope)));
			nvgStrokeWidth(args.vg, 1.6f); nvgStroke(args.vg);
			nvgBeginPath(args.vg); nvgCircle(args.vg, before.x, before.y, 4.f + 12.f * (1.f - envelope));
			nvgStrokeColor(args.vg, nvgRGBA(255, 183, 88, static_cast<int>(235.f * envelope)));
			nvgStrokeWidth(args.vg, 1.4f); nvgStroke(args.vg);
		}

		if (APP && APP->window && APP->window->uiFont) {
			static const char* mapNames[] = {"MANDELBROT", "JULIA", "BURNING SHIP"};
			const int map = clamp(static_cast<int>(snapshot.map), 0, 2);
			const char* suffix = snapshot.compatibilityWarning ? "  VERSION WARNING"
				: (observedQuality == Mandelwake::DISPLAY_FROZEN ? "  FROZEN" : (snapshot.seedLocked ? "  LOCKED" : ""));
			char status[96];
			std::snprintf(status, sizeof(status), "CH %d  %s%s", snapshot.selectedChannel + 1, mapNames[map], suffix);
			nvgFontFaceId(args.vg, APP->window->uiFont->handle);
			nvgFontSize(args.vg, 8.f); nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
			nvgFillColor(args.vg, snapshot.compatibilityWarning ? nvgRGB(255, 111, 111) : nvgRGBA(184, 221, 255, 205));
			nvgText(args.vg, 7.f, 6.f, status, nullptr);
		}
		nvgRestore(args.vg);
	}
};

} // namespace

MandelwakeWidget::MandelwakeWidget(Mandelwake* module) {
	setModule(module);
	PreviewBuildLogTimer previewBuildTimer("Mandelwake", module);
	visual_assets::SplitPanelRenderer splitPanel(this, "res/mandelwake.panel.svg");
	const std::string& panelPath = splitPanel.panelPath();
	splitPanel.addPerfectWaveBranding();
	visual_assets::addFractalGlassOverlay(this, panelPath, splitPanel.panelSurfaceEffectWidget());
	previewBuildTimer.markPanelDone();

	math::Rect displayMm(Vec(11.72f, 8.5f), Vec(68.f, 34.f));
	panel_svg::loadRectFromSvgMm(panelPath, "MANDELWAKE_DISPLAY", &displayMm);
	widget::FramebufferWidget* framebuffer = new widget::FramebufferWidget();
	framebuffer->box.pos = mm2px(displayMm.pos);
	framebuffer->box.size = mm2px(displayMm.size);
	framebuffer->dirtyOnSubpixelChange = false;
	MandelwakeDisplay* display = new MandelwakeDisplay();
	display->module = module;
	display->framebuffer = framebuffer;
	display->box.size = framebuffer->box.size;
	framebuffer->addChild(display);
	addChild(framebuffer);
	addChild(visual_assets::createPreviewFrameEnhancementWidget(displayMm, visual_assets::PreviewFrameTint::Purple));
	widget::FramebufferWidget* labelFramebuffer = new widget::FramebufferWidget();
	labelFramebuffer->box.size = box.size;
	labelFramebuffer->dirtyOnSubpixelChange = false;
	MandelwakePanelLabels* labels = new MandelwakePanelLabels();
	labels->box.size = box.size;
	labelFramebuffer->addChild(labels);
	addChild(labelFramebuffer);

	auto anchorMm = [&](const char* id, const Vec& fallback) {
		Vec result;
		loadAnchorPointMm(panelPath, id, &result, fallback);
		return result;
	};
	auto addMainKnob = [&](int paramId, const char* id, const Vec& fallback, bool halo) {
		const Vec pos = mm2px(anchorMm(id, fallback));
		if (halo) addParam(createParamCentered<LeviathanHaloKnob2>(pos, module, paramId));
		else addParam(createParamCentered<Eclipse2Knob>(pos, module, paramId));
	};

	addParam(createParamCentered<CKSSThree>(mm2px(anchorMm("MAP_PARAM", Vec(8.f, 49.f))), module, Mandelwake::MAP_PARAM));
	addMainKnob(Mandelwake::CENTER_X_PARAM, "CENTER_X_PARAM", Vec(24.f, 49.f), false);
	addMainKnob(Mandelwake::CENTER_Y_PARAM, "CENTER_Y_PARAM", Vec(42.f, 49.f), false);
	addMainKnob(Mandelwake::ZOOM_PARAM, "ZOOM_PARAM", Vec(60.f, 49.f), true);
	addMainKnob(Mandelwake::ITERATIONS_PARAM, "ITERATIONS_PARAM", Vec(78.f, 49.f), false);
	addMainKnob(Mandelwake::MUTATION_PARAM, "MUTATION_PARAM", Vec(17.f, 61.5f), false);
	addMainKnob(Mandelwake::SMOOTH_PARAM, "SMOOTH_PARAM", Vec(36.f, 61.5f), false);
	addMainKnob(Mandelwake::RATE_PARAM, "RATE_PARAM", Vec(55.f, 61.5f), false);
	addMainKnob(Mandelwake::DENSITY_PARAM, "DENSITY_PARAM", Vec(74.f, 61.5f), false);

	const int amountParams[] = {Mandelwake::X_AMOUNT_PARAM, Mandelwake::Y_AMOUNT_PARAM, Mandelwake::ZOOM_AMOUNT_PARAM, Mandelwake::MUTATE_AMOUNT_PARAM};
	const char* amountIds[] = {"X_AMOUNT_PARAM", "Y_AMOUNT_PARAM", "ZOOM_AMOUNT_PARAM", "MUTATE_AMOUNT_PARAM"};
	const float amountX[] = {17.f, 36.f, 55.f, 74.f};
	for (int i = 0; i < 4; ++i) {
		auto* knob = createParamCentered<BipolarDarkTinyClockworkGearKnob>(
			mm2px(anchorMm(amountIds[i], Vec(amountX[i], 73.5f))), module, amountParams[i]);
		addParam(knob);
	}
	addParam(createParamCentered<SmallGoldButton>(mm2px(anchorMm("RESEED_PARAM", Vec(39.f, 84.5f))), module, Mandelwake::RESEED_PARAM));
	auto* seedLock = createLightParamCentered<SmallGoldApertureButton>(
		mm2px(anchorMm("SEED_LOCK_PARAM", Vec(52.5f, 84.5f))), module,
		Mandelwake::SEED_LOCK_PARAM, Mandelwake::SEED_LOCK_LIGHT);
	addParam(seedLock);

	const int inputIds[] = {Mandelwake::CLOCK_INPUT, Mandelwake::RESET_INPUT, Mandelwake::X_INPUT, Mandelwake::Y_INPUT,
		Mandelwake::ZOOM_INPUT, Mandelwake::MUTATE_INPUT, Mandelwake::SMOOTH_INPUT, Mandelwake::RATE_INPUT};
	const char* inputAnchors[] = {"CLOCK_INPUT", "RESET_INPUT", "X_INPUT", "Y_INPUT", "ZOOM_INPUT", "MUTATE_INPUT", "SMOOTH_INPUT", "RATE_INPUT"};
	const int outputIds[] = {Mandelwake::X_OUTPUT, Mandelwake::Y_OUTPUT, Mandelwake::RADIUS_OUTPUT, Mandelwake::PHASE_OUTPUT,
		Mandelwake::GATE_OUTPUT, Mandelwake::ESCAPE_OUTPUT, Mandelwake::STEP_OUTPUT};
	const char* outputAnchors[] = {"X_OUTPUT", "Y_OUTPUT", "RADIUS_OUTPUT", "PHASE_OUTPUT", "GATE_OUTPUT", "ESCAPE_OUTPUT", "STEP_OUTPUT"};
	for (int i = 0; i < 8; ++i) {
		const Vec fallback(i & 1 ? 20.f : 8.f, 96.f + 9.3f * (i / 2));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(anchorMm(inputAnchors[i], fallback)), module, inputIds[i]));
	}
	for (int i = 0; i < 7; ++i) {
		const Vec fallback(i & 1 ? 83.4f : 71.4f, 96.f + 9.3f * (i / 2));
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(anchorMm(outputAnchors[i], fallback)), module, outputIds[i]));
	}
	previewBuildTimer.setAtlasStatus(panel_svg::getAtlasStatusLabelForSvg(panelPath));
	previewBuildTimer.markAnchorsDone();
}

void MandelwakeWidget::appendContextMenu(Menu* menu) {
	ModuleWidget::appendContextMenu(menu);
	auto* m = dynamic_cast<Mandelwake*>(module);
	if (!m) return;
	menu->addChild(new MenuSeparator());
	menu->addChild(createCheckMenuItem("Free-run without clock", "",
		[m]() { return m->freeRunWhenUnclocked.load(std::memory_order_relaxed); },
		[m]() { m->freeRunWhenUnclocked.store(!m->freeRunWhenUnclocked.load(std::memory_order_relaxed), std::memory_order_relaxed); }));
	menu->addChild(createCheckMenuItem("Restart internal phase on reset", "",
		[m]() { return m->restartInternalPhaseOnReset.load(std::memory_order_relaxed); },
		[m]() { m->restartInternalPhaseOnReset.store(!m->restartInternalPhaseOnReset.load(std::memory_order_relaxed), std::memory_order_relaxed); }));
	menu->addChild(createSubmenuItem("Phase output", m->phaseUnipolar.load() ? "0 to 10 V" : "-5 to 5 V", [m](Menu* child) {
		child->addChild(createCheckMenuItem("-5 to 5 V", "", [m]() { return !m->phaseUnipolar.load(); }, [m]() { m->phaseUnipolar.store(false); }));
		child->addChild(createCheckMenuItem("0 to 10 V", "", [m]() { return m->phaseUnipolar.load(); }, [m]() { m->phaseUnipolar.store(true); }));
	}));
	static const char* pulseLabels[] = {"0.1 ms", "1 ms", "5 ms", "10 ms"};
	menu->addChild(createSubmenuItem("Pulse width", pulseLabels[clamp(m->pulseWidthIndex.load(), 0, 3)], [m](Menu* child) {
		for (int i = 0; i < 4; ++i) child->addChild(createCheckMenuItem(pulseLabels[i], "",
			[m, i]() { return m->pulseWidthIndex.load() == i; }, [m, i]() { m->pulseWidthIndex.store(i); }));
	}));
	static const char* qualityLabels[] = {"Low", "Normal", "High", "Frozen"};
	menu->addChild(createSubmenuItem("Abyssal Wake quality", qualityLabels[clamp(m->displayQuality.load(), 0, 3)], [m](Menu* child) {
		for (int i = 0; i < 4; ++i) child->addChild(createCheckMenuItem(qualityLabels[i], "",
			[m, i]() { return m->displayQuality.load() == i; }, [m, i]() { m->displayQuality.store(i); }));
	}));
	menu->addChild(createSubmenuItem("Display channel", string::f("%d", clamp(m->selectedDisplayChannel.load(), 0, 15) + 1), [m](Menu* child) {
		for (int i = 0; i < 16; ++i) child->addChild(createCheckMenuItem(string::f("Channel %d", i + 1), "",
			[m, i]() { return m->selectedDisplayChannel.load() == i; }, [m, i]() { m->selectedDisplayChannel.store(i); }));
	}));
	menu->addChild(new MenuSeparator());
	menu->addChild(createMenuItem("Derive new seed", "", [m]() {
		const std::uint64_t entropy = (std::uint64_t {random::u32()} << 32) | std::uint64_t {random::u32()};
		Mandelwake::UiCommand command;
		command.seed = mandelwake::mix64(m->baseSeed() ^ mandelwake::kDomainReseed ^ entropy);
		m->enqueueUiCommand(command);
	}));
	menu->addChild(createMenuItem("Copy seed", "", [m]() {
		if (!APP || !APP->window || !APP->window->win) return;
		char text[32];
		std::snprintf(text, sizeof(text), "0x%016llx", static_cast<unsigned long long>(m->baseSeed()));
		glfwSetClipboardString(APP->window->win, text);
	}));
	menu->addChild(createMenuItem("Paste seed", "", [m]() {
		if (!APP || !APP->window || !APP->window->win) return;
		const char* text = glfwGetClipboardString(APP->window->win);
		if (!text || !*text) return;
		errno = 0;
		char* end = nullptr;
		const unsigned long long seed = std::strtoull(text, &end, 0);
		if (errno != 0 || end == text || *end != '\0') return;
		Mandelwake::UiCommand command;
		command.seed = static_cast<std::uint64_t>(seed);
		m->enqueueUiCommand(command);
	}));
}
