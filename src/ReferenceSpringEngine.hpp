#pragma once

#include "DoorstopEngine.hpp"

#include <array>
#include <cstdint>

namespace doorstop {

constexpr int REFERENCE_MODE_COUNT = 8;

struct ReferenceDiagnostics {
	std::uint64_t crossingCount = 0;
	float radiationGate = 0.f;
	float normalizedModalEnergy = 0.f;
};

class ReferenceSpringEngine {
public:
	ReferenceSpringEngine();

	void reset();
	void resetMotion();
	void restoreFactoryFresh();
	void setSampleRate(float newSampleRate);
	void setBreakIn(float amount);
	void setBreakInLocked(bool locked);
	void setSpecimenSeed(std::uint32_t seed);
	void strike(float normalizedVelocity);
	Frame process(float requestedSampleTime);

	bool isSleeping() const { return sleeping; }
	float getBreakIn() const { return breakIn; }
	bool isBreakInLocked() const { return breakInLocked; }
	std::uint32_t getSpecimenSeed() const { return specimenSeed; }
	float getMaximumDisplacement() const { return maximumDisplacement; }
	const ReferenceDiagnostics& getDiagnostics() const { return diagnostics; }

private:
	static constexpr float PI = 3.14159265358979323846f;
	static constexpr float LN_1000 = 6.907755278982137f;

	struct Mode {
		float position = 0.f;
		float velocity = 0.f;
	};

	enum class ArmedSide : std::int8_t {
		None = 0,
		Positive = 1,
		Negative = -1
	};

	float sampleRate = 44100.f;
	float sampleTime = 1.f / 44100.f;
	float breakIn = 0.f;
	bool breakInLocked = false;
	std::uint32_t specimenSeed = 1u;
	std::uint32_t noiseState = 1u;

	float baseFrequencyHz = 16.f;
	float dampingRatio = 0.020f;
	float nonlinearStiffness = 1.4f;
	float maximumDisplacement = 2.f;
	float baseOmega = 0.f;
	float baseOmegaSq = 0.f;
	float springDamping = 0.f;
	float maximumVelocity = 1.f;
	float energyCeiling = 1.f;
	float radiationCurvature = 0.5f;
	float radiationFloor = 0.3f;
	float radiationAsymmetry = 0.f;

	float displacement = 0.f;
	float springVelocity = 0.f;
	float acceleration = 0.f;
	float previousDisplacement = 0.f;
	ArmedSide armedSide = ArmedSide::None;
	int crossingRefractorySamples = 0;
	int strikeRefractorySamples = 0;
	float lastDirection = 1.f;

	std::array<Mode, REFERENCE_MODE_COUNT> modes {};
	std::array<float, REFERENCE_MODE_COUNT> restingFrequencyHz {};
	std::array<float, REFERENCE_MODE_COUNT> decayT60Seconds {};
	std::array<float, REFERENCE_MODE_COUNT> impactExcitation {};
	std::array<float, REFERENCE_MODE_COUNT> crossingExcitation {};
	std::array<float, REFERENCE_MODE_COUNT> outputGain {};
	std::array<float, REFERENCE_MODE_COUNT> directionTilt {};
	std::array<float, REFERENCE_MODE_COUNT> frequencyWarpDepth {};
	std::array<float, REFERENCE_MODE_COUNT> modeVelocityLimit {};

	float radiationEnvelope = 0.f;
	float smoothedEnergy = 0.f;
	float impactEnvelope = 0.f;
	float impactBrightness = 0.f;
	float impactLowpass = 0.f;
	float impactLowReject = 0.f;
	float mountPosition = 0.f;
	float mountVelocity = 0.f;
	float strikeLightEnvelope = 0.f;
	float dcPreviousInput = 0.f;
	float dcPreviousOutput = 0.f;
	float quietTime = 0.f;
	bool sleeping = true;

	float radiationAttack = 0.f;
	float radiationRelease = 0.f;
	float energySmoothing = 0.f;
	float impactSoftDecay = 0.f;
	float impactHardDecay = 0.f;
	float impactBrightnessDecay = 0.f;
	float impactSoftAlpha = 0.f;
	float impactHardAlpha = 0.f;
	float impactRejectAlpha = 0.f;
	float mountOmegaSq = 0.f;
	float mountGamma = 0.f;
	float strikeLightDecay = 0.f;
	float dcPole = 0.f;

	ReferenceDiagnostics diagnostics;

	void updateCoefficients();
	void updateWearAndSpecimen();
	void clearDynamicState();
	void recoverFromNonFinite();
	float springPotential(float x) const;
	float normalizedFlexEnergy() const;
	float normalizedModalEnergy() const;
	float specimenUnit(std::uint32_t propertyTag) const;
	std::uint32_t nextNoiseRandom();
	bool processFlexAndCrossing();
	void exciteCrossing(float normalizedSpeed);
	float processModes();
	float processImpact();
	float processMount();
	float processFlexAudio();
	float processDcBlocker(float input);
	bool allFinite() const;
	bool belowSleepThreshold(float outputVolts) const;
};

} // namespace doorstop
