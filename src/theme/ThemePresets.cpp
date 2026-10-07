#include "ThemePresets.hpp"

#include <cstring>

namespace leviathan {
namespace theme {
namespace {

ThemeSnapshot makeTheme(
	ThemeColor input,
	ThemeColor output,
	ThemeColor text,
	ThemeColor background,
	float textureAmount) {
	ThemeSnapshot snapshot = canonicalDefault();
	snapshot.colors.input = input;
	snapshot.colors.output = output;
	snapshot.colors.textInput = snapshot.colors.textOutput = text;
	snapshot.colors.background = background;
	snapshot.colors.backgroundEnabled = true;
	snapshot.surface.textureAmount = textureAmount;
	return snapshot;
}

const FactoryPreset kFactoryPresets[] = {
	{"factory:leviathan", "Leviathan", makeTheme(
		ThemeColor(0x57, 0x40, 0xbf), ThemeColor(0x1c, 0xcc, 0xd9),
		ThemeColor(0xff, 0xff, 0xff), ThemeColor(0x00, 0x00, 0x00), 1.f)},
	{"factory:abyssal", "Abyssal", makeTheme(
		ThemeColor(0x3f, 0x4c, 0x9a), ThemeColor(0xd6, 0x59, 0x8e),
		ThemeColor(0xff, 0xff, 0xff), ThemeColor(0x05, 0x0a, 0x18), 1.33f)},
	{"factory:monochrome", "Mono", makeTheme(
		ThemeColor(0xba, 0xba, 0xba), ThemeColor(0x32, 0x32, 0x32),
		ThemeColor(0xff, 0xff, 0xff), ThemeColor(0x08, 0x08, 0x08), 0.50f)},
	{"factory:ultraviolet", "Ultraviolet", makeTheme(
		ThemeColor(0xa4, 0x4d, 0xff), ThemeColor(0x35, 0xd8, 0xff),
		ThemeColor(0xff, 0xff, 0xff), ThemeColor(0x10, 0x05, 0x1c), 1.20f)},
	{"factory:all-hallows", "All Hallows", makeTheme(
		ThemeColor(0xf0, 0x78, 0x18), ThemeColor(0x7e, 0x3f, 0xc4),
		ThemeColor(0xff, 0xf0, 0xd2), ThemeColor(0x10, 0x0b, 0x16), 1.25f)}
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
