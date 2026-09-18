#include "../src/SibylVoicing.hpp"
#include <random>
namespace {
void testVoicing() {
  using namespace sibyl;
  // Independent exhaustive oracle: score complete paths without prefix pruning.
  auto oracle = [](const std::vector<std::vector<VoicingCandidate>> &layers, const std::vector<VoicingVoice> &voices) {
    std::vector<std::array<int, 8>> path, best;
    std::array<int64_t, 5> bestCost{};
    bool found = false;
    std::function<void(size_t, int)> walk = [&](size_t n, int missing) {
      if (n == layers.size()) {
        std::array<int64_t, 5> cost{{missing, 0, 0, 0, 0}};
        for (size_t v = 0; v < voices.size(); ++v) {
          cost[4] += std::abs(2 * path[0][v] - voices[v].minimum - voices[v].maximum);
          for (size_t c = 1; c < path.size(); ++c) {
            int d = std::abs(path[c][v] - path[c - 1][v]);
            cost[1] = std::max(cost[1], int64_t(d));
            cost[2] += d;
            cost[3] += d * d;
          }
        }
        if (!found || cost < bestCost || (cost == bestCost && path < best)) {
          found = true;
          bestCost = cost;
          best = path;
        }
        return;
      }
      for (const auto &candidate : layers[n]) {
        path.push_back(candidate.pitches);
        walk(n + 1, missing + candidate.missing);
        path.pop_back();
      }
    };
    walk(0, 0);
    return best;
  };
  std::vector<VoicingVoice> voices(1);
  voices[0].minimum = 0;
  voices[0].maximum = 15;
  std::vector<std::vector<VoicingCandidate>> trap;
  for (const auto &values : std::vector<std::vector<int>>{{1}, {0, 1, 9, 14}, {4, 15}, {14}, {2}}) {
    std::vector<VoicingCandidate> layer;
    for (int p : values) {
      VoicingCandidate c;
      c.pitches[0] = p;
      layer.push_back(c);
    }
    trap.push_back(layer);
  }
  VoicingBudget budget;
  auto solved = solveVoicing(trap, voices, budget);
  check(solved.valid && solved.pitches == oracle(trap, voices) && solved.maximumLeap == 12 && solved.movement == 25,
        "W07 future large leap must not discard the globally optimal prefix");
  std::mt19937 random(42017);
  bool exact = true;
  for (int test = 0; test < 400; ++test) {
    voices.resize(1 + random() % 3);
    for (auto &v : voices) {
      v.minimum = -12;
      v.maximum = 24;
    }
    std::vector<std::vector<VoicingCandidate>> layers(2 + random() % 4);
    for (auto &layer : layers)
      for (unsigned j = 0, count = 1 + random() % 4; j < count; ++j) {
        VoicingCandidate c;
        for (size_t v = 0; v < voices.size(); ++v)
          c.pitches[v] = int(random() % 25) - 12;
        std::sort(c.pitches.begin(), c.pitches.begin() + voices.size());
        c.missing = random() % 3;
        layer.push_back(c);
      }
    VoicingBudget b;
    auto r = solveVoicing(layers, voices, b);
    exact &= r.valid && r.pitches == oracle(layers, voices);
  }
  check(exact, "P6 staged optimizer matches exhaustive complete-path oracle in 400 seeded cases");
  budget = VoicingBudget();
  budget.maxTransitions = 1;
  check(!solveVoicing(trap, voices, budget).valid && budget.exhausted && budget.transitions == 1,
        "W05 transition exhaustion is shared by solver passes and never truncates optimum");
  const char *fixture = R"({"schemaVersion":3,"tracks":[{"id":"lo","channel":0},{"id":"hi","channel":1}],
    "patterns":{},"arrangement":[{"id":"s","lengthBeats":4,"tracks":{}}],
    "harmony":{"progressions":{"changes":{"lengthBeats":4,"chords":[
      {"id":"c","beat":0,"root":"C3","intervals":[0,4,7]},
      {"id":"a","beat":2,"root":"A2","intervals":[0,3,7]}]}}}})";
  const char *operation = R"([{"op":"voice_progression","progression_id":"changes","scene_id":"s","resolution":"1/16",
    "voices":[{"track_id":"lo","pattern_id":"low","min":"C2","max":"C4"},
              {"track_id":"hi","pattern_id":"high","min":"C3","max":"C5"}]}])";
  auto base = parseCompositionJson(fixture, 1);
  check(base.valid, "P6 voicing fixture compiles");
  if (!base.valid)
    return;
  auto decode = [](const std::string &s) {
    json_error_t e{};
    return json_loads(s.c_str(), 0, &e);
  };
  auto edit = [&](const Composition &comp, json_t *ops) { return applyCompositionEdit(comp, ops, comp.revision + 1); };
  json_t *ops = decode(operation);
  auto result = edit(*base.composition, ops);
  if (!result.valid)
    std::cerr << "P6 error " << result.errorCode << " " << result.errorPath << " " << result.errorMessage << "\n";
  check(result.valid, "W01 control-side materialization succeeds");
  if (result.valid) {
    auto again = edit(*base.composition, ops);
    check(again.valid &&
              serializeFullCompositionJson(*again.composition) == serializeFullCompositionJson(*result.composition),
          "W01 deterministic generated notes and IDs");
    const auto &low = result.composition->patterns.at("low");
    const auto &high = result.composition->patterns.at("high");
    bool coverage = low.steps.size() == 2 && high.steps.size() == 2;
    for (size_t c = 0; c < 2 && coverage; ++c) {
      int l = std::lround(low.steps[c].compiledPitchV * 12), h = std::lround(high.steps[c].compiledPitchV * 12);
      int root = c ? 9 : 0, third = c ? 0 : 4;
      coverage &= l <= h && l >= -24 && l <= 0 && h >= -12 && h <= 12;
      coverage &= ((voicingPitchClass(l) == root && voicingPitchClass(h) == third) ||
                   (voicingPitchClass(h) == root && voicingPitchClass(l) == third));
    }
    check(coverage && low.length == 16 && std::abs(low.steps[0].gate - 7.6f) < 1e-5,
          "W01 noncrossing registers, root/third coverage, grid and gates");
    HarmonyProgression common;
    HarmonyChord c;
    c.id = "c";
    c.rootSemitone = -12;
    c.intervals = {0, 4, 7};
    common.chords.push_back(c);
    c.id = "a";
    c.rootSemitone = -15;
    c.intervals = {0, 3, 7};
    common.chords.push_back(c);
    std::vector<VoicingVoice> registers(3);
    registers[0].minimum = registers[0].maximum = -12;
    registers[1].minimum = registers[1].maximum = -8;
    registers[2].minimum = -5;
    registers[2].maximum = -3;
    VoicingBudget commonBudget;
    auto commonResult = voiceProgression(common, registers, commonBudget);
    check(commonResult.valid && commonResult.maximumLeap == 2 && commonResult.movement == 2,
          "W02 common C and E tones retained while G moves to A");
    check(!result.voicingChanges.empty() && result.voicingChanges[0].find("voice_progression_v1") != std::string::npos,
          "P6 versioned provenance report");
    auto conflict = edit(*result.composition, ops);
    check(!conflict.valid && conflict.errorCode == "object_in_use", "P6 createOnly collision rejected");
    json_object_set_new(json_array_get(ops, 0), "write_policy", json_string("replace"));
    auto replaced = edit(*result.composition, ops);
    check(replaced.valid && replaced.composition->patterns.at("low").steps[0].id == low.steps[0].id &&
              replaced.voicingChanges[0].find("affectedReferencesTotal\":2") != std::string::npos,
          "P6 replace reuses IDs and reports affected assignments");
    json_t *sharedRoot = decode(serializeFullCompositionJson(*result.composition));
    json_t *sharedScenes = json_object_get(json_object_get(sharedRoot, "composition"), "arrangement");
    json_array_append_new(
        sharedScenes,
        decode(
            R"({"id":"other","lengthBeats":4,"tracks":{"lo":{"pattern":"low","phaseMode":"continue","overrides":{"transposeSemitones":12}}}})"));
    auto sharedBase = parseCompositionJson(harmony_json::dump(sharedRoot), 2);
    json_decref(sharedRoot);
    auto sharedReplace = edit(*sharedBase.composition, ops);
    json_t *sharedBefore = decode(serializeFullCompositionJson(*sharedBase.composition));
    json_t *sharedAfter =
        sharedReplace.valid ? decode(serializeFullCompositionJson(*sharedReplace.composition)) : nullptr;
    check(sharedReplace.valid &&
              json_equal(
                  json_array_get(json_object_get(json_object_get(sharedBefore, "composition"), "arrangement"), 1),
                  json_array_get(json_object_get(json_object_get(sharedAfter, "composition"), "arrangement"), 1)) &&
              sharedReplace.voicingChanges[0].find("affectedReferencesTotal\":3") != std::string::npos,
          "P6 replacement reports shared bank references and preserves other scene assignment attributes");
    json_decref(sharedBefore);
    json_decref(sharedAfter);
    json_t *change = decode(
        R"([{"op":"upsert_progression","id":"changes","progression":{"lengthBeats":4,"chords":[{"id":"d","beat":0,"root":"D3","intervals":[0,4,7]}]}}])");
    auto changed = edit(*result.composition, change);
    json_decref(change);
    check(changed.valid &&
              changed.composition->patterns.at("low").steps[0].compiledPitchV == low.steps[0].compiledPitchV,
          "W06 harmony edits leave materialized fixed pitches unchanged");
  }
  json_decref(ops);
  ops = decode(operation);
  json_t *voice = json_array_get(json_object_get(json_array_get(ops, 0), "voices"), 0);
  json_object_set_new(voice, "min", json_string("C#3"));
  json_object_set_new(voice, "max", json_string("C#3"));
  auto impossible = edit(*base.composition, ops);
  check(!impossible.valid && impossible.errorCode == "voicing_unsatisfiable" && base.composition->patterns.empty(),
        "W03 unsatisfiable registers leave accepted composition untouched");
  json_decref(ops);
  ops = decode(operation);
  json_object_set_new(json_array_get(ops, 0), "resolution", json_string("1/1d"));
  auto offgrid = edit(*base.composition, ops);
  check(!offgrid.valid && offgrid.errorCode == "off_grid_harmony",
        "W04 off-grid progression rejected without quantization");
  json_decref(ops);
  for (const char *patch : {R"({"resolution":"1/3"})", R"({"gate_ratio":1.01})", R"({"gate_ratio":null})",
                            R"({"write_policy":"overwrite"})", R"({"voices":[]})", R"({"unexpected":true})"}) {
    ops = decode(operation);
    json_t *fields = decode(patch);
    json_object_update(json_array_get(ops, 0), fields);
    json_decref(fields);
    auto invalid = edit(*base.composition, ops);
    json_decref(ops);
    check(!invalid.valid && !invalid.errorCode.empty(), "P6 malformed voicing request returns structured failure");
  }
  for (int variant = 0; variant < 3; ++variant) {
    json_t *changedRoot = decode(fixture);
    json_t *prog = json_object_get(json_object_get(json_object_get(changedRoot, "harmony"), "progressions"), "changes");
    if (variant == 0)
      json_object_set_new(json_array_get(json_object_get(prog, "chords"), 1), "beat", json_real(2.01));
    if (variant == 1) {
      json_object_set_new(prog, "lengthBeats", json_integer(257));
      json_object_set_new(json_array_get(json_object_get(changedRoot, "arrangement"), 0), "lengthBeats",
                          json_integer(257));
    }
    if (variant == 2)
      json_object_set_new(json_array_get(json_object_get(changedRoot, "arrangement"), 0), "lengthBeats",
                          json_integer(8));
    auto changedBase = parseCompositionJson(harmony_json::dump(changedRoot), 1);
    json_decref(changedRoot);
    ops = decode(operation);
    auto invalid = edit(*changedBase.composition, ops);
    json_decref(ops);
    check(!invalid.valid && invalid.errorCode == "off_grid_harmony",
          "W04 off-grid marker, excess pattern steps and mismatched scene length reject atomically");
  }
  // Failure after a successful voicing operation must roll back every generated pattern.
  ops = decode(operation);
  json_array_append_new(ops, decode(R"({"op":"delete_pattern","id":"does_not_exist"})"));
  auto rollback = edit(*base.composition, ops);
  json_decref(ops);
  check(!rollback.valid && !rollback.composition && base.composition->patterns.empty(),
        "P6 later-operation failure rolls back all materialized notes");
  // All passes and all operations share the published request limits.
  json_t *root = decode(fixture);
  json_t *chords = json_array();
  for (int i = 0; i < 128; ++i)
    json_array_append_new(chords, json_pack("{s:s,s:i,s:s,s:[i]}", "id", std::to_string(i).c_str(), "beat", i, "root",
                                            "C4", "intervals", 0));
  json_object_set_new(json_object_get(json_object_get(json_object_get(root, "harmony"), "progressions"), "changes"),
                      "chords", chords);
  json_object_set_new(json_object_get(json_object_get(json_object_get(root, "harmony"), "progressions"), "changes"),
                      "lengthBeats", json_integer(128));
  json_object_set_new(json_array_get(json_object_get(root, "arrangement"), 0), "lengthBeats", json_integer(128));
  json_t *tracks = json_array();
  json_t *voiceList = json_array();
  for (int i = 0; i < 8; ++i) {
    std::string id = "v" + std::to_string(i);
    json_array_append_new(tracks, json_pack("{s:s,s:i}", "id", id.c_str(), "channel", i));
    json_array_append_new(voiceList, json_pack("{s:s,s:s,s:s,s:s}", "track_id", id.c_str(), "pattern_id", id.c_str(),
                                               "min", "C4", "max", "C4"));
  }
  json_object_set_new(root, "tracks", tracks);
  auto large = parseCompositionJson(harmony_json::dump(root), 1);
  json_decref(root);
  ops = decode(operation);
  json_t *op = json_array_get(ops, 0);
  json_object_set_new(op, "voices", voiceList);
  json_object_set_new(op, "resolution", json_string("1/4"));
  json_object_set_new(op, "write_policy", json_string("replace"));
  for (int i = 1; i < 180; ++i)
    json_array_append(ops, op);
  auto requestCap = edit(*large.composition, ops);
  json_decref(ops);
  check(!requestCap.valid && requestCap.errorCode == "capacity_exceeded" && requestCap.voicingPartials == 200000,
        "W05 200000 partial limit is cumulative across a multi-operation transaction");
  std::vector<std::vector<VoicingCandidate>> dense(2);
  for (auto &layer : dense)
    for (int i = 0; i < 2048; ++i) {
      VoicingCandidate candidate;
      candidate.pitches[0] = i % 121;
      layer.push_back(candidate);
    }
  budget = VoicingBudget();
  voices.resize(1);
  auto transitionCap = solveVoicing(dense, voices, budget);
  check(!transitionCap.valid && transitionCap.code == "capacity_exceeded" && budget.transitions == 2000000,
        "W05 actual two-million transition limit is enforced without returning an approximate answer");
  HarmonyProgression progression;
  HarmonyChord chord;
  chord.id = "wide";
  chord.intervals = {0, 4, 7};
  progression.chords.push_back(chord);
  voices.resize(8);
  for (auto &v : voices) {
    v.minimum = -120;
    v.maximum = 120;
  }
  budget = VoicingBudget();
  auto cap = voiceProgression(progression, voices, budget);
  check(!cap.valid && cap.code == "capacity_exceeded", "W05 real candidate cap rejects excessive search");
  budget = VoicingBudget();
  budget.maxPartials = 4;
  cap = voiceProgression(progression, voices, budget);
  check(!cap.valid && cap.code == "capacity_exceeded" && budget.partials == 4,
        "W05 visited partial cap is enforced exactly");
}
} // namespace
