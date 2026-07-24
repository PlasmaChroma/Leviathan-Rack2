#pragma once

#include "DebugTerminalMetrics.hpp"
#include "DoorstopEngine.hpp"
#include "plugin.hpp"

#include <atomic>
#include <cstdint>

struct Doorstop final : Module {
	enum ParamId {
		MANUAL_PARAM,
		PARAMS_LEN
	};

	enum InputId {
		TRIG_INPUT,
		VELOCITY_INPUT,
		INPUTS_LEN
	};

	enum OutputId {
		AUDIO_OUTPUT,
		OUTPUTS_LEN
	};

	enum LightId {
		STRIKE_LIGHT,
		LIGHTS_LEN
	};

	dsp::SchmittTrigger trigTrigger;
	dsp::SchmittTrigger manualTrigger;
	doorstop::Engine engine;

	std::atomic<bool> allowVisualOverflow {true};
	std::atomic<int> soundModel {int(doorstop::SoundModel::ProbabilisticMix)};
	std::atomic<bool> breakInLocked {false};
	std::atomic<bool> restoreSpringRequested {false};
	std::atomic<float> serializedBreakIn {0.f};
	std::atomic<float> pendingBreakIn {0.f};
	std::atomic<bool> breakInStatePending {false};
	std::atomic<float> pendingManualVelocity {0.5f};
	std::atomic<bool> manualVelocityPending {false};
	std::atomic<float> visualDisplacement {0.f};
	std::atomic<float> visualVelocity {0.f};
	std::atomic<float> visualEnergy {0.f};
	std::atomic<float> visualStrike {0.f};
	std::atomic<int> visualLastStrikeModel {int(doorstop::SoundModel::Classic)};
	std::uint32_t telemetryDivider = 0u;
	debug_terminal::BaselineModuleMetrics debugMetrics;

	Doorstop();

	void process(const ProcessArgs& args) override;
	void onReset(const ResetEvent& e) override;
	void onSampleRateChange(const SampleRateChangeEvent& e) override;
	json_t* dataToJson() override;
	void dataFromJson(json_t* rootJ) override;

	void publishVisualState(const doorstop::Frame& frame);
	void publishZeroVisualState();
};
