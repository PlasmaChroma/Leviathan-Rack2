#include "Vessel.hpp"
#include "PanelSvgUtils.hpp"
#include "visual/VisualAssets.hpp"
#include <cstdio>

namespace {
struct RotationButton : SmallGoldButton { RotationButton() { momentary = false; setColor(nvgRGB(85, 215, 213)); } };
struct BowlDisplay : TransparentWidget {
    Vessel* vessel = nullptr;
    void draw(const DrawArgs& args) override {
        const float energy = vessel ? vessel->visualEnergy.load(std::memory_order_relaxed) : .55f;
        const float barHeight = mm2px(3.f);
        nvgBeginPath(args.vg); nvgRoundedRect(args.vg, 0, 0, box.size.x, barHeight, 2);
        nvgFillColor(args.vg, nvgRGB(7, 11, 19)); nvgFill(args.vg);
        nvgStrokeColor(args.vg, nvgRGBA(176, 141, 216, 125)); nvgStrokeWidth(args.vg, .7f); nvgStroke(args.vg);
        if (energy > 0.f) {
            nvgBeginPath(args.vg); nvgRoundedRect(args.vg, 1, 1, (box.size.x-2)*energy, barHeight-2, 1);
            nvgFillPaint(args.vg, nvgLinearGradient(args.vg, 0, 0, box.size.x, 0,
                nvgRGB(163, 113, 245), nvgRGB(74, 222, 214))); nvgFill(args.vg);
        }
        if (!APP || !APP->window || !APP->window->uiFont) return;
        const float hz = vessel ? vessel->visualFrequency.load(std::memory_order_relaxed) : 261.625565f;
        const bool fault = vessel && vessel->visualFault.load(std::memory_order_relaxed);
        const float delta = vessel ? vessel->visualSeparation.load(std::memory_order_relaxed) : 0.f;
        char text[48]; std::snprintf(text, sizeof(text), "%.1f / %.1f Hz", hz-.5f*delta, hz+.5f*delta);
        nvgFontFaceId(args.vg, APP->window->uiFont->handle); nvgFontSize(args.vg, 9.f);
        nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
        nvgFillColor(args.vg, fault ? nvgRGB(255, 133, 99) : nvgRGB(181, 213, 220));
        nvgText(args.vg, 1, mm2px(5.6f), fault ? "RATE / CONTACT FAULT" : "BOWL ENERGY", nullptr);
        if (!fault) {
            nvgTextAlign(args.vg, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
            nvgText(args.vg, box.size.x-1, mm2px(5.6f), text, nullptr);
        }
    }
};
}

struct VesselWidget final : ModuleWidget {
    debug_terminal::BaselineWidgetMetrics timing;
    debug_terminal::UiCycleTimingAccumulator layerTiming;
    explicit VesselWidget(Vessel* module) {
        setModule(module);
        visual_assets::SplitPanelRenderer panel(this, "res/Vessel.panel.svg");
        panel.addThemedLabels("res/Vessel.labels.svg", "res/Vessel.theme-text-input.svg", "res/Vessel.theme-text-output.svg");
        visual_assets::addCompactLeviathanLogoBranding(this, panel.panelPath());
        auto point = [&](const char* id, float x, float y) {
            Vec p; return panel_svg::loadPointFromSvgMm(panel.panelPath(), id, &p) ? p : Vec(x, y);
        };
        auto* display = new BowlDisplay(); display->vessel = module;
        math::Rect r(Vec(6, 15), Vec(69.28f, 8));
        panel_svg::loadRectFromSvgMm(panel.panelPath(), "ENERGY_DISPLAY", &r);
        display->box.pos = mm2px(r.pos); display->box.size = mm2px(r.size); addChild(display);
        const char* knobs[] = {"PITCH_PARAM", "FINE_PARAM", "VELOCITY_PARAM", "SPEED_PARAM", "PRESSURE_PARAM",
            "DECAY_PARAM", "IMPERFECTION_PARAM", "WIDTH_PARAM", "LEVEL_PARAM", "MALLET_PARAM", "BINAURAL_PARAM"};
        const int ids[] = {Vessel::PITCH_PARAM, Vessel::FINE_PARAM, Vessel::VELOCITY_PARAM, Vessel::SPEED_PARAM,
            Vessel::PRESSURE_PARAM, Vessel::DECAY_PARAM, Vessel::IMPERFECTION_PARAM, Vessel::WIDTH_PARAM,
            Vessel::LEVEL_PARAM, Vessel::MALLET_PARAM, Vessel::BINAURAL_PARAM};
        for (int i = 0; i < 11; ++i) {
            const float x = i == 9 ? 43.f : i == 10 ? 15.f : i%3 == 0 ? 15.f : i%3 == 1 ? 40.64f : 66.28f;
            const float y = i < 9 ? 32.f+18.f*(i/3) : 84.f;
            addParam(createParamCentered<Eclipse2Knob>(mm2px(point(knobs[i], x, y)), module, ids[i]));
        }
        addParam(createParamCentered<PlasmaSwitch>(mm2px(point("BOWL_PARAM", 28, 84)), module, Vessel::BOWL_PARAM));
        addParam(createParamCentered<SmallGoldButton>(mm2px(point("STRIKE_PARAM", 61, 84)), module, Vessel::STRIKE_PARAM));
        addParam(createParamCentered<RotationButton>(mm2px(point("ROTATE_PARAM", 72, 84)), module, Vessel::ROTATE_PARAM));
        const char* inputs[] = {"VOCT_INPUT", "STRIKE_INPUT", "VELOCITY_INPUT", "ROTATE_INPUT", "SPEED_INPUT", "PRESSURE_INPUT"};
        for (int i = 0; i < 6; ++i) addInput(createInputCentered<Magitek2InputJack>(
            mm2px(point(inputs[i], 12.f+19.f*(i%4), i < 4 ? 98.f : 110.5f)), module, i));
        addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(point("LEFT_OUTPUT", 50, 110.5f)), module, Vessel::LEFT_OUTPUT));
        addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(point("RIGHT_OUTPUT", 69, 110.5f)), module, Vessel::RIGHT_OUTPUT));
    }
    void step() override {
        const bool enabled = isDragonKingDebugEnabled();
        layerTiming.beginCycle(enabled);
        const auto start = debug_terminal::debugTimerStart(enabled);
        ModuleWidget::step();
        if (enabled) timing.recordStep(debug_terminal::elapsedUsSince(start));
    }
    void drawLayer(const DrawArgs& args, int layer) override {
        debug_terminal::ScopedUiCycleTimer timer(&layerTiming);
        ModuleWidget::drawLayer(args, layer);
    }
    void draw(const DrawArgs& args) override {
        const bool enabled = isDragonKingDebugEnabled();
        const auto start = debug_terminal::debugTimerStart(enabled);
        ModuleWidget::draw(args);
        auto* m = static_cast<Vessel*>(module);
        if (enabled) {
            timing.recordDraw(debug_terminal::elapsedUsSince(start));
            if (m && debug_terminal::baselineSubmitDue("Vessel", m->debugMetrics.instanceId, system::getTime()))
                debug_terminal::submitBaselineMetrics("Vessel", m->debugMetrics.instanceId,
                    m->debugMetrics.consumeProcessRange(), timing.consumeStepRange(), timing.consumeDrawRange(), layerTiming.consume());
        }
    }
};
Model* modelVessel = createModel<Vessel, VesselWidget>("Vessel");
