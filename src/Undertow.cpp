#include "Undertow.hpp"

namespace {

static constexpr float kLinHpCoeff = 0.9993f;
static constexpr float kShapeDcCoeff = 0.9993f;
static constexpr float kLinFmScale = 0.10f;
static constexpr float kMinFreqHz = 8.f;
static constexpr float kMaxFreqHz = 20000.f;

inline float triToSine(float x) {
  const float x2 = x * x;
  return x * (1.5707963f - 0.6459641f * x2 + 0.0796926f * x2 * x2);
}

inline float dcBlockShape(float x, Undertow::VoiceState* voice) {
  float y = x - voice->shapeDcX1 + kShapeDcCoeff * voice->shapeDcY1;
  voice->shapeDcX1 = x;
  voice->shapeDcY1 = y;
  return y;
}

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

} // namespace

Undertow::Undertow() {
  config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

  configParam<UndertowFreqQuantity>(COARSE_PARAM, 0.f, 1.f, undertowKnobValueForFrequency(261.63f), "Frequency");
  configParam(FINE_PARAM, -100.f, 100.f, 0.f, "Fine tune", " cents");
  configParam(LIN_FM_PARAM, 0.f, 1.f, 0.f, "Linear FM");
  configParam(SHAPE_PARAM, 0.f, 1.f, 0.f, "Shape");

  configInput(V_OCT_INPUT, "V/Oct");
  configInput(EXPO_INPUT, "Expo FM");
  configInput(LIN_FM_INPUT, "Linear FM");
  configInput(SHAPE_CV_INPUT, "Shape CV");
  configInput(SYNC_INPUT, "Sync");
  configInput(S_GATE_INPUT, "Sub gate");

  configOutput(SINE_OUTPUT, "Sine");
  configOutput(SHAPE_OUTPUT, "Shape");
  configOutput(SUB_OUTPUT, "Sub");
}

void Undertow::process(const ProcessArgs& args) {
  float coarseHz = undertowBaseFrequencyFromKnob(params[COARSE_PARAM].getValue());
  if (coarseTuneStepped) {
    const float coarsePitchV = std::log2(std::max(coarseHz, 1e-6f) / dsp::FREQ_C4);
    const float snappedPitchV = std::round(coarsePitchV);
    coarseHz = dsp::FREQ_C4 * dsp::approxExp2_taylor5(snappedPitchV);
  }
  const float fineOctaves = params[FINE_PARAM].getValue() / 1200.f;
  const float vOct = inputs[V_OCT_INPUT].isConnected() ? inputs[V_OCT_INPUT].getVoltage() : 0.f;
  const float expo = inputs[EXPO_INPUT].isConnected() ? inputs[EXPO_INPUT].getVoltage() : 0.f;
  const float baseFreq = coarseHz * dsp::approxExp2_taylor5(fineOctaves + vOct + expo);
  const float linIn = inputs[LIN_FM_INPUT].isConnected() ? inputs[LIN_FM_INPUT].getVoltage() : 0.f;
  const float lin = acCoupledLinFm(linIn, &voice);
  const float linAmt = params[LIN_FM_PARAM].getValue();
  const float freq = clamp(baseFreq + baseFreq * lin * linAmt * kLinFmScale, kMinFreqHz, kMaxFreqHz);
  const float phaseInc = freq * args.sampleTime;

  // Precompute "old state" signals for MinBLEP step correction when events force discontinuities.
  const float phaseBeforeEvents = voice.phase;
  const float triBeforeEvents = 4.f * std::fabs(phaseBeforeEvents - 0.5f) - 1.f;
  const float sineBeforeEvents = triToSine(triBeforeEvents);
  float shapeBeforeEvents = clamp(params[SHAPE_PARAM].getValue(), 0.f, 1.f);
  if (inputs[SHAPE_CV_INPUT].isConnected()) {
    const float cv = clamp(inputs[SHAPE_CV_INPUT].getVoltage() / 8.f, 0.f, 1.f);
    shapeBeforeEvents = clamp(shapeBeforeEvents * cv, 0.f, 1.f);
  }
  const float shapeBeforeCurve = shapeBeforeEvents * shapeBeforeEvents;
  const float asymBefore = triBeforeEvents + 0.22f * shapeBeforeEvents * (1.f - triBeforeEvents * triBeforeEvents);
  const float cubicBefore = asymBefore * asymBefore * asymBefore;
  const float kinkBefore = asymBefore - 0.35f * shapeBeforeEvents * clamp(cubicBefore, -1.f, 1.f);
  const float creasePhaseBefore = 1.f - 4.f * std::fabs(phaseBeforeEvents - 0.5f) * (1.f - std::fabs(phaseBeforeEvents - 0.5f));
  const float creaseBefore = clamp(creasePhaseBefore, -1.f, 1.f) * shapeBeforeCurve * 0.16f;
  const float altBefore = clamp(kinkBefore + creaseBefore, -1.2f, 1.2f);
  const float shapedBeforeEvents = crossfade(sineBeforeEvents, altBefore, shapeBeforeCurve);

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
  const float sine = triToSine(tri);

  float shape;
  if (inputs[SHAPE_CV_INPUT].isConnected()) {
    const float cv = clamp(inputs[SHAPE_CV_INPUT].getVoltage() / 8.f, 0.f, 1.f);
    shape = clamp(params[SHAPE_PARAM].getValue() * cv, 0.f, 1.f);
  } else {
    shape = clamp(params[SHAPE_PARAM].getValue(), 0.f, 1.f);
  }
  const float shapeCurve = shape * shape;
  const float asym = tri + 0.22f * shape * (1.f - tri * tri);
  const float cubic = asym * asym * asym;
  const float kink = asym - 0.35f * shape * clamp(cubic, -1.f, 1.f);
  const float creasePhase = 1.f - 4.f * std::fabs(voice.phase - 0.5f) * (1.f - std::fabs(voice.phase - 0.5f));
  const float crease = clamp(creasePhase, -1.f, 1.f) * shapeCurve * 0.16f;
  const float alt = clamp(kink + crease, -1.2f, 1.2f);
  const float shapedRaw = crossfade(sine, alt, shapeCurve);
  const float shaped = dcBlockShape(shapedRaw, &voice);

  if (syncRising) {
    const float sineStep = sine - sineBeforeEvents;
    const float shapeStep = shapedRaw - shapedBeforeEvents;
    insertBlepStep(&voice.sineBlep, sineStep * 5.f, syncDiscontinuityFrac);
    insertBlepStep(&voice.shapeBlep, shapeStep * 5.f, syncDiscontinuityFrac);
  }

  float subRaw = voice.subFlip ? 1.f : -1.f;
  if (wrapped) {
    const float subOld = subRaw;
    voice.subFlip = !voice.subFlip;
    subRaw = voice.subFlip ? 1.f : -1.f;
    insertBlepStep(&voice.subBlep, (subRaw - subOld) * 6.f, wrapDiscontinuityFrac);
  }

  bool sGatePatched = inputs[S_GATE_INPUT].isConnected();
  float sGateV = sGatePatched ? inputs[S_GATE_INPUT].getVoltage() : 10.f;
  bool sGateHigh = sGateV >= 1.f;
  if (voice.sGateTrig.process(sGateV)) {
    float subOld = subRaw;
    voice.subFlip = false;
    subRaw = voice.subFlip ? 1.f : -1.f;
    insertBlepStep(&voice.subBlep, (subRaw - subOld) * 6.f, 0.5f);
  }
  float sub = (!sGatePatched || sGateHigh) ? subRaw : 0.f;

  outputs[SINE_OUTPUT].setChannels(1);
  outputs[SHAPE_OUTPUT].setChannels(1);
  outputs[SUB_OUTPUT].setChannels(1);
  outputs[SINE_OUTPUT].setVoltage(5.f * sine + voice.sineBlep.process());
  outputs[SHAPE_OUTPUT].setVoltage(5.f * shaped + voice.shapeBlep.process());
  outputs[SUB_OUTPUT].setVoltage(6.f * sub + voice.subBlep.process());

  lights[SYNC_LIGHT].setBrightnessSmooth(syncRising ? 1.f : 0.f, args.sampleTime * 8.f);
  lights[S_GATE_LIGHT].setBrightnessSmooth((!sGatePatched || sGateHigh) ? 1.f : 0.f, args.sampleTime * 8.f);
}

json_t* Undertow::dataToJson() {
  json_t* root = json_object();
  json_object_set_new(root, "coarseTuneStepped", json_boolean(coarseTuneStepped));
  return root;
}

void Undertow::dataFromJson(json_t* root) {
  if (!root) {
    return;
  }
  if (json_t* steppedJ = json_object_get(root, "coarseTuneStepped")) {
    coarseTuneStepped = json_boolean_value(steppedJ);
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
