#include "Vessel.hpp"
#include "PanelSvgUtils.hpp"
#include "visual/VisualAssets.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>

namespace {
struct VesselPerformanceArea : app::Switch {
    enum class Kind { Strike, Rotate };
    Kind kind;
    ui::Tooltip* padTooltip = nullptr;
    float currentAmount = 1.f;
    float dragY = 0.f;
    bool hovered = false;

    explicit VesselPerformanceArea(Kind kind) : kind(kind) { momentary = true; }
    ~VesselPerformanceArea() override { destroyPadTooltip(); }

    float amountAt(float y) const {
        return 1.f - clamp(box.size.y > 0.f ? y / box.size.y : 0.f, 0.f, 1.f);
    }
    void publishAmount(float y) {
        dragY = clamp(y, 0.f, box.size.y);
        currentAmount = amountAt(dragY);
        if (auto* vessel = dynamic_cast<Vessel*>(module)) {
            if (kind == Kind::Strike)
                vessel->manualStrikeVelocity.store(currentAmount, std::memory_order_relaxed);
            else
                vessel->manualRotateSpeedScale.store(currentAmount, std::memory_order_relaxed);
        }
        refreshPadTooltip();
    }
    std::string tooltipText() const {
        const int percent = int(std::lround(100.f * currentAmount));
        return kind == Kind::Strike
            ? string::f("Strike: %d%% Velocity", percent)
            : string::f("Rub: %d%% Speed", percent);
    }
    void createPadTooltip() {
        if (!settings::tooltips || padTooltip || !APP || !APP->scene) return;
        padTooltip = new ui::Tooltip();
        padTooltip->text = tooltipText();
        APP->scene->addChild(padTooltip);
    }
    void destroyPadTooltip() {
        if (!padTooltip) return;
        if (padTooltip->parent) padTooltip->parent->removeChild(padTooltip);
        delete padTooltip;
        padTooltip = nullptr;
    }
    void refreshPadTooltip() {
        if (padTooltip) padTooltip->text = tooltipText();
    }
    void onHover(const event::Hover& e) override {
        publishAmount(e.pos.y);
        app::Switch::onHover(e);
    }
    void onButton(const event::Button& e) override {
        if (e.button != GLFW_MOUSE_BUTTON_LEFT) return;
        if (e.action == GLFW_PRESS) publishAmount(e.pos.y);
        app::Switch::onButton(e);
    }
    void onDragMove(const event::DragMove& e) override {
        const float zoom = std::max(getAbsoluteZoom(), 1e-6f);
        publishAmount(dragY + e.mouseDelta.y / zoom);
        app::Switch::onDragMove(e);
    }
    void onEnter(const event::Enter& e) override {
        hovered = true;
        app::Switch::onEnter(e);
        app::ParamWidget::destroyTooltip();
        createPadTooltip();
    }
    void onLeave(const event::Leave& e) override {
        hovered = false;
        destroyPadTooltip();
        app::Switch::onLeave(e);
    }
    void step() override {
        app::Switch::step();
        refreshPadTooltip();
    }
    void draw(const DrawArgs& args) override {
        const bool active = module && module->params[paramId].getValue() >= .5f;
        const NVGcolor color = kind == Kind::Strike ? nvgRGB(74, 222, 214) : nvgRGB(163, 113, 245);
        const float inset = mm2px(.35f);
        const float radius = mm2px(2.2f);
        nvgBeginPath(args.vg);
        nvgRoundedRect(args.vg, inset, inset,
            std::max(0.f, box.size.x - 2.f * inset), std::max(0.f, box.size.y - 2.f * inset), radius);
        nvgFillColor(args.vg, nvgTransRGBA(color, active ? 38 : hovered ? 24 : 14));
        nvgFill(args.vg);
        nvgStrokeWidth(args.vg, active ? 1.35f : 0.9f);
        nvgStrokeColor(args.vg, nvgTransRGBA(color, active ? 130 : hovered ? 90 : 55));
        nvgStroke(args.vg);
    }
};
struct VesselStrikeArea final : VesselPerformanceArea {
    VesselStrikeArea() : VesselPerformanceArea(Kind::Strike) {}
};
struct VesselRotateArea final : VesselPerformanceArea {
    VesselRotateArea() : VesselPerformanceArea(Kind::Rotate) {}
};
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
        nvgText(args.vg, 1, mm2px(5.6f), fault ? "RATE / CONTACT FAULT" : "ENERGY", nullptr);
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
    Widget* metalBowlRaster = nullptr;
    Widget* crystalBowlRaster = nullptr;
    explicit VesselWidget(Vessel* module) {
        setModule(module);
        visual_assets::SplitPanelRenderer panel(this, "res/Vessel.panel.svg");
        panel.addThemedLabels("res/Vessel.labels.svg", "res/Vessel.theme-text-input.svg", "res/Vessel.theme-text-output.svg");
        panel.addCompactLeviathanLogoBranding();
        auto point = [&](const char* id, float x, float y) {
            Vec p; return panel_svg::loadPointFromSvgMm(panel.panelPath(), id, &p) ? p : Vec(x, y);
        };
        auto* display = new BowlDisplay(); display->vessel = module;
        math::Rect r(Vec(3.5f, 15.f), Vec(74.28f, 8.f));
        panel_svg::loadRectFromSvgMm(panel.panelPath(), "ENERGY_DISPLAY", &r);
        display->box.pos = mm2px(r.pos); display->box.size = mm2px(r.size); addChild(display);
        math::Rect bowlRasterRect(Vec(6.64f, 29.89f), Vec(68.f, 40.22f));
        panel_svg::loadRectFromSvgMm(panel.panelPath(), "BOWL_RASTER", &bowlRasterRect);
        metalBowlRaster = visual_assets::createAspectFitRasterImageWidget(
            "res/Vessel/Metal-Crop-Only.png", bowlRasterRect);
        crystalBowlRaster = visual_assets::createAspectFitRasterImageWidget(
            "res/Vessel/Crystal-Crop-Only.png", bowlRasterRect);
        addChild(createLightCentered<SmallAperture<AmberGreenApertureLight>>(
            mm2px(point("VTUNE_EXPANDER_LIGHT", 78.08f, 5.8f)), module, Vessel::VTUNE_LINK_LIGHT));
        addParam(createParamCentered<Eclipse2Knob>(mm2px(point("BINAURAL_PARAM", 12.f, 85.f)), module, Vessel::BINAURAL_PARAM));
        addParam(createParamCentered<PlasmaSwitch>(mm2px(point("BOWL_PARAM", 29.f, 85.f)), module, Vessel::BOWL_PARAM));
        addParam(createParamCentered<Eclipse2Knob>(mm2px(point("PITCH_PARAM", 42.5f, 85.f)), module, Vessel::PITCH_PARAM));
        addParam(createParamCentered<BipolarDarkTinyClockworkGearKnob>(mm2px(point("FINE_PARAM", 55.25f, 85.f)), module, Vessel::FINE_PARAM));
        addParam(createParamCentered<Eclipse2Knob>(mm2px(point("MALLET_PARAM", 71.f, 85.f)), module, Vessel::MALLET_PARAM));
        math::Rect strikeRect(Vec(3.5f, 24.5f), Vec(36.74f, 51.f));
        panel_svg::loadRectFromSvgMm(panel.panelPath(), "STRIKE_AREA", &strikeRect);
        auto* strikeArea = createParam<VesselStrikeArea>(mm2px(strikeRect.pos), module, Vessel::STRIKE_PARAM);
        strikeArea->box.size = mm2px(strikeRect.size); addParam(strikeArea);
        math::Rect rotateRect(Vec(41.04f, 24.5f), Vec(36.74f, 51.f));
        panel_svg::loadRectFromSvgMm(panel.panelPath(), "ROTATE_AREA", &rotateRect);
        auto* rotateArea = createParam<VesselRotateArea>(mm2px(rotateRect.pos), module, Vessel::ROTATE_PARAM);
        rotateArea->box.size = mm2px(rotateRect.size); addParam(rotateArea);
        // Keep the bowl visually above the pad surfaces. These are transparent,
        // non-interactive raster widgets, so the underlying pad controls remain usable.
        addChild(metalBowlRaster);
        addChild(crystalBowlRaster);
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
        auto* vessel = static_cast<Vessel*>(module);
        const bool crystalSelected = vessel && vessel->params[Vessel::BOWL_PARAM].getValue() >= .5f;
        if (metalBowlRaster) metalBowlRaster->setVisible(!crystalSelected);
        if (crystalBowlRaster) crystalBowlRaster->setVisible(crystalSelected);
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
