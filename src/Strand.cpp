#include "Strand.hpp"
#include "PanelSvgUtils.hpp"
#include "visual/VisualAssets.hpp"
#include "visual/FractalGlassOverlay.hpp"

struct StrandFrequencyDisplayWidget : widget::TransparentWidget {
    Strand* strandModule = nullptr;
    int portIndex = 0;
    float lastFreq = -999.f;
    char cachedText[32] = "---";

    StrandFrequencyDisplayWidget(Strand* m, int port) : strandModule(m), portIndex(port) {}

    void step() override {
        widget::TransparentWidget::step();
        float freq = strandModule ? strandModule->getFrequency(portIndex) : 0.f;
        if (std::abs(freq - lastFreq) > 0.01f || (freq == 0.f && lastFreq != 0.f)) {
            lastFreq = freq;
            if (freq <= 0.05f) {
                std::snprintf(cachedText, sizeof(cachedText), "---");
            } else if (freq < 9.95f) {
                std::snprintf(cachedText, sizeof(cachedText), "%.1f Hz", freq);
            } else if (freq < 999.5f) {
                std::snprintf(cachedText, sizeof(cachedText), "%.0f Hz", freq);
            } else if (freq < 9995.f) {
                std::snprintf(cachedText, sizeof(cachedText), "%.2fk", freq * 0.001f);
            } else if (freq < 99950.f) {
                std::snprintf(cachedText, sizeof(cachedText), "%.1fk", freq * 0.001f);
            } else {
                std::snprintf(cachedText, sizeof(cachedText), "%.0fk", freq * 0.001f);
            }
        }
    }

    void draw(const DrawArgs& args) override {
        int fontHandle = (APP && APP->window && APP->window->uiFont) ? APP->window->uiFont->handle : -1;
        if (fontHandle < 0) return;

        float fontSize = 9.5f;
        nvgFontSize(args.vg, fontSize);
        nvgFontFaceId(args.vg, fontHandle);
        float bounds[4];
        nvgTextBounds(args.vg, 0.f, 0.f, cachedText, nullptr, bounds);
        float textWidth = bounds[2] - bounds[0];
        if (textWidth > box.size.x - 2.f && textWidth > 0.f) {
            fontSize *= (box.size.x - 2.f) / textWidth;
            nvgFontSize(args.vg, fontSize);
        }
        nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
        nvgFillColor(args.vg, nvgRGB(255, 255, 255));
        nvgText(args.vg, box.size.x * 0.5f, box.size.y * 0.5f + 0.5f, cachedText, nullptr);
    }
};

struct StrandWidget : ModuleWidget {
    explicit StrandWidget(Strand* module) {
        setModule(module);
        visual_assets::SplitPanelRenderer panel(this, "res/Strand.panel.svg");
        visual_assets::addFractalGlassOverlay(this, panel.panelPath(), panel.panelSurfaceEffectWidget());
        panel.addPerfectWaveSoloBranding();
        panel.addThemedLabels("res/Strand.labels.svg", "res/Strand.theme-text-input.svg", "res/Strand.theme-text-output.svg");

        const char* ids[] = {"A", "B", "C"};
        const float ys[] = {30.f, 64.f, 98.f};
        for (int i = 0; i < 3; ++i) {
            Vec p(7.62f, ys[i]);
            panel_svg::loadPointFromSvgMm(panel.panelPath(), ids[i], &p);
            if (i < 2) addInput(createInputCentered<Magitek2InputJack>(mm2px(p), module, i));
            else addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(p), module, i - 2));
        }

        const char* freqIds[] = {"FREQ_A", "FREQ_B", "FREQ_C"};
        const float freqYs[] = {39.5f, 73.5f, 107.5f};
        for (int i = 0; i < 3; ++i) {
            math::Rect rMm(Vec(1.2f, freqYs[i]), Vec(12.84f, 5.f));
            panel_svg::loadRectFromSvgMm(panel.panelPath(), freqIds[i], &rMm);
            auto* disp = new StrandFrequencyDisplayWidget(module, i);
            disp->box = math::Rect(mm2px(rMm.pos), mm2px(rMm.size));
            addChild(disp);
        }
    }
};
Model* modelStrand = createModel<Strand, StrandWidget>("Strand");
