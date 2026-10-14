namespace {
std::string overrideFixture(const std::string& overrides) {
    return R"({"schemaVersion":3,"meta":{"bpm":60,"seed":5900},"tracks":[{"id":"v","channel":0}],"patterns":{"p":{"length":4,"steps":[{"step":0,"note":"C3","transposeSemitones":12,"velocity":0.6,"probability":1,"gate":1,"mod":2,"mod2":-2,"mod3":9}]}},"arrangement":[{"id":"verse","lengthBeats":16,"tracks":{"v":"p"}},{"id":"chorus","lengthBeats":16,"tracks":{"v":{"pattern":"p","phaseMode":"restart","overrides":)"
        + overrides + R"(}}}]})";
}
void testOverrides() {
    using namespace sibyl;
    auto decode=[](const std::string& text) {json_error_t err{};return json_loads(text.c_str(),0,&err);};
    auto edit=[&](const Composition& base,const char* text,int revision=2) {
        json_t* ops=decode(text);auto result=applyCompositionEdit(base,ops,revision);json_decref(ops);return result;
    };
    auto base=parseCompositionJson(overrideFixture(R"({"transposeSemitones":12,"velocityScale":1.2,"velocityOffset":0.1,"probabilityScale":0.8,"probabilityOffset":0.1,"gateScale":0.9,"gateOffset":0.2,"modOffset":1,"mod2Offset":-2,"mod3Offset":2})"),1);
    check(base.valid,"P3 complete assignment overrides compile");
    if(!base.valid)return;
    auto round=parseCompositionJson(serializeFullCompositionJson(*base.composition),2);
    check(round.valid && round.composition->arrangement[1].tracks.at("v")==base.composition->arrangement[1].tracks.at("v"),
        "P3 override values and authored presence round-trip");
    for(const char* invalid : {"null","[]",R"({"transposeSemitones":1.0})",R"({"velocityScale":true})",
            R"({"velocityScale":4.01})",R"({"probabilityOffset":-1.01})",R"({"gateOffset":1025})",
            R"({"mod3Offset":20.01})",R"({"velocity_scale":1})",R"({"gateScale":-0.1})"})
        check(!parseCompositionJson(overrideFixture(invalid),1).valid,"P3 strict override types, fields and bounds");
    json_t* legacy=decode(overrideFixture("{}"));json_object_del(legacy,"schemaVersion");
    char* text=json_dumps(legacy,JSON_COMPACT);auto invalidVersion=parseCompositionJson(text,1);free(text);json_decref(legacy);
    check(!invalidVersion.valid && invalidVersion.errors[0].code=="schema_version_required","P3 legacy inputs cannot silently discard overrides");
    auto outOfPitch=parseCompositionJson(overrideFixture(R"({"transposeSemitones":120})"),1);
    check(outOfPitch.valid,"P3 scene and event transpose compose within output range");
    auto tooHigh=edit(*outOfPitch.composition,R"([{"op":"transpose_notes","pattern_id":"p","selector":{"all":true},"semitones":1}])");
    check(!tooHigh.valid && tooHigh.errorCode=="pitch_out_of_range","P3 validates effective pitch across all assigned scenes");
    const auto authored=serializePatternViewJson(*base.composition,"p");
    auto partial=edit(*base.composition,R"([{"op":"update_scene_assignment","scene_id":"chorus","track_id":"v","set":{"overrides":{"transposeSemitones":7}}}])");
    const auto& assignment=partial.composition->arrangement[1].tracks.at("v");
    check(partial.valid && assignment.hasPhaseModeOverride && assignment.overrides.values[VELOCITY_SCALE]==1.2f
        && assignment.overrides.values[TRANSPOSE]==7,"V04 partial assignment edit preserves phase and unspecified overrides");
    auto stringMerge=edit(*base.composition,R"([{"op":"update_scene_assignment","scene_id":"verse","track_id":"v","set":{"overrides":{"velocityOffset":0.2}}}])");
    check(stringMerge.valid && stringMerge.composition->arrangement[0].tracks.at("v").patternId=="p","P3 partial edit promotes legacy string assignment");
    auto removed=edit(*partial.composition,R"([{"op":"update_scene_assignment","scene_id":"chorus","track_id":"v","unset":["phaseMode","overrides.velocityScale"]}])");
    check(removed.valid && !removed.composition->arrangement[1].tracks.at("v").hasPhaseModeOverride
        && removed.composition->arrangement[1].tracks.at("v").overrides.values[VELOCITY_SCALE]==1.f
        && removed.composition->arrangement[1].tracks.at("v").overrides.values[VELOCITY_OFFSET]==.1f,
        "P3 leaf removal restores identity without clearing other attributes");
    auto replaced=edit(*base.composition,R"([{"op":"set_scene_assignment","scene_id":"chorus","track_id":"v","assignment":"p"}])");
    check(replaced.valid && !replaced.composition->arrangement[1].tracks.at("v").overrides.present,"P3 full assignment replacement is explicit");
    auto warning=edit(*base.composition,R"([{"op":"set_scene_track","scene_id":"chorus","track_id":"v","pattern_id":"p"}])");
    check(warning.valid && !warning.warnings.empty() && warning.warnings[0].code=="assignment_attributes_cleared",
        "P3 legacy assignment replacement warns when attributes are discarded");
    auto gone=edit(*base.composition,R"([{"op":"set_scene_assignment","scene_id":"chorus","track_id":"v","assignment":null}])");
    check(gone.valid && !gone.composition->arrangement[1].tracks.count("v"),"P3 null assignment removes route");
    check(!edit(*gone.composition,R"([{"op":"update_scene_assignment","scene_id":"chorus","track_id":"v","set":{"pattern":"p"}}])").valid,
        "P3 partial edit cannot create missing assignment");
    for(const char* op : {
            R"([{"op":"update_scene_assignment","scene_id":"chorus","track_id":"v","set":{"bad":0}}])",
            R"([{"op":"update_scene_assignment","scene_id":"chorus","track_id":"v","unset":["pattern"]}])",
            R"([{"op":"update_scene_assignment","scene_id":"chorus","track_id":"v","set":{"overrides":{"gateScale":2}},"unset":["overrides"]}])",
            R"([{"op":"update_scene_assignment","scene_id":"chorus","track_id":"v","set":{"overrides":{"gateScale":2}},"unset":["overrides.gateScale"]}])",
            R"([{"op":"set_scene_assignment","scene_id":"chorus","track_id":"v","assignment":{"pattern":"p","bad":0}}])",
            R"([{"op":"set_scene_assignment","scene_id":"chorus","track_id":"v","assignment":""}])",
            R"([{"op":"set_scene_assignment","scene_id":"chorus","track_id":"v","assignment":"missing"}])"})
        check(!edit(*base.composition,op).valid,"P3 assignment edits reject malformed or conflicting requests atomically");
    check(serializePatternViewJson(*base.composition,"p")==authored && base.composition->patterns.at("p").nextNoteId==2,
        "P3 scene variations leave shared authored patterns and IDs unchanged");
    json_t* beforePattern=decode(authored);json_t* afterPattern=decode(serializePatternViewJson(*partial.composition,"p"));
    check(json_equal(json_object_get(beforePattern,"pattern"),json_object_get(afterPattern,"pattern")),
        "P3 candidate pattern contents remain byte-equivalent as JSON values");json_decref(beforePattern);json_decref(afterPattern);
    auto rollback=edit(*base.composition,R"([{"op":"update_scene_assignment","scene_id":"chorus","track_id":"v","set":{"overrides":{"velocityScale":2}}},{"op":"update_scene_assignment","scene_id":"verse","track_id":"v","set":{"overrides":{"gateScale":-1}}}])");
    check(!rollback.valid && base.composition->arrangement[1].tracks.at("v").overrides.values[VELOCITY_SCALE]==1.2f,
        "P3 invalid later override rejects the complete transaction");
    for(int field=0;field<OVERRIDE_COUNT;++field) {
        auto changed=std::make_shared<Composition>(*base.composition);
        changed->arrangement[1].tracks["v"].overrides.values[field]+=.01f;
        check(changedTrackChannelMask(*base.composition,*changed)==1,"P3 every override participates in adoption comparison");
    }
    json_t* scene=decode(serializeSceneViewJson(*base.composition,"chorus",true));
    auto note=json_array_get(json_object_get(json_object_get(json_object_get(scene,"derived"),"assignmentNotes"),"v"),0);
    check(note && std::abs(json_number_value(json_object_get(note,"pitchV"))-1.)<1e-6
        && std::abs(json_number_value(json_object_get(note,"velocity"))-.82)<1e-6,
        "P3 scene preview separates static effective expressions from authored pattern data");json_decref(scene);
    AssignmentOverrides probability;probability.values[PROBABILITY_SCALE]=.8f;probability.values[PROBABILITY_OFFSET]=.1f;
    check(std::abs(probability.scaled(.5f,PROBABILITY_SCALE,PROBABILITY_OFFSET)+.1f-.6f)<1e-6,"V03 probability scale/offset then macro");
    check(std::abs(probability.scaled(.7f,PROBABILITY_SCALE,PROBABILITY_OFFSET)+.1f-.76f)<1e-6,"V05 probability evolution precedes scene overrides and macro");

    int64_t frame=0;gAllocationCount=0;gDeallocationCount=0;
    auto tick=[&](SibylModule& module) {Module::ProcessArgs args;args.sampleRate=48000.f;args.sampleTime=1.f/48000.f;args.frame=frame++;
        gTrackAllocations=true;module.process(args);gTrackAllocations=false;};
    auto chorus=[&](SibylModule& module) {std::string response,error;
        check(module.handleSibylRequest(SibylControl::Operation::TRANSPORT,R"({"action":"select_scene","scene_id":"chorus","apply_at":"immediate"})",response,error),"P3 chorus selection accepted");tick(module);};
    {
        SibylModule module;module.acceptComposition(base.composition,ApplyAt::IMMEDIATE,PhasePolicy::RESTART_ALL);tick(module);
        const auto* accepted=module.m_acceptedCompositionPtr;
        std::string response,error;
        const std::string operations=R"([{"op":"update_scene_assignment","scene_id":"chorus","track_id":"v","set":{"overrides":{"velocityScale":1.5}}}])";
        check(module.handleSibylRequest(SibylControl::Operation::VALIDATE,"{\"expected_revision\":1,\"operations\":"+operations+"}",response,error)
            && response.find("\"valid\":true")!=std::string::npos && module.m_acceptedCompositionPtr==accepted,
            "P3 assignment preview validates without publication");
        check(module.handleSibylRequest(SibylControl::Operation::EDIT,"{\"expected_revision\":1,\"apply_at\":\"nextBeat\",\"operations\":"+operations+"}",response,error)
            && module.m_acceptedRevision==2 && module.m_activeRevision.load()==1,"P3 assignment edit respects accepted/pending revisions");
        module.handleSibylRequest(SibylControl::Operation::GET_COMPOSITION,R"({"view":"scene","id":"chorus","fields":["effectiveExpressions"]})",response,error);
        check(response.find("assignmentNotes")!=std::string::npos,"P3 semantic scene projection carries static effective values");
        module.handleSibylRequest(SibylControl::Operation::GET_COMPOSITION,R"({"view":"scene","id":"chorus"})",response,error);
        check(response.find("assignmentNotes")==std::string::npos,"P3 ordinary scene reads stay compact");
    }
    {
        auto comp=std::const_pointer_cast<Composition>(parseCompositionJson(overrideFixture(R"({"transposeSemitones":12,"velocityScale":1.2,"velocityOffset":0.1,"modOffset":1,"mod2Offset":-2,"mod3Offset":2})"),1).composition);
        Macro macro;macro.id="1";macro.target="global.velocity";macro.amount=.1f;comp->macros["1"]=macro;
        SibylModule module;module.inputs[SibylModule::MACRO_1_INPUT].channels=1;module.inputs[SibylModule::MACRO_1_INPUT].setVoltage(10.f);
        module.acceptComposition(comp,ApplyAt::IMMEDIATE,PhasePolicy::RESTART_ALL);tick(module);
        float verse=module.outputs[SibylModule::V_OCT_OUTPUT].getVoltage();chorus(module);
        check(module.outputs[SibylModule::V_OCT_OUTPUT].getVoltage()==verse+1.f,"V01 shared pattern plays octave higher in chorus with additive event transpose");
        check(std::abs(module.outputs[SibylModule::VELOCITY_OUTPUT].getVoltage()-9.2f)<1e-5,"V02 velocity scale/offset precedes macro: 9.2 V");
        check(module.outputs[SibylModule::MOD_OUTPUT].getVoltage()==3.f && module.outputs[SibylModule::MOD_2_OUTPUT].getVoltage()==-4.f
            && module.outputs[SibylModule::MOD_3_OUTPUT].getVoltage()==10.f,"P3 all MOD offsets apply and clamp independently");
        json_t* saved=module.dataToJson();SibylModule restored;restored.dataFromJson(saved);json_decref(saved);tick(restored);chorus(restored);
        check(restored.outputs[SibylModule::V_OCT_OUTPUT].getVoltage()==1.f,"P3 patch-state restore preserves scene transpose");
    }
    {
        auto absent=std::const_pointer_cast<Composition>(parseCompositionJson(overrideFixture("{}"),1).composition);
        absent->patterns["p"].evolution.probability=.3f;absent->patterns["p"].evolution.velocity=.2f;
        absent->patterns["p"].steps[0].probability=.5f;
        auto identity=std::make_shared<Composition>(*absent);auto& ov=identity->arrangement[0].tracks["v"].overrides;
        ov.present=true;ov.fields=(1u<<OVERRIDE_COUNT)-1;
        SibylModule a,b;a.acceptComposition(absent,ApplyAt::IMMEDIATE,PhasePolicy::RESTART_ALL);b.acceptComposition(identity,ApplyAt::IMMEDIATE,PhasePolicy::RESTART_ALL);
        bool same=true;for(int sample=0;sample<110000;++sample){tick(a);tick(b);for(int output=0;output<SibylModule::NUM_OUTPUTS;++output)
            same &= a.outputs[output].getVoltage()==b.outputs[output].getVoltage();}
        check(same,"V05 identity overrides retain evolution and all output voltages sample-for-sample");
    }
    {
        auto normal=std::const_pointer_cast<Composition>(parseCompositionJson(overrideFixture("{}"),1).composition);
        normal->patterns["p"].steps[0].gate=0.f;
        auto identity=std::make_shared<Composition>(*normal);identity->arrangement[0].tracks["v"].overrides.present=true;
        identity->arrangement[0].tracks["v"].overrides.fields=1u<<GATE_SCALE;
        auto silent=std::make_shared<Composition>(*normal);auto& ov=silent->arrangement[0].tracks["v"].overrides;
        ov.present=true;ov.fields=1u<<GATE_SCALE;ov.values[GATE_SCALE]=0.f;
        SibylModule a,b,c;a.acceptComposition(normal,ApplyAt::IMMEDIATE,PhasePolicy::RESTART_ALL);
        b.acceptComposition(identity,ApplyAt::IMMEDIATE,PhasePolicy::RESTART_ALL);c.acceptComposition(silent,ApplyAt::IMMEDIATE,PhasePolicy::RESTART_ALL);
        tick(a);tick(b);tick(c);
        check(a.outputs[SibylModule::GATE_OUTPUT].getVoltage()==10.f && b.outputs[SibylModule::GATE_OUTPUT].getVoltage()==10.f,
            "P3 characterizes and preserves legacy minimum pulse for authored gate zero and identity override");
        bool zero=c.outputs[SibylModule::GATE_OUTPUT].getVoltage()==0.f;for(int i=0;i<1000;++i){tick(c);zero &= c.outputs[SibylModule::GATE_OUTPUT].getVoltage()==0.f;}
        check(zero,"P3 nonidentity zero gate override produces no minimum pulse");
        Macro macro;macro.id="1";macro.target="track.v.gate";macro.amount=.5f;silent=std::make_shared<Composition>(*silent);silent->revision=2;silent->macros["1"]=macro;
        c.inputs[SibylModule::MACRO_1_INPUT].channels=1;c.inputs[SibylModule::MACRO_1_INPUT].setVoltage(10.f);
        c.acceptComposition(silent,ApplyAt::IMMEDIATE,PhasePolicy::RESTART_ALL);tick(c);
        check(c.outputs[SibylModule::GATE_OUTPUT].getVoltage()==10.f,"P3 gate override includes live gate macro contribution");
        c.inputs[SibylModule::MACRO_1_INPUT].setVoltage(0.f);tick(c);
        check(c.outputs[SibylModule::GATE_OUTPUT].getVoltage()==0.f,"P3 live macro returning to nonpositive gate remains silent");
    }
    {
        auto comp=std::const_pointer_cast<Composition>(parseCompositionJson(overrideFixture("{}"),1).composition);
        TrackDef other=comp->tracks[0];other.id="other";other.channel=1;comp->tracks.push_back(other);
        comp->arrangement[0].tracks["other"]=comp->arrangement[0].tracks["v"];
        auto& ov=comp->arrangement[0].tracks["other"].overrides;ov.present=true;ov.fields=1u<<TRANSPOSE;ov.values[TRANSPOSE]=12;
        SibylModule module;module.acceptComposition(comp,ApplyAt::IMMEDIATE,PhasePolicy::RESTART_ALL);tick(module);
        check(module.outputs[SibylModule::V_OCT_OUTPUT].getVoltage(1)==module.outputs[SibylModule::V_OCT_OUTPUT].getVoltage(0)+1.f,
            "P3 simultaneous tracks share a pattern with independent assignment transpose");
    }
    {
        auto comp=std::const_pointer_cast<Composition>(parseCompositionJson(overrideFixture("{}"),1).composition);
        auto& ov=comp->arrangement[0].tracks["v"].overrides;ov.present=true;
        ov.fields=(1u<<PROBABILITY_SCALE)|(1u<<PROBABILITY_OFFSET);
        ov.values[PROBABILITY_SCALE]=.8f;ov.values[PROBABILITY_OFFSET]=.1f;
        auto& pattern=comp->patterns["p"];pattern.evolution.probability=.3f;pattern.steps[0].probability=.5f;
        Macro macro;macro.id="1";macro.target="global.probability";macro.amount=.1f;comp->macros["1"]=macro;
        SibylModule module;module.inputs[SibylModule::MACRO_1_INPUT].channels=1;module.inputs[SibylModule::MACRO_1_INPUT].setVoltage(10.f);
        module.acceptComposition(comp,ApplyAt::IMMEDIATE,PhasePolicy::RESTART_ALL);
        int64_t previous=-1;bool correct=true;int onsets=0;
        for(int i=0;i<150000;++i){tick(module);const auto& state=module.m_trackStates[0];
            if(state.activeEventStep>=0 && state.activeNominalStep!=previous){previous=state.activeNominalStep;++onsets;
                auto expression=evolveExpression(pattern,pattern.steps[0],comp->tracks[0],comp->meta.seed,state.evolution.pass,0);
                float threshold=std::max(0.f,std::min(1.f,expression.probability*.8f+.1f+.1f));
                uint64_t hash=comp->meta.seed;
                if(state.evolution.pass>0)hash=evolutionHash(hash^evolutionHash(state.evolution.pass)^0x70726f62ULL);
                correct &= state.activeEventPlayed == (float((hash%10000ULL)/10000.)<threshold);
            }
        }
        check(correct && onsets==4,"V03/V05 runtime probability applies scene transforms to evolved threshold before macro");
        auto rejected=std::make_shared<Composition>(*comp);rejected->revision=2;
        auto& condition=rejected->patterns["p"].steps[0].condition;condition.present=true;condition.count=1;
        condition.tests[0]={ConditionScope::PATTERN_PASS,ConditionTestKind::EVERY,4,4};
        rejected->arrangement[0].tracks["v"].overrides.values[PROBABILITY_OFFSET]=1.f;
        module.acceptComposition(rejected,ApplyAt::IMMEDIATE,PhasePolicy::RESTART_ALL);tick(module);
        check(!module.m_trackStates[0].activeEventPlayed && module.outputs[SibylModule::GATE_OUTPUT].getVoltage()==0.f,
            "V03 scene probability and macros cannot bypass condition rejection");
    }
    {
        for(bool tied : {false,true}) {
            auto comp=std::const_pointer_cast<Composition>(parseCompositionJson(overrideFixture("{}"),1).composition);
            auto& event=comp->patterns["p"].steps[0];event.ratchets=tied?1:4;event.tie=tied;
            auto& ov=comp->arrangement[0].tracks["v"].overrides;ov.present=true;ov.values[GATE_OFFSET]=-2.f;ov.fields=1u<<GATE_OFFSET;
            SibylModule module;module.acceptComposition(comp,ApplyAt::IMMEDIATE,PhasePolicy::RESTART_ALL);
            bool silent=true;for(int i=0;i<15000;++i){tick(module);silent &= module.outputs[SibylModule::GATE_OUTPUT].getVoltage()==0.f;}
            check(silent,"P3 nonpositive gate overrides remain silent for ties and ratchets");
        }
    }
    check(gAllocationCount==0 && gDeallocationCount==0,"P3 playback/adoption/scene variation adds no audio-thread allocation or free");
}
} // namespace
