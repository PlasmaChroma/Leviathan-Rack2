#pragma once
#include "../plugin.hpp"
#include "AdaptiveVisualUpdate.hpp"

namespace leviathan {
namespace render {

// One sampler per Rack UI thread across every participating module.
inline VisualFramePressure& rackVisualFramePressure() {
	static thread_local VisualFramePressure pressure;
	if (APP && APP->window) {
		const double frame = APP->window->getFrameTime();
		if (pressure.lastFrame == frame) return pressure;
		double target = rack::settings::frameRateLimit;
		const double refresh = APP->window->getMonitorRefreshRate();
		if (refresh > 0.0) target = target > 0.0 ? std::min(target, refresh) : refresh;
		pressure.observe(frame, target > 0.0 ? target : 60.0);
	}
	return pressure;
}

} // namespace render
} // namespace leviathan
