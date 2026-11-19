#pragma once

#include <simd/Vector.hpp>
#include <simd/functions.hpp>
#include <algorithm>
#include <cmath>

namespace sil {

// Eight reconstruction phases of a kTaps Hann-windowed sinc, parabola-refined
// between phases, with L/R and adjacent phases paired in SIMD.
// Coefficients are shared and initialized during module construction, never on
// the audio thread's first process call. No allocation or transcendental work
// occurs in process(). The signal must be delayed past kLatencySamples.
class TruePeakDetector {
public:
	static constexpr int kTaps = 128;
	static constexpr int kLatencySamples = kTaps / 2;
	TruePeakDetector() { (void)kernel(); reset(); }
	void reset() {
		for (auto& v : history) v = 0.f;
		write = blockSamples = 0;
		blockPeak = previousBlockPeak = 0.f;
		previousValid = false;
	}
	float process(float left, float right, float gain, float ceiling) {
		history[write] = history[write + kTaps] = F4(left, right, left, right);
		write = (write + 1) & (kTaps - 1);
		const Kernel& k = kernel();
		const float rawPeak = std::max(std::fabs(left), std::fabs(right));
		blockPeak = std::max(blockPeak, rawPeak);
		if (++blockSamples == kTaps) {
			previousBlockPeak = blockPeak;
			blockPeak = 0.f;
			blockSamples = 0;
		}
		// Two block maxima cover at least the complete FIR history. The kernel's
		// L1 norm bounds every interpolated sample, so quiet material can skip
		// convolution without changing limiter decisions. History stays warm.
		if (std::max(blockPeak, previousBlockPeak) * k.absoluteBound * gain < ceiling) {
			previousValid = false;
			return rawPeak;
		}
		// curve[ch][2..9] are this sample's phases in ascending time order
		// (phase p sits at t - kTaps/2 + p/8); [0..1] are the previous sample's last
		// two phases, so phase 0 of this sample follows previous phase 7.
		float curve[2][10];
		const F4* newest = history + write + kTaps - 1;
		for (int group = 0; group < 4; ++group) {
			// Independent accumulators avoid a kTaps-add dependency chain. The
			// mirrored history also removes wrap arithmetic from this hot loop.
			F4 s0 = 0.f, s1 = 0.f, s2 = 0.f, s3 = 0.f;
			const F4* w = k.weights[group];
			for (int j = 0; j < kTaps; j += 4) {
				s0 += newest[-j] * w[j];
				s1 += newest[-j-1] * w[j+1];
				s2 += newest[-j-2] * w[j+2];
				s3 += newest[-j-3] * w[j+3];
			}
			const F4 sum = rack::simd::fabs((s0 + s1) + (s2 + s3));
			curve[0][2 + 2*group] = sum[0];
			curve[1][2 + 2*group] = sum[1];
			curve[0][3 + 2*group] = sum[2];
			curve[1][3 + 2*group] = sum[3];
		}
		// The 1/8-sample grid alone under-reads peaks between phases (~0.12 dB
		// at 0.43*sr, more on dense material). A parabola through each local
		// grid maximum recovers the vertex to ~0.01 dB for a few scalar ops.
		const int firstCenter = previousValid ? 1 : 3;
		float peak = rawPeak;
		for (int ch = 0; ch < 2; ++ch) {
			float* v = curve[ch];
			v[0] = previousTail[ch][0];
			v[1] = previousTail[ch][1];
			for (int c = firstCenter - 1; c < 10; ++c) peak = std::max(peak, v[c]);
			for (int c = firstCenter; c < 9; ++c) {
				const float a = v[c-1], b = v[c], d = v[c+1];
				const float denom = a - 2.f*b + d;
				if (b >= a && b >= d && denom < 0.f) {
					const float diff = a - d;
					peak = std::max(peak, b - diff*diff / (8.f*denom));
				}
			}
			previousTail[ch][0] = v[8];
			previousTail[ch][1] = v[9];
		}
		previousValid = true;
		return peak;
	}

private:
	using F4 = rack::simd::float_4;
	F4 history[2 * kTaps];
	int write = 0;
	int blockSamples = 0;
	float blockPeak = 0.f, previousBlockPeak = 0.f;
	float previousTail[2][2] = {};
	bool previousValid = false;
	struct Kernel {
		F4 weights[4][kTaps];
		float absoluteBound = 1.f;
		Kernel() {
			constexpr double pi = 3.14159265358979323846;
			for (int phase = 0; phase < 8; ++phase) {
				double taps[kTaps], sum = 0.0;
				for (int j = 0; j < kTaps; ++j) {
					const double x = j - kTaps / 2.0 + phase / 8.0;
					const double sinc = std::fabs(x) < 1e-12 ? 1.0 : std::sin(pi*x)/(pi*x);
					taps[j] = sinc * 0.5 * (1.0 + std::cos(pi*x/(kTaps / 2.0)));
					sum += taps[j];
				}
				double absoluteSum = 0.0;
				for (int j = 0; j < kTaps; ++j) {
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
