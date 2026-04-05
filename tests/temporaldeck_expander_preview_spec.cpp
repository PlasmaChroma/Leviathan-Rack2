#include "../src/TemporalDeckEngine.hpp"
#include "../src/TemporalDeckExpanderProtocol.hpp"

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

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

TestResult testPreviewQuantizeClamp() {
  using temporaldeck_expander::quantizePreviewSample;
  bool pass = quantizePreviewSample(0.f) == 0 && quantizePreviewSample(10.f) == 32767 &&
              quantizePreviewSample(100.f) == 32767 && quantizePreviewSample(-100.f) == -32767;
  return {"Preview quantization clamps to int16 range", pass,
          "q(0)=" + std::to_string(quantizePreviewSample(0.f)) + " q(100)=" + std::to_string(quantizePreviewSample(100.f))};
}

TestResult testPreviewAccumulatorFinalizeAndWrap() {
  temporaldeck_expander::PreviewAccumulator preview;
  const uint32_t capacity = temporaldeck_expander::PREVIEW_BIN_COUNT * 2u;
  preview.reset(capacity);

  uint32_t samplesPerBin = preview.samplesPerBin;
  for (uint32_t i = 0; i < temporaldeck_expander::PREVIEW_BIN_COUNT * samplesPerBin; ++i) {
    preview.pushMonoSample(0.25f);
  }
  bool full = preview.filledBins == temporaldeck_expander::PREVIEW_BIN_COUNT && preview.writeIndex == 0;

  for (uint32_t i = 0; i < samplesPerBin; ++i) {
    preview.pushMonoSample(-0.5f);
  }
  bool wrapped = preview.filledBins == temporaldeck_expander::PREVIEW_BIN_COUNT && preview.writeIndex == 1;
  bool pass = full && wrapped;
  return {"Preview accumulator finalizes bins and wraps", pass,
          "samplesPerBin=" + std::to_string(samplesPerBin) + " writeIndex=" + std::to_string(preview.writeIndex)};
}

TestResult testPopulateHostMessageCopiesPreviewAndScalars() {
  std::array<temporaldeck_expander::ScopeBin, temporaldeck_expander::SCOPE_BIN_COUNT> scope;
  scope.fill(temporaldeck_expander::makeEmptyScopeBin());
  scope[0].min = -120;
  scope[0].max = 220;
  scope[1].min = -80;
  scope[1].max = 90;

  temporaldeck_expander::HostToDisplay msg;
  temporaldeck_expander::populateHostMessage(&msg, 42u, 7u,
                                             temporaldeck_expander::FLAG_PREVIEW_VALID |
                                               temporaldeck_expander::FLAG_MONO_BUFFER,
                                             48000.f, 321.f, 640.f, 1.25f, 0.5f, 10.f, 0.05f, 9000u, 4500u, 900.f,
                                             1120.f, 1.75f, 2u, scope.data());

  bool pass = msg.magic == temporaldeck_expander::MAGIC && msg.version == temporaldeck_expander::VERSION &&
              msg.publishSeq == 42u && msg.bufferGeneration == 7u && msg.flags != 0u && msg.sampleRate == 48000.f &&
              msg.lagSamples == 321.f && msg.accessibleLagSamples == 640.f && msg.bufferCapacityFrames == 9000u &&
              msg.bufferFilledFrames == 4500u && msg.scopeHalfWindowMs == 900.f && msg.scopeStartLagSamples == 1120.f &&
              msg.scopeBinSpanSamples == 1.75f && msg.scopeBinCount == 2u && msg.scope[0].min == scope[0].min &&
              msg.scope[0].max == scope[0].max && msg.scope[1].min == scope[1].min &&
              msg.scope[1].max == scope[1].max && !temporaldeck_expander::isScopeBinValid(msg.scope[2]);
  return {"Host message population copies scalars + scope bins", pass,
          "publishSeq=" + std::to_string(msg.publishSeq) + " gen=" + std::to_string(msg.bufferGeneration) +
            " scopeBinCount=" + std::to_string(msg.scopeBinCount)};
}

TestResult testEnginePreviewUpdatesOnlyWhenLiveWritesAdvance() {
  const float sr = 48000.f;
  Engine engine;
  engine.reset(sr);

  auto in = makeDefaultInput(sr);
  in.inL = 1.f;
  in.inR = 1.f;

  uint32_t samplesPerBin = engine.preview.samplesPerBin;
  for (uint32_t i = 0; i < samplesPerBin; ++i) {
    engine.process(in);
  }
  bool liveAdvanced = engine.preview.filledBins == 1;

  in.freezeButton = true;
  for (uint32_t i = 0; i < samplesPerBin; ++i) {
    engine.process(in);
  }
  bool freezeHeld = engine.preview.filledBins == 1;

  bool pass = liveAdvanced && freezeHeld;
  return {"Engine preview advances in live mode and holds during freeze", pass,
          "samplesPerBin=" + std::to_string(samplesPerBin) + " filledBins=" + std::to_string(engine.preview.filledBins)};
}

TestResult testSampleInstallPopulatesPreviewBins() {
  Engine engine;
  engine.reset(48000.f);

  const int frames = 128;
  std::vector<float> left(frames, 0.f);
  std::vector<float> right(frames, 0.f);
  for (int i = 0; i < frames; ++i) {
    left[i] = float(i) / float(std::max(1, frames - 1));
    right[i] = -left[i];
  }

  engine.installSample(left, right, frames, true, false);
  bool pass = engine.sampleLoaded && engine.sampleFrames == frames && engine.preview.filledBins > 0 &&
              engine.preview.samplesPerBin >= 1u;
  return {"Installing a sample precomputes expander preview bins", pass,
          "sampleFrames=" + std::to_string(engine.sampleFrames) +
            " filledBins=" + std::to_string(engine.preview.filledBins) +
            " samplesPerBin=" + std::to_string(engine.preview.samplesPerBin)};
}

TestResult testLiveToSampleConversionBumpsGenerationAndPreservesStaticSamplePreview() {
  const float sr = 48000.f;
  Engine engine;
  engine.reset(sr);
  uint64_t genBefore = engine.bufferGeneration;

  auto in = makeDefaultInput(sr);
  in.inL = 0.4f;
  in.inR = 0.4f;
  uint32_t samplesPerBin = engine.preview.samplesPerBin;
  for (uint32_t i = 0; i < samplesPerBin; ++i) {
    engine.process(in);
  }

  bool converted = engine.convertLiveWindowToSample(1.f, true);
  uint64_t genAfter = engine.bufferGeneration;
  bool generationBumped = genAfter > genBefore;
  bool previewPopulated = engine.preview.filledBins > 0;
  uint32_t previewFilledBefore = engine.preview.filledBins;
  uint32_t previewWriteBefore = engine.preview.writeIndex;

  for (uint32_t i = 0; i < std::max(1u, samplesPerBin); ++i) {
    engine.process(in);
  }
  bool sampleModeDoesNotAdvance = engine.preview.filledBins == previewFilledBefore &&
                                  engine.preview.writeIndex == previewWriteBefore;

  bool pass = converted && generationBumped && previewPopulated && sampleModeDoesNotAdvance;
  return {"Live->sample conversion bumps generation and keeps sample preview static", pass,
          "converted=" + std::to_string(int(converted)) + " genBefore=" + std::to_string(genBefore) +
            " genAfter=" + std::to_string(genAfter) + " filledBins=" + std::to_string(engine.preview.filledBins)};
}

} // namespace

int main() {
  std::vector<TestResult> tests;
  tests.push_back(testPreviewQuantizeClamp());
  tests.push_back(testPreviewAccumulatorFinalizeAndWrap());
  tests.push_back(testPopulateHostMessageCopiesPreviewAndScalars());
  tests.push_back(testEnginePreviewUpdatesOnlyWhenLiveWritesAdvance());
  tests.push_back(testSampleInstallPopulatesPreviewBins());
  tests.push_back(testLiveToSampleConversionBumpsGenerationAndPreservesStaticSamplePreview());

  int failed = 0;
  std::cout << "TemporalDeck Expander Preview Spec\n";
  std::cout << "---------------------------------\n";
  for (const auto &t : tests) {
    std::cout << (t.pass ? "[PASS] " : "[FAIL] ") << t.name << " :: " << t.detail << "\n";
    if (!t.pass) {
      failed++;
    }
  }
  std::cout << "---------------------------------\n";
  std::cout << "Summary: " << (tests.size() - failed) << "/" << tests.size() << " passed\n";
  return failed == 0 ? 0 : 1;
}
