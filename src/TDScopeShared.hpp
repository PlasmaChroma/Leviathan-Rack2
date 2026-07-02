#pragma once

#include "TemporalDeckExpanderProtocol.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstddef>
#include <utility>
#include <vector>

namespace tdscope {

struct ScopeWindowLagSpan {
  float halfWindowSamples = 0.f;
  float totalWindowSamples = 1.f;
  float forwardWindowSamples = 0.f;
  float backwardWindowSamples = 1.f;
  float windowTopLag = 0.f;
  float windowBottomLag = 0.f;
};

struct ScopeLaneGeometry {
  float laneGap = 0.f;
  float laneWidth = 1.f;
  float lane0CenterX = 0.f;
  float lane1CenterX = 0.f;
  float laneAmpHalfWidth = 0.46f;
};

inline ScopeLaneGeometry computeScopeLaneGeometry(float widgetWidth, bool renderStereo) {
  ScopeLaneGeometry g;
  g.laneGap = renderStereo ? 2.f : 0.f;
  g.laneWidth =
    renderStereo ? std::max((widgetWidth - g.laneGap) * 0.5f, 1.f) : std::max(widgetWidth, 1.f);
  g.lane0CenterX = renderStereo ? (g.laneWidth * 0.5f) : (widgetWidth * 0.5f);
  g.lane1CenterX = renderStereo ? (g.laneWidth + g.laneGap + g.laneWidth * 0.5f) : g.lane0CenterX;
  g.laneAmpHalfWidth = g.laneWidth * 0.46f;
  return g;
}

inline ScopeWindowLagSpan computeScopeWindowLagSpan(const temporaldeck_expander::HostToDisplay& msg) {
  ScopeWindowLagSpan span;
  const float sampleRate = std::max(msg.sampleRate, 1.f);
  span.halfWindowSamples = std::max(0.f, msg.scopeHalfWindowMs * 0.001f * sampleRate);
  span.totalWindowSamples = std::max(1.f, 2.f * span.halfWindowSamples);
  const bool sampleMode = (msg.flags & temporaldeck_expander::FLAG_SAMPLE_MODE) != 0u;
  span.forwardWindowSamples = span.halfWindowSamples;
  span.backwardWindowSamples = span.halfWindowSamples;
  if (!sampleMode) {
    span.forwardWindowSamples = std::min(span.halfWindowSamples, std::max(msg.lagSamples, 0.f));
    span.backwardWindowSamples = span.totalWindowSamples - span.forwardWindowSamples;
  }
  span.windowTopLag = msg.lagSamples + span.backwardWindowSamples;
  span.windowBottomLag = msg.lagSamples - span.forwardWindowSamples;
  if (msg.scopeBinCount > 0u && std::isfinite(msg.scopeStartLagSamples) && std::isfinite(msg.scopeBinSpanSamples) &&
      msg.scopeBinSpanSamples > 0.f) {
    span.windowTopLag =
      std::isfinite(msg.scopeVisibleStartLagSamples) ? msg.scopeVisibleStartLagSamples : msg.scopeStartLagSamples;
    span.windowBottomLag = span.windowTopLag - span.totalWindowSamples;
    if (!sampleMode) {
      span.backwardWindowSamples = span.windowTopLag - msg.lagSamples;
      span.forwardWindowSamples = msg.lagSamples - span.windowBottomLag;
    }
  }
  return span;
}

inline float computeReadHeadMarkerT(const ScopeWindowLagSpan& span, float markerLagSamples) {
  if (!std::isfinite(markerLagSamples) || !std::isfinite(span.windowTopLag) || !std::isfinite(span.windowBottomLag) ||
      span.windowTopLag == span.windowBottomLag) {
    return 0.5f;
  }
  return clamp((markerLagSamples - span.windowTopLag) / (span.windowBottomLag - span.windowTopLag), 0.f, 1.f);
}

inline float applyLiveReadHeadPolicy(float markerT, float markerLagSamples, float halfWindowSamples, bool sampleMode,
                                     bool verticalInverted) {
  float t = clamp(markerT, 0.f, 1.f);
  if (sampleMode) {
    // Sample mode policy: marker is always centered.
    t = 0.5f;
  } else {
    // Live scope policy:
    // - if lag reaches/exceeds half-window (900ms default), pin marker to center
    // - otherwise keep marker in the NOW-side half.
    if (std::isfinite(markerLagSamples) && std::isfinite(halfWindowSamples) && markerLagSamples >= halfWindowSamples) {
      t = 0.5f;
    } else {
      t = std::max(t, 0.5f);
    }
  }
  if (verticalInverted) {
    t = 1.f - t;
  }
  return t;
}

struct ScopeAutoScaleState {
  float displayFullScaleVolts = 5.f;
  bool initialized = false;
  bool lastSampleMode = true;
  float livePeakHoldVolts = 0.f;
  int livePeakHoldFrames = 0;
};

inline float solveLagDragPlaybackLag(float desiredPlaybackLag,
                                     bool sampleMode,
                                     bool sampleLoaded,
                                     bool sampleLoop,
                                     bool freezeActive,
                                     float accessibleLagSamples,
                                     double lastGoodMsgTimeSec,
                                     double nowSec,
                                     float sampleRate) {
  if (sampleMode && sampleLoaded && sampleLoop && accessibleLagSamples > 0.f) {
    double wrappedLag = std::fmod(double(desiredPlaybackLag), double(accessibleLagSamples) + 1.0);
    if (wrappedLag < 0.0) {
      wrappedLag += double(accessibleLagSamples) + 1.0;
    }
    return float(wrappedLag);
  }

  float effectiveAccessibleLag = accessibleLagSamples;
  if (!sampleMode && !freezeActive && lastGoodMsgTimeSec >= 0.0) {
    const double msgAgeSec = std::max(0.0, nowSec - lastGoodMsgTimeSec);
    effectiveAccessibleLag += std::max(sampleRate, 1.f) * float(msgAgeSec);
  }
  return clamp(desiredPlaybackLag, 0.f, std::max(0.f, effectiveAccessibleLag));
}

inline float computeLagDragVelocity(float previousLagSamples, float currentLagSamples, double dtSec, float sampleRate) {
  const float dt = std::max(float(dtSec), 1e-6f);
  float velocitySamples = (previousLagSamples - currentLagSamples) / dt;
  const float maxAbsGestureVelocity = std::max(sampleRate, 1.f) * 3.0f;
  velocitySamples = clamp(velocitySamples, -maxAbsGestureVelocity, maxAbsGestureVelocity);
  return velocitySamples;
}

inline std::pair<float, float> computeScopePeakStatsFromBins(const temporaldeck_expander::ScopeBin* leftScopeBins,
                                                             const temporaldeck_expander::ScopeBin* rightScopeBins,
                                                             uint32_t scopeBinCount,
                                                             bool renderStereo) {
  std::vector<float> peaks;
  peaks.reserve(renderStereo ? scopeBinCount * 2u : scopeBinCount);
  for (uint32_t i = 0; i < scopeBinCount; ++i) {
    const temporaldeck_expander::ScopeBin& bin = leftScopeBins[i];
    if (temporaldeck_expander::isScopeBinValid(bin)) {
      const int peakQ = std::max(std::abs(int(bin.min)), std::abs(int(bin.max)));
      const float peakV = (float(peakQ) / 32767.f) * temporaldeck_expander::kPreviewQuantizeVolts;
      peaks.push_back(clamp(peakV, 0.f, temporaldeck_expander::kPreviewQuantizeVolts));
    }
    if (renderStereo) {
      const temporaldeck_expander::ScopeBin& binR = rightScopeBins[i];
      if (temporaldeck_expander::isScopeBinValid(binR)) {
        const int peakQR = std::max(std::abs(int(binR.min)), std::abs(int(binR.max)));
        const float peakVR = (float(peakQR) / 32767.f) * temporaldeck_expander::kPreviewQuantizeVolts;
        peaks.push_back(clamp(peakVR, 0.f, temporaldeck_expander::kPreviewQuantizeVolts));
      }
    }
  }
  if (peaks.empty()) {
    return std::make_pair(0.f, 0.f);
  }
  const float windowPeakVolts = *std::max_element(peaks.begin(), peaks.end());
  const size_t n = peaks.size();
  size_t rank = size_t(std::ceil(0.99f * float(n)));
  rank = std::max<size_t>(1, std::min(rank, n));
  const size_t p99Index = rank - 1;
  std::nth_element(peaks.begin(), peaks.begin() + ptrdiff_t(p99Index), peaks.end());
  return std::make_pair(windowPeakVolts, peaks[p99Index]);
}

inline float updateScopeAutoScale(ScopeAutoScaleState* state,
                                  bool autoRangeEnabled,
                                  bool stateTickNeeded,
                                  bool dragActive,
                                  bool sampleMode,
                                  float windowPeakVolts,
                                  float p99Volts,
                                  float fallbackDisplayFullScaleVolts) {
  if (!state) {
    return std::max(fallbackDisplayFullScaleVolts, 0.001f);
  }
  if (!autoRangeEnabled) {
    state->initialized = false;
    state->lastSampleMode = true;
    state->livePeakHoldVolts = 0.f;
    state->livePeakHoldFrames = 0;
    return std::max(fallbackDisplayFullScaleVolts, 0.001f);
  }

  if (stateTickNeeded || !state->initialized) {
    const bool modeChanged = !state->initialized || sampleMode != state->lastSampleMode;
    if (p99Volts <= 0.f) {
      p99Volts = windowPeakVolts;
    }
    const float truePeakVolts = windowPeakVolts;
    const float stablePeakVolts = std::max(p99Volts, truePeakVolts * 0.72f);

    if (modeChanged || !std::isfinite(state->livePeakHoldVolts)) {
      state->livePeakHoldVolts = stablePeakVolts;
      state->livePeakHoldFrames = 0;
    } else if (stablePeakVolts > state->livePeakHoldVolts) {
      state->livePeakHoldVolts += (stablePeakVolts - state->livePeakHoldVolts) * 0.55f;
      state->livePeakHoldFrames = 14; // ~230ms @ 60Hz
    } else if (state->livePeakHoldFrames > 0) {
      state->livePeakHoldFrames--;
    } else {
      state->livePeakHoldVolts += (stablePeakVolts - state->livePeakHoldVolts) * 0.006f;
    }

    float targetFullScaleVolts = std::max(p99Volts * 1.08f, state->livePeakHoldVolts * 1.015f);
    targetFullScaleVolts = clamp(targetFullScaleVolts, 0.25f, temporaldeck_expander::kPreviewQuantizeVolts);

    if (!modeChanged) {
      const float hysteresisFrac = 0.03f;
      const float lowBand = state->displayFullScaleVolts * (1.f - hysteresisFrac);
      const float highBand = state->displayFullScaleVolts * (1.f + hysteresisFrac);
      if (targetFullScaleVolts >= lowBand && targetFullScaleVolts <= highBand) {
        targetFullScaleVolts = state->displayFullScaleVolts;
      }
    }
    if (!state->initialized) {
      state->displayFullScaleVolts = targetFullScaleVolts;
      state->initialized = true;
    } else {
      const float delta = targetFullScaleVolts - state->displayFullScaleVolts;
      if (std::fabs(delta) > 0.01f) {
        const float kAutoScaleAttackAlpha = 0.045f;
        const float kAutoScaleReleaseAlpha = 0.0045f;
        const float kAutoScaleAttackAlphaDrag = 0.020f;
        const float kAutoScaleReleaseAlphaDrag = 0.0020f;
        const float alpha = delta > 0.f
                              ? (dragActive ? kAutoScaleAttackAlphaDrag : kAutoScaleAttackAlpha)
                              : (dragActive ? kAutoScaleReleaseAlphaDrag : kAutoScaleReleaseAlpha);
        state->displayFullScaleVolts += delta * alpha;
      }
    }
    state->lastSampleMode = sampleMode;
  }

  return std::max(state->displayFullScaleVolts, 0.001f);
}

} // namespace tdscope
