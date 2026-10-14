#pragma once
#include <cstdint>

namespace visual_assets {
struct HaloKnob2DrawMetrics {
	uint64_t stepSurfaceNs = 0u;
	uint64_t glSurfaceFramebufferNs = 0u;
	uint64_t nanoVgSurfaceDrawNs = 0u;
	uint64_t centerFramebufferNs = 0u;
	uint64_t capReflectionFramebufferNs = 0u;
	uint32_t glSurfaceFramebufferDraws = 0u;
	uint32_t nanoVgSurfaceDraws = 0u;
	uint32_t centerFramebufferDraws = 0u;
	uint32_t capReflectionFramebufferDraws = 0u;
	void add(const HaloKnob2DrawMetrics& other) {
		stepSurfaceNs += other.stepSurfaceNs;
		glSurfaceFramebufferNs += other.glSurfaceFramebufferNs;
		nanoVgSurfaceDrawNs += other.nanoVgSurfaceDrawNs;
		centerFramebufferNs += other.centerFramebufferNs;
		capReflectionFramebufferNs += other.capReflectionFramebufferNs;
		glSurfaceFramebufferDraws += other.glSurfaceFramebufferDraws;
		nanoVgSurfaceDraws += other.nanoVgSurfaceDraws;
		centerFramebufferDraws += other.centerFramebufferDraws;
		capReflectionFramebufferDraws += other.capReflectionFramebufferDraws;
	}
};

// Scoped sinks attribute nested component work to the module currently being
// stepped/drawn. Idle modules cannot reset another owner's pending samples.
inline HaloKnob2DrawMetrics*& haloMetricsSink() {
    static thread_local HaloKnob2DrawMetrics fallback;
    static thread_local HaloKnob2DrawMetrics* sink = &fallback;
    return sink;
}
inline HaloKnob2DrawMetrics& haloKnob2DrawMetrics() { return *haloMetricsSink(); }
class ScopedHaloKnob2Metrics {
    HaloKnob2DrawMetrics* previous;
public:
    explicit ScopedHaloKnob2Metrics(HaloKnob2DrawMetrics& destination) : previous(haloMetricsSink()) {
        haloMetricsSink() = &destination;
    }
    ~ScopedHaloKnob2Metrics() { haloMetricsSink() = previous; }
    ScopedHaloKnob2Metrics(const ScopedHaloKnob2Metrics&) = delete;
    ScopedHaloKnob2Metrics& operator=(const ScopedHaloKnob2Metrics&) = delete;
};
}
