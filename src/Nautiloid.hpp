#pragma once

#include "plugin.hpp"
#include "IrisFractal.hpp"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

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
  void resetView();
  void previewSnapshot(std::vector<uint8_t>* rgb, int* width, int* height) const;
  void irisPreviewSnapshot(std::vector<uint8_t>* rgb, int* width, int* height) const;

  int fractalMode = iris::FRACTAL_MANDELBROT;
  float fractalZoom = 0.f;
  float fractalCenterX = 0.f;
  float fractalCenterY = 0.f;
  std::atomic<uint64_t> previewGeneration {0u};
  std::atomic<uint64_t> irisPreviewGeneration {0u};
  std::atomic<bool> loading {false};

private:
  struct WorkerRequest {
    int mode = iris::FRACTAL_MANDELBROT;
    float zoom = 0.f;
    float centerX = 0.f;
    float centerY = 0.f;
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
  iris::SourceField fractalCacheSource;
  int fractalCacheMode = iris::FRACTAL_NONE;
  float fractalCacheZoom = -1.f;
  float fractalCacheCenterX = 0.f;
  float fractalCacheCenterY = 0.f;
};

extern Model* modelNautiloid;
