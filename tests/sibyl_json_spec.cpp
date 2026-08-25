#include "SibylJSON.hpp"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

namespace {

int failures = 0;

void check(bool condition, const std::string& name) {
    if (condition) std::cout << "[PASS] " << name << "\n";
    else { std::cout << "[FAIL] " << name << "\n"; ++failures; }
}

bool hasPath(const std::vector<sibyl::ValidationIssue>& issues, const std::string& path) {
    for (const auto& issue : issues) if (issue.path == path) return true;
    return false;
}

const char* validComposition = R"JSON({
  "meta":{"title":"Contract Core","prompt":"Strict v2 fixture","bpm":130,"root":"F","rootOctave":3,"scale":"dorian","swing":0.12,"seed":42731},
  "clock":{"externalPpqn":4,"outputPpqn":24,"externalTimeoutMs":2000,"onExternalStop":"hold"},
  "transport":{"running":true,"loop":true,"defaultApplyAt":"nextBeat"},
  "tracks":[
    {"id":"bass","channel":0,"defaultGate":0.5,"defaultVelocity":0.8},
    {"id":"lead","channel":3,"defaultGate":4,"defaultVelocity":0.7}
  ],
  "patterns":{
    "bassline":{"length":16,"resolution":"1/16","steps":[
      {"step":0,"degree":0,"octave":-1,"gate":4,"velocity":1,"mod":-10,"mod2":6.5,"mod3":10,"observation":{"octaviaModuleId":9876,"monitors":["A","B"],"preFrames":4800,"postFrames":12000,"label":"filter transition"}},
      {"step":7,"note":"Eb4","probability":0.8,"ratchets":3},
      {"step":14,"pitchV":-1.0,"glideMs":100,"microshift":-0.08}
    ]}
  },
  "arrangement":[{"id":"intro","name":"Intro","description":"A restrained opening that establishes the bass ritual.","lengthBeats":16,"repeats":2,"phaseMode":"restart","tracks":{"bass":"bassline","lead":null}}],
  "macros":{"1":{"target":"global.probability","amount":0.5,"polarity":"unipolar","clamp":[0,1]},"2":{"target":"track.lead.mod","amount":0.25,"polarity":"bipolar","clamp":[-1,1]}}
})JSON";

void expectInvalid(const std::string& json, const std::string& path, const std::string& name) {
    auto result = sibyl::parseCompositionJson(json, 1);
    check(!result.valid && hasPath(result.errors, path), name);
}

} // namespace

int main() {
    auto valid = sibyl::parseCompositionJson(validComposition, 42);
    check(valid.valid && valid.errors.empty(), "complete v2 composition compiles");
    check(valid.composition && valid.composition->revision == 42, "compiler assigns requested revision");
    check(valid.composition && valid.composition->macros.size() == 2, "macros compile into immutable snapshot");
    check(valid.composition && valid.composition->arrangement[0].description ==
          "A restrained opening that establishes the bass ritual.",
          "scene descriptions compile into immutable arrangement metadata");
    check(sibyl::serializeFullCompositionJson(*valid.composition).find(
          "A restrained opening that establishes the bass ritual.") != std::string::npos &&
          sibyl::serializeSummaryJson(*valid.composition).find(
          "A restrained opening that establishes the bass ritual.") != std::string::npos,
          "scene descriptions appear in full and summary views");
    check(valid.composition && std::abs(valid.composition->patterns.at("bassline").steps[0].compiledPitchV - (-1.5833333f)) < 1e-5f,
          "negative scale octave compiles with Euclidean degree semantics");
    check(valid.composition && valid.composition->patterns.at("bassline").steps[0].hasMod2 &&
          valid.composition->patterns.at("bassline").steps[0].hasMod3,
          "MOD2 and MOD3 values compile as independent modulation lanes");
    check(valid.composition && valid.composition->patterns.at("bassline").steps[0].gate == 4.0f &&
          valid.composition->tracks[1].defaultGate == 4.0f,
          "multi-step event and track-default gates compile");
    const sibyl::StepEvent& observed = valid.composition->patterns.at("bassline").steps[0];
    check(observed.hasObservation && observed.observation.octaviaModuleId == 9876
          && observed.observation.monitorMask == 0x0c && observed.observation.preFrames == 4800
          && observed.observation.postFrames == 12000
          && observed.observation.label == "filter transition",
          "event observation marker compiles to an immutable exact-frame request");
    const std::string serialized = sibyl::serializeFullCompositionJson(*valid.composition);
    check(serialized.find("\"observation\"") != std::string::npos
          && serialized.find("filter transition") != std::string::npos,
          "observation marker survives portable composition serialization");

    auto forwardCompatible = sibyl::parseCompositionJson(R"({"meta":{"bpm":120,"futureColor":"violet"},"futureTop":7})", 1);
    check(forwardCompatible.valid && hasPath(forwardCompatible.warnings, "meta.futureColor") && hasPath(forwardCompatible.warnings, "futureTop"),
          "unknown fields warn without rejecting composition");

    expectInvalid("[]", "$", "non-object composition is rejected");
    expectInvalid(R"({"meta":{"scale":"enigmatic"}})", "meta.scale", "unknown scale is rejected");
    expectInvalid(R"({"transport":{"defaultApplyAt":"later"}})", "transport.defaultApplyAt", "unknown adoption boundary is rejected");
    expectInvalid(R"({"clock":{"onExternalStop":"guess"}})", "clock.onExternalStop", "unknown clock fallback is rejected");
    expectInvalid(R"({"patterns":{"p":{"length":16,"resolution":"1/12","steps":[]}}})", "patterns.p.resolution", "unsupported resolution is rejected");
    expectInvalid(R"({"patterns":{"p":{"length":1,"resolution":"1/16","steps":[{"step":0,"note":"C10"}]}}})", "patterns.p.steps[0].note", "note grammar and octave range are enforced");
    expectInvalid(R"({"patterns":{"p":{"length":1,"resolution":"1/16","steps":[{"step":0,"pitchV":0,"degree":1}]}}})", "patterns.p.steps[0]", "conflicting pitch representations are rejected");
    expectInvalid(R"({"patterns":{"p":{"length":1,"resolution":"1/16","steps":[{"step":0,"pitchV":0,"probability":1.1}]}}})", "patterns.p.steps[0].probability", "event ranges are enforced");
    expectInvalid(R"({"patterns":{"p":{"length":1,"resolution":"1/16","steps":[{"step":0,"pitchV":0,"mod2":10.1}]}}})", "patterns.p.steps[0].mod2", "secondary modulation voltage ranges are enforced");
    expectInvalid(R"({"patterns":{"p":{"length":1,"resolution":"1/16","steps":[{"step":0,"pitchV":0,"gate":1.1,"ratchets":2}]}}})", "patterns.p.steps[0].gate", "multi-step gates are rejected for ratcheted events");
    expectInvalid(R"({"patterns":{"p":{"length":1,"resolution":"1/16","steps":[{"step":0,"pitchV":0,"observation":{"monitors":["A"]}}]}}})", "patterns.p.steps[0].observation.octaviaModuleId", "observation requires an Octavia module ID");
    expectInvalid(R"({"patterns":{"p":{"length":1,"resolution":"1/16","steps":[{"step":0,"pitchV":0,"observation":{"octaviaModuleId":1,"monitors":["A","A"]}}]}}})", "patterns.p.steps[0].observation.monitors[1]", "observation monitor names are unique and validated");
    expectInvalid(R"({"patterns":{"p":{"length":1,"resolution":"1/16","steps":[{"step":0,"pitchV":0,"observation":{"octaviaModuleId":1,"monitors":["A"],"preFrames":200000,"postFrames":100000}}]}}})", "patterns.p.steps[0].observation", "observation window is bounded by retained history");
    expectInvalid(R"({"tracks":[{"id":"a","channel":0},{"id":"b","channel":0}]})", "tracks[1].channel", "duplicate channels are rejected");
    expectInvalid(R"({"tracks":[{"id":"a","channel":0}],"arrangement":[{"id":"s","lengthBeats":4,"repeats":1,"tracks":{"missing":null}}]})", "arrangement[0].tracks.missing", "scene track references are resolved");
    expectInvalid(std::string("{\"arrangement\":[{\"id\":\"s\",\"description\":\"") +
        std::string(513, 'x') + "\"}]}", "arrangement[0].description",
        "scene descriptions are bounded for agent and display use");
    expectInvalid(R"({"tracks":[{"id":"a","channel":0}],"macros":{"1":{"target":"track.missing.gate","amount":1,"polarity":"unipolar"}}})", "macros.1.target", "macro track targets are resolved");
    expectInvalid(R"({"meta":{"bpm":"fast"}})", "meta.bpm", "invalid field types are rejected instead of defaulted");

    std::cout << "[SUMMARY] sibyl_json_spec: " << (failures ? "FAILED" : "passed") << "\n";
    return failures == 0 ? 0 : 1;
}
