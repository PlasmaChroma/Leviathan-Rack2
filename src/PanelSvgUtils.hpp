#pragma once

#include "plugin.hpp"

namespace panel_svg {

bool loadRectFromSvgMm(const std::string& svgPath, const std::string& rectId, math::Rect* outRect);
bool loadPointFromSvgMm(const std::string& svgPath, const std::string& elementId, Vec* outPointMm);
bool loadCircleFromSvg(
	const std::string& svgPath,
	const std::string& circleId,
	Vec* outCenter,
	float* outRadius,
	float unitScale = 1.f
);

} // namespace panel_svg
