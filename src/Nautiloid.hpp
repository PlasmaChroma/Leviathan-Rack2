#pragma once

#include "plugin.hpp"
#include "NautiloidFractal.hpp"

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

struct Nautiloid final : Module {
  enum ParamId {
    SOURCE_MENU_PARAM,
    RESET_VIEW_PARAM,
    PARAMS_LEN
  };

  enum InputId {
    INPUTS_LEN
  };

  enum OutputId {
    OUTPUTS_LEN
  };

  enum LightId {
    LIGHTS_LEN
  };

  Nautiloid();
  ~Nautiloid() override;

  void process(const ProcessArgs& args) override;
  json_t* dataToJson() override;
  void dataFromJson(json_t* root) override;

  void requestFractal(int mode);
  void requestRender();
  void requestRenderWithCacheCenter(float cacheCenterX, float cacheCenterY, bool forceCacheRecenter = false);
  void requestRenderWithCenteredCache();
  void resetView();
  void previewSnapshot(std::vector<uint8_t>* rgb, int* width, int* height) const;
  void irisPreviewSnapshot(std::vector<uint8_t>* rgb, int* width, int* height) const;
  std::shared_ptr<const iris::SourceField> irisExpanderSourceSnapshot(uint64_t* generation) const;

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
    float cacheCenterX = 0.f;
    float cacheCenterY = 0.f;
    size_t currentTileCount = 0u;
    size_t fullTileCount = 0u;
    std::vector<uint8_t> tileCurrent;
  };

  void displayTileCacheSnapshot(DisplayTileCacheSnapshot* snapshot) const;

  int fractalMode = iris::FRACTAL_MANDELBROT;
  float fractalZoom = 0.f;
  float fractalCenterX = 0.f;
  float fractalCenterY = 0.f;
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
  std::atomic<uint64_t> irisRendersCompleted {0u};
  std::atomic<uint64_t> irisRendersDroppedStale {0u};
  std::atomic<uint64_t> irisExpanderPublishes {0u};
  std::atomic<bool> debugFileLoggingEnabled {false};
  std::atomic<bool> loading {false};

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
    float centerX = 0.f;
    float centerY = 0.f;
    std::vector<DisplayCacheTile> tiles;
    int stitchedWidth = 0;
    int stitchedHeight = 0;
    std::vector<uint8_t> stitchedRgb8;

    void clear();
    void ensureStorage(int cacheWidth, int cacheHeight, int tileSize);
    void writeTileToStitched(const DisplayCacheTile& tile);
    size_t validTileCount() const;
  };

  struct DisplayPresentationCache {
    int mode = iris::FRACTAL_NONE;
    float zoom = -1.f;
    float centerX = 0.f;
    float centerY = 0.f;
    int width = 0;
    int height = 0;
    float cacheScale = 1.f;
    std::vector<uint8_t> rgb8;

    bool valid() const;
  };

  struct ZoomAheadCache {
    int mode = iris::FRACTAL_NONE;
    float zoom = -1.f;
    float centerX = 0.f;
    float centerY = 0.f;
    int width = 0;
    int height = 0;
    float cacheScale = 1.f;
    std::vector<uint8_t> rgb8;

    bool valid() const;
  };

private:
  struct WorkerRequest {
    int mode = iris::FRACTAL_MANDELBROT;
    float zoom = 0.f;
    float centerX = 0.f;
    float centerY = 0.f;
    float cacheCenterX = 0.f;
    float cacheCenterY = 0.f;
    bool forceCacheRecenter = false;
    uint64_t serial = 0u;
  };

  void startWorker();
  void stopWorker();
  void submitRequest(const WorkerRequest& request);
  void submitCacheRequest(const WorkerRequest& request);
  void submitReprojectionRequest(const WorkerRequest& request);
  void workerLoop();
  void cacheWorkerLoop();
  void reprojectionWorkerLoop();
  bool publishDisplayCacheComposite(const WorkerRequest& request, bool allowPartial, bool* completeOut = nullptr);
  bool publishDisplayReprojection(const WorkerRequest& request);
  void publishAuthoritativeDisplaySource(iris::SourceField source, const WorkerRequest& request);
  void renderZoomAheadCache(const WorkerRequest& request);

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

  mutable std::mutex snapshotMutex;
  iris::SourceField previewSource;
  iris::SourceField authoritativeDisplaySource;
  int authoritativeDisplayMode = iris::FRACTAL_NONE;
  float authoritativeDisplayZoom = -1.f;
  float authoritativeDisplayCenterX = 0.f;
  float authoritativeDisplayCenterY = 0.f;
  iris::SourceField irisCompatibleSource;
  std::shared_ptr<const iris::SourceField> irisExpanderSource;
  uint64_t irisCompatibleSerial = 0u;
  int irisCompatibleMode = iris::FRACTAL_NONE;
  float irisCompatibleZoom = -1.f;
  float irisCompatibleCenterX = 0.f;
  float irisCompatibleCenterY = 0.f;
  uint64_t lastExpanderGenerationSentLeft = 0u;
  uint64_t lastExpanderGenerationSentRight = 0u;

  mutable std::mutex cacheDataMutex;
  DisplayTileCache displayTileCache;
  DisplayPresentationCache displayPresentationCache;
  ZoomAheadCache zoomAheadCache;
};

extern Model* modelNautiloid;
