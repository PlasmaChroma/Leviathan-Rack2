#include "PuffyCharacterController.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>

namespace {

struct Result {
	std::string name;
	bool passed = false;
	std::string detail;
};

Result sharedTransientTwitch() {
	float minimumImpulse = 1.f;
	float maximumImpulse = -1.f;
	float maximumReactiveSquint = 0.f;
	for (int character = 0; character < puffy::kCharacterCount; ++character) {
		PuffyVisualState visual;
		visual.negativeCharacter = character;
		visual.positiveCharacter = character;
		visual.transientActivity = 0.f;
		PuffyCharacterController controller;
		controller.reset(visual);
		PuffyPose pose;
		controller.update(1.f / 60.f, visual, &pose);
		visual.transientActivity = 1.f;
		controller.update(1.f / 60.f, visual, &pose);
		minimumImpulse = std::min(minimumImpulse, pose.squashX);
		maximumImpulse = std::max(maximumImpulse, pose.squashX);
		maximumReactiveSquint = std::max(maximumReactiveSquint, pose.squint);
	}
	return {
		"Large transient twitch is identical across every character",
		minimumImpulse > 0.01f
			&& maximumImpulse - minimumImpulse < 1e-7f
			&& maximumReactiveSquint > 0.f
			&& maximumReactiveSquint < 0.16f,
		"impulse[min,max]=" + std::to_string(minimumImpulse)
			+ "/" + std::to_string(maximumImpulse)
			+ " squint=" + std::to_string(maximumReactiveSquint)
	};
}

Result characterMotionIsUniform() {
	PuffyPose reference;
	float maximumDifference = 0.f;
	for (int character = 0; character < puffy::kCharacterCount; ++character) {
		PuffyVisualState visual;
		visual.effectiveAmount = 0.72f;
		visual.inputActivity = 0.38f;
		visual.transientActivity = 0.24f;
		visual.gainReduction = 0.31f;
		visual.negativeCharacter = character;
		visual.positiveCharacter = character;
		PuffyCharacterController controller;
		controller.reset(visual);
		PuffyPose pose;
		for (int i = 0; i < 360; ++i) {
			controller.update(1.f / 60.f, visual, &pose);
		}
		if (character == 0) {
			reference = pose;
			continue;
		}
		const float current[] = {
			pose.inflation, pose.squashX, pose.squashY, pose.verticalOffset,
			pose.gazeX, pose.gazeY, pose.leftBlink, pose.rightBlink, pose.squint,
			pose.mouthSmile, pose.mouthTension, pose.leftFinAngle,
			pose.rightFinAngle, pose.spineExtension, pose.blush,
		};
		const float expected[] = {
			reference.inflation, reference.squashX, reference.squashY,
			reference.verticalOffset, reference.gazeX, reference.gazeY,
			reference.leftBlink, reference.rightBlink, reference.squint,
			reference.mouthSmile, reference.mouthTension,
			reference.leftFinAngle, reference.rightFinAngle,
			reference.spineExtension, reference.blush,
		};
		for (size_t i = 0; i < sizeof(current) / sizeof(current[0]); ++i) {
			maximumDifference = std::max(
				maximumDifference, std::fabs(current[i] - expected[i]));
		}
	}
	return {
		"All characters share one motion language while colors remain independent",
		maximumDifference < 1e-7f,
		"maximumMotionDifference=" + std::to_string(maximumDifference)
	};
}

Result twitchThresholdAndDecay() {
	PuffyVisualState visual;
	PuffyCharacterController controller;
	controller.reset(visual);
	PuffyPose pose;
	visual.transientActivity = 0.40f;
	controller.update(1.f / 60.f, visual, &pose);
	const float belowThreshold = std::fabs(pose.squashX);
	visual.transientActivity = 1.f;
	controller.update(1.f / 60.f, visual, &pose);
	const float triggered = std::fabs(pose.squashX);
	for (int i = 0; i < 30; ++i) {
		visual.transientActivity = 0.f;
		controller.update(1.f / 60.f, visual, &pose);
	}
	return {
		"Transient twitch ignores small rises and decays completely",
		belowThreshold < 1e-7f && triggered > 0.005f
			&& std::fabs(pose.squashX) < 1e-7f,
		"below=" + std::to_string(belowThreshold)
			+ " triggered=" + std::to_string(triggered)
			+ " tail=" + std::to_string(pose.squashX)
	};
}

Result squintRemainsDistinctFromBlink() {
	PuffyVisualState visual;
	PuffyCharacterController controller;
	controller.reset(visual);
	PuffyPose pose;
	float maximumSquint = 0.f;
	float maximumBlink = 0.f;
	int sustainedSquintFrames = 0;
	for (int i = 0; i < 400; ++i) {
		controller.update(1.f / 60.f, visual, &pose);
		maximumSquint = std::max(maximumSquint, pose.squint);
		if (pose.squint >= 0.40f) {
			++sustainedSquintFrames;
		}
		maximumBlink = std::max(
			maximumBlink, std::max(pose.leftBlink, pose.rightBlink));
	}
	return {
		"Autonomous squint is partial while blink still closes fully",
		maximumSquint > 0.40f && maximumSquint <= 0.42f + 1e-6f
			&& maximumBlink > 0.99f
			&& sustainedSquintFrames >= 20,
		"squint=" + std::to_string(maximumSquint)
			+ " blink=" + std::to_string(maximumBlink)
			+ " sustainedFrames=" + std::to_string(sustainedSquintFrames)
	};
}

} // namespace

int main() {
	const Result results[] = {
		sharedTransientTwitch(),
		characterMotionIsUniform(),
		twitchThresholdAndDecay(),
		squintRemainsDistinctFromBlink(),
	};
	int failures = 0;
	for (const Result& result : results) {
		std::cout << (result.passed ? "[PASS] " : "[FAIL] ")
			<< result.name << " :: " << result.detail << '\n';
		if (!result.passed) {
			++failures;
		}
	}
	std::cout << "[SUMMARY] puffy_character_controller_spec: "
		<< (int(sizeof(results) / sizeof(results[0])) - failures)
		<< "/" << int(sizeof(results) / sizeof(results[0])) << " passed\n";
	return failures == 0 ? 0 : 1;
}
