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
			pose.mouthSmile, pose.mouthTension, pose.mouthClosure, pose.leftFinAngle,
			pose.rightFinAngle, pose.spineExtension, pose.blush,
		};
		const float expected[] = {
			reference.inflation, reference.squashX, reference.squashY,
			reference.verticalOffset, reference.gazeX, reference.gazeY,
			reference.leftBlink, reference.rightBlink, reference.squint,
			reference.mouthSmile, reference.mouthTension, reference.mouthClosure,
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

Result mouthClosesInfrequentlyAndReturnsToRest() {
	PuffyVisualState visual;
	PuffyCharacterController controller;
	controller.reset(visual);
	PuffyPose pose;
	float earlyMaximum = 0.f;
	for (int i = 0; i < 360; ++i) {
		controller.update(1.f / 60.f, visual, &pose);
		earlyMaximum = std::max(earlyMaximum, pose.mouthClosure);
	}
	float sequenceMaximum = 0.f;
	for (int i = 0; i < 180; ++i) {
		controller.update(1.f / 60.f, visual, &pose);
		sequenceMaximum = std::max(sequenceMaximum, pose.mouthClosure);
	}
	return {
		"Mouth rests open, closes occasionally, then reopens",
		earlyMaximum < 1e-7f && sequenceMaximum > 0.99f
			&& pose.mouthClosure < 1e-7f,
		"early=" + std::to_string(earlyMaximum)
			+ " closed=" + std::to_string(sequenceMaximum)
			+ " tail=" + std::to_string(pose.mouthClosure)
	};
}

Result polarityDominanceScalesEyeClosure() {
	struct EyeClosure {
		float leftBlink = 0.f;
		float rightBlink = 0.f;
		float leftSquint = 0.f;
		float rightSquint = 0.f;
	};
	const auto measure = [](float positive, float negative) {
		PuffyVisualState visual;
		visual.inputActivity = std::max(positive, negative);
		visual.positiveInputActivity = positive;
		visual.negativeInputActivity = negative;
		PuffyCharacterController controller;
		controller.reset(visual);
		PuffyPose pose;
		EyeClosure result;
		for (int i = 0; i < 400; ++i) {
			controller.update(1.f / 60.f, visual, &pose);
			result.leftBlink = std::max(result.leftBlink, pose.leftBlink);
			result.rightBlink = std::max(result.rightBlink, pose.rightBlink);
			result.leftSquint = std::max(result.leftSquint, pose.leftSquint);
			result.rightSquint = std::max(result.rightSquint, pose.rightSquint);
		}
		return result;
	};
	const EyeClosure balanced = measure(0.5f, 0.5f);
	const EyeClosure positive = measure(1.f, 0.05f);
	const EyeClosure negative = measure(0.05f, 1.f);
	const EyeClosure moderate = measure(0.8f, 0.2f);
	return {
		"Polarity dominance progressively protects only the over-represented eye",
		balanced.leftBlink > 0.99f && balanced.rightBlink > 0.99f
			&& std::fabs(balanced.leftBlink - balanced.rightBlink) < 1e-6f
			&& positive.leftBlink > 0.99f && positive.rightBlink < 0.02f
			&& positive.leftSquint > 0.40f && positive.rightSquint < 0.02f
			&& negative.rightBlink > 0.99f && negative.leftBlink < 0.02f
			&& negative.rightSquint > 0.40f && negative.leftSquint < 0.02f
			&& moderate.rightBlink > 0.10f && moderate.rightBlink < 0.40f
			&& moderate.leftBlink > 0.99f,
		"balanced=" + std::to_string(balanced.leftBlink)
			+ "/" + std::to_string(balanced.rightBlink)
			+ " positive=" + std::to_string(positive.leftBlink)
			+ "/" + std::to_string(positive.rightBlink)
			+ " negative=" + std::to_string(negative.leftBlink)
			+ "/" + std::to_string(negative.rightBlink)
			+ " moderate=" + std::to_string(moderate.leftBlink)
			+ "/" + std::to_string(moderate.rightBlink)
	};
}

Result energySurgeCreatesExcitedMotion() {
	PuffyVisualState visual;
	PuffyCharacterController controller;
	controller.reset(visual);
	PuffyPose pose;
	for (int i = 0; i < 60; ++i) {
		controller.update(1.f / 60.f, visual, &pose);
	}
	visual.inputActivity = 0.72f;
	controller.update(1.f / 60.f, visual, &pose);
	const float triggered = pose.excitement;
	float verticalRange = 0.f;
	float minimumOffset = pose.verticalOffset;
	float maximumOffset = pose.verticalOffset;
	float finRange = 0.f;
	float minimumFin = pose.leftFinAngle;
	float maximumFin = pose.leftFinAngle;
	for (int i = 0; i < 30; ++i) {
		controller.update(1.f / 60.f, visual, &pose);
		minimumOffset = std::min(minimumOffset, pose.verticalOffset);
		maximumOffset = std::max(maximumOffset, pose.verticalOffset);
		minimumFin = std::min(minimumFin, pose.leftFinAngle);
		maximumFin = std::max(maximumFin, pose.leftFinAngle);
	}
	verticalRange = maximumOffset - minimumOffset;
	finRange = maximumFin - minimumFin;
	return {
		"A sudden energy rise makes Puffy bob and flap excitedly",
		triggered >= 0.55f && verticalRange > 0.025f && finRange > 0.20f,
		"excitement=" + std::to_string(triggered)
			+ " bobRange=" + std::to_string(verticalRange)
			+ " finRange=" + std::to_string(finRange)
	};
}

Result steadyEnergyDoesNotExcite() {
	PuffyVisualState visual;
	visual.inputActivity = 0.65f;
	PuffyCharacterController controller;
	controller.reset(visual);
	PuffyPose pose;
	float maximumExcitement = 0.f;
	for (int i = 0; i < 240; ++i) {
		controller.update(1.f / 60.f, visual, &pose);
		maximumExcitement = std::max(maximumExcitement, pose.excitement);
	}
	return {
		"A steady loud passage does not repeatedly excite Puffy",
		maximumExcitement < 1e-7f,
		"maximumExcitement=" + std::to_string(maximumExcitement)
	};
}

Result sustainedSurgeDoesNotRetrigger() {
	PuffyVisualState visual;
	PuffyCharacterController controller;
	controller.reset(visual);
	PuffyPose pose;
	visual.inputActivity = 0.72f;
	controller.update(1.f / 60.f, visual, &pose);
	const float triggered = pose.excitement;
	for (int i = 0; i < 240; ++i) {
		controller.update(1.f / 60.f, visual, &pose);
	}
	return {
		"One sustained energy rise creates only one excitement event",
		triggered > 0.f && pose.excitement < 1e-7f,
		"triggered=" + std::to_string(triggered)
			+ " tail=" + std::to_string(pose.excitement)
	};
}

Result excitementDecays() {
	PuffyVisualState visual;
	PuffyCharacterController controller;
	controller.reset(visual);
	PuffyPose pose;
	visual.inputActivity = 0.8f;
	controller.update(1.f / 60.f, visual, &pose);
	const float triggered = pose.excitement;
	for (int i = 0; i < 150; ++i) {
		visual.inputActivity = 0.f;
		controller.update(1.f / 60.f, visual, &pose);
	}
	return {
		"Excitement returns fully to rest",
		triggered > 0.f && pose.excitement < 1e-7f,
		"triggered=" + std::to_string(triggered)
			+ " tail=" + std::to_string(pose.excitement)
	};
}

} // namespace

int main() {
	const Result results[] = {
		sharedTransientTwitch(),
		characterMotionIsUniform(),
		twitchThresholdAndDecay(),
		squintRemainsDistinctFromBlink(),
		mouthClosesInfrequentlyAndReturnsToRest(),
		polarityDominanceScalesEyeClosure(),
		energySurgeCreatesExcitedMotion(),
		steadyEnergyDoesNotExcite(),
		sustainedSurgeDoesNotRetrigger(),
		excitementDecays(),
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
