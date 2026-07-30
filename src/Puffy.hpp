#pragma once

#include "plugin.hpp"
#include "PuffyEngine.hpp"

#include <atomic>
#include <cstdint>

struct PuffyVisualState {
	float effectiveAmount = 0.f;
	float inputActivity = 0.f;
	float transientActivity = 0.f;
	float gainReduction = 0.f;
	int character = 0;
};

struct Puffy final : Module {
	enum ParamId {
		CHARACTER_PARAM,
		PUFF_PARAM,
		DEFLATE_PARAM,
		PUFF_CV_AMOUNT_PARAM,
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
		LIGHTS_LEN
	};

	ModuleTeardownTimer teardownTimer {"Puffy"};
	puffy::Engine engine;
	std::atomic<std::uint32_t> visualSequence {0u};
	std::atomic<float> visualEffectiveAmount {0.f};
	std::atomic<float> visualInputActivity {0.f};
	std::atomic<float> visualTransientActivity {0.f};
	std::atomic<float> visualGainReduction {0.f};
	std::atomic<int> visualCharacter {0};
	std::uint32_t visualDivider = 0u;
	std::uint32_t visualDivision = 200u;
	float lastGainReduction = 0.f;

	Puffy();
	~Puffy() override;

	void process(const ProcessArgs& args) override;
	void onReset(const ResetEvent& event) override;
	void onSampleRateChange(const SampleRateChangeEvent& event) override;
	bool readVisualState(PuffyVisualState* state) const;

private:
	void publishVisualState(const puffy::Frame& frame);
};

struct PuffyWidget;
extern Model* modelPuffy;
