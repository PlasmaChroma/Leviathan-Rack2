#include "PanelSvgUtils.hpp"

#include <fstream>
#include <regex>
#include <sstream>
#include <string>

namespace {

bool loadSvgText(const std::string& svgPath, std::string* outText) {
	if (!outText) {
		return false;
	}
	std::ifstream svgFile(svgPath.c_str());
	if (!svgFile.good()) {
		return false;
	}
	std::ostringstream svgBuffer;
	svgBuffer << svgFile.rdbuf();
	*outText = svgBuffer.str();
	return true;
}

std::string escapeRegexLiteral(const std::string& text) {
	return std::regex_replace(text, std::regex(R"([.^$|()\\[\]{}*+?])"), R"(\$&)");
}

bool parseAttrScaled(const std::string& tag, const char* attr, float unitScale, float* outValue) {
	if (!attr || !outValue) {
		return false;
	}
	const std::regex attrRegex(
		std::string("\\b") + attr + "\\s*=\\s*\"([^\"]+)\"",
		std::regex::icase
	);
	std::smatch attrMatch;
	if (!std::regex_search(tag, attrMatch, attrRegex)) {
		return false;
	}
	*outValue = std::stof(attrMatch.str(1)) * unitScale;
	return true;
}

} // namespace

namespace panel_svg {

bool loadRectFromSvgMm(const std::string& svgPath, const std::string& rectId, math::Rect* outRect) {
	if (!outRect) {
		return false;
	}

	std::string svgText;
	if (!loadSvgText(svgPath, &svgText)) {
		return false;
	}

	const std::string escapedId = escapeRegexLiteral(rectId);
	const std::regex rectRegex("<rect\\b[^>]*\\bid\\s*=\\s*\"" + escapedId + "\"[^>]*>", std::regex::icase);
	std::smatch rectMatch;
	if (!std::regex_search(svgText, rectMatch, rectRegex)) {
		return false;
	}
	const std::string rectTag = rectMatch.str(0);

	float xMm = 0.f;
	float yMm = 0.f;
	float wMm = 0.f;
	float hMm = 0.f;
	if (!parseAttrScaled(rectTag, "x", 0.01f, &xMm)
		|| !parseAttrScaled(rectTag, "y", 0.01f, &yMm)
		|| !parseAttrScaled(rectTag, "width", 0.01f, &wMm)
		|| !parseAttrScaled(rectTag, "height", 0.01f, &hMm)) {
		return false;
	}

	outRect->pos = Vec(xMm, yMm);
	outRect->size = Vec(wMm, hMm);
	return true;
}

bool loadCircleFromSvg(
	const std::string& svgPath,
	const std::string& circleId,
	Vec* outCenter,
	float* outRadius,
	float unitScale
) {
	if (!outCenter && !outRadius) {
		return false;
	}

	std::string svgText;
	if (!loadSvgText(svgPath, &svgText)) {
		return false;
	}

	const std::string escapedId = escapeRegexLiteral(circleId);
	const std::regex circleRegex(
		"<circle\\b[^>]*\\bid\\s*=\\s*\"" + escapedId + "\"[^>]*/?>",
		std::regex::icase
	);
	std::smatch circleMatch;
	if (!std::regex_search(svgText, circleMatch, circleRegex)) {
		return false;
	}
	const std::string circleTag = circleMatch.str(0);

	float cx = 0.f;
	float cy = 0.f;
	float radius = 0.f;
	if (!parseAttrScaled(circleTag, "cx", unitScale, &cx)
		|| !parseAttrScaled(circleTag, "cy", unitScale, &cy)
		|| !parseAttrScaled(circleTag, "r", unitScale, &radius)) {
		return false;
	}
	if (outCenter) {
		*outCenter = Vec(cx, cy);
	}
	if (outRadius) {
		*outRadius = radius;
	}
	return true;
}

bool loadPointFromSvgMm(const std::string& svgPath, const std::string& elementId, Vec* outPointMm) {
	if (!outPointMm) {
		return false;
	}

	Vec centerMm;
	if (loadCircleFromSvg(svgPath, elementId, &centerMm, nullptr, 0.01f)) {
		*outPointMm = centerMm;
		return true;
	}

	std::string svgText;
	if (!loadSvgText(svgPath, &svgText)) {
		return false;
	}

	const std::string escapedId = escapeRegexLiteral(elementId);
	const std::regex elementRegex(
		"<(ellipse|circle|rect)\\b[^>]*\\bid\\s*=\\s*\"" + escapedId + "\"[^>]*>",
		std::regex::icase
	);
	std::smatch elementMatch;
	if (!std::regex_search(svgText, elementMatch, elementRegex)) {
		return false;
	}
	const std::string elementTag = elementMatch.str(0);

	float cxMm = 0.f;
	float cyMm = 0.f;
	if (parseAttrScaled(elementTag, "cx", 0.01f, &cxMm) && parseAttrScaled(elementTag, "cy", 0.01f, &cyMm)) {
		*outPointMm = Vec(cxMm, cyMm);
		return true;
	}

	float xMm = 0.f;
	float yMm = 0.f;
	float wMm = 0.f;
	float hMm = 0.f;
	if (parseAttrScaled(elementTag, "x", 0.01f, &xMm)
		&& parseAttrScaled(elementTag, "y", 0.01f, &yMm)
		&& parseAttrScaled(elementTag, "width", 0.01f, &wMm)
		&& parseAttrScaled(elementTag, "height", 0.01f, &hMm)) {
		*outPointMm = Vec(xMm + 0.5f * wMm, yMm + 0.5f * hMm);
		return true;
	}

	return false;
}

} // namespace panel_svg
