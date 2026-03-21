#include "TemporalDeck.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <limits>
#include <string>
#include <utility>
#include <vector>

namespace {

static float realBufferSecondsForMode(int index) {
  switch (index) {
  case 1:
    return 17.f;
  case 2:
    return 481.f;
  case 0:
  default:
    return 9.f;
  }
}

static float usableBufferSecondsForMode(int index) { return std::max(1.f, realBufferSecondsForMode(index) - 1.f); }

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
  float durationSeconds = 9.f;

  void reset(float sr, float seconds = 9.f) {
    sampleRate = sr;
    durationSeconds = std::max(1.f, seconds);
    size = std::max(1, int(std::round(sampleRate * durationSeconds)));
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
    int i0 = wrapIndex(i1 - 1);
    int i2 = wrapIndex(i1 + 1);
    int i3 = wrapIndex(i1 + 2);
    return {cubicSample(left[i0], left[i1], left[i2], left[i3], t),
            cubicSample(right[i0], right[i1], right[i2], right[i3], t)};
  }

  std::pair<float, float> readLinear(double pos) const {
    if (size <= 0 || filled <= 0) {
      return {0.f, 0.f};
    }
    pos = wrapPosition(pos);
    int i0 = int(pos);
    int i1 = wrapIndex(i0 + 1);
    float t = float(pos - double(i0));
    return {crossfade(left[i0], left[i1], t), crossfade(right[i0], right[i1], t)};
  }

  std::pair<float, float> readHighQuality(double pos) const {
    if (size <= 0 || filled <= 0) {
      return {0.f, 0.f};
    }
    pos = wrapPosition(pos);
    int i2 = int(std::floor(pos));
    float t = float(pos - double(i2));
    int i0 = wrapIndex(i2 - 2);
    int i1 = wrapIndex(i2 - 1);
    int i3 = wrapIndex(i2 + 1);
    int i4 = wrapIndex(i2 + 2);
    int i5 = wrapIndex(i2 + 3);
    std::array<float, 6> l = {left[i0], left[i1], left[i2], left[i3], left[i4], left[i5]};
    std::array<float, 6> r = {right[i0], right[i1], right[i2], right[i3], right[i4], right[i5]};
    return {lagrange6Sample(l, t), lagrange6Sample(r, t)};
  }

  std::pair<float, float> readSinc(double pos) const {
    if (size <= 0 || filled <= 0) {
      return {0.f, 0.f};
    }
    pos = wrapPosition(pos);
    int center = int(std::floor(pos));
    float frac = float(pos - double(center));

    constexpr int kRadius = 8;
    float accL = 0.f;
    float accR = 0.f;
    float weightSum = 0.f;
    for (int k = -kRadius + 1; k <= kRadius; ++k) {
      int idx = wrapIndex(center + k);
      float dist = float(k) - frac;
      float w = windowedSinc(dist, float(kRadius));
      accL += left[idx] * w;
      accR += right[idx] * w;
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
  enum ScratchModel { SCRATCH_MODEL_LEGACY, SCRATCH_MODEL_HYBRID, SCRATCH_MODEL_SCRATCH3, SCRATCH_MODEL_COUNT };
  enum BufferDurationMode { BUFFER_DURATION_8S, BUFFER_DURATION_16S, BUFFER_DURATION_8MIN, BUFFER_DURATION_COUNT };
  static_assert(CARTRIDGE_CLEAN == TemporalDeck::CARTRIDGE_CLEAN, "Cartridge enum mismatch: CLEAN");
  static_assert(CARTRIDGE_M44_7 == TemporalDeck::CARTRIDGE_M44_7, "Cartridge enum mismatch: M44-7");
  static_assert(CARTRIDGE_CONCORDE_SCRATCH == TemporalDeck::CARTRIDGE_ORTOFON_SCRATCH,
                "Cartridge enum mismatch: Ortofon Scratch");
  static_assert(CARTRIDGE_680_HP == TemporalDeck::CARTRIDGE_STANTON_680HP,
                "Cartridge enum mismatch: Stanton 680HP");
  static_assert(CARTRIDGE_QBERT == TemporalDeck::CARTRIDGE_QBERT, "Cartridge enum mismatch: Q.Bert");
  static_assert(CARTRIDGE_LOFI == TemporalDeck::CARTRIDGE_LOFI, "Cartridge enum mismatch: Lo-Fi");
  static_assert(CARTRIDGE_COUNT == TemporalDeck::CARTRIDGE_COUNT, "Cartridge enum mismatch: count");
  static_assert(SCRATCH_MODEL_LEGACY == TemporalDeck::SCRATCH_MODEL_LEGACY, "Scratch model enum mismatch: Legacy");
  static_assert(SCRATCH_MODEL_HYBRID == TemporalDeck::SCRATCH_MODEL_HYBRID, "Scratch model enum mismatch: Hybrid");
  static_assert(SCRATCH_MODEL_SCRATCH3 == TemporalDeck::SCRATCH_MODEL_SCRATCH3,
                "Scratch model enum mismatch: Scratch3");
  static_assert(SCRATCH_MODEL_COUNT == TemporalDeck::SCRATCH_MODEL_COUNT, "Scratch model enum mismatch: count");
  static_assert(BUFFER_DURATION_8S == TemporalDeck::BUFFER_DURATION_8S, "Buffer duration enum mismatch: 8s");
  static_assert(BUFFER_DURATION_16S == TemporalDeck::BUFFER_DURATION_16S, "Buffer duration enum mismatch: 16s");
  static_assert(BUFFER_DURATION_8MIN == TemporalDeck::BUFFER_DURATION_8M, "Buffer duration enum mismatch: 8m");
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
  int scratchModel = SCRATCH_MODEL_HYBRID;
  int bufferDurationMode = BUFFER_DURATION_8S;
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
  float cachedDriveNorm = 1.f;
  float cachedMakeupGain = 1.f;
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

  void reset(float sr) {
    sampleRate = sr;
    buffer.reset(sr, realBufferSecondsForMode(bufferDurationMode));
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
    cachedDriveNorm = 1.f;
    cachedMakeupGain = 1.f;
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

  float computeBaseSpeed(float rateKnob, float rateCv, bool reverse) const {
    float speed = baseSpeedFromKnob(rateKnob);
    speed += clamp(rateCv / 5.f, -1.f, 1.f);
    speed = clamp(speed, -3.f, 3.f);
    if (reverse) {
      speed *= -1.f;
    }
    return speed;
  }

  float configuredSlipReturnTime() const {
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
    cachedDriveNorm = std::max(fastTanh(cachedCartridgeParams.drive), 1e-6f);
    cachedMakeupGain = makeupGainForCartridge(cartridgeCharacter);
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

  std::pair<float, float> applyCartridgeCharacter(std::pair<float, float> in, float motionAmount) {
    if (cartridgeCharacter == CARTRIDGE_CLEAN) {
      return in;
    }
    refreshCartridgeCache();
    const CartridgeParams &p = cachedCartridgeParams;
    motionAmount = clamp(motionAmount, 0.f, 1.f);

    // Motion amount modulates LP corner for "stylus under motion" dulling.
    float lpHz = p.lpHz + (p.lpMotionHz - p.lpHz) * (motionAmount * p.motionDulling);
    float lpCoeff = onePoleCoeff(lpHz);

    auto processChannel = [&](float x, CartridgeChannelState &state, float lpCoeff) {
      float rumble = state.rumble.lowpass(x, cachedHpCoeff);
      float hp = x - rumble;
      float body = state.body.lowpass(hp, cachedBodyCoeff);
      float air = state.air.lowpass(hp, lpCoeff);
      float presence = air - body;
      float voiced = air + p.bodyGain * body + p.presenceGain * presence;
      if (p.drive > 1.f) {
        float dry = voiced;
        float sat = fastTanh(voiced * p.drive) / cachedDriveNorm;
        voiced = crossfade(dry, sat, clamp(p.saturationMix, 0.f, 1.f));
      }
      return voiced;
    };

    float left = processChannel(in.first, cartridgeLeft, lpCoeff);
    float right = processChannel(in.second, cartridgeRight, lpCoeff);
    if (p.stereoTilt != 0.f) {
      // Lightweight stereo mismatch emulation (channel imbalance/azimuth-ish).
      float tilt = clamp(p.stereoTilt, -0.2f, 0.2f);
      left *= 1.f - tilt * 0.5f;
      right *= 1.f + tilt * 0.5f;
    }
    float xfeed = clamp(p.crossfeed, 0.f, 0.45f);
    if (xfeed > 0.f) {
      float mixedL = left * (1.f - xfeed) + right * xfeed;
      float mixedR = right * (1.f - xfeed) + left * xfeed;
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
    return {left, right};
  }

  double currentLag() const {
    if (buffer.size <= 0) {
      return 0.0;
    }
    double lag = newestReadablePos() - readHead;
    if (lag < 0.0) {
      lag += double(buffer.size);
    }
    return lag;
  }

  double newestReadablePos() const {
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
    double lag = newestPos - readHead;
    if (lag < 0.0) {
      lag += double(buffer.size);
    }
    return lag;
  }

  double unwrapReadNearWrite(double readPos, double writePos) const {
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
  };

  FrameResult process(float dt, float inL, float inR, float bufferKnob, float rateKnob, float mixKnob,
                      float feedbackKnob, bool freezeButton, bool reverseButton, bool slipButton, bool freezeGate,
                      bool scratchGate, bool scratchGateConnected, bool positionConnected, float positionCv,
                      float rateCv, bool platterTouched, bool wheelScratchHeld, bool platterMotionActive,
                      uint32_t platterGestureRevision, float platterLagTarget, float platterGestureVelocity,
                      float wheelDelta) {
    FrameResult result;
    double prevReadHead = readHead;
    float nowSnapThresholdSamples = sampleRate * (kNowSnapThresholdMs / 1000.f);
    bool pinToNow = false;
    bool keepSlipLagAligned = false;
    bool keepNowCatchLagAligned = false;
    freezeState = freezeButton || freezeGate;
    reverseState = reverseButton;
    bool prevSlipState = slipState;
    slipState = slipButton;
    bool slipModeChanged = slipReturnMode != lastSlipReturnMode;
    lastSlipReturnMode = slipReturnMode;

    double limit = accessibleLag(bufferKnob);
    double minLag = 0.0;
    double maxLag = std::max(limit, 0.0);
    float baseSpeed = computeBaseSpeed(rateKnob, rateCv, reverseState);
    float speed = baseSpeed;
    bool externalScratch = scratchGateConnected && scratchGate && positionConnected;
    bool positionFollow = positionConnected && !scratchGateConnected;
    bool manualTouchScratch = platterTouched;
    bool wheelScratch = wheelScratchHeld;
    bool manualScratch = manualTouchScratch || wheelScratch;
    bool hybridManualScratch = manualScratch && scratchModel == SCRATCH_MODEL_HYBRID;
    bool scratch3ManualScratch = manualScratch && scratchModel == SCRATCH_MODEL_SCRATCH3;
    bool anyScratch = externalScratch || manualScratch;
    bool wasScratchActive = scratchActive;
    bool releasedFromScratch = !anyScratch && wasScratchActive;
    bool slipJustEnabled = slipState && !prevSlipState;
    double newestPos = newestReadablePos();
    bool hasFreshPlatterGesture = platterGestureRevision != lastPlatterGestureRevision;
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

    if (releasedFromScratch && currentLagFromNewest(newestPos) <= nowSnapThresholdSamples) {
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

    // If slip speed mode changes while Slip is active and we are behind NOW,
    // immediately engage a return so the new mode takes audible effect.
    if (slipModeChanged && slipState && !anyScratch && currentLagFromNewest(newestPos) > nowSnapThresholdSamples) {
      slipReturning = true;
      slipFinalCatchActive = false;
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
    bool reverseAtOldestEdge =
      !scratchActive && !slipReturning && reverseState && limit > 0.f && lagNow >= (limit - 0.5f);
    if (reverseAtOldestEdge && speed < 0.f) {
      speed = 0.f;
    }

    if (manualScratch) {
      if (hybridManualScratch) {
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
            if (hasFreshPlatterGesture) {
              // Convert lag-space gesture velocity into buffer read velocity.
              // d(lag)/dt = writeVel - readVel  =>  readVel = writeVel + gestureVel
              //
              // Only consume fresh gesture velocity here; reusing stale
              // velocity while the mouse is held still causes directional drift.
              targetReadVelocity = sampleRate + platterGestureVelocity;
            }
            float motionNorm = clamp(std::fabs(targetReadVelocity) / std::max(sampleRate * 0.45f, 1.f), 0.f, 1.f);
            integrateHybridScratch(dt, limit, newestPos, targetReadVelocity, 1.55f + 0.55f * motionNorm,
                                   0.72f - 0.12f * motionNorm, 0.68f, nowSnapThresholdSamples);
          }
        } else {
          float wheelDeltaSoftRange = sampleRate * 0.16f * kWheelScratchTravelScale;
          float wheelDeltaShaped = wheelDeltaSoftRange * std::tanh(wheelDelta / std::max(wheelDeltaSoftRange, 1e-6f));
          if (wheelDeltaShaped < 0.f) {
            wheelDeltaShaped *= 2.0f;
          }
          if (std::fabs(wheelDelta) > 1e-6f) {
            scratchLagTargetSamples = clampLag(scratchLagTargetSamples + wheelDeltaShaped, limit);
            if (scratchLagTargetSamples < sampleRate * 0.012f) {
              scratchLagTargetSamples = 0.f;
            }
            scratchWheelVelocityBurst -= wheelDeltaShaped / std::max(kHybridScratchWheelImpulseTime, 1e-6f);
            scratchWheelVelocityBurst =
              clamp(scratchWheelVelocityBurst, -kHybridScratchMaxVelocity, kHybridScratchMaxVelocity);
          }
          decayHybridWheelBurst(dt);
          integrateHybridScratch(dt, limit, newestPos, 0.f, 0.92f, 1.0f, 1.05f, nowSnapThresholdSamples);
        }
      } else if (scratch3ManualScratch) {
        scratchLagSamples = clampLag(currentLagFromNewest(newestPos), limit);
        if (manualTouchScratch) {
          scratchWheelVelocityBurst = 0.f;
          if (hasFreshPlatterGesture) {
            lastPlatterGestureRevision = platterGestureRevision;
          }
          integrateScratch3Touch(dt, limit, newestPos, prevReadHead, hasFreshPlatterGesture, platterMotionActive,
                                 platterLagTarget, platterGestureVelocity, nowSnapThresholdSamples);
        } else {
          float wheelDeltaSoftRange = sampleRate * 0.16f * kWheelScratchTravelScale;
          float wheelDeltaShaped = wheelDeltaSoftRange * std::tanh(wheelDelta / std::max(wheelDeltaSoftRange, 1e-6f));
          if (wheelDeltaShaped < 0.f) {
            wheelDeltaShaped *= 2.0f;
          }
          if (std::fabs(wheelDelta) > 1e-6f) {
            scratchLagTargetSamples = clampLag(scratchLagTargetSamples + wheelDeltaShaped, limit);
            if (scratchLagTargetSamples < sampleRate * 0.012f) {
              scratchLagTargetSamples = 0.f;
            }
            scratchWheelVelocityBurst -= wheelDeltaShaped / std::max(kHybridScratchWheelImpulseTime, 1e-6f);
            scratchWheelVelocityBurst =
              clamp(scratchWheelVelocityBurst, -kHybridScratchMaxVelocity, kHybridScratchMaxVelocity);
          }
          decayHybridWheelBurst(dt);
          integrateHybridScratch(dt, limit, newestPos, 0.f, 0.92f, 1.0f, 1.05f, nowSnapThresholdSamples);
        }
      } else if (manualTouchScratch) {
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
            float alpha = clamp(dt / std::max(kSlowScratchSmoothingTime, 1e-6f), 0.f, 1.f);
            filteredManualLagTargetSamples =
              filteredManualLagTargetSamples + (rawTargetLag - filteredManualLagTargetSamples) * alpha;
            if (std::fabs(rawTargetLag - filteredManualLagTargetSamples) < kScratchTargetJitterThreshold) {
              filteredManualLagTargetSamples = rawTargetLag;
            }
          } else {
            filteredManualLagTargetSamples = rawTargetLag;
          }
          scratchLagTargetSamples = clampLag(filteredManualLagTargetSamples, limit);
          if (scratchLagTargetSamples <= nowSnapThresholdSamples) {
            startNowCatch(std::max(scratchLagSamples, scratchLagTargetSamples));
          }
          lastPlatterGestureRevision = platterGestureRevision;
        } else if (platterMotionActive) {
          // UI drag events are sparse versus audio rate. Predict target drift
          // between events so slow manual reverse feels continuous, not stepped.
          float predictedDelta = -platterGestureVelocity * dt * kManualVelocityPredictScale;
          scratchLagTargetSamples = clampLag(scratchLagTargetSamples + predictedDelta, limit);
        }
        // A mouse-down with no recent platter motion is a true freeze, not a
        // coast.
        bool stationaryManualHold = !platterMotionActive && !hasFreshPlatterGesture;
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
          bool snappedSlow = velMag < kSlowScratchVelThreshold && std::fabs(lagError) < kScratchTargetJitterThreshold;
          if (snappedSlow) {
            scratchLagSamples = scratchLagTargetSamples;
            platterVelocity = 0.f;
            readHead = buffer.wrapPosition(newestPos - scratchLagSamples);
          } else {
            float motionNorm = clamp(velMag / 140.f, 0.f, 1.f);
            float adaptiveFollowTime = kScratchFollowTime * (1.15f - 0.75f * motionNorm);
            float followProgress = clamp(dt / std::max(adaptiveFollowTime, 1e-6f), 0.f, 1.f);
            float shapedFollow = followProgress * (2.f - followProgress);
            float targetStep = lagError * shapedFollow;
            bool backwardScratch = lagError > 0.f;
            float dynamicSoftLimit =
              clamp(kScratchSoftLagStepMin + std::fabs(platterGestureVelocity) * 0.003f + std::fabs(lagError) * 0.08f,
                    kScratchSoftLagStepMin,
                    kScratchSoftLagStepMax * (backwardScratch ? 2.5f : 1.35f) * (1.f + 0.45f * motionNorm));

            // Lightweight inertia model: chase target step with a damped velocity
            // state so manual scratch movement has weight and less zipper edge.
            float followAlpha = clamp(dt * kScratchInertiaFollowHz, 0.f, 1.f);
            float damping = clamp(1.f - dt * kScratchInertiaDampingHz, 0.f, 1.f);
            platterVelocity = platterVelocity * damping + (targetStep - platterVelocity) * followAlpha;
            float lagStep = crossfade(targetStep, platterVelocity, kInertiaBlend);

            lagStep = dynamicSoftLimit * std::tanh(lagStep / std::max(dynamicSoftLimit, 1e-6f));
            if (backwardScratch && velMag > kReverseBiteVelocityThreshold) {
              float biteT =
                clamp((velMag - kReverseBiteVelocityThreshold) / std::max(kReverseBiteVelocityThreshold, 1e-6f), 0.f,
                      1.f);
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
        float wheelDeltaSoftRange = sampleRate * 0.16f * kWheelScratchTravelScale;
        float wheelDeltaShaped = wheelDeltaSoftRange * std::tanh(wheelDelta / std::max(wheelDeltaSoftRange, 1e-6f));
        if (wheelDeltaShaped < 0.f) {
          wheelDeltaShaped *= 2.0f;
        }
        // Rebase from current lag only when a new wheel event arrives to avoid
        // directional drift without collapsing the target between events.
        if (std::fabs(wheelDelta) > 1e-6f) {
          // Direction-aware base avoids "fighting" the glide when moving
          // toward NOW with repeated small forward scrolls.
          float baseLag = wheelDeltaShaped < 0.f ? std::min(scratchLagSamples, scratchLagTargetSamples)
                                                 : std::max(scratchLagSamples, scratchLagTargetSamples);
          scratchLagTargetSamples = clampLag(baseLag + wheelDeltaShaped, limit);
          // Near-zero snap keeps repeated forward wheel strokes from "hovering"
          // just above NOW due to smoothing/integration tails.
          float wheelNowSnapThreshold = sampleRate * 0.012f;
          if (scratchLagTargetSamples < wheelNowSnapThreshold) {
            scratchLagTargetSamples = 0.f;
          }
        }

        float lagError = scratchLagTargetSamples - scratchLagSamples;
        bool movingTowardNow = lagError < 0.f;
        float configuredSlipTime = std::max(configuredSlipReturnTime(), 1e-6f);
        float wheelFollowTime = movingTowardNow ? (configuredSlipTime * 0.5f) : configuredSlipTime;
        float wheelMotionNorm =
          clamp(std::fabs(wheelDeltaShaped) / std::max(sampleRate * 0.01f * kWheelScratchTravelScale, 1e-6f), 0.f, 1.f);
        wheelFollowTime *= (1.f - 0.35f * wheelMotionNorm);
        float alpha = dt / std::max(wheelFollowTime, 1e-6f);
        float lagStep = lagError * alpha;

        // Keep progression audible even for tiny alpha / small errors.
        float minStep = (movingTowardNow ? 0.8f : 0.35f) * (1.f + 0.8f * wheelMotionNorm);
        if (std::fabs(lagError) > minStep && std::fabs(lagStep) < minStep) {
          lagStep = std::copysign(minStep, lagError);
        }

        // Symmetric glide cap in both directions to avoid directional bias.
        float maxStep = kScratchSoftLagStepMax * (0.9f + 0.45f * wheelMotionNorm);
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
      } else

        if (!slipFinalCatchActive) {
        // Exponential-like approach to zero lag.
        // We target a specific lag value that decreases over time.
        float alpha = dt / std::max(slipReturnTime, 1e-6f);
        double targetLag = currentLagSamples * double(1.f - alpha);

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
    } else if (positionFollow && !externalScratch) {
      // Absolute Position CV
      double targetLag = lagForPositionCv(positionCv, limit);
      readHead = buffer.wrapPosition(newestPos - targetLag);
    } else {
      // Normal Transport
      double candidate = unwrapReadNearWrite(readHead, newestPos) + double(speed);
      candidate = std::max(newestPos - maxLag, std::min(candidate, newestPos - minLag));
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

    bool scratchReadPath = anyScratch || positionFollow;
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
    auto wet = buffer.readCubic(readHead);
    if (scratchReadPath) {
      if (effectiveScratchInterpolation == TemporalDeck::SCRATCH_INTERP_LAGRANGE6) {
        wet = buffer.readHighQuality(readHead);
      } else if (effectiveScratchInterpolation == TemporalDeck::SCRATCH_INTERP_SINC) {
        wet = buffer.readSinc(readHead);
      } else {
        wet = buffer.readCubic(readHead);
      }
    }
    wet = applyCartridgeCharacter(wet, motionAmount);
    if (scratchReadPath) {
      if (hybridManualScratch || scratch3ManualScratch) {
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
      double readDelta = readHead - prevReadHead;
      double halfSize = double(buffer.size) * 0.5;
      if (readDelta > halfSize) {
        readDelta -= double(buffer.size);
      }
      if (readDelta < -halfSize) {
        readDelta += double(buffer.size);
      }
      double visualDelta = readDelta;
      bool normalTransportVisual = !anyScratch && !positionFollow && !slipReturning;
      if (normalTransportVisual) {
        // Keep platter animation responsive to RATE even when readHead is near
        // NOW and constrained by buffer causality.
        visualDelta = double(speed);
      } else if (manualTouchScratch) {
        // Audio read motion is intentionally smoothed/limited during manual
        // scratch, which can make the platter graphic look unresponsive on
        // quick direction changes. For the visual, prefer the direct gesture
        // direction so fast back-and-forth scratches stay unambiguous.
        double gestureDelta = double(platterGestureVelocity) * double(dt);
        double platterModelDelta = hybridManualScratch ? double(scratchMotionVelocity) * double(dt)
                                  : (scratch3ManualScratch ? double(-scratch3LagVelocity) * double(dt)
                                                           : double(platterVelocity) * double(dt));
        bool gestureActive = std::fabs(gestureDelta) > 1e-5f;
        if (gestureActive) {
          // During active drag, the platter graphic should reflect the hand's
          // direction first and only use the motion model as a small stabilizer.
          visualDelta = crossfade(float(gestureDelta), float(platterModelDelta), 0.18f);
        } else {
          visualDelta = crossfade(float(readDelta), float(platterModelDelta), 0.82f);
        }
      }
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
    return result;
  }
};

struct DeckRateQuantity : ParamQuantity {
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
  std::atomic<bool> platterTouched{false};
  std::atomic<uint32_t> platterGestureRevision{0};
  std::atomic<float> platterLagTarget{0.f};
  std::atomic<float> platterGestureVelocity{0.f};
  std::atomic<float> platterWheelDelta{0.f};
  std::atomic<int> platterScratchHoldSamples{0};
  std::atomic<int> platterMotionFreshSamples{0};
  std::atomic<double> uiLagSamples{0.0};
  std::atomic<double> uiAccessibleLagSamples{0.0};
  std::atomic<float> uiSampleRate{44100.f};
  std::atomic<float> uiPlatterAngle{0.f};
  float uiPublishTimerSec = 0.f;
  int scratchInterpolationMode = TemporalDeck::SCRATCH_INTERP_LAGRANGE6;
  bool platterCursorLock = false;
  int cartridgeCharacter = TemporalDeck::CARTRIDGE_CLEAN;
  int scratchModel = TemporalDeck::SCRATCH_MODEL_HYBRID;
  int bufferDurationMode = TemporalDeck::BUFFER_DURATION_8S;
  int slipReturnMode = TemporalDeck::SLIP_RETURN_NORMAL;
};

TemporalDeck::TemporalDeck() : impl(new Impl()) {
  config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
  configParam(BUFFER_PARAM, 0.f, 1.f, 1.f, "Buffer", " s", 0.f, 8.f);
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
    paramQuantities[BUFFER_PARAM]->displayMultiplier = usableBufferSecondsForMode(impl->bufferDurationMode);
  }
  onSampleRateChange();
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

const char *TemporalDeck::scratchModelLabelFor(int index) {
  switch (index) {
  case SCRATCH_MODEL_SCRATCH3:
    return "Scratch3";
  case SCRATCH_MODEL_HYBRID:
    return "Hybrid";
  case SCRATCH_MODEL_LEGACY:
  default:
    return "Legacy";
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
  case BUFFER_DURATION_16S:
    return "16 s";
  case BUFFER_DURATION_8M:
    return "8 min";
  case BUFFER_DURATION_8S:
  default:
    return "8 s";
  }
}

float TemporalDeck::scratchSensitivity() {
  return ScratchSensitivityQuantity::sensitivityForValue(params[SCRATCH_SENSITIVITY_PARAM].getValue());
}

void TemporalDeck::applyBufferDurationMode(int mode) {
  impl->bufferDurationMode = clamp(mode, 0, BUFFER_DURATION_COUNT - 1);
  impl->engine.bufferDurationMode = impl->bufferDurationMode;
  if (paramQuantities[BUFFER_PARAM]) {
    paramQuantities[BUFFER_PARAM]->displayMultiplier = usableBufferSecondsForMode(impl->bufferDurationMode);
  }
  onSampleRateChange();
}

void TemporalDeck::applySampleRateChange(float sampleRate) {
  impl->cachedSampleRate = sampleRate;
  impl->engine.bufferDurationMode = impl->bufferDurationMode;
  impl->engine.reset(impl->cachedSampleRate);
  impl->uiSampleRate.store(impl->cachedSampleRate);
  impl->uiLagSamples.store(0.0);
  impl->uiAccessibleLagSamples.store(0.0);
  impl->uiPlatterAngle.store(0.f);
  impl->uiPublishTimerSec = 0.f;
  impl->platterScratchHoldSamples.store(0);
  impl->platterMotionFreshSamples.store(0);
  for (int i = 0; i < kArcLightCount; ++i) {
    lights[ARC_LIGHT_START + i].setBrightness(0.f);
    lights[ARC_MAX_LIGHT_START + i].setBrightness(0.f);
  }
}

void TemporalDeck::onSampleRateChange() {
  applySampleRateChange(APP->engine->getSampleRate());
}

json_t *TemporalDeck::dataToJson() {
  json_t *root = json_object();
  json_object_set_new(root, "freezeLatched", json_boolean(impl->freezeLatched));
  json_object_set_new(root, "reverseLatched", json_boolean(impl->reverseLatched));
  json_object_set_new(root, "slipLatched", json_boolean(impl->slipLatched));
  json_object_set_new(root, "scratchInterpolationMode", json_integer(impl->scratchInterpolationMode));
  json_object_set_new(root, "platterCursorLock", json_boolean(impl->platterCursorLock));
  json_object_set_new(root, "slipReturnMode", json_integer(impl->slipReturnMode));
  json_object_set_new(root, "cartridgeCharacterV2", json_integer(impl->cartridgeCharacter));
  int legacyCartridgeCharacter = impl->cartridgeCharacter;
  if (legacyCartridgeCharacter == CARTRIDGE_QBERT) {
    legacyCartridgeCharacter = CARTRIDGE_ORTOFON_SCRATCH;
  } else if (legacyCartridgeCharacter == CARTRIDGE_LOFI) {
    legacyCartridgeCharacter = 4;
  }
  json_object_set_new(root, "cartridgeCharacter", json_integer(legacyCartridgeCharacter));
  json_object_set_new(root, "scratchModel", json_integer(impl->scratchModel));
  json_object_set_new(root, "bufferDurationMode", json_integer(impl->bufferDurationMode));
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
  json_t *slipReturnModeJ = json_object_get(root, "slipReturnMode");
  json_t *cartridgeV2J = json_object_get(root, "cartridgeCharacterV2");
  json_t *cartridgeJ = json_object_get(root, "cartridgeCharacter");
  json_t *scratchModelJ = json_object_get(root, "scratchModel");
  json_t *bufferDurationJ = json_object_get(root, "bufferDurationMode");
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
  if (scratchModelJ) {
    impl->scratchModel = clamp((int)json_integer_value(scratchModelJ), 0, SCRATCH_MODEL_COUNT - 1);
  }
  if (bufferDurationJ) {
    impl->bufferDurationMode = clamp((int)json_integer_value(bufferDurationJ), 0, BUFFER_DURATION_COUNT - 1);
  }
  impl->engine.bufferDurationMode = impl->bufferDurationMode;
  if (paramQuantities[BUFFER_PARAM]) {
    paramQuantities[BUFFER_PARAM]->displayMultiplier = usableBufferSecondsForMode(impl->bufferDurationMode);
  }
}

void TemporalDeck::process(const ProcessArgs &args) {
  if (args.sampleRate != impl->cachedSampleRate) {
    applySampleRateChange(args.sampleRate);
  }

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

  impl->engine.scratchInterpolationMode = impl->scratchInterpolationMode;
  impl->engine.slipReturnMode = impl->slipReturnMode;
  impl->engine.cartridgeCharacter = impl->cartridgeCharacter;
  impl->engine.scratchModel = impl->scratchModel;
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
                       impl->reverseLatched, impl->slipLatched,
                       inputs[FREEZE_GATE_INPUT].getVoltage() >= TemporalDeckEngine::kFreezeGateThreshold,
                       inputs[SCRATCH_GATE_INPUT].getVoltage() >= TemporalDeckEngine::kScratchGateThreshold,
                       inputs[SCRATCH_GATE_INPUT].isConnected(), inputs[POSITION_CV_INPUT].isConnected(), positionCv,
                       rateCv, impl->platterTouched.load(), wheelScratchHeld, platterMotionActive,
                       impl->platterGestureRevision.load(), impl->platterLagTarget.load(),
                       impl->platterGestureVelocity.load(), wheelDelta);

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

  impl->uiPublishTimerSec += args.sampleTime;
  if (impl->uiPublishTimerSec >= kUiPublishIntervalSec) {
    impl->uiPublishTimerSec = std::fmod(impl->uiPublishTimerSec, kUiPublishIntervalSec);
    float maxLag = std::max(1.f, args.sampleRate * usableBufferSecondsForMode(impl->bufferDurationMode));
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
      bool isLimitLed = limitRatio > 0.f && std::fabs(float(i) - limitLed) < 0.5f;
      lights[ARC_MAX_LIGHT_START + i].setBrightness(isLimitLed ? 1.f : 0.f);
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

bool TemporalDeck::isSlipLatched() const {
  return impl->slipLatched;
}

int TemporalDeck::getCartridgeCharacter() const {
  return impl->cartridgeCharacter;
}

int TemporalDeck::getScratchModel() const {
  return impl->scratchModel;
}

void TemporalDeck::setScratchModel(int mode) {
  impl->scratchModel = clamp(mode, 0, SCRATCH_MODEL_COUNT - 1);
}

int TemporalDeck::getBufferDurationMode() const {
  return impl->bufferDurationMode;
}

bool TemporalDeck::isPlatterCursorLockEnabled() const {
  return impl->platterCursorLock;
}

void TemporalDeck::setPlatterCursorLockEnabled(bool enabled) {
  impl->platterCursorLock = enabled;
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

int TemporalDeck::getSlipReturnMode() const {
  return impl->slipReturnMode;
}

void TemporalDeck::setSlipReturnMode(int mode) {
  impl->slipReturnMode = clamp(mode, SLIP_RETURN_SLOW, SLIP_RETURN_COUNT - 1);
}
