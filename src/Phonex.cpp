#include "Phonex.hpp"

#include "PhonexRom.hpp"
#include "PhonexSequenceCompiler.hpp"

#include <algorithm>
#include <array>
#include <cmath>

namespace {

struct BundledSequenceBank {
	std::array<phonex::LpcSequence, phonex::kBundledPhraseCount> sequences{};
	BundledSequenceBank() {
		for (std::uint8_t i = 0; i < sequences.size(); ++i)
			phonex::compileBundledPhrase(i, sequences[i]);
	}
};

const phonex::LpcSequence& bundledSequence(int index) {
	static const BundledSequenceBank bank;
	return bank.sequences[clamp(index, 0, int(phonex::kBundledPhraseCount) - 1)];
}

const char* sourceName(Phonex::ActiveSource source) {
	return source == Phonex::ActiveSource::User ? "User" : "Bundled";
}

int wordFromVoltage(float voltage) {
	if (!std::isfinite(voltage))
		return 0;
	return clamp(int(std::round(clamp(voltage, 0.f, 10.f) * 6.3f)), 0, 63);
}

} // namespace

Phonex::Phonex() {
	for (int slot = 0; slot < kUserBankSize; ++slot) {
		userMailboxes[slot].store(nullptr, std::memory_order_relaxed);
		userSlotAvailable[slot].store(false, std::memory_order_relaxed);
	}
	config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
	configParam(PITCH_PARAM, -2.f, 2.f, 0.f, "Pitch", " oct");
	configParam(FORMANT_PARAM, -1.f, 1.f, 0.f, "Formant");
	configParam(SPEED_PARAM, -4.f, 4.f, 1.f, "Speed", "x");
	configParam(WARP_PARAM, -1.f, 1.f, 0.f, "Warp");
	configParam(EXCITE_BLEND_PARAM, 0.f, 1.f, 0.f, "Excitation blend", "%", 0.f, 100.f);
	configParam(BEND_PARAM, 0.f, 1.f, 0.f, "Bend", "%", 0.f, 100.f);
	configSwitch(GLITCH_PARAM, 0.f, 15.f, 0.f, "Glitch", {
		"CLEAN", "HOLD-4", "SKIP-4", "ADDR-X1", "ADDR-X2", "OFFSET-2",
		"REV-4", "ENERGY-2BIT", "PITCH-8", "PITCH-FOLD", "K-SIGN",
		"K-ROTATE", "K-HOLES", "K-4BIT", "VOICE-FLIP", "BUS-SCRAMBLE"});
	configSwitch(WORD_PARAM, 0.f, 63.f, 36.f, "Word");
	configButton(WORD_PUSH_PARAM, "Speak word");
	configSwitch(BANK_PARAM, 0.f, 1.f, 0.f, "Word bank", {"Stock", "User"});
	configInput(VOCT_INPUT, "V/oct");
	configInput(TRIG_GATE_INPUT, "Trigger / gate");
	configInput(SCRUB_CV_INPUT, "Scrub CV");
	configInput(WARP_CV_INPUT, "Warp CV");
	configInput(BEND_CV_INPUT, "Bend CV");
	configInput(EXT_EXCITE_INPUT, "External excitation");
	configInput(WORD_CV_INPUT, "Word select CV (0 to 10 V)");
	configOutput(AUDIO_OUTPUT, "Audio");
	configOutput(FRAME_CLK_OUTPUT, "Frame clock");
	configOutput(EOX_OUTPUT, "End of utterance");
	configLight(VOICED_LIGHT, "Voiced");
	configLight(FRAME_LIGHT, "Frame");
	configLight(EOX_LIGHT, "End of utterance");
	configLight(BEND_LIGHT, "Bend");
	engine.setSequence(&bundledSequence(36));
	engine.setSeed(kDefaultSeed);
}

void Phonex::process(const ProcessArgs& args) {
	const bool wordCvConnected = inputs[WORD_CV_INPUT].isConnected();
	const int word = wordCvConnected
		? wordFromVoltage(inputs[WORD_CV_INPUT].getVoltage())
		: clamp(int(std::round(params[WORD_PARAM].getValue())), 0, 63);
	selectedWord.store(word, std::memory_order_release);
	const bool userBank = params[BANK_PARAM].getValue() >= 0.5f;
	if (userBank) {
		phonex::SequenceMailbox* mailbox =
			userMailboxes[word].load(std::memory_order_acquire);
		const phonex::LpcSequence* published = mailbox
			? mailbox->acquire(observedUserGenerations[word]) : nullptr;
		if (published)
			userSequences[word] = published;
		const bool available = userSlotAvailable[word].load(std::memory_order_acquire);
		if (word != lastWord || !lastUserBank || published
			|| available != lastUserSlotAvailable) {
			engine.setSequence(available ? userSequences[word] : nullptr);
		}
		activeSource.store(ActiveSource::User, std::memory_order_release);
		lastUserSlotAvailable = available;
	}
	else {
		if (word != lastWord || lastUserBank)
			engine.setSequence(&bundledSequence(word));
		activeSource.store(ActiveSource::Bundled, std::memory_order_release);
		lastUserSlotAvailable = false;
	}
	lastWord = word;
	lastUserBank = userBank;
	const int requestedRate = internalRate.load(std::memory_order_relaxed) < 9000 ? 8000 : 10000;
	if (requestedRate != appliedInternalRate) {
		appliedInternalRate = requestedRate;
		engine.setInternalRate(float(requestedRate));
	}
	const int requestedReconstruction = clamp(reconstructionMode.load(std::memory_order_relaxed), 0, 1);
	if (requestedReconstruction != appliedReconstruction) {
		appliedReconstruction = requestedReconstruction;
		engine.setReconstructionMode(static_cast<phonex::ReconstructionMode>(requestedReconstruction));
	}
	const int requestedTriggerMode = clamp(triggerMode.load(std::memory_order_relaxed), 0, 1);
	if (requestedTriggerMode != appliedTriggerMode) {
		appliedTriggerMode = requestedTriggerMode;
		engine.setTriggerMode(static_cast<phonex::TriggerMode>(requestedTriggerMode));
	}
	const std::uint32_t requestedSeed = seed.load(std::memory_order_relaxed);
	if (requestedSeed != appliedSeed) {
		appliedSeed = requestedSeed;
		engine.setSeed(requestedSeed);
	}

	phonex::EngineControls controls;
	controls.hostSampleRate = args.sampleRate;
	controls.pitchOctaves = params[PITCH_PARAM].getValue();
	controls.voct = inputs[VOCT_INPUT].getVoltage();
	controls.formant = params[FORMANT_PARAM].getValue();
	controls.speed = params[SPEED_PARAM].getValue();
	controls.warp = params[WARP_PARAM].getValue();
	controls.warpCv = inputs[WARP_CV_INPUT].getVoltage();
	controls.exciteBlend = params[EXCITE_BLEND_PARAM].getValue();
	controls.forcedExcitation = static_cast<phonex::ForcedExcitation>(
		clamp(forcedExcitation.load(std::memory_order_relaxed), 0, 1));
	controls.bend = params[BEND_PARAM].getValue();
	controls.bendCv = inputs[BEND_CV_INPUT].getVoltage();
	controls.glitchLevel = static_cast<std::uint8_t>(clamp(
		int(std::round(params[GLITCH_PARAM].getValue())), 0, 15));
	controls.scrubConnected = inputs[SCRUB_CV_INPUT].isConnected();
	controls.scrubVoltage = inputs[SCRUB_CV_INPUT].getVoltage();
	controls.externalConnected = inputs[EXT_EXCITE_INPUT].isConnected();
	controls.externalExcitation = inputs[EXT_EXCITE_INPUT].getVoltage();
	controls.triggerGate = inputs[TRIG_GATE_INPUT].getVoltage() >= 1.f;
	controls.wordPush = params[WORD_PUSH_PARAM].getValue() >= 0.5f;
	const phonex::EngineOutput frame = engine.process(controls);
	outputs[AUDIO_OUTPUT].setVoltage(frame.audio);
	outputs[FRAME_CLK_OUTPUT].setVoltage(frame.framePulse ? 10.f : 0.f);
	outputs[EOX_OUTPUT].setVoltage(frame.eoxPulse ? 10.f : 0.f);
	lights[VOICED_LIGHT].setBrightness(frame.voiced ? 1.f : 0.f);
	lights[FRAME_LIGHT].setBrightness(frame.framePulse ? 1.f : 0.f);
	lights[EOX_LIGHT].setBrightness(frame.eoxPulse ? 1.f : 0.f);
	lights[BEND_LIGHT].setBrightness(clamp(controls.bend + controls.bendCv / 5.f, 0.f, 1.f));
}

phonex::TextCompileResult Phonex::submitText(phonex::StringView text) {
	const int slot = clamp(selectedWord.load(std::memory_order_acquire), 0, kUserBankSize - 1);
	const phonex::TextCompileResult result = storeUserText(slot, text);
	textStatus.store(result.status, std::memory_order_relaxed);
	unsupportedUnicode.store(result.unsupportedUnicode, std::memory_order_relaxed);
	if (result.status == phonex::CompileStatus::Ok
		|| result.status == phonex::CompileStatus::Empty) {
		if (text.empty())
			submittedText.clear();
		else
			submittedText.assign(text.data(), text.size());
		params[BANK_PARAM].setValue(1.f);
		activeSource.store(ActiveSource::User, std::memory_order_release);
		userBankRevision.fetch_add(1u, std::memory_order_relaxed);
	}
	return result;
}

void Phonex::publishUserSequence(int slot, const phonex::LpcSequence& sequence) {
	slot = clamp(slot, 0, kUserBankSize - 1);
	phonex::SequenceMailbox* mailbox = userMailboxes[slot].load(std::memory_order_acquire);
	if (!mailbox) {
		std::unique_ptr<phonex::SequenceMailbox> owner(new phonex::SequenceMailbox());
		mailbox = owner.get();
		mailbox->publish(sequence);
		userMailboxOwners[slot] = std::move(owner);
		userMailboxes[slot].store(mailbox, std::memory_order_release);
	}
	else {
		mailbox->publish(sequence);
	}
	userSlotAvailable[slot].store(true, std::memory_order_release);
}

phonex::TextCompileResult Phonex::storeUserText(int slot, phonex::StringView text) {
	slot = clamp(slot, 0, kUserBankSize - 1);
	phonex::LpcSequence sequence;
	const phonex::TextCompileResult result = phonex::compileText(text, sequence);
	if (result.status == phonex::CompileStatus::Empty) {
		userTexts[slot].clear();
		userSlotAvailable[slot].store(false, std::memory_order_release);
		return result;
	}
	if (result.status != phonex::CompileStatus::Ok)
		return result;
	userTexts[slot].assign(text.data(), text.size());
	publishUserSequence(slot, sequence);
	return result;
}

std::string Phonex::activeDisplayText() {
	const int word = clamp(selectedWord.load(std::memory_order_acquire), 0, 63);
	if (activeSource.load(std::memory_order_relaxed) == ActiveSource::User) {
		const std::string text = userText(word);
		return text.empty() ? "EMPTY" : text;
	}
	const phonex::StringView name = phonex::bundledPhraseName(static_cast<std::uint8_t>(word));
	return std::string(name.data(), name.size());
}

std::string Phonex::userText(int slot) const {
	return userTexts[clamp(slot, 0, kUserBankSize - 1)];
}

bool Phonex::userSlotPopulated(int slot) const {
	return userSlotAvailable[clamp(slot, 0, kUserBankSize - 1)].load(
		std::memory_order_acquire);
}

void Phonex::onReset(const ResetEvent& event) {
	(void) event;
	// Keep reset usable in Rack's headless/module-contract environment too;
	// Module::onReset() routes through the global engine to set parameters.
	params[PITCH_PARAM].setValue(0.f);
	params[FORMANT_PARAM].setValue(0.f);
	params[SPEED_PARAM].setValue(1.f);
	params[WARP_PARAM].setValue(0.f);
	params[EXCITE_BLEND_PARAM].setValue(0.f);
	params[BEND_PARAM].setValue(0.f);
	params[GLITCH_PARAM].setValue(0.f);
	params[WORD_PUSH_PARAM].setValue(0.f);
	params[BANK_PARAM].setValue(0.f);
	activeSource.store(ActiveSource::Bundled, std::memory_order_relaxed);
	internalRate.store(10000, std::memory_order_relaxed);
	reconstructionMode.store(int(phonex::ReconstructionMode::Filtered), std::memory_order_relaxed);
	triggerMode.store(int(phonex::TriggerMode::RetriggerPhrase), std::memory_order_relaxed);
	forcedExcitation.store(int(phonex::ForcedExcitation::Voiced), std::memory_order_relaxed);
	seed.store(kDefaultSeed, std::memory_order_relaxed);
	textStatus.store(phonex::CompileStatus::Empty, std::memory_order_relaxed);
	unsupportedUnicode.store(false, std::memory_order_relaxed);
	submittedText.clear();
	for (int slot = 0; slot < kUserBankSize; ++slot) {
		userTexts[slot].clear();
		userSlotAvailable[slot].store(false, std::memory_order_release);
	}
	userBankRevision.fetch_add(1u, std::memory_order_relaxed);
	lastWord = 36;
	lastUserBank = false;
	lastUserSlotAvailable = false;
	selectedWord.store(36, std::memory_order_relaxed);
	params[WORD_PARAM].setValue(36.f);
	engine.setSequence(&bundledSequence(36));
	engine.setSeed(kDefaultSeed);
}

json_t* Phonex::dataToJson() {
	json_t* root = json_object();
	json_object_set_new(root, "schemaVersion", json_integer(2));
	json_object_set_new(root, "userBankRevision", json_integer(
		userBankRevision.load(std::memory_order_relaxed)));
	json_object_set_new(root, "text", json_stringn(submittedText.data(), submittedText.size()));
	json_object_set_new(root, "activeSource", json_string(sourceName(
		activeSource.load(std::memory_order_relaxed))));
	json_object_set_new(root, "internalRate", json_integer(
		internalRate.load(std::memory_order_relaxed) < 9000 ? 8000 : 10000));
	json_object_set_new(root, "reconstruction", json_string(
		reconstructionMode.load(std::memory_order_relaxed) == int(phonex::ReconstructionMode::Filtered)
			? "Filtered" : "RawHold"));
	json_object_set_new(root, "triggerMode", json_string(
		triggerMode.load(std::memory_order_relaxed) == int(phonex::TriggerMode::AdvanceOneFrame)
			? "AdvanceOneFrame" : "RetriggerPhrase"));
	json_object_set_new(root, "forcedExcitation", json_string(
		forcedExcitation.load(std::memory_order_relaxed) == int(phonex::ForcedExcitation::Unvoiced)
			? "Unvoiced" : "Voiced"));
	json_object_set_new(root, "seed", json_integer(seed.load(std::memory_order_relaxed)));
	json_t* bank = json_array();
	for (const std::string& text : userTexts)
		json_array_append_new(bank, json_stringn(text.data(), text.size()));
	json_object_set_new(root, "userBank", bank);
	return root;
}

void Phonex::dataFromJson(json_t* root) {
	if (!json_is_object(root))
		return;
	int loadedRate = 10000;
	int loadedReconstruction = int(phonex::ReconstructionMode::Filtered);
	int loadedTrigger = int(phonex::TriggerMode::RetriggerPhrase);
	int loadedForced = int(phonex::ForcedExcitation::Voiced);
	std::uint32_t loadedSeed = kDefaultSeed;
	std::uint32_t loadedUserBankRevision = 0;
	ActiveSource loadedSource = ActiveSource::Bundled;
	std::string loadedText;
	json_t* value = json_object_get(root, "internalRate");
	if (json_is_integer(value))
		loadedRate = json_integer_value(value) == 8000 ? 8000 : 10000;
	value = json_object_get(root, "reconstruction");
	if (json_is_string(value)) {
		const std::string reconstruction = json_string_value(value);
		if (reconstruction == "Filtered")
			loadedReconstruction = int(phonex::ReconstructionMode::Filtered);
		else if (reconstruction == "RawHold")
			loadedReconstruction = int(phonex::ReconstructionMode::RawHold);
	}
	value = json_object_get(root, "triggerMode");
	if (json_is_string(value))
		loadedTrigger = std::string(json_string_value(value)) == "AdvanceOneFrame" ? 1 : 0;
	value = json_object_get(root, "forcedExcitation");
	if (json_is_string(value))
		loadedForced = std::string(json_string_value(value)) == "Unvoiced" ? 1 : 0;
	value = json_object_get(root, "seed");
	if (json_is_integer(value))
		loadedSeed = static_cast<std::uint32_t>(json_integer_value(value));
	value = json_object_get(root, "userBankRevision");
	if (json_is_integer(value) && json_integer_value(value) >= 0)
		loadedUserBankRevision = static_cast<std::uint32_t>(json_integer_value(value));
	value = json_object_get(root, "activeSource");
	if (json_is_string(value)) {
		const std::string source = json_string_value(value);
		loadedSource = source == "Text" || source == "User"
			? ActiveSource::User : ActiveSource::Bundled;
	}
	value = json_object_get(root, "text");
	if (json_is_string(value)) {
		const std::size_t length = std::min<std::size_t>(json_string_length(value),
			phonex::kMaxSubmittedTextBytes);
		loadedText.assign(json_string_value(value), length);
	}
	internalRate.store(loadedRate, std::memory_order_relaxed);
	reconstructionMode.store(loadedReconstruction, std::memory_order_relaxed);
	triggerMode.store(loadedTrigger, std::memory_order_relaxed);
	forcedExcitation.store(loadedForced, std::memory_order_relaxed);
	seed.store(loadedSeed, std::memory_order_relaxed);
	userBankRevision.store(loadedUserBankRevision, std::memory_order_relaxed);
	for (int slot = 0; slot < kUserBankSize; ++slot) {
		userTexts[slot].clear();
		userSlotAvailable[slot].store(false, std::memory_order_release);
	}
	bool loadedAnyUserSlot = false;
	value = json_object_get(root, "userBank");
	if (json_is_array(value)) {
		const std::size_t count = std::min<std::size_t>(json_array_size(value), kUserBankSize);
		for (std::size_t slot = 0; slot < count; ++slot) {
			json_t* textValue = json_array_get(value, slot);
			if (!json_is_string(textValue) || json_string_length(textValue) == 0)
				continue;
			const std::size_t length = std::min<std::size_t>(
				json_string_length(textValue), phonex::kMaxSubmittedTextBytes);
			const phonex::TextCompileResult result = storeUserText(
				int(slot), phonex::StringView(json_string_value(textValue), length));
			loadedAnyUserSlot = loadedAnyUserSlot
				|| result.status == phonex::CompileStatus::Ok;
		}
	}
	const int selected = clamp(int(std::round(params[WORD_PARAM].getValue())), 0, 63);
	if (!loadedAnyUserSlot && loadedSource == ActiveSource::User && !loadedText.empty()) {
		const auto result = storeUserText(selected, loadedText);
		loadedAnyUserSlot = result.status == phonex::CompileStatus::Ok;
	}
	if (loadedSource == ActiveSource::User && loadedAnyUserSlot)
		params[BANK_PARAM].setValue(1.f);
	activeSource.store(params[BANK_PARAM].getValue() >= 0.5f
		? ActiveSource::User : ActiveSource::Bundled, std::memory_order_relaxed);
	submittedText = activeSource.load(std::memory_order_relaxed) == ActiveSource::User
		? userTexts[selected] : loadedText;
	lastWord = selected;
	lastUserBank = false;
	lastUserSlotAvailable = false;
	selectedWord.store(lastWord, std::memory_order_relaxed);
	if (activeSource.load(std::memory_order_relaxed) == ActiveSource::Bundled)
		engine.setSequence(&bundledSequence(lastWord));
}
