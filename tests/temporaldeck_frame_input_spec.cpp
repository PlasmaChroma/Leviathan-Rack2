#include "../src/TemporalDeckFrameInput.hpp"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

namespace {

using temporaldeck::PlatterInputSnapshot;
using temporaldeck_frameinput::FrameInputControls;
using temporaldeck_frameinput::SignalInputs;

struct TestResult {
  std::string name;
  bool pass = false;
  std::string detail;
};

bool nearlyEqual(float a, float b, float eps = 1e-6f) {
  return std::fabs(a - b) <= eps;
}

TestResult testGatePosRateMapping() {
  SignalInputs signals;
  signals.inL = 0.25f;
  signals.inR = -0.75f;
  signals.positionCv = 3.5f;
  signals.rateCv = -1.25f;
  signals.rateCvConnected = true;
  signals.freezeGateHigh = true;
  signals.scratchGateHigh = true;
  signals.scratchGateConnected = true;
  signals.positionConnected = true;

  FrameInputControls controls;
  controls.dt = 1.f / 48000.f;
  controls.bufferKnob = 0.6f;
  controls.rateKnob = 0.4f;
  controls.mixKnob = 0.9f;
  controls.feedbackKnob = 0.2f;
  controls.freezeButton = true;
  controls.reverseButton = false;
  controls.slipButton = true;

  PlatterInputSnapshot snapshot;
  auto frame = temporaldeck_frameinput::buildFrameInput(signals, controls, snapshot);

  bool pass = nearlyEqual(frame.inL, signals.inL) && nearlyEqual(frame.inR, signals.inR) &&
              nearlyEqual(frame.positionCv, signals.positionCv) && nearlyEqual(frame.rateCv, signals.rateCv) &&
              frame.rateCvConnected == signals.rateCvConnected && frame.freezeGate == signals.freezeGateHigh &&
              frame.scratchGate == signals.scratchGateHigh && frame.scratchGateConnected == signals.scratchGateConnected &&
              frame.positionConnected == signals.positionConnected;
  return {"Gate/POS/RATE fields map 1:1", pass,
          "freezeGate=" + std::to_string(int(frame.freezeGate)) +
            " scratchGate=" + std::to_string(int(frame.scratchGate)) +
            " posCv=" + std::to_string(frame.positionCv) + " rateCv=" + std::to_string(frame.rateCv)};
}

TestResult testPlatterSnapshotAndButtonsMapping() {
  SignalInputs signals;
  FrameInputControls controls;
  controls.dt = 0.001f;
  controls.bufferKnob = 0.1f;
  controls.rateKnob = 0.9f;
  controls.mixKnob = 0.4f;
  controls.feedbackKnob = 0.7f;
  controls.freezeButton = false;
  controls.reverseButton = true;
  controls.slipButton = true;

  PlatterInputSnapshot snapshot;
  snapshot.quickSlipTrigger = true;
  snapshot.platterTouched = true;
  snapshot.wheelScratchHeld = true;
  snapshot.platterMotionActive = true;
  snapshot.platterGestureRevision = 42;
  snapshot.platterLagTarget = 123.5f;
  snapshot.platterGestureVelocity = -17.25f;
  snapshot.wheelDelta = 0.375f;

  auto frame = temporaldeck_frameinput::buildFrameInput(signals, controls, snapshot);
  bool pass = nearlyEqual(frame.dt, controls.dt) && nearlyEqual(frame.bufferKnob, controls.bufferKnob) &&
              nearlyEqual(frame.rateKnob, controls.rateKnob) && nearlyEqual(frame.mixKnob, controls.mixKnob) &&
              nearlyEqual(frame.feedbackKnob, controls.feedbackKnob) && frame.freezeButton == controls.freezeButton &&
              frame.reverseButton == controls.reverseButton && frame.slipButton == controls.slipButton &&
              frame.quickSlipTrigger == snapshot.quickSlipTrigger && frame.platterTouched == snapshot.platterTouched &&
              frame.wheelScratchHeld == snapshot.wheelScratchHeld &&
              frame.platterMotionActive == snapshot.platterMotionActive &&
              frame.platterGestureRevision == snapshot.platterGestureRevision &&
              nearlyEqual(frame.platterLagTarget, snapshot.platterLagTarget) &&
              nearlyEqual(frame.platterGestureVelocity, snapshot.platterGestureVelocity) &&
              nearlyEqual(frame.wheelDelta, snapshot.wheelDelta);
  return {"Controls + platter snapshot map 1:1", pass,
          "reverse=" + std::to_string(int(frame.reverseButton)) + " slip=" + std::to_string(int(frame.slipButton)) +
            " rev=" + std::to_string(frame.platterGestureRevision) + " wheelDelta=" + std::to_string(frame.wheelDelta)};
}

} // namespace

int main() {
  std::vector<TestResult> tests;
  tests.push_back(testGatePosRateMapping());
  tests.push_back(testPlatterSnapshotAndButtonsMapping());

  int failed = 0;
  std::cout << "TemporalDeck Frame Input Spec\n";
  std::cout << "-----------------------------\n";
  for (const auto &t : tests) {
    std::cout << (t.pass ? "[PASS] " : "[FAIL] ") << t.name << " :: " << t.detail << "\n";
    if (!t.pass) {
      failed++;
    }
  }
  std::cout << "-----------------------------\n";
  std::cout << "Summary: " << (tests.size() - failed) << "/" << tests.size() << " passed\n";
  return failed == 0 ? 0 : 1;
}
