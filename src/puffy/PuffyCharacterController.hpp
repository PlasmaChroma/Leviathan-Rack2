#pragma once

#include "Puffy.hpp"
#include "PuffyPose.hpp"

#include <cstdint>

class PuffyCharacterController {
public:
	explicit PuffyCharacterController(std::uint32_t personalitySeed = 0x6d2b79f5u);

	void setPersonalitySeed(
		std::uint32_t personalitySeed,
		const PuffyVisualState& visual);
	void reset(const PuffyVisualState& visual);
	bool update(float dt, const PuffyVisualState& visual, PuffyPose* pose);

private:
	float inflation = 0.f;
	float inflationVelocity = 0.f;
	float negativeCharacterTintWeights[puffy::kCharacterCount] = {1.f};
	float positiveCharacterTintWeights[puffy::kCharacterCount] = {1.f};
	float transientMemory = 0.f;
	float twitchPhase = -1.f;
	float twitchStrength = 0.f;
	float twitchCooldown = 0.f;
	float energyBaseline = 0.f;
	float excitement = 0.f;
	float excitementHold = 0.f;
	float excitementCooldown = 0.f;
	float excitementPhase = 0.f;
	float finFlutterPhase = 0.f;
	float breathPhase = 0.f;
	float bobPhase = 0.f;
	float movementFinActivity = 0.f;
	bool energySurgeArmed = true;
	float idleTime = 0.f;
	float nextBlinkTime = 3.2f;
	float blinkPhase = -1.f;
	float nextSquintTime = 5.4f;
	float squintPhase = -1.f;
	float nextMouthCloseTime = 7.6f;
	float mouthClosePhase = -1.f;
	float polarityDominance = 0.f;
	float gazeX = 0.f;
	float gazeTargetX = 0.f;
	float gazeStateTime = 1.2f;
	bool gazeGlancing = false;
	int gazeSequence = 0;
	std::uint32_t personalitySeed = 0x6d2b79f5u;
	std::uint32_t blinkRng = 1u;
	std::uint32_t squintRng = 1u;
	std::uint32_t mouthRng = 1u;
	std::uint32_t gazeRng = 1u;
	std::uint32_t motionRng = 1u;

	static float approach(float current, float target, float rate, float dt);
	static float clamp01(float value);
	static std::uint32_t mixSeed(std::uint32_t value);
	static float nextRandom01(std::uint32_t* state);
};
