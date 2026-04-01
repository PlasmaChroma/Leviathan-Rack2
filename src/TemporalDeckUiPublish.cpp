#include "TemporalDeckUiPublish.hpp"

#include "TemporalDeck.hpp"

#include <array>
#include <cmath>

namespace {

template <size_t N>
static void smoothLedBrightness(std::array<float, N> &values, float sideWeight = 0.22f) {
  sideWeight = clamp(sideWeight, 0.f, 0.49f);
  float centerWeight = 1.f - 2.f * sideWeight;
  std::array<float, N> src = values;
  for (size_t i = 0; i < N; ++i) {
    float prev = src[i > 0 ? i - 1 : i];
    float next = src[i + 1 < N ? i + 1 : i];
    values[i] = clamp(prev * sideWeight + src[i] * centerWeight + next * sideWeight, 0.f, 1.f);
  }
}

} // namespace

namespace temporaldeck_ui {

void publishArcLights(TemporalDeck *module, int sampleFrames, float maxLagSamples, bool sampleMode, bool sampleLoaded,
                      double lag, double accessibleLag, double sampleProgress) {
  std::array<float, TemporalDeck::kArcLightCount> arcYellow{};
  std::array<float, TemporalDeck::kArcLightCount> arcRed{};
  std::array<float, TemporalDeck::kArcLightCount> limitYellowBlendAllowed{};
  const float limitIndicatorRedBrightness = 0.9f;
  const float limitApproachWindowLeds = 2.0f;
  auto applyLimitMarker = [&](int i, float playheadLed, float limitLed, float *brightness, bool allowYellowBlend) {
    int limitLedIndex = clamp(int(std::round(limitLed)), 0, TemporalDeck::kArcLightCount - 1);
    bool isLimitLed = i == limitLedIndex;
    if (isLimitLed) {
      limitYellowBlendAllowed[i] = allowYellowBlend ? 1.f : 0.f;
      if (allowYellowBlend) {
        float approach = clamp(1.f - std::fabs(playheadLed - limitLed) / limitApproachWindowLeds, 0.f, 1.f);
        *brightness = std::max(*brightness, 0.42f * approach);
      }
    }
    arcRed[i] = isLimitLed ? limitIndicatorRedBrightness : 0.f;
  };

  if (sampleMode && sampleLoaded) {
    // Sample mode fills from left->right. The red limit marker indicates the
    // effective playback end (full sample end or knob-limited end).
    float leftLed = float(TemporalDeck::kArcLightCount - 1);
    float sampleNewest = std::max(1.f, float(sampleFrames - 1));
    float limitRatio = clamp(float(accessibleLag / sampleNewest), 0.f, 1.f);
    float sampleEndLed = (1.f - limitRatio) * leftLed;
    float progressNorm = clamp(float(sampleProgress), 0.f, 1.f);
    float ledSpan = std::max(0.f, leftLed - sampleEndLed);
    float progressLedUnits = progressNorm * ledSpan;
    float playheadLed = leftLed - progressLedUnits;
    for (int i = 0; i < TemporalDeck::kArcLightCount; ++i) {
      // Progressive fill from left to right with fractional leading LED.
      // At start, all LEDs are off; the leading LED ramps in proportionally.
      float ledFromStart = leftLed - float(i);
      float fill = clamp(progressLedUnits - ledFromStart, 0.f, 1.f);
      float brightness = 0.92f * fill;
      // Keep anything beyond the effective sample end dark.
      if (float(i) + 0.5f < sampleEndLed) {
        brightness = 0.f;
      }
      bool allowYellowBlend = brightness > 1e-4f;
      applyLimitMarker(i, playheadLed, sampleEndLed, &brightness, allowYellowBlend);
      arcYellow[i] = brightness;
    }
  } else {
    float lagRatio = clamp(float(lag) / maxLagSamples, 0.f, 1.f);
    float limitRatio = clamp(float(accessibleLag) / maxLagSamples, 0.f, 1.f);
    float lagLed = lagRatio * float(TemporalDeck::kArcLightCount - 1);
    float limitLed = limitRatio * float(TemporalDeck::kArcLightCount - 1);
    for (int i = 0; i < TemporalDeck::kArcLightCount; ++i) {
      float brightness = 0.f;
      if (i == 0) {
        brightness = clamp(lagLed, 0.f, 1.f);
      } else {
        brightness = clamp(lagLed - float(i) + 1.f, 0.f, 1.f);
      }
      if (float(i) > limitLed + 0.5f) {
        brightness = 0.f;
      }
      bool allowYellowBlend = brightness > 1e-4f;
      applyLimitMarker(i, lagLed, limitLed, &brightness, allowYellowBlend);
      arcYellow[i] = brightness;
    }
  }

  // Small spatial blend to remove visible dead-zones between LEDs while
  // preserving endpoint markers.
  smoothLedBrightness(arcYellow);
  for (int i = 0; i < TemporalDeck::kArcLightCount; ++i) {
    if (arcRed[i] > 0.f && limitYellowBlendAllowed[i] < 0.5f) {
      arcYellow[i] = 0.f;
    }
  }
  for (int i = 0; i < TemporalDeck::kArcLightCount; ++i) {
    module->lights[TemporalDeck::ARC_LIGHT_START + i].setBrightness(arcYellow[i]);
    module->lights[TemporalDeck::ARC_MAX_LIGHT_START + i].setBrightness(arcRed[i]);
  }
}

} // namespace temporaldeck_ui
