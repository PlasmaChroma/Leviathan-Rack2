#pragma once

#include <algorithm>
#include <cmath>
#include <utility>

namespace temporaldeck {
namespace scratch_smoothing {
constexpr float kTransitionTime = 0.004f;
constexpr float kStreamRecoveryTime = 0.005f;
constexpr float kHoldDelay = 0.008f;
constexpr float kHoldFadeOut = 0.008f;
constexpr float kHoldFadeIn = 0.003f;
constexpr double kStoppedTravel = 0.001;
constexpr double kMovingTravel = 0.002;
constexpr double kJumpTravel = 2.0;

// Runtime-only audio continuity state. Never moves or delays the read head.
struct State {
  enum class Source { None, Hand, DirectHold, Scope, Wheel, ExternalCv };
  // A stream recovery owns the single transition until it finishes. Scratch
  // events cannot restart it; a newer stream seek may replace its destination.
  void requestTransition(bool streamRecovery = false) {
    if (!streamRecovery && streamTransition && (transitionPending || transitionRemaining > 0.f)) return;
    transitionPending = true;
    streamTransition = streamRecovery;
    transitionDuration = streamRecovery ? kStreamRecoveryTime : kTransitionTime;
  }
  bool streamTransition = false;
  float transitionDuration = kTransitionTime;
  bool transitionPending = false;
  float transitionRemaining = 0.f;
  float transitionFromL = 0.f;
  float transitionFromR = 0.f;
  float stationaryTime = 0.f;
  float holdFade = 1.f;

  Source source = Source::None;
  int motionDirection = 0;
  bool motionJump = false;
  double trackedDelta = 0.0;

  void trackSource(Source nextSource) {
    if (nextSource != source) {
      requestTransition();
      motionDirection = 0;
      motionJump = false;
      trackedDelta = 0.0;
    }
    source = nextSource;
  }

  void trackMotion(double delta) {
    // These are playhead events, never tests of audio amplitude. A source drum
    // transient at steady read speed must not trigger a fade.
    int direction = motionDirection;
    if (std::fabs(delta) < kStoppedTravel) direction = 0;
    else if (delta > kMovingTravel) direction = 1;
    else if (delta < -kMovingTravel) direction = -1;
    bool jump = std::fabs(delta - trackedDelta) > kJumpTravel;
    if (direction != motionDirection || (jump && !motionJump)) {
      requestTransition();
    }
    motionDirection = direction;
    motionJump = jump;
    trackedDelta = delta;
  }

  std::pair<float, float> applyHold(std::pair<float, float> wet, bool scratching,
                                             double readDelta, float dt, bool enabled = true) {
    if (!enabled) {
      stationaryTime = 0.f;
      holdFade = 1.f;
      return wet;
    }
    if (!scratching && holdFade == 1.f) {
      stationaryTime = 0.f;
      return wet;
    }
    // Require a sustained stop, not a single zero-speed sample at a reversal.
    // Use actual head travel so held CV that still plays audio stays audible.
    constexpr float holdDelay = kHoldDelay;
    if (scratching && std::fabs(readDelta) < kStoppedTravel) {
      stationaryTime = std::min(holdDelay, stationaryTime + dt);
    } else {
      stationaryTime = 0.f;
    }
    bool held = scratching && stationaryTime >= holdDelay;
    if (held) {
      holdFade = std::max(0.f, holdFade - dt / kHoldFadeOut);
    } else {
      holdFade = std::min(1.f, holdFade + dt / kHoldFadeIn);
    }
    float gain = holdFade * holdFade * (3.f - 2.f * holdFade);
    wet.first *= gain;
    wet.second *= gain;
    return wet;
  }

  std::pair<float, float> applyTransition(std::pair<float, float> wet, float dt, float previousL = 0.f, float previousR = 0.f, bool enabled = true) {
    if (!enabled && !streamTransition) {
      transitionPending = false;
      transitionRemaining = 0.f;
      return wet;
    }
    const float duration = transitionDuration;
    if (transitionPending) {
      transitionPending = false;
      transitionFromL = previousL;
      transitionFromR = previousR;
      transitionRemaining = duration;
    }
    if (transitionRemaining > 0.f) {
      // Remove the handoff discontinuity after all scratch-only processing.
      // Crossfade from the last audible sample without feeding an additive
      // correction back through scratch continuity filters. No extra reads.
      float t = std::max(0.f, std::min(transitionRemaining / duration, 1.f));
      float weight = t * t * (3.f - 2.f * t);
      wet.first = (wet.first + (transitionFromL - wet.first) * weight);
      wet.second = (wet.second + (transitionFromR - wet.second) * weight);
      transitionRemaining = std::max(0.f, transitionRemaining - dt);
      if (transitionRemaining == 0.f) streamTransition = false;
    }
    return wet;
  }
  std::pair<float, float> process(std::pair<float, float> wet, bool scratching,
                                double travel, float dt, float previousL, float previousR, bool enabled) {
    wet = applyHold(wet, scratching, travel, dt, enabled);
    return applyTransition(wet, dt, previousL, previousR, enabled);
  }
};
} // namespace scratch_smoothing
} // namespace temporaldeck
