#pragma once

#include "MandelwakeEngine.hpp"
#include "MandelwakeVisualData.hpp"
#include "plugin.hpp"

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <limits>

struct Mandelwake final : Module {
	ModuleTeardownTimer teardownTimer {"Mandelwake"};

	enum ParamId {
		MAP_PARAM,
		CENTER_X_PARAM,
		CENTER_Y_PARAM,
		ZOOM_PARAM,
		ITERATIONS_PARAM,
		MUTATION_PARAM,
		SMOOTH_PARAM,
		RATE_PARAM,
		DENSITY_PARAM,
		X_AMOUNT_PARAM,
		Y_AMOUNT_PARAM,
		ZOOM_AMOUNT_PARAM,
		MUTATE_AMOUNT_PARAM,
		RESEED_PARAM,
		SEED_LOCK_PARAM,
		PARAMS_LEN
	};

	enum InputId {
		CLOCK_INPUT,
		RESET_INPUT,
		X_INPUT,
		Y_INPUT,
		ZOOM_INPUT,
		MUTATE_INPUT,
		SMOOTH_INPUT,
		RATE_INPUT,
		INPUTS_LEN
	};

	enum OutputId {
		X_OUTPUT,
		Y_OUTPUT,
		RADIUS_OUTPUT,
		PHASE_OUTPUT,
		GATE_OUTPUT,
		ESCAPE_OUTPUT,
		STEP_OUTPUT,
		OUTPUTS_LEN
	};

	enum LightId {
		SEED_LOCK_LIGHT,
		LIGHTS_LEN
	};

	enum DisplayQuality {
		DISPLAY_LOW = 0,
		DISPLAY_NORMAL,
		DISPLAY_HIGH,
		DISPLAY_FROZEN,
		DISPLAY_QUALITY_COUNT
	};

	enum class UiCommandType : std::uint8_t {
		SetSeed
	};

	struct UiCommand {
		UiCommandType type = UiCommandType::SetSeed;
		std::uint64_t seed = 0;
	};

	template <typename T, std::size_t Capacity>
	struct SpscQueue {
		static_assert(Capacity >= 2, "SPSC queue needs at least two slots");
		std::array<T, Capacity> values {};
		std::atomic<std::size_t> writeIndex {0};
		std::atomic<std::size_t> readIndex {0};

		bool push(const T& value) {
			const std::size_t write = writeIndex.load(std::memory_order_relaxed);
			const std::size_t next = (write + 1u) % Capacity;
			if (next == readIndex.load(std::memory_order_acquire)) return false;
			values[write] = value;
			writeIndex.store(next, std::memory_order_release);
			return true;
		}

		bool pop(T* value) {
			if (!value) return false;
			const std::size_t read = readIndex.load(std::memory_order_relaxed);
			if (read == writeIndex.load(std::memory_order_acquire)) return false;
			*value = values[read];
			readIndex.store((read + 1u) % Capacity, std::memory_order_release);
			return true;
		}
	};

	struct ChannelRuntime {
		dsp::SchmittTrigger clockTrigger;
		dsp::SchmittTrigger resetTrigger;
		std::uint64_t internalPhase = 0;
		std::uint64_t internalIncrement = 0;
		int cachedRateIndex = std::numeric_limits<int>::min();
		int cachedSmoothIndex = -1;
		float smoothCoefficient = 1.f;
		float targetX = 0.f;
		float targetY = 0.f;
		float targetRadius = 0.f;
		float targetPhase = 0.f;
		float currentX = 0.f;
		float currentY = 0.f;
		float currentRadius = 0.f;
		float currentPhase = 0.f;
		float lastMutation = 0.f;
		std::uint32_t gateSamples = 0;
		std::uint32_t escapeSamples = 0;
		std::uint32_t stepSamples = 0;
		bool active = false;
	};

	mandelwake::Engine engine;
	std::array<ChannelRuntime, mandelwake::kMaxChannels> runtime {};
	SpscQueue<mandelwake::VisualSnapshot, 3> visualSnapshots;
	SpscQueue<UiCommand, 8> uiCommands;
	std::atomic<std::uint64_t> publishedBaseSeed {0};

	std::atomic<int> selectedDisplayChannel {0};
	std::atomic<int> displayQuality {DISPLAY_NORMAL};
	std::atomic<bool> freeRunWhenUnclocked {true};
	std::atomic<bool> restartInternalPhaseOnReset {true};
	std::atomic<bool> phaseUnipolar {false};
	std::atomic<int> pulseWidthIndex {1};
	std::atomic<bool> compatibilityWarning {false};
	std::atomic<int> loadedAlgorithmVersion {1};

	dsp::SchmittTrigger reseedTrigger;
	float sampleRate = 44100.f;
	std::uint32_t pulseLengthSamples = 44;
	int cachedPulseWidthIndex = -1;
	std::uint32_t visualPublishCountdown = 0;
	int activeChannels = 1;
	int cachedMap = 0;
	bool clockWasConnected = false;
	bool resetWasConnected = false;

	Mandelwake();
	~Mandelwake() override;
	void onReset() override;
	void onRandomize() override;
	void process(const ProcessArgs& args) override;
	json_t* dataToJson() override;
	void dataFromJson(json_t* rootJ) override;

	bool consumeLatestVisualSnapshot(mandelwake::VisualSnapshot* snapshot);
	bool enqueueUiCommand(const UiCommand& command) { return uiCommands.push(command); }
	void setBaseSeedAndReset(std::uint64_t seed);
	std::uint64_t baseSeed() const { return publishedBaseSeed.load(std::memory_order_relaxed); }

private:
	void resetRuntimeChannel(int channel, bool resetInternalPhase);
	void resetRuntimeAll(bool resetInternalPhase);
	void refreshPulseLength();
	void refreshInternalIncrement(int channel, int rateIndex);
	mandelwake::StepInputs buildStepInputs(int channel);
	void applyStepOutputs(int channel, const mandelwake::StepOutputs& outputs);
	void updateSmoothing(int channel);
	void publishVisualSnapshot();
};

struct MandelwakeWidget : ModuleWidget {
	explicit MandelwakeWidget(Mandelwake* module);
	void appendContextMenu(Menu* menu) override;
};

extern Model* modelMandelwake;
