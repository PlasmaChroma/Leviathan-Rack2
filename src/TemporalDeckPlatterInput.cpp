#include "TemporalDeckPlatterInput.hpp"

#include <algorithm>
#include <cmath>

namespace temporaldeck {

void PlatterInputState::setScratch(bool touched, float lagSamples, float velocitySamples, int holdSamples) {
  platterTouched.store(touched, std::memory_order_relaxed);
  platterTouchHoldDirect.store(false, std::memory_order_relaxed);
  platterGestureRevision.fetch_add(1, std::memory_order_relaxed);
  platterLagTarget.store(lagSamples, std::memory_order_relaxed);
  platterGestureVelocity.store(velocitySamples, std::memory_order_relaxed);
  platterScratchHoldSamples.store(std::max(0, holdSamples), std::memory_order_relaxed);
  if (touched || holdSamples == 0) {
    platterWheelDelta.store(0.f, std::memory_order_relaxed);
  }
}

void PlatterInputState::setTouchHold(bool touched, float lagSamples) {
  platterTouched.store(touched, std::memory_order_relaxed);
  platterTouchHoldDirect.store(touched, std::memory_order_relaxed);
  platterLagTarget.store(lagSamples, std::memory_order_relaxed);
  platterGestureVelocity.store(0.f, std::memory_order_relaxed);
  platterMotionFreshSamples.store(0, std::memory_order_relaxed);
  if (touched) {
    platterWheelDelta.store(0.f, std::memory_order_relaxed);
  }
}

void PlatterInputState::setMotionFreshSamples(int motionFreshSamples) {
  platterMotionFreshSamples.store(std::max(0, motionFreshSamples), std::memory_order_relaxed);
}

void PlatterInputState::addWheelDelta(float delta, int holdSamples) {
  float expected = platterWheelDelta.load(std::memory_order_relaxed);
  while (!platterWheelDelta.compare_exchange_weak(expected, expected + delta, std::memory_order_relaxed,
                                                  std::memory_order_relaxed)) {
  }
  platterScratchHoldSamples.store(std::max(0, holdSamples), std::memory_order_relaxed);
}

void PlatterInputState::triggerQuickSlipReturn() {
  quickSlipTrigger.store(true, std::memory_order_relaxed);
}

void PlatterInputState::resetAudioHoldState() {
  platterScratchHoldSamples.store(0, std::memory_order_relaxed);
  platterMotionFreshSamples.store(0, std::memory_order_relaxed);
}

PlatterInputSnapshot PlatterInputState::consumeForFrame() {
  PlatterInputSnapshot snapshot;
  snapshot.quickSlipTrigger = quickSlipTrigger.exchange(false, std::memory_order_relaxed);

  int scratchHold = platterScratchHoldSamples.load(std::memory_order_relaxed);
  snapshot.wheelScratchHeld = scratchHold > 0;
  if (snapshot.wheelScratchHeld) {
    platterScratchHoldSamples.store(std::max(0, scratchHold - 1), std::memory_order_relaxed);
  }

  int motionFresh = platterMotionFreshSamples.load(std::memory_order_relaxed);
  snapshot.platterMotionActive = motionFresh > 0;
  if (snapshot.platterMotionActive) {
    platterMotionFreshSamples.store(std::max(0, motionFresh - 1), std::memory_order_relaxed);
  }

  float wheelDelta = platterWheelDelta.load(std::memory_order_relaxed);
  if (std::fabs(wheelDelta) > 1e-9f) {
    snapshot.wheelDelta = platterWheelDelta.exchange(0.f, std::memory_order_relaxed);
  } else {
    snapshot.wheelDelta = 0.f;
  }

  snapshot.platterTouched = platterTouched.load(std::memory_order_relaxed);
  snapshot.platterTouchHoldDirect = platterTouchHoldDirect.load(std::memory_order_relaxed);
  if (snapshot.platterTouched || snapshot.wheelScratchHeld || snapshot.platterMotionActive) {
    snapshot.platterGestureRevision = platterGestureRevision.load(std::memory_order_relaxed);
    snapshot.platterLagTarget = platterLagTarget.load(std::memory_order_relaxed);
    snapshot.platterGestureVelocity = platterGestureVelocity.load(std::memory_order_relaxed);
  }
  return snapshot;
}

} // namespace temporaldeck
