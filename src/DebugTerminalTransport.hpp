#pragma once

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cmath>
#include <limits>
#include <string>

namespace debug_terminal {

static constexpr double kTimingRangeSubmitIntervalSec = 1.0;

struct TimingRangeUs {
  float min = 0.f;
  float max = 0.f;
  float average = std::numeric_limits<float>::quiet_NaN();

  TimingRangeUs() = default;
  TimingRangeUs(float minValue, float maxValue) : min(minValue), max(maxValue) {
  }
};

struct UiTimingRangeAccumulator {
  bool hasSamples = false;
  float minUs = 0.f;
  float maxUs = 0.f;
  double totalUs = 0.0;
  uint64_t samples = 0;

  void add(float valueUs) {
    valueUs = std::max(0.f, valueUs);
    if (!std::isfinite(valueUs)) return;
    totalUs += valueUs;
    ++samples;
    if (!hasSamples) {
      minUs = valueUs;
      maxUs = valueUs;
      hasSamples = true;
      return;
    }
    minUs = std::min(minUs, valueUs);
    maxUs = std::max(maxUs, valueUs);
  }

  TimingRangeUs consume() {
    TimingRangeUs range;
    if (hasSamples) {
      range.min = minUs;
      range.max = maxUs;
      range.average = float(totalUs / double(samples));
    }
    hasSamples = false;
    minUs = 0.f;
    maxUs = 0.f;
    totalUs = 0.0;
    samples = 0;
    return range;
  }
};

// One timing producer, one UI consumer. Publish cumulative totals atomically,
// so consuming a mean never splits a sample's duration from its sample count.
// The producer never waits; the consumer gives up after four bounded attempts.
struct AtomicTimingAverage {
  std::atomic<uint64_t> version {0}, total {0}, count {0};
  uint64_t writerVersion = 0, writerTotal = 0, writerCount = 0;
  uint64_t consumedTotal = 0, consumedCount = 0;
  void add(uint64_t ns) {
    version.store(++writerVersion);
    writerTotal += ns;
    total.store(writerTotal);
    count.store(++writerCount);
    version.store(++writerVersion);
  }
  float consume(uint64_t* sampleCount = nullptr) {
    if (sampleCount) *sampleCount = 0;
    for (int attempt = 0; attempt < 4; ++attempt) {
      const auto first = version.load();
      if (first & 1u) continue;
      const auto ns = total.load(), samples = count.load();
      if (first != version.load()) continue;
      const auto deltaNs = ns - consumedTotal, deltaCount = samples - consumedCount;
      consumedTotal = ns; consumedCount = samples;
      if (sampleCount) *sampleCount = deltaCount;
      if (deltaCount) return float(double(deltaNs) * .001 / double(deltaCount));
      break;
    }
    return std::numeric_limits<float>::quiet_NaN();
  }
};

inline void atomicMin(std::atomic<uint64_t>& target, uint64_t value) {
  uint64_t current = target.load(std::memory_order_relaxed);
  while (value < current && !target.compare_exchange_weak(current, value, std::memory_order_relaxed)) {
  }
}

inline void atomicMax(std::atomic<uint64_t>& target, uint64_t value) {
  uint64_t current = target.load(std::memory_order_relaxed);
  while (value > current && !target.compare_exchange_weak(current, value, std::memory_order_relaxed)) {
  }
}

inline void recordAudioProcessTiming(std::atomic<uint64_t>& minNs,
                                     std::atomic<uint64_t>& maxNs,
                                     uint64_t elapsedNs,
                                     AtomicTimingAverage* average = nullptr) {
  atomicMin(minNs, elapsedNs);
  atomicMax(maxNs, elapsedNs);
  if (average) average->add(elapsedNs);
}

inline TimingRangeUs consumeAudioProcessTiming(std::atomic<uint64_t>& minNs,
                                               std::atomic<uint64_t>& maxNs,
                                               AtomicTimingAverage* average = nullptr) {
  const uint64_t minValue = minNs.exchange(std::numeric_limits<uint64_t>::max(), std::memory_order_acq_rel);
  const uint64_t maxValue = maxNs.exchange(0u, std::memory_order_acq_rel);
  TimingRangeUs result;
  if (minValue != std::numeric_limits<uint64_t>::max()) {
    result.min = float(double(minValue) * .001);
    result.max = float(double(maxValue) * .001);
  }
  if (average) result.average = average->consume();
  return result;
}

void submitTDScopeUiMetrics(uint32_t instanceId,
                            TimingRangeUs processUs,
                            TimingRangeUs stepUs,
                            TimingRangeUs drawUs,
                            int rows,
                            float densityPct,
                            float zoom,
                            float thickness,
                            uint64_t publishSeq,
                            uint64_t drawSeq,
                            uint64_t drawCalls);

void submitTemporalDeckUiMetrics(uint32_t instanceId,
                                 TimingRangeUs processUs,
                                 TimingRangeUs stepUs,
                                 TimingRangeUs drawUs,
                                 float scopePreviewUs,
                                 int scopeStride,
                                 bool scopeMetricValid);

void submitBifurxUiMetrics(uint32_t instanceId,
                           TimingRangeUs processUs,
                           TimingRangeUs stepUs,
                           TimingRangeUs drawUs,
                           bool renderOpengl,
                           float curvePrepUs,
                           float overlayPrepUs,
                           float surfaceRenderUs,
                           float workerSubmitUs,
                           float conduitDrawUs);

void submitWyrmMetrics(uint32_t instanceId,
                       TimingRangeUs processUs,
                       TimingRangeUs stepUs,
                       TimingRangeUs drawUs,
                       TimingRangeUs editorStepUs,
                       TimingRangeUs cachedEditorUs,
                       TimingRangeUs overlayUs,
                       float editorDrawUs,
                       int channels,
                       int bodySamples,
                       uint64_t bodySampleCacheHits,
                       uint64_t bodySampleCacheMisses,
                       bool fixedSurface,
                       int fixedSurfaceWidth,
                       int fixedSurfaceHeight,
                       uint64_t fixedSurfaceGeneration);

void submitIntegralFluxMetrics(uint32_t instanceId,
                               TimingRangeUs processUs,
                               TimingRangeUs stepUs,
                               TimingRangeUs drawUs,
                               TimingRangeUs apertureUs,
                               float gearUs,
                               float eclipseUs);

void submitProcMetrics(uint32_t instanceId,
                       TimingRangeUs processUs,
                       TimingRangeUs stepUs,
                       TimingRangeUs drawUs);

void submitUndertowMetrics(uint32_t instanceId,
                           TimingRangeUs processUs,
                           TimingRangeUs stepUs,
                           TimingRangeUs drawUs);

void submitIrisMetrics(uint32_t instanceId,
                       TimingRangeUs processUs,
                       TimingRangeUs stepUs,
                       TimingRangeUs drawUs);

void submitDoorstopMetrics(uint32_t instanceId,
                           TimingRangeUs processUs,
                           TimingRangeUs stepUs,
                           TimingRangeUs drawUs,
                           TimingRangeUs geometryIdleUs,
                           TimingRangeUs geometryTrailUs,
                           TimingRangeUs panelIdleUs,
                           TimingRangeUs panelTrailUs,
                           TimingRangeUs overflowIdleUs,
                           TimingRangeUs overflowTrailUs,
                           bool trailsActive);

void submitCrownstepAiMetrics(uint32_t instanceId, int aiThinkMs);

void submitBaselineMetrics(const char* moduleName,
                           uint32_t instanceId,
                           TimingRangeUs processUs,
                           TimingRangeUs stepUs,
                           TimingRangeUs drawUs);

void submitSibylMetrics(uint32_t instanceId,
                        int64_t rackModuleId,
                        TimingRangeUs processUs,
                        float processMeanUs,
                        uint64_t processSamples,
                        TimingRangeUs stepUs,
                        TimingRangeUs drawUs,
                        TimingRangeUs snapshotUs,
                        TimingRangeUs oracleUs,
                        int nvgPathOps);

// Read-only bridge for Octavia. This intentionally exposes only the latest
// compact terminal metric row; detailed profiling belongs in a separate,
// explicitly armed capture path.
std::string latestMetricsJson(int64_t rackModuleId);

} // namespace debug_terminal
