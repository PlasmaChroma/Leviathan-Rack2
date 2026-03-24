#pragma once

#include <string>
#include <vector>

namespace temporaldeck {

struct DecodedSampleFile {
  std::vector<float> left;
  std::vector<float> right;
  int channels = 0;
  int frames = 0;
  float sampleRate = 0.f;
  bool truncated = false;
};

bool decodeSampleFile(const std::string &path, DecodedSampleFile *out, std::string *errorOut = nullptr);

} // namespace temporaldeck
