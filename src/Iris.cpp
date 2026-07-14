#include "Iris.hpp"
#include "Nautiloid.hpp"
#include "NautiloidFractal.hpp"

#include <new>
#include <utility>

namespace {

const char* kEmbeddedSourceName = "iris-source.qoi";
std::atomic<uint32_t> gIrisDebugInstanceCounter {1u};
constexpr float kLinHpCutoffHz = 4.9f;
constexpr float kLinFmScale = 0.10f;
constexpr float kLinFmDriveThreshold = 0.80f;
constexpr float kLinFmMaxDrive = 4.0f;
constexpr float kMinFrequencyHz = 8.f;
constexpr float kMaxFrequencyHz = 20000.f;

bool hasLeftNautiloid(const Iris* module) {
  if (!module) return false;
  const engine::Module* left = module->leftExpander.module;
  return left && left->model &&
    ((left->model == modelNautiloid) || left->model->slug == "Nautiloid");
}

float acCoupledLinFm(float x, Iris::Voice* voice, float sampleTime) {
  const float hpCoeff = clamp(1.f - 2.f * float(M_PI) * kLinHpCutoffHz * sampleTime, 0.f, 1.f);
  const float y = x - voice->linHpState;
  voice->linHpState = x - hpCoeff * y;
  return y;
}

float fastAtanApprox(float x) {
  const float ax = std::fabs(x);
  if (ax <= 1.f) return x * (0.78539816339f + 0.273f * (1.f - ax));
  const float inv = 1.f / ax;
  const float t = inv * (0.78539816339f + 0.273f * (1.f - inv));
  return x >= 0.f ? 1.57079632679f - t : -1.57079632679f + t;
}

float drivenLinFm(float lin, float amount) {
  const float bus = lin * amount * kLinFmScale;
  const float driveNorm =
    clamp((amount - kLinFmDriveThreshold) / (1.f - kLinFmDriveThreshold), 0.f, 1.f);
  if (driveNorm <= 0.f || std::fabs(bus) < 1e-9f) return bus;
  const float drive = 1.f + driveNorm * (kLinFmMaxDrive - 1.f);
  const float norm = std::max(fastAtanApprox(drive), 1e-6f);
  return fastAtanApprox(bus * drive) / norm;
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

} // namespace

Iris::Iris() {
  debugMetrics.assignInstanceId(gIrisDebugInstanceCounter);
  config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
  configParam<IrisFreqQuantity>(
    COARSE_PARAM, 0.f, 1.f, irisKnobValueForFrequency(dsp::FREQ_C4), "Frequency");
  configParam(FINE_PARAM, -100.f, 100.f, 0.f, "Fine tune", " cents");
  configParam(SCAN_PARAM, 0.f, 1.f, 0.f, "Image scan", " %", 0.f, 100.f);
  configParam(SCAN_ATTEN_PARAM, -1.f, 1.f, 0.f, "Scan CV attenuverter", " %", 0.f, 100.f);
  configParam(LIN_FM_PARAM, 0.f, 1.f, 0.f, "Linear FM", " %", 0.f, 100.f);
  configSwitch(LFO_MODE_PARAM, 0.f, 1.f, 0.f, "LFO mode", {"Audio", "LFO"});
  configSwitch(COARSE_STEP_MODE_PARAM, 0.f, 1.f, 0.f, "Octave stepped", {"Continuous", "Octave stepped"});
  configSwitch(SOFT_SYNC_MODE_PARAM, 0.f, 1.f, 0.f, "Sync mode", {"Hard sync", "Soft sync"});
  configButton(SMOOTHING_MENU_PARAM, "Image options");
  configButton(IMAGE_CHANNEL_PARAM, "Image color channel");
  configButton(SOURCE_MENU_PARAM, "Source");
  configInput(V_OCT_INPUT, "V/Oct");
  configInput(LIN_FM_INPUT, "Linear FM");
  configInput(SCAN_INPUT, "Scan CV");
  configInput(SYNC_INPUT, "Sync");
  configOutput(OUT_OUTPUT, "Wavetable");
  configOutput(Q_OUTPUT, "Quadrature wavetable");
  lightDivider.setDivision(64);

  tableBuffers[0] = iris::makeDefaultTable();
  tableBuffers[1] = tableBuffers[0];
  activeTableIndex = 0;
  workerTableIndex.store(1, std::memory_order_release);
  pendingTableIndex.store(-1, std::memory_order_release);
  snapshotTable = tableBuffers[activeTableIndex];
  buildPreview(snapshotTable, &snapshotPreview);
  startWorker();
}

Iris::~Iris() {
  stopWorker();
}

void Iris::startWorker() {
  if (!worker.joinable()) {
    worker = std::thread([this]() { workerLoop(); });
  }
}

void Iris::stopWorker() {
  {
    std::lock_guard<std::mutex> lock(workerMutex);
    if (requestPending && workerRequest.sourceSlot) {
      nautiloid_iris_expander::releaseSourceSlot(workerRequest.sourceSlot);
      workerRequest.sourceSlot = nullptr;
    }
    workerStop = true;
    requestPending = false;
  }
  workerCv.notify_all();
  if (worker.joinable()) worker.join();
}

void Iris::submitRequest(const WorkerRequest& request) {
  {
    std::lock_guard<std::mutex> lock(workerMutex);
    if (requestPending && workerRequest.sourceSlot) {
      nautiloid_iris_expander::releaseSourceSlot(workerRequest.sourceSlot);
    }
    workerRequest = request;
    workerRequest.serial = ++nextRequestSerial;
    requestPending = true;
  }
  loading.store(true, std::memory_order_release);
  loadFailed.store(false, std::memory_order_release);
  workerCv.notify_one();
}

void Iris::requestImageLoad(const std::string& path) {
  if (path.empty()) return;
  restoredImageSourceMode.store(false, std::memory_order_release);
  WorkerRequest request;
  request.type = REQUEST_IMPORT_IMAGE_FILE;
  request.path = path;
  request.settings = conversionSettings;
  submitRequest(request);
}

void Iris::requestExpanderSource(const nautiloid_iris_expander::SourceSlot* sourceSlot, uint64_t generation) {
  if (!hasLeftNautiloid(this)) return;
  restoredImageSourceMode.store(false, std::memory_order_release);
  if (!nautiloid_iris_expander::acquireSourceSlot(sourceSlot, generation)) return;
  if (generation == lastExpanderSourceGeneration.load(std::memory_order_acquire) &&
      sourceSlot == lastExpanderSourceSlotSeen.load(std::memory_order_acquire)) {
    std::lock_guard<std::mutex> lock(snapshotMutex);
    if (currentSourceKind == iris::SOURCE_EXPANDER_IMAGE ||
        currentSourceKind == iris::SOURCE_NAUTILOID_FRACTAL) {
      nautiloid_iris_expander::releaseSourceSlot(sourceSlot);
      return;
    }
  }
  lastExpanderSourceGeneration.store(generation, std::memory_order_release);
  lastExpanderSourceSlotSeen.store(sourceSlot, std::memory_order_release);
  WorkerRequest request;
  request.type = REQUEST_EXPANDER_SOURCE;
  request.sourceSlot = sourceSlot;
  request.sourceGeneration = generation;
  request.settings = conversionSettings;
  submitRequest(request);
}

void Iris::requestReload() {
  WorkerRequest request;
  request.settings = conversionSettings;
  {
    std::lock_guard<std::mutex> lock(snapshotMutex);
    if (currentSourceKind == iris::SOURCE_NAUTILOID_FRACTAL &&
        iris::sourceHasNautiloidFractalParams(snapshotSourceField)) {
      const iris::NautiloidFractalSourceParams params =
        iris::nautiloidFractalParamsFromSource(snapshotSourceField);
      request.type = REQUEST_NAUTILOID_FRACTAL_SOURCE;
      request.nautiloidFractalMode = params.mode;
      request.nautiloidFractalZoom = params.zoom;
      request.nautiloidFractalCenterX = params.centerX;
      request.nautiloidFractalCenterY = params.centerY;
      request.sourceGeneration = params.generation;
      submitRequest(request);
      return;
    }
  }
  const std::string path = sourcePath();
  if (!path.empty()) {
    request.type = REQUEST_RELOAD_IMAGE_FILE;
    request.path = path;
    {
      std::lock_guard<std::mutex> lock(snapshotMutex);
      request.source = snapshotSourceField;
    }
  } else {
    std::lock_guard<std::mutex> lock(snapshotMutex);
    if (!snapshotSourceField.valid()) return;
    request.type = REQUEST_REBUILD_FROM_SOURCE;
    request.source = snapshotSourceField;
  }
  submitRequest(request);
}

void Iris::requestRebuild() {
  WorkerRequest request;
  request.settings = conversionSettings;
  {
    std::lock_guard<std::mutex> lock(snapshotMutex);
    if (currentSourceKind == iris::SOURCE_NAUTILOID_FRACTAL &&
        iris::sourceHasNautiloidFractalParams(snapshotSourceField)) {
      const iris::NautiloidFractalSourceParams params =
        iris::nautiloidFractalParamsFromSource(snapshotSourceField);
      request.type = REQUEST_NAUTILOID_FRACTAL_SOURCE;
      request.nautiloidFractalMode = params.mode;
      request.nautiloidFractalZoom = params.zoom;
      request.nautiloidFractalCenterX = params.centerX;
      request.nautiloidFractalCenterY = params.centerY;
      request.sourceGeneration = params.generation;
    } else if (snapshotSourceField.valid()) {
      request.type = REQUEST_REBUILD_FROM_SOURCE;
      request.source = snapshotSourceField;
    } else if (!snapshotSourceField.sourcePath.empty()) {
      request.type = REQUEST_IMPORT_IMAGE_FILE;
      request.path = snapshotSourceField.sourcePath;
    } else if (!snapshotTable.sourcePath.empty()) {
      request.type = REQUEST_IMPORT_IMAGE_FILE;
      request.path = snapshotTable.sourcePath;
    } else {
      request.type = REQUEST_DEFAULT;
    }
  }
  submitRequest(request);
}

void Iris::clearToDefault() {
  WorkerRequest request;
  request.type = REQUEST_DEFAULT;
  request.settings = conversionSettings;
  submitRequest(request);
}

void Iris::publishWorkerResult(WorkerResult& result, int tableIndex) {
  if (tableIndex < 0 || tableIndex >= int(tableBuffers.size()) || !tableBuffers[size_t(tableIndex)].valid()) {
    return;
  }
  const iris::ImageWavetable& table = tableBuffers[size_t(tableIndex)];
  {
    std::lock_guard<std::mutex> lock(snapshotMutex);
    if (result.hasSource) {
      snapshotSourceField = result.source;
    } else if (!result.preserveExistingSource) {
      snapshotSourceField = iris::SourceField();
    }
    if (!result.preserveExistingSource) {
      currentSourceKind = result.sourceKind;
      activeSourceKind.store(currentSourceKind, std::memory_order_release);
    }
    snapshotTable = table;
    buildPreview(snapshotTable, &snapshotPreview);
    lastError.clear();
  }
  pendingTableIndex.store(tableIndex, std::memory_order_release);
  previewGeneration.fetch_add(1u, std::memory_order_release);
  loading.store(false, std::memory_order_release);
  loadFailed.store(false, std::memory_order_release);
}

void Iris::workerLoop() {
  while (true) {
    WorkerRequest request;
    int tableIndex = -1;
    {
      std::unique_lock<std::mutex> lock(workerMutex);
      workerCv.wait_for(lock, std::chrono::milliseconds(20), [this]() {
        return workerStop ||
          (requestPending && pendingTableIndex.load(std::memory_order_acquire) < 0);
      });
      if (workerStop) break;
      if (!requestPending) continue;
      request = workerRequest;
      workerRequest = WorkerRequest();
      requestPending = false;
      tableIndex = workerTableIndex.load(std::memory_order_acquire);
    }
    if (tableIndex < 0 || tableIndex >= int(tableBuffers.size())) {
      if (request.sourceSlot) {
        nautiloid_iris_expander::releaseSourceSlot(request.sourceSlot);
        request.sourceSlot = nullptr;
      }
      continue;
    }
    iris::ImageWavetable* buildTable = &tableBuffers[size_t(tableIndex)];

    WorkerResult result;
    std::string error;
    bool ok = false;
    try {
      if (request.type == REQUEST_DEFAULT) {
        *buildTable = iris::makeDefaultTable();
        ok = true;
      } else if (request.type == REQUEST_LOAD_EMBEDDED_SOURCE) {
        iris::SourceField source;
        ok = iris::loadSourceField(request.path, &source, &error);
        if (ok) {
          source.sourcePath = request.source.sourcePath;
          source.sourceName = request.source.sourceName;
          source.originalWidth = request.source.originalWidth;
          source.originalHeight = request.source.originalHeight;
          source.originalChannels = request.source.originalChannels;
          ok = iris::buildWavetableFromSourceField(source, request.settings, buildTable, &error);
          result.source = std::move(source);
          result.hasSource = ok;
          result.sourceKind = iris::SOURCE_IMAGE;
        }
      } else if (request.type == REQUEST_REBUILD_FROM_SOURCE) {
        ok = iris::buildWavetableFromSourceField(request.source, request.settings, buildTable, &error);
        result.preserveExistingSource = true;
      } else if (request.type == REQUEST_EXPANDER_SOURCE) {
        if (request.sourceSlot &&
            request.sourceSlot->generation.load(std::memory_order_acquire) == request.sourceGeneration &&
            request.sourceSlot->source.valid()) {
          ok = iris::buildWavetableFromSourceField(request.sourceSlot->source, request.settings, buildTable, &error);
          result.source = request.sourceSlot->source;
          result.source.sourcePath.clear();
          if (result.source.sourceName.empty()) {
            result.source.sourceName = "Nautiloid";
          }
          result.hasSource = ok;
          result.sourceKind = iris::sourceHasNautiloidFractalParams(result.source)
            ? iris::SOURCE_NAUTILOID_FRACTAL
            : iris::SOURCE_EXPANDER_IMAGE;
        } else {
          error = "Invalid expander source";
        }
      } else if (request.type == REQUEST_NAUTILOID_FRACTAL_SOURCE) {
        iris::NautiloidFractalSourceParams params;
        params.mode = request.nautiloidFractalMode;
        params.zoom = request.nautiloidFractalZoom;
        params.centerX = request.nautiloidFractalCenterX;
        params.centerY = request.nautiloidFractalCenterY;
        params.generation = request.sourceGeneration;
        iris::SourceField source;
        ok = iris::makeNautiloidIrisSource(params, &source, &error);
        if (ok) {
          ok = iris::buildWavetableFromSourceField(source, request.settings, buildTable, &error);
          result.source = std::move(source);
          result.hasSource = ok;
          result.sourceKind = iris::SOURCE_NAUTILOID_FRACTAL;
        }
      } else if (request.type == REQUEST_RELOAD_IMAGE_FILE) {
        iris::SourceField source;
        ok = iris::importImageFileToSourceField(request.path, &source, &error);
        if (ok) {
          ok = iris::buildWavetableFromSourceField(source, request.settings, buildTable, &error);
          result.source = std::move(source);
          result.hasSource = ok;
          result.sourceKind = iris::SOURCE_IMAGE;
        } else if (request.source.valid()) {
          const std::string reloadError = error;
          ok = iris::buildWavetableFromSourceField(request.source, request.settings, buildTable, &error);
          result.preserveExistingSource = true;
          if (ok && !reloadError.empty()) {
            error = reloadError;
          }
        }
      } else if (request.type == REQUEST_IMPORT_IMAGE_FILE) {
        iris::SourceField source;
        ok = iris::importImageFileToSourceField(request.path, &source, &error);
        if (ok) {
          ok = iris::buildWavetableFromSourceField(source, request.settings, buildTable, &error);
          result.source = std::move(source);
          result.hasSource = ok;
          result.sourceKind = iris::SOURCE_IMAGE;
        }
      } else {
        error = "Unknown Iris image worker request";
      }
    } catch (const std::bad_alloc&) {
      error = "Image worker allocation failed";
      ok = false;
    } catch (const std::exception&) {
      error = "Image worker exception";
      ok = false;
    } catch (...) {
      error = "Unexpected image worker failure";
      ok = false;
    }

    {
      std::lock_guard<std::mutex> lock(workerMutex);
      if (request.serial != nextRequestSerial) {
        // Slider drags continuously replace the queued rebuild request. Publish
        // completed frames from the same source so the display tracks the drag,
        // while still rejecting stale loads and source changes.
        const bool publishIntermediateRebuild =
          request.type == REQUEST_REBUILD_FROM_SOURCE &&
          requestPending &&
          workerRequest.type == REQUEST_REBUILD_FROM_SOURCE &&
          request.source.sourcePath == workerRequest.source.sourcePath;
        const bool publishIntermediateExpander =
          request.type == REQUEST_EXPANDER_SOURCE &&
          requestPending &&
          workerRequest.type == REQUEST_EXPANDER_SOURCE;
        if (!publishIntermediateRebuild && !publishIntermediateExpander) {
          if (request.sourceSlot) {
            nautiloid_iris_expander::releaseSourceSlot(request.sourceSlot);
            request.sourceSlot = nullptr;
          }
          continue;
        }
      }
    }
    if (ok) {
      try {
        publishWorkerResult(result, tableIndex);
      } catch (const std::bad_alloc&) {
        error = "Table publication allocation failed";
        ok = false;
      } catch (const std::exception&) {
        error = "Table publication exception";
        ok = false;
      } catch (...) {
        error = "Unexpected table publication failure";
        ok = false;
      }
    }
    if (!ok) {
      if (request.type != REQUEST_DEFAULT) {
        WorkerResult fallback;
        *buildTable = iris::makeDefaultTable();
        try {
          publishWorkerResult(fallback, tableIndex);
        } catch (...) {
        }
      }
      {
        std::lock_guard<std::mutex> lock(snapshotMutex);
        lastError = error;
      }
      loading.store(false, std::memory_order_release);
      loadFailed.store(true, std::memory_order_release);
    }
    if (request.sourceSlot) {
      nautiloid_iris_expander::releaseSourceSlot(request.sourceSlot);
      request.sourceSlot = nullptr;
    }
  }
}

void Iris::process(const ProcessArgs& args) {
  const bool measurePerf = isDragonKingDebugEnabled();
  const auto processStart = debug_terminal::debugTimerStart(measurePerf);
  const int pendingIndex = pendingTableIndex.exchange(-1, std::memory_order_acq_rel);
  if (pendingIndex >= 0 && pendingIndex < int(tableBuffers.size()) && pendingIndex != activeTableIndex) {
    const int oldActiveIndex = activeTableIndex;
    activeTableIndex = pendingIndex;
    workerTableIndex.store(oldActiveIndex, std::memory_order_release);
    workerCv.notify_one();
  }
  const iris::ImageWavetable* table = &tableBuffers[size_t(activeTableIndex)];
  const int channels = std::max(1, std::min(inputs[V_OCT_INPUT].getChannels(), 16));
  outputs[OUT_OUTPUT].setChannels(channels);
  outputs[Q_OUTPUT].setChannels(channels);
  const float coarseParam = params[COARSE_PARAM].getValue();
  const bool lfoMode = params[LFO_MODE_PARAM].getValue() > 0.5f;
  const float coarseKnob =
    std::isfinite(coarseParam) ? clamp(coarseParam, 0.f, 1.f) : irisKnobValueForFrequency(dsp::FREQ_C4, lfoMode);
  float coarsePitch = std::log2(irisBaseFrequencyFromKnob(coarseKnob, lfoMode) / dsp::FREQ_C4);
  const bool coarseStepped = params[COARSE_STEP_MODE_PARAM].getValue() > 0.5f;
  if (coarseStepped) coarsePitch = std::round(coarsePitch);
  const float fineParam = params[FINE_PARAM].getValue();
  const float scanParam = params[SCAN_PARAM].getValue();
  const float scanAttenParam = params[SCAN_ATTEN_PARAM].getValue();
  const float linFmParam = params[LIN_FM_PARAM].getValue();
  const float fine = std::isfinite(fineParam) ? fineParam / 1200.f : 0.f;
  const float scanKnob = std::isfinite(scanParam) ? clamp(scanParam, 0.f, 1.f) : 0.f;
  const float scanAtten = std::isfinite(scanAttenParam) ? clamp(scanAttenParam, -1.f, 1.f) : 0.f;
  const float linFmAmount = std::isfinite(linFmParam) ? clamp(linFmParam, 0.f, 1.f) : 0.f;
  const bool softSync = params[SOFT_SYNC_MODE_PARAM].getValue() > 0.5f;
  const float processMinFrequency = lfoMode ? 0.005f : kMinFrequencyHz;
  const float processMaxFrequency = std::min(kMaxFrequencyHz, 0.45f * args.sampleRate);
  float scanDisplay = scanKnob;
  float frequencyDisplay = 0.f;
  for (int channel = 0; channel < channels; ++channel) {
    const float vOctInput = inputs[V_OCT_INPUT].getPolyVoltage(channel);
    const float mainPitch = coarsePitch + fine + (std::isfinite(vOctInput) ? vOctInput : 0.f);
    const float basePitch = clamp(mainPitch, -24.f, 16.f);
    const float baseFrequency =
      clamp(dsp::FREQ_C4 * dsp::exp2_taylor5(basePitch), processMinFrequency, processMaxFrequency);
    const float linInput = inputs[LIN_FM_INPUT].getPolyVoltage(channel);
    const float lin = acCoupledLinFm(
      std::isfinite(linInput) ? linInput : 0.f, &voices[size_t(channel)], args.sampleTime);
    const float linBus = drivenLinFm(lin, linFmAmount);
    const float frequency =
      clamp(baseFrequency + baseFrequency * linBus, processMinFrequency, processMaxFrequency);
    if (channel == 0) {
      frequencyDisplay = baseFrequency;
    }
    const float scanInput = inputs[SCAN_INPUT].getPolyVoltage(channel);
    float scan = scanKnob + (std::isfinite(scanInput) ? scanInput : 0.f) * 0.1f * scanAtten;
    scan = clamp(scan, 0.f, 1.f);
    if (channel == 0) scanDisplay = scan;
    const float syncInput = inputs[SYNC_INPUT].getPolyVoltage(channel);
    Voice& voice = voices[size_t(channel)];
    if (voice.sync.process(std::isfinite(syncInput) ? syncInput : 0.f)) {
      if (softSync) {
        voice.oscillator.softSync();
      } else {
        voice.oscillator.reset();
      }
    }
    const float quadWave = table ? table->sample(voice.oscillator.phase + 0.25f, scan) : 0.f;
    const float wave = table ? voice.oscillator.process(*table, frequency, args.sampleTime, scan) : 0.f;
    const float volts = std::isfinite(wave) ? 5.f * wave : 0.f;
    const float quadVolts = std::isfinite(quadWave) ? 5.f * quadWave : 0.f;
    outputs[OUT_OUTPUT].setVoltage(volts, channel);
    outputs[Q_OUTPUT].setVoltage(quadVolts, channel);
  }
  displayScan.store(scanDisplay, std::memory_order_relaxed);
  displayFrequencyHz.store(frequencyDisplay, std::memory_order_relaxed);
  if (lightDivider.process()) {
    lights[LOAD_LIGHT].setBrightness(loading.load(std::memory_order_relaxed) ? 1.f : 0.f);
    lights[ERROR_LIGHT].setBrightness(loadFailed.load(std::memory_order_relaxed) ? 1.f : 0.f);
    lights[COARSE_STEP_MODE_LIGHT].setBrightness(coarseStepped ? 0.5f : 0.f);
    lights[SOFT_SYNC_MODE_LIGHT].setBrightness(softSync ? 0.5f : 0.f);
    lights[LFO_MODE_LIGHT].setBrightness(lfoMode ? 0.5f : 0.f);
    const int imageChannelMode = clamp(displayImageChannelMode.load(std::memory_order_relaxed), 0, 3);
    lights[IMAGE_CHANNEL_ALL_LIGHT].setBrightness(imageChannelMode == iris::IMAGE_CHANNEL_ALL ? 1.f : 0.f);
    lights[IMAGE_CHANNEL_RED_LIGHT].setBrightness(imageChannelMode == iris::IMAGE_CHANNEL_RED ? 1.f : 0.f);
    lights[IMAGE_CHANNEL_GREEN_LIGHT].setBrightness(imageChannelMode == iris::IMAGE_CHANNEL_GREEN ? 1.f : 0.f);
    lights[IMAGE_CHANNEL_BLUE_LIGHT].setBrightness(imageChannelMode == iris::IMAGE_CHANNEL_BLUE ? 1.f : 0.f);
    const bool nautiloidConnected = hasLeftNautiloid(this);
    if (!nautiloidConnected) {
      restoredImageSourceMode.store(false, std::memory_order_release);
    }
    const int sourceKindNow = activeSourceKind.load(std::memory_order_acquire);
    const bool usingNautiloidSource =
      sourceKindNow == iris::SOURCE_EXPANDER_IMAGE ||
      sourceKindNow == iris::SOURCE_NAUTILOID_FRACTAL;
    const bool nautiloidReady =
      nautiloidConnected && usingNautiloidSource &&
      lastExpanderSourceGeneration.load(std::memory_order_acquire) != 0u &&
      lastExpanderSourceSlotSeen.load(std::memory_order_acquire) != nullptr;
    lights[NAUTILOID_LINK_LIGHT].setBrightness(nautiloidConnected && !nautiloidReady ? 1.f : 0.f);
    lights[NAUTILOID_READY_LIGHT].setBrightness(nautiloidReady ? 1.f : 0.f);
  }
  if (measurePerf) {
    debugMetrics.recordProcess(debug_terminal::elapsedNsSince(processStart));
  }
}

void Iris::onAdd(const AddEvent& e) {
  Module::onAdd(e);
  const std::string directory = getPatchStorageDirectory();
  WorkerRequest request;
  request.settings = conversionSettings;
  {
    std::lock_guard<std::mutex> lock(snapshotMutex);
    if (currentSourceKind == iris::SOURCE_NAUTILOID_FRACTAL &&
        iris::sourceHasNautiloidFractalParams(snapshotSourceField)) {
      const iris::NautiloidFractalSourceParams params =
        iris::nautiloidFractalParamsFromSource(snapshotSourceField);
      request.type = REQUEST_NAUTILOID_FRACTAL_SOURCE;
      request.nautiloidFractalMode = params.mode;
      request.nautiloidFractalZoom = params.zoom;
      request.nautiloidFractalCenterX = params.centerX;
      request.nautiloidFractalCenterY = params.centerY;
      request.sourceGeneration = params.generation;
      submitRequest(request);
      return;
    }
    request.source = snapshotSourceField;
  }
  if (embedSource && !directory.empty()) {
    const std::string sourcePath = system::join(directory, kEmbeddedSourceName);
    if (system::isFile(sourcePath)) {
      request.type = REQUEST_LOAD_EMBEDDED_SOURCE;
      request.path = sourcePath;
      submitRequest(request);
      return;
    }
  }
  if (!request.source.sourcePath.empty()) {
    request.type = REQUEST_IMPORT_IMAGE_FILE;
    request.path = request.source.sourcePath;
    submitRequest(request);
  }
}

void Iris::onSave(const SaveEvent& e) {
  Module::onSave(e);
  {
    std::lock_guard<std::mutex> lock(snapshotMutex);
    if (currentSourceKind == iris::SOURCE_EXPANDER_IMAGE ||
        currentSourceKind == iris::SOURCE_NAUTILOID_FRACTAL) {
      const std::string directory = getPatchStorageDirectory();
      if (!directory.empty()) {
        const std::string sourcePath = system::join(directory, kEmbeddedSourceName);
        if (system::isFile(sourcePath)) {
          system::remove(sourcePath);
        }
      }
      return;
    }
  }
  if (!embedSource) return;
  iris::SourceField source;
  {
    std::lock_guard<std::mutex> lock(snapshotMutex);
    source = snapshotSourceField;
  }
  if (!source.valid()) return;
  const std::string directory = createPatchStorageDirectory();
  std::string error;
  if (!iris::saveSourceField(system::join(directory, kEmbeddedSourceName), source, &error)) {
    WARN("Iris: failed to save embedded source field: %s", error.c_str());
  }
}

json_t* Iris::dataToJson() {
  json_t* root = json_object();
  json_object_set_new(root, "version", json_integer(2));
  {
    std::lock_guard<std::mutex> lock(snapshotMutex);
    const std::string sourcePath = snapshotSourceField.sourcePath.empty() ? snapshotTable.sourcePath : snapshotSourceField.sourcePath;
    const std::string sourceName = snapshotSourceField.sourceName.empty() ? snapshotTable.sourceName : snapshotSourceField.sourceName;
    const int sourceWidth = snapshotSourceField.originalWidth > 0 ? snapshotSourceField.originalWidth : snapshotTable.sourceWidth;
    const int sourceHeight = snapshotSourceField.originalHeight > 0 ? snapshotSourceField.originalHeight : snapshotTable.sourceHeight;
    const int sourceChannels = snapshotSourceField.originalChannels > 0 ? snapshotSourceField.originalChannels : snapshotTable.sourceChannels;
    const bool sourceIsNautiloid =
      currentSourceKind == iris::SOURCE_EXPANDER_IMAGE ||
      currentSourceKind == iris::SOURCE_NAUTILOID_FRACTAL;
    json_object_set_new(root, "sourceKind", json_integer(currentSourceKind));
    json_object_set_new(root, "sourceMode", json_string(sourceIsNautiloid ? "nautiloid" : "image"));
    json_object_set_new(root, "sourceModeNautiloidAttached", json_boolean(hasLeftNautiloid(this)));
    json_object_set_new(root, "sourcePath", json_string(sourcePath.c_str()));
    json_object_set_new(root, "sourceName", json_string(sourceName.c_str()));
    json_object_set_new(root, "sourceWidth", json_integer(sourceWidth));
    json_object_set_new(root, "sourceHeight", json_integer(sourceHeight));
    json_object_set_new(root, "sourceChannels", json_integer(sourceChannels));
    json_object_set_new(root, "rowCount", json_integer(snapshotTable.rowCount));
    if (currentSourceKind == iris::SOURCE_NAUTILOID_FRACTAL &&
        iris::sourceHasNautiloidFractalParams(snapshotSourceField)) {
      json_object_set_new(root, "nautiloidFractalVersion",
                          json_integer(snapshotSourceField.generatorVersion));
      json_object_set_new(root, "nautiloidFractalMode",
                          json_integer(snapshotSourceField.generatorFractalMode));
      json_object_set_new(root, "nautiloidFractalZoom",
                          json_real(snapshotSourceField.generatorFractalZoom));
      json_object_set_new(root, "nautiloidFractalCenterX",
                          json_real(snapshotSourceField.generatorFractalCenterX));
      json_object_set_new(root, "nautiloidFractalCenterY",
                          json_real(snapshotSourceField.generatorFractalCenterY));
      json_object_set_new(root, "nautiloidFractalGeneration",
                          json_integer(json_int_t(snapshotSourceField.generatorGeneration)));
    }
  }
  json_t* conversion = json_object();
  json_object_set_new(conversion, "normalizeMode", json_integer(conversionSettings.normalizeMode));
  json_object_set_new(conversion, "rowOrder", json_integer(conversionSettings.rowOrder));
  json_object_set_new(conversion, "trimMode", json_integer(conversionSettings.trimMode));
  json_object_set_new(conversion, "imageChannelMode", json_integer(conversionSettings.imageChannelMode));
  json_object_set_new(conversion, "seamSmoothing", json_real(conversionSettings.seamSmoothing));
  json_object_set_new(conversion, "waveSmoothing", json_real(conversionSettings.waveSmoothing));
  json_object_set_new(conversion, "dcRemove", json_boolean(conversionSettings.dcRemove));
  json_object_set_new(conversion, "invert", json_boolean(conversionSettings.invert));
  json_object_set_new(conversion, "contrast", json_real(conversionSettings.contrast));
  json_object_set_new(conversion, "brightness", json_real(conversionSettings.brightness));
  json_object_set_new(conversion, "gamma", json_real(conversionSettings.gamma));
  json_object_set_new(root, "conversion", conversion);
  json_object_set_new(root, "embedSource", json_boolean(embedSource));
  json_object_set_new(root, "embeddedSourceFile", json_string(kEmbeddedSourceName));
  json_object_set_new(root, "sourceStorageFormat", json_string("rgb8-qoi"));
  json_object_set_new(root, "canonicalSourceWidth", json_integer(iris::kCanonicalSourceWidth));
  json_object_set_new(root, "canonicalSourceHeight", json_integer(iris::kCanonicalSourceHeight));
  json_object_set_new(root, "showSourceColorPreview",
                      json_boolean(displayChannelPreview.load(std::memory_order_relaxed)));
  return root;
}

void Iris::dataFromJson(json_t* root) {
  if (!root) return;
  embedSource = jsonBoolOr(root, "embedSource", jsonBoolOr(root, "embedTable", true));
  displayChannelPreview.store(
    jsonBoolOr(root, "showSourceColorPreview", jsonBoolOr(root, "displayChannelPreview", false)),
    std::memory_order_relaxed);
  previewGeneration.fetch_add(1u, std::memory_order_release);
  {
    std::lock_guard<std::mutex> lock(snapshotMutex);
    json_t* sourceModeJ = json_object_get(root, "sourceMode");
    const bool savedNautiloidMode =
      sourceModeJ && json_is_string(sourceModeJ) &&
      std::string(json_string_value(sourceModeJ)) == "nautiloid";
    const bool savedWithNautiloidAttached =
      jsonBoolOr(root, "sourceModeNautiloidAttached", false);
    const int savedSourceKind = jsonIntegerOr(root, "sourceKind", iris::SOURCE_IMAGE);
    currentSourceKind = iris::SOURCE_IMAGE;
    if (savedSourceKind == iris::SOURCE_EXPANDER_IMAGE ||
        savedSourceKind == iris::SOURCE_NAUTILOID_FRACTAL) {
      currentSourceKind = savedSourceKind;
    } else if (savedNautiloidMode) {
      currentSourceKind = iris::SOURCE_EXPANDER_IMAGE;
    }
    if (savedSourceKind != currentSourceKind) {
      currentSourceKind = iris::SOURCE_IMAGE;
    }
    activeSourceKind.store(currentSourceKind, std::memory_order_release);
    restoredImageSourceMode.store(
      currentSourceKind == iris::SOURCE_IMAGE && savedWithNautiloidAttached,
      std::memory_order_release);
  }
  json_t* conversion = json_object_get(root, "conversion");
  if (conversion) {
    conversionSettings.normalizeMode = iris::NormalizeMode(clamp(
      jsonIntegerOr(conversion, "normalizeMode", iris::NORMALIZE_BALANCED), 0, 3));
    conversionSettings.rowOrder = iris::RowOrder(clamp(
      jsonIntegerOr(conversion, "rowOrder", iris::ROW_TOP_TO_BOTTOM), 0, 1));
    conversionSettings.trimMode = iris::TrimMode(clamp(
      jsonIntegerOr(conversion, "trimMode", iris::TRIM_OFF), 0, 3));
    conversionSettings.imageChannelMode = iris::ImageChannelMode(clamp(
      jsonIntegerOr(conversion, "imageChannelMode", iris::IMAGE_CHANNEL_ALL), 0, 3));
    displayImageChannelMode.store(int(conversionSettings.imageChannelMode), std::memory_order_relaxed);
    conversionSettings.seamSmoothing =
      clamp(jsonRealOr(conversion, "seamSmoothing", 0.f), 0.f, 1.f);
    conversionSettings.waveSmoothing =
      clamp(jsonRealOr(conversion, "waveSmoothing", 0.f), 0.f, 1.f);
    conversionSettings.dcRemove = jsonBoolOr(conversion, "dcRemove", false);
    conversionSettings.invert = jsonBoolOr(conversion, "invert", false);
    conversionSettings.contrast = jsonRealOr(conversion, "contrast", 1.f);
    conversionSettings.brightness = jsonRealOr(conversion, "brightness", 0.f);
    conversionSettings.gamma = jsonRealOr(conversion, "gamma", 1.f);
  }
  json_t* sourcePathJ = json_object_get(root, "sourcePath");
  json_t* sourceNameJ = json_object_get(root, "sourceName");
  json_t* sourceWidthJ = json_object_get(root, "sourceWidth");
  json_t* sourceHeightJ = json_object_get(root, "sourceHeight");
  json_t* sourceChannelsJ = json_object_get(root, "sourceChannels");
  if ((sourcePathJ && json_is_string(sourcePathJ)) || (sourceNameJ && json_is_string(sourceNameJ)) ||
      sourceWidthJ || sourceHeightJ || sourceChannelsJ) {
    std::lock_guard<std::mutex> lock(snapshotMutex);
    if (sourcePathJ && json_is_string(sourcePathJ)) {
      snapshotTable.sourcePath = json_string_value(sourcePathJ);
      snapshotSourceField.sourcePath = json_string_value(sourcePathJ);
    }
    if (sourceNameJ && json_is_string(sourceNameJ)) {
      snapshotTable.sourceName = json_string_value(sourceNameJ);
      snapshotSourceField.sourceName = json_string_value(sourceNameJ);
    }
    snapshotSourceField.originalWidth = jsonIntegerOr(root, "sourceWidth", 0);
    snapshotSourceField.originalHeight = jsonIntegerOr(root, "sourceHeight", 0);
    snapshotSourceField.originalChannels = jsonIntegerOr(root, "sourceChannels", 0);
    if (currentSourceKind == iris::SOURCE_NAUTILOID_FRACTAL) {
      snapshotSourceField.generatorKind = iris::SOURCE_GENERATOR_NAUTILOID_FRACTAL;
      snapshotSourceField.generatorVersion =
        jsonIntegerOr(root, "nautiloidFractalVersion", iris::kNautiloidFractalSourceVersion);
      snapshotSourceField.generatorFractalMode =
        jsonIntegerOr(root, "nautiloidFractalMode", iris::FRACTAL_MANDELBROT);
      snapshotSourceField.generatorFractalZoom =
        clamp(jsonRealOr(root, "nautiloidFractalZoom", 0.f), 0.f, 5.f);
      snapshotSourceField.generatorFractalCenterX =
        clamp(jsonRealOr(root, "nautiloidFractalCenterX", 0.f), -2.f, 2.f);
      snapshotSourceField.generatorFractalCenterY =
        clamp(jsonRealOr(root, "nautiloidFractalCenterY", 0.f), -2.f, 2.f);
      json_t* generationJ = json_object_get(root, "nautiloidFractalGeneration");
      if (generationJ && json_is_integer(generationJ)) {
        const json_int_t generation = json_integer_value(generationJ);
        snapshotSourceField.generatorGeneration = generation > 0 ? uint64_t(generation) : 0u;
      }
      if (!iris::sourceHasNautiloidFractalParams(snapshotSourceField)) {
        currentSourceKind = iris::SOURCE_IMAGE;
        activeSourceKind.store(currentSourceKind, std::memory_order_release);
      }
    }
  }
}

std::string Iris::sourceName() const {
  std::lock_guard<std::mutex> lock(snapshotMutex);
  return snapshotSourceField.sourceName.empty() ? snapshotTable.sourceName : snapshotSourceField.sourceName;
}

std::string Iris::sourcePath() const {
  std::lock_guard<std::mutex> lock(snapshotMutex);
  return snapshotSourceField.sourcePath.empty() ? snapshotTable.sourcePath : snapshotSourceField.sourcePath;
}

int Iris::sourceKind() const {
  return activeSourceKind.load(std::memory_order_acquire);
}

bool Iris::consumeRestoredImageSourceMode() {
  return activeSourceKind.load(std::memory_order_acquire) == iris::SOURCE_IMAGE &&
    restoredImageSourceMode.exchange(false, std::memory_order_acq_rel);
}

std::string Iris::statusText() const {
  if (loading.load(std::memory_order_relaxed)) return "Loading...";
  std::lock_guard<std::mutex> lock(snapshotMutex);
  if (!lastError.empty()) return "Load failed";
  if (!snapshotTable.valid()) return "No image";
  if (!snapshotSourceField.valid() && snapshotTable.sourcePath.empty()) return "Default";
  return std::to_string(snapshotTable.rowCount) + " x " + std::to_string(snapshotTable.frameSize);
}

void Iris::buildPreview(const iris::ImageWavetable& table, std::vector<uint8_t>* pixels) {
  if (!pixels) return;
  const int width = iris::kSourcePreviewWidth;
  const int height = iris::kSourcePreviewHeight;
  pixels->assign(size_t(width * height), 0u);
  if (!table.valid()) return;
  for (int y = 0; y < height; ++y) {
    const float scan = (float(y) + 0.5f) / float(height);
    for (int x = 0; x < width; ++x) {
      const float phase = (float(x) + 0.5f) / float(width);
      const float sample = table.sample(phase, scan);
      (*pixels)[size_t(y * width + x)] = uint8_t(std::round(clamp(sample * 0.5f + 0.5f, 0.f, 1.f) * 255.f));
    }
  }
}

void Iris::previewSnapshot(std::vector<uint8_t>* pixels, int* width, int* height) const {
  std::lock_guard<std::mutex> lock(snapshotMutex);
  if (pixels) *pixels = snapshotPreview;
  if (width) *width = previewWidth;
  if (height) *height = previewHeight;
}

void Iris::sourcePreviewSnapshot(std::vector<uint8_t>* pixels, int* width, int* height) const {
  std::lock_guard<std::mutex> lock(snapshotMutex);
  const size_t previewPixels =
    size_t(snapshotTable.sourcePreviewWidth) * size_t(snapshotTable.sourcePreviewHeight);
  if (previewPixels > 0u && snapshotTable.sourcePreviewRgb.size() == previewPixels * 3u) {
    if (pixels) *pixels = snapshotTable.sourcePreviewRgb;
    if (width) *width = snapshotTable.sourcePreviewWidth;
    if (height) *height = snapshotTable.sourcePreviewHeight;
    return;
  }
  iris::buildDisplayRgb8FromSourceField(snapshotSourceField, pixels, width, height);
}

void Iris::waveformSnapshot(float scan, int sampleCount, std::vector<float>* samples) const {
  if (!samples) return;
  sampleCount = std::max(sampleCount, 2);
  samples->resize(size_t(sampleCount));
  std::lock_guard<std::mutex> lock(snapshotMutex);
  if (!snapshotTable.valid()) {
    std::fill(samples->begin(), samples->end(), 0.f);
    return;
  }
  scan = clamp(scan, 0.f, 1.f);
  for (int i = 0; i < sampleCount; ++i) {
    (*samples)[size_t(i)] = snapshotTable.sample(float(i) / float(sampleCount - 1), scan);
  }
}

float IrisFreqQuantity::getDisplayValue() {
  auto* iris = static_cast<Iris*>(module);
  const bool lfoMode = iris ? (iris->params[Iris::LFO_MODE_PARAM].getValue() > 0.5f) : false;
  return irisBaseFrequencyFromKnob(getValue(), lfoMode);
}

void IrisFreqQuantity::setDisplayValue(float displayValue) {
  auto* iris = static_cast<Iris*>(module);
  const bool lfoMode = iris ? (iris->params[Iris::LFO_MODE_PARAM].getValue() > 0.5f) : false;
  setImmediateValue(irisKnobValueForFrequency(displayValue, lfoMode));
}

std::string IrisFreqQuantity::getDisplayValueString() {
  const float hz = getDisplayValue();
  if (hz >= 1000.f) {
    return string::f("%.2f kHz", hz / 1000.f);
  }
  if (hz < 100.f) {
    return string::f("%.2f Hz", hz);
  }
  return string::f("%.1f Hz", hz);
}
