#include "Nautiloid.hpp"
#include "Iris.hpp"

#include <algorithm>
#include <memory>
#include <utility>

namespace {

constexpr float kNautiloidMaxFractalZoom = 5.f;
constexpr int kDisplaySourceWidth = 768;
constexpr int kDisplaySourceHeight = 512;
constexpr float kFractalCacheScale = 3.f;
constexpr int kFractalCacheWidth = int(float(kDisplaySourceWidth) * kFractalCacheScale);
constexpr int kFractalCacheHeight = int(float(kDisplaySourceHeight) * kFractalCacheScale);
constexpr int kDisplayTileSize = 128;

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
    case iris::FRACTAL_EYE_OF_THE_WORLD:
      return Vec(0.0075f, 0.00395f);
    case iris::FRACTAL_TRICORN:
    default:
      return Vec(1.68f, 0.90f);
  }
}

int jsonIntegerOr(json_t* root, const char* key, int fallback) {
  json_t* value = json_object_get(root, key);
  return value ? int(json_integer_value(value)) : fallback;
}

float jsonRealOr(json_t* root, const char* key, float fallback) {
  json_t* value = json_object_get(root, key);
  return value ? float(json_number_value(value)) : fallback;
}

bool jsonBoolOr(json_t* root, const char* key, bool fallback) {
  json_t* value = json_object_get(root, key);
  return value ? json_boolean_value(value) != 0 : fallback;
}

const Nautiloid::DisplayCacheTile* findDisplayTile(
  const Nautiloid::DisplayTileCache& cache,
  int x,
  int y) {
  for (const Nautiloid::DisplayCacheTile& tile : cache.tiles) {
    if (tile.valid &&
        x >= tile.x && y >= tile.y &&
        x < tile.x + tile.width &&
        y < tile.y + tile.height) {
      return &tile;
    }
  }
  return nullptr;
}

bool cropDisplayTileCacheToSize(
  const Nautiloid::DisplayTileCache& cache,
  int mode,
  float zoom,
  float centerX,
  float centerY,
  float cacheScale,
  int outWidth,
  int outHeight,
  iris::SourceField* out) {
  if (!out || cacheScale <= 1.f || outWidth <= 1 || outHeight <= 1) return false;
  if (cache.validTileCount() == 0u || cache.mode != mode || std::fabs(cache.zoom - zoom) > 1e-5f) return false;
  const float zoomScale = std::pow(0.05f, clamp(zoom, 0.f, kNautiloidMaxFractalZoom));
  const Vec halfSpan = nautiloidFractalViewportHalfSpan(mode).mult(zoomScale);
  const float marginX = (cacheScale - 1.f) * halfSpan.x;
  const float marginY = (cacheScale - 1.f) * halfSpan.y;
  const float dx = centerX - cache.centerX;
  const float dy = centerY - cache.centerY;
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
  source.rgb8.assign(size_t(source.width) * size_t(source.height) * 3u, 0u);

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

  for (int y = 0; y < source.height; ++y) {
    const float srcY = cropTop + (float(y) + 0.5f) * cropH / float(source.height) - 0.5f;
    const int srcYi = clamp(int(std::round(srcY)), 0, kFractalCacheHeight - 1);
    for (int x = 0; x < source.width; ++x) {
      const float srcX = cropLeft + (float(x) + 0.5f) * cropW / float(source.width) - 0.5f;
      const int srcXi = clamp(int(std::round(srcX)), 0, kFractalCacheWidth - 1);
      const Nautiloid::DisplayCacheTile* tile = findDisplayTile(cache, srcXi, srcYi);
      if (!tile || !tile->valid) return false;
      const size_t outBase = (size_t(y) * size_t(source.width) + size_t(x)) * 3u;
      const int tileX = srcXi - tile->x;
      const int tileY = srcYi - tile->y;
      const size_t inBase = (size_t(tileY) * size_t(tile->width) + size_t(tileX)) * 3u;
      source.rgb8[outBase + 0u] = tile->rgb8[inBase + 0u];
      source.rgb8[outBase + 1u] = tile->rgb8[inBase + 1u];
      source.rgb8[outBase + 2u] = tile->rgb8[inBase + 2u];
    }
  }

  *out = std::move(source);
  return true;
}

bool displayTileCacheCoversView(
  const Nautiloid::DisplayTileCache& cache,
  int mode,
  float zoom,
  float centerX,
  float centerY,
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
  float centerX,
  float centerY,
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
  float requestedCenterX,
  float requestedCenterY,
  float cacheScale,
  float* alignedCenterX,
  float* alignedCenterY,
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
  const float targetX = cache.centerX + float(*shiftColumns) * tileWorldX;
  const float targetY = cache.centerY + float(*shiftRows) * tileWorldY;
  *alignedCenterX = clamp(targetX, -2.f, 2.f);
  *alignedCenterY = clamp(targetY, -2.f, 2.f);
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

} // namespace

void Nautiloid::DisplayTileCache::clear() {
  mode = iris::FRACTAL_NONE;
  zoom = -1.f;
  centerX = 0.f;
  centerY = 0.f;
  for (DisplayCacheTile& tile : tiles) {
    tile.valid = false;
  }
}

void Nautiloid::DisplayTileCache::ensureStorage(int cacheWidth, int cacheHeight, int tileSize) {
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
  return root;
}

void Nautiloid::dataFromJson(json_t* root) {
  if (!root) return;
  const int mode = jsonIntegerOr(root, "fractalMode", iris::FRACTAL_MANDELBROT);
  fractalMode = iris::isBuiltinFractalMode(mode) ? mode : iris::FRACTAL_MANDELBROT;
  fractalZoom = clamp(jsonRealOr(root, "fractalZoom", 0.f), 0.f, kNautiloidMaxFractalZoom);
  fractalCenterX = clamp(jsonRealOr(root, "fractalCenterX", 0.f), -2.f, 2.f);
  fractalCenterY = clamp(jsonRealOr(root, "fractalCenterY", 0.f), -2.f, 2.f);
  debugFileLoggingEnabled.store(
    jsonBoolOr(root, "debugFileLoggingEnabled", false), std::memory_order_relaxed);
  requestRender();
}

void Nautiloid::requestFractal(int mode) {
  if (!iris::isBuiltinFractalMode(mode)) return;
  if (fractalMode != mode) {
    fractalMode = mode;
    fractalZoom = 0.f;
    fractalCenterX = 0.f;
    fractalCenterY = 0.f;
  }
  requestRender();
}

void Nautiloid::requestRender() {
  requestRenderWithCacheCenter(fractalCenterX, fractalCenterY);
}

void Nautiloid::requestRenderWithCacheCenter(float cacheCenterX, float cacheCenterY, bool forceCacheRecenter) {
  WorkerRequest request;
  request.mode = fractalMode;
  request.zoom = clamp(fractalZoom, 0.f, kNautiloidMaxFractalZoom);
  request.centerX = clamp(fractalCenterX, -2.f, 2.f);
  request.centerY = clamp(fractalCenterY, -2.f, 2.f);
  request.cacheCenterX = clamp(cacheCenterX, -2.f, 2.f);
  request.cacheCenterY = clamp(cacheCenterY, -2.f, 2.f);
  request.forceCacheRecenter = forceCacheRecenter;
  submitRequest(request);
}

void Nautiloid::requestRenderWithCenteredCache() {
  requestRenderWithCacheCenter(fractalCenterX, fractalCenterY, true);
}

void Nautiloid::resetView() {
  fractalZoom = 0.f;
  fractalCenterX = 0.f;
  fractalCenterY = 0.f;
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
  next.tileCurrent.assign(size_t(next.columns) * size_t(next.rows), 0u);

  std::lock_guard<std::mutex> lock(cacheDataMutex);
  next.current =
    displayTileCache.mode == fractalMode &&
    std::fabs(displayTileCache.zoom - clamp(fractalZoom, 0.f, kNautiloidMaxFractalZoom)) <= 1e-5f;
  next.cacheCenterX = displayTileCache.centerX;
  next.cacheCenterY = displayTileCache.centerY;
  if (next.current) {
    for (const DisplayCacheTile& tile : displayTileCache.tiles) {
      if (!tile.valid) continue;
      const int column = tile.x / kDisplayTileSize;
      const int row = tile.y / kDisplayTileSize;
      if (column >= 0 && row >= 0 && column < next.columns && row < next.rows) {
        next.tileCurrent[size_t(row) * size_t(next.columns) + size_t(column)] = 1u;
      }
    }
  }

  *snapshot = std::move(next);
}

void Nautiloid::startWorker() {
  workerStop = false;
  cacheWorkerStop = false;
  worker = std::thread([this]() { workerLoop(); });
  cacheWorker = std::thread([this]() { cacheWorkerLoop(); });
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
  if (worker.joinable()) {
    worker.join();
  }
  if (cacheWorker.joinable()) {
    cacheWorker.join();
  }
}

void Nautiloid::submitRequest(const WorkerRequest& request) {
  {
    std::lock_guard<std::mutex> lock(workerMutex);
    workerRequest = request;
    workerRequest.serial = ++nextRequestSerial;
    renderRequestsSubmitted.fetch_add(1u, std::memory_order_relaxed);
    requestPending = true;
    loading.store(true, std::memory_order_release);
  }
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

    iris::SourceField source;
    std::string error;
    bool ok = false;
    {
      std::lock_guard<std::mutex> lock(cacheDataMutex);
      ok = cropDisplayTileCacheToSize(
          displayTileCache,
          request.mode,
          request.zoom,
          request.centerX,
          request.centerY,
          kFractalCacheScale,
          kDisplaySourceWidth,
          kDisplaySourceHeight,
          &source);
    }
    if (ok) {
      displayCacheHits.fetch_add(1u, std::memory_order_relaxed);
    } else {
      displayCacheMisses.fetch_add(1u, std::memory_order_relaxed);
    }
    if (!ok) {
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
    }

    if (!ok) {
      loading.store(false, std::memory_order_release);
      continue;
    }

    {
      std::lock_guard<std::mutex> lock(workerMutex);
      if (request.serial != nextRequestSerial) {
        if (!(requestPending && workerRequest.mode == request.mode)) {
          displayRendersDroppedStale.fetch_add(1u, std::memory_order_relaxed);
          continue;
        }
      }
    }
    {
      std::lock_guard<std::mutex> lock(snapshotMutex);
      previewSource = std::move(source);
    }
    previewGeneration.fetch_add(1u, std::memory_order_release);
    displayRendersCompleted.fetch_add(1u, std::memory_order_relaxed);
    loading.store(false, std::memory_order_release);
    submitCacheRequest(request);
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

    bool irisCompatibleCurrent = false;
    {
      std::lock_guard<std::mutex> lock(snapshotMutex);
      irisCompatibleCurrent =
        irisCompatibleSource.valid() &&
        irisCompatibleSerial == request.serial;
    }
    if (!irisCompatibleCurrent) {
      iris::SourceField nextIrisSource;
      std::string irisError;
      iris::NautiloidFractalSourceParams sourceParams;
      sourceParams.mode = request.mode;
      sourceParams.zoom = request.zoom;
      sourceParams.centerX = request.centerX;
      sourceParams.centerY = request.centerY;
      sourceParams.generation = request.serial;
      const bool irisOk = iris::makeNautiloidIrisSource(sourceParams, &nextIrisSource, &irisError);
      if (irisOk) {
        std::lock_guard<std::mutex> workerLock(workerMutex);
        if (request.serial == nextRequestSerial) {
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
        } else {
          irisRendersDroppedStale.fetch_add(1u, std::memory_order_relaxed);
        }
      }
    }

    float targetCacheCenterX = request.cacheCenterX;
    float targetCacheCenterY = request.cacheCenterY;
    {
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
        continue;
      }
      if (displayTileCache.mode != request.mode ||
          std::fabs(displayTileCache.zoom - request.zoom) > 1e-5f ||
          std::fabs(displayTileCache.centerX - targetCacheCenterX) > 1e-5f ||
          std::fabs(displayTileCache.centerY - targetCacheCenterY) > 1e-5f) {
        const bool canShiftExistingTiles =
          targetTileAligned &&
          displayTileCache.mode == request.mode &&
          std::fabs(displayTileCache.zoom - request.zoom) <= 1e-5f;
        if (canShiftExistingTiles) {
          shiftDisplayTileCacheTiles(&displayTileCache, shiftColumns, shiftRows);
          displayTileCache.centerX = targetCacheCenterX;
          displayTileCache.centerY = targetCacheCenterY;
        } else {
          displayTileCache.clear();
          displayTileCache.mode = request.mode;
          displayTileCache.zoom = request.zoom;
          displayTileCache.centerX = targetCacheCenterX;
          displayTileCache.centerY = targetCacheCenterY;
        }
      }
    }

    bool stale = false;
    bool renderedAnyTile = false;
    const float visibleCenterX = (0.5f + (request.centerX - targetCacheCenterX) /
      (2.f * nautiloidFractalViewportHalfSpan(request.mode).mult(
        std::pow(0.05f, clamp(request.zoom, 0.f, kNautiloidMaxFractalZoom))).x * kFractalCacheScale)) *
      float(kFractalCacheWidth);
    const float visibleCenterY = (0.5f + (request.centerY - targetCacheCenterY) /
      (2.f * nautiloidFractalViewportHalfSpan(request.mode).mult(
        std::pow(0.05f, clamp(request.zoom, 0.f, kNautiloidMaxFractalZoom))).y * kFractalCacheScale)) *
      float(kFractalCacheHeight);
    const std::vector<Vec> tileOrder = makeDisplayTileOrder(visibleCenterX, visibleCenterY);
    for (const Vec& tilePos : tileOrder) {
      if (stale) break;
      const int tileX = int(tilePos.x);
      const int tileY = int(tilePos.y);
        {
          std::lock_guard<std::mutex> lock(workerMutex);
          if (request.serial != nextRequestSerial) {
            displayRendersDroppedStale.fetch_add(1u, std::memory_order_relaxed);
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
            displayRendersDroppedStale.fetch_add(1u, std::memory_order_relaxed);
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
        }
        renderedAnyTile = true;
    }
    if (renderedAnyTile && !stale) {
      displayCacheRendersCompleted.fetch_add(1u, std::memory_order_relaxed);
    }
  }
}
