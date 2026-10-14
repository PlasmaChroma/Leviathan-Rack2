#pragma once

#include "PuffyEngine.hpp"

struct PuffyPose {
	float negativeCharacterTintWeights[puffy::kCharacterCount] = {1.f};
	float positiveCharacterTintWeights[puffy::kCharacterCount] = {1.f};
	float inflation = 0.f;
	float squashX = 0.f;
	float squashY = 0.f;
	float verticalOffset = 0.f;
	float gazeX = 0.f;
	float gazeY = 0.f;
	float leftBlink = 0.f;
	float rightBlink = 0.f;
	float squint = 0.f;
	float leftSquint = 0.f;
	float rightSquint = 0.f;
	float mouthSmile = 0.f;
	float mouthTension = 0.f;
	float mouthClosure = 0.f;
	float leftFinAngle = 0.f;
	float rightFinAngle = 0.f;
	float spineExtension = 0.f;
	float blush = 0.f;
	float excitement = 0.f;
};
