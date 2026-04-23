#include "TemporalDeckExpanderProtocol.hpp"
#include "TemporalDeckTest.hpp"
#include "DebugTerminalTransport.hpp"
#include "PanelSvgUtils.hpp"
#include "plugin.hpp"
#include <nanovg_gl.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <functional>
#include <limits>
#include <vector>

namespace {

static constexpr float kScopeDisplayVerticalSupersampleMax = 2.f;
static constexpr double kDebugTerminalSubmitIntervalSec = 1.0 / 8.0;
static std::atomic<uint32_t> gTDScopeDebugInstanceCounter {1u};

static float computeScopeDisplayVerticalSupersample(float rackZoom) {
  rackZoom = std::max(rackZoom, 1e-4f);
  (void) rackZoom;
  return 0.55f;
  // Zoomed out: reduce total row density to save draw calls when several
  // scopes are visible. Zoomed in: restore the full supersampled trace/halo.
  float zoomOutT = clamp((1.0f - rackZoom) / 0.35f, 0.f, 1.f);
  float zoomInT = clamp((rackZoom - 1.0f) / 1.0f, 0.f, 1.f);
  float supersample = 1.10f + (1.35f - 1.10f) * (1.f - zoomOutT);
  supersample += (kScopeDisplayVerticalSupersampleMax - 1.35f) * zoomInT;
  return clamp(supersample, 1.10f, kScopeDisplayVerticalSupersampleMax);
}

static PanelBorder *findPanelBorder(Widget *widget) {
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

static bool isTemporalDeckModule(const engine::Module *neighbor) {
  if (!neighbor || !neighbor->model) {
    return false;
  }
  return (neighbor->model == modelTemporalDeck) || (neighbor->model->slug == "TemporalDeck");
}

} // namespace

struct TDScope final : Module {
  enum LightId { LINK_LIGHT, PREVIEW_LIGHT, LIGHTS_LEN };
  enum ScopeRangeMode { SCOPE_RANGE_5V = 0, SCOPE_RANGE_10V, SCOPE_RANGE_2V5, SCOPE_RANGE_AUTO, SCOPE_RANGE_COUNT };
  enum ScopeChannelMode { SCOPE_CHANNEL_MONO = 0, SCOPE_CHANNEL_STEREO, SCOPE_CHANNEL_COUNT };
  enum DebugUiPublishRateMode {
    DEBUG_UI_PUBLISH_120HZ = 0,
    DEBUG_UI_PUBLISH_60HZ,
    DEBUG_UI_PUBLISH_30HZ,
    DEBUG_UI_PUBLISH_COUNT
  };
  enum DebugRenderMode {
    DEBUG_RENDER_STANDARD = 0,
    DEBUG_RENDER_TAIL_RASTER,
    DEBUG_RENDER_OPENGL,
    DEBUG_RENDER_COUNT
  };
  enum ColorScheme {
    COLOR_SCHEME_TEMPORAL_DECK = 0,
    COLOR_SCHEME_LEVIATHAN,
    COLOR_SCHEME_PICKLE,
    COLOR_SCHEME_HELLFIRE,
    COLOR_SCHEME_ANGELIC,
    COLOR_SCHEME_VIOLET_FLAME,
    COLOR_SCHEME_PIXIE,
    COLOR_SCHEME_WASP,
    COLOR_SCHEME_EMERALD,
    COLOR_SCHEME_COUNT
  };

  std::array<temporaldeck_expander::HostToDisplay, 2> leftMessages;
  std::array<temporaldeck_expander::HostToDisplay, 2> uiSnapshots;
  std::atomic<uint32_t> uiSnapshotFrontIndex {0};
  std::atomic<uint64_t> uiSnapshotPublishGen {0};
  std::atomic<bool> uiLinkActive {false};
  std::atomic<bool> uiPreviewValid {false};
  std::atomic<uint64_t> uiLastPublishSeq {0};
  std::atomic<uint64_t> uiSnapshotReadMissCount {0};
  std::atomic<uint64_t> uiDebugScopeDrawSeq {0};
  std::atomic<uint64_t> uiDebugScopeDrawCalls {0};
  std::atomic<float> uiDebugScopeRackZoom {1.f};
  std::atomic<float> uiDebugScopeZoomThicknessMul {1.f};
  std::atomic<float> uiDebugScopeUiDrawUs {0.f};
  std::atomic<float> uiDebugScopeUiDrawUsEma {0.f};
  std::atomic<float> uiDebugScopeDensityPct {100.f};
  std::atomic<int> uiDebugScopeDensityRows {0};
  uint32_t debugInstanceId = 0u;
  double uiDebugTerminalLastSubmitSec = -1.0;
  float uiPublishTimerSec = 0.f;
  float invalidMessageTimerSec = 1e9f;
  float invalidPreviewTimerSec = 1e9f;
  uint64_t lastPublishSeq = 0;
  int staleFrames = 0;
  bool previewValid = false;
  int scopeDisplayRangeMode = SCOPE_RANGE_5V;
  int scopeChannelMode = SCOPE_CHANNEL_MONO;
  int scopeColorScheme = COLOR_SCHEME_TEMPORAL_DECK;
  bool scopeTransientHaloEnabled = true;
  bool debugRenderMainTraceEnabled = true;
  bool debugRenderHaloEnabled = true;
  bool debugRenderConnectorsEnabled = true;
  bool debugRenderStereoRightLaneEnabled = true;
  bool debugFramebufferCacheEnabled = true;
  int debugRenderMode = DEBUG_RENDER_STANDARD;
  int debugUiPublishRateMode = DEBUG_UI_PUBLISH_120HZ;
  float requestPublishTimerSec = 0.f;
  uint64_t requestSeq = 0u;
  uint32_t lastRequestedScopeFormat = uint32_t(-1);
  bool lastLagDragActive = false;
  float lastLagDragSamples = 0.f;
  float lastLagDragVelocity = 0.f;
  std::atomic<bool> uiLagDragActive {false};
  std::atomic<float> uiLagDragSamples {0.f};
  std::atomic<float> uiLagDragVelocity {0.f};

  // Match faster host updates to improve perceived sync against speakers.
  static constexpr float kUiPublishIntervalSec = 1.f / 120.f;
  static constexpr float kRequestPublishIntervalSec = 1.f / 30.f;
  static constexpr float kRequestPublishIntervalDragSec = 1.f / 120.f;
  static constexpr float kLinkDropGraceSec = 1.f / 45.f;
  static constexpr float kPreviewDropGraceSec = 1.f / 45.f;

  TDScope() {
    config(0, 0, 0, LIGHTS_LEN);
    debugInstanceId = gTDScopeDebugInstanceCounter.fetch_add(1u, std::memory_order_relaxed);
    leftExpander.producerMessage = &leftMessages[0];
    leftExpander.consumerMessage = &leftMessages[1];
    uiSnapshots[0] = temporaldeck_expander::HostToDisplay();
    uiSnapshots[1] = temporaldeck_expander::HostToDisplay();
  }

  float scopeDisplayFullScaleVolts() const {
    switch (scopeDisplayRangeMode) {
      case SCOPE_RANGE_10V:
        return 10.f;
      case SCOPE_RANGE_2V5:
        return 2.5f;
      case SCOPE_RANGE_AUTO:
        return 5.f;
      case SCOPE_RANGE_5V:
      default:
        return 5.f;
    }
  }

  float debugUiPublishIntervalSec() const {
    switch (debugUiPublishRateMode) {
      case DEBUG_UI_PUBLISH_30HZ:
        return 1.f / 30.f;
      case DEBUG_UI_PUBLISH_60HZ:
        return 1.f / 60.f;
      case DEBUG_UI_PUBLISH_120HZ:
      default:
        return kUiPublishIntervalSec;
    }
  }

  bool useTailRasterRenderMode() const {
    return debugRenderMode == DEBUG_RENDER_TAIL_RASTER;
  }

  bool useGeometryHistoryRenderMode() const {
    return debugRenderMode == DEBUG_RENDER_OPENGL;
  }

  bool useOpenGlGeometryRenderMode() const {
    return debugRenderMode == DEBUG_RENDER_OPENGL;
  }

  json_t *dataToJson() override {
    json_t *root = json_object();
    json_object_set_new(root, "scopeDisplayRangeMode", json_integer(scopeDisplayRangeMode));
    json_object_set_new(root, "scopeChannelMode", json_integer(scopeChannelMode));
    json_object_set_new(root, "scopeColorScheme", json_integer(scopeColorScheme));
    json_object_set_new(root, "scopeTransientHaloEnabled", json_boolean(scopeTransientHaloEnabled));
    json_object_set_new(root, "debugRenderMainTraceEnabled", json_boolean(debugRenderMainTraceEnabled));
    json_object_set_new(root, "debugRenderHaloEnabled", json_boolean(debugRenderHaloEnabled));
    json_object_set_new(root, "debugRenderConnectorsEnabled", json_boolean(debugRenderConnectorsEnabled));
    json_object_set_new(root, "debugRenderStereoRightLaneEnabled", json_boolean(debugRenderStereoRightLaneEnabled));
    json_object_set_new(root, "debugFramebufferCacheEnabled", json_boolean(debugFramebufferCacheEnabled));
    json_object_set_new(root, "debugRenderMode", json_integer(debugRenderMode));
    json_object_set_new(root, "debugUiPublishRateMode", json_integer(debugUiPublishRateMode));
    return root;
  }

  void dataFromJson(json_t *root) override {
    if (!root) {
      return;
    }
    json_t *rangeJ = json_object_get(root, "scopeDisplayRangeMode");
    if (rangeJ) {
      scopeDisplayRangeMode = clamp(int(json_integer_value(rangeJ)), SCOPE_RANGE_5V, SCOPE_RANGE_COUNT - 1);
    }
    json_t *channelJ = json_object_get(root, "scopeChannelMode");
    if (channelJ) {
      scopeChannelMode = clamp(int(json_integer_value(channelJ)), SCOPE_CHANNEL_MONO, SCOPE_CHANNEL_COUNT - 1);
    }
    json_t *schemeJ = json_object_get(root, "scopeColorScheme");
    if (schemeJ) {
      scopeColorScheme = clamp(int(json_integer_value(schemeJ)), COLOR_SCHEME_TEMPORAL_DECK, COLOR_SCHEME_COUNT - 1);
    }
    json_t *haloJ = json_object_get(root, "scopeTransientHaloEnabled");
    if (haloJ) {
      scopeTransientHaloEnabled = json_boolean_value(haloJ);
    }
    json_t *mainTraceJ = json_object_get(root, "debugRenderMainTraceEnabled");
    if (mainTraceJ) {
      debugRenderMainTraceEnabled = json_boolean_value(mainTraceJ);
    }
    json_t *renderHaloJ = json_object_get(root, "debugRenderHaloEnabled");
    if (renderHaloJ) {
      debugRenderHaloEnabled = json_boolean_value(renderHaloJ);
    }
    json_t *connectorsJ = json_object_get(root, "debugRenderConnectorsEnabled");
    if (connectorsJ) {
      debugRenderConnectorsEnabled = json_boolean_value(connectorsJ);
    }
    json_t *stereoRightLaneJ = json_object_get(root, "debugRenderStereoRightLaneEnabled");
    if (stereoRightLaneJ) {
      debugRenderStereoRightLaneEnabled = json_boolean_value(stereoRightLaneJ);
    }
    json_t *framebufferCacheJ = json_object_get(root, "debugFramebufferCacheEnabled");
    if (framebufferCacheJ) {
      debugFramebufferCacheEnabled = json_boolean_value(framebufferCacheJ);
    }
    bool loadedRenderMode = false;
    json_t *renderModeJ = json_object_get(root, "debugRenderMode");
    if (renderModeJ) {
      int rawRenderMode = int(json_integer_value(renderModeJ));
      switch (rawRenderMode) {
        case 0: debugRenderMode = DEBUG_RENDER_STANDARD; break;
        case 1: debugRenderMode = DEBUG_RENDER_TAIL_RASTER; break;
        case 2: debugRenderMode = DEBUG_RENDER_OPENGL; break;
        case 3: debugRenderMode = DEBUG_RENDER_STANDARD; break;
        case 4: debugRenderMode = DEBUG_RENDER_TAIL_RASTER; break;
        case 5: debugRenderMode = DEBUG_RENDER_OPENGL; break;
        case 6: debugRenderMode = DEBUG_RENDER_OPENGL; break;
        default:
          debugRenderMode = clamp(rawRenderMode, DEBUG_RENDER_STANDARD, DEBUG_RENDER_COUNT - 1);
          break;
      }
      loadedRenderMode = true;
    }
    if (!loadedRenderMode) {
      bool legacyTailRaster = false;
      bool legacyTailRasterGpuShift = false;
      bool legacyGeometryHistory = false;
      bool legacyGlGeometry = false;
      json_t *tailRasterCacheJ = json_object_get(root, "debugTailRasterCacheEnabled");
      if (tailRasterCacheJ) {
        legacyTailRaster = json_boolean_value(tailRasterCacheJ);
      }
      json_t *tailRasterGpuShiftJ = json_object_get(root, "debugTailRasterGpuShiftEnabled");
      if (tailRasterGpuShiftJ) {
        legacyTailRasterGpuShift = json_boolean_value(tailRasterGpuShiftJ);
      }
      json_t *geometryHistoryJ = json_object_get(root, "debugGeometryHistoryCacheEnabled");
      if (geometryHistoryJ) {
        legacyGeometryHistory = json_boolean_value(geometryHistoryJ);
      }
      json_t *glGeometryJ = json_object_get(root, "debugGlGeometryEnabled");
      if (glGeometryJ) {
        legacyGlGeometry = json_boolean_value(glGeometryJ);
      }
      if (legacyGlGeometry && legacyGeometryHistory) {
        debugRenderMode = DEBUG_RENDER_OPENGL;
      } else if (legacyGlGeometry) {
        debugRenderMode = DEBUG_RENDER_OPENGL;
      } else if (legacyGeometryHistory) {
        debugRenderMode = DEBUG_RENDER_STANDARD;
      } else if (legacyTailRasterGpuShift) {
        debugRenderMode = DEBUG_RENDER_TAIL_RASTER;
      } else if (legacyTailRaster) {
        debugRenderMode = DEBUG_RENDER_TAIL_RASTER;
      } else {
        debugRenderMode = DEBUG_RENDER_STANDARD;
      }
    }
    json_t *publishRateJ = json_object_get(root, "debugUiPublishRateMode");
    if (publishRateJ) {
      debugUiPublishRateMode =
        clamp(int(json_integer_value(publishRateJ)), DEBUG_UI_PUBLISH_120HZ, DEBUG_UI_PUBLISH_COUNT - 1);
    }
  }

  void publishSnapshotToUi(const temporaldeck_expander::HostToDisplay &msg) {
    uint32_t frontIndex = uiSnapshotFrontIndex.load(std::memory_order_relaxed) & 1u;
    uint32_t backIndex = frontIndex ^ 1u;
    uiSnapshots[backIndex] = msg;
    uiSnapshotFrontIndex.store(backIndex, std::memory_order_release);
    uiSnapshotPublishGen.fetch_add(1u, std::memory_order_release);
    uiLastPublishSeq.store(msg.publishSeq, std::memory_order_release);
  }

  bool readSnapshotForUi(temporaldeck_expander::HostToDisplay *out) const {
    if (!out) {
      return false;
    }
    for (int i = 0; i < 3; ++i) {
      uint64_t gen0 = uiSnapshotPublishGen.load(std::memory_order_acquire);
      uint32_t frontIndex0 = uiSnapshotFrontIndex.load(std::memory_order_acquire) & 1u;
      *out = uiSnapshots[frontIndex0];
      uint32_t frontIndex1 = uiSnapshotFrontIndex.load(std::memory_order_acquire) & 1u;
      uint64_t gen1 = uiSnapshotPublishGen.load(std::memory_order_acquire);
      if (frontIndex0 == frontIndex1 && gen0 == gen1) {
        return out->magic == temporaldeck_expander::MAGIC &&
               out->version == temporaldeck_expander::VERSION &&
               out->size == sizeof(temporaldeck_expander::HostToDisplay);
      }
    }
    return false;
  }

  void setLagDragRequest(bool active, float lagSamples, float velocity = 0.f) {
    // WARNING: Expander lag-drag sign contract.
    // `lagSamples` increases when moving toward older audio (away from NOW).
    // `velocity` must use scratch gesture convention:
    //   positive velocity => motion toward NOW (decreasing lag)
    //   negative velocity => motion away from NOW (increasing lag)
    // Breaking this convention can silently invert drag direction downstream.
    uiLagDragActive.store(active, std::memory_order_relaxed);
    uiLagDragSamples.store(std::max(0.f, lagSamples), std::memory_order_relaxed);
    uiLagDragVelocity.store(velocity, std::memory_order_relaxed);
  }

  void process(const ProcessArgs &args) override {
    bool validMessage = false;
    bool previewValidNow = false;
    const temporaldeck_expander::HostToDisplay *latestMsg = nullptr;
    if (isTemporalDeckModule(leftExpander.module) && leftExpander.consumerMessage) {
      const auto *msg = reinterpret_cast<const temporaldeck_expander::HostToDisplay *>(leftExpander.consumerMessage);
      if (msg->magic == temporaldeck_expander::MAGIC && msg->version == temporaldeck_expander::VERSION &&
          msg->size == sizeof(temporaldeck_expander::HostToDisplay)) {
        validMessage = true;
        latestMsg = msg;
        previewValidNow = (msg->flags & temporaldeck_expander::FLAG_PREVIEW_VALID) != 0u;
        if (msg->publishSeq != lastPublishSeq) {
          lastPublishSeq = msg->publishSeq;
          staleFrames = 0;
        } else {
          staleFrames++;
        }
      }
    }
    previewValid = previewValidNow;

    if (validMessage) {
      invalidMessageTimerSec = 0.f;
    } else {
      invalidMessageTimerSec = std::min(invalidMessageTimerSec + args.sampleTime, 1e9f);
    }

    if (validMessage && previewValidNow) {
      invalidPreviewTimerSec = 0.f;
    } else {
      invalidPreviewTimerSec = std::min(invalidPreviewTimerSec + args.sampleTime, 1e9f);
    }

    bool hasTemporalDeckNeighbor = isTemporalDeckModule(leftExpander.module);
    bool linkActive = hasTemporalDeckNeighbor && staleFrames < 2048 && invalidMessageTimerSec <= kLinkDropGraceSec;
    bool previewVisible = invalidPreviewTimerSec <= kPreviewDropGraceSec;
    uiLinkActive.store(linkActive, std::memory_order_relaxed);
    uiPreviewValid.store(linkActive && previewVisible, std::memory_order_relaxed);

    // Publish requested scope payload format back to TemporalDeck.
    if (isTemporalDeckModule(leftExpander.module) && leftExpander.module->rightExpander.producerMessage) {
      uint32_t requestedScopeFormat = (scopeChannelMode == SCOPE_CHANNEL_STEREO)
                                        ? temporaldeck_expander::SCOPE_FORMAT_STEREO
                                        : temporaldeck_expander::SCOPE_FORMAT_MONO;
      bool lagDragActive = uiLagDragActive.load(std::memory_order_relaxed);
      float lagDragSamples = uiLagDragSamples.load(std::memory_order_relaxed);
      float lagDragVelocity = uiLagDragVelocity.load(std::memory_order_relaxed);
      if (!std::isfinite(lagDragSamples) || lagDragSamples < 0.f) {
        lagDragSamples = 0.f;
      }
      if (!std::isfinite(lagDragVelocity)) {
        lagDragVelocity = 0.f;
      }
      float requestIntervalSec = lagDragActive ? kRequestPublishIntervalDragSec : kRequestPublishIntervalSec;
      requestPublishTimerSec += args.sampleTime;
      bool formatChanged = requestedScopeFormat != lastRequestedScopeFormat;
      bool lagStateChanged = lagDragActive != lastLagDragActive;
      bool lagValueChanged = lagDragActive && (std::fabs(lagDragSamples - lastLagDragSamples) >= (1.f / 16.f) ||
                                               std::fabs(lagDragVelocity - lastLagDragVelocity) >= 0.1f);
      bool timerElapsed = requestPublishTimerSec >= requestIntervalSec;
      if (formatChanged || lagStateChanged || lagValueChanged || timerElapsed) {
        if (timerElapsed) {
          requestPublishTimerSec = std::fmod(requestPublishTimerSec, requestIntervalSec);
        } else {
          requestPublishTimerSec = 0.f;
        }
        auto *request =
          reinterpret_cast<temporaldeck_expander::DisplayToHost *>(leftExpander.module->rightExpander.producerMessage);
        if (request) {
          requestSeq++;
          temporaldeck_expander::populateDisplayRequest(request, requestSeq, requestedScopeFormat, lagDragActive,
                                                        lagDragSamples, lagDragVelocity);
          leftExpander.module->rightExpander.messageFlipRequested = true;
          lastRequestedScopeFormat = requestedScopeFormat;
          lastLagDragActive = lagDragActive;
          lastLagDragSamples = lagDragSamples;
          lastLagDragVelocity = lagDragVelocity;
        }
      }
    } else {
      requestPublishTimerSec = 0.f;
      lastRequestedScopeFormat = uint32_t(-1);
      lastLagDragActive = false;
      lastLagDragSamples = 0.f;
      uiLagDragActive.store(false, std::memory_order_relaxed);
    }

    float uiPublishIntervalSec =
      uiLagDragActive.load(std::memory_order_relaxed) ? kUiPublishIntervalSec : debugUiPublishIntervalSec();
    uiPublishTimerSec += args.sampleTime;
    if (linkActive && latestMsg && uiPublishTimerSec >= uiPublishIntervalSec) {
      uiPublishTimerSec = std::fmod(uiPublishTimerSec, uiPublishIntervalSec);
      publishSnapshotToUi(*latestMsg);
    }

    bool ready = linkActive && previewVisible;
    lights[LINK_LIGHT].setBrightness(linkActive && !ready ? 1.f : 0.f);
    lights[PREVIEW_LIGHT].setBrightness(ready ? 1.f : 0.f);
  }
};

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
  float autoDisplayFullScaleVolts = 5.f;
  bool autoDisplayScaleInitialized = false;
  bool autoLastSampleMode = true;
  float autoLivePeakHoldVolts = 0.f;
  int autoLivePeakHoldFrames = 0;
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
  bool lagDragging = false;
  Vec lagDragCursorPos;
  float lagDragAnchorCursorY = 0.f;
  float lagDragAnchorLagSamples = 0.f;
  float lagDragReferenceHeight = 1.f;
  float lagDragTravelSamples = 0.f;
  float lagDragNormalizedOffset = 0.f;
  float lagDragLocalLagSamples = 0.f;
  float lagDragResidualY = 0.f;
  double lagDragLastMoveSec = 0.0;
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
  bool redrawLastRenderHaloEnabled = false;
  bool redrawLastConnectorsEnabled = false;
  bool redrawLastStereoRightLaneEnabled = false;
  int redrawLastRenderMode = -1;
  int tailRasterImage = -1;
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

  ~TDScopeDisplayWidget() override {
    if (APP && APP->window) {
      NVGcontext *vg = APP->window->vg;
      if (vg) {
        if (tailRasterImage >= 0) {
          nvgDeleteImage(vg, tailRasterImage);
        }
      }
    }
  }

  bool isWithinDisplay(Vec pos) const {
    return pos.x >= 0.f && pos.y >= 0.f && pos.x < box.size.x && pos.y < box.size.y;
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
    float sampleRate = std::max(msg.sampleRate, 1.f);
    float halfWindowSamples = std::max(0.f, msg.scopeHalfWindowMs * 0.001f * sampleRate);
    float totalWindowSamples = std::max(1.f, 2.f * halfWindowSamples);
    bool sampleMode = (msg.flags & temporaldeck_expander::FLAG_SAMPLE_MODE) != 0u;
    float forwardWindowSamples = halfWindowSamples;
    float backwardWindowSamples = halfWindowSamples;
    if (!sampleMode) {
      forwardWindowSamples = std::min(halfWindowSamples, std::max(msg.lagSamples, 0.f));
      backwardWindowSamples = totalWindowSamples - forwardWindowSamples;
    }
    map.windowTopLag = msg.lagSamples + backwardWindowSamples;
    map.windowBottomLag = msg.lagSamples - forwardWindowSamples;
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

  bool beginLagDragAt(Vec pos) {
    if (!module || !hasLastGoodMsg || !isWithinDisplay(pos)) {
      return false;
    }
    ScopeWindowMap map = buildScopeWindowMap(lastGoodMsg);
    if (!map.valid) {
      return false;
    }
    lagDragging = true;
    lagDragCursorPos = pos;
    lagDragAnchorCursorY = pos.y;
    lagDragResidualY = 0.f;
    lagDragLastMoveSec = system::getTime();
    float anchorLagSamples = clamp(lastGoodMsg.lagSamples, 0.f, map.accessibleLag);
    lagDragAnchorLagSamples = anchorLagSamples;
    lagDragReferenceHeight = std::max(map.drawYDen, 1.f);
    lagDragTravelSamples = std::max(map.windowTopLag - map.windowBottomLag, 1.f);
    lagDragNormalizedOffset = 0.f;
    lagDragLocalLagSamples = anchorLagSamples;
    module->setLagDragRequest(true, lagDragLocalLagSamples, 0.f);
    return true;
  }

  void endLagDrag() {
    lagDragging = false;
    lagDragResidualY = 0.f;
    lagDragReferenceHeight = 1.f;
    lagDragTravelSamples = 0.f;
    lagDragNormalizedOffset = 0.f;
    if (module) {
      module->setLagDragRequest(false, 0.f);
    }
  }

  void onButton(const event::Button &e) override {
    if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
      if (e.action == GLFW_PRESS && isWithinDisplay(e.pos)) {
        (void)beginLagDragAt(e.pos);
        e.consume(this);
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

  void onDragStart(const event::DragStart &e) override {
    Widget::onDragStart(e);
  }

  void onDragMove(const event::DragMove &e) override {
    if (!lagDragging || e.button != GLFW_MOUSE_BUTTON_LEFT || !module || !hasLastGoodMsg) {
      Widget::onDragMove(e);
      return;
    }
    double nowSec = system::getTime();
    // Match platter gesture timing bounds to avoid tiny-dt velocity spikes
    // that can over-drive scratch motion.
    constexpr double kMinGestureDtSec = 1.0 / 240.0;
    constexpr double kMaxGestureDtSec = 1.0 / 20.0;
    double dtSec = std::max(kMinGestureDtSec, std::min(kMaxGestureDtSec, nowSec - lagDragLastMoveSec));
    lagDragLastMoveSec = nowSec;

    // Full-window time mapping is intentionally high resolution; suppress
    // tiny hand jitter so stationary holds stay truly pinned.
    // Keep this close to platter drag sensitivity so rapid short reversals
    // feel immediate instead of waiting on accumulated cursor motion.
    constexpr float kLagDragJitterDeadzonePx = 0.05f;
    // DragMoveEvent mouseDelta is reported in screen pixels, but the anchor
    // cursor position and widget box size live in rack-local coordinates
    // under ZoomWidget. Normalize delta back into local scope space so drag
    // travel stays consistent across rack zoom levels.
    float localMouseDeltaY = e.mouseDelta.y / currentRackZoom();
    lagDragResidualY += localMouseDeltaY;
    if (std::fabs(lagDragResidualY) < kLagDragJitterDeadzonePx) {
      module->setLagDragRequest(true, lagDragLocalLagSamples, 0.f);
      e.consume(this);
      return;
    }
    float appliedDeltaY = lagDragResidualY;
    lagDragResidualY = 0.f;
    bool sampleMode = (lastGoodMsg.flags & temporaldeck_expander::FLAG_SAMPLE_MODE) != 0u;
    bool freezeActive = (lastGoodMsg.flags & temporaldeck_expander::FLAG_FREEZE) != 0u;
    if (!sampleMode && !freezeActive && appliedDeltaY > 0.f) {
      // CONTAINMENT NOTE
      // This is not architecturally "clean". Scope is compensating for live
      // write-head advance here because, without it, slow downward drags in
      // live mode can fall behind the moving lag reference and feel like they
      // hit a false barrier. The matching trace that exposed this looked like:
      // target lag going stale while frame lag kept advancing.
      //
      // This compensation is intentionally one-sided. Applying it during
      // upward drags created visible/audible stutter because it fought reversal
      // intent. If future work removes or moves this logic, re-test both:
      // 1. slow downward live drags for the "barrier" symptom
      // 2. upward drags for reversal stutter
      // In live mode, compensate write-head advance only while dragging away
      // from NOW (downward). Applying this during upward drags can feel like
      // stutter because compensation fights reversal intent.
      lagDragAnchorLagSamples += std::max(lastGoodMsg.sampleRate, 1.f) * float(dtSec);
    }
    lagDragNormalizedOffset += appliedDeltaY / std::max(lagDragReferenceHeight, 1.f);
    ScopeWindowMap map = buildScopeWindowMap(lastGoodMsg);
    if (!map.valid) {
      return;
    }

    float previousLag = lagDragLocalLagSamples;
    // Keep scope drag in normalized local travel space so behavior is tied to
    // the visible scope window rather than raw rendered pixel density.
    float desiredPlaybackLag = lagDragAnchorLagSamples + lagDragNormalizedOffset * lagDragTravelSamples;
    bool sampleLoop = (lastGoodMsg.flags & temporaldeck_expander::FLAG_SAMPLE_LOOP) != 0u;
    bool sampleLoaded = (lastGoodMsg.flags & temporaldeck_expander::FLAG_SAMPLE_LOADED) != 0u;
    if (sampleMode && sampleLoaded && sampleLoop && map.accessibleLag > 0.f) {
      double wrappedLag = std::fmod(double(desiredPlaybackLag), double(map.accessibleLag) + 1.0);
      if (wrappedLag < 0.0) {
        wrappedLag += double(map.accessibleLag) + 1.0;
      }
      lagDragLocalLagSamples = float(wrappedLag);
    } else {
      // CONTAINMENT NOTE
      // This extra headroom is another live-mode workaround. Fresh host
      // snapshots arrive at UI cadence, not audio cadence, so a strict clamp to
      // the last advertised accessible lag can create a false local ceiling
      // during live drag. The host still remains authoritative; this just keeps
      // the UI-side request from getting artificially pinned too early.
      // Live (non-sample) streaming advances the write head between UI updates.
      // Add dynamic headroom so scope-side clamping doesn't create a false
      // downward barrier while dragging away from NOW.
      float effectiveAccessibleLag = map.accessibleLag;
      if (!sampleMode && !freezeActive && lastGoodMsgTimeSec >= 0.0) {
        double msgAgeSec = std::max(0.0, nowSec - lastGoodMsgTimeSec);
        effectiveAccessibleLag += std::max(lastGoodMsg.sampleRate, 1.f) * float(msgAgeSec);
      }
      lagDragLocalLagSamples = clamp(desiredPlaybackLag, 0.f, std::max(0.f, effectiveAccessibleLag));
    }
    // WARNING: Keep velocity sign aligned with setLagDragRequest() contract:
    // positive velocity means toward NOW (lag decreasing), hence
    // (previousLag - currentLag) / dt.
    float velocitySamples = (previousLag - lagDragLocalLagSamples) / float(dtSec);
    float sampleRate = std::max(lastGoodMsg.sampleRate, 1.f);
    float maxAbsGestureVelocity = sampleRate * 3.0f;
    velocitySamples = clamp(velocitySamples, -maxAbsGestureVelocity, maxAbsGestureVelocity);
    module->setLagDragRequest(true, lagDragLocalLagSamples, velocitySamples);
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
    if (!framebuffer) {
      return;
    }
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
      if (module->debugRenderHaloEnabled != redrawLastRenderHaloEnabled) {
        dirty = true;
        redrawLastRenderHaloEnabled = module->debugRenderHaloEnabled;
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

    if (lagDragging) {
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
      const float centerY = box.size.y * 0.5f;
      const float fontSize1 = 12.f;
      const float fontSize2 = 14.f;
      const float lineGap = 9.f;

      nvgSave(args.vg);
      nvgFontFaceId(args.vg, APP->window->uiFont->handle);
      nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);

      nvgFontSize(args.vg, fontSize1);
      nvgFillColor(args.vg, nvgRGBA(150, 176, 190, 220));
      nvgText(args.vg, centerX, centerY - lineGap, line1, nullptr);

      nvgFontSize(args.vg, fontSize2);
      nvgFillColor(args.vg, nvgRGBA(224, 238, 244, 236));
      nvgText(args.vg, centerX, centerY + lineGap, line2, nullptr);
      nvgRestore(args.vg);
    };

    if (!module) {
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
    const float yDen = std::max(drawHeight - 1.f, 1.f);
    const bool msgChanged = !cachedGeometryValid || msg.publishSeq != cachedPublishSeq;
    const bool rangeModeChanged = module->scopeDisplayRangeMode != cachedRangeMode;
    float displayFullScaleVolts = std::max(module->scopeDisplayFullScaleVolts(), 0.001f);
    if (module->scopeDisplayRangeMode == TDScope::SCOPE_RANGE_AUTO) {
      bool sampleMode = (msg.flags & temporaldeck_expander::FLAG_SAMPLE_MODE) != 0u;
      bool sampleLoaded = (msg.flags & temporaldeck_expander::FLAG_SAMPLE_LOADED) != 0u;
      auto computeLivePeakStats = [&]() -> std::pair<float, float> {
        // Returns {windowPeakVolts, p99Volts} over per-bin absolute peaks.
        std::vector<float> peaks;
        peaks.reserve(renderStereo ? scopeBinCount * 2u : scopeBinCount);
        for (uint32_t i = 0; i < scopeBinCount; ++i) {
          const temporaldeck_expander::ScopeBin &bin = leftScopeBins[i];
          if (!temporaldeck_expander::isScopeBinValid(bin)) {
            // fall through to right channel in stereo mode
          } else {
            int peakQ = std::max(std::abs(int(bin.min)), std::abs(int(bin.max)));
            float peakV = (float(peakQ) / 32767.f) * temporaldeck_expander::kPreviewQuantizeVolts;
            peaks.push_back(clamp(peakV, 0.f, temporaldeck_expander::kPreviewQuantizeVolts));
          }
          if (renderStereo) {
            const temporaldeck_expander::ScopeBin &binR = rightScopeBins[i];
            if (temporaldeck_expander::isScopeBinValid(binR)) {
              int peakQR = std::max(std::abs(int(binR.min)), std::abs(int(binR.max)));
              float peakVR = (float(peakQR) / 32767.f) * temporaldeck_expander::kPreviewQuantizeVolts;
              peaks.push_back(clamp(peakVR, 0.f, temporaldeck_expander::kPreviewQuantizeVolts));
            }
          }
        }
        if (peaks.empty()) {
          return std::make_pair(0.f, 0.f);
        }
        float windowPeakVolts = *std::max_element(peaks.begin(), peaks.end());
        size_t n = peaks.size();
        size_t rank = size_t(std::ceil(0.99f * float(n)));
        rank = std::max<size_t>(1, std::min(rank, n));
        size_t p99Index = rank - 1;
        std::nth_element(peaks.begin(), peaks.begin() + ptrdiff_t(p99Index), peaks.end());
        float p99Volts = peaks[p99Index];
        return std::make_pair(windowPeakVolts, p99Volts);
      };

      // Recompute auto-scale state only when snapshot updates (or mode changes).
      if (msgChanged || rangeModeChanged || !autoDisplayScaleInitialized) {
        bool modeChanged = !autoDisplayScaleInitialized || sampleMode != autoLastSampleMode;
        float targetFullScaleVolts = 5.f;
        if (sampleMode) {
          float peakVolts = 0.f;
          bool haveTrustedSamplePeak = sampleLoaded && std::isfinite(msg.sampleAbsolutePeakVolts);
          if (haveTrustedSamplePeak) {
            // In sample mode with loaded sample, treat 0V as a valid true peak
            // (e.g., fully silent sample) instead of falling back to bin scanning.
            peakVolts = std::max(msg.sampleAbsolutePeakVolts, 0.f);
          } else {
            auto liveStats = computeLivePeakStats();
            peakVolts = liveStats.first;
          }
          targetFullScaleVolts = clamp(peakVolts * 1.08f, 0.25f, temporaldeck_expander::kPreviewQuantizeVolts);
        } else {
          auto liveStats = computeLivePeakStats();
          float windowPeakVolts = liveStats.first;
          float p99Volts = liveStats.second;
          if (p99Volts <= 0.f) {
            p99Volts = windowPeakVolts;
          }
          float hostPeakVolts =
            (std::isfinite(msg.sampleAbsolutePeakVolts) && msg.sampleAbsolutePeakVolts > 0.f) ? msg.sampleAbsolutePeakVolts : 0.f;
          float truePeakVolts = std::max(windowPeakVolts, hostPeakVolts);

          // Guardrail: hold true peaks, then decay slowly so transients don't
          // immediately collapse display width.
          if (modeChanged || !std::isfinite(autoLivePeakHoldVolts)) {
            autoLivePeakHoldVolts = truePeakVolts;
            autoLivePeakHoldFrames = 0;
          } else if (truePeakVolts > autoLivePeakHoldVolts) {
            autoLivePeakHoldVolts = truePeakVolts;
            autoLivePeakHoldFrames = 24; // ~400ms @ 60Hz
          } else if (autoLivePeakHoldFrames > 0) {
            autoLivePeakHoldFrames--;
          } else {
            autoLivePeakHoldVolts += (truePeakVolts - autoLivePeakHoldVolts) * 0.03f;
          }

          targetFullScaleVolts = std::max(p99Volts * 1.10f, autoLivePeakHoldVolts * 1.02f);
          targetFullScaleVolts = clamp(targetFullScaleVolts, 0.25f, temporaldeck_expander::kPreviewQuantizeVolts);

          // Hysteresis: ignore small retargeting around current full-scale.
          if (!modeChanged) {
            float hysteresisFrac = 0.03f;
            float lowBand = autoDisplayFullScaleVolts * (1.f - hysteresisFrac);
            float highBand = autoDisplayFullScaleVolts * (1.f + hysteresisFrac);
            if (targetFullScaleVolts >= lowBand && targetFullScaleVolts <= highBand) {
              targetFullScaleVolts = autoDisplayFullScaleVolts;
            }
          }
        }
        if (!autoDisplayScaleInitialized) {
          autoDisplayFullScaleVolts = targetFullScaleVolts;
          autoDisplayScaleInitialized = true;
        } else {
          // Smooth autoscale transitions.
          // In live mode keep slower motion to minimize flicker.
          float delta = targetFullScaleVolts - autoDisplayFullScaleVolts;
          if (std::fabs(delta) > 0.01f) {
            float kAutoScaleAttackAlpha = sampleMode ? 0.16f : 0.10f;
            float kAutoScaleReleaseAlpha = sampleMode ? 0.08f : 0.03f;
            float alpha = delta > 0.f ? kAutoScaleAttackAlpha : kAutoScaleReleaseAlpha;
            autoDisplayFullScaleVolts += delta * alpha;
          }
        }
        autoLastSampleMode = sampleMode;
      }
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
    // Compensate stroke thickness by rack zoom so scope readability remains
    // steadier when modules are zoomed in/out.
    float zoomThicknessMul = clamp(1.f + 0.30f * std::log2(1.f / rackZoom), 0.70f, 1.42f);
    module->uiDebugScopeRackZoom.store(rackZoom, std::memory_order_relaxed);
    module->uiDebugScopeZoomThicknessMul.store(zoomThicknessMul, std::memory_order_relaxed);
    float halfWindowSamples = std::max(0.f, msg.scopeHalfWindowMs * 0.001f * std::max(msg.sampleRate, 1.f));
    float totalWindowSamples = std::max(1.f, 2.f * halfWindowSamples);
    bool sampleMode = (msg.flags & temporaldeck_expander::FLAG_SAMPLE_MODE) != 0u;
    float forwardWindowSamples = halfWindowSamples;
    float backwardWindowSamples = halfWindowSamples;
    if (!sampleMode) {
      // Live mode viewport bias near NOW:
      // lag=0 => read-head at bottom, lag=halfWindow => read-head centered.
      forwardWindowSamples = std::min(halfWindowSamples, std::max(msg.lagSamples, 0.f));
      backwardWindowSamples = totalWindowSamples - forwardWindowSamples;
    }
    float windowTopLag = msg.lagSamples + backwardWindowSamples;
    float windowBottomLag = msg.lagSamples - forwardWindowSamples;
    float readHeadT = 0.5f;
    if (windowTopLag != windowBottomLag) {
      readHeadT = clamp((msg.lagSamples - windowTopLag) / (windowBottomLag - windowTopLag), 0.f, 1.f);
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
      readHeadDrawY += kReadHeadNowNudgePx * nowProximity;
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
      if (renderStereo && module->debugRenderStereoRightLaneEnabled) {
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
    float displaySupersample = computeScopeDisplayVerticalSupersample(rackZoom);
    const int rowCount = std::max(1, int(std::ceil(drawHeight * displaySupersample)));
    const int fullDensityRowCount = std::max(1, int(std::ceil(drawHeight * kScopeDisplayVerticalSupersampleMax)));
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
    bool shouldRebuild = !cachedGeometryValid || msgChanged || rangeModeChanged || rowCount != cachedRowCount || stereoLayoutChanged;

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
        float t0 = clamp(float(iy) / float(rowCount), 0.f, 1.f);
        float t1 = clamp(float(iy + 1) / float(rowCount), 0.f, 1.f);
        float rowMinNorm = 0.f;
        float rowMaxNorm = 0.f;
        if (!sampleEnvelopeOverInterval(scopeData, t0, t1, &rowMinNorm, &rowMaxNorm)) {
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
        constexpr float kIntensityGamma = 0.68f;
        float visualIntensity = clamp(std::pow(intensity, kIntensityGamma) * 1.06f, 0.f, 1.f);
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
        constexpr float kIntensityGamma = 0.68f;
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
          float visualIntensity = clamp(std::pow(intensity, kIntensityGamma) * 1.06f * fillInfluence, 0.f, 1.f);
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
      cachedRangeMode = module->scopeDisplayRangeMode;
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
      cachedRangeMode = module->scopeDisplayRangeMode;
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
        case TDScope::COLOR_SCHEME_EMERALD:
          lowR = 15.f;
          lowG = 79.f;
          lowB = 54.f;
          midR = 47.f;
          midG = 168.f;
          midB = 110.f;
          highR = 87.f;
          highG = 240.f;
          highB = 182.f;
          midPoint = 0.52f;
          break;
        case TDScope::COLOR_SCHEME_WASP:
          lowR = 33.f;
          lowG = 27.f;
          lowB = 18.f;
          midR = 231.f;
          midG = 137.f;
          midB = 47.f;
          highR = 255.f;
          highG = 216.f;
          highB = 74.f;
          midPoint = 0.58f;
          break;
        case TDScope::COLOR_SCHEME_PIXIE:
          lowR = 255.f;
          lowG = 143.f;
          lowB = 209.f;
          midR = 211.f;
          midG = 180.f;
          midB = 239.f;
          highR = 129.f;
          highG = 255.f;
          highB = 210.f;
          midPoint = 0.48f;
          break;
        case TDScope::COLOR_SCHEME_VIOLET_FLAME:
          lowR = 42.f;
          lowG = 31.f;
          lowB = 95.f;
          midR = 147.f;
          midG = 71.f;
          midB = 217.f;
          highR = 181.f;
          highG = 109.f;
          highB = 255.f;
          midPoint = 0.54f;
          break;
        case TDScope::COLOR_SCHEME_ANGELIC:
          lowR = 248.f;
          lowG = 245.f;
          lowB = 255.f;
          midR = 232.f;
          midG = 220.f;
          midB = 255.f;
          highR = 179.f;
          highG = 229.f;
          highB = 255.f;
          midPoint = 0.40f;
          break;
        case TDScope::COLOR_SCHEME_HELLFIRE:
          lowR = 120.f;
          lowG = 24.f;
          lowB = 15.f;
          midR = 255.f;
          midG = 111.f;
          midB = 43.f;
          highR = 255.f;
          highG = 209.f;
          highB = 102.f;
          midPoint = 0.60f;
          break;
        case TDScope::COLOR_SCHEME_PICKLE:
          lowR = 62.f;
          lowG = 111.f;
          lowB = 49.f;
          midR = 132.f;
          midG = 185.f;
          midB = 72.f;
          highR = 190.f;
          highG = 234.f;
          highB = 97.f;
          midPoint = 0.56f;
          break;
        case TDScope::COLOR_SCHEME_LEVIATHAN:
          lowR = 122.f;
          lowG = 92.f;
          lowB = 255.f;
          midR = 75.f;
          midG = 141.f;
          midB = 255.f;
          highR = 28.f;
          highG = 204.f;
          highB = 217.f;
          midPoint = 0.52f;
          break;
        case TDScope::COLOR_SCHEME_TEMPORAL_DECK:
        default:
          lowR = 85.f;
          lowG = 227.f;
          lowB = 238.f;
          midR = 255.f;
          midG = 191.f;
          midB = 86.f;
          highR = 233.f;
          highG = 112.f;
          highB = 218.f;
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
      // Keep a strong top-end accent without paying for a second boost stroke pass.
      float hotT = clamp((clamp(intensity, 0.f, 1.f) - 0.82f) / 0.18f, 0.f, 1.f);
      hotT = hotT * hotT;
      float hotLift = 0.24f * hotT;
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

    auto ensureTailRasterImage = [&]() {
      int targetW = std::max(1, int(std::ceil(box.size.x)));
      int targetH = std::max(1, int(std::ceil(box.size.y)));
      bool recreate = (tailRasterImage < 0) || (tailRasterW != targetW) || (tailRasterH != targetH);
      if (!recreate) {
        return;
      }
      if (tailRasterImage >= 0) {
        nvgDeleteImage(args.vg, tailRasterImage);
        tailRasterImage = -1;
      }
      tailRasterW = targetW;
      tailRasterH = targetH;
      tailRasterPixels.assign(size_t(tailRasterW * tailRasterH * 4), 0u);
      tailRasterImage = nvgCreateImageRGBA(args.vg, tailRasterW, tailRasterH, NVG_IMAGE_PREMULTIPLIED, nullptr);
      tailRasterValid = false;
      tailRasterShiftResidualPx = 0.f;
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
        constexpr uint8_t kHaloMinAlphaToDraw = 28u;
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
          float tone = clamp(0.78f * visual + 0.22f * transientLift, 0.f, 1.f);
          NVGcolor c = brightenColor(
            gradientColorForIntensity(tone, uint8_t(std::lround(122.f + 120.f * tone))),
            transientLift * 0.90f);
          uint8_t r = uint8_t(std::lround(clamp(c.r, 0.f, 1.f) * 255.f));
          uint8_t g = uint8_t(std::lround(clamp(c.g, 0.f, 1.f) * 255.f));
          uint8_t b = uint8_t(std::lround(clamp(c.b, 0.f, 1.f) * 255.f));
          uint8_t a = uint8_t(std::lround(clamp(c.a, 0.f, 1.f) * 255.f));
          int xMin = int(std::lround(std::min(x0[idx], x1[idx])));
          int xMax = int(std::lround(std::max(x0[idx], x1[idx])));

          if (module->debugRenderHaloEnabled && module->scopeTransientHaloEnabled) {
            float haloLinear = clamp((transientLift - 0.080f) / 0.920f, 0.f, 1.f);
            float haloT = haloLinear * haloLinear;
            uint8_t haloAlpha = uint8_t(std::lround((72.f + 176.f * std::max(visual, 0.24f)) * haloT));
            if (haloAlpha >= kHaloMinAlphaToDraw) {
              int haloExtend = int(std::lround((1.35f + 5.20f * haloT) * zoomThicknessMul));
              int haloHalfW = std::max(1, int(std::lround((0.50f + 1.20f * haloT) * zoomThicknessMul)));
              for (int yy = yPx - haloHalfW; yy <= yPx + haloHalfW; ++yy) {
                fillTailRasterSpan(yy, xMin - haloExtend, xMax + haloExtend, 255u, 255u, 255u, haloAlpha);
              }
            }
          }

          if (module->debugRenderMainTraceEnabled) {
            int halfW = std::max(0, int(std::lround((0.78f + 0.62f * tone) * zoomThicknessMul * 0.5f)));
            for (int yy = yPx - halfW; yy <= yPx + halfW; ++yy) {
              fillTailRasterSpan(yy, xMin, xMax, r, g, b, a);
            }
          }

          if (module->debugRenderConnectorsEnabled && prevValid) {
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
    nvgScissor(args.vg, 0.f, drawTop, box.size.x, drawBottom - drawTop);

    auto drawLane = [&](const std::vector<float> &x0, const std::vector<float> &x1, const std::vector<float> &visualIntensity,
                        const std::vector<float> &colorDrive, const std::vector<uint8_t> &valid,
                        float laneCenterXForConnectors) {
      // Batch trace rendering into ordered intensity bins so NanoVG receives
      // far fewer stroke submissions while preserving a low->high gradient
      // progression across the scope.
      constexpr uint8_t kHaloMinAlphaToDraw = 28u;
      constexpr float kHaloFullDensityThreshold = 0.72f;
      constexpr int kMainStrokeBins = 10;
      constexpr int kConnectorStrokeBins = 8;
      const bool denseHaloRows = rowStep <= 0.75f;
      const bool fullHaloDensity = rackZoom >= 2.0f;

      auto quantizeStrokeBin = [&](float t, int binCount) -> int {
        t = clamp(t, 0.f, 1.f);
        return clamp(int(std::floor(t * float(binCount))), 0, binCount - 1);
      };
      auto strokeBinCenter = [&](int bin, int binCount) -> float {
        return (float(bin) + 0.5f) / float(binCount);
      };

      // Keep halo as a separate diffuse layer so stronger transients can still
      // bloom independently of the quantized main-trace bins.
      for (int iy = 0; iy < rowCount; ++iy) {
        size_t idx = size_t(iy);
        if (!valid[idx]) {
          continue;
        }

        float visual = clamp(visualIntensity[idx], 0.f, 1.f);
        float transientLift = clamp(colorDrive[idx], 0.f, 1.f);
        float mainW = (0.78f + 0.62f * visual) * zoomThicknessMul;
        // Bias toward faster fade-out for lower-transient content while
        // preserving strong halo response for high-transient peaks.
        float haloLinear = clamp((transientLift - 0.080f) / 0.920f, 0.f, 1.f);
        float haloT = haloLinear * haloLinear;

        if (module->debugRenderHaloEnabled && module->scopeTransientHaloEnabled && haloT > 1e-4f) {
          uint8_t haloAlpha = uint8_t(std::lround((72.f + 176.f * std::max(visual, 0.24f)) * haloT));
          bool drawHaloRow = haloAlpha >= kHaloMinAlphaToDraw;
          // The main trace already renders every supersampled row. For the
          // diffuse halo, decimating lower-energy rows in dense layouts keeps
          // the look while materially cutting UI-thread stroke count.
          if (drawHaloRow && !fullHaloDensity && denseHaloRows && haloT < kHaloFullDensityThreshold && (iy & 1)) {
            drawHaloRow = false;
          }
          if (drawHaloRow) {
            float haloExtend = (1.35f + 5.20f * haloT) * zoomThicknessMul;
            float haloW = mainW + (1.10f + 2.20f * haloT) * zoomThicknessMul;
            nvgBeginPath(args.vg);
            nvgMoveTo(args.vg, x0[idx] - haloExtend, rowY[idx]);
            nvgLineTo(args.vg, x1[idx] + haloExtend, rowY[idx]);
            nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, haloAlpha));
            nvgStrokeWidth(args.vg, haloW);
            nvgStroke(args.vg);
          }
        }
      }

      if (module->debugRenderMainTraceEnabled) {
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
            float tone = clamp(0.78f * visual + 0.22f * transientLift, 0.f, 1.f);
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
            transientCenter * 0.90f);
          float mainW = (0.78f + 0.62f * visualCenter) * zoomThicknessMul;
          nvgStrokeColor(args.vg, mainC);
          nvgStrokeWidth(args.vg, mainW);
          nvgStroke(args.vg);
        }
      }

      if (module->debugRenderConnectorsEnabled) {
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
            connectTransientLift * 0.72f);
          float connectW = (0.58f + 0.40f * connectVisual) * zoomThicknessMul;
          nvgStrokeColor(args.vg, connectC);
          nvgStrokeWidth(args.vg, connectW);
          nvgStroke(args.vg);
        }
      }
    };

    bool useTailRaster = module->useTailRasterRenderMode();
    if (!useTailRaster) {
      drawLane(rowX0, rowX1, rowVisualIntensity, rowColorDrive, rowValid, lane0CenterX);
      if (renderStereo && module->debugRenderStereoRightLaneEnabled) {
        drawLane(rowX0Right, rowX1Right, rowVisualIntensityRight, rowColorDriveRight, rowValidRight, lane1CenterX);

        // Draw subtle lane divider for stereo side-by-side view.
        float dividerX = laneWidth + laneGap * 0.5f;
        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, dividerX, drawTop);
        nvgLineTo(args.vg, dividerX, drawBottom);
        nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 22));
        nvgStrokeWidth(args.vg, 1.f);
        nvgStroke(args.vg);
      }
      tailRasterValid = false;
    } else {
      ensureTailRasterImage();
      if (tailRasterImage >= 0 && tailRasterW > 0 && tailRasterH > 0) {
        bool sameStereo = tailRasterStereo == renderStereo;
        float prevSpan = std::max(std::fabs(tailRasterWindowTopLag - tailRasterWindowBottomLag), 1e-6f);
        float currSpan = std::max(std::fabs(windowTopLag - windowBottomLag), 1e-6f);
        bool spanCompatible = std::fabs(currSpan - prevSpan) <= std::max(1e-4f, 0.01f * currSpan);
        bool canIncremental =
          tailRasterValid && sameStereo && spanCompatible && !lagDragging && (msg.publishSeq != tailRasterPublishSeq);

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
        if (renderStereo && module->debugRenderStereoRightLaneEnabled) {
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
    if (renderStereo && module->debugRenderStereoRightLaneEnabled) {
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
  bool redrawLastRenderHaloEnabled = false;
  bool redrawLastConnectorsEnabled = false;
  bool redrawLastStereoRightLaneEnabled = false;
  int redrawLastRenderMode = -1;
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
    if (module->debugRenderHaloEnabled != redrawLastRenderHaloEnabled) {
      dirty = true;
      redrawLastRenderHaloEnabled = module->debugRenderHaloEnabled;
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
    float displaySupersample = computeScopeDisplayVerticalSupersample(rackZoom);
    const int rowCount = std::max(1, int(std::ceil(drawHeight * displaySupersample)));
    const int fullDensityRowCount = std::max(1, int(std::ceil(drawHeight * kScopeDisplayVerticalSupersampleMax)));
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
          float prevDrive = clamp((*colorDriveOut)[idx], 0.f, 1.f);
          if (!isValid(idx)) {
            (*colorDriveOut)[idx] = prevDrive * 0.82f;
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
          float brightnessLift =
            clamp((0.38f + 0.92f * baseDrive) * std::pow(clamp(transientNorm, 0.f, 1.f), 0.56f * 0.84f), 0.f, 1.f);
          float alpha = (brightnessLift > prevDrive) ? 0.42f : 0.14f;
          (*colorDriveOut)[idx] = clamp(prevDrive + (brightnessLift - prevDrive) * alpha, 0.f, 1.f);
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
        float prevDrive = clamp((*colorDriveOut)[idx], 0.f, 1.f);
        if (!isValid(idx)) {
          (*colorDriveOut)[idx] = prevDrive * 0.82f;
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
        float brightnessLift =
          clamp((0.38f + 0.92f * baseDrive) * std::pow(clamp(transientNorm, 0.f, 1.f), 0.56f * 0.84f), 0.f, 1.f);
        float alpha = (brightnessLift > prevDrive) ? 0.42f : 0.14f;
        (*colorDriveOut)[idx] = clamp(prevDrive + (brightnessLift - prevDrive) * alpha, 0.f, 1.f);
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
        shiftVisibleColorDrive(&rowColorDrive, visibleShiftRows);
        if (renderStereo) {
          shiftVisibleColorDrive(&rowColorDriveRight, visibleShiftRows);
        }
      }
      int visibleRebuildStart = fullHistoryRebuild ? 0 : (rebuildStart - historyMarginRows);
      int visibleRebuildEnd = fullHistoryRebuild ? (rowCount - 1) : (rebuildEnd - historyMarginRows);
      visibleRebuildStart = clamp(visibleRebuildStart - 1, 0, rowCount - 1);
      visibleRebuildEnd = clamp(visibleRebuildEnd + 1, 0, rowCount - 1);
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
      float midR = 255.f, midG = 191.f, midB = 86.f;
      float highR = 233.f, highG = 112.f, highB = 218.f;
      float midPoint = 0.5f;
      switch (scheme) {
        case TDScope::COLOR_SCHEME_EMERALD: lowR = 15.f; lowG = 79.f; lowB = 54.f; midR = 47.f; midG = 168.f; midB = 110.f; highR = 87.f; highG = 240.f; highB = 182.f; midPoint = 0.52f; break;
        case TDScope::COLOR_SCHEME_WASP: lowR = 33.f; lowG = 27.f; lowB = 18.f; midR = 231.f; midG = 137.f; midB = 47.f; highR = 255.f; highG = 216.f; highB = 74.f; midPoint = 0.58f; break;
        case TDScope::COLOR_SCHEME_PIXIE: lowR = 255.f; lowG = 143.f; lowB = 209.f; midR = 211.f; midG = 180.f; midB = 239.f; highR = 129.f; highG = 255.f; highB = 210.f; midPoint = 0.48f; break;
        case TDScope::COLOR_SCHEME_VIOLET_FLAME: lowR = 42.f; lowG = 31.f; lowB = 95.f; midR = 147.f; midG = 71.f; midB = 217.f; highR = 181.f; highG = 109.f; highB = 255.f; midPoint = 0.54f; break;
        case TDScope::COLOR_SCHEME_ANGELIC: lowR = 248.f; lowG = 245.f; lowB = 255.f; midR = 232.f; midG = 220.f; midB = 255.f; highR = 179.f; highG = 229.f; highB = 255.f; midPoint = 0.40f; break;
        case TDScope::COLOR_SCHEME_HELLFIRE: lowR = 120.f; lowG = 24.f; lowB = 15.f; midR = 255.f; midG = 111.f; midB = 43.f; highR = 255.f; highG = 209.f; highB = 102.f; midPoint = 0.60f; break;
        case TDScope::COLOR_SCHEME_PICKLE: lowR = 62.f; lowG = 111.f; lowB = 49.f; midR = 132.f; midG = 185.f; midB = 72.f; highR = 190.f; highG = 234.f; highB = 97.f; midPoint = 0.56f; break;
        case TDScope::COLOR_SCHEME_LEVIATHAN: lowR = 122.f; lowG = 92.f; lowB = 255.f; midR = 75.f; midG = 141.f; midB = 255.f; highR = 28.f; highG = 204.f; highB = 217.f; midPoint = 0.52f; break;
        default: break;
      }
      for (int i = 0; i < 256; ++i) {
        float intensity = float(i) / 255.f;
        float r = 0.f, g = 0.f, b = 0.f;
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
    constexpr int kGlHaloStrokeBins = 8;
    constexpr int kGlMainStrokeBins = 10;
    constexpr int kGlConnectorStrokeBins = 8;
    constexpr float kGlMainAlphaGain = 1.18f;
    constexpr float kGlMainLiftGain = 1.16f;
    constexpr float kGlConnectorAlphaGain = 1.14f;
    constexpr float kGlConnectorLiftGain = 1.12f;
    constexpr float kGlHaloAlphaGain = 1.34f;
    constexpr float kGlHaloWidthGain = 1.10f;
    constexpr float kGlMainWidthGain = 1.10f;
    constexpr float kGlConnectorWidthGain = 1.08f;
    constexpr float kGlDeepZoomEnergyFillAlpha = 0.24f;
    constexpr float kGlConnectorBodyFillAlpha = 0.20f;
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
      float glZoomInAlphaComp = 1.f + 0.46f * glZoomInEase + 0.62f * glDeepZoomEase;
      float glZoomInLiftComp = 1.f + 0.30f * glZoomInEase + 0.36f * glDeepZoomEase;
      float glZoomInHaloAlphaComp = 1.f + 0.58f * glZoomInEase + 0.76f * glDeepZoomEase;
      float glDeepZoomEnergyFill = glDeepZoomEase;
      auto drawBatch = [&](std::vector<GlLineVertex> &verts, float width) {
        if (verts.empty()) {
          return;
        }
        glLineWidth(width);
        glVertexPointer(2, GL_FLOAT, sizeof(GlLineVertex), &verts[0].x);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(GlLineVertex), &verts[0].r);
        glDrawArrays(GL_LINES, 0, GLsizei(verts.size()));
      };
      if (module->debugRenderHaloEnabled && module->scopeTransientHaloEnabled) {
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
          float haloLinear = clamp((transientLift - 0.080f) / 0.920f, 0.f, 1.f);
          float haloT = haloLinear * haloLinear;
          uint8_t haloAlpha = uint8_t(std::lround((72.f + 176.f * std::max(visual, 0.24f)) * haloT));
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
          drawBatch(haloBatchVerts[size_t(widthBin)], haloW);
        }
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      }
      if (module->debugRenderMainTraceEnabled) {
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
          drawBatch(mainBatchVerts[size_t(widthBin)], mainW);
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
            drawBatch(fillVerts, fillW);
          }
          glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
      }
      if (module->debugRenderConnectorsEnabled) {
        const float connectorMinDeltaPx = std::max(0.60f * zoomThicknessMul, 0.40f);
        for (auto &verts : connectorBatchVerts) {
          verts.clear();
        }
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
                  (88.f + 92.f * connectVisual) * kGlConnectorAlphaGain * glZoomInAlphaComp, 0.f, 255.f)))),
              clamp(connectTransientLift * 0.72f * kGlConnectorLiftGain * glZoomInLiftComp, 0.f, 1.f));
            GLubyte r = encodeColorByte(c.r);
            GLubyte g = encodeColorByte(c.g);
            GLubyte b = encodeColorByte(c.b);
            GLubyte a = encodeColorByte(c.a);
            connectorBatchVerts[size_t(rowBin)].push_back({prevX0, prevY, r, g, b, a});
            connectorBatchVerts[size_t(rowBin)].push_back({x0, rowY[idx], r, g, b, a});
            connectorBatchVerts[size_t(rowBin)].push_back({prevX1, prevY, r, g, b, a});
            connectorBatchVerts[size_t(rowBin)].push_back({x1, rowY[idx], r, g, b, a});
            float prevCenter = 0.5f * (prevX0 + prevX1);
            float center = 0.5f * (x0 + x1);
            float centerSpan = 0.5f * (std::fabs(prevX1 - prevX0) + std::fabs(x1 - x0));
            if (centerSpan > connectorMinDeltaPx * 1.5f) {
              GLubyte bodyAlpha = GLubyte(std::lround(clamp(
                float(a) * kGlConnectorBodyFillAlpha * (0.65f + 0.35f * glDeepZoomEnergyFill), 0.f, 255.f)));
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
          float toneCenter = strokeBinCenter(widthBin, kGlConnectorStrokeBins);
          float connectW =
            (0.58f + 0.40f * toneCenter) * zoomThicknessMul * kGlConnectorWidthGain * glZoomInWidthComp;
          drawBatch(connectorBatchVerts[size_t(widthBin)], connectW);
          if (!connectorBodyBatchVerts[size_t(widthBin)].empty()) {
            float bodyW = connectW * (0.95f + 0.08f * glDeepZoomEnergyFill);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            drawBatch(connectorBodyBatchVerts[size_t(widthBin)], bodyW);
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

    publishUiDebugMetrics(densityPct, rowCount);
  }
};

struct TDScopeWidget : ModuleWidget {
  PanelBorder *panelBorder = nullptr;
  TDScopeGlWidget *glDisplay = nullptr;
  widget::FramebufferWidget *displayFb = nullptr;
  static constexpr float kTopBarYmm = 9.522227f;
  static constexpr float kTopBarLeftStartMm = 2.2491839f;

  bool shouldRenderDockBridge() const {
    TDScope *scopeModule = static_cast<TDScope *>(module);
    if (!scopeModule) {
      return false;
    }
    return isTemporalDeckModule(scopeModule->leftExpander.module) ||
           scopeModule->uiLinkActive.load(std::memory_order_relaxed);
  }

  TDScopeWidget(TDScope *module) {
    setModule(module);
    const std::string panelPath = asset::plugin(pluginInstance, "res/tdscope.svg");
    setPanel(createPanel(panelPath));
    if (auto *svgPanel = dynamic_cast<app::SvgPanel *>(getPanel())) {
      panelBorder = findPanelBorder(svgPanel->fb);
    }

    math::Rect scopeRectMm;
    if (!panel_svg::loadRectFromSvgMm(panelPath, "scope", &scopeRectMm)) {
      scopeRectMm.pos = Vec(1.1138f, 10.9404f);
      scopeRectMm.size = Vec(38.5563f, 109.4206f);
    }
    glDisplay = new TDScopeGlWidget();
    glDisplay->module = module;
    glDisplay->box.pos = mm2px(scopeRectMm.pos);
    glDisplay->box.size = mm2px(scopeRectMm.size);
    glDisplay->dirtyOnSubpixelChange = false;
    glDisplay->setVisible(module && module->useOpenGlGeometryRenderMode());
    addChild(glDisplay);

    displayFb = new widget::FramebufferWidget();
    displayFb->box.pos = mm2px(scopeRectMm.pos);
    displayFb->box.size = mm2px(scopeRectMm.size);
    displayFb->dirtyOnSubpixelChange = false;

    auto *display = new TDScopeDisplayWidget;
    display->module = module;
    display->framebuffer = displayFb;
    display->box.size = displayFb->box.size;
    displayFb->addChild(display);
    addChild(displayFb);

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

      // Bridge the top purple divider from the left panel edge when docked.
      float y = mm2px(kTopBarYmm);
      float x0 = 0.f;
      float x1 = mm2px(kTopBarLeftStartMm);
      if (x1 > x0 + 0.1f) {
        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, x0, y);
        nvgLineTo(args.vg, x1, y);
        nvgStrokeColor(args.vg, nvgRGBA(87, 64, 191, 255)); // #5740bf
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
          "Halo", "", [=]() { return scopeModule->debugRenderHaloEnabled; },
          [=]() { scopeModule->debugRenderHaloEnabled = !scopeModule->debugRenderHaloEnabled; }));
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
