#pragma once

#include <array>
#include <atomic>
#include <type_traits>

namespace snapshot_transport {

// Latest-value transport for exactly one producer and one consumer thread.
// Intermediate publications may be replaced; this is not an event queue.
// Each thread owns a payload slot, with the third slot exchanged atomically.
// A slow consumer never prevents publication or exposes its payload to writes.
template <typename T>
class SpscLatestSnapshot {
  static_assert(std::is_trivially_copyable<T>::value, "Snapshots must be fixed, value-only payloads");
  static_assert(ATOMIC_INT_LOCK_FREE == 2, "Snapshot exchange requires lock-free unsigned atomics");

public:
  // Producer only. Fixed storage, one payload copy, one atomic exchange.
  void publish(const T& value) {
    slots[producerIndex] = value;
    producerIndex = middle.exchange(producerIndex | kReady, std::memory_order_acq_rel) & kIndexMask;
  }

  // Consumer only, including calls through const module/widget interfaces.
  // The returned reference remains stable until this consumer next calls
  // readLatest(). Callers must finish reading/copying before that next call.
  // Before the first publication, returns a value-initialized payload.
  const T& readLatest() const {
    if (middle.load(std::memory_order_acquire) & kReady) {
      consumerIndex = middle.exchange(consumerIndex, std::memory_order_acq_rel) & kIndexMask;
    }
    return slots[consumerIndex];
  }

private:
  static constexpr unsigned kReady = 4u;
  static constexpr unsigned kIndexMask = 3u;
  std::array<T, 3> slots{};
  unsigned producerIndex = 0u;
  mutable unsigned consumerIndex = 1u;
  mutable std::atomic<unsigned> middle{2u};

  // The exchange's release publishes completed writes, or the consumer's
  // completed reads when returning its old slot. Its acquire lets the next
  // owner access/reuse that slot. Only the consumer clears kReady, so a ready
  // load followed by exchange cannot pick up an older, already-consumed slot.
};

} // namespace snapshot_transport
