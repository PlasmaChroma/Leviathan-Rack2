#pragma once

#include <array>
#include <cmath>

namespace bulkhead {
namespace geometry {

struct Vec2 {
	float x = 0.f;
	float y = 0.f;
};

struct RoomBounds {
	float left = -4.f;
	float right = 4.f;
	float bottom = -2.5f;
	float top = 2.5f;
};

enum WallId {
	WALL_LEFT = 0,
	WALL_RIGHT = 1,
	WALL_FRONT = 2,
	WALL_BACK = 3,
	WALL_COUNT = 4
};

inline float distanceMeters(const Vec2& a, const Vec2& b) {
	const float dx = b.x - a.x;
	const float dy = b.y - a.y;
	return std::sqrt(dx * dx + dy * dy);
}

Vec2 mirrorSourceAcrossWall(const RoomBounds& room, const Vec2& source, WallId wall);
std::array<Vec2, WALL_COUNT> firstOrderImageSources(const RoomBounds& room, const Vec2& source);
std::array<float, WALL_COUNT> firstOrderReflectionDistances(
	const RoomBounds& room,
	const Vec2& source,
	const Vec2& listener
);

} // namespace geometry
} // namespace bulkhead

