#include "../src/SibylJSON.hpp"
#include <cassert>
#include <iostream>
int main() {
    const std::string legacy = R"({"patterns":{"p":{"length":16,"steps":[{"step":8,"note":"C3"},{"step":0,"degree":0}]}}})";
    auto p = sibyl::parseCompositionJson(legacy, 1);
    assert(p.valid);
    auto& pattern = p.composition->patterns.at("p");
    assert(pattern.steps[0].step == 0 && pattern.steps[0].id == "n1" && pattern.steps[1].id == "n2" && pattern.nextNoteId == 3);
    auto again = sibyl::parseCompositionJson(sibyl::serializeFullCompositionJson(*p.composition), 2);
    assert(again.valid && again.composition->patterns.at("p").steps[1].id == "n2");
    for (const char* doc : {R"({"schemaVersion":2,"composition":{}})", R"({"schemaVersion":3,"composition":{"schemaVersion":3}})", R"({"schemaVersion":3})"})
        assert(sibyl::parseCompositionJson(doc, 1).valid);
    for (const char* doc : {R"({"schemaVersion":4})", R"({"schemaVersion":2,"composition":{"schemaVersion":3}})", R"({"patterns":{"p":{"steps":[{"step":0,"note":"C3","condition":{}}]}}})", R"({"schemaVersion":2,"harmony":{}})"})
        assert(!sibyl::parseCompositionJson(doc, 1).valid);
    auto transposed = sibyl::parseCompositionJson(R"({"schemaVersion":3,"patterns":{"p":{"steps":[{"step":0,"note":"C3","transposeSemitones":-12}]}}})",1);
    assert(transposed.valid && transposed.composition->patterns.at("p").steps[0].compiledPitchV == -2.f);
    auto invalid = sibyl::parseCompositionJson(R"({"schemaVersion":3,"patterns":{"p":{"steps":[{"step":0,"pitchV":10,"transposeSemitones":1}]}}})",1);
    assert(!invalid.valid && invalid.errors[0].code == "pitch_out_of_range");
    for (const char* doc : {
        R"({"schemaVersion":3,"patterns":{"p":{"nextNoteId":2147483647,"steps":[{"step":0,"note":"C3"}]}}})",
        R"({"schemaVersion":3,"patterns":{"p":{"steps":[{"id":"same","step":0,"note":"C3"},{"id":"same","step":1,"note":"D3"}]}}})",
        R"({"schemaVersion":3,"patterns":{"p":{"steps":[{"id":"bad\n","step":0,"note":"C3"}]}}})",
        R"({"schemaVersion":3,"patterns":{"p":{"nextNoteId":1.5,"steps":[]}}})",
        R"({"schemaVersion":3,"patterns":{"p":{"steps":[{"step":0,"degree":-2147483648,"octave":-2147483648}]}}})"
    }) assert(!sibyl::parseCompositionJson(doc,1).valid);
    auto occupied=sibyl::parseCompositionJson(R"({"schemaVersion":3,"patterns":{"p":{"steps":[{"id":"n1","step":0,"note":"C3"},{"step":1,"note":"D3"}]}}})",1);
    assert(occupied.valid && occupied.composition->patterns.at("p").steps[1].id=="n2");
    std::cout << "PASS: schema normalization, deterministic IDs, roundtrip, feature gates and transpose\n";
}
