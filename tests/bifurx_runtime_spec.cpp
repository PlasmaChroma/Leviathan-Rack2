#include "../src/plugin.hpp"

#include <cmath>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

// Provide the plugin global expected by module source.
Plugin* pluginInstance = nullptr;

#include "../src/Bifurx.cpp"

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
  module.params[Bifurx::TITO_PARAM].setValue(1.f);   // Clean
}

void clearCvInputs(Bifurx& module) {
  module.inputs[Bifurx::VOCT_INPUT].setChannels(0);
  module.inputs[Bifurx::FM_INPUT].setChannels(0);
  module.inputs[Bifurx::RESO_CV_INPUT].setChannels(0);
  module.inputs[Bifurx::BALANCE_CV_INPUT].setChannels(0);
  module.inputs[Bifurx::SPAN_CV_INPUT].setChannels(0);
}

float measureRuntimeGainDb(
  int circuitMode,
  int mode,
  float inputHz,
  float inputAmplitude,
  float freqNorm,
  float spanNorm,
  float reso,
  float balance
) {
  Bifurx module;
  module.onReset();
  module.filterCircuitMode = clampCircuitMode(circuitMode);
  module.activeCircuitMode = 0;
  module.pendingCircuitSwitch = false;
  module.pendingSolveSteps = 0;
  module.activeCircuitAlignA = 1.f;
  module.activeCircuitAlignB = 1.f;
  module.resetCircuitStates();

  configureBaseParams(module, mode, freqNorm, spanNorm, reso, balance);
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

bool capturePreviewStateForSpan(float spanNorm, BifurxPreviewState* outState) {
  if (!outState) {
    return false;
  }
  Bifurx module;
  module.onReset();
  module.filterCircuitMode = 0;
  module.activeCircuitMode = 0;
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
  int circuitMode,
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
  module.filterCircuitMode = clampCircuitMode(circuitMode);
  module.activeCircuitMode = 0;
  module.pendingCircuitSwitch = false;
  module.pendingSolveSteps = 0;
  module.activeCircuitAlignA = 1.f;
  module.activeCircuitAlignB = 1.f;
  module.resetCircuitStates();

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

  const float negLow = measureRuntimeGainDb(0, 5, 340.f, 0.10f, freqNorm, spanNorm, 0.35f, -0.85f);
  const float negHigh = measureRuntimeGainDb(0, 5, 1500.f, 0.10f, freqNorm, spanNorm, 0.35f, -0.85f);
  const float posLow = measureRuntimeGainDb(0, 5, 340.f, 0.10f, freqNorm, spanNorm, 0.35f, 0.85f);
  const float posHigh = measureRuntimeGainDb(0, 5, 1500.f, 0.10f, freqNorm, spanNorm, 0.35f, 0.85f);

  const bool lowFavoredWhenNegative = negLow > (negHigh + 1.f);
  const bool highFavoredWhenPositive = posHigh > (posLow + 1.f);
  return {
    "Runtime BALANCE tilts BB low/high emphasis (SVF)",
    lowFavoredWhenNegative && highFavoredWhenPositive,
    "neg(low,high)=(" + std::to_string(negLow) + "," + std::to_string(negHigh) + ") "
      "pos(low,high)=(" + std::to_string(posLow) + "," + std::to_string(posHigh) + ")"
  };
}

TestResult testRuntimeReportedLowCaseAcrossCircuitsKeepsAudibleOutput() {
  // User-reported scenario: 40Hz input with first LL marker around 53.9Hz.
  const float centerHz = std::sqrt(53.9f * 114.f);
  const float freqNorm = freqNormForCenterHz(centerHz);
  const float spanNorm = clamp(std::log2(114.f / 53.9f) / 8.f, 0.f, 1.f);

  float gains[4] = {};
  for (int circuit = 0; circuit < 4; ++circuit) {
    gains[circuit] = measureRuntimeGainDb(circuit, 0, 40.f, 0.25f, freqNorm, spanNorm, 0.35f, 0.f);
  }

  const float svf = gains[0];
  bool pass = true;
  for (int i = 0; i < 4; ++i) {
    pass = pass && std::isfinite(gains[i]);
    pass = pass && (gains[i] > -36.f);
    pass = pass && (gains[i] > (svf - 30.f));
  }

  return {
    "Runtime LL low-frequency case keeps non-SVF circuits above collapse floor",
    pass,
    "gainDb(SVF,DFM,MS2,PRD)=(" + std::to_string(gains[0]) + "," + std::to_string(gains[1]) + "," +
      std::to_string(gains[2]) + "," + std::to_string(gains[3]) + ")"
  };
}

TestResult testRuntimeLlDropoutRegressionSweepAcrossCircuits() {
  // Regression guard aimed at the reported LL dropout behavior for
  // non-SVF circuits near the first marker pair.
  const float centerHz = std::sqrt(53.9f * 114.f);
  const float freqNorm = freqNormForCenterHz(centerHz);
  const float spanNorm = clamp(std::log2(114.f / 53.9f) / 8.f, 0.f, 1.f);
  const float reso = 0.35f;
  const float balance = 0.f;
  const float amp = 0.18f;

  const float lowBandHz[] = {32.f, 40.f, 53.9f, 70.f, 90.f, 114.f};
  const float upperBandHz[] = {250.f, 500.f, 1200.f};

  float lowAvg[4] = {};
  float lowMin[4] = {};
  float lowMinHz[4] = {};
  float upperAvg[4] = {};
  bool pass = true;
  std::string detail;

  for (int circuit = 0; circuit < 4; ++circuit) {
    float lowSum = 0.f;
    float lowFloor = 1e9f;
    int lowCount = 0;
    for (float hz : lowBandHz) {
      const float g = measureRuntimeGainDb(circuit, 0, hz, amp, freqNorm, spanNorm, reso, balance);
      lowSum += g;
      if (g < lowFloor) {
        lowFloor = g;
        lowMinHz[circuit] = hz;
      }
      lowCount++;
      if (!std::isfinite(g)) {
        pass = false;
      }
    }

    float upperSum = 0.f;
    int upperCount = 0;
    for (float hz : upperBandHz) {
      const float g = measureRuntimeGainDb(circuit, 0, hz, amp, freqNorm, spanNorm, reso, balance);
      upperSum += g;
      upperCount++;
      if (!std::isfinite(g)) {
        pass = false;
      }
    }

    lowAvg[circuit] = lowSum / std::max(1, lowCount);
    lowMin[circuit] = lowFloor;
    upperAvg[circuit] = upperSum / std::max(1, upperCount);

    // LL should keep low-band energy from collapsing and remain "low-forward"
    // relative to upper content for the reported marker placement.
    const bool circuitPass = (lowMin[circuit] > -30.f)
      && (lowAvg[circuit] > -20.f)
      && (lowAvg[circuit] > (upperAvg[circuit] - 10.f));
    pass = pass && circuitPass;
  }

  const float svfLowAvg = lowAvg[0];
  for (int circuit = 1; circuit < 4; ++circuit) {
    // Non-SVF circuits may differ in voicing, but should not collapse compared
    // to SVF by an extreme margin in this LL regression window.
    pass = pass && (lowAvg[circuit] > (svfLowAvg - 24.f));
  }

  detail =
    "lowAvg(SVF,DFM,MS2,PRD)=(" + std::to_string(lowAvg[0]) + "," + std::to_string(lowAvg[1]) + "," +
      std::to_string(lowAvg[2]) + "," + std::to_string(lowAvg[3]) + ") "
    "lowMin=(" + std::to_string(lowMin[0]) + "," + std::to_string(lowMin[1]) + "," +
      std::to_string(lowMin[2]) + "," + std::to_string(lowMin[3]) + ") "
    "lowMinHz=(" + std::to_string(lowMinHz[0]) + "," + std::to_string(lowMinHz[1]) + "," +
      std::to_string(lowMinHz[2]) + "," + std::to_string(lowMinHz[3]) + ") "
    "upperAvg=(" + std::to_string(upperAvg[0]) + "," + std::to_string(upperAvg[1]) + "," +
      std::to_string(upperAvg[2]) + "," + std::to_string(upperAvg[3]) + ")";

  return {
    "Runtime LL dropout regression sweep stays above floor across circuits",
    pass,
    detail
  };
}

TestResult testRuntimeCurveFamiliesRemainDistinctPerCircuit() {
  const std::vector<int> modes = {0, 3, 5, 9};  // LL, NN, BB, HH
  const std::vector<float> hz = {40.f, 120.f, 320.f, 900.f, 2200.f, 6000.f};
  std::string detail;
  bool pass = true;

  for (int circuit = 0; circuit < 4; ++circuit) {
    std::vector<std::vector<float>> signatures;
    signatures.reserve(modes.size());
    for (int mode : modes) {
      BifurxPreviewState state;
      const bool ok = capturePreviewState(circuit, mode, 900.f, 0.55f, 0.35f, 0.f, &state);
      if (!ok) {
        pass = false;
        detail += " c" + std::to_string(circuit) + "m" + std::to_string(mode) + "=no_preview";
        continue;
      }
      signatures.push_back(curveSignatureDb(state, hz));
    }

    if (signatures.size() != modes.size()) {
      continue;
    }

    const float d03 = l1Distance(signatures[0], signatures[1]);
    const float d05 = l1Distance(signatures[0], signatures[2]);
    const float d09 = l1Distance(signatures[0], signatures[3]);
    const float d35 = l1Distance(signatures[1], signatures[2]);
    const float d39 = l1Distance(signatures[1], signatures[3]);
    const float d59 = l1Distance(signatures[2], signatures[3]);
    const bool distinct = (d03 > 10.f) && (d05 > 10.f) && (d09 > 10.f)
      && (d35 > 10.f) && (d39 > 10.f) && (d59 > 10.f);
    pass = pass && distinct;
    detail += " c" + std::to_string(circuit) + "(d03,d05,d09,d35,d39,d59)=(" +
      std::to_string(d03) + "," + std::to_string(d05) + "," + std::to_string(d09) + "," +
      std::to_string(d35) + "," + std::to_string(d39) + "," + std::to_string(d59) + ")";
  }

  return {
    "Runtime LL/NN/BB/HH curve families stay distinct per circuit",
    pass,
    detail
  };
}

TestResult testRuntimeCircuitsProduceDifferentBandBandCurves() {
  const std::vector<float> hz = {90.f, 220.f, 500.f, 1100.f, 2500.f, 5600.f};
  std::vector<std::vector<float>> signatures;
  signatures.reserve(4);
  bool pass = true;

  for (int circuit = 0; circuit < 4; ++circuit) {
    BifurxPreviewState state;
    const bool ok = capturePreviewState(circuit, 5, 900.f, 0.65f, 0.42f, 0.f, &state);  // BB
    if (!ok) {
      pass = false;
      signatures.push_back({});
      continue;
    }
    signatures.push_back(curveSignatureDb(state, hz));
  }

  const float d01 = l1Distance(signatures[0], signatures[1]);
  const float d02 = l1Distance(signatures[0], signatures[2]);
  const float d03 = l1Distance(signatures[0], signatures[3]);
  const float d12 = l1Distance(signatures[1], signatures[2]);
  const float d13 = l1Distance(signatures[1], signatures[3]);
  const float d23 = l1Distance(signatures[2], signatures[3]);
  const bool distinct = (d01 > 0.8f) && (d02 > 0.8f) && (d03 > 0.8f)
    && (d12 > 0.8f) && (d13 > 0.8f) && (d23 > 0.8f);

  pass = pass && distinct;
  return {
    "Runtime BB curve differs across SVF/DFM/MS2/PRD",
    pass,
    "d01=" + std::to_string(d01) + " d02=" + std::to_string(d02) + " d03=" + std::to_string(d03) +
      " d12=" + std::to_string(d12) + " d13=" + std::to_string(d13) + " d23=" + std::to_string(d23)
  };
}

}  // namespace

int main() {
  const std::vector<TestResult> tests = {
    testRuntimeSpanMonotonicInPreviewState(),
    testRuntimeBalanceTiltsBandBandInSvf(),
    testRuntimeReportedLowCaseAcrossCircuitsKeepsAudibleOutput(),
    testRuntimeLlDropoutRegressionSweepAcrossCircuits(),
    testRuntimeCurveFamiliesRemainDistinctPerCircuit(),
    testRuntimeCircuitsProduceDifferentBandBandCurves(),
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
