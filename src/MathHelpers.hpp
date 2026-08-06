#pragma once

#include <algorithm>
#include <array>
#include <cmath>

namespace levi_math {

inline float clamp01(float x) {
	return std::max(0.f, std::min(x, 1.f));
}

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

constexpr int AUDIO_SINE_LUT_INTERVALS = 2048;
constexpr float AUDIO_SINE_LUT_SCALE = float(AUDIO_SINE_LUT_INTERVALS);

extern const std::array<float, AUDIO_SINE_LUT_INTERVALS + 1> audioSineLut;

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

// Linear-interpolated sine for a phase expressed in complete cycles. The hot
// Puffy/FRENZY caller guarantees cycles in [-2, 2], allowing truncation after a
// fixed positive offset instead of floor(), fmod(), division, or transcendental
// math in the audio path.
inline float sinCyclesAudioBounded(float cycles) {
	const float shifted = cycles + 2.f;
	const int wholeCycles = int(shifted);
	const float wrapped = shifted - float(wholeCycles);
	const float scaled = wrapped * detail::AUDIO_SINE_LUT_SCALE;
	const int index = int(scaled);
	const float fraction = scaled - float(index);
	return detail::audioSineLut[index]
		+ (detail::audioSineLut[index + 1] - detail::audioSineLut[index])
			* fraction;
}

// Triangle wave for a phase expressed in complete cycles. Like the bounded
// sine helper above, Puffy guarantees cycles in [-2, 2], so wrapping needs no
// floor(), fmod(), division, lookup, or transcendental math. The wave crosses
// zero with a positive slope at each whole cycle.
inline float triangleCyclesAudioBounded(float cycles) {
	const float shifted = cycles + 2.f;
	const int wholeCycles = int(shifted);
	const float wrapped = shifted - float(wholeCycles);
	if (wrapped < 0.25f) {
		return 4.f * wrapped;
	}
	if (wrapped < 0.75f) {
		return 2.f - 4.f * wrapped;
	}
	return 4.f * wrapped - 4.f;
}

// Compatibility alias: existing callers retain the established rational curve.
// New DSP should choose tanhLegacy() or tanhAudio() explicitly.
inline float fastTanh(float x) {
	return tanhLegacy(x);
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
