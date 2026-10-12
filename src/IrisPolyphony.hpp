#pragma once

#include <algorithm>

namespace iris {

constexpr int kMaximumPolyphony = 16;

inline int outputChannelCount(int vOctChannels, int scanChannels) {
  return std::max(
    1,
    std::min(kMaximumPolyphony, std::max(vOctChannels, scanChannels)));
}

inline int vOctSourceChannel(int outputChannel, int vOctChannels) {
  return vOctChannels > 0 && outputChannel >= 0 && outputChannel < vOctChannels
    ? outputChannel
    : 0;
}

} // namespace iris
