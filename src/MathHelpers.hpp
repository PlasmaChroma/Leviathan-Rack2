#pragma once

#include "plugin.hpp"

#include <cmath>

namespace levi_math {

inline float clamp01(float x) {
	return clamp(x, 0.f, 1.f);
}

inline float fastTanh(float x) {
	const float x2 = x * x;
	if (x2 < 9.f) {
		return x * (27.f + x2) / (27.f + 9.f * x2);
	}
	return (x > 0.f) ? 1.f : -1.f;
}

inline float softClip(float x) {
	return fastTanh(x);
}

inline float softLimit(float x, float limit) {
	const float safeLimit = (limit > 1e-6f) ? limit : 1e-6f;
	return safeLimit * fastTanh(x / safeLimit);
}

inline float smoothstep01(float x) {
	x = clamp01(x);
	return x * x * (3.f - 2.f * x);
}

inline float wrap01(float x) {
	return x - std::floor(x);
}

inline float wrap01Fast(float x) {
	if (x >= 1.f) {
		x -= 1.f;
	}
	else if (x < 0.f) {
		x += 1.f;
	}
	return x;
}

} // namespace levi_math
