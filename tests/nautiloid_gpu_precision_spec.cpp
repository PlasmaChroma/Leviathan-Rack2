#include "../src/NautiloidGpuPrecision.hpp"

#include <cmath>
#include <iostream>
#include <string>

namespace {

int failures = 0;

void check(const std::string& name, bool condition) {
  std::cout << (condition ? "[PASS] " : "[FAIL] ") << name << "\n";
  if (!condition) ++failures;
}

} // namespace

int main() {
  using namespace nautiloid_gpu_precision;

  const double values[] = {0.0, 0.5, -0.75, 1.0, -1.76, 2.0};
  bool splitRoundTrips = true;
  for (double value : values) {
    const FloatPair pair = splitDouble(value);
    splitRoundTrips = splitRoundTrips &&
      std::fabs((double(pair.hi) + double(pair.lo)) - value) < 1e-14;
  }
  check("double coordinates split into reconstructable float pairs", splitRoundTrips);

  check("shallow viewport retains fast float shader",
    !requiresDeepPrecision(0.0, 0.0, 1.62, 0.86, 1152, 768));

  const double maxZoomScale = std::pow(0.05, 4.0);
  check("maximum zoom selects deep precision at origin",
    requiresDeepPrecision(
      0.0, 0.0, 1.62 * maxZoomScale, 0.86 * maxZoomScale, 768, 512));
  check("maximum zoom selects deep precision near coordinate limit",
    requiresDeepPrecision(
      2.0, -2.0, 1.62 * maxZoomScale, 0.86 * maxZoomScale, 1152, 768));

  const double zoomThreeScale = std::pow(0.05, 3.0);
  check("representative zoom-three viewport stays on fast path",
    !requiresDeepPrecision(
      0.5, -0.5, 1.62 * zoomThreeScale, 0.86 * zoomThreeScale, 768, 512));

  return failures == 0 ? 0 : 1;
}
