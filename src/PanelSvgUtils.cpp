#include "PanelSvgUtils.hpp"
#include "PanelAnchorAtlas.hpp"

#include <cctype>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

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
		std::string("(^|[\\s<])") + escapeRegexLiteral(attr) + "\\s*=\\s*\"([^\"]+)\"",
		std::regex::icase
	);
	std::smatch attrMatch;
	if (!std::regex_search(tag, attrMatch, attrRegex)) {
		return false;
	}
	*outValue = std::stof(attrMatch.str(2)) * unitScale;
	return true;
}

bool parseAttrString(const std::string& tag, const char* attr, std::string* outValue) {
	if (!attr || !outValue) {
		return false;
	}
	const std::regex attrRegex(
		std::string("(^|[\\s<])") + escapeRegexLiteral(attr) + "\\s*=\\s*\"([^\"]+)\"",
		std::regex::icase
	);
	std::smatch attrMatch;
	if (!std::regex_search(tag, attrMatch, attrRegex)) {
		return false;
	}
	*outValue = attrMatch.str(2);
	return true;
}

bool parseStyleValue(const std::string& tag, const std::string& key, std::string* outValue) {
	if (!outValue) {
		return false;
	}
	std::string style;
	if (!parseAttrString(tag, "style", &style)) {
		return false;
	}
	const std::string needle = key + ":";
	size_t pos = style.find(needle);
	if (pos == std::string::npos) {
		return false;
	}
	pos += needle.size();
	size_t end = style.find(';', pos);
	std::string value = style.substr(pos, end == std::string::npos ? std::string::npos : end - pos);
	while (!value.empty() && std::isspace(static_cast<unsigned char>(value.front()))) {
		value.erase(value.begin());
	}
	while (!value.empty() && std::isspace(static_cast<unsigned char>(value.back()))) {
		value.pop_back();
	}
	*outValue = value;
	return !outValue->empty();
}

bool parseHexColor(const std::string& value, NVGcolor* outColor) {
	if (!outColor || value.size() < 7u || value[0] != '#') {
		return false;
	}
	std::string hex = value.substr(1, 6);
	for (char c : hex) {
		if (!std::isxdigit(static_cast<unsigned char>(c))) {
			return false;
		}
	}
	unsigned long rgb = std::strtoul(hex.c_str(), nullptr, 16);
	*outColor = nvgRGB(int((rgb >> 16) & 0xffu), int((rgb >> 8) & 0xffu), int(rgb & 0xffu));
	return true;
}

bool parseUrlId(const std::string& value, std::string* outId) {
	if (!outId) {
		return false;
	}
	const std::regex urlRegex("url\\s*\\(\\s*#([^\\)\\s]+)\\s*\\)", std::regex::icase);
	std::smatch match;
	if (!std::regex_search(value, match, urlRegex)) {
		return false;
	}
	*outId = match.str(1);
	return !outId->empty();
}

struct GradientResolvedColors {
	bool hasStartColor = false;
	NVGcolor startColor {};
	bool hasEndColor = false;
	NVGcolor endColor {};
};

bool gradientFirstStopColor(const std::string& svgText, const std::string& gradientId, NVGcolor* outColor, int depth = 0) {
	if (!outColor || gradientId.empty() || depth > 4) {
		return false;
	}
	const std::string escapedId = escapeRegexLiteral(gradientId);
	const std::regex selfClosingGradientRegex("<linearGradient\\b[^>]*\\bid\\s*=\\s*\"" + escapedId + "\"[^>]*/>", std::regex::icase);
	std::smatch selfClosingMatch;
	if (std::regex_search(svgText, selfClosingMatch, selfClosingGradientRegex)) {
		const std::string gradientTag = selfClosingMatch.str(0);
		std::string href;
		if ((parseAttrString(gradientTag, "xlink:href", &href) || parseAttrString(gradientTag, "href", &href))
			&& !href.empty() && href[0] == '#') {
			return gradientFirstStopColor(svgText, href.substr(1), outColor, depth + 1);
		}
	}
	const std::regex gradientRegex("<linearGradient\\b[^>]*\\bid\\s*=\\s*\"" + escapedId + "\"[^>]*>([\\s\\S]*?)</linearGradient>", std::regex::icase);
	std::smatch gradientMatch;
	if (!std::regex_search(svgText, gradientMatch, gradientRegex)) {
		return false;
	}
	const std::string gradientTagAndBody = gradientMatch.str(0);
	const std::string gradientBody = gradientMatch.str(1);

	std::string href;
	if (parseAttrString(gradientTagAndBody, "xlink:href", &href) || parseAttrString(gradientTagAndBody, "href", &href)) {
		if (!href.empty() && href[0] == '#') {
			NVGcolor inheritedColor;
			if (gradientFirstStopColor(svgText, href.substr(1), &inheritedColor, depth + 1)) {
				*outColor = inheritedColor;
				return true;
			}
		}
	}

	const std::regex stopRegex("<stop\\b[^>]*>", std::regex::icase);
	auto stopBegin = std::sregex_iterator(gradientBody.begin(), gradientBody.end(), stopRegex);
	auto stopEnd = std::sregex_iterator();
	for (auto it = stopBegin; it != stopEnd; ++it) {
		const std::string stopTag = it->str(0);
		std::string offset;
		if (parseAttrString(stopTag, "offset", &offset) && offset != "0" && offset != "0%") {
			continue;
		}
		std::string colorText;
		if ((parseStyleValue(stopTag, "stop-color", &colorText) || parseAttrString(stopTag, "stop-color", &colorText))
			&& parseHexColor(colorText, outColor)) {
			return true;
		}
	}
	return false;
}

bool gradientLastStopColor(const std::string& svgText, const std::string& gradientId, NVGcolor* outColor, int depth = 0) {
	if (!outColor || gradientId.empty() || depth > 4) {
		return false;
	}
	const std::string escapedId = escapeRegexLiteral(gradientId);
	const std::regex selfClosingGradientRegex("<linearGradient\\b[^>]*\\bid\\s*=\\s*\"" + escapedId + "\"[^>]*/>", std::regex::icase);
	std::smatch selfClosingMatch;
	if (std::regex_search(svgText, selfClosingMatch, selfClosingGradientRegex)) {
		const std::string gradientTag = selfClosingMatch.str(0);
		std::string href;
		if ((parseAttrString(gradientTag, "xlink:href", &href) || parseAttrString(gradientTag, "href", &href))
			&& !href.empty() && href[0] == '#') {
			return gradientLastStopColor(svgText, href.substr(1), outColor, depth + 1);
		}
	}
	const std::regex gradientRegex("<linearGradient\\b[^>]*\\bid\\s*=\\s*\"" + escapedId + "\"[^>]*>([\\s\\S]*?)</linearGradient>", std::regex::icase);
	std::smatch gradientMatch;
	if (!std::regex_search(svgText, gradientMatch, gradientRegex)) {
		return false;
	}
	const std::string gradientTagAndBody = gradientMatch.str(0);
	const std::string gradientBody = gradientMatch.str(1);

	bool hasLastColor = false;
	NVGcolor lastColor {};
	const std::regex stopRegex("<stop\\b[^>]*>", std::regex::icase);
	auto stopBegin = std::sregex_iterator(gradientBody.begin(), gradientBody.end(), stopRegex);
	auto stopEnd = std::sregex_iterator();
	for (auto it = stopBegin; it != stopEnd; ++it) {
		const std::string stopTag = it->str(0);
		std::string colorText;
		if ((parseStyleValue(stopTag, "stop-color", &colorText) || parseAttrString(stopTag, "stop-color", &colorText))
			&& parseHexColor(colorText, &lastColor)) {
			hasLastColor = true;
		}
	}
	if (hasLastColor) {
		*outColor = lastColor;
		return true;
	}

	std::string href;
	if (parseAttrString(gradientTagAndBody, "xlink:href", &href) || parseAttrString(gradientTagAndBody, "href", &href)) {
		if (!href.empty() && href[0] == '#') {
			return gradientLastStopColor(svgText, href.substr(1), outColor, depth + 1);
		}
	}
	return false;
}

GradientResolvedColors resolveGradientColors(
	const std::string& svgText,
	const std::string& gradientId,
	std::map<std::string, GradientResolvedColors>* cache
) {
	if (cache) {
		auto it = cache->find(gradientId);
		if (it != cache->end()) {
			return it->second;
		}
	}
	GradientResolvedColors colors;
	colors.hasStartColor = gradientFirstStopColor(svgText, gradientId, &colors.startColor);
	colors.hasEndColor = gradientLastStopColor(svgText, gradientId, &colors.endColor);
	if (cache) {
		(*cache)[gradientId] = colors;
	}
	return colors;
}

void resolveRectFillColors(
	const std::string& svgText,
	const std::string& rectTag,
	std::map<std::string, GradientResolvedColors>* gradientCache,
	panel_svg::SvgRectMatch* match
) {
	if (!match) {
		return;
	}
	std::string fill;
	if (!parseStyleValue(rectTag, "fill", &fill)) {
		parseAttrString(rectTag, "fill", &fill);
	}
	if (fill.empty()) {
		return;
	}
	if (parseHexColor(fill, &match->fillColor)) {
		match->hasFillColor = true;
		return;
	}
	std::string gradientId;
	if (parseUrlId(fill, &gradientId)) {
		const GradientResolvedColors colors = resolveGradientColors(svgText, gradientId, gradientCache);
		match->hasFillColor = colors.hasStartColor;
		match->fillColor = colors.startColor;
		match->hasFillGradientEndColor = colors.hasEndColor;
		match->fillGradientEndColor = colors.endColor;
	}
}

void resolvePathFillColor(
	const std::string& svgText,
	const std::string& pathTag,
	std::map<std::string, GradientResolvedColors>* gradientCache,
	panel_svg::SvgPathMatch* match
) {
	if (!match) {
		return;
	}
	std::string fill;
	if (!parseStyleValue(pathTag, "fill", &fill)) {
		parseAttrString(pathTag, "fill", &fill);
	}
	if (fill.empty() || fill == "none") {
		return;
	}
	if (parseHexColor(fill, &match->fillColor)) {
		match->hasFillColor = true;
		return;
	}
	std::string gradientId;
	if (parseUrlId(fill, &gradientId)) {
		const GradientResolvedColors colors = resolveGradientColors(svgText, gradientId, gradientCache);
		match->hasFillColor = colors.hasStartColor;
		match->fillColor = colors.startColor;
	}
}

bool parseRectTagMm(const std::string& rectTag, float unitScale, math::Rect* outRect) {
	if (!outRect) {
		return false;
	}
	float xMm = 0.f;
	float yMm = 0.f;
	float wMm = 0.f;
	float hMm = 0.f;
	if (!parseAttrScaled(rectTag, "x", unitScale, &xMm)
		|| !parseAttrScaled(rectTag, "y", unitScale, &yMm)
		|| !parseAttrScaled(rectTag, "width", unitScale, &wMm)
		|| !parseAttrScaled(rectTag, "height", unitScale, &hMm)) {
		return false;
	}
	outRect->pos = Vec(xMm, yMm);
	outRect->size = Vec(wMm, hMm);
	return true;
}

bool parseRectCornerRadiusMm(const std::string& rectTag, float unitScale, Vec* outRadius) {
	if (!outRadius) {
		return false;
	}
	float rxMm = 0.f;
	float ryMm = 0.f;
	const bool hasRx = parseAttrScaled(rectTag, "rx", unitScale, &rxMm);
	const bool hasRy = parseAttrScaled(rectTag, "ry", unitScale, &ryMm);
	if (!hasRx && !hasRy) {
		return false;
	}
	if (!hasRx) {
		rxMm = ryMm;
	}
	if (!hasRy) {
		ryMm = rxMm;
	}
	outRadius->x = std::max(0.f, rxMm);
	outRadius->y = std::max(0.f, ryMm);
	return outRadius->x > 0.f || outRadius->y > 0.f;
}

struct SvgAffine {
	float a = 1.f;
	float b = 0.f;
	float c = 0.f;
	float d = 1.f;
	float e = 0.f;
	float f = 0.f;
};

SvgAffine multiplyAffine(const SvgAffine& lhs, const SvgAffine& rhs) {
	SvgAffine out;
	out.a = lhs.a * rhs.a + lhs.c * rhs.b;
	out.b = lhs.b * rhs.a + lhs.d * rhs.b;
	out.c = lhs.a * rhs.c + lhs.c * rhs.d;
	out.d = lhs.b * rhs.c + lhs.d * rhs.d;
	out.e = lhs.a * rhs.e + lhs.c * rhs.f + lhs.e;
	out.f = lhs.b * rhs.e + lhs.d * rhs.f + lhs.f;
	return out;
}

Vec transformPoint(const SvgAffine& transform, Vec point) {
	return Vec(
		transform.a * point.x + transform.c * point.y + transform.e,
		transform.b * point.x + transform.d * point.y + transform.f
	);
}

std::vector<float> parseNumberList(std::string text) {
	for (char& c : text) {
		if (c == ',') {
			c = ' ';
		}
	}
	std::vector<float> values;
	std::istringstream stream(text);
	float value = 0.f;
	while (stream >> value) {
		values.push_back(value);
	}
	return values;
}

SvgAffine parseTransformAttr(const std::string& transformText) {
	SvgAffine total;
	const std::regex transformRegex("(matrix|translate)\\s*\\(([^\\)]*)\\)", std::regex::icase);
	auto begin = std::sregex_iterator(transformText.begin(), transformText.end(), transformRegex);
	auto end = std::sregex_iterator();
	for (auto it = begin; it != end; ++it) {
		std::string kind = it->str(1);
		for (char& c : kind) {
			c = char(std::tolower(static_cast<unsigned char>(c)));
		}
		const std::vector<float> values = parseNumberList(it->str(2));
		SvgAffine next;
		if (kind == "matrix") {
			if (values.size() < 6u) {
				continue;
			}
			next.a = values[0];
			next.b = values[1];
			next.c = values[2];
			next.d = values[3];
			next.e = values[4] * 0.01f;
			next.f = values[5] * 0.01f;
		}
		else {
			if (values.empty()) {
				continue;
			}
			next.e = values[0] * 0.01f;
			next.f = values.size() > 1u ? values[1] * 0.01f : 0.f;
		}
		total = multiplyAffine(total, next);
	}
	return total;
}

SvgAffine transformForTag(const std::string& tag) {
	std::string transformText;
	if (!parseAttrString(tag, "transform", &transformText)) {
		return SvgAffine();
	}
	return parseTransformAttr(transformText);
}

math::Rect transformRectMm(const math::Rect& rectMm, const SvgAffine& transform) {
	const Vec p0 = transformPoint(transform, rectMm.pos);
	const Vec p1 = transformPoint(transform, rectMm.pos.plus(Vec(rectMm.size.x, 0.f)));
	const Vec p2 = transformPoint(transform, rectMm.pos.plus(Vec(0.f, rectMm.size.y)));
	const Vec p3 = transformPoint(transform, rectMm.pos.plus(rectMm.size));
	const float minX = std::min(std::min(p0.x, p1.x), std::min(p2.x, p3.x));
	const float minY = std::min(std::min(p0.y, p1.y), std::min(p2.y, p3.y));
	const float maxX = std::max(std::max(p0.x, p1.x), std::max(p2.x, p3.x));
	const float maxY = std::max(std::max(p0.y, p1.y), std::max(p2.y, p3.y));
	return math::Rect(Vec(minX, minY), Vec(maxX - minX, maxY - minY));
}

float parseSvgUserUnitToMmScale(const std::string& svgText) {
	const std::regex svgRegex("<svg\\b[^>]*>", std::regex::icase);
	std::smatch svgMatch;
	if (!std::regex_search(svgText, svgMatch, svgRegex)) {
		return 0.01f;
	}
	const std::string svgTag = svgMatch.str(0);
	std::string viewBoxText;
	std::string widthText;
	if (!parseAttrString(svgTag, "viewBox", &viewBoxText) || !parseAttrString(svgTag, "width", &widthText)) {
		return 0.01f;
	}
	const std::vector<float> viewBox = parseNumberList(viewBoxText);
	if (viewBox.size() < 4u || !(viewBox[2] > 0.f)) {
		return 0.01f;
	}
	const std::regex widthRegex("^\\s*([-+]?\\d*\\.?\\d+(?:[eE][-+]?\\d+)?)\\s*mm\\s*$", std::regex::icase);
	std::smatch widthMatch;
	if (!std::regex_search(widthText, widthMatch, widthRegex)) {
		return 0.01f;
	}
	const float widthMm = std::stof(widthMatch.str(1));
	if (!(widthMm > 0.f)) {
		return 0.01f;
	}
	return widthMm / viewBox[2];
}

Vec transformCornerRadiusMm(Vec radiusMm, const SvgAffine& transform) {
	const float xScale = std::sqrt(transform.a * transform.a + transform.b * transform.b);
	const float yScale = std::sqrt(transform.c * transform.c + transform.d * transform.d);
	return Vec(radiusMm.x * xScale, radiusMm.y * yScale);
}

struct PathToken {
	bool command = false;
	char commandValue = 0;
	float numberValue = 0.f;
};

std::vector<PathToken> tokenizePathData(const std::string& d) {
	std::vector<PathToken> tokens;
	size_t i = 0u;
	while (i < d.size()) {
		const char c = d[i];
		if (std::isspace(static_cast<unsigned char>(c)) || c == ',') {
			++i;
			continue;
		}
		if (std::isalpha(static_cast<unsigned char>(c))) {
			PathToken token;
			token.command = true;
			token.commandValue = c;
			tokens.push_back(token);
			++i;
			continue;
		}
		char* endPtr = nullptr;
		const float value = std::strtof(d.c_str() + i, &endPtr);
		if (endPtr == d.c_str() + i) {
			++i;
			continue;
		}
		PathToken token;
		token.numberValue = value;
		tokens.push_back(token);
		i = size_t(endPtr - d.c_str());
	}
	return tokens;
}

bool isPathCommandToken(const std::vector<PathToken>& tokens, size_t index) {
	return index < tokens.size() && tokens[index].command;
}

bool readPathNumber(const std::vector<PathToken>& tokens, size_t* index, float* outValue) {
	if (!index || !outValue || *index >= tokens.size() || tokens[*index].command) {
		return false;
	}
	*outValue = tokens[*index].numberValue;
	++(*index);
	return true;
}

bool readPathPoint(
	const std::vector<PathToken>& tokens,
	size_t* index,
	float unitScale,
	bool relative,
	Vec current,
	Vec* outPoint
) {
	float x = 0.f;
	float y = 0.f;
	if (!readPathNumber(tokens, index, &x) || !readPathNumber(tokens, index, &y) || !outPoint) {
		return false;
	}
	Vec point(x * unitScale, y * unitScale);
	if (relative) {
		point = current.plus(point);
	}
	*outPoint = point;
	return true;
}

void addPathBoundsPoint(bool* hasBounds, float* minX, float* minY, float* maxX, float* maxY, Vec point) {
	if (!hasBounds || !minX || !minY || !maxX || !maxY) {
		return;
	}
	if (!*hasBounds) {
		*hasBounds = true;
		*minX = *maxX = point.x;
		*minY = *maxY = point.y;
		return;
	}
	*minX = std::min(*minX, point.x);
	*minY = std::min(*minY, point.y);
	*maxX = std::max(*maxX, point.x);
	*maxY = std::max(*maxY, point.y);
}

bool parsePathTagMm(
	const std::string& pathTag,
	float unitScale,
	const SvgAffine& transform,
	std::vector<panel_svg::SvgPathCommand>* outCommands,
	math::Rect* outBounds
) {
	if (!outCommands || !outBounds) {
		return false;
	}
	outCommands->clear();
	std::string d;
	if (!parseAttrString(pathTag, "d", &d)) {
		return false;
	}
	const std::vector<PathToken> tokens = tokenizePathData(d);
	size_t index = 0u;
	char command = 0;
	Vec current;
	Vec subpathStart;
	bool hasCurrent = false;
	bool hasBounds = false;
	float minX = 0.f;
	float minY = 0.f;
	float maxX = 0.f;
	float maxY = 0.f;
	auto appendPointBounds = [&](Vec point) {
		addPathBoundsPoint(&hasBounds, &minX, &minY, &maxX, &maxY, point);
	};

	while (index < tokens.size()) {
		if (tokens[index].command) {
			command = tokens[index].commandValue;
			++index;
		}
		if (!command) {
			return false;
		}
		const bool relative = std::islower(static_cast<unsigned char>(command)) != 0;
		const char lower = char(std::tolower(static_cast<unsigned char>(command)));
		if (lower == 'z') {
			panel_svg::SvgPathCommand out;
			out.type = panel_svg::SvgPathCommand::Close;
			outCommands->push_back(out);
			current = subpathStart;
			hasCurrent = true;
			command = 0;
			continue;
		}

		bool firstInCommand = true;
		while (index < tokens.size() && !isPathCommandToken(tokens, index)) {
			if (lower == 'm') {
				Vec point;
				if (!readPathPoint(tokens, &index, unitScale, relative && hasCurrent, current, &point)) {
					return false;
				}
				point = transformPoint(transform, point);
				panel_svg::SvgPathCommand out;
				out.type = firstInCommand ? panel_svg::SvgPathCommand::MoveTo : panel_svg::SvgPathCommand::LineTo;
				out.p1 = point;
				outCommands->push_back(out);
				current = point;
				if (firstInCommand) {
					subpathStart = point;
				}
				hasCurrent = true;
				appendPointBounds(point);
			}
			else if (lower == 'l') {
				Vec point;
				if (!readPathPoint(tokens, &index, unitScale, relative, current, &point)) {
					return false;
				}
				point = transformPoint(transform, point);
				panel_svg::SvgPathCommand out;
				out.type = panel_svg::SvgPathCommand::LineTo;
				out.p1 = point;
				outCommands->push_back(out);
				current = point;
				hasCurrent = true;
				appendPointBounds(point);
			}
			else if (lower == 'h') {
				float x = 0.f;
				if (!readPathNumber(tokens, &index, &x)) {
					return false;
				}
				Vec point((relative ? current.x / unitScale : 0.f) + x, current.y / unitScale);
				point.x *= unitScale;
				point.y *= unitScale;
				point = transformPoint(transform, point);
				panel_svg::SvgPathCommand out;
				out.type = panel_svg::SvgPathCommand::LineTo;
				out.p1 = point;
				outCommands->push_back(out);
				current = point;
				hasCurrent = true;
				appendPointBounds(point);
			}
			else if (lower == 'v') {
				float y = 0.f;
				if (!readPathNumber(tokens, &index, &y)) {
					return false;
				}
				Vec point(current.x / unitScale, (relative ? current.y / unitScale : 0.f) + y);
				point.x *= unitScale;
				point.y *= unitScale;
				point = transformPoint(transform, point);
				panel_svg::SvgPathCommand out;
				out.type = panel_svg::SvgPathCommand::LineTo;
				out.p1 = point;
				outCommands->push_back(out);
				current = point;
				hasCurrent = true;
				appendPointBounds(point);
			}
			else if (lower == 'q') {
				Vec c;
				Vec point;
				if (!readPathPoint(tokens, &index, unitScale, relative, current, &c)
					|| !readPathPoint(tokens, &index, unitScale, relative, current, &point)) {
					return false;
				}
				c = transformPoint(transform, c);
				point = transformPoint(transform, point);
				panel_svg::SvgPathCommand out;
				out.type = panel_svg::SvgPathCommand::QuadTo;
				out.p1 = c;
				out.p2 = point;
				outCommands->push_back(out);
				current = point;
				hasCurrent = true;
				appendPointBounds(c);
				appendPointBounds(point);
			}
			else if (lower == 'c') {
				Vec c1;
				Vec c2;
				Vec point;
				if (!readPathPoint(tokens, &index, unitScale, relative, current, &c1)
					|| !readPathPoint(tokens, &index, unitScale, relative, current, &c2)
					|| !readPathPoint(tokens, &index, unitScale, relative, current, &point)) {
					return false;
				}
				c1 = transformPoint(transform, c1);
				c2 = transformPoint(transform, c2);
				point = transformPoint(transform, point);
				panel_svg::SvgPathCommand out;
				out.type = panel_svg::SvgPathCommand::BezierTo;
				out.p1 = c1;
				out.p2 = c2;
				out.p3 = point;
				outCommands->push_back(out);
				current = point;
				hasCurrent = true;
				appendPointBounds(c1);
				appendPointBounds(c2);
				appendPointBounds(point);
			}
			else {
				return false;
			}
			firstInCommand = false;
			if (lower == 'm') {
				command = relative ? 'l' : 'L';
			}
		}
	}
	if (!hasBounds || outCommands->empty()) {
		return false;
	}
	outBounds->pos = Vec(minX, minY);
	outBounds->size = Vec(maxX - minX, maxY - minY);
	return true;
}

} // namespace

namespace panel_svg {

bool loadRectFromSvgMm(const std::string& svgPath, const std::string& rectId, math::Rect* outRect) {
	if (!outRect) {
		return false;
	}

	PanelAnchorLookupResult anchor;
	if (lookupPanelAnchor(svgPath, rectId, &anchor) && anchor.hasRect) {
		outRect->pos = Vec(anchor.x * 0.01f, anchor.y * 0.01f);
		outRect->size = Vec(anchor.width * 0.01f, anchor.height * 0.01f);
		return true;
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

	const float unitScale = parseSvgUserUnitToMmScale(svgText);
	if (!parseRectTagMm(rectTag, unitScale, outRect)) {
		return false;
	}

	return true;
}

bool findRectsWithIdSubstringMm(const std::string& svgPath, const std::string& idSubstring, std::vector<SvgRectMatch>* outRects) {
	if (!outRects || idSubstring.empty()) {
		return false;
	}
	outRects->clear();

	std::string svgText;
	if (!loadSvgText(svgPath, &svgText)) {
		return false;
	}

	std::map<std::string, GradientResolvedColors> gradientCache;
	const float unitScale = parseSvgUserUnitToMmScale(svgText);
	std::vector<SvgAffine> transformStack;
	transformStack.push_back(SvgAffine());
	const std::regex tagRegex("</g\\s*>|<g\\b[^>]*>|<rect\\b[^>]*>", std::regex::icase);
	auto tagBegin = std::sregex_iterator(svgText.begin(), svgText.end(), tagRegex);
	auto tagEnd = std::sregex_iterator();
	for (auto it = tagBegin; it != tagEnd; ++it) {
		const std::string tag = it->str(0);
		if (tag.size() >= 3u && tag[1] == '/') {
			if (transformStack.size() > 1u) {
				transformStack.pop_back();
			}
			continue;
		}
		if (tag.size() >= 2u && (tag[1] == 'g' || tag[1] == 'G')) {
			const SvgAffine combined = multiplyAffine(transformStack.back(), transformForTag(tag));
			if (tag.size() < 2u || tag[tag.size() - 2u] != '/') {
				transformStack.push_back(combined);
			}
			continue;
		}
		const std::string rectTag = tag;
		std::string id;
		std::string label;
		parseAttrString(rectTag, "id", &id);
		parseAttrString(rectTag, "inkscape:label", &label);
		if (id.find(idSubstring) == std::string::npos && label.find(idSubstring) == std::string::npos) {
			continue;
		}
		math::Rect rect;
		if (!parseRectTagMm(rectTag, unitScale, &rect)) {
			continue;
		}
		const SvgAffine rectTransform = multiplyAffine(transformStack.back(), transformForTag(rectTag));
		rect = transformRectMm(rect, rectTransform);
		SvgRectMatch match;
		match.id = !id.empty() ? id : label;
		match.rect = rect;
		Vec cornerRadius;
		if (parseRectCornerRadiusMm(rectTag, unitScale, &cornerRadius)) {
			match.hasCornerRadius = true;
			match.cornerRadius = transformCornerRadiusMm(cornerRadius, rectTransform);
		}
		resolveRectFillColors(svgText, rectTag, &gradientCache, &match);
		outRects->push_back(match);
	}
	return !outRects->empty();
}

bool findRectsInGroupsWithIdSubstringMm(const std::string& svgPath, const std::string& groupIdSubstring, std::vector<SvgRectMatch>* outRects) {
	if (!outRects || groupIdSubstring.empty()) {
		return false;
	}
	outRects->clear();

	std::string svgText;
	if (!loadSvgText(svgPath, &svgText)) {
		return false;
	}

	std::map<std::string, GradientResolvedColors> gradientCache;
	const float unitScale = parseSvgUserUnitToMmScale(svgText);
	std::vector<SvgAffine> transformStack;
	transformStack.push_back(SvgAffine());
	std::vector<bool> groupMatchStack;
	groupMatchStack.push_back(false);

	const std::regex tagRegex("</g\\s*>|<g\\b[^>]*>|<rect\\b[^>]*>", std::regex::icase);
	auto tagBegin = std::sregex_iterator(svgText.begin(), svgText.end(), tagRegex);
	auto tagEnd = std::sregex_iterator();
	for (auto it = tagBegin; it != tagEnd; ++it) {
		const std::string tag = it->str(0);
		if (tag.size() >= 3u && tag[1] == '/') {
			if (transformStack.size() > 1u) {
				transformStack.pop_back();
			}
			if (groupMatchStack.size() > 1u) {
				groupMatchStack.pop_back();
			}
			continue;
		}
		if (tag.size() >= 2u && (tag[1] == 'g' || tag[1] == 'G')) {
			const SvgAffine combined = multiplyAffine(transformStack.back(), transformForTag(tag));
			std::string id;
			std::string label;
			parseAttrString(tag, "id", &id);
			parseAttrString(tag, "inkscape:label", &label);
			const bool thisGroupMatches = id.find(groupIdSubstring) != std::string::npos
				|| label.find(groupIdSubstring) != std::string::npos;
			if (tag.size() < 2u || tag[tag.size() - 2u] != '/') {
				transformStack.push_back(combined);
				groupMatchStack.push_back(groupMatchStack.back() || thisGroupMatches);
			}
			continue;
		}

		const std::string rectTag = tag;
		std::string id;
		std::string label;
		parseAttrString(rectTag, "id", &id);
		parseAttrString(rectTag, "inkscape:label", &label);
		const bool rectMatchesDirectly = id.find(groupIdSubstring) != std::string::npos
			|| label.find(groupIdSubstring) != std::string::npos;
		if (!groupMatchStack.back() && !rectMatchesDirectly) {
			continue;
		}
		math::Rect rect;
		if (!parseRectTagMm(rectTag, unitScale, &rect)) {
			continue;
		}
		const SvgAffine rectTransform = multiplyAffine(transformStack.back(), transformForTag(rectTag));
		rect = transformRectMm(rect, rectTransform);
		SvgRectMatch match;
		match.id = !id.empty() ? id : label;
		match.rect = rect;
		Vec cornerRadius;
		if (parseRectCornerRadiusMm(rectTag, unitScale, &cornerRadius)) {
			match.hasCornerRadius = true;
			match.cornerRadius = transformCornerRadiusMm(cornerRadius, rectTransform);
		}
		resolveRectFillColors(svgText, rectTag, &gradientCache, &match);
		outRects->push_back(match);
	}
	return !outRects->empty();
}

bool findPathsInGroupsWithIdSubstringMm(const std::string& svgPath, const std::string& groupIdSubstring, std::vector<SvgPathMatch>* outPaths) {
	if (!outPaths || groupIdSubstring.empty()) {
		return false;
	}
	outPaths->clear();

	std::string svgText;
	if (!loadSvgText(svgPath, &svgText)) {
		return false;
	}

	const float unitScale = parseSvgUserUnitToMmScale(svgText);
	std::map<std::string, GradientResolvedColors> gradientCache;
	std::vector<SvgAffine> transformStack;
	transformStack.push_back(SvgAffine());
	std::vector<bool> groupMatchStack;
	groupMatchStack.push_back(false);

	const std::regex tagRegex("</g\\s*>|<g\\b[^>]*>|<path\\b[^>]*>", std::regex::icase);
	auto tagBegin = std::sregex_iterator(svgText.begin(), svgText.end(), tagRegex);
	auto tagEnd = std::sregex_iterator();
	for (auto it = tagBegin; it != tagEnd; ++it) {
		const std::string tag = it->str(0);
		if (tag.size() >= 3u && tag[1] == '/') {
			if (transformStack.size() > 1u) {
				transformStack.pop_back();
			}
			if (groupMatchStack.size() > 1u) {
				groupMatchStack.pop_back();
			}
			continue;
		}
		if (tag.size() >= 2u && (tag[1] == 'g' || tag[1] == 'G')) {
			const SvgAffine combined = multiplyAffine(transformStack.back(), transformForTag(tag));
			std::string id;
			std::string label;
			parseAttrString(tag, "id", &id);
			parseAttrString(tag, "inkscape:label", &label);
			const bool thisGroupMatches = id.find(groupIdSubstring) != std::string::npos
				|| label.find(groupIdSubstring) != std::string::npos;
			if (tag.size() < 2u || tag[tag.size() - 2u] != '/') {
				transformStack.push_back(combined);
				groupMatchStack.push_back(groupMatchStack.back() || thisGroupMatches);
			}
			continue;
		}

		const std::string pathTag = tag;
		std::string id;
		std::string label;
		parseAttrString(pathTag, "id", &id);
		parseAttrString(pathTag, "inkscape:label", &label);
		const bool pathMatchesDirectly = id.find(groupIdSubstring) != std::string::npos
			|| label.find(groupIdSubstring) != std::string::npos;
		if (!groupMatchStack.back() && !pathMatchesDirectly) {
			continue;
		}
		SvgPathMatch match;
		match.id = !id.empty() ? id : label;
		const SvgAffine pathTransform = multiplyAffine(transformStack.back(), transformForTag(pathTag));
		if (!parsePathTagMm(pathTag, unitScale, pathTransform, &match.commands, &match.bounds)) {
			continue;
		}
		resolvePathFillColor(svgText, pathTag, &gradientCache, &match);
		outPaths->push_back(match);
	}
	return !outPaths->empty();
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

	PanelAnchorLookupResult anchor;
	if (lookupPanelAnchor(svgPath, circleId, &anchor) && anchor.hasCenter && anchor.hasRadius) {
		if (outCenter) {
			*outCenter = Vec(anchor.cx * unitScale, anchor.cy * unitScale);
		}
		if (outRadius) {
			*outRadius = anchor.radius * unitScale;
		}
		return true;
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

	PanelAnchorLookupResult anchor;
	if (lookupPanelAnchor(svgPath, elementId, &anchor) && anchor.hasCenter) {
		*outPointMm = Vec(anchor.cx * 0.01f, anchor.cy * 0.01f);
		return true;
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

const char* getAtlasStatusLabelForSvg(const std::string& svgPath) {
	switch (getPanelAnchorAtlasStatus(svgPath)) {
	case PanelAnchorAtlasStatus::Valid:
		return "valid";
	case PanelAnchorAtlasStatus::StaleOrUnreadable:
		return "stale";
	case PanelAnchorAtlasStatus::Missing:
	default:
		return "missing";
	}
}

} // namespace panel_svg
