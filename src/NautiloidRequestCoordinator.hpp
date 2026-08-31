#pragma once

#include <atomic>
#include <cstdint>
#include <limits>

namespace nautiloid_requests {

enum class IrisConsumerDemand : uint8_t {
  None = 0,
  RetainImageSource,
  AcceptUpdates,
};

enum class InteractionPhase : uint8_t {
  Preview = 0,
  Final,
};

enum class FallbackTransition : uint8_t {
  None = 0,
  Start,
  Park,
  Wake,
};

// Control-thread policy only. The module owns the threads and applies these
// transitions while holding its lifecycle mutex.
class FallbackLifecyclePolicy {
public:
  FallbackTransition requestActive() {
    if (!workersCreated) {
      workersCreated = true;
      active = true;
      return FallbackTransition::Start;
    }
    if (!active) {
      active = true;
      return FallbackTransition::Wake;
    }
    return FallbackTransition::None;
  }

  FallbackTransition requestParked() {
    if (!active) return FallbackTransition::None;
    active = false;
    return FallbackTransition::Park;
  }

  bool hasWorkers() const { return workersCreated; }
  bool isActive() const { return active; }

private:
  bool workersCreated = false;
  bool active = false;
};

// Rack-independent request policy shared by the module and focused tests.
// Serial zero is reserved for "not submitted".
class Coordinator {
public:
  static constexpr int64_t kIrisPreviewCadenceMs = 140;

  bool setIrisConsumerDemand(IrisConsumerDemand next) {
    int previousValue = irisConsumerDemand.load(std::memory_order_acquire);
    while (previousValue != int(next) && !irisConsumerDemand.compare_exchange_weak(
        previousValue, int(next), std::memory_order_acq_rel, std::memory_order_acquire)) {
    }
    const IrisConsumerDemand previous = IrisConsumerDemand(previousValue);
    if (previous != next &&
        (previous == IrisConsumerDemand::AcceptUpdates ||
         next == IrisConsumerDemand::None)) {
      invalidateIrisRequests();
    }
    return previous != IrisConsumerDemand::AcceptUpdates &&
      next == IrisConsumerDemand::AcceptUpdates;
  }

  IrisConsumerDemand currentIrisConsumerDemand() const {
    return IrisConsumerDemand(irisConsumerDemand.load(std::memory_order_acquire));
  }

  uint64_t allocateDisplaySerial() {
    return nextDisplaySerial.fetch_add(1u, std::memory_order_acq_rel) + 1u;
  }

  bool isNewestDisplaySerial(uint64_t serial) const {
    return serial != 0u &&
      serial == nextDisplaySerial.load(std::memory_order_acquire);
  }

  void invalidateDisplayRequests() {
    nextDisplaySerial.fetch_add(1u, std::memory_order_acq_rel);
  }

  uint64_t allocateIrisSerial(
    InteractionPhase phase,
    bool force,
    int64_t nowMs) {
    if (!force && currentIrisConsumerDemand() != IrisConsumerDemand::AcceptUpdates) {
      return 0u;
    }
    if (phase == InteractionPhase::Preview && !claimPreviewCadence(nowMs)) {
      return 0u;
    }

    const uint64_t serial = nextIrisSerial.fetch_add(1u, std::memory_order_acq_rel) + 1u;
    publishNewestIrisSerial(serial);
    return serial;
  }

  void invalidateIrisRequests() {
    const uint64_t serial = nextIrisSerial.fetch_add(1u, std::memory_order_acq_rel) + 1u;
    publishNewestIrisSerial(serial);
  }

  bool isNewestIrisSerial(uint64_t serial) const {
    return serial != 0u &&
      serial == newestDesiredIrisSerial.load(std::memory_order_acquire);
  }

  uint64_t newestIrisSerial() const {
    return newestDesiredIrisSerial.load(std::memory_order_acquire);
  }

private:
  void publishNewestIrisSerial(uint64_t serial) {
    uint64_t newest = newestDesiredIrisSerial.load(std::memory_order_acquire);
    while (newest < serial && !newestDesiredIrisSerial.compare_exchange_weak(
        newest, serial, std::memory_order_acq_rel, std::memory_order_acquire)) {
    }
  }
  bool claimPreviewCadence(int64_t nowMs) {
    int64_t previous = lastIrisPreviewMs.load(std::memory_order_acquire);
    while (true) {
      if (previous != std::numeric_limits<int64_t>::min() &&
          nowMs - previous < kIrisPreviewCadenceMs) {
        return false;
      }
      if (lastIrisPreviewMs.compare_exchange_weak(
          previous, nowMs, std::memory_order_acq_rel, std::memory_order_acquire)) {
        return true;
      }
    }
  }

  std::atomic<int> irisConsumerDemand {int(IrisConsumerDemand::None)};
  std::atomic<uint64_t> nextDisplaySerial {0u};
  std::atomic<uint64_t> nextIrisSerial {0u};
  std::atomic<uint64_t> newestDesiredIrisSerial {0u};
  std::atomic<int64_t> lastIrisPreviewMs {std::numeric_limits<int64_t>::min()};
};

} // namespace nautiloid_requests
