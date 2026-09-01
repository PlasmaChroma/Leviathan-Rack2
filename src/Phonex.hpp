#pragma once

#include "PhonexEngine.hpp"
#include "PhonexPronunciation.hpp"
#include "PhonexSequenceMailbox.hpp"
#include "plugin.hpp"

#include <atomic>
#include <cstdint>
#include <string>

struct Phonex final : Module {
	enum ParamId {
		PITCH_PARAM,
		FORMANT_PARAM,
		SPEED_PARAM,
		WARP_PARAM,
		EXCITE_BLEND_PARAM,
		BEND_PARAM,
		GLITCH_PARAM,
		WORD_PARAM,
		WORD_PUSH_PARAM,
		PARAMS_LEN
	};

	enum InputId {
		VOCT_INPUT,
		TRIG_GATE_INPUT,
		SCRUB_CV_INPUT,
		WARP_CV_INPUT,
		BEND_CV_INPUT,
		EXT_EXCITE_INPUT,
		WORD_CV_INPUT,
		INPUTS_LEN
	};

	enum OutputId {
		AUDIO_OUTPUT,
		FRAME_CLK_OUTPUT,
		EOX_OUTPUT,
		OUTPUTS_LEN
	};

	enum LightId {
		VOICED_LIGHT,
		FRAME_LIGHT,
		EOX_LIGHT,
		BEND_LIGHT,
		LIGHTS_LEN
	};

	enum class ActiveSource : std::uint8_t { Bundled = 0, Text };

	static constexpr std::uint32_t kDefaultSeed = 0x50484f4eu;

	phonex::Engine engine;
	phonex::SequenceMailbox textMailbox;
	std::atomic<ActiveSource> activeSource{ActiveSource::Bundled};
	std::atomic<int> internalRate{10000};
	std::atomic<int> reconstructionMode{int(phonex::ReconstructionMode::Filtered)};
	std::atomic<int> triggerMode{int(phonex::TriggerMode::RetriggerPhrase)};
	std::atomic<int> forcedExcitation{int(phonex::ForcedExcitation::Voiced)};
	std::atomic<std::uint32_t> seed{kDefaultSeed};
	std::atomic<phonex::CompileStatus> textStatus{phonex::CompileStatus::Empty};
	std::atomic<bool> unsupportedUnicode{false};
	std::atomic<int> selectedWord{36};
	std::string submittedText;

	Phonex();
	void process(const ProcessArgs& args) override;
	void onReset(const ResetEvent& event) override;
	json_t* dataToJson() override;
	void dataFromJson(json_t* root) override;

	phonex::TextCompileResult submitText(phonex::StringView text);
	std::string activeDisplayText();

private:
	std::uint32_t observedTextGeneration = 0;
	std::uint32_t appliedSeed = kDefaultSeed;
	int appliedInternalRate = 10000;
	int appliedReconstruction = -1;
	int appliedTriggerMode = int(phonex::TriggerMode::RetriggerPhrase);
	int lastWord = 36;
};
