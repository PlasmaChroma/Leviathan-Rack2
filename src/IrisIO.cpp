#include "IrisIO.hpp"

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#pragma GCC diagnostic ignored "-Wmisleading-indentation"
#pragma GCC diagnostic ignored "-Wshift-negative-value"
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#pragma GCC diagnostic ignored "-Wunused-function"
#endif
#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#include <cstring>
#include <fstream>
#include <limits>
#include <memory>

namespace iris {

namespace {

struct BinaryHeader {
  char magic[8];
  uint32_t version;
  uint32_t frameSize;
  uint32_t rowCount;
  uint32_t stride;
  uint32_t sourceWidth;
  uint32_t sourceHeight;
  uint32_t sourceChannels;
};

const char kMagic[8] = {'I', 'R', 'I', 'S', 'W', 'T', '0', '1'};

bool samplesAreFinite(const ImageWavetable& table) {
  if (!table.valid()) return false;
  for (size_t i = 0; i < table.samples.size(); ++i) {
    if (!std::isfinite(table.samples[i])) return false;
  }
  return true;
}

} // namespace

bool importImageFile(const std::string& path, const ConversionSettings& settings,
                     ImageWavetable* out, std::string* error) {
  int width = 0;
  int height = 0;
  int channels = 0;
  std::unique_ptr<stbi_uc, void (*)(void*)> pixels(
    stbi_load(path.c_str(), &width, &height, &channels, 4), stbi_image_free);
  if (!pixels) {
    if (error) {
      const char* reason = stbi_failure_reason();
      *error = reason ? reason : "Image decode failed";
    }
    return false;
  }
  ImageWavetable table;
  const bool ok = buildWavetableFromRgba(pixels.get(), width, height, channels, settings, &table, error);
  if (!ok) {
    return false;
  }
  table.sourcePath = path;
  const size_t slash = path.find_last_of("/\\");
  table.sourceName = slash == std::string::npos ? path : path.substr(slash + 1u);
  *out = std::move(table);
  return true;
}

bool saveBinaryTable(const std::string& path, const ImageWavetable& table, std::string* error) {
  if (!samplesAreFinite(table)) {
    if (error) *error = "Invalid wavetable";
    return false;
  }
  std::ofstream stream(path.c_str(), std::ios::binary | std::ios::trunc);
  if (!stream) {
    if (error) *error = "Could not open table for writing";
    return false;
  }
  BinaryHeader header;
  std::memcpy(header.magic, kMagic, sizeof(kMagic));
  header.version = 1u;
  header.frameSize = uint32_t(table.frameSize);
  header.rowCount = uint32_t(table.rowCount);
  header.stride = uint32_t(table.stride);
  header.sourceWidth = uint32_t(std::max(table.sourceWidth, 0));
  header.sourceHeight = uint32_t(std::max(table.sourceHeight, 0));
  header.sourceChannels = uint32_t(std::max(table.sourceChannels, 0));
  stream.write(reinterpret_cast<const char*>(&header), sizeof(header));
  stream.write(reinterpret_cast<const char*>(table.samples.data()),
               std::streamsize(table.samples.size() * sizeof(float)));
  if (!stream) {
    if (error) *error = "Could not write complete wavetable";
    return false;
  }
  return true;
}

bool loadBinaryTable(const std::string& path, ImageWavetable* out, std::string* error) {
  if (!out) {
    return false;
  }
  std::ifstream stream(path.c_str(), std::ios::binary);
  if (!stream) {
    if (error) *error = "Embedded wavetable is unavailable";
    return false;
  }
  BinaryHeader header;
  stream.read(reinterpret_cast<char*>(&header), sizeof(header));
  const bool headerValid =
    stream && std::memcmp(header.magic, kMagic, sizeof(kMagic)) == 0 && header.version == 1u &&
    header.frameSize >= 2u && header.frameSize <= 16384u && header.rowCount >= 1u &&
    header.rowCount <= uint32_t(kMaxRows) && header.stride == header.frameSize + 1u;
  if (!headerValid) {
    if (error) *error = "Embedded wavetable header is invalid";
    return false;
  }
  const size_t sampleCount = size_t(header.rowCount) * size_t(header.stride);
  if (sampleCount > std::numeric_limits<size_t>::max() / sizeof(float)) {
    if (error) *error = "Embedded wavetable is too large";
    return false;
  }
  ImageWavetable table;
  table.frameSize = int(header.frameSize);
  table.rowCount = int(header.rowCount);
  table.stride = int(header.stride);
  table.sourceWidth = int(header.sourceWidth);
  table.sourceHeight = int(header.sourceHeight);
  table.sourceChannels = int(header.sourceChannels);
  table.samples.resize(sampleCount);
  stream.read(reinterpret_cast<char*>(table.samples.data()), std::streamsize(sampleCount * sizeof(float)));
  if (!stream || !samplesAreFinite(table)) {
    if (error) *error = "Embedded wavetable data is incomplete";
    return false;
  }
  updateStatistics(&table);
  *out = std::move(table);
  return true;
}

} // namespace iris
