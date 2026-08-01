#pragma once

#include "Puffy.hpp"
#include "PuffyPose.hpp"

class PuffyCharacterController {
public:
	PuffyCharacterController();

	void reset(const PuffyVisualState& visual);
	bool update(float dt, const PuffyVisualState& visual, PuffyPose* pose);

private:
	float inflation = 0.f;
	float inflationVelocity = 0.f;
	float personality = 0.f;
	float negativeCharacterTintWeights[puffy::kCharacterCount] = {
		1.f, 0.f, 0.f, 0.f, 0.f};
	float positiveCharacterTintWeights[puffy::kCharacterCount] = {
		1.f, 0.f, 0.f, 0.f, 0.f};
	float transientMemory = 0.f;
	float idleTime = 0.f;
	float nextBlinkTime = 3.2f;
	float blinkPhase = -1.f;
	float gazeX = 0.f;
	float gazeTargetX = 0.f;
	float gazeStateTime = 1.2f;
	bool gazeGlancing = false;
	int gazeSequence = 0;

	static float approach(float current, float target, float rate, float dt);
	static float clamp01(float value);
};
