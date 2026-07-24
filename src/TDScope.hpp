#pragma once

#include "DebugTerminalTransport.hpp"
#include "PanelSvgUtils.hpp"
#include "TemporalDeckExpanderProtocol.hpp"
#include "TemporalDeckTest.hpp"
#include "plugin.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <limits>

struct TDScope;

namespace tdscope {

extern std::atomic<uint32_t> gTDScopeDebugInstanceCounter;
constexpr float kScopeDisplayVerticalSupersampleMax = 2.f;

float computeScopeDisplayVerticalSupersample(float rackZoom);
bool isTemporalDeckModule(const engine::Module *neighbor);
Widget *createDisplay(TDScope *module, math::Rect scopeRectMm);
Widget *createInput(TDScope *module, math::Rect scopeRectMm);
Widget *createGlDisplay(TDScope *module, math::Rect scopeRectMm);

} // namespace tdscope

struct TDScope final : Module {
  ModuleTeardownTimer teardownTimer {"TDScope"};

  enum LightId { LINK_LIGHT, PREVIEW_LIGHT, LIGHTS_LEN };
  enum ScopeRangeMode { SCOPE_RANGE_5V = 0, SCOPE_RANGE_10V, SCOPE_RANGE_2V5, SCOPE_RANGE_AUTO, SCOPE_RANGE_COUNT };
  enum ScopeChannelMode { SCOPE_CHANNEL_MONO = 0, SCOPE_CHANNEL_STEREO, SCOPE_CHANNEL_COUNT };
  enum DebugUiPublishRateMode {
    DEBUG_UI_PUBLISH_90HZ = 0,
    DEBUG_UI_PUBLISH_60HZ,
    DEBUG_UI_PUBLISH_30HZ,
    DEBUG_UI_PUBLISH_COUNT
  };
  enum DebugRenderMode {
    DEBUG_RENDER_STANDARD = 0,
    DEBUG_RENDER_TAIL_RASTER,
    DEBUG_RENDER_OPENGL,
    DEBUG_RENDER_OPENGL_SHDR = 7,
    DEBUG_RENDER_COUNT = 8
  };
  enum ColorScheme {
    COLOR_SCHEME_DEFAULT = 0,
    COLOR_SCHEME_CLASSIC,
    COLOR_SCHEME_MONOCHROME,
    COLOR_SCHEME_FIRE,
    COLOR_SCHEME_AMBER,
    COLOR_SCHEME_GREEN_PHOSPHOR,
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
  std::atomic<float> uiDebugModuleUiDrawUsEma {0.f};
  std::atomic<float> uiDebugModuleUiStepUsEma {0.f};
  std::atomic<float> uiDebugScopeDensityPct {100.f};
  std::atomic<int> uiDebugScopeDensityRows {0};
  std::atomic<uint64_t> perfAudioProcessMinNs {std::numeric_limits<uint64_t>::max()};
  std::atomic<uint64_t> perfAudioProcessMaxNs {0};
  uint32_t debugInstanceId = 0u;
  double uiDebugTerminalLastSubmitSec = -1.0;
  float uiPublishTimerSec = 0.f;
  float invalidMessageTimerSec = 1e9f;
  float invalidPreviewTimerSec = 1e9f;
  uint64_t lastPublishSeq = 0;
  int staleFrames = 0;
  bool previewValid = false;
  std::atomic<int> scopeDisplayRangeMode {SCOPE_RANGE_AUTO};
  std::atomic<bool> scopeVerticalInverted {false};
  std::atomic<int> scopeChannelMode {SCOPE_CHANNEL_MONO};
  std::atomic<int> scopeColorScheme {COLOR_SCHEME_DEFAULT};
  float scopeColorBrightness = 0.5f;
  std::atomic<bool> debugUseGlShaderRenderer {true};
  std::atomic<bool> debugFramebufferCacheEnabled {true};
  std::atomic<int> debugRenderMode {DEBUG_RENDER_OPENGL_SHDR};
  std::atomic<int> debugUiPublishRateMode {DEBUG_UI_PUBLISH_90HZ};
  float requestPublishTimerSec = 0.f;
  uint64_t requestSeq = 0u;
  uint32_t lastRequestedScopeFormat = uint32_t(-1);
  bool lastLagDragActive = false;
  bool lastLagDragStationaryHold = false;
  uint32_t lastLagDragPhase = temporaldeck_expander::LAG_DRAG_PHASE_INACTIVE;
  float lastLagDragSamples = 0.f;
  float lastLagDragVelocity = 0.f;
  std::atomic<bool> uiLagDragActive {false};
  std::atomic<bool> uiLagDragStationaryHold {false};
  std::atomic<uint32_t> uiLagDragPhase {temporaldeck_expander::LAG_DRAG_PHASE_INACTIVE};
  std::atomic<float> uiLagDragSamples {0.f};
  std::atomic<float> uiLagDragVelocity {0.f};
  std::atomic<uint32_t> uiLagDragSeq {0u};

  static constexpr float kUiPublishIntervalSec = 1.f / 90.f;
  static constexpr float kRequestPublishIntervalSec = 1.f / 30.f;
  static constexpr float kRequestPublishIntervalDragSec = 1.f / 120.f;
  // Temporal Deck can intentionally throttle expander preview publishes as low
  // as 20 Hz in frozen live idle. Keep link/preview validity latched long
  // enough to cover that cadence plus modest scheduling jitter.
  static constexpr float kLinkDropGraceSec = 0.125f;
  static constexpr float kPreviewDropGraceSec = 0.125f;

  static int normalizeColorSchemeIndex(int raw) {
    // Preserve older patch values by mapping legacy scheme ids.
    switch (raw) {
      case 0: // Temporal Deck
      case 1: // Leviathan
      case 5: // Violet Flame
      case 6: // Pixie
        return COLOR_SCHEME_DEFAULT;
      case 2: // Pickle
      case 8: // Emerald
        return COLOR_SCHEME_CLASSIC;
      case 4: // Angelic
        return COLOR_SCHEME_MONOCHROME;
      case 3: // Hellfire
      case 7: // Wasp
        return COLOR_SCHEME_FIRE;
      default:
        return clamp(raw, COLOR_SCHEME_DEFAULT, COLOR_SCHEME_COUNT - 1);
    }
  }

  TDScope() {
    config(0, 0, 0, LIGHTS_LEN);
    debugInstanceId = tdscope::gTDScopeDebugInstanceCounter.fetch_add(1u, std::memory_order_relaxed);
    // Expander contract (TD.Scope side):
    // - Host-to-display stream (`HostToDisplay`) is owned/published by TemporalDeck
    //   via TemporalDeck::rightExpander.producerMessage and consumed here via
    //   this module's leftExpander.consumerMessage.
    // - Display-to-host requests (`DisplayToHost`) are published here by writing
    //   into TemporalDeck's rightExpander.producerMessage (reached through
    //   leftExpander.module), and TemporalDeck consumes via rightExpander.consumerMessage.
    leftExpander.producerMessage = &leftMessages[0];
    leftExpander.consumerMessage = &leftMessages[1];
    uiSnapshots[0] = temporaldeck_expander::HostToDisplay();
    uiSnapshots[1] = temporaldeck_expander::HostToDisplay();
  }

  ~TDScope() override {
    teardownTimer.begin(id);
  }

  float scopeDisplayFullScaleVolts() const {
    const int rangeMode = scopeDisplayRangeMode.load(std::memory_order_relaxed);
    switch (rangeMode) {
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
    const int publishRateMode = debugUiPublishRateMode.load(std::memory_order_relaxed);
    switch (publishRateMode) {
      case DEBUG_UI_PUBLISH_30HZ:
        return 1.f / 30.f;
      case DEBUG_UI_PUBLISH_60HZ:
        return 1.f / 60.f;
      case DEBUG_UI_PUBLISH_90HZ:
      default:
        return kUiPublishIntervalSec;
    }
  }

  bool useTailRasterRenderMode() const {
    return debugRenderMode.load(std::memory_order_relaxed) == DEBUG_RENDER_TAIL_RASTER;
  }

  bool useGeometryHistoryRenderMode() const {
    int mode = debugRenderMode.load(std::memory_order_relaxed);
    return mode == DEBUG_RENDER_OPENGL || mode == DEBUG_RENDER_OPENGL_SHDR;
  }

  bool useOpenGlGeometryRenderMode() const {
    int mode = debugRenderMode.load(std::memory_order_relaxed);
    return mode == DEBUG_RENDER_OPENGL || mode == DEBUG_RENDER_OPENGL_SHDR;
  }

  bool useOpenGlShaderRenderMode() const {
    return debugRenderMode.load(std::memory_order_relaxed) == DEBUG_RENDER_OPENGL_SHDR;
  }

  float scopeColorBrightnessClamped() const {
    return clamp(scopeColorBrightness, 0.f, 1.f);
  }

  float scopeColorBrightnessScale() const {
    float brightness = scopeColorBrightnessClamped();
    if (brightness <= 0.5f) {
      return rescale(brightness, 0.f, 0.5f, 0.35f, 1.f);
    }
    return 1.f;
  }

  float scopeColorBrightnessLift() const {
    float brightness = scopeColorBrightnessClamped();
    if (brightness <= 0.5f) {
      return 0.f;
    }
    return rescale(brightness, 0.5f, 1.f, 0.f, 0.42f);
  }

  NVGcolor applyScopeColorBrightness(NVGcolor c) const {
    float colorScale = scopeColorBrightnessScale();
    float colorLift = scopeColorBrightnessLift();
    c.r = clamp(c.r * colorScale, 0.f, 1.f);
    c.g = clamp(c.g * colorScale, 0.f, 1.f);
    c.b = clamp(c.b * colorScale, 0.f, 1.f);
    c.r = c.r + (1.f - c.r) * colorLift;
    c.g = c.g + (1.f - c.g) * colorLift;
    c.b = c.b + (1.f - c.b) * colorLift;
    return c;
  }

  json_t *dataToJson() override {
    json_t *root = json_object();
    json_object_set_new(root, "scopeDisplayRangeMode", json_integer(scopeDisplayRangeMode.load(std::memory_order_relaxed)));
    json_object_set_new(root, "scopeVerticalInverted", json_boolean(scopeVerticalInverted.load(std::memory_order_relaxed)));
    json_object_set_new(root, "scopeChannelMode", json_integer(scopeChannelMode.load(std::memory_order_relaxed)));
    json_object_set_new(root, "scopeColorScheme", json_integer(scopeColorScheme.load(std::memory_order_relaxed)));
    json_object_set_new(root, "scopeColorSchemeVersion", json_integer(2));
    json_object_set_new(root, "scopeColorBrightness", json_real(scopeColorBrightness));
    json_object_set_new(root, "debugUseGlShaderRenderer", json_boolean(useOpenGlShaderRenderMode()));
    json_object_set_new(root, "debugFramebufferCacheEnabled", json_boolean(debugFramebufferCacheEnabled.load(std::memory_order_relaxed)));
    json_object_set_new(root, "debugRenderMode", json_integer(debugRenderMode.load(std::memory_order_relaxed)));
    json_object_set_new(root, "debugUiPublishRateMode", json_integer(debugUiPublishRateMode.load(std::memory_order_relaxed)));
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
      int rawScheme = int(json_integer_value(schemeJ));
      json_t *schemeVersionJ = json_object_get(root, "scopeColorSchemeVersion");
      int schemeVersion = schemeVersionJ ? int(json_integer_value(schemeVersionJ)) : 1;
      if (schemeVersion >= 2) {
        scopeColorScheme = clamp(rawScheme, COLOR_SCHEME_DEFAULT, COLOR_SCHEME_COUNT - 1);
      } else {
        scopeColorScheme = normalizeColorSchemeIndex(rawScheme);
      }
    }
    json_t *brightnessJ = json_object_get(root, "scopeColorBrightness");
    if (brightnessJ) {
      scopeColorBrightness = clamp(float(json_number_value(brightnessJ)), 0.f, 1.f);
    }
    bool legacyShaderRenderer = true;
    json_t *glShaderRendererJ = json_object_get(root, "debugUseGlShaderRenderer");
    if (glShaderRendererJ) {
      legacyShaderRenderer = json_boolean_value(glShaderRendererJ);
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
        case 6: debugRenderMode = DEBUG_RENDER_OPENGL_SHDR; break;
        case 7: debugRenderMode = DEBUG_RENDER_OPENGL_SHDR; break;
        default:
          debugRenderMode =
            (rawRenderMode >= DEBUG_RENDER_STANDARD && rawRenderMode <= DEBUG_RENDER_OPENGL_SHDR)
              ? rawRenderMode
              : DEBUG_RENDER_STANDARD;
          break;
      }
      loadedRenderMode = true;
      // Legacy serialized mode `2` represented OpenGL with a separate shader
      // boolean flag; preserve that intent when present.
      if (rawRenderMode == 2 && glShaderRendererJ) {
        debugRenderMode = legacyShaderRenderer ? DEBUG_RENDER_OPENGL_SHDR : DEBUG_RENDER_OPENGL;
      }
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
      if (legacyGlGeometry) {
        debugRenderMode = legacyShaderRenderer ? DEBUG_RENDER_OPENGL_SHDR : DEBUG_RENDER_OPENGL;
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
    debugUseGlShaderRenderer.store(useOpenGlShaderRenderMode(), std::memory_order_relaxed);
    json_t *publishRateJ = json_object_get(root, "debugUiPublishRateMode");
    if (publishRateJ) {
      debugUiPublishRateMode =
        clamp(int(json_integer_value(publishRateJ)), DEBUG_UI_PUBLISH_90HZ, DEBUG_UI_PUBLISH_COUNT - 1);
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
    // Bounded retries reduce chance of tearing between front-index and payload
    // under concurrent publish, while keeping UI reads deterministic and cheap.
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

  void setLagDragRequest(bool active, float normalizedOffset, float normalizedVelocity = 0.f, bool stationaryHold = false,
                         uint32_t phase = temporaldeck_expander::LAG_DRAG_PHASE_INACTIVE) {
    phase = temporaldeck_expander::normalizeLagDragPhase(phase);
    if (!active) {
      phase = temporaldeck_expander::LAG_DRAG_PHASE_INACTIVE;
    } else if (phase == temporaldeck_expander::LAG_DRAG_PHASE_INACTIVE) {
      phase = temporaldeck_expander::lagDragPhaseFromFlags(active, stationaryHold);
    }
    stationaryHold = phase == temporaldeck_expander::LAG_DRAG_PHASE_HOLD;
    // Publish drag request as a coherent snapshot for process-thread reads.
    uiLagDragSeq.fetch_add(1u, std::memory_order_release); // begin write (odd)
    uiLagDragActive.store(active, std::memory_order_relaxed);
    uiLagDragStationaryHold.store(active && stationaryHold, std::memory_order_relaxed);
    uiLagDragPhase.store(phase, std::memory_order_relaxed);
    uiLagDragSamples.store(normalizedOffset, std::memory_order_relaxed);
    uiLagDragVelocity.store(normalizedVelocity, std::memory_order_relaxed);
    uiLagDragSeq.fetch_add(1u, std::memory_order_release); // end write (even)
  }

  void process(const ProcessArgs &args) override {
    const bool measurePerf = isDragonKingDebugEnabled();
    const auto processStart = measurePerf ? std::chrono::steady_clock::now() : std::chrono::steady_clock::time_point();
    bool validMessage = false;
    bool previewValidNow = false;
    const temporaldeck_expander::HostToDisplay *latestMsg = nullptr;
    if (tdscope::isTemporalDeckModule(leftExpander.module) && leftExpander.consumerMessage) {
      const auto *msg = reinterpret_cast<const temporaldeck_expander::HostToDisplay *>(leftExpander.consumerMessage);
      if (msg->magic == temporaldeck_expander::MAGIC && msg->version == temporaldeck_expander::VERSION &&
          msg->size == sizeof(temporaldeck_expander::HostToDisplay)) {
        validMessage = true;
        latestMsg = msg;
        if ((msg->flags & temporaldeck_expander::FLAG_SCOPE_ATTACH_CHANNEL_SYNC) != 0u) {
          bool dualConnected = (msg->flags & temporaldeck_expander::FLAG_SCOPE_INPUTS_DUAL_CONNECTED) != 0u;
          scopeChannelMode.store(dualConnected ? SCOPE_CHANNEL_STEREO : SCOPE_CHANNEL_MONO, std::memory_order_relaxed);
        }
        if ((msg->flags & temporaldeck_expander::FLAG_SCOPE_AUTO_PROMOTE_STEREO) != 0u &&
            scopeChannelMode.load(std::memory_order_relaxed) == SCOPE_CHANNEL_MONO) {
          scopeChannelMode.store(SCOPE_CHANNEL_STEREO, std::memory_order_relaxed);
        }
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

    Module *left = leftExpander.module;
    bool hasTemporalDeckNeighbor = tdscope::isTemporalDeckModule(left);
    const int staleFrameLimit =
      std::max(2048, int(std::ceil(std::max(args.sampleRate, 1.f) * kLinkDropGraceSec)));
    bool linkActive = hasTemporalDeckNeighbor && staleFrames < staleFrameLimit && invalidMessageTimerSec <= kLinkDropGraceSec;
    bool previewVisible = invalidPreviewTimerSec <= kPreviewDropGraceSec;
    uiLinkActive.store(linkActive, std::memory_order_relaxed);
    uiPreviewValid.store(linkActive && previewVisible, std::memory_order_relaxed);

    if (tdscope::isTemporalDeckModule(left) && left->rightExpander.producerMessage) {
      uint32_t requestedScopeFormat = (scopeChannelMode == SCOPE_CHANNEL_STEREO)
                                        ? temporaldeck_expander::SCOPE_FORMAT_STEREO
                                        : temporaldeck_expander::SCOPE_FORMAT_MONO;
      bool lagDragActive = false;
      bool lagDragStationaryHold = false;
      uint32_t lagDragPhase = temporaldeck_expander::LAG_DRAG_PHASE_INACTIVE;
      float lagDragSamples = 0.f;
      float lagDragVelocity = 0.f;
      bool lagDragSnapshotRead = false;
      // Bounded retries keep read cheap while avoiding mixed-frame field reads.
      for (int i = 0; i < 3; ++i) {
        uint32_t seq0 = uiLagDragSeq.load(std::memory_order_acquire);
        if ((seq0 & 1u) != 0u) {
          continue;
        }
        bool active = uiLagDragActive.load(std::memory_order_relaxed);
        bool stationaryHold = uiLagDragStationaryHold.load(std::memory_order_relaxed);
        uint32_t phase = uiLagDragPhase.load(std::memory_order_relaxed);
        float samples = uiLagDragSamples.load(std::memory_order_relaxed);
        float velocity = uiLagDragVelocity.load(std::memory_order_relaxed);
        uint32_t seq1 = uiLagDragSeq.load(std::memory_order_acquire);
        if (seq0 == seq1 && (seq1 & 1u) == 0u) {
          lagDragActive = active;
          lagDragStationaryHold = stationaryHold;
          lagDragPhase = phase;
          lagDragSamples = samples;
          lagDragVelocity = velocity;
          lagDragSnapshotRead = true;
          break;
        }
      }
      // Fallback preserves prior behavior if write contention prevents a stable
      // sequence snapshot in this sample.
      if (!lagDragSnapshotRead) {
        lagDragActive = uiLagDragActive.load(std::memory_order_relaxed);
        lagDragStationaryHold = uiLagDragStationaryHold.load(std::memory_order_relaxed);
        lagDragPhase = uiLagDragPhase.load(std::memory_order_relaxed);
        lagDragSamples = uiLagDragSamples.load(std::memory_order_relaxed);
        lagDragVelocity = uiLagDragVelocity.load(std::memory_order_relaxed);
      }
      lagDragPhase = temporaldeck_expander::normalizeLagDragPhase(lagDragPhase);
      if (!lagDragActive) {
        lagDragPhase = temporaldeck_expander::LAG_DRAG_PHASE_INACTIVE;
      } else if (!temporaldeck_expander::isLagDragPhaseActive(lagDragPhase)) {
        lagDragPhase = temporaldeck_expander::lagDragPhaseFromFlags(lagDragActive, lagDragStationaryHold);
      }
      lagDragStationaryHold = lagDragPhase == temporaldeck_expander::LAG_DRAG_PHASE_HOLD;
      if (!std::isfinite(lagDragSamples)) {
        lagDragSamples = 0.f;
      }
      if (!std::isfinite(lagDragVelocity)) {
        lagDragVelocity = 0.f;
      }
      float requestIntervalSec = lagDragActive ? kRequestPublishIntervalDragSec : kRequestPublishIntervalSec;
      requestPublishTimerSec += args.sampleTime;
      bool formatChanged = requestedScopeFormat != lastRequestedScopeFormat;
      bool lagStateChanged = lagDragActive != lastLagDragActive;
      bool lagHoldChanged = lagDragStationaryHold != lastLagDragStationaryHold;
      bool lagPhaseChanged = lagDragPhase != lastLagDragPhase;
      bool lagValueChanged = lagDragActive && (std::fabs(lagDragSamples - lastLagDragSamples) >= (1.f / 16.f) ||
                                               std::fabs(lagDragVelocity - lastLagDragVelocity) >= 0.1f);
      bool timerElapsed = requestPublishTimerSec >= requestIntervalSec;
      if (formatChanged || lagStateChanged || lagHoldChanged || lagPhaseChanged || lagValueChanged || timerElapsed) {
        if (timerElapsed) {
          requestPublishTimerSec = std::fmod(requestPublishTimerSec, requestIntervalSec);
        } else {
          requestPublishTimerSec = 0.f;
        }
        auto *request =
          reinterpret_cast<temporaldeck_expander::DisplayToHost *>(left->rightExpander.producerMessage);
        if (request) {
          // Request direction contract:
          // TD.Scope publishes DisplayToHost into TemporalDeck's producer slot,
          // then flips TemporalDeck's right-expander message so TemporalDeck
          // reads it from rightExpander.consumerMessage in its next process().
          requestSeq++;
          temporaldeck_expander::populateDisplayRequest(request, requestSeq, requestedScopeFormat, lagDragActive,
                                                        lagDragStationaryHold, 0.f, 0.f, lagDragSamples, lagDragVelocity,
                                                        lagDragPhase);
          left->rightExpander.messageFlipRequested = true;
          lastRequestedScopeFormat = requestedScopeFormat;
          lastLagDragActive = lagDragActive;
          lastLagDragStationaryHold = lagDragStationaryHold;
          lastLagDragPhase = lagDragPhase;
          lastLagDragSamples = lagDragSamples;
          lastLagDragVelocity = lagDragVelocity;
        }
      }
    } else {
      requestPublishTimerSec = 0.f;
      lastRequestedScopeFormat = uint32_t(-1);
      lastLagDragActive = false;
      lastLagDragStationaryHold = false;
      lastLagDragSamples = 0.f;
      uiLagDragActive.store(false, std::memory_order_relaxed);
      uiLagDragStationaryHold.store(false, std::memory_order_relaxed);
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
    if (measurePerf) {
      const uint64_t elapsedNs = uint64_t(std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now() - processStart).count());
      debug_terminal::recordAudioProcessTiming(perfAudioProcessMinNs, perfAudioProcessMaxNs, elapsedNs);
    }
  }
};
