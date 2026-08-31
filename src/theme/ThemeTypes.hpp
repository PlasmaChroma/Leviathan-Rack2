#pragma once

#include <cstdint>

namespace leviathan {
namespace theme {

constexpr std::uint32_t kThemeSchemaVersion = 1u;

enum class ThemeRole : std::uint8_t {
	None = 0,
	Input,
	Output,
	Text
};

struct ThemeColor {
	std::uint8_t r = 0u;
	std::uint8_t g = 0u;
	std::uint8_t b = 0u;

	constexpr ThemeColor(std::uint8_t red = 0u, std::uint8_t green = 0u, std::uint8_t blue = 0u)
		: r(red), g(green), b(blue) {
	}

	bool operator==(const ThemeColor& other) const {
		return r == other.r && g == other.g && b == other.b;
	}
	bool operator!=(const ThemeColor& other) const {
		return !(*this == other);
	}
};

struct ThemeColors {
	ThemeColor input;
	ThemeColor output;
	ThemeColor text;

	bool operator==(const ThemeColors& other) const {
		return input == other.input && output == other.output && text == other.text;
	}
	bool operator!=(const ThemeColors& other) const {
		return !(*this == other);
	}
};

struct ThemeSurface {
	float textureAmount = 1.f;

	bool operator==(const ThemeSurface& other) const {
		return textureAmount == other.textureAmount;
	}
	bool operator!=(const ThemeSurface& other) const {
		return !(*this == other);
	}
};

struct ThemeSnapshot {
	std::uint32_t schemaVersion = kThemeSchemaVersion;
	ThemeColors colors;
	ThemeSurface surface;

	bool operator==(const ThemeSnapshot& other) const {
		return schemaVersion == other.schemaVersion
			&& colors == other.colors
			&& surface == other.surface;
	}
	bool operator!=(const ThemeSnapshot& other) const {
		return !(*this == other);
	}
};

inline ThemeSnapshot canonicalDefault() {
	ThemeSnapshot snapshot;
	snapshot.colors.input = {0x57, 0x40, 0xbf};
	snapshot.colors.output = {0x1c, 0xcc, 0xd9};
	snapshot.colors.text = {0xff, 0xff, 0xff};
	snapshot.surface.textureAmount = 1.f;
	return snapshot;
}

} // namespace theme
} // namespace leviathan
