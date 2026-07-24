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

struct SvgPathCommand {
	enum Type {
		MoveTo,
		LineTo,
		QuadTo,
		BezierTo,
		Close
	};

	Type type = MoveTo;
	Vec p1;
	Vec p2;
	Vec p3;
};

struct SvgPathMatch {
	std::string id;
	std::vector<SvgPathCommand> commands;
	math::Rect bounds;
	bool hasFillColor = false;
	NVGcolor fillColor {};
};

bool loadRectFromSvgMm(const std::string& svgPath, const std::string& rectId, math::Rect* outRect);
bool findRectsWithIdSubstringMm(const std::string& svgPath, const std::string& idSubstring, std::vector<SvgRectMatch>* outRects);
bool findRectsInGroupsWithIdSubstringMm(const std::string& svgPath, const std::string& groupIdSubstring, std::vector<SvgRectMatch>* outRects);
bool findPathsInGroupsWithIdSubstringMm(const std::string& svgPath, const std::string& groupIdSubstring, std::vector<SvgPathMatch>* outPaths);
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
