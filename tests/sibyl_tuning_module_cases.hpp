namespace {
void testTunedPlayback() {
    using namespace sibyl;
    for (int n : {38,53}) {
        const std::string fixture=std::string(R"({"schemaVersion":4,"meta":{"bpm":60},"pitchSystems":{"tunings":{"t":{"kind":"equal","divisions":)")+std::to_string(n)+R"(,"period":{"ratio":"2/1"}}},"contexts":{"c":{"tuning":"t","anchor":{"note":"C4"}}},"defaultContext":"c"},"tracks":[{"id":"v","channel":0}],"patterns":{"p":{"length":4,"resolution":"1/4","steps":[{"step":0,"tuned":{"step":1},"gate":3}]},"q":{"length":4,"resolution":"1/4","steps":[]}},"arrangement":[{"id":"s","lengthBeats":4,"tracks":{"v":"p"}}]})";
        auto parsed=parseCompositionJson(fixture,1);
        check(parsed.valid,"P7A native playback fixture compiles");
        if(!parsed.valid) continue;
        SibylModule module;
        module.outputs[SibylModule::V_OCT_OUTPUT].channels=1;
        module.outputs[SibylModule::GATE_OUTPUT].channels=1;
        module.acceptComposition(parsed.composition,ApplyAt::IMMEDIATE,PhasePolicy::RESTART_ALL);
        gAllocationCount=gDeallocationCount=0;
        gTrackAllocations=true;
        for(int i=0;i<100;++i) processOneSample(module);
        gTrackAllocations=false;
        check(gAllocationCount==0 && gDeallocationCount==0,"P7A playback has no allocations or frees");
        check(std::abs(module.outputs[SibylModule::V_OCT_OUTPUT].getVoltage()-1.f/n)<1e-6f && module.outputs[SibylModule::GATE_OUTPUT].getVoltage()>0.f,
              "P7A 38/53-EDO produces actual pitch/gate output");
        json_t* saved=module.dataToJson();
        SibylModule loaded; loaded.dataFromJson(saved); json_decref(saved);
        check(loaded.m_acceptedCompositionPtr && loaded.m_acceptedCompositionPtr->patterns.at("p").steps[0].tuned==parsed.composition->patterns.at("p").steps[0].tuned,
              "P7A patch persistence preserves native representation and definitions");
        json_t* operations=json_loads(R"([{"op":"update_notes","pattern_id":"p","selector":{"all":true},"set":{"velocity":0.3}},{"op":"duplicate_notes","pattern_id":"p","selector":{"all":true},"destination_pattern_id":"q","offset_steps":0}])",0,nullptr);
        auto edited=applyCompositionEdit(*parsed.composition,operations,2); json_decref(operations);
        check(edited.valid && edited.composition->patterns.at("q").steps[0].tuned.find("context")!=std::string::npos,
              "P7A edits preserve tuning and cross-pattern copies pin source context");
        check(edited.valid && edited.composition->patterns.at("p").steps[0].compiledPitchV==parsed.composition->patterns.at("p").steps[0].compiledPitchV,
              "P7A expression editing does not change native pitch");
        operations=json_loads(R"([{"op":"update_scene_assignment","scene_id":"s","track_id":"v","set":{"overrides":{"transposeSteps":1,"transposePeriods":1,"transposeCents":-1.5}}}])",0,nullptr);
        auto shifted=applyCompositionEdit(*parsed.composition,operations,2);json_decref(operations);
        check(shifted.valid,"P7B typed scene offsets compile");
        if(shifted.valid) {
            module.acceptComposition(shifted.composition,ApplyAt::NEXT_BEAT,PhasePolicy::RESTART_ALL);
            check(module.m_activeRevision.load()==1,"P7B quantized pitch change waits for adoption boundary");
            gAllocationCount=gDeallocationCount=0;gTrackAllocations=true;
            for(int i=0;i<50000;++i) processOneSample(module);
            gTrackAllocations=false;
            check(gAllocationCount==0 && gDeallocationCount==0,"P7B scene pitch lookup/adoption remains allocation-free");
            check(module.m_activeRevision.load()==2 && std::abs(module.outputs[SibylModule::V_OCT_OUTPUT].getVoltage()-(1.+2./n-1.5/1200))<1e-6,
                  "P7B native scene transforms reach actual output exactly once");
            json_t* request=json_loads(R"({"view":"effective_context","scene_id":"s","scene_repeat":0,"beat":0})",0,nullptr);
            auto view=serializeHarmonyView(*shifted.composition,request);json_decref(request);
            json_t* response=json_loads(view.c_str(),0,nullptr);
            json_t* note=json_array_get(json_object_get(json_object_get(response,"derived"),"notes"),0);
            check(std::abs(json_number_value(json_object_get(note,"effectivePitchV"))-module.outputs[SibylModule::V_OCT_OUTPUT].getVoltage())<1e-6,
                  "P7B effective context agrees with scene output");
            json_decref(response);
        }
    }
}
}
