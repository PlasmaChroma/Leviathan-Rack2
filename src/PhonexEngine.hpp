#pragma once

#include "PhonexTypes.hpp"

#include <algorithm>
#include <array>
#include <cstdint>

namespace phonex {

// Signed TMS5100 chirp shape, level-matched to the original PHONEX carrier.
// This is chip configuration data, not speech-ROM content. The signed values
// are the public TMS5100 table scaled by 1/128 rather than the hardware's
// fixed-point gain so the existing PHONEX corpus retains its useful level.
constexpr std::array<float, 41> kChirp {{
	 0.f / 128.f,  42.f / 128.f, -44.f / 128.f,  50.f / 128.f,
	-78.f / 128.f,  18.f / 128.f,  37.f / 128.f,  20.f / 128.f,
	 2.f / 128.f, -31.f / 128.f, -59.f / 128.f,   2.f / 128.f,
	95.f / 128.f,  90.f / 128.f,   5.f / 128.f,  15.f / 128.f,
	38.f / 128.f,  -4.f / 128.f, -91.f / 128.f, -91.f / 128.f,
	-42.f / 128.f, -35.f / 128.f, -36.f / 128.f,  -4.f / 128.f,
	 37.f / 128.f,  43.f / 128.f,  34.f / 128.f,  33.f / 128.f,
	 15.f / 128.f,  -1.f / 128.f,  -8.f / 128.f, -18.f / 128.f,
	-19.f / 128.f, -17.f / 128.f,  -9.f / 128.f, -10.f / 128.f,
	 -6.f / 128.f,   0.f / 128.f,   3.f / 128.f,   2.f / 128.f,
	  1.f / 128.f,
}};

constexpr std::uint32_t kLfsrMask = 0x1ffffu;
constexpr std::uint32_t kLfsrTopBit = 0x10000u;
constexpr std::uint32_t kLfsrXorMask = 0x12000u;
constexpr std::uint32_t kLfsrReset = 0x1ace1u;

class ChirpGenerator {
public:
	void reset();
	float next(float pitchPeriodTicks);
	float phase() const { return periodPhase_; }

private:
	float periodPhase_ = 0.f;
};

class NoiseGenerator {
public:
	void reset();
	float next();
	std::uint32_t state() const { return state_; }

private:
	std::uint32_t state_ = kLfsrReset;
};

class LatticeFilter {
public:
	void reset();
	void leak(float multiplier);
	float process(float excitation, const std::array<float, kLpcOrder>& reflection,
		float coefficientLimit = 0.995f);
	const std::array<float, kLpcOrder>& state() const { return state_; }

private:
	std::array<float, kLpcOrder> state_{};
};

enum class TriggerMode : std::uint8_t {
	RetriggerPhrase = 0,
	AdvanceOneFrame,
};

enum class ReconstructionMode : std::uint8_t {
	RawHold = 0,
	Filtered,
};

enum class OutputStage : std::uint8_t {
	LegacyCurve = 0,
	CalibratedLinear,
	CalibratedLimited,
};

enum class ReconstructionOrder : std::uint8_t {
	TwoPole = 1,
	FourPole = 2,
	SixPole = 3,
};

enum class ForcedExcitation : std::uint8_t {
	Voiced = 0,
	Unvoiced,
};

struct EngineControls {
	float hostSampleRate = 48000.f;
	float speed = 1.f;
	float pitchOctaves = 0.f;
	float voct = 0.f;
	float formant = 0.f;
	float exciteBlend = 0.f;
	ForcedExcitation forcedExcitation = ForcedExcitation::Voiced;
	float externalExcitation = 0.f;
	float warp = 0.f;
	float warpCv = 0.f;
	float bend = 0.f;
	float bendCv = 0.f;
	std::uint8_t glitchLevel = 0;
	float scrubVoltage = 0.f;
	bool scrubConnected = false;
	bool externalConnected = false;
	bool triggerGate = false;
	bool wordPush = false;
};

struct EngineOutput {
	float audio = 0.f;
	float position = 0.f;
	std::uint16_t frameIndex = 0;
	bool framePulse = false;
	bool eoxPulse = false;
	bool voiced = false;
};

class Engine {
public:
	void setSequence(const LpcSequence* sequence);
	void setInternalRate(float rateHz);
	void setTriggerMode(TriggerMode mode) { triggerMode_ = mode; }
	void setReconstructionMode(ReconstructionMode mode) { reconstructionMode_ = mode; }
	void setOutputStage(OutputStage stage) { outputStage_ = stage; }
	void setReconstructionOrder(ReconstructionOrder order) {
		reconstructionOrder_ = order;
		reconstructionHostRate_ = 0.f;
	}
	void setSeed(std::uint32_t seed);
	void retrigger(float speed);
	EngineOutput process(const EngineControls& controls);

	float position() const {
		return sequence_ && sequence_->frameCount > 0
			? std::min(position_, static_cast<float>(sequence_->frameCount - 1))
			: 0.f;
	}
	std::uint16_t frameIndex() const { return observedFrame_; }
	std::uint64_t internalTickCount() const { return internalTicks_; }
	LpcFrame currentFrame() const { return interpolatedFrame(); }

private:
	LpcFrame interpolatedFrame() const;
	void updateTransport(const EngineControls& controls, bool triggerRise);
	float synthesizeTick(const LpcFrame& frame, const EngineControls& controls,
		float bend, float skipProbability, float leakAmount, float overdrive);
	const std::array<float, kLpcOrder>& warpedReflection(
		const LpcFrame& frame, float formant, float warp, float overdrive);
	void clearSynthesis();
	float reconstructFiltered(float input, float hostSampleRate);

	const LpcSequence* sequence_ = nullptr;
	TriggerMode triggerMode_ = TriggerMode::RetriggerPhrase;
	ReconstructionMode reconstructionMode_ = ReconstructionMode::RawHold;
	OutputStage outputStage_ = OutputStage::CalibratedLimited;
	ReconstructionOrder reconstructionOrder_ = ReconstructionOrder::SixPole;
	LatticeFilter lattice_;
	ChirpGenerator chirp_;
	NoiseGenerator noise_;
	float position_ = 0.f;
	float internalRate_ = 10000.f;
	double internalPhase_ = 0.0;
	float heldSample_ = 0.f;
	std::array<float, 3> reconstructionZ1_{};
	std::array<float, 3> reconstructionZ2_{};
	std::array<float, 3> reconstructionB0_{{1.f, 1.f, 1.f}};
	std::array<float, 3> reconstructionB1_{};
	std::array<float, 3> reconstructionB2_{};
	std::array<float, 3> reconstructionA1_{};
	std::array<float, 3> reconstructionA2_{};
	float reconstructionHostRate_ = 0.f;
	float reconstructionInternalRate_ = 0.f;
	float smoothedFormant_ = 0.f;
	float smoothedWarp_ = 0.f;
	float lastVoicedPitchPeriod_ = 0.f;
	float jitterScale_ = 1.f;
	std::array<float, kLpcOrder> warpedReflection_{};
	std::array<float, kLpcOrder> cachedReflection_{};
	float cachedFormant_ = 0.f;
	float cachedWarp_ = 0.f;
	float cachedOverdrive_ = 1.f;
	bool warpSmootherReady_ = false;
	bool warpedReflectionValid_ = false;
	std::uint32_t seed_ = 0x6d2b79f5u;
	std::uint32_t bendState_ = 0x6d2b79f5u;
	std::uint8_t activeGlitchLevel_ = 0;
	std::uint16_t observedFrame_ = 0;
	std::uint64_t internalTicks_ = 0;
	bool triggerHigh_ = false;
	bool wordPushHigh_ = false;
	bool eoxArmed_ = true;
	bool playbackComplete_ = false;
	bool frameChanged_ = false;
	bool eoxEvent_ = false;
	std::uint32_t framePulseRemaining_ = 0;
	std::uint32_t eoxPulseRemaining_ = 0;
};

std::uint32_t xorshift32(std::uint32_t& state);
std::uint32_t phonexFrameHash(std::uint32_t seed, std::uint32_t frameIndex,
	std::uint32_t level);
LpcFrame selectGlitchedFrame(const LpcSequence& sequence, std::uint16_t frameIndex,
	std::uint8_t level, std::uint32_t seed);
std::array<float, kLpcOrder> formantShiftReflection(
	const std::array<float, kLpcOrder>& reflection, float amount);
std::array<float, kLpcOrder> warpReflectionCoefficients(
	const std::array<float, kLpcOrder>& reflection, float amount);
float tms5100InterpolationMix(float frameFraction);
float applyOutputStage(float reconstructed, OutputStage stage);

} // namespace phonex
