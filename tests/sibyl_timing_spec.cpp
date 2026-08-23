#include "SibylTiming.hpp"

#include <cmath>
#include <iostream>

namespace {
int failures = 0;
void check(bool condition, const char* name) {
	std::cout << (condition ? "[PASS] " : "[FAIL] ") << name << "\n";
	if (!condition) ++failures;
}

sibyl::Pattern pattern(const char* resolution = "1/16") {
	sibyl::Pattern value;
	value.length = 4;
	value.resolutionStr = resolution;
	value.resolutionBeats = 0.25;
	value.eventIndexByStep.assign(4, -1);
	return value;
}
}

int main() {
	check(sibyl::resolutionSupportsSwing("1/16") &&
		!sibyl::resolutionSupportsSwing("1/16d") && !sibyl::resolutionSupportsSwing("1/16t"),
		"only straight resolutions receive swing");
	check(sibyl::wrappedStep(-1, 4) == 3 && sibyl::wrappedStep(4, 4) == 0,
		"nominal steps wrap with Euclidean semantics");

	auto straight = pattern();
	sibyl::StepEvent even; even.step = 0;
	sibyl::StepEvent odd; odd.step = 1;
	check(std::abs(sibyl::scheduledEventBeat(straight, 0, even, 0.2) - 0.0) < 1e-12,
		"swing leaves the first subdivision fixed");
	check(std::abs(sibyl::scheduledEventBeat(straight, 1, odd, 0.2) - 0.30) < 1e-12,
		"swing delays the second straight subdivision");
	odd.microshift = -0.1f;
	check(std::abs(sibyl::scheduledEventBeat(straight, 1, odd, 0.2) - 0.275) < 1e-7,
		"microshift combines additively with swing");

	auto triplet = pattern("1/16t");
	check(std::abs(sibyl::scheduledEventBeat(triplet, 1, odd, 0.2) - 0.225) < 1e-7,
		"triplet timing ignores swing but retains microshift");

	sibyl::StepEvent wrap; wrap.step = 0; wrap.microshift = -0.25f;
	double wrappedOnset = sibyl::scheduledEventBeat(straight, 4, wrap, 0.0);
	check(std::abs(wrappedOnset - 0.9375) < 1e-12,
		"negative step-zero microshift schedules before the next loop boundary");
	check(sibyl::scheduledEventCrossed(0.93, 0.94, wrappedOnset) &&
		!sibyl::scheduledEventCrossed(0.94, 0.95, wrappedOnset),
		"scheduled onset fires exactly once when crossed");

	straight.steps.push_back(even);
	straight.eventIndexByStep[0] = 0;
	check(sibyl::eventAtStep(straight, 0) == &straight.steps[0] && sibyl::eventAtStep(straight, 2) == nullptr,
		"compiled event lookup distinguishes events from rests in O(1)");

	std::cout << "[SUMMARY] sibyl_timing_spec: " << (failures ? "FAILED" : "passed") << "\n";
	return failures ? 1 : 0;
}
