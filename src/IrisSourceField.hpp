#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace iris {

constexpr int kCanonicalSourceWidth = 1024;
constexpr int kCanonicalSourceHeight = 256;
constexpr int kCanonicalSourceChannels = 3;
constexpr int kCanonicalSourceBitDepth = 8;

enum SourceGeneratorKind {
  SOURCE_GENERATOR_NONE = 0,
  SOURCE_GENERATOR_NAUTILOID_FRACTAL = 1,
};

struct SourceField {
  int width = 0;
  int height = 0;
  int channels = kCanonicalSourceChannels;
  int bitDepth = kCanonicalSourceBitDepth;
  std::vector<uint8_t> rgb8;
  std::string sourcePath;
  std::string sourceName;
  int originalWidth = 0;
  int originalHeight = 0;
  int originalChannels = 0;
  int generatorKind = SOURCE_GENERATOR_NONE;
  int generatorVersion = 0;
  int generatorFractalMode = 0;
  float generatorFractalZoom = 0.f;
  float generatorFractalCenterX = 0.f;
  float generatorFractalCenterY = 0.f;
  uint64_t generatorGeneration = 0u;

  bool valid() const {
    return width > 0 && height > 0 && channels == kCanonicalSourceChannels &&
           bitDepth == kCanonicalSourceBitDepth &&
           rgb8.size() == size_t(width) * size_t(height) * size_t(kCanonicalSourceChannels);
  }
};

bool buildSourceFieldFromRgba8(const uint8_t* rgba, int width, int height,
                               int originalChannels, SourceField* out,
                               std::string* error = nullptr);
void buildDisplayRgb8FromSourceField(const SourceField& source, std::vector<uint8_t>* rgb8,
                                     int* width = nullptr, int* height = nullptr);
bool saveSourceFieldQoi(const std::string& path, const SourceField& source,
                        std::string* error = nullptr);
bool loadSourceFieldQoi(const std::string& path, SourceField* out,
                        std::string* error = nullptr);

} // namespace iris
