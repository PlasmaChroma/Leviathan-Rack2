#pragma once

#include <array>
#include <cstdint>

namespace umi {

constexpr int MAX_BALLS = 64;
constexpr int MAX_PEGS = 128;
constexpr int MAX_SEGMENTS = 96;
constexpr int SINK_COUNT = 8;
constexpr float BOARD_W = 1000.f;
constexpr float BOARD_H = 1600.f;
constexpr float PHYSICS_DT = 1.f / 240.f;

struct Vec2 {
	float x = 0.f;
	float y = 0.f;

	Vec2() = default;
	Vec2(float xValue, float yValue)
		: x(xValue), y(yValue) {
	}
};

struct Peg {
	Vec2 pos;
	float radius = 22.f;
	std::uint8_t visualType = 0;

	Peg() = default;
	Peg(Vec2 position, float radiusValue, std::uint8_t type)
		: pos(position), radius(radiusValue), visualType(type) {
	}
};

struct Segment {
	Vec2 a;
	Vec2 b;
	float radius = 8.f;
	std::uint8_t material = 0;
};

struct Sink {
	Vec2 pos;
	float radius = 56.f;
	std::uint8_t outputIndex = 0;
};

struct Layout {
	std::array<Peg, MAX_PEGS> pegs {};
	std::array<Segment, MAX_SEGMENTS> segments {};
	std::array<Sink, SINK_COUNT> sinks {};
	int pegCount = 0;
	int segmentCount = 0;
	Vec2 spawnCenter {500.f, 80.f};
	Vec2 spawnSpread {120.f, 0.f};
};

std::uint32_t hashSeed(std::uint32_t value);
Layout makePearlLayout(std::uint32_t seed);
bool validateLayout(const Layout& layout);

} // namespace umi
