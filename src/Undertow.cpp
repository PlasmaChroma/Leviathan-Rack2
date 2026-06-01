#include "Undertow.hpp"
#include "UndertowShape.hpp"

namespace {

static constexpr float kLinHpCoeff = 0.9993f;
static constexpr float kLinFmScale = 0.10f;
static constexpr float kMinFreqHz = 8.f;
static constexpr float kMaxFreqHz = 20000.f;

inline float acCoupledLinFm(float x, Undertow::VoiceState* voice) {
  // Minimal one-pole HP for LIN FM DC rejection.
  float y = x - voice->linHpState;
  voice->linHpState = x - kLinHpCoeff * y;
  return y;
}

inline void insertBlepStep(dsp::MinBlepGenerator<16, 16>* blep, float step, float fraction01) {
  if (!blep || std::fabs(step) < 1e-9f) {
    return;
  }
  float f = clamp(fraction01, 1e-6f, 1.f);
  // Rack MinBLEP expects discontinuity position in [-1, 0] samples from current sample.
  blep->insertDiscontinuity(f - 1.f, step);
}

inline void clearSubBlep(Undertow::VoiceState* voice) {
  for (int i = 0; i < 32; ++i) {
    voice->subBlep.process();
  }
}

} // namespace

Undertow::Undertow() {
  config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

  configParam<UndertowFreqQuantity>(COARSE_PARAM, 0.f, 1.f, undertowKnobValueForFrequency(261.63f), "Frequency");
  configParam(FINE_PARAM, -100.f, 100.f, 0.f, "Fine tune", " cents");
  configParam(LIN_FM_PARAM, 0.f, 1.f, 0.f, "Linear FM");
  configParam(SHAPE_PARAM, 0.f, 1.f, 0.f, "Morph", " %", 0.f, 100.f);
  configParam(COARSE_STEP_MODE_PARAM, 0.f, 1.f, 0.f, "Octave stepped");

  configInput(V_OCT_INPUT, "V/Oct");
  configInput(EXPO_INPUT, "Expo FM");
  configInput(LIN_FM_INPUT, "Linear FM");
  configInput(SHAPE_CV_INPUT, "Morph CV");
  configInput(SYNC_INPUT, "Sync");
  configInput(S_GATE_INPUT, "Sub gate");

  configOutput(SINE_OUTPUT, "Sine");
  configOutput(SHAPE_OUTPUT, "Morph");
  configOutput(SUB_OUTPUT, "Sub");
}

float Undertow::getShapeAmount() {
  const float shapeKnob = params[SHAPE_PARAM].getValue();
  if (inputs[SHAPE_CV_INPUT].isConnected()) {
    return undertow_shape::shapeControlTaper(shapeKnob * (inputs[SHAPE_CV_INPUT].getVoltage() / 8.f));
  }
  return undertow_shape::shapeControlTaper(shapeKnob);
}

void Undertow::process(const ProcessArgs& args) {
  const bool coarseTuneSteppedNow = params[COARSE_STEP_MODE_PARAM].getValue() > 0.5f;
  float coarseHz = undertowBaseFrequencyFromKnob(params[COARSE_PARAM].getValue());
  if (coarseTuneSteppedNow) {
    const float coarsePitchV = std::log2(std::max(coarseHz, 1e-6f) / dsp::FREQ_C4);
    const float snappedPitchV = std::round(coarsePitchV);
    coarseHz = dsp::FREQ_C4 * dsp::approxExp2_taylor5(snappedPitchV);
  }
  const float fineOctaves = params[FINE_PARAM].getValue() / 1200.f;
  const float vOct = inputs[V_OCT_INPUT].isConnected() ? inputs[V_OCT_INPUT].getVoltage() : 0.f;
  const float expo = inputs[EXPO_INPUT].isConnected() ? inputs[EXPO_INPUT].getVoltage() : 0.f;
  const float baseFreq = coarseHz * dsp::approxExp2_taylor5(fineOctaves + vOct + expo);
  displayFrequencyHz.store(clamp(baseFreq, kMinFreqHz, kMaxFreqHz), std::memory_order_relaxed);
  const float linIn = inputs[LIN_FM_INPUT].isConnected() ? inputs[LIN_FM_INPUT].getVoltage() : 0.f;
  const float lin = acCoupledLinFm(linIn, &voice);
  const float linAmt = params[LIN_FM_PARAM].getValue();
  const float freq = clamp(baseFreq + baseFreq * lin * linAmt * kLinFmScale, kMinFreqHz, kMaxFreqHz);
  const float phaseInc = freq * args.sampleTime;
  const float shape = getShapeAmount();

  // Precompute "old state" signals for MinBLEP step correction when events force discontinuities.
  const float phaseBeforeEvents = voice.phase;
  const float triBeforeEvents = 4.f * std::fabs(phaseBeforeEvents - 0.5f) - 1.f;
  const float sineBeforeEvents = undertow_shape::triToSine(triBeforeEvents);
  const float shapedBeforeEvents = undertow_shape::thresholdFold(phaseBeforeEvents, shape, shapeEntryAsymmetry,
                                                                 shapeHardEdges, shapeEntryAsymmetryOnRight);

  bool syncRising = voice.syncTrig.process(inputs[SYNC_INPUT].isConnected() ? inputs[SYNC_INPUT].getVoltage() : 0.f);
  float syncDiscontinuityFrac = 0.5f;
  if (syncRising) {
    if (phaseInc > 1e-9f) {
      syncDiscontinuityFrac = clamp((1.f - phaseBeforeEvents) / phaseInc, 1e-6f, 1.f);
    }
    voice.phase = 0.f;
  }
  const float phaseBeforeAdvance = voice.phase;
  voice.phase += phaseInc;
  bool wrapped = false;
  float wrapDiscontinuityFrac = 1.f;
  if (voice.phase >= 1.f) {
    wrapped = true;
    if (phaseInc > 1e-9f) {
      wrapDiscontinuityFrac = clamp((1.f - phaseBeforeAdvance) / phaseInc, 1e-6f, 1.f);
    }
    voice.phase -= std::floor(voice.phase);
  }

  const float tri = 4.f * std::fabs(voice.phase - 0.5f) - 1.f;
  const float sine = undertow_shape::triToSine(tri);
  const float shaped =
      undertow_shape::thresholdFold(voice.phase, shape, shapeEntryAsymmetry, shapeHardEdges, shapeEntryAsymmetryOnRight);

  if (syncRising) {
    const float sineStep = sine - sineBeforeEvents;
    const float shapeStep = shaped - shapedBeforeEvents;
    insertBlepStep(&voice.sineBlep, sineStep * 5.f, syncDiscontinuityFrac);
    insertBlepStep(&voice.shapeBlep, shapeStep * 5.f, syncDiscontinuityFrac);
  }

  bool sGatePatched = inputs[S_GATE_INPUT].isConnected();
  float sGateV = sGatePatched ? inputs[S_GATE_INPUT].getVoltage() : 10.f;
  bool sGateHigh = sGateV >= 1.f;
  bool sGateRising = voice.sGateTrig.process(sGateV);
  bool sGateFalling = voice.subGateHighLast && !sGateHigh;
  if (sGateRising || sGateFalling) {
    clearSubBlep(&voice);
  }
  if (sGateRising) {
    voice.subFlip = false;
  }
  voice.subGateHighLast = sGateHigh;

  float subRaw = voice.subFlip ? 1.f : -1.f;
  if (wrapped && (!sGatePatched || sGateHigh)) {
    const float subOld = subRaw;
    voice.subFlip = !voice.subFlip;
    subRaw = voice.subFlip ? 1.f : -1.f;
    insertBlepStep(&voice.subBlep, (subRaw - subOld) * 4.f, wrapDiscontinuityFrac);
  }

  outputs[SINE_OUTPUT].setChannels(1);
  outputs[SHAPE_OUTPUT].setChannels(1);
  outputs[SUB_OUTPUT].setChannels(1);
  outputs[SINE_OUTPUT].setVoltage(5.f * sine + voice.sineBlep.process());
  outputs[SHAPE_OUTPUT].setVoltage(clamp(5.f * shaped + voice.shapeBlep.process(), -5.f, 5.f));
  // The sub square is band-limited with MinBLEP, so it will not scope as an
  // ideal digital square.  Keep the steady state inside rails and let the clamp
  // catch residual correction energy instead of using clipping as the tone.
  const float subOut = (!sGatePatched || sGateHigh) ? clamp(4.f * subRaw + voice.subBlep.process(), -5.f, 5.f)
                                                    : (voice.subBlep.process(), 0.f);
  outputs[SUB_OUTPUT].setVoltage(subOut);

  lights[SYNC_LIGHT].setBrightnessSmooth(syncRising ? 1.f : 0.f, args.sampleTime * 8.f);
  lights[S_GATE_LIGHT].setBrightnessSmooth((sGatePatched && sGateHigh) ? 1.f : 0.f, args.sampleTime * 8.f);
  lights[COARSE_STEP_MODE_LIGHT].setBrightness(coarseTuneSteppedNow ? 0.5f : 0.f);
}

json_t* Undertow::dataToJson() {
  json_t* root = json_object();
  json_object_set_new(root, "shapeEntryAsymmetry", json_boolean(shapeEntryAsymmetry));
  json_object_set_new(root, "shapeEntryAsymmetryOnRight", json_boolean(shapeEntryAsymmetryOnRight));
  json_object_set_new(root, "shapeHardEdges", json_boolean(shapeHardEdges));
  return root;
}

void Undertow::dataFromJson(json_t* root) {
  if (!root) {
    return;
  }
  if (json_t* entryAsymmetryJ = json_object_get(root, "shapeEntryAsymmetry")) {
    shapeEntryAsymmetry = json_boolean_value(entryAsymmetryJ);
  }
  if (json_t* entryAsymmetrySideJ = json_object_get(root, "shapeEntryAsymmetryOnRight")) {
    shapeEntryAsymmetryOnRight = json_boolean_value(entryAsymmetrySideJ);
  }
  if (json_t* hardEdgesJ = json_object_get(root, "shapeHardEdges")) {
    shapeHardEdges = json_boolean_value(hardEdgesJ);
  }
}

float UndertowFreqQuantity::getDisplayValue() {
  return undertowBaseFrequencyFromKnob(getValue());
}

void UndertowFreqQuantity::setDisplayValue(float displayValue) {
  setImmediateValue(undertowKnobValueForFrequency(displayValue));
}

std::string UndertowFreqQuantity::getDisplayValueString() {
  const float hz = getDisplayValue();
  if (hz >= 1000.f) {
    return string::f("%.2f kHz", hz / 1000.f);
  }
  if (hz < 100.f) {
    return string::f("%.2f Hz", hz);
  }
  return string::f("%.1f Hz", hz);
}
