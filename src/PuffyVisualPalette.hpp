#pragma once

#include "plugin.hpp"

namespace puffy_visual {

inline NVGcolor characterTint(int character) {
	static const NVGcolor colors[puffy::kCharacterCount] = {
		nvgRGB(167, 220, 121),
		nvgRGB(255, 212, 77),
		nvgRGB(255, 138, 128),
		nvgRGB(105, 181, 255),
		nvgRGB(185, 133, 255),
		nvgRGB(255, 255, 255)
	};
	return colors[clamp(character, 0, puffy::kCharacterCount - 1)];
}

inline NVGcolor weightedCharacterTint(const float* weights) {
	float red = 0.f;
	float green = 0.f;
	float blue = 0.f;
	float alpha = 0.f;
	float total = 0.f;
	for (int i = 0; i < puffy::kCharacterCount; ++i) {
		const float weight = weights
			? clamp(weights[i], 0.f, 1.f)
			: (i == 0 ? 1.f : 0.f);
		const NVGcolor color = characterTint(i);
		red += color.r * weight;
		green += color.g * weight;
		blue += color.b * weight;
		alpha += color.a * weight;
		total += weight;
	}
	if (total <= 1e-6f) {
		return characterTint(0);
	}
	const float inverseTotal = 1.f / total;
	return nvgRGBAf(
		red * inverseTotal,
		green * inverseTotal,
		blue * inverseTotal,
		alpha * inverseTotal);
}

} // namespace puffy_visual
