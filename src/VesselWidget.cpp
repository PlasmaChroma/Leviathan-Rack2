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
        char text[48]; std::snprintf(text, sizeof(text), "L: %.1f Hz  R: %.1f Hz", hz-.5f*delta, hz+.5f*delta);
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
        panel.addCompactLeviathanLogoBranding();
        auto point = [&](const char* id, float x, float y) {
            Vec p; return panel_svg::loadPointFromSvgMm(panel.panelPath(), id, &p) ? p : Vec(x, y);
        };
        auto* display = new BowlDisplay(); display->vessel = module;
        math::Rect r(Vec(6, 15), Vec(69.28f, 8));
        panel_svg::loadRectFromSvgMm(panel.panelPath(), "ENERGY_DISPLAY", &r);
        display->box.pos = mm2px(r.pos); display->box.size = mm2px(r.size); addChild(display);
        addParam(createParamCentered<Eclipse2Knob>(mm2px(point("PITCH_PARAM", 27.f, 36.f)), module, Vessel::PITCH_PARAM));
        addParam(createParamCentered<BipolarDarkTinyClockworkGearKnob>(mm2px(point("FINE_PARAM", 54.f, 36.f)), module, Vessel::FINE_PARAM));
        addParam(createParamCentered<Eclipse2Knob>(mm2px(point("BINAURAL_PARAM", 12.f, 66.f)), module, Vessel::BINAURAL_PARAM));
        addParam(createParamCentered<PlasmaSwitch>(mm2px(point("BOWL_PARAM", 30, 66)), module, Vessel::BOWL_PARAM));
        addParam(createParamCentered<Eclipse2Knob>(mm2px(point("MALLET_PARAM", 45.f, 66.f)), module, Vessel::MALLET_PARAM));
        addParam(createParamCentered<SmallGoldButton>(mm2px(point("STRIKE_PARAM", 59, 66)), module, Vessel::STRIKE_PARAM));
        addParam(createParamCentered<RotationButton>(mm2px(point("ROTATE_PARAM", 73, 66)), module, Vessel::ROTATE_PARAM));
        const char* inputs[] = {"VOCT_INPUT", "STRIKE_INPUT", "VELOCITY_INPUT", "ROTATE_INPUT", "SPEED_INPUT", "PRESSURE_INPUT"};
        for (int i = 0; i < 6; ++i) addInput(createInputCentered<Magitek2InputJack>(
            mm2px(point(inputs[i], 12.f+19.f*(i%4), i < 4 ? 98.f : 110.5f)), module, i));
        addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(point("LEFT_OUTPUT", 50, 110.5f)), module, Vessel::LEFT_OUTPUT));
        addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(point("RIGHT_OUTPUT", 69, 110.5f)), module, Vessel::RIGHT_OUTPUT));
    }
    void appendContextMenu(Menu* menu) override {
        ModuleWidget::appendContextMenu(menu);
        auto* m = static_cast<Vessel*>(module);
        if (!m) return;
        menu->addChild(new MenuSeparator);
        menu->addChild(createSubmenuItem("Processing quality (performance test)", "", [m](Menu* sub) {
            const char* labels[] = {"48 kHz — Economy", "96 kHz — Balanced", "192 kHz — Reference"};
            for (int i = 0; i < 3; ++i) sub->addChild(createCheckMenuItem(labels[i], "",
                [m, i]() { return m->requestedQuality.load(std::memory_order_relaxed) == i; },
                [m, i]() { m->requestedQuality.store(i, std::memory_order_relaxed); }));
            sub->addChild(new MenuSeparator);
            sub->addChild(createMenuLabel("Switching clears the ringing bowl."));
            sub->addChild(createMenuLabel("Targets follow the host sample rate."));
        }));
        const float rate = m->visualInternalRate.load(std::memory_order_relaxed);
        menu->addChild(createMenuLabel(string::f("Actual internal rate: %.1f kHz%s", rate/1000.f,
            m->visualRateFallback.load(std::memory_order_relaxed) ? " (fallback)" : "")));
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
