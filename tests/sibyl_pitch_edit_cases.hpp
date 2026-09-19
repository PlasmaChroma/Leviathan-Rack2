#pragma once
#include "../src/SibylPitchView.hpp"
inline const char* pitchEditFixture() {
    return R"({"schemaVersion":4,"pitchSystems":{"tunings":{"t":{"kind":"equal","divisions":53,"period":{"ratio":"2/1"}},"u":{"kind":"table","period":{"ratio":"2/1"},"positions":[{"cents":0},{"cents":200},{"cents":700}]}},"scales":{"s":{"tuning":"t","steps":[0,9,17,22,31,39,48]}},"contexts":{"c":{"tuning":"t","scale":"s","anchor":{"pitchV":0}},"u":{"tuning":"u","anchor":{"pitchV":0}}},"defaultContext":"c"},"tracks":[{"id":"v","channel":0}],"patterns":{"p":{"length":4,"resolution":"1/4","steps":[{"step":0,"tuned":{"step":0}},{"step":1,"tuned":{"step":17}},{"step":2,"tuned":{"step":31}}]},"dest":{"length":4,"resolution":"1/4","pitchContext":"u","steps":[]},"l":{"length":4,"resolution":"1/4","steps":[{"step":0,"pitchV":0.58}]}},"arrangement":[{"id":"s","lengthBeats":4,"tracks":{"v":"p"}}]})";
}
inline void pitchEditCases() {
    using namespace sibyl;
    auto initial=parseCompositionJson(pitchEditFixture(),1);assert(initial.valid);
    auto b=initial.composition;const auto original=serializeFullCompositionJson(*b);
    auto shift=edit(*b,R"([{"op":"transpose_notes","pattern_id":"p","selector":{"all":true},"interval":{"steps":1}}])");
    assert(shift.valid && std::abs(shift.composition->patterns.at("p").steps[1].compiledPitchV-18./53)<1e-6);
    assert(shift.composition->patterns.at("p").steps[0].pitchOffsets.fields==1);
    auto semitone=edit(*b,R"([{"op":"transpose_notes","pattern_id":"p","selector":{"all":true},"semitones":1}])");
    assert(semitone.valid && std::abs(semitone.composition->patterns.at("p").steps[0].compiledPitchV-1./12)<1e-6);
    auto ratio=edit(*b,R"([{"op":"transpose_notes","pattern_id":"p","selector":{"all":true},"interval":{"ratio":"3/2"}}])");
    assert(ratio.valid && ratio.pitchChanges.size()==1 && std::abs(ratio.composition->patterns.at("p").steps[0].compiledPitchV-std::log2(1.5))<1e-6);
    auto unsupported=edit(*b,R"([{"op":"transpose_notes","pattern_id":"l","selector":{"all":true},"interval":{"steps":0}}])");
    assert(!unsupported.valid && unsupported.errorCode=="unsupported_pitch_transform");
    auto replace=edit(*shift.composition,R"([{"op":"update_notes","pattern_id":"p","selector":{"all":true},"set":{"note":"C4"}}])");
    assert(!replace.valid && replace.errorCode=="unsupported_pitch_transform");
    auto clear=edit(*shift.composition,R"([{"op":"update_notes","pattern_id":"p","selector":{"all":true},"set":{"note":"C4"},"unset":["transposeSteps"]}])");
    assert(clear.valid && clear.composition->patterns.at("p").steps[0].compiledPitchV==0);
    auto unequal=edit(*b,R"([{"op":"upsert_pattern","id":"p","pattern":{"length":4,"pitchContext":"u","steps":[{"step":0,"tuned":{"step":0}},{"step":1,"tuned":{"step":1}}]}},{"op":"update_scene_assignment","scene_id":"s","track_id":"v","set":{"overrides":{"transposeSteps":1,"transposePeriods":1,"transposeCents":-1.5}}}])");
    assert(unequal.valid);
    const auto& assignment=unequal.composition->arrangement[0].tracks.at("v");
    assert(std::abs((*assignment.compiledPitches)[0]-(1.+198.5/1200))<1e-6);
    assert(std::abs((*assignment.compiledPitches)[1]-(1.+698.5/1200))<1e-6);
    assert(changedTrackChannelMask(*b,*unequal.composition)==1);
    auto partial=edit(*unequal.composition,R"([{"op":"update_scene_assignment","scene_id":"s","track_id":"v","set":{"overrides":{"velocityScale":0.5}},"unset":["overrides.transposeCents"]}])");
    assert(partial.valid && partial.composition->arrangement[0].tracks.at("v").overrides.pitchOffsets.fields==3);
    auto nearest=edit(*b,R"([{"op":"retune_notes","pattern_id":"l","selector":{"all":true},"target_context":"c","mode":"nearest","target":"tuning","max_error_cents":10}])");
    assert(nearest.valid && std::abs(nearest.composition->patterns.at("l").steps[0].compiledPitchV-31./53)<1e-6);
    auto preserve=edit(*b,R"([{"op":"retune_notes","pattern_id":"l","selector":{"all":true},"target_context":"c","mode":"preserve","target":"scale","max_error_cents":10}])");
    assert(preserve.valid && preserve.composition->patterns.at("l").steps[0].compiledPitchV==b->patterns.at("l").steps[0].compiledPitchV);
    auto tolerance=edit(*b,R"([{"op":"retune_notes","pattern_id":"l","selector":{"all":true},"target_context":"c","mode":"preserve","target":"tuning","max_error_cents":1}])");
    assert(!tolerance.valid && tolerance.errorCode=="quantization_error_exceeded");
    auto reinterpreted=edit(*b,R"([{"op":"retune_notes","pattern_id":"p","selector":{"steps":[1]},"target_context":"u","mode":"reinterpret","target":"tuning"}])");
    assert(reinterpreted.valid && std::abs(reinterpreted.composition->patterns.at("p").steps[1].compiledPitchV-(5.+700./1200))<1e-6);
    auto wrongKind=edit(*b,R"([{"op":"retune_notes","pattern_id":"p","selector":{"steps":[1]},"target_context":"u","mode":"reinterpret","target":"scale"}])");
    assert(!wrongKind.valid && wrongKind.errorCode=="unsupported_pitch_transform");
    auto destination=edit(*b,R"([{"op":"duplicate_notes","pattern_id":"p","selector":{"steps":[1]},"destination_pattern_id":"dest","offset_steps":0,"pitch_context_policy":"destination"}])");
    assert(destination.valid && std::abs(destination.composition->patterns.at("dest").steps[0].compiledPitchV-(5.+700./1200))<1e-6);
    auto metadata=edit(*b,R"([{"op":"upsert_tuning","id":"t","tuning":{"kind":"equal","divisions":53,"period":{"ratio":"2/1"},"name":"new label"}}])");
    assert(metadata.valid && changedTrackChannelMask(*b,*metadata.composition)==0);
    auto scaleOnly=edit(*b,R"([{"op":"upsert_pitch_scale","id":"s","scale":{"tuning":"t","steps":[0,10,18,22,32,40,49]}}])");
    assert(scaleOnly.valid && changedTrackChannelMask(*b,*scaleOnly.composition)==0);
    auto degreeConsumer=edit(*b,R"([{"op":"update_notes","pattern_id":"p","selector":{"steps":[0]},"set":{"tuned":{"degree":1}}}])");assert(degreeConsumer.valid);
    auto degreeScale=edit(*degreeConsumer.composition,R"([{"op":"upsert_pitch_scale","id":"s","scale":{"tuning":"t","steps":[0,10,18,22,32,40,49]}}])");
    assert(degreeScale.valid && changedTrackChannelMask(*degreeConsumer.composition,*degreeScale.composition)==1);
    auto changed=edit(*b,R"([{"op":"upsert_tuning","id":"t","tuning":{"kind":"equal","divisions":54,"period":{"ratio":"2/1"}}}])");
    assert(changed.valid && changedTrackChannelMask(*b,*changed.composition)==1);
    auto removed=edit(*b,R"([{"op":"delete_tuning","id":"t"}])");assert(!removed.valid);
    auto failed=edit(*b,R"([{"op":"transpose_notes","pattern_id":"p","selector":{"all":true},"interval":{"steps":1}},{"op":"delete_tuning","id":"t"}])");assert(!failed.valid && serializeFullCompositionJson(*b)==original);
    auto ignoredProbability=edit(*b,R"([{"op":"update_notes","pattern_id":"l","selector":{"all":true},"set":{"probability":0}},{"op":"set_scene_assignment","scene_id":"s","track_id":"v","assignment":{"pattern":"l","overrides":{"transposeSteps":1}}}])");
    assert(!ignoredProbability.valid && ignoredProbability.errorCode=="unsupported_pitch_transform");
    auto tritave=edit(*b,R"([{"op":"upsert_tuning","id":"tri","tuning":{"kind":"equal","divisions":13,"period":{"ratio":"3/1"}}},{"op":"upsert_pitch_context","id":"tri","context":{"tuning":"tri","anchor":{"pitchV":0}}},{"op":"set_pattern_pitch_context","pattern_id":"p","context_id":"tri"},{"op":"transpose_notes","pattern_id":"p","selector":{"all":true},"interval":{"periods":1}}])");
    assert(tritave.valid && std::abs(tritave.composition->patterns.at("p").steps[0].compiledPitchV-std::log2(3.))<1e-6);
    for(const char* tie:{"lower","higher"}) {
        std::string ops=std::string(R"([{"op":"upsert_tuning","id":"one","tuning":{"kind":"equal","divisions":1,"period":{"ratio":"2/1"}}},{"op":"upsert_pitch_context","id":"one","context":{"tuning":"one","anchor":{"pitchV":0}}},{"op":"update_notes","pattern_id":"l","selector":{"all":true},"set":{"pitchV":0.5}},{"op":"retune_notes","pattern_id":"l","selector":{"all":true},"target_context":"one","target":"tuning","mode":"nearest","tie_break":")")+tie+"\"}]";
        auto tied=edit(*b,ops.c_str());assert(tied.valid && tied.composition->patterns.at("l").steps[0].compiledPitchV==(std::string(tie)=="lower"?0.f:1.f));
    }
    auto ordered=edit(*b,R"([{"op":"upsert_pitch_context","id":"future","context":{"tuning":"later","anchor":{"pitchV":0}}},{"op":"upsert_tuning","id":"later","tuning":{"kind":"equal","divisions":38,"period":{"ratio":"2/1"}}}])");assert(ordered.valid);
    assert(serializeFullCompositionJson(*b)==original);
    json_t* query=decode(R"({"view":"pitch_systems","page_size":1})");
    json_t* page=decode(serializePitchView(*b,query).c_str());assert(json_is_string(json_object_get(page,"cursor")));
    json_object_set(query,"cursor",json_object_get(page,"cursor"));json_decref(page);
    page=decode(serializePitchView(*metadata.composition,query).c_str());assert(json_is_false(json_object_get(page,"ok")));json_decref(page);json_decref(query);
    query=decode(R"({"view":"notes","pattern_id":"p","fields":["tuned","pitchDetails"]})");
    page=decode(serializeNotesView(*b,query).c_str());assert(json_is_true(json_object_get(page,"ok")));json_decref(page);json_decref(query);
    {
        json_t* root=decode(pitchEditFixture());
        json_object_set_new(json_object_get(json_object_get(root,"patterns"),"p"),"length",json_integer(1024));
        json_t* tracks=json_array(),*scenes=json_array();
        json_object_set_new(root,"tracks",tracks);json_object_set_new(root,"arrangement",scenes);
        for(int channel=0;channel<16;++channel) {
            const auto id="v"+std::to_string(channel);
            json_array_append_new(tracks,json_pack("{s:s,s:i}","id",id.c_str(),"channel",channel));
        }
        for(int scene=0;scene<65;++scene) {
            const auto id="s"+std::to_string(scene);json_t* assignments=json_object();
            for(int channel=0;channel<16;++channel) json_object_set_new(assignments,("v"+std::to_string(channel)).c_str(),json_string("p"));
            json_array_append_new(scenes,json_pack("{s:s,s:i,s:o}","id",id.c_str(),"lengthBeats",4,"tracks",assignments));
        }
        auto shared=parseCompositionJson(harmony_json::dump(root),1);
        assert(shared.valid && shared.composition->staticPitchEntries==1024);
        assert(shared.composition->arrangement[0].tracks.at("v0").compiledPitches==shared.composition->arrangement[64].tracks.at("v15").compiledPitches);
        for(int scene=0;scene<65;++scene) for(int channel=0;channel<16;++channel) {
            json_t* assignments=json_object_get(json_array_get(scenes,scene),"tracks");
            json_object_set_new(assignments,("v"+std::to_string(channel)).c_str(),json_pack("{s:s,s:{s:f}}","pattern","p","overrides","transposeCents",double(scene*16+channel)/1000.));
        }
        auto exhausted=parseCompositionJson(harmony_json::dump(root),1);json_decref(root);
        assert(!exhausted.valid && exhausted.errors[0].code=="capacity_exceeded");
    }
    std::cout<<"PASS: P7B transforms, unequal scene offsets, retuning, copy policy, definition transactions and views\n";
}
