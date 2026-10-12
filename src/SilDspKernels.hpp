#pragma once

#include <algorithm>
#include <simd/Vector.hpp>
#include <simd/functions.hpp>

namespace sil {

// Float clamp for the audio path. Rack's clamp() is std::fmax(std::fmin()), which Rack's
// build flags (no -ffinite-math-only) compile to two libm calls per use; std::min/max lower
// to minss/maxss. A NaN input is returned as NaN here rather than as the upper bound.
inline float clampf(float x, float lo, float hi) {
	return std::min(std::max(x, lo), hi);
}

// Same filter and same setCutoff() contract as rack::dsp::RCFilter (r is in radians/sample),
// with the recursion rewritten as y = k * (x + x1) - m * y1 so no divide sits in the
// loop-carried dependency. Coefficients are precomputed in setCutoff().
struct OnePoleRC {
	float k = 1.f;
	float m = 0.f;
	float x = 0.f;
	float y = 0.f;

	void setCutoff(float r) {
		const float c = 2.f / r;
		k = 1.f / (1.f + c);
		m = (1.f - c) / (1.f + c);
	}
	void process(float in) {
		const float out = k * (in + x) - m * y;
		x = in;
		y = out;
	}
	float lowpass() const { return y; }
	float highpass() const { return x - y; }
	void reset() {
		x = 0.f;
		y = 0.f;
	}
};

// Four independent band detectors evaluated together. Each lane is the same pair of
// OnePoleRC filters (high-pass into low-pass) followed by an |x| attack/release envelope,
// exactly as the scalar code ran them. All lanes share one attack and one release
// coefficient, which holds for every detector group in Sil.
struct DetectorBank4 {
	using f4 = rack::simd::float_4;

	f4 hpK = 1.f;
	f4 hpM = 0.f;
	f4 hpX = 0.f;
	f4 hpY = 0.f;
	f4 lpK = 1.f;
	f4 lpM = 0.f;
	f4 lpX = 0.f;
	f4 lpY = 0.f;
	f4 env = 1e-6f;

	// hpR/lpR use the RCFilter::setCutoff() radians/sample convention.
	void setLane(int lane, float hpR, float lpR) {
		coefficients(hpR, hpK[lane], hpM[lane]);
		coefficients(lpR, lpK[lane], lpM[lane]);
	}
	void resetEnvelopes() { env = 1e-6f; }
	void resetFilters() {
		hpX = 0.f;
		hpY = 0.f;
		lpX = 0.f;
		lpY = 0.f;
	}
	void reset() {
		resetFilters();
		resetEnvelopes();
	}
	void process(f4 in, float attack, float release) {
		const f4 hpOut = hpK * (in + hpX) - hpM * hpY;
		hpX = in;
		hpY = hpOut;
		const f4 band = hpX - hpY;                                   // RCFilter::highpass()
		const f4 lpOut = lpK * (band + lpX) - lpM * lpY;
		lpX = band;
		lpY = lpOut;
		const f4 ax = rack::simd::fabs(lpY);                         // RCFilter::lowpass()
		const f4 c = rack::simd::ifelse(ax > env, f4(attack), f4(release));
		env = ax + c * (env - ax);
	}

private:
	static void coefficients(float r, float& k, float& m) {
		const float c = 2.f / r;
		k = 1.f / (1.f + c);
		m = (1.f - c) / (1.f + c);
	}
};

} // namespace sil
