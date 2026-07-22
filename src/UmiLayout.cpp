#include "UmiLayout.hpp"

#include <algorithm>
#include <cmath>

namespace umi {
namespace {

struct LayoutRng {
	std::uint32_t state = 1u;

	explicit LayoutRng(std::uint32_t seed)
		: state(seed ? seed : 0x6d2b79f5u) {
	}

	std::uint32_t next() {
		std::uint32_t x = state;
		x ^= x << 13;
		x ^= x >> 17;
		x ^= x << 5;
		state = x ? x : 0x6d2b79f5u;
		return state;
	}

	float bipolar() {
		return (float(next() >> 8) * (1.f / 8388607.5f)) - 1.f;
	}
};

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
	Layout layout;
	LayoutRng rng(hashSeed(seed ^ 0x50454152u));

	constexpr int rowCounts[] = {4, 5, 6, 7, 6, 7, 8, 7, 8, 7, 6};
	constexpr float rowY[] = {220.f, 340.f, 460.f, 580.f, 700.f, 820.f,
		940.f, 1060.f, 1180.f, 1300.f, 1400.f};
	constexpr float spacing = 110.f;

	for (int row = 0; row < 11; ++row) {
		const int count = rowCounts[row];
		const float firstX = 0.5f * (BOARD_W - spacing * float(count - 1));
		for (int column = 0; column < count; ++column) {
			Peg& peg = layout.pegs[static_cast<std::size_t>(layout.pegCount++)];
			peg.pos.x = firstX + spacing * float(column) + rng.bipolar() * 8.f;
			peg.pos.y = rowY[row] + rng.bipolar() * 8.f;
			peg.radius = (row == 3 || row == 7) && (column == 0 || column == count - 1) ? 28.f : 23.f;
			peg.visualType = peg.radius > 24.f ? 1 : 0;
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

	addSegment({42.f, 100.f}, {42.f, 1575.f}, 12.f);
	addSegment({958.f, 100.f}, {958.f, 1575.f}, 12.f);
	addSegment({42.f, 1450.f}, {70.f, 1500.f}, 10.f, 1);
	addSegment({958.f, 1450.f}, {930.f, 1500.f}, 10.f, 1);

	for (int i = 0; i < SINK_COUNT; ++i) {
		Sink& sink = layout.sinks[static_cast<std::size_t>(i)];
		sink.pos = {90.f + 117.f * float(i), 1510.f};
		sink.radius = 46.f;
		sink.outputIndex = static_cast<std::uint8_t>(i);
	}

	for (int i = 0; i < SINK_COUNT - 1; ++i) {
		const float dividerX = 148.5f + 117.f * float(i);
		addSegment({dividerX, 1435.f}, {dividerX, 1595.f}, 8.f, 1);
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
