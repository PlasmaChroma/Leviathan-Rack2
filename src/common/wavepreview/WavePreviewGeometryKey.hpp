#pragma once

#include <algorithm>
#include <cmath>

namespace wave_preview {

// A normalized one-cycle contour is independent of its absolute period.
// Compare against the last accepted key so small changes accumulate.
struct GeometryKey {
	float riseFraction = 0.f;
	float shape = 0.f;
	float width = 0.f;
	float height = 0.f;
	bool valid = false;

	bool accept(float rise, float fall, float curve, float w, float h) {
		const float fraction = rise / std::max(rise + fall, 1e-6f);
		w = std::max(w, 1.f);
		h = std::max(h, 1.f);
		// Only absorb floating-point noise in normalized timing. Shape changes
		// retain the existing exact LUT invalidation rule.
		if (valid && std::fabs(fraction - riseFraction) <= 1e-6f
			&& curve == shape && w == width && h == height) return false;
		riseFraction = fraction;
		shape = curve;
		width = w;
		height = h;
		valid = true;
		return true;
	}
};

} // namespace wave_preview
