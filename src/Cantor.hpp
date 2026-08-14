#pragma once

#include "CantorCultureEngine.hpp"
#include "plugin.hpp"

#include <array>
#include <atomic>
#include <cstdint>

struct Cantor final : Module {
	enum ParamId {
		INTENT_PARAM,
		COHERENCE_PARAM,
		INTERPRET_PARAM,
		FIELD_PARAM,
		PARAMS_LEN
	};

	enum InputId {
		PITCH_INPUT,
		GATE_INPUT,
		INPUTS_LEN
	};

	enum OutputId {
		PITCH_OUTPUT,
		OUTPUTS_LEN
	};

	enum LightId {
		LIGHTS_LEN
	};

	cantor::CultureEngine culture;
	std::array<bool, cantor::kMaximumVoices> gateHigh {};
	std::array<float, cantor::kMaximumVoices> heldOutputs {};
	std::array<float, cantor::kMaximumVoices> staticRequests {};
	std::array<bool, cantor::kMaximumVoices> staticInitialized {};
	bool gateWasConnected = false;
	std::uint32_t staticDivider = 0u;

	std::atomic<std::uint32_t> cultureSeed {1u};
	std::atomic<std::uint32_t> serializedRandomState {1u};
	std::atomic<std::uint32_t> pendingSeed {1u};
	std::atomic<std::uint32_t> pendingRandomState {1u};
	std::atomic<bool> cultureStatePending {false};

	std::atomic<float> visualRequestedPitch {0.f};
	std::atomic<float> visualSelectedPitch {0.f};
	std::atomic<float> visualDistanceCents {0.f};
	std::atomic<float> visualHarmonicCost {0.f};
	std::atomic<int> visualActiveVoices {0};

	Cantor();

	void process(const ProcessArgs& args) override;
	void onReset(const ResetEvent& event) override;
	json_t* dataToJson() override;
	void dataFromJson(json_t* root) override;

private:
	cantor::CultureSettings currentSettings();
	void publishDecision(const cantor::CultureDecision& decision);
};

struct CantorWidget;
extern Model* modelCantor;
