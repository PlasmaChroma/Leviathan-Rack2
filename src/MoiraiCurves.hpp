#pragma once

#include "MoiraiTypes.hpp"

#include <algorithm>
#include <cmath>

namespace moirai {

inline float clamp01(float value) {
	return std::max(0.f, std::min(1.f, value));
}

inline float linearInterpolate(float a, float b, float amount) {
	return a + (b - a) * amount;
}

inline float fourth(float value) {
	const float squared = value * value;
	return squared * squared;
}

inline float powerBias(float phase, float amount) {
	const float u = clamp01(phase);
	const float bias = std::max(-1.f, std::min(1.f, amount));
	if (bias >= 0.f) return linearInterpolate(u, fourth(u), bias);
	const float mirrored = 1.f - u;
	return 1.f - linearInterpolate(mirrored, fourth(mirrored), -bias);
}

inline float shapeCurve(float phase, CurveType type, float amount = 0.f) {
	const float u = clamp01(phase);
	switch (type) {
		case CurveType::LINEAR: return u;
		case CurveType::SMOOTHSTEP: return u * u * (3.f - 2.f * u);
		case CurveType::SIGMOID:
			return u * u * u * (u * (u * 6.f - 15.f) + 10.f);
		case CurveType::HOLD: return u >= 1.f ? 1.f : 0.f;
		case CurveType::STEP: return 1.f;
		case CurveType::EXPONENTIAL: return powerBias(u, amount);
		case CurveType::LOGARITHMIC: return powerBias(u, -std::min(1.f, std::fabs(amount)));
	}
	return u;
}

inline float evaluateSegment(float start, float target, float phase,
		CurveType type, float authoredAmount, float curveBias = 0.f) {
	float amount = authoredAmount;
	if (type == CurveType::EXPONENTIAL || type == CurveType::LOGARITHMIC)
		amount = std::max(-1.f, std::min(1.f, amount + curveBias));
	return linearInterpolate(start, target, shapeCurve(phase, type, amount));
}

inline float monotoneHermite(float time, const CompiledContourPoint& left,
		const CompiledContourPoint& right) {
	const float span = right.time - left.time;
	if (!(span > 0.f)) return right.value;
	const float u = clamp01((time - left.time) / span);
	const float u2 = u * u;
	const float u3 = u2 * u;
	const float h00 = 2.f * u3 - 3.f * u2 + 1.f;
	const float h10 = u3 - 2.f * u2 + u;
	const float h01 = -2.f * u3 + 3.f * u2;
	const float h11 = u3 - u2;
	const float cubic = h00 * left.value + h10 * span * left.tangent
		+ h01 * right.value + h11 * span * right.tangent;
	return std::max(std::min(left.value, right.value),
		std::min(std::max(left.value, right.value), cubic));
}

inline float evaluateContour(const CompiledProgram& program, float phase) {
	if (program.points.empty()) return 0.f;
	if (program.points.size() == 1) return program.points.front().value;
	const float time = clamp01(phase);
	if (time <= program.points.front().time) return program.points.front().value;
	if (time >= program.points.back().time) return program.points.back().value;
	size_t high = 1;
	while (high < program.points.size() && time > program.points[high].time) ++high;
	if (high >= program.points.size()) return program.points.back().value;
	const CompiledContourPoint& left = program.points[high - 1];
	const CompiledContourPoint& right = program.points[high];
	if (program.interpolation == Interpolation::MONOTONE_CUBIC)
		return monotoneHermite(time, left, right);
	const float span = right.time - left.time;
	return span > 0.f
		? linearInterpolate(left.value, right.value, (time - left.time) / span)
		: right.value;
}

} // namespace moirai
