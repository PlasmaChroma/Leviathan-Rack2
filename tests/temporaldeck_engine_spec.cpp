#include "../src/TemporalDeckEngine.hpp"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

namespace {

using Engine = temporaldeck::TemporalDeckEngine;
using Buffer = temporaldeck::TemporalDeckBuffer;

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

std::pair<float, float> directSincReferenceWrapped(const Buffer &buffer, double pos) {
  if (buffer.size <= 0 || buffer.filled <= 0) {
    return {0.f, 0.f};
  }
  pos = buffer.wrapPosition(pos);
  int center = int(std::floor(pos));
  float frac = float(pos - double(center));
  if (std::fabs(frac) <= 1e-6f || std::fabs(1.f - frac) <= 1e-6f) {
    int idx = buffer.wrapIndex(int(std::round(pos)));
    return {buffer.left[idx], buffer.rightSample(idx)};
  }

  float accL = 0.f;
  float accR = 0.f;
  float weightSum = 0.f;
  for (int k = -Buffer::kSincRadius + 1; k <= Buffer::kSincRadius; ++k) {
    int idx = buffer.wrapIndex(center + k);
    float dist = float(k) - frac;
    float w = Buffer::windowedSinc(dist, float(Buffer::kSincRadius));
    accL += buffer.left[idx] * w;
    accR += buffer.rightSample(idx) * w;
    weightSum += w;
  }
  if (std::fabs(weightSum) > 1e-6f) {
    float inv = 1.f / weightSum;
    accL *= inv;
    accR *= inv;
  }
  return {accL, accR};
}

std::pair<float, float> directSincReferenceClamped(const std::vector<float> &left, const std::vector<float> &right, double pos,
                                                   int maxIndex) {
  if (left.empty() || right.empty() || maxIndex < 0) {
    return {0.f, 0.f};
  }
  pos = std::max(0.0, std::min(pos, double(maxIndex)));
  int i1 = int(std::floor(pos));
  float t = float(pos - double(i1));
  if (std::fabs(t) <= 1e-6f || std::fabs(1.f - t) <= 1e-6f) {
    int idx = std::max(0, std::min(int(std::round(pos)), maxIndex));
    return {left[size_t(idx)], right[size_t(idx)]};
  }

  float accL = 0.f;
  float accR = 0.f;
  float weightSum = 0.f;
  for (int k = -Buffer::kSincRadius + 1; k <= Buffer::kSincRadius; ++k) {
    int idx = std::max(0, std::min(i1 + k, maxIndex));
    float dist = float(k) - t;
    float w = Buffer::windowedSinc(dist, float(Buffer::kSincRadius));
    accL += left[size_t(idx)] * w;
    accR += right[size_t(idx)] * w;
    weightSum += w;
  }
  if (std::fabs(weightSum) > 1e-6f) {
    float inv = 1.f / weightSum;
    accL *= inv;
    accR *= inv;
  }
  return {accL, accR};
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

TestResult testLiveFreezeForwardTouchSnapAppliesToReadHead() {
  const float sr = 48000.f;
  Engine engine;
  engine.reset(sr);
  engine.sampleModeEnabled = false;
  engine.sampleLoaded = false;

  auto warm = makeDefaultInput(sr);
  warm.inL = 0.25f;
  warm.inR = -0.1f;
  for (int i = 0; i < 12000; ++i) {
    engine.process(warm);
  }

  const double startLag = 4000.0;
  const double targetLag = 1000.0;
  double newest = engine.newestReadablePos();
  engine.readHead = engine.buffer.wrapPosition(newest - startLag);
  engine.scratchLagSamples = startLag;
  engine.scratchLagTargetSamples = startLag;
  engine.lastPlatterGestureRevision = 0;

  auto in = makeDefaultInput(sr);
  in.freezeButton = true;
  in.platterTouched = true;
  in.platterMotionActive = true;
  in.platterGestureVelocity = 0.f;
  in.platterGestureRevision = 1;
  in.platterLagTarget = float(startLag);
  engine.process(in);

  in.platterGestureRevision = 2;
  in.platterLagTarget = float(targetLag);
  auto out = engine.process(in);

  double err = std::fabs(out.lag - targetLag);
  bool pass = err <= 4.0;
  return {"Live freeze forward touch snap updates read-head", pass,
          "lag=" + std::to_string(out.lag) + " target=" + std::to_string(targetLag) + " err=" + std::to_string(err)};
}

TestResult testLiveTouchUiLikeAlternatingScratchRegressionGuard() {
  const float sr = 48000.f;
  Engine engine;
  engine.reset(sr);
  engine.sampleModeEnabled = false;
  engine.sampleLoaded = false;

  auto in = makeDefaultInput(sr);
  in.inL = 0.12f;
  in.inR = -0.08f;
  for (int i = 0; i < 12000; ++i) {
    engine.process(in);
  }

  double newest = engine.newestReadablePos();
  double localLag = 5000.0;
  engine.readHead = engine.buffer.wrapPosition(newest - localLag);

  double prevEngineLag = engine.currentLagFromNewest(engine.newestReadablePos());
  double maxAbsGap = 0.0;
  double revExtraSum = 0.0;
  double fwdExtraSum = 0.0;
  int revCount = 0;
  int fwdCount = 0;
  uint32_t rev = 0;
  bool reversePhase = true;
  constexpr float kSensitivity = 1.f;
  constexpr float kAngleStep = 0.010f;
  for (int i = 0; i < 160; ++i) {
    if (i > 0 && (i % 16) == 0) {
      reversePhase = !reversePhase;
    }

    float deltaAngle = reversePhase ? -kAngleStep : kAngleStep;
    float lagDelta = platter_interaction::lagDeltaFromAngle(deltaAngle, sr, kSensitivity, Engine::kMouseScratchTravelScale,
                                                            Engine::kNominalPlatterRpm);
    localLag = platter_interaction::rebaseLagTarget(float(localLag), float(prevEngineLag), lagDelta);
    localLag = std::max(0.0, std::min(localLag - double(lagDelta), double(engine.maxLagFromKnob(1.f))));

    in.platterTouched = true;
    in.platterMotionActive = true;
    in.platterGestureRevision = ++rev;
    in.platterLagTarget = float(localLag);
    in.platterGestureVelocity = lagDelta / std::max(in.dt, 1e-6f);
    auto out = engine.process(in);
    double gap = out.lag - localLag;
    maxAbsGap = std::max(maxAbsGap, std::fabs(gap));
    double extra = (out.lag - prevEngineLag) - 1.0; // subtract nominal write-drift
    if (reversePhase) {
      revExtraSum += extra;
      revCount++;
    } else {
      fwdExtraSum += extra;
      fwdCount++;
    }
    prevEngineLag = out.lag;
  }

  double revAvg = revCount > 0 ? (revExtraSum / double(revCount)) : 0.0;
  double fwdAvg = fwdCount > 0 ? (fwdExtraSum / double(fwdCount)) : 0.0;
  bool pass = revAvg > 0.02 && fwdAvg < 0.25 && maxAbsGap < 15000.0;
  return {"Live touch UI-like alternating scratch regression guard", pass,
          "revAvgExtra=" + std::to_string(revAvg) + " fwdAvgExtra=" + std::to_string(fwdAvg) +
            " maxAbsGap=" + std::to_string(maxAbsGap)};
}

TestResult testLiveTouchDirectionalCompensationBalance() {
  const float sr = 48000.f;

  auto runDirection = [&](float deltaAngleSign) {
    Engine engine;
    engine.reset(sr);
    engine.sampleModeEnabled = false;
    engine.sampleLoaded = false;

    auto in = makeDefaultInput(sr);
    in.inL = 0.15f;
    in.inR = -0.1f;
    for (int i = 0; i < 12000; ++i) {
      engine.process(in);
    }

    double newest = engine.newestReadablePos();
    double startLag = 5000.0;
    engine.readHead = engine.buffer.wrapPosition(newest - startLag);
    engine.scratchLagSamples = startLag;
    engine.scratchLagTargetSamples = startLag;
    engine.lastPlatterGestureRevision = 0;

    in = makeDefaultInput(sr);
    in.freezeButton = false;
    in.platterTouched = true;
    in.platterMotionActive = true;

    uint32_t rev = 1;
    double localLag = startLag;
    in.platterGestureRevision = rev;
    in.platterLagTarget = float(localLag);
    in.platterGestureVelocity = 0.f;
    auto out = engine.process(in);
    double prevLag = out.lag;

    const int steps = 120;
    const float deltaAngle = deltaAngleSign * 0.010f;
    double stepExtraSum = 0.0;
    for (int i = 0; i < steps; ++i) {
      float lagDelta = platter_interaction::lagDeltaFromAngle(deltaAngle, sr, 1.f, 1.0f, Engine::kNominalPlatterRpm);
      localLag = platter_interaction::rebaseLagTarget(float(localLag), float(prevLag), lagDelta);
      localLag = std::max(0.0, std::min(localLag - double(lagDelta), double(out.accessibleLag)));

      in.platterGestureRevision = ++rev;
      in.platterLagTarget = float(localLag);
      in.platterGestureVelocity = lagDelta / std::max(in.dt, 1e-6f);
      out = engine.process(in);

      // Strip nominal +1 sample/frame live drift so this captures only
      // direction-dependent scratch compensation behavior.
      stepExtraSum += (out.lag - prevLag) - 1.0;
      prevLag = out.lag;
    }
    return stepExtraSum / double(steps);
  };

  double forwardAvgExtra = runDirection(+1.f);
  double backwardAvgExtra = runDirection(-1.f);
  double imbalance = std::fabs(backwardAvgExtra + forwardAvgExtra);
  bool pass =
    forwardAvgExtra < -0.02 && backwardAvgExtra > 0.02 && imbalance < 0.12;
  return {"Live touch directional compensation remains balanced", pass,
          "fwdAvgExtra=" + std::to_string(forwardAvgExtra) + " backAvgExtra=" + std::to_string(backwardAvgExtra) +
            " imbalance=" + std::to_string(imbalance)};
}

TestResult testConvertLiveWindowToSampleCapturesRedLimitToNow() {
  const float sr = 1000.f;
  Engine engine;
  engine.reset(sr);
  engine.sampleModeEnabled = false;
  engine.sampleLoaded = false;

  for (int i = 0; i < 10; ++i) {
    engine.buffer.write(float(i), float(100 + i));
  }

  bool converted = engine.convertLiveWindowToSample(0.00045f, true);
  bool sizeOk = engine.sampleFrames == 5;
  bool endpointsOk = sizeOk && std::fabs(engine.buffer.left[0] - 5.f) <= 1e-6f &&
                     std::fabs(engine.buffer.left[engine.sampleFrames - 1] - 9.f) <= 1e-6f;
  bool rightOk = sizeOk && !engine.buffer.monoStorage && std::fabs(engine.buffer.right[0] - 105.f) <= 1e-6f &&
                 std::fabs(engine.buffer.right[engine.sampleFrames - 1] - 109.f) <= 1e-6f;
  bool pass = converted && sizeOk && endpointsOk && rightOk && engine.sampleModeEnabled && engine.sampleLoaded &&
              engine.sampleTransportPlaying;
  return {"Convert live -> sample captures red-limit window", pass,
          "converted=" + std::to_string(int(converted)) + " frames=" + std::to_string(engine.sampleFrames) +
            " firstL=" + std::to_string(engine.buffer.left[0]) +
            " lastL=" + std::to_string(engine.buffer.left[std::max(0, engine.sampleFrames - 1)])};
}

TestResult testSincLutMatchesDirectWindowedSincReference() {
  Buffer buffer;
  buffer.reset(1024.f, 1.f, false);
  for (int i = 0; i < buffer.size; ++i) {
    float t = float(i) / std::max(1, buffer.size - 1);
    buffer.left[size_t(i)] =
      std::sin(temporaldeck::kTwoPi * (3.0f * t + 0.13f)) + 0.2f * std::sin(temporaldeck::kTwoPi * 9.0f * t);
    buffer.right[size_t(i)] =
      std::cos(temporaldeck::kTwoPi * (5.0f * t + 0.07f)) + 0.15f * std::sin(temporaldeck::kTwoPi * 11.0f * t);
  }
  buffer.filled = buffer.size;

  float maxAbsErrL = 0.f;
  float maxAbsErrR = 0.f;
  for (int i = 0; i < 500; ++i) {
    double pos = double(i) * 7.137 + 0.381;
    std::pair<float, float> got = buffer.readSinc(pos);
    std::pair<float, float> ref = directSincReferenceWrapped(buffer, pos);
    maxAbsErrL = std::max(maxAbsErrL, std::fabs(got.first - ref.first));
    maxAbsErrR = std::max(maxAbsErrR, std::fabs(got.second - ref.second));
  }

  bool pass = maxAbsErrL < 0.0025f && maxAbsErrR < 0.0025f;
  return {"SINC LUT closely matches direct windowed-sinc (wrapped)", pass,
          "maxErrL=" + std::to_string(maxAbsErrL) + " maxErrR=" + std::to_string(maxAbsErrR)};
}

TestResult testSampleBoundedSincLutMatchesDirectReference() {
  const float sr = 48000.f;
  Engine engine;
  engine.reset(sr);
  const int frames = 512;
  std::vector<float> left(size_t(frames), 0.f);
  std::vector<float> right(size_t(frames), 0.f);
  for (int i = 0; i < frames; ++i) {
    float t = float(i) / std::max(1, frames - 1);
    left[size_t(i)] =
      0.65f * std::sin(temporaldeck::kTwoPi * 4.0f * t) + 0.30f * std::sin(temporaldeck::kTwoPi * 13.0f * t);
    right[size_t(i)] =
      0.72f * std::cos(temporaldeck::kTwoPi * 6.0f * t) + 0.25f * std::sin(temporaldeck::kTwoPi * 17.0f * t);
  }
  engine.installSample(left, right, frames, true, false);
  engine.sampleModeEnabled = true;
  engine.sampleLoopEnabled = false;
  double newestPos = double(frames - 1);

  float maxAbsErrL = 0.f;
  float maxAbsErrR = 0.f;
  for (int i = 0; i < 600; ++i) {
    double pos = (double(i) * 0.853) + 0.231;
    std::pair<float, float> got = engine.readSampleBounded(pos, Engine::SCRATCH_INTERP_SINC, newestPos);
    std::pair<float, float> ref = directSincReferenceClamped(left, right, pos, frames - 1);
    maxAbsErrL = std::max(maxAbsErrL, std::fabs(got.first - ref.first));
    maxAbsErrR = std::max(maxAbsErrR, std::fabs(got.second - ref.second));
  }

  bool pass = maxAbsErrL < 0.0025f && maxAbsErrR < 0.0025f;
  return {"SINC LUT closely matches direct windowed-sinc (sample-bounded)", pass,
          "maxErrL=" + std::to_string(maxAbsErrL) + " maxErrR=" + std::to_string(maxAbsErrR)};
}

} // namespace

int main() {
  std::vector<TestResult> tests;
  tests.push_back(testLiveModeWritesAdvance());
  tests.push_back(testSampleModeDisablesWrites());
  tests.push_back(testSampleTransportStopsAtEndWithoutLoop());
  tests.push_back(testSampleLoopWraps());
  tests.push_back(testLiveFreezeForwardTouchSnapAppliesToReadHead());
  tests.push_back(testLiveTouchUiLikeAlternatingScratchRegressionGuard());
  tests.push_back(testLiveTouchDirectionalCompensationBalance());
  tests.push_back(testConvertLiveWindowToSampleCapturesRedLimitToNow());
  tests.push_back(testSincLutMatchesDirectWindowedSincReference());
  tests.push_back(testSampleBoundedSincLutMatchesDirectReference());

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
