#pragma once

#include <algorithm>
#include <cmath>

namespace platter_interaction {

constexpr float kPi = 3.14159265358979323846f;
constexpr float kNominalPlatterRpm = 33.333333f;

inline float wrapSignedAngle(float angle) {
  while (angle > kPi) {
    angle -= 2.f * kPi;
  }
  while (angle < -kPi) {
    angle += 2.f * kPi;
  }
  return angle;
}

inline float samplesPerRevolution(float sampleRate, float rpm = kNominalPlatterRpm) {
  return sampleRate * (60.f / std::max(rpm, 1e-6f));
}

inline float lagDeltaFromAngle(float deltaAngleRad, float sampleRate, float sensitivity, float travelScale,
                               float rpm = kNominalPlatterRpm) {
  float samplesPerRev = samplesPerRevolution(sampleRate, rpm) * std::max(travelScale, 0.f) * sensitivity;
  return (deltaAngleRad / (2.f * kPi)) * samplesPerRev;
}

inline float rebaseLagTarget(float currentTarget, float liveLag, float lagDelta) {
  if (lagDelta > 0.f) {
    // Toward NOW: keep the more-forward target.
    return std::min(currentTarget, liveLag);
  }
  if (lagDelta < 0.f) {
    // Away from NOW: keep the farther-behind target.
    return std::max(currentTarget, liveLag);
  }
  return liveLag;
}

inline bool hasActiveManualMotion(bool hasFreshGesture, bool motionFresh) {
  return hasFreshGesture || motionFresh;
}

inline bool shouldApplyWriteHeadCompensation(bool freezeState, bool hasFreshGesture, bool motionFresh) {
  return !freezeState && hasActiveManualMotion(hasFreshGesture, motionFresh);
}

} // namespace platter_interaction

