#pragma once

#include <algorithm>
#include <cmath>

namespace undertow_shape {

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

inline float smoothPulse(float x, float start, float end, float edge) {
  if (end <= start) {
    return 0.f;
  }
  edge = std::max(edge, 1e-5f);
  const float rise = smooth01((x - start) / edge);
  const float fall = 1.f - smooth01((x - (end - edge)) / edge);
  return clampf(rise * fall, 0.f, 1.f);
}

inline float triToSine(float x) {
  const float x2 = x * x;
  return x * (1.5707963f - 0.6459641f * x2 + 0.0796926f * x2 * x2);
}

inline float shapeControlTaper(float shape) {
  shape = clampf(shape, 0.f, 1.f);
  // Front-load the SHAPE travel: by 50% knob the folded triangle should be
  // visibly reaching the zero line, with the last half mostly increasing peak.
  return std::sqrt(shape);
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

inline float thresholdFold(float phase, float shape) {
  const float p = phase - std::floor(phase);
  const float tri = 4.f * std::fabs(p - 0.5f) - 1.f;
  const float sine = triToSine(tri);
  const float u = smooth01(shape);

  // Use a growing local triangle region rather than a uniform whole-wave
  // crossfade. This lets the triangle start as a narrow wedge and expand until
  // it occupies almost the full cycle, leaving only a small sine remnant.
  const float takeover = smooth01(shape / 0.52f);
  const float baseTriAmt = clampf(0.12f * shape + 0.22f * u, 0.f, 0.34f);
  const float triRegionStart = 0.5f - (0.065f + 0.435f * takeover);
  const float triRegionEnd = 0.5f + (0.095f + 0.405f * takeover);
  const float triRegionEdge = 0.006f + 0.016f * (1.f - takeover);
  const float triRegionAmt = 0.34f + 0.66f * takeover;
  const float regionTriMix = triRegionAmt * smoothPulse(p, triRegionStart, triRegionEnd, triRegionEdge);
  const float triMix = clampf(std::max(baseTriAmt, regionTriMix), 0.f, 1.f);
  float core = crossfade(sine, tri, triMix);

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
  const float localThreshold = foldThreshold + 0.045f * fold * foldAsym * asymSkew;
  const float under = softPositiveKnee(localThreshold - core, knee);
  float y = core + (2.05f + 0.55f * fold) * fold * under;

  const float peakLift = smooth01((shape - 0.72f) / 0.28f);
  y += 0.42f * peakLift * centerWindow;

  const float edge = smooth01((u - 0.46f) / 0.50f);
  const float zeroCrossWindow = std::max(0.f, 1.f - std::fabs(tri));
  y -= 0.030f * edge * std::sin(2.f * float(M_PI) * p) * zeroCrossWindow;

  const float posDrive = 0.04f * u + 0.05f * edge;
  const float negDrive = 0.025f * u;
  if (y >= 0.f) {
    y = y / (1.f + posDrive * y);
  } else {
    y = y / (1.f + negDrive * -y);
  }

  y -= 0.155f * fold;
  y *= 1.f + 0.42f * u;
  return clampf(y, -1.f, 1.f);
}

} // namespace undertow_shape
