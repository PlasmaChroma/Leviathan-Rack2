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

ShapeStats measureShape(int algorithm, float knobShape, int samples = 4096) {
  ShapeStats stats;
  double sum = 0.0;
  const float shape = undertow_shape::shapeControlTaper(knobShape);
  for (int i = 0; i < samples; ++i) {
    const float phase = float(i) / float(samples);
    const float volts = 5.f * undertow_shape::evaluate(algorithm, phase, shape);
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
  bool pass = true;
  std::string detail;
  const int algorithms[] = {
    undertow_shape::SHAPE_ALGO_GEOMETRIC,
    undertow_shape::SHAPE_ALGO_NONLINEAR,
    undertow_shape::SHAPE_ALGO_THRESHOLD_FOLD,
  };
  for (int algorithm : algorithms) {
    const ShapeStats s = measureShape(algorithm, 0.f);
    const bool ok = inRange(s.max, 4.85f, 5.05f) &&
      inRange(s.min, -5.05f, -4.85f) &&
      inRange(s.max - s.min, 9.75f, 10.10f) &&
      std::fabs(s.mean) < 0.05f;
    pass = pass && ok;
    detail += " algo=" + std::to_string(algorithm) + "{" + statsDetail(s) + "}";
  }
  return {"SHAPE=0 stays full-scale and centered", pass, detail};
}

TestResult testThresholdFoldMaxShapeHasUsableLevel() {
  const ShapeStats s = measureShape(undertow_shape::SHAPE_ALGO_THRESHOLD_FOLD, 1.f);
  const bool pass = inRange(s.max, 4.25f, 5.05f) &&
    inRange(s.min, -5.05f, -3.75f) &&
    inRange(s.max - s.min, 8.75f, 10.10f) &&
    std::fabs(s.mean) < 0.90f;
  return {"Threshold fold max shape remains near 10Vpp", pass, statsDetail(s)};
}

TestResult testShapeAlgorithmsStayInsideRailsAcrossSweep() {
  bool pass = true;
  std::string detail;
  const int algorithms[] = {
    undertow_shape::SHAPE_ALGO_GEOMETRIC,
    undertow_shape::SHAPE_ALGO_NONLINEAR,
    undertow_shape::SHAPE_ALGO_THRESHOLD_FOLD,
  };
  const float shapes[] = {0.f, 0.10f, 0.25f, 0.50f, 0.75f, 1.f};
  for (int algorithm : algorithms) {
    for (float shape : shapes) {
      const ShapeStats s = measureShape(algorithm, shape);
      const bool ok = s.max <= 5.001f && s.min >= -5.001f && std::isfinite(s.mean);
      pass = pass && ok;
      if (!ok) {
        detail += " algo=" + std::to_string(algorithm) +
          " shape=" + std::to_string(shape) + "{" + statsDetail(s) + "}";
      }
    }
  }
  if (detail.empty()) {
    detail = "all sampled shapes inside +/-5V";
  }
  return {"Shape algorithms stay inside output rails", pass, detail};
}

} // namespace

int main() {
  std::vector<TestResult> tests;
  tests.push_back(testShapeZeroIsFullScaleSineLike());
  tests.push_back(testThresholdFoldMaxShapeHasUsableLevel());
  tests.push_back(testShapeAlgorithmsStayInsideRailsAcrossSweep());

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
