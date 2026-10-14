#pragma once

#include <cstdint>

namespace sil {

struct SpectrumSnapshot {
  enum { kFftSize = 2048 };
  alignas(16) float mid[kFftSize] = {};
  alignas(16) float side[kFftSize] = {};
  uint32_t sequence = 0;

  // Audio-owned rings; writePosition is the next write, hence the oldest
  // sample. Publish samples and their revision together in chronological order.
  void capture(const float* midRing, const float* sideRing, int writePosition, uint32_t revision) {
    int out = 0;
    for (int i = writePosition; i < kFftSize; ++i, ++out) {
      mid[out] = midRing[i];
      side[out] = sideRing[i];
    }
    for (int i = 0; i < writePosition; ++i, ++out) {
      mid[out] = midRing[i];
      side[out] = sideRing[i];
    }
    sequence = revision;
  }
};

} // namespace sil
