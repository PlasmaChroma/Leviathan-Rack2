#include "Nautiloid.hpp"

#include <utility>

namespace {

constexpr float kNautiloidMaxFractalZoom = 5.f;
constexpr float kFractalCacheScale = 1.5f;
constexpr int kFractalCacheWidth = int(float(iris::kCanonicalSourceWidth) * kFractalCacheScale);
constexpr int kFractalCacheHeight = int(float(iris::kCanonicalSourceHeight) * kFractalCacheScale);

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

bool cropFractalCacheToCanonical(
  const iris::SourceField& cache,
  float cacheCenterX,
  float cacheCenterY,
  int mode,
  float zoom,
  float centerX,
  float centerY,
  float cacheScale,
  iris::SourceField* out) {
  if (!cache.valid() || !out || cacheScale <= 1.f) return false;
  const float zoomScale = std::pow(0.05f, clamp(zoom, 0.f, kNautiloidMaxFractalZoom));
  const Vec halfSpan = nautiloidFractalViewportHalfSpan(mode).mult(zoomScale);
  const float marginX = (cacheScale - 1.f) * halfSpan.x;
  const float marginY = (cacheScale - 1.f) * halfSpan.y;
  const float dx = centerX - cacheCenterX;
  const float dy = centerY - cacheCenterY;
  if (std::fabs(dx) > marginX || std::fabs(dy) > marginY) return false;

  iris::SourceField source;
  source.width = iris::kCanonicalSourceWidth;
  source.height = iris::kCanonicalSourceHeight;
  source.channels = iris::kCanonicalSourceChannels;
  source.bitDepth = iris::kCanonicalSourceBitDepth;
  source.originalWidth = source.width;
  source.originalHeight = source.height;
  source.originalChannels = source.channels;
  source.sourceName = cache.sourceName;
  source.rgb8.assign(size_t(source.width) * size_t(source.height) * 3u, 0u);

  const float cacheHalfX = halfSpan.x * cacheScale;
  const float cacheHalfY = halfSpan.y * cacheScale;
  const float cacheCenterPx = (0.5f + dx / (2.f * cacheHalfX)) * float(cache.width);
  const float cacheCenterPy = (0.5f + dy / (2.f * cacheHalfY)) * float(cache.height);
  const float cropW = float(cache.width) / cacheScale;
  const float cropH = float(cache.height) / cacheScale;
  const float cropLeft = cacheCenterPx - 0.5f * cropW;
  const float cropTop = cacheCenterPy - 0.5f * cropH;
  if (cropLeft < -0.01f || cropTop < -0.01f ||
      cropLeft + cropW > float(cache.width) + 0.01f ||
      cropTop + cropH > float(cache.height) + 0.01f) {
    return false;
  }

  for (int y = 0; y < source.height; ++y) {
    const float srcY = cropTop + (float(y) + 0.5f) * cropH / float(source.height) - 0.5f;
    const int y0 = clamp(int(std::floor(srcY)), 0, cache.height - 1);
    const int y1 = std::min(y0 + 1, cache.height - 1);
    const float fy = clamp(srcY - float(y0), 0.f, 1.f);
    for (int x = 0; x < source.width; ++x) {
      const float srcX = cropLeft + (float(x) + 0.5f) * cropW / float(source.width) - 0.5f;
      const int x0 = clamp(int(std::floor(srcX)), 0, cache.width - 1);
      const int x1 = std::min(x0 + 1, cache.width - 1);
      const float fx = clamp(srcX - float(x0), 0.f, 1.f);
      const size_t outBase = (size_t(y) * size_t(source.width) + size_t(x)) * 3u;
      for (int c = 0; c < 3; ++c) {
        const auto component = [&](int px, int py) {
          return float(cache.rgb8[(size_t(py) * size_t(cache.width) + size_t(px)) * 3u + size_t(c)]);
        };
        const float top = component(x0, y0) + (component(x1, y0) - component(x0, y0)) * fx;
        const float bottom = component(x0, y1) + (component(x1, y1) - component(x0, y1)) * fx;
        source.rgb8[outBase + size_t(c)] =
          uint8_t(std::round(clamp(top + (bottom - top) * fy, 0.f, 255.f)));
      }
    }
  }

  *out = std::move(source);
  return true;
}

} // namespace

Nautiloid::Nautiloid() {
  config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
  configButton(SOURCE_MENU_PARAM, "Fractal");
  configButton(RESET_VIEW_PARAM, "Reset view");
  startWorker();
  requestRender();
}

Nautiloid::~Nautiloid() {
  stopWorker();
}

void Nautiloid::process(const ProcessArgs& args) {
}

json_t* Nautiloid::dataToJson() {
  json_t* root = json_object();
  json_object_set_new(root, "fractalMode", json_integer(fractalMode));
  json_object_set_new(root, "fractalZoom", json_real(fractalZoom));
  json_object_set_new(root, "fractalCenterX", json_real(fractalCenterX));
  json_object_set_new(root, "fractalCenterY", json_real(fractalCenterY));
  return root;
}

void Nautiloid::dataFromJson(json_t* root) {
  if (!root) return;
  const int mode = jsonIntegerOr(root, "fractalMode", iris::FRACTAL_MANDELBROT);
  fractalMode = iris::isBuiltinFractalMode(mode) ? mode : iris::FRACTAL_MANDELBROT;
  fractalZoom = clamp(jsonRealOr(root, "fractalZoom", 0.f), 0.f, kNautiloidMaxFractalZoom);
  fractalCenterX = clamp(jsonRealOr(root, "fractalCenterX", 0.f), -2.f, 2.f);
  fractalCenterY = clamp(jsonRealOr(root, "fractalCenterY", 0.f), -2.f, 2.f);
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
  WorkerRequest request;
  request.mode = fractalMode;
  request.zoom = clamp(fractalZoom, 0.f, kNautiloidMaxFractalZoom);
  request.centerX = clamp(fractalCenterX, -2.f, 2.f);
  request.centerY = clamp(fractalCenterY, -2.f, 2.f);
  submitRequest(request);
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
      const bool cacheCompatible =
        fractalCacheSource.valid() &&
        fractalCacheMode == request.mode &&
        std::fabs(fractalCacheZoom - request.zoom) <= 1e-5f;
      ok = cacheCompatible &&
        cropFractalCacheToCanonical(
          fractalCacheSource,
          fractalCacheCenterX,
          fractalCacheCenterY,
          request.mode,
          request.zoom,
          request.centerX,
          request.centerY,
          kFractalCacheScale,
          &source);
    }
    if (!ok) {
      ok = iris::makeBuiltinFractalSourceSized(
        request.mode,
        request.zoom,
        request.centerX,
        request.centerY,
        iris::kCanonicalSourceWidth,
        iris::kCanonicalSourceHeight,
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
          continue;
        }
      }
    }
    {
      std::lock_guard<std::mutex> lock(snapshotMutex);
      previewSource = std::move(source);
    }
    previewGeneration.fetch_add(1u, std::memory_order_release);
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
    }

    {
      std::lock_guard<std::mutex> lock(cacheDataMutex);
      if (fractalCacheSource.valid() &&
          fractalCacheMode == request.mode &&
          std::fabs(fractalCacheZoom - request.zoom) <= 1e-5f &&
          std::fabs(fractalCacheCenterX - request.centerX) <= 1e-5f &&
          std::fabs(fractalCacheCenterY - request.centerY) <= 1e-5f) {
        continue;
      }
    }

    iris::SourceField nextCache;
    std::string error;
    const bool ok = iris::makeBuiltinFractalSourceSized(
      request.mode,
      request.zoom,
      request.centerX,
      request.centerY,
      kFractalCacheWidth,
      kFractalCacheHeight,
      kFractalCacheScale,
      &nextCache,
      &error);
    if (!ok) continue;

    {
      std::lock_guard<std::mutex> lock(workerMutex);
      if (request.serial != nextRequestSerial) {
        continue;
      }
    }
    {
      std::lock_guard<std::mutex> lock(cacheDataMutex);
      fractalCacheSource = std::move(nextCache);
      fractalCacheMode = request.mode;
      fractalCacheZoom = request.zoom;
      fractalCacheCenterX = request.centerX;
      fractalCacheCenterY = request.centerY;
    }
  }
}
