// Included by the Rack-linked module harness so allocation instrumentation also
// covers condition evaluation, publication and transport/adoption boundaries.
namespace {
std::string conditionFixture(const std::string& condition, const std::string& extra = "") {
    return R"({"schemaVersion":3,"meta":{"bpm":60,"seed":303},"tracks":[{"id":"v","channel":0}],"patterns":{"p":{"length":4,"resolution":"1/16","steps":[{"step":0,"note":"C3","condition":)"
        + condition + extra + R"(}]}},"arrangement":[{"id":"s","lengthBeats":16,"repeats":4,"tracks":{"v":"p"}}]})";
}
void testConditions() {
    using namespace sibyl;
    const std::string fourth = R"({"all":[{"scope":"patternPass","every":4,"offset":4}]})";
    const std::string first = R"({"all":[{"scope":"patternPass","is":"first"}]})";
    const std::string last = R"({"all":[{"scope":"sceneRepeat","is":"last"}]})";
    for (const char* invalid : {"null", "[]", "{}", R"({"all":{}})", R"({"all":[],"or":[]})",
            R"({"all":[{"scope":"patternPass","is":"last"}]})",
            R"({"all":[{"scope":"arrangementLoop","is":"last"}]})",
            R"({"all":[{"scope":"patternPass","every":4.0}]})",
            R"({"all":[{"scope":"patternPass","every":true}]})",
            R"({"all":[{"scope":"patternPass","every":9223372036854775807}]})",
            R"({"all":[{"scope":"patternPass","every":0}]})",
            R"({"all":[{"scope":"patternPass","every":4,"offset":5}]})",
            R"({"all":[{"scope":"patternPass","every":4,"offset":0}]})",
            R"({"all":[{"scope":"patternPass","is":"first","offset":1}]})",
            R"({"all":[{"scope":"patternPass","is":"first","every":4}]})",
            R"({"all":[{"scope":"bad","every":1}]})",
            R"({"all":[{"scope":"patternPass","every":1,"extra":0}]})"}) {
        const auto r = parseCompositionJson(conditionFixture(invalid), 1);
        check(!r.valid && !r.errors.empty() && r.errors[0].code == "invalid_condition", "P2 strict condition grammar");
    }
    std::string many = "{\"all\":[";
    for (int i=0;i<9;++i) { if(i) many+=","; many+=R"({"scope":"patternPass","every":1})"; }
    many+="]}";
    check(!parseCompositionJson(conditionFixture(many),1).valid, "P2 rejects more than eight tests");
    auto base = parseCompositionJson(conditionFixture(fourth, R"(,"evolve":false,"transposeSemitones":7,"mod":1,"mod2":2,"mod3":3)"),1);
    auto round = parseCompositionJson(serializeFullCompositionJson(*base.composition),2);
    check(round.valid && round.composition->patterns.at("p").steps[0].condition == base.composition->patterns.at("p").steps[0].condition,
        "P2 condition round trip preserves compiled semantics");
    json_error_t error{};
    json_t* ops = json_loads(R"([{"op":"update_notes","pattern_id":"p","selector":{"all":true},"set":{"condition":{"all":[{"scope":"sceneRepeat","is":"last"}]}}}])",0,&error);
    auto edited = applyCompositionEdit(*base.composition,ops,2); json_decref(ops);
    check(edited.valid && edited.composition->patterns.at("p").steps[0].condition.tests[0].kind == ConditionTestKind::LAST
        && changedTrackChannelMask(*base.composition,*edited.composition)==1, "P2 targeted condition update changes dependency mask");
    ops = json_loads(R"([{"op":"update_notes","pattern_id":"p","selector":{"all":true},"unset":["condition"]}])",0,&error);
    auto unset = applyCompositionEdit(*edited.composition,ops,3); json_decref(ops);
    check(unset.valid && !unset.composition->patterns.at("p").steps[0].condition.present, "P2 unset restores unconditional event");
    ops = json_loads(R"([{"op":"insert_notes","pattern_id":"p","notes":[{"step":1,"note":"D3"}]},{"op":"update_notes","pattern_id":"p","selector":{"all":true},"set":{"condition":{"all":[{"scope":"patternPass","is":"last"}]}}}])",0,&error);
    auto failed = applyCompositionEdit(*base.composition,ops,2); json_decref(ops);
    check(!failed.valid && failed.errorCode=="invalid_condition" && base.composition->patterns.at("p").nextNoteId==2,
        "P2 invalid condition rolls back entire edit and allocator");

    ConditionCursor cursor;
    cursor.rebase(0.,1.,1); cursor.advance(3.9,1.);
    check(cursor.pass==4 && cursor.eventPass(16,4)==5, "P2 anticipated event uses nominal next pass without advancing current pass");
    cursor.rebase(.5,2.,cursor.pass); cursor.advance(2.,2.);
    check(cursor.pass==5, "P2 timing rebase preserves pass then advances at next boundary");
    cursor.rebase(0.,1.,kConditionOrdinalMax); cursor.advance(1e300,1.);
    check(cursor.pass==kConditionOrdinalMax, "P2 counters saturate without overflow or missed-cycle loop");
    Condition combined; combined.present=true; combined.count=2;
    combined.tests[0]={ConditionScope::PATTERN_PASS,ConditionTestKind::EVERY,4,4};
    combined.tests[1]={ConditionScope::ARRANGEMENT_LOOP,ConditionTestKind::FIRST,1,1};
    check(conditionEligible(combined,4,1,1,1) && !conditionEligible(combined,4,1,1,2)
        && !conditionEligible(combined,1,1,1,1), "P2 AND scopes and floor-mod eligibility");
    {
        auto empty=parseCompositionJson(conditionFixture(R"({"all":[]})"),1).composition;
        SibylModule module;const auto& pattern=empty->patterns.at("p");
        check(module.eventConditionEligible(*empty,0,pattern,pattern.steps[0],4,1000.),
            "P2 empty AND retains unconditional tie look-ahead semantics");
    }

    int64_t frame=0;
    auto tick = [&](SibylModule& module, float rate=1000.f) {
        Module::ProcessArgs args; args.sampleRate=rate; args.sampleTime=1.f/rate; args.frame=frame++;
        gTrackAllocations=true; module.process(args); gTrackAllocations=false;
    };
    auto advance = [&](SibylModule& module, double phase) {
        int budget=100000;
        while(module.m_trackStates[0].patternPhaseBeats < phase && --budget) tick(module);
        if(!budget) check(false,"P2 fixture reached requested phase within bounded run");
    };
    auto transport = [&](SibylModule& module, const char* request) {
        std::string response, err;
        check(module.handleSibylRequest(SibylControl::Operation::TRANSPORT,request,response,err), "P2 transport accepted");
        tick(module);
    };
    gAllocationCount=0; gDeallocationCount=0;
    // R01 and same-rate boundary checks at production sample rates.
    for(float rate : {44100.f,48000.f,96000.f}) {
        auto parsed=parseCompositionJson(conditionFixture(fourth),1);
        auto comp=std::const_pointer_cast<Composition>(parsed.composition); comp->meta.bpm=600.f;
        SibylModule module; module.acceptComposition(comp,ApplyAt::IMMEDIATE,PhasePolicy::RESTART_ALL);
        int64_t previous=-1; int plays=0; bool accurate=true;
        while(module.m_trackStates[0].patternPhaseBeats<11.1) {
            tick(module,rate);
            const auto& state=module.m_trackStates[0];
            if(state.activeEventStep>=0 && state.activeNominalStep!=previous) {
                previous=state.activeNominalStep;
                int pass=int(previous/4)+1;
                accurate &= state.activeEventPlayed==(pass%4==0);
                accurate &= std::abs(state.patternPhaseBeats-double(pass-1))<=10./rate+1e-6;
                if(state.activeEventPlayed) ++plays;
            }
        }
        check(accurate && plays==3 && module.m_trackStates[0].condition.pass==12, "R01/R04 every fourth pass at production sample rate");
    }
    {
        auto comp=parseCompositionJson(conditionFixture(first),1).composition;
        SibylModule module; module.acceptComposition(comp,ApplyAt::IMMEDIATE,PhasePolicy::RESTART_ALL);
        tick(module); check(module.m_trackStates[0].activeEventPlayed,"R02 first pass plays");
        advance(module,2.1); check(!module.m_trackStates[0].activeEventPlayed,"R02 later passes skipped");
        transport(module,R"({"action":"restart","target":"patterns","apply_at":"immediate"})");
        check(module.m_trackStates[0].condition.pass==1 && module.m_trackStates[0].activeEventPlayed,"R02 explicit pattern restart plays first pass again");
        advance(module,1.2); auto pass=module.m_trackStates[0].condition.pass;
        transport(module,R"({"action":"reseed","apply_at":"immediate"})");
        transport(module,R"({"action":"restart","target":"randomness","apply_at":"immediate"})");
        check(module.m_trackStates[0].condition.pass==pass,"R12 randomness operations preserve condition counter");
        transport(module,R"({"action":"pause","apply_at":"immediate"})");
        const auto phase=module.m_trackStates[0].patternPhaseBeats;
        for(int i=0;i<2000;++i)tick(module);
        check(module.m_trackStates[0].condition.pass==pass && module.m_trackStates[0].patternPhaseBeats==phase,"P2 pause freezes counters");
    }
    {
        auto comp=std::const_pointer_cast<Composition>(parseCompositionJson(conditionFixture(last),1).composition);
        comp->arrangement[0].lengthBeats=1;
        SibylModule module; module.acceptComposition(comp,ApplyAt::IMMEDIATE,PhasePolicy::RESTART_ALL);
        tick(module); check(!module.m_trackStates[0].activeEventPlayed,"R06 first scene repeat silent");
        advance(module,3.01); check(module.m_trackStates[0].activeEventPlayed && module.m_sceneRepeat==3,"R03 final authored repeat eligible");
        for(int i=0;i<1010;++i) tick(module);
        check(module.m_arrangementLoop==2 && module.m_trackStates[0].condition.pass==1,"P2 automatic wrap increments arrangement loop and restarts pattern");
        transport(module,R"({"action":"select_scene","scene_id":"s","apply_at":"immediate"})");
        check(module.m_arrangementLoop==2 && module.m_sceneRepeat==0,"R09 manual scene jump does not increment arrangement loop or invent fill");
        transport(module,R"({"action":"stop","apply_at":"immediate"})");
        check(module.m_arrangementLoop==1 && module.m_trackStates[0].condition.pass==1,"P2 stop resets arrangement and condition ordinals");
    }
    {
        auto comp=std::const_pointer_cast<Composition>(parseCompositionJson(conditionFixture(fourth,R"(,"probability":0)"),1).composition);
        SibylModule module; module.acceptComposition(comp,ApplyAt::IMMEDIATE,PhasePolicy::RESTART_ALL);
        tick(module); advance(module,7.1);
        check(module.m_trackStates[0].condition.pass==8 && module.m_trackStates[0].evolution.pass==7 && module.m_trackStates[0].currentGate==0,
            "R05/R14 silent conditions still advance time and legacy evolution");
        module.publishTelemetry(false,60.);
        auto telemetry=module.readTelemetry();
        std::string response,err; module.handleSibylRequest(SibylControl::Operation::GET_STATUS,"{}",response,err);
        check(telemetry.conditionPass[0]==8 && telemetry.evolutionPass[0]==7 && response.find("conditionPasses")!=std::string::npos,
            "P2 coherent separate one-based condition telemetry");
        auto empty=std::make_shared<Composition>(*comp); empty->revision=2; empty->patterns["p"].steps.clear(); empty->patterns["p"].eventIndexByStep.assign(4,-1);
        module.acceptComposition(empty,ApplyAt::IMMEDIATE,PhasePolicy::PRESERVE);tick(module);advance(module,2.1);
        check(module.m_trackStates[0].condition.pass==10,"P2 completely empty traversals advance without notes");
    }
    {
        auto comp=std::const_pointer_cast<Composition>(parseCompositionJson(conditionFixture(
            R"({"all":[{"scope":"patternPass","every":2,"offset":2},{"scope":"sceneRepeat","is":"first"}]})", R"(,"microshift":-0.2)"),1).composition);
        comp->arrangement[0].lengthBeats=1;
        SibylModule module;module.acceptComposition(comp,ApplyAt::IMMEDIATE,PhasePolicy::RESTART_ALL);
        tick(module);advance(module,.96);
        check(module.m_trackStates[0].activeEventPlayed && module.m_trackStates[0].condition.pass==1 && module.m_sceneRepeat==0,
            "R07 anticipated step zero uses next pattern pass and earlier scene repeat");
        advance(module,1.01);check(module.m_trackStates[0].condition.pass==2,"R07 cursor advances only at nominal boundary");
    }
    {
        auto comp=std::const_pointer_cast<Composition>(makeTiedSwingComposition());
        auto& tie=comp->patterns["first"].steps[1];
        tie.condition.present=true;tie.condition.count=1;tie.condition.tests[0]={ConditionScope::PATTERN_PASS,ConditionTestKind::EVERY,4,4};
        tie.hasObservation=true;tie.observation.octaviaModuleId=2468;tie.observation.monitorMask=4;
        uint64_t sequence=octavia::observationBus().latestSequence();
        SibylModule module;module.acceptComposition(comp,ApplyAt::IMMEDIATE,PhasePolicy::RESTART_ALL);
        tick(module);advance(module,.1);check(module.m_trackStates[0].currentGate==0,"R11 false tie does not extend preceding short gate");
        advance(module,.4);octavia::ObservationTrigger trigger;uint64_t dropped=0;
        check(!module.m_trackStates[0].activeEventPlayed && module.m_trackStates[0].currentGate==0
            && !octavia::observationBus().poll(&sequence,&trigger,&dropped),"R11 skipped tie neither resurrects gate nor publishes observation");
    }
    {
        auto comp=std::const_pointer_cast<Composition>(parseCompositionJson(conditionFixture(fourth,R"(,"ratchets":4,"gate":0.5,"probability":0)"),1).composition);
        Macro macro;macro.id="1";macro.target="global.probability";macro.amount=1.f;comp->macros["1"]=macro;
        SibylModule module;module.inputs[SibylModule::MACRO_1_INPUT].channels=1;module.inputs[SibylModule::MACRO_1_INPUT].setVoltage(10.f);
        module.acceptComposition(comp,ApplyAt::IMMEDIATE,PhasePolicy::RESTART_ALL);
        bool silent=true;for(int i=0;i<900;++i){tick(module);silent &= module.m_trackStates[0].currentGate==0;}
        check(silent,"R10 probability macro and ratchets cannot bypass false condition");
        advance(module,3.01);check(module.m_trackStates[0].activeEventPlayed,"R10 positive macro does enable the eligible traversal");
    }
    {
        auto comp=std::const_pointer_cast<Composition>(parseCompositionJson(conditionFixture(first),1).composition);
        comp->patterns["q"]=comp->patterns["p"];comp->patterns["q"].id="q";
        auto second=comp->arrangement[0];second.id="second";second.phaseMode=PhaseMode::CONTINUE;
        comp->arrangement.push_back(second);
        auto third=second;third.id="third";third.tracks["v"].patternId="q";comp->arrangement.push_back(third);
        SibylModule module;module.acceptComposition(comp,ApplyAt::IMMEDIATE,PhasePolicy::RESTART_ALL);
        tick(module);advance(module,2.2);
        transport(module,R"({"action":"select_scene","scene_id":"second","apply_at":"immediate"})");
        check(module.m_trackStates[0].condition.pass==3 && module.m_trackStates[0].patternPhaseBeats>2.2,
            "R08 continue with same pattern preserves phase and pass");
        transport(module,R"({"action":"select_scene","scene_id":"third","apply_at":"immediate"})");
        check(module.m_trackStates[0].condition.pass==1 && module.m_trackStates[0].patternPhaseBeats>2.2,
            "R08 continue with different pattern rebases current partial traversal to one");
        advance(module,3.01);check(module.m_trackStates[0].condition.pass==2,"R08 replacement next wrap advances from rebased one");
        transport(module,R"({"action":"select_scene","scene_id":"second","phase_mode":"alignGlobal","apply_at":"immediate"})");
        check(module.m_trackStates[0].condition.pass==conditionCycle(module.m_globalPhaseBeats,1.)+1,
            "R08 alignGlobal derives nominal global ordinal");
        transport(module,R"({"action":"restart","target":"scene","apply_at":"immediate"})");
        check(module.m_trackStates[0].condition.pass==1,"R08 explicit scene restart resets conditions even with continue phase");
        auto timing=std::make_shared<Composition>(*comp);timing->revision=3;timing->patterns["p"].length=8;
        auto before=module.m_trackStates[0].condition.pass;
        module.acceptComposition(timing,ApplyAt::IMMEDIATE,PhasePolicy::PRESERVE);tick(module);
        check(module.m_trackStates[0].condition.pass==before,"R08 preserve adoption rebases timing without inventing a pass");
        timing=std::make_shared<Composition>(*timing);timing->revision=4;timing->patterns["p"].steps[0].compiledPitchV=2.f;
        module.acceptComposition(timing,ApplyAt::IMMEDIATE,PhasePolicy::RESTART_CHANGED);tick(module);
        check(module.m_trackStates[0].condition.pass==1 && module.m_trackStates[0].patternPhaseBeats<.01,
            "R08 restartChanged resets affected condition phase");
    }
    {
        auto comp=std::const_pointer_cast<Composition>(parseCompositionJson(conditionFixture(first),1).composition);
        comp->arrangement[0].lengthBeats=1;comp->arrangement[0].repeats=1;comp->transport.loop=false;
        auto& condition=comp->patterns["p"].steps[0].condition;
        condition.count=2;condition.tests[0]={ConditionScope::SCENE_REPEAT,ConditionTestKind::FIRST,1,1};
        condition.tests[1]={ConditionScope::SCENE_REPEAT,ConditionTestKind::LAST,1,1};
        SibylModule module;module.acceptComposition(comp,ApplyAt::IMMEDIATE,PhasePolicy::RESTART_ALL);tick(module);
        check(module.m_trackStates[0].activeEventPlayed,"R03 single repeat is both first and last");
        for(int i=0;i<1100;++i)tick(module);
        check(module.m_arrangementLoop==1 && !module.m_runtimeRunning.load(),"P2 non-loop ending does not increment arrangement ordinal");
    }
    {
        auto comp=std::const_pointer_cast<Composition>(parseCompositionJson(conditionFixture(fourth),1).composition);
        comp->patterns["p"].evolution.probability=.3f;comp->patterns["p"].evolution.velocity=.4f;
        comp->patterns["p"].steps[0].hasProbability=true;comp->patterns["p"].steps[0].probability=.7f;
        auto unconditional=std::make_shared<Composition>(*comp);unconditional->patterns["p"].steps[0].condition={};
        SibylModule module,baseline;
        module.acceptComposition(comp,ApplyAt::IMMEDIATE,PhasePolicy::RESTART_ALL);
        baseline.acceptComposition(unconditional,ApplyAt::IMMEDIATE,PhasePolicy::RESTART_ALL);
        bool identical=true;
        for(int i=0;i<11200;++i) {
            tick(module);tick(baseline);
            const auto& a=module.m_trackStates[0];const auto& b=baseline.m_trackStates[0];
            identical &= a.evolution.pass==b.evolution.pass;
            if ((a.activeNominalStep/4+1)%4==0) {
                identical &= a.activeEventPlayed==b.activeEventPlayed;
                if(a.activeEventPlayed) identical &= a.currentVel==b.currentVel;
            }
        }
        check(identical,"R17 rejected events preserve opted-in probability/evolution sequence on later eligible passes");
        module.inputs[SibylModule::CLOCK_INPUT].channels=1;
        module.inputs[SibylModule::CLOCK_INPUT].setVoltage(0.f);
        auto phase=module.m_trackStates[0].patternPhaseBeats;auto pass=module.m_trackStates[0].condition.pass;
        for(int i=0;i<3000;++i)tick(module);
        check(module.m_trackStates[0].patternPhaseBeats==phase && module.m_trackStates[0].condition.pass==pass,
            "P2 external clock without ticks holds condition time");
    }
    {
        auto comp=std::const_pointer_cast<Composition>(parseCompositionJson(conditionFixture(
            R"({"all":[{"scope":"sceneRepeat","is":"first"}]})"),1).composition);
        comp->arrangement[0].lengthBeats=.8509f;
        auto& pattern=comp->patterns["p"];pattern.steps[0].step=3;pattern.steps[0].microshift=.4032f;
        pattern.eventIndexByStep={-1,-1,-1,0};
        SibylModule module;module.acceptComposition(comp,ApplyAt::IMMEDIATE,PhasePolicy::RESTART_ALL);
        tick(module);advance(module,.851);
        check(module.m_sceneRepeat==1 && module.m_trackStates[0].activeEventPlayed,
            "P2 sub-sample onset before repeat boundary uses scheduled earlier repeat");
    }
    check(gAllocationCount==0 && gDeallocationCount==0,"P2 processing/adoption/skips/transport allocate and free nothing on audio thread");
}
} // namespace
