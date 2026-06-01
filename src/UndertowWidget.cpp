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
  bool valid = false;

  explicit UndertowShapePreviewWidget(Undertow* module) : module(module) {
  }

  static std::string formatFrequencyText(float hz) {
    if (!std::isfinite(hz) || hz < 0.f) {
      hz = 0.f;
    }
    if (hz < 1.f) {
      return string::f("%.1f mHz", hz * 1000.f);
    }
    if (hz >= 1000.f) {
      return string::f("%.2f kHz", hz / 1000.f);
    }
    if (hz < 10.f) {
      return string::f("%.2f Hz", hz);
    }
    if (hz < 100.f) {
      return string::f("%.1f Hz", hz);
    }
    return string::f("%.0f Hz", hz);
  }

  void step() override {
    Widget::step();
    if (!module) {
      return;
    }
    uint32_t version = 0;
    float ignoredFrequencyHz = 0.f;
    std::array<float, Undertow::SHAPE_PREVIEW_SAMPLE_COUNT> nextSamples {};
    module->getShapePreview(nextSamples, ignoredFrequencyHz, version);
    if (!valid || version != lastVersion) {
      samples = nextSamples;
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

    const float displayHz = module->displayFrequencyHz.load(std::memory_order_relaxed);
    const std::string freqText = formatFrequencyText(displayHz);
    nvgFontSize(args.vg, LABEL_FONT_SIZE);
    nvgFontFaceId(args.vg, APP->window->uiFont->handle);
    nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 255));
    nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_TOP);
    nvgText(args.vg, box.size.x * 0.5f, box.size.y + 1.5f, freqText.c_str(), nullptr);
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
    auto addModeToggle = [&](int paramId, int lightId, const char* anchorId, const Vec& fallbackMm) {
      Vec posMm;
      loadAnchorPointMm(panelPath, anchorId, &posMm, fallbackMm);
      addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<WhiteLight>>>(
        mm2px(posMm), module, paramId, lightId));
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
    // Top outputs: MORPH / SUB / SINE
    addOutputPort(Undertow::SHAPE_OUTPUT, "SHAPE_OUTPUT", Vec(32.800f, 112.800f));
    addOutputPort(Undertow::SUB_OUTPUT, "SUB_OUTPUT", Vec(20.300f, 112.800f));
    addOutputPort(Undertow::SINE_OUTPUT, "SINE_OUTPUT", Vec(7.800f, 112.800f));

    // Main controls: coarse/fine frequency, morph amount, and lin FM amount.
    addLargeKnob(Undertow::COARSE_PARAM, "COARSE_PARAM", Vec(8.600f, 21.000f));
    addFineKnob(Undertow::FINE_PARAM, "FINE_PARAM", Vec(30.167f, 21.000f));
    addSmallKnob(Undertow::LIN_FM_PARAM, "LIN_FM_PARAM", Vec(8.600f, 76.200f));
    addSmallKnob(Undertow::SHAPE_PARAM, "SHAPE_PARAM", Vec(30.167f, 76.200f));

    // Lower patch field: modulation, pitch, sync, and sub-gate.
    addInputPort(Undertow::LIN_FM_INPUT, "LIN_FM_INPUT", Vec(8.600f, 65.124f));
    addInputPort(Undertow::SHAPE_CV_INPUT, "SHAPE_CV_INPUT", Vec(30.167f, 65.124f));
    addModeToggle(Undertow::COARSE_STEP_MODE_PARAM, Undertow::COARSE_STEP_MODE_LIGHT, "COARSE_STEP_MODE_PARAM",
                  Vec(20.084f, 49.700f));
    addInputPort(Undertow::EXPO_INPUT, "EXPO_INPUT", Vec(20.084f, 55.489f));
    addInputPort(Undertow::V_OCT_INPUT, "V_OCT_INPUT", Vec(20.084f, 30.104f));
    addInputPort(Undertow::SYNC_INPUT, "SYNC_INPUT", Vec(8.600f, 42.500f));
    addInputPort(Undertow::S_GATE_INPUT, "S_GATE_INPUT", Vec(30.167f, 42.817f));

    addTinyLight(Undertow::SYNC_LIGHT, "SYNC_LIGHT", Vec(14.456f, 42.500f), nvgRGB(255, 235, 120));
    addTinyLight(Undertow::S_GATE_LIGHT, "S_GATE_LIGHT", Vec(36.089f, 42.817f), nvgRGB(255, 235, 120));

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
      "Continuous", "", [m]() { return m->params[Undertow::COARSE_STEP_MODE_PARAM].getValue() <= 0.5f; },
      [m]() { m->params[Undertow::COARSE_STEP_MODE_PARAM].setValue(0.f); }));
    menu->addChild(createCheckMenuItem(
      "Octave Stepped", "", [m]() { return m->params[Undertow::COARSE_STEP_MODE_PARAM].getValue() > 0.5f; },
      [m]() { m->params[Undertow::COARSE_STEP_MODE_PARAM].setValue(1.f); }));
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
