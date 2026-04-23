#include "TDScope.hpp"

#include <cstdio>

namespace {
constexpr double kDebugTerminalSubmitIntervalSec = 1.0 / 8.0;
}

struct TDScopeWidget : ModuleWidget {
  PanelBorder *panelBorder = nullptr;
  Widget *glDisplay = nullptr;
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
    const std::string panelPath = asset::plugin(pluginInstance, "res/tdscope.svg");
    setPanel(createPanel(panelPath));
    if (auto *svgPanel = dynamic_cast<app::SvgPanel *>(getPanel())) {
      panelBorder = tdscope::findPanelBorder(svgPanel->fb);
    }

    math::Rect scopeRectMm;
    if (!panel_svg::loadRectFromSvgMm(panelPath, "scope", &scopeRectMm)) {
      scopeRectMm.pos = Vec(1.1138f, 10.9404f);
      scopeRectMm.size = Vec(38.5563f, 109.4206f);
    }

    glDisplay = tdscope::createGlDisplay(module, scopeRectMm);
    glDisplay->setVisible(module && module->useOpenGlGeometryRenderMode());
    addChild(glDisplay);

    addChild(tdscope::createDisplay(module, scopeRectMm));
    addChild(tdscope::createInput(module, scopeRectMm));

    addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(Vec(3.2f, 5.8f)), module, TDScope::LINK_LIGHT));
    addChild(createLightCentered<SmallLight<GreenLight>>(mm2px(Vec(3.2f, 5.8f)), module, TDScope::PREVIEW_LIGHT));
  }

  void step() override {
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
  }

  void draw(const DrawArgs &args) override {
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
    if (scopeModule && isDragonKingDebugEnabled()) {
      double nowSec = system::getTime();
      if (scopeModule->uiDebugTerminalLastSubmitSec < 0.0 ||
          (nowSec - scopeModule->uiDebugTerminalLastSubmitSec) >= kDebugTerminalSubmitIntervalSec) {
        scopeModule->uiDebugTerminalLastSubmitSec = nowSec;
        float uiDrawUsEma = scopeModule->uiDebugScopeUiDrawUsEma.load(std::memory_order_relaxed);
        float densityPct = scopeModule->uiDebugScopeDensityPct.load(std::memory_order_relaxed);
        int densityRows = scopeModule->uiDebugScopeDensityRows.load(std::memory_order_relaxed);
        float rackZoom = scopeModule->uiDebugScopeRackZoom.load(std::memory_order_relaxed);
        float zoomThicknessMul = scopeModule->uiDebugScopeZoomThicknessMul.load(std::memory_order_relaxed);
        uint64_t publishSeq = scopeModule->uiLastPublishSeq.load(std::memory_order_relaxed);
        uint64_t drawSeq = scopeModule->uiDebugScopeDrawSeq.load(std::memory_order_relaxed);
        uint64_t drawCalls = scopeModule->uiDebugScopeDrawCalls.load(std::memory_order_relaxed);
        debug_terminal::submitTDScopeUiMetrics(scopeModule->debugInstanceId,
                                               uiDrawUsEma * 0.001f,
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

    menu->addChild(new MenuSeparator());
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
    menu->addChild(createMenuLabel("Channel View"));
    menu->addChild(createCheckMenuItem(
      "Mono", "", [=]() { return scopeModule->scopeChannelMode == TDScope::SCOPE_CHANNEL_MONO; },
      [=]() { scopeModule->scopeChannelMode = TDScope::SCOPE_CHANNEL_MONO; }));
    menu->addChild(createCheckMenuItem(
      "Stereo (side-by-side)", "",
      [=]() { return scopeModule->scopeChannelMode == TDScope::SCOPE_CHANNEL_STEREO; },
      [=]() { scopeModule->scopeChannelMode = TDScope::SCOPE_CHANNEL_STEREO; }));

    menu->addChild(new MenuSeparator());
    menu->addChild(createSubmenuItem("Colors", "", [=](Menu *submenu) {
      submenu->addChild(createCheckMenuItem(
        "Temporal Deck", "", [=]() { return scopeModule->scopeColorScheme == TDScope::COLOR_SCHEME_TEMPORAL_DECK; },
        [=]() { scopeModule->scopeColorScheme = TDScope::COLOR_SCHEME_TEMPORAL_DECK; }));
      submenu->addChild(createCheckMenuItem(
        "Leviathan", "", [=]() { return scopeModule->scopeColorScheme == TDScope::COLOR_SCHEME_LEVIATHAN; },
        [=]() { scopeModule->scopeColorScheme = TDScope::COLOR_SCHEME_LEVIATHAN; }));
      submenu->addChild(createCheckMenuItem(
        "Pickle", "", [=]() { return scopeModule->scopeColorScheme == TDScope::COLOR_SCHEME_PICKLE; },
        [=]() { scopeModule->scopeColorScheme = TDScope::COLOR_SCHEME_PICKLE; }));
      submenu->addChild(createCheckMenuItem(
        "Hellfire", "", [=]() { return scopeModule->scopeColorScheme == TDScope::COLOR_SCHEME_HELLFIRE; },
        [=]() { scopeModule->scopeColorScheme = TDScope::COLOR_SCHEME_HELLFIRE; }));
      submenu->addChild(createCheckMenuItem(
        "Angelic", "", [=]() { return scopeModule->scopeColorScheme == TDScope::COLOR_SCHEME_ANGELIC; },
        [=]() { scopeModule->scopeColorScheme = TDScope::COLOR_SCHEME_ANGELIC; }));
      submenu->addChild(createCheckMenuItem(
        "Violet Flame", "", [=]() { return scopeModule->scopeColorScheme == TDScope::COLOR_SCHEME_VIOLET_FLAME; },
        [=]() { scopeModule->scopeColorScheme = TDScope::COLOR_SCHEME_VIOLET_FLAME; }));
      submenu->addChild(createCheckMenuItem(
        "Pixie", "", [=]() { return scopeModule->scopeColorScheme == TDScope::COLOR_SCHEME_PIXIE; },
        [=]() { scopeModule->scopeColorScheme = TDScope::COLOR_SCHEME_PIXIE; }));
      submenu->addChild(createCheckMenuItem(
        "Wasp", "", [=]() { return scopeModule->scopeColorScheme == TDScope::COLOR_SCHEME_WASP; },
        [=]() { scopeModule->scopeColorScheme = TDScope::COLOR_SCHEME_WASP; }));
      submenu->addChild(createCheckMenuItem(
        "Emerald", "", [=]() { return scopeModule->scopeColorScheme == TDScope::COLOR_SCHEME_EMERALD; },
        [=]() { scopeModule->scopeColorScheme = TDScope::COLOR_SCHEME_EMERALD; }));
    }));
    menu->addChild(createCheckMenuItem(
      "Transient halo", "", [=]() { return scopeModule->scopeTransientHaloEnabled; },
      [=]() { scopeModule->scopeTransientHaloEnabled = !scopeModule->scopeTransientHaloEnabled; }));

    if (isDragonKingDebugEnabled()) {
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
          "Framebuffer cache", "", [=]() { return scopeModule->debugFramebufferCacheEnabled; },
          [=]() { scopeModule->debugFramebufferCacheEnabled = !scopeModule->debugFramebufferCacheEnabled; }));
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
          [=]() { scopeModule->debugRenderMode = TDScope::DEBUG_RENDER_OPENGL; }));
        submenu->addChild(new MenuSeparator());
        submenu->addChild(createCheckMenuItem(
          "Main trace", "", [=]() { return scopeModule->debugRenderMainTraceEnabled; },
          [=]() { scopeModule->debugRenderMainTraceEnabled = !scopeModule->debugRenderMainTraceEnabled; }));
        submenu->addChild(createCheckMenuItem(
          "Connectors", "", [=]() { return scopeModule->debugRenderConnectorsEnabled; },
          [=]() { scopeModule->debugRenderConnectorsEnabled = !scopeModule->debugRenderConnectorsEnabled; }));
        submenu->addChild(createCheckMenuItem(
          "Stereo right lane", "", [=]() { return scopeModule->debugRenderStereoRightLaneEnabled; },
          [=]() { scopeModule->debugRenderStereoRightLaneEnabled = !scopeModule->debugRenderStereoRightLaneEnabled; }));
      }));
    }
  }
};

Model *modelTDScope = createModel<TDScope, TDScopeWidget>("TDScope");
