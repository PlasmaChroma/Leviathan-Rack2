#pragma once

#include <atomic>
#include <cstdint>

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
    bool force) {
    if (!force && currentIrisConsumerDemand() != IrisConsumerDemand::AcceptUpdates) {
      return 0u;
    }

    const uint64_t serial = nextIrisSerial.fetch_add(1u, std::memory_order_acq_rel) + 1u;
    // Preview offers are only pending states. They must not invalidate the
    // preview already using the single Iris worker. Final work takes immediate
    // cancellation authority so interaction completion cannot be delayed by a
    // superseded preview.
    if (phase == InteractionPhase::Final) {
      publishIrisAuthoritySerial(serial);
    }
    return serial;
  }

  // Called by the single Iris worker when a coalesced preview actually becomes
  // active. A newer final request or lifecycle invalidation wins immediately.
  bool activateIrisPreviewSerial(uint64_t serial) {
    if (serial == 0u) return false;
    uint64_t newest = currentIrisAuthoritySerial.load(std::memory_order_acquire);
    while (newest < serial) {
      if (currentIrisAuthoritySerial.compare_exchange_weak(
          newest, serial, std::memory_order_acq_rel, std::memory_order_acquire)) {
        return true;
      }
    }
    return newest == serial;
  }

  void invalidateIrisRequests() {
    const uint64_t serial = nextIrisSerial.fetch_add(1u, std::memory_order_acq_rel) + 1u;
    publishIrisAuthoritySerial(serial);
  }

  bool isCurrentIrisSerial(uint64_t serial) const {
    return serial != 0u &&
      serial == currentIrisAuthoritySerial.load(std::memory_order_acquire);
  }

  uint64_t currentIrisSerial() const {
    return currentIrisAuthoritySerial.load(std::memory_order_acquire);
  }

private:
  void publishIrisAuthoritySerial(uint64_t serial) {
    uint64_t newest = currentIrisAuthoritySerial.load(std::memory_order_acquire);
    while (newest < serial && !currentIrisAuthoritySerial.compare_exchange_weak(
        newest, serial, std::memory_order_acq_rel, std::memory_order_acquire)) {
    }
  }
  std::atomic<int> irisConsumerDemand {int(IrisConsumerDemand::None)};
  std::atomic<uint64_t> nextDisplaySerial {0u};
  std::atomic<uint64_t> nextIrisSerial {0u};
  std::atomic<uint64_t> currentIrisAuthoritySerial {0u};
};

} // namespace nautiloid_requests
