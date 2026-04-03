#include "../src/TemporalDeckPlatterInput.hpp"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

namespace {

using temporaldeck::PlatterInputSnapshot;
using temporaldeck::PlatterInputState;

struct TestResult {
  std::string name;
  bool pass = false;
  std::string detail;
};

bool nearlyEqual(float a, float b, float eps = 1e-6f) {
  return std::fabs(a - b) <= eps;
}

TestResult testScratchHoldCountdownAndGestureVisibility() {
  PlatterInputState state;
  state.setScratch(true, 120.f, -3.5f, 3);

  PlatterInputSnapshot a = state.consumeForFrame();
  PlatterInputSnapshot b = state.consumeForFrame();
  PlatterInputSnapshot c = state.consumeForFrame();
  PlatterInputSnapshot d = state.consumeForFrame();

  bool pass = a.platterTouched && a.wheelScratchHeld && b.wheelScratchHeld && c.wheelScratchHeld && !d.wheelScratchHeld &&
              a.platterGestureRevision == 1 && nearlyEqual(a.platterLagTarget, 120.f) &&
              nearlyEqual(a.platterGestureVelocity, -3.5f);
  return {"Scratch hold countdown + gesture visibility", pass,
          "held flags=" + std::to_string(int(a.wheelScratchHeld)) + std::to_string(int(b.wheelScratchHeld)) +
            std::to_string(int(c.wheelScratchHeld)) + std::to_string(int(d.wheelScratchHeld)) +
            " rev=" + std::to_string(a.platterGestureRevision)};
}

TestResult testWheelDeltaAccumulatesAndClearsOnConsume() {
  PlatterInputState state;
  state.addWheelDelta(1.0f, 2);
  state.addWheelDelta(-0.25f, 2);

  PlatterInputSnapshot first = state.consumeForFrame();
  PlatterInputSnapshot second = state.consumeForFrame();

  bool pass = first.wheelScratchHeld && nearlyEqual(first.wheelDelta, 0.75f) && nearlyEqual(second.wheelDelta, 0.f);
  return {"Wheel delta accumulates then clears", pass,
          "firstDelta=" + std::to_string(first.wheelDelta) + " secondDelta=" + std::to_string(second.wheelDelta)};
}

TestResult testIdleStateHidesGestureFields() {
  PlatterInputState state;
  state.setScratch(false, 42.f, 2.f, 0);
  PlatterInputSnapshot snapshot = state.consumeForFrame();

  bool pass = !snapshot.platterTouched && !snapshot.wheelScratchHeld && !snapshot.platterMotionActive &&
              snapshot.platterGestureRevision == 0 && nearlyEqual(snapshot.platterLagTarget, 0.f) &&
              nearlyEqual(snapshot.platterGestureVelocity, 0.f);
  return {"Idle state hides gesture payload", pass,
          "rev=" + std::to_string(snapshot.platterGestureRevision) + " lag=" + std::to_string(snapshot.platterLagTarget)};
}

TestResult testMotionFreshCountdown() {
  PlatterInputState state;
  state.setMotionFreshSamples(2);
  PlatterInputSnapshot a = state.consumeForFrame();
  PlatterInputSnapshot b = state.consumeForFrame();
  PlatterInputSnapshot c = state.consumeForFrame();

  bool pass = a.platterMotionActive && b.platterMotionActive && !c.platterMotionActive;
  return {"Motion freshness countdown", pass,
          "flags=" + std::to_string(int(a.platterMotionActive)) + std::to_string(int(b.platterMotionActive)) +
            std::to_string(int(c.platterMotionActive))};
}

TestResult testQuickSlipTriggerIsOneShot() {
  PlatterInputState state;
  state.triggerQuickSlipReturn();
  PlatterInputSnapshot first = state.consumeForFrame();
  PlatterInputSnapshot second = state.consumeForFrame();

  bool pass = first.quickSlipTrigger && !second.quickSlipTrigger;
  return {"Quick slip trigger is one-shot", pass,
          "first=" + std::to_string(int(first.quickSlipTrigger)) +
            " second=" + std::to_string(int(second.quickSlipTrigger))};
}

} // namespace

int main() {
  std::vector<TestResult> tests;
  tests.push_back(testScratchHoldCountdownAndGestureVisibility());
  tests.push_back(testWheelDeltaAccumulatesAndClearsOnConsume());
  tests.push_back(testIdleStateHidesGestureFields());
  tests.push_back(testMotionFreshCountdown());
  tests.push_back(testQuickSlipTriggerIsOneShot());

  int failed = 0;
  std::cout << "TemporalDeck Platter Input Spec\n";
  std::cout << "-------------------------------\n";
  for (const auto &t : tests) {
    std::cout << (t.pass ? "[PASS] " : "[FAIL] ") << t.name << " :: " << t.detail << "\n";
    if (!t.pass) {
      failed++;
    }
  }
  std::cout << "-------------------------------\n";
  std::cout << "Summary: " << (tests.size() - failed) << "/" << tests.size() << " passed\n";
  return failed == 0 ? 0 : 1;
}
