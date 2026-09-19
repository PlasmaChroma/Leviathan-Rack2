#pragma once
#include "SibylJSON.hpp"
#include <set>

namespace sibyl {
inline void compileAutomation(json_t* root, Composition& comp, ParseResult& result) {
    json_t* definitions=json_object_get(root,"automation");
    if(!definitions) return;
    auto error=[&](const std::string& path,const std::string& message,const char* code="invalid_automation") {
        result.errors.push_back({path,message,code});result.valid=false;return false;
    };
    auto fields=[&](json_t* object,std::initializer_list<const char*> allowed,const std::string& path) {
        if(!json_is_object(object)) return error(path,"Expected object");
        const char* key;json_t* value;
        json_object_foreach(object,key,value) {
            bool found=false;for(const char* name:allowed) if(std::string(key)==name) found=true;
            if(!found) return error(path+"."+key,"Unknown field");
        }
        return true;
    };
    auto number=[&](json_t* value,double lo,double hi,const std::string& path) {
        return (json_is_number(value) && std::isfinite(json_number_value(value)) &&
            json_number_value(value)>=lo && json_number_value(value)<=hi) || error(path,"Expected finite number within bounds");
    };
    auto string=[](json_t* object,const char* key)->std::string {
        json_t* value=json_object_get(object,key);return json_is_string(value)?std::string(json_string_value(value),json_string_length(value)):std::string();
    };
    if(!json_is_object(definitions)) {error("automation","Expected automation map");return;}
    if(json_object_size(definitions)>512) {error("automation","Maximum 512 definitions","capacity_exceeded");return;}
    if(json_object_size(definitions)==0) return;
    size_t totalPoints=0;std::vector<std::string> ids;
    const char* key;json_t* value;
    json_object_foreach(definitions,key,value) {
        ids.emplace_back(key);totalPoints+=json_array_size(json_object_get(value,"points"));
    }
    if(totalPoints>16384) {error("automation","Maximum 16384 total points","capacity_exceeded");return;}
    const size_t bytes=comp.pitchStorageBytes+ids.size()*(sizeof(AutomationCurve)+192)+totalPoints*(sizeof(AutomationPoint)+sizeof(AutomationSegment))+
        comp.arrangement.size()*(sizeof(AutomationRoute)+sizeof(double));
    if(bytes>32u*1024u*1024u) {error("automation","Compiled storage exceeds 32 MiB","capacity_exceeded");return;}
    std::sort(ids.begin(),ids.end());comp.automation.reserve(ids.size());
    comp.sceneBeatPrefixes.reserve(comp.arrangement.size());
    for(const auto& scene:comp.arrangement) {
        comp.sceneBeatPrefixes.push_back(comp.arrangementDuration);
        const double duration=sceneTimelineLength(scene)*double(scene.repeats);
        const double next=comp.arrangementDuration+duration;
        if(!std::isfinite(next) || next<=comp.arrangementDuration) {error("automation","Scene interval cannot be represented on arrangement timeline","capacity_exceeded");return;}
        comp.arrangementDuration=next;
    }
    for(const auto& id:ids) {
        const std::string path="automation."+id;
        if(id.empty() || id.size()>64 || std::any_of(id.begin(),id.end(),[](unsigned char c){return c<32 || c==127;})) {
            error(path,"Invalid automation ID");continue;
        }
        json_t* object=json_object_get(definitions,id.c_str());
        if(!fields(object,{"target","scope","clock","mode","enabled","transitionMs","points"},path)) continue;
        AutomationCurve curve;curve.id=id;
        json_t* target=json_object_get(object,"target");
        if(!fields(target,{"track","lane"},path+".target")) continue;
        curve.trackId=string(target,"track");std::string lane=string(target,"lane");
        auto track=std::find_if(comp.tracks.begin(),comp.tracks.end(),[&](const TrackDef& t){return t.id==curve.trackId;});
        if(track==comp.tracks.end()) {error(path+".target.track","Undefined track");continue;}
        curve.channel=track->channel;
        if(lane!="mod" && lane!="mod2" && lane!="mod3") {error(path+".target.lane","Expected mod, mod2 or mod3");continue;}
        curve.lane=lane=="mod"?0:lane=="mod2"?1:2;
        json_t* scope=json_object_get(object,"scope");
        if(!fields(scope,{"scene","arrangement"},path+".scope") || json_object_size(scope)!=1) {error(path+".scope","Specify one scope");continue;}
        const std::string clock=string(object,"clock");
        if(json_object_get(scope,"arrangement")) {
            if(!json_is_true(json_object_get(scope,"arrangement")) || clock!="arrangement") {error(path+".scope","Arrangement scope requires true and arrangement clock");continue;}
            curve.clock=AutomationClock::ARRANGEMENT;curve.duration=comp.arrangementDuration;
        } else {
            curve.sceneId=string(scope,"scene");
            auto scene=std::find_if(comp.arrangement.begin(),comp.arrangement.end(),[&](const Scene& s){return s.id==curve.sceneId;});
            if(scene==comp.arrangement.end()) {error(path+".scope.scene","Undefined scene");continue;}
            if(clock!="sceneRepeat" && clock!="sceneVisit") {error(path+".clock","Expected sceneRepeat or sceneVisit");continue;}
            curve.clock=clock=="sceneRepeat"?AutomationClock::SCENE_REPEAT:AutomationClock::SCENE_VISIT;
            curve.duration=sceneTimelineLength(*scene)*(curve.clock==AutomationClock::SCENE_VISIT?double(scene->repeats):1.);
        }
        json_t* enabled=json_object_get(object,"enabled");
        if(enabled && !json_is_boolean(enabled)) {error(path+".enabled","Expected boolean");continue;}
        curve.enabled=!enabled || json_is_true(enabled);
        std::string mode=string(object,"mode");
        if(json_object_get(object,"mode") && mode!="replace" && mode!="add") {error(path+".mode","Expected replace or add");continue;}
        curve.add=mode=="add";
        json_t* transition=json_object_get(object,"transitionMs");
        if(transition) {if(!number(transition,0,1000,path+".transitionMs"))continue;curve.transitionMs=json_number_value(transition);}
        json_t* points=json_object_get(object,"points");
        if(!json_is_array(points) || json_array_size(points)<1 || json_array_size(points)>1024) {error(path+".points","Expected 1-1024 ordered points");continue;}
        curve.points.reserve(json_array_size(points));bool valid=true;size_t i;json_t* point;
        json_array_foreach(points,i,point) {
            std::string p=path+".points["+std::to_string(i)+"]";
            if(!fields(point,{"beat","value","shape"},p) || !number(json_object_get(point,"beat"),0,curve.duration,p+".beat") ||
                    !number(json_object_get(point,"value"),-10,10,p+".value")) {valid=false;break;}
            AutomationPoint compiled;compiled.beat=json_number_value(json_object_get(point,"beat"));compiled.value=json_number_value(json_object_get(point,"value"));
            std::string shape=string(point,"shape");
            if(json_object_get(point,"shape") && shape!="step" && shape!="linear" && shape!="smoothstep") {error(p+".shape","Unknown interpolation shape");valid=false;break;}
            compiled.shape=shape=="step"?AutomationShape::STEP:shape=="smoothstep"?AutomationShape::SMOOTHSTEP:AutomationShape::LINEAR;
            if((i==0 && compiled.beat!=0.) || (i>0 && compiled.beat<=curve.points.back().beat)) {error(p+".beat","First point must be zero and subsequent beats strictly increasing");valid=false;break;}
            curve.points.push_back(compiled);
        }
        if(!valid) continue;
        if(curve.points.back().shape!=AutomationShape::LINEAR) result.warnings.push_back({path+".points","Final point shape has no outgoing segment","unused_field"});
        curve.segments.reserve(curve.points.size()-1);
        for(size_t i=0;i+1<curve.points.size();++i) {
            const auto& a=curve.points[i];const auto& b=curve.points[i+1];AutomationSegment segment;
            segment.begin=a.beat;segment.end=b.beat;segment.inverseDuration=1./(b.beat-a.beat);segment.a=a.value;
            if(!std::isfinite(segment.inverseDuration)) {error(path+".points","Segment reciprocal is not representable","capacity_exceeded");valid=false;break;}
            double delta=b.value-a.value;
            if(a.shape==AutomationShape::LINEAR)segment.b=delta;
            if(a.shape==AutomationShape::SMOOTHSTEP){segment.c=3.*delta;segment.d=-2.*delta;}
            curve.segments.push_back(segment);
        }
        if(valid) comp.automation.push_back(std::move(curve));
    }
    AutomationRoute arrangement;arrangement.fill(-1);
    std::vector<AutomationRoute> scenes(comp.arrangement.size());for(auto& route:scenes)route.fill(-1);
    for(size_t i=0;i<comp.automation.size();++i) {
        const auto& curve=comp.automation[i];if(!curve.enabled)continue;
        int* slot=nullptr;int lane=curve.channel*3+curve.lane;
        if(curve.clock==AutomationClock::ARRANGEMENT)slot=&arrangement[lane];
        else for(size_t s=0;s<comp.arrangement.size();++s)if(comp.arrangement[s].id==curve.sceneId){slot=&scenes[s][lane];break;}
        if(slot && *slot>=0)error("automation."+curve.id,"Multiple enabled curves target the same lane and specificity","automation_conflict");
        else if(slot)*slot=int(i);
    }
    comp.automationRoutes=std::move(scenes);
    for(auto& route:comp.automationRoutes)for(int lane=0;lane<48;++lane)if(route[lane]<0)route[lane]=arrangement[lane];
}

inline json_t* automationToJson(const AutomationCurve& curve) {
    json_t* object=json_object();
    json_object_set_new(object,"target",json_pack("{s:s,s:s}","track",curve.trackId.c_str(),"lane",curve.lane==0?"mod":curve.lane==1?"mod2":"mod3"));
    json_object_set_new(object,"scope",curve.clock==AutomationClock::ARRANGEMENT?json_pack("{s:b}","arrangement",1):json_pack("{s:s}","scene",curve.sceneId.c_str()));
    json_object_set_new(object,"clock",json_string(curve.clock==AutomationClock::ARRANGEMENT?"arrangement":curve.clock==AutomationClock::SCENE_REPEAT?"sceneRepeat":"sceneVisit"));
    json_object_set_new(object,"mode",json_string(curve.add?"add":"replace"));json_object_set_new(object,"enabled",json_boolean(curve.enabled));
    json_object_set_new(object,"transitionMs",json_real(curve.transitionMs));json_t* points=json_array();
    for(const auto& point:curve.points) json_array_append_new(points,json_pack("{s:f,s:f,s:s}","beat",point.beat,"value",point.value,"shape",
        point.shape==AutomationShape::STEP?"step":point.shape==AutomationShape::SMOOTHSTEP?"smoothstep":"linear"));
    json_object_set_new(object,"points",points);return object;
}
inline std::string serializeAutomationView(const Composition& comp,json_t* request) {
    auto fail=[](const char* message){json_t* root=json_pack("{s:b,s:{s:s,s:s}}","ok",0,"error","code","invalid_request","message",message);
        char* text=json_dumps(root,JSON_COMPACT);std::string out=text;free(text);json_decref(root);return out;};
    json_t* id=json_object_get(request,"id");if(!json_is_string(id))return fail("Automation view requires id");
    const AutomationCurve* curve=nullptr;for(const auto& c:comp.automation)if(c.id==json_string_value(id)){curve=&c;break;}
    if(!curve)return fail("Automation does not exist");
    json_t* beats=json_object_get(request,"sample_beats");
    if(beats && (!json_is_array(beats)||json_array_size(beats)>256))return fail("sample_beats must contain at most 256 coordinates");
    size_t i;json_t* beat;json_array_foreach(beats,i,beat)
        if(!json_is_number(beat)||!std::isfinite(json_number_value(beat))||json_number_value(beat)<0.||json_number_value(beat)>curve->duration)
            return fail("Sample coordinate outside curve scope");
    json_t* root=json_pack("{s:b,s:i,s:i,s:s,s:s}","ok",1,"revision",comp.revision,"schemaVersion",4,"view","automation","id",curve->id.c_str());
    json_object_set_new(root,"automation",automationToJson(*curve));
    json_t* derived=json_pack("{s:f,s:i}","durationBeats",curve->duration,"segmentCount",int(curve->segments.size()));
    json_t* samples=json_array();size_t cursor=0;
    json_array_foreach(beats,i,beat)json_array_append_new(samples,json_pack("{s:f,s:f}","beat",json_number_value(beat),"value",sampleAutomation(*curve,json_number_value(beat),cursor)));
    json_object_set_new(derived,"samples",samples);json_object_set_new(root,"derived",derived);
    char* text=json_dumps(root,JSON_COMPACT);std::string out=text;free(text);json_decref(root);return out;
}
} // namespace sibyl
