#include "Iris.hpp"
#include "NvgGraphicsLifecycle.hpp"
#include "visual/VisualAssets.hpp"

#include <cctype>
#include <cstdlib>
#include <osdialog.h>

namespace {

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

struct IrisDisplay final : OpaqueWidget {
  Iris* module = nullptr;
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

  void draw(const DrawArgs& args) override {
    nvgBeginPath(args.vg);
    nvgRect(args.vg, 0.f, 0.f, box.size.x, box.size.y);
    nvgFillColor(args.vg, nvgRGB(4, 7, 10));
    nvgFill(args.vg);
    if (!module) return;

    const uint64_t currentGeneration = module->previewGeneration.load(std::memory_order_acquire);
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
      module->previewSnapshot(&gray, &width, &height);
      rgba.resize(gray.size() * 4u);
      for (size_t i = 0; i < gray.size(); ++i) {
        const float t = float(gray[i]) / 255.f;
        rgba[i * 4u + 0u] = uint8_t(30.f + 200.f * t);
        rgba[i * 4u + 1u] = uint8_t(28.f + 190.f * (1.f - std::fabs(2.f * t - 1.f)));
        rgba[i * 4u + 2u] = uint8_t(62.f + 190.f * (1.f - t));
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
      NVGpaint paint = nvgImagePattern(args.vg, 0.f, 0.f, box.size.x, box.size.y, 0.f, imageHandle, 1.f);
      nvgBeginPath(args.vg);
      nvgRect(args.vg, 0.f, 0.f, box.size.x, box.size.y);
      nvgFillPaint(args.vg, paint);
      nvgFill(args.vg);
    }

    const float scanY = clamp(module->displayScan.load(std::memory_order_relaxed), 0.f, 1.f) * box.size.y;
    nvgBeginPath(args.vg);
    nvgMoveTo(args.vg, 0.f, scanY);
    nvgLineTo(args.vg, box.size.x, scanY);
    nvgStrokeWidth(args.vg, 1.2f);
    nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 220));
    nvgStroke(args.vg);

    const std::string status = module->statusText();
    std::string name = module->sourceName();
    if (name.size() > 30u) name = name.substr(0u, 27u) + "...";
    nvgFontFaceId(args.vg, APP->window->uiFont->handle);
    nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_BOTTOM);
    nvgFontSize(args.vg, 9.f);
    nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 235));
    nvgText(args.vg, 5.f, box.size.y - 5.f, status.c_str(), nullptr);
    nvgTextAlign(args.vg, NVG_ALIGN_RIGHT | NVG_ALIGN_BOTTOM);
    nvgFillColor(args.vg, nvgRGBA(225, 232, 238, 205));
    nvgText(args.vg, box.size.x - 5.f, box.size.y - 5.f, name.c_str(), nullptr);
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
  explicit IrisWidget(Iris* module) {
    setModule(module);
    const std::string panelPath = asset::plugin(pluginInstance, "res/iris.panel.svg");
    setPanel(createPanel(panelPath));
    addChild(visual_assets::createPanelSurfaceEffectWidget(panelPath, box.size));
    addChild(createWidget<CyanOrbScrew>(Vec(0.f, 0.f)));
    addChild(createWidget<CyanOrbScrew>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

    IrisDisplay* display = new IrisDisplay(module);
    display->box.pos = mm2px(Vec(4.3f, 13.2f));
    display->box.size = mm2px(Vec(52.36f, 25.9f));
    addChild(display);

    addParam(createParamCentered<LeviathanHaloKnob2>(mm2px(Vec(13.5f, 54.f)), module, Iris::COARSE_PARAM));
    addParam(createParamCentered<BipolarTinyClockworkGearKnob>(mm2px(Vec(30.48f, 54.f)), module, Iris::FINE_PARAM));
    addParam(createParamCentered<LeviathanHaloKnob2>(mm2px(Vec(47.46f, 54.f)), module, Iris::SCAN_PARAM));
    addParam(createParamCentered<BipolarTinyClockworkGearKnob>(mm2px(Vec(13.5f, 74.f)), module, Iris::FM_ATTEN_PARAM));
    addParam(createParamCentered<BipolarTinyClockworkGearKnob>(mm2px(Vec(30.48f, 74.f)), module, Iris::SCAN_ATTEN_PARAM));
    addParam(createParamCentered<Eclipse2Knob>(mm2px(Vec(47.46f, 74.f)), module, Iris::LEVEL_PARAM));
    SmallGoldApertureButton* quant = createLightParamCentered<SmallGoldApertureButton>(
      mm2px(Vec(30.48f, 84.f)), module, Iris::QUANT_PARAM, Iris::QUANT_LIGHT);
    addParam(quant);

    addInput(createInputCentered<Magitek2InputJack>(mm2px(Vec(8.5f, 99.f)), module, Iris::V_OCT_INPUT));
    addInput(createInputCentered<Magitek2InputJack>(mm2px(Vec(23.f, 99.f)), module, Iris::FM_INPUT));
    addInput(createInputCentered<Magitek2InputJack>(mm2px(Vec(37.96f, 99.f)), module, Iris::SCAN_INPUT));
    addInput(createInputCentered<Magitek2InputJack>(mm2px(Vec(52.46f, 99.f)), module, Iris::SYNC_INPUT));
    addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(Vec(19.f, 118.f)), module, Iris::OUT_OUTPUT));
    addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(Vec(41.96f, 118.f)), module, Iris::INV_OUTPUT));
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
    menu->addChild(createMenuItem("Clear image", "", [irisModule]() { irisModule->clearToSine(); }));
    menu->addChild(new MenuSeparator());
    addEnumMenu(menu, "Normalize", irisModule, &irisModule->conversionSettings.normalizeMode,
      {{"Off", iris::NORMALIZE_NONE}, {"Global", iris::NORMALIZE_GLOBAL}, {"Per row", iris::NORMALIZE_PER_ROW}});
    addEnumMenu(menu, "Row order", irisModule, &irisModule->conversionSettings.rowOrder,
      {{"Top to bottom", iris::ROW_TOP_TO_BOTTOM}, {"Bottom to top", iris::ROW_BOTTOM_TO_TOP}});
    addEnumMenu(menu, "Trim flat rows", irisModule, &irisModule->conversionSettings.trimMode,
      {{"Off", iris::TRIM_OFF}, {"Gentle", iris::TRIM_GENTLE}, {"Medium", iris::TRIM_MEDIUM},
       {"Aggressive", iris::TRIM_AGGRESSIVE}});
    addEnumMenu(menu, "Seam smoothing", irisModule, &irisModule->conversionSettings.seamMode,
      {{"Off", iris::SEAM_OFF}, {"Small", iris::SEAM_SMALL}, {"Medium", iris::SEAM_MEDIUM},
       {"Large", iris::SEAM_LARGE}});
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
