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

inline float liveUiRebaseStrength(float liveLag, float sampleRate) {
  float sr = std::max(sampleRate, 1.f);
  float nearLag = sr * 1.0f;
  float deepLag = sr * 4.0f;
  if (liveLag <= nearLag) {
    return 1.0f;
  }
  if (liveLag >= deepLag) {
    return 0.35f;
  }
  float t = (liveLag - nearLag) / std::max(deepLag - nearLag, 1e-6f);
  float tClamped = (t < 0.f) ? 0.f : ((t > 1.f) ? 1.f : t);
  return 1.0f + (0.35f - 1.0f) * tClamped;
}

inline bool hasActiveManualMotion(bool hasFreshGesture, bool motionFresh) {
  return hasFreshGesture || motionFresh;
}

inline bool shouldApplyWriteHeadCompensation(bool freezeState, bool hasFreshGesture, bool motionFresh) {
  return !freezeState && hasActiveManualMotion(hasFreshGesture, motionFresh);
}

} // namespace platter_interaction
