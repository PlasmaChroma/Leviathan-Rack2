#include "../src/UmiEngine.hpp"

#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace {

struct TestResult {
	std::string name;
	bool pass = false;
	std::string detail;
};

TestResult testLayoutValidity() {
	const umi::Layout layout = umi::makePearlLayout(12345u);
	const bool pass = umi::validateLayout(layout) && layout.pegCount == 71 && layout.segmentCount == 11;
	return {"Pearl layout validity", pass,
		"pegs=" + std::to_string(layout.pegCount) + " segments=" + std::to_string(layout.segmentCount)};
}

TestResult testSpawnCapacityPolicies() {
	umi::Engine engine;
	engine.setCapacity(16);
	const int first = engine.spawnBurst(8);
	const int second = engine.spawnBurst(8);
	const int ignored = engine.spawnBurst(8);
	const int countBeforeReplace = engine.getActiveCount();
	engine.setReplaceOldest(true);
	const int replaced = engine.spawnBurst(8);
	const bool pass = first == 8 && second == 8 && ignored == 0 && countBeforeReplace == 16 &&
		replaced == 8 && engine.getActiveCount() == 16;
	return {"Spawn capacity policies", pass,
		"first=" + std::to_string(first) + " ignored=" + std::to_string(ignored) +
		" replaced=" + std::to_string(replaced)};
}

TestResult testEverySinkCapture() {
	umi::PhysicsParams params;
	params.gravity = 0.f;
	params.drag = 0.f;
	params.chaos = 0.f;
	bool pass = true;
	int captured = 0;
	for (int sinkIndex = 0; sinkIndex < umi::SINK_COUNT; ++sinkIndex) {
		umi::Engine engine;
		const umi::Sink& sink = engine.getLayout().sinks[static_cast<std::size_t>(sinkIndex)];
		pass = pass && engine.spawnAt(sink.pos, {0.f, 50.f});
		const umi::StepEvents events = engine.step(params);
		pass = pass && events.captureCount == 1 && events.captures[0].sinkIndex == sinkIndex && engine.getActiveCount() == 0;
		captured += events.captureCount;
	}
	return {"Every sink captures exactly once", pass, "captures=" + std::to_string(captured)};
}

std::vector<int> runDeterministicSequence(std::uint32_t seed) {
	umi::Engine engine;
	engine.reset(seed);
	engine.setCapacity(64);
	umi::PhysicsParams params;
	params.gravity = 1250.f;
	params.restitution = 0.58f;
	params.drag = 0.8f;
	params.chaos = 0.16f;
	std::vector<int> result;
	for (int step = 0; step < 4200; ++step) {
		if (step % 180 == 0) {
			engine.spawnBurst(2);
		}
		const umi::StepEvents events = engine.step(params);
		for (int i = 0; i < events.captureCount; ++i) {
			result.push_back(int(events.captures[static_cast<std::size_t>(i)].sinkIndex));
		}
	}
	return result;
}

TestResult testDeterminism() {
	const std::vector<int> a = runDeterministicSequence(0x12345678u);
	const std::vector<int> b = runDeterministicSequence(0x12345678u);
	const std::vector<int> c = runDeterministicSequence(0x87654321u);
	const bool pass = !a.empty() && a == b && a != c;
	return {"Seeded determinism", pass,
		"events=" + std::to_string(a.size()) + " alternate=" + std::to_string(c.size())};
}

TestResult testLongRunStability() {
	umi::Engine engine;
	engine.reset(777u);
	engine.setCapacity(64);
	umi::PhysicsParams params;
	params.gravity = 2200.f;
	params.tilt = 0.25f;
	params.restitution = 0.92f;
	params.drag = 0.f;
	params.chaos = 1.f;
	int captures = 0;
	for (int step = 0; step < 12000; ++step) {
		if (step % 30 == 0) {
			engine.spawnBurst(8);
		}
		captures += engine.step(params).captureCount;
	}
	bool finite = engine.getActiveCount() >= 0 && engine.getActiveCount() <= engine.getCapacity();
	for (const umi::Ball& ball : engine.getBalls()) {
		if (ball.active) {
			finite = finite && std::isfinite(ball.pos.x) && std::isfinite(ball.pos.y) &&
				std::isfinite(ball.vel.x) && std::isfinite(ball.vel.y);
		}
	}
	return {"Bounded long-run stability", finite && captures > 0,
		"active=" + std::to_string(engine.getActiveCount()) + " captures=" + std::to_string(captures)};
}

TestResult testClearAndResetAreSilent() {
	umi::Engine engine;
	engine.spawnBurst(8);
	engine.clear();
	umi::PhysicsParams params;
	const bool clearSilent = engine.getActiveCount() == 0 && engine.step(params).captureCount == 0;
	engine.spawnBurst(4);
	engine.reset(99u);
	const bool resetSilent = engine.getActiveCount() == 0 && engine.step(params).captureCount == 0;
	return {"Clear and reset are silent", clearSilent && resetSilent,
		"clear=" + std::to_string(clearSilent) + " reset=" + std::to_string(resetSilent)};
}

} // namespace

int main() {
	const std::vector<TestResult> tests {
		testLayoutValidity(),
		testSpawnCapacityPolicies(),
		testEverySinkCapture(),
		testDeterminism(),
		testLongRunStability(),
		testClearAndResetAreSilent()
	};

	int failed = 0;
	std::cout << "Umi Engine Spec\n";
	std::cout << "---------------\n";
	for (const TestResult& test : tests) {
		std::cout << (test.pass ? "[PASS] " : "[FAIL] ") << test.name << " :: " << test.detail << "\n";
		failed += test.pass ? 0 : 1;
	}
	std::cout << "---------------\n";
	std::cout << "Summary: " << (tests.size() - static_cast<std::size_t>(failed)) << "/" << tests.size() << " passed\n";
	return failed == 0 ? 0 : 1;
}
