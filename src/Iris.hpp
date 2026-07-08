#pragma once

#include "DebugTerminalMetrics.hpp"
#include "IrisIO.hpp"
#include "plugin.hpp"

#include <array>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

constexpr float kIrisMinHz = 10.f;
constexpr float kIrisMaxHz = 10000.f;
constexpr float kIrisCoarseOctaveSpan = 9.96578428466f;
constexpr float kIrisMinPitchFromC4 = -4.70919513631f;

inline float irisBaseFrequencyFromKnob(float knobNorm) {
  return kIrisMinHz * std::pow(kIrisMaxHz / kIrisMinHz, clamp(knobNorm, 0.f, 1.f));
}

inline float irisKnobValueForFrequency(float hz) {
  hz = clamp(hz, kIrisMinHz, kIrisMaxHz);
  return std::log(hz / kIrisMinHz) / std::log(kIrisMaxHz / kIrisMinHz);
}

struct IrisFreqQuantity final : ParamQuantity {
  float getDisplayValue() override;
  void setDisplayValue(float displayValue) override;
  std::string getDisplayValueString() override;
};

struct Iris final : Module {
  enum ParamId {
	    COARSE_PARAM,
	    FINE_PARAM,
	    SCAN_PARAM,
	    SCAN_ATTEN_PARAM,
	    LIN_FM_PARAM,
	    COARSE_STEP_MODE_PARAM,
	    SOFT_SYNC_MODE_PARAM,
	    SMOOTHING_MENU_PARAM,
	    IMAGE_CHANNEL_PARAM,
	    SOURCE_MENU_PARAM,
	    PARAMS_LEN
  };

  enum InputId {
    V_OCT_INPUT,
    LIN_FM_INPUT,
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
	    COARSE_STEP_MODE_LIGHT,
	    SOFT_SYNC_MODE_LIGHT,
	    IMAGE_CHANNEL_ALL_LIGHT,
	    IMAGE_CHANNEL_RED_LIGHT,
	    IMAGE_CHANNEL_GREEN_LIGHT,
	    IMAGE_CHANNEL_BLUE_LIGHT,
	    LIGHTS_LEN
  };

  struct Voice {
    iris::WavetableOscillator oscillator;
    dsp::SchmittTrigger sync;
    float linHpState = 0.f;
  };

  Iris();
  ~Iris() override;

  void process(const ProcessArgs& args) override;
  void onAdd(const AddEvent& e) override;
  void onSave(const SaveEvent& e) override;
  json_t* dataToJson() override;
  void dataFromJson(json_t* root) override;

  void requestImageLoad(const std::string& path);
  void requestBuiltinFractal(int mode);
  void requestReload();
  void clearToDefault();
  void requestRebuild();
  std::string sourceName() const;
  std::string sourcePath() const;
  std::string statusText() const;
  void previewSnapshot(std::vector<uint8_t>* pixels, int* width, int* height) const;
  void sourcePreviewSnapshot(std::vector<uint8_t>* pixels, int* width, int* height) const;
  void waveformSnapshot(float scan, int sampleCount, std::vector<float>* samples) const;
  bool embedsSource() const { return embedSource; }
  void setEmbedSource(bool enabled) { embedSource = enabled; }
  bool isBuiltinFractalSource() const;
  int builtinFractalMode() const;

  iris::ConversionSettings conversionSettings;
  float fractalZoom = 0.f;
  float fractalCenterX = 0.f;
  float fractalCenterY = 0.f;
  std::atomic<float> displayScan {0.f};
  std::atomic<float> displayFrequencyHz {0.f};
  std::atomic<int> displayImageChannelMode {iris::IMAGE_CHANNEL_ALL};
  std::atomic<bool> displayChannelPreview {false};
  std::atomic<bool> loading {false};
  std::atomic<bool> loadFailed {false};
  std::atomic<uint64_t> previewGeneration {0u};
  debug_terminal::BaselineModuleMetrics debugMetrics;

private:
  enum WorkerRequestType {
    REQUEST_NONE = 0,
    REQUEST_IMPORT_IMAGE_FILE = 1,
    REQUEST_LOAD_EMBEDDED_SOURCE = 2,
    REQUEST_REBUILD_FROM_SOURCE = 3,
    REQUEST_DEFAULT = 4,
    REQUEST_RELOAD_IMAGE_FILE = 5,
    REQUEST_BUILTIN_FRACTAL = 6,
  };

  struct WorkerRequest {
    WorkerRequestType type = REQUEST_NONE;
    std::string path;
    iris::ConversionSettings settings;
    iris::SourceField source;
    int fractalMode = iris::FRACTAL_NONE;
    float fractalZoom = 0.f;
    float fractalCenterX = 0.f;
    float fractalCenterY = 0.f;
    uint64_t serial = 0u;
  };

  struct WorkerResult {
    iris::SourceField source;
    iris::ImageWavetable table;
    bool hasSource = false;
    bool preserveExistingSource = false;
    int sourceKind = iris::SOURCE_IMAGE;
    int fractalMode = iris::FRACTAL_NONE;
  };

  void startWorker();
  void stopWorker();
  void submitRequest(const WorkerRequest& request);
  void workerLoop();
  void publishWorkerResult(WorkerResult& result);
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
  iris::SourceField snapshotSourceField;
  std::vector<uint8_t> snapshotPreview;
  int previewWidth = iris::kSourcePreviewWidth;
  int previewHeight = iris::kSourcePreviewHeight;
  std::string lastError;
  bool embedSource = true;
  int currentSourceKind = iris::SOURCE_IMAGE;
  int currentFractalMode = iris::FRACTAL_NONE;
  dsp::ClockDivider lightDivider;
};

extern Model* modelIris;
