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

} // namespace

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
      addOutput(createOutputCentered<PJ301MPort>(mm2px(posMm), module, outputId));
    };
    auto addTinyLight = [&](int lightId, const char* anchorId, const Vec& fallbackMm, NVGcolor color) {
      Vec posMm;
      loadAnchorPointMm(panelPath, anchorId, &posMm, fallbackMm);
      auto* light = createLightCentered<SmallSimpleLight<WhiteLight>>(mm2px(posMm), module, lightId);
      light->baseColors[0] = color;
      addChild(light);
    };

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
    menu->addChild(createMenuLabel("Shape Algorithm"));
    menu->addChild(createCheckMenuItem(
      "Geometric (Legacy)", "",
      [m]() { return m->shapeAlgorithm == Undertow::SHAPE_ALGO_GEOMETRIC; },
      [m]() { m->shapeAlgorithm = Undertow::SHAPE_ALGO_GEOMETRIC; }));
    menu->addChild(createCheckMenuItem(
      "Nonlinear (Gem Study)", "",
      [m]() { return m->shapeAlgorithm == Undertow::SHAPE_ALGO_NONLINEAR; },
      [m]() { m->shapeAlgorithm = Undertow::SHAPE_ALGO_NONLINEAR; }));
    menu->addChild(createCheckMenuItem(
      "Threshold Fold (GPT Study)", "",
      [m]() { return m->shapeAlgorithm == Undertow::SHAPE_ALGO_THRESHOLD_FOLD; },
      [m]() { m->shapeAlgorithm = Undertow::SHAPE_ALGO_THRESHOLD_FOLD; }));
  }
};

Model* modelUndertow = createModel<Undertow, UndertowWidget>("Undertow");
