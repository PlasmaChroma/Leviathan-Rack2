#include "Nautiloid.hpp"
#include "Iris.hpp"

#include <algorithm>
#include <memory>
#include <utility>

namespace {

constexpr float kNautiloidMaxFractalZoom = 4.f;
constexpr int kDisplaySourceWidth = 768;
constexpr int kDisplaySourceHeight = 512;
constexpr float kFractalCacheScale = 3.f;
constexpr int kFractalCacheWidth = int(float(kDisplaySourceWidth) * kFractalCacheScale);
constexpr int kFractalCacheHeight = int(float(kDisplaySourceHeight) * kFractalCacheScale);
constexpr int kDisplayTileSize = 128;
constexpr float kDisplayReprojectionMaxZoomDelta = 0.45f;
constexpr float kDisplayReprojectionMaxCenterPixels = 96.f;
constexpr int kZoomAheadLayerCount = 3;
constexpr std::array<float, kZoomAheadLayerCount> kZoomAheadLeads = {{0.18f, 0.36f, 0.54f}};
constexpr float kZoomAheadCacheScale = 1.35f;
constexpr int kZoomAheadWidth = 768;
constexpr int kZoomAheadHeight = 512;
constexpr int kZoomAheadTileSize = 128;

bool requestCanUseGpuPreview(int mode, bool shaderAvailable) {
  return shaderAvailable &&
    isDragonKingDebugEnabled() &&
    mode == iris::FRACTAL_MANDELBROT;
}

bool requestGpuPreviewVisible(int mode, bool shaderAvailable, const Nautiloid* module) {
  if (!requestCanUseGpuPreview(mode, shaderAvailable) || !module) return false;
  if (!module->debugGpuPreviewEnabled.load(std::memory_order_relaxed)) return false;
  return true;
}

bool requestGpuPreviewOwnsDisplay(int mode, float zoom, bool shaderAvailable, const Nautiloid* module) {
  (void) zoom;
  return requestGpuPreviewVisible(mode, shaderAvailable, module);
}

Vec nautiloidFractalViewportHalfSpan(int mode) {
  switch (mode) {
    case iris::FRACTAL_MANDELBROT:
      return Vec(1.62f, 0.86f);
    case iris::FRACTAL_JULIA:
      return Vec(1.58f, 0.72f);
    case iris::FRACTAL_PHOENIX_JULIA:
      return Vec(1.62f, 0.74f);
    case iris::FRACTAL_BURNING_SHIP:
      return Vec(0.42f, 0.145f);
    case iris::FRACTAL_CELTIC:
      return Vec(1.62f, 0.88f);
    case iris::FRACTAL_SPIDER:
      return Vec(1.56f, 0.84f);
    case iris::FRACTAL_NOVA:
      return Vec(2.0f, 0.86f);
    case iris::FRACTAL_NEWTON:
      return Vec(2.45f, 0.98f);
    case iris::FRACTAL_TRICORN:
    default:
      return Vec(1.68f, 0.90f);
  }
}

int jsonIntegerOr(json_t* root, const char* key, int fallback) {
  json_t* value = json_object_get(root, key);
  return value ? int(json_integer_value(value)) : fallback;
}

double jsonRealOr(json_t* root, const char* key, double fallback) {
  json_t* value = json_object_get(root, key);
  return value ? json_number_value(value) : fallback;
}

bool jsonBoolOr(json_t* root, const char* key, bool fallback) {
  json_t* value = json_object_get(root, key);
  return value ? json_boolean_value(value) != 0 : fallback;
}

double clampDouble(double value, double minValue, double maxValue) {
  return std::max(minValue, std::min(value, maxValue));
}

const Nautiloid::DisplayCacheTile* findDisplayTile(
  const std::vector<const Nautiloid::DisplayCacheTile*>& tileLookup,
  int columns,
  int x,
  int y) {
  if (columns <= 0 || x < 0 || y < 0) return nullptr;
  const int column = x / kDisplayTileSize;
  const int row = y / kDisplayTileSize;
  const size_t index = size_t(row) * size_t(columns) + size_t(column);
  if (index >= tileLookup.size()) return nullptr;
  const Nautiloid::DisplayCacheTile* tile = tileLookup[index];
  if (!tile || !tile->valid ||
      x < tile->x || y < tile->y ||
      x >= tile->x + tile->width ||
      y >= tile->y + tile->height) {
    return nullptr;
  }
  return tile;
}

std::vector<const Nautiloid::DisplayCacheTile*> makeDisplayTileLookup(
  const Nautiloid::DisplayTileCache& cache,
  int columns,
  int rows) {
  std::vector<const Nautiloid::DisplayCacheTile*> tileLookup(size_t(columns) * size_t(rows), nullptr);
  for (const Nautiloid::DisplayCacheTile& tile : cache.tiles) {
    if (!tile.valid) continue;
    const int column = tile.x / kDisplayTileSize;
    const int row = tile.y / kDisplayTileSize;
    if (column >= 0 && row >= 0 && column < columns && row < rows) {
      tileLookup[size_t(row) * size_t(columns) + size_t(column)] = &tile;
    }
  }
  return tileLookup;
}

bool cropDisplayTileCacheToSize(
  const Nautiloid::DisplayTileCache& cache,
  int mode,
  float zoom,
  double centerX,
  double centerY,
  float cacheScale,
  int outWidth,
  int outHeight,
  const iris::SourceField* fallback,
  bool allowPartial,
  iris::SourceField* out,
  float* coverageOut = nullptr,
  bool* completeOut = nullptr) {
  if (!out || cacheScale <= 1.f || outWidth <= 1 || outHeight <= 1) return false;
  if (cache.validTileCount() == 0u || cache.mode != mode || std::fabs(cache.zoom - zoom) > 1e-5f) return false;
  const float zoomScale = std::pow(0.05f, clamp(zoom, 0.f, kNautiloidMaxFractalZoom));
  const Vec halfSpan = nautiloidFractalViewportHalfSpan(mode).mult(zoomScale);
  const float marginX = (cacheScale - 1.f) * halfSpan.x;
  const float marginY = (cacheScale - 1.f) * halfSpan.y;
  const float dx = float(centerX - cache.centerX);
  const float dy = float(centerY - cache.centerY);
  if (std::fabs(dx) > marginX || std::fabs(dy) > marginY) return false;

  iris::SourceField source;
  source.width = outWidth;
  source.height = outHeight;
  source.channels = iris::kCanonicalSourceChannels;
  source.bitDepth = iris::kCanonicalSourceBitDepth;
  source.originalWidth = source.width;
  source.originalHeight = source.height;
  source.originalChannels = source.channels;
  source.sourceName = "Fractal tile cache";
  const bool fallbackUsable =
    fallback && fallback->valid() && fallback->width == source.width && fallback->height == source.height &&
    fallback->rgb8.size() == size_t(source.width) * size_t(source.height) * 3u;
  if (fallbackUsable) {
    source.rgb8 = fallback->rgb8;
  } else {
    source.rgb8.assign(size_t(source.width) * size_t(source.height) * 3u, 0u);
  }

  const float cacheHalfX = halfSpan.x * cacheScale;
  const float cacheHalfY = halfSpan.y * cacheScale;
  const float cacheCenterPx = (0.5f + dx / (2.f * cacheHalfX)) * float(kFractalCacheWidth);
  const float cacheCenterPy = (0.5f + dy / (2.f * cacheHalfY)) * float(kFractalCacheHeight);
  const float cropW = float(kFractalCacheWidth) / cacheScale;
  const float cropH = float(kFractalCacheHeight) / cacheScale;
  const float cropLeft = cacheCenterPx - 0.5f * cropW;
  const float cropTop = cacheCenterPy - 0.5f * cropH;
  if (cropLeft < -0.01f || cropTop < -0.01f ||
      cropLeft + cropW > float(kFractalCacheWidth) + 0.01f ||
      cropTop + cropH > float(kFractalCacheHeight) + 0.01f) {
    return false;
  }

  const int columns = (kFractalCacheWidth + kDisplayTileSize - 1) / kDisplayTileSize;
  const int rows = (kFractalCacheHeight + kDisplayTileSize - 1) / kDisplayTileSize;
  const std::vector<const Nautiloid::DisplayCacheTile*> tileLookup = makeDisplayTileLookup(cache, columns, rows);
  size_t coveredPixels = 0u;
  const size_t totalPixels = size_t(source.width) * size_t(source.height);
  for (int y = 0; y < source.height; ++y) {
    const float srcY = cropTop + (float(y) + 0.5f) * cropH / float(source.height) - 0.5f;
    const int srcYi = clamp(int(std::round(srcY)), 0, kFractalCacheHeight - 1);
    for (int x = 0; x < source.width; ++x) {
      const float srcX = cropLeft + (float(x) + 0.5f) * cropW / float(source.width) - 0.5f;
      const int srcXi = clamp(int(std::round(srcX)), 0, kFractalCacheWidth - 1);
      const Nautiloid::DisplayCacheTile* tile = findDisplayTile(tileLookup, columns, srcXi, srcYi);
      if (!tile) {
        if (!allowPartial || !fallbackUsable) return false;
        continue;
      }
      const size_t outBase = (size_t(y) * size_t(source.width) + size_t(x)) * 3u;
      const size_t inBase = (size_t(srcYi) * size_t(cache.stitchedWidth) + size_t(srcXi)) * 3u;
      if (cache.stitchedWidth != kFractalCacheWidth ||
          cache.stitchedHeight != kFractalCacheHeight ||
          inBase + 2u >= cache.stitchedRgb8.size()) {
        if (!allowPartial || !fallbackUsable) return false;
        continue;
      }
      source.rgb8[outBase + 0u] = cache.stitchedRgb8[inBase + 0u];
      source.rgb8[outBase + 1u] = cache.stitchedRgb8[inBase + 1u];
      source.rgb8[outBase + 2u] = cache.stitchedRgb8[inBase + 2u];
      ++coveredPixels;
    }
  }
  const bool complete = coveredPixels == totalPixels;
  if (!complete && (!allowPartial || !fallbackUsable || coveredPixels == 0u)) return false;
  if (coverageOut) {
    *coverageOut = totalPixels > 0u ? float(double(coveredPixels) / double(totalPixels)) : 0.f;
  }
  if (completeOut) {
    *completeOut = complete;
  }

  *out = std::move(source);
  return true;
}

bool displayTileCacheCoversView(
  const Nautiloid::DisplayTileCache& cache,
  int mode,
  float zoom,
  double centerX,
  double centerY,
  float cacheScale) {
  if (cache.validTileCount() == 0u || cache.mode != mode || std::fabs(cache.zoom - zoom) > 1e-5f) return false;
  const float zoomScale = std::pow(0.05f, clamp(zoom, 0.f, kNautiloidMaxFractalZoom));
  const Vec halfSpan = nautiloidFractalViewportHalfSpan(mode).mult(zoomScale);
  const float marginX = (cacheScale - 1.f) * halfSpan.x;
  const float marginY = (cacheScale - 1.f) * halfSpan.y;
  return std::fabs(centerX - cache.centerX) <= marginX &&
    std::fabs(centerY - cache.centerY) <= marginY;
}

bool displayTileCacheNeedsTileShiftRecentering(
  const Nautiloid::DisplayTileCache& cache,
  int mode,
  float zoom,
  double centerX,
  double centerY,
  float cacheScale) {
  if (cache.validTileCount() == 0u || cache.mode != mode || std::fabs(cache.zoom - zoom) > 1e-5f) return false;
  const float zoomScale = std::pow(0.05f, clamp(zoom, 0.f, kNautiloidMaxFractalZoom));
  const Vec halfSpan = nautiloidFractalViewportHalfSpan(mode).mult(zoomScale);
  const float tileWorldX = 2.f * halfSpan.x * cacheScale * float(kDisplayTileSize) / float(kFractalCacheWidth);
  const float tileWorldY = 2.f * halfSpan.y * cacheScale * float(kDisplayTileSize) / float(kFractalCacheHeight);
  return std::fabs(centerX - cache.centerX) >= tileWorldX ||
    std::fabs(centerY - cache.centerY) >= tileWorldY;
}

int tileShiftForDelta(float delta, float tileWorld) {
  if (tileWorld <= 0.f) return 0;
  const float units = delta / tileWorld;
  int shift = int(std::round(units));
  if (shift == 0 && std::fabs(units) >= 1.f) {
    shift = units > 0.f ? 1 : -1;
  }
  return shift;
}

bool computeTileAlignedCacheCenter(
  const Nautiloid::DisplayTileCache& cache,
  int mode,
  float zoom,
  double requestedCenterX,
  double requestedCenterY,
  float cacheScale,
  double* alignedCenterX,
  double* alignedCenterY,
  int* shiftColumns,
  int* shiftRows) {
  if (!alignedCenterX || !alignedCenterY || !shiftColumns || !shiftRows) return false;
  if (cache.validTileCount() == 0u || cache.mode != mode || std::fabs(cache.zoom - zoom) > 1e-5f) return false;
  const float zoomScale = std::pow(0.05f, clamp(zoom, 0.f, kNautiloidMaxFractalZoom));
  const Vec halfSpan = nautiloidFractalViewportHalfSpan(mode).mult(zoomScale);
  const float tileWorldX = 2.f * halfSpan.x * cacheScale * float(kDisplayTileSize) / float(kFractalCacheWidth);
  const float tileWorldY = 2.f * halfSpan.y * cacheScale * float(kDisplayTileSize) / float(kFractalCacheHeight);
  const int columns = (kFractalCacheWidth + kDisplayTileSize - 1) / kDisplayTileSize;
  const int rows = (kFractalCacheHeight + kDisplayTileSize - 1) / kDisplayTileSize;
  *shiftColumns = clamp(tileShiftForDelta(requestedCenterX - cache.centerX, tileWorldX), -columns, columns);
  *shiftRows = clamp(tileShiftForDelta(requestedCenterY - cache.centerY, tileWorldY), -rows, rows);
  const double targetX = cache.centerX + double(*shiftColumns) * double(tileWorldX);
  const double targetY = cache.centerY + double(*shiftRows) * double(tileWorldY);
  *alignedCenterX = clampDouble(targetX, -2.0, 2.0);
  *alignedCenterY = clampDouble(targetY, -2.0, 2.0);
  return std::fabs(*alignedCenterX - targetX) <= 1e-5f &&
    std::fabs(*alignedCenterY - targetY) <= 1e-5f &&
    (*shiftColumns != 0 || *shiftRows != 0);
}

void shiftDisplayTileCacheTiles(
  Nautiloid::DisplayTileCache* cache,
  int shiftColumns,
  int shiftRows) {
  if (!cache || (shiftColumns == 0 && shiftRows == 0)) return;
  const int shiftX = shiftColumns * kDisplayTileSize;
  const int shiftY = shiftRows * kDisplayTileSize;
  for (Nautiloid::DisplayCacheTile& tile : cache->tiles) {
    if (!tile.valid) continue;
    const int nextX = tile.x - shiftX;
    const int nextY = tile.y - shiftY;
    if (nextX < 0 || nextY < 0 || nextX >= kFractalCacheWidth || nextY >= kFractalCacheHeight) {
      tile.valid = false;
      continue;
    }
    const int expectedW = std::min(kDisplayTileSize, kFractalCacheWidth - nextX);
    const int expectedH = std::min(kDisplayTileSize, kFractalCacheHeight - nextY);
    if (tile.width != expectedW || tile.height != expectedH ||
        tile.rgb8.size() < size_t(tile.width) * size_t(tile.height) * 3u) {
      tile.valid = false;
      continue;
    }
    tile.x = nextX;
    tile.y = nextY;
  }
  std::fill(cache->stitchedRgb8.begin(), cache->stitchedRgb8.end(), 0u);
  for (const Nautiloid::DisplayCacheTile& tile : cache->tiles) {
    cache->writeTileToStitched(tile);
  }
}

std::vector<Vec> makeDisplayTileOrder(float centerX, float centerY) {
  std::vector<Vec> tiles;
  for (int y = 0; y < kFractalCacheHeight; y += kDisplayTileSize) {
    for (int x = 0; x < kFractalCacheWidth; x += kDisplayTileSize) {
      tiles.push_back(Vec(float(x), float(y)));
    }
  }
  std::sort(tiles.begin(), tiles.end(), [centerX, centerY](const Vec& a, const Vec& b) {
    const float ax = a.x + 0.5f * float(kDisplayTileSize) - centerX;
    const float ay = a.y + 0.5f * float(kDisplayTileSize) - centerY;
    const float bx = b.x + 0.5f * float(kDisplayTileSize) - centerX;
    const float by = b.y + 0.5f * float(kDisplayTileSize) - centerY;
    return ax * ax + ay * ay < bx * bx + by * by;
  });
  return tiles;
}

uint8_t bilinearChannel(const iris::SourceField& source, float x, float y, int channel) {
  if (source.width <= 0 || source.height <= 0 || source.rgb8.empty()) return 0u;
  x = clamp(x, 0.f, float(source.width - 1));
  y = clamp(y, 0.f, float(source.height - 1));
  const int x0 = clamp(int(std::floor(x)), 0, source.width - 1);
  const int y0 = clamp(int(std::floor(y)), 0, source.height - 1);
  const int x1 = std::min(x0 + 1, source.width - 1);
  const int y1 = std::min(y0 + 1, source.height - 1);
  const float tx = x - float(x0);
  const float ty = y - float(y0);
  const auto sample = [&source, channel](int sx, int sy) {
    const size_t base = (size_t(sy) * size_t(source.width) + size_t(sx)) * 3u + size_t(channel);
    return float(source.rgb8[base]);
  };
  const float a = sample(x0, y0) + (sample(x1, y0) - sample(x0, y0)) * tx;
  const float b = sample(x0, y1) + (sample(x1, y1) - sample(x0, y1)) * tx;
  return uint8_t(clamp(int(std::round(a + (b - a) * ty)), 0, 255));
}

bool displayTileCachePixelChannel(
  const Nautiloid::DisplayTileCache& cache,
  const std::vector<const Nautiloid::DisplayCacheTile*>& tileLookup,
  int columns,
  int x,
  int y,
  int channel,
  float* valueOut) {
  if (!valueOut || channel < 0 || channel >= 3) return false;
  const Nautiloid::DisplayCacheTile* tile = findDisplayTile(tileLookup, columns, x, y);
  if (!tile) return false;
  if (cache.stitchedWidth != kFractalCacheWidth ||
      cache.stitchedHeight != kFractalCacheHeight ||
      cache.stitchedRgb8.size() != size_t(kFractalCacheWidth) * size_t(kFractalCacheHeight) * 3u) {
    return false;
  }
  const size_t base = (size_t(y) * size_t(cache.stitchedWidth) + size_t(x)) * 3u + size_t(channel);
  if (base >= cache.stitchedRgb8.size()) return false;
  *valueOut = float(cache.stitchedRgb8[base]);
  return true;
}

bool bilinearDisplayTileCacheRgb(
  const Nautiloid::DisplayTileCache& cache,
  const std::vector<const Nautiloid::DisplayCacheTile*>& tileLookup,
  int columns,
  float x,
  float y,
  uint8_t* r,
  uint8_t* g,
  uint8_t* b) {
  if (!r || !g || !b) return false;
  if (x < 0.f || y < 0.f || x > float(kFractalCacheWidth - 1) || y > float(kFractalCacheHeight - 1)) {
    return false;
  }
  const int x0 = clamp(int(std::floor(x)), 0, kFractalCacheWidth - 1);
  const int y0 = clamp(int(std::floor(y)), 0, kFractalCacheHeight - 1);
  const int x1 = std::min(x0 + 1, kFractalCacheWidth - 1);
  const int y1 = std::min(y0 + 1, kFractalCacheHeight - 1);
  const float tx = x - float(x0);
  const float ty = y - float(y0);
  uint8_t* outs[3] = {r, g, b};
  for (int channel = 0; channel < 3; ++channel) {
    float c00 = 0.f;
    float c10 = 0.f;
    float c01 = 0.f;
    float c11 = 0.f;
    if (!displayTileCachePixelChannel(cache, tileLookup, columns, x0, y0, channel, &c00) ||
        !displayTileCachePixelChannel(cache, tileLookup, columns, x1, y0, channel, &c10) ||
        !displayTileCachePixelChannel(cache, tileLookup, columns, x0, y1, channel, &c01) ||
        !displayTileCachePixelChannel(cache, tileLookup, columns, x1, y1, channel, &c11)) {
      return false;
    }
    const float a = c00 + (c10 - c00) * tx;
    const float v = a + ((c01 + (c11 - c01) * tx) - a) * ty;
    *outs[channel] = uint8_t(clamp(int(std::round(v)), 0, 255));
  }
  return true;
}

bool bilinearPresentationCacheRgb(
  const Nautiloid::PresentationLayer& cache,
  float x,
  float y,
  uint8_t* r,
  uint8_t* g,
  uint8_t* b) {
  if (!cache.valid() || !r || !g || !b) return false;
  if (x < 0.f || y < 0.f || x > float(cache.width - 1) || y > float(cache.height - 1)) {
    return false;
  }
  const int x0 = clamp(int(std::floor(x)), 0, cache.width - 1);
  const int y0 = clamp(int(std::floor(y)), 0, cache.height - 1);
  const int x1 = std::min(x0 + 1, cache.width - 1);
  const int y1 = std::min(y0 + 1, cache.height - 1);
  const float tx = x - float(x0);
  const float ty = y - float(y0);
  if (!cache.tileCoversPixel(x0, y0) ||
      !cache.tileCoversPixel(x1, y0) ||
      !cache.tileCoversPixel(x0, y1) ||
      !cache.tileCoversPixel(x1, y1)) {
    return false;
  }
  uint8_t* outs[3] = {r, g, b};
  for (int channel = 0; channel < 3; ++channel) {
    const auto sample = [&cache, channel](int sx, int sy) {
      const size_t base = (size_t(sy) * size_t(cache.width) + size_t(sx)) * 3u + size_t(channel);
      return float(cache.rgb8[base]);
    };
    const float a = sample(x0, y0) + (sample(x1, y0) - sample(x0, y0)) * tx;
    const float value = a + ((sample(x0, y1) + (sample(x1, y1) - sample(x0, y1)) * tx) - a) * ty;
    *outs[channel] = uint8_t(clamp(int(std::round(value)), 0, 255));
  }
  return true;
}

} // namespace

bool Nautiloid::PresentationLayer::valid() const {
  return mode != iris::FRACTAL_NONE &&
    zoom >= 0.f &&
    width > 1 &&
    height > 1 &&
    cacheScale > 0.f &&
    tileSize > 0 &&
    rgb8.size() == size_t(width) * size_t(height) * 3u;
}

void Nautiloid::PresentationLayer::clear() {
  mode = iris::FRACTAL_NONE;
  zoom = -1.f;
  centerX = 0.f;
  centerY = 0.f;
  cacheScale = 1.f;
  std::fill(rgb8.begin(), rgb8.end(), 0u);
  std::fill(tileValid.begin(), tileValid.end(), 0u);
}

void Nautiloid::PresentationLayer::ensureStorage(int layerWidth, int layerHeight, int layerTileSize) {
  width = layerWidth;
  height = layerHeight;
  tileSize = layerTileSize;
  const size_t pixelBytes = size_t(std::max(width, 0)) * size_t(std::max(height, 0)) * 3u;
  if (rgb8.size() != pixelBytes) {
    rgb8.assign(pixelBytes, 0u);
  }
  const size_t tileCount = fullTileCount();
  if (tileValid.size() != tileCount) {
    tileValid.assign(tileCount, 0u);
  }
}

int Nautiloid::PresentationLayer::columns() const {
  return tileSize > 0 ? (width + tileSize - 1) / tileSize : 0;
}

int Nautiloid::PresentationLayer::rows() const {
  return tileSize > 0 ? (height + tileSize - 1) / tileSize : 0;
}

size_t Nautiloid::PresentationLayer::fullTileCount() const {
  const int cols = columns();
  const int rowCount = rows();
  return cols > 0 && rowCount > 0 ? size_t(cols) * size_t(rowCount) : 0u;
}

size_t Nautiloid::PresentationLayer::validTileCount() const {
  size_t count = 0u;
  for (uint8_t validTile : tileValid) {
    if (validTile) ++count;
  }
  return count;
}

bool Nautiloid::PresentationLayer::tileCoversPixel(int x, int y) const {
  if (!valid() || x < 0 || y < 0 || x >= width || y >= height || tileSize <= 0) return false;
  const int column = x / tileSize;
  const int row = y / tileSize;
  const int cols = columns();
  const size_t index = size_t(row) * size_t(cols) + size_t(column);
  return index < tileValid.size() && tileValid[index] != 0u;
}

void Nautiloid::PresentationLayer::writeTile(
  int tileX,
  int tileY,
  int tileW,
  int tileH,
  const std::vector<uint8_t>& tileRgb8) {
  if (width <= 0 || height <= 0 || tileSize <= 0 || tileW <= 0 || tileH <= 0) return;
  if (tileX < 0 || tileY < 0 || tileX + tileW > width || tileY + tileH > height) return;
  if (tileRgb8.size() < size_t(tileW) * size_t(tileH) * 3u ||
      rgb8.size() != size_t(width) * size_t(height) * 3u) {
    return;
  }
  for (int y = 0; y < tileH; ++y) {
    const size_t srcBase = size_t(y) * size_t(tileW) * 3u;
    const size_t dstBase = (size_t(tileY + y) * size_t(width) + size_t(tileX)) * 3u;
    const size_t byteCount = size_t(tileW) * 3u;
    std::copy(tileRgb8.data() + srcBase, tileRgb8.data() + srcBase + byteCount, rgb8.data() + dstBase);
  }
  const int column = tileX / tileSize;
  const int row = tileY / tileSize;
  const size_t index = size_t(row) * size_t(columns()) + size_t(column);
  if (index < tileValid.size()) {
    tileValid[index] = 1u;
  }
}

void Nautiloid::DisplayTileCache::clear() {
  mode = iris::FRACTAL_NONE;
  zoom = -1.f;
  centerX = 0.f;
  centerY = 0.f;
  for (DisplayCacheTile& tile : tiles) {
    tile.valid = false;
  }
  std::fill(stitchedRgb8.begin(), stitchedRgb8.end(), 0u);
}

void Nautiloid::DisplayTileCache::ensureStorage(int cacheWidth, int cacheHeight, int tileSize) {
  if (stitchedWidth != cacheWidth || stitchedHeight != cacheHeight ||
      stitchedRgb8.size() != size_t(cacheWidth) * size_t(cacheHeight) * 3u) {
    stitchedWidth = cacheWidth;
    stitchedHeight = cacheHeight;
    stitchedRgb8.assign(size_t(cacheWidth) * size_t(cacheHeight) * 3u, 0u);
  }

  const int columns = (cacheWidth + tileSize - 1) / tileSize;
  const int rows = (cacheHeight + tileSize - 1) / tileSize;
  const size_t required = size_t(columns) * size_t(rows);
  if (tiles.size() == required) return;

  tiles.clear();
  tiles.reserve(required);
  for (int row = 0; row < rows; ++row) {
    for (int column = 0; column < columns; ++column) {
      DisplayCacheTile tile;
      tile.x = column * tileSize;
      tile.y = row * tileSize;
      tile.width = std::min(tileSize, cacheWidth - tile.x);
      tile.height = std::min(tileSize, cacheHeight - tile.y);
      tile.valid = false;
      tile.rgb8.resize(size_t(tileSize) * size_t(tileSize) * 3u);
      tiles.push_back(std::move(tile));
    }
  }
}

void Nautiloid::DisplayTileCache::writeTileToStitched(const DisplayCacheTile& tile) {
  if (!tile.valid || stitchedWidth <= 0 || stitchedHeight <= 0 ||
      stitchedRgb8.size() != size_t(stitchedWidth) * size_t(stitchedHeight) * 3u) {
    return;
  }
  if (tile.x < 0 || tile.y < 0 || tile.x + tile.width > stitchedWidth || tile.y + tile.height > stitchedHeight) {
    return;
  }
  for (int y = 0; y < tile.height; ++y) {
    const size_t srcBase = size_t(y) * size_t(tile.width) * 3u;
    const size_t dstBase = (size_t(tile.y + y) * size_t(stitchedWidth) + size_t(tile.x)) * 3u;
    const size_t byteCount = size_t(tile.width) * 3u;
    if (srcBase + byteCount <= tile.rgb8.size() && dstBase + byteCount <= stitchedRgb8.size()) {
      std::copy(tile.rgb8.data() + srcBase,
                tile.rgb8.data() + srcBase + byteCount,
                stitchedRgb8.data() + dstBase);
    }
  }
}

size_t Nautiloid::DisplayTileCache::validTileCount() const {
  size_t count = 0u;
  for (const DisplayCacheTile& tile : tiles) {
    if (tile.valid) {
      ++count;
    }
  }
  return count;
}

Nautiloid::Nautiloid() {
  config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
  configButton(SOURCE_MENU_PARAM, "Fractal");
  configButton(RESET_VIEW_PARAM, "Reset view");
  displayTileCache.ensureStorage(kFractalCacheWidth, kFractalCacheHeight, kDisplayTileSize);
  startWorker();
  requestRender();
}

Nautiloid::~Nautiloid() {
  stopWorker();
}

void Nautiloid::process(const ProcessArgs& args) {
  const uint64_t generation = irisPreviewGeneration.load(std::memory_order_acquire);
  if (generation == 0u) return;
  std::shared_ptr<const iris::SourceField> source;
  if (Module* right = rightExpander.module) {
    if (right->model == modelIris && generation != lastExpanderGenerationSentRight) {
      if (!source) {
        source = irisExpanderSourceSnapshot(nullptr);
      }
      if (source) {
        if (Iris* irisModule = dynamic_cast<Iris*>(right)) {
          irisModule->requestExpanderSource(source, generation);
          lastExpanderGenerationSentRight = generation;
          irisExpanderPublishes.fetch_add(1u, std::memory_order_relaxed);
        }
      }
    }
  } else {
    lastExpanderGenerationSentRight = 0u;
  }
  if (Module* left = leftExpander.module) {
    if (left->model == modelIris && generation != lastExpanderGenerationSentLeft) {
      if (!source) {
        source = irisExpanderSourceSnapshot(nullptr);
      }
      if (source) {
        if (Iris* irisModule = dynamic_cast<Iris*>(left)) {
          irisModule->requestExpanderSource(source, generation);
          lastExpanderGenerationSentLeft = generation;
          irisExpanderPublishes.fetch_add(1u, std::memory_order_relaxed);
        }
      }
    }
  } else {
    lastExpanderGenerationSentLeft = 0u;
  }
}

json_t* Nautiloid::dataToJson() {
  json_t* root = json_object();
  json_object_set_new(root, "fractalMode", json_integer(fractalMode));
  json_object_set_new(root, "fractalZoom", json_real(fractalZoom));
  json_object_set_new(root, "fractalCenterX", json_real(fractalCenterX));
  json_object_set_new(root, "fractalCenterY", json_real(fractalCenterY));
  json_object_set_new(root, "debugFileLoggingEnabled",
                      json_boolean(debugFileLoggingEnabled.load(std::memory_order_relaxed)));
  json_object_set_new(root, "debugGpuPreviewEnabled",
                      json_boolean(debugGpuPreviewEnabled.load(std::memory_order_relaxed)));
  return root;
}

void Nautiloid::dataFromJson(json_t* root) {
  if (!root) return;
  const int mode = jsonIntegerOr(root, "fractalMode", iris::FRACTAL_MANDELBROT);
  fractalMode = iris::isBuiltinFractalMode(mode) ? mode : iris::FRACTAL_MANDELBROT;
  fractalZoom = clamp(jsonRealOr(root, "fractalZoom", 0.f), 0.f, kNautiloidMaxFractalZoom);
  fractalCenterX = clampDouble(jsonRealOr(root, "fractalCenterX", 0.0), -2.0, 2.0);
  fractalCenterY = clampDouble(jsonRealOr(root, "fractalCenterY", 0.0), -2.0, 2.0);
  debugFileLoggingEnabled.store(
    jsonBoolOr(root, "debugFileLoggingEnabled", false), std::memory_order_relaxed);
  debugGpuPreviewEnabled.store(
    jsonBoolOr(root, "debugGpuPreviewEnabled", false), std::memory_order_relaxed);
  debugGpuPreviewAvailable.store(false, std::memory_order_relaxed);
  showMandelbrotEyeMarker.store(false, std::memory_order_relaxed);
  requestRender();
}

void Nautiloid::requestFractal(int mode) {
  if (!iris::isBuiltinFractalMode(mode)) return;
  if (fractalMode != mode) {
    fractalMode = mode;
    fractalZoom = 0.f;
    fractalCenterX = 0.f;
    fractalCenterY = 0.f;
    showMandelbrotEyeMarker.store(false, std::memory_order_relaxed);
  }
  requestRender();
}

void Nautiloid::requestRender() {
  requestRenderWithCacheCenter(fractalCenterX, fractalCenterY);
}

void Nautiloid::requestRenderWithCacheCenter(double cacheCenterX, double cacheCenterY, bool forceCacheRecenter) {
  WorkerRequest request;
  request.mode = fractalMode;
  request.zoom = clamp(fractalZoom, 0.f, kNautiloidMaxFractalZoom);
  request.centerX = clampDouble(fractalCenterX, -2.0, 2.0);
  request.centerY = clampDouble(fractalCenterY, -2.0, 2.0);
  request.cacheCenterX = clampDouble(cacheCenterX, -2.0, 2.0);
  request.cacheCenterY = clampDouble(cacheCenterY, -2.0, 2.0);
  request.forceCacheRecenter = forceCacheRecenter;
  request.zoomInteractionActive = zoomInteractionActive.load(std::memory_order_relaxed);
  submitRequest(request);
}

void Nautiloid::requestInteractiveZoomPreview(double cacheCenterX, double cacheCenterY, bool forceCacheRecenter) {
  WorkerRequest request;
  request.mode = fractalMode;
  request.zoom = clamp(fractalZoom, 0.f, kNautiloidMaxFractalZoom);
  request.centerX = clampDouble(fractalCenterX, -2.0, 2.0);
  request.centerY = clampDouble(fractalCenterY, -2.0, 2.0);
  request.cacheCenterX = clampDouble(cacheCenterX, -2.0, 2.0);
  request.cacheCenterY = clampDouble(cacheCenterY, -2.0, 2.0);
  request.forceCacheRecenter = forceCacheRecenter;
  request.zoomInteractionActive = true;
  {
    std::lock_guard<std::mutex> lock(workerMutex);
    request.serial = nextRequestSerial;
  }
  submitReprojectionRequest(request);
  submitCacheRequest(request);
  submitIrisRequest(request);
}

void Nautiloid::requestRenderWithCenteredCache() {
  requestRenderWithCacheCenter(fractalCenterX, fractalCenterY, true);
}

void Nautiloid::resetView() {
  fractalZoom = 0.f;
  fractalCenterX = 0.f;
  fractalCenterY = 0.f;
  showMandelbrotEyeMarker.store(fractalMode == iris::FRACTAL_MANDELBROT, std::memory_order_relaxed);
  requestRender();
}

void Nautiloid::previewSnapshot(std::vector<uint8_t>* rgb, int* width, int* height) const {
  std::lock_guard<std::mutex> lock(snapshotMutex);
  if (width) *width = previewSource.valid() ? previewSource.width : 0;
  if (height) *height = previewSource.valid() ? previewSource.height : 0;
  if (!rgb) return;
  if (previewSource.valid()) {
    *rgb = previewSource.rgb8;
  } else {
    rgb->clear();
  }
}

void Nautiloid::irisPreviewSnapshot(std::vector<uint8_t>* rgb, int* width, int* height) const {
  std::lock_guard<std::mutex> lock(snapshotMutex);
  if (width) *width = irisCompatibleSource.valid() ? irisCompatibleSource.width : 0;
  if (height) *height = irisCompatibleSource.valid() ? irisCompatibleSource.height : 0;
  if (!rgb) return;
  if (irisCompatibleSource.valid()) {
    *rgb = irisCompatibleSource.rgb8;
  } else {
    rgb->clear();
  }
}

std::shared_ptr<const iris::SourceField> Nautiloid::irisExpanderSourceSnapshot(uint64_t* generation) const {
  if (generation) {
    *generation = irisPreviewGeneration.load(std::memory_order_acquire);
  }
  return std::atomic_load_explicit(&irisExpanderSource, std::memory_order_acquire);
}

void Nautiloid::displayTileCacheSnapshot(DisplayTileCacheSnapshot* snapshot) const {
  if (!snapshot) return;

  DisplayTileCacheSnapshot next;
  next.tileSize = kDisplayTileSize;
  next.cacheWidth = kFractalCacheWidth;
  next.cacheHeight = kFractalCacheHeight;
  next.cacheScale = kFractalCacheScale;
  next.columns = (kFractalCacheWidth + kDisplayTileSize - 1) / kDisplayTileSize;
  next.rows = (kFractalCacheHeight + kDisplayTileSize - 1) / kDisplayTileSize;
  next.fullTileCount = size_t(next.columns) * size_t(next.rows);
  next.tileCurrent.assign(size_t(next.columns) * size_t(next.rows), 0u);

  std::lock_guard<std::mutex> lock(cacheDataMutex);
  next.current =
    displayTileCache.mode == fractalMode &&
    std::fabs(displayTileCache.zoom - clamp(fractalZoom, 0.f, kNautiloidMaxFractalZoom)) <= 1e-5f;
  next.cacheMode = displayTileCache.mode;
  next.cacheZoom = displayTileCache.zoom;
  next.cacheCenterX = displayTileCache.centerX;
  next.cacheCenterY = displayTileCache.centerY;
  if (displayTileCache.mode == fractalMode) {
    for (const DisplayCacheTile& tile : displayTileCache.tiles) {
      if (!tile.valid) continue;
      const int column = tile.x / kDisplayTileSize;
      const int row = tile.y / kDisplayTileSize;
      if (column >= 0 && row >= 0 && column < next.columns && row < next.rows) {
        next.tileCurrent[size_t(row) * size_t(next.columns) + size_t(column)] = 1u;
        ++next.currentTileCount;
      }
    }
  }

  *snapshot = std::move(next);
}

void Nautiloid::zoomAheadCacheSnapshot(ZoomAheadCacheSnapshot* snapshot) const {
  if (!snapshot) return;
  ZoomAheadCacheSnapshot next;
  std::lock_guard<std::mutex> lock(cacheDataMutex);
  for (int i = 0; i < kZoomAheadLayerCount; ++i) {
    const PresentationLayer& layer = zoomAheadLayers[size_t(i)];
    next.currentTileCount[size_t(i)] = layer.validTileCount();
    next.fullTileCount[size_t(i)] = layer.fullTileCount();
    next.zoom[size_t(i)] = layer.zoom;
  }
  *snapshot = next;
}

void Nautiloid::startWorker() {
  workerStop = false;
  cacheWorkerStop = false;
  reprojectionWorkerStop = false;
  irisWorkerStop = false;
  worker = std::thread([this]() { workerLoop(); });
  cacheWorker = std::thread([this]() { cacheWorkerLoop(); });
  reprojectionWorker = std::thread([this]() { reprojectionWorkerLoop(); });
  irisWorker = std::thread([this]() { irisWorkerLoop(); });
}

void Nautiloid::stopWorker() {
  {
    std::lock_guard<std::mutex> lock(workerMutex);
    workerStop = true;
    requestPending = false;
  }
  workerCv.notify_one();
  {
    std::lock_guard<std::mutex> lock(cacheRequestMutex);
    cacheWorkerStop = true;
    cacheRequestPending = false;
  }
  cacheRequestCv.notify_one();
  {
    std::lock_guard<std::mutex> lock(reprojectionRequestMutex);
    reprojectionWorkerStop = true;
    reprojectionRequestPending = false;
  }
  reprojectionRequestCv.notify_one();
  {
    std::lock_guard<std::mutex> lock(irisRequestMutex);
    irisWorkerStop = true;
    irisRequestPending = false;
  }
  irisRequestCv.notify_one();
  if (worker.joinable()) {
    worker.join();
  }
  if (cacheWorker.joinable()) {
    cacheWorker.join();
  }
  if (reprojectionWorker.joinable()) {
    reprojectionWorker.join();
  }
  if (irisWorker.joinable()) {
    irisWorker.join();
  }
}

void Nautiloid::submitRequest(const WorkerRequest& request) {
  WorkerRequest submittedRequest;
  {
    std::lock_guard<std::mutex> lock(workerMutex);
    workerRequest = request;
    workerRequest.serial = ++nextRequestSerial;
    submittedRequest = workerRequest;
    renderRequestsSubmitted.fetch_add(1u, std::memory_order_relaxed);
    requestPending = true;
    displayRenderBusy.store(true, std::memory_order_release);
    loading.store(true, std::memory_order_release);
  }
  submitReprojectionRequest(submittedRequest);
  submitIrisRequest(submittedRequest);
  workerCv.notify_one();
}

void Nautiloid::submitCacheRequest(const WorkerRequest& request) {
  {
    std::lock_guard<std::mutex> lock(cacheRequestMutex);
    cacheRequest = request;
    cacheRequestPending = true;
    cacheRequestsSubmitted.fetch_add(1u, std::memory_order_relaxed);
  }
  cacheRequestCv.notify_one();
}

void Nautiloid::submitReprojectionRequest(const WorkerRequest& request) {
  {
    std::lock_guard<std::mutex> lock(reprojectionRequestMutex);
    reprojectionRequest = request;
    reprojectionRequestPending = true;
  }
  reprojectionRequestCv.notify_one();
}

void Nautiloid::submitIrisRequest(const WorkerRequest& request) {
  {
    std::lock_guard<std::mutex> lock(irisRequestMutex);
    irisRequest = request;
    irisRequestPending = true;
  }
  irisRequestCv.notify_one();
}

void Nautiloid::markDisplayRenderFinished(uint64_t serial) {
  std::lock_guard<std::mutex> lock(workerMutex);
  if (serial == nextRequestSerial && !requestPending) {
    displayRenderBusy.store(false, std::memory_order_release);
  }
}

void Nautiloid::publishAuthoritativeDisplaySource(iris::SourceField source, const WorkerRequest& request) {
  std::lock_guard<std::mutex> lock(snapshotMutex);
  previewSource = source;
  authoritativeDisplaySource = std::move(source);
  authoritativeDisplayMode = request.mode;
  authoritativeDisplayZoom = request.zoom;
  authoritativeDisplayCenterX = request.centerX;
  authoritativeDisplayCenterY = request.centerY;
}

bool Nautiloid::publishDisplayReprojection(const WorkerRequest& request) {
  iris::SourceField base;
  int baseMode = iris::FRACTAL_NONE;
  float baseZoom = -1.f;
  double baseCenterX = 0.0;
  double baseCenterY = 0.0;
  {
    std::lock_guard<std::mutex> lock(snapshotMutex);
    base = authoritativeDisplaySource;
    baseMode = authoritativeDisplayMode;
    baseZoom = authoritativeDisplayZoom;
    baseCenterX = authoritativeDisplayCenterX;
    baseCenterY = authoritativeDisplayCenterY;
  }
  if (!base.valid() || baseMode != request.mode || base.width != kDisplaySourceWidth || base.height != kDisplaySourceHeight) {
    return false;
  }
  if (std::fabs(request.zoom - baseZoom) <= 1e-5f) {
    return false;
  }
  if (std::fabs(request.zoom - baseZoom) > kDisplayReprojectionMaxZoomDelta) {
    return false;
  }
  const Vec oldHalfSpan = nautiloidFractalViewportHalfSpan(baseMode).mult(
    std::pow(0.05f, clamp(baseZoom, 0.f, kNautiloidMaxFractalZoom)));
  const Vec newHalfSpan = nautiloidFractalViewportHalfSpan(request.mode).mult(
    std::pow(0.05f, clamp(request.zoom, 0.f, kNautiloidMaxFractalZoom)));
  if (oldHalfSpan.x <= 0.f || oldHalfSpan.y <= 0.f || newHalfSpan.x <= 0.f || newHalfSpan.y <= 0.f) {
    return false;
  }
  const float centerPixelsX = float(std::fabs(request.centerX - baseCenterX) / (2.0 * double(oldHalfSpan.x)) * double(base.width));
  const float centerPixelsY = float(std::fabs(request.centerY - baseCenterY) / (2.0 * double(oldHalfSpan.y)) * double(base.height));
  if (centerPixelsX > kDisplayReprojectionMaxCenterPixels || centerPixelsY > kDisplayReprojectionMaxCenterPixels) {
    return false;
  }

  iris::SourceField reprojected;
  reprojected.width = kDisplaySourceWidth;
  reprojected.height = kDisplaySourceHeight;
  reprojected.channels = iris::kCanonicalSourceChannels;
  reprojected.bitDepth = iris::kCanonicalSourceBitDepth;
  reprojected.originalWidth = reprojected.width;
  reprojected.originalHeight = reprojected.height;
  reprojected.originalChannels = reprojected.channels;
  reprojected.sourceName = "Fractal zoom reprojection";
  reprojected.rgb8.assign(size_t(reprojected.width) * size_t(reprojected.height) * 3u, 0u);

  {
    std::lock_guard<std::mutex> lock(cacheDataMutex);
    const std::array<PresentationLayer, kZoomAheadLayerCount> aheadLayers = zoomAheadLayers;
    const bool cacheUsable =
      displayTileCache.validTileCount() > 0u &&
      displayTileCache.mode == request.mode &&
      displayTileCache.zoom >= 0.f;
    const PresentationLayer retainedCache = displayPresentationCache;
    const bool retainedCacheUsable =
      retainedCache.valid() &&
      retainedCache.mode == request.mode;
    const Vec cacheHalfSpan = cacheUsable
      ? nautiloidFractalViewportHalfSpan(displayTileCache.mode).mult(
          std::pow(0.05f, clamp(displayTileCache.zoom, 0.f, kNautiloidMaxFractalZoom))).mult(kFractalCacheScale)
      : Vec();
    std::array<Vec, kZoomAheadLayerCount> aheadHalfSpans;
    for (int i = 0; i < kZoomAheadLayerCount; ++i) {
      const PresentationLayer& layer = aheadLayers[size_t(i)];
      if (layer.valid() && layer.mode == request.mode &&
          std::fabs(request.zoom - layer.zoom) <= kZoomAheadLeads[size_t(i)] * 1.35f) {
        aheadHalfSpans[size_t(i)] = nautiloidFractalViewportHalfSpan(layer.mode).mult(
          std::pow(0.05f, clamp(layer.zoom, 0.f, kNautiloidMaxFractalZoom))).mult(layer.cacheScale);
      } else {
        aheadHalfSpans[size_t(i)] = Vec();
      }
    }
    const Vec retainedCacheHalfSpan = retainedCacheUsable
      ? nautiloidFractalViewportHalfSpan(retainedCache.mode).mult(
          std::pow(0.05f, clamp(retainedCache.zoom, 0.f, kNautiloidMaxFractalZoom))).mult(retainedCache.cacheScale)
      : Vec();
    const int cacheColumns = (kFractalCacheWidth + kDisplayTileSize - 1) / kDisplayTileSize;
    const int cacheRows = (kFractalCacheHeight + kDisplayTileSize - 1) / kDisplayTileSize;
    const std::vector<const DisplayCacheTile*> tileLookup =
      cacheUsable ? makeDisplayTileLookup(displayTileCache, cacheColumns, cacheRows) : std::vector<const DisplayCacheTile*>();

    bool usedAheadForFrame = false;
    for (int y = 0; y < reprojected.height; ++y) {
      const float ny = (float(y) + 0.5f) / float(reprojected.height) * 2.f - 1.f;
      const double worldY = request.centerY + double(ny) * double(newHalfSpan.y);
      const float oldNormY = float((worldY - baseCenterY) / double(oldHalfSpan.y));
      const float srcY = (oldNormY + 1.f) * 0.5f * float(base.height) - 0.5f;
      const float cacheSrcY = cacheUsable && cacheHalfSpan.y > 0.f
        ? float((0.5 + (worldY - displayTileCache.centerY) / (2.0 * double(cacheHalfSpan.y))) * double(kFractalCacheHeight) - 0.5)
        : -1.f;
      const float retainedSrcY = retainedCacheUsable && retainedCacheHalfSpan.y > 0.f
        ? float((0.5 + (worldY - retainedCache.centerY) / (2.0 * double(retainedCacheHalfSpan.y))) * double(retainedCache.height) - 0.5)
        : -1.f;
      for (int x = 0; x < reprojected.width; ++x) {
        const float nx = (float(x) + 0.5f) / float(reprojected.width) * 2.f - 1.f;
        const double worldX = request.centerX + double(nx) * double(newHalfSpan.x);
        const float oldNormX = float((worldX - baseCenterX) / double(oldHalfSpan.x));
        const float srcX = (oldNormX + 1.f) * 0.5f * float(base.width) - 0.5f;
        const size_t outBase = (size_t(y) * size_t(reprojected.width) + size_t(x)) * 3u;
        uint8_t r = 0u;
        uint8_t g = 0u;
        uint8_t b = 0u;
        const float cacheSrcX = cacheUsable && cacheHalfSpan.x > 0.f
          ? float((0.5 + (worldX - displayTileCache.centerX) / (2.0 * double(cacheHalfSpan.x))) * double(kFractalCacheWidth) - 0.5)
          : -1.f;
        const float retainedSrcX = retainedCacheUsable && retainedCacheHalfSpan.x > 0.f
          ? float((0.5 + (worldX - retainedCache.centerX) / (2.0 * double(retainedCacheHalfSpan.x))) * double(retainedCache.width) - 0.5)
          : -1.f;
        bool sampledAhead = false;
        for (int layerIndex = kZoomAheadLayerCount - 1; layerIndex >= 0; --layerIndex) {
          const PresentationLayer& layer = aheadLayers[size_t(layerIndex)];
          const Vec layerHalfSpan = aheadHalfSpans[size_t(layerIndex)];
          if (!layer.valid() || layerHalfSpan.x <= 0.f || layerHalfSpan.y <= 0.f) continue;
          const float layerSrcX =
            float((0.5 + (worldX - layer.centerX) / (2.0 * double(layerHalfSpan.x))) * double(layer.width) - 0.5);
          const float layerSrcY =
            float((0.5 + (worldY - layer.centerY) / (2.0 * double(layerHalfSpan.y))) * double(layer.height) - 0.5);
          if (bilinearPresentationCacheRgb(layer, layerSrcX, layerSrcY, &r, &g, &b)) {
            reprojected.rgb8[outBase + 0u] = r;
            reprojected.rgb8[outBase + 1u] = g;
            reprojected.rgb8[outBase + 2u] = b;
            sampledAhead = true;
            usedAheadForFrame = true;
            break;
          }
        }
        if (sampledAhead) {
          continue;
        } else if (cacheUsable &&
            bilinearDisplayTileCacheRgb(displayTileCache, tileLookup, cacheColumns, cacheSrcX, cacheSrcY, &r, &g, &b)) {
          reprojected.rgb8[outBase + 0u] = r;
          reprojected.rgb8[outBase + 1u] = g;
          reprojected.rgb8[outBase + 2u] = b;
        } else if (retainedCacheUsable &&
                   bilinearPresentationCacheRgb(retainedCache, retainedSrcX, retainedSrcY, &r, &g, &b)) {
          reprojected.rgb8[outBase + 0u] = r;
          reprojected.rgb8[outBase + 1u] = g;
          reprojected.rgb8[outBase + 2u] = b;
        } else if (srcX < 0.f || srcY < 0.f || srcX > float(base.width - 1) || srcY > float(base.height - 1)) {
          reprojected.rgb8[outBase + 0u] = 4u;
          reprojected.rgb8[outBase + 1u] = 7u;
          reprojected.rgb8[outBase + 2u] = 10u;
        } else {
          reprojected.rgb8[outBase + 0u] = bilinearChannel(base, srcX, srcY, 0);
          reprojected.rgb8[outBase + 1u] = bilinearChannel(base, srcX, srcY, 1);
          reprojected.rgb8[outBase + 2u] = bilinearChannel(base, srcX, srcY, 2);
        }
      }
    }
    if (usedAheadForFrame) {
      displayReprojectionZoomAheadHits.fetch_add(1u, std::memory_order_relaxed);
    }
  }

  {
    std::lock_guard<std::mutex> lock(workerMutex);
    if (request.serial != nextRequestSerial) {
      return false;
    }
  }
  {
    std::lock_guard<std::mutex> lock(snapshotMutex);
    previewSource = std::move(reprojected);
  }
  previewGeneration.fetch_add(1u, std::memory_order_release);
  displayReprojectionPublishes.fetch_add(1u, std::memory_order_relaxed);
  return true;
}

bool Nautiloid::publishDisplayCacheComposite(const WorkerRequest& request, bool allowPartial, bool* completeOut) {
  iris::SourceField fallback;
  if (allowPartial) {
    std::lock_guard<std::mutex> lock(snapshotMutex);
    fallback = previewSource;
  }

  iris::SourceField source;
  bool complete = false;
  {
    std::lock_guard<std::mutex> lock(cacheDataMutex);
    if (!cropDisplayTileCacheToSize(
          displayTileCache,
          request.mode,
          request.zoom,
          request.centerX,
          request.centerY,
          kFractalCacheScale,
          kDisplaySourceWidth,
          kDisplaySourceHeight,
          allowPartial ? &fallback : nullptr,
          allowPartial,
          &source,
          nullptr,
          &complete)) {
      return false;
    }
  }
  {
    std::lock_guard<std::mutex> lock(workerMutex);
    if (request.serial != nextRequestSerial) {
      return false;
    }
  }
  if (complete) {
    publishAuthoritativeDisplaySource(std::move(source), request);
  } else {
    std::lock_guard<std::mutex> lock(snapshotMutex);
    previewSource = std::move(source);
  }
  if (completeOut) {
    *completeOut = complete;
  }
  previewGeneration.fetch_add(1u, std::memory_order_release);
  displayRendersCompleted.fetch_add(1u, std::memory_order_relaxed);
  displayCacheCompositePublishes.fetch_add(1u, std::memory_order_relaxed);
  loading.store(false, std::memory_order_release);
  return true;
}

void Nautiloid::renderZoomAheadCaches(const WorkerRequest& request) {
  for (int layerIndex = 0; layerIndex < kZoomAheadLayerCount; ++layerIndex) {
    const float aheadZoom = clamp(
      request.zoom + kZoomAheadLeads[size_t(layerIndex)], 0.f, kNautiloidMaxFractalZoom);
    if (aheadZoom <= request.zoom + 1e-4f) continue;

    {
      std::lock_guard<std::mutex> lock(workerMutex);
      if (request.serial != nextRequestSerial) {
        return;
      }
    }

    {
      std::lock_guard<std::mutex> lock(cacheDataMutex);
      PresentationLayer& layer = zoomAheadLayers[size_t(layerIndex)];
      layer.ensureStorage(kZoomAheadWidth, kZoomAheadHeight, kZoomAheadTileSize);
      const bool compatible =
        layer.mode == request.mode &&
        std::fabs(layer.zoom - aheadZoom) <= 1e-5f &&
        std::fabs(layer.centerX - request.centerX) <= 1e-5f &&
        std::fabs(layer.centerY - request.centerY) <= 1e-5f &&
        std::fabs(layer.cacheScale - kZoomAheadCacheScale) <= 1e-5f;
      if (!compatible) {
        layer.clear();
        layer.ensureStorage(kZoomAheadWidth, kZoomAheadHeight, kZoomAheadTileSize);
        layer.mode = request.mode;
        layer.zoom = aheadZoom;
        layer.centerX = request.centerX;
        layer.centerY = request.centerY;
        layer.cacheScale = kZoomAheadCacheScale;
      }
      if (layer.validTileCount() >= layer.fullTileCount()) {
        continue;
      }
    }

    const std::vector<Vec> tileOrder = makeDisplayTileOrder(
      0.5f * float(kZoomAheadWidth),
      0.5f * float(kZoomAheadHeight));
    for (const Vec& tilePos : tileOrder) {
      const int tileX = int(tilePos.x);
      const int tileY = int(tilePos.y);
      if (tileX >= kZoomAheadWidth || tileY >= kZoomAheadHeight) continue;

      {
        std::lock_guard<std::mutex> lock(workerMutex);
        if (request.serial != nextRequestSerial) {
          return;
        }
      }
      {
        std::lock_guard<std::mutex> lock(cacheDataMutex);
        const PresentationLayer& layer = zoomAheadLayers[size_t(layerIndex)];
        if (layer.tileCoversPixel(tileX, tileY)) {
          continue;
        }
      }

      iris::SourceField tileSource;
      std::string error;
      const int tileW = std::min(kZoomAheadTileSize, kZoomAheadWidth - tileX);
      const int tileH = std::min(kZoomAheadTileSize, kZoomAheadHeight - tileY);
      const bool ok = iris::makeBuiltinFractalSourceSized(
        request.mode,
        aheadZoom,
        request.centerX,
        request.centerY,
        tileW,
        tileH,
        kZoomAheadCacheScale,
        kZoomAheadWidth,
        kZoomAheadHeight,
        tileX,
        tileY,
        &tileSource,
        &error);
      if (!ok || !tileSource.valid()) continue;

      {
        std::lock_guard<std::mutex> lock(workerMutex);
        if (request.serial != nextRequestSerial) {
          return;
        }
      }
      {
        std::lock_guard<std::mutex> lock(cacheDataMutex);
        PresentationLayer& layer = zoomAheadLayers[size_t(layerIndex)];
        if (layer.mode != request.mode ||
            std::fabs(layer.zoom - aheadZoom) > 1e-5f ||
            std::fabs(layer.centerX - request.centerX) > 1e-5f ||
            std::fabs(layer.centerY - request.centerY) > 1e-5f) {
          continue;
        }
        layer.writeTile(tileX, tileY, tileSource.width, tileSource.height, tileSource.rgb8);
        zoomAheadTilesRendered.fetch_add(1u, std::memory_order_relaxed);
      }
    }
  }
}

void Nautiloid::reprojectionWorkerLoop() {
  while (true) {
    WorkerRequest request;
    {
      std::unique_lock<std::mutex> lock(reprojectionRequestMutex);
      reprojectionRequestCv.wait(lock, [this]() { return reprojectionWorkerStop || reprojectionRequestPending; });
      if (reprojectionWorkerStop) break;
      request = reprojectionRequest;
      reprojectionRequestPending = false;
    }
    publishDisplayReprojection(request);
  }
}

void Nautiloid::workerLoop() {
  while (true) {
    WorkerRequest request;
    {
      std::unique_lock<std::mutex> lock(workerMutex);
      workerCv.wait(lock, [this]() { return workerStop || requestPending; });
      if (workerStop) break;
      request = workerRequest;
      requestPending = false;
    }

    const bool gpuPreviewOwnsDisplay =
      requestGpuPreviewOwnsDisplay(
        request.mode,
        request.zoom,
        debugGpuPreviewAvailable.load(std::memory_order_relaxed),
        this);
    if (gpuPreviewOwnsDisplay) {
      loading.store(false, std::memory_order_release);
      submitCacheRequest(request);
      markDisplayRenderFinished(request.serial);
      continue;
    }

    bool cacheRequestSubmitted = false;
    if (request.zoomInteractionActive) {
      submitCacheRequest(request);
      cacheRequestSubmitted = true;
    }

    bool cacheComplete = false;
    bool ok = publishDisplayCacheComposite(request, true, &cacheComplete);
    if (ok && cacheComplete) {
      displayCacheHits.fetch_add(1u, std::memory_order_relaxed);
    } else if (ok) {
      displayCachePartialHits.fetch_add(1u, std::memory_order_relaxed);
    }
    if (ok) {
      if (!cacheRequestSubmitted) {
        submitCacheRequest(request);
      }
      markDisplayRenderFinished(request.serial);
      continue;
    }

    iris::SourceField source;
    std::string error;
    {
      displayCacheMisses.fetch_add(1u, std::memory_order_relaxed);
    }
    ok = iris::makeBuiltinFractalSourceSized(
      request.mode,
      request.zoom,
      request.centerX,
      request.centerY,
      kDisplaySourceWidth,
      kDisplaySourceHeight,
      1.f,
      &source,
      &error);

    if (!ok) {
      loading.store(false, std::memory_order_release);
      markDisplayRenderFinished(request.serial);
      continue;
    }

    {
      std::lock_guard<std::mutex> lock(workerMutex);
      if (request.serial != nextRequestSerial) {
        displayRendersDroppedStale.fetch_add(1u, std::memory_order_relaxed);
        continue;
      }
    }
    publishAuthoritativeDisplaySource(std::move(source), request);
    previewGeneration.fetch_add(1u, std::memory_order_release);
    displayRendersCompleted.fetch_add(1u, std::memory_order_relaxed);
    loading.store(false, std::memory_order_release);
    if (!cacheRequestSubmitted) {
      submitCacheRequest(request);
    }
    markDisplayRenderFinished(request.serial);
  }
}

void Nautiloid::cacheWorkerLoop() {
  while (true) {
    WorkerRequest request;
    {
      std::unique_lock<std::mutex> lock(cacheRequestMutex);
      cacheRequestCv.wait(lock, [this]() { return cacheWorkerStop || cacheRequestPending; });
      if (cacheWorkerStop) break;
      request = cacheRequest;
      cacheRequestPending = false;
      cacheRequestsDequeued.fetch_add(1u, std::memory_order_relaxed);
    }

    double targetCacheCenterX = request.cacheCenterX;
    double targetCacheCenterY = request.cacheCenterY;
    const bool gpuPreviewOwnsDisplay =
      requestGpuPreviewOwnsDisplay(
        request.mode,
        request.zoom,
        debugGpuPreviewAvailable.load(std::memory_order_relaxed),
        this);
    const bool gpuPreviewVisible =
      requestGpuPreviewVisible(
        request.mode,
        debugGpuPreviewAvailable.load(std::memory_order_relaxed),
        this);
    bool skipDisplayTiles = gpuPreviewVisible || request.zoomInteractionActive;
    if (!skipDisplayTiles) {
      std::lock_guard<std::mutex> lock(cacheDataMutex);
      const bool existingCacheCoversView = displayTileCacheCoversView(
        displayTileCache,
        request.mode,
        request.zoom,
        request.centerX,
        request.centerY,
        kFractalCacheScale);
      const size_t fullTileCount =
        size_t((kFractalCacheWidth + kDisplayTileSize - 1) / kDisplayTileSize) *
        size_t((kFractalCacheHeight + kDisplayTileSize - 1) / kDisplayTileSize);
      displayTileCache.ensureStorage(kFractalCacheWidth, kFractalCacheHeight, kDisplayTileSize);
      const bool existingCacheFull = displayTileCache.validTileCount() >= fullTileCount;
      const bool existingCacheNeedsRecentering = displayTileCacheNeedsTileShiftRecentering(
        displayTileCache,
        request.mode,
        request.zoom,
        request.centerX,
        request.centerY,
        kFractalCacheScale);
      int shiftColumns = 0;
      int shiftRows = 0;
      bool targetTileAligned = false;
      if (!request.forceCacheRecenter && existingCacheCoversView && existingCacheFull && existingCacheNeedsRecentering) {
        targetTileAligned = computeTileAlignedCacheCenter(
          displayTileCache,
          request.mode,
          request.zoom,
          targetCacheCenterX,
          targetCacheCenterY,
          kFractalCacheScale,
          &targetCacheCenterX,
          &targetCacheCenterY,
          &shiftColumns,
          &shiftRows);
      }
      if (!request.forceCacheRecenter && existingCacheCoversView && (!existingCacheFull || !existingCacheNeedsRecentering)) {
        targetCacheCenterX = displayTileCache.centerX;
        targetCacheCenterY = displayTileCache.centerY;
      }
      if (!request.forceCacheRecenter && existingCacheCoversView && existingCacheFull && !existingCacheNeedsRecentering) {
        skipDisplayTiles = true;
      }
    if (displayTileCache.mode != request.mode ||
          std::fabs(displayTileCache.zoom - request.zoom) > 1e-5f ||
          std::fabs(displayTileCache.centerX - targetCacheCenterX) > 1e-5f ||
          std::fabs(displayTileCache.centerY - targetCacheCenterY) > 1e-5f) {
        if (displayTileCache.validTileCount() > 0u &&
            displayTileCache.stitchedRgb8.size() == size_t(kFractalCacheWidth) * size_t(kFractalCacheHeight) * 3u) {
          displayPresentationCache.ensureStorage(
            displayTileCache.stitchedWidth, displayTileCache.stitchedHeight, kDisplayTileSize);
          displayPresentationCache.mode = displayTileCache.mode;
          displayPresentationCache.zoom = displayTileCache.zoom;
          displayPresentationCache.centerX = displayTileCache.centerX;
          displayPresentationCache.centerY = displayTileCache.centerY;
          displayPresentationCache.cacheScale = kFractalCacheScale;
          displayPresentationCache.rgb8 = displayTileCache.stitchedRgb8;
          std::fill(displayPresentationCache.tileValid.begin(), displayPresentationCache.tileValid.end(), 0u);
          const int cols = displayPresentationCache.columns();
          for (const DisplayCacheTile& tile : displayTileCache.tiles) {
            if (!tile.valid) continue;
            const int column = tile.x / kDisplayTileSize;
            const int row = tile.y / kDisplayTileSize;
            const size_t index = size_t(row) * size_t(cols) + size_t(column);
            if (index < displayPresentationCache.tileValid.size()) {
              displayPresentationCache.tileValid[index] = 1u;
            }
          }
        }
        const bool canShiftExistingTiles =
          targetTileAligned &&
          displayTileCache.mode == request.mode &&
          std::fabs(displayTileCache.zoom - request.zoom) <= 1e-5f;
        if (canShiftExistingTiles) {
          shiftDisplayTileCacheTiles(&displayTileCache, shiftColumns, shiftRows);
          displayTileCache.centerX = targetCacheCenterX;
          displayTileCache.centerY = targetCacheCenterY;
          displayTileCacheShifts.fetch_add(1u, std::memory_order_relaxed);
        } else {
          displayTileCache.clear();
          displayTileCache.mode = request.mode;
          displayTileCache.zoom = request.zoom;
          displayTileCache.centerX = targetCacheCenterX;
          displayTileCache.centerY = targetCacheCenterY;
          for (PresentationLayer& layer : zoomAheadLayers) {
            if (layer.mode != request.mode || layer.zoom <= request.zoom) {
              layer.clear();
            }
          }
          displayTileCacheResets.fetch_add(1u, std::memory_order_relaxed);
        }
      }
    }

    bool stale = false;
    bool renderedAnyTile = false;
    int renderedTilesSincePublish = 0;
    const float visibleCenterX = float((0.5 + (request.centerX - targetCacheCenterX) /
      (2.0 * double(nautiloidFractalViewportHalfSpan(request.mode).mult(
        std::pow(0.05f, clamp(request.zoom, 0.f, kNautiloidMaxFractalZoom))).x) * double(kFractalCacheScale))) *
      double(kFractalCacheWidth));
    const float visibleCenterY = float((0.5 + (request.centerY - targetCacheCenterY) /
      (2.0 * double(nautiloidFractalViewportHalfSpan(request.mode).mult(
        std::pow(0.05f, clamp(request.zoom, 0.f, kNautiloidMaxFractalZoom))).y) * double(kFractalCacheScale))) *
      double(kFractalCacheHeight));
    const std::vector<Vec> tileOrder = makeDisplayTileOrder(visibleCenterX, visibleCenterY);
    for (const Vec& tilePos : tileOrder) {
      if (skipDisplayTiles) break;
      if (stale) break;
      const int tileX = int(tilePos.x);
      const int tileY = int(tilePos.y);
        {
          std::lock_guard<std::mutex> lock(workerMutex);
          if (request.serial != nextRequestSerial) {
            displayCacheTileAborts.fetch_add(1u, std::memory_order_relaxed);
            stale = true;
            break;
          }
        }
        {
          std::lock_guard<std::mutex> lock(cacheDataMutex);
          bool tileCurrent = false;
          for (const DisplayCacheTile& tile : displayTileCache.tiles) {
            if (tile.valid && tile.x == tileX && tile.y == tileY) {
              tileCurrent = true;
              break;
            }
          }
          if (tileCurrent) continue;
        }

        iris::SourceField tileSource;
        std::string error;
        const int tileW = std::min(kDisplayTileSize, kFractalCacheWidth - tileX);
        const int tileH = std::min(kDisplayTileSize, kFractalCacheHeight - tileY);
        const bool ok = iris::makeBuiltinFractalSourceSized(
          request.mode,
          request.zoom,
          targetCacheCenterX,
          targetCacheCenterY,
          tileW,
          tileH,
          kFractalCacheScale,
          kFractalCacheWidth,
          kFractalCacheHeight,
          tileX,
          tileY,
          &tileSource,
          &error);
        if (!ok) continue;

        {
          std::lock_guard<std::mutex> lock(workerMutex);
          if (request.serial != nextRequestSerial) {
            displayCacheTileAborts.fetch_add(1u, std::memory_order_relaxed);
            stale = true;
            break;
          }
        }
        {
          std::lock_guard<std::mutex> lock(cacheDataMutex);
          if (displayTileCache.mode != request.mode ||
              std::fabs(displayTileCache.zoom - request.zoom) > 1e-5f ||
              std::fabs(displayTileCache.centerX - targetCacheCenterX) > 1e-5f ||
              std::fabs(displayTileCache.centerY - targetCacheCenterY) > 1e-5f) {
            continue;
          }
          DisplayCacheTile* slot = nullptr;
          for (DisplayCacheTile& tile : displayTileCache.tiles) {
            if (!tile.valid) {
              slot = &tile;
              break;
            }
          }
          if (!slot) continue;
          slot->x = tileX;
          slot->y = tileY;
          slot->width = tileSource.width;
          slot->height = tileSource.height;
          if (slot->rgb8.size() < tileSource.rgb8.size()) {
            slot->rgb8.resize(size_t(kDisplayTileSize) * size_t(kDisplayTileSize) * 3u);
          }
          std::copy(tileSource.rgb8.begin(), tileSource.rgb8.end(), slot->rgb8.begin());
          slot->valid = true;
          displayTileCache.writeTileToStitched(*slot);
          if (displayTileCache.validTileCount() > 0u &&
              displayTileCache.stitchedRgb8.size() == size_t(kFractalCacheWidth) * size_t(kFractalCacheHeight) * 3u) {
            displayPresentationCache.ensureStorage(
              displayTileCache.stitchedWidth, displayTileCache.stitchedHeight, kDisplayTileSize);
            displayPresentationCache.mode = displayTileCache.mode;
            displayPresentationCache.zoom = displayTileCache.zoom;
            displayPresentationCache.centerX = displayTileCache.centerX;
            displayPresentationCache.centerY = displayTileCache.centerY;
            displayPresentationCache.cacheScale = kFractalCacheScale;
            displayPresentationCache.rgb8 = displayTileCache.stitchedRgb8;
            std::fill(displayPresentationCache.tileValid.begin(), displayPresentationCache.tileValid.end(), 0u);
            const int cols = displayPresentationCache.columns();
            for (const DisplayCacheTile& tile : displayTileCache.tiles) {
              if (!tile.valid) continue;
              const int column = tile.x / kDisplayTileSize;
              const int row = tile.y / kDisplayTileSize;
              const size_t index = size_t(row) * size_t(cols) + size_t(column);
              if (index < displayPresentationCache.tileValid.size()) {
                displayPresentationCache.tileValid[index] = 1u;
              }
            }
          }
        }
        renderedAnyTile = true;
        displayCacheTilesRendered.fetch_add(1u, std::memory_order_relaxed);
        ++renderedTilesSincePublish;
        if (renderedTilesSincePublish >= 4) {
          publishDisplayCacheComposite(request, true);
          renderedTilesSincePublish = 0;
        }
    }
    if (renderedAnyTile && !stale) {
      displayCacheRendersCompleted.fetch_add(1u, std::memory_order_relaxed);
      publishDisplayCacheComposite(request, true);
    }

    if (stale) {
      continue;
    }

    if (!gpuPreviewOwnsDisplay) {
      renderZoomAheadCaches(request);
    }
  }
}

void Nautiloid::irisWorkerLoop() {
  while (true) {
    WorkerRequest request;
    {
      std::unique_lock<std::mutex> lock(irisRequestMutex);
      irisRequestCv.wait(lock, [this]() { return irisWorkerStop || irisRequestPending; });
      if (irisWorkerStop) break;
      request = irisRequest;
      irisRequestPending = false;
    }

    bool irisCompatibleCurrent = false;
    {
      std::lock_guard<std::mutex> lock(snapshotMutex);
      irisCompatibleCurrent =
        irisCompatibleSource.valid() &&
        irisCompatibleMode == request.mode &&
        std::fabs(irisCompatibleZoom - request.zoom) <= 1e-5f &&
        std::fabs(irisCompatibleCenterX - request.centerX) <= 1e-12 &&
        std::fabs(irisCompatibleCenterY - request.centerY) <= 1e-12;
    }
    if (irisCompatibleCurrent) {
      continue;
    }

    iris::SourceField nextIrisSource;
    std::string irisError;
    iris::NautiloidFractalSourceParams sourceParams;
    sourceParams.mode = request.mode;
    sourceParams.zoom = request.zoom;
    sourceParams.centerX = request.centerX;
    sourceParams.centerY = request.centerY;
    sourceParams.generation = request.serial;
    const bool irisOk = iris::makeNautiloidIrisSource(sourceParams, &nextIrisSource, &irisError);
    if (!irisOk) {
      continue;
    }

    std::shared_ptr<const iris::SourceField> nextSharedSource =
      std::make_shared<const iris::SourceField>(nextIrisSource);
    std::lock_guard<std::mutex> lock(snapshotMutex);
    irisCompatibleSource = std::move(nextIrisSource);
    std::atomic_store_explicit(&irisExpanderSource, nextSharedSource, std::memory_order_release);
    irisCompatibleSerial = request.serial;
    irisCompatibleMode = request.mode;
    irisCompatibleZoom = request.zoom;
    irisCompatibleCenterX = request.centerX;
    irisCompatibleCenterY = request.centerY;
    irisPreviewGeneration.fetch_add(1u, std::memory_order_release);
    irisRendersCompleted.fetch_add(1u, std::memory_order_relaxed);
  }
}
