#include "IrisSourceField.hpp"

#include "third_party/qoi.h"

#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <fstream>
#include <limits>
#include <memory>

namespace iris {
namespace {

bool failWith(const std::string& message, std::string* error) {
  if (error) *error = message;
  return false;
}

float clamp01Local(float x) {
  if (!std::isfinite(x)) return 0.f;
  return std::max(0.f, std::min(x, 1.f));
}

uint32_t readBe32(const uint8_t* p) {
  return (uint32_t(p[0]) << 24) | (uint32_t(p[1]) << 16) | (uint32_t(p[2]) << 8) |
         uint32_t(p[3]);
}

bool validateQoiHeader(const std::vector<uint8_t>& data, std::string* error) {
  if (data.size() < 22u) {
    return failWith("Embedded source QOI is too small", error);
  }
  if (std::memcmp(data.data(), "qoif", 4) != 0) {
    return failWith("Embedded source QOI magic is invalid", error);
  }
  const uint32_t width = readBe32(data.data() + 4);
  const uint32_t height = readBe32(data.data() + 8);
  const uint8_t channels = data[12];
  if (width != uint32_t(kCanonicalSourceWidth) ||
      height != uint32_t(kCanonicalSourceHeight)) {
    return failWith("Embedded source QOI dimensions are invalid", error);
  }
  if (channels != uint8_t(kCanonicalSourceChannels)) {
    return failWith("Embedded source QOI channel count is invalid", error);
  }
  return true;
}

} // namespace

bool buildSourceFieldFromRgba8(const uint8_t* rgba, int width, int height,
                               int originalChannels, SourceField* out, std::string* error) {
  if (!rgba || width <= 0 || height <= 0 || !out) {
    return failWith("Invalid source image buffer", error);
  }

  SourceField source;
  source.width = kCanonicalSourceWidth;
  source.height = kCanonicalSourceHeight;
  source.channels = kCanonicalSourceChannels;
  source.bitDepth = kCanonicalSourceBitDepth;
  source.originalWidth = width;
  source.originalHeight = height;
  source.originalChannels = originalChannels;
  source.rgb8.assign(size_t(source.width) * size_t(source.height) * 3u, 0u);

  for (int y = 0; y < source.height; ++y) {
    const float srcY = (float(y) + 0.5f) * float(height) / float(source.height) - 0.5f;
    const int y0 = std::max(0, std::min(int(std::floor(srcY)), height - 1));
    const int y1 = std::min(y0 + 1, height - 1);
    const float fy = clamp01Local(srcY - float(y0));
    for (int x = 0; x < source.width; ++x) {
      const float srcX = (float(x) + 0.5f) * float(width) / float(source.width) - 0.5f;
      const int x0 = std::max(0, std::min(int(std::floor(srcX)), width - 1));
      const int x1 = std::min(x0 + 1, width - 1);
      const float fx = clamp01Local(srcX - float(x0));
      const size_t outBase = (size_t(y) * size_t(source.width) + size_t(x)) * 3u;
      for (int c = 0; c < 3; ++c) {
        const auto component = [&](int px, int py) {
          const size_t base = (size_t(py) * size_t(width) + size_t(px)) * 4u;
          return float(rgba[base + size_t(c)]);
        };
        const float top = component(x0, y0) + (component(x1, y0) - component(x0, y0)) * fx;
        const float bottom =
          component(x0, y1) + (component(x1, y1) - component(x0, y1)) * fx;
        source.rgb8[outBase + size_t(c)] =
          uint8_t(std::round(std::max(0.f, std::min(top + (bottom - top) * fy, 255.f))));
      }
    }
  }

  *out = std::move(source);
  return true;
}

void buildDisplayRgb8FromSourceField(const SourceField& source, std::vector<uint8_t>* rgb8,
                                     int* width, int* height) {
  if (width) *width = source.valid() ? source.width : 0;
  if (height) *height = source.valid() ? source.height : 0;
  if (!rgb8) return;
  if (!source.valid()) {
    rgb8->clear();
    return;
  }
  *rgb8 = source.rgb8;
}

bool saveSourceFieldQoi(const std::string& path, const SourceField& source, std::string* error) {
  if (!source.valid()) {
    return failWith("Invalid source field", error);
  }
  qoi_desc desc;
  desc.width = source.width;
  desc.height = source.height;
  desc.channels = 3;
  desc.colorspace = QOI_SRGB;
  if (!qoi_write(path.c_str(), source.rgb8.data(), &desc)) {
    return failWith("Could not write QOI source field", error);
  }
  return true;
}

bool loadSourceFieldQoi(const std::string& path, SourceField* out, std::string* error) {
  if (!out) {
    return failWith("Missing source field output", error);
  }
  std::vector<uint8_t> data;
  std::ifstream stream(path.c_str(), std::ios::binary);
  if (!stream) {
    return failWith("Embedded source QOI is unavailable", error);
  }
  stream.seekg(0, std::ios::end);
  const std::streamoff size = stream.tellg();
  if (size <= 0 || size > std::streamoff(std::numeric_limits<int>::max())) {
    return failWith("Embedded source QOI size is invalid", error);
  }
  stream.seekg(0, std::ios::beg);
  data.resize(size_t(size));
  stream.read(reinterpret_cast<char*>(data.data()), std::streamsize(size));
  if (!stream) {
    return failWith("Could not read embedded source QOI", error);
  }
  if (!validateQoiHeader(data, error)) {
    return false;
  }

  qoi_desc desc;
  std::unique_ptr<uint8_t, void (*)(void*)> decoded(
    static_cast<uint8_t*>(qoi_decode(data.data(), int(data.size()), &desc, 3)), std::free);
  if (!decoded) {
    return failWith("Could not decode QOI source field", error);
  }
  if (desc.width != kCanonicalSourceWidth || desc.height != kCanonicalSourceHeight ||
      desc.channels != kCanonicalSourceChannels) {
    return failWith("Decoded QOI source field has invalid shape", error);
  }

  SourceField source;
  source.width = int(desc.width);
  source.height = int(desc.height);
  source.channels = kCanonicalSourceChannels;
  source.bitDepth = kCanonicalSourceBitDepth;
  const size_t byteCount = size_t(source.width) * size_t(source.height) * 3u;
  source.rgb8.assign(decoded.get(), decoded.get() + byteCount);
  if (!source.valid()) {
    return failWith("Decoded QOI source field is invalid", error);
  }
  *out = std::move(source);
  return true;
}

} // namespace iris
