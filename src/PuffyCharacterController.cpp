#include "PuffyCharacterController.hpp"

#include <algorithm>
#include <cmath>

namespace {

constexpr float kPi = 3.14159265358979323846f;

float smoothstep(float value) {
	const float x = std::max(0.f, std::min(value, 1.f));
	return x * x * (3.f - 2.f * x);
}

} // namespace

PuffyCharacterController::PuffyCharacterController() {
	PuffyVisualState preview;
	preview.effectiveAmount = 0.25f;
	reset(preview);
}

float PuffyCharacterController::clamp01(float value) {
	return std::max(0.f, std::min(value, 1.f));
}

float PuffyCharacterController::approach(
	float current,
	float target,
	float rate,
	float dt) {
	return current + (target - current) * clamp01(rate * dt);
}

void PuffyCharacterController::reset(const PuffyVisualState& visual) {
	inflation = clamp01(
		0.65f * visual.effectiveAmount
		+ 0.25f * visual.inputActivity
		+ 0.10f * visual.gainReduction);
	inflationVelocity = 0.f;
	const int negativeCharacter =
		std::max(0, std::min(
			visual.negativeCharacter, puffy::kCharacterCount - 1));
	const int positiveCharacter =
		std::max(0, std::min(
			visual.positiveCharacter, puffy::kCharacterCount - 1));
	for (int i = 0; i < puffy::kCharacterCount; ++i) {
		negativeCharacterTintWeights[i] = i == negativeCharacter ? 1.f : 0.f;
		positiveCharacterTintWeights[i] = i == positiveCharacter ? 1.f : 0.f;
	}
	transientMemory = visual.transientActivity;
	twitchPhase = -1.f;
	twitchStrength = 0.f;
	twitchCooldown = 0.f;
	idleTime = 0.f;
	nextBlinkTime = 3.2f;
	blinkPhase = -1.f;
	nextSquintTime = 5.4f;
	squintPhase = -1.f;
	gazeX = 0.f;
	gazeTargetX = 0.f;
	gazeStateTime = 1.2f;
	gazeGlancing = false;
	gazeSequence = 0;
}

bool PuffyCharacterController::update(
	float requestedDt,
	const PuffyVisualState& visual,
	PuffyPose* pose) {
	if (!pose) {
		return false;
	}
	const float dt = std::max(0.f, std::min(requestedDt, 1.f / 15.f));
	idleTime += dt;
	const float targetInflation = clamp01(
		0.65f * visual.effectiveAmount
		+ 0.25f * visual.inputActivity
		+ 0.10f * visual.gainReduction);
	inflationVelocity += (targetInflation - inflation) * 48.f * dt;
	inflationVelocity *= std::max(0.f, 1.f - 8.5f * dt);
	inflation += inflationVelocity * dt;
	inflation = clamp01(inflation);

	const int negativeCharacter =
		std::max(0, std::min(
			visual.negativeCharacter, puffy::kCharacterCount - 1));
	const int positiveCharacter =
		std::max(0, std::min(
			visual.positiveCharacter, puffy::kCharacterCount - 1));
	for (int i = 0; i < puffy::kCharacterCount; ++i) {
		negativeCharacterTintWeights[i] = approach(
			negativeCharacterTintWeights[i],
			i == negativeCharacter ? 1.f : 0.f,
			5.f,
			dt);
		positiveCharacterTintWeights[i] = approach(
			positiveCharacterTintWeights[i],
			i == positiveCharacter ? 1.f : 0.f,
			5.f,
			dt);
		pose->negativeCharacterTintWeights[i] =
			negativeCharacterTintWeights[i];
		pose->positiveCharacterTintWeights[i] =
			positiveCharacterTintWeights[i];
	}
	const float transientRise =
		std::max(0.f, visual.transientActivity - transientMemory);
	transientMemory = approach(
		transientMemory, visual.transientActivity, 14.f, dt);
	twitchCooldown = std::max(0.f, twitchCooldown - dt);
	if (twitchCooldown <= 0.f
		&& visual.transientActivity >= 0.55f
		&& transientRise >= 0.12f) {
		twitchPhase = 0.f;
		twitchStrength = clamp01(
			(visual.transientActivity - 0.45f) * (1.f / 0.55f));
		twitchCooldown = 0.18f;
	}
	float twitch = 0.f;
	if (twitchPhase >= 0.f) {
		twitchPhase += dt;
		const float phase = twitchPhase / 0.24f;
		if (phase < 1.f) {
			const float decay = (1.f - phase) * (1.f - phase);
			twitch = twitchStrength * std::sin(2.f * kPi * phase) * decay;
		}
		else {
			twitchPhase = -1.f;
			twitchStrength = 0.f;
		}
	}

	if (blinkPhase < 0.f && squintPhase < 0.f && idleTime >= nextBlinkTime) {
		blinkPhase = 0.f;
		nextBlinkTime += 4.1f;
	}
	float blink = 0.f;
	if (blinkPhase >= 0.f) {
		blinkPhase += dt;
		if (blinkPhase < 0.09f) {
			blink = smoothstep(blinkPhase / 0.09f);
		}
		else if (blinkPhase < 0.14f) {
			blink = 1.f;
		}
		else if (blinkPhase < 0.28f) {
			blink = 1.f - smoothstep((blinkPhase - 0.14f) / 0.14f);
		}
		else {
			blinkPhase = -1.f;
		}
	}
	if (squintPhase < 0.f && blinkPhase < 0.f && idleTime >= nextSquintTime) {
		squintPhase = 0.f;
		nextSquintTime += 7.7f;
	}
	float squint = 0.f;
	if (squintPhase >= 0.f) {
		squintPhase += dt;
		if (squintPhase < 0.16f) {
			squint = 0.42f * smoothstep(squintPhase / 0.16f);
		}
		else if (squintPhase < 0.50f) {
			squint = 0.42f;
		}
		else if (squintPhase < 0.90f) {
			squint = 0.42f
				* (1.f - smoothstep((squintPhase - 0.50f) / 0.40f));
		}
		else {
			squintPhase = -1.f;
		}
	}

	const float breath = std::sin(idleTime * (2.f * kPi / 4.2f))
		* (0.003f + 0.006f * visual.inputActivity);
	pose->inflation = clamp01(inflation + breath);
	pose->squashX = 0.045f * twitch;
	pose->squashY = -pose->squashX * 0.65f;
	pose->verticalOffset =
		std::sin(idleTime * (2.f * kPi / 5.4f)) * 0.008f
		- 0.022f * twitch;
	// Doom-style gaze: rest near center, snap decisively to one side, hold
	// briefly, then return. Vary the cadence while alternating directions.
	gazeStateTime -= dt;
	if (gazeStateTime <= 0.f) {
		if (gazeGlancing) {
			gazeGlancing = false;
			gazeTargetX = 0.f;
			gazeStateTime = 2.20f + 0.55f * float(gazeSequence % 4);
		}
		else {
			gazeGlancing = true;
			const float direction = (gazeSequence & 1) ? 1.f : -1.f;
			gazeTargetX = direction
				* (0.60f + 0.06f * float(gazeSequence % 3));
			gazeStateTime =
				0.75f + 0.18f * float((gazeSequence + 1) % 3);
			gazeSequence++;
		}
	}
	gazeX = approach(
		gazeX, gazeTargetX, gazeGlancing ? 18.f : 11.f, dt);
	pose->gazeX = gazeX;
	pose->gazeY = 0.f;
	pose->leftBlink = blink;
	pose->rightBlink = blink;
	pose->squint = std::max(squint, 0.16f * std::fabs(twitch));
	pose->mouthSmile = 0.75f;
	pose->mouthTension = clamp01(0.35f * visual.gainReduction);
	const float finFlutter = std::sin(idleTime * 3.1f);
	pose->leftFinAngle =
		-0.12f - 0.10f * finFlutter - 0.18f * twitch;
	pose->rightFinAngle =
		0.12f + 0.10f * finFlutter + 0.18f * twitch;
	pose->spineExtension = clamp01(
		0.30f + 0.18f * visual.effectiveAmount);
	pose->blush = approach(pose->blush, visual.gainReduction, 8.f, dt);
	return true;
}
