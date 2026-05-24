#include "BulkheadGeometry.hpp"

namespace bulkhead {
namespace geometry {

Vec2 mirrorSourceAcrossWall(const RoomBounds& room, const Vec2& source, WallId wall) {
	Vec2 image = source;
	switch (wall) {
	case WALL_LEFT:
		image.x = 2.f * room.left - source.x;
		break;
	case WALL_RIGHT:
		image.x = 2.f * room.right - source.x;
		break;
	case WALL_FRONT:
		image.y = 2.f * room.top - source.y;
		break;
	case WALL_BACK:
		image.y = 2.f * room.bottom - source.y;
		break;
	default:
		break;
	}
	return image;
}

std::array<Vec2, WALL_COUNT> firstOrderImageSources(const RoomBounds& room, const Vec2& source) {
	return {
		mirrorSourceAcrossWall(room, source, WALL_LEFT),
		mirrorSourceAcrossWall(room, source, WALL_RIGHT),
		mirrorSourceAcrossWall(room, source, WALL_FRONT),
		mirrorSourceAcrossWall(room, source, WALL_BACK)
	};
}

std::array<float, WALL_COUNT> firstOrderReflectionDistances(
	const RoomBounds& room,
	const Vec2& source,
	const Vec2& listener
) {
	const std::array<Vec2, WALL_COUNT> images = firstOrderImageSources(room, source);
	return {
		distanceMeters(listener, images[WALL_LEFT]),
		distanceMeters(listener, images[WALL_RIGHT]),
		distanceMeters(listener, images[WALL_FRONT]),
		distanceMeters(listener, images[WALL_BACK])
	};
}

} // namespace geometry
} // namespace bulkhead

