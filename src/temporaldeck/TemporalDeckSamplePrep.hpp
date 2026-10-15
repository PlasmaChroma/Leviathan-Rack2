#pragma once

#include "TemporalDeckEngine.hpp"
#include "codec.hpp"

#include <vector>

namespace temporaldeck {

constexpr float kSampleFileVoltageScale = 5.f;

inline float sampleFileToBufferVoltage(float x) {
  return x * kSampleFileVoltageScale;
}

inline float bufferVoltageToSampleFile(float x) {
  return x / kSampleFileVoltageScale;
}

struct PreparedSampleData {
  std::vector<float> left;
  std::vector<float> right;
  temporaldeck_expander::PreviewAccumulator preview;
  float sampleAbsolutePeakVolts = 0.f;
  uint64_t buildSerial = 0u;
  int buildRequestType = 0;
  int sourceFrames = 0;
  int sourceChannels = 0;
  double workerDecodeMs = 0.0;
  double workerPrepMs = 0.0;
  double workerTotalMs = 0.0;
  int frames = 0;
  int bufferMode = TemporalDeckEngine::BUFFER_DURATION_10S;
  float sampleRate = 44100.f;
  bool truncated = false;
  bool monoStorage = false;
  bool previewValid = false;
  bool valid = false;
};

int chooseSampleBufferMode(const DecodedSampleFile &sample);

bool buildPreparedSample(const DecodedSampleFile &decodedSample, float targetSampleRate, int bufferMode,
                         PreparedSampleData *outPrepared);

bool buildPreparedEmptyBuffer(float targetSampleRate, int bufferMode, PreparedSampleData *outPrepared);

} // namespace temporaldeck
