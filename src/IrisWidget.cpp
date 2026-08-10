#include "Iris.hpp"
#include "Nautiloid.hpp"
#include "NvgGraphicsLifecycle.hpp"
#include "PanelSvgUtils.hpp"
#include "visual/VisualAssets.hpp"
#include "visual/FractalGlassOverlay.hpp"
#include "visual/PreviewSurface.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdlib>
#include <vector>
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

bool isNautiloidModule(const engine::Module* neighbor) {
  if (!neighbor || !neighbor->model) {
    return false;
  }
  return (neighbor->model == modelNautiloid) || (neighbor->model->slug == "Nautiloid");
}

struct IrisLeftShift {
  app::ModuleWidget* widget = nullptr;
  Vec oldPos;
  Vec newPos;
};

bool overlapsVertically(const app::ModuleWidget* widget, float top, float bottom) {
  constexpr float kPositionEpsilon = 0.01f;
  const float widgetTop = widget->box.pos.y;
  const float widgetBottom = widgetTop + widget->box.size.y;
  return widgetBottom > top + kPositionEpsilon && widgetTop < bottom - kPositionEpsilon;
}

std::vector<IrisLeftShift> makeRoomForNautiloid(
    app::RackWidget* rack,
    app::ModuleWidget* irisWidget,
    const Vec& nautPos,
    const Vec& nautSize) {
  constexpr float kPositionEpsilon = 0.01f;
  const float rowTop = std::min(irisWidget->box.pos.y, nautPos.y);
  const float rowBottom = std::max(
    irisWidget->box.pos.y + irisWidget->box.size.y,
    nautPos.y + nautSize.y);

  std::vector<app::ModuleWidget*> candidates;
  for (app::ModuleWidget* widget : rack->getModules()) {
    if (!widget || widget == irisWidget ||
        !overlapsVertically(widget, rowTop, rowBottom) ||
        widget->box.pos.x >= irisWidget->box.pos.x - kPositionEpsilon) {
      continue;
    }
    candidates.push_back(widget);
  }

  std::sort(candidates.begin(), candidates.end(), [](const app::ModuleWidget* a, const app::ModuleWidget* b) {
    return a->box.pos.x + a->box.size.x > b->box.pos.x + b->box.size.x;
  });

  std::vector<IrisLeftShift> shifts;
  float availableRight = nautPos.x;
  for (app::ModuleWidget* widget : candidates) {
    const float widgetRight = widget->box.pos.x + widget->box.size.x;
    if (widgetRight <= availableRight + kPositionEpsilon) {
      break;
    }
    IrisLeftShift shift;
    shift.widget = widget;
    shift.oldPos = widget->box.pos;
    shift.newPos = Vec(availableRight - widget->box.size.x, widget->box.pos.y);
    shifts.push_back(shift);
    availableRight = shift.newPos.x;
  }

  // Move the farthest module first, so each destination is clear before the
  // next member of the obstruction chain is packed against it.
  std::vector<IrisLeftShift*> moved;
  for (auto it = shifts.rbegin(); it != shifts.rend(); ++it) {
    if (!rack->requestModulePos(it->widget, it->newPos)) {
      for (auto movedIt = moved.rbegin(); movedIt != moved.rend(); ++movedIt) {
        rack->requestModulePos((*movedIt)->widget, (*movedIt)->oldPos);
      }
      shifts.clear();
      return shifts;
    }
    moved.push_back(&*it);
  }
  return shifts;
}

void restoreIrisLeftShifts(app::RackWidget* rack, const std::vector<IrisLeftShift>& shifts) {
  for (const IrisLeftShift& shift : shifts) {
    rack->requestModulePos(shift.widget, shift.oldPos);
  }
}

Nautiloid* spawnNautiloidLeftOfIris(ModuleWidget* irisWidget) {
  if (!irisWidget || !irisWidget->module || !APP || !APP->scene || !APP->scene->rack || !modelNautiloid) {
    return nullptr;
  }

  Module* left = irisWidget->module->leftExpander.module;
  if (isNautiloidModule(left)) {
    return dynamic_cast<Nautiloid*>(left);
  }

  engine::Module* nautModule = modelNautiloid->createModule();
  if (!nautModule) {
    return nullptr;
  }
  app::ModuleWidget* nautWidget = modelNautiloid->createModuleWidget(nautModule);
  if (!nautWidget) {
    delete nautModule;
    return nullptr;
  }

  app::RackWidget* rack = APP->scene->rack;
  const Vec nautPos = irisWidget->box.pos.minus(Vec(nautWidget->box.size.x, 0.f));

  if (APP->history) {
    rack->updateModuleOldPositions();
  }
  const std::vector<IrisLeftShift> shifts =
    makeRoomForNautiloid(rack, irisWidget, nautPos, nautWidget->box.size);
  if (!rack->requestModulePos(nautWidget, nautPos)) {
    restoreIrisLeftShifts(rack, shifts);
    delete nautWidget;
    delete nautModule;
    return nullptr;
  }

  history::ComplexAction* moveAction = nullptr;
  if (APP->history) {
    moveAction = rack->getModuleDragAction();
  }

  APP->engine->addModule(nautModule);
  rack->addModule(nautWidget);

  if (APP->history) {
    history::ComplexAction* h = new history::ComplexAction;
    h->name = "add Nautiloid";
    if (moveAction && !moveAction->isEmpty()) {
      h->push(moveAction);
    }
    else {
      delete moveAction;
    }
    history::ModuleAdd* addAction = new history::ModuleAdd;
    addAction->setModule(nautWidget);
    h->push(addAction);
    APP->history->push(h);
  }
  return dynamic_cast<Nautiloid*>(nautModule);
}

void selectNautiloidSourceForIris(ModuleWidget* irisWidget, Iris* iris) {
  if (!irisWidget || !iris) return;
  Module* left = iris->leftExpander.module;
  if (!isNautiloidModule(left)) {
    if (Nautiloid* naut = spawnNautiloidLeftOfIris(irisWidget)) {
      naut->requestIrisSourceSync();
    }
    return;
  }

  Nautiloid* naut = dynamic_cast<Nautiloid*>(left);
  if (!naut) return;
  naut->requestIrisSourceSync();
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
  value = clamp(value, -1.f, 1.f);
  const float t = value < 0.f ? value + 1.f : value;
  const float topR = 28.f;
  const float topG = 204.f;
  const float topB = 217.f;
  const float midR = 7.f;
  const float midG = 12.f;
  const float midB = 31.f;
  const float bottomR = 122.f;
  const float bottomG = 92.f;
  const float bottomB = 255.f;
  const float red = value < 0.f ? bottomR + (midR - bottomR) * t : midR + (topR - midR) * t;
  const float green = value < 0.f ? bottomG + (midG - bottomG) * t : midG + (topG - midG) * t;
  const float blue = value < 0.f ? bottomB + (midB - bottomB) * t : midB + (topB - midB) * t;
  return nvgRGBA(
    uint8_t(std::round(red)),
    uint8_t(std::round(green)),
    uint8_t(std::round(blue)),
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

  void onContextDestroy(const ContextDestroyEvent& e) override {
    nvg_gfx_lifecycle::resetOwnedNvgImage(
      imageContext, imageHandle, uploadedWidth, uploadedHeight, nullptr, false);
    generation = uint64_t(-1);
    OpaqueWidget::onContextDestroy(e);
  }

  void onContextCreate(const ContextCreateEvent& e) override {
    nvg_gfx_lifecycle::resetOwnedNvgImage(
      imageContext, imageHandle, uploadedWidth, uploadedHeight, nullptr, false);
    generation = uint64_t(-1);
    if (framebuffer) framebuffer->setDirty();
    OpaqueWidget::onContextCreate(e);
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

  void draw(const DrawArgs& args) override {
    const float scanTop = std::min(kIrisDisplayVerticalInset, box.size.y * 0.5f);
    const float scanBottom = std::max(scanTop, box.size.y - kIrisDisplayVerticalInset);
    const int channels = module
      ? clamp(module->displayPolyChannelCount.load(std::memory_order_acquire), 1, 16)
      : 1;

    // Draw later voices first so the primary channel remains the clearest
    // reference when two or more voices share the same scan position.
    for (int channel = channels - 1; channel >= 0; --channel) {
      const float scan = module
        ? clamp(
          channel == 0
            ? module->displayScan.load(std::memory_order_relaxed)
            : module->displayPolyScans[size_t(channel)].load(std::memory_order_relaxed),
          0.f, 1.f)
        : 0.62f;
      const float scanY = scanTop + scan * (scanBottom - scanTop);
      nvgBeginPath(args.vg);
      nvgMoveTo(args.vg, 0.f, scanY);
      nvgLineTo(args.vg, box.size.x, scanY);
      if (channel == 0) {
        nvgStrokeWidth(args.vg, 1.2f);
        nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 220));
      }
      else {
        nvgStrokeWidth(args.vg, 0.9f);
        nvgStrokeColor(args.vg, nvgRGBA(185, 225, 255, 112));
      }
      nvgStroke(args.vg);
    }
  }
};

struct IrisWaveformPreview final : TransparentWidget {
  static constexpr int GRADIENT_WIDTH = 2;
  static constexpr int GRADIENT_HEIGHT = 256;

  Iris* module = nullptr;
  widget::FramebufferWidget* framebuffer = nullptr;
  uint64_t generation = uint64_t(-1);
  float cachedScan = -1.f;
  std::vector<float> waveform;
  NVGcontext* gradientContext = nullptr;
  int gradientImage = -1;
  int gradientWidth = 0;
  int gradientHeight = 0;

  explicit IrisWaveformPreview(Iris* module) : module(module) {}

  ~IrisWaveformPreview() override {
    if (APP && APP->window && APP->window->vg) {
      nvg_gfx_lifecycle::resetOwnedNvgImage(
        gradientContext, gradientImage, gradientWidth, gradientHeight, APP->window->vg, true);
      return;
    }
    nvg_gfx_lifecycle::resetOwnedNvgImage(
      gradientContext, gradientImage, gradientWidth, gradientHeight, nullptr, false);
  }

  void onContextDestroy(const ContextDestroyEvent& e) override {
    nvg_gfx_lifecycle::resetOwnedNvgImage(
      gradientContext, gradientImage, gradientWidth, gradientHeight, nullptr, false);
    TransparentWidget::onContextDestroy(e);
  }

  void onContextCreate(const ContextCreateEvent& e) override {
    nvg_gfx_lifecycle::resetOwnedNvgImage(
      gradientContext, gradientImage, gradientWidth, gradientHeight, nullptr, false);
    generation = uint64_t(-1);
    if (framebuffer) framebuffer->setDirty();
    TransparentWidget::onContextCreate(e);
  }

  bool ensureGradientImage(NVGcontext* vg) {
    if (!vg) return false;
    if (gradientContext != vg) {
      nvg_gfx_lifecycle::resetOwnedNvgImage(
        gradientContext, gradientImage, gradientWidth, gradientHeight, vg, false);
      gradientContext = vg;
    }
    if (gradientImage >= 0 && nvg_gfx_lifecycle::ownedNvgImageSizeMatches(
          vg, gradientImage, GRADIENT_WIDTH, GRADIENT_HEIGHT)) {
      return true;
    }

    const std::array<uint8_t, 4> positive {{28u, 204u, 217u, 245u}};
    const std::array<uint8_t, 4> center {{178u, 212u, 246u, 238u}};
    const std::array<uint8_t, 4> negative {{122u, 92u, 255u, 245u}};
    std::array<uint8_t, GRADIENT_WIDTH * GRADIENT_HEIGHT * 4> pixels {};
    for (int y = 0; y < GRADIENT_HEIGHT; ++y) {
      const float position = float(y) / float(GRADIENT_HEIGHT - 1);
      const bool upper = position <= 0.5f;
      const float mix = upper ? position * 2.f : (position - 0.5f) * 2.f;
      const std::array<uint8_t, 4>& from = upper ? positive : center;
      const std::array<uint8_t, 4>& to = upper ? center : negative;
      for (int x = 0; x < GRADIENT_WIDTH; ++x) {
        const size_t base = size_t(y * GRADIENT_WIDTH + x) * 4u;
        for (int channel = 0; channel < 4; ++channel) {
          pixels[base + size_t(channel)] = uint8_t(std::lround(
            float(from[size_t(channel)]) +
            (float(to[size_t(channel)]) - float(from[size_t(channel)])) * mix));
        }
      }
    }
    gradientImage = nvgCreateImageRGBA(
      vg, GRADIENT_WIDTH, GRADIENT_HEIGHT, 0, pixels.data());
    if (gradientImage < 0) return false;
    gradientWidth = GRADIENT_WIDTH;
    gradientHeight = GRADIENT_HEIGHT;
    return true;
  }

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
      nvgStrokeWidth(args.vg, 1.45f);
      if (ensureGradientImage(args.vg)) {
        const NVGpaint gradientPaint = nvgImagePattern(
          args.vg, 0.f, top, box.size.x, bottom - top, 0.f, gradientImage, 1.f);
        nvgStrokePaint(args.vg, gradientPaint);
      } else {
        nvgStrokeColor(args.vg, nvgRGBA(178, 212, 246, 238));
      }
      nvgStroke(args.vg);
    }
  }
};

struct IrisPhaseTracerOverlay final : TransparentWidget {
  static constexpr float DOT_RADIUS = 2.1f;
  static constexpr float DOT_SHOW_MAX_HZ = 2.f;
  static constexpr float DOT_HIDE_MIN_HZ = 2.4f;

  Iris* module = nullptr;
  bool dotVisible = false;

  explicit IrisPhaseTracerOverlay(Iris* module) : module(module) {}

  void step() override {
    if (!module) {
      dotVisible = false;
    }
    else {
      const float frequency = module->displayPhaseFrequencyHz.load(std::memory_order_relaxed);
      if (!std::isfinite(frequency) || frequency >= DOT_HIDE_MIN_HZ) {
        dotVisible = false;
      }
      else if (frequency > 0.f && frequency <= DOT_SHOW_MAX_HZ) {
        dotVisible = true;
      }
    }
    TransparentWidget::step();
  }

  void draw(const DrawArgs& args) override {
    if (!module || !dotVisible) return;

    const float phase = module->displayPhase.load(std::memory_order_relaxed);
    const float wave = module->displayWaveValue.load(std::memory_order_relaxed);
    if (!std::isfinite(phase) || !std::isfinite(wave)) return;

    const float top = 2.f;
    const float bottom = box.size.y - 2.f;
    const float center = 0.5f * (top + bottom);
    const float left = 2.f;
    const float right = box.size.x - 2.f;
    const float phase01 = phase - std::floor(phase);
    const float x = left + phase01 * (right - left);
    const float y = center - clamp(wave, -1.f, 1.f) * 0.5f * (bottom - top);

    nvgBeginPath(args.vg);
    nvgCircle(args.vg, x, y, DOT_RADIUS);
    nvgFillColor(args.vg, nvgRGBA(255, 232, 72, 255));
    nvgFill(args.vg);
    nvgBeginPath(args.vg);
    nvgCircle(args.vg, x, y, DOT_RADIUS + 0.55f);
    nvgStrokeWidth(args.vg, 0.9f);
    nvgStrokeColor(args.vg, nvgRGBA(0, 0, 0, 220));
    nvgStroke(args.vg);
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
        const bool lfoMode = module->params[Iris::LFO_MODE_PARAM].getValue() > 0.5f;
        displayHz = irisBaseFrequencyFromKnob(module->params[Iris::COARSE_PARAM].getValue(), lfoMode);
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
  float maximum = 1.f;

  IrisSmoothingMenuQuantity(Iris* module, float* setting, const char* label,
                            float maximum = 1.f)
    : module(module), setting(setting), label(label), maximum(maximum) {}

  void setValue(float value) override {
    if (!module || !setting) return;
    const float next = clamp(value, 0.f, maximum);
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
    return maximum;
  }

  std::string getLabel() override {
    return label;
  }

  float getDisplayValue() override {
    return maximum > 0.f ? getValue() / maximum * 100.f : 0.f;
  }

  void setDisplayValue(float value) override {
    setValue(value * 0.01f * maximum);
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
    seamSlider->box.size = Vec(270.f, 24.f);
    seamSlider->quantity = new IrisSmoothingMenuQuantity(
      module, &module->conversionSettings.seamSmoothing, "Seam smoothing");
    menu->addChild(seamSlider);
    ui::Slider* waveSlider = new ui::Slider();
    waveSlider->box.size = Vec(270.f, 24.f);
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

struct IrisSourceMenuButton final : TL1105 {
  Iris* module = nullptr;

  void onButton(const event::Button& e) override {
    if (!module || e.button != GLFW_MOUSE_BUTTON_LEFT || e.action != GLFW_PRESS) {
      TL1105::onButton(e);
      return;
    }
    ui::Menu* menu = createMenu();
    menu->box.pos = getAbsoluteOffset(Vec(0.f, box.size.y));
    menu->addChild(createMenuLabel("Source"));
    menu->addChild(createCheckMenuItem(
      "Image file...", "",
      [this]() { return module && module->sourceKind() == iris::SOURCE_IMAGE; },
      [this]() { chooseIrisImage(module); }));
    menu->addChild(createCheckMenuItem(
      "Nautiloid", "",
      [this]() {
        if (!module || !isNautiloidModule(module->leftExpander.module)) return false;
        const int kind = module->sourceKind();
        return kind == iris::SOURCE_EXPANDER_IMAGE || kind == iris::SOURCE_NAUTILOID_FRACTAL;
      },
      [this]() { selectNautiloidSourceForIris(getAncestorOfType<ModuleWidget>(), module); }));
    e.consume(this);
  }

  void draw(const DrawArgs& args) override {
    TL1105::draw(args);
    const float cx = 0.5f * box.size.x;
    const float cy = 0.5f * box.size.y;
    const float radius = std::max(1.3f, 0.13f * box.size.x);
    const NVGcolor stroke = nvgRGBA(225, 232, 240, 244);
    nvgStrokeWidth(args.vg, 1.1f);
    nvgStrokeColor(args.vg, stroke);
    nvgLineCap(args.vg, NVG_ROUND);
    nvgLineJoin(args.vg, NVG_ROUND);

    nvgBeginPath(args.vg);
    nvgCircle(args.vg, cx, cy, radius);
    nvgMoveTo(args.vg, cx, cy - radius);
    nvgBezierTo(args.vg, cx + radius * 2.1f, cy - radius * 1.5f,
                cx + radius * 2.1f, cy + radius * 1.5f, cx, cy + radius);
    nvgMoveTo(args.vg, cx, cy - radius);
    nvgBezierTo(args.vg, cx - radius * 2.1f, cy - radius * 1.5f,
                cx - radius * 2.1f, cy + radius * 1.5f, cx, cy + radius);
    nvgStroke(args.vg);
  }
};

struct IrisChannelPreviewAnchoredTooltip final : ui::Tooltip {
  WeakPtr<Widget> anchor;

  void step() override {
    ui::Tooltip::step();
    Widget* anchorWidget = anchor.get();
    if (!anchorWidget || !APP || !APP->scene) {
      if (parent) requestDelete();
      return;
    }

    const float anchorZoom = std::max(anchorWidget->getAbsoluteZoom(), 1e-6f);
    const Vec anchorOrigin = anchorWidget->getAbsoluteOffset(Vec());
    const Vec anchorSize = anchorWidget->box.size.mult(anchorZoom);
    const Vec sceneSize = APP->scene->box.size;
    const float margin = 4.f;
    float top = margin;
    if (APP->scene->menuBar && APP->scene->menuBar->isVisible()) {
      top = std::max(top,
        APP->scene->menuBar->box.pos.y + APP->scene->menuBar->box.size.y + margin);
    }
    const float desiredX = anchorOrigin.x + anchorSize.x + 2.f;
    const float desiredY = anchorOrigin.y + anchorSize.y + 2.f;
    const float maxX = std::max(margin, sceneSize.x - margin - box.size.x);
    const float maxY = std::max(top, sceneSize.y - margin - box.size.y);
    setPosition(Vec(
      clamp(desiredX, margin, maxX),
      clamp(desiredY, top, maxY)));
  }
};

struct IrisChannelPreviewButton final : TL1105 {
  Iris* module = nullptr;
  WeakPtr<ui::Tooltip> tooltip;
  int pressedFrames = 0;

  ~IrisChannelPreviewButton() {
    destroyTooltip();
  }

  std::string tooltipText() const {
    const bool enabled = module && module->displayChannelPreview.load(std::memory_order_relaxed);
    return enabled ? "Show converted waveform field" : "Show source color channels";
  }

  void createTooltip() {
    if (!settings::tooltips || tooltip || !APP || !APP->scene) return;
    auto* nextTooltip = new IrisChannelPreviewAnchoredTooltip();
    nextTooltip->text = tooltipText();
    nextTooltip->anchor.set(this);
    tooltip.set(nextTooltip);
    APP->scene->addChild(nextTooltip);
  }

  void destroyTooltip() {
    ui::Tooltip* currentTooltip = tooltip.get();
    if (!currentTooltip) return;
    if (currentTooltip->parent) currentTooltip->parent->removeChild(currentTooltip);
    delete currentTooltip;
    tooltip.set(nullptr);
  }

  void refreshTooltip() {
    if (ui::Tooltip* currentTooltip = tooltip.get()) currentTooltip->text = tooltipText();
  }

  void setPressedVisual(bool pressed) {
    if (!sw || frames.empty()) return;
    const size_t frameIndex = pressed && frames.size() > 1u ? 1u : 0u;
    sw->setSvg(frames[frameIndex]);
    if (fb) fb->setDirty();
  }

  void onButton(const event::Button& e) override {
    // This is a callback-only UI control, not an engine parameter. Prevent
    // TL1105's ParamWidget path from opening an invalid parameter menu.
    if (e.button != GLFW_MOUSE_BUTTON_LEFT) {
      e.consume(this);
      return;
    }
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
    refreshTooltip();
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

  std::string tooltipText() const {
    const int mode = module
      ? clamp(module->displayImageChannelMode.load(std::memory_order_relaxed), 0, 3)
      : iris::IMAGE_CHANNEL_ALL;
    const char* modeName = "All";
    switch (mode) {
      case iris::IMAGE_CHANNEL_RED: modeName = "Red"; break;
      case iris::IMAGE_CHANNEL_GREEN: modeName = "Green"; break;
      case iris::IMAGE_CHANNEL_BLUE: modeName = "Blue"; break;
      case iris::IMAGE_CHANNEL_ALL:
      default: modeName = "All"; break;
    }
    return std::string("Color channel: ") + modeName;
  }

  void updateTooltipName() {
    if (ParamQuantity* quantity = getParamQuantity()) {
      quantity->name = tooltipText();
    }
  }

  void initParamQuantity() override {
    SmallGoldButton::initParamQuantity();
    updateTooltipName();
  }

  void step() override {
    updateTooltipName();
    SmallGoldButton::step();
  }

  void onButton(const event::Button& e) override {
    if (module && e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
      const int current = clamp(int(module->conversionSettings.imageChannelMode), 0, 3);
      module->conversionSettings.imageChannelMode = iris::ImageChannelMode((current + 1) % 4);
      module->displayImageChannelMode.store(
        int(module->conversionSettings.imageChannelMode), std::memory_order_relaxed);
      module->requestRebuild();
      updateTooltipName();
    }
    SmallGoldButton::onButton(e);
  }

  void onEnter(const event::Enter& e) override {
    updateTooltipName();
    SmallGoldButton::onEnter(e);
  }
};

} // namespace

struct IrisWidget final : ModuleWidget {
  debug_terminal::BaselineWidgetMetrics debugWidgetMetrics;

  explicit IrisWidget(Iris* module) {
    setModule(module);
    visual_assets::SplitPanelRenderer splitPanel(this, "res/iris.panel.svg");
    const std::string& panelPath = splitPanel.panelPath();
    splitPanel.addLabels("res/iris.labels.svg");
    splitPanel.addCompactLeviathanLogoBranding();
    visual_assets::addFractalGlassOverlay(
      this, panelPath, splitPanel.panelSurfaceEffectWidget());
    addChild(createWidget<CyanOrbScrew>(Vec(RACK_GRID_WIDTH, 0.f)));
    addChild(createWidget<CyanOrbScrew>(Vec(box.size.x - 2.f * RACK_GRID_WIDTH, 0.f)));
    addChild(createWidget<CyanOrbScrew>(
      Vec(RACK_GRID_WIDTH, box.size.y - RACK_GRID_WIDTH)));
    addChild(createWidget<CyanOrbScrew>(
      Vec(box.size.x - 2.f * RACK_GRID_WIDTH, box.size.y - RACK_GRID_WIDTH)));

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
    widget::FramebufferWidget* waveformSurface = preview_surface::createCachedOpaqueGrid(
      mm2px(waveformContentRectMm.size));
    waveformSurface->box.pos = mm2px(waveformContentRectMm.pos);
    addChild(waveformSurface);
    widget::FramebufferWidget* waveformFb = new widget::FramebufferWidget();
    waveformFb->box.pos = mm2px(waveformContentRectMm.pos);
    waveformFb->box.size = mm2px(waveformContentRectMm.size);
    waveformFb->dirtyOnSubpixelChange = false;
    IrisWaveformPreview* waveformPreview = new IrisWaveformPreview(module);
    waveformPreview->box.size = waveformFb->box.size;
    waveformPreview->framebuffer = waveformFb;
    waveformFb->addChild(waveformPreview);
    addChild(waveformFb);
    IrisPhaseTracerOverlay* phaseTracer = new IrisPhaseTracerOverlay(module);
    phaseTracer->box.pos = mm2px(waveformContentRectMm.pos);
    phaseTracer->box.size = mm2px(waveformContentRectMm.size);
    addChild(phaseTracer);
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

    IrisSourceMenuButton* sourceMenu =
      createParamCentered<IrisSourceMenuButton>(
        mm2px(anchor("IRIS_SOURCE_MENU_BUTTON", Vec(7.4f, 65.5f))),
        module, Iris::SOURCE_MENU_PARAM);
    sourceMenu->module = module;
    addParam(sourceMenu);

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
    addChild(createLightCentered<SmallAperture<AmberGreenApertureLight>>(
      mm2px(anchor("NAUTILOID_EXPANDER_LIGHT", Vec(3.2f, 5.8f))),
      module, Iris::NAUTILOID_LINK_LIGHT));

    addParam(createParamCentered<LeviathanHaloKnob2>(
      mm2px(anchor("IRIS_COARSE_PARAM", Vec(13.5f, 54.f))), module, Iris::COARSE_PARAM));
    addParam(createParamCentered<BipolarDarkTinyClockworkGearKnob>(
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
    static_cast<SmallGoldApertureLight*>(octaveStep->getLight())->setBaseColor(nvgRGB(255, 118, 24));
    addParam(octaveStep);
    SmallGoldApertureButton* softSync = createLightParamCentered<SmallGoldApertureButton>(
      mm2px(anchor("IRIS_SOFT_SYNC_MODE_PARAM", Vec(44.64f, 76.73f))),
      module, Iris::SOFT_SYNC_MODE_PARAM, Iris::SOFT_SYNC_MODE_LIGHT);
    static_cast<SmallGoldApertureLight*>(softSync->getLight())->setBaseColor(nvgRGB(255, 118, 24));
    addParam(softSync);
    SmallGoldApertureButton* lfoMode = createLightParamCentered<SmallGoldApertureButton>(
      mm2px(anchor("IRIS_LFO_MODE_PARAM", Vec(25.3f, 111.8f))),
      module, Iris::LFO_MODE_PARAM, Iris::LFO_MODE_LIGHT);
    static_cast<SmallGoldApertureLight*>(lfoMode->getLight())->setBaseColor(nvgRGB(255, 118, 24));
    addParam(lfoMode);

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
      mm2px(anchor("IRIS_INV_OUTPUT", Vec(41.96f, 118.f))), module, Iris::Q_OUTPUT));
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
