#pragma once
#include "../MathsFunction.hpp"

namespace wave_preview {
// Integral Flux's production geometry, including its exact crest placement.
template <typename Point, std::size_t POINT_COUNT, std::size_t LUT_SIZE>
int buildMathsFunctionPoints(std::array<Point, POINT_COUNT>& points, float width, float height,
    float riseTime, float fallTime, const std::array<float, LUT_SIZE>& cachedRiseLut,
    const std::array<float, LUT_SIZE>& cachedFallLut) {
    using maths_function::clamp;

	float w = std::max(width, 1.f);
	float h = std::max(height, 1.f);
	float drawPad = 0.5f * 1.4f + 1.0f;
	float left = drawPad;
	float top = drawPad;
	float right = std::max(left + 1.f, w - drawPad);
	float bottom = std::max(top + 1.f, h - drawPad);
	float drawW = right - left;
	float drawH = bottom - top;
	// The preview always shows exactly one full rise+fall cycle across widget width.
	float totalTime = std::max(riseTime + fallTime, 1e-6f);
	float riseRatio = riseTime / totalTime;
	float peakX = left + riseRatio * drawW;
	float riseWidth = std::max(peakX - left, 1e-4f);
	float fallWidth = std::max(right - peakX, 1e-4f);

	for (int i = 0; i < int(POINT_COUNT); ++i) {
		float xNorm = float(i) / float(POINT_COUNT - 1);
		float x = left + xNorm * drawW;
		float y = -1.f;
		if (x <= peakX) {
			float t = (x - left) / riseWidth;
			float v = maths_function::sampleSegmentLut(cachedRiseLut, t);
			y = -1.f + 2.f * v;
		}
		else {
			float t = (x - peakX) / fallWidth;
			float v = maths_function::sampleSegmentLut(cachedFallLut, t);
			y = -1.f + 2.f * v;
		}
		float py = top + (0.5f - 0.5f * y) * drawH;
		py = clamp(py, top, bottom);
		points[i] = Point(x, py);
	}

	// Preserve full crest height without flattening the apex into a
	// two-point plateau when the true peak falls between sample columns.
	float peakIndexF = riseRatio * float(POINT_COUNT - 1);
	int peakIndex = std::max(1, std::min(int(POINT_COUNT) - 2, int(std::round(peakIndexF))));

	points[peakIndex] = Point(peakX, top);
	points.front() = Point(left, bottom);
	points.back() = Point(right, bottom);
    return peakIndex;
}
}
