#pragma once
#include "DebugTerminalMetrics.hpp"

namespace chimera {

// One UI frame spans step(), the shadow pass, draw(), and the light pass.
// Commit only at the next step, after every pass has had a chance to contribute.
struct RenderFrameMetrics {
    bool enabled = false, drew = false;
    float drawUs = 0.f, cacheUs = 0.f, liveUs = 0.f, lightUs = 0.f, glStepUs = 0.f;
    unsigned cacheRenders = 0;

    void addDraw(float us) {
        drew = true;
        drawUs += us;
    }
    void addLayer(float us, int layer) {
        if (layer == 1) lightUs += us;
    }
    // Only normal draw() call durations contribute to drawUs.
    // Layer calls and GL work in step are reported separately.
};

struct RenderComponentTimer {
    RenderFrameMetrics* metrics;
    bool cached;
    std::chrono::steady_clock::time_point start;
    RenderComponentTimer(RenderFrameMetrics* metrics, bool cached)
        : metrics(metrics && metrics->enabled ? metrics : nullptr), cached(cached),
          start(debug_terminal::debugTimerStart(this->metrics != nullptr)) {}
    ~RenderComponentTimer() {
        if (!metrics) return;
        const float us = debug_terminal::elapsedUsSince(start);
        if (cached) { metrics->cacheUs += us; ++metrics->cacheRenders; }
        else metrics->liveUs += us;
    }
};

} // namespace chimera
