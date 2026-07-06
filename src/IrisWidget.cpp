#include "Iris.hpp"
#include "NvgGraphicsLifecycle.hpp"
#include "PanelSvgUtils.hpp"
#include "visual/VisualAssets.hpp"

#include <cctype>
#include <cstdlib>
#include <osdialog.h>

namespace {

constexpr float kIrisDisplayVerticalInset = 1.f;

bool supportedImagePath(const std::string& path) {
  std::string extension = system::getExtension(path);
  for (size_t i = 0; i < extension.size(); ++i) extension[i] = char(std::tolower(extension[i]));
  return extension == ".png" || extension == ".jpg" || extension == ".jpeg" ||
         extension == ".bmp" || extension == ".tga";
}

void chooseIrisImage(Iris* module) {
  if (!module) return;
  osdialog_filters* filters = osdialog_filters_parse("Images:png,jpg,jpeg,bmp,tga");
  char* path = osdialog_file(OSDIALOG_OPEN, nullptr, nullptr, filters);
  osdialog_filters_free(filters);
  if (!path) return;
  const std::string selected(path);
  std::free(path);
  module->requestImageLoad(selected);
}

const iris::ImageWavetable& irisBrowserPreviewTable() {
  static const iris::ImageWavetable table = iris::makeDefaultTable();
  return table;
}

const std::vector<uint8_t>& irisBrowserPreviewPixels() {
  static std::vector<uint8_t> pixels;
  if (pixels.empty()) {
    const iris::ImageWavetable& table = irisBrowserPreviewTable();
    const int width = 128;
    const int height = 64;
    pixels.assign(size_t(width * height), 0u);
    for (int y = 0; y < height; ++y) {
      const float scan = (float(y) + 0.5f) / float(height);
      for (int x = 0; x < width; ++x) {
        const float phase = (float(x) + 0.5f) / float(width);
        const float sample = table.sample(phase, scan);
        pixels[size_t(y * width + x)] =
          uint8_t(std::round(clamp(sample * 0.5f + 0.5f, 0.f, 1.f) * 255.f));
      }
    }
  }
  return pixels;
}

void irisBrowserWaveformSnapshot(float scan, int sampleCount, std::vector<float>* samples) {
  if (!samples) return;
  sampleCount = std::max(sampleCount, 2);
  samples->resize(size_t(sampleCount));
  const iris::ImageWavetable& table = irisBrowserPreviewTable();
  for (int i = 0; i < sampleCount; ++i) {
    (*samples)[size_t(i)] = table.sample(float(i) / float(sampleCount - 1), scan);
  }
}

struct IrisDisplay final : OpaqueWidget {
  Iris* module = nullptr;
  widget::FramebufferWidget* framebuffer = nullptr;
  uint64_t generation = uint64_t(-1);
  NVGcontext* imageContext = nullptr;
  int imageHandle = -1;
  int uploadedWidth = 0;
  int uploadedHeight = 0;
  std::vector<uint8_t> rgba;

  explicit IrisDisplay(Iris* module) : module(module) {}

  ~IrisDisplay() override {
    nvg_gfx_lifecycle::resetOwnedNvgImage(
      imageContext, imageHandle, uploadedWidth, uploadedHeight, nullptr, false);
  }

  void onButton(const event::Button& e) override {
    if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS && module) {
      chooseIrisImage(module);
      e.consume(this);
      return;
    }
    OpaqueWidget::onButton(e);
  }

  void step() override {
    const uint64_t currentGeneration =
      module ? module->previewGeneration.load(std::memory_order_acquire) : 0u;
    if (generation != currentGeneration && framebuffer) {
      framebuffer->setDirty();
    }
    OpaqueWidget::step();
  }

  void draw(const DrawArgs& args) override {
    nvgBeginPath(args.vg);
    nvgRect(args.vg, 0.f, 0.f, box.size.x, box.size.y);
    nvgFillColor(args.vg, nvgRGB(4, 7, 10));
    nvgFill(args.vg);

    const uint64_t currentGeneration =
      module ? module->previewGeneration.load(std::memory_order_acquire) : 0u;
    if (imageContext != args.vg) {
      nvg_gfx_lifecycle::resetOwnedNvgImage(
        imageContext, imageHandle, uploadedWidth, uploadedHeight, args.vg, false);
      imageContext = args.vg;
      generation = uint64_t(-1);
    }
    if (generation != currentGeneration || imageHandle < 0 ||
        !nvg_gfx_lifecycle::ownedNvgImageSizeMatches(args.vg, imageHandle, uploadedWidth, uploadedHeight)) {
      std::vector<uint8_t> gray;
      int width = 0;
      int height = 0;
      if (module) {
        module->previewSnapshot(&gray, &width, &height);
      } else {
        const std::vector<uint8_t>& preview = irisBrowserPreviewPixels();
        gray.assign(preview.begin(), preview.end());
        width = 128;
        height = 64;
      }
      rgba.resize(gray.size() * 4u);
      for (size_t i = 0; i < gray.size(); ++i) {
        const float value = float(gray[i]) / 127.5f - 1.f;
        const float amount = clamp(std::fabs(value), 0.f, 1.f);
        const float red = value < 0.f ? 122.f : 28.f;
        const float green = value < 0.f ? 92.f : 204.f;
        const float blue = value < 0.f ? 255.f : 217.f;
        rgba[i * 4u + 0u] = uint8_t(std::round(red * amount));
        rgba[i * 4u + 1u] = uint8_t(std::round(green * amount));
        rgba[i * 4u + 2u] = uint8_t(std::round(blue * amount));
        rgba[i * 4u + 3u] = 255u;
      }
      nvg_gfx_lifecycle::resetOwnedNvgImage(
        imageContext, imageHandle, uploadedWidth, uploadedHeight, args.vg, imageContext == args.vg);
      imageContext = args.vg;
      if (width > 0 && height > 0 && !rgba.empty()) {
        imageHandle = nvgCreateImageRGBA(args.vg, width, height, NVG_IMAGE_PREMULTIPLIED, rgba.data());
        uploadedWidth = width;
        uploadedHeight = height;
      }
      generation = currentGeneration;
    }
    if (imageHandle >= 0) {
      const float imageTop = std::min(kIrisDisplayVerticalInset, box.size.y * 0.5f);
      const float imageHeight = std::max(0.f, box.size.y - 2.f * imageTop);
      NVGpaint paint =
        nvgImagePattern(args.vg, 0.f, imageTop, box.size.x, imageHeight, 0.f, imageHandle, 1.f);
      nvgBeginPath(args.vg);
      nvgRect(args.vg, 0.f, imageTop, box.size.x, imageHeight);
      nvgFillPaint(args.vg, paint);
      nvgFill(args.vg);
    }

  }
};

struct IrisScanLineOverlay final : TransparentWidget {
  Iris* module = nullptr;

  explicit IrisScanLineOverlay(Iris* module) : module(module) {}

  void onButton(const event::Button& e) override {
    if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS && module) {
      chooseIrisImage(module);
      e.consume(this);
      return;
    }
    TransparentWidget::onButton(e);
  }

  void draw(const DrawArgs& args) override {
    const float scan = module ? clamp(module->displayScan.load(std::memory_order_relaxed), 0.f, 1.f) : 0.62f;
    const float scanTop = std::min(kIrisDisplayVerticalInset, box.size.y * 0.5f);
    const float scanBottom = std::max(scanTop, box.size.y - kIrisDisplayVerticalInset);
    const float scanY = scanTop + scan * (scanBottom - scanTop);
    nvgBeginPath(args.vg);
    nvgMoveTo(args.vg, 0.f, scanY);
    nvgLineTo(args.vg, box.size.x, scanY);
    nvgStrokeWidth(args.vg, 1.2f);
    nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 220));
    nvgStroke(args.vg);
  }
};

struct IrisWaveformPreview final : TransparentWidget {
  Iris* module = nullptr;
  widget::FramebufferWidget* framebuffer = nullptr;
  uint64_t generation = uint64_t(-1);
  float cachedScan = -1.f;
  std::vector<float> waveform;

  explicit IrisWaveformPreview(Iris* module) : module(module) {}

  bool refreshWaveformIfNeeded() {
    const float scan = module ? clamp(module->displayScan.load(std::memory_order_relaxed), 0.f, 1.f) : 0.62f;
    const uint64_t currentGeneration =
      module ? module->previewGeneration.load(std::memory_order_acquire) : 0u;
    if (generation != currentGeneration || std::fabs(scan - cachedScan) >= 1.f / 512.f || waveform.empty()) {
      if (module) {
        module->waveformSnapshot(scan, 256, &waveform);
      } else {
        irisBrowserWaveformSnapshot(scan, 256, &waveform);
      }
      generation = currentGeneration;
      cachedScan = scan;
      return true;
    }
    return false;
  }

  void step() override {
    if (refreshWaveformIfNeeded() && framebuffer) {
      framebuffer->setDirty();
    }
    TransparentWidget::step();
  }

  void draw(const DrawArgs& args) override {
    refreshWaveformIfNeeded();

    const float top = 2.f;
    const float bottom = box.size.y - 2.f;
    const float center = 0.5f * (top + bottom);
    const float left = 2.f;
    const float right = box.size.x - 2.f;

    nvgBeginPath(args.vg);
    nvgMoveTo(args.vg, left, center);
    nvgLineTo(args.vg, right, center);
    nvgStrokeWidth(args.vg, 0.6f);
    nvgStrokeColor(args.vg, nvgRGBA(120, 132, 142, 70));
    nvgStroke(args.vg);

    if (!waveform.empty()) {
      nvgBeginPath(args.vg);
      for (size_t i = 0; i < waveform.size(); ++i) {
        const float x = left + (right - left) * float(i) / float(waveform.size() - 1u);
        const float y = center - clamp(waveform[i], -1.f, 1.f) * 0.5f * (bottom - top);
        if (i == 0u) nvgMoveTo(args.vg, x, y);
        else nvgLineTo(args.vg, x, y);
      }
      nvgSave(args.vg);
      nvgScissor(args.vg, 0.f, 0.f, box.size.x, center);
      nvgStrokeWidth(args.vg, 1.45f);
      nvgStrokeColor(args.vg, nvgRGBA(28, 204, 217, 245));
      nvgStroke(args.vg);
      nvgRestore(args.vg);

      nvgBeginPath(args.vg);
      for (size_t i = 0; i < waveform.size(); ++i) {
        const float x = left + (right - left) * float(i) / float(waveform.size() - 1u);
        const float y = center - clamp(waveform[i], -1.f, 1.f) * 0.5f * (bottom - top);
        if (i == 0u) nvgMoveTo(args.vg, x, y);
        else nvgLineTo(args.vg, x, y);
      }
      nvgSave(args.vg);
      nvgScissor(args.vg, 0.f, center, box.size.x, box.size.y - center);
      nvgStrokeWidth(args.vg, 1.45f);
      nvgStrokeColor(args.vg, nvgRGBA(122, 92, 255, 245));
      nvgStroke(args.vg);
      nvgRestore(args.vg);
    }
  }
};

struct IrisFrequencyReadout final : TransparentWidget {
  static constexpr float LABEL_FONT_SIZE = 11.5f;
  Iris* module = nullptr;

  explicit IrisFrequencyReadout(Iris* module) : module(module) {}

  static std::string formatFrequencyText(float hz) {
    if (!std::isfinite(hz) || hz < 0.f) hz = 0.f;
    if (hz < 1.f) return string::f("%.1f mHz", hz * 1000.f);
    if (hz >= 1000.f) return string::f("%.2f kHz", hz / 1000.f);
    if (hz < 10.f) return string::f("%.2f Hz", hz);
    if (hz < 100.f) return string::f("%.1f Hz", hz);
    return string::f("%.0f Hz", hz);
  }

  void draw(const DrawArgs& args) override {
    float displayHz = dsp::FREQ_C4;
    if (module) {
      displayHz = module->displayFrequencyHz.load(std::memory_order_relaxed);
      if (displayHz <= 0.f) {
        displayHz = irisBaseFrequencyFromKnob(module->params[Iris::COARSE_PARAM].getValue());
      }
    }
    const std::string text = formatFrequencyText(displayHz);
    nvgFontSize(args.vg, LABEL_FONT_SIZE);
    nvgFontFaceId(args.vg, APP->window->uiFont->handle);
    nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 255));
    nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_TOP);
    nvgText(args.vg, box.size.x * 0.5f, 1.5f, text.c_str(), nullptr);
  }
};

template <typename EnumType>
void addEnumMenu(Menu* menu, const char* label, Iris* module, EnumType* setting,
                 const std::vector<std::pair<std::string, EnumType> >& choices) {
  menu->addChild(createSubmenuItem(label, "", [module, setting, choices](Menu* submenu) {
    for (size_t i = 0; i < choices.size(); ++i) {
      const EnumType value = choices[i].second;
      submenu->addChild(createCheckMenuItem(
        choices[i].first, "", [setting, value]() { return *setting == value; },
        [module, setting, value]() {
          *setting = value;
          module->requestRebuild();
        }));
    }
  }));
}

} // namespace

struct IrisWidget final : ModuleWidget {
  debug_terminal::BaselineWidgetMetrics debugWidgetMetrics;

  explicit IrisWidget(Iris* module) {
    setModule(module);
    const std::string panelPath = asset::plugin(pluginInstance, "res/iris.panel.svg");
    setPanel(createPanel(panelPath));
    addChild(visual_assets::createPanelSurfaceEffectWidget(panelPath, box.size));
    {
      widget::SvgWidget* labels = new widget::SvgWidget();
      labels->setSvg(visual_assets::loadPluginSvgCached("res/iris.labels.svg"));
      labels->box.size = box.size;

      widget::FramebufferWidget* labelsFb = new widget::FramebufferWidget();
      labelsFb->box.size = box.size;
      labelsFb->oversample = 2.0f;
      labelsFb->dirtyOnSubpixelChange = true;
      labelsFb->addChild(labels);
      addChild(labelsFb);
    }
    addChild(createWidget<CyanOrbScrew>(Vec(0.f, 0.f)));
    addChild(createWidget<CyanOrbScrew>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

    math::Rect displayRectMm(Vec(4.3f, 13.2f), Vec(52.36f, 25.9f));
    panel_svg::loadRectFromSvgMm(panelPath, "IRIS_DISPLAY", &displayRectMm);
    widget::FramebufferWidget* displayFb = new widget::FramebufferWidget();
    displayFb->box.pos = mm2px(displayRectMm.pos);
    displayFb->box.size = mm2px(displayRectMm.size);
    displayFb->dirtyOnSubpixelChange = false;
    IrisDisplay* display = new IrisDisplay(module);
    display->box.size = displayFb->box.size;
    display->framebuffer = displayFb;
    displayFb->addChild(display);
    addChild(displayFb);
    IrisScanLineOverlay* scanLine = new IrisScanLineOverlay(module);
    scanLine->box.pos = mm2px(displayRectMm.pos);
    scanLine->box.size = mm2px(displayRectMm.size);
    addChild(scanLine);
    addChild(visual_assets::createPreviewFrameEnhancementWidget(
      displayRectMm, visual_assets::PreviewFrameTint::Purple));

    math::Rect waveformRectMm(Vec(4.3f, 42.f), Vec(52.36f, 18.f));
    panel_svg::loadRectFromSvgMm(panelPath, "IRIS_WAVE_PREVIEW", &waveformRectMm);
    addChild(visual_assets::createPreviewFrameEnhancementWidget(waveformRectMm));
    math::Rect waveformContentRectMm = waveformRectMm;
    waveformContentRectMm.pos = waveformContentRectMm.pos.plus(Vec(0.2f, 0.2f));
    waveformContentRectMm.size = waveformContentRectMm.size.minus(Vec(0.4f, 0.4f));
    widget::FramebufferWidget* waveformFb = new widget::FramebufferWidget();
    waveformFb->box.pos = mm2px(waveformContentRectMm.pos);
    waveformFb->box.size = mm2px(waveformContentRectMm.size);
    waveformFb->dirtyOnSubpixelChange = false;
    IrisWaveformPreview* waveformPreview = new IrisWaveformPreview(module);
    waveformPreview->box.size = waveformFb->box.size;
    waveformPreview->framebuffer = waveformFb;
    waveformFb->addChild(waveformPreview);
    addChild(waveformFb);
    IrisFrequencyReadout* frequencyReadout = new IrisFrequencyReadout(module);
    frequencyReadout->box.pos = mm2px(Vec(
      waveformRectMm.pos.x, waveformRectMm.pos.y + waveformRectMm.size.y));
    frequencyReadout->box.size = mm2px(Vec(waveformRectMm.size.x, 5.f));
    addChild(frequencyReadout);

    auto anchor = [&](const char* id, const Vec& fallbackMm) {
      Vec posMm = fallbackMm;
      panel_svg::loadPointFromSvgMm(panelPath, id, &posMm);
      return posMm;
    };

    addParam(createParamCentered<LeviathanHaloKnob2>(
      mm2px(anchor("IRIS_COARSE_PARAM", Vec(13.5f, 54.f))), module, Iris::COARSE_PARAM));
    addParam(createParamCentered<BipolarTinyClockworkGearKnob>(
      mm2px(anchor("IRIS_FINE_PARAM", Vec(30.48f, 54.f))), module, Iris::FINE_PARAM));
    addParam(createParamCentered<LeviathanHaloKnob2>(
      mm2px(anchor("IRIS_SCAN_PARAM", Vec(47.46f, 54.f))), module, Iris::SCAN_PARAM));
    addParam(createParamCentered<Eclipse2Knob>(
      mm2px(anchor("IRIS_FM_ATTEN_PARAM", Vec(13.5f, 74.f))), module, Iris::LIN_FM_PARAM));
    Eclipse2Knob* scanAtten = createParamCentered<Eclipse2Knob>(
      mm2px(anchor("IRIS_SCAN_ATTEN_PARAM", Vec(30.48f, 74.f))), module, Iris::SCAN_ATTEN_PARAM);
    scanAtten->setProgressRingBipolar(true);
    addParam(scanAtten);
    SmallGoldApertureButton* octaveStep = createLightParamCentered<SmallGoldApertureButton>(
      mm2px(anchor("IRIS_COARSE_STEP_MODE_PARAM", Vec(30.48f, 84.f))),
      module, Iris::COARSE_STEP_MODE_PARAM, Iris::COARSE_STEP_MODE_LIGHT);
    addParam(octaveStep);
    SmallGoldApertureButton* softSync = createLightParamCentered<SmallGoldApertureButton>(
      mm2px(anchor("IRIS_SOFT_SYNC_MODE_PARAM", Vec(44.64f, 76.73f))),
      module, Iris::SOFT_SYNC_MODE_PARAM, Iris::SOFT_SYNC_MODE_LIGHT);
    addParam(softSync);

    addInput(createInputCentered<Magitek2InputJack>(
      mm2px(anchor("IRIS_V_OCT_INPUT", Vec(8.5f, 99.f))), module, Iris::V_OCT_INPUT));
    addInput(createInputCentered<Magitek2InputJack>(
      mm2px(anchor("IRIS_FM_INPUT", Vec(23.f, 99.f))), module, Iris::LIN_FM_INPUT));
    addInput(createInputCentered<Magitek2InputJack>(
      mm2px(anchor("IRIS_SCAN_INPUT", Vec(37.96f, 99.f))), module, Iris::SCAN_INPUT));
    addInput(createInputCentered<Magitek2InputJack>(
      mm2px(anchor("IRIS_SYNC_INPUT", Vec(52.46f, 99.f))), module, Iris::SYNC_INPUT));
    addOutput(createOutputCentered<Magitek2OutputJack>(
      mm2px(anchor("IRIS_OUT_OUTPUT", Vec(19.f, 118.f))), module, Iris::OUT_OUTPUT));
    addOutput(createOutputCentered<Magitek2OutputJack>(
      mm2px(anchor("IRIS_INV_OUTPUT", Vec(41.96f, 118.f))), module, Iris::INV_OUTPUT));
  }

  void step() override {
    const bool measurePerf = isDragonKingDebugEnabled();
    const auto stepStart = debug_terminal::debugTimerStart(measurePerf);
    ModuleWidget::step();
    if (measurePerf) {
      debugWidgetMetrics.recordStep(debug_terminal::elapsedUsSince(stepStart));
    }
  }

  void draw(const DrawArgs& args) override {
    const bool measurePerf = isDragonKingDebugEnabled();
    const auto drawStart = debug_terminal::debugTimerStart(measurePerf);
    ModuleWidget::draw(args);
    Iris* irisModule = static_cast<Iris*>(module);
    if (!irisModule) return;

    if (isDragonKingDebugEnabled()) {
      debug_terminal::drawDebugInstanceId(args.vg, box.size, irisModule->debugMetrics.instanceId);
    }

    if (measurePerf) {
      debugWidgetMetrics.recordDraw(debug_terminal::elapsedUsSince(drawStart));

      const double nowSec = system::getTime();
      if (debug_terminal::baselineSubmitDue("Iris", irisModule->debugMetrics.instanceId, nowSec)) {
        debug_terminal::submitBaselineMetrics(
          "Iris",
          irisModule->debugMetrics.instanceId,
          irisModule->debugMetrics.consumeProcessRange(),
          debugWidgetMetrics.consumeStepRange(),
          debugWidgetMetrics.consumeDrawRange());
      }
    }
  }

  void onPathDrop(const event::PathDrop& e) override {
    Iris* irisModule = static_cast<Iris*>(module);
    if (irisModule) {
      for (size_t i = 0; i < e.paths.size(); ++i) {
        if (system::isFile(e.paths[i]) && supportedImagePath(e.paths[i])) {
          irisModule->requestImageLoad(e.paths[i]);
          e.consume(this);
          return;
        }
      }
    }
    ModuleWidget::onPathDrop(e);
  }

  void appendContextMenu(Menu* menu) override {
    ModuleWidget::appendContextMenu(menu);
    Iris* irisModule = dynamic_cast<Iris*>(module);
    if (!irisModule) return;
    menu->addChild(new MenuSeparator());
    menu->addChild(createMenuItem("Load image...", "", [irisModule]() { chooseIrisImage(irisModule); }));
    menu->addChild(createMenuItem("Reload image", "", [irisModule]() { irisModule->requestReload(); },
                                  irisModule->sourcePath().empty()));
    menu->addChild(createMenuItem("Clear image", "", [irisModule]() { irisModule->clearToDefault(); }));
    menu->addChild(new MenuSeparator());
    addEnumMenu(menu, "Normalize", irisModule, &irisModule->conversionSettings.normalizeMode,
      {{"Balanced", iris::NORMALIZE_BALANCED}, {"Global", iris::NORMALIZE_GLOBAL},
       {"Per row", iris::NORMALIZE_PER_ROW}, {"Off", iris::NORMALIZE_NONE}});
    addEnumMenu(menu, "Row order", irisModule, &irisModule->conversionSettings.rowOrder,
      {{"Top to bottom", iris::ROW_TOP_TO_BOTTOM}, {"Bottom to top", iris::ROW_BOTTOM_TO_TOP}});
    addEnumMenu(menu, "Trim flat rows", irisModule, &irisModule->conversionSettings.trimMode,
      {{"Off", iris::TRIM_OFF}, {"Gentle", iris::TRIM_GENTLE}, {"Medium", iris::TRIM_MEDIUM},
       {"Aggressive", iris::TRIM_AGGRESSIVE}});
    addEnumMenu(menu, "Seam smoothing", irisModule, &irisModule->conversionSettings.seamMode,
      {{"Off", iris::SEAM_OFF}, {"Small", iris::SEAM_SMALL}, {"Medium", iris::SEAM_MEDIUM},
       {"Large", iris::SEAM_LARGE}});
    addEnumMenu(menu, "Wave smoothing", irisModule, &irisModule->conversionSettings.smoothingMode,
      {{"Off", iris::SMOOTH_OFF}, {"Gentle", iris::SMOOTH_GENTLE}, {"Medium", iris::SMOOTH_MEDIUM},
       {"Strong", iris::SMOOTH_STRONG}});
    menu->addChild(createCheckMenuItem("Per-row DC removal", "",
      [irisModule]() { return irisModule->conversionSettings.dcRemove; },
      [irisModule]() {
        irisModule->conversionSettings.dcRemove = !irisModule->conversionSettings.dcRemove;
        irisModule->requestRebuild();
      }));
    menu->addChild(createCheckMenuItem("Invert conversion", "",
      [irisModule]() { return irisModule->conversionSettings.invert; },
      [irisModule]() {
        irisModule->conversionSettings.invert = !irisModule->conversionSettings.invert;
        irisModule->requestRebuild();
      }));
    menu->addChild(new MenuSeparator());
    menu->addChild(createCheckMenuItem("Embed wavetable in patch", "",
      [irisModule]() { return irisModule->embedsTable(); },
      [irisModule]() { irisModule->setEmbedTable(!irisModule->embedsTable()); }));
  }
};

Model* modelIris = createModel<Iris, IrisWidget>("Iris");
