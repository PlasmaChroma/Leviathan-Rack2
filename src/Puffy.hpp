#pragma once

#include "DebugTerminalMetrics.hpp"
#include "plugin.hpp"
#include "PuffyEngine.hpp"

#include <atomic>
#include <cstdint>

struct PuffyVisualState {
	float effectiveAmount = 0.f;
	float wetMix = 1.f;
	float inputActivity = 0.f;
	float positiveInputActivity = 0.f;
	float negativeInputActivity = 0.f;
	float leftPositiveInputActivity = 0.f;
	float leftNegativeInputActivity = 0.f;
	float rightPositiveInputActivity = 0.f;
	float rightNegativeInputActivity = 0.f;
	float transientActivity = 0.f;
	float gainReduction = 0.f;
	int negativeCharacter = 0;
	int positiveCharacter = 0;
	bool charactersLinked = true;
	bool stereoInputsConnected = false;
	float movementAcceleration = 0.f;
};

struct Puffy final : Module {
	enum ParamId {
		CHARACTER_PARAM,
		PUFF_PARAM,
		SENSITIVITY_PARAM,
		PUFF_CV_AMOUNT_PARAM,
		MIX_PARAM,
		POSITIVE_CHARACTER_PARAM,
		CHARACTER_LINK_PARAM,
		ROAMING_RANGE_PARAM,
		LIMITER_BUTTON_PARAM,
		ROAMING_BUTTON_PARAM,
		PARAMS_LEN
	};

	enum InputId {
		INPUT_L,
		INPUT_R,
		PUFF_CV_INPUT,
		INPUTS_LEN
	};

	enum OutputId {
		OUTPUT_L,
		OUTPUT_R,
		OUTPUTS_LEN
	};

	enum LightId {
		LIMIT_LIGHT,
		CHARACTER_LINK_LIGHT,
		LIMITER_HARD_LIGHT,
		LIMITER_SOFT_LIGHT,
		LIMITER_OFF_LIGHT,
		ROAMING_LIGHT,
		LIGHTS_LEN
	};

	ModuleTeardownTimer teardownTimer {"Puffy"};
	puffy::Engine engine;
	std::atomic<bool> autoDeflateEnabled {false};
	std::atomic<bool> roamingEnabled {false};
	std::atomic<int> limiterMode {int(puffy::LimiterMode::Hard)};
	dsp::SchmittTrigger limiterButtonTrigger;
	// UI acknowledgement that the rack-level roaming avatar is attached.
	// Keep this separate from the persisted preference so the panel fish never
	// disappears merely because the overlay could not yet be created.
	std::atomic<bool> roamingAvatarActive {false};
	std::atomic<float> roamingTargetX {0.f};
	std::atomic<float> roamingTargetY {0.f};
	std::atomic<float> roamingDirectionAngle {0.f};
	std::atomic<float> roamingDistance {0.f};
	std::atomic<float> roamingMovementAcceleration {0.f};
	std::atomic<std::uint32_t> visualSequence {0u};
	std::atomic<float> visualEffectiveAmount {0.f};
	std::atomic<float> visualWetMix {1.f};
	std::atomic<float> visualInputActivity {0.f};
	std::atomic<float> visualPositiveInputActivity {0.f};
	std::atomic<float> visualNegativeInputActivity {0.f};
	std::atomic<float> visualLeftPositiveInputActivity {0.f};
	std::atomic<float> visualLeftNegativeInputActivity {0.f};
	std::atomic<float> visualRightPositiveInputActivity {0.f};
	std::atomic<float> visualRightNegativeInputActivity {0.f};
	std::atomic<float> visualTransientActivity {0.f};
	std::atomic<float> visualGainReduction {0.f};
	std::atomic<int> visualNegativeCharacter {0};
	std::atomic<int> visualPositiveCharacter {0};
	std::atomic<bool> visualCharactersLinked {true};
	std::atomic<bool> visualStereoInputsConnected {false};
	std::uint32_t visualDivider = 0u;
	std::uint32_t visualDivision = 200u;
	float lastGainReduction = 0.f;
	debug_terminal::BaselineModuleMetrics debugMetrics;

	Puffy();
	~Puffy() override;

	void process(const ProcessArgs& args) override;
	void onReset(const ResetEvent& event) override;
	void onSampleRateChange(const SampleRateChangeEvent& event) override;
	json_t* dataToJson() override;
	void dataFromJson(json_t* root) override;
	bool readVisualState(PuffyVisualState* state) const;
	void synchronizeCharacterSelectionFromUi(bool negativeIsSource);

private:
	void publishVisualState(const puffy::Frame& frame, bool stereoInputsConnected);
};

struct PuffyWidget;
extern Model* modelPuffy;
