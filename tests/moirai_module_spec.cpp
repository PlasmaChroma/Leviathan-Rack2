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
json_t* request(Moirai& module, OctaviaSemanticControl::Operation operation,
		const char* body, bool& handled) {
	std::string response;
	std::string error;
	handled = module.handleSemanticRequest(operation, body, response, error);
	json_error_t parseError {};
	return json_loads(response.c_str(), 0, &parseError);
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

	Moirai semantic;
	semantic.outputs[Moirai::A_OUTPUT].channels = 1;
	semantic.outputs[Moirai::B_OUTPUT].channels = 1;
	bool handled = false;
	json_t* capabilities = request(semantic, OctaviaSemanticControl::Operation::CAPABILITIES, "{}", handled);
	check(handled && capabilities && json_is_true(json_object_get(capabilities, "ok")) &&
		std::string(json_string_value(json_object_get(capabilities, "capabilityId"))) ==
			"leviathan.moirai.envelope-bank",
		"Moirai advertises the generic semantic capability identity");
	json_decref(capabilities);
	json_t* programView = request(semantic, OctaviaSemanticControl::Operation::GET_DOCUMENT,
		R"({"view":"program","id":"factory_adsr"})", handled);
	check(handled && programView && std::string(json_string_value(json_object_get(programView, "id"))) == "factory_adsr",
		"semantic program view returns one complete authored program");
	json_decref(programView);
	json_t* channelView = request(semantic, OctaviaSemanticControl::Operation::GET_DOCUMENT,
		R"({"view":"channel","id":"0"})", handled);
	check(handled && channelView && json_integer_value(json_object_get(channelView, "channel")) == 0 &&
		json_is_object(json_object_get(channelView, "lanes")),
		"semantic channel view returns both lane assignments");
	json_decref(channelView);

	json_t* edited = request(semantic, OctaviaSemanticControl::Operation::EDIT,
		R"({"expected_revision":0,"apply_at":"nextTrigger","active_voice_policy":"finishCurrent","operations":[{"op":"set_clock","clock":{"fallbackBpm":98}}]})", handled);
	check(handled && edited && json_integer_value(json_object_get(edited, "acceptedRevision")) == 1 &&
		json_integer_value(json_object_get(edited, "activeRevision")) == 0 &&
		json_integer_value(json_object_get(edited, "pendingRevision")) == 1,
		"semantic edit commits one accepted generation and reports pending adoption");
	json_decref(edited);
	semantic.inputs[Moirai::GATE_INPUT].channels = 1;
	semantic.inputs[Moirai::GATE_INPUT].setVoltage(10.f);
	process(semantic, 1.f / 48000.f);
	json_t* status = request(semantic, OctaviaSemanticControl::Operation::GET_STATUS, "{}", handled);
	check(handled && status && json_integer_value(json_object_get(status, "activeRevision")) == 1 &&
		json_is_null(json_object_get(status, "pendingRevision")),
		"nextTrigger edit adopts before the triggering audio sample");
	json_decref(status);

	json_t* conflict = request(semantic, OctaviaSemanticControl::Operation::EDIT,
		R"({"expected_revision":0,"apply_at":"immediate","active_voice_policy":"finishCurrent","operations":[{"op":"set_clock","clock":{"fallbackBpm":110}}]})", handled);
	check(!handled && conflict && std::string(json_string_value(json_object_get(
		json_object_get(conflict, "error"), "code"))) == "revision_conflict",
		"semantic revision conflicts reject without committing");
	json_decref(conflict);

	semantic.inputs[Moirai::GATE_INPUT].setVoltage(0.f);
	process(semantic, 1.f / 48000.f);
	json_t* command = request(semantic, OctaviaSemanticControl::Operation::COMMAND,
		R"({"action":"trigger","lane":"A","channel":3})", handled);
	process(semantic, 1.f / 48000.f);
	check(handled && command && semantic.envelopeEngine.voice(0, 3).running &&
		!semantic.envelopeEngine.voice(1, 3).running,
		"semantic trigger command crosses atomically to the selected audio-thread voice");
	json_decref(command);

	std::cout << (failures ? "[SUMMARY] moirai_module_spec: FAILED\n"
		: "[SUMMARY] moirai_module_spec: passed\n");
	return failures ? 1 : 0;
}
