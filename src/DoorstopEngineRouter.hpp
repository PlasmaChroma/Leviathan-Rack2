#pragma once

#include "DoorstopEngine.hpp"
#include "HelicalContinuumEngine.hpp"
#include "ReferenceSpringEngine.hpp"

#include <cstdint>

namespace doorstop {

enum class EngineMode : std::uint8_t {
	ReferenceV1 = 0,
	Legacy,
	ReferenceV2,
	ReferenceV3,
	Count
};

class DoorstopEngineRouter {
public:
	DoorstopEngineRouter();

	void reset();
	void resetMotion();
	void restoreFactoryFresh();
	void setSampleRate(float newSampleRate);
	void setEngineMode(EngineMode newMode);
	void setSoundModel(SoundModel newModel);
	void setBreakIn(float amount);
	void setBreakInLocked(bool locked);
	void setSpecimenSeed(std::uint32_t seed);
	void strike(float normalizedVelocity);
	Frame process(float sampleTime);

	bool isSleeping() const;
	EngineMode getEngineMode() const { return selectedMode; }
	SoundModel getSoundModel() const { return selectedLegacyModel; }
	SoundModel getLastStrikeModel() const;
	float getBreakIn() const { return breakIn; }
	bool isBreakInLocked() const { return breakInLocked; }
	std::uint32_t getSpecimenSeed() const { return specimenSeed; }
	float getVisualMaximumDisplacement() const;

	Engine& getLegacyEngine() { return legacy; }
	const Engine& getLegacyEngine() const { return legacy; }
	ReferenceSpringEngine& getReferenceEngine() { return reference; }
	const ReferenceSpringEngine& getReferenceEngine() const { return reference; }
	ReferenceSpringEngine& getReferenceV2Engine() { return referenceV2; }
	const ReferenceSpringEngine& getReferenceV2Engine() const {
		return referenceV2;
	}
	HelicalContinuumEngine& getReferenceV3Engine() { return referenceV3; }
	const HelicalContinuumEngine& getReferenceV3Engine() const {
		return referenceV3;
	}

private:
	Engine legacy;
	ReferenceSpringEngine reference;
	ReferenceSpringEngine referenceV2 {
		ReferenceSpringProfile::DarkRefinedV2
	};
	HelicalContinuumEngine referenceV3;
	EngineMode selectedMode = EngineMode::ReferenceV1;
	SoundModel selectedLegacyModel = SoundModel::ProbabilisticMix;
	float sampleRate = 44100.f;
	float breakIn = 0.f;
	bool breakInLocked = false;
	std::uint32_t specimenSeed = 1u;

	bool transitionActive = false;
	bool transitionQueued = false;
	EngineMode queuedMode = EngineMode::ReferenceV1;
	EngineMode transitionOutgoing = EngineMode::Legacy;
	EngineMode transitionDestination = EngineMode::ReferenceV1;
	float transitionProgress = 1.f;
	float transitionStep = 1.f;

	void applyConditionTo(EngineMode mode);
	void resetEngineMotion(EngineMode mode);
	Frame processEngine(EngineMode mode, float sampleTime);
	ReferenceSpringEngine& referenceEngine(EngineMode mode);
	const ReferenceSpringEngine& referenceEngine(EngineMode mode) const;
	bool engineSleeping(EngineMode mode) const;
};

} // namespace doorstop
