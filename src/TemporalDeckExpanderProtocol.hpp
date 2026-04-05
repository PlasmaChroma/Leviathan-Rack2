#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <type_traits>

namespace temporaldeck_expander {

constexpr uint32_t MAGIC = 0x54445831u; // "TDX1"
constexpr uint16_t VERSION = 2u;
constexpr uint32_t PREVIEW_BIN_COUNT = 4096u;
constexpr uint32_t SCOPE_BIN_COUNT = 1024u;
constexpr float kPreviewQuantizeVolts = 10.f;

enum HostFlags : uint32_t {
  FLAG_SAMPLE_MODE = 1u << 0,
  FLAG_SAMPLE_LOADED = 1u << 1,
  FLAG_SAMPLE_PLAYING = 1u << 2,
  FLAG_SAMPLE_LOOP = 1u << 3,
  FLAG_FREEZE = 1u << 4,
  FLAG_REVERSE = 1u << 5,
  FLAG_SLIP = 1u << 6,
  FLAG_PREVIEW_VALID = 1u << 7,
  FLAG_MONO_BUFFER = 1u << 8,
};

struct ScopeBin {
  int16_t min = 0;
  int16_t max = 0;
};

using PreviewBin = ScopeBin;

inline ScopeBin makeEmptyScopeBin() {
  ScopeBin bin;
  bin.min = 1;
  bin.max = 0;
  return bin;
}

inline bool isScopeBinValid(const ScopeBin &bin) { return bin.max >= bin.min; }

struct HostToDisplay {
  uint32_t magic = MAGIC;
  uint16_t version = VERSION;
  uint16_t size = uint16_t(sizeof(HostToDisplay));

  uint64_t publishSeq = 0;
  uint64_t bufferGeneration = 0;

  uint32_t flags = 0;

  float sampleRate = 44100.f;
  float lagSamples = 0.f;
  float accessibleLagSamples = 0.f;
  float platterAngle = 0.f;
  float samplePlayheadSec = 0.f;
  float sampleDurationSec = 0.f;
  float sampleProgress = 0.f;

  uint32_t bufferCapacityFrames = 0;
  uint32_t bufferFilledFrames = 0;

  float scopeHalfWindowMs = 900.f;
  float scopeStartLagSamples = 0.f;
  float scopeBinSpanSamples = 1.f;
  uint32_t scopeBinCount = 0;

  ScopeBin scope[SCOPE_BIN_COUNT];
};

static_assert(std::is_standard_layout<HostToDisplay>::value, "HostToDisplay must stay POD-like");

inline int16_t quantizePreviewSample(float monoVolts) {
  float clamped = std::max(-kPreviewQuantizeVolts, std::min(monoVolts, kPreviewQuantizeVolts));
  float scaled = (clamped / kPreviewQuantizeVolts) * 32767.f;
  return int16_t(std::lrint(scaled));
}

struct PreviewAccumulator {
  std::array<PreviewBin, PREVIEW_BIN_COUNT> bins;
  uint32_t writeIndex = 0;
  uint32_t filledBins = 0;
  uint32_t samplesPerBin = 1;
  uint32_t samplesInCurrentBin = 0;
  int16_t currentMin = 0;
  int16_t currentMax = 0;

  void reset(uint32_t bufferCapacityFrames) {
    writeIndex = 0;
    filledBins = 0;
    samplesPerBin = std::max(1u, bufferCapacityFrames / PREVIEW_BIN_COUNT);
    samplesInCurrentBin = 0;
    currentMin = 0;
    currentMax = 0;
    for (size_t i = 0; i < bins.size(); ++i) {
      bins[i].min = 0;
      bins[i].max = 0;
    }
  }

  void pushMonoSample(float monoVolts) {
    int16_t q = quantizePreviewSample(monoVolts);
    if (samplesInCurrentBin == 0) {
      currentMin = q;
      currentMax = q;
    } else {
      currentMin = std::min(currentMin, q);
      currentMax = std::max(currentMax, q);
    }
    samplesInCurrentBin++;
    if (samplesInCurrentBin < samplesPerBin) {
      return;
    }
    bins[writeIndex].min = currentMin;
    bins[writeIndex].max = currentMax;
    writeIndex = (writeIndex + 1u) % PREVIEW_BIN_COUNT;
    filledBins = std::min(filledBins + 1u, PREVIEW_BIN_COUNT);
    samplesInCurrentBin = 0;
  }

  void finalizePartialBin() {
    if (samplesInCurrentBin == 0) {
      return;
    }
    bins[writeIndex].min = currentMin;
    bins[writeIndex].max = currentMax;
    writeIndex = (writeIndex + 1u) % PREVIEW_BIN_COUNT;
    filledBins = std::min(filledBins + 1u, PREVIEW_BIN_COUNT);
    samplesInCurrentBin = 0;
  }
};

inline void populateHostMessage(HostToDisplay *out, uint64_t publishSeq, uint64_t bufferGeneration, uint32_t flags,
                                float sampleRate, float lagSamples, float accessibleLagSamples, float platterAngle,
                                float samplePlayheadSec, float sampleDurationSec, float sampleProgress,
                                uint32_t bufferCapacityFrames, uint32_t bufferFilledFrames,
                                float scopeHalfWindowMs, float scopeStartLagSamples, float scopeBinSpanSamples,
                                uint32_t scopeBinCount, const ScopeBin *scopeBins) {
  if (!out) {
    return;
  }
  out->magic = MAGIC;
  out->version = VERSION;
  out->size = uint16_t(sizeof(HostToDisplay));
  out->publishSeq = publishSeq;
  out->bufferGeneration = bufferGeneration;
  out->flags = flags;
  out->sampleRate = sampleRate;
  out->lagSamples = lagSamples;
  out->accessibleLagSamples = accessibleLagSamples;
  out->platterAngle = platterAngle;
  out->samplePlayheadSec = samplePlayheadSec;
  out->sampleDurationSec = sampleDurationSec;
  out->sampleProgress = sampleProgress;
  out->bufferCapacityFrames = bufferCapacityFrames;
  out->bufferFilledFrames = bufferFilledFrames;
  out->scopeHalfWindowMs = scopeHalfWindowMs;
  out->scopeStartLagSamples = scopeStartLagSamples;
  out->scopeBinSpanSamples = scopeBinSpanSamples;
  out->scopeBinCount = std::min(scopeBinCount, SCOPE_BIN_COUNT);
  ScopeBin empty = makeEmptyScopeBin();
  for (size_t i = 0; i < SCOPE_BIN_COUNT; ++i) {
    out->scope[i] = empty;
  }
  if (scopeBins && out->scopeBinCount > 0) {
    std::memcpy(out->scope, scopeBins, sizeof(ScopeBin) * size_t(out->scopeBinCount));
  }
}

} // namespace temporaldeck_expander
