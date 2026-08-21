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
  "meta":{"title":"Contract Core","prompt":"Strict v1 fixture","bpm":130,"root":"F","rootOctave":3,"scale":"dorian","swing":0.12,"seed":42731},
  "clock":{"externalPpqn":4,"outputPpqn":24,"externalTimeoutMs":2000,"onExternalStop":"hold"},
  "transport":{"running":true,"loop":true,"defaultApplyAt":"nextBeat"},
  "tracks":[
    {"id":"bass","channel":0,"defaultGate":0.5,"defaultVelocity":0.8,"modRange":"unipolar"},
    {"id":"lead","channel":3,"defaultGate":0.4,"defaultVelocity":0.7,"modRange":"bipolar"}
  ],
  "patterns":{
    "bassline":{"length":16,"resolution":"1/16","steps":[
      {"step":0,"degree":0,"octave":-1,"gate":0.8,"velocity":1,"mod":0.25},
      {"step":7,"note":"Eb4","probability":0.8,"ratchets":3},
      {"step":14,"pitchV":-1.0,"glideMs":100,"microshift":-0.08}
    ]}
  },
  "arrangement":[{"id":"intro","name":"Intro","lengthBeats":16,"repeats":2,"phaseMode":"restart","tracks":{"bass":"bassline","lead":null}}],
  "macros":{"1":{"target":"global.probability","amount":0.5,"polarity":"unipolar","clamp":[0,1]},"2":{"target":"track.lead.mod","amount":0.25,"polarity":"bipolar","clamp":[-1,1]}}
})JSON";

void expectInvalid(const std::string& json, const std::string& path, const std::string& name) {
    auto result = sibyl::parseCompositionJson(json, 1);
    check(!result.valid && hasPath(result.errors, path), name);
}

} // namespace

int main() {
    auto valid = sibyl::parseCompositionJson(validComposition, 42);
    check(valid.valid && valid.errors.empty(), "complete v1 composition compiles");
    check(valid.composition && valid.composition->revision == 42, "compiler assigns requested revision");
    check(valid.composition && valid.composition->macros.size() == 2, "macros compile into immutable snapshot");
    check(valid.composition && std::abs(valid.composition->patterns.at("bassline").steps[0].compiledPitchV - (-1.5833333f)) < 1e-5f,
          "negative scale octave compiles with Euclidean degree semantics");

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
    expectInvalid(R"({"tracks":[{"id":"a","channel":0},{"id":"b","channel":0}]})", "tracks[1].channel", "duplicate channels are rejected");
    expectInvalid(R"({"tracks":[{"id":"a","channel":0}],"arrangement":[{"id":"s","lengthBeats":4,"repeats":1,"tracks":{"missing":null}}]})", "arrangement[0].tracks.missing", "scene track references are resolved");
    expectInvalid(R"({"tracks":[{"id":"a","channel":0}],"macros":{"1":{"target":"track.missing.gate","amount":1,"polarity":"unipolar"}}})", "macros.1.target", "macro track targets are resolved");
    expectInvalid(R"({"meta":{"bpm":"fast"}})", "meta.bpm", "invalid field types are rejected instead of defaulted");

    std::cout << "[SUMMARY] sibyl_json_spec: " << (failures ? "FAILED" : "passed") << "\n";
    return failures == 0 ? 0 : 1;
}
