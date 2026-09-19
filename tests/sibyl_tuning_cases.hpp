#pragma once
#include "../src/SibylTuning.hpp"
#include <cmath>

inline void tuningCases() {
    using namespace sibyl;
    for (int n=1;n<=1024;++n) {
        PitchTuning t; t.divisions=n;
        for (int k : {-2049,-54,-1,0,1,31,2050})
            assert(std::abs(t.lattice(k+n)-t.lattice(k)-1.)<1e-11);
    }
    PitchTuning unequal; unequal.divisions=3; unequal.positionsV={0.,.2,.7};
    assert(std::abs(unequal.lattice(-1)+.3)<1e-14);
    assert(std::abs(unequal.lattice(2)-unequal.lattice(1)-.5)<1e-14);
    assert(pitchDegreeIndex(-1,53,{0,9,17,22,31,39,48})==-5);
    const std::string prefix=R"({"schemaVersion":4,"pitchSystems":{"tunings":{"t":{"kind":"equal","divisions":53,"period":{"ratio":"2/1"}}},"scales":{"s":{"tuning":"t","steps":[0,9,17,22,31,39,48]}},"contexts":{"c":{"tuning":"t","scale":"s","anchor":{"note":"C4"}}},"defaultContext":"c"},"patterns":{"p":{"length":16,"steps":)";
    auto parse=[&](const std::string& notes) { return parseCompositionJson(prefix+notes+"}}}",1); };
    auto a=parse(R"([{"step":0,"tuned":{"step":31}},{"step":1,"tuned":{"degree":-1}},{"step":2,"tuned":{"ratio":"6/4"}},{"step":3,"note":"G4"}])");
    if(!a.valid) for(const auto& e:a.errors) std::cerr<<e.path<<": "<<e.message<<"\n";
    assert(a.valid);
    const auto& notes=a.composition->patterns.at("p").steps;
    assert(std::abs(notes[0].compiledPitchV-31./53)<1e-6);
    assert(std::abs(notes[1].compiledPitchV+5./53)<1e-6);
    assert(std::abs(notes[2].compiledPitchV-std::log2(1.5))<1e-6);
    assert(notes[2].compiledPitchV!=notes[0].compiledPitchV);
    assert(notes[3].compiledPitchV==7.f/12.f);
    auto round=parseCompositionJson(serializeFullCompositionJson(*a.composition),2);
    assert(round.valid && round.composition->patterns.at("p").steps[2].tuned.find("3/2")!=std::string::npos);
    for(const char* bad : {R"([{"step":0,"tuned":{"step":2147483648}}])",R"([{"step":0,"tuned":{"step":2147483647}}])",R"([{"step":0,"tuned":{"step":0,"degree":0}}])",R"([{"step":0,"tuned":{"ratio":"0/1"}}])",R"([{"step":0,"tuned":{"ratio":"1/0"}}])",R"([{"step":0,"tuned":{"context":"missing","step":0}}])"}) assert(!parse(bad).valid);
    auto gate=parseCompositionJson(R"({"schemaVersion":3,"pitchSystems":{}})",1);
    assert(!gate.valid && gate.errors[0].code=="schema_version_required");
    {
        const std::string table=R"({"schemaVersion":4,"pitchSystems":{"tunings":{"t":{"kind":"table","period":{"ratio":"3/1"},"positions":[{"ratio":"1/1"},{"ratio":"5/4"},{"cents":900}],"description":"retained"}},"contexts":{"c":{"tuning":"t","anchor":{"frequencyHz":261.6255653005986}}},"defaultContext":"c"},"patterns":{"p":{"steps":[{"step":0,"tuned":{"step":-1}},{"step":1,"tuned":{"cents":25,"periods":1}}]}}})";
        auto result=parseCompositionJson(table,1);
        assert(result.valid);
        assert(std::abs(result.composition->patterns.at("p").steps[0].compiledPitchV-(.75-std::log2(3.)))<1e-6);
        assert(std::abs(result.composition->patterns.at("p").steps[1].compiledPitchV-(25./1200+std::log2(3.)))<1e-6);
        const auto canonical=serializeFullCompositionJson(*result.composition);
        assert(canonical.find("retained")!=std::string::npos && canonical.find("5/4")!=std::string::npos);
        auto invalidTable=table; auto position=invalidTable.find("900"); invalidTable.replace(position,3,"100");
        assert(!parseCompositionJson(invalidTable,1).valid);
    }
    {
        const std::string twoContexts=R"({"schemaVersion":4,"pitchSystems":{"tunings":{"t":{"kind":"equal","divisions":53,"period":{"ratio":"2/1"}}},"contexts":{"c":{"tuning":"t","anchor":{"pitchV":0}},"d":{"tuning":"t","anchor":{"pitchV":1}}},"defaultContext":"c"},"patterns":{"p":{"pitchContext":"d","steps":[{"step":0,"tuned":{"step":0}},{"step":1,"tuned":{"context":"c","step":0}},{"step":2,"note":"C4"}]}}})";
        auto result=parseCompositionJson(twoContexts,1); assert(result.valid);
        const auto& events=result.composition->patterns.at("p").steps;
        assert(events[0].compiledPitchV==1.f && events[1].compiledPitchV==0.f && events[2].compiledPitchV==0.f);
    }
    for(int n : {38,53}) {
        std::string input=prefix+R"([{"step":0,"tuned":{"step":1}}])"+"}}}";
        const auto at=input.find("\"divisions\":53"); input.replace(at,14,"\"divisions\":"+std::to_string(n));
        // Use the full lattice here; the 53-position scale is invalid for 38-EDO.
        auto begin=input.find("\"scales\":"); auto end=input.find("\"contexts\":",begin); input.erase(begin,end-begin);
        auto scale=input.find("\"scale\":\"s\","); input.erase(scale,12);
        auto result=parseCompositionJson(input,1); assert(result.valid);
        assert(std::abs(result.composition->patterns.at("p").steps[0].compiledPitchV-1./n)<1e-6);
    }
    PitchTuning tritave; tritave.divisions=13; tritave.periodV=std::log2(3.);
    assert(std::abs(tritave.lattice(13)-std::log2(3.))<1e-14);
    std::cout<<"PASS: generic tuning math, native codec, exact ratios, bounds and 38/53-EDO\n";
}
