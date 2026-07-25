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
	const bool pass = umi::validateLayout(layout) && layout.pegCount == 88 && layout.segmentCount == 25;
	return {"Pearl layout validity", pass,
		"pegs=" + std::to_string(layout.pegCount) + " segments=" + std::to_string(layout.segmentCount)};
}

TestResult testStaggeredGridAndBottomClearance() {
	const umi::Layout first = umi::makePearlLayout(1u);
	const umi::Layout second = umi::makePearlLayout(987654321u);
	bool staggered = first.pegCount == 88 && second.pegCount == first.pegCount;
	int index = 0;
	for (int row = 0; row < 11 && staggered; ++row) {
		const bool expandedOddDisplayRow = row >= 2 && row <= 6 && (row % 2 == 0);
		const int columns = expandedOddDisplayRow ? 9 : ((row & 1) ? 8 : 7);
		const float firstX = 0.5f * (umi::BOARD_W - 110.f * float(columns - 1));
		for (int column = 0; column < columns; ++column, ++index) {
			const umi::Peg& peg = first.pegs[static_cast<std::size_t>(index)];
			const umi::Peg& otherSeedPeg = second.pegs[static_cast<std::size_t>(index)];
			staggered = staggered && std::fabs(peg.pos.x - (firstX + 110.f * column)) < 1e-5f &&
				std::fabs(peg.pos.y - (190.f + 105.f * row)) < 1e-5f &&
				std::fabs(peg.pos.x - otherSeedPeg.pos.x) < 1e-5f &&
				std::fabs(peg.pos.y - otherSeedPeg.pos.y) < 1e-5f;
		}
	}
	float firstDividerY = umi::BOARD_H;
	for (int i = 0; i < first.segmentCount; ++i) {
		const umi::Segment& segment = first.segments[static_cast<std::size_t>(i)];
		if (std::fabs(segment.a.x - segment.b.x) < 1e-5f && segment.a.x > 100.f && segment.a.x < 900.f) {
			firstDividerY = std::min(firstDividerY, std::min(segment.a.y, segment.b.y));
		}
	}
	float lastPegBottom = 0.f;
	for (int i = 0; i < first.pegCount; ++i) {
		const umi::Peg& peg = first.pegs[static_cast<std::size_t>(i)];
		lastPegBottom = std::max(lastPegBottom, peg.pos.y + peg.radius);
	}
	const float clearance = firstDividerY - lastPegBottom;
	bool compactSinks = std::fabs(first.sinks.front().pos.x - 205.f) < 1e-5f &&
		std::fabs(first.sinks.back().pos.x - 795.f) < 1e-5f;
	for (const umi::Sink& sink : first.sinks) compactSinks = compactSinks && std::fabs(sink.radius - 38.f) < 1e-5f;
	const bool contouredWalls = first.segments[0].a.x < 0.f && first.segments[16].b.x > 150.f;
	return {"Staggered grid and contoured sink approach", staggered && compactSinks && contouredWalls && clearance >= 150.f,
		"clearance=" + std::to_string(clearance) + " sinkRadius=" + std::to_string(first.sinks[0].radius)};
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

TestResult testBurstSpawnSeparation() {
	umi::Engine engine;
	engine.setCapacity(64);
	int spawned = 0;
	for (int burst = 0; burst < 8; ++burst) {
		spawned += engine.spawnBurst(8, (burst & 1) ? 1.f : 0.f);
	}
	bool separated = spawned == 64 && engine.getActiveCount() == 64;
	float minimumGap = 1.0e9f;
	const auto& balls = engine.getBalls();
	for (int first = 0; first < engine.getCapacity(); ++first) {
		if (!balls[static_cast<std::size_t>(first)].active) continue;
		const umi::Ball& a = balls[static_cast<std::size_t>(first)];
		separated = separated && a.pos.x >= 70.f && a.pos.x <= 930.f;
		for (int second = first + 1; second < engine.getCapacity(); ++second) {
			if (!balls[static_cast<std::size_t>(second)].active) continue;
			const umi::Ball& b = balls[static_cast<std::size_t>(second)];
			const float dx = b.pos.x - a.pos.x;
			const float dy = b.pos.y - a.pos.y;
			const float distance = std::sqrt(dx * dx + dy * dy);
			minimumGap = std::min(minimumGap, distance - (a.radius + b.radius));
			separated = separated && distance >= a.radius + b.radius;
		}
	}
	return {"Burst spawning avoids pre-overlapping pearls", separated,
		"spawned=" + std::to_string(spawned) + " minimumGap=" + std::to_string(minimumGap)};
}

TestResult testPearlInteraction() {
	umi::Engine engine;
	engine.setCapacity(2);
	const bool spawned =
		engine.spawnAt({480.f, 50.f}, {100.f, 0.f})
		&& engine.spawnAt({515.f, 50.f}, {-100.f, 0.f});
	umi::PhysicsParams params;
	params.gravity = 0.f;
	params.drag = 0.f;
	params.chaos = 0.f;
	params.restitution = 1.f;
	engine.step(params);

	const auto& balls = engine.getBalls();
	const umi::Ball& first = balls[0];
	const umi::Ball& second = balls[1];
	const float dx = second.pos.x - first.pos.x;
	const float dy = second.pos.y - first.pos.y;
	const float distance = std::sqrt(dx * dx + dy * dy);
	const float momentumX = first.vel.x + second.vel.x;
	const bool pass = spawned
		&& first.active && second.active
		&& distance >= first.radius + second.radius - 1e-4f
		&& first.vel.x < 0.f && second.vel.x > 0.f
		&& std::fabs(momentumX) < 1e-4f;
	return {"Pearls separate and exchange equal-mass impulse", pass,
		"distance=" + std::to_string(distance)
			+ " velocities=" + std::to_string(first.vel.x)
			+ "/" + std::to_string(second.vel.x)};
}

TestResult testStaticColliderCoverage() {
	umi::Engine engine;
	engine.setCapacity(1);
	umi::PhysicsParams params;
	params.gravity = 0.f;
	params.restitution = 0.5f;
	params.drag = 0.f;
	params.chaos = 0.f;
	const umi::Layout& layout = engine.getLayout();
	int tested = 0;
	std::string failedCollider;

	auto displacedFrom = [&](umi::Vec2 probe) {
		engine.clear();
		if (!engine.spawnAt(probe)) {
			return false;
		}
		const umi::StepEvents events = engine.step(params);
		if (events.captureCount > 0) {
			return true;
		}
		for (const umi::Ball& ball : engine.getBalls()) {
			if (!ball.active) {
				continue;
			}
			const float dx = ball.pos.x - probe.x;
			const float dy = ball.pos.y - probe.y;
			return dx * dx + dy * dy > 1.0e-4f;
		}
		return false;
	};

	bool pass = true;
	for (int i = 0; i < layout.pegCount; ++i) {
		if (!displacedFrom(layout.pegs[static_cast<std::size_t>(i)].pos)) {
			pass = false;
			if (failedCollider.empty()) failedCollider = "peg " + std::to_string(i);
		}
		tested++;
	}
	for (int i = 0; i < layout.segmentCount; ++i) {
		const umi::Segment& segment = layout.segments[static_cast<std::size_t>(i)];
		umi::Vec2 probe {
			0.5f * (segment.a.x + segment.b.x),
			0.5f * (segment.a.y + segment.b.y)
		};
		for (const umi::Sink& sink : layout.sinks) {
			const float dx = probe.x - sink.pos.x;
			const float dy = probe.y - sink.pos.y;
			if (dx * dx + dy * dy <= sink.radius * sink.radius) {
				probe = segment.a;
				break;
			}
		}
		if (!displacedFrom(probe)) {
			pass = false;
			if (failedCollider.empty()) failedCollider = "segment " + std::to_string(i);
		}
		tested++;
	}
	return {"Static collider broad-phase coverage", pass,
		"colliders=" + std::to_string(tested)
			+ (failedCollider.empty() ? "" : " firstFailure=" + failedCollider)};
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
		testStaggeredGridAndBottomClearance(),
		testSpawnCapacityPolicies(),
		testBurstSpawnSeparation(),
		testPearlInteraction(),
		testStaticColliderCoverage(),
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
