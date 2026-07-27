#include "DoorstopEngineRouter.hpp"

#include <algorithm>
#include <cmath>

namespace doorstop {
namespace {

constexpr float PI_OVER_TWO = 1.57079632679489661923f;

EngineMode sanitizeMode(EngineMode mode) {
	return mode < EngineMode::Count ? mode : EngineMode::ReferenceV1;
}

} // namespace

DoorstopEngineRouter::DoorstopEngineRouter() {
	setSampleRate(sampleRate);
	applyConditionTo(EngineMode::ReferenceV1);
	applyConditionTo(EngineMode::Legacy);
}

void DoorstopEngineRouter::setSampleRate(float newSampleRate) {
	if (!std::isfinite(newSampleRate) || newSampleRate < 1000.f) {
		return;
	}
	sampleRate = newSampleRate;
	legacy.setSampleRate(sampleRate);
	reference.setSampleRate(sampleRate);
	transitionStep = 1.f / std::max(1.f, 0.015f * sampleRate);
}

void DoorstopEngineRouter::applyConditionTo(EngineMode mode) {
	if (mode == EngineMode::Legacy) {
		legacy.setSoundModel(selectedLegacyModel);
		legacy.setBreakIn(breakIn);
		legacy.setBreakInLocked(breakInLocked);
	}
	else {
		reference.setBreakIn(breakIn);
		reference.setBreakInLocked(breakInLocked);
		reference.setSpecimenSeed(specimenSeed);
	}
}

void DoorstopEngineRouter::resetEngineMotion(EngineMode mode) {
	if (mode == EngineMode::Legacy) legacy.resetMotion();
	else reference.resetMotion();
}

void DoorstopEngineRouter::setEngineMode(EngineMode newMode) {
	newMode = sanitizeMode(newMode);
	if (!transitionActive && newMode == selectedMode) {
		return;
	}
	if (transitionActive) {
		if (newMode == transitionDestination) {
			selectedMode = newMode;
			return;
		}
		if (newMode == transitionOutgoing) {
			std::swap(transitionOutgoing, transitionDestination);
			transitionProgress = 1.f - transitionProgress;
			selectedMode = newMode;
			return;
		}
	}

	transitionOutgoing = selectedMode;
	transitionDestination = newMode;
	selectedMode = newMode;
	resetEngineMotion(transitionDestination);
	applyConditionTo(transitionDestination);
	transitionProgress = 0.f;
	transitionActive = true;
}

void DoorstopEngineRouter::setSoundModel(SoundModel newModel) {
	if (newModel >= SoundModel::Count) {
		newModel = SoundModel::ProbabilisticMix;
	}
	selectedLegacyModel = newModel;
	legacy.setSoundModel(newModel);
}

void DoorstopEngineRouter::setBreakIn(float amount) {
	if (!std::isfinite(amount)) {
		return;
	}
	breakIn = std::max(0.f, std::min(amount, 1.f));
	legacy.setBreakIn(breakIn);
	reference.setBreakIn(breakIn);
}

void DoorstopEngineRouter::setBreakInLocked(bool locked) {
	breakInLocked = locked;
	legacy.setBreakInLocked(locked);
	reference.setBreakInLocked(locked);
}

void DoorstopEngineRouter::setSpecimenSeed(std::uint32_t seed) {
	specimenSeed = seed ? seed : 1u;
	reference.setSpecimenSeed(specimenSeed);
}

void DoorstopEngineRouter::strike(float normalizedVelocity) {
	applyConditionTo(selectedMode);
	if (selectedMode == EngineMode::Legacy) {
		legacy.strike(normalizedVelocity);
		breakIn = legacy.getBreakIn();
		reference.setBreakIn(breakIn);
	}
	else {
		reference.strike(normalizedVelocity);
		breakIn = reference.getBreakIn();
		legacy.setBreakIn(breakIn);
	}
}

Frame DoorstopEngineRouter::processEngine(EngineMode mode, float requestedSampleTime) {
	if (mode == EngineMode::Legacy) {
		return legacy.process(requestedSampleTime);
	}
	return reference.process(requestedSampleTime);
}

Frame DoorstopEngineRouter::process(float requestedSampleTime) {
	if (!transitionActive) {
		return processEngine(selectedMode, requestedSampleTime);
	}

	const Frame outgoing = processEngine(transitionOutgoing, requestedSampleTime);
	const Frame destination = processEngine(transitionDestination, requestedSampleTime);
	const float t = std::max(0.f, std::min(transitionProgress, 1.f));
	const float outgoingGain = std::cos(t * PI_OVER_TWO);
	const float destinationGain = std::sin(t * PI_OVER_TWO);
	Frame result = destination.sleeping ? outgoing : destination;
	result.outputVolts = outgoing.outputVolts * outgoingGain
		+ destination.outputVolts * destinationGain;
	result.sleeping = outgoing.sleeping && destination.sleeping;
	result.enteredSleep = false;

	transitionProgress += transitionStep;
	if (transitionProgress >= 1.f) {
		resetEngineMotion(transitionOutgoing);
		transitionActive = false;
		transitionProgress = 1.f;
		result.enteredSleep = result.sleeping;
	}
	return result;
}

void DoorstopEngineRouter::resetMotion() {
	legacy.resetMotion();
	reference.resetMotion();
	transitionActive = false;
	transitionProgress = 1.f;
}

void DoorstopEngineRouter::restoreFactoryFresh() {
	breakIn = 0.f;
	legacy.restoreFactoryFresh();
	reference.restoreFactoryFresh();
	setBreakInLocked(breakInLocked);
	resetMotion();
}

void DoorstopEngineRouter::reset() {
	breakIn = 0.f;
	breakInLocked = false;
	selectedMode = EngineMode::ReferenceV1;
	selectedLegacyModel = SoundModel::ProbabilisticMix;
	legacy.reset();
	legacy.setSoundModel(selectedLegacyModel);
	reference.reset();
	reference.setSpecimenSeed(specimenSeed);
	resetMotion();
}

bool DoorstopEngineRouter::isSleeping() const {
	if (transitionActive) {
		return legacy.isSleeping() && reference.isSleeping();
	}
	return selectedMode == EngineMode::Legacy
		? legacy.isSleeping() : reference.isSleeping();
}

SoundModel DoorstopEngineRouter::getLastStrikeModel() const {
	return selectedMode == EngineMode::Legacy
		? legacy.getLastStrikeModel() : SoundModel::Classic;
}

float DoorstopEngineRouter::getVisualMaximumDisplacement() const {
	return selectedMode == EngineMode::Legacy
		? legacy.getEffectiveTuning().maxDisplacement
		: reference.getMaximumDisplacement();
}

} // namespace doorstop
