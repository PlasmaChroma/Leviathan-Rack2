#pragma once
#include "SibylPitchEdit.hpp"

namespace sibyl {
inline bool retuneSelected(json_t* working,json_t* pattern,const std::vector<json_t*>& selected,
                           json_t* op,EditResult& result,NoteChange& change,const std::string& path) {
    auto error=[&](const char* code,const char* message) {
        result.errorCode=code;result.errorMessage=message;result.errorPath=path;return false;
    };
    auto propagate=[&](const ParseResult& parsed) {
        if(parsed.errors.empty()) return error("validation_failed","Cannot resolve source pitch");
        const auto& e=parsed.errors.front();result.errorCode=e.code;result.errorPath=e.path;result.errorMessage=e.message;return false;
    };
    const auto mode=harmony_json::string(json_object_get(op,"mode"));
    const auto target=harmony_json::string(json_object_get(op,"target"));
    const auto contextId=harmony_json::string(json_object_get(op,"target_context"));
    json_t* tieJ=json_object_get(op,"tie_break");
    const auto tie=tieJ?harmony_json::string(tieJ):"lower";
    json_t* maxJ=json_object_get(op,"max_error_cents");
    if((mode!="nearest" && mode!="preserve" && mode!="reinterpret") || (target!="tuning" && target!="scale") ||
       (tie!="lower" && tie!="higher") || (maxJ && !harmony_json::number(maxJ,0,24000)))
        return error("invalid_operation","Invalid retune mode, target, tie break or maximum error");
    for(auto* note:selected) if(json_object_get(note,"harmonic"))
        return error("dynamic_pitch_requires_harmony_edit","Retune supports static events only");
    // Only event-level state is compiled: scene transforms are intentionally excluded.
    json_t* mini=json_object();
    json_object_set_new(mini,"schemaVersion",json_integer(4));
    for(const char* field:{"meta","pitchSystems"}) if(json_t* v=json_object_get(working,field)) json_object_set(mini,field,v);
    json_t* p=json_deep_copy(pattern);json_t* notes=json_array();
    for(auto* note:selected) json_array_append(notes,note);
    json_object_set_new(p,"steps",notes);
    json_t* patterns=json_object();json_object_set_new(patterns,"source",p);json_object_set_new(mini,"patterns",patterns);
    auto parsed=parseCompositionJson(harmony_json::dump(mini),0);json_decref(mini);
    if(!parsed.valid) return propagate(parsed);
    const auto& comp=*parsed.composition;const auto& systems=comp.pitchSystems;
    auto context=systems.contexts.find(contextId);
    if(context==systems.contexts.end()) return error("unresolved_pitch_context","Target context does not exist");
    const auto& tuning=systems.tunings.at(context->second.tuning);
    std::vector<int> positions;
    if(target=="scale" && !context->second.scale.empty()) positions=systems.scales.at(context->second.scale).steps;
    else for(int i=0;i<tuning.divisions;++i) positions.push_back(i);
    json_t* report=json_pack("{s:I,s:s,s:s,s:s,s:b}","operationIndex",json_int_t(change.operationIndex),"operation","retune_notes","mode",mode.c_str(),"sourceDomain","eventBeforeSceneOverrides","truncated",int(selected.size()>change.idLimit));
    std::unique_ptr<json_t,void(*)(json_t*)> reportOwner(report,json_decref);
    json_t* details=json_array();json_object_set_new(report,"notes",details);
    json_object_set_new(report,"total",json_integer(selected.size()));
    const auto& source=comp.patterns.at("source");
    for(auto* note:selected) {
        const int step=int(json_integer_value(json_object_get(note,"step")));
        const StepEvent& event=source.steps[size_t(source.eventIndexByStep[size_t(step)])];
        double before=event.compiledPitchV;
        ParseResult check;check.valid=true;
        if((event.pitchType==PitchType::TUNED || event.pitchOffsets.fields) &&
           !tuning_json::transformed(event,systems,nullptr,before,check,path)) return propagate(check);
        double grid=before,after=before,residual=0.;
        json_t* tuned=nullptr;
        if(mode=="reinterpret") {
            json_t* old=json_object_get(note,"tuned");const char* coordinate=target=="tuning"?"step":"degree";
            if(!old || !json_object_get(old,coordinate)) return error("unsupported_pitch_transform","Reinterpret requires matching native coordinate kind");
            tuned=json_deep_copy(old);json_object_set_new(tuned,"context",json_string(contextId.c_str()));
            StepEvent next=event;next.nativePitch=NativePitch();
            double base=0.;
            bool valid=tuning_json::resolve(tuned,"",systems,base,check,path,&next.nativePitch) && tuning_json::transformed(next,systems,nullptr,after,check,path);
            if(!valid) {json_decref(tuned);return propagate(check);}
            grid=after;
        } else {
            double distance=std::numeric_limits<double>::infinity();int64_t coordinate=0;
            const int64_t period=int64_t(std::floor((before-context->second.anchorV)/tuning.periodV));
            for(int delta=-1;delta<=1;++delta) for(size_t i=0;i<positions.size();++i) {
                int64_t q=period+delta;
                double value=context->second.anchorV+tuning.lattice(q*tuning.divisions+positions[i]);
                double d=std::abs(value-before);
                if(d<distance-1e-12 || (std::abs(d-distance)<=1e-12 && (tie=="lower"?value<grid:value>grid))) {
                    distance=d;grid=value;coordinate=target=="tuning"?q*tuning.divisions+positions[i]:q*int64_t(positions.size())+int64_t(i);
                }
            }
            if(coordinate<INT_MIN || coordinate>INT_MAX) return error("pitch_out_of_range","Target coordinate exceeds signed 32-bit range");
            residual=mode=="preserve"?(before-grid)*1200.:0.;
            after=mode=="preserve"?before:grid;
            tuned=json_pack("{s:s,s:I}","context",contextId.c_str(),target=="tuning"?"step":"degree",json_int_t(coordinate));
        }
        if(maxJ && std::abs(grid-before)*1200.>json_number_value(maxJ)+1e-9) {
            json_decref(tuned);return error("quantization_error_exceeded","Selected target exceeds maximum error");
        }
        if(!pitchInDomain(after)) {json_decref(tuned);return error("pitch_out_of_range","Retuned event is outside pitch bounds");}
        if(mode!="reinterpret") {
            for(const char* f:{"note","pitchV","degree","octave","transposeSemitones","transposeSteps","transposePeriods","transposeCents"}) json_object_del(note,f);
            if(mode=="preserve") json_object_set_new(note,"transposeCents",json_real(residual));
        }
        json_object_set_new(note,"tuned",tuned);++change.updated;
        if(json_array_size(details)<change.idLimit)
            json_array_append_new(details,json_pack("{s:s,s:f,s:f,s:f,s:f,s:f}","noteId",event.id.c_str(),"beforePitchV",before,"afterPitchV",after,"gridPitchV",grid,"errorCents",(grid-before)*1200.,"residualCents",residual));
    }
    result.pitchChanges.push_back(harmony_json::dump(report));
    return true;
}
} // namespace sibyl
