#pragma once

#include "IrisWavetable.hpp"

#include <string>

namespace iris {

bool importImageFileToSourceField(const std::string& path, SourceField* out, std::string* error);
bool importImageFile(const std::string& path, const ConversionSettings& settings,
                     ImageWavetable* out, std::string* error);
bool saveSourceField(const std::string& path, const SourceField& source, std::string* error);
bool loadSourceField(const std::string& path, SourceField* out, std::string* error);
bool saveBinaryTable(const std::string& path, const ImageWavetable& table, std::string* error);
bool loadBinaryTable(const std::string& path, ImageWavetable* out, std::string* error);

} // namespace iris
