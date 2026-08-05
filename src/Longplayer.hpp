#pragma once

#include "LongplayerStream.hpp"
#include "plugin.hpp"

#include <atomic>
#include <cstdint>
#include <string>

struct Longplayer final : Module {
	enum ParamId {
		PLAY_PARAM,
		LOOP_PARAM,
		RATE_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		TRIGGER_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		LEFT_OUTPUT,
		RIGHT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		PLAY_LIGHT,
		LIGHTS_LEN
	};

	ModuleTeardownTimer teardownTimer {"Longplayer"};
	longplayer::Stream stream;
	dsp::SchmittTrigger trigger;
	double playhead = 0.0;
	std::uint64_t observedGeneration = 0u;
	std::uint32_t appliedSeekRevision = 0u;
	std::uint32_t publishDivider = 0u;
	float lastLeft = 0.f;
	float lastRight = 0.f;
	float transitionLeft = 0.f;
	float transitionRight = 0.f;
	int transitionSamples = 0;
	int transitionLength = 1;
	bool wasPlaying = false;

	std::atomic<float> requestedSeek {0.f};
	std::atomic<std::uint32_t> requestedSeekRevision {0u};
	std::atomic<float> uiProgress {0.f};
	std::atomic<bool> uiBuffering {false};

	Longplayer();
	~Longplayer() override;
	void process(const ProcessArgs& args) override;
	void onReset() override;
	void onSampleRateChange(const SampleRateChangeEvent& event) override;
	json_t* dataToJson() override;
	void dataFromJson(json_t* root) override;

	bool loadFile(const std::string& path, std::string* error = nullptr);
	void clearFile();
	void seekNormalized(float normalized);
	float progress() const;
	double durationSeconds() const;
	double playheadSeconds() const;
	bool isLoading() const;
	bool isBuffering() const;
	bool hasFile() const;
	std::string filePath() const;
	std::string displayName() const;
	std::string loadError() const;

private:
	void beginTransition();
};

struct LongplayerWidget;
extern Model* modelLongplayer;
