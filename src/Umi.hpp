#pragma once

#include "UmiEngine.hpp"
#include "plugin.hpp"

#include <array>
#include <atomic>
#include <cstdint>

struct Umi final : Module {
	ModuleTeardownTimer teardownTimer {"Umi"};

	enum ParamId {
		DROP_PARAM,
		RATE_PARAM,
		DENSITY_PARAM,
		GRAVITY_PARAM,
		TILT_PARAM,
		BOUNCE_PARAM,
		DRAG_PARAM,
		CHAOS_PARAM,
		CLEAR_PARAM,
		PARAMS_LEN
	};

	enum InputId {
		DROP_INPUT,
		GRAVITY_CV_INPUT,
		TILT_CV_INPUT,
		BOUNCE_CV_INPUT,
		CHAOS_CV_INPUT,
		CLEAR_INPUT,
		INPUTS_LEN
	};

	enum OutputId {
		GATES_OUTPUT,
		ANY_OUTPUT,
		LEFT_OUTPUT,
		RIGHT_OUTPUT,
		VEL_OUTPUT,
		POS_OUTPUT,
		ACT_OUTPUT,
		OUTPUTS_LEN
	};

	enum LightId {
		DROP_LIGHT,
		CLEAR_LIGHT,
		SINK1_LIGHT,
		SINK2_LIGHT,
		SINK3_LIGHT,
		SINK4_LIGHT,
		SINK5_LIGHT,
		SINK6_LIGHT,
		SINK7_LIGHT,
		SINK8_LIGHT,
		ANY_LIGHT,
		LIGHTS_LEN
	};

	struct BallRenderState {
		umi::Vec2 pos;
		umi::Vec2 vel;
		float radius = 0.f;
		float age = 0.f;
		std::uint32_t id = 0;
	};

	struct RenderSnapshot {
		std::array<BallRenderState, umi::MAX_BALLS> balls {};
		std::array<std::uint32_t, umi::SINK_COUNT> captureSerial {};
		std::uint32_t ballCount = 0;
		std::uint32_t dropSerial = 0;
		std::uint32_t seed = 1;
		float activity = 0.f;
		std::uint8_t layoutIndex = 0;
	};

	enum class UiCommandType : std::uint8_t {
		DropAtX,
		SetSeed,
		Clear,
		ResetBoard
	};

	struct UiCommand {
		UiCommandType type = UiCommandType::Clear;
		float value = 0.f;
		std::uint32_t seed = 1;
	};

	template <typename T, std::size_t Capacity>
	struct SpscQueue {
		static_assert(Capacity >= 2, "SPSC queues need at least two slots");
		std::array<T, Capacity> values {};
		std::atomic<std::size_t> writeIndex {0};
		std::atomic<std::size_t> readIndex {0};

		bool push(const T& value) {
			const std::size_t write = writeIndex.load(std::memory_order_relaxed);
			const std::size_t next = (write + 1) % Capacity;
			if (next == readIndex.load(std::memory_order_acquire)) {
				return false;
			}
			values[write] = value;
			writeIndex.store(next, std::memory_order_release);
			return true;
		}

		bool pop(T* value) {
			const std::size_t read = readIndex.load(std::memory_order_relaxed);
			if (read == writeIndex.load(std::memory_order_acquire)) {
				return false;
			}
			*value = values[read];
			readIndex.store((read + 1) % Capacity, std::memory_order_release);
			return true;
		}
	};

	umi::Engine engine;
	umi::PhysicsParams physicsParams;
	std::array<dsp::PulseGenerator, umi::SINK_COUNT> sinkPulses {};
	dsp::PulseGenerator anyPulse;
	dsp::PulseGenerator leftPulse;
	dsp::PulseGenerator rightPulse;
	dsp::SchmittTrigger dropButtonTrigger;
	dsp::SchmittTrigger dropInputTrigger;
	dsp::SchmittTrigger clearButtonTrigger;
	dsp::SchmittTrigger clearInputTrigger;
	dsp::ClockDivider controlDivider;

	SpscQueue<RenderSnapshot, 3> renderSnapshots;
	SpscQueue<UiCommand, 16> uiCommands;
	std::array<std::uint32_t, umi::SINK_COUNT> captureSerial {};
	std::atomic<std::uint32_t> publishedSeed {1u};
	std::atomic<int> maxBallsSetting {32};
	std::atomic<int> pulseLengthIndex {2};
	std::atomic<bool> replaceOldestSetting {false};

	float physicsAccumulator = 0.f;
	float autoDropPhase = 0.f;
	float cachedRateHz = 0.f;
	int cachedDensity = 1;
	int appliedMaxBalls = 32;
	bool appliedReplaceOldest = false;
	float velocityCv = 0.f;
	float positionCv = 0.f;
	float activity = 0.f;
	std::uint32_t dropSerial = 0;
	std::uint8_t renderSnapshotDivider = 0;
	float dropFlash = 0.f;
	float clearFlash = 0.f;

	Umi();
	~Umi() override;
	void onReset() override;
	void process(const ProcessArgs& args) override;
	json_t* dataToJson() override;
	void dataFromJson(json_t* rootJ) override;

	bool enqueueUiCommand(const UiCommand& command) { return uiCommands.push(command); }
	bool consumeLatestSnapshot(RenderSnapshot* snapshot);
	float currentPulseLengthSeconds() const;

private:
	void updateCachedControls();
	void applySettings();
	void spawnDrop(float normalizedX = -1.f);
	void clearBoard(bool resetTiming);
	void resetBoard(std::uint32_t newSeed);
	void handleCapture(const umi::CaptureEvent& event);
	void publishSnapshot();
};

struct UmiWidget : ModuleWidget {
	explicit UmiWidget(Umi* module);
	void appendContextMenu(Menu* menu) override;
};

extern Model* modelUmi;
