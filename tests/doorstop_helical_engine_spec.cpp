#include "../src/HelicalContinuumEngine.hpp"
#include "../src/DoorstopEngineRouter.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace {

struct Result {
	std::string name;
	bool pass;
	std::string detail;
};

Result zeroStateRemainsZero() {
	doorstop::HelicalContinuumEngine engine;
	engine.setSampleRate(48000.f);
	bool exact = true;
	for (int i = 0; i < 48000; ++i) {
		const doorstop::Frame frame = engine.process(1.f / 48000.f);
		exact = exact && frame.outputVolts == 0.f && frame.displacement == 0.f
			&& frame.velocity == 0.f && frame.energy == 0.f && frame.sleeping;
	}
	return {"V3 zero state remains exactly silent", exact,
		"exact=" + std::to_string(exact)};
}

Result supportedRatesAreFiniteAndPairedMotionIsVisible() {
	bool pass = true;
	float largestPeak = 0.f;
	int smallestCrossings = 1000000;
	for (float rate : {44100.f, 48000.f, 96000.f, 192000.f}) {
		doorstop::HelicalContinuumEngine engine;
		engine.setSampleRate(rate);
		engine.setSpecimenSeed(77u);
		engine.strike(0.75f);
		float peak = 0.f;
		float previous = 0.f;
		int crossings = 0;
		for (int i = 0; i < int(2.f * rate); ++i) {
			const doorstop::Frame frame = engine.process(1.f / rate);
			peak = std::max(peak, std::fabs(frame.outputVolts));
			if (i > int(0.020f * rate)
				&& ((previous < 0.f && frame.displacement >= 0.f)
					|| (previous > 0.f && frame.displacement <= 0.f))) ++crossings;
			previous = frame.displacement;
			pass = pass && std::isfinite(frame.outputVolts)
				&& std::isfinite(frame.displacement)
				&& std::fabs(frame.outputVolts) <= 5.0001f;
		}
		largestPeak = std::max(largestPeak, peak);
		smallestCrossings = std::min(smallestCrossings, crossings);
		pass = pass && peak > 0.01f && crossings >= 60 && crossings <= 110;
	}
	return {"V3 is finite across sample rates and retains the low paired motion",
		pass, "largestPeak=" + std::to_string(largestPeak)
			+ " smallestCrossings=" + std::to_string(smallestCrossings)};
}

Result specimenPopulationIsDeterministicAndDistinct() {
	constexpr float rate = 48000.f;
	constexpr float dt = 1.f / rate;
	doorstop::HelicalContinuumEngine a;
	doorstop::HelicalContinuumEngine b;
	doorstop::HelicalContinuumEngine c;
	for (doorstop::HelicalContinuumEngine* engine : {&a, &b, &c}) {
		engine->setSampleRate(rate);
	}
	a.setSpecimenSeed(0x12345678u);
	b.setSpecimenSeed(0x12345678u);
	c.setSpecimenSeed(0x87654321u);
	a.strike(0.7f);
	b.strike(0.7f);
	c.strike(0.7f);
	bool exact = true;
	double distinct = 0.0;
	for (int i = 0; i < int(2.f * rate); ++i) {
		const float sa = a.process(dt).outputVolts;
		const float sb = b.process(dt).outputVolts;
		const float sc = c.process(dt).outputVolts;
		exact = exact && sa == sb;
		distinct += std::fabs(sa - sc);
	}
	return {"V3 specimen coefficients are deterministic and distinct",
		exact && distinct > 10.0,
		"exact=" + std::to_string(exact)
			+ " distinct=" + std::to_string(distinct)};
}

Result routerReturnsExactV3Frames() {
	constexpr float rate = 48000.f;
	constexpr float dt = 1.f / rate;
	doorstop::HelicalContinuumEngine direct;
	doorstop::DoorstopEngineRouter routed;
	direct.setSampleRate(rate);
	routed.setSampleRate(rate);
	direct.setSpecimenSeed(7331u);
	routed.setSpecimenSeed(7331u);
	routed.setEngineMode(doorstop::EngineMode::ReferenceV3);
	for (int i = 0; i < int(0.020f * rate); ++i) routed.process(dt);
	direct.strike(0.8f);
	routed.strike(0.8f);
	bool exact = true;
	for (int i = 0; i < int(2.f * rate); ++i) {
		const doorstop::Frame a = direct.process(dt);
		const doorstop::Frame b = routed.process(dt);
		exact = exact && a.outputVolts == b.outputVolts
			&& a.displacement == b.displacement && a.velocity == b.velocity
			&& a.energy == b.energy && a.strikeLight == b.strikeLight
			&& a.sleeping == b.sleeping && a.enteredSleep == b.enteredSleep;
		if (!exact) break;
	}
	return {"Steady V3 routing returns the exact engine frame", exact,
		"exact=" + std::to_string(exact)};
}

} // namespace

int main() {
	const std::vector<Result> results {
		zeroStateRemainsZero(),
		supportedRatesAreFiniteAndPairedMotionIsVisible(),
		specimenPopulationIsDeterministicAndDistinct(),
		routerReturnsExactV3Frames(),
	};
	int failed = 0;
	std::cout << "Doorstop Helical Engine Spec\n-----------------------------\n";
	for (const Result& result : results) {
		std::cout << (result.pass ? "[PASS] " : "[FAIL] ") << result.name
			<< " :: " << result.detail << "\n";
		if (!result.pass) ++failed;
	}
	std::cout << "-----------------------------\nSummary: "
		<< (results.size() - failed) << "/" << results.size() << " passed\n";
	return failed == 0 ? 0 : 1;
}
