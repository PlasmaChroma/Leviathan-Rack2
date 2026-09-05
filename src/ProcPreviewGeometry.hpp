#pragma once

#include "plugin.hpp"
#include "WavePreviewSimplifier.hpp"
#include <array>

// Shared by the live widget and offline benchmark; ShapeModel supplies Proc DSP math.
template <typename ShapeModel>
struct ProcPreviewGeometry {
	static constexpr int POINT_COUNT = 128;
	static constexpr int PREVIEW_LUT_SIZE = 512;
	static constexpr float WAVE_LINE_WIDTH = 1.4f;
	static constexpr float WAVE_EDGE_PAD = 1.0f;
	std::array<Vec, POINT_COUNT> points {};
	struct SimplifiedPreviewPath {
		std::array<Vec, POINT_COUNT> points {};
		int count = 0;
	};
	SimplifiedPreviewPath simplifiedFullPath;
	SimplifiedPreviewPath simplifiedRisePath;
	SimplifiedPreviewPath simplifiedFallPath;
	std::array<float, PREVIEW_LUT_SIZE> cachedRiseLut {};
	std::array<float, PREVIEW_LUT_SIZE> cachedFallLut {};
	float cachedLutCurveSigned = 0.f;
	bool cachedLutsValid = false;
	bool pointsValid = false;
	int peakPointIndex = POINT_COUNT / 2;

	static void buildSegmentLut(std::array<float, PREVIEW_LUT_SIZE>& lut, float curveSigned, bool rising) {
		// Build once per preview update. Midpoint integration reduces visual artifacts at extreme curve asymmetry.
		float scale = ShapeModel::slopeWarpScale(curveSigned);
		float dp = 1.f / float(PREVIEW_LUT_SIZE - 1);
		float x = rising ? 0.f : 1.f;
		lut[0] = x;
		for (int i = 1; i < PREVIEW_LUT_SIZE; ++i) {
			float k1 = ShapeModel::slopeWarp(x, curveSigned) * scale;
			float xMid = rising ? (x + 0.5f * dp * k1) : (x - 0.5f * dp * k1);
			xMid = clamp(xMid, 0.f, 1.f);
			float k2 = ShapeModel::slopeWarp(xMid, curveSigned) * scale;
			x += rising ? (dp * k2) : (-dp * k2);
			x = clamp(x, 0.f, 1.f);
			lut[i] = x;
		}
		lut.front() = rising ? 0.f : 1.f;
		lut.back() = rising ? 1.f : 0.f;
	}

	static float sampleSegmentLut(const std::array<float, PREVIEW_LUT_SIZE>& lut, float t) {
		t = clamp(t, 0.f, 1.f);
		float idx = t * float(PREVIEW_LUT_SIZE - 1);
		int i0 = int(idx);
		int i1 = std::min(i0 + 1, PREVIEW_LUT_SIZE - 1);
		float f = idx - float(i0);
		return lut[i0] + (lut[i1] - lut[i0]) * f;
	}

	void ensureSegmentLuts(float curveSigned) {
		if (cachedLutsValid && curveSigned == cachedLutCurveSigned) {
			return;
		}
		buildSegmentLut(cachedRiseLut, curveSigned, true);
		buildSegmentLut(cachedFallLut, curveSigned, false);
		cachedLutCurveSigned = curveSigned;
		cachedLutsValid = true;
	}

	void rebuildSimplifiedPath(SimplifiedPreviewPath& destination, int start, int end) {
		destination.count = 0;
		start = clamp(start, 0, POINT_COUNT - 1);
		end = clamp(end, 0, POINT_COUNT - 1);
		const int count = end - start + 1;
		if (count < 2) {
			return;
		}
		wave_preview::simplifyPath(points.data() + start, count, 1, 0.02f,
			[&destination](const Vec& pt, bool) {
				if (destination.count < POINT_COUNT) {
					destination.points[size_t(destination.count++)] = pt;
				}
			});
	}

	void rebuildSimplifiedPaths() {
		const int peakIndex = clamp(peakPointIndex, 1, POINT_COUNT - 2);
		rebuildSimplifiedPath(simplifiedFullPath, 0, POINT_COUNT - 1);
		rebuildSimplifiedPath(simplifiedRisePath, 0, peakIndex);
		rebuildSimplifiedPath(simplifiedFallPath, peakIndex, POINT_COUNT - 1);
	}

	const SimplifiedPreviewPath* simplifiedPathForSegment(int start, int end) const {
		const int peakIndex = clamp(peakPointIndex, 1, POINT_COUNT - 2);
		if (start == 0 && end == POINT_COUNT - 1) return &simplifiedFullPath;
		if (start == 0 && end == peakIndex) return &simplifiedRisePath;
		if (start == peakIndex && end == POINT_COUNT - 1) return &simplifiedFallPath;
		return nullptr;
	}

	void rebuildPoints(Vec size, float riseTime, float fallTime, float curveSigned, bool interactiveRecent) {
		float w = std::max(size.x, 1.f);
		float h = std::max(size.y, 1.f);
		float drawPad = 0.5f * WAVE_LINE_WIDTH + WAVE_EDGE_PAD;
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
		// Reserved hook if we later render interactive-state emphasis.
		(void) interactiveRecent;
		ensureSegmentLuts(curveSigned);

		for (int i = 0; i < POINT_COUNT; ++i) {
			float xNorm = float(i) / float(POINT_COUNT - 1);
			float x = left + xNorm * drawW;
			float y = -1.f;
			if (x <= peakX) {
				float t = (x - left) / riseWidth;
				float v = sampleSegmentLut(cachedRiseLut, t);
				y = -1.f + 2.f * v;
			}
			else {
				float t = (x - peakX) / fallWidth;
				float v = sampleSegmentLut(cachedFallLut, t);
				y = -1.f + 2.f * v;
			}
			float py = top + (0.5f - 0.5f * y) * drawH;
			py = clamp(py, top, bottom);
			points[i] = Vec(x, py);
		}

			// Preserve full crest height without flattening the apex into a
			// two-point plateau when the true peak falls between sample columns.
			float peakIndexF = riseRatio * float(POINT_COUNT - 1);
			int peakIndex = std::max(1, std::min(POINT_COUNT - 2, int(std::round(peakIndexF))));
			peakPointIndex = peakIndex;
			points[peakIndex] = Vec(peakX, top);
			points.front() = Vec(left, bottom);
			points.back() = Vec(right, bottom);
		pointsValid = true;
		rebuildSimplifiedPaths();
	}

	void drawWaveSegment(NVGcontext* vg, int start, int end, NVGcolor color) {
		if (!pointsValid) {
			return;
		}
		const SimplifiedPreviewPath* path = simplifiedPathForSegment(start, end);
		if (!path || path->count < 2) {
			return;
		}
		nvgBeginPath(vg);
		nvgMoveTo(vg, path->points[0].x, path->points[0].y);
		for (int i = 1; i < path->count; ++i) {
			nvgLineTo(vg, path->points[size_t(i)].x, path->points[size_t(i)].y);
		}
		nvgStrokeColor(vg, color);
		nvgStrokeWidth(vg, WAVE_LINE_WIDTH);
		nvgLineCap(vg, NVG_BUTT);
		nvgLineJoin(vg, NVG_ROUND);
		nvgStroke(vg);
	}
};
