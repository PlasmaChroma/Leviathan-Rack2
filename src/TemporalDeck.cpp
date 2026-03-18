#include "plugin.hpp"
#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <limits>
#include <regex>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace {

struct TemporalDeckBuffer {
  std::vector<float> left;
  std::vector<float> right;
  int size = 0;
  int writeHead = 0;
  int filled = 0;
  float sampleRate = 44100.f;

  void reset(float sr) {
    sampleRate = sr;
    size = std::max(1, int(std::round(sampleRate * 9.f)));
    left.assign(size, 0.f);
    right.assign(size, 0.f);
    writeHead = 0;
    filled = 0;
  }

  int wrapIndex(int index) const {
    if (size <= 0) {
      return 0;
    }
    index %= size;
    if (index < 0) {
      index += size;
    }
    return index;
  }

  float wrapPosition(float pos) const {
    if (size <= 0) {
      return 0.f;
    }
    pos = std::fmod(pos, float(size));
    if (pos < 0.f) {
      pos += float(size);
    }
    return pos;
  }

  void write(float inL, float inR) {
    if (size <= 0) {
      return;
    }
    left[writeHead] = inL;
    right[writeHead] = inR;
    writeHead = wrapIndex(writeHead + 1);
    filled = std::min(filled + 1, size);
  }

  static float cubicSample(float y0, float y1, float y2, float y3, float t) {
    float a0 = y3 - y2 - y0 + y1;
    float a1 = y0 - y1 - a0;
    float a2 = y2 - y0;
    float a3 = y1;
    return ((a0 * t + a1) * t + a2) * t + a3;
  }

  static float lagrange6Sample(const std::array<float, 6> &y, float t) {
    static constexpr float kNodes[6] = {-2.f, -1.f, 0.f, 1.f, 2.f, 3.f};
    float sum = 0.f;
    for (int j = 0; j < 6; ++j) {
      float weight = 1.f;
      for (int m = 0; m < 6; ++m) {
        if (m == j) {
          continue;
        }
        weight *= (t - kNodes[m]) / (kNodes[j] - kNodes[m]);
      }
      sum += y[j] * weight;
    }
    return sum;
  }

  std::pair<float, float> readCubic(float pos) const {
    if (size <= 0 || filled <= 0) {
      return {0.f, 0.f};
    }
    pos = wrapPosition(pos);
    int i1 = int(std::floor(pos));
    float t = pos - float(i1);
    int i0 = wrapIndex(i1 - 1);
    int i2 = wrapIndex(i1 + 1);
    int i3 = wrapIndex(i1 + 2);
    return {cubicSample(left[i0], left[i1], left[i2], left[i3], t),
            cubicSample(right[i0], right[i1], right[i2], right[i3], t)};
  }

  std::pair<float, float> readLinear(float pos) const {
    if (size <= 0 || filled <= 0) {
      return {0.f, 0.f};
    }
    pos = wrapPosition(pos);
    int i0 = int(pos);
    int i1 = wrapIndex(i0 + 1);
    float t = pos - float(i0);
    return {crossfade(left[i0], left[i1], t),
            crossfade(right[i0], right[i1], t)};
  }

  std::pair<float, float> readHighQuality(float pos) const {
    if (size <= 0 || filled <= 0) {
      return {0.f, 0.f};
    }
    pos = wrapPosition(pos);
    int i2 = int(std::floor(pos));
    float t = pos - float(i2);
    int i0 = wrapIndex(i2 - 2);
    int i1 = wrapIndex(i2 - 1);
    int i3 = wrapIndex(i2 + 1);
    int i4 = wrapIndex(i2 + 2);
    int i5 = wrapIndex(i2 + 3);
    std::array<float, 6> l = {left[i0], left[i1], left[i2],
                              left[i3], left[i4], left[i5]};
    std::array<float, 6> r = {right[i0], right[i1], right[i2],
                              right[i3], right[i4], right[i5]};
    return {lagrange6Sample(l, t), lagrange6Sample(r, t)};
  }
};

struct TemporalDeckEngine {
  static constexpr float kScratchGateThreshold = 1.f;
  static constexpr float kFreezeGateThreshold = 1.f;
  static constexpr float kSlipReturnTime = 0.12f;
  static constexpr float kSlipEnableReturnThreshold = 64.f;
  static constexpr float kSlipFinalCatchThresholdMs = 120.f;
  static constexpr float kSlipFinalCatchTime = 0.035f;
  static constexpr float kScratchFollowTime = 0.012f;
  static constexpr float kScratchSoftLagStepMin = 6.0f;
  static constexpr float kScratchSoftLagStepMax = 28.0f;
  static constexpr float kSlowScratchVelThreshold = 28.0f;
  static constexpr float kSlowScratchSmoothingTime = 0.0075f;
  static constexpr float kScratchTargetJitterThreshold = 0.35f;
  static constexpr float kReverseBiteVelocityThreshold = 22.0f;
  static constexpr float kReverseBiteMaxBoost = 1.55f;
  static constexpr float kNowSnapThresholdMs = 20.0f;
  static constexpr float kNowCatchTime = 0.004f;
  static constexpr float kMouseScratchTravelScale = 4.0f;
  static constexpr float kWheelScratchTravelScale = 4.5f;
  static constexpr float kInertiaBlend = 0.25f;
  static constexpr float kNominalPlatterRpm = 33.333333f;

  enum CartridgeCharacter {
    CARTRIDGE_CLEAN,
    CARTRIDGE_CLASSIC,
    CARTRIDGE_BATTLE,
    CARTRIDGE_LOFI,
    CARTRIDGE_COUNT
  };

  struct OnePoleState {
    float z = 0.f;

    void reset() { z = 0.f; }

    float lowpass(float in, float coeff) {
      z += coeff * (in - z);
      return z;
    }
  };

  struct CartridgeChannelState {
    OnePoleState rumble;
    OnePoleState body;
    OnePoleState air;

    void reset() {
      rumble.reset();
      body.reset();
      air.reset();
    }
  };

  struct CartridgeParams {
    float hpHz = 0.f;
    float bodyHz = 0.f;
    float lpHz = 0.f;
    float lpMotionHz = 0.f;
    float bodyGain = 0.f;
    float presenceGain = 0.f;
    float crossfeed = 0.f;
    float drive = 1.f;
    float stereoTilt = 0.f;

    CartridgeParams() {}

    CartridgeParams(float hpHz, float bodyHz, float lpHz, float lpMotionHz,
                    float bodyGain, float presenceGain, float crossfeed,
                    float drive, float stereoTilt)
        : hpHz(hpHz), bodyHz(bodyHz), lpHz(lpHz), lpMotionHz(lpMotionHz),
          bodyGain(bodyGain), presenceGain(presenceGain),
          crossfeed(crossfeed), drive(drive), stereoTilt(stereoTilt) {}
  };

  TemporalDeckBuffer buffer;
  float sampleRate = 44100.f;
  float readHead = 0.f;
  float timelineHead = 0.f;
  float platterPhase = 0.f;
  float platterVelocity = 0.f;
  bool freezeState = false;
  bool reverseState = false;
  bool slipState = false;
  bool highQualityScratchInterpolation = true;
  bool scratchActive = false;
  bool slipReturning = false;
  bool slipFinalCatchActive = false;
  bool nowCatchActive = false;
  float slipReturnRemaining = 0.f;
  float slipReturnStartLag = 0.f;
  float nowCatchRemaining = 0.f;
  float nowCatchStartLag = 0.f;
  float scratchLagSamples = 0.f;
  float scratchLagTargetSamples = 0.f;
  float filteredManualLagTargetSamples = 0.f;
  float lastPlatterLagTarget = 0.f;
  uint32_t lastPlatterGestureRevision = 0;
  int cartridgeCharacter = CARTRIDGE_CLEAN;
  CartridgeChannelState cartridgeLeft;
  CartridgeChannelState cartridgeRight;

  void reset(float sr) {
    sampleRate = sr;
    buffer.reset(sr);
    readHead = 0.f;
    timelineHead = 0.f;
    platterPhase = 0.f;
    platterVelocity = 0.f;
    scratchActive = false;
    slipReturning = false;
    slipFinalCatchActive = false;
    nowCatchActive = false;
    slipReturnRemaining = 0.f;
    slipReturnStartLag = 0.f;
    nowCatchRemaining = 0.f;
    nowCatchStartLag = 0.f;
    scratchLagSamples = 0.f;
    scratchLagTargetSamples = 0.f;
    filteredManualLagTargetSamples = 0.f;
    lastPlatterLagTarget = 0.f;
    cartridgeLeft.reset();
    cartridgeRight.reset();
  }

  float maxLagFromKnob(float knob) const {
    return clamp(knob, 0.f, 1.f) * sampleRate * 8.f;
  }

  float accessibleLag(float knob) const {
    return std::min(maxLagFromKnob(knob), float(buffer.filled));
  }

  float clampLag(float lag, float limit) const {
    return clamp(lag, 0.f, std::max(0.f, limit));
  }

  static float baseSpeedFromKnob(float rateKnob) {
    rateKnob = clamp(rateKnob, 0.f, 1.f);
    // Knob-only range is 0.5x .. 2.0x, centered at 1.0x.
    if (rateKnob < 0.5f) {
      float t = rateKnob / 0.5f;
      return 0.5f + t * 0.5f;
    }
    float t = (rateKnob - 0.5f) / 0.5f;
    return 1.f + t;
  }

  float computeBaseSpeed(float rateKnob, float rateCv, bool reverse) const {
    float speed = baseSpeedFromKnob(rateKnob);
    speed += clamp(rateCv / 5.f, -1.f, 1.f);
    speed = clamp(speed, -3.f, 3.f);
    if (reverse) {
      speed *= -1.f;
    }
    return speed;
  }

  float lagForPositionCv(float cv, float limit) const {
    float normalized = clamp(std::fabs(cv) / 10.f, 0.f, 1.f);
    return normalized * limit;
  }

  float onePoleCoeff(float hz) const {
    hz = clamp(hz, 0.f, sampleRate * 0.45f);
    if (hz <= 0.f) {
      return 0.f;
    }
    return 1.f - std::exp(-2.f * float(M_PI) * hz / std::max(sampleRate, 1.f));
  }

  static CartridgeParams paramsForCartridge(int mode) {
    switch (mode) {
    case CARTRIDGE_CLASSIC:
      return {35.f, 1600.f, 12500.f, 11800.f, 0.10f, 0.08f, 0.025f, 1.03f,
              0.02f};
    case CARTRIDGE_BATTLE:
      return {55.f, 1900.f, 14500.f, 9200.f, 0.03f, 0.20f, 0.015f, 1.08f,
              0.03f};
    case CARTRIDGE_LOFI:
      return {90.f, 1250.f, 6800.f, 3800.f, 0.22f, -0.10f, 0.05f, 1.18f,
              0.08f};
    case CARTRIDGE_CLEAN:
    default:
      return {};
    }
  }

  std::pair<float, float> applyCartridgeCharacter(std::pair<float, float> in,
                                                  float motionAmount) {
    if (cartridgeCharacter == CARTRIDGE_CLEAN) {
      return in;
    }

    const CartridgeParams p = paramsForCartridge(cartridgeCharacter);
    motionAmount = clamp(motionAmount, 0.f, 1.f);

    float lpHz = p.lpHz + (p.lpMotionHz - p.lpHz) * motionAmount;
    float lpHzL = lpHz * (1.f - p.stereoTilt);
    float lpHzR = lpHz * (1.f + p.stereoTilt);
    float hpCoeff = onePoleCoeff(p.hpHz);
    float bodyCoeff = onePoleCoeff(p.bodyHz);
    float lpCoeffL = onePoleCoeff(lpHzL);
    float lpCoeffR = onePoleCoeff(lpHzR);

    auto processChannel = [&](float x, CartridgeChannelState& state,
                              float lpCoeff) {
      float rumble = state.rumble.lowpass(x, hpCoeff);
      float hp = x - rumble;
      float body = state.body.lowpass(hp, bodyCoeff);
      float air = state.air.lowpass(hp, lpCoeff);
      float presence = air - body;
      float voiced = air + p.bodyGain * body + p.presenceGain * presence;
      if (p.drive > 1.f) {
        float norm = std::tanh(p.drive);
        voiced = std::tanh(voiced * p.drive) / std::max(norm, 1e-6f);
      }
      return voiced;
    };

    float left = processChannel(in.first, cartridgeLeft, lpCoeffL);
    float right = processChannel(in.second, cartridgeRight, lpCoeffR);
    float xfeed = clamp(p.crossfeed, 0.f, 0.45f);
    if (xfeed > 0.f) {
      float mixedL = left * (1.f - xfeed) + right * xfeed;
      float mixedR = right * (1.f - xfeed) + left * xfeed;
      left = mixedL;
      right = mixedR;
    }
    return {left, right};
  }

  float currentLag() const {
    if (buffer.size <= 0) {
      return 0.f;
    }
    float lag = newestReadablePos() - readHead;
    if (lag < 0.f) {
      lag += float(buffer.size);
    }
    return lag;
  }

  float newestReadablePos() const {
    if (buffer.size <= 0 || buffer.filled <= 0) {
      return 0.f;
    }
    int newest = buffer.writeHead - 1;
    if (newest < 0) {
      newest += buffer.size;
    }
    return float(newest);
  }

  float platterRadiansPerSample() const {
    return (2.f * float(M_PI) * (kNominalPlatterRpm / 60.f)) /
           std::max(sampleRate, 1.f);
  }

  float samplesPerPlatterRadian() const {
    return 1.f / std::max(platterRadiansPerSample(), 1e-9f);
  }

  float currentLagFromNewest(float newestPos) const {
    if (buffer.size <= 0) {
      return 0.f;
    }
    float lag = newestPos - readHead;
    if (lag < 0.f) {
      lag += float(buffer.size);
    }
    return lag;
  }

  float unwrapReadNearWrite(float readPos, float writePos) const {
    if (buffer.size <= 0) {
      return readPos;
    }
    float sizeF = float(buffer.size);
    while (readPos > writePos) {
      readPos -= sizeF;
    }
    while (readPos <= writePos - sizeF) {
      readPos += sizeF;
    }
    return readPos;
  }

  struct FrameResult {
    float outL = 0.f;
    float outR = 0.f;
    float lag = 0.f;
    float accessibleLag = 0.f;
    float platterAngle = 0.f;
  };

  FrameResult process(float dt, float inL, float inR, float bufferKnob,
                      float rateKnob, float mixKnob, float feedbackKnob,
                      bool freezeButton, bool reverseButton, bool slipButton,
                      bool freezeGate, bool scratchGate,
                      bool scratchGateConnected, bool positionConnected,
                      float positionCv, float rateCv, bool platterTouched,
                      bool wheelScratchHeld, bool platterMotionActive,
                      uint32_t platterGestureRevision, float platterLagTarget,
                      float platterGestureVelocity, float wheelDelta) {
    FrameResult result;
    float prevReadHead = readHead;
    float nowSnapThresholdSamples = sampleRate * (kNowSnapThresholdMs / 1000.f);
    bool pinToNow = false;
    bool keepSlipLagAligned = false;
    bool keepNowCatchLagAligned = false;
    freezeState = freezeButton || freezeGate;
    reverseState = reverseButton;
    bool prevSlipState = slipState;
    slipState = slipButton;

    float limit = accessibleLag(bufferKnob);
    float minLag = 0.f;
    float maxLag = std::max(limit, 0.f);
    float baseSpeed = computeBaseSpeed(rateKnob, rateCv, reverseState);
    float speed = baseSpeed;
    bool externalScratch =
        scratchGateConnected && scratchGate && positionConnected;
    bool positionFollow = positionConnected && !scratchGateConnected;
    bool manualTouchScratch = platterTouched;
    bool wheelScratch = wheelScratchHeld;
    bool manualScratch = manualTouchScratch || wheelScratch;
    bool anyScratch = externalScratch || manualScratch;
    bool wasScratchActive = scratchActive;
    bool releasedFromScratch = !anyScratch && wasScratchActive;
    bool slipJustEnabled = slipState && !prevSlipState;
    float newestPos = newestReadablePos();
    bool hasFreshPlatterGesture =
        platterGestureRevision != lastPlatterGestureRevision;
    auto startNowCatch = [&](float startLag) {
      nowCatchActive = true;
      nowCatchRemaining = kNowCatchTime;
      nowCatchStartLag = std::max(0.f, startLag);
    };

    if (!wasScratchActive && anyScratch) {
      scratchLagSamples = currentLagFromNewest(newestPos);
      scratchLagTargetSamples = scratchLagSamples;
      filteredManualLagTargetSamples = scratchLagSamples;
      lastPlatterLagTarget = platterLagTarget;
      lastPlatterGestureRevision = platterGestureRevision;
    }
    scratchActive = anyScratch;

    if (!slipState) {
      slipReturning = false;
      slipFinalCatchActive = false;
    }

    if (releasedFromScratch && slipState) {
      slipReturning = true;
      slipFinalCatchActive = false;
      slipReturnRemaining = 0.f;
    }

    if (releasedFromScratch &&
        currentLagFromNewest(newestPos) <= nowSnapThresholdSamples) {
      startNowCatch(currentLagFromNewest(newestPos));
      slipReturning = false;
      slipFinalCatchActive = false;
    }

    if (slipJustEnabled && !anyScratch) {
      if (currentLagFromNewest(newestPos) > kSlipEnableReturnThreshold) {
        slipReturning = true;
        slipFinalCatchActive = false;
      }
    }

    if (anyScratch) {
      slipReturning = false;
      slipFinalCatchActive = false;
    }

    // 3. Determine actual playhead (readHead)
    if (freezeState) {
      speed = 0.f;
    }

    float lagNow = currentLagFromNewest(newestPos);
    bool reverseAtOldestEdge = !scratchActive && !slipReturning &&
                               reverseState && limit > 0.f &&
                               lagNow >= (limit - 0.5f);
    if (reverseAtOldestEdge && speed < 0.f) {
      speed = 0.f;
    }

    if (manualScratch) {
      if (manualTouchScratch) {
        // Manual mouse scratching is intentionally event-driven.
        //
        // We tried bridging sparse UI drag events by continuing to follow the
        // previous drag target between updates, but that caused two
        // regressions:
        // 1. click-and-hold would drift instead of freezing
        // 2. slow reverse drags would still creep clockwise because the live
        //    write point kept moving while the old target stayed latched
        //
        // The rule here is strict:
        // - fresh gesture revision: move to the new requested lag
        // - no fresh gesture revision: hold the current audio position
        //
        // Be careful changing this. Any "helpful" smoothing between gesture
        // updates needs to be validated against stationary hold and very slow
        // reverse drag behavior.
        if (hasFreshPlatterGesture) {
          if (nowCatchActive && platterLagTarget > nowSnapThresholdSamples) {
            nowCatchActive = false;
          }
          float rawTargetLag = clampLag(platterLagTarget, limit);
          float velMag = std::fabs(platterGestureVelocity);
          if (velMag < kSlowScratchVelThreshold) {
            float alpha = clamp(dt / std::max(kSlowScratchSmoothingTime, 1e-6f),
                                0.f, 1.f);
            filteredManualLagTargetSamples =
                filteredManualLagTargetSamples +
                (rawTargetLag - filteredManualLagTargetSamples) * alpha;
            if (std::fabs(rawTargetLag - filteredManualLagTargetSamples) <
                kScratchTargetJitterThreshold) {
              filteredManualLagTargetSamples = rawTargetLag;
            }
          } else {
            filteredManualLagTargetSamples = rawTargetLag;
          }
          scratchLagTargetSamples =
              clampLag(filteredManualLagTargetSamples, limit);
          if (scratchLagTargetSamples <= nowSnapThresholdSamples) {
            startNowCatch(std::max(scratchLagSamples, scratchLagTargetSamples));
          }
          lastPlatterGestureRevision = platterGestureRevision;
        }
        // A mouse-down with no recent platter motion is a true freeze, not a
        // coast.
        bool stationaryManualHold =
            !platterMotionActive && !hasFreshPlatterGesture;
        if (stationaryManualHold) {
          platterVelocity = 0.f;
          readHead = prevReadHead;
          scratchLagSamples = currentLagFromNewest(newestPos);
          scratchLagTargetSamples = scratchLagSamples;
        } else {
          // During active drag, chase the requested lag with a bounded step.
          // This preserves stationary hold while removing zipper/click
          // artifacts from event-rate target jumps.
          float lagError = scratchLagTargetSamples - scratchLagSamples;
          float velMag = std::fabs(platterGestureVelocity);
          bool snappedSlow =
              velMag < kSlowScratchVelThreshold &&
              std::fabs(lagError) < kScratchTargetJitterThreshold;
          if (snappedSlow) {
            scratchLagSamples = scratchLagTargetSamples;
            platterVelocity = 0.f;
            readHead = buffer.wrapPosition(newestPos - scratchLagSamples);
          } else {
            float followProgress =
                clamp(dt / std::max(kScratchFollowTime, 1e-6f), 0.f, 1.f);
            float shapedFollow = 1.f - std::pow(1.f - followProgress, 2.2f);
            float lagStep = lagError * shapedFollow;
            bool backwardScratch = lagError > 0.f;
            float dynamicSoftLimit = clamp(
                kScratchSoftLagStepMin +
                    std::fabs(platterGestureVelocity) * 0.003f +
                    std::fabs(lagError) * 0.08f,
                kScratchSoftLagStepMin,
                kScratchSoftLagStepMax * (backwardScratch ? 2.5f : 1.35f));
            lagStep = dynamicSoftLimit *
                      std::tanh(lagStep / std::max(dynamicSoftLimit, 1e-6f));
            if (backwardScratch && velMag > kReverseBiteVelocityThreshold) {
              float biteT =
                  clamp((velMag - kReverseBiteVelocityThreshold) /
                            std::max(kReverseBiteVelocityThreshold, 1e-6f),
                        0.f, 1.f);
              float biteBoost = 1.f + (kReverseBiteMaxBoost - 1.f) * biteT;
              lagStep *= biteBoost;
            }
            scratchLagSamples = clampLag(scratchLagSamples + lagStep, limit);
            platterVelocity = lagStep;
            readHead = buffer.wrapPosition(newestPos - scratchLagSamples);
          }
        }
      } else {
        // Wheel scratch accumulates target lag per scroll event, then glides
        // toward that target with SLIP-like easing while wheel-hold is active.
        float wheelDeltaSoftRange =
            sampleRate * 0.16f * kWheelScratchTravelScale;
        float wheelDeltaShaped =
            wheelDeltaSoftRange *
            std::tanh(wheelDelta / std::max(wheelDeltaSoftRange, 1e-6f));
        if (wheelDeltaShaped < 0.f) {
          wheelDeltaShaped *= 2.0f;
        }
        // Rebase from current lag only when a new wheel event arrives to avoid
        // directional drift without collapsing the target between events.
        if (std::fabs(wheelDelta) > 1e-6f) {
          // Direction-aware base avoids "fighting" the glide when moving
          // toward NOW with repeated small forward scrolls.
          float baseLag =
              wheelDeltaShaped < 0.f
                  ? std::min(scratchLagSamples, scratchLagTargetSamples)
                  : std::max(scratchLagSamples, scratchLagTargetSamples);
          scratchLagTargetSamples = clampLag(baseLag + wheelDeltaShaped, limit);
          float wheelNowSnapThreshold = sampleRate * 0.012f;
          if (scratchLagTargetSamples < wheelNowSnapThreshold) {
            scratchLagTargetSamples = 0.f;
          }
        }

        float lagError = scratchLagTargetSamples - scratchLagSamples;
        bool movingTowardNow = lagError < 0.f;
        float wheelFollowTime =
            movingTowardNow ? (kSlipReturnTime * 0.5f) : kSlipReturnTime;
        float alpha = dt / std::max(wheelFollowTime, 1e-6f);
        float lagStep = lagError * alpha;

        // Keep progression audible even for tiny alpha / small errors.
        float minStep = movingTowardNow ? 0.8f : 0.35f;
        if (std::fabs(lagError) > minStep && std::fabs(lagStep) < minStep) {
          lagStep = std::copysign(minStep, lagError);
        }

        // Symmetric glide cap in both directions to avoid directional bias.
        float maxStep = kScratchSoftLagStepMax * 1.6f;
        lagStep = clamp(lagStep, -maxStep, maxStep);

        if (std::fabs(lagError) <= 0.5f) {
          scratchLagSamples = scratchLagTargetSamples;
        } else {
          scratchLagSamples = clampLag(scratchLagSamples + lagStep, limit);
        }
        float wheelNowSnapThreshold = sampleRate * 0.010f;
        if (movingTowardNow && scratchLagSamples < wheelNowSnapThreshold) {
          scratchLagSamples = 0.f;
          scratchLagTargetSamples = 0.f;
        }
        readHead = buffer.wrapPosition(newestPos - scratchLagSamples);
      }
      lastPlatterLagTarget = platterLagTarget;
    } else if (externalScratch) {
      scratchLagSamples = lagForPositionCv(positionCv, limit);
      scratchLagTargetSamples = scratchLagSamples;
      readHead = buffer.wrapPosition(newestPos - scratchLagSamples);
    } else if (slipReturning) {
      // Return to NOW (lag = 0)
      float currentLagSamples = currentLagFromNewest(newestPos);
      float finalCatchThresholdSamples =
          sampleRate * (kSlipFinalCatchThresholdMs / 1000.f);

      if (currentLagSamples <= nowSnapThresholdSamples) {
        startNowCatch(currentLagSamples);
        slipReturning = false;
        slipFinalCatchActive = false;
      } else

          if (!slipFinalCatchActive) {
        // Exponential-like approach to zero lag.
        // We target a specific lag value that decreases over time.
        float alpha = dt / std::max(kSlipReturnTime, 1e-6f);
        float targetLag = currentLagSamples * (1.f - alpha);

        // Ensure we actually move towards zero even if alpha is tiny.
        if (targetLag > currentLagSamples - 0.5f) {
          targetLag = currentLagSamples - 0.5f;
        }
        if (targetLag < 0.f)
          targetLag = 0.f;

        readHead = buffer.wrapPosition(newestPos - targetLag);
        keepSlipLagAligned = true;

        if (targetLag <= finalCatchThresholdSamples) {
          slipFinalCatchActive = true;
          slipReturnRemaining = kSlipFinalCatchTime;
          slipReturnStartLag = targetLag;
        }
      } else {
        // Final catch phase: smooth snap to live input.
        slipReturnRemaining = std::max(0.f, slipReturnRemaining - dt);
        float progress = 1.f - clamp(slipReturnRemaining /
                                         std::max(kSlipFinalCatchTime, 1e-6f),
                                     0.f, 1.f);
        float shapedProgress = 1.f - std::pow(1.f - progress, 2.5f);
        float targetLag = slipReturnStartLag * (1.f - shapedProgress);

        readHead = buffer.wrapPosition(newestPos - targetLag);
        keepSlipLagAligned = true;

        if (slipReturnRemaining <= 0.f || targetLag < 0.5f) {
          readHead = newestPos;
          slipReturning = false;
          slipFinalCatchActive = false;
        }
      }
    } else if (positionFollow && !externalScratch) {
      // Absolute Position CV
      float targetLag = lagForPositionCv(positionCv, limit);
      readHead = buffer.wrapPosition(newestPos - targetLag);
    } else {
      // Normal Transport
      float candidate = unwrapReadNearWrite(readHead, newestPos) + speed;
      candidate = clamp(candidate, newestPos - maxLag, newestPos - minLag);
      readHead = buffer.wrapPosition(candidate);
    }

    if (anyScratch) {
      slipReturning = false;
      slipFinalCatchActive = false;
      slipReturnRemaining = 0.f;
      slipReturnStartLag = 0.f;
    }

    if (nowCatchActive) {
      nowCatchRemaining = std::max(0.f, nowCatchRemaining - dt);
      float progress =
          1.f -
          clamp(nowCatchRemaining / std::max(kNowCatchTime, 1e-6f), 0.f, 1.f);
      float shapedProgress = progress * (2.f - progress);
      float targetLag = nowCatchStartLag * (1.f - shapedProgress);
      if (nowCatchRemaining <= 0.f || targetLag < 0.5f) {
        nowCatchActive = false;
        targetLag = 0.f;
        pinToNow = true;
      } else {
        keepNowCatchLagAligned = true;
      }
      scratchLagSamples = targetLag;
      scratchLagTargetSamples = targetLag;
      readHead = buffer.wrapPosition(newestPos - targetLag);
    }

    bool holdAtScratchEdge =
        manualScratch && limit > 0.f && scratchLagSamples >= (limit - 0.5f);
    bool holdAtReverseEdge = reverseAtOldestEdge;
    bool holdAtBufferEdge = holdAtScratchEdge || holdAtReverseEdge;

    bool scratchReadPath = anyScratch || positionFollow;
    bool useLinearInterpolation =
        !highQualityScratchInterpolation && scratchReadPath;
    float readDeltaForTone = readHead - prevReadHead;
    if (buffer.size > 0) {
      float halfSize = float(buffer.size) * 0.5f;
      if (readDeltaForTone > halfSize) {
        readDeltaForTone -= float(buffer.size);
      }
      if (readDeltaForTone < -halfSize) {
        readDeltaForTone += float(buffer.size);
      }
    }
    float motionAmount = clamp((std::fabs(readDeltaForTone) - 1.f) / 3.f, 0.f, 1.f);
    auto wet = useLinearInterpolation ? buffer.readLinear(readHead)
               : (highQualityScratchInterpolation && scratchReadPath)
                   ? buffer.readHighQuality(readHead)
                   : buffer.readCubic(readHead);
    wet = applyCartridgeCharacter(wet, motionAmount);
    float mix = clamp(mixKnob, 0.f, 1.f);
    float outL = inL * (1.f - mix) + wet.first * mix;
    float outR = inR * (1.f - mix) + wet.second * mix;

    if (!freezeState && !holdAtBufferEdge) {
      float feedback = clamp(feedbackKnob, 0.f, 1.f);
      buffer.write(inL + outL * feedback, inR + outR * feedback);
      newestPos = newestReadablePos();
      if (pinToNow) {
        readHead = newestPos;
      } else if (keepSlipLagAligned || keepNowCatchLagAligned) {
        readHead = buffer.wrapPosition(readHead + 1.f);
      }
    }

    if (buffer.size > 0) {
      float readDelta = readHead - prevReadHead;
      float halfSize = float(buffer.size) * 0.5f;
      if (readDelta > halfSize) {
        readDelta -= float(buffer.size);
      }
      if (readDelta < -halfSize) {
        readDelta += float(buffer.size);
      }
      platterPhase += readDelta * platterRadiansPerSample();
    }

    result.outL = outL;
    result.outR = outR;
    result.lag = currentLagFromNewest(newestPos);
    result.accessibleLag = limit;
    result.platterAngle = platterPhase;
    return result;
  }
};

struct TemporalDeck;

struct TemporalDeckDisplayWidget : Widget {
  TemporalDeck *module = nullptr;
  Vec centerMm = mm2px(Vec(50.8f, 72.f));
  float platterRadiusPx = mm2px(Vec(29.5f, 0.f)).x;

  void draw(const DrawArgs &args) override;
};

struct TemporalDeckTonearmWidget : Widget {
  TemporalDeck *module = nullptr;
  Vec centerPx = mm2px(Vec(50.8f, 72.f));
  float platterRadiusPx = mm2px(Vec(29.5f, 0.f)).x;

  void draw(const DrawArgs &args) override;
};

struct TemporalDeckPlatterWidget : OpaqueWidget {
  TemporalDeck *module = nullptr;
  Vec centerPx = mm2px(Vec(50.8f, 72.f));
  float platterRadiusPx = mm2px(Vec(29.5f, 0.f)).x;
  float deadZonePx = 0.f;
  bool dragging = false;
  Vec onButtonPos;
  float lastAngle = 0.f;
  float localLagSamples = 0.f;

  Vec localCenter() const { return centerPx.minus(box.pos); }

  bool isWithinPlatter(Vec panelPos) const {
    Vec local = panelPos.minus(localCenter());
    float radius = local.norm();
    return radius <= platterRadiusPx;
  }

  void updateScratchFromLocal(Vec local, Vec mouseDelta);

  void draw(const DrawArgs &args) override;
  void onButton(const event::Button &e) override;
  void onHoverScroll(const event::HoverScroll &e) override;
  void onDragMove(const event::DragMove &e) override;
  void onDragStart(const event::DragStart &e) override;
  void onDragEnd(const event::DragEnd &e) override;
};

struct DeckRateQuantity : ParamQuantity {
  std::string getDisplayValueString() override {
    return string::f("%.2fx",
                     TemporalDeckEngine::baseSpeedFromKnob(getValue()));
  }
};

struct ScratchSensitivityQuantity : ParamQuantity {
  static float sensitivityForValue(float v) {
    if (v <= 0.5f) {
      return rescale(v, 0.f, 0.5f, 0.5f, 1.f);
    }
    return rescale(v, 0.5f, 1.f, 1.f, 2.f);
  }

  std::string getDisplayValueString() override {
    return string::f("%.2fx", sensitivityForValue(getValue()));
  }

  std::string getLabel() override { return "Scratch sensitivity"; }
};

struct TemporalDeck : Module {
  static constexpr float kUiPublishRateHz = 120.f;
  static constexpr float kUiPublishIntervalSec = 1.f / kUiPublishRateHz;
  static constexpr int kArcLightCount = 31;

  enum ParamId {
    BUFFER_PARAM,
    RATE_PARAM,
    SCRATCH_SENSITIVITY_PARAM,
    MIX_PARAM,
    FEEDBACK_PARAM,
    FREEZE_PARAM,
    REVERSE_PARAM,
    SLIP_PARAM,
    PARAMS_LEN
  };
  enum InputId {
    POSITION_CV_INPUT,
    RATE_CV_INPUT,
    INPUT_L_INPUT,
    INPUT_R_INPUT,
    SCRATCH_GATE_INPUT,
    FREEZE_GATE_INPUT,
    INPUTS_LEN
  };
  enum OutputId { OUTPUT_L_OUTPUT, OUTPUT_R_OUTPUT, OUTPUTS_LEN };
  enum LightId {
    FREEZE_LIGHT,
    REVERSE_LIGHT,
    SLIP_LIGHT,
    ARC_LIGHT_START,
    ARC_MAX_LIGHT_START = ARC_LIGHT_START + kArcLightCount,
    LIGHTS_LEN = ARC_MAX_LIGHT_START + kArcLightCount
  };

  TemporalDeckEngine engine;
  dsp::SchmittTrigger freezeTrigger;
  dsp::SchmittTrigger reverseTrigger;
  dsp::SchmittTrigger slipTrigger;
  float cachedSampleRate = 0.f;
  bool freezeLatched = false;
  bool reverseLatched = false;
  bool slipLatched = false;
  std::atomic<bool> platterTouched{false};
  std::atomic<uint32_t> platterGestureRevision{0};
  std::atomic<float> platterLagTarget{0.f};
  std::atomic<float> platterGestureVelocity{0.f};
  std::atomic<float> platterWheelDelta{0.f};
  std::atomic<int> platterScratchHoldSamples{0};
  std::atomic<int> platterMotionFreshSamples{0};
  std::atomic<float> uiLagSamples{0.f};
  std::atomic<float> uiAccessibleLagSamples{0.f};
  std::atomic<float> uiSampleRate{44100.f};
  std::atomic<float> uiPlatterAngle{0.f};
  float uiPublishTimerSec = 0.f;
  bool highQualityScratchInterpolation = true;
  int cartridgeCharacter = TemporalDeckEngine::CARTRIDGE_CLEAN;

  float scratchSensitivity() {
    return ScratchSensitivityQuantity::sensitivityForValue(
        params[SCRATCH_SENSITIVITY_PARAM].getValue());
  }

  TemporalDeck() {
    config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
    configParam(BUFFER_PARAM, 0.f, 1.f, 1.f, "Buffer", " s", 0.f, 8.f);
    configParam<DeckRateQuantity>(RATE_PARAM, 0.f, 1.f, 0.5f, "Rate");
    configParam<ScratchSensitivityQuantity>(SCRATCH_SENSITIVITY_PARAM, 0.f, 1.f,
                                            0.5f, "Scratch sensitivity");
    configParam(MIX_PARAM, 0.f, 1.f, 1.f, "Mix");
    configParam(FEEDBACK_PARAM, 0.f, 1.f, 0.f, "Feedback");
    configButton(FREEZE_PARAM, "Freeze");
    configButton(REVERSE_PARAM, "Reverse");
    configButton(SLIP_PARAM, "Slip");
    configInput(POSITION_CV_INPUT, "Position CV");
    configInput(RATE_CV_INPUT, "Rate CV");
    configInput(INPUT_L_INPUT, "Left audio");
    configInput(INPUT_R_INPUT, "Right audio");
    configInput(SCRATCH_GATE_INPUT, "Scratch gate");
    configInput(FREEZE_GATE_INPUT, "Freeze gate");
    configOutput(OUTPUT_L_OUTPUT, "Left audio");
    configOutput(OUTPUT_R_OUTPUT, "Right audio");
    onSampleRateChange();
  }

  void onSampleRateChange() override {
    cachedSampleRate = APP->engine->getSampleRate();
    engine.reset(cachedSampleRate);
    uiSampleRate.store(cachedSampleRate);
    uiLagSamples.store(0.f);
    uiAccessibleLagSamples.store(0.f);
    uiPlatterAngle.store(0.f);
    uiPublishTimerSec = 0.f;
    platterScratchHoldSamples.store(0);
    platterMotionFreshSamples.store(0);
    for (int i = 0; i < kArcLightCount; ++i) {
      lights[ARC_LIGHT_START + i].setBrightness(0.f);
      lights[ARC_MAX_LIGHT_START + i].setBrightness(0.f);
    }
  }

  json_t *dataToJson() override {
    json_t *root = json_object();
    json_object_set_new(root, "freezeLatched", json_boolean(freezeLatched));
    json_object_set_new(root, "reverseLatched", json_boolean(reverseLatched));
    json_object_set_new(root, "slipLatched", json_boolean(slipLatched));
    json_object_set_new(root, "highQualityScratchInterpolation",
                        json_boolean(highQualityScratchInterpolation));
    json_object_set_new(root, "cartridgeCharacter",
                        json_integer(cartridgeCharacter));
    return root;
  }

  void dataFromJson(json_t *root) override {
    if (!root) {
      return;
    }
    json_t *freezeJ = json_object_get(root, "freezeLatched");
    json_t *reverseJ = json_object_get(root, "reverseLatched");
    json_t *slipJ = json_object_get(root, "slipLatched");
    json_t *scratchInterpJ =
        json_object_get(root, "highQualityScratchInterpolation");
    json_t *cartridgeJ = json_object_get(root, "cartridgeCharacter");
    if (freezeJ) {
      freezeLatched = json_boolean_value(freezeJ);
    }
    if (reverseJ) {
      reverseLatched = json_boolean_value(reverseJ);
    }
    if (slipJ) {
      slipLatched = json_boolean_value(slipJ);
    }
    if (scratchInterpJ) {
      highQualityScratchInterpolation = json_boolean_value(scratchInterpJ);
    }
    if (cartridgeJ) {
      cartridgeCharacter = clamp((int) json_integer_value(cartridgeJ), 0,
                                 TemporalDeckEngine::CARTRIDGE_COUNT - 1);
    }
  }

  void process(const ProcessArgs &args) override {
    if (args.sampleRate != cachedSampleRate) {
      onSampleRateChange();
    }

    if (freezeTrigger.process(params[FREEZE_PARAM].getValue())) {
      bool next = !freezeLatched;
      freezeLatched = next;
      if (next) {
        reverseLatched = false;
        slipLatched = false;
      }
    }
    if (reverseTrigger.process(params[REVERSE_PARAM].getValue())) {
      bool next = !reverseLatched;
      reverseLatched = next;
      if (next) {
        freezeLatched = false;
        slipLatched = false;
      }
    }
    if (slipTrigger.process(params[SLIP_PARAM].getValue())) {
      bool next = !slipLatched;
      slipLatched = next;
      if (next) {
        freezeLatched = false;
        reverseLatched = false;
      }
    }

    float inL = inputs[INPUT_L_INPUT].getVoltage();
    float inR = inputs[INPUT_R_INPUT].isConnected()
                    ? inputs[INPUT_R_INPUT].getVoltage()
                    : inL;
    float positionCv = inputs[POSITION_CV_INPUT].getVoltage();
    float rateCv = inputs[RATE_CV_INPUT].getVoltage();

    engine.highQualityScratchInterpolation = highQualityScratchInterpolation;
    engine.cartridgeCharacter = cartridgeCharacter;
    int scratchHold = platterScratchHoldSamples.load();
    bool wheelScratchHeld = scratchHold > 0;
    if (wheelScratchHeld) {
      platterScratchHoldSamples.store(std::max(0, scratchHold - 1));
    }
    int motionFresh = platterMotionFreshSamples.load();
    bool platterMotionActive = motionFresh > 0;
    if (platterMotionActive) {
      platterMotionFreshSamples.store(std::max(0, motionFresh - 1));
    }
    float wheelDelta = platterWheelDelta.exchange(0.f);

    auto frame = engine.process(
        args.sampleTime, inL, inR, params[BUFFER_PARAM].getValue(),
        params[RATE_PARAM].getValue(), params[MIX_PARAM].getValue(),
        params[FEEDBACK_PARAM].getValue(), freezeLatched, reverseLatched,
        slipLatched,
        inputs[FREEZE_GATE_INPUT].getVoltage() >=
            TemporalDeckEngine::kFreezeGateThreshold,
        inputs[SCRATCH_GATE_INPUT].getVoltage() >=
            TemporalDeckEngine::kScratchGateThreshold,
        inputs[SCRATCH_GATE_INPUT].isConnected(),
        inputs[POSITION_CV_INPUT].isConnected(), positionCv, rateCv,
        platterTouched.load(), wheelScratchHeld, platterMotionActive,
        platterGestureRevision.load(), platterLagTarget.load(),
        platterGestureVelocity.load(), wheelDelta);

    outputs[OUTPUT_L_OUTPUT].setVoltage(frame.outL);
    outputs[OUTPUT_R_OUTPUT].setVoltage(frame.outR);
    lights[FREEZE_LIGHT].setBrightness(freezeLatched ? 1.f : 0.f);
    lights[REVERSE_LIGHT].setBrightness(reverseLatched ? 1.f : 0.f);
    lights[SLIP_LIGHT].setBrightness(slipLatched ? 1.f : 0.f);
    // Keep platter rotation responsive during scratch gestures.
    uiPlatterAngle.store(frame.platterAngle);
    // Manual drag math consumes uiLagSamples as its base state. This must stay
    // current at audio rate; publishing only on the slower UI tick reintroduced
    // stale-lag errors where slow reverse drags fought the user.
    uiLagSamples.store(frame.lag);
    uiAccessibleLagSamples.store(frame.accessibleLag);
    uiSampleRate.store(args.sampleRate);

    uiPublishTimerSec += args.sampleTime;
    if (uiPublishTimerSec >= kUiPublishIntervalSec) {
      uiPublishTimerSec = std::fmod(uiPublishTimerSec, kUiPublishIntervalSec);
      float maxLag = std::max(1.f, args.sampleRate * 8.f);
      float lagRatio = clamp(frame.lag / maxLag, 0.f, 1.f);
      float limitRatio = clamp(frame.accessibleLag / maxLag, 0.f, 1.f);
      float lagLed = lagRatio * float(kArcLightCount - 1);
      float limitLed = limitRatio * float(kArcLightCount - 1);
      for (int i = 0; i < kArcLightCount; ++i) {
        float brightness = 0.f;
        if (i == 0) {
          brightness = clamp(lagLed, 0.f, 1.f);
        } else {
          brightness = clamp(lagLed - float(i) + 1.f, 0.f, 1.f);
        }
        if (limitRatio > 0.f && std::fabs(float(i) - limitLed) < 0.5f) {
          brightness = std::max(brightness, 0.28f);
        }
        if (float(i) > limitLed + 0.5f) {
          brightness = 0.f;
        }
        lights[ARC_LIGHT_START + i].setBrightness(brightness);
        bool isLimitLed =
            limitRatio > 0.f && std::fabs(float(i) - limitLed) < 0.5f;
        lights[ARC_MAX_LIGHT_START + i].setBrightness(isLimitLed ? 1.f : 0.f);
      }
    }
  }

  void setPlatterScratch(bool touched, float lagSamples, float velocitySamples,
                         int holdSamples = 0) {
    platterTouched.store(touched);
    platterGestureRevision.fetch_add(1);
    platterLagTarget.store(lagSamples);
    platterGestureVelocity.store(velocitySamples);
    platterScratchHoldSamples.store(std::max(0, holdSamples));
    // Reset wheel delta when starting a new gesture or manual touch.
    if (touched || holdSamples == 0) {
      platterWheelDelta.store(0.f);
    }
  }

  void setPlatterMotionFreshSamples(int motionFreshSamples) {
    platterMotionFreshSamples.store(std::max(0, motionFreshSamples));
  }

  void addPlatterWheelDelta(float delta, int holdSamples) {
    float current = platterWheelDelta.load();
    platterWheelDelta.store(current + delta);
    platterScratchHoldSamples.store(std::max(0, holdSamples));
  }
};

static bool loadSvgCircleMm(const std::string &svgPath,
                            const std::string &circleId, Vec *outCenterMm,
                            float *outRadiusMm) {
  std::ifstream svgFile(svgPath);
  if (!svgFile.good()) {
    return false;
  }
  std::ostringstream svgBuffer;
  svgBuffer << svgFile.rdbuf();
  const std::string svgText = svgBuffer.str();

  const std::regex tagRe("<circle\\b[^>]*\\bid\\s*=\\s*\"" + circleId +
                             "\"[^>]*/?>",
                         std::regex::icase);
  std::smatch tagMatch;
  if (!std::regex_search(svgText, tagMatch, tagRe) || tagMatch.empty()) {
    return false;
  }

  const std::string tag = tagMatch.str(0);
  auto parseAttr = [&](const char *attr, float *out) {
    const std::regex attrRe(std::string("\\b") + attr + "\\s*=\\s*\"([^\"]+)\"",
                            std::regex::icase);
    std::smatch attrMatch;
    if (!std::regex_search(tag, attrMatch, attrRe)) {
      return false;
    }
    *out = std::stof(attrMatch.str(1));
    return true;
  };

  float cxMm = 0.f;
  float cyMm = 0.f;
  float radiusMm = 0.f;
  if (!parseAttr("cx", &cxMm) || !parseAttr("cy", &cyMm) ||
      !parseAttr("r", &radiusMm)) {
    return false;
  }

  *outCenterMm = Vec(cxMm, cyMm);
  *outRadiusMm = radiusMm;
  return true;
}

static bool loadPlatterAnchor(Vec &centerPx, float &radiusPx) {
  Vec centerMm;
  float radiusMm = 0.f;
  if (!loadSvgCircleMm(asset::plugin(pluginInstance, "res/deck.svg"),
                       "PLATTER_AREA", &centerMm, &radiusMm)) {
    return false;
  }
  centerPx = mm2px(centerMm);
  radiusPx = mm2px(Vec(radiusMm, 0.f)).x;
  return true;
}

static bool isLeftMouseDown() {
  return APP && APP->window && APP->window->win &&
         glfwGetMouseButton(APP->window->win, GLFW_MOUSE_BUTTON_LEFT) ==
             GLFW_PRESS;
}

void TemporalDeckDisplayWidget::draw(const DrawArgs &args) {
  if (!module) {
    return;
  }
  float accessibleLag = std::max(1.f, module->uiAccessibleLagSamples.load());
  float lag = clamp(module->uiLagSamples.load(), 0.f, accessibleLag);
  nvgSave(args.vg);
  float arcRadius = platterRadiusPx + mm2px(Vec(3.5f, 0.f)).x;

  if (APP && APP->window && APP->window->uiFont) {
    /*
    const char* mouseText = module->platterTouched.load() ? "mDown" : "mUp";
    const char* motionText = module->platterMotionFreshSamples.load() > 0 ?
    "drag" : "still"; Vec debugPos = centerMm.plus(Vec(-platterRadiusPx * 0.92f,
    -platterRadiusPx * 0.98f));

    nvgFontFaceId(args.vg, APP->window->uiFont->handle);
    nvgFontSize(args.vg, 10.0f);
    nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
    nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 210));
    nvgText(args.vg, debugPos.x, debugPos.y, mouseText, nullptr);
    nvgText(args.vg, debugPos.x, debugPos.y + 11.5f, motionText, nullptr);
    */

    float lagMs = 1000.f * lag / std::max(module->uiSampleRate.load(), 1.f);
    char text[32];
    std::snprintf(text, sizeof(text), "%.0f ms", lagMs);
    Vec textPos = centerMm.plus(
        Vec(arcRadius + mm2px(Vec(8.0f, 0.f)).x, -arcRadius * 0.86f));

    nvgFontFaceId(args.vg, APP->window->uiFont->handle);
    nvgFontSize(args.vg, 11.5f);
    nvgTextAlign(args.vg, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
    nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 255));
    nvgText(args.vg, textPos.x, textPos.y, text, nullptr);
  }
  nvgRestore(args.vg);
}

void TemporalDeckTonearmWidget::draw(const DrawArgs &args) {
  nvgSave(args.vg);
  Vec center = centerPx;
  Vec armPivot =
      center.plus(Vec(platterRadiusPx * 1.0f, platterRadiusPx * 0.72f));
  Vec stylusTip =
      center.plus(Vec(platterRadiusPx * 0.9f, platterRadiusPx * 0.04f));
  float platterPhase = module ? module->uiPlatterAngle.load() : 0.f;
  float wobbleAngle = 0.012f * std::sin(platterPhase * 0.31f)
                    + 0.006f * std::sin(platterPhase * 0.77f + 0.8f);
  auto rotateAroundPivot = [&](Vec p) {
    Vec rel = p.minus(armPivot);
    float c = std::cos(wobbleAngle);
    float s = std::sin(wobbleAngle);
    return armPivot.plus(Vec(rel.x * c - rel.y * s, rel.x * s + rel.y * c));
  };
  stylusTip = rotateAroundPivot(stylusTip);
  Vec armDir = stylusTip.minus(armPivot);
  float armLen = armDir.norm();
  if (armLen > 1e-4f) {
    armDir = armDir.div(armLen);
    Vec armNormal(-armDir.y, armDir.x);

    nvgBeginPath(args.vg);
    nvgCircle(args.vg, armPivot.x, armPivot.y, mm2px(Vec(5.3f, 0.f)).x);
    nvgFillColor(args.vg, nvgRGBA(28, 31, 36, 236));
    nvgFill(args.vg);

    nvgBeginPath(args.vg);
    nvgCircle(args.vg, armPivot.x, armPivot.y, mm2px(Vec(3.0f, 0.f)).x);
    nvgFillColor(args.vg, nvgRGBA(142, 148, 156, 235));
    nvgFill(args.vg);

    Vec counterweight = armPivot.minus(armDir.mult(mm2px(Vec(4.2f, 0.f)).x));
    nvgBeginPath(args.vg);
    nvgRoundedRect(args.vg, counterweight.x - mm2px(Vec(2.2f, 0.f)).x,
                   counterweight.y - mm2px(Vec(1.6f, 0.f)).x,
                   mm2px(Vec(4.4f, 0.f)).x, mm2px(Vec(3.2f, 0.f)).x, 1.2f);
    nvgFillColor(args.vg, nvgRGBA(74, 78, 86, 220));
    nvgFill(args.vg);

    Vec armStart = armPivot.plus(armDir.mult(mm2px(Vec(4.8f, 0.f)).x));
    Vec armEnd = stylusTip.minus(armDir.mult(mm2px(Vec(6.2f, 0.f)).x));
    nvgBeginPath(args.vg);
    nvgMoveTo(args.vg, armStart.x, armStart.y);
    nvgLineTo(args.vg, armEnd.x, armEnd.y);
    nvgStrokeColor(args.vg, nvgRGBA(196, 202, 209, 236));
    nvgStrokeWidth(args.vg, 3.3f);
    nvgStroke(args.vg);

    nvgBeginPath(args.vg);
    nvgMoveTo(args.vg, armStart.x, armStart.y);
    nvgLineTo(args.vg, armEnd.x, armEnd.y);
    nvgStrokeColor(args.vg, nvgRGBA(86, 90, 98, 176));
    nvgStrokeWidth(args.vg, 1.0f);
    nvgStroke(args.vg);

    Vec elbow = armEnd.plus(armDir.mult(mm2px(Vec(1.9f, 0.f)).x));
    Vec headshellFront = stylusTip.minus(armDir.mult(mm2px(Vec(1.4f, 0.f)).x));
    Vec headshellA = elbow.plus(armNormal.mult(mm2px(Vec(1.8f, 0.f)).x));
    Vec headshellB = elbow.minus(armNormal.mult(mm2px(Vec(1.8f, 0.f)).x));
    Vec headshellC =
        headshellFront.minus(armNormal.mult(mm2px(Vec(1.05f, 0.f)).x));
    Vec headshellD =
        headshellFront.plus(armNormal.mult(mm2px(Vec(1.05f, 0.f)).x));
    nvgBeginPath(args.vg);
    nvgMoveTo(args.vg, headshellA.x, headshellA.y);
    nvgLineTo(args.vg, headshellD.x, headshellD.y);
    nvgLineTo(args.vg, headshellC.x, headshellC.y);
    nvgLineTo(args.vg, headshellB.x, headshellB.y);
    nvgClosePath(args.vg);
    nvgFillColor(args.vg, nvgRGBA(206, 172, 86, 226));
    nvgFill(args.vg);

    Vec cartBack = headshellFront.minus(armDir.mult(mm2px(Vec(0.6f, 0.f)).x));
    Vec cartFront = cartBack.plus(armDir.mult(mm2px(Vec(3.3f, 0.f)).x));
    Vec cartA = cartBack.plus(armNormal.mult(mm2px(Vec(1.55f, 0.f)).x));
    Vec cartB = cartBack.minus(armNormal.mult(mm2px(Vec(1.55f, 0.f)).x));
    Vec cartC = cartFront.minus(armNormal.mult(mm2px(Vec(1.35f, 0.f)).x));
    Vec cartD = cartFront.plus(armNormal.mult(mm2px(Vec(1.35f, 0.f)).x));
    nvgBeginPath(args.vg);
    nvgMoveTo(args.vg, cartA.x, cartA.y);
    nvgLineTo(args.vg, cartD.x, cartD.y);
    nvgLineTo(args.vg, cartC.x, cartC.y);
    nvgLineTo(args.vg, cartB.x, cartB.y);
    nvgClosePath(args.vg);
    nvgFillColor(args.vg, nvgRGBA(46, 49, 56, 238));
    nvgFill(args.vg);

  }
  nvgRestore(args.vg);
}

void TemporalDeckPlatterWidget::draw(const DrawArgs &args) {
  nvgSave(args.vg);
  float rotation = module ? module->uiPlatterAngle.load() : 0.f;
  Vec center = localCenter();

  NVGcolor outerDark = nvgRGB(20, 22, 26);
  NVGpaint vinylGrad = nvgRadialGradient(
      args.vg, center.x - platterRadiusPx * 0.18f,
      center.y - platterRadiusPx * 0.22f, platterRadiusPx * 0.15f,
      platterRadiusPx * 1.05f, nvgRGBA(52, 56, 64, 220), outerDark);

  nvgBeginPath(args.vg);
  nvgCircle(args.vg, center.x, center.y, platterRadiusPx);
  nvgFillPaint(args.vg, vinylGrad);
  nvgFill(args.vg);

  // Optimization: Skip complex grooves if platter is too small to see them
  // clearly
  if (platterRadiusPx > 10.f) {
    nvgSave(args.vg);
    nvgTranslate(args.vg, center.x, center.y);
    nvgRotate(args.vg, rotation * 0.92f);

    for (int i = 0; i < 16; ++i) {
      float grooveRadius = platterRadiusPx * (0.24f + 0.047f * i);
      float alpha = (i % 2 == 0) ? 34.f : 18.f;
      float wobbleAmp = 0.55f + 0.05f * float(i % 4);
      float wobblePhase = 0.47f * float(i) + 0.061f * float(i * i);
      float wobbleFreq = 3.1f + 0.23f * float((i * 2 + 1) % 5);
      float ringRotation = 0.19f * float(i) + 0.043f * float(i * i);

      nvgBeginPath(args.vg);
      constexpr int kSteps = 64; // Reduced from 96 for performance
      for (int step = 0; step <= kSteps; ++step) {
        float t =
            2.f * float(M_PI) * float(step) / float(kSteps) + ringRotation;
        // Simplified wobble: removed expensive pow and copysign
        float wobble = std::sin(t * wobbleFreq + wobblePhase);
        float radius = grooveRadius + wobbleAmp * wobble;
        float x = std::cos(t) * radius;
        float y = std::sin(t) * radius;
        if (step == 0)
          nvgMoveTo(args.vg, x, y);
        else
          nvgLineTo(args.vg, x, y);
      }
      nvgStrokeColor(args.vg, nvgRGBA(210, 218, 228, (unsigned char)alpha));
      nvgStrokeWidth(args.vg, 0.7f);
      nvgStroke(args.vg);
    }
    nvgRestore(args.vg);
  }

  float labelRadius = platterRadiusPx * 0.33f;
  nvgBeginPath(args.vg);
  nvgCircle(args.vg, center.x, center.y, labelRadius);
  nvgFillColor(args.vg, nvgRGB(90, 178, 187));
  nvgFill(args.vg);

  nvgSave(args.vg);
  nvgTranslate(args.vg, center.x, center.y);
  nvgRotate(args.vg, rotation);

  nvgBeginPath(args.vg);
  nvgCircle(args.vg, 0.f, 0.f, labelRadius * 0.74f);
  nvgFillColor(args.vg, nvgRGB(12, 41, 45));
  nvgFill(args.vg);

  for (int i = 0; i < 3; ++i) {
    float angle = 2.f * float(M_PI) * float(i) / 3.f;
    Vec a(std::cos(angle) * labelRadius * 0.22f,
          std::sin(angle) * labelRadius * 0.22f);
    Vec b(std::cos(angle) * labelRadius * 0.62f,
          std::sin(angle) * labelRadius * 0.62f);
    nvgBeginPath(args.vg);
    nvgMoveTo(args.vg, a.x, a.y);
    nvgLineTo(args.vg, b.x, b.y);
    nvgStrokeColor(args.vg, nvgRGBA(90, 178, 187, 255));
    nvgStrokeWidth(args.vg, 1.2f);
    nvgStroke(args.vg);
  }

  nvgBeginPath(args.vg);
  nvgRoundedRect(args.vg, -labelRadius * 0.42f, -labelRadius * 0.055f,
                 labelRadius * 0.84f, labelRadius * 0.11f, 1.2f);
  nvgFillColor(args.vg, nvgRGBA(90, 178, 187, 120));
  nvgFill(args.vg);

  nvgRestore(args.vg);

  nvgBeginPath(args.vg);
  nvgCircle(args.vg, center.x, center.y, labelRadius * 0.12f);
  nvgFillColor(args.vg, nvgRGB(222, 228, 235));
  nvgFill(args.vg);

  nvgBeginPath(args.vg);
  nvgCircle(args.vg, center.x, center.y, platterRadiusPx);
  nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 32));
  nvgStrokeWidth(args.vg, 1.1f);
  nvgStroke(args.vg);

  nvgRestore(args.vg);
  Widget::draw(args);
}

void TemporalDeckPlatterWidget::updateScratchFromLocal(Vec local,
                                                       Vec mouseDelta) {
  if (!module || !dragging) {
    return;
  }
  // These thresholds are intentionally small so slow deliberate platter motion
  // still becomes a fresh gesture. Raising them too far makes the engine fall
  // back to the stationary-hold path and the platter starts to feel resistant.
  constexpr float kScratchMoveThresholdPx = 0.2f;
  constexpr float kScratchMoveThresholdRad = 0.001f;
  float radius = local.norm();
  if (radius < deadZonePx * 0.25f) {
    return;
  }
  float angle = std::atan2(local.y, local.x);
  float deltaAngle = angle - lastAngle;
  if (deltaAngle > M_PI) {
    deltaAngle -= 2.f * M_PI;
  }
  if (deltaAngle < -M_PI) {
    deltaAngle += 2.f * M_PI;
  }
  float mouseMovePx = std::fabs(mouseDelta.x) + std::fabs(mouseDelta.y);
  if (mouseMovePx < kScratchMoveThresholdPx &&
      std::fabs(deltaAngle) < kScratchMoveThresholdRad) {
    return;
  }
  // Always apply drag deltas to the engine's latest lag, not the last UI
  // event's cached lag. The live point continues to advance while the mouse is
  // held, so stale lag here causes slow backward drags to creep forward.
  float accessibleLag = module->uiAccessibleLagSamples.load();
  localLagSamples = clamp(module->uiLagSamples.load(), 0.f, accessibleLag);
  float effectiveRadius = std::max(radius, deadZonePx);
  float weight = clamp(effectiveRadius / platterRadiusPx, 0.3f, 1.f);
  float sensitivity = module->scratchSensitivity();
  float samplesPerRadian =
      60.f * module->uiSampleRate.load() /
      (2.f * float(M_PI) * TemporalDeckEngine::kNominalPlatterRpm) *
      TemporalDeckEngine::kMouseScratchTravelScale * sensitivity;
  float lagDelta = deltaAngle * samplesPerRadian * weight;
  localLagSamples = clamp(localLagSamples - lagDelta, 0.f, accessibleLag);
  float velocity = (std::fabs(mouseDelta.x) + std::fabs(mouseDelta.y)) *
                   module->uiSampleRate.load() * 0.0005f * sensitivity;
  if (deltaAngle < 0.f) {
    velocity *= -1.f;
  }
  module->setPlatterScratch(true, localLagSamples, velocity);
  int motionFreshSamples =
      std::max(1, int(std::round(module->uiSampleRate.load() * 0.02f)));
  module->setPlatterMotionFreshSamples(motionFreshSamples);
  lastAngle = angle;
}

void TemporalDeckPlatterWidget::onButton(const event::Button &e) {
  onButtonPos = e.pos;
  if (e.button == GLFW_MOUSE_BUTTON_LEFT && isWithinPlatter(e.pos)) {
    if (e.action == GLFW_PRESS) {
      if (module) {
        localLagSamples = module->uiLagSamples.load();
        module->setPlatterScratch(true, localLagSamples, 0.f);
        module->setPlatterMotionFreshSamples(0);
      }
      e.consume(this);
      return;
    }
    if (e.action == GLFW_RELEASE && !dragging) {
      if (module) {
        module->setPlatterScratch(false, localLagSamples, 0.f);
        module->setPlatterMotionFreshSamples(0);
      }
      e.consume(this);
      return;
    }
  }
  Widget::onButton(e);
}

void TemporalDeckPlatterWidget::onHoverScroll(const event::HoverScroll &e) {
  if (!module || !isWithinPlatter(e.pos)) {
    OpaqueWidget::onHoverScroll(e);
    return;
  }

  float scroll = -e.scrollDelta.y;
  if (std::fabs(scroll) < 1e-4f) {
    OpaqueWidget::onHoverScroll(e);
    return;
  }

  float maxLag = module->uiAccessibleLagSamples.load();
  if (maxLag <= 0.f) {
    e.consume(this);
    return;
  }

  float sampleRate = module->uiSampleRate.load();
  float samplesPerNotch =
      sampleRate * 0.008f * TemporalDeckEngine::kWheelScratchTravelScale *
      module->scratchSensitivity();
  float lagDelta = scroll * samplesPerNotch;
  float holdSeconds = module->slipLatched ? 0.16f : 0.03f;
  int holdSamples = std::max(1, int(std::round(sampleRate * holdSeconds)));

  module->addPlatterWheelDelta(lagDelta, holdSamples);
  e.consume(this);
}

void TemporalDeckPlatterWidget::onDragStart(const event::DragStart &e) {
  if (!module || e.button != GLFW_MOUSE_BUTTON_LEFT ||
      !isWithinPlatter(onButtonPos)) {
    return;
  }
  Vec local = onButtonPos.minus(localCenter());
  dragging = true;
  lastAngle = std::atan2(local.y, local.x);
  localLagSamples = module->uiLagSamples.load();
  module->setPlatterScratch(true, localLagSamples, 0.f);
  module->setPlatterMotionFreshSamples(0);
  e.consume(this);
}

void TemporalDeckPlatterWidget::onDragMove(const event::DragMove &e) {
  if (!dragging || e.button != GLFW_MOUSE_BUTTON_LEFT) {
    return;
  }
  if (!isLeftMouseDown()) {
    dragging = false;
    if (module) {
      module->setPlatterScratch(false, localLagSamples, 0.f);
      module->setPlatterMotionFreshSamples(0);
    }
    return;
  }
  Vec local = APP->scene->rack->getMousePos()
                  .minus(parent->box.pos)
                  .minus(box.pos)
                  .minus(localCenter());
  updateScratchFromLocal(local, e.mouseDelta);
  e.consume(this);
}

void TemporalDeckPlatterWidget::onDragEnd(const event::DragEnd &e) {
  if (dragging && e.button == GLFW_MOUSE_BUTTON_LEFT) {
    if (isLeftMouseDown()) {
      e.consume(this);
      return;
    }
    dragging = false;
    if (module) {
      module->setPlatterScratch(false, localLagSamples, 0.f);
      module->setPlatterMotionFreshSamples(0);
    }
    e.consume(this);
  }
}

struct BananutBlack : app::SvgPort {
  BananutBlack() {
    setSvg(Svg::load(asset::plugin(pluginInstance, "res/BananutBlack.svg")));
  }
};

struct TemporalDeckWidget : ModuleWidget {
  TemporalDeckWidget(TemporalDeck *module) {
    setModule(module);
    setPanel(createPanel(asset::plugin(pluginInstance, "res/deck.svg")));

    addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
    addChild(
        createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
    addChild(createWidget<ScrewSilver>(
        Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    addChild(createWidget<ScrewSilver>(Vec(
        box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

    addParam(createParamCentered<RoundBlackKnob>(
        mm2px(Vec(8.408, 17.086)), module, TemporalDeck::BUFFER_PARAM));
    addParam(createParamCentered<RoundBlackKnob>(
        mm2px(Vec(24.39, 99.026)), module, TemporalDeck::RATE_PARAM));
    addParam(createParamCentered<RoundBlackKnob>(
        mm2px(Vec(78.482, 98.872)), module, TemporalDeck::MIX_PARAM));
    addParam(createParamCentered<RoundBlackKnob>(
        mm2px(Vec(78.482, 112.996)), module, TemporalDeck::FEEDBACK_PARAM));
    addParam(createParamCentered<LEDButton>(mm2px(Vec(62.1, 101.1)), module,
                                            TemporalDeck::FREEZE_PARAM));
    addParam(createParamCentered<LEDButton>(mm2px(Vec(50.2, 101.1)), module,
                                            TemporalDeck::REVERSE_PARAM));
    addParam(createParamCentered<LEDButton>(mm2px(Vec(37.8, 101.1)), module,
                                            TemporalDeck::SLIP_PARAM));

    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(48.465, 112.9)), module,
                                             TemporalDeck::POSITION_CV_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(24.405, 112.9)), module,
                                             TemporalDeck::RATE_CV_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.837, 99.012)), module,
                                             TemporalDeck::INPUT_L_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.878, 112.9)), module,
                                             TemporalDeck::INPUT_R_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(37.703, 112.9)), module,
                                             TemporalDeck::SCRATCH_GATE_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(62.1, 112.9)), module,
                                             TemporalDeck::FREEZE_GATE_INPUT));

    addOutput(createOutputCentered<BananutBlack>(
        mm2px(Vec(94.041, 99.012)), module, TemporalDeck::OUTPUT_L_OUTPUT));
    addOutput(createOutputCentered<BananutBlack>(
        mm2px(Vec(94.0, 113.146)), module, TemporalDeck::OUTPUT_R_OUTPUT));

    addChild(createLightCentered<MediumLight<RedLight>>(
        mm2px(Vec(62.1, 95.3)), module, TemporalDeck::FREEZE_LIGHT));
    addChild(createLightCentered<MediumLight<RedLight>>(
        mm2px(Vec(50.2, 95.3)), module, TemporalDeck::REVERSE_LIGHT));
    addChild(createLightCentered<MediumLight<RedLight>>(
        mm2px(Vec(37.8, 95.3)), module, TemporalDeck::SLIP_LIGHT));

    Vec platterCenter = mm2px(Vec(50.8f, 72.f));
    float platterRadius = mm2px(Vec(29.5f, 0.f)).x;
    loadPlatterAnchor(platterCenter, platterRadius);

    float arcRadius = platterRadius + mm2px(Vec(3.5f, 0.f)).x;
    for (int i = 0; i < TemporalDeck::kArcLightCount; ++i) {
      float t = float(i) / float(TemporalDeck::kArcLightCount - 1);
      float angle = -float(M_PI) * t;
      Vec ledPos = platterCenter.plus(
          Vec(std::cos(angle), std::sin(angle)).mult(arcRadius));
      addChild(createLightCentered<MediumLight<RedLight>>(
          ledPos, module, TemporalDeck::ARC_MAX_LIGHT_START + i));
      addChild(createLightCentered<MediumLight<YellowLight>>(
          ledPos, module, TemporalDeck::ARC_LIGHT_START + i));
    }

    auto display = new TemporalDeckDisplayWidget();
    display->module = module;
    display->centerMm = platterCenter;
    display->platterRadiusPx = platterRadius;
    display->box.size = box.size;
    addChild(display);

    auto platter = new TemporalDeckPlatterWidget();
    platter->module = module;
    platter->centerPx = platterCenter;
    platter->platterRadiusPx = platterRadius;
    platter->deadZonePx = platterRadius * 0.08f;
    platter->box.pos = platterCenter.minus(Vec(platterRadius, platterRadius));
    platter->box.size = Vec(platterRadius * 2.f, platterRadius * 2.f);
    addChild(platter);

    auto *tonearm = new TemporalDeckTonearmWidget;
    tonearm->module = module;
    tonearm->centerPx = platterCenter;
    tonearm->platterRadiusPx = platterRadius;
    tonearm->box.pos = Vec(0.f, 0.f);
    tonearm->box.size = box.size;
    addChild(tonearm);

    addParam(createParamCentered<RoundBlackKnob>(
        mm2px(Vec(18.5, 85.5)), module,
        TemporalDeck::SCRATCH_SENSITIVITY_PARAM));
  }

  void appendContextMenu(Menu *menu) override {
    TemporalDeck *module = dynamic_cast<TemporalDeck *>(this->module);
    assert(menu);
    menu->addChild(new MenuSeparator());
    menu->addChild(createIndexPtrSubmenuItem(
        "Cartridge character",
        {"Clean", "Classic", "Battle", "Lo-Fi"},
        module ? &module->cartridgeCharacter : nullptr));
    menu->addChild(createBoolPtrMenuItem(
        "High-quality scratch interpolation", "",
        module ? &module->highQualityScratchInterpolation : nullptr));
  }
};

} // namespace

Model *modelTemporalDeck =
    createModel<TemporalDeck, TemporalDeckWidget>("TemporalDeck");
