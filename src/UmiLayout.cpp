#include "UmiLayout.hpp"

#include <algorithm>
#include <cmath>

namespace umi {
namespace {

bool finiteVec(const Vec2& v) {
	return std::isfinite(v.x) && std::isfinite(v.y);
}

} // namespace

std::uint32_t hashSeed(std::uint32_t value) {
	value = value ? value : 0x9e3779b9u;
	value ^= value >> 16;
	value *= 0x7feb352du;
	value ^= value >> 15;
	value *= 0x846ca68bu;
	value ^= value >> 16;
	return value ? value : 0x6d2b79f5u;
}

Layout makePearlLayout(std::uint32_t seed) {
	(void) seed;
	Layout layout;

	constexpr int GRID_ROWS = 11;
	constexpr float GRID_FIRST_Y = 190.f;
	constexpr float GRID_X_SPACING = 110.f;
	constexpr float GRID_Y_SPACING = 105.f;

	for (int row = 0; row < GRID_ROWS; ++row) {
		const bool expandedOddDisplayRow = row >= 2 && row <= 6 && (row % 2 == 0);
		const int columns = expandedOddDisplayRow ? 9 : ((row & 1) ? 8 : 7);
		const float firstX = 0.5f * (BOARD_W - GRID_X_SPACING * float(columns - 1));
		for (int column = 0; column < columns; ++column) {
			Peg& peg = layout.pegs[static_cast<std::size_t>(layout.pegCount++)];
			peg.pos.x = firstX + GRID_X_SPACING * float(column);
			peg.pos.y = GRID_FIRST_Y + GRID_Y_SPACING * float(row);
			peg.radius = 22.f;
			peg.visualType = 0;
		}
	}

	auto addSegment = [&](Vec2 a, Vec2 b, float radius, std::uint8_t material = 0) {
		if (layout.segmentCount >= MAX_SEGMENTS) {
			return;
		}
		Segment& segment = layout.segments[static_cast<std::size_t>(layout.segmentCount++)];
		segment.a = a;
		segment.b = b;
		segment.radius = radius;
		segment.material = material;
	};

	// Follow the illustrated shell: broad and nearly vertical above, then
	// progressively sweep inward to frame the compact sink bank below.
	const Vec2 LEFT_WALL[] = {
		{-12.f, 100.f}, {-12.f, 900.f}, {-5.f, 1030.f}, {10.f, 1130.f},
		{35.f, 1220.f}, {68.f, 1300.f}, {108.f, 1370.f}, {148.f, 1430.f},
		{172.f, 1480.f}, {188.f, 1585.f}
	};
	constexpr int WALL_POINT_COUNT = int(sizeof(LEFT_WALL) / sizeof(LEFT_WALL[0]));
	for (int i = 0; i < WALL_POINT_COUNT - 1; ++i) {
		addSegment(LEFT_WALL[i], LEFT_WALL[i + 1], 10.f, 2);
		addSegment({BOARD_W - LEFT_WALL[i].x, LEFT_WALL[i].y},
			{BOARD_W - LEFT_WALL[i + 1].x, LEFT_WALL[i + 1].y}, 10.f, 2);
	}

	constexpr float SINK_FIRST_X = 205.f;
	constexpr float SINK_LAST_X = 795.f;
	constexpr float SINK_SPACING = (SINK_LAST_X - SINK_FIRST_X) / float(SINK_COUNT - 1);
	for (int i = 0; i < SINK_COUNT; ++i) {
		Sink& sink = layout.sinks[static_cast<std::size_t>(i)];
		sink.pos = {SINK_FIRST_X + SINK_SPACING * float(i), 1510.f};
		sink.radius = 38.f;
		sink.outputIndex = static_cast<std::uint8_t>(i);
	}

	for (int i = 0; i < SINK_COUNT - 1; ++i) {
		const float dividerX = SINK_FIRST_X + SINK_SPACING * (float(i) + 0.5f);
		addSegment({dividerX, 1455.f}, {dividerX, 1585.f}, 4.f, 1);
	}

	return layout;
}

bool validateLayout(const Layout& layout) {
	if (layout.pegCount < 1 || layout.pegCount > MAX_PEGS ||
		layout.segmentCount < 1 || layout.segmentCount > MAX_SEGMENTS ||
		!finiteVec(layout.spawnCenter) || layout.spawnCenter.x < 0.f || layout.spawnCenter.x > BOARD_W ||
		layout.spawnCenter.y < 0.f || layout.spawnCenter.y > BOARD_H) {
		return false;
	}
	for (int i = 0; i < layout.pegCount; ++i) {
		const Peg& peg = layout.pegs[static_cast<std::size_t>(i)];
		if (!finiteVec(peg.pos) || !std::isfinite(peg.radius) || peg.radius <= 0.f ||
			peg.pos.x < 0.f || peg.pos.x > BOARD_W || peg.pos.y < 0.f || peg.pos.y > BOARD_H) {
			return false;
		}
	}
	for (int i = 0; i < layout.segmentCount; ++i) {
		const Segment& segment = layout.segments[static_cast<std::size_t>(i)];
		if (!finiteVec(segment.a) || !finiteVec(segment.b) || !std::isfinite(segment.radius) || segment.radius <= 0.f) {
			return false;
		}
	}
	for (int i = 0; i < SINK_COUNT; ++i) {
		const Sink& sink = layout.sinks[static_cast<std::size_t>(i)];
		if (!finiteVec(sink.pos) || !std::isfinite(sink.radius) || sink.radius <= 0.f ||
			sink.pos.x < 0.f || sink.pos.x > BOARD_W || sink.pos.y < 0.f || sink.pos.y > BOARD_H ||
			sink.outputIndex != i) {
			return false;
		}
	}
	return true;
}

} // namespace umi
