#pragma once

#include "UmiLayout.hpp"

#include <array>
#include <cstdint>

namespace umi {

struct Ball {
	Vec2 pos;
	Vec2 vel;
	float radius = 18.f;
	float age = 0.f;
	float lowSpeedTime = 0.f;
	std::uint32_t id = 0;
	bool active = false;
	bool nudged = false;
};

struct PhysicsParams {
	float gravity = 1100.f;
	float tilt = 0.f;
	float restitution = 0.57f;
	float drag = 1.2f;
	float chaos = 0.08f;
};

struct CaptureEvent {
	std::uint32_t ballId = 0;
	std::uint8_t sinkIndex = 0;
	float speed = 0.f;
};

struct StepEvents {
	std::array<CaptureEvent, MAX_BALLS> captures {};
	int captureCount = 0;
};

class Engine {
public:
	Engine();

	void reset(std::uint32_t newSeed);
	void clear();
	void setCapacity(int requestedMaxBalls);
	void setReplaceOldest(bool enabled);
	int spawnBurst(int density, float normalizedX = -1.f);
	bool spawnAt(Vec2 pos, Vec2 velocity = {});
	StepEvents step(const PhysicsParams& params, float dt = PHYSICS_DT);

	const Layout& getLayout() const { return layout; }
	const std::array<Ball, MAX_BALLS>& getBalls() const { return balls; }
	int getActiveCount() const { return activeCount; }
	int getCapacity() const { return capacity; }
	std::uint32_t getSeed() const { return seed; }

private:
	struct Rng {
		std::uint32_t state = 1u;
		void reset(std::uint32_t newState);
		std::uint32_t next();
		float uniform();
		float bipolar();
	};

	Layout layout;
	std::array<Ball, MAX_BALLS> balls {};
	Rng rng;
	std::uint32_t seed = 1u;
	std::uint32_t nextBallId = 1u;
	int activeCount = 0;
	int capacity = 32;
	bool replaceOldest = false;

	int findFreeSlot() const;
	int findOldestSlot() const;
	void deactivate(int slot);
	void collideBallWithPeg(Ball& ball, const Peg& peg, float restitution, int colliderIndex);
	void collideBallWithSegment(Ball& ball, const Segment& segment, float restitution, int colliderIndex);
};

} // namespace umi
