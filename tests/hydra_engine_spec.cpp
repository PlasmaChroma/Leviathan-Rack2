#include "../src/hydra/HydraEngine.hpp"

#include <cmath>
#include <cstdio>
#include <cstdlib>

namespace {

int failures = 0;

void check(bool ok, const char* what) {
	if (!ok) {
		std::printf("FAIL: %s\n", what);
		++failures;
	}
}

struct RunStats {
	bool finite = true;
	float peak = 0.f;
	double energy = 0.0;
};

RunStats run(hydra::Engine& e, float rate, float seconds, bool freeze, bool withInput) {
	RunStats s;
	const int n = int(rate * seconds);
	unsigned rng = 12345u;
	for (int i = 0; i < n; ++i) {
		rng = rng * 1664525u + 1013904223u;
		const float noise = withInput ? (float(rng >> 8) / float(1u << 24) - 0.5f) * 0.8f : 0.f;
		float l = 0.f, r = 0.f;
		e.process(noise, noise, freeze, false, &l, &r);
		if (!std::isfinite(l) || !std::isfinite(r)) {
			s.finite = false;
		}
		s.peak = std::fmax(s.peak, std::fmax(std::fabs(l), std::fabs(r)));
		s.energy += double(l) * l + double(r) * r;
	}
	return s;
}

} // namespace

int main() {
	const float rates[] = {44100.f, 48000.f, 96000.f};
	for (float rate : rates) {
		for (int q = 0; q < hydra::QUALITY_LAST; ++q) {
			hydra::Engine e;
			e.init(rate);
			hydra::Controls c;
			c.quality = q;
			c.density = 0.9f;
			c.dryWet = 1.f;
			c.reverb = 0.3f;
			c.size = 0.6f;
			c.time = 0.1f;
			c.feedback = 0.3f;
			e.setControls(c);
			const RunStats s = run(e, rate, 2.0f, false, true);
			check(s.finite, "output finite");
			check(s.peak <= 1.0f, "output bounded");
			check(s.energy > 1e-3, "grains produce output");
		}
	}

	{
		hydra::Engine e;
		e.init(48000.f);
		hydra::Controls c;
		c.synthMode = true;
		c.density = 0.9f;
		c.dryWet = 1.f;
		e.setControls(c);
		const RunStats s = run(e, 48000.f, 2.0f, false, false);
		check(s.finite, "synth finite");
		check(e.inSynthMode(), "synth mode active");
		check(s.energy > 1e-3, "synth produces output");
	}

	if (failures == 0) {
		std::printf("hydra_engine_spec: ok\n");
	}
	return failures == 0 ? 0 : 1;
}
