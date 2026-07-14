#pragma once

#include "IrisSourceField.hpp"

#include <atomic>
#include <limits>

namespace nautiloid_iris_expander {

constexpr int kSourceSlotCount = 3;
constexpr uint32_t kWriterClaim = std::numeric_limits<uint32_t>::max();

struct SourceSlot {
  iris::SourceField source;
  std::atomic<uint64_t> generation {0u};
  mutable std::atomic<uint32_t> readers {0u};
};

inline bool acquireSourceSlot(const SourceSlot* slot, uint64_t generation) {
  if (!slot || generation == 0u) return false;
  uint32_t readers = slot->readers.load(std::memory_order_acquire);
  while (true) {
    // A writer claims an otherwise idle slot before changing its SourceField.
    // Never join a claimed slot, and reserve the sentinel value for that claim.
    if (readers == kWriterClaim || readers == kWriterClaim - 1u) return false;
    if (slot->readers.compare_exchange_weak(
          readers, readers + 1u, std::memory_order_acq_rel, std::memory_order_acquire)) {
      break;
    }
  }
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

inline bool claimSourceSlotForWrite(SourceSlot* slot) {
  if (!slot) return false;
  uint32_t expected = 0u;
  return slot->readers.compare_exchange_strong(
    expected, kWriterClaim, std::memory_order_acq_rel, std::memory_order_acquire);
}

inline void releaseSourceSlotWrite(SourceSlot* slot) {
  if (slot) {
    slot->readers.store(0u, std::memory_order_release);
  }
}

} // namespace nautiloid_iris_expander
