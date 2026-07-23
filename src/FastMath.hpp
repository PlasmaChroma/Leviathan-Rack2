#pragma once

#include <array>
#include <cmath>

namespace levi_math {

// Established Leviathan saturation curve. Keep this stable for modules whose
// sound and patch behavior already depend on it.
inline float tanhLegacy(float x) {
	const float x2 = x * x;
	if (x2 < 9.f) {
		return x * (27.f + x2) / (27.f + 9.f * x2);
	}
	return x > 0.f ? 1.f : -1.f;
}

namespace detail {

constexpr int AUDIO_TANH_LUT_INTERVALS = 1024;
constexpr float AUDIO_TANH_LUT_LIMIT = 8.f;
constexpr float AUDIO_TANH_LUT_SCALE =
	float(AUDIO_TANH_LUT_INTERVALS) / AUDIO_TANH_LUT_LIMIT;

extern const std::array<float, AUDIO_TANH_LUT_INTERVALS + 1> audioTanhLut;

} // namespace detail

// High-accuracy approximation of mathematical tanh() for audio-rate DSP.
// Linear interpolation keeps the maximum normalized error below 6e-6.
inline float tanhAudio(float x) {
	const float magnitude = std::fabs(x);
	if (!(magnitude < detail::AUDIO_TANH_LUT_LIMIT)) {
		// Preserve NaN propagation while saturating finite overflow and infinity.
		return magnitude == magnitude ? std::copysign(1.f, x) : x;
	}
	const float scaled = magnitude * detail::AUDIO_TANH_LUT_SCALE;
	const int index = int(scaled);
	const float fraction = scaled - float(index);
	const float value = detail::audioTanhLut[index]
		+ (detail::audioTanhLut[index + 1] - detail::audioTanhLut[index])
			* fraction;
	return std::copysign(value, x);
}

} // namespace levi_math
