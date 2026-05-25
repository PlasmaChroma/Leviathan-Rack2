#include "TDScope.hpp"

#include <chrono>
#include <cstdio>

namespace {
constexpr double kDebugTerminalSubmitIntervalSec = 1.0 / 8.0;

struct TDScopeBrightnessQuantity final : Quantity {
  TDScope *module = nullptr;

  explicit TDScopeBrightnessQuantity(TDScope *module) : module(module) {}

  void setValue(float value) override {
    if (module) {
      module->scopeColorBrightness = clamp(value, 0.f, 1.f);
    }
  }

  float getValue() override {
    return module ? module->scopeColorBrightnessClamped() : 0.5f;
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
    return "Brightness";
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
};

const char *debugRenderModeLabel(const TDScope *scopeModule) {
  if (!scopeModule) {
    return "STD";
  }
  const int renderMode = scopeModule->debugRenderMode.load(std::memory_order_relaxed);
  switch (renderMode) {
    case TDScope::DEBUG_RENDER_STANDARD:
      return "STD";
    case TDScope::DEBUG_RENDER_TAIL_RASTER:
      return "RASTER";
    case TDScope::DEBUG_RENDER_OPENGL:
      return scopeModule->debugUseGlShaderRenderer.load(std::memory_order_relaxed) ? "GL SHDR" : "GL";
    default:
      return "STD";
  }
}
}

struct TDScopeWidget : ModuleWidget {
  PanelBorder *panelBorder = nullptr;
  Widget *glDisplay = nullptr;
  math::Rect scopeRectPx;
  static constexpr float kTopBarYmm = 9.522227f;
  static constexpr float kTopBarLeftStartMm = 2.2491839f;

  bool shouldRenderDockBridge() const {
    TDScope *scopeModule = static_cast<TDScope *>(module);
    if (!scopeModule) {
      return false;
    }
    return tdscope::isTemporalDeckModule(scopeModule->leftExpander.module) ||
           scopeModule->uiLinkActive.load(std::memory_order_relaxed);
  }

  TDScopeWidget(TDScope *module) {
    setModule(module);
    PreviewBuildLogTimer previewBuildTimer("TDScope", module);
    const std::string panelPath = asset::plugin(pluginInstance, "res/tdscope.svg");
    setPanel(createPanel(panelPath));
    previewBuildTimer.markPanelDone();
    if (auto *svgPanel = dynamic_cast<app::SvgPanel *>(getPanel())) {
      panelBorder = tdscope::findPanelBorder(svgPanel->fb);
    }

    math::Rect scopeRectMm;
    if (!panel_svg::loadRectFromSvgMm(panelPath, "scope", &scopeRectMm)) {
      scopeRectMm.pos = Vec(1.1138f, 10.9404f);
      scopeRectMm.size = Vec(38.5563f, 109.4206f);
    }
    scopeRectPx.pos = mm2px(scopeRectMm.pos);
    scopeRectPx.size = mm2px(scopeRectMm.size);
    previewBuildTimer.setAtlasStatus(panel_svg::getAtlasStatusLabelForSvg(panelPath));
    previewBuildTimer.markAnchorsDone();

    glDisplay = tdscope::createGlDisplay(module, scopeRectMm);
    glDisplay->setVisible(module && module->useOpenGlGeometryRenderMode());
    addChild(glDisplay);

    addChild(tdscope::createDisplay(module, scopeRectMm));
    addChild(tdscope::createInput(module, scopeRectMm));

    addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(Vec(3.2f, 5.8f)), module, TDScope::LINK_LIGHT));
    addChild(createLightCentered<SmallLight<GreenLight>>(mm2px(Vec(3.2f, 5.8f)), module, TDScope::PREVIEW_LIGHT));
  }

  void step() override {
    using PerfClock = std::chrono::steady_clock;
    const PerfClock::time_point stepStart = PerfClock::now();
    bool linkedToDeck = shouldRenderDockBridge();
    TDScope *scopeModule = static_cast<TDScope *>(module);
    if (glDisplay) {
      glDisplay->setVisible(scopeModule && scopeModule->useOpenGlGeometryRenderMode());
    }
    const float borderGrowPx = linkedToDeck ? 3.f : 0.f;
    if (panelBorder && (panelBorder->box.pos.x != -borderGrowPx || panelBorder->box.size.x != (box.size.x + borderGrowPx))) {
      panelBorder->box.pos.x = -borderGrowPx;
      panelBorder->box.size.x = box.size.x + borderGrowPx;
      if (auto *svgPanel = dynamic_cast<app::SvgPanel *>(getPanel())) {
        svgPanel->fb->dirty = true;
      }
    }
    ModuleWidget::step();
    if (scopeModule) {
      const float stepUs = float(std::chrono::duration_cast<std::chrono::nanoseconds>(
                                   PerfClock::now() - stepStart).count()) *
                           0.001f;
      const float prevStepUs = scopeModule->uiDebugModuleUiStepUsEma.load(std::memory_order_relaxed);
      const float emaStepUs = (prevStepUs > 0.f) ? (prevStepUs + (stepUs - prevStepUs) * 0.18f) : stepUs;
      scopeModule->uiDebugModuleUiStepUsEma.store(std::max(0.f, emaStepUs), std::memory_order_relaxed);
    }
  }

  void draw(const DrawArgs &args) override {
    using PerfClock = std::chrono::steady_clock;
    const PerfClock::time_point moduleDrawStart = PerfClock::now();
    bool linkedToDeck = shouldRenderDockBridge();
    if (linkedToDeck) {
      DrawArgs adjusted = args;
      adjusted.clipBox.pos.x -= mm2px(0.3f);
      adjusted.clipBox.size.x += mm2px(0.3f);
      ModuleWidget::draw(adjusted);

      float y = mm2px(kTopBarYmm);
      float x0 = 0.f;
      float x1 = mm2px(kTopBarLeftStartMm);
      if (x1 > x0 + 0.1f) {
        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, x0, y);
        nvgLineTo(args.vg, x1, y);
        nvgStrokeColor(args.vg, nvgRGBA(87, 64, 191, 255));
        nvgStrokeWidth(args.vg, mm2px(0.50f));
        nvgLineCap(args.vg, NVG_ROUND);
        nvgStroke(args.vg);
      }
    } else {
      ModuleWidget::draw(args);
    }

    TDScope *scopeModule = static_cast<TDScope *>(module);
    if (scopeModule && APP && APP->window && APP->window->uiFont) {
      const float modeX = scopeRectPx.pos.x + scopeRectPx.size.x - 1.2f;
      const float modeY = std::max(1.5f, mm2px(9.522227f) - mm2px(0.75f));
      const char *modeLabel = debugRenderModeLabel(scopeModule);
      nvgSave(args.vg);
      nvgFontFaceId(args.vg, APP->window->uiFont->handle);
      nvgFontSize(args.vg, 6.8f);
      nvgTextAlign(args.vg, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
      nvgFillColor(args.vg, nvgRGBA(8, 10, 14, 220));
      nvgText(args.vg, modeX + 0.45f, modeY + 0.45f, modeLabel, nullptr);
      nvgFillColor(args.vg, nvgRGBA(225, 232, 240, 230));
      nvgText(args.vg, modeX, modeY, modeLabel, nullptr);
      nvgRestore(args.vg);
    }

    if (scopeModule) {
      const float drawUs = float(std::chrono::duration_cast<std::chrono::nanoseconds>(
                                   PerfClock::now() - moduleDrawStart).count()) *
                           0.001f;
      const float prevUs = scopeModule->uiDebugModuleUiDrawUsEma.load(std::memory_order_relaxed);
      const float emaUs = (prevUs > 0.f) ? (prevUs + (drawUs - prevUs) * 0.18f) : drawUs;
      scopeModule->uiDebugModuleUiDrawUsEma.store(std::max(0.f, emaUs), std::memory_order_relaxed);
    }

    if (scopeModule && isDragonKingDebugEnabled()) {
      double nowSec = system::getTime();
      if (scopeModule->uiDebugTerminalLastSubmitSec < 0.0 ||
          (nowSec - scopeModule->uiDebugTerminalLastSubmitSec) >= kDebugTerminalSubmitIntervalSec) {
        scopeModule->uiDebugTerminalLastSubmitSec = nowSec;
        float uiDrawUsEma = scopeModule->uiDebugModuleUiDrawUsEma.load(std::memory_order_relaxed);
        float uiStepUsEma = scopeModule->uiDebugModuleUiStepUsEma.load(std::memory_order_relaxed);
        float densityPct = scopeModule->uiDebugScopeDensityPct.load(std::memory_order_relaxed);
        int densityRows = scopeModule->uiDebugScopeDensityRows.load(std::memory_order_relaxed);
        float rackZoom = scopeModule->uiDebugScopeRackZoom.load(std::memory_order_relaxed);
        float zoomThicknessMul = scopeModule->uiDebugScopeZoomThicknessMul.load(std::memory_order_relaxed);
        uint64_t publishSeq = scopeModule->uiLastPublishSeq.load(std::memory_order_relaxed);
        uint64_t drawSeq = scopeModule->uiDebugScopeDrawSeq.load(std::memory_order_relaxed);
        uint64_t drawCalls = scopeModule->uiDebugScopeDrawCalls.load(std::memory_order_relaxed);
        debug_terminal::submitTDScopeUiMetrics(scopeModule->debugInstanceId,
                                               (uiStepUsEma + uiDrawUsEma) * 0.001f,
                                               densityRows,
                                               densityPct,
                                               rackZoom,
                                               zoomThicknessMul,
                                               publishSeq,
                                               drawSeq,
                                               drawCalls);
      }
      if (APP && APP->window && APP->window->uiFont) {
        char debugIdLabel[32];
        std::snprintf(debugIdLabel, sizeof(debugIdLabel), "ID:%u", scopeModule->debugInstanceId);
        const float x = box.size.x - mm2px(0.9f);
        const float y = mm2px(2.5f);
        nvgSave(args.vg);
        nvgFontFaceId(args.vg, APP->window->uiFont->handle);
        nvgFontSize(args.vg, 6.8f);
        nvgTextAlign(args.vg, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
        nvgFillColor(args.vg, nvgRGBA(8, 10, 14, 210));
        nvgText(args.vg, x + 0.45f, y + 0.45f, debugIdLabel, nullptr);
        nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 230));
        nvgText(args.vg, x, y, debugIdLabel, nullptr);
        nvgRestore(args.vg);
      }
    }
  }

  void appendContextMenu(Menu *menu) override {
    ModuleWidget::appendContextMenu(menu);
    TDScope *scopeModule = dynamic_cast<TDScope *>(module);
    if (!scopeModule) {
      return;
    }

    auto addBrightnessSlider = [=](Menu *targetMenu) {
      auto *brightnessSlider = new ui::Slider();
      brightnessSlider->box.size = Vec(180.f, 24.f);
      brightnessSlider->quantity = new TDScopeBrightnessQuantity(scopeModule);
      targetMenu->addChild(brightnessSlider);
    };

    menu->addChild(new MenuSeparator());
    menu->addChild(createMenuLabel("Channel View"));
    menu->addChild(createCheckMenuItem(
      "Mono", "", [=]() { return scopeModule->scopeChannelMode == TDScope::SCOPE_CHANNEL_MONO; },
      [=]() { scopeModule->scopeChannelMode = TDScope::SCOPE_CHANNEL_MONO; }));
    menu->addChild(createCheckMenuItem(
      "Stereo (side-by-side)", "",
      [=]() { return scopeModule->scopeChannelMode == TDScope::SCOPE_CHANNEL_STEREO; },
      [=]() { scopeModule->scopeChannelMode = TDScope::SCOPE_CHANNEL_STEREO; }));
	menu->addChild(createCheckMenuItem(
      "Inverted Vertical", "", [=]() { return scopeModule->scopeVerticalInverted.load(std::memory_order_relaxed); },
      [=]() { scopeModule->scopeVerticalInverted.store(!scopeModule->scopeVerticalInverted.load(std::memory_order_relaxed), std::memory_order_relaxed); }));
    menu->addChild(createSubmenuItem("Scope Range", "", [=](Menu *submenu) {
      submenu->addChild(createCheckMenuItem(
        "Auto (window peak)", "",
        [=]() { return scopeModule->scopeDisplayRangeMode == TDScope::SCOPE_RANGE_AUTO; },
        [=]() { scopeModule->scopeDisplayRangeMode = TDScope::SCOPE_RANGE_AUTO; }));
      submenu->addChild(createCheckMenuItem(
        "+/-2.5V full width", "", [=]() { return scopeModule->scopeDisplayRangeMode == TDScope::SCOPE_RANGE_2V5; },
        [=]() { scopeModule->scopeDisplayRangeMode = TDScope::SCOPE_RANGE_2V5; }));
      submenu->addChild(createCheckMenuItem(
        "+/-5V full width", "", [=]() { return scopeModule->scopeDisplayRangeMode == TDScope::SCOPE_RANGE_5V; },
        [=]() { scopeModule->scopeDisplayRangeMode = TDScope::SCOPE_RANGE_5V; }));
      submenu->addChild(createCheckMenuItem(
        "+/-10V full width", "", [=]() { return scopeModule->scopeDisplayRangeMode == TDScope::SCOPE_RANGE_10V; },
        [=]() { scopeModule->scopeDisplayRangeMode = TDScope::SCOPE_RANGE_10V; }));
    }));

    menu->addChild(new MenuSeparator());
    menu->addChild(createSubmenuItem("Colors", "", [=](Menu *submenu) {
      submenu->addChild(createCheckMenuItem(
        "Default (Purple/Cyan)", "", [=]() { return scopeModule->scopeColorScheme == TDScope::COLOR_SCHEME_DEFAULT; },
        [=]() { scopeModule->scopeColorScheme = TDScope::COLOR_SCHEME_DEFAULT; }));
      submenu->addChild(createCheckMenuItem(
        "Classic (Green/Red)", "", [=]() { return scopeModule->scopeColorScheme == TDScope::COLOR_SCHEME_CLASSIC; },
        [=]() { scopeModule->scopeColorScheme = TDScope::COLOR_SCHEME_CLASSIC; }));
      submenu->addChild(createCheckMenuItem(
        "Monochrome (Gray/White)", "",
        [=]() { return scopeModule->scopeColorScheme == TDScope::COLOR_SCHEME_MONOCHROME; },
        [=]() { scopeModule->scopeColorScheme = TDScope::COLOR_SCHEME_MONOCHROME; }));
      submenu->addChild(createCheckMenuItem(
        "Fire (Red/Yellow)", "", [=]() { return scopeModule->scopeColorScheme == TDScope::COLOR_SCHEME_FIRE; },
        [=]() { scopeModule->scopeColorScheme = TDScope::COLOR_SCHEME_FIRE; }));
    }));
    addBrightnessSlider(menu);

    menu->addChild(new MenuSeparator());
    menu->addChild(createSubmenuItem("Debug Render", "", [=](Menu *submenu) {
      submenu->addChild(createMenuLabel("Scope Rate"));
      submenu->addChild(createCheckMenuItem(
        "120 Hz", "", [=]() { return scopeModule->debugUiPublishRateMode == TDScope::DEBUG_UI_PUBLISH_120HZ; },
        [=]() { scopeModule->debugUiPublishRateMode = TDScope::DEBUG_UI_PUBLISH_120HZ; }));
      submenu->addChild(createCheckMenuItem(
        "60 Hz", "", [=]() { return scopeModule->debugUiPublishRateMode == TDScope::DEBUG_UI_PUBLISH_60HZ; },
        [=]() { scopeModule->debugUiPublishRateMode = TDScope::DEBUG_UI_PUBLISH_60HZ; }));
      submenu->addChild(createCheckMenuItem(
        "30 Hz", "", [=]() { return scopeModule->debugUiPublishRateMode == TDScope::DEBUG_UI_PUBLISH_30HZ; },
        [=]() { scopeModule->debugUiPublishRateMode = TDScope::DEBUG_UI_PUBLISH_30HZ; }));
      submenu->addChild(new MenuSeparator());
      submenu->addChild(createCheckMenuItem(
        "Framebuffer cache", "", [=]() { return scopeModule->debugFramebufferCacheEnabled.load(std::memory_order_relaxed); },
        [=]() { scopeModule->debugFramebufferCacheEnabled.store(!scopeModule->debugFramebufferCacheEnabled.load(std::memory_order_relaxed), std::memory_order_relaxed); }));
      submenu->addChild(createMenuLabel("Render Mode"));
      submenu->addChild(createCheckMenuItem(
        "Standard", "", [=]() { return scopeModule->debugRenderMode == TDScope::DEBUG_RENDER_STANDARD; },
        [=]() { scopeModule->debugRenderMode = TDScope::DEBUG_RENDER_STANDARD; }));
      submenu->addChild(createCheckMenuItem(
        "Tail raster", "", [=]() { return scopeModule->debugRenderMode == TDScope::DEBUG_RENDER_TAIL_RASTER; },
        [=]() { scopeModule->debugRenderMode = TDScope::DEBUG_RENDER_TAIL_RASTER; }));
      submenu->addChild(createCheckMenuItem(
        "OpenGL", "",
        [=]() {
          return scopeModule->debugRenderMode == TDScope::DEBUG_RENDER_OPENGL &&
                 !scopeModule->debugUseGlShaderRenderer.load(std::memory_order_relaxed);
        },
        [=]() {
          scopeModule->debugRenderMode = TDScope::DEBUG_RENDER_OPENGL;
          scopeModule->debugUseGlShaderRenderer.store(false, std::memory_order_relaxed);
        }));
      submenu->addChild(createCheckMenuItem(
        "OpenGL SHDR", "",
        [=]() {
          return scopeModule->debugRenderMode == TDScope::DEBUG_RENDER_OPENGL &&
                 scopeModule->debugUseGlShaderRenderer.load(std::memory_order_relaxed);
        },
        [=]() {
          scopeModule->debugRenderMode = TDScope::DEBUG_RENDER_OPENGL;
          scopeModule->debugUseGlShaderRenderer.store(true, std::memory_order_relaxed);
        }));
    }));
  }
};

Model *modelTDScope = createModel<TDScope, TDScopeWidget>("TDScope");
