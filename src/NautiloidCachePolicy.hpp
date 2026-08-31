#pragma once

#include <cstddef>
#include <cstdint>

namespace nautiloid_cache {

class CompositePublishPolicy {
public:
  static constexpr size_t kInitialUsefulTileCount = 4u;
  static constexpr int64_t kMinimumIntervalMs = 60;

  bool shouldPublishPartial(size_t validTileCount, int64_t nowMs) {
    if (validTileCount < kInitialUsefulTileCount) return false;
    if (published && nowMs - lastPublishMs < kMinimumIntervalMs) return false;
    published = true;
    lastPublishMs = nowMs;
    return true;
  }

private:
  bool published = false;
  int64_t lastPublishMs = 0;
};

} // namespace nautiloid_cache
