#include "../src/TemporalDeckEngine.hpp"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

namespace temporaldeck_modes {

float realBufferSecondsForMode(int index) {
  switch (index) {
  case temporaldeck::TemporalDeckEngine::BUFFER_DURATION_10S:
    return 11.f;
  case temporaldeck::TemporalDeckEngine::BUFFER_DURATION_20S:
    return 21.f;
  case temporaldeck::TemporalDeckEngine::BUFFER_DURATION_10MIN_STEREO:
  case temporaldeck::TemporalDeckEngine::BUFFER_DURATION_10MIN_MONO:
    return 601.f;
  default:
    return 11.f;
  }
}

float usableBufferSecondsForMode(int index) {
  return std::max(1.f, realBufferSecondsForMode(index) - 1.f);
}

bool isMonoBufferMode(int index) {
  return index == temporaldeck::TemporalDeckEngine::BUFFER_DURATION_10MIN_MONO;
}

} // namespace temporaldeck_modes

namespace {

using Engine = temporaldeck::TemporalDeckEngine;

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
  in.rateKnob = 0.5f; // 1.0x
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

void installRampSample(Engine &engine, int frames) {
  std::vector<float> left(std::max(frames, 1), 0.f);
  for (int i = 0; i < frames; ++i) {
    left[i] = float(i) / std::max(frames - 1, 1);
  }
  engine.installSample(left, left, frames, true, false);
  engine.sampleModeEnabled = true;
  engine.sampleTransportPlaying = true;
}

TestResult testLiveModeWritesAdvance() {
  const float sr = 48000.f;
  Engine engine;
  engine.reset(sr);
  engine.sampleModeEnabled = false;
  engine.sampleLoaded = false;

  auto in = makeDefaultInput(sr);
  in.inL = 0.25f;
  in.inR = -0.5f;

  int beforeWriteHead = engine.buffer.writeHead;
  int beforeFilled = engine.buffer.filled;
  engine.process(in);

  bool writeAdvanced = engine.buffer.writeHead != beforeWriteHead;
  bool filledAdvanced = engine.buffer.filled > beforeFilled;
  bool pass = writeAdvanced && filledAdvanced;
  return {"Live mode advances write head", pass,
          "writeHead " + std::to_string(beforeWriteHead) + " -> " + std::to_string(engine.buffer.writeHead) +
            ", filled " + std::to_string(beforeFilled) + " -> " + std::to_string(engine.buffer.filled)};
}

TestResult testSampleModeDisablesWrites() {
  const float sr = 48000.f;
  Engine engine;
  engine.reset(sr);
  installRampSample(engine, 32);
  engine.sampleLoopEnabled = false;

  auto in = makeDefaultInput(sr);
  in.inL = 1.f;
  in.inR = 1.f;

  int beforeWriteHead = engine.buffer.writeHead;
  int beforeFilled = engine.buffer.filled;
  engine.process(in);

  bool writeUnchanged = engine.buffer.writeHead == beforeWriteHead;
  bool filledUnchanged = engine.buffer.filled == beforeFilled;
  bool pass = writeUnchanged && filledUnchanged;
  return {"Sample mode does not advance write head", pass,
          "writeHead " + std::to_string(beforeWriteHead) + " -> " + std::to_string(engine.buffer.writeHead) +
            ", filled " + std::to_string(beforeFilled) + " -> " + std::to_string(engine.buffer.filled)};
}

TestResult testSampleTransportStopsAtEndWithoutLoop() {
  const float sr = 48000.f;
  const int frames = 16;
  Engine engine;
  engine.reset(sr);
  installRampSample(engine, frames);
  engine.sampleLoopEnabled = false;

  auto in = makeDefaultInput(sr);
  bool sawAutoFreeze = false;
  for (int i = 0; i < 64; ++i) {
    auto out = engine.process(in);
    sawAutoFreeze = sawAutoFreeze || out.autoFreezeRequested;
  }

  double expectedEnd = double(frames - 1);
  bool atEnd = std::fabs(engine.readHead - expectedEnd) <= 1e-4;
  bool pass = atEnd && sawAutoFreeze;
  return {"Sample transport clamps at end (no loop)", pass,
          "readHead=" + std::to_string(engine.readHead) + " expected=" + std::to_string(expectedEnd) +
            ", autoFreeze=" + (sawAutoFreeze ? "true" : "false")};
}

TestResult testSampleLoopWraps() {
  const float sr = 48000.f;
  const int frames = 16;
  Engine engine;
  engine.reset(sr);
  installRampSample(engine, frames);
  engine.sampleLoopEnabled = true;

  auto in = makeDefaultInput(sr);
  bool sawAutoFreeze = false;
  for (int i = 0; i < 40; ++i) {
    auto out = engine.process(in);
    sawAutoFreeze = sawAutoFreeze || out.autoFreezeRequested;
  }

  double expected = 8.0; // 40 steps at 1x from frame 0 over 16-frame loop.
  bool wrapped = std::fabs(engine.readHead - expected) <= 1e-4;
  bool pass = wrapped && !sawAutoFreeze;
  return {"Sample transport wraps with loop enabled", pass,
          "readHead=" + std::to_string(engine.readHead) + " expected=" + std::to_string(expected) +
            ", autoFreeze=" + (sawAutoFreeze ? "true" : "false")};
}

} // namespace

int main() {
  std::vector<TestResult> tests;
  tests.push_back(testLiveModeWritesAdvance());
  tests.push_back(testSampleModeDisablesWrites());
  tests.push_back(testSampleTransportStopsAtEndWithoutLoop());
  tests.push_back(testSampleLoopWraps());

  int failed = 0;
  std::cout << "TemporalDeck Engine Spec\n";
  std::cout << "------------------------\n";
  for (const auto &t : tests) {
    std::cout << (t.pass ? "[PASS] " : "[FAIL] ") << t.name << " :: " << t.detail << "\n";
    if (!t.pass) {
      failed++;
    }
  }
  std::cout << "------------------------\n";
  std::cout << "Summary: " << (tests.size() - failed) << "/" << tests.size() << " passed\n";
  return failed == 0 ? 0 : 1;
}
