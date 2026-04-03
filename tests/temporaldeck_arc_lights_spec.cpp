#include "../src/TemporalDeckArcLights.hpp"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

namespace {

using temporaldeck_ui::ArcLightState;
using temporaldeck_ui::computeArcLightState;
using temporaldeck_ui::kTemporalDeckArcLightCount;

struct TestResult {
  std::string name;
  bool pass = false;
  std::string detail;
};

int countRedLeds(const ArcLightState &state, float threshold = 1e-4f) {
  int count = 0;
  for (int i = 0; i < kTemporalDeckArcLightCount; ++i) {
    if (state.red[i] > threshold) {
      count++;
    }
  }
  return count;
}

float totalYellow(const ArcLightState &state) {
  float sum = 0.f;
  for (int i = 0; i < kTemporalDeckArcLightCount; ++i) {
    sum += state.yellow[i];
  }
  return sum;
}

TestResult testLiveModeAtNowShowsSingleRedLimit() {
  ArcLightState state = computeArcLightState(0, 48000.f, false, false, 0.0, 0.0, 0.0);
  bool pass = countRedLeds(state) == 1 && state.red[0] > 0.5f && std::fabs(totalYellow(state)) < 1e-3f;
  return {"Live mode NOW marker", pass,
          "redCount=" + std::to_string(countRedLeds(state)) + " red0=" + std::to_string(state.red[0]) +
            " yellowSum=" + std::to_string(totalYellow(state))};
}

TestResult testLiveModeLimitAtEndMovesRedMarker() {
  ArcLightState state = computeArcLightState(0, 1000.f, false, false, 1000.0, 1000.0, 0.0);
  int last = kTemporalDeckArcLightCount - 1;
  bool pass = countRedLeds(state) == 1 && state.red[last] > 0.5f && totalYellow(state) > 1.0f;
  return {"Live mode end marker", pass,
          "redCount=" + std::to_string(countRedLeds(state)) + " redLast=" + std::to_string(state.red[last]) +
            " yellowSum=" + std::to_string(totalYellow(state))};
}

TestResult testSampleModeProgressIncreasesYellowFill() {
  ArcLightState start = computeArcLightState(101, 48000.f, true, true, 0.0, 50.0, 0.0);
  ArcLightState end = computeArcLightState(101, 48000.f, true, true, 0.0, 50.0, 1.0);
  bool pass = totalYellow(end) > totalYellow(start) + 1.0f && countRedLeds(start) == 1 && countRedLeds(end) == 1;
  return {"Sample mode progress fill", pass,
          "yellowStart=" + std::to_string(totalYellow(start)) + " yellowEnd=" + std::to_string(totalYellow(end))};
}

TestResult testSampleModeNotLoadedFallsBackToLivePath() {
  ArcLightState a = computeArcLightState(101, 1000.f, true, false, 250.0, 500.0, 0.8);
  ArcLightState b = computeArcLightState(101, 1000.f, false, false, 250.0, 500.0, 0.8);

  float diff = 0.f;
  for (int i = 0; i < kTemporalDeckArcLightCount; ++i) {
    diff += std::fabs(a.yellow[i] - b.yellow[i]);
    diff += std::fabs(a.red[i] - b.red[i]);
  }
  bool pass = diff < 1e-4f;
  return {"Sample-mode disabled when sample unloaded", pass, "absDiff=" + std::to_string(diff)};
}

} // namespace

int main() {
  std::vector<TestResult> tests;
  tests.push_back(testLiveModeAtNowShowsSingleRedLimit());
  tests.push_back(testLiveModeLimitAtEndMovesRedMarker());
  tests.push_back(testSampleModeProgressIncreasesYellowFill());
  tests.push_back(testSampleModeNotLoadedFallsBackToLivePath());

  int failed = 0;
  std::cout << "TemporalDeck Arc Lights Spec\n";
  std::cout << "----------------------------\n";
  for (const auto &t : tests) {
    std::cout << (t.pass ? "[PASS] " : "[FAIL] ") << t.name << " :: " << t.detail << "\n";
    if (!t.pass) {
      failed++;
    }
  }
  std::cout << "----------------------------\n";
  std::cout << "Summary: " << (tests.size() - failed) << "/" << tests.size() << " passed\n";
  return failed == 0 ? 0 : 1;
}
