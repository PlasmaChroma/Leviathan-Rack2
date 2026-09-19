#pragma once
#include "../src/SibylHarmonyView.hpp"
#include "../src/SibylTuningCatalog.hpp"
#include "../src/SibylVoicing.hpp"
#include <random>

#include "sibyl_native_fixture.hpp"
inline void nativePitchCases() {
  using namespace sibyl;
  for (int n : {38, 53}) {
    auto parsed = parseCompositionJson(nativeHarmonyFixture(n), 1);
    if (!parsed.valid)
      for (const auto &e : parsed.errors)
        std::cerr << e.path << ": " << e.message << "\n";
    assert(parsed.valid);
    auto b = parsed.composition;
    const auto &p = b->patterns.at("p");
    const auto &a = b->arrangement[0].tracks.at("v");
    assert(std::abs(sceneEventPitch(*b, p.steps[0], a, 0, 0, 0) - 17. / n) < 1e-6);
    assert(std::abs(sceneEventPitch(*b, p.steps[1], a, 0, 0, 0) - (31. / n - 1)) < 1e-6);
    auto round = parseCompositionJson(serializeFullCompositionJson(*b), 1);
    assert(round.valid);
    json_t *before = decode(serializeFullCompositionJson(*b).c_str());
    json_t *after = decode(serializeFullCompositionJson(*round.composition).c_str());
    assert(json_equal(before, after));
    json_decref(before);
    json_decref(after);
    auto shifted = edit(
        *b,
        R"([{"op":"update_scene_assignment","scene_id":"s","track_id":"v","set":{"overrides":{"transposeSteps":1,"transposePeriods":1,"transposeCents":-1.5}}}])");
    assert(shifted.valid);
    assert(std::abs(sceneEventPitch(*shifted.composition, p.steps[0],
                                    shifted.composition->arrangement[0].tracks.at("v"), 0, 0, 0) -
                    (18. / n + 1 - 1.5 / 1200)) < 1e-6);
    assert(changedTrackChannelMask(*b, *shifted.composition) == 1);
    auto voiced = edit(
        *b,
        R"([{"op":"voice_progression","progression_id":"h","scene_id":"s","resolution":"1/4","write_policy":"replace","voices":[{"track_id":"b","pattern_id":"bass","min":{"pitchV":-1},"max":{"pitchV":0}},{"track_id":"v","pattern_id":"upper","min":{"pitchV":0},"max":{"pitchV":1}}]}])");
    if (!voiced.valid)
      std::cerr << voiced.errorCode << ": " << voiced.errorMessage << "\n";
    assert(voiced.valid && voiced.voicingChanges[0].find("voice_progression_micro_v1") != std::string::npos);
    for (const auto &note : voiced.composition->patterns.at("upper").steps)
      assert(note.pitchType == PitchType::PITCH_V);
    auto tuneChanged = edit(
        *voiced.composition,
        R"([{"op":"upsert_tuning","id":"t","tuning":{"kind":"equal","divisions":72,"period":{"ratio":"2/1"}}}])");
    assert(tuneChanged.valid);
    assert(tuneChanged.composition->patterns.at("upper").steps[0].compiledPitchV ==
           voiced.composition->patterns.at("upper").steps[0].compiledPitchV);
    auto invalid = edit(
        *b,
        R"([{"op":"update_notes","pattern_id":"p","selector":{"steps":[0]},"set":{"harmonic":{"kind":"tone","role":"absent"}}}])");
    assert(!invalid.valid);
    auto offgrid = edit(
        *b,
        R"([{"op":"upsert_progression","id":"h","progression":{"pitchContext":"c","lengthBeats":4,"chords":[{"id":"I","beat":0,"rootPitch":{"tuned":{"ratio":"1/1"}},"tones":[{"id":"r","interval":{"ratio":"1/1"},"roles":["root","third"]},{"id":"f","interval":{"ratio":"3/2"},"roles":["fifth"]}]}]}},{"op":"update_scene_assignment","scene_id":"s","track_id":"v","set":{"overrides":{"transposeSteps":0}}}])");
    assert(!offgrid.valid && offgrid.errorCode == "unsupported_pitch_transform");
    auto copied = edit(
        *b,
        R"([{"op":"duplicate_notes","pattern_id":"p","destination_pattern_id":"dest","selector":{"steps":[2]},"offset_steps":0}])");
    assert(copied.valid &&
           copied.composition->patterns.at("dest").steps[0].harmonic.find("context") != std::string::npos);
    std::string old = nativeHarmonyFixture(n);
    old.replace(old.find("\"schemaVersion\":4"), 17, "\"schemaVersion\":3");
    auto gated = parseCompositionJson(old, 1);
    assert(!gated.valid && gated.errors.front().code == "schema_version_required");
  }
  auto base = parseCompositionJson(nativeHarmonyFixture(), 1);
  assert(base.valid);
  auto badTie = edit(*base.composition, R"([{"op":"update_notes","pattern_id":"p","selector":{"steps":[2]},"set":{"harmonic":{"kind":"nearest","reference":{"tuned":{"step":1}},"range":{"min":"C3","max":"C5"},"tieBreak":7}}}])");
  assert(!badTie.valid && badTie.errorCode == "invalid_harmony");
  {
    VoicingVoice voice; voice.minimumV=-1.;voice.maximumV=1.;
    voice.minimum=-1200000;voice.maximum=1200000;
    VoicingBudget budget;budget.maxCandidates=1;
    auto limited=voiceNativeProgression(base.composition->progressions[0],{voice},budget);
    assert(!limited.valid && limited.code=="capacity_exceeded");
  }
  {
    json_t *fixture = decode(nativeHarmonyFixture().c_str());
    json_object_set_new(
        json_object_get(json_object_get(fixture, "pitchSystems"), "tunings"), "t",
        decode(
            R"({"kind":"table","period":{"ratio":"3/1"},"positions":[{"ratio":"1/1"},{"cents":200},{"cents":700}]})"));
    json_t *progression =
        json_object_get(json_object_get(json_object_get(fixture, "harmony"), "progressions"), "h");
    json_object_set_new(
        progression, "chords",
        decode(
            R"([{"id":"u","beat":0,"rootPitch":{"tuned":{"step":0}},"tones":[{"id":"r","interval":{"steps":0},"roles":["root"]},{"id":"m","interval":{"steps":1},"roles":["third"]},{"id":"f","interval":{"steps":2},"roles":["fifth"]}]}])"));
    json_object_set_new(
        json_object_get(json_object_get(fixture, "patterns"), "p"), "steps",
        decode(
            R"([{"step":0,"harmonic":{"kind":"tone","role":"third","periods":1},"transposeSteps":1},{"step":1,"harmonic":{"kind":"nearest","reference":{"tuned":{"cents":1900}},"range":{"min":{"pitchV":1.5},"max":{"pitchV":1.6}}}}])"));
    auto parsed = parseCompositionJson(harmony_json::dump(fixture), 1);
    json_decref(fixture);
    assert(parsed.valid);
    const auto &c = *parsed.composition;
    const auto &a = c.arrangement[0].tracks.at("v");
    assert(std::abs(sceneEventPitch(c, c.patterns.at("p").steps[0], a, 0, 0, 0) -
                    (std::log2(3.) + 700. / 1200)) < 1e-6);
    assert(std::abs(sceneEventPitch(c, c.patterns.at("p").steps[1], a, 0, 0, 0) - std::log2(3.)) < 1e-6);
    auto shifted = edit(
        c,
        R"([{"op":"update_scene_assignment","scene_id":"s","track_id":"v","set":{"overrides":{"transposeSteps":1}}}])");
    assert(shifted.valid);
    assert(std::abs(sceneEventPitch(*shifted.composition, shifted.composition->patterns.at("p").steps[0],
                                    shifted.composition->arrangement[0].tracks.at("v"), 0, 0, 0) -
                    2 * std::log2(3.)) < 1e-6);
    auto before = serializeFullCompositionJson(c);
    auto bad = edit(c, R"([{"op":"import_tuning_scl","id":"bad","text":"broken\n1\n-7.0\n"}])");
    assert(!bad.valid && serializeFullCompositionJson(c) == before);
  }
  json_t *request = decode(R"({"view":"tuning_catalog"})");
  auto catalog = serializeTuningAssistance(*base.composition, request);
  json_decref(request);
  assert(catalog.find("53edo") != std::string::npos && catalog.find("13edt") != std::string::npos);
  request = decode(
      R"({"view":"map_intervals","context_id":"c","intervals":[{"ratio":"1/1"},{"ratio":"5/4"},{"ratio":"3/2"}]})");
  json_t *mapped = decode(serializeTuningAssistance(*base.composition, request).c_str());
  json_decref(request);
  assert(json_integer_value(
             json_object_get(json_array_get(json_object_get(mapped, "intervals"), 2), "step")) == 31);
  json_decref(mapped);
  request = decode(R"({"view":"map_intervals","context_id":"c","intervals":[{"cents":0},{"cents":1}]})");
  assert(serializeTuningAssistance(*base.composition, request).find("scale_mapping_collision") !=
         std::string::npos);
  json_decref(request);
  for (int n : {1, 38, 53, 1024}) {
    PitchTuning tuning;
    tuning.divisions = n;
    json_t *definition = decode(R"({"kind":"equal","period":{"ratio":"2/1"}})");
    auto text = tuning_json::exportScala(definition, tuning);
    json_decref(definition);
    ParseResult r;
    json_t *imported = tuning_json::importScala(text, r, "text");
    assert(imported && json_array_size(json_object_get(imported, "positions")) == size_t(n));
    for (int k = 1; k < n; ++k)
      assert(std::abs(json_number_value(json_object_get(
                          json_array_get(json_object_get(imported, "positions"), k), "cents")) /
                          1200. -
                      double(k) / n) < 1e-12);
    json_decref(imported);
  }
  for (const auto &text : std::vector<std::string>{"! comment\r\n\r\n3\r\n100.0 cents\r\n3/2 E\r\n3\r\n",
                                                   "singleton\n1\n2/1\n"}) {
    ParseResult r;
    json_t *imported = tuning_json::importScala(text, r, "text");
    assert(imported);
    json_decref(imported);
  }
  for (const auto &text : std::vector<std::string>{"negative\n1\n-5.0\n", "zero\n0\n",
                                                   "duplicate\n2\n2/1\n2/1\n", "order\n2\n700.\n600.\n"}) {
    ParseResult r;
    assert(!tuning_json::importScala(text, r, "text") && r.errors[0].code == "unsupported_tuning_shape");
  }
  for (const auto &text : std::vector<std::string>{"bad\n1\n0/1\n", "bad\n1\nabc\n", "bad\n2\n100.0\n"}) {
    ParseResult r;
    assert(!tuning_json::importScala(text, r, "text") && r.errors[0].code == "invalid_scala");
  }
  auto imported = edit(*base.composition,
                       R"([{"op":"import_tuning_scl","id":"tritave","text":"three\n3\n3/2\n2\n3\n"}])");
  assert(imported.valid &&
         std::abs(imported.composition->pitchSystems.tunings.at("tritave").periodV - std::log2(3.)) < 1e-12);
  // Independent full-path oracle: scores milli-cents, preserves precise candidate identity.
  std::mt19937 rng(7353);
  for (int trial = 0; trial < 400; ++trial) {
    std::vector<VoicingVoice> voices(2);
    for (auto &v : voices) {
      v.minimum = -1200000;
      v.maximum = 1200000;
    }
    std::vector<std::vector<VoicingCandidate>> layers(3);
    for (auto &layer : layers)
      for (int k = 0; k < 3; ++k) {
        VoicingCandidate c;
        c.native = true;
        c.missing = int(rng() % 3);
        for (int v = 0; v < 2; ++v) {
          c.pitches[v] = int(rng() % 4000001) - 2000000;
          c.volts[v] = c.pitches[v] / 1200000.;
          c.toneRanks[v] = uint8_t(k);
        }
        layer.push_back(c);
      }
    std::array<int64_t, 5> best;
    best.fill(INT64_MAX);
    std::vector<VoicingCandidate> bestPath;
    for (const auto &a : layers[0])
      for (const auto &b : layers[1])
        for (const auto &c : layers[2]) {
          std::array<int64_t, 5> cost{{a.missing + b.missing + c.missing, 0, 0, 0, 0}};
          std::vector<VoicingCandidate> path{a, b, c};
          for (int v = 0; v < 2; ++v) {
            cost[4] += std::abs(int64_t(2) * a.pitches[v]);
            for (int t = 1; t < 3; ++t) {
              int64_t d = std::abs(int64_t(path[t].pitches[v]) - path[t - 1].pitches[v]);
              cost[1] = std::max(cost[1], d);
              cost[2] += d;
              cost[3] += d * d;
            }
          }
          if (cost < best ||
              (cost == best && std::lexicographical_compare(path.begin(), path.end(), bestPath.begin(),
                                                            bestPath.end(), voicingLess))) {
            best = cost;
            bestPath = path;
          }
        }
    VoicingBudget budget;
    auto solution = solveVoicing(layers, voices, budget);
    assert(solution.valid);
    assert(solution.missing == best[0] && solution.maximumLeap == best[1] && solution.movement == best[2] &&
           solution.squaredMovement == best[3] && solution.initialDisplacementTwice == best[4]);
    for (int i = 0; i < 3; ++i)
      assert(solution.selected[i].volts == bestPath[i].volts);
  }
  assert(voicingTick(.5 / 1200000.) == 0 && voicingTick(-.5 / 1200000.) == -1);
  std::cout << "PASS: P7C/D native harmony, materialization, Scala, mapping and 400-path oracle cases\n";
}
