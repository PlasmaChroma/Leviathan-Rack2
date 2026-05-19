#pragma once

#include "ChronomawState.hpp"
#include "plugin.hpp"
#include <array>
#include <cmath>

namespace chronomaw {

struct WaveformDescriptor {
	WaveformMode mode;
	const char* label;
};

inline const std::array<WaveformDescriptor, 11>& waveformDescriptors() {
	static const std::array<WaveformDescriptor, 11> k = {{
		{WaveformMode::Gate, "Gate/Pulse"},
		{WaveformMode::RatchetX2, "Ratchet x2"},
		{WaveformMode::RatchetX4, "Ratchet x4"},
		{WaveformMode::Triangle, "Triangle"},
		{WaveformMode::Trapezoid, "Trapezoid"},
		{WaveformMode::Sine, "Sine"},
		{WaveformMode::Hump, "Hump"},
		{WaveformMode::ExpEnvelope, "Exp Envelope"},
		{WaveformMode::LogEnvelope, "Log Envelope"},
		{WaveformMode::ClassicRandom, "Classic Random"},
		{WaveformMode::SmoothRandom, "Smooth Random"},
	}};
	return k;
}

inline int waveformCount() {
	return int(waveformDescriptors().size());
}

inline const char* waveformLabel(WaveformMode mode) {
	for (const auto& d : waveformDescriptors()) {
		if (d.mode == mode) {
			return d.label;
		}
	}
	return waveformDescriptors()[0].label;
}

inline WaveformMode waveformFromIndex(int index) {
	const int i = clamp(index, 0, waveformCount() - 1);
	return waveformDescriptors()[size_t(i)].mode;
}

inline int waveformIndex(WaveformMode mode) {
	for (int i = 0; i < waveformCount(); ++i) {
		if (waveformDescriptors()[size_t(i)].mode == mode) {
			return i;
		}
	}
	return 0;
}

inline int waveformLutSize() {
	return 2048;
}

inline float sampleLut(const std::array<float, 2048>& lut, float phase01) {
	const float p = phase01 - std::floor(phase01);
	const float scaled = p * float(waveformLutSize() - 1);
	const int i0 = clamp(int(scaled), 0, waveformLutSize() - 1);
	const int i1 = std::min(i0 + 1, waveformLutSize() - 1);
	const float frac = scaled - float(i0);
	return lut[size_t(i0)] + (lut[size_t(i1)] - lut[size_t(i0)]) * frac;
}

struct WaveformLuts {
	std::array<float, 2048> triangle {};
	std::array<float, 2048> trapezoid {};
	std::array<float, 2048> sine {};
	std::array<float, 2048> expEnv {};
	std::array<float, 2048> logEnv {};
};

inline const WaveformLuts& waveformLuts() {
	static const WaveformLuts luts = []() {
		WaveformLuts out;
		constexpr float kTwoPi = 6.28318530717958647692f;
		constexpr float kHalfPi = 1.57079632679489661923f;
		for (int i = 0; i < waveformLutSize(); ++i) {
			const float p = float(i) / float(waveformLutSize() - 1);
			const float tri = (p < 0.5f) ? (2.f * p) : (2.f * (1.f - p));
			float trap = 0.f;
			if (p < 0.2f) {
				trap = p / 0.2f;
			}
			else if (p < 0.8f) {
				trap = 1.f;
			}
			else {
				trap = (1.f - p) / 0.2f;
			}
			const float s = 0.5f * (std::sin(kTwoPi * p - kHalfPi) + 1.f);
			const float e = std::exp(-6.f * p);
			const float l = 1.f - std::log1p(9.f * p) / std::log1p(9.f);
			out.triangle[size_t(i)] = clamp(tri * kOutputMaxV, kOutputMinV, kOutputMaxV);
			out.trapezoid[size_t(i)] = clamp(trap * kOutputMaxV, kOutputMinV, kOutputMaxV);
			out.sine[size_t(i)] = clamp(s * kOutputMaxV, kOutputMinV, kOutputMaxV);
			out.expEnv[size_t(i)] = clamp(e * kOutputMaxV, kOutputMinV, kOutputMaxV);
			out.logEnv[size_t(i)] = clamp(l * kOutputMaxV, kOutputMinV, kOutputMaxV);
		}
		return out;
	}();
	return luts;
}

inline uint32_t mixHash(uint32_t x) {
	x ^= x >> 16;
	x *= 0x7feb352du;
	x ^= x >> 15;
	x *= 0x846ca68bu;
	x ^= x >> 16;
	return x;
}

inline float unitRand(uint32_t seed, uint64_t cycle) {
	const uint32_t lo = uint32_t(cycle & 0xffffffffu);
	const uint32_t hi = uint32_t((cycle >> 32) & 0xffffffffu);
	const uint32_t h = mixHash(seed ^ mixHash(lo ^ (hi * 0x9e3779b9u)));
	return float(h) * (1.0f / 4294967295.0f);
}

inline double effectiveTimingMultiplier(const OutputState& outState) {
	const double raw = double(clamp(outState.multiplier, 1.f / 16384.f, 192.f));
	if (outState.modifierMode == ModifierMode::Div) {
		const double divisor = double(clamp(std::round(float(1.0 / std::max(1.0 / 16384.0, raw))), 1.f, 16384.f));
		return 1.0 / divisor;
	}
	if (outState.modifierMode == ModifierMode::Mult) {
		return double(clamp(std::round(float(raw)), 1.f, 192.f));
	}
	return raw;
}

inline double rawTimingPhase(const OutputState& outState, double basePhase01) {
	double p = basePhase01;
	p *= effectiveTimingMultiplier(outState);
	p += double(clamp(outState.phasePct * 0.005f, -0.5f, 0.5f));
	p += double(clamp(outState.rotatePct * 0.005f, -0.5f, 0.5f));
	return p;
}

inline float applyTimingPhase(const OutputState& outState, double basePhase01) {
	double p = rawTimingPhase(outState, basePhase01);
	p -= std::floor(p);

	const float swing = clamp(outState.swingPct * 0.01f, -1.f, 1.f);
	const float halfScale = 1.f + 0.6f * swing;
	if (p < 0.5f) {
		p = 0.5f * clamp((p / 0.5f) / std::max(0.2f, halfScale), 0.f, 1.f);
	}
	else {
		const float t = (p - 0.5f) / 0.5f;
		p = 0.5f + 0.5f * clamp(t * std::max(0.2f, halfScale), 0.f, 1.f);
	}

	const float skew = clamp(outState.skewPct * 0.01f, -1.f, 1.f);
	const float gamma = (skew >= 0.f) ? (1.f / (1.f + 1.8f * skew)) : (1.f - 0.65f * skew);
	p = std::pow(double(clamp(float(p), 0.f, 1.f)), double(clamp(gamma, 0.25f, 4.f)));
	return float(p - std::floor(p));
}

inline float waveformInternalVoltage(const OutputState& outState, float phase01, uint64_t cycle) {
	const float p = phase01 - std::floor(phase01);
	const float width = clamp(outState.widthPct * 0.01f, 0.01f, 1.f);
	const WaveformMode waveform = outState.waveform;
	switch (waveform) {
	case WaveformMode::RatchetX2: {
		// Two triggers per cycle, width as duty per subpulse.
		const float ph = p * 2.f;
		const float sub = ph - std::floor(ph);
		return (sub < width) ? kOutputMaxV : kOutputMinV;
	}
	case WaveformMode::RatchetX4: {
		// Four triggers per cycle, width as duty per subpulse.
		const float ph = p * 4.f;
		const float sub = ph - std::floor(ph);
		return (sub < width) ? kOutputMaxV : kOutputMinV;
	}
	case WaveformMode::Triangle: {
		return sampleLut(waveformLuts().triangle, p);
	}
	case WaveformMode::Trapezoid: {
		return sampleLut(waveformLuts().trapezoid, p);
	}
	case WaveformMode::Sine: {
		return sampleLut(waveformLuts().sine, p);
	}
	case WaveformMode::Hump: {
		// One-cycle parabola hump peaking at mid-cycle.
		const float d = p - 0.5f;
		const float hump = std::max(0.f, 1.f - 4.f * d * d);
		return clamp(hump * kOutputMaxV, kOutputMinV, kOutputMaxV);
	}
	case WaveformMode::ExpEnvelope: {
		// Instant peak at cycle start, smooth exponential decay over cycle.
		const float t = clamp(p / width, 0.f, 1.f);
		return sampleLut(waveformLuts().expEnv, t);
	}
	case WaveformMode::LogEnvelope: {
		// Instant peak at cycle start, logarithmic-style decay (longer tail).
		const float t = clamp(p / width, 0.f, 1.f);
		return sampleLut(waveformLuts().logEnv, t);
	}
	case WaveformMode::ClassicRandom: {
		const float r = unitRand(outState.randomSeed, cycle);
		return clamp(r * kOutputMaxV, kOutputMinV, kOutputMaxV);
	}
	case WaveformMode::SmoothRandom: {
		const float r0 = unitRand(outState.randomSeed, cycle);
		const float r1 = unitRand(outState.randomSeed, cycle + 1u);
		const float s = r0 + (r1 - r0) * p;
		return clamp(s * kOutputMaxV, kOutputMinV, kOutputMaxV);
	}
	case WaveformMode::Gate:
	default:
		return (p < width) ? kOutputMaxV : kOutputMinV;
	}
}

inline float renderOutputVoltage(const OutputState& outState, bool running, double basePhase01, uint64_t cycle) {
	const float phase01 = applyTimingPhase(outState, basePhase01);
	const float internalV = running ? waveformInternalVoltage(outState, phase01, cycle) : kOutputMinV;
	if (outState.muted) {
		return 0.f;
	}
	const float level = clamp(outState.levelPct * 0.01f, 0.f, 1.f);
	const float offset = clamp(outState.offsetPct * 0.01f, -1.f, 1.f);
	float v = internalV * level + offset * kOutputMaxV;
	if (outState.invert) {
		v = kOutputMaxV - v;
	}
	return clamp(v, kOutputMinV, kOutputMaxV);
}

} // namespace chronomaw
