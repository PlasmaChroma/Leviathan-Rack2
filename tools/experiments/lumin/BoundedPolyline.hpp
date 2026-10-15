#pragma once

#include <array>
#include <cmath>
#include <cstddef>

namespace leviathan {
namespace render {

// An optional second pass over an already simplified preview. Work is bounded
// by Capacity (128 in the pilot), with no heap allocation or square roots.
// Every removed input vertex is within tolerance of its replacement segment.
// Endpoints and the topmost vertex are retained, preserving the preview crest.
// This bounds centerline error, not NanoVG stroke/join rasterization error.
template <typename Point, size_t Capacity>
size_t reducePreviewPolyline(std::array<Point, Capacity>& points, size_t count,
                             float tolerance = 0.02f) {
	if (count < 3 || count > Capacity || !(tolerance > 0.f) || !std::isfinite(tolerance))
		return count;
	// Reject invalid inputs before mutating the retained path. Preview coordinates
	// are small; this guard also bounds all squared-distance intermediates.
	size_t crest = 0;
	for (size_t i = 0; i < count; ++i) {
		if (!(std::fabs(points[i].x) <= 1e6f) || !(std::fabs(points[i].y) <= 1e6f))
			return count;
		if (points[i].y < points[crest].y) crest = i;
	}
	struct Range { size_t first, last; };
	std::array<Range, Capacity> pending;
	std::array<bool, Capacity> keep{};
	keep[0] = keep[count - 1] = keep[crest] = true;
	size_t pendingCount = 0;
	if (crest > 0 && crest + 1 < count) {
		pending[pendingCount++] = {0, crest};
		pending[pendingCount++] = {crest, count - 1};
	}
	else pending[pendingCount++] = {0, count - 1};
	const float toleranceSq = tolerance * tolerance;
	while (pendingCount) {
		const Range range = pending[--pendingCount];
		if (range.last <= range.first + 1) continue;
		const Point& a = points[range.first];
		const Point& b = points[range.last];
		const float dx = b.x - a.x, dy = b.y - a.y;
		const float lengthSq = dx * dx + dy * dy;
		// Compare scaled squared distances to avoid division in the scan.
		const float weight = lengthSq > 0.f ? lengthSq : 1.f;
		float greatest = toleranceSq * weight;
		size_t split = range.first;
		for (size_t i = range.first + 1; i < range.last; ++i) {
			const float x = points[i].x - a.x, y = points[i].y - a.y;
			const float projection = x * dx + y * dy;
			float distance;
			if (projection <= 0.f || lengthSq == 0.f)
				distance = (x * x + y * y) * weight;
			else if (projection >= lengthSq) {
				const float bx = points[i].x - b.x, by = points[i].y - b.y;
				distance = (bx * bx + by * by) * weight;
			}
			else {
				const float cross = x * dy - y * dx;
				distance = cross * cross;
			}
			if (distance > greatest) { greatest = distance; split = i; }
		}
		if (split != range.first) {
			keep[split] = true;
			pending[pendingCount++] = {range.first, split};
			pending[pendingCount++] = {split, range.last};
		}
	}
	size_t written = 0;
	for (size_t i = 0; i < count; ++i)
		if (keep[i]) points[written++] = points[i];
	return written;
}

} // namespace render
} // namespace leviathan
