#pragma once

#include "plugin.hpp"
#include <algorithm>
#include <array>

struct WavePreviewTracerStyle {
	NVGcolor color = nvgRGBA(255, 190, 80, 255);
	float lineWidth = 1.15f;
	float fadeSec = 0.333f;
	float minCaptureIntervalSec = 1.f / 24.f;
	float maxAlpha = 118.f;
	int drawStride = 2;
};

template <size_t PointCount, size_t FrameCount>
struct WavePreviewTracer {
	static_assert(PointCount > 0, "WavePreviewTracer requires at least one point");
	static_assert(FrameCount > 0, "WavePreviewTracer requires at least one frame");

	struct Frame {
		std::array<Vec, PointCount> points {};
		double birthSec = 0.0;
		bool active = false;
	};

	std::array<Frame, FrameCount> frames {};
	int nextFrame = 0;
	double lastCaptureSec = -1.0;

	void capture(const std::array<Vec, PointCount>& points, double nowSec, float minCaptureIntervalSec) {
		if (lastCaptureSec > 0.0 && (nowSec - lastCaptureSec) < minCaptureIntervalSec) {
			return;
		}
		Frame& frame = frames[nextFrame];
		frame.points = points;
		frame.birthSec = nowSec;
		frame.active = true;
		nextFrame = (nextFrame + 1) % int(FrameCount);
		lastCaptureSec = nowSec;
	}

	void expire(double nowSec, float fadeSec) {
		for (Frame& frame : frames) {
			if (frame.active && (nowSec - frame.birthSec) >= fadeSec) {
				frame.active = false;
			}
		}
	}

	void clear() {
		for (Frame& frame : frames) {
			frame.active = false;
		}
		nextFrame = 0;
		lastCaptureSec = -1.0;
	}

	void draw(NVGcontext* vg, double nowSec, const WavePreviewTracerStyle& style) const {
		const int stride = std::max(style.drawStride, 1);
		const float fadeSec = std::max(style.fadeSec, 1e-6f);
		for (const Frame& frame : frames) {
			if (!frame.active) {
				continue;
			}
			const float age = float(nowSec - frame.birthSec);
			if (age < 0.f || age >= fadeSec) {
				continue;
			}
			const float fade = 1.f - age / fadeSec;
			const int alpha = clamp(int(style.maxAlpha * fade), 0, 255);
			if (alpha <= 0) {
				continue;
			}
			NVGcolor color = style.color;
			color.a *= float(alpha) / 255.f;
			nvgBeginPath(vg);
			nvgMoveTo(vg, frame.points[0].x, frame.points[0].y);
			for (size_t i = size_t(stride); i < PointCount; i += size_t(stride)) {
				nvgLineTo(vg, frame.points[i].x, frame.points[i].y);
			}
			if ((PointCount - 1) % size_t(stride) != 0) {
				nvgLineTo(vg, frame.points[PointCount - 1].x, frame.points[PointCount - 1].y);
			}
			nvgStrokeColor(vg, color);
			nvgStrokeWidth(vg, style.lineWidth);
			nvgLineCap(vg, NVG_BUTT);
			nvgLineJoin(vg, NVG_ROUND);
			nvgStroke(vg);
		}
	}
};
