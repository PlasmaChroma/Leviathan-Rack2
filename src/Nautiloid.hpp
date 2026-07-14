#pragma once

#include "plugin.hpp"
#include "NautiloidFractal.hpp"
#include "NautiloidIrisExpander.hpp"

#include <atomic>
#include <array>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

template <typename T>
struct NautiloidAtomicValue {
  std::atomic<T> value;

  NautiloidAtomicValue(T initial = T()) : value(initial) {}
  NautiloidAtomicValue(const NautiloidAtomicValue&) = delete;
  NautiloidAtomicValue& operator=(const NautiloidAtomicValue&) = delete;
  operator T() const { return value.load(std::memory_order_acquire); }
  NautiloidAtomicValue& operator=(T next) {
    value.store(next, std::memory_order_release);
    return *this;
  }
};

struct Nautiloid final : Module {
  enum ParamId {
    SOURCE_MENU_PARAM,
    RESET_VIEW_PARAM,
    PARAMS_LEN
  };

  enum InputId {
    ZOOM_RATE_INPUT,
    INPUTS_LEN
  };

  enum OutputId {
    OUTPUTS_LEN
  };

  enum LightId {
    IRIS_LINK_LIGHT,
    IRIS_READY_LIGHT,
    LIGHTS_LEN
  };

  Nautiloid();
  ~Nautiloid() override;

  void process(const ProcessArgs& args) override;
  json_t* dataToJson() override;
  void dataFromJson(json_t* root) override;

  void requestFractal(int mode);
  void requestRender();
  void requestRenderWithCacheCenter(double cacheCenterX, double cacheCenterY, bool forceCacheRecenter = false);
  void requestRenderWithCenteredCache();
  void requestInteractiveZoomPreview(double cacheCenterX, double cacheCenterY, bool forceCacheRecenter = false);
  void requestIrisSourceSync();
  void resetView();
  void previewSnapshot(std::vector<uint8_t>* rgb, int* width, int* height) const;
  void irisPreviewSnapshot(std::vector<uint8_t>* rgb, int* width, int* height) const;
  const nautiloid_iris_expander::SourceSlot* irisExpanderSourceSlotSnapshot(uint64_t* generation) const;

  struct DisplayTileCacheSnapshot {
    int columns = 0;
    int rows = 0;
    int tileSize = 0;
    int cacheWidth = 0;
    int cacheHeight = 0;
    float cacheScale = 1.f;
    bool current = false;
    int cacheMode = iris::FRACTAL_NONE;
    float cacheZoom = -1.f;
    double cacheCenterX = 0.0;
    double cacheCenterY = 0.0;
    size_t currentTileCount = 0u;
    size_t fullTileCount = 0u;
    std::vector<uint8_t> tileCurrent;
  };

  void displayTileCacheSnapshot(DisplayTileCacheSnapshot* snapshot) const;
  struct ZoomAheadCacheSnapshot {
    std::array<size_t, 3> currentTileCount {};
    std::array<size_t, 3> fullTileCount {};
    std::array<float, 3> zoom {};
  };

  void zoomAheadCacheSnapshot(ZoomAheadCacheSnapshot* snapshot) const;

  struct FractalState {
    int mode = iris::FRACTAL_MANDELBROT;
    float zoom = 0.f;
    double centerX = 0.0;
    double centerY = 0.0;
  };

  FractalState fractalStateSnapshot() const;
  void setFractalState(const FractalState& state);

  NautiloidAtomicValue<int> fractalMode {iris::FRACTAL_MANDELBROT};
  NautiloidAtomicValue<float> fractalZoom {0.f};
  NautiloidAtomicValue<double> fractalCenterX {0.0};
  NautiloidAtomicValue<double> fractalCenterY {0.0};
  std::atomic<uint64_t> previewGeneration {0u};
  std::atomic<uint64_t> irisPreviewGeneration {0u};
  std::atomic<uint64_t> renderRequestsSubmitted {0u};
  std::atomic<uint64_t> displayRendersCompleted {0u};
  std::atomic<uint64_t> displayRendersDroppedStale {0u};
  std::atomic<uint64_t> displayCacheHits {0u};
  std::atomic<uint64_t> displayCachePartialHits {0u};
  std::atomic<uint64_t> displayCacheMisses {0u};
  std::atomic<uint64_t> cacheRequestsSubmitted {0u};
  std::atomic<uint64_t> cacheRequestsDequeued {0u};
  std::atomic<uint64_t> displayCacheRendersCompleted {0u};
  std::atomic<uint64_t> displayCacheCompositePublishes {0u};
  std::atomic<uint64_t> displayCacheTilesRendered {0u};
  std::atomic<uint64_t> displayCacheTileAborts {0u};
  std::atomic<uint64_t> displayTileCacheResets {0u};
  std::atomic<uint64_t> displayTileCacheShifts {0u};
  std::atomic<uint64_t> displayReprojectionPublishes {0u};
  std::atomic<uint64_t> displayReprojectionZoomAheadHits {0u};
  std::atomic<uint64_t> zoomAheadTilesRendered {0u};
  std::atomic<uint64_t> irisRendersCompleted {0u};
  std::atomic<uint64_t> irisRendersDroppedStale {0u};
  std::atomic<uint64_t> irisExpanderPublishes {0u};
  std::atomic<bool> debugFileLoggingEnabled {false};
  std::atomic<bool> debugGpuPreviewEnabled {true};
  std::atomic<bool> debugGpuPreviewAvailable {false};
  std::atomic<bool> zoomInteractionActive {false};
  std::atomic<bool> displayRenderBusy {false};
  std::atomic<bool> forceIrisSourceSync {false};
  std::atomic<bool> loading {false};
  std::atomic<float> zoomRateCvNorm {0.f};
  std::atomic<bool> zoomRateCvConnected {false};

  struct DisplayCacheTile {
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    bool valid = false;
    std::vector<uint8_t> rgb8;
  };

  struct DisplayTileCache {
    int mode = iris::FRACTAL_NONE;
    float zoom = -1.f;
    double centerX = 0.0;
    double centerY = 0.0;
    std::vector<DisplayCacheTile> tiles;
    int stitchedWidth = 0;
    int stitchedHeight = 0;
    std::vector<uint8_t> stitchedRgb8;

    void clear();
    void ensureStorage(int cacheWidth, int cacheHeight, int tileSize);
    void writeTileToStitched(const DisplayCacheTile& tile);
    size_t validTileCount() const;
  };

  struct PresentationLayer {
    int mode = iris::FRACTAL_NONE;
    float zoom = -1.f;
    double centerX = 0.0;
    double centerY = 0.0;
    int width = 0;
    int height = 0;
    float cacheScale = 1.f;
    int tileSize = 0;
    std::vector<uint8_t> rgb8;
    std::vector<uint8_t> tileValid;

    bool valid() const;
    void clear();
    void ensureStorage(int layerWidth, int layerHeight, int layerTileSize);
    int columns() const;
    int rows() const;
    size_t fullTileCount() const;
    size_t validTileCount() const;
    bool tileCoversPixel(int x, int y) const;
    void writeTile(int tileX, int tileY, int tileW, int tileH, const std::vector<uint8_t>& tileRgb8);
  };

private:
  struct WorkerRequest {
    int mode = iris::FRACTAL_MANDELBROT;
    float zoom = 0.f;
    double centerX = 0.0;
    double centerY = 0.0;
    double cacheCenterX = 0.0;
    double cacheCenterY = 0.0;
    bool forceCacheRecenter = false;
    bool zoomInteractionActive = false;
    uint64_t serial = 0u;
  };

  void startWorker();
  void stopWorker();
  void submitRequest(const WorkerRequest& request);
  void submitCacheRequest(const WorkerRequest& request);
  void submitReprojectionRequest(const WorkerRequest& request);
  void submitIrisRequest(const WorkerRequest& request);
  void markDisplayRenderFinished(uint64_t serial);
  void workerLoop();
  void cacheWorkerLoop();
  void reprojectionWorkerLoop();
  void irisWorkerLoop();
  bool publishDisplayCacheComposite(const WorkerRequest& request, bool allowPartial, bool* completeOut = nullptr);
  bool publishDisplayReprojection(const WorkerRequest& request);
  void publishAuthoritativeDisplaySource(iris::SourceField source, const WorkerRequest& request);
  void renderZoomAheadCaches(const WorkerRequest& request);

  std::atomic<uint64_t> fractalStateSequence {0u};
  mutable std::mutex fractalStateWriteMutex;
  std::atomic<bool> stopRequested {false};

  mutable std::mutex workerMutex;
  std::condition_variable workerCv;
  bool workerStop = false;
  bool requestPending = false;
  WorkerRequest workerRequest;
  uint64_t nextRequestSerial = 0u;
  std::thread worker;

  mutable std::mutex cacheRequestMutex;
  std::condition_variable cacheRequestCv;
  bool cacheWorkerStop = false;
  bool cacheRequestPending = false;
  WorkerRequest cacheRequest;
  std::thread cacheWorker;

  mutable std::mutex reprojectionRequestMutex;
  std::condition_variable reprojectionRequestCv;
  bool reprojectionWorkerStop = false;
  bool reprojectionRequestPending = false;
  WorkerRequest reprojectionRequest;
  std::thread reprojectionWorker;

  mutable std::mutex irisRequestMutex;
  std::condition_variable irisRequestCv;
  bool irisWorkerStop = false;
  bool irisRequestPending = false;
  WorkerRequest irisRequest;
  std::thread irisWorker;

  mutable std::mutex snapshotMutex;
  iris::SourceField previewSource;
  iris::SourceField authoritativeDisplaySource;
  int authoritativeDisplayMode = iris::FRACTAL_NONE;
  float authoritativeDisplayZoom = -1.f;
  double authoritativeDisplayCenterX = 0.0;
  double authoritativeDisplayCenterY = 0.0;
  iris::SourceField irisCompatibleSource;
  std::array<nautiloid_iris_expander::SourceSlot, nautiloid_iris_expander::kSourceSlotCount> irisExpanderSlots;
  std::atomic<int> irisExpanderPublishedSlot {-1};
  int irisExpanderWriteSlot = 0;
  uint64_t irisCompatibleSerial = 0u;
  int irisCompatibleMode = iris::FRACTAL_NONE;
  float irisCompatibleZoom = -1.f;
  double irisCompatibleCenterX = 0.0;
  double irisCompatibleCenterY = 0.0;
  uint64_t lastExpanderGenerationSentLeft = 0u;
  uint64_t lastExpanderGenerationSentRight = 0u;
  bool rightIrisConnectionObserved = false;
  bool rightIrisWasConnected = false;
  Module* lastRightIrisModule = nullptr;

  mutable std::mutex cacheDataMutex;
  DisplayTileCache displayTileCache;
  PresentationLayer displayPresentationCache;
  std::array<PresentationLayer, 3> zoomAheadLayers;
};

extern Model* modelNautiloid;
