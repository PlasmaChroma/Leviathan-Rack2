#pragma once
#include "SibylEdit.hpp"
#include "SibylScala.hpp"

namespace sibyl {
inline bool isPitchDefinitionOperation(const std::string& name) {
    return name=="import_tuning_scl" || name=="upsert_tuning" || name=="delete_tuning" || name=="upsert_pitch_scale" ||
        name=="delete_pitch_scale" || name=="upsert_pitch_context" || name=="delete_pitch_context" ||
        name=="set_default_pitch_context" || name=="set_pattern_pitch_context";
}
inline bool applyPitchDefinitionOperation(json_t* working,json_t* op,size_t index,EditResult& result) {
    const auto name=harmony_json::string(json_object_get(op,"op"));
    const std::string path="operations["+std::to_string(index)+"]";
    ParseResult validation; validation.valid=true;
    auto reject=[&](const char* code,const std::string& field,const char* message) {
        result.errorCode=code; result.errorPath=path+field; result.errorMessage=message; return false;
    };
    auto allowed=[&](std::initializer_list<const char*> fields) {
        if(tuning_json::fields(op,fields,validation,path,"invalid_operation")) return true;
        const auto& e=validation.errors.front();result.errorCode=e.code;result.errorPath=e.path;result.errorMessage=e.message;return false;
    };
    if(name=="import_tuning_scl") {
        if(!allowed({"op","id","text","name"})) return false;
        auto id=harmony_json::string(json_object_get(op,"id"));json_t* text=json_object_get(op,"text");json_t* label=json_object_get(op,"name");
        if(!harmony_json::id(id)||!json_is_string(text)||(label&&(!json_is_string(label)||json_string_length(label)>256))) return reject("invalid_operation","","Expected ID, Scala text and optional bounded name");
        json_t* tuning=tuning_json::importScala(harmony_json::string(text),validation,path+".text");
        if(!tuning) {const auto& e=validation.errors.front();result.errorCode=e.code;result.errorPath=e.path;result.errorMessage=e.message;return false;}
        if(label)json_object_set(tuning,"name",label);
        json_t* systems=json_object_get(working,"pitchSystems");if(!systems){systems=json_object();json_object_set_new(working,"pitchSystems",systems);}
        json_t* bank=json_object_get(systems,"tunings");if(!bank){bank=json_object();json_object_set_new(systems,"tunings",bank);}
        json_object_set_new(bank,id.c_str(),tuning);return true;
    }
    if(name=="set_pattern_pitch_context" || name=="set_default_pitch_context") {
        const bool pattern=name=="set_pattern_pitch_context";
        if(!(pattern ? allowed({"op","pattern_id","context_id"}) : allowed({"op","context_id"}))) return false;
        json_t* id=json_object_get(op,"context_id");
        if(!id || (!json_is_null(id) && !harmony_json::id(harmony_json::string(id))))
            return reject("invalid_operation",".context_id","Expected context ID or null");
        json_t* target=nullptr;
        if(pattern) {
            std::string patternId=harmony_json::string(json_object_get(op,"pattern_id"));
            target=json_object_get(json_object_get(working,"patterns"),patternId.c_str());
            if(!target) return reject("object_not_found",".pattern_id","Pattern not found");
        } else {
            target=json_object_get(working,"pitchSystems");
            if(!target) { target=json_object();json_object_set_new(working,"pitchSystems",target); }
        }
        if(json_is_null(id)) json_object_del(target,pattern?"pitchContext":"defaultContext");
        else json_object_set(target,pattern?"pitchContext":"defaultContext",id);
        return true;
    }
    const bool upsert=name.compare(0,7,"upsert_")==0;
    const char* group=name.find("tuning")!=std::string::npos?"tunings":name.find("scale")!=std::string::npos?"scales":"contexts";
    const char* field=std::string(group)=="tunings"?"tuning":std::string(group)=="scales"?"scale":"context";
    if(!(upsert?allowed({"op","id",field}):allowed({"op","id"}))) return false;
    std::string id=harmony_json::string(json_object_get(op,"id"));
    if(!harmony_json::id(id)) return reject("invalid_operation",".id","Invalid definition ID");
    json_t* systems=json_object_get(working,"pitchSystems");
    if(!systems) { systems=json_object();json_object_set_new(working,"pitchSystems",systems); }
    json_t* definitions=json_object_get(systems,group);
    if(!definitions) { definitions=json_object();json_object_set_new(systems,group,definitions); }
    if(upsert) {
        json_t* value=json_object_get(op,field);
        if(!json_is_object(value)) return reject("invalid_operation","","Expected definition object");
        json_object_set(definitions,id.c_str(),value);
    } else {
        if(!json_object_get(definitions,id.c_str())) return reject("object_not_found",".id","Definition not found");
        json_object_del(definitions,id.c_str());
    }
    // References are deliberately checked by the final transaction compiler.
    return true;
}
} // namespace sibyl
