#pragma once

#include "TemporalDeckEngine.hpp"
#include "codec.hpp"

#include <vector>

namespace temporaldeck {

struct PreparedSampleData {
  std::vector<float> left;
  std::vector<float> right;
  int frames = 0;
  int bufferMode = TemporalDeckEngine::BUFFER_DURATION_10S;
  float sampleRate = 44100.f;
  bool truncated = false;
  bool autoPlayOnLoad = true;
  bool monoStorage = false;
  bool valid = false;
};

int chooseSampleBufferMode(const DecodedSampleFile &sample);

bool buildPreparedSample(const DecodedSampleFile &decodedSample, float targetSampleRate, int bufferMode,
                         bool autoPlayOnLoad, PreparedSampleData *outPrepared);

} // namespace temporaldeck
