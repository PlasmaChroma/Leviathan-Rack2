#include "Iris.hpp"

#include <new>

namespace {

const char* kEmbeddedTableName = "iris-table.bin";
std::atomic<uint32_t> gIrisDebugInstanceCounter {1u};

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
  configParam(FM_ATTEN_PARAM, -1.f, 1.f, 0.f, "FM attenuverter", " %", 0.f, 100.f);
  configSwitch(COARSE_STEP_MODE_PARAM, 0.f, 1.f, 0.f, "Octave stepped", {"Continuous", "Octave stepped"});
  configSwitch(SOFT_SYNC_MODE_PARAM, 0.f, 1.f, 0.f, "Sync mode", {"Hard sync", "Soft sync"});
  configInput(V_OCT_INPUT, "V/Oct");
  configInput(FM_INPUT, "Exponential FM");
  configInput(SCAN_INPUT, "Scan CV");
  configInput(SYNC_INPUT, "Sync");
  configOutput(OUT_OUTPUT, "Wavetable");
  configOutput(INV_OUTPUT, "Inverted wavetable");

  activeTable = new iris::ImageWavetable(iris::makeDefaultTable());
  snapshotTable = *activeTable;
  buildPreview(snapshotTable, &snapshotPreview);
  startWorker();
}

Iris::~Iris() {
  stopWorker();
  delete pendingTable.exchange(nullptr, std::memory_order_acq_rel);
  delete retiredTable.exchange(nullptr, std::memory_order_acq_rel);
  delete activeTable;
}

void Iris::startWorker() {
  if (!worker.joinable()) {
    worker = std::thread([this]() { workerLoop(); });
  }
}

void Iris::stopWorker() {
  {
    std::lock_guard<std::mutex> lock(workerMutex);
    workerStop = true;
    requestPending = false;
  }
  workerCv.notify_all();
  if (worker.joinable()) worker.join();
}

void Iris::submitRequest(const WorkerRequest& request) {
  {
    std::lock_guard<std::mutex> lock(workerMutex);
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
  WorkerRequest request;
  request.type = REQUEST_IMAGE;
  request.path = path;
  request.settings = conversionSettings;
  submitRequest(request);
}

void Iris::requestReload() {
  const std::string path = sourcePath();
  if (!path.empty()) requestImageLoad(path);
}

void Iris::requestRebuild() {
  const std::string path = sourcePath();
  if (path.empty()) return;
  WorkerRequest request;
  request.type = REQUEST_REBUILD;
  request.path = path;
  request.settings = conversionSettings;
  submitRequest(request);
}

void Iris::clearToDefault() {
  WorkerRequest request;
  request.type = REQUEST_DEFAULT;
  request.settings = conversionSettings;
  submitRequest(request);
}

void Iris::publishBuiltTable(iris::ImageWavetable* table, bool preserveSourceMetadata) {
  if (!table || !table->valid()) {
    delete table;
    return;
  }
  {
    std::lock_guard<std::mutex> lock(snapshotMutex);
    if (preserveSourceMetadata) {
      table->sourcePath = snapshotTable.sourcePath;
      table->sourceName = snapshotTable.sourceName;
    }
    snapshotTable = *table;
    buildPreview(snapshotTable, &snapshotPreview);
    lastError.clear();
  }
  delete pendingTable.exchange(table, std::memory_order_acq_rel);
  previewGeneration.fetch_add(1u, std::memory_order_release);
  loading.store(false, std::memory_order_release);
  loadFailed.store(false, std::memory_order_release);
}

void Iris::workerLoop() {
  while (true) {
    if (iris::ImageWavetable* retired = retiredTable.exchange(nullptr, std::memory_order_acq_rel)) {
      delete retired;
    }
    WorkerRequest request;
    {
      std::unique_lock<std::mutex> lock(workerMutex);
      workerCv.wait_for(lock, std::chrono::milliseconds(20), [this]() {
        return workerStop || requestPending || retiredTable.load(std::memory_order_acquire) != nullptr;
      });
      if (workerStop) break;
      if (!requestPending) continue;
      request = workerRequest;
      requestPending = false;
    }

    iris::ImageWavetable* built = nullptr;
    std::string error;
    bool ok = false;
    try {
      built = new iris::ImageWavetable;
      if (request.type == REQUEST_DEFAULT) {
        *built = iris::makeDefaultTable();
        ok = true;
      } else if (request.type == REQUEST_EMBEDDED) {
        ok = iris::loadBinaryTable(request.path, built, &error);
      } else {
        ok = iris::importImageFile(request.path, request.settings, built, &error);
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
        delete built;
        continue;
      }
    }
    if (ok) {
      try {
        publishBuiltTable(built, request.type == REQUEST_EMBEDDED);
        built = nullptr;
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
      delete built;
      built = nullptr;
      {
        std::lock_guard<std::mutex> lock(snapshotMutex);
        lastError = error;
      }
      loading.store(false, std::memory_order_release);
      loadFailed.store(true, std::memory_order_release);
    } else {
      built = nullptr;
    }
  }
}

void Iris::process(const ProcessArgs& args) {
  const bool measurePerf = isDragonKingDebugEnabled();
  const auto processStart = debug_terminal::debugTimerStart(measurePerf);
  if (retiredTable.load(std::memory_order_acquire) == nullptr) {
    if (iris::ImageWavetable* pending = pendingTable.exchange(nullptr, std::memory_order_acq_rel)) {
      iris::ImageWavetable* old = activeTable;
      activeTable = pending;
      retiredTable.store(old, std::memory_order_release);
    }
  }
  const iris::ImageWavetable* table = activeTable;
  const int channels = std::max(1, std::min(inputs[V_OCT_INPUT].getChannels(), 16));
  outputs[OUT_OUTPUT].setChannels(channels);
  outputs[INV_OUTPUT].setChannels(channels);
  const float coarseParam = params[COARSE_PARAM].getValue();
  float coarsePitch = kIrisMinPitchFromC4 +
    (std::isfinite(coarseParam) ? clamp(coarseParam, 0.f, 1.f) : irisKnobValueForFrequency(dsp::FREQ_C4)) *
      kIrisCoarseOctaveSpan;
  const bool coarseStepped = params[COARSE_STEP_MODE_PARAM].getValue() > 0.5f;
  if (coarseStepped) coarsePitch = std::round(coarsePitch);
  const float fineParam = params[FINE_PARAM].getValue();
  const float scanParam = params[SCAN_PARAM].getValue();
  const float scanAttenParam = params[SCAN_ATTEN_PARAM].getValue();
  const float fmAttenParam = params[FM_ATTEN_PARAM].getValue();
  const float fine = std::isfinite(fineParam) ? fineParam / 1200.f : 0.f;
  const float scanKnob = std::isfinite(scanParam) ? clamp(scanParam, 0.f, 1.f) : 0.f;
  const float scanAtten = std::isfinite(scanAttenParam) ? clamp(scanAttenParam, -1.f, 1.f) : 0.f;
  const float fmAtten = std::isfinite(fmAttenParam) ? clamp(fmAttenParam, -1.f, 1.f) : 0.f;
  const bool softSync = params[SOFT_SYNC_MODE_PARAM].getValue() > 0.5f;
  float scanDisplay = scanKnob;
  for (int channel = 0; channel < channels; ++channel) {
    const float vOctInput = inputs[V_OCT_INPUT].getPolyVoltage(channel);
    const float fmInput = inputs[FM_INPUT].getPolyVoltage(channel);
    float pitch = coarsePitch + fine + (std::isfinite(vOctInput) ? vOctInput : 0.f) +
                  (std::isfinite(fmInput) ? fmInput : 0.f) * fmAtten;
    pitch = clamp(pitch, -24.f, 16.f);
    const float frequency = clamp(dsp::FREQ_C4 * dsp::exp2_taylor5(pitch), 0.f, args.sampleRate * 0.45f);
    const float scanInput = inputs[SCAN_INPUT].getPolyVoltage(channel);
    float scan = scanKnob + (std::isfinite(scanInput) ? scanInput : 0.f) * 0.1f * scanAtten;
    scan = clamp(scan, 0.f, 1.f);
    if (channel == 0) scanDisplay = scan;
    const float syncInput = inputs[SYNC_INPUT].getPolyVoltage(channel);
    if (voices[size_t(channel)].sync.process(std::isfinite(syncInput) ? syncInput : 0.f)) {
      if (softSync) {
        voices[size_t(channel)].oscillator.softSync();
      } else {
        voices[size_t(channel)].oscillator.reset();
      }
    }
    const float wave = table ? voices[size_t(channel)].oscillator.process(*table, frequency, args.sampleTime, scan) : 0.f;
    const float volts = std::isfinite(wave) ? 5.f * wave : 0.f;
    outputs[OUT_OUTPUT].setVoltage(volts, channel);
    outputs[INV_OUTPUT].setVoltage(-volts, channel);
  }
  displayScan.store(scanDisplay, std::memory_order_relaxed);
  lights[LOAD_LIGHT].setBrightness(loading.load(std::memory_order_relaxed) ? 1.f : 0.f);
  lights[ERROR_LIGHT].setBrightness(loadFailed.load(std::memory_order_relaxed) ? 1.f : 0.f);
  lights[COARSE_STEP_MODE_LIGHT].setBrightness(coarseStepped ? 0.5f : 0.f);
  lights[SOFT_SYNC_MODE_LIGHT].setBrightness(softSync ? 0.5f : 0.f);
  if (measurePerf) {
    debugMetrics.recordProcess(debug_terminal::elapsedNsSince(processStart));
  }
}

void Iris::onAdd(const AddEvent& e) {
  Module::onAdd(e);
  if (!embedTable) return;
  const std::string directory = getPatchStorageDirectory();
  if (directory.empty()) return;
  const std::string tablePath = system::join(directory, kEmbeddedTableName);
  if (!system::isFile(tablePath)) return;
  WorkerRequest request;
  request.type = REQUEST_EMBEDDED;
  request.path = tablePath;
  request.settings = conversionSettings;
  submitRequest(request);
}

void Iris::onSave(const SaveEvent& e) {
  Module::onSave(e);
  if (!embedTable) return;
  iris::ImageWavetable snapshot;
  {
    std::lock_guard<std::mutex> lock(snapshotMutex);
    snapshot = snapshotTable;
  }
  const std::string directory = createPatchStorageDirectory();
  std::string error;
  if (!iris::saveBinaryTable(system::join(directory, kEmbeddedTableName), snapshot, &error)) {
    WARN("Iris: failed to save embedded wavetable: %s", error.c_str());
  }
}

json_t* Iris::dataToJson() {
  json_t* root = json_object();
  json_object_set_new(root, "version", json_integer(1));
  {
    std::lock_guard<std::mutex> lock(snapshotMutex);
    json_object_set_new(root, "sourcePath", json_string(snapshotTable.sourcePath.c_str()));
    json_object_set_new(root, "sourceName", json_string(snapshotTable.sourceName.c_str()));
    json_object_set_new(root, "sourceWidth", json_integer(snapshotTable.sourceWidth));
    json_object_set_new(root, "sourceHeight", json_integer(snapshotTable.sourceHeight));
    json_object_set_new(root, "rowCount", json_integer(snapshotTable.rowCount));
  }
  json_t* conversion = json_object();
  json_object_set_new(conversion, "normalizeMode", json_integer(conversionSettings.normalizeMode));
  json_object_set_new(conversion, "rowOrder", json_integer(conversionSettings.rowOrder));
  json_object_set_new(conversion, "trimMode", json_integer(conversionSettings.trimMode));
  json_object_set_new(conversion, "seamMode", json_integer(conversionSettings.seamMode));
  json_object_set_new(conversion, "smoothingMode", json_integer(conversionSettings.smoothingMode));
  json_object_set_new(conversion, "dcRemove", json_boolean(conversionSettings.dcRemove));
  json_object_set_new(conversion, "invert", json_boolean(conversionSettings.invert));
  json_object_set_new(conversion, "contrast", json_real(conversionSettings.contrast));
  json_object_set_new(conversion, "brightness", json_real(conversionSettings.brightness));
  json_object_set_new(conversion, "gamma", json_real(conversionSettings.gamma));
  json_object_set_new(root, "conversion", conversion);
  json_object_set_new(root, "embedTable", json_boolean(embedTable));
  json_object_set_new(root, "embeddedTableFile", json_string(kEmbeddedTableName));
  return root;
}

void Iris::dataFromJson(json_t* root) {
  if (!root) return;
  embedTable = jsonBoolOr(root, "embedTable", true);
  json_t* conversion = json_object_get(root, "conversion");
  if (conversion) {
    conversionSettings.normalizeMode = iris::NormalizeMode(clamp(
      jsonIntegerOr(conversion, "normalizeMode", iris::NORMALIZE_BALANCED), 0, 3));
    conversionSettings.rowOrder = iris::RowOrder(clamp(
      jsonIntegerOr(conversion, "rowOrder", iris::ROW_TOP_TO_BOTTOM), 0, 1));
    conversionSettings.trimMode = iris::TrimMode(clamp(
      jsonIntegerOr(conversion, "trimMode", iris::TRIM_OFF), 0, 3));
    conversionSettings.seamMode = iris::SeamMode(clamp(
      jsonIntegerOr(conversion, "seamMode", iris::SEAM_OFF), 0, 3));
    conversionSettings.smoothingMode = iris::SmoothingMode(clamp(
      jsonIntegerOr(conversion, "smoothingMode", iris::SMOOTH_OFF), 0, 3));
    conversionSettings.dcRemove = jsonBoolOr(conversion, "dcRemove", false);
    conversionSettings.invert = jsonBoolOr(conversion, "invert", false);
    conversionSettings.contrast = jsonRealOr(conversion, "contrast", 1.f);
    conversionSettings.brightness = jsonRealOr(conversion, "brightness", 0.f);
    conversionSettings.gamma = jsonRealOr(conversion, "gamma", 1.f);
  }
  json_t* sourcePathJ = json_object_get(root, "sourcePath");
  json_t* sourceNameJ = json_object_get(root, "sourceName");
  if ((sourcePathJ && json_is_string(sourcePathJ)) || (sourceNameJ && json_is_string(sourceNameJ))) {
    std::lock_guard<std::mutex> lock(snapshotMutex);
    if (sourcePathJ && json_is_string(sourcePathJ)) snapshotTable.sourcePath = json_string_value(sourcePathJ);
    if (sourceNameJ && json_is_string(sourceNameJ)) snapshotTable.sourceName = json_string_value(sourceNameJ);
  }
}

std::string Iris::sourceName() const {
  std::lock_guard<std::mutex> lock(snapshotMutex);
  return snapshotTable.sourceName;
}

std::string Iris::sourcePath() const {
  std::lock_guard<std::mutex> lock(snapshotMutex);
  return snapshotTable.sourcePath;
}

std::string Iris::statusText() const {
  if (loading.load(std::memory_order_relaxed)) return "Loading...";
  std::lock_guard<std::mutex> lock(snapshotMutex);
  if (!lastError.empty()) return "Load failed";
  if (!snapshotTable.valid()) return "No image";
  return std::to_string(snapshotTable.rowCount) + " x " + std::to_string(snapshotTable.frameSize);
}

void Iris::buildPreview(const iris::ImageWavetable& table, std::vector<uint8_t>* pixels) {
  if (!pixels) return;
  const int width = 128;
  const int height = 64;
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
  return irisBaseFrequencyFromKnob(getValue());
}

void IrisFreqQuantity::setDisplayValue(float displayValue) {
  setImmediateValue(irisKnobValueForFrequency(displayValue));
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
