#include "../src/SibylNoteEdit.hpp"
#include "../src/SibylAdoption.hpp"
#include <cassert>
#include <iostream>
using namespace sibyl;
json_t* decode(const char* text) { json_error_t e; auto* j=json_loads(text,0,&e); assert(j); return j; }
EditResult edit(const Composition& c,const char* text) {
    auto* j=decode(text); auto result=applyCompositionEdit(c,j,c.revision+1); json_decref(j); return result;
}
CompositionPtr base() {
    auto p=parseCompositionJson(R"({"meta":{"scale":"major"},"tracks":[{"id":"v","channel":0,"defaultVelocity":0.8,"defaultGate":0.6}],"patterns":{"p":{"length":16,"steps":[{"step":0,"degree":0,"velocity":0.2,"probability":0.5,"mod":1,"mod2":2,"mod3":3,"evolve":false},{"step":4,"note":"C4","velocity":0.35},{"step":8,"pitchV":1,"velocity":0.8},{"step":12,"degree":4},{"step":15,"note":"G4"}],"evolution":{"probability":0.2}},"dest":{"length":16,"steps":[]}},"arrangement":[{"id":"s","tracks":{"v":"p"}}]})",1);
    assert(p.valid);return p.composition;
}
#include "sibyl_pitch_edit_cases.hpp"
#include "sibyl_native_pitch_cases.hpp"
void noteBatchCases() {
    auto native = parseCompositionJson(nativeHarmonyFixture(), 1);
    assert(native.valid);
    auto isolated = edit(*native.composition, R"([{"op":"insert_note_batch","pattern_id":"dest","encoding":"columns_v1","columns":["step","tuned.step"],"rows":[[0,0],[1,17],[2,31]],"defaults":{"tuned":{"context":"c"}}}])");
    assert(isolated.valid);
    const auto& isolatedNotes = isolated.composition->patterns.at("dest").steps;
    assert(isolatedNotes.size() == 3);
    assert(isolatedNotes[0].nativePitch.index == 0);
    assert(isolatedNotes[1].nativePitch.index == 17);
    assert(isolatedNotes[2].nativePitch.index == 31);
    auto b = base();
    auto series=edit(*b,R"([{"op":"insert_note_batch","pattern_id":"dest","encoding":"columns_v1","columns":["degree","velocity"],"rows":[[0,0.8],[4,0.6]],"onsets":{"start":1,"spacing":3,"count":4},"defaults":{"gate":0.5},"overrides":{"3":{"degree":2,"gate":0}},"expect_count":4}])");
    auto exact=edit(*b,R"([{"op":"insert_notes","pattern_id":"dest","notes":[{"step":1,"degree":0,"velocity":0.8,"gate":0.5},{"step":4,"degree":4,"velocity":0.6,"gate":0.5},{"step":7,"degree":0,"velocity":0.8,"gate":0.5},{"step":10,"degree":2,"velocity":0.6,"gate":0}]}])");
    assert(series.valid && exact.valid && serializeFullCompositionJson(*series.composition)==serializeFullCompositionJson(*exact.composition));
    for(const char* invalid:{R"({"-1":{"gate":1}})",R"({"04":{"gate":1}})",R"({"4":{"gate":1}})",R"({"0":{"unknown":1}})"}) {
        std::string request=std::string(R"([{"op":"insert_note_batch","pattern_id":"dest","encoding":"columns_v1","columns":["degree"],"rows":[[0]],"onsets":{"start":0,"spacing":4,"count":4},"overrides":)")+invalid+"}]";
        assert(!edit(*b,request.c_str()).valid);
    }
    auto readRequest=decode(R"({"view":"notes","pattern_id":"p","fields":"full","encoding":"columns_v1"})");
    auto columnar=decode(serializeNotesView(*b,readRequest).c_str());
    json_object_del(readRequest,"encoding");
    auto objects=decode(serializeNotesView(*b,readRequest).c_str());
    json_t* rows=json_object_get(columnar,"rows"), *columns=json_object_get(columnar,"columns");
    assert(json_array_size(rows)==5);
    for(size_t i=0;i<json_array_size(rows);++i) {
        json_t* reconstructed=json_object();
        json_t* absent=json_object_get(json_object_get(columnar,"missing"),std::to_string(i).c_str());
        for(size_t c=0;c<json_array_size(columns);++c) {
            bool missing=false;
            size_t j; json_t* v;json_array_foreach(absent,j,v) missing |= json_integer_value(v)==json_int_t(c);
            if(!missing) json_object_set(reconstructed,json_string_value(json_array_get(columns,c)),json_array_get(json_array_get(rows,i),c));
        }
        assert(json_equal(reconstructed,json_array_get(json_object_get(objects,"notes"),i)));
        json_decref(reconstructed);
    }
    json_decref(readRequest);json_decref(columnar);json_decref(objects);
    // 1. Basic columnar batch insert with defaults and expect_count
    auto batch1 = edit(*b, R"([{"op":"insert_note_batch","pattern_id":"dest","encoding":"columns_v1","columns":["step","degree","velocity","gate"],"rows":[[1,0,0.7,0.5],[3,2,0.8,0.6],[5,4,0.9,0.7]],"defaults":{"probability":0.85},"expect_count":3}])");
    assert(batch1.valid && batch1.changes.size() == 1);
    assert(batch1.changes[0].inserted == 3);
    assert(batch1.composition->patterns.at("dest").steps.size() == 3);
    assert(batch1.composition->patterns.at("dest").steps[0].step == 1);
    assert(batch1.composition->patterns.at("dest").steps[0].degree == 0);
    assert(batch1.composition->patterns.at("dest").steps[0].velocity == 0.7f);
    assert(batch1.composition->patterns.at("dest").steps[0].gate == 0.5f);
    assert(batch1.composition->patterns.at("dest").steps[0].probability == 0.85f);

    // 2. Collision replacement with batch
    auto batch2 = edit(*batch1.composition, R"([{"op":"insert_note_batch","pattern_id":"dest","encoding":"columns_v1","columns":["step","note"],"rows":[[1,"E4"],[7,"G4"]],"collision":"replace"}])");
    assert(batch2.valid && batch2.changes[0].inserted == 2 && batch2.changes[0].deleted == 1);
    assert(batch2.changes[0].displacedIds.size() == 1);
    assert(batch2.composition->patterns.at("dest").steps.size() == 4);

    // 3. Null cells inherit defaults
    auto batch3 = edit(*b, R"([{"op":"insert_note_batch","pattern_id":"dest","encoding":"columns_v1","columns":["step","degree","velocity"],"rows":[[0,0,null],[2,1,0.5]],"defaults":{"velocity":0.65,"probability":1.0}}])");
    assert(batch3.valid && batch3.changes[0].inserted == 2);
    assert(batch3.composition->patterns.at("dest").steps[0].velocity == 0.65f);
    assert(batch3.composition->patterns.at("dest").steps[1].velocity == 0.5f);

    // 4. Invalid operation: invalid encoding
    auto badEncoding = edit(*b, R"([{"op":"insert_note_batch","pattern_id":"dest","encoding":"columns_v0","columns":["step","degree"],"rows":[[0,1]]}])");
    assert(!badEncoding.valid && badEncoding.errorCode == "invalid_operation");

    // 5. Invalid operation: row length mismatch
    auto badRow = edit(*b, R"([{"op":"insert_note_batch","pattern_id":"dest","encoding":"columns_v1","columns":["step","degree","velocity"],"rows":[[0,1]]}])");
    assert(!badRow.valid && badRow.errorCode == "invalid_operation");

    // 6. Duplicate or conflicting column
    auto dupCol = edit(*b, R"([{"op":"insert_note_batch","pattern_id":"dest","encoding":"columns_v1","columns":["step","degree","degree"],"rows":[[0,1,2]]}])");
    assert(!dupCol.valid && dupCol.errorCode == "duplicate_or_conflicting_column");

    auto confCol = edit(*b, R"([{"op":"insert_note_batch","pattern_id":"dest","encoding":"columns_v1","columns":["step","tuned","tuned.step"],"rows":[[0,{},1]]}])");
    assert(!confCol.valid && confCol.errorCode == "duplicate_or_conflicting_column");

    // 7. Expect count mismatch
    auto badCount = edit(*b, R"([{"op":"insert_note_batch","pattern_id":"dest","encoding":"columns_v1","columns":["step","degree"],"rows":[[0,1]],"expect_count":2}])");
    assert(!badCount.valid && badCount.errorCode == "selection_count_mismatch");

    // 8. Step collision within batch
    auto batchCollision = edit(*b, R"([{"op":"insert_note_batch","pattern_id":"dest","encoding":"columns_v1","columns":["step","degree"],"rows":[[2,1],[2,3]]}])");
    assert(!batchCollision.valid && batchCollision.errorCode == "step_collision");

    // 9. Step collision with existing pattern (default collision: "error")
    auto existCollision = edit(*b, R"([{"op":"insert_note_batch","pattern_id":"p","encoding":"columns_v1","columns":["step","note"],"rows":[[0,"C4"]]}])");
    assert(!existCollision.valid && existCollision.errorCode == "step_collision");

    std::cout << "PASS: P8B insert_note_batch columnar event batch tests\n";
}

int main() {
    noteBatchCases();
    pitchEditCases();
    nativePitchCases();
    auto b=base(); auto original=serializeFullCompositionJson(*b);
    auto trans=edit(*b,R"([{"op":"transpose_notes","pattern_id":"p","selector":{"order":"stepDesc","limit":4},"expect_count":4,"semitones":-12}])");
    assert(trans.valid && trans.changes[0].updated==4);
    for(size_t i=0;i<5;++i) {
        auto& before=b->patterns.at("p").steps[i];auto& after=trans.composition->patterns.at("p").steps[i];
        assert(before.id==after.id && before.pitchType==after.pitchType && after.compiledPitchV==before.compiledPitchV-(i?1:0));
    }
    auto degree=edit(*b,R"([{"op":"transpose_notes","pattern_id":"p","selector":{"all":true},"degrees":1}])");
    assert(!degree.valid&&degree.errorCode=="unsupported_pitch_transform");
    auto ghost=edit(*b,R"([{"op":"update_notes","pattern_id":"p","selector":{"where":{"velocity":{"lt":0.4}}},"adjust":{"probability":{"multiply":0.6}},"expect_count":2}])");
    assert(ghost.valid&&ghost.composition->patterns.at("p").steps[0].probability==.3f&&ghost.composition->patterns.at("p").steps[1].probability==.6f);
    assert(!ghost.composition->patterns.at("p").steps[0].evolve&&ghost.composition->patterns.at("p").steps[0].mod3==3.f);
    assert(ghost.composition->patterns.at("p").evolution.probability==.2f);
    auto inherit=edit(*b,R"([{"op":"update_notes","pattern_id":"p","selector":{"ids":["n4"]},"adjust":{"velocity":{"multiply":0.5}}}])");
    assert(!inherit.valid&&inherit.errorCode=="inherited_value_requires_track");
    inherit=edit(*b,R"([{"op":"update_notes","pattern_id":"p","selector":{"ids":["n4"]},"resolve_defaults_for_track":"v","adjust":{"velocity":{"multiply":0.5}}}])");
    assert(inherit.valid&&inherit.composition->patterns.at("p").steps[3].velocity==.4f);
    auto unset=edit(*b,R"([{"op":"update_notes","pattern_id":"p","selector":{"ids":["n1"]},"unset":["probability"],"set":{"pitchV":0.5}}])");
    assert(unset.valid&&!unset.composition->patterns.at("p").steps[0].hasProbability&&unset.composition->patterns.at("p").steps[0].pitchType==PitchType::PITCH_V);
    auto rotate=edit(*b,R"([{"op":"rotate_notes","pattern_id":"p","selector":{"steps":[0,4,8,12]},"steps":4}])");
    assert(rotate.valid&&rotate.composition->patterns.at("p").steps[0].id=="n4");
    auto collide=edit(*b,R"([{"op":"rotate_notes","pattern_id":"p","selector":{"ids":["n1"]},"steps":4}])");
    assert(!collide.valid&&collide.errorCode=="step_collision");
    auto replace=edit(*b,R"([{"op":"rotate_notes","pattern_id":"p","selector":{"ids":["n1"]},"steps":4,"collision":"replace"}])");
    assert(replace.valid&&replace.changes[0].displacedIds[0]=="n2"&&replace.composition->patterns.at("p").steps[0].id=="n1");
    auto copy=edit(*b,R"([{"op":"duplicate_notes","pattern_id":"p","selector":{"ids":["n5"]},"offset_steps":2}])");
    assert(!copy.valid);
    copy=edit(*b,R"([{"op":"duplicate_notes","pattern_id":"p","selector":{"ids":["n5"]},"offset_steps":2,"wrap":true,"return_id_mapping":true}])");
    assert(copy.valid&&copy.changes[0].createdIds[0].second=="n6"&&copy.composition->patterns.at("p").nextNoteId==7);
    auto repeat=edit(*b,R"([{"op":"duplicate_notes","pattern_id":"p","selector":{"ids":["n5"]},"offset_steps":2,"wrap":true,"return_id_mapping":true}])");
    assert(repeat.valid&&repeat.changes[0].createdIds==copy.changes[0].createdIds);
    auto selfCopy=edit(*b,R"([{"op":"duplicate_notes","pattern_id":"p","selector":{"all":true},"offset_steps":0,"collision":"replace"}])");
    assert(!selfCopy.valid&&selfCopy.errorCode=="step_collision");
    auto cross=edit(*b,R"([{"op":"duplicate_notes","pattern_id":"p","selector":{"all":true},"destination_pattern_id":"dest","offset_steps":0}])");
    assert(cross.valid&&cross.composition->patterns.at("dest").steps.size()==5);
    auto chain=edit(*b,R"([{"op":"delete_notes","pattern_id":"p","selector":{"ids":["n3"]}},{"op":"insert_notes","pattern_id":"p","notes":[{"step":8,"note":"D4"}]},{"op":"rotate_notes","pattern_id":"p","selector":{"ids":["n6"]},"steps":1}])");
    assert(chain.valid&&chain.composition->patterns.at("p").steps[2].id=="n6"&&chain.composition->patterns.at("p").steps[2].step==9);
    auto reloaded=parseCompositionJson(serializeFullCompositionJson(*chain.composition),8);
    assert(reloaded.valid&&reloaded.composition->patterns.at("p").nextNoteId==7);
    auto upsert=edit(*chain.composition,R"([{"op":"upsert_pattern","id":"p","pattern":{"steps":[{"step":9,"note":"E4"},{"step":10,"note":"F4"}]}}])");
    assert(upsert.valid&&upsert.composition->patterns.at("p").steps[0].id=="n6"&&upsert.composition->patterns.at("p").steps[1].id=="n7");
    auto rollback=edit(*b,R"([{"op":"insert_notes","pattern_id":"p","notes":[{"step":1,"note":"D4"}]},{"op":"rotate_notes","pattern_id":"p","selector":{"ids":["n1"]},"steps":4}])");
    assert(!rollback.valid&&serializeFullCompositionJson(*b)==original);
    for(const char* invalid:{
        R"([{"op":"update_notes","pattern_id":"p","selector":{"all":true},"sett":{"velocity":0.5}}])",
        R"([{"op":"delete_notes","pattern_id":"p","selector":{}}])",
        R"([{"op":"delete_notes","pattern_id":"p","selector":{"ids":["n1","n1"]}}])",
        R"([{"op":"delete_notes","pattern_id":"p","selector":{"ids":["absent"]}}])",
        R"([{"op":"delete_notes","pattern_id":"p","selector":{"all":true,"steps":[0]}}])",
        R"([{"op":"delete_notes","pattern_id":"p","selector":{"all":true},"expect_count":4}])",
        R"([{"op":"update_notes","pattern_id":"p","selector":{"all":true},"set":{"velocity":0.5},"unset":["velocity"]}])",
        R"([{"op":"update_notes","pattern_id":"p","selector":{"all":true},"set":{"condition":{"all":[{"scope":"patternPass","every":0}]}}}])",
        R"([{"op":"update_notes","pattern_id":"p","selector":{"all":true},"adjust":{"probability":{"add":1e300}}}])",
        R"([{"op":"insert_notes","pattern_id":"p","notes":[{"id":"n1","step":1,"note":"C4"}]}])"
    }) assert(!edit(*b,invalid).valid);
    auto clamped=edit(*b,R"([{"op":"update_notes","pattern_id":"p","selector":{"all":true},"adjust":{"probability":{"add":2}},"clamp":true}])");
    assert(clamped.valid&&clamped.changes[0].clamped==5);
    auto empty=edit(*b,R"([{"op":"delete_notes","pattern_id":"p","selector":{"steps":[1]},"allow_empty":true}])");
    assert(empty.valid&&!empty.warnings.empty());
    auto* request=decode(R"({"view":"notes","pattern_id":"p","selector":{"all":true},"fields":"full","page_size":2})");
    auto* page=decode(serializeNotesView(*b,request).c_str());assert(json_array_size(json_object_get(page,"notes"))==2);
    json_object_set(request,"cursor",json_object_get(page,"cursor"));json_decref(page);
    page=decode(serializeNotesView(*b,request).c_str());
    assert(std::string(json_string_value(json_object_get(json_array_get(json_object_get(page,"notes"),0),"id")))=="n3");json_decref(page);
    page=decode(serializeNotesView(*trans.composition,request).c_str());assert(std::string(json_string_value(json_object_get(json_object_get(page,"error"),"code")))=="revision_conflict");json_decref(page);json_decref(request);
    auto observed=parseCompositionJson(R"({"patterns":{"p":{"length":4,"steps":[{"step":0,"note":"C3","gate":0.4,"ratchets":2,"microshift":0.1,"glideMs":10,"evolve":false,"observation":{"octaviaModuleId":123,"monitors":["A"],"postFrames":32}}]}}})",1);
    assert(observed.valid);
    auto observedCopy=edit(*observed.composition,R"([{"op":"duplicate_notes","pattern_id":"p","selector":{"all":true},"offset_steps":1}])");
    assert(observedCopy.valid && observedCopy.composition->patterns.at("p").steps[1].hasObservation
        && observedCopy.composition->patterns.at("p").steps[1].ratchets==2 && !observedCopy.warnings.empty());
    auto stripped=edit(*observed.composition,R"([{"op":"duplicate_notes","pattern_id":"p","selector":{"all":true},"offset_steps":1,"copy_observations":false}])");
    assert(stripped.valid && !stripped.composition->patterns.at("p").steps[1].hasObservation);
    auto limited=edit(*b,R"([{"op":"transpose_notes","pattern_id":"p","selector":{"all":true},"semitones":1,"report_id_limit":2}])");
    auto* reports=editChangesJson(limited);
    assert(json_is_true(json_object_get(json_array_get(reports,0),"idsTruncated"))
        && json_array_size(json_object_get(json_array_get(reports,0),"noteIds"))==2);
    json_decref(reports);
    auto idOnly=*b;idOnly.patterns["p"].steps[0].id="custom";idOnly.patterns["p"].nextNoteId=50;
    assert(changedTrackChannelMask(*b,idOnly)==0);
    assert(changedTrackChannelMask(*b,*trans.composition)==1);
    std::cout << "PASS: P1 selectors, atomic transforms, IDs, inheritance, reports, read paging and adoption\n";
}
