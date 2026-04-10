#include "TemporalDeckExpanderProtocol.hpp"
#include "PanelSvgUtils.hpp"
#include "plugin.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <limits>
#include <vector>

namespace {

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
  temporaldeck_expander::HostToDisplay uiSnapshot;
  std::atomic<uint32_t> uiSnapshotSeq {0};
  std::atomic<bool> uiLinkActive {false};
  std::atomic<bool> uiPreviewValid {false};
  std::atomic<uint64_t> uiLastPublishSeq {0};
  float uiPublishTimerSec = 0.f;
  uint64_t lastPublishSeq = 0;
  int staleFrames = 0;
  bool previewValid = false;
  int scopeDisplayRangeMode = SCOPE_RANGE_5V;
  int scopeChannelMode = SCOPE_CHANNEL_MONO;
  int scopeColorScheme = COLOR_SCHEME_TEMPORAL_DECK;
  float requestPublishTimerSec = 0.f;
  uint64_t requestSeq = 0u;
  uint32_t lastRequestedScopeFormat = uint32_t(-1);

  static constexpr float kUiPublishIntervalSec = 1.f / 60.f;
  static constexpr float kRequestPublishIntervalSec = 1.f / 30.f;

  TDScope() {
    config(0, 0, 0, LIGHTS_LEN);
    leftExpander.producerMessage = &leftMessages[0];
    leftExpander.consumerMessage = &leftMessages[1];
    uiSnapshot = temporaldeck_expander::HostToDisplay();
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

  json_t *dataToJson() override {
    json_t *root = json_object();
    json_object_set_new(root, "scopeDisplayRangeMode", json_integer(scopeDisplayRangeMode));
    json_object_set_new(root, "scopeChannelMode", json_integer(scopeChannelMode));
    json_object_set_new(root, "scopeColorScheme", json_integer(scopeColorScheme));
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
  }

  void publishSnapshotToUi(const temporaldeck_expander::HostToDisplay &msg) {
    uint32_t seq = uiSnapshotSeq.load(std::memory_order_relaxed);
    uiSnapshotSeq.store(seq + 1u, std::memory_order_release); // writer active (odd)
    uiSnapshot = msg;
    uiSnapshotSeq.store(seq + 2u, std::memory_order_release); // writer done (even)
    uiLastPublishSeq.store(msg.publishSeq, std::memory_order_release);
  }

  bool readSnapshotForUi(temporaldeck_expander::HostToDisplay *out) const {
    if (!out) {
      return false;
    }
    for (int i = 0; i < 3; ++i) {
      uint32_t seq0 = uiSnapshotSeq.load(std::memory_order_acquire);
      if ((seq0 & 1u) != 0u) {
        continue;
      }
      *out = uiSnapshot;
      uint32_t seq1 = uiSnapshotSeq.load(std::memory_order_acquire);
      if (seq0 == seq1 && (seq1 & 1u) == 0u) {
        return out->magic == temporaldeck_expander::MAGIC &&
               out->version == temporaldeck_expander::VERSION &&
               out->size == sizeof(temporaldeck_expander::HostToDisplay);
      }
    }
    return false;
  }

  void process(const ProcessArgs &args) override {
    bool validMessage = false;
    previewValid = false;
    const temporaldeck_expander::HostToDisplay *latestMsg = nullptr;
    if (leftExpander.module && leftExpander.consumerMessage) {
      const auto *msg = reinterpret_cast<const temporaldeck_expander::HostToDisplay *>(leftExpander.consumerMessage);
      if (msg->magic == temporaldeck_expander::MAGIC && msg->version == temporaldeck_expander::VERSION &&
          msg->size == sizeof(temporaldeck_expander::HostToDisplay)) {
        validMessage = true;
        latestMsg = msg;
        previewValid = (msg->flags & temporaldeck_expander::FLAG_PREVIEW_VALID) != 0u;
        if (msg->publishSeq != lastPublishSeq) {
          lastPublishSeq = msg->publishSeq;
          staleFrames = 0;
        } else {
          staleFrames++;
        }
      }
    }

    bool linkActive = validMessage && staleFrames < 2048;
    uiLinkActive.store(linkActive, std::memory_order_relaxed);
    uiPreviewValid.store(linkActive && previewValid, std::memory_order_relaxed);

    // Publish requested scope payload format back to TemporalDeck.
    if (isTemporalDeckModule(leftExpander.module) && leftExpander.module->rightExpander.producerMessage) {
      uint32_t requestedScopeFormat = (scopeChannelMode == SCOPE_CHANNEL_STEREO)
                                        ? temporaldeck_expander::SCOPE_FORMAT_STEREO
                                        : temporaldeck_expander::SCOPE_FORMAT_MONO;
      requestPublishTimerSec += args.sampleTime;
      bool formatChanged = requestedScopeFormat != lastRequestedScopeFormat;
      bool timerElapsed = requestPublishTimerSec >= kRequestPublishIntervalSec;
      if (formatChanged || timerElapsed) {
        if (timerElapsed) {
          requestPublishTimerSec = std::fmod(requestPublishTimerSec, kRequestPublishIntervalSec);
        } else {
          requestPublishTimerSec = 0.f;
        }
        auto *request =
          reinterpret_cast<temporaldeck_expander::DisplayToHost *>(leftExpander.module->rightExpander.producerMessage);
        if (request) {
          requestSeq++;
          temporaldeck_expander::populateDisplayRequest(request, requestSeq, requestedScopeFormat);
          leftExpander.module->rightExpander.messageFlipRequested = true;
          lastRequestedScopeFormat = requestedScopeFormat;
        }
      }
    } else {
      requestPublishTimerSec = 0.f;
      lastRequestedScopeFormat = uint32_t(-1);
    }

    uiPublishTimerSec += args.sampleTime;
    if (linkActive && latestMsg && uiPublishTimerSec >= kUiPublishIntervalSec) {
      uiPublishTimerSec = std::fmod(uiPublishTimerSec, kUiPublishIntervalSec);
      publishSnapshotToUi(*latestMsg);
    }

    bool ready = linkActive && previewValid;
    lights[LINK_LIGHT].setBrightness(linkActive && !ready ? 1.f : 0.f);
    lights[PREVIEW_LIGHT].setBrightness(ready ? 1.f : 0.f);
  }
};

struct TDScopeDisplayWidget final : Widget {
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
  std::vector<float> rowY;
  std::vector<uint8_t> rowValid;
  std::vector<uint8_t> rowValidRight;
  std::vector<uint8_t> rowBucket;
  std::vector<uint8_t> rowBucketRight;
  std::vector<uint8_t> rowHoldFrames;
  std::vector<uint8_t> rowHoldFramesRight;
  uint64_t cachedPublishSeq = 0;
  int cachedRowCount = 0;
  int cachedRangeMode = -1;
  bool cachedStereoLayout = false;
  bool cachedGeometryValid = false;
  std::vector<LiveScopeBucket> liveBucketsLeft;
  std::vector<LiveScopeBucket> liveBucketsRight;
  bool liveBucketsInitialized = false;
  bool liveBucketsStereo = false;
  int liveBucketCount = 0;
  float liveBucketSpanSamples = 0.f;
  uint64_t liveBucketLastPublishSeq = 0;

  void draw(const DrawArgs &args) override {
    bool linkActive = module && module->uiLinkActive.load(std::memory_order_relaxed);
    bool previewValid = module && module->uiPreviewValid.load(std::memory_order_relaxed);

    if (!module) {
      return;
    }

    temporaldeck_expander::HostToDisplay msg;
    if (!module->readSnapshotForUi(&msg) || !linkActive || !previewValid) {
      return;
    }

    uint32_t scopeBinCount = std::min(msg.scopeBinCount, temporaldeck_expander::SCOPE_BIN_COUNT);
    if (scopeBinCount == 0u) {
      return;
    }
    bool hostStereoPayload = (msg.flags & temporaldeck_expander::FLAG_SCOPE_STEREO) != 0u;
    bool renderStereo = (module->scopeChannelMode == TDScope::SCOPE_CHANNEL_STEREO) && hostStereoPayload;

    int peakQAbs = 0;
    for (uint32_t i = 0; i < scopeBinCount; ++i) {
      const temporaldeck_expander::ScopeBin &bin = msg.scope[i];
      if (!temporaldeck_expander::isScopeBinValid(bin)) {
        continue;
      }
      peakQAbs = std::max(peakQAbs, int(std::abs(int(bin.min))));
      peakQAbs = std::max(peakQAbs, int(std::abs(int(bin.max))));
      if (renderStereo) {
        const temporaldeck_expander::ScopeBin &binR = msg.scopeRight[i];
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
          const temporaldeck_expander::ScopeBin &bin = msg.scope[i];
          if (!temporaldeck_expander::isScopeBinValid(bin)) {
            // fall through to right channel in stereo mode
          } else {
            int peakQ = std::max(std::abs(int(bin.min)), std::abs(int(bin.max)));
            float peakV = (float(peakQ) / 32767.f) * temporaldeck_expander::kPreviewQuantizeVolts;
            peaks.push_back(clamp(peakV, 0.f, temporaldeck_expander::kPreviewQuantizeVolts));
          }
          if (renderStereo) {
            const temporaldeck_expander::ScopeBin &binR = msg.scopeRight[i];
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
    if (lowSignalWindow) {
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
    float scopeBinSpanSamples = std::max(msg.scopeBinSpanSamples, 1e-6f);
    const int rowCount = std::max(1, int(std::ceil(drawHeight)));
    constexpr int kIntensityBuckets = 16;
    size_t rowCountU = size_t(rowCount);
    if (rowX0.size() != rowCountU) {
      rowX0.assign(rowCountU, lane0CenterX);
      rowX1.assign(rowCountU, lane0CenterX);
      rowX0Right.assign(rowCountU, lane1CenterX);
      rowX1Right.assign(rowCountU, lane1CenterX);
      rowVisualIntensity.assign(rowCountU, 0.f);
      rowVisualIntensityRight.assign(rowCountU, 0.f);
      rowY.assign(rowCountU, 0.f);
      rowValid.assign(rowCountU, 0u);
      rowValidRight.assign(rowCountU, 0u);
      rowBucket.assign(rowCountU, 0u);
      rowBucketRight.assign(rowCountU, 0u);
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
      ingestLiveLane(msg.scope, &liveBucketsLeft);
      if (renderStereo) {
        ingestLiveLane(msg.scopeRight, &liveBucketsRight);
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
          std::vector<uint8_t> *bucketOut, std::vector<uint8_t> *holdOut) {
      if (!x0Out || !x1Out || !visualOut || !validOut || !bucketOut || !holdOut) {
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
        float y = drawTop + float(iy) + 0.5f;
        rowY[idx] = y;
        float t0 = clamp((y - drawTop - 0.5f) / yDen, 0.f, 1.f);
        float t1 = clamp((y - drawTop + 0.5f) / yDen, 0.f, 1.f);
        float rowMinNorm = 0.f;
        float rowMaxNorm = 0.f;
        if (!sampleEnvelopeOverInterval(scopeData, t0, t1, &rowMinNorm, &rowMaxNorm)) {
          if (prevValid && prevHold > 0u) {
            (*x0Out)[idx] = prevX0;
            (*x1Out)[idx] = prevX1;
            float carryVisual = clamp(prevVisual * kRowTailIntensityDecay, 0.f, 1.f);
            int bucket = int(std::floor(carryVisual * float(kIntensityBuckets)));
            bucket = clamp(bucket, 0, kIntensityBuckets - 1);
            (*visualOut)[idx] = carryVisual;
            (*bucketOut)[idx] = uint8_t(bucket);
            (*validOut)[idx] = 1u;
            (*holdOut)[idx] = uint8_t(prevHold - 1u);
          } else {
            (*x0Out)[idx] = laneCenterXLocal;
            (*x1Out)[idx] = laneCenterXLocal;
            (*visualOut)[idx] = 0.f;
            (*bucketOut)[idx] = 0u;
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
        int bucket = int(std::floor(visualIntensity * float(kIntensityBuckets)));
        bucket = clamp(bucket, 0, kIntensityBuckets - 1);
        (*visualOut)[idx] = visualIntensity;
        (*bucketOut)[idx] = uint8_t(bucket);
        (*validOut)[idx] = 1u;
        (*holdOut)[idx] = kRowTailHoldFrames;
      }
    };

    auto rebuildLaneFromLiveBuckets =
      [&](const std::vector<LiveScopeBucket> *buckets, float laneCenterXLocal, float laneHalfWidthLocal,
          std::vector<float> *x0Out, std::vector<float> *x1Out, std::vector<float> *visualOut, std::vector<uint8_t> *validOut,
          std::vector<uint8_t> *bucketOut, std::vector<uint8_t> *holdOut) {
        if (!buckets || !x0Out || !x1Out || !visualOut || !validOut || !bucketOut || !holdOut || buckets->size() != rowCountU ||
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

          float y = drawTop + float(iy) + 0.5f;
          rowY[idx] = y;
          float tMid = clamp((y - drawTop) / yDen, 0.f, 1.f);
          float lagMid = windowTopLag + (windowBottomLag - windowTopLag) * tMid;
          int64_t bucketIndex = int64_t(std::floor(lagMid / liveBucketSpanSamples));
          int slot = bucketSlotForIndex(bucketIndex);
          const LiveScopeBucket &bucket = (*buckets)[size_t(slot)];

          if (!(bucket.key == bucketIndex && bucket.hasData)) {
            if (prevValid && prevHold > 0u) {
              (*x0Out)[idx] = prevX0;
              (*x1Out)[idx] = prevX1;
              float carryVisual = clamp(prevVisual * kGapIntensityDecay, 0.f, 1.f);
              int intensityBucket = int(std::floor(carryVisual * float(kIntensityBuckets)));
              intensityBucket = clamp(intensityBucket, 0, kIntensityBuckets - 1);
              (*visualOut)[idx] = carryVisual;
              (*bucketOut)[idx] = uint8_t(intensityBucket);
              (*validOut)[idx] = 1u;
              (*holdOut)[idx] = uint8_t(prevHold - 1u);
            } else {
              (*x0Out)[idx] = laneCenterXLocal;
              (*x1Out)[idx] = laneCenterXLocal;
              (*visualOut)[idx] = 0.f;
              (*bucketOut)[idx] = 0u;
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
          float visualIntensity = clamp(std::pow(intensity, kIntensityGamma) * 1.06f * fillFraction, 0.f, 1.f);
          int intensityBucket = int(std::floor(visualIntensity * float(kIntensityBuckets)));
          intensityBucket = clamp(intensityBucket, 0, kIntensityBuckets - 1);
          (*visualOut)[idx] = visualIntensity;
          (*bucketOut)[idx] = uint8_t(intensityBucket);
          (*validOut)[idx] = 1u;
          (*holdOut)[idx] = kGapHoldFrames;
        }
      };

    if (shouldRebuild) {
      if (liveMode) {
        rebuildLaneFromLiveBuckets(
          &liveBucketsLeft, lane0CenterX, laneAmpHalfWidth, &rowX0, &rowX1, &rowVisualIntensity, &rowValid, &rowBucket,
          &rowHoldFrames);
        if (renderStereo) {
          rebuildLaneFromLiveBuckets(
            &liveBucketsRight, lane1CenterX, laneAmpHalfWidth, &rowX0Right, &rowX1Right, &rowVisualIntensityRight, &rowValidRight,
            &rowBucketRight, &rowHoldFramesRight);
        } else {
          std::fill(rowValidRight.begin(), rowValidRight.end(), 0u);
          std::fill(rowVisualIntensityRight.begin(), rowVisualIntensityRight.end(), 0.f);
          std::fill(rowHoldFramesRight.begin(), rowHoldFramesRight.end(), 0u);
          for (size_t idx = 0; idx < rowCountU; ++idx) {
            rowX0Right[idx] = lane1CenterX;
            rowX1Right[idx] = lane1CenterX;
            rowBucketRight[idx] = 0u;
          }
        }
      } else {
        rebuildLaneFromScopeBins(
          msg.scope, lane0CenterX, laneAmpHalfWidth, &rowX0, &rowX1, &rowVisualIntensity, &rowValid, &rowBucket,
          &rowHoldFrames);
        if (renderStereo) {
          rebuildLaneFromScopeBins(
            msg.scopeRight, lane1CenterX, laneAmpHalfWidth, &rowX0Right, &rowX1Right, &rowVisualIntensityRight, &rowValidRight,
            &rowBucketRight, &rowHoldFramesRight);
        } else {
          std::fill(rowValidRight.begin(), rowValidRight.end(), 0u);
          std::fill(rowVisualIntensityRight.begin(), rowVisualIntensityRight.end(), 0.f);
          std::fill(rowHoldFramesRight.begin(), rowHoldFramesRight.end(), 0u);
          for (size_t idx = 0; idx < rowCountU; ++idx) {
            rowX0Right[idx] = lane1CenterX;
            rowX1Right[idx] = lane1CenterX;
            rowBucketRight[idx] = 0u;
          }
        }
      }
      cachedPublishSeq = msg.publishSeq;
      cachedRowCount = rowCount;
      cachedRangeMode = module->scopeDisplayRangeMode;
      cachedStereoLayout = renderStereo;
      cachedGeometryValid = true;
    }

    auto gradientColorForIntensity = [&](float intensity, uint8_t alpha) -> NVGcolor {
      intensity = clamp(intensity, 0.f, 1.f);
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
      switch (module->scopeColorScheme) {
        case TDScope::COLOR_SCHEME_EMERALD:
          // Emerald: low -> deep forest green, mid -> jade, high -> mint.
          lowR = 15.f;   // #0f4f36
          lowG = 79.f;
          lowB = 54.f;
          midR = 47.f;   // #2fa86e
          midG = 168.f;
          midB = 110.f;
          highR = 87.f;  // #57f0b6
          highG = 240.f;
          highB = 182.f;
          midPoint = 0.52f;
          break;
        case TDScope::COLOR_SCHEME_WASP:
          // Wasp: low -> amber shadow, mid -> warning orange, high -> yellow.
          lowR = 33.f;   // #211b12
          lowG = 27.f;
          lowB = 18.f;
          midR = 231.f;  // #e7892f
          midG = 137.f;
          midB = 47.f;
          highR = 255.f; // #ffd84a
          highG = 216.f;
          highB = 74.f;
          midPoint = 0.58f;
          break;
        case TDScope::COLOR_SCHEME_PIXIE:
          // Pixie: low -> candy pink, mid -> fairy lavender, high -> mint.
          lowR = 255.f;  // #ff8fd1
          lowG = 143.f;
          lowB = 209.f;
          midR = 211.f;  // #d3b4ef
          midG = 180.f;
          midB = 239.f;
          highR = 129.f; // #81ffd2
          highG = 255.f;
          highB = 210.f;
          midPoint = 0.48f;
          break;
        case TDScope::COLOR_SCHEME_VIOLET_FLAME:
          // Violet Flame: low -> indigo, mid -> flame magenta, high -> violet.
          lowR = 42.f;   // #2a1f5f
          lowG = 31.f;
          lowB = 95.f;
          midR = 147.f;  // #9347d9
          midG = 71.f;
          midB = 217.f;
          highR = 181.f; // #b56dff
          highG = 109.f;
          highB = 255.f;
          midPoint = 0.54f;
          break;
        case TDScope::COLOR_SCHEME_ANGELIC:
          // Angelic: low -> pearl, mid -> halo lavender, high -> sky blue.
          lowR = 248.f;  // #f8f5ff
          lowG = 245.f;
          lowB = 255.f;
          midR = 232.f;  // #e8dcff
          midG = 220.f;
          midB = 255.f;
          highR = 179.f; // #b3e5ff
          highG = 229.f;
          highB = 255.f;
          midPoint = 0.40f;
          break;
        case TDScope::COLOR_SCHEME_HELLFIRE:
          // Hellfire: low -> lava red, mid -> inferno orange, high -> ember.
          lowR = 120.f;  // #78180f
          lowG = 24.f;
          lowB = 15.f;
          midR = 255.f;  // #ff6f2b
          midG = 111.f;
          midB = 43.f;
          highR = 255.f; // #ffd166
          highG = 209.f;
          highB = 102.f;
          midPoint = 0.60f;
          break;
        case TDScope::COLOR_SCHEME_PICKLE:
          // Pickle: low -> dill green, mid -> olive brine, high -> chartreuse.
          lowR = 62.f;   // #3e6f31
          lowG = 111.f;
          lowB = 49.f;
          midR = 132.f;  // #84b948
          midG = 185.f;
          midB = 72.f;
          highR = 190.f; // #beea61
          highG = 234.f;
          highB = 97.f;
          midPoint = 0.56f;
          break;
        case TDScope::COLOR_SCHEME_LEVIATHAN:
          // Leviathan: low -> purple, mid -> vivid blue, high -> cyan.
          lowR = 122.f;
          lowG = 92.f;
          lowB = 255.f;
          midR = 75.f;   // #4b8dff
          midG = 141.f;
          midB = 255.f;
          highR = 28.f;
          highG = 204.f;
          highB = 217.f;
          midPoint = 0.52f;
          break;
        case TDScope::COLOR_SCHEME_TEMPORAL_DECK:
        default:
          // Temporal Deck: low cyan -> mid ember yellow -> high magenta.
          lowR = 85.f;
          lowG = 227.f;
          lowB = 238.f;
          midR = 255.f;  // #ffbf56
          midG = 191.f;
          midB = 86.f;
          highR = 233.f;
          highG = 112.f;
          highB = 218.f;
          midPoint = 0.5f;
          // No midpoint hold: keep color flow continuous through the center.
          midHoldHalfWidth = 0.f;
          break;
      }
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
      return nvgRGBA(rq, gq, bq, alpha);
    };

    std::array<float, kIntensityBuckets> mainWidth {};
    std::array<float, kIntensityBuckets> connectWidth {};
    std::array<float, kIntensityBuckets> boostWidth {};
    std::array<NVGcolor, kIntensityBuckets> mainColor {};
    std::array<NVGcolor, kIntensityBuckets> connectColor {};
    std::array<NVGcolor, kIntensityBuckets> boostColor {};
    std::array<uint8_t, kIntensityBuckets> bucketHasBoost {};
    for (int bucket = 0; bucket < kIntensityBuckets; ++bucket) {
      float visual = (float(bucket) + 0.5f) / float(kIntensityBuckets);
      mainWidth[size_t(bucket)] = 0.78f + 0.62f * visual;
      connectWidth[size_t(bucket)] = 0.58f + 0.40f * visual;
      mainColor[size_t(bucket)] =
        gradientColorForIntensity(visual, uint8_t(std::lround(122.f + 120.f * visual)));
      connectColor[size_t(bucket)] =
        gradientColorForIntensity(visual, uint8_t(std::lround(88.f + 92.f * visual)));
      if (visual > 0.86f) {
        float boostT = clamp((visual - 0.86f) / 0.14f, 0.f, 1.f);
        boostColor[size_t(bucket)] = gradientColorForIntensity(1.f, uint8_t(std::lround(52.f + 108.f * boostT)));
        boostWidth[size_t(bucket)] = mainWidth[size_t(bucket)] + 0.34f;
        bucketHasBoost[size_t(bucket)] = 1u;
      }
    }

    nvgSave(args.vg);
    nvgScissor(args.vg, 0.f, drawTop, box.size.x, drawBottom - drawTop);

    auto drawLane = [&](const std::vector<float> &x0, const std::vector<float> &x1, const std::vector<float> &visualIntensity,
                        const std::vector<uint8_t> &valid, const std::vector<uint8_t> &bucketByRow, float laneCenterXForConnectors) {
      // Batch horizontal bars by quantized intensity bucket.
      for (int bucket = 0; bucket < kIntensityBuckets; ++bucket) {
        bool havePath = false;
        nvgBeginPath(args.vg);
        for (int iy = 0; iy < rowCount; ++iy) {
          size_t idx = size_t(iy);
          if (!valid[idx] || bucketByRow[idx] != uint8_t(bucket)) {
            continue;
          }
          nvgMoveTo(args.vg, x0[idx], rowY[idx]);
          nvgLineTo(args.vg, x1[idx], rowY[idx]);
          havePath = true;
        }
        if (!havePath) {
          continue;
        }
        nvgStrokeColor(args.vg, mainColor[size_t(bucket)]);
        nvgStrokeWidth(args.vg, mainWidth[size_t(bucket)]);
        nvgStroke(args.vg);
      }

      // Optional bright core pass for the highest-intensity buckets.
      for (int bucket = 0; bucket < kIntensityBuckets; ++bucket) {
        if (!bucketHasBoost[size_t(bucket)]) {
          continue;
        }
        bool havePath = false;
        nvgBeginPath(args.vg);
        for (int iy = 0; iy < rowCount; ++iy) {
          size_t idx = size_t(iy);
          if (!valid[idx] || bucketByRow[idx] != uint8_t(bucket)) {
            continue;
          }
          nvgMoveTo(args.vg, x0[idx], rowY[idx]);
          nvgLineTo(args.vg, x1[idx], rowY[idx]);
          havePath = true;
        }
        if (!havePath) {
          continue;
        }
        nvgStrokeColor(args.vg, boostColor[size_t(bucket)]);
        nvgStrokeWidth(args.vg, boostWidth[size_t(bucket)]);
        nvgStroke(args.vg);
      }

      // Batch connector lines by the average intensity of adjacent rows.
      for (int bucket = 0; bucket < kIntensityBuckets; ++bucket) {
        bool havePath = false;
        bool prevValid = false;
        float prevX0 = laneCenterXForConnectors;
        float prevX1 = laneCenterXForConnectors;
        float prevY = drawTop + 0.5f;
        float prevVisual = 0.f;
        nvgBeginPath(args.vg);
        for (int iy = 0; iy < rowCount; ++iy) {
          size_t idx = size_t(iy);
          if (!valid[idx]) {
            prevValid = false;
            continue;
          }
          if (prevValid) {
            float connectVisual = clamp(0.5f * (prevVisual + visualIntensity[idx]), 0.f, 1.f);
            int connectBucket = int(std::floor(connectVisual * float(kIntensityBuckets)));
            connectBucket = clamp(connectBucket, 0, kIntensityBuckets - 1);
            if (connectBucket == bucket) {
              nvgMoveTo(args.vg, prevX0, prevY);
              nvgLineTo(args.vg, x0[idx], rowY[idx]);
              nvgMoveTo(args.vg, prevX1, prevY);
              nvgLineTo(args.vg, x1[idx], rowY[idx]);
              havePath = true;
            }
          }
          prevX0 = x0[idx];
          prevX1 = x1[idx];
          prevY = rowY[idx];
          prevVisual = visualIntensity[idx];
          prevValid = true;
        }
        if (!havePath) {
          continue;
        }
        nvgStrokeColor(args.vg, connectColor[size_t(bucket)]);
        nvgStrokeWidth(args.vg, connectWidth[size_t(bucket)]);
        nvgStroke(args.vg);
      }
    };

    drawLane(rowX0, rowX1, rowVisualIntensity, rowValid, rowBucket, lane0CenterX);
    if (renderStereo) {
      drawLane(rowX0Right, rowX1Right, rowVisualIntensityRight, rowValidRight, rowBucketRight, lane1CenterX);

      // Draw subtle lane divider for stereo side-by-side view.
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

      // Full-width center core plus vertical feather.
      drawReadHeadLine(readHeadDrawY - 2.f, 64, 1.f);
      drawReadHeadLine(readHeadDrawY + 2.f, 64, 1.f);
      drawReadHeadLine(readHeadDrawY - 1.f, 148, 1.f);
      drawReadHeadLine(readHeadDrawY + 1.f, 148, 1.f);
      drawReadHeadLine(readHeadDrawY, 255, 1.15f);
    }
    nvgResetScissor(args.vg);
    nvgRestore(args.vg);
  }
};

struct TDScopeWidget : ModuleWidget {
  PanelBorder *panelBorder = nullptr;
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

    auto *display = new TDScopeDisplayWidget;
    display->module = module;
    math::Rect scopeRectMm;
    if (panel_svg::loadRectFromSvgMm(panelPath, "scope", &scopeRectMm)) {
      display->box.pos = mm2px(scopeRectMm.pos);
      display->box.size = mm2px(scopeRectMm.size);
    } else {
      display->box.pos = mm2px(Vec(1.1138f, 10.9404f));
      display->box.size = mm2px(Vec(38.5563f, 109.4206f));
    }
    addChild(display);

    addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(Vec(3.2f, 5.8f)), module, TDScope::LINK_LIGHT));
    addChild(createLightCentered<SmallLight<GreenLight>>(mm2px(Vec(3.2f, 5.8f)), module, TDScope::PREVIEW_LIGHT));
  }

  void step() override {
    bool linkedToDeck = shouldRenderDockBridge();
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
  }

  void appendContextMenu(Menu *menu) override {
    ModuleWidget::appendContextMenu(menu);
    TDScope *scopeModule = dynamic_cast<TDScope *>(module);
    if (!scopeModule) {
      return;
    }

    menu->addChild(new MenuSeparator());
    menu->addChild(createMenuLabel("Scope Range"));
    menu->addChild(createCheckMenuItem(
      "Auto (window peak)", "",
      [=]() { return scopeModule->scopeDisplayRangeMode == TDScope::SCOPE_RANGE_AUTO; },
      [=]() { scopeModule->scopeDisplayRangeMode = TDScope::SCOPE_RANGE_AUTO; }));
    menu->addChild(createCheckMenuItem(
      "+/-2.5V full width", "", [=]() { return scopeModule->scopeDisplayRangeMode == TDScope::SCOPE_RANGE_2V5; },
      [=]() { scopeModule->scopeDisplayRangeMode = TDScope::SCOPE_RANGE_2V5; }));
    menu->addChild(createCheckMenuItem(
      "+/-5V full width", "", [=]() { return scopeModule->scopeDisplayRangeMode == TDScope::SCOPE_RANGE_5V; },
      [=]() { scopeModule->scopeDisplayRangeMode = TDScope::SCOPE_RANGE_5V; }));
    menu->addChild(createCheckMenuItem(
      "+/-10V full width", "", [=]() { return scopeModule->scopeDisplayRangeMode == TDScope::SCOPE_RANGE_10V; },
      [=]() { scopeModule->scopeDisplayRangeMode = TDScope::SCOPE_RANGE_10V; }));

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
  }
};

Model *modelTDScope = createModel<TDScope, TDScopeWidget>("TDScope");
