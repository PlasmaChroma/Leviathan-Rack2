#include "ThemePresets.hpp"

#include <cstring>

namespace leviathan {
namespace theme {
namespace {

ThemeSnapshot makeTheme(
	ThemeColor input,
	ThemeColor output,
	ThemeColor accent,
	float textureAmount) {
	ThemeSnapshot snapshot = canonicalDefault();
	snapshot.colors.input = input;
	snapshot.colors.output = output;
	snapshot.colors.accent = accent;
	snapshot.surface.textureAmount = textureAmount;
	return snapshot;
}

const FactoryPreset kFactoryPresets[] = {
	{"factory:leviathan", "Leviathan", canonicalDefault()},
	{"factory:abyssal", "Abyssal", makeTheme(
		ThemeColor(0x3f, 0x4c, 0x9a), ThemeColor(0x16, 0x7d, 0x8c),
		ThemeColor(0x2a, 0x33, 0x5f), 1.35f)},
	{"factory:monochrome", "Mono", makeTheme(
		ThemeColor(0xa7, 0xa9, 0xb0), ThemeColor(0xe1, 0xe3, 0xe8),
		ThemeColor(0x67, 0x6a, 0x73), 0.35f)},
	{"factory:ultraviolet", "Ultraviolet", makeTheme(
		ThemeColor(0xa4, 0x4d, 0xff), ThemeColor(0x35, 0xd8, 0xff),
		ThemeColor(0xff, 0x4d, 0xff), 1.20f)}
};

} // namespace

const FactoryPreset* factoryPresets(std::size_t* count) {
	if (count) *count = sizeof(kFactoryPresets) / sizeof(kFactoryPresets[0]);
	return kFactoryPresets;
}

const FactoryPreset* findFactoryPreset(const char* id) {
	if (!id) return nullptr;
	std::size_t count = 0u;
	const FactoryPreset* presets = factoryPresets(&count);
	for (std::size_t i = 0u; i < count; ++i) {
		if (std::strcmp(presets[i].id, id) == 0) return &presets[i];
	}
	return nullptr;
}

} // namespace theme
} // namespace leviathan
