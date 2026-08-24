#pragma once

#include "DebugTerminalTransport.hpp"
#include "plugin.hpp"

#include <atomic>
#include <chrono>
#include <cstdio>
#include <limits>
#include <string>
#include <unordered_map>

namespace debug_terminal {

struct BaselineModuleMetrics {
  std::atomic<uint64_t> sampledCount {0};
  std::atomic<uint64_t> processNs {0};
  std::atomic<uint64_t> processMinNs {std::numeric_limits<uint64_t>::max()};
  std::atomic<uint64_t> processMaxNs {0};
  uint32_t instanceId = 0u;

  void assignInstanceId(std::atomic<uint32_t>& counter) {
    instanceId = counter.fetch_add(1u, std::memory_order_relaxed);
  }

  void recordProcess(uint64_t elapsedNs) {
    sampledCount.fetch_add(1u, std::memory_order_relaxed);
    processNs.fetch_add(elapsedNs, std::memory_order_relaxed);
    recordAudioProcessTiming(processMinNs, processMaxNs, elapsedNs);
  }

  TimingRangeUs consumeProcessRange() {
    sampledCount.exchange(0, std::memory_order_acq_rel);
    processNs.exchange(0, std::memory_order_acq_rel);
    return consumeAudioProcessTiming(processMinNs, processMaxNs);
  }
};

struct BaselineWidgetMetrics {
  float stepUsEma = 0.f;
  float drawUsEma = 0.f;
  UiTimingRangeAccumulator stepUsRange;
  UiTimingRangeAccumulator drawUsRange;

  void recordStep(float stepUs) {
    stepUsEma = (stepUsEma > 0.f) ? (stepUsEma + (stepUs - stepUsEma) * 0.18f) : stepUs;
    stepUsRange.add(stepUs);
  }

  void recordDraw(float drawUs) {
    drawUsEma = (drawUsEma > 0.f) ? (drawUsEma + (drawUs - drawUsEma) * 0.18f) : drawUs;
    drawUsRange.add(drawUs);
  }

  TimingRangeUs consumeStepRange() { return stepUsRange.consume(); }
  TimingRangeUs consumeDrawRange() { return drawUsRange.consume(); }
};

inline std::chrono::steady_clock::time_point debugTimerStart(bool enabled) {
  return enabled ? std::chrono::steady_clock::now() : std::chrono::steady_clock::time_point();
}

inline float elapsedUsSince(std::chrono::steady_clock::time_point startedAt) {
  return float(std::chrono::duration_cast<std::chrono::nanoseconds>(
    std::chrono::steady_clock::now() - startedAt).count()) * 0.001f;
}

inline uint64_t elapsedNsSince(std::chrono::steady_clock::time_point startedAt) {
  return uint64_t(std::chrono::duration_cast<std::chrono::nanoseconds>(
    std::chrono::steady_clock::now() - startedAt).count());
}

inline void drawDebugInstanceId(NVGcontext* vg, Vec widgetSize, uint64_t instanceId) {
  if (!vg || !APP || !APP->window || !APP->window->uiFont) return;
  char label[32];
  std::snprintf(label, sizeof(label), "ID:%llu",
    static_cast<unsigned long long>(instanceId));
  const float x = widgetSize.x - mm2px(0.9f);
  const float y = mm2px(2.5f);
  nvgSave(vg);
  nvgFontFaceId(vg, APP->window->uiFont->handle);
  nvgFontSize(vg, 6.8f);
  nvgTextAlign(vg, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
  nvgFillColor(vg, nvgRGBA(8, 10, 14, 210));
  nvgText(vg, x + 0.45f, y + 0.45f, label, nullptr);
  nvgFillColor(vg, nvgRGBA(255, 255, 255, 230));
  nvgText(vg, x, y, label, nullptr);
  nvgRestore(vg);
}

inline bool baselineSubmitDue(const char* moduleName, uint32_t instanceId, double nowSec) {
  static std::unordered_map<std::string, double> lastSubmitSecByKey;
  const std::string key = std::string(moduleName ? moduleName : "") + "|" + std::to_string(instanceId);
  double& lastSubmitSec = lastSubmitSecByKey[key];
  if (lastSubmitSec > 0.0 && (nowSec - lastSubmitSec) < kTimingRangeSubmitIntervalSec) {
    return false;
  }
  lastSubmitSec = nowSec;
  return true;
}

} // namespace debug_terminal
