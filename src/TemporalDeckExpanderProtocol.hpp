#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <type_traits>

namespace temporaldeck_expander {

constexpr uint32_t MAGIC = 0x54445831u; // "TDX1"
constexpr uint16_t VERSION = 4u;
constexpr uint32_t DISPLAY_MAGIC = 0x54445844u; // "TDXD"
constexpr uint16_t DISPLAY_VERSION = 1u;
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
  FLAG_SCOPE_STEREO = 1u << 9,
};

enum ScopeFormat : uint32_t {
  SCOPE_FORMAT_MONO = 0u,
  SCOPE_FORMAT_STEREO = 1u,
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
  float sampleAbsolutePeakVolts = 0.f;

  uint32_t bufferCapacityFrames = 0;
  uint32_t bufferFilledFrames = 0;

  float scopeHalfWindowMs = 900.f;
  float scopeStartLagSamples = 0.f;
  float scopeBinSpanSamples = 1.f;
  uint32_t scopeBinCount = 0;

  ScopeBin scope[SCOPE_BIN_COUNT];
  ScopeBin scopeRight[SCOPE_BIN_COUNT];
};

static_assert(std::is_standard_layout<HostToDisplay>::value, "HostToDisplay must stay POD-like");

struct DisplayToHost {
  uint32_t magic = DISPLAY_MAGIC;
  uint16_t version = DISPLAY_VERSION;
  uint16_t size = uint16_t(sizeof(DisplayToHost));

  uint64_t requestSeq = 0;
  uint32_t requestedScopeFormat = SCOPE_FORMAT_MONO;
  uint32_t reserved = 0;
};

static_assert(std::is_standard_layout<DisplayToHost>::value, "DisplayToHost must stay POD-like");

inline bool isDisplayRequestValid(const DisplayToHost &msg) {
  return msg.magic == DISPLAY_MAGIC && msg.version == DISPLAY_VERSION && msg.size == sizeof(DisplayToHost);
}

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

  // Tiered max tracking for rolling peak efficiency
  static constexpr uint32_t BINS_PER_BLOCK = 64;
  static constexpr uint32_t BLOCK_COUNT = PREVIEW_BIN_COUNT / BINS_PER_BLOCK;
  std::array<int16_t, BLOCK_COUNT> blockMaxes;
  int16_t globalMaxQ = 0;

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
    blockMaxes.fill(0);
    globalMaxQ = 0;
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
    finalizeCurrentBin();
  }

  void finalizePartialBin() {
    if (samplesInCurrentBin == 0) {
      return;
    }
    finalizeCurrentBin();
  }

  void finalizeCurrentBin() {
    int oldMin = int(bins[writeIndex].min);
    int oldMax = int(bins[writeIndex].max);
    int16_t oldBinMax = int16_t(std::max(std::abs(oldMin), std::abs(oldMax)));

    bins[writeIndex].min = currentMin;
    bins[writeIndex].max = currentMax;
    int16_t newBinMax = int16_t(std::max(std::abs(int(currentMin)), std::abs(int(currentMax))));

    uint32_t blockIdx = writeIndex / BINS_PER_BLOCK;
    if (newBinMax >= blockMaxes[blockIdx]) {
      blockMaxes[blockIdx] = newBinMax;
    } else if (oldBinMax == blockMaxes[blockIdx]) {
      // Old bin was the max of this block, re-scan block.
      int16_t bm = 0;
      uint32_t start = blockIdx * BINS_PER_BLOCK;
      for (uint32_t i = 0; i < BINS_PER_BLOCK; ++i) {
        const auto &bin = bins[start + i];
        int16_t m = int16_t(std::max(std::abs(int(bin.min)), std::abs(int(bin.max))));
        if (m > bm) bm = m;
      }
      blockMaxes[blockIdx] = bm;
    }

    // Refresh global max from blockMaxes if it might have changed.
    if (blockMaxes[blockIdx] > globalMaxQ) {
      globalMaxQ = blockMaxes[blockIdx];
    } else if (oldBinMax == globalMaxQ) {
      int16_t gm = 0;
      for (int16_t bm : blockMaxes) {
        if (bm > gm) gm = bm;
      }
      globalMaxQ = gm;
    }

    writeIndex = (writeIndex + 1u) % PREVIEW_BIN_COUNT;
    filledBins = std::min(filledBins + 1u, PREVIEW_BIN_COUNT);
    samplesInCurrentBin = 0;
    currentMin = 0;
    currentMax = 0;
  }

  int16_t getAbsolutePeakQ() const {
    int q = int(globalMaxQ);
    // Include current partial bin
    if (samplesInCurrentBin > 0) {
      q = std::max(q, std::abs(int(currentMin)));
      q = std::max(q, std::abs(int(currentMax)));
    }
    return int16_t(std::min(q, 32767));
  }
};

inline void populateHostMessage(HostToDisplay *out, uint64_t publishSeq, uint64_t bufferGeneration, uint32_t flags,
                                float sampleRate, float lagSamples, float accessibleLagSamples, float platterAngle,
                                float samplePlayheadSec, float sampleDurationSec, float sampleProgress,
                                float sampleAbsolutePeakVolts,
                                uint32_t bufferCapacityFrames, uint32_t bufferFilledFrames,
                                float scopeHalfWindowMs, float scopeStartLagSamples, float scopeBinSpanSamples,
                                uint32_t scopeBinCount, const ScopeBin *scopeBins, const ScopeBin *scopeRightBins = nullptr) {
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
  out->sampleAbsolutePeakVolts = sampleAbsolutePeakVolts;
  out->bufferCapacityFrames = bufferCapacityFrames;
  out->bufferFilledFrames = bufferFilledFrames;
  out->scopeHalfWindowMs = scopeHalfWindowMs;
  out->scopeStartLagSamples = scopeStartLagSamples;
  out->scopeBinSpanSamples = scopeBinSpanSamples;
  out->scopeBinCount = std::min(scopeBinCount, SCOPE_BIN_COUNT);
  ScopeBin empty = makeEmptyScopeBin();
  for (size_t i = 0; i < SCOPE_BIN_COUNT; ++i) {
    out->scope[i] = empty;
    out->scopeRight[i] = empty;
  }
  if (scopeBins && out->scopeBinCount > 0) {
    std::memcpy(out->scope, scopeBins, sizeof(ScopeBin) * size_t(out->scopeBinCount));
  }
  if (scopeRightBins && out->scopeBinCount > 0) {
    std::memcpy(out->scopeRight, scopeRightBins, sizeof(ScopeBin) * size_t(out->scopeBinCount));
  }
}

inline void populateDisplayRequest(DisplayToHost *out, uint64_t requestSeq, uint32_t requestedScopeFormat) {
  if (!out) {
    return;
  }
  out->magic = DISPLAY_MAGIC;
  out->version = DISPLAY_VERSION;
  out->size = uint16_t(sizeof(DisplayToHost));
  out->requestSeq = requestSeq;
  out->requestedScopeFormat = (requestedScopeFormat == SCOPE_FORMAT_STEREO) ? SCOPE_FORMAT_STEREO : SCOPE_FORMAT_MONO;
  out->reserved = 0u;
}

} // namespace temporaldeck_expander
