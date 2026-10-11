#include "Strand.hpp"
#include "PanelSvgUtils.hpp"
#include "visual/VisualAssets.hpp"

struct StrandFrequencyDisplayWidget : widget::TransparentWidget {
    Strand* strandModule = nullptr;
    int portIndex = 0;
    float lastFreq = -999.f;
    char cachedText[32] = "---";
    const char* cachedUnit = "";

    StrandFrequencyDisplayWidget(Strand* m, int port) : strandModule(m), portIndex(port) {}

    void step() override {
        widget::TransparentWidget::step();
        float freq = strandModule ? strandModule->getFrequency(portIndex) : 0.f;
        if (std::abs(freq - lastFreq) > 0.01f || (freq == 0.f && lastFreq != 0.f)) {
            lastFreq = freq;
            cachedUnit = freq <= 0.05f ? "" : (freq < 999.5f ? "Hz" : "kHz");
            if (freq <= 0.05f) {
                std::snprintf(cachedText, sizeof(cachedText), "---");
            } else if (freq < 9.95f) {
                std::snprintf(cachedText, sizeof(cachedText), "%.1f", freq);
            } else if (freq < 999.5f) {
                std::snprintf(cachedText, sizeof(cachedText), "%.0f", freq);
            } else if (freq < 9995.f) {
                std::snprintf(cachedText, sizeof(cachedText), "%.2f", freq * 0.001f);
            } else if (freq < 99950.f) {
                std::snprintf(cachedText, sizeof(cachedText), "%.1f", freq * 0.001f);
            } else {
                std::snprintf(cachedText, sizeof(cachedText), "%.0f", freq * 0.001f);
            }
        }
    }

    void draw(const DrawArgs& args) override {
        int fontHandle = (APP && APP->window && APP->window->uiFont) ? APP->window->uiFont->handle : -1;
        if (fontHandle < 0) return;

        nvgBeginPath(args.vg);
        nvgRoundedRect(args.vg, 0.f, 0.f, box.size.x, box.size.y, 2.f);
        nvgFillColor(args.vg, nvgRGBA(0, 0, 0, 210));
        nvgFill(args.vg);

        constexpr float numberSize = 11.5f;
        constexpr float unitSize = 8.5f;
        nvgFontFaceId(args.vg, fontHandle);
        nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE);
        nvgFontSize(args.vg, numberSize);
        const float numberWidth = nvgTextBounds(args.vg, 0.f, 0.f, cachedText, nullptr, nullptr);
        nvgFontSize(args.vg, unitSize);
        const float unitWidth = nvgTextBounds(args.vg, 0.f, 0.f, cachedUnit, nullptr, nullptr);
        const float gap = cachedUnit[0] ? 2.f : 0.f;
        const float totalWidth = numberWidth + gap + unitWidth;
        const float scale = totalWidth > 0.f ? std::min(1.f, std::max(0.f, box.size.x - 6.f) / totalWidth) : 1.f;
        const float x = (box.size.x - totalWidth * scale) * 0.5f;
        nvgFontSize(args.vg, numberSize * scale);
        float ascender = 0.f, descender = 0.f;
        nvgTextMetrics(args.vg, &ascender, &descender, nullptr);
        const float baseline = box.size.y * 0.5f + (ascender + descender) * 0.5f;
        nvgFillColor(args.vg, nvgRGB(255, 255, 255));
        nvgText(args.vg, x, baseline, cachedText, nullptr);
        if (cachedUnit[0]) {
            nvgFontSize(args.vg, unitSize * scale);
            nvgText(args.vg, x + (numberWidth + gap) * scale, baseline, cachedUnit, nullptr);
        }
    }
};

struct StrandWidget : ModuleWidget {
    explicit StrandWidget(Strand* module) {
        setModule(module);
        const std::string panelPath = asset::plugin(pluginInstance, "res/Strand.panel.svg");
        setPanel(createPanel(asset::plugin(pluginInstance, "res/Strand.background.svg")));
        math::Rect artworkRectMm(Vec(0.f, 0.f), Vec(15.24f, 128.5f));
        panel_svg::loadRectFromSvgMm(panelPath, "FULL_PANEL_RASTER", &artworkRectMm);
        addChild(visual_assets::createAspectFitRasterImageWidget(
            "res/Strand/Strand-Panel.png", artworkRectMm));
        visual_assets::addPerfectWaveSoloPanelBranding(this, panelPath);
        auto* border = new app::PanelBorder();
        border->box.size = box.size;
        addChild(border);

        const char* ids[] = {"A", "B", "C"};
        const float ys[] = {32.8f, 67.6f, 100.9f};
        for (int i = 0; i < 3; ++i) {
            Vec p(7.62f, ys[i]);
            panel_svg::loadPointFromSvgMm(panelPath, ids[i], &p);
            if (i < 2) addInput(createInputCentered<Magitek2InputJack>(mm2px(p), module, i));
            else addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(p), module, i - 2));
        }

        const char* freqIds[] = {"FREQ_A", "FREQ_B", "FREQ_C"};
        const float freqYs[] = {42.7f, 77.5f, 110.5f};
        for (int i = 0; i < 3; ++i) {
            math::Rect rMm(Vec(1.2f, freqYs[i]), Vec(12.84f, 5.f));
            panel_svg::loadRectFromSvgMm(panelPath, freqIds[i], &rMm);
            auto* disp = new StrandFrequencyDisplayWidget(module, i);
            disp->box = math::Rect(mm2px(rMm.pos), mm2px(rMm.size));
            addChild(disp);
        }
    }
};
Model* modelStrand = createModel<Strand, StrandWidget>("Strand");
