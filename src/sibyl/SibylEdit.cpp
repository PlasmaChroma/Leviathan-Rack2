#include "SibylEdit.hpp"
#include "SibylNoteEdit.hpp"
#include "SibylPitchEdit.hpp"
#include "SibylAssignmentEdit.hpp"
#include "SibylHarmonyEdit.hpp"
#include "SibylVoicingEdit.hpp"

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

    if (isPitchDefinitionOperation(name)) return applyPitchDefinitionOperation(working,op,index,result);
    if (isNoteOperation(name)) return applyNoteOperation(working, op, index, result);
    const std::string operationName = name;
    if (operationName == "set_scene_assignment" || operationName == "update_scene_assignment")
        return applyAssignmentOperation(working, op, index, result);
    if (isHarmonyOperation(operationName)) return applyHarmonyOperation(working,op,index,result);
    if (operationName == "voice_progression") return applyVoicingOperation(working,op,index,result);
    if (operationName == "upsert_automation" || operationName == "delete_automation") {
        const bool upsert=operationName=="upsert_automation";
        const char* key; json_t* value;
        json_object_foreach(op,key,value) if(std::string(key)!="op" && std::string(key)!="id" && (!upsert || std::string(key)!="automation"))
            return fail(result,"invalid_operation",path+"."+key,"Unknown automation operation field");
        const char* id=requiredString(op,"id");
        if(!id || !*id || std::string(id).size()>64)return fail(result,"invalid_operation",path+".id","Expected automation ID");
        json_t* definitions=json_object_get(working,"automation");
        if(!definitions){definitions=json_object();json_object_set_new(working,"automation",definitions);}
        if(upsert){
            json_t* curve=requiredObject(op,"automation");
            if(!curve)return fail(result,"invalid_operation",path+".automation","Expected complete curve object");
            json_object_set(definitions,id,curve);
        }else{
            if(!json_object_get(definitions,id))return fail(result,"object_not_found",path+".id","Automation not found");
            json_object_del(definitions,id);
        }
        return true;
    }
	if (std::string(name) == "replace_composition") {
		json_t* composition = requiredObject(op, "composition");
		if (!composition) return fail(result, "invalid_operation", path + ".composition", "replace_composition requires an object composition.");
		ParseResult normalized; normalized.valid = true;
        json_t* replacement = normalizeComposition(composition, normalized);
        if (!replacement) {
            const auto& issue = normalized.errors.front();
            return fail(result, issue.code, issue.path, issue.message);
        }
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
        if (isPattern) {
            if (!json_object_get(copy, "length")) json_object_set_new(copy, "length", json_integer(16));
            if (!json_object_get(copy, "resolution")) json_object_set_new(copy, "resolution", json_string("1/16"));
            ParseResult normalized; normalized.valid = true;
            if (!normalizeNoteIds(copy, "patterns." + std::string(id), normalized, json_object_get(target, id))) {
                json_decref(copy);
                const auto& issue = normalized.errors.front();
                return fail(result, issue.code, issue.path, issue.message);
            }
        }
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
	if (std::string(name) == "update_pattern") {
		if (!id || !json_is_object(patterns)) return fail(result, "invalid_operation", path, "update_pattern requires id.");
		json_t* pattern = json_object_get(patterns, id);
		if (!pattern) return fail(result, "object_not_found", path, "Pattern '" + std::string(id) + "' does not exist.");
		
		json_t* setJ = json_object_get(op, "set");
		json_t* unsetJ = json_object_get(op, "unset");
		if (!setJ && !unsetJ) return fail(result, "invalid_operation", path, "update_pattern requires set or unset.");
		if (setJ && !json_is_object(setJ)) return fail(result, "invalid_operation", path + ".set", "Expected object set.");
		if (unsetJ && !json_is_array(unsetJ)) return fail(result, "invalid_operation", path + ".unset", "Expected array unset.");

		const std::set<std::string> allowedProps = {"length", "resolution", "evolution", "pitchContext", "macroBindings", "steps"};
		if (setJ) {
			const char* k; json_t* v;
			json_object_foreach(setJ, k, v) {
				if (!allowedProps.count(k))
					return fail(result, "invalid_operation", path + ".set." + k, "Unknown pattern property: " + std::string(k));
			}
			// Safe truncation check: shrinking length must not truncate existing active notes
			if (json_t* newLenJ = json_object_get(setJ, "length")) {
				if (!json_is_integer(newLenJ) || json_integer_value(newLenJ) < 1 || json_integer_value(newLenJ) > 1024)
					return fail(result, "invalid_operation", path + ".set.length", "Expected length integer 1-1024.");
				int64_t newLen = json_integer_value(newLenJ);
				json_t* steps = json_object_get(pattern, "steps");
				if (json_is_array(steps)) {
					size_t sIdx; json_t* note;
					json_array_foreach(steps, sIdx, note) {
						json_t* stepJ = json_object_get(note, "step");
						if (stepJ && json_integer_value(stepJ) >= newLen) {
							return fail(result, "invalid_operation", path + ".set.length",
								"Cannot shrink pattern length below active note step " + std::to_string(json_integer_value(stepJ)) + "; delete notes explicitly first.");
						}
					}
				}
			}
			json_object_foreach(setJ, k, v) {
				json_object_set(pattern, k, v);
			}
		}
		if (unsetJ) {
			size_t uIdx; json_t* uKey;
			json_array_foreach(unsetJ, uIdx, uKey) {
				if (!json_is_string(uKey)) return fail(result, "invalid_operation", path + ".unset", "Unset keys must be strings.");
				std::string uk = json_string_value(uKey);
				if (uk == "length" || uk == "resolution" || uk == "steps")
					return fail(result, "invalid_operation", path + ".unset", "Cannot unset mandatory pattern property '" + uk + "'.");
				json_object_del(pattern, uk.c_str());
			}
		}
		ParseResult normalized; normalized.valid = true;
		if (!normalizeNoteIds(pattern, "patterns." + std::string(id), normalized)) {
			const auto& issue = normalized.errors.front();
			return fail(result, issue.code, issue.path, issue.message);
		}
		return true;
	}
	if (std::string(name) == "clone_pattern") {
		const char* srcId = requiredString(op, "source_id");
		if (!id || !srcId || !json_is_object(patterns)) return fail(result, "invalid_operation", path, "clone_pattern requires source_id and id.");
		json_t* srcPattern = json_object_get(patterns, srcId);
		if (!srcPattern) return fail(result, "object_not_found", path + ".source_id", "Source pattern '" + std::string(srcId) + "' does not exist.");
		if (json_object_get(patterns, id)) return fail(result, "invalid_operation", path + ".id", "Pattern '" + std::string(id) + "' already exists.");

		json_t* cloned = json_deep_copy(srcPattern);
		json_t* overrides = json_object_get(op, "overrides");
		if (overrides) {
			if (!json_is_object(overrides)) {
				json_decref(cloned);
				return fail(result, "invalid_operation", path + ".overrides", "Expected object overrides.");
			}
			const std::set<std::string> allowedProps = {"length", "resolution", "evolution", "pitchContext", "macroBindings", "steps"};
			const char* k; json_t* v;
			json_object_foreach(overrides, k, v) {
				if (!allowedProps.count(k)) {
					json_decref(cloned);
					return fail(result, "invalid_operation", path + ".overrides." + k, "Unknown pattern override property: " + std::string(k));
				}
			}
			if (json_t* newLenJ = json_object_get(overrides, "length")) {
				if (!json_is_integer(newLenJ) || json_integer_value(newLenJ) < 1 || json_integer_value(newLenJ) > 1024) {
					json_decref(cloned);
					return fail(result, "invalid_operation", path + ".overrides.length", "Expected length integer 1-1024.");
				}
			int64_t newLen = json_integer_value(newLenJ);
				json_t* steps = json_object_get(cloned, "steps");
				if (json_is_array(steps)) {
					size_t sIdx; json_t* note;
					json_array_foreach(steps, sIdx, note) {
						json_t* stepJ = json_object_get(note, "step");
						if (stepJ && json_integer_value(stepJ) >= newLen) {
							json_decref(cloned);
							return fail(result, "invalid_operation", path + ".overrides.length",
								"Cannot shrink pattern length below active note step " + std::to_string(json_integer_value(stepJ)) + "; delete notes explicitly first.");
						}
					}
				}
			}
			json_object_foreach(overrides, k, v) {
				json_object_set(cloned, k, v);
			}
		}
		// Reset IDs on cloned notes so they receive fresh sequential identifiers in the new pattern
		json_t* steps = json_object_get(cloned, "steps");
		if (json_is_array(steps)) {
			size_t sIdx; json_t* note;
			json_array_foreach(steps, sIdx, note) {
				json_object_del(note, "id");
			}
		}
		json_object_del(cloned, "nextNoteId");
		ParseResult normalized; normalized.valid = true;
		if (!normalizeNoteIds(cloned, "patterns." + std::string(id), normalized, nullptr)) {
			json_decref(cloned);
			const auto& issue = normalized.errors.front();
			return fail(result, issue.code, issue.path, issue.message);
		}
		json_object_set_new(patterns, id, cloned);
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
	if (std::string(name) == "clone_scene") {
		const char* srcId = requiredString(op, "source_id");
		if (!id || !srcId || !json_is_array(arrangement)) return fail(result, "invalid_operation", path, "clone_scene requires source_id and id.");
		int srcIdx = findArrayObject(arrangement, srcId);
		if (srcIdx < 0) return fail(result, "object_not_found", path + ".source_id", "Source scene '" + std::string(srcId) + "' does not exist.");
		if (findArrayObject(arrangement, id) >= 0) return fail(result, "invalid_operation", path + ".id", "Scene '" + std::string(id) + "' already exists.");

		json_t* srcScene = json_array_get(arrangement, size_t(srcIdx));
		json_t* cloned = json_deep_copy(srcScene);
		json_object_set_new(cloned, "id", json_string(id));

		if (json_t* rep = json_object_get(op, "repeats")) {
			if (!json_is_integer(rep) || json_integer_value(rep) < 1 || json_integer_value(rep) > 1024) {
				json_decref(cloned);
				return fail(result, "invalid_operation", path + ".repeats", "Expected repeats integer 1-1024.");
			}
			json_object_set(cloned, "repeats", rep);
		}
		if (json_t* lb = json_object_get(op, "lengthBeats")) {
			if (!json_is_number(lb) || json_number_value(lb) <= 0. || !std::isfinite(json_number_value(lb))) {
				json_decref(cloned);
				return fail(result, "invalid_operation", path + ".lengthBeats", "Expected positive finite lengthBeats.");
			}
			json_object_set(cloned, "lengthBeats", lb);
		}
		if (json_t* harm = json_object_get(op, "harmony")) {
			if (json_is_null(harm)) json_object_del(cloned, "harmony");
			else json_object_set(cloned, "harmony", harm);
		}

		// Handle patterns: "share" (default) vs "copy"
		json_t* patPolicyJ = json_object_get(op, "patterns");
		std::string patPolicy = patPolicyJ && json_is_string(patPolicyJ) ? json_string_value(patPolicyJ) : "share";
		if (patPolicy != "share" && patPolicy != "copy") {
			json_decref(cloned);
			return fail(result, "invalid_operation", path + ".patterns", "Expected patterns 'share' or 'copy'.");
		}

		json_t* tracksObj = json_object_get(cloned, "tracks");
		if (tracksObj && json_is_object(tracksObj)) {
			if (patPolicy == "copy") {
				json_t* idMapping = json_object_get(op, "pattern_id_mapping");
                if (idMapping && !json_is_object(idMapping)) {
                    json_decref(cloned);
                    return fail(result, "invalid_operation", path + ".pattern_id_mapping", "Expected destination ID map.");
                }
                const char* mappingKey; json_t* mappingValue;
                json_object_foreach(idMapping, mappingKey, mappingValue) {
                    if (!json_is_string(mappingValue) || !*json_string_value(mappingValue)) {
                        json_decref(cloned);
                        return fail(result, "invalid_operation", path + ".pattern_id_mapping." + mappingKey, "Expected nonempty destination ID.");
                    }
                }
                std::map<std::string, std::string> copiedDestinations;
				const char* tKey; json_t* tVal;
				json_object_foreach(tracksObj, tKey, tVal) {
					std::string origPatternId;
					if (json_is_string(tVal)) origPatternId = json_string_value(tVal);
					else if (json_is_object(tVal) && json_object_get(tVal, "pattern"))
						origPatternId = json_string_value(json_object_get(tVal, "pattern"));
					if (!origPatternId.empty()) {
						std::string targetPatternId;
						if (idMapping && json_is_object(idMapping) && json_object_get(idMapping, origPatternId.c_str())) {
							targetPatternId = json_string_value(json_object_get(idMapping, origPatternId.c_str()));
						} else {
							targetPatternId = origPatternId + "_" + std::string(id);
						}
                        auto copied = copiedDestinations.find(targetPatternId);
                        if ((copied != copiedDestinations.end() && copied->second != origPatternId)
                                || (copied == copiedDestinations.end() && json_object_get(patterns, targetPatternId.c_str()))) {
                            json_decref(cloned);
                            return fail(result, "invalid_operation", path + ".pattern_id_mapping", "Copy destination already exists or aliases another source: " + targetPatternId);
                        }
                        copiedDestinations[targetPatternId] = origPatternId;
                        // Reuse only copies made for this source in this operation.
						if (!json_object_get(patterns, targetPatternId.c_str())) {
							json_t* origPat = json_object_get(patterns, origPatternId.c_str());
							if (origPat) {
								json_t* patCopy = json_deep_copy(origPat);
								json_t* pSteps = json_object_get(patCopy, "steps");
							if (json_is_array(pSteps)) {
								size_t sI; json_t* pNote;
								json_array_foreach(pSteps, sI, pNote) json_object_del(pNote, "id");
							}
							json_object_del(patCopy, "nextNoteId");
							ParseResult norm; norm.valid = true;
							normalizeNoteIds(patCopy, "patterns." + targetPatternId, norm, nullptr);
							json_object_set_new(patterns, targetPatternId.c_str(), patCopy);
						}
					}
					if (json_is_string(tVal)) json_object_set_new(tracksObj, tKey, json_string(targetPatternId.c_str()));
					else json_object_set_new(tVal, "pattern", json_string(targetPatternId.c_str()));
				}
			}
		}

		// Apply track_overrides
		json_t* trackOverrides = json_object_get(op, "track_overrides");
		if (trackOverrides) {
			if (!json_is_object(trackOverrides)) {
				json_decref(cloned);
				return fail(result, "invalid_operation", path + ".track_overrides", "Expected object track_overrides.");
			}
			const char* oTrack; json_t* oVal;
			json_object_foreach(trackOverrides, oTrack, oVal) {
				if (json_is_null(oVal)) {
					json_object_del(tracksObj, oTrack);
				} else if (json_is_string(oVal)) {
					json_object_set(tracksObj, oTrack, oVal);
				} else if (json_is_object(oVal)) {
					json_object_set(tracksObj, oTrack, oVal);
				} else {
					json_decref(cloned);
					return fail(result, "invalid_operation", path + ".track_overrides." + oTrack, "Expected string, object, or null.");
				}
			}
		}
	}

	json_t* posJ = json_object_get(op, "position");
	std::string pos = posJ && json_is_string(posJ) ? json_string_value(posJ) : "end";
	if (pos == "after_source") {
		json_array_insert_new(arrangement, size_t(srcIdx + 1), cloned);
	} else {
		json_array_append_new(arrangement, cloned);
	}
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
		json_t* previous = json_object_get(sceneTracks, trackId);
		if (json_is_object(previous) && (json_object_get(previous, "phaseMode") || json_object_get(previous, "overrides")))
			result.warnings.push_back({path, "assignment_attributes_cleared: legacy set_scene_track discarded phase/override attributes", "assignment_attributes_cleared"});
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
	if (!operations || !json_is_array(operations) || json_array_size(operations) == 0 || json_array_size(operations) > 256) {
		fail(result, "invalid_request", "operations", "operations must contain 1-256 entries.");
		return result;
	}
	json_error_t error;
	json_t* serialized = json_loads(serializeFullCompositionJson(base).c_str(), 0, &error);
	json_t* serializedComposition = serialized ? json_object_get(serialized, "composition") : nullptr;
	json_t* working = serializedComposition ? json_deep_copy(serializedComposition) : nullptr;
	if (!working) {
        if (serialized) json_decref(serialized);
		fail(result, "internal_error", "", "Could not serialize the accepted composition.");
		return result;
	}
	size_t index;
	json_t* op;
	json_array_foreach(operations, index, op) {
		if (!applyOperation(working, op, index, result)) {
			json_decref(working);
            json_decref(serialized);
			return result;
		}
	}
    // Compare authored objects while the two control-side JSON snapshots exist.
    auto recordMap = [&](json_t* before, json_t* after, const std::string& prefix) {
        std::set<std::string> keys;
        const char* key; json_t* value;
        json_object_foreach(before, key, value) keys.insert(key);
        json_object_foreach(after, key, value) keys.insert(key);
        for (const auto& id : keys)
            if (!json_equal(json_object_get(before, id.c_str()), json_object_get(after, id.c_str())))
                result.affectedObjects.push_back(prefix + "/" + id);
    };
    for (const char* key : {"meta", "clock", "transport"})
        if (!json_equal(json_object_get(serializedComposition,key), json_object_get(working,key)))
            result.affectedObjects.push_back(key);
    for (const char* key : {"patterns", "macros", "automation"})
        recordMap(json_object_get(serializedComposition,key), json_object_get(working,key), key);
    for (const char* key : {"tracks", "arrangement"}) {
        json_t* before = json_object(); json_t* after = json_object();
        size_t i; json_t* object;
        json_array_foreach(json_object_get(serializedComposition,key),i,object) {
            const char* id = json_string_value(json_object_get(object,"id"));
            if(id) json_object_set(before,id,object);
        }
        json_array_foreach(json_object_get(working,key),i,object) {
            const char* id = json_string_value(json_object_get(object,"id"));
            if(id) json_object_set(after,id,object);
        }
        recordMap(before,after,key);
        json_decref(before); json_decref(after);
        if (std::string(key)=="arrangement" && !json_equal(json_object_get(serializedComposition,key),json_object_get(working,key)))
            result.affectedObjects.push_back("arrangement");
    }
    for (const char* key : {"pitchSystems", "harmony"})
        recordMap(json_object_get(serializedComposition,key), json_object_get(working,key), key);
    json_decref(serialized);
	char* encoded = json_dumps(working, JSON_COMPACT);
	json_decref(working);
	ParseResult parsed = parseCompositionJson(encoded ? encoded : "{}", revision);
	if (encoded) free(encoded);
	result.valid = parsed.valid;
	result.composition = parsed.composition;
	result.errors = parsed.errors;
	result.warnings.insert(result.warnings.end(), parsed.warnings.begin(), parsed.warnings.end());
	if (!parsed.valid) {
		result.errorCode = parsed.errors.empty() ? "validation_failed" : parsed.errors.front().code;
		if (!parsed.errors.empty()) {
			result.errorPath = parsed.errors.front().path;
			result.errorMessage = parsed.errors.front().message;
		} else result.errorMessage = "Composition validation failed.";
	}
	return result;
}

json_t* editAffectedObjectsJson(const EditResult& result) {
    json_t* out = json_object();
    json_t* ids = json_array();
    const size_t count = std::min(size_t(32), result.affectedObjects.size());
    for (size_t i=0;i<count;++i) json_array_append_new(ids,json_string(result.affectedObjects[i].c_str()));
    json_object_set_new(out,"ids",ids);
    json_object_set_new(out,"total",json_integer(result.affectedObjects.size()));
    json_object_set_new(out,"omitted",json_integer(result.affectedObjects.size()-count));
    return out;
}

} // namespace sibyl
