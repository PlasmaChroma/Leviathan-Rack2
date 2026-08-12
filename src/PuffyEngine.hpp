#pragma once

#include <dsp/resampler.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace puffy {

constexpr float kReferenceVolts = 5.f;
constexpr int kOversampleFactor = 4;
constexpr int kOversampleQuality = 8;

enum class LimiterMode {
	Hard = 0,
	Soft = 1,
	Off = 2
};

enum class Character {
	Bloom = 0,
	Spine = 1,
	Frenzy = 2,
	Riptide = 3,
	Void = 4,
	Swarm = 5,
	Teeth = 6
};

constexpr int kCharacterCount = int(Character::Teeth) + 1;

struct DynamicsState {
	float fast = 0.f;
	float slowSq = 0.f;
	float transient = 0.f;
};

struct Frame {
	float left = 0.f;
	float right = 0.f;
	float effectiveAmount = 0.f;
	float wetMix = 1.f;
	float inputActivity = 0.f;
	float positiveInputActivity = 0.f;
	float negativeInputActivity = 0.f;
	float leftPositiveInputActivity = 0.f;
	float leftNegativeInputActivity = 0.f;
	float rightPositiveInputActivity = 0.f;
	float rightNegativeInputActivity = 0.f;
	float transientActivity = 0.f;
	float limiterGain = 1.f;
	int negativeCharacter = 0;
	int positiveCharacter = 0;
};

class Engine {
public:
	Engine();

	void setSampleRate(float sampleRate);
	void setSwarmSeed(std::uint32_t seed);
	void setLimiterMode(LimiterMode mode);
	void reset();
	Frame process(
		float inputLeft,
		float inputRight,
		float amountTarget,
		int negativeCharacter,
		int positiveCharacter,
		bool autoDeflate,
		float sensitivity,
		float wetTarget = 1.f,
		bool trackStereoActivity = false);
	Frame process(
		float inputLeft,
		float inputRight,
		float amountTarget,
		int character,
		bool autoDeflate,
		float sensitivity,
		float wetTarget = 1.f,
		bool trackStereoActivity = false) {
		return process(
			inputLeft, inputRight, amountTarget, character, character,
			autoDeflate, sensitivity, wetTarget, trackStereoActivity);
	}

	float getSampleRate() const {
		return sampleRate;
	}

	static float processCharacter(
		Character character,
		float input,
		float amount,
		const DynamicsState& dynamics,
		float swarmChaos = 0.f);

private:
	struct CharacterCoefficients {
		Character character = Character::Bloom;
		float amount = 0.f;
		float drive = 1.f;
		float foldPhaseCycles = 0.5f;
		float foldGain = 1.f;
		float phaseSkew = 0.f;
		float positivePolarityBias = 0.f;
		float negativePolarityBias = 0.f;
		float polarityEdgeBias = 0.f;
		float normalization = 1.f;
		float voidStepSize = 1.f;
		float voidInverseStepSize = 1.f;
		float voidStepMix = 0.f;
		float voidOutputGain = 1.f;
		float voidRailThreshold = 1.f;
		float swarmDrive = 1.f;
		float swarmScatter = 0.f;
		float swarmRailAttraction = 0.f;
		float swarmFastMix = 0.f;
		float swarmOutputGain = 1.f;
	};

	struct SwarmFrame {
		float lanes[kOversampleFactor] {};
	};

	struct DcBlocker {
		float x1 = 0.f;
		float y1 = 0.f;
		float coefficient = 0.f;

		void reset();
		float process(float input);
	};

	struct PathState {
		rack::dsp::Decimator<kOversampleFactor, kOversampleQuality> decimatorLeft;
		rack::dsp::Decimator<kOversampleFactor, kOversampleQuality> decimatorRight;
		DcBlocker dcLeft;
		DcBlocker dcRight;

		void reset();
	};

	float sampleRate = 48000.f;
	float amount = 0.f;
	float wetMix = 1.f;
	float amountCoefficient = 1.f;
	float autoDeflateCoefficient = 1.f;
	float autoDeflateEnergyCoefficient = 1.f;
	float autoDeflateAttackCoefficient = 1.f;
	float autoDeflateReleaseCoefficient = 1.f;
	float autoDeflateMix = 1.f;
	float autoDeflateInputSq = 0.f;
	float autoDeflateOutputSq = 0.f;
	float autoDeflateGain = 1.f;
	float autoDeflateTargetGain = 1.f;
	int autoDeflateControlCounter = 0;
	bool autoDeflateStateInitialized = false;
	float detectorAttackCoefficient = 1.f;
	float detectorReleaseCoefficient = 1.f;
	float detectorSlowCoefficient = 1.f;
	float activityAttackCoefficient = 1.f;
	float activityReleaseCoefficient = 1.f;
	float limiterReleaseCoefficient = 1.f;
	float dcCoefficient = 0.f;
	float inputActivity = 0.f;
	float positiveInputActivity = 0.f;
	float negativeInputActivity = 0.f;
	float leftPositiveInputActivity = 0.f;
	float leftNegativeInputActivity = 0.f;
	float rightPositiveInputActivity = 0.f;
	float rightNegativeInputActivity = 0.f;
	float limiterGain = 1.f;
	float hardLimiterGain = 1.f;
	float softLimiterGain = 1.f;
	LimiterMode requestedLimiterMode = LimiterMode::Hard;
	LimiterMode currentLimiterMode = LimiterMode::Hard;
	LimiterMode limiterTransitionFrom = LimiterMode::Hard;
	LimiterMode limiterTransitionTo = LimiterMode::Hard;
	LimiterMode pendingLimiterMode = LimiterMode::Hard;
	int limiterTransitionSample = 0;
	int limiterTransitionLength = 1;
	bool limiterTransitionActive = false;
	bool pendingLimiterModeActive = false;
	float projectedInputGain = 1.f;
	float cachedSensitivity = -2.f;
	float cachedSensitivityTargetGain = 1.f;
	DynamicsState dynamics;
	std::uint32_t swarmInitialSeed = 0x6d2b79f5u;
	std::uint32_t swarmRngState = 0x6d2b79f5u;
	float swarmPreviousFast = 0.f;
	float swarmCurrentFast = 0.f;
	float swarmSlow = 0.f;
	float swarmSlowCoefficient = 1.f;

	rack::dsp::Upsampler<kOversampleFactor, kOversampleQuality> upsamplerLeft;
	rack::dsp::Upsampler<kOversampleFactor, kOversampleQuality> upsamplerRight;
	PathState primaryPath;
	PathState secondaryPath;

	struct CharacterPair {
		Character negative = Character::Bloom;
		Character positive = Character::Bloom;
		CharacterPair() = default;
		CharacterPair(Character negative, Character positive)
			: negative(negative), positive(positive) {
		}

		bool operator==(const CharacterPair& other) const {
			return negative == other.negative && positive == other.positive;
		}
	};

	CharacterPair currentCharacters;
	CharacterPair transitionFrom;
	CharacterPair transitionTo;
	CharacterPair pendingCharacters;
	int transitionSample = 0;
	int transitionLength = 1;
	bool transitionActive = false;
	bool pendingCharacterActive = false;

	void resetSharedControlState();
	void resetChannel(bool left);
	void beginLimiterTransition(LimiterMode requested);
	float updateLimiterBranchGain(
		LimiterMode mode,
		float peak,
		float currentGain) const;
	float limiterBranchGain(LimiterMode mode) const;
	void beginCharacterTransition(CharacterPair requested);
	static CharacterCoefficients prepareCharacter(
		Character character,
		float amount,
		const DynamicsState& dynamics);
	static float applyCharacter(
		float input,
		const CharacterCoefficients& coefficients,
		float swarmChaos);
	SwarmFrame prepareSwarmFrame(float fastMix);
	float updateFollower(float current, float target, float attack, float release) const;
	float sensitivityTargetGain(float bipolarSensitivity);
	float processPath(
		PathState& path,
		const CharacterCoefficients& negativeCoefficients,
		const CharacterCoefficients& positiveCoefficients,
		float* oversampledLeft,
		float* oversampledRight,
		float wetAmount,
		const SwarmFrame& swarmFrame,
		bool left);
};

} // namespace puffy
