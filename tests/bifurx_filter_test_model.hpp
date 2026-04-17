#pragma once

#include <algorithm>
#include <cmath>
#include <complex>

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
      return T(0.98f) * cascadeLp;
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
      return T(0.98f) * cascadeHpToHp;
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
  constexpr bool kTuneSvfOnly = true;
  PreviewModel model;
  float qScale = 1.f;
  float cutoffScale = 1.f;
  if (!kTuneSvfOnly) {
    switch (int(clampf(float(state.circuitMode), 0.f, 3.f))) {
      default: break;
    }
  }

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
  model.circuitMode = kTuneSvfOnly ? 0 : int(clampf(float(state.circuitMode), 0.f, 3.f));

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
  const std::complex<float> lpA = model.lowA.response(omega);
  const std::complex<float> bpA = model.bandA.response(omega);
  const std::complex<float> hpA = model.highA.response(omega);
  const std::complex<float> ntA = model.notchA.response(omega);
  const std::complex<float> lpB = model.lowB.response(omega);
  const std::complex<float> bpB = model.bandB.response(omega);
  const std::complex<float> hpB = model.highB.response(omega);
  const std::complex<float> ntB = model.notchB.response(omega);
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

} // namespace bifurx_test_model
