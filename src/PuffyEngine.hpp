#pragma once

#include <dsp/resampler.hpp>

#include <algorithm>
#include <cmath>

namespace puffy {

constexpr float kReferenceVolts = 5.f;
constexpr int kOversampleFactor = 4;
constexpr int kOversampleQuality = 8;

enum class Character {
	Bloom = 0,
	Spine = 1,
	Frenzy = 2,
	Riptide = 3
};

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
	float transientActivity = 0.f;
	float limiterGain = 1.f;
	int character = 0;
};

class Engine {
public:
	Engine();

	void setSampleRate(float sampleRate);
	void reset();
	Frame process(
		float inputLeft,
		float inputRight,
		float amountTarget,
		int character,
		bool autoDeflate,
		float manualDeflate,
		float wetTarget = 1.f);

	float getSampleRate() const {
		return sampleRate;
	}

	static float processCharacter(
		Character character,
		float input,
		float amount,
		const DynamicsState& dynamics);

private:
	struct CharacterCoefficients {
		Character character = Character::Bloom;
		float amount = 0.f;
		float drive = 1.f;
		float foldCycles = 1.f;
		float foldGain = 1.f;
		float phaseSkew = 0.f;
		float normalization = 1.f;
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
	float autoDeflateMix = 1.f;
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
	float limiterGain = 1.f;
	float cachedManualDeflate = -1.f;
	float cachedManualGain = 1.f;
	DynamicsState dynamics;

	rack::dsp::Upsampler<kOversampleFactor, kOversampleQuality> upsamplerLeft;
	rack::dsp::Upsampler<kOversampleFactor, kOversampleQuality> upsamplerRight;
	PathState primaryPath;
	PathState secondaryPath;

	Character currentCharacter = Character::Bloom;
	Character transitionFrom = Character::Bloom;
	Character transitionTo = Character::Bloom;
	Character pendingCharacter = Character::Bloom;
	int transitionSample = 0;
	int transitionLength = 1;
	bool transitionActive = false;
	bool pendingCharacterActive = false;

	void resetSharedControlState();
	void resetChannel(bool left);
	void beginCharacterTransition(Character requested);
	static CharacterCoefficients prepareCharacter(
		Character character,
		float amount,
		const DynamicsState& dynamics);
	static float applyCharacter(float input, const CharacterCoefficients& coefficients);
	float updateFollower(float current, float target, float attack, float release) const;
	float updateAutoGain(Character character, float currentAmount) const;
	float manualGain(float normalizedDeflate);
	float processPath(
		PathState& path,
		const CharacterCoefficients& coefficients,
		float* oversampledLeft,
		float* oversampledRight,
		float autoDeflateAmount,
		float wetAmount,
		bool left);
};

} // namespace puffy
