#include "../src/Iris.hpp"

#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#define QOI_IMPLEMENTATION
#include "../src/third_party/qoi.h"

Model* modelNautiloid = nullptr;
Model* modelChromatide = nullptr;

bool isDragonKingDebugEnabled() {
  return false;
}

struct IrisPhase4TestAccess {
  static std::shared_ptr<const iris::SourceField> seedRetainedSource(
      Iris& module, std::shared_ptr<const iris::SourceField> source) {
    std::lock_guard<std::mutex> lock(module.snapshotMutex);
    module.snapshotSource = std::move(source);
    return module.snapshotSource;
  }

  static void requestMissingFileReload(
      Iris& module, const std::shared_ptr<const iris::SourceField>& source) {
    Iris::WorkerRequest request;
    request.type = Iris::REQUEST_RELOAD_IMAGE_FILE;
    request.path = "/tmp/leviathan-iris-definitely-missing.png";
    request.source = source;
    request.settings = module.conversionSettings;
    request.settings.frameSize = 64;
    request.settings.rows = 8;
    module.submitRequest(request);
  }

  static const iris::SourceField* snapshotSourceIdentity(Iris& module) {
    std::lock_guard<std::mutex> lock(module.snapshotMutex);
    return module.snapshotSource.get();
  }
};

namespace {

int failures = 0;

void check(const std::string& name, bool condition) {
  std::cout << (condition ? "[PASS] " : "[FAIL] ") << name << "\n";
  if (!condition) ++failures;
}

bool waitUntil(const std::function<bool()>& predicate) {
  for (int attempt = 0; attempt < 400; ++attempt) {
    if (predicate()) return true;
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
  return false;
}

std::shared_ptr<const iris::SourceField> makeRetainedSource() {
  auto source = std::make_shared<iris::SourceField>();
  source->width = iris::kCanonicalSourceWidth;
  source->height = iris::kCanonicalSourceHeight;
  source->channels = iris::kCanonicalSourceChannels;
  source->bitDepth = iris::kCanonicalSourceBitDepth;
  source->sourcePath = "/tmp/original-iris-image.png";
  source->sourceName = "original-iris-image.png";
  source->rgb8.resize(
    size_t(source->width) * size_t(source->height) * size_t(source->channels));
  for (int y = 0; y < source->height; ++y) {
    const uint8_t level = uint8_t((255 * y) / std::max(source->height - 1, 1));
    for (int x = 0; x < source->width; ++x) {
      const size_t base = size_t(y * source->width + x) * 3u;
      source->rgb8[base + 0u] = level;
      source->rgb8[base + 1u] = level;
      source->rgb8[base + 2u] = level;
    }
  }
  return source;
}

} // namespace

int main() {
  Iris module;
  const auto retainedSource = makeRetainedSource();
  IrisPhase4TestAccess::seedRetainedSource(module, retainedSource);

  const uint64_t initialGeneration = module.previewGeneration.load(std::memory_order_acquire);
  IrisPhase4TestAccess::requestMissingFileReload(module, retainedSource);
  const bool partialFinished = waitUntil([&]() {
    return module.previewGeneration.load(std::memory_order_acquire) > initialGeneration &&
      !module.loading.load(std::memory_order_acquire);
  });
  check("missing-file reload rebuilds and publishes the retained source", partialFinished);
  check("partial reload success keeps its read error visible",
        module.loadFailed.load(std::memory_order_acquire) && module.statusText() == "Load failed");
  check("partial reload reuses the immutable retained source",
        IrisPhase4TestAccess::snapshotSourceIdentity(module) == retainedSource.get());

  std::vector<uint8_t> previewBeforeFailure;
  std::vector<float> waveformBeforeFailure;
  int previewWidth = 0;
  int previewHeight = 0;
  module.previewSnapshot(&previewBeforeFailure, &previewWidth, &previewHeight);
  module.waveformSnapshot(0.5f, 65, &waveformBeforeFailure);
  const std::string sourceNameBeforeFailure = module.sourceName();
  const uint64_t generationBeforeFailure =
    module.previewGeneration.load(std::memory_order_acquire);

  module.requestImageLoad("/tmp/leviathan-iris-still-definitely-missing.png");
  const bool failureFinished = waitUntil([&]() {
    return !module.loading.load(std::memory_order_acquire) &&
      module.loadFailed.load(std::memory_order_acquire);
  });
  std::vector<uint8_t> previewAfterFailure;
  std::vector<float> waveformAfterFailure;
  module.previewSnapshot(&previewAfterFailure, nullptr, nullptr);
  module.waveformSnapshot(0.5f, 65, &waveformAfterFailure);

  check("ordinary load failure completes with an error", failureFinished);
  check("ordinary load failure does not publish a new generation",
        module.previewGeneration.load(std::memory_order_acquire) == generationBeforeFailure);
  check("ordinary load failure preserves source, preview, and wavetable snapshots",
        module.sourceName() == sourceNameBeforeFailure &&
        previewAfterFailure == previewBeforeFailure &&
        waveformAfterFailure == waveformBeforeFailure);

  if (failures != 0) {
    std::cerr << failures << " Iris Phase 4 checks failed\n";
    return 1;
  }
  std::cout << "Iris Phase 4 checks passed\n";
  return 0;
}
