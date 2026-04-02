#include "../src/TemporalDeckEngine.hpp"
#include "../src/TemporalDeckPlatterInput.hpp"
#include "../src/TemporalDeckTransportControl.hpp"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

namespace {

using Engine = temporaldeck::TemporalDeckEngine;
using temporaldeck::PlatterInputSnapshot;
using temporaldeck::PlatterInputState;
using temporaldeck_transport::TransportButtonEvents;
using temporaldeck_transport::TransportButtonResult;
using temporaldeck_transport::TransportControlState;

struct TestResult {
  std::string name;
  bool pass = false;
  std::string detail;
};

Engine::FrameInput makeDefaultInput(float sampleRate) {
  Engine::FrameInput in;
  in.dt = 1.f / std::max(sampleRate, 1.f);
  in.inL = 0.f;
  in.inR = 0.f;
  in.bufferKnob = 1.f;
  in.rateKnob = 0.5f;
  in.mixKnob = 1.f;
  in.feedbackKnob = 0.f;
  in.freezeButton = false;
  in.reverseButton = false;
  in.slipButton = false;
  in.quickSlipTrigger = false;
  in.freezeGate = false;
  in.scratchGate = false;
  in.scratchGateConnected = false;
  in.positionConnected = false;
  in.positionCv = 0.f;
  in.rateCv = 0.f;
  in.rateCvConnected = false;
  in.platterTouched = false;
  in.wheelScratchHeld = false;
  in.platterMotionActive = false;
  in.platterGestureRevision = 0;
  in.platterLagTarget = 0.f;
  in.platterGestureVelocity = 0.f;
  in.wheelDelta = 0.f;
  return in;
}

struct VirtualRig {
  Engine engine;
  PlatterInputState platter;
  TransportControlState transport;

  float sampleRate = 48000.f;
  float bufferKnob = 1.f;
  float rateKnob = 0.5f;
  float mixKnob = 1.f;
  float feedbackKnob = 0.f;
  bool freezeGateHigh = false;
  bool scratchGateHigh = false;
  bool scratchGateConnected = false;
  bool positionConnected = false;
  float positionCv = 0.f;
  float rateCv = 0.f;
  bool rateCvConnected = false;
  bool desiredSampleModeEnabled = false;
  bool sampleLoopEnabled = false;
  uint32_t pendingSeekRevision = 0;
  uint32_t appliedSeekRevision = 0;
  float pendingSeekNormalized = 0.f;

  VirtualRig() {
    engine.reset(sampleRate);
    engine.bufferDurationMode = Engine::BUFFER_DURATION_10S;
  }

  void fillLive(int frames) {
    for (int i = 0; i < frames; ++i) {
      step(0.05f, -0.02f);
    }
  }

  void setScratch(bool touched, float lagSamples, float velocitySamples, int holdSamples = 0) {
    platter.setScratch(touched, lagSamples, velocitySamples, holdSamples);
  }

  void setMotionFreshSamples(int samples) {
    platter.setMotionFreshSamples(samples);
  }

  void addWheelDelta(float delta, int holdSamples) {
    platter.addWheelDelta(delta, holdSamples);
  }

  void triggerQuickSlip() {
    platter.triggerQuickSlipReturn();
  }

  void setSampleSeek(float normalized) {
    pendingSeekNormalized = normalized;
    pendingSeekRevision++;
  }

  void pressButtons(bool freeze, bool reverse, bool slip) {
    TransportButtonEvents events;
    events.freezePressed = freeze;
    events.reversePressed = reverse;
    events.slipPressed = slip;
    TransportButtonResult result = temporaldeck_transport::applyTransportButtonEvents(
      transport, events, desiredSampleModeEnabled, engine.sampleLoaded);
    if (result.forceSampleTransportPlay) {
      engine.sampleTransportPlaying = true;
    }
  }

  void installRampSample(int frames) {
    std::vector<float> left(std::max(frames, 1), 0.f);
    for (int i = 0; i < frames; ++i) {
      left[i] = float(i) / std::max(frames - 1, 1);
    }
    engine.installSample(left, left, frames, true, false);
    desiredSampleModeEnabled = true;
    sampleLoopEnabled = false;
  }

  Engine::FrameResult step(float inL = 0.f, float inR = 0.f) {
    temporaldeck_transport::applyFreezeGateEdge(transport, freezeGateHigh);
    engine.sampleRate = sampleRate;
    engine.slipReturnMode = transport.slipReturnMode;
    engine.sampleModeEnabled = desiredSampleModeEnabled;
    engine.sampleLoopEnabled = sampleLoopEnabled;
    appliedSeekRevision = temporaldeck_transport::applyPendingSampleSeek(engine, appliedSeekRevision, pendingSeekRevision,
                                                                         pendingSeekNormalized, bufferKnob);

    PlatterInputSnapshot snapshot = platter.consumeForFrame();
    Engine::FrameInput in = makeDefaultInput(sampleRate);
    in.inL = inL;
    in.inR = inR;
    in.bufferKnob = bufferKnob;
    in.rateKnob = rateKnob;
    in.mixKnob = mixKnob;
    in.feedbackKnob = feedbackKnob;
    in.freezeButton = transport.freezeLatched;
    in.reverseButton = transport.reverseLatched;
    in.slipButton = transport.slipLatched;
    in.quickSlipTrigger = snapshot.quickSlipTrigger;
    in.freezeGate = freezeGateHigh;
    in.scratchGate = scratchGateHigh;
    in.scratchGateConnected = scratchGateConnected;
    in.positionConnected = positionConnected;
    in.positionCv = positionCv;
    in.rateCv = rateCv;
    in.rateCvConnected = rateCvConnected;
    in.platterTouched = snapshot.platterTouched;
    in.wheelScratchHeld = snapshot.wheelScratchHeld;
    in.platterMotionActive = snapshot.platterMotionActive;
    in.platterGestureRevision = snapshot.platterGestureRevision;
    in.platterLagTarget = snapshot.platterLagTarget;
    in.platterGestureVelocity = snapshot.platterGestureVelocity;
    in.wheelDelta = snapshot.wheelDelta;

    Engine::FrameResult out = engine.process(in);
    temporaldeck_transport::applyAutoFreezeRequest(transport, out.autoFreezeRequested, freezeGateHigh);
    desiredSampleModeEnabled = engine.sampleModeEnabled;
    return out;
  }
};

TestResult testDragGesturePath() {
  VirtualRig rig;
  rig.fillLive(4096);
  double beforeLag = rig.step().lag;

  rig.setScratch(true, 1800.f, -120.f, 2);
  rig.setMotionFreshSamples(2);
  Engine::FrameResult out = rig.step();

  bool pass = rig.engine.scratchActive && (std::fabs(out.lag - beforeLag) > 1e-3 || std::fabs(rig.engine.scratchLagSamples) > 1e-3);
  return {"Drag gesture updates live scratch path", pass,
          "scratchActive=" + std::to_string(int(rig.engine.scratchActive)) + " lagBefore=" + std::to_string(beforeLag) +
            " lagAfter=" + std::to_string(out.lag)};
}

TestResult testWheelScratchPath() {
  VirtualRig rig;
  rig.fillLive(4096);

  rig.addWheelDelta(-2.0f, 2);
  rig.step();
  bool activeA = rig.engine.scratchActive;
  rig.step();
  bool activeB = rig.engine.scratchActive;
  rig.step();
  bool activeC = rig.engine.scratchActive;

  bool pass = activeA && activeB && !activeC;
  return {"Wheel scratch drives scratch state", pass,
          "activeA=" + std::to_string(int(activeA)) + " activeB=" + std::to_string(int(activeB)) +
            " activeC=" + std::to_string(int(activeC))};
}

TestResult testQuickSlipTriggerPath() {
  VirtualRig rig;
  rig.fillLive(4096);
  double newest = rig.engine.newestReadablePos();
  rig.engine.readHead = rig.engine.buffer.wrapPosition(newest - 3000.0);
  double lagBeforeQuickSlip = rig.engine.currentLagFromNewest(rig.engine.newestReadablePos());

  rig.triggerQuickSlip();
  rig.step();
  float overrideA = rig.engine.slipReturnOverrideTime;
  rig.step();
  float overrideB = rig.engine.slipReturnOverrideTime;

  bool pass = lagBeforeQuickSlip > 64.0 && rig.engine.slipReturning && overrideA >= 0.f && overrideB <= overrideA;
  return {"Quick-slip trigger engages return one-shot path", pass,
          "lagBefore=" + std::to_string(lagBeforeQuickSlip) + " slipReturning=" + std::to_string(int(rig.engine.slipReturning)) +
            " overrideA=" + std::to_string(overrideA) + " overrideB=" + std::to_string(overrideB)};
}

TestResult testFreezeReverseInteractionPath() {
  VirtualRig rig;
  rig.pressButtons(true, false, false);
  bool freezeLatchedAfterPress = rig.transport.freezeLatched;

  rig.freezeGateHigh = true;
  rig.step();
  rig.freezeGateHigh = false;
  rig.step();
  bool freezeClearedOnGateFall = !rig.transport.freezeLatched;

  rig.installRampSample(128);
  rig.engine.sampleTransportPlaying = false;
  rig.pressButtons(false, true, false);
  bool reverseLatched = rig.transport.reverseLatched;
  bool freezeClearedByReverse = !rig.transport.freezeLatched;
  bool forcePlay = rig.engine.sampleTransportPlaying;

  bool pass = freezeLatchedAfterPress && freezeClearedOnGateFall && reverseLatched && freezeClearedByReverse && forcePlay;
  return {"Freeze/reverse edge interactions remain consistent", pass,
          "freezePressed=" + std::to_string(int(freezeLatchedAfterPress)) +
            " freezeFallingEdgeClear=" + std::to_string(int(freezeClearedOnGateFall)) +
            " reverseLatched=" + std::to_string(int(reverseLatched)) +
            " forcePlay=" + std::to_string(int(forcePlay))};
}

TestResult testSampleModeTransitionAndSeekPath() {
  VirtualRig rig;
  rig.bufferKnob = 0.5f;
  rig.installRampSample(200);

  Engine::FrameResult sampleOn = rig.step();
  rig.setSampleSeek(0.9f);
  rig.step();
  double seekedPlayhead = rig.engine.samplePlayhead;
  double seekMax = double(199) * 0.5;

  rig.desiredSampleModeEnabled = false;
  Engine::FrameResult sampleOff = rig.step();
  rig.desiredSampleModeEnabled = true;
  Engine::FrameResult sampleOnAgain = rig.step();

  bool seekClamped = seekedPlayhead <= seekMax + 1e-3 && seekedPlayhead >= 0.0;
  bool pass = sampleOn.sampleMode && sampleOn.sampleLoaded && seekClamped && !sampleOff.sampleMode && sampleOnAgain.sampleMode;
  return {"Sample mode transition + seek clamp path", pass,
          "modeOn=" + std::to_string(int(sampleOn.sampleMode)) + " modeOff=" + std::to_string(int(sampleOff.sampleMode)) +
            " modeOnAgain=" + std::to_string(int(sampleOnAgain.sampleMode)) +
            " seeked=" + std::to_string(seekedPlayhead) + " seekMax=" + std::to_string(seekMax)};
}

TestResult testEdgeFreezeGateFallingClearsOnlyButtonLatch() {
  VirtualRig rig;
  rig.pressButtons(true, false, false);
  rig.freezeGateHigh = true;
  rig.step();
  rig.freezeGateHigh = false;
  rig.step();
  bool buttonLatchCleared = !rig.transport.freezeLatched && !rig.transport.freezeLatchedByButton;

  temporaldeck_transport::applyAutoFreezeRequest(rig.transport, true, false);
  rig.freezeGateHigh = true;
  rig.step();
  rig.freezeGateHigh = false;
  rig.step();
  bool autoLatchPreserved = rig.transport.freezeLatched && !rig.transport.freezeLatchedByButton;

  bool pass = buttonLatchCleared && autoLatchPreserved;
  return {"Edge: freeze gate falling edge button-vs-auto behavior", pass,
          "buttonLatchCleared=" + std::to_string(int(buttonLatchCleared)) +
            " autoLatchPreserved=" + std::to_string(int(autoLatchPreserved))};
}

TestResult testEdgeSampleLoopToggleAtEndHandling() {
  VirtualRig rig;
  rig.installRampSample(32);
  rig.sampleLoopEnabled = false;

  for (int i = 0; i < 128; ++i) {
    rig.step();
  }
  bool frozeAtEnd = rig.transport.freezeLatched;
  bool clampedAtEnd = std::fabs(rig.engine.readHead - 31.0) <= 1e-3;

  rig.sampleLoopEnabled = true;
  rig.pressButtons(true, false, false); // unfreeze
  rig.step();
  for (int i = 0; i < 40; ++i) {
    rig.step();
  }
  bool movedFromEnd = std::fabs(rig.engine.readHead - 31.0) > 1e-3;
  bool notRefrozen = !rig.transport.freezeLatched;

  bool pass = frozeAtEnd && clampedAtEnd && movedFromEnd && notRefrozen;
  return {"Edge: sample loop off/on handling at sample end", pass,
          "frozeAtEnd=" + std::to_string(int(frozeAtEnd)) + " clampedAtEnd=" + std::to_string(int(clampedAtEnd)) +
            " movedFromEnd=" + std::to_string(int(movedFromEnd)) + " notRefrozen=" + std::to_string(int(notRefrozen))};
}

TestResult testEdgeQuickSlipOneShotSemantics() {
  VirtualRig rig;
  rig.fillLive(4096);
  double newest = rig.engine.newestReadablePos();
  rig.engine.readHead = rig.engine.buffer.wrapPosition(newest - 3000.0);

  rig.triggerQuickSlip();
  rig.step();
  float overrideA = rig.engine.slipReturnOverrideTime;
  rig.step();
  float overrideB = rig.engine.slipReturnOverrideTime;
  rig.step();
  float overrideC = rig.engine.slipReturnOverrideTime;
  bool monotonicDecay = overrideA > overrideB && overrideB > overrideC;

  // Re-arm requires a new trigger; verify that retrigger raises the override.
  newest = rig.engine.newestReadablePos();
  rig.engine.readHead = rig.engine.buffer.wrapPosition(newest - 3000.0);
  rig.triggerQuickSlip();
  rig.step();
  float overrideD = rig.engine.slipReturnOverrideTime;
  bool retriggered = overrideD > overrideC;

  bool pass = rig.engine.slipReturning && monotonicDecay && retriggered;
  return {"Edge: quick-slip one-shot semantics", pass,
          "A=" + std::to_string(overrideA) + " B=" + std::to_string(overrideB) + " C=" + std::to_string(overrideC) +
            " D=" + std::to_string(overrideD)};
}

TestResult testEdgeSampleSeekDuringTransportStateChanges() {
  VirtualRig rig;
  rig.bufferKnob = 0.5f;
  rig.installRampSample(200);
  rig.step();

  rig.transport.slipLatched = true;
  rig.engine.slipReturning = true;
  rig.engine.slipBlendActive = true;
  rig.engine.nowCatchActive = true;

  rig.setSampleSeek(0.95f);
  rig.pressButtons(false, true, false);
  rig.step();

  double expected = 199.0 * 0.5;
  bool seekApplied = rig.appliedSeekRevision == rig.pendingSeekRevision && rig.engine.samplePlayhead <= expected + 1e-3 &&
                     rig.engine.samplePlayhead >= 0.0;
  bool seekReadHeadAligned = rig.engine.readHead <= expected + 1.0 && rig.engine.readHead >= -1e-3;
  bool transientCleared = !rig.engine.slipReturning && !rig.engine.slipBlendActive && !rig.engine.nowCatchActive;
  bool reverseSet = rig.transport.reverseLatched;

  bool pass = seekApplied && seekReadHeadAligned && transientCleared && reverseSet;
  return {"Edge: sample seek while transport state changes", pass,
          "seekApplied=" + std::to_string(int(seekApplied)) + " transientCleared=" + std::to_string(int(transientCleared)) +
            " reverseSet=" + std::to_string(int(reverseSet))};
}

} // namespace

int main() {
  std::vector<TestResult> tests;
  tests.push_back(testDragGesturePath());
  tests.push_back(testWheelScratchPath());
  tests.push_back(testQuickSlipTriggerPath());
  tests.push_back(testFreezeReverseInteractionPath());
  tests.push_back(testSampleModeTransitionAndSeekPath());
  tests.push_back(testEdgeFreezeGateFallingClearsOnlyButtonLatch());
  tests.push_back(testEdgeSampleLoopToggleAtEndHandling());
  tests.push_back(testEdgeQuickSlipOneShotSemantics());
  tests.push_back(testEdgeSampleSeekDuringTransportStateChanges());

  int failed = 0;
  std::cout << "TemporalDeck Virtual Integration Spec\n";
  std::cout << "-------------------------------------\n";
  for (const auto &t : tests) {
    std::cout << (t.pass ? "[PASS] " : "[FAIL] ") << t.name << " :: " << t.detail << "\n";
    if (!t.pass) {
      failed++;
    }
  }
  std::cout << "-------------------------------------\n";
  std::cout << "Summary: " << (tests.size() - failed) << "/" << tests.size() << " passed\n";
  return failed == 0 ? 0 : 1;
}
