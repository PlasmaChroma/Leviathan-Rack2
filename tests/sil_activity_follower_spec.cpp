#include "../src/sil/SilActivityFollower.hpp"

#include <cmath>
#include <cstdio>
#include <cstdlib>

namespace {
constexpr double pi = 3.14159265358979323846;
constexpr int kDivision = 16;
void require(bool ok, const char* message) {
	if (!ok) { std::fprintf(stderr, "FAIL: %s\n", message); std::exit(1); }
}
// Runs a 440 Hz sine at `amplitude` for `seconds`; returns final activity and the minimum seen.
struct Result { float last = 1.f, minimum = 1.f; };
Result run(sil::ActivityFollower& f, float sr, float amplitude, float seconds, long& t) {
	Result r;
	const long count = long(sr * seconds);
	for (long i = 0; i < count; ++i, ++t) {
		f.processSample(amplitude * float(std::sin(2 * pi * 440 * double(t) / sr)));
		if (t % kDivision == 0) {
			r.last = f.decide();
			r.minimum = std::fmin(r.minimum, r.last);
			require(std::isfinite(r.last) && r.last >= 0.f && r.last <= 1.f, "activity out of range");
		}
	}
	return r;
}
float db(float d) { return std::pow(10.f, d / 20.f); }
}

int main() {
	for (float sr : {44100.f, 48000.f, 96000.f}) {
		sil::ActivityFollower f;
		f.configure(sr, kDivision);
		long t = 0;
		run(f, sr, 0.5f, 1.f, t); // onset from silence starts inactive and rises within ~0.5 s
		require(run(f, sr, 0.5f, 4.f, t).minimum > 0.999f, "steady program keeps full activity");
		require(run(f, sr, 0.5f * db(-20.f), 3.f, t).minimum > 0.999f, "20 dB drop keeps full activity");

		f.reset(); t = 0;
		run(f, sr, 0.5f, 4.f, t);
		const Result drop = run(f, sr, 0.5f * db(-45.f), 1.f, t);
		require(drop.last < 0.05f, "45 dB drop releases activity within 1 s");
		const Result held = run(f, sr, 0.5f * db(-45.f), 12.f, t);
		require(held.last > 0.95f, "sustained quiet passage becomes the new reference");
		const Result back = run(f, sr, 0.5f, 1.f, t);
		require(back.last > 0.99f, "return to loud program restores activity");

		f.reset(); t = 0;
		require(run(f, sr, 0.f, 1.f, t).last < 0.01f, "silence reads as inactive");
	}
	std::printf("sil_activity_follower_spec: OK\n");
	return 0;
}
