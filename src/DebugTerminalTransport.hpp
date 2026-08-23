#pragma once

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <limits>

namespace debug_terminal {

static constexpr double kTimingRangeSubmitIntervalSec = 1.0;

struct TimingRangeUs {
  float min = 0.f;
  float max = 0.f;

  TimingRangeUs() = default;
  TimingRangeUs(float minValue, float maxValue) : min(minValue), max(maxValue) {
  }
};

struct UiTimingRangeAccumulator {
  bool hasSamples = false;
  float minUs = 0.f;
  float maxUs = 0.f;

  void add(float valueUs) {
    valueUs = std::max(0.f, valueUs);
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
    }
    hasSamples = false;
    minUs = 0.f;
    maxUs = 0.f;
    return range;
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
                                     uint64_t elapsedNs) {
  atomicMin(minNs, elapsedNs);
  atomicMax(maxNs, elapsedNs);
}

inline TimingRangeUs consumeAudioProcessTiming(std::atomic<uint64_t>& minNs,
                                               std::atomic<uint64_t>& maxNs) {
  const uint64_t minValue = minNs.exchange(std::numeric_limits<uint64_t>::max(), std::memory_order_acq_rel);
  const uint64_t maxValue = maxNs.exchange(0u, std::memory_order_acq_rel);
  if (minValue == std::numeric_limits<uint64_t>::max() || maxValue == 0u) {
    return {};
  }
  return TimingRangeUs(float(double(minValue) * 0.001), float(double(maxValue) * 0.001));
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
                           float uiLocalPrepUs,
                           bool renderOpengl,
                           float curvePrepUs,
                           float overlayPrepUs,
                           int visualWorkerMode,
                           float visualWorkerAgeMs,
                           float visualWorkerQueueMs);

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
                       uint64_t bodySampleCacheMisses);

void submitGlZoomMetrics(const char* moduleName,
                         uint32_t instanceId,
                         uint32_t contextGeneration,
                         uint32_t dirtyMask,
                         float absoluteZoom,
                         int framebufferWidth,
                         int framebufferHeight,
                         float framebufferDrawUs,
                         uint64_t framebufferDrawCount,
                         float shaderCompileUs,
                         float shaderLinkUs,
                         int shaderVariant);

void submitIntegralFluxMetrics(uint32_t instanceId,
                               TimingRangeUs processUs,
                               TimingRangeUs stepUs,
                               TimingRangeUs drawUs,
                               TimingRangeUs apertureUs,
                               float gearUs,
                               float eclipseUs,
                               float linearPointUs,
                               float shapeGlyphUs,
                               float ch1CurvePointsReducedAvg,
                               float ch1TracerExtraPointsReducedAvg);

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

} // namespace debug_terminal
