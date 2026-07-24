#include "../src/UndertowShape.hpp"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

namespace {

struct TestResult {
  std::string name;
  bool pass = false;
  std::string detail;
};

struct ShapeStats {
  float min = 1e9f;
  float max = -1e9f;
  float mean = 0.f;
};

ShapeStats measureShape(float knobShape, bool entryAsymmetry = false, bool hardEdges = true,
                        bool entryAsymmetryOnRight = false, int samples = 4096) {
  ShapeStats stats;
  double sum = 0.0;
  const float shape = undertow_shape::shapeControlTaper(knobShape);
  for (int i = 0; i < samples; ++i) {
    const float phase = float(i) / float(samples);
    const float volts = 5.f * undertow_shape::thresholdFold(phase, shape, entryAsymmetry, hardEdges,
                                                            entryAsymmetryOnRight);
    stats.min = std::min(stats.min, volts);
    stats.max = std::max(stats.max, volts);
    sum += volts;
  }
  stats.mean = float(sum / double(samples));
  return stats;
}

std::string statsDetail(const ShapeStats& s) {
  return "min=" + std::to_string(s.min) +
    " max=" + std::to_string(s.max) +
    " pp=" + std::to_string(s.max - s.min) +
    " mean=" + std::to_string(s.mean);
}

bool inRange(float x, float lo, float hi) {
  return x >= lo && x <= hi;
}

TestResult testShapeZeroIsFullScaleSineLike() {
  const ShapeStats s = measureShape(0.f);
  const bool pass = inRange(s.max, 4.85f, 5.05f) &&
    inRange(s.min, -5.05f, -4.85f) &&
    inRange(s.max - s.min, 9.75f, 10.10f) &&
    std::fabs(s.mean) < 0.05f;
  const std::string detail = statsDetail(s);
  return {"SHAPE=0 stays full-scale and centered", pass, detail};
}

TestResult testThresholdFoldMaxShapeHasUsableLevel() {
  const ShapeStats s = measureShape(1.f);
  const bool pass = inRange(s.max, 4.25f, 5.05f) &&
    inRange(s.min, -5.05f, -3.75f) &&
    inRange(s.max - s.min, 8.75f, 10.10f) &&
    std::fabs(s.mean) < 0.90f;
  return {"Threshold fold max shape remains near 10Vpp", pass, statsDetail(s)};
}

TestResult testShapeAlgorithmsStayInsideRailsAcrossSweep() {
  bool pass = true;
  std::string detail;
  const float shapes[] = {0.f, 0.10f, 0.25f, 0.50f, 0.75f, 1.f};
  for (float shape : shapes) {
    const ShapeStats s = measureShape(shape);
    const bool ok = s.max <= 5.001f && s.min >= -5.001f && std::isfinite(s.mean);
    pass = pass && ok;
    if (!ok) {
      detail += " shape=" + std::to_string(shape) + "{" + statsDetail(s) + "}";
    }
  }
  if (detail.empty()) {
    detail = "all sampled shapes inside +/-5V";
  }
  return {"Shape output stays inside rails", pass, detail};
}

TestResult testThresholdFoldIsContinuousAtWrap() {
  bool pass = true;
  float worst = 0.f;
  for (int si = 0; si <= 100; ++si) {
    const float shape = float(si) / 100.f;
    for (int asym = 0; asym <= 1; ++asym) {
      for (int hard = 0; hard <= 1; ++hard) {
        for (int side = 0; side <= 1; ++side) {
          const float beforeWrap =
              undertow_shape::thresholdFold(1.f - 1e-6f, shape, asym != 0, hard != 0, side != 0);
          const float atWrap = undertow_shape::thresholdFold(0.f, shape, asym != 0, hard != 0, side != 0);
          worst = std::max(worst, std::fabs(beforeWrap - atWrap));
        }
      }
    }
  }
  pass = worst < 1e-3f;
  return {"Threshold fold remains continuous at phase wrap", pass, "worst delta=" + std::to_string(worst)};
}

} // namespace

int main() {
  std::vector<TestResult> tests;
  tests.push_back(testShapeZeroIsFullScaleSineLike());
  tests.push_back(testThresholdFoldMaxShapeHasUsableLevel());
  tests.push_back(testShapeAlgorithmsStayInsideRailsAcrossSweep());
  tests.push_back(testThresholdFoldIsContinuousAtWrap());

  int failed = 0;
  std::cout << "Undertow Shape Spec\n";
  std::cout << "-------------------\n";
  for (const TestResult& test : tests) {
    std::cout << (test.pass ? "[PASS] " : "[FAIL] ") << test.name << " :: " << test.detail << "\n";
    if (!test.pass) {
      failed++;
    }
  }
  std::cout << "-------------------\n";
  std::cout << "Summary: " << (tests.size() - failed) << "/" << tests.size() << " passed\n";
  return failed == 0 ? 0 : 1;
}
