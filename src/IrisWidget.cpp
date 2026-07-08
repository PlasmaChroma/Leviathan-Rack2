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
    const int width = iris::kSourcePreviewWidth;
    const int height = iris::kSourcePreviewHeight;
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

NVGcolor irisPreviewChannelColor(int mode, float value) {
  const float amount = clamp(std::fabs(value), 0.f, 1.f);
  const uint8_t alpha = 255u;
  switch (mode) {
    case iris::IMAGE_CHANNEL_RED:
      return nvgRGBA(
        uint8_t(std::round(245.f * amount)),
        uint8_t(std::round(32.f * amount)),
        uint8_t(std::round(52.f * amount)),
        alpha);
    case iris::IMAGE_CHANNEL_GREEN:
      return nvgRGBA(
        uint8_t(std::round(36.f * amount)),
        uint8_t(std::round(230.f * amount)),
        uint8_t(std::round(112.f * amount)),
        alpha);
    case iris::IMAGE_CHANNEL_BLUE:
      return nvgRGBA(
        uint8_t(std::round(54.f * amount)),
        uint8_t(std::round(126.f * amount)),
        uint8_t(std::round(255.f * amount)),
        alpha);
    case iris::IMAGE_CHANNEL_ALL:
    default:
      return nvgRGBA(
        uint8_t(std::round(235.f * amount)),
        uint8_t(std::round(240.f * amount)),
        uint8_t(std::round(246.f * amount)),
        alpha);
  }
}

NVGcolor irisPreviewConvertedColor(float value) {
  const float amount = clamp(std::fabs(value), 0.f, 1.f);
  const float red = value < 0.f ? 122.f : 28.f;
  const float green = value < 0.f ? 92.f : 204.f;
  const float blue = value < 0.f ? 255.f : 217.f;
  return nvgRGBA(
    uint8_t(std::round(red * amount)),
    uint8_t(std::round(green * amount)),
    uint8_t(std::round(blue * amount)),
    255);
}

void filterIrisSourcePreview(std::vector<uint8_t>* rgb, int mode) {
  if (!rgb || rgb->empty()) return;
  for (size_t i = 0; i + 2u < rgb->size(); i += 3u) {
    switch (mode) {
      case iris::IMAGE_CHANNEL_RED:
        (*rgb)[i + 1u] = 0u;
        (*rgb)[i + 2u] = 0u;
        break;
      case iris::IMAGE_CHANNEL_GREEN:
        (*rgb)[i] = 0u;
        (*rgb)[i + 2u] = 0u;
        break;
      case iris::IMAGE_CHANNEL_BLUE:
        (*rgb)[i] = 0u;
        (*rgb)[i + 1u] = 0u;
        break;
      case iris::IMAGE_CHANNEL_ALL:
      default:
        break;
    }
  }
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
  bool channelPreview = false;
  int channelMode = iris::IMAGE_CHANNEL_ALL;
  NVGcontext* imageContext = nullptr;
  int imageHandle = -1;
  int uploadedWidth = 0;
  int uploadedHeight = 0;
  std::vector<uint8_t> rgba;

  explicit IrisDisplay(Iris* module) : module(module) {}

  ~IrisDisplay() override {
    if (APP && APP->window && APP->window->vg) {
      nvg_gfx_lifecycle::resetOwnedNvgImage(
        imageContext, imageHandle, uploadedWidth, uploadedHeight, APP->window->vg, true);
      return;
    }
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
    const bool currentChannelPreview =
      module ? module->displayChannelPreview.load(std::memory_order_relaxed) : false;
    const int currentChannelMode =
      module ? clamp(module->displayImageChannelMode.load(std::memory_order_relaxed), 0, 3)
             : iris::IMAGE_CHANNEL_ALL;
    if ((generation != currentGeneration || channelPreview != currentChannelPreview ||
         channelMode != currentChannelMode) && framebuffer) {
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
    const bool currentChannelPreview =
      module ? module->displayChannelPreview.load(std::memory_order_relaxed) : false;
    const int currentChannelMode =
      module ? clamp(module->displayImageChannelMode.load(std::memory_order_relaxed), 0, 3)
             : iris::IMAGE_CHANNEL_ALL;
    if (imageContext != args.vg) {
      nvg_gfx_lifecycle::resetOwnedNvgImage(
        imageContext, imageHandle, uploadedWidth, uploadedHeight, args.vg, false);
      imageContext = args.vg;
      generation = uint64_t(-1);
    }
    if (generation != currentGeneration || channelPreview != currentChannelPreview ||
        channelMode != currentChannelMode || imageHandle < 0 ||
        !nvg_gfx_lifecycle::ownedNvgImageSizeMatches(args.vg, imageHandle, uploadedWidth, uploadedHeight)) {
      std::vector<uint8_t> gray;
      std::vector<uint8_t> rgb;
      int width = 0;
      int height = 0;
      if (module) {
        if (currentChannelPreview) {
          module->sourcePreviewSnapshot(&rgb, &width, &height);
        }
        if (!currentChannelPreview || rgb.empty()) {
          module->previewSnapshot(&gray, &width, &height);
        }
      } else {
        const std::vector<uint8_t>& preview = irisBrowserPreviewPixels();
        gray.assign(preview.begin(), preview.end());
        width = iris::kSourcePreviewWidth;
        height = iris::kSourcePreviewHeight;
      }
      if (currentChannelPreview && !rgb.empty()) {
        filterIrisSourcePreview(&rgb, currentChannelMode);
        const size_t pixelCount = rgb.size() / 3u;
        rgba.resize(pixelCount * 4u);
        for (size_t i = 0; i < pixelCount; ++i) {
          rgba[i * 4u + 0u] = rgb[i * 3u + 0u];
          rgba[i * 4u + 1u] = rgb[i * 3u + 1u];
          rgba[i * 4u + 2u] = rgb[i * 3u + 2u];
          rgba[i * 4u + 3u] = 255u;
        }
      } else {
        rgba.resize(gray.size() * 4u);
        for (size_t i = 0; i < gray.size(); ++i) {
          const float value = float(gray[i]) / 127.5f - 1.f;
          const NVGcolor color = currentChannelPreview
            ? irisPreviewChannelColor(currentChannelMode, value)
            : irisPreviewConvertedColor(value);
          rgba[i * 4u + 0u] = uint8_t(std::round(clamp(color.r, 0.f, 1.f) * 255.f));
          rgba[i * 4u + 1u] = uint8_t(std::round(clamp(color.g, 0.f, 1.f) * 255.f));
          rgba[i * 4u + 2u] = uint8_t(std::round(clamp(color.b, 0.f, 1.f) * 255.f));
          rgba[i * 4u + 3u] = uint8_t(std::round(clamp(color.a, 0.f, 1.f) * 255.f));
        }
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
      channelPreview = currentChannelPreview;
      channelMode = currentChannelMode;
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

struct IrisSmoothingMenuQuantity final : Quantity {
  Iris* module = nullptr;
  float* setting = nullptr;
  const char* label = "";

  IrisSmoothingMenuQuantity(Iris* module, float* setting, const char* label)
    : module(module), setting(setting), label(label) {}

  void setValue(float value) override {
    if (!module || !setting) return;
    const float next = clamp(value, 0.f, 1.f);
    if (std::fabs(*setting - next) > 1e-5f) {
      *setting = next;
      module->requestRebuild();
    }
  }

  float getValue() override {
    return setting ? float(*setting) : 0.f;
  }

  float getDefaultValue() override {
    return 0.f;
  }

  float getMinValue() override {
    return 0.f;
  }

  float getMaxValue() override {
    return 1.f;
  }

  std::string getLabel() override {
    return label;
  }

  float getDisplayValue() override {
    return getValue() * 100.f;
  }

  void setDisplayValue(float value) override {
    setValue(value * 0.01f);
  }

  std::string getDisplayValueString() override {
    return string::f("%.0f%%", getDisplayValue());
  }
};

struct IrisSmoothingMenuButton final : TL1105 {
  Iris* module = nullptr;

  void onButton(const event::Button& e) override {
    if (!module || e.button != GLFW_MOUSE_BUTTON_LEFT || e.action != GLFW_PRESS) {
      TL1105::onButton(e);
      return;
    }
    ui::Menu* menu = createMenu();
    menu->box.pos = getAbsoluteOffset(Vec(0.f, box.size.y));
    menu->addChild(createMenuLabel("Smoothing"));
    ui::Slider* seamSlider = new ui::Slider();
    seamSlider->box.size = Vec(180.f, 24.f);
    seamSlider->quantity = new IrisSmoothingMenuQuantity(
      module, &module->conversionSettings.seamSmoothing, "Seam smoothing");
    menu->addChild(seamSlider);
    ui::Slider* waveSlider = new ui::Slider();
    waveSlider->box.size = Vec(180.f, 24.f);
    waveSlider->quantity = new IrisSmoothingMenuQuantity(
      module, &module->conversionSettings.waveSmoothing, "Wave smoothing");
    menu->addChild(waveSlider);
    addEnumMenu(menu, "Normalize", module, &module->conversionSettings.normalizeMode,
      {{"Balanced", iris::NORMALIZE_BALANCED}, {"Global", iris::NORMALIZE_GLOBAL},
       {"Per row", iris::NORMALIZE_PER_ROW}, {"Off", iris::NORMALIZE_NONE}});
    addEnumMenu(menu, "Row order", module, &module->conversionSettings.rowOrder,
      {{"Top to bottom", iris::ROW_TOP_TO_BOTTOM}, {"Bottom to top", iris::ROW_BOTTOM_TO_TOP}});
    addEnumMenu(menu, "Trim flat rows", module, &module->conversionSettings.trimMode,
      {{"Off", iris::TRIM_OFF}, {"Gentle", iris::TRIM_GENTLE}, {"Medium", iris::TRIM_MEDIUM},
       {"Aggressive", iris::TRIM_AGGRESSIVE}});
    menu->addChild(createCheckMenuItem("Per-row DC removal", "",
      [this]() { return module->conversionSettings.dcRemove; },
      [this]() {
        module->conversionSettings.dcRemove = !module->conversionSettings.dcRemove;
        module->requestRebuild();
      }));
    menu->addChild(createCheckMenuItem("Invert conversion", "",
      [this]() { return module->conversionSettings.invert; },
      [this]() {
        module->conversionSettings.invert = !module->conversionSettings.invert;
        module->requestRebuild();
      }));
    e.consume(this);
  }

  void draw(const DrawArgs& args) override {
    TL1105::draw(args);
    const float cx = 0.5f * box.size.x;
    const float cy = 0.5f * box.size.y;
    const float halfW = std::max(2.4f, 0.28f * box.size.x);
    const float amplitude = std::max(1.6f, 0.17f * box.size.y);
    const float left = cx - halfW;
    const float right = cx + halfW;
    const float quarterW = 0.5f * halfW;
    nvgBeginPath(args.vg);
    nvgMoveTo(args.vg, left, cy);
    nvgBezierTo(
      args.vg, left + quarterW * 0.55f, cy - amplitude,
      cx - quarterW * 0.55f, cy - amplitude, cx, cy);
    nvgBezierTo(
      args.vg, cx + quarterW * 0.55f, cy + amplitude,
      right - quarterW * 0.55f, cy + amplitude, right, cy);
    nvgStrokeWidth(args.vg, 1.2f);
    nvgStrokeColor(args.vg, nvgRGBA(225, 232, 240, 244));
    nvgLineCap(args.vg, NVG_ROUND);
    nvgStroke(args.vg);
  }
};

struct IrisChannelPreviewButton final : TL1105 {
  Iris* module = nullptr;
  ui::Tooltip* tooltip = nullptr;
  int pressedFrames = 0;

  ~IrisChannelPreviewButton() {
    destroyTooltip();
  }

  std::string tooltipText() const {
    const bool enabled = module && module->displayChannelPreview.load(std::memory_order_relaxed);
    return enabled ? "Show converted waveform field" : "Show source color channels";
  }

  void createTooltip() {
    if (settings::tooltips && !tooltip) {
      tooltip = new ui::Tooltip();
      tooltip->text = tooltipText();
      APP->scene->addChild(tooltip);
    }
  }

  void destroyTooltip() {
    if (tooltip) {
      APP->scene->removeChild(tooltip);
      delete tooltip;
      tooltip = nullptr;
    }
  }

  void refreshTooltip() {
    if (tooltip) tooltip->text = tooltipText();
  }

  void setPressedVisual(bool pressed) {
    if (!sw || frames.empty()) return;
    const size_t frameIndex = pressed && frames.size() > 1u ? 1u : 0u;
    sw->setSvg(frames[frameIndex]);
    if (fb) fb->setDirty();
  }

  void onButton(const event::Button& e) override {
    if (module && e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
      const bool current = module->displayChannelPreview.load(std::memory_order_relaxed);
      const bool next = !current;
      module->displayChannelPreview.store(next, std::memory_order_relaxed);
      pressedFrames = 5;
      setPressedVisual(true);
      refreshTooltip();
      e.consume(this);
      return;
    }
    TL1105::onButton(e);
  }

  void onEnter(const event::Enter& e) override {
    TL1105::onEnter(e);
    createTooltip();
  }

  void onLeave(const event::Leave& e) override {
    TL1105::onLeave(e);
    destroyTooltip();
  }

  void step() override {
    if (pressedFrames > 0) {
      --pressedFrames;
      if (pressedFrames == 0) setPressedVisual(false);
    }
    TL1105::step();
  }

  void draw(const DrawArgs& args) override {
    TL1105::draw(args);
    const bool enabled = module && module->displayChannelPreview.load(std::memory_order_relaxed);
    const int mode = module
      ? clamp(module->displayImageChannelMode.load(std::memory_order_relaxed), 0, 3)
      : iris::IMAGE_CHANNEL_ALL;
    const float cx = 0.5f * box.size.x;
    const float cy = 0.5f * box.size.y;
    const float rx = std::max(3.1f, 0.31f * box.size.x);
    const float ry = std::max(1.9f, 0.19f * box.size.y);
    const NVGcolor stroke = enabled
      ? irisPreviewChannelColor(mode, 1.f)
      : nvgRGBA(225, 232, 240, 244);

    nvgBeginPath(args.vg);
    nvgMoveTo(args.vg, cx - rx, cy);
    nvgBezierTo(args.vg, cx - rx * 0.58f, cy - ry, cx - rx * 0.24f, cy - ry, cx, cy - ry);
    nvgBezierTo(args.vg, cx + rx * 0.24f, cy - ry, cx + rx * 0.58f, cy - ry, cx + rx, cy);
    nvgBezierTo(args.vg, cx + rx * 0.58f, cy + ry, cx + rx * 0.24f, cy + ry, cx, cy + ry);
    nvgBezierTo(args.vg, cx - rx * 0.24f, cy + ry, cx - rx * 0.58f, cy + ry, cx - rx, cy);
    nvgStrokeWidth(args.vg, 1.05f);
    nvgStrokeColor(args.vg, stroke);
    nvgLineCap(args.vg, NVG_ROUND);
    nvgLineJoin(args.vg, NVG_ROUND);
    nvgStroke(args.vg);

    nvgBeginPath(args.vg);
    nvgCircle(args.vg, cx, cy, std::max(1.15f, 0.095f * box.size.x));
    nvgFillColor(args.vg, stroke);
    nvgFill(args.vg);
  }
};

struct IrisImageChannelButton final : SmallGoldButton {
  Iris* module = nullptr;

  void onButton(const event::Button& e) override {
    if (module && e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
      const int current = clamp(int(module->conversionSettings.imageChannelMode), 0, 3);
      module->conversionSettings.imageChannelMode = iris::ImageChannelMode((current + 1) % 4);
      module->displayImageChannelMode.store(
        int(module->conversionSettings.imageChannelMode), std::memory_order_relaxed);
      module->requestRebuild();
    }
    SmallGoldButton::onButton(e);
  }
};

} // namespace

struct IrisWidget final : ModuleWidget {
  debug_terminal::BaselineWidgetMetrics debugWidgetMetrics;

  explicit IrisWidget(Iris* module) {
    setModule(module);
    const std::string panelPath = asset::plugin(pluginInstance, "res/iris.panel.svg");
    setPanel(createPanel(panelPath));
    addChild(visual_assets::createPanelSurfaceEffectWidget(panelPath, box.size));
    addChild(visual_assets::createPanelLabelsWidget("res/iris.labels.svg", box.size));
    addChild(createWidget<CyanOrbScrew>(Vec(RACK_GRID_WIDTH, 0.f)));
    addChild(createWidget<CyanOrbScrew>(Vec(box.size.x - 2.f * RACK_GRID_WIDTH, 0.f)));
    addChild(createWidget<CyanOrbScrew>(Vec(0.f, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    addChild(createWidget<CyanOrbScrew>(
      Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

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

    IrisSmoothingMenuButton* smoothingMenu =
      createParamCentered<IrisSmoothingMenuButton>(
        mm2px(anchor("IRIS_SMOOTHING_MENU_BUTTON", Vec(3.46f, 65.5f))),
        module, Iris::SMOOTHING_MENU_PARAM);
    smoothingMenu->module = module;
    addParam(smoothingMenu);

    IrisChannelPreviewButton* channelPreviewButton =
      createWidgetCentered<IrisChannelPreviewButton>(
        mm2px(anchor("IRIS_CHANNEL_PREVIEW_BUTTON", Vec(57.5f, 65.5f))));
    channelPreviewButton->module = module;
    addChild(channelPreviewButton);

    const Vec channelButtonPos =
      anchor("IRIS_IMAGE_CHANNEL_BUTTON", Vec(7.600001f, 96.225596f));
    IrisImageChannelButton* channelButton =
      createParamCentered<IrisImageChannelButton>(
        mm2px(channelButtonPos), module, Iris::IMAGE_CHANNEL_PARAM);
    channelButton->module = module;
    addParam(channelButton);
    addChild(createLightCentered<SmallAperture<WhiteApertureLight>>(
      mm2px(anchor("IRIS_IMAGE_CHANNEL_ALL_LIGHT", Vec(3.600001f, 92.525596f))),
      module, Iris::IMAGE_CHANNEL_ALL_LIGHT));
    addChild(createLightCentered<SmallAperture<RedApertureLight>>(
      mm2px(anchor("IRIS_IMAGE_CHANNEL_RED_LIGHT", Vec(11.600001f, 92.525596f))),
      module, Iris::IMAGE_CHANNEL_RED_LIGHT));
    addChild(createLightCentered<SmallAperture<GreenApertureLight>>(
      mm2px(anchor("IRIS_IMAGE_CHANNEL_GREEN_LIGHT", Vec(11.600001f, 99.925596f))),
      module, Iris::IMAGE_CHANNEL_GREEN_LIGHT));
    addChild(createLightCentered<SmallAperture<BlueApertureLight>>(
      mm2px(anchor("IRIS_IMAGE_CHANNEL_BLUE_LIGHT", Vec(3.600001f, 99.925596f))),
      module, Iris::IMAGE_CHANNEL_BLUE_LIGHT));

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
      mm2px(anchor("IRIS_COARSE_STEP_MODE_PARAM", Vec(14.2f, 94.07f))),
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
    menu->addChild(createCheckMenuItem("Embed image source in patch", "",
      [irisModule]() { return irisModule->embedsSource(); },
      [irisModule]() { irisModule->setEmbedSource(!irisModule->embedsSource()); }));
  }
};

Model* modelIris = createModel<Iris, IrisWidget>("Iris");
