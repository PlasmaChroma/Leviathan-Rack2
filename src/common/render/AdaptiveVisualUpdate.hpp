#pragma once
#include <algorithm>
#include <cmath>

namespace leviathan {
namespace render {

// UI-thread only. Feed one timestamp per host frame, shared by all consumers.
struct VisualFramePressure {
	double lastFrame = -1.0, target = 0.0, average = 0.0;
	double overloaded = 0.0, healthy = 0.0;
	int level = 0;

	void observe(double frame, double targetHz) {
		if (!std::isfinite(frame) || !std::isfinite(targetHz) || targetHz <= 0.0) return;
		if (frame == lastFrame) return;
		const double dt = frame - lastFrame;
		if (lastFrame < 0.0 || dt <= 0.0 || dt > 0.25 || target != targetHz) {
			average = 1.0 / targetHz;
			overloaded = healthy = 0.0;
			level = 0;
			lastFrame = frame;
			target = targetHz;
			return;
		}
		lastFrame = frame;
		average += std::min(1.0, dt / 0.5) * (dt - average);
		if (average > 1.15 / target) {
			healthy = 0.0;
			overloaded += dt;
			if (overloaded >= 0.5) { level = std::min(3, level + 1); overloaded = 0.0; }
		}
		else if (average < 1.05 / target) {
			overloaded = 0.0;
			healthy += dt;
			if (healthy >= 2.0) { level = std::max(0, level - 1); healthy = 0.0; }
		}
		else { overloaded = healthy = 0.0; }
	}
};

struct VisualUpdatePolicy {
	double preferredHz = 60.0;
	double minimumHz = 10.0;
	double interval(int pressure) const {
		return 1.0 / std::max(minimumHz, preferredHz / double(1 << pressure));
	}
};

// Time slots distribute consumers across frames without accumulating catch-up work.
struct VisualUpdateGate {
	double lastSlot = -1.0;
	double phase = 0.0;
	bool due(double now, double interval) {
		const double slot = std::floor(now / interval + phase);
		if (slot == lastSlot) return false;
		lastSlot = slot;
		return true;
	}
};

} // namespace render
} // namespace leviathan
