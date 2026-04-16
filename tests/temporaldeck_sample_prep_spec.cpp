#include "../src/TemporalDeckSamplePrep.hpp"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

namespace {

using temporaldeck::DecodedSampleFile;
using temporaldeck::PreparedSampleData;
using temporaldeck::TemporalDeckEngine;

struct TestResult {
  std::string name;
  bool pass = false;
  std::string detail;
};

bool nearlyEqual(float a, float b, float eps = 1e-5f) {
  return std::fabs(a - b) <= eps;
}

TestResult testChooseBufferModeByChannelCount() {
  DecodedSampleFile mono;
  mono.channels = 1;
  DecodedSampleFile stereo;
  stereo.channels = 2;

  int monoMode = temporaldeck::chooseSampleBufferMode(mono);
  int stereoMode = temporaldeck::chooseSampleBufferMode(stereo);

  bool pass = monoMode == TemporalDeckEngine::BUFFER_DURATION_10MIN_MONO &&
              stereoMode == TemporalDeckEngine::BUFFER_DURATION_10MIN_STEREO;
  return {"Choose mode by channel count", pass,
          "monoMode=" + std::to_string(monoMode) + " stereoMode=" + std::to_string(stereoMode)};
}

TestResult testBuildPreparedSampleMonoFoldDown() {
  DecodedSampleFile decoded;
  decoded.channels = 2;
  decoded.frames = 4;
  decoded.sampleRate = 48000.f;
  decoded.left = {1.f, 0.f, -1.f, 0.5f};
  decoded.right = {0.5f, -0.5f, 0.5f, -0.5f};

  PreparedSampleData prepared;
  bool ok = temporaldeck::buildPreparedSample(decoded, 48000.f, TemporalDeckEngine::BUFFER_DURATION_10MIN_MONO, &prepared);

  bool foldMatches = prepared.left.size() == 4 && nearlyEqual(prepared.left[0], 3.75f) &&
                     nearlyEqual(prepared.left[1], -1.25f) && nearlyEqual(prepared.left[2], -1.25f) &&
                     nearlyEqual(prepared.left[3], 0.f);
  bool pass = ok && prepared.valid && prepared.monoStorage && prepared.right.empty() && prepared.frames == 4 &&
              foldMatches;
  return {"Mono fold-down + scaling", pass,
          "frames=" + std::to_string(prepared.frames) + " first=" +
            (prepared.left.empty() ? std::string("n/a") : std::to_string(prepared.left[0]))};
}

TestResult testBuildPreparedSampleTruncatesToBufferLimit() {
  DecodedSampleFile decoded;
  decoded.channels = 1;
  decoded.frames = 1000;
  decoded.sampleRate = 10.f;
  decoded.left.assign(decoded.frames, 0.1f);

  PreparedSampleData prepared;
  bool ok = temporaldeck::buildPreparedSample(decoded, 10.f, TemporalDeckEngine::BUFFER_DURATION_10S, &prepared);

  int expectedMax = int(temporaldeck_modes::usableBufferSecondsForMode(TemporalDeckEngine::BUFFER_DURATION_10S) * 10.f);
  bool pass = ok && prepared.valid && prepared.truncated && prepared.frames == expectedMax &&
              int(prepared.left.size()) == expectedMax;
  return {"Truncate to selected buffer duration", pass,
          "frames=" + std::to_string(prepared.frames) + " expected=" + std::to_string(expectedMax)};
}

TestResult testInvalidInputClearsPreparedOutput() {
  DecodedSampleFile decoded;
  decoded.channels = 1;
  decoded.frames = 0;
  decoded.sampleRate = 48000.f;
  decoded.left.clear();

  PreparedSampleData prepared;
  prepared.valid = true;
  prepared.frames = 123;
  bool ok = temporaldeck::buildPreparedSample(decoded, 48000.f, TemporalDeckEngine::BUFFER_DURATION_10S, &prepared);

  bool pass = !ok && !prepared.valid && prepared.frames == 0 && prepared.left.empty() && prepared.right.empty();
  return {"Invalid input returns false + resets output", pass,
          "ok=" + std::to_string(int(ok)) + " frames=" + std::to_string(prepared.frames)};
}

} // namespace

int main() {
  std::vector<TestResult> tests;
  tests.push_back(testChooseBufferModeByChannelCount());
  tests.push_back(testBuildPreparedSampleMonoFoldDown());
  tests.push_back(testBuildPreparedSampleTruncatesToBufferLimit());
  tests.push_back(testInvalidInputClearsPreparedOutput());

  int failed = 0;
  std::cout << "TemporalDeck Sample Prep Spec\n";
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
