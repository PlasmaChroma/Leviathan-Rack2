#include "VTune.hpp"
#include "PanelSvgUtils.hpp"
#include "visual/VisualAssets.hpp"

struct VTuneWidget final : ModuleWidget {
    explicit VTuneWidget(VTune* module) {
        setModule(module);
        visual_assets::SplitPanelRenderer panel(this, "res/VTune.panel.svg");
        panel.addThemedLabels("res/VTune.labels.svg", "res/VTune.theme-text-input.svg", "res/VTune.theme-text-output.svg");
        panel.addCompactLeviathanLogoBranding();
        auto point = [&](const char* id, float x, float y) {
            Vec p;
            return panel_svg::loadPointFromSvgMm(panel.panelPath(), id, &p) ? p : Vec(x, y);
        };

        const char* anchors[] = {
            "VELOCITY_PARAM", "SPEED_PARAM", "PRESSURE_PARAM", "SUSTAIN_PARAM",
            "IMPERFECTION_PARAM", "WIDTH_PARAM", "LEVEL_PARAM"
        };
        const int ids[] = {
            VTune::VELOCITY_PARAM, VTune::SPEED_PARAM, VTune::PRESSURE_PARAM, VTune::SUSTAIN_PARAM,
            VTune::IMPERFECTION_PARAM, VTune::WIDTH_PARAM, VTune::LEVEL_PARAM
        };
        const Vec fallback[] = {
            Vec(10.5f, 22.f), Vec(30.14f, 22.f), Vec(10.5f, 43.f), Vec(30.14f, 43.f),
            Vec(10.5f, 64.f), Vec(30.14f, 64.f), Vec(20.32f, 85.f)
        };
        for (int i = 0; i < 7; ++i) {
            addParam(createParamCentered<Eclipse2Knob>(mm2px(point(anchors[i], fallback[i].x, fallback[i].y)), module, ids[i]));
        }
    }
};

Model* modelVTune = createModel<VTune, VTuneWidget>("VTune");
