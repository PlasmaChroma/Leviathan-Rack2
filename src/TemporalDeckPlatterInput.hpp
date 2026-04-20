#pragma once

#include <atomic>
#include <cstdint>

namespace temporaldeck {

struct PlatterInputSnapshot {
  bool quickSlipTrigger = false;
  bool platterTouched = false;
  bool platterTouchHoldDirect = false;
  bool wheelScratchHeld = false;
  bool platterMotionActive = false;
  uint32_t platterGestureRevision = 0;
  float platterLagTarget = 0.f;
  float platterGestureVelocity = 0.f;
  float wheelDelta = 0.f;
};

class PlatterInputState {
public:
  void setScratch(bool touched, float lagSamples, float velocitySamples, int holdSamples = 0);
  void setDirectScratch(bool touched, float lagSamples, float velocitySamples);
  void setTouchHold(bool touched, float lagSamples);
  void setMotionFreshSamples(int motionFreshSamples);
  void addWheelDelta(float delta, int holdSamples);
  void triggerQuickSlipReturn();
  void resetAudioHoldState();
  PlatterInputSnapshot consumeForFrame();

private:
  std::atomic<bool> platterTouched{false};
  std::atomic<bool> platterTouchHoldDirect{false};
  std::atomic<uint32_t> platterGestureRevision{0};
  std::atomic<float> platterLagTarget{0.f};
  std::atomic<float> platterGestureVelocity{0.f};
  std::atomic<float> platterWheelDelta{0.f};
  std::atomic<int> platterScratchHoldSamples{0};
  std::atomic<int> platterMotionFreshSamples{0};
  std::atomic<bool> quickSlipTrigger{false};
};

} // namespace temporaldeck
