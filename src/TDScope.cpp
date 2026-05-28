#include "TDScope.hpp"
#include "TDScopeShared.hpp"
#include "NvgGraphicsLifecycle.hpp"
#include <nanovg_gl.h>

#include <chrono>
#include <cstdio>
#include <cstring>
#include <functional>
#include <limits>
#include <vector>

namespace tdscope {

std::atomic<uint32_t> gTDScopeDebugInstanceCounter {1u};

float computeScopeDisplayVerticalSupersample(float rackZoom) {
  rackZoom = std::max(rackZoom, 1e-4f);
  (void) rackZoom;
  // Fixed-density path: keep row count stable across zoom levels for now.
  // We can reintroduce adaptive supersampling later if profiling justifies it.
  return 0.55f;
}

PanelBorder *findPanelBorder(Widget *widget) {
  if (!widget) {
    return nullptr;
  }
  for (Widget *child : widget->children) {
    if (auto *border = dynamic_cast<PanelBorder *>(child)) {
      return border;
    }
  }
  return nullptr;
}

bool isTemporalDeckModule(const engine::Module *neighbor) {
  if (!neighbor || !neighbor->model) {
    return false;
  }
  // Legacy fallback for older host states where pointer identity may miss
  // but stable slug matching still identifies Temporal Deck.
  return (neighbor->model == modelTemporalDeck) || (neighbor->model->slug == "TemporalDeck");
}

} // namespace tdscope

struct TDScopeDisplayWidget final : Widget {
  // Scope is an input proxy for platter drag. Define drag travel directly in
  // platter turns so full-height swipes have stable, deck-equivalent meaning.
  // Scale factor applied to nominal 1-turn-per-full-height scope drag.
  // 1.0 = one nominal platter turn across full scope height.
  static constexpr float kScopeDragNominalTurnScale = 0.30f;
  static constexpr float kNominalPlatterRpm = 33.3333f;

  struct LiveScopeBucket {
    float minNorm = 0.f;
    float maxNorm = 0.f;
    float coveredSamples = 0.f;
    float totalSamples = 1.f;
    bool hasData = false;
    int64_t key = std::numeric_limits<int64_t>::min();
    uint64_t updateSeq = 0;
  };

  TDScope *module = nullptr;
  widget::FramebufferWidget *framebuffer = nullptr;
  tdscope::ScopeAutoScaleState autoScaleState;
  std::vector<float> rowX0;
  std::vector<float> rowX1;
  std::vector<float> rowX0Right;
  std::vector<float> rowX1Right;
  std::vector<float> rowVisualIntensity;
  std::vector<float> rowVisualIntensityRight;
  std::vector<float> rowColorDrive;
  std::vector<float> rowColorDriveRight;
  std::vector<float> rowY;
  std::vector<uint8_t> rowValid;
  std::vector<uint8_t> rowValidRight;
  std::vector<uint8_t> rowHoldFrames;
  std::vector<uint8_t> rowHoldFramesRight;
  temporaldeck_expander::HostToDisplay lastGoodMsg;
  bool hasLastGoodMsg = false;
  double lastGoodMsgTimeSec = -1.0;
  uint64_t cachedPublishSeq = 0;
  int cachedRowCount = 0;
  int cachedRangeMode = -1;
  bool cachedVerticalInverted = false;
  bool cachedStereoLayout = false;
  bool cachedGeometryValid = false;
  std::vector<float> historyX0;
  std::vector<float> historyX1;
  std::vector<float> historyX0Right;
  std::vector<float> historyX1Right;
  std::vector<float> historyVisualIntensity;
  std::vector<float> historyVisualIntensityRight;
  std::vector<uint8_t> historyValid;
  std::vector<uint8_t> historyValidRight;
  std::vector<uint8_t> historyHoldFrames;
  std::vector<uint8_t> historyHoldFramesRight;
  int historyCapacityRows = 0;
  int historyMarginRows = 0;
  int historyRowCount = 0;
  bool historyStereoLayout = false;
  bool historyValidState = false;
  float historyRowSpanSamples = 0.f;
  float historyReferenceWindowTopLag = 0.f;
  float historyReferenceWindowBottomLag = 0.f;
  float historyNewestPosSamples = 0.f;
  float historyShiftResidualRows = 0.f;
  int historyHeadRow = 0;
  std::vector<LiveScopeBucket> liveBucketsLeft;
  std::vector<LiveScopeBucket> liveBucketsRight;
  bool liveBucketsInitialized = false;
  bool liveBucketsStereo = false;
  int liveBucketCount = 0;
  float liveBucketSpanSamples = 0.f;
  uint64_t liveBucketLastPublishSeq = 0;
  uint64_t redrawLastPublishSeq = 0;
  bool redrawHasFreshFrame = false;
  bool redrawLastLinkActive = false;
  bool redrawLastPreviewValid = false;
  float redrawLastRackZoom = 1.f;
  int redrawLastRangeMode = -1;
  bool redrawLastVerticalInverted = false;
  int redrawLastChannelMode = -1;
  int redrawLastColorScheme = -1;
  int redrawLastRenderMode = -1;
  int tailRasterImage = -1;
  NVGcontext* tailRasterImageVg = nullptr;
  int tailRasterW = 0;
  int tailRasterH = 0;
  std::vector<uint8_t> tailRasterPixels;
  bool tailRasterValid = false;
  bool tailRasterStereo = false;
  float tailRasterWindowTopLag = 0.f;
  float tailRasterWindowBottomLag = 0.f;
  float tailRasterNewestPosSamples = 0.f;
  float tailRasterShiftResidualPx = 0.f;
  uint64_t tailRasterPublishSeq = 0;

  void resetTailRasterImage(NVGcontext* vg, bool deleteCurrentHandle) {
    nvg_gfx_lifecycle::resetOwnedNvgImage(
      tailRasterImageVg,
      tailRasterImage,
      tailRasterW,
      tailRasterH,
      vg,
      deleteCurrentHandle
    );
    tailRasterPixels.clear();
    tailRasterValid = false;
    tailRasterShiftResidualPx = 0.f;
  }

  ~TDScopeDisplayWidget() override {
    if (APP && APP->window && APP->window->vg) {
      resetTailRasterImage(APP->window->vg, true);
      return;
    }
    resetTailRasterImage(nullptr, false);
  }

  struct ScopeWindowMap {
    float drawTop = 0.f;
    float drawBottom = 1.f;
    float drawYDen = 1.f;
    float windowTopLag = 0.f;
    float windowBottomLag = 0.f;
    float accessibleLag = 0.f;
    bool valid = false;
  };

  ScopeWindowMap buildScopeWindowMap(const temporaldeck_expander::HostToDisplay &msg) const {
    ScopeWindowMap map;
    tdscope::ScopeWindowLagSpan span = tdscope::computeScopeWindowLagSpan(msg);
    map.windowTopLag = span.windowTopLag;
    map.windowBottomLag = span.windowBottomLag;
    map.accessibleLag = std::max(0.f, msg.accessibleLagSamples);
    float yInset = 0.75f;
    map.drawTop = yInset;
    map.drawBottom = std::max(map.drawTop + 1.f, box.size.y - yInset);
    // Drag mapping is continuous cursor motion, so use continuous draw height
    // rather than row-center spacing.
    map.drawYDen = std::max(map.drawBottom - map.drawTop, 1.f);
    map.valid = map.drawBottom > map.drawTop;
    return map;
  }

  float currentRackZoom() const {
    if (APP && APP->scene && APP->scene->rackScroll) {
      return std::max(APP->scene->rackScroll->getZoom(), 1e-4f);
    }
    return 1.f;
  }

  NVGcolor silWaveformLowColor() const {
    float lowR = 122.f, lowG = 92.f, lowB = 255.f;
    if (module) {
      const int colorScheme = module->scopeColorScheme.load(std::memory_order_relaxed);
      switch (colorScheme) {
        case TDScope::COLOR_SCHEME_CLASSIC: lowR = 0.f; lowG = 255.f; lowB = 65.f; break;
        case TDScope::COLOR_SCHEME_MONOCHROME: lowR = 92.f; lowG = 92.f; lowB = 92.f; break;
        case TDScope::COLOR_SCHEME_FIRE: lowR = 140.f; lowG = 0.f; lowB = 0.f; break;
        default: break;
      }
    }
    NVGcolor c = nvgRGBAf(lowR / 255.f, lowG / 255.f, lowB / 255.f, 1.f);
    if (module) c = module->applyScopeColorBrightness(c);
    return c;
  }

  NVGcolor silWaveformHighColor() const {
    float highR = 28.f, highG = 204.f, highB = 217.f;
    if (module) {
      const int colorScheme = module->scopeColorScheme.load(std::memory_order_relaxed);
      switch (colorScheme) {
        case TDScope::COLOR_SCHEME_CLASSIC: highR = 255.f; highG = 15.f; highB = 5.f; break;
        case TDScope::COLOR_SCHEME_MONOCHROME: highR = 242.f; highG = 242.f; highB = 242.f; break;
        case TDScope::COLOR_SCHEME_FIRE: highR = 255.f; highG = 255.f; highB = 30.f; break;
        default: break;
      }
    }
    NVGcolor c = nvgRGBAf(highR / 255.f, highG / 255.f, highB / 255.f, 1.f);
    if (module) c = module->applyScopeColorBrightness(c);
    return c;
  }

  void drawSilWaveformBackground(NVGcontext *vg, bool renderStereo, float laneWidth, float laneGap, float drawTop, float drawBottom) const {
    const float xInset = 0.75f;
    const float bgX = xInset;
    const float bgY = drawTop;
    const float bgW = std::max(0.f, box.size.x - 2.f * xInset);
    const float bgH = std::max(0.f, drawBottom - drawTop);
    nvgBeginPath(vg);
    nvgRect(vg, bgX, bgY, bgW, bgH);
    nvgFillColor(vg, nvgRGBA(0, 0, 0, 255));
    nvgFill(vg);

    if (renderStereo) {
      const float dividerX = laneWidth + laneGap * 0.5f;
      nvgBeginPath(vg);
      nvgMoveTo(vg, dividerX, drawTop);
      nvgLineTo(vg, dividerX, drawBottom);
      NVGcolor divC = silWaveformLowColor();
      divC.a = 0.25f;
      nvgStrokeColor(vg, divC);
      nvgStrokeWidth(vg, 0.5f);
      nvgStroke(vg);
    }
  }

  void step() override {
    Widget::step();
    if (!framebuffer) {
      return;
    }
    bool lagDragActive = module && module->uiLagDragActive.load(std::memory_order_relaxed);
    if (!module || !module->debugFramebufferCacheEnabled) {
      framebuffer->setDirty();
      return;
    }

    bool dirty = false;
    bool linkActive = module && module->uiLinkActive.load(std::memory_order_relaxed);
    bool previewValid = module && module->uiPreviewValid.load(std::memory_order_relaxed);
    if (linkActive != redrawLastLinkActive || previewValid != redrawLastPreviewValid) {
      dirty = true;
      redrawLastLinkActive = linkActive;
      redrawLastPreviewValid = previewValid;
    }

    float rackZoom = currentRackZoom();
    if (std::fabs(rackZoom - redrawLastRackZoom) > 0.01f) {
      dirty = true;
      redrawLastRackZoom = rackZoom;
    }

    if (module) {
      const int scopeDisplayRangeMode = module->scopeDisplayRangeMode.load(std::memory_order_relaxed);
      const bool scopeVerticalInverted = module->scopeVerticalInverted.load(std::memory_order_relaxed);
      const int scopeChannelMode = module->scopeChannelMode.load(std::memory_order_relaxed);
      const int scopeColorScheme = module->scopeColorScheme.load(std::memory_order_relaxed);
      const int debugRenderMode = module->debugRenderMode.load(std::memory_order_relaxed);
      if (scopeDisplayRangeMode != redrawLastRangeMode) {
        dirty = true;
        redrawLastRangeMode = scopeDisplayRangeMode;
      }
      if (scopeVerticalInverted != redrawLastVerticalInverted) {
        dirty = true;
        redrawLastVerticalInverted = scopeVerticalInverted;
      }
      if (scopeChannelMode != redrawLastChannelMode) {
        dirty = true;
        redrawLastChannelMode = scopeChannelMode;
      }
      if (scopeColorScheme != redrawLastColorScheme) {
        dirty = true;
        redrawLastColorScheme = scopeColorScheme;
      }
      if (debugRenderMode != redrawLastRenderMode) {
        dirty = true;
        redrawLastRenderMode = debugRenderMode;
        tailRasterValid = false;
        tailRasterShiftResidualPx = 0.f;
        historyValidState = false;
        historyShiftResidualRows = 0.f;
      }
    }

    temporaldeck_expander::HostToDisplay msg;
    bool snapshotOk = module && module->readSnapshotForUi(&msg);
    bool hasFreshFrame = snapshotOk && linkActive && previewValid;
    if (hasFreshFrame) {
      if (!redrawHasFreshFrame || msg.publishSeq != redrawLastPublishSeq) {
        dirty = true;
        redrawLastPublishSeq = msg.publishSeq;
      }
      redrawHasFreshFrame = true;
    } else if (redrawHasFreshFrame) {
      dirty = true;
      redrawHasFreshFrame = false;
    }

    if (lagDragActive) {
      dirty = true;
    }

    if (dirty) {
      framebuffer->setDirty();
    }
  }

  void draw(const DrawArgs &args) override {
    auto drawStart = std::chrono::steady_clock::now();
    if (module) {
      module->uiDebugScopeDrawCalls.fetch_add(1u, std::memory_order_relaxed);
    }
    auto publishUiDebugMetrics = [&](float densityPct, int densityRows) {
      if (!module) {
        return;
      }
      auto drawEnd = std::chrono::steady_clock::now();
      float drawUs =
        std::chrono::duration_cast<std::chrono::duration<float, std::micro>>(drawEnd - drawStart).count();
      float prevEma = module->uiDebugScopeUiDrawUsEma.load(std::memory_order_relaxed);
      float emaUs = (prevEma > 0.f) ? (prevEma + (drawUs - prevEma) * 0.18f) : drawUs;
      module->uiDebugScopeUiDrawUs.store(drawUs, std::memory_order_relaxed);
      module->uiDebugScopeUiDrawUsEma.store(emaUs, std::memory_order_relaxed);
      module->uiDebugScopeDensityPct.store(clamp(densityPct, 0.f, 100.f), std::memory_order_relaxed);
      module->uiDebugScopeDensityRows.store(std::max(densityRows, 0), std::memory_order_relaxed);
    };

    bool linkActive = module && module->uiLinkActive.load(std::memory_order_relaxed);
    bool previewValid = module && module->uiPreviewValid.load(std::memory_order_relaxed);
    // Keep stale-frame reuse short so we prefer freshest scope state.
    constexpr double kUiSnapshotGraceSec = 0.02;

    auto drawStatusMessage = [&](const char* line1, const char* line2) {
      if (!APP || !APP->window || !APP->window->uiFont) {
        return;
      }
      const float centerX = box.size.x * 0.5f;
      const float bottomY = box.size.y - 7.f;
      const float fontSize1 = 12.f;
      const float fontSize2 = 14.f;
      const float lineGap = 9.f;

      nvgSave(args.vg);
      nvgFontFaceId(args.vg, APP->window->uiFont->handle);
      nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);

      nvgFontSize(args.vg, fontSize1);
      nvgFillColor(args.vg, nvgRGBA(150, 176, 190, 220));
      nvgText(args.vg, centerX, bottomY - 2.f * lineGap, line1, nullptr);

      nvgFontSize(args.vg, fontSize2);
      nvgFillColor(args.vg, nvgRGBA(224, 238, 244, 236));
      nvgText(args.vg, centerX, bottomY - lineGap + 2.f, line2, nullptr);
      nvgRestore(args.vg);
    };

    if (!module) {
      drawStatusMessage("Attach to", "Temporal Deck");
      publishUiDebugMetrics(0.f, 0);
      return;
    }
    bool hasTemporalDeckNeighbor = tdscope::isTemporalDeckModule(module->leftExpander.module);
    if (!hasTemporalDeckNeighbor) {
      drawStatusMessage("Attach to", "Temporal Deck");
      publishUiDebugMetrics(0.f, 0);
      return;
    }

    temporaldeck_expander::HostToDisplay msg;
    const bool snapshotOk = module->readSnapshotForUi(&msg);
    const double nowSec = system::getTime();
    if (snapshotOk) {
      lastGoodMsg = msg;
      hasLastGoodMsg = true;
      lastGoodMsgTimeSec = nowSec;
    } else if (linkActive && previewValid && module) {
      module->uiSnapshotReadMissCount.fetch_add(1u, std::memory_order_relaxed);
    }

    if (!snapshotOk || !linkActive || !previewValid) {
      bool canReuseLastGood =
        hasLastGoodMsg && lastGoodMsgTimeSec >= 0.0 && (nowSec - lastGoodMsgTimeSec) <= kUiSnapshotGraceSec;
      if (!canReuseLastGood) {
        drawStatusMessage(linkActive ? "Waiting for" : "Attach to", "Temporal Deck");
        publishUiDebugMetrics(0.f, 0);
        return;
      }
      msg = lastGoodMsg;
    }
    module->uiDebugScopeDrawSeq.store(msg.publishSeq, std::memory_order_relaxed);

    uint32_t scopeBinCount = std::min(msg.scopeBinCount, temporaldeck_expander::SCOPE_BIN_COUNT);
    if (scopeBinCount == 0u) {
      publishUiDebugMetrics(0.f, 0);
      return;
    }
    bool hostStereoPayload = (msg.flags & temporaldeck_expander::FLAG_SCOPE_STEREO) != 0u;
    const int scopeChannelMode = module->scopeChannelMode.load(std::memory_order_relaxed);
    bool renderStereo = (scopeChannelMode == TDScope::SCOPE_CHANNEL_STEREO) && hostStereoPayload;
    const temporaldeck_expander::ScopeBin *leftScopeBins = msg.scope;
    const temporaldeck_expander::ScopeBin *rightScopeBins = msg.scopeRight;
    const int debugRenderMode = module->debugRenderMode.load(std::memory_order_relaxed);
    const bool silStandardStyle = debugRenderMode == TDScope::DEBUG_RENDER_STANDARD;

    int peakQAbs = 0;
    for (uint32_t i = 0; i < scopeBinCount; ++i) {
      const temporaldeck_expander::ScopeBin &bin = leftScopeBins[i];
      if (!temporaldeck_expander::isScopeBinValid(bin)) {
        continue;
      }
      peakQAbs = std::max(peakQAbs, int(std::abs(int(bin.min))));
      peakQAbs = std::max(peakQAbs, int(std::abs(int(bin.max))));
      if (renderStereo) {
        const temporaldeck_expander::ScopeBin &binR = rightScopeBins[i];
        if (temporaldeck_expander::isScopeBinValid(binR)) {
          peakQAbs = std::max(peakQAbs, int(std::abs(int(binR.min))));
          peakQAbs = std::max(peakQAbs, int(std::abs(int(binR.max))));
        }
      }
    }
    float peakWindowVolts = (float(peakQAbs) / 32767.f) * temporaldeck_expander::kPreviewQuantizeVolts;
    bool lowSignalWindow = peakWindowVolts < 0.03f;

    const float yInset = 0.75f;
    const float drawTop = yInset;
    const float drawBottom = std::max(drawTop + 1.f, box.size.y - yInset);
    const float drawHeight = std::max(drawBottom - drawTop, 1.f);

    tdscope::ScopeLaneGeometry laneGeo = tdscope::computeScopeLaneGeometry(box.size.x, renderStereo);
    const float laneGap = laneGeo.laneGap;
    const float laneWidth = laneGeo.laneWidth;
    const float lane0CenterX = laneGeo.lane0CenterX;
    const float lane1CenterX = laneGeo.lane1CenterX;
    const float laneAmpHalfWidth = laneGeo.laneAmpHalfWidth;
    const float yDen = std::max(drawHeight - 1.f, 1.f);
    if (!module->useOpenGlGeometryRenderMode()) {
      const float xInset = 0.75f;
      const float bgX = xInset;
      const float bgY = drawTop;
      const float bgW = std::max(0.f, box.size.x - 2.f * xInset);
      const float bgH = std::max(0.f, drawBottom - drawTop);
      nvgBeginPath(args.vg);
      nvgRect(args.vg, bgX, bgY, bgW, bgH);
      nvgFillColor(args.vg, nvgRGBA(0, 0, 0, 255));
      nvgFill(args.vg);
    }
    if (silStandardStyle) {
      drawSilWaveformBackground(args.vg, renderStereo, laneWidth, laneGap, drawTop, drawBottom);
    }
    const bool msgChanged = !cachedGeometryValid || msg.publishSeq != cachedPublishSeq;
    const int scopeDisplayRangeMode = module->scopeDisplayRangeMode.load(std::memory_order_relaxed);
    const bool rangeModeChanged = scopeDisplayRangeMode != cachedRangeMode;
    const bool dragActive = module->uiLagDragActive.load(std::memory_order_relaxed);
    float displayFullScaleVolts = std::max(module->scopeDisplayFullScaleVolts(), 0.001f);
    if (scopeDisplayRangeMode == TDScope::SCOPE_RANGE_AUTO) {
      bool sampleMode = (msg.flags & temporaldeck_expander::FLAG_SAMPLE_MODE) != 0u;
      const std::pair<float, float> liveStats = tdscope::computeScopePeakStatsFromBins(
        leftScopeBins, rightScopeBins, scopeBinCount, renderStereo);
      displayFullScaleVolts = tdscope::updateScopeAutoScale(
        &autoScaleState,
        true,
        msgChanged || rangeModeChanged || !autoScaleState.initialized,
        dragActive,
        sampleMode,
        liveStats.first,
        liveStats.second,
        displayFullScaleVolts);
    } else {
      displayFullScaleVolts = tdscope::updateScopeAutoScale(
        &autoScaleState, false, false, dragActive, true, 0.f, 0.f, displayFullScaleVolts);
    }
    float scopeNormGain = temporaldeck_expander::kPreviewQuantizeVolts / displayFullScaleVolts;
    float rackZoom = 1.f;
    if (APP && APP->scene && APP->scene->rackScroll) {
      rackZoom = std::max(APP->scene->rackScroll->getZoom(), 1e-4f);
    }
    // Compensate stroke thickness by rack zoom so scope readability remains
    // steadier when modules are zoomed in/out.
    float zoomThicknessMul = clamp(1.f + 0.30f * std::log2(1.f / rackZoom), 0.70f, 1.42f);
    module->uiDebugScopeRackZoom.store(rackZoom, std::memory_order_relaxed);
    module->uiDebugScopeZoomThicknessMul.store(zoomThicknessMul, std::memory_order_relaxed);
    tdscope::ScopeWindowLagSpan lagSpan = tdscope::computeScopeWindowLagSpan(msg);
    float totalWindowSamples = lagSpan.totalWindowSamples;
    bool sampleMode = (msg.flags & temporaldeck_expander::FLAG_SAMPLE_MODE) != 0u;
    float windowTopLag = lagSpan.windowTopLag;
    float windowBottomLag = lagSpan.windowBottomLag;
    bool verticalInverted = module->scopeVerticalInverted.load(std::memory_order_relaxed);
    float readHeadT = 0.5f;
    if (windowTopLag != windowBottomLag) {
      readHeadT = clamp((msg.lagSamples - windowTopLag) / (windowBottomLag - windowTopLag), 0.f, 1.f);
    }
    if (verticalInverted) {
      readHeadT = 1.f - readHeadT;
    }
    float readHeadY = drawTop + readHeadT * yDen + 0.5f;
    if (lowSignalWindow && sampleMode) {
      readHeadY = drawTop + 0.5f * yDen + 0.5f;
    }
    // Keep the full multi-line read-head band visible near window edges,
    // and allow a slight downward nudge at the live "now" position.
    constexpr float kReadHeadHalfBandPx = 2.f;
    constexpr float kReadHeadNowNudgePx = 0.45f;
    float readHeadDrawY = readHeadY;
    if (!sampleMode) {
      float nowProximity = clamp((readHeadT - 0.96f) / 0.04f, 0.f, 1.f);
      readHeadDrawY += (verticalInverted ? -kReadHeadNowNudgePx : kReadHeadNowNudgePx) * nowProximity;
    }
    float minReadHeadY = drawTop + kReadHeadHalfBandPx + 0.5f;
    // Slightly looser lower bound so the "now" nudge can sit lower without clipping.
    float maxReadHeadY = drawBottom - kReadHeadHalfBandPx;
    if (maxReadHeadY >= minReadHeadY) {
      readHeadDrawY = clamp(readHeadDrawY, minReadHeadY, maxReadHeadY);
    } else {
      readHeadDrawY = 0.5f * (drawTop + drawBottom);
    }
    if (module->useOpenGlGeometryRenderMode()) {
      if (renderStereo) {
        float dividerX = laneWidth + laneGap * 0.5f;
        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, dividerX, drawTop);
        nvgLineTo(args.vg, dividerX, drawBottom);
        nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 22));
        nvgStrokeWidth(args.vg, 1.f);
        nvgStroke(args.vg);
      }
      float lineX0 = 2.f;
      float lineX1 = box.size.x - 2.f;
      if (lineX1 > lineX0) {
        auto drawReadHeadLine = [&](float y, uint8_t alpha, float width) {
          nvgBeginPath(args.vg);
          nvgMoveTo(args.vg, lineX0, y);
          nvgLineTo(args.vg, lineX1, y);
          nvgStrokeColor(args.vg, nvgRGBA(244, 220, 96, alpha));
          nvgStrokeWidth(args.vg, width);
          nvgStroke(args.vg);
        };
        drawReadHeadLine(readHeadDrawY - 2.f, 64, 1.f);
        drawReadHeadLine(readHeadDrawY + 2.f, 64, 1.f);
        drawReadHeadLine(readHeadDrawY - 1.f, 148, 1.f);
        drawReadHeadLine(readHeadDrawY + 1.f, 148, 1.f);
        drawReadHeadLine(readHeadDrawY, 255, 1.15f);
      }
      return;
    }
    float scopeBinSpanSamples = std::max(msg.scopeBinSpanSamples, 1e-6f);
    const int rowCount = 512;
    const int fullDensityRowCount =
      std::max(1, int(std::ceil(drawHeight * tdscope::kScopeDisplayVerticalSupersampleMax)));
    const float densityPct = 100.f * (float(rowCount) / float(fullDensityRowCount));
    size_t rowCountU = size_t(rowCount);
    const float rowStep = drawHeight / float(rowCount);
    if (rowX0.size() != rowCountU) {
      rowX0.assign(rowCountU, lane0CenterX);
      rowX1.assign(rowCountU, lane0CenterX);
      rowX0Right.assign(rowCountU, lane1CenterX);
      rowX1Right.assign(rowCountU, lane1CenterX);
      rowVisualIntensity.assign(rowCountU, 0.f);
      rowVisualIntensityRight.assign(rowCountU, 0.f);
      rowColorDrive.assign(rowCountU, 0.f);
      rowColorDriveRight.assign(rowCountU, 0.f);
      rowY.assign(rowCountU, 0.f);
      rowValid.assign(rowCountU, 0u);
      rowValidRight.assign(rowCountU, 0u);
      rowHoldFrames.assign(rowCountU, 0u);
      rowHoldFramesRight.assign(rowCountU, 0u);
      cachedGeometryValid = false;
    }

    bool stereoLayoutChanged = cachedStereoLayout != renderStereo;
    bool shouldRebuild =
      !cachedGeometryValid || msgChanged || rangeModeChanged || rowCount != cachedRowCount || stereoLayoutChanged;

    auto decodeScopeBin = [&](const temporaldeck_expander::ScopeBin &bin, float *minNorm, float *maxNorm) {
      *minNorm = clamp((float(bin.min) / 32767.f) * scopeNormGain, -1.f, 1.f);
      *maxNorm = clamp((float(bin.max) / 32767.f) * scopeNormGain, -1.f, 1.f);
    };

    auto resetLiveBucketArray = [&](std::vector<LiveScopeBucket> *buckets) {
      if (!buckets) {
        return;
      }
      buckets->assign(rowCountU, LiveScopeBucket());
      for (size_t i = 0; i < rowCountU; ++i) {
        (*buckets)[i].totalSamples = std::max(liveBucketSpanSamples, 1e-6f);
      }
    };

    const bool liveMode = !sampleMode;
    const float requestedLiveBucketSpanSamples = std::max(totalWindowSamples / float(std::max(rowCount, 1)), 1e-6f);
    if (!liveMode) {
      liveBucketsInitialized = false;
      liveBucketLastPublishSeq = 0;
    } else {
      bool resetLiveBuckets = !liveBucketsInitialized || liveBucketCount != rowCount || liveBucketsStereo != renderStereo;
      float bucketSpanDelta = std::fabs(requestedLiveBucketSpanSamples - liveBucketSpanSamples);
      if (!resetLiveBuckets && bucketSpanDelta > std::max(1e-3f, liveBucketSpanSamples * 0.01f)) {
        resetLiveBuckets = true;
      }
      if (resetLiveBuckets) {
        liveBucketCount = rowCount;
        liveBucketSpanSamples = requestedLiveBucketSpanSamples;
        liveBucketsStereo = renderStereo;
        resetLiveBucketArray(&liveBucketsLeft);
        resetLiveBucketArray(&liveBucketsRight);
        liveBucketsInitialized = true;
        liveBucketLastPublishSeq = 0;
      }
    }

    auto bucketSlotForIndex = [&](int64_t bucketIndex) -> int {
      if (liveBucketCount <= 0) {
        return 0;
      }
      int64_t mod = bucketIndex % int64_t(liveBucketCount);
      if (mod < 0) {
        mod += int64_t(liveBucketCount);
      }
      return int(mod);
    };

    auto ingestLiveLane = [&](const temporaldeck_expander::ScopeBin *scopeData, std::vector<LiveScopeBucket> *buckets) {
      if (!liveMode || !scopeData || !buckets || liveBucketCount <= 0 || buckets->size() != rowCountU) {
        return;
      }
      for (uint32_t i = 0; i < scopeBinCount; ++i) {
        const temporaldeck_expander::ScopeBin &bin = scopeData[i];
        if (!temporaldeck_expander::isScopeBinValid(bin)) {
          continue;
        }
        float binMinNorm = 0.f;
        float binMaxNorm = 0.f;
        decodeScopeBin(bin, &binMinNorm, &binMaxNorm);

        float binLagHi = msg.scopeStartLagSamples - float(i) * scopeBinSpanSamples;
        float binLagLo = binLagHi - scopeBinSpanSamples;
        float lagMin = std::max(std::min(binLagLo, binLagHi), windowBottomLag);
        float lagMax = std::min(std::max(binLagLo, binLagHi), windowTopLag);
        if (!(lagMax > lagMin)) {
          continue;
        }

        int64_t bucketIndex0 = int64_t(std::floor(lagMin / liveBucketSpanSamples));
        int64_t bucketIndex1 = int64_t(std::floor((lagMax - 1e-6f) / liveBucketSpanSamples));
        if (bucketIndex1 < bucketIndex0) {
          continue;
        }

        for (int64_t bucketIndex = bucketIndex0; bucketIndex <= bucketIndex1; ++bucketIndex) {
          float bucketStart = float(bucketIndex) * liveBucketSpanSamples;
          float bucketEnd = bucketStart + liveBucketSpanSamples;
          float overlap = std::min(lagMax, bucketEnd) - std::max(lagMin, bucketStart);
          if (!(overlap > 0.f)) {
            continue;
          }

          int slot = bucketSlotForIndex(bucketIndex);
          LiveScopeBucket &bucket = (*buckets)[size_t(slot)];
          if (bucket.key != bucketIndex) {
            bucket = LiveScopeBucket();
            bucket.totalSamples = liveBucketSpanSamples;
            bucket.key = bucketIndex;
          }
          if (bucket.updateSeq != msg.publishSeq) {
            bucket.minNorm = 0.f;
            bucket.maxNorm = 0.f;
            bucket.coveredSamples = 0.f;
            bucket.totalSamples = liveBucketSpanSamples;
            bucket.hasData = false;
            bucket.updateSeq = msg.publishSeq;
          }

          if (!bucket.hasData) {
            bucket.minNorm = binMinNorm;
            bucket.maxNorm = binMaxNorm;
            bucket.hasData = true;
          } else {
            bucket.minNorm = std::min(bucket.minNorm, binMinNorm);
            bucket.maxNorm = std::max(bucket.maxNorm, binMaxNorm);
          }

          bucket.coveredSamples = std::min(bucket.totalSamples, bucket.coveredSamples + overlap);
        }
      }
    };

    if (liveMode && msg.publishSeq != liveBucketLastPublishSeq) {
      ingestLiveLane(leftScopeBins, &liveBucketsLeft);
      if (renderStereo) {
        ingestLiveLane(rightScopeBins, &liveBucketsRight);
      }
      liveBucketLastPublishSeq = msg.publishSeq;
    }

    auto sampleEnvelopeOverInterval = [&](const temporaldeck_expander::ScopeBin *scopeData, float t0, float t1,
                                          float *minNormOut, float *maxNormOut) -> bool {
      float lag0 = windowTopLag + (windowBottomLag - windowTopLag) * clamp(t0, 0.f, 1.f);
      float lag1 = windowTopLag + (windowBottomLag - windowTopLag) * clamp(t1, 0.f, 1.f);
      float binPos0 = (msg.scopeStartLagSamples - lag0) / scopeBinSpanSamples;
      float binPos1 = (msg.scopeStartLagSamples - lag1) / scopeBinSpanSamples;
      float binPosMin = std::min(binPos0, binPos1);
      float binPosMax = std::max(binPos0, binPos1);
      if (binPosMax < 0.f || binPosMin > float(scopeBinCount - 1u)) {
        return false;
      }

      int binIndex0 = std::max(0, int(std::floor(binPosMin)));
      int binIndex1 = std::min(int(scopeBinCount - 1u), int(std::ceil(binPosMax)));
      if (binIndex1 < binIndex0) {
        return false;
      }

      float rowMinNorm = 0.f;
      float rowMaxNorm = 0.f;
      bool any = false;
      for (int i = binIndex0; i <= binIndex1; ++i) {
        const temporaldeck_expander::ScopeBin &bin = scopeData[size_t(i)];
        if (!temporaldeck_expander::isScopeBinValid(bin)) {
          continue;
        }
        float minNorm = 0.f;
        float maxNorm = 0.f;
        decodeScopeBin(bin, &minNorm, &maxNorm);
        if (!any) {
          rowMinNorm = minNorm;
          rowMaxNorm = maxNorm;
          any = true;
        } else {
          rowMinNorm = std::min(rowMinNorm, minNorm);
          rowMaxNorm = std::max(rowMaxNorm, maxNorm);
        }
      }
      if (!any) {
        return false;
      }
      *minNormOut = rowMinNorm;
      *maxNormOut = rowMaxNorm;
      return true;
    };

    auto sampleEnvelopeForLagRange = [&](const temporaldeck_expander::ScopeBin *scopeData, float lagHi, float lagLo,
                                         float *minNormOut, float *maxNormOut) -> bool {
      float binPos0 = (msg.scopeStartLagSamples - lagHi) / scopeBinSpanSamples;
      float binPos1 = (msg.scopeStartLagSamples - lagLo) / scopeBinSpanSamples;
      float binPosMin = std::min(binPos0, binPos1);
      float binPosMax = std::max(binPos0, binPos1);
      if (binPosMax < 0.f || binPosMin > float(scopeBinCount - 1u)) {
        return false;
      }
      int binIndex0 = std::max(0, int(std::floor(binPosMin)));
      int binIndex1 = std::min(int(scopeBinCount - 1u), int(std::ceil(binPosMax)));
      if (binIndex1 < binIndex0) {
        return false;
      }
      float rowMinNorm = 0.f;
      float rowMaxNorm = 0.f;
      bool any = false;
      for (int i = binIndex0; i <= binIndex1; ++i) {
        const temporaldeck_expander::ScopeBin &bin = scopeData[size_t(i)];
        if (!temporaldeck_expander::isScopeBinValid(bin)) {
          continue;
        }
        float minNorm = 0.f;
        float maxNorm = 0.f;
        decodeScopeBin(bin, &minNorm, &maxNorm);
        if (!any) {
          rowMinNorm = minNorm;
          rowMaxNorm = maxNorm;
          any = true;
        } else {
          rowMinNorm = std::min(rowMinNorm, minNorm);
          rowMaxNorm = std::max(rowMaxNorm, maxNorm);
        }
      }
      if (!any) {
        return false;
      }
      *minNormOut = rowMinNorm;
      *maxNormOut = rowMaxNorm;
      return true;
    };

    auto rebuildLaneFromScopeBins =
      [&](const temporaldeck_expander::ScopeBin *scopeData, float laneCenterXLocal, float laneHalfWidthLocal,
          std::vector<float> *x0Out, std::vector<float> *x1Out, std::vector<float> *visualOut, std::vector<uint8_t> *validOut,
          std::vector<uint8_t> *holdOut) {
      if (!x0Out || !x1Out || !visualOut || !validOut || !holdOut) {
        return;
      }
      constexpr uint8_t kRowTailHoldFrames = 2u;
      constexpr float kRowTailIntensityDecay = 0.92f;
      for (int iy = 0; iy < rowCount; ++iy) {
        size_t idx = size_t(iy);
        bool prevValid = (*validOut)[idx] != 0u;
        float prevX0 = (*x0Out)[idx];
        float prevX1 = (*x1Out)[idx];
        float prevVisual = (*visualOut)[idx];
        uint8_t prevHold = (*holdOut)[idx];
        float y = drawTop + (float(iy) + 0.5f) * rowStep;
        rowY[idx] = y;
        float rowMinNorm = 0.f;
        float rowMaxNorm = 0.f;
        bool haveRow = false;
        float t0 = clamp(float(iy) / float(rowCount), 0.f, 1.f);
        float t1 = clamp(float(iy + 1) / float(rowCount), 0.f, 1.f);
        haveRow = sampleEnvelopeOverInterval(scopeData, t0, t1, &rowMinNorm, &rowMaxNorm);
        if (!haveRow) {
          if (prevValid && prevHold > 0u) {
            (*x0Out)[idx] = prevX0;
            (*x1Out)[idx] = prevX1;
            float carryVisual = clamp(prevVisual * kRowTailIntensityDecay, 0.f, 1.f);
            (*visualOut)[idx] = carryVisual;
            (*validOut)[idx] = 1u;
            (*holdOut)[idx] = uint8_t(prevHold - 1u);
          } else {
            (*x0Out)[idx] = laneCenterXLocal;
            (*x1Out)[idx] = laneCenterXLocal;
            (*visualOut)[idx] = 0.f;
            (*validOut)[idx] = 0u;
            (*holdOut)[idx] = 0u;
          }
          continue;
        }

        float x0 = laneCenterXLocal + rowMinNorm * laneHalfWidthLocal;
        float x1 = laneCenterXLocal + rowMaxNorm * laneHalfWidthLocal;
        if (x1 < x0) {
          std::swap(x0, x1);
        }
        (*x0Out)[idx] = x0;
        (*x1Out)[idx] = x1;
        float peakness = clamp(std::max(std::fabs(rowMinNorm), std::fabs(rowMaxNorm)), 0.f, 1.f);
        float density = clamp(0.5f * (rowMaxNorm - rowMinNorm), 0.f, 1.f);
        float intensity = clamp(0.65f * peakness + 0.35f * density, 0.f, 1.f);
        float visualIntensity = clamp(std::pow(intensity, 0.60f) * 1.12f, 0.f, 1.f);
        (*visualOut)[idx] = visualIntensity;
        (*validOut)[idx] = 1u;
        (*holdOut)[idx] = kRowTailHoldFrames;
      }
    };

    auto rebuildLaneFromLiveBuckets =
      [&](const std::vector<LiveScopeBucket> *buckets, float laneCenterXLocal, float laneHalfWidthLocal,
          std::vector<float> *x0Out, std::vector<float> *x1Out, std::vector<float> *visualOut, std::vector<uint8_t> *validOut,
          std::vector<uint8_t> *holdOut) {
        if (!buckets || !x0Out || !x1Out || !visualOut || !validOut || !holdOut || buckets->size() != rowCountU ||
            liveBucketCount <= 0 || liveBucketSpanSamples <= 0.f) {
          return;
        }
        constexpr uint8_t kGapHoldFrames = 1u;
        constexpr float kGapIntensityDecay = 0.88f;
        for (int iy = 0; iy < rowCount; ++iy) {
          size_t idx = size_t(iy);
          bool prevValid = (*validOut)[idx] != 0u;
          float prevX0 = (*x0Out)[idx];
          float prevX1 = (*x1Out)[idx];
          float prevVisual = (*visualOut)[idx];
          uint8_t prevHold = (*holdOut)[idx];

          float y = drawTop + (float(iy) + 0.5f) * rowStep;
          rowY[idx] = y;
          float tMid = clamp((float(iy) + 0.5f) / float(rowCount), 0.f, 1.f);
          float lagMid = windowTopLag + (windowBottomLag - windowTopLag) * tMid;
          int64_t bucketIndex = int64_t(std::floor(lagMid / liveBucketSpanSamples));
          int slot = bucketSlotForIndex(bucketIndex);
          const LiveScopeBucket &bucket = (*buckets)[size_t(slot)];

          if (!(bucket.key == bucketIndex && bucket.hasData)) {
            if (prevValid && prevHold > 0u) {
              (*x0Out)[idx] = prevX0;
              (*x1Out)[idx] = prevX1;
              float carryVisual = clamp(prevVisual * kGapIntensityDecay, 0.f, 1.f);
              (*visualOut)[idx] = carryVisual;
              (*validOut)[idx] = 1u;
              (*holdOut)[idx] = uint8_t(prevHold - 1u);
            } else {
              (*x0Out)[idx] = laneCenterXLocal;
              (*x1Out)[idx] = laneCenterXLocal;
              (*visualOut)[idx] = 0.f;
              (*validOut)[idx] = 0u;
              (*holdOut)[idx] = 0u;
            }
            continue;
          }

          float rowMinNorm = bucket.minNorm;
          float rowMaxNorm = bucket.maxNorm;
          float x0 = laneCenterXLocal + rowMinNorm * laneHalfWidthLocal;
          float x1 = laneCenterXLocal + rowMaxNorm * laneHalfWidthLocal;
          if (x1 < x0) {
            std::swap(x0, x1);
          }
          (*x0Out)[idx] = x0;
          (*x1Out)[idx] = x1;

          float peakness = clamp(std::max(std::fabs(rowMinNorm), std::fabs(rowMaxNorm)), 0.f, 1.f);
          float density = clamp(0.5f * (rowMaxNorm - rowMinNorm), 0.f, 1.f);
          float intensity = clamp(0.65f * peakness + 0.35f * density, 0.f, 1.f);
          float fillFraction = (bucket.totalSamples > 0.f) ? (bucket.coveredSamples / bucket.totalSamples) : 0.f;
          fillFraction = clamp(fillFraction, 0.f, 1.f);
          // At wider/farther views, per-row coverage can drop and make the
          // scope look artificially dim. Keep some coverage influence for
          // fidelity, but apply a floor so visibility remains stable.
          float fillInfluence = 0.55f + 0.45f * fillFraction;
          float visualIntensity = clamp(std::pow(intensity, 0.60f) * 1.12f * fillInfluence, 0.f, 1.f);
          (*visualOut)[idx] = visualIntensity;
          (*validOut)[idx] = 1u;
          (*holdOut)[idx] = kGapHoldFrames;
        }
      };

    auto rebuildTransientColorDrive = [&](const std::vector<float> &x0, const std::vector<float> &x1,
                                         const std::vector<float> &visualIntensity, const std::vector<uint8_t> &valid,
                                         float laneHalfWidthLocal, std::vector<float> *colorDriveOut) {
      if (!colorDriveOut || colorDriveOut->size() != rowCountU) {
        return;
      }
      constexpr float kTransientRiseAlpha = 0.42f;
      constexpr float kTransientFallAlpha = 0.14f;
      float laneWidthDen = std::max(2.f * laneHalfWidthLocal, 1e-3f);
      for (int iy = 0; iy < rowCount; ++iy) {
        size_t idx = size_t(iy);
        float baseDrive = clamp(visualIntensity[idx], 0.f, 1.f);
        float prevDrive = clamp((*colorDriveOut)[idx], 0.f, 1.f);
        if (!valid[idx]) {
          (*colorDriveOut)[idx] = prevDrive * 0.82f;
          continue;
        }
        float center = 0.5f * (x0[idx] + x1[idx]);
        float span = x1[idx] - x0[idx];
        float transientNorm = 0.f;
        auto accumulateTransientAgainst = [&](size_t otherIdx) {
          float otherCenter = 0.5f * (x0[otherIdx] + x1[otherIdx]);
          float otherSpan = x1[otherIdx] - x0[otherIdx];
          float centerDeltaNorm = std::fabs(center - otherCenter) / laneWidthDen;
          float spanDeltaNorm = std::fabs(span - otherSpan) / laneWidthDen;
          transientNorm = std::max(transientNorm, std::max(centerDeltaNorm * 1.15f, spanDeltaNorm * 1.35f));
        };
        if (iy > 0 && valid[size_t(iy - 1)]) {
          accumulateTransientAgainst(size_t(iy - 1));
        }
        if (iy + 1 < rowCount && valid[size_t(iy + 1)]) {
          accumulateTransientAgainst(size_t(iy + 1));
        }
        transientNorm = clamp(transientNorm, 0.f, 1.f);
        float transientBoost = std::pow(transientNorm, 0.56f);
        float brightnessLift = clamp((0.38f + 0.92f * baseDrive) * std::pow(transientBoost, 0.84f), 0.f, 1.f);
        float alpha = (brightnessLift > prevDrive) ? kTransientRiseAlpha : kTransientFallAlpha;
        (*colorDriveOut)[idx] = clamp(prevDrive + (brightnessLift - prevDrive) * alpha, 0.f, 1.f);
      }
    };

    bool useGeometryHistoryCache = module->useGeometryHistoryRenderMode();
    if (useGeometryHistoryCache) {
      int historyMargin = rowCount;
      int historyCapacity = rowCount * 3;
      auto resetHistoryVectors = [&]() {
        historyCapacityRows = historyCapacity;
        historyMarginRows = historyMargin;
        historyRowCount = rowCount;
        historyStereoLayout = renderStereo;
        historyHeadRow = 0;
        historyX0.assign(size_t(historyCapacityRows), lane0CenterX);
        historyX1.assign(size_t(historyCapacityRows), lane0CenterX);
        historyX0Right.assign(size_t(historyCapacityRows), lane1CenterX);
        historyX1Right.assign(size_t(historyCapacityRows), lane1CenterX);
        historyVisualIntensity.assign(size_t(historyCapacityRows), 0.f);
        historyVisualIntensityRight.assign(size_t(historyCapacityRows), 0.f);
        historyValid.assign(size_t(historyCapacityRows), 0u);
        historyValidRight.assign(size_t(historyCapacityRows), 0u);
        historyHoldFrames.assign(size_t(historyCapacityRows), 0u);
        historyHoldFramesRight.assign(size_t(historyCapacityRows), 0u);
        historyValidState = false;
        historyShiftResidualRows = 0.f;
      };
      if (historyCapacityRows != historyCapacity || historyRowCount != rowCount || historyStereoLayout != renderStereo ||
          historyX0.size() != size_t(historyCapacity)) {
        resetHistoryVectors();
      }

      auto clearHistoryRows = [&](int y0, int y1) {
        y0 = clamp(y0, 0, historyCapacityRows - 1);
        y1 = clamp(y1, 0, historyCapacityRows - 1);
        if (y1 < y0) {
          return;
        }
        auto historySlotForLogicalRow = [&](int logicalRow) {
          int slot = historyHeadRow + logicalRow;
          slot %= historyCapacityRows;
          if (slot < 0) {
            slot += historyCapacityRows;
          }
          return slot;
        };
        for (int iy = y0; iy <= y1; ++iy) {
          size_t idx = size_t(historySlotForLogicalRow(iy));
          historyX0[idx] = lane0CenterX;
          historyX1[idx] = lane0CenterX;
          historyVisualIntensity[idx] = 0.f;
          historyValid[idx] = 0u;
          historyHoldFrames[idx] = 0u;
          historyX0Right[idx] = lane1CenterX;
          historyX1Right[idx] = lane1CenterX;
          historyVisualIntensityRight[idx] = 0.f;
          historyValidRight[idx] = 0u;
          historyHoldFramesRight[idx] = 0u;
        }
      };

      auto shiftHistoryRows = [&](int shiftRows) {
        if (shiftRows == 0) {
          return;
        }
        historyHeadRow += shiftRows;
        historyHeadRow %= historyCapacityRows;
        if (historyHeadRow < 0) {
          historyHeadRow += historyCapacityRows;
        }
        if (shiftRows > 0) {
          clearHistoryRows(historyCapacityRows - shiftRows, historyCapacityRows - 1);
        } else {
          clearHistoryRows(0, -shiftRows - 1);
        }
      };

      auto rebuildHistoryLaneRows = [&](const temporaldeck_expander::ScopeBin *scopeData, float laneCenterXLocal,
                                        float laneHalfWidthLocal, std::vector<float> *x0Out, std::vector<float> *x1Out,
                                        std::vector<float> *visualOut, std::vector<uint8_t> *validOut,
                                        std::vector<uint8_t> *holdOut, int startRow, int endRow) {
        if (!scopeData || !x0Out || !x1Out || !visualOut || !validOut || !holdOut || historyCapacityRows <= 0) {
          return;
        }
        constexpr uint8_t kRowTailHoldFrames = 2u;
        constexpr float kRowTailIntensityDecay = 0.92f;
        startRow = clamp(startRow, 0, historyCapacityRows - 1);
        endRow = clamp(endRow, 0, historyCapacityRows - 1);
        if (endRow < startRow) {
          return;
        }
        for (int iy = startRow; iy <= endRow; ++iy) {
          int slot = historyHeadRow + iy;
          slot %= historyCapacityRows;
          if (slot < 0) {
            slot += historyCapacityRows;
          }
          size_t idx = size_t(slot);
          bool prevValid = (*validOut)[idx] != 0u;
          float prevX0 = (*x0Out)[idx];
          float prevX1 = (*x1Out)[idx];
          float prevVisual = (*visualOut)[idx];
          uint8_t prevHold = (*holdOut)[idx];
          float lagHi = windowTopLag - float(iy - historyMarginRows) * historyRowSpanSamples;
          float lagLo = lagHi - historyRowSpanSamples;
          float rowMinNorm = 0.f;
          float rowMaxNorm = 0.f;
          if (!sampleEnvelopeForLagRange(scopeData, lagHi, lagLo, &rowMinNorm, &rowMaxNorm)) {
            if (prevValid && prevHold > 0u) {
              (*x0Out)[idx] = prevX0;
              (*x1Out)[idx] = prevX1;
              (*visualOut)[idx] = clamp(prevVisual * kRowTailIntensityDecay, 0.f, 1.f);
              (*validOut)[idx] = 1u;
              (*holdOut)[idx] = uint8_t(prevHold - 1u);
            } else {
              (*x0Out)[idx] = laneCenterXLocal;
              (*x1Out)[idx] = laneCenterXLocal;
              (*visualOut)[idx] = 0.f;
              (*validOut)[idx] = 0u;
              (*holdOut)[idx] = 0u;
            }
            continue;
          }
          float x0 = laneCenterXLocal + rowMinNorm * laneHalfWidthLocal;
          float x1 = laneCenterXLocal + rowMaxNorm * laneHalfWidthLocal;
          if (x1 < x0) {
            std::swap(x0, x1);
          }
          (*x0Out)[idx] = x0;
          (*x1Out)[idx] = x1;
          float peakness = clamp(std::max(std::fabs(rowMinNorm), std::fabs(rowMaxNorm)), 0.f, 1.f);
          float density = clamp(0.5f * (rowMaxNorm - rowMinNorm), 0.f, 1.f);
          float intensity = clamp(0.65f * peakness + 0.35f * density, 0.f, 1.f);
          (*visualOut)[idx] = clamp(std::pow(intensity, 0.60f) * 1.12f, 0.f, 1.f);
          (*validOut)[idx] = 1u;
          (*holdOut)[idx] = kRowTailHoldFrames;
        }
      };

      bool historyCompatible = historyValidState && historyRowCount == rowCount && historyStereoLayout == renderStereo &&
                               std::fabs(historyRowSpanSamples - (totalWindowSamples / float(std::max(rowCount, 1)))) <=
                                 std::max(1e-3f, historyRowSpanSamples * 0.01f);
      bool fullHistoryRebuild = !historyCompatible || rangeModeChanged || stereoLayoutChanged;
      int rebuildStart = historyMarginRows;
      int rebuildEnd = historyMarginRows + rowCount - 1;
      if (!fullHistoryRebuild && msg.publishSeq != cachedPublishSeq) {
        float shiftLagSamples =
          (historyReferenceWindowTopLag - windowTopLag) + (msg.scopeNewestPosSamples - historyNewestPosSamples);
        float exactShiftRows = historyShiftResidualRows + shiftLagSamples / std::max(totalWindowSamples / float(std::max(rowCount, 1)), 1e-6f);
        int shiftRows = int(std::trunc(exactShiftRows));
        historyShiftResidualRows = exactShiftRows - float(shiftRows);
        if (std::abs(shiftRows) >= historyMarginRows) {
          fullHistoryRebuild = true;
          historyShiftResidualRows = 0.f;
        } else if (shiftRows != 0) {
          shiftHistoryRows(shiftRows);
          if (shiftRows > 0) {
            rebuildStart = std::max(historyMarginRows, historyMarginRows + rowCount - shiftRows - 1);
            rebuildEnd = historyMarginRows + rowCount - 1;
          } else {
            rebuildStart = historyMarginRows;
            rebuildEnd = std::min(historyMarginRows + rowCount - 1, historyMarginRows - shiftRows);
          }
        } else {
          if (exactShiftRows >= 0.f) {
            rebuildStart = std::max(historyMarginRows, historyMarginRows + rowCount - 2);
            rebuildEnd = historyMarginRows + rowCount - 1;
          } else {
            rebuildStart = historyMarginRows;
            rebuildEnd = std::min(historyMarginRows + rowCount - 1, historyMarginRows + 1);
          }
        }
      } else if (!fullHistoryRebuild) {
        rebuildStart = historyMarginRows;
        rebuildEnd = historyMarginRows + rowCount - 1;
      }

      historyRowSpanSamples = totalWindowSamples / float(std::max(rowCount, 1));
      if (fullHistoryRebuild) {
        clearHistoryRows(0, historyCapacityRows - 1);
        rebuildStart = historyMarginRows;
        rebuildEnd = historyMarginRows + rowCount - 1;
        historyShiftResidualRows = 0.f;
      }
      rebuildHistoryLaneRows(
        leftScopeBins, lane0CenterX, laneAmpHalfWidth, &historyX0, &historyX1, &historyVisualIntensity, &historyValid,
        &historyHoldFrames, rebuildStart, rebuildEnd);
      if (renderStereo) {
        rebuildHistoryLaneRows(
          rightScopeBins, lane1CenterX, laneAmpHalfWidth, &historyX0Right, &historyX1Right, &historyVisualIntensityRight,
          &historyValidRight, &historyHoldFramesRight, rebuildStart, rebuildEnd);
      }

      for (int iy = 0; iy < rowCount; ++iy) {
        size_t vidx = size_t(iy);
        int hslot = historyHeadRow + historyMarginRows + iy;
        hslot %= historyCapacityRows;
        if (hslot < 0) {
          hslot += historyCapacityRows;
        }
        size_t hidx = size_t(hslot);
        rowY[vidx] = drawTop + (float(iy) + 0.5f) * rowStep;
        rowX0[vidx] = historyX0[hidx];
        rowX1[vidx] = historyX1[hidx];
        rowVisualIntensity[vidx] = historyVisualIntensity[hidx];
        rowValid[vidx] = historyValid[hidx];
        rowHoldFrames[vidx] = historyHoldFrames[hidx];
        rowX0Right[vidx] = historyX0Right[hidx];
        rowX1Right[vidx] = historyX1Right[hidx];
        rowVisualIntensityRight[vidx] = historyVisualIntensityRight[hidx];
        rowValidRight[vidx] = historyValidRight[hidx];
        rowHoldFramesRight[vidx] = historyHoldFramesRight[hidx];
      }
      rebuildTransientColorDrive(rowX0, rowX1, rowVisualIntensity, rowValid, laneAmpHalfWidth, &rowColorDrive);
      if (renderStereo) {
        rebuildTransientColorDrive(
          rowX0Right, rowX1Right, rowVisualIntensityRight, rowValidRight, laneAmpHalfWidth, &rowColorDriveRight);
      } else {
        std::fill(rowColorDriveRight.begin(), rowColorDriveRight.end(), 0.f);
      }
      cachedPublishSeq = msg.publishSeq;
      cachedRowCount = rowCount;
      cachedRangeMode = scopeDisplayRangeMode;
      cachedVerticalInverted = verticalInverted;
      cachedStereoLayout = renderStereo;
      cachedGeometryValid = true;
      historyValidState = true;
      historyReferenceWindowTopLag = windowTopLag;
      historyReferenceWindowBottomLag = windowBottomLag;
      historyNewestPosSamples = msg.scopeNewestPosSamples;
    } else if (shouldRebuild) {
      if (liveMode) {
        rebuildLaneFromLiveBuckets(
          &liveBucketsLeft, lane0CenterX, laneAmpHalfWidth, &rowX0, &rowX1, &rowVisualIntensity, &rowValid, &rowHoldFrames);
        if (renderStereo) {
          rebuildLaneFromLiveBuckets(
            &liveBucketsRight, lane1CenterX, laneAmpHalfWidth, &rowX0Right, &rowX1Right, &rowVisualIntensityRight, &rowValidRight,
            &rowHoldFramesRight);
        } else {
          std::fill(rowValidRight.begin(), rowValidRight.end(), 0u);
          std::fill(rowVisualIntensityRight.begin(), rowVisualIntensityRight.end(), 0.f);
          std::fill(rowHoldFramesRight.begin(), rowHoldFramesRight.end(), 0u);
          for (size_t idx = 0; idx < rowCountU; ++idx) {
            rowX0Right[idx] = lane1CenterX;
            rowX1Right[idx] = lane1CenterX;
          }
        }
      } else {
        rebuildLaneFromScopeBins(
          leftScopeBins, lane0CenterX, laneAmpHalfWidth, &rowX0, &rowX1, &rowVisualIntensity, &rowValid, &rowHoldFrames);
        if (renderStereo) {
          rebuildLaneFromScopeBins(
            rightScopeBins, lane1CenterX, laneAmpHalfWidth, &rowX0Right, &rowX1Right, &rowVisualIntensityRight, &rowValidRight,
            &rowHoldFramesRight);
        } else {
          std::fill(rowValidRight.begin(), rowValidRight.end(), 0u);
          std::fill(rowVisualIntensityRight.begin(), rowVisualIntensityRight.end(), 0.f);
          std::fill(rowHoldFramesRight.begin(), rowHoldFramesRight.end(), 0u);
          for (size_t idx = 0; idx < rowCountU; ++idx) {
            rowX0Right[idx] = lane1CenterX;
            rowX1Right[idx] = lane1CenterX;
          }
        }
      }
      cachedPublishSeq = msg.publishSeq;
      cachedRowCount = rowCount;
      cachedRangeMode = scopeDisplayRangeMode;
      cachedVerticalInverted = verticalInverted;
      cachedStereoLayout = renderStereo;
      cachedGeometryValid = true;
      rebuildTransientColorDrive(rowX0, rowX1, rowVisualIntensity, rowValid, laneAmpHalfWidth, &rowColorDrive);
      if (renderStereo) {
        rebuildTransientColorDrive(rowX0Right, rowX1Right, rowVisualIntensityRight, rowValidRight, laneAmpHalfWidth,
                                   &rowColorDriveRight);
      } else {
        std::fill(rowColorDriveRight.begin(), rowColorDriveRight.end(), 0.f);
      }
    }

    static std::array<std::array<NVGcolor, 256>, TDScope::COLOR_SCHEME_COUNT> colorLut;
    static std::array<uint8_t, TDScope::COLOR_SCHEME_COUNT> colorLutValid {};
    auto ensureColorLut = [&](int scheme) {
      scheme = clamp(scheme, 0, TDScope::COLOR_SCHEME_COUNT - 1);
      if (colorLutValid[size_t(scheme)]) {
        return;
      }
      float lowR = 85.f;
      float lowG = 227.f;
      float lowB = 238.f;
      float midR = 176.f;
      float midG = 143.f;
      float midB = 245.f;
      float highR = 233.f;
      float highG = 112.f;
      float highB = 218.f;
      float midPoint = 0.5f;
      float midHoldHalfWidth = 0.f;
      switch (scheme) {
        case TDScope::COLOR_SCHEME_CLASSIC:
          lowR = 0.f;
          lowG = 255.f;
          lowB = 65.f;
          midR = 255.f;
          midG = 232.f;
          midB = 32.f;
          highR = 255.f;
          highG = 15.f;
          highB = 5.f;
          midPoint = 0.5f;
          break;
        case TDScope::COLOR_SCHEME_MONOCHROME:
          lowR = 92.f;
          lowG = 92.f;
          lowB = 92.f;
          midR = 168.f;
          midG = 168.f;
          midB = 168.f;
          highR = 242.f;
          highG = 242.f;
          highB = 242.f;
          midPoint = 0.52f;
          break;
        case TDScope::COLOR_SCHEME_FIRE:
          lowR = 140.f;
          lowG = 0.f;
          lowB = 0.f;
          midR = 255.f;
          midG = 120.f;
          midB = 0.f;
          highR = 255.f;
          highG = 255.f;
          highB = 30.f;
          midPoint = 0.5f;
          break;
        case TDScope::COLOR_SCHEME_DEFAULT:
        default:
          lowR = 122.f;
          lowG = 92.f;
          lowB = 255.f;
          midR = 81.f;
          midG = 157.f;
          midB = 244.f;
          highR = 28.f;
          highG = 204.f;
          highB = 217.f;
          midPoint = 0.5f;
          midHoldHalfWidth = 0.f;
          break;
      }
      for (int i = 0; i < 256; ++i) {
        float intensity = float(i) / 255.f;
        float r = 0.f;
        float g = 0.f;
        float b = 0.f;
        float midStart = clamp(midPoint - midHoldHalfWidth, 0.f, 1.f);
        float midEnd = clamp(midPoint + midHoldHalfWidth, 0.f, 1.f);
        if (midEnd <= midStart + 1e-6f) {
          if (intensity <= midPoint) {
            float t = (midPoint > 1e-6f) ? (intensity / midPoint) : 0.f;
            r = lowR + (midR - lowR) * t;
            g = lowG + (midG - lowG) * t;
            b = lowB + (midB - lowB) * t;
          } else {
            float t = (1.f - midPoint > 1e-6f) ? ((intensity - midPoint) / (1.f - midPoint)) : 1.f;
            r = midR + (highR - midR) * t;
            g = midG + (highG - midG) * t;
            b = midB + (highB - midB) * t;
          }
        } else if (intensity < midStart) {
          float t = (midStart > 1e-6f) ? (intensity / midStart) : 0.f;
          r = lowR + (midR - lowR) * t;
          g = lowG + (midG - lowG) * t;
          b = lowB + (midB - lowB) * t;
        } else if (intensity <= midEnd) {
          r = midR;
          g = midG;
          b = midB;
        } else {
          float t = (1.f - midEnd > 1e-6f) ? ((intensity - midEnd) / (1.f - midEnd)) : 1.f;
          r = midR + (highR - midR) * t;
          g = midG + (highG - midG) * t;
          b = midB + (highB - midB) * t;
        }
        uint8_t rq = uint8_t(std::lround(clamp(r, 0.f, 255.f)));
        uint8_t gq = uint8_t(std::lround(clamp(g, 0.f, 255.f)));
        uint8_t bq = uint8_t(std::lround(clamp(b, 0.f, 255.f)));
        colorLut[size_t(scheme)][size_t(i)] = nvgRGBA(rq, gq, bq, 255);
      }
      colorLutValid[size_t(scheme)] = 1u;
    };
    auto gradientColorForIntensity = [&](float intensity, uint8_t alpha) -> NVGcolor {
      int scheme = clamp(module->scopeColorScheme, 0, TDScope::COLOR_SCHEME_COUNT - 1);
      ensureColorLut(scheme);
      int index = clamp(int(std::lround(clamp(intensity, 0.f, 1.f) * 255.f)), 0, 255);
      NVGcolor c = colorLut[size_t(scheme)][size_t(index)];
      // Keep hue/chroma strong (Sil-like vibrance) before brightness shaping.
      float luma = c.r * 0.2126f + c.g * 0.7152f + c.b * 0.0722f;
      constexpr float kVibrance = 1.40f;
      c.r = clamp(luma + (c.r - luma) * kVibrance, 0.f, 1.f);
      c.g = clamp(luma + (c.g - luma) * kVibrance, 0.f, 1.f);
      c.b = clamp(luma + (c.b - luma) * kVibrance, 0.f, 1.f);
      // Keep a strong top-end accent without washing hue toward white.
      float hotT = clamp((clamp(intensity, 0.f, 1.f) - 0.74f) / 0.26f, 0.f, 1.f);
      float hotLift = 0.22f * std::pow(hotT, 1.15f);
      c.r = clamp(c.r * (1.f + hotLift), 0.f, 1.f);
      c.g = clamp(c.g * (1.f + hotLift), 0.f, 1.f);
      c.b = clamp(c.b * (1.f + hotLift), 0.f, 1.f);
      c = module->applyScopeColorBrightness(c);
      c.a = float(alpha) / 255.f;
      return c;
    };

    auto brightenColor = [&](NVGcolor c, float lift) -> NVGcolor {
      lift = clamp(lift, 0.f, 1.f);
      float gain = 1.f + 0.88f * lift;
      c.r = clamp(c.r * gain, 0.f, 1.f);
      c.g = clamp(c.g * gain, 0.f, 1.f);
      c.b = clamp(c.b * gain, 0.f, 1.f);
      float luma = c.r * 0.2126f + c.g * 0.7152f + c.b * 0.0722f;
      float sat = 1.f + 0.30f * lift;
      c.r = clamp(luma + (c.r - luma) * sat, 0.f, 1.f);
      c.g = clamp(luma + (c.g - luma) * sat, 0.f, 1.f);
      c.b = clamp(luma + (c.b - luma) * sat, 0.f, 1.f);
      return c;
    };

    auto ensureTailRasterImage = [&]() {
      int targetW = std::max(1, int(std::ceil(box.size.x)));
      int targetH = std::max(1, int(std::ceil(box.size.y)));
      if (tailRasterImageVg != args.vg) {
        resetTailRasterImage(args.vg, false);
      }
      if (tailRasterImage >= 0) {
        if (!nvg_gfx_lifecycle::ownedNvgImageSizeMatches(args.vg, tailRasterImage, tailRasterW, tailRasterH)) {
          resetTailRasterImage(args.vg, true);
        }
      }
      bool recreate = (tailRasterImage < 0) || (tailRasterW != targetW) || (tailRasterH != targetH);
      if (!recreate) {
        return;
      }
      resetTailRasterImage(args.vg, true);
      tailRasterW = targetW;
      tailRasterH = targetH;
      tailRasterPixels.assign(size_t(tailRasterW * tailRasterH * 4), 0u);
      tailRasterImage = nvgCreateImageRGBA(args.vg, tailRasterW, tailRasterH, NVG_IMAGE_PREMULTIPLIED, nullptr);
      tailRasterImageVg = args.vg;
      if (tailRasterImage >= 0) {
        nvgUpdateImage(args.vg, tailRasterImage, tailRasterPixels.data());
      }
    };

    auto clearTailRasterRows = [&](int y0, int y1) {
      if (tailRasterW <= 0 || tailRasterH <= 0 || tailRasterPixels.empty()) {
        return;
      }
      y0 = clamp(y0, 0, tailRasterH - 1);
      y1 = clamp(y1, 0, tailRasterH - 1);
      if (y1 < y0) {
        return;
      }
      size_t stride = size_t(tailRasterW) * 4u;
      for (int y = y0; y <= y1; ++y) {
        std::memset(tailRasterPixels.data() + size_t(y) * stride, 0, stride);
      }
    };

    auto blendTailRasterPixel = [&](int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
      if (a == 0u || x < 0 || y < 0 || x >= tailRasterW || y >= tailRasterH) {
        return;
      }
      size_t idx = (size_t(y) * size_t(tailRasterW) + size_t(x)) * 4u;
      uint8_t sr = uint8_t((uint16_t(r) * uint16_t(a) + 127u) / 255u);
      uint8_t sg = uint8_t((uint16_t(g) * uint16_t(a) + 127u) / 255u);
      uint8_t sb = uint8_t((uint16_t(b) * uint16_t(a) + 127u) / 255u);
      uint8_t sa = a;
      uint8_t dr = tailRasterPixels[idx + 0];
      uint8_t dg = tailRasterPixels[idx + 1];
      uint8_t db = tailRasterPixels[idx + 2];
      uint8_t da = tailRasterPixels[idx + 3];
      uint16_t inv = uint16_t(255u - sa);
      tailRasterPixels[idx + 0] = uint8_t(std::min<uint16_t>(255u, uint16_t(sr) + uint16_t((uint16_t(dr) * inv + 127u) / 255u)));
      tailRasterPixels[idx + 1] = uint8_t(std::min<uint16_t>(255u, uint16_t(sg) + uint16_t((uint16_t(dg) * inv + 127u) / 255u)));
      tailRasterPixels[idx + 2] = uint8_t(std::min<uint16_t>(255u, uint16_t(sb) + uint16_t((uint16_t(db) * inv + 127u) / 255u)));
      tailRasterPixels[idx + 3] = uint8_t(std::min<uint16_t>(255u, uint16_t(sa) + uint16_t((uint16_t(da) * inv + 127u) / 255u)));
    };

    auto fillTailRasterSpan = [&](int y, int x0, int x1, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
      if (y < 0 || y >= tailRasterH || a == 0u) {
        return;
      }
      if (x1 < x0) {
        std::swap(x0, x1);
      }
      x0 = clamp(x0, 0, tailRasterW - 1);
      x1 = clamp(x1, 0, tailRasterW - 1);
      for (int x = x0; x <= x1; ++x) {
        blendTailRasterPixel(x, y, r, g, b, a);
      }
    };

    auto drawTailRasterLaneRows =
      [&](const std::vector<float> &x0, const std::vector<float> &x1, const std::vector<float> &visualIntensity,
          const std::vector<float> &colorDrive, const std::vector<uint8_t> &valid, float laneCenterXForConnectors,
          int iyMin, int iyMax) {
        iyMin = clamp(iyMin, 0, rowCount - 1);
        iyMax = clamp(iyMax, 0, rowCount - 1);
        if (iyMax < iyMin) {
          return;
        }

        bool prevValid = false;
        float prevX0 = laneCenterXForConnectors;
        float prevX1 = laneCenterXForConnectors;
        int prevY = 0;
        for (int iy = iyMin; iy <= iyMax; ++iy) {
          size_t idx = size_t(iy);
          if (!valid[idx]) {
            prevValid = false;
            continue;
          }
          int yPx = clamp(int(std::lround(rowY[idx])), 0, tailRasterH - 1);
          float visual = clamp(visualIntensity[idx], 0.f, 1.f);
          float transientLift = clamp(colorDrive[idx], 0.f, 1.f);
          float tone = clamp(0.70f * visual + 0.30f * transientLift, 0.f, 1.f);
          NVGcolor c = brightenColor(
            gradientColorForIntensity(tone, uint8_t(std::lround(122.f + 120.f * tone))),
            transientLift * 0.62f);
          uint8_t r = uint8_t(std::lround(clamp(c.r, 0.f, 1.f) * 255.f));
          uint8_t g = uint8_t(std::lround(clamp(c.g, 0.f, 1.f) * 255.f));
          uint8_t b = uint8_t(std::lround(clamp(c.b, 0.f, 1.f) * 255.f));
          uint8_t a = uint8_t(std::lround(clamp(c.a, 0.f, 1.f) * 255.f));
          int xMin = int(std::lround(std::min(x0[idx], x1[idx])));
          int xMax = int(std::lround(std::max(x0[idx], x1[idx])));

          int halfW = std::max(0, int(std::lround((0.78f + 0.62f * tone) * zoomThicknessMul * 0.5f)));
          for (int yy = yPx - halfW; yy <= yPx + halfW; ++yy) {
            fillTailRasterSpan(yy, xMin, xMax, r, g, b, a);
          }

          if (prevValid) {
            int y0i = prevY;
            int y1i = yPx;
            int dy = std::abs(y1i - y0i);
            int steps = std::max(1, dy);
            for (int s = 0; s <= steps; ++s) {
              float t = float(s) / float(steps);
              int ys = int(std::lround(float(y0i) + float(y1i - y0i) * t));
              int lx = int(std::lround(prevX0 + (x0[idx] - prevX0) * t));
              int rx = int(std::lround(prevX1 + (x1[idx] - prevX1) * t));
              fillTailRasterSpan(ys, lx, rx, r, g, b, uint8_t(std::min(255, int(a) / 2 + 28)));
            }
          }

          prevX0 = x0[idx];
          prevX1 = x1[idx];
          prevY = yPx;
          prevValid = true;
        }
      };

    nvgSave(args.vg);
    if (verticalInverted) {
      nvgTranslate(args.vg, 0.f, box.size.y);
      nvgScale(args.vg, 1.f, -1.f);
    }
    nvgScissor(args.vg, 0.f, drawTop, box.size.x, drawBottom - drawTop);

    auto drawLane = [&](const std::vector<float> &x0, const std::vector<float> &x1, const std::vector<float> &visualIntensity,
                        const std::vector<float> &colorDrive, const std::vector<uint8_t> &valid,
                        float laneCenterXForConnectors) {
      // Batch trace rendering into ordered intensity bins so NanoVG receives
      // far fewer stroke submissions while preserving a low->high gradient
      // progression across the scope.
      constexpr int kMainStrokeBins = 10;
      constexpr int kConnectorStrokeBins = 8;

      auto quantizeStrokeBin = [&](float t, int binCount) -> int {
        t = clamp(t, 0.f, 1.f);
        return clamp(int(std::floor(t * float(binCount))), 0, binCount - 1);
      };
      auto strokeBinCenter = [&](int bin, int binCount) -> float {
        return (float(bin) + 0.5f) / float(binCount);
      };

      if (silStandardStyle) {
        const NVGcolor silLow = gradientColorForIntensity(0.f, 255u);
        const NVGcolor silHigh = gradientColorForIntensity(1.f, 255u);
        constexpr int kSilStrokeBins = 12;
        for (int bin = 0; bin < kSilStrokeBins; ++bin) {
          bool hasPath = false;
          nvgBeginPath(args.vg);
          for (int iy = 0; iy < rowCount; ++iy) {
            size_t idx = size_t(iy);
            if (!valid[idx]) {
              continue;
            }
            const float amp = clamp(0.84f * visualIntensity[idx] + 0.16f * colorDrive[idx], 0.f, 1.f);
            if (quantizeStrokeBin(amp, kSilStrokeBins) != bin) {
              continue;
            }
            nvgMoveTo(args.vg, x0[idx], rowY[idx]);
            nvgLineTo(args.vg, x1[idx], rowY[idx]);
            hasPath = true;
          }
          if (!hasPath) {
            continue;
          }
          const float ampCenter = strokeBinCenter(bin, kSilStrokeBins);
          NVGcolor c = nvgLerpRGBA(silLow, silHigh, ampCenter);
          c.a = 1.f;
          nvgStrokeColor(args.vg, c);
          nvgStrokeWidth(args.vg, std::max(0.75f, 1.0f * zoomThicknessMul));
          nvgStroke(args.vg);
        }
        return;
      }

      for (int bin = 0; bin < kMainStrokeBins; ++bin) {
        bool hasPath = false;
        nvgBeginPath(args.vg);
        for (int iy = 0; iy < rowCount; ++iy) {
          size_t idx = size_t(iy);
          if (!valid[idx]) {
            continue;
          }
          float visual = clamp(visualIntensity[idx], 0.f, 1.f);
          float transientLift = clamp(colorDrive[idx], 0.f, 1.f);
          float tone = clamp(0.70f * visual + 0.30f * transientLift, 0.f, 1.f);
          if (quantizeStrokeBin(tone, kMainStrokeBins) != bin) {
            continue;
          }
          nvgMoveTo(args.vg, x0[idx], rowY[idx]);
          nvgLineTo(args.vg, x1[idx], rowY[idx]);
          hasPath = true;
        }
        if (!hasPath) {
          continue;
        }
        float toneCenter = strokeBinCenter(bin, kMainStrokeBins);
        float visualCenter = toneCenter;
        float transientCenter = toneCenter;
        NVGcolor mainC = brightenColor(
          gradientColorForIntensity(visualCenter, uint8_t(std::lround(122.f + 120.f * visualCenter))),
          transientCenter * 0.62f);
        float mainW = (0.78f + 0.62f * visualCenter) * zoomThicknessMul;
        nvgStrokeColor(args.vg, mainC);
        nvgStrokeWidth(args.vg, mainW);
        nvgStroke(args.vg);
      }

      const float connectorMinDeltaPx = std::max(0.60f * zoomThicknessMul, 0.40f);
      for (int bin = 0; bin < kConnectorStrokeBins; ++bin) {
        bool hasPath = false;
        bool prevValid = false;
        float prevX0 = laneCenterXForConnectors;
        float prevX1 = laneCenterXForConnectors;
        float prevY = drawTop + 0.5f;
        float prevVisual = 0.f;
        float prevColorDrive = 0.f;
        nvgBeginPath(args.vg);
        for (int iy = 0; iy < rowCount; ++iy) {
          size_t idx = size_t(iy);
          if (!valid[idx]) {
            prevValid = false;
            continue;
          }
          float visual = clamp(visualIntensity[idx], 0.f, 1.f);
          float transientLift = clamp(colorDrive[idx], 0.f, 1.f);
          if (prevValid) {
            float leftDeltaPx = std::fabs(x0[idx] - prevX0);
            float rightDeltaPx = std::fabs(x1[idx] - prevX1);
            if (leftDeltaPx < connectorMinDeltaPx && rightDeltaPx < connectorMinDeltaPx) {
              prevX0 = x0[idx];
              prevX1 = x1[idx];
              prevY = rowY[idx];
              prevVisual = visual;
              prevColorDrive = transientLift;
              prevValid = true;
              continue;
            }
            float connectVisual = clamp(0.5f * (prevVisual + visual), 0.f, 1.f);
            float connectTransientLift = clamp(0.5f * (prevColorDrive + transientLift), 0.f, 1.f);
            float tone = clamp(0.82f * connectVisual + 0.18f * connectTransientLift, 0.f, 1.f);
            if (quantizeStrokeBin(tone, kConnectorStrokeBins) == bin) {
              nvgMoveTo(args.vg, prevX0, prevY);
              nvgLineTo(args.vg, x0[idx], rowY[idx]);
              nvgMoveTo(args.vg, prevX1, prevY);
              nvgLineTo(args.vg, x1[idx], rowY[idx]);
              hasPath = true;
            }
          }
          prevX0 = x0[idx];
          prevX1 = x1[idx];
          prevY = rowY[idx];
          prevVisual = visual;
          prevColorDrive = transientLift;
          prevValid = true;
        }
        if (!hasPath) {
          continue;
        }
        float toneCenter = strokeBinCenter(bin, kConnectorStrokeBins);
        float connectVisual = toneCenter;
        float connectTransientLift = toneCenter;
        NVGcolor connectC = brightenColor(
          gradientColorForIntensity(connectVisual, uint8_t(std::lround(88.f + 92.f * connectVisual))),
          connectTransientLift * 0.58f);
        float connectW = (0.58f + 0.40f * connectVisual) * zoomThicknessMul;
        nvgStrokeColor(args.vg, connectC);
        nvgStrokeWidth(args.vg, connectW);
        nvgStroke(args.vg);
      }
    };

    bool useTailRaster = module->useTailRasterRenderMode();
    if (!useTailRaster) {
      drawLane(rowX0, rowX1, rowVisualIntensity, rowColorDrive, rowValid, lane0CenterX);
      if (renderStereo) {
        drawLane(rowX0Right, rowX1Right, rowVisualIntensityRight, rowColorDriveRight, rowValidRight, lane1CenterX);
      }
      tailRasterValid = false;
    } else {
      ensureTailRasterImage();
      if (tailRasterImage >= 0 && tailRasterW > 0 && tailRasterH > 0) {
        bool sameStereo = tailRasterStereo == renderStereo;
        float prevSpan = std::max(std::fabs(tailRasterWindowTopLag - tailRasterWindowBottomLag), 1e-6f);
        float currSpan = std::max(std::fabs(windowTopLag - windowBottomLag), 1e-6f);
        bool spanCompatible = std::fabs(currSpan - prevSpan) <= std::max(1e-4f, 0.01f * currSpan);
        bool lagDragActive = module && module->uiLagDragActive.load(std::memory_order_relaxed);
        bool canIncremental =
          tailRasterValid && sameStereo && spanCompatible && !lagDragActive && (msg.publishSeq != tailRasterPublishSeq);

        int iyMin = 0;
        int iyMax = rowCount - 1;
        if (canIncremental) {
          float shiftLagSamples =
            (tailRasterWindowTopLag - windowTopLag) + (msg.scopeNewestPosSamples - tailRasterNewestPosSamples);
          float exactShiftPx = tailRasterShiftResidualPx + (shiftLagSamples / currSpan) * float(tailRasterH - 1);
          int shiftPx = int(std::trunc(exactShiftPx));
          tailRasterShiftResidualPx = exactShiftPx - float(shiftPx);
          if (shiftPx > -tailRasterH && shiftPx < tailRasterH) {
            size_t stride = size_t(tailRasterW) * 4u;
            if (shiftPx > 0) {
              size_t copyRows = size_t(tailRasterH - shiftPx);
              std::memmove(tailRasterPixels.data(),
                           tailRasterPixels.data() + size_t(shiftPx) * stride,
                           copyRows * stride);
              clearTailRasterRows(tailRasterH - shiftPx, tailRasterH - 1);
              iyMin = clamp(int(std::floor(((float(tailRasterH - shiftPx) - drawTop) / std::max(rowStep, 1e-6f))) - 3),
                            0, rowCount - 1);
              iyMax = rowCount - 1;
            } else if (shiftPx < 0) {
              int downPx = -shiftPx;
              size_t copyRows = size_t(tailRasterH - downPx);
              std::memmove(tailRasterPixels.data() + size_t(downPx) * stride,
                           tailRasterPixels.data(),
                           copyRows * stride);
              clearTailRasterRows(0, downPx - 1);
              iyMin = 0;
              iyMax = clamp(int(std::ceil(((float(downPx) - drawTop) / std::max(rowStep, 1e-6f))) + 3), 0, rowCount - 1);
            } else {
              // Sub-pixel motion accumulated but did not cross a full pixel yet.
              // Refresh only a narrow edge band instead of falling back to a
              // full redraw, which avoids visible cadence hitching.
              constexpr int kEdgeBandRows = 6;
              if (exactShiftPx >= 0.f) {
                iyMin = std::max(0, rowCount - kEdgeBandRows);
                iyMax = rowCount - 1;
              } else {
                iyMin = 0;
                iyMax = std::min(rowCount - 1, kEdgeBandRows - 1);
              }
            }
          } else {
            canIncremental = false;
            tailRasterShiftResidualPx = 0.f;
          }
        }
        if (!canIncremental) {
          clearTailRasterRows(0, tailRasterH - 1);
          iyMin = 0;
          iyMax = rowCount - 1;
          tailRasterShiftResidualPx = 0.f;
        }

        drawTailRasterLaneRows(rowX0, rowX1, rowVisualIntensity, rowColorDrive, rowValid, lane0CenterX, iyMin, iyMax);
        if (renderStereo) {
          drawTailRasterLaneRows(
            rowX0Right, rowX1Right, rowVisualIntensityRight, rowColorDriveRight, rowValidRight, lane1CenterX, iyMin, iyMax);
        }

        nvgUpdateImage(args.vg, tailRasterImage, tailRasterPixels.data());
        nvgBeginPath(args.vg);
        nvgRect(args.vg, 0.f, 0.f, box.size.x, box.size.y);
        NVGpaint rasterPaint =
          nvgImagePattern(args.vg, 0.f, 0.f, float(tailRasterW), float(tailRasterH), 0.f, tailRasterImage, 1.f);
        nvgFillPaint(args.vg, rasterPaint);
        nvgFill(args.vg);

        tailRasterStereo = renderStereo;
        tailRasterWindowTopLag = windowTopLag;
        tailRasterWindowBottomLag = windowBottomLag;
        tailRasterNewestPosSamples = msg.scopeNewestPosSamples;
        tailRasterPublishSeq = msg.publishSeq;
        tailRasterValid = true;
      }
    }
    float lineX0 = 2.f;
    float lineX1 = box.size.x - 2.f;
    if (renderStereo && !silStandardStyle) {
      float dividerX = laneWidth + laneGap * 0.5f;
      nvgBeginPath(args.vg);
      nvgMoveTo(args.vg, dividerX, drawTop);
      nvgLineTo(args.vg, dividerX, drawBottom);
      nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 22));
      nvgStrokeWidth(args.vg, 1.f);
      nvgStroke(args.vg);
    }
    if (lineX1 > lineX0) {
      auto drawReadHeadLine = [&](float y, uint8_t alpha, float width) {
        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, lineX0, y);
        nvgLineTo(args.vg, lineX1, y);
        nvgStrokeColor(args.vg, nvgRGBA(244, 220, 96, alpha));
        nvgStrokeWidth(args.vg, width);
        nvgStroke(args.vg);
      };

      // Full-width center core plus vertical feather.
      drawReadHeadLine(readHeadDrawY - 2.f, 64, 1.f);
      drawReadHeadLine(readHeadDrawY + 2.f, 64, 1.f);
      drawReadHeadLine(readHeadDrawY - 1.f, 148, 1.f);
      drawReadHeadLine(readHeadDrawY + 1.f, 148, 1.f);
      drawReadHeadLine(readHeadDrawY, 255, 1.15f);
    }
    nvgResetScissor(args.vg);

    nvgRestore(args.vg);
    publishUiDebugMetrics(densityPct, rowCount);
  }
};

namespace tdscope {

Widget *createDisplay(TDScope *module, math::Rect scopeRectMm) {
  auto *displayFb = new widget::FramebufferWidget();
  displayFb->box.pos = mm2px(scopeRectMm.pos);
  displayFb->box.size = mm2px(scopeRectMm.size);
  displayFb->dirtyOnSubpixelChange = false;

  auto *display = new TDScopeDisplayWidget;
  display->module = module;
  display->framebuffer = displayFb;
  display->box.size = displayFb->box.size;
  displayFb->addChild(display);
  return displayFb;
}

} // namespace tdscope

struct TDScopeInputWidget final : Widget {
  static constexpr double kLagDragHoldDetectSec = 1.0 / 180.0;
  TDScope *module = nullptr;
  temporaldeck_expander::HostToDisplay lastGoodMsg;
  bool hasLastGoodMsg = false;
  double lastGoodMsgTimeSec = -1.0;
  bool lagDragging = false;
  float lagDragAnchorLagSamples = 0.f;
  float lagDragReferenceHeight = 1.f;
  float lagDragTravelSamples = 0.f;
  float lagDragNormalizedOffset = 0.f;
  float lagDragLocalLagSamples = 0.f;
  float lagDragResidualY = 0.f;
  double lagDragLastMoveSec = 0.0;
  bool lagDragStationaryHoldActive = false;

  bool isWithinDisplay(Vec pos) const {
    return pos.x >= 0.f && pos.y >= 0.f && pos.x < box.size.x && pos.y < box.size.y;
  }
  bool isPhysicallyAttachedToTemporalDeck() const {
    return module && tdscope::isTemporalDeckModule(module->leftExpander.module);
  }

  struct ScopeWindowMap {
    float drawTop = 0.f;
    float drawBottom = 1.f;
    float drawYDen = 1.f;
    float windowTopLag = 0.f;
    float windowBottomLag = 0.f;
    float accessibleLag = 0.f;
    bool valid = false;
  };

  ScopeWindowMap buildScopeWindowMap(const temporaldeck_expander::HostToDisplay &msg) const {
    ScopeWindowMap map;
    tdscope::ScopeWindowLagSpan span = tdscope::computeScopeWindowLagSpan(msg);
    map.windowTopLag = span.windowTopLag;
    map.windowBottomLag = span.windowBottomLag;
    map.accessibleLag = std::max(0.f, msg.accessibleLagSamples);
    float yInset = 0.75f;
    map.drawTop = yInset;
    map.drawBottom = std::max(map.drawTop + 1.f, box.size.y - yInset);
    map.drawYDen = std::max(map.drawBottom - map.drawTop, 1.f);
    map.valid = map.drawBottom > map.drawTop;
    return map;
  }

  float currentRackZoom() const {
    if (APP && APP->scene && APP->scene->rackScroll) {
      return std::max(APP->scene->rackScroll->getZoom(), 1e-4f);
    }
    return 1.f;
  }

  bool refreshLastGoodMsg() {
    if (!module) {
      return false;
    }
    temporaldeck_expander::HostToDisplay msg;
    if (module->readSnapshotForUi(&msg)) {
      lastGoodMsg = msg;
      hasLastGoodMsg = true;
      lastGoodMsgTimeSec = system::getTime();
      return true;
    }
    return false;
  }

  bool beginLagDragAt(Vec pos) {
    if (!module || !isWithinDisplay(pos)) {
      return false;
    }
    refreshLastGoodMsg();
    if (!hasLastGoodMsg) {
      return false;
    }
    ScopeWindowMap map = buildScopeWindowMap(lastGoodMsg);
    if (!map.valid) {
      return false;
    }
    lagDragging = true;
    lagDragResidualY = 0.f;
    lagDragLastMoveSec = system::getTime();
    lagDragStationaryHoldActive = false;
    float anchorLagSamples = clamp(lastGoodMsg.lagSamples, 0.f, map.accessibleLag);
    lagDragAnchorLagSamples = anchorLagSamples;
    lagDragReferenceHeight = std::max(map.drawYDen, 1.f);
    lagDragTravelSamples = std::max(map.windowTopLag - map.windowBottomLag, 1.f);
    lagDragNormalizedOffset = 0.f;
    lagDragLocalLagSamples = anchorLagSamples;
    module->setLagDragRequest(true, lagDragLocalLagSamples, 0.f, true);
    return true;
  }

  void endLagDrag() {
    lagDragging = false;
    lagDragResidualY = 0.f;
    lagDragReferenceHeight = 1.f;
    lagDragTravelSamples = 0.f;
    lagDragNormalizedOffset = 0.f;
    lagDragStationaryHoldActive = false;
    if (module) {
      module->setLagDragRequest(false, 0.f, 0.f, false);
    }
  }

  void onButton(const event::Button &e) override {
    if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
      if (e.action == GLFW_PRESS && isWithinDisplay(e.pos)) {
        if (!isPhysicallyAttachedToTemporalDeck()) {
          Widget::onButton(e);
          return;
        }
        if (beginLagDragAt(e.pos)) {
          e.consume(this);
        }
        return;
      }
      if (e.action == GLFW_RELEASE && lagDragging) {
        endLagDrag();
        e.consume(this);
        return;
      }
    }
    Widget::onButton(e);
  }

  void onDragMove(const event::DragMove &e) override {
    if (!lagDragging || e.button != GLFW_MOUSE_BUTTON_LEFT || !module) {
      Widget::onDragMove(e);
      return;
    }
    refreshLastGoodMsg();
    if (!hasLastGoodMsg) {
      module->setLagDragRequest(true, lagDragLocalLagSamples, 0.f, true);
      e.consume(this);
      return;
    }
    double nowSec = system::getTime();
    constexpr double kMinGestureDtSec = 1.0 / 240.0;
    constexpr double kMaxGestureDtSec = 1.0 / 20.0;
    double dtSec = std::max(kMinGestureDtSec, std::min(kMaxGestureDtSec, nowSec - lagDragLastMoveSec));
    lagDragLastMoveSec = nowSec;

    constexpr float kLagDragJitterDeadzonePx = 0.05f;
    // DragMove has no local position, and Rack event recursion does not
    // localize mouseDelta. Convert screen/rack-scroll pixels into scope-local
    // pixels before normalizing by the visible widget height.
    float localMouseDeltaY = e.mouseDelta.y / currentRackZoom();
    lagDragResidualY += localMouseDeltaY;
    if (std::fabs(lagDragResidualY) < kLagDragJitterDeadzonePx) {
      module->setLagDragRequest(true, lagDragLocalLagSamples, 0.f, true);
      lagDragStationaryHoldActive = true;
      e.consume(this);
      return;
    }
    float appliedDeltaY = lagDragResidualY;
    lagDragResidualY = 0.f;
    float lagDragDirection = module->scopeVerticalInverted ? -1.f : 1.f;
    float signedDeltaY = appliedDeltaY * lagDragDirection;
    bool sampleMode = (lastGoodMsg.flags & temporaldeck_expander::FLAG_SAMPLE_MODE) != 0u;
    bool freezeActive = (lastGoodMsg.flags & temporaldeck_expander::FLAG_FREEZE) != 0u;
    if (lagDragStationaryHoldActive) {
      ScopeWindowMap holdMap = buildScopeWindowMap(lastGoodMsg);
      lagDragAnchorLagSamples = clamp(lastGoodMsg.lagSamples, 0.f, holdMap.accessibleLag);
      lagDragLocalLagSamples = lagDragAnchorLagSamples;
      lagDragNormalizedOffset = 0.f;
      lagDragStationaryHoldActive = false;
    }
    if (!sampleMode && !freezeActive && signedDeltaY > 0.f) {
      lagDragAnchorLagSamples += std::max(lastGoodMsg.sampleRate, 1.f) * float(dtSec);
    }
    lagDragNormalizedOffset += signedDeltaY / std::max(lagDragReferenceHeight, 1.f);
    ScopeWindowMap map = buildScopeWindowMap(lastGoodMsg);
    if (!map.valid) {
      e.consume(this);
      return;
    }

    float previousLag = lagDragLocalLagSamples;
    float desiredPlaybackLag = lagDragAnchorLagSamples + lagDragNormalizedOffset * lagDragTravelSamples;
    bool sampleLoop = (lastGoodMsg.flags & temporaldeck_expander::FLAG_SAMPLE_LOOP) != 0u;
    bool sampleLoaded = (lastGoodMsg.flags & temporaldeck_expander::FLAG_SAMPLE_LOADED) != 0u;
    lagDragLocalLagSamples = tdscope::solveLagDragPlaybackLag(
      desiredPlaybackLag,
      sampleMode,
      sampleLoaded,
      sampleLoop,
      freezeActive,
      map.accessibleLag,
      lastGoodMsgTimeSec,
      nowSec,
      lastGoodMsg.sampleRate);
    float velocitySamples = tdscope::computeLagDragVelocity(
      previousLag, lagDragLocalLagSamples, dtSec, lastGoodMsg.sampleRate);
    module->setLagDragRequest(true, lagDragLocalLagSamples, velocitySamples, false);
    e.consume(this);
  }

  void onDragEnd(const event::DragEnd &e) override {
    if (lagDragging && e.button == GLFW_MOUSE_BUTTON_LEFT) {
      endLagDrag();
      e.consume(this);
      return;
    }
    Widget::onDragEnd(e);
  }

  void step() override {
    Widget::step();
    if (!lagDragging || !module) {
      return;
    }
    double nowSec = system::getTime();
    if ((nowSec - lagDragLastMoveSec) >= kLagDragHoldDetectSec) {
      module->setLagDragRequest(true, lagDragLocalLagSamples, 0.f, true);
      lagDragStationaryHoldActive = true;
    }
  }
};

namespace tdscope {

Widget *createInput(TDScope *module, math::Rect scopeRectMm) {
  auto *input = new TDScopeInputWidget;
  input->module = module;
  input->box.pos = mm2px(scopeRectMm.pos);
  input->box.size = mm2px(scopeRectMm.size);
  return input;
}

} // namespace tdscope
