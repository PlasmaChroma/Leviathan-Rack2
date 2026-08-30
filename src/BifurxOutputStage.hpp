#pragma once

#include "MathHelpers.hpp"

#include <algorithm>
#include <cmath>

namespace bifurx {

constexpr float kOutputSoftLimitKneeVolts = 4.f;
constexpr float kOutputSoftLimitCeilingVolts = 5.f;
constexpr float kOutputSoftLimitHeadroomVolts =
	kOutputSoftLimitCeilingVolts - kOutputSoftLimitKneeVolts;

// Eurorack audio is nominally +/-5 V. Keep ordinary signals exactly linear,
// then enter a unity-slope, symmetric knee that approaches that boundary.
// The disabled path is an exact finite pass-through.
inline float applyLevelOutputStage(
	float modeOut,
	float levelKnob,
	bool softLimitingEnabled = true
) {
	(void) levelKnob;
	const float out = std::isfinite(modeOut) ? modeOut : 0.f;
	if (!softLimitingEnabled) {
		return out;
	}

	const float magnitude = std::fabs(out);
	if (magnitude <= kOutputSoftLimitKneeVolts) {
		return out;
	}

	const float normalizedOver =
		(magnitude - kOutputSoftLimitKneeVolts) / kOutputSoftLimitHeadroomVolts;
	const float limitedMagnitude = std::min(
		kOutputSoftLimitCeilingVolts,
		kOutputSoftLimitKneeVolts
			+ kOutputSoftLimitHeadroomVolts * levi_math::tanhAudio(normalizedOver)
	);
	return std::copysign(limitedMagnitude, out);
}

} // namespace bifurx
