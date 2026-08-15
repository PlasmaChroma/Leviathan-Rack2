#pragma once

#include "ThemeTypes.hpp"

#include <cstdint>

namespace leviathan {
namespace theme {

enum ThemeChange : std::uint32_t {
	ChangeNone = 0u,
	ChangeColors = 1u << 0,
	ChangeSurface = 1u << 1,
	ChangePresets = 1u << 2
};

struct ThemeState {
	ThemeSnapshot snapshot;
	std::uint64_t generation = 0u;
	std::uint64_t colorGeneration = 0u;
	std::uint64_t surfaceGeneration = 0u;
	std::uint64_t presetGeneration = 0u;
};

ThemeSnapshot canonicalize(ThemeSnapshot snapshot);
ThemeState read();
std::uint64_t generation();
std::uint64_t colorGeneration();
std::uint64_t surfaceGeneration();
std::uint64_t presetGeneration();
ThemeColor color(ThemeRole role);
ThemeChange setColor(ThemeRole role, ThemeColor value);
ThemeChange setTextureAmount(float amount);
ThemeChange apply(const ThemeSnapshot& snapshot);
ThemeChange resetToDefault();

// Loads an initial durable snapshot before module widgets are created. This is
// intentionally separate from apply() so startup does not masquerade as a user edit.
void initialize(const ThemeSnapshot& snapshot);

} // namespace theme
} // namespace leviathan
