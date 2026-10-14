#include "TemporalDeckArcLights.hpp"

#include <algorithm>
#include <cmath>

namespace {

template <typename T>
T clampv(T v, T lo, T hi) {
  return std::max(lo, std::min(v, hi));
}

template <size_t N>
void smoothLedBrightness(std::array<float, N> &values, float sideWeight = 0.22f) {
  sideWeight = clampv(sideWeight, 0.f, 0.49f);
  float centerWeight = 1.f - 2.f * sideWeight;
  std::array<float, N> src = values;
  for (size_t i = 0; i < N; ++i) {
    float prev = src[i > 0 ? i - 1 : i];
    float next = src[i + 1 < N ? i + 1 : i];
    values[i] = clampv(prev * sideWeight + src[i] * centerWeight + next * sideWeight, 0.f, 1.f);
  }
}

} // namespace

namespace temporaldeck_ui {

ArcLightState computeArcLightState(int sampleFrames, float maxLagSamples, bool sampleMode, bool sampleLoaded, double lag,
                                   double accessibleLag, double sampleProgress) {
  ArcLightState state;
  std::array<float, kTemporalDeckArcLightCount> limitYellowBlendAllowed{};
  const float limitIndicatorRedBrightness = 0.9f;
  const float limitApproachWindowLeds = 2.0f;
  auto applyLimitMarker = [&](int i, float playheadLed, float limitLed, float *brightness, bool allowYellowBlend) {
    int limitLedIndex = clampv(int(std::round(limitLed)), 0, kTemporalDeckArcLightCount - 1);
    bool isLimitLed = i == limitLedIndex;
    if (isLimitLed) {
      limitYellowBlendAllowed[i] = allowYellowBlend ? 1.f : 0.f;
      if (allowYellowBlend) {
        float approach = clampv(1.f - std::fabs(playheadLed - limitLed) / limitApproachWindowLeds, 0.f, 1.f);
        *brightness = std::max(*brightness, 0.42f * approach);
      }
    }
    state.red[i] = isLimitLed ? limitIndicatorRedBrightness : 0.f;
  };

  if (sampleMode && sampleLoaded) {
    float leftLed = float(kTemporalDeckArcLightCount - 1);
    float sampleNewest = std::max(1.f, float(sampleFrames - 1));
    float limitRatio = clampv(float(accessibleLag / sampleNewest), 0.f, 1.f);
    float sampleEndLed = (1.f - limitRatio) * leftLed;
    float progressNorm = clampv(float(sampleProgress), 0.f, 1.f);
    float ledSpan = std::max(0.f, leftLed - sampleEndLed);
    float progressLedUnits = progressNorm * ledSpan;
    float playheadLed = leftLed - progressLedUnits;
    for (int i = 0; i < kTemporalDeckArcLightCount; ++i) {
      float ledFromStart = leftLed - float(i);
      float fill = clampv(progressLedUnits - ledFromStart, 0.f, 1.f);
      float brightness = 0.92f * fill;
      if (float(i) + 0.5f < sampleEndLed) {
        brightness = 0.f;
      }
      bool allowYellowBlend = brightness > 1e-4f;
      applyLimitMarker(i, playheadLed, sampleEndLed, &brightness, allowYellowBlend);
      state.yellow[i] = brightness;
    }
  } else {
    float safeMaxLag = std::max(maxLagSamples, 1e-6f);
    float lagRatio = clampv(float(lag) / safeMaxLag, 0.f, 1.f);
    float limitRatio = clampv(float(accessibleLag) / safeMaxLag, 0.f, 1.f);
    float lagLed = lagRatio * float(kTemporalDeckArcLightCount - 1);
    float limitLed = limitRatio * float(kTemporalDeckArcLightCount - 1);
    for (int i = 0; i < kTemporalDeckArcLightCount; ++i) {
      float brightness = 0.f;
      if (i == 0) {
        brightness = clampv(lagLed, 0.f, 1.f);
      } else {
        brightness = clampv(lagLed - float(i) + 1.f, 0.f, 1.f);
      }
      if (float(i) > limitLed + 0.5f) {
        brightness = 0.f;
      }
      bool allowYellowBlend = brightness > 1e-4f;
      applyLimitMarker(i, lagLed, limitLed, &brightness, allowYellowBlend);
      state.yellow[i] = brightness;
    }
  }

  smoothLedBrightness(state.yellow);
  for (int i = 0; i < kTemporalDeckArcLightCount; ++i) {
    if (state.red[i] > 0.f && limitYellowBlendAllowed[i] < 0.5f) {
      state.yellow[i] = 0.f;
    }
  }
  return state;
}

} // namespace temporaldeck_ui
