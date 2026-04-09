#pragma once

#include "plugin.hpp"

namespace panel_svg {

bool loadRectFromSvgMm(const std::string& svgPath, const std::string& rectId, math::Rect* outRect);
bool loadPointFromSvgMm(const std::string& svgPath, const std::string& elementId, Vec* outPointMm);

} // namespace panel_svg
