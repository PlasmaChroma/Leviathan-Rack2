#include "TemporalDeck.hpp"
#include "TemporalDeckBufferModes.hpp"
#include "TemporalDeckEngine.hpp"
#include "TemporalDeckPlatterInput.hpp"
#include "TemporalDeckSamplePrep.hpp"
#include "TemporalDeckUiPublish.hpp"
#include "codec.hpp"

#include <algorithm>
#include <atomic>
#include <cmath>
#include <condition_variable>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <mutex>
#include <new>
#include <string>
#include <thread>
#include <utility>
#include <vector>

// C++11/MinGW can require out-of-class definitions for constexpr static members
// when they are ODR-used.
constexpr int TemporalDeck::CARTRIDGE_CLEAN;
constexpr int TemporalDeck::CARTRIDGE_M44_7;
constexpr int TemporalDeck::CARTRIDGE_ORTOFON_SCRATCH;
constexpr int TemporalDeck::CARTRIDGE_STANTON_680HP;
constexpr int TemporalDeck::CARTRIDGE_QBERT;
constexpr int TemporalDeck::CARTRIDGE_LOFI;
constexpr int TemporalDeck::CARTRIDGE_COUNT;

constexpr int TemporalDeck::SCRATCH_INTERP_CUBIC;
constexpr int TemporalDeck::SCRATCH_INTERP_LAGRANGE6;
constexpr int TemporalDeck::SCRATCH_INTERP_SINC;
constexpr int TemporalDeck::SCRATCH_INTERP_COUNT;

constexpr int TemporalDeck::SLIP_RETURN_SLOW;
constexpr int TemporalDeck::SLIP_RETURN_NORMAL;
constexpr int TemporalDeck::SLIP_RETURN_INSTANT;
constexpr int TemporalDeck::SLIP_RETURN_COUNT;

constexpr int TemporalDeck::BUFFER_DURATION_10S;
constexpr int TemporalDeck::BUFFER_DURATION_20S;
constexpr int TemporalDeck::BUFFER_DURATION_10M_STEREO;
constexpr int TemporalDeck::BUFFER_DURATION_10M_MONO;
constexpr int TemporalDeck::BUFFER_DURATION_COUNT;

constexpr int TemporalDeck::EXTERNAL_GATE_POS_GLIDE;
constexpr int TemporalDeck::EXTERNAL_GATE_POS_MODULE_SYNC;
constexpr int TemporalDeck::EXTERNAL_GATE_POS_COUNT;

constexpr int TemporalDeck::SAMPLE_SOURCE_LIVE;
constexpr int TemporalDeck::SAMPLE_SOURCE_FILE;

constexpr int TemporalDeck::PLATTER_ART_BUILTIN_SVG;
constexpr int TemporalDeck::PLATTER_ART_DRAGON_KING;
constexpr int TemporalDeck::PLATTER_ART_PROCEDURAL;
constexpr int TemporalDeck::PLATTER_ART_CUSTOM;
constexpr int TemporalDeck::PLATTER_ART_BLANK;
constexpr int TemporalDeck::PLATTER_ART_TEMPORAL_DECK;
constexpr int TemporalDeck::PLATTER_ART_MODE_COUNT;

constexpr int TemporalDeck::PLATTER_BRIGHTNESS_FULL;
constexpr int TemporalDeck::PLATTER_BRIGHTNESS_MEDIUM;
constexpr int TemporalDeck::PLATTER_BRIGHTNESS_LOW;
constexpr int TemporalDeck::PLATTER_BRIGHTNESS_COUNT;

constexpr float TemporalDeck::kNominalPlatterRpm;
constexpr float TemporalDeck::kMouseScratchTravelScale;
constexpr float TemporalDeck::kWheelScratchTravelScale;
constexpr float TemporalDeck::kUiPublishRateHz;
constexpr float TemporalDeck::kUiPublishIntervalSec;
constexpr int TemporalDeck::kArcLightCount;

using temporaldeck::TemporalDeckEngine;

namespace {
using temporaldeck_modes::isMonoBufferMode;
using temporaldeck_modes::realBufferSecondsForMode;
using temporaldeck_modes::usableBufferSecondsForMode;

using temporaldeck::DecodedSampleFile;
using temporaldeck::decodeSampleFile;
using temporaldeck::PreparedSampleData;
using temporaldeck::buildPreparedSample;
using temporaldeck::chooseSampleBufferMode;
using temporaldeck::PlatterInputSnapshot;
using temporaldeck::PlatterInputState;

static std::string normalizePathForPrefixCompare(std::string path) {
  std::replace(path.begin(), path.end(), '\\', '/');
  while (path.size() > 1 && path.back() == '/') {
    path.pop_back();
  }
  return path;
}

static bool hasPathPrefix(const std::string &path, const std::string &prefix) {
  if (path.empty() || prefix.empty() || path.size() < prefix.size()) {
    return false;
  }
  if (path.compare(0, prefix.size(), prefix) != 0) {
    return false;
  }
  if (path.size() == prefix.size()) {
    return true;
  }
  char next = path[prefix.size()];
  return next == '/' || next == '\\';
}

static bool isManagedVinylArtPath(const std::string &path) {
  if (path.empty() || !pluginInstance) {
    return false;
  }
  std::string normalizedPath = normalizePathForPrefixCompare(path);
  std::string builtInRoot = normalizePathForPrefixCompare(asset::plugin(pluginInstance, "res/Vinyl"));
  std::string expandedRoot = normalizePathForPrefixCompare(system::join(asset::user(), "Leviathan/TemporalDeck/Vinyl"));
  return hasPathPrefix(normalizedPath, builtInRoot) || hasPathPrefix(normalizedPath, expandedRoot);
}

static double clampd(double x, double a, double b) {
  return std::max(a, std::min(x, b));
}

static int nextCartridgeCharacter(int current) {
  switch (current) {
  case TemporalDeck::CARTRIDGE_CLEAN:
    return TemporalDeck::CARTRIDGE_M44_7;
  case TemporalDeck::CARTRIDGE_M44_7:
    return TemporalDeck::CARTRIDGE_ORTOFON_SCRATCH;
  case TemporalDeck::CARTRIDGE_ORTOFON_SCRATCH:
    return TemporalDeck::CARTRIDGE_QBERT;
  case TemporalDeck::CARTRIDGE_QBERT:
    return TemporalDeck::CARTRIDGE_STANTON_680HP;
  case TemporalDeck::CARTRIDGE_STANTON_680HP:
    return TemporalDeck::CARTRIDGE_LOFI;
  case TemporalDeck::CARTRIDGE_LOFI:
  default:
    return TemporalDeck::CARTRIDGE_CLEAN;
  }
}


struct DeckRateQuantity : ParamQuantity {
  static float valueForSpeed(float speed) {
    speed = clamp(speed, 0.5f, 2.f);
    if (speed <= 1.f) {
      return speed - 0.5f;
    }
    return speed * 0.5f;
  }

  float getDisplayValue() override { return TemporalDeckEngine::baseSpeedFromKnob(getValue()); }

  void setDisplayValue(float displayValue) override { setImmediateValue(valueForSpeed(displayValue)); }

  std::string getDisplayValueString() override {
    return string::f("%.2fx", TemporalDeckEngine::baseSpeedFromKnob(getValue()));
  }
};

struct ScratchSensitivityQuantity : ParamQuantity {
  static float sensitivityForValue(float v) {
    if (v <= 0.5f) {
      return rescale(v, 0.f, 0.5f, 0.5f, 1.f);
    }
    return rescale(v, 0.5f, 1.f, 1.f, 2.f);
  }

  std::string getDisplayValueString() override { return string::f("%.2fx", sensitivityForValue(getValue())); }

  std::string getLabel() override { return "Scratch sensitivity"; }
};

} // namespace

struct TemporalDeck::Impl {
  struct AsyncSampleBuildRequest {
    enum Type {
      NONE = 0,
      LOAD_PATH = 1,
      REBUILD_FROM_DECODED = 2,
    };
    int type = NONE;
    std::string path;
    float targetSampleRate = 44100.f;
    int requestedBufferMode = TemporalDeck::BUFFER_DURATION_10S;
  };

  TemporalDeckEngine engine;
  dsp::SchmittTrigger freezeTrigger;
  dsp::SchmittTrigger reverseTrigger;
  dsp::SchmittTrigger slipTrigger;
  dsp::SchmittTrigger cartridgeCycleTrigger;
  float cachedSampleRate = 0.f;
  bool freezeLatched = false;
  bool freezeLatchedByButton = false;
  bool reverseLatched = false;
  bool slipLatched = false;
  bool prevFreezeGateHigh = false;
  std::atomic<bool> sampleModeEnabled{false};
  std::atomic<bool> sampleLoopEnabled{false};
  bool sampleAutoPlayOnLoad = true;
  std::mutex sampleStateMutex;
  std::atomic<bool> pendingSampleStateApply{false};
  std::string samplePath;
  std::string sampleDisplayName;
  DecodedSampleFile decodedSample;
  std::atomic<bool> decodedSampleAvailable{false};
  std::mutex preparedSampleMutex;
  PreparedSampleData preparedSample;
  std::atomic<bool> pendingPreparedSampleInstall{false};
  std::thread sampleBuildThread;
  std::mutex sampleBuildMutex;
  std::condition_variable sampleBuildCv;
  bool sampleBuildStop = false;
  bool sampleBuildHasRequest = false;
  AsyncSampleBuildRequest sampleBuildRequest;
  std::atomic<bool> sampleBuildInProgress{false};
  std::atomic<uint64_t> sampleBuildRequestSerial{0};
  uint64_t sampleBuildAppliedSerial = 0;
  std::atomic<bool> allocationFallbackPending{false};
  PlatterInputState platterInput;
  std::atomic<float> pendingSampleSeekNormalized{0.f};
  std::atomic<uint32_t> pendingSampleSeekRevision{0};
  uint32_t appliedSampleSeekRevision = 0;
  std::atomic<double> uiLagSamples{0.0};
  std::atomic<double> uiAccessibleLagSamples{0.0};
  std::atomic<float> uiSampleRate{44100.f};
  std::atomic<float> uiPlatterAngle{0.f};
  std::atomic<bool> uiFreezeLatched{false};
  std::atomic<bool> uiSampleModeEnabled{false};
  std::atomic<bool> uiSampleLoaded{false};
  std::atomic<bool> uiSampleTransportPlaying{false};
  std::atomic<double> uiSamplePlayheadSeconds{0.0};
  std::atomic<double> uiSampleDurationSeconds{0.0};
  std::atomic<double> uiSampleProgress{0.0};
  float uiPublishTimerSec = 0.f;
  int scratchInterpolationMode = TemporalDeck::SCRATCH_INTERP_LAGRANGE6;
  bool platterTraceLoggingEnabled = false;
  int cartridgeCharacter = TemporalDeck::CARTRIDGE_CLEAN;
  std::atomic<int> bufferDurationMode{TemporalDeck::BUFFER_DURATION_10S};
  int externalGatePosMode = TemporalDeck::EXTERNAL_GATE_POS_GLIDE;
  int slipReturnMode = TemporalDeck::SLIP_RETURN_NORMAL;
  int platterArtMode = TemporalDeck::PLATTER_ART_DRAGON_KING;
  int platterBrightnessMode = TemporalDeck::PLATTER_BRIGHTNESS_FULL;
  std::string customPlatterArtPath;
  bool pendingInitialPlatterArtSelection = true;

  void requestAsyncSampleBuild(const AsyncSampleBuildRequest &request) {
    {
      std::lock_guard<std::mutex> lock(sampleBuildMutex);
      sampleBuildRequest = request;
      sampleBuildHasRequest = true;
      sampleBuildRequestSerial.fetch_add(1, std::memory_order_relaxed);
      sampleBuildInProgress.store(true, std::memory_order_relaxed);
    }
    sampleBuildCv.notify_one();
  }

  void startSampleBuildWorker() {
    sampleBuildStop = false;
    sampleBuildThread = std::thread([this]() {
      while (true) {
        AsyncSampleBuildRequest request;
        uint64_t requestSerial = 0;
        {
          std::unique_lock<std::mutex> lock(sampleBuildMutex);
          sampleBuildCv.wait(lock, [this]() { return sampleBuildStop || sampleBuildHasRequest; });
          if (sampleBuildStop) {
            break;
          }
          request = sampleBuildRequest;
          sampleBuildHasRequest = false;
          requestSerial = sampleBuildRequestSerial.load(std::memory_order_relaxed);
        }

        DecodedSampleFile decoded;
        bool autoPlayOnLoad = true;
        bool validDecoded = false;

        if (request.type == AsyncSampleBuildRequest::LOAD_PATH) {
          std::string decodeError;
          bool decodeOk = false;
          try {
            decodeOk = decodeSampleFile(request.path, &decoded, &decodeError);
          } catch (const std::bad_alloc &) {
            WARN("TemporalDeck: sample decode allocation failed, falling back to 10s live mode");
            allocationFallbackPending.store(true, std::memory_order_relaxed);
            pendingSampleStateApply.store(true, std::memory_order_relaxed);
            sampleBuildInProgress.store(false, std::memory_order_relaxed);
            continue;
          }
          if (!decodeOk) {
            WARN("TemporalDeck: sample decode failed for '%s': %s", request.path.c_str(), decodeError.c_str());
            sampleBuildInProgress.store(false, std::memory_order_relaxed);
            continue;
          }
          {
            std::lock_guard<std::mutex> lock(sampleStateMutex);
            samplePath = request.path;
            sampleDisplayName = system::getFilename(request.path);
            decodedSample = decoded;
            autoPlayOnLoad = sampleAutoPlayOnLoad;
            decodedSampleAvailable.store(decodedSample.frames > 0 && !decodedSample.left.empty(),
                                        std::memory_order_relaxed);
          }
          validDecoded = decoded.frames > 0 && !decoded.left.empty();
        } else if (request.type == AsyncSampleBuildRequest::REBUILD_FROM_DECODED) {
          std::lock_guard<std::mutex> lock(sampleStateMutex);
          decoded = decodedSample;
          autoPlayOnLoad = sampleAutoPlayOnLoad;
          validDecoded = decoded.frames > 0 && !decoded.left.empty();
        }

        if (!validDecoded) {
          sampleBuildInProgress.store(false, std::memory_order_relaxed);
          continue;
        }

        int targetMode = request.requestedBufferMode;
        if (request.type == AsyncSampleBuildRequest::LOAD_PATH) {
          targetMode = chooseSampleBufferMode(decoded);
          bufferDurationMode.store(targetMode, std::memory_order_relaxed);
        }

        PreparedSampleData prepared;
        try {
          if (buildPreparedSample(decoded, request.targetSampleRate, targetMode, autoPlayOnLoad, &prepared)) {
            if (requestSerial == sampleBuildRequestSerial.load(std::memory_order_relaxed)) {
              std::lock_guard<std::mutex> lock(preparedSampleMutex);
              preparedSample = std::move(prepared);
              pendingPreparedSampleInstall.store(true, std::memory_order_relaxed);
            }
          }
        } catch (const std::bad_alloc &) {
          WARN("TemporalDeck: sample prep allocation failed, falling back to 10s live mode");
          allocationFallbackPending.store(true, std::memory_order_relaxed);
          pendingSampleStateApply.store(true, std::memory_order_relaxed);
        }
        sampleBuildInProgress.store(false, std::memory_order_relaxed);
      }
    });
  }

  void stopSampleBuildWorker() {
    {
      std::lock_guard<std::mutex> lock(sampleBuildMutex);
      sampleBuildStop = true;
      sampleBuildHasRequest = false;
    }
    sampleBuildCv.notify_all();
    if (sampleBuildThread.joinable()) {
      sampleBuildThread.join();
    }
  }
};

struct ProcessSignalInputs {
  float inL = 0.f;
  float inR = 0.f;
  float positionCv = 0.f;
  float rateCv = 0.f;
  bool rateCvConnected = false;
  bool freezeGateHigh = false;
  bool scratchGateHigh = false;
  bool scratchGateConnected = false;
  bool positionConnected = false;
};

static ProcessSignalInputs readProcessSignalInputs(TemporalDeck &module) {
  ProcessSignalInputs in;
  in.inL = module.inputs[TemporalDeck::INPUT_L_INPUT].getVoltage();
  in.inR = module.inputs[TemporalDeck::INPUT_R_INPUT].isConnected()
             ? module.inputs[TemporalDeck::INPUT_R_INPUT].getVoltage()
             : in.inL;
  in.positionCv = module.inputs[TemporalDeck::POSITION_CV_INPUT].getVoltage();
  in.rateCv = module.inputs[TemporalDeck::RATE_CV_INPUT].getVoltage();
  in.rateCvConnected = module.inputs[TemporalDeck::RATE_CV_INPUT].isConnected();
  in.freezeGateHigh =
    module.inputs[TemporalDeck::FREEZE_GATE_INPUT].getVoltage() >= TemporalDeckEngine::kFreezeGateThreshold;
  in.scratchGateHigh =
    module.inputs[TemporalDeck::SCRATCH_GATE_INPUT].getVoltage() >= TemporalDeckEngine::kScratchGateThreshold;
  in.scratchGateConnected = module.inputs[TemporalDeck::SCRATCH_GATE_INPUT].isConnected();
  in.positionConnected = module.inputs[TemporalDeck::POSITION_CV_INPUT].isConnected();
  return in;
}

static void writeFrameOutputs(TemporalDeck &module, const TemporalDeckEngine::FrameResult &frame) {
  module.outputs[TemporalDeck::OUTPUT_L_OUTPUT].setVoltage(frame.outL);
  module.outputs[TemporalDeck::S_GATE_O_OUTPUT].setVoltage(frame.scratchGateOut);
  module.outputs[TemporalDeck::OUTPUT_R_OUTPUT].setVoltage(frame.outR);
  module.outputs[TemporalDeck::S_POS_O_OUTPUT].setVoltage(frame.scratchPosOut);
}

static void updateTransportModeLights(TemporalDeck &module, bool freezeActive, bool reverseLatched, bool slipLatched,
                                      int slipReturnMode) {
  module.lights[TemporalDeck::FREEZE_LIGHT].setBrightness(freezeActive ? 1.f : 0.f);
  module.lights[TemporalDeck::REVERSE_LIGHT].setBrightness(reverseLatched ? 1.f : 0.f);
  if (!slipLatched) {
    module.lights[TemporalDeck::SLIP_SLOW_LIGHT].setBrightness(0.f);
    module.lights[TemporalDeck::SLIP_LIGHT].setBrightness(0.f);
    module.lights[TemporalDeck::SLIP_FAST_LIGHT].setBrightness(0.f);
    return;
  }
  float selectedModeBrightness = 1.f;
  float unselectedModeBrightness = 0.03f;
  module.lights[TemporalDeck::SLIP_SLOW_LIGHT].setBrightness(
    slipReturnMode == TemporalDeck::SLIP_RETURN_SLOW ? selectedModeBrightness : unselectedModeBrightness);
  module.lights[TemporalDeck::SLIP_LIGHT].setBrightness(
    slipReturnMode == TemporalDeck::SLIP_RETURN_NORMAL ? selectedModeBrightness : unselectedModeBrightness);
  module.lights[TemporalDeck::SLIP_FAST_LIGHT].setBrightness(
    slipReturnMode == TemporalDeck::SLIP_RETURN_INSTANT ? selectedModeBrightness : unselectedModeBrightness);
}

TemporalDeck::TemporalDeck() : impl(new Impl()) {
  config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
  configParam(BUFFER_PARAM, 0.f, 1.f, 1.f, "Buffer", " s", 0.f, 10.f);
  configParam<DeckRateQuantity>(RATE_PARAM, 0.f, 1.f, 0.5f, "Rate");
  configParam<ScratchSensitivityQuantity>(SCRATCH_SENSITIVITY_PARAM, 0.f, 1.f, 0.5f, "Scratch sensitivity");
  configParam(MIX_PARAM, 0.f, 1.f, 1.f, "Mix");
  configParam(FEEDBACK_PARAM, 0.f, 1.f, 0.f, "Feedback");
  configButton(FREEZE_PARAM, "Freeze");
  configButton(REVERSE_PARAM, "Reverse");
  configButton(SLIP_PARAM, "Slip");
  configButton(CARTRIDGE_CYCLE_PARAM, "Cycle cartridge");
  configInput(POSITION_CV_INPUT, "Position CV");
  configInput(RATE_CV_INPUT, "Rate CV");
  configInput(INPUT_L_INPUT, "Left audio");
  configInput(INPUT_R_INPUT, "Right audio");
  configInput(SCRATCH_GATE_INPUT, "Scratch gate");
  configInput(FREEZE_GATE_INPUT, "Freeze gate");
  configOutput(OUTPUT_L_OUTPUT, "Left audio");
  configOutput(S_GATE_O_OUTPUT, "Scratch gate");
  configOutput(OUTPUT_R_OUTPUT, "Right audio");
  configOutput(S_POS_O_OUTPUT, "Scratch position");
  if (paramQuantities[BUFFER_PARAM]) {
    int mode = clamp(impl->bufferDurationMode.load(), 0, BUFFER_DURATION_COUNT - 1);
    paramQuantities[BUFFER_PARAM]->displayMultiplier = usableBufferSecondsForMode(mode);
  }
  impl->startSampleBuildWorker();
  applySampleRateChange(APP->engine->getSampleRate());
}

TemporalDeck::~TemporalDeck() {
  if (impl) {
    impl->stopSampleBuildWorker();
  }
}

float TemporalDeck::scratchSensitivity() {
  return ScratchSensitivityQuantity::sensitivityForValue(params[SCRATCH_SENSITIVITY_PARAM].getValue());
}

void TemporalDeck::applyBufferDurationMode(int mode) {
  int clamped = clamp(mode, 0, BUFFER_DURATION_COUNT - 1);
  impl->bufferDurationMode.store(clamped);
  if (paramQuantities[BUFFER_PARAM]) {
    paramQuantities[BUFFER_PARAM]->displayMultiplier = usableBufferSecondsForMode(clamped);
  }
}

void TemporalDeck::applySampleRateChange(float sampleRate) {
  impl->cachedSampleRate = sampleRate;
  int mode = clamp(impl->bufferDurationMode.load(), 0, BUFFER_DURATION_COUNT - 1);
  bool sampleModeEnabled = impl->sampleModeEnabled.load(std::memory_order_relaxed);
  bool sampleLoopEnabled = impl->sampleLoopEnabled.load(std::memory_order_relaxed);
  auto applyUiState = [&](int uiMode) {
    impl->uiSampleRate.store(impl->cachedSampleRate);
    impl->uiLagSamples.store(0.0);
    impl->uiAccessibleLagSamples.store(0.0);
    impl->uiPlatterAngle.store(0.f);
    impl->uiFreezeLatched.store(false);
    impl->uiSampleModeEnabled.store(impl->engine.sampleModeEnabled);
    impl->uiSampleLoaded.store(impl->engine.sampleLoaded);
    impl->uiSampleTransportPlaying.store(impl->engine.sampleTransportPlaying);
    impl->uiSamplePlayheadSeconds.store(0.0);
    impl->uiSampleDurationSeconds.store(
      impl->engine.sampleLoaded ? double(impl->engine.sampleFrames) / std::max(double(impl->cachedSampleRate), 1.0) : 0.0);
    impl->uiSampleProgress.store(0.0);
    if (paramQuantities[BUFFER_PARAM]) {
      float displaySeconds = usableBufferSecondsForMode(uiMode);
      if (impl->engine.sampleLoaded && impl->engine.sampleFrames > 0) {
        displaySeconds = float(impl->engine.sampleFrames) / std::max(impl->cachedSampleRate, 1.f);
      }
      paramQuantities[BUFFER_PARAM]->displayMultiplier = displaySeconds;
    }
    impl->uiPublishTimerSec = 0.f;
    impl->platterInput.resetAudioHoldState();
    for (int i = 0; i < kArcLightCount; ++i) {
      lights[ARC_LIGHT_START + i].setBrightness(0.f);
      lights[ARC_MAX_LIGHT_START + i].setBrightness(0.f);
    }
  };

  try {
    impl->engine.bufferDurationMode = mode;
    impl->engine.reset(impl->cachedSampleRate);
    impl->engine.sampleModeEnabled = sampleModeEnabled;
    impl->engine.sampleLoopEnabled = sampleLoopEnabled;
    impl->engine.externalGatePosMode = impl->externalGatePosMode;
  } catch (const std::bad_alloc &) {
    WARN("TemporalDeck: buffer allocation failed, forcing 10s live fallback");
    {
      std::lock_guard<std::mutex> lock(impl->sampleStateMutex);
      impl->samplePath.clear();
      impl->sampleDisplayName.clear();
      impl->decodedSample = DecodedSampleFile();
    }
    impl->decodedSampleAvailable.store(false, std::memory_order_relaxed);
    impl->pendingPreparedSampleInstall.store(false, std::memory_order_relaxed);
    {
      std::lock_guard<std::mutex> lock(impl->preparedSampleMutex);
      impl->preparedSample = PreparedSampleData();
    }
    impl->sampleModeEnabled.store(false, std::memory_order_relaxed);
    impl->bufferDurationMode.store(BUFFER_DURATION_10S, std::memory_order_relaxed);
    mode = BUFFER_DURATION_10S;
    impl->engine.bufferDurationMode = mode;
    impl->engine.reset(impl->cachedSampleRate);
    impl->engine.sampleModeEnabled = false;
    impl->engine.sampleLoopEnabled = sampleLoopEnabled;
    impl->engine.externalGatePosMode = impl->externalGatePosMode;
  }
  applyUiState(mode);
}

void TemporalDeck::onSampleRateChange() {
  // Reconfiguration is applied on the audio thread from process() to avoid
  // cross-thread buffer reallocations.
}

json_t *TemporalDeck::dataToJson() {
  json_t *root = json_object();
  bool sampleModeEnabled = impl->sampleModeEnabled.load(std::memory_order_relaxed);
  bool sampleAutoPlayOnLoad = true;
  std::string samplePath;
  {
    std::lock_guard<std::mutex> lock(impl->sampleStateMutex);
    sampleAutoPlayOnLoad = impl->sampleAutoPlayOnLoad;
    samplePath = impl->samplePath;
  }
  json_object_set_new(root, "freezeLatched", json_boolean(impl->freezeLatched));
  json_object_set_new(root, "reverseLatched", json_boolean(impl->reverseLatched));
  json_object_set_new(root, "slipLatched", json_boolean(impl->slipLatched));
  json_object_set_new(root, "scratchInterpolationMode", json_integer(impl->scratchInterpolationMode));
  json_object_set_new(root, "platterTraceLoggingEnabled", json_boolean(impl->platterTraceLoggingEnabled));
  json_object_set_new(root, "externalGatePosMode", json_integer(impl->externalGatePosMode));
  json_object_set_new(root, "slipReturnMode", json_integer(impl->slipReturnMode));
  json_object_set_new(root, "cartridgeCharacter", json_integer(impl->cartridgeCharacter));
  json_object_set_new(root, "bufferDurationMode", json_integer(impl->bufferDurationMode.load()));
  json_object_set_new(root, "sampleModeEnabled", json_boolean(sampleModeEnabled));
  json_object_set_new(root, "sampleLoopEnabled", json_boolean(impl->sampleLoopEnabled.load(std::memory_order_relaxed)));
  json_object_set_new(root, "sampleAutoPlayOnLoad", json_boolean(sampleAutoPlayOnLoad));
  json_object_set_new(root, "platterArtMode", json_integer(impl->platterArtMode));
  json_object_set_new(root, "platterBrightnessMode", json_integer(impl->platterBrightnessMode));
  if (!impl->customPlatterArtPath.empty()) {
    json_object_set_new(root, "customPlatterArtPath", json_string(impl->customPlatterArtPath.c_str()));
  }
  if (!samplePath.empty()) {
    json_object_set_new(root, "samplePath", json_string(samplePath.c_str()));
  }
  return root;
}

void TemporalDeck::dataFromJson(json_t *root) {
  if (!root) {
    return;
  }
  impl->pendingInitialPlatterArtSelection = false;
  json_t *freezeJ = json_object_get(root, "freezeLatched");
  json_t *reverseJ = json_object_get(root, "reverseLatched");
  json_t *slipJ = json_object_get(root, "slipLatched");
  json_t *scratchInterpModeJ = json_object_get(root, "scratchInterpolationMode");
  json_t *platterTraceLoggingJ = json_object_get(root, "platterTraceLoggingEnabled");
  json_t *externalGatePosModeJ = json_object_get(root, "externalGatePosMode");
  json_t *slipReturnModeJ = json_object_get(root, "slipReturnMode");
  json_t *cartridgeJ = json_object_get(root, "cartridgeCharacter");
  json_t *bufferDurationJ = json_object_get(root, "bufferDurationMode");
  json_t *sampleModeEnabledJ = json_object_get(root, "sampleModeEnabled");
  json_t *sampleLoopEnabledJ = json_object_get(root, "sampleLoopEnabled");
  json_t *platterArtModeJ = json_object_get(root, "platterArtMode");
  json_t *platterBrightnessModeJ = json_object_get(root, "platterBrightnessMode");
  json_t *customPlatterArtPathJ = json_object_get(root, "customPlatterArtPath");
  json_t *samplePathJ = json_object_get(root, "samplePath");
  if (freezeJ) {
    impl->freezeLatched = json_boolean_value(freezeJ);
    impl->freezeLatchedByButton = impl->freezeLatched;
  }
  if (reverseJ) {
    impl->reverseLatched = json_boolean_value(reverseJ);
  }
  if (slipJ) {
    impl->slipLatched = json_boolean_value(slipJ);
  }
  if (scratchInterpModeJ) {
    impl->scratchInterpolationMode =
      clamp((int)json_integer_value(scratchInterpModeJ), SCRATCH_INTERP_CUBIC, SCRATCH_INTERP_COUNT - 1);
  }
  if (platterTraceLoggingJ) {
    impl->platterTraceLoggingEnabled = json_boolean_value(platterTraceLoggingJ);
  }
  if (externalGatePosModeJ) {
    impl->externalGatePosMode =
      clamp((int)json_integer_value(externalGatePosModeJ), EXTERNAL_GATE_POS_GLIDE, EXTERNAL_GATE_POS_COUNT - 1);
  }
  if (slipReturnModeJ) {
    impl->slipReturnMode = clamp((int)json_integer_value(slipReturnModeJ), SLIP_RETURN_SLOW, SLIP_RETURN_COUNT - 1);
  }
  if (cartridgeJ) {
    impl->cartridgeCharacter = clamp((int)json_integer_value(cartridgeJ), 0, CARTRIDGE_COUNT - 1);
  }
  if (bufferDurationJ) {
    impl->bufferDurationMode.store(clamp((int)json_integer_value(bufferDurationJ), 0, BUFFER_DURATION_COUNT - 1));
  }
  if (sampleModeEnabledJ) {
    impl->sampleModeEnabled.store(json_boolean_value(sampleModeEnabledJ), std::memory_order_relaxed);
  }
  if (sampleLoopEnabledJ) {
    impl->sampleLoopEnabled.store(json_boolean_value(sampleLoopEnabledJ), std::memory_order_relaxed);
  }
  {
    std::lock_guard<std::mutex> lock(impl->sampleStateMutex);
    impl->sampleAutoPlayOnLoad = true;
  }
  if (platterArtModeJ) {
    impl->platterArtMode =
      clamp((int)json_integer_value(platterArtModeJ), PLATTER_ART_BUILTIN_SVG, PLATTER_ART_MODE_COUNT - 1);
  }
  if (platterBrightnessModeJ) {
    impl->platterBrightnessMode =
      clamp((int)json_integer_value(platterBrightnessModeJ), PLATTER_BRIGHTNESS_FULL, PLATTER_BRIGHTNESS_COUNT - 1);
  }
  if (customPlatterArtPathJ && json_is_string(customPlatterArtPathJ)) {
    impl->customPlatterArtPath = json_string_value(customPlatterArtPathJ);
  }
  if (!isDragonKingDebugEnabled()) {
    bool preserveManagedCustom =
      (impl->platterArtMode == PLATTER_ART_CUSTOM) && isManagedVinylArtPath(impl->customPlatterArtPath);
    if (impl->platterArtMode == PLATTER_ART_CUSTOM && !preserveManagedCustom) {
      impl->platterArtMode = PLATTER_ART_DRAGON_KING;
    }
    if (!preserveManagedCustom) {
      impl->customPlatterArtPath.clear();
    }
  }
  int mode = clamp(impl->bufferDurationMode.load(), 0, BUFFER_DURATION_COUNT - 1);
  if (paramQuantities[BUFFER_PARAM]) {
    paramQuantities[BUFFER_PARAM]->displayMultiplier = usableBufferSecondsForMode(mode);
  }
  if (samplePathJ && json_is_string(samplePathJ)) {
    std::string error;
    loadSampleFromPath(json_string_value(samplePathJ), &error);
  }
  // Ensure restored patch state (buffer mode, sample state, etc.) is applied
  // to the runtime buffer allocation on the first audio process callback.
  impl->pendingSampleStateApply.store(true);
}

void TemporalDeck::process(const ProcessArgs &args) {
  if (impl->allocationFallbackPending.exchange(false, std::memory_order_relaxed)) {
    {
      std::lock_guard<std::mutex> lock(impl->sampleStateMutex);
      impl->samplePath.clear();
      impl->sampleDisplayName.clear();
      impl->decodedSample = DecodedSampleFile();
    }
    impl->decodedSampleAvailable.store(false, std::memory_order_relaxed);
    impl->pendingPreparedSampleInstall.store(false, std::memory_order_relaxed);
    {
      std::lock_guard<std::mutex> lock(impl->preparedSampleMutex);
      impl->preparedSample = PreparedSampleData();
    }
    impl->sampleModeEnabled.store(false, std::memory_order_relaxed);
    impl->bufferDurationMode.store(BUFFER_DURATION_10S, std::memory_order_relaxed);
    impl->pendingSampleStateApply.store(true, std::memory_order_relaxed);
  }

  if (impl->pendingPreparedSampleInstall.exchange(false, std::memory_order_relaxed)) {
    PreparedSampleData prepared;
    bool havePrepared = false;
    {
      std::lock_guard<std::mutex> lock(impl->preparedSampleMutex);
      if (impl->preparedSample.valid) {
        prepared = std::move(impl->preparedSample);
        impl->preparedSample = PreparedSampleData();
        havePrepared = true;
      }
    }
    if (havePrepared) {
      impl->sampleBuildAppliedSerial = impl->sampleBuildRequestSerial.load(std::memory_order_relaxed);
      impl->cachedSampleRate = prepared.sampleRate;
      impl->bufferDurationMode.store(prepared.bufferMode, std::memory_order_relaxed);
      impl->engine.bufferDurationMode = prepared.bufferMode;
      impl->engine.reset(prepared.sampleRate, false);
      impl->engine.sampleModeEnabled = impl->sampleModeEnabled.load(std::memory_order_relaxed);
      impl->engine.sampleLoopEnabled = impl->sampleLoopEnabled.load(std::memory_order_relaxed);
      impl->engine.externalGatePosMode = impl->externalGatePosMode;
      impl->engine.installPreparedSample(std::move(prepared.left), std::move(prepared.right), prepared.frames,
                                         prepared.autoPlayOnLoad, prepared.truncated, prepared.monoStorage);
      impl->sampleModeEnabled.store(true, std::memory_order_relaxed);
      if (paramQuantities[BUFFER_PARAM]) {
        paramQuantities[BUFFER_PARAM]->displayMultiplier =
          float(impl->engine.sampleFrames) / std::max(prepared.sampleRate, 1.f);
      }
      impl->sampleBuildInProgress.store(false, std::memory_order_relaxed);
    }
  }

  int requestedBufferMode = clamp(impl->bufferDurationMode.load(std::memory_order_relaxed), 0, BUFFER_DURATION_COUNT - 1);
  bool bufferModeChanged = requestedBufferMode != impl->engine.bufferDurationMode;
  bool sampleStateApplyRequested = impl->pendingSampleStateApply.exchange(false);
  bool sampleRateChanged = args.sampleRate != impl->cachedSampleRate;
  bool decodedAvailable = impl->decodedSampleAvailable.load(std::memory_order_relaxed);
  bool shouldRebuildLoadedSample = decodedAvailable && (bufferModeChanged || sampleRateChanged || sampleStateApplyRequested);
  if (!decodedAvailable && (bufferModeChanged || sampleRateChanged || sampleStateApplyRequested)) {
    applySampleRateChange(args.sampleRate);
  } else if (shouldRebuildLoadedSample && !impl->sampleBuildInProgress.load(std::memory_order_relaxed)) {
    Impl::AsyncSampleBuildRequest request;
    request.type = Impl::AsyncSampleBuildRequest::REBUILD_FROM_DECODED;
    request.targetSampleRate = args.sampleRate;
    request.requestedBufferMode = requestedBufferMode;
    impl->requestAsyncSampleBuild(request);
  }

  bool desiredSampleModeEnabled = impl->sampleModeEnabled.load(std::memory_order_relaxed);

  if (impl->freezeTrigger.process(params[FREEZE_PARAM].getValue())) {
    bool next = !impl->freezeLatched;
    impl->freezeLatched = next;
    impl->freezeLatchedByButton = next;
    if (next) {
      impl->reverseLatched = false;
      impl->slipLatched = false;
    }
  }
  if (impl->reverseTrigger.process(params[REVERSE_PARAM].getValue())) {
    bool next = !impl->reverseLatched;
    impl->reverseLatched = next;
    if (next) {
      impl->freezeLatched = false;
      impl->freezeLatchedByButton = false;
      impl->slipLatched = false;
      if (desiredSampleModeEnabled && impl->engine.sampleLoaded) {
        // In sample mode, REV should have immediate transport effect.
        impl->engine.sampleTransportPlaying = true;
        impl->uiSampleTransportPlaying.store(true);
      }
    }
  }
  if (impl->slipTrigger.process(params[SLIP_PARAM].getValue())) {
    if (!impl->slipLatched) {
      impl->slipLatched = true;
      impl->slipReturnMode = SLIP_RETURN_SLOW;
      impl->freezeLatched = false;
      impl->freezeLatchedByButton = false;
      impl->reverseLatched = false;
    } else if (impl->slipReturnMode == SLIP_RETURN_SLOW) {
      impl->slipReturnMode = SLIP_RETURN_NORMAL;
      impl->freezeLatched = false;
      impl->freezeLatchedByButton = false;
      impl->reverseLatched = false;
    } else if (impl->slipReturnMode == SLIP_RETURN_NORMAL) {
      impl->slipReturnMode = SLIP_RETURN_INSTANT;
      impl->freezeLatched = false;
      impl->freezeLatchedByButton = false;
      impl->reverseLatched = false;
    } else {
      impl->slipLatched = false;
    }
  }
  if (impl->cartridgeCycleTrigger.process(params[CARTRIDGE_CYCLE_PARAM].getValue())) {
    impl->cartridgeCharacter = nextCartridgeCharacter(impl->cartridgeCharacter);
  }

  ProcessSignalInputs signalIn = readProcessSignalInputs(*this);
  float inL = signalIn.inL;
  float inR = signalIn.inR;
  float positionCv = signalIn.positionCv;
  float rateCv = signalIn.rateCv;
  bool rateCvConnected = signalIn.rateCvConnected;
  bool freezeGateHigh = signalIn.freezeGateHigh;
  bool scratchGateHigh = signalIn.scratchGateHigh;
  bool scratchGateConnected = signalIn.scratchGateConnected;
  bool positionConnected = signalIn.positionConnected;
  bool freezeGateFallingEdge = impl->prevFreezeGateHigh && !freezeGateHigh;
  impl->prevFreezeGateHigh = freezeGateHigh;
  if (freezeGateFallingEdge && impl->freezeLatched && impl->freezeLatchedByButton) {
    impl->freezeLatched = false;
    impl->freezeLatchedByButton = false;
  }

  impl->engine.scratchInterpolationMode = impl->scratchInterpolationMode;
  impl->engine.slipReturnMode = impl->slipReturnMode;
  impl->engine.externalGatePosMode = impl->externalGatePosMode;
  impl->engine.cartridgeCharacter = impl->cartridgeCharacter;
  impl->engine.sampleRate = args.sampleRate;
  impl->engine.sampleModeEnabled = desiredSampleModeEnabled;
  impl->engine.sampleLoopEnabled = impl->sampleLoopEnabled.load(std::memory_order_relaxed);
  uint32_t pendingSeekRevision = impl->pendingSampleSeekRevision.load(std::memory_order_relaxed);
  if (pendingSeekRevision != impl->appliedSampleSeekRevision) {
    float seekNorm = clamp(impl->pendingSampleSeekNormalized.load(std::memory_order_relaxed), 0.f, 1.f);
    if (impl->engine.sampleLoaded && impl->engine.sampleFrames > 0) {
      double sampleEndPos = std::max(0.0, double(impl->engine.sampleFrames - 1));
      double sampleWindowEndPos = sampleEndPos * double(clamp(params[BUFFER_PARAM].getValue(), 0.f, 1.f));
      // Arc seek maps to absolute sample time over the full arc; window limit
      // then clamps that target if it lies past the current playback cap.
      double targetFrame = clampd(double(seekNorm) * sampleEndPos, 0.0, sampleWindowEndPos);
      impl->engine.samplePlayhead = targetFrame;
      impl->engine.readHead = targetFrame;
      impl->engine.scratchLagSamples = 0.0;
      impl->engine.scratchLagTargetSamples = 0.0;
      impl->engine.nowCatchActive = false;
      impl->engine.cancelSlipReturnState();
    }
    impl->appliedSampleSeekRevision = pendingSeekRevision;
  }
  PlatterInputSnapshot platterInput = impl->platterInput.consumeForFrame();

  TemporalDeckEngine::FrameInput frameInput;
  frameInput.dt = args.sampleTime;
  frameInput.inL = inL;
  frameInput.inR = inR;
  frameInput.bufferKnob = params[BUFFER_PARAM].getValue();
  frameInput.rateKnob = params[RATE_PARAM].getValue();
  frameInput.mixKnob = params[MIX_PARAM].getValue();
  frameInput.feedbackKnob = params[FEEDBACK_PARAM].getValue();
  frameInput.freezeButton = impl->freezeLatched;
  frameInput.reverseButton = impl->reverseLatched;
  frameInput.slipButton = impl->slipLatched;
  frameInput.quickSlipTrigger = platterInput.quickSlipTrigger;
  frameInput.freezeGate = freezeGateHigh;
  frameInput.scratchGate = scratchGateHigh;
  frameInput.scratchGateConnected = scratchGateConnected;
  frameInput.positionConnected = positionConnected;
  frameInput.positionCv = positionCv;
  frameInput.rateCv = rateCv;
  frameInput.rateCvConnected = rateCvConnected;
  frameInput.platterTouched = platterInput.platterTouched;
  frameInput.wheelScratchHeld = platterInput.wheelScratchHeld;
  frameInput.platterMotionActive = platterInput.platterMotionActive;
  frameInput.platterGestureRevision = platterInput.platterGestureRevision;
  frameInput.platterLagTarget = platterInput.platterLagTarget;
  frameInput.platterGestureVelocity = platterInput.platterGestureVelocity;
  frameInput.wheelDelta = platterInput.wheelDelta;

  auto frame = impl->engine.process(frameInput);

  if (frame.autoFreezeRequested && !impl->freezeLatched && !freezeGateHigh) {
    impl->freezeLatched = true;
    impl->freezeLatchedByButton = false;
    impl->reverseLatched = false;
    impl->slipLatched = false;
  }

  writeFrameOutputs(*this, frame);
  bool freezeActive = impl->freezeLatched || freezeGateHigh;
  updateTransportModeLights(*this, freezeActive, impl->reverseLatched, impl->slipLatched,
                            impl->slipReturnMode);
  impl->uiPlatterAngle.store(frame.platterAngle, std::memory_order_relaxed);
  impl->uiLagSamples.store(frame.lag, std::memory_order_relaxed);
  impl->uiAccessibleLagSamples.store(frame.accessibleLag, std::memory_order_relaxed);
  impl->uiSampleRate.store(args.sampleRate, std::memory_order_relaxed);
  impl->uiFreezeLatched.store(freezeActive, std::memory_order_relaxed);
  impl->uiSampleModeEnabled.store(frame.sampleMode, std::memory_order_relaxed);
  impl->uiSampleLoaded.store(frame.sampleLoaded, std::memory_order_relaxed);
  impl->uiSampleTransportPlaying.store(frame.sampleTransportPlaying, std::memory_order_relaxed);
  impl->uiSamplePlayheadSeconds.store(frame.samplePlayhead, std::memory_order_relaxed);
  impl->uiSampleDurationSeconds.store(frame.sampleDuration, std::memory_order_relaxed);
  impl->uiSampleProgress.store(frame.sampleProgress, std::memory_order_relaxed);

  impl->sampleModeEnabled.store(impl->engine.sampleModeEnabled, std::memory_order_relaxed);
  impl->uiPublishTimerSec += args.sampleTime;
  if (impl->uiPublishTimerSec >= kUiPublishIntervalSec) {
    impl->uiPublishTimerSec = std::fmod(impl->uiPublishTimerSec, kUiPublishIntervalSec);
    int sampleFrames = impl->engine.sampleFrames;
    int bufferMode = impl->bufferDurationMode.load(std::memory_order_relaxed);
    float maxLagSamples = std::max(1.f, args.sampleRate * usableBufferSecondsForMode(bufferMode));
    temporaldeck_ui::publishArcLights(this, sampleFrames, maxLagSamples, frame.sampleMode, frame.sampleLoaded,
                                      frame.lag, frame.accessibleLag, frame.sampleProgress);
  }
}

void TemporalDeck::setPlatterScratch(bool touched, float lagSamples, float velocitySamples, int holdSamples) {
  impl->platterInput.setScratch(touched, lagSamples, velocitySamples, holdSamples);
}

void TemporalDeck::setPlatterMotionFreshSamples(int motionFreshSamples) {
  impl->platterInput.setMotionFreshSamples(motionFreshSamples);
}

void TemporalDeck::addPlatterWheelDelta(float delta, int holdSamples) {
  impl->platterInput.addWheelDelta(delta, holdSamples);
}

void TemporalDeck::triggerQuickSlipReturn() {
  impl->platterInput.triggerQuickSlipReturn();
}

double TemporalDeck::getUiLagSamples() const {
  return impl->uiLagSamples.load(std::memory_order_relaxed);
}

double TemporalDeck::getUiAccessibleLagSamples() const {
  return impl->uiAccessibleLagSamples.load(std::memory_order_relaxed);
}

float TemporalDeck::getUiSampleRate() const {
  return impl->uiSampleRate.load(std::memory_order_relaxed);
}

float TemporalDeck::getUiPlatterAngle() const {
  return impl->uiPlatterAngle.load(std::memory_order_relaxed);
}

bool TemporalDeck::isUiFreezeLatched() const {
  return impl->uiFreezeLatched.load(std::memory_order_relaxed);
}


bool TemporalDeck::isSampleModeEnabled() const {
  return impl->uiSampleModeEnabled.load(std::memory_order_relaxed);
}

bool TemporalDeck::hasLoadedSample() const {
  return impl->uiSampleLoaded.load(std::memory_order_relaxed);
}

bool TemporalDeck::isSampleAutoPlayOnLoadEnabled() const {
  std::lock_guard<std::mutex> lock(impl->sampleStateMutex);
  return impl->sampleAutoPlayOnLoad;
}

void TemporalDeck::setSampleAutoPlayOnLoadEnabled(bool enabled) {
  {
    std::lock_guard<std::mutex> lock(impl->sampleStateMutex);
    impl->sampleAutoPlayOnLoad = enabled;
  }
  impl->pendingSampleStateApply.store(true);
}

void TemporalDeck::setSampleModeEnabled(bool enabled) {
  impl->sampleModeEnabled.store(enabled, std::memory_order_relaxed);
  impl->pendingSampleStateApply.store(true);
  impl->uiSampleModeEnabled.store(enabled && impl->engine.sampleLoaded, std::memory_order_relaxed);
}

bool TemporalDeck::isSampleTransportPlaying() const {
  return impl->uiSampleTransportPlaying.load(std::memory_order_relaxed);
}

bool TemporalDeck::isSampleLoopEnabled() const {
  return impl->sampleLoopEnabled.load(std::memory_order_relaxed);
}

void TemporalDeck::setSampleLoopEnabled(bool enabled) {
  impl->sampleLoopEnabled.store(enabled, std::memory_order_relaxed);
  impl->engine.sampleLoopEnabled = enabled;
}

void TemporalDeck::setSampleTransportPlaying(bool enabled) {
  impl->engine.sampleTransportPlaying = enabled && impl->engine.sampleLoaded;
  impl->uiSampleTransportPlaying.store(impl->engine.sampleTransportPlaying, std::memory_order_relaxed);
}

void TemporalDeck::stopSampleTransport() {
  impl->engine.sampleTransportPlaying = false;
  if (impl->engine.sampleLoaded) {
    impl->engine.samplePlayhead = 0.0;
    impl->engine.readHead = 0.0;
  }
  impl->uiSampleTransportPlaying.store(false, std::memory_order_relaxed);
  impl->uiSamplePlayheadSeconds.store(0.0, std::memory_order_relaxed);
  impl->uiSampleProgress.store(0.0, std::memory_order_relaxed);
}

void TemporalDeck::clearLoadedSample() {
  {
    std::lock_guard<std::mutex> lock(impl->sampleStateMutex);
    impl->samplePath.clear();
    impl->sampleDisplayName.clear();
    impl->decodedSample = DecodedSampleFile();
  }
  impl->decodedSampleAvailable.store(false, std::memory_order_relaxed);
  impl->pendingPreparedSampleInstall.store(false, std::memory_order_relaxed);
  {
    std::lock_guard<std::mutex> lock(impl->preparedSampleMutex);
    impl->preparedSample = PreparedSampleData();
  }
  Impl::AsyncSampleBuildRequest cancelRequest;
  cancelRequest.type = Impl::AsyncSampleBuildRequest::NONE;
  cancelRequest.targetSampleRate = std::max(impl->cachedSampleRate, 1.f);
  cancelRequest.requestedBufferMode = impl->bufferDurationMode.load(std::memory_order_relaxed);
  impl->requestAsyncSampleBuild(cancelRequest);
  impl->sampleModeEnabled.store(false, std::memory_order_relaxed);
  impl->bufferDurationMode.store(BUFFER_DURATION_10S);
  if (paramQuantities[BUFFER_PARAM]) {
    paramQuantities[BUFFER_PARAM]->displayMultiplier = usableBufferSecondsForMode(BUFFER_DURATION_10S);
  }
  impl->pendingSampleStateApply.store(true);
}

bool TemporalDeck::loadSampleFromPath(const std::string &path, std::string *errorOut) {
  (void)errorOut;
  bool autoPlayOnLoad = true;
  bool wasFreezeLatched = impl->freezeLatched;
  bool wasFreezeLatchedByButton = impl->freezeLatchedByButton;
  {
    std::lock_guard<std::mutex> lock(impl->sampleStateMutex);
    autoPlayOnLoad = impl->sampleAutoPlayOnLoad;
  }
  impl->freezeLatched = wasFreezeLatched || !autoPlayOnLoad;
  impl->freezeLatchedByButton = impl->freezeLatched ? wasFreezeLatchedByButton : false;
  impl->reverseLatched = false;
  impl->slipLatched = false;

  Impl::AsyncSampleBuildRequest request;
  request.type = Impl::AsyncSampleBuildRequest::LOAD_PATH;
  request.path = path;
  request.targetSampleRate = std::max(impl->cachedSampleRate, 1.f);
  request.requestedBufferMode = impl->bufferDurationMode.load(std::memory_order_relaxed);
  impl->requestAsyncSampleBuild(request);
  return true;
}

void TemporalDeck::seekSampleByNormalizedPosition(double normalized) {
  impl->pendingSampleSeekNormalized.store(clamp(float(normalized), 0.f, 1.f), std::memory_order_relaxed);
  impl->pendingSampleSeekRevision.fetch_add(1, std::memory_order_relaxed);
}

double TemporalDeck::getUiSamplePlayheadSeconds() const {
  return impl->uiSamplePlayheadSeconds.load();
}

double TemporalDeck::getUiSampleDurationSeconds() const {
  return impl->uiSampleDurationSeconds.load();
}

double TemporalDeck::getUiSampleProgress() const {
  return impl->uiSampleProgress.load();
}

std::string TemporalDeck::getLoadedSampleDisplayName() const {
  std::lock_guard<std::mutex> lock(impl->sampleStateMutex);
  return impl->sampleDisplayName;
}

bool TemporalDeck::wasLoadedSampleTruncated() const {
  return impl->engine.sampleTruncated;
}

bool TemporalDeck::isSlipLatched() const {
  return impl->slipLatched;
}

int TemporalDeck::getCartridgeCharacter() const {
  return impl->cartridgeCharacter;
}

int TemporalDeck::getBufferDurationMode() const {
  return clamp(impl->bufferDurationMode.load(), 0, BUFFER_DURATION_COUNT - 1);
}

bool TemporalDeck::isBufferModeMono() const {
  return isMonoBufferMode(clamp(impl->bufferDurationMode.load(), 0, BUFFER_DURATION_COUNT - 1));
}

bool TemporalDeck::consumePendingInitialPlatterArtSelection() {
  if (!impl->pendingInitialPlatterArtSelection) {
    return false;
  }
  impl->pendingInitialPlatterArtSelection = false;
  return true;
}

int TemporalDeck::getPlatterArtMode() const {
  return clamp(impl->platterArtMode, PLATTER_ART_BUILTIN_SVG, PLATTER_ART_MODE_COUNT - 1);
}

void TemporalDeck::setPlatterArtMode(int mode) {
  int clamped = clamp(mode, PLATTER_ART_BUILTIN_SVG, PLATTER_ART_MODE_COUNT - 1);
  impl->platterArtMode = clamped;
}

int TemporalDeck::getPlatterBrightnessMode() const {
  return clamp(impl->platterBrightnessMode, PLATTER_BRIGHTNESS_FULL, PLATTER_BRIGHTNESS_COUNT - 1);
}

void TemporalDeck::setPlatterBrightnessMode(int mode) {
  impl->platterBrightnessMode = clamp(mode, PLATTER_BRIGHTNESS_FULL, PLATTER_BRIGHTNESS_COUNT - 1);
}

std::string TemporalDeck::getCustomPlatterArtPath() const {
  return impl->customPlatterArtPath;
}

bool TemporalDeck::setCustomPlatterArtPath(const std::string &path) {
  if (path.empty()) {
    return false;
  }
  impl->customPlatterArtPath = path;
  impl->platterArtMode = PLATTER_ART_CUSTOM;
  return true;
}

void TemporalDeck::clearCustomPlatterArtPath() {
  impl->customPlatterArtPath.clear();
  if (impl->platterArtMode == PLATTER_ART_CUSTOM) {
    impl->platterArtMode = PLATTER_ART_BUILTIN_SVG;
  }
}

bool TemporalDeck::isPlatterTraceLoggingEnabled() const {
  return impl->platterTraceLoggingEnabled;
}

void TemporalDeck::setPlatterTraceLoggingEnabled(bool enabled) {
  impl->platterTraceLoggingEnabled = enabled;
}

bool TemporalDeck::isHighQualityScratchInterpolationEnabled() const {
  return impl->scratchInterpolationMode != SCRATCH_INTERP_CUBIC;
}

void TemporalDeck::setHighQualityScratchInterpolationEnabled(bool enabled) {
  impl->scratchInterpolationMode = enabled ? SCRATCH_INTERP_LAGRANGE6 : SCRATCH_INTERP_CUBIC;
}

int TemporalDeck::getScratchInterpolationMode() const {
  return impl->scratchInterpolationMode;
}

void TemporalDeck::setScratchInterpolationMode(int mode) {
  impl->scratchInterpolationMode = clamp(mode, SCRATCH_INTERP_CUBIC, SCRATCH_INTERP_COUNT - 1);
}

void TemporalDeck::setSlipLatched(bool enabled) {
  impl->slipLatched = enabled;
  if (enabled) {
    impl->freezeLatched = false;
    impl->freezeLatchedByButton = false;
    impl->reverseLatched = false;
  }
}

int TemporalDeck::getSlipReturnMode() const {
  return impl->slipReturnMode;
}

void TemporalDeck::setSlipReturnMode(int mode) {
  impl->slipReturnMode = clamp(mode, SLIP_RETURN_SLOW, SLIP_RETURN_COUNT - 1);
}

int TemporalDeck::getExternalGatePosMode() const {
  return impl->externalGatePosMode;
}

void TemporalDeck::setExternalGatePosMode(int mode) {
  impl->externalGatePosMode = clamp(mode, EXTERNAL_GATE_POS_GLIDE, EXTERNAL_GATE_POS_COUNT - 1);
}
