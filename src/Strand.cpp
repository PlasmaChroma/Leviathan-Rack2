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
        const char* ids[] = {"A", "B", "C"};
        const float ys[] = {34.f, 68.f, 102.f};
        for (int i = 0; i < 3; ++i) {
            Vec p(7.62f, ys[i]);
            panel_svg::loadPointFromSvgMm(panel.panelPath(), ids[i], &p);
            if (i < 2) addInput(createInputCentered<Magitek2InputJack>(mm2px(p), module, i));
            else addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(p), module, i - 2));
        }
    }
};
Model* modelStrand = createModel<Strand, StrandWidget>("Strand");
