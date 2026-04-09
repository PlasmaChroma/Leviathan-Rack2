#include "PanelSvgUtils.hpp"

#include <fstream>
#include <regex>
#include <sstream>
#include <string>

namespace panel_svg {

bool loadRectFromSvgMm(const std::string& svgPath, const std::string& rectId, math::Rect* outRect) {
	if (!outRect) {
		return false;
	}

	std::ifstream svgFile(svgPath.c_str());
	if (!svgFile.good()) {
		return false;
	}

	std::ostringstream svgBuffer;
	svgBuffer << svgFile.rdbuf();
	const std::string svgText = svgBuffer.str();

	const std::regex rectRegex("<rect\\b[^>]*\\bid\\s*=\\s*\"" + rectId + "\"[^>]*>", std::regex::icase);
	std::smatch rectMatch;
	if (!std::regex_search(svgText, rectMatch, rectRegex)) {
		return false;
	}
	const std::string rectTag = rectMatch.str(0);

	auto parseAttrMm = [&](const char* attr, float* outMm) -> bool {
		if (!outMm) {
			return false;
		}
		const std::regex attrRegex(std::string("\\b") + attr + "\\s*=\\s*\"([^\"]+)\"", std::regex::icase);
		std::smatch attrMatch;
		if (!std::regex_search(rectTag, attrMatch, attrRegex)) {
			return false;
		}
		// Inkscape panel coordinates are in 1/100 mm (viewBox-style coordinates).
		*outMm = std::stof(attrMatch.str(1)) * 0.01f;
		return true;
	};

	float xMm = 0.f;
	float yMm = 0.f;
	float wMm = 0.f;
	float hMm = 0.f;
	if (!parseAttrMm("x", &xMm) || !parseAttrMm("y", &yMm) || !parseAttrMm("width", &wMm) || !parseAttrMm("height", &hMm)) {
		return false;
	}

	outRect->pos = Vec(xMm, yMm);
	outRect->size = Vec(wMm, hMm);
	return true;
}

bool loadPointFromSvgMm(const std::string& svgPath, const std::string& elementId, Vec* outPointMm) {
	if (!outPointMm) {
		return false;
	}

	std::ifstream svgFile(svgPath.c_str());
	if (!svgFile.good()) {
		return false;
	}

	std::ostringstream svgBuffer;
	svgBuffer << svgFile.rdbuf();
	const std::string svgText = svgBuffer.str();

	const std::regex elementRegex(
		"<(ellipse|circle|rect)\\b[^>]*\\bid\\s*=\\s*\"" + elementId + "\"[^>]*>",
		std::regex::icase
	);
	std::smatch elementMatch;
	if (!std::regex_search(svgText, elementMatch, elementRegex)) {
		return false;
	}
	const std::string elementTag = elementMatch.str(0);

	auto parseAttrMm = [&](const char* attr, float* outMm) -> bool {
		if (!outMm) {
			return false;
		}
		const std::regex attrRegex(std::string("\\b") + attr + "\\s*=\\s*\"([^\"]+)\"", std::regex::icase);
		std::smatch attrMatch;
		if (!std::regex_search(elementTag, attrMatch, attrRegex)) {
			return false;
		}
		*outMm = std::stof(attrMatch.str(1)) * 0.01f;
		return true;
	};

	float cxMm = 0.f;
	float cyMm = 0.f;
	if (parseAttrMm("cx", &cxMm) && parseAttrMm("cy", &cyMm)) {
		*outPointMm = Vec(cxMm, cyMm);
		return true;
	}

	float xMm = 0.f;
	float yMm = 0.f;
	float wMm = 0.f;
	float hMm = 0.f;
	if (parseAttrMm("x", &xMm) && parseAttrMm("y", &yMm) && parseAttrMm("width", &wMm) && parseAttrMm("height", &hMm)) {
		*outPointMm = Vec(xMm + 0.5f * wMm, yMm + 0.5f * hMm);
		return true;
	}

	return false;
}

} // namespace panel_svg
