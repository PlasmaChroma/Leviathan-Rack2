namespace {
std::string automationCurve(const std::string& points=R"([{"beat":0,"value":0},{"beat":32,"value":5}])",
        const std::string& clock="sceneVisit",const std::string& mode="replace",int transition=0) {
    return std::string(R"({"target":{"track":"v","lane":"mod"},"scope":)")+
        (clock=="arrangement"?R"({"arrangement":true})":R"({"scene":"s"})")+
        ",\"clock\":\""+clock+"\",\"mode\":\""+mode+"\",\"transitionMs\":"+std::to_string(transition)+",\"points\":"+points+"}";
}
std::string automationFixture(const std::string& definitions) {
    return R"({"schemaVersion":3,"meta":{"bpm":60},"tracks":[{"id":"v","channel":0}],"patterns":{"p":{"length":4,"resolution":"1/4","steps":[{"step":0,"note":"C4","gate":1024,"mod":1,"mod2":2,"mod3":3}]}},"arrangement":[{"id":"s","lengthBeats":8,"repeats":4,"tracks":{"v":"p"}},{"id":"t","lengthBeats":8,"phaseMode":"continue","tracks":{"v":"p"}}],"automation":)"+definitions+"}";
}
void testAutomation() {
    using namespace sibyl;
    auto decode=[](const std::string& text){json_error_t error{};return json_loads(text.c_str(),0,&error);};
    auto compile=[](json_t* root,int revision=1){char* text=json_dumps(root,JSON_COMPACT);auto r=parseCompositionJson(text,revision);free(text);return r;};
    auto parse=[&](const std::string& curve){return parseCompositionJson(automationFixture("{\"rise\":"+curve+"}"),1);};
    auto base=parse(automationCurve());check(base.valid,"P4 linear automation fixture compiles");if(!base.valid)return;
    auto round=parseCompositionJson(serializeFullCompositionJson(*base.composition),2);
    check(round.valid && round.composition->automation[0].audibleEquals(base.composition->automation[0]),"P4 authored curve round trip");
    size_t cursor=0;bool samples=true;for(int i=0;i<=4;++i)samples &= std::abs(sampleAutomation(base.composition->automation[0],i*8.,cursor)-i*1.25)<1e-12;
    check(samples,"A01 linear 32-beat sweep has expected quarter samples and endpoint");
    auto step=parse(automationCurve(R"([{"beat":0,"value":-2,"shape":"step"},{"beat":4,"value":2},{"beat":8,"value":6}])"));
    cursor=0;check(sampleAutomation(step.composition->automation[0],3.999,cursor)==-2 && sampleAutomation(step.composition->automation[0],4,cursor)==2
        && sampleAutomation(step.composition->automation[0],20,cursor)==6,"P4 step boundary and held tail");
    auto smooth=parse(automationCurve(R"([{"beat":0,"value":0,"shape":"smoothstep"},{"beat":4,"value":8,"shape":"step"}])"));
    cursor=0;check(smooth.valid && sampleAutomation(smooth.composition->automation[0],1,cursor)==1.25 && !smooth.warnings.empty(),"P4 smoothstep polynomial and unused final shape warning");
    for(const char* points : {"[]",R"([{"beat":1,"value":0}])",R"([{"beat":0,"value":0},{"beat":0,"value":1}])",
            R"([{"beat":0,"value":0},{"beat":33,"value":1}])",R"([{"beat":0,"value":11}])",
            R"([{"beat":0,"value":0,"shape":"spline"}])",R"([{"beat":0,"value":0,"extra":1}])"})
        check(!parse(automationCurve(points)).valid,"A12 malformed points rejected");
    for(const char* key: {"target","scope","clock","mode","enabled","transitionMs","points"}) {
        json_t* root=decode(automationFixture("{\"rise\":"+automationCurve()+"}"));
        json_object_set_new(json_object_get(json_object_get(root,"automation"),"rise"),key,json_null());
        check(!compile(root).valid,"P4 wrong scalar/container type rejected");json_decref(root);
    }
    auto conflict=parseCompositionJson(automationFixture("{\"a\":"+automationCurve()+",\"b\":"+automationCurve()+"}"),1);
    check(!conflict.valid && conflict.errors[0].code=="automation_conflict","A08 duplicate specificity is automation_conflict");
    auto scopes=parseCompositionJson(automationFixture("{\"a\":"+automationCurve(R"([{"beat":0,"value":-3}])","arrangement")+",\"b\":"+automationCurve(R"([{"beat":0,"value":2}])")+"}"),1);
    check(scopes.valid && scopes.composition->automationRoutes[0][0]==1 && scopes.composition->automationRoutes[1][0]==0,
        "A08 scene curve owns complete visit ahead of arrangement curve");
    {
        json_t* root=decode(automationFixture("{\"rise\":"+automationCurve()+"}"));
        json_t* curve=json_object_get(json_object_get(root,"automation"),"rise");
        json_object_set_new(json_object_get(curve,"target"),"track",json_string("missing"));check(!compile(root).valid,"A12 unresolved target rejected");
        json_object_set_new(json_object_get(curve,"target"),"track",json_string("v"));
        json_object_set_new(json_object_get(curve,"scope"),"arrangement",json_true());check(!compile(root).valid,"P4 mixed scopes rejected");json_decref(root);
    }
    {
        json_t* root=decode(automationFixture("{}"));json_t* definitions=json_object_get(root,"automation");
        json_t* curve=decode(automationCurve(R"([{"beat":0,"value":1}])"));json_object_set_new(curve,"enabled",json_false());
        for(int i=0;i<513;++i)json_object_set(definitions,("c"+std::to_string(i)).c_str(),curve);
        auto r=compile(root);check(!r.valid && r.errors[0].code=="capacity_exceeded","P4 definition capacity checked before route allocation");
        json_object_clear(definitions);json_t* points=json_array();for(int i=0;i<1024;++i)json_array_append_new(points,json_pack("{s:f,s:f}","beat",i/64.,"value",0.));
        json_object_set_new(curve,"points",points);for(int i=0;i<17;++i)json_object_set(definitions,("c"+std::to_string(i)).c_str(),curve);
        r=compile(root);check(!r.valid && r.errors[0].code=="capacity_exceeded","P4 total-point capacity checked before segment compilation");
        json_object_clear(definitions);json_object_set(definitions,"large",curve);r=compile(root);cursor=0;
        check(r.valid && sampleAutomation(r.composition->automation[0],15.97,cursor)==0. && cursor>1000,"P4 seek jumps across large curve with bounded search");
        json_decref(curve);json_decref(root);
    }
    {
        json_t* root=decode(automationFixture("{\"rise\":"+automationCurve(R"([{"beat":0,"value":0},{"beat":0.7,"value":5}])","sceneRepeat")+"}"));
        json_object_set_new(json_array_get(json_object_get(root,"arrangement"),0),"lengthBeats",json_real(.7));
        auto r=compile(root);auto saved=r.valid?parseCompositionJson(serializeFullCompositionJson(*r.composition),2):r;
        check(saved.valid && saved.composition->automation[0].points.back().beat==.7,"P4 fractional scope endpoint survives legacy float timing and double codec round trip");json_decref(root);
    }
    {
        json_t* request=decode(R"({"id":"rise","sample_beats":[0,8,16,24,32]})");
        json_t* response=decode(serializeAutomationView(*base.composition,request));
        check(json_is_true(json_object_get(response,"ok")) && json_array_size(json_object_get(json_object_get(response,"derived"),"samples"))==5,"P4 bounded automation view samples in volts/beats");json_decref(response);
        json_t* beats=json_array();for(int i=0;i<257;++i)json_array_append_new(beats,json_integer(0));json_object_set_new(request,"sample_beats",beats);
        response=decode(serializeAutomationView(*base.composition,request));check(json_is_false(json_object_get(response,"ok")),"P4 sample projection bound enforced");json_decref(response);json_decref(request);
    }
    // Handover arithmetic, including saturation only after the blend.
    auto constant=parse(automationCurve(R"([{"beat":0,"value":10}])","sceneVisit","replace",4));
    {
        AutomationLaneState lane;double output=2.;bool exact=true;
        for(int i=0;i<=4;++i){output=lane.process(&constant.composition->automation[0],0,1,false,0,0,0,2,output,1000);exact &= output==2.+2.*i;}
        check(exact && !lane.blending,"A11 starts weight zero and reaches target at ceil duration samples");
        output=lane.process(nullptr,-1,2,true,0,0,0,2,output,1000);check(output==10.,"P4 curve removal uses outgoing transition");
        for(int i=0;i<4;++i)output=lane.process(nullptr,-1,2,false,0,0,0,2,output,1000);
        check(output==2.,"P4 removal blends back to legacy latch");
        AutomationLaneState limited;output=-10.;for(int i=0;i<=2;++i)output=limited.process(&constant.composition->automation[0],0,1,false,0,0,10,0,output,1000);
        check(output==5.,"P4 blend target is limited once after interpolation");
        AutomationLaneState rate;output=2.;for(int i=0;i<=2;++i)output=rate.process(&constant.composition->automation[0],0,1,false,0,0,0,2,output,1000);
        output=rate.process(&constant.composition->automation[0],0,1,false,0,0,0,2,output,2000);
        check(output==6. && rate.blendSamples==4,"P4 sample-rate change preserves remaining seconds and actual output");
        for(int i=0;i<4;++i)output=rate.process(&constant.composition->automation[0],0,1,false,0,0,0,2,output,2000);
        check(output==10. && !rate.blending,"P4 resampled handover reaches moving destination");
    }
    int64_t frame=0;gAllocationCount=0;gDeallocationCount=0;
    auto tick=[&](SibylModule& module,float sampleRate=1000.f){Module::ProcessArgs args;args.sampleRate=sampleRate;args.sampleTime=1.f/sampleRate;args.frame=frame++;
        gTrackAllocations=true;module.process(args);gTrackAllocations=false;};
    auto transport=[&](SibylModule& module,const char* request){std::string response,error;check(module.handleSibylRequest(SibylControl::Operation::TRANSPORT,request,response,error),"P4 transport accepted");tick(module);};
    auto upsert=[&](const Composition& comp,const char* id,const std::string& curve,int revision){json_t* object=decode(curve);
        json_t* operations=json_pack("[{s:s,s:s,s:O}]","op","upsert_automation","id",id,"automation",object);json_decref(object);
        auto edit=applyCompositionEdit(comp,operations,revision);json_decref(operations);return edit;};
    {
        auto comp=std::const_pointer_cast<Composition>(base.composition);comp->patterns["p"].steps[0].hasProbability=true;comp->patterns["p"].steps[0].probability=0.f;
        SibylModule module;module.acceptComposition(comp,ApplyAt::IMMEDIATE,PhasePolicy::RESTART_ALL);tick(module);
        module.m_runtimeRunning.store(false);
        bool values=true;for(int i=0;i<4;++i){module.m_sceneRepeat=i;module.m_scenePhase=0.;tick(module);values &= std::abs(module.outputs[SibylModule::MOD_OUTPUT].getVoltage()-i*1.25f)<1e-6;}
        check(values && module.outputs[SibylModule::GATE_OUTPUT].getVoltage()==0.f,"A02/A04 sceneVisit sweep advances across silent repeats");
        module.m_sceneRepeat=3;module.m_scenePhase=8.;tick(module);
        check(module.outputs[SibylModule::MOD_OUTPUT].getVoltage()==0.f,"P4 exact scene boundary uses destination ownership");
    }
    {
        auto repeat=parse(automationCurve(R"([{"beat":0,"value":0},{"beat":8,"value":4}])","sceneRepeat"));
        SibylModule module;module.acceptComposition(repeat.composition,ApplyAt::IMMEDIATE,PhasePolicy::RESTART_ALL);tick(module);module.m_runtimeRunning.store(false);
        module.m_sceneRepeat=3;module.m_scenePhase=2.;tick(module);check(std::abs(module.outputs[SibylModule::MOD_OUTPUT].getVoltage()-1.f)<1e-6,"A04 sceneRepeat coordinate ignores earlier repeats");
    }
    {
        json_t* root=decode(automationFixture("{\"rise\":"+automationCurve()+"}"));json_object_del(json_array_get(json_object_get(root,"arrangement"),0),"tracks");
        auto comp=compile(root);json_decref(root);check(comp.valid,"P4 patternless target compiles");
        SibylModule module;module.acceptComposition(comp.composition,ApplyAt::IMMEDIATE,PhasePolicy::RESTART_ALL);tick(module);
        module.m_scenePhase=4.;module.m_runtimeRunning.store(false);tick(module);
        check(std::abs(module.outputs[SibylModule::MOD_OUTPUT].getVoltage()-.625f)<1e-6 && module.outputs[SibylModule::GATE_OUTPUT].getVoltage()==0,"A03 patternless track drives independent MOD output");
        json_t* operations=decode(R"([{"op":"set_scene_assignment","scene_id":"t","track_id":"v","assignment":null},{"op":"delete_track","id":"v"}])");
        check(!applyCompositionEdit(*comp.composition,operations,2).valid,"P4 deleting a referenced automation track is rejected");json_decref(operations);
    }
    {
        auto parsed=parse(automationCurve(R"([{"beat":0,"value":2}])","sceneVisit","add"));
        auto comp=std::const_pointer_cast<Composition>(parsed.composition);auto& ov=comp->arrangement[0].tracks["v"].overrides;
        ov.present=true;ov.fields=1u<<MOD_OFFSET;ov.values[MOD_OFFSET]=.5f;
        Macro macro;macro.id="1";macro.target="track.v.mod";macro.amount=.25f;comp->macros["1"]=macro;
        SibylModule module;module.inputs[SibylModule::MACRO_1_INPUT].channels=1;module.inputs[SibylModule::MACRO_1_INPUT].setVoltage(10.f);
        module.acceptComposition(comp,ApplyAt::IMMEDIATE,PhasePolicy::RESTART_ALL);tick(module);
        check(module.outputs[SibylModule::MOD_OUTPUT].getVoltage()==3.75f && module.m_trackStates[0].rawEventMod[0]==1.f,"A06 add mode does not double-count scene offsets or macros");
        transport(module,R"({"action":"pause","apply_at":"immediate"})");module.inputs[SibylModule::MACRO_1_INPUT].setVoltage(0.f);tick(module);
        check(module.outputs[SibylModule::MOD_OUTPUT].getVoltage()==3.5f,"A09 macros remain live on paused owned lane");
        module.inputs[SibylModule::MACRO_1_INPUT].setVoltage(10.f);transport(module,R"({"action":"restart","target":"patterns","apply_at":"immediate"})");
        check(module.outputs[SibylModule::MOD_OUTPUT].getVoltage()==2.75f,"P4 pattern restart clears automated raw base while paused");
    }
    {
        auto legacy=parseCompositionJson(automationFixture("{}"),1);
        SibylModule module;module.acceptComposition(legacy.composition,ApplyAt::IMMEDIATE,PhasePolicy::RESTART_ALL);tick(module);
        check(module.m_trackStates[0].rawEventMod[0]==1.f,"A13 raw event base maintained without active curve");
        auto added=upsert(*legacy.composition,"rise",automationCurve(R"([{"beat":0,"value":2}])","sceneVisit","add"),2);
        module.acceptComposition(added.composition,ApplyAt::IMMEDIATE,PhasePolicy::PRESERVE);tick(module);
        check(module.outputs[SibylModule::MOD_OUTPUT].getVoltage()==3.f && module.outputs[SibylModule::GATE_OUTPUT].getVoltage()==10.f,"A13 enable add automation mid-note preserves base and gate");
        auto replace=upsert(*added.composition,"rise",automationCurve(R"([{"beat":0,"value":2}])"),3);
        module.acceptComposition(replace.composition,ApplyAt::IMMEDIATE,PhasePolicy::PRESERVE);tick(module);
        check(module.outputs[SibylModule::MOD_OUTPUT].getVoltage()==2.f,"A05 replace ownership overrides explicit note MOD");
    }
    {
        SibylModule module;module.acceptComposition(base.composition,ApplyAt::IMMEDIATE,PhasePolicy::RESTART_ALL);tick(module);
        // This block needs a sounding note, unlike the earlier probability-zero fixture.
        auto sounding=std::make_shared<Composition>(*base.composition);sounding->revision=2;sounding->patterns["p"].steps[0].probability=1.f;
        module.acceptComposition(sounding,ApplyAt::IMMEDIATE,PhasePolicy::RESTART_ALL);tick(module);
        for(int i=0;i<500;++i)tick(module);
        auto revised=upsert(*sounding,"rise",automationCurve(R"([{"beat":0,"value":0},{"beat":32,"value":10}])"),3);
        module.acceptComposition(revised.composition,ApplyAt::IMMEDIATE,PhasePolicy::PRESERVE);
        check(module.m_pendingAdoptionPtr.load()->restartChannelMask==0,"A10 automation-only edit has no note dependency mask");tick(module);
        check(module.outputs[SibylModule::GATE_OUTPUT].getVoltage()==10.f && std::abs(module.outputs[SibylModule::MOD_OUTPUT].getVoltage()-module.m_scenePhase*10./32.)<1e-6,
            "A10 revised curve samples current coordinate without closing gate");
        auto blended=upsert(*revised.composition,"rise",automationCurve(R"([{"beat":0,"value":8}])","sceneVisit","replace",4),4);
        float before=module.outputs[SibylModule::MOD_OUTPUT].getVoltage();module.acceptComposition(blended.composition,ApplyAt::IMMEDIATE,PhasePolicy::PRESERVE);tick(module);
        check(module.outputs[SibylModule::MOD_OUTPUT].getVoltage()==before,"A11 adoption blend starts from actual previous output");tick(module);before=module.outputs[SibylModule::MOD_OUTPUT].getVoltage();
        auto again=upsert(*blended.composition,"rise",automationCurve(R"([{"beat":0,"value":-2}])","sceneVisit","replace",2),5);
        module.acceptComposition(again.composition,ApplyAt::IMMEDIATE,PhasePolicy::PRESERVE);tick(module);
        check(module.outputs[SibylModule::MOD_OUTPUT].getVoltage()==before,"A11 repeated edit restarts blend from current emitted voltage");tick(module);tick(module);
        check(module.outputs[SibylModule::MOD_OUTPUT].getVoltage()==-2.f,"A11 repeated edit reaches incoming target duration");
        std::string future=automationCurve(R"([{"beat":0,"value":4}])");size_t at=future.find("\"scene\":\"s\"");future.replace(at,11,"\"scene\":\"t\"");
        auto futureEdit=upsert(*again.composition,"a_future",future,6);check(futureEdit.valid,"P4 future scene-only edit compiles");
        module.acceptComposition(futureEdit.composition,ApplyAt::IMMEDIATE,PhasePolicy::PRESERVE);
        check(module.m_pendingAdoptionPtr.load()->automationChangeMasks[0]==0 && module.m_pendingAdoptionPtr.load()->automationChangeMasks[1]!=0,"P4 future scene-only edit has scene-local MOD masks");tick(module);
        check(module.outputs[SibylModule::MOD_OUTPUT].getVoltage()==-2.f && !module.m_automationLanes[0].blending,"P4 unchanged lane survives reordered compiled curve indices without handover");
    }
    {
        auto arranged=parse(automationCurve(R"([{"beat":0,"value":0},{"beat":40,"value":10}])","arrangement"));
        SibylModule module;module.acceptComposition(arranged.composition,ApplyAt::IMMEDIATE,PhasePolicy::RESTART_ALL);tick(module);
        transport(module,R"({"action":"select_scene","scene_id":"t","apply_at":"immediate"})");
        check(std::abs(module.outputs[SibylModule::MOD_OUTPUT].getVoltage()-8.f)<.001,"A09 manual jump relocates arrangement score position");
        transport(module,R"({"action":"stop","apply_at":"immediate"})");check(module.outputs[SibylModule::MOD_OUTPUT].getVoltage()==0.f,"A09 stop relocates curve to reset coordinate");
        transport(module,R"({"action":"play","apply_at":"immediate"})");module.m_loopOverride.store(0);module.m_sceneIndex=1;module.m_sceneRepeat=0;module.m_scenePhase=7.9999;
        tick(module);check(!module.m_runtimeRunning.load() && module.outputs[SibylModule::MOD_OUTPUT].getVoltage()==10.f,"A09 nonloop stop holds terminal endpoint");
        module.m_loopOverride.store(1);module.m_runtimeRunning.store(true);module.m_sceneIndex=1;module.m_scenePhase=7.9999;tick(module);
        check(module.m_sceneIndex==0 && module.outputs[SibylModule::MOD_OUTPUT].getVoltage()<.001f,"A09 automatic arrangement wrap restarts score coordinate");
        module.inputs[SibylModule::CLOCK_INPUT].channels=1;module.inputs[SibylModule::CLOCK_INPUT].setVoltage(0.f);float held=module.outputs[SibylModule::MOD_OUTPUT].getVoltage();
        for(int i=0;i<3000;++i)tick(module);
        check(module.outputs[SibylModule::MOD_OUTPUT].getVoltage()==held,"A09 external clock hold freezes curve coordinate");
    }
    {
        auto original=parse(automationCurve(R"([{"beat":0,"value":1}])"));
        SibylModule module;module.acceptComposition(original.composition,ApplyAt::IMMEDIATE,PhasePolicy::RESTART_ALL);tick(module);
        auto pending=upsert(*original.composition,"rise",automationCurve(R"([{"beat":0,"value":8}])"),2);
        module.acceptComposition(pending.composition,ApplyAt::NEXT_SCENE,PhasePolicy::PRESERVE);tick(module);
        check(module.outputs[SibylModule::MOD_OUTPUT].getVoltage()==1.f,"P4 queued curve stays inaudible before boundary");
        auto replacement=upsert(*pending.composition,"rise",automationCurve(R"([{"beat":0,"value":3}])","sceneVisit","replace",4),3);
        module.acceptComposition(replacement.composition,ApplyAt::IMMEDIATE,PhasePolicy::PRESERVE);tick(module);
        check(module.outputs[SibylModule::MOD_OUTPUT].getVoltage()==1.f && module.m_automationLanes[0].blending,
            "P4 pending replacement compares and blends against sounding generation");
        for(int i=0;i<4;++i)tick(module);
        check(module.outputs[SibylModule::MOD_OUTPUT].getVoltage()==3.f,"P4 superseded pending curve never becomes blend source");
        json_t* operations=decode(R"([{"op":"delete_automation","id":"rise"}])");
        auto removed=applyCompositionEdit(*replacement.composition,operations,4);json_decref(operations);
        module.acceptComposition(removed.composition,ApplyAt::IMMEDIATE,PhasePolicy::PRESERVE);tick(module);
        check(module.outputs[SibylModule::MOD_OUTPUT].getVoltage()==3.f,"P4 deleting final curve uses outgoing transition");
        for(int i=0;i<4;++i)tick(module);
        check(module.outputs[SibylModule::MOD_OUTPUT].getVoltage()==1.f && module.outputs[SibylModule::GATE_OUTPUT].getVoltage()==10.f,
            "P4 final curve removal restores legacy latch without closing note");
    }
    {
        auto ramp=parse(automationCurve());
        bool rates=true;
        for(int rate:{44100,48000,96000}) {
            SibylModule module;module.acceptComposition(ramp.composition,ApplyAt::IMMEDIATE,PhasePolicy::RESTART_ALL);
            rack::engine::Module::ProcessArgs args{};args.sampleRate=float(rate);args.sampleTime=1.f/float(rate);
            for(int i=0;i<rate;++i) {args.frame=i;gTrackAllocations=true;module.process(args);gTrackAllocations=false;}
            // The first sample adopts at beat zero; compare against its exact
            // accumulated coordinate, then allow one sample plus float rounding.
            rates &= std::abs(module.outputs[SibylModule::MOD_OUTPUT].getVoltage()-module.m_scenePhase*5./32.)<1e-7;
            rates &= std::abs(module.outputs[SibylModule::MOD_OUTPUT].getVoltage()-5./32.)<=5./(32.*rate)+1e-7;
        }
        check(rates,"P4 one-second ramp agrees within one sample at 44.1/48/96 kHz");
        AutomationCurve moving;moving.transitionMs=4;moving.points={{0.,0.},{8.,8.}};
        moving.segments={{0.,8.,.125,0.,8.,0.,0.}};AutomationLaneState lane;
        bool follows=true;double previous=2.;
        for(int i=0;i<=4;++i) {
            double value=lane.process(&moving,0,1,false,double(i),0.,0.,0.,previous,1000.);
            follows &= std::abs(value-(2.+(double(i)-2.)*double(i)/4.))<1e-12;previous=value;
        }
        check(follows,"A11 handover blends toward moving destination rather than frozen target");
    }
    {
        json_t* root=decode(automationFixture("{}"));json_t* tracks=json_array();json_t* curves=json_object();
        for(int channel=0;channel<16;++channel) {
            std::string id=channel==0?"v":"v"+std::to_string(channel);
            json_array_append_new(tracks,json_pack("{s:s,s:i}","id",id.c_str(),"channel",channel));
            for(int lane=0;lane<3;++lane) {
                json_t* curve=decode(automationCurve(R"([{"beat":0,"value":4}])","arrangement"));
                json_t* target=json_object_get(curve,"target");json_object_set_new(target,"track",json_string(id.c_str()));
                json_object_set_new(target,"lane",json_string(lane==0?"mod":lane==1?"mod2":"mod3"));
                json_object_set_new(curves,(id+"_"+std::to_string(lane)).c_str(),curve);
            }
        }
        json_object_set_new(root,"tracks",tracks);json_object_set_new(root,"automation",curves);
        auto all=compile(root);json_decref(root);check(all.valid,"P4 maximum 48-lane routing compiles");
        if(all.valid) {
            SibylModule module;module.acceptComposition(all.composition,ApplyAt::IMMEDIATE,PhasePolicy::RESTART_ALL);tick(module);
            bool correct=true;
            for(int output:{SibylModule::MOD_OUTPUT,SibylModule::MOD_2_OUTPUT,SibylModule::MOD_3_OUTPUT})
                for(int channel=0;channel<16;++channel) correct &= module.outputs[output].getVoltage(channel)==4.f;
            check(correct,"P4 all 48 MOD outputs render on assigned and patternless tracks");
        }
    }
    check(gAllocationCount==0 && gDeallocationCount==0,"P4 playback, seeks, transitions and adoption allocate/free nothing on audio thread");
}
} // namespace
