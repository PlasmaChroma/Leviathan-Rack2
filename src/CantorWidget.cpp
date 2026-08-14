#include "Cantor.hpp"

#include "PanelSvgUtils.hpp"
#include "visual/VisualAssets.hpp"

#include <cmath>
#include <cstdio>

namespace {

struct CantorMindDisplay final : TransparentWidget {
	Cantor* module = nullptr;

	void draw(const DrawArgs& args) override {
		const float width = box.size.x;
		const float height = box.size.y;
		const NVGpaint background = nvgLinearGradient(
			args.vg, 0.f, 0.f, 0.f, height,
			nvgRGB(5, 10, 17), nvgRGB(14, 7, 24));
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, 0.f, 0.f, width, height, 4.f);
		nvgFillPaint(args.vg, background);
		nvgFill(args.vg);
		nvgStrokeColor(args.vg, nvgRGBA(79, 217, 226, 150));
		nvgStrokeWidth(args.vg, 1.f);
		nvgStroke(args.vg);

		const float requested = module
			? module->visualRequestedPitch.load(std::memory_order_relaxed) : 0.401f;
		const float selected = module
			? module->visualSelectedPitch.load(std::memory_order_relaxed) : 0.386f;
		const float distance = module
			? module->visualDistanceCents.load(std::memory_order_relaxed) : 18.f;
		const float harmonic = module
			? module->visualHarmonicCost.load(std::memory_order_relaxed) : 0.18f;
		const int voices = module
			? module->visualActiveVoices.load(std::memory_order_relaxed) : 3;

		std::shared_ptr<Font> font = APP && APP->window
			? APP->window->loadFont(asset::system(
				"res/fonts/ShareTechMono-Regular.ttf")) : nullptr;
		if (!font) return;
		nvgFontFaceId(args.vg, font->handle);
		nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
		nvgFillColor(args.vg, nvgRGB(109, 239, 225));
		nvgFontSize(args.vg, 10.f);
		nvgText(args.vg, 7.f, 11.f, "CULTURE / MIND", nullptr);

		char line[80];
		nvgFontSize(args.vg, 9.f);
		nvgFillColor(args.vg, nvgRGB(214, 224, 235));
		std::snprintf(line, sizeof(line), "REQ %+7.3f V", requested);
		nvgText(args.vg, 7.f, 25.f, line, nullptr);
		std::snprintf(line, sizeof(line), "OUT %+7.3f V", selected);
		nvgText(args.vg, 7.f, 37.f, line, nullptr);
		nvgFillColor(args.vg, nvgRGB(170, 142, 255));
		std::snprintf(
			line, sizeof(line), "D %5.1fc  H %.2f  V %d",
			distance, harmonic, voices);
		nvgText(args.vg, 7.f, 49.f, line, nullptr);
	}
};

} // namespace

struct CantorWidget final : ModuleWidget {
	explicit CantorWidget(Cantor* module) {
		setModule(module);
		PreviewBuildLogTimer previewTimer("Cantor", module);
		visual_assets::SplitPanelRenderer splitPanel(
			this, "res/Cantor.panel.svg");
		const std::string& panelPath = splitPanel.panelPath();
		splitPanel.addLabels("res/Cantor.labels.svg");
		previewTimer.markPanelDone();

		auto point = [&](const char* id, const Vec& fallbackMm) {
			Vec value;
			return panel_svg::loadPointFromSvgMm(panelPath, id, &value)
				? value : fallbackMm;
		};
		auto rect = [&](const char* id, const math::Rect& fallbackMm) {
			math::Rect value;
			return panel_svg::loadRectFromSvgMm(panelPath, id, &value)
				? value : fallbackMm;
		};

		const math::Rect displayMm = rect(
			"MIND_DISPLAY", math::Rect(Vec(3.f, 14.f), Vec(44.8f, 34.f)));
		auto* display = new CantorMindDisplay();
		display->module = module;
		display->box.pos = mm2px(displayMm.pos);
		display->box.size = mm2px(displayMm.size);
		addChild(display);

		addParam(createParamCentered<Eclipse2Knob>(
			mm2px(point("INTENT_PARAM", Vec(14.f, 67.f))),
			module, Cantor::INTENT_PARAM));
		addParam(createParamCentered<Eclipse2Knob>(
			mm2px(point("COHERENCE_PARAM", Vec(36.8f, 67.f))),
			module, Cantor::COHERENCE_PARAM));
		addParam(createParamCentered<Eclipse2Knob>(
			mm2px(point("INTERPRET_PARAM", Vec(14.f, 91.f))),
			module, Cantor::INTERPRET_PARAM));
		addParam(createParamCentered<Eclipse2Knob>(
			mm2px(point("FIELD_PARAM", Vec(36.8f, 91.f))),
			module, Cantor::FIELD_PARAM));

		addInput(createInputCentered<Magitek2InputJack>(
			mm2px(point("PITCH_INPUT", Vec(9.f, 116.5f))),
			module, Cantor::PITCH_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(
			mm2px(point("GATE_INPUT", Vec(25.4f, 116.5f))),
			module, Cantor::GATE_INPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(
			mm2px(point("PITCH_OUTPUT", Vec(41.8f, 116.5f))),
			module, Cantor::PITCH_OUTPUT));

		previewTimer.setAtlasStatus(
			panel_svg::getAtlasStatusLabelForSvg(panelPath));
		previewTimer.markAnchorsDone();
	}
};

Model* modelCantor = createModel<Cantor, CantorWidget>("Cantor");
