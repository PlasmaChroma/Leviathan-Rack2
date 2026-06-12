#include "Undertow.hpp"
#include "UndertowShape.hpp"
#include "WavePreviewTracer.hpp"

namespace {

static constexpr float kLinHpCutoffHz = 4.9f;
static constexpr float kLinFmScale = 0.10f;
static constexpr float kLinFmDriveThreshold = 0.80f;
static constexpr float kLinFmMaxDrive = 4.0f;
static constexpr float kSubLevelVolts = 5.f;
static constexpr float kMinFreqHz = 8.f;
static constexpr float kMaxFreqHz = 20000.f;
static constexpr float kAnalogCharacterEnvAttackSec = 0.002f;
static constexpr float kAnalogCharacterEnvReleaseSec = 0.050f;
static constexpr float kAnalogCharacterBaseDrive = 1.02f;
static constexpr float kAnalogCharacterEnvDriveDepth = 0.18f;
static constexpr float kAnalogCharacterDriveSkew = 0.02f;

inline float acCoupledLinFm(float x, Undertow::VoiceState* voice, float sampleTime) {
  // Minimal one-pole HP for LIN FM DC rejection.
  const float hpCoeff = clamp(1.f - 2.f * float(M_PI) * kLinHpCutoffHz * sampleTime, 0.f, 1.f);
  float y = x - voice->linHpState;
  voice->linHpState = x - hpCoeff * y;
  return y;
}

// Cheap asymmetric output soft-clip. This keeps the default analog character
// subtle and gives the SINE/SHAPE outputs a little more hardware-style bend.
inline float fastAtanApprox(float x) {
  const float ax = std::fabs(x);
  if (ax <= 1.f) {
    return x * (0.78539816339f + 0.273f * (1.f - ax));
  }
  const float inv = 1.f / ax;
  const float t = inv * (0.78539816339f + 0.273f * (1.f - inv));
  return (x >= 0.f) ? (1.57079632679f - t) : (-1.57079632679f + t);
}

inline float drivenLinFm(float lin, float amount) {
  // Normalize the documented 10V LIN FM range before applying the upper-range
  // bus drive. This keeps the 80% transition continuous instead of clipping raw
  // Rack volts down to a much smaller post-drive value.
  const float bus = lin * amount * kLinFmScale;
  const float driveNorm = clamp((amount - kLinFmDriveThreshold) / (1.f - kLinFmDriveThreshold), 0.f, 1.f);
  if (driveNorm <= 0.f || std::fabs(bus) < 1e-9f) {
    return bus;
  }
  const float drive = 1.f + driveNorm * (kLinFmMaxDrive - 1.f);
  const float norm = std::max(fastAtanApprox(drive), 1e-6f);
  return fastAtanApprox(bus * drive) / norm;
}

inline float onePoleCoeff(float sampleTime, float tauSeconds) {
  return clamp(sampleTime / (tauSeconds + sampleTime), 0.f, 1.f);
}

inline float updateAnalogCharacterEnv(float x, Undertow::VoiceState* voice, float sampleTime) {
  const float target = std::fabs(x);
  const bool rising = target > voice->analogCharacterEnv;
  const float coeff =
      rising ? onePoleCoeff(sampleTime, kAnalogCharacterEnvAttackSec) : onePoleCoeff(sampleTime, kAnalogCharacterEnvReleaseSec);
  voice->analogCharacterEnv += (target - voice->analogCharacterEnv) * coeff;
  return voice->analogCharacterEnv;
}

inline float analogCharacter(float x, float env, float baseDrive, float driveSkew) {
  const float drive = baseDrive + kAnalogCharacterEnvDriveDepth * clamp(env, 0.f, 1.f);
  const float signDrive = x >= 0.f ? (drive + driveSkew) : (drive - driveSkew);
  const float norm = std::max(fastAtanApprox(signDrive), 1e-6f);
  return fastAtanApprox(x * signDrive) / norm;
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
  configParam(LIN_FM_PARAM, 0.f, 1.f, 0.f, "Linear FM", " %", 0.f, 100.f);
  configParam(SHAPE_PARAM, 0.f, 1.f, 0.f, "Morph", " %", 0.f, 100.f);
  configParam(COARSE_STEP_MODE_PARAM, 0.f, 1.f, 0.f, "Octave stepped");
  configParam(EDGE_HARDNESS_PARAM, 0.f, 1.f, 0.5f, "Morph edge hardness", " %", 0.f, 100.f);

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

Undertow::~Undertow() {
  teardownTimer.begin(id);
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
  const float lin = acCoupledLinFm(linIn, &voice, args.sampleTime);
  const float linAmt = params[LIN_FM_PARAM].getValue();
  const float linBus = drivenLinFm(lin, linAmt);
  const float freq = clamp(baseFreq + baseFreq * linBus, kMinFreqHz, kMaxFreqHz);
  const float phaseInc = freq * args.sampleTime;
  const float shape = getShapeAmount();
  displayShapeAmount.store(shape, std::memory_order_relaxed);
  const bool entryAsymmetry = shapeEntryAsymmetry.load(std::memory_order_relaxed);
  const bool entryAsymmetryOnRight = shapeEntryAsymmetryOnRight.load(std::memory_order_relaxed);
  const float edgeHardness = params[EDGE_HARDNESS_PARAM].getValue();
  const bool analogCharacterEnabledNow = analogCharacterEnabled.load(std::memory_order_relaxed);

  // Precompute "old state" signals for MinBLEP step correction when events force discontinuities.
  const float phaseBeforeEvents = voice.phase;
  const float triBeforeEvents = 4.f * std::fabs(phaseBeforeEvents - 0.5f) - 1.f;
  const float sineBeforeEvents = undertow_shape::triToSine(triBeforeEvents);
  const float shapedBeforeEvents =
      undertow_shape::thresholdFold(phaseBeforeEvents, shape, entryAsymmetry, edgeHardness, entryAsymmetryOnRight);

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
      undertow_shape::thresholdFold(voice.phase, shape, entryAsymmetry, edgeHardness, entryAsymmetryOnRight);
  const float analogCharacterEnv =
      updateAnalogCharacterEnv(0.5f * (std::fabs(sine) + std::fabs(shaped)), &voice, args.sampleTime);

  if (syncRising) {
    const float sineStep = analogCharacterEnabledNow
                               ? analogCharacter(sine, analogCharacterEnv, kAnalogCharacterBaseDrive,
                                                 kAnalogCharacterDriveSkew) -
                                     analogCharacter(sineBeforeEvents, analogCharacterEnv, kAnalogCharacterBaseDrive,
                                                     kAnalogCharacterDriveSkew)
                               : sine - sineBeforeEvents;
    const float shapeStep = analogCharacterEnabledNow
                                ? analogCharacter(shaped, analogCharacterEnv, kAnalogCharacterBaseDrive,
                                                  kAnalogCharacterDriveSkew) -
                                      analogCharacter(shapedBeforeEvents, analogCharacterEnv,
                                                      kAnalogCharacterBaseDrive, kAnalogCharacterDriveSkew)
                                : shaped - shapedBeforeEvents;
    insertBlepStep(&voice.sineBlep, sineStep * 5.f, syncDiscontinuityFrac);
    insertBlepStep(&voice.shapeBlep, shapeStep * 5.f, syncDiscontinuityFrac);
  }

  bool sGatePatched = inputs[S_GATE_INPUT].isConnected();
  float sGateV = sGatePatched ? inputs[S_GATE_INPUT].getVoltage() : 10.f;
  bool sGateHigh = sGateV >= 1.f;
  bool sGateRising = voice.sGateTrig.process(sGateV);
  bool sGateFalling = voice.subGateHighLast && !sGateHigh;
  const float subBeforeGate =
      (voice.subGateHighLast || !sGatePatched) ? (voice.subFlip ? kSubLevelVolts : -kSubLevelVolts) : 0.f;
  if (sGateRising) {
    voice.subFlip = false;
  }

  float subRaw = voice.subFlip ? 1.f : -1.f;
  float subBase = (sGatePatched && !sGateHigh) ? 0.f : kSubLevelVolts * subRaw;
  if (sGateRising || sGateFalling) {
    insertBlepStep(&voice.subBlep, subBase - subBeforeGate, 0.5f);
  }
  voice.subGateHighLast = sGateHigh;

  if (wrapped && (!sGatePatched || sGateHigh)) {
    const float subOldBase = subBase;
    voice.subFlip = !voice.subFlip;
    subRaw = voice.subFlip ? 1.f : -1.f;
    subBase = kSubLevelVolts * subRaw;
    insertBlepStep(&voice.subBlep, subBase - subOldBase, wrapDiscontinuityFrac);
  }

  outputs[SINE_OUTPUT].setChannels(1);
  outputs[SHAPE_OUTPUT].setChannels(1);
  outputs[SUB_OUTPUT].setChannels(1);
  const float sineOut = analogCharacterEnabledNow
                            ? analogCharacter(sine, analogCharacterEnv, kAnalogCharacterBaseDrive,
                                              kAnalogCharacterDriveSkew)
                            : sine;
  const float shapeOut = analogCharacterEnabledNow
                             ? analogCharacter(shaped, analogCharacterEnv, kAnalogCharacterBaseDrive,
                                               kAnalogCharacterDriveSkew)
                             : shaped;
  outputs[SINE_OUTPUT].setVoltage(5.f * sineOut + voice.sineBlep.process());
  outputs[SHAPE_OUTPUT].setVoltage(clamp(5.f * shapeOut + voice.shapeBlep.process(), -5.f, 5.f));
  // The sub square and S-Gate edges are band-limited with MinBLEP, so the
  // correction may briefly move around the steady +/-5V rails.
  const float subOut = clamp(subBase + voice.subBlep.process(), -5.f, 5.f);
  outputs[SUB_OUTPUT].setVoltage(subOut);

  lights[SYNC_LIGHT].setBrightnessSmooth(syncRising ? 1.f : 0.f, args.sampleTime * 8.f);
  lights[S_GATE_LIGHT].setBrightnessSmooth((sGatePatched && sGateHigh) ? 1.f : 0.f, args.sampleTime * 8.f);
  lights[COARSE_STEP_MODE_LIGHT].setBrightness(coarseTuneSteppedNow ? 0.5f : 0.f);
}

json_t* Undertow::dataToJson() {
  json_t* root = json_object();
  json_object_set_new(root, "shapeEntryAsymmetry", json_boolean(shapeEntryAsymmetry.load(std::memory_order_relaxed)));
  json_object_set_new(root, "shapeEntryAsymmetryOnRight",
                      json_boolean(shapeEntryAsymmetryOnRight.load(std::memory_order_relaxed)));
  json_object_set_new(root, "analogCharacterEnabled",
                      json_boolean(analogCharacterEnabled.load(std::memory_order_relaxed)));
  json_object_set_new(root, "previewTracerEnabled",
                      json_boolean(previewTracerEnabled.load(std::memory_order_relaxed)));
  json_object_set_new(root, "previewTracerCacheMode",
                      json_integer(previewTracerCacheMode.load(std::memory_order_relaxed)));
  json_object_set_new(root, "shapeEdgeHardness", json_real(params[EDGE_HARDNESS_PARAM].getValue()));
  return root;
}

void Undertow::dataFromJson(json_t* root) {
  if (!root) {
    return;
  }
  if (json_t* entryAsymmetryJ = json_object_get(root, "shapeEntryAsymmetry")) {
    shapeEntryAsymmetry.store(json_boolean_value(entryAsymmetryJ), std::memory_order_relaxed);
  }
  if (json_t* entryAsymmetrySideJ = json_object_get(root, "shapeEntryAsymmetryOnRight")) {
    shapeEntryAsymmetryOnRight.store(json_boolean_value(entryAsymmetrySideJ), std::memory_order_relaxed);
  }
  if (json_t* analogCharacterEnabledJ = json_object_get(root, "analogCharacterEnabled")) {
    analogCharacterEnabled.store(json_boolean_value(analogCharacterEnabledJ), std::memory_order_relaxed);
  }
  if (json_t* previewTracerEnabledJ = json_object_get(root, "previewTracerEnabled")) {
    previewTracerEnabled.store(json_boolean_value(previewTracerEnabledJ), std::memory_order_relaxed);
  }
  if (json_t* previewTracerModeJ = json_object_get(root, "previewTracerCacheMode")) {
    const int mode = int(json_integer_value(previewTracerModeJ));
    previewTracerCacheMode.store(mode == WAVE_PREVIEW_TRACER_CURVE_CACHE ? WAVE_PREVIEW_TRACER_CURVE_CACHE
                                                                          : WAVE_PREVIEW_TRACER_FRAME_CACHE,
                                 std::memory_order_relaxed);
  }
  if (!isDragonKingPreviewWidgetOptionsEnabled()) {
    previewTracerCacheMode.store(WAVE_PREVIEW_TRACER_FRAME_CACHE, std::memory_order_relaxed);
  }
  if (json_t* edgeHardnessJ = json_object_get(root, "shapeEdgeHardness")) {
    params[EDGE_HARDNESS_PARAM].setValue(clamp(float(json_number_value(edgeHardnessJ)), 0.f, 1.f));
  }
  else if (json_t* hardEdgesJ = json_object_get(root, "shapeHardEdges")) {
    params[EDGE_HARDNESS_PARAM].setValue(json_boolean_value(hardEdgesJ) ? 0.5f : 0.f);
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
