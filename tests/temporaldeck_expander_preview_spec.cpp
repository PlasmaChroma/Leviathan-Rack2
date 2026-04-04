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
  temporaldeck_expander::PreviewAccumulator preview;
  preview.reset(temporaldeck_expander::PREVIEW_BIN_COUNT);
  preview.pushMonoSample(-1.f);
  temporaldeck_expander::HostToDisplay msg;
  temporaldeck_expander::populateHostMessage(&msg, 42u, 7u,
                                             temporaldeck_expander::FLAG_PREVIEW_VALID |
                                               temporaldeck_expander::FLAG_MONO_BUFFER,
                                             48000.f, 321.f, 640.f, 1.25f, 0.5f, 10.f, 0.05f, 9000u, 4500u, preview);

  bool pass = msg.magic == temporaldeck_expander::MAGIC && msg.version == temporaldeck_expander::VERSION &&
              msg.publishSeq == 42u && msg.bufferGeneration == 7u && msg.flags != 0u && msg.sampleRate == 48000.f &&
              msg.lagSamples == 321.f && msg.accessibleLagSamples == 640.f && msg.bufferCapacityFrames == 9000u &&
              msg.bufferFilledFrames == 4500u && msg.samplesPerBin == preview.samplesPerBin &&
              msg.previewWriteIndex == preview.writeIndex && msg.previewFilledBins == preview.filledBins &&
              msg.preview[0].min == preview.bins[0].min && msg.preview[0].max == preview.bins[0].max;
  return {"Host message population copies scalars + preview", pass,
          "publishSeq=" + std::to_string(msg.publishSeq) + " gen=" + std::to_string(msg.bufferGeneration) +
            " filledBins=" + std::to_string(msg.previewFilledBins)};
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

TestResult testLiveToSampleConversionBumpsGenerationAndStopsPreviewWrites() {
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
  bool previewReset = engine.preview.filledBins == 0;

  for (uint32_t i = 0; i < samplesPerBin; ++i) {
    engine.process(in);
  }
  bool sampleModeDoesNotAdvance = engine.preview.filledBins == 0;

  bool pass = converted && generationBumped && previewReset && sampleModeDoesNotAdvance;
  return {"Live->sample conversion bumps generation and pauses preview accumulation", pass,
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
  tests.push_back(testLiveToSampleConversionBumpsGenerationAndStopsPreviewWrites());

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
