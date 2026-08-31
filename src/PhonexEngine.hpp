#pragma once

#include "PhonexTypes.hpp"

#include <array>
#include <cstdint>

namespace phonex {

constexpr std::array<float, 16> kChirp {{
	0.00f, 0.38f, 0.82f, 1.00f,
	0.68f, 0.32f, 0.04f, -0.22f,
	-0.40f, -0.34f, -0.26f, -0.18f,
	-0.11f, -0.06f, -0.02f, 0.00f,
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
	float process(float excitation, const std::array<float, kLpcOrder>& reflection);
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

enum class ForcedExcitation : std::uint8_t {
	Voiced = 0,
	Unvoiced,
};

struct EngineControls {
	float hostSampleRate = 48000.f;
	float speed = 1.f;
	float pitchOctaves = 0.f;
	float voct = 0.f;
	float voctAttenuverter = 1.f;
	float exciteBlend = 0.f;
	ForcedExcitation forcedExcitation = ForcedExcitation::Voiced;
	float externalExcitation = 0.f;
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
	void retrigger(float speed);
	EngineOutput process(const EngineControls& controls);

	float position() const { return position_; }
	std::uint16_t frameIndex() const { return observedFrame_; }
	std::uint64_t internalTickCount() const { return internalTicks_; }
	LpcFrame currentFrame() const { return interpolatedFrame(); }

private:
	LpcFrame interpolatedFrame() const;
	void updateTransport(const EngineControls& controls, bool triggerRise);
	float synthesizeTick(const LpcFrame& frame, const EngineControls& controls);
	void clearSynthesis();

	const LpcSequence* sequence_ = nullptr;
	TriggerMode triggerMode_ = TriggerMode::RetriggerPhrase;
	ReconstructionMode reconstructionMode_ = ReconstructionMode::RawHold;
	LatticeFilter lattice_;
	ChirpGenerator chirp_;
	NoiseGenerator noise_;
	float position_ = 0.f;
	float internalRate_ = 10000.f;
	double internalPhase_ = 0.0;
	float heldSample_ = 0.f;
	float filteredSample_ = 0.f;
	std::uint16_t observedFrame_ = 0;
	std::uint64_t internalTicks_ = 0;
	bool triggerHigh_ = false;
	bool wordPushHigh_ = false;
	bool eoxArmed_ = true;
	bool frameChanged_ = false;
	bool eoxEvent_ = false;
	std::uint32_t framePulseRemaining_ = 0;
	std::uint32_t eoxPulseRemaining_ = 0;
};

} // namespace phonex
