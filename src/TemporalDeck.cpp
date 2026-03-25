#include "TemporalDeck.hpp"
#include "codec.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

namespace {

static float realBufferSecondsForMode(int index) {
  switch (index) {
  case 0:
    return 11.f;
  case 1:
    return 21.f;
  case 2:
    return 601.f;
  case 3:
    return 601.f;
  default:
    return 11.f;
  }
}

static float usableBufferSecondsForMode(int index) { return std::max(1.f, realBufferSecondsForMode(index) - 1.f); }

static bool isMonoBufferMode(int index) { return index == TemporalDeck::BUFFER_DURATION_10M_MONO; }

using temporaldeck::DecodedSampleFile;
using temporaldeck::decodeSampleFile;

static int chooseSampleBufferMode(const DecodedSampleFile &sample) {
  if (sample.channels <= 1) {
    return TemporalDeck::BUFFER_DURATION_10M_MONO;
  }
  return TemporalDeck::BUFFER_DURATION_10M_STEREO;
}

static int maxFramesForModeAtSampleRate(int mode, float sampleRate) {
  return std::max(1, int(std::floor(usableBufferSecondsForMode(mode) * std::max(sampleRate, 1.f))));
}

static int resampledFrameCount(int sourceFrames, float sourceRate, float targetRate) {
  if (sourceFrames <= 0 || sourceRate <= 1.f || targetRate <= 1.f) {
    return 0;
  }
  double seconds = double(sourceFrames) / double(sourceRate);
  return std::max(1, int(std::round(seconds * double(targetRate))));
}

static double clampd(double x, double a, double b) {
  return std::max(a, std::min(x, b));
}

template <size_t N>
static void smoothLedBrightness(std::array<float, N> &values, float sideWeight = 0.22f) {
  sideWeight = clamp(sideWeight, 0.f, 0.49f);
  float centerWeight = 1.f - 2.f * sideWeight;
  std::array<float, N> src = values;
  for (size_t i = 0; i < N; ++i) {
    float prev = src[i > 0 ? i - 1 : i];
    float next = src[i + 1 < N ? i + 1 : i];
    values[i] = clamp(prev * sideWeight + src[i] * centerWeight + next * sideWeight, 0.f, 1.f);
  }
}

static void resampleSampleChannel(const std::vector<float> &src, float sourceRate, float targetRate, int outFrames,
                                  std::vector<float> *dst) {
  dst->assign(std::max(outFrames, 0), 0.f);
  if (src.empty() || outFrames <= 0) {
    return;
  }
  if (src.size() == 1 || std::fabs(sourceRate - targetRate) < 1e-3f) {
    for (int i = 0; i < outFrames; ++i) {
      int srcIndex = std::min<int>(i, src.size() - 1);
      (*dst)[i] = src[srcIndex];
    }
    return;
  }

  double ratio = double(sourceRate) / double(targetRate);
  for (int i = 0; i < outFrames; ++i) {
    double srcPos = double(i) * ratio;
    int i0 = clamp(int(std::floor(srcPos)), 0, int(src.size()) - 1);
    int i1 = clamp(i0 + 1, 0, int(src.size()) - 1);
    float t = float(srcPos - double(i0));
    (*dst)[i] = crossfade(src[i0], src[i1], t);
  }
}

static int nextCartridgeCharacter(int current) {
  switch (current) {
  case TemporalDeck::CARTRIDGE_CLEAN:
    return TemporalDeck::CARTRIDGE_M44_7;
  case TemporalDeck::CARTRIDGE_M44_7:
    return TemporalDeck::CARTRIDGE_ORTOFON_SCRATCH;
  case TemporalDeck::CARTRIDGE_ORTOFON_SCRATCH:
    return TemporalDeck::CARTRIDGE_STANTON_680HP;
  case TemporalDeck::CARTRIDGE_STANTON_680HP:
    return TemporalDeck::CARTRIDGE_QBERT;
  case TemporalDeck::CARTRIDGE_QBERT:
    return TemporalDeck::CARTRIDGE_LOFI;
  case TemporalDeck::CARTRIDGE_LOFI:
  default:
    return TemporalDeck::CARTRIDGE_CLEAN;
  }
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
    constexpr float kPi = float(M_PI);
    if (ax < 1e-5f) {
      return 1.f;
    }
    // Blackman-windowed sinc for smooth, low-ripple scratch resampling.
    float sinc = std::sin(kPi * x) / (kPi * x);
    float windowPhase = (ax / radius);
    float blackman = 0.42f + 0.5f * std::cos(kPi * windowPhase) + 0.08f * std::cos(2.f * kPi * windowPhase);
    return sinc * blackman;
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

    constexpr int kRadius = 8;
    float accL = 0.f;
    float accR = 0.f;
    float weightSum = 0.f;
    for (int k = -kRadius + 1; k <= kRadius; ++k) {
      int idx = wrapIndex(center + k);
      float dist = float(k) - frac;
      float w = windowedSinc(dist, float(kRadius));
      accL += left[idx] * w;
      accR += rightSample(idx) * w;
      weightSum += w;
    }
    if (std::fabs(weightSum) > 1e-6f) {
      float inv = 1.f / weightSum;
      accL *= inv;
      accR *= inv;
    }
    return {accL, accR};
  }
};

struct TemporalDeckEngine {
  static constexpr float kScratchGateThreshold = 1.f;
  static constexpr float kFreezeGateThreshold = 1.f;
  static constexpr float kSlipReturnTime = 0.12f;
  static constexpr float kSlipReturnQuickTime = 0.06f;
  static constexpr float kSlipReturnSlowMultiplier = 1.85f;
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
  static constexpr float kManualVelocityPredictScale = 0.95f;
  static constexpr float kHybridScratchHandFollowHz = 220.f;
  static constexpr float kHybridScratchVelocityDampingHz = 9.f;
  static constexpr float kHybridScratchCorrectionHz = 70.f;
  static constexpr float kHybridScratchWheelBurstDecayHz = 24.f;
  static constexpr float kHybridScratchWheelImpulseTime = 0.03f;
  static constexpr float kHybridScratchMaxVelocity = 96000.f;
  static constexpr float kHybridScratchMaxAccel = 1200000.f;
  static constexpr float kHybridScratchVelocityDeadband = 8.f;
  static constexpr float kScratch3LagAlpha = 0.32f;
  static constexpr float kScratch3LagBeta = 0.018f;
  static constexpr float kScratch3VelocityFollowHz = 28.f;
  static constexpr float kScratch3VelocityDecayHz = 30.f;
  static constexpr float kScratch3VelocityDeadband = 10.f;
  static constexpr float kScratch3MaxLagVelocity = 96000.f;
  static constexpr float kExternalCvMaxTurnsPerSec = 2.0f;
  static constexpr float kExternalCvMaxTurnAccelPerSec2 = 18.0f;
  static constexpr float kExternalCvCorrectionHz = 22.f;
  static constexpr float kExternalCvVelocityDampingHz = 8.f;
  static constexpr float kScratchInertiaFollowHz = 950.f;
  static constexpr float kScratchInertiaDampingHz = 380.f;
  static constexpr float kInertiaBlend = 0.25f;
  static constexpr float kNominalPlatterRpm = 33.333333f;

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
  static_assert(CARTRIDGE_CLEAN == TemporalDeck::CARTRIDGE_CLEAN, "Cartridge enum mismatch: CLEAN");
  static_assert(CARTRIDGE_M44_7 == TemporalDeck::CARTRIDGE_M44_7, "Cartridge enum mismatch: M44-7");
  static_assert(CARTRIDGE_CONCORDE_SCRATCH == TemporalDeck::CARTRIDGE_ORTOFON_SCRATCH,
                "Cartridge enum mismatch: Ortofon Scratch");
  static_assert(CARTRIDGE_680_HP == TemporalDeck::CARTRIDGE_STANTON_680HP,
                "Cartridge enum mismatch: Stanton 680HP");
  static_assert(CARTRIDGE_QBERT == TemporalDeck::CARTRIDGE_QBERT, "Cartridge enum mismatch: Q.Bert");
  static_assert(CARTRIDGE_LOFI == TemporalDeck::CARTRIDGE_LOFI, "Cartridge enum mismatch: Lo-Fi");
  static_assert(CARTRIDGE_COUNT == TemporalDeck::CARTRIDGE_COUNT, "Cartridge enum mismatch: count");
  static_assert(BUFFER_DURATION_10S == TemporalDeck::BUFFER_DURATION_10S, "Buffer duration enum mismatch: 10s");
  static_assert(BUFFER_DURATION_20S == TemporalDeck::BUFFER_DURATION_20S, "Buffer duration enum mismatch: 20s");
  static_assert(BUFFER_DURATION_10MIN_STEREO == TemporalDeck::BUFFER_DURATION_10M_STEREO,
                "Buffer duration enum mismatch: 10m stereo");
  static_assert(BUFFER_DURATION_10MIN_MONO == TemporalDeck::BUFFER_DURATION_10M_MONO,
                "Buffer duration enum mismatch: 10m mono");
  static_assert(BUFFER_DURATION_COUNT == TemporalDeck::BUFFER_DURATION_COUNT,
                "Buffer duration enum mismatch: count");

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
  bool sampleLoaded = false;
  bool sampleTransportPlaying = false;
  bool sampleTruncated = false;
  int sampleFrames = 0;
  double samplePlayhead = 0.0;
  double readHead = 0.0;
  double timelineHead = 0.0;
  float platterPhase = 0.f;
  float platterVelocity = 0.f;
  bool freezeState = false;
  bool reverseState = false;
  bool slipState = false;
  int scratchInterpolationMode = TemporalDeck::SCRATCH_INTERP_LAGRANGE6;
  int slipReturnMode = TemporalDeck::SLIP_RETURN_NORMAL;
  bool scratchActive = false;
  bool slipReturning = false;
  bool slipFinalCatchActive = false;
  bool nowCatchActive = false;
  float slipReturnRemaining = 0.f;
  float slipReturnStartLag = 0.f;
  float slipReturnOverrideTime = -1.f;
  float nowCatchRemaining = 0.f;
  float nowCatchStartLag = 0.f;
  double scratchLagSamples = 0.0;
  double scratchLagTargetSamples = 0.0;
  float scratchHandVelocity = 0.f;
  float scratchMotionVelocity = 0.f;
  float scratch3LagVelocity = 0.f;
  float scratch3GestureAgeSec = 0.f;
  float scratchWheelVelocityBurst = 0.f;
  double filteredManualLagTargetSamples = 0.0;
  double lastPlatterLagTarget = 0.0;
  uint32_t lastPlatterGestureRevision = 0;
  int cartridgeCharacter = CARTRIDGE_CLEAN;
  int bufferDurationMode = BUFFER_DURATION_10S;
  int lastSlipReturnMode = TemporalDeck::SLIP_RETURN_NORMAL;
  CartridgeChannelState cartridgeLeft;
  CartridgeChannelState cartridgeRight;
  float lofiWowPhaseA = 0.f;
  float lofiWowPhaseB = 0.f;
  float lofiFlutterPhase = 0.f;
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
  bool externalCvGateHigh = false;
  double externalCvAnchorLagSamples = 0.0;
  float prevBaseSpeed = 1.f;

  void reset(float sr) {
    sampleRate = sr;
    buffer.reset(sr, realBufferSecondsForMode(bufferDurationMode), isMonoBufferMode(bufferDurationMode));
    sampleLoaded = false;
    sampleTransportPlaying = false;
    sampleTruncated = false;
    sampleFrames = 0;
    samplePlayhead = 0.f;
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
    slipReturnOverrideTime = -1.f;
    nowCatchRemaining = 0.f;
    nowCatchStartLag = 0.f;
    scratchLagSamples = 0.f;
    scratchLagTargetSamples = 0.f;
    scratchHandVelocity = 0.f;
    scratchMotionVelocity = 0.f;
    scratch3LagVelocity = 0.f;
    scratch3GestureAgeSec = 0.f;
    scratchWheelVelocityBurst = 0.f;
    filteredManualLagTargetSamples = 0.f;
    lastPlatterLagTarget = 0.f;
    lastPlatterGestureRevision = 0;
    cartridgeLeft.reset();
    cartridgeRight.reset();
    lofiWowPhaseA = 0.f;
    lofiWowPhaseB = 0.f;
    lofiFlutterPhase = 0.f;
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
    prevBaseSpeed = 1.f;
  }

  double maxLagFromKnob(float knob) const {
    return double(clamp(knob, 0.f, 1.f)) * double(sampleRate) * double(usableBufferSecondsForMode(bufferDurationMode));
  }

  double accessibleLag(float knob) const { return std::min(maxLagFromKnob(knob), double(buffer.filled)); }

  double clampLag(double lag, double limit) const { return std::max(0.0, std::min(lag, std::max(0.0, limit))); }

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
    float t = clamp((rateCv + 10.f) / 20.f, 0.f, 1.f);
    return 0.5f + 1.5f * t;
  }

  float computeBaseSpeed(float rateKnob, float rateCv, bool rateCvConnected, bool reverse) const {
    float speed = rateCvConnected ? baseSpeedFromCv(rateCv) : baseSpeedFromKnob(rateKnob);
    speed = clamp(speed, -3.f, 3.f);
    if (reverse) {
      speed *= -1.f;
    }
    return speed;
  }

  float configuredSlipReturnTime() const {
    if (slipReturnOverrideTime >= 0.f) {
      return slipReturnOverrideTime;
    }
    if (slipReturnMode == TemporalDeck::SLIP_RETURN_INSTANT) {
      return 0.f;
    }
    if (slipReturnMode == TemporalDeck::SLIP_RETURN_SLOW) {
      return kSlipReturnTime * kSlipReturnSlowMultiplier;
    }
    return kSlipReturnTime;
  }

  double lagForPositionCv(float cv, double limit) const {
    double normalized = double(clamp(std::fabs(cv) / 10.f, 0.f, 1.f));
    return normalized * limit;
  }

  double lagOffsetForPositionCv(float cv) const {
    float clampedCv = clamp(cv, -10.f, 10.f);
    return double(clampedCv) * double(sampleRate);
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
      return 1.12f;
    case CARTRIDGE_CONCORDE_SCRATCH:
      return 1.08f;
    case CARTRIDGE_680_HP:
      return 1.07f;
    case CARTRIDGE_QBERT:
      return 1.14f;
    case CARTRIDGE_LOFI:
      return 1.36f;
    case CARTRIDGE_CLEAN:
    default:
      return 1.f;
    }
  }

  static float playbackColorMixForCartridge(int mode) {
    switch (mode) {
    case CARTRIDGE_M44_7:
      return 0.74f;
    case CARTRIDGE_CONCORDE_SCRATCH:
      return 0.72f;
    case CARTRIDGE_680_HP:
      return 0.70f;
    case CARTRIDGE_QBERT:
      return 0.76f;
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
      float sr = std::max(sampleRate, 1.f);
      float dt = 1.f / sr;
      constexpr float kTau = 2.f * float(M_PI);

      lofiWowPhaseA += kTau * 0.33f * dt;
      lofiWowPhaseB += kTau * 0.57f * dt;
      lofiFlutterPhase += kTau * 7.6f * dt;
      if (lofiWowPhaseA > kTau)
        lofiWowPhaseA -= kTau;
      if (lofiWowPhaseB > kTau)
        lofiWowPhaseB -= kTau;
      if (lofiFlutterPhase > kTau)
        lofiFlutterPhase -= kTau;

      // Sum of slow wow + quicker flutter components (deterministic, low cost).
      float wowFlutter = 0.0064f * std::sin(lofiWowPhaseA) + 0.0041f * std::sin(lofiWowPhaseB + 0.7f) +
                         0.0022f * std::sin(lofiFlutterPhase + 1.4f);
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
    return (2.f * float(M_PI) * (kNominalPlatterRpm / 60.f)) / std::max(sampleRate, 1.f);
  }

  float samplesPerPlatterRadian() const { return 1.f / std::max(platterRadiansPerSample(), 1e-9f); }

  double currentLagFromNewest(double newestPos) const {
    if (buffer.size <= 0) {
      return 0.0;
    }
    if (sampleModeEnabled && sampleLoaded) {
      return clampd(newestPos - readHead, 0.0, std::max(0.0, newestPos));
    }
    double lag = newestPos - readHead;
    if (lag < 0.0) {
      lag += double(buffer.size);
    }
    return lag;
  }

  double unwrapReadNearWrite(double readPos, double writePos) const {
    if (sampleModeEnabled && sampleLoaded) {
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

  std::pair<float, float> readSampleBounded(double pos, int interpolationMode) const {
    int maxIndex = std::max(0, sampleFrames - 1);
    if (!sampleLoaded || sampleFrames <= 0 || maxIndex < 0) {
      return {0.f, 0.f};
    }
    pos = clampd(pos, 0.0, double(maxIndex));
    int i1 = int(std::floor(pos));
    float t = float(pos - double(i1));
    const float *leftData = buffer.left.data();
    const float *rightData = buffer.monoStorage ? buffer.left.data() : buffer.right.data();
    auto leftAt = [&](int idx) { return leftData[clampSampleIndex(idx, maxIndex)]; };
    auto rightAt = [&](int idx) { return rightData[clampSampleIndex(idx, maxIndex)]; };

    // Exact/near-exact sample-center reads are common in transport playback.
    // Skip interpolation math when the phase is effectively integral.
    if (std::fabs(t) <= 1e-6f || std::fabs(1.f - t) <= 1e-6f) {
      int idx = clampSampleIndex(int(std::round(pos)), maxIndex);
      return {leftData[idx], rightData[idx]};
    }

    if (interpolationMode == TemporalDeck::SCRATCH_INTERP_SINC) {
      constexpr int kRadius = 8;
      float accL = 0.f;
      float accR = 0.f;
      float weightSum = 0.f;
      bool interior = (i1 - (kRadius - 1) >= 0) && (i1 + kRadius <= maxIndex);
      if (interior) {
        for (int k = -kRadius + 1; k <= kRadius; ++k) {
          int idx = i1 + k;
          float dist = float(k) - t;
          float w = TemporalDeckBuffer::windowedSinc(dist, float(kRadius));
          accL += leftData[idx] * w;
          accR += rightData[idx] * w;
          weightSum += w;
        }
      } else {
        for (int k = -kRadius + 1; k <= kRadius; ++k) {
          int idx = clampSampleIndex(i1 + k, maxIndex);
          float dist = float(k) - t;
          float w = TemporalDeckBuffer::windowedSinc(dist, float(kRadius));
          accL += leftData[idx] * w;
          accR += rightData[idx] * w;
          weightSum += w;
        }
      }
      if (std::fabs(weightSum) > 1e-6f) {
        float inv = 1.f / weightSum;
        accL *= inv;
        accR *= inv;
      }
      return {accL, accR};
    }

    if (interpolationMode == TemporalDeck::SCRATCH_INTERP_LAGRANGE6) {
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

  void installSample(const std::vector<float> &left, const std::vector<float> &right, int frames, bool autoplay,
                     bool truncated) {
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
                              float dampingScale, float correctionScale, float nowSnapThresholdSamples) {
    // Hybrid scratch is velocity-first internally. We still keep lag as the
    // module-facing state, but we integrate the read head in buffer space and
    // derive lag from that position so "zero hand velocity" naturally means a
    // stationary read head.
    scratchLagTargetSamples = clampLag(scratchLagTargetSamples, limit);
    scratchLagSamples = clampLag(scratchLagSamples, limit);

    float lagError = scratchLagTargetSamples - scratchLagSamples;
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
    candidate = std::max(newestPos - std::max(limit, 0.0), std::min(candidate, newestPos));
    readHead = buffer.wrapPosition(candidate);
    scratchLagSamples = clampLag(currentLagFromNewest(newestPos), limit);

    if (scratchLagTargetSamples <= nowSnapThresholdSamples && scratchLagSamples <= nowSnapThresholdSamples &&
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
    // External CV scratch follows a physically limited lag trajectory instead of
    // teleporting the read head. Limits are set to roughly upper-end
    // turntablist hand speed.
    scratchHandVelocity = 0.f;
    scratchMotionVelocity = 0.f;
    scratchWheelVelocityBurst = 0.f;
    platterVelocity = 0.f;

    scratchLagSamples = clampLag(currentLagFromNewest(newestPos), limit);
    scratchLagTargetSamples = clampLag(targetLag, limit);

    float samplesPerRev = platter_interaction::samplesPerRevolution(sampleRate, kNominalPlatterRpm);
    float maxLagVelocity = samplesPerRev * kExternalCvMaxTurnsPerSec;
    float maxLagAccel = samplesPerRev * kExternalCvMaxTurnAccelPerSec2;

    float lagError = float(scratchLagTargetSamples - scratchLagSamples);
    float desiredLagVelocity = clamp(lagError * kExternalCvCorrectionHz, -maxLagVelocity, maxLagVelocity);
    float maxVelStep = maxLagAccel * dt;
    scratch3LagVelocity += clamp(desiredLagVelocity - scratch3LagVelocity, -maxVelStep, maxVelStep);

    float damping = clamp(1.f - dt * kExternalCvVelocityDampingHz, 0.f, 1.f);
    scratch3LagVelocity *= damping;

    double lagEstimate = clampLag(scratchLagSamples + double(scratch3LagVelocity) * double(dt), limit);
    if ((lagError > 0.f && lagEstimate > scratchLagTargetSamples) || (lagError < 0.f && lagEstimate < scratchLagTargetSamples)) {
      lagEstimate = scratchLagTargetSamples;
      scratch3LagVelocity = 0.f;
    }
    if ((lagEstimate <= 0.0 && scratch3LagVelocity < 0.f) ||
        (lagEstimate >= limit && scratch3LagVelocity > 0.f)) {
      scratch3LagVelocity = 0.f;
    }
    if (scratchLagTargetSamples <= nowSnapThresholdSamples && lagEstimate <= nowSnapThresholdSamples &&
        scratch3LagVelocity <= 0.f) {
      lagEstimate = 0.0;
      scratch3LagVelocity = 0.f;
    }

    scratchLagSamples = lagEstimate;
    readHead = buffer.wrapPosition(newestPos - lagEstimate);
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

  FrameResult process(float dt, float inL, float inR, float bufferKnob, float rateKnob, float mixKnob,
                      float feedbackKnob, bool freezeButton, bool reverseButton, bool slipButton, bool quickSlipTrigger,
                      bool freezeGate, bool scratchGate, bool scratchGateConnected, bool positionConnected,
                      float positionCv, float rateCv, bool rateCvConnected, bool platterTouched, bool wheelScratchHeld,
                      bool platterMotionActive,
                      uint32_t platterGestureRevision, float platterLagTarget, float platterGestureVelocity,
                      float wheelDelta) {
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
    bool freezeForScratchModel = freezeState || sampleManualFreezeBehavior;
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
    bool fastSampleTransportPath =
      sampleModeActive && !anyScratch && !wasScratchActive && !slipState && !prevSlipState &&
      !slipReturning && !slipFinalCatchActive && !nowCatchActive && !quickSlipTrigger && !externalCvGateHigh;
    if (fastSampleTransportPath) {
      if (!sampleTransportPlaying || freezeState) {
        speed = 0.f;
      }
      samplePlayhead = clampd(samplePlayhead + double(speed), 0.0, sampleWindowEndPos);
      if (samplePlayhead >= sampleWindowEndPos && speed > 0.f) {
        samplePlayhead = sampleWindowEndPos;
        autoFreezeRequested = true;
      }
      if (samplePlayhead <= 0.0 && speed < 0.f) {
        samplePlayhead = 0.0;
        autoFreezeRequested = true;
      }
      readHead = samplePlayhead;
      newestPos = sampleWindowEndPos;

      std::pair<float, float> wet = readSampleBounded(readHead, TemporalDeck::SCRATCH_INTERP_CUBIC);
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
      if (platterPhase > float(M_PI) || platterPhase < -float(M_PI)) {
        platterPhase = std::fmod(platterPhase, 2.f * float(M_PI));
      }

      scratchActive = false;
      externalCvGateHigh = false;
      result.lag = currentLagFromNewest(newestPos);
      result.accessibleLag = sampleWindowEndPos;
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
      clearScratchMotionState();
    }

    if (scratchGateHigh && !externalCvGateHigh) {
      externalCvAnchorLagSamples = currentLagFromNewest(newestPos);
    }
    externalCvGateHigh = scratchGateHigh;

    scratchActive = anyScratch;

    bool quickSlipActive = slipReturnOverrideTime >= 0.f;
    if (!slipState && !quickSlipActive) {
      slipReturning = false;
      slipFinalCatchActive = false;
    }

    if (releasedFromScratch && slipState && !sampleModeActive) {
      slipReturning = true;
      slipFinalCatchActive = false;
      slipReturnRemaining = 0.f;
    }

    if (releasedFromScratch && !sampleModeActive && currentLagFromNewest(newestPos) <= nowSnapThresholdSamples) {
      startNowCatch(currentLagFromNewest(newestPos));
      slipReturning = false;
      slipFinalCatchActive = false;
    }

    if (slipJustEnabled && !anyScratch && !sampleModeActive) {
      if (currentLagFromNewest(newestPos) > kSlipEnableReturnThreshold) {
        slipReturning = true;
        slipFinalCatchActive = false;
      }
    }

    // If slip speed mode changes while Slip is active and we are behind NOW,
    // immediately engage a return so the new mode takes audible effect.
    if (slipModeChanged && slipState && !anyScratch && !sampleModeActive &&
        currentLagFromNewest(newestPos) > nowSnapThresholdSamples) {
      slipReturning = true;
      slipFinalCatchActive = false;
    }

    if (quickSlipTrigger && !anyScratch && !sampleModeActive && currentLagFromNewest(newestPos) > nowSnapThresholdSamples) {
      slipReturning = true;
      slipFinalCatchActive = false;
      slipReturnOverrideTime = kSlipReturnQuickTime;
    }

    if (anyScratch) {
      slipReturning = false;
      slipFinalCatchActive = false;
      slipReturnOverrideTime = -1.f;
    }

    if (sampleModeActive) {
      if (releasedFromScratch) {
        // In sample mode, release should continue from where the scratch
        // gesture landed rather than snapping back to the prior transport
        // anchor.
        samplePlayhead = clampd(readHead, 0.0, sampleWindowEndPos);
        scratchLagSamples = 0.0;
        scratchLagTargetSamples = 0.0;
      }
      if (!sampleTransportPlaying || freezeForScratchModel) {
        speed = 0.f;
      }
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

    // 3. Determine actual playhead (readHead)
    if (freezeForScratchModel) {
      speed = 0.f;
    }

    newestPos = sampleModeActive ? sampleWindowEndPos : newestReadablePos();
    float lagNow = currentLagFromNewest(newestPos);
    bool reverseAtOldestEdge =
      !scratchActive && !slipReturning && reverseState && limit > 0.f && lagNow >= (limit - 0.5f);
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
          scratchLagTargetSamples = clampLag(platterLagTarget, limit);
          lastPlatterGestureRevision = platterGestureRevision;
        }

        bool stationaryManualHold = !platterMotionActive && !hasFreshPlatterGesture;
        if (stationaryManualHold) {
          scratchLagTargetSamples = scratchLagSamples;
          scratchHandVelocity = 0.f;
          scratchMotionVelocity = 0.f;
        } else {
          float targetReadVelocity = 0.f;
          if (platter_interaction::hasActiveManualMotion(hasFreshPlatterGesture, platterMotionActive)) {
            // While motion is fresh, gesture velocity is relative to the write
            // head. Convert it into absolute read velocity by adding the write
            // baseline (except in freeze, where write head is stationary).
            targetReadVelocity = platterGestureVelocity;
            if (platter_interaction::shouldApplyWriteHeadCompensation(freezeForScratchModel, hasFreshPlatterGesture,
                                                                      platterMotionActive)) {
              targetReadVelocity += sampleRate;
            }
          }
          float motionNorm = clamp(std::fabs(targetReadVelocity) / std::max(sampleRate * 0.45f, 1.f), 0.f, 1.f);
          integrateHybridScratch(dt, limit, newestPos, targetReadVelocity, 1.55f + 0.55f * motionNorm,
                                 0.72f - 0.12f * motionNorm, 0.68f, nowSnapThresholdSamples);
        }
      } else {
        // Wheel scratch uses the same Hybrid motion model as drag scratch.
        float wheelDeltaSoftRange = sampleRate * 0.16f * kWheelScratchTravelScale;
        float wheelDeltaShaped = wheelDeltaSoftRange * std::tanh(wheelDelta / std::max(wheelDeltaSoftRange, 1e-6f));
        if (!freezeForScratchModel && wheelDeltaShaped < 0.f) {
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
        // Wheel mode nudges lag around a moving write head in live playback.
        // Use write-head baseline velocity so small forward wheel gestures don't
        // get overwhelmed by natural lag growth when not frozen.
        float wheelTargetReadVelocity = freezeForScratchModel ? 0.f : sampleRate;
        integrateHybridScratch(dt, limit, newestPos, wheelTargetReadVelocity, 1.25f, 0.95f, 1.20f,
                               nowSnapThresholdSamples);
      }
      lastPlatterLagTarget = platterLagTarget;
    } else if (externalScratch) {
      // POSITION CV is interpreted as signed time offset around a lag anchor
      // latched on gate rise. While gate is held, the anchor naturally drifts
      // deeper into the buffer with write-head progression.
      double targetLag = externalCvAnchorLagSamples + lagOffsetForPositionCv(positionCv);
      targetLag = clampLag(targetLag, limit);
      integrateExternalCvScratch(dt, limit, newestPos, targetLag, nowSnapThresholdSamples);
    } else if (slipReturning && !sampleModeActive) {
      float slipReturnTime = configuredSlipReturnTime();
      if (slipReturnTime <= 0.f) {
        readHead = newestPos;
        keepSlipLagAligned = true;
        slipReturning = false;
        slipFinalCatchActive = false;
      } else {
        // Return to NOW (lag = 0)
        double currentLagSamples = currentLagFromNewest(newestPos);
        float finalCatchThresholdSamples = sampleRate * (kSlipFinalCatchThresholdMs / 1000.f);

        if (currentLagSamples <= nowSnapThresholdSamples) {
          startNowCatch(currentLagSamples);
          slipReturning = false;
          slipFinalCatchActive = false;
        } else if (!slipFinalCatchActive) {
          // Exponential-like approach to zero lag.
          // We target a specific lag value that decreases over time.
          float alpha = dt / std::max(slipReturnTime, 1e-6f);
          double targetLag = currentLagSamples * double(1.f - alpha);

          // Ensure we actually move towards zero even if alpha is tiny.
          if (targetLag > currentLagSamples - 0.5f) {
            targetLag = currentLagSamples - 0.5f;
          }
          if (targetLag < 0.f) {
            targetLag = 0.f;
          }

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
          float progress = 1.f - clamp(slipReturnRemaining / std::max(kSlipFinalCatchTime, 1e-6f), 0.f, 1.f);
          float shapedProgress = 1.f - std::pow(1.f - progress, 2.5f);
          double targetLag = slipReturnStartLag * double(1.f - shapedProgress);

          readHead = buffer.wrapPosition(newestPos - targetLag);
          keepSlipLagAligned = true;

          if (slipReturnRemaining <= 0.f || targetLag < 0.5f) {
            readHead = newestPos;
            slipReturning = false;
            slipFinalCatchActive = false;
          }
        }
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

    if (anyScratch) {
      slipReturning = false;
      slipFinalCatchActive = false;
      slipReturnRemaining = 0.f;
      slipReturnStartLag = 0.f;
      slipReturnOverrideTime = -1.f;
    } else if (!slipReturning) {
      slipReturnOverrideTime = -1.f;
    }

    if (nowCatchActive && !sampleModeActive) {
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

    bool holdAtScratchEdge = manualScratch && limit > 0.f && scratchLagSamples >= (limit - 0.5f);
    bool holdAtReverseEdge = reverseAtOldestEdge;
    bool holdAtBufferEdge = holdAtScratchEdge || holdAtReverseEdge;

    bool scratchReadPath = anyScratch;
    int effectiveScratchInterpolation = clamp(scratchInterpolationMode, TemporalDeck::SCRATCH_INTERP_CUBIC,
                                              TemporalDeck::SCRATCH_INTERP_COUNT - 1);
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
      // Match live-mode behavior: normal transport playback uses cubic.
      int sampleInterp = scratchReadPath ? effectiveScratchInterpolation : TemporalDeck::SCRATCH_INTERP_CUBIC;
      wet = readSampleBounded(readHead, sampleInterp);
    } else if (scratchReadPath) {
      if (effectiveScratchInterpolation == TemporalDeck::SCRATCH_INTERP_LAGRANGE6) {
        wet = buffer.readHighQuality(readHead);
      } else if (effectiveScratchInterpolation == TemporalDeck::SCRATCH_INTERP_SINC) {
        wet = buffer.readSinc(readHead);
      } else {
        wet = buffer.readCubic(readHead);
      }
    } else {
      wet = buffer.readCubic(readHead);
    }
    wet = applyCartridgeCharacter(wet, motionAmount, scratchReadPath);
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

    bool writeAdvanced = false;
    if (!sampleModeActive && !freezeState && !holdAtBufferEdge) {
      if (noFeedback) {
        buffer.write(inL, inR);
      } else {
        buffer.write(inL + outL * feedback, inR + outR * feedback);
      }
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
      platterPhase += float(visualDelta) * platterRadiansPerSample();
      if (platterPhase > float(M_PI) || platterPhase < -float(M_PI)) {
        platterPhase = std::fmod(platterPhase, 2.f * float(M_PI));
      }
    }

    result.outL = outL;
    result.outR = outR;
    result.lag = currentLagFromNewest(newestPos);
    result.accessibleLag = limit;
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

struct DeckRateQuantity : ParamQuantity {
  static float valueForSpeed(float speed) {
    speed = clamp(speed, 0.5f, 2.f);
    if (speed <= 1.f) {
      return speed - 0.5f;
    }
    return speed * 0.5f;
  }

  float getDisplayValue() override { return TemporalDeckEngine::baseSpeedFromKnob(getValue()); }

  void setDisplayValue(float displayValue) override { setImmediateValue(valueForSpeed(displayValue)); }

  std::string getDisplayValueString() override {
    return string::f("%.2fx", TemporalDeckEngine::baseSpeedFromKnob(getValue()));
  }
};

struct ScratchSensitivityQuantity : ParamQuantity {
  static float sensitivityForValue(float v) {
    if (v <= 0.5f) {
      return rescale(v, 0.f, 0.5f, 0.5f, 1.f);
    }
    return rescale(v, 0.5f, 1.f, 1.f, 2.f);
  }

  std::string getDisplayValueString() override { return string::f("%.2fx", sensitivityForValue(getValue())); }

  std::string getLabel() override { return "Scratch sensitivity"; }
};

} // namespace

struct TemporalDeck::Impl {
  TemporalDeckEngine engine;
  dsp::SchmittTrigger freezeTrigger;
  dsp::SchmittTrigger reverseTrigger;
  dsp::SchmittTrigger slipTrigger;
  dsp::SchmittTrigger cartridgeCycleTrigger;
  float cachedSampleRate = 0.f;
  bool freezeLatched = false;
  bool reverseLatched = false;
  bool slipLatched = false;
  std::atomic<bool> sampleModeEnabled{false};
  bool sampleAutoPlayOnLoad = true;
  std::mutex sampleStateMutex;
  std::atomic<bool> pendingSampleStateApply{false};
  std::string samplePath;
  std::string sampleDisplayName;
  DecodedSampleFile decodedSample;
  std::atomic<bool> platterTouched{false};
  std::atomic<uint32_t> platterGestureRevision{0};
  std::atomic<float> platterLagTarget{0.f};
  std::atomic<float> platterGestureVelocity{0.f};
  std::atomic<float> platterWheelDelta{0.f};
  std::atomic<int> platterScratchHoldSamples{0};
  std::atomic<int> platterMotionFreshSamples{0};
  std::atomic<bool> quickSlipTrigger{false};
  std::atomic<float> pendingSampleSeekNormalized{0.f};
  std::atomic<uint32_t> pendingSampleSeekRevision{0};
  uint32_t appliedSampleSeekRevision = 0;
  std::atomic<double> uiLagSamples{0.0};
  std::atomic<double> uiAccessibleLagSamples{0.0};
  std::atomic<float> uiSampleRate{44100.f};
  std::atomic<float> uiPlatterAngle{0.f};
  std::atomic<bool> uiFreezeLatched{false};
  std::atomic<bool> uiSampleModeEnabled{false};
  std::atomic<bool> uiSampleLoaded{false};
  std::atomic<bool> uiSampleTransportPlaying{false};
  std::atomic<double> uiSamplePlayheadSeconds{0.0};
  std::atomic<double> uiSampleDurationSeconds{0.0};
  std::atomic<double> uiSampleProgress{0.0};
  float uiPublishTimerSec = 0.f;
  int scratchInterpolationMode = TemporalDeck::SCRATCH_INTERP_LAGRANGE6;
  bool platterCursorLock = false;
  bool freezeTraceLoggingEnabled = false;
  int cartridgeCharacter = TemporalDeck::CARTRIDGE_CLEAN;
  std::atomic<int> bufferDurationMode{TemporalDeck::BUFFER_DURATION_10S};
  int slipReturnMode = TemporalDeck::SLIP_RETURN_NORMAL;
};


TemporalDeck::TemporalDeck() : impl(new Impl()) {
  config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
  configParam(BUFFER_PARAM, 0.f, 1.f, 1.f, "Buffer", " s", 0.f, 10.f);
  configParam<DeckRateQuantity>(RATE_PARAM, 0.f, 1.f, 0.5f, "Rate");
  configParam<ScratchSensitivityQuantity>(SCRATCH_SENSITIVITY_PARAM, 0.f, 1.f, 0.5f, "Scratch sensitivity");
  configParam(MIX_PARAM, 0.f, 1.f, 1.f, "Mix");
  configParam(FEEDBACK_PARAM, 0.f, 1.f, 0.f, "Feedback");
  configButton(FREEZE_PARAM, "Freeze");
  configButton(REVERSE_PARAM, "Reverse");
  configButton(SLIP_PARAM, "Slip");
  configButton(CARTRIDGE_CYCLE_PARAM, "Cycle cartridge");
  configInput(POSITION_CV_INPUT, "Position CV");
  configInput(RATE_CV_INPUT, "Rate CV");
  configInput(INPUT_L_INPUT, "Left audio");
  configInput(INPUT_R_INPUT, "Right audio");
  configInput(SCRATCH_GATE_INPUT, "Scratch gate");
  configInput(FREEZE_GATE_INPUT, "Freeze gate");
  configOutput(OUTPUT_L_OUTPUT, "Left audio");
  configOutput(OUTPUT_R_OUTPUT, "Right audio");
  if (paramQuantities[BUFFER_PARAM]) {
    int mode = clamp(impl->bufferDurationMode.load(), 0, BUFFER_DURATION_COUNT - 1);
    paramQuantities[BUFFER_PARAM]->displayMultiplier = usableBufferSecondsForMode(mode);
  }
  applySampleRateChange(APP->engine->getSampleRate());
}

TemporalDeck::~TemporalDeck() = default;

const char *TemporalDeck::cartridgeLabelFor(int index) {
  switch (index) {
  case CARTRIDGE_M44_7:
    return "M44-7";
  case CARTRIDGE_ORTOFON_SCRATCH:
    return "C.MKII S";
  case CARTRIDGE_STANTON_680HP:
    return "680 HP";
  case CARTRIDGE_QBERT:
    return "Q.Bert";
  case CARTRIDGE_LOFI:
    return "Lo-Fi";
  case CARTRIDGE_CLEAN:
  default:
    return "Clean";
  }
}

CartridgeVisualStyle TemporalDeck::cartridgeVisualStyleFor(int index) {
  switch (index) {
  case CARTRIDGE_M44_7:
    return {nvgRGBA(26, 26, 26, 238), nvgRGBA(110, 110, 118, 190), nvgRGBA(252, 252, 252, 235)};
  case CARTRIDGE_ORTOFON_SCRATCH:
    return {nvgRGBA(242, 242, 242, 240), nvgRGBA(26, 26, 26, 210), nvgRGBA(18, 18, 18, 228)};
  case CARTRIDGE_STANTON_680HP:
    return {nvgRGBA(180, 186, 194, 238), nvgRGBA(120, 126, 134, 195), nvgRGBA(24, 24, 28, 230)};
  case CARTRIDGE_QBERT:
    return {nvgRGBA(34, 35, 40, 240), nvgRGBA(240, 242, 246, 210), nvgRGBA(248, 200, 58, 235)};
  case CARTRIDGE_LOFI:
    return {nvgRGBA(56, 51, 44, 238), nvgRGBA(98, 84, 70, 190), nvgRGBA(186, 170, 138, 210)};
  case CARTRIDGE_CLEAN:
  default:
    return {nvgRGBA(90, 178, 187, 236), nvgRGBA(12, 41, 45, 190), nvgRGBA(18, 18, 18, 230)};
  }
}

const char *TemporalDeck::scratchInterpolationLabelFor(int index) {
  switch (index) {
  case SCRATCH_INTERP_LAGRANGE6:
    return "6-point Lagrange";
  case SCRATCH_INTERP_SINC:
    return "Sinc (CPU heavy)";
  case SCRATCH_INTERP_CUBIC:
  default:
    return "Cubic";
  }
}

const char *TemporalDeck::slipReturnLabelFor(int index) {
  switch (index) {
  case SLIP_RETURN_SLOW:
    return "Slow";
  case SLIP_RETURN_INSTANT:
    return "Instant";
  case SLIP_RETURN_NORMAL:
  default:
    return "Normal";
  }
}

const char *TemporalDeck::bufferDurationLabelFor(int index) {
  switch (index) {
  case BUFFER_DURATION_20S:
    return "20 s";
  case BUFFER_DURATION_10M_STEREO:
    return "10 min stereo";
  case BUFFER_DURATION_10M_MONO:
    return "10 min mono";
  case BUFFER_DURATION_10S:
  default:
    return "10 s";
  }
}

float TemporalDeck::scratchSensitivity() {
  return ScratchSensitivityQuantity::sensitivityForValue(params[SCRATCH_SENSITIVITY_PARAM].getValue());
}

void TemporalDeck::applyBufferDurationMode(int mode) {
  int clamped = clamp(mode, 0, BUFFER_DURATION_COUNT - 1);
  impl->bufferDurationMode.store(clamped);
  if (paramQuantities[BUFFER_PARAM]) {
    paramQuantities[BUFFER_PARAM]->displayMultiplier = usableBufferSecondsForMode(clamped);
  }
}

void TemporalDeck::applySampleRateChange(float sampleRate) {
  impl->cachedSampleRate = sampleRate;
  int mode = clamp(impl->bufferDurationMode.load(), 0, BUFFER_DURATION_COUNT - 1);
  DecodedSampleFile decodedSample;
  bool sampleModeEnabled = impl->sampleModeEnabled.load(std::memory_order_relaxed);
  bool sampleAutoPlayOnLoad = true;
  {
    std::lock_guard<std::mutex> lock(impl->sampleStateMutex);
    decodedSample = impl->decodedSample;
    sampleAutoPlayOnLoad = impl->sampleAutoPlayOnLoad;
  }
  impl->engine.bufferDurationMode = mode;
  impl->engine.reset(impl->cachedSampleRate);
  impl->engine.sampleModeEnabled = sampleModeEnabled;
  if (decodedSample.frames > 0 && !decodedSample.left.empty()) {
    int outFrames = resampledFrameCount(decodedSample.frames, decodedSample.sampleRate, impl->cachedSampleRate);
    int maxFrames = maxFramesForModeAtSampleRate(impl->engine.bufferDurationMode, impl->cachedSampleRate);
    bool truncated = false;
    if (outFrames > maxFrames) {
      outFrames = maxFrames;
      truncated = true;
    }
    if (outFrames > 0) {
      float sr = std::max(impl->cachedSampleRate, 1.f);
      float sampleSeconds = std::max(float(outFrames) / sr, 1.f / sr);
      // In sample mode, allocate only as much buffer as needed for the loaded
      // material instead of always reserving the full mode capacity.
      impl->engine.buffer.reset(impl->cachedSampleRate, sampleSeconds, isMonoBufferMode(mode));
    }
    std::vector<float> left;
    std::vector<float> right;
    resampleSampleChannel(decodedSample.left, decodedSample.sampleRate, impl->cachedSampleRate, outFrames, &left);
    if (decodedSample.channels > 1) {
      resampleSampleChannel(decodedSample.right, decodedSample.sampleRate, impl->cachedSampleRate, outFrames, &right);
    }
    impl->engine.installSample(left, right, outFrames, sampleAutoPlayOnLoad, truncated || decodedSample.truncated);
    impl->engine.sampleModeEnabled = sampleModeEnabled;
  }
  impl->uiSampleRate.store(impl->cachedSampleRate);
  impl->uiLagSamples.store(0.0);
  impl->uiAccessibleLagSamples.store(0.0);
  impl->uiPlatterAngle.store(0.f);
  impl->uiFreezeLatched.store(false);
  impl->uiSampleModeEnabled.store(impl->engine.sampleModeEnabled);
  impl->uiSampleLoaded.store(impl->engine.sampleLoaded);
  impl->uiSampleTransportPlaying.store(impl->engine.sampleTransportPlaying);
  impl->uiSamplePlayheadSeconds.store(0.0);
  impl->uiSampleDurationSeconds.store(impl->engine.sampleLoaded ? double(impl->engine.sampleFrames) / std::max(double(impl->cachedSampleRate), 1.0) : 0.0);
  impl->uiSampleProgress.store(0.0);
  if (paramQuantities[BUFFER_PARAM]) {
    float displaySeconds = usableBufferSecondsForMode(mode);
    if (impl->engine.sampleLoaded && impl->engine.sampleFrames > 0) {
      displaySeconds = float(impl->engine.sampleFrames) / std::max(impl->cachedSampleRate, 1.f);
    }
    paramQuantities[BUFFER_PARAM]->displayMultiplier = displaySeconds;
  }
  impl->uiPublishTimerSec = 0.f;
  impl->platterScratchHoldSamples.store(0);
  impl->platterMotionFreshSamples.store(0);
  for (int i = 0; i < kArcLightCount; ++i) {
    lights[ARC_LIGHT_START + i].setBrightness(0.f);
    lights[ARC_MAX_LIGHT_START + i].setBrightness(0.f);
  }
}

void TemporalDeck::onSampleRateChange() {
  // Reconfiguration is applied on the audio thread from process() to avoid
  // cross-thread buffer reallocations.
}

json_t *TemporalDeck::dataToJson() {
  json_t *root = json_object();
  bool sampleModeEnabled = impl->sampleModeEnabled.load(std::memory_order_relaxed);
  bool sampleAutoPlayOnLoad = true;
  std::string samplePath;
  {
    std::lock_guard<std::mutex> lock(impl->sampleStateMutex);
    sampleAutoPlayOnLoad = impl->sampleAutoPlayOnLoad;
    samplePath = impl->samplePath;
  }
  json_object_set_new(root, "freezeLatched", json_boolean(impl->freezeLatched));
  json_object_set_new(root, "reverseLatched", json_boolean(impl->reverseLatched));
  json_object_set_new(root, "slipLatched", json_boolean(impl->slipLatched));
  json_object_set_new(root, "scratchInterpolationMode", json_integer(impl->scratchInterpolationMode));
  json_object_set_new(root, "platterCursorLock", json_boolean(impl->platterCursorLock));
  json_object_set_new(root, "freezeTraceLoggingEnabled", json_boolean(impl->freezeTraceLoggingEnabled));
  json_object_set_new(root, "slipReturnMode", json_integer(impl->slipReturnMode));
  json_object_set_new(root, "cartridgeCharacterV2", json_integer(impl->cartridgeCharacter));
  int legacyCartridgeCharacter = impl->cartridgeCharacter;
  if (legacyCartridgeCharacter == CARTRIDGE_QBERT) {
    legacyCartridgeCharacter = CARTRIDGE_ORTOFON_SCRATCH;
  } else if (legacyCartridgeCharacter == CARTRIDGE_LOFI) {
    legacyCartridgeCharacter = 4;
  }
  json_object_set_new(root, "cartridgeCharacter", json_integer(legacyCartridgeCharacter));
  json_object_set_new(root, "bufferDurationMode", json_integer(impl->bufferDurationMode.load()));
  json_object_set_new(root, "sampleModeEnabled", json_boolean(sampleModeEnabled));
  json_object_set_new(root, "sampleAutoPlayOnLoad", json_boolean(sampleAutoPlayOnLoad));
  if (!samplePath.empty()) {
    json_object_set_new(root, "samplePath", json_string(samplePath.c_str()));
  }
  return root;
}

void TemporalDeck::dataFromJson(json_t *root) {
  if (!root) {
    return;
  }
  json_t *freezeJ = json_object_get(root, "freezeLatched");
  json_t *reverseJ = json_object_get(root, "reverseLatched");
  json_t *slipJ = json_object_get(root, "slipLatched");
  json_t *scratchInterpModeJ = json_object_get(root, "scratchInterpolationMode");
  json_t *scratchInterpJ = json_object_get(root, "highQualityScratchInterpolation");
  json_t *platterCursorLockJ = json_object_get(root, "platterCursorLock");
  json_t *freezeTraceLoggingJ = json_object_get(root, "freezeTraceLoggingEnabled");
  json_t *slipReturnModeJ = json_object_get(root, "slipReturnMode");
  json_t *cartridgeV2J = json_object_get(root, "cartridgeCharacterV2");
  json_t *cartridgeJ = json_object_get(root, "cartridgeCharacter");
  json_t *bufferDurationJ = json_object_get(root, "bufferDurationMode");
  json_t *sampleModeEnabledJ = json_object_get(root, "sampleModeEnabled");
  json_t *sampleAutoPlayOnLoadJ = json_object_get(root, "sampleAutoPlayOnLoad");
  json_t *samplePathJ = json_object_get(root, "samplePath");
  if (freezeJ) {
    impl->freezeLatched = json_boolean_value(freezeJ);
  }
  if (reverseJ) {
    impl->reverseLatched = json_boolean_value(reverseJ);
  }
  if (slipJ) {
    impl->slipLatched = json_boolean_value(slipJ);
  }
  if (scratchInterpModeJ) {
    impl->scratchInterpolationMode =
      clamp((int)json_integer_value(scratchInterpModeJ), SCRATCH_INTERP_CUBIC, SCRATCH_INTERP_COUNT - 1);
  } else if (scratchInterpJ) {
    // Backward compatibility with older saves.
    impl->scratchInterpolationMode =
      json_boolean_value(scratchInterpJ) ? SCRATCH_INTERP_LAGRANGE6 : SCRATCH_INTERP_CUBIC;
  }
  if (platterCursorLockJ) {
    impl->platterCursorLock = json_boolean_value(platterCursorLockJ);
  }
  if (freezeTraceLoggingJ) {
    impl->freezeTraceLoggingEnabled = json_boolean_value(freezeTraceLoggingJ);
  }
  if (slipReturnModeJ) {
    impl->slipReturnMode = clamp((int)json_integer_value(slipReturnModeJ), SLIP_RETURN_SLOW, SLIP_RETURN_COUNT - 1);
  }
  if (cartridgeV2J) {
    impl->cartridgeCharacter = clamp((int)json_integer_value(cartridgeV2J), 0, CARTRIDGE_COUNT - 1);
  } else if (cartridgeJ) {
    int legacy = (int)json_integer_value(cartridgeJ);
    // Legacy mapping before Q.Bert existed:
    // 0 Clean, 1 M44-7, 2 Concorde, 3 680 HP, 4 Lo-Fi
    if (legacy == 4) {
      impl->cartridgeCharacter = CARTRIDGE_LOFI;
    } else {
      impl->cartridgeCharacter = clamp(legacy, 0, CARTRIDGE_COUNT - 1);
    }
  }
  if (bufferDurationJ) {
    impl->bufferDurationMode.store(clamp((int)json_integer_value(bufferDurationJ), 0, BUFFER_DURATION_COUNT - 1));
  }
  if (sampleModeEnabledJ) {
    impl->sampleModeEnabled.store(json_boolean_value(sampleModeEnabledJ), std::memory_order_relaxed);
  }
  if (sampleAutoPlayOnLoadJ) {
    std::lock_guard<std::mutex> lock(impl->sampleStateMutex);
    impl->sampleAutoPlayOnLoad = json_boolean_value(sampleAutoPlayOnLoadJ);
  }
  int mode = clamp(impl->bufferDurationMode.load(), 0, BUFFER_DURATION_COUNT - 1);
  impl->engine.bufferDurationMode = mode;
  if (paramQuantities[BUFFER_PARAM]) {
    paramQuantities[BUFFER_PARAM]->displayMultiplier = usableBufferSecondsForMode(mode);
  }
  if (samplePathJ && json_is_string(samplePathJ)) {
    std::string error;
    loadSampleFromPath(json_string_value(samplePathJ), &error);
  }
}

void TemporalDeck::process(const ProcessArgs &args) {
  int requestedBufferMode = clamp(impl->bufferDurationMode.load(std::memory_order_relaxed), 0, BUFFER_DURATION_COUNT - 1);
  bool bufferModeChanged = requestedBufferMode != impl->engine.bufferDurationMode;
  bool sampleStateApplyRequested = impl->pendingSampleStateApply.exchange(false);
  if (bufferModeChanged || args.sampleRate != impl->cachedSampleRate || sampleStateApplyRequested) {
    applySampleRateChange(args.sampleRate);
  }
  bool desiredSampleModeEnabled = impl->sampleModeEnabled.load(std::memory_order_relaxed);

  if (impl->freezeTrigger.process(params[FREEZE_PARAM].getValue())) {
    bool next = !impl->freezeLatched;
    impl->freezeLatched = next;
    if (next) {
      impl->reverseLatched = false;
      impl->slipLatched = false;
    }
  }
  if (impl->reverseTrigger.process(params[REVERSE_PARAM].getValue())) {
    bool next = !impl->reverseLatched;
    impl->reverseLatched = next;
    if (next) {
      impl->freezeLatched = false;
      impl->slipLatched = false;
      if (desiredSampleModeEnabled && impl->engine.sampleLoaded) {
        // In sample mode, REV should have immediate transport effect.
        impl->engine.sampleTransportPlaying = true;
        impl->uiSampleTransportPlaying.store(true);
      }
    }
  }
  if (impl->slipTrigger.process(params[SLIP_PARAM].getValue())) {
    if (!impl->slipLatched) {
      impl->slipLatched = true;
      impl->slipReturnMode = SLIP_RETURN_SLOW;
      impl->freezeLatched = false;
      impl->reverseLatched = false;
    } else if (impl->slipReturnMode == SLIP_RETURN_SLOW) {
      impl->slipReturnMode = SLIP_RETURN_NORMAL;
      impl->freezeLatched = false;
      impl->reverseLatched = false;
    } else if (impl->slipReturnMode == SLIP_RETURN_NORMAL) {
      impl->slipReturnMode = SLIP_RETURN_INSTANT;
      impl->freezeLatched = false;
      impl->reverseLatched = false;
    } else {
      impl->slipLatched = false;
    }
  }
  if (impl->cartridgeCycleTrigger.process(params[CARTRIDGE_CYCLE_PARAM].getValue())) {
    impl->cartridgeCharacter = nextCartridgeCharacter(impl->cartridgeCharacter);
  }

  float inL = inputs[INPUT_L_INPUT].getVoltage();
  float inR = inputs[INPUT_R_INPUT].isConnected() ? inputs[INPUT_R_INPUT].getVoltage() : inL;
  float positionCv = inputs[POSITION_CV_INPUT].getVoltage();
  float rateCv = inputs[RATE_CV_INPUT].getVoltage();
  bool rateCvConnected = inputs[RATE_CV_INPUT].isConnected();
  bool freezeGateHigh = inputs[FREEZE_GATE_INPUT].getVoltage() >= TemporalDeckEngine::kFreezeGateThreshold;
  bool scratchGateHigh = inputs[SCRATCH_GATE_INPUT].getVoltage() >= TemporalDeckEngine::kScratchGateThreshold;
  bool scratchGateConnected = inputs[SCRATCH_GATE_INPUT].isConnected();
  bool positionConnected = inputs[POSITION_CV_INPUT].isConnected();

  impl->engine.scratchInterpolationMode = impl->scratchInterpolationMode;
  impl->engine.slipReturnMode = impl->slipReturnMode;
  impl->engine.cartridgeCharacter = impl->cartridgeCharacter;
  impl->engine.sampleModeEnabled = desiredSampleModeEnabled;
  uint32_t pendingSeekRevision = impl->pendingSampleSeekRevision.load(std::memory_order_relaxed);
  if (pendingSeekRevision != impl->appliedSampleSeekRevision) {
    float seekNorm = clamp(impl->pendingSampleSeekNormalized.load(std::memory_order_relaxed), 0.f, 1.f);
    if (impl->engine.sampleLoaded && impl->engine.sampleFrames > 0) {
      double sampleEndPos = std::max(0.0, double(impl->engine.sampleFrames - 1));
      double sampleWindowEndPos = sampleEndPos * double(clamp(params[BUFFER_PARAM].getValue(), 0.f, 1.f));
      // Arc seek maps to absolute sample time over the full arc; window limit
      // then clamps that target if it lies past the current playback cap.
      double targetFrame = clampd(double(seekNorm) * sampleEndPos, 0.0, sampleWindowEndPos);
      impl->engine.samplePlayhead = targetFrame;
      impl->engine.readHead = targetFrame;
      impl->engine.scratchLagSamples = 0.0;
      impl->engine.scratchLagTargetSamples = 0.0;
      impl->engine.nowCatchActive = false;
      impl->engine.slipReturning = false;
      impl->engine.slipFinalCatchActive = false;
    }
    impl->appliedSampleSeekRevision = pendingSeekRevision;
  }
  int scratchHold = impl->platterScratchHoldSamples.load();
  bool wheelScratchHeld = scratchHold > 0;
  if (wheelScratchHeld) {
    impl->platterScratchHoldSamples.store(std::max(0, scratchHold - 1));
  }
  int motionFresh = impl->platterMotionFreshSamples.load();
  bool platterMotionActive = motionFresh > 0;
  if (platterMotionActive) {
    impl->platterMotionFreshSamples.store(std::max(0, motionFresh - 1));
  }
  float wheelDelta = impl->platterWheelDelta.exchange(0.f);

  auto frame =
    impl->engine.process(args.sampleTime, inL, inR, params[BUFFER_PARAM].getValue(), params[RATE_PARAM].getValue(),
                       params[MIX_PARAM].getValue(), params[FEEDBACK_PARAM].getValue(), impl->freezeLatched,
                       impl->reverseLatched, impl->slipLatched, impl->quickSlipTrigger.exchange(false),
                       freezeGateHigh, scratchGateHigh,
                       scratchGateConnected, positionConnected, positionCv,
                       rateCv, rateCvConnected, impl->platterTouched.load(), wheelScratchHeld, platterMotionActive,
                       impl->platterGestureRevision.load(), impl->platterLagTarget.load(),
                       impl->platterGestureVelocity.load(), wheelDelta);

  if (frame.autoFreezeRequested && !impl->freezeLatched && !freezeGateHigh) {
    impl->freezeLatched = true;
    impl->reverseLatched = false;
    impl->slipLatched = false;
  }

  outputs[OUTPUT_L_OUTPUT].setVoltage(frame.outL);
  outputs[OUTPUT_R_OUTPUT].setVoltage(frame.outR);
  lights[FREEZE_LIGHT].setBrightness(impl->freezeLatched ? 1.f : 0.f);
  lights[REVERSE_LIGHT].setBrightness(impl->reverseLatched ? 1.f : 0.f);
  if (!impl->slipLatched) {
    lights[SLIP_SLOW_LIGHT].setBrightness(0.f);
    lights[SLIP_LIGHT].setBrightness(0.f);
    lights[SLIP_FAST_LIGHT].setBrightness(0.f);
  } else {
    float selectedModeBrightness = 1.f;
    float unselectedModeBrightness = 0.03f;
    lights[SLIP_SLOW_LIGHT].setBrightness(impl->slipReturnMode == SLIP_RETURN_SLOW ? selectedModeBrightness
                                                                                    : unselectedModeBrightness);
    lights[SLIP_LIGHT].setBrightness(impl->slipReturnMode == SLIP_RETURN_NORMAL ? selectedModeBrightness
                                                                                 : unselectedModeBrightness);
    lights[SLIP_FAST_LIGHT].setBrightness(impl->slipReturnMode == SLIP_RETURN_INSTANT ? selectedModeBrightness
                                                                                        : unselectedModeBrightness);
  }
  impl->uiPlatterAngle.store(frame.platterAngle);
  impl->uiLagSamples.store(frame.lag);
  impl->uiAccessibleLagSamples.store(frame.accessibleLag);
  impl->uiSampleRate.store(args.sampleRate);
  impl->uiFreezeLatched.store(impl->freezeLatched);
  impl->uiSampleModeEnabled.store(frame.sampleMode);
  impl->uiSampleLoaded.store(frame.sampleLoaded);
  impl->uiSampleTransportPlaying.store(frame.sampleTransportPlaying);
  impl->uiSamplePlayheadSeconds.store(frame.samplePlayhead);
  impl->uiSampleDurationSeconds.store(frame.sampleDuration);
  impl->uiSampleProgress.store(frame.sampleProgress);

  impl->sampleModeEnabled.store(impl->engine.sampleModeEnabled, std::memory_order_relaxed);
  impl->uiPublishTimerSec += args.sampleTime;
  if (impl->uiPublishTimerSec >= kUiPublishIntervalSec) {
    impl->uiPublishTimerSec = std::fmod(impl->uiPublishTimerSec, kUiPublishIntervalSec);
    std::array<float, kArcLightCount> arcYellow{};
    std::array<float, kArcLightCount> arcRed{};
    std::array<float, kArcLightCount> limitYellowBlendAllowed{};
    const float limitIndicatorRedBrightness = 0.9f;
    const float limitApproachWindowLeds = 2.0f;
    auto applyLimitMarker = [&](int i, float playheadLed, float limitLed, float *brightness, bool allowYellowBlend) {
      bool isLimitLed = std::fabs(float(i) - limitLed) < 0.5f;
      if (isLimitLed) {
        limitYellowBlendAllowed[i] = allowYellowBlend ? 1.f : 0.f;
        if (allowYellowBlend) {
          float approach = clamp(1.f - std::fabs(playheadLed - limitLed) / limitApproachWindowLeds, 0.f, 1.f);
          *brightness = std::max(*brightness, 0.42f * approach);
        }
      }
      arcRed[i] = isLimitLed ? limitIndicatorRedBrightness : 0.f;
    };
    if (frame.sampleMode && frame.sampleLoaded) {
      // Sample mode fills from left->right. The red limit marker indicates the
      // effective playback end (full sample end or knob-limited end).
      float leftLed = float(kArcLightCount - 1);
      float sampleNewest = std::max(1.f, float(impl->engine.sampleFrames - 1));
      float limitRatio = clamp(float(frame.accessibleLag / sampleNewest), 0.f, 1.f);
      float sampleEndLed = (1.f - limitRatio) * leftLed;
      float progressNorm = clamp(float(frame.sampleProgress), 0.f, 1.f);
      float ledSpan = std::max(0.f, leftLed - sampleEndLed);
      float progressLedUnits = progressNorm * ledSpan;
      float playheadLed = leftLed - progressLedUnits;
      for (int i = 0; i < kArcLightCount; ++i) {
        // Progressive fill from left to right with fractional leading LED.
        // At start, all LEDs are off; the leading LED ramps in proportionally.
        float ledFromStart = leftLed - float(i);
        float fill = clamp(progressLedUnits - ledFromStart, 0.f, 1.f);
        float brightness = 0.92f * fill;
        // Keep anything beyond the effective sample end dark.
        if (float(i) + 0.5f < sampleEndLed) {
          brightness = 0.f;
        }
        bool allowYellowBlend = brightness > 1e-4f;
        applyLimitMarker(i, playheadLed, sampleEndLed, &brightness, allowYellowBlend);
        arcYellow[i] = brightness;
      }
    } else {
      int mode = clamp(impl->bufferDurationMode.load(std::memory_order_relaxed), 0, BUFFER_DURATION_COUNT - 1);
      float maxLag = std::max(1.f, args.sampleRate * usableBufferSecondsForMode(mode));
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
        if (float(i) > limitLed + 0.5f) {
          brightness = 0.f;
        }
        bool allowYellowBlend = brightness > 1e-4f;
        applyLimitMarker(i, lagLed, limitLed, &brightness, allowYellowBlend);
        arcYellow[i] = brightness;
      }
    }

    // Small spatial blend to remove visible dead-zones between LEDs while
    // preserving endpoint markers.
    smoothLedBrightness(arcYellow);
    for (int i = 0; i < kArcLightCount; ++i) {
      if (arcRed[i] > 0.f && limitYellowBlendAllowed[i] < 0.5f) {
        arcYellow[i] = 0.f;
      }
    }
    for (int i = 0; i < kArcLightCount; ++i) {
      lights[ARC_LIGHT_START + i].setBrightness(arcYellow[i]);
      lights[ARC_MAX_LIGHT_START + i].setBrightness(arcRed[i]);
    }
  }
}

void TemporalDeck::setPlatterScratch(bool touched, float lagSamples, float velocitySamples, int holdSamples) {
  impl->platterTouched.store(touched);
  impl->platterGestureRevision.fetch_add(1);
  impl->platterLagTarget.store(lagSamples);
  impl->platterGestureVelocity.store(velocitySamples);
  impl->platterScratchHoldSamples.store(std::max(0, holdSamples));
  if (touched || holdSamples == 0) {
    impl->platterWheelDelta.store(0.f);
  }
}

void TemporalDeck::setPlatterMotionFreshSamples(int motionFreshSamples) {
  impl->platterMotionFreshSamples.store(std::max(0, motionFreshSamples));
}

void TemporalDeck::addPlatterWheelDelta(float delta, int holdSamples) {
  float expected = impl->platterWheelDelta.load(std::memory_order_relaxed);
  while (!impl->platterWheelDelta.compare_exchange_weak(expected, expected + delta, std::memory_order_relaxed,
                                                         std::memory_order_relaxed)) {
  }
  impl->platterScratchHoldSamples.store(std::max(0, holdSamples));
}

void TemporalDeck::triggerQuickSlipReturn() {
  impl->quickSlipTrigger.store(true);
}

double TemporalDeck::getUiLagSamples() const {
  return impl->uiLagSamples.load();
}

double TemporalDeck::getUiAccessibleLagSamples() const {
  return impl->uiAccessibleLagSamples.load();
}

float TemporalDeck::getUiSampleRate() const {
  return impl->uiSampleRate.load();
}

float TemporalDeck::getUiPlatterAngle() const {
  return impl->uiPlatterAngle.load();
}

bool TemporalDeck::isUiFreezeLatched() const {
  return impl->uiFreezeLatched.load();
}


bool TemporalDeck::isSampleModeEnabled() const {
  return impl->uiSampleModeEnabled.load();
}

bool TemporalDeck::hasLoadedSample() const {
  return impl->uiSampleLoaded.load();
}

bool TemporalDeck::isSampleAutoPlayOnLoadEnabled() const {
  std::lock_guard<std::mutex> lock(impl->sampleStateMutex);
  return impl->sampleAutoPlayOnLoad;
}

void TemporalDeck::setSampleAutoPlayOnLoadEnabled(bool enabled) {
  {
    std::lock_guard<std::mutex> lock(impl->sampleStateMutex);
    impl->sampleAutoPlayOnLoad = enabled;
  }
  impl->pendingSampleStateApply.store(true);
}

void TemporalDeck::setSampleModeEnabled(bool enabled) {
  impl->sampleModeEnabled.store(enabled, std::memory_order_relaxed);
  impl->pendingSampleStateApply.store(true);
  impl->uiSampleModeEnabled.store(enabled && impl->engine.sampleLoaded);
}

bool TemporalDeck::isSampleTransportPlaying() const {
  return impl->uiSampleTransportPlaying.load();
}

void TemporalDeck::setSampleTransportPlaying(bool enabled) {
  impl->engine.sampleTransportPlaying = enabled && impl->engine.sampleLoaded;
  impl->uiSampleTransportPlaying.store(impl->engine.sampleTransportPlaying);
}

void TemporalDeck::stopSampleTransport() {
  impl->engine.sampleTransportPlaying = false;
  if (impl->engine.sampleLoaded) {
    impl->engine.samplePlayhead = 0.0;
    impl->engine.readHead = 0.0;
  }
  impl->uiSampleTransportPlaying.store(false);
  impl->uiSamplePlayheadSeconds.store(0.0);
  impl->uiSampleProgress.store(0.0);
}

void TemporalDeck::clearLoadedSample() {
  {
    std::lock_guard<std::mutex> lock(impl->sampleStateMutex);
    impl->samplePath.clear();
    impl->sampleDisplayName.clear();
    impl->decodedSample = DecodedSampleFile();
  }
  impl->sampleModeEnabled.store(false, std::memory_order_relaxed);
  impl->bufferDurationMode.store(BUFFER_DURATION_10S);
  if (paramQuantities[BUFFER_PARAM]) {
    paramQuantities[BUFFER_PARAM]->displayMultiplier = usableBufferSecondsForMode(BUFFER_DURATION_10S);
  }
  impl->pendingSampleStateApply.store(true);
}

bool TemporalDeck::loadSampleFromPath(const std::string &path, std::string *errorOut) {
  DecodedSampleFile decoded;
  if (!decodeSampleFile(path, &decoded, errorOut)) {
    return false;
  }

  int targetMode = chooseSampleBufferMode(decoded);
  impl->bufferDurationMode.store(targetMode);
  if (paramQuantities[BUFFER_PARAM]) {
    paramQuantities[BUFFER_PARAM]->displayMultiplier = usableBufferSecondsForMode(targetMode);
  }
  bool autoPlayOnLoad = true;
  {
    std::lock_guard<std::mutex> lock(impl->sampleStateMutex);
    impl->samplePath = path;
    impl->sampleDisplayName = system::getFilename(path);
    impl->decodedSample = std::move(decoded);
    autoPlayOnLoad = impl->sampleAutoPlayOnLoad;
  }
  impl->sampleModeEnabled.store(true, std::memory_order_relaxed);
  impl->freezeLatched = !autoPlayOnLoad;
  impl->reverseLatched = false;
  impl->slipLatched = false;
  impl->pendingSampleStateApply.store(true);
  return true;
}

void TemporalDeck::seekSampleByNormalizedPosition(double normalized) {
  impl->pendingSampleSeekNormalized.store(clamp(float(normalized), 0.f, 1.f), std::memory_order_relaxed);
  impl->pendingSampleSeekRevision.fetch_add(1, std::memory_order_relaxed);
}

double TemporalDeck::getUiSamplePlayheadSeconds() const {
  return impl->uiSamplePlayheadSeconds.load();
}

double TemporalDeck::getUiSampleDurationSeconds() const {
  return impl->uiSampleDurationSeconds.load();
}

double TemporalDeck::getUiSampleProgress() const {
  return impl->uiSampleProgress.load();
}

std::string TemporalDeck::getLoadedSampleDisplayName() const {
  std::lock_guard<std::mutex> lock(impl->sampleStateMutex);
  return impl->sampleDisplayName;
}

bool TemporalDeck::wasLoadedSampleTruncated() const {
  return impl->engine.sampleTruncated;
}

bool TemporalDeck::isSlipLatched() const {
  return impl->slipLatched;
}

int TemporalDeck::getCartridgeCharacter() const {
  return impl->cartridgeCharacter;
}

int TemporalDeck::getBufferDurationMode() const {
  return clamp(impl->bufferDurationMode.load(), 0, BUFFER_DURATION_COUNT - 1);
}

bool TemporalDeck::isBufferModeMono() const {
  return isMonoBufferMode(clamp(impl->bufferDurationMode.load(), 0, BUFFER_DURATION_COUNT - 1));
}

bool TemporalDeck::isPlatterCursorLockEnabled() const {
  return impl->platterCursorLock;
}

void TemporalDeck::setPlatterCursorLockEnabled(bool enabled) {
  impl->platterCursorLock = enabled;
}

bool TemporalDeck::isFreezeTraceLoggingEnabled() const {
  return impl->freezeTraceLoggingEnabled;
}

void TemporalDeck::setFreezeTraceLoggingEnabled(bool enabled) {
  impl->freezeTraceLoggingEnabled = enabled;
}

bool TemporalDeck::isHighQualityScratchInterpolationEnabled() const {
  return impl->scratchInterpolationMode != SCRATCH_INTERP_CUBIC;
}

void TemporalDeck::setHighQualityScratchInterpolationEnabled(bool enabled) {
  impl->scratchInterpolationMode = enabled ? SCRATCH_INTERP_LAGRANGE6 : SCRATCH_INTERP_CUBIC;
}

int TemporalDeck::getScratchInterpolationMode() const {
  return impl->scratchInterpolationMode;
}

void TemporalDeck::setScratchInterpolationMode(int mode) {
  impl->scratchInterpolationMode = clamp(mode, SCRATCH_INTERP_CUBIC, SCRATCH_INTERP_COUNT - 1);
}

void TemporalDeck::setSlipLatched(bool enabled) {
  impl->slipLatched = enabled;
  if (enabled) {
    impl->freezeLatched = false;
    impl->reverseLatched = false;
  }
}

int TemporalDeck::getSlipReturnMode() const {
  return impl->slipReturnMode;
}

void TemporalDeck::setSlipReturnMode(int mode) {
  impl->slipReturnMode = clamp(mode, SLIP_RETURN_SLOW, SLIP_RETURN_COUNT - 1);
}
