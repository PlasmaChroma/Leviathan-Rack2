#pragma once

#include "SibylEdit.hpp"
#include <set>

namespace sibyl {
// Control-side JSON edits only. Runtime routing holds compiled assignments.
inline bool applyAssignmentOperation(json_t* working, json_t* op, size_t index, EditResult& result) {
    const std::string path = "operations[" + std::to_string(index) + "]";
    auto fail = [&](const std::string& field, const char* message) {
        result.errorCode = "invalid_operation"; result.errorPath = path + field; result.errorMessage = message; return false;
    };
    auto fields = [&](json_t* object, const std::set<std::string>& allowed, const std::string& at) {
        if (!json_is_object(object)) return fail(at, "Expected object");
        const char* key; json_t* value;
        json_object_foreach(object, key, value) if (!allowed.count(key)) return fail(at + "." + key, "Unknown field");
        return true;
    };
    const bool partial = std::string(json_string_value(json_object_get(op, "op"))) == "update_scene_assignment";
    if (!fields(op, partial ? std::set<std::string>{"op","scene_id","track_id","set","unset"} :
            std::set<std::string>{"op","scene_id","track_id","assignment"}, "")) return false;
    json_t* sceneId = json_object_get(op, "scene_id"); json_t* trackId = json_object_get(op, "track_id");
    if (!json_is_string(sceneId) || !json_is_string(trackId)) return fail("", "scene_id and track_id must be strings");
    const char* track = json_string_value(trackId);
    bool trackFound = false; size_t i; json_t* item;
    json_array_foreach(json_object_get(working,"tracks"), i, item)
        if (json_equal(json_object_get(item,"id"),trackId)) trackFound = true;
    if (!trackFound) return fail(".track_id", "Track does not exist");
    json_t* scene = nullptr;
    json_array_foreach(json_object_get(working,"arrangement"), i, item)
        if (json_equal(json_object_get(item,"id"),sceneId)) { scene=item; break; }
    if (!scene) return fail(".scene_id", "Scene does not exist");
    json_t* assignments = json_object_get(scene,"tracks");
    if (!json_is_object(assignments)) return fail("", "Scene assignments must be an object");
    auto overrides = [&](json_t* object, const std::string& at) {
        if (!json_is_object(object)) return fail(at,"Expected overrides object");
        const char* key; json_t* value;
        json_object_foreach(object,key,value) if(overrideFieldIndex(key)<0) return fail(at+"."+key,"Unknown override field");
        return true;
    };
    auto patternReference = [&](json_t* value, const std::string& at) {
        return (json_is_string(value) && json_string_length(value) > 0 && json_string_length(value) <= 64)
            || fail(at,"Expected nonempty pattern ID of at most 64 bytes");
    };
    if (!partial) {
        json_t* assignment=json_object_get(op,"assignment");
        if (!assignment || (!json_is_object(assignment) && !json_is_string(assignment) && !json_is_null(assignment)))
            return fail(".assignment","Expected assignment object, string or null");
        if (json_is_object(assignment)) {
            if (!fields(assignment,{"pattern","phaseMode","overrides"},".assignment")) return false;
            if (!patternReference(json_object_get(assignment,"pattern"),".assignment.pattern")) return false;
            json_t* ov=json_object_get(assignment,"overrides");
            if(ov && !overrides(ov,".assignment.overrides")) return false;
        }
        if (json_is_string(assignment) && !patternReference(assignment,".assignment")) return false;
        if(json_is_null(assignment)) json_object_del(assignments,track);
        else json_object_set(assignments,track,assignment);
        return true;
    }
    json_t* old=json_object_get(assignments,track);
    if (!old || json_is_null(old)) return fail(".track_id","Assignment does not exist; use set_scene_assignment to create it");
    json_t* set=json_object_get(op,"set"); json_t* unset=json_object_get(op,"unset");
    if(!set && !unset) return fail("","Expected set or unset");
    if(set && !fields(set,{"pattern","phaseMode","overrides"},".set")) return false;
    if(json_object_get(set,"pattern") && !patternReference(json_object_get(set,"pattern"),".set.pattern")) return false;
    json_t* ov=json_object_get(set,"overrides");
    if(ov && !overrides(ov,".set.overrides")) return false;
    if(unset && !json_is_array(unset)) return fail(".unset","Expected field-name array");
    std::set<std::string> removals;
    json_array_foreach(unset,i,item) {
        if(!json_is_string(item)) return fail(".unset","Expected field name");
        const std::string field=json_string_value(item);
        const bool leaf=field.compare(0,10,"overrides.")==0 && overrideFieldIndex(field.substr(10))>=0;
        if(field!="phaseMode" && field!="overrides" && !leaf) return fail(".unset","Unsupported field removal");
        if(!removals.insert(field).second) return fail(".unset","Duplicate removal");
        if(json_object_get(set,field.c_str()) || (leaf && json_object_get(ov,field.substr(10).c_str())))
            return fail(".unset","Cannot set and unset the same field");
    }
    if(removals.count("overrides")) for(const auto& removal:removals)
        if(removal.compare(0,10,"overrides.")==0) return fail(".unset","Cannot unset a parent and its leaf together");
    // Deep-copy before mutation: candidate operation objects can be reused by callers.
    json_t* next=json_is_string(old) ? json_pack("{s:O}","pattern",old) : json_deep_copy(old);
    if(set) {
        const char* key;json_t* value;
        json_object_foreach(set,key,value) {
            if(std::string(key)=="overrides") {
                json_t* target=json_object_get(next,"overrides");
                if(!target){target=json_object();json_object_set_new(next,"overrides",target);}
                json_object_update(target,value);
            } else json_object_set(next,key,value);
        }
    }
    for(const auto& field:removals) {
        if(field.compare(0,10,"overrides.")==0) json_object_del(json_object_get(next,"overrides"),field.substr(10).c_str());
        else json_object_del(next,field.c_str());
    }
    json_object_set_new(assignments,track,next);
    return true;
}
} // namespace sibyl
