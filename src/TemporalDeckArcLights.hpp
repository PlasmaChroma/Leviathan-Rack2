#pragma once

#include <array>

namespace temporaldeck_ui {

constexpr int kTemporalDeckArcLightCount = 31;

struct ArcLightState {
  std::array<float, kTemporalDeckArcLightCount> yellow{};
  std::array<float, kTemporalDeckArcLightCount> red{};
};

ArcLightState computeArcLightState(int sampleFrames, float maxLagSamples, bool sampleMode, bool sampleLoaded, double lag,
                                   double accessibleLag, double sampleProgress);

} // namespace temporaldeck_ui
