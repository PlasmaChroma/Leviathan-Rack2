#pragma once

#include <simd/Vector.hpp>
#include <simd/functions.hpp>
#include <algorithm>
#include <cmath>

namespace sil {

// Eight reconstruction phases, with L/R and adjacent phases paired in SIMD.
// Coefficients are shared and initialized during module construction, never on
// the audio thread's first process call. No allocation or transcendental work
// occurs in process(). The signal must be delayed past kLatencySamples.
class TruePeakDetector {
public:
	static constexpr int kLatencySamples = 32;
	TruePeakDetector() { (void)kernel(); reset(); }
	void reset() {
		for (auto& v : history) v = 0.f;
		write = blockSamples = 0;
		blockPeak = previousBlockPeak = 0.f;
	}
	float process(float left, float right, float gain, float ceiling) {
		history[write] = history[write + 64] = F4(left, right, left, right);
		write = (write + 1) & 63;
		const Kernel& k = kernel();
		const float rawPeak = std::max(std::fabs(left), std::fabs(right));
		blockPeak = std::max(blockPeak, rawPeak);
		if (++blockSamples == 64) {
			previousBlockPeak = blockPeak;
			blockPeak = 0.f;
			blockSamples = 0;
		}
		// Two block maxima cover at least the complete FIR history. The kernel's
		// L1 norm bounds every interpolated sample, so quiet material can skip
		// convolution without changing limiter decisions. History stays warm.
		if (std::max(blockPeak, previousBlockPeak) * k.absoluteBound * gain < ceiling)
			return rawPeak;
		F4 peaks = 0.f;
		const F4* newest = history + write + 63;
		for (int group = 0; group < 4; ++group) {
			// Independent accumulators avoid a 64-add dependency chain. The
			// mirrored history also removes wrap arithmetic from this hot loop.
			F4 s0 = 0.f, s1 = 0.f, s2 = 0.f, s3 = 0.f;
			const F4* w = k.weights[group];
			for (int j = 0; j < 64; j += 4) {
				s0 += newest[-j] * w[j];
				s1 += newest[-j-1] * w[j+1];
				s2 += newest[-j-2] * w[j+2];
				s3 += newest[-j-3] * w[j+3];
			}
			const F4 sum = (s0 + s1) + (s2 + s3);
			peaks = rack::simd::fmax(peaks, rack::simd::fabs(sum));
		}
		return std::max(rawPeak,
			std::max(std::max(peaks[0], peaks[1]), std::max(peaks[2], peaks[3])));
	}

private:
	using F4 = rack::simd::float_4;
	F4 history[128];
	int write = 0;
	int blockSamples = 0;
	float blockPeak = 0.f, previousBlockPeak = 0.f;
	struct Kernel {
		F4 weights[4][64];
		float absoluteBound = 1.f;
		Kernel() {
			constexpr double pi = 3.14159265358979323846;
			for (int phase = 0; phase < 8; ++phase) {
				double taps[64], sum = 0.0;
				for (int j = 0; j < 64; ++j) {
					const double x = j - 32.0 + phase / 8.0;
					const double sinc = std::fabs(x) < 1e-12 ? 1.0 : std::sin(pi*x)/(pi*x);
					taps[j] = sinc * 0.5 * (1.0 + std::cos(pi*x/32.0));
					sum += taps[j];
				}
				double absoluteSum = 0.0;
				for (int j = 0; j < 64; ++j) {
					const float value = float(taps[j] / sum);
					absoluteSum += std::fabs(value);
					weights[phase/2][j][(phase%2)*2] = value;
					weights[phase/2][j][(phase%2)*2+1] = value;
				}
				absoluteBound = std::max(absoluteBound, float(absoluteSum * 1.000001));
			}
		}
	};
	static const Kernel& kernel() { static const Kernel instance; return instance; }
};

} // namespace sil
