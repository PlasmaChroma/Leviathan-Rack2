#pragma once

#include "MoiraiCompiler.hpp"
#include "MoiraiEngine.hpp"
#include "MoiraiPresets.hpp"
#include "OctaviaSemanticControl.hpp"
#include "plugin.hpp"

#include <array>
#include <atomic>

struct MoiraiTelemetrySnapshot {
	int acceptedRevision = 0;
	int activeRevision = 0;
	int pendingRevision = -1;
	int channels = 1;
	float estimatedBpm = 120.f;
	bool externalClock = false;
	uint16_t activeMask[moirai::kLaneCount] {};
	float selectedValue[moirai::kLaneCount] {};
	float selectedPhase[moirai::kLaneCount] {};
	int selectedStage[moirai::kLaneCount] {};
};

struct Moirai final : Module, OctaviaSemanticControl {
	enum ParamId {
		TIME_PARAM,
		CURVE_PARAM,
		LEVEL_PARAM,
		LANE_PARAM,
		CHANNEL_PARAM,
		MANUAL_TRIGGER_PARAM,
		NUM_PARAMS
	};
	enum InputId {
		GATE_INPUT,
		VELOCITY_INPUT,
		M1_INPUT,
		M2_INPUT,
		M3_INPUT,
		CLOCK_INPUT,
		RESET_INPUT,
		NUM_INPUTS
	};
	enum OutputId {
		A_OUTPUT,
		EOC_A_OUTPUT,
		B_OUTPUT,
		EOC_B_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightId {
		LANE_A_LIGHT,
		LANE_B_LIGHT,
		NUM_LIGHTS
	};

	static_assert(TIME_PARAM == 0 && CURVE_PARAM == 1 && LEVEL_PARAM == 2 &&
		LANE_PARAM == 3 && CHANNEL_PARAM == 4 && MANUAL_TRIGGER_PARAM == 5 && NUM_PARAMS == 6,
		"Moirai parameter IDs are compatibility-frozen");
	static_assert(GATE_INPUT == 0 && VELOCITY_INPUT == 1 && M1_INPUT == 2 && M2_INPUT == 3 &&
		M3_INPUT == 4 && CLOCK_INPUT == 5 && RESET_INPUT == 6 && NUM_INPUTS == 7,
		"Moirai input IDs are compatibility-frozen");
	static_assert(A_OUTPUT == 0 && EOC_A_OUTPUT == 1 && B_OUTPUT == 2 && EOC_B_OUTPUT == 3 &&
		NUM_OUTPUTS == 4, "Moirai output IDs are compatibility-frozen");

	moirai::Bank authoredBank;
	moirai::CompiledBankPtr compiledBank;
	std::string persistenceError;
	moirai::Engine envelopeEngine;
	dsp::SchmittTrigger resetTrigger;
	dsp::SchmittTrigger clockTrigger;
	dsp::SchmittTrigger laneButtonTrigger;
	dsp::SchmittTrigger manualButtonTrigger;
	std::array<std::array<dsp::PulseGenerator, moirai::kMaxChannels>, moirai::kLaneCount> eocPulses;
	std::atomic<int> selectedLane {0};
	std::atomic<int> selectedChannel {0};
	std::atomic<int> telemetryChannels {1};
	std::array<std::atomic<uint16_t>, moirai::kLaneCount> telemetryActiveMask;
	std::array<std::atomic<float>, moirai::kLaneCount> telemetrySelectedValue;
	std::atomic<float> telemetryBpm {120.f};
	std::atomic<bool> telemetryExternalClock {false};
	std::atomic<uint64_t> telemetrySequence {0};
	std::atomic<int> acceptedRevisionSource {0};
	std::atomic<int> telemetryAcceptedRevision {0};
	std::atomic<int> telemetryActiveRevision {0};
	std::atomic<int> telemetryPendingRevision {-1};
	std::array<std::atomic<float>, moirai::kLaneCount> telemetrySelectedPhase;
	std::array<std::atomic<int>, moirai::kLaneCount> telemetrySelectedStage;
	std::array<std::atomic<uint16_t>, moirai::kLaneCount> semanticTriggerMask;
	std::atomic<bool> semanticResetRequested {false};
	int telemetryCountdown = 0;
	int manualChannelFloor = 1;
	double clockElapsed = 0.0;
	float estimatedBpm = 120.f;
	bool seenClockEdge = false;

	Moirai();
	void process(const ProcessArgs& args) override;
	void onReset(const ResetEvent& e) override;
	json_t* dataToJson() override;
	void dataFromJson(json_t* rootJ) override;
	MoiraiTelemetrySnapshot readTelemetry() const noexcept;
	const char* semanticCapabilityId() const noexcept override {
		return "leviathan.moirai.envelope-bank";
	}
	bool handleSemanticRequest(OctaviaSemanticControl::Operation operation,
		const std::string& requestJson, std::string& responseJson,
		std::string& error) override;

	float resolvedInputVoltage(InputId inputId, int channel, float neutral) noexcept;
	void resetRuntime() noexcept;
};
