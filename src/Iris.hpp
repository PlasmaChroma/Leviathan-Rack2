#pragma once

#include "DebugTerminalMetrics.hpp"
#include "IrisIO.hpp"
#include "IrisWorkerCompletion.hpp"
#include "NautiloidIrisExpander.hpp"
#include "NautiloidColor.hpp"
#include "plugin.hpp"

#include <array>
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <memory>
#include <mutex>
#include <thread>

constexpr float kIrisMinHz = 10.f;
constexpr float kIrisMaxHz = 10000.f;
constexpr float kIrisLfoMinHz = 0.01f;
constexpr float kIrisLfoMaxHz = 100.f;
constexpr float kIrisCoarseOctaveSpan = 9.96578428466f;
constexpr float kIrisMinPitchFromC4 = -4.70919513631f;

inline float irisBaseFrequencyFromKnob(float knobNorm, bool lfoMode = false) {
  const float minHz = lfoMode ? kIrisLfoMinHz : kIrisMinHz;
  const float maxHz = lfoMode ? kIrisLfoMaxHz : kIrisMaxHz;
  return minHz * std::pow(maxHz / minHz, clamp(knobNorm, 0.f, 1.f));
}

inline float irisKnobValueForFrequency(float hz, bool lfoMode = false) {
  const float minHz = lfoMode ? kIrisLfoMinHz : kIrisMinHz;
  const float maxHz = lfoMode ? kIrisLfoMaxHz : kIrisMaxHz;
  hz = clamp(hz, minHz, maxHz);
  return std::log(hz / minHz) / std::log(maxHz / minHz);
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
	    LFO_MODE_PARAM,
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
    Q_OUTPUT,
    INV_OUTPUT = Q_OUTPUT,
    OUTPUTS_LEN
  };

  enum LightId {
	    LOAD_LIGHT,
	    ERROR_LIGHT,
	    COARSE_STEP_MODE_LIGHT,
	    SOFT_SYNC_MODE_LIGHT,
	    LFO_MODE_LIGHT,
	    IMAGE_CHANNEL_ALL_LIGHT,
	    IMAGE_CHANNEL_RED_LIGHT,
	    IMAGE_CHANNEL_GREEN_LIGHT,
	    IMAGE_CHANNEL_BLUE_LIGHT,
	    NAUTILOID_LINK_LIGHT,
	    NAUTILOID_READY_LIGHT,
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
  void requestExpanderSource(const nautiloid_iris_expander::SourceSlot* sourceSlot, uint64_t generation);
  void requestOwnedExpanderSource(
    std::shared_ptr<const iris::SourceField> source, uint64_t generation);
  void requestReload();
  void clearToDefault();
  void requestRebuild();
  std::string sourceName() const;
  std::string sourcePath() const;
  std::string statusText() const;
  int sourceKind() const;
  bool consumeRestoredImageSourceMode();
  std::shared_ptr<const std::vector<uint8_t>> previewPixelsSnapshot(
    int* width = nullptr, int* height = nullptr) const;
  std::shared_ptr<const iris::SourceField> sourceFieldSnapshot() const;
  void previewSnapshot(std::vector<uint8_t>* pixels, int* width, int* height) const;
  void sourcePreviewSnapshot(std::vector<uint8_t>* pixels, int* width, int* height) const;
  void waveformSnapshot(float scan, int sampleCount, std::vector<float>* samples) const;
  bool embedsSource() const { return embedSource; }
  void setEmbedSource(bool enabled) { embedSource = enabled; }

  iris::ConversionSettings conversionSettings;
  std::atomic<float> displayScan {0.f};
  std::array<std::atomic<float>, 16> displayPolyScans;
  std::atomic<int> displayPolyChannelCount {1};
  std::atomic<float> displayFrequencyHz {0.f};
  // Rate-limited channel-one state for the low-frequency waveform tracer.
  std::atomic<float> displayPhase {0.f};
  std::atomic<float> displayWaveValue {0.f};
  std::atomic<float> displayPhaseFrequencyHz {0.f};
  std::atomic<int> displayImageChannelMode {iris::IMAGE_CHANNEL_ALL};
  std::atomic<bool> displayChannelPreview {false};
  std::atomic<int> activeSourceKind {iris::SOURCE_IMAGE};
  std::atomic<bool> loading {false};
  std::atomic<bool> loadFailed {false};
  std::atomic<uint64_t> previewGeneration {0u};
  debug_terminal::BaselineModuleMetrics debugMetrics;
  std::atomic<bool> restoredImageSourceMode {false};

private:
#ifdef LEVIATHAN_IRIS_PHASE4_TEST
  friend struct IrisPhase4TestAccess;
#endif

  using SourcePtr = std::shared_ptr<const iris::SourceField>;

  enum WorkerRequestType {
    REQUEST_NONE = 0,
    REQUEST_IMPORT_IMAGE_FILE = 1,
    REQUEST_LOAD_EMBEDDED_SOURCE = 2,
    REQUEST_REBUILD_FROM_SOURCE = 3,
    REQUEST_DEFAULT = 4,
    REQUEST_RELOAD_IMAGE_FILE = 5,
    REQUEST_EXPANDER_SOURCE = 7,
    REQUEST_NAUTILOID_FRACTAL_SOURCE = 8,
  };

  struct WorkerRequest {
    WorkerRequestType type = REQUEST_NONE;
    std::string path;
    iris::ConversionSettings settings;
    SourcePtr source;
    const nautiloid_iris_expander::SourceSlot* sourceSlot = nullptr;
    int nautiloidFractalMode = iris::FRACTAL_NONE;
    int nautiloidFractalColorMode = nautiloid_color::PRISM;
    float nautiloidFractalZoom = 0.f;
    float nautiloidFractalCenterX = 0.f;
    float nautiloidFractalCenterY = 0.f;
    uint64_t sourceGeneration = 0u;
    uint64_t serial = 0u;
  };

  struct WorkerResult {
    SourcePtr source;
    bool preserveExistingSource = false;
    int sourceKind = iris::SOURCE_IMAGE;
  };

  void startWorker();
  void stopWorker();
  void submitRequest(const WorkerRequest& request);
  void workerLoop();
  bool publishWorkerResult(WorkerResult& result, int tableIndex, std::string diagnostic = {});
  static void buildPreview(const iris::ImageWavetable& table, std::vector<uint8_t>* pixels);

  std::array<Voice, 16> voices;
  std::array<iris::ImageWavetable, 3> tableBuffers;
  int activeTableIndex = 0;
  int fadeFromTableIndex = -1;
  float tableCrossfade = 1.f;
  std::atomic<int> workerTableIndex {1};
  std::atomic<int> pendingTableIndex {-1};

  std::thread worker;
  mutable std::mutex workerMutex;
  std::condition_variable workerCv;
  bool workerStop = false;
  bool requestPending = false;
  WorkerRequest workerRequest;
  uint64_t nextRequestSerial = 0u;

  mutable std::mutex snapshotMutex;
  iris::ImageWavetable snapshotTable;
  SourcePtr snapshotSource;
  std::shared_ptr<const std::vector<uint8_t>> snapshotPreview;
  int previewWidth = iris::kSourcePreviewWidth;
  int previewHeight = iris::kSourcePreviewHeight;
  std::string lastError;
  bool embedSource = true;
  int currentSourceKind = iris::SOURCE_IMAGE;
  std::atomic<uint64_t> lastExpanderSourceGeneration {0u};
  std::atomic<uintptr_t> lastExpanderSourceIdentity {0u};
  dsp::ClockDivider lightDivider;
  float phaseTracerPublishTimer = 0.f;
};

extern Model* modelIris;
