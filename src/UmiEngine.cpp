#include "UmiEngine.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

namespace umi {
namespace {

constexpr float MAX_SPEED = 2400.f;
constexpr float MAX_SPEED_SQ = MAX_SPEED * MAX_SPEED;
constexpr float TANGENTIAL_RETENTION = 0.985f;

Vec2 add(Vec2 a, Vec2 b) {
	return {a.x + b.x, a.y + b.y};
}

Vec2 sub(Vec2 a, Vec2 b) {
	return {a.x - b.x, a.y - b.y};
}

Vec2 mul(Vec2 value, float scalar) {
	return {value.x * scalar, value.y * scalar};
}

float dot(Vec2 a, Vec2 b) {
	return a.x * b.x + a.y * b.y;
}

float lengthSquared(Vec2 value) {
	return dot(value, value);
}

bool finiteVec(Vec2 value) {
	return std::isfinite(value.x) && std::isfinite(value.y);
}

Vec2 deterministicNormal(std::uint32_t ballId, int colliderIndex) {
	const std::uint32_t selector = hashSeed(ballId ^ (std::uint32_t(colliderIndex) * 0x9e3779b9u)) & 3u;
	switch (selector) {
		case 0: return {1.f, 0.f};
		case 1: return {-1.f, 0.f};
		case 2: return {0.f, 1.f};
		default: return {0.f, -1.f};
	}
}

bool sweptPointHitsCircle(Vec2 from, Vec2 to, Vec2 center, float radius) {
	const Vec2 travel = sub(to, from);
	const float travelSq = lengthSquared(travel);
	float t = 0.f;
	if (travelSq > 1.0e-9f) {
		t = std::max(0.f, std::min(1.f, dot(sub(center, from), travel) / travelSq));
	}
	const Vec2 closest = add(from, mul(travel, t));
	return lengthSquared(sub(closest, center)) <= radius * radius;
}

} // namespace

void Engine::Rng::reset(std::uint32_t newState) {
	state = newState ? newState : 0x6d2b79f5u;
}

std::uint32_t Engine::Rng::next() {
	std::uint32_t x = state;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	state = x ? x : 0x6d2b79f5u;
	return state;
}

float Engine::Rng::uniform() {
	return float(next() >> 8) * (1.f / 16777215.f);
}

float Engine::Rng::bipolar() {
	return uniform() * 2.f - 1.f;
}

Engine::Engine() {
	reset(1u);
}

void Engine::reset(std::uint32_t newSeed) {
	seed = newSeed ? newSeed : 0x6d2b79f5u;
	layout = makePearlLayout(seed);
	rng.reset(hashSeed(seed ^ 0x53494d55u));
	nextBallId = 1u;
	clear();
}

void Engine::clear() {
	for (Ball& ball : balls) {
		ball = Ball {};
	}
	activeCount = 0;
}

void Engine::setCapacity(int requestedMaxBalls) {
	capacity = std::max(1, std::min(MAX_BALLS, requestedMaxBalls));
	while (activeCount > capacity) {
		const int oldest = findOldestSlot();
		if (oldest < 0) {
			break;
		}
		deactivate(oldest);
	}
}

void Engine::setReplaceOldest(bool enabled) {
	replaceOldest = enabled;
}

int Engine::findFreeSlot() const {
	for (int i = 0; i < capacity; ++i) {
		if (!balls[static_cast<std::size_t>(i)].active) {
			return i;
		}
	}
	return -1;
}

int Engine::findOldestSlot() const {
	int oldestSlot = -1;
	std::uint32_t oldestId = std::numeric_limits<std::uint32_t>::max();
	for (int i = 0; i < capacity; ++i) {
		const Ball& ball = balls[static_cast<std::size_t>(i)];
		if (ball.active && ball.id < oldestId) {
			oldestId = ball.id;
			oldestSlot = i;
		}
	}
	return oldestSlot;
}

void Engine::deactivate(int slot) {
	Ball& ball = balls[static_cast<std::size_t>(slot)];
	if (!ball.active) {
		return;
	}
	ball.active = false;
	activeCount--;
}

bool Engine::spawnAt(Vec2 pos, Vec2 velocity) {
	int slot = findFreeSlot();
	if (slot < 0 && replaceOldest) {
		slot = findOldestSlot();
		if (slot >= 0) {
			deactivate(slot);
		}
	}
	if (slot < 0) {
		return false;
	}
	Ball& ball = balls[static_cast<std::size_t>(slot)];
	ball = Ball {};
	ball.pos = pos;
	ball.vel = velocity;
	ball.id = nextBallId++;
	if (nextBallId == 0u) {
		nextBallId = 1u;
	}
	ball.active = true;
	activeCount++;
	return true;
}

int Engine::spawnBurst(int density, float normalizedX) {
	const int count = std::max(1, std::min(8, density));
	const float baseX = normalizedX >= 0.f
		? 100.f + std::max(0.f, std::min(1.f, normalizedX)) * 800.f
		: layout.spawnCenter.x + rng.bipolar() * layout.spawnSpread.x;
	int spawned = 0;
	for (int i = 0; i < count; ++i) {
		const float centered = float(i) - 0.5f * float(count - 1);
		const Vec2 pos {
			std::max(70.f, std::min(930.f, baseX + centered * 7.f + rng.bipolar() * 3.f)),
			layout.spawnCenter.y - std::fabs(centered) * 2.f
		};
		const Vec2 velocity {rng.bipolar() * 28.f + centered * 2.f, rng.uniform() * 18.f};
		if (spawnAt(pos, velocity)) {
			spawned++;
		}
	}
	return spawned;
}

void Engine::collideBallWithPeg(Ball& ball, const Peg& peg, float restitution, int colliderIndex) {
	Vec2 delta = sub(ball.pos, peg.pos);
	const float minDistance = ball.radius + peg.radius;
	const float distanceSq = lengthSquared(delta);
	if (distanceSq >= minDistance * minDistance) {
		return;
	}
	Vec2 normal;
	float distance = 0.f;
	if (distanceSq > 1.0e-10f) {
		distance = std::sqrt(distanceSq);
		normal = mul(delta, 1.f / distance);
	}
	else {
		normal = deterministicNormal(ball.id, colliderIndex);
	}
	ball.pos = add(ball.pos, mul(normal, minDistance - distance));
	const float normalVelocity = dot(ball.vel, normal);
	if (normalVelocity < 0.f) {
		const Vec2 normalPart = mul(normal, normalVelocity);
		const Vec2 tangentPart = sub(ball.vel, normalPart);
		ball.vel = sub(mul(tangentPart, TANGENTIAL_RETENTION), mul(normalPart, restitution));
	}
}

void Engine::collideBallWithSegment(Ball& ball, const Segment& segment, float restitution, int colliderIndex) {
	const Vec2 axis = sub(segment.b, segment.a);
	const float axisSq = lengthSquared(axis);
	float t = 0.f;
	if (axisSq > 1.0e-10f) {
		t = std::max(0.f, std::min(1.f, dot(sub(ball.pos, segment.a), axis) / axisSq));
	}
	const Vec2 closest = add(segment.a, mul(axis, t));
	const Peg capsulePoint {closest, segment.radius, 0};
	collideBallWithPeg(ball, capsulePoint, restitution, colliderIndex);
}

StepEvents Engine::step(const PhysicsParams& params, float dt) {
	StepEvents events;
	const float safeDt = std::max(0.f, std::min(0.05f, dt));
	const float gravity = std::max(0.f, std::min(4000.f, params.gravity));
	const float tilt = std::max(-1.f, std::min(1.f, params.tilt));
	const float restitution = std::max(0.f, std::min(1.f, params.restitution));
	const float chaos = std::max(0.f, std::min(1.f, params.chaos));
	const float drag = std::max(0.f, std::min(8.f, params.drag));
	const float damping = 1.f / (1.f + drag * safeDt);

	for (int slot = 0; slot < capacity; ++slot) {
		Ball& ball = balls[static_cast<std::size_t>(slot)];
		if (!ball.active) {
			continue;
		}
		const Vec2 previousPos = ball.pos;
		ball.vel.x += (tilt * gravity * 0.45f + rng.bipolar() * chaos * 80.f) * safeDt;
		ball.vel.y += (gravity + rng.bipolar() * chaos * 28.f) * safeDt;
		ball.vel = mul(ball.vel, damping);
		const float speedSqBeforeClamp = lengthSquared(ball.vel);
		if (speedSqBeforeClamp > MAX_SPEED_SQ) {
			ball.vel = mul(ball.vel, MAX_SPEED / std::sqrt(speedSqBeforeClamp));
		}
		ball.pos = add(ball.pos, mul(ball.vel, safeDt));

		for (int i = 0; i < layout.segmentCount; ++i) {
			collideBallWithSegment(ball, layout.segments[static_cast<std::size_t>(i)], restitution, MAX_PEGS + i);
		}
		for (int i = 0; i < layout.pegCount; ++i) {
			collideBallWithPeg(ball, layout.pegs[static_cast<std::size_t>(i)], restitution, i);
		}

		bool captured = false;
		for (int sinkIndex = 0; sinkIndex < SINK_COUNT; ++sinkIndex) {
			const Sink& sink = layout.sinks[static_cast<std::size_t>(sinkIndex)];
			if (!sweptPointHitsCircle(previousPos, ball.pos, sink.pos, sink.radius)) {
				continue;
			}
			CaptureEvent& event = events.captures[static_cast<std::size_t>(events.captureCount++)];
			event.ballId = ball.id;
			event.sinkIndex = static_cast<std::uint8_t>(sinkIndex);
			event.speed = std::sqrt(lengthSquared(ball.vel));
			deactivate(slot);
			captured = true;
			break;
		}
		if (captured) {
			continue;
		}

		ball.age += safeDt;
		const float speedSq = lengthSquared(ball.vel);
		if (speedSq < 25.f) {
			ball.lowSpeedTime += safeDt;
		}
		else {
			ball.lowSpeedTime = 0.f;
			ball.nudged = false;
		}
		if (ball.lowSpeedTime > 2.f && !ball.nudged) {
			ball.vel.x += rng.bipolar() * 80.f;
			ball.vel.y -= 25.f + rng.uniform() * 35.f;
			ball.nudged = true;
		}

		const bool invalid = !finiteVec(ball.pos) || !finiteVec(ball.vel);
		const bool escaped = ball.pos.x < -300.f || ball.pos.x > 1300.f || ball.pos.y < -300.f || ball.pos.y > 1900.f;
		if (invalid || escaped || ball.age > 12.f || ball.lowSpeedTime > 5.f) {
			deactivate(slot);
		}
	}
	return events;
}

} // namespace umi
