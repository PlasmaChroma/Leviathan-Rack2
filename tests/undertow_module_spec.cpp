#include "../src/plugin.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

Plugin* pluginInstance = nullptr;

bool isDragonKingDebugEnabled() {
  return false;
}

bool isDragonKingPreviewWidgetOptionsEnabled() {
  return true;
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

#include "../src/Undertow.cpp"

namespace {

struct Sample {
  float sine = 0.f;
  float shape = 0.f;
  float sub = 0.f;
  float displayHz = 0.f;
  float previewShape = 0.f;
  float syncLight = 0.f;
  float sGateLight = 0.f;
};

struct Trace {
  std::string name;
  std::vector<Sample> samples;
  float sineAbsPeak = 0.f;
};

struct Summary {
  double sineSum = 0.0;
  double sineEnergy = 0.0;
  double shapeSum = 0.0;
  double shapeEnergy = 0.0;
  double subSum = 0.0;
  double subEnergy = 0.0;
  double syncLightSum = 0.0;
  double sGateLightSum = 0.0;
  double displayHz = 0.0;
  double previewShape = 0.0;
  double sineAbsPeak = 0.0;
};

struct TestResult {
  std::string name;
  bool pass = false;
  std::string detail;
};

Module::ProcessArgs processArgs(float sampleRate = 48000.f) {
  Module::ProcessArgs args;
  args.sampleRate = sampleRate;
  args.sampleTime = 1.f / sampleRate;
  return args;
}

void connectInput(Undertow& module, int inputId) {
  module.inputs[inputId].channels = 1;
}

void setInputChannels(Undertow& module, int inputId, int channels) {
  module.inputs[inputId].channels = uint8_t(channels);
}

void connectAllOutputs(Undertow& module) {
  for (int outputId = 0; outputId < Undertow::OUTPUTS_LEN; ++outputId) {
    module.outputs[outputId].channels = 1;
  }
}

bool nearlyEqual(float actual, float expected, float tolerance = 1e-5f) {
  return std::fabs(actual - expected) <= tolerance;
}

Sample readSample(Undertow& module) {
  Sample sample;
  sample.sine = module.outputs[Undertow::SINE_OUTPUT].getVoltage();
  sample.shape = module.outputs[Undertow::SHAPE_OUTPUT].getVoltage();
  sample.sub = module.outputs[Undertow::SUB_OUTPUT].getVoltage();
  sample.displayHz = module.displayFrequencyHz.load(std::memory_order_relaxed);
  sample.previewShape = module.displayShapeAmount.load(std::memory_order_relaxed);
  sample.syncLight = module.lights[Undertow::SYNC_LIGHT].getBrightness();
  sample.sGateLight = module.lights[Undertow::S_GATE_LIGHT].getBrightness();
  return sample;
}

template <typename Drive>
Trace renderTrace(const std::string& name, int sampleCount, float sampleRate, Drive drive) {
  Undertow module;
  Module::ProcessArgs args = processArgs(sampleRate);
  Trace trace;
  trace.name = name;
  trace.samples.reserve(sampleCount);
  for (int frame = 0; frame < sampleCount; ++frame) {
    drive(module, frame);
    args.frame = frame;
    module.process(args);
    const Sample sample = readSample(module);
    trace.samples.push_back(sample);
    trace.sineAbsPeak = std::max(trace.sineAbsPeak, std::fabs(sample.sine));
  }
  return trace;
}

Trace freeRunningTrace() {
  return renderTrace("free", 4096, 48000.f, [](Undertow& module, int frame) {
    (void) module;
    (void) frame;
  });
}

Trace modulationTrace() {
  return renderTrace("modulation", 4096, 48000.f, [](Undertow& module, int frame) {
    if (frame == 0) {
      connectInput(module, Undertow::V_OCT_INPUT);
      connectInput(module, Undertow::EXPO_INPUT);
      connectInput(module, Undertow::LIN_FM_INPUT);
      connectInput(module, Undertow::SHAPE_CV_INPUT);
      module.params[Undertow::FINE_PARAM].setValue(-37.f);
      module.params[Undertow::LIN_FM_PARAM].setValue(0.86f);
      module.params[Undertow::SHAPE_PARAM].setValue(0.73f);
      module.params[Undertow::EDGE_HARDNESS_PARAM].setValue(0.91f);
      module.shapeEntryAsymmetry.store(true, std::memory_order_relaxed);
      module.shapeEntryAsymmetryOnRight.store(true, std::memory_order_relaxed);
    }
    const int ramp = (frame % 257) - 128;
    module.inputs[Undertow::V_OCT_INPUT].setVoltage(-0.5f + float((frame / 683) % 3) * 0.25f);
    module.inputs[Undertow::EXPO_INPUT].setVoltage(0.125f);
    module.inputs[Undertow::LIN_FM_INPUT].setVoltage(float(ramp) * (7.5f / 128.f));
    module.inputs[Undertow::SHAPE_CV_INPUT].setVoltage(5.25f);
  });
}

Trace syncTrace(float sampleRate, bool analogCharacter) {
  const std::string name = std::string(analogCharacter ? "sync-character-" : "sync-clean-")
      + std::to_string(int(sampleRate));
  return renderTrace(name, 8192, sampleRate,
                     [analogCharacter](Undertow& module, int frame) {
    if (frame == 0) {
      connectInput(module, Undertow::SYNC_INPUT);
      module.params[Undertow::COARSE_PARAM].setValue(undertowKnobValueForFrequency(997.f));
      module.params[Undertow::FINE_PARAM].setValue(23.f);
      module.analogCharacterEnabled.store(analogCharacter, std::memory_order_relaxed);
    }
    const bool pulse = frame == 311 || frame == 997 || frame == 2039 || frame == 4093 || frame == 6151;
    module.inputs[Undertow::SYNC_INPUT].setVoltage(pulse ? 10.f : 0.f);
  });
}

float measureSyncStressPeak() {
  static const float sampleRates[] {44100.f, 48000.f, 96000.f, 192000.f};
  static const float frequencies[] {10.f, 261.63f, 997.f, 10000.f};
  float peak = 0.f;
  for (float sampleRate : sampleRates) {
    for (float frequency : frequencies) {
      for (bool character : {false, true}) {
        Undertow module;
        connectInput(module, Undertow::SYNC_INPUT);
        module.params[Undertow::COARSE_PARAM].setValue(undertowKnobValueForFrequency(frequency));
        module.analogCharacterEnabled.store(character, std::memory_order_relaxed);
        Module::ProcessArgs args = processArgs(sampleRate);
        const int sampleCount = int(sampleRate * 0.75f);
        for (int frame = 0; frame < sampleCount; ++frame) {
          // Coprime pulse spacings exercise many oscillator phases while leaving
          // at least one low sample to rearm the Schmitt trigger.
          const bool pulse = (frame % 37) == 13 || (frame % 101) == 67;
          module.inputs[Undertow::SYNC_INPUT].setVoltage(pulse ? 10.f : 0.f);
          args.frame = frame;
          module.process(args);
          peak = std::max(peak, std::fabs(module.outputs[Undertow::SINE_OUTPUT].getVoltage()));
        }
      }
    }
  }
  return peak;
}

Trace subGateAndModesTrace() {
  return renderTrace("sub-gate-modes", 4096, 44100.f, [](Undertow& module, int frame) {
    if (frame == 0) {
      connectInput(module, Undertow::S_GATE_INPUT);
      module.params[Undertow::COARSE_PARAM].setValue(undertowKnobValueForFrequency(333.f));
      module.params[Undertow::SHAPE_PARAM].setValue(0.42f);
    }
    if (frame == 2048) {
      module.params[Undertow::COARSE_STEP_MODE_PARAM].setValue(1.f);
      module.analogCharacterEnabled.store(false, std::memory_order_relaxed);
    }
    const int gatePhase = frame % 521;
    module.inputs[Undertow::S_GATE_INPUT].setVoltage(gatePhase >= 73 && gatePhase < 337 ? 10.f : 0.f);
  });
}

const std::vector<int>& checkpointFrames(const Trace& trace) {
  static const std::vector<int> shortFrames {0, 1, 2, 31, 127, 509, 1023, 2047, 3071, 4095};
  static const std::vector<int> syncFrames {0, 310, 311, 312, 996, 997, 998, 2038, 2039, 2040,
                                            4092, 4093, 4094, 6150, 6151, 6152, 8191};
  return trace.samples.size() > 4096 ? syncFrames : shortFrames;
}

Summary summarize(const Trace& trace) {
  Summary summary;
  for (const Sample& s : trace.samples) {
    summary.sineSum += s.sine;
    summary.sineEnergy += double(s.sine) * double(s.sine);
    summary.shapeSum += s.shape;
    summary.shapeEnergy += double(s.shape) * double(s.shape);
    summary.subSum += s.sub;
    summary.subEnergy += double(s.sub) * double(s.sub);
    summary.syncLightSum += s.syncLight;
    summary.sGateLightSum += s.sGateLight;
  }
  const Sample& final = trace.samples.back();
  summary.displayHz = final.displayHz;
  summary.previewShape = final.previewShape;
  summary.sineAbsPeak = trace.sineAbsPeak;
  return summary;
}

uint64_t quantizedTraceHash(const Trace& trace) {
  uint64_t hash = 1469598103934665603ull;
  auto append = [&hash](float value) {
    const int64_t quantized = std::llround(double(value) * 10000.0);
    for (int byte = 0; byte < 8; ++byte) {
      hash ^= uint8_t(uint64_t(quantized) >> (byte * 8));
      hash *= 1099511628211ull;
    }
  };
  for (const Sample& s : trace.samples) {
    append(s.sine);
    append(s.shape);
    append(s.sub);
    append(s.displayHz);
    append(s.previewShape);
    append(s.syncLight);
    append(s.sGateLight);
  }
  return hash;
}

void printCapture(const Trace& trace) {
  std::cout << "TRACE " << trace.name << " peak=" << std::setprecision(9) << trace.sineAbsPeak << '\n';
  for (int frame : checkpointFrames(trace)) {
    const Sample& s = trace.samples.at(frame);
    std::cout << frame << " {" << s.sine << "f, " << s.shape << "f, " << s.sub << "f, "
              << s.displayHz << "f, " << s.previewShape << "f, " << s.syncLight << "f, "
              << s.sGateLight << "f}\n";
  }
  const Summary summary = summarize(trace);
  std::cout << "SUMMARY " << trace.name << " {" << std::setprecision(17)
            << summary.sineSum << ", " << summary.sineEnergy << ", " << summary.shapeSum << ", "
            << summary.shapeEnergy << ", " << summary.subSum << ", " << summary.subEnergy << ", "
            << summary.syncLightSum << ", " << summary.sGateLightSum << ", " << summary.displayHz << ", "
            << summary.previewShape << ", " << summary.sineAbsPeak << "}\n";
  std::cout << "HASH " << trace.name << " " << quantizedTraceHash(trace) << '\n';
}

TestResult tracesAreHealthy(const std::vector<Trace>& traces) {
  for (const Trace& trace : traces) {
    for (size_t frame = 0; frame < trace.samples.size(); ++frame) {
      const Sample& s = trace.samples[frame];
      if (!std::isfinite(s.sine) || !std::isfinite(s.shape) || !std::isfinite(s.sub)
          || !std::isfinite(s.displayHz) || !std::isfinite(s.previewShape)
          || !std::isfinite(s.syncLight) || !std::isfinite(s.sGateLight)) {
        return {"captured monophonic traces are finite", false,
                trace.name + " became non-finite at frame " + std::to_string(frame)};
      }
    }
  }
  return {"captured monophonic traces are finite", true, ""};
}

TestResult releasedSchemaIsStable() {
  const bool pass = Undertow::COARSE_PARAM == 0
      && Undertow::EDGE_HARDNESS_PARAM == 5
      && Undertow::PARAMS_LEN == 6
      && Undertow::V_OCT_INPUT == 0
      && Undertow::S_GATE_INPUT == 5
      && Undertow::INPUTS_LEN == 6
      && Undertow::SINE_OUTPUT == 0
      && Undertow::SUB_OUTPUT == 2
      && Undertow::OUTPUTS_LEN == 3
      && Undertow::SYNC_LIGHT == 0
      && Undertow::COARSE_STEP_MODE_LIGHT == 2
      && Undertow::LIGHTS_LEN == 3;
  return {"released Undertow parameter, port, and light schema remains stable", pass,
          pass ? "" : "released enum IDs or counts changed"};
}

bool closeEnough(double actual, double expected) {
  return std::fabs(actual - expected) <= 0.02 + std::fabs(expected) * 2e-5;
}

std::array<double, 11> summaryValues(const Summary& s) {
  return {s.sineSum, s.sineEnergy, s.shapeSum, s.shapeEnergy, s.subSum, s.subEnergy,
          s.syncLightSum, s.sGateLightSum, s.displayHz, s.previewShape, s.sineAbsPeak};
}

TestResult monophonicReferenceIsStable(const std::vector<Trace>& traces) {
  static const Summary expected[] {
      {161.29912575986236, 58277.659040547835, 161.30947360303253, 58261.236935288136,
       -299.43023180961609, 101255.97070219701, 0, 0, 261.62997436523438, 0, 5.0133771896362305},
      {-441.89271344942972, 59065.200999305729, 930.6217290838249, 49787.849486682564,
       -5.113079309463501, 101327.11605826281, 0, 0, 279.27716064453125, 0.47906249761581421,
       5.0140213966369629},
      {-71.700061572017148, 102469.90398290107, -71.660000098170713, 102412.72699348154,
       5.1304018497467041, 195224.48447905964, 913.76947635051147, 0, 1010.3325805664062, 0,
       5.0447778701782227},
      {-4.915619729552418, 116688.39375442083, -8.7411311850883067, 116610.60046901472,
       -32.557539224624634, 196058.13096877403, 992.47948556949632, 0, 1010.3325805664062, 0,
       6.4097270965576172},
      {-13.902091519907117, 116737.15323048241, -18.91344846971333, 116645.87550350689,
       -724.68881416320801, 200318.45940221834, 1891.6302246958949, 0, 1010.3325805664062, 0,
       6.7924933433532715},
      {-85.849561918294057, 54989.906996349171, 1326.3478570694569, 45731.87330666183,
       243.78322768211365, 51977.507479109277, 0, 3160.1361015290022, 261.6256103515625,
       0.41999998688697815, 5.0225763320922852},
  };
  if (traces.size() != sizeof(expected) / sizeof(expected[0])) {
    return {"monophonic reference summaries remain stable", false, "trace count changed"};
  }
  for (size_t i = 0; i < traces.size(); ++i) {
    const Summary actual = summarize(traces[i]);
    const auto actualValues = summaryValues(actual);
    const auto expectedValues = summaryValues(expected[i]);
    for (size_t field = 0; field < actualValues.size(); ++field) {
      if (!closeEnough(actualValues[field], expectedValues[field])) {
        return {"monophonic reference summaries remain stable", false,
                traces[i].name + " changed at summary field " + std::to_string(field)};
      }
    }
  }
  return {"monophonic reference summaries remain stable", true, ""};
}

TestResult monophonicWaveformFingerprintsAreStable(const std::vector<Trace>& traces) {
  static const uint64_t expected[] {
      17030102344340558454ull,
      810040591452090963ull,
      15063204619675716719ull,
      2770763147626393649ull,
      9606999483199204496ull,
      2847041597128889999ull,
  };
  if (traces.size() != sizeof(expected) / sizeof(expected[0])) {
    return {"quantized monophonic waveform fingerprints remain stable", false, "trace count changed"};
  }
  for (size_t i = 0; i < traces.size(); ++i) {
    const uint64_t actual = quantizedTraceHash(traces[i]);
    if (actual != expected[i]) {
      return {"quantized monophonic waveform fingerprints remain stable", false,
              traces[i].name + " fingerprint changed from " + std::to_string(expected[i]) + " to "
                  + std::to_string(actual)};
    }
  }
  return {"quantized monophonic waveform fingerprints remain stable", true, ""};
}

TestResult measuredSyncPeakRemainsBounded() {
  const float peak = measureSyncStressPeak();
  const bool pass = std::isfinite(peak) && peak <= 15.5f && std::fabs(peak - 12.3484449f) <= 0.02f;
  return {"hard-sync stress peak matches the captured monophonic baseline", pass,
          pass ? "" : "observed peak " + std::to_string(peak) + " V; expected about 12.3484 V and at most 15.5 V"};
}

TestResult everyInputCanSetOutputPolyphony() {
  for (int inputId = 0; inputId < Undertow::INPUTS_LEN; ++inputId) {
    Undertow module;
    connectAllOutputs(module);
    setInputChannels(module, inputId, inputId + 2);
    Module::ProcessArgs args = processArgs();
    module.process(args);
    for (int outputId = 0; outputId < Undertow::OUTPUTS_LEN; ++outputId) {
      if (module.outputs[outputId].getChannels() != inputId + 2) {
        return {"every input establishes all output channel counts", false,
                "input " + std::to_string(inputId) + " did not publish " + std::to_string(inputId + 2)
                    + " channels on output " + std::to_string(outputId)};
      }
    }
  }
  Undertow unpatched;
  connectAllOutputs(unpatched);
  Module::ProcessArgs args = processArgs();
  unpatched.process(args);
  const bool mono = unpatched.outputs[Undertow::SINE_OUTPUT].getChannels() == 1
      && unpatched.outputs[Undertow::SHAPE_OUTPUT].getChannels() == 1
      && unpatched.outputs[Undertow::SUB_OUTPUT].getChannels() == 1;
  return {"every input establishes all output channel counts", mono,
          mono ? "" : "unpatched Undertow did not remain monophonic"};
}

TestResult polyVOctProducesIndependentPitches() {
  Undertow module;
  setInputChannels(module, Undertow::V_OCT_INPUT, 3);
  module.inputs[Undertow::V_OCT_INPUT].setVoltage(0.f, 0);
  module.inputs[Undertow::V_OCT_INPUT].setVoltage(1.f, 1);
  module.inputs[Undertow::V_OCT_INPUT].setVoltage(-1.f, 2);
  Module::ProcessArgs args = processArgs();
  for (int frame = 0; frame < 32; ++frame) {
    args.frame = frame;
    module.process(args);
  }
  const float p0 = module.voices[0].phase;
  const float p1 = module.voices[1].phase;
  const float p2 = module.voices[2].phase;
  const bool pass = p1 > p0 && p0 > p2 && nearlyEqual(p1, p0 * 2.f, 2e-5f)
      && nearlyEqual(p2, p0 * 0.5f, 2e-5f);
  return {"polyphonic V/OCT produces independent octave-related phases", pass,
          pass ? "" : "phases were " + std::to_string(p0) + ", " + std::to_string(p1) + ", "
              + std::to_string(p2)};
}

TestResult monoInputsBroadcastToAllVoices() {
  Undertow module;
  setInputChannels(module, Undertow::V_OCT_INPUT, 4);
  for (int channel = 0; channel < 4; ++channel) {
    module.inputs[Undertow::V_OCT_INPUT].setVoltage(0.f, channel);
  }
  connectInput(module, Undertow::EXPO_INPUT);
  connectInput(module, Undertow::LIN_FM_INPUT);
  connectInput(module, Undertow::SHAPE_CV_INPUT);
  connectInput(module, Undertow::SYNC_INPUT);
  connectInput(module, Undertow::S_GATE_INPUT);
  module.inputs[Undertow::EXPO_INPUT].setVoltage(0.125f);
  module.inputs[Undertow::LIN_FM_INPUT].setVoltage(3.5f);
  module.inputs[Undertow::SHAPE_CV_INPUT].setVoltage(6.f);
  module.inputs[Undertow::SYNC_INPUT].setVoltage(10.f);
  module.inputs[Undertow::S_GATE_INPUT].setVoltage(10.f);
  module.params[Undertow::SHAPE_PARAM].setValue(0.8f);
  module.params[Undertow::LIN_FM_PARAM].setValue(0.7f);
  Module::ProcessArgs args = processArgs();
  for (int frame = 0; frame < 64; ++frame) {
    if (frame == 1) module.inputs[Undertow::SYNC_INPUT].setVoltage(0.f);
    args.frame = frame;
    module.process(args);
  }
  for (int channel = 1; channel < 4; ++channel) {
    for (int outputId = 0; outputId < Undertow::OUTPUTS_LEN; ++outputId) {
      if (!nearlyEqual(module.outputs[outputId].getVoltage(channel), module.outputs[outputId].getVoltage(0))) {
        return {"monophonic CV, SYNC, and S-GATE broadcast identically", false,
                "channel " + std::to_string(channel) + " diverged on output " + std::to_string(outputId)};
      }
    }
  }
  return {"monophonic CV, SYNC, and S-GATE broadcast identically", true, ""};
}

TestResult morphIsPerVoiceButPreviewIsChannelZero() {
  Undertow module;
  module.params[Undertow::SHAPE_PARAM].setValue(1.f);
  setInputChannels(module, Undertow::SHAPE_CV_INPUT, 3);
  module.inputs[Undertow::SHAPE_CV_INPUT].setVoltage(2.f, 0);
  module.inputs[Undertow::SHAPE_CV_INPUT].setVoltage(5.f, 1);
  module.inputs[Undertow::SHAPE_CV_INPUT].setVoltage(8.f, 2);
  Module::ProcessArgs args = processArgs();
  for (int frame = 0; frame < 97; ++frame) {
    args.frame = frame;
    module.process(args);
  }
  const float expectedPreview = undertow_shape::shapeControlTaper(2.f / 8.f);
  const bool initial = nearlyEqual(module.displayShapeAmount.load(std::memory_order_relaxed), expectedPreview)
      && !nearlyEqual(module.outputs[Undertow::SHAPE_OUTPUT].getVoltage(0),
                      module.outputs[Undertow::SHAPE_OUTPUT].getVoltage(2), 1e-3f);
  const float channel0Before = module.outputs[Undertow::SHAPE_OUTPUT].getVoltage(0);
  module.inputs[Undertow::SHAPE_CV_INPUT].setVoltage(0.f, 1);
  module.inputs[Undertow::SHAPE_CV_INPUT].setVoltage(0.f, 2);
  // Higher-channel CV changes cannot alter the value published to the preview.
  args.frame = 97;
  module.process(args);
  const bool pass = initial
      && nearlyEqual(module.displayShapeAmount.load(std::memory_order_relaxed), expectedPreview)
      && std::isfinite(channel0Before);
  return {"Morph is per voice while preview publication remains channel zero", pass,
          pass ? "" : "per-channel Morph or channel-zero preview behavior changed"};
}

TestResult syncAndGateAreChannelIndependent() {
  Undertow module;
  setInputChannels(module, Undertow::V_OCT_INPUT, 4);
  setInputChannels(module, Undertow::SYNC_INPUT, 4);
  setInputChannels(module, Undertow::S_GATE_INPUT, 2);
  for (int channel = 0; channel < 4; ++channel) {
    module.inputs[Undertow::V_OCT_INPUT].setVoltage(0.f, channel);
    module.inputs[Undertow::SYNC_INPUT].setVoltage(0.f, channel);
  }
  module.inputs[Undertow::S_GATE_INPUT].setVoltage(10.f, 0);
  module.inputs[Undertow::S_GATE_INPUT].setVoltage(0.f, 1);
  Module::ProcessArgs args = processArgs();
  for (int frame = 0; frame < 128; ++frame) {
    args.frame = frame;
    module.process(args);
  }
  const float phaseBeforeSync = module.voices[0].phase;
  module.inputs[Undertow::SYNC_INPUT].setVoltage(10.f, 1);
  args.frame = 128;
  module.process(args);
  const float phaseIncrement = dsp::FREQ_C4 * args.sampleTime;
  const bool syncIsolated = nearlyEqual(module.voices[1].phase, phaseIncrement, 2e-5f)
      && nearlyEqual(module.voices[0].phase, phaseBeforeSync + phaseIncrement, 2e-5f)
      && nearlyEqual(module.voices[2].phase, module.voices[0].phase, 2e-5f)
      && nearlyEqual(module.outputs[Undertow::SINE_OUTPUT].getVoltage(0),
                     module.outputs[Undertow::SINE_OUTPUT].getVoltage(2), 2e-5f)
      && !nearlyEqual(module.outputs[Undertow::SINE_OUTPUT].getVoltage(0),
                      module.outputs[Undertow::SINE_OUTPUT].getVoltage(1), 1e-3f)
      && module.lights[Undertow::SYNC_LIGHT].getBrightness() > 0.99f;
  const bool gateIsolated = module.outputs[Undertow::SUB_OUTPUT].getVoltage(0) < -4.9f
      && nearlyEqual(module.outputs[Undertow::SUB_OUTPUT].getVoltage(1), 0.f)
      && nearlyEqual(module.outputs[Undertow::SUB_OUTPUT].getVoltage(2), 0.f)
      && nearlyEqual(module.outputs[Undertow::SUB_OUTPUT].getVoltage(3), 0.f)
      && module.lights[Undertow::S_GATE_LIGHT].getBrightness() > 0.99f;
  const bool pass = syncIsolated && gateIsolated;
  return {"polyphonic SYNC and shorter S-GATE remain channel independent", pass,
          pass ? "" : "SYNC isolation, missing-channel zero, or S-GATE aggregation failed"};
}

TestResult reactivatedVoicesResetWithoutResettingChannelZero() {
  Undertow module;
  setInputChannels(module, Undertow::V_OCT_INPUT, 3);
  Module::ProcessArgs args = processArgs();
  for (int frame = 0; frame < 100; ++frame) {
    args.frame = frame;
    module.process(args);
  }
  setInputChannels(module, Undertow::V_OCT_INPUT, 1);
  for (int frame = 100; frame < 140; ++frame) {
    args.frame = frame;
    module.process(args);
  }
  const float channel0BeforeExpansion = module.voices[0].phase;
  setInputChannels(module, Undertow::V_OCT_INPUT, 3);
  args.frame = 140;
  module.process(args);
  const float expectedIncrement = dsp::FREQ_C4 * args.sampleTime;
  const bool reset = nearlyEqual(module.voices[1].phase, expectedIncrement, 2e-5f)
      && nearlyEqual(module.voices[2].phase, expectedIncrement, 2e-5f);
  const bool channel0Continuous = module.voices[0].phase > channel0BeforeExpansion
      || channel0BeforeExpansion > 1.f - expectedIncrement;
  const bool pass = reset && channel0Continuous;
  return {"reactivated voices reset while channel zero runs continuously", pass,
          pass ? "" : "reactivated phases resumed stale state or channel zero reset"};
}

TestResult linFmHistoryRemainsPerVoice() {
  Undertow module;
  setInputChannels(module, Undertow::V_OCT_INPUT, 3);
  setInputChannels(module, Undertow::LIN_FM_INPUT, 3);
  module.params[Undertow::LIN_FM_PARAM].setValue(1.f);
  module.inputs[Undertow::LIN_FM_INPUT].setVoltage(6.f, 0);
  module.inputs[Undertow::LIN_FM_INPUT].setVoltage(-6.f, 1);
  module.inputs[Undertow::LIN_FM_INPUT].setVoltage(0.f, 2);
  Module::ProcessArgs args = processArgs();
  for (int frame = 0; frame < 256; ++frame) {
    args.frame = frame;
    module.process(args);
  }
  const bool pass = module.voices[0].linHpState > 0.f
      && module.voices[1].linHpState < 0.f
      && nearlyEqual(module.voices[0].linHpState, -module.voices[1].linHpState, 2e-5f)
      && nearlyEqual(module.voices[2].linHpState, 0.f)
      && !nearlyEqual(module.outputs[Undertow::SINE_OUTPUT].getVoltage(0),
                      module.outputs[Undertow::SINE_OUTPUT].getVoltage(1), 1e-3f);
  return {"LIN FM filter history remains independent per voice", pass,
          pass ? "" : "opposite LIN FM channels shared or lost filter history"};
}

TestResult sixteenChannelStressRemainsFiniteAndBounded() {
  Undertow module;
  for (int inputId = 0; inputId < Undertow::INPUTS_LEN; ++inputId) {
    setInputChannels(module, inputId, PORT_MAX_CHANNELS);
  }
  module.params[Undertow::LIN_FM_PARAM].setValue(1.f);
  module.params[Undertow::SHAPE_PARAM].setValue(1.f);
  Module::ProcessArgs args = processArgs(96000.f);
  for (int frame = 0; frame < 4096; ++frame) {
    for (int channel = 0; channel < PORT_MAX_CHANNELS; ++channel) {
      const float ramp = float(((frame * 17 + channel * 29) % 257) - 128) / 128.f;
      module.inputs[Undertow::V_OCT_INPUT].setVoltage(float((channel % 7) - 3) * 0.25f, channel);
      module.inputs[Undertow::EXPO_INPUT].setVoltage(ramp * 0.2f, channel);
      module.inputs[Undertow::LIN_FM_INPUT].setVoltage(ramp * 10.f, channel);
      module.inputs[Undertow::SHAPE_CV_INPUT].setVoltage(float(channel) * (8.f / 15.f), channel);
      module.inputs[Undertow::SYNC_INPUT].setVoltage(((frame + channel * 11) % 173) == 0 ? 10.f : 0.f, channel);
      module.inputs[Undertow::S_GATE_INPUT].setVoltage(((frame / 97 + channel) & 1) ? 10.f : 0.f, channel);
    }
    args.frame = frame;
    module.process(args);
    for (int channel = 0; channel < PORT_MAX_CHANNELS; ++channel) {
      const float sine = module.outputs[Undertow::SINE_OUTPUT].getVoltage(channel);
      const float shape = module.outputs[Undertow::SHAPE_OUTPUT].getVoltage(channel);
      const float sub = module.outputs[Undertow::SUB_OUTPUT].getVoltage(channel);
      if (!std::isfinite(sine) || !std::isfinite(shape) || !std::isfinite(sub)
          || std::fabs(sine) > 15.5f || std::fabs(shape) > 5.0001f || std::fabs(sub) > 5.0001f) {
        return {"sixteen-channel randomized stress remains finite and bounded", false,
                "invalid output at frame " + std::to_string(frame) + ", channel " + std::to_string(channel)};
      }
    }
  }
  return {"sixteen-channel randomized stress remains finite and bounded", true, ""};
}

} // namespace

int main(int argc, char** argv) {
  const std::vector<Trace> traces {
      freeRunningTrace(),
      modulationTrace(),
      syncTrace(44100.f, false),
      syncTrace(48000.f, true),
      syncTrace(96000.f, true),
      subGateAndModesTrace(),
  };

  if (argc > 1 && std::string(argv[1]) == "--capture") {
    for (const Trace& trace : traces) printCapture(trace);
    std::cout << "SYNC_STRESS_PEAK " << std::setprecision(9) << measureSyncStressPeak() << '\n';
    return 0;
  }

  const std::vector<TestResult> results {
      releasedSchemaIsStable(),
      tracesAreHealthy(traces),
      monophonicReferenceIsStable(traces),
      monophonicWaveformFingerprintsAreStable(traces),
      measuredSyncPeakRemainsBounded(),
      everyInputCanSetOutputPolyphony(),
      polyVOctProducesIndependentPitches(),
      monoInputsBroadcastToAllVoices(),
      morphIsPerVoiceButPreviewIsChannelZero(),
      syncAndGateAreChannelIndependent(),
      reactivatedVoicesResetWithoutResettingChannelZero(),
      linFmHistoryRemainsPerVoice(),
      sixteenChannelStressRemainsFiniteAndBounded(),
  };
  std::cout << "Undertow Monophonic Reference\n";
  int failures = 0;
  for (const TestResult& result : results) {
    std::cout << (result.pass ? "[PASS] " : "[FAIL] ") << result.name;
    if (!result.detail.empty()) std::cout << ": " << result.detail;
    std::cout << '\n';
    if (!result.pass) ++failures;
  }
  return failures == 0 ? 0 : 1;
}
