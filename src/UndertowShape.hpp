#pragma once

#include <algorithm>
#include <cmath>

namespace undertow_shape {

enum ShapeAlgorithm {
  SHAPE_ALGO_GEOMETRIC = 0,
  SHAPE_ALGO_NONLINEAR = 1,
  SHAPE_ALGO_THRESHOLD_FOLD = 2,
};

inline float clampf(float x, float lo, float hi) {
  return std::max(lo, std::min(x, hi));
}

inline float crossfade(float a, float b, float mix) {
  return a + (b - a) * mix;
}

inline float smooth01(float x) {
  x = clampf(x, 0.f, 1.f);
  return x * x * (3.f - 2.f * x);
}

inline float triToSine(float x) {
  const float x2 = x * x;
  return x * (1.5707963f - 0.6459641f * x2 + 0.0796926f * x2 * x2);
}

inline float shapeControlTaper(float shape) {
  shape = clampf(shape, 0.f, 1.f);
  // Perceptual taper: the STO-like fold/triangle character should emerge before
  // full clockwise. Keep 0 and 1 fixed while accelerating the mid/high region.
  return clampf(1.f - std::sqrt(1.f - shape), 0.f, 1.f);
}

inline float softPositiveKnee(float x, float knee) {
  knee = std::max(knee, 1e-5f);
  if (x <= 0.f) {
    return 0.f;
  }
  if (x >= knee) {
    return x - 0.5f * knee;
  }
  return 0.5f * x * x / knee;
}

inline float geometric(float phase, float shape) {
  const float p = phase - std::floor(phase);
  const float tri = 4.f * std::fabs(p - 0.5f) - 1.f;
  const float sine = triToSine(tri);
  const float s = smooth01(shape);

  const float triAmt = 0.99f * smooth01((shape - 0.00f) / 0.48f);
  const float foldAmt = smooth01((shape - 0.34f) / 0.62f);
  const float leanAmt = smooth01((shape - 0.12f) / 0.88f);

  float y = crossfade(sine, tri, triAmt);

  const float midDist = std::fabs(p - 0.5f);
  const float troughWidth = 0.16f - 0.05f * s;
  const float troughWindow = 1.f - smooth01(midDist / troughWidth);
  const float troughFold = troughWindow * troughWindow * troughWindow;
  y += 0.62f * foldAmt * troughFold;

  const float zeroCrossWindow = 1.f - std::fabs(tri);
  y += 0.16f * leanAmt * std::sin(2.f * float(M_PI) * p) * zeroCrossWindow;

  const float drive = 0.08f * s;
  y = y / (1.f + drive * std::fabs(y));

  y -= 0.14f * foldAmt;
  y *= 1.f + 0.16f * s;
  return clampf(y, -1.f, 1.f);
}

inline float nonlinear(float phase, float shape) {
  const float p = phase - std::floor(phase);
  const float tri = 4.f * std::fabs(p - 0.5f) - 1.f;
  const float s = smooth01(shape);

  const float sineBase = triToSine(tri);
  const float drive = 1.8f + 4.4f * s;
  const float offset = 0.34f * s;
  const float x = drive * (tri + offset);
  const float overdriven = x / (1.f + std::fabs(x));
  const float cancelAmt = 0.45f * s;
  float y = crossfade(sineBase, overdriven - cancelAmt * sineBase, s);
  y -= 0.08f * s;
  y *= 1.f + 0.16f * s;
  return clampf(y, -1.f, 1.f);
}

inline float thresholdFold(float phase, float shape) {
  const float p = phase - std::floor(phase);
  const float tri = 4.f * std::fabs(p - 0.5f) - 1.f;
  const float sine = triToSine(tri);
  const float u = smooth01(shape);

  const float triAmt = 0.035f * u + 0.30f * u * u;
  float core = crossfade(sine, tri, triAmt);

  const float fold = smooth01((u - 0.07f) / 0.86f);
  const float foldThreshold = -0.995f + 0.58f * fold;
  const float knee = 0.115f - 0.075f * fold;
  const float foldAsym = 1.f - smooth01((u - 0.58f) / 0.42f);
  // Express the STO-like left/right imbalance by moving the fold threshold,
  // not by adding a separate center bump after folding. This keeps the trough
  // geometry coherent and avoids the "patched-on" feel near the inner peak.
  const float center = p - 0.5f;
  const float centerWidth = 0.18f;
  const float centerNorm = clampf(center / centerWidth, -1.f, 1.f);
  const float centerWindow = 1.f - smooth01(std::fabs(centerNorm));
  const float asymSkew = centerNorm * centerWindow;
  const float localThreshold = foldThreshold + 0.065f * fold * foldAsym * asymSkew;
  const float under = softPositiveKnee(localThreshold - core, knee);
  float y = core + (2.05f + 0.55f * fold) * fold * under;

  const float edge = smooth01((u - 0.46f) / 0.50f);
  const float zeroCrossWindow = std::max(0.f, 1.f - std::fabs(tri));
  y -= 0.055f * edge * std::sin(2.f * float(M_PI) * p) * zeroCrossWindow;

  const float posDrive = 0.06f * u + 0.06f * edge;
  const float negDrive = 0.025f * u;
  if (y >= 0.f) {
    y = y / (1.f + posDrive * y);
  } else {
    y = y / (1.f + negDrive * -y);
  }

  y -= 0.185f * fold;
  y *= 1.f + 0.34f * u;
  return clampf(y, -1.f, 1.f);
}

inline float evaluate(int algorithm, float phase, float shape) {
  switch (algorithm) {
    case SHAPE_ALGO_THRESHOLD_FOLD:
      return thresholdFold(phase, shape);
    case SHAPE_ALGO_NONLINEAR:
      return nonlinear(phase, shape);
    case SHAPE_ALGO_GEOMETRIC:
    default:
      return geometric(phase, shape);
  }
}

} // namespace undertow_shape
