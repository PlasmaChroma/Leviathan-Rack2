#include "Nautiloid.hpp"
#include "NvgGraphicsLifecycle.hpp"
#include "visual/VisualAssets.hpp"

namespace {

constexpr float kNautiloidWidthMm = 101.6f;
constexpr float kNautiloidHeightMm = 128.5f;
constexpr float kNautiloidMaxFractalZoom = 5.f;

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
        module->fractalCenterX = clamp(module->fractalCenterX - delta.x / box.size.x * 2.f * halfSpan.x, -2.f, 2.f);
        module->fractalCenterY = clamp(module->fractalCenterY - delta.y / box.size.y * 2.f * halfSpan.y, -2.f, 2.f);
        if (nautiloidRequestDue(&lastPanRequestTime, 0.05)) {
          module->requestRender();
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
        module->requestRender();
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
    const float centerX = 0.5f * box.size.x;
    const float handleX = value * box.size.x;
    const float trackH = std::max(3.f, std::min(8.f, box.size.y * 0.34f));
    const float trackY = 0.5f * (box.size.y - trackH);
    const float radius = 0.5f * trackH;

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
      nvgBeginPath(args.vg);
      nvgRoundedRect(args.vg, fillLeft, trackY, fillW, trackH, radius);
      nvgFillColor(args.vg, zoomIn ? nvgRGB(79, 206, 184) : nvgRGB(228, 148, 80));
      nvgFill(args.vg);
    }

    nvgBeginPath(args.vg);
    nvgMoveTo(args.vg, centerX, trackY - 3.f);
    nvgLineTo(args.vg, centerX, trackY + trackH + 3.f);
    nvgStrokeWidth(args.vg, 1.25f);
    nvgStrokeColor(args.vg, nvgRGBA(232, 238, 246, 118));
    nvgStroke(args.vg);

    const float handleW = std::max(8.f, box.size.y * 0.42f);
    const float handleH = std::max(trackH + 6.f, box.size.y * 0.72f);
    const float handleY = 0.5f * (box.size.y - handleH);
    nvgBeginPath(args.vg);
    nvgRoundedRect(args.vg, handleX - 0.5f * handleW, handleY, handleW, handleH, 3.f);
    nvgFillColor(args.vg, nvgRGB(226, 232, 240));
    nvgFill(args.vg);
    nvgStrokeWidth(args.vg, 1.f);
    nvgStrokeColor(args.vg, nvgRGBA(20, 24, 30, 180));
    nvgStroke(args.vg);
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
    zoomSlider->box.size = mm2px(Vec(91.6f, 7.f));
    NautiloidZoomSpeedQuantity* zoomSpeed = new NautiloidZoomSpeedQuantity();
    zoomSlider->zoomSpeed = zoomSpeed;
    zoomSlider->quantity = zoomSpeed;
    addChild(zoomSlider);

    NautiloidSourceButton* sourceButton =
      createParamCentered<NautiloidSourceButton>(mm2px(Vec(36.f, 96.f)), module, Nautiloid::SOURCE_MENU_PARAM);
    sourceButton->module = module;
    addParam(sourceButton);

    NautiloidResetButton* resetButton =
      createParamCentered<NautiloidResetButton>(mm2px(Vec(65.6f, 96.f)), module, Nautiloid::RESET_VIEW_PARAM);
    resetButton->module = module;
    addParam(resetButton);
  }
};

Model* modelNautiloid = createModel<Nautiloid, NautiloidWidget>("Nautiloid");
