#pragma once
#include <algorithm>
#include <cstddef>
#include <cmath>
#include <vector>

namespace wave_preview {

template <typename PointType, typename Func>
inline void simplifyPath(const PointType* points, size_t count, size_t stride, float tolerance, Func&& emit) {
	if (count == 0) {
		return;
	}
	stride = std::max(stride, size_t(1));
	if (count <= 1) {
		emit(points[0], true);
		return;
	}

	// Use stack memory for typical point counts (<= 512) to avoid heap allocation
	// during draw calls. Fall back to std::vector for arbitrarily large counts.
	size_t stackIndices[512];
	size_t* indices = stackIndices;
	std::vector<size_t> heapIndices;
	if (count > 512) {
		heapIndices.resize(count);
		indices = heapIndices.data();
	}

	size_t numIndices = 0;
	indices[numIndices++] = 0;
	for (size_t i = stride; i < count; i += stride) {
		indices[numIndices++] = i;
	}
	if (indices[numIndices - 1] != count - 1) {
		indices[numIndices++] = count - 1;
	}

	emit(points[indices[0]], true);
	if (numIndices <= 2) {
		if (numIndices == 2) {
			emit(points[indices[1]], false);
		}
		return;
	}

	size_t anchor = 0; // index in the indices array

	for (size_t i = 1; i < numIndices - 1; ++i) {
		const size_t idxAnchor = indices[anchor];
		const size_t idxCurr = indices[i];
		const size_t idxNext = indices[i + 1];

		const float dySegment = points[idxCurr].y - points[idxAnchor].y;
		const float dyNext = points[idxNext].y - points[idxCurr].y;
		const float n = float(i - anchor);

		if (std::abs(n * dyNext - dySegment) > n * tolerance) {
			emit(points[idxCurr], false);
			anchor = i;
		}
	}

	emit(points[indices[numIndices - 1]], false);
}

template <typename PointType>
inline size_t compactNearlyCollinear(PointType* points, size_t count, float tolerance) {
	if (count <= 2 || tolerance <= 0.f) {
		return count;
	}

	const float toleranceSq = tolerance * tolerance;
	size_t write = 1;

	for (size_t i = 1; i < count - 1; ++i) {
		const PointType& prev = points[write - 1];
		const PointType& curr = points[i];
		const PointType& next = points[i + 1];

		const float dx = next.x - prev.x;
		const float dy = next.y - prev.y;
		const float lenSq = dx * dx + dy * dy;
		if (lenSq <= 1e-12f) {
			points[write++] = curr;
			continue;
		}

		const float inX = curr.x - prev.x;
		const float inY = curr.y - prev.y;
		const float outX = next.x - curr.x;
		const float outY = next.y - curr.y;
		if (inX * outX + inY * outY < 0.f) {
			points[write++] = curr;
			continue;
		}

		const float cross = inX * dy - inY * dx;
		if (cross * cross > toleranceSq * lenSq) {
			points[write++] = curr;
		}
	}

	points[write++] = points[count - 1];
	return write;
}

} // namespace wave_preview
