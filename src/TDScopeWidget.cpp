#include "TDScope.hpp"
#include "VisualAssets.hpp"

#include <chrono>
#include <cstdio>

namespace {
constexpr double kDebugTerminalSubmitIntervalSec = debug_terminal::kTimingRangeSubmitIntervalSec;

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
      return "GL";
    case TDScope::DEBUG_RENDER_OPENGL_SHDR:
      return "GL SHDR";
    default:
      return "STD";
  }
}

struct FittedSvgWidget final : TransparentWidget {
  std::shared_ptr<window::Svg> svg;

  void setSvg(std::shared_ptr<window::Svg> svg) {
    this->svg = svg;
  }

  void draw(const DrawArgs &args) override {
    if (!svg || !svg->handle || box.size.x <= 0.f || box.size.y <= 0.f) {
      return;
    }
    const Vec svgSize = svg->getSize();
    if (svgSize.x <= 0.f || svgSize.y <= 0.f) {
      return;
    }

    nvgSave(args.vg);
    nvgScale(args.vg, box.size.x / svgSize.x, box.size.y / svgSize.y);
    svg->draw(args.vg);
    nvgRestore(args.vg);
  }
};

struct UnpairedStatusWidget final : TransparentWidget {
  void draw(const DrawArgs &args) override {
    if (!APP || !APP->window || !APP->window->uiFont) {
      return;
    }

    const float centerX = box.size.x * 0.5f;
    const float bottomY = box.size.y - 7.f;
    const float lineGap = 9.f;

    nvgSave(args.vg);
    nvgFontFaceId(args.vg, APP->window->uiFont->handle);
    nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);

    nvgFontSize(args.vg, 12.f);
    nvgFillColor(args.vg, nvgRGBA(150, 176, 190, 220));
    nvgText(args.vg, centerX, bottomY - 2.f * lineGap, "Attach to", nullptr);

    nvgFontSize(args.vg, 14.f);
    nvgFillColor(args.vg, nvgRGBA(224, 238, 244, 236));
    nvgText(args.vg, centerX, bottomY - lineGap + 2.f, "Temporal Deck", nullptr);
    nvgRestore(args.vg);
  }
};
}

struct TDScopeWidget : ModuleWidget {
  PanelBorder *panelBorder = nullptr;
  Widget *glDisplay = nullptr;
  Widget *standardDisplay = nullptr;
  Widget *input = nullptr;
  Widget *unpairedDragon = nullptr;
  Widget *unpairedStatus = nullptr;
  math::Rect scopeRectPx;
  debug_terminal::UiTimingRangeAccumulator uiStepUsRange;
  debug_terminal::UiTimingRangeAccumulator uiDrawUsRange;
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

  bool isPairedToTemporalDeck() const {
    TDScope *scopeModule = static_cast<TDScope *>(module);
    return scopeModule && tdscope::isTemporalDeckModule(scopeModule->leftExpander.module);
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

    const bool initialPairedToDeck = isPairedToTemporalDeck();

    glDisplay = tdscope::createGlDisplay(module, scopeRectMm);
    glDisplay->setVisible(initialPairedToDeck && module && module->useOpenGlGeometryRenderMode());
    addChild(glDisplay);

    standardDisplay = tdscope::createDisplay(module, scopeRectMm);
    standardDisplay->setVisible(initialPairedToDeck);
    addChild(standardDisplay);
    input = tdscope::createInput(module, scopeRectMm);
    input->setVisible(initialPairedToDeck);
    addChild(input);

    math::Rect dragonRectMm;
    if (!panel_svg::loadRectFromSvgMm(panelPath, "DRAGON_RENDER_AREA", &dragonRectMm)) {
      dragonRectMm.pos = Vec(1.72215f, 25.0f);
      dragonRectMm.size = Vec(37.3422f, 76.2759f);
    }
    auto *dragon = new FittedSvgWidget;
    dragon->setSvg(visual_assets::loadPluginSvgCached("res/icon/Leviathan_Optimized.svg"));
    auto *dragonFb = new widget::FramebufferWidget;
    dragonFb->box.pos = mm2px(dragonRectMm.pos);
    dragonFb->box.size = mm2px(dragonRectMm.size);
    dragonFb->dirtyOnSubpixelChange = false;
    dragon->box.size = dragonFb->box.size;
    dragonFb->addChild(dragon);
    dragonFb->setVisible(!initialPairedToDeck);
    unpairedDragon = dragonFb;
    addChild(unpairedDragon);

    auto *status = new UnpairedStatusWidget;
    status->box.pos = mm2px(scopeRectMm.pos);
    status->box.size = mm2px(scopeRectMm.size);
    status->setVisible(!initialPairedToDeck);
    unpairedStatus = status;
    addChild(unpairedStatus);

    addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(Vec(3.2f, 5.8f)), module, TDScope::LINK_LIGHT));
    addChild(createLightCentered<SmallLight<GreenLight>>(mm2px(Vec(3.2f, 5.8f)), module, TDScope::PREVIEW_LIGHT));
  }

  void step() override {
    using PerfClock = std::chrono::steady_clock;
    const bool measurePerf = isDragonKingDebugEnabled();
    const PerfClock::time_point stepStart = measurePerf ? PerfClock::now() : PerfClock::time_point();
    bool linkedToDeck = shouldRenderDockBridge();
    bool pairedToDeck = isPairedToTemporalDeck();
    TDScope *scopeModule = static_cast<TDScope *>(module);
    if (glDisplay) {
      glDisplay->setVisible(pairedToDeck && scopeModule && scopeModule->useOpenGlGeometryRenderMode());
    }
    if (standardDisplay) {
      standardDisplay->setVisible(pairedToDeck);
    }
    if (input) {
      input->setVisible(pairedToDeck);
    }
    if (unpairedDragon) {
      unpairedDragon->setVisible(!pairedToDeck);
    }
    if (unpairedStatus) {
      unpairedStatus->setVisible(!pairedToDeck);
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
    if (scopeModule && measurePerf) {
      const float stepUs = float(std::chrono::duration_cast<std::chrono::nanoseconds>(
                                   PerfClock::now() - stepStart).count()) *
                           0.001f;
      const float prevStepUs = scopeModule->uiDebugModuleUiStepUsEma.load(std::memory_order_relaxed);
      const float emaStepUs = (prevStepUs > 0.f) ? (prevStepUs + (stepUs - prevStepUs) * 0.18f) : stepUs;
      scopeModule->uiDebugModuleUiStepUsEma.store(std::max(0.f, emaStepUs), std::memory_order_relaxed);
      uiStepUsRange.add(stepUs);
    }
  }

  void draw(const DrawArgs &args) override {
    using PerfClock = std::chrono::steady_clock;
    const bool measurePerf = isDragonKingDebugEnabled();
    const PerfClock::time_point moduleDrawStart = measurePerf ? PerfClock::now() : PerfClock::time_point();
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
    if (scopeModule && isDragonKingDebugEnabled() && APP && APP->window && APP->window->uiFont) {
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

    if (scopeModule && measurePerf) {
      const float drawUs = float(std::chrono::duration_cast<std::chrono::nanoseconds>(
                                   PerfClock::now() - moduleDrawStart).count()) *
                           0.001f;
      const float prevUs = scopeModule->uiDebugModuleUiDrawUsEma.load(std::memory_order_relaxed);
      const float emaUs = (prevUs > 0.f) ? (prevUs + (drawUs - prevUs) * 0.18f) : drawUs;
      scopeModule->uiDebugModuleUiDrawUsEma.store(std::max(0.f, emaUs), std::memory_order_relaxed);
      uiDrawUsRange.add(drawUs);
    }

    if (scopeModule && isDragonKingDebugEnabled()) {
      double nowSec = system::getTime();
      if (scopeModule->uiDebugTerminalLastSubmitSec < 0.0 ||
          (nowSec - scopeModule->uiDebugTerminalLastSubmitSec) >= kDebugTerminalSubmitIntervalSec) {
        scopeModule->uiDebugTerminalLastSubmitSec = nowSec;
        float densityPct = scopeModule->uiDebugScopeDensityPct.load(std::memory_order_relaxed);
        int densityRows = scopeModule->uiDebugScopeDensityRows.load(std::memory_order_relaxed);
        float rackZoom = scopeModule->uiDebugScopeRackZoom.load(std::memory_order_relaxed);
        float zoomThicknessMul = scopeModule->uiDebugScopeZoomThicknessMul.load(std::memory_order_relaxed);
        uint64_t publishSeq = scopeModule->uiLastPublishSeq.load(std::memory_order_relaxed);
        uint64_t drawSeq = scopeModule->uiDebugScopeDrawSeq.load(std::memory_order_relaxed);
        uint64_t drawCalls = scopeModule->uiDebugScopeDrawCalls.load(std::memory_order_relaxed);
        debug_terminal::submitTDScopeUiMetrics(scopeModule->debugInstanceId,
                                               debug_terminal::consumeAudioProcessTiming(scopeModule->perfAudioProcessMinNs,
                                                                                         scopeModule->perfAudioProcessMaxNs),
                                               uiStepUsRange.consume(),
                                               uiDrawUsRange.consume(),
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
      submenu->addChild(createCheckMenuItem(
        "Retro Amber", "", [=]() { return scopeModule->scopeColorScheme == TDScope::COLOR_SCHEME_AMBER; },
        [=]() { scopeModule->scopeColorScheme = TDScope::COLOR_SCHEME_AMBER; }));
      submenu->addChild(createCheckMenuItem(
        "Retro Green", "",
        [=]() { return scopeModule->scopeColorScheme == TDScope::COLOR_SCHEME_GREEN_PHOSPHOR; },
        [=]() { scopeModule->scopeColorScheme = TDScope::COLOR_SCHEME_GREEN_PHOSPHOR; }));
    }));
    addBrightnessSlider(menu);

    if (isDragonKingDebugEnabled()) {
      menu->addChild(new MenuSeparator());
      menu->addChild(createSubmenuItem("Debug Render", "", [=](Menu *submenu) {
        submenu->addChild(createMenuLabel("Scope Rate"));
        submenu->addChild(createCheckMenuItem(
          "90 Hz", "", [=]() { return scopeModule->debugUiPublishRateMode == TDScope::DEBUG_UI_PUBLISH_90HZ; },
          [=]() { scopeModule->debugUiPublishRateMode = TDScope::DEBUG_UI_PUBLISH_90HZ; }));
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
          [=]() { return scopeModule->debugRenderMode == TDScope::DEBUG_RENDER_OPENGL; },
          [=]() {
            scopeModule->debugRenderMode = TDScope::DEBUG_RENDER_OPENGL;
          }));
        submenu->addChild(createCheckMenuItem(
          "OpenGL SHDR", "",
          [=]() { return scopeModule->debugRenderMode == TDScope::DEBUG_RENDER_OPENGL_SHDR; },
          [=]() {
            scopeModule->debugRenderMode = TDScope::DEBUG_RENDER_OPENGL_SHDR;
          }));
      }));
    }
  }
};

Model *modelTDScope = createModel<TDScope, TDScopeWidget>("TDScope");
