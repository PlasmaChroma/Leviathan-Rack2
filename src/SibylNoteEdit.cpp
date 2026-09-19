#include "SibylNoteEdit.hpp"
#include "SibylRetune.hpp"
#include "SibylPitchView.hpp"
#include <algorithm>
#include <cmath>
#include <set>
#include <sstream>
#include <iomanip>
#include <limits>

namespace sibyl {
namespace {
struct Owned {
    json_t* p;
    explicit Owned(json_t* v) : p(v) {}
    ~Owned() { json_decref(p); }
    Owned(const Owned&) = delete;
};
bool fail(EditResult& r, const std::string& code, const std::string& path, const std::string& message) {
    r.errorCode = code; r.errorPath = path; r.errorMessage = message; return false;
}
std::string str(json_t* v) { return json_is_string(v) ? std::string(json_string_value(v), json_string_length(v)) : ""; }
bool integer(json_t* v, int64_t low, int64_t high) {
    return json_is_integer(v) && json_integer_value(v) >= low && json_integer_value(v) <= high;
}
bool finite(json_t* v) { return json_is_number(v) && std::isfinite(json_number_value(v)); }
bool fields(json_t* object, const std::set<std::string>& allowed, EditResult& r, const std::string& path, const char* code = "invalid_operation") {
    if (!json_is_object(object)) return fail(r, code, path, "Expected object");
    const char* key; json_t* value;
    json_object_foreach(object, key, value) if (!allowed.count(key)) return fail(r, code, path + "." + key, "Unknown field");
    return true;
}
bool boolField(json_t* o, const char* key, EditResult& r, const std::string& path) {
    json_t* v = json_object_get(o,key);
    return !v || json_is_boolean(v) || fail(r,"invalid_operation",path+"."+key,"Expected boolean");
}
const std::set<std::string> eventFields = {"id","step","pitchV","note","degree","octave","transposeSemitones","transposeSteps","transposePeriods","transposeCents","gate","velocity","probability","mod","mod2","mod3","glideMs","microshift","ratchets","tie","evolve","observation","condition","harmonic","tuned"};
const std::set<std::string> optionalFields = {"octave","transposeSemitones","transposeSteps","transposePeriods","transposeCents","gate","velocity","probability","mod","mod2","mod3","glideMs","microshift","ratchets","tie","evolve","observation","condition"};
bool expressionRange(const std::string& key, double& lo, double& hi) {
    lo = 0;
    if (key == "gate") hi = 1024;
    else if (key == "velocity" || key == "probability") hi = 1;
    else if (key == "glideMs") hi = 3600000;
    else if (key == "microshift") {lo = -.499999999; hi = .499999999;}
    else if (key == "mod" || key == "mod2" || key == "mod3") {lo = -10; hi = 10;}
    else return false;
    return true;
}
bool parseFailure(const ParseResult& p, EditResult& r) {
    r.errors = p.errors;
    if (p.errors.empty()) return fail(r,"validation_failed","$","Invalid candidate");
    return fail(r,p.errors[0].code,p.errors[0].path,p.errors[0].message);
}
void removeByIds(json_t* events, const std::set<std::string>& ids) {
    for (size_t i = json_array_size(events); i > 0; --i)
        if (ids.count(str(json_object_get(json_array_get(events,i-1),"id")))) json_array_remove(events,i-1);
}
}

bool selectNotes(json_t* pattern, json_t* selector, std::vector<json_t*>& selected, EditResult& r, const std::string& path) {
    if (!fields(selector,{"ids","steps","step_range","where","order","limit","all"},r,path,"invalid_selector")) return false;
    const bool hasAll = json_object_get(selector,"all");
    bool restriction = false;
    for (const char* key : {"ids","steps","step_range","where"}) restriction |= json_object_get(selector,key) != nullptr;
    if ((hasAll && (!json_is_true(json_object_get(selector,"all")) || restriction)) ||
        (!hasAll && !restriction && !(json_object_get(selector,"order") && json_object_get(selector,"limit"))))
        return fail(r,"invalid_selector",path,"Use restrictions, all:true, or order with limit");
    std::string order = json_object_get(selector,"order") ? str(json_object_get(selector,"order")) : "stepAsc";
    if (order != "stepAsc" && order != "stepDesc") return fail(r,"invalid_selector",path+".order","Expected stepAsc or stepDesc");
    json_t* limit = json_object_get(selector,"limit");
    if (limit && !integer(limit,1,1024)) return fail(r,"invalid_selector",path+".limit","Expected integer 1–1024");
    std::set<std::string> ids;
    std::set<int64_t> steps;
    json_t* idList = json_object_get(selector,"ids");
    json_t* stepList = json_object_get(selector,"steps");
    size_t i; json_t* v;
    if (idList) {
        if (!json_is_array(idList) || json_array_size(idList)==0 || json_array_size(idList)>1024) return fail(r,"invalid_selector",path+".ids","Expected 1–1024 IDs");
        json_array_foreach(idList,i,v) {
            if (!json_is_string(v) || str(v).empty() || !ids.insert(str(v)).second) return fail(r,"invalid_selector",path+".ids["+std::to_string(i)+"]","Invalid or duplicate ID");
            bool found=false; size_t j; json_t* event;
            json_array_foreach(json_object_get(pattern,"steps"),j,event) found |= str(json_object_get(event,"id")) == str(v);
            if (!found) return fail(r,"note_not_found",path+".ids["+std::to_string(i)+"]","Note '"+str(v)+"' not found");
        }
    }
    json_t* lengthJ = json_object_get(pattern,"length");
    if (!integer(lengthJ,1,1024)) return fail(r,"invalid_selector",path,"Invalid pattern length");
    const int64_t length = json_integer_value(lengthJ);
    if (stepList) {
        if (!json_is_array(stepList) || json_array_size(stepList)==0 || json_array_size(stepList)>1024) return fail(r,"invalid_selector",path+".steps","Expected 1–1024 steps");
        json_array_foreach(stepList,i,v) if (!integer(v,0,length-1) || !steps.insert(json_integer_value(v)).second)
            return fail(r,"invalid_selector",path+".steps["+std::to_string(i)+"]","Invalid or duplicate step");
    }
    json_t* range = json_object_get(selector,"step_range");
    if (range && (!json_is_array(range) || json_array_size(range)!=2 || !integer(json_array_get(range,0),0,length) || !integer(json_array_get(range,1),0,length) || json_integer_value(json_array_get(range,0))>json_integer_value(json_array_get(range,1))))
        return fail(r,"invalid_selector",path+".step_range","Expected bounded [start,end)");
    json_t* where = json_object_get(selector,"where");
    if (where) {
        if (!fields(where,{"velocity","probability","gate","evolve","tie"},r,path+".where","invalid_selector") || json_object_size(where)==0)
            return fail(r,"invalid_selector",path+".where","Expected nonempty typed predicates");
        const char* key; json_t* predicate;
        json_object_foreach(where,key,predicate) {
            std::string pp=path+".where."+key;
            if (std::string(key)=="tie" || std::string(key)=="evolve") {
                if (!json_is_boolean(predicate)) return fail(r,"invalid_selector",pp,"Expected boolean");
            } else {
                if (!fields(predicate,{"eq","lt","lte","gt","gte"},r,pp,"invalid_selector") || json_object_size(predicate)==0) return fail(r,"invalid_selector",pp,"Expected numeric comparisons");
                const char* cmp; json_t* number;
                json_object_foreach(predicate,cmp,number) if (!finite(number)) return fail(r,"invalid_selector",pp+"."+cmp,"Expected finite number");
            }
        }
    }
    json_t* event;
    json_array_foreach(json_object_get(pattern,"steps"),i,event) {
        int64_t step=json_integer_value(json_object_get(event,"step"));
        if (idList && !ids.count(str(json_object_get(event,"id")))) continue;
        if (stepList && !steps.count(step)) continue;
        if (range && (step<json_integer_value(json_array_get(range,0)) || step>=json_integer_value(json_array_get(range,1)))) continue;
        bool match=true; const char* key; json_t* pred;
        json_object_foreach(where,key,pred) {
            std::string field=key; json_t* value=json_object_get(event,key);
            if (field=="tie" || field=="evolve") {
                bool actual=value ? json_is_true(value) : field=="evolve";
                match &= actual==bool(json_is_true(pred));
            } else {
                if (!value && field!="probability") {match=false; continue;}
                double number=value ? json_number_value(value) : 1;
                const char* cmp; json_t* n;
                json_object_foreach(pred,cmp,n) {
                    double v=json_number_value(n); std::string op=cmp;
                    match &= op=="eq" ? number==v : op=="lt" ? number<v : op=="lte" ? number<=v : op=="gt" ? number>v : number>=v;
                }
            }
        }
        if (match) selected.push_back(event);
    }
    std::sort(selected.begin(),selected.end(),[&](json_t* a,json_t* b) {
        int64_t x=json_integer_value(json_object_get(a,"step")),y=json_integer_value(json_object_get(b,"step"));
        return order=="stepAsc" ? x<y : x>y;
    });
    if (limit && selected.size()>size_t(json_integer_value(limit))) selected.resize(json_integer_value(limit));
    return true;
}

bool isNoteOperation(const std::string& name) {
    return name=="retune_notes" || name=="update_notes" || name=="insert_notes" || name=="delete_notes" || name=="transpose_notes" || name=="rotate_notes" || name=="duplicate_notes";
}

bool applyNoteOperation(json_t* working, json_t* op, size_t index, EditResult& r) {
    const std::string path="operations["+std::to_string(index)+"]", name=str(json_object_get(op,"op"));
    std::set<std::string> allowed={"op","pattern_id","report_id_limit"};
    if(name!="insert_notes") for(const char* f:{"selector","expect_count","allow_empty"}) allowed.insert(f);
    if(name=="update_notes") for(const char* f:{"set","unset","adjust","clamp","resolve_defaults_for_track"}) allowed.insert(f);
    if(name=="insert_notes") for(const char* f:{"notes","collision"}) allowed.insert(f);
    if(name=="transpose_notes") {allowed.insert("semitones");allowed.insert("degrees");allowed.insert("interval");}
    if(name=="retune_notes") for(const char* f:{"target_context","mode","target","tie_break","max_error_cents"}) allowed.insert(f);
    if(name=="rotate_notes") {allowed.insert("steps");allowed.insert("collision");}
    if(name=="duplicate_notes") for(const char* f:{"offset_steps","collision","wrap","destination_pattern_id","copy_observations","return_id_mapping","pitch_context_policy"}) allowed.insert(f);
    if(!fields(op,allowed,r,path)) return false;
    std::string id=str(json_object_get(op,"pattern_id"));
    json_t* patterns=json_object_get(working,"patterns"),*pattern=json_object_get(patterns,id.c_str());
    if(id.empty() || !pattern) return fail(r,"object_not_found",path+".pattern_id","Pattern not found");
    for(const char* f:{"allow_empty","clamp","wrap","copy_observations","return_id_mapping"}) if(!boolField(op,f,r,path)) return false;
    json_t* collisionJ=json_object_get(op,"collision");
    const std::string collision=collisionJ ? str(collisionJ) : "error";
    if(collision!="error" && collision!="replace") return fail(r,"invalid_operation",path+".collision","Expected error or replace");
    json_t* count=json_object_get(op,"expect_count");
    if(count && !integer(count,0,1024)) return fail(r,"invalid_operation",path+".expect_count","Expected integer 0–1024");
    NoteChange change; change.operationIndex=index;change.patternId=id;
    json_t* reportLimit=json_object_get(op,"report_id_limit");
    if(reportLimit && !integer(reportLimit,1,1024)) return fail(r,"invalid_operation",path+".report_id_limit","Expected integer 1–1024");
    if(reportLimit) change.idLimit=json_integer_value(reportLimit);
    std::vector<json_t*> selected;
    if(name!="insert_notes" && !selectNotes(pattern,json_object_get(op,"selector"),selected,r,path+".selector")) return false;
    change.matched=selected.size();
    if(count && selected.size()!=size_t(json_integer_value(count))) return fail(r,"selection_count_mismatch",path+".expect_count","Selected count differs from expected count");
    if(name!="insert_notes" && selected.empty() && !json_is_true(json_object_get(op,"allow_empty"))) return fail(r,"selection_empty",path+".selector","Selection is empty");
    for(auto* n:selected) change.noteIds.push_back(str(json_object_get(n,"id")));
    json_t* events=json_object_get(pattern,"steps");
    if(!events) {events=json_array();json_object_set_new(pattern,"steps",events);}
    if(name=="retune_notes") {
        if(!retuneSelected(working,pattern,selected,op,r,change,path)) return false;
    } else if(name=="delete_notes") {
        removeByIds(events,std::set<std::string>(change.noteIds.begin(),change.noteIds.end()));change.deleted=selected.size();
    } else if(name=="transpose_notes") {
        json_t* semis=json_object_get(op,"semitones"),*degrees=json_object_get(op,"degrees");
        json_t* interval=json_object_get(op,"interval");
        if(interval) {
            if(semis || degrees || !fields(interval,{"steps","periods","cents","ratio"},r,path+".interval") || json_object_size(interval)!=1)
                return fail(r,"invalid_operation",path,"Exactly one transposition unit is required");
            const char* field=json_object_get(interval,"steps")?"transposeSteps":json_object_get(interval,"periods")?"transposePeriods":"transposeCents";
            const bool continuous=std::string(field)=="transposeCents";
            json_t* amount=json_object_get(interval,continuous?"cents":std::string(field)=="transposeSteps"?"steps":"periods");
            double cents=0.;
            if(continuous) {
                if(json_t* ratio=json_object_get(interval,"ratio")) {
                    ParseResult parsed;parsed.valid=true;double volts=0.;
                    Owned copy(json_deep_copy(ratio));
                    if(!tuning_json::ratio(copy.p,volts,parsed,path+".interval.ratio")) return parseFailure(parsed,r);
                    cents=volts*1200.;
                    Owned report(json_pack("{s:I,s:s,s:s,s:f}","operationIndex",json_int_t(index),"operation","transpose_notes","requestedRatio",str(ratio).c_str(),"resolvedCents",cents));
            r.pitchChanges.push_back(harmony_json::dump(report.p));
                } else if(!finite(amount)) return fail(r,"invalid_operation",path,"Expected finite cents");
                else cents=json_number_value(amount);
            } else if(!integer(amount,INT32_MIN,INT32_MAX)) return fail(r,"invalid_operation",path,"Expected integer interval");
            for(auto* n:selected) {
                if(continuous) {
                    double value=json_number_value(json_object_get(n,field))+cents;
                    if(!std::isfinite(value)||std::abs(value)>24000.) return fail(r,"pitch_out_of_range",path,"Cent offset exceeds bounds");
                    json_object_set_new(n,field,json_real(value));
                } else {
                    int64_t value=json_integer_value(json_object_get(n,field))+json_integer_value(amount);
                    if(value<INT32_MIN||value>INT32_MAX) return fail(r,"pitch_out_of_range",path,"Integer offset overflow");
                    json_object_set_new(n,field,json_integer(value));
                }
                ++change.updated;
            }
        } else {
        if(bool(semis)==bool(degrees) || !integer(semis?semis:degrees,INT32_MIN,INT32_MAX)) return fail(r,"invalid_operation",path,"Exactly one integer semitones or degrees is required");
        for(auto* n:selected) {
            const char* field=semis?"transposeSemitones":"degree";
            json_t* pitchObject=degrees && json_object_get(n,"tuned") ? json_object_get(n,"tuned") : n;
            if(degrees && !json_object_get(pitchObject,"degree")) return fail(r,"unsupported_pitch_transform",path+".degrees","Degree transpose requires only degree events");
            json_t* current=json_object_get(pitchObject,field);
            if(current && !integer(current,semis?-120:INT32_MIN,semis?120:INT32_MAX)) return fail(r,"pitch_out_of_range",path,"Existing pitch offset is invalid");
            int64_t value=json_integer_value(current)+json_integer_value(semis?semis:degrees);
            if(value<(semis?-120:INT32_MIN) || value>(semis?120:INT32_MAX)) return fail(r,"pitch_out_of_range",path,"Transpose exceeds supported range");
            if(json_integer_value(semis?semis:degrees)!=0) {json_object_set_new(pitchObject,field,json_integer(value));++change.updated;}
        }
        }
    } else if(name=="update_notes") {
        json_t* set=json_object_get(op,"set"),*unset=json_object_get(op,"unset"),*adjust=json_object_get(op,"adjust");
        if(!set&&!unset&&!adjust) return fail(r,"invalid_operation",path,"Expected set, unset or adjust");
        std::set<std::string> touched;
        auto setAllowed=eventFields;setAllowed.erase("id");setAllowed.erase("step");
        if(set&&!fields(set,setAllowed,r,path+".set")) return false;
        if (set) {
            const char* f; json_t* v;
            json_object_foreach(set,f,v) {
                double low,high;std::string field=f;
                if(expressionRange(field,low,high) && (!finite(v)||json_number_value(v)<low||json_number_value(v)>high))
                    return fail(r,"invalid_operation",path+".set."+f,"Value outside authored range");
                if((field=="degree"||field=="octave")&&!integer(v,INT32_MIN,INT32_MAX)) return fail(r,"invalid_operation",path+".set."+f,"Expected signed 32-bit integer");
                if(field=="transposeSemitones"&&!integer(v,-120,120)) return fail(r,"invalid_operation",path+".set."+f,"Expected integer -120 to 120");
                if(field=="ratchets"&&!integer(v,1,16)) return fail(r,"invalid_operation",path+".set."+f,"Expected integer 1�16");
                if((field=="tie"||field=="evolve")&&!json_is_boolean(v)) return fail(r,"invalid_operation",path+".set."+f,"Expected boolean");
                if(field=="pitchV"&&(!finite(v)||json_number_value(v)<-10||json_number_value(v)>10)) return fail(r,"pitch_out_of_range",path+".set.pitchV","Expected pitch within -10 to 10 V");
                if(field=="note"&&!json_is_string(v)) return fail(r,"invalid_operation",path+".set.note","Expected scientific note string");
                if(field=="observation"&&!fields(v,{"octaviaModuleId","monitors","preFrames","postFrames","label"},r,path+".set.observation")) return false;
            }
        }
        if(adjust&&!json_is_object(adjust)) return fail(r,"invalid_operation",path+".adjust","Expected object");
        if(unset&&!json_is_array(unset)) return fail(r,"invalid_operation",path+".unset","Expected array");
        const char* key;json_t* value;
        json_object_foreach(set,key,value) touched.insert(key);
        size_t i;json_t* u;
        json_array_foreach(unset,i,u) if(!json_is_string(u)||!optionalFields.count(str(u))||!touched.insert(str(u)).second) return fail(r,"invalid_operation",path+".unset","Invalid, duplicate or overlapping unset field");
        json_object_foreach(adjust,key,value) {
            double lo,hi;
            if(!expressionRange(key,lo,hi)||!touched.insert(key).second||!fields(value,{"multiply","add"},r,path+".adjust."+key)) return fail(r,"invalid_operation",path+".adjust."+key,"Invalid or overlapping adjustment");
            for(const char* f:{"multiply","add"}) if(json_object_get(value,f)&&!finite(json_object_get(value,f))) return fail(r,"invalid_operation",path+".adjust."+key+"."+f,"Expected finite number");
        }
        int pitches=0;for(const char* f:{"note","degree","pitchV","harmonic","tuned"}) pitches+=json_object_get(set,f)!=nullptr;
        if(pitches>1) return fail(r,"invalid_operation",path+".set","Set only one pitch representation");
        json_t* defaults=nullptr;json_t* trackJ=json_object_get(op,"resolve_defaults_for_track");
        if(trackJ) {
            json_t* track;
            json_array_foreach(json_object_get(working,"tracks"),i,track) if(str(json_object_get(track,"id"))==str(trackJ)) defaults=track;
            if(!json_is_string(trackJ)||!defaults) return fail(r,"object_not_found",path+".resolve_defaults_for_track","Track not found");
        }
        for(auto* n:selected) {
            Owned before(json_deep_copy(n));
            if(pitches) {
                for(const char* f:{"note","degree","pitchV","harmonic","tuned"}) if(!json_object_get(set,f)) json_object_del(n,f);
                if(!json_object_get(set,"degree")) json_object_del(n,"octave");
            }
            json_object_foreach(set,key,value) json_object_set(n,key,value);
            json_array_foreach(unset,i,u) json_object_del(n,json_string_value(u));
            json_object_foreach(adjust,key,value) {
                json_t* old=json_object_get(n,key);double base=0;std::string f=key;
                if(old) {if(!finite(old)) return fail(r,"invalid_operation",path+".adjust."+key,"Cannot adjust nonnumeric value");base=json_number_value(old);}
                else if(f=="probability") base=1;
                else if(f=="velocity"||f=="gate") {
                    if(!defaults) return fail(r,"inherited_value_requires_track",path+".adjust."+key,"Specify resolve_defaults_for_track for inherited value");
                    json_t* inherited=json_object_get(defaults,f=="velocity"?"defaultVelocity":"defaultGate");
                    base=inherited?json_number_value(inherited):.5;
                }
                json_t* mul=json_object_get(value,"multiply"),*add=json_object_get(value,"add");
                double result=base*(mul?json_number_value(mul):1)+(add?json_number_value(add):0),lo,hi;expressionRange(f,lo,hi);
                if(!std::isfinite(result)) return fail(r,"invalid_operation",path+".adjust."+key,"Adjustment overflow");
                if(result<lo||result>hi) {
                    if(!json_is_true(json_object_get(op,"clamp"))) return fail(r,"validation_failed",path+".adjust."+key,"Adjusted value is out of range; use clamp:true explicitly");
                    result=std::max(lo,std::min(hi,result));++change.clamped;
                }
                json_object_set_new(n,key,json_real(result));
            }
            if(!json_equal(n,before.p)) ++change.updated;
        }
    } else {
        const bool duplicate=name=="duplicate_notes",insert=name=="insert_notes";
        const auto policy=json_object_get(op,"pitch_context_policy")?str(json_object_get(op,"pitch_context_policy")):"source";
        if(policy!="source" && policy!="destination") return fail(r,"invalid_operation",path+".pitch_context_policy","Expected source or destination");
        json_t* destination=pattern;
        json_t* destId=json_object_get(op,"destination_pattern_id");
        if(destId) {
            destination=json_object_get(patterns,str(destId).c_str());
            if(!json_is_string(destId)||!destination) return fail(r,"object_not_found",path+".destination_pattern_id","Destination pattern not found");
            if(!json_equal(json_object_get(destination,"resolution"),json_object_get(pattern,"resolution"))) return fail(r,"invalid_operation",path+".destination_pattern_id","Pattern resolutions must match");
            change.patternId=str(destId);
        }
        if(duplicate && destination!=pattern && std::any_of(selected.begin(),selected.end(),[](json_t* n){return json_object_get(n,"tuned")!=nullptr || json_object_get(n,"harmonic")!=nullptr;})) {
            Owned report(json_pack("{s:I,s:s,s:s,s:s,s:s}","operationIndex",json_int_t(index),"operation","duplicate_notes","sourcePattern",id.c_str(),"destinationPattern",change.patternId.c_str(),"pitchContextPolicy",policy.c_str()));
            json_object_set_new(report.p,"harmonicBindingPolicy",json_string("destination"));
            r.pitchChanges.push_back(harmony_json::dump(report.p));
        }
        json_t* destEvents=json_object_get(destination,"steps");
        if(!destEvents) {destEvents=json_array();json_object_set_new(destination,"steps",destEvents);}
        int64_t length=json_object_get(destination,"length") ? json_integer_value(json_object_get(destination,"length")) : 16;
        if(length<1||length>1024) return fail(r,"validation_failed",path,"Invalid destination length");
        Owned copies(json_array());std::vector<std::string> sourceIds;
        if(insert) {
            json_t* notes=json_object_get(op,"notes");
            if(!json_is_array(notes)||json_array_size(notes)<1||json_array_size(notes)>1024) return fail(r,"invalid_operation",path+".notes","Expected 1–1024 notes");
            size_t i;json_t* n;
            json_array_foreach(notes,i,n) {
                if(!fields(n,eventFields,r,path+".notes["+std::to_string(i)+"]")) return false;
                json_array_append_new(copies.p,json_deep_copy(n));
            }
        } else {
            const char* offsetField=duplicate?"offset_steps":"steps";json_t* offset=json_object_get(op,offsetField);
            if(!integer(offset,INT32_MIN,INT32_MAX)) return fail(r,"invalid_operation",path+"."+offsetField,"Expected integer step offset");
            bool copiedObservation=false;
            for(auto* n:selected) {
                json_t* copy=json_deep_copy(n);json_array_append_new(copies.p,copy);
                if(!integer(json_object_get(n,"step"),0,1023)) return fail(r,"invalid_operation",path,"Invalid source step");
                int64_t step=json_integer_value(json_object_get(n,"step"))+json_integer_value(offset);
                if(!duplicate||json_is_true(json_object_get(op,"wrap"))) step=(step%length+length)%length;
                json_object_set_new(copy,"step",json_integer(step));
                if(duplicate && destination!=pattern) {
                    json_t* harmonic=json_object_get(copy,"harmonic");json_t* range=json_object_get(harmonic,"range");
                    for(json_t* endpoint:{copy,json_object_get(harmonic,"reference"),json_object_get(range,"min"),json_object_get(range,"max")}) {
                        json_t* tuned=json_object_get(endpoint,"tuned");if(!tuned)continue;
                        if(policy=="destination")json_object_del(tuned,"context");
                        else if(!json_object_get(tuned,"context")) {
                            json_t* context=json_object_get(pattern,"pitchContext");
                            if(!context)context=json_object_get(json_object_get(working,"pitchSystems"),"defaultContext");
                            if(!json_is_string(context))return fail(r,"unresolved_pitch_context",path,"Source native reference has no context");
                            json_object_set(tuned,"context",context);
                        }
                    }
                }
                if(duplicate) {
                    sourceIds.push_back(str(json_object_get(n,"id")));json_object_del(copy,"id");
                    if(json_is_false(json_object_get(op,"copy_observations"))) json_object_del(copy,"observation");
                    else copiedObservation |= json_object_get(copy,"observation")!=nullptr;
                }
            }
            if(copiedObservation) r.warnings.push_back({path,"Observation markers copied to duplicated notes","observations_copied"});
        }
        std::set<int64_t> destinations;size_t i;json_t* copy;
        json_array_foreach(copies.p,i,copy) {
            json_t* step=json_object_get(copy,"step");
            if(!integer(step,0,length-1)) return fail(r,"invalid_operation",path+".steps","Destination step outside pattern");
            if(!destinations.insert(json_integer_value(step)).second) return fail(r,"step_collision",path,"Multiple copies have the same destination");
        }
        std::set<std::string> removals;
        if(!duplicate&&!insert) removals.insert(change.noteIds.begin(),change.noteIds.end());
        json_t* n;
        json_array_foreach(destEvents,i,n) if(destinations.count(json_integer_value(json_object_get(n,"step")))&&!removals.count(str(json_object_get(n,"id")))) {
            if(duplicate && destination==pattern && std::find(change.noteIds.begin(),change.noteIds.end(),str(json_object_get(n,"id")))!=change.noteIds.end())
                return fail(r,"step_collision",path,"Duplication must retain every selected source note");
            if(collision=="error") return fail(r,"step_collision",path,"Destination occupied by note '"+str(json_object_get(n,"id"))+"'");
            change.displacedIds.push_back(str(json_object_get(n,"id")));removals.insert(str(json_object_get(n,"id")));
        }
        removeByIds(destEvents,removals);
        json_array_foreach(copies.p,i,copy) json_array_append(destEvents,copy);
        if(json_array_size(destEvents)>1024) return fail(r,"capacity_exceeded",path,"Maximum 1024 events per pattern");
        ParseResult normalized;normalized.valid=true;
        if(!normalizeNoteIds(destination,"patterns."+change.patternId,normalized)) return parseFailure(normalized,r);
        if(insert||duplicate) {
            change.noteIds.clear();
            json_array_foreach(copies.p,i,copy) {
                std::string created=str(json_object_get(copy,"id"));change.noteIds.push_back(created);
                if(duplicate&&json_is_true(json_object_get(op,"return_id_mapping"))) change.createdIds.emplace_back(sourceIds[i],created);
            }
            change.inserted=json_array_size(copies.p);
        } else change.updated=json_integer_value(json_object_get(op,"steps"))%length==0?0:selected.size();
        change.deleted=change.displacedIds.size();
    }
    if(!change.updated&&!change.inserted&&!change.deleted) r.warnings.push_back({path,"Operation made no change","no_effect"});
    r.changes.push_back(std::move(change));return true;
}

json_t* editChangesJson(const EditResult& r) {
    json_t* changes=json_array();
    for(const auto& c:r.changes) {
        json_t* o=json_object();json_array_append_new(changes,o);
        json_object_set_new(o,"operationIndex",json_integer(c.operationIndex));json_object_set_new(o,"patternId",json_string(c.patternId.c_str()));
        json_object_set_new(o,"matched",json_integer(c.matched));json_object_set_new(o,"updated",json_integer(c.updated));json_object_set_new(o,"inserted",json_integer(c.inserted));json_object_set_new(o,"deleted",json_integer(c.deleted));json_object_set_new(o,"clamped",json_integer(c.clamped));
        auto ids=[&](const char* field,const std::vector<std::string>& list) {
            json_t* a=json_array();for(size_t i=0;i<std::min(list.size(),c.idLimit);++i) json_array_append_new(a,json_string(list[i].c_str()));json_object_set_new(o,field,a);
        };
        ids("noteIds",c.noteIds);ids("displacedIds",c.displacedIds);
        json_object_set_new(o,"idsTruncated",json_boolean(c.noteIds.size()>c.idLimit||c.displacedIds.size()>c.idLimit||c.createdIds.size()>c.idLimit));
        json_object_set_new(o,"noteIdsTotal",json_integer(c.noteIds.size()));json_object_set_new(o,"displacedIdsTotal",json_integer(c.displacedIds.size()));
        if(!c.createdIds.empty()) {
            json_t* a=json_array();for(size_t i=0;i<std::min(c.createdIds.size(),c.idLimit);++i) json_array_append_new(a,json_pack("{s:s,s:s}","source",c.createdIds[i].first.c_str(),"created",c.createdIds[i].second.c_str()));json_object_set_new(o,"createdIds",a);
        }
    }
    for(const auto* reports : {&r.voicingChanges,&r.pitchChanges}) for(const auto& encoded:*reports) {
        json_error_t error;json_t* report=json_loads(encoded.c_str(),0,&error);
        if(!report)continue;
        const json_int_t index=json_integer_value(json_object_get(report,"operationIndex"));
        size_t at=0;while(at<json_array_size(changes)&&json_integer_value(json_object_get(json_array_get(changes,at),"operationIndex"))<=index)++at;
        json_array_insert_new(changes,at,report);
    }
    return changes;
}

std::string serializeNotesView(const Composition& comp,json_t* request) {
    EditResult r;
    auto error=[&]() {
        Owned out(json_pack("{s:b,s:{s:s,s:s,s:s}}","ok",0,"error","code",r.errorCode.c_str(),"path",r.errorPath.c_str(),"message",r.errorMessage.c_str()));
        char* encoded=json_dumps(out.p,JSON_COMPACT);std::string result=encoded;free(encoded);return result;
    };
    if(!fields(request,{"view","pattern_id","selector","fields","page_size","cursor"},r,"request","invalid_request")) return error();
    std::string id=str(json_object_get(request,"pattern_id"));
    if(id.empty()||!comp.patterns.count(id)) {fail(r,"object_not_found","pattern_id","Pattern not found");return error();}
    json_error_t e;
    Owned wrapper(json_loads(serializePatternViewJson(comp,id).c_str(),0,&e));
    json_t* pattern=json_object_get(wrapper.p,"pattern");
    Owned defaultSelector(json_pack("{s:b}","all",1));
    json_t* selector=json_object_get(request,"selector");if(!selector)selector=defaultSelector.p;
    std::vector<json_t*> selected;if(!selectNotes(pattern,selector,selected,r,"selector")) return error();
    json_t* projection=json_object_get(request,"fields");bool full=str(projection)=="full";
    std::set<std::string> projectionFields={"id","step","note","degree","octave","pitchV","transposeSemitones","velocity","probability","condition","harmonic","tuned"};
    if(projection&&!full) {
        if(!json_is_array(projection)||json_array_size(projection)==0||json_array_size(projection)>eventFields.size()) {fail(r,"invalid_request","fields","Expected full or nonempty field list");return error();}
        projectionFields.clear();size_t i;json_t* v;json_array_foreach(projection,i,v) if(!json_is_string(v)||(!eventFields.count(str(v)) && str(v)!="effectivePitchV" && str(v)!="pitchDetails")||!projectionFields.insert(str(v)).second) {fail(r,"invalid_request","fields","Unknown or repeated projection field");return error();}
    }
    json_t* sizeJ=json_object_get(request,"page_size");
    if(sizeJ&&!integer(sizeJ,1,256)) {fail(r,"invalid_request","page_size","Expected integer 1–256");return error();}
    size_t size=sizeJ?json_integer_value(sizeJ):128,offset=0;
    // Stable opaque fingerprint binds page cursors to the complete selection and projection.
    Owned binding(json_pack("{s:s,s:O,s:O,s:i}","pattern",id.c_str(),"selector",selector,"fields",projection?projection:json_null(),"size",int(size)));
    char* canonical=json_dumps(binding.p,JSON_COMPACT|JSON_SORT_KEYS);uint64_t hash=1469598103934665603ULL;
    for(const unsigned char* c=(unsigned char*)canonical;*c;++c) hash=(hash^*c)*1099511628211ULL;
    free(canonical);
    json_t* cursor=json_object_get(request,"cursor");
    if(cursor) {
        std::string token=str(cursor);std::istringstream stream(token);uint64_t version,revision,fingerprint,position;char a,b,c;
        if(token.size()>100||!(stream>>version>>a>>revision>>b>>fingerprint>>c>>position)||a!=':'||b!=':'||c!=':'||version!=1||stream.peek()!=EOF) {fail(r,"invalid_request","cursor","Invalid cursor");return error();}
        if(revision!=uint64_t(comp.revision)) {fail(r,"revision_conflict","cursor","Cursor belongs to a different revision");return error();}
        if(fingerprint!=hash||position>selected.size()) {fail(r,"invalid_request","cursor","Cursor does not match this query");return error();}
        offset=position;
    }
    Owned out(json_pack("{s:b,s:i,s:i,s:s,s:s,s:i}","ok",1,"revision",comp.revision,"schemaVersion",4,"view","notes","patternId",id.c_str(),"total",int(selected.size())));
    json_t* notes=json_array();json_object_set_new(out.p,"notes",notes);
    size_t end=std::min(offset+size,selected.size());
    for(size_t i=offset;i<end;++i) {
        json_t* n=full?json_deep_copy(selected[i]):json_object();
        if(!full) for(const auto& f:projectionFields) if(json_t* v=json_object_get(selected[i],f.c_str())) json_object_set(n,f.c_str(),v);
        if((projectionFields.count("effectivePitchV") || projectionFields.count("pitchDetails")) && !full) {
            const int step=json_integer_value(json_object_get(selected[i],"step"));
            const auto& compiled=comp.patterns.at(id);
            for(const auto& event:compiled.steps) if(event.step==step)
                json_object_set_new(n,"derived",event.pitchType==PitchType::HARMONIC ? json_pack("{s:b}","requiresContext",1) : (projectionFields.count("pitchDetails")?nativePitchDetails(comp,event,event.compiledPitchV):json_pack("{s:f}","effectivePitchV",double(event.compiledPitchV))));
        }
        json_array_append_new(notes,n);
    }
    if(end<selected.size()) {
        std::string token="1:"+std::to_string(comp.revision)+":"+std::to_string(hash)+":"+std::to_string(end);json_object_set_new(out.p,"cursor",json_string(token.c_str()));
    } else json_object_set_new(out.p,"cursor",json_null());
    char* encoded=json_dumps(out.p,JSON_COMPACT);std::string result=encoded;free(encoded);return result;
}
}
