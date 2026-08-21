#include "SibylEdit.hpp"

#include <algorithm>
#include <cstdlib>
#include <unordered_set>

namespace sibyl {
namespace {

bool fail(EditResult& result, const std::string& code, const std::string& path, const std::string& message) {
	result.errorCode = code;
	result.errorPath = path;
	result.errorMessage = message;
	return false;
}

json_t* requiredObject(json_t* op, const char* key) {
	json_t* value = json_object_get(op, key);
	return value && json_is_object(value) ? value : nullptr;
}

const char* requiredString(json_t* op, const char* key) {
	json_t* value = json_object_get(op, key);
	return value && json_is_string(value) ? json_string_value(value) : nullptr;
}

int findArrayObject(json_t* array, const char* id) {
	if (!array || !json_is_array(array)) return -1;
	size_t index;
	json_t* item;
	json_array_foreach(array, index, item) {
		json_t* idJ = json_object_get(item, "id");
		if (idJ && json_is_string(idJ) && std::string(json_string_value(idJ)) == id) return int(index);
	}
	return -1;
}

bool replaceOrAppendById(json_t* array, const char* id, json_t* value) {
	json_t* copy = json_deep_copy(value);
	if (!copy) return false;
	json_object_set_new(copy, "id", json_string(id));
	int index = findArrayObject(array, id);
	if (index >= 0) return json_array_set_new(array, size_t(index), copy) == 0;
	return json_array_append_new(array, copy) == 0;
}

bool patternReferenced(json_t* arrangement, const char* patternId, std::string& sceneId) {
	size_t index;
	json_t* scene;
	json_array_foreach(arrangement, index, scene) {
		json_t* tracks = json_object_get(scene, "tracks");
		if (!tracks || !json_is_object(tracks)) continue;
		const char* trackId;
		json_t* assignment;
		json_object_foreach(tracks, trackId, assignment) {
			const char* assigned = nullptr;
			if (json_is_string(assignment)) assigned = json_string_value(assignment);
			else if (json_is_object(assignment)) {
				json_t* patternJ = json_object_get(assignment, "pattern");
				if (patternJ && json_is_string(patternJ)) assigned = json_string_value(patternJ);
			}
			if (assigned && std::string(assigned) == patternId) {
				json_t* idJ = json_object_get(scene, "id");
				sceneId = idJ && json_is_string(idJ) ? json_string_value(idJ) : std::to_string(index);
				return true;
			}
		}
	}
	return false;
}

bool trackReferenced(json_t* arrangement, const char* trackId, std::string& sceneId) {
	size_t index;
	json_t* scene;
	json_array_foreach(arrangement, index, scene) {
		json_t* tracks = json_object_get(scene, "tracks");
		if (!tracks || !json_is_object(tracks) || !json_object_get(tracks, trackId)) continue;
		json_t* idJ = json_object_get(scene, "id");
		sceneId = idJ && json_is_string(idJ) ? json_string_value(idJ) : std::to_string(index);
		return true;
	}
	return false;
}

bool applyOperation(json_t*& working, json_t* op, size_t index, EditResult& result) {
	const std::string path = "operations[" + std::to_string(index) + "]";
	if (!json_is_object(op)) return fail(result, "invalid_operation", path, "Operation must be an object.");
	const char* name = requiredString(op, "op");
	if (!name) return fail(result, "invalid_operation", path + ".op", "Operation requires a string op.");

	if (std::string(name) == "replace_composition") {
		json_t* composition = requiredObject(op, "composition");
		if (!composition) return fail(result, "invalid_operation", path + ".composition", "replace_composition requires an object composition.");
		json_t* replacement = json_deep_copy(composition);
		if (!replacement) return fail(result, "internal_error", path, "Could not copy replacement composition.");
		json_decref(working);
		working = replacement;
		return true;
	}

	json_t* meta = json_object_get(working, "meta");
	json_t* clock = json_object_get(working, "clock");
	json_t* tracks = json_object_get(working, "tracks");
	json_t* patterns = json_object_get(working, "patterns");
	json_t* arrangement = json_object_get(working, "arrangement");
	json_t* macros = json_object_get(working, "macros");

	if (std::string(name) == "set_meta" || std::string(name) == "set_clock") {
		const bool isMeta = std::string(name) == "set_meta";
		json_t* target = isMeta ? meta : clock;
		const char* field = requiredString(op, "path");
		json_t* value = json_object_get(op, "value");
		if (!field || !value) return fail(result, "invalid_operation", path, std::string(name) + " requires path and value.");
		static const std::unordered_set<std::string> metaFields = {
			"title", "prompt", "bpm", "root", "rootOctave", "scale", "swing", "seed"
		};
		static const std::unordered_set<std::string> clockFields = {
			"externalPpqn", "outputPpqn", "externalTimeoutMs", "onExternalStop"
		};
		const auto& allowed = isMeta ? metaFields : clockFields;
		if (!allowed.count(field)) return fail(result, "invalid_operation", path + ".path", "Unsupported " + std::string(isMeta ? "meta" : "clock") + " path '" + field + "'.");
		if (!target || !json_is_object(target)) return fail(result, "invalid_composition", path, "Composition section is missing.");
		json_object_set(target, field, value);
		return true;
	}

	const char* id = requiredString(op, "id");
	if (std::string(name) == "upsert_track") {
		json_t* value = requiredObject(op, "track");
		if (!id || !value || !json_is_array(tracks)) return fail(result, "invalid_operation", path, "upsert_track requires id and track.");
		return replaceOrAppendById(tracks, id, value) || fail(result, "internal_error", path, "Could not update track.");
	}
	if (std::string(name) == "delete_track") {
		if (!id || !json_is_array(tracks)) return fail(result, "invalid_operation", path, "delete_track requires id.");
		std::string sceneId;
		if (trackReferenced(arrangement, id, sceneId)) return fail(result, "object_in_use", path, "Track '" + std::string(id) + "' is referenced by scene '" + sceneId + "'.");
		int found = findArrayObject(tracks, id);
		if (found < 0) return fail(result, "object_not_found", path, "Track '" + std::string(id) + "' does not exist.");
		json_array_remove(tracks, size_t(found));
		return true;
	}
	if (std::string(name) == "upsert_pattern" || std::string(name) == "upsert_macro") {
		const bool isPattern = std::string(name) == "upsert_pattern";
		json_t* target = isPattern ? patterns : macros;
		json_t* value = requiredObject(op, isPattern ? "pattern" : "macro");
		if (!id || !value || !json_is_object(target)) return fail(result, "invalid_operation", path, std::string(name) + " requires id and object value.");
		json_t* copy = json_deep_copy(value);
		json_object_del(copy, "id");
		json_object_set_new(target, id, copy);
		return true;
	}
	if (std::string(name) == "delete_pattern" || std::string(name) == "delete_macro") {
		const bool isPattern = std::string(name) == "delete_pattern";
		json_t* target = isPattern ? patterns : macros;
		if (!id || !json_is_object(target)) return fail(result, "invalid_operation", path, std::string(name) + " requires id.");
		if (!json_object_get(target, id)) return fail(result, "object_not_found", path, std::string(isPattern ? "Pattern '" : "Macro '") + id + "' does not exist.");
		if (isPattern) {
			std::string sceneId;
			if (patternReferenced(arrangement, id, sceneId)) return fail(result, "object_in_use", path, "Pattern '" + std::string(id) + "' is referenced by scene '" + sceneId + "'.");
		}
		json_object_del(target, id);
		return true;
	}
	if (std::string(name) == "upsert_scene") {
		json_t* value = requiredObject(op, "scene");
		if (!id || !value || !json_is_array(arrangement)) return fail(result, "invalid_operation", path, "upsert_scene requires id and scene.");
		return replaceOrAppendById(arrangement, id, value) || fail(result, "internal_error", path, "Could not update scene.");
	}
	if (std::string(name) == "delete_scene") {
		if (!id || !json_is_array(arrangement)) return fail(result, "invalid_operation", path, "delete_scene requires id.");
		int found = findArrayObject(arrangement, id);
		if (found < 0) return fail(result, "object_not_found", path, "Scene '" + std::string(id) + "' does not exist.");
		json_array_remove(arrangement, size_t(found));
		return true;
	}
	if (std::string(name) == "set_scene_track") {
		const char* sceneId = requiredString(op, "scene_id");
		const char* trackId = requiredString(op, "track_id");
		json_t* patternJ = json_object_get(op, "pattern_id");
		if (!sceneId || !trackId || !patternJ || (!json_is_string(patternJ) && !json_is_null(patternJ)))
			return fail(result, "invalid_operation", path, "set_scene_track requires scene_id, track_id, and string or null pattern_id.");
		int found = findArrayObject(arrangement, sceneId);
		if (found < 0) return fail(result, "object_not_found", path, "Scene '" + std::string(sceneId) + "' does not exist.");
		json_t* scene = json_array_get(arrangement, size_t(found));
		json_t* sceneTracks = json_object_get(scene, "tracks");
		if (!sceneTracks || !json_is_object(sceneTracks)) return fail(result, "invalid_composition", path, "Scene tracks must be an object.");
		if (json_is_null(patternJ)) json_object_del(sceneTracks, trackId);
		else json_object_set(sceneTracks, trackId, patternJ);
		return true;
	}
	if (std::string(name) == "reorder_scenes") {
		json_t* ids = json_object_get(op, "scene_ids");
		if (!ids) ids = json_object_get(op, "ids");
		if (!ids || !json_is_array(ids) || !json_is_array(arrangement) || json_array_size(ids) != json_array_size(arrangement))
			return fail(result, "invalid_operation", path, "reorder_scenes requires every scene id exactly once.");
		json_t* reordered = json_array();
		std::unordered_set<std::string> seen;
		size_t orderIndex;
		json_t* idJ;
		json_array_foreach(ids, orderIndex, idJ) {
			if (!json_is_string(idJ)) { json_decref(reordered); return fail(result, "invalid_operation", path, "Scene ids must be strings."); }
			std::string sceneId = json_string_value(idJ);
			int found = findArrayObject(arrangement, sceneId.c_str());
			if (found < 0 || !seen.insert(sceneId).second) { json_decref(reordered); return fail(result, "invalid_operation", path, "reorder_scenes requires every scene id exactly once."); }
			json_array_append(reordered, json_array_get(arrangement, size_t(found)));
		}
		json_object_set_new(working, "arrangement", reordered);
		return true;
	}
	return fail(result, "unknown_operation", path + ".op", "Unknown edit operation '" + std::string(name) + "'.");
}

} // namespace

EditResult applyCompositionEdit(const Composition& base, json_t* operations, int revision) {
	EditResult result;
	if (!operations || !json_is_array(operations) || json_array_size(operations) == 0) {
		fail(result, "invalid_request", "operations", "operations must be a non-empty array.");
		return result;
	}
	json_error_t error;
	json_t* serialized = json_loads(serializeFullCompositionJson(base).c_str(), 0, &error);
	json_t* serializedComposition = serialized ? json_object_get(serialized, "composition") : nullptr;
	json_t* working = serializedComposition ? json_deep_copy(serializedComposition) : nullptr;
	if (serialized) json_decref(serialized);
	if (!working) {
		fail(result, "internal_error", "", "Could not serialize the accepted composition.");
		return result;
	}
	size_t index;
	json_t* op;
	json_array_foreach(operations, index, op) {
		if (!applyOperation(working, op, index, result)) {
			json_decref(working);
			return result;
		}
	}
	char* encoded = json_dumps(working, JSON_COMPACT);
	json_decref(working);
	ParseResult parsed = parseCompositionJson(encoded ? encoded : "{}", revision);
	if (encoded) free(encoded);
	result.valid = parsed.valid;
	result.composition = parsed.composition;
	result.errors = parsed.errors;
	result.warnings = parsed.warnings;
	if (!parsed.valid) {
		result.errorCode = "validation_failed";
		if (!parsed.errors.empty()) {
			result.errorPath = parsed.errors.front().path;
			result.errorMessage = parsed.errors.front().message;
		} else result.errorMessage = "Composition validation failed.";
	}
	return result;
}

} // namespace sibyl
