#include "Moirai.hpp"

#include <iostream>

Plugin* pluginInstance = nullptr;

namespace {
int failures = 0;
void check(bool condition, const char* name) {
	std::cout << (condition ? "[PASS] " : "[FAIL] ") << name << "\n";
	if (!condition) ++failures;
}
void process(Moirai& module, float sampleTime) {
	Module::ProcessArgs args;
	args.sampleTime = sampleTime;
	args.sampleRate = 1.f / sampleTime;
	module.process(args);
}
}

int main() {
	Moirai module;
	// Rack leaves disconnected outputs at zero channels and ignores setChannels().
	// Give the envelope outputs a channel to model the cables used by the host.
	module.outputs[Moirai::A_OUTPUT].channels = 1;
	module.outputs[Moirai::B_OUTPUT].channels = 1;
	module.inputs[Moirai::GATE_INPUT].channels = 2;
	module.inputs[Moirai::GATE_INPUT].setVoltage(10.f, 0);
	module.inputs[Moirai::GATE_INPUT].setVoltage(0.f, 1);
	process(module, 0.004f);
	check(module.outputs[Moirai::A_OUTPUT].getChannels() == 2 &&
		module.outputs[Moirai::B_OUTPUT].getChannels() == 2,
		"Moirai follows the polyphonic GATE channel count on both lanes");
	check(module.outputs[Moirai::A_OUTPUT].getVoltage(0) > 0.f &&
		module.outputs[Moirai::B_OUTPUT].getVoltage(0) > 0.f &&
		module.outputs[Moirai::A_OUTPUT].getVoltage(1) == 0.f,
		"factory ADSR responds independently on both lanes and channels");
	check(module.outputs[Moirai::A_OUTPUT].getVoltage(0) > 0.f,
		"disconnected velocity input uses its neutral 10 V level");

	module.params[Moirai::CHANNEL_PARAM].setValue(3.f);
	module.params[Moirai::MANUAL_TRIGGER_PARAM].setValue(1.f);
	process(module, 0.004f);
	check(module.outputs[Moirai::A_OUTPUT].getChannels() == 4 &&
		module.outputs[Moirai::A_OUTPUT].getVoltage(3) > 0.f,
		"manual trigger temporarily raises polyphony through the selected channel");
	module.params[Moirai::MANUAL_TRIGGER_PARAM].setValue(0.f);
	process(module, 0.001f);

	module.inputs[Moirai::RESET_INPUT].channels = 1;
	module.inputs[Moirai::RESET_INPUT].setVoltage(10.f);
	process(module, 1.f / 48000.f);
	bool reset = true;
	for (int lane = 0; lane < 2; ++lane)
		for (int channel = 0; channel < 4; ++channel)
			reset = reset && !module.envelopeEngine.voice(lane, channel).running;
	check(reset, "RESET immediately idles every active Moirai voice");

	module.authoredBank.revision = 17;
	module.authoredBank.seed = 991;
	module.authoredBank.lanes[0].channelLabels[5] = "fifth voice";
	module.selectedLane.store(1);
	json_t* saved = module.dataToJson();
	Moirai restored;
	restored.dataFromJson(saved);
	check(restored.authoredBank.revision == 17 && restored.authoredBank.seed == 991 &&
		restored.authoredBank.lanes[0].channelLabels[5] == "fifth voice" &&
		restored.compiledBank && restored.compiledBank->revision == 17 &&
		restored.selectedLane.load() == 1,
		"patch state round-trips the authored and compiled bank");
	json_decref(saved);

	json_t* invalidState = module.dataToJson();
	json_object_set_new(json_object_get(invalidState, "bank"), "schemaVersion", json_integer(99));
	restored.dataFromJson(invalidState);
	check(restored.authoredBank.revision == 0 &&
		restored.authoredBank.programs.count("factory_adsr") == 1 &&
		restored.compiledBank && !restored.persistenceError.empty(),
		"invalid patch bank falls back safely and records a load error");
	json_decref(invalidState);

	std::cout << (failures ? "[SUMMARY] moirai_module_spec: FAILED\n"
		: "[SUMMARY] moirai_module_spec: passed\n");
	return failures ? 1 : 0;
}
