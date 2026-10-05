#include "VTune.hpp"
#include "PanelSvgUtils.hpp"
#include "visual/FractalGlassOverlay.hpp"
#include "visual/VisualAssets.hpp"
#include "vtune/BodyMapWidget.hpp"
struct VTuneWidget final : ModuleWidget {
    explicit VTuneWidget(VTune* module) {
        setModule(module);
        visual_assets::SplitPanelRenderer panel(this, "res/VTune.panel.svg");
        panel.addThemedLabels("res/VTune.labels.svg", "res/VTune.theme-text-input.svg", "res/VTune.theme-text-output.svg");
        panel.addCompactLeviathanLogoBranding();
        visual_assets::addFractalGlassOverlay(
            this, panel.panelPath(), panel.panelSurfaceEffectWidget());
        math::Rect outlineRect(Vec(1.f, 62.f), Vec(38.64f, 56.4f));
        panel_svg::loadRectFromSvgMm(panel.panelPath(), "VF_OUTLINE_RASTER", &outlineRect);
        auto* body = new vtune_body::BodyMapWidget(module);
        body->box = math::Rect(mm2px(outlineRect.pos), mm2px(outlineRect.size));
        addChild(body);
        auto point = [&](const char* id, float x, float y) {
            Vec p;
            return panel_svg::loadPointFromSvgMm(panel.panelPath(), id, &p) ? p : Vec(x, y);
        };

        const char* anchors[] = {
            "VELOCITY_PARAM", "SPEED_PARAM", "PRESSURE_PARAM", "SUSTAIN_PARAM",
            "IMPERFECTION_PARAM", "WIDTH_PARAM"
        };
        const int ids[] = {
            VTune::VELOCITY_PARAM, VTune::SPEED_PARAM, VTune::PRESSURE_PARAM, VTune::SUSTAIN_PARAM,
            VTune::IMPERFECTION_PARAM, VTune::WIDTH_PARAM
        };
        const Vec fallback[] = {
            Vec(30.14f, 22.f), Vec(10.5f, 22.f), Vec(10.5f, 43.f), Vec(30.14f, 43.f),
            Vec(10.5f, 64.f), Vec(30.14f, 64.f)
        };
        addChild(createLightCentered<SmallAperture<AmberGreenApertureLight>>(
            mm2px(point("VESSEL_EXPANDER_LIGHT", 3.2f, 5.8f)), module, VTune::VESSEL_LINK_LIGHT));
        for (int i = 0; i < 6; ++i) {
            addParam(createParamCentered<Eclipse2Knob>(mm2px(point(anchors[i], fallback[i].x, fallback[i].y)), module, ids[i]));
        }
    }
    void appendContextMenu(Menu* menu) override {
        ModuleWidget::appendContextMenu(menu);
        vtune_body::appendBodyMapMenu(menu, static_cast<VTune*>(module));
    }
};

Model* modelVTune = createModel<VTune, VTuneWidget>("VTune");
