#pragma once

#include "DoorstopEngine.hpp"

#include <array>
#include <cstdint>

namespace doorstop {

// Stage-1 Reference V3 surrogate. Every retained family is represented as a
// split lateral pair so the audible body and the visible fundamental share one
// two-plane mechanical state. Contact and moving radiation geometry are later
// stages; this engine deliberately contains no global amplitude gate.
class HelicalContinuumEngine {
public:
	static constexpr int PAIR_COUNT = 7;
	static constexpr int MODE_COUNT = 2 * PAIR_COUNT;

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
	float getVisualMaximumDisplacement() const { return 2.f; }

private:
	static constexpr float PI = 3.14159265358979323846f;
	static constexpr int SUBSTEPS = 2;

	struct Vec2 {
		float x = 0.f;
		float y = 0.f;
	};

	struct ModalState {
		float position = 0.f;
		float velocity = 0.f;
		float acceleration = 0.f;
	};

	struct StrikePulse {
		float magnitude = 0.f;
		float directionX = 1.f;
		float directionY = 0.f;
		float cosine = 1.f;
		float sine = 0.f;
		float cosineStep = 1.f;
		float sineStep = 0.f;
		int remainingSubsteps = 0;
	};

	float sampleRate = 44100.f;
	float sampleTime = 1.f / 44100.f;
	float breakIn = 0.f;
	bool breakInLocked = false;
	std::uint32_t specimenSeed = 1u;

	std::array<ModalState, MODE_COUNT> modes {};
	std::array<float, MODE_COUNT> omegaSq {};
	std::array<float, MODE_COUNT> damping {};
	std::array<float, MODE_COUNT> tipParticipation {};
	std::array<float, MODE_COUNT> radiationWeight {};
	std::array<float, MODE_COUNT> strainWeight {};

	Vec2 capPosition {};
	Vec2 capVelocity {};
	StrikePulse strikePulse {};
	float stiffnessScale = 1.f;
	int controlCounter = 0;
	float dcPreviousInput = 0.f;
	float dcPreviousOutput = 0.f;
	float dcPole = 0.f;
	float strikeLight = 0.f;
	float strikeLightDecay = 0.f;
	float visualAudibleEnvelope = 0.f;
	float visualEnvelopeDecay = 0.f;
	float quietTime = 0.f;
	bool sleeping = true;

	void updateCoefficients();
	void updateSpecimenCoefficients();
	void clearDynamicState();
	void recoverFromNonFinite();
	float specimenUnit(std::uint32_t tag) const;
	float calculateEnergy() const;
	float processSubstep(float h);
	float processDcBlocker(float input);
	bool allFinite() const;
};

} // namespace doorstop
