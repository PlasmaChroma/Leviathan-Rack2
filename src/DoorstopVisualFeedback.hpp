#pragma once

#include <algorithm>
#include <cmath>

namespace doorstop {

inline float shapeVisualEnergyActivity(float activity, bool expandLowEnergy) {
	if (!std::isfinite(activity)) return 0.f;
	activity = std::max(0.f, std::min(activity, 1.f));
	if (!expandLowEnergy) return activity;
	// V1/V2/Legacy publish much smaller physical-energy values. This is the
	// historical meter curve, evaluated only when a strike charges the bar.
	constexpr float DISPLAY_CURVE = 63.f;
	return std::log1p(DISPLAY_CURVE * activity) / std::log1p(DISPLAY_CURVE);
}

inline float updateVisualEnergyEnvelope(
		float current, float trackedActivity, bool triggered,
		bool followTrackedActivity, bool sleeping, float sampleTime) {
	if (sleeping) return 0.f;
	if (!std::isfinite(current)) current = 0.f;
	if (!std::isfinite(trackedActivity)) trackedActivity = 0.f;
	current = std::max(0.f, std::min(current, 1.f));
	trackedActivity = std::max(0.f, std::min(trackedActivity, 1.f));
	if (triggered) {
		return std::max(current, trackedActivity);
	}
	// Decay naturally, but converge more quickly when the spring's visual
	// activity has already fallen away. Both paths are strictly non-increasing,
	// so resonator fluctuations can never recharge the bar without a trigger.
	constexpr float DECAY_SECONDS = 1.f;
	constexpr float ACTIVITY_FOLLOW_SECONDS = 0.12f;
	const float safeSampleTime = std::isfinite(sampleTime)
		? std::max(0.f, sampleTime) : 0.f;
	const float decay = std::max(0.f, 1.f - safeSampleTime / DECAY_SECONDS);
	const float naturallyDecayed = current * decay;
	const float follow = std::min(1.f, safeSampleTime / ACTIVITY_FOLLOW_SECONDS);
	if (!followTrackedActivity) return naturallyDecayed;
	const float activityCeiling = std::min(current, trackedActivity);
	const float activityTracked = current
		+ (activityCeiling - current) * follow;
	return std::min(naturallyDecayed, activityTracked);
}

} // namespace doorstop
