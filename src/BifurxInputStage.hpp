#pragma once

#include "MathHelpers.hpp"

#include <algorithm>

namespace bifurx {

inline float levelDriveGain(float knob) {
	const float x = levi_math::clamp01(knob);
	// Midpoint is exactly unity so the default LEVEL setting is neutral.
	return 0.075f + 0.95f * x + 3.6f * x * x * x;
}

inline float levelInputGain(float knob) {
	const float x = levi_math::clamp01(knob);
	if (x <= 0.5f) {
		return 2.f * x;
	}
	const float hot = 2.f * (x - 0.5f);
	return 1.f + 2.5f * hot * hot;
}

inline float levelDriveAmount(float knob) {
	const float x = levi_math::clamp01(knob);
	constexpr float kLevelDriveStart = 0.62f;
	if (x <= kLevelDriveStart) {
		return 0.f;
	}
	const float hot = levi_math::clamp01((x - kLevelDriveStart) / (1.f - kLevelDriveStart));
	return hot * hot;
}

inline float applyLevelInputStage(float input, float levelKnob) {
	constexpr float kLevelMaxDriveGain = 2.5f;
	const float clean = input * levelInputGain(levelKnob);
	const float driveAmount = levelDriveAmount(levelKnob);
	if (driveAmount <= 1e-5f) {
		return clean;
	}
	const float driveGain = 1.f + (kLevelMaxDriveGain - 1.f) * driveAmount;
	const float driven = 5.f * levi_math::tanhLegacy((clean * driveGain) / 5.f);
	return clean + (driven - clean) * driveAmount;
}

} // namespace bifurx
