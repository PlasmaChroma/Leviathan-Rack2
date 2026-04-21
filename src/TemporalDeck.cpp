#include "TemporalDeckArcLights.hpp"
#include "TemporalDeck.hpp"
#include "TemporalDeckEngine.hpp"
#include "TemporalDeckExpanderProtocol.hpp"
#include "TemporalDeckFrameInput.hpp"
#include "TemporalDeckPlatterInput.hpp"
#include "TemporalDeckSampleLifecycle.hpp"
#include "TemporalDeckTransportControl.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <limits>
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
constexpr int TemporalDeck::BUFFER_DURATION_COUNT;

constexpr int TemporalDeck::EXTERNAL_GATE_POS_GLIDE;
constexpr int TemporalDeck::EXTERNAL_GATE_POS_MODULE_SYNC;
constexpr int TemporalDeck::EXTERNAL_GATE_POS_COUNT;

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

// Push scope payload faster so TD.Scope tracks live output motion more tightly.
static constexpr float kExpanderPublishRateHz = 120.f;
static constexpr float kExpanderPublishIntervalSec = 1.f / kExpanderPublishRateHz;
static constexpr float kScopeHalfWindowMs = 900.f;
static constexpr float kScopeHalfWindowSeconds = kScopeHalfWindowMs * 0.001f;
static constexpr int kScopeEvaluationBudgetPerPublish = 16384;
static constexpr int kScopeLagFpShift = 10;
static constexpr int64_t kScopeLagFpOne = int64_t(1) << kScopeLagFpShift;

static std::string temporalDeckUserRootPathForTrace() { return system::join(asset::user(), "Leviathan/TemporalDeck"); }

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

static float readScopeChannelAtLagSamples(const TemporalDeckEngine &engine, double newestPos, bool sampleMode,
                                          bool sampleLoopEnabled, double lagSamples, ScopeChannelMode channelMode) {
  if (engine.buffer.size <= 0) {
    return 0.f;
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
  float left = engine.buffer.left[size_t(idx)];
  float right = engine.buffer.rightSample(idx);
  return reduceScopeChannelValue(left, right, channelMode);
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
  out->binCount = temporaldeck_expander::SCOPE_BIN_COUNT;
  if (out->binCount == 0u || engine.buffer.size <= 0) {
    return false;
  }

  float sr = std::max(sampleRate, 1.f);
  float halfWindowSamples = sr * kScopeHalfWindowSeconds;
  float totalWindowSamples = std::max(1.f, 2.f * halfWindowSamples);
  int64_t totalWindowLagFp =
    std::max<int64_t>(kScopeLagFpOne, int64_t(std::llround(double(totalWindowSamples) * double(kScopeLagFpOne))));
  out->binSpanLagFp = std::max<int64_t>(1, totalWindowLagFp / int64_t(out->binCount));
  out->binSpanSamples = float(double(out->binSpanLagFp) / double(kScopeLagFpOne));
  int totalWindowSamplesInt = std::max(1, int(std::ceil(totalWindowSamples)));
  int effectiveEvalBudget = kScopeEvaluationBudgetPerPublish;
  out->scopeStride = std::max(1, int(std::ceil(double(totalWindowSamplesInt) / double(effectiveEvalBudget))));

  float forwardWindowSamples = halfWindowSamples;
  float backwardWindowSamples = halfWindowSamples;
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

  out->newestPos = sampleMode ? double(std::max(0.f, accessibleLagSamples)) : engine.newestReadablePos();
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
      mono =
        readScopeChannelAtLagSamples(engine, params.newestPos, params.sampleMode, params.sampleLoopEnabled, double(lag), channelMode);
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

static bool canReuseScopeWindowCache(const ScopeWindowParams &current, const ScopeWindowCache &cache) {
  if (!cache.valid || cache.scopeBinCount == 0u) {
    return false;
  }
  if (!current.sampleMode) {
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
  std::array<temporaldeck_expander::ScopeBin, temporaldeck_expander::SCOPE_BIN_COUNT> *binsOut) {
  if (!binsOut) {
    return 0u;
  }

  uint32_t scopeBinCount = 0u;
  bool reused = false;
  if (cache && canReuseScopeWindowCache(params, *cache)) {
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
                                   int frames, int channels, float sampleRate, std::string *errorOut) {
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
    int16_t l = floatToPcm16(left[size_t(i)]);
    writeLe16(out, uint16_t(l));
    if (channels == 2) {
      int16_t r = floatToPcm16(right[size_t(i)]);
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
  dsp::SchmittTrigger reverseTrigger;
  dsp::SchmittTrigger slipTrigger;
  dsp::SchmittTrigger cartridgeCycleTrigger;
  float cachedSampleRate = 0.f;
  temporaldeck_transport::TransportControlState transportControl;
  temporaldeck_lifecycle::TemporalDeckSampleLifecycle sampleLifecycle;
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
  int expanderLagDragFramesSinceUpdate = 0;
  int scratchInterpolationMode = TemporalDeck::SCRATCH_INTERP_LAGRANGE6;
  bool highQualityRateInterpolation = false;
  bool platterTraceLoggingEnabled = false;
  bool scopeDragTraceLoggingEnabled = false;
  bool scopeDragTraceCaptureActive = false;
  std::ofstream scopeDragTraceFile;
  std::string scopeDragTracePath;
  double scopeDragTraceStartTimeSec = 0.0;
  uint64_t scopeDragTraceSequence = 0;
  bool scopeDragTraceWasActive = false;
  bool scopeDragTraceHavePrev = false;
  float scopeDragTracePrevTargetLag = 0.f;
  float scopeDragTracePrevFrameLag = 0.f;
  int scopeDragTraceStallFrames = 0;
  float scopeDragTraceLogTimerSec = 0.f;
  int cartridgeCharacter = TemporalDeck::CARTRIDGE_CLEAN;
  std::atomic<int> bufferDurationMode{TemporalDeck::BUFFER_DURATION_10S};
  int externalGatePosMode = TemporalDeck::EXTERNAL_GATE_POS_GLIDE;
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
  config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
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
  configInput(POSITION_CV_INPUT, "Position CV");
  configInput(RATE_CV_INPUT, "Rate CV");
  configInput(INPUT_L_INPUT, "Left audio");
  configInput(INPUT_R_INPUT, "Right audio");
  configInput(SCRATCH_GATE_INPUT, "Scratch gate");
  configInput(FREEZE_GATE_INPUT, "Freeze gate");
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
  if (impl) {
    impl->sampleLifecycle.stopWorker();
  }
}

float TemporalDeck::scratchSensitivity() {
  return ScratchSensitivityQuantity::sensitivityForValue(params[SCRATCH_SENSITIVITY_PARAM].getValue());
}

void TemporalDeck::applyBufferDurationMode(int mode) {
  int clamped = clamp(mode, 0, BUFFER_DURATION_COUNT - 1);
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
  // Reconfiguration is applied on the audio thread from process() to avoid
  // cross-thread buffer reallocations.
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
  json_object_set_new(root, "slipReturnMode", json_integer(impl->transportControl.slipReturnMode));
  json_object_set_new(root, "cartridgeCharacter", json_integer(impl->cartridgeCharacter));
  json_object_set_new(root, "bufferDurationMode", json_integer(impl->bufferDurationMode.load()));
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
  json_t *slipReturnModeJ = json_object_get(root, "slipReturnMode");
  json_t *cartridgeJ = json_object_get(root, "cartridgeCharacter");
  json_t *bufferDurationJ = json_object_get(root, "bufferDurationMode");
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
  if (slipReturnModeJ) {
    impl->transportControl.slipReturnMode = clamp((int)json_integer_value(slipReturnModeJ), SLIP_RETURN_SLOW, SLIP_RETURN_COUNT - 1);
  }
  if (cartridgeJ) {
    impl->cartridgeCharacter = clamp((int)json_integer_value(cartridgeJ), 0, CARTRIDGE_COUNT - 1);
  }
  if (bufferDurationJ) {
    impl->bufferDurationMode.store(clamp((int)json_integer_value(bufferDurationJ), 0, BUFFER_DURATION_COUNT - 1));
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
  if (impl->sampleLifecycle.consumeAllocationFallbackPending()) {
    impl->sampleLifecycle.clearDecodedAndPreparedState();
    impl->sampleModeEnabled.store(false, std::memory_order_relaxed);
    impl->bufferDurationMode.store(BUFFER_DURATION_10S, std::memory_order_relaxed);
    impl->sampleLifecycle.setPendingSampleStateApply();
  }

  PreparedSampleData prepared;
  if (impl->sampleLifecycle.consumePendingPreparedSample(&prepared)) {
    impl->cachedSampleRate = prepared.sampleRate;
    impl->bufferDurationMode.store(prepared.bufferMode, std::memory_order_relaxed);
    impl->engine.bufferDurationMode = prepared.bufferMode;
    impl->engine.reset(prepared.sampleRate, false);
    impl->engine.sampleModeEnabled = impl->sampleModeEnabled.load(std::memory_order_relaxed);
    impl->engine.sampleLoopEnabled = impl->sampleLoopEnabled.load(std::memory_order_relaxed);
    impl->engine.externalGatePosMode = impl->externalGatePosMode;
    impl->engine.installPreparedSample(std::move(prepared.left), std::move(prepared.right), prepared.frames,
                                       prepared.truncated, prepared.monoStorage);
    impl->sampleModeEnabled.store(true, std::memory_order_relaxed);
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
  }

  int requestedBufferMode = clamp(impl->bufferDurationMode.load(std::memory_order_relaxed), 0, BUFFER_DURATION_COUNT - 1);
  bool bufferModeChanged = requestedBufferMode != impl->engine.bufferDurationMode;
  bool sampleStateApplyRequested = impl->sampleLifecycle.consumePendingSampleStateApply();
  bool sampleRateChanged = args.sampleRate != impl->cachedSampleRate;
  bool decodedAvailable = impl->sampleLifecycle.decodedSampleAvailable();
  bool shouldRebuildLoadedSample = decodedAvailable && (bufferModeChanged || sampleRateChanged || sampleStateApplyRequested);
  if (!decodedAvailable && (bufferModeChanged || sampleRateChanged || sampleStateApplyRequested)) {
    applySampleRateChange(args.sampleRate);
  } else if (shouldRebuildLoadedSample && !impl->sampleLifecycle.sampleBuildInProgress()) {
    temporaldeck_lifecycle::TemporalDeckSampleLifecycle::AsyncSampleBuildRequest request;
    request.type = temporaldeck_lifecycle::TemporalDeckSampleLifecycle::AsyncSampleBuildRequest::REBUILD_FROM_DECODED;
    request.targetSampleRate = args.sampleRate;
    request.requestedBufferMode = requestedBufferMode;
    impl->sampleLifecycle.requestAsyncSampleBuild(request);
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
  transportButtons.freezePressed = impl->freezeTrigger.process(params[FREEZE_PARAM].getValue());
  transportButtons.reversePressed = impl->reverseTrigger.process(params[REVERSE_PARAM].getValue());
  transportButtons.slipPressed = impl->slipTrigger.process(params[SLIP_PARAM].getValue());
  temporaldeck_transport::TransportButtonResult transportResult = temporaldeck_transport::applyTransportButtonEvents(
    impl->transportControl, transportButtons, desiredSampleModeEnabled, impl->engine.sampleLoaded);
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
  bool freezeGateHigh = signalIn.freezeGateHigh;
  bool scratchGateHigh = signalIn.scratchGateHigh;
  bool scratchGateConnected = signalIn.scratchGateConnected;
  bool positionConnected = signalIn.positionConnected;
  temporaldeck_transport::applyFreezeGateEdge(impl->transportControl, freezeGateHigh);

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
  impl->appliedSampleSeekRevision = temporaldeck_transport::applyPendingSampleSeek(
    impl->engine, impl->appliedSampleSeekRevision, pendingSeekRevision, pendingSeekNorm, bufferKnob);
  impl->appliedLiveSeekRevision = temporaldeck_transport::applyPendingLiveSeekArc(
    impl->engine, impl->appliedLiveSeekRevision, pendingLiveSeekRevision, pendingLiveSeekArcNorm, bufferKnob);

  bool scopeTraceDebugEnabled = isDragonKingDebugEnabled();
  if (!scopeTraceDebugEnabled && impl->scopeDragTraceLoggingEnabled) {
    impl->scopeDragTraceLoggingEnabled = false;
  }
  if (impl->scopeDragTraceLoggingEnabled && scopeTraceDebugEnabled && !impl->scopeDragTraceCaptureActive) {
    std::string traceDir = system::join(temporalDeckUserRootPathForTrace(), "scope_traces");
    system::createDirectories(traceDir);
    long long stampMs = (long long)std::llround(system::getUnixTime() * 1000.0);
    std::string filename = "scope_drag_trace_" + std::to_string(stampMs) + ".csv";
    impl->scopeDragTracePath = system::join(traceDir, filename);
    impl->scopeDragTraceFile.open(impl->scopeDragTracePath.c_str(), std::ios::out | std::ios::trunc);
    if (!impl->scopeDragTraceFile.good()) {
      WARN("TemporalDeck: failed to open scope drag trace file: %s", impl->scopeDragTracePath.c_str());
      impl->scopeDragTracePath.clear();
      impl->scopeDragTraceLoggingEnabled = false;
    } else {
      impl->scopeDragTraceFile.setf(std::ios::fixed);
      impl->scopeDragTraceFile << std::setprecision(6);
      impl->scopeDragTraceFile << "# TemporalDeck scope drag trace v1\n";
      impl->scopeDragTraceFile << "# Start when scope drag trace logging is enabled, stop when it is disabled\n";
      impl->scopeDragTraceFile
        << "seq,t_sec,event,request_seq,scope_active,new_request,just_started,stall_frames,target_lag,target_delta,"
           "request_velocity,applied_velocity,frame_lag,frame_lag_delta,freeze,sample_mode\n";
      impl->scopeDragTraceStartTimeSec = system::getTime();
      impl->scopeDragTraceSequence = 0;
      impl->scopeDragTraceCaptureActive = true;
      INFO("TemporalDeck: scope drag trace capture started: %s", impl->scopeDragTracePath.c_str());
    }
  } else if ((!impl->scopeDragTraceLoggingEnabled || !scopeTraceDebugEnabled) && impl->scopeDragTraceCaptureActive) {
    if (impl->scopeDragTraceFile.good()) {
      impl->scopeDragTraceFile.flush();
      impl->scopeDragTraceFile.close();
    }
    INFO("TemporalDeck: scope drag trace capture saved: %s", impl->scopeDragTracePath.c_str());
    impl->scopeDragTraceCaptureActive = false;
    impl->scopeDragTraceStartTimeSec = 0.0;
    impl->scopeDragTraceSequence = 0;
    impl->scopeDragTracePath.clear();
  }

  bool expanderConnected =
    isTDScopeModule(rightExpander.module) && rightExpander.module->leftExpander.producerMessage;
  uint32_t requestedScopeFormat = temporaldeck_expander::SCOPE_FORMAT_MONO;
  bool haveLagDragRequest = false;
  bool lagDragRequestActive = false;
  float lagDragRequestSamples = 0.f;
  float lagDragRequestVelocity = 0.f;
  uint64_t lagDragRequestSeq = 0u;
  bool scopeTraceNewRequest = false;
  bool scopeTraceDragJustStarted = false;
  float scopeTraceLagTarget = 0.f;
  float scopeTraceVelocityApplied = 0.f;
  if (isTDScopeModule(rightExpander.module) && rightExpander.consumerMessage) {
    const auto *request =
      reinterpret_cast<const temporaldeck_expander::DisplayToHost *>(rightExpander.consumerMessage);
    if (request && temporaldeck_expander::isDisplayRequestValid(*request)) {
      haveLagDragRequest = true;
      lagDragRequestSeq = request->requestSeq;
      requestedScopeFormat = (request->requestedScopeFormat == temporaldeck_expander::SCOPE_FORMAT_STEREO)
                               ? temporaldeck_expander::SCOPE_FORMAT_STEREO
                               : temporaldeck_expander::SCOPE_FORMAT_MONO;
      lagDragRequestActive = temporaldeck_expander::decodeLagDragRequest(request->reserved, &lagDragRequestSamples);
      if (request->version >= 2u) {
        lagDragRequestVelocity = request->lagDragVelocity;
      }
      if (!std::isfinite(lagDragRequestSamples) || lagDragRequestSamples < 0.f) {
        lagDragRequestSamples = 0.f;
      }
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
      float lagTarget = clamp(lagDragRequestSamples, 0.f, maxLag);
      scopeTraceNewRequest = true;
      scopeTraceLagTarget = lagTarget;
      if (lagDragRequestActive) {
        bool dragJustStarted = !impl->expanderLagDragWasActive;
        scopeTraceDragJustStarted = dragJustStarted;
        if (dragJustStarted) {
          // Scope touch-down should latch a stationary hold without creating
          // a fresh gesture that invokes write-head compensation motion.
          impl->platterInput.setTouchHold(true, lagTarget);
          impl->platterInput.setMotionFreshSamples(0);
          scopeTraceVelocityApplied = 0.f;
        } else {
          float velocitySamples = 0.f;
          int frames = std::max(1, impl->expanderLagDragFramesSinceUpdate);
          float dtSec = std::max(args.sampleTime, float(frames) * args.sampleTime);
          // WARNING: Keep derived velocity in the same convention as
          // incoming scope velocity before blending.
          // positive velocity => toward NOW (decreasing lag).
          float derivedVelocity = (impl->expanderLagDragLastLagSamples - lagTarget) / dtSec;
          if (std::fabs(lagDragRequestVelocity) > 1e-6f) {
            // Scope drag should feel equivalent to direct platter motion:
            // trust scope-provided gesture velocity when available so
            // reversals are immediate instead of inertia-smoothed by host.
            velocitySamples = lagDragRequestVelocity;
          } else {
            velocitySamples = derivedVelocity;
          }
          // Safety clamp: Scope is an external input path, so guard against
          // sender-side timing spikes producing unrealistic multi-turn motion.
          float maxAbsGestureVelocity = std::max(args.sampleRate * 3.0f, 1.0f);
          velocitySamples = clamp(velocitySamples, -maxAbsGestureVelocity, maxAbsGestureVelocity);
          scopeTraceVelocityApplied = velocitySamples;
          // CONTAINMENT NOTE
          // This receive path is where expander-supplied scope drag becomes a
          // "real" platter gesture for the engine. That means Scope and host
          // currently share behavior responsibility, even though the intended
          // architecture is for Scope to be mostly I/O. Keep that in mind when
          // refactoring: if this translation is simplified without also moving
          // the live-drag workarounds out of TDScope, regressions will look
          // like stalled live drags, over-travel, or delayed reversals.
          // Scope emits equivalent platter-gesture intent. TemporalDeck
          // realizes that intent through the same scratch gesture path used
          // by actual platter dragging.
          impl->platterInput.setScratch(true, lagTarget, velocitySamples);
          int motionFreshSamples = int(std::round(args.sampleRate * std::max(args.sampleTime, dtSec) * 1.5f));
          int minHoldSamples = int(std::round(args.sampleRate * 0.025f));
          int maxHoldSamples = int(std::round(args.sampleRate * 0.090f));
          motionFreshSamples = clamp(motionFreshSamples, minHoldSamples, maxHoldSamples);
          impl->platterInput.setMotionFreshSamples(motionFreshSamples);
        }
          impl->expanderLagDragLastLagSamples = lagTarget;
          impl->expanderLagDragFramesSinceUpdate = 0;
        impl->expanderLagDragWasActive = true;
      } else {
        if (impl->expanderLagDragWasActive) {
          impl->platterInput.setScratch(false, impl->expanderLagDragLastLagSamples, 0.f);
          impl->platterInput.setMotionFreshSamples(0);
        }
        impl->expanderLagDragWasActive = false;
        impl->expanderLagDragFramesSinceUpdate = 0;
      }
    } else if (impl->expanderLagDragWasActive) {
      impl->expanderLagDragFramesSinceUpdate = std::min(impl->expanderLagDragFramesSinceUpdate + 1, 1 << 20);
    }
  } else {
    impl->expanderLagDragRequestSeen = false;
  }
  PlatterInputSnapshot platterInput = impl->platterInput.consumeForFrame();

  temporaldeck_frameinput::FrameInputControls controls;
  controls.dt = args.sampleTime;
  controls.bufferKnob = params[BUFFER_PARAM].getValue();
  controls.rateKnob = params[RATE_PARAM].getValue();
  controls.mixKnob = params[MIX_PARAM].getValue();
  controls.feedbackKnob = params[FEEDBACK_PARAM].getValue();
  controls.freezeButton = impl->transportControl.freezeLatched;
  controls.reverseButton = impl->transportControl.reverseLatched;
  controls.slipButton = impl->transportControl.slipLatched;

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

  if (impl->scopeDragTraceLoggingEnabled && scopeTraceDebugEnabled) {
    bool scopeActive = haveLagDragRequest && lagDragRequestActive;
    if (!scopeActive) {
      if (impl->scopeDragTraceCaptureActive && impl->scopeDragTraceFile.good() && impl->scopeDragTraceWasActive) {
        double tSec = std::max(0.0, system::getTime() - impl->scopeDragTraceStartTimeSec);
        impl->scopeDragTraceFile << impl->scopeDragTraceSequence++ << "," << tSec << ",SCOPE_DRAG_END,"
                                 << (unsigned long long)lagDragRequestSeq << ",0,0,0,0,0,0,0,0,"
                                 << float(frame.lag) << ",0,0," << (frame.sampleMode ? 1 : 0) << "\n";
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
        if (impl->scopeDragTraceCaptureActive && impl->scopeDragTraceFile.good()) {
          double tSec = std::max(0.0, system::getTime() - impl->scopeDragTraceStartTimeSec);
          impl->scopeDragTraceFile << impl->scopeDragTraceSequence++ << "," << tSec << ",SCOPE_DRAG,"
                                   << (unsigned long long)lagDragRequestSeq << "," << (scopeActive ? 1 : 0) << ","
                                   << (scopeTraceNewRequest ? 1 : 0) << "," << (scopeTraceDragJustStarted ? 1 : 0) << ","
                                   << impl->scopeDragTraceStallFrames << "," << targetLag << "," << targetDelta << ","
                                   << lagDragRequestVelocity << "," << scopeTraceVelocityApplied << "," << float(frame.lag)
                                   << "," << frameLagDelta << "," << (freezeTrace ? 1 : 0) << ","
                                   << (frame.sampleMode ? 1 : 0) << "\n";
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

    bool justConnected = !impl->expanderWasConnected;
    bool generationChanged = impl->engine.bufferGeneration != impl->expanderLastPublishedGeneration;
    impl->expanderPublishTimerSec += args.sampleTime;
    bool timerElapsed = impl->expanderPublishTimerSec >= kExpanderPublishIntervalSec;
    bool shouldPublish = justConnected || generationChanged || timerElapsed;
    if (shouldPublish) {
      if (timerElapsed) {
        impl->expanderPublishTimerSec = std::fmod(impl->expanderPublishTimerSec, kExpanderPublishIntervalSec);
      } else {
        impl->expanderPublishTimerSec = 0.f;
      }
      auto *msg =
        reinterpret_cast<temporaldeck_expander::HostToDisplay *>(rightExpander.module->leftExpander.producerMessage);
      if (msg) {
        bool holdPreviewLag =
          !frame.sampleMode && platterInput.platterTouched && !platterInput.platterMotionActive && !platterInput.wheelScratchHeld;
        float scopeLagForPreview = float(frame.lag);
        if (holdPreviewLag) {
          if (!impl->expanderScopeLagHoldActive) {
            impl->expanderScopeLagHoldSamples = scopeLagForPreview;
            impl->expanderScopeLagHoldActive = true;
          }
          scopeLagForPreview = impl->expanderScopeLagHoldSamples;
          if (!impl->expanderScopeNewestPosHoldActive) {
            impl->expanderScopeNewestPosHold = impl->engine.newestReadablePos();
            impl->expanderScopeNewestAbsolutePosHold = impl->engine.newestReadableAbsolutePos();
            impl->expanderScopeNewestPosHoldActive = true;
          }
        } else {
          impl->expanderScopeLagHoldActive = false;
          impl->expanderScopeNewestPosHoldActive = false;
        }
        double scopeLiveNewestAbsolutePosOverride =
          impl->expanderScopeNewestPosHoldActive ? impl->expanderScopeNewestAbsolutePosHold : -1.0;

        std::array<temporaldeck_expander::ScopeBin, temporaldeck_expander::SCOPE_BIN_COUNT> scopeBins;
        std::array<temporaldeck_expander::ScopeBin, temporaldeck_expander::SCOPE_BIN_COUNT> scopeBinsRight;
        float scopeStartLagSamples = 0.f;
        float scopeBinSpanSamples = 1.f;
        uint32_t scopeBinCount = 0u;
        ScopeWindowParams scopeParams;
        auto scopePreviewMeasureStart = std::chrono::steady_clock::now();
        bool haveScopeParams = computeScopeWindowParams(
          impl->engine, frame.sampleMode, impl->sampleLoopEnabled.load(std::memory_order_relaxed), impl->cachedSampleRate,
          scopeLagForPreview, float(frame.accessibleLag), scopeLiveNewestAbsolutePosOverride, &scopeParams);
        if (haveScopeParams) {
          ScopeChannelMode leftMode = wantStereoScope ? SCOPE_CHANNEL_LEFT : SCOPE_CHANNEL_MID;
          uint32_t leftCount =
            buildScopeWindowBinsWithCache(impl->engine, scopeParams, leftMode, &impl->expanderScopeCacheMono, &scopeBins);
          uint32_t rightCount = 0u;
          if (wantStereoScope) {
            rightCount = buildScopeWindowBinsWithCache(
              impl->engine, scopeParams, SCOPE_CHANNEL_RIGHT, &impl->expanderScopeCacheRight, &scopeBinsRight);
            scopeBinCount = (leftCount > 0u && rightCount > 0u) ? std::min(leftCount, rightCount) : 0u;
          } else {
            impl->expanderScopeCacheRight.valid = false;
            scopeBinCount = leftCount;
          }
          scopeStartLagSamples = scopeParams.scopeStartLagSamples;
          scopeBinSpanSamples = scopeParams.binSpanSamples;
        } else {
          impl->expanderScopeCacheMono.valid = false;
          impl->expanderScopeCacheRight.valid = false;
        }
        auto scopePreviewMeasureEnd = std::chrono::steady_clock::now();
        float scopePreviewMeasureUs =
          float(std::chrono::duration_cast<std::chrono::microseconds>(scopePreviewMeasureEnd - scopePreviewMeasureStart).count());
        float previousMeasureUs = impl->uiScopePreviewCostUs.load(std::memory_order_relaxed);
        float smoothedMeasureUs =
          (previousMeasureUs > 0.f) ? (previousMeasureUs + (scopePreviewMeasureUs - previousMeasureUs) * 0.25f) : scopePreviewMeasureUs;
        impl->uiScopePreviewCostUs.store(smoothedMeasureUs, std::memory_order_relaxed);
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
        if (impl->engine.buffer.monoStorage) {
          flags |= temporaldeck_expander::FLAG_MONO_BUFFER;
        }

        impl->expanderPublishSeq++;
        float sampleAbsolutePeakVolts =
          (frame.sampleMode && frame.sampleLoaded) ? impl->engine.sampleAbsolutePeakVolts : impl->engine.getLiveAbsolutePeakVolts();
        float combinedSensitivity = scratchSensitivity() * kMouseScratchTravelScale;
        temporaldeck_expander::populateHostMessage(
          msg, impl->expanderPublishSeq, impl->engine.bufferGeneration, flags, impl->cachedSampleRate, scopeLagForPreview,
          float(frame.accessibleLag), frame.platterAngle, float(frame.samplePlayhead), float(frame.sampleDuration),
          float(frame.sampleProgress), sampleAbsolutePeakVolts, combinedSensitivity,
          uint32_t(std::max(0, impl->engine.buffer.size)),
          uint32_t(std::max(0, impl->engine.buffer.filled)), kScopeHalfWindowMs, scopeStartLagSamples,
          scopeBinSpanSamples, scopeBinCount, scopeBins.data(), wantStereoScope ? scopeBinsRight.data() : nullptr);        rightExpander.module->leftExpander.messageFlipRequested = true;
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
    impl->expanderLagDragFramesSinceUpdate = 0;
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
}

void TemporalDeck::setPlatterScratch(bool touched, float lagSamples, float velocitySamples, int holdSamples) {
  impl->platterInput.setScratch(touched, lagSamples, velocitySamples, holdSamples);
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

float TemporalDeck::getUiScopePreviewCostUs() const {
  return impl->uiScopePreviewCostUs.load(std::memory_order_relaxed);
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
  impl->sampleLifecycle.clearDecodedAndPreparedState();
  temporaldeck_lifecycle::TemporalDeckSampleLifecycle::AsyncSampleBuildRequest cancelRequest;
  cancelRequest.type = temporaldeck_lifecycle::TemporalDeckSampleLifecycle::AsyncSampleBuildRequest::NONE;
  cancelRequest.targetSampleRate = std::max(impl->cachedSampleRate, 1.f);
  cancelRequest.requestedBufferMode = impl->bufferDurationMode.load(std::memory_order_relaxed);
  impl->sampleLifecycle.requestAsyncSampleBuild(cancelRequest);
  impl->sampleModeEnabled.store(false, std::memory_order_relaxed);
  impl->bufferDurationMode.store(BUFFER_DURATION_10S);
  if (paramQuantities[BUFFER_PARAM]) {
    paramQuantities[BUFFER_PARAM]->displayMultiplier = usableBufferSecondsForMode(BUFFER_DURATION_10S);
  }
  impl->sampleLifecycle.setPendingSampleStateApply();
}

bool TemporalDeck::loadSampleFromPath(const std::string &path, std::string *errorOut) {
  (void)errorOut;
  bool wasFreezeLatched = impl->transportControl.freezeLatched;
  bool wasFreezeLatchedByButton = impl->transportControl.freezeLatchedByButton;
  impl->transportControl.freezeLatched = wasFreezeLatched;
  impl->transportControl.freezeLatchedByButton = impl->transportControl.freezeLatched ? wasFreezeLatchedByButton : false;
  impl->transportControl.reverseLatched = false;
  impl->transportControl.slipLatched = false;
  impl->pendingLegacySampleFreezeOnPreparedInstall = false;

  temporaldeck_lifecycle::TemporalDeckSampleLifecycle::AsyncSampleBuildRequest request;
  request.type = temporaldeck_lifecycle::TemporalDeckSampleLifecycle::AsyncSampleBuildRequest::LOAD_PATH;
  request.path = path;
  request.targetSampleRate = std::max(impl->cachedSampleRate, 1.f);
  request.requestedBufferMode = impl->bufferDurationMode.load(std::memory_order_relaxed);
  impl->sampleLifecycle.requestAsyncSampleBuild(request);
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
  int frames = impl->engine.sampleFrames;
  int channels = impl->engine.buffer.monoStorage ? 1 : 2;
  if (!writeStereoOrMonoWav16(path, impl->engine.buffer.left, impl->engine.buffer.right, frames, channels,
                              impl->engine.sampleRate, errorOut)) {
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
  return impl->platterTraceLoggingEnabled;
}

void TemporalDeck::setPlatterTraceLoggingEnabled(bool enabled) {
  impl->platterTraceLoggingEnabled = enabled;
}

bool TemporalDeck::isScopeDragTraceLoggingEnabled() const {
  return impl->scopeDragTraceLoggingEnabled;
}

void TemporalDeck::setScopeDragTraceLoggingEnabled(bool enabled) {
  impl->scopeDragTraceLoggingEnabled = enabled;
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
    impl->transportControl.reverseLatched = false;
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
  case BUFFER_DURATION_10M_STEREO:
    return "10 min stereo";
  case BUFFER_DURATION_10M_MONO:
    return "10 min mono";
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
