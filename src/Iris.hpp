#pragma once

#include "IrisIO.hpp"
#include "plugin.hpp"

#include <array>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

struct Iris final : Module {
  enum ParamId {
    COARSE_PARAM,
    FINE_PARAM,
    SCAN_PARAM,
    SCAN_ATTEN_PARAM,
    FM_ATTEN_PARAM,
    LEVEL_PARAM,
    QUANT_PARAM,
    PARAMS_LEN
  };

  enum InputId {
    V_OCT_INPUT,
    FM_INPUT,
    SCAN_INPUT,
    SYNC_INPUT,
    INPUTS_LEN
  };

  enum OutputId {
    OUT_OUTPUT,
    INV_OUTPUT,
    OUTPUTS_LEN
  };

  enum LightId {
    LOAD_LIGHT,
    ERROR_LIGHT,
    QUANT_LIGHT,
    LIGHTS_LEN
  };

  struct Voice {
    iris::WavetableOscillator oscillator;
    dsp::SchmittTrigger sync;
  };

  Iris();
  ~Iris() override;

  void process(const ProcessArgs& args) override;
  void onAdd(const AddEvent& e) override;
  void onSave(const SaveEvent& e) override;
  json_t* dataToJson() override;
  void dataFromJson(json_t* root) override;

  void requestImageLoad(const std::string& path);
  void requestReload();
  void clearToSine();
  void requestRebuild();
  std::string sourceName() const;
  std::string sourcePath() const;
  std::string statusText() const;
  void previewSnapshot(std::vector<uint8_t>* pixels, int* width, int* height) const;
  bool embedsTable() const { return embedTable; }
  void setEmbedTable(bool enabled) { embedTable = enabled; }

  iris::ConversionSettings conversionSettings;
  std::atomic<float> displayScan {0.f};
  std::atomic<bool> loading {false};
  std::atomic<bool> loadFailed {false};
  std::atomic<uint64_t> previewGeneration {0u};

private:
  enum WorkerRequestType {
    REQUEST_NONE = 0,
    REQUEST_IMAGE = 1,
    REQUEST_EMBEDDED = 2,
    REQUEST_REBUILD = 3,
    REQUEST_SINE = 4,
  };

  struct WorkerRequest {
    WorkerRequestType type = REQUEST_NONE;
    std::string path;
    iris::ConversionSettings settings;
    uint64_t serial = 0u;
  };

  void startWorker();
  void stopWorker();
  void submitRequest(const WorkerRequest& request);
  void workerLoop();
  void publishBuiltTable(iris::ImageWavetable* table, bool preserveSourceMetadata);
  static void buildPreview(const iris::ImageWavetable& table, std::vector<uint8_t>* pixels);

  std::array<Voice, 16> voices;
  iris::ImageWavetable* activeTable = nullptr;
  std::atomic<iris::ImageWavetable*> pendingTable {nullptr};
  std::atomic<iris::ImageWavetable*> retiredTable {nullptr};

  std::thread worker;
  mutable std::mutex workerMutex;
  std::condition_variable workerCv;
  bool workerStop = false;
  bool requestPending = false;
  WorkerRequest workerRequest;
  uint64_t nextRequestSerial = 0u;

  mutable std::mutex snapshotMutex;
  iris::ImageWavetable snapshotTable;
  std::vector<uint8_t> snapshotPreview;
  int previewWidth = 128;
  int previewHeight = 64;
  std::string lastError;
  bool embedTable = true;
};

extern Model* modelIris;
