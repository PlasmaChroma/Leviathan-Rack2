#pragma once

#include "DebugTerminalTransport.hpp"
#include "PanelSvgUtils.hpp"
#include "TemporalDeckExpanderProtocol.hpp"
#include "TemporalDeckTest.hpp"
#include "plugin.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>

struct TDScope;

namespace tdscope {

extern std::atomic<uint32_t> gTDScopeDebugInstanceCounter;
constexpr float kScopeDisplayVerticalSupersampleMax = 2.f;

float computeScopeDisplayVerticalSupersample(float rackZoom);
PanelBorder *findPanelBorder(Widget *widget);
bool isTemporalDeckModule(const engine::Module *neighbor);
Widget *createDisplay(TDScope *module, math::Rect scopeRectMm);
Widget *createInput(TDScope *module, math::Rect scopeRectMm);
Widget *createGlDisplay(TDScope *module, math::Rect scopeRectMm);

} // namespace tdscope

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
  bool scopeVerticalInverted = false;
  int scopeChannelMode = SCOPE_CHANNEL_MONO;
  int scopeColorScheme = COLOR_SCHEME_TEMPORAL_DECK;
  bool scopeTransientHaloEnabled = true;
  bool debugRenderMainTraceEnabled = true;
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

  static constexpr float kUiPublishIntervalSec = 1.f / 120.f;
  static constexpr float kRequestPublishIntervalSec = 1.f / 30.f;
  static constexpr float kRequestPublishIntervalDragSec = 1.f / 120.f;
  static constexpr float kLinkDropGraceSec = 1.f / 45.f;
  static constexpr float kPreviewDropGraceSec = 1.f / 45.f;

  TDScope() {
    config(0, 0, 0, LIGHTS_LEN);
    debugInstanceId = tdscope::gTDScopeDebugInstanceCounter.fetch_add(1u, std::memory_order_relaxed);
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
    json_object_set_new(root, "scopeVerticalInverted", json_boolean(scopeVerticalInverted));
    json_object_set_new(root, "scopeChannelMode", json_integer(scopeChannelMode));
    json_object_set_new(root, "scopeColorScheme", json_integer(scopeColorScheme));
    json_object_set_new(root, "scopeTransientHaloEnabled", json_boolean(scopeTransientHaloEnabled));
    json_object_set_new(root, "debugRenderMainTraceEnabled", json_boolean(debugRenderMainTraceEnabled));
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
    json_t *verticalInvertJ = json_object_get(root, "scopeVerticalInverted");
    if (verticalInvertJ) {
      scopeVerticalInverted = json_boolean_value(verticalInvertJ);
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
    uiLagDragActive.store(active, std::memory_order_relaxed);
    uiLagDragSamples.store(std::max(0.f, lagSamples), std::memory_order_relaxed);
    uiLagDragVelocity.store(velocity, std::memory_order_relaxed);
  }

  void process(const ProcessArgs &args) override {
    bool validMessage = false;
    bool previewValidNow = false;
    const temporaldeck_expander::HostToDisplay *latestMsg = nullptr;
    if (tdscope::isTemporalDeckModule(leftExpander.module) && leftExpander.consumerMessage) {
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

    bool hasTemporalDeckNeighbor = tdscope::isTemporalDeckModule(leftExpander.module);
    bool linkActive = hasTemporalDeckNeighbor && staleFrames < 2048 && invalidMessageTimerSec <= kLinkDropGraceSec;
    bool previewVisible = invalidPreviewTimerSec <= kPreviewDropGraceSec;
    uiLinkActive.store(linkActive, std::memory_order_relaxed);
    uiPreviewValid.store(linkActive && previewVisible, std::memory_order_relaxed);

    if (tdscope::isTemporalDeckModule(leftExpander.module) && leftExpander.module->rightExpander.producerMessage) {
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
