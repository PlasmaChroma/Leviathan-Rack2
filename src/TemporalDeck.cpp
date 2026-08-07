#include "TemporalDeckArcLights.hpp"
#include "TemporalDeck.hpp"
#include "TemporalDeckEngine.hpp"
#include "TemporalDeckExpanderProtocol.hpp"
#include "TemporalDeckFrameInput.hpp"
#include "LongPlayStreamEngine.hpp"
#include "TemporalDeckPlatterInput.hpp"
#include "TemporalDeckSampleLifecycle.hpp"
#include "TemporalDeckTransportControl.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <limits>
#include <memory>
#include <mutex>
#include <new>
#include <string>
#include <utility>
#include <vector>

// C++11/MinGW can require out-of-class definitions for constexpr static members
// when they are ODR-used.
constexpr int TemporalDeck::CARTRIDGE_CLEAN;
constexpr int TemporalDeck::CARTRIDGE_M44_7;
constexpr int TemporalDeck::CARTRIDGE_ORTOFON_SCRATCH;
constexpr int TemporalDeck::CARTRIDGE_STANTON_680HP;
constexpr int TemporalDeck::CARTRIDGE_QBERT;
constexpr int TemporalDeck::CARTRIDGE_LOFI;
constexpr int TemporalDeck::CARTRIDGE_COUNT;

constexpr int TemporalDeck::SCRATCH_INTERP_CUBIC;
constexpr int TemporalDeck::SCRATCH_INTERP_LAGRANGE6;
constexpr int TemporalDeck::SCRATCH_INTERP_SINC;
constexpr int TemporalDeck::SCRATCH_INTERP_COUNT;

constexpr int TemporalDeck::SLIP_RETURN_SLOW;
constexpr int TemporalDeck::SLIP_RETURN_NORMAL;
constexpr int TemporalDeck::SLIP_RETURN_INSTANT;
constexpr int TemporalDeck::SLIP_RETURN_COUNT;

constexpr int TemporalDeck::BUFFER_DURATION_10S;
constexpr int TemporalDeck::BUFFER_DURATION_20S;
constexpr int TemporalDeck::BUFFER_DURATION_10M_STEREO;
constexpr int TemporalDeck::BUFFER_DURATION_10M_MONO;
constexpr int TemporalDeck::BUFFER_DURATION_1M_STEREO;
constexpr int TemporalDeck::BUFFER_DURATION_2M_STEREO;
constexpr int TemporalDeck::BUFFER_DURATION_LONGPLAY_DISK;
constexpr int TemporalDeck::BUFFER_DURATION_COUNT;

constexpr int TemporalDeck::EXTERNAL_GATE_POS_GLIDE;
constexpr int TemporalDeck::EXTERNAL_GATE_POS_MODULE_SYNC;
constexpr int TemporalDeck::EXTERNAL_GATE_POS_COUNT;

constexpr int TemporalDeck::REVERSE_CV_MODE_PULSED;
constexpr int TemporalDeck::REVERSE_CV_MODE_GATE;
constexpr int TemporalDeck::REVERSE_CV_MODE_COUNT;
constexpr int TemporalDeck::FREEZE_CV_MODE_PULSED;
constexpr int TemporalDeck::FREEZE_CV_MODE_GATE;
constexpr int TemporalDeck::FREEZE_CV_MODE_COUNT;

constexpr int TemporalDeck::SAMPLE_SOURCE_LIVE;
constexpr int TemporalDeck::SAMPLE_SOURCE_FILE;

constexpr int TemporalDeck::PLATTER_ART_BUILTIN_SVG;
constexpr int TemporalDeck::PLATTER_ART_DRAGON_KING;
constexpr int TemporalDeck::PLATTER_ART_PROCEDURAL;
constexpr int TemporalDeck::PLATTER_ART_CUSTOM;
constexpr int TemporalDeck::PLATTER_ART_BLANK;
constexpr int TemporalDeck::PLATTER_ART_TEMPORAL_DECK;
constexpr int TemporalDeck::PLATTER_ART_MODE_COUNT;

constexpr int TemporalDeck::PLATTER_BRIGHTNESS_FULL;
constexpr int TemporalDeck::PLATTER_BRIGHTNESS_MEDIUM;
constexpr int TemporalDeck::PLATTER_BRIGHTNESS_LOW;
constexpr int TemporalDeck::PLATTER_BRIGHTNESS_COUNT;

constexpr float TemporalDeck::kNominalPlatterRpm;
constexpr float TemporalDeck::kMouseScratchTravelScale;
constexpr float TemporalDeck::kWheelScratchTravelScale;
constexpr float TemporalDeck::kUiPublishRateHz;
constexpr float TemporalDeck::kUiPublishIntervalSec;
constexpr int TemporalDeck::kArcLightCount;

using temporaldeck::TemporalDeckEngine;

namespace {
using temporaldeck_modes::isMonoBufferMode;
using temporaldeck_modes::usableBufferSecondsForMode;

using temporaldeck::PreparedSampleData;
using temporaldeck::PlatterInputSnapshot;
using temporaldeck::PlatterInputState;
static std::atomic<uint32_t> gTemporalDeckDebugInstanceCounter {1u};

struct LongPlayBridge {
  temporaldeck::LongPlayStreamEngine *stream = nullptr;
  double sourceFramesPerOutputFrame = 1.0;

  static bool readFrame(void *context, uint64_t outputFrame, float *left, float *right) {
    LongPlayBridge *bridge = static_cast<LongPlayBridge *>(context);
    if (!bridge || !bridge->stream) {
      return false;
    }
    const double sourcePosition = double(outputFrame) * bridge->sourceFramesPerOutputFrame;
    const uint64_t sourceFrame0 = uint64_t(std::floor(std::max(0.0, sourcePosition)));
    const uint64_t sourceFrame1 = std::min(
      sourceFrame0 + 1u, std::max<uint64_t>(1u, bridge->stream->totalFrames()) - 1u);
    float l0 = 0.f;
    float r0 = 0.f;
    float l1 = 0.f;
    float r1 = 0.f;
    if (!bridge->stream->readFrame(sourceFrame0, &l0, &r0) ||
        !bridge->stream->readFrame(sourceFrame1, &l1, &r1)) {
      return false;
    }
    const float fraction = float(sourcePosition - double(sourceFrame0));
    if (left) *left = crossfade(l0, l1, fraction) * temporaldeck::kSampleFileVoltageScale;
    if (right) *right = crossfade(r0, r1, fraction) * temporaldeck::kSampleFileVoltageScale;
    return true;
  }

  static void setDesiredFrame(void *context, uint64_t outputFrame, bool loop,
                              uint64_t loopStartOutputFrame,
                              uint64_t loopEndOutputFrameExclusive) {
    LongPlayBridge *bridge = static_cast<LongPlayBridge *>(context);
    if (!bridge || !bridge->stream) {
      return;
    }
    const uint64_t sourceFrame = uint64_t(
      std::floor(double(outputFrame) * bridge->sourceFramesPerOutputFrame));
    const uint64_t loopStartSourceFrame = uint64_t(
      std::floor(double(loopStartOutputFrame) * bridge->sourceFramesPerOutputFrame));
    const uint64_t loopEndSourceFrameExclusive = uint64_t(
      std::ceil(double(loopEndOutputFrameExclusive) * bridge->sourceFramesPerOutputFrame));
    bridge->stream->setDesiredWindow(sourceFrame, loop, loopStartSourceFrame,
                                     loopEndSourceFrameExclusive);
  }

  static bool isFrameResident(void *context, uint64_t outputFrame) {
    LongPlayBridge *bridge = static_cast<LongPlayBridge *>(context);
    if (!bridge || !bridge->stream) {
      return false;
    }
    const double sourcePosition = double(outputFrame) * bridge->sourceFramesPerOutputFrame;
    const uint64_t sourceFrame0 = uint64_t(std::floor(std::max(0.0, sourcePosition)));
    const uint64_t sourceFrame1 = std::min(
      sourceFrame0 + 1u, std::max<uint64_t>(1u, bridge->stream->totalFrames()) - 1u);
    return bridge->stream->isFrameResident(sourceFrame0) &&
      bridge->stream->isFrameResident(sourceFrame1);
  }
};

// Match TD.Scope's current practical update ceiling so the audio thread does
// not spend time preparing preview payloads faster than the UI can consume.
static constexpr float kExpanderPublishRateHz = 90.f;
static constexpr float kExpanderPublishIntervalSec = 1.f / kExpanderPublishRateHz;
static constexpr float kExpanderPublishRateHzLiveIdle = 60.f;
static constexpr float kExpanderPublishIntervalSecLiveIdle = 1.f / kExpanderPublishRateHzLiveIdle;
static constexpr float kExpanderPublishRateHzFrozenLive = 20.f;
static constexpr float kExpanderPublishIntervalSecFrozenLive = 1.f / kExpanderPublishRateHzFrozenLive;
static constexpr float kScopeHalfWindowMs = 900.f;
static constexpr float kScopeHalfWindowSeconds = kScopeHalfWindowMs * 0.001f;
static constexpr float kScopeDragNominalTurnScale = 1.00f;
static constexpr float kScopeLiveNowAssistWindowSec = 1.0f;
static constexpr float kScopeLiveNearNowTargetBoostBase = 0.25f;
static constexpr float kScopeLiveNearNowTargetBoostExtra = 2.4f;
static constexpr int kScopeEvaluationBudgetPerPublish = 16384;
static constexpr int kScopeLagFpShift = 10;
static constexpr int64_t kScopeLagFpOne = int64_t(1) << kScopeLagFpShift;
static std::mutex gTemporalDeckLifetimeLogMutex;

static double elapsedMs(std::chrono::steady_clock::time_point start, std::chrono::steady_clock::time_point end) {
  return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() * 1e-3;
}

static std::string csvString(const std::string &s) {
  std::string out;
  out.reserve(s.size() + 2);
  out.push_back('"');
  for (char c : s) {
    if (c == '"') {
      out.push_back('"');
    }
    out.push_back(c);
  }
  out.push_back('"');
  return out;
}

static void appendTemporalDeckLifetimeShutdownLog(uint32_t debugInstanceId,
                                                  const std::string &samplePath,
                                                  bool sampleLoaded,
                                                  int sampleFrames,
                                                  int bufferSize,
                                                  size_t engineSampleBytes,
                                                  size_t lifecycleDecodedBytes,
                                                  size_t lifecyclePreparedBytes,
                                                  bool decodedAvailable,
                                                  bool buildInProgressBeforeStop,
                                                  double stopWorkerMs,
                                                  double implResetMs,
                                                  double totalMs) {
  if (!isTemporalDeckLifetimeLoggingEnabled()) {
    return;
  }
  const std::string dir = system::join(asset::user(), "Leviathan/TemporalDeck/Lifetime");
  system::createDirectories(dir);
  const std::string path = system::join(dir, "shutdown.csv");

  std::lock_guard<std::mutex> lock(gTemporalDeckLifetimeLogMutex);
  std::ifstream existing(path);
  const bool writeHeader = !existing.good();
  existing.close();

  std::ofstream out(path, std::ios::app);
  if (!out.is_open()) {
    WARN("TemporalDeck: failed to open lifetime shutdown log: %s", path.c_str());
    return;
  }
  if (writeHeader) {
    out << "unix_time_sec,module_debug_id,sample_path,sample_loaded,sample_frames,buffer_size,"
        << "engine_sample_bytes,lifecycle_decoded_bytes,lifecycle_prepared_bytes,decoded_available,"
        << "build_in_progress_before_stop,stop_worker_ms,impl_reset_ms,total_ms\n";
  }
  out << std::time(nullptr) << ','
      << debugInstanceId << ','
      << csvString(samplePath) << ','
      << (sampleLoaded ? 1 : 0) << ','
      << sampleFrames << ','
      << bufferSize << ','
      << engineSampleBytes << ','
      << lifecycleDecodedBytes << ','
      << lifecyclePreparedBytes << ','
      << (decodedAvailable ? 1 : 0) << ','
      << (buildInProgressBeforeStop ? 1 : 0) << ','
      << std::fixed << std::setprecision(3)
      << stopWorkerMs << ','
      << implResetMs << ','
      << totalMs << '\n';
}

static void appendTemporalDeckLifetimeLoadingLog(uint32_t debugInstanceId,
                                                 const char *eventName,
                                                 const std::string &samplePath,
                                                 uint64_t requestSerial,
                                                 int requestType,
                                                 float sampleRate,
                                                 int bufferMode,
                                                 int sourceFrames,
                                                 int sourceChannels,
                                                 int preparedFrames,
                                                 size_t engineSampleBytes,
                                                 size_t lifecycleDecodedBytes,
                                                 size_t lifecyclePreparedBytes,
                                                 bool decodedAvailable,
                                                 bool buildInProgress,
                                                 double workerDecodeMs,
                                                 double workerPrepMs,
                                                 double workerTotalMs,
                                                 double processResetMs,
                                                 double processInstallMs,
                                                 double processTotalMs,
                                                 const char *note = "") {
  if (!isTemporalDeckLifetimeLoggingEnabled()) {
    return;
  }
  const std::string dir = system::join(asset::user(), "Leviathan/TemporalDeck/Lifetime");
  system::createDirectories(dir);
  const std::string path = system::join(dir, "loading.csv");

  std::lock_guard<std::mutex> lock(gTemporalDeckLifetimeLogMutex);
  std::ifstream existing(path);
  const bool writeHeader = !existing.good();
  existing.close();

  std::ofstream out(path, std::ios::app);
  if (!out.is_open()) {
    WARN("TemporalDeck: failed to open lifetime loading log: %s", path.c_str());
    return;
  }
  if (writeHeader) {
    out << "unix_time_sec,module_debug_id,event,sample_path,request_serial,request_type,sample_rate,buffer_mode,"
        << "source_frames,source_channels,prepared_frames,engine_sample_bytes,lifecycle_decoded_bytes,"
        << "lifecycle_prepared_bytes,decoded_available,build_in_progress,worker_decode_ms,worker_prep_ms,"
        << "worker_total_ms,process_reset_ms,process_install_ms,process_total_ms,note\n";
  }
  out << std::time(nullptr) << ','
      << debugInstanceId << ','
      << csvString(eventName ? eventName : "") << ','
      << csvString(samplePath) << ','
      << requestSerial << ','
      << requestType << ','
      << std::fixed << std::setprecision(3)
      << sampleRate << ','
      << bufferMode << ','
      << sourceFrames << ','
      << sourceChannels << ','
      << preparedFrames << ','
      << engineSampleBytes << ','
      << lifecycleDecodedBytes << ','
      << lifecyclePreparedBytes << ','
      << (decodedAvailable ? 1 : 0) << ','
      << (buildInProgress ? 1 : 0) << ','
      << workerDecodeMs << ','
      << workerPrepMs << ','
      << workerTotalMs << ','
      << processResetMs << ','
      << processInstallMs << ','
      << processTotalMs << ','
      << csvString(note ? note : "") << '\n';
}

struct ScopeInteractionRequest {
  bool valid = false;
  uint32_t requestedScopeFormat = temporaldeck_expander::SCOPE_FORMAT_MONO;
  bool active = false;
  bool stationaryHold = false;
  bool usesNormalizedMotion = false;
  float lagSamples = 0.f;
  float velocitySamples = 0.f;
  float normalizedOffset = 0.f;
  float normalizedVelocity = 0.f;
  uint32_t phase = temporaldeck_expander::LAG_DRAG_PHASE_INACTIVE;
  uint64_t requestSeq = 0u;
};

struct ScopeViewState {
  bool freezeWaveformWindow = false;
  bool useNewestAbsoluteAnchor = false;
  double waveformNewestAbsoluteAnchor = -1.0;
  float waveformLagAnchor = 0.f;
  float markerLag = 0.f;
};

static ScopeInteractionRequest decodeScopeInteractionRequest(const temporaldeck_expander::DisplayToHost* request) {
  ScopeInteractionRequest decoded;
  if (!request || !temporaldeck_expander::isDisplayRequestValid(*request)) {
    return decoded;
  }

  decoded.valid = true;
  decoded.requestSeq = request->requestSeq;
  decoded.requestedScopeFormat = (request->requestedScopeFormat == temporaldeck_expander::SCOPE_FORMAT_STEREO)
                                   ? temporaldeck_expander::SCOPE_FORMAT_STEREO
                                   : temporaldeck_expander::SCOPE_FORMAT_MONO;
  decoded.active = temporaldeck_expander::decodeLagDragRequest(request->reserved, &decoded.lagSamples, &decoded.stationaryHold);
  if (request->version >= 2u) {
    decoded.velocitySamples = request->lagDragVelocity;
  }

  const size_t normalizedRequestMinSize =
    offsetof(temporaldeck_expander::DisplayToHost, lagDragNormalizedVelocity) + sizeof(float);
  if (request->version >= 3u && request->size >= normalizedRequestMinSize) {
    decoded.usesNormalizedMotion = true;
    decoded.normalizedOffset = request->lagDragNormalizedOffset;
    decoded.normalizedVelocity = request->lagDragNormalizedVelocity;
  }

  const size_t phaseRequestMinSize = offsetof(temporaldeck_expander::DisplayToHost, lagDragPhase) + sizeof(uint32_t);
  if (request->version >= 4u && request->size >= phaseRequestMinSize) {
    decoded.phase = temporaldeck_expander::normalizeLagDragPhase(request->lagDragPhase);
    decoded.active = temporaldeck_expander::isLagDragPhaseActive(decoded.phase);
    decoded.stationaryHold = decoded.phase == temporaldeck_expander::LAG_DRAG_PHASE_HOLD;
  } else {
    decoded.phase = temporaldeck_expander::lagDragPhaseFromFlags(decoded.active, decoded.stationaryHold);
  }

  if (!std::isfinite(decoded.lagSamples) || decoded.lagSamples < 0.f) {
    decoded.lagSamples = 0.f;
  }
  if (!std::isfinite(decoded.normalizedOffset)) {
    decoded.normalizedOffset = 0.f;
  }
  if (!std::isfinite(decoded.normalizedVelocity)) {
    decoded.normalizedVelocity = 0.f;
  }
  return decoded;
}

static ScopeViewState computeScopeViewState(const PlatterInputSnapshot& platterInput, float frameLagSamples,
                                            float frameAccessibleLagSamples, bool expanderLagDragWasActive,
                                            bool expanderLagDragLastStationaryHold, float expanderLagDragLastLagSamples,
                                            bool& lagHoldActive, float& lagHoldSamples, bool& newestPosHoldActive,
                                            double& newestAbsolutePosHold, const TemporalDeckEngine& engine) {
  ScopeViewState state;
  state.markerLag = frameLagSamples;
  state.waveformLagAnchor = frameLagSamples;
  if (expanderLagDragWasActive && std::isfinite(expanderLagDragLastLagSamples)) {
    state.waveformLagAnchor = clamp(expanderLagDragLastLagSamples, 0.f, frameAccessibleLagSamples);
  }
  state.freezeWaveformWindow = platterInput.platterTouchHoldDirect ||
                               (platterInput.scopeLagDragActive && !platterInput.platterMotionActive) ||
                               (expanderLagDragWasActive && expanderLagDragLastStationaryHold);
  if (state.freezeWaveformWindow) {
    if (!lagHoldActive) {
      lagHoldSamples = state.waveformLagAnchor;
      lagHoldActive = true;
    }
    state.waveformLagAnchor = lagHoldSamples;
    if (!newestPosHoldActive) {
      newestAbsolutePosHold = engine.newestReadableAbsolutePos();
      newestPosHoldActive = true;
    }
    state.useNewestAbsoluteAnchor = true;
    state.waveformNewestAbsoluteAnchor = newestAbsolutePosHold;
  } else {
    lagHoldActive = false;
    newestPosHoldActive = false;
  }
  return state;
}

static bool isTDScopeModule(const engine::Module *neighbor) {
  if (!neighbor || !neighbor->model) {
    return false;
  }
  return (neighbor->model == modelTDScope) || (neighbor->model->slug == "TDScope");
}

struct ScopeWindowParams {
  bool sampleMode = false;
  bool sampleLoopEnabled = false;
  float sampleRate = 44100.f;
  float lagSamples = 0.f;
  float accessibleLagSamples = 0.f;
  float minLagSamples = 0.f;
  float maxLagSamples = 0.f;
  uint32_t binCount = temporaldeck_expander::SCOPE_BIN_COUNT;
  int scopeStride = 1;
  int64_t scopeStartLagFp = 0;
  float scopeStartLagSamples = 0.f;
  float scopeVisibleStartLagSamples = 0.f;
  int64_t binSpanLagFp = 1;
  float binSpanSamples = 1.f;
  double newestPos = 0.0;
  double newestLiveAbsolutePos = 0.0;
  int newestLiveIndex = 0;
};

struct ScopeWindowCache {
  bool valid = false;
  ScopeWindowParams params;
  uint32_t scopeBinCount = 0u;
  uint64_t streamResidencyGeneration = 0u;
  std::array<temporaldeck_expander::ScopeBin, temporaldeck_expander::SCOPE_BIN_COUNT> bins;
};

enum ScopeChannelMode {
  // Must stay aligned with TemporalDeckEngine::readLiveScopeEnvelopeRange():
  // 0 = left, 1 = right, 2 = mid.
  SCOPE_CHANNEL_LEFT = 0,
  SCOPE_CHANNEL_RIGHT = 1,
  SCOPE_CHANNEL_MID = 2
};

static float reduceScopeChannelValue(float left, float right, ScopeChannelMode channelMode) {
  switch (channelMode) {
    case SCOPE_CHANNEL_LEFT:
      return left;
    case SCOPE_CHANNEL_RIGHT:
      return right;
    case SCOPE_CHANNEL_MID:
    default:
      return 0.5f * (left + right);
  }
}

static bool readScopeChannelAtLagSamples(const TemporalDeckEngine &engine, double newestPos, bool sampleMode,
                                         bool sampleLoopEnabled, double lagSamples, ScopeChannelMode channelMode,
                                         float *valueOut) {
  if (!valueOut) {
    return false;
  }
  if (engine.buffer.size <= 0) {
    return false;
  }
  double pos = newestPos - lagSamples;
  int idx = 0;
  if (sampleMode) {
    int maxIndex = std::max(0, std::min(engine.sampleFrames - 1, int(std::floor(std::max(0.0, newestPos)))));
    if (maxIndex <= 0) {
      idx = 0;
    } else {
      if (sampleLoopEnabled) {
        double length = std::max(1.0, double(maxIndex) + 1.0);
        double wrappedPos = std::fmod(pos, length);
        if (wrappedPos < 0.0) {
          wrappedPos += length;
        }
        idx = int(std::lround(wrappedPos));
      } else {
        idx = int(std::lround(pos));
      }
      idx = std::max(0, std::min(maxIndex, idx));
    }
  } else {
    double wrappedPos = engine.buffer.wrapPosition(pos);
    idx = engine.buffer.wrapIndex(int(std::lround(wrappedPos)));
  }
  float left = 0.f;
  float right = 0.f;
  if (sampleMode && engine.diskBackedSample) {
    if (!engine.readStreamedScopeFrame(idx, &left, &right)) {
      return false;
    }
  } else {
    left = sampleMode ? engine.sampleLeftAt(idx) : engine.buffer.left[size_t(idx)];
    right = sampleMode ? engine.sampleRightAt(idx) : engine.buffer.rightSample(idx);
  }
  *valueOut = reduceScopeChannelValue(left, right, channelMode);
  return true;
}

static bool computeScopeWindowParams(const TemporalDeckEngine &engine, bool sampleMode, bool sampleLoopEnabled,
                                     float sampleRate, float lagSamples, float accessibleLagSamples,
                                     double liveNewestAbsolutePosOverride, ScopeWindowParams *out) {
  if (!out) {
    return false;
  }
  *out = ScopeWindowParams();
  out->sampleMode = sampleMode;
  out->sampleLoopEnabled = sampleLoopEnabled;
  out->sampleRate = sampleRate;
  out->lagSamples = lagSamples;
  out->accessibleLagSamples = accessibleLagSamples;
  out->binCount =
    sampleMode ? temporaldeck_expander::SAMPLE_SCOPE_BIN_COUNT : temporaldeck_expander::LIVE_SCOPE_BIN_COUNT;
  if (out->binCount == 0u || engine.buffer.size <= 0) {
    return false;
  }

  float sr = std::max(sampleRate, 1.f);
  float halfWindowSamples = sr * kScopeHalfWindowSeconds;
  float totalWindowSamples = std::max(1.f, 2.f * halfWindowSamples * (sampleMode ? 3.f : 1.f));
  int64_t totalWindowLagFp =
    std::max<int64_t>(kScopeLagFpOne, int64_t(std::llround(double(totalWindowSamples) * double(kScopeLagFpOne))));
  out->binSpanLagFp = std::max<int64_t>(1, totalWindowLagFp / int64_t(out->binCount));
  out->binSpanSamples = float(double(out->binSpanLagFp) / double(kScopeLagFpOne));
  int totalWindowSamplesInt = std::max(1, int(std::ceil(totalWindowSamples)));
  int effectiveEvalBudget = kScopeEvaluationBudgetPerPublish;
  out->scopeStride = std::max(1, int(std::ceil(double(totalWindowSamplesInt) / double(effectiveEvalBudget))));

  float forwardWindowSamples = sampleMode ? 3.f * halfWindowSamples : halfWindowSamples;
  float backwardWindowSamples = forwardWindowSamples;
  if (!sampleMode) {
    // Live mode: when near NOW, keep full 1.8s window but bias it backward
    // so the read-head can move toward the bottom of the scope.
    forwardWindowSamples = std::min(halfWindowSamples, std::max(lagSamples, 0.f));
    backwardWindowSamples = totalWindowSamples - forwardWindowSamples;
  }

  float scopeStartLagSamples = lagSamples + backwardWindowSamples;
  out->scopeStartLagFp = int64_t(std::llround(double(scopeStartLagSamples) * double(kScopeLagFpOne)));
  // Anchor bin boundaries to a global lag grid in fixed-point so envelope
  // sampling phase stays deterministic while the visible window moves.
  if (out->binSpanLagFp > 0) {
    out->scopeStartLagFp = ((out->scopeStartLagFp + out->binSpanLagFp - 1) / out->binSpanLagFp) * out->binSpanLagFp;
  }
  out->scopeStartLagSamples = float(double(out->scopeStartLagFp) / double(kScopeLagFpOne));
  out->scopeVisibleStartLagSamples = sampleMode ? (lagSamples + halfWindowSamples) : out->scopeStartLagSamples;

  out->newestPos = sampleMode ? double(std::max(0.f, accessibleLagSamples))
                              : (liveNewestAbsolutePosOverride >= 0.0
                                   ? engine.buffer.wrapPosition(liveNewestAbsolutePosOverride)
                                   : engine.newestReadablePos());
  out->newestLiveAbsolutePos =
    sampleMode ? out->newestPos
               : (liveNewestAbsolutePosOverride >= 0.0 ? liveNewestAbsolutePosOverride : engine.newestReadableAbsolutePos());
  if (!sampleMode) {
    out->newestLiveIndex = engine.buffer.wrapIndex(int(std::lround(out->newestPos)));
  }
  out->minLagSamples = 0.f;
  out->maxLagSamples = std::max(0.f, accessibleLagSamples);
  return true;
}

static bool evaluateScopeBinAtIndex(const TemporalDeckEngine &engine, const ScopeWindowParams &params, uint32_t binIndex,
                                    ScopeChannelMode channelMode, temporaldeck_expander::ScopeBin *outBin) {
  if (!outBin || binIndex >= params.binCount) {
    return false;
  }

  int64_t lagHighFp = params.scopeStartLagFp - int64_t(binIndex) * params.binSpanLagFp;
  int64_t lagLowFp = lagHighFp - params.binSpanLagFp;
  float lagHigh = float(double(lagHighFp) / double(kScopeLagFpOne));
  float lagLow = float(double(lagLowFp) / double(kScopeLagFpOne));
  float overlapLow = 0.f;
  float overlapHigh = 0.f;
  if (params.sampleMode && params.sampleLoopEnabled && params.maxLagSamples > 0.f) {
    // In sample loop mode, window coverage wraps around the sample bounds.
    // Do not clip to [0, maxLag] so bins near boundaries still render.
    overlapLow = std::min(lagLow, lagHigh);
    overlapHigh = std::max(lagLow, lagHigh);
  } else {
    overlapLow = std::max(params.minLagSamples, std::min(lagLow, lagHigh));
    overlapHigh = std::min(params.maxLagSamples, std::max(lagLow, lagHigh));
    if (overlapHigh < overlapLow) {
      *outBin = temporaldeck_expander::makeEmptyScopeBin();
      return false;
    }
  }

  int firstLag = int(std::floor(overlapLow));
  int lastLag = int(std::ceil(overlapHigh));
  if (lastLag < firstLag) {
    *outBin = temporaldeck_expander::makeEmptyScopeBin();
    return false;
  }

  bool hasData = false;
  float minMono = 0.f;
  float maxMono = 0.f;
  if (!params.sampleMode) {
    if (!engine.readLiveScopeEnvelopeRange(params.newestLiveAbsolutePos, overlapLow, overlapHigh, channelMode, &minMono, &maxMono)) {
      *outBin = temporaldeck_expander::makeEmptyScopeBin();
      return false;
    }
    outBin->min = temporaldeck_expander::quantizePreviewSample(minMono);
    outBin->max = temporaldeck_expander::quantizePreviewSample(maxMono);
    return true;
  }
  int lastAccumulatedLag = std::numeric_limits<int>::min();
  auto accumulateLag = [&](int lag) {
    if (lag == lastAccumulatedLag) {
      return;
    }
    float mono = 0.f;
    if (!params.sampleMode) {
      // Fast path: live scope extraction uses integer lag taps, so avoid
      // generic floating wrap/round math on every sampled point.
      int idx = engine.buffer.wrapIndex(params.newestLiveIndex - lag);
      float left = engine.buffer.left[size_t(idx)];
      float right = engine.buffer.rightSample(idx);
      mono = reduceScopeChannelValue(left, right, channelMode);
    } else {
      if (!readScopeChannelAtLagSamples(engine, params.newestPos, params.sampleMode,
                                        params.sampleLoopEnabled, double(lag), channelMode, &mono)) {
        lastAccumulatedLag = lag;
        return;
      }
    }
    if (!hasData) {
      minMono = mono;
      maxMono = mono;
      hasData = true;
    } else {
      minMono = std::min(minMono, mono);
      maxMono = std::max(maxMono, mono);
    }
    lastAccumulatedLag = lag;
  };

  // Always include both bin edges, and sample interior points on a stable global lattice.
  // This reduces visible peak jitter ("dancing peaks") when the scope window shifts.
  accumulateLag(firstLag);
  int alignedLag = ((firstLag + params.scopeStride - 1) / params.scopeStride) * params.scopeStride;
  for (int lag = alignedLag; lag <= lastLag; lag += params.scopeStride) {
    accumulateLag(lag);
  }
  accumulateLag(lastLag);

  if (!hasData) {
    *outBin = temporaldeck_expander::makeEmptyScopeBin();
    return false;
  }

  outBin->min = temporaldeck_expander::quantizePreviewSample(minMono);
  outBin->max = temporaldeck_expander::quantizePreviewSample(maxMono);
  return true;
}

static uint32_t buildScopeWindowBins(const TemporalDeckEngine &engine, const ScopeWindowParams &params,
                                     ScopeChannelMode channelMode,
                                     std::array<temporaldeck_expander::ScopeBin, temporaldeck_expander::SCOPE_BIN_COUNT> *binsOut) {
  if (!binsOut || params.binCount == 0u || engine.buffer.size <= 0) {
    return 0u;
  }
  const temporaldeck_expander::ScopeBin emptyBin = temporaldeck_expander::makeEmptyScopeBin();
  binsOut->fill(emptyBin);

  uint32_t validCount = 0u;
  for (uint32_t i = 0; i < params.binCount; ++i) {
    temporaldeck_expander::ScopeBin bin = emptyBin;
    if (evaluateScopeBinAtIndex(engine, params, i, channelMode, &bin)) {
      validCount++;
    }
    (*binsOut)[i] = bin;
  }

  // Preserve timeline indexing across the full window (including intentionally
  // empty bins near clamped edges). Returning only validCount can truncate the
  // tail and hide forward look-ahead at sample boundaries.
  return validCount > 0u ? params.binCount : 0u;
}

static bool canReuseScopeWindowCache(const ScopeWindowParams &current, const ScopeWindowCache &cache,
                                     bool allowLiveReuse, bool diskBackedSample,
                                     uint64_t streamResidencyGeneration) {
  if (!cache.valid || cache.scopeBinCount == 0u) {
    return false;
  }
  if (!current.sampleMode && !allowLiveReuse) {
    return false;
  }
  // Rebuild only when the worker actually publishes different residency.
  // Between block publications, streamed scope windows can use the same
  // shift-and-edge cache as resident samples.
  if (current.sampleMode && diskBackedSample &&
      cache.streamResidencyGeneration != streamResidencyGeneration) {
    return false;
  }
  const ScopeWindowParams &prev = cache.params;
  if (current.sampleMode != prev.sampleMode || current.sampleLoopEnabled != prev.sampleLoopEnabled) {
    return false;
  }
  if (current.binCount != prev.binCount || current.binSpanLagFp != prev.binSpanLagFp ||
      current.scopeStride != prev.scopeStride) {
    return false;
  }
  if (std::fabs(current.maxLagSamples - prev.maxLagSamples) > 1.f) {
    return false;
  }
  return true;
}

static uint32_t buildScopeWindowBinsWithCache(
  const TemporalDeckEngine &engine, const ScopeWindowParams &params, ScopeChannelMode channelMode, ScopeWindowCache *cache,
  std::array<temporaldeck_expander::ScopeBin, temporaldeck_expander::SCOPE_BIN_COUNT> *binsOut, bool allowLiveReuse = false) {
  if (!binsOut) {
    return 0u;
  }

  uint32_t scopeBinCount = 0u;
  bool reused = false;
  if (cache && canReuseScopeWindowCache(params, *cache, allowLiveReuse,
                                        engine.diskBackedSample,
                                        engine.streamResidencyGeneration)) {
    const ScopeWindowParams &prev = cache->params;
    int64_t newestDeltaFp = int64_t(std::llround((params.newestPos - prev.newestPos) * double(kScopeLagFpOne)));
    int64_t shiftLagFp = (prev.scopeStartLagFp - params.scopeStartLagFp) + newestDeltaFp;
    int shiftBins = int(std::llround(double(shiftLagFp) / double(params.binSpanLagFp)));
    int64_t residual = shiftLagFp - int64_t(shiftBins) * params.binSpanLagFp;
    if (residual < 0) {
      residual = -residual;
    }

    if (residual <= (params.binSpanLagFp / 4) && std::abs(shiftBins) < int(params.binCount)) {
      reused = true;
      const temporaldeck_expander::ScopeBin emptyBin = temporaldeck_expander::makeEmptyScopeBin();
      binsOut->fill(emptyBin);
      for (uint32_t i = 0; i < params.binCount; ++i) {
        int j = int(i) + shiftBins;
        if (j >= 0 && j < int(params.binCount)) {
          (*binsOut)[i] = cache->bins[size_t(j)];
        } else {
          temporaldeck_expander::ScopeBin bin = emptyBin;
          (void)evaluateScopeBinAtIndex(engine, params, i, channelMode, &bin);
          (*binsOut)[i] = bin;
        }
      }
      uint32_t validCount = 0u;
      for (uint32_t i = 0; i < params.binCount; ++i) {
        if (temporaldeck_expander::isScopeBinValid((*binsOut)[i])) {
          validCount++;
        }
      }
      scopeBinCount = validCount > 0u ? params.binCount : 0u;
    }
  }

  if (!reused) {
    scopeBinCount = buildScopeWindowBins(engine, params, channelMode, binsOut);
  }

  if (cache) {
    cache->valid = scopeBinCount > 0u;
    cache->params = params;
    cache->scopeBinCount = scopeBinCount;
    cache->streamResidencyGeneration = engine.streamResidencyGeneration;
    if (scopeBinCount > 0u) {
      cache->bins = *binsOut;
    }
  }

  return scopeBinCount;
}

static std::string normalizePathForPrefixCompare(std::string path) {
  std::replace(path.begin(), path.end(), '\\', '/');
  while (path.size() > 1 && path.back() == '/') {
    path.pop_back();
  }
  return path;
}

static bool hasPathPrefix(const std::string &path, const std::string &prefix) {
  if (path.empty() || prefix.empty() || path.size() < prefix.size()) {
    return false;
  }
  if (path.compare(0, prefix.size(), prefix) != 0) {
    return false;
  }
  if (path.size() == prefix.size()) {
    return true;
  }
  char next = path[prefix.size()];
  return next == '/' || next == '\\';
}

static bool isManagedVinylArtPath(const std::string &path) {
  if (path.empty() || !pluginInstance) {
    return false;
  }
  std::string normalizedPath = normalizePathForPrefixCompare(path);
  std::string builtInRoot = normalizePathForPrefixCompare(asset::plugin(pluginInstance, "res/Vinyl"));
  std::string expandedRoot = normalizePathForPrefixCompare(system::join(asset::user(), "Leviathan/TemporalDeck/Vinyl"));
  return hasPathPrefix(normalizedPath, builtInRoot) || hasPathPrefix(normalizedPath, expandedRoot);
}

static void writeLe16(std::ofstream &out, uint16_t value) {
  char bytes[2];
  bytes[0] = char(value & 0xFFu);
  bytes[1] = char((value >> 8) & 0xFFu);
  out.write(bytes, sizeof(bytes));
}

static void writeLe32(std::ofstream &out, uint32_t value) {
  char bytes[4];
  bytes[0] = char(value & 0xFFu);
  bytes[1] = char((value >> 8) & 0xFFu);
  bytes[2] = char((value >> 16) & 0xFFu);
  bytes[3] = char((value >> 24) & 0xFFu);
  out.write(bytes, sizeof(bytes));
}

static int16_t floatToPcm16(float x) {
  x = clamp(x, -1.f, 1.f);
  int v = int(std::lround(double(x) * 32767.0));
  v = std::max(-32768, std::min(32767, v));
  return int16_t(v);
}

static bool writeStereoOrMonoWav16(const std::string &path, const std::vector<float> &left, const std::vector<float> &right,
                                   int frames, int channels, float sampleRate, float inputScale,
                                   std::string *errorOut) {
  if (path.empty()) {
    if (errorOut) {
      *errorOut = "Missing save path";
    }
    return false;
  }
  if (frames <= 0 || channels < 1 || channels > 2 || sampleRate <= 0.f) {
    if (errorOut) {
      *errorOut = "Invalid sample data";
    }
    return false;
  }
  if (int(left.size()) < frames || (channels == 2 && int(right.size()) < frames)) {
    if (errorOut) {
      *errorOut = "Sample buffer is incomplete";
    }
    return false;
  }

  uint32_t sr = uint32_t(std::max(1, int(std::lround(double(sampleRate)))));
  uint32_t blockAlign = uint32_t(channels * 2);
  uint64_t dataBytes64 = uint64_t(std::max(frames, 0)) * uint64_t(blockAlign);
  if (dataBytes64 > uint64_t(std::numeric_limits<uint32_t>::max())) {
    if (errorOut) {
      *errorOut = "Sample is too large to save as WAV";
    }
    return false;
  }
  uint32_t dataBytes = uint32_t(dataBytes64);
  uint64_t riffSize64 = 36ull + uint64_t(dataBytes);
  if (riffSize64 > uint64_t(std::numeric_limits<uint32_t>::max())) {
    if (errorOut) {
      *errorOut = "WAV output exceeds RIFF size limit";
    }
    return false;
  }
  uint32_t riffSize = uint32_t(riffSize64);
  uint32_t byteRate = sr * blockAlign;

  std::ofstream out(path.c_str(), std::ios::binary | std::ios::trunc);
  if (!out.good()) {
    if (errorOut) {
      *errorOut = "Failed to open save path";
    }
    return false;
  }

  out.write("RIFF", 4);
  writeLe32(out, riffSize);
  out.write("WAVE", 4);
  out.write("fmt ", 4);
  writeLe32(out, 16u);
  writeLe16(out, 1u); // PCM int16
  writeLe16(out, uint16_t(channels));
  writeLe32(out, sr);
  writeLe32(out, byteRate);
  writeLe16(out, uint16_t(blockAlign));
  writeLe16(out, 16u);
  out.write("data", 4);
  writeLe32(out, dataBytes);

  for (int i = 0; i < frames; ++i) {
    int16_t l = floatToPcm16(left[size_t(i)] * inputScale);
    writeLe16(out, uint16_t(l));
    if (channels == 2) {
      int16_t r = floatToPcm16(right[size_t(i)] * inputScale);
      writeLe16(out, uint16_t(r));
    }
  }

  if (!out.good()) {
    if (errorOut) {
      *errorOut = "Failed while writing WAV file";
    }
    return false;
  }
  return true;
}

static int nextCartridgeCharacter(int current) {
  switch (current) {
  case TemporalDeck::CARTRIDGE_CLEAN:
    return TemporalDeck::CARTRIDGE_M44_7;
  case TemporalDeck::CARTRIDGE_M44_7:
    return TemporalDeck::CARTRIDGE_ORTOFON_SCRATCH;
  case TemporalDeck::CARTRIDGE_ORTOFON_SCRATCH:
    return TemporalDeck::CARTRIDGE_QBERT;
  case TemporalDeck::CARTRIDGE_QBERT:
    return TemporalDeck::CARTRIDGE_STANTON_680HP;
  case TemporalDeck::CARTRIDGE_STANTON_680HP:
    return TemporalDeck::CARTRIDGE_LOFI;
  case TemporalDeck::CARTRIDGE_LOFI:
  default:
    return TemporalDeck::CARTRIDGE_CLEAN;
  }
}


struct DeckRateQuantity : ParamQuantity {
  static float valueForSpeed(float speed) {
    speed = clamp(speed, 0.5f, 2.f);
    if (speed <= 1.f) {
      return speed - 0.5f;
    }
    return speed * 0.5f;
  }

  float getDisplayValue() override { return TemporalDeckEngine::baseSpeedFromKnob(getValue()); }

  void setDisplayValue(float displayValue) override { setImmediateValue(valueForSpeed(displayValue)); }

  std::string getDisplayValueString() override {
    return string::f("%.2fx", TemporalDeckEngine::baseSpeedFromKnob(getValue()));
  }
};

struct ScratchSensitivityQuantity : ParamQuantity {
  static float sensitivityForValue(float v) {
    if (v <= 0.5f) {
      return rescale(v, 0.f, 0.5f, 0.5f, 1.f);
    }
    return rescale(v, 0.5f, 1.f, 1.f, 2.f);
  }

  std::string getDisplayValueString() override { return string::f("%.2fx", sensitivityForValue(getValue())); }

  std::string getLabel() override { return "Scratch sensitivity"; }
};

} // namespace

struct TemporalDeck::Impl {
  TemporalDeckEngine engine;
  dsp::SchmittTrigger freezeTrigger;
  dsp::SchmittTrigger freezeCvTrigger;
  dsp::SchmittTrigger reverseTrigger;
  dsp::SchmittTrigger reverseCvTrigger;
  dsp::SchmittTrigger slipTrigger;
  dsp::SchmittTrigger cartridgeCycleTrigger;
  float cachedSampleRate = 0.f;
  temporaldeck_transport::TransportControlState transportControl;
  temporaldeck_lifecycle::TemporalDeckSampleLifecycle sampleLifecycle;
  std::unique_ptr<temporaldeck::LongPlayStreamEngine> longPlayOwner;
  std::atomic<temporaldeck::LongPlayStreamEngine *> longPlayStream{nullptr};
  LongPlayBridge longPlayBridge;
  std::atomic<bool> longPlayInstallPending{false};
  bool longPlayStartupHold = false;
  uint64_t installedLongPlayGeneration = 0u;
  std::atomic<bool> sampleModeEnabled{false};
  std::atomic<bool> sampleLoopEnabled{false};
  PlatterInputState platterInput;
  std::atomic<bool> pendingLiveToSampleConvert{false};
  std::atomic<float> pendingSampleSeekNormalized{0.f};
  std::atomic<uint32_t> pendingSampleSeekRevision{0};
  uint32_t appliedSampleSeekRevision = 0;
  std::atomic<float> pendingLiveSeekArcNormalized{0.f};
  std::atomic<uint32_t> pendingLiveSeekRevision{0};
  uint32_t appliedLiveSeekRevision = 0;
  std::atomic<double> uiLagSamples{0.0};
  std::atomic<double> uiAccessibleLagSamples{0.0};
  std::atomic<float> uiSampleRate{44100.f};
  std::atomic<uint64_t> perfAudioSampledCount{0u};
  std::atomic<uint64_t> perfAudioProcessNs{0u};
  std::atomic<uint64_t> perfAudioProcessMinNs{std::numeric_limits<uint64_t>::max()};
  std::atomic<uint64_t> perfAudioProcessMaxNs{0u};
  uint32_t debugInstanceId = 0u;
  std::atomic<float> uiDrawCostUs{0.f};
  std::atomic<float> uiScopePreviewCostUs{0.f};
  std::atomic<int> uiScopePreviewStride{0};
  std::atomic<bool> uiScopePreviewMetricValid{false};
  std::atomic<float> uiPlatterAngle{0.f};
  std::atomic<bool> uiFreezeLatched{false};
  std::atomic<bool> uiSampleModeEnabled{false};
  std::atomic<bool> uiSampleLoaded{false};
  std::atomic<bool> uiSampleTransportPlaying{false};
  std::atomic<double> uiSamplePlayheadSeconds{0.0};
  std::atomic<double> uiSampleDurationSeconds{0.0};
  std::atomic<double> uiSampleProgress{0.0};
  float uiPublishTimerSec = 0.f;
  uint64_t expanderPublishSeq = 0;
  float expanderPublishTimerSec = 0.f;
  bool expanderWasConnected = false;
  bool expanderPreviewValid = false;
  uint64_t expanderLastPublishedGeneration = 0;
  bool expanderInputLWasConnected = false;
  bool expanderInputRWasConnected = false;
  bool expanderStereoAutoPromotePending = false;
  bool expanderStereoSampleWasLoaded = false;
  bool expanderAttachChannelSyncPending = false;
  bool expanderAttachChannelSyncStereo = false;
  ScopeWindowCache expanderScopeCacheMono;
  ScopeWindowCache expanderScopeCacheRight;
  bool expanderScopeLagHoldActive = false;
  float expanderScopeLagHoldSamples = 0.f;
  bool expanderScopeNewestPosHoldActive = false;
  double expanderScopeNewestPosHold = 0.0;
  double expanderScopeNewestAbsolutePosHold = 0.0;
  std::array<temporaldeck_expander::DisplayToHost, 2> expanderRequestMessages;
  uint32_t expanderRequestedScopeFormat = temporaldeck_expander::SCOPE_FORMAT_MONO;
  bool expanderLagDragWasActive = false;
  bool expanderLagDragRequestSeen = false;
  uint64_t expanderLagDragLastRequestSeq = 0u;
  float expanderLagDragLastLagSamples = 0.f;
  float expanderLagDragAnchorLagSamples = 0.f;
  int expanderLagDragFramesSinceUpdate = 0;
  bool expanderLagDragHasLastRequestTime = false;
  double expanderLagDragLastRequestTimeSec = 0.0;
  bool expanderLagDragLastStationaryHold = false;
  bool expanderLagDragHoldAnchorActive = false;
  float expanderLagDragHoldAnchorSamples = 0.f;
  int scratchInterpolationMode = TemporalDeck::SCRATCH_INTERP_LAGRANGE6;
  bool highQualityRateInterpolation = false;
  std::atomic<bool> platterTraceLoggingEnabled{false};
  std::atomic<bool> scopeDragTraceLoggingEnabled{false};
  bool scopeDragTraceCaptureActive = false;
  double scopeDragTraceStartTimeSec = 0.0;
  uint64_t scopeDragTraceSequence = 0;
  bool scopeDragTraceWasActive = false;
  bool scopeDragTraceHavePrev = false;
  float scopeDragTracePrevTargetLag = 0.f;
  float scopeDragTracePrevFrameLag = 0.f;
  int scopeDragTraceStallFrames = 0;
  float scopeDragTraceLogTimerSec = 0.f;
  static constexpr uint32_t scopeDragTraceQueueCapacity = 1024u;
  std::array<TemporalDeck::ScopeDragTraceEvent, scopeDragTraceQueueCapacity> scopeDragTraceQueue;
  std::atomic<uint32_t> scopeDragTraceQueueWrite{0u};
  std::atomic<uint32_t> scopeDragTraceQueueRead{0u};
  std::atomic<uint32_t> scopeDragTraceDropped{0u};
  bool pendingSampleStateApplyDeferralLogged = false;
  int cartridgeCharacter = TemporalDeck::CARTRIDGE_CLEAN;
  std::atomic<int> bufferDurationMode{TemporalDeck::BUFFER_DURATION_10S};
  int lastRamBufferDurationMode = TemporalDeck::BUFFER_DURATION_10S;
  int externalGatePosMode = TemporalDeck::EXTERNAL_GATE_POS_GLIDE;
  int freezeCvMode = TemporalDeck::FREEZE_CV_MODE_GATE;
  int reverseCvMode = TemporalDeck::REVERSE_CV_MODE_GATE;
  int platterArtMode = TemporalDeck::PLATTER_ART_DRAGON_KING;
  int platterBrightnessMode = TemporalDeck::PLATTER_BRIGHTNESS_FULL;
  std::string customPlatterArtPath;
  bool pendingInitialPlatterArtSelection = true;
  bool pendingLegacySampleFreezeOnPreparedInstall = false;
};

using ProcessSignalInputs = temporaldeck_frameinput::SignalInputs;

static ProcessSignalInputs readProcessSignalInputs(TemporalDeck &module) {
  ProcessSignalInputs in;
  in.inL = module.inputs[TemporalDeck::INPUT_L_INPUT].getVoltage();
  in.inR = module.inputs[TemporalDeck::INPUT_R_INPUT].isConnected()
             ? module.inputs[TemporalDeck::INPUT_R_INPUT].getVoltage()
             : in.inL;
  in.positionCv = module.inputs[TemporalDeck::POSITION_CV_INPUT].getVoltage();
  in.rateCv = module.inputs[TemporalDeck::RATE_CV_INPUT].getVoltage();
  in.rateCvConnected = module.inputs[TemporalDeck::RATE_CV_INPUT].isConnected();
  in.freezeGateHigh =
    module.inputs[TemporalDeck::FREEZE_GATE_INPUT].getVoltage() >= TemporalDeckEngine::kFreezeGateThreshold;
  in.scratchGateHigh =
    module.inputs[TemporalDeck::SCRATCH_GATE_INPUT].getVoltage() >= TemporalDeckEngine::kScratchGateThreshold;
  in.scratchGateConnected = module.inputs[TemporalDeck::SCRATCH_GATE_INPUT].isConnected();
  in.positionConnected = module.inputs[TemporalDeck::POSITION_CV_INPUT].isConnected();
  return in;
}

static void writeFrameOutputs(TemporalDeck &module, const TemporalDeckEngine::FrameResult &frame) {
  module.outputs[TemporalDeck::OUTPUT_L_OUTPUT].setVoltage(frame.outL);
  module.outputs[TemporalDeck::S_GATE_O_OUTPUT].setVoltage(frame.scratchGateOut);
  module.outputs[TemporalDeck::OUTPUT_R_OUTPUT].setVoltage(frame.outR);
  module.outputs[TemporalDeck::S_POS_O_OUTPUT].setVoltage(frame.scratchPosOut);
}

static void updateTransportModeLights(TemporalDeck &module, bool freezeActive, bool reverseLatched, bool slipLatched,
                                      int slipReturnMode) {
  module.lights[TemporalDeck::FREEZE_LIGHT].setBrightness(freezeActive ? 1.f : 0.f);
  module.lights[TemporalDeck::REVERSE_LIGHT].setBrightness(reverseLatched ? 1.f : 0.f);
  if (!slipLatched) {
    module.lights[TemporalDeck::SLIP_SLOW_LIGHT].setBrightness(0.f);
    module.lights[TemporalDeck::SLIP_LIGHT].setBrightness(0.f);
    module.lights[TemporalDeck::SLIP_FAST_LIGHT].setBrightness(0.f);
    return;
  }
  float selectedModeBrightness = 1.f;
  float unselectedModeBrightness = 0.03f;
  module.lights[TemporalDeck::SLIP_SLOW_LIGHT].setBrightness(
    slipReturnMode == TemporalDeck::SLIP_RETURN_SLOW ? selectedModeBrightness : unselectedModeBrightness);
  module.lights[TemporalDeck::SLIP_LIGHT].setBrightness(
    slipReturnMode == TemporalDeck::SLIP_RETURN_NORMAL ? selectedModeBrightness : unselectedModeBrightness);
  module.lights[TemporalDeck::SLIP_FAST_LIGHT].setBrightness(
    slipReturnMode == TemporalDeck::SLIP_RETURN_INSTANT ? selectedModeBrightness : unselectedModeBrightness);
}

namespace temporaldeck_ui {

static void applyArcLightState(TemporalDeck *module, const ArcLightState &state) {
  static_assert(TemporalDeck::kArcLightCount == kTemporalDeckArcLightCount, "Arc light count mismatch");
  for (int i = 0; i < TemporalDeck::kArcLightCount; ++i) {
    module->lights[TemporalDeck::ARC_LIGHT_START + i].setBrightness(state.yellow[i]);
    module->lights[TemporalDeck::ARC_MAX_LIGHT_START + i].setBrightness(state.red[i]);
  }
}

void publishArcLights(TemporalDeck *module, int sampleFrames, float maxLagSamples, bool sampleMode, bool sampleLoaded,
                      double lag, double accessibleLag, double sampleProgress) {
  ArcLightState state =
    computeArcLightState(sampleFrames, maxLagSamples, sampleMode, sampleLoaded, lag, accessibleLag, sampleProgress);
  applyArcLightState(module, state);
}

} // namespace temporaldeck_ui

TemporalDeck::TemporalDeck() : impl(new Impl()) {
  impl->debugInstanceId = gTemporalDeckDebugInstanceCounter.fetch_add(1u, std::memory_order_relaxed);
  config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
  // Expander contract (TemporalDeck host side):
  // - TemporalDeck publishes HostToDisplay payloads to TD.Scope by writing to
  //   TD.Scope's leftExpander.producerMessage and flipping that message.
  // - TD.Scope publishes DisplayToHost requests by writing to TemporalDeck's
  //   rightExpander.producerMessage and flipping it; TemporalDeck consumes
  //   those requests from rightExpander.consumerMessage.
  rightExpander.producerMessage = &impl->expanderRequestMessages[0];
  rightExpander.consumerMessage = &impl->expanderRequestMessages[1];
  configParam(BUFFER_PARAM, 0.f, 1.f, 1.f, "Buffer", " s", 0.f, 10.f);
  configParam<DeckRateQuantity>(RATE_PARAM, 0.f, 1.f, 0.5f, "Rate");
  configParam<ScratchSensitivityQuantity>(SCRATCH_SENSITIVITY_PARAM, 0.f, 1.f, 0.5f, "Scratch sensitivity");
  configParam(MIX_PARAM, 0.f, 1.f, 1.f, "Mix");
  configParam(FEEDBACK_PARAM, 0.f, 1.f, 0.f, "Feedback");
  configButton(FREEZE_PARAM, "Freeze");
  configButton(REVERSE_PARAM, "Reverse");
  configButton(SLIP_PARAM, "Slip");
  configButton(CARTRIDGE_CYCLE_PARAM, "Cycle cartridge");
  configButton(ADD_SCOPE_PARAM, "Spawn TD.Scope");
  configInput(POSITION_CV_INPUT, "Position CV");
  configInput(RATE_CV_INPUT, "Rate CV");
  configInput(INPUT_L_INPUT, "Left audio");
  configInput(INPUT_R_INPUT, "Right audio");
  configInput(SCRATCH_GATE_INPUT, "Scratch gate");
  configInput(FREEZE_GATE_INPUT, "Freeze gate");
  configInput(REVERSE_CV_INPUT, "Reverse gate");
  configOutput(OUTPUT_L_OUTPUT, "Left audio");
  configOutput(S_GATE_O_OUTPUT, "Scratch gate");
  configOutput(OUTPUT_R_OUTPUT, "Right audio");
  configOutput(S_POS_O_OUTPUT, "Scratch position");
  if (paramQuantities[BUFFER_PARAM]) {
    int mode = clamp(impl->bufferDurationMode.load(), 0, BUFFER_DURATION_COUNT - 1);
    paramQuantities[BUFFER_PARAM]->displayMultiplier = usableBufferSecondsForMode(mode);
  }
  impl->sampleLifecycle.startWorker();
  applySampleRateChange(APP->engine->getSampleRate());
}

TemporalDeck::~TemporalDeck() {
  teardownTimer.begin(id);
  if (!impl) {
    return;
  }

  const bool lifetimeLogging = isTemporalDeckLifetimeLoggingEnabled();
  const auto shutdownStart = std::chrono::steady_clock::now();
  uint32_t debugInstanceId = 0u;
  std::string samplePath;
  bool sampleLoaded = false;
  int sampleFrames = 0;
  int bufferSize = 0;
  size_t engineSampleBytes = 0u;
  size_t lifecycleDecodedBytes = 0u;
  size_t lifecyclePreparedBytes = 0u;
  bool decodedAvailable = false;
  bool buildInProgressBeforeStop = false;

  if (lifetimeLogging) {
    debugInstanceId = impl->debugInstanceId;
    impl->sampleLifecycle.sampleJsonSnapshot(&samplePath);
    sampleLoaded = impl->engine.sampleLoaded;
    sampleFrames = impl->engine.sampleFrames;
    bufferSize = impl->engine.buffer.size;
    engineSampleBytes = (impl->engine.buffer.left.capacity() + impl->engine.buffer.right.capacity()) * sizeof(float);
    impl->sampleLifecycle.sampleMemorySnapshot(&lifecycleDecodedBytes, &lifecyclePreparedBytes);
    decodedAvailable = impl->sampleLifecycle.decodedSampleAvailable();
    buildInProgressBeforeStop = impl->sampleLifecycle.sampleBuildInProgress();
  }

  const auto stopStart = std::chrono::steady_clock::now();
  impl->sampleLifecycle.stopWorker();
  const auto stopEnd = std::chrono::steady_clock::now();

  const auto resetStart = std::chrono::steady_clock::now();
  impl.reset();
  const auto resetEnd = std::chrono::steady_clock::now();

  if (lifetimeLogging) {
    appendTemporalDeckLifetimeShutdownLog(debugInstanceId,
                                          samplePath,
                                          sampleLoaded,
                                          sampleFrames,
                                          bufferSize,
                                          engineSampleBytes,
                                          lifecycleDecodedBytes,
                                          lifecyclePreparedBytes,
                                          decodedAvailable,
                                          buildInProgressBeforeStop,
                                          elapsedMs(stopStart, stopEnd),
                                          elapsedMs(resetStart, resetEnd),
                                          elapsedMs(shutdownStart, resetEnd));
  }
}

float TemporalDeck::scratchSensitivity() {
  return ScratchSensitivityQuantity::sensitivityForValue(params[SCRATCH_SENSITIVITY_PARAM].getValue());
}

void TemporalDeck::applyBufferDurationMode(int mode) {
  int clamped = clamp(mode, 0, BUFFER_DURATION_COUNT - 1);
  if (!temporaldeck_modes::isRamBufferMode(clamped)) {
    return;
  }
  impl->lastRamBufferDurationMode = clamped;
  impl->bufferDurationMode.store(clamped);
  if (paramQuantities[BUFFER_PARAM]) {
    paramQuantities[BUFFER_PARAM]->displayMultiplier = usableBufferSecondsForMode(clamped);
  }
}

void TemporalDeck::applySampleRateChange(float sampleRate) {
  impl->cachedSampleRate = sampleRate;
  int mode = clamp(impl->bufferDurationMode.load(), 0, BUFFER_DURATION_COUNT - 1);
  bool sampleModeEnabled = impl->sampleModeEnabled.load(std::memory_order_relaxed);
  bool sampleLoopEnabled = impl->sampleLoopEnabled.load(std::memory_order_relaxed);
  auto applyUiState = [&](int uiMode) {
    impl->uiSampleRate.store(impl->cachedSampleRate);
    impl->uiLagSamples.store(0.0);
    impl->uiAccessibleLagSamples.store(0.0);
    impl->uiPlatterAngle.store(0.f);
    impl->uiFreezeLatched.store(false);
    impl->uiSampleModeEnabled.store(impl->engine.sampleModeEnabled);
    impl->uiSampleLoaded.store(impl->engine.sampleLoaded);
    impl->uiSampleTransportPlaying.store(impl->engine.sampleTransportPlaying);
    impl->uiSamplePlayheadSeconds.store(0.0);
    impl->uiSampleDurationSeconds.store(
      impl->engine.sampleLoaded ? double(impl->engine.sampleFrames) / std::max(double(impl->cachedSampleRate), 1.0) : 0.0);
    impl->uiSampleProgress.store(0.0);
    if (paramQuantities[BUFFER_PARAM]) {
      float displaySeconds = usableBufferSecondsForMode(uiMode);
      if (impl->engine.sampleLoaded && impl->engine.sampleFrames > 0) {
        displaySeconds = float(impl->engine.sampleFrames) / std::max(impl->cachedSampleRate, 1.f);
      }
      paramQuantities[BUFFER_PARAM]->displayMultiplier = displaySeconds;
    }
    impl->uiPublishTimerSec = 0.f;
    impl->platterInput.resetAudioHoldState();
    for (int i = 0; i < kArcLightCount; ++i) {
      lights[ARC_LIGHT_START + i].setBrightness(0.f);
      lights[ARC_MAX_LIGHT_START + i].setBrightness(0.f);
    }
  };

  try {
    impl->engine.bufferDurationMode = mode;
    impl->engine.reset(impl->cachedSampleRate);
    impl->engine.sampleModeEnabled = sampleModeEnabled;
    impl->engine.sampleLoopEnabled = sampleLoopEnabled;
    impl->engine.highQualityRateInterpolation = impl->highQualityRateInterpolation;
    impl->engine.externalGatePosMode = impl->externalGatePosMode;
  } catch (const std::bad_alloc &) {
    WARN("TemporalDeck: buffer allocation failed, forcing 10s live fallback");
    impl->sampleLifecycle.clearDecodedAndPreparedState();
    impl->sampleModeEnabled.store(false, std::memory_order_relaxed);
    impl->bufferDurationMode.store(BUFFER_DURATION_10S, std::memory_order_relaxed);
    mode = BUFFER_DURATION_10S;
    impl->engine.bufferDurationMode = mode;
    impl->engine.reset(impl->cachedSampleRate);
    impl->engine.sampleModeEnabled = false;
    impl->engine.sampleLoopEnabled = sampleLoopEnabled;
    impl->engine.highQualityRateInterpolation = impl->highQualityRateInterpolation;
    impl->engine.externalGatePosMode = impl->externalGatePosMode;
  }
  applyUiState(mode);
}

void TemporalDeck::onSampleRateChange() {
  // process() publishes a POD runtime-build request. The sample worker creates
  // complete storage; audio only swaps it into the engine when ready.
}

json_t *TemporalDeck::dataToJson() {
  json_t *root = json_object();
  bool sampleModeEnabled = impl->sampleModeEnabled.load(std::memory_order_relaxed);
  std::string samplePath;
  impl->sampleLifecycle.sampleJsonSnapshot(&samplePath);
  json_object_set_new(root, "freezeLatched", json_boolean(impl->transportControl.freezeLatched));
  json_object_set_new(root, "reverseLatched", json_boolean(impl->transportControl.reverseLatched));
  json_object_set_new(root, "slipLatched", json_boolean(impl->transportControl.slipLatched));
  json_object_set_new(root, "scratchInterpolationMode", json_integer(impl->scratchInterpolationMode));
  json_object_set_new(root, "highQualityRateInterpolation", json_boolean(impl->highQualityRateInterpolation));
  json_object_set_new(root, "externalGatePosMode", json_integer(impl->externalGatePosMode));
  json_object_set_new(root, "freezeCvMode", json_integer(impl->freezeCvMode));
  json_object_set_new(root, "reverseCvMode", json_integer(impl->reverseCvMode));
  json_object_set_new(root, "slipReturnMode", json_integer(impl->transportControl.slipReturnMode));
  json_object_set_new(root, "cartridgeCharacter", json_integer(impl->cartridgeCharacter));
  json_object_set_new(root, "bufferDurationMode", json_integer(impl->bufferDurationMode.load()));
  json_object_set_new(root, "lastRamBufferDurationMode", json_integer(impl->lastRamBufferDurationMode));
  json_object_set_new(root, "sampleModeEnabled", json_boolean(sampleModeEnabled));
  json_object_set_new(root, "sampleLoopEnabled", json_boolean(impl->sampleLoopEnabled.load(std::memory_order_relaxed)));
  json_object_set_new(root, "platterArtMode", json_integer(impl->platterArtMode));
  json_object_set_new(root, "platterBrightnessMode", json_integer(impl->platterBrightnessMode));
  if (!impl->customPlatterArtPath.empty()) {
    json_object_set_new(root, "customPlatterArtPath", json_string(impl->customPlatterArtPath.c_str()));
  }
  if (!samplePath.empty()) {
    json_object_set_new(root, "samplePath", json_string(samplePath.c_str()));
  }
  return root;
}

void TemporalDeck::dataFromJson(json_t *root) {
  if (!root) {
    return;
  }
  impl->pendingInitialPlatterArtSelection = false;
  json_t *freezeJ = json_object_get(root, "freezeLatched");
  json_t *reverseJ = json_object_get(root, "reverseLatched");
  json_t *slipJ = json_object_get(root, "slipLatched");
  json_t *scratchInterpModeJ = json_object_get(root, "scratchInterpolationMode");
  json_t *highQualityRateInterpJ = json_object_get(root, "highQualityRateInterpolation");
  json_t *externalGatePosModeJ = json_object_get(root, "externalGatePosMode");
  json_t *freezeCvModeJ = json_object_get(root, "freezeCvMode");
  json_t *reverseCvModeJ = json_object_get(root, "reverseCvMode");
  json_t *slipReturnModeJ = json_object_get(root, "slipReturnMode");
  json_t *cartridgeJ = json_object_get(root, "cartridgeCharacter");
  json_t *bufferDurationJ = json_object_get(root, "bufferDurationMode");
  json_t *lastRamBufferDurationJ = json_object_get(root, "lastRamBufferDurationMode");
  json_t *sampleModeEnabledJ = json_object_get(root, "sampleModeEnabled");
  json_t *sampleLoopEnabledJ = json_object_get(root, "sampleLoopEnabled");
  json_t *sampleAutoPlayOnLoadJ = json_object_get(root, "sampleAutoPlayOnLoad");
  json_t *platterArtModeJ = json_object_get(root, "platterArtMode");
  json_t *platterBrightnessModeJ = json_object_get(root, "platterBrightnessMode");
  json_t *customPlatterArtPathJ = json_object_get(root, "customPlatterArtPath");
  json_t *samplePathJ = json_object_get(root, "samplePath");
  if (freezeJ) {
    impl->transportControl.freezeLatched = json_boolean_value(freezeJ);
    impl->transportControl.freezeLatchedByButton = impl->transportControl.freezeLatched;
  }
  if (reverseJ) {
    impl->transportControl.reverseLatched = json_boolean_value(reverseJ);
  }
  if (slipJ) {
    impl->transportControl.slipLatched = json_boolean_value(slipJ);
  }
  if (scratchInterpModeJ) {
    impl->scratchInterpolationMode =
      clamp((int)json_integer_value(scratchInterpModeJ), SCRATCH_INTERP_CUBIC, SCRATCH_INTERP_COUNT - 1);
  }
  if (highQualityRateInterpJ) {
    impl->highQualityRateInterpolation = json_boolean_value(highQualityRateInterpJ);
  }
  if (externalGatePosModeJ) {
    impl->externalGatePosMode =
      clamp((int)json_integer_value(externalGatePosModeJ), EXTERNAL_GATE_POS_GLIDE, EXTERNAL_GATE_POS_COUNT - 1);
  }
  if (freezeCvModeJ) {
    impl->freezeCvMode =
      clamp((int)json_integer_value(freezeCvModeJ), FREEZE_CV_MODE_PULSED, FREEZE_CV_MODE_COUNT - 1);
  }
  if (reverseCvModeJ) {
    impl->reverseCvMode =
      clamp((int)json_integer_value(reverseCvModeJ), REVERSE_CV_MODE_PULSED, REVERSE_CV_MODE_COUNT - 1);
  }
  if (slipReturnModeJ) {
    impl->transportControl.slipReturnMode = clamp((int)json_integer_value(slipReturnModeJ), SLIP_RETURN_SLOW, SLIP_RETURN_COUNT - 1);
  }
  if (cartridgeJ) {
    impl->cartridgeCharacter = clamp((int)json_integer_value(cartridgeJ), 0, CARTRIDGE_COUNT - 1);
  }
  if (bufferDurationJ) {
    int restoredMode = clamp((int)json_integer_value(bufferDurationJ), 0, BUFFER_DURATION_COUNT - 1);
    impl->bufferDurationMode.store(restoredMode);
    if (temporaldeck_modes::isRamBufferMode(restoredMode)) {
      impl->lastRamBufferDurationMode = restoredMode;
    }
  }
  if (lastRamBufferDurationJ) {
    impl->lastRamBufferDurationMode = temporaldeck_modes::sanitizeRamBufferMode(
      int(json_integer_value(lastRamBufferDurationJ)), impl->lastRamBufferDurationMode);
  }
  if (sampleModeEnabledJ) {
    impl->sampleModeEnabled.store(json_boolean_value(sampleModeEnabledJ), std::memory_order_relaxed);
  }
  if (sampleLoopEnabledJ) {
    impl->sampleLoopEnabled.store(json_boolean_value(sampleLoopEnabledJ), std::memory_order_relaxed);
  }
  if (platterArtModeJ) {
    impl->platterArtMode =
      clamp((int)json_integer_value(platterArtModeJ), PLATTER_ART_BUILTIN_SVG, PLATTER_ART_MODE_COUNT - 1);
  }
  if (platterBrightnessModeJ) {
    impl->platterBrightnessMode =
      clamp((int)json_integer_value(platterBrightnessModeJ), PLATTER_BRIGHTNESS_FULL, PLATTER_BRIGHTNESS_COUNT - 1);
  }
  if (customPlatterArtPathJ && json_is_string(customPlatterArtPathJ)) {
    impl->customPlatterArtPath = json_string_value(customPlatterArtPathJ);
  }
  if (!isDragonKingDebugEnabled()) {
    bool preserveManagedCustom =
      (impl->platterArtMode == PLATTER_ART_CUSTOM) && isManagedVinylArtPath(impl->customPlatterArtPath);
    if (impl->platterArtMode == PLATTER_ART_CUSTOM && !preserveManagedCustom) {
      impl->platterArtMode = PLATTER_ART_DRAGON_KING;
    }
    if (!preserveManagedCustom) {
      impl->customPlatterArtPath.clear();
    }
  }
  int mode = clamp(impl->bufferDurationMode.load(), 0, BUFFER_DURATION_COUNT - 1);
  if (paramQuantities[BUFFER_PARAM]) {
    paramQuantities[BUFFER_PARAM]->displayMultiplier = usableBufferSecondsForMode(mode);
  }
  if (samplePathJ && json_is_string(samplePathJ)) {
    impl->pendingLegacySampleFreezeOnPreparedInstall =
      sampleAutoPlayOnLoadJ && !json_is_true(sampleAutoPlayOnLoadJ);
    std::string error;
    loadSampleFromPath(json_string_value(samplePathJ), &error);
  }
  // Ensure restored patch state (buffer mode, sample state, etc.) is applied
  // to the runtime buffer allocation on the first audio process callback.
  impl->sampleLifecycle.setPendingSampleStateApply();
}

void TemporalDeck::process(const ProcessArgs &args) {
  const bool perfTimingEnabled = isDragonKingDebugEnabled();
  const auto processStart = perfTimingEnabled ? std::chrono::steady_clock::now() : std::chrono::steady_clock::time_point{};

  if (impl->sampleLifecycle.consumeAllocationFallbackPending()) {
    impl->sampleLifecycle.requestClearDecodedAndPreparedStateFromAudio();
    impl->sampleModeEnabled.store(false, std::memory_order_relaxed);
    impl->bufferDurationMode.store(BUFFER_DURATION_10S, std::memory_order_relaxed);
    impl->sampleLifecycle.setPendingSampleStateApply();
  }

  if (impl->longPlayInstallPending.load(std::memory_order_acquire)) {
    temporaldeck::LongPlayStreamEngine *stream =
      impl->longPlayStream.load(std::memory_order_acquire);
    const int maximumGuardFrames = std::max(1, int(std::ceil(args.sampleRate * 1.1f)));
    if (stream && stream->ready() && !impl->sampleLifecycle.sampleBuildInProgress()) {
      if (impl->engine.buffer.size <= maximumGuardFrames) {
        const uint64_t generation = stream->generation();
        if (generation != impl->installedLongPlayGeneration) {
          const double sourceRate = std::max<double>(stream->sampleRate(), 1.0);
          const double outputRate = std::max<double>(args.sampleRate, 1.0);
          const double logicalFrames = double(stream->totalFrames()) * outputRate / sourceRate;
          const int installedFrames = int(std::max(
            1.0, std::min(logicalFrames, double(std::numeric_limits<int>::max()))));
          impl->longPlayBridge.stream = stream;
          impl->longPlayBridge.sourceFramesPerOutputFrame = sourceRate / outputRate;
          impl->engine.bufferDurationMode = BUFFER_DURATION_LONGPLAY_DISK;
          impl->engine.installStreamedSample(
            &impl->longPlayBridge, &LongPlayBridge::readFrame,
            &LongPlayBridge::setDesiredFrame, &LongPlayBridge::isFrameResident,
            installedFrames,
            stream->absolutePeak() * temporaldeck::kSampleFileVoltageScale);
          // File-open readiness precedes the first decoded cache block. Hold
          // the restored transport at its initial frame until audio and scope
          // can both observe the same resident source data.
          impl->longPlayStartupHold = true;
          impl->bufferDurationMode.store(
            BUFFER_DURATION_LONGPLAY_DISK, std::memory_order_relaxed);
          impl->sampleModeEnabled.store(true, std::memory_order_relaxed);
          impl->installedLongPlayGeneration = generation;
          impl->longPlayInstallPending.store(false, std::memory_order_release);
          if (impl->pendingLegacySampleFreezeOnPreparedInstall) {
            impl->transportControl.freezeLatched = true;
            impl->transportControl.freezeLatchedByButton = false;
            impl->engine.sampleTransportPlaying = false;
            impl->uiSampleTransportPlaying.store(false, std::memory_order_relaxed);
            impl->pendingLegacySampleFreezeOnPreparedInstall = false;
          }
          if (paramQuantities[BUFFER_PARAM]) {
            paramQuantities[BUFFER_PARAM]->displayMultiplier =
              float(double(installedFrames) / outputRate);
          }
        }
      }
    } else if (stream && !stream->ready() && !stream->loading()) {
      impl->longPlayInstallPending.store(false, std::memory_order_release);
      impl->longPlayStartupHold = false;
    }
  }
  if (temporaldeck::LongPlayStreamEngine *stream =
        impl->longPlayStream.load(std::memory_order_acquire)) {
    if (impl->engine.diskBackedSample && stream->ready()) {
      impl->engine.sampleAbsolutePeakVolts =
        stream->absolutePeak() * temporaldeck::kSampleFileVoltageScale;
      impl->engine.streamResidencyGeneration = stream->residencyGeneration();
    }
  }

  bool installedPreparedSampleThisFrame = false;
  if (PreparedSampleData *preparedPtr = impl->sampleLifecycle.consumePendingPreparedSample()) {
    PreparedSampleData &prepared = *preparedPtr;
    std::string samplePath;
    size_t lifecycleDecodedBytes = 0u;
    size_t lifecyclePreparedBytes = 0u;
    if (isTemporalDeckLifetimeLoggingEnabled()) {
      impl->sampleLifecycle.sampleJsonSnapshot(&samplePath);
      impl->sampleLifecycle.sampleMemorySnapshot(&lifecycleDecodedBytes, &lifecyclePreparedBytes);
    }
    const auto processInstallStart = std::chrono::steady_clock::now();
    impl->cachedSampleRate = prepared.sampleRate;
    impl->bufferDurationMode.store(prepared.bufferMode, std::memory_order_relaxed);
    impl->engine.bufferDurationMode = prepared.bufferMode;
    const auto resetStart = std::chrono::steady_clock::now();
    impl->engine.reset(prepared.sampleRate, false);
    const auto resetEnd = std::chrono::steady_clock::now();
    impl->engine.sampleModeEnabled = impl->sampleModeEnabled.load(std::memory_order_relaxed);
    impl->engine.sampleLoopEnabled = impl->sampleLoopEnabled.load(std::memory_order_relaxed);
    impl->engine.externalGatePosMode = impl->externalGatePosMode;
    const auto installStart = std::chrono::steady_clock::now();
    impl->engine.installPreparedSample(std::move(prepared.left), std::move(prepared.right), prepared.frames,
                                       prepared.truncated, prepared.monoStorage, &prepared.preview,
                                       prepared.sampleAbsolutePeakVolts, prepared.previewValid);
    const auto installEnd = std::chrono::steady_clock::now();
    installedPreparedSampleThisFrame = true;
    impl->pendingSampleStateApplyDeferralLogged = false;
    impl->sampleModeEnabled.store(impl->engine.sampleLoaded, std::memory_order_relaxed);
    if (impl->pendingLegacySampleFreezeOnPreparedInstall) {
      impl->transportControl.freezeLatched = true;
      impl->transportControl.freezeLatchedByButton = false;
      impl->engine.sampleTransportPlaying = false;
      impl->uiSampleTransportPlaying.store(false, std::memory_order_relaxed);
      impl->pendingLegacySampleFreezeOnPreparedInstall = false;
    }
    if (paramQuantities[BUFFER_PARAM]) {
      paramQuantities[BUFFER_PARAM]->displayMultiplier = float(impl->engine.sampleFrames) / std::max(prepared.sampleRate, 1.f);
    }
    appendTemporalDeckLifetimeLoadingLog(impl->debugInstanceId,
                                         "process_install_prepared",
                                         samplePath,
                                         prepared.buildSerial,
                                         prepared.buildRequestType,
                                         prepared.sampleRate,
                                         prepared.bufferMode,
                                         prepared.sourceFrames,
                                         prepared.sourceChannels,
                                         prepared.frames,
                                         (impl->engine.buffer.left.capacity() + impl->engine.buffer.right.capacity()) * sizeof(float),
                                         lifecycleDecodedBytes,
                                         lifecyclePreparedBytes,
                                         impl->sampleLifecycle.decodedSampleAvailable(),
                                         impl->sampleLifecycle.sampleBuildInProgress(),
                                         prepared.workerDecodeMs,
                                         prepared.workerPrepMs,
                                         prepared.workerTotalMs,
                                         elapsedMs(resetStart, resetEnd),
                                         elapsedMs(installStart, installEnd),
                                         elapsedMs(processInstallStart, std::chrono::steady_clock::now()));
    impl->sampleLifecycle.retirePreparedSampleFromAudio(preparedPtr);
  }

  int requestedBufferMode = clamp(impl->bufferDurationMode.load(std::memory_order_relaxed), 0, BUFFER_DURATION_COUNT - 1);
  bool bufferModeChanged = requestedBufferMode != impl->engine.bufferDurationMode;
  bool sampleStateApplyRequested = impl->sampleLifecycle.consumePendingSampleStateApply();
  if (installedPreparedSampleThisFrame) {
    sampleStateApplyRequested = false;
  }
  bool sampleRateChanged = args.sampleRate != impl->cachedSampleRate;
  if (sampleRateChanged && impl->engine.diskBackedSample) {
    temporaldeck::LongPlayStreamEngine *stream =
      impl->longPlayStream.load(std::memory_order_acquire);
    if (stream && stream->ready()) {
      const bool loopEnabled = impl->sampleLoopEnabled.load(std::memory_order_relaxed);
      const double sourceRate = std::max<double>(stream->sampleRate(), 1.0);
      const double outputRate = std::max<double>(args.sampleRate, 1.0);
      const int installedFrames = int(std::max(1.0, std::min(
        double(stream->totalFrames()) * outputRate / sourceRate,
        double(std::numeric_limits<int>::max()))));
      impl->cachedSampleRate = args.sampleRate;
      impl->longPlayBridge.sourceFramesPerOutputFrame = sourceRate / outputRate;
      impl->engine.reset(args.sampleRate, false);
      impl->engine.bufferDurationMode = BUFFER_DURATION_LONGPLAY_DISK;
      impl->engine.sampleLoopEnabled = loopEnabled;
      impl->engine.installStreamedSample(
        &impl->longPlayBridge, &LongPlayBridge::readFrame,
        &LongPlayBridge::setDesiredFrame, &LongPlayBridge::isFrameResident,
        installedFrames, stream->absolutePeak() * temporaldeck::kSampleFileVoltageScale);
      impl->longPlayStartupHold = true;
      impl->engine.sampleModeEnabled = true;
      sampleRateChanged = false;
    }
  }
  bool decodedAvailable = impl->sampleLifecycle.decodedSampleAvailable();
  bool sampleBuildInProgress = impl->sampleLifecycle.sampleBuildInProgress();
  bool shouldApplyWithoutDecoded = !decodedAvailable && (bufferModeChanged || sampleRateChanged || sampleStateApplyRequested);
  if (shouldApplyWithoutDecoded && sampleBuildInProgress) {
    if (isTemporalDeckLifetimeLoggingEnabled() && !impl->pendingSampleStateApplyDeferralLogged) {
      std::string samplePath;
      size_t lifecycleDecodedBytes = 0u;
      size_t lifecyclePreparedBytes = 0u;
      impl->sampleLifecycle.sampleJsonSnapshot(&samplePath);
      impl->sampleLifecycle.sampleMemorySnapshot(&lifecycleDecodedBytes, &lifecyclePreparedBytes);
      appendTemporalDeckLifetimeLoadingLog(impl->debugInstanceId,
                                           "defer_sample_state_apply",
                                           samplePath,
                                           0u,
                                           0,
                                           args.sampleRate,
                                           requestedBufferMode,
                                           0,
                                           0,
                                           0,
                                           (impl->engine.buffer.left.capacity() + impl->engine.buffer.right.capacity()) * sizeof(float),
                                           lifecycleDecodedBytes,
                                           lifecyclePreparedBytes,
                                           decodedAvailable,
                                           sampleBuildInProgress,
                                           0.0,
                                           0.0,
                                           0.0,
                                           0.0,
                                           0.0,
                                           0.0,
                                           "sample worker active; deferring restored live-buffer allocation");
      impl->pendingSampleStateApplyDeferralLogged = true;
    }
    impl->sampleLifecycle.setPendingSampleStateApply();
    bufferModeChanged = false;
    sampleRateChanged = false;
    sampleStateApplyRequested = false;
    shouldApplyWithoutDecoded = false;
  }
  bool shouldRebuildLoadedSample = decodedAvailable && (bufferModeChanged || sampleRateChanged || sampleStateApplyRequested);
  if (shouldApplyWithoutDecoded) {
    temporaldeck_lifecycle::TemporalDeckSampleLifecycle::AsyncSampleBuildRequest request;
    request.type = temporaldeck_lifecycle::TemporalDeckSampleLifecycle::AsyncSampleBuildRequest::BUILD_EMPTY_BUFFER;
    request.targetSampleRate = args.sampleRate;
    request.requestedBufferMode = requestedBufferMode;
    impl->sampleLifecycle.requestAsyncRuntimeBuild(
      request.type, request.targetSampleRate, request.requestedBufferMode);
  } else if (shouldRebuildLoadedSample && !sampleBuildInProgress) {
    temporaldeck_lifecycle::TemporalDeckSampleLifecycle::AsyncSampleBuildRequest request;
    request.type = temporaldeck_lifecycle::TemporalDeckSampleLifecycle::AsyncSampleBuildRequest::REBUILD_FROM_DECODED;
    request.targetSampleRate = args.sampleRate;
    request.requestedBufferMode = requestedBufferMode;
    uint64_t requestSerial = impl->sampleLifecycle.requestAsyncRuntimeBuild(
      request.type, request.targetSampleRate, request.requestedBufferMode);
    appendTemporalDeckLifetimeLoadingLog(impl->debugInstanceId,
                                         "request_rebuild_from_decoded",
                                         impl->sampleLifecycle.samplePath(),
                                         requestSerial,
                                         request.type,
                                         request.targetSampleRate,
                                         request.requestedBufferMode,
                                         0,
                                         0,
                                         0,
                                         (impl->engine.buffer.left.capacity() + impl->engine.buffer.right.capacity()) * sizeof(float),
                                         0u,
                                         0u,
                                         decodedAvailable,
                                         true,
                                         0.0,
                                         0.0,
                                         0.0,
                                         0.0,
                                         0.0,
                                         0.0);
  }

  if (impl->pendingLiveToSampleConvert.exchange(false, std::memory_order_relaxed)) {
    if (impl->engine.convertLiveWindowToSample(params[BUFFER_PARAM].getValue())) {
      impl->sampleModeEnabled.store(true, std::memory_order_relaxed);
      if (paramQuantities[BUFFER_PARAM]) {
        paramQuantities[BUFFER_PARAM]->displayMultiplier =
          float(impl->engine.sampleFrames) / std::max(args.sampleRate, 1.f);
      }
    }
  }

  bool desiredSampleModeEnabled = impl->sampleModeEnabled.load(std::memory_order_relaxed);
  temporaldeck_transport::TransportButtonEvents transportButtons;
  bool freezeButtonPressed = impl->freezeTrigger.process(params[FREEZE_PARAM].getValue());
  bool freezeCvConnected = inputs[FREEZE_GATE_INPUT].isConnected();
  bool freezeCvHigh = inputs[FREEZE_GATE_INPUT].getVoltage() >= TemporalDeckEngine::kFreezeGateThreshold;
  bool freezeCvPressed = impl->freezeCvTrigger.process(inputs[FREEZE_GATE_INPUT].getVoltage());
  bool freezeGateModeActive = (impl->freezeCvMode == FREEZE_CV_MODE_GATE) && freezeCvConnected;
  transportButtons.freezePressed = freezeGateModeActive ? false : freezeButtonPressed;
  if (!freezeGateModeActive && freezeCvConnected && freezeCvPressed) {
    transportButtons.freezePressed = true;
  }
  bool reverseButtonPressed = impl->reverseTrigger.process(params[REVERSE_PARAM].getValue());
  bool reverseCvConnected = inputs[REVERSE_CV_INPUT].isConnected();
  bool reverseCvHigh = inputs[REVERSE_CV_INPUT].getVoltage() >= TemporalDeckEngine::kFreezeGateThreshold;
  bool reverseCvPressed = impl->reverseCvTrigger.process(inputs[REVERSE_CV_INPUT].getVoltage());
  bool reverseGateModeActive = (impl->reverseCvMode == REVERSE_CV_MODE_GATE) && reverseCvConnected;
  transportButtons.reversePressed =
    reverseGateModeActive ? false : (reverseButtonPressed || reverseCvPressed);
  transportButtons.slipPressed = impl->slipTrigger.process(params[SLIP_PARAM].getValue());
  temporaldeck_transport::TransportButtonResult transportResult = temporaldeck_transport::applyTransportButtonEvents(
    impl->transportControl, transportButtons, desiredSampleModeEnabled, impl->engine.sampleLoaded);

  if (reverseGateModeActive) {
    impl->transportControl.reverseLatched = reverseCvHigh;
    if (reverseCvHigh) {
      impl->transportControl.freezeLatched = false;
      impl->transportControl.freezeLatchedByButton = false;
      if (desiredSampleModeEnabled && impl->engine.sampleLoaded) {
        transportResult.forceSampleTransportPlay = true;
      }
    }
  }
  if (freezeGateModeActive) {
    impl->transportControl.freezeLatched = freezeCvHigh;
    impl->transportControl.freezeLatchedByButton = false;
    if (freezeCvHigh) {
      impl->transportControl.slipLatched = false;
    }
  }
  if (transportResult.forceSampleTransportPlay) {
    impl->engine.sampleTransportPlaying = true;
    impl->uiSampleTransportPlaying.store(true, std::memory_order_relaxed);
  }

  if (impl->cartridgeCycleTrigger.process(params[CARTRIDGE_CYCLE_PARAM].getValue())) {
    impl->cartridgeCharacter = nextCartridgeCharacter(impl->cartridgeCharacter);
  }

  ProcessSignalInputs signalIn = readProcessSignalInputs(*this);
  float inL = signalIn.inL;
  float inR = signalIn.inR;
  float positionCv = signalIn.positionCv;
  float rateCv = signalIn.rateCv;
  bool rateCvConnected = signalIn.rateCvConnected;
  bool freezeGateHigh = freezeGateModeActive && signalIn.freezeGateHigh;
  bool scratchGateHigh = signalIn.scratchGateHigh;
  bool scratchGateConnected = signalIn.scratchGateConnected;
  bool positionConnected = signalIn.positionConnected;
  if (freezeGateModeActive) {
    temporaldeck_transport::applyFreezeGateEdge(impl->transportControl, freezeGateHigh);
  } else {
    impl->transportControl.prevFreezeGateHigh = false;
  }

  impl->engine.scratchInterpolationMode = impl->scratchInterpolationMode;
  impl->engine.highQualityRateInterpolation = impl->highQualityRateInterpolation;
  impl->engine.slipReturnMode = impl->transportControl.slipReturnMode;
  impl->engine.externalGatePosMode = impl->externalGatePosMode;
  impl->engine.cartridgeCharacter = impl->cartridgeCharacter;
  impl->engine.sampleRate = args.sampleRate;
  impl->engine.sampleModeEnabled = desiredSampleModeEnabled;
  impl->engine.sampleLoopEnabled = impl->sampleLoopEnabled.load(std::memory_order_relaxed);
  uint32_t pendingSeekRevision = impl->pendingSampleSeekRevision.load(std::memory_order_relaxed);
  float pendingSeekNorm = impl->pendingSampleSeekNormalized.load(std::memory_order_relaxed);
  uint32_t pendingLiveSeekRevision = impl->pendingLiveSeekRevision.load(std::memory_order_relaxed);
  float pendingLiveSeekArcNorm = impl->pendingLiveSeekArcNormalized.load(std::memory_order_relaxed);
  float bufferKnob = params[BUFFER_PARAM].getValue();
  impl->engine.updateStreamActiveWindow(bufferKnob);
  impl->appliedSampleSeekRevision = temporaldeck_transport::applyPendingSampleSeek(
    impl->engine, impl->appliedSampleSeekRevision, pendingSeekRevision, pendingSeekNorm, bufferKnob);
  impl->appliedLiveSeekRevision = temporaldeck_transport::applyPendingLiveSeekArc(
    impl->engine, impl->appliedLiveSeekRevision, pendingLiveSeekRevision, pendingLiveSeekArcNorm, bufferKnob);
  impl->engine.requestStreamWindow();
  impl->engine.servicePendingStreamSeek();

  auto enqueueScopeTraceEvent = [&](const ScopeDragTraceEvent &event) {
    uint32_t write = impl->scopeDragTraceQueueWrite.load(std::memory_order_relaxed);
    uint32_t read = impl->scopeDragTraceQueueRead.load(std::memory_order_acquire);
    uint32_t next = (write + 1u) % Impl::scopeDragTraceQueueCapacity;
    if (next == read) {
      impl->scopeDragTraceDropped.fetch_add(1u, std::memory_order_relaxed);
      return false;
    }
    impl->scopeDragTraceQueue[size_t(write)] = event;
    impl->scopeDragTraceQueueWrite.store(next, std::memory_order_release);
    return true;
  };

  bool scopeTraceDebugEnabled = isDragonKingDebugEnabled();
  bool scopeDragTraceEnabled = impl->scopeDragTraceLoggingEnabled.load(std::memory_order_relaxed) && scopeTraceDebugEnabled;
  if (scopeDragTraceEnabled && !impl->scopeDragTraceCaptureActive) {
    impl->scopeDragTraceStartTimeSec = system::getTime();
    impl->scopeDragTraceSequence = 0;
    impl->scopeDragTraceCaptureActive = true;
    ScopeDragTraceEvent started;
    started.type = ScopeDragTraceEvent::EVENT_CAPTURE_STARTED;
    started.eventSeq = impl->scopeDragTraceSequence++;
    enqueueScopeTraceEvent(started);
  } else if (!scopeDragTraceEnabled && impl->scopeDragTraceCaptureActive) {
    ScopeDragTraceEvent stopped;
    stopped.type = ScopeDragTraceEvent::EVENT_CAPTURE_STOPPED;
    stopped.eventSeq = impl->scopeDragTraceSequence++;
    stopped.tSec = float(std::max(0.0, system::getTime() - impl->scopeDragTraceStartTimeSec));
    enqueueScopeTraceEvent(stopped);
    impl->scopeDragTraceCaptureActive = false;
    impl->scopeDragTraceStartTimeSec = 0.0;
    impl->scopeDragTraceSequence = 0;
    impl->scopeDragTraceWasActive = false;
    impl->scopeDragTraceHavePrev = false;
    impl->scopeDragTraceStallFrames = 0;
    impl->scopeDragTraceLogTimerSec = 0.f;
  }

  Module* right = rightExpander.module;
  bool expanderConnected =
    isTDScopeModule(right) && right->leftExpander.producerMessage;
  bool stereoSampleLoadedNow = impl->engine.sampleLoaded && !impl->engine.buffer.monoStorage;
  if (stereoSampleLoadedNow && !impl->expanderStereoSampleWasLoaded) {
    impl->expanderStereoAutoPromotePending = true;
  }
  impl->expanderStereoSampleWasLoaded = stereoSampleLoadedNow;
  if (expanderConnected) {
    bool inputLConnected = inputs[INPUT_L_INPUT].isConnected();
    bool inputRConnected = inputs[INPUT_R_INPUT].isConnected();
    int prevConnectedCount = int(impl->expanderInputLWasConnected) + int(impl->expanderInputRWasConnected);
    int nowConnectedCount = int(inputLConnected) + int(inputRConnected);
    bool justConnected = !impl->expanderWasConnected;
    if (justConnected) {
      impl->expanderAttachChannelSyncPending = true;
      impl->expanderAttachChannelSyncStereo = (nowConnectedCount == 2) || stereoSampleLoadedNow;
    }
    if (prevConnectedCount == 1 && nowConnectedCount == 2) {
      impl->expanderStereoAutoPromotePending = true;
    }
    impl->expanderInputLWasConnected = inputLConnected;
    impl->expanderInputRWasConnected = inputRConnected;
  } else {
    impl->expanderInputLWasConnected = false;
    impl->expanderInputRWasConnected = false;
    impl->expanderStereoAutoPromotePending = false;
    impl->expanderAttachChannelSyncPending = false;
    impl->expanderAttachChannelSyncStereo = false;
  }
  uint32_t requestedScopeFormat = temporaldeck_expander::SCOPE_FORMAT_MONO;
  bool haveLagDragRequest = false;
  bool lagDragRequestActive = false;
  bool lagDragRequestStationaryHold = false;
  bool lagDragRequestUsesNormalizedMotion = false;
  float lagDragRequestSamples = 0.f;
  float lagDragRequestVelocity = 0.f;
  float lagDragRequestNormalizedOffset = 0.f;
  float lagDragRequestNormalizedVelocity = 0.f;
  uint64_t lagDragRequestSeq = 0u;
  bool scopeTraceNewRequest = false;
  bool scopeTraceDragJustStarted = false;
  float scopeTraceLagTarget = 0.f;
  float scopeTraceVelocityApplied = 0.f;
  double processNowSec = system::getTime();
  if (isTDScopeModule(right) && rightExpander.consumerMessage) {
    // Request direction contract:
    // this is the TD.Scope -> TemporalDeck path. TD.Scope writes requests into
    // TemporalDeck's rightExpander.producerMessage and flips; host reads here.
    const auto *request = reinterpret_cast<const temporaldeck_expander::DisplayToHost *>(rightExpander.consumerMessage);
    ScopeInteractionRequest scopeRequest = decodeScopeInteractionRequest(request);
    if (scopeRequest.valid) {
      haveLagDragRequest = true;
      lagDragRequestSeq = scopeRequest.requestSeq;
      requestedScopeFormat = scopeRequest.requestedScopeFormat;
      lagDragRequestActive = scopeRequest.active;
      lagDragRequestStationaryHold = scopeRequest.stationaryHold;
      lagDragRequestUsesNormalizedMotion = scopeRequest.usesNormalizedMotion;
      lagDragRequestSamples = scopeRequest.lagSamples;
      lagDragRequestVelocity = scopeRequest.velocitySamples;
      lagDragRequestNormalizedOffset = scopeRequest.normalizedOffset;
      lagDragRequestNormalizedVelocity = scopeRequest.normalizedVelocity;
    }
  }
  if (haveLagDragRequest) {
    // Expander scope requests are treated as intent signals: lag target is
    // authoritative and optional velocity is advisory. Final scratch behavior
    // remains centralized in the host/engine path below.
    // WARNING: Sign contract for expander scope drag messages:
    // - `lagTarget` increases away from NOW (older audio).
    // - `lagDragRequestVelocity` uses scratch gesture convention:
    //     positive => toward NOW (decreasing lag)
    //     negative => away from NOW (increasing lag)
    // Any receive-side derivation or blending must preserve this convention.
    bool isNewLagRequest = !impl->expanderLagDragRequestSeen || lagDragRequestSeq != impl->expanderLagDragLastRequestSeq;
    if (isNewLagRequest) {
      impl->expanderLagDragRequestSeen = true;
      impl->expanderLagDragLastRequestSeq = lagDragRequestSeq;
      float maxLag = std::max(0.f, float(impl->uiAccessibleLagSamples.load(std::memory_order_relaxed)));
      float lagTarget = float(impl->engine.clampLag(lagDragRequestSamples, maxLag));
      bool dragJustStarted = lagDragRequestActive && !impl->expanderLagDragWasActive;
      bool scopeRequestStationary = lagDragRequestStationaryHold;
      bool scopeLiveReverseTargetCompensated = false;
      float requestDtSec = 0.f;
      if (lagDragRequestActive && impl->expanderLagDragHasLastRequestTime) {
        double rawDtSec = processNowSec - impl->expanderLagDragLastRequestTimeSec;
        if (std::isfinite(rawDtSec) && rawDtSec > 0.0) {
          constexpr float kMinRequestDtSec = 1.0f / 240.0f;
          constexpr float kMaxRequestDtSec = 1.0f / 20.0f;
          requestDtSec = clamp(float(rawDtSec), kMinRequestDtSec, kMaxRequestDtSec);
        }
      }
      if (lagDragRequestActive && lagDragRequestUsesNormalizedMotion) {
        bool reanchorDrag =
          dragJustStarted || (impl->expanderLagDragLastStationaryHold && !lagDragRequestStationaryHold);
        if (reanchorDrag) {
          float currentLag = float(impl->uiLagSamples.load(std::memory_order_relaxed));
          if (!std::isfinite(currentLag) || currentLag < 0.f) {
            currentLag = float(impl->engine.currentLagFromNewest(impl->engine.newestReadablePos()));
          }
          impl->expanderLagDragAnchorLagSamples = float(impl->engine.clampLag(currentLag, maxLag));
        }
        float scopeDragTravelSamples = platter_interaction::samplesPerRevolution(
                                         std::max(args.sampleRate, 1.f), TemporalDeck::kNominalPlatterRpm) *
                                       kScopeDragNominalTurnScale * scratchSensitivity() *
                                       TemporalDeck::kMouseScratchTravelScale;
        scopeDragTravelSamples = std::max(scopeDragTravelSamples, 1.f);
        lagTarget = float(impl->engine.clampLag(impl->expanderLagDragAnchorLagSamples + lagDragRequestNormalizedOffset * scopeDragTravelSamples,
                          maxLag));
        lagDragRequestVelocity = clamp(lagDragRequestNormalizedVelocity * scopeDragTravelSamples,
                                       -std::max(args.sampleRate * 3.0f, 1.0f),
                                       std::max(args.sampleRate * 3.0f, 1.0f));
        const bool scopeLiveDrag =
          !(desiredSampleModeEnabled && impl->engine.sampleLoaded) && !impl->transportControl.freezeLatched && !freezeGateHigh;
        if (scopeLiveDrag && !scopeRequestStationary && lagDragRequestNormalizedVelocity > 0.f) {
          const float sampleRate = std::max(args.sampleRate, 1.f);
          const float nearNowWindowSamples = sampleRate * kScopeLiveNowAssistWindowSec;
          const float referenceLag =
            clamp(dragJustStarted ? lagTarget : impl->expanderLagDragLastLagSamples, 0.f, nearNowWindowSamples);
          const float depthT = clamp(referenceLag / nearNowWindowSamples, 0.f, 1.f);
          const float nearNowT = 1.f - depthT;
          const float targetBoost = kScopeLiveNearNowTargetBoostBase +
                                     nearNowT * nearNowT * kScopeLiveNearNowTargetBoostExtra;
          const float assistDt = requestDtSec > 0.f ? requestDtSec : args.sampleTime;
          const float assistVelocity = sampleRate * targetBoost;
          lagTarget = clamp(lagTarget - assistVelocity * assistDt, 0.f, maxLag);
          lagDragRequestVelocity =
            clamp(lagDragRequestVelocity + assistVelocity, -std::max(args.sampleRate * 3.0f, 1.0f),
                  std::max(args.sampleRate * 3.0f, 1.0f));
        } else if (scopeLiveDrag && !scopeRequestStationary && lagDragRequestNormalizedVelocity < 0.f &&
                   !dragJustStarted) {
          const float sampleRate = std::max(args.sampleRate, 1.f);
          const float assistDt = requestDtSec > 0.f ? requestDtSec : args.sampleTime;
          const float reverseHandVelocitySamples = std::max(0.f, -lagDragRequestVelocity);
          const float reverseTargetFloor =
            clamp(impl->expanderLagDragLastLagSamples + (sampleRate + reverseHandVelocitySamples) * assistDt, 0.f, maxLag);
          lagTarget = std::max(lagTarget, reverseTargetFloor);
          scopeLiveReverseTargetCompensated = lagTarget >= reverseTargetFloor;
        }
        if (scopeRequestStationary && impl->expanderLagDragWasActive) {
          if (impl->expanderLagDragHoldAnchorActive) {
            lagTarget = float(impl->engine.clampLag(impl->expanderLagDragHoldAnchorSamples, maxLag));
          } else if (!impl->expanderLagDragLastStationaryHold && std::isfinite(impl->expanderLagDragLastLagSamples)) {
            lagTarget = float(impl->engine.clampLag(impl->expanderLagDragLastLagSamples, maxLag));
          }
        }
      }
      scopeTraceNewRequest = true;
      scopeTraceLagTarget = lagTarget;
      if (lagDragRequestActive) {
        scopeTraceDragJustStarted = dragJustStarted;
        auto applyScopeStationaryHold = [&](float lagTarget) {
          // Stationary scope touch is a read-head hold, not a fresh gesture.
          // Scope re-publishes hold requests while the mouse remains down, so
          // avoid setScratch() here; its gesture revision would make the engine
          // keep applying hybrid gesture correction even with zero velocity.
          float currentLag = float(impl->engine.currentLagFromNewest(impl->engine.newestReadablePos()));
          if (!std::isfinite(currentLag) || currentLag < 0.f) {
            currentLag = lagTarget;
          }
          if (!impl->expanderLagDragHoldAnchorActive || !impl->expanderLagDragLastStationaryHold) {
            impl->expanderLagDragHoldAnchorSamples =
              (impl->expanderLagDragWasActive && !impl->expanderLagDragLastStationaryHold) ? lagTarget : currentLag;
            impl->expanderLagDragHoldAnchorActive = true;
          }
          impl->platterInput.setTouchHold(true, impl->expanderLagDragHoldAnchorSamples);
          impl->platterInput.setMotionFreshSamples(0);
          scopeTraceVelocityApplied = 0.f;
        };
        if (scopeRequestStationary) {
          // Touch-down without motion should behave like a stationary platter
          // touch. Active movement arrives through setScopeLagDrag with fresh
          // motion samples below.
          applyScopeStationaryHold(lagTarget);
        } else {
          impl->expanderLagDragHoldAnchorActive = false;
          float velocitySamples = 0.f;
          float dtSec = requestDtSec;
          if (dtSec <= 0.f) {
            int frames = std::max(1, impl->expanderLagDragFramesSinceUpdate);
            dtSec = std::max(args.sampleTime, float(frames) * args.sampleTime);
          }
          // WARNING: Keep derived velocity in the same convention as
          // incoming scope velocity before blending.
          // positive velocity => toward NOW (decreasing lag).
          float derivedVelocity = impl->engine.lagErrorToTarget(impl->expanderLagDragLastLagSamples, lagTarget, maxLag) / dtSec;
          if (scopeLiveReverseTargetCompensated) {
            derivedVelocity += std::max(args.sampleRate, 1.f);
          }
          velocitySamples = derivedVelocity;
          if (std::fabs(lagDragRequestVelocity) > 1e-6f) {
            // Keep scope as a thin interface: lag target remains authoritative
            // and scope-reported velocity is only advisory. Preserve immediate
            // reversals from the scope path, but otherwise let host-derived
            // target motion define the gesture velocity that the engine sees.
            bool derivedHasDirection = std::fabs(derivedVelocity) > 1e-3f;
            bool requestHasDirection = std::fabs(lagDragRequestVelocity) > 1e-3f;
            bool directionDisagrees =
              derivedHasDirection && requestHasDirection && ((derivedVelocity > 0.f) != (lagDragRequestVelocity > 0.f));
            if (!derivedHasDirection || directionDisagrees) {
              velocitySamples = lagDragRequestVelocity;
            } else {
              velocitySamples = 0.75f * derivedVelocity + 0.25f * lagDragRequestVelocity;
            }
          }
          // Safety clamp: Scope is an external input path, so guard against
          // sender-side timing spikes producing unrealistic multi-turn motion.
          float maxAbsGestureVelocity = std::max(args.sampleRate * 3.0f, 1.0f);
          velocitySamples = clamp(velocitySamples, -maxAbsGestureVelocity, maxAbsGestureVelocity);
          scopeTraceVelocityApplied = velocitySamples;
          impl->platterInput.setScopeLagDrag(true, lagTarget, velocitySamples, false);
          int motionFreshSamples = int(std::round(args.sampleRate * std::max(args.sampleTime, dtSec) * 1.5f));
          int minHoldSamples = int(std::round(args.sampleRate * 0.025f));
          int maxHoldSamples = int(std::round(args.sampleRate * 0.090f));
          motionFreshSamples = clamp(motionFreshSamples, minHoldSamples, maxHoldSamples);
          impl->platterInput.setMotionFreshSamples(motionFreshSamples);
        }
        impl->expanderLagDragLastLagSamples = lagTarget;
        impl->expanderLagDragFramesSinceUpdate = 0;
        impl->expanderLagDragLastRequestTimeSec = processNowSec;
        impl->expanderLagDragHasLastRequestTime = true;
        impl->expanderLagDragWasActive = true;
        impl->expanderLagDragLastStationaryHold = scopeRequestStationary;
      } else {
        if (impl->expanderLagDragWasActive) {
          impl->platterInput.setScopeLagDrag(false, impl->expanderLagDragLastLagSamples, 0.f, false);
          impl->platterInput.setMotionFreshSamples(0);
        }
        impl->expanderLagDragHoldAnchorActive = false;
        impl->expanderLagDragWasActive = false;
        impl->expanderLagDragLastStationaryHold = false;
        impl->expanderLagDragFramesSinceUpdate = 0;
        impl->expanderLagDragHasLastRequestTime = false;
      }
    } else if (impl->expanderLagDragWasActive) {
      impl->expanderLagDragFramesSinceUpdate = std::min(impl->expanderLagDragFramesSinceUpdate + 1, 1 << 20);
    }
  } else {
    if (impl->expanderLagDragWasActive) {
      // Scope can disappear while a drag request is active (module deleted or
      // detached). Force-release host scratch state so lag does not keep
      // climbing from a stale touched/gesture condition.
      impl->platterInput.setScopeLagDrag(false, impl->expanderLagDragLastLagSamples, 0.f, false);
      impl->platterInput.setMotionFreshSamples(0);
    }
    impl->expanderLagDragWasActive = false;
    impl->expanderLagDragFramesSinceUpdate = 0;
    impl->expanderLagDragRequestSeen = false;
    impl->expanderLagDragHasLastRequestTime = false;
    impl->expanderLagDragLastStationaryHold = false;
    impl->expanderLagDragHoldAnchorActive = false;
  }
  PlatterInputSnapshot platterInput = impl->platterInput.consumeForFrame();

  if (impl->longPlayStartupHold) {
    impl->engine.requestStreamWindow();
    const double scopeRadiusFrames =
      double(std::max(args.sampleRate, 1.f)) * double(kScopeHalfWindowSeconds * 3.f);
    if (impl->engine.isStreamedScopeWindowResident(
          impl->engine.readHead, scopeRadiusFrames,
          impl->sampleLoopEnabled.load(std::memory_order_relaxed))) {
      impl->longPlayStartupHold = false;
    }
  }

  temporaldeck_frameinput::FrameInputControls controls;
  controls.dt = args.sampleTime;
  controls.bufferKnob = params[BUFFER_PARAM].getValue();
  controls.rateKnob = params[RATE_PARAM].getValue();
  controls.mixKnob = params[MIX_PARAM].getValue();
  controls.feedbackKnob = params[FEEDBACK_PARAM].getValue();
  controls.freezeButton = impl->transportControl.freezeLatched || impl->longPlayStartupHold;
  controls.reverseButton = impl->transportControl.reverseLatched;
  controls.slipButton = impl->transportControl.slipLatched && !impl->transportControl.reverseLatched;

  ProcessSignalInputs frameSignals;
  frameSignals.inL = inL;
  frameSignals.inR = inR;
  frameSignals.positionCv = positionCv;
  frameSignals.rateCv = rateCv;
  frameSignals.rateCvConnected = rateCvConnected;
  frameSignals.freezeGateHigh = freezeGateHigh;
  frameSignals.scratchGateHigh = scratchGateHigh;
  frameSignals.scratchGateConnected = scratchGateConnected;
  frameSignals.positionConnected = positionConnected;

  TemporalDeckEngine::FrameInput frameInput =
    temporaldeck_frameinput::buildFrameInput(frameSignals, controls, platterInput);

  auto frame = impl->engine.process(frameInput);
  impl->engine.requestStreamWindow();

  if (impl->engine.diskBackedSample) {
    if (temporaldeck::LongPlayStreamEngine *stream = impl->longPlayStream.load(std::memory_order_relaxed)) {
      impl->engine.sampleAbsolutePeakVolts =
        stream->absolutePeak() * temporaldeck::kSampleFileVoltageScale;
    }
  }

  if (scopeDragTraceEnabled) {
    bool scopeActive = haveLagDragRequest && lagDragRequestActive;
    if (!scopeActive) {
      if (impl->scopeDragTraceCaptureActive && impl->scopeDragTraceWasActive) {
        ScopeDragTraceEvent event;
        event.type = ScopeDragTraceEvent::EVENT_SCOPE_DRAG_END;
        event.eventSeq = impl->scopeDragTraceSequence++;
        event.tSec = float(std::max(0.0, system::getTime() - impl->scopeDragTraceStartTimeSec));
        event.requestSeq = lagDragRequestSeq;
        event.frameLag = float(frame.lag);
        event.sampleMode = frame.sampleMode;
        enqueueScopeTraceEvent(event);
      }
      if (impl->scopeDragTraceWasActive) {
        WARN("TemporalDeck ScopeDragTrace END lag=%.2f", float(frame.lag));
      }
      impl->scopeDragTraceWasActive = false;
      impl->scopeDragTraceHavePrev = false;
      impl->scopeDragTraceStallFrames = 0;
      impl->scopeDragTraceLogTimerSec = 0.f;
    } else {
      float targetLag = scopeTraceNewRequest ? scopeTraceLagTarget : impl->expanderLagDragLastLagSamples;
      float prevTargetLag = impl->scopeDragTraceHavePrev ? impl->scopeDragTracePrevTargetLag : targetLag;
      float prevFrameLag = impl->scopeDragTraceHavePrev ? impl->scopeDragTracePrevFrameLag : float(frame.lag);
      float targetDelta = targetLag - prevTargetLag;
      float frameLagDelta = float(frame.lag) - prevFrameLag;
      bool wantsAwayFromNow = targetDelta > 0.35f || lagDragRequestVelocity < -8.f;
      bool movingAwayFromNow = frameLagDelta > 0.20f;
      if (wantsAwayFromNow && !movingAwayFromNow && !scopeTraceDragJustStarted) {
        impl->scopeDragTraceStallFrames = std::min(impl->scopeDragTraceStallFrames + 1, 1 << 20);
      } else if (!wantsAwayFromNow || movingAwayFromNow) {
        impl->scopeDragTraceStallFrames = std::max(impl->scopeDragTraceStallFrames - 1, 0);
      }
      impl->scopeDragTraceLogTimerSec += args.sampleTime;
      bool shouldLog = scopeTraceNewRequest || !impl->scopeDragTraceWasActive || impl->scopeDragTraceStallFrames >= 3 ||
                       impl->scopeDragTraceLogTimerSec >= (1.f / 45.f);
      if (shouldLog) {
        impl->scopeDragTraceLogTimerSec = 0.f;
        bool freezeTrace = frameInput.freezeButton || frameInput.freezeGate;
        if (impl->scopeDragTraceCaptureActive) {
          ScopeDragTraceEvent event;
          event.type = ScopeDragTraceEvent::EVENT_SCOPE_DRAG;
          event.eventSeq = impl->scopeDragTraceSequence++;
          event.tSec = float(std::max(0.0, system::getTime() - impl->scopeDragTraceStartTimeSec));
          event.requestSeq = lagDragRequestSeq;
          event.frameLag = float(frame.lag);
          event.targetLag = targetLag;
          event.targetDelta = targetDelta;
          event.requestVelocity = lagDragRequestVelocity;
          event.appliedVelocity = scopeTraceVelocityApplied;
          event.frameLagDelta = frameLagDelta;
          event.stallFrames = impl->scopeDragTraceStallFrames;
          event.scopeActive = scopeActive;
          event.newRequest = scopeTraceNewRequest;
          event.justStarted = scopeTraceDragJustStarted;
          event.freeze = freezeTrace;
          event.sampleMode = frame.sampleMode;
          enqueueScopeTraceEvent(event);
        }
        WARN("TemporalDeck ScopeDragTrace seq=%llu target=%.2f targetΔ=%.2f reqVel=%.2f appliedVel=%.2f lag=%.2f lagΔ=%.2f freeze=%d "
             "sample=%d started=%d stall=%d",
             (unsigned long long)lagDragRequestSeq, targetLag, targetDelta, lagDragRequestVelocity, scopeTraceVelocityApplied,
             float(frame.lag), frameLagDelta, freezeTrace ? 1 : 0, frame.sampleMode ? 1 : 0,
             scopeTraceDragJustStarted ? 1 : 0, impl->scopeDragTraceStallFrames);
      }
      impl->scopeDragTraceWasActive = true;
      impl->scopeDragTraceHavePrev = true;
      impl->scopeDragTracePrevTargetLag = targetLag;
      impl->scopeDragTracePrevFrameLag = float(frame.lag);
    }
  }

  temporaldeck_transport::applyAutoFreezeRequest(impl->transportControl, frame.autoFreezeRequested, freezeGateHigh);

  writeFrameOutputs(*this, frame);
  bool freezeActive = impl->transportControl.freezeLatched || freezeGateHigh;
  updateTransportModeLights(*this, freezeActive, impl->transportControl.reverseLatched, impl->transportControl.slipLatched,
                            impl->transportControl.slipReturnMode);
  impl->uiPlatterAngle.store(frame.platterAngle, std::memory_order_relaxed);
  impl->uiLagSamples.store(frame.lag, std::memory_order_relaxed);
  impl->uiAccessibleLagSamples.store(frame.accessibleLag, std::memory_order_relaxed);
  impl->uiSampleRate.store(args.sampleRate, std::memory_order_relaxed);
  impl->uiFreezeLatched.store(freezeActive, std::memory_order_relaxed);
  impl->uiSampleModeEnabled.store(frame.sampleMode, std::memory_order_relaxed);
  impl->uiSampleLoaded.store(frame.sampleLoaded, std::memory_order_relaxed);
  impl->uiSampleTransportPlaying.store(frame.sampleTransportPlaying, std::memory_order_relaxed);
  impl->uiSamplePlayheadSeconds.store(frame.samplePlayhead, std::memory_order_relaxed);
  impl->uiSampleDurationSeconds.store(frame.sampleDuration, std::memory_order_relaxed);
  impl->uiSampleProgress.store(frame.sampleProgress, std::memory_order_relaxed);
  if (expanderConnected) {
    impl->expanderRequestedScopeFormat = requestedScopeFormat;
    bool wantStereoScope = requestedScopeFormat == temporaldeck_expander::SCOPE_FORMAT_STEREO;
    const bool scopeInteractionActive =
      platterInput.platterMotionActive || platterInput.scopeLagDragActive || impl->expanderLagDragWasActive;
    const bool frozenLiveScopeIdle = freezeActive && !frame.sampleMode && !scopeInteractionActive;
    const bool liveScopeIdle = !freezeActive && !frame.sampleMode && !scopeInteractionActive;
    const float expanderPublishIntervalSec =
      frozenLiveScopeIdle ? kExpanderPublishIntervalSecFrozenLive
                          : (liveScopeIdle ? kExpanderPublishIntervalSecLiveIdle : kExpanderPublishIntervalSec);

    bool justConnected = !impl->expanderWasConnected;
    bool generationChanged = impl->engine.bufferGeneration != impl->expanderLastPublishedGeneration;
    impl->expanderPublishTimerSec += args.sampleTime;
    bool timerElapsed = impl->expanderPublishTimerSec >= expanderPublishIntervalSec;
    bool shouldPublish = justConnected || generationChanged || timerElapsed;
    if (shouldPublish) {
      if (timerElapsed) {
        impl->expanderPublishTimerSec = std::fmod(impl->expanderPublishTimerSec, expanderPublishIntervalSec);
      } else {
        impl->expanderPublishTimerSec = 0.f;
      }
      auto *msg =
        reinterpret_cast<temporaldeck_expander::HostToDisplay *>(right->leftExpander.producerMessage);
      if (msg) {
        // Data direction contract:
        // this is the TemporalDeck -> TD.Scope stream. Host writes display data
        // into TD.Scope's leftExpander.producerMessage and flips it so TD.Scope
        // consumes from leftExpander.consumerMessage.
        ScopeViewState scopeViewState =
          computeScopeViewState(platterInput, float(frame.lag), float(frame.accessibleLag), impl->expanderLagDragWasActive,
                                impl->expanderLagDragLastStationaryHold, impl->expanderLagDragLastLagSamples,
                                impl->expanderScopeLagHoldActive, impl->expanderScopeLagHoldSamples,
                                impl->expanderScopeNewestPosHoldActive, impl->expanderScopeNewestAbsolutePosHold, impl->engine);
        float scopeLagForPreview = scopeViewState.waveformLagAnchor;
        double scopeLiveNewestAbsolutePosOverride =
          scopeViewState.useNewestAbsoluteAnchor ? scopeViewState.waveformNewestAbsoluteAnchor : -1.0;

        std::array<temporaldeck_expander::ScopeBin, temporaldeck_expander::SCOPE_BIN_COUNT> scopeBins;
        std::array<temporaldeck_expander::ScopeBin, temporaldeck_expander::SCOPE_BIN_COUNT> scopeBinsRight;
        float scopeStartLagSamples = 0.f;
        float scopeVisibleStartLagSamples = 0.f;
        float scopeBinSpanSamples = 1.f;
        float scopeNewestPosSamples = 0.f;
        uint32_t scopeBinCount = 0u;
        ScopeWindowParams scopeParams;
        auto scopePreviewMeasureStart =
          perfTimingEnabled ? std::chrono::steady_clock::now() : std::chrono::steady_clock::time_point();
        // A restored LongPlay stream is installed asynchronously. Until that
        // install completes, frame.sampleMode is necessarily false even though
        // the saved patch expects sample mode. Do not publish the temporary
        // live buffer as a valid scope: TD.Scope should remain in its waiting
        // state and then receive the installed sample generation immediately.
        const bool samplePreviewNotReady =
          impl->longPlayInstallPending.load(std::memory_order_acquire) || impl->longPlayStartupHold;
        bool haveScopeParams = !samplePreviewNotReady && computeScopeWindowParams(
          impl->engine, frame.sampleMode, impl->sampleLoopEnabled.load(std::memory_order_relaxed), impl->cachedSampleRate,
          scopeLagForPreview, float(frame.accessibleLag), scopeLiveNewestAbsolutePosOverride, &scopeParams);
        if (haveScopeParams) {
          ScopeChannelMode leftMode = wantStereoScope ? SCOPE_CHANNEL_LEFT : SCOPE_CHANNEL_MID;
          const bool allowFrozenLiveScopeCacheReuse = freezeActive && !frame.sampleMode && !scopeInteractionActive;
          uint32_t leftCount = buildScopeWindowBinsWithCache(impl->engine, scopeParams, leftMode,
                                                             &impl->expanderScopeCacheMono, &scopeBins,
                                                             allowFrozenLiveScopeCacheReuse);
          uint32_t rightCount = 0u;
          if (wantStereoScope) {
            rightCount = buildScopeWindowBinsWithCache(impl->engine, scopeParams, SCOPE_CHANNEL_RIGHT,
                                                       &impl->expanderScopeCacheRight, &scopeBinsRight,
                                                       allowFrozenLiveScopeCacheReuse);
            scopeBinCount = (leftCount > 0u && rightCount > 0u) ? std::min(leftCount, rightCount) : 0u;
          } else {
            impl->expanderScopeCacheRight.valid = false;
            scopeBinCount = leftCount;
          }
          scopeStartLagSamples = scopeParams.scopeStartLagSamples;
          scopeVisibleStartLagSamples = scopeParams.scopeVisibleStartLagSamples;
          scopeBinSpanSamples = scopeParams.binSpanSamples;
          scopeNewestPosSamples = float(scopeParams.newestPos);
        } else {
          impl->expanderScopeCacheMono.valid = false;
          impl->expanderScopeCacheRight.valid = false;
        }
        if (perfTimingEnabled) {
          auto scopePreviewMeasureEnd = std::chrono::steady_clock::now();
          float scopePreviewMeasureUs =
            float(std::chrono::duration_cast<std::chrono::microseconds>(scopePreviewMeasureEnd - scopePreviewMeasureStart).count());
          float previousMeasureUs = impl->uiScopePreviewCostUs.load(std::memory_order_relaxed);
          float smoothedMeasureUs =
            (previousMeasureUs > 0.f) ? (previousMeasureUs + (scopePreviewMeasureUs - previousMeasureUs) * 0.25f) : scopePreviewMeasureUs;
          impl->uiScopePreviewCostUs.store(smoothedMeasureUs, std::memory_order_relaxed);
        }
        impl->uiScopePreviewStride.store(haveScopeParams ? std::max(0, scopeParams.scopeStride) : 0, std::memory_order_relaxed);
        impl->uiScopePreviewMetricValid.store(haveScopeParams, std::memory_order_relaxed);
        uint32_t flags = 0;
        if (frame.sampleMode) {
          flags |= temporaldeck_expander::FLAG_SAMPLE_MODE;
        }
        if (frame.sampleLoaded) {
          flags |= temporaldeck_expander::FLAG_SAMPLE_LOADED;
        }
        if (frame.sampleTransportPlaying) {
          flags |= temporaldeck_expander::FLAG_SAMPLE_PLAYING;
        }
        if (impl->sampleLoopEnabled.load(std::memory_order_relaxed)) {
          flags |= temporaldeck_expander::FLAG_SAMPLE_LOOP;
        }
        if (freezeActive) {
          flags |= temporaldeck_expander::FLAG_FREEZE;
        }
        if (impl->transportControl.reverseLatched) {
          flags |= temporaldeck_expander::FLAG_REVERSE;
        }
        if (impl->transportControl.slipLatched) {
          flags |= temporaldeck_expander::FLAG_SLIP;
        }
        bool scopeReady = scopeBinCount > 0u;
        if (scopeReady) {
          flags |= temporaldeck_expander::FLAG_PREVIEW_VALID;
          if (wantStereoScope) {
            flags |= temporaldeck_expander::FLAG_SCOPE_STEREO;
          }
        }
        if (impl->expanderStereoAutoPromotePending) {
          flags |= temporaldeck_expander::FLAG_SCOPE_AUTO_PROMOTE_STEREO;
          impl->expanderStereoAutoPromotePending = false;
        }
        if (impl->expanderAttachChannelSyncPending) {
          flags |= temporaldeck_expander::FLAG_SCOPE_ATTACH_CHANNEL_SYNC;
          if (impl->expanderAttachChannelSyncStereo) {
            flags |= temporaldeck_expander::FLAG_SCOPE_INPUTS_DUAL_CONNECTED;
          }
          impl->expanderAttachChannelSyncPending = false;
        }
        if (impl->engine.buffer.monoStorage) {
          flags |= temporaldeck_expander::FLAG_MONO_BUFFER;
        }

        impl->expanderPublishSeq++;
        float sampleAbsolutePeakVolts =
          (frame.sampleMode && frame.sampleLoaded) ? impl->engine.sampleAbsolutePeakVolts : impl->engine.getLiveAbsolutePeakVolts();
        float combinedSensitivity = scratchSensitivity() * kMouseScratchTravelScale;
        temporaldeck_expander::populateHostMessage(
          msg, impl->expanderPublishSeq, impl->engine.bufferGeneration, flags, impl->cachedSampleRate,
          scopeViewState.markerLag,
          float(frame.accessibleLag), frame.platterAngle, float(frame.samplePlayhead), float(frame.sampleDuration),
          float(frame.sampleProgress), sampleAbsolutePeakVolts, combinedSensitivity,
          uint32_t(std::max(0, impl->engine.buffer.size)),
          uint32_t(std::max(0, impl->engine.buffer.filled)), kScopeHalfWindowMs, scopeStartLagSamples,
          scopeVisibleStartLagSamples,
          scopeBinSpanSamples, scopeNewestPosSamples, scopeBinCount, scopeBins.data(),
          wantStereoScope ? scopeBinsRight.data() : nullptr);
        right->leftExpander.messageFlipRequested = true;
        impl->expanderLastPublishedGeneration = impl->engine.bufferGeneration;
        impl->expanderPreviewValid = scopeReady;
      } else {
        impl->expanderPreviewValid = false;
        impl->uiScopePreviewMetricValid.store(false, std::memory_order_relaxed);
        impl->uiScopePreviewStride.store(0, std::memory_order_relaxed);
      }
    }
  } else {
    impl->expanderPublishTimerSec = 0.f;
    impl->expanderPreviewValid = false;
    impl->expanderLagDragWasActive = false;
    impl->expanderLagDragRequestSeen = false;
    impl->expanderLagDragLastRequestSeq = 0u;
    impl->expanderLagDragLastLagSamples = 0.f;
    impl->expanderLagDragAnchorLagSamples = 0.f;
    impl->expanderLagDragFramesSinceUpdate = 0;
    impl->expanderLagDragHasLastRequestTime = false;
    impl->expanderLagDragLastRequestTimeSec = 0.0;
    impl->expanderLagDragLastStationaryHold = false;
    impl->expanderLagDragHoldAnchorActive = false;
    impl->expanderScopeCacheMono.valid = false;
    impl->expanderScopeCacheRight.valid = false;
    impl->expanderScopeLagHoldActive = false;
    impl->expanderScopeNewestPosHoldActive = false;
    impl->expanderRequestedScopeFormat = temporaldeck_expander::SCOPE_FORMAT_MONO;
    impl->uiScopePreviewMetricValid.store(false, std::memory_order_relaxed);
    impl->uiScopePreviewStride.store(0, std::memory_order_relaxed);
  }
  impl->expanderWasConnected = expanderConnected;
  bool expanderReady = expanderConnected && impl->expanderPreviewValid;
  lights[EXPANDER_LINK_LIGHT].setBrightness(expanderConnected && !expanderReady ? 1.f : 0.f);
  lights[EXPANDER_READY_LIGHT].setBrightness(expanderReady ? 1.f : 0.f);

  impl->sampleModeEnabled.store(impl->engine.sampleModeEnabled, std::memory_order_relaxed);
  impl->uiPublishTimerSec += args.sampleTime;
  if (impl->uiPublishTimerSec >= kUiPublishIntervalSec) {
    impl->uiPublishTimerSec = std::fmod(impl->uiPublishTimerSec, kUiPublishIntervalSec);
    int sampleFrames = impl->engine.sampleFrames;
    int bufferMode = impl->bufferDurationMode.load(std::memory_order_relaxed);
    float maxLagSamples = std::max(1.f, args.sampleRate * usableBufferSecondsForMode(bufferMode));
    temporaldeck_ui::publishArcLights(this, sampleFrames, maxLagSamples, frame.sampleMode, frame.sampleLoaded,
                                      frame.lag, frame.accessibleLag, frame.sampleProgress);
  }
  if (perfTimingEnabled) {
    const auto processEnd = std::chrono::steady_clock::now();
    const uint64_t elapsedNs =
      uint64_t(std::max<int64_t>(0, std::chrono::duration_cast<std::chrono::nanoseconds>(processEnd - processStart).count()));
    impl->perfAudioProcessNs.fetch_add(elapsedNs, std::memory_order_relaxed);
    impl->perfAudioSampledCount.fetch_add(1u, std::memory_order_relaxed);
    debug_terminal::recordAudioProcessTiming(impl->perfAudioProcessMinNs, impl->perfAudioProcessMaxNs, elapsedNs);
  }
}

void TemporalDeck::setPlatterScratch(bool touched, float lagSamples, float velocitySamples, int holdSamples) {
  impl->platterInput.setScratch(touched, lagSamples, velocitySamples, holdSamples);
}

void TemporalDeck::setPlatterTouchHold(bool touched, float lagSamples) {
  impl->platterInput.setTouchHold(touched, lagSamples);
}

void TemporalDeck::setPlatterMotionFreshSamples(int motionFreshSamples) {
  impl->platterInput.setMotionFreshSamples(motionFreshSamples);
}

void TemporalDeck::addPlatterWheelDelta(float delta, int holdSamples) {
  impl->platterInput.addWheelDelta(delta, holdSamples);
}

void TemporalDeck::triggerQuickSlipReturn() {
  impl->platterInput.triggerQuickSlipReturn();
}

double TemporalDeck::getUiLagSamples() const {
  return impl->uiLagSamples.load(std::memory_order_relaxed);
}

double TemporalDeck::getUiAccessibleLagSamples() const {
  return impl->uiAccessibleLagSamples.load(std::memory_order_relaxed);
}

float TemporalDeck::getUiSampleRate() const {
  return impl->uiSampleRate.load(std::memory_order_relaxed);
}

debug_terminal::TimingRangeUs TemporalDeck::consumeAudioProcessUs() {
  impl->perfAudioSampledCount.exchange(0u, std::memory_order_acq_rel);
  impl->perfAudioProcessNs.exchange(0u, std::memory_order_acq_rel);
  return debug_terminal::consumeAudioProcessTiming(impl->perfAudioProcessMinNs, impl->perfAudioProcessMaxNs);
}

float TemporalDeck::getUiScopePreviewCostUs() const {
  return impl->uiScopePreviewCostUs.load(std::memory_order_relaxed);
}

float TemporalDeck::getUiDrawCostUs() const {
  return impl->uiDrawCostUs.load(std::memory_order_relaxed);
}

uint32_t TemporalDeck::getDebugInstanceId() const {
  return impl->debugInstanceId;
}

void TemporalDeck::setUiDrawCostUs(float costUs) {
  costUs = std::max(0.f, costUs);
  float prevUs = impl->uiDrawCostUs.load(std::memory_order_relaxed);
  float smoothedUs = (prevUs > 0.f) ? (prevUs + (costUs - prevUs) * 0.18f) : costUs;
  impl->uiDrawCostUs.store(smoothedUs, std::memory_order_relaxed);
}

int TemporalDeck::getUiScopePreviewStride() const {
  return impl->uiScopePreviewStride.load(std::memory_order_relaxed);
}

bool TemporalDeck::isUiScopePreviewMetricValid() const {
  return impl->uiScopePreviewMetricValid.load(std::memory_order_relaxed);
}

float TemporalDeck::getUiPlatterAngle() const {
  return impl->uiPlatterAngle.load(std::memory_order_relaxed);
}

bool TemporalDeck::isUiFreezeLatched() const {
  return impl->uiFreezeLatched.load(std::memory_order_relaxed);
}


bool TemporalDeck::isSampleModeEnabled() const {
  return impl->uiSampleModeEnabled.load(std::memory_order_relaxed);
}

bool TemporalDeck::hasLoadedSample() const {
  return impl->uiSampleLoaded.load(std::memory_order_relaxed);
}

void TemporalDeck::setSampleModeEnabled(bool enabled) {
  impl->sampleModeEnabled.store(enabled, std::memory_order_relaxed);
  impl->sampleLifecycle.setPendingSampleStateApply();
  impl->uiSampleModeEnabled.store(enabled && impl->engine.sampleLoaded, std::memory_order_relaxed);
}

bool TemporalDeck::isSampleTransportPlaying() const {
  return impl->uiSampleTransportPlaying.load(std::memory_order_relaxed);
}

bool TemporalDeck::isSampleLoopEnabled() const {
  return impl->sampleLoopEnabled.load(std::memory_order_relaxed);
}

void TemporalDeck::setSampleLoopEnabled(bool enabled) {
  impl->sampleLoopEnabled.store(enabled, std::memory_order_relaxed);
  impl->engine.sampleLoopEnabled = enabled;
}

void TemporalDeck::setSampleTransportPlaying(bool enabled) {
  impl->engine.sampleTransportPlaying = enabled && impl->engine.sampleLoaded;
  impl->uiSampleTransportPlaying.store(impl->engine.sampleTransportPlaying, std::memory_order_relaxed);
}

void TemporalDeck::stopSampleTransport() {
  impl->engine.sampleTransportPlaying = false;
  if (impl->engine.sampleLoaded) {
    impl->engine.samplePlayhead = 0.0;
    impl->engine.readHead = 0.0;
  }
  impl->uiSampleTransportPlaying.store(false, std::memory_order_relaxed);
  impl->uiSamplePlayheadSeconds.store(0.0, std::memory_order_relaxed);
  impl->uiSampleProgress.store(0.0, std::memory_order_relaxed);
}

void TemporalDeck::clearLoadedSample() {
  if (temporaldeck::LongPlayStreamEngine *stream =
        impl->longPlayStream.load(std::memory_order_acquire)) {
    stream->clear();
  }
  impl->longPlayInstallPending.store(false, std::memory_order_release);
  impl->longPlayStartupHold = false;
  impl->sampleLifecycle.clearDecodedAndPreparedState();
  temporaldeck_lifecycle::TemporalDeckSampleLifecycle::AsyncSampleBuildRequest cancelRequest;
  cancelRequest.type = temporaldeck_lifecycle::TemporalDeckSampleLifecycle::AsyncSampleBuildRequest::NONE;
  cancelRequest.targetSampleRate = std::max(impl->cachedSampleRate, 1.f);
  cancelRequest.requestedBufferMode = impl->bufferDurationMode.load(std::memory_order_relaxed);
  impl->sampleLifecycle.requestAsyncSampleBuild(cancelRequest);
  impl->sampleModeEnabled.store(false, std::memory_order_relaxed);
  impl->bufferDurationMode.store(BUFFER_DURATION_10S);
  impl->lastRamBufferDurationMode = BUFFER_DURATION_10S;
  if (paramQuantities[BUFFER_PARAM]) {
    paramQuantities[BUFFER_PARAM]->displayMultiplier = usableBufferSecondsForMode(BUFFER_DURATION_10S);
  }
  impl->sampleLifecycle.setPendingSampleStateApply();
}

bool TemporalDeck::loadSampleFromPath(const std::string &path, std::string *errorOut) {
  // Released load behavior preserves Freeze, while a newly loaded source
  // starts without inherited Reverse or Slip state. Apply this before storage
  // selection so RAM and LongPlay loads cannot diverge.
  const bool wasFreezeLatched = impl->transportControl.freezeLatched;
  const bool wasFreezeLatchedByButton = impl->transportControl.freezeLatchedByButton;
  impl->transportControl.freezeLatched = wasFreezeLatched;
  impl->transportControl.freezeLatchedByButton =
    wasFreezeLatched ? wasFreezeLatchedByButton : false;
  impl->transportControl.reverseLatched = false;
  impl->transportControl.slipLatched = false;

  temporaldeck::LongPlayFileInfo longPlayInfo;
  std::string probeError;
  const bool probed = temporaldeck::probeLongPlayFile(path, &longPlayInfo, &probeError);
  const bool useLongPlay = probed && longPlayInfo.sampleRate > 0u &&
    double(longPlayInfo.totalFrames) / double(longPlayInfo.sampleRate) > 600.0;
  if (useLongPlay) {
    if (!impl->longPlayOwner) {
      impl->longPlayOwner.reset(new temporaldeck::LongPlayStreamEngine());
      impl->longPlayStream.store(impl->longPlayOwner.get(), std::memory_order_release);
    }
    impl->sampleLifecycle.clearDecodedAndPreparedState();
    impl->sampleLifecycle.setSampleSavedPath(path);
    temporaldeck_lifecycle::TemporalDeckSampleLifecycle::AsyncSampleBuildRequest guardRequest;
    guardRequest.type = temporaldeck_lifecycle::TemporalDeckSampleLifecycle::AsyncSampleBuildRequest::BUILD_EMPTY_BUFFER;
    guardRequest.targetSampleRate = std::max(impl->cachedSampleRate, 1.f);
    guardRequest.requestedBufferMode = BUFFER_DURATION_LONGPLAY_DISK;
    impl->sampleLifecycle.requestAsyncSampleBuild(guardRequest);
    impl->longPlayOwner->requestLoad(path);
    impl->longPlayInstallPending.store(true, std::memory_order_release);
    // Hold the current source before its cache is invalidated. The hold stays
    // internal and releases only after the replacement's audio/scope window
    // is resident.
    impl->longPlayStartupHold = true;
    impl->installedLongPlayGeneration = impl->longPlayOwner->generation();
    return true;
  }
  if (!probed && errorOut) {
    *errorOut = probeError;
  }
  if (temporaldeck::LongPlayStreamEngine *stream =
        impl->longPlayStream.load(std::memory_order_acquire)) {
    stream->clear();
  }
  impl->longPlayInstallPending.store(false, std::memory_order_release);
  impl->longPlayStartupHold = false;
  const int ramBufferMode = temporaldeck_modes::sanitizeRamBufferMode(
    impl->lastRamBufferDurationMode, BUFFER_DURATION_10S);
  impl->bufferDurationMode.store(ramBufferMode, std::memory_order_relaxed);
  if (paramQuantities[BUFFER_PARAM]) {
    paramQuantities[BUFFER_PARAM]->displayMultiplier = usableBufferSecondsForMode(ramBufferMode);
  }
  temporaldeck_lifecycle::TemporalDeckSampleLifecycle::AsyncSampleBuildRequest request;
  request.type = temporaldeck_lifecycle::TemporalDeckSampleLifecycle::AsyncSampleBuildRequest::LOAD_PATH;
  request.path = path;
  request.targetSampleRate = std::max(impl->cachedSampleRate, 1.f);
  request.requestedBufferMode = ramBufferMode;
  impl->pendingSampleStateApplyDeferralLogged = false;
  uint64_t requestSerial = impl->sampleLifecycle.requestAsyncSampleBuild(request);
  appendTemporalDeckLifetimeLoadingLog(impl->debugInstanceId,
                                       "request_load_path",
                                       path,
                                       requestSerial,
                                       request.type,
                                       request.targetSampleRate,
                                       request.requestedBufferMode,
                                       0,
                                       0,
                                       0,
                                       (impl->engine.buffer.left.capacity() + impl->engine.buffer.right.capacity()) * sizeof(float),
                                       0u,
                                       0u,
                                       impl->sampleLifecycle.decodedSampleAvailable(),
                                       true,
                                       0.0,
                                       0.0,
                                       0.0,
                                       0.0,
                                       0.0,
                                       0.0);
  return true;
}

void TemporalDeck::convertLiveToSample() {
  bool wasFreezeLatched = impl->transportControl.freezeLatched;
  bool wasFreezeLatchedByButton = impl->transportControl.freezeLatchedByButton;
  impl->transportControl.freezeLatched = wasFreezeLatched;
  impl->transportControl.freezeLatchedByButton = impl->transportControl.freezeLatched ? wasFreezeLatchedByButton : false;
  impl->transportControl.reverseLatched = false;
  impl->transportControl.slipLatched = false;
  impl->pendingLegacySampleFreezeOnPreparedInstall = false;

  impl->sampleLifecycle.clearDecodedAndPreparedState();
  temporaldeck_lifecycle::TemporalDeckSampleLifecycle::AsyncSampleBuildRequest cancelRequest;
  cancelRequest.type = temporaldeck_lifecycle::TemporalDeckSampleLifecycle::AsyncSampleBuildRequest::NONE;
  cancelRequest.targetSampleRate = std::max(impl->cachedSampleRate, 1.f);
  cancelRequest.requestedBufferMode = impl->bufferDurationMode.load(std::memory_order_relaxed);
  impl->sampleLifecycle.requestAsyncSampleBuild(cancelRequest);
  impl->pendingLiveToSampleConvert.store(true, std::memory_order_relaxed);
}

void TemporalDeck::seekSampleByNormalizedPosition(double normalized) {
  impl->pendingSampleSeekNormalized.store(clamp(float(normalized), 0.f, 1.f), std::memory_order_relaxed);
  impl->pendingSampleSeekRevision.fetch_add(1, std::memory_order_relaxed);
}

void TemporalDeck::seekLiveByArcNormalizedPosition(double normalized) {
  impl->pendingLiveSeekArcNormalized.store(clamp(float(normalized), 0.f, 1.f), std::memory_order_relaxed);
  impl->pendingLiveSeekRevision.fetch_add(1, std::memory_order_relaxed);
}

bool TemporalDeck::isLoadedSampleLiveConversion() const {
  if (!hasLoadedSample()) {
    return false;
  }
  return impl->sampleLifecycle.samplePath().empty();
}

bool TemporalDeck::saveLoadedSampleToPath(const std::string &path, std::string *errorOut) {
  if (!impl->engine.sampleLoaded || impl->engine.sampleFrames <= 0) {
    if (errorOut) {
      *errorOut = "No sample is loaded";
    }
    return false;
  }
  if (impl->engine.diskBackedSample) {
    if (errorOut) {
      *errorOut = "LongPlay samples remain disk-backed and cannot be exported as a RAM capture";
    }
    return false;
  }
  int frames = impl->engine.sampleFrames;
  int channels = impl->engine.buffer.monoStorage ? 1 : 2;
  std::vector<float> left(size_t(frames), 0.f);
  std::vector<float> right(channels == 2 ? size_t(frames) : 0u);
  for (int i = 0; i < frames; ++i) {
    left[size_t(i)] = impl->engine.sampleLeftAt(i);
    if (channels == 2) {
      right[size_t(i)] = impl->engine.sampleRightAt(i);
    }
  }
  if (!writeStereoOrMonoWav16(path, left, right, frames, channels,
                              impl->engine.sampleRate, temporaldeck::bufferVoltageToSampleFile(1.f), errorOut)) {
    return false;
  }
  // Promote live-converted sample to file-backed state for patch restore.
  impl->sampleLifecycle.setSampleSavedPath(path);
  return true;
}

double TemporalDeck::getUiSamplePlayheadSeconds() const {
  return impl->uiSamplePlayheadSeconds.load();
}

double TemporalDeck::getUiSampleDurationSeconds() const {
  return impl->uiSampleDurationSeconds.load();
}

double TemporalDeck::getUiSampleProgress() const {
  return impl->uiSampleProgress.load();
}

std::string TemporalDeck::getLoadedSampleDisplayName() const {
  return impl->sampleLifecycle.sampleDisplayName();
}

bool TemporalDeck::wasLoadedSampleTruncated() const {
  return impl->engine.sampleTruncated;
}

bool TemporalDeck::isSlipLatched() const {
  return impl->transportControl.slipLatched;
}

int TemporalDeck::getCartridgeCharacter() const {
  return impl->cartridgeCharacter;
}

int TemporalDeck::getBufferDurationMode() const {
  return clamp(impl->bufferDurationMode.load(), 0, BUFFER_DURATION_COUNT - 1);
}

bool TemporalDeck::isBufferModeMono() const {
  return isMonoBufferMode(clamp(impl->bufferDurationMode.load(), 0, BUFFER_DURATION_COUNT - 1));
}

bool TemporalDeck::consumePendingInitialPlatterArtSelection() {
  if (!impl->pendingInitialPlatterArtSelection) {
    return false;
  }
  impl->pendingInitialPlatterArtSelection = false;
  return true;
}

int TemporalDeck::getPlatterArtMode() const {
  return clamp(impl->platterArtMode, PLATTER_ART_BUILTIN_SVG, PLATTER_ART_MODE_COUNT - 1);
}

void TemporalDeck::setPlatterArtMode(int mode) {
  int clamped = clamp(mode, PLATTER_ART_BUILTIN_SVG, PLATTER_ART_MODE_COUNT - 1);
  impl->platterArtMode = clamped;
}

int TemporalDeck::getPlatterBrightnessMode() const {
  return clamp(impl->platterBrightnessMode, PLATTER_BRIGHTNESS_FULL, PLATTER_BRIGHTNESS_COUNT - 1);
}

void TemporalDeck::setPlatterBrightnessMode(int mode) {
  impl->platterBrightnessMode = clamp(mode, PLATTER_BRIGHTNESS_FULL, PLATTER_BRIGHTNESS_COUNT - 1);
}

std::string TemporalDeck::getCustomPlatterArtPath() const {
  return impl->customPlatterArtPath;
}

bool TemporalDeck::setCustomPlatterArtPath(const std::string &path) {
  if (path.empty()) {
    return false;
  }
  impl->customPlatterArtPath = path;
  impl->platterArtMode = PLATTER_ART_CUSTOM;
  return true;
}

void TemporalDeck::clearCustomPlatterArtPath() {
  impl->customPlatterArtPath.clear();
  if (impl->platterArtMode == PLATTER_ART_CUSTOM) {
    impl->platterArtMode = PLATTER_ART_BUILTIN_SVG;
  }
}

bool TemporalDeck::isPlatterTraceLoggingEnabled() const {
  return impl->platterTraceLoggingEnabled.load(std::memory_order_relaxed);
}

void TemporalDeck::setPlatterTraceLoggingEnabled(bool enabled) {
  impl->platterTraceLoggingEnabled.store(enabled, std::memory_order_relaxed);
}

bool TemporalDeck::isScopeDragTraceLoggingEnabled() const {
  return impl->scopeDragTraceLoggingEnabled.load(std::memory_order_relaxed);
}

void TemporalDeck::setScopeDragTraceLoggingEnabled(bool enabled) {
  impl->scopeDragTraceLoggingEnabled.store(enabled, std::memory_order_relaxed);
}

bool TemporalDeck::popScopeDragTraceEvent(ScopeDragTraceEvent *outEvent) {
  if (!outEvent) {
    return false;
  }
  uint32_t read = impl->scopeDragTraceQueueRead.load(std::memory_order_relaxed);
  uint32_t write = impl->scopeDragTraceQueueWrite.load(std::memory_order_acquire);
  if (read == write) {
    return false;
  }
  *outEvent = impl->scopeDragTraceQueue[size_t(read)];
  uint32_t next = (read + 1u) % Impl::scopeDragTraceQueueCapacity;
  impl->scopeDragTraceQueueRead.store(next, std::memory_order_release);
  return true;
}

uint32_t TemporalDeck::consumeScopeDragTraceDroppedCount() {
  return impl->scopeDragTraceDropped.exchange(0u, std::memory_order_acq_rel);
}

bool TemporalDeck::isHighQualityRateInterpolationEnabled() const {
  return impl->highQualityRateInterpolation;
}

void TemporalDeck::setHighQualityRateInterpolationEnabled(bool enabled) {
  impl->highQualityRateInterpolation = enabled;
  impl->engine.highQualityRateInterpolation = enabled;
}

bool TemporalDeck::isHighQualityScratchInterpolationEnabled() const {
  return impl->scratchInterpolationMode != SCRATCH_INTERP_CUBIC;
}

void TemporalDeck::setHighQualityScratchInterpolationEnabled(bool enabled) {
  impl->scratchInterpolationMode = enabled ? SCRATCH_INTERP_LAGRANGE6 : SCRATCH_INTERP_CUBIC;
}

int TemporalDeck::getScratchInterpolationMode() const {
  return impl->scratchInterpolationMode;
}

void TemporalDeck::setScratchInterpolationMode(int mode) {
  impl->scratchInterpolationMode = clamp(mode, SCRATCH_INTERP_CUBIC, SCRATCH_INTERP_COUNT - 1);
}

void TemporalDeck::setSlipLatched(bool enabled) {
  impl->transportControl.slipLatched = enabled;
  if (enabled) {
    impl->transportControl.freezeLatched = false;
    impl->transportControl.freezeLatchedByButton = false;
  }
}

int TemporalDeck::getSlipReturnMode() const {
  return impl->transportControl.slipReturnMode;
}

void TemporalDeck::setSlipReturnMode(int mode) {
  impl->transportControl.slipReturnMode = clamp(mode, SLIP_RETURN_SLOW, SLIP_RETURN_COUNT - 1);
}

int TemporalDeck::getExternalGatePosMode() const {
  return impl->externalGatePosMode;
}

void TemporalDeck::setExternalGatePosMode(int mode) {
  impl->externalGatePosMode = clamp(mode, EXTERNAL_GATE_POS_GLIDE, EXTERNAL_GATE_POS_COUNT - 1);
}

int TemporalDeck::getReverseCvMode() const {
  return impl->reverseCvMode;
}

int TemporalDeck::getFreezeCvMode() const {
  return impl->freezeCvMode;
}

void TemporalDeck::setFreezeCvMode(int mode) {
  impl->freezeCvMode = clamp(mode, FREEZE_CV_MODE_PULSED, FREEZE_CV_MODE_COUNT - 1);
}

void TemporalDeck::setReverseCvMode(int mode) {
  impl->reverseCvMode = clamp(mode, REVERSE_CV_MODE_PULSED, REVERSE_CV_MODE_COUNT - 1);
}

const char *TemporalDeck::cartridgeLabelFor(int index) {
  switch (index) {
  case CARTRIDGE_M44_7:
    return "M44-7";
  case CARTRIDGE_ORTOFON_SCRATCH:
    return "C.MKII S";
  case CARTRIDGE_STANTON_680HP:
    return "680 HP";
  case CARTRIDGE_QBERT:
    return "Q.Bert";
  case CARTRIDGE_LOFI:
    return "Lo-Fi";
  case CARTRIDGE_CLEAN:
  default:
    return "Clean";
  }
}

CartridgeVisualStyle TemporalDeck::cartridgeVisualStyleFor(int index) {
  switch (index) {
  case CARTRIDGE_M44_7:
    return {nvgRGBA(26, 26, 26, 238), nvgRGBA(110, 110, 118, 190), nvgRGBA(252, 252, 252, 235)};
  case CARTRIDGE_ORTOFON_SCRATCH:
    return {nvgRGBA(242, 242, 242, 240), nvgRGBA(26, 26, 26, 210), nvgRGBA(18, 18, 18, 228)};
  case CARTRIDGE_STANTON_680HP:
    return {nvgRGBA(180, 186, 194, 238), nvgRGBA(120, 126, 134, 195), nvgRGBA(24, 24, 28, 230)};
  case CARTRIDGE_QBERT:
    return {nvgRGBA(34, 35, 40, 240), nvgRGBA(240, 242, 246, 210), nvgRGBA(248, 200, 58, 235)};
  case CARTRIDGE_LOFI:
    return {nvgRGBA(35, 28, 74, 238), nvgRGBA(87, 64, 191, 205), nvgRGBA(87, 64, 191, 224)};
  case CARTRIDGE_CLEAN:
  default:
    return {nvgRGBA(90, 178, 187, 236), nvgRGBA(12, 41, 45, 190), nvgRGBA(0, 0, 0, 235)};
  }
}

const char *TemporalDeck::scratchInterpolationLabelFor(int index) {
  switch (index) {
  case SCRATCH_INTERP_LAGRANGE6:
    return "6-point Lagrange";
  case SCRATCH_INTERP_SINC:
    return "Sinc (CPU heavy)";
  case SCRATCH_INTERP_CUBIC:
  default:
    return "Cubic";
  }
}

const char *TemporalDeck::slipReturnLabelFor(int index) {
  switch (index) {
  case SLIP_RETURN_SLOW:
    return "Slow";
  case SLIP_RETURN_INSTANT:
    return "Instant";
  case SLIP_RETURN_NORMAL:
  default:
    return "Normal";
  }
}

const char *TemporalDeck::bufferDurationLabelFor(int index) {
  switch (index) {
  case BUFFER_DURATION_20S:
    return "20 s";
  case BUFFER_DURATION_1M_STEREO:
    return "1 min stereo";
  case BUFFER_DURATION_2M_STEREO:
    return "2 min stereo";
  case BUFFER_DURATION_10M_STEREO:
    return "10 min stereo";
  case BUFFER_DURATION_10M_MONO:
    return "10 min mono";
  case BUFFER_DURATION_LONGPLAY_DISK:
    return "LongPlay (Disk 1h+)";
  case BUFFER_DURATION_10S:
  default:
    return "10 s";
  }
}

const char *TemporalDeck::externalGatePosLabelFor(int index) {
  switch (index) {
  case EXTERNAL_GATE_POS_MODULE_SYNC:
    return "Module sync";
  case EXTERNAL_GATE_POS_GLIDE:
  default:
    return "Glide / inertia";
  }
}

const char *TemporalDeck::reverseCvModeLabelFor(int index) {
  switch (index) {
  case REVERSE_CV_MODE_GATE:
    return "Gated Control";
  case REVERSE_CV_MODE_PULSED:
  default:
    return "Pulsed Control";
  }
}

const char *TemporalDeck::freezeCvModeLabelFor(int index) {
  switch (index) {
  case FREEZE_CV_MODE_GATE:
    return "Gated Control";
  case FREEZE_CV_MODE_PULSED:
  default:
    return "Pulsed Control";
  }
}

const char *TemporalDeck::platterArtModeLabelFor(int index) {
  switch (index) {
  case PLATTER_ART_DRAGON_KING:
    return "Dragon King";
  case PLATTER_ART_BLANK:
    return "Blank";
  case PLATTER_ART_TEMPORAL_DECK:
    return "Temporal Deck";
  case PLATTER_ART_PROCEDURAL:
    return "Procedural";
  case PLATTER_ART_CUSTOM:
    return "Custom file";
  case PLATTER_ART_BUILTIN_SVG:
  default:
    return "Built-in SVG";
  }
}

const char *TemporalDeck::platterBrightnessLabelFor(int index) {
  switch (index) {
  case PLATTER_BRIGHTNESS_LOW:
    return "Low";
  case PLATTER_BRIGHTNESS_MEDIUM:
    return "Medium";
  case PLATTER_BRIGHTNESS_FULL:
  default:
    return "Full";
  }
}
