#pragma once

#include "ThemeTypes.hpp"

#include <cstddef>

namespace leviathan {
namespace theme {

struct FactoryPreset {
	const char* id;
	const char* name;
	ThemeSnapshot snapshot;
};

const FactoryPreset* factoryPresets(std::size_t* count = nullptr);
const FactoryPreset* findFactoryPreset(const char* id);

} // namespace theme
} // namespace leviathan
