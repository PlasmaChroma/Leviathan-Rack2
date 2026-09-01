#pragma once

#include "PhonexEngine.hpp"
#include "PhonexPronunciation.hpp"
#include "PhonexSequenceMailbox.hpp"
#include "OctaviaSemanticControl.hpp"
#include "plugin.hpp"

#include <atomic>
#include <array>
#include <cstdint>
#include <memory>
#include <string>

struct Phonex final : Module, OctaviaSemanticControl {
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
		BANK_PARAM,
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

	enum class ActiveSource : std::uint8_t {
		Bundled = 0,
		User,
		Text = User, // Legacy source name retained for schema-v1 compatibility.
	};

	static constexpr std::uint32_t kDefaultSeed = 0x50484f4eu;
	static constexpr int kUserBankSize = 64;

	phonex::Engine engine;
	std::atomic<ActiveSource> activeSource{ActiveSource::Bundled};
	std::atomic<int> internalRate{10000};
	std::atomic<int> reconstructionMode{int(phonex::ReconstructionMode::Filtered)};
	std::atomic<int> triggerMode{int(phonex::TriggerMode::RetriggerPhrase)};
	std::atomic<int> forcedExcitation{int(phonex::ForcedExcitation::Voiced)};
	std::atomic<std::uint32_t> seed{kDefaultSeed};
	std::atomic<phonex::CompileStatus> textStatus{phonex::CompileStatus::Empty};
	std::atomic<bool> unsupportedUnicode{false};
	std::atomic<int> selectedWord{36};
	std::atomic<std::uint32_t> userBankRevision{0};
	std::string submittedText;
	std::array<std::string, kUserBankSize> userTexts;

	Phonex();
	void process(const ProcessArgs& args) override;
	void onReset(const ResetEvent& event) override;
	json_t* dataToJson() override;
	void dataFromJson(json_t* root) override;

	phonex::TextCompileResult submitText(phonex::StringView text);
	std::string activeDisplayText();
	std::string userText(int slot) const;
	bool userSlotPopulated(int slot) const;
	const char* semanticCapabilityId() const noexcept override {
		return "leviathan.phonex.word-bank";
	}
	bool handleSemanticRequest(OctaviaSemanticControl::Operation operation,
		const std::string& requestJson, std::string& responseJson,
		std::string& error) override;

private:
	std::array<std::unique_ptr<phonex::SequenceMailbox>, kUserBankSize> userMailboxOwners;
	std::array<std::atomic<phonex::SequenceMailbox*>, kUserBankSize> userMailboxes;
	std::array<std::atomic<bool>, kUserBankSize> userSlotAvailable;
	std::array<std::uint32_t, kUserBankSize> observedUserGenerations{};
	std::array<const phonex::LpcSequence*, kUserBankSize> userSequences{};
	std::uint32_t appliedSeed = kDefaultSeed;
	int appliedInternalRate = 10000;
	int appliedReconstruction = -1;
	int appliedTriggerMode = int(phonex::TriggerMode::RetriggerPhrase);
	int lastWord = 36;
	bool lastUserBank = false;
	bool lastUserSlotAvailable = false;

	void publishUserSequence(int slot, const phonex::LpcSequence& sequence);
	phonex::TextCompileResult storeUserText(int slot, phonex::StringView text);
};
