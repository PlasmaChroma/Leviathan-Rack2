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
	applyConditionTo(EngineMode::ReferenceV2);
	applyConditionTo(EngineMode::ReferenceV3);
	applyConditionTo(EngineMode::Legacy);
}

void DoorstopEngineRouter::setSampleRate(float newSampleRate) {
	if (!std::isfinite(newSampleRate) || newSampleRate < 1000.f) {
		return;
	}
	sampleRate = newSampleRate;
	legacy.setSampleRate(sampleRate);
	reference.setSampleRate(sampleRate);
	referenceV2.setSampleRate(sampleRate);
	referenceV3.setSampleRate(sampleRate);
	transitionStep = 1.f / std::max(1.f, 0.015f * sampleRate);
}

ReferenceSpringEngine& DoorstopEngineRouter::referenceEngine(
	EngineMode mode) {
	return mode == EngineMode::ReferenceV2 ? referenceV2 : reference;
}

const ReferenceSpringEngine& DoorstopEngineRouter::referenceEngine(
	EngineMode mode) const {
	return mode == EngineMode::ReferenceV2 ? referenceV2 : reference;
}

void DoorstopEngineRouter::applyConditionTo(EngineMode mode) {
	if (mode == EngineMode::Legacy) {
		legacy.setSoundModel(selectedLegacyModel);
		legacy.setBreakIn(breakIn);
		legacy.setBreakInLocked(breakInLocked);
	}
	else if (mode == EngineMode::ReferenceV3) {
		referenceV3.setBreakIn(breakIn);
		referenceV3.setBreakInLocked(breakInLocked);
		referenceV3.setSpecimenSeed(specimenSeed);
	}
	else {
		ReferenceSpringEngine& destination = referenceEngine(mode);
		destination.setBreakIn(breakIn);
		destination.setBreakInLocked(breakInLocked);
		destination.setSpecimenSeed(specimenSeed);
	}
}

void DoorstopEngineRouter::resetEngineMotion(EngineMode mode) {
	if (mode == EngineMode::Legacy) legacy.resetMotion();
	else if (mode == EngineMode::ReferenceV3) referenceV3.resetMotion();
	else referenceEngine(mode).resetMotion();
}

void DoorstopEngineRouter::setEngineMode(EngineMode newMode) {
	newMode = sanitizeMode(newMode);
	if (!transitionActive && newMode == selectedMode) {
		return;
	}
	if (transitionActive) {
		if (newMode == transitionDestination) {
			selectedMode = newMode;
			transitionQueued = false;
			return;
		}
		if (newMode == transitionOutgoing) {
			std::swap(transitionOutgoing, transitionDestination);
			transitionProgress = 1.f - transitionProgress;
			selectedMode = newMode;
			transitionQueued = false;
			return;
		}
		// A third engine cannot join an equal-power two-engine fade without
		// creating an abrupt drop or temporarily tripling CPU. Queue it and
		// begin the second fade as soon as the current pair completes.
		queuedMode = newMode;
		transitionQueued = true;
		return;
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
	referenceV2.setBreakIn(breakIn);
	referenceV3.setBreakIn(breakIn);
}

void DoorstopEngineRouter::setBreakInLocked(bool locked) {
	breakInLocked = locked;
	legacy.setBreakInLocked(locked);
	reference.setBreakInLocked(locked);
	referenceV2.setBreakInLocked(locked);
	referenceV3.setBreakInLocked(locked);
}

void DoorstopEngineRouter::setSpecimenSeed(std::uint32_t seed) {
	specimenSeed = seed ? seed : 1u;
	reference.setSpecimenSeed(specimenSeed);
	referenceV2.setSpecimenSeed(specimenSeed);
	referenceV3.setSpecimenSeed(specimenSeed);
}

void DoorstopEngineRouter::strike(float normalizedVelocity) {
	applyConditionTo(selectedMode);
	if (selectedMode == EngineMode::Legacy) {
		legacy.strike(normalizedVelocity);
		breakIn = legacy.getBreakIn();
		reference.setBreakIn(breakIn);
		referenceV2.setBreakIn(breakIn);
	}
	else if (selectedMode == EngineMode::ReferenceV3) {
		referenceV3.strike(normalizedVelocity);
		breakIn = referenceV3.getBreakIn();
		legacy.setBreakIn(breakIn);
		reference.setBreakIn(breakIn);
		referenceV2.setBreakIn(breakIn);
	}
	else {
		ReferenceSpringEngine& selected = referenceEngine(selectedMode);
		selected.strike(normalizedVelocity);
		breakIn = selected.getBreakIn();
		legacy.setBreakIn(breakIn);
		reference.setBreakIn(breakIn);
		referenceV2.setBreakIn(breakIn);
		referenceV3.setBreakIn(breakIn);
	}
}

Frame DoorstopEngineRouter::processEngine(EngineMode mode, float requestedSampleTime) {
	if (mode == EngineMode::Legacy) {
		return legacy.process(requestedSampleTime);
	}
	if (mode == EngineMode::ReferenceV3) {
		return referenceV3.process(requestedSampleTime);
	}
	return referenceEngine(mode).process(requestedSampleTime);
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
		if (transitionQueued) {
			const EngineMode queued = queuedMode;
			transitionQueued = false;
			setEngineMode(queued);
		}
	}
	return result;
}

void DoorstopEngineRouter::resetMotion() {
	legacy.resetMotion();
	reference.resetMotion();
	referenceV2.resetMotion();
	referenceV3.resetMotion();
	transitionActive = false;
	transitionQueued = false;
	transitionProgress = 1.f;
}

void DoorstopEngineRouter::restoreFactoryFresh() {
	breakIn = 0.f;
	legacy.restoreFactoryFresh();
	reference.restoreFactoryFresh();
	referenceV2.restoreFactoryFresh();
	referenceV3.restoreFactoryFresh();
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
	referenceV2.reset();
	referenceV2.setSpecimenSeed(specimenSeed);
	referenceV3.reset();
	referenceV3.setSpecimenSeed(specimenSeed);
	resetMotion();
}

bool DoorstopEngineRouter::engineSleeping(EngineMode mode) const {
	if (mode == EngineMode::Legacy) return legacy.isSleeping();
	if (mode == EngineMode::ReferenceV3) return referenceV3.isSleeping();
	return referenceEngine(mode).isSleeping();
}

bool DoorstopEngineRouter::isSleeping() const {
	if (transitionActive) {
		return engineSleeping(transitionOutgoing)
			&& engineSleeping(transitionDestination);
	}
	return engineSleeping(selectedMode);
}

SoundModel DoorstopEngineRouter::getLastStrikeModel() const {
	return selectedMode == EngineMode::Legacy
		? legacy.getLastStrikeModel() : SoundModel::Classic;
}

float DoorstopEngineRouter::getVisualMaximumDisplacement() const {
	if (selectedMode == EngineMode::Legacy) {
		return legacy.getEffectiveTuning().maxDisplacement;
	}
	if (selectedMode == EngineMode::ReferenceV3) {
		return referenceV3.getVisualMaximumDisplacement();
	}
	return referenceEngine(selectedMode).getMaximumDisplacement();
}

} // namespace doorstop
