#pragma once
#include "SibylTuningJSON.hpp"
#include <sstream>

namespace sibyl {
inline json_t* nativePitchDetails(const Composition& comp,const StepEvent& event,double volts) {
    json_t* out=json_pack("{s:f,s:f}","effectivePitchV",volts,"nominalFrequencyHz",261.6255653005986*std::exp2(volts));
    // This is a display label only; it never feeds pitch compilation.
    int semitone=int(std::floor(volts*12.+.5));
    static const char* names[]={"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
    int octave=semitone/12, pc=semitone%12;if(pc<0){pc+=12;--octave;}
    std::string label=std::string(names[pc])+std::to_string(octave+4);
    json_object_set_new(out,"nearestConventionalNote",json_string(label.c_str()));
    json_object_set_new(out,"deviationCents",json_real(volts*1200.-semitone*100.));
    if(event.pitchType==PitchType::TUNED) {
        json_t* authored=json_loads(event.tuned.c_str(),0,nullptr);
        for(const char* unit:{"step","degree","ratio","cents"}) if(json_object_get(authored,unit)) json_object_set_new(out,"unit",json_string(unit));
        json_object_set_new(out,"authored",authored);
        const auto& native=event.nativePitch;const auto& context=comp.pitchSystems.contexts.at(native.context);
        json_object_set_new(out,"context",json_string(native.context.c_str()));
        json_object_set_new(out,"tuning",json_string(context.tuning.c_str()));
        if(!context.scale.empty()) json_object_set_new(out,"scale",json_string(context.scale.c_str()));
        json_object_set_new(out,"periodCents",json_real(native.periodV*1200.));
        json_object_set_new(out,"lattice",json_boolean(native.lattice));
        if(native.lattice) json_object_set_new(out,"nativeIndex",json_integer(native.index));
    }
    json_object_set_new(out,"transposeCents",json_real(event.pitchOffsets.cents));
    return out;
}
inline std::string serializePitchView(const Composition& comp,json_t* request) {
    ParseResult check;check.valid=true;
    auto error=[](const char* code,const char* message) {
        json_t* j=json_pack("{s:b,s:{s:s,s:s}}","ok",0,"error","code",code,"message",message);
        std::string text=harmony_json::dump(j);json_decref(j);return text;
    };
    if(!tuning_json::fields(request,{"view","id","page_size","cursor"},check,"request","invalid_request"))
        return error("invalid_request","Unknown pitch view field");
    if(json_t* id=json_object_get(request,"id")) if(!json_is_string(id)) return error("invalid_request","id must be a string");
    const auto view=harmony_json::string(json_object_get(request,"view"));
    const auto id=harmony_json::string(json_object_get(request,"id"));
    json_t* definitions=comp.pitchSystems.authored.empty()?json_object():json_loads(comp.pitchSystems.authored.c_str(),0,nullptr);
    std::unique_ptr<json_t,void(*)(json_t*)> owner(definitions,json_decref);
    json_t* out=json_pack("{s:b,s:i,s:i,s:s}","ok",1,"revision",comp.revision,"schemaVersion",4,"view",view.c_str());
    std::unique_ptr<json_t,void(*)(json_t*)> outputOwner(out,json_decref);
    if(view=="pitch_context") {
        auto context=comp.pitchSystems.contexts.find(id);
        if(context==comp.pitchSystems.contexts.end()) return error("unresolved_pitch_context","Unknown context");
        if(json_object_get(request,"cursor")||json_object_get(request,"page_size")) return error("invalid_request","Context view is not paged");
        json_object_set(out,"context",json_object_get(json_object_get(definitions,"contexts"),id.c_str()));
        json_object_set(out,"tuning",json_object_get(json_object_get(definitions,"tunings"),context->second.tuning.c_str()));
        if(!context->second.scale.empty()) json_object_set(out,"scale",json_object_get(json_object_get(definitions,"scales"),context->second.scale.c_str()));
        json_object_set_new(out,"anchorPitchV",json_real(context->second.anchorV));
    } else {
        const std::string group=id.empty()?"tunings":id;
        if(group!="tunings" && group!="scales" && group!="contexts") return error("invalid_request","id must be tunings, scales or contexts");
        json_t* sizeJ=json_object_get(request,"page_size");
        if(sizeJ && !harmony_json::integer(sizeJ,1,128)) return error("invalid_request","Page size must be 1..128");
        size_t page=sizeJ?size_t(json_integer_value(sizeJ)):16,offset=0;
        if(json_t* cursor=json_object_get(request,"cursor")) {
            std::string text=harmony_json::string(cursor);
            const std::string prefix="p7:"+std::to_string(comp.revision)+":"+group+":"+std::to_string(page)+":";
            if(text.compare(0,prefix.size(),prefix)!=0) return error("revision_conflict","Cursor revision or query changed");
            const auto suffix=text.substr(prefix.size());
            if(suffix.empty()||suffix.size()>6||suffix.find_first_not_of("0123456789")!=std::string::npos) return error("invalid_request","Invalid cursor");
            offset=size_t(std::stoul(suffix));
        }
        json_t* entries=json_object_get(definitions,group.c_str());std::vector<std::string> ids;
        const char* key;json_t* value;json_object_foreach(entries,key,value) ids.emplace_back(key);
        std::sort(ids.begin(),ids.end());
        if(offset>ids.size()) return error("invalid_request","Cursor outside definitions");
        json_t* values=json_object();const size_t end=std::min(ids.size(),offset+page);
        for(size_t i=offset;i<end;++i) json_object_set(values,ids[i].c_str(),json_object_get(entries,ids[i].c_str()));
        json_object_set_new(out,"definitions",values);
        json_object_set_new(out,"group",json_string(group.c_str()));
        json_object_set_new(out,"total",json_integer(ids.size()));
        json_object_set_new(out,"defaultContext",comp.pitchSystems.defaultContext.empty()?json_null():json_string(comp.pitchSystems.defaultContext.c_str()));
        if(end<ids.size()) {
            std::string cursor="p7:"+std::to_string(comp.revision)+":"+group+":"+std::to_string(page)+":"+std::to_string(end);
            json_object_set_new(out,"cursor",json_string(cursor.c_str()));
        } else json_object_set_new(out,"cursor",json_null());
    }
    return harmony_json::dump(out);
}
} // namespace sibyl
