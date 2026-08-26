#include "MoiraiEdit.hpp"
#include "MoiraiPresets.hpp"

#include <jansson.h>
#include <iostream>

namespace {
int failures = 0;
void check(bool value, const char* name) {
	std::cout << (value ? "[PASS] " : "[FAIL] ") << name << "\n";
	if (!value) ++failures;
}

moirai::EditResult edit(const moirai::Bank& bank, const char* text) {
	json_error_t error {};
	json_t* request = json_loads(text, 0, &error);
	if (!request) return {};
	moirai::EditResult result = moirai::applyBankEdit(bank, request);
	json_decref(request);
	return result;
}
}

int main() {
	const moirai::Bank base = moirai::makeInitialBank();
	const auto changed = edit(base, R"JSON({
		"expected_revision":0,
		"apply_at":"nextTrigger",
		"active_voice_policy":"finishCurrent",
		"operations":[
			{"op":"apply_preset","preset_id":"factory_pluck","id":"bass_pluck"},
			{"op":"clone_program","source_id":"bass_pluck","id":"filter_pluck","name":"Filter Pluck"},
			{"op":"assign_program","lane":"A","channel":0,"program":"bass_pluck"},
			{"op":"assign_program","lane":"B","channel":0,"program":"filter_pluck"},
			{"op":"set_channel_label","lane":"A","channel":0,"label":"Bass"},
			{"op":"set_output_mode","lane":"B","mode":"0_5"},
			{"op":"set_clock","clock":{"fallbackBpm":98.0}}
		]
	})JSON");
	check(changed.valid && changed.bank.revision == 1 && changed.compiledBank
		&& changed.compiledBank->revision == 1,
		"successful ordered transaction increments and compiles exactly one revision");
	check(changed.valid && changed.applyAt == moirai::ApplyAt::NEXT_TRIGGER
		&& changed.activeVoicePolicy == moirai::ActiveVoicePolicy::FINISH_CURRENT,
		"adoption and active-voice policies are parsed with the transaction");
	check(changed.valid && changed.bank.programs.count("bass_pluck")
		&& changed.bank.programs.at("filter_pluck").name == "Filter Pluck"
		&& changed.bank.lanes[0].assignments[0] == "bass_pluck"
		&& changed.bank.lanes[1].assignments[0] == "filter_pluck"
		&& changed.bank.lanes[0].channelLabels[0] == "Bass"
		&& changed.bank.lanes[1].outputMode == moirai::OutputMode::UNIPOLAR_5
		&& changed.bank.clock.fallbackBpm == 98.f,
		"preset, clone, assignment, label, lane output, and clock operations compose in order");

	const auto conflict = edit(base, R"({"expected_revision":4,"apply_at":"immediate","active_voice_policy":"finishCurrent","operations":[{"op":"set_clock","clock":{"fallbackBpm":90}}]})");
	check(!conflict.valid && conflict.errorCode == "revision_conflict"
		&& conflict.currentRevision == 0,
		"revision conflict reports the accepted revision and performs no work");

	const auto rollback = edit(base, R"({"expected_revision":0,"apply_at":"immediate","active_voice_policy":"finishCurrent","operations":[{"op":"apply_preset","preset_id":"factory_pluck","id":"temporary"},{"op":"assign_program","lane":"A","channel":99,"program":"temporary"}]})");
	check(!rollback.valid && rollback.errorCode == "invalid_operation"
		&& base.programs.count("temporary") == 0 && base.revision == 0,
		"later operation failure rolls back the entire private transaction");

	const auto inUse = edit(base, R"({"expected_revision":0,"apply_at":"immediate","active_voice_policy":"finishCurrent","operations":[{"op":"delete_program","id":"factory_adsr"}]})");
	check(!inUse.valid && inUse.errorCode == "object_in_use",
		"program deletion rejects lane references before final compilation");

	const auto invalidFinal = edit(base, R"({"expected_revision":0,"apply_at":"allIdle","active_voice_policy":"restartActive","operations":[{"op":"set_clock","clock":{"fallbackBpm":900}}]})");
	check(!invalidFinal.valid && invalidFinal.errorCode == "validation_failed"
		&& base.clock.fallbackBpm == 120.f,
		"full-bank validation failure leaves the accepted bank untouched");

	const auto unknown = edit(base, R"({"expected_revision":0,"apply_at":"immediate","active_voice_policy":"finishCurrent","operations":[{"op":"warp_fate"}]})");
	check(!unknown.valid && unknown.errorCode == "unknown_operation"
		&& unknown.errorPath == "/operations/0/op",
		"unknown operations fail with a stable indexed path");

	std::cout << "[SUMMARY] moirai_edit_spec: " << (failures ? "FAILED" : "passed") << "\n";
	return failures == 0 ? 0 : 1;
}
