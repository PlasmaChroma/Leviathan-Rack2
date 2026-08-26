#include "MoiraiJSON.hpp"
#include "MoiraiPresets.hpp"

#include <iostream>

namespace {
int failures = 0;
void check(bool condition, const char* name) {
	std::cout << (condition ? "[PASS] " : "[FAIL] ") << name << "\n";
	if (!condition) ++failures;
}
bool hasError(const moirai::JsonResult& result, const char* code, const char* path) {
	for (const moirai::ValidationIssue& issue : result.errors)
		if (issue.code == code && issue.path == path) return true;
	return false;
}
}

int main() {
	moirai::Bank source = moirai::makeInitialBank();
	source.revision = 9;
	source.seed = 424242;
	source.clock.onClockLoss = moirai::ClockLossPolicy::FALLBACK;
	source.lanes[0].assignments[3] = "factory_adsr";
	source.lanes[0].channelLabels[3] = "lead";
	source.lanes[1].outputMode = moirai::OutputMode::BIPOLAR_5;
	moirai::Program& staged = source.programs.at("factory_adsr");
	staged.gatePath[0].loopStart = true;
	staged.gatePath[1].loopEnd.mode = moirai::LoopMode::COUNTED;
	staged.gatePath[1].loopEnd.count = 3;
	moirai::MacroBinding binding;
	binding.source = moirai::MacroSource::M2;
	binding.target = moirai::MacroTarget::CURVE_BIAS;
	binding.sampling = moirai::MacroSampling::CONTINUOUS;
	binding.smoothingMs = 12.f;
	staged.macroBindings.push_back(binding);

	json_t* json = moirai::bankToJson(source);
	moirai::JsonResult parsed = moirai::parseBankJson(json);
	check(parsed.valid, "full authored bank parses after serialization");
	check(parsed.bank.revision == 9 && parsed.bank.seed == 424242 &&
		parsed.bank.lanes[0].assignments[3] == "factory_adsr" &&
		parsed.bank.lanes[0].channelLabels[3] == "lead" &&
		parsed.bank.lanes[1].outputMode == moirai::OutputMode::BIPOLAR_5,
		"bank metadata and sparse lane state round-trip");
	const moirai::Program& restored = parsed.bank.programs.at("factory_adsr");
	check(restored.gatePath[0].loopStart &&
		restored.gatePath[1].loopEnd.mode == moirai::LoopMode::COUNTED &&
		restored.gatePath[1].loopEnd.count == 3 && restored.macroBindings.size() == 1 &&
		restored.macroBindings[0].sampling == moirai::MacroSampling::CONTINUOUS,
		"stages, loops, and macro bindings round-trip");
	json_decref(json);

	json_t* unknown = moirai::bankToJson(source);
	json_object_set_new(unknown, "typo", json_true());
	moirai::JsonResult rejectedUnknown = moirai::parseBankJson(unknown);
	check(!rejectedUnknown.valid && hasError(rejectedUnknown, "unknown_field", "/typo"),
		"unknown schema-v1 fields are rejected with a stable path");
	json_decref(unknown);

	json_t* badDuration = moirai::bankToJson(source);
	json_t* program = json_object_get(json_object_get(badDuration, "programs"), "factory_adsr");
	json_t* duration = json_object_get(json_array_get(json_object_get(program, "gatePath"), 0), "duration");
	json_object_set_new(duration, "beats", json_real(0.25));
	moirai::JsonResult rejectedDuration = moirai::parseBankJson(badDuration);
	check(!rejectedDuration.valid && hasError(rejectedDuration, "invalid_duration",
		"/programs/factory_adsr/gatePath/0/duration"), "ambiguous duration units are rejected");
	json_decref(badDuration);

	json_t* missingProgram = moirai::bankToJson(source);
	json_object_set_new(json_object_get(json_object_get(missingProgram, "lanes"), "A"),
		"defaultProgram", json_string("does_not_exist"));
	moirai::JsonResult rejectedReference = moirai::parseBankJson(missingProgram);
	check(!rejectedReference.valid && hasError(rejectedReference, "missing_program", "/lanes/A/defaultProgram"),
		"parsed documents receive full compiler validation");
	json_decref(missingProgram);

	std::cout << (failures ? "[SUMMARY] moirai_json_spec: FAILED\n" : "[SUMMARY] moirai_json_spec: passed\n");
	return failures ? 1 : 0;
}
