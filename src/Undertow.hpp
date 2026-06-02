#pragma once

#include "plugin.hpp"
#include <atomic>
#include <cmath>

constexpr float kUndertowMinHz = 10.f;
constexpr float kUndertowMaxHz = 10000.f;

inline float undertowBaseFrequencyFromKnob(float knobNorm) {
  return kUndertowMinHz * std::pow(kUndertowMaxHz / kUndertowMinHz, clamp(knobNorm, 0.f, 1.f));
}

inline float undertowKnobValueForFrequency(float hz) {
  hz = clamp(hz, kUndertowMinHz, kUndertowMaxHz);
  return std::log(hz / kUndertowMinHz) / std::log(kUndertowMaxHz / kUndertowMinHz);
}

struct UndertowFreqQuantity final : ParamQuantity {
  float getDisplayValue() override;
  void setDisplayValue(float displayValue) override;
  std::string getDisplayValueString() override;
};

struct Undertow final : Module {
  enum ParamId {
    COARSE_PARAM,
    FINE_PARAM,
    LIN_FM_PARAM,
    SHAPE_PARAM,
    COARSE_STEP_MODE_PARAM,
    PARAMS_LEN
  };

  enum InputId {
    V_OCT_INPUT,
    EXPO_INPUT,
    LIN_FM_INPUT,
    SHAPE_CV_INPUT,
    SYNC_INPUT,
    S_GATE_INPUT,
    INPUTS_LEN
  };

  enum OutputId {
    SINE_OUTPUT,
    SHAPE_OUTPUT,
    SUB_OUTPUT,
    OUTPUTS_LEN
  };

  enum LightId {
    SYNC_LIGHT,
    S_GATE_LIGHT,
    COARSE_STEP_MODE_LIGHT,
    LIGHTS_LEN
  };

  struct VoiceState {
    float phase = 0.f;
    bool subFlip = false;
    bool subGateHighLast = false;
    float linHpState = 0.f;
    dsp::SchmittTrigger syncTrig;
    dsp::SchmittTrigger sGateTrig;
    dsp::MinBlepGenerator<16, 16> sineBlep;
    dsp::MinBlepGenerator<16, 16> shapeBlep;
    dsp::MinBlepGenerator<16, 16> subBlep;
  };

  VoiceState voice;
  std::atomic<bool> shapeEntryAsymmetry {false};
  std::atomic<bool> shapeEntryAsymmetryOnRight {false};
  std::atomic<float> shapeEdgeHardness {0.5f};
  std::atomic<float> displayFrequencyHz {0.f};
  std::atomic<float> displayShapeAmount {0.f};

  Undertow();
  float getShapeAmount();
  void process(const ProcessArgs& args) override;
  json_t* dataToJson() override;
  void dataFromJson(json_t* root) override;
};

extern Model* modelUndertow;
