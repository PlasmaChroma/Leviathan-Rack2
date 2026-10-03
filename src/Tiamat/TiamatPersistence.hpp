#pragma once
#include "TiamatRuntime.hpp"
#include <jansson.h>
#include <string>
namespace tiamat {
bool parseSeed(const std::string&, std::uint64_t&) noexcept;
json_t* presetToJson(const Preset&);
Preset presetFromJson(json_t*);
}
