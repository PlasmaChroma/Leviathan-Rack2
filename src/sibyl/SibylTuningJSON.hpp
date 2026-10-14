#pragma once
#include "SibylJSON.hpp"
#include "SibylJSONUtils.hpp"
#include <climits>

namespace sibyl { namespace tuning_json {
inline bool fail(ParseResult& r, const std::string& p, const std::string& message,
                 const char* code = "invalid_tuning") {
    return harmony_json::error(r, p, message, code);
}
inline bool fields(json_t* j, std::initializer_list<const char*> names, ParseResult& r,
                   const std::string& p, const char* code = "invalid_tuning") {
    if (!json_is_object(j)) return fail(r,p,"Expected object",code);
    const char* key; json_t* value;
    json_object_foreach(j,key,value) {
        bool found = false;
        for (const char* n : names) found |= std::string(key)==n;
        if (!found) return fail(r,p+"."+key,"Unknown field",code);
    }
    return true;
}
inline bool offsets(json_t* j, PitchOffsets& out, ParseResult& r, const std::string& path) {
    for(const char* name : {"transposeSteps","transposePeriods","transposeCents"}) {
        json_t* value=json_object_get(j,name);
        if(!value) continue;
        int field=pitchOffsetField(name);
        if(field==2) {
            if(!harmony_json::number(value,-24000,24000)) return fail(r,path+"."+name,"Expected finite cents in [-24000,24000]");
            out.cents=json_number_value(value);
        } else {
            if(!harmony_json::integer(value,INT_MIN,INT_MAX)) return fail(r,path+"."+name,"Expected signed 32-bit integer");
            if(field==0) out.steps=int32_t(json_integer_value(value));
            else out.periods=int32_t(json_integer_value(value));
        }
        out.fields|=uint8_t(1u<<field);
    }
    return true;
}
inline void writeOffsets(json_t* j, const PitchOffsets& out) {
    if(out.fields&1) json_object_set_new(j,"transposeSteps",json_integer(out.steps));
    if(out.fields&2) json_object_set_new(j,"transposePeriods",json_integer(out.periods));
    if(out.fields&4) json_object_set_new(j,"transposeCents",json_real(out.cents));
}
inline bool transformed(const StepEvent& event, const PitchSystems& systems,
                        const AssignmentOverrides* assignment, double& voltage,
                        ParseResult& r, const std::string& path) {
    const PitchOffsets empty;
    const auto& a=assignment ? assignment->pitchOffsets : empty;
    const auto& e=event.pitchOffsets;
    if(((a.fields|e.fields)&1) && !event.nativePitch.lattice)
        return fail(r,path+".transposeSteps","Step offsets require a native lattice pitch","unsupported_pitch_transform");
    const auto& native=event.nativePitch;
    voltage=native.baseV;
    if(native.lattice) {
        const auto& context=systems.contexts.at(native.context);
        const auto& tuning=systems.tunings.at(context.tuning);
        voltage=context.anchorV+tuning.lattice(native.index+int64_t(e.steps)+int64_t(a.steps));
    }
    voltage+=(int64_t(e.periods)+int64_t(a.periods))*native.periodV;
    voltage+=double(event.transposeSemitones)/12.;
    voltage+=e.cents/1200.;
    if(assignment) {
        voltage+=double(assignment->values[TRANSPOSE])/12.;
        voltage+=a.cents/1200.;
    }
    if(!pitchInDomain(voltage)) return fail(r,path,"Effective pitch is outside -10 to 10 V","pitch_out_of_range");
    return true;
}
inline bool metadata(json_t* j, ParseResult& r, const std::string& p) {
    for (const char* name : {"name","description"}) {
        json_t* v=json_object_get(j,name);
        if (v && (!json_is_string(v) || json_string_length(v) > (std::string(name)=="name"?256u:2048u)))
            return fail(r,p+"."+name,"Invalid metadata string");
    }
    if (json_t* source=json_object_get(j,"source")) {
        if (!fields(source,{"format","name","description"},r,p+".source")) return false;
        json_t* format=json_object_get(source,"format");
        if (format && (!json_is_string(format)||json_string_length(format)>64))
            return fail(r,p+".source.format","Invalid format string");
        return metadata(source,r,p+".source");
    }
    return true;
}
// Parse strictly before conversion: no strtod acceptance of suffixes or integer overflow.
inline bool ratio(json_t* value, double& volts, ParseResult& r, const std::string& p) {
    std::string s=harmony_json::string(value);
    uint64_t n=0,d=0; bool slash=false;
    if (s.empty() || s.size()>21) return fail(r,p,"Expected positive integer/integer ratio");
    for (char c : s) {
        if (c=='/' && !slash && n>0) { slash=true; continue; }
        if (c<'0'||c>'9') return fail(r,p,"Invalid ratio");
        uint64_t& part=slash?d:n;
        part=part*10+uint64_t(c-'0');
        if (part>INT_MAX) return fail(r,p,"Ratio component exceeds 2147483647");
    }
    if (!slash || !n || !d) return fail(r,p,"Ratio components must be positive");
    uint64_t a=n,b=d;
    while (b) { uint64_t t=a%b; a=b; b=t; }
    const std::string canonical=std::to_string(n/a)+"/"+std::to_string(d/a);
    json_string_set(value,canonical.c_str());
    volts=std::log2(double(n)/double(d));
    return true;
}
inline bool interval(json_t* j, double& volts, ParseResult& r, const std::string& p) {
    if (!fields(j,{"ratio","cents"},r,p)) return false;
    json_t* ratioJ=json_object_get(j,"ratio"), *cents=json_object_get(j,"cents");
    if (bool(ratioJ)==bool(cents)) return fail(r,p,"Exactly one ratio or cents is required");
    if (ratioJ) return ratio(ratioJ,volts,r,p+".ratio");
    if (!harmony_json::number(cents,-24000,24000)) return fail(r,p+".cents","Invalid cents");
    volts=json_number_value(cents)/1200.; return true;
}
inline bool definitions(json_t* j, PitchSystems& ps, ParseResult& r) {
    if (!j) return true;
    const std::string p="pitchSystems";
    if (!fields(j,{"tunings","scales","contexts","defaultContext"},r,p)) return false;
    for (const char* name : {"tunings","scales","contexts"}) {
        json_t* v=json_object_get(j,name);
        if (v && (!json_is_object(v) || json_object_size(v)>(std::string(name)=="tunings"?128u:256u)))
            return fail(r,p+"."+name,"Definition count/type exceeds limits");
    }
    const char* id; json_t* v; size_t totalPositions=0;
    json_object_foreach(json_object_get(j,"tunings"),id,v) {
        std::string path=p+".tunings."+id;
        if (!harmony_json::id(id)) return fail(r,path,"Invalid tuning ID");
        if (!fields(v,{"kind","divisions","period","positions","name","description","source"},r,path) || !metadata(v,r,path)) return false;
        PitchTuning t;
        if (!interval(json_object_get(v,"period"),t.periodV,r,path+".period")) return false;
        if (t.periodV<1./1200. || t.periodV>4.) return fail(r,path+".period","Period must be 1 through 4800 cents");
        const auto kind=harmony_json::string(json_object_get(v,"kind"));
        if (kind=="equal") {
            if (!harmony_json::integer(json_object_get(v,"divisions"),1,1024) || json_object_get(v,"positions"))
                return fail(r,path,"Equal tuning requires divisions 1..1024 and no positions");
            t.divisions=int(json_integer_value(json_object_get(v,"divisions")));
        } else if (kind=="table") {
            json_t* positions=json_object_get(v,"positions");
            if (!json_is_array(positions)||json_array_size(positions)<1||json_array_size(positions)>1024||json_object_get(v,"divisions"))
                return fail(r,path,"Table requires 1..1024 positions and no divisions");
            totalPositions+=json_array_size(positions);
            if (totalPositions>65536) return fail(r,path,"Total table positions exceed 65536","capacity_exceeded");
            size_t i; json_t* position;
            json_array_foreach(positions,i,position) {
                double volts;
                const std::string at=path+".positions["+std::to_string(i)+"]";
                if (!interval(position,volts,r,at)) return false;
                if ((i==0 && volts!=0.) || volts>=t.periodV || (i && volts<=t.positionsV.back()))
                    return fail(r,at,"Positions must start at unison and strictly increase below period");
                t.positionsV.push_back(volts);
            }
            t.divisions=int(t.positionsV.size());
        } else return fail(r,path+".kind","Expected equal or table");
        ps.tunings.emplace(id,std::move(t));
    }
    json_object_foreach(json_object_get(j,"scales"),id,v) {
        const std::string path=p+".scales."+id;
        if (!harmony_json::id(id) || !fields(v,{"tuning","steps","name","description","source"},r,path,"invalid_pitch_scale") || !metadata(v,r,path))
            return fail(r,path,"Invalid scale definition","invalid_pitch_scale");
        PitchScale s; s.tuning=harmony_json::string(json_object_get(v,"tuning"));
        auto t=ps.tunings.find(s.tuning); json_t* steps=json_object_get(v,"steps");
        if (t==ps.tunings.end() || !json_is_array(steps)||json_array_size(steps)<1||json_array_size(steps)>size_t(t->second.divisions))
            return fail(r,path,"Unknown tuning or invalid scale size","invalid_pitch_scale");
        size_t i; json_t* step;
        json_array_foreach(steps,i,step) {
            if (!harmony_json::integer(step,0,t->second.divisions-1)) return fail(r,path+".steps","Invalid scale step","invalid_pitch_scale");
            int n=int(json_integer_value(step));
            if ((!i && n!=0) || (i && n<=s.steps.back())) return fail(r,path+".steps","Scale must start at zero and strictly increase","invalid_pitch_scale");
            s.steps.push_back(n);
        }
        ps.scales.emplace(id,std::move(s));
    }
    json_object_foreach(json_object_get(j,"contexts"),id,v) {
        const std::string path=p+".contexts."+id;
        if (!harmony_json::id(id) || !fields(v,{"tuning","scale","anchor","name","description","source"},r,path) || !metadata(v,r,path))
            return fail(r,path,"Invalid context","unresolved_pitch_context");
        PitchContext c; c.tuning=harmony_json::string(json_object_get(v,"tuning"));
        c.scale=harmony_json::string(json_object_get(v,"scale"));
        if (ps.tunings.count(c.tuning)==0 || (json_object_get(v,"scale") && (ps.scales.count(c.scale)==0 || ps.scales.at(c.scale).tuning!=c.tuning)))
            return fail(r,path,"Unresolved or mismatched tuning/scale","unresolved_pitch_context");
        json_t* anchor=json_object_get(v,"anchor");
        if (!fields(anchor,{"note","pitchV","frequencyHz"},r,path+".anchor") || json_object_size(anchor)!=1)
            return fail(r,path+".anchor","Exactly one anchor representation is required");
        if (json_t* note=json_object_get(anchor,"note")) {
            int semitone=0;
            if (!harmony_json::note(note,semitone,r,path+".anchor.note")) return false;
            c.anchorV=double(semitone)/12.;
        } else if (json_t* voltage=json_object_get(anchor,"pitchV")) {
            if (!harmony_json::number(voltage,-10,10)) return fail(r,path+".anchor","Invalid anchor voltage");
            c.anchorV=json_number_value(voltage);
        } else {
            json_t* hz=json_object_get(anchor,"frequencyHz");
            if (!json_is_number(hz)||!std::isfinite(json_number_value(hz))||json_number_value(hz)<=0.) return fail(r,path+".anchor","Frequency must be positive and finite");
            c.anchorV=std::log2(json_number_value(hz)/261.6255653005986);
            if (!pitchInDomain(c.anchorV)) return fail(r,path+".anchor","Anchor outside supported voltage range");
        }
        ps.contexts.emplace(id,std::move(c));
    }
    if (json_t* def=json_object_get(j,"defaultContext")) {
        ps.defaultContext=harmony_json::string(def);
        if (!json_is_string(def)||!ps.contexts.count(ps.defaultContext)) return fail(r,p+".defaultContext","Unknown context","unresolved_pitch_context");
    }
    ps.authored=harmony_json::dump(j); return true;
}
inline bool resolve(json_t* j, const std::string& inherited, const PitchSystems& ps,
                    double& voltage, ParseResult& r, const std::string& p, NativePitch* native = nullptr) {
    if (!fields(j,{"context","step","degree","cents","ratio","periods"},r,p)) return false;
    std::string context=inherited.empty()?ps.defaultContext:inherited;
    if (json_t* explicitContext=json_object_get(j,"context")) {
        context=harmony_json::string(explicitContext);
        if (!harmony_json::id(context)) return fail(r,p+".context","Invalid context ID","unresolved_pitch_context");
    }
    auto c=ps.contexts.find(context);
    if (c==ps.contexts.end()) return fail(r,p,"No resolvable pitch context","unresolved_pitch_context");
    const auto& t=ps.tunings.at(c->second.tuning);
    json_t* coordinate=nullptr; std::string kind;
    for (const char* name : {"step","degree","cents","ratio"}) if (json_t* v=json_object_get(j,name)) {
        if (coordinate) return fail(r,p,"Exactly one tuned coordinate is required");
        coordinate=v; kind=name;
    }
    if (!coordinate) return fail(r,p,"Missing tuned coordinate");
    int64_t periods=0;
    if (json_t* v=json_object_get(j,"periods")) {
        if (!harmony_json::integer(v,INT_MIN,INT_MAX)) return fail(r,p+".periods","Expected signed 32-bit integer");
        periods=json_integer_value(v);
    }
    double relative=0.;
    if (kind=="step" || kind=="degree") {
        if (!harmony_json::integer(coordinate,INT_MIN,INT_MAX)) return fail(r,p+"."+kind,"Expected signed 32-bit integer");
        int64_t k=json_integer_value(coordinate);
        if (kind=="degree" && !c->second.scale.empty()) k=pitchDegreeIndex(int32_t(k),t.divisions,ps.scales.at(c->second.scale).steps);
        // Signed 32-bit coordinates and N<=1024 keep these intermediates safely in int64.
        relative=t.lattice(k+periods*t.divisions);
        if(native) { native->lattice=true; native->index=k+periods*t.divisions; }
    } else {
        if (kind=="ratio") { if (!ratio(coordinate,relative,r,p+".ratio")) return false; }
        else {
            if (!harmony_json::number(coordinate,-24000,24000)) return fail(r,p+".cents","Invalid cents");
            relative=json_number_value(coordinate)/1200.;
        }
        relative+=double(periods)*t.periodV;
    }
    voltage=c->second.anchorV+relative;
    if(native) { native->context=context; native->periodV=t.periodV; native->baseV=voltage; }
    return true; // The caller validates after applying all authored transforms.
}
}} // namespace sibyl::tuning_json
