#pragma once

#include <array>
#include <cmath>

namespace aperture_light {

struct Transfer {
	float glow = 0.f;
	float core = 0.f;
	float hot = 0.f;
};

inline Transfer transferFromBrightness(float brightness) {
	static constexpr int LUT_INTERVALS = 1024;
	struct Tables {
		std::array<float, LUT_INTERVALS + 1> glow {};
		std::array<float, LUT_INTERVALS + 1> core {};

		Tables() {
			for (int i = 0; i <= LUT_INTERVALS; ++i) {
				const float t = float(i) / float(LUT_INTERVALS);
				glow[size_t(i)] = std::pow(t, 0.55f);
				core[size_t(i)] = std::pow(t, 0.85f);
			}
		}
	};
	static const Tables tables;

	const float t = brightness <= 0.f ? 0.f : (brightness >= 1.f ? 1.f : brightness);
	const float index = t * float(LUT_INTERVALS);
	const int i0 = int(index);
	const int i1 = i0 < LUT_INTERVALS ? i0 + 1 : LUT_INTERVALS;
	const float fraction = index - float(i0);

	Transfer result;
	result.glow = tables.glow[size_t(i0)]
		+ (tables.glow[size_t(i1)] - tables.glow[size_t(i0)]) * fraction;
	result.core = tables.core[size_t(i0)]
		+ (tables.core[size_t(i1)] - tables.core[size_t(i0)]) * fraction;
	result.hot = t * t * t;
	return result;
}

} // namespace aperture_light
