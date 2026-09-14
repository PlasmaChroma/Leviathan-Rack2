#pragma once
#include "../plugin.hpp"
namespace eclipse2_track {
inline void draw(const Widget::DrawArgs& args, Vec center, float radiusPx, float largeRadiusPx, float minAngle, float maxAngle) {
	// 1. Draw Recessed Dark Track Ring
	const float startArcAngle = minAngle - 0.5f * M_PI;
	const float endArcAngle = maxAngle - 0.5f * M_PI;

	// Subtle outer drop shadow for track depth
	nvgBeginPath(args.vg);
	nvgArc(args.vg, center.x, center.y, radiusPx, startArcAngle, endArcAngle, NVG_CW);
	nvgStrokeColor(args.vg, nvgRGBA(3, 2, 2, 96));
	nvgStrokeWidth(args.vg, largeRadiusPx * 4.4f);
	nvgLineCap(args.vg, NVG_ROUND);
	nvgStroke(args.vg);

	// Sharp black border stroke around the track (thickened and slightly transparent)
	nvgBeginPath(args.vg);
	nvgArc(args.vg, center.x, center.y, radiusPx, startArcAngle, endArcAngle, NVG_CW);
	nvgStrokeColor(args.vg, nvgRGBA(0, 0, 0, 245));
	nvgStrokeWidth(args.vg, largeRadiusPx * 3.4f);
	nvgLineCap(args.vg, NVG_ROUND);
	nvgStroke(args.vg);

	// Core dark track channel
	nvgBeginPath(args.vg);
	nvgArc(args.vg, center.x, center.y, radiusPx, startArcAngle, endArcAngle, NVG_CW);
	nvgStrokeColor(args.vg, nvgRGBA(14, 12, 11, 230));
	nvgStrokeWidth(args.vg, largeRadiusPx * 2.4f);
	nvgLineCap(args.vg, NVG_ROUND);
	nvgStroke(args.vg);

	// Light specular accent inside the track (warm bronze glint)
	nvgBeginPath(args.vg);
	nvgArc(args.vg, center.x, center.y, radiusPx, startArcAngle, endArcAngle, NVG_CW);
	nvgStrokeColor(args.vg, nvgRGBA(255, 220, 150, 16));
	nvgStrokeWidth(args.vg, largeRadiusPx * 1.8f);
	nvgLineCap(args.vg, NVG_ROUND);
	nvgStroke(args.vg);

}
}
