#include "Moirai.hpp"
#include "MoiraiCurves.hpp"
#include "PanelSvgUtils.hpp"
#include "visual/FractalGlassOverlay.hpp"
#include "visual/VisualAssets.hpp"

using namespace rack;

namespace {

struct MoiraiDisplay final : TransparentWidget {
	Moirai* module = nullptr;

	void draw(const DrawArgs& args) override {
		const float w = box.size.x;
		const float h = box.size.y;
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, 0.f, 0.f, w, h, 4.f);
		NVGpaint background = nvgLinearGradient(args.vg, 0.f, 0.f, 0.f, h,
			nvgRGBA(7, 16, 27, 255), nvgRGBA(1, 4, 9, 255));
		nvgFillPaint(args.vg, background);
		nvgFill(args.vg);

		int lane = 0;
		int channel = 0;
		int channels = 1;
		float bpm = 120.f;
		float values[2] {0.62f, 0.35f};
		uint16_t masks[2] {1u, 1u};
		bool external = false;
		if (module) {
			lane = module->selectedLane.load(std::memory_order_relaxed);
			channel = module->selectedChannel.load(std::memory_order_relaxed);
			channels = module->telemetryChannels.load(std::memory_order_relaxed);
			bpm = module->telemetryBpm.load(std::memory_order_relaxed);
			external = module->telemetryExternalClock.load(std::memory_order_relaxed);
			for (int index = 0; index < 2; ++index) {
				values[index] = module->telemetrySelectedValue[index].load(std::memory_order_relaxed);
				masks[index] = module->telemetryActiveMask[index].load(std::memory_order_relaxed);
			}
		}
		if (APP && APP->window && APP->window->uiFont) {
			nvgFontFaceId(args.vg, APP->window->uiFont->handle);
			nvgFillColor(args.vg, nvgRGBA(232, 242, 248, 255));
			nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
			nvgFontSize(args.vg, 9.f);
			nvgText(args.vg, 7.f, 6.f, "MOIRAI", nullptr);
			nvgFillColor(args.vg, nvgRGBA(83, 229, 236, 255));
			nvgTextAlign(args.vg, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP);
			nvgFontSize(args.vg, 7.f);
			const std::string status = string::f("%s  %3.0f BPM", external ? "EXT" : "INT", bpm);
			nvgText(args.vg, w - 7.f, 7.f, status.c_str(), nullptr);
			nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
			nvgFontSize(args.vg, 7.5f);
			const std::string selection = string::f("LANE %c  ·  CH %02d/%02d", lane ? 'B' : 'A', channel + 1, channels);
			nvgText(args.vg, 7.f, 20.f, selection.c_str(), nullptr);
			nvgFillColor(args.vg, nvgRGBA(157, 137, 245, 255));
			nvgFontSize(args.vg, 7.f);
			nvgText(args.vg, 7.f, 32.f, "factory_adsr", nullptr);
		}

		const float barX = 7.f;
		const float barW = w - 14.f;
		for (int index = 0; index < 2; ++index) {
			const float y = 46.f + index * 13.f;
			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, barX, y, barW, 6.f, 3.f);
			nvgFillColor(args.vg, nvgRGBA(23, 37, 52, 245));
			nvgFill(args.vg);
			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, barX, y, barW * moirai::clamp01(values[index]), 6.f, 3.f);
			nvgFillColor(args.vg, index == lane ? nvgRGBA(90, 234, 238, 255) : nvgRGBA(138, 111, 225, 210));
			nvgFill(args.vg);
			if (APP && APP->window && APP->window->uiFont) {
				nvgFontFaceId(args.vg, APP->window->uiFont->handle);
				nvgFontSize(args.vg, 6.5f);
				nvgTextAlign(args.vg, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
				nvgFillColor(args.vg, nvgRGBA(225, 236, 243, 235));
				const std::string label = string::f("%c  %02X", index ? 'B' : 'A', masks[index]);
				nvgText(args.vg, w - 8.f, y + 3.f, label.c_str(), nullptr);
			}
		}
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, 0.5f, 0.5f, w - 1.f, h - 1.f, 4.f);
		nvgStrokeWidth(args.vg, 1.f);
		nvgStrokeColor(args.vg, nvgRGBA(68, 216, 224, 150));
		nvgStroke(args.vg);
	}
};

} // namespace

struct MoiraiWidget final : ModuleWidget {
	explicit MoiraiWidget(Moirai* module) {
		setModule(module);
		visual_assets::SplitPanelRenderer splitPanel(this, "res/Moirai.panel.svg");
		const std::string& panelPath = splitPanel.panelPath();
		splitPanel.addLabels("res/Moirai.labels.svg");
		splitPanel.addCompactLeviathanLogoBranding();
		visual_assets::addFractalGlassOverlay(this, panelPath, splitPanel.panelSurfaceEffectWidget());
		auto anchor = [&](const char* id, Vec fallbackMm) {
			Vec point = fallbackMm;
			panel_svg::loadPointFromSvgMm(panelPath, id, &point);
			return mm2px(point);
		};
		math::Rect displayMm(Vec(3.f, 11.f), Vec(54.96f, 20.f));
		panel_svg::loadRectFromSvgMm(panelPath, "MOIRAI_DISPLAY", &displayMm);
		auto* display = new MoiraiDisplay;
		display->module = module;
		display->box.pos = mm2px(displayMm.pos);
		display->box.size = mm2px(displayMm.size);
		addChild(display);

		addParam(createParamCentered<SmallGoldButton>(anchor("LANE_PARAM", Vec(12.f, 53.f)), module, Moirai::LANE_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(anchor("CHANNEL_PARAM", Vec(30.48f, 53.f)), module, Moirai::CHANNEL_PARAM));
		addParam(createParamCentered<SmallGoldButton>(anchor("MANUAL_TRIGGER_PARAM", Vec(49.f, 53.f)), module, Moirai::MANUAL_TRIGGER_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(anchor("TIME_PARAM", Vec(12.f, 72.f)), module, Moirai::TIME_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(anchor("CURVE_PARAM", Vec(30.48f, 72.f)), module, Moirai::CURVE_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(anchor("LEVEL_PARAM", Vec(49.f, 72.f)), module, Moirai::LEVEL_PARAM));
		addChild(createLightCentered<SmallLight<GreenLight>>(anchor("LANE_A_LIGHT", Vec(8.f, 59.f)), module, Moirai::LANE_A_LIGHT));
		addChild(createLightCentered<SmallLight<BlueLight>>(anchor("LANE_B_LIGHT", Vec(16.f, 59.f)), module, Moirai::LANE_B_LIGHT));

		const float inputX[7] = {5.f, 13.5f, 22.f, 30.5f, 39.f, 47.5f, 56.f};
		const char* inputIds[7] = {"GATE_INPUT", "VELOCITY_INPUT", "M1_INPUT", "M2_INPUT", "M3_INPUT", "CLOCK_INPUT", "RESET_INPUT"};
		for (int index = 0; index < 7; ++index)
			addInput(createInputCentered<Magitek2InputJack>(anchor(inputIds[index], Vec(inputX[index], 93.f)), module, index));
		const float outputX[4] = {8.f, 22.f, 39.f, 53.f};
		const char* outputIds[4] = {"A_OUTPUT", "EOC_A_OUTPUT", "B_OUTPUT", "EOC_B_OUTPUT"};
		for (int index = 0; index < 4; ++index)
			addOutput(createOutputCentered<Magitek2OutputJack>(anchor(outputIds[index], Vec(outputX[index], 116.f)), module, index));
	}
};

Model* modelMoirai = createModel<Moirai, MoiraiWidget>("Moirai");
