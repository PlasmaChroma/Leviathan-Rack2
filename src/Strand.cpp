#include "Strand.hpp"
#include "PanelSvgUtils.hpp"
#include "visual/VisualAssets.hpp"
#include "visual/FractalGlassOverlay.hpp"

struct StrandWidget : ModuleWidget {
    explicit StrandWidget(Strand* module) {
        setModule(module);
        visual_assets::SplitPanelRenderer panel(this, "res/Strand.panel.svg");
        visual_assets::addFractalGlassOverlay(this, panel.panelPath(), panel.panelSurfaceEffectWidget());
        panel.addPerfectWaveSoloBranding();
        panel.addThemedLabels("res/Strand.labels.svg", "res/Strand.theme-text-input.svg", "res/Strand.theme-text-output.svg");
        const char* ids[] = {"A_L", "A_R", "B_L", "B_R", "C_L", "C_R"};
        const float ys[] = {25.f, 39.f, 58.f, 72.f, 91.f, 105.f};
        for (int i = 0; i < 6; ++i) {
            Vec p(7.62f, ys[i]);
            panel_svg::loadPointFromSvgMm(panel.panelPath(), ids[i], &p);
            if (i < 4) addInput(createInputCentered<Magitek2InputJack>(mm2px(p), module, i));
            else addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(p), module, i - 4));
        }
    }
};
Model* modelStrand = createModel<Strand, StrandWidget>("Strand");
