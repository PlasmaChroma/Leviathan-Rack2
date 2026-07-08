#pragma once

#include "plugin.hpp"
#include "IrisFractal.hpp"

#include <atomic>
#include <condition_variable>
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
  void requestRenderWithCacheCenter(float cacheCenterX, float cacheCenterY);
  void resetView();
  void previewSnapshot(std::vector<uint8_t>* rgb, int* width, int* height) const;
  void irisPreviewSnapshot(std::vector<uint8_t>* rgb, int* width, int* height) const;

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
  std::atomic<uint64_t> displayCacheMisses {0u};
  std::atomic<uint64_t> cacheRequestsSubmitted {0u};
  std::atomic<uint64_t> cacheRequestsDequeued {0u};
  std::atomic<uint64_t> displayCacheRendersCompleted {0u};
  std::atomic<uint64_t> irisRendersCompleted {0u};
  std::atomic<uint64_t> irisRendersDroppedStale {0u};
  std::atomic<bool> debugFileLoggingEnabled {false};
  std::atomic<bool> loading {false};

  struct DisplayCacheTile {
    int x = 0;
    int y = 0;
    iris::SourceField source;
  };

  struct DisplayTileCache {
    int mode = iris::FRACTAL_NONE;
    float zoom = -1.f;
    float centerX = 0.f;
    float centerY = 0.f;
    std::vector<DisplayCacheTile> tiles;

    void clear();
  };

private:
  struct WorkerRequest {
    int mode = iris::FRACTAL_MANDELBROT;
    float zoom = 0.f;
    float centerX = 0.f;
    float centerY = 0.f;
    float cacheCenterX = 0.f;
    float cacheCenterY = 0.f;
    uint64_t serial = 0u;
  };

  void startWorker();
  void stopWorker();
  void submitRequest(const WorkerRequest& request);
  void submitCacheRequest(const WorkerRequest& request);
  void workerLoop();
  void cacheWorkerLoop();

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

  mutable std::mutex snapshotMutex;
  iris::SourceField previewSource;
  iris::SourceField irisCompatibleSource;
  uint64_t irisCompatibleSerial = 0u;
  int irisCompatibleMode = iris::FRACTAL_NONE;
  float irisCompatibleZoom = -1.f;
  float irisCompatibleCenterX = 0.f;
  float irisCompatibleCenterY = 0.f;

  mutable std::mutex cacheDataMutex;
  DisplayTileCache displayTileCache;
};

extern Model* modelNautiloid;
