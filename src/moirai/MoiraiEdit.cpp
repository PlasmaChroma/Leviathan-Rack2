#include "MoiraiEdit.hpp"

#include "MoiraiCompiler.hpp"
#include "MoiraiJSON.hpp"
#include "MoiraiPresets.hpp"

#include <cstdlib>

namespace moirai {
namespace {

bool fail(EditResult& result, const std::string& code, const std::string& path,
		const std::string& message) {
	result.errorCode = code;
	result.errorPath = path;
	result.errorMessage = message;
	return false;
}

const char* stringField(json_t* object, const char* key) {
	json_t* value = json_object_get(object, key);
	return json_is_string(value) ? json_string_value(value) : nullptr;
}

bool laneIndex(json_t* operation, int& lane, EditResult& result,
		const std::string& path) {
	const char* name = stringField(operation, "lane");
	if (!name || (std::string(name) != "A" && std::string(name) != "B"))
		return fail(result, "invalid_operation", path + "/lane", "lane must be A or B");
	lane = std::string(name) == "A" ? 0 : 1;
	return true;
}

json_t* lanesObject(json_t* bank) { return json_object_get(bank, "lanes"); }
json_t* programsObject(json_t* bank) { return json_object_get(bank, "programs"); }

bool referencedProgram(json_t* bank, const std::string& id, std::string& reference) {
	json_t* lanes = lanesObject(bank);
	for (const char* laneName : {"A", "B"}) {
		json_t* lane = json_object_get(lanes, laneName);
		json_t* defaultJ = json_object_get(lane, "defaultProgram");
		if (json_is_string(defaultJ) && id == json_string_value(defaultJ)) {
			reference = std::string("/lanes/") + laneName + "/defaultProgram";
			return true;
		}
		json_t* assignments = json_object_get(lane, "assignments");
		const char* channel = nullptr;
		json_t* assigned = nullptr;
		json_object_foreach(assignments, channel, assigned) {
			if (json_is_string(assigned) && id == json_string_value(assigned)) {
				reference = std::string("/lanes/") + laneName + "/assignments/" + channel;
				return true;
			}
		}
	}
	return false;
}

json_t* serializedPreset(const Program& preset) {
	Bank temporary;
	temporary.programs[preset.id] = preset;
	json_t* root = bankToJson(temporary);
	json_t* program = json_deep_copy(json_object_get(
		json_object_get(root, "programs"), preset.id.c_str()));
	json_decref(root);
	return program;
}

bool applyOperation(json_t*& working, json_t* operation, size_t index,
		EditResult& result) {
	const std::string path = "/operations/" + std::to_string(index);
	if (!json_is_object(operation))
		return fail(result, "invalid_operation", path, "operation must be an object");
	const char* op = stringField(operation, "op");
	if (!op) return fail(result, "invalid_operation", path + "/op", "operation requires string op");
	const std::string name(op);

	if (name == "replace_bank") {
		json_t* bank = json_object_get(operation, "bank");
		if (!json_is_object(bank))
			return fail(result, "invalid_operation", path + "/bank", "replace_bank requires object bank");
		json_t* replacement = json_deep_copy(bank);
		if (!replacement) return fail(result, "internal_error", path, "could not copy replacement bank");
		json_decref(working);
		working = replacement;
		return true;
	}

	json_t* programs = programsObject(working);
	const char* id = stringField(operation, "id");
	if (name == "upsert_program") {
		json_t* program = json_object_get(operation, "program");
		if (!id || !json_is_object(program))
			return fail(result, "invalid_operation", path, "upsert_program requires id and program");
		return json_object_set(programs, id, program) == 0
			|| fail(result, "internal_error", path, "could not update program");
	}
	if (name == "delete_program") {
		if (!id) return fail(result, "invalid_operation", path + "/id", "delete_program requires id");
		if (!json_object_get(programs, id))
			return fail(result, "object_not_found", path + "/id", "program does not exist");
		std::string reference;
		if (referencedProgram(working, id, reference))
			return fail(result, "object_in_use", path + "/id", "program is referenced at " + reference);
		json_object_del(programs, id);
		return true;
	}
	if (name == "clone_program") {
		const char* source = stringField(operation, "source_id");
		if (!source || !id) return fail(result, "invalid_operation", path, "clone_program requires source_id and id");
		json_t* sourceProgram = json_object_get(programs, source);
		if (!sourceProgram) return fail(result, "object_not_found", path + "/source_id", "source program does not exist");
		json_t* clone = json_deep_copy(sourceProgram);
		if (const char* displayName = stringField(operation, "name"))
			json_object_set_new(clone, "name", json_string(displayName));
		json_object_set_new(programs, id, clone);
		return true;
	}
	if (name == "apply_preset") {
		const char* presetId = stringField(operation, "preset_id");
		if (!presetId || !id) return fail(result, "invalid_operation", path, "apply_preset requires preset_id and id");
		const Program* preset = findFactoryProgram(presetId);
		if (!preset) return fail(result, "object_not_found", path + "/preset_id", "factory preset does not exist");
		json_t* program = serializedPreset(*preset);
		if (!program) return fail(result, "internal_error", path, "could not serialize factory preset");
		json_object_set_new(programs, id, program);
		return true;
	}

	int lane = 0;
	if (name == "assign_program" || name == "set_channel_label"
			|| name == "set_lane_defaults" || name == "set_output_mode") {
		if (!laneIndex(operation, lane, result, path)) return false;
	}
	json_t* laneJ = (lane == 0 || lane == 1)
		? json_object_get(lanesObject(working), lane == 0 ? "A" : "B") : nullptr;
	if (name == "assign_program" || name == "set_channel_label") {
		json_t* channelJ = json_object_get(operation, "channel");
		if (!json_is_integer(channelJ) || json_integer_value(channelJ) < 0
				|| json_integer_value(channelJ) >= kMaxChannels)
			return fail(result, "invalid_operation", path + "/channel", "channel must be in 0..15");
		const std::string channel = std::to_string(json_integer_value(channelJ));
		json_t* target = json_object_get(laneJ,
			name == "assign_program" ? "assignments" : "channelLabels");
		const char* key = name == "assign_program" ? "program" : "label";
		json_t* value = json_object_get(operation, key);
		if (json_is_null(value)) json_object_del(target, channel.c_str());
		else if (json_is_string(value)) json_object_set(target, channel.c_str(), value);
		else return fail(result, "invalid_operation", path + "/" + key, std::string(key) + " must be string or null");
		return true;
	}
	if (name == "set_lane_defaults") {
		json_t* defaults = json_object_get(operation, "defaults");
		if (!json_is_object(defaults)) return fail(result, "invalid_operation", path + "/defaults", "defaults must be an object");
		const char* key = nullptr; json_t* value = nullptr;
		json_object_foreach(defaults, key, value) json_object_set(laneJ, key, value);
		return true;
	}
	if (name == "set_output_mode") {
		json_t* mode = json_object_get(operation, "mode");
		if (!json_is_string(mode)) return fail(result, "invalid_operation", path + "/mode", "mode must be a string");
		json_object_set(laneJ, "outputMode", mode);
		return true;
	}
	if (name == "set_macro_binding") {
		const char* programId = stringField(operation, "program_id");
		json_t* indexJ = json_object_get(operation, "index");
		json_t* binding = json_object_get(operation, "binding");
		json_t* program = programId ? json_object_get(programs, programId) : nullptr;
		json_t* bindings = program ? json_object_get(program, "macroBindings") : nullptr;
		if (!program) return fail(result, "object_not_found", path + "/program_id", "program does not exist");
		if (!json_is_integer(indexJ) || json_integer_value(indexJ) < 0 || !json_is_array(bindings))
			return fail(result, "invalid_operation", path + "/index", "index must be nonnegative");
		const size_t bindingIndex = static_cast<size_t>(json_integer_value(indexJ));
		if (json_is_null(binding)) {
			if (bindingIndex >= json_array_size(bindings)) return fail(result, "object_not_found", path + "/index", "binding does not exist");
			json_array_remove(bindings, bindingIndex);
		} else if (!json_is_object(binding) || bindingIndex > json_array_size(bindings)) {
			return fail(result, "invalid_operation", path + "/binding", "binding must be object, or null at an existing index");
		} else if (bindingIndex == json_array_size(bindings)) json_array_append(bindings, binding);
		else json_array_set(bindings, bindingIndex, binding);
		return true;
	}
	if (name == "set_clock") {
		json_t* clock = json_object_get(operation, "clock");
		if (!json_is_object(clock)) return fail(result, "invalid_operation", path + "/clock", "clock must be an object");
		json_t* target = json_object_get(working, "clock");
		const char* key = nullptr; json_t* value = nullptr;
		json_object_foreach(clock, key, value) json_object_set(target, key, value);
		return true;
	}
	return fail(result, "unknown_operation", path + "/op", "unknown edit operation '" + name + "'");
}

bool parsePolicies(json_t* request, EditResult& result) {
	const char* apply = stringField(request, "apply_at");
	if (!apply) return fail(result, "invalid_request", "/apply_at", "apply_at is required");
	if (std::string(apply) == "immediate") result.applyAt = ApplyAt::IMMEDIATE;
	else if (std::string(apply) == "nextTrigger") result.applyAt = ApplyAt::NEXT_TRIGGER;
	else if (std::string(apply) == "allIdle") result.applyAt = ApplyAt::ALL_IDLE;
	else if (std::string(apply) == "nextClock") result.applyAt = ApplyAt::NEXT_CLOCK;
	else return fail(result, "invalid_request", "/apply_at", "unsupported adoption boundary");
	const char* policy = stringField(request, "active_voice_policy");
	if (!policy) return fail(result, "invalid_request", "/active_voice_policy", "active_voice_policy is required");
	if (std::string(policy) == "finishCurrent") result.activeVoicePolicy = ActiveVoicePolicy::FINISH_CURRENT;
	else if (std::string(policy) == "restartActive") result.activeVoicePolicy = ActiveVoicePolicy::RESTART_ACTIVE;
	else return fail(result, "invalid_request", "/active_voice_policy", "unsupported active voice policy");
	if (result.activeVoicePolicy == ActiveVoicePolicy::RESTART_ACTIVE
			&& result.applyAt != ApplyAt::IMMEDIATE)
		return fail(result, "invalid_request", "/active_voice_policy",
			"restartActive is valid only with immediate adoption");
	return true;
}

} // namespace

EditResult applyBankEdit(const Bank& base, json_t* request) {
	EditResult result;
	result.currentRevision = base.revision;
	if (!json_is_object(request)) {
		fail(result, "invalid_request", "", "edit request must be an object");
		return result;
	}
	json_t* expectedJ = json_object_get(request, "expected_revision");
	if (!json_is_integer(expectedJ)) {
		fail(result, "invalid_request", "/expected_revision", "expected_revision is required");
		return result;
	}
	if (json_integer_value(expectedJ) != base.revision) {
		fail(result, "revision_conflict", "/expected_revision", "accepted revision has changed");
		return result;
	}
	if (!parsePolicies(request, result)) return result;
	json_t* operations = json_object_get(request, "operations");
	if (!json_is_array(operations) || json_array_size(operations) == 0) {
		fail(result, "invalid_request", "/operations", "operations must be a non-empty array");
		return result;
	}
	json_t* working = bankToJson(base);
	size_t index = 0; json_t* operation = nullptr;
	json_array_foreach(operations, index, operation) {
		if (!applyOperation(working, operation, index, result)) {
			json_decref(working);
			return result;
		}
	}
	json_object_set_new(working, "revision", json_integer(base.revision + 1));
	JsonResult parsed = parseBankJson(working);
	json_decref(working);
	if (!parsed.valid) {
		result.errorCode = "validation_failed";
		result.errors = parsed.errors;
		if (!parsed.errors.empty()) {
			result.errorPath = parsed.errors.front().path;
			result.errorMessage = parsed.errors.front().message;
		}
		return result;
	}
	CompileResult compiled = compileBank(parsed.bank);
	if (!compiled.valid) {
		result.errorCode = "validation_failed";
		result.errors = compiled.errors;
		result.warnings = compiled.warnings;
		return result;
	}
	result.valid = true;
	result.bank = std::move(parsed.bank);
	result.compiledBank = std::move(compiled.bank);
	result.warnings = std::move(compiled.warnings);
	return result;
}

} // namespace moirai
