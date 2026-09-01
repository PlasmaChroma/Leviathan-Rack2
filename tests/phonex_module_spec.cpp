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

void interfaceContract(Tests& tests) {
	static_assert(Phonex::PITCH_PARAM == 0 && Phonex::WORD_PUSH_PARAM == 8,
		"frozen PHONEX parameter IDs");
	static_assert(Phonex::VOCT_INPUT == 0 && Phonex::EXT_EXCITE_INPUT == 5
		&& Phonex::WORD_CV_INPUT == 6,
		"frozen PHONEX input IDs");
	static_assert(Phonex::AUDIO_OUTPUT == 0 && Phonex::EOX_OUTPUT == 2,
		"frozen PHONEX output IDs");
	static_assert(Phonex::VOICED_LIGHT == 0 && Phonex::BEND_LIGHT == 3,
		"frozen PHONEX light IDs");
	Phonex module;
	tests.expect(module.getNumParams() == 9 && module.getNumInputs() == 7
		&& module.getNumOutputs() == 3 && module.getNumLights() == 4,
		"exact Rack interface counts");
	ParamQuantity* pitch = module.paramQuantities[Phonex::PITCH_PARAM];
	ParamQuantity* formant = module.paramQuantities[Phonex::FORMANT_PARAM];
	ParamQuantity* speed = module.paramQuantities[Phonex::SPEED_PARAM];
	ParamQuantity* word = module.paramQuantities[Phonex::WORD_PARAM];
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
		&& module.activeSource.load() == Phonex::ActiveSource::Text
		&& module.activeDisplayText() == "robot", "text submission publishes and selects Text");
	module.inputs[Phonex::WORD_CV_INPUT].channels = 1;
	module.inputs[Phonex::WORD_CV_INPUT].setVoltage(36.f / 6.3f);
	module.process(processArgs);
	tests.expect(module.activeSource.load() == Phonex::ActiveSource::Bundled
		&& module.activeDisplayText() == "HELLO",
		"connecting WORD CV overrides Text even when it selects the previous word");
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
	tests.expect(loaded.activeSource.load() == Phonex::ActiveSource::Text
		&& loaded.submittedText == "Leviathan 42"
		&& loaded.internalRate.load() == 8000
		&& loaded.reconstructionMode.load() == int(phonex::ReconstructionMode::Filtered)
		&& loaded.triggerMode.load() == int(phonex::TriggerMode::AdvanceOneFrame)
		&& loaded.forcedExcitation.load() == int(phonex::ForcedExcitation::Unvoiced)
		&& loaded.seed.load() == 0xfedcba98u,
		"JSON schema v1 round trip");

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
		&& loaded.params[Phonex::WORD_PARAM].getValue() == 36.f,
		"reset restores PHONEX defaults");
}

} // namespace

int main() {
	Tests tests;
	interfaceContract(tests);
	processingContract(tests);
	persistenceContract(tests);
	std::cout << "[TEST SUMMARY] checks=" << tests.checks
		<< " failures=" << tests.failures << '\n';
	return tests.failures == 0 ? 0 : 1;
}
