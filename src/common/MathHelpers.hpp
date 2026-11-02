#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>

namespace levi_math {

inline float clamp01(float x) {
	return std::max(0.f, std::min(x, 1.f));
}

// Fast float clamp with legacy operand ordering. For finite ordered bounds,
// NaN x returns hi. Equal values select the bound's signed zero. NaN bounds
// are not supported; this is not a general fmin/fmax semantic replacement.
inline float clampf(float x, float lo, float hi) {
	return std::max(lo, std::min(hi, x));
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

// Single-cycle correction; callers guarantee finite x in [-1, 2).
inline float wrap01Fast(float x) {
	if (x >= 1.f) {
		x -= 1.f;
	}
	else if (x < 0.f) {
		x += 1.f;
	}
	return x;
}

// ---------------------------------------------------------------------------
// Scalar exp2/log2/pow helpers (formerly FastAudioMath.hpp)
// ---------------------------------------------------------------------------
// Scalar range-reduced audio math, independent of Rack/SIMD headers. Uses the
// polynomial/exponent-splitting approach already used by Phonex and Rack's
// exp2_taylor5, with a centered interval and extra terms for integer-frame Gene
// timing. No tables, allocation, lazy initialization, or libm calls.
// Preconditions: finite x in [-1022, 1022]. Exact at integer octaves.
inline double exp2Audio(double x) {
	const int exponent = int(x >= 0 ? x + 0.5 : x - 0.5);
	const double y = (x-exponent) * 0.69314718055994530942;
	const std::uint64_t bits = std::uint64_t(exponent+1023) << 52;
	double scale;
	std::memcpy(&scale, &bits, sizeof(scale));
	const double polynomial = 1.0 + y*(1.0 + y*(0.5 + y*(1.0/6 + y*(1.0/24 +
		y*(1.0/120 + y*(1.0/720 + y*(1.0/5040 + y*(1.0/40320 +
		y*(1.0/362880 + y*(1.0/3628800))))))))));
	return scale*polynomial;
}

// Preconditions: positive, finite, normal double. Normalize to [1/sqrt(2),
// sqrt(2)] so the atanh series has |y| <= 0.172, unlike a linear bit-log estimate.
inline double log2Audio(double x) {
	std::uint64_t bits;
	std::memcpy(&bits, &x, sizeof(bits));
	int exponent = int(bits >> 52) - 1023;
	bits = (bits & UINT64_C(0x000fffffffffffff)) | UINT64_C(0x3ff0000000000000);
	double m;
	std::memcpy(&m, &bits, sizeof(m));
	if (m > 1.4142135623730950488) { m *= 0.5; ++exponent; }
	const double y = (m-1.0)/(m+1.0), square = y*y;
	return exponent + 2.8853900817779268147*y*(1.0 + square*(1.0/3 +
		square*(1.0/5 + square*(1.0/7 + square*(1.0/9 +
		square*(1.0/11 + square*(1.0/13)))))));
}
// Reuse Andy Simper's coefficients from Rack's exp2_taylor5 (also used by
// Flux, Proc, Bifurx and Wyrm). Scalar double exponent splitting avoids Rack's
// float offset rounding. <0.001 cent across Chimera's pitch/rate domain.
// Preconditions: finite x in [-1022, 1022], as for exp2Audio().
inline double exp2Pitch(double x) {
	int exponent = int(x);
	if (x < exponent) --exponent;
	const double f = x-exponent;
	const std::uint64_t bits = std::uint64_t(exponent+1023) << 52;
	double scale;
	std::memcpy(&scale, &bits, sizeof(scale));
	return scale*(1.0 + f*(0.69315169353961 + f*(0.2401595990753 +
		f*(0.055817908652 + f*(0.008991698010 + f*0.001879100722)))));
}
// Legacy policy: nonpositive x returns zero (including 0^0). For positive x,
// require a normal finite double and log2Audio(x)*exponent in [-1022, 1022].
inline double powUnitAudio(double x, double exponent) {
	return x <= 0 ? 0.0 : exp2Pitch(log2Audio(x)*exponent);
}

// Checked entry points are opt-in. Existing bounded functions keep their exact
// arithmetic and cost. Use strict floating-point flags when NaN/Inf handling
// is required; finite-math-only invalidates these contracts.
inline double exp2AudioChecked(double x) {
	return x >= -1022.0 && x <= 1022.0 ? exp2Audio(x) : std::exp2(x);
}

inline double exp2PitchChecked(double x) {
	return x >= -1022.0 && x <= 1022.0 ? exp2Pitch(x) : std::exp2(x);
}

inline double log2AudioChecked(double x) {
	return x >= std::numeric_limits<double>::min()
		&& x <= std::numeric_limits<double>::max() ? log2Audio(x) : std::log2(x);
}

// Retains powUnitAudio's nonpositive-input policy. Exceptional positive
// inputs or transformed exponents outside the fast domain use libm pow.
inline double powUnitAudioChecked(double x, double exponent) {
	if (x <= 0.0) return 0.0;
	if (x >= std::numeric_limits<double>::min()
		&& x <= std::numeric_limits<double>::max()) {
		const double power = log2Audio(x) * exponent;
		if (power >= -1022.0 && power <= 1022.0) return exp2Pitch(power);
	}
	return std::pow(x, exponent);
}

// Arbitrary finite cycle phases. Nonfinite input produces NaN without a
// float-to-integer conversion. Large float inputs already have lost fraction
// bits; wrapping cannot recover that precision.
inline float sinCyclesAudioChecked(float cycles) {
	if (!std::isfinite(cycles)) return std::numeric_limits<float>::quiet_NaN();
	return sinCyclesAudioBounded(cycles >= -2.f && cycles <= 2.f
		? cycles : wrap01(cycles));
}

inline float triangleCyclesAudioChecked(float cycles) {
	if (!std::isfinite(cycles)) return std::numeric_limits<float>::quiet_NaN();
	return triangleCyclesAudioBounded(cycles >= -2.f && cycles <= 2.f
		? cycles : wrap01(cycles));
}

} // namespace levi_math
