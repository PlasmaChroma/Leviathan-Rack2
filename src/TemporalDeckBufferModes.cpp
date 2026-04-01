#include "TemporalDeckBufferModes.hpp"

#include "TemporalDeck.hpp"

#include <algorithm>

namespace temporaldeck_modes {

float realBufferSecondsForMode(int index) {
  switch (index) {
  case TemporalDeck::BUFFER_DURATION_10S:
    return 11.f;
  case TemporalDeck::BUFFER_DURATION_20S:
    return 21.f;
  case TemporalDeck::BUFFER_DURATION_10M_STEREO:
  case TemporalDeck::BUFFER_DURATION_10M_MONO:
    return 601.f;
  default:
    return 11.f;
  }
}

float usableBufferSecondsForMode(int index) { return std::max(1.f, realBufferSecondsForMode(index) - 1.f); }

bool isMonoBufferMode(int index) { return index == TemporalDeck::BUFFER_DURATION_10M_MONO; }

} // namespace temporaldeck_modes
