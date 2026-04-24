#pragma once

#include <algorithm>
#include <cmath>
#include <complex>
#include <cstddef>
#include <vector>

namespace bifurx_test_model {

constexpr float kPi = 3.14159265358979323846f;
constexpr float kFreqMinHz = 4.f;
constexpr float kFreqMaxHz = 28000.f;

inline float clampf(float x, float lo, float hi) {
  return std::max(lo, std::min(hi, x));
}

inline float clamp01(float x) {
  return clampf(x, 0.f, 1.f);
}

inline float mixf(float a, float b, float t) {
  return a + (b - a) * t;
}

inline float fastExp(float x) {
  return std::exp(x);
}

inline float signedWeight(float balance, bool upperPeak) {
  const float sign = upperPeak ? 1.f : -1.f;
  return fastExp(0.82f * sign * clampf(balance, -1.f, 1.f));
}

inline float cascadeWideMorph(float spanNorm) {
  const float x = clamp01((clamp01(spanNorm) - 0.03f) / 0.97f);
  return std::pow(x, 0.58f);
}

inline float highHighSpanCompGain(float wideMorph) {
  const float x = clamp01((wideMorph - 0.75f) / 0.25f);
  return 1.f + 0.685f * std::pow(x, 1.1f);
}

struct SemanticExportProfile {
  float lpScale = 0.f;
  float bpScale = 0.f;
  float hpScale = 0.f;
};

inline SemanticExportProfile semanticExportProfile(int circuitMode, int stageIndex) {
  const int clampedCircuitMode = std::max(0, std::min(3, circuitMode));
  (void)stageIndex;
  SemanticExportProfile profile;
  switch (clampedCircuitMode) {
    case 1:
      profile.lpScale = 0.f;
      profile.bpScale = 5.5f;
      profile.hpScale = 5.5f;
      return profile;
    case 2:
      profile.lpScale = 0.f;
      profile.bpScale = 6.5f;
      profile.hpScale = 0.f;
      return profile;
    case 3:
      profile.lpScale = 5.5f;
      profile.bpScale = 3.8f;
      profile.hpScale = 4.5f;
      return profile;
    default:
      return profile;
  }
}

template <typename T>
inline T normalizeSemanticComponent(const T& value, float exportScale) {
  if (!(exportScale > 0.f)) {
    return value;
  }
  const float magnitude = std::abs(value);
  if (!(magnitude > 0.f) || !std::isfinite(magnitude)) {
    return value;
  }
  const float compressed = exportScale * std::tanh(magnitude / exportScale);
  if (!(compressed > 0.f) || !std::isfinite(compressed)) {
    return value;
  }
  return value * T(compressed / magnitude);
}

struct SvfOutputs {
  float lp = 0.f;
  float bp = 0.f;
  float hp = 0.f;
};

inline SvfOutputs normalizeSemanticOutputs(const SvfOutputs& raw, int circuitMode, int stageIndex) {
  const SemanticExportProfile profile = semanticExportProfile(circuitMode, stageIndex);
  SvfOutputs out;
  out.lp = normalizeSemanticComponent(raw.lp, profile.lpScale);
  out.bp = normalizeSemanticComponent(raw.bp, profile.bpScale);
  out.hp = normalizeSemanticComponent(raw.hp, profile.hpScale);
  return out;
}

struct DisplayBiquad {
  float b0 = 0.f;
  float b1 = 0.f;
  float b2 = 0.f;
  float a1 = 0.f;
  float a2 = 0.f;

  std::complex<float> response(float omega) const {
    const std::complex<float> z1 = std::exp(std::complex<float>(0.f, -omega));
    const std::complex<float> z2 = z1 * z1;
    const std::complex<float> num = b0 + b1 * z1 + b2 * z2;
    const std::complex<float> den = 1.f + a1 * z1 + a2 * z2;
    return num / den;
  }
};

inline DisplayBiquad makeDisplayBiquad(float sampleRate, float cutoff, float q, int type) {
  const float sr = std::max(sampleRate, 1.f);
  const float freq = clampf(cutoff, kFreqMinHz, 0.46f * sr);
  const float omega = 2.f * kPi * freq / sr;
  const float cosW = std::cos(omega);
  const float sinW = std::sin(omega);
  const float alpha = sinW / (2.f * std::max(q, 0.05f));

  float b0 = 0.f;
  float b1 = 0.f;
  float b2 = 0.f;
  const float a0 = 1.f + alpha;
  const float a1 = -2.f * cosW;
  const float a2 = 1.f - alpha;

  switch (type) {
    case 0: // lowpass
      b0 = 0.5f * (1.f - cosW);
      b1 = 1.f - cosW;
      b2 = 0.5f * (1.f - cosW);
      break;
    case 1: // bandpass
      b0 = alpha;
      b1 = 0.f;
      b2 = -alpha;
      break;
    case 2: // highpass
      b0 = 0.5f * (1.f + cosW);
      b1 = -(1.f + cosW);
      b2 = 0.5f * (1.f + cosW);
      break;
    default: // notch
      b0 = 1.f;
      b1 = -2.f * cosW;
      b2 = 1.f;
      break;
  }

  DisplayBiquad biquad;
  biquad.b0 = b0 / a0;
  biquad.b1 = b1 / a0;
  biquad.b2 = b2 / a0;
  biquad.a1 = a1 / a0;
  biquad.a2 = a2 / a0;
  return biquad;
}

template <typename T>
inline T combineModeResponse(
  int mode,
  const T& lpA,
  const T& bpA,
  const T& hpA,
  const T& /*ntA*/,
  const T& lpB,
  const T& bpB,
  const T& hpB,
  const T& /*ntB*/,
  const T& cascadeLp,
  const T& cascadeNotch,
  const T& cascadeHpToLp,
  const T& cascadeHpToHp,
  float wA,
  float wB,
  float wideMorph
) {
  (void)wideMorph;
  switch (mode) {
    case 0:
      return cascadeLp;
    case 1:
      return T(0.92f) * T(wA) * lpA + T(1.18f) * T(wB) * bpB - T(0.16f) * (bpA + bpB);
    case 2:
      return T(1.08f) * T(wB) * lpB - T(0.62f) * T(wA) * bpA;
    case 3:
      return T(1.03f) * cascadeNotch;
    case 4:
      return T(0.98f) * T(wA) * lpA + T(0.98f) * T(wB) * hpB - T(0.06f) * (bpA + bpB);
    case 5:
      return T(1.08f) * (T(wA) * bpA + T(wB) * bpB);
    case 6:
      return T(1.04f) * cascadeHpToLp;
    case 7:
      return T(1.08f) * T(wA) * hpA - T(0.60f) * T(wB) * bpB;
    case 8:
      return T(1.18f) * T(wA) * bpA + T(0.94f) * T(wB) * hpB - T(0.14f) * (hpA + bpB);
    case 9:
      return T(1.06f * highHighSpanCompGain(wideMorph)) * cascadeHpToHp;
    default:
      return T(1.f);
  }
}

struct PreviewState {
  float sampleRate = 44100.f;
  float freqA = 440.f;
  float freqB = 440.f;
  float qA = 1.f;
  float qB = 1.f;
  float balance = 0.f;
  float resoNorm = 0.f;
  float spanNorm = 0.5f;
  int mode = 0;
  int circuitMode = 0;
};

struct PreviewModel {
  DisplayBiquad lowA;
  DisplayBiquad bandA;
  DisplayBiquad highA;
  DisplayBiquad notchA;
  DisplayBiquad lowB;
  DisplayBiquad bandB;
  DisplayBiquad highB;
  DisplayBiquad notchB;
  float markerFreqA = 440.f;
  float markerFreqB = 440.f;
  float sampleRate = 44100.f;
  float wA = 1.f;
  float wB = 1.f;
  float wideMorph = 0.f;
  int mode = 0;
  int circuitMode = 0;
};

inline PreviewModel makePreviewModel(const PreviewState& state) {
  PreviewModel model;
  float qScale = 1.f;
  float cutoffScale = 1.f;

  const float freqA = clampf(state.freqA * cutoffScale, kFreqMinHz, 0.46f * std::max(state.sampleRate, 1.f));
  const float freqB = clampf(state.freqB * cutoffScale, kFreqMinHz, 0.46f * std::max(state.sampleRate, 1.f));
  const float qA = clampf(state.qA * qScale, 0.2f, 18.f);
  const float qB = clampf(state.qB * qScale, 0.2f, 18.f);

  model.lowA = makeDisplayBiquad(state.sampleRate, freqA, qA, 0);
  model.bandA = makeDisplayBiquad(state.sampleRate, freqA, qA, 1);
  model.highA = makeDisplayBiquad(state.sampleRate, freqA, qA, 2);
  model.notchA = makeDisplayBiquad(state.sampleRate, freqA, qA, 3);
  model.lowB = makeDisplayBiquad(state.sampleRate, freqB, qB, 0);
  model.bandB = makeDisplayBiquad(state.sampleRate, freqB, qB, 1);
  model.highB = makeDisplayBiquad(state.sampleRate, freqB, qB, 2);
  model.notchB = makeDisplayBiquad(state.sampleRate, freqB, qB, 3);
  model.markerFreqA = freqA;
  model.markerFreqB = freqB;
  model.sampleRate = state.sampleRate;
  model.mode = state.mode;
  model.circuitMode = int(clampf(float(state.circuitMode), 0.f, 3.f));

  const float lowW = signedWeight(state.balance, false);
  const float highW = signedWeight(state.balance, true);
  const float norm = 2.f / (lowW + highW);
  model.wA = lowW * norm;
  model.wB = highW * norm;
  model.wideMorph = cascadeWideMorph(state.spanNorm);
  return model;
}

inline std::complex<float> response(const PreviewModel& model, float hz) {
  const float omega = 2.f * kPi * clampf(hz, kFreqMinHz, 0.49f * model.sampleRate) / std::max(model.sampleRate, 1.f);
  const SemanticExportProfile profileA = semanticExportProfile(model.circuitMode, 0);
  const SemanticExportProfile profileB = semanticExportProfile(model.circuitMode, 1);
  const std::complex<float> lpA = normalizeSemanticComponent(model.lowA.response(omega), profileA.lpScale);
  const std::complex<float> bpA = normalizeSemanticComponent(model.bandA.response(omega), profileA.bpScale);
  const std::complex<float> hpA = normalizeSemanticComponent(model.highA.response(omega), profileA.hpScale);
  const std::complex<float> ntA = lpA + hpA;
  const std::complex<float> lpB = normalizeSemanticComponent(model.lowB.response(omega), profileB.lpScale);
  const std::complex<float> bpB = normalizeSemanticComponent(model.bandB.response(omega), profileB.bpScale);
  const std::complex<float> hpB = normalizeSemanticComponent(model.highB.response(omega), profileB.hpScale);
  const std::complex<float> ntB = lpB + hpB;
  const std::complex<float> cascadeLp = lpB * lpA;
  const std::complex<float> cascadeNotch = ntB * ntA;
  const std::complex<float> cascadeHpToLp = lpB * hpA;
  const std::complex<float> cascadeHpToHp = hpB * hpA;

  return combineModeResponse<std::complex<float>>(
    model.mode,
    lpA, bpA, hpA, ntA,
    lpB, bpB, hpB, ntB,
    cascadeLp, cascadeNotch, cascadeHpToLp, cascadeHpToHp,
    model.wA, model.wB, model.wideMorph
  );
}

inline float responseDb(const PreviewModel& model, float hz) {
  const float mag = std::abs(response(model, hz));
  return 20.f * std::log10(std::max(mag, 1e-5f));
}

inline float levelDriveGain(float knob) {
  const float x = clamp01(knob);
  return 0.06f + 0.95f * x + 3.6f * x * x * x;
}

inline float softClip(float x) {
  return std::tanh(x);
}

inline float amplitudeRatioDb(float numerator, float denominator) {
  return 20.f * std::log10((std::fabs(numerator) + 1e-6f) / (std::fabs(denominator) + 1e-6f));
}

struct SvfCoeffs {
  float g = 0.f;
  float k = 0.f;
  float a1 = 1.f;
};

inline SvfCoeffs makeSvfCoeffs(float sampleRate, float cutoff, float damping) {
  const float sr = std::max(sampleRate, 1.f);
  const float limitedCutoff = clampf(cutoff, kFreqMinHz, 0.46f * sr);
  const float g = std::tan(kPi * limitedCutoff / sr);
  const float k = clampf(damping, 0.02f, 2.2f);
  const float a1 = 1.f / (1.f + g * (g + k));
  SvfCoeffs c;
  c.g = g;
  c.k = k;
  c.a1 = a1;
  return c;
}

struct SvfState {
  float ic1eq = 0.f;
  float ic2eq = 0.f;
};

inline SvfOutputs processSvf(SvfState& s, float input, const SvfCoeffs& c) {
  const float v1 = c.a1 * (s.ic1eq + c.g * (input - s.ic2eq));
  const float v2 = s.ic2eq + c.g * v1;

  s.ic1eq = 2.f * v1 - s.ic1eq;
  s.ic2eq = 2.f * v2 - s.ic2eq;

  SvfOutputs out;
  out.bp = v1;
  out.lp = v2;
  out.hp = input - c.k * v1 - v2;
  return out;
}

struct LlRuntimeTelemetry {
  float inputRms = 0.f;
  float stageALpRms = 0.f;
  float stageBLpRms = 0.f;
  float outputRms = 0.f;
  float stageBOverADb = 0.f;
  float outputOverInputDb = 0.f;
};

struct LlRuntimeSweepPoint {
  float freqHz = 0.f;
  LlRuntimeTelemetry telemetry;
};

inline LlRuntimeTelemetry measureLlRuntimeTelemetry(
  float sampleRate,
  float inputHz,
  float inputAmplitude,
  float levelKnob,
  float cutoffA,
  float cutoffB,
  float dampingA,
  float dampingB,
  int circuitMode = 0
) {
  const int settleSamples = int(sampleRate * 0.30f);
  const int measureSamples = int(sampleRate * 0.60f);
  const int totalSamples = settleSamples + measureSamples;

  const SvfCoeffs cA = makeSvfCoeffs(sampleRate, cutoffA, dampingA);
  const SvfCoeffs cB = makeSvfCoeffs(sampleRate, cutoffB, dampingB);
  SvfState a;
  SvfState b;

  const float drive = levelDriveGain(levelKnob);
  float inSq = 0.f;
  float aSq = 0.f;
  float bSq = 0.f;
  float outSq = 0.f;
  int nAccum = 0;

  for (int n = 0; n < totalSamples; ++n) {
    const float t = float(n) / sampleRate;
    const float in = inputAmplitude * std::sin(2.f * kPi * inputHz * t);
    const float drivenIn = 5.f * softClip(0.2f * in * drive);
    const SvfOutputs oA = normalizeSemanticOutputs(processSvf(a, drivenIn, cA), circuitMode, 0);
    const SvfOutputs oB = normalizeSemanticOutputs(processSvf(b, oA.lp, cB), circuitMode, 1);
    const float modeOut = oB.lp;
    const float out = 5.5f * softClip(modeOut / 5.5f);

    if (n >= settleSamples) {
      inSq += in * in;
      aSq += oA.lp * oA.lp;
      bSq += oB.lp * oB.lp;
      outSq += out * out;
      nAccum++;
    }
  }

  LlRuntimeTelemetry telemetry;
  telemetry.inputRms = std::sqrt(std::max(inSq / std::max(1, nAccum), 1e-12f));
  telemetry.stageALpRms = std::sqrt(std::max(aSq / std::max(1, nAccum), 1e-12f));
  telemetry.stageBLpRms = std::sqrt(std::max(bSq / std::max(1, nAccum), 1e-12f));
  telemetry.outputRms = std::sqrt(std::max(outSq / std::max(1, nAccum), 1e-12f));
  telemetry.stageBOverADb = amplitudeRatioDb(telemetry.stageBLpRms, telemetry.stageALpRms);
  telemetry.outputOverInputDb = amplitudeRatioDb(telemetry.outputRms, telemetry.inputRms);
  return telemetry;
}

inline std::vector<LlRuntimeSweepPoint> makeLlRuntimeSweep(
  float sampleRate,
  const std::vector<float>& freqsHz,
  float inputAmplitude,
  float levelKnob,
  float cutoffA,
  float cutoffB,
  float dampingA,
  float dampingB,
  int circuitMode = 0
) {
  std::vector<LlRuntimeSweepPoint> sweep;
  sweep.reserve(freqsHz.size());
  for (float hz : freqsHz) {
    LlRuntimeSweepPoint point;
    point.freqHz = hz;
    point.telemetry = measureLlRuntimeTelemetry(
      sampleRate,
      hz,
      inputAmplitude,
      levelKnob,
      cutoffA,
      cutoffB,
      dampingA,
      dampingB,
      circuitMode
    );
    sweep.push_back(point);
  }
  return sweep;
}

inline float simulateLlRuntimeGainDb(
  float sampleRate,
  float inputHz,
  float inputAmplitude,
  float levelKnob,
  float cutoffA,
  float cutoffB,
  float dampingA,
  float dampingB,
  int circuitMode = 0
) {
  const LlRuntimeTelemetry telemetry = measureLlRuntimeTelemetry(
    sampleRate, inputHz, inputAmplitude, levelKnob, cutoffA, cutoffB, dampingA, dampingB, circuitMode
  );
  return telemetry.outputOverInputDb;
}

inline float simulateHhRuntimeGainDb(
  float sampleRate,
  float inputHz,
  float inputAmplitude,
  float levelKnob,
  float cutoffA,
  float cutoffB,
  float dampingA,
  float dampingB,
  float wideMorph,
  int circuitMode = 0
) {
  const int settleSamples = int(sampleRate * 0.30f);
  const int measureSamples = int(sampleRate * 0.60f);
  const int totalSamples = settleSamples + measureSamples;

  const SvfCoeffs cA = makeSvfCoeffs(sampleRate, cutoffA, dampingA);
  const SvfCoeffs cB = makeSvfCoeffs(sampleRate, cutoffB, dampingB);
  SvfState a;
  SvfState b;

  const float drive = levelDriveGain(levelKnob);
  const float hhGain = 1.06f * highHighSpanCompGain(wideMorph);
  float inSq = 0.f;
  float outSq = 0.f;
  int nAccum = 0;

  for (int n = 0; n < totalSamples; ++n) {
    const float t = float(n) / sampleRate;
    const float in = inputAmplitude * std::sin(2.f * kPi * inputHz * t);
    const float drivenIn = 5.f * softClip(0.2f * in * drive);
    const SvfOutputs oA = normalizeSemanticOutputs(processSvf(a, drivenIn, cA), circuitMode, 0);
    const SvfOutputs oB = normalizeSemanticOutputs(processSvf(b, oA.hp, cB), circuitMode, 1);
    const float modeOut = hhGain * oB.hp;
    const float out = 5.5f * softClip(modeOut / 5.5f);

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

} // namespace bifurx_test_model
