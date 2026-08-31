#pragma once

#include <algorithm>
#include <cmath>
#include <limits>

namespace nautiloid_gpu_precision {

struct FloatPair {
  float hi = 0.f;
  float lo = 0.f;
};

inline FloatPair splitDouble(double value) {
  if (!std::isfinite(value)) return FloatPair();
  const float hi = float(value);
  FloatPair pair;
  pair.hi = hi;
  pair.lo = float(value - double(hi));
  return pair;
}

inline float halfFloatSpacing(double magnitude) {
  const float representative = float(std::max(1.0, std::fabs(magnitude)));
  const float next = std::nextafter(representative, std::numeric_limits<float>::infinity());
  return 0.5f * (next - representative);
}

inline bool requiresDeepPrecision(
  double centerX,
  double centerY,
  double halfSpanX,
  double halfSpanY,
  int framebufferWidth,
  int framebufferHeight,
  double maxErrorInPixels = 0.25) {
  if (framebufferWidth < 1 || framebufferHeight < 1 ||
      !(halfSpanX > 0.0) || !(halfSpanY > 0.0)) {
    return false;
  }
  const double pixelX = 2.0 * halfSpanX / double(framebufferWidth);
  const double pixelY = 2.0 * halfSpanY / double(framebufferHeight);
  const double coordinateMagnitude = std::max({
    1.0,
    std::fabs(centerX) + halfSpanX,
    std::fabs(centerY) + halfSpanY,
  });
  const double floatError = double(halfFloatSpacing(coordinateMagnitude));
  return floatError > maxErrorInPixels * pixelX ||
    floatError > maxErrorInPixels * pixelY;
}

} // namespace nautiloid_gpu_precision
