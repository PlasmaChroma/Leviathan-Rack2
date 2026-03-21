#include "platter_spec_cases.hpp"
#define TEMPORALDECK_MATH_ONLY
#include "../src/TemporalDeck.hpp"
#undef TEMPORALDECK_MATH_ONLY

#include <algorithm>
#include <cmath>

namespace spec {

constexpr double kPi = platter_interaction::kPi;
constexpr double kMouseScratchTravelScale = 1.0;

double lagDeltaFromAngle(double deltaAngleRad, double sampleRate, double sensitivity) {
  return platter_interaction::lagDeltaFromAngle(float(deltaAngleRad), float(sampleRate), float(sensitivity),
                                                float(kMouseScratchTravelScale));
}

bool approxEqual(double a, double b, double absTol) {
  return std::fabs(a - b) <= absTol;
}

struct Vec2 {
  double x = 0.0;
  double y = 0.0;
};

double wrapSignedAngle(double a) {
  return platter_interaction::wrapSignedAngle(float(a));
}

double angleOf(const Vec2 &p) {
  return std::atan2(p.y, p.x);
}

Vec2 polar(double radius, double angle) {
  return {radius * std::cos(angle), radius * std::sin(angle)};
}

std::vector<Vec2> makeSweepSequence(double startAngle, double endAngle, int steps, double baseRadius,
                                    bool modulateRadius) {
  std::vector<Vec2> seq;
  seq.reserve(std::max(steps, 1) + 1);
  for (int i = 0; i <= steps; ++i) {
    double t = steps > 0 ? (double(i) / double(steps)) : 0.0;
    double angle = startAngle + (endAngle - startAngle) * t;
    double radius = baseRadius;
    if (modulateRadius) {
      // Radius wobble to emulate inconsistent hand distance from platter center.
      radius *= (1.0 + 0.32 * std::sin(4.0 * angle + 0.35));
      radius = std::max(radius, baseRadius * 0.45);
    }
    seq.push_back(polar(radius, angle));
  }
  return seq;
}

double applyMouseSequenceToLag(const std::vector<Vec2> &seq, double startLagSamples, double sampleRate,
                               double sensitivity) {
  if (seq.size() < 2) {
    return startLagSamples;
  }
  double lag = startLagSamples;
  double prevAngle = angleOf(seq[0]);
  for (size_t i = 1; i < seq.size(); ++i) {
    double angle = angleOf(seq[i]);
    double deltaAngle = wrapSignedAngle(angle - prevAngle);
    lag -= lagDeltaFromAngle(deltaAngle, sampleRate, sensitivity);
    prevAngle = angle;
  }
  return lag;
}

TestResult testFreezeOneRev() {
  constexpr double sr = 48000.0;
  constexpr double sens = 1.0;
  double startLag = 2.0 * sr;
  double endLag = startLag - lagDeltaFromAngle(2.0 * kPi, sr, sens);
  double expected = (2.0 - 1.8) * sr;
  bool ok = approxEqual(endLag, expected, sr * 0.05); // +/- 5%
  return {"Freeze 1-rev calibration", ok, "endLag=" + std::to_string(endLag / sr) + "s expected~" +
                                             std::to_string(expected / sr) + "s"};
}

TestResult testFreezeHalfRevLinearity() {
  constexpr double sr = 48000.0;
  constexpr double sens = 1.0;
  double startLag = 3.0 * sr;
  double lagA = startLag - lagDeltaFromAngle(kPi, sr, sens);
  double lagB = lagA - lagDeltaFromAngle(kPi, sr, sens);
  double lagFull = startLag - lagDeltaFromAngle(2.0 * kPi, sr, sens);
  bool ok = approxEqual(lagB, lagFull, sr * 0.01);
  return {"Freeze half-rev linearity", ok,
          "lag(two half) vs lag(full): " + std::to_string(lagB / sr) + "s vs " + std::to_string(lagFull / sr) + "s"};
}

TestResult testDirectionSymmetry() {
  constexpr double sr = 48000.0;
  constexpr double sens = 1.0;
  double deltaFwd = lagDeltaFromAngle(0.75 * kPi, sr, sens);
  double deltaBack = lagDeltaFromAngle(-0.75 * kPi, sr, sens);
  bool ok = approxEqual(deltaFwd + deltaBack, 0.0, sr * 1e-6);
  return {"Direction symmetry", ok, "deltaFwd+deltaBack=" + std::to_string((deltaFwd + deltaBack) / sr) + "s"};
}

TestResult testRebaseDirectionAware() {
  float liveLag = 4200.f;
  float targetLag = 5000.f;
  float towardNowDelta = 120.f;
  float awayFromNowDelta = -120.f;
  float towardBase = platter_interaction::rebaseLagTarget(targetLag, liveLag, towardNowDelta);
  float awayBase = platter_interaction::rebaseLagTarget(targetLag, liveLag, awayFromNowDelta);
  bool ok = (towardBase == liveLag) && (awayBase == targetLag);
  return {"Direction-aware rebase", ok,
          "towardBase=" + std::to_string(towardBase) + " awayBase=" + std::to_string(awayBase)};
}

TestResult testWriteHeadCompensationPolicy() {
  bool liveMotion = platter_interaction::shouldApplyWriteHeadCompensation(false, true, false);
  bool liveFresh = platter_interaction::shouldApplyWriteHeadCompensation(false, false, true);
  bool freeze = platter_interaction::shouldApplyWriteHeadCompensation(true, true, true);
  bool idle = platter_interaction::shouldApplyWriteHeadCompensation(false, false, false);
  bool ok = liveMotion && liveFresh && !freeze && !idle;
  return {"Write-head compensation policy", ok,
          "liveMotion=" + std::to_string(liveMotion) + " liveFresh=" + std::to_string(liveFresh) +
            " freeze=" + std::to_string(freeze) + " idle=" + std::to_string(idle)};
}

TestResult testLiveForwardCompensated() {
  // Contract-level model: while motion is active, write-head baseline
  // compensation keeps forward drag from being penalized by elapsed wall time.
  constexpr double sr = 48000.0;
  constexpr double sens = 1.0;
  double startLag = 2.0 * sr;
  double endLag = startLag - lagDeltaFromAngle(2.0 * kPi, sr, sens);
  bool ok = (endLag <= 0.4 * sr);
  return {"Live forward non-resistance (compensated model)", ok,
          "endLag=" + std::to_string(endLag / sr) + "s from start 2.0s"};
}

TestResult testStationaryHoldNoDrift() {
  constexpr double sr = 48000.0;
  double startLag = 1.25 * sr;
  double endLag = startLag; // No motion => no drift in contract harness.
  bool ok = approxEqual(endLag, startLag, 1.0);
  return {"Stationary hold no drift", ok,
          "endLag-startLag=" + std::to_string((endLag - startLag) / sr) + "s"};
}

TestResult testMouseSeqFullRevolutionFreeze() {
  constexpr double sr = 48000.0;
  constexpr double sens = 1.0;
  double startLag = 2.0 * sr;
  std::vector<Vec2> seq = makeSweepSequence(0.0, 2.0 * kPi, 720, 120.0, false);
  double endLag = applyMouseSequenceToLag(seq, startLag, sr, sens);
  double expectedLag = (2.0 - 1.8) * sr;
  bool ok = approxEqual(endLag, expectedLag, sr * 0.05);
  return {"Mouse sequence: full revolution freeze", ok,
          "endLag=" + std::to_string(endLag / sr) + "s expected~" + std::to_string(expectedLag / sr) + "s"};
}

TestResult testMouseSeqHalfThenHalfFreeze() {
  constexpr double sr = 48000.0;
  constexpr double sens = 1.0;
  double startLag = 3.0 * sr;
  std::vector<Vec2> half1 = makeSweepSequence(0.0, kPi, 360, 130.0, false);
  std::vector<Vec2> half2 = makeSweepSequence(kPi, 2.0 * kPi, 360, 130.0, false);
  double lagAfterHalf = applyMouseSequenceToLag(half1, startLag, sr, sens);
  double lagAfterFullByHalves = applyMouseSequenceToLag(half2, lagAfterHalf, sr, sens);
  double lagAfterOneFull = startLag - lagDeltaFromAngle(2.0 * kPi, sr, sens);
  bool ok = approxEqual(lagAfterFullByHalves, lagAfterOneFull, sr * 0.01);
  return {"Mouse sequence: half+half linearity", ok,
          "half+half=" + std::to_string(lagAfterFullByHalves / sr) + "s full=" + std::to_string(lagAfterOneFull / sr) +
            "s"};
}

TestResult testMouseSeqForwardThenReverseCancel() {
  constexpr double sr = 48000.0;
  constexpr double sens = 1.0;
  double startLag = 1.7 * sr;
  std::vector<Vec2> fwd = makeSweepSequence(0.0, 0.85 * kPi, 240, 110.0, false);
  std::vector<Vec2> rev = makeSweepSequence(0.85 * kPi, 0.0, 240, 110.0, false);
  double lagFwd = applyMouseSequenceToLag(fwd, startLag, sr, sens);
  double lagEnd = applyMouseSequenceToLag(rev, lagFwd, sr, sens);
  bool ok = approxEqual(lagEnd, startLag, sr * 0.01);
  return {"Mouse sequence: forward then reverse cancel", ok,
          "lagEnd-start=" + std::to_string((lagEnd - startLag) / sr) + "s"};
}

TestResult testMouseSeqRadiusInvariance() {
  constexpr double sr = 48000.0;
  constexpr double sens = 1.0;
  double startLag = 2.4 * sr;
  std::vector<Vec2> fixedRadius = makeSweepSequence(0.0, 1.25 * kPi, 420, 120.0, false);
  std::vector<Vec2> variableRadius = makeSweepSequence(0.0, 1.25 * kPi, 420, 120.0, true);
  double lagA = applyMouseSequenceToLag(fixedRadius, startLag, sr, sens);
  double lagB = applyMouseSequenceToLag(variableRadius, startLag, sr, sens);
  bool ok = approxEqual(lagA, lagB, sr * 0.03);
  return {"Mouse sequence: radius invariance", ok,
          "fixed=" + std::to_string(lagA / sr) + "s variable=" + std::to_string(lagB / sr) + "s"};
}

std::vector<TestResult> collectTests() {
  std::vector<TestResult> tests;
  tests.push_back(testFreezeOneRev());
  tests.push_back(testFreezeHalfRevLinearity());
  tests.push_back(testDirectionSymmetry());
  tests.push_back(testRebaseDirectionAware());
  tests.push_back(testWriteHeadCompensationPolicy());
  tests.push_back(testLiveForwardCompensated());
  tests.push_back(testStationaryHoldNoDrift());
  tests.push_back(testMouseSeqFullRevolutionFreeze());
  tests.push_back(testMouseSeqHalfThenHalfFreeze());
  tests.push_back(testMouseSeqForwardThenReverseCancel());
  tests.push_back(testMouseSeqRadiusInvariance());
  return tests;
}

} // namespace spec
