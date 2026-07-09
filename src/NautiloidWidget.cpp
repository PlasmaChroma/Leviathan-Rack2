#include "Nautiloid.hpp"
#include "NvgGraphicsLifecycle.hpp"
#include "visual/VisualAssets.hpp"

#include <fstream>

namespace {

constexpr float kNautiloidWidthMm = 101.6f;
constexpr float kNautiloidHeightMm = 128.5f;
constexpr float kNautiloidMaxFractalZoom = 5.f;

std::string nautiloidUserRootPath() {
  return system::join(asset::user(), "Leviathan/Nautiloid");
}

std::string nautiloidDebugLogPath() {
  return system::join(nautiloidUserRootPath(), "fractal-pipeline.csv");
}

Vec nautiloidFractalViewportHalfSpan(int mode) {
  switch (mode) {
    case iris::FRACTAL_MANDELBROT:
      return Vec(1.62f, 0.86f);
    case iris::FRACTAL_JULIA:
      return Vec(1.58f, 0.72f);
    case iris::FRACTAL_PHOENIX_JULIA:
      return Vec(1.62f, 0.74f);
    case iris::FRACTAL_BURNING_SHIP:
      return Vec(0.42f, 0.145f);
    case iris::FRACTAL_CELTIC:
      return Vec(1.62f, 0.88f);
    case iris::FRACTAL_SPIDER:
      return Vec(1.56f, 0.84f);
    case iris::FRACTAL_NOVA:
      return Vec(2.0f, 0.86f);
    case iris::FRACTAL_NEWTON:
      return Vec(2.45f, 0.98f);
    case iris::FRACTAL_EYE_OF_THE_WORLD:
      return Vec(0.0075f, 0.00395f);
    case iris::FRACTAL_TRICORN:
    default:
      return Vec(1.68f, 0.90f);
  }
}

bool nautiloidRequestDue(double* lastRequestTime, double minIntervalSec) {
  const double now = system::getTime();
  if (!std::isfinite(*lastRequestTime) || now - *lastRequestTime >= minIntervalSec) {
    *lastRequestTime = now;
    return true;
  }
  return false;
}

struct NautiloidDisplay final : OpaqueWidget {
  Nautiloid* module = nullptr;
  widget::FramebufferWidget* framebuffer = nullptr;
  uint64_t generation = uint64_t(-1);
  NVGcontext* imageContext = nullptr;
  int imageHandle = -1;
  int uploadedWidth = 0;
  int uploadedHeight = 0;
  std::vector<uint8_t> rgba;
  bool panActive = false;
  Vec lastPanLocal;
  double lastPanRequestTime = -INFINITY;

  explicit NautiloidDisplay(Nautiloid* module) : module(module) {}

  ~NautiloidDisplay() override {
    if (APP && APP->window && APP->window->vg) {
      nvg_gfx_lifecycle::resetOwnedNvgImage(
        imageContext, imageHandle, uploadedWidth, uploadedHeight, APP->window->vg, true);
      return;
    }
    nvg_gfx_lifecycle::resetOwnedNvgImage(
      imageContext, imageHandle, uploadedWidth, uploadedHeight, nullptr, false);
  }

  Vec currentLocalMousePos() const {
    if (!parent || !APP || !APP->scene || !APP->scene->rack) {
      return Vec();
    }
    return APP->scene->rack->getMousePos().minus(parent->box.pos).minus(box.pos);
  }

  void onButton(const event::Button& e) override {
    if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS && module) {
      panActive = true;
      lastPanLocal = currentLocalMousePos();
      e.consume(this);
      return;
    }
    if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_RELEASE) {
      panActive = false;
    }
    OpaqueWidget::onButton(e);
  }

  void onDragStart(const event::DragStart& e) override {
    if (module && e.button == GLFW_MOUSE_BUTTON_LEFT) {
      panActive = true;
      lastPanLocal = currentLocalMousePos();
      e.consume(this);
      return;
    }
    OpaqueWidget::onDragStart(e);
  }

  void onDragMove(const event::DragMove& e) override {
    if (module && panActive && e.button == GLFW_MOUSE_BUTTON_LEFT) {
      const Vec current = currentLocalMousePos();
      const Vec delta = current.minus(lastPanLocal);
      lastPanLocal = current;
      if (box.size.x > 1.f && box.size.y > 1.f && (std::fabs(delta.x) > 0.f || std::fabs(delta.y) > 0.f)) {
        const float zoomScale = std::pow(0.05f, clamp(module->fractalZoom, 0.f, kNautiloidMaxFractalZoom));
        const Vec halfSpan = nautiloidFractalViewportHalfSpan(module->fractalMode).mult(zoomScale);
        const Vec centerDelta(
          -delta.x / box.size.x * 2.f * halfSpan.x,
          -delta.y / box.size.y * 2.f * halfSpan.y);
        module->fractalCenterX = clamp(module->fractalCenterX + centerDelta.x, -2.f, 2.f);
        module->fractalCenterY = clamp(module->fractalCenterY + centerDelta.y, -2.f, 2.f);
        if (nautiloidRequestDue(&lastPanRequestTime, 0.05)) {
          const float cacheLead = 3.f;
          const float maxLeadX = 0.35f * halfSpan.x;
          const float maxLeadY = 0.35f * halfSpan.y;
          const float cacheCenterX =
            clamp(module->fractalCenterX + clamp(centerDelta.x * cacheLead, -maxLeadX, maxLeadX), -2.f, 2.f);
          const float cacheCenterY =
            clamp(module->fractalCenterY + clamp(centerDelta.y * cacheLead, -maxLeadY, maxLeadY), -2.f, 2.f);
          module->requestRenderWithCacheCenter(cacheCenterX, cacheCenterY);
        }
      }
      e.consume(this);
      return;
    }
    OpaqueWidget::onDragMove(e);
  }

  void onDragEnd(const event::DragEnd& e) override {
    if (e.button == GLFW_MOUSE_BUTTON_LEFT && panActive) {
      panActive = false;
      if (module) {
        module->requestRenderWithCenteredCache();
      }
      e.consume(this);
      return;
    }
    OpaqueWidget::onDragEnd(e);
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
      std::vector<uint8_t> rgb;
      int width = 0;
      int height = 0;
      if (module) {
        module->previewSnapshot(&rgb, &width, &height);
      }
      rgba.resize(rgb.size() / 3u * 4u);
      for (size_t i = 0; i + 2u < rgb.size(); i += 3u) {
        const size_t out = (i / 3u) * 4u;
        rgba[out + 0u] = rgb[i + 0u];
        rgba[out + 1u] = rgb[i + 1u];
        rgba[out + 2u] = rgb[i + 2u];
        rgba[out + 3u] = 255u;
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
      NVGpaint paint =
        nvgImagePattern(args.vg, 0.f, 0.f, box.size.x, box.size.y, 0.f, imageHandle, 1.f);
      nvgBeginPath(args.vg);
      nvgRect(args.vg, 0.f, 0.f, box.size.x, box.size.y);
      nvgFillPaint(args.vg, paint);
      nvgFill(args.vg);
    }
    if (module && module->loading.load(std::memory_order_acquire)) {
      nvgBeginPath(args.vg);
      nvgRect(args.vg, 0.f, 0.f, box.size.x, box.size.y);
      nvgFillColor(args.vg, nvgRGBA(0, 0, 0, 34));
      nvgFill(args.vg);
    }
  }
};

struct NautiloidZoomSpeedQuantity final : Quantity {
  float position = 0.5f;

  void setValue(float value) override {
    position = clamp(value, 0.f, 1.f);
  }

  float getValue() override {
    return position;
  }

  float getDefaultValue() override {
    return 0.5f;
  }

  float getDisplayValue() override {
    return (getValue() - 0.5f) * 200.f;
  }

  void setDisplayValue(float displayValue) override {
    setValue(displayValue / 200.f + 0.5f);
  }

  std::string getLabel() override {
    return "Zoom";
  }

  std::string getUnit() override {
    return "%";
  }

  std::string getDisplayValueString() override {
    return string::f("%+.0f", getDisplayValue());
  }
};

struct NautiloidIrisMiniDisplay final : OpaqueWidget {
  Nautiloid* module = nullptr;
  widget::FramebufferWidget* framebuffer = nullptr;
  uint64_t generation = uint64_t(-1);
  NVGcontext* imageContext = nullptr;
  int imageHandle = -1;
  int uploadedWidth = 0;
  int uploadedHeight = 0;
  std::vector<uint8_t> rgba;

  explicit NautiloidIrisMiniDisplay(Nautiloid* module) : module(module) {}

  ~NautiloidIrisMiniDisplay() override {
    if (APP && APP->window && APP->window->vg) {
      nvg_gfx_lifecycle::resetOwnedNvgImage(
        imageContext, imageHandle, uploadedWidth, uploadedHeight, APP->window->vg, true);
      return;
    }
    nvg_gfx_lifecycle::resetOwnedNvgImage(
      imageContext, imageHandle, uploadedWidth, uploadedHeight, nullptr, false);
  }

  void step() override {
    const uint64_t currentGeneration =
      module ? module->irisPreviewGeneration.load(std::memory_order_acquire) : 0u;
    if (generation != currentGeneration && framebuffer) {
      framebuffer->setDirty();
    }
    OpaqueWidget::step();
  }

  void draw(const DrawArgs& args) override {
    nvgBeginPath(args.vg);
    nvgRect(args.vg, 0.f, 0.f, box.size.x, box.size.y);
    nvgFillColor(args.vg, nvgRGB(3, 5, 7));
    nvgFill(args.vg);

    const uint64_t currentGeneration =
      module ? module->irisPreviewGeneration.load(std::memory_order_acquire) : 0u;
    if (imageContext != args.vg) {
      nvg_gfx_lifecycle::resetOwnedNvgImage(
        imageContext, imageHandle, uploadedWidth, uploadedHeight, args.vg, false);
      imageContext = args.vg;
      generation = uint64_t(-1);
    }
    if (generation != currentGeneration || imageHandle < 0 ||
        !nvg_gfx_lifecycle::ownedNvgImageSizeMatches(args.vg, imageHandle, uploadedWidth, uploadedHeight)) {
      std::vector<uint8_t> rgb;
      int width = 0;
      int height = 0;
      if (module) {
        module->irisPreviewSnapshot(&rgb, &width, &height);
      }
      rgba.resize(rgb.size() / 3u * 4u);
      for (size_t i = 0; i + 2u < rgb.size(); i += 3u) {
        const size_t out = (i / 3u) * 4u;
        rgba[out + 0u] = rgb[i + 0u];
        rgba[out + 1u] = rgb[i + 1u];
        rgba[out + 2u] = rgb[i + 2u];
        rgba[out + 3u] = 255u;
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
      NVGpaint paint =
        nvgImagePattern(args.vg, 0.f, 0.f, box.size.x, box.size.y, 0.f, imageHandle, 1.f);
      nvgBeginPath(args.vg);
      nvgRect(args.vg, 0.f, 0.f, box.size.x, box.size.y);
      nvgFillPaint(args.vg, paint);
      nvgFill(args.vg);
    }
  }
};

struct NautiloidTileCacheGrid final : TransparentWidget {
  Nautiloid* module = nullptr;

  explicit NautiloidTileCacheGrid(Nautiloid* module) : module(module) {}

  void draw(const DrawArgs& args) override {
    nvgBeginPath(args.vg);
    nvgRoundedRect(args.vg, 0.f, 0.f, box.size.x, box.size.y, 3.f);
    nvgFillColor(args.vg, nvgRGB(4, 7, 10));
    nvgFill(args.vg);
    nvgStrokeWidth(args.vg, 1.f);
    nvgStrokeColor(args.vg, nvgRGBA(88, 65, 191, 150));
    nvgStroke(args.vg);

    if (!module) return;

    Nautiloid::DisplayTileCacheSnapshot snapshot;
    module->displayTileCacheSnapshot(&snapshot);
    if (snapshot.columns <= 0 || snapshot.rows <= 0 ||
        snapshot.tileCurrent.size() != size_t(snapshot.columns) * size_t(snapshot.rows)) {
      return;
    }

    constexpr float pad = 3.2f;
    const float gap = 1.05f;
    const float cellW = (box.size.x - 2.f * pad - gap * float(snapshot.columns - 1)) / float(snapshot.columns);
    const float cellH = (box.size.y - 2.f * pad - gap * float(snapshot.rows - 1)) / float(snapshot.rows);
    const float cell = std::max(1.f, std::min(cellW, cellH));
    const float gridW = float(snapshot.columns) * cell + float(snapshot.columns - 1) * gap;
    const float gridH = float(snapshot.rows) * cell + float(snapshot.rows - 1) * gap;
    const float x0 = 0.5f * (box.size.x - gridW);
    const float y0 = 0.5f * (box.size.y - gridH);

    const NVGcolor staleColor = snapshot.current ? nvgRGBA(33, 40, 56, 205) : nvgRGBA(45, 30, 70, 145);
    const NVGcolor currentColor = nvgRGB(28, 204, 217);
    const NVGcolor edgeColor = nvgRGBA(226, 232, 240, 58);
    for (int row = 0; row < snapshot.rows; ++row) {
      for (int column = 0; column < snapshot.columns; ++column) {
        const size_t index = size_t(row) * size_t(snapshot.columns) + size_t(column);
        const bool current = snapshot.tileCurrent[index] != 0u;
        const float x = x0 + float(column) * (cell + gap);
        const float y = y0 + float(row) * (cell + gap);
        nvgBeginPath(args.vg);
        nvgRoundedRect(args.vg, x, y, cell, cell, std::min(1.8f, cell * 0.25f));
        nvgFillColor(args.vg, current ? currentColor : staleColor);
        nvgFill(args.vg);
        nvgStrokeWidth(args.vg, 0.65f);
        nvgStrokeColor(args.vg, current ? nvgRGBA(220, 255, 255, 90) : edgeColor);
        nvgStroke(args.vg);
      }
    }

    if (snapshot.current) {
      const float zoomScale = std::pow(0.05f, clamp(module->fractalZoom, 0.f, kNautiloidMaxFractalZoom));
      const Vec halfSpan = nautiloidFractalViewportHalfSpan(module->fractalMode).mult(zoomScale);
      const float cacheScale = std::max(1.f, snapshot.cacheScale);
      const float cacheHalfX = halfSpan.x * cacheScale;
      const float cacheHalfY = halfSpan.y * cacheScale;
      if (cacheHalfX > 0.f && cacheHalfY > 0.f) {
        const float dx = module->fractalCenterX - snapshot.cacheCenterX;
        const float dy = module->fractalCenterY - snapshot.cacheCenterY;
        const float centerX = x0 + (0.5f + dx / (2.f * cacheHalfX)) * gridW;
        const float centerY = y0 + (0.5f + dy / (2.f * cacheHalfY)) * gridH;
        const float viewW = gridW / cacheScale;
        const float viewH = gridH / cacheScale;
        nvgBeginPath(args.vg);
        nvgRect(args.vg, centerX - 0.5f * viewW, centerY - 0.5f * viewH, viewW, viewH);
        nvgStrokeWidth(args.vg, 1.2f);
        nvgStrokeColor(args.vg, nvgRGBA(236, 240, 255, 190));
        nvgStroke(args.vg);
      }
    }
  }
};

struct NautiloidDebugCounters final : TransparentWidget {
  Nautiloid* module = nullptr;
  double lastLogTime = -INFINITY;
  uint64_t lastLoggedRequests = uint64_t(-1);
  uint64_t lastLoggedIrisGeneration = uint64_t(-1);

  explicit NautiloidDebugCounters(Nautiloid* module) : module(module) {}

  void step() override {
    if (module && module->debugFileLoggingEnabled.load(std::memory_order_relaxed)) {
      const double now = system::getTime();
      const uint64_t requests = module->renderRequestsSubmitted.load(std::memory_order_relaxed);
      const uint64_t irisGeneration = module->irisPreviewGeneration.load(std::memory_order_relaxed);
      if ((!std::isfinite(lastLogTime) || now - lastLogTime >= 0.25) &&
          (requests != lastLoggedRequests || irisGeneration != lastLoggedIrisGeneration)) {
        lastLogTime = now;
        lastLoggedRequests = requests;
        lastLoggedIrisGeneration = irisGeneration;
        appendLog(now);
      }
    }
    TransparentWidget::step();
  }

  void draw(const DrawArgs& args) override {
    if (!module) return;

    const auto load = [](const std::atomic<uint64_t>& value) {
      return value.load(std::memory_order_relaxed);
    };

    nvgFontSize(args.vg, 7.5f);
    nvgFontFaceId(args.vg, APP->window->uiFont->handle);
    nvgTextLetterSpacing(args.vg, 0.f);
    nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
    nvgFillColor(args.vg, nvgRGBA(205, 218, 235, 210));

    const std::string left = string::f(
      "req %llu  disp %llu  hit/miss %llu/%llu",
      (unsigned long long) load(module->renderRequestsSubmitted),
      (unsigned long long) load(module->displayRendersCompleted),
      (unsigned long long) load(module->displayCacheHits),
      (unsigned long long) load(module->displayCacheMisses));
    const std::string right = string::f(
      "iris %llu  exp %llu  cache %llu/%llu  stale d/i %llu/%llu",
      (unsigned long long) load(module->irisRendersCompleted),
      (unsigned long long) load(module->irisExpanderPublishes),
      (unsigned long long) load(module->cacheRequestsDequeued),
      (unsigned long long) load(module->displayCacheRendersCompleted),
      (unsigned long long) load(module->displayRendersDroppedStale),
      (unsigned long long) load(module->irisRendersDroppedStale));

    nvgText(args.vg, 0.f, 0.f, left.c_str(), nullptr);
    nvgText(args.vg, 0.f, 9.f, right.c_str(), nullptr);
  }

  void appendLog(double now) {
    const std::string dir = nautiloidUserRootPath();
    system::createDirectories(dir);
    const std::string path = nautiloidDebugLogPath();
    const bool needsHeader = !system::exists(path) || system::getFileSize(path) == 0u;
    std::ofstream log(path, std::ios::app);
    if (!log) return;
    if (needsHeader) {
      log << "time,zoom,center_x,center_y,loading,req,display_gen,iris_gen,display_done,"
             "display_stale,cache_hits,cache_misses,cache_submitted,cache_dequeued,"
             "cache_done,iris_done,iris_stale,iris_expander_publishes\n";
    }
    log
      << now << ','
      << module->fractalZoom << ','
      << module->fractalCenterX << ','
      << module->fractalCenterY << ','
      << (module->loading.load(std::memory_order_relaxed) ? 1 : 0) << ','
      << module->renderRequestsSubmitted.load(std::memory_order_relaxed) << ','
      << module->previewGeneration.load(std::memory_order_relaxed) << ','
      << module->irisPreviewGeneration.load(std::memory_order_relaxed) << ','
      << module->displayRendersCompleted.load(std::memory_order_relaxed) << ','
      << module->displayRendersDroppedStale.load(std::memory_order_relaxed) << ','
      << module->displayCacheHits.load(std::memory_order_relaxed) << ','
      << module->displayCacheMisses.load(std::memory_order_relaxed) << ','
      << module->cacheRequestsSubmitted.load(std::memory_order_relaxed) << ','
      << module->cacheRequestsDequeued.load(std::memory_order_relaxed) << ','
      << module->displayCacheRendersCompleted.load(std::memory_order_relaxed) << ','
      << module->irisRendersCompleted.load(std::memory_order_relaxed) << ','
      << module->irisRendersDroppedStale.load(std::memory_order_relaxed)
      << ','
      << module->irisExpanderPublishes.load(std::memory_order_relaxed)
      << '\n';
  }
};

struct NautiloidZoomSlider final : ui::Slider {
  Nautiloid* module = nullptr;
  NautiloidZoomSpeedQuantity* zoomSpeed = nullptr;
  bool zoomActive = false;
  double lastStepTime = -INFINITY;
  double lastRequestTime = -INFINITY;

  void onButton(const event::Button& e) override {
    if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
      if (e.action == GLFW_PRESS) {
        zoomActive = true;
        lastStepTime = system::getTime();
      } else if (e.action == GLFW_RELEASE) {
        stopZoom();
      }
    }
    ui::Slider::onButton(e);
  }

  void onDragStart(const event::DragStart& e) override {
    if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
      zoomActive = true;
      lastStepTime = system::getTime();
    }
    ui::Slider::onDragStart(e);
  }

  void onDragEnd(const event::DragEnd& e) override {
    ui::Slider::onDragEnd(e);
    if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
      stopZoom();
    }
  }

  void step() override {
    const double now = system::getTime();
    if (module && zoomActive && zoomSpeed) {
      if (!std::isfinite(lastStepTime)) {
        lastStepTime = now;
      }
      const double dt = std::max(0.0, std::min(now - lastStepTime, 0.05));
      lastStepTime = now;
      const float speed = (zoomSpeed->getValue() - 0.5f) * 2.f;
      if (std::fabs(speed) > 0.015f && dt > 0.0) {
        const float shapedSpeed = speed * std::fabs(speed);
        const float next = clamp(module->fractalZoom + shapedSpeed * float(dt) * 0.85f, 0.f, kNautiloidMaxFractalZoom);
        if (std::fabs(module->fractalZoom - next) > 1e-5f) {
          module->fractalZoom = next;
          if (nautiloidRequestDue(&lastRequestTime, 0.04)) {
            module->requestRender();
          }
        }
      }
    }
    ui::Slider::step();
  }

  void draw(const DrawArgs& args) override {
    const float value = zoomSpeed ? clamp(zoomSpeed->getValue(), 0.f, 1.f) : 0.5f;
    const float zoomAmount =
      module ? clamp(module->fractalZoom / kNautiloidMaxFractalZoom, 0.f, 1.f) : 0.f;
    const float centerX = 0.5f * box.size.x;
    const float handleX = value * box.size.x;
    const float trackH = std::max(3.f, std::min(8.f, box.size.y * 0.34f));
    const float progressH = std::max(2.f, std::min(4.f, box.size.y * 0.18f));
    const float gap = std::max(2.f, box.size.y * 0.14f);
    const float contentH = trackH + gap + progressH;
    const float trackY = std::max(0.f, 0.5f * (box.size.y - contentH));
    const float progressY = trackY + trackH + gap;
    const float radius = 0.5f * trackH;
    const float progressRadius = 0.5f * progressH;

    nvgBeginPath(args.vg);
    nvgRoundedRect(args.vg, 0.f, trackY, box.size.x, trackH, radius);
    nvgFillColor(args.vg, nvgRGB(12, 16, 22));
    nvgFill(args.vg);
    nvgStrokeWidth(args.vg, 1.f);
    nvgStrokeColor(args.vg, nvgRGBA(160, 170, 188, 72));
    nvgStroke(args.vg);

    const float fillLeft = std::min(centerX, handleX);
    const float fillW = std::fabs(handleX - centerX);
    if (fillW > 0.75f) {
      const bool zoomIn = handleX > centerX;
      nvgSave(args.vg);
      nvgIntersectScissor(args.vg, fillLeft, trackY - 1.f, fillW, trackH + 2.f);
      nvgBeginPath(args.vg);
      nvgRoundedRect(args.vg, fillLeft - (zoomIn ? 0.f : radius), trackY, fillW + radius, trackH, radius);
      nvgFillColor(args.vg, zoomIn ? nvgRGB(28, 204, 217) : nvgRGB(122, 92, 255));
      nvgFill(args.vg);
      nvgRestore(args.vg);
    }

    nvgBeginPath(args.vg);
    nvgMoveTo(args.vg, centerX, trackY - 3.f);
    nvgLineTo(args.vg, centerX, trackY + trackH + 3.f);
    nvgStrokeWidth(args.vg, 1.25f);
    nvgStrokeColor(args.vg, nvgRGBA(232, 238, 246, 118));
    nvgStroke(args.vg);

    const float handleW = std::max(8.f, box.size.y * 0.42f);
    const float desiredHandleH = std::max(trackH + 5.f, box.size.y * 0.42f);
    const float handleH = std::min(desiredHandleH, std::max(trackH + 2.f, progressY - 1.f));
    const float handleY =
      clamp(trackY + 0.5f * (trackH - handleH), 0.f, std::max(0.f, progressY - handleH - 1.f));
    nvgBeginPath(args.vg);
    nvgRoundedRect(args.vg, handleX - 0.5f * handleW, handleY, handleW, handleH, 3.f);
    nvgFillColor(args.vg, nvgRGB(226, 232, 240));
    nvgFill(args.vg);
    nvgStrokeWidth(args.vg, 1.f);
    nvgStrokeColor(args.vg, nvgRGBA(20, 24, 30, 180));
    nvgStroke(args.vg);

    nvgBeginPath(args.vg);
    nvgRoundedRect(args.vg, 0.f, progressY, box.size.x, progressH, progressRadius);
    nvgFillColor(args.vg, nvgRGB(8, 11, 16));
    nvgFill(args.vg);
    nvgStrokeWidth(args.vg, 1.f);
    nvgStrokeColor(args.vg, nvgRGBA(160, 170, 188, 58));
    nvgStroke(args.vg);

    const float progressW = zoomAmount * box.size.x;
    if (progressW > 0.5f) {
      nvgSave(args.vg);
      nvgIntersectScissor(args.vg, 0.f, progressY - 1.f, progressW, progressH + 2.f);
      nvgBeginPath(args.vg);
      nvgRoundedRect(args.vg, 0.f, progressY, box.size.x, progressH, progressRadius);
      NVGpaint progressPaint = nvgLinearGradient(
        args.vg, 0.f, progressY, box.size.x, progressY, nvgRGB(122, 92, 255), nvgRGB(28, 204, 217));
      nvgFillPaint(args.vg, progressPaint);
      nvgFill(args.vg);
      nvgRestore(args.vg);
    }
  }

  void stopZoom() {
    zoomActive = false;
    lastStepTime = -INFINITY;
    if (zoomSpeed) {
      zoomSpeed->setValue(0.5f);
    }
    if (module) {
      module->requestRender();
    }
  }
};

struct NautiloidSourceButton final : TL1105 {
  Nautiloid* module = nullptr;

  void onButton(const event::Button& e) override {
    if (!module || e.button != GLFW_MOUSE_BUTTON_LEFT || e.action != GLFW_PRESS) {
      TL1105::onButton(e);
      return;
    }
    ui::Menu* menu = createMenu();
    menu->box.pos = getAbsoluteOffset(Vec(0.f, box.size.y));
    menu->addChild(createMenuLabel("Fractals"));
    for (int mode = iris::kFirstBuiltinFractalMode; mode <= iris::kLastBuiltinFractalMode; ++mode) {
      if (!iris::isBuiltinFractalMode(mode)) continue;
      menu->addChild(createCheckMenuItem(
        iris::builtinFractalName(mode), "",
        [this, mode]() { return module->fractalMode == mode; },
        [this, mode]() { module->requestFractal(mode); }));
    }
    e.consume(this);
  }

  void draw(const DrawArgs& args) override {
    TL1105::draw(args);
    const float cx = 0.5f * box.size.x;
    const float cy = 0.5f * box.size.y;
    const float r = std::max(1.3f, 0.13f * box.size.x);
    nvgStrokeWidth(args.vg, 1.1f);
    nvgStrokeColor(args.vg, nvgRGBA(225, 232, 240, 244));
    nvgBeginPath(args.vg);
    nvgCircle(args.vg, cx, cy, r);
    nvgMoveTo(args.vg, cx - r * 1.6f, cy);
    nvgLineTo(args.vg, cx + r * 1.6f, cy);
    nvgMoveTo(args.vg, cx, cy - r * 1.6f);
    nvgLineTo(args.vg, cx, cy + r * 1.6f);
    nvgStroke(args.vg);
  }
};

struct NautiloidResetButton final : TL1105 {
  Nautiloid* module = nullptr;

  void onButton(const event::Button& e) override {
    if (module && e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
      module->resetView();
      e.consume(this);
      return;
    }
    TL1105::onButton(e);
  }

  void draw(const DrawArgs& args) override {
    TL1105::draw(args);
    const float cx = 0.5f * box.size.x;
    const float cy = 0.5f * box.size.y;
    const float r = std::max(2.f, 0.18f * box.size.x);
    nvgStrokeWidth(args.vg, 1.2f);
    nvgStrokeColor(args.vg, nvgRGBA(225, 232, 240, 244));
    nvgBeginPath(args.vg);
    nvgArc(args.vg, cx, cy, r, -0.25f * float(M_PI), 1.35f * float(M_PI), NVG_CCW);
    nvgLineTo(args.vg, cx - r * 0.95f, cy - r * 0.35f);
    nvgStroke(args.vg);
  }
};

} // namespace

struct NautiloidWidget final : ModuleWidget {
  explicit NautiloidWidget(Nautiloid* module) {
    setModule(module);
    setPanel(createPanel(asset::plugin(pluginInstance, "res/nautiloid.panel.svg")));

    const math::Rect displayRectMm(Vec(1.8f, 6.5f), Vec(98.f, 65.27f));
    addChild(visual_assets::createPreviewFrameEnhancementWidget(
      displayRectMm, visual_assets::PreviewFrameTint::Purple));
    widget::FramebufferWidget* displayFb = new widget::FramebufferWidget();
    displayFb->box.pos = mm2px(displayRectMm.pos.plus(Vec(0.4f, 0.4f)));
    displayFb->box.size = mm2px(displayRectMm.size.minus(Vec(0.8f, 0.8f)));
    displayFb->dirtyOnSubpixelChange = false;
    NautiloidDisplay* display = new NautiloidDisplay(module);
    display->framebuffer = displayFb;
    display->box.size = displayFb->box.size;
    displayFb->addChild(display);
    addChild(displayFb);

    NautiloidZoomSlider* zoomSlider = new NautiloidZoomSlider();
    zoomSlider->module = module;
    zoomSlider->box.pos = mm2px(Vec(5.f, 79.f));
    zoomSlider->box.size = mm2px(Vec(91.6f, 9.f));
    NautiloidZoomSpeedQuantity* zoomSpeed = new NautiloidZoomSpeedQuantity();
    zoomSlider->zoomSpeed = zoomSpeed;
    zoomSlider->quantity = zoomSpeed;
    addChild(zoomSlider);

    NautiloidSourceButton* sourceButton =
      createParamCentered<NautiloidSourceButton>(mm2px(Vec(42.f, 75.4f)), module, Nautiloid::SOURCE_MENU_PARAM);
    sourceButton->module = module;
    addParam(sourceButton);

    NautiloidResetButton* resetButton =
      createParamCentered<NautiloidResetButton>(mm2px(Vec(59.6f, 75.4f)), module, Nautiloid::RESET_VIEW_PARAM);
    resetButton->module = module;
    addParam(resetButton);

    const math::Rect tileCacheRectMm(Vec(2.f, 102.f), Vec(42.f, 25.9f));
    NautiloidTileCacheGrid* tileCacheGrid = new NautiloidTileCacheGrid(module);
    tileCacheGrid->box.pos = mm2px(tileCacheRectMm.pos);
    tileCacheGrid->box.size = mm2px(tileCacheRectMm.size);
    addChild(tileCacheGrid);

    const math::Rect irisPreviewRectMm(Vec(47.4f, 102.f), Vec(52.36f, 25.9f));
    addChild(visual_assets::createPreviewFrameEnhancementWidget(
      irisPreviewRectMm, visual_assets::PreviewFrameTint::Purple));
    widget::FramebufferWidget* irisPreviewFb = new widget::FramebufferWidget();
    irisPreviewFb->box.pos = mm2px(irisPreviewRectMm.pos.plus(Vec(0.35f, 0.35f)));
    irisPreviewFb->box.size = mm2px(irisPreviewRectMm.size.minus(Vec(0.7f, 0.7f)));
    irisPreviewFb->dirtyOnSubpixelChange = false;
    NautiloidIrisMiniDisplay* irisPreview = new NautiloidIrisMiniDisplay(module);
    irisPreview->framebuffer = irisPreviewFb;
    irisPreview->box.size = irisPreviewFb->box.size;
    irisPreviewFb->addChild(irisPreview);
    addChild(irisPreviewFb);

    NautiloidDebugCounters* counters = new NautiloidDebugCounters(module);
    counters->box.pos = mm2px(Vec(48.0f, 96.6f));
    counters->box.size = mm2px(Vec(50.5f, 8.f));
    addChild(counters);
  }

  void appendContextMenu(Menu* menu) override {
    ModuleWidget::appendContextMenu(menu);
    Nautiloid* naut = dynamic_cast<Nautiloid*>(module);
    if (!naut || !isDragonKingDebugEnabled()) return;

    menu->addChild(new MenuSeparator());
    menu->addChild(createMenuLabel("Nautiloid Debug"));
    menu->addChild(createCheckMenuItem(
      "Log fractal pipeline to file", "",
      [naut]() {
        return naut->debugFileLoggingEnabled.load(std::memory_order_relaxed);
      },
      [naut]() {
        const bool current = naut->debugFileLoggingEnabled.load(std::memory_order_relaxed);
        naut->debugFileLoggingEnabled.store(!current, std::memory_order_relaxed);
      }));
    menu->addChild(createMenuLabel(nautiloidDebugLogPath()));
  }
};

Model* modelNautiloid = createModel<Nautiloid, NautiloidWidget>("Nautiloid");
