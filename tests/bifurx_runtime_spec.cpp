#include "../src/plugin.hpp"

#include <cmath>
#include <iostream>
#include <string>
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

float freqNormForCenterHz(float centerHz) {
  constexpr float kFreqMinHz = 4.f;
  constexpr float kFreqLog2Span = 12.7731392f;  // log2(28000 / 4)
  const float safeCenter = std::max(centerHz, kFreqMinHz);
  return clamp(std::log2(safeCenter / kFreqMinHz) / kFreqLog2Span, 0.f, 1.f);
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

  const int idx = module.previewPublishedIndex.load(std::memory_order_acquire);
  *outState = module.previewStates[idx];
  return true;
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

  const int idx = module.previewPublishedIndex.load(std::memory_order_acquire);
  *outState = module.previewStates[idx];
  return true;
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

}  // namespace

int main() {
  const std::vector<TestResult> tests = {
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
