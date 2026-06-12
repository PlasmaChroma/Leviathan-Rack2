#include "Undertow.hpp"
#include "UndertowShape.hpp"
#include "PanelSvgUtils.hpp"
#include "VisualAssets.hpp"
#include "WavePreviewTracer.hpp"
#include <array>

namespace {

struct UndertowEdgeHardnessQuantity final : Quantity {
  Undertow* module = nullptr;

  explicit UndertowEdgeHardnessQuantity(Undertow* module) : module(module) {}

  void setValue(float value) override {
    if (!module) {
      return;
    }
    module->params[Undertow::EDGE_HARDNESS_PARAM].setValue(clamp(value, 0.f, 1.f));
  }

  float getValue() override {
    return module ? module->params[Undertow::EDGE_HARDNESS_PARAM].getValue() : 0.5f;
  }

  float getDefaultValue() override {
    return 0.5f;
  }

  float getMinValue() override {
    return 0.f;
  }

  float getMaxValue() override {
    return 1.f;
  }

  std::string getLabel() override {
    return "Morph edge hardness";
  }

  std::string getUnit() override {
    return "%";
  }

  float getDisplayValue() override {
    return getValue() * 100.f;
  }

  void setDisplayValue(float displayValue) override {
    setValue(displayValue / 100.f);
  }

  std::string getDisplayValueString() override {
    return string::f("%.0f", getDisplayValue());
  }
};

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
  static constexpr int PREVIEW_POINT_COUNT = 256;
  static constexpr float WAVE_LINE_WIDTH = 1.25f;
  static constexpr float WAVE_EDGE_PAD = 1.0f;
  static constexpr float LABEL_FONT_SIZE = 11.5f;
  static constexpr float DEFAULT_FREQUENCY_HZ = 261.63f;
  static constexpr float DEFAULT_SHAPE_AMOUNT = 0.f;
  static constexpr float DEFAULT_EDGE_HARDNESS = 0.5f;
  static constexpr int TRAIL_FRAME_COUNT = 11;
  static constexpr float TRAIL_FADE_SEC = 0.333f;
  static constexpr float TRAIL_MIN_CAPTURE_INTERVAL_SEC = 1.f / 24.f;
  static constexpr float TRAIL_LINE_WIDTH = 1.05f;
  static constexpr int TRAIL_DRAW_STRIDE = 2;
  Undertow* module = nullptr;
  std::array<float, PREVIEW_POINT_COUNT> samples {};
  std::array<Vec, PREVIEW_POINT_COUNT> points {};
  WavePreviewTracer<PREVIEW_POINT_COUNT, TRAIL_FRAME_COUNT> curveTracer;
  WavePreviewBufferedTracer<PREVIEW_POINT_COUNT> frameTracer;
  bool samplesInitialized = false;
  bool pointsInitialized = false;
  bool hasLastPreviewState = false;
  float lastShapeAmount = DEFAULT_SHAPE_AMOUNT;
  float lastEdgeHardness = DEFAULT_EDGE_HARDNESS;
  bool lastAsymEnabled = false;
  bool lastAsymOnRight = false;
  Vec lastPointSize;

  explicit UndertowShapePreviewWidget(Undertow* module) : module(module) {
    refreshSamples(DEFAULT_SHAPE_AMOUNT, DEFAULT_EDGE_HARDNESS, false, false);
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

  void refreshSamples(float shapeAmount, float edgeHardness, bool asymEnabled, bool asymOnRight) {
    for (int i = 0; i < PREVIEW_POINT_COUNT; ++i) {
      const float phase = float(i) / float(PREVIEW_POINT_COUNT - 1);
      const float folded = undertow_shape::thresholdFold(phase, shapeAmount, asymEnabled, edgeHardness, asymOnRight);
      samples[size_t(i)] = clamp(folded, -1.f, 1.f) * 5.f;
    }
    samplesInitialized = true;
  }

  void rebuildPoints() {
    const float w = std::max(box.size.x, 1.f);
    const float h = std::max(box.size.y, 1.f);
    const float drawPad = 0.5f * WAVE_LINE_WIDTH + WAVE_EDGE_PAD;
    const float left = drawPad;
    const float top = drawPad;
    const float right = std::max(left + 1.f, w - drawPad);
    const float bottom = std::max(top + 1.f, h - drawPad);
    const float drawW = right - left;
    const float drawH = bottom - top;
    for (int i = 0; i < PREVIEW_POINT_COUNT; ++i) {
      const float xNorm = float(i) / float(PREVIEW_POINT_COUNT - 1);
      const float x = left + xNorm * drawW;
      const float yNorm = clamp(0.5f - 0.5f * (samples[size_t(i)] / 5.f), 0.f, 1.f);
      points[size_t(i)] = Vec(x, top + yNorm * drawH);
    }
    pointsInitialized = true;
    lastPointSize = box.size;
  }

  void step() override {
    Widget::step();
    const double nowSec = system::getTime();
    if (module) {
      const float shapeAmount = module->displayShapeAmount.load(std::memory_order_relaxed);
      const float edgeHardness = module->params[Undertow::EDGE_HARDNESS_PARAM].getValue();
      const bool asymEnabled = module->shapeEntryAsymmetry.load(std::memory_order_relaxed);
      const bool asymOnRight = module->shapeEntryAsymmetryOnRight.load(std::memory_order_relaxed);
      const bool curveChanged = !hasLastPreviewState ||
                                std::fabs(shapeAmount - lastShapeAmount) > 1e-4f ||
                                std::fabs(edgeHardness - lastEdgeHardness) > 1e-4f ||
                                asymEnabled != lastAsymEnabled ||
                                asymOnRight != lastAsymOnRight;
      const bool sizeChanged = std::fabs(box.size.x - lastPointSize.x) > 0.5f ||
                               std::fabs(box.size.y - lastPointSize.y) > 0.5f;
      const bool tracerEnabled = module->previewTracerEnabled.load(std::memory_order_relaxed);
      const int tracerMode = module->previewTracerCacheMode.load(std::memory_order_relaxed);
      if (!tracerEnabled) {
        curveTracer.clear();
        frameTracer.clear();
      }
      else if (tracerMode == WAVE_PREVIEW_TRACER_CURVE_CACHE) {
        curveTracer.expire(nowSec, TRAIL_FADE_SEC);
        frameTracer.clear();
      }
      else {
        curveTracer.clear();
      }
      if (curveChanged) {
        if (tracerEnabled && pointsInitialized) {
          if (tracerMode == WAVE_PREVIEW_TRACER_CURVE_CACHE) {
            curveTracer.capture(points, nowSec, TRAIL_MIN_CAPTURE_INTERVAL_SEC);
          }
          else {
            WavePreviewBufferedTracerStyle style;
            style.color = nvgRGBA(255, 190, 80, 255);
            style.fadeSec = TRAIL_FADE_SEC;
            style.minCaptureIntervalSec = TRAIL_MIN_CAPTURE_INTERVAL_SEC;
            style.maxAlpha = 104.f;
            style.drawStride = TRAIL_DRAW_STRIDE;
            frameTracer.capture(points, nowSec, box.size, style);
          }
        }
        refreshSamples(shapeAmount, edgeHardness, asymEnabled, asymOnRight);
        rebuildPoints();
        lastShapeAmount = shapeAmount;
        lastEdgeHardness = edgeHardness;
        lastAsymEnabled = asymEnabled;
        lastAsymOnRight = asymOnRight;
        hasLastPreviewState = true;
      }
      else if (!pointsInitialized || sizeChanged) {
        rebuildPoints();
      }
    }
    else if (!samplesInitialized) {
      refreshSamples(DEFAULT_SHAPE_AMOUNT, DEFAULT_EDGE_HARDNESS, false, false);
      rebuildPoints();
    }
    else if (!pointsInitialized) {
      rebuildPoints();
    }
  }

  void draw(const DrawArgs& args) override {
    if (!samplesInitialized) {
      refreshSamples(DEFAULT_SHAPE_AMOUNT, DEFAULT_EDGE_HARDNESS, false, false);
    }
    if (!pointsInitialized) {
      rebuildPoints();
    }

    const float w = std::max(box.size.x, 1.f);
    const float h = std::max(box.size.y, 1.f);
    const float drawPad = 0.5f * WAVE_LINE_WIDTH + WAVE_EDGE_PAD;
    const float left = drawPad;
    const float top = drawPad;
    const float right = std::max(left + 1.f, w - drawPad);
    const float bottom = std::max(top + 1.f, h - drawPad);
    const float drawH = bottom - top;

    nvgSave(args.vg);
    nvgScissor(args.vg, 0.f, 0.f, w, h);

    nvgBeginPath(args.vg);
    nvgMoveTo(args.vg, left, top + 0.5f * drawH);
    nvgLineTo(args.vg, right, top + 0.5f * drawH);
    nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 36));
    nvgStrokeWidth(args.vg, 0.65f);
    nvgStroke(args.vg);

    if (module && module->previewTracerEnabled.load(std::memory_order_relaxed)) {
      const int tracerMode = module->previewTracerCacheMode.load(std::memory_order_relaxed);
      if (tracerMode == WAVE_PREVIEW_TRACER_CURVE_CACHE) {
        WavePreviewTracerStyle style;
        style.color = nvgRGBA(255, 190, 80, 255);
        style.lineWidth = TRAIL_LINE_WIDTH;
        style.fadeSec = TRAIL_FADE_SEC;
        style.minCaptureIntervalSec = TRAIL_MIN_CAPTURE_INTERVAL_SEC;
        style.maxAlpha = 104.f;
        style.drawStride = TRAIL_DRAW_STRIDE;
        curveTracer.draw(args.vg, system::getTime(), style);
      }
      else {
        WavePreviewBufferedTracerStyle style;
        style.color = nvgRGBA(255, 190, 80, 255);
        style.fadeSec = TRAIL_FADE_SEC;
        style.minCaptureIntervalSec = TRAIL_MIN_CAPTURE_INTERVAL_SEC;
        style.maxAlpha = 104.f;
        style.drawStride = TRAIL_DRAW_STRIDE;
        frameTracer.draw(args.vg, system::getTime(), box.size, style);
      }
    }

    nvgBeginPath(args.vg);
    nvgMoveTo(args.vg, points[0].x, points[0].y);
    for (int i = 1; i < PREVIEW_POINT_COUNT; ++i) {
      nvgLineTo(args.vg, points[size_t(i)].x, points[size_t(i)].y);
    }
    nvgStrokeColor(args.vg, nvgRGBA(230, 230, 220, 255));
    nvgStrokeWidth(args.vg, WAVE_LINE_WIDTH);
    nvgLineCap(args.vg, NVG_BUTT);
    nvgLineJoin(args.vg, NVG_ROUND);
    nvgStroke(args.vg);

    nvgResetScissor(args.vg);
    nvgRestore(args.vg);

    float displayHz = DEFAULT_FREQUENCY_HZ;
    if (module) {
      displayHz = module->displayFrequencyHz.load(std::memory_order_relaxed);
      if (displayHz <= 0.f) {
        displayHz = undertowBaseFrequencyFromKnob(module->params[Undertow::COARSE_PARAM].getValue());
      }
    }
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
    addChild(createWidget<ScrewSilver>(Vec(0.f, 0.f)));
    addChild(createWidget<ScrewSilver>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    previewBuildTimer.markPanelDone();
    previewBuildTimer.setAtlasStatus(panel_svg::getAtlasStatusLabelForSvg(panelPath));

    auto addLargeKnob = [&](int paramId, const char* anchorId, const Vec& fallbackMm) {
      Vec posMm;
      loadAnchorPointMm(panelPath, anchorId, &posMm, fallbackMm);
      addParam(createParamCentered<BigClockworkGearKnob>(mm2px(posMm), module, paramId));
    };
    auto addFineKnob = [&](int paramId, const char* anchorId, const Vec& fallbackMm) {
      Vec posMm;
      loadAnchorPointMm(panelPath, anchorId, &posMm, fallbackMm);
      addParam(createParamCentered<BipolarTinyClockworkGearKnob>(mm2px(posMm), module, paramId));
    };
    auto addSmallKnob = [&](int paramId, const char* anchorId, const Vec& fallbackMm) {
      Vec posMm;
      loadAnchorPointMm(panelPath, anchorId, &posMm, fallbackMm);
      addParam(createParamCentered<EclipseKnob>(mm2px(posMm), module, paramId));
    };
    auto addTinyKnob = [&](int paramId, const char* anchorId, const Vec& fallbackMm) {
      Vec posMm;
      loadAnchorPointMm(panelPath, anchorId, &posMm, fallbackMm);
      addParam(createParamCentered<TinyClockworkGearKnob>(mm2px(posMm), module, paramId));
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
    addTinyKnob(Undertow::EDGE_HARDNESS_PARAM, "EDGE_HARDNESS_PARAM", Vec(20.084f, 71.2f));
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
        [m]() { return !m->shapeEntryAsymmetry.load(std::memory_order_relaxed); },
        [m]() { m->shapeEntryAsymmetry.store(false, std::memory_order_relaxed); }));
      submenu->addChild(createCheckMenuItem(
        "Rising", "",
        [m]() {
          return m->shapeEntryAsymmetry.load(std::memory_order_relaxed) &&
                 !m->shapeEntryAsymmetryOnRight.load(std::memory_order_relaxed);
        },
        [m]() {
          m->shapeEntryAsymmetry.store(true, std::memory_order_relaxed);
          m->shapeEntryAsymmetryOnRight.store(false, std::memory_order_relaxed);
        }));
      submenu->addChild(createCheckMenuItem(
        "Falling", "",
        [m]() {
          return m->shapeEntryAsymmetry.load(std::memory_order_relaxed) &&
                 m->shapeEntryAsymmetryOnRight.load(std::memory_order_relaxed);
        },
        [m]() {
          m->shapeEntryAsymmetry.store(true, std::memory_order_relaxed);
          m->shapeEntryAsymmetryOnRight.store(true, std::memory_order_relaxed);
        }));
    }));
    menu->addChild(createCheckMenuItem(
      "Analog character", "",
      [m]() { return m->analogCharacterEnabled.load(std::memory_order_relaxed); },
      [m]() { m->analogCharacterEnabled.store(!m->analogCharacterEnabled.load(std::memory_order_relaxed),
                                              std::memory_order_relaxed); }));
    menu->addChild(createCheckMenuItem(
      "Preview tracer", "",
      [m]() { return m->previewTracerEnabled.load(std::memory_order_relaxed); },
      [m]() { m->previewTracerEnabled.store(!m->previewTracerEnabled.load(std::memory_order_relaxed),
                                            std::memory_order_relaxed); }));
    menu->addChild(createSubmenuItem("Tracer Quality", "", [m](Menu* submenu) {
      submenu->addChild(createCheckMenuItem(
        "Curve cache", "",
        [m]() { return m->previewTracerCacheMode.load(std::memory_order_relaxed) == WAVE_PREVIEW_TRACER_CURVE_CACHE; },
        [m]() { m->previewTracerCacheMode.store(WAVE_PREVIEW_TRACER_CURVE_CACHE, std::memory_order_relaxed); }));
      submenu->addChild(createCheckMenuItem(
        "Frame cache", "",
        [m]() { return m->previewTracerCacheMode.load(std::memory_order_relaxed) == WAVE_PREVIEW_TRACER_FRAME_CACHE; },
        [m]() { m->previewTracerCacheMode.store(WAVE_PREVIEW_TRACER_FRAME_CACHE, std::memory_order_relaxed); }));
    }));
    auto* edgeHardnessSlider = new ui::Slider();
    edgeHardnessSlider->box.size = Vec(180.f, 24.f);
    edgeHardnessSlider->quantity = new UndertowEdgeHardnessQuantity(m);
    menu->addChild(edgeHardnessSlider);
  }
};

Model* modelUndertow = createModel<Undertow, UndertowWidget>("Undertow");
