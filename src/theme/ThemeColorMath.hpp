#pragma once

#include "ThemeTypes.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace leviathan { namespace theme {

inline void rgbToHsv(ThemeColor color, float* hue, float* saturation, float* value) {
	const float r = color.r / 255.f;
	const float g = color.g / 255.f;
	const float b = color.b / 255.f;
	const float hi = std::max(r, std::max(g, b));
	const float lo = std::min(r, std::min(g, b));
	const float range = hi - lo;
	float h = 0.f;
	if (range > 1e-6f) {
		if (hi == r) h = std::fmod((g - b) / range, 6.f);
		else if (hi == g) h = (b - r) / range + 2.f;
		else h = (r - g) / range + 4.f;
		h /= 6.f;
		if (h < 0.f) h += 1.f;
	}
	if (hue) *hue = h;
	if (saturation) *saturation = hi > 1e-6f ? range / hi : 0.f;
	if (value) *value = hi;
}

inline ThemeColor hsvToRgb(float hue, float saturation, float value) {
	hue = hue - std::floor(hue);
	saturation = std::min(std::max(saturation, 0.f), 1.f);
	value = std::min(std::max(value, 0.f), 1.f);
	const float scaled = hue * 6.f;
	const int sector = int(std::floor(scaled));
	const float fraction = scaled - sector;
	const float p = value * (1.f - saturation);
	const float q = value * (1.f - saturation * fraction);
	const float t = value * (1.f - saturation * (1.f - fraction));
	float r = value, g = t, b = p;
	switch (sector % 6) {
		case 0: r = value; g = t; b = p; break;
		case 1: r = q; g = value; b = p; break;
		case 2: r = p; g = value; b = t; break;
		case 3: r = p; g = q; b = value; break;
		case 4: r = t; g = p; b = value; break;
		case 5: r = value; g = p; b = q; break;
	}
	return ThemeColor(
		std::uint8_t(std::round(r * 255.f)),
		std::uint8_t(std::round(g * 255.f)),
		std::uint8_t(std::round(b * 255.f)));
}

}} // namespace leviathan::theme
