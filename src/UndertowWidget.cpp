#include "Undertow.hpp"
#include "PanelSvgUtils.hpp"

namespace {

bool loadAnchorPointMm(const std::string& panelPath, const char* id, Vec* outMm, const Vec& fallbackMm) {
  if (panel_svg::loadPointFromSvgMm(panelPath, id, outMm)) {
    return true;
  }
  *outMm = fallbackMm;
  return false;
}

math::Rect insetRectMm(math::Rect rect, float insetMm) {
  rect.pos.x += insetMm;
  rect.pos.y += insetMm;
  rect.size.x = std::max(0.f, rect.size.x - 2.f * insetMm);
  rect.size.y = std::max(0.f, rect.size.y - 2.f * insetMm);
  return rect;
}

} // namespace

struct UndertowShapePreviewWidget final : Widget {
  static constexpr float WAVE_LINE_WIDTH = 1.25f;
  static constexpr float WAVE_EDGE_PAD = 1.0f;
  static constexpr float LABEL_FONT_SIZE = 11.5f;
  Undertow* module = nullptr;
  std::array<float, Undertow::SHAPE_PREVIEW_SAMPLE_COUNT> samples {};
  uint32_t lastVersion = 0;
  float lastFreqHz = 0.f;
  float displayedFreqHz = 0.f;
  bool valid = false;

  explicit UndertowShapePreviewWidget(Undertow* module) : module(module) {
  }

  void step() override {
    Widget::step();
    if (!module) {
      return;
    }
    uint32_t version = 0;
    float frequencyHz = 0.f;
    std::array<float, Undertow::SHAPE_PREVIEW_SAMPLE_COUNT> nextSamples {};
    module->getShapePreview(nextSamples, frequencyHz, version);
    if (!valid || version != lastVersion) {
      samples = nextSamples;
      lastFreqHz = frequencyHz;
      if (!valid || displayedFreqHz <= 0.f) {
        displayedFreqHz = frequencyHz;
      } else if (std::fabs(frequencyHz - displayedFreqHz) > 0.15f) {
        displayedFreqHz += 0.25f * (frequencyHz - displayedFreqHz);
      }
      lastVersion = version;
      valid = true;
    }
  }

  void draw(const DrawArgs& args) override {
    if (!valid) {
      return;
    }

    const float w = std::max(box.size.x, 1.f);
    const float h = std::max(box.size.y, 1.f);
    const float drawPad = 0.5f * WAVE_LINE_WIDTH + WAVE_EDGE_PAD;
    const float left = drawPad;
    const float top = drawPad;
    const float right = std::max(left + 1.f, w - drawPad);
    const float bottom = std::max(top + 1.f, h - drawPad);
    const float drawW = right - left;
    const float drawH = bottom - top;

    nvgSave(args.vg);
    nvgScissor(args.vg, 0.f, 0.f, w, h);

    nvgBeginPath(args.vg);
    nvgMoveTo(args.vg, left, top + 0.5f * drawH);
    nvgLineTo(args.vg, right, top + 0.5f * drawH);
    nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 36));
    nvgStrokeWidth(args.vg, 0.65f);
    nvgStroke(args.vg);

    nvgBeginPath(args.vg);
    for (int i = 0; i < Undertow::SHAPE_PREVIEW_SAMPLE_COUNT; ++i) {
      const float xNorm = float(i) / float(Undertow::SHAPE_PREVIEW_SAMPLE_COUNT - 1);
      const float x = left + xNorm * drawW;
      const float yNorm = clamp(0.5f - 0.5f * (samples[size_t(i)] / 5.f), 0.f, 1.f);
      const float y = top + yNorm * drawH;
      if (i == 0) {
        nvgMoveTo(args.vg, x, y);
      } else {
        nvgLineTo(args.vg, x, y);
      }
    }
    nvgStrokeColor(args.vg, nvgRGBA(230, 230, 220, 255));
    nvgStrokeWidth(args.vg, WAVE_LINE_WIDTH);
    nvgLineCap(args.vg, NVG_BUTT);
    nvgLineJoin(args.vg, NVG_ROUND);
    nvgStroke(args.vg);

    nvgResetScissor(args.vg);
    nvgRestore(args.vg);

    char freqText[32];
    if (displayedFreqHz < 1.f) {
      std::snprintf(freqText, sizeof(freqText), "%4.0f mHz", displayedFreqHz * 1000.f);
    } else if (displayedFreqHz >= 1000.f) {
      std::snprintf(freqText, sizeof(freqText), "%4.2f kHz", displayedFreqHz / 1000.f);
    } else {
      std::snprintf(freqText, sizeof(freqText), "%5.1f Hz", displayedFreqHz);
    }
    nvgFontSize(args.vg, LABEL_FONT_SIZE);
    nvgFontFaceId(args.vg, APP->window->uiFont->handle);
    nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 255));
    nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_TOP);
    nvgText(args.vg, box.size.x * 0.5f, box.size.y + 1.5f, freqText, nullptr);
  }
};

struct UndertowWidget final : ModuleWidget {
  explicit UndertowWidget(Undertow* module) {
    setModule(module);
    PreviewBuildLogTimer previewBuildTimer("Undertow", module);
    const std::string panelPath = asset::plugin(pluginInstance, "res/undertow.svg");
    setPanel(createPanel(panelPath));
    previewBuildTimer.markPanelDone();
    previewBuildTimer.setAtlasStatus(panel_svg::getAtlasStatusLabelForSvg(panelPath));

    auto addLargeKnob = [&](int paramId, const char* anchorId, const Vec& fallbackMm) {
      Vec posMm;
      loadAnchorPointMm(panelPath, anchorId, &posMm, fallbackMm);
      addParam(createParamCentered<Davies1900hWhiteKnob>(mm2px(posMm), module, paramId));
    };
    auto addFineKnob = [&](int paramId, const char* anchorId, const Vec& fallbackMm) {
      Vec posMm;
      loadAnchorPointMm(panelPath, anchorId, &posMm, fallbackMm);
      addParam(createParamCentered<BefacoTinyKnobWhite>(mm2px(posMm), module, paramId));
    };
    auto addSmallKnob = [&](int paramId, const char* anchorId, const Vec& fallbackMm) {
      Vec posMm;
      loadAnchorPointMm(panelPath, anchorId, &posMm, fallbackMm);
      addParam(createParamCentered<RoundBlackKnob>(mm2px(posMm), module, paramId));
    };
    auto addInputPort = [&](int inputId, const char* anchorId, const Vec& fallbackMm) {
      Vec posMm;
      loadAnchorPointMm(panelPath, anchorId, &posMm, fallbackMm);
      addInput(createInputCentered<DarkPJ301MPort>(mm2px(posMm), module, inputId));
    };
    auto addOutputPort = [&](int outputId, const char* anchorId, const Vec& fallbackMm) {
      Vec posMm;
      loadAnchorPointMm(panelPath, anchorId, &posMm, fallbackMm);
      addOutput(createOutputCentered<BananutBlack>(mm2px(posMm), module, outputId));
    };
    auto addTinyLight = [&](int lightId, const char* anchorId, const Vec& fallbackMm, NVGcolor color) {
      Vec posMm;
      loadAnchorPointMm(panelPath, anchorId, &posMm, fallbackMm);
      auto* light = createLightCentered<SmallSimpleLight<WhiteLight>>(mm2px(posMm), module, lightId);
      light->baseColors[0] = color;
      addChild(light);
    };

    {
      math::Rect previewRectMm;
      if (panel_svg::loadRectFromSvgMm(panelPath, "wave_preview", &previewRectMm)) {
        previewRectMm = insetRectMm(previewRectMm, 0.2f);
        auto* previewWidget = new UndertowShapePreviewWidget(module);
        previewWidget->box.pos = mm2px(previewRectMm.pos);
        previewWidget->box.size = mm2px(previewRectMm.size);
        addChild(previewWidget);
      }
    }

    // STO-style first layout pass with anchor-first lookups.
    // Top outputs: SHAPE / SUB / SINE
    addOutputPort(Undertow::SHAPE_OUTPUT, "SHAPE_OUTPUT", Vec(10.0f, 14.5f));
    addOutputPort(Undertow::SUB_OUTPUT, "SUB_OUTPUT", Vec(20.3f, 14.5f));
    addOutputPort(Undertow::SINE_OUTPUT, "SINE_OUTPUT", Vec(30.6f, 14.5f));

    // Main controls: coarse freq center-left, fine trim near it, shape to the right, lin fm amount left-lower.
    addLargeKnob(Undertow::COARSE_PARAM, "COARSE_PARAM", Vec(14.4f, 40.0f));
    addFineKnob(Undertow::FINE_PARAM, "FINE_PARAM", Vec(27.8f, 35.2f));
    addSmallKnob(Undertow::LIN_FM_PARAM, "LIN_FM_PARAM", Vec(8.6f, 63.8f));
    addSmallKnob(Undertow::SHAPE_PARAM, "SHAPE_PARAM", Vec(29.0f, 63.8f));

    // Lower patch field: STO-style modulation/pitch/sync/sub-gate ordering.
    addInputPort(Undertow::LIN_FM_INPUT, "LIN_FM_INPUT", Vec(8.6f, 84.0f));
    addInputPort(Undertow::SHAPE_CV_INPUT, "SHAPE_CV_INPUT", Vec(29.0f, 84.0f));
    addInputPort(Undertow::EXPO_INPUT, "EXPO_INPUT", Vec(8.6f, 98.8f));
    addInputPort(Undertow::V_OCT_INPUT, "V_OCT_INPUT", Vec(29.0f, 98.8f));
    addInputPort(Undertow::SYNC_INPUT, "SYNC_INPUT", Vec(8.6f, 113.6f));
    addInputPort(Undertow::S_GATE_INPUT, "S_GATE_INPUT", Vec(29.0f, 113.6f));

    addTinyLight(Undertow::SYNC_LIGHT, "SYNC_LIGHT", Vec(5.0f, 8.0f), nvgRGB(255, 235, 120));
    addTinyLight(Undertow::S_GATE_LIGHT, "S_GATE_LIGHT", Vec(35.0f, 8.0f), nvgRGB(120, 255, 140));

    previewBuildTimer.markAnchorsDone();
  }

  void appendContextMenu(Menu* menu) override {
    ModuleWidget::appendContextMenu(menu);
    auto* m = dynamic_cast<Undertow*>(module);
    if (!m) {
      return;
    }
    menu->addChild(new MenuSeparator());
    menu->addChild(createMenuLabel("Coarse Tune"));
    menu->addChild(createCheckMenuItem(
      "Continuous", "",
      [m]() { return !m->coarseTuneStepped; },
      [m]() { m->coarseTuneStepped = false; }));
    menu->addChild(createCheckMenuItem(
      "Octave Stepped", "",
      [m]() { return m->coarseTuneStepped; },
      [m]() { m->coarseTuneStepped = true; }));
    menu->addChild(new MenuSeparator());
    menu->addChild(createMenuLabel("Morph"));
    menu->addChild(createSubmenuItem("Asymmetry", "", [m](Menu* submenu) {
      submenu->addChild(createCheckMenuItem(
        "Off", "",
        [m]() { return !m->shapeEntryAsymmetry; },
        [m]() { m->shapeEntryAsymmetry = false; }));
      submenu->addChild(createCheckMenuItem(
        "Rising", "",
        [m]() { return m->shapeEntryAsymmetry && !m->shapeEntryAsymmetryOnRight; },
        [m]() {
          m->shapeEntryAsymmetry = true;
          m->shapeEntryAsymmetryOnRight = false;
        }));
      submenu->addChild(createCheckMenuItem(
        "Falling", "",
        [m]() { return m->shapeEntryAsymmetry && m->shapeEntryAsymmetryOnRight; },
        [m]() {
          m->shapeEntryAsymmetry = true;
          m->shapeEntryAsymmetryOnRight = true;
        }));
    }));
    menu->addChild(createCheckMenuItem(
      "Hard morph edges", "",
      [m]() { return m->shapeHardEdges; },
      [m]() { m->shapeHardEdges = !m->shapeHardEdges; }));
  }
};

Model* modelUndertow = createModel<Undertow, UndertowWidget>("Undertow");
