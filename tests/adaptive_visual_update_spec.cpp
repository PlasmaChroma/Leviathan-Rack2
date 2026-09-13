#include "../src/render/AdaptiveVisualUpdate.hpp"
#include <cassert>
#include <iostream>

int main() {
	using namespace leviathan::render;
	VisualFramePressure pressure;
	double now = 0.0;
	pressure.observe(now, 30.0);
	for (int i = 0; i < 300; ++i) pressure.observe(now += 1.0 / 30.0, 30.0);
	assert(pressure.level == 0); // A deliberate 30 FPS limit is healthy.
	for (int i = 0; i < 120; ++i) {
		pressure.observe(now += 1.0 / 15.0, 30.0);
		const double average = pressure.average;
		pressure.observe(now, 30.0);
		assert(pressure.average == average); // Multiple module consumers count once.
	}
	assert(pressure.level == 3);
	for (int i = 0; i < 30; ++i) pressure.observe(now += 1.0 / 30.0, 30.0);
	assert(pressure.level > 0); // Recovery is deliberately slower.
	for (int i = 0; i < 300; ++i) pressure.observe(now += 1.0 / 30.0, 30.0);
	assert(pressure.level == 0);
	pressure.observe(now += 2.0, 30.0);
	assert(pressure.level == 0); // A hidden window does not cause throttling.
	VisualUpdatePolicy policy;
	assert(policy.interval(3) == 0.1);
	VisualUpdateGate gate;
	assert(gate.due(1.01, 0.1));
	assert(!gate.due(1.02, 0.1));
	assert(gate.due(1.12, 0.1)); // Pending final state eventually gets a refresh.
	std::cout << "adaptive_visual_update_spec passed\n";
}
