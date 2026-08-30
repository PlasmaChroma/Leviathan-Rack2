#include "../src/plugin.hpp"

#include <cmath>
#include <atomic>
#include <iostream>
#include <string>
#include <thread>
#include <utility>
#include <vector>

// Provide the plugin global expected by module source.
Plugin* pluginInstance = nullptr;

// Bifurx includes the shared DragonKing debug setting from plugin.cpp in the
// plugin build. The runtime spec includes Bifurx.cpp directly, so keep this
// test self-contained instead of linking the full plugin translation unit.
bool isDragonKingDebugEnabled() {
	return false;
}

bool isModuleTeardownLoggingEnabled() {
	return false;
}

void refreshDragonKingDebugEnabled() {
}

ModuleTeardownTimer::ModuleTeardownTimer(const char* moduleName)
	: moduleName(moduleName) {
}

void ModuleTeardownTimer::begin(int moduleId) {
	this->moduleId = moduleId;
}

ModuleTeardownTimer::~ModuleTeardownTimer() {
}

#include "../src/Bifurx.cpp"

using namespace bifurx;

namespace {

constexpr float kRuntimePi = 3.14159265358979323846f;

struct TestResult {
  std::string name;
  bool pass = false;
  std::string detail;
};

TestResult testSpanShapeLutTracksReferenceCurve() {
  float maxError = 0.f;
  bool monotonic = true;
  float previous = shapedSpan(0.f);
  constexpr int steps = 20000;
  for (int i = 1; i <= steps; ++i) {
    const float x = float(i) / float(steps);
    const float actual = shapedSpan(x);
    const float reference = std::pow(x, 1.45f);
    maxError = std::max(maxError, std::fabs(actual - reference));
    monotonic = monotonic && actual >= previous;
    previous = actual;
  }
  const bool endpoints = shapedSpan(0.f) == 0.f && shapedSpan(1.f) == 1.f;
  return {
    "SPAN lookup table preserves the authored shaping curve",
    endpoints && monotonic && maxError < 7e-6f,
    "maxError=" + std::to_string(maxError)
  };
}

float freqNormForCenterHz(float centerHz) {
  constexpr float kFreqMinHz = 4.f;
  constexpr float kFreqLog2Span = 12.7731392f;  // log2(28000 / 4)
  const float safeCenter = std::max(centerHz, kFreqMinHz);
  return clamp(std::log2(safeCenter / kFreqMinHz) / kFreqLog2Span, 0.f, 1.f);
}

TestResult testResonanceCurveUsesFullControlTravel() {
  struct ResonancePoint {
    float knob;
    float expectedQ;
  };
  constexpr ResonancePoint points[] = {
    {0.f, 0.5f},
    {0.25f, 0.9f},
    {0.5f, 1.75f},
    {0.75f, 5.f},
    {1.f, 33.3333f},
  };

  bool targetsPass = true;
  std::string measured;
  for (const ResonancePoint& point : points) {
    const float q = 1.f / resoToDamping(point.knob);
    const float relativeError = std::fabs(q - point.expectedQ) / point.expectedQ;
    targetsPass = targetsPass && relativeError < 0.025f;
    measured += " [r=" + std::to_string(point.knob) + " Q=" + std::to_string(q) + "]";
  }

  bool monotonic = true;
  float previousQ = 1.f / resoToDamping(0.f);
  for (int i = 1; i <= 100; ++i) {
    const float q = 1.f / resoToDamping(float(i) / 100.f);
    monotonic = monotonic && q > previousQ;
    previousQ = q;
  }

  const bool belgradOnsetPreserved = std::fabs(kSelfOscResoStart - 0.80f) < 1e-6f;
  return {
    "Resonance curve uses full travel while preserving 0.80 self-osc onset",
    targetsPass && monotonic && belgradOnsetPreserved,
    "targets=" + std::to_string(int(targetsPass)) +
      " monotonic=" + std::to_string(int(monotonic)) +
      " onset=" + std::to_string(kSelfOscResoStart) + measured
  };
}

TestResult testFrequencyQuantityRoundTripsAccurately() {
  constexpr float frequencies[] = {4.f, 10.f, 440.f, 4000.f, 28000.f};
  float worstRelativeError = 0.f;
  bool pass = true;
  for (float hz : frequencies) {
    const float param = bifurxParamFromFrequencyHz(hz);
    const float roundTripHz = bifurxFrequencyHzFromParam(param);
    const float relativeError = std::fabs(roundTripHz - hz) / std::max(hz, 1.f);
    worstRelativeError = std::max(worstRelativeError, relativeError);
    pass = pass && relativeError < 2e-6f;
  }
  pass = pass
    && bifurxParamFromFrequencyHz(4.f) == 0.f
    && bifurxParamFromFrequencyHz(28000.f) == 1.f;
  return {
    "Frequency text-entry mapping round-trips accurately at endpoints and interior values",
    pass,
    "worstRelativeError=" + std::to_string(worstRelativeError)
  };
}

TestResult testPreviewInvalidatesForModelDependencies() {
  BifurxPreviewState base;
  base.freqA = 400.f;
  base.freqB = 1600.f;
  base.qA = 2.f;
  base.qB = 3.f;
  base.spanNorm = 0.25f;
  base.resoNorm = 0.4f;

  BifurxPreviewState spanChanged = base;
  spanChanged.spanNorm = 0.75f;
  BifurxPreviewState resoChanged = base;
  resoChanged.resoNorm = 0.6f;
  const bool pass = previewStatesDiffer(base, spanChanged)
    && previewStatesDiffer(base, resoChanged)
    && !previewStatesDiffer(base, base);
  return {
    "Preview invalidation includes SPAN and resonance model dependencies",
    pass,
    "spanChanged=" + std::to_string(int(previewStatesDiffer(base, spanChanged)))
      + " resoChanged=" + std::to_string(int(previewStatesDiffer(base, resoChanged)))
  };
}

void configureBaseParams(Bifurx& module, int mode, float freqNorm, float spanNorm, float reso, float balance) {
  module.params[Bifurx::MODE_PARAM].setValue(float(mode));
  module.params[Bifurx::LEVEL_PARAM].setValue(0.5f);
  module.params[Bifurx::FREQ_PARAM].setValue(freqNorm);
  module.params[Bifurx::SPAN_PARAM].setValue(spanNorm);
  module.params[Bifurx::RESO_PARAM].setValue(reso);
  module.params[Bifurx::BALANCE_PARAM].setValue(balance);
  module.params[Bifurx::FM_AMT_PARAM].setValue(0.f);
  module.params[Bifurx::SPAN_CV_ATTEN_PARAM].setValue(0.f);
  module.params[Bifurx::TITO_PARAM].setValue(0.f);   // Neutral
}

void clearCvInputs(Bifurx& module) {
  module.inputs[Bifurx::VOCT_INPUT].setChannels(0);
  module.inputs[Bifurx::FM_INPUT].setChannels(0);
  module.inputs[Bifurx::RESO_CV_INPUT].setChannels(0);
  module.inputs[Bifurx::BALANCE_CV_INPUT].setChannels(0);
  module.inputs[Bifurx::SPAN_CV_INPUT].setChannels(0);
}

float measureRuntimeGainDb(
  int mode,
  float inputHz,
  float inputAmplitude,
  float freqNorm,
  float spanNorm,
  float reso,
  float balance,
  float tito = 0.f
) {
  Bifurx module;
  module.onReset();

  configureBaseParams(module, mode, freqNorm, spanNorm, reso, balance);
  module.params[Bifurx::TITO_PARAM].setValue(tito);
  clearCvInputs(module);

  Module::ProcessArgs args;
  args.sampleRate = 48000.f;
  args.sampleTime = 1.f / args.sampleRate;

  const int settleSamples = int(0.55f * args.sampleRate);
  const int measureSamples = int(0.65f * args.sampleRate);
  const int totalSamples = settleSamples + measureSamples;

  float inSq = 0.f;
  float outSq = 0.f;
  int nAccum = 0;
  for (int n = 0; n < totalSamples; ++n) {
    const float t = float(n) / args.sampleRate;
    const float in = inputAmplitude * std::sin(2.f * kRuntimePi * inputHz * t);
    module.inputs[Bifurx::IN_INPUT].setVoltage(in);
    module.process(args);
    const float out = module.outputs[Bifurx::OUT_OUTPUT].getVoltage();
    if (n >= settleSamples) {
      inSq += in * in;
      outSq += out * out;
      nAccum++;
    }
  }

  const float inRms = std::sqrt(std::max(inSq / std::max(1, nAccum), 1e-12f));
  const float outRms = std::sqrt(std::max(outSq / std::max(1, nAccum), 1e-12f));
  return 20.f * std::log10(std::max(outRms / inRms, 1e-6f));
}

struct TitoOutputCapture {
  bool finite = true;
  float rms = 0.f;
  std::vector<float> samples;
};

struct ZeroInputCapture {
  bool finite = true;
  float rms = 0.f;
  float peakAbs = 0.f;
};

float titoStimulusSample(float t, int n) {
  const float toneA = 1.45f * std::sin(2.f * kRuntimePi * 137.f * t);
  const float toneB = 0.72f * std::sin(2.f * kRuntimePi * 421.f * t + 0.37f);
  const float toneC = 0.48f * std::sin(2.f * kRuntimePi * 1139.f * t + 1.19f);
  const float tick = ((n % 997) < 7) ? 0.42f : 0.f;
  return toneA + toneB + toneC + tick;
}

TitoOutputCapture captureTitoOutput(
  int mode,
  float tito,
  float centerHz,
  float spanNorm,
  float reso,
  float balance
) {
  Bifurx module;
  module.onReset();

  configureBaseParams(module, mode, freqNormForCenterHz(centerHz), spanNorm, reso, balance);
  module.params[Bifurx::LEVEL_PARAM].setValue(0.82f);
  module.params[Bifurx::TITO_PARAM].setValue(tito);
  clearCvInputs(module);

  Module::ProcessArgs args;
  args.sampleRate = 48000.f;
  args.sampleTime = 1.f / args.sampleRate;

  const int settleSamples = 4096;
  const int measureSamples = 4096;
  TitoOutputCapture capture;
  capture.samples.reserve(measureSamples);

  float outSq = 0.f;
  for (int n = 0; n < settleSamples + measureSamples; ++n) {
    const float t = float(n) / args.sampleRate;
    module.inputs[Bifurx::IN_INPUT].setVoltage(titoStimulusSample(t, n));
    module.process(args);
    const float out = module.outputs[Bifurx::OUT_OUTPUT].getVoltage();
    capture.finite = capture.finite && std::isfinite(out);
    if (n >= settleSamples) {
      capture.samples.push_back(out);
      outSq += out * out;
    }
  }

  capture.rms = std::sqrt(std::max(outSq / float(std::max(1, measureSamples)), 0.f));
  capture.finite = capture.finite && std::isfinite(capture.rms);
  return capture;
}

float normalizedWaveDistance(const TitoOutputCapture& a, const TitoOutputCapture& b) {
  const std::size_t n = std::min(a.samples.size(), b.samples.size());
  if (n == 0) {
    return 0.f;
  }

  float diffSq = 0.f;
  float refSq = 0.f;
  for (std::size_t i = 0; i < n; ++i) {
    const float d = a.samples[i] - b.samples[i];
    diffSq += d * d;
    refSq += a.samples[i] * a.samples[i];
  }

  return std::sqrt(diffSq / float(n)) / std::max(std::sqrt(refSq / float(n)), 1e-6f);
}

ZeroInputCapture captureZeroInputOutput(
  int mode,
  float centerHz,
  float spanNorm,
  float reso,
  float balance,
  float level,
  int settleSamples,
  int measureSamples
) {
  Bifurx module;
  module.onReset();

  configureBaseParams(module, mode, freqNormForCenterHz(centerHz), spanNorm, reso, balance);
  module.params[Bifurx::LEVEL_PARAM].setValue(level);
  module.params[Bifurx::TITO_PARAM].setValue(0.f);
  module.highResonanceSelfOscEnabled = true;
  clearCvInputs(module);

  Module::ProcessArgs args;
  args.sampleRate = 48000.f;
  args.sampleTime = 1.f / args.sampleRate;

  ZeroInputCapture capture;
  float outSq = 0.f;
  int nAccum = 0;
  for (int n = 0; n < settleSamples + measureSamples; ++n) {
    module.inputs[Bifurx::IN_INPUT].setVoltage(0.f);
    module.process(args);
    const float out = module.outputs[Bifurx::OUT_OUTPUT].getVoltage();
    capture.finite = capture.finite && std::isfinite(out);
    if (n >= settleSamples) {
      outSq += out * out;
      capture.peakAbs = std::max(capture.peakAbs, std::fabs(out));
      nAccum++;
    }
  }
  capture.rms = std::sqrt(std::max(outSq / float(std::max(1, nAccum)), 0.f));
  capture.finite = capture.finite && std::isfinite(capture.rms) && std::isfinite(capture.peakAbs);
  return capture;
}

bool capturePreviewStateForSpan(float spanNorm, BifurxPreviewState* outState) {
  if (!outState) {
    return false;
  }
  Bifurx module;
  module.onReset();
  module.resetCircuitStates();

  configureBaseParams(module, 0, freqNormForCenterHz(900.f), spanNorm, 0.35f, 0.f);
  clearCvInputs(module);
  module.inputs[Bifurx::IN_INPUT].setVoltage(0.f);

  Module::ProcessArgs args;
  args.sampleRate = 48000.f;
  args.sampleTime = 1.f / args.sampleRate;

  const uint32_t seqBefore = module.previewPublishSeq.load(std::memory_order_acquire);
  const int runSamples = int(0.45f * args.sampleRate);
  for (int n = 0; n < runSamples; ++n) {
    module.process(args);
  }
  const uint32_t seqAfter = module.previewPublishSeq.load(std::memory_order_acquire);
  if (seqAfter == seqBefore) {
    return false;
  }

  double publishTimeSec = 0.0;
  uint32_t copiedSeq = seqBefore;
  return module.readPreviewState(seqBefore, outState, &publishTimeSec, &copiedSeq);
}

bool capturePreviewState(
  int mode,
  float centerHz,
  float spanNorm,
  float reso,
  float balance,
  BifurxPreviewState* outState
) {
  if (!outState) {
    return false;
  }
  Bifurx module;
  module.onReset();

  configureBaseParams(module, mode, freqNormForCenterHz(centerHz), spanNorm, reso, balance);
  clearCvInputs(module);
  module.inputs[Bifurx::IN_INPUT].setVoltage(0.f);

  Module::ProcessArgs args;
  args.sampleRate = 48000.f;
  args.sampleTime = 1.f / args.sampleRate;

  const uint32_t seqBefore = module.previewPublishSeq.load(std::memory_order_acquire);
  const int runSamples = int(0.55f * args.sampleRate);
  for (int n = 0; n < runSamples; ++n) {
    module.process(args);
  }
  const uint32_t seqAfter = module.previewPublishSeq.load(std::memory_order_acquire);
  if (seqAfter == seqBefore) {
    return false;
  }

  double publishTimeSec = 0.0;
  uint32_t copiedSeq = seqBefore;
  return module.readPreviewState(seqBefore, outState, &publishTimeSec, &copiedSeq);
}

TestResult testPublishedSnapshotsRemainCoherentUnderContention() {
  Bifurx module;
  std::atomic<bool> writerDone {false};
  std::atomic<bool> coherent {true};
  std::atomic<int> reads {0};

  std::thread writer([&]() {
    for (int i = 1; i <= 50000; ++i) {
      const float base = float(i);
      BifurxPreviewState state;
      state.sampleRate = 48000.f;
      state.freqA = base;
      state.freqB = base + 100000.f;
      state.qA = base + 200000.f;
      state.qB = base + 300000.f;
      state.balance = base + 400000.f;
      state.spanNorm = base + 500000.f;
      module.publishPreviewState(state);
    }
    writerDone.store(true, std::memory_order_release);
  });

  std::thread reader([&]() {
    uint32_t lastSeq = 0;
    while (!writerDone.load(std::memory_order_acquire)
        || lastSeq != module.previewPublishSeq.load(std::memory_order_acquire)) {
      BifurxPreviewState state;
      double publishTimeSec = 0.0;
      uint32_t seq = lastSeq;
      if (!module.readPreviewState(lastSeq, &state, &publishTimeSec, &seq)) {
        std::this_thread::yield();
        continue;
      }
      const float base = state.freqA;
      const bool snapshotCoherent =
        state.freqB == base + 100000.f &&
        state.qA == base + 200000.f &&
        state.qB == base + 300000.f &&
        state.balance == base + 400000.f &&
        state.spanNorm == base + 500000.f &&
        std::isfinite(publishTimeSec) &&
        seq > lastSeq;
      coherent.store(coherent.load(std::memory_order_relaxed) && snapshotCoherent, std::memory_order_relaxed);
      lastSeq = seq;
      reads.fetch_add(1, std::memory_order_relaxed);
    }
  });

  writer.join();
  reader.join();
  const bool pass = coherent.load(std::memory_order_relaxed) && reads.load(std::memory_order_relaxed) > 0;
  return {
    "Published preview snapshots remain coherent under contention",
    pass,
    "reads=" + std::to_string(reads.load(std::memory_order_relaxed))
  };
}

TestResult testAnalysisFramesPublishAsOverlappingWindows() {
  Bifurx module;
  module.subscribeAnalysisVisual();
  alignas(16) float raw[kFftSize] = {};
  alignas(16) float output[kFftSize] = {};
  alignas(16) float response[kFftSize] = {};
  uint32_t seq = 0;

  for (int i = 0; i < kFftSize - 1; ++i) {
    module.pushAnalysisSample(float(i), float(i + 10), float(i + 20));
  }
  const bool earlyPublish = module.copyAnalysisFrame(0, raw, output, response, &seq);
  module.pushAnalysisSample(float(kFftSize - 1), float(kFftSize + 9), float(kFftSize + 19));
  const bool firstPublished = module.copyAnalysisFrame(0, raw, output, response, &seq);
  const uint32_t firstSeq = seq;
  const bool firstWindow = firstPublished && raw[0] == 0.f && raw[kFftSize - 1] == float(kFftSize - 1)
    && output[0] == 10.f && response[kFftSize - 1] == float(kFftSize + 19);

  for (int i = kFftSize; i < kFftSize + kFftHopSize; ++i) {
    module.pushAnalysisSample(float(i), float(i + 10), float(i + 20));
  }
  const bool secondPublished = module.copyAnalysisFrame(firstSeq, raw, output, response, &seq);
  const bool overlappingWindow = secondPublished
    && raw[0] == float(kFftHopSize)
    && raw[kFftSize - 1] == float(kFftSize + kFftHopSize - 1);
  module.unsubscribeAnalysisVisual();
  const bool pass = !earlyPublish && firstWindow && overlappingWindow && seq == firstSeq + 1u;
  return {
    "Analysis publishes coherent 50-percent-overlapped frames without rotation",
    pass,
    "firstSeq=" + std::to_string(firstSeq) + " secondSeq=" + std::to_string(seq)
  };
}

TestResult testAnalysisCaptureSleepsWithoutVisualSubscriber() {
  Bifurx module;
  for (int i = 0; i < kFftSize + kFftHopSize; ++i) {
    module.pushAnalysisSample(float(i), float(i), float(i));
  }
  const bool slept = module.analysisPublishSeq.load(std::memory_order_acquire) == 0u
    && module.analysisCaptureSlots[0] < 0
    && module.analysisCaptureSlots[1] < 0
    && module.analysisCaptureCountdown == 0;

  module.subscribeAnalysisVisual();
  for (int i = 0; i < 64; ++i) {
    module.pushAnalysisSample(float(i), float(i), float(i));
  }
  const bool woke = module.analysisCaptureSlots[0] >= 0
    && module.analysisCapturePositions[0] == 64;
  module.unsubscribeAnalysisVisual();
  module.pushAnalysisSample(0.f, 0.f, 0.f);
  const bool stoppedCleanly = module.analysisVisualSubscribers.load(std::memory_order_acquire) == 0u
    && module.analysisCaptureSlots[0] < 0
    && module.analysisCaptureSlots[1] < 0
    && module.analysisCaptureCountdown == 0;
  return {
    "Analysis capture sleeps without a live visual subscriber",
    slept && woke && stoppedCleanly,
    "slept=" + std::to_string(int(slept)) + " woke=" + std::to_string(int(woke))
      + " stopped=" + std::to_string(int(stoppedCleanly))
  };
}

TestResult testVisualWorkerDefaultSetterHonorsMode() {
  setBifurxVisualWorkerDefaultMode(VISUAL_WORKER_OFF);
  const bool off = getBifurxVisualWorkerDefaultMode() == VISUAL_WORKER_OFF;
  setBifurxVisualWorkerDefaultMode(VISUAL_WORKER_AUTO);
  const bool automatic = getBifurxVisualWorkerDefaultMode() == VISUAL_WORKER_AUTO;
  setBifurxVisualWorkerDefaultMode(VISUAL_WORKER_ON);
  const bool on = getBifurxVisualWorkerDefaultMode() == VISUAL_WORKER_ON;
  return {
    "Visual worker global default setter honors supported modes",
    off && automatic && on,
    "off=" + std::to_string(int(off)) + " auto=" + std::to_string(int(automatic))
      + " on=" + std::to_string(int(on))
  };
}

TestResult testResetClearsRuntimeState() {
  Bifurx module;
  configureBaseParams(module, 5, freqNormForCenterHz(900.f), 0.58f, 0.75f, 0.f);
  module.params[Bifurx::TITO_PARAM].setValue(0.7f);

  Module::ProcessArgs args;
  args.sampleRate = 48000.f;
  args.sampleTime = 1.f / args.sampleRate;
  for (int n = 0; n < 4096; ++n) {
    module.inputs[Bifurx::IN_INPUT].setVoltage(4.f * std::sin(2.f * kRuntimePi * 733.f * float(n) * args.sampleTime));
    module.process(args);
  }
  const bool wasExcited = std::fabs(module.coreA.ic1eq) + std::fabs(module.coreA.ic2eq)
    + std::fabs(module.coreB.ic1eq) + std::fabs(module.coreB.ic2eq) > 1e-5f;

  // Rack's ResetEvent base implementation dispatches the deprecated no-argument
  // hook after resetting parameters. Call that hook directly in this standalone
  // harness, which has no live Rack engine/history context.
  module.onReset();
  const bool coresCleared = module.coreA.ic1eq == 0.f && module.coreA.ic2eq == 0.f
    && module.coreB.ic1eq == 0.f && module.coreB.ic2eq == 0.f;
  const bool cachesCleared = !module.controlFastCacheValid
    && module.titoCoeffFreqA == 0.f && module.titoCoeffFreqB == 0.f
    && module.selfOscCoeffFreqA == 0.f && module.selfOscCoeffFreqB == 0.f
    && module.cachedFrequencyRangeSampleRate == 0.f
    && module.cachedPitchSampleRate == 0.f
    && !module.cachedCharacterStateValid
    && !module.previewFilterInitialized;
  const bool analysisCleared = module.analysisCaptureSlots[0] < 0
    && module.analysisCaptureSlots[1] < 0 && module.analysisCaptureCountdown == 0;

  module.inputs[Bifurx::IN_INPUT].setVoltage(0.f);
  module.process(args);
  const float firstOutput = module.outputs[Bifurx::OUT_OUTPUT].getVoltage();
  const bool silentRestart = std::isfinite(firstOutput) && std::fabs(firstOutput) < 1e-7f;
  return {
    "Reset clears Bifurx circuit, coefficient, smoothing, and capture state",
    wasExcited && coresCleared && cachesCleared && analysisCleared && silentRestart,
    "excited=" + std::to_string(int(wasExcited)) + " firstOutput=" + std::to_string(firstOutput)
  };
}

TestResult testVoctStepsImmediatelyWithoutImplicitGlide() {
  Bifurx module;
  configureBaseParams(module, 5, freqNormForCenterHz(440.f), 0.f, 0.35f, 0.f);
  clearCvInputs(module);
  // The standalone Rack harness has no Engine cable bookkeeping, so mark the
  // port connected directly before exercising its voltage.
  module.inputs[Bifurx::VOCT_INPUT].channels = 1;

  Module::ProcessArgs args;
  args.sampleRate = 48000.f;
  args.sampleTime = 1.f / args.sampleRate;
  module.inputs[Bifurx::VOCT_INPUT].setVoltage(0.f);
  module.process(args);
  const float baseHz = module.cachedFreqA0;

  module.inputs[Bifurx::VOCT_INPUT].setVoltage(1.f);
  module.process(args);
  const float steppedHz = module.cachedFreqA0;
  const float ratio = steppedHz / std::max(baseHz, 1e-6f);
  const bool pass = std::fabs(ratio - 2.f) < 2e-4f;
  return {
    "V/Oct follows a one-volt pitch step on the next sample without internal glide",
    pass,
    "baseHz=" + std::to_string(baseHz) + " steppedHz=" + std::to_string(steppedHz)
      + " ratio=" + std::to_string(ratio)
  };
}

TestResult testSpanIsPreservedAtFrequencyRails() {
  constexpr float spanParam = 0.55f;
  const float expectedRatio = std::exp2(8.f * shapedSpan(spanParam));
  float measuredRatios[2] = {};
  bool pass = true;

  for (int edge = 0; edge < 2; ++edge) {
    Bifurx module;
    configureBaseParams(module, 5, edge == 0 ? 0.f : 1.f, spanParam, 0.35f, 0.f);
    clearCvInputs(module);
    Module::ProcessArgs args;
    args.sampleRate = 48000.f;
    args.sampleTime = 1.f / args.sampleRate;
    module.process(args);
    measuredRatios[edge] = module.cachedFreqB0 / std::max(module.cachedFreqA0, 1e-6f);
    pass = pass && std::fabs(measuredRatios[edge] / expectedRatio - 1.f) < 5e-4f;
  }

  return {
    "SPAN separation is preserved by shifting the cutoff pair at both frequency rails",
    pass,
    "expectedRatio=" + std::to_string(expectedRatio)
      + " low=" + std::to_string(measuredRatios[0])
      + " high=" + std::to_string(measuredRatios[1])
  };
}

TestResult testRuntimeHighHighHasUnityCascadeGain() {
  constexpr float cascadeHpToHp = 0.731f;
  const float combined = combineModeResponse<float>(
    9,
    0.f, 0.f, 0.f, 0.f,
    0.f, 0.f, 0.f, 0.f,
    0.f, 0.f, 0.f, 0.f, 0.f, cascadeHpToHp,
    1.f, 1.f
  );
  return {
    "Production High + High applies unity gain to the cascaded high-pass output",
    combined == cascadeHpToHp,
    "cascade=" + std::to_string(cascadeHpToHp) + " combined=" + std::to_string(combined)
  };
}

std::vector<float> curveSignatureDb(const BifurxPreviewState& state, const std::vector<float>& hz) {
  std::vector<float> out;
  out.reserve(hz.size());
  const BifurxPreviewModel model = makePreviewModel(state);
  for (float f : hz) {
    out.push_back(previewModelResponseDb(model, f));
  }
  return out;
}

float l1Distance(const std::vector<float>& a, const std::vector<float>& b) {
  const std::size_t n = std::min(a.size(), b.size());
  float sum = 0.f;
  for (std::size_t i = 0; i < n; ++i) {
    sum += std::fabs(a[i] - b[i]);
  }
  return sum;
}

TestResult testRuntimeSpanMonotonicInPreviewState() {
  BifurxPreviewState a;
  BifurxPreviewState b;
  BifurxPreviewState c;
  const bool okA = capturePreviewStateForSpan(0.20f, &a);
  const bool okB = capturePreviewStateForSpan(0.55f, &b);
  const bool okC = capturePreviewStateForSpan(0.90f, &c);
  if (!okA || !okB || !okC) {
    return {"Runtime preview publishes span state", false, "preview publish did not tick for one or more spans"};
  }

  const float sepA = a.freqB / std::max(a.freqA, 1e-6f);
  const float sepB = b.freqB / std::max(b.freqA, 1e-6f);
  const float sepC = c.freqB / std::max(c.freqA, 1e-6f);
  const bool pass = (sepA < sepB) && (sepB < sepC);
  return {
    "Runtime preview A/B separation grows with SPAN",
    pass,
    "sep(0.20,0.55,0.90)=(" + std::to_string(sepA) + "," + std::to_string(sepB) + "," + std::to_string(sepC) + ")"
  };
}

TestResult testRuntimeBalanceTiltsBandBandInSvf() {
  const float centerHz = std::sqrt(340.f * 1500.f);
  const float freqNorm = freqNormForCenterHz(centerHz);
  const float spanNorm = clamp(std::log2(1500.f / 340.f) / 8.f, 0.f, 1.f);

  const float negLow = measureRuntimeGainDb(5, 340.f, 0.10f, freqNorm, spanNorm, 0.35f, -0.85f);
  const float negHigh = measureRuntimeGainDb(5, 1500.f, 0.10f, freqNorm, spanNorm, 0.35f, -0.85f);
  const float posLow = measureRuntimeGainDb(5, 340.f, 0.10f, freqNorm, spanNorm, 0.35f, 0.85f);
  const float posHigh = measureRuntimeGainDb(5, 1500.f, 0.10f, freqNorm, spanNorm, 0.35f, 0.85f);

  const bool lowFavoredWhenNegative = negLow > (negHigh + 1.f);
  const bool highFavoredWhenPositive = posHigh > (posLow + 1.f);
  return {
    "Runtime BALANCE tilts BB low/high emphasis (SVF)",
    lowFavoredWhenNegative && highFavoredWhenPositive,
    "neg(low,high)=(" + std::to_string(negLow) + "," + std::to_string(negHigh) + ") "
      "pos(low,high)=(" + std::to_string(posLow) + "," + std::to_string(posHigh) + ")"
  };
}

TestResult testRuntimeReportedLowCaseKeepsAudibleOutput() {
  const float centerHz = std::sqrt(53.9f * 114.f);
  const float freqNorm = freqNormForCenterHz(centerHz);
  const float spanNorm = clamp(std::log2(114.f / 53.9f) / 8.f, 0.f, 1.f);
  const float gain = measureRuntimeGainDb(0, 40.f, 0.25f, freqNorm, spanNorm, 0.35f, 0.f);
  const bool pass = std::isfinite(gain) && (gain > -36.f);
  return {
    "Runtime LL low-frequency case stays above floor",
    pass,
    "gainDb=" + std::to_string(gain)
  };
}

TestResult testRuntimeLlDropoutRegressionSweep() {
  const float centerHz = std::sqrt(53.9f * 114.f);
  const float freqNorm = freqNormForCenterHz(centerHz);
  const float spanNorm = clamp(std::log2(114.f / 53.9f) / 8.f, 0.f, 1.f);
  const float reso = 0.35f;
  const float balance = 0.f;
  const float amp = 0.18f;
  const float lowBandHz[] = {32.f, 40.f, 53.9f, 70.f, 90.f, 114.f};
  const float upperBandHz[] = {250.f, 500.f, 1200.f};

  float lowSum = 0.f;
  float lowMin = 1e9f;
  float lowMinHz = 0.f;
  for (float hz : lowBandHz) {
    const float g = measureRuntimeGainDb(0, hz, amp, freqNorm, spanNorm, reso, balance);
    if (!std::isfinite(g)) {
      return {"Runtime LL dropout sweep remains finite", false, "non-finite low-band gain"};
    }
    lowSum += g;
    if (g < lowMin) {
      lowMin = g;
      lowMinHz = hz;
    }
  }
  float upperSum = 0.f;
  for (float hz : upperBandHz) {
    const float g = measureRuntimeGainDb(0, hz, amp, freqNorm, spanNorm, reso, balance);
    if (!std::isfinite(g)) {
      return {"Runtime LL dropout sweep remains finite", false, "non-finite upper-band gain"};
    }
    upperSum += g;
  }

  const float lowAvg = lowSum / float(sizeof(lowBandHz) / sizeof(lowBandHz[0]));
  const float upperAvg = upperSum / float(sizeof(upperBandHz) / sizeof(upperBandHz[0]));
  const bool pass = (lowMin > -30.f) && (lowAvg > -20.f) && (lowAvg > (upperAvg - 10.f));
  return {
    "Runtime LL dropout sweep stays above floor",
    pass,
    "lowAvg=" + std::to_string(lowAvg) +
      " lowMin=" + std::to_string(lowMin) +
      " lowMinHz=" + std::to_string(lowMinHz) +
      " upperAvg=" + std::to_string(upperAvg)
  };
}

TestResult testRuntimeCurveFamiliesRemainDistinct() {
  const std::vector<int> modes = {0, 3, 5, 9};  // LL, NN, BB, HH
  const std::vector<float> hz = {40.f, 120.f, 320.f, 900.f, 2200.f, 6000.f};
  std::vector<std::vector<float>> signatures;
  signatures.reserve(modes.size());

  for (int mode : modes) {
    BifurxPreviewState state;
    const bool ok = capturePreviewState(mode, 900.f, 0.55f, 0.35f, 0.f, &state);
    if (!ok) {
      return {
        "Runtime LL/NN/BB/HH curve families stay distinct",
        false,
        "preview publish failed for mode=" + std::to_string(mode)
      };
    }
    signatures.push_back(curveSignatureDb(state, hz));
  }

  const float d03 = l1Distance(signatures[0], signatures[1]);
  const float d05 = l1Distance(signatures[0], signatures[2]);
  const float d09 = l1Distance(signatures[0], signatures[3]);
  const float d35 = l1Distance(signatures[1], signatures[2]);
  const float d39 = l1Distance(signatures[1], signatures[3]);
  const float d59 = l1Distance(signatures[2], signatures[3]);
  const bool pass = (d03 > 10.f) && (d05 > 10.f) && (d09 > 10.f)
    && (d35 > 10.f) && (d39 > 10.f) && (d59 > 10.f);

  return {
    "Runtime LL/NN/BB/HH curve families stay distinct",
    pass,
    "d03=" + std::to_string(d03) + " d05=" + std::to_string(d05) + " d09=" + std::to_string(d09) +
      " d35=" + std::to_string(d35) + " d39=" + std::to_string(d39) + " d59=" + std::to_string(d59)
  };
}

TestResult testRuntimePreviewPublishesHighQBeyondLegacyClamp() {
  BifurxPreviewState state;
  const bool ok = capturePreviewState(5, 900.f, 0.58f, 1.f, 0.f, &state);
  if (!ok) {
    return {"Runtime preview publishes high-Q state", false, "preview publish failed"};
  }

  const bool pass = state.qA > 25.f && state.qB > 25.f;
  return {
    "Runtime preview publishes high Q beyond legacy clamp",
    pass,
    "qA=" + std::to_string(state.qA) + " qB=" + std::to_string(state.qB)
  };
}

TestResult testRuntimePreviewMarkerGainTracksBandHeavyModes() {
  struct Scenario {
    int mode = 0;
    float centerHz = 900.f;
    float spanNorm = 0.5f;
    float reso = 0.35f;
    float balance = 0.f;
    std::vector<float> probesHz;
    const char* label = "";
  };

  const std::vector<Scenario> scenarios = {
    {1, 900.f, 0.52f, 0.70f, 0.f, {}, "LB"},
    {5, 900.f, 0.58f, 1.00f, 0.f, {}, "BB"},
    {8, 900.f, 0.52f, 0.70f, 0.f, {}, "BH"},
  };

  bool pass = true;
  float worstDeltaDb = 0.f;
  std::string detail;

  for (const Scenario& scenario : scenarios) {
    BifurxPreviewState state;
    const bool ok = capturePreviewState(
      scenario.mode, scenario.centerHz, scenario.spanNorm, scenario.reso, scenario.balance, &state
    );
    if (!ok) {
      return {
        "Runtime preview marker gain tracks band-heavy modes",
        false,
        std::string("preview publish failed for ") + scenario.label
      };
    }

    const BifurxPreviewModel model = makePreviewModel(state);
    std::vector<float> probesHz = scenario.probesHz;
    if (probesHz.empty()) {
      if (scenario.mode == 1) {
        probesHz.push_back(state.freqB);
      }
      else if (scenario.mode == 5) {
        probesHz.push_back(state.freqA);
        probesHz.push_back(state.freqB);
      }
      else {
        probesHz.push_back(state.freqA);
      }
    }

    const float freqNorm = freqNormForCenterHz(scenario.centerHz);
    for (float hz : probesHz) {
      const float previewDb = previewModelResponseDb(model, hz);
      const float runtimeDb = measureRuntimeGainDb(
        scenario.mode, hz, 0.05f, freqNorm, scenario.spanNorm, scenario.reso, scenario.balance
      );
      const float deltaDb = std::fabs(previewDb - runtimeDb);
      worstDeltaDb = std::max(worstDeltaDb, deltaDb);
      if (deltaDb > 6.f) {
        pass = false;
      }
      if (!detail.empty()) {
        detail += " ";
      }
      detail += std::string(scenario.label) +
        "@"+ std::to_string(hz) +
        "(p=" + std::to_string(previewDb) +
        ",r=" + std::to_string(runtimeDb) +
        ",d=" + std::to_string(deltaDb) + ")";
    }
  }

  return {
    "Runtime preview marker gain tracks band-heavy modes",
    pass,
    "worstDelta=" + std::to_string(worstDeltaDb) + " " + detail
  };
}

TestResult testRuntimeTitoProducesFiniteContrastAcrossModes() {
  bool pass = true;
  float worstSmDistance = 1e9f;
  float worstXmDistance = 1e9f;
  float bestSmDistance = 0.f;
  float bestXmDistance = 0.f;
  int worstSmMode = -1;
  int worstXmMode = -1;
  std::string weakCases;

  for (int mode = 0; mode < 10; ++mode) {
    const TitoOutputCapture clean = captureTitoOutput(mode, 0.f, 880.f, 0.58f, 0.86f, 0.f);
    const TitoOutputCapture xm = captureTitoOutput(mode, 1.f, 880.f, 0.58f, 0.86f, 0.f);
    const TitoOutputCapture sm = captureTitoOutput(mode, -1.f, 880.f, 0.58f, 0.86f, 0.f);

    const float xmDistance = normalizedWaveDistance(clean, xm);
    const float smDistance = normalizedWaveDistance(clean, sm);
    const bool finite = clean.finite && xm.finite && sm.finite
      && std::isfinite(xmDistance) && std::isfinite(smDistance)
      && std::isfinite(clean.rms) && std::isfinite(xm.rms) && std::isfinite(sm.rms);
    const bool audibleContrast = (xmDistance > 0.012f) || (smDistance > 0.012f);
    pass = pass && finite && audibleContrast;

    if (smDistance < worstSmDistance) {
      worstSmDistance = smDistance;
      worstSmMode = mode;
    }
    if (xmDistance < worstXmDistance) {
      worstXmDistance = xmDistance;
      worstXmMode = mode;
    }
    bestSmDistance = std::max(bestSmDistance, smDistance);
    bestXmDistance = std::max(bestXmDistance, xmDistance);

    if (finite && !audibleContrast) {
      weakCases += " m" + std::to_string(mode)
        + "(xm=" + std::to_string(xmDistance) + ",sm=" + std::to_string(smDistance) + ")";
    }
    else if (!finite) {
      weakCases += " m" + std::to_string(mode) + "(nonfinite)";
    }
  }

  return {
    "Runtime TITO XM/SM stays finite and changes output across modes",
    pass,
    "worstXm=m" + std::to_string(worstXmMode) + ":" + std::to_string(worstXmDistance) +
      " worstSm=m" + std::to_string(worstSmMode) +
      ":" + std::to_string(worstSmDistance) +
      " bestXm=" + std::to_string(bestXmDistance) +
      " bestSm=" + std::to_string(bestSmDistance) +
      " weak=" + weakCases
  };
}

TestResult testRuntimeSelfOscSoftOnsetRamp() {
  const ZeroInputCapture low = captureZeroInputOutput(5, 900.f, 0.58f, 0.78f, 0.f, 0.5f, 12000, 20000);
  const ZeroInputCapture onset = captureZeroInputOutput(5, 900.f, 0.58f, 0.90f, 0.f, 0.5f, 12000, 20000);
  const ZeroInputCapture hot = captureZeroInputOutput(5, 900.f, 0.58f, 1.00f, 0.f, 0.5f, 12000, 20000);

  const bool finite = low.finite && onset.finite && hot.finite;
  const bool onsetRises = onset.rms > std::max(low.rms * 1.4f, 1e-4f);
  const bool hotRemainsActive = hot.rms > 0.02f;
  const bool bounded = onset.peakAbs < 20.f && hot.peakAbs < 20.f;
  const bool pass = finite && onsetRises && hotRemainsActive && bounded;
  return {
    "Runtime self-osc onset ramps near upper-RESO region without abrupt jump",
    pass,
    "rms(low,onset,hot)=(" + std::to_string(low.rms) + "," + std::to_string(onset.rms) + "," +
      std::to_string(hot.rms) + ") peak(onset,hot)=(" + std::to_string(onset.peakAbs) + "," +
      std::to_string(hot.peakAbs) + ")"
  };
}

TestResult testRuntimeSelfOscHighResBounded() {
  const ZeroInputCapture hot = captureZeroInputOutput(0, 900.f, 0.55f, 1.00f, 0.f, 0.5f, 12000, 96000);
  const bool pass = hot.finite && hot.rms > 0.02f && hot.peakAbs < 20.f;
  return {
    "Runtime high-RESO self-osc stays finite and bounded",
    pass,
    "finite=" + std::to_string(int(hot.finite)) + " rms=" + std::to_string(hot.rms) +
      " peakAbs=" + std::to_string(hot.peakAbs)
  };
}

TestResult testDisplayOnlyColorSchemeJsonRoundTripAndPassThrough() {
  bool pass = true;
  std::string detail;

  for (int scheme = 0; scheme < Bifurx::SCHEME_LEN; ++scheme) {
    Bifurx source;
    source.onReset();
    source.params[Bifurx::MODE_PARAM].setValue(float(kBifurxDisplayOnlyMode));
    source.colorScheme = (Bifurx::ColorScheme) scheme;

    json_t* stateJ = source.dataToJson();
    if (!stateJ) {
      return {"Display-only JSON round-trip + pass-through", false, "dataToJson returned null"};
    }

    Bifurx loaded;
    loaded.onReset();
    loaded.params[Bifurx::MODE_PARAM].setValue(float(kBifurxDisplayOnlyMode));
    loaded.dataFromJson(stateJ);
    json_decref(stateJ);

    const bool schemeOk = int(loaded.colorScheme) == scheme;
    pass = pass && schemeOk;

    Module::ProcessArgs args;
    args.sampleRate = 48000.f;
    args.sampleTime = 1.f / args.sampleRate;

    float inSq = 0.f;
    float errSq = 0.f;
    for (int n = 0; n < 4096; ++n) {
      const float t = float(n) * args.sampleTime;
      const float in = 3.6f * std::sin(2.f * kRuntimePi * 733.f * t) +
        0.9f * std::sin(2.f * kRuntimePi * 1471.f * t + 0.31f);
      loaded.inputs[Bifurx::IN_INPUT].setVoltage(in);
      loaded.process(args);
      const float out = loaded.outputs[Bifurx::OUT_OUTPUT].getVoltage();
      const float e = out - in;
      inSq += in * in;
      errSq += e * e;
      pass = pass && std::isfinite(out);
    }

    const float relErr = std::sqrt(errSq / std::max(inSq, 1e-12f));
    const bool passThroughOk = relErr < 1e-6f;
    pass = pass && passThroughOk;

    detail += " scheme" + std::to_string(scheme) +
      "(ok=" + std::to_string(int(schemeOk)) +
      ",relErr=" + std::to_string(relErr) + ")";
  }

  {
    Bifurx clamped;
    clamped.onReset();
    json_t* root = json_object();
    json_object_set_new(root, "colorScheme", json_integer(1234));
    clamped.dataFromJson(root);
    json_decref(root);
    const bool clampOk = int(clamped.colorScheme) == int(Bifurx::SCHEME_LEN - 1);
    pass = pass && clampOk;
    detail += " clampOk=" + std::to_string(int(clampOk));
  }

  return {
    "Display-only JSON round-trip + pass-through",
    pass,
    detail
  };
}

TestResult testTwoColorFftGradientMidpointAndJsonRoundTrip() {
  bool pass = true;
  bool authoredMiddleDiffers = false;
  for (int scheme = 0; scheme < Bifurx::SCHEME_LEN; ++scheme) {
    const BifurxColors authored = BifurxColors::get((Bifurx::ColorScheme) scheme, true);
    const BifurxColors continuous = BifurxColors::get((Bifurx::ColorScheme) scheme, false);
    const NVGcolor midpoint = mixColor(authored.low, authored.high, 0.5f);
    const auto close = [](float a, float b) { return std::fabs(a - b) < 1e-6f; };
    pass = pass
      && close(continuous.low.r, authored.low.r)
      && close(continuous.low.g, authored.low.g)
      && close(continuous.low.b, authored.low.b)
      && close(continuous.high.r, authored.high.r)
      && close(continuous.high.g, authored.high.g)
      && close(continuous.high.b, authored.high.b)
      && close(continuous.white.r, midpoint.r)
      && close(continuous.white.g, midpoint.g)
      && close(continuous.white.b, midpoint.b)
      && close(continuous.white.a, midpoint.a);
    authoredMiddleDiffers = authoredMiddleDiffers
      || !close(authored.white.r, midpoint.r)
      || !close(authored.white.g, midpoint.g)
      || !close(authored.white.b, midpoint.b);
  }

  Bifurx source;
  source.threeColorFftGradient.store(true, std::memory_order_relaxed);
  source.legacyVisuals.store(true, std::memory_order_relaxed);
  json_t* stateJ = source.dataToJson();
  Bifurx loaded;
  loaded.dataFromJson(stateJ);
  json_decref(stateJ);
  const bool roundTripEnabled = loaded.threeColorFftGradient.load(std::memory_order_relaxed);
  const bool legacyVisualsRoundTrip = loaded.legacyVisuals.load(std::memory_order_relaxed);

  Bifurx legacyDefault;
  json_t* legacyJ = json_object();
  legacyDefault.dataFromJson(legacyJ);
  json_decref(legacyJ);
  const bool threeColorDefaultDisabled = !legacyDefault.threeColorFftGradient.load(std::memory_order_relaxed);
  const bool modernVisualsDefault = !legacyDefault.legacyVisuals.load(std::memory_order_relaxed);

  pass = pass && authoredMiddleDiffers && roundTripEnabled && legacyVisualsRoundTrip
    && threeColorDefaultDisabled && modernVisualsDefault;
  return {
    "Two-color FFT gradient uses endpoint midpoint and persists",
    pass,
    "authoredMiddleDiffers=" + std::to_string(int(authoredMiddleDiffers)) +
      " roundTripEnabled=" + std::to_string(int(roundTripEnabled)) +
      " legacyVisualsRoundTrip=" + std::to_string(int(legacyVisualsRoundTrip)) +
      " threeColorDefaultDisabled=" + std::to_string(int(threeColorDefaultDisabled)) +
      " modernVisualsDefault=" + std::to_string(int(modernVisualsDefault))
  };
}

TestResult testBrowserPreviewUsesAuthoredUndertowScene() {
  BifurxSpectrumBase display;
  display.initializeStaticPreviewStateIfNeeded();

  float overlayMin = kOverlayDbfsCeiling;
  float overlayMax = kOverlayDbfsFloor;
  bool finite = true;
  for (int i = 0; i < kCurvePointCount; ++i) {
    const float value = display.state.overlayOutputDbfs[i];
    finite = finite && std::isfinite(value);
    overlayMin = std::min(overlayMin, value);
    overlayMax = std::max(overlayMax, value);
  }

  const BifurxPreviewState& preview = display.state.previewState;
  const bool configured = preview.mode == kBrowserPreviewMode &&
    std::fabs(preview.spanParamNorm - kBrowserPreviewSpan) < 1e-4f &&
    std::fabs(preview.resoNorm - kBrowserPreviewResonance) < 1e-4f &&
    std::fabs(preview.balance - kBrowserPreviewBalance) < 1e-4f &&
    preview.freqA > 90.f && preview.freqA < 120.f &&
    preview.freqB > 3500.f && preview.freqB < 4500.f;
  const bool undertowShapeVaries = previewProbeStimulusSample(preview, 0) > 4.f &&
    previewProbeStimulusSample(preview, 60) < -3.f;
  const bool hasSpectrum = display.state.hasOverlay && (overlayMax - overlayMin) > 12.f;

  return {
    "Browser preview recreates the authored Undertow Morph into Low + Band scene",
    configured && undertowShapeVaries && finite && hasSpectrum,
    "cutoffsHz=(" + std::to_string(preview.freqA) + "," + std::to_string(preview.freqB) +
      ") rangeDb=" + std::to_string(overlayMax - overlayMin) +
      " topDbfs=" + std::to_string(display.state.displayTopDbfs)
  };
}

}  // namespace

int main() {
  const std::vector<TestResult> tests = {
    testSpanShapeLutTracksReferenceCurve(),
    testResonanceCurveUsesFullControlTravel(),
    testFrequencyQuantityRoundTripsAccurately(),
    testPreviewInvalidatesForModelDependencies(),
    testPublishedSnapshotsRemainCoherentUnderContention(),
    testAnalysisFramesPublishAsOverlappingWindows(),
    testAnalysisCaptureSleepsWithoutVisualSubscriber(),
    testVisualWorkerDefaultSetterHonorsMode(),
    testResetClearsRuntimeState(),
    testVoctStepsImmediatelyWithoutImplicitGlide(),
    testSpanIsPreservedAtFrequencyRails(),
    testRuntimeHighHighHasUnityCascadeGain(),
    testRuntimeSpanMonotonicInPreviewState(),
    testRuntimeBalanceTiltsBandBandInSvf(),
    testRuntimeReportedLowCaseKeepsAudibleOutput(),
    testRuntimeLlDropoutRegressionSweep(),
    testRuntimeCurveFamiliesRemainDistinct(),
    testRuntimePreviewPublishesHighQBeyondLegacyClamp(),
    testRuntimePreviewMarkerGainTracksBandHeavyModes(),
    testRuntimeTitoProducesFiniteContrastAcrossModes(),
    testRuntimeSelfOscSoftOnsetRamp(),
    testRuntimeSelfOscHighResBounded(),
    testDisplayOnlyColorSchemeJsonRoundTripAndPassThrough(),
    testTwoColorFftGradientMidpointAndJsonRoundTrip(),
    testBrowserPreviewUsesAuthoredUndertowScene(),
  };

  int fails = 0;
  for (const TestResult& t : tests) {
    std::cout << (t.pass ? "[PASS] " : "[FAIL] ") << t.name;
    if (!t.detail.empty()) {
      std::cout << " :: " << t.detail;
    }
    std::cout << "\n";
    if (!t.pass) {
      fails++;
    }
  }

  if (fails > 0) {
    std::cout << "[SUMMARY] bifurx_runtime_spec failed " << fails << " / " << tests.size() << " tests\n";
    return 1;
  }
  std::cout << "[SUMMARY] bifurx_runtime_spec passed " << tests.size() << " tests\n";
  return 0;
}
