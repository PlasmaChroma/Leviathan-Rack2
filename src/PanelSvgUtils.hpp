#pragma once

#include "plugin.hpp"

#include <vector>

namespace panel_svg {

struct SvgRectMatch {
	std::string id;
	math::Rect rect;
	bool hasCornerRadius = false;
	Vec cornerRadius;
	bool hasFillColor = false;
	NVGcolor fillColor {};
	bool hasFillGradientEndColor = false;
	NVGcolor fillGradientEndColor {};
};

bool loadRectFromSvgMm(const std::string& svgPath, const std::string& rectId, math::Rect* outRect);
bool findRectsWithIdSubstringMm(const std::string& svgPath, const std::string& idSubstring, std::vector<SvgRectMatch>* outRects);
bool findRectsInGroupsWithIdSubstringMm(const std::string& svgPath, const std::string& groupIdSubstring, std::vector<SvgRectMatch>* outRects);
bool loadPointFromSvgMm(const std::string& svgPath, const std::string& elementId, Vec* outPointMm);
bool loadCircleFromSvg(
	const std::string& svgPath,
	const std::string& circleId,
	Vec* outCenter,
	float* outRadius,
	float unitScale = 1.f
);
const char* getAtlasStatusLabelForSvg(const std::string& svgPath);

} // namespace panel_svg
