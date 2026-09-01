#include "../src/Phonex.hpp"

#include <cmath>
#include <iostream>
#include <string>

Plugin* pluginInstance = nullptr;

namespace {

struct Tests {
	int checks = 0;
	int failures = 0;
	void expect(bool condition, const std::string& name) {
		++checks;
		if (!condition) {
			++failures;
			std::cerr << "[FAIL] " << name << '\n';
		}
	}
};

Module::ProcessArgs args(float rate = 48000.f) {
	Module::ProcessArgs value;
	value.sampleRate = rate;
	value.sampleTime = 1.f / rate;
	return value;
}

json_t* semanticRequest(Phonex& module, OctaviaSemanticControl::Operation operation,
		const std::string& request, bool& handled, std::string* errorOut = nullptr) {
	std::string response;
	std::string error;
	handled = module.handleSemanticRequest(operation, request, response, error);
	if (errorOut)
		*errorOut = error;
	json_error_t parseError{};
	return json_loads(response.c_str(), 0, &parseError);
}

void interfaceContract(Tests& tests) {
	static_assert(Phonex::PITCH_PARAM == 0 && Phonex::WORD_PUSH_PARAM == 8
		&& Phonex::BANK_PARAM == 9,
		"frozen PHONEX parameter IDs");
	static_assert(Phonex::VOCT_INPUT == 0 && Phonex::EXT_EXCITE_INPUT == 5
		&& Phonex::WORD_CV_INPUT == 6,
		"frozen PHONEX input IDs");
	static_assert(Phonex::AUDIO_OUTPUT == 0 && Phonex::EOX_OUTPUT == 2,
		"frozen PHONEX output IDs");
	static_assert(Phonex::VOICED_LIGHT == 0 && Phonex::BEND_LIGHT == 3,
		"frozen PHONEX light IDs");
	Phonex module;
	tests.expect(module.getNumParams() == 10 && module.getNumInputs() == 7
		&& module.getNumOutputs() == 3 && module.getNumLights() == 4,
		"exact Rack interface counts");
	ParamQuantity* pitch = module.paramQuantities[Phonex::PITCH_PARAM];
	ParamQuantity* formant = module.paramQuantities[Phonex::FORMANT_PARAM];
	ParamQuantity* speed = module.paramQuantities[Phonex::SPEED_PARAM];
	ParamQuantity* word = module.paramQuantities[Phonex::WORD_PARAM];
	ParamQuantity* bank = module.paramQuantities[Phonex::BANK_PARAM];
	tests.expect(pitch->getMinValue() == -2.f && pitch->getMaxValue() == 2.f
		&& module.params[Phonex::PITCH_PARAM].getValue() == 0.f,
		"PITCH range/default");
	tests.expect(formant->getMinValue() == -1.f && formant->getMaxValue() == 1.f
		&& module.params[Phonex::FORMANT_PARAM].getValue() == 0.f,
		"FORMANT range/default");
	tests.expect(speed->getMinValue() == -4.f && speed->getMaxValue() == 4.f
		&& module.params[Phonex::SPEED_PARAM].getValue() == 1.f,
		"SPEED range/default");
	tests.expect(dynamic_cast<SwitchQuantity*>(word) != nullptr
		&& word->snapEnabled && !word->smoothEnabled
		&& word->getMinValue() == 0.f && word->getMaxValue() == 63.f
		&& module.params[Phonex::WORD_PARAM].getValue() == 36.f,
		"WORD frozen range/default and ratcheting switch behavior");
	tests.expect(module.paramQuantities[Phonex::GLITCH_PARAM]->snapEnabled,
		"GLITCH is snapped");
	tests.expect(dynamic_cast<SwitchQuantity*>(bank) != nullptr
		&& bank->getMinValue() == 0.f && bank->getMaxValue() == 1.f
		&& module.params[Phonex::BANK_PARAM].getValue() == 0.f,
		"BANK is a Stock/User switch defaulting to Stock");
	tests.expect(module.reconstructionMode.load() == int(phonex::ReconstructionMode::Filtered),
		"filtered reconstruction is the clean module default");
}

void processingContract(Tests& tests) {
	Phonex module;
	const auto processArgs = args();
	float peak = 0.f;
	for (int i = 0; i < 4800; ++i) {
		module.process(processArgs);
		peak = std::max(peak, std::abs(module.outputs[Phonex::AUDIO_OUTPUT].getVoltage()));
	}
	tests.expect(peak > 0.1f && peak <= 5.f, "default HELLO produces bounded audio");
	module.params[Phonex::BEND_PARAM].setValue(0.25f);
	module.inputs[Phonex::BEND_CV_INPUT].channels = 1;
	module.inputs[Phonex::BEND_CV_INPUT].setVoltage(2.5f);
	module.process(processArgs);
	tests.expect(std::abs(module.lights[Phonex::BEND_LIGHT].getBrightness() - 0.75f) < 1e-6f,
		"BEND CV maps +/-5 V to additive unity range");

	const auto submitted = module.submitText("robot");
	module.process(processArgs);
	tests.expect(submitted.status == phonex::CompileStatus::Ok
		&& module.activeSource.load() == Phonex::ActiveSource::User
		&& module.params[Phonex::BANK_PARAM].getValue() == 1.f
		&& module.activeDisplayText() == "robot", "text submission stores and selects User");
	module.params[Phonex::WORD_PARAM].setValue(12.f);
	module.process(processArgs);
	tests.expect(module.activeDisplayText() == "EMPTY"
		&& !module.userSlotPopulated(12), "unfilled User slots are empty");
	const auto secondSubmitted = module.submitText("dragon king");
	module.process(processArgs);
	tests.expect(secondSubmitted.status == phonex::CompileStatus::Ok
		&& module.userText(12) == "dragon king"
		&& module.activeDisplayText() == "dragon king",
		"a second User slot stores independently");
	module.params[Phonex::WORD_PARAM].setValue(36.f);
	module.process(processArgs);
	tests.expect(module.activeDisplayText() == "robot",
		"returning to a User slot recalls its entry");
	module.inputs[Phonex::WORD_CV_INPUT].channels = 1;
	module.inputs[Phonex::WORD_CV_INPUT].setVoltage(12.f / 6.3f);
	module.process(processArgs);
	tests.expect(module.activeSource.load() == Phonex::ActiveSource::User
		&& module.activeDisplayText() == "dragon king",
		"WORD CV sequences entries in the User bank");
	module.params[Phonex::BANK_PARAM].setValue(0.f);
	module.inputs[Phonex::WORD_CV_INPUT].setVoltage(36.f / 6.3f);
	module.process(processArgs);
	tests.expect(module.activeSource.load() == Phonex::ActiveSource::Bundled
		&& module.activeDisplayText() == "HELLO",
		"Stock bank and WORD CV select the bundled phrase at the same index");
	module.inputs[Phonex::WORD_CV_INPUT].channels = 0;
	module.params[Phonex::WORD_PARAM].setValue(38.f);
	module.process(processArgs);
	tests.expect(module.activeSource.load() == Phonex::ActiveSource::Bundled
		&& module.activeDisplayText() == "SPEAK", "turning WORD selects Bundled");

	module.inputs[Phonex::WORD_CV_INPUT].channels = 1;
	module.inputs[Phonex::WORD_CV_INPUT].setVoltage(10.f);
	module.process(processArgs);
	tests.expect(module.activeSource.load() == Phonex::ActiveSource::Bundled
		&& module.activeDisplayText() == "PHONEX"
		&& module.params[Phonex::WORD_PARAM].getValue() == 38.f,
		"WORD CV maps 10 V directly to word 63 without moving the knob");
	module.inputs[Phonex::WORD_CV_INPUT].setVoltage(0.f);
	module.process(processArgs);
	tests.expect(module.activeDisplayText() == "A",
		"WORD CV maps 0 V directly to word 0");
	module.inputs[Phonex::WORD_CV_INPUT].setVoltage(-10.f);
	module.process(processArgs);
	tests.expect(module.activeDisplayText() == "A",
		"WORD CV clamps negative voltages to word 0");
	module.inputs[Phonex::WORD_CV_INPUT].channels = 0;
	module.process(processArgs);
	tests.expect(module.activeDisplayText() == "SPEAK",
		"disconnecting WORD CV restores knob selection");

	module.params[Phonex::SPEED_PARAM].setValue(0.f);
	module.params[Phonex::WORD_PUSH_PARAM].setValue(0.f);
	module.process(processArgs);
	module.params[Phonex::WORD_PUSH_PARAM].setValue(1.f);
	module.process(processArgs);
	tests.expect(module.engine.position() == 0.f, "WORD push retriggers active source");
	module.params[Phonex::WORD_PUSH_PARAM].setValue(0.f);

	module.params[Phonex::SPEED_PARAM].setValue(1.f);
	int framePulseSamples = 0;
	for (int i = 0; i < 1100; ++i) {
		module.process(processArgs);
		if (module.outputs[Phonex::FRAME_CLK_OUTPUT].getVoltage() > 1.f)
			++framePulseSamples;
	}
	tests.expect(framePulseSamples == 48, "FRAME_CLK Rack output pulse lasts 1 ms");
}

void persistenceContract(Tests& tests) {
	Phonex source;
	source.params[Phonex::WORD_PARAM].setValue(12.f);
	source.process(args());
	source.submitText("dragon king");
	source.params[Phonex::WORD_PARAM].setValue(36.f);
	source.process(args());
	source.submitText("Leviathan 42");
	source.internalRate.store(8000);
	source.reconstructionMode.store(int(phonex::ReconstructionMode::Filtered));
	source.triggerMode.store(int(phonex::TriggerMode::AdvanceOneFrame));
	source.forcedExcitation.store(int(phonex::ForcedExcitation::Unvoiced));
	source.seed.store(0xfedcba98u);
	json_t* saved = source.dataToJson();
	Phonex loaded;
	loaded.dataFromJson(saved);
	json_decref(saved);
	loaded.process(args());
	tests.expect(loaded.activeSource.load() == Phonex::ActiveSource::User
		&& loaded.submittedText == "Leviathan 42"
		&& loaded.userText(12) == "dragon king"
		&& loaded.userText(36) == "Leviathan 42"
		&& loaded.userBankRevision.load() == source.userBankRevision.load()
		&& loaded.params[Phonex::BANK_PARAM].getValue() == 1.f
		&& loaded.internalRate.load() == 8000
		&& loaded.reconstructionMode.load() == int(phonex::ReconstructionMode::Filtered)
		&& loaded.triggerMode.load() == int(phonex::TriggerMode::AdvanceOneFrame)
		&& loaded.forcedExcitation.load() == int(phonex::ForcedExcitation::Unvoiced)
		&& loaded.seed.load() == 0xfedcba98u,
		"JSON schema v2 preserves the User bank and engine settings");

	json_t* malformed = json_object();
	json_object_set_new(malformed, "activeSource", json_string("Bogus"));
	json_object_set_new(malformed, "internalRate", json_integer(123));
	json_object_set_new(malformed, "reconstruction", json_string("Bogus"));
	json_object_set_new(malformed, "triggerMode", json_string("Bogus"));
	json_object_set_new(malformed, "forcedExcitation", json_string("Bogus"));
	Phonex safe;
	safe.dataFromJson(malformed);
	json_decref(malformed);
	tests.expect(safe.activeSource.load() == Phonex::ActiveSource::Bundled
		&& safe.internalRate.load() == 10000
		&& safe.reconstructionMode.load() == int(phonex::ReconstructionMode::Filtered)
		&& safe.triggerMode.load() == int(phonex::TriggerMode::RetriggerPhrase)
		&& safe.forcedExcitation.load() == int(phonex::ForcedExcitation::Voiced),
		"malformed JSON clamps to safe settings");

	Module::ResetEvent reset;
	loaded.params[Phonex::PITCH_PARAM].setValue(1.5f);
	loaded.params[Phonex::FORMANT_PARAM].setValue(-0.8f);
	loaded.params[Phonex::SPEED_PARAM].setValue(-3.f);
	loaded.params[Phonex::BEND_PARAM].setValue(0.8f);
	loaded.onReset(reset);
	tests.expect(loaded.activeSource.load() == Phonex::ActiveSource::Bundled
		&& loaded.internalRate.load() == 10000
		&& loaded.seed.load() == Phonex::kDefaultSeed
		&& loaded.params[Phonex::PITCH_PARAM].getValue() == 0.f
		&& loaded.params[Phonex::FORMANT_PARAM].getValue() == 0.f
		&& loaded.params[Phonex::SPEED_PARAM].getValue() == 1.f
		&& loaded.params[Phonex::BEND_PARAM].getValue() == 0.f
		&& loaded.params[Phonex::WORD_PARAM].getValue() == 36.f
		&& loaded.params[Phonex::BANK_PARAM].getValue() == 0.f
		&& !loaded.userSlotPopulated(12) && !loaded.userSlotPopulated(36),
		"reset restores PHONEX defaults");
}

void semanticBankContract(Tests& tests) {
	Phonex module;
	bool handled = false;
	json_t* response = semanticRequest(module,
		OctaviaSemanticControl::Operation::CAPABILITIES, "{}", handled);
	json_t* capabilities = response ? json_object_get(response, "capabilities") : nullptr;
	json_t* requestSchemas = capabilities
		? json_object_get(capabilities, "requestSchemas") : nullptr;
	json_t* editSchema = requestSchemas
		? json_object_get(requestSchemas, "edit") : nullptr;
	json_t* editProperties = editSchema
		? json_object_get(editSchema, "properties") : nullptr;
	json_t* editOperations = editProperties
		? json_object_get(editProperties, "operations") : nullptr;
	json_t* operationItems = editOperations
		? json_object_get(editOperations, "items") : nullptr;
	tests.expect(handled && response
		&& std::string(json_string_value(json_object_get(response, "capabilityId")))
			== "leviathan.phonex.word-bank"
		&& json_integer_value(json_object_get(capabilities, "slotCount")) == 64,
		"generic semantic capability discovers the Phonex word bank");
	tests.expect(json_is_object(requestSchemas) && json_is_object(editSchema)
		&& std::string(json_string_value(json_object_get(editSchema, "$schema")))
			== "https://json-schema.org/draft/2020-12/schema"
		&& json_array_size(json_object_get(operationItems, "oneOf")) == 3
		&& json_array_size(json_object_get(editSchema, "examples")) >= 1,
		"Phonex capability describes edit requests with machine-readable schemas and examples");
	json_decref(response);

	const std::string candidate =
		"{\"operations\":[{\"op\":\"set_slot\",\"slot\":12,\"text\":\"dragon king\"}]}";
	response = semanticRequest(module,
		OctaviaSemanticControl::Operation::VALIDATE, candidate, handled);
	tests.expect(handled && response
		&& json_is_true(json_object_get(response, "valid"))
		&& module.userText(12).empty() && module.userBankRevision.load() == 0,
		"semantic validation compiles without changing the bank");
	json_decref(response);

	const std::string initialEdit =
		"{\"expectedRevision\":0,\"operations\":["
		"{\"op\":\"clear_bank\"},"
		"{\"op\":\"set_slot\",\"slot\":12,\"text\":\"dragon king\"},"
		"{\"op\":\"set_slot\",\"slot\":36,\"text\":\"robot voice\"}]}";
	response = semanticRequest(module,
		OctaviaSemanticControl::Operation::EDIT, initialEdit, handled);
	tests.expect(handled && response && json_is_true(json_object_get(response, "ok"))
		&& json_integer_value(json_object_get(response, "revision")) == 1
		&& module.userText(12) == "dragon king"
		&& module.userText(36) == "robot voice"
		&& module.userBankRevision.load() == 1,
		"semantic edit atomically loads a multi-slot test bank");
	json_decref(response);

	response = semanticRequest(module,
		OctaviaSemanticControl::Operation::GET_DOCUMENT,
		"{\"view\":\"summary\"}", handled);
	tests.expect(handled && response
		&& json_integer_value(json_object_get(response, "populatedCount")) == 2
		&& json_array_size(json_object_get(response, "entries")) == 2,
		"semantic summary returns only populated test entries");
	json_decref(response);

	response = semanticRequest(module,
		OctaviaSemanticControl::Operation::GET_DOCUMENT,
		"{\"view\":\"slot\",\"id\":\"12\"}", handled);
	tests.expect(handled && response
		&& std::string(json_string_value(json_object_get(response, "text"))) == "dragon king",
		"semantic slot view reads one user-bank entry");
	json_decref(response);

	const std::string staleEdit =
		"{\"expectedRevision\":0,\"operations\":[{\"op\":\"clear_bank\"}]}";
	response = semanticRequest(module,
		OctaviaSemanticControl::Operation::EDIT, staleEdit, handled);
	json_t* issue = response ? json_object_get(response, "error") : nullptr;
	tests.expect(!handled && response
		&& std::string(json_string_value(json_object_get(issue, "code"))) == "revision_conflict"
		&& module.userText(12) == "dragon king" && module.userBankRevision.load() == 1,
		"stale semantic revisions reject without changing the bank");
	json_decref(response);

	const std::string invalidEdit =
		"{\"expectedRevision\":1,\"operations\":["
		"{\"op\":\"clear_slot\",\"slot\":12},"
		"{\"op\":\"set_slot\",\"slot\":36,\"text\":\"[BOGUS]\"}]}";
	response = semanticRequest(module,
		OctaviaSemanticControl::Operation::EDIT, invalidEdit, handled);
	issue = response ? json_object_get(response, "error") : nullptr;
	tests.expect(!handled && response
		&& std::string(json_string_value(json_object_get(issue, "code"))) == "compile_failed"
		&& module.userText(12) == "dragon king"
		&& module.userText(36) == "robot voice"
		&& module.userBankRevision.load() == 1,
		"failed semantic compilation rolls back the entire transaction");
	json_decref(response);

	const std::string clearEdit =
		"{\"expectedRevision\":1,\"operations\":[{\"op\":\"clear_slot\",\"slot\":12}]}";
	response = semanticRequest(module,
		OctaviaSemanticControl::Operation::EDIT, clearEdit, handled);
	tests.expect(handled && response && module.userText(12).empty()
		&& module.userText(36) == "robot voice"
		&& module.userBankRevision.load() == 2,
		"semantic clear_slot removes only its selected entry");
	json_decref(response);
}

} // namespace

int main() {
	Tests tests;
	interfaceContract(tests);
	processingContract(tests);
	persistenceContract(tests);
	semanticBankContract(tests);
	std::cout << "[TEST SUMMARY] checks=" << tests.checks
		<< " failures=" << tests.failures << '\n';
	return tests.failures == 0 ? 0 : 1;
}
