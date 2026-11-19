#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>

// Maths function family extracted from Integral Flux. No Rack dependency.
// Normalization is control-rate work; integration uses multiply/divide only.
namespace maths_function {
constexpr float LINEAR_SHAPE = 0.33f;
constexpr float WARP_K_MAX = 40.f;
constexpr int WARP_SCALE_SAMPLES = 16;
// Match Integral Flux's legacy operand ordering, including NaN/signed zero.
inline float clamp(float x, float lo, float hi) { return std::max(lo, std::min(hi, x)); }
inline float slopeWarp(float x, float s) {
	x = clamp(x, 0.f, 1.f);
	float u = std::fabs(s);
	if (u < 1e-6f) {
		return 1.f;
	}
	float k = WARP_K_MAX * u;
	float x2 = x * x;
	if (s < 0.f) {
		return 1.f / (1.f + k * x2);
	}
	return 1.f + k * x2;
}

inline float slopeWarpScale(float s) {
	if (std::fabs(s) < 1e-6f) {
		return 1.f;
	}
	float sum = 0.f;
	for (int i = 0; i < WARP_SCALE_SAMPLES; ++i) {
		float xi = (i + 0.5f) / float(WARP_SCALE_SAMPLES);
		sum += 1.f / slopeWarp(xi, s);
	}
	return sum / float(WARP_SCALE_SAMPLES);
}

inline float shapeSignedFromKnob(float shape01, float linearShape) {
	shape01 = clamp(shape01, 0.f, 1.f);
	linearShape = clamp(linearShape, 1e-4f, 1.f - 1e-4f);
	if (shape01 < linearShape) {
		return (shape01 - linearShape) / linearShape;
	}
	if (shape01 > linearShape) {
		return (shape01 - linearShape) / (1.f - linearShape);
	}
	return 0.f;
}


inline float phaseFromOutput(float x, float shape, bool rising) {
    x = clamp(x, 0.f, 1.f);
    if (std::fabs(shape) < 1e-6f) return rising ? x : 1.f-x;
    const float start = rising ? 0.f : x;
    const float span = rising ? x : 1.f-x;
    float sum = 0.f;
    for (int i=0;i<WARP_SCALE_SAMPLES;++i) {
        const float t=(float(i)+.5f)/float(WARP_SCALE_SAMPLES);
        sum += 1.f/slopeWarp(start+span*t,shape);
    }
    return clamp(span*sum/float(WARP_SCALE_SAMPLES)/slopeWarpScale(shape),0.f,1.f);
}
template <std::size_t PREVIEW_LUT_SIZE>
inline void buildSegmentLut(std::array<float, PREVIEW_LUT_SIZE>& lut, float curveSigned, bool rising) {
	// Build once per preview update. Midpoint integration reduces visual artifacts at extreme curve asymmetry.
	float scale = slopeWarpScale(curveSigned);
	float dp = 1.f / float(PREVIEW_LUT_SIZE - 1);
	float x = rising ? 0.f : 1.f;
	lut[0] = x;
	for (int i = 1; i < int(PREVIEW_LUT_SIZE); ++i) {
		float k1 = slopeWarp(x, curveSigned) * scale;
		float xMid = rising ? (x + 0.5f * dp * k1) : (x - 0.5f * dp * k1);
		xMid = clamp(xMid, 0.f, 1.f);
		float k2 = slopeWarp(xMid, curveSigned) * scale;
		x += rising ? (dp * k2) : (-dp * k2);
		x = clamp(x, 0.f, 1.f);
		lut[i] = x;
}
	lut.front() = rising ? 0.f : 1.f;
	lut.back() = rising ? 1.f : 0.f;
}

template <std::size_t PREVIEW_LUT_SIZE>
inline float sampleSegmentLut(const std::array<float, PREVIEW_LUT_SIZE>& lut, float t) {
	t = clamp(t, 0.f, 1.f);
	float idx = t * float(PREVIEW_LUT_SIZE - 1);
	int i0 = int(idx);
	int i1 = std::min(i0 + 1, int(PREVIEW_LUT_SIZE) - 1);
	float f = idx - float(i0);
	return lut[i0] + (lut[i1] - lut[i0]) * f;
}

} // namespace maths_function
