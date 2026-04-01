#pragma once

struct TemporalDeck;

namespace temporaldeck_ui {

void publishArcLights(TemporalDeck *module, int sampleFrames, float maxLagSamples, bool sampleMode, bool sampleLoaded,
                      double lag, double accessibleLag, double sampleProgress);

} // namespace temporaldeck_ui
