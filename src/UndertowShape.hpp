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
  return clampf(shape, 0.f, 1.f);
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

inline float foldedTriangleWidth(float shape) {
  const float s = clampf(shape, 0.f, 1.f);
  return 0.030f + 0.460f * smooth01(s);
}

inline float foldedTriangleTarget(float phase, float shape, bool entryAsymmetry = false) {
  const float p = phase - std::floor(phase);
  const float s = clampf(shape, 0.f, 1.f);
  const float width = foldedTriangleWidth(s);
  const float centerNorm = (p - 0.5f) / width;
  const float x = std::fabs(centerNorm);
  const float triangle = clampf(1.f - x, 0.f, 1.f);
  const float leftEntry = clampf(-centerNorm, 0.f, 1.f);
  const float baseRegion = 1.f - smooth01(triangle / 0.45f);
  const float entryLift = entryAsymmetry ? (0.09f * smooth01((s - 0.12f) / 0.72f) * leftEntry * baseRegion) : 0.f;
  return -1.f + entryLift + 2.f * s * triangle;
}

inline float thresholdFold(float phase, float shape, bool entryAsymmetry = false, bool hardEdges = false) {
  const float p = phase - std::floor(phase);
  const float tri = 4.f * std::fabs(p - 0.5f) - 1.f;
  const float sine = triToSine(tri);
  const float s = clampf(shape, 0.f, 1.f);
  const float width = foldedTriangleWidth(s);
  const float centerNorm = (p - 0.5f) / width;
  const float x = std::fabs(centerNorm);
  const float triangle = clampf(1.f - x, 0.f, 1.f);

  // Direct target: a triangle rises out of the sine trough. Its apex is -1 at
  // 0% shape, 0 at 50%, and +1 at 100%, while its width expands with shape.
  const float foldedTriangle = foldedTriangleTarget(p, s, entryAsymmetry);
  const float active = smooth01(s / 0.08f);
  // Keep the triangle base near the lower rail. A wide edge blend makes the
  // sine erase the base too early, which reduces SHAPE peak-to-peak level.
  const float edgeWidth = hardEdges ? 0.018f : 0.075f;
  const float edgeBlend = smooth01(triangle / edgeWidth);
  const float y = crossfade(sine, foldedTriangle, active * edgeBlend);
  return clampf(y, -1.f, 1.f);
}

} // namespace undertow_shape
