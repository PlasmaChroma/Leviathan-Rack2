#pragma once

#include "IrisSourceField.hpp"

#include <atomic>

namespace nautiloid_iris_expander {

constexpr int kSourceSlotCount = 3;

struct SourceSlot {
  iris::SourceField source;
  std::atomic<uint64_t> generation {0u};
  mutable std::atomic<uint32_t> readers {0u};
};

inline bool acquireSourceSlot(const SourceSlot* slot, uint64_t generation) {
  if (!slot || generation == 0u) return false;
  slot->readers.fetch_add(1u, std::memory_order_acquire);
  if (slot->generation.load(std::memory_order_acquire) == generation && slot->source.valid()) {
    return true;
  }
  slot->readers.fetch_sub(1u, std::memory_order_release);
  return false;
}

inline void releaseSourceSlot(const SourceSlot* slot) {
  if (slot) {
    slot->readers.fetch_sub(1u, std::memory_order_release);
  }
}

} // namespace nautiloid_iris_expander
