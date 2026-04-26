#pragma once

#include "TemporalDeckExpanderProtocol.hpp"
#include "TemporalDeckTest.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <type_traits>
#include <utility>
#include <vector>

namespace temporaldeck_modes {

inline float realBufferSecondsForMode(int index) {
  switch (index) {
  case 0:
    return 11.f; // 10s mode with guard second
  case 1:
    return 21.f; // 20s mode with guard second
  case 2:
  case 3:
    return 601.f; // 10m modes with guard second
  default:
    return 11.f;
  }
}

inline float usableBufferSecondsForMode(int index) { return std::max(1.f, realBufferSecondsForMode(index) - 1.f); }

inline bool isMonoBufferMode(int index) { return index == 3; }

} // namespace temporaldeck_modes

namespace temporaldeck {

using temporaldeck_modes::isMonoBufferMode;
using temporaldeck_modes::realBufferSecondsForMode;
using temporaldeck_modes::usableBufferSecondsForMode;

constexpr float kPi = 3.14159265358979323846f;
constexpr float kTwoPi = 2.f * kPi;

inline double clampd(double x, double a, double b) {
  return std::max(a, std::min(x, b));
}

inline double wrapd(double x, double length) {
  if (length <= 0.0) {
    return 0.0;
  }
  x = std::fmod(x, length);
  if (x < 0.0) {
    x += length;
  }
  return x;
}

template <typename T, typename U, typename V>
inline typename std::common_type<T, U, V>::type clamp(T value, U minValue, V maxValue) {
  typedef typename std::common_type<T, U, V>::type R;
  R rValue = static_cast<R>(value);
  R rMin = static_cast<R>(minValue);
  R rMax = static_cast<R>(maxValue);
  return std::max(rMin, std::min(rValue, rMax));
}

template <typename T, typename U>
inline T crossfade(T a, T b, U x) {
  return a + (b - a) * x;
}

struct TemporalDeckBuffer {
  std::vector<float> left;
  std::vector<float> right;
  int size = 0;
  int writeHead = 0;
  int filled = 0;
  float sampleRate = 44100.f;
  float durationSeconds = 11.f;
  bool monoStorage = false;

  void reset(float sr, float seconds = 11.f, bool mono = false) {
    sampleRate = sr;
    durationSeconds = std::max(1.f, seconds);
    monoStorage = mono;
    size = std::max(1, int(std::round(sampleRate * durationSeconds)));
    left.assign(size, 0.f);
    if (monoStorage) {
      std::vector<float>().swap(right);
    } else {
      right.assign(size, 0.f);
    }
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

  double wrapPosition(double pos) const {
    if (size <= 0) {
      return 0.0;
    }
    pos = std::fmod(pos, double(size));
    if (pos < 0.0) {
      pos += double(size);
    }
    return pos;
  }

  void write(float inL, float inR) {
    if (size <= 0) {
      return;
    }
    if (monoStorage) {
      left[writeHead] = 0.5f * (inL + inR);
    } else {
      left[writeHead] = inL;
      right[writeHead] = inR;
    }
    writeHead = wrapIndex(writeHead + 1);
    filled = std::min(filled + 1, size);
  }

  float rightSample(int idx) const { return monoStorage ? left[idx] : right[idx]; }

  static float cubicSample(float y0, float y1, float y2, float y3, float t) {
    float a0 = y3 - y2 - y0 + y1;
    float a1 = y0 - y1 - a0;
    float a2 = y2 - y0;
    float a3 = y1;
    return ((a0 * t + a1) * t + a2) * t + a3;
  }

  struct Lagrange6Weights {
    float w0 = 0.f;
    float w1 = 0.f;
    float w2 = 0.f;
    float w3 = 0.f;
    float w4 = 0.f;
    float w5 = 0.f;
  };

  static Lagrange6Weights lagrange6Weights(float t) {
    float tP2 = t + 2.f;
    float tP1 = t + 1.f;
    float tM1 = t - 1.f;
    float tM2 = t - 2.f;
    float tM3 = t - 3.f;

    // Uniform nodes x = {-2, -1, 0, 1, 2, 3}
    // with precomputed inverse denominator products:
    // {-1/120, 1/24, -1/12, 1/12, -1/24, 1/120}
    Lagrange6Weights w;
    w.w0 = (-1.f / 120.f) * (tP1 * t * tM1 * tM2 * tM3);
    w.w1 = (1.f / 24.f) * (tP2 * t * tM1 * tM2 * tM3);
    w.w2 = (-1.f / 12.f) * (tP2 * tP1 * tM1 * tM2 * tM3);
    w.w3 = (1.f / 12.f) * (tP2 * tP1 * t * tM2 * tM3);
    w.w4 = (-1.f / 24.f) * (tP2 * tP1 * t * tM1 * tM3);
    w.w5 = (1.f / 120.f) * (tP2 * tP1 * t * tM1 * tM2);
    return w;
  }

  static float windowedSinc(float x, float radius) {
    float ax = std::fabs(x);
    if (ax > radius) {
      return 0.f;
    }
    if (ax < 1e-5f) {
      return 1.f;
    }
    // Blackman-windowed sinc for smooth, low-ripple scratch resampling.
    float sinc = std::sin(kPi * x) / (kPi * x);
    float windowPhase = (ax / radius);
    float blackman = 0.42f + 0.5f * std::cos(kPi * windowPhase) + 0.08f * std::cos(2.f * kPi * windowPhase);
    return sinc * blackman;
  }

  static constexpr int kSincRadius = 8;
  static constexpr int kSincTapCount = kSincRadius * 2;
  static constexpr int kSincPhaseCount = 1024;

  struct SincKernel {
    std::array<float, kSincTapCount> weights {};
    float invWeightSum = 1.f;
  };

  static const std::array<SincKernel, kSincPhaseCount> &sincKernelLut() {
    struct LutBuilder {
      std::array<SincKernel, kSincPhaseCount> kernels {};
      LutBuilder() {
        for (int phase = 0; phase < kSincPhaseCount; ++phase) {
          float frac = float(phase) / float(kSincPhaseCount);
          float weightSum = 0.f;
          for (int tap = 0; tap < kSincTapCount; ++tap) {
            int k = tap - kSincRadius + 1; // [-7, 8]
            float w = windowedSinc(float(k) - frac, float(kSincRadius));
            kernels[phase].weights[size_t(tap)] = w;
            weightSum += w;
          }
          kernels[phase].invWeightSum = (std::fabs(weightSum) > 1e-6f) ? (1.f / weightSum) : 1.f;
        }
      }
    };
    static const LutBuilder lutBuilder;
    return lutBuilder.kernels;
  }

  static const SincKernel &sincKernelForFraction(float frac) {
    frac = clamp(frac, 0.f, 0.999999f);
    int phase = int(std::lround(frac * float(kSincPhaseCount)));
    phase = clamp(phase, 0, kSincPhaseCount - 1);
    return sincKernelLut()[size_t(phase)];
  }

  std::pair<float, float> readCubic(double pos) const {
    if (size <= 0 || filled <= 0) {
      return {0.f, 0.f};
    }
    pos = wrapPosition(pos);
    int i1 = int(std::floor(pos));
    float t = float(pos - double(i1));
    // Exact/near-exact sample-center reads are common during transport playback.
    // Skip cubic math when phase is effectively integral.
    if (std::fabs(t) <= 1e-6f || std::fabs(1.f - t) <= 1e-6f) {
      int idx = wrapIndex(int(std::round(pos)));
      return {left[idx], rightSample(idx)};
    }
    int i0 = wrapIndex(i1 - 1);
    int i2 = wrapIndex(i1 + 1);
    int i3 = wrapIndex(i1 + 2);
    return {cubicSample(left[i0], left[i1], left[i2], left[i3], t),
            cubicSample(rightSample(i0), rightSample(i1), rightSample(i2), rightSample(i3), t)};
  }

  std::pair<float, float> readLinear(double pos) const {
    if (size <= 0 || filled <= 0) {
      return {0.f, 0.f};
    }
    pos = wrapPosition(pos);
    int i0 = int(pos);
    int i1 = wrapIndex(i0 + 1);
    float t = float(pos - double(i0));
    return {crossfade(left[i0], left[i1], t), crossfade(rightSample(i0), rightSample(i1), t)};
  }

  std::pair<float, float> readHighQuality(double pos) const {
    if (size <= 0 || filled <= 0) {
      return {0.f, 0.f};
    }
    pos = wrapPosition(pos);
    int i2 = int(std::floor(pos));
    float t = float(pos - double(i2));
    if (std::fabs(t) <= 1e-6f || std::fabs(1.f - t) <= 1e-6f) {
      int idx = wrapIndex(int(std::round(pos)));
      return {left[idx], rightSample(idx)};
    }
    int i0 = wrapIndex(i2 - 2);
    int i1 = wrapIndex(i2 - 1);
    int i3 = wrapIndex(i2 + 1);
    int i4 = wrapIndex(i2 + 2);
    int i5 = wrapIndex(i2 + 3);
    Lagrange6Weights w = lagrange6Weights(t);
    float outL = left[i0] * w.w0 + left[i1] * w.w1 + left[i2] * w.w2 + left[i3] * w.w3 + left[i4] * w.w4 +
                 left[i5] * w.w5;
    float outR = rightSample(i0) * w.w0 + rightSample(i1) * w.w1 + rightSample(i2) * w.w2 + rightSample(i3) * w.w3 +
                 rightSample(i4) * w.w4 + rightSample(i5) * w.w5;
    return {outL, outR};
  }

  std::pair<float, float> readSinc(double pos) const {
    if (size <= 0 || filled <= 0) {
      return {0.f, 0.f};
    }
    pos = wrapPosition(pos);
    int center = int(std::floor(pos));
    float frac = float(pos - double(center));
    if (std::fabs(frac) <= 1e-6f || std::fabs(1.f - frac) <= 1e-6f) {
      int idx = wrapIndex(int(std::round(pos)));
      return {left[idx], rightSample(idx)};
    }

    float accL = 0.f;
    float accR = 0.f;
    const SincKernel &kernel = sincKernelForFraction(frac);
    for (int tap = 0; tap < kSincTapCount; ++tap) {
      int k = tap - kSincRadius + 1;
      int idx = wrapIndex(center + k);
      float w = kernel.weights[size_t(tap)];
      accL += left[idx] * w;
      accR += rightSample(idx) * w;
    }
    accL *= kernel.invWeightSum;
    accR *= kernel.invWeightSum;
    return {accL, accR};
  }
};

struct LiveScopeEnvelopeBlock {
  int64_t key = -1;
  float minL = 0.f;
  float maxL = 0.f;
  float minR = 0.f;
  float maxR = 0.f;
  float minMid = 0.f;
  float maxMid = 0.f;
  int sampleCount = 0;
  bool hasData = false;
};

struct TemporalDeckEngine {
  static constexpr float kScratchGateThreshold = 1.f;
  static constexpr float kFreezeGateThreshold = 1.f;
  static constexpr float kQuickSlipMaxReturnTime = 1.0f;
  static constexpr float kQuickSlipVelocityCapRatio = 64.0f;
  static constexpr float kSlipEnableReturnThreshold = 64.f;
  static constexpr float kSlipBlendTime = 0.010f;
  static constexpr float kSlipBlendTimeMin = 0.004f;
  static constexpr float kSlipBlendTimeMax = 0.012f;
  static constexpr float kSlipNearNowBlendThresholdMs = 18.f;
  static constexpr float kSlipNearNowDampingThresholdMs = 24.f;
  static constexpr float kSlipNearNowVelocityCapSlope = 40.f;
  static constexpr float kSlipNearNowVelocityCapFloorRatio = 0.12f;
  static constexpr float kSlipCatchLagReferenceSec = 0.12f;
  static constexpr float kSampleSlipResumeSnapMs = 9.0f;
  static constexpr float kSlipDynamicLpCutoffLowHz = 1800.f;
  static constexpr float kSlipDynamicLpCutoffHighHz = 17000.f;
  static constexpr float kSlipDynamicLpMixLow = 0.22f;
  static constexpr float kSlipDynamicLpMixHigh = 0.82f;
  static constexpr float kSlipCatchMaxExtraRatioQuick = 2.40f;
  static constexpr float kSlipCatchMaxExtraRatioSlow = 1.10f;
  static constexpr float kSlipCatchMaxExtraRatioNormal = 1.65f;
  static constexpr float kSlipCatchMaxExtraRatioInstant = 3.50f;
  static constexpr float kSlipCatchAccelQuick = 16.0f;
  static constexpr float kSlipCatchAccelSlow = 7.5f;
  static constexpr float kSlipCatchAccelNormal = 11.0f;
  static constexpr float kSlipCatchAccelInstant = 22.0f;
  static constexpr float kSlipCatchBrakeMultiplier = 1.6f;
  static constexpr float kSlipCatchDoneVelocityRatio = 0.35f;
  static constexpr float kScratchFollowTime = 0.012f;
  static constexpr float kScratchSoftLagStepMin = 6.0f;
  static constexpr float kScratchSoftLagStepMax = 28.0f;
  static constexpr float kSlowScratchVelThreshold = 28.0f;
  static constexpr float kSlowScratchSmoothingTime = 0.0075f;
  static constexpr float kScratchTargetJitterThreshold = 0.35f;
  static constexpr float kReverseBiteVelocityThreshold = 22.0f;
  static constexpr float kReverseBiteMaxBoost = 1.55f;
  static constexpr float kNowSnapThresholdMs = 33.0f;
  static constexpr float kNowCatchTime = 0.004f;
  static constexpr float kMouseScratchTravelScale = 4.0f;
  static constexpr float kWheelScratchTravelScale = 4.5f;
  static constexpr float kManualVelocityPredictScale = 0.95f;
  static constexpr float kHybridScratchHandFollowHz = 220.f;
  static constexpr float kHybridScratchVelocityDampingHz = 9.f;
  static constexpr float kHybridScratchCorrectionHz = 70.f;
  static constexpr float kHybridScratchWheelBurstDecayHz = 24.f;
  static constexpr float kHybridScratchWheelImpulseTime = 0.03f;
  static constexpr float kSampleWheelForwardBurstFloorRatio = 1.10f;
  static constexpr float kHybridScratchMaxVelocity = 96000.f;
  static constexpr float kHybridScratchMaxAccel = 1200000.f;
  static constexpr float kHybridScratchVelocityDeadband = 8.f;
  static constexpr float kScratch3LagAlpha = 0.32f;
  static constexpr float kScratch3LagBeta = 0.018f;
  static constexpr float kScratch3VelocityFollowHz = 28.f;
  static constexpr float kScratch3VelocityDecayHz = 30.f;
  static constexpr float kScratch3VelocityDeadband = 10.f;
  static constexpr float kScratch3MaxLagVelocity = 96000.f;
  static constexpr float kExternalCvMaxTurnsPerSec = 5.5f;
  static constexpr float kExternalCvMaxTurnAccelPerSec2 = 55.0f;
  static constexpr float kExternalCvCorrectionHz = 28.f;
  static constexpr float kExternalCvVelocityDampingHz = 8.f;
  static constexpr float kScratchInertiaFollowHz = 950.f;
  static constexpr float kScratchInertiaDampingHz = 380.f;
  static constexpr float kInertiaBlend = 0.25f;
  static constexpr float kNominalPlatterRpm = 33.333333f;
  static constexpr float kLofiModControlRateHz = 3000.f;
  static constexpr int kLiveScopeEnvelopeBlockSamples = 32;

  enum CartridgeCharacter {
    CARTRIDGE_CLEAN,
    CARTRIDGE_M44_7,
    CARTRIDGE_CONCORDE_SCRATCH,
    CARTRIDGE_680_HP,
    CARTRIDGE_QBERT,
    CARTRIDGE_LOFI,
    CARTRIDGE_COUNT
  };
  enum BufferDurationMode {
    BUFFER_DURATION_10S,
    BUFFER_DURATION_20S,
    BUFFER_DURATION_10MIN_STEREO,
    BUFFER_DURATION_10MIN_MONO,
    BUFFER_DURATION_COUNT
  };
  enum ScratchInterpolationMode {
    SCRATCH_INTERP_CUBIC,
    SCRATCH_INTERP_LAGRANGE6,
    SCRATCH_INTERP_SINC,
    SCRATCH_INTERP_COUNT
  };
  enum SlipReturnMode {
    SLIP_RETURN_SLOW,
    SLIP_RETURN_NORMAL,
    SLIP_RETURN_INSTANT,
    SLIP_RETURN_COUNT
  };
  enum ExternalGatePosMode {
    EXTERNAL_GATE_POS_GLIDE,
    EXTERNAL_GATE_POS_MODULE_SYNC,
    EXTERNAL_GATE_POS_COUNT
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
    float saturationMix = 1.f;
    float motionDulling = 1.f;
    float scratchCompensation = 0.f;

    CartridgeParams() {}

    CartridgeParams(float hpHz, float bodyHz, float lpHz, float lpMotionHz, float bodyGain, float presenceGain,
                    float crossfeed, float drive, float stereoTilt, float saturationMix = 1.f,
                    float motionDulling = 1.f, float scratchCompensation = 0.f)
        : hpHz(hpHz), bodyHz(bodyHz), lpHz(lpHz), lpMotionHz(lpMotionHz), bodyGain(bodyGain),
          presenceGain(presenceGain), crossfeed(crossfeed), drive(drive), stereoTilt(stereoTilt),
          saturationMix(saturationMix), motionDulling(motionDulling), scratchCompensation(scratchCompensation) {}
  };

  TemporalDeckBuffer buffer;
  float sampleRate = 44100.f;
  std::atomic<bool> sampleModeEnabled{false};
  bool sampleLoopEnabled = false;
  bool sampleLoaded = false;
  bool sampleTransportPlaying = false;
  bool sampleTruncated = false;
  int sampleFrames = 0;
  float sampleAbsolutePeakVolts = 0.f;
  double samplePlayhead = 0.0;
  double readHead = 0.0;
  double timelineHead = 0.0;
  float platterPhase = 0.f;
  float platterVelocity = 0.f;
  bool freezeState = false;
  bool reverseState = false;
  bool slipState = false;
  bool highQualityRateInterpolation = false;
  int scratchInterpolationMode = SCRATCH_INTERP_LAGRANGE6;
  int externalGatePosMode = EXTERNAL_GATE_POS_GLIDE;
  int slipReturnMode = SLIP_RETURN_NORMAL;
  bool scratchActive = false;
  bool slipReturning = false;
  bool slipBlendActive = false;
  bool nowCatchActive = false;
  float slipReturnOverrideTime = -1.f;
  float slipCatchVelocity = 0.f;
  float sampleSlipVelocity = 0.f;
  float slipBlendRemaining = 0.f;
  double slipBlendStartReadHead = 0.0;
  double slipBlendStartLag = 0.0;
  double sampleSlipAnchorPos = 0.0;
  float nowCatchRemaining = 0.f;
  float nowCatchStartLag = 0.f;
  double scratchLagSamples = 0.0;
  double scratchLagTargetSamples = 0.0;
  double liveManualScratchAnchorNewestPos = 0.0;
  double liveManualScratchAnchorLagSamples = 0.0;
  float scratchHandVelocity = 0.f;
  float scratchMotionVelocity = 0.f;
  float scratch3LagVelocity = 0.f;
  float scratch3GestureAgeSec = 0.f;
  float scratchWheelVelocityBurst = 0.f;
  double filteredManualLagTargetSamples = 0.0;
  double lastPlatterLagTarget = 0.0;
  uint32_t lastPlatterGestureRevision = 0;
  bool platterTouchHoldLatched = false;
  double platterTouchHoldReadHead = 0.0;
  int cartridgeCharacter = CARTRIDGE_CLEAN;
  int bufferDurationMode = BUFFER_DURATION_10S;
  int lastSlipReturnMode = SLIP_RETURN_NORMAL;
  CartridgeChannelState cartridgeLeft;
  CartridgeChannelState cartridgeRight;
  float lofiWowPhaseA = 0.f;
  float lofiWowPhaseB = 0.f;
  float lofiFlutterPhase = 0.f;
  float lofiWowFlutterCached = 0.f;
  int lofiModUpdateCountdown = 0;
  int lofiModUpdateIntervalSamples = 1;
  float lofiCrackleEnv = 0.f;
  float lofiCracklePolarity = 1.f;
  uint32_t lofiRng = 0x5A17C3E1u;
  int cachedCartridgeCharacter = -1;
  CartridgeParams cachedCartridgeParams;
  float cachedHpCoeff = 0.f;
  float cachedBodyCoeff = 0.f;
  float cachedLpCoeffBase = 0.f;
  float cachedLpCoeffMotion = 0.f;
  float cachedDriveNorm = 1.f;
  bool cachedDriveEnabled = false;
  float cachedSaturationMix = 1.f;
  float cachedCrossfeed = 0.f;
  float cachedStereoTilt = 0.f;
  float cachedTiltLeftGain = 1.f;
  float cachedTiltRightGain = 1.f;
  float cachedAirMixGain = 1.f;
  float cachedBodyMixGain = 0.f;
  float cachedMakeupGain = 1.f;
  float cachedPlaybackColorMix = 1.f;
  float cachedScratchCompensation = 0.f;
  float prevScratchReadDelta = 0.f;
  int prevScratchDeltaSign = 0;
  float scratchFlipTransientEnv = 0.f;
  float prevWetL = 0.f;
  float prevWetR = 0.f;
  float prevScratchOutL = 0.f;
  float prevScratchOutR = 0.f;
  float scratchDcInL = 0.f;
  float scratchDcInR = 0.f;
  float scratchDcOutL = 0.f;
  float scratchDcOutR = 0.f;
  float slipDynLpStateL = 0.f;
  float slipDynLpStateR = 0.f;
  bool slipDynLpPrimed = false;
  bool externalCvGateHigh = false;
  double externalCvAnchorLagSamples = 0.0;
  bool scratchOutGateHigh = false;
  double scratchOutAnchorLagSamples = 0.0;
  float prevBaseSpeed = 1.f;
  temporaldeck_expander::PreviewAccumulator preview;
  uint64_t bufferGeneration = 1;
  std::vector<LiveScopeEnvelopeBlock> liveScopeEnvelope;
  uint64_t liveScopeEnvelopeWriteCount = 0;

  void resetPreviewAccumulator(uint32_t capacityFrames = 0u) {
    uint32_t effectiveCapacity = capacityFrames > 0u ? capacityFrames : uint32_t(std::max(1, buffer.size));
    preview.reset(effectiveCapacity);
  }

  void rebuildPreviewFromCurrentSample() {
    resetPreviewAccumulator(uint32_t(std::max(1, sampleFrames)));
    sampleAbsolutePeakVolts = 0.f;
    if (!sampleLoaded || sampleFrames <= 0 || buffer.size <= 0) {
      return;
    }
    for (int i = 0; i < sampleFrames; ++i) {
      float l = buffer.left[i];
      float r = buffer.rightSample(i);
      sampleAbsolutePeakVolts = std::max(sampleAbsolutePeakVolts, std::fabs(l));
      sampleAbsolutePeakVolts = std::max(sampleAbsolutePeakVolts, std::fabs(r));
      preview.pushMonoSample(0.5f * (l + r));
    }
    preview.finalizePartialBin();
  }

  void bumpBufferGeneration() {
    if (bufferGeneration == UINT64_MAX) {
      bufferGeneration = 1;
    } else {
      bufferGeneration += 1;
    }
  }

  void resetLiveScopeEnvelope() {
    int blockCount = std::max(1, (buffer.size + kLiveScopeEnvelopeBlockSamples - 1) / kLiveScopeEnvelopeBlockSamples + 4);
    liveScopeEnvelope.assign(size_t(blockCount), LiveScopeEnvelopeBlock());
    liveScopeEnvelopeWriteCount = 0;
  }

  void pushLiveScopeEnvelopeSample(float inL, float inR) {
    if (liveScopeEnvelope.empty()) {
      resetLiveScopeEnvelope();
    }
    if (liveScopeEnvelope.empty()) {
      return;
    }
    uint64_t sampleIndex = liveScopeEnvelopeWriteCount;
    int64_t blockKey = int64_t(sampleIndex / uint64_t(kLiveScopeEnvelopeBlockSamples));
    size_t slot = size_t(uint64_t(blockKey) % uint64_t(liveScopeEnvelope.size()));
    LiveScopeEnvelopeBlock &block = liveScopeEnvelope[slot];
    float mid = 0.5f * (inL + inR);
    if (block.key != blockKey || !block.hasData) {
      block = LiveScopeEnvelopeBlock();
      block.key = blockKey;
      block.minL = block.maxL = inL;
      block.minR = block.maxR = inR;
      block.minMid = block.maxMid = mid;
      block.sampleCount = 1;
      block.hasData = true;
    } else {
      block.minL = std::min(block.minL, inL);
      block.maxL = std::max(block.maxL, inL);
      block.minR = std::min(block.minR, inR);
      block.maxR = std::max(block.maxR, inR);
      block.minMid = std::min(block.minMid, mid);
      block.maxMid = std::max(block.maxMid, mid);
      block.sampleCount += 1;
    }
    liveScopeEnvelopeWriteCount += 1;
  }

  double newestReadableAbsolutePos() const {
    if (sampleModeEnabled && sampleLoaded) {
      return std::max(0.0, double(sampleFrames - 1));
    }
    if (liveScopeEnvelopeWriteCount == 0u) {
      return 0.0;
    }
    return double(liveScopeEnvelopeWriteCount - 1u);
  }

  bool readLiveScopeEnvelopeRange(double newestAbsolutePos, float lagLow, float lagHigh, int channelMode,
                                  float *minOut, float *maxOut) const {
    if (!minOut || !maxOut || liveScopeEnvelope.empty() || liveScopeEnvelopeWriteCount == 0u || buffer.filled <= 0) {
      return false;
    }
    float overlapLow = std::max(0.f, std::min(lagLow, lagHigh));
    float overlapHigh = std::max(lagLow, lagHigh);
    if (!(overlapHigh >= overlapLow)) {
      return false;
    }

    double newestAvailableAbs = double(liveScopeEnvelopeWriteCount - 1u);
    double oldestAvailableAbs = newestAvailableAbs - double(std::max(0, buffer.filled - 1));
    double absLow = newestAbsolutePos - double(overlapHigh);
    double absHigh = newestAbsolutePos - double(overlapLow);
    if (absHigh < oldestAvailableAbs || absLow > newestAvailableAbs) {
      return false;
    }
    absLow = std::max(absLow, oldestAvailableAbs);
    absHigh = std::min(absHigh, newestAvailableAbs);
    if (!(absHigh >= absLow)) {
      return false;
    }

    int64_t blockKey0 = int64_t(std::floor(absLow / double(kLiveScopeEnvelopeBlockSamples)));
    int64_t blockKey1 = int64_t(std::floor(absHigh / double(kLiveScopeEnvelopeBlockSamples)));
    bool any = false;
    float rangeMin = 0.f;
    float rangeMax = 0.f;
    for (int64_t blockKey = blockKey0; blockKey <= blockKey1; ++blockKey) {
      size_t slot = size_t(uint64_t(blockKey) % uint64_t(liveScopeEnvelope.size()));
      const LiveScopeEnvelopeBlock &block = liveScopeEnvelope[slot];
      if (block.key != blockKey || !block.hasData) {
        continue;
      }
      float blockMin = 0.f;
      float blockMax = 0.f;
      switch (channelMode) {
      case 0:
        blockMin = block.minL;
        blockMax = block.maxL;
        break;
      case 1:
        blockMin = block.minR;
        blockMax = block.maxR;
        break;
      case 2:
      default:
        blockMin = block.minMid;
        blockMax = block.maxMid;
        break;
      }
      if (!any) {
        rangeMin = blockMin;
        rangeMax = blockMax;
        any = true;
      } else {
        rangeMin = std::min(rangeMin, blockMin);
        rangeMax = std::max(rangeMax, blockMax);
      }
    }
    if (!any) {
      return false;
    }
    *minOut = rangeMin;
    *maxOut = rangeMax;
    return true;
  }

  void reset(float sr, bool resetBuffer = true) {
    sampleRate = sr;
    if (resetBuffer) {
      buffer.reset(sr, realBufferSecondsForMode(bufferDurationMode), isMonoBufferMode(bufferDurationMode));
      resetPreviewAccumulator();
      resetLiveScopeEnvelope();
      bumpBufferGeneration();
    }
    sampleLoaded = false;
    sampleTransportPlaying = false;
    sampleTruncated = false;
    sampleFrames = 0;
    sampleAbsolutePeakVolts = 0.f;
    samplePlayhead = 0.f;
    readHead = 0.f;
    timelineHead = 0.f;
    platterPhase = 0.f;
    platterVelocity = 0.f;
    scratchActive = false;
    slipReturning = false;
    slipBlendActive = false;
    nowCatchActive = false;
    slipReturnOverrideTime = -1.f;
    slipCatchVelocity = 0.f;
    sampleSlipVelocity = 0.f;
    slipBlendRemaining = 0.f;
    slipBlendStartReadHead = 0.0;
    slipBlendStartLag = 0.0;
    sampleSlipAnchorPos = 0.0;
    nowCatchRemaining = 0.f;
    nowCatchStartLag = 0.f;
    scratchLagSamples = 0.f;
    scratchLagTargetSamples = 0.f;
    liveManualScratchAnchorNewestPos = 0.0;
    liveManualScratchAnchorLagSamples = 0.0;
    scratchHandVelocity = 0.f;
    scratchMotionVelocity = 0.f;
    scratch3LagVelocity = 0.f;
    scratch3GestureAgeSec = 0.f;
    scratchWheelVelocityBurst = 0.f;
    filteredManualLagTargetSamples = 0.f;
    lastPlatterLagTarget = 0.f;
    lastPlatterGestureRevision = 0;
    platterTouchHoldLatched = false;
    platterTouchHoldReadHead = 0.0;
    cartridgeLeft.reset();
    cartridgeRight.reset();
    lofiWowPhaseA = 0.f;
    lofiWowPhaseB = 0.f;
    lofiFlutterPhase = 0.f;
    lofiWowFlutterCached = 0.f;
    lofiModUpdateCountdown = 0;
    lofiModUpdateIntervalSamples = 1;
    lofiCrackleEnv = 0.f;
    lofiCracklePolarity = 1.f;
    lofiRng = 0x5A17C3E1u;
    cachedCartridgeCharacter = -1;
    cachedCartridgeParams = CartridgeParams();
    cachedHpCoeff = 0.f;
    cachedBodyCoeff = 0.f;
    cachedLpCoeffBase = 0.f;
    cachedLpCoeffMotion = 0.f;
    cachedDriveNorm = 1.f;
    cachedDriveEnabled = false;
    cachedSaturationMix = 1.f;
    cachedCrossfeed = 0.f;
    cachedStereoTilt = 0.f;
    cachedTiltLeftGain = 1.f;
    cachedTiltRightGain = 1.f;
    cachedAirMixGain = 1.f;
    cachedBodyMixGain = 0.f;
    cachedMakeupGain = 1.f;
    cachedPlaybackColorMix = 1.f;
    cachedScratchCompensation = 0.f;
    lastSlipReturnMode = slipReturnMode;
    prevScratchReadDelta = 0.f;
    prevScratchDeltaSign = 0;
    scratchFlipTransientEnv = 0.f;
    prevWetL = 0.f;
    prevWetR = 0.f;
    prevScratchOutL = 0.f;
    prevScratchOutR = 0.f;
    scratchDcInL = 0.f;
    scratchDcInR = 0.f;
    scratchDcOutL = 0.f;
    scratchDcOutR = 0.f;
    externalCvGateHigh = false;
    externalCvAnchorLagSamples = 0.0;
    scratchOutGateHigh = false;
    scratchOutAnchorLagSamples = 0.0;
    prevBaseSpeed = 1.f;
  }

  double maxLagFromKnob(float knob) const {
    return double(clamp(knob, 0.f, 1.f)) * double(sampleRate) * double(usableBufferSecondsForMode(bufferDurationMode));
  }

  double accessibleLag(float knob) const {
    // The newest readable sample sits at writeHead - 1, so the oldest valid
    // lag is filled - 1 samples behind that. Using `filled` overstates the live
    // range by one sample and can wrap the read head into unwritten space at
    // the oldest edge.
    double maxReadableLag = std::max(0, buffer.filled - 1);
    return std::min(maxLagFromKnob(knob), maxReadableLag);
  }

  double clampLag(double lag, double limit) const {
    if (isSampleLoopActive()) {
      return wrapd(lag, sampleLoopLength(limit));
    }
    return std::max(0.0, std::min(lag, std::max(0.0, limit)));
  }

  float lagErrorToTarget(double targetLag, double currentLag, double newestPos) const {
    double error = targetLag - currentLag;
    if (isSampleLoopActive()) {
      double length = sampleLoopLength(newestPos);
      if (length > 1e-6) {
        while (error > 0.5 * length) {
          error -= length;
        }
        while (error < -0.5 * length) {
          error += length;
        }
      }
    }
    return float(error);
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

  static float baseSpeedFromCv(float rateCv) {
    rateCv = clamp(rateCv, -10.f, 10.f);
    if (rateCv <= 0.f) {
      float t = (rateCv + 10.f) / 10.f;
      return 0.5f + 0.5f * t;
    }
    float t = rateCv / 10.f;
    return 1.f + t;
  }

  float computeBaseSpeed(float rateKnob, float rateCv, bool rateCvConnected, bool reverse) const {
    float speed = rateCvConnected ? baseSpeedFromCv(rateCv) : baseSpeedFromKnob(rateKnob);
    speed = clamp(speed, -3.f, 3.f);
    if (reverse) {
      speed *= -1.f;
    }
    return speed;
  }

  float slipCatchMaxExtraRatio() const {
    if (slipReturnOverrideTime >= 0.f) {
      return kSlipCatchMaxExtraRatioQuick;
    }
    switch (slipReturnMode) {
    case SLIP_RETURN_SLOW:
      return kSlipCatchMaxExtraRatioSlow;
    case SLIP_RETURN_INSTANT:
      return kSlipCatchMaxExtraRatioInstant;
    case SLIP_RETURN_NORMAL:
    default:
      return kSlipCatchMaxExtraRatioNormal;
    }
  }

  float slipCatchAccelRatio() const {
    if (slipReturnOverrideTime >= 0.f) {
      return kSlipCatchAccelQuick;
    }
    switch (slipReturnMode) {
    case SLIP_RETURN_SLOW:
      return kSlipCatchAccelSlow;
    case SLIP_RETURN_INSTANT:
      return kSlipCatchAccelInstant;
    case SLIP_RETURN_NORMAL:
    default:
      return kSlipCatchAccelNormal;
    }
  }

  float slipCatchLagCurveExponent() const {
    if (slipReturnOverrideTime >= 0.f) {
      return 0.78f;
    }
    switch (slipReturnMode) {
    case SLIP_RETURN_SLOW:
      return 1.18f;
    case SLIP_RETURN_INSTANT:
      return 0.70f;
    case SLIP_RETURN_NORMAL:
    default:
      return 0.92f;
    }
  }

  float slipCatchDoneVelocityRatioForMode() const {
    if (slipReturnOverrideTime >= 0.f) {
      return 0.62f;
    }
    switch (slipReturnMode) {
    case SLIP_RETURN_SLOW:
      return 0.35f;
    case SLIP_RETURN_INSTANT:
      return 0.70f;
    case SLIP_RETURN_NORMAL:
    default:
      return 0.55f;
    }
  }

  float slipNearNowDampingFloor() const {
    if (slipReturnOverrideTime >= 0.f) {
      return 0.90f;
    }
    switch (slipReturnMode) {
    case SLIP_RETURN_SLOW:
      return 0.72f;
    case SLIP_RETURN_INSTANT:
      return 0.92f;
    case SLIP_RETURN_NORMAL:
    default:
      return 0.88f;
    }
  }

  float slipNearNowCapBoost() const {
    if (slipReturnOverrideTime >= 0.f) {
      return 1.65f;
    }
    switch (slipReturnMode) {
    case SLIP_RETURN_SLOW:
      return 1.0f;
    case SLIP_RETURN_INSTANT:
      return 1.85f;
    case SLIP_RETURN_NORMAL:
    default:
      return 1.55f;
    }
  }

  void cancelSlipReturnState() {
    slipReturning = false;
    slipBlendActive = false;
    slipCatchVelocity = 0.f;
    sampleSlipVelocity = 0.f;
    slipBlendRemaining = 0.f;
    slipBlendStartReadHead = readHead;
    slipBlendStartLag = 0.0;
    slipReturnOverrideTime = -1.f;
  }

  float sampleSlipCatchSpeedRatio() const {
    switch (slipReturnMode) {
    case SLIP_RETURN_SLOW:
      return 3.5f;
    case SLIP_RETURN_INSTANT:
      return 1e6f;
    case SLIP_RETURN_NORMAL:
    default:
      return 6.0f;
    }
  }

  float sampleSlipCatchAccelRatio() const {
    switch (slipReturnMode) {
    case SLIP_RETURN_SLOW:
      return 18.0f;
    case SLIP_RETURN_INSTANT:
      return 1e6f;
    case SLIP_RETURN_NORMAL:
    default:
      return 34.0f;
    }
  }

  double sampleDeltaToTarget(double currentPos, double targetPos, double newestPos) const {
    double delta = targetPos - currentPos;
    if (isSampleLoopActive()) {
      double length = sampleLoopLength(newestPos);
      if (length > 1e-6) {
        while (delta > 0.5 * length) {
          delta -= length;
        }
        while (delta < -0.5 * length) {
          delta += length;
        }
      }
    }
    return delta;
  }

  bool integrateSampleSlipReturn(double newestPos, float dt) {
    float snapThresholdSamples = std::max(sampleRate * (kSampleSlipResumeSnapMs * 0.001f), 0.5f);
    if (slipReturnMode == SLIP_RETURN_INSTANT) {
      samplePlayhead = normalizeSamplePosition(sampleSlipAnchorPos, newestPos);
      readHead = samplePlayhead;
      cancelSlipReturnState();
      return true;
    }

    double currentPos = normalizeSamplePosition(readHead, newestPos);
    double targetPos = normalizeSamplePosition(sampleSlipAnchorPos, newestPos);
    double delta = sampleDeltaToTarget(currentPos, targetPos, newestPos);
    if (std::fabs(delta) <= snapThresholdSamples) {
      samplePlayhead = targetPos;
      readHead = targetPos;
      cancelSlipReturnState();
      return true;
    }

    float sr = std::max(sampleRate, 1.f);
    float desiredVelocity = clamp(float(delta) * 18.0f, -sampleSlipCatchSpeedRatio() * sr,
                                  sampleSlipCatchSpeedRatio() * sr);
    float maxDv = sampleSlipCatchAccelRatio() * sr * dt;
    sampleSlipVelocity += clamp(desiredVelocity - sampleSlipVelocity, -maxDv, maxDv);
    sampleSlipVelocity *= clamp(1.f - dt * 10.f, 0.f, 1.f);

    double nextPos = currentPos + double(sampleSlipVelocity) * double(dt);
    double nextDelta = sampleDeltaToTarget(nextPos, targetPos, newestPos);
    if ((delta > 0.0 && nextDelta < 0.0) || (delta < 0.0 && nextDelta > 0.0) || std::fabs(nextDelta) <= 0.5) {
      nextPos = targetPos;
      sampleSlipVelocity = 0.f;
    }

    samplePlayhead = normalizeSamplePosition(nextPos, newestPos);
    readHead = samplePlayhead;
    if (std::fabs(sampleDeltaToTarget(samplePlayhead, targetPos, newestPos)) <= snapThresholdSamples) {
      samplePlayhead = targetPos;
      readHead = targetPos;
      cancelSlipReturnState();
      return true;
    }
    return false;
  }

  void startSlipBlend(double lagNow, float sr) {
    slipBlendActive = true;
    float lagSec = float(std::max(0.0, lagNow) / std::max(sr, 1.f));
    float lagNorm = clamp(lagSec / (kSlipNearNowBlendThresholdMs * 0.001f), 0.f, 1.f);
    float speedNorm = clamp(slipCatchVelocity / std::max(sr * std::max(slipCatchMaxExtraRatio(), 0.1f), 1.f), 0.f, 1.f);
    float blendVoicing = clamp(0.65f * speedNorm + 0.35f * lagNorm, 0.f, 1.f);
    slipBlendRemaining = crossfade(kSlipBlendTimeMin, kSlipBlendTimeMax, blendVoicing);
    if (slipReturnOverrideTime >= 0.f) {
      slipBlendRemaining *= 0.55f;
    }
    slipBlendStartReadHead = readHead;
    slipBlendStartLag = lagNow;
  }

  bool integrateSlipCatchup(double newestPos, double maxLag, float baseSpeed, float dt) {
    if (slipReturnOverrideTime >= 0.f) {
      slipReturnOverrideTime = std::max(0.f, slipReturnOverrideTime - dt);
    }

    double lagNow = currentLagFromNewest(newestPos);
    float sr = std::max(sampleRate, 1.f);
    float transportVel = std::max(baseSpeed, 0.f) * sr;

    if (slipReturnOverrideTime < 0.f && slipReturnMode == SLIP_RETURN_INSTANT) {
      readHead = newestPos;
      cancelSlipReturnState();
      return true;
    }

    if (!slipBlendActive && lagNow <= 0.5) {
      readHead = newestPos;
      cancelSlipReturnState();
      return true;
    }

    float lagSec = float(lagNow / sr);
    float lagNorm = clamp(lagSec / kSlipCatchLagReferenceSec, 0.f, 1.f);
    float desiredExtraRatio = slipCatchMaxExtraRatio() * std::pow(lagNorm, slipCatchLagCurveExponent());

    float blendThresholdSec = kSlipNearNowBlendThresholdMs * 0.001f;
    if (slipReturnOverrideTime >= 0.f) {
      blendThresholdSec *= 0.72f;
    } else if (slipReturnMode == SLIP_RETURN_SLOW) {
      blendThresholdSec *= 1.30f;
    } else if (slipReturnMode == SLIP_RETURN_NORMAL) {
      blendThresholdSec *= 1.15f;
    } else if (slipReturnMode == SLIP_RETURN_INSTANT) {
      blendThresholdSec *= 0.85f;
    }
    float nearNowGain = clamp(lagSec / blendThresholdSec, 0.f, 1.f);
    desiredExtraRatio *= nearNowGain;

    float desiredExtraVel = desiredExtraRatio * sr;
    if (slipReturnOverrideTime >= 0.f) {
      // Time-bounded quick-slip: dynamically schedule velocity to land back at
      // NOW within the remaining window rather than hard-snapping on timeout.
      float remaining = std::max(slipReturnOverrideTime, std::max(dt, 1e-4f));
      float requiredTotalVel = float(lagNow) / remaining;
      float requiredExtraVel = std::max(0.f, requiredTotalVel - transportVel);
      float quickCap = sr * kQuickSlipVelocityCapRatio;
      requiredExtraVel = clamp(requiredExtraVel, 0.f, quickCap);
      desiredExtraVel = std::max(desiredExtraVel, requiredExtraVel);
      slipCatchVelocity = desiredExtraVel;
    } else {
      float accelBase = slipCatchAccelRatio() * sr;
      float brakeBase = accelBase * kSlipCatchBrakeMultiplier;
      float dv = desiredExtraVel - slipCatchVelocity;
      float maxDv = (dv >= 0.f ? accelBase : brakeBase) * dt;
      slipCatchVelocity += clamp(dv, -maxDv, maxDv);
      slipCatchVelocity = std::max(0.f, slipCatchVelocity);
      float dampingThresholdSec = kSlipNearNowDampingThresholdMs * 0.001f;
      if (lagSec < dampingThresholdSec) {
        float nearNowNorm = clamp(lagSec / dampingThresholdSec, 0.f, 1.f);
        float dampingFloor = slipNearNowDampingFloor();
        float damping = dampingFloor + (1.f - dampingFloor) * nearNowNorm;
        slipCatchVelocity *= damping;
        float velocityCap =
          std::max(sr * kSlipNearNowVelocityCapFloorRatio,
                   lagSec * sr * kSlipNearNowVelocityCapSlope * slipNearNowCapBoost());
        slipCatchVelocity = std::min(slipCatchVelocity, velocityCap);
      }
    }

    double unwrappedRead = unwrapReadNearWrite(readHead, newestPos);
    double candidate = unwrappedRead + double(transportVel + slipCatchVelocity) * double(dt);
    candidate = std::max(newestPos - std::max(maxLag, 0.0), std::min(candidate, newestPos));
    readHead = buffer.wrapPosition(candidate);

    lagNow = currentLagFromNewest(newestPos);
    lagSec = float(lagNow / sr);
    if (!slipBlendActive && lagSec <= blendThresholdSec &&
        slipCatchVelocity <= (slipCatchDoneVelocityRatioForMode() * sr)) {
      startSlipBlend(lagNow, sr);
    }
    return false;
  }

  double lagForPositionCv(float cv, double limit) const {
    double normalized = double(clamp(std::fabs(cv) / 10.f, 0.f, 1.f));
    return normalized * limit;
  }

  double lagOffsetForPositionCv(float cv) const {
    float clampedCv = clamp(cv, -10.f, 10.f);
    // POS polarity: +V moves forward (toward NOW), -V moves backward.
    // Lag is defined as distance behind NOW, so forward motion is negative lag.
    return -double(clampedCv) * double(sampleRate);
  }

  float onePoleCoeff(float hz) const {
    hz = clamp(hz, 0.f, sampleRate * 0.45f);
    if (hz <= 0.f) {
      return 0.f;
    }
    return 1.f - std::exp(-kTwoPi * hz / std::max(sampleRate, 1.f));
  }

  static CartridgeParams paramsForCartridge(int mode) {
    switch (mode) {
    case CARTRIDGE_M44_7:
      // M44-7: warm/fat, slight high roll-off, moderate output saturation.
      return {20.f, 140.f, 17000.f, 15000.f, 0.20f, -0.08f, 0.004f, 1.035f, 0.004f, 0.85f, 0.40f, 0.06f};
    case CARTRIDGE_CONCORDE_SCRATCH:
      // Concorde MKII Scratch: energetic, crisp transients, 5k-10k bite region.
      return {28.f, 2600.f, 18200.f, 16300.f, 0.03f, 0.16f, 0.003f, 1.03f, 0.008f, 0.55f, 0.35f, 0.05f};
    case CARTRIDGE_680_HP:
      // Stanton 680 HP: silky highs + low-mid bloom with strong stereo separation.
      return {18.f, 350.f, 19500.f, 17000.f, 0.13f, -0.01f, 0.002f, 1.018f, 0.010f, 0.30f, 0.35f, 0.07f};
    case CARTRIDGE_QBERT:
      // Q.Bert: hot output, mid-forward scratch articulation, softer top.
      return {24.f, 2500.f, 16500.f, 13500.f, 0.04f, 0.21f, 0.003f, 1.06f, 0.006f, 0.90f, 0.40f, 0.05f};
    case CARTRIDGE_LOFI:
      // Lo-Fi: intentionally veiled, smeared, and dirty.
      return {130.f, 980.f, 4300.f, 2100.f, 0.30f, -0.22f, 0.085f, 1.33f, 0.12f, 1.f, 1.f, 0.f};
    case CARTRIDGE_CLEAN:
    default:
      return {};
    }
  }

  static float makeupGainForCartridge(int mode) {
    switch (mode) {
    case CARTRIDGE_M44_7:
      return 0.971f;
    case CARTRIDGE_CONCORDE_SCRATCH:
      return 0.891f;
    case CARTRIDGE_680_HP:
      return 0.995f;
    case CARTRIDGE_QBERT:
      return 0.820f;
    case CARTRIDGE_LOFI:
      return 1.300f;
    case CARTRIDGE_CLEAN:
    default:
      return 1.f;
    }
  }

  static float playbackColorMixForCartridge(int mode) {
    switch (mode) {
    case CARTRIDGE_M44_7:
      return 0.60f;
    case CARTRIDGE_CONCORDE_SCRATCH:
      return 0.58f;
    case CARTRIDGE_680_HP:
      return 0.56f;
    case CARTRIDGE_QBERT:
      return 0.62f;
    case CARTRIDGE_LOFI:
    case CARTRIDGE_CLEAN:
    default:
      return 1.f;
    }
  }

  static float fastTanh(float x) {
    x = clamp(x, -3.f, 3.f);
    float x2 = x * x;
    return x * (27.f + x2) / (27.f + 9.f * x2);
  }

  void refreshCartridgeCache() {
    if (cachedCartridgeCharacter == cartridgeCharacter) {
      return;
    }
    // Cartridge mode changes are infrequent; cache derived values so the
    // per-sample path avoids repeated parameter/coeff recomputation.
    cachedCartridgeCharacter = cartridgeCharacter;
    cachedCartridgeParams = paramsForCartridge(cartridgeCharacter);
    cachedHpCoeff = onePoleCoeff(cachedCartridgeParams.hpHz);
    cachedBodyCoeff = onePoleCoeff(cachedCartridgeParams.bodyHz);
    cachedLpCoeffBase = onePoleCoeff(cachedCartridgeParams.lpHz);
    cachedLpCoeffMotion = onePoleCoeff(cachedCartridgeParams.lpMotionHz);
    cachedSaturationMix = clamp(cachedCartridgeParams.saturationMix, 0.f, 1.f);
    cachedDriveEnabled = cachedCartridgeParams.drive > 1.f && cachedSaturationMix > 1e-6f;
    cachedCrossfeed = clamp(cachedCartridgeParams.crossfeed, 0.f, 0.45f);
    cachedStereoTilt = clamp(cachedCartridgeParams.stereoTilt, -0.2f, 0.2f);
    cachedTiltLeftGain = 1.f - cachedStereoTilt * 0.5f;
    cachedTiltRightGain = 1.f + cachedStereoTilt * 0.5f;
    cachedAirMixGain = 1.f + cachedCartridgeParams.presenceGain;
    cachedBodyMixGain = cachedCartridgeParams.bodyGain - cachedCartridgeParams.presenceGain;
    cachedDriveNorm = std::max(fastTanh(cachedCartridgeParams.drive), 1e-6f);
    cachedMakeupGain = makeupGainForCartridge(cartridgeCharacter);
    cachedPlaybackColorMix = playbackColorMixForCartridge(cartridgeCharacter);
    cachedScratchCompensation = std::max(cachedCartridgeParams.scratchCompensation, 0.f);
  }

  float lofiRandUnit() {
    // Small xorshift RNG for deterministic low-cost analog-ish modulation.
    lofiRng ^= (lofiRng << 13);
    lofiRng ^= (lofiRng >> 17);
    lofiRng ^= (lofiRng << 5);
    return float(lofiRng & 0x00FFFFFFu) / float(0x01000000u);
  }

  float lofiRandSigned() { return lofiRandUnit() * 2.f - 1.f; }

  float updateLofiWowFlutter() {
    float sr = std::max(sampleRate, 1.f);
    int desiredInterval = clamp(int(std::round(sr / kLofiModControlRateHz)), 1, 64);
    if (desiredInterval != lofiModUpdateIntervalSamples) {
      lofiModUpdateIntervalSamples = desiredInterval;
      if (lofiModUpdateCountdown > lofiModUpdateIntervalSamples) {
        lofiModUpdateCountdown = lofiModUpdateIntervalSamples;
      }
    }
    if (lofiModUpdateCountdown <= 0) {
      float dt = float(lofiModUpdateIntervalSamples) / sr;
      constexpr float kTau = kTwoPi;
      lofiWowPhaseA += kTau * 0.33f * dt;
      lofiWowPhaseB += kTau * 0.57f * dt;
      lofiFlutterPhase += kTau * 7.6f * dt;
      auto wrapPhase = [&](float &phase) {
        if (phase > kTau || phase < 0.f) {
          phase = std::fmod(phase, kTau);
          if (phase < 0.f) {
            phase += kTau;
          }
        }
      };
      wrapPhase(lofiWowPhaseA);
      wrapPhase(lofiWowPhaseB);
      wrapPhase(lofiFlutterPhase);
      lofiWowFlutterCached = 0.0064f * std::sin(lofiWowPhaseA) + 0.0041f * std::sin(lofiWowPhaseB + 0.7f) +
                             0.0022f * std::sin(lofiFlutterPhase + 1.4f);
      lofiModUpdateCountdown = lofiModUpdateIntervalSamples;
    }
    lofiModUpdateCountdown -= 1;
    return lofiWowFlutterCached;
  }

  std::pair<float, float> applyCartridgeCharacter(std::pair<float, float> in, float motionAmount, bool scratchReadPath) {
    if (cartridgeCharacter == CARTRIDGE_CLEAN) {
      return in;
    }
    refreshCartridgeCache();
    const CartridgeParams &p = cachedCartridgeParams;
    motionAmount = clamp(motionAmount, 0.f, 1.f);

    // Motion amount modulates LP corner for "stylus under motion" dulling.
    // Use cached endpoint coefficients to avoid per-sample exp() in onePoleCoeff().
    float lpMotionMix = clamp(motionAmount * p.motionDulling, 0.f, 1.f);
    float lpCoeff = crossfade(cachedLpCoeffBase, cachedLpCoeffMotion, lpMotionMix);

    auto processChannel = [&](float x, CartridgeChannelState &state, float lpCoeff) {
      float rumble = state.rumble.lowpass(x, cachedHpCoeff);
      float hp = x - rumble;
      float body = state.body.lowpass(hp, cachedBodyCoeff);
      float air = state.air.lowpass(hp, lpCoeff);
      float voiced = air * cachedAirMixGain + body * cachedBodyMixGain;
      if (cachedDriveEnabled) {
        float dry = voiced;
        float sat = fastTanh(voiced * p.drive) / cachedDriveNorm;
        voiced = crossfade(dry, sat, cachedSaturationMix);
      }
      return voiced;
    };

    float left = processChannel(in.first, cartridgeLeft, lpCoeff);
    float right = processChannel(in.second, cartridgeRight, lpCoeff);
    if (cachedStereoTilt != 0.f) {
      // Lightweight stereo mismatch emulation (channel imbalance/azimuth-ish).
      left *= cachedTiltLeftGain;
      right *= cachedTiltRightGain;
    }
    if (cachedCrossfeed > 0.f) {
      float mixedL = left * (1.f - cachedCrossfeed) + right * cachedCrossfeed;
      float mixedR = right * (1.f - cachedCrossfeed) + left * cachedCrossfeed;
      left = mixedL;
      right = mixedR;
    }

    if (cartridgeCharacter == CARTRIDGE_LOFI) {
      // Sum of slow wow + quicker flutter components (deterministic, low cost).
      float wowFlutter = updateLofiWowFlutter();
      float wearTilt = 1.f + wowFlutter;

      // Semi-worn character: gentle high smearing + channel mismatch.
      float lofiBlend = clamp(0.28f + motionAmount * 0.34f, 0.f, 0.68f);
      float mono = 0.5f * (left + right);
      left = left * (1.f - lofiBlend) + mono * lofiBlend * (1.01f + 0.35f * wowFlutter);
      right = right * (1.f - lofiBlend) + mono * lofiBlend * (0.99f - 0.30f * wowFlutter);
      left *= (0.995f + 0.010f * wearTilt);
      right *= (0.995f - 0.009f * wearTilt);

      // Light vinyl bed noise.
      float hissBase = 0.0024f + 0.0018f * motionAmount;
      float hissL = hissBase * lofiRandSigned();
      float hissR = hissBase * lofiRandSigned();

      // Occasional tiny crackle pops, slightly more frequent during motion.
      float crackleChance = 0.00009f + 0.00020f * motionAmount;
      if (lofiRandUnit() < crackleChance) {
        lofiCrackleEnv = std::max(lofiCrackleEnv, 0.45f + 0.55f * lofiRandUnit());
        lofiCracklePolarity = lofiRandSigned() >= 0.f ? 1.f : -1.f;
      }
      float crackle = lofiCracklePolarity * lofiCrackleEnv * (0.010f + 0.008f * motionAmount);
      lofiCrackleEnv *= 0.87f;

      left += hissL + crackle;
      right += hissR + crackle * 0.94f;
    }
    left *= cachedMakeupGain;
    right *= cachedMakeupGain;
    if (!scratchReadPath && cartridgeCharacter != CARTRIDGE_LOFI) {
      // Keep cartridge level while reducing non-scratch coloration intensity.
      float dryL = in.first * cachedMakeupGain;
      float dryR = in.second * cachedMakeupGain;
      float playbackMix = clamp(cachedPlaybackColorMix, 0.f, 1.f);
      left = crossfade(dryL, left, playbackMix);
      right = crossfade(dryR, right, playbackMix);
    }
    return {left, right};
  }

  double currentLag() const {
    if (buffer.size <= 0) {
      return 0.0;
    }
    if (sampleModeEnabled && sampleLoaded) {
      double sampleNewest = std::max(0.0, double(sampleFrames - 1));
      return clampd(sampleNewest - readHead, 0.0, sampleNewest);
    }
    double lag = newestReadablePos() - readHead;
    if (lag < 0.0) {
      lag += double(buffer.size);
    }
    return lag;
  }

  double newestReadablePos() const {
    if (sampleModeEnabled && sampleLoaded) {
      return std::max(0.0, double(sampleFrames - 1));
    }
    if (buffer.size <= 0 || buffer.filled <= 0) {
      return 0.0;
    }
    int newest = buffer.writeHead - 1;
    if (newest < 0) {
      newest += buffer.size;
    }
    return double(newest);
  }

  float platterRadiansPerSample() const {
    return (kTwoPi * (kNominalPlatterRpm / 60.f)) / std::max(sampleRate, 1.f);
  }

  float samplesPerPlatterRadian() const { return 1.f / std::max(platterRadiansPerSample(), 1e-9f); }

  double currentLagFromNewest(double newestPos) const {
    if (buffer.size <= 0) {
      return 0.0;
    }
    if (sampleModeEnabled && sampleLoaded) {
      return sampleLagFromNewest(newestPos, readHead);
    }
    double lag = newestPos - readHead;
    if (lag < 0.0) {
      lag += double(buffer.size);
    }
    return lag;
  }

  int sampleLoopMaxIndex(double newestPos) const {
    return clamp(int(std::floor(std::max(0.0, newestPos))), 0, std::max(0, sampleFrames - 1));
  }

  double sampleLoopLength(double newestPos) const {
    return std::max(1.0, double(sampleLoopMaxIndex(newestPos)) + 1.0);
  }

  bool isSampleLoopActive() const {
    return sampleModeEnabled && sampleLoaded && sampleLoopEnabled;
  }

  double normalizeSamplePosition(double pos, double newestPos) const {
    if (isSampleLoopActive()) {
      return wrapd(pos, sampleLoopLength(newestPos));
    }
    return clampd(pos, 0.0, std::max(0.0, newestPos));
  }

  double normalizeSampleLag(double lag, double newestPos) const {
    if (isSampleLoopActive()) {
      return wrapd(lag, sampleLoopLength(newestPos));
    }
    return clampd(lag, 0.0, std::max(0.0, newestPos));
  }

  double sampleLagFromNewest(double newestPos, double readPos) const {
    if (isSampleLoopActive()) {
      return wrapd(newestPos - readPos, sampleLoopLength(newestPos));
    }
    return clampd(newestPos - readPos, 0.0, std::max(0.0, newestPos));
  }

  double unwrapReadNearWrite(double readPos, double writePos) const {
    if (sampleModeEnabled && sampleLoaded) {
      if (isSampleLoopActive()) {
        double length = sampleLoopLength(writePos);
        readPos = wrapd(readPos, length);
        while (readPos > writePos) {
          readPos -= length;
        }
        while (readPos <= writePos - length) {
          readPos += length;
        }
        return readPos;
      }
      return clampd(readPos, 0.0, std::max(0.0, double(sampleFrames - 1)));
    }
    if (buffer.size <= 0) {
      return readPos;
    }
    double sizeF = double(buffer.size);
    while (readPos > writePos) {
      readPos -= sizeF;
    }
    while (readPos <= writePos - sizeF) {
      readPos += sizeF;
    }
    return readPos;
  }


  static int clampSampleIndex(int idx, int maxIndex) {
    return clamp(idx, 0, std::max(0, maxIndex));
  }

  std::pair<float, float> readSampleBounded(double pos, int interpolationMode, double newestPos) const {
    int maxIndex = std::max(0, sampleFrames - 1);
    if (!sampleLoaded || sampleFrames <= 0 || maxIndex < 0) {
      return {0.f, 0.f};
    }
    bool loopActive = isSampleLoopActive();
    int readMaxIndex = loopActive ? sampleLoopMaxIndex(newestPos) : maxIndex;
    pos = loopActive ? normalizeSamplePosition(pos, newestPos) : clampd(pos, 0.0, double(readMaxIndex));
    int i1 = int(std::floor(pos));
    float t = float(pos - double(i1));
    const float *leftData = buffer.left.data();
    const float *rightData = buffer.monoStorage ? buffer.left.data() : buffer.right.data();
    auto wrappedIndex = [&](int idx) {
      if (!loopActive) {
        return clampSampleIndex(idx, readMaxIndex);
      }
      int length = std::max(1, readMaxIndex + 1);
      int wrapped = idx % length;
      if (wrapped < 0) {
        wrapped += length;
      }
      return wrapped;
    };
    auto leftAt = [&](int idx) { return leftData[wrappedIndex(idx)]; };
    auto rightAt = [&](int idx) { return rightData[wrappedIndex(idx)]; };

    // Exact/near-exact sample-center reads are common in transport playback.
    // Skip interpolation math when the phase is effectively integral.
    if (std::fabs(t) <= 1e-6f || std::fabs(1.f - t) <= 1e-6f) {
      int idx = clampSampleIndex(int(std::round(pos)), maxIndex);
      return {leftData[idx], rightData[idx]};
    }

    if (interpolationMode == SCRATCH_INTERP_SINC) {
      float accL = 0.f;
      float accR = 0.f;
      const TemporalDeckBuffer::SincKernel &kernel = TemporalDeckBuffer::sincKernelForFraction(t);
      bool interior = !loopActive && (i1 - (TemporalDeckBuffer::kSincRadius - 1) >= 0) &&
                      (i1 + TemporalDeckBuffer::kSincRadius <= readMaxIndex);
      if (interior) {
        for (int tap = 0; tap < TemporalDeckBuffer::kSincTapCount; ++tap) {
          int k = tap - TemporalDeckBuffer::kSincRadius + 1;
          int idx = i1 + k;
          float w = kernel.weights[size_t(tap)];
          accL += leftData[idx] * w;
          accR += rightData[idx] * w;
        }
      } else {
        for (int tap = 0; tap < TemporalDeckBuffer::kSincTapCount; ++tap) {
          int k = tap - TemporalDeckBuffer::kSincRadius + 1;
          int idx = clampSampleIndex(i1 + k, maxIndex);
          float w = kernel.weights[size_t(tap)];
          accL += leftData[idx] * w;
          accR += rightData[idx] * w;
        }
      }
      accL *= kernel.invWeightSum;
      accR *= kernel.invWeightSum;
      return {accL, accR};
    }

    if (interpolationMode == SCRATCH_INTERP_LAGRANGE6) {
      auto w = TemporalDeckBuffer::lagrange6Weights(t);
      bool interior = (i1 >= 2) && (i1 + 3 <= maxIndex);
      if (interior) {
        int i0 = i1 - 2;
        int iA = i1 - 1;
        int iB = i1 + 1;
        int iC = i1 + 2;
        int iD = i1 + 3;
        float outL = leftData[i0] * w.w0 + leftData[iA] * w.w1 + leftData[i1] * w.w2 + leftData[iB] * w.w3 +
                     leftData[iC] * w.w4 + leftData[iD] * w.w5;
        float outR = rightData[i0] * w.w0 + rightData[iA] * w.w1 + rightData[i1] * w.w2 + rightData[iB] * w.w3 +
                     rightData[iC] * w.w4 + rightData[iD] * w.w5;
        return {outL, outR};
      }
      int i0 = clampSampleIndex(i1 - 2, maxIndex);
      int iA = clampSampleIndex(i1 - 1, maxIndex);
      int iB = clampSampleIndex(i1 + 1, maxIndex);
      int iC = clampSampleIndex(i1 + 2, maxIndex);
      int iD = clampSampleIndex(i1 + 3, maxIndex);
      float outL = leftData[i0] * w.w0 + leftData[iA] * w.w1 + leftData[i1] * w.w2 + leftData[iB] * w.w3 +
                   leftData[iC] * w.w4 + leftData[iD] * w.w5;
      float outR = rightData[i0] * w.w0 + rightData[iA] * w.w1 + rightData[i1] * w.w2 + rightData[iB] * w.w3 +
                   rightData[iC] * w.w4 + rightData[iD] * w.w5;
      return {outL, outR};
    }

    bool interior = (i1 >= 1) && (i1 + 2 <= maxIndex);
    if (interior) {
      int i0 = i1 - 1;
      int i2 = i1 + 1;
      int i3 = i1 + 2;
      return {TemporalDeckBuffer::cubicSample(leftData[i0], leftData[i1], leftData[i2], leftData[i3], t),
              TemporalDeckBuffer::cubicSample(rightData[i0], rightData[i1], rightData[i2], rightData[i3], t)};
    }
    int i0 = clampSampleIndex(i1 - 1, maxIndex);
    int i2 = clampSampleIndex(i1 + 1, maxIndex);
    int i3 = clampSampleIndex(i1 + 2, maxIndex);
    return {TemporalDeckBuffer::cubicSample(leftAt(i0), leftAt(i1), leftAt(i2), leftAt(i3), t),
            TemporalDeckBuffer::cubicSample(rightAt(i0), rightAt(i1), rightAt(i2), rightAt(i3), t)};
  }

  float getLiveAbsolutePeakVolts() const {
    int16_t q = preview.getAbsolutePeakQ();
    return (float(q) / 32767.f) * temporaldeck_expander::kPreviewQuantizeVolts;
  }

  std::pair<float, float> readLiveInterpolatedAt(double pos, int interpolationMode) const {
    if (interpolationMode == SCRATCH_INTERP_LAGRANGE6) {
      return buffer.readHighQuality(pos);
    }
    if (interpolationMode == SCRATCH_INTERP_SINC) {
      return buffer.readSinc(pos);
    }
    return buffer.readCubic(pos);
  }

  void installSample(const std::vector<float> &left, const std::vector<float> &right, int frames, bool autoplay,
                     bool truncated) {
    (void)autoplay;
    sampleLoaded = frames > 0 && !left.empty();
    sampleModeEnabled = sampleLoaded || sampleModeEnabled;
    // Transport run-state is freeze-driven in sample mode. Keep transport
    // armed when a sample is loaded so unfreezing always resumes playback.
    sampleTransportPlaying = sampleLoaded;
    sampleTruncated = truncated;
    sampleFrames = std::max(0, std::min(frames, buffer.size));
    samplePlayhead = 0.0;
    readHead = 0.0;
    timelineHead = 0.0;
    buffer.filled = sampleFrames;
    buffer.writeHead = buffer.wrapIndex(sampleFrames);
    resetPreviewAccumulator();
    resetLiveScopeEnvelope();
    bumpBufferGeneration();
    if (sampleFrames <= 0) {
      return;
    }
    std::fill(buffer.left.begin(), buffer.left.end(), 0.f);
    if (!buffer.monoStorage) {
      std::fill(buffer.right.begin(), buffer.right.end(), 0.f);
    }
    for (int i = 0; i < sampleFrames; ++i) {
      float l = left[i];
      float r = right.empty() ? l : right[i];
      if (buffer.monoStorage) {
        buffer.left[i] = 0.5f * (l + r);
      } else {
        buffer.left[i] = l;
        buffer.right[i] = r;
      }
    }
    rebuildPreviewFromCurrentSample();
  }

  void installPreparedSample(std::vector<float> &&left, std::vector<float> &&right, int frames, bool truncated,
                             bool monoStorage) {
    sampleLoaded = frames > 0 && !left.empty();
    sampleModeEnabled = sampleLoaded || sampleModeEnabled;
    sampleTransportPlaying = sampleLoaded;
    sampleTruncated = truncated;
    samplePlayhead = 0.0;
    readHead = 0.0;
    timelineHead = 0.0;

    buffer.sampleRate = sampleRate;
    buffer.monoStorage = monoStorage;
    buffer.left = std::move(left);
    if (monoStorage) {
      std::vector<float>().swap(buffer.right);
    } else {
      buffer.right = std::move(right);
      if (buffer.right.size() < buffer.left.size()) {
        buffer.right.resize(buffer.left.size(), 0.f);
      }
    }
    if (buffer.left.empty()) {
      buffer.left.assign(1, 0.f);
    }
    buffer.size = std::max(1, int(buffer.left.size()));
    buffer.durationSeconds = std::max(1.f, float(buffer.size) / std::max(sampleRate, 1.f));

    sampleFrames = std::max(0, std::min(frames, buffer.size));
    buffer.filled = sampleFrames;
    buffer.writeHead = buffer.wrapIndex(sampleFrames);
    resetLiveScopeEnvelope();
    rebuildPreviewFromCurrentSample();
    bumpBufferGeneration();
  }

  bool convertLiveWindowToSample(float bufferKnob) {
    bool sampleModeActive = sampleModeEnabled && sampleLoaded && sampleFrames > 0;
    if (sampleModeActive || buffer.filled <= 0 || buffer.size <= 0) {
      return false;
    }

    double liveLimit = accessibleLag(bufferKnob);
    int capturedFrames = std::max(1, int(std::floor(std::max(0.0, liveLimit))) + 1);
    capturedFrames = std::min(capturedFrames, buffer.filled);
    if (capturedFrames <= 0) {
      return false;
    }

    std::vector<float> left(capturedFrames, 0.f);
    std::vector<float> right;
    if (!buffer.monoStorage) {
      right.assign(capturedFrames, 0.f);
    }

    int newestIndex = buffer.wrapIndex(buffer.writeHead - 1);
    int oldestIndex = buffer.wrapIndex(newestIndex - (capturedFrames - 1));
    for (int i = 0; i < capturedFrames; ++i) {
      int src = buffer.wrapIndex(oldestIndex + i);
      left[i] = buffer.left[src];
      if (!buffer.monoStorage) {
        right[i] = buffer.rightSample(src);
      }
    }

    installPreparedSample(std::move(left), std::move(right), capturedFrames, false, buffer.monoStorage);
    sampleModeEnabled = sampleLoaded;
    return sampleLoaded;
  }

  void clearScratchMotionState() {
    platterVelocity = 0.f;
    scratchHandVelocity = 0.f;
    scratchMotionVelocity = 0.f;
    scratch3LagVelocity = 0.f;
    scratch3GestureAgeSec = 0.f;
    scratchWheelVelocityBurst = 0.f;
  }

  void decayHybridWheelBurst(float dt) {
    float decay = clamp(1.f - dt * kHybridScratchWheelBurstDecayHz, 0.f, 1.f);
    scratchWheelVelocityBurst *= decay;
    if (std::fabs(scratchWheelVelocityBurst) < kHybridScratchVelocityDeadband) {
      scratchWheelVelocityBurst = 0.f;
    }
  }

  void integrateHybridScratch(float dt, double limit, double newestPos, float targetReadVelocity, float followScale,
                              float dampingScale, float correctionScale, float nowSnapThresholdSamples,
                              bool clampOvershootToTarget = false, bool allowNowSnap = true) {
    // Hybrid scratch is velocity-first internally. We still keep lag as the
    // module-facing state, but we integrate the read head in buffer space and
    // derive lag from that position so "zero hand velocity" naturally means a
    // stationary read head.
    scratchLagTargetSamples = clampLag(scratchLagTargetSamples, limit);
    scratchLagSamples = clampLag(scratchLagSamples, limit);

    float lagError = lagErrorToTarget(scratchLagTargetSamples, scratchLagSamples, newestPos);
    float correctionVelocity = clamp(-lagError * (kHybridScratchCorrectionHz * correctionScale),
                                     -kHybridScratchMaxVelocity, kHybridScratchMaxVelocity);
    float handFollowHz = kHybridScratchHandFollowHz * followScale;
    float handAlpha = clamp(dt * handFollowHz, 0.f, 1.f);
    scratchHandVelocity += (targetReadVelocity - scratchHandVelocity) * handAlpha;

    float desiredVelocity = scratchHandVelocity + scratchWheelVelocityBurst + correctionVelocity;
    float speedNorm = clamp(std::fabs(desiredVelocity) / std::max(sampleRate * 0.4f, 1.f), 0.f, 1.f);
    float accelLimit = kHybridScratchMaxAccel * (0.55f + 0.85f * speedNorm) * followScale;
    float velocityError = desiredVelocity - scratchMotionVelocity;
    bool reversingPlatter = scratchMotionVelocity * desiredVelocity < 0.f;
    float maxVelocityStep = accelLimit * (reversingPlatter ? 1.35f : 1.0f) * dt;
    scratchMotionVelocity += clamp(velocityError, -maxVelocityStep, maxVelocityStep);

    float damping = clamp(1.f - dt * (kHybridScratchVelocityDampingHz * dampingScale), 0.f, 1.f);
    float coastDamping = std::fabs(desiredVelocity) < (0.75f * kHybridScratchVelocityDeadband)
                           ? clamp(1.f - dt * (kHybridScratchVelocityDampingHz * dampingScale * 1.8f), 0.f, 1.f)
                           : damping;
    scratchMotionVelocity *= coastDamping;

    if (std::fabs(scratchHandVelocity) < kHybridScratchVelocityDeadband &&
        std::fabs(scratchMotionVelocity) < kHybridScratchVelocityDeadband && std::fabs(lagError) < 0.5f &&
        std::fabs(desiredVelocity) < kHybridScratchVelocityDeadband) {
      scratchHandVelocity = 0.f;
      scratchMotionVelocity = 0.f;
    }

    double candidate = unwrapReadNearWrite(readHead, newestPos) + double(scratchMotionVelocity) * double(dt);
    if (isSampleLoopActive()) {
      // In sample loop mode, manual scratch motion should be continuous across
      // the loop boundary rather than clamping at the window edges.
      readHead = normalizeSamplePosition(candidate, newestPos);
    } else {
      candidate = std::max(newestPos - std::max(limit, 0.0), std::min(candidate, newestPos));
      readHead = buffer.wrapPosition(candidate);
    }
    scratchLagSamples = clampLag(currentLagFromNewest(newestPos), limit);

    if (clampOvershootToTarget) {
      float lagErrorAfter = lagErrorToTarget(scratchLagTargetSamples, scratchLagSamples, newestPos);
      bool crossedTarget =
        (lagError > 0.f && lagErrorAfter < 0.f) || (lagError < 0.f && lagErrorAfter > 0.f);
      if (crossedTarget) {
        scratchLagSamples = clampLag(scratchLagTargetSamples, limit);
        scratchHandVelocity = 0.f;
        scratchMotionVelocity = 0.f;
        scratchWheelVelocityBurst = 0.f;
        double targetRead = newestPos - scratchLagSamples;
        readHead = isSampleLoopActive() ? normalizeSamplePosition(targetRead, newestPos) : buffer.wrapPosition(targetRead);
      }
    }

    if (allowNowSnap && scratchLagTargetSamples <= nowSnapThresholdSamples && scratchLagSamples <= nowSnapThresholdSamples &&
        scratchMotionVelocity >= 0.f) {
      scratchLagSamples = 0.f;
      scratchLagTargetSamples = 0.f;
      scratchHandVelocity = 0.f;
      scratchMotionVelocity = 0.f;
      scratchWheelVelocityBurst = 0.f;
      readHead = newestPos;
    }
  }

  void integrateExternalCvScratch(float dt, double limit, double newestPos, double targetLag,
                                  float nowSnapThresholdSamples) {
    bool syncMode = (externalGatePosMode == EXTERNAL_GATE_POS_MODULE_SYNC);
    if (syncMode) {
      // External gate+POS is defined as direct lag positioning in module-sync
      // mode so S.GATE/S.POS can drive another deck to the same read points.
      scratchHandVelocity = 0.f;
      scratchMotionVelocity = 0.f;
      scratchWheelVelocityBurst = 0.f;
      platterVelocity = 0.f;
      scratch3LagVelocity = 0.f;

      bool loopActive = isSampleLoopActive();
      scratchLagTargetSamples = clampLag(targetLag, limit);
      double lagEstimate = scratchLagTargetSamples;
      if (!loopActive && lagEstimate <= nowSnapThresholdSamples) {
        lagEstimate = 0.0;
        scratchLagTargetSamples = 0.0;
      }

      scratchLagSamples = lagEstimate;
      readHead = isSampleLoopActive() ? normalizeSamplePosition(newestPos - lagEstimate, newestPos)
                                      : buffer.wrapPosition(newestPos - lagEstimate);
      return;
    }

    // Glide/inertia mode: external CV scratch follows a physically limited lag
    // trajectory instead of teleporting the read head.
    scratchHandVelocity = 0.f;
    scratchMotionVelocity = 0.f;
    scratchWheelVelocityBurst = 0.f;
    platterVelocity = 0.f;

    bool loopActive = isSampleLoopActive();
    scratchLagSamples = clampLag(currentLagFromNewest(newestPos), limit);
    scratchLagTargetSamples = clampLag(targetLag, limit);

    float samplesPerRev = platter_interaction::samplesPerRevolution(sampleRate, kNominalPlatterRpm);
    float maxLagVelocity = samplesPerRev * kExternalCvMaxTurnsPerSec;
    float maxLagAccel = samplesPerRev * kExternalCvMaxTurnAccelPerSec2;

    float lagError = lagErrorToTarget(scratchLagTargetSamples, scratchLagSamples, newestPos);
    float desiredLagVelocity = clamp(lagError * kExternalCvCorrectionHz, -maxLagVelocity, maxLagVelocity);
    float maxVelStep = maxLagAccel * dt;
    scratch3LagVelocity += clamp(desiredLagVelocity - scratch3LagVelocity, -maxVelStep, maxVelStep);

    float damping = clamp(1.f - dt * kExternalCvVelocityDampingHz, 0.f, 1.f);
    scratch3LagVelocity *= damping;

    double lagEstimate = clampLag(scratchLagSamples + double(scratch3LagVelocity) * double(dt), limit);
    if (!loopActive) {
      if ((lagError > 0.f && lagEstimate > scratchLagTargetSamples) ||
          (lagError < 0.f && lagEstimate < scratchLagTargetSamples)) {
        lagEstimate = scratchLagTargetSamples;
        scratch3LagVelocity = 0.f;
      }
    }
    if (!loopActive) {
      if ((lagEstimate <= 0.0 && scratch3LagVelocity < 0.f) ||
          (lagEstimate >= limit && scratch3LagVelocity > 0.f)) {
        scratch3LagVelocity = 0.f;
      }
      if (scratchLagTargetSamples <= nowSnapThresholdSamples && lagEstimate <= nowSnapThresholdSamples &&
          scratch3LagVelocity <= 0.f) {
        lagEstimate = 0.0;
        scratch3LagVelocity = 0.f;
      }
    }

    scratchLagSamples = lagEstimate;
    readHead = isSampleLoopActive() ? normalizeSamplePosition(newestPos - lagEstimate, newestPos)
                                    : buffer.wrapPosition(newestPos - lagEstimate);
  }

  void integrateScratch3Touch(float dt, double limit, double newestPos, double prevReadHead,
                              bool hasFreshPlatterGesture, bool platterMotionActive, float platterLagTarget,
                              float platterGestureVelocity, float nowSnapThresholdSamples) {
    if (hasFreshPlatterGesture) {
      if (nowCatchActive && platterLagTarget > nowSnapThresholdSamples) {
        nowCatchActive = false;
      }
      scratchLagTargetSamples = clampLag(platterLagTarget, limit);
      scratch3GestureAgeSec = 0.f;
    } else {
      scratch3GestureAgeSec += dt;
    }

    bool gestureAlive = hasFreshPlatterGesture || platterMotionActive;
    if (!gestureAlive && std::fabs(scratch3LagVelocity) < kScratch3VelocityDeadband) {
      scratch3LagVelocity = 0.f;
      readHead = prevReadHead;
      scratchLagSamples = clampLag(currentLagFromNewest(newestPos), limit);
      scratchLagTargetSamples = scratchLagSamples;
      return;
    }

    scratchLagSamples = clampLag(scratchLagSamples, limit);
    scratchLagTargetSamples = clampLag(scratchLagTargetSamples, limit);

    float lagEstimate = scratchLagSamples;
    float lagVelocity = scratch3LagVelocity;
    if (gestureAlive) {
      float desiredLagVelocity = -platterGestureVelocity;
      float speedNorm = clamp(std::fabs(desiredLagVelocity) / std::max(sampleRate * 0.4f, 1.f), 0.f, 1.f);
      float velAlpha = clamp(dt * (kScratch3VelocityFollowHz * (0.8f + 0.6f * speedNorm)), 0.f, 1.f);
      lagVelocity += (desiredLagVelocity - lagVelocity) * velAlpha;

      float lagPred = lagEstimate + lagVelocity * dt;
      float residual = scratchLagTargetSamples - lagPred;
      lagEstimate = lagPred + kScratch3LagAlpha * residual;
      lagVelocity += (kScratch3LagBeta / std::max(dt, 1e-6f)) * residual;
    } else {
      float decay = clamp(1.f - dt * kScratch3VelocityDecayHz, 0.f, 1.f);
      lagVelocity *= decay;
      lagEstimate += lagVelocity * dt;
      if (std::fabs(lagVelocity) < kScratch3VelocityDeadband) {
        lagVelocity = 0.f;
      }
    }

    lagVelocity = clamp(lagVelocity, -kScratch3MaxLagVelocity, kScratch3MaxLagVelocity);
    lagEstimate = clampLag(lagEstimate, limit);
    scratch3LagVelocity = lagVelocity;
    scratchLagSamples = lagEstimate;
    readHead = buffer.wrapPosition(newestPos - scratchLagSamples);

    if (scratchLagTargetSamples <= nowSnapThresholdSamples && scratchLagSamples <= nowSnapThresholdSamples &&
        scratch3LagVelocity <= 0.f) {
      scratchLagSamples = 0.f;
      scratchLagTargetSamples = 0.f;
      scratch3LagVelocity = 0.f;
      readHead = newestPos;
    }
  }

  struct FrameResult {
    float outL = 0.f;
    float outR = 0.f;
    float scratchGateOut = 0.f;
    float scratchPosOut = 0.f;
    double lag = 0.0;
    double accessibleLag = 0.0;
    float platterAngle = 0.f;
    double samplePlayhead = 0.0;
    double sampleDuration = 0.0;
    double sampleProgress = 0.0;
    bool sampleMode = false;
    bool sampleLoaded = false;
    bool sampleTransportPlaying = false;
    bool autoFreezeRequested = false;
  };

  struct FrameInput {
    float dt = 0.f;
    float inL = 0.f;
    float inR = 0.f;
    float bufferKnob = 0.f;
    float rateKnob = 0.f;
    float mixKnob = 0.f;
    float feedbackKnob = 0.f;
    bool freezeButton = false;
    bool reverseButton = false;
    bool slipButton = false;
    bool quickSlipTrigger = false;
    bool freezeGate = false;
    bool scratchGate = false;
    bool scratchGateConnected = false;
    bool positionConnected = false;
    float positionCv = 0.f;
    float rateCv = 0.f;
    bool rateCvConnected = false;
    bool platterTouched = false;
    bool platterTouchHoldDirect = false;
    bool wheelScratchHeld = false;
    bool platterMotionActive = false;
    uint32_t platterGestureRevision = 0;
    float platterLagTarget = 0.f;
    float platterGestureVelocity = 0.f;
    float wheelDelta = 0.f;
  };

  FrameResult process(const FrameInput &input) {
    const float dt = input.dt;
    const float inL = input.inL;
    const float inR = input.inR;
    const float bufferKnob = input.bufferKnob;
    const float rateKnob = input.rateKnob;
    const float mixKnob = input.mixKnob;
    const float feedbackKnob = input.feedbackKnob;
    const bool freezeButton = input.freezeButton;
    const bool reverseButton = input.reverseButton;
    const bool slipButton = input.slipButton;
    const bool quickSlipTrigger = input.quickSlipTrigger;
    const bool freezeGate = input.freezeGate;
    const bool scratchGate = input.scratchGate;
    const bool scratchGateConnected = input.scratchGateConnected;
    const bool positionConnected = input.positionConnected;
    const float positionCv = input.positionCv;
    const float rateCv = input.rateCv;
    const bool rateCvConnected = input.rateCvConnected;
    const bool platterTouched = input.platterTouched;
    const bool platterTouchHoldDirect = input.platterTouchHoldDirect;
    const bool wheelScratchHeld = input.wheelScratchHeld;
    const bool platterMotionActive = input.platterMotionActive;
    const uint32_t platterGestureRevision = input.platterGestureRevision;
    const float platterLagTarget = input.platterLagTarget;
    const float platterGestureVelocity = input.platterGestureVelocity;
    const float wheelDelta = input.wheelDelta;
    FrameResult result;
    double prevReadHead = readHead;
    float nowSnapThresholdSamples = sampleRate * (kNowSnapThresholdMs / 1000.f);
    bool sampleModeActive = sampleModeEnabled && sampleLoaded && sampleFrames > 0;
    bool autoFreezeRequested = false;
    bool pinToNow = false;
    bool keepSlipLagAligned = false;
    bool keepNowCatchLagAligned = false;
    freezeState = freezeButton || freezeGate;
    reverseState = reverseButton;
    bool prevSlipState = slipState;
    slipState = slipButton;
    bool slipModeChanged = slipReturnMode != lastSlipReturnMode;
    lastSlipReturnMode = slipReturnMode;

    double sampleEndPos = sampleModeActive ? std::max(0.0, double(sampleFrames - 1)) : 0.0;
    double sampleWindowEndPos = sampleModeActive ? sampleEndPos * double(clamp(bufferKnob, 0.f, 1.f)) : 0.0;
    double limit = sampleModeActive ? sampleWindowEndPos : accessibleLag(bufferKnob);
    double minLag = 0.0;
    double maxLag = sampleModeActive ? sampleWindowEndPos : std::max(limit, 0.0);
    float baseSpeed = computeBaseSpeed(rateKnob, rateCv, rateCvConnected, reverseState);
    float prevBaseSpeedLocal = prevBaseSpeed;
    prevBaseSpeed = baseSpeed;
    float speed = baseSpeed;
    float mix = clamp(mixKnob, 0.f, 1.f);
    float feedback = clamp(feedbackKnob, 0.f, 1.f);
    bool fullyWet = (mix == 1.f);
    bool noFeedback = (feedback == 0.f);
    bool scratchGateHigh = scratchGateConnected && scratchGate;
    bool externalScratch = scratchGateHigh && positionConnected;
    bool manualTouchScratch = platterTouched;
    bool wheelScratch = wheelScratchHeld;
    bool manualScratch = manualTouchScratch || wheelScratch;
    bool sampleManualFreezeBehavior = sampleModeActive && manualTouchScratch;
    bool liveManualFreezeLikeBehavior = !sampleModeActive && manualTouchScratch;
    bool freezeForScratchModel = freezeState || sampleManualFreezeBehavior || liveManualFreezeLikeBehavior;
    bool anyScratch = externalScratch || manualScratch;
    bool wasScratchActive = scratchActive;
    bool releasedFromScratch = !anyScratch && wasScratchActive;
    constexpr float kUnityRateSnapEps = 1e-4f;
    bool enteredUnityRateFromKnob = !rateCvConnected && !reverseState && std::fabs(baseSpeed - 1.f) <= kUnityRateSnapEps &&
                                    std::fabs(prevBaseSpeedLocal - 1.f) > kUnityRateSnapEps;
    bool slipJustEnabled = slipState && !prevSlipState;
    double newestPos = sampleModeActive ? sampleWindowEndPos : newestReadablePos();
    auto snapReadHeadToSampleCenter = [&](double p) {
      double snapped = std::round(p);
      if (sampleModeActive) {
        return clampd(snapped, 0.0, sampleWindowEndPos);
      }
      return buffer.wrapPosition(snapped);
    };
    if (releasedFromScratch) {
      // One-shot post-scratch phase quantization: at most 0.5-sample movement,
      // but it returns transport to exact sample centers so interpolation fast
      // paths can engage immediately after release.
      readHead = snapReadHeadToSampleCenter(readHead);
      if (sampleModeActive) {
        samplePlayhead = readHead;
      }
    }
    if (enteredUnityRateFromKnob && !anyScratch) {
      // One-shot re-quantization when the knob returns to exact 1.0x.
      // This avoids carrying long-lived fractional phase after non-unity travel.
      readHead = snapReadHeadToSampleCenter(readHead);
      if (sampleModeActive) {
        samplePlayhead = readHead;
      }
    }
    bool hasFreshPlatterGesture = platterGestureRevision != lastPlatterGestureRevision;
    auto updateScratchControlOutputs = [&](bool gateHigh, double lagNow, bool externalScratchActive) {
      if (gateHigh && !scratchOutGateHigh) {
        bool syncMode = (externalGatePosMode == EXTERNAL_GATE_POS_MODULE_SYNC);
        // Manual/wheel gestures are always relative to gate-rise lag. In
        // module-sync mode, external gate+POS preserves its own gate-rise
        // anchor so held non-zero POS is reflected immediately on S.POS.
        scratchOutAnchorLagSamples = (syncMode && externalScratchActive) ? externalCvAnchorLagSamples : lagNow;
      }
      scratchOutGateHigh = gateHigh;
      result.scratchGateOut = gateHigh ? 10.f : 0.f;
      if (!gateHigh) {
        // Keep S_POS at 0 whenever S_GATE is low.
        result.scratchPosOut = 0.f;
        scratchOutAnchorLagSamples = lagNow;
      } else {
        // Keep S_POS polarity consistent with POS input:
        // +V = forward/toward NOW, -V = backward/deeper lag.
        float posSec = float((scratchOutAnchorLagSamples - lagNow) / std::max(sampleRate, 1.f));
        result.scratchPosOut = clamp(posSec, -10.f, 10.f);
      }
    };
    bool fastSampleTransportPath =
      sampleModeActive && !anyScratch && !wasScratchActive && !slipState && !prevSlipState &&
      !slipReturning && !slipBlendActive && !nowCatchActive && !quickSlipTrigger && !externalCvGateHigh;
    if (fastSampleTransportPath) {
      if (!sampleTransportPlaying || freezeState) {
        speed = 0.f;
      }
      if (sampleLoopEnabled) {
        samplePlayhead = normalizeSamplePosition(samplePlayhead + double(speed), sampleWindowEndPos);
      } else {
        samplePlayhead = clampd(samplePlayhead + double(speed), 0.0, sampleWindowEndPos);
        if (samplePlayhead >= sampleWindowEndPos && speed > 0.f) {
          samplePlayhead = sampleWindowEndPos;
          autoFreezeRequested = true;
        }
        if (samplePlayhead <= 0.0 && speed < 0.f) {
          samplePlayhead = 0.0;
          autoFreezeRequested = true;
        }
      }
      readHead = samplePlayhead;
      newestPos = sampleWindowEndPos;

      std::pair<float, float> wet = readSampleBounded(readHead, SCRATCH_INTERP_CUBIC, sampleWindowEndPos);
      float readDeltaForTone = float(readHead - prevReadHead);
      float motionAmount = clamp(float((std::fabs(readDeltaForTone) - 1.0) / 3.0), 0.f, 1.f);
      wet = applyCartridgeCharacter(wet, motionAmount, false);

      scratchFlipTransientEnv *= 0.92f;
      if (scratchFlipTransientEnv < 1e-4f) {
        scratchFlipTransientEnv = 0.f;
        prevScratchDeltaSign = 0;
      }
      scratchDcInL = wet.first;
      scratchDcInR = wet.second;
      scratchDcOutL = 0.f;
      scratchDcOutR = 0.f;

      prevScratchReadDelta = readDeltaForTone;
      prevWetL = wet.first;
      prevWetR = wet.second;
      prevScratchOutL = wet.first;
      prevScratchOutR = wet.second;

      if (fullyWet) {
        result.outL = wet.first;
        result.outR = wet.second;
      } else {
        result.outL = inL * (1.f - mix) + wet.first * mix;
        result.outR = inR * (1.f - mix) + wet.second * mix;
      }

      double visualDelta = readHead - prevReadHead;
      platterPhase += float(visualDelta) * platterRadiansPerSample();
      if (platterPhase > kPi || platterPhase < -kPi) {
        platterPhase = std::fmod(platterPhase, kTwoPi);
      }

      scratchActive = false;
      externalCvGateHigh = false;
      result.lag = currentLagFromNewest(newestPos);
      result.accessibleLag = sampleWindowEndPos;
      updateScratchControlOutputs(anyScratch, result.lag, externalScratch);
      result.platterAngle = platterPhase;
      result.sampleMode = true;
      result.sampleLoaded = sampleLoaded;
      result.sampleTransportPlaying = sampleTransportPlaying;
      result.autoFreezeRequested = autoFreezeRequested;
      double sampleUiEndFrame = sampleWindowEndPos;
      double sampleUiFrame = clampd(readHead, 0.0, sampleUiEndFrame);
      result.samplePlayhead = std::max(0.0, sampleUiFrame / std::max(sampleRate, 1.f));
      result.sampleDuration = std::max(0.0, (sampleUiEndFrame + 1.0) / std::max(sampleRate, 1.f));
      result.sampleProgress = sampleUiEndFrame > 0.0 ? clampd(sampleUiFrame / sampleUiEndFrame, 0.0, 1.0) : 0.0;
      return result;
    }
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
      if (!sampleModeActive && manualTouchScratch) {
        liveManualScratchAnchorNewestPos = newestPos;
        liveManualScratchAnchorLagSamples = scratchLagSamples;
      }
      if (sampleModeActive) {
        sampleSlipAnchorPos = normalizeSamplePosition(readHead, sampleWindowEndPos);
      }
      clearScratchMotionState();
    }

    if (scratchGateHigh && !externalCvGateHigh) {
      externalCvAnchorLagSamples = currentLagFromNewest(newestPos);
      if (positionConnected) {
        // Gate-rise latches an anchor and applies the current POS offset
        // immediately, so held non-zero POS does not glide into place.
        double targetLag = clampLag(externalCvAnchorLagSamples + lagOffsetForPositionCv(positionCv), limit);
        scratchLagSamples = targetLag;
        scratchLagTargetSamples = targetLag;
        scratch3LagVelocity = 0.f;
        double targetRead = newestPos - targetLag;
        readHead = isSampleLoopActive() ? normalizeSamplePosition(targetRead, newestPos) : buffer.wrapPosition(targetRead);
      }
    }
    externalCvGateHigh = scratchGateHigh;

    scratchActive = anyScratch;

    bool quickSlipActive = slipReturnOverrideTime >= 0.f;
    if (!slipState && !quickSlipActive) {
      cancelSlipReturnState();
    }

    auto beginSlipReturn = [&]() {
      slipReturning = true;
      slipBlendActive = false;
      slipCatchVelocity = 0.f;
      slipBlendRemaining = 0.f;
      slipBlendStartReadHead = readHead;
      slipBlendStartLag = currentLagFromNewest(newestPos);
      nowCatchActive = false;
    };

    if (releasedFromScratch && slipState) {
      if (sampleModeActive) {
        slipReturning = true;
        slipBlendActive = false;
        sampleSlipVelocity = 0.f;
        nowCatchActive = false;
      } else {
        beginSlipReturn();
      }
    }

    if (releasedFromScratch && !sampleModeActive && !slipReturning && !slipBlendActive &&
        currentLagFromNewest(newestPos) <= nowSnapThresholdSamples) {
      startNowCatch(currentLagFromNewest(newestPos));
      cancelSlipReturnState();
    }

    if (slipJustEnabled && !anyScratch) {
      if (sampleModeActive) {
        sampleSlipAnchorPos = normalizeSamplePosition(readHead, sampleWindowEndPos);
      } else if (currentLagFromNewest(newestPos) > kSlipEnableReturnThreshold) {
        beginSlipReturn();
      }
    }

    // If slip speed mode changes while Slip is active and we are behind NOW,
    // immediately engage a return so the new mode takes audible effect.
    if (slipModeChanged && slipState && !anyScratch &&
        (!sampleModeActive ? currentLagFromNewest(newestPos) > nowSnapThresholdSamples : slipReturning)) {
      if (!sampleModeActive) {
        beginSlipReturn();
      }
    }

    if (quickSlipTrigger && !anyScratch && !sampleModeActive && currentLagFromNewest(newestPos) > nowSnapThresholdSamples) {
      // Quick slip uses quick voicing but has a hard timeout so it cannot drag
      // on indefinitely for long return distances.
      slipReturnOverrideTime = kQuickSlipMaxReturnTime;
      beginSlipReturn();
    }

    if (anyScratch || freezeForScratchModel) {
      cancelSlipReturnState();
    }

    if (sampleModeActive) {
      if (releasedFromScratch) {
        samplePlayhead = normalizeSamplePosition(readHead, sampleWindowEndPos);
        scratchLagSamples = 0.0;
        scratchLagTargetSamples = 0.0;
      }
      bool sampleSlipJustCompleted = false;
      if (slipReturning || slipBlendActive) {
        speed = 0.f;
        sampleSlipJustCompleted = integrateSampleSlipReturn(sampleWindowEndPos, dt);
      }
      if (!sampleTransportPlaying || freezeForScratchModel) {
        speed = 0.f;
      }
      if ((!(slipReturning || slipBlendActive) || sampleSlipJustCompleted) && sampleLoopEnabled) {
        samplePlayhead = normalizeSamplePosition(samplePlayhead + double(speed), sampleWindowEndPos);
      } else if ((!(slipReturning || slipBlendActive) || sampleSlipJustCompleted)) {
        samplePlayhead = clampd(samplePlayhead + double(speed), 0.0, sampleWindowEndPos);
        if (samplePlayhead >= sampleWindowEndPos && speed > 0.f) {
          samplePlayhead = sampleWindowEndPos;
          if (!freezeForScratchModel) {
            autoFreezeRequested = true;
          }
        }
        if (samplePlayhead <= 0.0 && speed < 0.f) {
          samplePlayhead = 0.0;
          if (!freezeForScratchModel) {
            autoFreezeRequested = true;
          }
        }
      }
    }

    // 3. Determine actual playhead (readHead)
    if (freezeForScratchModel) {
      speed = 0.f;
    }

    newestPos = sampleModeActive ? sampleWindowEndPos : newestReadablePos();
    float lagNow = currentLagFromNewest(newestPos);
    bool reverseAtOldestEdge =
      !sampleLoopEnabled && !scratchActive && !slipReturning && !slipBlendActive &&
      reverseState && limit > 0.f && lagNow >= (limit - 0.5f);
    if (reverseAtOldestEdge && speed < 0.f) {
      speed = 0.f;
      if (!freezeForScratchModel) {
        autoFreezeRequested = true;
      }
    }

    if (manualScratch) {
      // Hybrid scratch keeps lag as the authoritative state, but moves it
      // from an integrated motion model instead of direct target chasing.
      scratchLagSamples = clampLag(currentLagFromNewest(newestPos), limit);
      if (manualTouchScratch) {
        scratchWheelVelocityBurst = 0.f;
        if (hasFreshPlatterGesture) {
          if (nowCatchActive && platterLagTarget > nowSnapThresholdSamples) {
            nowCatchActive = false;
          }
          double targetLag = clampLag(platterLagTarget, limit);
          if (!sampleModeActive && freezeState) {
            // Explicit freeze in live mode keeps write-head drift compensation
            // so held gestures stay anchored while transport is paused.
            double driftLag = std::max(0.0, newestPos - liveManualScratchAnchorNewestPos);
            double nearNowWindow = std::max(1.0, double(sampleRate) * 0.060);
            double driftMix = 1.0;
            if (targetLag < nearNowWindow) {
              driftMix = clampd(targetLag / nearNowWindow, 0.0, 1.0);
            }
            targetLag += driftLag * driftMix;
            // Keep drift compensation anchored to scratch-start timing so
            // live drag targets stay aligned with write-head progression.
            // The anchor is only resynced explicitly at buffer-limit hold.
          }
          scratchLagTargetSamples = clampLag(targetLag, limit);
          if (!sampleModeActive && limit > 0.0 && scratchLagSamples >= (limit - 0.5) &&
              scratchLagTargetSamples >= (limit - 0.5)) {
            // When manual touch scratch is pinned at the live readable limit,
            // keep the drag anchor in step with the advancing write head.
            // This avoids a growing drift term that can make forward movement
            // feel sticky after holding at the red limit.
            liveManualScratchAnchorNewestPos = newestPos;
          }
          if (!sampleModeActive && freezeState && limit > 0.0 && scratchLagSamples >= (limit - 0.5) &&
              scratchLagTargetSamples < (scratchLagSamples - 1.0)) {
            // Edge-release assist: when the hand moves toward NOW after being
            // pinned at the live readable limit, immediately release from the
            // clamp point so movement doesn't feel sticky.
            scratchLagSamples = scratchLagTargetSamples;
            scratchHandVelocity = 0.f;
            scratchMotionVelocity = 0.f;
            scratch3LagVelocity = 0.f;
            readHead = buffer.wrapPosition(newestPos - scratchLagSamples);
          }
          if (!sampleModeActive && freezeState && scratchLagTargetSamples < (scratchLagSamples - 1.0)) {
            // Live touch gestures that move toward NOW should feel direct and
            // not be dominated by accumulated lag-state inertia.
            scratchLagSamples = scratchLagTargetSamples;
            scratchHandVelocity = 0.f;
            scratchMotionVelocity = 0.f;
            scratch3LagVelocity = 0.f;
            readHead = buffer.wrapPosition(newestPos - scratchLagSamples);
          }
          lastPlatterGestureRevision = platterGestureRevision;
        }

        bool directTouchHoldActive = manualTouchScratch && platterTouchHoldDirect;
        if (!directTouchHoldActive) {
          platterTouchHoldLatched = false;
        }

        bool stationaryManualHold = !platterMotionActive && !hasFreshPlatterGesture;
        if (directTouchHoldActive) {
          // Direct-position requests are only for stationary hold behavior.
          // Active scope movement should arrive through the normal scratch
          // gesture path instead.
          double targetLag = clampLag(platterLagTarget, limit);
          if (!platterTouchHoldLatched || hasFreshPlatterGesture) {
            platterTouchHoldReadHead =
              isSampleLoopActive() ? normalizeSamplePosition(newestPos - targetLag, newestPos)
                                   : buffer.wrapPosition(newestPos - targetLag);
            platterTouchHoldLatched = true;
          }
          readHead = platterTouchHoldReadHead;
          double heldLag = clampLag(currentLagFromNewest(newestPos), limit);
          scratchLagSamples = heldLag;
          scratchLagTargetSamples = heldLag;
          scratchHandVelocity = 0.f;
          scratchMotionVelocity = 0.f;
          scratch3LagVelocity = 0.f;
          lastPlatterGestureRevision = platterGestureRevision;
        } else if (stationaryManualHold) {
          scratchLagTargetSamples = scratchLagSamples;
          scratchHandVelocity = 0.f;
          scratchMotionVelocity = 0.f;
        } else {
          bool manualMotionActive = platter_interaction::hasActiveManualMotion(hasFreshPlatterGesture, platterMotionActive);
          float targetReadVelocity = 0.f;
          if (manualMotionActive) {
            // While motion is fresh, gesture velocity is relative to the write
            // head. Convert it into absolute read velocity by adding the write
            // baseline (except in freeze, where write head is stationary).
            targetReadVelocity = platterGestureVelocity;
            if (platter_interaction::shouldApplyWriteHeadCompensation(freezeForScratchModel, hasFreshPlatterGesture,
                                                                      platterMotionActive)) {
              targetReadVelocity += sampleRate;
            }
          }
          float gestureDirection = 0.f;
          if (hasFreshPlatterGesture) {
            float lagDeltaSinceLastGesture = platterLagTarget - float(lastPlatterLagTarget);
            if (std::fabs(lagDeltaSinceLastGesture) > 1e-4f) {
              // Lag increasing means moving farther from NOW (backward read);
              // lag decreasing means moving toward NOW (forward read).
              gestureDirection = (lagDeltaSinceLastGesture > 0.f) ? -1.f : 1.f;
            }
          }
          if (gestureDirection == 0.f && std::fabs(targetReadVelocity) > kHybridScratchVelocityDeadband) {
            gestureDirection = (targetReadVelocity > 0.f) ? 1.f : -1.f;
          }
          bool reverseGestureIntent = gestureDirection < 0.f;
          if (manualTouchScratch && manualMotionActive && platterGestureVelocity < 0.f) {
            // CONTAINMENT NOTE
            // This branch exists because slow live drags away from NOW were
            // getting classified too weakly to overcome write-head baseline
            // compensation, which manifested as a downward "barrier" in
            // TD.Scope-driven drag. The cleaner architecture would be for the
            // expander/host contract to encode this intent unambiguously before
            // it gets here; for now the engine preserves reverse intent for any
            // negative touch velocity during active manual motion.
            // In live touch drag, even slow negative gesture velocity means the
            // user is pulling away from NOW. Preserve reverse intent so write
            // compensation does not create a false downward "barrier".
            reverseGestureIntent = true;
          } else if (platterGestureVelocity < -kHybridScratchVelocityDeadband) {
            // Preserve reverse intent across sparse gesture updates. Using raw
            // platter velocity avoids sign flips caused by write compensation.
            reverseGestureIntent = true;
          }
          if (!sampleModeActive && !freezeForScratchModel && reverseGestureIntent) {
            // In live touch scratch, reverse gestures should not need to
            // overcome write-head baseline speed before audible/visual motion
            // appears. Keep compensation for forward motion only.
            targetReadVelocity -= sampleRate;
          }
          float motionNorm = clamp(std::fabs(targetReadVelocity) / std::max(sampleRate * 0.45f, 1.f), 0.f, 1.f);
          bool allowNowSnap = !manualTouchScratch;
          double preIntegrateReadHead = unwrapReadNearWrite(readHead, newestPos);
          integrateHybridScratch(dt, limit, newestPos, targetReadVelocity, 1.55f + 0.55f * motionNorm,
                                 0.72f - 0.12f * motionNorm, 0.68f, nowSnapThresholdSamples, false, allowNowSnap);
          if (!sampleModeActive && manualMotionActive && gestureDirection != 0.f) {
            double postIntegrateReadHead = unwrapReadNearWrite(readHead, newestPos);
            double deltaRead = postIntegrateReadHead - preIntegrateReadHead;
            // During active live drag, never allow motion opposite to gesture
            // direction. This prevents sporadic forward steps while dragging
            // backward (and vice versa) when lag-correction terms disagree.
            bool oppositeMotion =
              (gestureDirection > 0.f && deltaRead < 0.0) || (gestureDirection < 0.f && deltaRead > 0.0);
            if (oppositeMotion) {
              readHead = buffer.wrapPosition(preIntegrateReadHead);
              scratchLagSamples = clampLag(currentLagFromNewest(newestPos), limit);
            }
          }
        }
      } else {
        // Wheel scratch uses the same Hybrid motion model as drag scratch.
        float wheelDeltaSoftRange = sampleRate * 0.16f * kWheelScratchTravelScale;
        float wheelDeltaShaped = wheelDeltaSoftRange * std::tanh(wheelDelta / std::max(wheelDeltaSoftRange, 1e-6f));
        if (!sampleModeActive && !freezeForScratchModel && wheelDeltaShaped < 0.f) {
          // In live circular mode, small toward-NOW wheel nudges must overcome
          // moving write-head drift. Give forward wheel ticks a slight assist.
          wheelDeltaShaped *= 1.35f;
        }
        if (std::fabs(wheelDelta) > 1e-6f) {
          scratchLagTargetSamples = clampLag(scratchLagTargetSamples + wheelDeltaShaped, limit);
          float wheelNowSnapThreshold = sampleRate * 0.012f;
          if (scratchLagTargetSamples < wheelNowSnapThreshold) {
            scratchLagTargetSamples = 0.f;
          }
          scratchWheelVelocityBurst -= wheelDeltaShaped / std::max(kHybridScratchWheelImpulseTime, 1e-6f);
          scratchWheelVelocityBurst =
            clamp(scratchWheelVelocityBurst, -kHybridScratchMaxVelocity, kHybridScratchMaxVelocity);
        }
        decayHybridWheelBurst(dt);
        bool freshForwardWheelImpulse = std::fabs(wheelDelta) > 1e-6f && wheelDeltaShaped < 0.f;
        if (sampleModeActive && freshForwardWheelImpulse) {
          float wheelLagError = lagErrorToTarget(scratchLagTargetSamples, scratchLagSamples, newestPos);
          if (wheelLagError < -0.5f) {
            // In sample mode, a forward wheel nudge should read as an actual
            // scratch gesture, not just barely keep pace with 1.0x transport.
            float minForwardBurst = sampleRate * kSampleWheelForwardBurstFloorRatio;
            scratchWheelVelocityBurst = std::max(scratchWheelVelocityBurst, minForwardBurst);
          }
        }
        // In live circular mode, wheel scratch nudges lag around a moving write
        // head. In sample mode there is no moving write head, so keep this
        // neutral to preserve forward/back symmetry.
        float wheelTargetReadVelocity = (sampleModeActive || freezeForScratchModel) ? 0.f : sampleRate;
        bool clampWheelOvershoot = sampleModeActive;
        integrateHybridScratch(dt, limit, newestPos, wheelTargetReadVelocity, 1.25f, 0.95f, 1.20f,
                               nowSnapThresholdSamples, clampWheelOvershoot, true);
      }
      lastPlatterLagTarget = platterLagTarget;
    } else if (externalScratch) {
      // POSITION CV is interpreted as signed time offset around a lag anchor
      // latched on gate rise. While gate is held, the anchor naturally drifts
      // deeper into the buffer with write-head progression.
      double targetLag = externalCvAnchorLagSamples + lagOffsetForPositionCv(positionCv);
      targetLag = clampLag(targetLag, limit);
      integrateExternalCvScratch(dt, limit, newestPos, targetLag, nowSnapThresholdSamples);
    } else if ((slipReturning || slipBlendActive) && !sampleModeActive && !freezeForScratchModel) {
      if (integrateSlipCatchup(newestPos, maxLag, speed, dt)) {
        pinToNow = true;
      }
    } else {
      // Normal Transport
      if (sampleModeActive) {
        // In sample mode, transport playback follows the bounded transport
        // cursor, not the scratch lag reference frame.
        readHead = samplePlayhead;
      } else {
        double candidate = unwrapReadNearWrite(readHead, newestPos) + double(speed);
        candidate = std::max(newestPos - maxLag, std::min(candidate, newestPos - minLag));
        readHead = buffer.wrapPosition(candidate);
      }
    }

    if (!slipReturning && !slipBlendActive) {
      slipReturnOverrideTime = -1.f;
    }

    if (nowCatchActive && !sampleModeActive && !slipReturning && !slipBlendActive) {
      nowCatchRemaining = std::max(0.f, nowCatchRemaining - dt);
      float progress = 1.f - clamp(nowCatchRemaining / std::max(kNowCatchTime, 1e-6f), 0.f, 1.f);
      float shapedProgress = progress * (2.f - progress);
      double targetLag = nowCatchStartLag * double(1.f - shapedProgress);
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

    bool holdAtReverseEdge = reverseAtOldestEdge;
    bool holdAtBufferEdge = holdAtReverseEdge;

    bool scratchReadPath = anyScratch;
    bool slipReadPath = !sampleModeActive && (slipReturning || slipBlendActive);
    bool variableRateReadPath = scratchReadPath || slipReadPath;
    int effectiveScratchInterpolation = clamp(scratchInterpolationMode, SCRATCH_INTERP_CUBIC,
                                              SCRATCH_INTERP_COUNT - 1);
    double readDeltaForTone = readHead - prevReadHead;
    if (buffer.size > 0) {
      double halfSize = double(buffer.size) * 0.5;
      if (readDeltaForTone > halfSize) {
        readDeltaForTone -= double(buffer.size);
      }
      if (readDeltaForTone < -halfSize) {
        readDeltaForTone += double(buffer.size);
      }
    }
    float motionAmount = clamp(float((std::fabs(readDeltaForTone) - 1.0) / 3.0), 0.f, 1.f);
    if (scratchReadPath) {
      // Preserve more buffer detail during scratching by reducing motion-driven
      // cartridge darkening.
      motionAmount *= 0.4f;
    }
    std::pair<float, float> wet;
    if (sampleModeActive) {
      int sampleInterp = SCRATCH_INTERP_CUBIC;
      if (scratchReadPath) {
        sampleInterp = effectiveScratchInterpolation;
      } else if (highQualityRateInterpolation) {
        sampleInterp = SCRATCH_INTERP_LAGRANGE6;
      }
      wet = readSampleBounded(readHead, sampleInterp, sampleWindowEndPos);
    } else if (slipBlendActive) {
      std::pair<float, float> catchWet = readLiveInterpolatedAt(readHead, effectiveScratchInterpolation);
      std::pair<float, float> liveWet = readLiveInterpolatedAt(newestPos, effectiveScratchInterpolation);
      float blendProgress = 1.f - clamp(slipBlendRemaining / std::max(kSlipBlendTime, 1e-6f), 0.f, 1.f);
      float theta = blendProgress * 0.5f * kPi;
      float catchGain = std::cos(theta);
      float liveGain = std::sin(theta);
      wet.first = catchWet.first * catchGain + liveWet.first * liveGain;
      wet.second = catchWet.second * catchGain + liveWet.second * liveGain;
      slipBlendRemaining = std::max(0.f, slipBlendRemaining - dt);
      if (slipBlendRemaining <= 0.f) {
        readHead = newestPos;
        cancelSlipReturnState();
        pinToNow = true;
      }
    } else if (variableRateReadPath) {
      wet = readLiveInterpolatedAt(readHead, effectiveScratchInterpolation);
    } else {
      wet = buffer.readCubic(readHead);
    }
    wet = applyCartridgeCharacter(wet, motionAmount, scratchReadPath);
    if (slipReadPath) {
      float slipSpeedNorm =
        clamp(slipCatchVelocity / std::max(sampleRate * std::max(slipCatchMaxExtraRatio(), 0.1f), 1.f), 0.f, 1.f);
      float slipSpeedToneNorm = std::pow(slipSpeedNorm, 1.25f);
      float cutoffHz = crossfade(kSlipDynamicLpCutoffHighHz, kSlipDynamicLpCutoffLowHz, slipSpeedToneNorm);
      float lpCoeff = onePoleCoeff(cutoffHz);
      if (!slipDynLpPrimed) {
        slipDynLpStateL = wet.first;
        slipDynLpStateR = wet.second;
        slipDynLpPrimed = true;
      }
      slipDynLpStateL += (wet.first - slipDynLpStateL) * lpCoeff;
      slipDynLpStateR += (wet.second - slipDynLpStateR) * lpCoeff;
      float lpMix = crossfade(kSlipDynamicLpMixLow, kSlipDynamicLpMixHigh, slipSpeedToneNorm);
      wet.first = crossfade(wet.first, slipDynLpStateL, lpMix);
      wet.second = crossfade(wet.second, slipDynLpStateR, lpMix);
    } else {
      slipDynLpPrimed = false;
      slipDynLpStateL = wet.first;
      slipDynLpStateR = wet.second;
    }
    if (scratchReadPath) {
      if (manualScratch) {
        // The hybrid motion model is meant to stand on its own, so keep only a
        // light continuity/de-click layer instead of the full legacy cleanup
        // stack.
        scratchFlipTransientEnv = 0.f;
        prevScratchDeltaSign = 0;
        scratchDcInL = wet.first;
        scratchDcInR = wet.second;
        scratchDcOutL = 0.f;
        scratchDcOutR = 0.f;

        float microStepAmt = clamp((0.95f - std::fabs(readDeltaForTone)) / 0.95f, 0.f, 1.f);
        float continuityAmt = wheelScratch ? (0.30f * microStepAmt) : (0.20f * microStepAmt);
        wet.first = crossfade(wet.first, prevScratchOutL, continuityAmt);
        wet.second = crossfade(wet.second, prevScratchOutR, continuityAmt);

        float ultraSlowAmt = clamp((0.32f - std::fabs(readDeltaForTone)) / 0.32f, 0.f, 1.f);
        float smoothAmt = manualTouchScratch ? (0.08f * ultraSlowAmt) : (0.05f * ultraSlowAmt);
        wet.first = crossfade(wet.first, prevWetL, smoothAmt);
        wet.second = crossfade(wet.second, prevWetR, smoothAmt);
      } else {
        int deltaSign = (readDeltaForTone > 0.2f) ? 1 : ((readDeltaForTone < -0.2f) ? -1 : 0);
        if (deltaSign != 0 && prevScratchDeltaSign != 0 && deltaSign != prevScratchDeltaSign) {
          float jerk = std::fabs(readDeltaForTone - prevScratchReadDelta);
          scratchFlipTransientEnv = std::max(scratchFlipTransientEnv, clamp(jerk * 0.18f, 0.f, 1.f));
        }
        if (deltaSign != 0) {
          prevScratchDeltaSign = deltaSign;
        }
        // Derive transient from real signal edge change, not synthetic impulse.
        float detailMid = 0.5f * ((wet.first - prevWetL) + (wet.second - prevWetR));
        float transientMotion = clamp((std::fabs(readDeltaForTone) - 1.15f) / 1.9f, 0.f, 1.f);
        float transientBase = wheelScratch ? 0.06f : (manualTouchScratch ? 0.14f : 0.30f);
        float transient = detailMid * (transientBase * scratchFlipTransientEnv * transientMotion);
        wet.first += transient * (prevScratchDeltaSign >= 0 ? 1.0f : 0.9f);
        wet.second += transient * (prevScratchDeltaSign <= 0 ? 1.0f : 0.9f);
        scratchFlipTransientEnv *= 0.968f;

        // Slow reverse glide: trim sub-rumble and suppress tiny discontinuity
        // clicks without flattening fast scratch articulation.
        float slowReverseAmt = 0.f;
        if (manualScratch && readDeltaForTone < 0.f) {
          slowReverseAmt = clamp((1.35f - std::fabs(readDeltaForTone)) / 1.35f, 0.f, 1.f);
        }
        if (slowReverseAmt > 0.f) {
          constexpr float kDcR = 0.995f;
          float hpL = wet.first - scratchDcInL + kDcR * scratchDcOutL;
          float hpR = wet.second - scratchDcInR + kDcR * scratchDcOutR;
          scratchDcInL = wet.first;
          scratchDcInR = wet.second;
          scratchDcOutL = hpL;
          scratchDcOutR = hpR;
          float rumbleTrim = 0.55f * slowReverseAmt;
          wet.first = crossfade(wet.first, hpL, rumbleTrim);
          wet.second = crossfade(wet.second, hpR, rumbleTrim);
        }

        float microStepAmt = clamp((1.1f - std::fabs(readDeltaForTone)) / 1.1f, 0.f, 1.f);
        float deClickAmt = wheelScratch ? (0.48f * microStepAmt)
                                        : (manualTouchScratch ? (0.38f * microStepAmt) : (0.22f * microStepAmt));
        wet.first = crossfade(wet.first, prevScratchOutL, deClickAmt);
        wet.second = crossfade(wet.second, prevScratchOutR, deClickAmt);

        // Manual glides can still sound "needle-grindy" from fast micro-edge
        // changes; add a tiny adaptive smoothing layer only in manual mode.
        if (manualScratch) {
          float grindAmt = clamp((1.55f - std::fabs(readDeltaForTone)) / 1.55f, 0.f, 1.f);
          float smoothAmt = 0.16f * grindAmt;
          wet.first = crossfade(wet.first, prevWetL, smoothAmt);
          wet.second = crossfade(wet.second, prevWetR, smoothAmt);
        }
      }
    } else {
      scratchFlipTransientEnv *= 0.92f;
      if (scratchFlipTransientEnv < 1e-4f) {
        scratchFlipTransientEnv = 0.f;
        prevScratchDeltaSign = 0;
      }
      scratchDcInL = wet.first;
      scratchDcInR = wet.second;
      scratchDcOutL = 0.f;
      scratchDcOutR = 0.f;
    }
    if (anyScratch && cachedScratchCompensation > 0.f) {
      // Small per-cartridge gain recovery during active scratch motion.
      // This compensates perceived loudness dip from interpolation + de-click
      // smoothing without flattening the cartridge tone differences.
      float warpAmount = clamp(float(std::fabs(readDeltaForTone) / 3.0), 0.f, 1.f);
      float comp = 1.f + cachedScratchCompensation * (0.35f + 0.65f * warpAmount);
      wet.first *= comp;
      wet.second *= comp;
    }
    prevScratchReadDelta = readDeltaForTone;
    prevWetL = wet.first;
    prevWetR = wet.second;
    prevScratchOutL = wet.first;
    prevScratchOutR = wet.second;
    float outL = 0.f;
    float outR = 0.f;
    if (fullyWet) {
      outL = wet.first;
      outR = wet.second;
    } else {
      outL = inL * (1.f - mix) + wet.first * mix;
      outR = inR * (1.f - mix) + wet.second * mix;
    }

    double lagPreWriteForVisual = 0.0;
    bool useLagPreWriteForVisual = !sampleModeActive && manualTouchScratch && !freezeState;
    if (useLagPreWriteForVisual) {
      // Capture lag before live write-head advancement so platter visuals can
      // reflect hand-induced motion, not transport drift.
      lagPreWriteForVisual = currentLagFromNewest(newestPos);
    }

    bool writeAdvanced = false;
    if (!sampleModeActive && !freezeState && !holdAtBufferEdge) {
      float writeL = 0.f;
      float writeR = 0.f;
      if (noFeedback) {
        writeL = inL;
        writeR = inR;
      } else {
        writeL = inL + outL * feedback;
        writeR = inR + outR * feedback;
      }
      pushLiveScopeEnvelopeSample(writeL, writeR);
      buffer.write(writeL, writeR);
      preview.pushMonoSample(0.5f * (writeL + writeR));
      writeAdvanced = true;
      newestPos = newestReadablePos();
      if (pinToNow) {
        readHead = newestPos;
      } else if (keepSlipLagAligned || keepNowCatchLagAligned) {
        readHead = buffer.wrapPosition(readHead + 1.f);
      }
    }
    if (externalCvGateHigh && writeAdvanced) {
      // Keep anchored POS reference in write-head time while gate is held.
      externalCvAnchorLagSamples += 1.0;
    }
    if (anyScratch && writeAdvanced) {
      // Keep S_POS output anchored in write-head time while S_GATE is high.
      // This avoids output drift when the read head is held and live input
      // continues advancing the write head.
      scratchOutAnchorLagSamples += 1.0;
    }

    if (buffer.size > 0) {
      double readDelta = readHead - prevReadHead;
      double halfSize = double(buffer.size) * 0.5;
      if (readDelta > halfSize) {
        readDelta -= double(buffer.size);
      }
      if (readDelta < -halfSize) {
        readDelta += double(buffer.size);
      }
      // Drive the platter UI from actual read-head movement so the visual stays
      // synchronized when transport is causality-limited near NOW.
      double visualDelta = readDelta;
      if (useLagPreWriteForVisual) {
        visualDelta = double(lagNow) - lagPreWriteForVisual;
      }
      platterPhase += float(visualDelta) * platterRadiansPerSample();
      if (platterPhase > kPi || platterPhase < -kPi) {
        platterPhase = std::fmod(platterPhase, kTwoPi);
      }
    }

    result.outL = outL;
    result.outR = outR;
    result.lag = currentLagFromNewest(newestPos);
    result.accessibleLag = limit;
    updateScratchControlOutputs(anyScratch, result.lag, externalScratch);
    result.platterAngle = platterPhase;
    result.sampleMode = sampleModeActive;
    result.sampleLoaded = sampleLoaded;
    result.sampleTransportPlaying = sampleTransportPlaying;
    result.autoFreezeRequested = autoFreezeRequested;
    double sampleUiEndFrame = sampleModeActive ? sampleWindowEndPos : 0.0;
    double sampleUiFrame = sampleModeActive ? clampd(readHead, 0.0, sampleUiEndFrame) : 0.0;
    result.samplePlayhead = sampleModeActive ? std::max(0.0, sampleUiFrame / std::max(sampleRate, 1.f)) : 0.0;
    result.sampleDuration = sampleModeActive ? std::max(0.0, (sampleUiEndFrame + 1.0) / std::max(sampleRate, 1.f)) : 0.0;
    result.sampleProgress = sampleModeActive && sampleUiEndFrame > 0.0 ? clampd(sampleUiFrame / sampleUiEndFrame, 0.0, 1.0) : 0.0;
    return result;
  }
};

} // namespace temporaldeck
