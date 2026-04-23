#include "TDScope.hpp"
#include <nanovg_gl.h>

#include <array>
#include <chrono>
#include <cstddef>
#include <cstring>
#include <functional>
#include <limits>
#include <vector>

struct TDScopeGlWidget final : widget::OpenGlWidget {
  struct LiveScopeBucket {
    float minNorm = 0.f;
    float maxNorm = 0.f;
    float coveredSamples = 0.f;
    float totalSamples = 1.f;
    bool hasData = false;
    int64_t key = std::numeric_limits<int64_t>::min();
    uint64_t updateSeq = 0;
  };
  struct GlLineVertex {
    GLfloat x;
    GLfloat y;
    GLubyte r;
    GLubyte g;
    GLubyte b;
    GLubyte a;
  };
  struct GlSegmentQuadVertex {
    GLfloat x;
    GLfloat y;
    GLubyte r;
    GLubyte g;
    GLubyte b;
    GLubyte a;
    GLfloat ax;
    GLfloat ay;
    GLfloat bx;
    GLfloat by;
    GLfloat radius;
  };
  struct GlFieldVertex {
    GLfloat x;
    GLfloat y;
  };

  TDScope *module = nullptr;
  temporaldeck_expander::HostToDisplay lastGoodMsg;
  bool hasLastGoodMsg = false;
  double lastGoodMsgTimeSec = -1.0;
  uint64_t redrawLastPublishSeq = 0;
  bool redrawHasFreshFrame = false;
  bool redrawLastLinkActive = false;
  bool redrawLastPreviewValid = false;
  float redrawLastRackZoom = 1.f;
  int redrawLastRangeMode = -1;
  int redrawLastChannelMode = -1;
  int redrawLastColorScheme = -1;
  bool redrawLastHaloEnabled = false;
  bool redrawLastMainTraceEnabled = false;
  bool redrawLastConnectorsEnabled = false;
  bool redrawLastStereoRightLaneEnabled = false;
  int redrawLastRenderMode = -1;
  bool fallbackRendererActive = false;
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
  std::vector<LiveScopeBucket> liveBucketsLeft;
  std::vector<LiveScopeBucket> liveBucketsRight;
  bool liveBucketsInitialized = false;
  bool liveBucketsStereo = false;
  int liveBucketCount = 0;
  float liveBucketSpanSamples = 0.f;
  uint64_t liveBucketLastPublishSeq = 0;
  float autoDisplayFullScaleVolts = 5.f;
  bool autoDisplayScaleInitialized = false;
  bool autoLastSampleMode = true;
  float autoLivePeakHoldVolts = 0.f;
  int autoLivePeakHoldFrames = 0;
  uint64_t cachedPublishSeq = 0;
  int cachedRowCount = 0;
  int cachedRangeMode = -1;
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
  std::array<std::vector<GlLineVertex>, 8> haloBatchVerts;
  std::array<std::vector<GlLineVertex>, 10> mainBatchVerts;
  std::array<std::vector<GlLineVertex>, 8> connectorBatchVerts;
  std::vector<GlSegmentQuadVertex> haloSegmentVerts;
  std::vector<GlSegmentQuadVertex> bodySegmentVerts;
  std::vector<GlSegmentQuadVertex> fillSegmentVerts;
  std::vector<GlSegmentQuadVertex> continuitySegmentVerts;
  bool shaderInitAttempted = false;
  bool shaderReady = false;
  GLuint shaderProgram = 0;
  GLuint shaderVertex = 0;
  GLuint shaderFragment = 0;
  GLuint shaderVbo = 0;
  GLint shaderUniformColorScale = -1;
  GLint shaderUniformColorLift = -1;
  GLint shaderUniformAlphaScale = -1;
  GLint shaderUniformAlphaGamma = -1;
  bool segmentShaderInitAttempted = false;
  bool segmentShaderReady = false;
  GLuint segmentShaderProgram = 0;
  GLuint segmentShaderVertex = 0;
  GLuint segmentShaderFragment = 0;
  GLuint segmentShaderVbo = 0;
  bool fieldShaderInitAttempted = false;
  bool fieldShaderReady = false;
  GLuint fieldShaderProgram = 0;
  GLuint fieldShaderVertex = 0;
  GLuint fieldShaderFragment = 0;
  GLuint fieldShaderVbo = 0;
  GLuint fieldRowTexture = 0;
  GLuint fieldColorLutTexture = 0;
  GLint fieldUniformRowTex = -1;
  GLint fieldUniformColorLutTex = -1;
  GLint fieldUniformRowCount = -1;
  GLint fieldUniformDrawTop = -1;
  GLint fieldUniformRowStep = -1;
  GLint fieldUniformZoomThickness = -1;
  GLint fieldUniformZoomInWidthComp = -1;
  GLint fieldUniformZoomInHaloWidthComp = -1;
  GLint fieldUniformZoomInAlphaComp = -1;
  GLint fieldUniformZoomInLiftComp = -1;
  GLint fieldUniformZoomInHaloAlphaComp = -1;
  GLint fieldUniformDeepZoomEnergyFill = -1;
  GLint fieldUniformRenderMain = -1;
  GLint fieldUniformRenderHalo = -1;
  GLint fieldUniformRenderContinuity = -1;
  std::vector<GLfloat> fieldRowData;
  int fieldRowTextureWidth = 0;
  int fieldColorLutScheme = -1;

  void step() override {
    if (!module || !module->useOpenGlGeometryRenderMode()) {
      setDirty();
      FramebufferWidget::step();
      return;
    }

    bool dirty = false;
    bool linkActive = module->uiLinkActive.load(std::memory_order_relaxed);
    bool previewValid = module->uiPreviewValid.load(std::memory_order_relaxed);
    if (linkActive != redrawLastLinkActive || previewValid != redrawLastPreviewValid) {
      dirty = true;
      redrawLastLinkActive = linkActive;
      redrawLastPreviewValid = previewValid;
    }

    float rackZoom = 1.f;
    if (APP && APP->scene && APP->scene->rackScroll) {
      rackZoom = std::max(APP->scene->rackScroll->getZoom(), 1e-4f);
    }
    if (std::fabs(rackZoom - redrawLastRackZoom) > 1e-4f) {
      dirty = true;
      redrawLastRackZoom = rackZoom;
    }
    if (module->scopeDisplayRangeMode != redrawLastRangeMode) {
      dirty = true;
      redrawLastRangeMode = module->scopeDisplayRangeMode;
    }
    if (module->scopeChannelMode != redrawLastChannelMode) {
      dirty = true;
      redrawLastChannelMode = module->scopeChannelMode;
    }
    if (module->scopeColorScheme != redrawLastColorScheme) {
      dirty = true;
      redrawLastColorScheme = module->scopeColorScheme;
    }
    if (module->scopeTransientHaloEnabled != redrawLastHaloEnabled) {
      dirty = true;
      redrawLastHaloEnabled = module->scopeTransientHaloEnabled;
    }
    if (module->debugRenderMainTraceEnabled != redrawLastMainTraceEnabled) {
      dirty = true;
      redrawLastMainTraceEnabled = module->debugRenderMainTraceEnabled;
    }
    if (module->debugRenderConnectorsEnabled != redrawLastConnectorsEnabled) {
      dirty = true;
      redrawLastConnectorsEnabled = module->debugRenderConnectorsEnabled;
    }
    if (module->debugRenderStereoRightLaneEnabled != redrawLastStereoRightLaneEnabled) {
      dirty = true;
      redrawLastStereoRightLaneEnabled = module->debugRenderStereoRightLaneEnabled;
    }
    if (module->debugRenderMode != redrawLastRenderMode) {
      dirty = true;
      redrawLastRenderMode = module->debugRenderMode;
    }

    temporaldeck_expander::HostToDisplay msg;
    bool snapshotOk = module->readSnapshotForUi(&msg);
    bool hasFreshFrame = snapshotOk && linkActive && previewValid;
    if (hasFreshFrame) {
      if (!redrawHasFreshFrame || msg.publishSeq != redrawLastPublishSeq) {
        dirty = true;
        redrawLastPublishSeq = msg.publishSeq;
      }
    } else if (redrawHasFreshFrame) {
      dirty = true;
    }
    redrawHasFreshFrame = hasFreshFrame;
    if (dirty) {
      setDirty();
    }
    FramebufferWidget::step();
  }

  void drawFramebuffer() override {
    auto drawStart = std::chrono::steady_clock::now();
    fallbackRendererActive = false;
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

    math::Vec fbSize = getFramebufferSize();
    glViewport(0, 0, std::max(1, int(std::lround(fbSize.x))), std::max(1, int(std::lround(fbSize.y))));
    glDisable(GL_SCISSOR_TEST);
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (!module || !module->useOpenGlGeometryRenderMode()) {
      publishUiDebugMetrics(0.f, 0);
      return;
    }

    bool linkActive = module->uiLinkActive.load(std::memory_order_relaxed);
    bool previewValid = module->uiPreviewValid.load(std::memory_order_relaxed);
    constexpr double kUiSnapshotGraceSec = 0.02;

    temporaldeck_expander::HostToDisplay msg;
    const bool snapshotOk = module->readSnapshotForUi(&msg);
    const double nowSec = system::getTime();
    if (snapshotOk) {
      lastGoodMsg = msg;
      hasLastGoodMsg = true;
      lastGoodMsgTimeSec = nowSec;
    } else if (linkActive && previewValid) {
      module->uiSnapshotReadMissCount.fetch_add(1u, std::memory_order_relaxed);
    }
    if (!snapshotOk || !linkActive || !previewValid) {
      bool canReuseLastGood =
        hasLastGoodMsg && lastGoodMsgTimeSec >= 0.0 && (nowSec - lastGoodMsgTimeSec) <= kUiSnapshotGraceSec;
      if (!canReuseLastGood) {
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
    bool renderStereo = (module->scopeChannelMode == TDScope::SCOPE_CHANNEL_STEREO) && hostStereoPayload;
    const temporaldeck_expander::ScopeBin *leftScopeBins = msg.scope;
    const temporaldeck_expander::ScopeBin *rightScopeBins = msg.scopeRight;

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
    const float laneGap = renderStereo ? 2.f : 0.f;
    const float laneWidth =
      renderStereo ? std::max((box.size.x - laneGap) * 0.5f, 1.f) : std::max(box.size.x, 1.f);
    const float lane0CenterX = renderStereo ? (laneWidth * 0.5f) : (box.size.x * 0.5f);
    const float lane1CenterX = renderStereo ? (laneWidth + laneGap + laneWidth * 0.5f) : lane0CenterX;
    const float laneAmpHalfWidth = laneWidth * 0.46f;
    const bool msgChanged = !cachedGeometryValid || msg.publishSeq != cachedPublishSeq;
    const bool rangeModeChanged = module->scopeDisplayRangeMode != cachedRangeMode;

    float displayFullScaleVolts = std::max(module->scopeDisplayFullScaleVolts(), 0.001f);
    if (module->scopeDisplayRangeMode == TDScope::SCOPE_RANGE_AUTO) {
      float targetFullScaleVolts = 5.f;
      if (!lowSignalWindow) {
        float basePeakVolts = std::max(peakWindowVolts * 1.22f, 0.25f);
        if (basePeakVolts <= 2.5f) {
          targetFullScaleVolts = 2.5f;
        } else if (basePeakVolts <= 5.f) {
          targetFullScaleVolts = 5.f;
        } else if (basePeakVolts <= 10.f) {
          targetFullScaleVolts = 10.f;
        } else {
          targetFullScaleVolts = basePeakVolts;
        }
      }
      bool sampleMode = (msg.flags & temporaldeck_expander::FLAG_SAMPLE_MODE) != 0u;
      if (!autoDisplayScaleInitialized || autoLastSampleMode != sampleMode) {
        autoDisplayFullScaleVolts = targetFullScaleVolts;
        autoDisplayScaleInitialized = true;
      } else {
        if (!sampleMode) {
          if (peakWindowVolts > autoLivePeakHoldVolts) {
            autoLivePeakHoldVolts = peakWindowVolts;
            autoLivePeakHoldFrames = 8;
          } else if (autoLivePeakHoldFrames > 0) {
            autoLivePeakHoldFrames -= 1;
          } else {
            autoLivePeakHoldVolts *= 0.94f;
          }
          float heldPeakVolts = std::max(peakWindowVolts, autoLivePeakHoldVolts);
          float heldTarget = std::max(heldPeakVolts * 1.22f, 0.25f);
          if (heldTarget <= 2.5f) {
            targetFullScaleVolts = 2.5f;
          } else if (heldTarget <= 5.f) {
            targetFullScaleVolts = 5.f;
          } else if (heldTarget <= 10.f) {
            targetFullScaleVolts = 10.f;
          } else {
            targetFullScaleVolts = heldTarget;
          }
        }
        float delta = targetFullScaleVolts - autoDisplayFullScaleVolts;
        if (std::fabs(delta) > 0.01f) {
          float kAutoScaleAttackAlpha = sampleMode ? 0.16f : 0.10f;
          float kAutoScaleReleaseAlpha = sampleMode ? 0.08f : 0.03f;
          float alpha = delta > 0.f ? kAutoScaleAttackAlpha : kAutoScaleReleaseAlpha;
          autoDisplayFullScaleVolts += delta * alpha;
        }
      }
      autoLastSampleMode = sampleMode;
      displayFullScaleVolts = autoDisplayFullScaleVolts;
    } else {
      autoDisplayScaleInitialized = false;
      autoLastSampleMode = true;
      autoLivePeakHoldVolts = 0.f;
      autoLivePeakHoldFrames = 0;
    }

    float scopeNormGain = temporaldeck_expander::kPreviewQuantizeVolts / displayFullScaleVolts;
    float rackZoom = 1.f;
    if (APP && APP->scene && APP->scene->rackScroll) {
      rackZoom = std::max(APP->scene->rackScroll->getZoom(), 1e-4f);
    }
    float zoomThicknessMul = clamp(1.f + 0.30f * std::log2(1.f / rackZoom), 0.70f, 1.42f);
    module->uiDebugScopeRackZoom.store(rackZoom, std::memory_order_relaxed);
    module->uiDebugScopeZoomThicknessMul.store(zoomThicknessMul, std::memory_order_relaxed);

    float halfWindowSamples = std::max(0.f, msg.scopeHalfWindowMs * 0.001f * std::max(msg.sampleRate, 1.f));
    float totalWindowSamples = std::max(1.f, 2.f * halfWindowSamples);
    bool sampleMode = (msg.flags & temporaldeck_expander::FLAG_SAMPLE_MODE) != 0u;
    float forwardWindowSamples = halfWindowSamples;
    float backwardWindowSamples = halfWindowSamples;
    if (!sampleMode) {
      forwardWindowSamples = std::min(halfWindowSamples, std::max(msg.lagSamples, 0.f));
      backwardWindowSamples = totalWindowSamples - forwardWindowSamples;
    }
    float windowTopLag = msg.lagSamples + backwardWindowSamples;
    float windowBottomLag = msg.lagSamples - forwardWindowSamples;
    float scopeBinSpanSamples = std::max(msg.scopeBinSpanSamples, 1e-6f);
    const int rowCount = 333;
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
      rowY.assign(rowCountU, drawTop);
      rowValid.assign(rowCountU, 0u);
      rowValidRight.assign(rowCountU, 0u);
      rowHoldFrames.assign(rowCountU, 0u);
      rowHoldFramesRight.assign(rowCountU, 0u);
      for (auto &verts : haloBatchVerts) {
        verts.clear();
        verts.reserve(size_t(rowCount / 2 + 8) * 2u);
      }
      for (auto &verts : mainBatchVerts) {
        verts.clear();
        verts.reserve(size_t(rowCount / 2 + 8) * 2u);
      }
      for (auto &verts : connectorBatchVerts) {
        verts.clear();
        verts.reserve(size_t(rowCount / 2 + 8) * 4u);
      }
      cachedGeometryValid = false;
    }

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

    auto rebuildLaneFromScopeBins =
      [&](const temporaldeck_expander::ScopeBin *scopeData, float laneCenterXLocal, float laneHalfWidthLocal,
          std::vector<float> *x0Out, std::vector<float> *x1Out, std::vector<float> *visualOut, std::vector<uint8_t> *validOut,
          std::vector<uint8_t> *holdOut) {
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
          float t0 = clamp(float(iy) / float(rowCount), 0.f, 1.f);
          float t1 = clamp(float(iy + 1) / float(rowCount), 0.f, 1.f);
          float rowMinNorm = 0.f;
          float rowMaxNorm = 0.f;
          if (!sampleEnvelopeOverInterval(scopeData, t0, t1, &rowMinNorm, &rowMaxNorm)) {
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
          (*visualOut)[idx] = clamp(std::pow(intensity, 0.68f) * 1.06f, 0.f, 1.f);
          (*validOut)[idx] = 1u;
          (*holdOut)[idx] = kRowTailHoldFrames;
        }
      };

    auto rebuildLaneFromLiveBuckets =
      [&](const std::vector<LiveScopeBucket> *buckets, float laneCenterXLocal, float laneHalfWidthLocal,
          std::vector<float> *x0Out, std::vector<float> *x1Out, std::vector<float> *visualOut, std::vector<uint8_t> *validOut,
          std::vector<uint8_t> *holdOut) {
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
              (*visualOut)[idx] = clamp(prevVisual * kGapIntensityDecay, 0.f, 1.f);
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
          float fillInfluence = 0.55f + 0.45f * clamp(fillFraction, 0.f, 1.f);
          (*visualOut)[idx] = clamp(std::pow(intensity, 0.68f) * 1.06f * fillInfluence, 0.f, 1.f);
          (*validOut)[idx] = 1u;
          (*holdOut)[idx] = kGapHoldFrames;
        }
      };

    using RowFloatAccessor = std::function<float(size_t)>;
    using RowValidAccessor = std::function<bool(size_t)>;
    auto rebuildTransientColorDrive = [&](const RowFloatAccessor &getX0, const RowFloatAccessor &getX1,
                                         const RowFloatAccessor &getVisualIntensity, const RowValidAccessor &isValid,
                                         float laneHalfWidthLocal, std::vector<float> *colorDriveOut) {
      if (!colorDriveOut || colorDriveOut->size() != rowCountU) {
        return;
      }
      float laneWidthDen = std::max(2.f * laneHalfWidthLocal, 1e-3f);
      auto updateRange = [&](int startRow, int endRow) {
        startRow = clamp(startRow, 0, rowCount - 1);
        endRow = clamp(endRow, 0, rowCount - 1);
        if (endRow < startRow) {
          return;
        }
        for (int iy = startRow; iy <= endRow; ++iy) {
          size_t idx = size_t(iy);
          float baseDrive = clamp(getVisualIntensity(idx), 0.f, 1.f);
          if (!isValid(idx)) {
            (*colorDriveOut)[idx] = 0.f;
            continue;
          }
          float center = 0.5f * (getX0(idx) + getX1(idx));
          float span = getX1(idx) - getX0(idx);
          float transientNorm = 0.f;
          auto accumulateTransientAgainst = [&](size_t otherIdx) {
            float otherCenter = 0.5f * (getX0(otherIdx) + getX1(otherIdx));
            float otherSpan = getX1(otherIdx) - getX0(otherIdx);
            transientNorm = std::max(transientNorm,
                                     std::max(std::fabs(center - otherCenter) / laneWidthDen * 1.15f,
                                              std::fabs(span - otherSpan) / laneWidthDen * 1.35f));
          };
          if (iy > 0 && isValid(size_t(iy - 1))) {
            accumulateTransientAgainst(size_t(iy - 1));
          }
          if (iy + 1 < rowCount && isValid(size_t(iy + 1))) {
            accumulateTransientAgainst(size_t(iy + 1));
          }
          float transientBoost = std::pow(clamp(transientNorm, 0.f, 1.f), 0.56f);
          float brightnessLift = clamp((0.38f + 0.92f * baseDrive) * std::pow(transientBoost, 0.84f), 0.f, 1.f);
          (*colorDriveOut)[idx] = brightnessLift;
        }
      };
      updateRange(0, rowCount - 1);
    };

    auto rebuildTransientColorDriveRange = [&](const RowFloatAccessor &getX0, const RowFloatAccessor &getX1,
                                               const RowFloatAccessor &getVisualIntensity,
                                               const RowValidAccessor &isValid, float laneHalfWidthLocal,
                                               std::vector<float> *colorDriveOut, int startRow, int endRow) {
      if (!colorDriveOut || colorDriveOut->size() != rowCountU) {
        return;
      }
      float laneWidthDen = std::max(2.f * laneHalfWidthLocal, 1e-3f);
      startRow = clamp(startRow, 0, rowCount - 1);
      endRow = clamp(endRow, 0, rowCount - 1);
      if (endRow < startRow) {
        return;
      }
      for (int iy = startRow; iy <= endRow; ++iy) {
        size_t idx = size_t(iy);
        float baseDrive = clamp(getVisualIntensity(idx), 0.f, 1.f);
        if (!isValid(idx)) {
          (*colorDriveOut)[idx] = 0.f;
          continue;
        }
        float center = 0.5f * (getX0(idx) + getX1(idx));
        float span = getX1(idx) - getX0(idx);
        float transientNorm = 0.f;
        auto accumulateTransientAgainst = [&](size_t otherIdx) {
          float otherCenter = 0.5f * (getX0(otherIdx) + getX1(otherIdx));
          float otherSpan = getX1(otherIdx) - getX0(otherIdx);
          transientNorm = std::max(transientNorm,
                                   std::max(std::fabs(center - otherCenter) / laneWidthDen * 1.15f,
                                            std::fabs(span - otherSpan) / laneWidthDen * 1.35f));
        };
        if (iy > 0 && isValid(size_t(iy - 1))) {
          accumulateTransientAgainst(size_t(iy - 1));
        }
        if (iy + 1 < rowCount && isValid(size_t(iy + 1))) {
          accumulateTransientAgainst(size_t(iy + 1));
        }
        float transientBoost = std::pow(clamp(transientNorm, 0.f, 1.f), 0.56f);
        float brightnessLift = clamp((0.38f + 0.92f * baseDrive) * std::pow(transientBoost, 0.84f), 0.f, 1.f);
        (*colorDriveOut)[idx] = brightnessLift;
      }
    };
    auto shiftVisibleColorDrive = [&](std::vector<float> *colorDriveOut, int shiftRows) {
      if (!colorDriveOut || colorDriveOut->size() != rowCountU || shiftRows == 0 || std::abs(shiftRows) >= rowCount) {
        if (colorDriveOut && colorDriveOut->size() == rowCountU && std::abs(shiftRows) >= rowCount) {
          std::fill(colorDriveOut->begin(), colorDriveOut->end(), 0.f);
        }
        return;
      }
      size_t count = size_t(rowCount - std::abs(shiftRows));
      if (shiftRows > 0) {
        std::memmove(colorDriveOut->data(), colorDriveOut->data() + shiftRows, count * sizeof(float));
        std::fill(colorDriveOut->begin() + (rowCount - shiftRows), colorDriveOut->end(), 0.f);
      } else {
        int downRows = -shiftRows;
        std::memmove(colorDriveOut->data() + downRows, colorDriveOut->data(), count * sizeof(float));
        std::fill(colorDriveOut->begin(), colorDriveOut->begin() + downRows, 0.f);
      }
    };

    for (int iy = 0; iy < rowCount; ++iy) {
      rowY[size_t(iy)] = drawTop + (float(iy) + 0.5f) * rowStep;
    }

    bool stereoLayoutChanged = cachedStereoLayout != renderStereo;
    bool useGeometryHistoryCache = module->debugRenderMode == TDScope::DEBUG_RENDER_OPENGL;
    bool shouldRebuild = !cachedGeometryValid || msgChanged || rangeModeChanged || cachedRowCount != rowCount ||
                         stereoLayoutChanged;
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
          float binPos0 = (msg.scopeStartLagSamples - lagHi) / scopeBinSpanSamples;
          float binPos1 = (msg.scopeStartLagSamples - lagLo) / scopeBinSpanSamples;
          float binPosMin = std::min(binPos0, binPos1);
          float binPosMax = std::max(binPos0, binPos1);
          float rowMinNorm = 0.f;
          float rowMaxNorm = 0.f;
          bool any = false;
          if (!(binPosMax < 0.f || binPosMin > float(scopeBinCount - 1u))) {
            int binIndex0 = std::max(0, int(std::floor(binPosMin)));
            int binIndex1 = std::min(int(scopeBinCount - 1u), int(std::ceil(binPosMax)));
            if (binIndex1 >= binIndex0) {
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
            }
          }
          if (!any) {
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
          (*visualOut)[idx] = clamp(std::pow(intensity, 0.68f) * 1.06f, 0.f, 1.f);
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
      int visibleShiftRows = 0;
      if (!fullHistoryRebuild && msg.publishSeq != cachedPublishSeq) {
        float shiftLagSamples =
          (historyReferenceWindowTopLag - windowTopLag) + (msg.scopeNewestPosSamples - historyNewestPosSamples);
        float exactShiftRows =
          historyShiftResidualRows + shiftLagSamples / std::max(totalWindowSamples / float(std::max(rowCount, 1)), 1e-6f);
        int shiftRows = int(std::trunc(exactShiftRows));
        historyShiftResidualRows = exactShiftRows - float(shiftRows);
        if (std::abs(shiftRows) >= historyMarginRows) {
          fullHistoryRebuild = true;
          historyShiftResidualRows = 0.f;
        } else if (shiftRows != 0) {
          visibleShiftRows = shiftRows;
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

      auto historyVisibleSlot = [&](size_t visibleIdx) {
        int slot = historyHeadRow + historyMarginRows + int(visibleIdx);
        slot %= historyCapacityRows;
        if (slot < 0) {
          slot += historyCapacityRows;
        }
        return size_t(slot);
      };
      auto historyGetX0 = [&](size_t visibleIdx) { return historyX0[historyVisibleSlot(visibleIdx)]; };
      auto historyGetX1 = [&](size_t visibleIdx) { return historyX1[historyVisibleSlot(visibleIdx)]; };
      auto historyGetVisual = [&](size_t visibleIdx) { return historyVisualIntensity[historyVisibleSlot(visibleIdx)]; };
      auto historyIsValid = [&](size_t visibleIdx) { return historyValid[historyVisibleSlot(visibleIdx)] != 0u; };
      auto historyGetX0Right = [&](size_t visibleIdx) { return historyX0Right[historyVisibleSlot(visibleIdx)]; };
      auto historyGetX1Right = [&](size_t visibleIdx) { return historyX1Right[historyVisibleSlot(visibleIdx)]; };
      auto historyGetVisualRight = [&](size_t visibleIdx) {
        return historyVisualIntensityRight[historyVisibleSlot(visibleIdx)];
      };
      auto historyIsValidRight = [&](size_t visibleIdx) { return historyValidRight[historyVisibleSlot(visibleIdx)] != 0u; };

      if (fullHistoryRebuild) {
        std::fill(rowColorDrive.begin(), rowColorDrive.end(), 0.f);
        if (renderStereo) {
          std::fill(rowColorDriveRight.begin(), rowColorDriveRight.end(), 0.f);
        }
      } else if (visibleShiftRows != 0) {
        // Preserve the already computed visible transient drive when history rows
        // shift, then only recompute exposed/new rows.
        shiftVisibleColorDrive(&rowColorDrive, visibleShiftRows);
        if (renderStereo) {
          shiftVisibleColorDrive(&rowColorDriveRight, visibleShiftRows);
        }
      }

      int visibleRebuildStart = 0;
      int visibleRebuildEnd = rowCount - 1;
      if (!fullHistoryRebuild) {
        visibleRebuildStart = clamp(rebuildStart - historyMarginRows, 0, rowCount - 1);
        visibleRebuildEnd = clamp(rebuildEnd - historyMarginRows, 0, rowCount - 1);
        if (visibleShiftRows > 0) {
          int exposedStart = std::max(0, rowCount - visibleShiftRows);
          visibleRebuildStart = std::min(visibleRebuildStart, exposedStart);
          visibleRebuildEnd = rowCount - 1;
        } else if (visibleShiftRows < 0) {
          int exposedEnd = std::min(rowCount - 1, -visibleShiftRows - 1);
          visibleRebuildStart = 0;
          visibleRebuildEnd = std::max(visibleRebuildEnd, exposedEnd);
        }
        // Include one neighbor row for local transient-delta continuity.
        visibleRebuildStart = std::max(0, visibleRebuildStart - 1);
        visibleRebuildEnd = std::min(rowCount - 1, visibleRebuildEnd + 1);
      }
      rebuildTransientColorDriveRange(
        historyGetX0, historyGetX1, historyGetVisual, historyIsValid, laneAmpHalfWidth, &rowColorDrive, visibleRebuildStart,
        visibleRebuildEnd);
      if (renderStereo) {
        rebuildTransientColorDriveRange(
          historyGetX0Right, historyGetX1Right, historyGetVisualRight, historyIsValidRight, laneAmpHalfWidth,
          &rowColorDriveRight,
          visibleRebuildStart, visibleRebuildEnd);
      } else {
        std::fill(rowColorDriveRight.begin(), rowColorDriveRight.end(), 0.f);
      }
      cachedPublishSeq = msg.publishSeq;
      cachedRowCount = rowCount;
      cachedRangeMode = module->scopeDisplayRangeMode;
      cachedStereoLayout = renderStereo;
      cachedGeometryValid = true;
      historyValidState = true;
      historyReferenceWindowTopLag = windowTopLag;
      historyReferenceWindowBottomLag = windowBottomLag;
      historyNewestPosSamples = msg.scopeNewestPosSamples;
      shouldRebuild = false;
    }

    if (shouldRebuild) {
      if (liveMode) {
        rebuildLaneFromLiveBuckets(
          &liveBucketsLeft, lane0CenterX, laneAmpHalfWidth, &rowX0, &rowX1, &rowVisualIntensity, &rowValid, &rowHoldFrames);
        if (renderStereo) {
          rebuildLaneFromLiveBuckets(&liveBucketsRight, lane1CenterX, laneAmpHalfWidth, &rowX0Right, &rowX1Right,
                                     &rowVisualIntensityRight, &rowValidRight, &rowHoldFramesRight);
        } else {
          std::fill(rowValidRight.begin(), rowValidRight.end(), 0u);
          std::fill(rowVisualIntensityRight.begin(), rowVisualIntensityRight.end(), 0.f);
          std::fill(rowHoldFramesRight.begin(), rowHoldFramesRight.end(), 0u);
        }
      } else {
        rebuildLaneFromScopeBins(
          leftScopeBins, lane0CenterX, laneAmpHalfWidth, &rowX0, &rowX1, &rowVisualIntensity, &rowValid, &rowHoldFrames);
        if (renderStereo) {
          rebuildLaneFromScopeBins(rightScopeBins, lane1CenterX, laneAmpHalfWidth, &rowX0Right, &rowX1Right,
                                   &rowVisualIntensityRight, &rowValidRight, &rowHoldFramesRight);
        } else {
          std::fill(rowValidRight.begin(), rowValidRight.end(), 0u);
          std::fill(rowVisualIntensityRight.begin(), rowVisualIntensityRight.end(), 0.f);
          std::fill(rowHoldFramesRight.begin(), rowHoldFramesRight.end(), 0u);
        }
      }

      rebuildTransientColorDrive(
        [&](size_t idx) { return rowX0[idx]; }, [&](size_t idx) { return rowX1[idx]; },
        [&](size_t idx) { return rowVisualIntensity[idx]; }, [&](size_t idx) { return rowValid[idx] != 0u; },
        laneAmpHalfWidth, &rowColorDrive);
      if (renderStereo) {
        rebuildTransientColorDrive(
          [&](size_t idx) { return rowX0Right[idx]; }, [&](size_t idx) { return rowX1Right[idx]; },
          [&](size_t idx) { return rowVisualIntensityRight[idx]; },
          [&](size_t idx) { return rowValidRight[idx] != 0u; }, laneAmpHalfWidth, &rowColorDriveRight);
      } else {
        std::fill(rowColorDriveRight.begin(), rowColorDriveRight.end(), 0.f);
      }

      cachedPublishSeq = msg.publishSeq;
      cachedRowCount = rowCount;
      cachedRangeMode = module->scopeDisplayRangeMode;
      cachedStereoLayout = renderStereo;
      cachedGeometryValid = true;
    }

    static std::array<std::array<NVGcolor, 256>, TDScope::COLOR_SCHEME_COUNT> colorLut;
    static std::array<uint8_t, TDScope::COLOR_SCHEME_COUNT> colorLutValid {};
    auto ensureColorLut = [&](int scheme) {
      scheme = clamp(scheme, 0, TDScope::COLOR_SCHEME_COUNT - 1);
      if (colorLutValid[size_t(scheme)]) {
        return;
      }
      float lowR = 85.f, lowG = 227.f, lowB = 238.f;
      float highR = 233.f, highG = 112.f, highB = 218.f;
      switch (scheme) {
        case TDScope::COLOR_SCHEME_EMERALD: lowR = 15.f; lowG = 79.f; lowB = 54.f; highR = 87.f; highG = 240.f; highB = 182.f; break;
        case TDScope::COLOR_SCHEME_WASP: lowR = 33.f; lowG = 27.f; lowB = 18.f; highR = 255.f; highG = 216.f; highB = 74.f; break;
        case TDScope::COLOR_SCHEME_PIXIE: lowR = 255.f; lowG = 143.f; lowB = 209.f; highR = 129.f; highG = 255.f; highB = 210.f; break;
        case TDScope::COLOR_SCHEME_VIOLET_FLAME: lowR = 42.f; lowG = 31.f; lowB = 95.f; highR = 181.f; highG = 109.f; highB = 255.f; break;
        case TDScope::COLOR_SCHEME_ANGELIC: lowR = 248.f; lowG = 245.f; lowB = 255.f; highR = 179.f; highG = 229.f; highB = 255.f; break;
        case TDScope::COLOR_SCHEME_HELLFIRE: lowR = 120.f; lowG = 24.f; lowB = 15.f; highR = 255.f; highG = 209.f; highB = 102.f; break;
        case TDScope::COLOR_SCHEME_PICKLE: lowR = 62.f; lowG = 111.f; lowB = 49.f; highR = 190.f; highG = 234.f; highB = 97.f; break;
        case TDScope::COLOR_SCHEME_LEVIATHAN: lowR = 122.f; lowG = 92.f; lowB = 255.f; highR = 92.f; highG = 190.f; highB = 246.f; break;
        default: break;
      }
      for (int i = 0; i < 256; ++i) {
        float intensity = float(i) / 255.f;
        float r = lowR + (highR - lowR) * intensity;
        float g = lowG + (highG - lowG) * intensity;
        float b = lowB + (highB - lowB) * intensity;
        colorLut[size_t(scheme)][size_t(i)] = nvgRGBA(uint8_t(std::lround(clamp(r, 0.f, 255.f))),
                                                      uint8_t(std::lround(clamp(g, 0.f, 255.f))),
                                                      uint8_t(std::lround(clamp(b, 0.f, 255.f))), 255);
      }
      colorLutValid[size_t(scheme)] = 1u;
    };
    auto gradientColorForIntensity = [&](float intensity, uint8_t alpha) -> NVGcolor {
      int scheme = clamp(module->scopeColorScheme, 0, TDScope::COLOR_SCHEME_COUNT - 1);
      ensureColorLut(scheme);
      int index = clamp(int(std::lround(clamp(intensity, 0.f, 1.f) * 255.f)), 0, 255);
      NVGcolor c = colorLut[size_t(scheme)][size_t(index)];
      float hotT = clamp((clamp(intensity, 0.f, 1.f) - 0.82f) / 0.18f, 0.f, 1.f);
      float hotLift = 0.24f * hotT * hotT;
      c.r = c.r + (1.f - c.r) * hotLift;
      c.g = c.g + (1.f - c.g) * hotLift;
      c.b = c.b + (1.f - c.b) * hotLift;
      c.a = float(alpha) / 255.f;
      return c;
    };
    auto brightenColor = [&](NVGcolor c, float lift) -> NVGcolor {
      lift = clamp(lift, 0.f, 1.f);
      c.r = c.r + (1.f - c.r) * lift;
      c.g = c.g + (1.f - c.g) * lift;
      c.b = c.b + (1.f - c.b) * lift;
      return c;
    };
    auto encodeColorByte = [](float v) -> GLubyte {
      return GLubyte(std::lround(clamp(v, 0.f, 1.f) * 255.f));
    };
    struct ShaderPassParams {
      float colorScale = 1.f;
      float colorLift = 0.f;
      float alphaScale = 1.f;
      float alphaGamma = 1.f;
    };
    auto makeShaderPassParams = [](float colorScale, float colorLift, float alphaScale, float alphaGamma) {
      ShaderPassParams params;
      params.colorScale = colorScale;
      params.colorLift = colorLift;
      params.alphaScale = alphaScale;
      params.alphaGamma = alphaGamma;
      return params;
    };
    auto initShaderPipeline = [&]() {
      if (shaderInitAttempted) {
        return shaderReady;
      }
      shaderInitAttempted = true;
      static const char *kVertexShaderSrc =
        "#version 120\n"
        "varying vec4 vColor;\n"
        "void main() {\n"
        "  gl_Position = ftransform();\n"
        "  vColor = gl_Color;\n"
        "}\n";
      static const char *kFragmentShaderSrc =
        "#version 120\n"
        "varying vec4 vColor;\n"
        "uniform float uColorScale;\n"
        "uniform float uColorLift;\n"
        "uniform float uAlphaScale;\n"
        "uniform float uAlphaGamma;\n"
        "void main() {\n"
        "  vec3 rgb = clamp(vColor.rgb * uColorScale, 0.0, 1.0);\n"
        "  rgb = rgb + (vec3(1.0) - rgb) * clamp(uColorLift, 0.0, 1.0);\n"
        "  float alpha = clamp(vColor.a, 0.0, 1.0);\n"
        "  alpha = pow(alpha, max(uAlphaGamma, 0.05));\n"
        "  alpha = clamp(alpha * uAlphaScale, 0.0, 1.0);\n"
        "  gl_FragColor = vec4(rgb, alpha);\n"
        "}\n";

      auto compileShader = [](GLenum type, const char *src) -> GLuint {
        GLuint shader = glCreateShader(type);
        if (!shader) {
          WARN("TDScopeGL field shader: glCreateShader failed for type=%u", unsigned(type));
          return 0;
        }
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);
        GLint status = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE) {
          GLint logLen = 0;
          glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
          std::vector<char> logBuf(size_t(std::max(logLen, 1)), '\0');
          GLsizei written = 0;
          glGetShaderInfoLog(shader, GLsizei(logBuf.size()), &written, logBuf.data());
          WARN("TDScopeGL field shader compile failed (type=%u): %s", unsigned(type), logBuf.data());
          glDeleteShader(shader);
          return 0;
        }
        return shader;
      };

      shaderVertex = compileShader(GL_VERTEX_SHADER, kVertexShaderSrc);
      shaderFragment = compileShader(GL_FRAGMENT_SHADER, kFragmentShaderSrc);
      if (!shaderVertex || !shaderFragment) {
        if (shaderVertex) {
          glDeleteShader(shaderVertex);
          shaderVertex = 0;
        }
        if (shaderFragment) {
          glDeleteShader(shaderFragment);
          shaderFragment = 0;
        }
        return false;
      }

      shaderProgram = glCreateProgram();
      if (!shaderProgram) {
        glDeleteShader(shaderVertex);
        glDeleteShader(shaderFragment);
        shaderVertex = 0;
        shaderFragment = 0;
        return false;
      }
      glAttachShader(shaderProgram, shaderVertex);
      glAttachShader(shaderProgram, shaderFragment);
      glLinkProgram(shaderProgram);
      GLint linkStatus = GL_FALSE;
      glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);
      if (linkStatus != GL_TRUE) {
        glDeleteProgram(shaderProgram);
        glDeleteShader(shaderVertex);
        glDeleteShader(shaderFragment);
        shaderProgram = 0;
        shaderVertex = 0;
        shaderFragment = 0;
        return false;
      }

      glGenBuffers(1, &shaderVbo);
      if (shaderVbo == 0) {
        glDeleteProgram(shaderProgram);
        glDeleteShader(shaderVertex);
        glDeleteShader(shaderFragment);
        shaderProgram = 0;
        shaderVertex = 0;
        shaderFragment = 0;
        return false;
      }

      shaderUniformColorScale = glGetUniformLocation(shaderProgram, "uColorScale");
      shaderUniformColorLift = glGetUniformLocation(shaderProgram, "uColorLift");
      shaderUniformAlphaScale = glGetUniformLocation(shaderProgram, "uAlphaScale");
      shaderUniformAlphaGamma = glGetUniformLocation(shaderProgram, "uAlphaGamma");
      if (shaderUniformColorScale < 0 || shaderUniformColorLift < 0 || shaderUniformAlphaScale < 0 ||
          shaderUniformAlphaGamma < 0) {
        glDeleteBuffers(1, &shaderVbo);
        glDeleteProgram(shaderProgram);
        glDeleteShader(shaderVertex);
        glDeleteShader(shaderFragment);
        shaderProgram = 0;
        shaderVertex = 0;
        shaderFragment = 0;
        shaderVbo = 0;
        shaderUniformColorScale = -1;
        shaderUniformColorLift = -1;
        shaderUniformAlphaScale = -1;
        shaderUniformAlphaGamma = -1;
        return false;
      }

      shaderReady = true;
      return true;
    };
    auto initSegmentShaderPipeline = [&]() {
      if (segmentShaderInitAttempted) {
        return segmentShaderReady;
      }
      segmentShaderInitAttempted = true;
      static const GLuint kAttrPos = 0;
      static const GLuint kAttrColor = 1;
      static const GLuint kAttrSegment = 2;
      static const GLuint kAttrRadius = 3;
      static const char *kVertexShaderSrc =
        "#version 120\n"
        "attribute vec2 aPos;\n"
        "attribute vec4 aColor;\n"
        "attribute vec4 aSegment;\n"
        "attribute float aRadius;\n"
        "varying vec2 vLocalPos;\n"
        "varying vec4 vColor;\n"
        "varying vec2 vSegA;\n"
        "varying vec2 vSegB;\n"
        "varying float vRadius;\n"
        "void main() {\n"
        "  gl_Position = gl_ModelViewProjectionMatrix * vec4(aPos, 0.0, 1.0);\n"
        "  vLocalPos = aPos;\n"
        "  vColor = aColor;\n"
        "  vSegA = aSegment.xy;\n"
        "  vSegB = aSegment.zw;\n"
        "  vRadius = max(aRadius, 0.001);\n"
        "}\n";
      static const char *kFragmentShaderSrc =
        "#version 120\n"
        "varying vec2 vLocalPos;\n"
        "varying vec4 vColor;\n"
        "varying vec2 vSegA;\n"
        "varying vec2 vSegB;\n"
        "varying float vRadius;\n"
        "void main() {\n"
        "  vec2 pa = vLocalPos - vSegA;\n"
        "  vec2 ba = vSegB - vSegA;\n"
        "  float denom = max(dot(ba, ba), 1e-6);\n"
        "  float h = clamp(dot(pa, ba) / denom, 0.0, 1.0);\n"
        "  float dist = length(pa - ba * h);\n"
        "  float sigma = max(vRadius * 0.70, 0.001);\n"
        "  float alpha = clamp(vColor.a * exp(-0.5 * (dist * dist) / (sigma * sigma)), 0.0, 1.0);\n"
        "  gl_FragColor = vec4(vColor.rgb, alpha);\n"
        "}\n";

      auto compileShader = [](GLenum type, const char *src) -> GLuint {
        GLuint shader = glCreateShader(type);
        if (!shader) {
          return 0;
        }
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);
        GLint status = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE) {
          glDeleteShader(shader);
          return 0;
        }
        return shader;
      };

      segmentShaderVertex = compileShader(GL_VERTEX_SHADER, kVertexShaderSrc);
      segmentShaderFragment = compileShader(GL_FRAGMENT_SHADER, kFragmentShaderSrc);
      if (!segmentShaderVertex || !segmentShaderFragment) {
        if (segmentShaderVertex) {
          glDeleteShader(segmentShaderVertex);
          segmentShaderVertex = 0;
        }
        if (segmentShaderFragment) {
          glDeleteShader(segmentShaderFragment);
          segmentShaderFragment = 0;
        }
        return false;
      }

      segmentShaderProgram = glCreateProgram();
      if (!segmentShaderProgram) {
        glDeleteShader(segmentShaderVertex);
        glDeleteShader(segmentShaderFragment);
        segmentShaderVertex = 0;
        segmentShaderFragment = 0;
        return false;
      }
      glAttachShader(segmentShaderProgram, segmentShaderVertex);
      glAttachShader(segmentShaderProgram, segmentShaderFragment);
      glBindAttribLocation(segmentShaderProgram, kAttrPos, "aPos");
      glBindAttribLocation(segmentShaderProgram, kAttrColor, "aColor");
      glBindAttribLocation(segmentShaderProgram, kAttrSegment, "aSegment");
      glBindAttribLocation(segmentShaderProgram, kAttrRadius, "aRadius");
      glLinkProgram(segmentShaderProgram);
      GLint linkStatus = GL_FALSE;
      glGetProgramiv(segmentShaderProgram, GL_LINK_STATUS, &linkStatus);
      if (linkStatus != GL_TRUE) {
        glDeleteProgram(segmentShaderProgram);
        glDeleteShader(segmentShaderVertex);
        glDeleteShader(segmentShaderFragment);
        segmentShaderProgram = 0;
        segmentShaderVertex = 0;
        segmentShaderFragment = 0;
        return false;
      }

      glGenBuffers(1, &segmentShaderVbo);
      if (segmentShaderVbo == 0) {
        glDeleteProgram(segmentShaderProgram);
        glDeleteShader(segmentShaderVertex);
        glDeleteShader(segmentShaderFragment);
        segmentShaderProgram = 0;
        segmentShaderVertex = 0;
        segmentShaderFragment = 0;
        return false;
      }

      segmentShaderReady = true;
      return true;
    };
    auto initFieldShaderPipeline = [&]() {
      if (fieldShaderInitAttempted) {
        return fieldShaderReady;
      }
      fieldShaderInitAttempted = true;
      static const GLuint kAttrPos = 0;
      static const char *kVertexShaderSrc =
        "#version 120\n"
        "attribute vec2 aPos;\n"
        "varying vec2 vLocalPos;\n"
        "void main() {\n"
        "  gl_Position = gl_ModelViewProjectionMatrix * vec4(aPos, 0.0, 1.0);\n"
        "  vLocalPos = aPos;\n"
        "}\n";
      static const char *kFragmentShaderSrc =
        "#version 120\n"
        "uniform sampler2D uRowTex;\n"
        "uniform sampler2D uColorLutTex;\n"
        "uniform float uRowCount;\n"
        "uniform float uDrawTop;\n"
        "uniform float uRowStep;\n"
        "uniform float uZoomThickness;\n"
        "uniform float uZoomInWidthComp;\n"
        "uniform float uZoomInHaloWidthComp;\n"
        "uniform float uZoomInAlphaComp;\n"
        "uniform float uZoomInLiftComp;\n"
        "uniform float uZoomInHaloAlphaComp;\n"
        "uniform float uDeepZoomEnergyFill;\n"
        "uniform float uRenderMain;\n"
        "uniform float uRenderHalo;\n"
        "uniform float uRenderContinuity;\n"
        "varying vec2 vLocalPos;\n"
        "vec4 fetchRow(float idx) {\n"
        "  float t = (clamp(idx, 0.0, uRowCount - 1.0) + 0.5) / max(uRowCount, 1.0);\n"
        "  return texture2D(uRowTex, vec2(t, 0.5));\n"
        "}\n"
        "bool rowValid(vec4 row) {\n"
        "  return row.z >= 0.0;\n"
        "}\n"
        "vec4 gradientColor(float intensity, float alpha) {\n"
        "  float t = clamp(intensity, 0.0, 1.0);\n"
        "  vec4 c = texture2D(uColorLutTex, vec2(t, 0.5));\n"
        "  c.a = alpha;\n"
        "  return c;\n"
        "}\n"
        "vec4 brightenColor(vec4 c, float lift) {\n"
        "  lift = clamp(lift, 0.0, 1.0);\n"
        "  c.rgb = c.rgb + (vec3(1.0) - c.rgb) * lift;\n"
        "  return c;\n"
        "}\n"
        "float segmentDistance(vec2 p, vec2 a, vec2 b) {\n"
        "  vec2 pa = p - a;\n"
        "  vec2 ba = b - a;\n"
        "  float denom = max(dot(ba, ba), 1e-6);\n"
        "  float h = clamp(dot(pa, ba) / denom, 0.0, 1.0);\n"
        "  return length(pa - ba * h);\n"
        "}\n"
        "float gaussianAlpha(float dist, float radius) {\n"
        "  float sigma = max(radius * 0.70, 0.001);\n"
        "  return exp(-0.5 * (dist * dist) / (sigma * sigma));\n"
        "}\n"
        "float gaussianAlphaTight(float dist, float radius, float sigmaScale) {\n"
        "  float sigma = max(radius * sigmaScale, 0.001);\n"
        "  return exp(-0.5 * (dist * dist) / (sigma * sigma));\n"
        "}\n"
        "void accumulateRow(vec2 p, vec4 row, float rowIdx, inout vec3 baseRgb, inout float baseAlphaMax,\n"
        "                   inout vec3 haloRgb, inout float haloAlphaMax) {\n"
        "  if (!rowValid(row)) {\n"
        "    return;\n"
        "  }\n"
        "  float y = uDrawTop + (rowIdx + 0.5) * uRowStep;\n"
        "  float x0 = row.x;\n"
        "  float x1 = row.y;\n"
        "  float visual = clamp(row.z, 0.0, 1.0);\n"
        "  float transientLift = clamp(row.w, 0.0, 1.0);\n"
        "  float tone = clamp(0.78 * visual + 0.22 * transientLift, 0.0, 1.0);\n"
        "  float colorVisual = tone;\n"
        "  float mainAlpha = clamp(((122.0 + 120.0 * colorVisual) / 255.0) * 1.16 * uZoomInAlphaComp, 0.0, 1.0);\n"
        "  vec4 mainColor = gradientColor(colorVisual, mainAlpha);\n"
        "  float mainHotT = clamp((colorVisual - 0.82) / 0.18, 0.0, 1.0);\n"
        "  float mainHotLift = 0.24 * mainHotT * mainHotT;\n"
        "  mainColor.rgb = mainColor.rgb + (vec3(1.0) - mainColor.rgb) * mainHotLift;\n"
        "  mainColor = brightenColor(mainColor, clamp(transientLift * 0.90, 0.0, 1.0));\n"
        "  float mainW = (0.78 + 0.62 * tone) * uZoomThickness * 1.10 * uZoomInWidthComp;\n"
        "  float lowVisualBoost = 1.0 + 0.32 * (1.0 - clamp(visual, 0.0, 1.0));\n"
        "  mainW *= lowVisualBoost;\n"
        "  float maxMainW = max(uRowStep * 0.82, 0.70);\n"
        "  mainW = min(mainW, maxMainW);\n"
        "  float mainRadius = max(mainW * 0.55, 0.40);\n"
        "  float dist = segmentDistance(p, vec2(x0, y), vec2(x1, y));\n"
        "  float mainCovCore = gaussianAlpha(dist, mainRadius * 0.82);\n"
        "  float mainCovSoft = gaussianAlpha(dist, mainRadius * 1.25);\n"
        "  float mainCov = clamp(0.72 * mainCovCore + 0.28 * mainCovSoft, 0.0, 1.0);\n"
        "  if (uRenderMain > 0.5) {\n"
        "    float widthFade = 1.0 / (1.0 + max(mainW - 1.25, 0.0) * 0.34);\n"
        "    float mainPremult = mainColor.a * mainCov * widthFade;\n"
        "    baseRgb += mainColor.rgb * mainPremult;\n"
        "    baseAlphaMax = max(baseAlphaMax, mainPremult);\n"
        "  }\n"
        "  if (uRenderHalo > 0.5) {\n"
        "    float haloLinear = clamp((transientLift - 0.030) / 0.800, 0.0, 1.0);\n"
        "    float haloT = haloLinear * haloLinear;\n"
        "    float haloAlpha = ((88.0 + 196.0 * max(visual, 0.24)) / 255.0) * haloT;\n"
        "    float boostedHaloAlpha = clamp(haloAlpha * 0.98 * uZoomInHaloAlphaComp, 0.0, 1.0);\n"
        "    if (haloAlpha >= (26.0 / 255.0) && boostedHaloAlpha > 0.001) {\n"
        "      float haloW = (mainW + (1.10 + 2.20 * haloT) * uZoomThickness) * 1.10 * uZoomInHaloWidthComp;\n"
        "      float haloRadius = max(haloW * 0.48, mainRadius + 0.18);\n"
        "      float haloCov = gaussianAlphaTight(segmentDistance(p, vec2(x0, y), vec2(x1, y)), haloRadius, 0.54);\n"
        "      float haloPremult = boostedHaloAlpha * haloCov;\n"
        "      haloRgb += vec3(1.0) * haloPremult;\n"
        "      haloAlphaMax = max(haloAlphaMax, haloPremult);\n"
        "    }\n"
        "  }\n"
        "}\n"
        "void accumulateContinuity(vec2 p, vec4 rowA, float idxA, vec4 rowB, float idxB, inout vec3 baseRgb,\n"
        "                          inout float baseAlphaMax) {\n"
        "  if (!(uRenderContinuity > 0.5) || !rowValid(rowA) || !rowValid(rowB)) {\n"
        "    return;\n"
        "  }\n"
        "  float x0a = rowA.x;\n"
        "  float x1a = rowA.y;\n"
        "  float x0b = rowB.x;\n"
        "  float x1b = rowB.y;\n"
        "  float delta = max(abs(x0b - x0a), abs(x1b - x1a));\n"
        "  float connectorMinDelta = max(0.60 * uZoomThickness, 0.40);\n"
        "  if (delta < connectorMinDelta) {\n"
        "    return;\n"
        "  }\n"
        "  float prevVisual = clamp(rowA.z, 0.0, 1.0);\n"
        "  float visual = clamp(rowB.z, 0.0, 1.0);\n"
        "  float prevDrive = clamp(rowA.w, 0.0, 1.0);\n"
        "  float drive = clamp(rowB.w, 0.0, 1.0);\n"
        "  float connectVisual = clamp(0.5 * (prevVisual + visual), 0.0, 1.0);\n"
        "  float connectTransientLift = clamp(0.5 * (prevDrive + drive), 0.0, 1.0);\n"
        "  float connectTone = clamp(0.82 * connectVisual + 0.18 * connectTransientLift, 0.0, 1.0);\n"
        "  float connectColorVisual = connectTone;\n"
        "  vec4 c = gradientColor(connectColorVisual,\n"
        "                         clamp(((88.0 + 92.0 * connectColorVisual) / 255.0) * uZoomInAlphaComp, 0.0, 1.0));\n"
        "  float connectHotT = clamp((connectColorVisual - 0.82) / 0.18, 0.0, 1.0);\n"
        "  float connectHotLift = 0.24 * connectHotT * connectHotT;\n"
        "  c.rgb = c.rgb + (vec3(1.0) - c.rgb) * connectHotLift;\n"
        "  c = brightenColor(c, clamp(connectTransientLift * 0.72, 0.0, 1.0));\n"
        "  float prevCenter = 0.5 * (x0a + x1a);\n"
        "  float center = 0.5 * (x0b + x1b);\n"
        "  float centerDrift = abs(center - prevCenter);\n"
        "  float centerSpan = 0.5 * (abs(x1a - x0a) + abs(x1b - x0b));\n"
        "  if (centerSpan <= connectorMinDelta * 1.2 || centerDrift <= connectorMinDelta * 0.70) {\n"
        "    return;\n"
        "  }\n"
        "  float driftT = clamp((centerDrift - connectorMinDelta * 0.70) / max(connectorMinDelta * 1.80, 1e-4), 0.0, 1.0);\n"
        "  float continuityRadius = max((0.80 + 0.40 * connectTone) * uZoomThickness * 1.04 *\n"
        "                               (1.02 + 0.08 * uZoomInWidthComp) * (1.01 + 0.06 * uDeepZoomEnergyFill) * 0.52,\n"
        "                               0.45);\n"
        "  float yA = uDrawTop + (idxA + 0.5) * uRowStep;\n"
        "  float yB = uDrawTop + (idxB + 0.5) * uRowStep;\n"
        "  float contCov = gaussianAlpha(segmentDistance(p, vec2(prevCenter, yA), vec2(center, yB)), continuityRadius);\n"
        "  float contAlpha = clamp(c.a * (0.28 + 0.06 * uDeepZoomEnergyFill) * driftT, 0.0, 1.0) * contCov;\n"
        "  baseRgb += c.rgb * contAlpha;\n"
        "  baseAlphaMax = max(baseAlphaMax, contAlpha);\n"
        "}\n"
        "void main() {\n"
        "  vec2 p = vLocalPos;\n"
        "  float rowPos = ((p.y - uDrawTop) / max(uRowStep, 1e-6)) - 0.5;\n"
        "  float i0 = floor(rowPos);\n"
        "  float i1 = i0 + 1.0;\n"
        "  vec4 row0 = fetchRow(i0);\n"
        "  vec4 row1 = fetchRow(i1);\n"
        "  vec4 rowPrev = fetchRow(i0 - 1.0);\n"
        "  vec3 baseRgb = vec3(0.0);\n"
        "  vec3 haloRgb = vec3(0.0);\n"
        "  float baseAlphaMax = 0.0;\n"
        "  float haloAlphaMax = 0.0;\n"
        "  accumulateRow(p, row0, i0, baseRgb, baseAlphaMax, haloRgb, haloAlphaMax);\n"
        "  accumulateRow(p, row1, i1, baseRgb, baseAlphaMax, haloRgb, haloAlphaMax);\n"
        "  accumulateContinuity(p, rowPrev, i0 - 1.0, row0, i0, baseRgb, baseAlphaMax);\n"
        "  accumulateContinuity(p, row0, i0, row1, i1, baseRgb, baseAlphaMax);\n"
        "  bool haloOnly = (uRenderHalo > 0.5) && !(uRenderMain > 0.5) && !(uRenderContinuity > 0.5);\n"
        "  if (haloOnly) {\n"
        "    vec3 haloOut = clamp(haloRgb, 0.0, 1.0);\n"
        "    float haloAlpha = clamp(haloAlphaMax, 0.0, 1.0);\n"
        "    gl_FragColor = vec4(haloOut, haloAlpha);\n"
        "  } else {\n"
        "    vec3 baseOut = clamp(baseRgb, 0.0, 1.0);\n"
        "    float baseAlpha = clamp(baseAlphaMax, 0.0, 1.0);\n"
        "    gl_FragColor = vec4(baseOut, baseAlpha);\n"
        "  }\n"
        "}\n";

      auto compileShader = [](GLenum type, const char *src) -> GLuint {
        GLuint shader = glCreateShader(type);
        if (!shader) {
          return 0;
        }
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);
        GLint status = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE) {
          glDeleteShader(shader);
          return 0;
        }
        return shader;
      };

      fieldShaderVertex = compileShader(GL_VERTEX_SHADER, kVertexShaderSrc);
      fieldShaderFragment = compileShader(GL_FRAGMENT_SHADER, kFragmentShaderSrc);
      if (!fieldShaderVertex || !fieldShaderFragment) {
        if (fieldShaderVertex) {
          glDeleteShader(fieldShaderVertex);
          fieldShaderVertex = 0;
        }
        if (fieldShaderFragment) {
          glDeleteShader(fieldShaderFragment);
          fieldShaderFragment = 0;
        }
        return false;
      }

      fieldShaderProgram = glCreateProgram();
      if (!fieldShaderProgram) {
        glDeleteShader(fieldShaderVertex);
        glDeleteShader(fieldShaderFragment);
        fieldShaderVertex = 0;
        fieldShaderFragment = 0;
        return false;
      }
      glAttachShader(fieldShaderProgram, fieldShaderVertex);
      glAttachShader(fieldShaderProgram, fieldShaderFragment);
      glBindAttribLocation(fieldShaderProgram, kAttrPos, "aPos");
      glLinkProgram(fieldShaderProgram);
      GLint linkStatus = GL_FALSE;
      glGetProgramiv(fieldShaderProgram, GL_LINK_STATUS, &linkStatus);
      if (linkStatus != GL_TRUE) {
        GLint logLen = 0;
        glGetProgramiv(fieldShaderProgram, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> logBuf(size_t(std::max(logLen, 1)), '\0');
        GLsizei written = 0;
        glGetProgramInfoLog(fieldShaderProgram, GLsizei(logBuf.size()), &written, logBuf.data());
        WARN("TDScopeGL field shader link failed: %s", logBuf.data());
        glDeleteProgram(fieldShaderProgram);
        glDeleteShader(fieldShaderVertex);
        glDeleteShader(fieldShaderFragment);
        fieldShaderProgram = 0;
        fieldShaderVertex = 0;
        fieldShaderFragment = 0;
        return false;
      }

      glGenBuffers(1, &fieldShaderVbo);
      glGenTextures(1, &fieldRowTexture);
      glGenTextures(1, &fieldColorLutTexture);
      if (!fieldShaderVbo || !fieldRowTexture || !fieldColorLutTexture) {
        if (fieldShaderVbo) {
          glDeleteBuffers(1, &fieldShaderVbo);
          fieldShaderVbo = 0;
        }
        if (fieldRowTexture) {
          glDeleteTextures(1, &fieldRowTexture);
          fieldRowTexture = 0;
        }
        if (fieldColorLutTexture) {
          glDeleteTextures(1, &fieldColorLutTexture);
          fieldColorLutTexture = 0;
        }
        glDeleteProgram(fieldShaderProgram);
        glDeleteShader(fieldShaderVertex);
        glDeleteShader(fieldShaderFragment);
        fieldShaderProgram = 0;
        fieldShaderVertex = 0;
        fieldShaderFragment = 0;
        return false;
      }

      fieldUniformRowTex = glGetUniformLocation(fieldShaderProgram, "uRowTex");
      fieldUniformColorLutTex = glGetUniformLocation(fieldShaderProgram, "uColorLutTex");
      fieldUniformRowCount = glGetUniformLocation(fieldShaderProgram, "uRowCount");
      fieldUniformDrawTop = glGetUniformLocation(fieldShaderProgram, "uDrawTop");
      fieldUniformRowStep = glGetUniformLocation(fieldShaderProgram, "uRowStep");
      fieldUniformZoomThickness = glGetUniformLocation(fieldShaderProgram, "uZoomThickness");
      fieldUniformZoomInWidthComp = glGetUniformLocation(fieldShaderProgram, "uZoomInWidthComp");
      fieldUniformZoomInHaloWidthComp = glGetUniformLocation(fieldShaderProgram, "uZoomInHaloWidthComp");
      fieldUniformZoomInAlphaComp = glGetUniformLocation(fieldShaderProgram, "uZoomInAlphaComp");
      fieldUniformZoomInLiftComp = glGetUniformLocation(fieldShaderProgram, "uZoomInLiftComp");
      fieldUniformZoomInHaloAlphaComp = glGetUniformLocation(fieldShaderProgram, "uZoomInHaloAlphaComp");
      fieldUniformDeepZoomEnergyFill = glGetUniformLocation(fieldShaderProgram, "uDeepZoomEnergyFill");
      fieldUniformRenderMain = glGetUniformLocation(fieldShaderProgram, "uRenderMain");
      fieldUniformRenderHalo = glGetUniformLocation(fieldShaderProgram, "uRenderHalo");
      fieldUniformRenderContinuity = glGetUniformLocation(fieldShaderProgram, "uRenderContinuity");
      if (fieldUniformRowTex < 0 || fieldUniformColorLutTex < 0 || fieldUniformRowCount < 0 ||
          fieldUniformDrawTop < 0 || fieldUniformRowStep < 0 || fieldUniformZoomThickness < 0 ||
          fieldUniformZoomInWidthComp < 0 ||
          fieldUniformZoomInHaloWidthComp < 0 || fieldUniformZoomInAlphaComp < 0 ||
          fieldUniformZoomInHaloAlphaComp < 0 ||
          fieldUniformDeepZoomEnergyFill < 0 || fieldUniformRenderMain < 0 || fieldUniformRenderHalo < 0 ||
          fieldUniformRenderContinuity < 0) {
        WARN("TDScopeGL field shader uniform lookup failed: rowTex=%d colorLut=%d rowCount=%d drawTop=%d rowStep=%d zoomThickness=%d "
             "zoomInWidth=%d zoomInHaloWidth=%d zoomInAlpha=%d zoomInLift=%d zoomInHaloAlpha=%d deepZoomFill=%d renderMain=%d "
             "renderHalo=%d renderContinuity=%d",
             fieldUniformRowTex, fieldUniformColorLutTex, fieldUniformRowCount, fieldUniformDrawTop, fieldUniformRowStep,
             fieldUniformZoomThickness, fieldUniformZoomInWidthComp, fieldUniformZoomInHaloWidthComp,
             fieldUniformZoomInAlphaComp, fieldUniformZoomInLiftComp, fieldUniformZoomInHaloAlphaComp,
             fieldUniformDeepZoomEnergyFill, fieldUniformRenderMain, fieldUniformRenderHalo, fieldUniformRenderContinuity);
        return false;
      }

      glBindTexture(GL_TEXTURE_2D, fieldRowTexture);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glBindTexture(GL_TEXTURE_2D, fieldColorLutTexture);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glBindTexture(GL_TEXTURE_2D, 0);

      fieldShaderReady = true;
      return true;
    };
    constexpr int kGlHaloStrokeBins = 8;
    constexpr int kGlMainStrokeBins = 10;
    constexpr int kGlConnectorStrokeBins = 8;
    constexpr float kGlMainAlphaGain = 1.26f;
    constexpr float kGlMainLiftGain = 1.16f;
    constexpr float kGlConnectorAlphaGain = 1.14f;
    constexpr float kGlConnectorLiftGain = 1.12f;
    constexpr float kGlHaloAlphaGain = 1.34f;
    constexpr float kGlHaloWidthGain = 1.10f;
    constexpr float kGlMainWidthGain = 1.10f;
    constexpr float kGlConnectorWidthGain = 1.08f;
    constexpr float kGlDeepZoomEnergyFillAlpha = 0.24f;
    auto drawLane = [&](const RowFloatAccessor &getX0, const RowFloatAccessor &getX1,
                        const RowFloatAccessor &getVisualIntensity, const std::vector<float> &colorDrive,
                        const RowValidAccessor &isValid, float laneCenterXForConnectors) {
      constexpr uint8_t kHaloMinAlphaToDraw = 28u;
      auto quantizeStrokeBin = [&](float t, int binCount) -> int {
        t = clamp(t, 0.f, 1.f);
        return clamp(int(std::floor(t * float(binCount))), 0, binCount - 1);
      };
      auto strokeBinCenter = [&](int bin, int binCount) -> float {
        return (float(bin) + 0.5f) / float(binCount);
      };
      float glZoomInT = clamp((rackZoom - 1.f) / 2.4f, 0.f, 1.f);
      float glZoomInEase = std::pow(glZoomInT, 0.72f);
      float glDeepZoomT = clamp((rackZoom - 2.f) / 1.4f, 0.f, 1.f);
      float glDeepZoomEase = std::pow(glDeepZoomT, 0.82f);
      float glZoomInWidthComp = 1.f + 0.16f * glZoomInEase + 0.05f * glDeepZoomEase;
      float glZoomInHaloWidthComp = 1.f + 0.22f * glZoomInEase + 0.06f * glDeepZoomEase;
      float glZoomInAlphaComp = 1.f + 0.14f * glZoomInEase + 0.18f * glDeepZoomEase;
      float glZoomInLiftComp = 1.f + 0.30f * glZoomInEase + 0.36f * glDeepZoomEase;
      float glZoomInHaloAlphaComp = 1.f + 0.58f * glZoomInEase + 0.76f * glDeepZoomEase;
      float glDeepZoomEnergyFill = glDeepZoomEase;
      auto uploadFieldLaneTextures = [&]() -> bool {
        if (!initFieldShaderPipeline()) {
          return false;
        }
        if (fieldRowData.size() != rowCountU * 4u) {
          fieldRowData.assign(rowCountU * 4u, 0.f);
        }
        for (int iy = 0; iy < rowCount; ++iy) {
          size_t idx = size_t(iy);
          size_t base = idx * 4u;
          if (!isValid(idx)) {
            fieldRowData[base + 0u] = laneCenterXForConnectors;
            fieldRowData[base + 1u] = laneCenterXForConnectors;
            fieldRowData[base + 2u] = -1.f;
            fieldRowData[base + 3u] = 0.f;
            continue;
          }
          fieldRowData[base + 0u] = getX0(idx);
          fieldRowData[base + 1u] = getX1(idx);
          fieldRowData[base + 2u] = clamp(getVisualIntensity(idx), 0.f, 1.f);
          fieldRowData[base + 3u] = clamp(colorDrive[idx], 0.f, 1.f);
        }

        int scheme = clamp(module->scopeColorScheme, 0, TDScope::COLOR_SCHEME_COUNT - 1);
        ensureColorLut(scheme);
        if (fieldRowTextureWidth != rowCount) {
          glBindTexture(GL_TEXTURE_2D, fieldRowTexture);
          glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGBA32F, rowCount, 1, 0, GL_RGBA, GL_FLOAT, fieldRowData.data());
          fieldRowTextureWidth = rowCount;
        } else {
          glBindTexture(GL_TEXTURE_2D, fieldRowTexture);
          glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, rowCount, 1, GL_RGBA, GL_FLOAT, fieldRowData.data());
        }
        if (fieldColorLutScheme != scheme) {
          std::array<GLubyte, 256 * 4> lutBytes {};
          for (int i = 0; i < 256; ++i) {
            const NVGcolor &c = colorLut[size_t(scheme)][size_t(i)];
            lutBytes[size_t(i) * 4u + 0u] = encodeColorByte(c.r);
            lutBytes[size_t(i) * 4u + 1u] = encodeColorByte(c.g);
            lutBytes[size_t(i) * 4u + 2u] = encodeColorByte(c.b);
            lutBytes[size_t(i) * 4u + 3u] = 255u;
          }
          glBindTexture(GL_TEXTURE_2D, fieldColorLutTexture);
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, lutBytes.data());
          fieldColorLutScheme = scheme;
        }

        return true;
      };
      auto drawFieldLanePass = [&](float renderMain, float renderHalo, float renderContinuity, GLenum srcBlend,
                                   GLenum dstBlend) -> bool {
        if (!uploadFieldLaneTextures()) {
          return false;
        }
        const GlFieldVertex quad[6] = {
          {0.f, 0.f},
          {box.size.x, 0.f},
          {box.size.x, box.size.y},
          {0.f, 0.f},
          {box.size.x, box.size.y},
          {0.f, box.size.y},
        };
        static const GLuint kAttrPos = 0;
        glUseProgram(fieldShaderProgram);
        glUniform1i(fieldUniformRowTex, 0);
        glUniform1i(fieldUniformColorLutTex, 1);
        glUniform1f(fieldUniformRowCount, float(rowCount));
        glUniform1f(fieldUniformDrawTop, drawTop);
        glUniform1f(fieldUniformRowStep, rowStep);
        glUniform1f(fieldUniformZoomThickness, zoomThicknessMul);
        glUniform1f(fieldUniformZoomInWidthComp, glZoomInWidthComp);
        glUniform1f(fieldUniformZoomInHaloWidthComp, glZoomInHaloWidthComp);
        glUniform1f(fieldUniformZoomInAlphaComp, glZoomInAlphaComp);
        glUniform1f(fieldUniformZoomInLiftComp, glZoomInLiftComp);
        glUniform1f(fieldUniformZoomInHaloAlphaComp, glZoomInHaloAlphaComp);
        glUniform1f(fieldUniformDeepZoomEnergyFill, glDeepZoomEnergyFill);
        glUniform1f(fieldUniformRenderMain, renderMain);
        glUniform1f(fieldUniformRenderHalo, renderHalo);
        glUniform1f(fieldUniformRenderContinuity, renderContinuity);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fieldRowTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, fieldColorLutTexture);
        glBindBuffer(GL_ARRAY_BUFFER, fieldShaderVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STREAM_DRAW);
        glEnableVertexAttribArray(kAttrPos);
        glVertexAttribPointer(
          kAttrPos, 2, GL_FLOAT, GL_FALSE, sizeof(GlFieldVertex), reinterpret_cast<const GLvoid *>(offsetof(GlFieldVertex, x)));
        glBlendFunc(srcBlend, dstBlend);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisableVertexAttribArray(kAttrPos);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0);
        return true;
      };
      const bool renderHaloField = module->scopeTransientHaloEnabled;
      const bool renderMainField = module->debugRenderMainTraceEnabled;
      const bool renderContinuityField = module->debugRenderConnectorsEnabled;
      if (renderHaloField || renderMainField || renderContinuityField) {
        bool fieldDrawOk = true;
        if (renderHaloField) {
          fieldDrawOk = drawFieldLanePass(0.f, 1.f, 0.f, GL_ONE, GL_ONE) && fieldDrawOk;
        }
        if (fieldDrawOk && (renderMainField || renderContinuityField)) {
          fieldDrawOk = drawFieldLanePass(renderMainField ? 1.f : 0.f, 0.f, renderContinuityField ? 1.f : 0.f,
                                         GL_ONE, GL_ONE_MINUS_SRC_ALPHA) &&
                        fieldDrawOk;
        }
        if (fieldDrawOk) {
          fallbackRendererActive = false;
          return;
        }
      }
      fallbackRendererActive = true;
      auto appendSegmentQuad = [&](std::vector<GlSegmentQuadVertex> *verts, float ax, float ay, float bx, float by,
                                   float radius, GLubyte r, GLubyte g, GLubyte b, GLubyte a) {
        if (!verts) {
          return;
        }
        float pad = std::max(radius * 3.0f, 0.75f);
        float xMin = std::min(ax, bx) - pad;
        float xMax = std::max(ax, bx) + pad;
        float yMin = std::min(ay, by) - pad;
        float yMax = std::max(ay, by) + pad;
        auto push = [&](float x, float y) {
          verts->push_back({x, y, r, g, b, a, ax, ay, bx, by, radius});
        };
        push(xMin, yMin);
        push(xMax, yMin);
        push(xMax, yMax);
        push(xMin, yMin);
        push(xMax, yMax);
        push(xMin, yMax);
      };
      auto drawSegmentBatch = [&](std::vector<GlSegmentQuadVertex> &verts) {
        if (verts.empty() || !initSegmentShaderPipeline()) {
          return false;
        }
        static const GLuint kAttrPos = 0;
        static const GLuint kAttrColor = 1;
        static const GLuint kAttrSegment = 2;
        static const GLuint kAttrRadius = 3;
        glUseProgram(segmentShaderProgram);
        glBindBuffer(GL_ARRAY_BUFFER, segmentShaderVbo);
        glBufferData(
          GL_ARRAY_BUFFER, GLsizeiptr(verts.size() * sizeof(GlSegmentQuadVertex)), verts.data(), GL_STREAM_DRAW);
        glEnableVertexAttribArray(kAttrPos);
        glEnableVertexAttribArray(kAttrColor);
        glEnableVertexAttribArray(kAttrSegment);
        glEnableVertexAttribArray(kAttrRadius);
        glVertexAttribPointer(kAttrPos, 2, GL_FLOAT, GL_FALSE, sizeof(GlSegmentQuadVertex),
                              reinterpret_cast<const GLvoid *>(offsetof(GlSegmentQuadVertex, x)));
        glVertexAttribPointer(kAttrColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(GlSegmentQuadVertex),
                              reinterpret_cast<const GLvoid *>(offsetof(GlSegmentQuadVertex, r)));
        glVertexAttribPointer(kAttrSegment, 4, GL_FLOAT, GL_FALSE, sizeof(GlSegmentQuadVertex),
                              reinterpret_cast<const GLvoid *>(offsetof(GlSegmentQuadVertex, ax)));
        glVertexAttribPointer(kAttrRadius, 1, GL_FLOAT, GL_FALSE, sizeof(GlSegmentQuadVertex),
                              reinterpret_cast<const GLvoid *>(offsetof(GlSegmentQuadVertex, radius)));
        glDrawArrays(GL_TRIANGLES, 0, GLsizei(verts.size()));
        glDisableVertexAttribArray(kAttrRadius);
        glDisableVertexAttribArray(kAttrSegment);
        glDisableVertexAttribArray(kAttrColor);
        glDisableVertexAttribArray(kAttrPos);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glUseProgram(0);
        return true;
      };
      if (initSegmentShaderPipeline()) {
        haloSegmentVerts.clear();
        bodySegmentVerts.clear();
        fillSegmentVerts.clear();
        continuitySegmentVerts.clear();
        haloSegmentVerts.reserve(size_t(rowCount) * 6u);
        bodySegmentVerts.reserve(size_t(rowCount) * 6u);
        fillSegmentVerts.reserve(size_t(rowCount / 2 + 8) * 6u);
        continuitySegmentVerts.reserve(size_t(rowCount / 2 + 8) * 6u);

        bool prevValid = false;
        float prevX0 = laneCenterXForConnectors;
        float prevX1 = laneCenterXForConnectors;
        float prevY = drawTop + 0.5f;
        float prevVisual = 0.f;
        float prevColorDrive = 0.f;
        const float connectorMinDeltaPx = std::max(0.60f * zoomThicknessMul, 0.40f);
        for (int iy = 0; iy < rowCount; ++iy) {
          size_t idx = size_t(iy);
          if (!isValid(idx)) {
            prevValid = false;
            continue;
          }
          float x0 = getX0(idx);
          float x1 = getX1(idx);
          float y = rowY[idx];
          float visual = clamp(getVisualIntensity(idx), 0.f, 1.f);
          float transientLift = clamp(colorDrive[idx], 0.f, 1.f);
          float tone = clamp(0.78f * visual + 0.22f * transientLift, 0.f, 1.f);
          NVGcolor mainColor = brightenColor(
            gradientColorForIntensity(
              tone,
              uint8_t(std::lround(clamp((122.f + 120.f * tone) * kGlMainAlphaGain * glZoomInAlphaComp, 0.f, 255.f)))),
            clamp(transientLift * 0.90f * kGlMainLiftGain * glZoomInLiftComp, 0.f, 1.f));
          GLubyte mainR = encodeColorByte(mainColor.r);
          GLubyte mainG = encodeColorByte(mainColor.g);
          GLubyte mainB = encodeColorByte(mainColor.b);
          GLubyte mainA = encodeColorByte(mainColor.a);
          float mainW = (0.78f + 0.62f * tone) * zoomThicknessMul * kGlMainWidthGain * glZoomInWidthComp;
          float mainRadius = std::max(mainW * 0.55f, 0.40f);
          appendSegmentQuad(&bodySegmentVerts, x0, y, x1, y, mainRadius, mainR, mainG, mainB, mainA);

          if (glDeepZoomEnergyFill > 1e-4f) {
            GLubyte fillAlpha =
              GLubyte(std::lround(clamp(255.f * kGlDeepZoomEnergyFillAlpha * glDeepZoomEnergyFill, 0.f, 255.f)));
            appendSegmentQuad(&fillSegmentVerts, x0, y, x1, y, mainRadius * (1.08f + 0.04f * glDeepZoomEnergyFill),
                              mainR, mainG, mainB, fillAlpha);
          }

          if (module->scopeTransientHaloEnabled) {
            float haloLinear = clamp((transientLift - 0.030f) / 0.800f, 0.f, 1.f);
            float haloT = haloLinear * haloLinear;
            uint8_t haloAlpha = uint8_t(std::lround((88.f + 196.f * std::max(visual, 0.24f)) * haloT));
            if (haloAlpha >= kHaloMinAlphaToDraw) {
              float haloW =
                (mainW + (1.10f + 2.20f * haloT) * zoomThicknessMul) * kGlHaloWidthGain * glZoomInHaloWidthComp;
              float haloRadius = std::max(haloW * 0.58f, mainRadius + 0.30f);
              uint8_t boostedHaloAlpha = uint8_t(std::lround(
                clamp(float(haloAlpha) * kGlHaloAlphaGain * glZoomInHaloAlphaComp, 0.f, 255.f)));
              appendSegmentQuad(&haloSegmentVerts, x0, y, x1, y, haloRadius, 255, 255, 255, boostedHaloAlpha);
            }
          }

          if (module->debugRenderConnectorsEnabled && prevValid) {
            if (!(std::fabs(x0 - prevX0) < connectorMinDeltaPx && std::fabs(x1 - prevX1) < connectorMinDeltaPx)) {
              float connectVisual = clamp(0.5f * (prevVisual + visual), 0.f, 1.f);
              float connectTransientLift = clamp(0.5f * (prevColorDrive + transientLift), 0.f, 1.f);
              float connectTone = clamp(0.82f * connectVisual + 0.18f * connectTransientLift, 0.f, 1.f);
              NVGcolor c = brightenColor(
                gradientColorForIntensity(
                  connectVisual,
                  uint8_t(std::lround(clamp(
                    (104.f + 108.f * connectVisual) * kGlConnectorAlphaGain * glZoomInAlphaComp, 0.f, 255.f)))),
                clamp(connectTransientLift * 0.84f * kGlConnectorLiftGain * glZoomInLiftComp, 0.f, 1.f));
              GLubyte r = encodeColorByte(c.r);
              GLubyte g = encodeColorByte(c.g);
              GLubyte b = encodeColorByte(c.b);
              GLubyte a = encodeColorByte(c.a);
              float prevCenter = 0.5f * (prevX0 + prevX1);
              float center = 0.5f * (x0 + x1);
              float centerSpan = 0.5f * (std::fabs(prevX1 - prevX0) + std::fabs(x1 - x0));
              if (centerSpan > connectorMinDeltaPx * 1.2f) {
                GLubyte bodyAlpha = GLubyte(std::lround(clamp(
                  float(a) * (0.52f + 0.12f * glDeepZoomEnergyFill), 0.f, 255.f)));
                float continuityRadius =
                  std::max((0.92f + 0.52f * connectTone) * zoomThicknessMul * kGlConnectorWidthGain *
                             (1.04f + 0.10f * glZoomInWidthComp) * (1.02f + 0.10f * glDeepZoomEnergyFill) * 0.58f,
                           0.45f);
                appendSegmentQuad(&continuitySegmentVerts, prevCenter, prevY, center, y, continuityRadius, r, g, b,
                                  bodyAlpha);
              }
            }
          }

          prevX0 = x0;
          prevX1 = x1;
          prevY = y;
          prevVisual = visual;
          prevColorDrive = transientLift;
          prevValid = true;
        }

        if (!haloSegmentVerts.empty()) {
          glBlendFunc(GL_SRC_ALPHA, GL_ONE);
          drawSegmentBatch(haloSegmentVerts);
          glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        if (module->debugRenderMainTraceEnabled) {
          drawSegmentBatch(bodySegmentVerts);
          if (!fillSegmentVerts.empty()) {
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            drawSegmentBatch(fillSegmentVerts);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
          }
        }
        if (!continuitySegmentVerts.empty()) {
          drawSegmentBatch(continuitySegmentVerts);
        }
        return;
      }
      auto drawBatch = [&](std::vector<GlLineVertex> &verts, float width, const ShaderPassParams &shaderParams) {
        if (verts.empty()) {
          return;
        }
        glLineWidth(width);
        if (initShaderPipeline()) {
          glUseProgram(shaderProgram);
          glUniform1f(shaderUniformColorScale, shaderParams.colorScale);
          glUniform1f(shaderUniformColorLift, shaderParams.colorLift);
          glUniform1f(shaderUniformAlphaScale, shaderParams.alphaScale);
          glUniform1f(shaderUniformAlphaGamma, shaderParams.alphaGamma);
          glBindBuffer(GL_ARRAY_BUFFER, shaderVbo);
          glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(verts.size() * sizeof(GlLineVertex)), verts.data(), GL_STREAM_DRAW);
          glVertexPointer(2, GL_FLOAT, sizeof(GlLineVertex), reinterpret_cast<const GLvoid *>(offsetof(GlLineVertex, x)));
          glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(GlLineVertex), reinterpret_cast<const GLvoid *>(offsetof(GlLineVertex, r)));
        } else {
          glBindBuffer(GL_ARRAY_BUFFER, 0);
          glUseProgram(0);
          glVertexPointer(2, GL_FLOAT, sizeof(GlLineVertex), &verts[0].x);
          glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(GlLineVertex), &verts[0].r);
        }
        glDrawArrays(GL_LINES, 0, GLsizei(verts.size()));
        if (shaderReady) {
          glBindBuffer(GL_ARRAY_BUFFER, 0);
          glUseProgram(0);
        }
      };
      if (module->scopeTransientHaloEnabled) {
        const ShaderPassParams haloShaderParams = makeShaderPassParams(1.08f, 0.14f, 1.16f, 0.92f);
        for (auto &verts : haloBatchVerts) {
          verts.clear();
        }
        for (int iy = 0; iy < rowCount; ++iy) {
          size_t idx = size_t(iy);
          if (!isValid(idx)) {
            continue;
          }
          float visual = clamp(getVisualIntensity(idx), 0.f, 1.f);
          float transientLift = clamp(colorDrive[idx], 0.f, 1.f);
          float haloLinear = clamp((transientLift - 0.030f) / 0.800f, 0.f, 1.f);
          float haloT = haloLinear * haloLinear;
          uint8_t haloAlpha = uint8_t(std::lround((88.f + 196.f * std::max(visual, 0.24f)) * haloT));
          if (haloAlpha < kHaloMinAlphaToDraw) {
            continue;
          }
          int rowBin = quantizeStrokeBin(haloT, kGlHaloStrokeBins);
          float haloExtend = (1.35f + 5.20f * haloT) * zoomThicknessMul;
          uint8_t boostedHaloAlpha = uint8_t(std::lround(
            clamp(float(haloAlpha) * kGlHaloAlphaGain * glZoomInHaloAlphaComp, 0.f, 255.f)));
          haloBatchVerts[size_t(rowBin)].push_back({getX0(idx) - haloExtend, rowY[idx], 255, 255, 255, boostedHaloAlpha});
          haloBatchVerts[size_t(rowBin)].push_back({getX1(idx) + haloExtend, rowY[idx], 255, 255, 255, boostedHaloAlpha});
        }
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        for (int widthBin = 0; widthBin < kGlHaloStrokeBins; ++widthBin) {
          float haloCenter = strokeBinCenter(widthBin, kGlHaloStrokeBins);
          float visualCenter = haloCenter;
          float mainW = (0.78f + 0.62f * visualCenter) * zoomThicknessMul;
          float haloW =
            (mainW + (1.10f + 2.20f * haloCenter) * zoomThicknessMul) * kGlHaloWidthGain * glZoomInHaloWidthComp;
          drawBatch(haloBatchVerts[size_t(widthBin)], haloW, haloShaderParams);
        }
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      }
      if (module->debugRenderMainTraceEnabled) {
        const ShaderPassParams mainShaderParams = makeShaderPassParams(1.04f, 0.04f, 1.06f, 0.97f);
        const ShaderPassParams fillShaderParams = makeShaderPassParams(1.03f, 0.03f, 0.96f, 1.00f);
        for (auto &verts : mainBatchVerts) {
          verts.clear();
        }
        for (int iy = 0; iy < rowCount; ++iy) {
          size_t idx = size_t(iy);
          if (!isValid(idx)) {
            continue;
          }
          float visual = clamp(getVisualIntensity(idx), 0.f, 1.f);
          float transientLift = clamp(colorDrive[idx], 0.f, 1.f);
          float tone = clamp(0.78f * visual + 0.22f * transientLift, 0.f, 1.f);
          int rowBin = quantizeStrokeBin(tone, kGlMainStrokeBins);
          NVGcolor c = brightenColor(
            gradientColorForIntensity(
              tone,
              uint8_t(std::lround(clamp((122.f + 120.f * tone) * kGlMainAlphaGain * glZoomInAlphaComp, 0.f, 255.f)))),
            clamp(transientLift * 0.90f * kGlMainLiftGain * glZoomInLiftComp, 0.f, 1.f));
          GLubyte r = encodeColorByte(c.r);
          GLubyte g = encodeColorByte(c.g);
          GLubyte b = encodeColorByte(c.b);
          GLubyte a = encodeColorByte(c.a);
          mainBatchVerts[size_t(rowBin)].push_back({getX0(idx), rowY[idx], r, g, b, a});
          mainBatchVerts[size_t(rowBin)].push_back({getX1(idx), rowY[idx], r, g, b, a});
        }
        for (int widthBin = 0; widthBin < kGlMainStrokeBins; ++widthBin) {
          float toneCenter = strokeBinCenter(widthBin, kGlMainStrokeBins);
          float mainW =
            (0.78f + 0.62f * toneCenter) * zoomThicknessMul * kGlMainWidthGain * glZoomInWidthComp;
          drawBatch(mainBatchVerts[size_t(widthBin)], mainW, mainShaderParams);
        }
        if (glDeepZoomEnergyFill > 1e-4f) {
          glBlendFunc(GL_SRC_ALPHA, GL_ONE);
          for (int widthBin = 0; widthBin < kGlMainStrokeBins; ++widthBin) {
            std::vector<GlLineVertex> &verts = mainBatchVerts[size_t(widthBin)];
            if (verts.empty()) {
              continue;
            }
            float toneCenter = strokeBinCenter(widthBin, kGlMainStrokeBins);
            float fillW = (0.78f + 0.62f * toneCenter) * zoomThicknessMul * (1.01f + 0.04f * glDeepZoomEnergyFill);
            GLubyte fillAlpha =
              GLubyte(std::lround(clamp(255.f * kGlDeepZoomEnergyFillAlpha * glDeepZoomEnergyFill, 0.f, 255.f)));
            if (fillAlpha == 0u) {
              continue;
            }
            std::vector<GlLineVertex> fillVerts = verts;
            for (GlLineVertex &v : fillVerts) {
              v.a = fillAlpha;
            }
            drawBatch(fillVerts, fillW, fillShaderParams);
          }
          glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
      }
      if (module->debugRenderConnectorsEnabled) {
        const ShaderPassParams connectorBodyShaderParams = makeShaderPassParams(1.02f, 0.03f, 0.86f, 1.02f);
        const float connectorMinDeltaPx = std::max(0.60f * zoomThicknessMul, 0.40f);
        std::array<std::vector<GlLineVertex>, kGlConnectorStrokeBins> connectorBodyBatchVerts;
        for (auto &verts : connectorBodyBatchVerts) {
          verts.reserve(size_t(rowCount / 2 + 8) * 2u);
        }
        bool prevValid = false;
        float prevX0 = laneCenterXForConnectors;
        float prevX1 = laneCenterXForConnectors;
        float prevY = drawTop + 0.5f;
        float prevVisual = 0.f;
        float prevColorDrive = 0.f;
        for (int iy = 0; iy < rowCount; ++iy) {
          size_t idx = size_t(iy);
          if (!isValid(idx)) {
            prevValid = false;
            continue;
          }
          float x0 = getX0(idx);
          float x1 = getX1(idx);
          float visual = clamp(getVisualIntensity(idx), 0.f, 1.f);
          float transientLift = clamp(colorDrive[idx], 0.f, 1.f);
          if (prevValid) {
            if (std::fabs(x0 - prevX0) < connectorMinDeltaPx && std::fabs(x1 - prevX1) < connectorMinDeltaPx) {
              prevX0 = x0;
              prevX1 = x1;
              prevY = rowY[idx];
              prevVisual = visual;
              prevColorDrive = transientLift;
              continue;
            }
            float connectVisual = clamp(0.5f * (prevVisual + visual), 0.f, 1.f);
            float connectTransientLift = clamp(0.5f * (prevColorDrive + transientLift), 0.f, 1.f);
            float tone = clamp(0.82f * connectVisual + 0.18f * connectTransientLift, 0.f, 1.f);
            int rowBin = quantizeStrokeBin(tone, kGlConnectorStrokeBins);
            NVGcolor c = brightenColor(
              gradientColorForIntensity(
                connectVisual,
                uint8_t(std::lround(clamp(
                  (104.f + 108.f * connectVisual) * kGlConnectorAlphaGain * glZoomInAlphaComp, 0.f, 255.f)))),
              clamp(connectTransientLift * 0.84f * kGlConnectorLiftGain * glZoomInLiftComp, 0.f, 1.f));
            GLubyte r = encodeColorByte(c.r);
            GLubyte g = encodeColorByte(c.g);
            GLubyte b = encodeColorByte(c.b);
            GLubyte a = encodeColorByte(c.a);
            float prevCenter = 0.5f * (prevX0 + prevX1);
            float center = 0.5f * (x0 + x1);
            float centerSpan = 0.5f * (std::fabs(prevX1 - prevX0) + std::fabs(x1 - x0));
            if (centerSpan > connectorMinDeltaPx * 1.2f) {
              GLubyte bodyAlpha = GLubyte(std::lround(clamp(
                float(a) * (0.52f + 0.12f * glDeepZoomEnergyFill), 0.f, 255.f)));
              connectorBodyBatchVerts[size_t(rowBin)].push_back({prevCenter, prevY, r, g, b, bodyAlpha});
              connectorBodyBatchVerts[size_t(rowBin)].push_back({center, rowY[idx], r, g, b, bodyAlpha});
            }
          }
          prevX0 = x0;
          prevX1 = x1;
          prevY = rowY[idx];
          prevVisual = visual;
          prevColorDrive = transientLift;
          prevValid = true;
        }
        for (int widthBin = 0; widthBin < kGlConnectorStrokeBins; ++widthBin) {
          if (!connectorBodyBatchVerts[size_t(widthBin)].empty()) {
            float toneCenter = strokeBinCenter(widthBin, kGlConnectorStrokeBins);
            float bodyW =
              (0.92f + 0.52f * toneCenter) * zoomThicknessMul * kGlConnectorWidthGain * (1.04f + 0.10f * glZoomInWidthComp);
            bodyW *= (1.02f + 0.10f * glDeepZoomEnergyFill);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            drawBatch(connectorBodyBatchVerts[size_t(widthBin)], bodyW, connectorBodyShaderParams);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
          }
        }
      }
    };

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, box.size.x, box.size.y, 0.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    auto historyVisibleSlot = [&](size_t visibleIdx) {
      int slot = historyHeadRow + historyMarginRows + int(visibleIdx);
      slot %= std::max(historyCapacityRows, 1);
      if (slot < 0) {
        slot += std::max(historyCapacityRows, 1);
      }
      return size_t(slot);
    };
    const bool drawFromHistory = useGeometryHistoryCache && historyValidState && historyCapacityRows > 0;
    if (drawFromHistory) {
      drawLane(
        [&](size_t idx) { return historyX0[historyVisibleSlot(idx)]; },
        [&](size_t idx) { return historyX1[historyVisibleSlot(idx)]; },
        [&](size_t idx) { return historyVisualIntensity[historyVisibleSlot(idx)]; }, rowColorDrive,
        [&](size_t idx) { return historyValid[historyVisibleSlot(idx)] != 0u; }, lane0CenterX);
      if (renderStereo && module->debugRenderStereoRightLaneEnabled) {
        drawLane(
          [&](size_t idx) { return historyX0Right[historyVisibleSlot(idx)]; },
          [&](size_t idx) { return historyX1Right[historyVisibleSlot(idx)]; },
          [&](size_t idx) { return historyVisualIntensityRight[historyVisibleSlot(idx)]; }, rowColorDriveRight,
          [&](size_t idx) { return historyValidRight[historyVisibleSlot(idx)] != 0u; }, lane1CenterX);
      }
    } else {
      drawLane(
        [&](size_t idx) { return rowX0[idx]; }, [&](size_t idx) { return rowX1[idx]; },
        [&](size_t idx) { return rowVisualIntensity[idx]; }, rowColorDrive,
        [&](size_t idx) { return rowValid[idx] != 0u; }, lane0CenterX);
      if (renderStereo && module->debugRenderStereoRightLaneEnabled) {
        drawLane(
          [&](size_t idx) { return rowX0Right[idx]; }, [&](size_t idx) { return rowX1Right[idx]; },
          [&](size_t idx) { return rowVisualIntensityRight[idx]; }, rowColorDriveRight,
          [&](size_t idx) { return rowValidRight[idx] != 0u; }, lane1CenterX);
      }
    }
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisable(GL_LINE_SMOOTH);
    if (fallbackRendererActive) {
      glDisable(GL_BLEND);
      glMatrixMode(GL_PROJECTION);
      glPushMatrix();
      glLoadIdentity();
      glOrtho(0.0, box.size.x, box.size.y, 0.0, -1.0, 1.0);
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      glLoadIdentity();
      glColor4f(0.92f, 0.12f, 0.12f, 1.0f);
      glBegin(GL_QUADS);
      glVertex2f(box.size.x - 11.0f, 5.0f);
      glVertex2f(box.size.x - 5.0f, 5.0f);
      glVertex2f(box.size.x - 5.0f, 11.0f);
      glVertex2f(box.size.x - 11.0f, 11.0f);
      glEnd();
      glColor4f(1.f, 1.f, 1.f, 1.f);
      glPopMatrix();
      glMatrixMode(GL_PROJECTION);
      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);
    }

    publishUiDebugMetrics(densityPct, rowCount);
  }
};

namespace tdscope {

Widget *createGlDisplay(TDScope *module, math::Rect scopeRectMm) {
  auto *glDisplay = new TDScopeGlWidget();
  glDisplay->module = module;
  glDisplay->box.pos = mm2px(scopeRectMm.pos);
  glDisplay->box.size = mm2px(scopeRectMm.size);
  glDisplay->box.pos.y += 1.f;
  glDisplay->box.size.y = std::max(1.f, glDisplay->box.size.y - 2.f);
  glDisplay->dirtyOnSubpixelChange = false;
  return glDisplay;
}

} // namespace tdscope
