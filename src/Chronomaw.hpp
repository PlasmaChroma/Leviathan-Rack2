#pragma once

#include "ChronomawEngine.hpp"
#include "plugin.hpp"
#include <atomic>

struct Chronomaw : Module {
	static constexpr int kTimelineHistorySize = 360;
	static constexpr int kTimelineFutureSize = 360;
	static constexpr float kTimelineCaptureIntervalSec = 1.f / 60.f;

	enum ParamId {
		RUN_PARAM,
		BPM_PARAM,
		ACTIVE_BANK_PARAM,
		LOAD_BANK_PARAM,
		SAVE_BANK_PARAM,
		SELECTED_OUTPUT_PARAM,
		TIMELINE_ZOOM_PARAM,
		DENSITY_MODE_PARAM,
		PARAMS_LEN
	};

	enum InputId {
		CLK_INPUT,
		RUN_INPUT,
		RESET_INPUT,
		CV_1_INPUT,
		CV_2_INPUT,
		CV_3_INPUT,
		CV_4_INPUT,
		INPUTS_LEN
	};

	enum OutputId {
		OUT_1_OUTPUT,
		OUT_2_OUTPUT,
		OUT_3_OUTPUT,
		OUT_4_OUTPUT,
		OUT_5_OUTPUT,
		OUT_6_OUTPUT,
		OUT_7_OUTPUT,
		OUT_8_OUTPUT,
		OUTPUTS_LEN
	};

	enum LightId {
		RUN_LIGHT,
		SYNC_LIGHT,
		OUT_1_LIGHT,
		OUT_2_LIGHT,
		OUT_3_LIGHT,
		OUT_4_LIGHT,
		OUT_5_LIGHT,
		OUT_6_LIGHT,
		OUT_7_LIGHT,
		OUT_8_LIGHT,
		LIGHTS_LEN
	};

	chronomaw::ModuleState state;
	chronomaw::Engine engine;
	chronomaw::FrameOutputs frameOut;
	std::array<std::array<std::atomic<float>, kTimelineHistorySize>, chronomaw::kNumOutputs> timelineInternalHistory {};
	std::array<std::array<std::atomic<float>, kTimelineHistorySize>, chronomaw::kNumOutputs> timelineInternalHistoryMin {};
	std::array<std::array<std::atomic<float>, kTimelineHistorySize>, chronomaw::kNumOutputs> timelineInternalHistoryMax {};
	std::array<std::array<std::atomic<float>, kTimelineHistorySize>, chronomaw::kNumOutputs> timelineOutputHistory {};
	std::array<std::array<std::atomic<float>, kTimelineHistorySize>, chronomaw::kNumOutputs> timelineOutputHistoryMin {};
	std::array<std::array<std::atomic<float>, kTimelineHistorySize>, chronomaw::kNumOutputs> timelineOutputHistoryMax {};
	std::array<std::array<std::atomic<float>, kTimelineHistorySize>, chronomaw::kNumOutputs> timelineOutputPhaseHistory {};
	std::array<std::array<std::atomic<float>, kTimelineFutureSize>, chronomaw::kNumOutputs> timelineFutureOutput {};
	std::array<float, chronomaw::kNumOutputs> timelineInternalAccum {};
	std::array<float, chronomaw::kNumOutputs> timelineOutputAccum {};
	std::array<float, chronomaw::kNumOutputs> timelineInternalAccumMin {};
	std::array<float, chronomaw::kNumOutputs> timelineInternalAccumMax {};
	std::array<float, chronomaw::kNumOutputs> timelineOutputAccumMin {};
	std::array<float, chronomaw::kNumOutputs> timelineOutputAccumMax {};
	int timelineAccumSamples = 0;
	std::atomic<int> timelineWritePos {0};
	std::atomic<float> timelinePhaseBeats {0.f};
	std::atomic<float> timelineBpm {chronomaw::kDefaultBpm};
	std::atomic<uint64_t> timelineCycleCount {0u};
	std::array<std::atomic<float>, chronomaw::kNumOutputs> timelineTimingPhaseOffsets {};
	std::atomic<bool> timelineRunning {false};
	float timelineCaptureElapsedSec = 0.f;
	dsp::SchmittTrigger runButtonEdge;
	dsp::SchmittTrigger loadBankEdge;
	dsp::SchmittTrigger saveBankEdge;

	Chronomaw();

	void process(const ProcessArgs& args) override;
	void onReset() override;
	json_t* dataToJson() override;
	void dataFromJson(json_t* rootJ) override;
};

struct ChronomawWidget : ModuleWidget {
	explicit ChronomawWidget(Chronomaw* module);
	void appendContextMenu(Menu* menu) override;
};

extern Model* modelChronomaw;
