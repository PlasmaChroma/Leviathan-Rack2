#include "UmiEngine.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

namespace umi {
namespace {

constexpr float MAX_SPEED = 2400.f;
constexpr float MAX_SPEED_SQ = MAX_SPEED * MAX_SPEED;
constexpr float TANGENTIAL_RETENTION = 0.985f;
constexpr float SPAWN_MIN_X = 70.f;
constexpr float SPAWN_MAX_X = 930.f;
constexpr float SPAWN_SPACING = 40.f;
constexpr std::array<float, 3> SPAWN_ROWS {{80.f, 40.f, 120.f}};
constexpr float COLLIDER_GRID_MIN_X = -320.f;
constexpr float COLLIDER_GRID_MAX_X = 1320.f;
constexpr float COLLIDER_GRID_MIN_Y = -320.f;
constexpr float COLLIDER_GRID_MAX_Y = 1920.f;
constexpr float MAX_BALL_RADIUS = 18.f;

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

Vec2 deterministicPairNormal(std::uint32_t firstId, std::uint32_t secondId) {
	const std::uint32_t low = std::min(firstId, secondId);
	const std::uint32_t high = std::max(firstId, secondId);
	const std::uint32_t selector = hashSeed(
		low * 0x9e3779b9u ^ high * 0x85ebca6bu) & 7u;
	constexpr float DIAGONAL = 0.7071067811865475f;
	switch (selector) {
		case 0: return {1.f, 0.f};
		case 1: return {DIAGONAL, DIAGONAL};
		case 2: return {0.f, 1.f};
		case 3: return {-DIAGONAL, DIAGONAL};
		case 4: return {-1.f, 0.f};
		case 5: return {-DIAGONAL, -DIAGONAL};
		case 6: return {0.f, -1.f};
		default: return {DIAGONAL, -DIAGONAL};
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
	rebuildColliderGrid();
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

bool Engine::spawnPositionClear(Vec2 pos, float radius, int ignoredSlot) const {
	for (int slot = 0; slot < capacity; ++slot) {
		if (slot == ignoredSlot) {
			continue;
		}
		const Ball& ball = balls[static_cast<std::size_t>(slot)];
		if (!ball.active) {
			continue;
		}
		const float minimumDistance = radius + ball.radius + 2.f;
		if (lengthSquared(sub(pos, ball.pos)) < minimumDistance * minimumDistance) {
			return false;
		}
	}
	return true;
}

bool Engine::findBurstSpawnPosition(
	float preferredX,
	int ignoredSlot,
	Vec2* position) const {
	if (!position) {
		return false;
	}
	constexpr int LANE_COUNT =
		int((SPAWN_MAX_X - SPAWN_MIN_X) / SPAWN_SPACING) + 1;
	const float radius = Ball {}.radius;
	for (float y : SPAWN_ROWS) {
		int nearestLane = int(std::round((preferredX - SPAWN_MIN_X) / SPAWN_SPACING));
		nearestLane = std::max(0, std::min(LANE_COUNT - 1, nearestLane));
		for (int laneDistance = 0; laneDistance < LANE_COUNT; ++laneDistance) {
			const int candidates[] = {
				nearestLane + laneDistance,
				nearestLane - laneDistance
			};
			const int candidateCount = laneDistance == 0 ? 1 : 2;
			for (int candidateIndex = 0; candidateIndex < candidateCount; ++candidateIndex) {
				const int lane = candidates[candidateIndex];
				if (lane < 0 || lane >= LANE_COUNT) {
					continue;
				}
				const Vec2 candidate {
					SPAWN_MIN_X + float(lane) * SPAWN_SPACING,
					y
				};
				if (spawnPositionClear(candidate, radius, ignoredSlot)) {
					*position = candidate;
					return true;
				}
			}
		}
	}
	return false;
}

void Engine::deactivate(int slot) {
	Ball& ball = balls[static_cast<std::size_t>(slot)];
	if (!ball.active) {
		return;
	}
	ball.active = false;
	activeCount--;
}

int Engine::colliderCellIndex(Vec2 pos) const {
	const float normalizedX = (pos.x - COLLIDER_GRID_MIN_X)
		/ (COLLIDER_GRID_MAX_X - COLLIDER_GRID_MIN_X);
	const float normalizedY = (pos.y - COLLIDER_GRID_MIN_Y)
		/ (COLLIDER_GRID_MAX_Y - COLLIDER_GRID_MIN_Y);
	const int column = std::max(0, std::min(
		COLLIDER_GRID_COLUMNS - 1,
		int(normalizedX * float(COLLIDER_GRID_COLUMNS))));
	const int row = std::max(0, std::min(
		COLLIDER_GRID_ROWS - 1,
		int(normalizedY * float(COLLIDER_GRID_ROWS))));
	return row * COLLIDER_GRID_COLUMNS + column;
}

void Engine::rebuildColliderGrid() {
	colliderGrid = {};
	colliderGridValid = true;

	const float cellWidth =
		(COLLIDER_GRID_MAX_X - COLLIDER_GRID_MIN_X) / float(COLLIDER_GRID_COLUMNS);
	const float cellHeight =
		(COLLIDER_GRID_MAX_Y - COLLIDER_GRID_MIN_Y) / float(COLLIDER_GRID_ROWS);
	auto cellColumn = [&](float x) {
		return std::max(0, std::min(COLLIDER_GRID_COLUMNS - 1,
			int((x - COLLIDER_GRID_MIN_X) / cellWidth)));
	};
	auto cellRow = [&](float y) {
		return std::max(0, std::min(COLLIDER_GRID_ROWS - 1,
			int((y - COLLIDER_GRID_MIN_Y) / cellHeight)));
	};

	for (int pegIndex = 0; pegIndex < layout.pegCount; ++pegIndex) {
		const Peg& peg = layout.pegs[static_cast<std::size_t>(pegIndex)];
		const float reach = peg.radius + MAX_BALL_RADIUS;
		const int firstColumn = cellColumn(peg.pos.x - reach);
		const int lastColumn = cellColumn(peg.pos.x + reach);
		const int firstRow = cellRow(peg.pos.y - reach);
		const int lastRow = cellRow(peg.pos.y + reach);
		for (int row = firstRow; row <= lastRow; ++row) {
			for (int column = firstColumn; column <= lastColumn; ++column) {
				ColliderCell& cell = colliderGrid[static_cast<std::size_t>(
					row * COLLIDER_GRID_COLUMNS + column)];
				if (cell.pegCount >= MAX_COLLIDERS_PER_CELL) {
					colliderGridValid = false;
					continue;
				}
				cell.pegIndices[static_cast<std::size_t>(cell.pegCount++)] =
					static_cast<std::uint8_t>(pegIndex);
			}
		}
	}

	for (int segmentIndex = 0; segmentIndex < layout.segmentCount; ++segmentIndex) {
		const Segment& segment = layout.segments[static_cast<std::size_t>(segmentIndex)];
		const float reach = segment.radius + MAX_BALL_RADIUS;
		const int firstColumn = cellColumn(std::min(segment.a.x, segment.b.x) - reach);
		const int lastColumn = cellColumn(std::max(segment.a.x, segment.b.x) + reach);
		const int firstRow = cellRow(std::min(segment.a.y, segment.b.y) - reach);
		const int lastRow = cellRow(std::max(segment.a.y, segment.b.y) + reach);
		for (int row = firstRow; row <= lastRow; ++row) {
			for (int column = firstColumn; column <= lastColumn; ++column) {
				ColliderCell& cell = colliderGrid[static_cast<std::size_t>(
					row * COLLIDER_GRID_COLUMNS + column)];
				if (cell.segmentCount >= MAX_COLLIDERS_PER_CELL) {
					colliderGridValid = false;
					continue;
				}
				cell.segmentIndices[static_cast<std::size_t>(cell.segmentCount++)] =
					static_cast<std::uint8_t>(segmentIndex);
			}
		}
	}

	sinkMinY = std::numeric_limits<float>::max();
	sinkMaxY = std::numeric_limits<float>::lowest();
	for (const Sink& sink : layout.sinks) {
		sinkMinY = std::min(sinkMinY, sink.pos.y - sink.radius);
		sinkMaxY = std::max(sinkMaxY, sink.pos.y + sink.radius);
	}
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
	const float firstPreferredX = baseX - 0.5f * float(count - 1) * SPAWN_SPACING;
	int spawned = 0;
	for (int i = 0; i < count; ++i) {
		const float centered = float(i) - 0.5f * float(count - 1);
		const int ignoredSlot = activeCount >= capacity && replaceOldest
			? findOldestSlot()
			: -1;
		Vec2 pos;
		if (!findBurstSpawnPosition(
			firstPreferredX + float(i) * SPAWN_SPACING,
			ignoredSlot,
			&pos)) {
			continue;
		}
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

void Engine::collideBalls(Ball& first, Ball& second, float restitution) {
	Vec2 delta = sub(second.pos, first.pos);
	const float minimumDistance = first.radius + second.radius;
	const float distanceSq = lengthSquared(delta);
	if (distanceSq >= minimumDistance * minimumDistance) {
		return;
	}

	float distance = 0.f;
	Vec2 normal;
	if (distanceSq > 1.0e-10f) {
		distance = std::sqrt(distanceSq);
		normal = mul(delta, 1.f / distance);
	}
	else {
		normal = deterministicPairNormal(first.id, second.id);
	}

	const Vec2 correction = mul(normal, 0.5f * (minimumDistance - distance));
	first.pos = sub(first.pos, correction);
	second.pos = add(second.pos, correction);

	const float relativeNormalVelocity = dot(sub(second.vel, first.vel), normal);
	if (relativeNormalVelocity >= 0.f) {
		return;
	}
	const float impulseMagnitude =
		-0.5f * (1.f + restitution) * relativeNormalVelocity;
	const Vec2 impulse = mul(normal, impulseMagnitude);
	first.vel = sub(first.vel, impulse);
	second.vel = add(second.vel, impulse);
}

StepEvents Engine::step(const PhysicsParams& params, float dt) {
	StepEvents events;
	if (activeCount == 0) {
		return events;
	}
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
		if (!finiteVec(ball.pos) || !finiteVec(ball.vel)) {
			deactivate(slot);
			continue;
		}

		if (colliderGridValid) {
			const ColliderCell& cell =
				colliderGrid[static_cast<std::size_t>(colliderCellIndex(ball.pos))];
			for (int candidate = 0; candidate < cell.segmentCount; ++candidate) {
				const int i = cell.segmentIndices[static_cast<std::size_t>(candidate)];
				collideBallWithSegment(ball,
					layout.segments[static_cast<std::size_t>(i)],
					restitution, MAX_PEGS + i);
			}
			for (int candidate = 0; candidate < cell.pegCount; ++candidate) {
				const int i = cell.pegIndices[static_cast<std::size_t>(candidate)];
				collideBallWithPeg(ball,
					layout.pegs[static_cast<std::size_t>(i)],
					restitution, i);
			}
		}
		else {
			for (int i = 0; i < layout.segmentCount; ++i) {
				collideBallWithSegment(ball, layout.segments[static_cast<std::size_t>(i)], restitution, MAX_PEGS + i);
			}
			for (int i = 0; i < layout.pegCount; ++i) {
				collideBallWithPeg(ball, layout.pegs[static_cast<std::size_t>(i)], restitution, i);
			}
		}

		bool captured = false;
		const float sweptMinY = std::min(previousPos.y, ball.pos.y);
		const float sweptMaxY = std::max(previousPos.y, ball.pos.y);
		if (sweptMaxY >= sinkMinY && sweptMinY <= sinkMaxY) {
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

	for (int firstSlot = 0; firstSlot < capacity; ++firstSlot) {
		Ball& first = balls[static_cast<std::size_t>(firstSlot)];
		if (!first.active) {
			continue;
		}
		for (int secondSlot = firstSlot + 1; secondSlot < capacity; ++secondSlot) {
			Ball& second = balls[static_cast<std::size_t>(secondSlot)];
			if (second.active) {
				const float minimumDistance = first.radius + second.radius;
				if (std::abs(second.pos.x - first.pos.x) >= minimumDistance ||
					std::abs(second.pos.y - first.pos.y) >= minimumDistance) {
					continue;
				}
				collideBalls(first, second, restitution);
			}
		}
	}
	return events;
}

} // namespace umi
