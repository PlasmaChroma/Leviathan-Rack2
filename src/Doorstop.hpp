#pragma once

#include "DebugTerminalMetrics.hpp"
#include "DoorstopEngineRouter.hpp"
#include "DoorstopVisualFeedback.hpp"
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
		LIGHTS_LEN
	};

	dsp::SchmittTrigger trigTrigger;
	dsp::SchmittTrigger manualTrigger;
	doorstop::DoorstopEngineRouter engine;

	std::atomic<bool> allowVisualOverflow {true};
	std::atomic<int> engineMode {int(doorstop::EngineMode::ReferenceV1)};
	std::atomic<int> soundModel {int(doorstop::SoundModel::ProbabilisticMix)};
	std::atomic<int> referenceV3Tuning {
		int(doorstop::HelicalTuningVariant::BoingProbe)};
	std::atomic<int> referenceV4ModelDataRevision {
		doorstop::ContactHelixEngine::MODEL_DATA_REVISION};
	std::atomic<float> v4Excitation {doorstop::contact_helix_defaults::EXCITATION};
	std::atomic<float> v4Bend {doorstop::contact_helix_defaults::BEND};
	std::atomic<float> v4Twang {doorstop::contact_helix_defaults::FUNDAMENTAL};
	std::atomic<float> v4Contact {doorstop::contact_helix_defaults::CONTACT};
	std::atomic<float> v4Decay {doorstop::contact_helix_defaults::BODY_DECAY};
	std::atomic<float> v4Output {doorstop::contact_helix_defaults::OUTPUT};
	std::atomic<float> v4Pitch {doorstop::contact_helix_defaults::PITCH};
	std::atomic<float> v4Dispersion {doorstop::contact_helix_defaults::DISPERSION};
	std::atomic<float> v4Sweep {doorstop::contact_helix_defaults::SWEEP};
	std::atomic<float> v4Metal {doorstop::contact_helix_defaults::METAL};
	std::atomic<float> v4ContactLoss {doorstop::contact_helix_defaults::CONTACT_LOSS};
	std::atomic<float> v4Gap {doorstop::contact_helix_defaults::GAP};
	std::atomic<float> v4CapMass {doorstop::contact_helix_defaults::CAP_MASS};
	std::atomic<float> v4MountCompliance {doorstop::contact_helix_defaults::MOUNT_COMPLIANCE};
	std::atomic<float> v4BendDecay {doorstop::contact_helix_defaults::BEND_DECAY};
	std::atomic<float> v4BendRate {doorstop::contact_helix_defaults::BEND_RATE};
	std::atomic<float> v4Transfer {doorstop::contact_helix_defaults::TRANSFER};
	std::atomic<float> v4Radiation {doorstop::contact_helix_defaults::RADIATION};
	std::atomic<float> v4Pairing {doorstop::contact_helix_defaults::PAIRING};
	std::atomic<float> v4Reaction {doorstop::contact_helix_defaults::REACTION};
	std::atomic<float> v4Lobes {doorstop::contact_helix_defaults::LOBES};
	std::atomic<float> v4Attack {doorstop::contact_helix_defaults::ATTACK};
	std::atomic<float> v4FlickTime {doorstop::contact_helix_defaults::FLICK_TIME};
	std::atomic<float> v4MidBody {doorstop::contact_helix_defaults::MID_BODY};
	std::atomic<float> v4V3Pitch {doorstop::contact_helix_defaults::V3_PITCH};
	std::atomic<std::uint32_t> specimenSeed {1u};
	std::atomic<std::uint32_t> pendingSpecimenSeed {1u};
	std::atomic<bool> specimenStatePending {false};
	std::atomic<bool> newSpecimenRequested {false};
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
	float visualEnergyEnvelope = 0.f;
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
